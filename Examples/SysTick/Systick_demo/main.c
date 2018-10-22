/**    
  ******************************************************************************    
  * @file    SysTick/Systick_demo/main.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   The main function file.
  ******************************************************************************    
  */    

/* Includes ------------------------------------------------------------------*/    
#include "gd32f1x0.h"
#include <stdio.h>
#include "systick.h"
#include "gd32f1x0_eval.h"

/* Private functions ---------------------------------------------------------*/    

/**
  * @brief  Toggle the LED every 500ms.
  * @param  None
  * @retval None
  */    
void LED_Spark(void)
{    
    static __IO uint32_t TimingDelayLocal = 0;

    if (TimingDelayLocal != 0x00)
    {
        if(TimingDelayLocal < 500)
        {
            GD_EVAL_LEDOff(LED1);
        }
        else
        {
            GD_EVAL_LEDOn(LED1);
        }
        TimingDelayLocal--;
    }
    else
    {
        TimingDelayLocal = 1000;
    }
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    /* Initialize the LED */
    GD_EVAL_LEDInit(LED1);

    SysTick_Configuration();

    while (1)
    {
    }
}




/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/    
