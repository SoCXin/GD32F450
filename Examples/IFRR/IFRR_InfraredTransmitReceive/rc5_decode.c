/**    
  ******************************************************************************
  * @file    IFRR/IFRR_InfraredTransmitReceive/rc5_decode.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   The RC5 Infrared decoding file.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "rc5_decode.h"
#include "ir_decode.h"

/* Private variables ---------------------------------------------------------*/
/* Logic table for rising edge: every line has values corresponding to previous bit.
   In columns are actual bit values for given bit time. */
const tRC5_lastBitType RC5_logicTableRisingEdge[2][2] =
{
    /* lastbit = ZERO */
    {RC5_ZER ,RC5_INV}, 
    /* lastbit = ONE  */
    {RC5_NAN ,RC5_ZER},
};

/* Logic table for falling edge: every line has values corresponding to previous bit. 
   In columns are actual bit values for given bit time. */
const tRC5_lastBitType RC5_logicTableFallingEdge[2][2] =
{
    /* lastbit = ZERO */
    {RC5_NAN ,RC5_ONE},  
    /* lastbit = ONE  */
    {RC5_ONE ,RC5_INV},  
};

/*!< RC5 Frame state */
__IO StatusYesOrNo RC5FrameReceived = NO; 

/*!< First empty packet */
__IO tRC5_packet   RC5TmpPacket;

/* RC5  bits time definitions */
uint16_t  RC5MinT  = 0;
uint16_t  RC5MaxT  = 0;
uint16_t  RC5Min2T = 0;
uint16_t  RC5Max2T = 0;
uint32_t RC5_Data  = 0;

/*!< Timer clock */
static uint32_t TIMCLKValueKHz = 0; 

/* Private FunctionPrototypes ----------------------------------------------------------*/
static uint8_t RC5_GetPulseLength (uint16_t pulseLength);
static void RC5_modifyLastBit(tRC5_lastBitType bit);
static void RC5_WriteBit(uint8_t bitVal);
static uint32_t TIMER_GetCounterCLKValue(void);

/* Private function ----------------------------------------------------------*/
/**
  * @brief  De-initializes the peripherals (RCC,GPIO, TIMER)
  * @param  None
  * @retval None
  */
void RC5_Decode_DeInit(void)
{ 
    TIMER_DeInit( IR_TIM );
    GPIO_DeInit( IR_GPIO_PORT);
}

/**
  * @brief  Initialize the RC5 decoder module ( Time range)
  * @param  None
  * @retval None
  */
void RC5_Decode_Init(void)
{ 
  __IO uint16_t RC5TimeOut = 0;

  /*  Clock Configuration for TIMER */
  RCC_APB1PeriphClock_Enable(IR_TIM_CLK , ENABLE);

  /* Enable GPIO clock */
  RCC_AHBPeriphClock_Enable( IR_GPIO_PORT_CLK , ENABLE );

    /* IR_DECODE  */
    /* GD32150R-EVAL:TIMER3_CH1 PC6 
       GD32190R-EVAL:TIMER3_CH2 PA7
       */
    {
        /* Configure the GPIO ports */
        GPIO_InitPara  GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PUPD_NOPULL;
        GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
        GPIO_InitStructure.GPIO_Pin   = IR_GPIO_PIN ;
        GPIO_Init( IR_GPIO_PORT, &GPIO_InitStructure );

#if defined(GD32F170_190)
        GPIO_PinAFConfig( GPIOA , GPIO_PINSOURCE7, GPIO_AF_1 );
#endif
    }
  
      /* Enable the TIMER global Interrupt */
    {
        NVIC_InitPara NVIC_InitStructure;
  
        NVIC_InitStructure.NVIC_IRQ                = IR_TIM_IRQn ;
        NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
        NVIC_InitStructure.NVIC_IRQSubPriority     = 1;
        NVIC_InitStructure.NVIC_IRQEnable          = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }

    {
        TIMER_BaseInitPara TIMER_BaseStruct;
        TIMER_ICInitPara   TIMER_ICInitStruct;
  
        /* DeInit IR_TIM */
        TIMER_DeInit( IR_TIM );
          
        /* Time base configuration for timer IR_TIM */
        TIMER_BaseStruct.TIMER_Period        = 0x0  ;
        TIMER_BaseStruct.TIMER_Prescaler     = 0x00 ; 
        TIMER_BaseStruct.TIMER_ClockDivision = TIMER_CDIV_DIV1;
        TIMER_BaseStruct.TIMER_CounterMode   = TIMER_COUNTER_UP;
        TIMER_BaseInit( IR_TIM, &TIMER_BaseStruct );
          
        /* Prescaler configuration */
        TIMER_PrescalerConfig(IR_TIM, TIM_PRESCALER, TIMER_PSC_RELOAD_NOW);
      
        TIMER_ICStructInit(&TIMER_ICInitStruct);
          
        /* TIMER configuration */
        TIMER_ICInitStruct.TIMER_CH          = IR_TIM_Channel;
        TIMER_ICInitStruct.TIMER_ICPolarity  = TIMER_IC_POLARITY_FALLING;
        TIMER_ICInitStruct.TIMER_ICSelection = TIMER_IC_SELECTION_DIRECTTI;
        TIMER_ICInitStruct.TIMER_ICPrescaler = TIMER_IC_PSC_DIV1;
        TIMER_ICInitStruct.TIMER_ICFilter    = 0x0;

        TIMER_PWMCaptureConfig(IR_TIM, &TIMER_ICInitStruct); 
    }

    /* Timer Clock */
    TIMCLKValueKHz = TIMER_GetCounterCLKValue()/1000;
    
    /* Select the TIMER Input Trigger */
#if defined(GD32F130_150)
    TIMER_SelectInputTrigger( IR_TIM , TIMER_TS_TI1FP1 );
#elif defined(GD32F170_190)
    TIMER_SelectInputTrigger( IR_TIM , TIMER_TS_TI2FP2 );
#endif

    /* Select the slave Mode: Reset Mode */
    TIMER_SelectSlaveMode(IR_TIM, TIMER_SLAVE_MODE_RESET);
    
    /* Configures the TIMER Update Request Interrupt source: counter overflow */
    TIMER_UpdateRequestConfig( IR_TIM,  TIMER_UPDATE_SRC_REGULAR );
    
    RC5TimeOut = TIMCLKValueKHz * RC5_TIME_OUT_US/1000;
    
    /* Set the TIMER auto-reload register for each IR protocol */
    IR_TIM->CARL = RC5TimeOut;
    
    /* Clear flag */
    TIMER_ClearBitState(IR_TIM, TIMER_FLAG_UPDATE | TIMER_FLAG_CH1 | TIMER_FLAG_CH2 );

    /* Enable the CC2/CC1 Interrupt Request */
    TIMER_INTConfig(IR_TIM, TIMER_INT_CH2, ENABLE);

    /* Enable the CC2/CC1 Interrupt Request */
    TIMER_INTConfig(IR_TIM, TIMER_INT_CH1, ENABLE);
    
    /* Enable the timer */
    TIMER_Enable(IR_TIM, ENABLE);  

    /* Bit time range */
    RC5MinT = ( RC5_T_US - RC5_T_TOLERANCE_US ) * TIMCLKValueKHz / 1000 ;
    RC5MaxT = ( RC5_T_US + RC5_T_TOLERANCE_US ) * TIMCLKValueKHz / 1000 ;
    RC5Min2T = ( 2 * RC5_T_US - RC5_T_TOLERANCE_US ) * TIMCLKValueKHz / 1000 ;
    RC5Max2T = ( 2 * RC5_T_US + RC5_T_TOLERANCE_US ) * TIMCLKValueKHz / 1000 ;
    
    /* Default state */
    RC5_ResetPacket();
}

/**
  * @brief  Decode the IR frame (ADDRESS, COMMAND) when all the frame is 
  *         received, the IRFrameReceived will equal to YES.
  *         The IRFrameReceived is set to YES inside the  IR_DataSampling() 
  *         function when the whole IR frame is received without any error.
  *         If any received bit timing is out of the defined range, the IR packet
  *         is reset and the IRFrameReceived is set to NO.
  *         After the IR received display, the IRFrameReceived is set NO.
  *         User can check the IRFrameReceived variable status to verify if there
  *         is a new IR frame already received.          
  * @param  rc5_frame: pointer to IR_Frame_TypeDef structure that contains the 
  *         the IR protocol fields (Address, Command,...).
  * @retval None
  */
void RC5_Decode(RC5_Frame_TypeDef *rc5_frame)
{ 
    /* If frame received */
    if(RC5FrameReceived != NO)  
    {
        RC5_Data = RC5TmpPacket.data ;

        /* RC5 frame field decoding */
        rc5_frame->Address   = (RC5TmpPacket.data >> 6) & 0x1F;
        rc5_frame->Command   = (RC5TmpPacket.data) & 0x3F; 
        rc5_frame->FieldBit  = (RC5TmpPacket.data >> 12) & 0x1;
        rc5_frame->ToggleBit = (RC5TmpPacket.data >> 11) & 0x1;
    
        /* Check if command ranges between 64 to 127:Upper Field */
        if (rc5_frame->FieldBit == 0x00)
        {
            rc5_frame->Command =  (1<<6)| rc5_frame->Command; 
        }
    
        /* Default state */
        RC5FrameReceived = NO;
        RC5_ResetPacket();
    }  
}

/**
  * @brief  Set the incoming packet structure to default state.
  * @param  None
  * @retval None
  */
void RC5_ResetPacket(void)
{
    RC5TmpPacket.data     = 0;
    RC5TmpPacket.bitCount = RC5_PACKET_BIT_COUNT - 1;
    RC5TmpPacket.lastBit  = RC5_ONE;
    RC5TmpPacket.status   = RC5_PACKET_STATUS_EMPTY;
}

/**
  * @brief  Identify the RC5 data bits.
  * @param  rawPulseLength: low/high pulse duration
  * @param  edge: '1' for Rising  or '0' for falling edge
  * @retval None
  */
void RC5_DataSampling(uint16_t rawPulseLength, uint8_t edge)
{ 
    uint8_t pulse;
    tRC5_lastBitType tmpLastBit;
  
    /* Decode the pulse length in protocol units */
    pulse = RC5_GetPulseLength(rawPulseLength);

    /* On Rising Edge */
    if ( edge == 1 )
    { 
        if (pulse <= RC5_2T_TIME) 
        { 
            /* Bit determination by the rising edge */
            tmpLastBit = RC5_logicTableRisingEdge[RC5TmpPacket.lastBit][pulse];
            RC5_modifyLastBit (tmpLastBit);
        }
        else
        {
            RC5_ResetPacket();
        }
    }

    /* On Falling Edge */
    else    
    {
        /* If this is the first falling edge - don't compute anything */
        if (RC5TmpPacket.status & RC5_PACKET_STATUS_EMPTY)
        { 
            RC5TmpPacket.status &= (uint8_t)~RC5_PACKET_STATUS_EMPTY;
        }
        else    
        {
            if (pulse <= RC5_2T_TIME) 
            { 
                /* Bit determination by the falling edge */
                tmpLastBit = RC5_logicTableFallingEdge[RC5TmpPacket.lastBit][pulse];
                RC5_modifyLastBit (tmpLastBit);
            }
            else
            {
                RC5_ResetPacket();
            }
        }
    }
}  

/**
  * @brief  Convert raw pulse length expressed in timer ticks to protocol bit times.
  * @param  pulseLength:pulse duration
  * @retval bit time value
  */
static uint8_t RC5_GetPulseLength (uint16_t pulseLength)
{ 
    /* Valid bit time */
    if ((pulseLength > RC5MinT) && (pulseLength < RC5MaxT))
    { 
        /* We've found the length */
        /* Return the correct value */
        return (RC5_1T_TIME);    
    }
    else if ((pulseLength > RC5Min2T) && (pulseLength < RC5Max2T))
    {
        /* We've found the length */
        /* Return the correct value */
        return (RC5_2T_TIME);
    }
    /* Error */
    return RC5_WRONG_TIME;
}

/**
  * @brief  perform checks if the last bit was not incorrect. 
  * @param  bit: where bit can be  RC5_NAN or RC5_INV or RC5_ZER or RC5_ONE
  * @retval None
  */
static void RC5_modifyLastBit(tRC5_lastBitType bit)
{
    if (bit != RC5_NAN)
    {
        if (RC5TmpPacket.lastBit != RC5_INV)
        { 
            /* Restore the last bit */
            RC5TmpPacket.lastBit = bit;

            /* Insert one bit into the RC5 Packet */
            RC5_WriteBit(RC5TmpPacket.lastBit);
        }
        else 
        {
            RC5_ResetPacket();
        }
    }
}

/**
  * @brief  Insert one bit into the final data word. 
  * @param  bitVal: bit value 'RC5_ONE' or 'RC5_ZER'
  * @retval None
  */
static void RC5_WriteBit(uint8_t bitVal)
{
    /* First convert RC5 symbols to ones and zeros */
    if (bitVal == RC5_ONE)
    { 
        bitVal = 1;
    }
    else if (bitVal == RC5_ZER)
    {
        bitVal = 0;
    }
    else
    {
        RC5_ResetPacket();
        return;
    } 

    /* Write this particular bit to data field */
    RC5TmpPacket.data |=  bitVal; 
  
    /* Test the bit number determined */
    /* If this is not the last bit */
    if (RC5TmpPacket.bitCount != 0)  
    {
        /* Shift the data field */
        RC5TmpPacket.data = RC5TmpPacket.data << 1;

        /* And decrement the bitCount */
        RC5TmpPacket.bitCount--;
    } 
    else
    {
        RC5FrameReceived = YES;
    }
}

/**
  * @brief  Identify TIMER clock
  * @param  None
  * @retval Timer clock
  */
static uint32_t TIMER_GetCounterCLKValue(void)
{
    uint32_t apbprescaler = 0, apbfrequency = 0;
    uint32_t timprescaler = 0;
    __IO RCC_ClocksPara RCC_ClockFreq;   
  
    /* This function fills the RCC_ClockFreq structure with the current
       frequencies of different on chip clocks */
    RCC_GetClocksFreq((RCC_ClocksPara*)&RCC_ClockFreq);
  
    /* Get the clock prescaler of APB1 */
    apbprescaler = ((RCC->GCFGR >> 8) & 0x7);
    apbfrequency = RCC_ClockFreq.APB1_Frequency; 
    timprescaler = TIM_PRESCALER;
  
    /* If APBx clock div >= 4 */
    if (apbprescaler >= 4)
    {
        return ((apbfrequency * 2)/(timprescaler + 1));
    }
    else
    {
        return (apbfrequency/(timprescaler+ 1));
    }
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
