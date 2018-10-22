/**
  ******************************************************************************
  * @file    TIMER/TIMER1_DMA/systick.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   The SysTick configuration file.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "SysTick.h"

/* Private variable ------------------------------------------------------- */
static __IO uint32_t TimingDelay;

/* Private functions ---------------------------------------------------------*/
/**
* @brief  Configure a SysTick Base time to 1ms.
* @param  None
* @retval None
*/
void SysTick_Configuration(void)
{
    /* Setup SysTick Timer for 1ms interrupts  */
    if (SysTick_Config(SystemCoreClock / 1000))
    { 
        /* Capture error */ 
        while (1);
    }
    /* Configure the SysTick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00);
}

/**
* @brief  Insert a delay time.
* @param  nTime: specifies the delay time length, in milliseconds.
* @retval None
*/
void Delay_1ms(uint32_t nTime)
{  
    TimingDelay = nTime;
    while(TimingDelay != 0);
}

/**
* @brief  Decrement the TimingDelay variable.
* @param  None.
* @retval None
*/
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00)
    { 
        TimingDelay--;
    }
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
