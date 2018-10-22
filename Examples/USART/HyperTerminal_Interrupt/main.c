/**
  ******************************************************************************
  * @file    USART/HyperTerminal_Interrupt/main.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.  
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "stdio.h"
#include "gd32f1x0_eval.h"

/* Private variables ---------------------------------------------------------*/
USART_InitPara USART_InitStructure;
extern uint8_t NbrOfDataToTransfer;
extern uint8_t NbrOfDataToRead;
extern __IO uint8_t TxCount; 
extern __IO uint16_t RxCount; 
extern uint8_t RxBuffer[32];

/* Private define  ---------------------------------------------------------  */
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private function prototypes -----------------------------------------------*/
void NVIC_Configurationuration(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    USART_InitPara USART_InitStructure;
    
    /* NVIC configuration */
    NVIC_Configurationuration();
    
  
    USART_DeInit( EVAL_COM1 );

    USART_InitStructure.USART_BRR        = 115200;
    USART_InitStructure.USART_WL            = USART_WL_8B;
    USART_InitStructure.USART_STBits            = USART_STBITS_1;
    USART_InitStructure.USART_Parity                = USART_PARITY_RESET;
    USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE;
    USART_InitStructure.USART_RxorTx                = USART_RXORTX_RX | USART_RXORTX_TX;

    GD_EVAL_COMInit(&USART_InitStructure);
    /* Enable the USART1 Transmoit interrupt: this interrupt is generated when the 
    USART1 transmit data register is empty */  
    USART_INT_Set(EVAL_COM1, USART_INT_TBE, ENABLE);
    
    /* Wait until USART1 send the TxBuffer */
    while(TxCount < NbrOfDataToTransfer)
    {}
    
    /* The software must wait until TC=1. The TC flag remains cleared during all data
    transfers and it is set by hardware at the last frame end of transmission*/
    while (USART_GetBitState(EVAL_COM1, USART_FLAG_TC) == RESET)
    {}
    
    /* Enable the USART1 Receive interrupt: this interrupt is generated when the 
    USART1 receive data register is not empty */
    USART_INT_Set(EVAL_COM1, USART_INT_RBNE, ENABLE);
    
    /* Wait until USART1 receive the RxBuffer */
    while(RxCount < NbrOfDataToRead)
    {}
    if(RxCount == NbrOfDataToRead)
    printf("\n\r USART receive successfully!\n\r");
      
    /* Infinite loop */
    while (1)
    {
    }
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configurationuration(void)
{
    NVIC_InitPara NVIC_InitStructure;
    
    /* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQ = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Retarget the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_DataSend( EVAL_COM1 , (uint8_t) ch );
    
    /* Loop until transmit data register is empty */
    while (USART_GetBitState( EVAL_COM1 , USART_FLAG_TBE) == RESET)
    {}
    
    return ch;
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
