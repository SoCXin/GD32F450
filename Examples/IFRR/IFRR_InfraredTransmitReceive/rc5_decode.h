/**    
  ******************************************************************************
  * @file    IFRR/IFRR_InfraredTransmitReceive/rc5_decode.h
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   The head file of RC5 Infrared decoding.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RC5_DECODE_H
#define __RC5_DECODE_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"

/* Exported types ------------------------------------------------------------*/
/** 
  * @brief  RC5 frame structure  
  */
typedef struct
{   
    __IO uint8_t FieldBit;   /*!< Field bit */   
    __IO uint8_t ToggleBit;  /*!< Toggle bit field */
    __IO uint8_t Address;    /*!< Address field */ 
    __IO uint8_t Command;    /*!< Command field */ 
  
} RC5_Frame_TypeDef;

/** 
  * @brief  RC5 packet structure
  */
typedef struct
{
    __IO uint16_t data;     /*!< RC5 data */ 
    __IO uint8_t  status;   /*!< RC5 status */
    __IO uint8_t  lastBit;  /*!< RC5 last bit */
    __IO uint8_t  bitCount; /*!< RC5 bit count */
} tRC5_packet;

enum RC5_lastBitType
{
    RC5_ZER,
    RC5_ONE,
    RC5_NAN,
    RC5_INV
};

typedef enum RC5_lastBitType tRC5_lastBitType;

/* Exported define ---------------------------------------------------------- */
#define RC5_1T_TIME                          0x00
#define RC5_2T_TIME                          0x01
#define RC5_WRONG_TIME                       0xFF
#define RC5_TIME_OUT_US                      3600
/* Half bit period */
#define RC5_T_US                             900
/* Tolerance time */
#define RC5_T_TOLERANCE_US                   270    
#define RC5_NUMBER_OF_VALID_PULSE_LENGTH     2
/* Total bits */
#define RC5_PACKET_BIT_COUNT                 13      

/* Packet struct for reception */
#define RC5_PACKET_STATUS_EMPTY              1<<0

/* Exported functions ------------------------------------------------------- */
void Menu_RC5Decode_Func(void);
void RC5_Decode_DeInit(void);
void RC5_Decode_Init(void);
void RC5_Decode(RC5_Frame_TypeDef *rc5_frame);
void RC5_ResetPacket(void);
void RC5_DataSampling(uint16_t rawPulseLength, uint8_t edge);

#endif /* __RC5_DECODE_H */

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/


