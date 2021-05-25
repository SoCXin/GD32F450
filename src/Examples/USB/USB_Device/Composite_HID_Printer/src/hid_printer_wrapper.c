/*!
    \file  hid_printer_wrapper.c
    \brief this file calls to the separate HID and printer class layer handlers

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

#include "hid_printer_wrapper.h"
#include "usbd_int.h"
#include "usbd_conf.h"

#define USBD_VID                        0x28E9
#define USBD_PID                        0x325A

usbd_int_cb_struct *usbd_int_fops = NULL;

/* Note:it should use the C99 standard when compiling the below codes */
/* USB standard device descriptor */
__ALIGN_BEGIN const usb_descriptor_device_struct device_descripter __ALIGN_END =
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

__ALIGN_BEGIN const usb_descriptor_configuration_set_struct configuration_descriptor __ALIGN_END = 
{
    .Config = 
    {
        .Header = 
         {
             .bLength = sizeof(usb_descriptor_configuration_struct), 
             .bDescriptorType = USB_DESCTYPE_CONFIGURATION 
         },
        .wTotalLength = sizeof(configuration_descriptor),
        .bNumInterfaces = 0x02,
        .bConfigurationValue = 0x01,
        .iConfiguration = 0x00,
        .bmAttributes = 0x80,
        .bMaxPower = 0x32
    },

    .HID_Interface = 
    {
        .Header = 
         {
             .bLength = sizeof(usb_descriptor_interface_struct), 
             .bDescriptorType = USB_DESCTYPE_INTERFACE 
         },
        .bInterfaceNumber = 0x00,
        .bAlternateSetting = 0x00,
        .bNumEndpoints = 0x02,
        .bInterfaceClass = 0x03,
        .bInterfaceSubClass = 0x00,
        .bInterfaceProtocol = 0x00,
        .iInterface = 0x00
    },

    .HID_VendorHID = 
    {
        .Header = 
         {
             .bLength = sizeof(usb_hid_descriptor_hid_struct),
             .bDescriptorType = CUSTOMHID_DESC_TYPE 
         },
        .bcdHID = 0x0111,
        .bCountryCode = 0x00,
        .bNumDescriptors = 0x01,
        .bDescriptorType = CUSTOMHID_REPORT_DESCTYPE,
        .wDescriptorLength = CUSTOMHID_REPORT_DESC_SIZE,
    },

    .HID_ReportINEndpoint = 
    {
        .Header = 
         {
             .bLength = sizeof(usb_descriptor_endpoint_struct), 
             .bDescriptorType = USB_DESCTYPE_ENDPOINT 
         },
        .bEndpointAddress = CUSTOMHID_IN_EP,
        .bmAttributes = 0x03,
        .wMaxPacketSize = CUSTOMHID_IN_PACKET,
        .bInterval = 0x20
    },

    .HID_ReportOUTEndpoint = 
    {
        .Header = 
         {
             .bLength = sizeof(usb_descriptor_endpoint_struct), 
             .bDescriptorType = USB_DESCTYPE_ENDPOINT
         },
        .bEndpointAddress = CUSTOMHID_OUT_EP,
        .bmAttributes = 0x03,
        .wMaxPacketSize = CUSTOMHID_OUT_PACKET,
        .bInterval = 0x20
    },
  
    .Printer_interface = 
    {
        .Header = 
         {
             .bLength = sizeof(usb_descriptor_interface_struct), 
             .bDescriptorType = USB_DESCTYPE_INTERFACE 
         },
        .bInterfaceNumber = 0x01,
        .bAlternateSetting = 0x00,
        .bNumEndpoints = 0x02,
        .bInterfaceClass = 0x07,
        .bInterfaceSubClass = 0x01,
        .bInterfaceProtocol = 0x02,
        .iInterface = 0x00
    },

    .Printer_DataInEndpoint = 
    {
        .Header = 
         {
             .bLength = sizeof(usb_descriptor_endpoint_struct), 
             .bDescriptorType = USB_DESCTYPE_ENDPOINT 
         },
        .bEndpointAddress = PRINTER_IN_EP,
        .bmAttributes = 0x02,
        .wMaxPacketSize = PRINTER_IN_PACKET,
        .bInterval = 0x00
    },

    .Printer_DataOutEndpoint = 
    {
        .Header = 
         {
             .bLength = sizeof(usb_descriptor_endpoint_struct), 
             .bDescriptorType = USB_DESCTYPE_ENDPOINT 
         },
        .bEndpointAddress = PRINTER_OUT_EP,
        .bmAttributes = 0x02,
        .wMaxPacketSize = PRINTER_OUT_PACKET,
        .bInterval = 0x00
    }
};

/* USB language ID Descriptor */
__ALIGN_BEGIN const usb_descriptor_language_id_struct usbd_language_id_desc __ALIGN_END = 
{
    .Header = 
     {
         .bLength = sizeof(usb_descriptor_language_id_struct), 
         .bDescriptorType = USB_DESCTYPE_STRING
     },
    .wLANGID = ENG_LANGID
};

__ALIGN_BEGIN uint8_t* usbd_strings[] __ALIGN_END = 
{
    [USBD_LANGID_STR_IDX] = (uint8_t *)&usbd_language_id_desc,
    [USBD_MFC_STR_IDX] = USBD_STRING_DESC("GigaDevice"),
    [USBD_PRODUCT_STR_IDX] = USBD_STRING_DESC("GD32 Composite HID Printer"),
    [USBD_SERIAL_STR_IDX] = USBD_STRING_DESC("GD32F4XX-V2.0.0-66sdfb5c6de")
};

/*!
    \brief      initialize the HID/printer device
    \param[in]  pudev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
uint8_t hid_printer_init (void *pudev, uint8_t config_index)
{
    /* HID initialization */
    custom_hid_init(pudev, config_index);

    /* printer initialization */
    printer_init(pudev, config_index);

    return USBD_OK;
}

/*!
    \brief      de-initialize the HID/printer device
    \param[in]  pudev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
uint8_t hid_printer_deinit (void *pudev, uint8_t config_index)
{
    /* HID De-initialization */
    custom_hid_deinit(pudev, config_index);

    /* printer De-initialization */
    printer_deinit(pudev, config_index);

    return USBD_OK;
}

/*!
    \brief      handle the custom HID/printer class-specific request
    \param[in]  pudev: pointer to USB device instance
    \param[in]  req: device class request
    \param[out] none
    \retval     USB device operation status
*/
uint8_t hid_printer_req_handler (void *pudev, usb_device_req_struct *req)
{
    if ((req->wIndex & 0xFF) == HID_INTERFACE) {
        return custom_hid_req_handler(pudev, req);
    } else {
        return printer_req_handler(pudev, req);
    }
}

/*!
    \brief      handle data stage
    \param[in]  pudev: pointer to USB device instance
    \param[in]  rx_tx: data transfer direction:
      \arg        USBD_TX
      \arg        USBD_RX
    \param[in]  ep_id: endpoint identifier
    \param[out] none
    \retval     USB device operation status
*/
uint8_t hid_printer_data_handler (void *pudev, usb_dir_enum rx_tx, uint8_t ep_id)
{
    if (((CUSTOMHID_IN_EP & 0x7F) == ep_id) || (ep_id == (CUSTOMHID_OUT_EP & 0x7F))) {
        return custom_hid_data_handler(pudev, rx_tx, ep_id);
    } else {
        return printer_data_handler(pudev, rx_tx, ep_id);
    }
}
