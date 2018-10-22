/**
  ******************************************************************************
  * @file    I2C/SlaveTransmitter/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the SlaveTransmitter demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/ GD32190R-EVAL board, it shows the I2C programming 
  mode in slave transmitting mode.In this demo,BOARD_I2C is operated as slave 
  transmitter,and the SCL line and SDA line of BOARD_I2C interface are controled by 
  the I/O pin PB6 and PB7 respectively.
    This demo shows the sending data process of the slave. And it will send the 
  data in the BOARD_I2C_Buf_Write array through the BOARD_I2C.

  @note
    The demo doesn't perform the data transfer actually, which is due to no specific 
  master.In the specific application, we need config the master to start a data 
  transmission.In addition,the master and the slave may need to be connected by 
  the jumper if necessary.

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/