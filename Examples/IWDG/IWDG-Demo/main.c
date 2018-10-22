/**
  ******************************************************************************
  * @file    IWDG/IWDG-Demo/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "gd32f1x0_eval.h"

/* Private variables ---------------------------------------------------------*/
volatile uint32_t g_timedelay=0;
TypeState HSEStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void Delay_ms(uint32_t time);

/* Private function ----------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    /* System Clocks Configuration */
    RCC_Configuration();

    /* LED configuration */
    GD_EVAL_LEDInit(LED1);

    /* Key Configuration */
    GD_EVAL_KeyInit(KEY_TAMPER, KEY_MODE_EXTI);

    if (SysTick_Config(72000000/1000))
    { 
        /* Capture error */ 
        while (1)
        {

        }
    }

    Delay_ms(50);

    /* Enable write access to IWDG_PSR and IWDG_RLDR registers */
    IWDG_Write_Enable(IWDG_WRITEACCESS_ENABLE);

    /* IWDG counter clock: 40KHz(LSI) / 64 = 0.625 KHz */
    IWDG_SetPrescaler(IWDG_PRESCALER_64);

    /* Set counter reload value to 625 */
    IWDG_SetReloadValue(625);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();

    /* Check if the system has resumed from IWDG reset */
    if (RCC_GetBitState(RCC_FLAG_IWDGRST) != RESET)
    {
        GD_EVAL_LEDOn(LED1);

        /* Clear reset flags */
        RCC_ClearBitState();

        while(1);
    }
    else
    {
        GD_EVAL_LEDOff(LED1);
    }

    while (1)
    {

    }
}

/**
  * @brief  Configure the different system clocks
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();

    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);

    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if (HSEStartUpStatus == SUCCESS)
    {
        /* HCLK = SYSCLK */
        RCC_AHBConfig(RCC_SYSCLK_DIV1);

        /* PCLK2 = SYSCLK */
        RCC_APB2Config( RCC_SYSCLK_DIV1);

        /* PCLK1 = SYSCLK */
        RCC_APB1Config( RCC_SYSCLK_DIV1);

        /* Select HSE as system clock source */
        RCC_CK_SYSConfig(RCC_SYSCLKSOURCE_HSE);

        /* Wait till HSE is used as system clock source */
        while (RCC_GetCK_SYSSource() != 0x04)
        {
        }
    }

   /* Enable GPIOC clock */
   RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOC, ENABLE);
}

/**
  * @brief  Insert a delay time
  * @param  None
  * @retval None
  */ 
void Delay_ms(uint32_t time)
{
    g_timedelay = time;
    while(g_timedelay);
}

/************************ (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
