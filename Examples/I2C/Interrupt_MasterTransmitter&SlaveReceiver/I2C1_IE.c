/**
  ******************************************************************************
  * @file    I2C/Interrupt_MasterTransmitter&SlaveReceiver/I2C1_IE.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   I2C1_MasterTransmitter interrupt program.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_i2c.h"
#include "I2C_IE.h"

/* Private variables ---------------------------------------------------------*/
uint32_t    event1;

/* Private function ---------------------------------------------------------*/
/**
  * @brief  Handle I2C1 event interrupt request.
  * @param  None
  * @retval None
  */
void I2C1_EventIRQ_Handler(void)
{
    if(I2C_GetBitState(I2C1, I2C_FLAG_SBSEND))
    {
        /* Send slave address */
        I2C_AddressingDevice_7bit(I2C1, I2C2_SLAVE_ADDRESS7, I2C_DIRECTION_TRANSMITTER);
    }
    else if(I2C_GetBitState(I2C1, I2C_FLAG_ADDSEND))
    {
        /*Clear ADDSEND bit */
        event1 = I2C_GetCurrentState(I2C1);
    }
    else if(I2C_GetBitState(I2C1, I2C_FLAG_TBE))
    {
        if(I2C_nBytes>0)
        {
            /* The master sends a data byte */
            I2C_SendData(I2C1,*I2C_WriteBuffer++);
            I2C_nBytes--;
        }
        else
        {
            /* The master sends a stop condition to I2C bus */
            I2C_StopOnBus_Enable(I2C1, ENABLE);
            /* Disable the I2C1 interrupt */
            I2C_INTConfig(I2C1,I2C_INT_EIE | I2C_INT_EE | I2C_INT_BIE, DISABLE);
        }
    }

}

/**
  * @brief  Handle I2C1 error interrupt request.
  * @param  None
  * @retval None
  */
void I2C1_ErrorIRQ_Handler(void)
{
    /* No acknowledge received */
    if(I2C_GetBitState(I2C1, I2C_FLAG_AE))
    {
        I2C_ClearBitState(I2C1, I2C_FLAG_AE);
    }

    /* SMBus Alert */
    if(I2C_GetBitState(I2C1, I2C_FLAG_SMBALTS))
    {
        I2C_ClearBitState(I2C1, I2C_FLAG_SMBALTS);
    }

    /* Bus timeout in SMBus mode */
    if(I2C_GetBitState(I2C1, I2C_FLAG_SMBTO))
    {
        I2C_ClearBitState(I2C1, I2C_FLAG_SMBTO);
    }

    /* Over-run or under-run when SCL stretch is disabled */
    if(I2C_GetBitState(I2C1, I2C_FLAG_RXORE))
    {
        I2C_ClearBitState(I2C1, I2C_FLAG_RXORE);
    }

    /* Arbitration lost */
    if(I2C_GetBitState(I2C1, I2C_FLAG_LOSTARB))
    {
        I2C_ClearBitState(I2C1, I2C_FLAG_LOSTARB);
    }

    /* Bus error */
    if(I2C_GetBitState(I2C1, I2C_FLAG_BE))
    {
        I2C_ClearBitState(I2C1, I2C_FLAG_BE);
    }

    /* CRC value doesn't match */
    if(I2C_GetBitState(I2C1, I2C_FLAG_PECE))
    {
        I2C_ClearBitState(I2C1, I2C_FLAG_PECE);
    }

    /* Disable the error interrupt */
    I2C_INTConfig(I2C1,I2C_INT_EIE | I2C_INT_EE | I2C_INT_BIE, DISABLE);
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/

