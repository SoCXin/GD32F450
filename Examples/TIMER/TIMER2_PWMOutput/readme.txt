/**
  ******************************************************************************
  * @file    TIMER/TIMER2_PWMOutput/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the TIMER demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/GD32190R-EVAL board, it shows how to
  configure the TIMER peripheral in PWM (Pulse Width Modulation) mode.
  
    The TIMxCLK frequency is set to SystemCoreClock (72MHz), the Prescaler is 72
  so the TIMER2 counter clock is 1MHz.
  
    TIMER2 Channel2 duty cycle = (4000/ 16000)* 100 = 25%.
    TIMER2 Channel3 duty cycle = (8000/ 16000)* 100 = 50%.
    TIMER2 Channel4 duty cycle = (12000/ 16000)* 100 = 75%.

  @note
  Connect the TIMER2 pins to an oscilloscope to monitor the different waveforms:
    - TIMER2_CH2  pin (PB3)
    - TIMER2_CH3  pin (PB10)
    - TIMER2_CH4  pin (PB11)

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/
