/**
  ******************************************************************************
  * @file    CMP/AnalogWatchdog/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the comparator AnalogWatchdog demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/GD32190R-EVAL board, it shows how to make 
    an analog watchdog by using GD32F1X0 Comparator 
    peripherals in window mode:
    - The upper threshold is set to VREFINT = 1.22V
    - The lower threshold is set to VREFINT / 2 = 1.22V = 0.610V
    - The input voltage is configured to be connected to PC1(GD32150R) or PC0(GD32190R) 

    If the input voltage is above the higher threshold, LED1 and LED3 are turned On.
    If the input voltage is under the lower threshold, LED2 and LED4 are turned On.
    If the input voltage is within the thresholds, the MCU remains in Deepsleep mode
    and all leds are turned Off.
  @note
    Connect PA1 to PC1(GD32150R) or PC0(GD32190R).   

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/