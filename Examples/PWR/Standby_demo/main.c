/**
  ******************************************************************************
  * @file    PWR/Standby_demo/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Main program body. 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_eval.h"

/* Private variables ----------------------------------------------------------*/
RTC_InitPara my_RTC_InitParameter;
RTC_AlarmPara  my_RTC_AlarmParameter;
GPIO_InitPara my_GPIO_InitParameter;
void SysTick_Initialization(void);
void RTC_Setup(void);
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    /*Initialize the SysTick to generate the interrupt every 250ms*/
    SysTick_Initialization();

    /* Tamper KEY Initialization*/
    GD_EVAL_KeyInit(KEY_TAMPER,KEY_MODE_GPIO);

    /*LED3 Initialization*/
    GD_EVAL_LEDInit(LED3);
    GD_EVAL_LEDOn(LED3);

    /*Wait the Button pushed*/
    while(GPIO_ReadInputBit(TAMPER_KEY_GPIO_PORT, TAMPER_KEY_PIN) != RESET);
    /*Set RTC Alarm to wakeup the Standby mode*/
    RTC_Setup();

    while(1)
    {}
}

/**
  * @brief  Configure the RTC.
  * @param  None
  * @retval None
  */
void RTC_Setup(void)
{  
    /*Enable LSI*/
    RCC_LSI_Enable(ENABLE);
    while(RCC_GetBitState(RCC_FLAG_LSISTB) == RESET)
    {
    }
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_PWR, ENABLE);
    PWR_BackupAccess_Enable(ENABLE);

    /*Check if the Standby mode has already happened.*/
    if (PWR_GetBitState(PWR_FLAG_STB) != RESET)
    {       
        PWR_ClearBitState(PWR_FLAG_STB);
        if (PWR_GetBitState(PWR_FLAG_STB) != RESET)
        {
            /*error*/
            while(1);
        }    
        /*RTC Alarm has already been configured.*/
        RTC_WaitRSF_ToSetAgain();        
    }
    else
    {  
        /*Reset the Backdomain area.*/
        RCC_BackupReset_Enable(ENABLE);
        RCC_BackupReset_Enable(DISABLE);

        /*Choose LSI as the RTC clock*/
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
        RCC_RTCCLK_Enable(ENABLE);
        RTC_WaitRSF_ToSetAgain();
         
        /*Configure the RTC Alarm*/
        my_RTC_AlarmParameter.RTC_AM_PM = RTC_AM;
        my_RTC_AlarmParameter.RTC_Hour   = 0x01;
        my_RTC_AlarmParameter.RTC_Minute = 0x00;
        my_RTC_AlarmParameter.RTC_Second = 0x3;
        my_RTC_AlarmParameter.RTC_Alarm_Date = 0x31;
        my_RTC_AlarmParameter.RTC_Alarm_Weekday_Sel = RTC_ALARM_DATE_SELECTED;
        my_RTC_AlarmParameter.RTC_Alarm_Mask = RTC_ALARM_DATE_MASK;
        RTC_SetAlarm(&my_RTC_AlarmParameter,RTC_STORE_BCD    );
        RTC_ClearBitState(RTC_STR_AF);
        RTC_INT_Enable(RTC_INT_ALRA, ENABLE);  
        RTC_Alarm_Enable( ENABLE);    
    } 
    /*Configure the RTC time*/
    my_RTC_InitParameter.RTC_AM_PM     = RTC_AM;
    my_RTC_InitParameter.RTC_Hour_Format = RTC_24HOUR;
    my_RTC_InitParameter.RTC_Prescaler_Asyn = 0x7F;
    my_RTC_InitParameter.RTC_Prescaler_Syn = 0x0138;    
    my_RTC_InitParameter.RTC_Year = 0x1;
    my_RTC_InitParameter.RTC_WeekDay = 0x1;
    my_RTC_InitParameter.RTC_Month = 0x1;
    my_RTC_InitParameter.RTC_Date = 0x1;        
    my_RTC_InitParameter.RTC_Hour   = 0x01;
    my_RTC_InitParameter.RTC_Minute = 0x00;
    my_RTC_InitParameter.RTC_Second = 0x00;  
    RTC_Config(&my_RTC_InitParameter,RTC_STORE_BCD);
  
    /*Enter Standby mode*/
    PWR_ClearBitState(PWR_FLAG_WKUP);   
    RTC_ClearBitState(RTC_STR_AF);
    PWR_STDBYMode_Entry(PWR_STDBYENTRY_WFI);
}

/**
  * @brief  Configure the SysTick.
  * @param  None
  * @retval None
  */
void SysTick_Initialization(void)
{
    /* Interrupt generated every 250 ms */
    if (SysTick_Config((SystemCoreClock/8) / 4))
    { 
        /*error*/
        while (1);
    }

    SysTick_CKSource_Enable(SYSTICK_CKSOURCE_HCLK_DIV8);
    NVIC_SetPriority(SysTick_IRQn, 0x04);
}

/************************ (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
