/**
  ******************************************************************************
  * @file    ADC/ADC_DMA_TwoResolution/readme.txt
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Description of the ADC/ADC_DMA_TwoResolution demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32190R-EVAL board, it shows how to use the ADC
  to convert analog signal to digital data through DMA.The ADC is configured
  in continuous mode,can configure 6B and 12B resolution. PC0(channel10) is
  chosen as analog input pin. The ADC clock is configured to 12MHz(below 28MHz).
    As the AD conversion begins by software,the converted data from ADC_RDTR register
  to SRAM begins continuously.Users can printf the data by USART.In this demo,
  ADC is configured to 12-bit Resolution at first, and we can get the result from 
  hyperTerminal,then ADC is configured to 6-bit Resolution,we can also get the result 
  from HyperTerminal,so we can directly see the difference between high Resolution and
  low Resolution.The process is repeated.

  @note
    The analog input pin should configured to AN mode and the ADC clock should 
  below 28MHz for GD32190R-EVAL.

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/