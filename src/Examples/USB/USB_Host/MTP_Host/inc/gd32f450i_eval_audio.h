/*!
    \file  gd32f450i_eval_audio.h
    \brief this file contains all the prototypes for the gd32f450i_eval_audio.c

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

#ifndef GD32F450I_EVAL_AUDIO_H
#define GD32F450I_EVAL_AUDIO_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Include audio component Driver */
#include "gd32f450i_eval.h"

/* Codec output DEVICE */
#define OUTPUT_DEVICE_SPEAKER         1
#define OUTPUT_DEVICE_HEADPHONE       2
#define OUTPUT_DEVICE_BOTH            3
#define OUTPUT_DEVICE_AUTO            4


/* I2S_Audio_Frequency I2S Audio Frequency */
#define I2S_AUDIOFREQ_192K               ((uint32_t)192000)
#define I2S_AUDIOFREQ_96K                ((uint32_t)96000)
#define I2S_AUDIOFREQ_48K                ((uint32_t)48000)
#define I2S_AUDIOFREQ_44K                ((uint32_t)44100)
#define I2S_AUDIOFREQ_32K                ((uint32_t)32000)
#define I2S_AUDIOFREQ_22K                ((uint32_t)22050)
#define I2S_AUDIOFREQ_16K                ((uint32_t)16000)
#define I2S_AUDIOFREQ_11K                ((uint32_t)11025)
#define I2S_AUDIOFREQ_8K                 ((uint32_t)8000)
#define I2S_AUDIOFREQ_DEFAULT            ((uint32_t)2)

/* Audio status definition */     
#define AUDIO_OK                            0
#define AUDIO_ERROR                         1
#define AUDIO_TIMEOUT                       2

#define DMA_MAX(x)           (((x) <= DMA_MAX_SZE)? (x):DMA_MAX_SZE)

uint8_t bsp_audio_out_init(uint16_t outputdevice, uint8_t volume, uint32_t audiofreq);
void    audio_mal_play(uint32_t addr, uint32_t size);
void    bsp_audio_out_changebuffer(uint16_t *pdata, uint16_t size);
uint8_t bsp_audio_out_pause(uint32_t audiofreq);
uint8_t bsp_audio_out_resume(void);
uint8_t bsp_audio_out_stop(uint32_t option);
void    audio_mal_init(void);

/* User Callbacks: user has to implement these functions in his code if they are needed. */
/* This function is called when the requested data has been completely transferred.*/
void    bsp_audio_out_transfercomplete_callback(void);

/* This function is called when half of the requested buffer has been transferred. */
void    bsp_audio_out_halftransfer_callback(void);


#ifdef __cplusplus
}
#endif

#endif /* GD32F450I_EVAL_AUDIO_H */
