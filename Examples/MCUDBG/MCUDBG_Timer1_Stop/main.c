/**    
  ******************************************************************************    
  * @file    MCUDGB/MCUDBG_Timer1_stop/main.c    
  * @author  MCU SD    
  * @version V2.0.0    
  * @date    15-Jan-2016    
  * @brief   The main function file.    
  ******************************************************************************    
  */    

/* Includes ------------------------------------------------------------------*/    
#include "gd32f1x0.h"    
#include <stdio.h>    
#include "systick.h"    
#include "gd32f1x0_eval.h"

/* Private function prototypes -----------------------------------------------*/
void LED_config(void);
void TIM_Configuration(void);

/* Private functions ---------------------------------------------------------*/    
/**    
  * @brief  Main program.    
  * @param  None    
  * @retval None    
  */    
int main(void)    
{    
    LED_config();
    SysTick_Configuration();
    TIM_Configuration();
    MCUDBG_CTLR1PeriphConfig(MCUDBG_TIMER1_HOLD,ENABLE);
    
    while (1)    
    {    
        GD_EVAL_LEDToggle(LED1);
        Delay_1ms(1000);
    }        
}    

/**    
  * @brief  Configures the GPIO ports.
  * @param  None    
  * @retval None    
  */    
void LED_config(void)    
{    
    GD_EVAL_LEDInit(LED1);
    GD_EVAL_LEDOff(LED1);
}

/**
  * @brief  Configure the TIMER peripheral.
  * @param  None
  * @retval None
  */
void TIM_Configuration(void)
{
/* -----------------------------------------------------------------------
    TIMER1 Configuration to:
    Generate 3 complementary PWM signals with 3 different duty cycles:
    TIMER1CLK is fixed to SystemCoreClock, the TIMER1 Prescaler is equal 
    to 5400 so the TIMER1 counter clock used is 20KHz.

    The Three Duty cycles are computed as the following description:
    The channel 1 duty cycle is set to 25% so channel 1N is set to 75%.
    The channel 2 duty cycle is set to 50% so channel 2N is set to 50%.
    The channel 3 duty cycle is set to 75% so channel 3N is set to 25%.
  ----------------------------------------------------------------------- */
    TIMER_BaseInitPara TIM_TimeBaseStructure;
    TIMER_OCInitPara  TIM_OCInitStructure;
    GPIO_InitPara GPIO_InitStructure;
      
    /* GPIOA, GPIOB clocks enable */
    RCC_AHBPeriphClock_Enable( RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB , ENABLE);
    
    /* TIMER1 clock enable */
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_TIMER1, ENABLE);
    
    /* Configure PA8 PA9 PA10(TIMER1 CH1 CH2 CH3) as alternate function */
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;    
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;    
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;    
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL; 
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE8,GPIO_AF_2);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE9,GPIO_AF_2);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE10,GPIO_AF_2);
    
    /* Configure PB13 PB14 PB15(TIMER1 CH1N CH2N CH3N) as alternate function */
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;    
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;    
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;    
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL; 
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE13,GPIO_AF_2);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE14,GPIO_AF_2);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE15,GPIO_AF_2);

    /* TIMER1 configuration */
    TIMER_DeInit(TIMER1);
    TIM_TimeBaseStructure.TIMER_Prescaler     = 5399;
    TIM_TimeBaseStructure.TIMER_CounterMode   = TIMER_COUNTER_UP;
    TIM_TimeBaseStructure.TIMER_Period        = 15999;
    TIM_TimeBaseStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
    TIMER_BaseInit(TIMER1,&TIM_TimeBaseStructure);

    /* CH1,CH2 and CH3 Configuration in PWM mode */
    TIM_OCInitStructure.TIMER_OCMode       = TIMER_OC_MODE_PWM1;
    TIM_OCInitStructure.TIMER_OCPolarity   = TIMER_OC_POLARITY_HIGH;
    TIM_OCInitStructure.TIMER_OCNPolarity  = TIMER_OCN_POLARITY_HIGH;
    TIM_OCInitStructure.TIMER_OutputState  = TIMER_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.TIMER_OutputNState = TIMER_OUTPUTN_STATE_ENABLE;
    TIM_OCInitStructure.TIMER_OCIdleState  = TIMER_OC_IDLE_STATE_RESET;
    
    TIM_OCInitStructure.TIMER_Pulse = 3999;
    TIMER_OC1_Init(TIMER1, &TIM_OCInitStructure);
    TIMER_OC1_Preload(TIMER1,TIMER_OC_PRELOAD_DISABLE);
    
    TIM_OCInitStructure.TIMER_Pulse = 7999;
    TIMER_OC2_Init(TIMER1, &TIM_OCInitStructure);
    TIMER_OC2_Preload(TIMER1,TIMER_OC_PRELOAD_DISABLE);

    TIM_OCInitStructure.TIMER_Pulse = 11999;
    TIMER_OC3_Init(TIMER1, &TIM_OCInitStructure);
    TIMER_OC3_Preload(TIMER1,TIMER_OC_PRELOAD_DISABLE);

    /* TIMER1 output enable */
    TIMER_CtrlPWMOutputs(TIMER1,ENABLE);
    /* Auto-reload preload enable */
    TIMER_CARLPreloadConfig(TIMER1,ENABLE);
    /* TIMER enable counter*/
    TIMER_Enable( TIMER1, ENABLE );
}



/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/    
