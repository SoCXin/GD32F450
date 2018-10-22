/**
  ******************************************************************************
  * @file    I2C/Interrupt_MasterReceiver&SlaveTransmitter/I2C2_IE.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   I2C1_SlaveTransmitter interrupt program.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_i2c.h"
#include "I2C_IE.h"

/* Private variables ---------------------------------------------------------*/
uint32_t    event2;

/* Private function ----------------------------------------------------------*/
/**
  * @brief  Handle I2C2 event interrupt request.
  * @param  None
  * @retval None
  */
void I2C2_EventIRQ_Handler(void)
{
    if(I2C_GetBitState(I2C2, I2C_FLAG_ADDSEND))
    {
        /* Clear the ADDSEND bit */
        event2 = I2C_GetCurrentState(I2C2);
    }
    else if((I2C_GetBitState(I2C2, I2C_FLAG_TBE))&&(!I2C_GetBitState(I2C2, I2C_FLAG_AE)))
    {
        /* Send a data byte */
        I2C_SendData(I2C2,*I2C_WriteBuffer++);
    }
}

/**
  * @brief  Handle I2C2 error interrupt request.
  * @param  None
  * @retval None
  */
void I2C2_ErrorIRQ_Handler(void)
{
    /* No acknowledge received */
    if(I2C_GetBitState(I2C2, I2C_FLAG_AE))
    {
        I2C_ClearBitState(I2C2, I2C_FLAG_AE);
    }

    /* SMBus Alert */
    if(I2C_GetBitState(I2C2, I2C_FLAG_SMBALTS))
    {
        I2C_ClearBitState(I2C2, I2C_FLAG_SMBALTS);
    }

    /* Bus timeout in SMBus mode */
    if(I2C_GetBitState(I2C2, I2C_FLAG_SMBTO))
    {
        I2C_ClearBitState(I2C2, I2C_FLAG_SMBTO);
    }

    /* Over-run or under-run when SCL stretch is disabled */
    if(I2C_GetBitState(I2C2, I2C_FLAG_RXORE))
    {
       I2C_ClearBitState(I2C2, I2C_FLAG_RXORE);
    }

    /* Arbitration lost */
    if(I2C_GetBitState(I2C2, I2C_FLAG_LOSTARB))
    {
        I2C_ClearBitState(I2C2, I2C_FLAG_LOSTARB);
    }

    /* Bus error */
    if(I2C_GetBitState(I2C2, I2C_FLAG_BE))
    {
        I2C_ClearBitState(I2C2, I2C_FLAG_BE);
    }

    /* CRC value doesn't match */
    if(I2C_GetBitState(I2C2, I2C_FLAG_PECE))
    {
        I2C_ClearBitState(I2C2, I2C_FLAG_PECE);
    }

    /* Disable the error interrupt */
    I2C_INTConfig(I2C2,I2C_INT_EIE | I2C_INT_EE | I2C_INT_BIE, DISABLE);
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
