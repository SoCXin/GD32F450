/**
  ******************************************************************************
  * @file    CMP/PWMSignalControl/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "stdio.h"

/* Private function prototypes -----------------------------------------------*/

uint16_t Period = 65535;
void RCC_Config(void);
void GPIO_Config(void);
void TIMER_Config(void);
void CMP_Config(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    RCC_Config();
    GPIO_Config();
    TIMER_Config();
    CMP_Config();
    while(1)
    {}
}

/**
  * @brief  RCC configuration.
  * @param  None
  * @retval None
  */
void RCC_Config(void)
{
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOA,ENABLE);
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOB,ENABLE);
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_TIMER2,ENABLE);
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_CMP,ENABLE);
}

/**
  * @brief  GPIO configuration.
  * @param  None
  * @retval None
  */
void GPIO_Config(void)
{
    GPIO_InitPara GPIO_InitStruct;
    /* Configure PB3 PWM output */
    GPIO_ParaInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_PIN_3 ;
    GPIO_InitStruct.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStruct.GPIO_Speed =  GPIO_SPEED_50MHZ;
    GPIO_InitStruct.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStruct.GPIO_PuPd  =GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE3,GPIO_AF_2);
    /* Configure PA1 comparator input */
    GPIO_ParaInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_PIN_1 ;
    GPIO_InitStruct.GPIO_Mode  = GPIO_MODE_AN;
    GPIO_InitStruct.GPIO_Speed =  GPIO_SPEED_50MHZ;
    GPIO_InitStruct.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStruct.GPIO_PuPd  =GPIO_PUPD_PULLUP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief  TIMER2 configuration.
  * @param  None
  * @retval None
  */
void TIMER_Config(void)
{
    TIMER_OCInitPara TIMER_OCInitStruct;
    TIMER_BaseInitPara TIMER_TimeBaseStruct;
    /* Timer base configure */
    TIMER_BaseStructInit(&TIMER_TimeBaseStruct);
    TIMER_TimeBaseStruct.TIMER_Prescaler = 71;
    TIMER_TimeBaseStruct.TIMER_CounterMode = TIMER_COUNTER_UP;
    TIMER_TimeBaseStruct.TIMER_Period = 65535;
    TIMER_TimeBaseStruct.TIMER_ClockDivision = TIMER_CDIV_DIV1 ;
    TIMER_BaseInit(TIMER2, &TIMER_TimeBaseStruct);
    /* PWM1 Mode configure Channel2 in PWM1 mode */
    TIMER_OCStructInit(&TIMER_OCInitStruct);
    TIMER_OCInitStruct.TIMER_OCMode = TIMER_OC_MODE_PWM1 ;
    TIMER_OCInitStruct.TIMER_OutputState = TIMER_OUTPUT_STATE_ENABLE ;
    TIMER_OCInitStruct.TIMER_Pulse = (Period/2) + 1 ;
    TIMER_OC2_Init(TIMER2, &TIMER_OCInitStruct);
    /* Select OCREFCLR as source for clearing OC2REF */
    TIMER_SelectOCRefClear(TIMER2,TIMER_OCREF_CLEAR_OCREFCLR)  ;
    TIMER_OC2_RefClear(TIMER2,TIMER_OC_CLEAR_ENABLE);
    /* Enable TIMER2 counter */
    TIMER_Enable(TIMER2,ENABLE);
}

/**
  * @brief  Comparator configuration.
  * @param  None
  * @retval None
  */
void CMP_Config(void)
{
    CMP_InitPara CMP_InitStruct;
    /* Configure comparator 1 output timer2 OCREF signal*/
    CMP_ParaInit(&CMP_InitStruct);
    CMP_InitStruct.CMP_InvertingInput = CMP_INVERTINGINPUT_VREFINT;
    CMP_InitStruct.CMP_Output = CMP_OUTPUT_TIM2OCREFCLR;
    CMP_InitStruct.CMP_OutputPolarity = CMP_OUTPUTPOLARITY_NONINVERTED;
    CMP_InitStruct.CMP_Hysteresis = CMP_HYSTERESIS_NO;
    CMP_InitStruct.CMP_OperatingMode = CMP_OPERATINGMODE_HIGHSPEED;
    CMP_Init(CMP_CHANNEL_CMP1, &CMP_InitStruct);

    /* Enable CMP1 */
    CMP_Enable(CMP_CHANNEL_CMP1, ENABLE);
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
