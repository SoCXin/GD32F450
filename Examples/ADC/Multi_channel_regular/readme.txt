/**
  ******************************************************************************
  * @file    ADC/Multi_channel_regular/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the ADC/Multi_channel_regular demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the GD32150R-EVAL/GD32190R-EVAL board, it shows how to use the ADC
  to convert 4 channels analog signal to digital data through DMA.The ADC is configured 
  in continuous mode,PC1(channel11)¡¢PC2(channel12)¡¢PC3(channel13)¡¢PC4(channel14) is 
  chosen as analog input pins.The ADC clock is configured to 12MHz(below 14MHz or 28MHz).
    The AD convertion begins by software,and the converted data transfers from ADC_RDTR register
  to ADC_ConvertedValue[] through DMA. If the DMA completes transferring 4 data,an interrupt
  occur,in the interrupt service routine,the converted value of each channel is saved in 
  seperate arrays,when the channel11 converted value is over the programmed high threshold 
  (value 0B66) or below the low threshold (value 08BA),LED2 is lighted on or else LED3 is 
  lighted on.Then a new conversion restarts by software.,until the data arrays are full
  (256 times later).      
 
  @note
    The analog input pin should configured to AN mode and the ADC clock should 
  below 14MHz for GD32150R(28MHz for GD32190R-EVAL). 

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/