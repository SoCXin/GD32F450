/**
  ******************************************************************************
  * @file    TIMER/TIMER1_DMABurst/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the TIMER demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/GD32190R-EVAL board, it shows how to
  use DMA with TIMER1 Update request to transfer Data from memory to TIMER1 capture
  compare register 1~4.
 
    TIMER1CLK is fixed to SystemCoreClock, the TIMER1 Prescaler is equal to 72
  so the TIMER1 counter clock used is 1MHz.

    The objective is to configure TIMER1 channel 1~4(PA8~PA11) to generate PWM signal.
  capture compare register 1~4 are to be updated twice per circle.On the first update 
  DMA request, data1 is transferred to CHCC1, data2 is transferred to CHCC2, data3 is 
  transferred to CHCC3,data4 is transferred to CHCC4 and duty cycle(10%,20%,30%,40%). 
  On the second update DMA request, data5 is transferred to CHCC1, data6 is transferred 
  to CHCC2, data7 is transferred to CHCC3,data8 is transferred to CHCC4 and duty cycle
  (50%,60%,70%,80%).

  @note

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/
