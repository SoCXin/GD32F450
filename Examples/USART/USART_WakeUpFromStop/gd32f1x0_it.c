/**
  ******************************************************************************
  * @file    USART/USART_WakeUpFromStop/gd32f1x0_it.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_it.h"
#include "main.h"

/* Private variables ---------------------------------------------------------*/
__IO uint8_t InterruptCounter = 0x00;
__IO uint8_t Counter = 0;

/* Private fuctions ----------------------------------------------------------*/
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
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
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
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
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
    if (Counter == 20)
    {
        LED_Spark();
        /* Reset Counter */
        Counter = 0;
    }
    else
    {
        /* increment Counter */
        Counter++; 
    }
}

/**
  * @brief  This function handles USART wakeup from deepsleep interrupt request .
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
    if (USART_GetIntBitState(USART1, USART_INT_WU) == SET)
    { 
        /* Clear The USART WU flag */  
        USART_ClearIntBitState(USART1, USART_INT_WU);
        InterruptCounter = 0x01;
    }
}
/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
