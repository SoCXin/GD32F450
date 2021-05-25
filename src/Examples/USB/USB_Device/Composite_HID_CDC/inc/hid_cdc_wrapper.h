/*!
    \file  hid_cdc_wrapper.h
    \brief header file for the hid_cdc_wrapper.c file
    

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

#ifndef HID_CDC_WRAPPER_H
#define HID_CDC_WRAPPER_H

#include "custom_hid_core.h"
#include "cdc_acm_core.h"

#define HID_INTERFACE                0x00
#define CDC_COM_INTERFACE            0x01
#define HID_CDC_CONFIG_DESC_SIZE     0x6B

#pragma pack(1)

typedef struct
{
    usb_descriptor_header_struct Header;  /*!< regular descriptor header containing the descriptor's type and length */
    uint8_t bFirstInterface;              /*!< bFirstInterface */
    uint8_t bInterfaceCount;              /*!< bInterfaceCount */
    uint8_t bFunctionClass;               /*!< bFunctionClass */
    uint8_t bFunctionSubClass;            /*!< bFunctionSubClass */
    uint8_t bFunctionProtocol;            /*!< bFunctionProtocol */
    uint8_t iFunction;                    /*!< iFunction  */
} usb_descriptor_IAD_function_struct;

#pragma pack()

typedef struct
{
    usb_descriptor_configuration_struct               Config;
    usb_descriptor_interface_struct                   HID_Interface;
    usb_hid_descriptor_hid_struct                     HID_VendorHID;
    usb_descriptor_endpoint_struct                    HID_ReportINEndpoint;
    usb_descriptor_endpoint_struct                    HID_ReportOUTEndpoint;
    usb_descriptor_IAD_function_struct                IAD;
    usb_descriptor_interface_struct                   cdc_loopback_interface;
    usb_descriptor_header_function_struct             cdc_loopback_header;
    usb_descriptor_call_managment_function_struct     cdc_loopback_call_managment;
    usb_descriptor_acm_function_struct                cdc_loopback_acm;
    usb_descriptor_union_function_struct              cdc_loopback_union;
    usb_descriptor_endpoint_struct                    cdc_loopback_cmd_endpoint;
    usb_descriptor_interface_struct                   cdc_loopback_data_interface;
    usb_descriptor_endpoint_struct                    cdc_loopback_out_endpoint;
    usb_descriptor_endpoint_struct                    cdc_loopback_in_endpoint;
} usb_hid_cdc_descriptor_configuration_set_struct;

extern const usb_descriptor_device_struct device_descripter;
extern const usb_hid_cdc_descriptor_configuration_set_struct configuration_descriptor;
extern uint8_t* usbd_strings[];

/* function declarations */
/* initialize the HID/CDC device */
uint8_t hid_cdc_init (void *pudev, uint8_t config_index);
/* de-initialize the HID/CDC device */
uint8_t hid_cdc_deinit (void *pudev, uint8_t config_index);
/* handle the custom HID/CDC class-specific and standard requests */
uint8_t hid_cdc_req_handler (void *pudev, usb_device_req_struct *req);
/* handle data stage */
uint8_t hid_cdc_data_handler (void *pudev, usb_dir_enum rx_tx, uint8_t ep_id);

#endif  /* HID_CDC_WRAPPER_H */
