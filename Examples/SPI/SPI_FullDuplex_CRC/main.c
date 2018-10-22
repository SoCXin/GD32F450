/**
  ******************************************************************************
  * @file    SPI/SPI_FullDuplex_CRC/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************  
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "gd32f1x0_conf.h"

/* Private typedef ------------------------------------------------------------*/
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BufferSize  10

/* Private variables ---------------------------------------------------------*/
uint8_t SPI1_Buffer_Tx[ BufferSize ] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA };
uint8_t SPI2_Buffer_Tx[ BufferSize ] = {0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A };
uint8_t SPI1_Buffer_Rx[ BufferSize ], SPI2_Buffer_Rx[ BufferSize ];
uint32_t TxIdx = 0, RxIdx = 0;
__IO uint16_t CRC1Value = 0, CRC2Value = 0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;

/* Private function prototypes -----------------------------------------------*/
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

/* Private function ----------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{    
    /* Initialization GPIO and SPI */
    GPIO_InitPara GPIO_InitStructure;
    SPI_InitPara  SPI_InitStructure;
    
    /* Enable Peripheral clock */
    RCC_AHBPeriphClock_Enable( RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB | RCC_AHBPERIPH_GPIOC , ENABLE);
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_SPI1,ENABLE);
    RCC_APB1PeriphClock_Enable( RCC_APB1PERIPH_SPI2 ,ENABLE );

    /* Configure SPI1 pins: */
    GPIO_PinAFConfig( GPIOA , GPIO_PINSOURCE5, GPIO_AF_0 );
    GPIO_PinAFConfig( GPIOA , GPIO_PINSOURCE6, GPIO_AF_0 );
    GPIO_PinAFConfig( GPIOA , GPIO_PINSOURCE7, GPIO_AF_0 );
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_5| GPIO_PIN_6 | GPIO_PIN_7  ;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* SPI1 configuration */
    SPI_InitStructure.SPI_TransType  = SPI_TRANSTYPE_FULLDUPLEX;
    SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;;
    SPI_InitStructure.SPI_FrameFormat  = SPI_FRAMEFORMAT_8BIT;;
    SPI_InitStructure.SPI_SCKPL  = SPI_SCKPL_HIGH;
    SPI_InitStructure.SPI_SCKPH = SPI_SCKPH_2EDGE ;
    SPI_InitStructure.SPI_SWNSSEN= SPI_SWNSS_SOFT;
    SPI_InitStructure.SPI_PSC = SPI_PSC_32 ;
    SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;;
    SPI_InitStructure.SPI_CRCPOL  = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    /* Configure SPI2 pins: */
    /* The clock pin of slave spi is set AF mode ,this is important */
    GPIO_PinAFConfig( GPIOB , GPIO_PINSOURCE13, GPIO_AF_0 );
    GPIO_PinAFConfig( GPIOB , GPIO_PINSOURCE14, GPIO_AF_0 );
    GPIO_PinAFConfig( GPIOB , GPIO_PINSOURCE15, GPIO_AF_0 );
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* SPI2 configuration */
    SPI_InitStructure.SPI_TransType  = SPI_TRANSTYPE_FULLDUPLEX;
    SPI_InitStructure.SPI_Mode = SPI_MODE_SLAVE;
    SPI_InitStructure.SPI_SWNSSEN= SPI_SWNSS_SOFT;
    SPI_Init(SPI2, &SPI_InitStructure);

    /* Enable SPI AND SPI2 CRC calculation */
    SPI_CRC_Enable(SPI1, ENABLE);
    SPI_CRC_Enable(SPI2, ENABLE);
    
    /* Enable SPI1 AND SPI2 */
    SPI_Enable(SPI2, ENABLE);
    SPI_Enable(SPI1, ENABLE);
    
    /* Transfer procedure */
    while (TxIdx < BufferSize - 1)
    {
        while (SPI_I2S_GetBitState(SPI2, SPI_FLAG_TBE) == RESET);
        SPI_I2S_SendData(SPI2, SPI2_Buffer_Tx[ TxIdx ] );
        while (SPI_I2S_GetBitState(SPI1, SPI_FLAG_TBE) == RESET);
        SPI_I2S_SendData(SPI1, SPI1_Buffer_Tx[ TxIdx++ ]);

        while (SPI_I2S_GetBitState(SPI2, SPI_FLAG_RBNE) == RESET);
        SPI2_Buffer_Rx[ RxIdx ] = SPI_I2S_ReceiveData( SPI2 );
        while (SPI_I2S_GetBitState(SPI1, SPI_FLAG_RBNE) == RESET);
        SPI1_Buffer_Rx[ RxIdx++ ] = SPI_I2S_ReceiveData( SPI1 );
    }
    while (SPI_I2S_GetBitState(SPI1, SPI_FLAG_TBE) == RESET);
    while (SPI_I2S_GetBitState(SPI2, SPI_FLAG_TBE) == RESET);
    SPI_I2S_SendData(SPI2, SPI2_Buffer_Tx[TxIdx]);
    /* Set Next Data is CRC */
    SPI_SendCRCNext(SPI2);
    SPI_I2S_SendData(SPI1, SPI1_Buffer_Tx[TxIdx]);
    /* Set Next Data is CRC */
    SPI_SendCRCNext(SPI1);

    while (SPI_I2S_GetBitState(SPI1, SPI_FLAG_RBNE) == RESET);
    SPI1_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPI1);
    while (SPI_I2S_GetBitState(SPI2, SPI_FLAG_RBNE) == RESET);
    SPI2_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPI2);
    
    /* Wait for SPI1 data reception: CRC transmitted by SPI2 */
    while (SPI_I2S_GetBitState(SPI1, SPI_FLAG_RBNE) == RESET);
    /* Wait for SPI2 data reception: CRC transmitted by SPI1 */
    while (SPI_I2S_GetBitState(SPI2, SPI_FLAG_RBNE) == RESET);
    
    /* Compare the receive data with send data*/
    TransferStatus1 = Buffercmp(SPI2_Buffer_Rx, SPI1_Buffer_Tx, BufferSize);
    TransferStatus2 = Buffercmp(SPI1_Buffer_Rx, SPI2_Buffer_Tx, BufferSize);
    
    /* Check the SPI1 CRC Error flag */
    if ((SPI_I2S_GetBitState(SPI1, SPI_FLAG_CRCERR)) == SET)
    {
        TransferStatus2 = FAILED;
    }
    /* Check the SPI2 CRC Error flag */
    if ((SPI_I2S_GetBitState(SPI2, SPI_FLAG_CRCERR)) == SET)
    {
        TransferStatus1 = FAILED;
    }
    
    /* Read SPI1 received CRC value */
    CRC1Value = SPI_I2S_ReceiveData(SPI1);
    /* Read SPI2 received CRC value */
    CRC2Value = SPI_I2S_ReceiveData(SPI2);

    /* Disable SPIx and SPIx CRC*/
    SPI_CRC_Enable(SPI1, DISABLE);
    SPI_CRC_Enable(SPI2, DISABLE);
    SPI_Enable(SPI2, DISABLE);
    SPI_Enable(SPI1, DISABLE);    
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1: buffers to be compared.
  * @param  pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
    while (BufferLength--)
    {
        if (*pBuffer1 != *pBuffer2)
        {
            return FAILED;
        }
        pBuffer1++;
        pBuffer2++;
    }
    return PASSED;
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
