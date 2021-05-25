/*!
    \file  audio.c
    \brief this file provides the Audio Out (playback) interface API

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

#include "audio.h"
#include "main.h"
#include "gd32f4xx_spi.h"
#include "gd32f450i_eval_audio.h"

#define DEFAULT_AUDIOFREQ  I2S_AUDIOFREQ_44K

audio_buffertypedef  buffer_ctl;
static wav_infotypedef wav_info;
static int16_t file_pos = 0;
static __IO uint32_t uwvolume = 40;
static audio_errortypedef audio_getfileinfo(uint16_t file_idx, wav_infotypedef *info);

/*!
    \brief      initializes Audio Interface
	\param[in]  none
    \param[out] none
    \retval     audio error
*/
audio_errortypedef audio_init(void)
{
    if (bsp_audio_out_init(OUTPUT_DEVICE_AUTO, uwvolume, DEFAULT_AUDIOFREQ) == 0) {
        return AUDIO_ERROR_NONE;
    }

    return AUDIO_ERROR_IO;
}

/*!
    \brief      starts audio streaming
	\param[in]  idx: file index
    \param[out] none
    \retval     audio error
*/  
audio_errortypedef audio_start(uint8_t idx)
{
    uint32_t bytesread;

    if (mtp_getwavobjectnumber() > idx) {
        lcd_text_color_set(LCD_COLOR_GREEN);
        lcd_log_text_zone_clear(TEXT_ZONE_START_X, TEXT_ZONE_START_Y, TEXT_ZONE_OFFSET_X, TEXT_ZONE_OFFSET_Y);

        audio_getfileinfo(idx, &wav_info);

        /* CODEC_I2S peripheral configuration */
        i2s_psc_config(SPI1, wav_info.SampleRate, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_ENABLE);

        /* Enable the I2S DMA request */
        spi_dma_enable(SPI1, SPI_DMA_TRANSMIT);

        buffer_ctl.offset = BUFFER_OFFSET_NONE;
        
        /* Judge whether the wav file is single channel */
        if (wav_info.NbrChannels == 1) {
            if (mtp_getdata(idx, 44,AUDIO_BUFFER_SIZE/2, &buffer_ctl.buff[0],&bytesread) == 0) {
                /* If a single channel, then half a word would be sent twice */
                for (int i = AUDIO_BUFFER_SIZE/2 -1; i >= 0; i -= 2) {
                    buffer_ctl.buff[i*2-2] = buffer_ctl.buff[i-1];
                    buffer_ctl.buff[i*2-1] = buffer_ctl.buff[i];
                    buffer_ctl.buff[i*2] = buffer_ctl.buff[i-1];
                    buffer_ctl.buff[i*2+1] = buffer_ctl.buff[i];
                }
                
                audio_state = AUDIO_STATE_PLAY;

                if (bytesread != 0) {
                    dma_interrupt_enable(DMA0, DMA_CH4, DMA_CHXCTL_FTFIE|DMA_CHXCTL_HTFIE);
                    nvic_irq_enable((uint8_t)DMA0_Channel4_IRQn, 1U, 0U);

                    audio_mal_play((uint32_t)&buffer_ctl.buff[0], AUDIO_BUFFER_SIZE/2);
                    buffer_ctl.fptr = bytesread;

                    return AUDIO_ERROR_NONE;
                }
            }
        } else if (wav_info.NbrChannels == 2) {
            if (mtp_getdata(idx, 44,AUDIO_BUFFER_SIZE, &buffer_ctl.buff[0],&bytesread) == 0) {
                audio_state = AUDIO_STATE_PLAY;

                if (bytesread != 0) {
                    dma_interrupt_enable(DMA0, DMA_CH4, DMA_CHXCTL_FTFIE|DMA_CHXCTL_HTFIE);
                    nvic_irq_enable((uint8_t)DMA0_Channel4_IRQn, 1U, 0U);

                    audio_mal_play((uint32_t)&buffer_ctl.buff[0], AUDIO_BUFFER_SIZE/2);
                    buffer_ctl.fptr = bytesread;

                    return AUDIO_ERROR_NONE;
                }
            }
        }
    }
    return AUDIO_ERROR_IO;
}

/*!
    \brief      manages audio process
	\param[in]  none
    \param[out] none
    \retval     audio error
*/
audio_errortypedef audio_process(void)
{
    uint32_t bytesread;
    audio_errortypedef error_state = AUDIO_ERROR_NONE;

    switch (audio_state) {
        case AUDIO_STATE_PLAY:
            if (buffer_ctl.fptr >= wav_info.FileSize) {
                error_state = AUDIO_ERROR_EOF;
            }

            if (buffer_ctl.offset == BUFFER_OFFSET_HALF) {
                /* Judge whether the wav file is single channel */
                if (wav_info.NbrChannels == 1) {
                    if (mtp_getdata(file_pos, 
                             buffer_ctl.fptr, 
                             AUDIO_BUFFER_SIZE/4,
                             &buffer_ctl.buff[0],&bytesread)> 0) {
                        error_state = AUDIO_ERROR_IO;
                    }
                    /* If a single channel, then half a word would be sent twice */
                    for (int i = AUDIO_BUFFER_SIZE/4 - 1; i >= 0; i -= 2) {
                        buffer_ctl.buff[i*2-2] = buffer_ctl.buff[i-1];
                        buffer_ctl.buff[i*2-1] = buffer_ctl.buff[i];
                        buffer_ctl.buff[i*2] = buffer_ctl.buff[i-1];
                        buffer_ctl.buff[i*2+1] = buffer_ctl.buff[i];
                    }
                /* Judge whether the wav file is dual channel */
                } else if (wav_info.NbrChannels == 2) {
                    if (mtp_getdata(file_pos, buffer_ctl.fptr, AUDIO_BUFFER_SIZE/2,
                                                  &buffer_ctl.buff[0],&bytesread)> 0) { 
                        error_state = AUDIO_ERROR_IO;
                    }
                }
                buffer_ctl.offset = BUFFER_OFFSET_NONE;
                buffer_ctl.fptr += bytesread;
            }

            if (buffer_ctl.offset == BUFFER_OFFSET_FULL) {
                /* Judge whether the wav file is single channel */
                if (wav_info.NbrChannels == 1) {
                    if (mtp_getdata(file_pos,
                             buffer_ctl.fptr,
                             AUDIO_BUFFER_SIZE/4, 
                             &buffer_ctl.buff[AUDIO_BUFFER_SIZE/2],
                             &bytesread)> 0) {
                        error_state = AUDIO_ERROR_IO;
                    }
                    /* If a single channel, then half a word would be sent twice */
                    for (int i = AUDIO_BUFFER_SIZE/4 -1; i >= 0; i-=2) {
                        buffer_ctl.buff[i*2+AUDIO_BUFFER_SIZE/2-2] = buffer_ctl.buff[i+AUDIO_BUFFER_SIZE/2-1];
                        buffer_ctl.buff[i*2+AUDIO_BUFFER_SIZE/2-1] = buffer_ctl.buff[i+AUDIO_BUFFER_SIZE/2];
                        buffer_ctl.buff[i*2+AUDIO_BUFFER_SIZE/2] = buffer_ctl.buff[i+AUDIO_BUFFER_SIZE/2-1];
                        buffer_ctl.buff[i*2+AUDIO_BUFFER_SIZE/2+1] = buffer_ctl.buff[i+AUDIO_BUFFER_SIZE/2];
                    }
                } else if (wav_info.NbrChannels == 2) {
                    if (mtp_getdata(file_pos, buffer_ctl.fptr, AUDIO_BUFFER_SIZE/2, 
                        &buffer_ctl.buff[AUDIO_BUFFER_SIZE/2], &bytesread)> 0) {
                        error_state = AUDIO_ERROR_IO;
                    }
                }
                
                buffer_ctl.offset = BUFFER_OFFSET_NONE;
                buffer_ctl.fptr += bytesread; 
            }
            break;

        case AUDIO_STATE_NEXT:
            if (++file_pos >= mtp_getwavobjectnumber()) {
                file_pos = 0; 
            }
            bsp_audio_out_stop(0);
            audio_start(file_pos);
            break;    

        case AUDIO_STATE_PREVIOUS:
            if (--file_pos < 0) {
                file_pos = mtp_getwavobjectnumber() - 1; 
            }
            bsp_audio_out_stop(0);
            audio_start(file_pos);
            break;   

        case AUDIO_STATE_PAUSE:
            bsp_audio_out_pause(wav_info.SampleRate);
            audio_state = AUDIO_STATE_WAIT;
            break;

        case AUDIO_STATE_RESUME:
            bsp_audio_out_resume();
            audio_state = AUDIO_STATE_PLAY;
            break;

        case AUDIO_STATE_STOP:
        case AUDIO_STATE_VOLUME_UP: 
        case AUDIO_STATE_VOLUME_DOWN:    
        case AUDIO_STATE_WAIT:
        case AUDIO_STATE_CONFIG:    
        case AUDIO_STATE_IDLE:
        case AUDIO_STATE_INIT:    
        default:
            /* Do Nothing */
            break;
    }

    return error_state;
}

/*!
    \brief      probes the playback key state
	\param[in]  state: key state
    \param[out] none
    \retval     none
*/
void audio_playbackprobekey(joystate_typedef state)
{
    /* Handle File List Selection */ 
    if (state == JOY_DOWN) {
        audio_state = AUDIO_STATE_NEXT;
    } else if (state == JOY_UP) {
        audio_state = AUDIO_STATE_PREVIOUS;
    } else if (state == JOY_SEL) {
        audio_state = (audio_state == AUDIO_STATE_WAIT) ? AUDIO_STATE_RESUME : AUDIO_STATE_PAUSE;
    }
}

/*!
    \brief      gets the file information
	\param[in]  file_idx: file index
	\param[in]  info: pointer to wav file information
    \param[out] none
    \retval     audio error
*/
static audio_errortypedef audio_getfileinfo(uint16_t file_idx, wav_infotypedef *info)
{
    uint8_t str [FILE_NAME_SIZE + 20];  
    uint8_t fname [FILE_NAME_SIZE];    
    uint32_t bytesread;
    uint32_t duration;

    if (mtp_getwavobjectname(file_idx, fname) == 0) {
        if (mtp_getdata(file_idx, 0, sizeof(wav_infotypedef), (uint8_t*)&wav_info, &bytesread) == 0) {
            sprintf((char *)str, "Playing file (%d/%d): %s", 
                     file_idx + 1, mtp_getwavobjectnumber(),(char *)fname);
            lcd_log_print((uint8_t *)str, 0, LCD_COLOR_GREY);

            sprintf((char *)str , "Sample rate : %lu Hz", (unsigned long)info->SampleRate);
            lcd_log_print((uint8_t *)str, 0, LCD_COLOR_CYAN);

            sprintf((char *)str , "Channels number : %d", info->NbrChannels);
            lcd_log_print((uint8_t *)str, 0, LCD_COLOR_CYAN);

            duration = info->FileSize / info->ByteRate; 
            sprintf((char *)str , "File Size : %lu MB [%02lu:%02lu]", (unsigned long)info->FileSize/1024/1024, \
                                                    (unsigned long)duration/60,(unsigned long) duration%60 );
            lcd_log_print((uint8_t *)str, 0, LCD_COLOR_CYAN);
            lcd_text_color_set(LCD_COLOR_BLACK); 

            return AUDIO_ERROR_NONE;
        }
    }
    return AUDIO_ERROR_IO;
}

/*!
    \brief      calculates the remaining file size and new position of the pointer
	\param[in]  none
    \param[out] none
    \retval     none
*/
void bsp_audio_out_transfercomplete_callback(void)
{
    if (audio_state == AUDIO_STATE_PLAY) {
        buffer_ctl.offset = BUFFER_OFFSET_FULL;
    }
}

/*!
    \brief      manages the DMA half transfer complete interrupt
	\param[in]  none
    \param[out] none
    \retval     none
*/
void bsp_audio_out_halftransfer_callback(void)
{ 
    if (audio_state == AUDIO_STATE_PLAY) {
        buffer_ctl.offset = BUFFER_OFFSET_HALF;
    }
}
