/**
  ******************************************************************************
  * @file    DMA/FLASH_RAM/main.c 
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h" 
#include "gd32f1x0_gpio.h"
#include "gd32f1x0_eval.h"
/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BufferSize                       1024
#define FMC_PAGE_SIZE                    ((uint16_t)0x400)

#define BANK1_WRITE_START_ADDR           ((uint32_t)0x08004000)
#define BANK1_WRITE_END_ADDR             ((uint32_t)0x08004800)


/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
TestStatus Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);
void LED_config(void);

/* Private variables ---------------------------------------------------------*/
DMA_InitPara  DMA_InitStructure;
__IO TestStatus TransferStatus = FAILED;
uint32_t DST_Buffer[256];
uint32_t *ptrd;
__IO uint32_t count = 0;

uint32_t EraseCounter = 0x00, Address = 0x00 ,wrp = 0,wrp2 = 0;
uint32_t Data = 0x3210ABCD;
__IO uint32_t NbrOfPage = 0x00;
volatile FMC_State FMCStatus = FMC_READY;
volatile TestStatus MemoryProgramStatus = PASSED;

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
    
    /* System Clocks Configuration */
    RCC_Configuration();
         
    /* NVIC configuration */
    NVIC_Configuration();
    
    LED_config() ;
    GD_EVAL_LEDOff(LED1);
    GD_EVAL_LEDOff(LED3);
    GD_EVAL_LEDOff(LED2);
    GD_EVAL_LEDOff(LED4);

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
        wrp += (FMCStatus == FMC_WRPERR);
        FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );
    }

    FMC_Lock();
    
    ptrd = (uint32_t*)BANK1_WRITE_START_ADDR;
    for(i = 0; i < 256; i++)
    {
        if(*ptrd != 0xFFFFFFFF)
        { 
            TransferStatus = FAILED;
            break;
        }
        ptrd++;
    }

    /* Unlock the Flash Bank1 Program Erase controller */
    FMC_Unlock();
    
    /* Define the number of page to be erased */
    NbrOfPage = (BANK1_WRITE_END_ADDR - BANK1_WRITE_START_ADDR) / FMC_PAGE_SIZE;
    
    /* Clear All pending flags */
    FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );    
    
    /* Program Flash Bank1 */
    Address = BANK1_WRITE_START_ADDR;
    wrp2 = 0;
    while(Address < BANK1_WRITE_END_ADDR)
    {
        FMCStatus = FMC_ProgramWord(Address, Data);
        Address = Address + 4; 
        wrp2 += (FMCStatus == FMC_WRPERR);
        FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );    
    }
    
    FMC_Lock();

    /* DMA1 channel1 configuration */
    DMA_DeInit(DMA1_CHANNEL1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)BANK1_WRITE_START_ADDR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DST_Buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PERIPHERALSRC;
    DMA_InitStructure.DMA_BufferSize = BufferSize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_ENABLE;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PERIPHERALDATASIZE_BYTE;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MEMORYDATASIZE_BYTE;
    DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_VERYHIGH;
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_ENABLE;
    DMA_Init(DMA1_CHANNEL1, &DMA_InitStructure);
    
    DMA_INTConfig(DMA1_CHANNEL1, DMA_INT_TC, ENABLE);
    
    /* Enable DMA transfer */
    DMA_Enable(DMA1_CHANNEL1, ENABLE);

    for(i = 0; i < 10000; i++)
    {
        if(count)
        break;
    }    
    
    ptrd = DST_Buffer;
    for(i = 0; i < 256; i++)
    {
        if(*ptrd != Data)
        { 
            TransferStatus = FAILED;
            break;
        }
        ptrd++;
    }
    
    if(TransferStatus != FAILED && count == 1)
    {
        GD_EVAL_LEDOff(LED1);
        GD_EVAL_LEDOff(LED3);
        GD_EVAL_LEDOn(LED2);
        GD_EVAL_LEDOn(LED4);
    }
    else
    {

        GD_EVAL_LEDOff(LED2);
        GD_EVAL_LEDOff(LED4);
        GD_EVAL_LEDOn(LED1);
        GD_EVAL_LEDOn(LED3);
    }
   
    while(1)
    {
    }
}
  
void LED_config(void)
{
    GD_EVAL_LEDInit (LED1);
    GD_EVAL_LEDInit (LED2);
    GD_EVAL_LEDInit (LED3);
    GD_EVAL_LEDInit (LED4);
}

/**
  * @brief  Configure the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
    /* Enable peripheral clocks */
    /* Enable DMA1 clock */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_DMA1, ENABLE);
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
    NVIC_InitPara NVIC_InitStructure;

    /* Enable DMA1 IRQ Channel */
    NVIC_InitStructure.NVIC_IRQ = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);  
    NVIC_InitStructure.NVIC_IRQ = DMA1_Channel2_3_IRQn;
    NVIC_Init(&NVIC_InitStructure);  
    NVIC_InitStructure.NVIC_IRQ = DMA1_Channel4_5_IRQn;
    NVIC_Init(&NVIC_InitStructure);  
    NVIC_InitStructure.NVIC_IRQ = DMA1_Channel6_7_IRQn;
    NVIC_Init(&NVIC_InitStructure); 
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
