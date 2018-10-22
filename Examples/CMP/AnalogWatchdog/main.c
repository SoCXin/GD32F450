/**
  ******************************************************************************
  * @file    CMP/AnalogWatchdog/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "stdio.h"
#include "gd32f1x0_eval.h"

/* Private function prototypes -----------------------------------------------*/
typedef enum
{
    STATE_OVER_THRESHOLD ,
    STATE_WITHIN_THRESHOLD, 
    STATE_UNDER_THRESHOLD 
} STATE;

void CMP_Config(void);
void DeepsleepEntry(void);
STATE CheckState(void);

uint32_t g_delayms;

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Comparator configuration.
  * @param  None
  * @retval None
  */
void CMP_Config(void)
{
    CMP_InitPara CMP_InitParaStruct,CMP_InitParaStruct1;
    GPIO_InitPara GPIO_InitStruct;
    EXTI_InitPara EXTI_InitStruct;
    NVIC_InitPara NVIC_InitStruct;

    /* GPIOA Peripheral clock enable */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOA,ENABLE);

    /* Configure PA1 as comparator input */
    GPIO_ParaInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_PIN_1 ;
    GPIO_InitStruct.GPIO_Mode  = GPIO_MODE_AN;
    GPIO_InitStruct.GPIO_Speed =  GPIO_SPEED_50MHZ;
    GPIO_InitStruct.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStruct.GPIO_PuPd  =GPIO_PUPD_PULLUP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Comparator Peripheral clock enable */
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_CMP ,ENABLE);

    /* Configure comparator*/
    CMP_ParaInit(&CMP_InitParaStruct);
    CMP_InitParaStruct.CMP_InvertingInput = CMP_INVERTINGINPUT_VREFINT;
    CMP_InitParaStruct.CMP_Output = CMP_OUTPUT_NONE;
    CMP_InitParaStruct.CMP_Hysteresis = CMP_HYSTERESIS_HIGH;
    CMP_InitParaStruct.CMP_OperatingMode = CMP_OPERATINGMODE_LOWSPEED;
    CMP_Init(CMP_CHANNEL_CMP1, &CMP_InitParaStruct);

    CMP_ParaInit(&CMP_InitParaStruct1);
    CMP_InitParaStruct1.CMP_InvertingInput = CMP_INVERTINGINPUT_1_2VREFINT;
    CMP_InitParaStruct1.CMP_Output = CMP_OUTPUT_NONE;
    CMP_InitParaStruct1.CMP_Hysteresis = CMP_HYSTERESIS_HIGH;
    CMP_InitParaStruct1.CMP_OperatingMode = CMP_OPERATINGMODE_LOWSPEED;
    CMP_Init(CMP_CHANNEL_CMP2, &CMP_InitParaStruct1);

    /* Configure EXTI Line 21 in interrupt mode */
    EXTI_InitStruct.EXTI_LINE = EXTI_LINE21 ;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStruct.EXTI_LINEEnable = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    /* Configure EXTI Line 22 in interrupt mode */
    EXTI_InitStruct.EXTI_LINE = EXTI_LINE22 ;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStruct.EXTI_LINEEnable = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    /* Configure comparator interrupt */
    NVIC_InitStruct.NVIC_IRQ = ADC1_CMP_IRQn;
    NVIC_InitStruct.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStruct. NVIC_IRQSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /* Enable comparator window */
    CMP_Window_Enable(ENABLE);

    /* Enable comparator */
    CMP_Enable(CMP_CHANNEL_CMP1, ENABLE);
    CMP_Enable(CMP_CHANNEL_CMP2, ENABLE);
}

/**
  * @brief  Deepsleep mode configuration.
  * @param  None
  * @retval None
  */
void DeepsleepEntry(void)
{
    /* PWR Peripheral clock enable */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_PWR, ENABLE);
   /* Request to enter Deepsleep mode */
    PWR_DEEPSLEEPMode_Entry(PWR_LDO_LOWPOWER,PWR_DEEPSLEEPENTRY_WFI);
}

/**
  * @brief  Check comparator output state.
  * @param  None
  * @retval None
  */
STATE CheckState()
{
    STATE STATECMP;

    /* Check if CMP1 output level is high and CMP2 output level is high */
    if ((CMP_GetOutputLevel(CMP_CHANNEL_CMP1) == CMP_OUTPUTLEVEL_HIGH)
    && (CMP_GetOutputLevel(CMP_CHANNEL_CMP2) == CMP_OUTPUTLEVEL_HIGH))
    {
        STATECMP = STATE_OVER_THRESHOLD;
    }
    /* Check if CMP1 output level is low and CMP2 output level is high */
    if ((CMP_GetOutputLevel(CMP_CHANNEL_CMP1) == CMP_OUTPUTLEVEL_LOW) 
    && (CMP_GetOutputLevel(CMP_CHANNEL_CMP2) == CMP_OUTPUTLEVEL_HIGH))
    {
        STATECMP = STATE_WITHIN_THRESHOLD;
    }
    /* Check if CMP1 output level is low and CMP2 output level is low */
    if ((CMP_GetOutputLevel(CMP_CHANNEL_CMP1) == CMP_OUTPUTLEVEL_LOW)
    && (CMP_GetOutputLevel(CMP_CHANNEL_CMP2) == CMP_OUTPUTLEVEL_LOW))
    {
        STATECMP = STATE_UNDER_THRESHOLD;
    }
    return STATECMP;
}

/**
  * @brief  delay configuration.
  * @param  None
  * @retval None
  */
void Delay_ms(uint32_t time)
{
    g_delayms = time;
    while(g_delayms);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    SysTick_Config(72000000/1000);
    /* Initialize LEDs available on board */
    GD_EVAL_LEDInit(LED1);
    GD_EVAL_LEDInit(LED2);
    GD_EVAL_LEDInit(LED3);
    GD_EVAL_LEDInit(LED4);

    /* configure CMP1 and CMP2 */
    CMP_Config();
    /* configure CMP1 and CMP2 */
    CheckState();
    while(1)
    {
        /* Input voltage is over the thresholds: higher and lower thresholds */
        if (CheckState() == STATE_OVER_THRESHOLD)
        {
            GD_EVAL_LEDOn(LED1);
            GD_EVAL_LEDOff(LED2);
            GD_EVAL_LEDOn(LED3);
            GD_EVAL_LEDOff(LED4);
        }
        /* Input voltage is within the thresholds: higher and lower thresholds */
        if (CheckState() == STATE_WITHIN_THRESHOLD)
        {
            Delay_ms(500);
            if (CheckState() == STATE_WITHIN_THRESHOLD)
            {
                GD_EVAL_LEDOff(LED1);
                GD_EVAL_LEDOff(LED2);
                GD_EVAL_LEDOff(LED3);
                GD_EVAL_LEDOff(LED4);
                /* Enter deepsleep mode */
                DeepsleepEntry();
            }
        }
        /* Input voltage is under the thresholds: higher and lower thresholds */
        if(CheckState() == STATE_UNDER_THRESHOLD)
        {
            GD_EVAL_LEDOff(LED1);
            GD_EVAL_LEDOn(LED2);
            GD_EVAL_LEDOff(LED3);
            GD_EVAL_LEDOn(LED4);
        }
    }
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
