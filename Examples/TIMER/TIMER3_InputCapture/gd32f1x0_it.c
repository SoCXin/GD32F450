/**
  ******************************************************************************
  * @file    TIMER/TIMER3_InputCapture/gd32f1x0_it.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_it.h"
#include "systick.h"
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/
__IO uint16_t readvalue1 = 0, readvalue2 = 0;
__IO uint16_t ccnumber = 0;
__IO uint32_t count = 0;
__IO float fre = 0;

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

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    TimingDelay_Decrement();
}

/**
  * @brief  This function handles TIMER3 interrupt request.
  * @param  None
  * @retval None
  */
void TIMER3_IRQHandler()
{
    if(TIMER_GetIntBitState(TIMER3,TIMER_INT_CH1)==ENABLE)
    {
        TIMER_ClearIntBitState(TIMER3,TIMER_INT_CH1);
        if(ccnumber == 0)
        {
            readvalue1 = TIMER_GetCapture1(TIMER3);
            ccnumber = 1;
        }
        else if(ccnumber == 1)
        {
            readvalue2 = TIMER_GetCapture1(TIMER3); 
            if (readvalue2 > readvalue1)
            {
                count = (readvalue2 - readvalue1); 
            }
            else
            {
                count = ((0xFFFF - readvalue1) + readvalue2); 
            } 
            fre = (float) 2000 / count;
            printf("the value1 is %d,the value2 is %d\n",readvalue1,readvalue2);
            printf("the count is %d\n",count);
            printf("the frequence is %f\n",fre);
            ccnumber = 0;
        }

    }
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
