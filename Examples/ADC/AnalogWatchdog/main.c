/**
  ******************************************************************************
  * @file    ADC/AnalogWatchdog/main.c 
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
#ifdef GD32F130_150
#define BOARD_ADC_CHANNEL ADC_CHANNEL_11
#define ADC_GPIO_PIN GPIO_PIN_1
#elif defined GD32F170_190
#define BOARD_ADC_CHANNEL ADC_CHANNEL_10
#define ADC_GPIO_PIN GPIO_PIN_0
#endif

/* Private variables ---------------------------------------------------------*/
ADC_InitPara  ADC_InitStructure;
    
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
    
    /* NVIC configuration ----------------------------------------------------*/
    NVIC_Configuration();
    
    /* Configure GPIO ports --------------------------------------------------*/
    GPIO_Configuration();   
    
    /* Configure ADC ---------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode_Scan = DISABLE;
    ADC_InitStructure.ADC_Mode_Continuous = ENABLE;
    ADC_InitStructure.ADC_Trig_External = ADC_EXTERNAL_TRIGGER_MODE_NONE;
    ADC_InitStructure.ADC_Data_Align = ADC_DATAALIGN_RIGHT;
    ADC_InitStructure.ADC_Channel_Number = 1;
    ADC_Init(&ADC_InitStructure);
    
    /* Configure ADC regular channel11 or channel10 */ 
    ADC_RegularChannel_Config(BOARD_ADC_CHANNEL, 1, ADC_SAMPLETIME_239POINT5);
    
    /* Configure high and low analog watchdog thresholds */
    ADC_AnalogWatchdogThresholds_Config(0x0A00, 0x0400);
    /* Configure channel11 as the single analog watchdog guarded channel */
    ADC_AnalogWatchdogSingleChannel_Config(BOARD_ADC_CHANNEL);
    /* Enable analog watchdog on one regular channel */
    ADC_AnalogWatchdog_Enable(ADC_ANALOGWATCHDOG_SINGLEREGENABLE);
    
    /* Enable AWE interrupt */
    ADC_INTConfig(ADC_INT_AWE, ENABLE);
    
    /* Enable ADC */
    ADC_Enable(ENABLE);
    
    /* ADC calibration */   
    ADC_Calibration();
    
    /* Start ADC Software Conversion */ 
    ADC_SoftwareStartConv_Enable(ENABLE);
    
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
    /* ADCCLK = PCLK2/6 */
    RCC_ADCCLKConfig(RCC_ADCCLK_APB2_DIV6); 
    /* Enable GPIOC clock */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOC, ENABLE);
    /* Enable ADC clock */
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_ADC1, ENABLE);
}

/**
  * @brief  Configure GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
    GPIO_InitPara GPIO_InitStructure;
    
    /* Configure PC1 or PC0 (ADC Channel11 or Channel10) as analog input */
    GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    /* LED1 GPIO Initialization */
    GD_EVAL_LEDInit(LED2);
}

/**
  * @brief  Configure NVIC.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
    NVIC_InitPara NVIC_InitStructure;
    
    /* Configure ADC interrupt */
    NVIC_InitStructure.NVIC_IRQ = ADC1_CMP_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
