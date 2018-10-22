/**
  ******************************************************************************
  * @file    TSI/TSI_LinearTouch_tft/GUI.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   GUI display functions.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "delay.h"
#include "font.h"

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  GBR to RGB format conversion.
  * @param  c:GBR color value.
  * @retval GBR color value.
  */
uint16_t LCD_BGR2RGB(uint16_t c)
{
    uint16_t r,g,b,rgb;
    b=(c>>0)&0x1f;
    g=(c>>5)&0x3f;
    r=(c>>11)&0x1f;
    rgb=(b<<11)+(g<<5)+(r<<0);
    return(rgb);
}

/**
  * @brief  Gui circle.
  * @param  X,Y,R,fc.
  * @retval None
  */
void Gui_Circle(uint16_t X,uint16_t Y,uint16_t R,uint16_t fc) 
{
    unsigned short  a,b; 
    int c; 
    a=0; 
    b=R; 
    c=3-2*R;
    LCD_CS_CLR;

    while (a<b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc);
        Gui_DrawPoint(X-a,Y+b,fc);
        Gui_DrawPoint(X+a,Y-b,fc);
        Gui_DrawPoint(X-a,Y-b,fc);
        Gui_DrawPoint(X+b,Y+a,fc);
        Gui_DrawPoint(X-b,Y+a,fc);
        Gui_DrawPoint(X+b,Y-a,fc);
        Gui_DrawPoint(X-b,Y-a,fc);

        if(c<0) c=c+4*a+6; 
        else 
        { 
            c=c+4*(a-b)+10; 
            b-=1; 
        } 
        a+=1; 
    } 
    if (a==b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y-b,fc); 
        Gui_DrawPoint(X-a,Y-b,fc); 
        Gui_DrawPoint(X+b,Y+a,fc); 
        Gui_DrawPoint(X-b,Y+a,fc); 
        Gui_DrawPoint(X+b,Y-a,fc); 
        Gui_DrawPoint(X-b,Y-a,fc); 
    } 
    LCD_CS_SET;
}

/**
  * @brief  Gui draw Line.
  * @param  x0,y0,x1,y1,Color.
  * @retval None
  */
void Gui_DrawLine(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1,uint16_t Color)
{
    /* -difference in x's
       -difference in y's
       -dx,dy * 2
       -amount in pixel space to move during drawing
       -amount in pixel space to move during drawing
       -the discriminant i.e. error i.e. decision variable
       -used for looping */
    int dx,dy,dx2,dy2,x_inc,y_inc,error,index;

    LCD_CS_CLR;
    Lcd_SetXY(x0,y0);

    /* Calculate X distance */
    dx = x1-x0;
    /* Calculate Y distance */
    dy = y1-y0;

    if (dx>=0)
    {
        x_inc = 1;
    }
    else
    {
        x_inc = -1;
        dx    = -dx;  
    } 

    if (dy>=0)
    {
        y_inc = 1;
    } 
    else
    {
        y_inc = -1;
        dy    = -dy; 
    } 

    dx2 = dx << 1;
    dy2 = dy << 1;

    if (dx > dy)
    {
        /* initialize error */
        error = dy2 - dx; 
        /* draw the line */
        for (index=0; index <= dx; index++)
        {
            Gui_DrawPoint(x0,y0,Color);
            /* test if error has overflowed */
            if (error >= 0)
            {
                error-=dx2;
                /* move to next line */
                y0+=y_inc;
            }
            /* adjust the error term */
            error+=dy2;
            /* move to the next pixel */
            x0+=x_inc;
        }
    }
    else
    {
        /* initialize error term */
        error = dx2 - dy; 
        /* draw the linedraw the line*/
        for (index=0; index <= dy; index++)
        {
            /* set the pixel */
            Gui_DrawPoint(x0,y0,Color);

            /* test if error overflowed */
            if (error >= 0)
            {
                error-=dy2;
                /* move to next line */
                x0+=x_inc;
            }
            /* adjust the error term */
            error+=dx2;

            /* move to the next pixel */
            y0+=y_inc;
        }
    }
    LCD_CS_SET;
}

/**
  * @brief  Gui box.
  * @param  x,y,x1,w,h,bc.
  * @retval None
  */
void Gui_box(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t bc)
{
    LCD_CS_CLR;
    Gui_DrawLine(x,y,x+w,y,0xEF7D);
    Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
    Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
    Gui_DrawLine(x,y,x,y+h,0xEF7D);
    Gui_DrawLine(x+1,y+1,x+1+w-2,y+1+h-2,bc);
    LCD_CS_SET;
}

/**
  * @brief  Gui box2.
  * @param  x,y,x1,w,h,mode.
  * @retval None
  */
void Gui_box2(uint16_t x,uint16_t y,uint16_t w,uint16_t h, uint8_t mode)
{
    LCD_CS_CLR;
    if (mode==0)
    {
        Gui_DrawLine(x,y,x+w,y,0xEF7D);
        Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
        Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
        Gui_DrawLine(x,y,x,y+h,0xEF7D);
    }
    if (mode==1)
    {
        Gui_DrawLine(x,y,x+w,y,0x2965);
        Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xEF7D);
        Gui_DrawLine(x,y+h,x+w,y+h,0xEF7D);
        Gui_DrawLine(x,y,x,y+h,0x2965);
    }
    if (mode==2)
    {
        Gui_DrawLine(x,y,x+w,y,0xffff);
        Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xffff);
        Gui_DrawLine(x,y+h,x+w,y+h,0xffff);
        Gui_DrawLine(x,y,x,y+h,0xffff);
    }
    LCD_CS_SET;
}

/**
  * @brief  Display button down.
  * @param  x1,y1,x2,y2.
  * @retval None
  */
void DisplayButtonDown(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    LCD_CS_CLR;
    Gui_DrawLine(x1,  y1,  x2,y1, GRAY2);
    Gui_DrawLine(x1+1,y1+1,x2,y1+1, GRAY1);
    Gui_DrawLine(x1,  y1,  x1,y2, GRAY2);
    Gui_DrawLine(x1+1,y1+1,x1+1,y2, GRAY1);
    Gui_DrawLine(x1,  y2,  x2,y2, WHITE);
    Gui_DrawLine(x2,  y1,  x2,y2, WHITE);
    LCD_CS_SET;
}

/**
  * @brief  Display button up.
  * @param  x1,y1,x2,y2.
  * @retval None
  */
void DisplayButtonUp(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    LCD_CS_CLR;
    Gui_DrawLine(x1,  y1,  x2,y1, WHITE);
    Gui_DrawLine(x1,  y1,  x1,y2, WHITE);

    Gui_DrawLine(x1+1,y2-1,x2,y2-1, GRAY1);
    Gui_DrawLine(x1,  y2,  x2,y2, GRAY2);
    Gui_DrawLine(x2-1,y1+1,x2-1,y2, GRAY1);
    Gui_DrawLine(x2  ,y1  ,x2,y2, GRAY2);
    LCD_CS_SET;
}


/**
  * @brief  Gui draw font to GBK16.
  * @param  x,y,fc,bc,*s.
  * @retval None
  */
void Gui_DrawFont_GBK16(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s)
{
    unsigned char i,j;
    unsigned short k,x0;
    x0=x;

    LCD_CS_CLR;
    while( *s )
    {
        /* ASCII character table from 32 to 128 */
        if((*s) < 128)
        {
            k=*s;
            if( k==13 )
            {
                x=x0;
                y+=16;
            }
            else 
            {
                if( k>32 )
                    k-=32;
                else k=0;

                for(i=0;i<16;i++)
                for(j=0;j<8;j++) 
                {
                    if(asc16[k*16+i]&(0x80>>j))  Gui_DrawPoint(x+j,y+i,fc);
                    else 
                    {
                        if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
                    }
                }
                x+=8;
            }
            s++;
        }
        else 
        {
            for(k=0;k<hz16_num;k++) 
            {
                if ((hz16[k].Index[0]==*(s))&&(hz16[k].Index[1]==*(s+1)))
                { 
                    for(i=0;i<16;i++)
                    {
                        for(j=0;j<8;j++) 
                        {
                            if(hz16[k].Msk[i*2]&(0x80>>j))
                                Gui_DrawPoint(x+j,y+i,fc);
                            else 
                            {
                                if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
                            }
                        }
                        for(j=0;j<8;j++) 
                        {
                            if(hz16[k].Msk[i*2+1]&(0x80>>j))
                                Gui_DrawPoint(x+j+8,y+i,fc);
                            else 
                            {
                                if(fc!=bc) 
                                    Gui_DrawPoint(x+j+8,y+i,bc);
                            }
                        }
                    }
                }
            }
            s+=2;x+=16;
       } 
    }
    LCD_CS_SET;
}

/**
  * @brief  Gui draw font to GBK24.
  * @param  x,y,fc,bc,*s.
  * @retval None
  */
void Gui_DrawFont_GBK24(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s)
{
    unsigned char i,j;
    unsigned short k;

    LCD_CS_CLR;
    while(*s) 
    {
        if( *s < 0x80 ) 
        {
            k=*s;
            if (k>32) k-=32; else k=0;
            for(i=0;i<16;i++)
            for(j=0;j<8;j++) 
            {
                if(asc16[k*16+i]&(0x80>>j))  
                    Gui_DrawPoint(x+j,y+i,fc);
                else 
                {
                    if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
                }
            }
            s++;x+=8;
        }
        else 
        {
            for(k=0;k<hz24_num;k++) 
            {
                if((hz24[k].Index[0]==*(s))&&(hz24[k].Index[1]==*(s+1)))
                {
                    for(i=0;i<24;i++)
                    {
                        for(j=0;j<8;j++) 
                        {
                            if(hz24[k].Msk[i*3]&(0x80>>j))
                                Gui_DrawPoint(x+j,y+i,fc);
                            else 
                            {
                                if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
                            }
                        }
                        for(j=0;j<8;j++) 
                        {
                            if(hz24[k].Msk[i*3+1]&(0x80>>j))
                                Gui_DrawPoint(x+j+8,y+i,fc);
                            else 
                            {
                                if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
                            }
                        }
                        for(j=0;j<8;j++) 
                        {
                            if(hz24[k].Msk[i*3+2]&(0x80>>j))
                                Gui_DrawPoint(x+j+16,y+i,fc);
                            else 
                            {
                                if(fc!=bc) Gui_DrawPoint(x+j+16,y+i,bc);
                            }
                        }
                    }
                }
            }
        s+=2;x+=24;
        }
    }
    LCD_CS_SET;
}

/**
  * @brief  Gui draw font to Num32.
  * @param  x,y,fc,bc,num.
  * @retval None
  */
void Gui_DrawFont_Num32(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint16_t num)
{
    unsigned char i,j,k,c;

    LCD_CS_CLR;
    for(i=0;i<32;i++)
    {
        for(j=0;j<4;j++) 
        {
            c=*(sz32+num*32*4+i*4+j);
            for (k=0;k<8;k++)
            {
                if(c&(0x80>>k))
                    Gui_DrawPoint(x+j*8+k,y+i,fc);
                else
                {
                    if (fc!=bc) Gui_DrawPoint(x+j*8+k,y+i,bc);
                }
            }
        }
    }
    LCD_CS_SET;
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/

