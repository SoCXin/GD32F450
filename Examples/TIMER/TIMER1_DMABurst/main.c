/**
  ******************************************************************************
  * @file    TIMER/TIMER1_DMABurst/main.c 
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

/* Private define ------------------------------------------------------------*/
#define TIMER1_DTRSF  ((uint32_t)0x040012c4c)

/* Private variables ---------------------------------------------------------*/
uint16_t buffer[8]={99,199,299,399,499,599,699,799};

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void TIMER_Configuration(void);
void DMA_Configuration(void);

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

    /*Configure PA8~PA11(TIMER1 CH1 CH2 CH3 CH4) as alternate function*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE8,GPIO_AF_2);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE9,GPIO_AF_2);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE10,GPIO_AF_2);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE11,GPIO_AF_2);

}

/**
  * @brief  Configure the DMA peripheral.
  * @param  None
  * @retval None
  */
void DMA_Configuration(void)
{
    DMA_InitPara DMA_InitStructure;

    /* DMA1 clock enable */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_DMA1,ENABLE);

    /* DMA1 Channel5 Config */
    DMA_DeInit(DMA1_CHANNEL5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = TIMER1_DTRSF;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t) buffer;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PERIPHERALDST;
    DMA_InitStructure.DMA_BufferSize         = 8;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PERIPHERALINC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MEMORYINC_ENABLE;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PERIPHERALDATASIZE_HALFWORD;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MEMORYDATASIZE_HALFWORD;
    DMA_InitStructure.DMA_Mode               = DMA_MODE_CIRCULAR;
    DMA_InitStructure.DMA_Priority           = DMA_PRIORITY_HIGH;
    DMA_InitStructure.DMA_MTOM               = DMA_MEMTOMEM_DISABLE;
    DMA_Init(DMA1_CHANNEL5, &DMA_InitStructure);
    
    /* DMA1 Channel5 enable */
    DMA_Enable(DMA1_CHANNEL5, ENABLE);
}

/**
  * @brief  Configure the TIMER peripheral.
  * @param  None
  * @retval None
  */
void TIMER_Configuration(void)
{
    /* TIMER1 DMA Transfer example -------------------------------------------------
    TIMER1CLK = 72MHz, Prescaler = 72 
    TIMER1 counter clock = SystemCoreClock/72 = 1MHz.

    The objective is to configure TIMER1 channel 1 to generate PWM signal with a
    frequency equal to 1KHz and a variable duty cycle(25%,50%,75%) that is 
    changed by the DMA after a specific number of Update DMA request.

    The number of this repetitive requests is defined by the TIMER1 Repetition counter,
    each 2 Update Requests, the TIMER1 Channel 1 Duty Cycle changes to the next new 
    value defined by the buffer . 
    -----------------------------------------------------------------------------*/
    TIMER_BaseInitPara TIMER_TimeBaseStructure;
    TIMER_OCInitPara  TIMER_OCInitStructure;
    /* TIMERS clock enable */
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_TIMER1|RCC_APB2PERIPH_TIMER15|RCC_APB2PERIPH_TIMER16|RCC_APB2PERIPH_TIMER17,ENABLE);
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_TIMER2|RCC_APB1PERIPH_TIMER3|RCC_APB1PERIPH_TIMER6|RCC_APB1PERIPH_TIMER14,ENABLE);

    /* TIMER1  configuration */
    TIMER_DeInit(TIMER1);
    TIMER_TimeBaseStructure.TIMER_Prescaler     = 71;
    TIMER_TimeBaseStructure.TIMER_CounterMode   = TIMER_COUNTER_UP;
    TIMER_TimeBaseStructure.TIMER_Period        = 999;
    TIMER_TimeBaseStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
    TIMER_BaseInit(TIMER1,&TIMER_TimeBaseStructure);

    /* CH1 Configuration in PWM mode */
    TIMER_OCInitStructure.TIMER_OCMode       = TIMER_OC_MODE_PWM1;
    TIMER_OCInitStructure.TIMER_OCPolarity   = TIMER_OC_POLARITY_HIGH;
    TIMER_OCInitStructure.TIMER_OCNPolarity  = TIMER_OCN_POLARITY_HIGH;
    TIMER_OCInitStructure.TIMER_OutputState  = TIMER_OUTPUT_STATE_ENABLE;
    TIMER_OCInitStructure.TIMER_OutputNState = TIMER_OUTPUTN_STATE_ENABLE;
    TIMER_OCInitStructure.TIMER_OCIdleState  = TIMER_OC_IDLE_STATE_SET;
    TIMER_OCInitStructure.TIMER_OCNIdleState = TIMER_OCN_IDLE_STATE_RESET;
     
    TIMER_OCInitStructure.TIMER_Pulse = buffer[0];
    TIMER_OC1_Init(TIMER1, &TIMER_OCInitStructure);
    TIMER_OC1_Preload(TIMER1,TIMER_OC_PRELOAD_DISABLE);

    TIMER_OC2_Init(TIMER1, &TIMER_OCInitStructure);
    TIMER_OC2_Preload(TIMER1,TIMER_OC_PRELOAD_DISABLE);

    TIMER_OC3_Init(TIMER1, &TIMER_OCInitStructure);
    TIMER_OC3_Preload(TIMER1,TIMER_OC_PRELOAD_DISABLE);

    TIMER_OC4_Init(TIMER1, &TIMER_OCInitStructure);
    TIMER_OC4_Preload(TIMER1,TIMER_OC_PRELOAD_DISABLE);

    /* TIMER1 output enable */
    TIMER_CtrlPWMOutputs(TIMER1,ENABLE);
    /* Auto-reload preload enable */
    TIMER_CARLPreloadConfig(TIMER1,ENABLE);
    /* TIMER1 Update DMA Request enable */

    TIMER_DMAConfig(TIMER1,TIMER_DMA_BASE_ADDR_CHCC1,TIMER_DMA_BURST_4TRANSFERS);
    TIMER_DMACmd( TIMER1, TIMER_DMA_UPDATE, ENABLE);
    /* TIMER enable counter*/
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
    GD_EVAL_LEDInit(LED2);
    SysTick_Configuration();
    DMA_Configuration();
    TIMER_Configuration();

    while(1)
    {
        GD_EVAL_LEDOn(LED2);
        Delay_1ms(1000);
        GD_EVAL_LEDOff(LED2);
        Delay_1ms(1000);
    }
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
