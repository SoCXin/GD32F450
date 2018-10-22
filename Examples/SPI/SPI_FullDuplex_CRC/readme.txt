/**
  ******************************************************************************
  * @file    SPI/SPI_FullDuplex_CRC/readme.txt
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Description of the SPI_FullDuplex_CRC example.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/ GD32190R-EVAL board,The function of 
  this demo is exchange data between SPI1 and SPI2.SPI1 is set as master and SPI2
  is slave.They are work in fullduplex mode and followed by CRC transmission.
    After the SPI1_Buffer_Tx and SPI2_Buffer_Tx are sent and SPI2_Buffer_Rx and 
  SPI1_Buffer_Rx are received, check whether the SPI2_Buffer_Rx, SPI1_Buffer_Tx 
  and SPI1_Buffer_Rx, SPI2_Buffer_Tx are the same. If the data received is equal
  to data sent, TransferStatus1 and TransferStatus2 are equal to PASSED. Then SPI1
  and SPI2 CRC Error flags are checked, TransferStatus1 and TransferStatus2 are 
  assigned to FAILED if one of the SPI_FLAG_CRCERR flags are equal to SET.
  @note
    Connect SPI1 SCK  PIN(PA5) TO SPI2 SCK  PIN(PB13)
    Connect SPI1 MISO PIN(PA6) TO SPI2 MISO PIN(PB14)
    Connect SPI1 MOSI PIN(PA7) TO SPI2 MOSI PIN(PB15)

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/