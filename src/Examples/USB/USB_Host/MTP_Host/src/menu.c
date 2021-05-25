/*!
    \file  menu.c 
    \brief this file implements Menu Functions

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

#include "menu.h"
#include "usbh_int.h"

uint8_t                     prev_select = 0;
mtp_demo_statemachine       mtp_demo;
mtp_demo_selectmode         mtp_select_mode;
audio_playback_statetypedef audio_state;

uint8_t *mtp_main_menu[] = 
{
    (uint8_t *)"    1 - Explore audio file                   ",
    (uint8_t *)"    2 - Start audio Player                   ",
};

/*!
    \brief      demo state machine
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mtp_menuinit(void)
{
    mtp_demo.state = MTP_DEMO_IDLE;
    mtp_select_mode = MTP_SELECT_MENU;
    mtp_selectitem(mtp_main_menu, 0); 
}

/*!
    \brief      manages MTP menu process
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mtp_menuprocess(usb_core_handle_struct *pudev, usbh_host_struct *puhost)
{
    switch (mtp_demo.state) {
        case MTP_DEMO_IDLE:
            mtp_selectitem(mtp_main_menu, 0); 
            mtp_demo.state = MTP_DEMO_WAIT;
            mtp_demo.select = 0;
            break;    
        
        case MTP_DEMO_WAIT:
            if (mtp_demo.select != prev_select) {
                prev_select = mtp_demo.select;
                mtp_selectitem(mtp_main_menu, mtp_demo.select & 0x7F);

                /* Handle select item */
                if (mtp_demo.select & 0x80) {
                    switch (mtp_demo.select & 0x7F) {
                        case 0:
                            mtp_demo.state = MTP_DEMO_EXPLORE;  
                            break;

                        case 1:
                            /* Set PLAYBACK state and start playing 1st file */ 
                            audio_state = AUDIO_STATE_IDLE;
                            mtp_demo.state = MTP_DEMO_PLAYBACK;
                            mtp_changeselectmode(MTP_PLAYBACK_CONTROL);
                            break;

                        case 2:
                            mtp_demo.state = MTP_REENUMERATE;  
                            break;

                        default:
                            break;
                    }
                }
            }
            break;
        
        case MTP_DEMO_EXPLORE:  
            if (appli_state == APPLICATION_READY) {
              if (mtp_init() == 0) {
                if (mtp_explorewavfile() > 0) {
                    lcd_log_print((uint8_t *)("There is no WAV file on the Smart Device.\n"), 0, LCD_COLOR_RED);
                    mtp_changeselectmode(MTP_SELECT_MENU);
                    mtp_demo.state = MTP_DEMO_IDLE;
                } else {
                    mtp_demo.state = MTP_DEMO_WAIT;
                }
              }
            } else {
                mtp_demo.state = MTP_DEMO_WAIT;
            }
            break; 
        
        case MTP_DEMO_PLAYBACK:
            if (appli_state == APPLICATION_READY) {
                if (mtp_init() == 0) {
                    if (audio_state == AUDIO_STATE_IDLE) {
                        /* Start Playing */
                        audio_state = AUDIO_STATE_INIT;
                        if (audio_start(0) == AUDIO_ERROR_IO) {
                             mtp_changeselectmode(MTP_SELECT_MENU); 
                             mtp_demo.state = MTP_DEMO_IDLE;
                        }
                    } else { /* Not idle */
                        if (audio_process() == AUDIO_ERROR_IO) {
                            mtp_changeselectmode(MTP_SELECT_MENU);  
                            mtp_demo.state = MTP_DEMO_IDLE;
                        }
                    }
                }
            } else {
                mtp_demo.state = MTP_DEMO_WAIT;
            }
            break; 

        default:
            break;
    }

    if (appli_state == APPLICATION_DISCONNECT) {
        appli_state = APPLICATION_IDLE;     
        mtp_changeselectmode(MTP_SELECT_MENU);     
    }

    mtp_demo.select &= 0x7F;
}

/*!
    \brief      manages the menu on the screen
    \param[in]  menu: menu table
	\param[in]  item: selected item to be highlighted
    \param[out] none
    \retval     none
*/
void mtp_selectitem(uint8_t **menu, uint8_t item)
{
    lcd_text_color_set(LCD_COLOR_MAGENTA);
    lcd_rectangle_fill(416, 0, 34 , 272);

    switch (item) {
        case 0:
            lcd_text_color_set(LCD_COLOR_GREEN);
            lcd_vertical_string_display(417, 0, menu [0]);
            lcd_text_color_set(LCD_COLOR_WHITE);
            lcd_vertical_string_display(433, 0, menu [1]);
            break;

        case 1:
            lcd_text_color_set(LCD_COLOR_WHITE);
            lcd_vertical_string_display(417, 0, menu [0]);
            lcd_text_color_set(LCD_COLOR_GREEN);
            lcd_vertical_string_display(433, 0, menu [1]);
            break;

        default:
            lcd_text_color_set(LCD_COLOR_WHITE);
            lcd_vertical_string_display(417, 0, menu [0]);
            lcd_vertical_string_display(433, 0, menu [1]);
            break; 
    }
}

/*!
    \brief      probes the MTP joystick state
    \param[in]  state: joystick state
    \param[out] none
    \retval     none
*/
void mtp_menuprobekey(joystate_typedef state)
{
    /* Handle Menu inputs */
    if ((state == JOY_UP) && (mtp_demo.select > 0)) {
        mtp_demo.select--;
    } else if ((state == JOY_DOWN) && (mtp_demo.select < 1)) {
        mtp_demo.select++;
    } else if (state == JOY_SEL) {
        if (usbh_mtp_isready(&usb_host) > 0) {
            mtp_demo.select |= 0x80;
        }
    }
}

/*!
    \brief      button state detection
    \param[in]  none
    \param[out] none
    \retval     button state
*/
joystate_typedef bsp_joy_getstate(void)
{
  uint8_t tmp = 0;   
  
  /* Read the status joystick pins */
  tmp = (((gd_eval_key_state_get(KEY_USER))  << 2 & 0x04)  | \
         ((gd_eval_key_state_get(KEY_TAMPER)) << 1 & 0x02) | \
         ((gd_eval_key_state_get(KEY_WAKEUP)) << 0 & 0x01) ) & 0x07;
    
    /* Check the pressed keys */  
    if (!(tmp & 0x04)) {
        return(joystate_typedef) JOY_SEL;
    } else if (!(tmp & 0x02)) {
        return(joystate_typedef) JOY_DOWN;
    } else if (!(tmp & 0x01)) {
        return(joystate_typedef) JOY_UP;
    } else { 
        return(joystate_typedef) JOY_NONE;
    }
}

/*!
    \brief      the callback function of key state
    \param[in]  none
    \param[out] none
    \retval     none
*/
void hal_joystick_menu_callback(void)
{
    static joystate_typedef joystate = JOY_NONE;

    /* Get the Joystick State */
    joystate = bsp_joy_getstate();

    if (joystate != JOY_NONE) {
        if (mtp_select_mode == MTP_SELECT_MENU) {
            mtp_menuprobekey(joystate); 
        } else if (mtp_select_mode == MTP_PLAYBACK_CONTROL) {
            audio_playbackprobekey(joystate);
        }
    }
}

/*!
    \brief      Changes the selection mode.
    \param[in]  select_mode: Selection mode
    \param[out] none
    \retval     none
*/
void mtp_changeselectmode(mtp_demo_selectmode select_mode)
{
    if (select_mode == MTP_SELECT_MENU) {
        lcd_text_color_set(LCD_COLOR_GREEN);
        lcd_log_text_zone_clear(TEXT_ZONE_START_X, TEXT_ZONE_START_Y, TEXT_ZONE_OFFSET_X, TEXT_ZONE_OFFSET_Y);
        mtp_selectitem(mtp_main_menu, 0x00);
    } else if(select_mode == MTP_PLAYBACK_CONTROL) {
        lcd_text_color_set(LCD_COLOR_GREEN);
        lcd_log_text_zone_clear(TEXT_ZONE_START_X, TEXT_ZONE_START_Y, TEXT_ZONE_OFFSET_X, TEXT_ZONE_OFFSET_Y);
        mtp_selectitem(mtp_main_menu, 0xFF);     
    }
    mtp_select_mode = select_mode; 
    mtp_demo.select = 0;
}
