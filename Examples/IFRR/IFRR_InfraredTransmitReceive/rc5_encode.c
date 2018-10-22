/**    
  ******************************************************************************
  * @file    IFRR/IFRR_InfraredTransmitReceive/rc5_encode.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   The RC5 Infrared encoder file.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/        
#include "rc5_encode.h"
#include "ir_decode.h"

/* Private define ------------------------------------------------------------*/
/* RC5 high level definition*/
#define  RC5HIGHSTATE   ( (uint8_t )0x02 )
/* RC5 low level definition*/
#define  RC5LOWSTATE    ( (uint8_t )0x01 )

/* Private variables ---------------------------------------------------------*/
RC5_Ctrl_TypeDef RC5_Ctrl1 = RC5_Ctrl_Reset;

uint8_t       RC5_RealFrameLength        = 14;
uint8_t       RC5_GlobalFrameLength      = 64;
uint16_t      RC5_FrameBinaryFormat      = 0;
uint32_t      RC5_FrameManchestarFormat  = 0;
uint8_t       Send_Operation_Ready       = 0;
__IO uint8_t  Send_Operation_Completed   = 1;
uint8_t       BitsSent_Counter           = 0;

uint8_t AddressIndex = 0;
uint8_t InstructionIndex = 0;

/* Private FunctionPrototypes ----------------------------------------------------------*/
static uint16_t RC5_BinFrameGeneration( uint8_t RC5_Address, uint8_t RC5_Instruction, RC5_Ctrl_TypeDef RC5_Ctrl );
static uint32_t RC5_ManchesterConvert( uint16_t RC5_BinaryFrameFormat );

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Init Hardware ( IPs used ) for RC5 generation
  * @param  None
  * @retval None
  */
void RC5_Encode_Init( void )
{
    TIMER_BaseInitPara  TIMER_BaseStruct;
    TIMER_OCInitPara    TIMER_OCInitStructure;
    
    /* TIMER16 clock enable */
    RCC_APB2PeriphClock_Enable( RCC_APB2PERIPH_TIMER16 , ENABLE );
    
    /* TIMER17 clock enable */
    RCC_APB2PeriphClock_Enable( RCC_APB2PERIPH_TIMER17 , ENABLE );
  
    {
        GPIO_InitPara  GPIO_InitStructure;
        
        /* IR_ENCODE */
        /* PB9 IR_OUT Pin configuration: output */
        GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_9;
        GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
        GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
        GPIO_Init( GPIOB , &GPIO_InitStructure );

        GPIO_PinAFConfig( GPIOB , GPIO_PINSOURCE9, GPIO_AF_0 );
    }

    {
      {
          /* DeInit TIMER17 */
          TIMER_DeInit( TIMER17 );
          
          /* Time Base = 36Khz */
          /* Time base configuration for timer 17 */
          TIMER_BaseStruct.TIMER_Period          = 1999;
          TIMER_BaseStruct.TIMER_Prescaler       = 0x00;
          TIMER_BaseStruct.TIMER_ClockDivision   = TIMER_CDIV_DIV1;
          TIMER_BaseStruct.TIMER_CounterMode     = TIMER_COUNTER_UP;
          TIMER_BaseInit( TIMER17, &TIMER_BaseStruct );
          
          /* Prescaler configuration */
          TIMER_PrescalerConfig( TIMER17, 0, TIMER_PSC_RELOAD_NOW );
          
          /* Output Compare Timing Mode configuration: Channel 1N */
          TIMER_OCInitStructure.TIMER_OCMode         = TIMER_OC_MODE_PWM1;
          TIMER_OCInitStructure.TIMER_OutputState    = TIMER_OUTPUT_STATE_ENABLE;
          /* Set duty cycle to 25% to be compatible with RC5 specification */
          TIMER_OCInitStructure.TIMER_Pulse          = 2000/4-1; 
          TIMER_OCInitStructure.TIMER_OCPolarity     = TIMER_OC_POLARITY_HIGH;
          TIMER_OCInitStructure.TIMER_OCIdleState    = TIMER_OC_IDLE_STATE_RESET;
          TIMER_OC1_Init( TIMER17, &TIMER_OCInitStructure );
          
          /* Timer17 preload enable */
          TIMER_OC1_Preload( TIMER17, TIMER_OC_PRELOAD_ENABLE );
          
          /* Timer 17 Enable */
          TIMER_Enable( TIMER17, ENABLE );
          
          /* Enable the TIMER16 channel1 output to be connected internly to the IRTIM */
          TIMER_CtrlPWMOutputs( TIMER17, ENABLE );
      }    
  
      {
          /* DeInit TIMER16 */
          TIMER_DeInit( TIMER16 );
      
          /* Elementary period 889 us */
          /* Time Base configuration for timer 16 */
          TIMER_BaseStruct.TIMER_Prescaler          = 1;
          TIMER_BaseStruct.TIMER_CounterMode        = TIMER_COUNTER_UP;
          TIMER_BaseStruct.TIMER_Period             = 64008 /2 - 1 ;
          TIMER_BaseStruct.TIMER_ClockDivision      = 0;
          TIMER_BaseStruct.TIMER_RepetitionCounter  = 0;
          TIMER_BaseInit( TIMER16 , &TIMER_BaseStruct );
          
          /* Duty Cycle = 25% */
          /* Channel 1 Configuration in Timing mode */
          TIMER_OCInitStructure.TIMER_OCMode          = TIMER_OC_MODE_TIMING;
          TIMER_OCInitStructure.TIMER_OutputState     = TIMER_OUTPUT_STATE_ENABLE;
          TIMER_OCInitStructure.TIMER_OutputNState    = TIMER_OUTPUTN_STATE_DISABLE;
          TIMER_OCInitStructure.TIMER_Pulse           = 64008 /2 /4;
          TIMER_OCInitStructure.TIMER_OCPolarity      = TIMER_OC_POLARITY_HIGH ;
          TIMER_OCInitStructure.TIMER_OCNPolarity     = TIMER_OCN_POLARITY_HIGH;
          TIMER_OCInitStructure.TIMER_OCIdleState     = TIMER_OC_IDLE_STATE_RESET;
          TIMER_OCInitStructure.TIMER_OCNIdleState    = TIMER_OCN_IDLE_STATE_RESET;
          TIMER_OC1_Init( TIMER16, &TIMER_OCInitStructure );
            
          /* Enable the TIMER17 Interrupt */
          {
              NVIC_InitPara NVIC_InitStructure;
              NVIC_PRIGroup_Enable( NVIC_PRIGROUP_0 );
              
              NVIC_InitStructure.NVIC_IRQ                  = TIMER16_IRQn;
              NVIC_InitStructure.NVIC_IRQPreemptPriority   = 0;
              NVIC_InitStructure.NVIC_IRQSubPriority       = 0;
              NVIC_InitStructure.NVIC_IRQEnable            = ENABLE;
              NVIC_Init( &NVIC_InitStructure );
          }
      
          /* TIMER16 Main Output Enable */
          TIMER_CtrlPWMOutputs( TIMER16, ENABLE );
          
          /* TIMER IT Disable */
          TIMER_INTConfig( TIMER16, TIMER_INT_UPDATE, DISABLE );
          
          /* TIMER Disable */
          TIMER_Enable( TIMER16 , DISABLE );
      }
  }
}

/**
  * @brief  Generate and Send the RC5 frame.
  * @param  RC5_Address: the RC5 Device destination 
  * @param  RC5_Instruction: the RC5 command instruction 
  * @param  RC5_Ctrl: the RC5 Control bit.
  * @retval None
  */
void RC5_Encode_SendFrame( uint8_t RC5_Address, uint8_t RC5_Instruction, RC5_Ctrl_TypeDef RC5_Ctrl )
{
    uint16_t RC5_FrameBinaryFormat = 0;
    
    /* Generate a binary format of the Frame */
    RC5_FrameBinaryFormat = RC5_BinFrameGeneration( RC5_Address, RC5_Instruction, RC5_Ctrl );
    
    /* Generate a Manchester format of the Frame */
    RC5_FrameManchestarFormat = RC5_ManchesterConvert( RC5_FrameBinaryFormat );
    
    /* Set the Send operation Ready flag to indicate that the frame is ready to be sent */
    Send_Operation_Ready = 1;
    
    /* TIMER IT Enable */
    TIMER_INTConfig( TIMER16, TIMER_INT_UPDATE , ENABLE );
  
    /* Enable all Interrupt */
    TIMER_Enable( TIMER16 , ENABLE );
}

/**
  * @brief  Send by hardware Manchester Format RC5 Frame.
  * @param  RC5_BinaryFrameFormat: the RC5 frame in binary format.
  * @retval Noe
  */
void RC5_Encode_SignalGenerate( uint32_t RC5_ManchestarFrameFormat )
{
    uint8_t bit_msg = 0;
    
    if( (Send_Operation_Ready == 1 ) && ( BitsSent_Counter <= ( RC5_GlobalFrameLength * 2 )) )
    {
        Send_Operation_Completed = 0x00;
        bit_msg = ( uint8_t )( (RC5_ManchestarFrameFormat >> BitsSent_Counter )& 1 );
        
        if ( bit_msg== 1 )
        {
            TIMER_Forced_OC1( TIMER16, TIMER_FORCED_HIGH );
        }
        else
        {
            TIMER_Forced_OC1( TIMER16, TIMER_FORCED_LOW );
        }
        BitsSent_Counter++;
    }
    else
    {
        Send_Operation_Completed = 0x01;

        /* TIMER IT Disable */
        TIMER_INTConfig( TIMER16, TIMER_INT_UPDATE, DISABLE );
        TIMER_Enable( TIMER16, DISABLE );
        Send_Operation_Ready = 0;
        BitsSent_Counter = 0;
        TIMER_Forced_OC1( TIMER16 , TIMER_FORCED_LOW );
    }
}

/**
  * @brief  Generate the binary format of the RC5 frame.
  * @param  RC5_Address: Select the device adress.
  * @param  RC5_Instruction: Select the device instruction.
  * @param  RC5_Ctrl: Select the device control bit status.
  * @retval Binary format of the RC5 Frame.
  */
static uint16_t RC5_BinFrameGeneration( uint8_t RC5_Address, uint8_t RC5_Instruction, RC5_Ctrl_TypeDef RC5_Ctrl )
{
    uint16_t star1 = 0x2000;
    uint16_t star2 = 0x1000;
    uint16_t addr = 0;
    
    while( Send_Operation_Completed == 0x00 ) 
    { 
    }
  
    /* Check if Instruction is 128-bit length */
    if( RC5_Instruction >= 64 )
    {
        /* Reset field bit: command is 7-bit length */
        star2 = 0;
    
        /* Keep the lowest 6 bits of the command */
        RC5_Instruction &= 0x003F;
    }
    /* Instruction is 64-bit length */
    else 
    {
        /* Set field bit: command is 6-bit length */
        star2 = 0x1000;
    }
  
    Send_Operation_Ready = 0;
    RC5_FrameManchestarFormat = 0;
    RC5_FrameBinaryFormat=0;
    addr = ( (uint16_t )( RC5_Address ))<<6;
    RC5_FrameBinaryFormat =  ( star1 )|( star2 )|( RC5_Ctrl )|( addr )|( RC5_Instruction );
    return ( RC5_FrameBinaryFormat );
}

/**
  * @brief  Convert the RC5 frame from binary to Manchester Format.
  * @param  RC5_BinaryFrameFormat : the RC5 frame in binary format.
  * @retval The RC5 frame in Manchester format.
  */
static uint32_t RC5_ManchesterConvert( uint16_t RC5_BinaryFrameFormat )
{
    uint8_t i=0;
    uint16_t Mask = 1;
    uint16_t bit_format = 0;
    uint32_t ConvertedMsg =0;
    
    for ( i=0; i < RC5_RealFrameLength; i++ )
    {
        bit_format =( (( (uint16_t )( RC5_BinaryFrameFormat ))>>i )& Mask )<<i;
        ConvertedMsg = ConvertedMsg << 2;
        
        /* Manchester 1 -|_  */
        if( bit_format != 0 )
        {
            ConvertedMsg |= RC5HIGHSTATE;
        }

        /* Manchester 0 _|-  */
        else 
        {
            ConvertedMsg |= RC5LOWSTATE;
        }
    }
    return ( ConvertedMsg );
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
