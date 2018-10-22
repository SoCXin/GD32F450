/**
  ******************************************************************************
  * @file    ADC/TIMTrigger_AutoInsertion/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include <stdlib.h>
#include <stdio.h>
#include "gd32f1x0_eval.h"

/* Private define ------------------------------------------------------------*/
#define ADC_RDTR_Address    ((uint32_t)0x4001244C)

/* Private variables ---------------------------------------------------------*/
ADC_InitPara                ADC_InitStructure;
DMA_InitPara                DMA_InitStructure;
TIMER_BaseInitPara          TIM_TimeBaseStructure;
TIMER_OCInitPara            TIM_OCInitStructure;

__IO uint16_t ADC_RegularConvertedValueTab[64], ADC_InsertedConvertedValueTab[64];

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
  
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{       
    /* Configure System clocks -----------------------------------------------*/
    RCC_Configuration();
    
    /* Configure NVIC --------------------------------------------------------*/
    NVIC_Configuration();
  
    /* Configure GPIO ports --------------------------------------------------*/
    GPIO_Configuration();
    
    /* Configure TIM2 --------------------------------------------------------*/ 
    /* Configure Time Base */
    TIMER_BaseStructInit(&TIM_TimeBaseStructure); 
    TIM_TimeBaseStructure.TIMER_Period = 256;          
    TIM_TimeBaseStructure.TIMER_Prescaler = 6;       
    TIM_TimeBaseStructure.TIMER_ClockDivision = 0x0;    
    TIM_TimeBaseStructure.TIMER_CounterMode = TIMER_COUNTER_UP;  
    TIMER_BaseInit(TIMER2, &TIM_TimeBaseStructure);
    
    /* Configure TIM2 channel2 in PWM mode */
    TIM_OCInitStructure.TIMER_OCMode = TIMER_OC_MODE_PWM1; 
    TIM_OCInitStructure.TIMER_OutputState = TIMER_OUTPUT_STATE_ENABLE;                
    TIM_OCInitStructure.TIMER_Pulse = 128; 
    TIM_OCInitStructure.TIMER_OCPolarity = TIMER_OC_POLARITY_LOW;         
    TIMER_OC2_Init(TIMER2, &TIM_OCInitStructure);
    
    /* Configure DMA1 Channel1 -----------------------------------------------*/
    DMA_DeInit(DMA1_CHANNEL1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC_RDTR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_RegularConvertedValueTab;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PERIPHERALSRC;
    DMA_InitStructure.DMA_BufferSize = 64;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PERIPHERALDATASIZE_HALFWORD;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MEMORYDATASIZE_HALFWORD;
    DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
    DMA_Init(DMA1_CHANNEL1, &DMA_InitStructure);
    
    /* Enable DMA1 channel1 */
    DMA_Enable(DMA1_CHANNEL1, ENABLE);
    
    /* Configure ADC ---------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode_Scan = ENABLE;
    ADC_InitStructure.ADC_Mode_Continuous = DISABLE;
    ADC_InitStructure.ADC_Trig_External  = ADC_EXTERNAL_TRIGGER_MODE_T2_CC2;
    ADC_InitStructure.ADC_Data_Align = ADC_DATAALIGN_RIGHT;
    ADC_InitStructure.ADC_Channel_Number = 1;
    ADC_Init(&ADC_InitStructure);
    
    /* Configure ADC regular channel14 */ 
    ADC_RegularChannel_Config(ADC_CHANNEL_14, 1, ADC_SAMPLETIME_55POINT5);
    
    /* Set inserted sequencer length */
    ADC_InsertedSequencerLength_Config(1);
    /* Configure ADC inserted channel */ 
    ADC_InsertedChannel_Config(ADC_CHANNEL_11, 1, ADC_SAMPLETIME_55POINT5);
    /* Configure ADC inserted external trigger */
    ADC_ExternalTrigInsertedConv_Config(ADC_EXTERNAL_TRIG_INSERTCONV_NONE);
    
    /* Enable automatic inserted conversion start after regular one */
    ADC_AutoInsertedConv_Enable(ENABLE);
    
    /* Enable ADC DMA */
    ADC_DMA_Enable(ENABLE);   
    
    /* Enable ADC external trigger */ 
    ADC_ExternalTrigConv_Enable( ENABLE);
    
    /* Enable EOIC interrupt */
    ADC_INTConfig(ADC_INT_EOIC, ENABLE);
    
    /* Enable ADC */
    ADC_Enable(ENABLE); 
    
    ADC_Calibration();
    
    /* TIM2 counter enable */
    TIMER_Enable(TIMER2, ENABLE);
    /* TIM2 main Output enable */
    TIMER_CtrlPWMOutputs(TIMER2, ENABLE);  
    
    /* Test on channel1 transfer complete flag */
    while(!DMA_GetBitState(DMA1_FLAG_TC1));
    /* Clear channel1 transfer complete flag */
    DMA_ClearBitState(DMA1_FLAG_TC1); 
    
    /* TIM2 counter disable */
    TIMER_Enable(TIMER2, DISABLE);
    /* Turn on the Led1 */
    GD_EVAL_LEDOn(LED1);
    while (1)
    {
    }
}

/**
  * @brief  Configure system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
    /* Configure the ADCCLK to 12MHz */
    RCC_ADCCLKConfig(RCC_ADCCLK_APB2_DIV6);
    
    /* Enable DMA1 and GPIO clock */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_DMA1|RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOC , ENABLE);
    
    /* Enable ADC and TIM2 clock */
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_ADC1, ENABLE);
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_TIMER2, ENABLE);
}

/**
  * @brief  Configure the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
    GPIO_InitPara GPIO_InitStructure;
    
    /* Configure TIM2_CH2 (PA1) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_OType= GPIO_OTYPE_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE1,GPIO_AF_2);

    /* Configure LED GPIO ports --------------------------------------------------*/                 
    GD_EVAL_LEDInit(LED1);
    GD_EVAL_LEDOff(LED1);
    /* Configure PC1 and PC4 (ADC Channel11 and Channel14) as analog input */
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_1 | GPIO_PIN_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
  * @brief  Configure NVIC and Vector Table base location.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
    NVIC_InitPara NVIC_InitStructure;
    
    /* Configure and enable ADC interrupt */
    NVIC_InitStructure.NVIC_IRQ = ADC1_CMP_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
