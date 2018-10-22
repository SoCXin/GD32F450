/**
  ******************************************************************************
  * @file    TIMER/TIMERs_CascadeSynchro/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program.
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
  * @brief  Configures system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
    /* GPIOA, GPIOB, GPI                              OC and GPIOF clocks enable */
    RCC_AHBPeriphClock_Enable( RCC_AHBPERIPH_GPIOA |
                               RCC_AHBPERIPH_GPIOB |
                               RCC_AHBPERIPH_GPIOC |
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

    /*Configure PA6(TIMER3 CH1) as alternate function*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_6;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE6,GPIO_AF_1);

    /*Configure PB3(TIMER2 CH2) as alternate function*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_3;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE3,GPIO_AF_2);

    /*Configure PA8(TIMER1 CH1) as alternate function*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_8;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE8,GPIO_AF_2);
}

/**
  * @brief  Configure the TIMER peripheral.
  * @param  None
  * @retval None
  */
void TIMER_Configuration(void)
{
    /* Timers synchronisation in cascade mode ----------------------------
       1/TIMER2 is configured as Master Timer:
       - PWM Mode is used
       - The TIMER2 Update event is used as Trigger Output  

       2/TIMER3 is slave for TIMER2 and Master for TIMER1,
       - PWM Mode is used
       - The ITR1(TIMER2) is used as input trigger 
       - External clock mode is used,the counter counts on the rising edges of
       the selected trigger.
       - The TIMER3 Update event is used as Trigger Output. 

       3/TIMER1 is slave for TIMER3,
       - PWM Mode is used
       - The ITR2(TIMER3) is used as input trigger
       - External clock mode is used,the counter counts on the rising edges of
       the selected trigger.
      -------------------------------------------------------------------- */
    TIMER_BaseInitPara TIMER_TimeBaseStructure;
    TIMER_OCInitPara  TIMER_OCInitStructure;

    /* TIMERS clock enable */
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_TIMER1|RCC_APB2PERIPH_TIMER15|RCC_APB2PERIPH_TIMER16|RCC_APB2PERIPH_TIMER17,ENABLE);
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_TIMER2|RCC_APB1PERIPH_TIMER3|RCC_APB1PERIPH_TIMER6|RCC_APB1PERIPH_TIMER14,ENABLE);

    /* TIMER2  configuration */
    TIMER_DeInit(TIMER2);
    TIMER_TimeBaseStructure.TIMER_Prescaler     = 35999;
    TIMER_TimeBaseStructure.TIMER_CounterMode   = TIMER_COUNTER_UP;
    TIMER_TimeBaseStructure.TIMER_Period        = 3999;
    TIMER_TimeBaseStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
    TIMER_BaseInit(TIMER2,&TIMER_TimeBaseStructure);

    TIMER_OCInitStructure.TIMER_OCMode      = TIMER_OC_MODE_PWM1;
    TIMER_OCInitStructure.TIMER_OCPolarity  = TIMER_OC_POLARITY_HIGH;
    TIMER_OCInitStructure.TIMER_OutputState = TIMER_OUTPUT_STATE_ENABLE;
    TIMER_OCInitStructure.TIMER_OCIdleState = TIMER_OC_IDLE_STATE_RESET;
      
    TIMER_OCInitStructure.TIMER_Pulse = 1999;
    TIMER_OC2_Init(TIMER2, &TIMER_OCInitStructure);
    TIMER_OC2_Preload(TIMER2,TIMER_OC_PRELOAD_DISABLE);

    /* Auto-reload preload enable */
    TIMER_CARLPreloadConfig(TIMER2,ENABLE);
    /* Select the Master Slave Mode */
    TIMER_SelectMasterSlaveMode(TIMER2,TIMER_MASTER_SLAVE_MODE_ENABLE);
    /* TIMER2 Update event is used as Trigger Output */
    TIMER_SelectOutputTrigger(TIMER2,TIMER_TRI_OUT_SRC_UPDATE);

    /* TIMER3  configuration */
    TIMER_DeInit(TIMER3);
    TIMER_TimeBaseStructure.TIMER_Prescaler     = 0;
    TIMER_TimeBaseStructure.TIMER_CounterMode   = TIMER_COUNTER_UP;
    TIMER_TimeBaseStructure.TIMER_Period        = 1;
    TIMER_TimeBaseStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
    TIMER_BaseInit(TIMER3,&TIMER_TimeBaseStructure);

    TIMER_OCInitStructure.TIMER_OCMode      = TIMER_OC_MODE_PWM1;
    TIMER_OCInitStructure.TIMER_OCPolarity  = TIMER_OC_POLARITY_HIGH;
    TIMER_OCInitStructure.TIMER_OutputState = TIMER_OUTPUT_STATE_ENABLE;
    TIMER_OCInitStructure.TIMER_OCIdleState = TIMER_OC_IDLE_STATE_RESET;
      
    TIMER_OCInitStructure.TIMER_Pulse = 1;
    TIMER_OC1_Init(TIMER3, &TIMER_OCInitStructure);
    TIMER_OC1_Preload(TIMER3,TIMER_OC_PRELOAD_DISABLE);
    /* Auto-reload preload enable */
    TIMER_CARLPreloadConfig(TIMER3,ENABLE);
    /* Slave Mode selection: TIMER3 */
    TIMER_SelectSlaveMode(TIMER3,TIMER_SLAVE_MODE_EXTERNAL1);
    TIMER_SelectInputTrigger(TIMER3,TIMER_TS_ITR1);
    /* Select the Master Slave Mode */
    TIMER_SelectMasterSlaveMode(TIMER3,TIMER_MASTER_SLAVE_MODE_ENABLE);
    /* TIMER3 Update event is used as Trigger Output */
    TIMER_SelectOutputTrigger(TIMER3,TIMER_TRI_OUT_SRC_UPDATE);

    /* TIMER1  configuration */
    TIMER_DeInit(TIMER1);
    TIMER_TimeBaseStructure.TIMER_Prescaler     = 0;
    TIMER_TimeBaseStructure.TIMER_CounterMode   = TIMER_COUNTER_UP;
    TIMER_TimeBaseStructure.TIMER_Period        = 1;
    TIMER_TimeBaseStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
    TIMER_BaseInit(TIMER1,&TIMER_TimeBaseStructure);

    TIMER_OCInitStructure.TIMER_OCMode      = TIMER_OC_MODE_PWM1;
    TIMER_OCInitStructure.TIMER_OCPolarity  = TIMER_OC_POLARITY_HIGH;
    TIMER_OCInitStructure.TIMER_OutputState = TIMER_OUTPUT_STATE_ENABLE;
    TIMER_OCInitStructure.TIMER_OCIdleState = TIMER_OC_IDLE_STATE_RESET;
      
    TIMER_OCInitStructure.TIMER_Pulse = 1;
    TIMER_OC1_Init(TIMER1, &TIMER_OCInitStructure);
    TIMER_OC1_Preload(TIMER1,TIMER_OC_PRELOAD_DISABLE);
    /* Auto-reload preload enable */
    TIMER_CARLPreloadConfig(TIMER1,ENABLE);
    /* TIMER1 output enable */
    TIMER_CtrlPWMOutputs(TIMER1, ENABLE);
    /* Slave Mode selection: TIMER1 */
    TIMER_SelectSlaveMode(TIMER1,TIMER_SLAVE_MODE_EXTERNAL1);
    TIMER_SelectInputTrigger(TIMER1,TIMER_TS_ITR2);

    /* TIMER enable counter*/
    TIMER_Enable( TIMER2, ENABLE );
    TIMER_Enable( TIMER3, ENABLE );
    TIMER_Enable( TIMER1, ENABLE );
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
