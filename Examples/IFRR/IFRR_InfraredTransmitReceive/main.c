/**    
  ******************************************************************************
  * @file    IFRR/IFRR_InfraredTransmitReceive/main.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   The main function file.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "rc5_decode.h"
#include "rc5_encode.h"
#include "ir_decode.h"
#include "gd32f1x0_eval.h"

/* Private variables ---------------------------------------------------------*/
__IO uint32_t TimingDelay = 0;
RC5_Frame_TypeDef RC5_FRAME;

/* Exported variables ---------------------------------------------------------*/
extern uint8_t AddressIndex;
extern uint8_t InstructionIndex;
extern RC5_Ctrl_TypeDef RC5_Ctrl1;

/*!< RC5 Frame state */ 
extern __IO StatusYesOrNo RC5FrameReceived; 
extern __IO uint8_t  Send_Operation_Completed;

/* Private function prototypes -----------------------------------------------*/
void TestStatusLedInit( void );
void FlashLED ( int Times  );
void Delay(__IO uint32_t nTime);

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{   
    int i;

    /* Init Test Status LED */ 
    TestStatusLedInit();

    /* Flash LED for test */
    FlashLED( 1 );

    /* Enable the GPIO Clock */
    RCC_AHBPeriphClock_Enable(  RCC_AHBPERIPH_GPIOA |RCC_AHBPERIPH_GPIOB |
                                RCC_AHBPERIPH_GPIOC |RCC_AHBPERIPH_GPIOF, ENABLE);

    /* Initialize the InfraRed application: RC5 Encode*/
    RC5_Encode_Init();

    /* Initialize the InfraRed application: RC5 decode*/
    RC5_Decode_Init();

    for( i=0; i<0x1000; i++ ){}

    /* test */
    {
        AddressIndex     = 0x06 ;
        InstructionIndex = 15 ;

        /*Send IR frame (ADDRESS, COMMAND)*/
        RC5_Encode_SendFrame( AddressIndex , InstructionIndex , RC5_Ctrl1 );

        while( Send_Operation_Completed == 0x00 ) 
        { 
        }

        while( RC5FrameReceived == 0x00 ) 
        { 
        }

        /* Decode the RC5 frame */
        RC5_Decode( &RC5_FRAME );
    }

    /* TransferStatus ? PASSED : FAILED */
    while (1)
    {
        if( RC5_FRAME.Command == 15 )
        {
            /* Turn on LED1 */
            GD_EVAL_LEDOn(LED1);

            /* Insert 200 ms delay */
            Delay(200);

            /* Turn on LED2 */
            GD_EVAL_LEDOn(LED2);

            /* Insert 200 ms delay */
            Delay(200);

            /* Turn on LED3 */
            GD_EVAL_LEDOn(LED3);

            /* Insert 200 ms delay */
            Delay(200);

            /* Turn on LED4 */
            GD_EVAL_LEDOn(LED4);

            /* Insert 200 ms delay */
            Delay(200);

            /* Turn off LEDs */
            GD_EVAL_LEDOff(LED1);
            GD_EVAL_LEDOff(LED2);
            GD_EVAL_LEDOff(LED3);
            GD_EVAL_LEDOff(LED4);

            /* Insert 200 ms delay */
            Delay(200);
        }
        else
        {
            /* Flash LED for status error */
            FlashLED( 1 );
        }

    }
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{ 
    TimingDelay = nTime;
  
    while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00)
    { 
        TimingDelay--;
    }
}

/**
  * @brief  Test Status Led Init.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void TestStatusLedInit( void )
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

    /* Setup SysTick Timer for 1 msec interrupts  */
    if (SysTick_Config(SystemCoreClock / 1000))
    { 
        /* Capture error */ 
        while (1);
    }
}

/**
  * @brief  Test Status Led .
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void FlashLED ( int Times  )
{
    int i;
    
    for( i = 0 ; i < Times ; i ++ )
    {
        /* Insert 200 ms delay */
        Delay(200);

        /* Turn on LEDs */
        GD_EVAL_LEDInit(LED1);
        GD_EVAL_LEDInit(LED2);
        GD_EVAL_LEDInit(LED3);
        GD_EVAL_LEDInit(LED4);

        /* Insert 200 ms delay */
        Delay(200);

        /* Turn off LEDs */
        GD_EVAL_LEDOff(LED1);
        GD_EVAL_LEDOff(LED2);
        GD_EVAL_LEDOff(LED3);
        GD_EVAL_LEDOff(LED4);
    }
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
