/**
  ******************************************************************************
  * @file    Lcd_seg.h 
  * @author  MCU SD
  * @version V1.0.0
  * @date    15-Jan-2016
  * @brief   Headfile of the lcd segment driver.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_SEG_H
#define __LCD_SEG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"

/* Exported types ------------------------------------------------------------*/
typedef enum
{
    INTEGER = 0,
    FLOAT = 1,
    TIME = 2
}Display_Typedef;

/* Exported functions ------------------------------------------------------- */
void LCD_Seg_Init(void);
void LCD_Seg_DisplayDigit(uint8_t ch, uint8_t position);
void LCD_Seg_DisplayNumber(uint16_t num);
void LCD_Seg_DisplayFloatNumber(float num);
void LCD_Seg_DisplayTime(uint8_t hour, uint8_t minute , uint8_t second);
void LCD_Seg_ClearDigit(uint8_t position);
void LCD_Seg_ClearAll(void);

#ifdef __cplusplus
}
#endif

#endif /* __LCD_SEG_H */

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
