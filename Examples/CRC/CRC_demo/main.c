/**
  ******************************************************************************
  * @file    CRC/CRC_demo/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "gd32f1x0_eval.h"

/* Private variables ---------------------------------------------------------*/
uint32_t vab1 = 0, success_flag = 0;
uint32_t read32_1, read32_2, read32_3, read32_4, read32_5, read32_6, read32_7, read32_8;

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    GD_EVAL_LEDInit(LED1);

    vab1 = (uint32_t)0xabcd1234;
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_CRC, ENABLE);
    
    CRC_DeInit();
    read32_1 = CRC_CalcSingleData(vab1);
    
    /* input reverse */
    CRC_DeInit();
    CRC_ReverseInputData_Config(CRC_REVERSE_INPUT_DATA_BYTE);
    read32_2 = CRC_CalcSingleData(vab1);
    
    CRC_DeInit();
    CRC_ReverseInputData_Config(CRC_REVERSE_INPUT_DATA_HALFWORD);    
    read32_3 = CRC_CalcSingleData(vab1);

    CRC_DeInit();
    CRC_ReverseInputData_Config(CRC_REVERSE_INPUT_DATA_WORD);        
    read32_4 = CRC_CalcSingleData(vab1);
    
    /* output reverse */
    CRC_DeInit();
    CRC_ReverseOutputData_Enable(ENABLE);
    read32_5 = CRC_CalcSingleData(vab1);
    
    CRC_DeInit();
    CRC_ReverseInputData_Config(CRC_REVERSE_INPUT_DATA_BYTE);
    CRC_ReverseOutputData_Enable(ENABLE);
    read32_6 = CRC_CalcSingleData(vab1);
    
    CRC_DeInit();
    CRC_ReverseInputData_Config(CRC_REVERSE_INPUT_DATA_HALFWORD);
    CRC_ReverseOutputData_Enable(ENABLE);
    read32_7 = CRC_CalcSingleData(vab1);
    
    CRC_DeInit();
    CRC_ReverseInputData_Config(CRC_REVERSE_INPUT_DATA_WORD);
    CRC_ReverseOutputData_Enable(ENABLE);
    read32_8 = CRC_CalcSingleData(vab1);
    
    /* check the caculation result */
    if((read32_1 == 0xf7018a40)&&(read32_2 == 0x49fc6721)&&(read32_3 == 0x606444e3)&&(read32_4 == 0x16d70081)
      &&(read32_5 == 0x025180ef)&&(read32_6 == 0x84e63f92)&&(read32_7 == 0xc7222606)&&(read32_8 == 0x8100eb68))
    {
        success_flag = 0x1;
        GD_EVAL_LEDOn(LED1);
    }

    while (1)
    {
    }
}

/************************ (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
