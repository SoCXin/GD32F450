/**
  ******************************************************************************
  * @file    I2C/Interrupt_MasterReceiver&SlaveTransmitter/main.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "I2C_IE.h"

/* Private variables ---------------------------------------------------------*/
uint8_t I2C_Buf_Write[16];
uint8_t I2C_Buf_Read[16];

volatile uint8_t*       I2C_WriteBuffer;
volatile uint8_t*       I2C_ReadBuffer;
volatile uint16_t       I2C_nBytes;
volatile OP_Status      Status;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void I2C_GPIO_Configuration(void);
void I2C_Configuration(void);
void I2C_NVIC_Configuraion(void);

/* Private function  ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    int i;
    
    RCC_Configuration();
    I2C_GPIO_Configuration();
    I2C_Configuration();
    I2C_NVIC_Configuraion();

    for(i=0;i<16;i++)
    {
        I2C_Buf_Write[i]=i+0x80;
    }

    I2C_WriteBuffer=I2C_Buf_Write;
    I2C_ReadBuffer=I2C_Buf_Read;
    I2C_nBytes=16;
    Status=start;

    /* Enable the I2C1 interrupt */
    I2C_INTConfig(I2C1, I2C_INT_EIE |I2C_INT_EE| I2C_INT_BIE, ENABLE);

    /* Enable the I2C2 interrupt */
    I2C_INTConfig(I2C2, I2C_INT_EIE |I2C_INT_EE| I2C_INT_BIE, ENABLE);

    if(I2C_nBytes==2)
    {
        /* Set POAP bit before set GENSTA bit */
        /* Set the NACK for the next data byte which will be received into the shift register */
        I2C_NACKPosition_Enable(I2C1,I2C_NACKPOSITION_NEXT);
    }
    /* The master waits until the I2C bus is idle*/
    while(I2C_GetBitState(I2C1, I2C_FLAG_I2CBSY));

    /* The master sends a start condition to I2C bus */
    I2C_StartOnBus_Enable(I2C1, ENABLE);
    
    while(I2C_nBytes>0)
    {
    }
    while(Status!=end)
    {
    }
    while(1);
}

 /**
  * @brief  Enable the peripheral clock.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
    /* Enable GPIOB clock */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOB,ENABLE);

    /* I2C1 APB1 clock */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_I2C1,ENABLE);

    /* I2C2 APB1 clock */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_I2C2,ENABLE);
}

/**
  * @brief  Cofigure the GPIO Ports.
  * @param  None
  * @retval None
  */
void I2C_GPIO_Configuration(void)
{
    GPIO_InitPara GPIO_InitStructure;

    /* I2C1 and I2C2 GPIO ports */
    GPIO_InitStructure.GPIO_Pin =  GPIO_PIN_6 | GPIO_PIN_7|GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE6,GPIO_AF_1);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE7,GPIO_AF_1);

    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE10,GPIO_AF_1);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE11,GPIO_AF_1);
}

/**
  * @brief  Cofigure the I2C1 and I2C2 interfaces.
  * @param  None
  * @retval None
  */
void I2C_Configuration(void)
{
    /* I2C1 and I2C2 periphral configuration */
    I2C_InitPara I2C_InitStructure;
    /* I2C1 */
    I2C_InitStructure.I2C_Protocol = I2C_PROTOCOL_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DUTYCYCLE_2;
    I2C_InitStructure.I2C_BitRate = 100000;
    I2C_InitStructure.I2C_AddressingMode = I2C_ADDRESSING_MODE_7BIT;
    I2C_InitStructure.I2C_DeviceAddress = I2C1_SLAVE_ADDRESS7;
    
    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Enable(I2C1,ENABLE);
    I2C_Acknowledge_Enable(I2C1,ENABLE);

    /* I2C2 */
    I2C_InitStructure.I2C_DeviceAddress = I2C2_SLAVE_ADDRESS7;
    I2C_Init(I2C2,&I2C_InitStructure);
    I2C_Enable(I2C2,ENABLE);
    I2C_Acknowledge_Enable(I2C2,ENABLE);
}

/**
  * @brief  Cofigure the NVIC peripheral.
  * @param  None
  * @retval None
  */
void I2C_NVIC_Configuraion(void)
{
    NVIC_InitPara NVIC_InitStructure;

    /* 1 bits for pre-emption priority and 3 bits for subpriority */
    NVIC_PRIGroup_Enable(NVIC_PRIGROUP_1);

    /* I2C1 event interrupt */
    NVIC_InitStructure.NVIC_IRQ = I2C1_EV_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* I2C1 error interrupt */
    NVIC_InitStructure.NVIC_IRQ = I2C1_ER_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* I2C2 event interrupt */
    NVIC_InitStructure.NVIC_IRQ = I2C2_EV_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* I2C2 error interrupt */
    NVIC_InitStructure.NVIC_IRQ = I2C2_ER_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
