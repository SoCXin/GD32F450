/**    
  ******************************************************************************
  * @file    IFRR/IFRR_InfraredTransmitReceive/ir_decode.h
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   The head file of Infrared decoding.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IR_DECODE_H
#define __IR_DECODE_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"

/* Exported types ------------------------------------------------------------*/
typedef enum { NO = 0, YES = !NO} StatusYesOrNo;

/* Exported define ---------------------------------------------------------- */
#if defined(GD32F130_150)
  #define IR_TIM                 TIMER3                   /*!< Timer used for IR decoding */
  #define TIM_PRESCALER          71                       /*!< TIMER prescaler */
  #define IR_TIM_CLK             RCC_APB1PERIPH_TIMER3    /*!< Clock of the used timer */
  #define IR_TIM_IRQn            TIMER3_IRQn              /*!< IR TIMER IRQ */
  #define IR_TIM_Channel         TIMER_CH_1               /*!< IR TIMER Channel */

  #define IR_GPIO_PORT           GPIOC                    /*!< Port which IR output is connected */
  #define IR_GPIO_PORT_CLK       RCC_AHBPERIPH_GPIOC      /*!< IR pin GPIO Clock Port */
  #define IR_GPIO_PIN            GPIO_PIN_6               /*!< Pin which IR is connected */
  #define IR_GPIO_SOURCE         GPIO_PINSOURCE6

#elif defined(GD32F170_190)
  #define IR_TIM                 TIMER3                   /*!< Timer used for IR decoding */
  #define TIM_PRESCALER          71                       /*!< TIMER prescaler */
  #define IR_TIM_CLK             RCC_APB1PERIPH_TIMER3    /*!< Clock of the used timer */
  #define IR_TIM_IRQn            TIMER3_IRQn              /*!< IR TIMER IRQ */
  #define IR_TIM_Channel         TIMER_CH_2               /*!< IR TIMER Channel */

  #define IR_GPIO_PORT           GPIOA                    /*!< Port which IR output is connected */
  #define IR_GPIO_PORT_CLK       RCC_AHBPERIPH_GPIOA      /*!< IR pin GPIO Clock Port */
  #define IR_GPIO_PIN            GPIO_PIN_7               /*!< Pin which IR is connected */
  #define IR_GPIO_SOURCE         GPIO_PINSOURCE7
#endif

#endif /* __IR_DECODE_H */

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
