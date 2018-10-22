/**
  ******************************************************************************
  * @file    CAN/LoopBack/gd32f1x0_it.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_it.h"

/* Private variables ---------------------------------------------------------*/
extern __IO uint32_t ret;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   This function handles NMI exception.
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
  * @brief  This function handles CAN1 Handler.
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler(void)
{
    CanRxMessage RxMessage;
    /* Initial RxMessage */
    RxMessage.StdId=0x00;
    RxMessage.ExtId=0x00;
    RxMessage.FF=0;
    RxMessage.DLC=0;
    RxMessage.FI=0;
    RxMessage.Data[0]=0x00;
    RxMessage.Data[1]=0x00;

    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
    /* Check RxMessage */
    if((RxMessage.ExtId==0x1234) && (RxMessage.FF==CAN_FF_EXTENDED)
        && (RxMessage.DLC==2) && ((RxMessage.Data[1]|RxMessage.Data[0]<<8)==0xDECA))
    {
        ret = 1; 
    }
    else
    {
        ret = 0; 
    }
}

/**
  * @brief  This function handles CAN2 Handler.
  * @param  None
  * @retval None
  */
void CAN2_RX0_IRQHandler(void)
{
    CanRxMessage RxMessage;
    /* Initial RxMessage */
    RxMessage.StdId=0x00;
    RxMessage.ExtId=0x00;
    RxMessage.FF=0;
    RxMessage.DLC=0;
    RxMessage.FI=0;
    RxMessage.Data[0]=0x00;
    RxMessage.Data[1]=0x00;
    /* Check RxMessage */
    CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);

    if((RxMessage.ExtId==0x1234) && (RxMessage.FF==CAN_FF_EXTENDED)
        && (RxMessage.DLC==2) && ((RxMessage.Data[1]|RxMessage.Data[0]<<8)==0xDECA))
    {
        ret = 1; 
    }
    else
    {
        ret = 0; 
    }
}


/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
