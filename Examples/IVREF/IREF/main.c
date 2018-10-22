/**
  ******************************************************************************
  * @file    IVREF/IREF/main.c 
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
void GPIO_Configuration(void);
void IREF_Configuration(void);

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
    IREF_Configuration();

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
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOB, ENABLE);
    /* DAC Periph clock enable */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_IVREF, ENABLE);
}


/**
  * @brief  Configure system clocks.
  * @param  None
  * @retval None
  */
void IREF_Configuration(void)
{  
    IVREF_DeInit();
    
    IVREF_SetIREFMode(IREF_MODE_HIGH_CURRENT);
    IVREF_IREF_Enable(ENABLE);
    IVREF_SetIREFSink(IREF_SOURCE_CURRENT);
    IVREF_IREFPrecisionTrimConfig(IREF_CURRENT_PRECISION_TRIM_5);
    IVREF_IREFStepDataConfig(IREF_CURRENT_STEP_DATA_3);
}


/**
  * @brief  Configure GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
    GPIO_InitPara GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin =  GPIO_PIN_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}



/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
