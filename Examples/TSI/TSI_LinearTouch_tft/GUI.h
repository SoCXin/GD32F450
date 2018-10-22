/**
  ******************************************************************************
  * @file    TSI/TSI_LinearTouch_tft/GUI.h 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   GUI display header file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GUI_H
#define __GUI_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include <stdlib.h>

/* Exported functions --------------------------------------------------------*/
uint16_t LCD_BGR2RGB(uint16_t c);
void Gui_Circle(uint16_t X,uint16_t Y,uint16_t R,uint16_t fc); 
void Gui_DrawLine(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1,uint16_t Color);  
void Gui_box(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t bc);
void Gui_box2(uint16_t x,uint16_t y,uint16_t w,uint16_t h, uint8_t mode);
void DisplayButtonDown(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void DisplayButtonUp(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void Gui_DrawFont_GBK16(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s);
void Gui_DrawFont_GBK24(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s);
void Gui_DrawFont_Num32(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint16_t num);

#endif /* __GUI_H */

/******************* (C) COPYRIGHT 2015 GIGADEVICE *****END OF FILE****/



