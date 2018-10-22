/**
  ******************************************************************************
  * @file    DMA/RAM_RAM/main.h 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Headfie of main program.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include <stdio.h>

/* Private function prototypes -----------------------------------------------*/
void LED_config(void);

#ifdef __GNUC__

    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#endif /* __MAIN_H */

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
