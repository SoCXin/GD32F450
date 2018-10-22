/**
  ******************************************************************************
  * @file    WWDG/WWDG_demo/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_eval.h"

/* Private variables ---------------------------------------------------------*/
__IO uint32_t TimingDelay = 0;

/* Private function prototypes -----------------------------------------------*/
void Delay(uint32_t nTime);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    GD_EVAL_LEDInit(LED1);
    GD_EVAL_LEDInit(LED2);

    GD_EVAL_LEDOn(LED1);
    GD_EVAL_LEDOn(LED2);

    /* Setup SysTick Timer for 1 msec interrupts */
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        /* Capture error */ 
        while (1)
        {}
    }

    /* Check if the system has resumed from WWDG reset */
    if (RCC_GetBitState(RCC_FLAG_WWDGRST) != RESET)
    {
        /* WWDGRST flag set */

        /* Turn on LED */
        GD_EVAL_LEDOn(LED1);

        /* Clear reset flags */
        RCC_ClearBitState();

        while(1);
    }
    else
    {
        /* WWDGRST flag is not set */

        Delay(150);

        /* Turn off LED */
        GD_EVAL_LEDOff(LED1);
    }

    /* WWDG configuration */

    /* Enable WWDG clock */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_WWDG, ENABLE);

    /* WWDG clock counter = (PCLK1 (72MHz)/4096)/8 = 2200Hz (~455 us) */
    WWDG_SetPrescalerValue(WWDG_PRESCALER_8);

    /* Set Window value to 80; WWDG counter should be refreshed only when the counter
       is below 80 (and greater than 64) otherwise a reset will be generated */
    WWDG_SetWindowValue(80);

    /* Enable WWDG and set counter value to 127, WWDG timeout = ~455 us * 64 = 29.2 ms 
       In this case the refresh window is: ~455 * (127-80)= 21.4ms < refresh window < ~455 * 64 =29.2ms */
    WWDG_Enable(127);

    while (1)
    {
        /* Toggle LED */
        GD_EVAL_LEDToggle(LED2);

        /* Insert 25 ms delay */
        Delay(26);

        /* Update WWDG counter */
        WWDG_SetCounterValue(127);
    }
}

/**
  * @brief  Insert a delay time.
  * @param  nTime: specify the delay time length, in 10 ms.
  * @retval None
  */
void Delay(uint32_t nTime)
{
    TimingDelay = nTime;

    while(TimingDelay != 0);
}

/************************ (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
