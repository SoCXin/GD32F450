/**
  ******************************************************************************
  * @file    TSI/TSI_TouchKey_leds/main.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "gd32f1x0_eval.h"
#include <stdlib.h>
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/
#define THRESHOLD1       0x30
#define THRESHOLD2       0x30

GPIO_InitPara GPIO_InitStructure;

/* The current cycle number array of the channel pin */
uint16_t SampleCycle[2] = {0,0};

/* Reference value sample array of TSI_GROUP3 */
uint16_t Sample_Refervalue_Group3[20] = {0};
uint16_t Sample_Refervalue_Group4[20] = {0};

/* Average value of cycles */
uint16_t Sample_Refervalue_Average[2] = {0};

/* Private function prototypes -----------------------------------------------*/
void Delay(uint32_t nCount);
void GPIO_Init_Configure(void);
void TSI_Init_Configure(void);
void LED_Init_Configure(void);
void Acquisition_TSI_PIN(uint32_t TSI_groupx_pin);

/* Private functions ----------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    int m=0;

    /* GPIOB Periph clock enable */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOB, ENABLE);

    /* TSI Periph clock enable */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_TSI, ENABLE);

    /* PB0   TSI_GROUP3_PIN2     SAMPCAP
       PB1   TSI_GROUP3_PIN3     CHANNEL
       PB2   TSI_GROUP3_PIN4     CHANNEL */

    /* Configure the GPIO ports */
    GPIO_Init_Configure();

    /* Configure the TSI peripheral */
    TSI_Init_Configure();

    /* Configure the LEDs */
    LED_Init_Configure();

    /* Reference cycle value acquisition and processing */
    {
        for(m=0;m<20;m++)
        {
            /* Get Charge-Transfer complete cycle number of TSI_GROUP3_PIN3 */
            {
                Acquisition_TSI_PIN(TSI_GROUP3_PIN3);

                /* Check the TSI flag:End of acquisition interrupt */
                if(TSI_GetIntBitState(TSI_STR_CTCF) == (uint8_t)SET)
                {
                    /* Get Charge-Transfer complete cycle number */
                    Sample_Refervalue_Group3[m] = TSI_GetGroupSampleCycle(TSI_GROUP3);
                }

                /* Disable the selected pin as Channel pin */
                TSI_ChannelModeConfig(TSI_GROUP3_PIN3, DISABLE);
            }

            /* Get Charge-Transfer complete cycle number of TSI_GROUP3_PIN4 */
            {
                Acquisition_TSI_PIN(TSI_GROUP3_PIN4);

                if(TSI_GetIntBitState(TSI_STR_CTCF) == (uint8_t)SET)
                {
                    Sample_Refervalue_Group4[m] = TSI_GetGroupSampleCycle(TSI_GROUP3);
                }

                TSI_ChannelModeConfig(TSI_GROUP3_PIN4, DISABLE);
            }

            /* Delay for a period of time while all banks have been acquired */
            Delay(0xFFFF);
        }
        {
            for(m=1;m<20;m++)
            {
                Sample_Refervalue_Average[0] += Sample_Refervalue_Group3[m];
                Sample_Refervalue_Average[1] += Sample_Refervalue_Group4[m];
            }

            /* Average channel cycle value are obtained */
            Sample_Refervalue_Average[0] = Sample_Refervalue_Average[0]/19;
            Sample_Refervalue_Average[1] = Sample_Refervalue_Average[1]/19;
        }
    }

    /* Main loop */
    while (1)
    {
        /* Acquisition TSI_GROUP3_PIN3 */
        {
            Acquisition_TSI_PIN(TSI_GROUP3_PIN3);
     
            /* Check the TSI flag--End of acquisition interrupt */
            if(TSI_GetIntBitState(TSI_STR_CTCF) == (uint8_t)SET)
            {
                /* Get Charge-Transfer complete cycle number */
                SampleCycle[0] = TSI_GetGroupSampleCycle(TSI_GROUP3);
            }
            else
            {
                /* Max Count Error */
            }

            /* Light LED1 */
            if((Sample_Refervalue_Average[0]-SampleCycle[0]) > THRESHOLD1)
            {
                /* TSI_GROUP3_PIN3 is touched */
                GD_EVAL_LEDOn(LED1);
            }
            else
            {
                GD_EVAL_LEDOff(LED1);
            }
            TSI_ChannelModeConfig(TSI_GROUP3_PIN3, DISABLE);
        }

        /* Acquisition TSI_GROUP3_PIN4 */
        {
            Acquisition_TSI_PIN(TSI_GROUP3_PIN4);

            if(TSI_GetIntBitState(TSI_STR_CTCF) == (uint8_t)SET)
            {
                SampleCycle[1] = TSI_GetGroupSampleCycle(TSI_GROUP3);
            }
            else
            {
                /* Max Count Error */
            }

            /* Light LED2 */
            if((Sample_Refervalue_Average[1]-SampleCycle[1]) > THRESHOLD2)
            {
                /* TSI_GROUP3_PIN4 is touched */
                GD_EVAL_LEDOn(LED2);
            }
            else
            {
                GD_EVAL_LEDOff(LED2);
            }
            TSI_ChannelModeConfig(TSI_GROUP3_PIN4, DISABLE);
        }

        /* Delay for a period of time while all banks have been acquired */
        Delay(0xFFFF);
    }
}
/**
  * @brief  Insert a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
void Delay(uint32_t nCount)
{
    for(; nCount != 0; nCount--);
}

/**
  * @brief  Configure the GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Init_Configure(void)
{
    /* GPIO PB0 */
    /* Alternate function Output Open-Drain for Sampling Capacitor IO */
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_2MHZ;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_0;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* GPIO PB1
       GPIO PB2 */
    /* Alternate function Output Push-Pull for Channel and Shield IOs */
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_1| GPIO_PIN_2;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Connect pin to Peripheral */
    GPIO_PinAFConfig(GPIOB, GPIO_PINSOURCE0, GPIO_AF_3);
    GPIO_PinAFConfig(GPIOB, GPIO_PINSOURCE1, GPIO_AF_3);
    GPIO_PinAFConfig(GPIOB, GPIO_PINSOURCE2, GPIO_AF_3);
}

/**
  * @brief  Configure the TSI peripheral.
  * @param  None
  * @retval None
  */
void TSI_Init_Configure(void)
{
    /* TSI Init structure definition */
    TSI_BaseInitPara TSI_BaseInitStruct;

    /* TSI Group Init structure definition */
    TSI_GroupInitPara TSI_GroupInitStruct;
  
    TSI_BaseInitStruct.TSI_PulsePrescaler = TSI_CTCDIV_8;
    TSI_BaseInitStruct.TSI_pulseLow       = TSI_CTDT_8CTCLK;
    TSI_BaseInitStruct.TSI_pulseHigh      = TSI_CDT_4CTCLK;
    TSI_BaseInitStruct.TSI_TriggerMode    = TSI_SOFTWARE_TRIGGLER;
    TSI_BaseInitStruct.TSI_EageSelect     = TSI_RISING_EDGE_TRIGGLER; 
    TSI_BaseInitStruct.TSI_MaxCycle       = TSI_MCN_4095_CYCLE;
    TSI_BaseInit(&TSI_BaseInitStruct);

    TSI_GroupInitStruct.TSI_Group      = TSI_GROUP3;
    TSI_GroupInitStruct.TSI_SamplePin  = TSI_GROUP3_PIN2;
    TSI_GroupInitStruct.TSI_ChannelPin = TSI_GROUP3_PIN3|TSI_GROUP3_PIN4;
    TSI_GroupInit(&TSI_GroupInitStruct);

    /* Disable Hysteresis mode */
    TSI_PinHysteresisConfig(TSI_GROUP3_PIN2|
                            TSI_GROUP3_PIN3|
                            TSI_GROUP3_PIN4, DISABLE);
    /* Enable TSI */
    TSI_Enable(ENABLE);
}

/**
  * @brief  LEDs configure.
  * @param  None
  * @retval None
  */
void LED_Init_Configure(void)
{
    /* Initialize the LEDs */
    GD_EVAL_LEDInit(LED1);
    GD_EVAL_LEDInit(LED2);

    /* Close all of LEDs */
    GD_EVAL_LEDOff(LED1);
    GD_EVAL_LEDOff(LED2);
}

/**
  * @brief  Acquisition TSI_GROUP3_PINx.
  * @param  TSI_groupx_pin:TSI_GROUPx_PINx.
  * @retval None
  */
void Acquisition_TSI_PIN(uint32_t TSI_groupx_pin)
{
    /* Configure the TSI Pin Channel Mode */
    TSI_ChannelModeConfig(TSI_groupx_pin,ENABLE);

    /* Wait capacitors discharge */
    Delay(0xD00);

    /* Clear both CMCE and CCTCF flags */
    TSI_ClearIntBitState(TSI_CEFR_CMNE|TSI_CEFR_CCTCF);

    /* Start a new acquisition */
    TSI_StartAcquisition(ENABLE);

    /* Wait the specified TSI flag state: MCE or CTCF */
    while(TSI_GetIntBitState(TSI_STR_MNE | TSI_STR_CTCF) == (uint8_t)RESET); 
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
