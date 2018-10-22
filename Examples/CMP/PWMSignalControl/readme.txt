/**
  ******************************************************************************
  * @file    CMP/PWMSignalControl/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of comparator PWMSignalControl demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/GD32190R-EVAL board, it shows how to 
    control PWM output by using Comparator output

    - CMP1 is configured as following:
    - Inverting input is internally connected to VREFINT = 1.22V
    - Non Inverting input is connected to PC1(GD32150R)/PC0(GD32190R)
    - Output is internally connected to TIM2 OCREFCLR (output compare reference clear)

    - While PC1/PC0 is lower than VREFINT (1.22V), PWM signal is displayed on PB3 
    - While PC1/PC0 is higher than VREFINT, PB3 is displayed in low level.

  @note
    Connect PA1 to PC1(GD32150R)/PC0(GD32190R).

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/