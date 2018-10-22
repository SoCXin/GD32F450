/**
  ******************************************************************************
  * @file    CMP/Switch/main.c 
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
void DAC_Configuration(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    CMP_InitPara CMP_InitParaStruct;
    RCC_Configuration();
    GPIO_Configuration();
    DAC_Configuration();
    /* Set data for DAC channel1 */
    DAC_SetDAC1Data(DAC_ALIGN_12B_R ,0xA00);
    DAC_SoftwareTrigger_Enable(DAC1,ENABLE);
    /* initialize CMP1 */
    CMP_ParaInit(&CMP_InitParaStruct);
    CMP_InitParaStruct.CMP_InvertingInput = CMP_INVERTINGINPUT_1_2VREFINT;
    CMP_InitParaStruct.CMP_Output = CMP_OUTPUT_NONE;
    CMP_InitParaStruct.CMP_OutputPolarity = CMP_OUTPUTPOLARITY_NONINVERTED;
    CMP_InitParaStruct.CMP_Hysteresis = CMP_HYSTERESIS_NO;
    CMP_InitParaStruct.CMP_OperatingMode = CMP_OPERATINGMODE_VERYLOWSPEED;
    CMP_Init(CMP_CHANNEL_CMP1, &CMP_InitParaStruct);
    /* Turn on switch between CMP1 and DAC */
    CMP_Switch_Enable(ENABLE);
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
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_DAC,ENABLE);
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
    GPIO_InitStruct.GPIO_Pin   = GPIO_PIN_4 ;
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

/**
  * @brief  DAC configuration.
  * @param  None
  * @retval None
  */
void DAC_Configuration(void)
{
    DAC_InitPara DAC_InitStruct;

    DAC_Init(DAC1,&DAC_InitStruct);
    DAC_InitStruct.DAC_Trigger= DAC_TRIGGER_SOFTWARE;
    DAC_InitStruct.DAC_OutputBuffer=DAC_OUTPUTBUFFER_ENABLE ;
    
    DAC_Enable(DAC1,ENABLE);
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
