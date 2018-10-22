/**
  ******************************************************************************
  * @file    I2C/Interrupt_MasterReceiver&SlaveTransmitter/I2C_IE.h
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   The header file of I2C1 and I2C2 interrupt.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _I2C_IE_H
#define _I2C_IE_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"

/* Exported macro ------------------------------------------------------------*/
#define I2C1_SLAVE_ADDRESS7    0x82
#define I2C2_SLAVE_ADDRESS7    0x72

/* Exported types ------------------------------------------------------------*/
typedef enum {start = 0, end = !start} OP_Status;

/* Exported variables --------------------------------------------------------*/
extern  volatile OP_Status    Status;
extern  volatile uint8_t*     I2C_WriteBuffer;
extern  volatile uint8_t*     I2C_ReadBuffer;
extern  volatile uint16_t     I2C_nBytes;

/* Exported functions ------------------------------------------------------- */
void I2C1_EventIRQ_Handler(void);
void I2C1_ErrorIRQ_Handler(void);

void I2C2_EventIRQ_Handler(void);
void I2C2_ErrorIRQ_Handler(void);

#endif /* _I2C_IE_H */

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/

