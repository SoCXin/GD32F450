/**
  ******************************************************************************
  * @file    USART/HyperTerminal_Interrupt/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the HyperTerminal_Interrupt demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/ GD32190R-EVAL board, it shows how to use the USART1 
  Transmit and Receive interrupts to communicate with the hyperterminal.
    Firstly,the USART1 sends the strings to the hyperterminal and still waiting for
  receiving data from the hyperterminal.Only the number of data you enter equal with
  or more than 32 bytes,USART1 will send strings: "USART receive successfully!"(the 
  reception is stopped when this maximum receive value is reached).
  The USART1 is configured as follow:
    - BaudRate = 112500 baud  
    - Word Length = 8 Bits 
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled
     
 
     
/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/