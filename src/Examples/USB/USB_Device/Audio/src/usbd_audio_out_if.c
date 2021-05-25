/*!
    \file    usbd_audio_out_if.c
    \brief   this file provides the Audio Out (palyback) interface API

    \version 2016-08-15, V1.0.0, firmware for GD32F4xx
    \version 2018-12-12, V2.0.0, firmware for GD32F4xx
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

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


#include "audio_core.h"
#include "usbd_audio_out_if.h"


static uint8_t  init         (uint32_t audiofreq, uint32_t volume, uint32_t options);
static uint8_t  deinit       (uint32_t options);
static uint8_t  audiocmd     (uint8_t* pbuf, uint32_t size, uint8_t cmd);
static uint8_t  volumectl    (uint8_t vol);
static uint8_t  mutectl      (uint8_t cmd);
static uint8_t  periodictc   (uint8_t cmd);
static uint8_t  getstate     (void);


audio_fops_typedef  audio_out_fops = 
{
    init,
    deinit,
    audiocmd,
    volumectl,
    mutectl,
    periodictc,
    getstate
};

static uint8_t audiostate = AUDIO_STATE_INACTIVE;

/*!
    \brief      initialize and configures all required resources for audio play function
    \param[in]  audiofreq: startup audio frequency
    \param[in]  volume: Startup volume to be set
    \param[in]  options: specific options passed to low layer function
    \param[out] none
    \retval     AUDIO_OK if all operations succeed, AUDIO_FAIL else
*/
static uint8_t  init (uint32_t audiofreq, uint32_t volume, uint32_t options)
{
    static uint32_t Initialized = 0;

    /* check if the low layer has already been initialized */
    if (Initialized == 0) {
        /* call low layer function */
        if (eval_audio_init(OUTPUT_DEVICE_AUTO, volume, audiofreq) != 0) {
            audiostate = AUDIO_STATE_ERROR;
            return AUDIO_FAIL;
        }

        /* set the initialization flag to prevent reinitializing the interface again */
        Initialized = 1;
    }

    /* Update the Audio state machine */
    audiostate = AUDIO_STATE_ACTIVE;

    return AUDIO_OK;
}

/*!
    \brief      free all resources used by low layer and stops audio-play function
    \param[in]  Options: options passed to low layer function
    \param[out] none
    \retval     AUDIO_OK if all operations succeed, AUDIO_FAIL else
*/
static uint8_t  deinit (uint32_t options)
{
    /* update the audio state machine */
    audiostate = AUDIO_STATE_INACTIVE;

    return AUDIO_OK;
}

/*!
    \brief      play, stop, pause or resume current file
    \param[in]  pbuf: address from which file should be played
    \param[in]  size: size of the current buffer/file
    \param[in]  cmd: command to be executed, can be:
                    AUDIO_CMD_PLAY
                    AUDIO_CMD_PAUSE
                    AUDIO_CMD_RESUME
                    AUDIO_CMD_STOP
    \param[out] none
    \retval     AUDIO_OK if all operations succeed, AUDIO_FAIL else
*/
static uint8_t  audiocmd (uint8_t* pbuf, uint32_t size, uint8_t cmd)
{
    /* check the current state */
    if ((audiostate == AUDIO_STATE_INACTIVE) || (audiostate == AUDIO_STATE_ERROR)) {
        audiostate = AUDIO_STATE_ERROR;

        return AUDIO_FAIL;
    }

    switch (cmd) {
        /* process the PLAY command */
        case AUDIO_CMD_PLAY:
            /* if current state is active or stopped */
            if ((audiostate == AUDIO_STATE_ACTIVE) || \
                (audiostate == AUDIO_STATE_STOPPED) || \
                (audiostate == AUDIO_STATE_PLAYING)) {
                audio_mal_play((uint32_t)pbuf, (size/2));
                audiostate = AUDIO_STATE_PLAYING;

                return AUDIO_OK;

            /* if current state is paused */
            } else if (audiostate == AUDIO_STATE_PAUSED) {
                if (eval_audio_pause_resume(AUDIO_RESUME, (uint32_t)pbuf, (size/2)) != 0) {
                    audiostate = AUDIO_STATE_ERROR;

                    return AUDIO_FAIL;
                } else {
                    audiostate = AUDIO_STATE_PLAYING;

                    return AUDIO_OK;
                }

            /* not allowed command */
            } else {
                return AUDIO_FAIL;
            }

        /* process the STOP command ----------------------------*/
        case AUDIO_CMD_STOP:
            if (audiostate != AUDIO_STATE_PLAYING) {
                /* unsupported command */

                return AUDIO_FAIL;
            } else if (eval_audio_stop(CODEC_PDWN_SW) != 0) {
                audiostate = AUDIO_STATE_ERROR;

                return AUDIO_FAIL;
            } else {
                audiostate = AUDIO_STATE_STOPPED;

                return AUDIO_OK;
            }

        /* process the PAUSE command */
        case AUDIO_CMD_PAUSE:
            if (audiostate != AUDIO_STATE_PLAYING) {
                /* unsupported command */
                return AUDIO_FAIL;
            } else if (eval_audio_pause_resume(AUDIO_PAUSE, (uint32_t)pbuf, (size/2)) != 0) {
                audiostate = AUDIO_STATE_ERROR;

                return AUDIO_FAIL;
            } else {
                audiostate = AUDIO_STATE_PAUSED;

                return AUDIO_OK;
            }

        /* unsupported command */
        default:
            return AUDIO_FAIL;
    }
}

/*!
    \brief      set the volume level in %
    \param[in]  vol: volume level to be set in % (from 0% to 100%)
    \param[out] none
    \retval     AUDIO_OK if all operations succeed, AUDIO_FAIL else
*/
static uint8_t  volumectl (uint8_t vol)
{
    return AUDIO_OK;
}

/*!
    \brief      mute or unmute the audio current output
    \param[in]  cmd: can be 0 to unmute, or 1 to mute
    \param[out] none
    \retval     AUDIO_OK if all operations succeed, AUDIO_FAIL else
*/
static uint8_t  mutectl (uint8_t cmd)
{
    return AUDIO_OK;
}

/*!
    \brief      none
    \param[in]  cmd: AUDIO_PAUSE (or 0) to pause, AUDIO_RESUME (or any value different from 0) to resume
    \param[out] none
    \retval     AUDIO_OK if all operations succeed, AUDIO_FAIL else
*/
static uint8_t  periodictc (uint8_t cmd)
{
    return AUDIO_OK;
}

/*!
    \brief      return the current state of the audio machine
    \param[in]  none
    \param[out] none
    \retval     current state
*/
static uint8_t  getstate (void)
{
    return audiostate;
}
