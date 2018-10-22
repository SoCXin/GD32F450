/**
  ******************************************************************************
  * @file    ADC/AnalogWatchdog/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the ADC/AnalogWatchdog demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/ GD32190R-EVAL board, it shows how to
  use the ADC analog watchdog to guard continuously an ADC channel.The ADC is configured 
  in continuous mode, PC1(GD32150R-EVAL) or PC0(GD32190R-EVAL) is chosen as analog
  input pin.The ADC clock is configured to 12MHz(below 14MHz or 28MHz).
    When the channel11(channel10) converted value is over the programmed analog 
  watchdog high threshold (value 0x0A00) or below the analog watchdog low threshold
  (value 0x0400),an AWE interrupt will occur,and LED2 will turn on.    
 
  @note
    The analog input pin should configured to AN mode and the ADC clock should 
  below 14MHz for GD32150R(28MHz for GD32190R-EVAL).

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/