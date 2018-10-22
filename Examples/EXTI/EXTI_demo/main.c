/**
  ******************************************************************************
  * @file    EXTI/EXTI_demo/main.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_eval.h"

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    GD_EVAL_LEDInit(LED1);
    GD_EVAL_LEDOn(LED1);

    /* Configure the User Key */
    GD_EVAL_KeyInit(KEY_WAKEUP, KEY_MODE_EXTI);

    while(1)
    {

    }
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
