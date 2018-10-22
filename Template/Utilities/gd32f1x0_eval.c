/**
  ******************************************************************************
  * @file    gd32f1x0_eval.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Firmware functions to manage Leds, Keys, COM ports
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_eval.h"


/** @addtogroup Utilities
  * @{
  */ 

/** @addtogroup GD32_EVAL
  * @{
  */ 

/** @addtogroup GD32F1X0_EVAL
  * @{
  */   
    
/** @defgroup GD32F1X0_EVAL_LOW_LEVEL
  * @{
  */ 

/* Private variables ---------------------------------------------------------*/
GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, 
                                 LED2_GPIO_PORT, 
                                 LED3_GPIO_PORT,
                                 LED4_GPIO_PORT};

const uint16_t GPIO_PIN[LEDn] = {LED1_PIN,
                                 LED2_PIN, 
                                 LED3_PIN, 
                                 LED4_PIN};

const uint32_t GPIO_CLK[LEDn] = {LED1_GPIO_CLK,
                                 LED2_GPIO_CLK, 
                                 LED3_GPIO_CLK, 
                                 LED4_GPIO_CLK};

GPIO_TypeDef* KEY_PORT[KEYn] = {WAKEUP_KEY_GPIO_PORT, 
                                TAMPER_KEY_GPIO_PORT,
#ifdef GD32F130_150                                
                                USER_KEY_GPIO_PORT
#endif
                                };

const uint16_t KEY_PIN[KEYn] = {WAKEUP_KEY_PIN, 
                                TAMPER_KEY_PIN,
#ifdef GD32F130_150
                                USER_KEY_PIN
#endif
                                };

const uint32_t KEY_CLK[KEYn] = {WAKEUP_KEY_GPIO_CLK, 
                                TAMPER_KEY_GPIO_CLK,
#ifdef GD32F130_150
                                USER_KEY_GPIO_CLK
#endif
                                };

const uint16_t KEY_EXTI_LINE[KEYn] = {WAKEUP_KEY_EXTI_LINE,
                                      TAMPER_KEY_EXTI_LINE,
#ifdef GD32F130_150
                                      USER_KEY_EXTI_LINE
#endif
                                      };

const uint16_t KEY_PORT_SOURCE[KEYn] = {WAKEUP_KEY_EXTI_PORT_SOURCE,
                                        TAMPER_KEY_EXTI_PORT_SOURCE,
#ifdef GD32F130_150
                                        USER_KEY_EXTI_PORT_SOURCE
#endif
                                        };

const uint16_t KEY_PIN_SOURCE[KEYn] = {WAKEUP_KEY_EXTI_PIN_SOURCE,
                                       TAMPER_KEY_EXTI_PIN_SOURCE,
#ifdef GD32F130_150
                                       USER_KEY_EXTI_PIN_SOURCE
#endif
                                       };

const uint16_t KEY_IRQn[KEYn] = {WAKEUP_KEY_EXTI_IRQn, 
                                 TAMPER_KEY_EXTI_IRQn,
#ifdef GD32F130_150
                                 USER_KEY_EXTI_IRQn
#endif
                                 };

/** @defgroup GD321X0_EVAL_LOW_LEVEL_Private_Functions
  * @{
  */

/**
  * @brief  Configures LED GPIO
  * @param  LedNum: Specifies the Led to be configured
  *         This parameter can be one of following parameters:
  *            @arg LED1
  *            @arg LED2
  *            @arg LED3
  *            @arg LED4
  * @retval None
  */
void  GD_EVAL_LEDInit (Led_TypeDef LedNum)
{
    GPIO_InitPara  GPIO_InitStructure;

    /* Enable the LED Clock */
    RCC_AHBPeriphClock_Enable(GPIO_CLK[LedNum], ENABLE);

    /* Configure the LED pin */
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN[LedNum];
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_Init(GPIO_PORT[LedNum], &GPIO_InitStructure);
    GPIO_PORT[LedNum]->BCR = GPIO_PIN[LedNum] ;
}

/**
  * @brief  Turn on selected LED 
  * @param  LedNum: Specifies the Led to be turn on
  *         This parameter can be one of following parameters:
  *            @arg LED1
  *            @arg LED2
  *            @arg LED3
  *            @arg LED4
  * @retval None
  */
void GD_EVAL_LEDOn(Led_TypeDef LedNum)
{
    GPIO_PORT[LedNum]->BOR = GPIO_PIN[LedNum];
}

/**
  * @brief  Turn off selected LED
  * @param  Led: Specifies the Led to be turn off
  *         This parameter can be one of following parameters:
  *            @arg LED1
  *            @arg LED2
  *            @arg LED3
  *            @arg LED4
  * @retval None
  */
void GD_EVAL_LEDOff(Led_TypeDef LedNum)
{
    GPIO_PORT[LedNum]->BCR = GPIO_PIN[LedNum];
}

/**
  * @brief  Toggle the selected LED.
  * @param  Led: Specifies the Led to be toggled
  *         This parameter can be one of following parameters:
  *            @arg LED1
  *            @arg LED2
  *            @arg LED3
  *            @arg LED4
  * @retval None
  */
void GD_EVAL_LEDToggle(Led_TypeDef LedNum)
{
#ifdef GD32F130_150
    GPIO_PORT[LedNum]->DOR ^= GPIO_PIN[LedNum];
#elif defined(GD32F170_190)
    GPIO_PORT[LedNum]->TGR = GPIO_PIN[LedNum];
#endif
}

/**
  * @brief  Configure Key
  * @param  KeyNum: Specifies the key to be configured.
  *         This parameter can be one of following parameters:
  *            @arg KEY_TAMPER: Tamper key
  *            @arg KEY_WAKEUP: Wakeup key 
  *            @arg KEY_USER: User key 
  * @param  Button_Mode: Specifies Button mode.
  *         This parameter can be one of following parameters:   
  *            @arg KEY_MODE_GPIO: key will be used as simple IO 
  *            @arg KEY_MODE_EXTI: Key will be connected to EXTI line with interrupt
  *                                generation capability
  * @retval None
  */
void GD_EVAL_KeyInit(Key_TypeDef KeyNum, KeyMode_TypeDef Key_Mode)
{
    GPIO_InitPara GPIO_InitStructure;
    EXTI_InitPara EXTI_InitStructure;
    NVIC_InitPara NVIC_InitStructure;

    /* Enable the KEY Clock */
    RCC_AHBPeriphClock_Enable(KEY_CLK[KeyNum], ENABLE);
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_CFG, ENABLE);

    /* Configure Button pin as input */
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_InitStructure.GPIO_Pin = KEY_PIN[KeyNum];
    GPIO_Init(KEY_PORT[KeyNum], &GPIO_InitStructure);

    if (Key_Mode == KEY_MODE_EXTI)
    {
        /* Enable and set key EXTI Interrupt to the lowest priority */
        NVIC_InitStructure.NVIC_IRQ = KEY_IRQn[KeyNum];
        NVIC_InitStructure.NVIC_IRQPreemptPriority = 2;
        NVIC_InitStructure.NVIC_IRQSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        /* Connect key EXTI Line to key GPIO Pin */
        SYSCFG_EXTILine_Config(KEY_PORT_SOURCE[KeyNum], KEY_PIN_SOURCE[KeyNum]);

        /* Configure key EXTI line */
        EXTI_InitStructure.EXTI_LINE = KEY_EXTI_LINE[KeyNum];
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   
        EXTI_InitStructure.EXTI_LINEEnable = ENABLE;
        EXTI_Init(&EXTI_InitStructure);

        EXTI_ClearIntBitState(KEY_EXTI_LINE[KeyNum]);
    }
}

/**
  * @brief  Returns the selected Button state.
  * @param  Button: Specifies the Button to be checked.
  *         This parameter can be one of following parameters:
  *            @arg KEY_TAMPER: Tamper key
  *            @arg KEY_WAKEUP: Wakeup key
  *            @arg KEY_USER: User key
  * @retval The Button GPIO pin value.
  */
uint8_t GD_EVAL_KeyGetState(Key_TypeDef Button)
{
    return GPIO_ReadInputBit(KEY_PORT[Button], KEY_PIN[Button]);
}

/**
  * @brief  Configures COM port
  * @param  COM: Specifies the COM port to be configured
  *         This parameter can be one of following parameters:
  *              @arg COM1
  * @param  USART_InitStruct: pointer to a USART_InitTypeDef structure that
  *         contains the configuration information for the specified USART peripheral
  * @retval None
  */
void GD_EVAL_COMInit(USART_InitPara * USART_InitStruct)
{
    GPIO_InitPara GPIO_InitStructure;

    /* Enable GPIO clock */
    RCC_AHBPeriphClock_Enable(EVAL_COM1_TX_GPIO_CLK | EVAL_COM1_RX_GPIO_CLK, ENABLE);

#if defined(GD32F130_150)
    /* Enable USART clock */
    RCC_APB2PeriphClock_Enable(EVAL_COM1_CLK, ENABLE);
    
#elif defined(GD32F170_190)
    /* Enable USART clock */
    RCC_APB1PeriphClock_Enable(EVAL_COM1_CLK, ENABLE);

#else
    #error "Please define GD32F130_150 or GD32F170_190"

#endif 

    /* Connect PXx to USARTx_Tx */
    GPIO_PinAFConfig(EVAL_COM1_TX_GPIO_PORT, EVAL_COM1_TX_SOURCE, EVAL_COM1_TX_AF);

    /* Connect PXx to USARTx_Rx */
    GPIO_PinAFConfig(EVAL_COM1_RX_GPIO_PORT, EVAL_COM1_RX_SOURCE, EVAL_COM1_TX_AF);

    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = EVAL_COM1_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_10MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_PULLUP;
    GPIO_Init(EVAL_COM1_TX_GPIO_PORT, &GPIO_InitStructure);

    /* Configure USART Rx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = EVAL_COM1_RX_PIN;
    GPIO_Init(EVAL_COM1_RX_GPIO_PORT, &GPIO_InitStructure);

    /* USART configuration */
    USART_Init(EVAL_COM1, USART_InitStruct);

    /* Enable USART */
    USART_Enable(EVAL_COM1, ENABLE);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/

