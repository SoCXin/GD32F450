/**
  ******************************************************************************
  * @file    TIMER/TIMER1_Deadtime_Break/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the TIMER demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/GD32190R-EVAL board, it shows how to
  configure the TIMER1 peripheral to generate complementary signals, to insert a 
  defined dead time value, to use the break feature and to lock the desired 
  parameters.

    TIMER1CLK is fixed to SystemCoreClock, the TIMER1 Prescaler is equal to 2 ,so
  the TIMER1 counter clock used is 36MHz.

    The Duty cycle is computed as the following description:
    The channel 1 duty cycle is set to 25% so channel 1N is set to 75%.

    Insert a dead time equal to (63*16*4)/SystemCoreClock = 56us

    Configure the break feature, active at Low level(with GND), and using the 
  automatic output enable feature.

    Use the Locking parameters level1.

  @note

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/
