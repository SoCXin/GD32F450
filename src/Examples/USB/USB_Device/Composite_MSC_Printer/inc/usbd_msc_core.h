/*!
    \file  usbd_msc_core.h
    \brief the header file of USB MSC device class core functions

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

#ifndef USBD_MSC_CORE_H
#define USBD_MSC_CORE_H

#include "usbd_core.h"
#include "usbd_std.h"

#define STORAGE_LUN_NUM                   1

#define MSC_EPIN_SIZE                     MSC_DATA_PACKET_SIZE
#define MSC_EPOUT_SIZE                    MSC_DATA_PACKET_SIZE

#define BBB_GET_MAX_LUN                   0xFE
#define BBB_RESET                         0xFF

#define USB_LANGID_STRING_SIZE            0x04
#define USB_MSC_CONFIG_DESC_SIZE          32

/* USB configuration descriptor struct */
typedef struct
{
    usb_descriptor_configuration_struct        config;

    usb_descriptor_interface_struct            msc_interface;
    usb_descriptor_endpoint_struct             msc_in_endpoint;
    usb_descriptor_endpoint_struct             msc_out_endpoint;
} usb_msc_descriptor_configuration_set_struct;

extern uint8_t* usbd_msc_strings[USB_STRING_COUNT];
extern const usb_descriptor_device_struct msc_device_descripter;
extern const usb_msc_descriptor_configuration_set_struct msc_configuration_descriptor;

/* function declarations */
/* initialize the MSC device */
uint8_t msc_init (void *pudev, uint8_t config_index);
/* de-initialize the MSC device */
uint8_t msc_deinit (void *pudev, uint8_t config_index);
/* handle the MSC class-specific and standard requests */
uint8_t msc_req_handler (void *pudev, usb_device_req_struct *req);
/* handle data stage */
uint8_t msc_data_handler (void *pudev, usb_dir_enum rx_tx, uint8_t ep_id);

#endif  /* USBD_MSC_CORE_H */
