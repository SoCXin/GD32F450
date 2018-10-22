;/**
;  ******************************************************************************
;  * @file    startup_gd32f1x0.s
;  * @author  MCU SD
;  * @version V2.0.0   
;  * @date    15-Jan-2016
;  * @brief   GD32F1x0 startup code.
;  ******************************************************************************
;  */


        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  System_Init
        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)		                    ; Top of Stack
        DCD     Reset_Handler                       ; Vector Number 1,Reset Handler

        DCD     NMI_Handler                         ; Vector Number 2,NMI Handler
        DCD     HardFault_Handler                   ; Vector Number 3,Hard Fault Handler
        DCD     MemManage_Handler                   ; Vector Number 4,MPU Fault Handler
        DCD     BusFault_Handler                    ; Vector Number 5,Bus Fault Handler
        DCD     UsageFault_Handler                  ; Vector Number 6,Usage Fault Handler
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     SVC_Handler                         ; Vector Number 11,SVCall Handler
        DCD     DebugMon_Handler                    ; Vector Number 12,Debug Monitor Handler
        DCD     0                                   ; Reserved
        DCD     PendSV_Handler                      ; Vector Number 14,PendSV Handler
        DCD     SysTick_Handler                     ; Vector Number 15,SysTick Handler

        ; External Interrupts
        DCD     WWDG_IRQHandler                     ; Vector Number 16,Window Watchdog
        DCD     LVD_IRQHandler                      ; Vector Number 17,LVD through EXTI Line detect
        DCD     RTC_IRQHandler                      ; Vector Number 18,RTC through EXTI Line
        DCD     FMC_IRQHandler                      ; Vector Number 19,FMC
        DCD     RCC_IRQHandler                      ; Vector Number 20,RCC
        DCD     EXTI0_1_IRQHandler                  ; Vector Number 21,EXTI Line 0 and EXTI Line 1
        DCD     EXTI2_3_IRQHandler                  ; Vector Number 22,EXTI Line 2 and EXTI Line 3
        DCD     EXTI4_15_IRQHandler                 ; Vector Number 23,EXTI Line 4 to EXTI Line 15
        DCD     TSI_IRQHandler                      ; Vector Number 24,TSI
        DCD     DMA1_Channel1_IRQHandler            ; Vector Number 25,DMA1 Channel 1
        DCD     DMA1_Channel2_3_IRQHandler          ; Vector Number 26,DMA1 Channel 2 and DMA1 Channel 3
        DCD     DMA1_Channel4_5_IRQHandler          ; Vector Number 27,DMA1 Channel 4 and DMA1 Channel 5
        DCD     ADC1_CMP_IRQHandler                 ; Vector Number 28,ADC1 and Comparator 1-2 
        DCD     TIMER1_BRK_UP_TRG_COM_IRQHandler    ; Vector Number 29,TIMER1 Break, Update, Trigger and Commutation
        DCD     TIMER1_CC_IRQHandler                ; Vector Number 30,TIMER1 Capture Compare
        DCD     TIMER2_IRQHandler                   ; Vector Number 31,TIMER2
        DCD     TIMER3_IRQHandler                   ; Vector Number 32,TIMER3
        DCD     TIMER6_DAC_IRQHandler               ; Vector Number 33,TIMER6 and DAC
        DCD     0                                   ; Reserved
        DCD     TIMER14_IRQHandler                  ; Vector Number 35,TIMER14
        DCD     TIMER15_IRQHandler                  ; Vector Number 36,TIMER15
        DCD     TIMER16_IRQHandler                  ; Vector Number 37,TIMER16
        DCD     TIMER17_IRQHandler                  ; Vector Number 38,TIMER17
        DCD     I2C1_EV_IRQHandler                  ; Vector Number 39,I2C1 Event 
        DCD     I2C2_EV_IRQHandler                  ; Vector Number 40,I2C2 Event
        DCD     SPI1_IRQHandler                     ; Vector Number 41,SPI1
        DCD     SPI2_IRQHandler                     ; Vector Number 42,SPI2
        DCD     USART1_IRQHandler                   ; Vector Number 43,USART1
        DCD     USART2_IRQHandler                   ; Vector Number 44,USART2
        DCD     0                                   ; Reserved
        DCD     CEC_IRQHandler                      ; Vector Number 46,CEC
        DCD     0                                   ; Reserved        
        DCD     I2C1_ER_IRQHandler                  ; Vector Number 48,I2C1 Error  
        DCD     0                                   ; Reserved    
        DCD     I2C2_ER_IRQHandler                  ; Vector Number 50,I2C2 Error  
        DCD     I2C3_EV_IRQHandler                  ; Vector Number 51,I2C3 Event  
        DCD     I2C3_ER_IRQHandler                  ; Vector Number 52,I2C3 Error  
        DCD     USB_FS_LP_IRQHandler                ; Vector Number 53,USB FS LP
        DCD     USB_FS_HP_IRQHandler                ; Vector Number 54,USB FS HP
        DCD     0                                   ; Reserved 
        DCD     0                                   ; Reserved 
        DCD     0                                   ; Reserved 
        DCD     USBWakeUp_IRQHandler                ; Vector Number 58,USB Wakeup
        DCD     CAN1_TX_IRQHandler                  ; Vector Number 59,CAN1 TX
        DCD     CAN1_RX0_IRQHandler                 ; Vector Number 60,CAN1 RX0
        DCD     CAN1_RX1_IRQHandler                 ; Vector Number 61,CAN1 RX1
        DCD     CAN1_SCE_IRQHandler                 ; Vector Number 62,CAN1 SCE     
        DCD     LCD_IRQHandler                      ; Vector Number 63,LCD
        DCD     DMA1_Channel6_7_IRQHandler          ; Vector Number 64,DMA1 Channel6 and Channel7 
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     SPI3_IRQHandler                     ; Vector Number 67,SPI3     
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     CAN2_TX_IRQHandler                  ; Vector Number 86,CAN2 TX
        DCD     CAN2_RX0_IRQHandler                 ; Vector Number 87,CAN2 RX0
        DCD     CAN2_RX1_IRQHandler                 ; Vector Number 88,CAN2 RX1
        DCD     CAN2_SCE_IRQHandler                 ; Vector Number 89,CAN2 SCE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:NOROOT:REORDER(2)
Reset_Handler
        LDR     R0, =System_Init
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0
        
        PUBWEAK NMI_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
NMI_Handler
        B NMI_Handler
       
        PUBWEAK HardFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
HardFault_Handler
        B HardFault_Handler
       
        PUBWEAK MemManage_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
UsageFault_Handler
        B UsageFault_Handler
        
        PUBWEAK SVC_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SVC_Handler
        B SVC_Handler
       
        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
DebugMon_Handler
        B DebugMon_Handler
        
        PUBWEAK PendSV_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
PendSV_Handler
        B PendSV_Handler
        
        PUBWEAK SysTick_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SysTick_Handler
        B SysTick_Handler
        
        PUBWEAK WWDG_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
WWDG_IRQHandler
        B WWDG_IRQHandler
        
        PUBWEAK LVD_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
LVD_IRQHandler
        B LVD_IRQHandler
        
        PUBWEAK RTC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RTC_IRQHandler
        B RTC_IRQHandler
        
        PUBWEAK FMC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FMC_IRQHandler
        B FMC_IRQHandler
        
        PUBWEAK RCC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RCC_IRQHandler
        B RCC_IRQHandler
        
        PUBWEAK EXTI0_1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI0_1_IRQHandler
        B EXTI0_1_IRQHandler
        
        PUBWEAK EXTI2_3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI2_3_IRQHandler
        B EXTI2_3_IRQHandler
        
        PUBWEAK EXTI4_15_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI4_15_IRQHandler
        B EXTI4_15_IRQHandler
        
        PUBWEAK TSI_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TSI_IRQHandler
        B TSI_IRQHandler
        
        PUBWEAK DMA1_Channel1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DMA1_Channel1_IRQHandler
        B DMA1_Channel1_IRQHandler
        
        PUBWEAK DMA1_Channel2_3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DMA1_Channel2_3_IRQHandler
        B DMA1_Channel2_3_IRQHandler
        
        PUBWEAK DMA1_Channel4_5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DMA1_Channel4_5_IRQHandler
        B DMA1_Channel4_5_IRQHandler
        
        PUBWEAK ADC1_CMP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
ADC1_CMP_IRQHandler
        B ADC1_CMP_IRQHandler
        
        PUBWEAK TIMER1_BRK_UP_TRG_COM_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER1_BRK_UP_TRG_COM_IRQHandler
        B TIMER1_BRK_UP_TRG_COM_IRQHandler
        
        PUBWEAK TIMER1_CC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER1_CC_IRQHandler
        B TIMER1_CC_IRQHandler
        
        PUBWEAK TIMER2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER2_IRQHandler
        B TIMER2_IRQHandler
        
        PUBWEAK TIMER3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER3_IRQHandler
        B TIMER3_IRQHandler
        
        PUBWEAK TIMER6_DAC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER6_DAC_IRQHandler
        B TIMER6_DAC_IRQHandler
        
        PUBWEAK TIMER14_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER14_IRQHandler
        B TIMER14_IRQHandler
        
        PUBWEAK TIMER15_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER15_IRQHandler
        B TIMER15_IRQHandler
        
        PUBWEAK TIMER16_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER16_IRQHandler
        B TIMER16_IRQHandler
        
        PUBWEAK TIMER17_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER17_IRQHandler
        B TIMER17_IRQHandler
        
        PUBWEAK I2C1_EV_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C1_EV_IRQHandler
        B I2C1_EV_IRQHandler
        
        PUBWEAK I2C2_EV_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C2_EV_IRQHandler
        B I2C2_EV_IRQHandler
                
        PUBWEAK SPI1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPI1_IRQHandler
        B SPI1_IRQHandler
        
        PUBWEAK SPI2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPI2_IRQHandler
        B SPI2_IRQHandler
        
        PUBWEAK USART1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART1_IRQHandler
        B USART1_IRQHandler
        
        PUBWEAK USART2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART2_IRQHandler
        B USART2_IRQHandler
        
        PUBWEAK CEC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
CEC_IRQHandler
        B CEC_IRQHandler
        
        PUBWEAK I2C1_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C1_ER_IRQHandler
        B I2C1_ER_IRQHandler
        
        PUBWEAK I2C2_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C2_ER_IRQHandler
        B I2C2_ER_IRQHandler
        
        PUBWEAK I2C3_EV_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C3_EV_IRQHandler
        B I2C3_EV_IRQHandler

        PUBWEAK I2C3_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C3_ER_IRQHandler
        B I2C3_ER_IRQHandler

        PUBWEAK USB_FS_LP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USB_FS_LP_IRQHandler
        B USB_FS_LP_IRQHandler

        PUBWEAK USB_FS_HP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USB_FS_HP_IRQHandler
        B USB_FS_HP_IRQHandler 

        PUBWEAK USBWakeUp_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USBWakeUp_IRQHandler
        B USBWakeUp_IRQHandler             

        PUBWEAK CAN1_TX_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
CAN1_TX_IRQHandler
        B CAN1_TX_IRQHandler

        PUBWEAK CAN1_RX0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
CAN1_RX0_IRQHandler
        B CAN1_RX0_IRQHandler

        PUBWEAK CAN1_RX1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
CAN1_RX1_IRQHandler
        B CAN1_RX1_IRQHandler

        PUBWEAK CAN1_SCE_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
CAN1_SCE_IRQHandler
        B CAN1_SCE_IRQHandler

        PUBWEAK LCD_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
LCD_IRQHandler
        B LCD_IRQHandler

        PUBWEAK DMA1_Channel6_7_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DMA1_Channel6_7_IRQHandler
        B DMA1_Channel6_7_IRQHandler

        PUBWEAK SPI3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPI3_IRQHandler
        B SPI3_IRQHandler

        PUBWEAK CAN2_TX_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
CAN2_TX_IRQHandler
        B CAN2_TX_IRQHandler

        PUBWEAK CAN2_RX0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
CAN2_RX0_IRQHandler
        B CAN2_RX0_IRQHandler

        PUBWEAK CAN2_RX1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)	
CAN2_RX1_IRQHandler
        B CAN2_RX1_IRQHandler
	
        PUBWEAK CAN2_SCE_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
CAN2_SCE_IRQHandler
        B CAN2_SCE_IRQHandler	
        END
        
;/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
