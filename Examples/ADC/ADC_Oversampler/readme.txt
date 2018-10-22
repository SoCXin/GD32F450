/**
  ******************************************************************************
  * @file    ADC/ADC_Oversampler/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the ADC/ADC_Oversampler demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32190R-EVAL board, it shows how to use the oversampling 
  function to improve the precision of ADC conversion.The ADC is configured Oversampler 
  mode,PC0(channel10) is chosen as analog input pin. The ADC clock is configured to
  12MHz(below 28MHz).

    At first,the ADC is configured in normal sampling mode,12-bit precision.Then the ADC
  is configured in oversampling mode,4X ratio and no shift bits,equals 14-bit precision.
  The different converted data show though USART1.Users can compare two kinds data.

  @note
    The analog input pin should configured to AN mode and the ADC clock should 
  below 28MHz for GD32190R-EVAL.  

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/