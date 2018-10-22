/**
  ******************************************************************************
  * @file    I2C/SlaveReceiver/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    6-Sep-2014 
  * @brief   Description of the SlaveReceiver demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/ GD32190R-EVAL board, it shows the I2C programming 
  mode in slave receiving mode. In this demo, BOARD_I2C is operated as slave receiver, 
  and the SCL line and SDA line of BOARD_I2C interface are controled by the I/O pin 
  PB6 and PB7(or PB10 and PB11 )respectively.
    This demo shows the receving process of the slave.And it will store the 
  received data in the BOARD_I2C_Buf_Read array.

  @note
    The demo doesn't perform the data transfer actually, which is due to no specific 
  master.In the specific application, we need config the master to start a data 
  transmission.In addition,the master and the slave may need to be connected by 
  the jumper if necessary.

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/