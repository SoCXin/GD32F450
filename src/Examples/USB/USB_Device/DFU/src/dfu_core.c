/*!
    \file  dfu_core.c
    \brief USB DFU device class core functions

    \version 2016-08-15, V1.0.0, firmware for GD32F4xx
    \version 2018-12-12, V2.0.0, firmware for GD32F4xx
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "dfu_core.h"
#include "usb_delay.h"
#include "usbd_int.h"
#include "dfu_mal.h"

#define USBD_VID              0x28e9
#define USBD_PID              0x0189

/* DFU Requests management functions */
static void DFU_Req_DETACH    (void *pudev, usb_device_req_struct *req);
static void DFU_Req_DNLOAD    (void *pudev, usb_device_req_struct *req);
static void DFU_Req_UPLOAD    (void *pudev, usb_device_req_struct *req);
static void DFU_Req_GETSTATUS (void *pudev);
static void DFU_Req_CLRSTATUS (void *pudev);
static void DFU_Req_GETSTATE  (void *pudev);
static void DFU_Req_ABORT     (void *pudev);
static void DFU_LeaveDFUMode  (void *pudev);

static uint8_t  dfu_getstatus_complete (void *pudev);

/* data management variables */
extern const uint8_t* USBD_DFU_StringDesc[];
extern uint8_t MAL_Buffer[];
#define FLASH_IF_STRING       "@Internal Flash   /0x08000000/16*002Ka,112*002Kg"

/* State Machine variables */
uint8_t DeviceState = STATE_dfuIDLE;
uint8_t DeviceStatus[6] =
{
    STATUS_OK,     /* bStatus: device status is OK */
    0x00,          /* bwPollTimeout: 0ms */
    0x00,
    0x00,
    STATE_dfuIDLE, /* bState: device state is dfuIDLE */
    0x00           /* iString: index is 0 */
};

uint32_t Manifest_State = MANIFEST_COMPLETE;

/* Data Management variables */
static uint16_t BlockNum = 0;
static uint16_t Length = 0;
static uint32_t BaseAddress = APP_LOADED_ADDR;  /*!< User application base address to erase, program or read */
static __IO uint32_t USBD_DFU_AltSet = 0;

usbd_int_cb_struct *usbd_int_fops = NULL;

/* note:it should use the c99 standard when compiling the below codes */
/* USB standard device descriptor */
const usb_descriptor_device_struct device_descripter =
{
    .Header = 
     {
         .bLength = USB_DEVICE_DESC_SIZE, 
         .bDescriptorType = USB_DESCTYPE_DEVICE
     },
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = USB_MAX_EP0_SIZE,
    .idVendor = USBD_VID,
    .idProduct = USBD_PID,
    .bcdDevice = 0x0100,
    .iManufacturer = USBD_MFC_STR_IDX,
    .iProduct = USBD_PRODUCT_STR_IDX,
    .iSerialNumber = USBD_SERIAL_STR_IDX,
    .bNumberConfigurations = USBD_CFG_MAX_NUM
};

/* USB device configuration descriptor */
const usb_descriptor_configuration_set_struct configuration_descriptor = 
{
    .Config = 
    {
        .Header = 
         {
             .bLength = sizeof(usb_descriptor_configuration_struct), 
             .bDescriptorType = USB_DESCTYPE_CONFIGURATION 
         },
        .wTotalLength = USB_DFU_CONFIG_DESC_SIZE,
        .bNumInterfaces = 0x01,
        .bConfigurationValue = 0x01,
        .iConfiguration = 0x00,
        .bmAttributes = 0x80,
        .bMaxPower = 0x32
    },

    .DFU_Interface = 
    {
        .Header = 
         {
             .bLength = sizeof(usb_descriptor_interface_struct), 
             .bDescriptorType = USB_DESCTYPE_INTERFACE 
         },
        .bInterfaceNumber = 0x00,
        .bAlternateSetting = 0x00,
        .bNumEndpoints = 0x00,
        .bInterfaceClass = 0xFE,
        .bInterfaceSubClass = 0x01,
        .bInterfaceProtocol = 0x02,
        .iInterface = 0x00
    },

    .DFU_Function_Desc = 
    {
        .Header = 
         {
            .bLength = sizeof(usb_dfu_function_descriptor_struct), 
            .bDescriptorType = DFU_DESC_TYPE 
         },
        .bmAttributes = 0x0B,
        .wDetachTimeOut = 0x00FF,
        .wTransferSize = TRANSFER_SIZE,
        .bcdDFUVersion = 0x011A,
    },
};

/* USB language ID Descriptor */
const usb_descriptor_language_id_struct usbd_language_id_desc = 
{
    .Header = 
     {
         .bLength = sizeof(usb_descriptor_language_id_struct), 
         .bDescriptorType = USB_DESCTYPE_STRING
     },
    .wLANGID = ENG_LANGID
};

/* USB serial string */
uint8_t usbd_serial_string[USB_SERIAL_STRING_SIZE] =
{
    USB_SERIAL_STRING_SIZE,
    USB_DESCTYPE_STRING,
};

uint8_t* usbd_strings[] = 
{
    [USBD_LANGID_STR_IDX] = (uint8_t *)&usbd_language_id_desc,
    [USBD_MFC_STR_IDX] = USBD_STRING_DESC("GigaDevice"),
    [USBD_PRODUCT_STR_IDX] = USBD_STRING_DESC("GD32 USB DFU in FS Mode"),
    [USBD_SERIAL_STR_IDX] = usbd_serial_string,
    [USBD_CONFIG_STR_IDX] = USBD_STRING_DESC("GD32 USB CONFIG"),
    [USBD_INTERFACE_STR_IDX] = USBD_STRING_DESC(FLASH_IF_STRING)
};

/*!
    \brief      initialize the MSC device
    \param[in]  pudev: pointer to usb device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     usb device operation status
*/
uint8_t dfu_init (void *pudev, uint8_t config_index)
{
    /* Initilialize the MAL(Media Access Layer) */
    DFU_MAL_Init();

    return USBD_OK;
}

/*!
    \brief      de-initialize the MSC device
    \param[in]  pudev: pointer to usb device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     usb device operation status
*/
uint8_t  dfu_deinit (void *pudev, uint8_t config_index)
{
    /* Restore default state */
    DeviceState = STATE_dfuIDLE;
    DeviceStatus[0] = STATUS_OK;
    DeviceStatus[4] = DeviceState;
    BlockNum = 0;
    Length = 0;

    /* DeInitilialize the MAL(Media Access Layer) */
    DFU_MAL_DeInit();

    return USBD_OK;
}

/*!
    \brief      handle the MSC class-specific requests
    \param[in]  pudev: pointer to usb device instance
    \param[in]  req: device class-specific request
    \param[out] none
    \retval     usb device operation status
*/
uint8_t dfu_req_handler (void *pudev, usb_device_req_struct *req)
{
    uint16_t len = 0;
    uint8_t *pbuf = NULL;

    switch (req->bmRequestType & USB_REQ_MASK) {
    case USB_CLASS_REQ:
        switch (req->bRequest)
        {
            case DFU_DNLOAD:
                DFU_Req_DNLOAD(pudev, req);
                break;

            case DFU_UPLOAD:
                DFU_Req_UPLOAD(pudev, req);
                break;

            case DFU_GETSTATUS:
                DFU_Req_GETSTATUS(pudev);
                break;

            case DFU_CLRSTATUS:
                DFU_Req_CLRSTATUS(pudev);
                break;

            case DFU_GETSTATE:
                DFU_Req_GETSTATE(pudev);
                break;

            case DFU_ABORT:
                DFU_Req_ABORT(pudev);
                break;

            case DFU_DETACH:
                DFU_Req_DETACH(pudev, req);
                break;

            default:
                usbd_enum_error(pudev, req);
                return USBD_FAIL;
        }
        break;
    case USB_STANDARD_REQ:
        /* standard device request */
        switch (req->bRequest) {
        case USBREQ_GET_DESCRIPTOR:
            if((req->wValue >> 8) == DFU_DESC_TYPE)
            {
                pbuf = (uint8_t*)(&configuration_descriptor.DFU_Function_Desc);
                len = USB_MIN(USB_DFU_DESC_SIZE, req->wLength);
            }

            usbd_ctltx (pudev, pbuf, len);
            break;
        case USBREQ_GET_INTERFACE:
            usbd_ctltx (pudev, (uint8_t *)&USBD_DFU_AltSet, 1);
            break;
        case USBREQ_SET_INTERFACE:
            USBD_DFU_AltSet = (uint8_t)(req->wValue);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    return USBD_OK;
}

/*!
    \brief      handle data Stage
    \param[in]  pudev: pointer to usb device instance
    \param[in]  rx_tx: the flag of Rx or Tx
    \param[in]  ep_id: the endpoint ID
    \param[out] none
    \retval     usb device operation status
*/
uint8_t dfu_data_handler (void *pudev, usb_dir_enum rx_tx, uint8_t ep_num)
{
    if ((USB_TX == rx_tx) && ((DFU_IN_EP & 0x7F) == ep_num)) {
        dfu_getstatus_complete(pudev);

        return USBD_OK;
    } else if ((USB_RX == rx_tx) && (DFU_OUT_EP == ep_num)) {
        return USBD_OK;
    } else {
        return USBD_FAIL;
    }
}

/*!
    \brief      handle data IN stage in control endpoint 0
    \param[in]  pudev: pointer to usb device instance
    \param[out] none
    \retval     usb device operation status
  */
static uint8_t  dfu_getstatus_complete (void *pudev)
{
    uint32_t Addr;
    usb_device_req_struct req;

    if (DeviceState == STATE_dfuDNBUSY)
    {
        /* Decode the Special Command*/
        if (BlockNum == 0)
        {
            if ((MAL_Buffer[0] == GET_COMMANDS) && (Length == 1))
            {}
            else if ((MAL_Buffer[0] == SET_ADDRESS_POINTER) && (Length == 5))
            {
                BaseAddress  = MAL_Buffer[1];
                BaseAddress += MAL_Buffer[2] << 8;
                BaseAddress += MAL_Buffer[3] << 16;
                BaseAddress += MAL_Buffer[4] << 24;
            }
            else if ((MAL_Buffer[0] == ERASE) && (Length == 5))
            {
                BaseAddress  = MAL_Buffer[1];
                BaseAddress += MAL_Buffer[2] << 8;
                BaseAddress += MAL_Buffer[3] << 16;
                BaseAddress += MAL_Buffer[4] << 24;
                DFU_MAL_Erase(BaseAddress);
            }
            else
            {
                /* Call the error management function (command will be NACKed) */
                req.bmRequestType = 0;
                req.wLength = 1;
                usbd_enum_error (pudev, &req);
            }
        }
        /* Regular Download Command */
        else if (BlockNum > 1)
        {
            /* Decode the required address */
            Addr = ((BlockNum - 2) * TRANSFER_SIZE) + BaseAddress;

            /* Preform the write operation */
            DFU_MAL_Write(Addr, Length);
            BlockNum = 0;
        }

        Length = 0;

        /* Update the state machine */
        DeviceState =  STATE_dfuDNLOAD_SYNC;
        DeviceStatus[4] = DeviceState;
        DeviceStatus[1] = 0;
        DeviceStatus[2] = 0;
        DeviceStatus[3] = 0;

        return USBD_OK;
    }
    else if (DeviceState == STATE_dfuMANIFEST)/* Manifestation in progress*/
    {
        /* Start leaving DFU mode */
        DFU_LeaveDFUMode(pudev);
    }

    return USBD_OK;
}

/**
  * @brief  Handle the DFU_DETACH request
  * @param  pudev: pointer to usb device instance
  * @param  req: DFU class request
  * @retval None.
  */
static void DFU_Req_DETACH(void *pudev, usb_device_req_struct *req)
{
    switch(DeviceState)
    {
        case STATE_dfuIDLE:
        case STATE_dfuDNLOAD_SYNC:
        case STATE_dfuDNLOAD_IDLE:
        case STATE_dfuMANIFEST_SYNC:
        case STATE_dfuUPLOAD_IDLE:
            /* Update the state machine */
            DeviceState = STATE_dfuIDLE;
            DeviceStatus[0] = STATUS_OK;
            DeviceStatus[1] = 0;
            DeviceStatus[2] = 0;
            DeviceStatus[3] = 0; /* bwPollTimeout = 0ms */
            DeviceStatus[4] = DeviceState;
            DeviceStatus[5] = 0; /* iString */
            BlockNum = 0;
            Length = 0;
            break;

        default:
            break;
    }

    /* Check the detach capability in the DFU functional descriptor */
    if (configuration_descriptor.DFU_Function_Desc.wDetachTimeOut & DFU_DETACH_MASK)
    {
        /* Perform an Attach-Detach operation on USB bus */
        /* perform an unconnected operation on USB bus */
        USB_SOFT_DISCONNECT_ENABLE();

        /* perform an connected operation on USB bus */
        USB_SOFT_DISCONNECT_DISABLE();
    }
    else
    {
        /* Wait for the period of time specified in detach request */
        usb_mdelay (req->wValue);
    }
}

/**
  * @brief  Handle the DFU_DNLOAD request
  * @param  pudev: pointer to usb device instance
  * @param  req: DFU class request
  * @retval None
  */
static void DFU_Req_DNLOAD(void *pudev, usb_device_req_struct *req)
{
    /* Data setup request */
    if (req->wLength > 0)
    {
        if ((DeviceState == STATE_dfuIDLE) || (DeviceState == STATE_dfuDNLOAD_IDLE))
        {
            /* Update the global length and block number */
            BlockNum = req->wValue;
            Length = req->wLength;

            /* Update the state machine */
            DeviceState = STATE_dfuDNLOAD_SYNC;
            DeviceStatus[4] = DeviceState;

            /* Prepare the reception of the buffer over EP0 */
            usbd_ctlrx (pudev,
                        (uint8_t*)MAL_Buffer,
                        Length);
        }
        /* Unsupported state */
        else
        {
            /* Call the error management function (command will be nacked */
            usbd_enum_error (pudev, req);
        }
    }
    /* 0 Data DNLOAD request */
    else
    {
        /* End of DNLOAD operation*/
        if (DeviceState == STATE_dfuDNLOAD_IDLE || DeviceState == STATE_dfuIDLE)
        {
            Manifest_State = MANIFEST_IN_PROGRESS;
            DeviceState = STATE_dfuMANIFEST_SYNC;
            DeviceStatus[1] = 0;
            DeviceStatus[2] = 0;
            DeviceStatus[3] = 0;
            DeviceStatus[4] = DeviceState;
        }
        else
        {
            usbd_enum_error (pudev, req);
        }
    }
}

/**
  * @brief  Handles the DFU UPLOAD request.
  * @param  pudev: pointer to usb device instance
  * @param  req: DFU class request
  * @retval None
  */
static void DFU_Req_UPLOAD(void *pudev, usb_device_req_struct *req)
{
    uint8_t *Phy_Addr = NULL;
    uint32_t Addr = 0;

    /* Data setup request */
    if (req->wLength > 0)
    {
        if ((DeviceState == STATE_dfuIDLE) || (DeviceState == STATE_dfuUPLOAD_IDLE))
        {
            /* Update the global langth and block number */
            BlockNum = req->wValue;
            Length = req->wLength;

            /* Set device poll timeout */
            DeviceStatus[1] = 0;
            DeviceStatus[2] = 0;
            DeviceStatus[3] = 0;

            /* DFU Get Command */
            if (BlockNum == 0)
            {
                /* Update the state machine */
                DeviceState = (Length > 3)? STATE_dfuIDLE : STATE_dfuUPLOAD_IDLE;
                DeviceStatus[4] = DeviceState;

                /* Store the values of all supported commands */
                MAL_Buffer[0] = GET_COMMANDS;
                MAL_Buffer[1] = SET_ADDRESS_POINTER;
                MAL_Buffer[2] = ERASE;

                /* Send the status data over EP0 */
                usbd_ctltx (pudev,
                            (uint8_t *)(&(MAL_Buffer[0])),
                            3);
            }
            else if (BlockNum > 1)
            {
                DeviceState = STATE_dfuUPLOAD_IDLE ;
                DeviceStatus[4] = DeviceState;
                /* Change is Accelerated */
                Addr = ((BlockNum - 2) * TRANSFER_SIZE) + BaseAddress;

                /* Return the physical address where data are stored */
                Phy_Addr = DFU_MAL_Read(Addr, Length);

                /* Send the status data over EP0 */
                usbd_ctltx (pudev, Phy_Addr, Length);
            }
            else  /* unsupported wBlockNum */
            {
                DeviceState = STATUS_errSTALLEDPKT;
                DeviceStatus[4] = DeviceState;

                usbd_enum_error (pudev, req); 
            }
        }
        else
        {
            Length = 0;
            BlockNum = 0;

            usbd_enum_error (pudev, req);
        }
    }
    else
    {
        DeviceState = STATE_dfuIDLE;
        DeviceStatus[1] = 0;
        DeviceStatus[2] = 0;
        DeviceStatus[3] = 0;
        DeviceStatus[4] = DeviceState;
    }
}

/**
  * @brief  Handle the DFU_GETSTATUS request
  * @param  pudev: pointer to usb device instance
  * @retval None
  */
static void DFU_Req_GETSTATUS(void *pudev)
{
    switch (DeviceState)
    {
        case STATE_dfuDNLOAD_SYNC:
            if (Length != 0)
            {
                DeviceState = STATE_dfuDNBUSY;
                DeviceStatus[4] = DeviceState;

                if (BlockNum == 0)
                {
                    if(MAL_Buffer[0] == ERASE)
                    {
                        DFU_MAL_GetStatus(BaseAddress, CMD_ERASE, DeviceStatus);
                    }
                    else
                    {
                        DFU_MAL_GetStatus(BaseAddress, CMD_WRITE, DeviceStatus);
                    }
                }
            }
            else  /* (wlength==0)*/
            {
                DeviceState = STATE_dfuDNLOAD_IDLE;
                DeviceStatus[4] = DeviceState;
                DeviceStatus[1] = 0;
                DeviceStatus[2] = 0;
                DeviceStatus[3] = 0;
            }
            break;

        case STATE_dfuMANIFEST_SYNC:
            if (Manifest_State == MANIFEST_IN_PROGRESS)
            {
                DeviceState = STATE_dfuMANIFEST;
                DeviceStatus[4] = DeviceState;
                DeviceStatus[1] = 1;
                DeviceStatus[2] = 0;
                DeviceStatus[3] = 0; /* bwPollTimeout = 1ms */
            }
            else if ((Manifest_State == MANIFEST_COMPLETE) && \
                        (configuration_descriptor.DFU_Function_Desc.bmAttributes & 0x04))
            {
                DeviceState = STATE_dfuIDLE;
                DeviceStatus[4] = DeviceState;
                DeviceStatus[1] = 0;
                DeviceStatus[2] = 0;
                DeviceStatus[3] = 0;
            }
            break;

        default:
            break;
    }

    /* Send the status data of DFU interface to host over EP0 */
    usbd_ctltx (pudev,
                (uint8_t *)(&(DeviceStatus[0])),
                6);
}

/**
  * @brief  Handle the DFU_CLRSTATUS request
  * @param  pudev: pointer to usb device instance
  * @retval None
  */
static void DFU_Req_CLRSTATUS(void *pudev)
{
    if (DeviceState == STATE_dfuERROR)
    {
        DeviceState = STATE_dfuIDLE;
        DeviceStatus[0] = STATUS_OK;
    }
    else
    {
        /*State Error*/
        DeviceState = STATE_dfuERROR;
        DeviceStatus[0] = STATUS_errUNKNOWN;
    }

    DeviceStatus[1] = 0;
    DeviceStatus[2] = 0;
    DeviceStatus[3] = 0; /* bwPollTimeout = 0ms */
    DeviceStatus[4] = DeviceState;
    DeviceStatus[5] = 0; /* iString: index = 0 */
}

/**
  * @brief  Handle the DFU_GETSTATE request
  * @param  pudev: pointer to usb device instance
  * @retval None
  */
static void DFU_Req_GETSTATE(void *pudev)
{
    /* Send the current state of the DFU interface to host */
    usbd_ctltx (pudev, &DeviceState, 1);
}

/**
  * @brief  Handle the DFU_ABORT request
  * @param  pudev: pointer to usb device instance
  * @retval None
  */
static void DFU_Req_ABORT(void *pudev)
{
    switch(DeviceState)
    {
        case STATE_dfuIDLE:
        case STATE_dfuDNLOAD_SYNC:
        case STATE_dfuDNLOAD_IDLE:
        case STATE_dfuMANIFEST_SYNC:
        case STATE_dfuUPLOAD_IDLE:
            DeviceState = STATE_dfuIDLE;
            DeviceStatus[0] = STATUS_OK;
            DeviceStatus[1] = 0;
            DeviceStatus[2] = 0;
            DeviceStatus[3] = 0; /* bwPollTimeout = 0ms */
            DeviceStatus[4] = DeviceState;
            DeviceStatus[5] = 0; /* iString: index = 0 */
            BlockNum = 0;
            Length = 0;
            break;

        default:
            break;
    }
}

/**
  * @brief  Leave DFU mode and reset device to jump to user loaded code
  * @param  pudev: pointer to usb device instance
  * @retval None
  */
void DFU_LeaveDFUMode(void *pudev)
{
    Manifest_State = MANIFEST_COMPLETE;

    DeviceStatus[1] = 0;
    DeviceStatus[2] = 0;
    DeviceStatus[3] = 0;

    if (configuration_descriptor.DFU_Function_Desc.bmAttributes & 0x04)
    {
        DeviceState = STATE_dfuMANIFEST_SYNC;
        DeviceStatus[4] = DeviceState;

        return;
    }
    else
    {
        DeviceState = STATE_dfuMANIFEST_WAIT_RESET;
        DeviceStatus[4] = DeviceState;

        /* Disconnect the USB device */
        USB_SOFT_DISCONNECT_ENABLE();

        /* DeInitilialize the MAL(Media Access Layer) */
        DFU_MAL_DeInit();

        /* Generate system reset to allow jumping to the user code */
        NVIC_SystemReset();
       
        /* This instruction will not be reached (system reset) */
        return;
    }
}

