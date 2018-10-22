/**
  ******************************************************************************
  * @file    DMA/FLASH-RAM/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the DMA FLASH to RAM example.
  ******************************************************************************
  */

  @brief
    This example is based on the GD32150R-EVAL/ GD32190R-EVAL board, it provides a description 
  of how to use  DMA channel1 to transfer data buffer from FLASH memory 
  to embedded SRAM memory.
    
    Before programming the flash addresses, an erase operation is performed firstly.
  After the erase operation, a comparison between FLASH memory and 0xFFFFFFFF(Reset value) 
  is done to check that the FLASH memory has been correctly erased.

    Once the erase operation is finished correctly, the programming operation will be
  performed by using the FMC_ProgramWord function. The written data is transfered to 
  embedded SRAM memory by DMA1 Channel1. The transfer is started by enabling the DMA1 Channel1.
  At the end of the transfer, a Transfer Complete interrupt is generated since it
  is enabled. A comparison between the FLASH memory and embedded SRAM memory is done to
  check that all data have been correctly transferred.If the result of comparison is passed,LED2
  and LED4 light up. Otherwise LED1 and LED3 light up.



/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/