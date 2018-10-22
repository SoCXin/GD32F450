/**
  ******************************************************************************
  * @file    TIMER/TIMER2_OCActive/main.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <gd32f1x0.h>
#include "gd32f1x0_eval.h"
#include "systick.h"
#include <stdio.h>

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void TIMER_Configuration(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Configure system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
    /* GPIOA, GPIOB, GPIOC and GPIOF clocks enable */
    RCC_AHBPeriphClock_Enable( RCC_AHBPERIPH_GPIOA |
                               RCC_AHBPERIPH_GPIOB |
                               RCC_AHBPERIPH_GPIOF  , ENABLE);
}

/**
  * @brief  Configure the GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
    GPIO_InitPara GPIO_InitStructure;

    /*Configure PA0(TIMER2 CH1) as alternate function*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_0;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE0,GPIO_AF_2);

    /*Configure PA1(TIMER2 CH2) as alternate function*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_1;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE1,GPIO_AF_2);

    /*Configure PA2(TIMER2 CH3) as alternate function*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE2,GPIO_AF_2);
}

/**
  * @brief  Configure the TIMER peripheral.
  * @param  None
  * @retval None
  */
void TIMER_Configuration(void)
{
    /* ---------------------------------------------------------------
    TIMER2 Configuration: 
    TIMER2CLK = SystemCoreClock / 36000 = 2K,
    And generate 3 signals with 3 different delays:
    TIMER2_CH1 delay = 4000/2000  = 2s
    TIMER2_CH2 delay = 8000/2000  = 4s
    TIMER2_CH3 delay = 12000/2000 = 6s
    --------------------------------------------------------------- */
    TIMER_BaseInitPara TIMER_TimeBaseStructure;
    TIMER_OCInitPara  TIMER_OCInitStructure;

    /* TIMERS clock enable */
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_TIMER1|RCC_APB2PERIPH_TIMER15|RCC_APB2PERIPH_TIMER16|RCC_APB2PERIPH_TIMER17,ENABLE);
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_TIMER2|RCC_APB1PERIPH_TIMER3|RCC_APB1PERIPH_TIMER6|RCC_APB1PERIPH_TIMER14,ENABLE);

    /* TIMER2  configuration */
    TIMER_DeInit(TIMER2);
    TIMER_TimeBaseStructure.TIMER_Prescaler     = 35999;
    TIMER_TimeBaseStructure.TIMER_CounterMode   = TIMER_COUNTER_UP;
    TIMER_TimeBaseStructure.TIMER_Period        = 19999;
    TIMER_TimeBaseStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
    TIMER_BaseInit(TIMER2,&TIMER_TimeBaseStructure);

    /* CH1,CH2 and CH3 Configuration in OC_ACTIVE mode */
    TIMER_OCInitStructure.TIMER_OCMode      = TIMER_OC_MODE_ACTIVE;
    TIMER_OCInitStructure.TIMER_OCPolarity  = TIMER_OC_POLARITY_HIGH;
    TIMER_OCInitStructure.TIMER_OutputState = TIMER_OUTPUT_STATE_ENABLE;

      
    TIMER_OCInitStructure.TIMER_Pulse = 3999;
    TIMER_OC1_Init(TIMER2, &TIMER_OCInitStructure);
    TIMER_OC1_Preload(TIMER2,TIMER_OC_PRELOAD_DISABLE);

    TIMER_OCInitStructure.TIMER_Pulse = 7999;
    TIMER_OC2_Init(TIMER2, &TIMER_OCInitStructure);
    TIMER_OC2_Preload(TIMER2,TIMER_OC_PRELOAD_DISABLE);

    TIMER_OCInitStructure.TIMER_Pulse = 11999;
    TIMER_OC3_Init(TIMER2, &TIMER_OCInitStructure);
    TIMER_OC3_Preload(TIMER2,TIMER_OC_PRELOAD_DISABLE);

    /* Auto-reload preload enable */
    TIMER_CARLPreloadConfig(TIMER2,ENABLE);
    /* TIMER enable counter*/
    TIMER_Enable( TIMER2, ENABLE );
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main()
{
    RCC_Configuration();
    GPIO_Configuration();
    GD_EVAL_LEDInit(LED1);
    SysTick_Configuration();
    TIMER_Configuration();

    while(1)
    {
        GD_EVAL_LEDOn(LED1);
        Delay_1ms(1000);
        GD_EVAL_LEDOff(LED1);
        Delay_1ms(1000);
    }
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
