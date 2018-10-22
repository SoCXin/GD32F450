/**
  ******************************************************************************
  * @file    SPI/SPI_Simplex_Interrupt/readme.txt
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Description of the SPI_Simplex_Interrupt example.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/ GD32190R-EVAL board,The function of
  this demo is transfer data from SPI1 to SPI2.SPI1 is set as master and SPI2 is 
  slave.They are work in simplex mode using Tx interrupt for master and Rx interrupt 
  for slave.
    If SPI_SLAVE_Buffer_Rx and SPI_MASTER_Buffer_Tx are the same, TransferStatus is 
  equal to PASSED. 
  @note
    Connect SPI1 SCK  PIN(PA5) TO SPI2 SCK  PIN(PB13)
    Connect SPI1 MOSI PIN(PA7) TO SPI2 MISO PIN(PB14)

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/
