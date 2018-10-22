/**
  ******************************************************************************
  * @file    ADC/ADC_DMA/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the ADC/ADC_DMA demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/GD32190R-EVAL board, it shows how to
  use the ADC to convert analog signal to digital data through DMA.The ADC is 
  configured in continuous mode, PC1(GD32150R-EVAL) or PC0(GD32190R-EVAL) is chosen
  as analog input pin. The ADC clock is configured to 12MHz(below 14MHz or 28MHz).
    As the AD convertion begins by software,the converted data from ADC_RDTR register
  to SRAM begins continuously.Users can change the VR1 on the GD32150R-EVAL/GD32190R-EVAL
  board,measure TP1(GD32150R-EVAL) or TP2(GD32190R-EVAL) pad board,and check if 
  its value matches the converted data through the watch window.     

  @note
    The analog input pin should configured to AN mode and the ADC clock should 
  below 14MHz for GD32150R(28MHz for GD32190R-EVAL). 

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/
