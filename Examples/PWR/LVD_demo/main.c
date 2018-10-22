/**
  ******************************************************************************
  * @file    PWR/LVD_demo/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "gd32f1x0_eval.h"
/* Private function prototypes -----------------------------------------------*/
void NVIC_Configuration(void);
void EXTI_Configuration(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    GPIO_InitPara GPIO_InitStructure;
    
    /* Configure the LED1 GPIO */ 
    GD_EVAL_LEDInit(LED1);
    /* Turn on LED1 */
    GD_EVAL_LEDOn(LED1);
    /* Enable PWR clock */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_PWR, ENABLE);
    /* Configure the EXTI line16 */
    EXTI_Configuration();
    /* NVIC configuration */
    NVIC_Configuration();
    /* Configure the LVD threshold to 2.9V(GD32F130_150) or 4.5V(GD32F170_190), and enable the LVD */
    PWR_LVDConfig(PWR_LVDT_7, ENABLE);

    while(1)
    {
    }
}

/**
  * @brief  Configure NVIC and Vector Table base location.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{    
    NVIC_InitPara NVIC_InitStructure;
    /* 1 bits for pre-emption priority and 3 bits for subpriority */
    NVIC_PRIGroup_Enable(NVIC_PRIGROUP_1);
    
    NVIC_InitStructure.NVIC_IRQ = LVD_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configure the selected EXTI line.
  * @param  None
  * @retval None
  */
void EXTI_Configuration(void)
{
    EXTI_InitPara EXTI_InitStructure;
    
    /* Configure EXTI Line16(LVD Output) to generate an interrupt on rising and falling edges */
    EXTI_ClearIntBitState(EXTI_LINE16);
    EXTI_InitStructure.EXTI_LINE = EXTI_LINE16;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LINEEnable = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
