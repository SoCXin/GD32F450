/*!
    \file  lcd_log.c
    \brief LCD log driver

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

#include "lcd_log.h"
#include "exmc_sdram.h"
#include <string.h>

#define LCD_FRAME_BUFFER         ((uint32_t)0xC0000000)
#define BUFFER_OFFSET            ((uint32_t)0x7F800)
#define LCD_DEFAULT_FONT         Font16x24

uint16_t LINE;
static sFONT *current_font;
static uint16_t current_textcolor = 0x0000;
static uint16_t current_backcolor = 0xFFFF;
static uint32_t current_framebuffer = LCD_FRAME_BUFFER;
static uint32_t current_layer = LCD_LAYER_BACKGROUND;

static void pixel_set(int16_t x, int16_t y);

/*!
    \brief      initialize the LCD GPIO and TLI
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_init(void)
{
    tli_parameter_struct  tli_init_struct;
    /* GPIO clock enable */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOH);
    rcu_periph_clock_enable(RCU_GPIOI);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_GPIOF);

    /* configure HSYNC(PI10), VSYNC(PI9), PCLK(PG7) */
    /* configure LCD_R7(PG6), LCD_R6(PH12), LCD_R5(PH11), LCD_R4(PH10), LCD_R3(PH9),LCD_R2(PH8), 
                 LCD_R1(PH3), LCD_R0(PH2), LCD_G7(PI2), LCD_G6(PI1), LCD_G5(PI0), LCD_G4(PH15), 
                 LCD_G3(PH14), LCD_G2(PH13),LCD_G1(PE6), LCD_G0(PE5),LCD_B7(PI7), LCD_B6(PI6), 
                 LCD_B5(PI5), LCD_B4(PI4), LCD_B3(PG11),LCD_B2(PG10), LCD_B1(PG12), LCD_B0(PE4) */    
    /* TLI pins AF configure */
    gpio_af_set(GPIOE,GPIO_AF_14,GPIO_PIN_5); 
    gpio_af_set(GPIOE,GPIO_AF_14,GPIO_PIN_6);  
    gpio_af_set(GPIOE,GPIO_AF_14,GPIO_PIN_4);
    
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_2);  
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_3);
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_8);
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_9);  
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_10); 
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_11);  
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_12); 
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_13);  
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_14);     
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_15); 
    
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_0);  
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_1); 
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_2);     
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_4); 
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_5);  
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_6);     
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_7);
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_9);     
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_10);
    
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_6);     
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_7);
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_10);
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_11);     
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_12);    

    gpio_af_set(GPIOF,GPIO_AF_14,GPIO_PIN_10);  

    /* TLI GPIO configure */
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);

    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                 |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                           |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);

    gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2
                 |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_10);
    gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2
                           |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_10);
    
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_15);
    gpio_bit_set(GPIOB,GPIO_PIN_15);

    rcu_periph_clock_enable(RCU_TLI);

#ifdef __CC_ARM
    /* configure the PLLSAI clock to generate lcd clock */
    if(ERROR == rcu_pllsai_config(192, 2, 3)){
        while(1);
    }
#endif /* __CC_ARM */
#ifdef __IAR_SYSTEMS_ICC__
    /* configure the PLLSAI clock to generate lcd clock */
    if(ERROR == rcu_pllsai_config(192, 2, 3)){
        while(1);
    }
#endif /* __IAR_SYSTEMS_ICC__ */
    rcu_tli_clock_div_config(RCU_PLLSAIR_DIV8);
    rcu_osci_on(RCU_PLLSAI_CK);
    if(ERROR == rcu_osci_stab_wait(RCU_PLLSAI_CK)){
        while(1);
    }
    
    /* config the EXMC access mode */
    exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);
    
    /* TLI initialization */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;
    
    /* LCD display timing configuration */
    tli_init_struct.synpsz_hpsz = 40;
    tli_init_struct.synpsz_vpsz = 9;
    tli_init_struct.backpsz_hbpsz = 42; 
    tli_init_struct.backpsz_vbpsz = 11;  
    tli_init_struct.activesz_hasz = 522;
    tli_init_struct.activesz_vasz = 283;
    tli_init_struct.totalsz_htsz = 524; 
    tli_init_struct.totalsz_vtsz = 285;
    /* LCD background color configure*/
    tli_init_struct.backcolor_red = 0xFF;
    tli_init_struct.backcolor_green = 0xFF;
    tli_init_struct.backcolor_blue = 0xFF; 
    tli_init(&tli_init_struct);
}

/*!
    \brief      initialize TLI layer0 and layer1
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_layer_init(void)
{
    tli_layer_parameter_struct  tli_layer_init_struct;
    /* TLI layer1 configuration */
    tli_layer_init_struct.layer_window_leftpos = (0 + 43);
    tli_layer_init_struct.layer_window_rightpos = (0 + 480 + 43 - 1); 
    tli_layer_init_struct.layer_window_toppos = (0 + 12);
    tli_layer_init_struct.layer_window_bottompos = (0 + 272 + 12 - 1);
    tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;
    tli_layer_init_struct.layer_sa = 0xFF;
    tli_layer_init_struct.layer_default_blue = 0xFF;        
    tli_layer_init_struct.layer_default_green = 0xFF;       
    tli_layer_init_struct.layer_default_red = 0xFF;         
    tli_layer_init_struct.layer_default_alpha = 0x0;
    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_SA;    
    tli_layer_init_struct.layer_acf2 = LAYER_ACF2_SA;
    tli_layer_init_struct.layer_frame_bufaddr = LCD_FRAME_BUFFER + BUFFER_OFFSET;
    tli_layer_init_struct.layer_frame_line_length = ((LCD_PIXEL_WIDTH * 2) + 3); 
    tli_layer_init_struct.layer_frame_buf_stride_offset = (LCD_PIXEL_WIDTH * 2);
    tli_layer_init_struct.layer_frame_total_line_number = 60; 
    tli_layer_init(LAYER1, &tli_layer_init_struct);

    /* TLI layer0 configuration */
    tli_layer_init_struct.layer_window_leftpos = (0 + 43);
    tli_layer_init_struct.layer_window_rightpos = (0 + 480 + 43 - 1);
    tli_layer_init_struct.layer_window_toppos = (0 + 12);
    tli_layer_init_struct.layer_window_bottompos = (0 + 272 + 12 - 1);
    tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;
    tli_layer_init_struct.layer_sa = 0xFF;
    tli_layer_init_struct.layer_default_blue = 0xFF;        
    tli_layer_init_struct.layer_default_green = 0xFF;       
    tli_layer_init_struct.layer_default_red = 0xFF;         
    tli_layer_init_struct.layer_default_alpha = 0xFF;
    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;    
    tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;
    tli_layer_init_struct.layer_frame_bufaddr = LCD_FRAME_BUFFER;
    tli_layer_init_struct.layer_frame_line_length = ((LCD_PIXEL_WIDTH * 2) + 3); 
    tli_layer_init_struct.layer_frame_buf_stride_offset = (LCD_PIXEL_WIDTH * 2);
    tli_layer_init_struct.layer_frame_total_line_number = LCD_PIXEL_HEIGHT; 
    tli_layer_init(LAYER0, &tli_layer_init_struct);

    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    lcd_font_set(&LCD_DEFAULT_FONT);
    tli_dither_config(TLI_DITHER_ENABLE);
}

/*!
    \brief      set the LCD layer
    \param[in]  layer: LCD layer
      \arg        LCD_LAYER_BACKGROUND
      \arg        LCD_LAYER_FOREGROUND
    \param[out] none
    \retval     none
*/
void lcd_layer_set(uint32_t layer)
{
    if (layer == LCD_LAYER_BACKGROUND){
        current_framebuffer = LCD_FRAME_BUFFER;
        current_layer = LCD_LAYER_BACKGROUND;
    }else{
        current_framebuffer = LCD_FRAME_BUFFER + BUFFER_OFFSET;
        current_layer = LCD_LAYER_FOREGROUND;
    }
}

/*!
    \brief      clear the LCD with specified color
    \param[in]  color: LCD color
      \arg        LCD_COLOR_WHITE
      \arg        LCD_COLOR_BLACK
      \arg        LCD_COLOR_GREY
      \arg        LCD_COLOR_BLUE
      \arg        LCD_COLOR_BLUE2
      \arg        LCD_COLOR_RED
      \arg        LCD_COLOR_MAGENTA
      \arg        LCD_COLOR_GREEN
      \arg        LCD_COLOR_CYAN
      \arg        LCD_COLOR_YELLOW
    \param[out] none
    \retval     none
*/
void lcd_clear(uint16_t color)
{
    uint32_t index = 0;
    for (index = 0x00; index < BUFFER_OFFSET; index++){
        *(__IO uint16_t*)(current_framebuffer + (2*index)) = color;
    }
}

/*!
    \brief      set the transparency of LCD
    \param[in]  trans: transparency of LCD, from 0 to 255
    \param[out] none
    \retval     none
*/
void lcd_transparency_set(uint8_t trans)
{
    if (current_layer == LCD_LAYER_BACKGROUND){
        TLI_LxSA(LAYER0) = trans;
    }else{
        TLI_LxSA(LAYER1) = trans;
    }
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
}

/*!
    \brief      set point with the specified position and color
    \param[in]  xpos: position of x
    \param[in]  ypos: position of y
    \param[in]  color: LCD color
      \arg        LCD_COLOR_WHITE
      \arg        LCD_COLOR_BLACK
      \arg        LCD_COLOR_GREY
      \arg        LCD_COLOR_BLUE
      \arg        LCD_COLOR_BLUE2
      \arg        LCD_COLOR_RED
      \arg        LCD_COLOR_MAGENTA
      \arg        LCD_COLOR_GREEN
      \arg        LCD_COLOR_CYAN
      \arg        LCD_COLOR_YELLOW
    \param[out] none
    \retval     none
*/
void lcd_point_set(uint16_t xpos, uint16_t ypos, uint16_t color)
{
    *(__IO uint16_t*)(current_framebuffer + 2*((LCD_PIXEL_WIDTH*ypos) + xpos)) = color;
}

/*!
    \brief      get point with the specified position and color
    \param[in]  xpos: position of x
    \param[in]  ypos: position of y
    \param[out] none
    \retval     value of point (x, y)
*/
uint16_t lcd_point_get(uint16_t xpos, uint16_t ypos)
{
    return *(__IO uint16_t*)(current_framebuffer + 2*((LCD_PIXEL_WIDTH*ypos) + xpos));
}

/*!
    \brief      set the text font
    \param[in]  font: the text font
    \param[out] none
    \retval     none
*/
void lcd_font_set(sFONT *font)
{
    current_font = font;
}

/*!
    \brief      get the text font
    \param[in]  none
    \param[out] none
    \retval     the text font
*/
sFONT* lcd_font_get(void)
{
    return current_font;
}

/*!
    \brief      set the text color
    \param[in]  color: LCD color
      \arg        LCD_COLOR_WHITE
      \arg        LCD_COLOR_BLACK
      \arg        LCD_COLOR_GREY
      \arg        LCD_COLOR_BLUE
      \arg        LCD_COLOR_BLUE2
      \arg        LCD_COLOR_RED
      \arg        LCD_COLOR_MAGENTA
      \arg        LCD_COLOR_GREEN
      \arg        LCD_COLOR_CYAN
      \arg        LCD_COLOR_YELLOW
    \param[out] none
    \retval     none
*/
void lcd_text_color_set(uint16_t color)
{
    current_textcolor = color;
}

/*!
    \brief      set the background color
    \param[in]  color: LCD color
      \arg        LCD_COLOR_WHITE
      \arg        LCD_COLOR_BLACK
      \arg        LCD_COLOR_GREY
      \arg        LCD_COLOR_BLUE
      \arg        LCD_COLOR_BLUE2
      \arg        LCD_COLOR_RED
      \arg        LCD_COLOR_MAGENTA
      \arg        LCD_COLOR_GREEN
      \arg        LCD_COLOR_CYAN
      \arg        LCD_COLOR_YELLOW
    \param[out] none
    \retval     none
*/
void lcd_background_color_set(uint16_t color)
{
    current_backcolor = color;
}

/*!
    \brief      fill the whole rectangle
    \param[in]  xpos: position of x
    \param[in]  ypos: position of y
    \param[in]  width: width of the rectangle
    \param[in]  height: height of the rectangle
    \param[out] none
    \retval     none
*/
void lcd_rectangle_fill(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height)
{
    uint16_t x, y;

    for(x = xpos; x < xpos + width; x++){
        for(y = ypos; y < ypos + height; y++){
            pixel_set(x, y);
        }
    }
}

/*!
    \brief      display the string on LCD
    \param[in]  stringline: line to display the character
      \arg        LCD_LINE_x(x=0..39 for 480*272)
    \param[in]  ptr: a pointer to the string
    \param[out] none
    \retval     none
*/
void lcd_string_display(uint16_t stringline, uint8_t *ptr)
{
    uint16_t column = 0;

    while ((column < LCD_PIXEL_WIDTH) && (*ptr != 0)){
        /* display character on LCD */
        lcd_char_display(stringline, column, *ptr);
        column += current_font->Width;
        ptr++;
    }
}

/*!
    \brief      display the vertical string on LCD
    \param[in]  stringline: line to display the character
      \arg        LCD_LINE_x(x=0..39 for 480*272) 
    \param[in]  offset: offset value
    \param[in]  ptr: a pointer to the string
    \param[out] none
    \retval     none
*/
void lcd_vertical_string_display(uint16_t stringline, uint16_t offset, uint8_t *ptr)
{
    uint16_t column = LCD_PIXEL_HEIGHT - (current_font->Width + offset + 2);

    while ((column > 0) && (*ptr != 0)){
        /* display character on LCD */
        lcd_vertical_char_display(stringline, column, *ptr);
        column -= current_font->Width;
        ptr++;
    }
}

/*!
    \brief      display the vertical string on LCD
    \param[in]  stringline: line to display the character
      \arg        LCD_LINE_x(x=0..39 for 480*272) 
    \param[in]  offset: offset value
    \param[in]  ptr: a pointer to the string
    \param[out] none
    \retval     none
*/
void log_lcd_vertical_string_display(uint16_t stringline, uint16_t offset, uint8_t *ptr)
{
    uint16_t column = LCD_PIXEL_HEIGHT - (current_font->Width + offset + 2);
    uint16_t len = strlen((const char*)ptr);
    while (*ptr != 0){
       if (column >= current_font->Width) {
           /* display character on LCD */
            lcd_vertical_char_display(stringline, column, *ptr);
            column -= current_font->Width;
            ptr++;
       } else if (len > column/current_font->Width){
            len -= column/current_font->Width;
            column = LCD_PIXEL_HEIGHT - (current_font->Width + offset + 2);
            stringline += 20;
            LINE += 20;
       }
    }
    
    /* delay for display */
    for (uint32_t delay_i=10000; delay_i>0; delay_i--) {
        for (uint32_t delay_j=1000; delay_j>0; delay_j--);
    }
}

/*!
    \brief      display the character on LCD
    \param[in]  line: line to display the character
      \arg        LCD_LINE_x(x=0..39 for 480*272) 
    \param[in]  column: column address
    \param[in]  ascii: character ascii code(from 0x20 to 0x7E)
    \param[out] none
    \retval     none
*/
void lcd_char_display(uint16_t line, uint16_t column, uint8_t ascii)
{
    ascii -= 32;
    lcd_char_draw(line, column, &current_font->table[ascii * current_font->Height]);
}

/*!
    \brief      display the vertical character on LCD
    \param[in]  line: line to display the character
      \arg        LCD_LINE_x(x=0..39 for 480*272) 
    \param[in]  column: column address
    \param[in]  ascii: character ascii code(from 0x20 to 0x7E)
    \param[out] none
    \retval     none
*/
void lcd_vertical_char_display(uint16_t line, uint16_t column, uint8_t ascii)
{
    ascii -= 32;
    lcd_vertical_char_draw(line, column, &current_font->table[ascii * current_font->Height]);
}

/*!
    \brief      draw the character on LCD
    \param[in]  xpos: position of x
    \param[in]  ypos: position of y
    \param[in]  c: a pointer to the character
    \param[out] none
    \retval     none
*/
void lcd_char_draw(uint16_t xpos, uint16_t ypos, const uint16_t *c)
{
    uint32_t index = 0, counter = 0, x = 0;
    uint32_t xaddress = 0;

    x = xpos * LCD_PIXEL_WIDTH * 2;
    xaddress += ypos;

    for(index = 0; index < current_font->Height; index++){
        for(counter = 0; counter < current_font->Width; counter++){
            if((((c[index] & ((0x80 << ((current_font->Width / 12) * 8)) >> counter)) == 0x00) && (current_font->Width <= 12))||
                (((c[index] & (0x1 << counter)) == 0x00) && (current_font->Width > 12))){
                /* write the background color */
                *(__IO uint16_t*) (current_framebuffer + (2*xaddress) + x) = current_backcolor;
            }else{
                /* write the text color */
                *(__IO uint16_t*) (current_framebuffer + (2*xaddress) + x) = current_textcolor;
            }

            xaddress++;
        }

        xaddress += (LCD_PIXEL_WIDTH - current_font->Width);
    }
}

/*!
    \brief      draw the vertical character on LCD
    \param[in]  xpos: position of x
    \param[in]  ypos: position of y
    \param[in]  c: a pointer to the character
    \param[out] none
    \retval     none
*/
void lcd_vertical_char_draw(uint16_t xpos, uint16_t ypos, const uint16_t *c)
{
    uint32_t index = 0, counter = 0;

    for(index = 0; index < current_font->Height; index++){
        for(counter = 0; counter < current_font->Width; counter++){
            if((((c[index] & ((0x80 << ((current_font->Width / 12) * 8)) >> counter)) == 0x00) && (current_font->Width <= 12))||
                (((c[index] & (0x1 << counter)) == 0x00) && (current_font->Width > 12))){
            }else{
                pixel_set(xpos + index, ypos + current_font->Width - counter);
            }
        }
    }
}

/*!
    \brief      set a pixel
    \param[in]  x: x value
    \param[in]  y: y value
    \param[out] none
    \retval     none
*/
static void pixel_set(int16_t x, int16_t y)
{
    if(x < 0 || x > (LCD_PIXEL_WIDTH - 1) || y < 0 || y > (LCD_PIXEL_HEIGHT - 1)){
        return;
    }

    /* draw pixel with current text color */
    *(__IO uint16_t*)(current_framebuffer + 2*((LCD_PIXEL_WIDTH*y) + x)) = current_textcolor;
}

/*!
    \brief      initialize the LCD log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_init (void)
{
    lcd_clear(LCD_COLOR_GREEN);
    
    lcd_font_set(&Font8x16);
    
    lcd_text_color_set(LCD_COLOR_RED);
    
    lcd_background_color_set(LCD_COLOR_WHITE);
}

/*!
    \brief      de-initialize the LCD log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_deinit (void)
{
}

/*!
    \brief      display the application header (title) on the LCD screen 
    \param[in]  p_title: pointer to the string to be displayed
    \param[in]  start_x: the start x position
    \param[out] none
    \retval     none
*/
void lcd_log_header_set (uint8_t *p_title, uint16_t start_x)
{
    lcd_text_color_set(LCD_COLOR_BLUE2);

    lcd_rectangle_fill(0, 0, 30, 272);

    lcd_text_color_set(LCD_COLOR_RED);

    lcd_vertical_string_display(8, start_x, p_title);
    LINE = 30;
}

/*!
    \brief      display the application footer (status) on the LCD screen 
    \param[in]  p_status: pointer to the string to be displayed
    \param[in]  start_x: the start x position
    \param[out] none
    \retval     none
*/
void lcd_log_footer_set (uint8_t *p_status, uint16_t start_x)
{
    lcd_text_color_set(LCD_COLOR_BLUE2);

    lcd_rectangle_fill(450, 0, 480, 272);

    lcd_text_color_set(LCD_COLOR_RED);

    lcd_vertical_string_display(458, start_x, p_status);
}

/*!
    \brief      clear the text zone 
    \param[in]  start_x: the start x position
    \param[in]  start_y: the start y position
    \param[in]  width: the width to clear text zone
    \param[in]  height: the heitht to clear text zone
    \param[out] none
    \retval     none
*/
void lcd_log_text_zone_clear(uint16_t start_x,
                             uint16_t start_y,
                             uint16_t width,
                             uint16_t height)
{
    lcd_rectangle_fill(start_x, start_y, width, height);
    LINE = 30;
}

/*!
    \brief      redirect the printf to the lcd 
    \param[in]  p_str: pointer to string to be displayed
    \param[in]  offset: the offset to set
    \param[in]  char_color: the clar color to set
    \param[out] none
    \retval     none
*/
void lcd_log_print (uint8_t *p_str, uint16_t offset, uint16_t char_color)
{
#if defined(USE_HOST_MODE) && defined(USE_DEVICE_MODE)
    if(LINE >= 400){
        LINE = 40;
        lcd_text_color_set(LCD_COLOR_GREEN);
        lcd_rectangle_fill(30, 0, 390, 272);
    }
#else
    if(LINE >= 380){
        LINE = 40;
        lcd_text_color_set(LCD_COLOR_GREEN);
        lcd_rectangle_fill(30, 0, 355, 272);
    }
#endif

    lcd_text_color_set(char_color);

    log_lcd_vertical_string_display(LINE, offset, p_str);

    LINE += 20;
}
