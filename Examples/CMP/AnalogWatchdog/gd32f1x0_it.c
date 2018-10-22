/**
  ******************************************************************************
  * @file    CMP/AnalogWatchdog/gd32f1x0_it.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_it.h"
extern CheckState(void);
/* Private function prototypes -----------------------------------------------*/
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

extern uint32_t g_delayms;
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    if(g_delayms)g_delayms--;
}

/**
  * @brief  This function handles External lines 21 and 22 interrupt request.
  * @param  None
  * @retval None
  */
void ADC1_CMP_IRQHandler(void)
{
    if((EXTI_GetIntBitState(EXTI_LINE21) != RESET) || (EXTI_GetIntBitState(EXTI_LINE22) != RESET))
    {
        /* Recover the system clock to 72MHz */
        System_Init();
        CheckState();
        /* Clear EXTI line21 bit */
        EXTI_ClearIntBitState(EXTI_LINE21);
        /* Clear EXTI line22 bit */
        EXTI_ClearIntBitState(EXTI_LINE22);
    }
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
