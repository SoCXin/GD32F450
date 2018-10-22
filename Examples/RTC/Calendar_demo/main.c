/**
  ******************************************************************************
  * @file    RTC/Calendar/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gd32f1x0_eval.h"

/* Private define ------------------------------------------------------------*/
/* Choose the RTC clock source */
/* #define RTC_CLOCK_SOURCE_LSE */   
#define RTC_CLOCK_SOURCE_LSI  

#define BKP_VALUE    0x32F0 

/* Private variables ---------------------------------------------------------*/
RTC_InitPara   RTC_InitStructure;
RTC_AlarmPara  RTC_AlarmInitStructure;
NVIC_InitPara  NVIC_InitStructure;
EXTI_InitPara  EXTI_InitStructure;
USART_InitPara USART_InitStructure;
GPIO_InitPara  GPIO_InitStructure;
__IO uint32_t prescaler_a = 0, prescaler_s = 0;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{  
    /* Configure the USART peripheral ----------------------------------------*/
    USART_InitStructure.USART_BRR = 115200;
    USART_InitStructure.USART_WL = USART_WL_8B;
    USART_InitStructure.USART_STBits = USART_STBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_RESET;
    USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE ;
    USART_InitStructure.USART_RxorTx = USART_RXORTX_RX | USART_RXORTX_TX; 
    GD_EVAL_COMInit(&USART_InitStructure);

    printf("\n\r  *********************** RTC Calendar Demo ***********************\n\r");

    /* Check if RTC has aready been configured */
    if (RTC->BKP0R != BKP_VALUE)
    {
        RTC_PreConfiguration();

        RTC_Setup(); 

    }
    else
    {
        /* Detect the reset source */
        if (RCC_GetBitState(RCC_FLAG_POPDRST) != RESET)
        {
            printf("\r\n Power On Reset Occurred....\n\r");
        }
        else if (RCC_GetBitState(RCC_FLAG_EPRST) != RESET)
        {
            printf("\r\n External Reset Occurred....\n\r");
        }

        printf("\n\r No Need To Configure RTC....\n\r");

        #ifdef RTC_CLOCK_SOURCE_LSI
            RCC_LSI_Enable(ENABLE);
        #endif 

        RTC_WaitRSF_ToSetAgain();
        RTC_ClearBitState(RTC_STR_AF);
        EXTI_ClearIntBitState(EXTI_LINE17);

        RTC_ShowTime();
        RTC_ShowAlarm();
    }
    
    RCC_ClearBitState();

    /* LED1 GPIO Initialization */
    RCC_AHBPeriphClock_Enable(LED1_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = LED1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);
    
    LED1_GPIO_PORT->BOR = LED1_PIN;

    /* LED2 GPIO Initialization */
    RCC_AHBPeriphClock_Enable(LED2_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = LED2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);
    
    LED2_GPIO_PORT->BOR = LED2_PIN;

    /* RTC ALARM Interrupt Configuration */
    EXTI_ClearIntBitState(EXTI_LINE17);
    EXTI_InitStructure.EXTI_LINE = EXTI_LINE17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LINEEnable = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQ = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    while (1)
    {
    }
}

/**
  * @brief  Select the RTC clock source.
  * @param  None
  * @retval None
  */
void RTC_PreConfiguration(void)
{
    /* Enable PWR clock */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_PWR, ENABLE);
    /* Enable the access of the RTC registers */
    PWR_BackupAccess_Enable(ENABLE);

    #if defined (RTC_CLOCK_SOURCE_LSI) 
        RCC_LSI_Enable(ENABLE);
        while(RCC_GetBitState(RCC_FLAG_LSISTB) == RESET)
        {
        }
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

        prescaler_s = 0x18F;
        prescaler_a = 0x63;
    #elif defined (RTC_CLOCK_SOURCE_LSE)
        RCC_LSEConfig(RCC_LSE_EN);
        while(RCC_GetBitState(RCC_FLAG_LSESTB) == RESET)
        {
        }
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

        prescaler_s = 0xFF;
        prescaler_a = 0x7F;
    #else
    #error RTC Clock source should be defined.
    #endif /* RTC_CLOCK_SOURCE_LSI */

    RCC_RTCCLK_Enable(ENABLE);
    RTC_WaitRSF_ToSetAgain();
}

/**
  * @brief  Use Hyperterminal to setup RTC time and alarm.
  * @param  None
  * @retval None
  */
void RTC_Setup(void)
{
    /* Setup RTC time value */
    uint32_t tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;

    printf("\n\r==============Input Time Value=====================================\n\r");

    RTC_InitStructure.RTC_Prescaler_Asyn = prescaler_a;
    RTC_InitStructure.RTC_Prescaler_Syn = prescaler_s;
    RTC_InitStructure.RTC_Year = 0x1;
    RTC_InitStructure.RTC_WeekDay = 0x1;
    RTC_InitStructure.RTC_Month = 0x1;
    RTC_InitStructure.RTC_Date = 0x1;
    RTC_InitStructure.RTC_Hour_Format = RTC_24HOUR;
    RTC_InitStructure.RTC_AM_PM = RTC_AM;

    /* current time input */
    printf("  Please Input Hour:\n\r");
    while (tmp_hh == 0xFF)
    {
        tmp_hh = USART_Input_Threshold(23);
        RTC_InitStructure.RTC_Hour = tmp_hh;
    }
    printf("  %0.2d\n\r", tmp_hh);

    printf("  Please Input Minute:\n\r");
    while (tmp_mm == 0xFF)
    {
        tmp_mm = USART_Input_Threshold(59);
        RTC_InitStructure.RTC_Minute = tmp_mm;
    }
    printf("  %0.2d\n\r", tmp_mm);

    printf("  Please Input Second:\n\r");
    while (tmp_ss == 0xFF)
    {
        tmp_ss = USART_Input_Threshold(59);
        RTC_InitStructure.RTC_Second = tmp_ss;
    }
    printf("  %0.2d\n\r", tmp_ss);

    /* RTC current time configuration */
    if(RTC_Config(&RTC_InitStructure, RTC_STORE_NORMAL) == ERROR)
    {
        printf("\n\r>> RTC Time Configuration Failed.  <<\n\r");
    } 
    else
    {
        printf("\n\r>> RTC Time Configuration Success. <<\n\r");
        RTC_ShowTime();
        RTC->BKP0R = BKP_VALUE;
    }

    /* Setup RTC alarm */
    tmp_hh = 0xFF;
    tmp_mm = 0xFF;
    tmp_ss = 0xFF;

    RTC_Alarm_Enable(DISABLE);
    printf("\n\r==============Input Alarm Value=====================================\n\r");
    RTC_AlarmInitStructure.RTC_Alarm_Mask = RTC_ALARM_DATE_MASK;
    RTC_AlarmInitStructure.RTC_Alarm_Weekday_Sel = RTC_ALARM_DATE_SELECTED;
    RTC_AlarmInitStructure.RTC_Alarm_Date = 0x31;
    RTC_AlarmInitStructure.RTC_AM_PM = RTC_AM;

    /* RTC alarm input */
    printf("  Please Input Alarm Hour:\n\r");
    while (tmp_hh == 0xFF)
    {
        tmp_hh = USART_Input_Threshold(23);
        RTC_AlarmInitStructure.RTC_Hour = tmp_hh;
    }
    printf("  %0.2d\n\r", tmp_hh);
  
    printf("  Please Input Alarm Minute:\n\r");
    while (tmp_mm == 0xFF)
    {
        tmp_mm = USART_Input_Threshold(59);
        RTC_AlarmInitStructure.RTC_Minute = tmp_mm;
    }
    printf("  %0.2d\n\r", tmp_mm);

    printf("  Please Input Alarm Second:\n\r");
    while (tmp_ss == 0xFF)
    {
        tmp_ss = USART_Input_Threshold(59);
        RTC_AlarmInitStructure.RTC_Second = tmp_ss;
    }
    printf("  %0.2d", tmp_ss);  

    /* RTC alarm configuration */
    RTC_SetAlarm(&RTC_AlarmInitStructure,RTC_STORE_NORMAL);
    printf("\n\r>> RTC Set Alarm Success.  <<\n\r");
    RTC_ShowAlarm(); 
    RTC_INT_Enable(RTC_INT_ALRA, ENABLE);  
    RTC_Alarm_Enable(ENABLE);    
}

/**
  * @brief  Display the current time.
  * @param  None
  * @retval None
  */
void RTC_ShowTime(void)
{
    RTC_GetTimeDate(&RTC_InitStructure, RTC_STORE_NORMAL);
    printf("\n\r  The current time is :  %0.2d:%0.2d:%0.2d \n\r", RTC_InitStructure.RTC_Hour, RTC_InitStructure.RTC_Minute, RTC_InitStructure.RTC_Second);
}

/**
  * @brief  Display the alram value.
  * @param  None
  * @retval None
  */
void RTC_ShowAlarm(void)
{
    RTC_GetAlarm(&RTC_AlarmInitStructure, RTC_STORE_NORMAL);
    printf("\n\r  The alarm is :  %0.2d:%0.2d:%0.2d \n\r", RTC_AlarmInitStructure.RTC_Hour, RTC_AlarmInitStructure.RTC_Minute, RTC_AlarmInitStructure.RTC_Second);
}


/**
  * @brief  Get the input value and check if it is valid.
  * @param  None
  * @retval None
  */
uint8_t USART_Input_Threshold(uint32_t value)
{
    uint32_t index = 0;
    uint32_t tmp[2] = {0, 0};

    while (index < 2)
    {
        while (USART_GetBitState(EVAL_COM1, USART_FLAG_RBNE) == RESET)
        {}
        tmp[index++] = (USART_DataReceive(EVAL_COM1));
        if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
        {
            printf("\n\r Please Input a valid number between 0 and 9 \n\r");
            index--;
        }
    }

    index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
    if (index > value)
    {
        printf("\n\r Please Input a valid number between 0 and %d \n\r", value);
        return 0xFF;
    }
    return index;
}

/**
  * @brief  Retarget the printf function of C library to the USART module.
  * @param  None
  * @retval None
  */
OUTPUTCHAR_PROTOTYPE
{
    USART_DataSend(EVAL_COM1, (uint8_t) character);
    while (USART_GetBitState(EVAL_COM1, USART_FLAG_TC) == RESET)
    {}
    return character;
}

/************************ (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
