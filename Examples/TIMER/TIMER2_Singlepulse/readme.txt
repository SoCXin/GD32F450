/**
  ******************************************************************************
  * @file    TIMER/TIMER2_Singlepulse/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the TIMER demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32F150R-EVAL/GD32190R-EVAL board, it shows how 
  to configure the TIMER2 peripheral in Single Pulse mode.
    The external signal is connected to TIMER2 CH1 pin (PA0) and the Falling 
  edge is used as active edge.
    The Single Pulse signal is output on TIMER2 CH2 pin (PA1).
    The TIMER2CLK frequency is set to SystemCoreClock (72MHz),the Prescaler is 
  4,so the TIMER2 counter clock is 18MHz.
  
    Single Pulse value = (TIMER2_Period - TIMER2_Pulse) / TIMER2 counter clock
                       = (65535 - 11535) / 18MHz = 3.0 ms.

  @note
    Connect the TIMER2 CH1 pin (PA0)to KEY_TAMPER(PC13),When KEY_TAMPER is pressed,
  a single pulse is generated.

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/
