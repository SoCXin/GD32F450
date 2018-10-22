/**
  ******************************************************************************
  * @file    TIMER/TIMER2_ExtTrigger/main.c 
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

    /*Configure PA5(TIMER2 CH1) as alternate function*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_5;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE5,GPIO_AF_2);

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
    /* Timer with an external trigger -----
    TIMER2 is configured as Slave Timer for an external Trigger connected
    to TIMER2 TI1 pin :
    - The TIMER2 TI1FP1 is used as Trigger Input
    - Rising edge is used to start the TIMER2: Trigger Mode.
    - TIMER2 CH3 is used PWM2 Mode 
    The starts of the TIMER2 counter are controlled by the 
    external trigger.  
    -------------------------------------------------------------------- */
    TIMER_BaseInitPara  TIMER_TimeBaseStructure;
    TIMER_OCInitPara    TIMER_OCInitStructure;
    TIMER_ICInitPara    TIMER_ICInitStructure;

    /* TIMERS clock enable */
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_TIMER1|RCC_APB2PERIPH_TIMER15|RCC_APB2PERIPH_TIMER16|RCC_APB2PERIPH_TIMER17,ENABLE);
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_TIMER2|RCC_APB1PERIPH_TIMER3|RCC_APB1PERIPH_TIMER6|RCC_APB1PERIPH_TIMER14,ENABLE);

    /* TIMER2  configuration */
    TIMER_DeInit(TIMER2);
    TIMER_TimeBaseStructure.TIMER_Prescaler     = 35999;
    TIMER_TimeBaseStructure.TIMER_CounterMode   = TIMER_COUNTER_UP;
    TIMER_TimeBaseStructure.TIMER_Period        = 7999;
    TIMER_TimeBaseStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
    TIMER_BaseInit(TIMER2,&TIMER_TimeBaseStructure);

    /* CH3 Configuration in PWM mode */
    TIMER_OCInitStructure.TIMER_OCMode       = TIMER_OC_MODE_PWM2;
    TIMER_OCInitStructure.TIMER_OCPolarity   = TIMER_OC_POLARITY_HIGH;
    TIMER_OCInitStructure.TIMER_OCNPolarity  = TIMER_OCN_POLARITY_HIGH;
    TIMER_OCInitStructure.TIMER_OutputState  = TIMER_OUTPUT_STATE_ENABLE;
    TIMER_OCInitStructure.TIMER_OutputNState = TIMER_OUTPUTN_STATE_DISABLE;
    TIMER_OCInitStructure.TIMER_OCIdleState  = TIMER_OC_IDLE_STATE_RESET;
    TIMER_OCInitStructure.TIMER_OCNIdleState = TIMER_OCN_IDLE_STATE_RESET;
    TIMER_OCInitStructure.TIMER_Pulse        = 3999;
    TIMER_OC3_Init(TIMER2, &TIMER_OCInitStructure);
    TIMER_OC3_Preload(TIMER2,TIMER_OC_PRELOAD_DISABLE);

    /* TIMER2 Input Capture Configuration */
    TIMER_ICInitStructure.TIMER_CH          = TIMER_CH_1;
    TIMER_ICInitStructure.TIMER_ICPolarity  = TIMER_IC_POLARITY_RISING;
    TIMER_ICInitStructure.TIMER_ICSelection = TIMER_IC_SELECTION_DIRECTTI;
    TIMER_ICInitStructure.TIMER_ICPrescaler = TIMER_IC_PSC_DIV1;
    TIMER_ICInitStructure.TIMER_ICFilter = 0x02;
    TIMER_ICInit( TIMER2, &TIMER_ICInitStructure);
    
    /* Slave Mode selection: TIMER2 */
    /* TIMER2 Input trigger : External Trigger connected to TI1 */
    TIMER_SelectInputTrigger( TIMER2, TIMER_TS_TI1FP1);
    TIMER_SelectSlaveMode( TIMER2, TIMER_SLAVE_MODE_TRIGGER);
    /* Auto-reload preload enable */
    TIMER_CARLPreloadConfig(TIMER2,ENABLE);
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
