/**
  ******************************************************************************
  * @file    RTC/Calendar/main.h 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Headfile of main program
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"

#include <stdio.h>  

/* Exported functions --------------------------------------------------------*/
void RTC_PreConfiguration(void);
void RTC_Setup(void);
void RTC_ShowTime(void);
void RTC_ShowAlarm(void);
uint8_t USART_Input_Threshold(uint32_t value);

#ifdef __GNUC__
    #define OUTPUTCHAR_PROTOTYPE int __io_putchar(int character)
#else
    #define OUTPUTCHAR_PROTOTYPE int fputc(int character, FILE *file)
#endif /* __GNUC__ */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
