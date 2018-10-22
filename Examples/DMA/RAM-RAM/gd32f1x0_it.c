/**
  ******************************************************************************
  * @file    gd32f1x0_it.c 
  * @author  MCU SD
  * @version  V2.0.0
  * @date    15-Jan-2016   
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_it.h"
#include "gd32f1x0_dma.h"
#include "gd32f1x0_usart.h"
#include "main.h"
#include "gd32f1x0_eval.h"

/* Private variables ---------------------------------------------------------*/
extern __IO uint32_t count;

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
 

}

/**
  * @brief  This function handles DMA1_Channel1 Handler.
  * @param  None
  * @retval None
  */
void DMA1_Channel1_IRQHandler(void)
{
  
    if(DMA_GetIntBitState(DMA1_INT_HT1))
    {     
        count++;
        printf("TxHT1 \r\n");
    
    }
    if(DMA_GetIntBitState(DMA1_INT_TC1))
    {     
        count++;
        printf("TxTC1 \r\n");
        DMA_ClearIntBitState(DMA1_INT_GL1);    
    }
}

/**
  * @brief  This function handles DMA1_Channel2_3 Handler.
  * @param  None
  * @retval None
  */
void DMA1_Channel2_3_IRQHandler(void)
{
    if(DMA_GetIntBitState(DMA1_INT_HT2))
    {     
        count++;
        printf("TxHT2 \r\n");
    }
    if(DMA_GetIntBitState(DMA1_INT_TC2))
    {     
        count++;
        printf("TxTC2 \r\n");
        DMA_ClearIntBitState(DMA1_INT_GL2);    
    }
    if(DMA_GetIntBitState(DMA1_INT_HT3))
    {     
        count++;
        printf("TxHT3 \r\n");
    }
    if(DMA_GetIntBitState(DMA1_INT_TC3))
    {     
        count++;
        printf("TxTC3 \r\n");
        DMA_ClearIntBitState(DMA1_INT_GL3);    
    }
}

/**
  * @brief  This function handles DMA1_Channel4_5 Handler.
  * @param  None
  * @retval None
  */
void DMA1_Channel4_5_IRQHandler(void)
{
    if(DMA_GetIntBitState(DMA1_INT_HT4))
    {     
        count++;
        printf("TxHT4 \r\n");
    }
    if(DMA_GetIntBitState(DMA1_INT_TC4))
    {     
        count++;
        printf("TxTC4 \r\n");
        DMA_ClearIntBitState(DMA1_INT_GL4);    
    }
    if(DMA_GetIntBitState(DMA1_INT_HT5))
    {     
        count++;
        printf("TxHT5 \r\n");
    }
    if(DMA_GetIntBitState(DMA1_INT_TC5))
    {     
        count++;
        printf("TxTC5 \r\n");
        DMA_ClearIntBitState(DMA1_INT_GL5);    
    }
}

/**
  * @brief  This function handles DMA1_Channel6_7 Handler.
  * @param  None
  * @retval None
  */
void DMA1_Channel6_7_IRQHandler(void)
{
    if(DMA_GetIntBitState(DMA1_INT_HT6))
    {     
        count++;
        printf("TxHT6 \r\n");
    }
    if(DMA_GetIntBitState(DMA1_INT_TC6))
    {     
        count++;
        printf("TxTC6 \r\n");
        DMA_ClearIntBitState(DMA1_INT_GL6);    
    }
    if(DMA_GetIntBitState(DMA1_INT_HT7))
    {     
        count++;
        printf("TxHT7 \r\n");
    }
    if(DMA_GetIntBitState(DMA1_INT_TC7))
    {     
        printf("TxTC7 \r\n");
        count++;
        DMA_ClearIntBitState(DMA1_INT_GL7);    
    }
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/



/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
