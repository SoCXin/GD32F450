/**
  ******************************************************************************
  * @file    ADC/Multi_channel_regular/gd32f1x0_it.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_it.h"
#include "gd32f1x0_eval.h"

/* Private define ------------------------------------------------------------*/
#define ADC_DRH          0x0B66   /*!< equal to 2.35V(150R) or 3.56V(190R)    */
#define ADC_DRL          0x08BA   /*!< equal to 1.80V(150R) or 2.72V(190R)    */

/* Private variables ---------------------------------------------------------*/
extern int AD_I;
extern uint16_t ADC_ConvertedValue[4]; 
extern uint16_t ADC_Channel1[256];
extern uint16_t ADC_Channel2[256]; 
extern uint16_t ADC_Channel3[256]; 
extern uint16_t ADC_Channel4[256];

/* Private function prototypes -----------------------------------------------*/
extern void Delay_ARMJISHU(__IO uint32_t nCount);

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
  * @brief  This function handles DMA transfer complete interrupts requests.
  * @param  None
  * @retval None
  */
void DMA1_Channel1_IRQHandler(void)
{
    /* Clear DMA1 TC1 pending interrupt bit */
    DMA_ClearIntBitState(DMA1_INT_TC1);
    
    ADC_Channel1[AD_I]=ADC_ConvertedValue[0];  
    ADC_Channel2[AD_I]=ADC_ConvertedValue[1];
    ADC_Channel3[AD_I]=ADC_ConvertedValue[2];
    ADC_Channel4[AD_I]=ADC_ConvertedValue[3];
    
    if(ADC_DRH<ADC_Channel1[AD_I]||ADC_Channel1[AD_I]<ADC_DRL)
    {
        GD_EVAL_LEDOn(LED2);
        GD_EVAL_LEDOff(LED3);  
    }
    else
    {
        GD_EVAL_LEDOn(LED3);
        GD_EVAL_LEDOff(LED2);
    }
    
    AD_I++;

    if (AD_I<256)
    ADC_SoftwareStartConv_Enable(ENABLE);
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
