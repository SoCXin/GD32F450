/**
  ******************************************************************************
  * @file    TIMER/TIMER2_OCActive/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the TIMER demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/GD32190R-EVAL board, it shows how to
  configure the TIMER peripheral to generate three different signals with three 
  different delays.
  
    The TIMxCLK frequency is set to SystemCoreClock (72MHz), the Prescaler is 36000
  so the TIMER2 counter clock is 2KHz.

    And generate 3 signals with 3 different delays:
    TIMER2_CH1 delay = 4000/2000  = 2s
    TIMER2_CH2 delay = 8000/2000  = 4s
    TIMER2_CH3 delay = 12000/2000 = 6s 

  @note 
    Connect the TIMER2 pins to leds to watch the signal:
    - TIMER2_CH1  pin (PA0)
    - TIMER2_CH2  pin (PA1)
    - TIMER2_CH3  pin (PA2)

    if use GD32150R-EVAL board:
    - LED2  pin (PC11)
    - LED3  pin (PC12)
    - LED4  pin (PD2)

    if use GD32190R-EVAL board:
    - LED2  pin (PA12)
    - LED3  pin (PB6)
    - LED4  pin (PB7)
/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/
