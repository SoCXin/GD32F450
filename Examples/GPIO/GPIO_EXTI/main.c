/**
  *****************************************************************************
  * @file    GPIO/GPIO_EXTI/main.c 
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
    GD_EVAL_KeyInit(KEY_TAMPER,KEY_MODE_EXTI);
    GD_EVAL_LEDInit(LED1);
    while(1)
    {
    }
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
