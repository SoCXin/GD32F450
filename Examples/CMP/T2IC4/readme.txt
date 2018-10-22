/**
  ******************************************************************************
  * @file    CMP/T2IC4/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the comparator output timer input capture demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/GD32190R-EVAL board, it shows how to 
  configure the comparator trigger timer 2 channel 4 input capture event, which 
  is configured to generate an interrupt on both rising and falling edge of the
  output signal.In this demo, input 3.3V(GD32150R) or 5V(GD32190R) to CMP1_IP.
  After system start-up, enable comparator and trigger interrupt,then LED3 is on.  

 @note
    Connect PA1 to +3V3(GD32150R) or +5V(GD32190R).   

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/