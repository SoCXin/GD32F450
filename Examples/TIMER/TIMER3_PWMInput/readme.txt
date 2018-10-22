/**
  ******************************************************************************
  * @file    TIMER/TIMER3_PWMInput/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the TIMER demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/GD32190R-EVAL board, it shows how to 
  use the TIMER peripheral to measure the frequency and duty cycle of an external
  signal.
  
    The TIMxCLK frequency is set to SystemCoreClock (72MHz), the Prescaler is 36000
  so the TIMER3 counter clock is 2KHz.
  
    The external signal is connected to TIMER3 CH1 pin (PB4). 
    The Rising edge is used as active edge.
    The TIMER3 CHCC1 is used to compute the frequency value.
    The TIMER3 CHCC2 is used to compute the duty cycle value. 

  @note
    The available frequence of the input signal is ranging from 0.03Hz to 2KHz.
  You can use USART to watch the frequency and duty cycle of the input signal.
  The USART is configured as follow:
    - BaudRate = 115200 baud  
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/
