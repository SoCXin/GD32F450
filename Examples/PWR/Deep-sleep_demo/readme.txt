/**
  ******************************************************************************
  * @file    PWR/Deep-sleep_demo/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the PWR Deep-sleep demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/ GD32190R-EVAL board, it shows how to 
  enter the system to Deep-sleep mode and wake-up using RTC Alarm and EXTI Line 
  interrupt. The RTC Alarm(EXTI Line17) is configured to generate an 5s Alarm event, 
  The EXTI Line interrupt(EXTI Line13) is configured to generate interrupt on rising edge.
    The system enters and exits Deep-sleep mode as following: 
    After system start-up, the LED1 twinkles twice, and the RTC is configured to 
  generate an Alarm event, then the system enters Deep-sleep mode, the RTC Alarm 
  will wake-up the system after 5 seconds, After exit from Deep-sleep, the system 
  clock is reconfigured to its previous state (as HSE and PLL are disabled in 
  Deep-sleep mode), and the LED2 keeps on after it twinkles twice, the LED1 keeps 
  twinkling. When the Wakeup key is pressed, the system enters the Deep-sleep mode once 
  again, LED1 and LED2 keeps on. When the Tamper key is pressed, the system exits Deep-sleep 
  mode, and return to the previous state(LED2 keeps on, LED1 keeps twinkling).  

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/