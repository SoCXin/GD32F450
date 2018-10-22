/**
  ******************************************************************************
  * @file    FMC/Erase_Program/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h" 
#include "gd32f1x0_gpio.h"
#include "gd32f1x0_eval.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BufferSize              2048
#define FMC_PAGE_SIZE           ((uint16_t)0x400)
#define BANK1_WRITE_START_ADDR  ((uint32_t)0x08004000)
#define BANK1_WRITE_END_ADDR    ((uint32_t)0x08004800)


/* Private variables ---------------------------------------------------------*/
uint32_t m2m = 0,m2u =0;
__IO TestStatus TransferStatus = FAILED;
uint32_t *ptrd;
uint32_t EraseCounter = 0x00, Address = 0x00 ;
uint32_t Data = 0x1234ABCD;
__IO uint32_t NbrOfPage = 0x00;
volatile FMC_State FMCStatus = FMC_READY;
volatile TestStatus MemoryProgramStatus = PASSED;
#define FMC_PAGE_PROGRAM
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

int main(void)
{
    uint32_t i;

    GD_EVAL_LEDInit(LED1);
    GD_EVAL_LEDInit(LED2);
    GD_EVAL_LEDInit(LED3);
    GD_EVAL_LEDInit(LED4);

#ifdef FMC_PAGE_PROGRAM

    TransferStatus = PASSED;
    /* Unlock the Flash Bank1 Program Erase controller */
    FMC_Unlock();
    
    /* Define the number of page to be erased */
    NbrOfPage = (BANK1_WRITE_END_ADDR - BANK1_WRITE_START_ADDR) / FMC_PAGE_SIZE;
    
    /* Clear All pending flags */
    FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );
    
    /* Erase the FLASH pages */
    for(EraseCounter = 0; EraseCounter < NbrOfPage; EraseCounter++)
    {
        FMCStatus = FMC_ErasePage(BANK1_WRITE_START_ADDR + (FMC_PAGE_SIZE * EraseCounter));
        FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );
    }
    
    FMC_Lock();
    
    ptrd = (uint32_t*)BANK1_WRITE_START_ADDR;

    for(i = 0; i < 512; i++)
    {
        if(*ptrd != 0xFFFFFFFF)
        { 
            TransferStatus = FAILED;
            break;
        }
        else
        {
            ptrd++;
        }
    }

    /* Unlock the Flash Bank1 Program Erase controller */
    FMC_Unlock();

    /* Program Flash Bank1 */
    Address = BANK1_WRITE_START_ADDR;

    while(Address < BANK1_WRITE_END_ADDR)
    {
        FMCStatus = FMC_ProgramWord(Address, Data);
        Address = Address + 4; 
        FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );    
    }
    
    FMC_Lock();
    
    ptrd = (uint32_t*)BANK1_WRITE_START_ADDR;

    for(i = 0; i < 512; i++)
    {
        if(*ptrd == 0xFFFFFFFF)
        { 
            TransferStatus = FAILED;
            break;
        }
        else
        {
            ptrd++;
        }
    }
    
    if(TransferStatus == FAILED)
    {
        GD_EVAL_LEDOn(LED1);
        GD_EVAL_LEDOff(LED2);
        GD_EVAL_LEDOn(LED3);
        GD_EVAL_LEDOff(LED4);
    }
    else
    {
        GD_EVAL_LEDOff(LED1);
        GD_EVAL_LEDOn(LED2);
        GD_EVAL_LEDOff(LED3);
        GD_EVAL_LEDOn(LED4);
    }
#endif

#ifdef FMC_RERPGRRAM
    FMC_Unlock();
    FMC_ErasePage(0x8004000);
    FMC_ProgramWord(0x8004000,0x12345678);
    /* change bit 1 to bit 0 */
    FMC_ReProgramWord(0x8004000, 0x00111100);
    if(*(__IO uint32_t*)0x8004000==0x12345678)
    {
        GD_EVAL_LEDOff(LED1);
        GD_EVAL_LEDOn(LED2);
        GD_EVAL_LEDOff(LED3);
        GD_EVAL_LEDOn(LED4);
    }
    else
    {
        GD_EVAL_LEDOn(LED1);
        GD_EVAL_LEDOff(LED2);
        GD_EVAL_LEDOn(LED3);
        GD_EVAL_LEDOff(LED4);
    }
#endif

    while(1)
    {
    }
}
  

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
