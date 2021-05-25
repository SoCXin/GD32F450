/*!
    \file  gd32f4xx_it.c
    \brief this file includes the interrupt handlers for the application

    \version 2016-08-15, V1.0.0, firmware for GD32F4xx
    \version 2018-12-12, V2.0.0, firmware for GD32F4xx
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "usbh_int.h"
#include "usbh_core.h"
#include "gd32f4xx_it.h"

extern usb_core_handle_struct          usb_core_dev;
extern usbh_host_struct                usb_host;
extern uint32_t                        hwp_delay_num;

/*!
    \brief      this function handles NMI exception.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles external line 1 interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI1_IRQHandler(void)
{
//    if(exti_interrupt_flag_get(EXTI_1) != RESET)
//    {
//        usb_host.usr_cb->over_current_detected();
//        exti_interrupt_flag_clear(EXTI_1);
//    }
}

/*!
    \brief      this function handles hard fault exception.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/*!
    \brief      this function handles memory manage exception.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/*!
    \brief      this function handles bus fault exception.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/*!
    \brief      this function handles usage fault exception.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/*!
    \brief      this function handles SVCall exception.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles Debug Monitor exception.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}


/*!
    \brief      this function handles PendSVC exception.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles Timer2 Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER2_IRQHandler(void)
{
    if (timer_interrupt_flag_get(TIMER2, TIMER_INT_UP) != RESET){
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_UP);

        if (hwp_delay_num > 0x00){
            hwp_delay_num--;
        }else{
            timer_disable(TIMER2);
        }
    }
}
#ifdef USE_USBFS
/*!
    \brief      this function handles USBFS global interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_IRQHandler(void)
{
    usbh_isr(&usb_core_dev);
}
#elif defined(USE_USBHS)
/*!
    \brief      this function handles USBHS global interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBHS_IRQHandler(void)
{
    usbh_isr(&usb_core_dev);
}
#endif /* USE_USBFS */
/*!
    \brief      this function handles SysTick Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
}

