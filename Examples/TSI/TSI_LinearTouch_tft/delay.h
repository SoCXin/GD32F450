/**
  ******************************************************************************
  * @file    TSI/TSI_LinearTouch_tft/delay.h
  * @author  MCU SD
 *  @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Delay header file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DELAY_H
#define __DELAY_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"

/* Exported functions --------------------------------------------------------*/
void delay_init(uint8_t SYSCLK);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);

#endif /* __DELAY_H */

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/





























