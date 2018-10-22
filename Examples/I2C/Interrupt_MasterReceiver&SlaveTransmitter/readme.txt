/**
  ******************************************************************************
  * @file    I2C/Interrupt_MasterReceiver&SlaveTransmitter/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the Interrupt_MasterReceiver&SlaveTransmitter demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/GD32190R-EVAL board, it shows how to use the I2C 
  interrupt programming mode in master receiving mode or slave transmitting mode.
  In this demo, I2C1 is operated as the master receiver and I2C2 as the slave 
  transmitter.Moreover,the SCL line and SDA line of I2C1 interface are controled 
  by the I/O pin PB6 and PB7 respectively. The SCL and SDA of I2C2 are controled 
  by the pin PB10 and PB11 respectively.
    This demo will send the data of I2C_Buf_Write array through I2C2 interface 
  to the I2C1,and it will store the data received by I2C1 in the I2C_Buf_Read 
  array.

  @note
    We shoud use the jumper to connect the PB6 and PB10. The PB7 and PB11 are 
  connected as well.

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/
