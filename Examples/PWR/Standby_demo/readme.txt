/**
  ******************************************************************************
  * @file    PWR/Standby_demo/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the Standby demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/ GD32190R-EVAL board, it explains how
  to enter the standby mode and wake up by the RTC alarm.
    After start-up, the SysTick is configured to generate the systick interrupt 
  every 250ms. The LED3 is toggled when the systick interrupt occurs. 
    When the Tamper Key button is pushed (PC13 is low), the RTC module is configured 
  to generate an alarm event in 3 seconds and the system enters the standby mode 
  immediately. Then LED3 stops toggling.
    The system can be wake up by a system reset or the alarm event. LED3 is toggled 
  again after waking up and the phenomenon can be repeated.

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/