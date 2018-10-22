/**
  ******************************************************************************
  * @file    CMP/T2IC4/main.c 
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
void TIMER_Configuration(void);


/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    CMP_InitPara CMP_InitParaStruct;
    /* Configurate RCC */
    RCC_Configuration();
    /* Configurate GPIOA*/
    GPIO_Configuration();
    GD_EVAL_LEDInit(LED3);
    /* Initialize CMP1 */
    CMP_ParaInit(&CMP_InitParaStruct);
    CMP_InitParaStruct.CMP_InvertingInput = CMP_INVERTINGINPUT_1_4VREFINT;
    CMP_InitParaStruct.CMP_Output = CMP_OUTPUT_TIM2IC4;
    CMP_InitParaStruct.CMP_OutputPolarity = CMP_OUTPUTPOLARITY_NONINVERTED;
    CMP_InitParaStruct.CMP_Hysteresis = CMP_HYSTERESIS_NO;
    CMP_InitParaStruct.CMP_OperatingMode = CMP_OPERATINGMODE_VERYLOWSPEED;
    CMP_Init(CMP_CHANNEL_CMP1, &CMP_InitParaStruct);

    /* Configurate Timer2 and Interrupt */
    TIMER_Configuration();

    /* Enable CMP1 and Trigger Timer2 input capture */
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
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOB,ENABLE);
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOC,ENABLE);
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_TIMER2,ENABLE);
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

    GPIO_InitStruct.GPIO_Pin   = GPIO_PIN_11;
    GPIO_InitStruct.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStruct.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PUPD_PULLUP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE11,GPIO_AF_2);
}
/**
  * @brief  Timer2 configuration.
  * @param  None
  * @retval None
  */
void TIMER_Configuration(void)
{
    TIMER_BaseInitPara TIMER_TimeBaseStruct;
    NVIC_InitPara NVIC_InitStruct;

    TIMER_BaseStructInit(&TIMER_TimeBaseStruct);
    TIMER_TimeBaseStruct.TIMER_Prescaler = 0;
    TIMER_TimeBaseStruct.TIMER_CounterMode = TIMER_COUNTER_UP;
    TIMER_TimeBaseStruct.TIMER_Period = 65535;
    TIMER_TimeBaseStruct.TIMER_ClockDivision = TIMER_CDIV_DIV1 ;
    TIMER_BaseInit(TIMER2, &TIMER_TimeBaseStruct);
    TIMER_ClearBitState(TIMER2,TIMER_FLAG_UPDATE );

   
    NVIC_InitStruct.NVIC_IRQ = TIMER2_IRQn;
    NVIC_InitStruct.NVIC_IRQPreemptPriority = 1;
    NVIC_InitStruct.NVIC_IRQSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIMER2->STR = 0;

    TIMER_INTConfig(TIMER2, TIMER_INT_CH4, ENABLE);

    TIMER_Enable(TIMER2, ENABLE);
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
