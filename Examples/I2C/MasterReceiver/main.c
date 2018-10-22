/**
  ******************************************************************************
  * @file    I2C/MasterReceiver/main.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#ifdef GD32F130_150
#define BOARD_I2C I2C1
#define I2C_SCL_GPIO_PIN GPIO_PIN_6
#define I2C_SDA_GPIO_PIN GPIO_PIN_7
#define I2C_SCL_GPIO_PINSOURCE GPIO_PINSOURCE6
#define I2C_SDA_GPIO_PINSOURCE GPIO_PINSOURCE7
#define I2C_RCC RCC_APB1PERIPH_I2C1
#elif defined GD32F170_190
#define BOARD_I2C I2C2
#define I2C_SCL_GPIO_PIN GPIO_PIN_10
#define I2C_SDA_GPIO_PIN GPIO_PIN_11
#define I2C_SCL_GPIO_PINSOURCE GPIO_PINSOURCE10
#define I2C_SDA_GPIO_PINSOURCE GPIO_PINSOURCE11
#define I2C_RCC RCC_APB1PERIPH_I2C2
#endif

/* Private variables ---------------------------------------------------------*/
uint8_t BOARD_I2C_Buf_Write[16];
uint8_t BOARD_I2C_Buf_Read[16];

/* Private define ------------------------------------------------------------*/
#define I2C_OWN_ADDRESS7    0x82
#define SLAVE_ADDRESS7       0x72

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void I2C_Configuration(void);

/* Private function  ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    int i;
    
    RCC_Configuration();
    GPIO_Configuration();
    I2C_Configuration();

    for(i=0;i<16;i++)
    {
        BOARD_I2C_Buf_Write[i]=i+0x80;
    }
    /* Enable I2C2 */
    I2C_Enable(BOARD_I2C,ENABLE);

    /* Enable Acknowledge */
    I2C_Acknowledge_Enable(BOARD_I2C,ENABLE);

    /* Wait until I2C Bus is idle */
    while(I2C_GetBitState(BOARD_I2C, I2C_FLAG_I2CBSY));

    /* Send a start condition to I2C bus */
    I2C_StartOnBus_Enable(BOARD_I2C, ENABLE);

    /* Wait until SBSEND bit is set */
    while(!I2C_StateDetect(BOARD_I2C, I2C_PROGRAMMINGMODE_MASTER_SBSEND));

    /* Send slave address to I2C bus*/
    I2C_AddressingDevice_7bit(BOARD_I2C, SLAVE_ADDRESS7, I2C_DIRECTION_RECEIVER);

    /* Wait until ADDSEND bit is set*/
    while(!I2C_StateDetect(BOARD_I2C, I2C_PROGRAMMINGMODE_MASTER_RECEIVER_ADDSEND));
    
    for(i=0;i<16;i++)
    {
        if(i==13)
        {
              /* Wait until the second last data byte is received into the shift register */
              while(!I2C_GetBitState(BOARD_I2C,I2C_FLAG_BTC));
              /* Send a NACK for the last data byte */
              I2C_Acknowledge_Enable(BOARD_I2C, DISABLE);
        }
        /* Wait until the RBNE bit is set */
        while(!I2C_StateDetect(BOARD_I2C, I2C_PROGRAMMINGMODE_MASTER_BYTE_RECEIVED));
        /* Read a data from I2C_DTR */
        BOARD_I2C_Buf_Read[i] = I2C_ReceiveData(BOARD_I2C);
    }

    /* Send a stop condition to I2C bus*/
    I2C_StopOnBus_Enable(BOARD_I2C, ENABLE);
    while(BOARD_I2C->CTLR1&0x0200);
    I2C_Acknowledge_Enable(BOARD_I2C, ENABLE);

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
    /* Enable BOARD_I2C APB1 clock */
    RCC_APB1PeriphClock_Enable(I2C_RCC,ENABLE);
}

/**
  * @brief  Cofigure the GPIO Ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
    GPIO_InitPara GPIO_InitStructure;

    /* BOARD_I2C GPIO ports*/
    GPIO_InitStructure.GPIO_Pin =  I2C_SCL_GPIO_PIN | I2C_SDA_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_PinAFConfig(GPIOB,I2C_SCL_GPIO_PINSOURCE,GPIO_AF_1);
    GPIO_PinAFConfig(GPIOB,I2C_SDA_GPIO_PINSOURCE,GPIO_AF_1);
}

/**
  * @brief  Cofigure the BOARD_I2C interface.
  * @param  None
  * @retval None
  */
void I2C_Configuration(void)
{
    I2C_InitPara I2C_InitStructure;
    I2C_InitStructure.I2C_Protocol = I2C_PROTOCOL_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DUTYCYCLE_2;
    I2C_InitStructure.I2C_BitRate = 100000;
    I2C_InitStructure.I2C_AddressingMode = I2C_ADDRESSING_MODE_7BIT;
    I2C_InitStructure.I2C_DeviceAddress = I2C_OWN_ADDRESS7;
    I2C_Init(BOARD_I2C, &I2C_InitStructure);
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
