/**
  ******************************************************************************
  * @file    Lcd_seg.c 
  * @author  MCU SD
  * @version V1.0.0
  * @date    15-Jan-2016
  * @brief   LCD driver.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lcd_seg.h"
#include "gd32f1x0_lcd.h"

/* Private variables ---------------------------------------------------------*/
/* Digit LCD DATA buffer */
uint8_t digit[4];

/* Table of the digit code for LCD */
__I uint32_t NumberTable[10]= 
{
/* 0     1     2     3     4 */
0xDD, 0x88, 0x79, 0xE9, 0xAC,
/* 5     6     7     8     9 */
0xE5, 0xF5, 0x89, 0xFD, 0xED
};

/* Private function prototypes -----------------------------------------------*/
static void DigittoCode(uint8_t c);
static void LCD_GPIO_Config(void);
static void LCD_Seg_WriteDigit(uint8_t ch, uint8_t position , Display_Typedef type);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Convert digit to LCD code.
  * @param  c: The digit to write.
  * @retval None
  */
static void DigittoCode(uint8_t c)
{
    uint8_t ch = 0;

    /* The *c is a number */
    if ( c < 10 )
    {
        ch = NumberTable[c];
    }

    digit[0] = (uint8_t)((ch)&0x03);
    digit[1] = (uint8_t)((ch>>2)&0x03);
    digit[2] = (uint8_t)((ch>>4)&0x03);
    digit[3] = (uint8_t)((ch>>6)&0x03);
}

/**
  * @brief  Init the GPIO port of LCD peripheral.
  * @retval None
  */
static void LCD_GPIO_Config(void)
{
    GPIO_InitPara GPIO_InitStructure;

    /* Enable the clock of GPIO */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB | RCC_AHBPERIPH_GPIOC | RCC_AHBPERIPH_GPIOF,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF; 
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_PinAFConfig( GPIOA, GPIO_PINSOURCE8, GPIO_AF_11);
    GPIO_PinAFConfig( GPIOA, GPIO_PINSOURCE9, GPIO_AF_11);
    GPIO_PinAFConfig( GPIOA, GPIO_PINSOURCE10, GPIO_AF_11);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_9 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    GPIO_PinAFConfig( GPIOB, GPIO_PINSOURCE9, GPIO_AF_11);
    GPIO_PinAFConfig( GPIOB, GPIO_PINSOURCE14, GPIO_AF_11);
    GPIO_PinAFConfig( GPIOB, GPIO_PINSOURCE15, GPIO_AF_11);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
    GPIO_PinAFConfig( GPIOC, GPIO_PINSOURCE4, GPIO_AF_11);
    GPIO_PinAFConfig( GPIOC, GPIO_PINSOURCE5, GPIO_AF_11);
    GPIO_PinAFConfig( GPIOC, GPIO_PINSOURCE6, GPIO_AF_11);
    GPIO_PinAFConfig( GPIOC, GPIO_PINSOURCE7, GPIO_AF_11);
    GPIO_PinAFConfig( GPIOC, GPIO_PINSOURCE8, GPIO_AF_11);
    GPIO_PinAFConfig( GPIOC, GPIO_PINSOURCE9, GPIO_AF_11);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOF,&GPIO_InitStructure);
    GPIO_PinAFConfig( GPIOF, GPIO_PINSOURCE4, GPIO_AF_11);
    GPIO_PinAFConfig( GPIOF, GPIO_PINSOURCE5, GPIO_AF_11);
    GPIO_PinAFConfig( GPIOF, GPIO_PINSOURCE6, GPIO_AF_11);
    GPIO_PinAFConfig( GPIOF, GPIO_PINSOURCE7, GPIO_AF_11);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_2 | GPIO_PIN_0 | GPIO_PIN_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    GPIO_PinAFConfig( GPIOB, GPIO_PINSOURCE0, GPIO_AF_11);
    GPIO_PinAFConfig( GPIOB, GPIO_PINSOURCE2, GPIO_AF_11);
    GPIO_PinAFConfig( GPIOB, GPIO_PINSOURCE12, GPIO_AF_11);

}

/**
  * @brief  Init the GPIO port of the LCD and LCD peripheral.
  * @param  None 
  * @retval None
  */
void LCD_Seg_Init(void)
{
    uint16_t i;
    LCD_InitPara LCD_InitStructure;

    /* Configure the LCD GPIO pins */
    LCD_GPIO_Config(); 

    /*!< Enable the clock of LCD */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_LCD, ENABLE);
    for(i=0;i<500;i++);
    /* Configure the LCD interface */
    LCD_InitStructure.LCD_Prescaler = LCD_PRESCALER_1;
    LCD_InitStructure.LCD_Divider = LCD_DIVIDER_16;
    LCD_InitStructure.LCD_Duty = LCD_DUTY_1_4;
    LCD_InitStructure.LCD_VBias = LCD_BIAS_1_3;
    LCD_InitStructure.LCD_VSRC = LCD_VOLTAGESOURCE_INTERNAL;
    LCD_Init(&LCD_InitStructure);

    /* Configure the Pulse On Duration */
    LCD_PulseOnDurationConfig(LCD_PULSEON_DURATION_7);

    /* Configure the Datatime Period */
    LCD_DeadTimeConfig(LCD_DEADTIME_PERIOD_7);

    /* Configure the LCD Contrast LEVEL_7(3.51V) */
    LCD_ContrastConfig(LCD_CONTRAST_LEVEL_7);

    /* Enable high drive function */
    LCD_HighDrive_Enable(ENABLE);

    /* Loop until SYNF flag is set */
    while ((LCD->SFR & LCD_FLAG_SYNF) == (uint32_t)RESET)
    {
    }
    /* Enable LCD peripheral */
    LCD_Enable(ENABLE);

    /* Wait the LCD is enable */
    while(!(LCD_GetBitState(LCD_FLAG_ONF)))
    {
    }

    /* Wait the LCD CFGR register synchronization */
//    while(!(LCD_GetBitState(LCD_FLAG_VRDYF)))
//    {
//    }

}

/**
  * @brief  This function write one digit to the LCD DATA register.
  * @param  ch: The digit to write.
  * @param  point: Whether add a point to the digit.
  *   This parameter can be one of the following values:  
  *     @arg POINT_OFF: No point in back of digit.
  *     @arg POINT_ON: Add a point in back of digit.
  * @param  position: Position in the LCD of the digit to write.
  *   This number must be 1-6.
  * @retval None
  */
void LCD_Seg_DisplayDigit(uint8_t ch, uint8_t position)
{
    /* Wait the last LCD DATA update  request finished */
    while(LCD_GetBitState(LCD_FLAG_UPRF))
    {
    }

    /* LCD Write a Char */
    LCD_Seg_WriteDigit(ch, position , INTEGER);

    /* Request LCD DATA update */
    LCD_DataUpdateRequest();
}

/**
  * @brief  This function write a integer(6 digits) to LCD DATA register.
  * @param  num: Number to send to LCD(0-999999).
  * @retval None
  */
void LCD_Seg_DisplayNumber(uint16_t num)
{
    uint8_t i = 0x00 , length , ch[6];

    ch[5] = num/100000;
    ch[4] = (num%100000)/10000;
    ch[3] = (num%10000)/1000;
    ch[2] = ( num%1000 )/100;
    ch[1] = ( num%100 )/10;
    ch[0] = num%10;
    
    if(ch[5]) length = 6;
    else if(ch[4]) length = 5;
    else if(ch[3]) length = 4;
    else if(ch[2]) length = 3;
    else if(ch[1]) length = 2;
    else length = 1;

    LCD_Seg_ClearAll();
    /* Wait the last LCD DATA update request finished */
    while(LCD_GetBitState(LCD_FLAG_UPRF))
    {
    }

    /* Send the string character one by one to LCD */
    while (i < length)
    {
        /* Display one digit on LCD */
        LCD_Seg_WriteDigit( ch[i] , 6-i , INTEGER );
        /* Increment the digit counter */
        i++;
    }

    /* Request LCD DATA update */
    LCD_DataUpdateRequest();
}

/**
  * @brief  This function write a float number(6 digits which has 2 decimal) to LCD DATA register.
  * @param  num: Number to send to LCD.
  * @retval None
  */
void LCD_Seg_DisplayFloatNumber(float num)
{
    uint8_t i = 0x00 , length , ch[6];
    uint32_t temp;

    temp = (uint32_t)( num*100 );
    ch[5] = temp/100000;
    ch[4] = ( temp%100000 )/10000;
    ch[3] = ( temp%10000 )/1000;
    ch[2] = ( temp%1000 )/100;
    ch[1] = ( temp%100 )/10;
    ch[0] = temp%10;

    if(ch[5]) length = 6;
    else if(ch[4]) length = 5;
    else if(ch[3]) length = 4;
    else length = 3;

    LCD_Seg_ClearAll();
    /* Wait the last LCD DATA update request finished */
    while(LCD_GetBitState(LCD_FLAG_UPRF))
    {
    }

    /* Send the string character one by one to LCD */
    while (i < length)
    {
        /* Display one digit on LCD */
        LCD_Seg_WriteDigit(ch[i], 6-i , FLOAT);
        /* Increment the digit counter */
        i++;
    }

    /* Request LCD DATA update */
    LCD_DataUpdateRequest();
}

/**
  * @brief  This function write time to LCD DATA register.
  * @param  num: Number to send to LCD.
  * @retval None
  */
void LCD_Seg_DisplayTime(uint8_t hour, uint8_t minute , uint8_t second)
{
    uint8_t i = 0x00 , ch[6];

    ch[0] = hour/10;
    ch[1] = hour%10;
    ch[2] = minute/10;
    ch[3] = minute%10;
    ch[4] = second/10;
    ch[5] = second%10;

    /* Wait the last LCD DATA update request finished */
    while(LCD_GetBitState(LCD_FLAG_UPRF))
    {
    }

    /* Send the string character one by one to LCD */
    while (i < 6)
    {
        /* Display one digit on LCD */
        LCD_Seg_WriteDigit(ch[i], i+1 , TIME);
        /* Increment the digit counter */
        i++;
    }

    /* Request LCD DATA update */
    LCD_DataUpdateRequest();
}

/**
  * @brief  This function write a digit to LCD DATA register.
  * @param  ch: The digit to write.
  * @param  point: Whether add a point to the digit.
  *   This parameter can be one of the following values:  
  *     @arg POINT_OFF: No point in back of digit.
  *     @arg POINT_ON: Add a point in back of digit.
  * @param  position: Position in the LCD of the digit to write.
  *   This number must be 1-6.
  * @retval None
  */
static void LCD_Seg_WriteDigit(uint8_t ch, uint8_t position , Display_Typedef type )
{
    /* Convert ASCii to LCD digit or char */
    DigittoCode(ch);

    switch (position)
    {
        case 6:
        /* Clear the corresponding segments (COM0-COM3, SEG30-31) */
        LCD->DATA0 &= (uint32_t)(0x3FFFFFFF);
        LCD->DATA1 &= (uint32_t)(0x3FFFFFFF);
        LCD->DATA2 &= (uint32_t)(0x3FFFFFFF);
        LCD->DATA3 &= (uint32_t)(0x3FFFFFFF);

        /* Write the colon of the time (COM0, SEG27) */
        if(type == TIME)
        LCD->DATA0 |= (uint32_t)((uint32_t)0x01 << 31);
        /* Write the corresponding segments (COM0-COM3, SEG30-31) */
        LCD->DATA0 |= (uint32_t)(digit[0] << 30);
        LCD->DATA1 |= (uint32_t)(digit[1] << 30);
        LCD->DATA2 |= (uint32_t)(digit[2] << 30);
        LCD->DATA3 |= (uint32_t)(digit[3] << 30);
        break;

        case 5:
        /* Clear the corresponding segments (COM0-COM3, SEG28-29) */
        LCD->DATA0 &= (uint32_t)(0xCFFFFFFF);
        LCD->DATA1 &= (uint32_t)(0xCFFFFFFF);
        LCD->DATA2 &= (uint32_t)(0xCFFFFFFF);
        LCD->DATA3 &= (uint32_t)(0xCFFFFFFF);

        /* Write the corresponding segments (COM0-COM3, SEG28-29) */
        LCD->DATA0 |= (uint32_t)(digit[0] << 28);
        LCD->DATA1 |= (uint32_t)(digit[1] << 28);
        LCD->DATA2 |= (uint32_t)(digit[2] << 28);
        LCD->DATA3 |= (uint32_t)(digit[3] << 28);
        break;

        case 4:
        /* Clear the corresponding segments (COM0-COM3, SEG26-27) */
        LCD->DATA0 &= (uint32_t)(0xF3FFFFFF);
        LCD->DATA1 &= (uint32_t)(0xF3FFFFFF);
        LCD->DATA2 &= (uint32_t)(0xF3FFFFFF);
        LCD->DATA3 &= (uint32_t)(0xF3FFFFFF);

        /* Write the point (COM0, SEG27) */
        if(type == FLOAT)
        LCD->DATA0 |= (uint32_t)(0x01 << 27);
        /* Write the corresponding segments (COM0-COM3, SEG26-27) */
        LCD->DATA0 |= (uint32_t)(digit[0] << 26);
        LCD->DATA1 |= (uint32_t)(digit[1] << 26);
        LCD->DATA2 |= (uint32_t)(digit[2] << 26);
        LCD->DATA3 |= (uint32_t)(digit[3] << 26);
        break;

        case 3:
        /* Clear the corresponding segments (COM0-COM3, SEG24-25) */
        LCD->DATA0 &= (uint32_t)(0xFCFFFFFF);
        LCD->DATA1 &= (uint32_t)(0xFCFFFFFF);
        LCD->DATA2 &= (uint32_t)(0xFCFFFFFF);
        LCD->DATA3 &= (uint32_t)(0xFCFFFFFF);

        /* Write the colon of the time (COM0, SEG25) */
        if(type == TIME)
        LCD->DATA0 |= (uint32_t)(0x01 << 25);
        /* Write the corresponding segments (COM0-COM3, SEG24-25) */
        LCD->DATA0 |= (uint32_t)(digit[0] << 24);
        LCD->DATA1 |= (uint32_t)(digit[1] << 24);
        LCD->DATA2 |= (uint32_t)(digit[2] << 24);
        LCD->DATA3 |= (uint32_t)(digit[3] << 24);
        break;

        case 2:
        /* Clear the corresponding segments (COM0-COM3, SEG22-23) */
        LCD->DATA0 &= (uint32_t)(0xFF3FFFFF);
        LCD->DATA1 &= (uint32_t)(0xFF3FFFFF);
        LCD->DATA2 &= (uint32_t)(0xFF3FFFFF);
        LCD->DATA3 &= (uint32_t)(0xFF3FFFFF);

        /* Write the corresponding segments (COM0-COM3, SEG22-23) */
        LCD->DATA0 |= (uint32_t)(digit[0] << 22);
        LCD->DATA1 |= (uint32_t)(digit[1] << 22);
        LCD->DATA2 |= (uint32_t)(digit[2] << 22);
        LCD->DATA3 |= (uint32_t)(digit[3] << 22);
        break;


        case 1:
        /* Clear the corresponding segments (COM0-COM3, SEG14-15) */
        LCD->DATA0 &= (uint32_t)(0xFFFF3FFF);
        LCD->DATA1 &= (uint32_t)(0xFFFF3FFF);
        LCD->DATA2 &= (uint32_t)(0xFFFF3FFF);
        LCD->DATA3 &= (uint32_t)(0xFFFF3FFF);

        /* Write the corresponding segments (COM0-COM3, SEG14-15) */
        LCD->DATA0 |= (uint32_t)(digit[0] << 14);
        LCD->DATA1 |= (uint32_t)(digit[1] << 14);
        LCD->DATA2 |= (uint32_t)(digit[2] << 14);
        LCD->DATA3 |= (uint32_t)(digit[3] << 14);
        break;

    }
}

/**
  * @brief This function clear data in the LCD DATA register.
  * @param  position: Position in the LCD of the digit to write.
  *   This number must be 1-6.
  * @retval None
  */
void LCD_Seg_ClearDigit(uint8_t position)
{
  switch (position)
  {
    case 6:
        /* Clear the corresponding segments (COM0-COM3, SEG30-31) */
        LCD->DATA0 &= (uint32_t)(0x3FFFFFFF);
        LCD->DATA1 &= (uint32_t)(0x3FFFFFFF);
        LCD->DATA2 &= (uint32_t)(0x3FFFFFFF);
        LCD->DATA3 &= (uint32_t)(0x3FFFFFFF);
        break;

    case 5:
        /* Clear the corresponding segments (COM0-COM3, SEG28-29) */
        LCD->DATA0 &= (uint32_t)(0xCFFFFFFF);
        LCD->DATA1 &= (uint32_t)(0xCFFFFFFF);
        LCD->DATA2 &= (uint32_t)(0xCFFFFFFF);
        LCD->DATA3 &= (uint32_t)(0xCFFFFFFF);
        break;

    case 4:
        /* Clear the corresponding segments (COM0-COM3, SEG26-27) */
        LCD->DATA0 &= (uint32_t)(0xF3FFFFFF);
        LCD->DATA1 &= (uint32_t)(0xF3FFFFFF);
        LCD->DATA2 &= (uint32_t)(0xF3FFFFFF);
        LCD->DATA3 &= (uint32_t)(0xF3FFFFFF);
        break;

    case 3:
        /* Clear the corresponding segments (COM0-COM3, SEG24-25) */
        LCD->DATA0 &= (uint32_t)(0xFCFFFFFF);
        LCD->DATA1 &= (uint32_t)(0xFCFFFFFF);
        LCD->DATA2 &= (uint32_t)(0xFCFFFFFF);
        LCD->DATA3 &= (uint32_t)(0xFCFFFFFF);
        break;

    case 2:
        /* Clear the corresponding segments (COM0-COM3, SEG22-23) */
        LCD->DATA0 &= (uint32_t)(0xFF3FFFFF);
        LCD->DATA1 &= (uint32_t)(0xFF3FFFFF);
        LCD->DATA2 &= (uint32_t)(0xFF3FFFFF);
        LCD->DATA3 &= (uint32_t)(0xFF3FFFFF);
        break;

    case 1:
        /* Clear the corresponding segments (COM0-COM3, SEG14-15) */
        LCD->DATA0 &= (uint32_t)(0xFFFF3FFF);
        LCD->DATA1 &= (uint32_t)(0xFFFF3FFF);
        LCD->DATA2 &= (uint32_t)(0xFFFF3FFF);
        LCD->DATA3 &= (uint32_t)(0xFFFF3FFF);
        break;
  }
}

/**
  * @brief  This function Clear all the LCD DATA register.
  * @param  None 
  * @retval None
  */
void LCD_Seg_ClearAll(void)
{
    LCD->DATA0 = 0x0000;
    LCD->DATA1 = 0x0000;
    LCD->DATA2 = 0x0000;
    LCD->DATA3 = 0x0000;
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
