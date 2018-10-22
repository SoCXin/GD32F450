/**
  ******************************************************************************
  * @file    WWDG/WWDG_demo/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the WWDG demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/GD32190R-EVAL board, it shows the WWDG
  with different counter value and window value make the different WWDG timeout. As
  a result, LED1 display a different phenomenon. 
    In this demo, the WWDG counter value is set to 127, the refresh window is set to 
  80. In this case the WWDG timeout is set to 29.2ms(the timeout may varies due to 
  LSI frequency dispersion), the value in the delay function is set to 25ms, the WWDG 
  counter can reloaded normal,the LED1 is turn off, LED2 is blinking fast.
    If the value in the delay function is not set properly, the WWDG counter is not 
  reloaded. As a result, the system reset and the LED1 and LED2 are remain illuminated.
    In this example the system clock is set to 72 MHz.

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/