/**
  ******************************************************************************
  * @file    DAC/Dual_DAC_OutputVoltage/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"

/* Private variables ---------------------------------------------------------*/

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
    /* Configure DAC */
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
  * @brief  Configure the DAC peripheral.
  * @param  None
  * @retval None
  */
void DAC_Configuration(void)
{
    DAC_InitPara DAC_InitStructure;

    /* DAC1 channel Configuration */
    DAC_InitStructure.DAC_Trigger = DAC_TRIGGER_SOFTWARE;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
    DAC_Init(DAC1, &DAC_InitStructure);
    /* DAC2 channel Configuration */ 
    DAC_Init(DAC2, &DAC_InitStructure);

    /* Enable DAC1 Channel: Once the DAC1 channel is enabled, PA.04 is 
    automatically connected to the DAC converter. */
    DAC_Enable(DAC1, ENABLE);
    /* Enable DAC2 Channel: Once the DAC2 channel is enabled, PA.05 is 
    automatically connected to the DAC converter. */
    DAC_Enable(DAC2, ENABLE);
    DAC_SetDualDACData(DAC_ALIGN_12B_L,0x7ff0,0x1ff0);
    DAC_DualSoftwareTrigger_Enable(ENABLE);
}

/**
  * @brief  Configure GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
    GPIO_InitPara GPIO_InitStructure;

    /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically
    connected to the DAC converter. In order to avoid parasitic consumption,
    the GPIO pin should be configured in analog */
    GPIO_InitStructure.GPIO_Pin =  GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}



/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
