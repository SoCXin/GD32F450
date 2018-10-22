/**
  ******************************************************************************
  * @file    DMA/RAM-RAM/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the DMA RAM to RAM example.
  ******************************************************************************
  */

  @brief
    This example is based on the GD32150R-EVAL/ GD32190R-EVAL board, it provides a description 
  of how to use  DMA channel(1 to 7) to transfer data buffer from RAM 
  to RAM memory.
    
    DMA1 Channel(1 to 7) is configured to transfer the contents of data buffer
  stored in "SRC_Const_Buffer" to the reception buffer declared in RAM.

    The start of transfer is triggered by software. At the end of the transfer, 
  a Transfer Complete interrupt is generated since it is enabled. Once interrupt 
  is generated, MCU uses printf function to send specified message to the Hyperterminal using COM1. 
  The Transfer Complete Interrupt pending bit is then cleared. A comparison between the
  source and destination buffers is done to check that all data have been correctly transferred.
  MCU uses printf function to send the result of comparison to the Hyperterminal using COM1.  



/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/