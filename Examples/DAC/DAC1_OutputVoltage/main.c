/**
  ******************************************************************************
  * @file    DAC/DAC1_OutputVoltage/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"

/* Private functions ---------------------------------------------------------*/
void RCC_Configuration(void);
void DAC_Configuration(void);
void GPIO_Configuration(void);

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    /* Configure System Clocks */
    RCC_Configuration();
    /*Configure GPIO ports */
    GPIO_Configuration();
    /* Configure DAC */
    DAC_Configuration();
    /* Start DAC1 conversion by software */
    DAC_SoftwareTrigger_Enable(DAC1,ENABLE);
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
    /* GPIOA Periph clock enable */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOA, ENABLE);
    /* DAC Periph clock enable */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_DAC, ENABLE);
}

/**
  * @brief  Configure DAC.
  * @param  None
  * @retval None
  */
void DAC_Configuration(void)
{   
    DAC_InitPara   DAC_InitStructure;
    DAC_InitStructure.DAC_Trigger = DAC_TRIGGER_SOFTWARE;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    DAC_Init(DAC1,&DAC_InitStructure);
    /* Enable DAC1 */
    DAC_Enable(DAC1,ENABLE);
    /* Set DAC1_L12DHR register */
    DAC_SetDAC1Data(DAC_ALIGN_12B_L, 0x7FF0);
}

/**
  * @brief  Configure GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
    GPIO_InitPara GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin =  GPIO_PIN_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}



/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
