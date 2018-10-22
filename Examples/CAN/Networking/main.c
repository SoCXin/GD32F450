/**
  ******************************************************************************
  * @file    CAN/Networking/main.c
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

#define USE_CAN_NUM                    1          /*!< 1 or 2 */

#define KEY_GPIO_PORT                  GPIOC
#define KEY_GPIO_PIN                   GPIO_PIN_13
#define KEY_PRESSED                    0
#define KEY_NOT_PRESSED                1

#if (1 == USE_CAN_NUM) 
    #define CANX                       CAN1
#else /*__CAN1_USED__*/
    #define CANX                       CAN2
#endif /*__CAN2_USED__*/

/* Private variables ---------------------------------------------------------*/
CAN_InitPara                CAN_InitStructure;
CAN_FilterInitPara    CAN_FilterInitStructure;
CanTxMessage  TxMessage;
uint8_t KeyNumber = 0x0;
extern CanRxMessage  RxMessage;
extern TypeState g_recvflag; 

/* Private function prototypes -----------------------------------------------*/
void NVIC_Config(void);
void CAN_Config(void);
void Delay(void);
void USART2_Config(void);
void GPIO_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{         
    /* NVIC configuration */
    NVIC_Config();
    GPIO_Config();
    USART2_Config();
    /* CAN configuration */
    CAN_Config(); 
#if (1 == USE_CAN_NUM) 
    CAN_PHY_Enable(CAN1, ENABLE);
#else /*__CAN1_USED__*/
    CAN_PHY_Enable(CAN1, DISABLE);
#endif /* __CAN2_USED__ */       
    CAN_INTConfig(CANX, CAN_INT_RFNE0, ENABLE);
    printf("GD32F1x0 CAN networking test\r\n");
    printf("Press Tamper button to transmit data\r\n");
    /* Transmit */
    TxMessage.StdId = 0x321;
    TxMessage.ExtId = 0x01;
    TxMessage.FT = CAN_FT_DATA;
    TxMessage.FF = CAN_FF_STANDARD;
    TxMessage.DLC = 1;

    /* Infinite loop */
    while(1)
    {
        while(GPIO_ReadInputBit(KEY_GPIO_PORT, KEY_GPIO_PIN) == KEY_PRESSED)
        {
            if(KeyNumber == 0x10) 
            {
                KeyNumber = 0x00;
            }
            else
            {
                TxMessage.Data[0] = KeyNumber++;
                printf("Transmit data: %x\r\n", TxMessage.Data[0]);
                CAN_Transmit(CANX, &TxMessage);
                
                Delay();
                while(GPIO_ReadInputBit(KEY_GPIO_PORT, KEY_GPIO_PIN) == KEY_PRESSED)
                {
                }
            }
        }
        if(SET == g_recvflag)
        {
            g_recvflag = RESET;
            printf("Recive data: %x\r\n", RxMessage.Data[0]);
        }
    }  
}

/**
  * @brief  Configure the CAN.
  * @param  None
  * @retval None
  */
void CAN_Config(void)
{  
    /* CAN register init */
    CAN_DeInit(CANX);
    CAN_StructInit(&CAN_InitStructure);

#if (1 == USE_CAN_NUM) 
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_CAN1, ENABLE);
#else /*__CAN1_USED__*/
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_CAN1, ENABLE);
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_CAN2, ENABLE);
#endif /* __CAN2_USED__ */
    
    /* CAN cell init */
    CAN_InitStructure.CAN_TTC = DISABLE;
    CAN_InitStructure.CAN_ABOR = DISABLE;
    CAN_InitStructure.CAN_AWK = DISABLE;
    CAN_InitStructure.CAN_ARD = DISABLE;
    CAN_InitStructure.CAN_RFOD = DISABLE;
    CAN_InitStructure.CAN_TFO = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_MODE_NORMAL;
  
    /* CAN Baudrate = 1MBps*/
    CAN_InitStructure.CAN_SJW = CAN_SJW_1TQ;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_3TQ;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_2TQ;
    CAN_InitStructure.CAN_Prescaler = 12; 
    CAN_Init(CANX, &CAN_InitStructure);

    /* CAN filter init */
#if (1 == USE_CAN_NUM) 
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
#else /*__CAN1_USED__*/
    CAN_FilterInitStructure.CAN_FilterNumber = 14;
#endif /*__CAN2_USED__*/
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FILTERMODE_MASK;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FILTERSCALE_32BIT;
    CAN_FilterInitStructure.CAN_FilterListHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterListLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskListHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskListLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssociation = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterWork = ENABLE;
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
  
#if (1 == USE_CAN_NUM)  
    NVIC_InitStructure.NVIC_IRQ = CAN1_RX0_IRQn;
#else  /*__CAN1_USED__*/
    NVIC_InitStructure.NVIC_IRQ = CAN2_RX0_IRQn;
#endif /*__CAN2_USED__*/

    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
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
    
    #if (1 == USE_CAN_NUM)  
    /* Configure CAN1 pins */
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    #else  /*__CAN1_USED__*/
    /* Configure CAN2 RX pin */
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE12,GPIO_AF_9);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  
    /* Configure CAN2 TX pin */
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE13,GPIO_AF_9);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    #endif /*__CAN2_USED__*/
    
    /* Configure Tamper Button pin */
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_Init(GPIOC, &GPIO_InitStructure);    
}

/**
  * @brief  Delay
  * @param  None
  * @retval None
  */
void Delay(void)
{
    uint16_t nTime = 0x0000;

    for(nTime = 0; nTime <0xFFF; nTime++)
    {
    }
}

/**
  * @brief  Configure the USART2.
  * @param  None
  * @retval None
  */
void USART2_Config(void)
{
    USART_InitPara USART_InitStructure;
    /* Configure the USART2*/ 
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
