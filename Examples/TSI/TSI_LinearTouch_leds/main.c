/**
  ******************************************************************************
  * @file    TSI/TSI_LinearTouch_leds/main.c
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
GPIO_InitPara GPIO_InitStructure;

/* The flag of touching */
int touch_flag[3] = {0,0,0};

/* The current cycle number array of the channel pin */
uint16_t SampleCycle[4] = {0,0,0,0};

/* Reference value sample array of TSI_GROUP6 */
uint16_t Sample_Refervalue_Group2[20] = {0};
uint16_t Sample_Refervalue_Group3[20] = {0};
uint16_t Sample_Refervalue_Group4[20] = {0};

/* Average value of cycles */
uint16_t Sample_Refervalue_Average[3] = {0};

uint16_t threshold1 = 0;
uint16_t threshold2 = 0;

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

    /* PB11  TSI_GROUP6_PIN1     SAMPCAP
       PB12  TSI_GROUP6_PIN2     CHANNEL
       PB13  TSI_GROUP6_PIN3     CHANNEL
       PB14  TSI_GROUP6_PIN4     CHANNEL */

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
            /* Get Charge-Transfer complete cycle number of TSI_GROUP6_PIN2 */
            {
                Acquisition_TSI_PIN(TSI_GROUP6_PIN2);

                /* Check the TSI flag:End of acquisition interrupt */
                if(TSI_GetIntBitState(TSI_STR_CTCF) == (uint8_t)SET)
                {
                    /* Get Charge-Transfer complete cycle number */
                    Sample_Refervalue_Group2[m] = TSI_GetGroupSampleCycle(TSI_GROUP6);
                }

                /* Disable the selected pin as Channel pin */
                TSI_ChannelModeConfig(TSI_GROUP6_PIN2, DISABLE);
            }

            /* Get Charge-Transfer complete cycle number of TSI_GROUP6_PIN3 */
            {
                Acquisition_TSI_PIN(TSI_GROUP6_PIN3);

                if(TSI_GetIntBitState(TSI_STR_CTCF) == (uint8_t)SET)
                {
                    Sample_Refervalue_Group3[m] = TSI_GetGroupSampleCycle(TSI_GROUP6);
                }

                TSI_ChannelModeConfig(TSI_GROUP6_PIN3, DISABLE);
            }

            /* Get Charge-Transfer complete cycle number of TSI_GROUP6_PIN4 */
            {
                Acquisition_TSI_PIN(TSI_GROUP6_PIN4);

                if( TSI_GetIntBitState(TSI_STR_CTCF) == (uint8_t)SET)
                {
                    Sample_Refervalue_Group4[m] = TSI_GetGroupSampleCycle(TSI_GROUP6);
                }
              
                TSI_ChannelModeConfig(TSI_GROUP6_PIN4, DISABLE);
            }

            /* Delay for a period of time while all banks have been acquired */
            Delay(0x1000);
        }
        {
            for(m=1;m<20;m++)
            {
                Sample_Refervalue_Average[0] += Sample_Refervalue_Group2[m];
                Sample_Refervalue_Average[1] += Sample_Refervalue_Group3[m];
                Sample_Refervalue_Average[2] += Sample_Refervalue_Group4[m];
            }

            /* Average channel cycle value are obtained */
            Sample_Refervalue_Average[0] = Sample_Refervalue_Average[0]/19;
            Sample_Refervalue_Average[1] = Sample_Refervalue_Average[1]/19;
            Sample_Refervalue_Average[2] = Sample_Refervalue_Average[2]/19;
        }
        
        /* Set threshold use for determine touch location of TSI_GROUP6_PIN2 */
        threshold1 = Sample_Refervalue_Average[2]- Sample_Refervalue_Average[1];
        threshold2 = Sample_Refervalue_Average[2]- Sample_Refervalue_Average[1]+15;
    }

    /* Main loop */
    while (1)
    {
        /* Acquisition TSI_GROUP6_PIN2 */
        {
            Acquisition_TSI_PIN(TSI_GROUP6_PIN2);
     
            /* Check the TSI flag--End of acquisition interrupt */
            if(TSI_GetIntBitState(TSI_STR_CTCF) == (uint8_t)SET)
            {
                /* Get Charge-Transfer complete cycle number */
                SampleCycle[TSI_PIN2] = TSI_GetGroupSampleCycle(TSI_GROUP6);
            }
            else
            {
                /* Max Count Error */
            }

            /* Light LED1 and LED4 */
            if((Sample_Refervalue_Average[0]-SampleCycle[TSI_PIN2]) > 0x15)
            {
                /* TSI_GROUP6_PIN2 is touched */
                touch_flag[0] = 1;
            }
            else
            {
                touch_flag[0] = 0;
                GD_EVAL_LEDOff(LED1);
                GD_EVAL_LEDOff(LED4);
            }
            TSI_ChannelModeConfig(TSI_GROUP6_PIN2, DISABLE);
        }

        /* Acquisition TSI_GROUP6_PIN3 */
        {
            Acquisition_TSI_PIN(TSI_GROUP6_PIN3);

            if(TSI_GetIntBitState(TSI_STR_CTCF) == (uint8_t)SET)
            {
                SampleCycle[TSI_PIN3] = TSI_GetGroupSampleCycle(TSI_GROUP6);
            }
            else
            {
                /* Max Count Error */
            }

            /* Light LED2 */
            if((Sample_Refervalue_Average[1]-SampleCycle[TSI_PIN3]) > 0x20)
            {
                /* TSI_GROUP6_PIN3 is touched */
                touch_flag[1] = 1;
                GD_EVAL_LEDOn(LED2);
            }
            else
            {
                touch_flag[1] = 0;
                GD_EVAL_LEDOff(LED2);
            }
            TSI_ChannelModeConfig(TSI_GROUP6_PIN3, DISABLE);
        }

        /* Acquisition TSI_GROUP6_PIN4 */
        {
            Acquisition_TSI_PIN(TSI_GROUP6_PIN4);

            if(TSI_GetIntBitState(TSI_STR_CTCF) == (uint8_t)SET)
            {
                SampleCycle[TSI_PIN4] = TSI_GetGroupSampleCycle(TSI_GROUP6);
            }
            else
            {
                /* Max Count Error */
            }

            /* Light LED3 */
            if((Sample_Refervalue_Average[2]-SampleCycle[TSI_PIN4]) > 0x20)
            {
                /* TSI_GROUP6_PIN4 is touched */
                touch_flag[2] = 1;
                GD_EVAL_LEDOn(LED3);
            }
            else
            {
                touch_flag[2] = 0;
                GD_EVAL_LEDOff(LED3);
            }
            TSI_ChannelModeConfig(TSI_GROUP6_PIN4, DISABLE);
        }

        /* Judge specific location of channel2 */
        if(touch_flag[0] == 1)  
        {
            if((SampleCycle[TSI_PIN3]-SampleCycle[TSI_PIN2]+threshold1)<(SampleCycle[TSI_PIN4]-SampleCycle[TSI_PIN2]))
            {
                /* Light LED1 when touch the left location */
                GD_EVAL_LEDOn(LED1);
            }
            else if((SampleCycle[TSI_PIN3]-SampleCycle[TSI_PIN2]+threshold2)>(SampleCycle[TSI_PIN4]-SampleCycle[TSI_PIN2]))
            {
                /* Light LED4 when touch the right location */
                GD_EVAL_LEDOn(LED4);
            }
        }

        /* Delay for a period of time while all banks have been acquired */
        Delay(0x1FFFFF);
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
    /* GPIOB11 */
    /* Alternate function Output Open-Drain for Sampling Capacitor IOs */
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_2MHZ;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* GPIOB12
       GPIOB13
       GPIOB14 */
    /* Alternate function Output Push-Pull for Channel and Shield IOs */
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_12| GPIO_PIN_13| GPIO_PIN_14;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Connect pin to Peripheral */
    GPIO_PinAFConfig(GPIOB, GPIO_PINSOURCE11, GPIO_AF_3);
    GPIO_PinAFConfig(GPIOB, GPIO_PINSOURCE12, GPIO_AF_3);
    GPIO_PinAFConfig(GPIOB, GPIO_PINSOURCE13, GPIO_AF_3);
    GPIO_PinAFConfig(GPIOB, GPIO_PINSOURCE14, GPIO_AF_3);
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
  
    TSI_BaseInitStruct.TSI_PulsePrescaler = TSI_CTCDIV_32;
    TSI_BaseInitStruct.TSI_pulseLow       = TSI_CTDT_2CTCLK;
    TSI_BaseInitStruct.TSI_pulseHigh      = TSI_CDT_2CTCLK;
    TSI_BaseInitStruct.TSI_TriggerMode    = TSI_SOFTWARE_TRIGGLER;
    TSI_BaseInitStruct.TSI_EageSelect     = TSI_RISING_EDGE_TRIGGLER; 
    TSI_BaseInitStruct.TSI_MaxCycle       = TSI_MCN_2047_CYCLE;
    TSI_BaseInit(&TSI_BaseInitStruct);

    TSI_GroupInitStruct.TSI_Group      = TSI_GROUP6;
    TSI_GroupInitStruct.TSI_SamplePin  = TSI_GROUP6_PIN1;
    TSI_GroupInitStruct.TSI_ChannelPin = TSI_GROUP6_PIN2|TSI_GROUP6_PIN3|TSI_GROUP6_PIN4;
    TSI_GroupInit(&TSI_GroupInitStruct);

    /* Disable Hysteresis mode */
    TSI_PinHysteresisConfig(TSI_GROUP6_PIN1|
                            TSI_GROUP6_PIN2|
                            TSI_GROUP6_PIN3|
                            TSI_GROUP6_PIN4, DISABLE);
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
    GD_EVAL_LEDInit(LED3);
    GD_EVAL_LEDInit(LED4);

    /* Close all of LEDs */
    GD_EVAL_LEDOff(LED1);
    GD_EVAL_LEDOff(LED2);
    GD_EVAL_LEDOff(LED3);
    GD_EVAL_LEDOff(LED4);
}

/**
  * @brief  Acquisition TSI_GROUP6_PINx.
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
