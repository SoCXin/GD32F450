/**
  ******************************************************************************
  * @file    RTC/Calendar/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of RTC/Calendar demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/GD32190R-EVAL board, it explains how 
  to configure the RTC module. In this demo, RTC peripheral is configured to keep
  time and generate an alarm interrupt.
    The RTC clock source can be chose to LSE or LSI by uncomment the corresponding 
  define in main file. And the HyperTerminal should be connected to the evaluation 
  board via USART1/USART2. The vbat bin on the board should connect to the CR1220 battery.
    After start-up, the program check if the BKP data register is written to a key 
  value. If the value is not correct, the program will ask to set the time and alarm
  value on the HyperTerminal. If the key value is correct, the time and alarm value
  will be displayed on the HyperTerminal. 
    Then LED2 and LED3 are turned on.
    The RTC module is in the Backup Domain, and is not reset by the system reset or 
  power reset. 
    When the alarm interrupt generated, the LED2 toggled (is turned off).

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/