/**
  ******************************************************************************
  * @file    I2C/MasterTransmitter/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the MasterTransmitter demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/ GD32190R-EVAL board, it shows the I2C programming 
  mode in master transmitting mode.In this demo,BOARD_I2C is operated as the master 
  transmitter, and the SCL line and SDA line of BOARD_I2C interface are controled by 
  the I/O pin PB6 and PB7(or PB10 and PB11)respectively.
    This demo shows the sending process of the master.And it will send the data 
  in the BOARD_I2C_Buf_Write array through BOARD_I2C.

  @note
    The demo doesn't perform the data transfer actually, which is due to no specific 
  slave.In the specific application, we must send the correct slave address,and the 
  master and the slave may need to be connected by the jumper if necessary.

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/