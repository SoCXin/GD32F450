/*!
    \file  lcd_log.h
    \brief the header file of LCD log

    \version 2016-08-15, V1.0.0, firmware for GD32F4xx
    \version 2018-12-12, V2.0.0, firmware for GD32F4xx
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef LCD_LOG_H
#define LCD_LOG_H

#include "stdint.h"
#include "lcd_font.h"
#include <stdio.h>

/* definitions of color */
#define LCD_COLOR_GREY          0xF7DE
#define LCD_COLOR_BLUE          0x001F
#define LCD_COLOR_BLUE2         0x051F
#define LCD_COLOR_GREEN         0x07E0
#define LCD_COLOR_RED           0xF800
#define LCD_COLOR_CYAN          0x07FF
#define LCD_COLOR_MAGENTA       0xF81F
#define LCD_COLOR_YELLOW        0xFFE0
#define LCD_COLOR_LIGHTBLUE     0x841F
#define LCD_COLOR_LIGHTGREEN    0x87F0
#define LCD_COLOR_LIGHTRED      0xFC10
#define LCD_COLOR_LIGHTCYAN     0x87FF
#define LCD_COLOR_LIGHTMAGENTA  0xFC1F
#define LCD_COLOR_LIGHTYELLOW   0xFFF0
#define LCD_COLOR_DARKBLUE      0x0010
#define LCD_COLOR_DARKGREEN     0x0400
#define LCD_COLOR_DARKRED       0x8000
#define LCD_COLOR_DARKCYAN      0x0410
#define LCD_COLOR_DARKMAGENTA   0x8010
#define LCD_COLOR_DARKYELLOW    0x8400
#define LCD_COLOR_WHITE         0xFFFF
#define LCD_COLOR_LIGHTGRAY     0xD69A
#define LCD_COLOR_GRAY          0x8410
#define LCD_COLOR_DARKGRAY      0x4208
#define LCD_COLOR_BLACK         0x0000
#define LCD_COLOR_BROWN         0xA145
#define LCD_COLOR_ORANGE        0xFD20

/* definitions of lines depending on the chosen fonts */
#define LCD_LINE_0               LINE(0)
#define LCD_LINE_1               LINE(1)
#define LCD_LINE_2               LINE(2)
#define LCD_LINE_3               LINE(3)
#define LCD_LINE_4               LINE(4)
#define LCD_LINE_5               LINE(5)
#define LCD_LINE_6               LINE(6)
#define LCD_LINE_7               LINE(7)
#define LCD_LINE_8               LINE(8)
#define LCD_LINE_9               LINE(9)
#define LCD_LINE_10              LINE(10)
#define LCD_LINE_11              LINE(11)
#define LCD_LINE_12              LINE(12)
#define LCD_LINE_13              LINE(13)
#define LCD_LINE_14              LINE(14)
#define LCD_LINE_15              LINE(15)
#define LCD_LINE_16              LINE(16)
#define LCD_LINE_17              LINE(17)
#define LCD_LINE_18              LINE(18)
#define LCD_LINE_19              LINE(19)
#define LCD_LINE_20              LINE(20)
#define LCD_LINE_21              LINE(21)
#define LCD_LINE_22              LINE(22)
#define LCD_LINE_23              LINE(23)
#define LCD_LINE_24              LINE(24)
#define LCD_LINE_25              LINE(25)
#define LCD_LINE_26              LINE(26)
#define LCD_LINE_27              LINE(27)
#define LCD_LINE_28              LINE(28)
#define LCD_LINE_29              LINE(29)
#define LCD_LINE_30              LINE(30)
#define LCD_LINE_31              LINE(31)
#define LCD_LINE_32              LINE(32)
#define LCD_LINE_33              LINE(33)
#define LCD_LINE_34              LINE(34)
#define LCD_LINE_35              LINE(35)
#define LCD_LINE_36              LINE(36)
#define LCD_LINE_37              LINE(37)
#define LCD_LINE_38              LINE(38)
#define LCD_LINE_39              LINE(39)
#define LCD_LINE_40              LINE(40)
#define LCD_LINE_41              LINE(41)
#define LCD_LINE_42              LINE(42)
#define LCD_LINE_43              LINE(43)
#define LCD_LINE_44              LINE(44)
#define LCD_LINE_45              LINE(45)
#define LCD_LINE_46              LINE(46)
#define LCD_LINE_47              LINE(47)
#define LCD_LINE_48              LINE(48)
#define LCD_LINE_49              LINE(49)
#define LCD_LINE_50              LINE(50)
#define LCD_LINE_51              LINE(51)
#define LCD_LINE_52              LINE(52)
#define LCD_LINE_53              LINE(53)
#define LCD_LINE_54              LINE(54)
#define LCD_LINE_55              LINE(55)
#define LCD_LINE_56              LINE(56)
#define LCD_LINE_57              LINE(57)
#define LCD_LINE_58              LINE(58)
#define LCD_LINE_59              LINE(59)

#define LCD_LAYER_BACKGROUND     0x0000
#define LCD_LAYER_FOREGROUND     0x0001

#define LCD_PIXEL_WIDTH          ((uint16_t)480)
#define LCD_PIXEL_HEIGHT         ((uint16_t)272)

typedef struct 
{
    int16_t x;
    int16_t y;
}point_struct, *point;



/* initialize the LCD GPIO and TLI */
void lcd_init(void);
/* initialize TLI layer0 and layer1 */
void lcd_layer_init(void);
/* set the LCD layer */
void lcd_layer_set(uint32_t layer);
/* clear the LCD with specified color */
void lcd_clear(uint16_t color);
/* set the transparency of LCD */
void lcd_transparency_set(uint8_t trans);
/* set point with the specified position and color */
void lcd_point_set(uint16_t xpos, uint16_t ypos, uint16_t color);
/* get point with the specified position and color */
uint16_t lcd_point_get(uint16_t xpos, uint16_t ypos);
/* set the text font */
void lcd_font_set(sFONT *font);
/* get the text font */
sFONT* lcd_font_get(void);
/* set the text color */
void lcd_text_color_set(uint16_t color);
/* set the background color */
void lcd_background_color_set(uint16_t color);

/* fill the whole rectangle */
void lcd_rectangle_fill(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height);
/* display the string on LCD */
void lcd_string_display(uint16_t stringline, uint8_t *ptr);
/* display the vertical string on LCD */
void lcd_vertical_string_display(uint16_t stringline, uint16_t offset, uint8_t *ptr);
void log_lcd_vertical_string_display(uint16_t stringline, uint16_t offset, uint8_t *ptr);
/* display the character on LCD */
void lcd_char_display(uint16_t line, uint16_t column, uint8_t ascii);
/* display the vertical character on LCD */
void lcd_vertical_char_display(uint16_t line, uint16_t column, uint8_t ascii);
/* draw the character on LCD */
void lcd_char_draw(uint16_t xpos, uint16_t ypos, const uint16_t *c);
/* draw the vertical character on LCD */
void lcd_vertical_char_draw(uint16_t xpos, uint16_t ypos, const uint16_t *c);
/* initialize the LCD log module */
void lcd_log_init (void);
/* de-initialize the LCD log module */
void lcd_log_deinit (void);
/* display the application header (title) on the LCD screen */
void lcd_log_header_set (uint8_t *p_title, uint16_t start_x);
/* display the application footer (status) on the LCD screen */
void lcd_log_footer_set (uint8_t *p_status, uint16_t start_x);
/* redirect the printf to the lcd */
void lcd_log_print (uint8_t *p_str, uint16_t offset, uint16_t char_color);
/* clear the text zone */
void lcd_log_text_zone_clear (uint16_t start_x,
                              uint16_t start_y,
                              uint16_t width,
                              uint16_t height);
#endif

