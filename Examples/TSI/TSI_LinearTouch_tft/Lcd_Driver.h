/**
  ******************************************************************************
  * @file    TSI/TSI_LinearTouch_tft/Lcd_Driver.h
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   LCD driver header file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_DRIVER_H
#define __LCD_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "gd32f1x0.h"

/* Exported define -----------------------------------------------------------*/
#define RED     0xf800
#define GREEN   0x07e0
#define BLUE    0x001f
#define WHITE   0xffff
#define BLACK   0x0000
#define YELLOW  0xFFE0

/* GRAYs */
#define GRAY0   0xEF7D
#define GRAY1   0x8410
#define GRAY2   0x4208

/* PF5 TFT CS */
#define LCD_CS_SET    GPIOF->BOR = GPIO_PIN_5
#define LCD_CS_CLR    GPIOF->BCR = GPIO_PIN_5

/* PC3 TFT RS/DC */
#define LCD_RS_SET    GPIOC->BOR = GPIO_PIN_3
#define LCD_RS_CLR    GPIOC->BCR = GPIO_PIN_3

/* PC4 TFT RST */
#define LCD_RST_SET   GPIOC->BOR = GPIO_PIN_4
#define LCD_RST_CLR   GPIOC->BCR = GPIO_PIN_4    

/* Exported functions --------------------------------------------------------*/
uint8_t SPI_WriteByte(SPI_TypeDef* SPIx,uint8_t Byte);
void Lcd_WriteIndex(uint8_t Index);
void Lcd_WriteData(uint8_t Data);
void Lcd_Init(void);
void Lcd_Clear(uint16_t Color);
void Lcd_SetXY(uint16_t x,uint16_t y);
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data);

#endif /* __LCD_DRIVER_H */

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
