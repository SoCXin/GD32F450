/**
  ******************************************************************************
  * @file    FMC/Write_Protection/main.c 
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
#define BufferSize                   2048

#define FMC_PAGE_SIZE                ((uint16_t)0x400) 
#define FMC_PAGES_PROTECTED          (WRP_SECTOR4)

#define BANK1_WRITE_START_ADDR       ((uint32_t)0x08004000)
#define BANK1_WRITE_END_ADDR         ((uint32_t)0x08004800)


/* Private function prototypes -----------------------------------------------*/
void LED_config(void);

/* Private variables ---------------------------------------------------------*/
uint32_t m2m = 0,m2u =0;
__IO TestStatus TransferStatus = FAILED;

uint32_t *ptrd;
uint32_t WriteProtected_Value = 0xFFFFFFFF, ProtectedPages = 0x0;
uint32_t EraseCounter = 0x00, Address = 0x00;
uint32_t Data = 0x1234ABCD;
__IO uint32_t NbrOfPage = 0x00;
volatile FMC_State FMCStatus = FMC_READY;
volatile TestStatus MemoryProgramStatus = PASSED;

/* Uncomment this line to program the Falsh pages */
#define FMC_PAGE_PROGRAM

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    uint32_t i;
    TransferStatus = PASSED;
    
    GD_EVAL_LEDInit(LED1);
    GD_EVAL_LEDInit(LED2);
    GD_EVAL_LEDInit(LED3);
    GD_EVAL_LEDInit(LED4);

    /* Unlock the Flash Bank1 Program Erase controller */
    FMC_Unlock();
    
    FMC_OB_Unlock();
    
    /* Clear All pending flags */
    FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );      
    
    /* Get status of pages write protection */
    WriteProtected_Value = FMC_OB_GetWRP();
   
#ifdef WRITE_PROTECTION_DISABLE 
  
    /* Get write protected pages */
    ProtectedPages = ~(WriteProtected_Value | FMC_PAGES_PROTECTED);
    
    /* Check whether the specified pages is write protected  */
    if((WriteProtected_Value | (~FMC_PAGES_PROTECTED)) != 0xFFFFFFFF)
    {
        /* Erase all the option Bytes */
        FMCStatus = FMC_OB_Erase();
    
        /* Check if other pages write protected */
        if(ProtectedPages != 0x0)
        {
            /* Restore other pages write protected */
            FMCStatus = FMC_OB_EnableWRP(ProtectedPages);
        }
        /* Generate a system reset to reload the option byte */      
        FMC_OB_Reset();
    }

#elif defined WRITE_PROTECTION_ENABLE
    /* Get already write protected pages and the new specified pages to be protected */
    ProtectedPages =  (~WriteProtected_Value) | FMC_PAGES_PROTECTED; 
    
    /* Check whether the specified pages is write protected */
    if(((~WriteProtected_Value) & FMC_PAGES_PROTECTED )!= FMC_PAGES_PROTECTED)
    {
        FMCStatus = FMC_OB_Erase();
        
        /* Enable the pages write protection */
        FMCStatus = FMC_OB_EnableWRP(ProtectedPages);
        
        /* Generate a system reset to reload the option byte */ 
        FMC_OB_Reset();
    }  
#endif 

#ifdef FMC_PAGE_PROGRAM  

    /* Define the number of page to be erased */
    NbrOfPage = (BANK1_WRITE_END_ADDR - BANK1_WRITE_START_ADDR) / FMC_PAGE_SIZE;
    
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
    while(1)
    {
    }
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
