/**
  ******************************************************************************
  * @file    DMA/RAM_USART/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h" 
#include "gd32f1x0_eval.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
/* Private define ------------------------------------------------------------*/
#define BufferSize   (countof(SRC_Const_Buffer) - 1)
#define countof(a)   (sizeof(a) / sizeof(*(a)))
/* Private define ------------------------------------------------------------*/
#ifdef __GNUC__

  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */



/* Private variables ---------------------------------------------------------*/
DMA_InitPara  DMA_InitStructure;
__IO TestStatus TransferStatus = FAILED;
__IO uint32_t count = 0;
__IO uint32_t TimingDelay = 0;

/* Private function prototypes -----------------------------------------------*/
uint8_t SRC_Const_Buffer[]= "\n\r123456789ABCDEF\n\r";
  
void RCC_Configuration(void);
void USART_Configuration(void);
void LED_config(void);  
void NVIC_Configuration(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{          
    /* System Clocks Configuration */
    RCC_Configuration();
    
    /* NVIC configuration */
    NVIC_Configuration();
    
    /* LED configuration */
    LED_config();
    GD_EVAL_LEDOn(LED1);
    GD_EVAL_LEDOn(LED3);
    
    /* USART configuration */
    USART_Configuration(); 
#ifdef GD32F130_150
    /* DMA1 channel2 configuration */
    DMA_DeInit(DMA1_CHANNEL2);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)0x40013828;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SRC_Const_Buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PERIPHERALDST;
    DMA_InitStructure.DMA_BufferSize = BufferSize;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PERIPHERALDATASIZE_BYTE;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MEMORYDATASIZE_BYTE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
    DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_VERYHIGH;
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
    DMA_Init(DMA1_CHANNEL2, &DMA_InitStructure);
      
    USART_DMA_Enable(USART1, USART_DMAREQ_TX, ENABLE);
    
    DMA_INTConfig(DMA1_CHANNEL2, DMA_INT_TC, ENABLE);
    
    /* Enable DMA transfer */
    DMA_Enable(DMA1_CHANNEL2, ENABLE);
#elif defined GD32F170_190    
    /* DMA1 channel2 configuration */
    DMA_DeInit(DMA1_CHANNEL4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)0x40004428;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SRC_Const_Buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PERIPHERALDST;
    DMA_InitStructure.DMA_BufferSize = BufferSize;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PERIPHERALDATASIZE_BYTE;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MEMORYDATASIZE_BYTE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
    DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_VERYHIGH;
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
    DMA_Init(DMA1_CHANNEL4, &DMA_InitStructure);
      
    USART_DMA_Enable(USART2, USART_DMAREQ_TX, ENABLE);
    
    DMA_INTConfig(DMA1_CHANNEL4, DMA_INT_TC, ENABLE);
    
    /* Enable DMA transfer */
    DMA_Enable(DMA1_CHANNEL4, ENABLE);
#endif    
    /* Waiting for the transfer to complete*/
    while(count == 0)
    {
    
    }
    
    GD_EVAL_LEDOff(LED1);
    GD_EVAL_LEDOff(LED3);
    GD_EVAL_LEDOn(LED2);
    GD_EVAL_LEDOn(LED4);
    
    while(1)
    {
        
    }
}

/**
  * @brief  Led configure
  * @param  None
  * @retval None
  */
void LED_config(void)
{
    GD_EVAL_LEDInit (LED1);
    GD_EVAL_LEDInit (LED2);
    GD_EVAL_LEDInit (LED3);
    GD_EVAL_LEDInit (LED4);
}

/**
* @brief  Configure the different system clocks.
* @param  None
* @retval None
*/
void RCC_Configuration(void)
{
    /* Enable peripheral clocks */
    /* Enable DMA1 clock */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_DMA1, ENABLE);
}


/**
* @brief  Configure the USART .
* @param  None
* @retval None
*/
void USART_Configuration(void)
{
    USART_InitPara    USART_InitStructure;
    
    USART_InitStructure.USART_BRR = 9600;
    USART_InitStructure.USART_WL = USART_WL_8B;
    USART_InitStructure.USART_STBits = USART_STBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_RESET;
    USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE;
    USART_InitStructure.USART_RxorTx = USART_RXORTX_RX | USART_RXORTX_TX;
 
    GD_EVAL_COMInit( &USART_InitStructure);
    USART_ClearBitState(EVAL_COM1,USART_FLAG_TC); 
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
    NVIC_InitPara NVIC_InitStructure;
#ifdef GD32F130_150
    /* Enable DMA1 IRQ Channel */
    NVIC_InitStructure.NVIC_IRQ = DMA1_Channel2_3_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);  
#elif defined GD32F170_190
   /* Enable DMA1 IRQ Channel */
    NVIC_InitStructure.NVIC_IRQ = DMA1_Channel4_5_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif    
}

PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_DataSend(EVAL_COM1, (uint8_t) ch);
    
    /* Loop until the end of transmission */
    while (USART_GetBitState(EVAL_COM1, USART_FLAG_TC) == RESET)
    {}
    
    return ch;
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
