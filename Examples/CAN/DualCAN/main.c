/**
  ******************************************************************************
  * @file    CAN/DualCAN/main.c
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
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#define CAN_BAUDRATE  1000      /* 1MBps   */
/* #define CAN_BAUDRATE  500*/  /* 500kBps */
/* #define CAN_BAUDRATE  250*/  /* 250kBps */
/* #define CAN_BAUDRATE  125*/  /* 125kBps */
/* #define CAN_BAUDRATE  100*/  /* 100kBps */ 
/* #define CAN_BAUDRATE  50*/   /* 50kBps  */ 
/* #define CAN_BAUDRATE  20*/   /* 20kBps  */ 


/* Private variables ---------------------------------------------------------*/
CAN_InitPara        CAN_InitStructure;
CAN_FilterInitPara  CAN_FilterInitStructure;
CanTxMessage TxMessage;
TypeState g_can1_recvflag;
TypeState g_can2_recvflag;
TypeState g_can1_errflag;
TypeState g_can2_errflag;

extern CanRxMessage RxMessage;
/* Private function prototypes -----------------------------------------------*/
void NVIC_Config(void);
void CAN_Config(void);
void Delay(void);
void GPIO_Config(void);
void USART2_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    g_can1_recvflag = RESET;
    g_can2_recvflag = RESET;
    g_can1_errflag = RESET;
    g_can2_errflag = RESET;
        
    /* Configure GPIO */
    GPIO_Config(); 
    /* Configure USART2 */
    USART2_Config(); 
    
    printf("\r\nGD32F1x0 Dual CAN\r\n");
    printf("To start Press on:  \r\n");
    printf("Wakeup or Tamper Button\r\n");

#if CAN_BAUDRATE == 1000 
    printf("BAUDRATE = 1MBps   ");
#elif CAN_BAUDRATE == 500 
    printf("BAUDRATE = 500kBps   ");
#elif CAN_BAUDRATE == 250 
    printf("BAUDRATE = 250kBps   ");
#elif CAN_BAUDRATE == 125 
    printf("BAUDRATE = 125kBps   ");
#elif  CAN_BAUDRATE == 100 
    printf("BAUDRATE = 100kBps   ");
#elif  CAN_BAUDRATE == 50 
    printf("BAUDRATE = 50kBps   ");
#elif  CAN_BAUDRATE == 20
    printf("BAUDRATE = 20kBps   ");
#endif
    /* NVIC configuration */
    NVIC_Config();
    
    /* CANs configuration */
    CAN_Config();
    CAN_PHY_Enable(CAN1, ENABLE);
    /* IT Configuration for CAN1 */  
    CAN_INTConfig(CAN1, CAN_INT_RFNE0, ENABLE);    
    /* IT Configuration for CAN2 */  
    CAN_INTConfig(CAN2, CAN_INT_RFNE0, ENABLE);
    /* Transmit */
    TxMessage.StdId = 0x300>>1;
    TxMessage.ExtId = 0x00;
    TxMessage.FT = CAN_FT_DATA;
    TxMessage.FF = CAN_FF_STANDARD;
    TxMessage.DLC = 2;
    /* Infinite loop */
    while(1)
    {
        if(GPIO_ReadInputBit(GPIOC, GPIO_PIN_13) == RESET)   
        {
            TxMessage.Data[0] = 0x55;
            TxMessage.Data[1] = 0xAA;
            printf("\r\n data:%x,%x CAN1 Transmitting...", TxMessage.Data[0],TxMessage.Data[1]);
            CAN_Transmit(CAN1, &TxMessage);
        
            /* Loop while Tamper button is pressed */
            while(GPIO_ReadInputBit(GPIOC, GPIO_PIN_13) == RESET)
            {
            }
        }
        if(GPIO_ReadInputBit(GPIOA, GPIO_PIN_0) == RESET)   
        {
            TxMessage.Data[0] = 0xAA;
            TxMessage.Data[1] = 0x55;
            printf("\r\n data:%x,%x CAN2 Transmitting...", TxMessage.Data[0],TxMessage.Data[1]);  
            CAN_Transmit(CAN2, &TxMessage);

            /* Loop while Wakeup button is pressed */
            while(GPIO_ReadInputBit(GPIOA, GPIO_PIN_0) == RESET)
            {
            }
        }
        if(SET == g_can1_recvflag)
        {
            g_can1_recvflag = RESET;
            printf("\r\n CAN1 receive:%x,%x",RxMessage.Data[0],RxMessage.Data[1]);
        }
        if(SET == g_can2_recvflag)
        {
            g_can2_recvflag = RESET;
            printf("\r\n CAN2 receive:%x,%x",RxMessage.Data[0],RxMessage.Data[1]);
        }
        if(SET == g_can1_errflag)
        {
            g_can1_errflag = RESET;
            printf("\r\n CAN1 communication error!");
        }
        if(SET == g_can2_errflag)
        {
            g_can2_errflag = RESET;
            printf("\r\n CAN2 communication error!");
        }
    }
}

/**
  * @brief  Configure GPIO.
  * @param  None
  * @retval None
  */
void GPIO_Config(void)
{
    GPIO_InitPara  GPIO_InitStructure;
    /* GPIO clocks enable */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOA | 
                              RCC_AHBPERIPH_GPIOB | 
                              RCC_AHBPERIPH_GPIOC | 
                              RCC_AHBPERIPH_GPIOD, ENABLE);

    /* Configure USART2 pins */
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE2,GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE3,GPIO_AF_1);    
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Configure CAN1 pins */
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure CAN2 RX pin */
   
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE12,GPIO_AF_9);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  
    /* Configure CAN2 TX pin */
    
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE13,GPIO_AF_9);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
    /* Configure Tamper Button pin */
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    /* Configure Wakeup Key Button pin */
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);    
}

/**
  * @brief  Configure CAN1 and CAN2.
  * @param  None
  * @retval None
  */
void CAN_Config(void)
{

    /* Configure CAN1 and CAN2 */  
    /* CAN1 and CAN2 Periph clocks enable */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_CAN1 | RCC_APB1PERIPH_CAN2, ENABLE);  
  
    /* CAN1 and CAN2 register init */
    CAN_DeInit(CAN1);
    CAN_DeInit(CAN2);

    /* Struct init*/
    CAN_StructInit(&CAN_InitStructure);

    /* CAN1 and CAN2  cell init */
    CAN_InitStructure.CAN_TTC = DISABLE;
    CAN_InitStructure.CAN_ABOR = DISABLE;
    CAN_InitStructure.CAN_AWK = DISABLE;
    CAN_InitStructure.CAN_ARD = DISABLE;
    CAN_InitStructure.CAN_RFOD = DISABLE;
    CAN_InitStructure.CAN_TFO = ENABLE;
    CAN_InitStructure.CAN_Mode = CAN_MODE_NORMAL;
    CAN_InitStructure.CAN_SJW = CAN_SJW_1TQ;  
    CAN_InitStructure.CAN_BS1 = CAN_BS1_3TQ;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_2TQ;
    /* 1MBps */
#if CAN_BAUDRATE == 1000
    CAN_InitStructure.CAN_Prescaler =12;
    /* 500KBps */
#elif CAN_BAUDRATE == 500 
    CAN_InitStructure.CAN_Prescaler =24;
    /* 250KBps */
#elif CAN_BAUDRATE == 250 
    CAN_InitStructure.CAN_Prescaler =48;
    /* 125KBps */
#elif CAN_BAUDRATE == 125 
    CAN_InitStructure.CAN_Prescaler =96;
    /* 100KBps */
#elif  CAN_BAUDRATE == 100 
    CAN_InitStructure.CAN_Prescaler =120;
    /* 50KBps */
#elif  CAN_BAUDRATE == 50 
    CAN_InitStructure.CAN_Prescaler =240;
    /* 20KBps */
#elif  CAN_BAUDRATE == 20 
    CAN_InitStructure.CAN_Prescaler =600;
#else
    #error "Please select first the CAN Baudrate in Private defines in main.c "
#endif  /* CAN_BAUDRATE == 1000 */
  
    /*Initialize the CAN1 and CAN2 */
    CAN_Init(CAN1, &CAN_InitStructure);
    CAN_Init(CAN2, &CAN_InitStructure);

    /* CAN1 filter init */
    CAN_FilterInitStructure.CAN_FilterNumber = 1;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FILTERMODE_MASK;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FILTERSCALE_32BIT;
    CAN_FilterInitStructure.CAN_FilterListHigh = 0x3000;
    CAN_FilterInitStructure.CAN_FilterListLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskListHigh = 0x3000;
    CAN_FilterInitStructure.CAN_FilterMaskListLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssociation = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterWork = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
  
    /* CAN2 filter init */
    CAN_FilterInitStructure.CAN_FilterNumber = 15;
    CAN_FilterInit(&CAN_FilterInitStructure);  
}

/**
  * @brief  Configure the NVIC for CAN.
  * @param  None
  * @retval None
  */
void NVIC_Config(void)
{
    NVIC_InitPara NVIC_InitStructure;

    NVIC_PRIGroup_Enable(NVIC_PRIGROUP_0);

    /* Enable CAN1 RX0 interrupt IRQ channel */
    NVIC_InitStructure.NVIC_IRQ = CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable CAN2 RX0 interrupt IRQ channel */
    NVIC_InitStructure.NVIC_IRQ = CAN2_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 1;
    NVIC_InitStructure.NVIC_IRQSubPriority = 1;
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
    /* Configure the USART2 */ 
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_USART2, ENABLE); 
    
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
