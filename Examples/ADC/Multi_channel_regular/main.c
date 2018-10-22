/**
  ******************************************************************************
  * @file    ADC/Multi_channel_regular/main.c 
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
#define ADC_RDTR_Address        ((uint32_t)0x4001244C) 

/* Private variables ---------------------------------------------------------*/
uint16_t ADC_ConvertedValue[4];
uint16_t ADC_Channel1[256];
uint16_t ADC_Channel2[256]; 
uint16_t ADC_Channel3[256]; 
uint16_t ADC_Channel4[256]; 
int AD_I=0;

/* Private function prototypes -----------------------------------------------*/
void ADC_Configuration(void);
void NVIC_Configuration(void);

void Delay_ARMJISHU(__IO uint32_t nCount)
{
    for (; nCount != 0; nCount--);
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    NVIC_Configuration();
    ADC_Configuration();

    GD_EVAL_LEDOff(LED2);
    GD_EVAL_LEDOff(LED3);

    while (1)
    {
    }
}

/**
  * @brief  Configure NVIC and Vector Table base location.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
    NVIC_InitPara NVIC_InitStructure;
    
    NVIC_PRIGroup_Enable(NVIC_PRIGROUP_1);
    
    /* Configure DMA1 interrupt */
    NVIC_InitStructure.NVIC_IRQ = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 1;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configure the system clocks,GPIO ports,DMA and ADC.
  * @param  None
  * @retval None
  */
void ADC_Configuration(void)
{     
    GPIO_InitPara GPIO_InitStructure;
    DMA_InitPara DMA_InitStructure;
    ADC_InitPara ADC_InitStructure;
    
    /* Configure the system clocks -------------------------------------------*/
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_ADC1, ENABLE);      
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOC|RCC_AHBPERIPH_GPIOF,ENABLE);
    RCC_ADCCLKConfig(RCC_ADCCLK_APB2_DIV6);
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_DMA1, ENABLE);
    
    /* Configure GPIO ports --------------------------------------------------*/                 
    GD_EVAL_LEDInit(LED2);
    GD_EVAL_LEDInit(LED3);

    /* Configure PC1 (ADC Channel11) as analog input */
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_1;     
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);     
    
    /* Configure PC2 (ADC Channel12) as analog input */
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_2;     
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);     
    
    /* Configure PC3 (ADC Channel13) as analog input */ 
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_3;     
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);      
    
    /* Configure PC4 (ADC Channel14) as analog input */ 
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_4;     
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);     
    
    /* DMA channel1 configuration ----------------------------------------------*/
    DMA_DeInit(DMA1_CHANNEL1); 
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC_RDTR_Address; 
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue; 
    DMA_InitStructure.DMA_DIR = DMA_DIR_PERIPHERALSRC;  
    DMA_InitStructure.DMA_BufferSize = 4;    
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;    
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE; 
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PERIPHERALDATASIZE_HALFWORD; 
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MEMORYDATASIZE_HALFWORD; 
    DMA_InitStructure.DMA_Mode = DMA_MODE_CIRCULAR; 
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE; 
    DMA_Init(DMA1_CHANNEL1, &DMA_InitStructure);     
    
    /* Enable DMA1 channel1 */
    DMA_Enable(DMA1_CHANNEL1, ENABLE);  
    
    ADC_DeInit(&ADC_InitStructure);
    ADC_InitStructure.ADC_Mode_Scan = ENABLE;        
    ADC_InitStructure.ADC_Mode_Continuous = DISABLE;  
    ADC_InitStructure.ADC_Trig_External = ADC_EXTERNAL_TRIGGER_MODE_NONE;  
    ADC_InitStructure.ADC_Data_Align = ADC_DATAALIGN_RIGHT;
    ADC_InitStructure.ADC_Channel_Number = 4;  
    ADC_Init(&ADC_InitStructure);  
    
    /* ADC regular channels configuration */ 
    ADC_RegularChannel_Config(ADC_CHANNEL_11, 1, ADC_SAMPLETIME_55POINT5);
    ADC_RegularChannel_Config(ADC_CHANNEL_12, 2, ADC_SAMPLETIME_55POINT5);
    ADC_RegularChannel_Config(ADC_CHANNEL_13, 3, ADC_SAMPLETIME_55POINT5);
    ADC_RegularChannel_Config(ADC_CHANNEL_14, 4, ADC_SAMPLETIME_55POINT5);
    
    /* Enable ADC DMA */
    ADC_DMA_Enable(ENABLE); 
    DMA_INTConfig(DMA1_CHANNEL1, DMA1_INT_TC1,ENABLE);
    
    /* Enable ADC */
    ADC_Enable(ENABLE); 
    ADC_Calibration();

    /* Start ADC Software Conversion */ 
    ADC_SoftwareStartConv_Enable(ENABLE);  
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
