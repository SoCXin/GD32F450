/**
  ******************************************************************************
  * @file    TIMER/TIMER3_InputCapture/main.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <gd32f1x0.h>
#include "gd32f1x0_eval.h"
#include "systick.h"
#include <stdio.h>

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void TIMER_Configuration(void);
int fputc(int ch, FILE *f);
void USART_Configuration(void);
void NVIC_Configuration(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Retarget the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    USART_DataSend( EVAL_COM1, (uint8_t) ch );

    /* Loop until transmit data register is empty */
    while (USART_GetBitState ( EVAL_COM1 , USART_FLAG_TC ) == RESET);

    return ch;
}

/**
  * @brief  Configure the USART peripheral.
  * @param  None
  * @retval None
  */
void USART_Configuration(void)
{
    USART_InitPara USART_InitStructure;

    USART_InitStructure.USART_BRR                 = 115200;
    USART_InitStructure.USART_WL                  = USART_WL_8B;
    USART_InitStructure.USART_STBits              = USART_STBITS_1;
    USART_InitStructure.USART_Parity              = USART_PARITY_RESET;
    USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE;
    USART_InitStructure.USART_RxorTx              = USART_RXORTX_RX | USART_RXORTX_TX;

    GD_EVAL_COMInit(&USART_InitStructure);
}

/**
  * @brief  Configure system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
    /* GPIOA, GPIOB, GPIOC and GPIOF clocks enable */
    RCC_AHBPeriphClock_Enable( RCC_AHBPERIPH_GPIOA |
                               RCC_AHBPERIPH_GPIOB |
                               RCC_AHBPERIPH_GPIOF  , ENABLE);
}

/**
  * @brief  Configure the GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
    GPIO_InitPara GPIO_InitStructure;

    /*Configure PB4 PB5 PB0 PB1(TIMER3 CH1 CH2 CH3 CH4) as alternate function*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE0,GPIO_AF_1);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE1,GPIO_AF_1);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE4,GPIO_AF_1);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE5,GPIO_AF_1);
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
    NVIC_InitPara NVIC_InitStructure;

    NVIC_PRIGroup_Enable(NVIC_PRIGROUP_1);
    /* Enable the TIMER3 Interrupt */
    NVIC_InitStructure.NVIC_IRQ                = TIMER3_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 1;
    NVIC_InitStructure.NVIC_IRQSubPriority     = 1;
    NVIC_InitStructure.NVIC_IRQEnable          = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configure the TIMER peripheral.
  * @param  None
  * @retval None
  */
void TIMER_Configuration(void)
{
    /* TIMER3 configuration: Input Capture mode ---------------------
    The external signal is connected to TIMER3 CH1 pin (PB4)
    The Rising edge is used as active edge
    The TIMER3 CHCC1 is used to compute the frequency value
    ------------------------------------------------------------ */

    TIMER_BaseInitPara TIMER_TimeBaseStructure;
    TIMER_ICInitPara   TIMER_ICInitStructure;;

    /* TIMERS clock enable */
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_TIMER1|RCC_APB2PERIPH_TIMER15|RCC_APB2PERIPH_TIMER16|RCC_APB2PERIPH_TIMER17,ENABLE);
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_TIMER2|RCC_APB1PERIPH_TIMER3|RCC_APB1PERIPH_TIMER6|RCC_APB1PERIPH_TIMER14,ENABLE);

    /* TIMER3  configuration */
    TIMER_DeInit(TIMER3);
    TIMER_TimeBaseStructure.TIMER_Prescaler     = 35999;
    TIMER_TimeBaseStructure.TIMER_CounterMode   = TIMER_COUNTER_UP;
    TIMER_TimeBaseStructure.TIMER_Period        = 65535;
    TIMER_TimeBaseStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
    TIMER_BaseInit(TIMER3,&TIMER_TimeBaseStructure);

    /* Input capture configuration */
    TIMER_ICInitStructure.TIMER_CH          = TIMER_CH_1;
    TIMER_ICInitStructure.TIMER_ICPolarity  = TIMER_IC_POLARITY_RISING;
    TIMER_ICInitStructure.TIMER_ICSelection = TIMER_IC_SELECTION_DIRECTTI;
    TIMER_ICInitStructure.TIMER_ICPrescaler = TIMER_IC_PSC_DIV1;
    TIMER_ICInitStructure.TIMER_ICFilter    = 0x0;
    TIMER_ICInit(TIMER3, &TIMER_ICInitStructure);

    /* Auto-reload preload enable */
    TIMER_CARLPreloadConfig(TIMER3,ENABLE);

    /* Clear the CH1 Interrupt flag an Enable the CH1 Interrupt Request */
    TIMER_ClearIntBitState(TIMER3,TIMER_INT_CH1);
    TIMER_INTConfig(TIMER3,TIMER_INT_CH1,ENABLE);

    /* TIMER enable counter*/
    TIMER_Enable( TIMER3, ENABLE );
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main()
{
    RCC_Configuration();
    GPIO_Configuration();
    GD_EVAL_LEDInit(LED1);
    SysTick_Configuration();
    NVIC_Configuration();
    USART_Configuration();
    TIMER_Configuration();

    while(1)
    {
        GD_EVAL_LEDOn(LED1);
        Delay_1ms(1000);
        GD_EVAL_LEDOff(LED1);
        Delay_1ms(1000);
    }
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
