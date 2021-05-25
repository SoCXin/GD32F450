/*!
    \file  main.h
    \brief header for main.c module

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

#ifndef MAIN_H
#define MAIN_H

#include "usbh_mtp.h" 
#include "usbh_mtp_ptp.h"
#include "usb_delay.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_std.h"
#include "usbh_ctrl.h"
#include "exmc_sdram.h"
#include "gd32f450i_eval.h"
#include "lcd_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    MTP_DEMO_IDLE = 0,
    MTP_DEMO_WAIT,  
    MTP_DEMO_EXPLORE,
    MTP_DEMO_PLAYBACK,
    MTP_REENUMERATE,  
}MTP_Demo_State;

typedef enum 
{ 
  JOY_NONE  = 0,
  JOY_SEL   = 1,
  JOY_DOWN  = 2,
  JOY_UP    = 3,
}joystate_typedef;

typedef struct _DemoStateMachine {
  __IO MTP_Demo_State              state;
  __IO uint8_t                     select;  
}mtp_demo_statemachine;

typedef struct AUDIO_Info_t {
    uint32_t ChunkID;       /* 0  */ 
    uint32_t FileSize;      /* 4  */
    uint32_t FileFormat;    /* 8  */
    uint32_t SubChunk1ID;   /* 12 */
    uint32_t SubChunk1Size; /* 16 */  
    uint16_t AudioFormat;   /* 20 */ 
    uint16_t NbrChannels;   /* 22 */   
    uint32_t SampleRate;    /* 24 */ 
    uint32_t ByteRate;      /* 28 */
    uint16_t BlockAlign;    /* 32 */  
    uint16_t BitPerSample;  /* 34 */  
    uint32_t SubChunk2ID;   /* 36 */   
    uint32_t SubChunk2Size; /* 40 */    
}wav_infotypedef;

typedef enum {
    AUDIO_DEMO_IDLE = 0,
    AUDIO_DEMO_WAIT,  
    AUDIO_DEMO_EXPLORE,
    AUDIO_DEMO_PLAYBACK,
    AUDIO_REENUMERATE  
}AUDIO_Demo_State;

typedef enum {
    APPLICATION_IDLE = 0,  
    APPLICATION_START,   
    APPLICATION_READY,
    APPLICATION_DISCONNECT,
}mtp_applicationtypedef;

typedef enum {
    AUDIO_STATE_IDLE = 0,
    AUDIO_STATE_WAIT,    
    AUDIO_STATE_INIT,    
    AUDIO_STATE_CONFIG,    
    AUDIO_STATE_PLAY,  
    AUDIO_STATE_NEXT,  
    AUDIO_STATE_PREVIOUS,
    AUDIO_STATE_FORWARD,   
    AUDIO_STATE_BACKWARD,
    AUDIO_STATE_STOP,   
    AUDIO_STATE_PAUSE,
    AUDIO_STATE_RESUME,
    AUDIO_STATE_VOLUME_UP,
    AUDIO_STATE_VOLUME_DOWN,  
}audio_playback_statetypedef;

typedef enum {
    AUDIO_ERROR_NONE = 0,  
    AUDIO_ERROR_IO,   
    AUDIO_ERROR_EOF,
}audio_errortypedef;

typedef enum {
    MTP_ERROR_NONE = 0,  
    MTP_ERROR_IO,  
    MTP_ERROR_INVALID_VALUE,     
}MTP_ErrorTypeDef;

typedef enum {
    MTP_SELECT_MENU = 0,
    MTP_PLAYBACK_CONTROL ,  
}mtp_demo_selectmode;

extern usb_core_handle_struct usb_core_dev;
extern usbh_host_struct usb_host;
extern mtp_applicationtypedef appli_state;
extern audio_playback_statetypedef audio_state;

void toggle_leds(void);
void mtp_menuinit(void);
void mtp_menuprocess(usb_core_handle_struct *pudev, usbh_host_struct *puhost);
void audio_playbackprobekey(joystate_typedef state);
uint8_t mtp_explorewavfile(void);
uint8_t mtp_getdata(uint32_t file_idx, uint32_t offset, uint32_t maxbytes, 
                    uint8_t *object, uint32_t *len);
uint8_t mtp_getwavobjectname(uint16_t object_index, uint8_t *filename);
uint8_t mtp_init(void);
uint16_t mtp_getwavobjectnumber(void);
audio_errortypedef audio_init(void);
audio_errortypedef audio_start(uint8_t idx);
audio_errortypedef audio_process(void);
audio_errortypedef AUDIO_Stop(void);

#endif /* MAIN_H */
