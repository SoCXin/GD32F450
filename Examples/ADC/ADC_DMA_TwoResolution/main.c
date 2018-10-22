/**
  ******************************************************************************
  * @file    ADC/ADC_DMA_TwoResolution/main.c
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

/* Private define ------------------------------------------------------------*/
#define ADC_RDTR_Address ((uint32_t)0x4001244C)

/* Private variables ---------------------------------------------------------*/
__IO uint16_t ADCConvertedValue6B;
__IO uint16_t ADCConvertedValue12B;
__IO float ADC_Value6B;
__IO float ADC_Value12B;
ADC_InitPara ADC_InitStructure;
DMA_InitPara DMA_InitStructure;
uint32_t Time_Delay = 0;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
void DMA_6BConfiguration(void);
void DMA_12BConfiguration(void);
void ADC_6BConfiguration(void);
void ADC_12BConfiguration(void);
void delay_ms(uint32_t nms);

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
    printf("\n\rADC different resolution test: \n\r");

    while (1)
    {
        DMA_12BConfiguration();
        ADC_12BConfiguration();

        /* Test on channel1 transfer complete flag */
        while(!DMA_GetBitState(DMA1_FLAG_TC1));
        /* Clear channel1 transfer complete flag */
        DMA_ClearBitState(DMA1_FLAG_TC1);

        ADC_Value12B = (float)ADCConvertedValue12B/4096*5;
        printf("\n\rresolution12   %5.3fV\n\r",ADC_Value12B);
        ADC_DeInit(&ADC_InitStructure);

        delay_ms(5000);

        DMA_6BConfiguration();
        ADC_6BConfiguration();

        /* Test on channel1 transfer complete flag */
        while(!DMA_GetBitState(DMA1_FLAG_TC1));
        /* Clear channel1 transfer complete flag */
        DMA_ClearBitState(DMA1_FLAG_TC1);

        ADC_Value6B = (float)ADCConvertedValue6B/4032*5;
        printf("\n\rresolution6    %5.3fV\n\r",ADC_Value6B);
        ADC_DeInit(&ADC_InitStructure);

        delay_ms(5000);
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

    /* Enable DMA1 and GPIO clock */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_DMA1|RCC_AHBPERIPH_GPIOC|RCC_AHBPERIPH_GPIOA, ENABLE);

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
  * @brief  Configure ADC_6B resolution.
  * @param  None
  * @retval None
  */
void ADC_6BConfiguration(void)
{
    /* Configure ADC ----------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode_Scan = DISABLE;
    ADC_InitStructure.ADC_Mode_Continuous = ENABLE;
    ADC_InitStructure.ADC_Trig_External = ADC_EXTERNAL_TRIGGER_MODE_NONE;
    ADC_InitStructure.ADC_Data_Align = ADC_DATAALIGN_RIGHT;
    ADC_InitStructure.ADC_Channel_Number = 1;
    ADC_Init(&ADC_InitStructure);

    /* Configure ADC regular channel10 */
    ADC_RegularChannel_Config(ADC_CHANNEL_10, 1, ADC_SAMPLETIME_55POINT5);

    /* Enable ADC DMA */
    ADC_DMA_Enable(ENABLE);
    ADC_Resolution_Config(ADC_RESOLUTION_6B);
    /* Enable ADC */
    ADC_Enable(ENABLE);
    ADC_Calibration();
    /* Start ADC Software Conversion */
    ADC_SoftwareStartConv_Enable(ENABLE);
}

/**
  * @brief  Configure ADC_12B resolution.
  * @param  None
  * @retval None
  */
void ADC_12BConfiguration(void)
{
    /* Configure ADC ----------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode_Scan = DISABLE;
    ADC_InitStructure.ADC_Mode_Continuous = DISABLE;
    ADC_InitStructure.ADC_Trig_External = ADC_EXTERNAL_TRIGGER_MODE_NONE;
    ADC_InitStructure.ADC_Data_Align = ADC_DATAALIGN_RIGHT;
    ADC_InitStructure.ADC_Channel_Number = 1;
    ADC_Init(&ADC_InitStructure);

    /* Configure ADC regular channel10 */
    ADC_RegularChannel_Config(ADC_CHANNEL_10, 1, ADC_SAMPLETIME_55POINT5);
    /* Enable ADC DMA */
    ADC_DMA_Enable(ENABLE);
    ADC_Resolution_Config(ADC_RESOLUTION_12B);
    /* Enable ADC */
    ADC_Enable(ENABLE);
    ADC_Calibration();
    /* Start ADC Software Conversion */
    ADC_SoftwareStartConv_Enable(ENABLE);
}

/**
  * @brief  Configure DMA_6B ports.
  * @param  None
  * @retval None
  */
void DMA_6BConfiguration(void)
{
    DMA_DeInit(DMA1_CHANNEL1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC_RDTR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue6B;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PERIPHERALSRC;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_DISABLE;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PERIPHERALDATASIZE_HALFWORD;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MEMORYDATASIZE_HALFWORD;
    DMA_InitStructure.DMA_Mode = DMA_MODE_CIRCULAR;
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
    DMA_Init(DMA1_CHANNEL1, &DMA_InitStructure);

    /* Enable DMA1 channel1 */
    DMA_Enable(DMA1_CHANNEL1, ENABLE);
}

/**
  * @brief  Configure GPIO ports.
  * @param  None
  * @retval None
  */
void DMA_12BConfiguration(void)
{
    DMA_DeInit(DMA1_CHANNEL1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue12B;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PERIPHERALSRC;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_DISABLE;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PERIPHERALDATASIZE_HALFWORD;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MEMORYDATASIZE_HALFWORD;
    DMA_InitStructure.DMA_Mode = DMA_MODE_CIRCULAR;
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
    DMA_Init(DMA1_CHANNEL1, &DMA_InitStructure);

    /* Enable DMA1 channel1 */
    DMA_Enable(DMA1_CHANNEL1, ENABLE);
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
  * @brief  Insert a delay time.
  * @param  nms: the delay time length, in milliseconds.
  * @retval None
  */
void delay_ms(uint32_t nms)
{
    /* Setup SysTick Timer for 1 ms interrupts */
    if (SysTick_Config(SystemCoreClock / 1000))
    { 
        /* Capture error */ 
        while (1);
    }
    NVIC_SetPriority(SysTick_IRQn, 0x00);
    Time_Delay = nms;
    while(Time_Delay);
    SysTick->CTRL = 0x00;
    SysTick->VAL = 0x00;
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
