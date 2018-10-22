/**
  ******************************************************************************
  * @file    gd32f1x0_eval.h
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   GD32f1x0_EVAL's Leds, Keys and COM ports hardware resources defines.
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GD32F1X0_EVAL_H
#define __GD32F1X0_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup GD32_EVAL
  * @{
  */

/** @addtogroup GD32F1X0_EVAL
  * @{
  */
      
/** @addtogroup GD32F1X0_EVAL_LOW_LEVEL
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
typedef enum 
{
    LED1 = 0,
    LED2 = 1,
    LED3 = 2,
    LED4 = 3
} 
Led_TypeDef;

typedef enum 
{
    KEY_WAKEUP = 0,
    KEY_TAMPER = 1,
#ifdef GD32F130_150
    KEY_USER = 2
#endif
}
Key_TypeDef;

typedef enum 
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
}
KeyMode_TypeDef;

/** @defgroup GD32F1X0_EVAL_LOW_LEVEL_Exported_Constants
  * @{
  */ 

#if defined(GD32F130_150)

/** 
  * @brief  Define for GD32130_150_EVAL board  
  */

/** @addtogroup GD32130_150_EVAL_LOW_LEVEL_LED
  * @{
  */
#define LEDn                             4

#define LED1_PIN                         GPIO_PIN_10
#define LED1_GPIO_PORT                   GPIOC
#define LED1_GPIO_CLK                    RCC_AHBPERIPH_GPIOC
  
#define LED2_PIN                         GPIO_PIN_11
#define LED2_GPIO_PORT                   GPIOC
#define LED2_GPIO_CLK                    RCC_AHBPERIPH_GPIOC
  
#define LED3_PIN                         GPIO_PIN_12
#define LED3_GPIO_PORT                   GPIOC
#define LED3_GPIO_CLK                    RCC_AHBPERIPH_GPIOC
  
#define LED4_PIN                         GPIO_PIN_2
#define LED4_GPIO_PORT                   GPIOD
#define LED4_GPIO_CLK                    RCC_AHBPERIPH_GPIOD

/**
  * @}
  */ 

/** @addtogroup GD32130_150_EVAL_LOW_LEVEL_BUTTON
  * @{
  */  
#define KEYn                             3

/**
  * @brief Tamper push-button
  */
#define TAMPER_KEY_PIN                     GPIO_PIN_13
#define TAMPER_KEY_GPIO_PORT               GPIOC
#define TAMPER_KEY_GPIO_CLK                RCC_AHBPERIPH_GPIOC
#define TAMPER_KEY_EXTI_LINE               EXTI_LINE13
#define TAMPER_KEY_EXTI_PORT_SOURCE        EXTI_SOURCE_GPIOC
#define TAMPER_KEY_EXTI_PIN_SOURCE         EXTI_SOURCE_PIN13
#define TAMPER_KEY_EXTI_IRQn               EXTI4_15_IRQn

/**
  * @brief Wakeup push-button
  */
#define WAKEUP_KEY_PIN                     GPIO_PIN_0
#define WAKEUP_KEY_GPIO_PORT               GPIOA
#define WAKEUP_KEY_GPIO_CLK                RCC_AHBPERIPH_GPIOA
#define WAKEUP_KEY_EXTI_LINE               EXTI_LINE0
#define WAKEUP_KEY_EXTI_PORT_SOURCE        EXTI_SOURCE_GPIOA
#define WAKEUP_KEY_EXTI_PIN_SOURCE         EXTI_SOURCE_PIN0
#define WAKEUP_KEY_EXTI_IRQn               EXTI0_1_IRQn  

/**
  * @brief User push-button
  */
#define USER_KEY_PIN                       GPIO_PIN_7
#define USER_KEY_GPIO_PORT                 GPIOF
#define USER_KEY_GPIO_CLK                  RCC_AHBPERIPH_GPIOF
#define USER_KEY_EXTI_LINE                 EXTI_LINE7
#define USER_KEY_EXTI_PORT_SOURCE          EXTI_SOURCE_GPIOF
#define USER_KEY_EXTI_PIN_SOURCE           EXTI_SOURCE_PIN7
#define USER_KEY_EXTI_IRQn                 EXTI4_15_IRQn

/**
  * @}
  */

/** @addtogroup GD32130_150_EVAL_LOW_LEVEL_COM
  * @{
  */

/**
 * @brief Definition for COM port1, connected to USART1
 */ 
#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCC_APB2PERIPH_USART1

#define EVAL_COM1_TX_PIN                 GPIO_PIN_9
#define EVAL_COM1_TX_GPIO_PORT           GPIOA
#define EVAL_COM1_TX_GPIO_CLK            RCC_AHBPERIPH_GPIOA
#define EVAL_COM1_TX_SOURCE              GPIO_PINSOURCE9
#define EVAL_COM1_TX_AF                  GPIO_AF_1

#define EVAL_COM1_RX_PIN                 GPIO_PIN_10
#define EVAL_COM1_RX_GPIO_PORT           GPIOA
#define EVAL_COM1_RX_GPIO_CLK            RCC_AHBPERIPH_GPIOA
#define EVAL_COM1_RX_SOURCE              GPIO_PINSOURCE10
#define EVAL_COM1_RX_AF                  GPIO_AF_1

#define EVAL_COM1_CTS_PIN                GPIO_PIN_3
#define EVAL_COM1_CTS_GPIO_PORT          GPIOD
#define EVAL_COM1_CTS_GPIO_CLK           RCC_AHBPERIPH_GPIOD
#define EVAL_COM1_CTS_SOURCE             GPIO_PINSOURCE3
#define EVAL_COM1_CTS_AF                 GPIO_AF_0

#define EVAL_COM1_RTS_PIN                GPIO_PIN_4
#define EVAL_COM1_RTS_GPIO_PORT          GPIOD
#define EVAL_COM1_RTS_GPIO_CLK           RCC_AHBPERIPH_GPIOD
#define EVAL_COM1_RTS_SOURCE             GPIO_PINSOURCE4
#define EVAL_COM1_RTS_AF                 GPIO_AF_0
   
#define EVAL_COM1_IRQn                   USART1_IRQn

/**
  * @}
  */

#elif defined(GD32F170_190)

/** @addtogroup GD32170_190_EVAL_LOW_LEVEL_LED
  * @{
  */
#define LEDn                             4

#define LED1_PIN                         GPIO_PIN_11
#define LED1_GPIO_PORT                   GPIOA
#define LED1_GPIO_CLK                    RCC_AHBPERIPH_GPIOA
  
#define LED2_PIN                         GPIO_PIN_12
#define LED2_GPIO_PORT                   GPIOA
#define LED2_GPIO_CLK                    RCC_AHBPERIPH_GPIOA
  
#define LED3_PIN                         GPIO_PIN_6
#define LED3_GPIO_PORT                   GPIOB
#define LED3_GPIO_CLK                    RCC_AHBPERIPH_GPIOB
  
#define LED4_PIN                         GPIO_PIN_7
#define LED4_GPIO_PORT                   GPIOB
#define LED4_GPIO_CLK                    RCC_AHBPERIPH_GPIOB

/**
  * @}
  */ 

/** @addtogroup GD32170_190_EVAL_LOW_LEVEL_BUTTON
  * @{
  */  
#define KEYn                             2

/**
  * @brief Tamper push-button
  */
#define TAMPER_KEY_PIN                     GPIO_PIN_13
#define TAMPER_KEY_GPIO_PORT               GPIOC
#define TAMPER_KEY_GPIO_CLK                RCC_AHBPERIPH_GPIOC
#define TAMPER_KEY_EXTI_LINE               EXTI_LINE13
#define TAMPER_KEY_EXTI_PORT_SOURCE        EXTI_SOURCE_GPIOC
#define TAMPER_KEY_EXTI_PIN_SOURCE         EXTI_SOURCE_PIN13
#define TAMPER_KEY_EXTI_IRQn               EXTI4_15_IRQn

/**
  * @brief Wakeup push-button
  */
#define WAKEUP_KEY_PIN                     GPIO_PIN_0
#define WAKEUP_KEY_GPIO_PORT               GPIOA
#define WAKEUP_KEY_GPIO_CLK                RCC_AHBPERIPH_GPIOA
#define WAKEUP_KEY_EXTI_LINE               EXTI_LINE0
#define WAKEUP_KEY_EXTI_PORT_SOURCE        EXTI_SOURCE_GPIOA
#define WAKEUP_KEY_EXTI_PIN_SOURCE         EXTI_SOURCE_PIN0
#define WAKEUP_KEY_EXTI_IRQn               EXTI0_1_IRQn  

/**
  * @}
  */

/** @addtogroup GD32170_190_EVAL_LOW_LEVEL_COM
  * @{
  */

/**
  * @brief Definition for COM port2, connected to USART2
  */ 
#define EVAL_COM1                        USART2
#define EVAL_COM1_CLK                    RCC_APB1PERIPH_USART2

#define EVAL_COM1_TX_PIN                 GPIO_PIN_2
#define EVAL_COM1_TX_GPIO_PORT           GPIOA
#define EVAL_COM1_TX_GPIO_CLK            RCC_AHBPERIPH_GPIOA
#define EVAL_COM1_TX_SOURCE              GPIO_PINSOURCE2
#define EVAL_COM1_TX_AF                  GPIO_AF_1

#define EVAL_COM1_RX_PIN                 GPIO_PIN_3
#define EVAL_COM1_RX_GPIO_PORT           GPIOA
#define EVAL_COM1_RX_GPIO_CLK            RCC_AHBPERIPH_GPIOA
#define EVAL_COM1_RX_SOURCE              GPIO_PINSOURCE3
#define EVAL_COM1_RX_AF                  GPIO_AF_1

#define EVAL_COM1_IRQn                   USART2_IRQn

/**
  * @}
  */

#else
    #error "Please define GD32F130_150 or GD32F170_190"
#endif

/** @defgroup GD321X0_EVAL_LOW_LEVEL_Exported_Functions
  * @{
  */
void GD_EVAL_LEDInit          (Led_TypeDef LedNum);
void GD_EVAL_LEDOn            (Led_TypeDef LedNum);
void GD_EVAL_LEDOff           (Led_TypeDef LedNum);
void GD_EVAL_LEDToggle        (Led_TypeDef LedNum);
void GD_EVAL_KeyInit          (Key_TypeDef KeyNum, KeyMode_TypeDef Key_Mode);
uint8_t GD_EVAL_KeyGetState   (Key_TypeDef KeyNum);
void GD_EVAL_COMInit          (USART_InitPara* USART_InitStruct);

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __GD32F1x0_EVAL_H */

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
