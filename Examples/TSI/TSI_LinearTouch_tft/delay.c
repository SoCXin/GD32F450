/**
  ******************************************************************************
  * @file    TSI/TSI_LinearTouch_tft/delay.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Delay functions.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "delay.h"

/* Private variables ---------------------------------------------------------*/
/* Delay times multiplier for us */
static uint8_t fac_us=0;
/* Delay times multiplier for ms */
static uint16_t fac_ms=0;

/* Private function  ---------------------------------------------------------*/
/**
  * @brief  Initialization time delay function.
  * @param  SYSCLK:system clock.
  * @retval None
  */
void delay_init(uint8_t SYSCLK)
{
    SysTick->CTRL&=0xfffffffb;
    fac_us=SYSCLK/8;
    fac_ms=(uint16_t)fac_us*1000;
}

/**
  * @brief  Delay to ms.
  * @param  nms:specifies the delay time length.
  * @retval None
  */
void delay_ms(uint16_t nms)
{
    uint32_t temp;
    /* Load time */
    SysTick->LOAD=(uint32_t)nms*fac_ms;
    /* Clear counter */
    SysTick->VAL =0x00;
    SysTick->CTRL=0x01 ;
    do
    {
        temp=SysTick->CTRL;
    }
    while(temp&0x01&&!(temp&(1<<16)));
    /* Close the counter */
    SysTick->CTRL=0x00;
    /* Clear counter */
    SysTick->VAL =0X00;
}   

/**
  * @brief  Delay to nus.
  * @param  nus:specifies the delay time length.
  * @retval None
  */
void delay_us(uint32_t nus)
{
    uint32_t temp;
    /* Load time */
    SysTick->LOAD=nus*fac_us;
    /* Clear counter */
    SysTick->VAL=0x00;
    SysTick->CTRL=0x01;
    do
    {
        temp=SysTick->CTRL;
    }
    while(temp&0x01&&!(temp&(1<<16)));

    /* Close the counter */
    SysTick->CTRL=0x00;
    /* Clear counter */
    SysTick->VAL =0X00;
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
