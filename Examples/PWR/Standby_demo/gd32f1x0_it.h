/**
  ******************************************************************************
  * @file    PWR/Standby_demo/gd32f1x0_it.h 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Headfile of the Interrupt Service Routines.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GD32F1X0_IT_H
#define __GD32F1X0_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"

/* Exported functions ------------------------------------------------------- */
void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __GD32F1X0_IT_H */

/************************ (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
