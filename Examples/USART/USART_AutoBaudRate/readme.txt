/**
  ******************************************************************************
  * @file    USART/USART_AutoBaudRate/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    6-Sep-2014 
  * @brief   Description of the USART_AutoBaudRate demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL board, it shows how to use the USART1 
  to detect and automatically set the USARTx_BRR value based on the reception of 
  one character. 
    In this demo,USART_AUTOBAUDRATE_STARTBIT(measure the duration of the Start bit)
  is set as the Auto baud rate detection mode.If Auto  baud rate detection is 
  completed without error,USART1 will send strings:"USART Auto baud rate detection
  finshed successfully!"
     
 
  @note
     You can send any character with LSB set 1(eg.0xff or 0x01)to implement the 
  auto baud rate detection.


/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/