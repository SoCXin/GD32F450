/**
  ******************************************************************************
  * @file    USART/USART_WakeUpFromStop/main.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Main program body.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include "gd32f1x0_it.h"
#include "main.h"
#include "gd32f1x0_eval.h"

/* Private variables ---------------------------------------------------------*/
uint8_t DataReceived = 0;
extern __IO uint8_t InterruptCounter;

/* Private function prototypes -----------------------------------------------*/
static void USART_Configuration(void);
static void WakeUp_StartBitMethod(void);
static void RestoreConfiguration(void);
void delay_s(uint32_t nTime);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{    
    /*!< At this stage the microcontroller clock setting is already configured, 
    this is done through SystemInit() function which is called from startup
    file (startup_gd32f1x0.s) before to branch to application main.
    */
    SysTick_Config((SystemCoreClock / 1000));
    
    GD_EVAL_LEDInit (LED2);
    
    /* USART configuration */
    USART_Configuration();
    
    /* Configure SystemClock*/ 
    delay_s(20);
    
    /* Wake up from USART DeepSleep mode by Start bit Method */
    WakeUp_StartBitMethod();
    
    /* Configure SystemClock*/
    RestoreConfiguration();
    
    /* Configure and enable the systick timer to generate an interrupt each 1 ms */
    SysTick_Config((SystemCoreClock / 1000));
    
    while (1)
    {
    }
}

/**
  * @brief  delay functions.
  * @param  None
  * @retval None
  */
void delay_s(uint32_t nTime)
{
    uint32_t TimingDelay = 7200000*nTime;
    while(TimingDelay != 0)
    TimingDelay--;
}

/**
  * @brief  Start Bit Method to Wake Up USART from DeepSleep mode Test.
  * @param  None
  * @retval None
  */
static void WakeUp_StartBitMethod(void)
{ 
    /* Configure the wake up Method = Start bit */
    USART_DEEPSLEEPModeWakeUpSourceConfig(USART1, USART_WAKEUPSOURCE_STARTBIT);
    
    /* Enable USART1 */ 
    USART_Enable(USART1, ENABLE);
    
    /* Before entering the USART in STOP mode the REACK flag must be checked to ensure the USART RX is ready */
    while(USART_GetBitState(USART1, USART_FLAG_REA) == RESET)
    {}
    
    /* Enable USART STOP mode by setting the UESM bit in the CTLR1 register.*/
    USART_DEEPSLEEPModeEnable(USART1, ENABLE);
    
    /* Enable the wake up from stop Interrupt */ 
    USART_INT_Set(USART1, USART_INT_WU, ENABLE);   
    
    /* Enable PWR APB clock */
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_PWR, ENABLE);
    
    /*Enter Deep-sleep mode*/
    PWR_DEEPSLEEPMode_Entry(PWR_LDO_LOWPOWER, PWR_DEEPSLEEPENTRY_WFI);
    
    /* Waiting Wake Up interrupt */
    while(InterruptCounter == 0x00)
    {}
    
    /* Disable USART peripheral in DEEPSLEEP mode */ 
    USART_DEEPSLEEPModeEnable(USART1, DISABLE);
    
    while(USART_GetBitState(USART1, USART_FLAG_RBNE) == RESET)
    {}
    DataReceived = USART_DataReceive(USART1);
    
    /* Clear the TEN bit (if a transmission is on going or a data is in the TDR, it will be sent before
    efectivelly disabling the transmission) */
    USART_TransferDirection_Enable(USART1, USART_RXORTX_TX, DISABLE);
    
    /* Check the Transfer Complete Flag */
    while (USART_GetBitState(USART1, USART_FLAG_TC) == RESET)
    {}
    
    /* USART Disable */
    USART_Enable(USART1, DISABLE);
}

/**
  * @brief Configure the USART Device
  * @param  None
  * @retval None
  */
static void USART_Configuration(void)
{ 
    USART_InitPara USART_InitStructure;

    /* Configure the HSI as USART clock */
    RCC_USARTCLKConfig(RCC_USART1CLK_HSI);
    
    /* USARTx configured as follow:
    - BaudRate = 115200 baud  
    - Word Length = 8 Bits
    - Stop Bit = 1 Stop Bit
    - Parity = No Parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled
    */
    USART_DeInit(USART1);
    USART_InitStructure.USART_BRR = 115200;
    USART_InitStructure.USART_WL = USART_WL_8B;
    USART_InitStructure.USART_STBits = USART_STBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_RESET;
    USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE;
    USART_InitStructure.USART_RxorTx = USART_RXORTX_RX | USART_RXORTX_TX;

    GD_EVAL_COMInit(&USART_InitStructure);
    /* USART Disable */
    USART_Enable(USART1, DISABLE);
    /* USART1 IRQ Channel configuration */
    {
        NVIC_InitPara NVIC_InitStructure;
        
        /* Enable the USARTx Interrupt */
        NVIC_InitStructure.NVIC_IRQ                = USART1_IRQn;
        NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
        NVIC_InitStructure.NVIC_IRQSubPriority     = 0;
        NVIC_InitStructure.NVIC_IRQEnable          = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }
}

/**
  * @brief  Restore peripheral config before entering DEEPSLEEP mode.
  * @param  None
  * @retval None
  */
static void RestoreConfiguration(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
    
    /* SYSCLK, HCLK, PCLK configuration */
    /* Enable HSE */
    RCC_HSEConfig( RCC_HSE_ON );
    
    /* Wait till HSE is ready and if Time out is reached exit */
    HSEStatus = RCC_WaitForHSEStartUp();
    
    if (HSEStatus == (uint32_t)0x01)
    {
    /* HCLK = SYSCLK */
    RCC_AHBConfig(RCC_SYSCLK_DIV1); 
    
    /* PCLK = HCLK */
    RCC_APB1Config(RCC_APB1AHB_DIV1);
        
    /*  PLL configuration:  = HSE *  6 = 48 MHz */
    RCC_HSEPREDVConfig(RCC_HSEPREDV1_DIV1);
    RCC_PLLConfig(RCC_PLLSOURCE_HSEPREDIV, RCC_GCFGR_PLLMF6);
    
    /* Enable PLL */
    RCC_PLL_Enable(ENABLE);
    
    /* PLL as system clock source */
    RCC_CK_SYSConfig(RCC_SYSCLKSOURCE_PLLCLK);
    } 
}


/**
  * @brief  Led Spark.
  * @param  None
  * @retval None
  */
void LED_Spark(void)
{
    static __IO uint32_t TimingDelayLocal = 0;
    
    if (TimingDelayLocal != 0x00)
    {
        if(TimingDelayLocal < 50)  
        {
            /* Light On */
            GD_EVAL_LEDOn(LED2);
        }
        else                      
        {
            /* Light Off  */
            GD_EVAL_LEDOff(LED2);
        }
    TimingDelayLocal--;           
    }
    else
    {
        TimingDelayLocal = 100;
    }
}

/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
