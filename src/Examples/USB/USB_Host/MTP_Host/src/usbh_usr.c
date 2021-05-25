/*!
    \file  usbh_usr.c
    \brief user application layer for USBFS host-mode MSC class operation

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

#include <string.h>
#include "usbh_usr.h"
#include "lcd_log.h"
#include "menu.h"
#include "main.h"

extern usb_core_handle_struct          usb_core_dev;
extern uint16_t LINE;
extern uint8_t *mtp_main_menu[];

extern mtp_demo_statemachine       mtp_demo;
extern uint8_t                     prev_select;
extern mtp_demo_selectmode         mtp_select_mode;
extern audio_playback_statetypedef audio_state;


uint8_t line_idx = 0;
uint8_t need_clear = 0;

/*  points to the DEVICE_PROP structure of current device */
usbh_user_callback_struct usr_cb =
{
    usbh_user_init,
    usbh_user_deinit,
    usbh_user_device_connected,
    usbh_user_device_reset,
    usbh_user_device_disconnected,
    usbh_user_over_current_detected,
    usbh_user_device_speed_detected,
    usbh_user_device_desc_available,
    usbh_user_device_address_assigned,
    usbh_user_configuration_descavailable,
    usbh_user_manufacturer_string,
    usbh_user_product_string,
    usbh_user_serialnum_string,
    usbh_user_enumeration_finish,
    usbh_user_userinput,
    NULL,
    usbh_user_device_not_supported,
    usbh_user_unrecovered_error
};

const uint8_t MSG_HOST_INIT[]        = "> Host Library Initialized.";
const uint8_t MSG_DEV_ATTACHED[]     = "> Device Attached.";
const uint8_t MSG_DEV_DISCONNECTED[] = "> Device Disconnected.";
const uint8_t MSG_DEV_ENUMERATED[]   = "> Enumeration completed.";
const uint8_t MSG_DEV_HIGHSPEED[]    = "> High speed device detected.";
const uint8_t MSG_DEV_FULLSPEED[]    = "> Full speed device detected.";
const uint8_t MSG_DEV_LOWSPEED[]     = "> Low speed device detected.";
const uint8_t MSG_DEV_ERROR[]        = "> Device fault.";

const uint8_t MSG_HOST_HEADER[]      = "> USBFS MTP Host";
const uint8_t MSG_HOST_FOOTER[]      = "> USB Host Library v1.0.0";

const uint8_t MSG_LIB_START[]        = "## USB Host library started ##";
const uint8_t MSG_DEV_NOSUP[]        = "> Device not supported.";
const uint8_t MSG_OVERCURRENT[]      = "> Overcurrent detected.";
const uint8_t MSG_RESET_DEV[]        = "> Reset the USB device.";

const uint8_t MSG_MTP_CLASS[]        = "> MTP device connected.";
const uint8_t MSG_MSC_CLASS[]        = "> Mass storage device connected.";
const uint8_t MSG_HID_CLASS[]        = "> HID device connected.";

const uint8_t MSG_DISK_SIZE[]        = "> Size of the disk in MBytes: ";
const uint8_t MSG_LUN[]              = "> LUN Available in the device:";
const uint8_t MSG_ROOT_CONT[]        = "> Exploring disk flash ...";
const uint8_t MSG_WR_PROTECT[]       = "> The disk is write protected.";
const uint8_t MSG_UNREC_ERROR[]      = "> UNRECOVERED ERROR STATE.";
const uint8_t MSG_FILE_NOTINIT[]     = "> Cannot initialize File System.";
const uint8_t MSG_FILE_INIT[]        = "> File System initialized.";
const uint8_t MSG_Write_File[]       = "> Writing File to disk flash ...";
const uint8_t MSG_Write_Protect[]    = "> Disk flash is write protected ";
const uint8_t MSG_NOT_WRITE[]        = "> GD32.TXT CANNOT be writen.";
const uint8_t MSG_CREATE_FILE[]      = "> GD32.TXT created in the disk.";

/*!
    \brief      user operation for host-mode initialization
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_init(void)
{
    static uint8_t startup = 0U;

    if (0U == startup) {
        startup = 1U;

        /* configure the LEDs and KEYs*/
        gd_eval_led_init(LED3);
        gd_eval_led_init(LED2);
        gd_eval_led_init(LED1);
        gd_eval_key_init(KEY_USER, KEY_MODE_GPIO);
        gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);
        gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);

        lcd_init();
        lcd_layer_init();
        tli_layer_enable(LAYER0);
        tli_layer_enable(LAYER1);
        tli_enable();
      
        lcd_layer_set(LCD_LAYER_BACKGROUND);
        lcd_transparency_set(255);
        lcd_clear(LCD_COLOR_GREEN);
        
        lcd_layer_set(LCD_LAYER_FOREGROUND);
        lcd_transparency_set(0);
        lcd_clear(LCD_COLOR_GREEN);
        
        lcd_log_init();
        lcd_layer_set(LCD_LAYER_BACKGROUND);
        lcd_log_header_set((uint8_t *)MSG_HOST_HEADER,0);

        lcd_log_print((uint8_t *)MSG_LIB_START, 
                      0, 
                      LCD_COLOR_BLACK);

        lcd_log_footer_set((uint8_t *)MSG_HOST_FOOTER,0);
        
        mtp_menuinit();
        
        audio_init();
    }
}

/*!
    \brief      user operation for device attached
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_connected(void)
{
    if (need_clear != 0) {
        lcd_text_color_set(LCD_COLOR_WHITE);
        lcd_log_text_zone_clear(TEXT_ZONE_START_X, TEXT_ZONE_START_Y, TEXT_ZONE_OFFSET_X, TEXT_ZONE_OFFSET_Y);
        need_clear = 0;
    }

    lcd_log_print((uint8_t *)MSG_DEV_ATTACHED, 0, LCD_COLOR_BLACK);
}

/*!
    \brief      user operation when unrecoveredError happens
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_unrecovered_error (void)
{
    lcd_log_print((uint8_t *)MSG_UNREC_ERROR, 0, LCD_COLOR_BLACK);
}

/*!
    \brief      user operation for device disconnect event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_disconnected (void)
{
    LINE = 30;
    lcd_text_color_set(LCD_COLOR_WHITE);
    lcd_log_text_zone_clear(TEXT_ZONE_START_X, TEXT_ZONE_START_Y, TEXT_ZONE_OFFSET_X, TEXT_ZONE_OFFSET_Y);

    lcd_log_print((uint8_t *)MSG_DEV_DISCONNECTED, 0, LCD_COLOR_BLACK);

    need_clear = 1;
}

/*!
    \brief      user operation for reset USB Device
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_reset(void)
{
    lcd_log_print((uint8_t *)MSG_RESET_DEV, 0, LCD_COLOR_BLACK);
  
}

/*!
    \brief      user operation for detectting device speed
    \param[in]  device_speed: device speed
    \param[out] none
    \retval     none
*/
void usbh_user_device_speed_detected(uint8_t device_speed)
{
    
    if (HPRT_PRTSPD_HIGH_SPEED == device_speed) {
        lcd_log_print((uint8_t *)MSG_DEV_HIGHSPEED, 
                      0, LCD_COLOR_BLACK);
    } else if(HPRT_PRTSPD_FULL_SPEED == device_speed) {
        lcd_log_print((uint8_t *)MSG_DEV_FULLSPEED, 
                      0, LCD_COLOR_BLACK);
    } else if(HPRT_PRTSPD_LOW_SPEED == device_speed) {
        lcd_log_print((uint8_t *)MSG_DEV_LOWSPEED, 
                      0, LCD_COLOR_BLACK);
    } else {
        lcd_log_print((uint8_t *)MSG_DEV_ERROR, 
                      0, LCD_COLOR_BLACK);
    }
}

/*!
    \brief      user operation when device descriptor is available
    \param[in]  device_desc: device descriptor
    \param[out] none
    \retval     none
*/
void usbh_user_device_desc_available(void *device_desc)
{
    uint8_t tempstr[64];
    usb_descriptor_device_struct *pDevStr = device_desc;

    sprintf((char *)tempstr, "VID: %04Xh", (uint32_t)pDevStr->idVendor);
    lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);

    sprintf((char *)tempstr, "PID: %04Xh" , (uint32_t)pDevStr->idProduct);
    lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
}

/*!
    \brief      usb device is successfully assigned the Address 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_address_assigned(void)
{
}

/*!
    \brief      user operation when configuration descriptor is available
    \param[in]  cfg_desc: pointer to configuration descriptor
    \param[in]  itf_desc: pointer to interface descriptor
    \param[in]  ep_desc: pointer to endpoint descriptor
    \param[out] none
    \retval     none
*/
void usbh_user_configuration_descavailable(usb_descriptor_configuration_struct *cfg_desc,
                                          usb_descriptor_interface_struct *itf_desc,
                                          usb_descriptor_endpoint_struct *ep_desc)
{
    usb_descriptor_interface_struct *id = itf_desc;  

    if (0x08U  == (*id).bInterfaceClass) {
        lcd_log_print((uint8_t *)MSG_MSC_CLASS, 0, LCD_COLOR_BLACK);
      
    } else if (0x03U  == (*id).bInterfaceClass) {
        lcd_log_print((uint8_t *)MSG_HID_CLASS, 0, LCD_COLOR_BLACK);
        
    } else if (0xFFU  == (*id).bInterfaceClass) {
        lcd_log_print((uint8_t *)MSG_MTP_CLASS, 0, LCD_COLOR_BLACK);
    }
}

/*!
    \brief      user operation when manufacturer string exists
    \param[in]  manufacturer_string: manufacturer string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_manufacturer_string(void *manufacturer_string)
{
    uint8_t tempstr[64];

    sprintf((char *)tempstr, "Manufacturer: %s", (char *)manufacturer_string);
    lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
}

/*!
    \brief      user operation when product string exists
    \param[in]  product_string: product string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_product_string(void *product_string)
{
    uint8_t tempstr[64];

    sprintf((char *)tempstr, "Product: %s", (char *)product_string);
    lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
}

/*!
    \brief      user operatin when serialNum string exists
    \param[in]  serial_num_string: serialNum string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_serialnum_string(void *serial_num_string)
{
    uint8_t tempstr[64];

    sprintf((char *)tempstr, "Serial Number: %s", (char *)serial_num_string);
    lcd_log_print((uint8_t *)tempstr, 0, LCD_COLOR_BLACK);
}

/*!
    \brief      user response request is displayed to ask for application jump to class
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_enumeration_finish(void)
{
    uint8_t Str1[] = "To see the Mobile Device information: ";
    uint8_t Str2[] = "Press User Key...";
    
    lcd_log_print((uint8_t *)MSG_DEV_ENUMERATED, 0, LCD_COLOR_BLACK);
    lcd_log_print((uint8_t *)Str1, 0, LCD_COLOR_RED);
    lcd_log_print((uint8_t *)Str2, 0, LCD_COLOR_RED);
}

/*!
    \brief      user operation when device is not supported
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_not_supported(void)
{
    lcd_log_print((uint8_t *)MSG_DEV_NOSUP, 0, LCD_COLOR_BLACK);
}

/*!
    \brief      user action for application state entry
    \param[in]  none
    \param[out] none
    \retval     user response for user key
*/
usbh_user_status_enum usbh_user_userinput(void)
{
    usbh_user_status_enum usbh_usr_status = USBH_USER_NO_RESP;

    /*key B3 is in polling mode to detect user action */
    if (RESET == gd_eval_key_state_get(KEY_USER)) {
        lcd_text_color_set(LCD_COLOR_GREEN);
        lcd_log_text_zone_clear(TEXT_ZONE_START_X, TEXT_ZONE_START_Y, TEXT_ZONE_OFFSET_X, TEXT_ZONE_OFFSET_Y);
        usbh_usr_status = USBH_USER_RESP_OK;
    }

    return usbh_usr_status;
}

/*!
    \brief      user operation for device over-current detection event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_over_current_detected (void)
{
    lcd_log_print((uint8_t *)MSG_OVERCURRENT, 0, LCD_COLOR_BLACK);
}

/*!
    \brief      deinit user state and associated variables
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_deinit(void)
{
     mtp_demo.state = MTP_DEMO_IDLE;
}
