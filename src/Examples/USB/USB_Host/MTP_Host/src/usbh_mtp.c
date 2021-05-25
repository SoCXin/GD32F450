/*!
    \file  usbh_mtp.c
    \brief this file is the MTP Layer Handlers for USB Host MTP class.

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

#include "usbh_mtp.h"

mtp_handletypedef mtp_handle;

extern void usbh_userprocess(usbh_host_struct *phost, uint8_t id);
static usbh_status_enum usbh_mtp_process (usb_core_handle_struct *pudev, usbh_host_struct *puhost, void *pustate);
static usbh_status_enum usbh_mtp_classrequest (usb_core_handle_struct *pudev, usbh_host_struct *puhost, void *pustate);
static uint8_t mtp_findctlendpoint(usbh_host_struct *phost);
static uint8_t mtp_finddataoutendpoint(usbh_host_struct *phost);
static uint8_t mtp_finddatainendpoint(usbh_host_struct *phost);
static usbh_status_enum usbh_mtp_events (usb_core_handle_struct *pudev, usbh_host_struct *phost);
static void mtp_decodeevent (usbh_host_struct *phost) ;

class_polling_fun_cb_struct class_polling_cb = 
{
    usbh_mtp_classrequest,
    usbh_mtp_process,
};

/*!
    \brief      initialized the MTP class
    \param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_mtp_interfaceinit (usb_core_handle_struct *pudev, void *puhost)
{
    usbh_host_struct *phost = puhost;
    usbh_status_enum status = USBH_OK ;
    uint8_t interface, endpoint;

    if ((USB_MTP_CLASS == phost->device.itf_desc[0].bInterfaceClass) && \
        (MTP_PROTOCOL == phost->device.itf_desc[0].bInterfaceProtocol)) {
        interface = 0x00;
    } else {
        interface = 0xFF;
    }


    if(interface == 0xFF) { /* No Valid Interface */
        status = USBH_FAIL;
        lcd_log_print((uint8_t *)("Cannot Find the interface for Still Image Class."), 0, LCD_COLOR_BLACK);
    } else {
        endpoint = mtp_findctlendpoint(phost);

        if (&mtp_handle == NULL) {
            status = USBH_FAIL;  
            lcd_log_print((uint8_t *)("Cannot allocate RAM for MTP Handle"), 0, LCD_COLOR_BLACK);
        }

        /*Collect the control endpoint address and length*/
        mtp_handle.notificationep = phost->device.ep_desc[interface][endpoint].bEndpointAddress;
        mtp_handle.notificationepsize  = phost->device.ep_desc[interface][endpoint].wMaxPacketSize;
        mtp_handle.notificationpipe = usbh_channel_alloc(pudev, mtp_handle.notificationep);
        mtp_handle.events.poll = phost->device.ep_desc[interface][endpoint].bInterval;

        /* Open pipe for Notification endpoint */
        usbh_channel_open  (pudev,
                            mtp_handle.notificationpipe,                            
                            phost->device.address,
                            phost->device.speed,
                            USB_EPTYPE_INTR,
                            mtp_handle.notificationepsize); 

        endpoint = mtp_finddatainendpoint(phost);

        /*Collect the control endpoint address and length*/
        mtp_handle.datainep = phost->device.ep_desc[interface][endpoint].bEndpointAddress;
        mtp_handle.datainepsize  = phost->device.ep_desc[interface][endpoint].wMaxPacketSize;
        mtp_handle.datainpipe = usbh_channel_alloc(pudev, mtp_handle.datainep);

        /* Open pipe for DATA IN endpoint */
        usbh_channel_open (pudev,
                           mtp_handle.datainpipe,                         
                           phost->device.address,
                           phost->device.speed,
                           USB_EPTYPE_BULK,
                           mtp_handle.datainepsize); 

        endpoint = mtp_finddataoutendpoint(phost);

        /*Collect the DATA OUT endpoint address and length*/
        mtp_handle.dataoutep = phost->device.ep_desc[interface][endpoint].bEndpointAddress;
        mtp_handle.dataoutepsize  = phost->device.ep_desc[interface][endpoint].wMaxPacketSize;
        mtp_handle.dataoutpipe = usbh_channel_alloc(pudev, mtp_handle.dataoutep);

        /* Open pipe for DATA OUT endpoint */
        usbh_channel_open (pudev,
                           mtp_handle.dataoutpipe,                          
                           phost->device.address,
                           phost->device.speed,
                           USB_EPTYPE_BULK,
                           mtp_handle.dataoutepsize); 

        mtp_handle.state = MTP_OPENSESSION;
        mtp_handle.is_ready = 0;
        mtp_handle.events.state = MTP_EVENTS_INIT;
        return usbh_ptp_init(phost);
    }

    return status;
}

/*!
    \brief      find MTP control interface
	\param[in]  phost: pointer to USB host handler
    \param[out] none
    \retval     none
*/
static uint8_t mtp_findctlendpoint(usbh_host_struct *phost)
{
    uint8_t interface, endpoint;

    for (interface = 0;  interface < USBH_MAX_INTERFACES_NUM ; interface ++ ) {
        if (phost->device.itf_desc[interface].bInterfaceClass == USB_MTP_CLASS) {
            for (endpoint = 0;  endpoint < USBH_MAX_EP_NUM ; endpoint ++ ) {
                if ((phost->device.ep_desc[interface][endpoint].bEndpointAddress & 0x80)&&
                    (phost->device.ep_desc[interface][endpoint].wMaxPacketSize > 0)&&
                    ((phost->device.ep_desc[interface][endpoint].bmAttributes & USB_EPTYPE_MASK) == USB_EPTYPE_INTR)) {

                    return endpoint;
                }
            }
        }
    }

    return 0xFF; /* Invalid Endpoint */
}

/*!
    \brief      find MTP DATA OUT interface
	\param[in]  phost: pointer to USB host handler
    \param[out] none
    \retval     none
*/
static uint8_t mtp_finddataoutendpoint(usbh_host_struct *phost)
{
    uint8_t interface, endpoint;

    for (interface = 0;  interface < USBH_MAX_INTERFACES_NUM ; interface ++ ) {
        if (phost->device.itf_desc[interface].bInterfaceClass == USB_MTP_CLASS) {
            for (endpoint = 0;  endpoint < USBH_MAX_EP_NUM ; endpoint ++ ) {
                if (((phost->device.ep_desc[interface][endpoint].bEndpointAddress & 0x80) == 0)&&
                      (phost->device.ep_desc[interface][endpoint].wMaxPacketSize > 0)&&
                       ((phost->device.ep_desc[interface][endpoint].bmAttributes & USB_EPTYPE_MASK) == USB_EPTYPE_BULK)) {
                    return endpoint;
                }
            }
        }
    }

    return 0xFF; /* Invalid Endpoint */
}

/*!
    \brief      find MTP DATA IN interface
	\param[in]  phost: pointer to USB host handler
    \param[out] none
    \retval     none
*/
static uint8_t mtp_finddatainendpoint(usbh_host_struct *phost)
{
    uint8_t interface, endpoint;

    for (interface = 0;  interface < USBH_MAX_INTERFACES_NUM ; interface ++ ) {
        if (phost->device.itf_desc[interface].bInterfaceClass == USB_MTP_CLASS) {
            for (endpoint = 0;  endpoint < USBH_MAX_EP_NUM ; endpoint ++ ) {
                if ((phost->device.ep_desc[interface][endpoint].bEndpointAddress & 0x80)&&
                     (phost->device.ep_desc[interface][endpoint].wMaxPacketSize > 0)&&
                      ((phost->device.ep_desc[interface][endpoint].bmAttributes & USB_EPTYPE_MASK) == USB_EPTYPE_BULK)) {
                    return endpoint;
                }
            }
        }
    }

    return 0xFF; /* Invalid Endpoint */
}

/*!
    \brief      de-init the pipes used for the MTP class
	\param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
    \param[out] none
    \retval     none
*/
void USBH_MTP_InterfaceDeInit (usb_core_handle_struct *pudev, void *phost)
{
    if (mtp_handle.dataoutpipe) {
        usb_hostchannel_halt(phost, mtp_handle.dataoutpipe);
        usbh_channel_free  (phost, mtp_handle.dataoutpipe);
        mtp_handle.dataoutpipe = 0;     /* Reset the Channel as Free */
    }

    if (mtp_handle.datainpipe) {
        usb_hostchannel_halt(phost, mtp_handle.datainpipe);
        usbh_channel_free  (phost, mtp_handle.datainpipe);
        mtp_handle.datainpipe = 0;     /* Reset the Channel as Free */
    }

    if (mtp_handle.notificationpipe) {
        usb_hostchannel_halt(phost, mtp_handle.notificationpipe);
        usbh_channel_free  (phost, mtp_handle.notificationpipe);
        mtp_handle.notificationpipe = 0;     /* Reset the Channel as Free */
    }
}

/*!
    \brief      the function is responsible for handling standard requests
	\param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  pustate:
    \param[out] none
    \retval     none
*/
static usbh_status_enum usbh_mtp_classrequest (usb_core_handle_struct *pudev, usbh_host_struct *puhost, void *pustate)
{  
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif    
    return USBH_OK;;
}

/*!
    \brief      the function is for managing state machine for MTP data transfers 
	\param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  pustate:
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_mtp_process (usb_core_handle_struct *pudev, usbh_host_struct *puhost, void *pustate)
{
    usbh_status_enum status = USBH_BUSY;
    uint32_t idx = 0;
    uint8_t tempstr[128];

    switch (mtp_handle.state) {
        case MTP_OPENSESSION:
            status = usbh_ptp_opensession (pudev, puhost, 1); /* Session '0' is not valid */

            if (status == USBH_OK) {
                lcd_log_print((uint8_t *)("MTP Session #0 Opened"), 0, LCD_COLOR_BLACK);
                mtp_handle.state = MTP_GETDEVICEINFO; 
            }
            break;

        case MTP_GETDEVICEINFO:
            status = usbh_ptp_getdeviceinfo (pudev, puhost, &(mtp_handle.info.devinfo));

            if (status == USBH_OK) {
                lcd_log_print((uint8_t *)(">>>>> MTP Device Information"), 0, LCD_COLOR_BLACK);

                sprintf((char *)tempstr, "Standard version : %x", (uint16_t)(mtp_handle.info.devinfo.StandardVersion));
                lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);

                sprintf((char *)tempstr, "Vendor ExtID : %s", (char *)((mtp_handle.info.devinfo.VendorExtensionID == 6)?"MTP": "NOT SUPPORTED"));
                lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);

                sprintf((char *)tempstr, "Functional mode : %s", (char *)((mtp_handle.info.devinfo.FunctionalMode == 0) ? "Standard" : "Vendor"));
                lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);

                sprintf((char *)tempstr, "Number of Supported Operation(s) : %d", (uint16_t)(mtp_handle.info.devinfo.OperationsSupported_len));
                lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);

                sprintf((char *)tempstr, "Number of Supported Events(s) : %d", (uint16_t)(mtp_handle.info.devinfo.EventsSupported_len));
                lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);

                sprintf((char *)tempstr, "Number of Supported Proprieties : %d", (uint16_t)(mtp_handle.info.devinfo.DevicePropertiesSupported_len));
                lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);

                sprintf((char *)tempstr, "Manufacturer : %s", (char *)(mtp_handle.info.devinfo.Manufacturer));
                lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);

                sprintf((char *)tempstr, "Model : %s", (char *)(mtp_handle.info.devinfo.Model));
                lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);

                sprintf((char *)tempstr, "Device version : %s", (char *)(mtp_handle.info.devinfo.DeviceVersion));
                lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);

                sprintf((char *)tempstr, "Serial number : %s", (char *)(mtp_handle.info.devinfo.SerialNumber));
                lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);

                mtp_handle.state = MTP_GETSTORAGEIDS;  
            }
            break;

        case MTP_GETSTORAGEIDS:
            status = usbh_ptp_getstorageids (pudev, puhost, &(mtp_handle.info.storids));

            if (status == USBH_OK) {
                sprintf((char *)tempstr, "Number of storage ID items : %d", (uint16_t)(mtp_handle.info.storids.n));
                lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);

                for (idx  = 0; idx < mtp_handle.info.storids.n; idx ++) {
                    sprintf((char *)tempstr, "storage#%d ID : %x", idx, (uint16_t)(mtp_handle.info.storids.Storage[idx]));
                    lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
                }

                mtp_handle.current_storage_unit = 0;
                mtp_handle.state = MTP_GETSTORAGEINFO;
            }
            break;

        case MTP_GETSTORAGEINFO:
            status = usbh_ptp_getstorageinfo (pudev,
                                     	      puhost, 
                                              mtp_handle.info.storids.Storage[mtp_handle.current_storage_unit], 
                                              &((mtp_handle.info.storinfo)[mtp_handle.current_storage_unit]));

            if (status == USBH_OK) {
                sprintf((char *)tempstr, "Volume#%lu: %s   [%s]", (unsigned long)mtp_handle.current_storage_unit,
                mtp_handle.info.storinfo[mtp_handle.current_storage_unit].StorageDescription,
                mtp_handle.info.storinfo[mtp_handle.current_storage_unit].VolumeLabel);
                lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);

                if (++mtp_handle.current_storage_unit >= mtp_handle.info.storids.n) {
                    mtp_handle.state = MTP_IDLE;
                    mtp_handle.is_ready = 1;
                    mtp_handle.current_storage_unit = 0;
                    mtp_handle.params.CurrentStorageId = mtp_handle.info.storids.Storage[0];

                    lcd_log_print((uint8_t *)("MTP Class initialized."), 0, LCD_COLOR_BLACK);
                    sprintf((char *)tempstr, "%s is default storage unit", (char *)(mtp_handle.info.storinfo[0].StorageDescription));
                    lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
                    usbh_userprocess(puhost, HOST_USER_CLASS_ACTIVE); 
                }
            }
            break;

        case  MTP_IDLE:
            usbh_mtp_events(pudev, puhost);

        default:
            status = USBH_OK;
        break;
    }
    return status;
}

/*!
    \brief      select the storage unit to be used
	\param[in]  phost: pointer to USB host handler
    \param[out] none
    \retval     MTP handler status
*/
uint8_t usbh_mtp_isready (usbh_host_struct *phost)
{
    return (mtp_handle.is_ready);
}

/*!
    \brief      get number of the storage unit
	\param[in]  phost: pointer to USB host handler
    \param[out] storage_num: storage unit number
    \retval     MTP handler status
*/
usbh_status_enum usbh_mtp_getnumstorage (usbh_host_struct *phost, uint8_t *storage_num)
{
    usbh_status_enum status = USBH_FAIL;

    if (mtp_handle.is_ready > 0) {
        *storage_num = mtp_handle.info.storids.n;
        status  = USBH_OK;
    }

    return status;
}

/*!
    \brief      select the storage unit to be used
	\param[in]  phost: pointer to USB host handler
	\param[in]  storage_idx: storage unit index
    \param[out] none
    \retval     MTP handler status
*/
usbh_status_enum usbh_mtp_selectstorage (usbh_host_struct *phost, uint8_t storage_idx)
{
    usbh_status_enum status = USBH_FAIL;

    if ((storage_idx < mtp_handle.info.storids.n) && (mtp_handle.is_ready)) {
        mtp_handle.params.CurrentStorageId = mtp_handle.info.storids.Storage[storage_idx];
        status  = USBH_OK;
    }

    return status;
}

/*!
    \brief      get storage information
	\param[in]  phost: pointer to USB host handler
	\param[in]  storage_idx: storage index
	\param[in]  info:
    \param[out] none
    \retval     MTP handler status
*/
usbh_status_enum usbh_mtp_getstorageinfo (usbh_host_struct *phost, 
                                          uint8_t storage_idx, 
										  MTP_StorageInfoTypedef *info)
{
    usbh_status_enum status = USBH_FAIL;

    if ((storage_idx < mtp_handle.info.storids.n) && (mtp_handle.is_ready)) {
        *info = mtp_handle.info.storinfo[storage_idx];
        status  = USBH_OK;
    }

    return status;
}

/*!
    \brief      select the storage unit to be used
	\param[in]  pudev: pointer to USB device handler
	\param[in]  phost: pointer to USB host handler
    \param[in]  storage_idx:
	\param[in]  objectformatcode:
	\param[in]  associationOH:
	\param[in]  numobs:
    \param[out] none
    \retval     MTP handler status
*/
usbh_status_enum usbh_mtp_getnumobjects (usb_core_handle_struct *pudev,
                                         usbh_host_struct *phost, 
                                         uint32_t storage_idx, 
                                         uint32_t objectformatcode, 
                                         uint32_t associationOH,
                                         uint32_t* numobs)
{
    usbh_status_enum status = USBH_FAIL;
    uint32_t timeout = (uint32_t)USB_CURRENT_FRAME_GET();
    if ((storage_idx < mtp_handle.info.storids.n) && (mtp_handle.is_ready)) {
        while ((status = usbh_ptp_getnumobjects (pudev, phost, 
                                                         mtp_handle.info.storids.Storage[storage_idx], 
                                                         objectformatcode, 
                                                         associationOH,
                                                         numobs)) == USBH_BUSY) {

            if ((((uint32_t)USB_CURRENT_FRAME_GET() - timeout) >   5000) || (pudev->host.connect_status == 0)) {
                return USBH_FAIL;
            }
        }
    }
    return status;
}

/*!
    \brief      get object handler
	\param[in]  pudev:
	\param[in]  phost: pointer to USB host handler
	\param[in]  storage_idx:
	\param[in]  objectformatcode:
	\param[in]  associationOH:
	\param[in]  objecthandles:
    \param[out] none
    \retval     MTP handler status
*/
usbh_status_enum usbh_mtp_getobjecthandles (usb_core_handle_struct *pudev, 
                                            usbh_host_struct *phost, 
                                            uint32_t storage_idx, 
                                            uint32_t objectformatcode, 
                                            uint32_t associationOH,
                                            ptp_objecthandlestypedef* objecthandles)
{
    usbh_status_enum status = USBH_FAIL;
    uint32_t timeout = (uint32_t)USB_CURRENT_FRAME_GET();

    if ((storage_idx < mtp_handle.info.storids.n) && (mtp_handle.is_ready)) {  
        while ((status = usbh_ptp_getobjecthandles (pudev, phost, 
                                                mtp_handle.info.storids.Storage[storage_idx], 
                                                objectformatcode, 
                                                associationOH,
                                                objecthandles)) == USBH_BUSY) {
            if ((((uint32_t)USB_CURRENT_FRAME_GET() - timeout) >  5000) || (pudev->host.connect_status == 0)) {
                return USBH_FAIL;
            }
        }
    }
    return status;
}

/*!
    \brief      get object information
	\param[in]  pudev:
	\param[in]  phost: pointer to USB host handler
	\param[in]  handle:
	\param[in]  objectinfo:
    \param[out] none
    \retval     MTP handler status
*/
usbh_status_enum usbh_mtp_getobjectinfo (usb_core_handle_struct *pudev, 
                                         usbh_host_struct *phost, 
                                         uint32_t handle, 
                                         ptp_objectinfotypedef* objectinfo)
{
    usbh_status_enum status = USBH_FAIL;
    uint32_t timeout = (uint32_t)USB_CURRENT_FRAME_GET();

    if (mtp_handle.is_ready) {
        while ((status = usbh_ptp_getobjectinfo (pudev, phost, handle, objectinfo)) == USBH_BUSY) {
            if ((((uint32_t)USB_CURRENT_FRAME_GET() - timeout) >  5000) || (pudev->host.connect_status == 0)) {
                return USBH_FAIL;
            }
        }
    }

    return status;
}

/*!
    \brief      delete an object
	\param[in]  pudev:
	\param[in]  phost: pointer to USB host handler
	\param[in]  handle:
	\param[in]  objectformatcode:
    \param[out] none
    \retval     MTP handler status
*/
usbh_status_enum usbh_mtp_deleteobject (usb_core_handle_struct *pudev, 
                                        usbh_host_struct *phost,
                                        uint32_t handle,
                                        uint32_t objectformatcode)
{
  usbh_status_enum status = USBH_FAIL;
  uint32_t timeout = (uint32_t)USB_CURRENT_FRAME_GET();
  
  if(mtp_handle.is_ready)
  {   
    while ((status = usbh_ptp_deleteobject (pudev, phost, handle, objectformatcode)) == USBH_BUSY)
    {
      if((((uint32_t)USB_CURRENT_FRAME_GET() - timeout) >  5000) || (pudev->host.connect_status == 0)) 
      {
        return USBH_FAIL;
      }
    }
  }
  return status;
}

/*!
    \brief      get object
	\param[in]  pudev:
	\param[in]  phost: pointer to USB host handler
    \param[in]  handle:
	\param[in]  object:
	\param[out] none
    \retval     MTP handler status
*/
usbh_status_enum usbh_mtp_getobject (usb_core_handle_struct *pudev,
                                     usbh_host_struct *phost, 
                                     uint32_t handle, 
                                     uint8_t *object)
{
    usbh_status_enum status = USBH_FAIL;
    uint32_t timeout = (uint32_t)USB_CURRENT_FRAME_GET();

    if (mtp_handle.is_ready) {
        while ((status = usbh_ptp_getobject (pudev, phost, handle, object)) == USBH_BUSY) {
            if ((((uint32_t)USB_CURRENT_FRAME_GET() - timeout) >  5000) || (pudev->host.connect_status == 0)) {
                return USBH_FAIL;
            }
        }
    }
    return status;
}

/*!
    \brief      get object partially
	\param[in]  pudev:
	\param[in]  phost: pointer to USB host handler
    \param[in]  handle:
	\param[in]  offset:
	\param[in]  maxbytes:
	\param[in]  object:
	\param[in]  len:
    \param[out] none
    \retval     MTP handler status
*/
usbh_status_enum usbh_mtp_getpartialobject(usb_core_handle_struct *pudev,
                                           usbh_host_struct *phost, 
                                           uint32_t handle, 
                                           uint32_t offset,
                                           uint32_t maxbytes, 
                                           uint8_t *object,
                                           uint32_t *len)
{
    usbh_status_enum status = USBH_FAIL;
    uint32_t timeout = (uint32_t)USB_CURRENT_FRAME_GET();;

    if (mtp_handle.is_ready) {
        while ((status = usbh_ptp_getpartialobject(pudev, phost, 
                                                           handle, 
                                                           offset,
                                                           maxbytes, 
                                                           object,
                                                           len)) == USBH_BUSY) {
            if ((((uint32_t)USB_CURRENT_FRAME_GET() - timeout) >  5000) || (pudev->host.connect_status == 0)) {
                return USBH_FAIL;
            }
        }
    }

    return status;
}

/*!
    \brief      get object prop supported
	\param[in]  pudev:
	\param[in]  phost: pointer to USB host handler
    \param[in]  ofc:
	\param[in]  propnum:
	\param[in]  props:
    \param[out] none
    \retval     MTP handler status
*/
usbh_status_enum usbh_mtp_getobjectpropssupported (usb_core_handle_struct *pudev, 
                                                   usbh_host_struct *phost,
                                                   uint16_t ofc,
                                                   uint32_t *propnum, 
                                                   uint16_t *props)
{
    usbh_status_enum status = USBH_FAIL;
    uint32_t timeout = (uint32_t)USB_CURRENT_FRAME_GET();

    if (mtp_handle.is_ready) {
        while ((status = usbh_ptp_getobjectpropssupported (pudev,phost,
                                                         ofc,
                                                         propnum, 
                                                         props)) == USBH_BUSY) {
            if ((((uint32_t)USB_CURRENT_FRAME_GET() - timeout) >  5000) || (pudev->host.connect_status == 0)) {
                return USBH_FAIL;
            }
        }
    }

    return status;
}

/*!
    \brief      get object prop descriptor
	\param[in]  pudev:
	\param[in]  phost: pointer to USB host handler
    \param[in]  opc:
	\param[in]  ofc:
	\param[in]  opd:
    \param[out] none
    \retval     MTP handler status
*/
usbh_status_enum usbh_mtp_getobjectpropdesc (usb_core_handle_struct *pudev,
                                             usbh_host_struct *phost,
                                             uint16_t opc, 
                                             uint16_t ofc, 
                                             PTP_ObjectPropDescTypeDef *opd)
{
    usbh_status_enum status = USBH_FAIL;
    uint32_t timeout = (uint32_t)USB_CURRENT_FRAME_GET();

    if (mtp_handle.is_ready) {
        while ((status = usbh_ptp_getobjectpropdesc (pudev, phost,
                                                    opc, 
                                                    ofc, 
                                                    opd)) == USBH_BUSY) {
            if ((((uint32_t)USB_CURRENT_FRAME_GET() - timeout) >  5000) || (pudev->host.connect_status == 0)) {
            return USBH_FAIL;
            }
        }
    }

    return status; 
}

/*!
    \brief      get object prop list
	\param[in]  pudev:
	\param[in]  phost: pointer to USB host handler
    \param[in]  handle:
	\param[in]  pprops:
	\param[in]  nrofprops:
    \param[out] none
    \retval     MTP handler status
*/
usbh_status_enum usbh_mtp_getobjectproplist (usb_core_handle_struct *pudev, 
                                             usbh_host_struct *phost,
                                             uint32_t handle, 
                                             mtp_propertiestypedef *pprops, 
                                             uint32_t *nrofprops)
{
    usbh_status_enum status = USBH_FAIL;
    uint32_t timeout = (uint32_t)USB_CURRENT_FRAME_GET();

    if (mtp_handle.is_ready) {
        while ((status = usbh_ptp_getobjectproplist (pudev, 
		                                             phost,
                                                     handle, 
                                                     pprops, 
                                                     nrofprops)) == USBH_BUSY) {
            if ((((uint32_t)USB_CURRENT_FRAME_GET() - timeout) >  5000) || (pudev->host.connect_status == 0)) {
                return USBH_FAIL;
            }
        }
    }

    return status; 
}

/*!
    \brief      get object partially
	\param[in]  pudev:
	\param[in]  phost: pointer to USB host handler
    \param[in]  handle:
	\param[in]  object:
	\param[in]  size:
    \param[out] none
    \retval     MTP handler status
*/
usbh_status_enum usbh_mtp_sendobject (usb_core_handle_struct *pudev, usbh_host_struct *phost, 
                                           uint32_t handle, 
                                           uint8_t *object,
                                           uint32_t size)
{
    usbh_status_enum status = USBH_FAIL;
    uint32_t timeout = (uint32_t)USB_CURRENT_FRAME_GET();

    if (mtp_handle.is_ready) {
        while ((status = usbh_ptp_sendobject (pudev, phost, handle, object, size)) == USBH_BUSY) {
            if ((((uint32_t)USB_CURRENT_FRAME_GET() - timeout) >  5000) || (pudev->host.connect_status == 0)) {
                return USBH_FAIL;
            }
        }
    }

    return status;
}

/*!
    \brief      handle MTP events
	\param[in]  pudev:
	\param[in]  phost: pointer to USB host handler
    \param[out] none
    \retval     MTP handler status
*/
static usbh_status_enum usbh_mtp_events (usb_core_handle_struct *pudev, usbh_host_struct *phost)
{
    usbh_status_enum status = USBH_BUSY ;

    switch (mtp_handle.events.state) {
        case MTP_EVENTS_INIT:
            if (((uint32_t)USB_CURRENT_FRAME_GET() & 1) == 0) {
                mtp_handle.events.timer = (uint16_t)USB_CURRENT_FRAME_GET();

                usbh_xfer (pudev, 
                         (uint8_t *)&(mtp_handle.events.container), 
                         mtp_handle.notificationpipe,
                         mtp_handle.notificationepsize);

                mtp_handle.events.state = MTP_EVENTS_GETDATA ;
            }
            break;

        case MTP_EVENTS_GETDATA:
            if (hcd_urb_state_get(pudev, mtp_handle.notificationpipe) == URB_DONE) {
                mtp_decodeevent(phost);
            }

            if ((USB_CURRENT_FRAME_GET() - mtp_handle.events.timer) >= mtp_handle.events.poll) {
                mtp_handle.events.timer = USB_CURRENT_FRAME_GET();

                usbh_xfer (pudev, 
                         (uint8_t *)&(mtp_handle.events.container), 
                         mtp_handle.notificationpipe,
                         mtp_handle.notificationepsize);                                
            }
            break;

        default:  
            break; 
    }

    return status;
}

/*!
    \brief      decode device event sent by responder
	\param[in]  phost: pointer to USB host handler
    \param[out] none
    \retval     none
*/
static void mtp_decodeevent (usbh_host_struct *phost) 
{
    uint16_t code;
    uint32_t param1;
    uint8_t tempstr[128];

    /* Process the event */
    code = mtp_handle.events.container.code;
    param1 = mtp_handle.events.container.param1;

    switch (code) {
        case PTP_EC_Undefined:
            sprintf((char *)tempstr, "EVT: PTP_EC_Undefined in session %u", (uint16_t)(mtp_handle.ptp.session_id));
            lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
            break;
            
        case PTP_EC_CancelTransaction:
            sprintf((char *)tempstr, "EVT: PTP_EC_CancelTransaction in session %u", (uint16_t)(mtp_handle.ptp.session_id));
            lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
            break;

        case PTP_EC_ObjectAdded:
            sprintf((char *)tempstr, "EVT: PTP_EC_ObjectAdded in session %u", (uint16_t)(mtp_handle.ptp.session_id));
            lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
            break;

        case PTP_EC_ObjectRemoved:
            sprintf((char *)tempstr, "EVT: PTP_EC_ObjectRemoved in session %u", (uint16_t)(mtp_handle.ptp.session_id));
            lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
            break;

        case PTP_EC_StoreAdded:
            sprintf((char *)tempstr, "EVT: PTP_EC_StoreAdded in session %u", (uint16_t)(mtp_handle.ptp.session_id));
            lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
            break;

        case PTP_EC_StoreRemoved:
            sprintf((char *)tempstr, "EVT: PTP_EC_StoreRemoved in session %u", (uint16_t)(mtp_handle.ptp.session_id));
            lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
            break;

        case PTP_EC_DevicePropChanged:
            sprintf((char *)tempstr, "EVT: PTP_EC_DevicePropChanged in session %u", (uint16_t)(mtp_handle.ptp.session_id));
            lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
            break;

        case PTP_EC_ObjectInfoChanged:
            sprintf((char *)tempstr, "EVT: PTP_EC_ObjectInfoChanged in session %u", (uint16_t)(mtp_handle.ptp.session_id));
            lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
            break;

        case PTP_EC_DeviceInfoChanged:
            sprintf((char *)tempstr, "EVT: PTP_EC_DeviceInfoChanged in session %u", (uint16_t)(mtp_handle.ptp.session_id));
            lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
            break;

        case PTP_EC_RequestObjectTransfer:
            sprintf((char *)tempstr, "EVT: PTP_EC_RequestObjectTransfer in session %u", (uint16_t)(mtp_handle.ptp.session_id));
            lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
            break;

        case PTP_EC_StoreFull:
            sprintf((char *)tempstr, "EVT: PTP_EC_StoreFull in session %u", (uint16_t)(mtp_handle.ptp.session_id));
            lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
            break;

        case PTP_EC_DeviceReset:
            sprintf((char *)tempstr, "EVT: PTP_EC_DeviceReset in session %u", (uint16_t)(mtp_handle.ptp.session_id));
            lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
            break;

        case PTP_EC_StorageInfoChanged :
            sprintf((char *)tempstr, "EVT: PTP_EC_StorageInfoChanged in session %u", (uint16_t)(mtp_handle.ptp.session_id));
            lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
            break;

        case PTP_EC_CaptureComplete :
            sprintf((char *)tempstr, "EVT: PTP_EC_CaptureComplete in session %u", (uint16_t)(mtp_handle.ptp.session_id));
            lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
            break;

        case PTP_EC_UnreportedStatus :
            sprintf((char *)tempstr, "EVT: PTP_EC_UnreportedStatus in session %u", (uint16_t)(mtp_handle.ptp.session_id));
            lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
            break;

        default :
            sprintf((char *)tempstr, "Received unknown event in session %u", (uint16_t)(mtp_handle.ptp.session_id));
            lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
            break;
    }

    usbh_mtp_eventscallback(phost, code, param1);
}

/*!
    \brief      get device prop descriptor
	\param[in]  pudev:
	\param[in]  phost: pointer to USB host handler
    \param[in]  propcode:
	\param[in]  devicepropertydesc:
    \param[out] none
    \retval     MTP handler status
*/
usbh_status_enum usbh_mtp_getdevicepropdesc (usb_core_handle_struct *pudev, 
                                             usbh_host_struct *phost,
                                             uint16_t propcode, 
                                             ptp_devicepropdesctypdef* devicepropertydesc)

{
    usbh_status_enum status = USBH_FAIL;
    uint32_t timeout = (uint32_t)USB_CURRENT_FRAME_GET();

    if (mtp_handle.is_ready) {
        while ((status = usbh_ptp_getdevicepropdesc (pudev, phost, propcode, devicepropertydesc)) == USBH_BUSY) {
            if((((uint32_t)USB_CURRENT_FRAME_GET() - timeout) >  5000) || (pudev->host.connect_status == 0)) {
                return USBH_FAIL;
            }
        }
    }

    return status;
}

/*!
    \brief      the function informs that host has received an event
	\param[in]  phost: pointer to USB host handler
    \param{in]  event:
	\param[in]  param:
    \param[out] none
    \retval     MTP handler status
*/
__weak void usbh_mtp_eventscallback(usbh_host_struct *phost, uint32_t event, uint32_t param)
{

}
