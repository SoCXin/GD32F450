/**
  ******************************************************************************
  * @file    USART/Printf/main.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.  
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include <stdio.h>
#include "gd32f1x0_eval.h"

/* Private define ------------------------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{    
    USART_InitPara USART_InitStructure;

    USART_DeInit( EVAL_COM1 );

    USART_InitStructure.USART_BRR           = 115200;
    USART_InitStructure.USART_WL            = USART_WL_8B;
    USART_InitStructure.USART_STBits            = USART_STBITS_1;
    USART_InitStructure.USART_Parity                = USART_PARITY_RESET;
    USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE;
    USART_InitStructure.USART_RxorTx                = USART_RXORTX_RX | USART_RXORTX_TX;

    GD_EVAL_COMInit(&USART_InitStructure);
    
    /* Output a message on Hyperterminal using printf function */
    printf("\n\rUSART Printf Example: retarget the C library printf function to the USART\n\r");
    
    /* Loop until the end of transmission */
    /* The software must wait until TC=1. The TC flag remains cleared during all data
     transfers and it is set by hardware at the last frame end of transmission*/
    while ( USART_GetBitState( EVAL_COM1 , USART_FLAG_TC ) == RESET )
    {
    }
    while (1)
    {
    }
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
    {
    }

    return ch;
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
