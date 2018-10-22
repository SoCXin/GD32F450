/**
  ******************************************************************************
  * @file    PWR/LVD_demo/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the LVD demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/ GD32190R-EVAL board, it shows how to
  configure the low voltage detector using an external interrupt line. In this demo,
  EXTI line16 is configured to generate an interrupt on either rising or falling edge of the 
  LVD output signal (which indicates that the VDD voltage is below the LVD threshold).
    After system start-up, LED1 is on, however, LED1 is toggled every time(LED1 is
  switched off) when the voltage drops below the 2.9V(GD32F130_150) or 4.5V(GD32F170_190)
  threshold in the interrupt routine.   
 
  @note
    Use a variable DC power supply connected to the 3V3(GD32F130_150) or 5V(GD32F170_190)
  input to the evaluation board and dropping the voltage to 2.9V(GD32F130_150) or 4.5V(GD32F170_190). 

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/