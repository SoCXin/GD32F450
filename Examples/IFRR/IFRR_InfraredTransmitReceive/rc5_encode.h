/**    
  ******************************************************************************
  * @file    IFRR/IFRR_InfraredTransmitReceive/rc5_encode.h
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   The head file of RC5 Infrared encoder.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RC5_ENCODE_H
#define __RC5_ENCODE_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"

/* Exported types ------------------------------------------------------------*/
/* Definition of the RC5 Control bit value */
typedef enum
{
  RC5_Ctrl_Reset  = ((uint16_t)0),
  RC5_Ctrl_Set    = ((uint16_t)0x0800)
}RC5_Ctrl_TypeDef;

/* Exported functions ------------------------------------------------------- */
void Menu_RC5_Encode_Func(void);
void RC5_Encode_Init(void);
void RC5_Encode_SendFrame(uint8_t RC5_Address, uint8_t RC5_Instruction, RC5_Ctrl_TypeDef RC5_Ctrl);
void RC5_Encode_SignalGenerate(uint32_t RC5_ManchestarFrameFormat);

#endif  /*__RC5_ENCODE_H */

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
