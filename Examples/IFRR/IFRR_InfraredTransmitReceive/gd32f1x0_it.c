/**
  ******************************************************************************
  * @file    IFRR/IFRR_InfraredTransmitReceive/gd32f1x0_it.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_it.h"
#include "rc5_encode.h"
#include "rc5_decode.h"
#include "ir_decode.h"

/* Private variables ---------------------------------------------------------*/
uint32_t ICValue1 = 0;
uint32_t ICValue2 = 0;

/* Exported variables --------------------------------------------------------*/
extern uint32_t RC5_FrameManchestarFormat;
extern __IO tRC5_packet   RC5TmpPacket;

/* Private function prototypes -----------------------------------------------*/
void TimingDelay_Decrement(void);

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
  * @brief  This function handles TIM17 interrupt request.
  * @param  None
  * @retval None
  */
void TIMER16_IRQHandler(void)
{

    RC5_Encode_SignalGenerate(RC5_FrameManchestarFormat);
    
    /* Clear TIMER16 update interrupt */
    TIMER_ClearIntBitState( TIMER16, TIMER_INT_UPDATE );
}

/**
  * @brief  This function handles TIMER3 overflow and update interrupt request.
  * @param  None
  * @retval None
  */
void TIMER3_IRQHandler(void)
{
 #if defined(GD32F130_150)
    /* IC2 Interrupt*/
    if((TIMER_GetBitState(IR_TIM, TIMER_FLAG_CH2 ) != RESET))
    {
        TIMER_ClearBitState(IR_TIM, TIMER_FLAG_CH2 );

        /* Get the Input Capture value */
        ICValue2 = IR_TIM->CHCC2; 

        /* RC5 */
        RC5_DataSampling(ICValue2, 1);
    }  

    /* IC1 Interrupt */   
    if((TIMER_GetBitState(IR_TIM, TIMER_FLAG_CH1) != RESET))
    {
        TIMER_ClearBitState(IR_TIM, TIMER_FLAG_CH1);
  
        /* Get the Input Capture value */
        ICValue1 = IR_TIM->CHCC1 - IR_TIM->CHCC2;
  
        RC5_DataSampling(ICValue1, 0);
    }

#elif defined(GD32F170_190)
    /* IC1 Interrupt*/
    if((TIMER_GetBitState(IR_TIM, TIMER_FLAG_CH1 ) != RESET))
    {
        TIMER_ClearBitState(IR_TIM, TIMER_FLAG_CH1 );

        /* Get the Input Capture value */
        ICValue1 = IR_TIM->CHCC1;

        /* RC5 */
        RC5_DataSampling(ICValue1, 1);
    }

    /* IC2 Interrupt */   
    if((TIMER_GetBitState(IR_TIM, TIMER_FLAG_CH2) != RESET))
    {
        TIMER_ClearBitState(IR_TIM, TIMER_FLAG_CH2);
  
        /* Get the Input Capture value */
        ICValue2 = IR_TIM->CHCC2 - IR_TIM->CHCC1;
  
        RC5_DataSampling(ICValue2, 0);
    }

#endif
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
