/**
  ******************************************************************************
  * @file    TIMER/TIMER1_6-steps/gd32f1x0_it.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_it.h"
#include "systick.h"


__IO uint32_t step = 1;

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    TIMER_GenerateEvent(TIMER1,TIMER_EVENT_SRC_COM);
}




void TIMER1_BRK_UP_TRG_COM_IRQHandler(void)
{

    TIMER_ClearBitState(TIMER1,TIMER_FLAG_COM);
    switch(step)
        {
        /* Next step: Step 2 Configuration .A-C` breakover---------------------------- */
    
        case 1: 
            /*  Channel1 configuration */
            TIMER_OCxModeConfig(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM1);
            TIMER_CCxCmd(TIMER1,TIMER_CH_1,TIMER_CCX_ENABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_1,TIMER_CCXN_DISABLE);
            /*  Channel2 configuration */
            TIMER_CCxCmd(TIMER1,TIMER_CH_2,TIMER_CCX_DISABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_2,TIMER_CCXN_DISABLE);
            /*  Channel3 configuration */
            TIMER_OCxModeConfig(TIMER1,TIMER_CH_3,TIMER_OC_MODE_PWM1);
            TIMER_CCxCmd(TIMER1,TIMER_CH_3,TIMER_CCX_DISABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_3,TIMER_CCXN_ENABLE);

            step++;
            break; 

    /* Next step: Step 3 Configuration .B-C` breakover---------------------------- */
        case 2:
            /*  Channel1 configuration */
            TIMER_CCxCmd(TIMER1,TIMER_CH_1,TIMER_CCX_DISABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_1,TIMER_CCXN_DISABLE);
            /*  Channel2 configuration */
            TIMER_OCxModeConfig(TIMER1,TIMER_CH_2,TIMER_OC_MODE_PWM1);
            TIMER_CCxCmd(TIMER1,TIMER_CH_2,TIMER_CCX_ENABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_2,TIMER_CCXN_DISABLE);    
            /*  Channel3 configuration */
            TIMER_OCxModeConfig(TIMER1,TIMER_CH_3,TIMER_OC_MODE_PWM1);       
            TIMER_CCxCmd(TIMER1,TIMER_CH_3,TIMER_CCX_DISABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_3,TIMER_CCXN_ENABLE);

            step++;
            break;
        
        

    /* Next step: Step 4 Configuration .B-A` breakover---------------------------- */
        case 3:
            /*  Channel1 configuration */    
            TIMER_OCxModeConfig(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM1);
            TIMER_CCxCmd(TIMER1,TIMER_CH_1,TIMER_CCX_DISABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_1,TIMER_CCXN_ENABLE);
            /*  Channel2 configuration */
            TIMER_OCxModeConfig(TIMER1,TIMER_CH_2,TIMER_OC_MODE_PWM1);
            TIMER_CCxCmd(TIMER1,TIMER_CH_2,TIMER_CCX_ENABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_2,TIMER_CCXN_DISABLE);
            /*  Channel3 configuration */
            TIMER_CCxCmd(TIMER1,TIMER_CH_3,TIMER_CCX_DISABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_3,TIMER_CCXN_DISABLE);

            step++;
            break;
        

    /* Next step: Step 5 Configuration .C-A` breakover---------------------------- */
        case 4:
            /*  Channel1 configuration */
            TIMER_OCxModeConfig(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM1);
            TIMER_CCxCmd(TIMER1,TIMER_CH_1,TIMER_CCX_DISABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_1,TIMER_CCXN_ENABLE);

            /*  Channel2 configuration */       
            TIMER_CCxCmd(TIMER1,TIMER_CH_2,TIMER_CCX_DISABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_2,TIMER_CCXN_DISABLE);

            /*  Channel3 configuration */
            TIMER_OCxModeConfig(TIMER1,TIMER_CH_3,TIMER_OC_MODE_PWM1);
            TIMER_CCxCmd(TIMER1,TIMER_CH_3,TIMER_CCX_ENABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_3,TIMER_CCXN_DISABLE);

            step++;
            break;

    /* Next step: Step 6 Configuration .C-B` breakover---------------------------- */
        case 5:
            /*  Channel1 configuration */
            TIMER_CCxCmd(TIMER1,TIMER_CH_1,TIMER_CCX_DISABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_1,TIMER_CCXN_DISABLE);

            /*  Channel2 configuration */
            TIMER_OCxModeConfig(TIMER1,TIMER_CH_2,TIMER_OC_MODE_PWM1);  
            TIMER_CCxCmd(TIMER1,TIMER_CH_2,TIMER_CCX_DISABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_2,TIMER_CCXN_ENABLE);

            /*  Channel3 configuration */
            TIMER_OCxModeConfig(TIMER1,TIMER_CH_3,TIMER_OC_MODE_PWM1); 
            TIMER_CCxCmd(TIMER1,TIMER_CH_3,TIMER_CCX_ENABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_3,TIMER_CCXN_DISABLE);

            step++;
            break;
  
    /* Next step: Step 1 Configuration .A-B` breakover---------------------------- */        
        case 6:
            /*  Channel1 configuration */
            TIMER_OCxModeConfig(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM1);
            TIMER_CCxCmd(TIMER1,TIMER_CH_1,TIMER_CCX_ENABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_1,TIMER_CCXN_DISABLE);

            /*  Channel2 configuration */
            TIMER_OCxModeConfig(TIMER1,TIMER_CH_2,TIMER_OC_MODE_PWM1);
            TIMER_CCxCmd(TIMER1,TIMER_CH_2,TIMER_CCX_DISABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_2,TIMER_CCXN_ENABLE);

            /*  Channel3 configuration */
            TIMER_CCxCmd(TIMER1,TIMER_CH_3,TIMER_CCX_DISABLE);
            TIMER_CCxNCmd(TIMER1,TIMER_CH_3,TIMER_CCXN_DISABLE);

            step = 1;
            break;
            }

}


/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
