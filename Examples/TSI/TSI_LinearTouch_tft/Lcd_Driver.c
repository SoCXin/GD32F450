/**
  ******************************************************************************
  * @file    TSI/TSI_LinearTouch_tft/Lcd_Driver.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   LCD driver functions.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "Lcd_Driver.h"
#include "delay.h"

/* Private define ------------------------------------------------------------*/
#ifdef H_VIEW
    #define X_MAX_PIXEL         320
    #define Y_MAX_PIXEL         240
#else
    #define X_MAX_PIXEL         240
    #define Y_MAX_PIXEL         320
#endif

/* Private function prototypes -----------------------------------------------*/
uint8_t SPI_WriteByte(SPI_TypeDef* SPIx,uint8_t Byte);
void SPI1_Init(void);
void Lcd_WriteData16Bit(uint8_t DataH,uint8_t DataL);
void Lcd_Reset(void);

/* Private function  ---------------------------------------------------------*/
/**
  * @brief  Write an unsigned 8-bit bytes.
  * @param  SPIx,Byte
  * @retval None
  */
uint8_t SPI_WriteByte(SPI_TypeDef* SPIx,uint8_t Byte)
{
    while((SPIx->STR&SPI_FLAG_TBE)==RESET);
    SPIx->DTR=Byte;
    while((SPIx->STR&SPI_FLAG_RBNE)==RESET);
    return SPIx->DTR;
} 

/**
  * @brief  Init SPI1.
  * @param  None
  * @retval None
  */
void SPI1_Init(void)
{
    SPI_InitPara  SPI_InitStructure;
    {
        /* Configure the GPIO ports */
        GPIO_InitPara GPIO_InitStructure;

        /* Connect pin to Periph */
        GPIO_PinAFConfig( GPIOB , GPIO_PINSOURCE3, GPIO_AF_0 );
        GPIO_PinAFConfig( GPIOB , GPIO_PINSOURCE4, GPIO_AF_0 );
        GPIO_PinAFConfig( GPIOB , GPIO_PINSOURCE5, GPIO_AF_0 );

        /* Configure pins as AF pushpull */
        GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_3|
                                        GPIO_PIN_4|
                                        GPIO_PIN_5 ;
        GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
        GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
        GPIO_Init( GPIOB , &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUT;

        GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_3|GPIO_PIN_4;
        GPIO_Init( GPIOC , &GPIO_InitStructure); 

        GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_5;
        GPIO_Init( GPIOF , &GPIO_InitStructure);
    }

    RCC_APB2PeriphClock_Enable( RCC_APB2PERIPH_SPI1, ENABLE );

    SPI_InitStructure.SPI_TransType   = SPI_TRANSTYPE_FULLDUPLEX;
    SPI_InitStructure.SPI_Mode        = SPI_MODE_MASTER;
    SPI_InitStructure.SPI_FrameFormat = SPI_FRAMEFORMAT_8BIT;
    SPI_InitStructure.SPI_SCKPL       = SPI_SCKPL_HIGH;
    SPI_InitStructure.SPI_SCKPH       = SPI_SCKPH_2EDGE;
    SPI_InitStructure.SPI_SWNSSEN     = SPI_SWNSS_SOFT;
    SPI_InitStructure.SPI_PSC         = SPI_PSC_2 ;
    SPI_InitStructure.SPI_FirstBit    = SPI_FIRSTBIT_MSB;
    SPI_InitStructure.SPI_CRCPOL      = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Enable(SPI1, ENABLE);
}

/**
  * @brief  Write the register address.
  * @param  Index:register address
  * @retval None
  */
void Lcd_WriteIndex(uint8_t Index)
{
    LCD_RS_CLR;
    SPI_WriteByte(SPI1,Index);
}

/**
  * @brief  Write the register data.
  * @param  Data:register data
  * @retval None
  */
void Lcd_WriteData(uint8_t Data)
{
    LCD_RS_SET;
    SPI_WriteByte(SPI1,Data);
}

/**
  * @brief  Write the register data(an unsigned 16-bit data).
  * @param  DataH,DataL:register data
  * @retval None
  */
void Lcd_WriteData16Bit(uint8_t DataH,uint8_t DataL)
{
    Lcd_WriteData(DataH);
    Lcd_WriteData(DataL);
}

/**
  * @brief  Lcd reset.
  * @param  None
  * @retval None
  */
void Lcd_Reset(void)
{
    LCD_RST_CLR;
    delay_ms(100);
    LCD_RST_SET;
    delay_ms(50);
}

/**
  * @brief  Lcd Init.
  * @param  None
  * @retval None
  */
void Lcd_Init(void)
{
    SPI1_Init();

    LCD_CS_CLR;
    Lcd_Reset();

    Lcd_WriteIndex(0xCB);
    Lcd_WriteData(0x39);
    Lcd_WriteData(0x2C);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x34);
    Lcd_WriteData(0x02);

    Lcd_WriteIndex(0xCF);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0XC1);
    Lcd_WriteData(0X30);

    Lcd_WriteIndex(0xE8);
    Lcd_WriteData(0x85);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x78);

    Lcd_WriteIndex(0xEA);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x00);

    Lcd_WriteIndex(0xED);
    Lcd_WriteData(0x64);
    Lcd_WriteData(0x03);
    Lcd_WriteData(0X12);
    Lcd_WriteData(0X81);

    Lcd_WriteIndex(0xF7);
    Lcd_WriteData(0x20);

    /* Power control VRH[5:0] */
    Lcd_WriteIndex(0xC0);
    Lcd_WriteData(0x23);

    /* Power control SAP[2:0];BT[3:0] */
    Lcd_WriteIndex(0xC1);
    Lcd_WriteData(0x10);

    /* VCM control */
    Lcd_WriteIndex(0xC5);
    Lcd_WriteData(0x3e);
    Lcd_WriteData(0x28);

    /* VCM control2  */
    Lcd_WriteIndex(0xC7);
    Lcd_WriteData(0x86);

    /* Memory Access Control */
    Lcd_WriteIndex(0x36);

#ifdef H_VIEW
    Lcd_WriteData(0xE8);
#else
    Lcd_WriteData(0x48); 
#endif

    Lcd_WriteIndex(0x3A);
    Lcd_WriteData(0x55);

    Lcd_WriteIndex(0xB1);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x18);

    /* Display Function Control */
    Lcd_WriteIndex(0xB6);
    Lcd_WriteData(0x08);
    Lcd_WriteData(0x82);
    Lcd_WriteData(0x27);

    /* 3Gamma Function Disable */
    Lcd_WriteIndex(0xF2);
    Lcd_WriteData(0x00);

    /* Gamma curve selected  */
    Lcd_WriteIndex(0x26);
    Lcd_WriteData(0x01);

    /* Set Gamma  */
    Lcd_WriteIndex(0xE0);
    Lcd_WriteData(0x0F);
    Lcd_WriteData(0x31);
    Lcd_WriteData(0x2B);
    Lcd_WriteData(0x0C);
    Lcd_WriteData(0x0E);
    Lcd_WriteData(0x08);
    Lcd_WriteData(0x4E);
    Lcd_WriteData(0xF1);
    Lcd_WriteData(0x37);
    Lcd_WriteData(0x07);
    Lcd_WriteData(0x10);
    Lcd_WriteData(0x03);
    Lcd_WriteData(0x0E);
    Lcd_WriteData(0x09);
    Lcd_WriteData(0x00);

    /* Set Gamma  */
    Lcd_WriteIndex(0XE1);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x0E);
    Lcd_WriteData(0x14);
    Lcd_WriteData(0x03);
    Lcd_WriteData(0x11);
    Lcd_WriteData(0x07);
    Lcd_WriteData(0x31);
    Lcd_WriteData(0xC1);
    Lcd_WriteData(0x48);
    Lcd_WriteData(0x08);
    Lcd_WriteData(0x0F);
    Lcd_WriteData(0x0C);
    Lcd_WriteData(0x31);
    Lcd_WriteData(0x36);
    Lcd_WriteData(0x0F);

    /* Exit Sleep */
    Lcd_WriteIndex(0x11);
    delay_ms(120);

    /* Display on */
    Lcd_WriteIndex(0x29);
    Lcd_WriteIndex(0x2c); 

    LCD_CS_SET;
}

/**
  * @brief  Set lcd display region.
  * @param  x_start,y_start,x_end,y_end
  * @retval None
  */
void Lcd_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end)
{
    LCD_CS_CLR;
    Lcd_WriteIndex(0x2a);
    Lcd_WriteData16Bit(x_start>>8,x_start);
    Lcd_WriteData16Bit(x_end>>8,x_end);
    Lcd_WriteIndex(0x2b);
    Lcd_WriteData16Bit(y_start>>8,y_start);
    Lcd_WriteData16Bit(y_end>>8,y_end);
    Lcd_WriteIndex(0x2c);
    LCD_CS_SET;
}

/**
  * @brief  Set the start display point of lcd.
  * @param  x,y
  * @retval None
  */
void Lcd_SetXY(uint16_t x,uint16_t y)
{

    Lcd_WriteIndex(0x2a);
    Lcd_WriteData16Bit(x>>8,x);
    Lcd_WriteIndex(0x2b);
    Lcd_WriteData16Bit(y>>8,y);
    Lcd_WriteIndex(0x2c);
}

/**
  * @brief  Draw a point on the lcd.
  * @param  x,y,Data
  * @retval None
  */
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data)
{

    Lcd_SetXY(x,y);
    Lcd_WriteData(Data>>8);
    Lcd_WriteData(Data);
}

/**
  * @brief  Clear the lcd.
  * @param  Color
  * @retval None
  */
void Lcd_Clear(uint16_t Color)
{
    unsigned int i,m;
    Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
    LCD_RS_SET;

    LCD_CS_CLR;
    for(i=0;i<Y_MAX_PIXEL;i++)
    {
        for(m=0;m<X_MAX_PIXEL;m++)
        {
            SPI_WriteByte(SPI1,Color>>8);
            SPI_WriteByte(SPI1,Color);
        }
    }
    LCD_CS_SET;
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/

