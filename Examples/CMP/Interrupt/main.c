/**
  ******************************************************************************
  * @file    CMP/Interrput/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "stdio.h"
#include "gd32f1x0_eval.h"

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    CMP_InitPara CMP_InitParaStruct;
    EXTI_InitPara EXTI_InitStruct;
    NVIC_InitPara NVIC_InitStruct;
    GD_EVAL_LEDInit(LED2);
    /* configure RCC */
    RCC_Configuration();
    /* configure GPIO */
    GPIO_Configuration();
    /* Initial CMP */
    CMP_ParaInit(&CMP_InitParaStruct);
    CMP_InitParaStruct.CMP_InvertingInput = CMP_INVERTINGINPUT_1_4VREFINT;
    CMP_InitParaStruct.CMP_Output = CMP_OUTPUT_NONE;
    CMP_InitParaStruct.CMP_OutputPolarity = CMP_OUTPUTPOLARITY_NONINVERTED;
    CMP_InitParaStruct.CMP_Hysteresis = CMP_HYSTERESIS_NO;
    CMP_InitParaStruct.CMP_OperatingMode = CMP_OPERATINGMODE_VERYLOWSPEED;
    CMP_Init(CMP_CHANNEL_CMP1, &CMP_InitParaStruct);

     /* NVIC CMP1 output intertupt handle */
    NVIC_InitStruct.NVIC_IRQ = ADC1_CMP_IRQn;
    NVIC_InitStruct.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStruct. NVIC_IRQSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStruct);  
    
    /* EXTI trigger source CMP1 output configuration */
    EXTI_InitStruct.EXTI_LINE = EXTI_LINE21 ;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStruct.EXTI_LINEEnable = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    /* Enable CMP1 and enter interrupt */
    CMP_Enable(CMP_CHANNEL_CMP1, ENABLE);

    while(1);
}

/**
  * @brief  RCC configuration.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOA,ENABLE);
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_CMP ,ENABLE);
}

/**
  * @brief  GPIO configuration.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
    GPIO_InitPara GPIO_InitStruct;

    GPIO_ParaInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_PIN_1 ;
    GPIO_InitStruct.GPIO_Mode  = GPIO_MODE_AN;
    GPIO_InitStruct.GPIO_Speed =  GPIO_SPEED_50MHZ;
    GPIO_InitStruct.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStruct.GPIO_PuPd  =GPIO_PUPD_PULLUP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
