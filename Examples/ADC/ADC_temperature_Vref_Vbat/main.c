/**
  ******************************************************************************
  * @file    ADC/ADC_temperature_Vref_Vbat/main.c 
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

/* Private variables ---------------------------------------------------------*/
ADC_InitPara ADC_InitStructure;

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

    /* Configure ADC ---------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode_Scan = ENABLE;
    ADC_InitStructure.ADC_Mode_Continuous = DISABLE;
    ADC_InitStructure.ADC_Trig_External = ADC_EXTERNAL_TRIGGER_MODE_NONE;
    ADC_InitStructure.ADC_Data_Align = ADC_DATAALIGN_RIGHT;
    ADC_InitStructure.ADC_Channel_Number = 0;
    ADC_Init(&ADC_InitStructure);

    ADC_TempSensorVrefint_Enable(ENABLE);
    ADC_VBat_Enable(ENABLE);

    /* Configure ADC regular channel16 - 18 */ 
    ADC_InsertedSequencerLength_Config(3);
    ADC_InsertedChannel_Config(ADC_CHANNEL_16, 1, ADC_SAMPLETIME_239POINT5);
    ADC_InsertedChannel_Config(ADC_CHANNEL_17, 1, ADC_SAMPLETIME_239POINT5);
    ADC_InsertedChannel_Config(ADC_CHANNEL_18, 1, ADC_SAMPLETIME_239POINT5);

    ADC_ExternalTrigConv_Enable(ENABLE);
    ADC_ExternalTrigInsertedConv_Config(ADC_EXTERNAL_TRIG_INSERTCONV_NONE);


    /* Enable ADC */
    ADC_Enable(ENABLE);

    /* ADC Calibration*/
    ADC_Calibration();

    /* Start ADC Software Conversion */ 
    ADC_SoftwareStartInsertedConv_Enable(ENABLE);
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
    RCC_ADCCLKConfig( RCC_ADCCLK_APB2_DIV6 ); 

    /* Enable ADC clock */
    RCC_APB2PeriphClock_Enable( RCC_APB2PERIPH_ADC1, ENABLE );
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
