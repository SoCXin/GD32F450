/**
  ******************************************************************************
  * @file    ADC/TIMTrigger_AutoInsertion/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the ADC/TIMTrigger_AutoInsertion demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/ GD32190R-EVAL board, it shows how to
  convert ADC regular group channels continuously using TIM2 external trigger and
  inserted group channels using the auto-inserted feature.The ADC clock is configured 
  to 12MHz(below 14MHz or 28MHz).
    ADC is configured to convert on each TIM2 capture compare event ADC channel14.
  Each time an end of regular conversion occurs the DMA transfers the converted data 
  from ADC_RDTR register to the ADC_RegularConvertedValueTab table. 
    Enabling the auto-inserted feature, allows to convert automatically the inserted 
  channel ADC channel11 after the end of the regular channel14. An ADC interrupt is 
  generated then by EOIC flag at the end of the inserted channel conversion and in 
  the interrupt routine the result is stored in the ADC_InsertedConvertedValueTab table.
  The procedure is repeated 64 times then the TIM2 peripheral is disabled and thus, 
  no conversion will be done neither regular nor inserted. 
    
  @note
    The analog input pin should configured to AN mode and the ADC clock should 
  below 14MHz for GD32150R(28MHz for GD32190R-EVAL).  

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/