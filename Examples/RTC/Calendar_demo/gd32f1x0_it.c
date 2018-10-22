/**
  ******************************************************************************
  * @file    RTC/Calendar/gd32f1x0_it.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_it.h"
#include "main.h"
#include "gd32f1x0_eval.h"

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}


/**
  * @brief  This function handles RTC interrupt request Handler.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
    if(RTC_GetBitState(RTC_STR_AF) != RESET)
    {
        LED1_GPIO_PORT->DOR ^= LED1_PIN ;
        RTC_ClearBitState(RTC_STR_AF);
        EXTI_ClearIntBitState(EXTI_LINE17);
    } 
}

/************************ (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
