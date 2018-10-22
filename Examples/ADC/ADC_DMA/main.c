/**
  ******************************************************************************
  * @file    ADC/ADC_DMA/main.c 
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

/* Private define ------------------------------------------------------------*/
#ifdef GD32F130_150
#define BOARD_ADC_CHANNEL ADC_CHANNEL_11
#define ADC_GPIO_PIN GPIO_PIN_1
#elif defined GD32F170_190
#define BOARD_ADC_CHANNEL ADC_CHANNEL_10
#define ADC_GPIO_PIN GPIO_PIN_0
#endif

#define ADC_RDTR_Address    ((uint32_t)0x4001244C)

/* Private variables ---------------------------------------------------------*/
__IO uint16_t ADCConvertedValue;
ADC_InitPara ADC_InitStructure;
DMA_InitPara DMA_InitStructure;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);

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
    
    /* Configure GPIO ports --------------------------------------------------*/
    GPIO_Configuration();
    
    /* Configure DMA1 channel1 -----------------------------------------------*/
    DMA_DeInit(DMA1_CHANNEL1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC_RDTR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PERIPHERALSRC;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_DISABLE;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PERIPHERALDATASIZE_HALFWORD;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MEMORYDATASIZE_HALFWORD;
    DMA_InitStructure.DMA_Mode = DMA_MODE_CIRCULAR;
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
    DMA_Init(DMA1_CHANNEL1, &DMA_InitStructure);
    
    /* Enable DMA1 channel1 */
    DMA_Enable(DMA1_CHANNEL1, ENABLE);
    
    /* Configure ADC ---------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode_Scan = DISABLE;
    ADC_InitStructure.ADC_Mode_Continuous = ENABLE;
    ADC_InitStructure.ADC_Trig_External = ADC_EXTERNAL_TRIGGER_MODE_NONE;
    ADC_InitStructure.ADC_Data_Align = ADC_DATAALIGN_RIGHT;
    ADC_InitStructure.ADC_Channel_Number = 1;
    ADC_Init(&ADC_InitStructure);
    
    /* Configure ADC regular channelx */ 
    ADC_RegularChannel_Config( BOARD_ADC_CHANNEL, 1, ADC_SAMPLETIME_239POINT5);
    
    /* Enable ADC DMA */
    ADC_DMA_Enable(ENABLE);
    
    /* Enable ADC */
    ADC_Enable(ENABLE);
    
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

    /* Enable DMA1 and GPIOC clock */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_DMA1|RCC_AHBPERIPH_GPIOC, ENABLE);
    
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
    
    /* Configure PC1 or PC0 (ADC Channel11 or Channel10) as analog input -------------------------*/
    GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
