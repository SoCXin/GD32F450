/**
  ******************************************************************************
  * @file    TIMER/TIMER1_6-steps/systick.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   The SysTick configuration file.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "SysTick.h"

/* Private variable ----------------------------------------------------------*/
static __IO uint32_t TimingDelay;

/* Private functions ------------------------------------------------------- */
/**
* @brief  Configure a SysTick Base time to 0.1s.
* @param  None
* @retval None
*/
void SysTick_Configuration(void)
{
    /* Setup SysTick Timer for 100ms interrupts  */
    if (SysTick_Config(SystemCoreClock / 10))
    { 
        /* Capture error */ 
        while (1);
    }
    /* Configure the SysTick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00);
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
