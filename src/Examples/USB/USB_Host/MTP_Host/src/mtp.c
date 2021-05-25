/*!
    \file  mtp.c
    \brief this file provides APIs to explore MTP storage objects

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

#include "mtp.h"
#include "main.h"
#include "lcd_log.h"


uint32_t numobs = 0;
mtp_objecthandlestypedef wavhandles;

static uint8_t mtp_getwavobjecthandles(void);


/*!
    \brief      start audio streaming
    \param[in]  none
    \param[out] none
    \retval     none
*/
uint8_t mtp_init(void)
{
    static uint8_t is_initialized = 0;
    uint8_t ret = 1;
  
    if (is_initialized == 0) {
        if (usbh_mtp_isready(&usb_host) > 0) { 
            if (usbh_mtp_getnumobjects(&usb_core_dev, &usb_host, 0, PTP_OFC_WAV, PTP_AT_Undefined, &numobs) == USBH_OK) {
                /* Get objects handlers */
                if(mtp_getwavobjecthandles() == 0) {
                  is_initialized = 1;
                  ret = 0;
                }
            }
        }
    } else {
        ret = 0;  
    }

    return ret;
}

/*!
    \brief      explore wav files
    \param[in]  none
    \param[out] none
    \retval     return 0 if OK, otherwise 1
*/
uint8_t mtp_explorewavfile(void)
{
    uint8_t ret = 1;
    uint32_t index;
    mtp_objectinfotypedef objectinfo;
    uint8_t tempstr[128];

    mtp_init();

    if (usbh_mtp_isready(&usb_host) > 0) {
        lcd_log_print((uint8_t *)("Available wav files:"), 0, LCD_COLOR_LIGHTRED);

        /* Get Available WAV files number */
        if ((numobs = mtp_getwavobjectnumber()) > 0) {
            /* Get objects handlers */
            if (mtp_getwavobjecthandles() == 0) {
                ret = 0; 

                for (index = 0; index < numobs; index ++) {
                    if( usbh_mtp_getobjectinfo (&usb_core_dev, &usb_host,
                                              wavhandles.handler[index],
                                              &objectinfo) == USBH_OK) {
                        sprintf((char *)tempstr, " %lu- %s", (unsigned long)index, (char *)objectinfo.Filename);
                        lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
                    } else {
                        ret = 1; 
                    }
                }
            }
        }
    } else {
        lcd_log_print((uint8_t *)("MTP Device Not yet ready...\n"), 0, LCD_COLOR_RED);
    }

    return ret;
}

/*!
    \brief      gets data from MTP device
    \param[in]  file_idx: file index
	\param[in]  offset: offset
	\param[in]  maxbytes: max bytes
	\param[in]  object: pointer to the file object
	\param[in]  len: pointer to the file length
    \param[out] none
    \retval     return status 0 if OK, otherwise 1
*/
uint8_t mtp_getdata(uint32_t file_idx, uint32_t offset, uint32_t maxbytes, uint8_t *object, uint32_t *len)
{ 
    usbh_mtp_getpartialobject(&usb_core_dev, &usb_host, 
                            wavhandles.handler[file_idx], 
                            offset,
                            maxbytes, 
                            object,
                            len);
    return 0;
}

/*!
    \brief      gets wav object number
    \param[in]  none
    \param[out] none
    \retval     none
*/
uint16_t mtp_getwavobjectnumber(void)
{
    return numobs;
}

/*!
    \brief      gets wav object names
    \param[in]  object_index: object index
	\param[in]  filename: pointer to the file name
    \param[out] none
    \retval     none
*/
uint8_t mtp_getwavobjectname(uint16_t object_index, uint8_t *filename)
{
    uint8_t ret = 1;
    mtp_objectinfotypedef objectinfo;

    if (usbh_mtp_getobjectinfo(&usb_core_dev, &usb_host, wavhandles.handler[object_index], &objectinfo) == USBH_OK) {
        strcpy((char *)filename, (char *)objectinfo.Filename);
        ret = 0;
    }

    return ret;
}

/*!
    \brief      get wav object handler
    \param[in]  none
    \param[out] none
    \retval     return status 0 if OK, otherwise 1
*/
static uint8_t mtp_getwavobjecthandles(void)
{ 
    /* Get objects handlers */
    if (usbh_mtp_getobjecthandles(&usb_core_dev, &usb_host, 0, PTP_OFC_WAV, PTP_AT_Undefined, &wavhandles) == USBH_OK) {
        return 0;
    }

    return 1;
}
