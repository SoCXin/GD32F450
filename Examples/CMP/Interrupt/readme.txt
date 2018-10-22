/**
  ******************************************************************************
  * @file    CMP/Interrupt/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the comparator interrupt demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/GD32190R-EVAL board, it shows how to 
  configure the comparator trigger interrupt using an external interrupt line. 
  In this demo,input 3.3V(GD32150R) or 5V(GD32190R)to PA1  ,EXTI line 21 is 
  configured to generate an interrupt on rising edge of the output signal.After 
  system start-up,enable comparator and trigger interrupt,then LED2 is on. 
   
  @note
    Connect PA1 to +3V3 or +5V.

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/