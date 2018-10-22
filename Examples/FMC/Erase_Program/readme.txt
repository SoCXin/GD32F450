/**
  ******************************************************************************
  * @file    FMC/Erase_Program/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the FLASH Erase_Program example.
  ******************************************************************************
  */

  @brief
    This example is based on the GD32150R-EVAL/GD32190R-EVAL board, it provides a
  description of how to Erase and Program FLASH memory and program without previous
  earse. 
    
    Before an erase operation, FMC_Unlock function is used firstly to unlock 
  the Flash memory Program/Erase Controller. 

  After the erase operation, A comparison between FLASH memory and 0xFFFFFFFF(Reset value) 
  is done to check that the FLASH memory has been correctly erased.

    Once the erase operation is finished correctly, the programming operation will be
  performed by using the FMC_ProgramWord function. A comparison between the FLASH memory 
  and the source data is done to check that all data have been correctly programed.if all
  operation are correct, LED2 and LED4 are on.

    If program the memory without any erase, the content of memory bit 1 could change 
  to be bit 0,LED1 and LED3 are on.
  
 
/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/