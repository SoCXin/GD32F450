/**
  ******************************************************************************
  * @file    TIMER/TIMER1_6-steps/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the TIMER demo.
  ******************************************************************************
  */

  @brief
    This demo is based on the This demo is based on the GD32150R-EVAL/ GD32190R-EVAL
  board, it shows how to configure the TIMER1 peripheral to generate three
  complementary TIMER1 signals (for BLDC ) with dead time.

    TIMER1CLK is fixed to SystemCoreClock, the TIMER1 Prescaler is equal to 72 so the 
  TIMER1 counter clock used is 1MHz.  
  Channel change sequence: 
  AB`->AC`->BC`->BA`->CA`->CB`
  step1:1-0  0-1  0-0   (CH1-CH1N  CH2-CH2N  CH3-CH3N)
  step2:1-0  0-0  0-1   (CH1-CH1N  CH2-CH2N  CH3-CH3N)
  step3:0-0  1-0  0-1   (CH1-CH1N  CH2-CH2N  CH3-CH3N)
  step4:0-1  1-0  0-0   (CH1-CH1N  CH2-CH2N  CH3-CH3N)
  step5:0-1  0-0  1-0   (CH1-CH1N  CH2-CH2N  CH3-CH3N)
  step6:0-0  0-1  1-0   (CH1-CH1N  CH2-CH2N  CH3-CH3N)

  @note
    Connect the TIMER1 pins to an oscilloscope to monitor the different waveforms:
    - TIMER1_CH1  pin (PA8)
    - TIMER1_CH1N pin (PB13)
    - TIMER1_CH2  pin (PA9)
    - TIMER1_CH1N pin (PB14)
    - TIMER1_CH3  pin (PA10)
    - TIMER1_CH3N pin (PB15)

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/
