/**
  ******************************************************************************
  * @file    CEC/CEC_host-slave communication/gd32f1x0_it.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_it.h"

/* Private variables ---------------------------------------------------------*/
/* Data transmission array1*/
uint8_t TransmitBuffer1[2] = {0xA5,0x5A};

/* Data transmission array2*/
uint8_t TransmitBuffer2[2] = {0x5A,0xA5};

/* Receive data buffer array */
uint8_t ReceiveBuffer[10];

/* Sent data pointer */
uint8_t *SendBuffer;

__IO uint8_t ReceivedFrame = 0;

uint8_t send_inc = 0, rcv_inc = 0;

uint16_t TransErrorCode = 0;
uint16_t RecepErrorCode = 0;
uint8_t ByteNumber = 0;

extern void LED_Spark(void);

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

}

/**
  * @brief  This function handles EXTI4_15 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{
    /* Get the button interrupt bit state */
    if(EXTI_GetIntBitState(EXTI_LINE7) != RESET)
    {
        ByteNumber = 2;
        CEC_SendData(0x12);
        SendBuffer = TransmitBuffer1;
        CEC_StartOfMessage();
        EXTI_ClearIntBitState(EXTI_LINE7);
    }
    /* Get the button interrupt bit state */
    else if(EXTI_GetIntBitState(EXTI_LINE13) != RESET)
    {
        ByteNumber = 2;
        CEC_SendData(0x12);
        SendBuffer = TransmitBuffer2;
        CEC_StartOfMessage();
        EXTI_ClearIntBitState(EXTI_LINE13);
    }
}


/**
  * @brief  This function handles CEC interrupt request.
  * @param  None
  * @retval None
  */
void CEC_IRQHandler(void)
{
    /********************** Reception *******************************************/
    /* Check if a reception error occured */
    if(CEC->ISTR & (CEC_INT_RAE| CEC_INT_RLBPE| CEC_INT_RSBPE| CEC_INT_RBRE| CEC_INT_RO))
    {
        RecepErrorCode = CEC->ISTR;
        CEC->ISTR      = (CEC_INT_RAE| CEC_INT_RLBPE| CEC_INT_RSBPE| CEC_INT_RBRE| CEC_INT_RO);
        ReceivedFrame  = 2;
    }

    if(CEC_GetIntBitState(CEC_INT_RBR))
    { 
        /* Check if the byte received is a Header */
        ReceiveBuffer[rcv_inc] = CEC_ReceiveData();
        rcv_inc++;
        /* Clear all reception flags */
        CEC_ClearIntBitState(CEC_INT_RBR);
    }

    /* Check if the byte received is the last one of the message */
    if(CEC_GetIntBitState(CEC_INT_REND))
    {  
        ReceivedFrame = 1;
        rcv_inc = 0;
        /* Clear all reception flags */
        CEC_ClearIntBitState(CEC_INT_REND);
    }

    /********************** Transmission ****************************************/
    /* Check if a transmission error occurred */
    if(CEC->ISTR& (CEC_INT_TAE| CEC_INT_TE| CEC_INT_TU| CEC_INT_LSTARB))
    {
        TransErrorCode = CEC->ISTR;
        CEC->ISTR      = (CEC_INT_TAE| CEC_INT_TE| CEC_INT_TU| CEC_INT_LSTARB);
    }
  
    /* Check if end of message bit is set in the data to be transmitted */
    if(CEC_GetIntBitState(CEC_INT_TEND))
    {
        CEC_ClearIntBitState(CEC_INT_TEND| CEC_INT_TBR);
        send_inc = 0;
    }
    /* Check if data byte has been sent */
    else if(CEC_GetIntBitState(CEC_INT_TBR))
    {
        /* Set EOM bit if the byte to be transmitted is the last one of the Transmit Buffer */
        if(send_inc++ == (ByteNumber - 1))
        {
            CEC_EndOfMessage();
            CEC_SendData(*SendBuffer++);
        }
        else
        {
            /* Put the byte in the TX Buffer */
            CEC_SendData(*SendBuffer++);
        }
        CEC_ClearIntBitState(CEC_INT_TBR);
    }
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/

