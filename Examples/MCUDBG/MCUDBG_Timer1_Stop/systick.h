/**    
  ******************************************************************************    
  * @file    MCUDGB/MCUDBG_Timer1_stop/systick.h    
  * @author  MCU SD    
  * @version V2.0.0    
  * @date    15-Jan-2016    
  * @brief   The header file of the SysTick configuration.    
  ******************************************************************************    
  */    

/* Define to prevent recursive inclusion -------------------------------------*/    
#ifndef __SYS_TICK_H    
#define __SYS_TICK_H    

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported functions ------------------------------------------------------- */    
void SysTick_Configuration(void);    
void Delay_1ms(uint32_t nTime);
void TimingDelay_Decrement(void);   

#endif  /* __SYS_TICK_H */  

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/    
