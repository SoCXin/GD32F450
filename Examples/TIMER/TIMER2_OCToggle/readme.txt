/**
  ******************************************************************************
  * @file    TIMER/TIMER2_OCToggle/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the TIMER demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/GD32190R-EVAL board, it shows how to
  configure the TIMER peripheral to generate the toggle signal.
  
    The TIMxCLK frequency is set to SystemCoreClock (72MHz), the Prescaler is 36000
  so the TIMER2 counter clock is 2KHz.
  
    TIMER2 Configuration: Output Compare Toggle Mode:
    CH2 update rate = TIMER2 counter clock / CHCC2 = 2000/4000 = 0.5 Hz 

  @note

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/
