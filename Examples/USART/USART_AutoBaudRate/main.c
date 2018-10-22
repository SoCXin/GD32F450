/**
  ******************************************************************************
  * @file    USART/USART_AutoBaudRate/main.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.  
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "main.h"
#include "gd32f1x0_eval.h"

/* Private define ----------------------------------------------------------- */
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private function prototypes -----------------------------------------------*/
static void USART_Configuration(void);
static void AutoBauRate_StartBitMethod(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{ 
    LED_config();
    /* Configure and enable the systick timer to generate an interrupt each 1 ms */
    SysTick_Config((SystemCoreClock / 100));
    
    /* USART configuration */
    USART_Configuration();
    
    /* AutoBaudRate for USART by Start bit Method */
    AutoBauRate_StartBitMethod();
    
    while(1);
}

/**
  * @brief  Configure the USART Device
  * @param  None
  * @retval None
  */
static void USART_Configuration(void)
{ 
    USART_InitPara USART_InitStructure;
    
    /* USARTx configured as follow:
    - BaudRate = 115200 baud  
    - Word Length = 8 Bits
    - Stop Bit = 1 Stop Bit
    - Parity = No Parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled
    */
    USART_DeInit(USART1);
    USART_InitStructure.USART_BRR = 115200;
    USART_InitStructure.USART_WL = USART_WL_8B;
    USART_InitStructure.USART_STBits = USART_STBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_RESET;
    USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE;
    USART_InitStructure.USART_RxorTx = USART_RXORTX_RX | USART_RXORTX_TX;
    USART_Init(USART1, &USART_InitStructure);
    GD_EVAL_COMInit(&USART_InitStructure);
}

/**
  * @brief  Start Bit Method to USART AutoBaudRate.
  * @param  None
  * @retval None
  */
static void AutoBauRate_StartBitMethod(void)
{ 
    /* USART enable */
    USART_Enable(USART1, ENABLE);
    
    /* Configure the AutoBaudRate method */
    USART_AutoBaudRateDectMode_Set(USART1, USART_AUTOBAUDRATE_STARTBIT);
    
    /* Enable AutoBaudRate feature */
    USART_AutoBaudRateDect_Enable(USART1, ENABLE);
    
    /* Wait until Receive enable acknowledge flag is set */
    while(USART_GetBitState(USART1, USART_FLAG_REA) == RESET)
    {}  
    
    /* Wait until Transmit enable acknowledge flag is set */  
    while(USART_GetBitState(USART1, USART_FLAG_TEA) == RESET)
    {}  
    
    /* Loop until the end of Autobaudrate phase */
    while(USART_GetBitState(USART1, USART_FLAG_ABDF) == RESET)
    {}  
    
    /* If AutoBaudBate error occurred */
    if (USART_GetBitState(USART1, USART_FLAG_ABDE) != RESET)
    {
    
    }
    else
    {
    
        /* Wait until RBNE flag is set */
        while(USART_GetBitState(USART1, USART_FLAG_RBNE) == RESET)
        {}
        
        /* Wait until TBE flag is set */    
        while(USART_GetBitState(USART1, USART_FLAG_TBE) == RESET)
        {}
        
        printf("\n\rUSART Auto baud rate detection finshed successfully!\n\r"); 
        
        /* Check the Transfer Complete Flag */
        while ( USART_GetBitState(USART1, USART_FLAG_TC) == RESET)
        {}
        
        /* clear the TE bit (if a transmission is on going or a data is in the TDR, it will be sent before
        efectivelly disabling the transmission) */
        USART_TransferDirection_Enable(USART1, USART_RXORTX_TX, DISABLE);
        
    }
    
    /* USART Disable */
    USART_Enable(USART1, DISABLE);
}

/**
  * @brief  LED GPIO config.
  * @param  None
  * @retval None
  */
void LED_config(void)
{
GD_EVAL_LEDInit (LED1);
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
    USART_DataSend( USART1 , (uint8_t) ch );
    
    /* Loop until transmit data register is empty */
    while (USART_GetBitState( USART1 , USART_FLAG_TBE) == RESET)
    {}
    
    return ch;
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
