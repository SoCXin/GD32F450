/**
  ******************************************************************************
  * @file    CEC/CEC_host-slave communication/main.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include <stdlib.h>
#include <stdio.h>
#include "Lcd_Driver.h"
#include "GUI.h"
#include "delay.h"
#include "gd32f1x0_eval.h"

/* Private variables   ---------------------------------------------------------*/
GPIO_InitPara GPIO_InitStructure;
NVIC_InitPara NVIC_InitStructure;
EXTI_InitPara EXTI_InitStructure;

extern uint8_t ReceiveBuffer[10];
extern __IO uint8_t ReceivedFrame;
extern uint8_t rcv_inc;
extern uint8_t ByteNumber;
__IO uint8_t NUM=0;

/* Private function prototypes -----------------------------------------------*/
void CEC_Config(void);
void EXTI_Config(void);

/* Private function  ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    /* GPIO Periph clock enable */
    RCC_AHBPeriphClock_Enable( RCC_AHBPERIPH_GPIOA| RCC_AHBPERIPH_GPIOB|
                               RCC_AHBPERIPH_GPIOC| RCC_AHBPERIPH_GPIOF, ENABLE ); 

    /* TFT_LCD Init */
    delay_init(72);
    {
        Lcd_Init();
        Lcd_Clear(GREEN);

        /* TFT_LCD display "Start..." */
        Gui_DrawFont_GBK16(16,10, BLUE,GREEN,(uint8_t*)("Start...")); 

        /* TFT_LCD display the value of num(initial value is zero) */
        Gui_DrawFont_Num32(40,40, BLUE,GREEN,NUM);
    }

    /* Configure the CEC peripheral */
    CEC_Config();

    /* Configure the EXTI */
    GD_EVAL_KeyInit(KEY_TAMPER, KEY_MODE_EXTI);
    GD_EVAL_KeyInit(KEY_USER, KEY_MODE_EXTI);

    /* Main loop */
    while(1)
    {
        /* Wait receive data */
        while(ReceivedFrame==0);

        if(ReceivedFrame == 1)
        {
            if((ReceiveBuffer[1]==0xA5)&&(ReceiveBuffer[2]==0x5A))NUM++;
            if((ReceiveBuffer[1]==0x5A)&&(ReceiveBuffer[2]==0xA5))NUM--;
            if(NUM==0xFF)NUM=9;
            if(NUM==10)NUM=0;
            Gui_DrawFont_Num32(40,40,BLUE,GREEN,NUM);
        }

        /* A reception error occured */
        else
        {}
        ReceivedFrame = 0;
    }
}
/**
  * @brief  Configure the CEC peripheral.
  * @param  None
  * @retval None
  */
void CEC_Config(void)
{
    CEC_InitPara CEC_InitStructure;
    NVIC_InitPara NVIC_InitStructure;
    GPIO_InitPara GPIO_InitStructure;

    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_CEC , ENABLE);

    /* Enable CEC_LINE_GPIO clocks */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOB, ENABLE);

    /* Configure CEC_LINE_GPIO as Output open drain */
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_8;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;

    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE8,GPIO_AF_0);
   
    NVIC_PRIGroup_Enable(NVIC_PRIGROUP_1);

    /* Enable the CEC global Interrupt (with higher priority) */
    NVIC_InitStructure.NVIC_IRQ                = CEC_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQSubPriority     = 0x0;
    NVIC_InitStructure.NVIC_IRQEnable          = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure CEC :
       CEC_CFGR_SFTM      : SFTM 1.5 data bit periods
       CEC_CFGR_RTOL      : Standard bit timing tolerance
       CEC_CFGR_RBRESTP   : Do not stop reception on bit Rising Error
       CEC_CFGR_RBREGEN   : Do not generate Error-Bit on bit Rising Error
       CEC_CFGR_RLREGEN   :Do not generate Error-Bit on Long Bit Period Error
       CEC_CFGR_RBRDNOGEN :Generate Error-Bit in Broadcast
       CEC_CFGR_SFTMOPT   :SFT counter starts when SOM is set */
    CEC_InitStructure.CEC_SFTM     = CEC_SFTM_1T;
    CEC_InitStructure.CEC_RTOL     = CEC_RTOL_STD;
    CEC_InitStructure.CEC_RBRESTOP = CEC_RBRESTOP_OFF;
    CEC_InitStructure.CEC_RBREGEN  = CEC_RBREGEN_OFF;
    CEC_InitStructure.CEC_RLBPEGEN = CEC_RLBPEGEN_OFF;
    CEC_InitStructure.CEC_BCNG     = CEC_BCNG_OFF;
    CEC_InitStructure.CEC_SFTMOPT  = CEC_SFTMOPT_SOM;
    CEC_Init(&CEC_InitStructure);

    /* Set the CEC initiator address */
    CEC_SetOwnAddress(0x2);

    /* Activate CEC interrupts associated to the set of TX flags */
    CEC_INTConfig(CEC_INT_TEND| CEC_INT_TBR,ENABLE);
    /* Activate CEC interrupts associated to the set of RX flags */
    CEC_INTConfig(CEC_INT_REND| CEC_INT_RBR,ENABLE);

    /* Activate CEC interrupts associated to the set of TX error */
    CEC_INTConfig(CEC_INT_TAE| CEC_INT_TE| CEC_INT_TU| CEC_INT_LSTARB,ENABLE); 
    /* Activate CEC interrupts associated to the set of RX error */
    CEC_INTConfig(CEC_INT_RAE| CEC_INT_RLBPE| CEC_INT_RSBPE| CEC_INT_RBRE| CEC_INT_RO,ENABLE);

    /* Enable CEC */
    CEC_Enable(ENABLE);
}

/**
  * @brief  Configure the EXTI peripheral.
  * @param  None
  * @retval None
  */
void EXTI_Config()
{
    GPIO_InitPara GPIO_InitStructure;
    EXTI_InitPara EXTI_InitStructure;
    NVIC_InitPara NVIC_InitStructure;

    /* Enable the BUTTON Clock */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOF| RCC_AHBPERIPH_GPIOC, ENABLE);
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_CFG, ENABLE);

    /* Configure Button pin as input */
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_InitStructure.GPIO_Pin  = GPIO_PIN_7;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_InitStructure.GPIO_Pin  = GPIO_PIN_13;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Connect Button EXTI Line to Button GPIO Pin */
    SYSCFG_EXTILine_Config(EXTI_SOURCE_GPIOF, EXTI_SOURCE_PIN7);
    SYSCFG_EXTILine_Config(EXTI_SOURCE_GPIOC, EXTI_SOURCE_PIN13);

    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_LINE       = EXTI_LINE7;
    EXTI_InitStructure.EXTI_Mode       = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger    = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LINEEnable = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    EXTI_InitStructure.EXTI_LINE       = EXTI_LINE13;
    EXTI_InitStructure.EXTI_Mode       = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger    = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LINEEnable = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQ                = EXTI4_15_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 1;
    NVIC_InitStructure.NVIC_IRQSubPriority     = 2;
    NVIC_InitStructure.NVIC_IRQEnable          = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
