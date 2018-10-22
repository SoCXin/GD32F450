/**
  ******************************************************************************
  * @file    PWR/Deep-sleep_demo/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Main program body.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "gd32f1x0_eval.h"

uint32_t Time_Delay = 0;
void delay_ms(uint32_t nms);
void RTC_ALARM_Config(void);
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{     
    /* Initialize LEDs and Key Buttons */
    GD_EVAL_LEDInit(LED1); 
    GD_EVAL_LEDInit(LED2); 
    GD_EVAL_KeyInit(KEY_WAKEUP,KEY_MODE_GPIO);
    /* Configure the GPIO to EXTI_line of Key Button */        
    GD_EVAL_KeyInit(KEY_TAMPER,KEY_MODE_EXTI);    
    
    /* LED1 twinkles twice */
    GD_EVAL_LEDOn(LED1);
    delay_ms(200);
    GD_EVAL_LEDOff(LED1);  
    delay_ms(200);
    GD_EVAL_LEDOn(LED1);
    delay_ms(200);
    
    RTC_ALARM_Config();

    /* Turn off LED1 */
    GD_EVAL_LEDOff(LED1);  
    /*Enter Standby mode*/   
    RTC_ClearBitState(RTC_STR_AF);
    PWR_DEEPSLEEPMode_Entry(PWR_LDO_LOWPOWER, PWR_DEEPSLEEPENTRY_WFI);
    /* Reinitialize the system clock after wake up from deep-sleep mode */
    System_Init();
    
    /* LED2 twinkles twice */
    GD_EVAL_LEDOn(LED2);
    delay_ms(200);
    GD_EVAL_LEDOff(LED2);  
    delay_ms(200);
    GD_EVAL_LEDOn(LED2);
    delay_ms(200);
    
    while (1)
    {    
        if(GPIO_ReadInputBit(WAKEUP_KEY_GPIO_PORT, WAKEUP_KEY_PIN) == 0)
        {
            /* Elimitate jitter */
            delay_ms(5);
            
            if(GPIO_ReadInputBit(WAKEUP_KEY_GPIO_PORT, WAKEUP_KEY_PIN) == 0)
            {
                /* Enable PWR clock */
                RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_PWR, ENABLE);
                            
                /* Request to enter DEEPSLEEP mode with regulator in low power mode*/
                PWR_DEEPSLEEPMode_Entry(PWR_LDO_LOWPOWER, PWR_DEEPSLEEPENTRY_WFI);
                GD_EVAL_LEDOn(LED2);
                delay_ms(200);
                GD_EVAL_LEDOff(LED2);  
                delay_ms(200);
                GD_EVAL_LEDOn(LED2);
                delay_ms(200);
            }
            else
            {
                GD_EVAL_LEDOn(LED1);
                delay_ms(200);
                GD_EVAL_LEDOff(LED1);
                delay_ms(200);
                GD_EVAL_LEDOn(LED1);
                delay_ms(200);                
            }
        }
        else
        {
            GD_EVAL_LEDOn(LED1);
            delay_ms(200);
            GD_EVAL_LEDOff(LED1); 
            delay_ms(200);
            GD_EVAL_LEDOn(LED1);
            delay_ms(200);            
        }
    }
}

/**
  * @brief  Insert a delay time.
  * @param  nms: the delay time length, in milliseconds.
  * @retval None
  */
void delay_ms(uint32_t nms)
{
    /* Setup SysTick Timer for 1 ms interrupts */
    if (SysTick_Config(SystemCoreClock / 1000))
    { 
        /* Capture error */ 
        while (1);
    }
    Time_Delay = nms;
    while(Time_Delay);
    SysTick->CTRL = 0x00;
    SysTick->VAL = 0x00;
}

/**
  * @brief  Configure RTC clock source and alarm.
  * @param  None
  * @retval None
  */
void RTC_ALARM_Config(void)
{
    uint8_t tmp_sec;
    
    /* RTC Alarm Interrupt Configuration */
    EXTI_InitPara EXTI_InitStructure;
    NVIC_InitPara NVIC_InitStructure;
    RTC_InitPara RTC_InitStructure;
    RTC_AlarmPara RTC_AlarmStructure;
    
    /* RTC clock source configuration */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_PWR, ENABLE);
    
    /* Allow access to RTC */
    PWR_BackupAccess_Enable(ENABLE);
    
    /*Reset the Backdomain area */
    RCC_BackupReset_Enable(ENABLE);
    RCC_BackupReset_Enable(DISABLE);
    
    /* Enable the LSI OSC */
    RCC_LSI_Enable(ENABLE);
    /* Wait till LSI is ready */
    while(RCC_GetBitState(RCC_FLAG_LSISTB) == RESET)
    {
    }
    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    
    /* Enable the RTC Clock */
    RCC_RTCCLK_Enable(ENABLE);
    
    /* Wait for RTC APB registers synchronisation */
    RTC_WaitRSF_ToSetAgain(); 
    
    /* Configure the RTC time */
    RTC_InitStructure.RTC_Prescaler_Asyn = 0x7F;
    RTC_InitStructure.RTC_Prescaler_Syn = 0x0138; 
    RTC_InitStructure.RTC_AM_PM = RTC_AM;
    RTC_InitStructure.RTC_Year = 0x1;
    RTC_InitStructure.RTC_WeekDay = 0x1;
    RTC_InitStructure.RTC_Month = 0x1;
    RTC_InitStructure.RTC_Date = 0x1;      
    RTC_InitStructure.RTC_Hour   = 0x01;
    RTC_InitStructure.RTC_Minute = 0x00;
    RTC_InitStructure.RTC_Second = 0x00; 
    RTC_InitStructure.RTC_Hour_Format = RTC_24HOUR; 
    RTC_Config(&RTC_InitStructure, RTC_STORE_BCD);
    
    if (RTC_Config(&RTC_InitStructure, RTC_STORE_NORMAL) == ERROR)
    {
        while(1);
    } 
    
    /* Configure EXTI Line17(RTC Alarm) to generate an interrupt on rising edge */
    EXTI_ClearIntBitState(EXTI_LINE17);
    EXTI_InitStructure.EXTI_LINE = EXTI_LINE17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LINEEnable = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    /* NVIC configuration */   
    NVIC_InitStructure.NVIC_IRQ = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* Get current time */
    RTC_GetTimeDate(&RTC_InitStructure, RTC_STORE_BCD);
    
    /* Config the RTC alarm */
    RTC_AlarmStructure.RTC_Alarm_Mask = RTC_ALARM_DATE_MASK;
    RTC_AlarmStructure.RTC_Alarm_Weekday_Sel = RTC_ALARM_DATE_SELECTED;
    RTC_AlarmStructure.RTC_Alarm_Date = 0x31;
    RTC_AlarmStructure.RTC_AM_PM = RTC_AM;
    RTC_AlarmStructure.RTC_Hour = 0x01;
    RTC_AlarmStructure.RTC_Minute = 0x00;
    tmp_sec = RTC_InitStructure.RTC_Second + 5;
    if(tmp_sec >= 60)
    {
        tmp_sec = tmp_sec - 60;
    }
    RTC_AlarmStructure.RTC_Second = tmp_sec;
    
    RTC_SetAlarm(&RTC_AlarmStructure, RTC_STORE_BCD);
    RTC_ClearBitState(RTC_STR_AF);
    /* Enable the RTC Alarm event interrupt */
    RTC_INT_Enable(RTC_INT_ALRA, ENABLE);
    
    /* Enable the alarm */
    RTC_Alarm_Enable(ENABLE);
    
    /* Clear the Alarm event flag */
    RTC_ClearBitState(RTC_STR_AF); 
}
/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
