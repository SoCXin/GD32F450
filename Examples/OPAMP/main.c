/**
  ******************************************************************************
  * @file    OPAMP/OPAMP/main.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "systick.h"
#include <stdlib.h>
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/
__IO uint16_t ADCConvertedValue;
float ADC_Value;

#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private functions ---------------------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void OPAMP_Configuration(void);
void ADC_Configuration(void);
void USART_Configuration(void);

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    /* Configure System Clocks */
    RCC_Configuration();
    SysTick_Configuration();
    /*Configure GPIO ports */
    GPIO_Configuration();
    OPAMP_Configuration();
    ADC_Configuration();
    USART_Configuration();
    while (1)
    {
        ADC_SoftwareStartConv_Enable(ENABLE);
        while(ADC_GetBitState(ADC_FLAG_EOC) != SET);
        ADCConvertedValue = ADC_GetConversionValue();
        ADC_Value = ((float)ADCConvertedValue*5.0/4095.0);
        ADC_ClearBitState(ADC_FLAG_EOC);
        ADC_SoftwareStartConv_Enable(DISABLE);
        printf("\n\rADC_Value %5.3f\n\r",ADC_Value);
        Delay_1ms(2000);
        printf("\n\r");
        printf("\n\r");
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
    RCC_ADCCLKConfig(RCC_ADCCLK_APB2_DIV8);
    /* GPIOA Periph clock enable */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOA|RCC_AHBPERIPH_GPIOC, ENABLE);
    /* OPAMP Periph clock enable */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_OPAMP, ENABLE);
    /* ADC1 Periph clock enable */
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_ADC1, ENABLE);
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_USART2, ENABLE);
}


/**
  * @brief  Configure system clocks.
  * @param  None
  * @retval None
  */
void OPAMP_Configuration(void)
{
    OPAMP_DeInit();

    OPAMP_PowerRangeConfig(OPAMP_OPARANGE_HIGH);
    OPAMP_Enable(OPAMP3,ENABLE);
    OPAMP_SwitchEnable(OPAMP_S1OPA3 |OPAMP_S2OPA3,ENABLE);
    
}

/**
  * @brief  Configure system clocks.
  * @param  None
  * @retval None
  */
void ADC_Configuration(void)
{
    ADC_InitPara ADC_InitStructure;
    /* Configure ADC1 --------------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode_Scan = ENABLE;
    ADC_InitStructure.ADC_Mode_Continuous = ENABLE;
    ADC_InitStructure.ADC_Trig_External = ADC_EXTERNAL_TRIGGER_MODE_NONE;
    ADC_InitStructure.ADC_Data_Align = ADC_DATAALIGN_RIGHT;
    ADC_InitStructure.ADC_Channel_Number = 1;
    ADC_Init(&ADC_InitStructure);
    /* Congig ADC1 regular channel11 */ 
    ADC_RegularChannel_Config( ADC_CHANNEL_13, 1, ADC_SAMPLETIME_239POINT5);
    /* Enable ADC1 */
    ADC_Enable(ENABLE);
    ADC_Calibration();
    /* Start ADC1 Software Conversion */
    ADC_SoftwareStartConv_Enable(ENABLE);
}

/**
  * @brief  Configure system clocks.
  * @param  None
  * @retval None
  */
void USART_Configuration(void)
{
    USART_InitPara  USART_InitStructure;

    USART_DeInit( USART2 );
    USART_InitStructure.USART_BRR                 = 115200;
    USART_InitStructure.USART_WL                  = USART_WL_8B;
    USART_InitStructure.USART_STBits              = USART_STBITS_1;
    USART_InitStructure.USART_Parity              = USART_PARITY_RESET;
    USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE;
    USART_InitStructure.USART_RxorTx              = USART_RXORTX_RX | USART_RXORTX_TX;
    USART_Init(USART2, &USART_InitStructure);
    USART_Enable(USART2, ENABLE);
}

/**
  * @brief  Configure GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
    GPIO_InitPara GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin =  GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_InitStructure.GPIO_OType   = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd   = GPIO_PUPD_NOPULL;    
    GPIO_Init(GPIOC, &GPIO_InitStructure);

 
    GPIO_InitStructure.GPIO_Pin     = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed   = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType   = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOA , &GPIO_InitStructure);

    GPIO_PinAFConfig( GPIOA , GPIO_PINSOURCE2, GPIO_AF_1 );
    GPIO_PinAFConfig( GPIOA , GPIO_PINSOURCE3, GPIO_AF_1 );
}



/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_DataSend( USART2 , (uint8_t) ch );

    /* Loop until transmit data register is empty */
    while ( USART_GetBitState( USART2 , USART_FLAG_TBE ) == RESET)
    {}

    return ch;
}
/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
