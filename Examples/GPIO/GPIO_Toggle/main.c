/**
  *****************************************************************************
  * @file    GPIO/GPIO_Toggle/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  *****************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/ 
#include "gd32f1x0.h"
#include "systick.h"
#include "gd32f1x0_eval.h"

/* Private define ------------------------------------------------------------*/
   
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    SysTick_Configuration();
    GD_EVAL_LEDInit(LED1);
    GD_EVAL_LEDInit(LED2);
    GD_EVAL_LEDInit(LED3);
    GD_EVAL_LEDInit(LED4);

    while(1)
    {
        GD_EVAL_LEDOn(LED1);
        GD_EVAL_LEDOn(LED2);
        GD_EVAL_LEDOn(LED3);
        GD_EVAL_LEDOn(LED4);
        Delay_1ms(500);
       
        GD_EVAL_LEDOff(LED1);
        GD_EVAL_LEDOff(LED2);
        GD_EVAL_LEDOff(LED3);
        GD_EVAL_LEDOff(LED4);
        Delay_1ms(500);

        GD_EVAL_LEDOn(LED1);
        GD_EVAL_LEDOff(LED4);
        Delay_1ms(500);

        GD_EVAL_LEDOn(LED2);
        GD_EVAL_LEDOff(LED1);
        Delay_1ms(500);

        GD_EVAL_LEDOn(LED3);
        GD_EVAL_LEDOff(LED2);
        Delay_1ms(500);

        GD_EVAL_LEDOn(LED4);
        GD_EVAL_LEDOff(LED3);
        Delay_1ms(500);
    }
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
