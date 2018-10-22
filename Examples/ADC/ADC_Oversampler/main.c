/**
  ******************************************************************************
  * @file    ADC/ADC_Oversampler/main.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

#if!defined (GD32F170_190)
 #error "This demo can only run on the GD32F170_190 devices!"
#endif 

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include <stdlib.h>
#include <stdio.h>
#include "gd32f1x0_eval.h"

/* Private variables ---------------------------------------------------------*/
__IO uint16_t ADCConvertedValue;
__IO uint16_t ADCConvertedValue_OVER;
ADC_InitPara ADC_InitStructure;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
void ADC_Configuration(void);
void ADC_OVERConfiguration(void);


#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
/* __GNUC__ */

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
    USART_Configuration();

    /* Output a message on Hyperterminal using printf function */
    printf("\n\rADC different test: \n\r");

    ADC_Configuration();
    while(ADC_GetBitState(ADC_FLAG_EOC) != SET);
    ADCConvertedValue = ADC_GetConversionValue();

    printf("\n\rThe original data  %d\n\r",ADCConvertedValue);
    ADC_DeInit(&ADC_InitStructure);

    ADC_OVERConfiguration();
    while(ADC_GetBitState(ADC_FLAG_EOC) != SET);
    ADCConvertedValue_OVER = ADC_GetConversionValue();

    printf("\n\rOversampling data  %d\n\r",ADCConvertedValue_OVER);

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
    /* Enable GPIO clock */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOC|RCC_AHBPERIPH_GPIOA, ENABLE);
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

    /* Configure PC0 (ADC Channel10) as analog input -------------------------*/
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

}

/**
  * @brief  Configure ADC Configuration.
  * @param  None
  * @retval None
  */
void ADC_OVERConfiguration(void)
{
    /* Configure ADC ----------------------------------------------------------*/
    ADC_InitPara ADC_InitStructure;

    ADC_InitStructure.ADC_Mode_Scan = DISABLE ;
    ADC_InitStructure.ADC_Mode_Continuous = DISABLE;
    ADC_InitStructure.ADC_Trig_External = ADC_EXTERNAL_TRIGGER_MODE_NONE;
    ADC_InitStructure.ADC_Data_Align = ADC_DATAALIGN_RIGHT;
    ADC_InitStructure.ADC_Channel_Number = 1;
    ADC_Init(&ADC_InitStructure);

    /* ADC regular channels configuration */
    ADC_RegularChannel_Config(ADC_CHANNEL_10, 1,ADC_SAMPLETIME_55POINT5);

    ADC_OversamplingRatio_Config(ADC_OVERSAMPLING_RATIO_4X);
    ADC_OversamplingShift_Config(ADC_OVERSAMPLING_SHIFT_NONE);
    ADC_Oversampler_Enable(ENABLE);

    /* Enable ADC */
    ADC_Enable(ENABLE);
    ADC_Calibration();

    /* Start ADC Software Conversion */
    ADC_SoftwareStartConv_Enable(ENABLE);
}

/**
  * @brief  Configure ADC Configuration.
  * @param  None
  * @retval None
  */
void ADC_Configuration(void)
{
    /* Configure ADC ----------------------------------------------------------*/
    ADC_InitPara ADC_InitStructure;

    ADC_InitStructure.ADC_Mode_Scan = DISABLE ;
    ADC_InitStructure.ADC_Mode_Continuous = DISABLE;
    ADC_InitStructure.ADC_Trig_External = ADC_EXTERNAL_TRIGGER_MODE_NONE;
    ADC_InitStructure.ADC_Data_Align = ADC_DATAALIGN_RIGHT;
    ADC_InitStructure.ADC_Channel_Number = 1;
    ADC_Init(&ADC_InitStructure);

    /* ADC regular channels configuration */
    ADC_RegularChannel_Config(ADC_CHANNEL_10, 1, ADC_SAMPLETIME_55POINT5);

    /* Enable ADC */
    ADC_Enable(ENABLE);
    ADC_Calibration();

    /* Start ADC Software Conversion */
    ADC_SoftwareStartConv_Enable(ENABLE);
}


/**
  * @brief  Configure USART parameter.
  * @param  None
  * @retval None
  */
void USART_Configuration(void)
{
    USART_InitPara USART_InitStructure;

    USART_InitStructure.USART_BRR = 115200;
    USART_InitStructure.USART_WL = USART_WL_8B;
    USART_InitStructure.USART_STBits = USART_STBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_RESET;
    USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE;
    USART_InitStructure.USART_RxorTx = USART_RXORTX_RX | USART_RXORTX_TX;
    GD_EVAL_COMInit(&USART_InitStructure);
}

/**
  * @brief  Retarget the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    /* Loop until transmit data register is empty */
    while (USART_GetBitState( EVAL_COM1 , USART_FLAG_TBE) == RESET)
    {
    }
    /* Place your implementation of fputc here */
    USART_DataSend( EVAL_COM1 , (uint8_t) ch );
    return ch;
}


/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
