/**
  ******************************************************************************
  * @file    CAN/LoopBack/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the CAN loopback demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32190R-EVAL board, it shows how to set a communication 
  with the CAN in loopback mode.
    The example first performs a transmission and a reception of a standard data
  frame by polling at 125 Kbps. The received frame is checked and print informations 
  indicate whether the communication was successful. Then, an extended data frame is 
  transmitted at 500 Kbps. Reception is done in the interrupt handler when the message 
  becomes pending in the FIFO and print informations indicate whether the communication 
  was successful.
    User can select CAN1 or CAN2 cell using the private defines in main.c
  in loopback mode.

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/
