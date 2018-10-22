/**
  ******************************************************************************
  * @file    CAN/LoopBack/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Main program body.
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include <stdio.h>

/* Private define ------------------------------------------------------------*/
#define __CAN1_USED__

//#define __CAN2_USED__   

#ifdef  __CAN1_USED__
    #define CANX CAN1   /* __CAN1_USED__ */
#else 
    #define CANX CAN2   /* __CAN2_USED__ */
#endif

#ifdef __GNUC__
    /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
       set to 'Yes') calls __io_putchar() */
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private variables ---------------------------------------------------------*/
__IO uint32_t ret = 0; /*!< for return of the interrupt handling */
volatile TestStatus TestRx;

/* Private function prototypes -----------------------------------------------*/
void NVIC_Configuration(void);
void USART2_Config(void);
TestStatus CAN_Polling(void);
TestStatus CAN_Interrupt(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    /* NVIC Configuration */
    NVIC_Configuration();
    USART2_Config();
    printf("loopback test start!\r\n");
    /* CAN transmit at 125Kb/s and receive by polling in loopback mode */
    TestRx = CAN_Polling();

    if (TestRx == FAILED)
    {
        printf("loopback test fail!\r\n");
    }
    else
    {
        printf("loopback test success!\r\n");
    }

    /* CAN transmit at 500Kb/s and receive by interrupt in loopback mode */
    TestRx = CAN_Interrupt();

    if (TestRx == FAILED)
    {
        printf("loopback interrupt test fail!\r\n");
    }
    else
    {
        printf("loopback interrupt test success!\r\n");
    }

    /* Infinite loop */
    while (1)
    {
    }
}

/**
  * @brief  Configure the CAN, transmit and receive by polling
  * @param  None
  * @retval PASSED if the reception is well done, FAILED in other case
  */
TestStatus CAN_Polling(void)
{
    CAN_InitPara        CAN_InitStructure;
    CAN_FilterInitPara  CAN_FilterInitStructure;
    CanTxMessage TxMessage;
    CanRxMessage RxMessage;
    uint32_t i = 0;
    uint8_t TransmitMailbox = 0;
#ifdef  __CAN1_USED__
    /* CANX Periph clock enable */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_CAN1, ENABLE);
#else /*__CAN1_USED__*/
    /* CAN1 & 2  Periph clock enable */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_CAN1, ENABLE);
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_CAN2, ENABLE);
#endif  /* __CAN2_USED__ */
    /* CAN register init */
    CAN_DeInit(CANX);

    CAN_StructInit(&CAN_InitStructure);

    /* CAN cell init */
    CAN_InitStructure.CAN_TTC=DISABLE;
    CAN_InitStructure.CAN_ABOR=DISABLE;
    CAN_InitStructure.CAN_AWK=DISABLE;
    CAN_InitStructure.CAN_ARD=DISABLE;
    CAN_InitStructure.CAN_RFOD=DISABLE;
    CAN_InitStructure.CAN_TFO=DISABLE;
    CAN_InitStructure.CAN_Mode=CAN_MODE_LOOPBACK;
  
    /* Baudrate = 125kbps*/
    CAN_InitStructure.CAN_SJW=CAN_SJW_1TQ;
    CAN_InitStructure.CAN_BS1=CAN_BS1_3TQ;
    CAN_InitStructure.CAN_BS2=CAN_BS2_2TQ;
    CAN_InitStructure.CAN_Prescaler=96;
    CAN_Init(CANX, &CAN_InitStructure);

    /* CAN filter init */
#ifdef  __CAN1_USED__
    CAN_FilterInitStructure.CAN_FilterNumber=0;
#else /*__CAN1_USED__*/
    CAN_FilterInitStructure.CAN_FilterNumber=14;
#endif  /* __CAN2_USED__ */
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FILTERMODE_MASK;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FILTERSCALE_32BIT;
    CAN_FilterInitStructure.CAN_FilterListHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterListLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskListHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskListLow = 0x0000;  
    CAN_FilterInitStructure.CAN_FilterFIFOAssociation = CAN_FIFO0;


    CAN_FilterInitStructure.CAN_FilterWork=ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    /* Transmit */
    TxMessage.StdId = 0x11;
    TxMessage.FT = CAN_FT_DATA;
    TxMessage.FF = CAN_FF_STANDARD;
    TxMessage.DLC = 2;
    TxMessage.Data[0] = 0xCA;
    TxMessage.Data[1] = 0xFE;

    TransmitMailbox=CAN_Transmit(CANX, &TxMessage);
    i = 0;
    while((CAN_TransmitState(CANX, TransmitMailbox) != CANTXOK) && (i != 0xFFFF))
    {
        i++;
    }

    i = 0;
    while((CAN_MessageLength(CANX, CAN_FIFO0) < 1) && (i != 0xFFFF))
    {
        i++;
    }

    /* Receive */
    RxMessage.StdId=0x00;
    RxMessage.FF=0;
    RxMessage.DLC=0;
    RxMessage.Data[0]=0x00;
    RxMessage.Data[1]=0x00;
    CAN_Receive(CANX, CAN_FIFO0, &RxMessage);

    if (RxMessage.StdId!=0x11)
    {
        return FAILED;  
    }

    if (RxMessage.FF!=CAN_FF_STANDARD)
    {
        return FAILED;
    }

    if (RxMessage.DLC!=2)
    {
        return FAILED;  
    }

    if ((RxMessage.Data[0]<<8|RxMessage.Data[1])!=0xCAFE)
    {
        return FAILED;
    }
    /* Test Passed */
    return PASSED; 
}

/**
  * @brief  Configure the CAN, transmit and receive using interrupt.
  * @param  None
  * @retval PASSED if the reception is well done, FAILED in other case
  */
TestStatus CAN_Interrupt(void)
{
    CAN_InitPara        CAN_InitStructure;
    CAN_FilterInitPara  CAN_FilterInitStructure;
    CanTxMessage TxMessage;
    uint32_t i = 0;

    /* CAN register init */
    CAN_DeInit(CANX);

    CAN_StructInit(&CAN_InitStructure);

    /* CAN cell init */
    CAN_InitStructure.CAN_TTC=DISABLE;
    CAN_InitStructure.CAN_ABOR=DISABLE;
    CAN_InitStructure.CAN_AWK=DISABLE;
    CAN_InitStructure.CAN_ARD=DISABLE;
    CAN_InitStructure.CAN_RFOD=DISABLE;
    CAN_InitStructure.CAN_TFO=DISABLE;
    CAN_InitStructure.CAN_Mode=CAN_MODE_LOOPBACK;
    CAN_InitStructure.CAN_SJW=CAN_SJW_1TQ;
  
    /* Baudrate = 500 Kbps */
    CAN_InitStructure.CAN_BS1=CAN_BS1_3TQ;
    CAN_InitStructure.CAN_BS2=CAN_BS2_2TQ;
    CAN_InitStructure.CAN_Prescaler=24;
    CAN_Init(CANX, &CAN_InitStructure);

  /* CAN filter init */
#ifdef  __CAN1_USED__
    CAN_FilterInitStructure.CAN_FilterNumber=1;
#else /*__CAN1_USED__*/
    CAN_FilterInitStructure.CAN_FilterNumber=15;
#endif  /* __CAN2_USED__ */
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FILTERMODE_MASK;
    CAN_FilterInitStructure.CAN_FilterScale=CAN_FILTERSCALE_32BIT;
    CAN_FilterInitStructure.CAN_FilterListHigh=0x0000;
    CAN_FilterInitStructure.CAN_FilterListLow=0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskListHigh=0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskListLow=0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssociation=CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterWork=ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    /* CAN FIFO0 message pending interrupt enable */ 
    CAN_INTConfig(CANX, CAN_INT_RFNE0, ENABLE);

    /* Transmit 1 message */
    TxMessage.StdId=0;
    TxMessage.ExtId=0x1234;
    TxMessage.FF=CAN_FF_EXTENDED;
    TxMessage.FT=CAN_FT_DATA;
    TxMessage.DLC=2;
    TxMessage.Data[0]=0xDE;
    TxMessage.Data[1]=0xCA;
    CAN_Transmit(CANX, &TxMessage);

    /* Initialize the value that will be returned */
    ret = 0xFF;
       
    /* Receive message with interrupt handling */
    i=0;
    while((ret == 0xFF) && (i < 0xFFF))
    {
        i++;
    }
  
    if (i == 0xFFF)
    {
        ret=0;  
    }

    /* Disable interrupt handling */
    CAN_INTConfig(CANX, CAN_INT_RFNE0, DISABLE);

    return (TestStatus)ret;
}

/**
  * @brief  Configure the NVIC and Vector Table base address.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
    NVIC_InitPara NVIC_InitStructure;

    /* Enable CANX RX0 interrupt IRQ channel */
#ifdef  __CAN1_USED__ 
    NVIC_InitStructure.NVIC_IRQ = CAN1_RX0_IRQn;
#else  /*__CAN1_USED__*/
    NVIC_InitStructure.NVIC_IRQ = CAN2_RX0_IRQn;
#endif /*__CAN2_USED__*/
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configure the USART2.
  * @param  None
  * @retval None
  */
void USART2_Config(void)
{
    USART_InitPara USART_InitStructure;  
    GPIO_InitPara  GPIO_InitStructure;    
    /* GPIOA,USART2 clock enable */ 
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_USART2, ENABLE);  
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOA, ENABLE);
     
    /* Configure USART2 pins */
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE2,GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE3,GPIO_AF_1);    
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_2 | GPIO_PIN_3 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
      
    USART_InitStructure.USART_BRR = 115200;
    USART_InitStructure.USART_WL = USART_WL_8B;
    USART_InitStructure.USART_STBits = USART_STBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_RESET;
    USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE;
    USART_InitStructure.USART_RxorTx = USART_RXORTX_RX | USART_RXORTX_TX;

    USART_Init(USART2, &USART_InitStructure);
    /* Enable the USART2 */
    USART_Enable(USART2, ENABLE);                           
}

/**
  * @brief  Retarget the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    USART_DataSend(USART2, (uint8_t) ch);
    /* Loop until the end of transmission */
    while (USART_GetBitState(USART2, USART_FLAG_TBE) == RESET)
    {
    }
    return ch;
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
