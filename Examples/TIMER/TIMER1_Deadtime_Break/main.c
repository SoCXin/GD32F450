/**        
  ******************************************************************************        
  * @file    TIMER/TIMER1_Deadtime_Break/main.c
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

    /*Configure PA8(TIMER1 CH1) as alternate function*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_8;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE8,GPIO_AF_2);

    /*Configure PA6(TIMER1 BKIN) as alternate function*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_6;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE6,GPIO_AF_2);

    /*Configure PB13(TIMER1 CH1N) as alternate function*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE13,GPIO_AF_2);
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
    Generate 1 complementary PWM signal.
    TIMER1CLK is fixed to SystemCoreClock, the TIMER1 Prescaler is equal to 2 so the
    TIMER1 counter clock used is 36MHz.
    The Duty cycle is computed as the following description:
    The channel 1 duty cycle is set to 25% so channel 1N is set to 75%.

    Insert a dead time equal to ((32+31)*16*4)/SystemCoreClock = 56us

    Configure the break feature, active at High level, and using the automatic
    output enable feature.

    Use the Locking parameters level1.
    ----------------------------------------------------------------------- */
    TIMER_BaseInitPara TIMER_TimeBaseStructure;
    TIMER_OCInitPara   TIMER_OCInitStructure;
    TIMER_BKDTInitPara TIMER_BKDTInitStructure;

    /* TIMERS clock enable */
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_TIMER1|RCC_APB2PERIPH_TIMER15|RCC_APB2PERIPH_TIMER16|RCC_APB2PERIPH_TIMER17,ENABLE);
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_TIMER2|RCC_APB1PERIPH_TIMER3|RCC_APB1PERIPH_TIMER6|RCC_APB1PERIPH_TIMER14,ENABLE);

    /* TIMER1  configuration */
    TIMER_DeInit(TIMER1);
    TIMER_TimeBaseStructure.TIMER_Prescaler     = 1;
    TIMER_TimeBaseStructure.TIMER_CounterMode   = TIMER_COUNTER_UP;
    TIMER_TimeBaseStructure.TIMER_Period        = 15999;
    TIMER_TimeBaseStructure.TIMER_ClockDivision = TIMER_CDIV_DIV4;
    TIMER_BaseInit(TIMER1,&TIMER_TimeBaseStructure);

    /* CH1 Configuration in PWM mode */
    TIMER_OCInitStructure.TIMER_OCMode       = TIMER_OC_MODE_PWM1;
    TIMER_OCInitStructure.TIMER_OCPolarity   = TIMER_OC_POLARITY_HIGH;
    TIMER_OCInitStructure.TIMER_OCNPolarity  = TIMER_OCN_POLARITY_HIGH;
    TIMER_OCInitStructure.TIMER_OutputState  = TIMER_OUTPUT_STATE_ENABLE;
    TIMER_OCInitStructure.TIMER_OutputNState = TIMER_OUTPUTN_STATE_ENABLE;
    TIMER_OCInitStructure.TIMER_OCIdleState  = TIMER_OC_IDLE_STATE_SET;
    TIMER_OCInitStructure.TIMER_OCNIdleState = TIMER_OCN_IDLE_STATE_RESET;

    TIMER_OCInitStructure.TIMER_Pulse = 3999;
    TIMER_OC1_Init(TIMER1, &TIMER_OCInitStructure);
    TIMER_OC1_Preload(TIMER1,TIMER_OC_PRELOAD_DISABLE);

    /* Automatic Output enable, Break, dead time and lock configuration*/
    TIMER_BKDTInitStructure.TIMER_ROSState      = TIMER_ROS_STATE_DISABLE;
    TIMER_BKDTInitStructure.TIMER_IOSState      = TIMER_IOS_STATE_DISABLE;
    TIMER_BKDTInitStructure.TIMER_LOCKLevel     = TIMER_LOCK_LEVEL_1;
    TIMER_BKDTInitStructure.TIMER_DeadTime      = 255;
    TIMER_BKDTInitStructure.TIMER_Break         = TIMER_BREAK_ENABLE;
    TIMER_BKDTInitStructure.TIMER_BreakPolarity = TIMER_BREAK_POLARITY_LOW;
    TIMER_BKDTInitStructure.TIMER_OutAuto       = TIMER_OUTAUTO_ENABLE;
    TIMER_BKDTConfig(TIMER1,&TIMER_BKDTInitStructure);

    /* TIMER1 output enable */
    TIMER_CtrlPWMOutputs(TIMER1,ENABLE);
    /* Auto-reload preload enable */
    TIMER_CARLPreloadConfig(TIMER1,ENABLE);
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
