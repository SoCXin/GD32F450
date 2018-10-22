/**        
  ******************************************************************************
  * @file    TIMER/TIMER1_6-steps/main.c
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
void NVIC_Configuration(void);

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

    /*Configure PA8~PA10(TIMER1 CH1 CH2 CH3) as alternate function*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE8,GPIO_AF_2);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE9,GPIO_AF_2);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE10,GPIO_AF_2);

    /*Configure PA6(TIMER1 BKIN) as alternate function*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_6;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE6,GPIO_AF_2);

    /*Configure PB13~PB15(TIMER1 CH1N CH2N CH3N) as alternate function*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE13,GPIO_AF_2);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE14,GPIO_AF_2);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE15,GPIO_AF_2);
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
    NVIC_InitPara NVIC_InitStructure;

    NVIC_PRIGroup_Enable(NVIC_PRIGROUP_1);

    /* Enable the TIMER3 Interrupt */
    NVIC_InitStructure.NVIC_IRQ                = TIMER1_BRK_UP_TRG_COM_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority     = 1;
    NVIC_InitStructure.NVIC_IRQEnable          = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configure the TIMER peripheral.
  * @param  None
  * @retval None
  */
void TIMER_Configuration(void)
{        
    /* -----------------------------------------------------------------------
    TIMER1 Configuration:
    Generate 3 complementary PWM signal.
    TIMER1CLK is fixed to SystemCoreClock, the TIMER1 Prescaler is equal to 72 
    so the TIMER1 counter clock used is 1MHz.
    Insert a dead time equal to 72/SystemCoreClock =1us 
    Configure the break feature, active at High level, and using the automatic
    output enable feature.
    Use the Locking parameters level0.
    ----------------------------------------------------------------------- */
    TIMER_BaseInitPara TIMER_TimeBaseStructure;
    TIMER_OCInitPara   TIMER_OCInitStructure;
    TIMER_BKDTInitPara TIMER_BKDTInitStructure;

    /* TIMERS clock enable */
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_TIMER1|RCC_APB2PERIPH_TIMER15|RCC_APB2PERIPH_TIMER16|RCC_APB2PERIPH_TIMER17,ENABLE);
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_TIMER2|RCC_APB1PERIPH_TIMER3|RCC_APB1PERIPH_TIMER6|RCC_APB1PERIPH_TIMER14,ENABLE);

    /* TIMER1  configuration */
    TIMER_DeInit(TIMER1);
    TIMER_TimeBaseStructure.TIMER_Prescaler     = 71;
    TIMER_TimeBaseStructure.TIMER_CounterMode   = TIMER_COUNTER_UP;
    TIMER_TimeBaseStructure.TIMER_Period        = 600;
    TIMER_TimeBaseStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
    TIMER_BaseInit(TIMER1,&TIMER_TimeBaseStructure);

    /* CH1 Configuration in PWM mode */
    TIMER_OCInitStructure.TIMER_OCMode       = TIMER_OC_MODE_TIMING;
    TIMER_OCInitStructure.TIMER_OCPolarity   = TIMER_OC_POLARITY_HIGH;
    TIMER_OCInitStructure.TIMER_OCNPolarity  = TIMER_OCN_POLARITY_HIGH;
    TIMER_OCInitStructure.TIMER_OutputState  = TIMER_OUTPUT_STATE_ENABLE;
    TIMER_OCInitStructure.TIMER_OutputNState = TIMER_OUTPUTN_STATE_ENABLE;
    TIMER_OCInitStructure.TIMER_OCIdleState  = TIMER_OC_IDLE_STATE_SET;
    TIMER_OCInitStructure.TIMER_OCNIdleState = TIMER_OCN_IDLE_STATE_SET;

    TIMER_OCInitStructure.TIMER_Pulse = 300;
    TIMER_OC1_Init(TIMER1, &TIMER_OCInitStructure);
    TIMER_OC1_Preload(TIMER1,TIMER_OC_PRELOAD_ENABLE);

    TIMER_OCInitStructure.TIMER_Pulse = 300;
    TIMER_OC2_Init(TIMER1, &TIMER_OCInitStructure);
    TIMER_OC2_Preload(TIMER1,TIMER_OC_PRELOAD_ENABLE);

    TIMER_OCInitStructure.TIMER_Pulse = 300;
    TIMER_OC3_Init(TIMER1, &TIMER_OCInitStructure);
    TIMER_OC3_Preload(TIMER1,TIMER_OC_PRELOAD_ENABLE);

    /* Automatic Output enable, Break, dead time and lock configuration*/
    TIMER_BKDTInitStructure.TIMER_ROSState      = TIMER_ROS_STATE_ENABLE;
    TIMER_BKDTInitStructure.TIMER_IOSState      = TIMER_IOS_STATE_ENABLE;
    TIMER_BKDTInitStructure.TIMER_LOCKLevel     = TIMER_LOCK_LEVEL_OFF;
    TIMER_BKDTInitStructure.TIMER_DeadTime      = 72;
    TIMER_BKDTInitStructure.TIMER_Break         = TIMER_BREAK_ENABLE;
    TIMER_BKDTInitStructure.TIMER_BreakPolarity = TIMER_BREAK_POLARITY_LOW;
    TIMER_BKDTInitStructure.TIMER_OutAuto       = TIMER_OUTAUTO_ENABLE;
    TIMER_BKDTConfig(TIMER1,&TIMER_BKDTInitStructure);

    /* TIMER1 output enable */
    TIMER_CtrlPWMOutputs(TIMER1,ENABLE);

    TIMER_INTConfig(TIMER1,TIMER_INT_CCUG,ENABLE);
    TIMER_INTConfig(TIMER1,TIMER_INT_BREAK,DISABLE);

    /* TIMER1 counter enable */
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
    SysTick_Configuration();
    NVIC_Configuration();
    TIMER_Configuration();

    while(1){}
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
