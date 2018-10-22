/**
  ******************************************************************************
  * @file    CEC/CEC/gd32f1x0_it.h 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Head file of the Interrupt Service Routines.
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GD32F1X0_IT_H
#define __GD32F1X0_IT_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"

/* Exported function ----------------------------------------------------------*/
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#endif /* __GD32F1X0_IT_H */

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
