/**
  ******************************************************************************
  * @file    CAN/DualCAN/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the Dual CAN demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32190R-EVAL board, it shows how to configure the 
  CAN1 and CAN2 peripherals to send and receive CAN frames in normal mode. pressing 
  Wakeup button or Tamper button:
  - When Tamper button is pressed, CAN1 sends a message to CAN2 and print it.
    when CAN2 receives correctly this message,the receive data will be printed. 
  - When Wakeup button is pressed, CAN2 sends a message to CAN1 and print it.
    when CAN1 receives correctly this message,the receive data will be printed.    
 
  @note
    User can select one from the preconfigured CAN baud rates from the private 
  defines in main.c.These rates is correct only when the system clock frequency 
  is 72M. 
    Connect JP8 to CAN1
    Connect JP4 CAN_L to JP3 CAN_L
    Connect JP4 CAN_H to JP3 CAN_H

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/
