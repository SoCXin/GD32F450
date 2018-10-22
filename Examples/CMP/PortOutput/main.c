/**
  ******************************************************************************
  * @file    CMP/PortOutput/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "stdio.h"

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
    CMP_InitPara CMP_InitParaStruct;
    /* Enable Clock */
    RCC_Configuration();
    /* Enable GPIOA analog and alternate output */
    GPIO_Configuration();
    /* Configure CMP1 */
    CMP_ParaInit(&CMP_InitParaStruct);
    CMP_InitParaStruct.CMP_InvertingInput = CMP_INVERTINGINPUT_1_4VREFINT;
    CMP_InitParaStruct.CMP_Output = CMP_OUTPUT_NONE;
    CMP_InitParaStruct.CMP_OutputPolarity = CMP_OUTPUTPOLARITY_NONINVERTED;
    CMP_InitParaStruct.CMP_Hysteresis = CMP_HYSTERESIS_NO;
    CMP_InitParaStruct.CMP_OperatingMode = CMP_OPERATINGMODE_VERYLOWSPEED;
    CMP_Init(CMP_CHANNEL_CMP1, &CMP_InitParaStruct);
    /* Enable CMP1 */
    CMP_Enable(CMP_CHANNEL_CMP1, ENABLE);

    while(1);
}

/**
  * @brief  RCC configuration.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOA,ENABLE);
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_CMP ,ENABLE);
}

/**
  * @brief  GPIO configuration.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
    GPIO_InitPara GPIO_InitStruct;

    GPIO_ParaInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_PIN_1 ;
    GPIO_InitStruct.GPIO_Mode  = GPIO_MODE_AN;
    GPIO_InitStruct.GPIO_Speed =  GPIO_SPEED_50MHZ;
    GPIO_InitStruct.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStruct.GPIO_PuPd  =GPIO_PUPD_PULLUP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin   = GPIO_PIN_6;
    GPIO_InitStruct.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStruct.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PUPD_PULLUP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE6,GPIO_AF_7);
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
