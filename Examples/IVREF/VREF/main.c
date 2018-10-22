/**
  ******************************************************************************
  * @file    IVREF/VREF/main.c 
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
void VREF_Configuration(void);

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
    VREF_Configuration();

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
    /* GPIOB Periph clock enable */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOB, ENABLE);
    /* IVREF Periph clock enable */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_IVREF, ENABLE);
}

/**
  * @brief  Configure system clocks.
  * @param  None
  * @retval None
  */
void VREF_Configuration(void)
{
    IVREF_DeInit();
    IVREF_SetVREFMode(VREF_DISCONNECT_EXTERNAL_CAP);
    IVREF_VREFPrecisionTrimConfig(VREF_VOLTAGE_PRECISION_TRIM_8); 
    IVREF_VREF_Enable(ENABLE);
}


/**
  * @brief  Configure GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
    GPIO_InitPara GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin =  GPIO_PIN_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}



/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
