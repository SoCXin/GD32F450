/**
  ******************************************************************************
  * @file    LCD/LCD_demo/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <gd32f1x0.h>
#include "systick.h"
#include "lcd_seg.h"

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void TIMER_Configuration(void);
void DMA_Configuration(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Configure system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
    /* GPIOA, GPIOB, GPIOC and GPIOF clocks enable */
    RCC_AHBPeriphClock_Enable( RCC_AHBPERIPH_GPIOA |
                               RCC_AHBPERIPH_GPIOB |
                               RCC_AHBPERIPH_GPIOC |
                               RCC_AHBPERIPH_GPIOF  , ENABLE);

    /* PWR clocks enable */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_PWR, ENABLE);

    /* Enable LSI clocks and select LSI as the clock for LCD */
    PWR_BackupAccess_Enable(ENABLE);
    RCC_LSI_Enable(ENABLE);
    while(RCC_GetBitState(RCC_FLAG_LSISTB) == RESET)
    {
    }
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main()
{
    uint16_t i = 0;
    /* Configure system clocks */
    RCC_Configuration();

    /* Configure a SysTick Base time to 1ms */
    SysTick_Configuration();

    /* Init the LCD */
    LCD_Seg_Init();

    while(1)
    {
        LCD_Seg_DisplayNumber(i++);
        Delay_1ms(1000);
    }
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
