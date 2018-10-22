/**
  ******************************************************************************
  * @file    DMA/RAM-USART/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the DMA RAM-USART example.
  ******************************************************************************
  */

  @brief
     This example is based on the GD32150R-EVAL/ GD32190R-EVAL board, it provides a description 
  of how to use DMA channel2(or channel4 ) to transfer data from RAM memory to USART transmit
  data register.
     The start of transfer is triggered by software. At the end of the transfer, 
  a Transfer Complete interrupt is generated since it is enabled. If the DMA transfer
  operation is finished correctly, data stored in SRC_Const_Buffer will be transfered 
  to a serial port tool by USART and LED2 and LED4 light up. Otherwise LED1 and LED3 
  light up.
      
/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/