/*!
    \file  gd32f450i_eval_audio.c
    \brief this file provides the Audio driver for the gd32f450i-EVAL board.

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

#include "gd32f450i_eval_audio.h"
#include "gd32f4xx_dma.h"
#include "audio.h"

/* Mask for the bit EN of the I2S CFGR register */
#define I2S_ENABLE_MASK                 (0x0400)

static dma_single_data_parameter_struct dma_initstructure;
static void codec_audiointerface_deinit(void);

/*!
    \brief      configure the audio peripherals
    \param[in]  outputdevice: 
       OUTPUT_DEVICE_SPEAKER
       OUTPUT_DEVICE_HEADPHONE
       OUTPUT_DEVICE_BOTH
    \param[in]  volume: initial volume level (from 0 (mute) to 100 (max))
    \param[in]  audiofreq: audio frequency used to play the audio stream
    \param[out] none
    \retval     AUDIO_OK if correct communication, else wrong communication
*/
uint8_t bsp_audio_out_init(uint16_t outputdevice, uint8_t volume, uint32_t audiofreq)
{
    uint8_t ret = AUDIO_ERROR;
    
    /* enable the GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOI);
    /* enable I2S1 clock */
    rcu_periph_clock_enable(RCU_SPI1);

    /* I2S1_MCK(PA6), I2S1_CK(PI1), I2S1_WS(PI0), I2S1_SD(PC1) GPIO pin configuration */
    gpio_af_set(GPIOA, GPIO_AF_6, GPIO_PIN_6);
    gpio_af_set(GPIOC, GPIO_AF_7, GPIO_PIN_1);
    gpio_af_set(GPIOI, GPIO_AF_5, GPIO_PIN_0| GPIO_PIN_1);
    
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);

    gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0| GPIO_PIN_1);
    gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0| GPIO_PIN_1);

    /* enable I2S1 clock */
    rcu_periph_clock_enable(RCU_SPI1);

    /* CODEC_I2S peripheral configuration */
    spi_i2s_deinit(SPI1);

    /* CODEC_I2S peripheral configuration */
    i2s_psc_config(SPI1, audiofreq, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_DISABLE);
    i2s_init(SPI1, I2S_MODE_MASTERTX, I2S_STD_MSB, I2S_CKPL_HIGH);

    /* The I2S peripheral will be enabled only in the EVAL_AUDIO_Play() function 
       or by user functions if DMA mode not enabled */  
    audio_mal_init();
    ret = AUDIO_OK;

    return ret;
}

/*!
    \brief      initializes and prepares the Media to perform audio data transfer from Media to the I2S peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void audio_mal_init(void)  
{
    /* Enable the DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);

    /* Configure the DMA Stream */
    dma_deinit(DMA0, DMA_CH4);

    /* Set the parameters to be configured */
    dma_initstructure.periph_addr = SPI1 + 0x0CU;
    dma_initstructure.memory0_addr = (uint32_t)0;/* This field will be configured in play function */
    dma_initstructure.direction = DMA_MEMORY_TO_PERIPH;
    dma_initstructure.number = (uint32_t)0xFFFE;/* This field will be configured in play function */
    dma_initstructure.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_initstructure.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_initstructure.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
    dma_initstructure.circular_mode = DMA_CIRCULAR_MODE_ENABLE; //DMA_CIRCULAR_MODE_DISABLE;
    dma_initstructure.priority = DMA_PRIORITY_HIGH;

    dma_single_data_mode_init(DMA0, DMA_CH4, &dma_initstructure);
    dma_channel_subperipheral_select(DMA0, DMA_CH4, DMA_SUBPERI0);
}

/*!
    \brief      starts playing audio stream from the audio Media.
    \param[in]  addr: pointer to the audio stream buffer
    \param[in]  size: number of data in the audio stream buffer
    \param[out] none
    \retval     none
*/
void audio_mal_play(uint32_t addr, uint32_t size)
{
    /* Enable the I2S DMA Stream*/
    dma_channel_disable(DMA0, DMA_CH4);

    /* Clear the Interrupt flag */
    dma_interrupt_flag_clear(DMA0, DMA_CH4, DMA_INTF_FTFIF);
    dma_interrupt_flag_clear(DMA0, DMA_CH4, DMA_INTF_HTFIF);

    /* Configure the buffer address and size */
    dma_initstructure.memory0_addr = (uint32_t)addr;
    dma_initstructure.number = (uint32_t)(size);

    /* Configure the DMA Stream with the new parameters */
    dma_single_data_mode_init(DMA0, DMA_CH4, &dma_initstructure);

    /* Enable the I2S DMA Stream*/
    dma_channel_enable(DMA0, DMA_CH4);

    /* If the I2S peripheral is still not enabled, enable it */
    if ((SPI_I2SCTL(SPI1) & I2S_ENABLE_MASK) == 0) {
        i2s_enable(SPI1);
    }
}

/*!
    \brief      send n-bytes on the IIS interface
    \param[in]  pdata: pointer to the data address
    \param[in]  size: number of data to be written
    \param[out] none
    \retval     none
*/
void bsp_audio_out_changebuffer(uint16_t *pdata, uint16_t size)
{
    audio_mal_play((uint32_t)pdata, size);
}

/*!
    \brief      this function Pauses the audio file stream. In case of using DMA, the DMA Pause feature is used
    \param[in]  audiofreq: audio frequency
    \param[out] none
    \retval     AUDIO_OK if correct communication, else wrong communication
*/
uint8_t bsp_audio_out_pause(uint32_t audiofreq)
{    
    /* Stop the current DMA request by resetting the I2S cell */
    codec_audiointerface_deinit();

    /* Re-configure the I2S interface for the next resume operation */
    rcu_periph_clock_enable(RCU_SPI1);

    /* CODEC_I2S peripheral configuration */
    spi_i2s_deinit(SPI1);

    /* CODEC_I2S peripheral configuration */
    i2s_psc_config(SPI1, audiofreq, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_ENABLE);
    i2s_init(SPI1, I2S_MODE_MASTERTX, I2S_STD_MSB, I2S_CKPL_HIGH);

    /* Disable the DMA Stream */
    dma_channel_disable(DMA0, DMA_CH4);

    /* Clear the Interrupt flag */
    dma_flag_clear(DMA0, DMA_CH4, DMA_INTF_HTFIF|DMA_INTF_FTFIF);

    /* Return AUDIO_OK when all operations are correctly done */
    return AUDIO_OK;
}

/*!
    \brief      this function resume the audio file stream
    \param[in]  none
    \param[out] none
    \retval     AUDIO_OK if correct communication, else wrong communication
*/
uint8_t bsp_audio_out_resume(void)
{
    /* Enable the I2S DMA request */
    spi_dma_enable(SPI1, SPI_DMA_TRANSMIT);

    /* Enable the I2S DMA Stream*/
    dma_channel_enable(DMA0, DMA_CH4);

    /* If the I2S peripheral is still not enabled, enable it */
    if ((SPI_I2SCTL(SPI1) & I2S_ENABLE_MASK) == 0) {
        i2s_enable(SPI1);
    }

    return AUDIO_OK;
}

/*!
    \brief      stops audio playing and power down the audio codec
    \param[in]  option: could be one of the following parameters
                - CODEC_PDWN_SW: for software power off (by writing registers). 
                              Then no need to reconfigure the Codec after power on.
                - CODEC_PDWN_HW: completely shut down the codec (physically). 
                              Then need to reconfigure the Codec after power on.  

    \param[out] none
    \retval     AUDIO_OK if correct communication, else wrong communication
*/
uint8_t bsp_audio_out_stop(uint32_t option)
{
    /* Call the Media layer stop function */
    spi_dma_disable(SPI1, SPI_DMA_TRANSMIT);
    
    /* Stop the Transfer on the I2S side: Stop and disable the DMA stream */
    dma_channel_disable(DMA0, DMA_CH4);

    /* Clear all the DMA flags for the next transfer */
    dma_flag_clear(DMA0, DMA_CH4, DMA_INTF_HTFIF|DMA_INTF_FTFIF);

    /* Stop the current DMA request by resetting the I2S cell */
    codec_audiointerface_deinit();

    /* Re-configure the I2S interface for the next paly operation */
    /* enable I2S1 clock */
    rcu_periph_clock_enable(RCU_SPI1);

    /* CODEC_I2S peripheral configuration */
    spi_i2s_deinit(SPI1);

    /* CODEC_I2S peripheral configuration */
    i2s_psc_config(SPI1, I2S_AUDIOSAMPLE_44K, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_ENABLE);
    i2s_init(SPI1, I2S_MODE_MASTERTX, I2S_STD_MSB, I2S_CKPL_HIGH);

    return AUDIO_OK;
}

/*!
    \brief      restores the audio codec audio interface to its default state
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void codec_audiointerface_deinit(void)
{
    /* Disable the CODEC_I2S peripheral (in case it hasn't already been disabled) */
    i2s_disable(SPI1);

    /* Deinitialize the CODEC_I2S peripheral */
    spi_i2s_deinit(SPI1);

    /* Disable the CODEC_I2S peripheral clock */
    rcu_periph_clock_disable(RCU_SPI1);
}

/*!
    \brief      TX transfer completed callbacks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void hal_sai_txcpltcallback(void)
{
    /* Manage the remaining file size and new address offset: This function 
     should be coded by user (its prototype is already declared in gd32f450i_eval_audio.h) */
    bsp_audio_out_transfercomplete_callback();
}

/*!
    \brief      TX half transfer completed callbacks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void hal_sai_txhalfcpltcallback(void)
{
    /* Manage the remaining file size and new address offset: This function 
     should be coded by user (its prototype is already declared in gd32f450i_eval_audio.h) */
    bsp_audio_out_halftransfer_callback();
}
