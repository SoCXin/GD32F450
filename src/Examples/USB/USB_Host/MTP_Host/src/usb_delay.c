/*!
    \file  usb_delay.c 
    \brief time delay driver

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

#include "usb_delay.h"
#include "usb_conf.h"

#define TIM_MSEC_DELAY             0x01U
#define TIM_USEC_DELAY             0x02U

__IO uint32_t hwp_delay_num = 0U;
__IO uint32_t timer_prescaler = 11U;

static void hwp_time_set           (uint8_t unit);
static void hwp_delay              (uint32_t nTime, uint8_t unit);

/*!
    \brief      initializes delay unit using timer2
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_time_init (void)
{
    /* set the vector table base address at 0x08000000 */
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x00U);

    /* configure the priority group to 2 bits */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

    /* enable the TIM2 gloabal interrupt */
    nvic_irq_enable((uint8_t)TIMER2_IRQn, 1U, 0U);

    rcu_periph_clock_enable(RCU_TIMER2);
}

/*!
    \brief      delay in micro seconds
    \param[in]  usec: value of delay required in micro seconds
    \param[out] none
    \retval     none
*/
void usb_udelay (const uint32_t usec)
{
    hwp_delay(usec, TIM_USEC_DELAY);
}

/*!
    \brief      delay in milliseconds
    \param[in]  msec: value of delay required in milliseconds
    \param[out] none
    \retval     none
*/
void usb_mdelay (const uint32_t msec)
{
    hwp_delay(msec, TIM_MSEC_DELAY);
}

/*!
    \brief      delay routine based on TIM2
    \param[in]  nTime: delay Time 
    \param[in]  unit: delay Time unit = mili sec / micro sec
    \param[out] none
    \retval     none
*/
static void hwp_delay(uint32_t nTime, uint8_t unit)
{
    hwp_delay_num = nTime;
    hwp_time_set(unit);

    while (0U != hwp_delay_num) {
    }

    timer_disable(TIMER2);
}

/*!
    \brief      configures TIM2 for delay routine based on TIM2
    \param[in]  unit: msec /usec
    \param[out] none
    \retval     none
*/
static void hwp_time_set(uint8_t unit)
{
    timer_parameter_struct  timer_base_structure;

    timer_disable(TIMER2);
    timer_interrupt_disable(TIMER2, TIMER_INT_UP);

    if (TIM_USEC_DELAY == unit) {
        timer_base_structure.period = 7U;
    } else if (TIM_MSEC_DELAY == unit) {
        timer_base_structure.period = 7999U;
    } else {
        /* no operation */
    }

    timer_base_structure.prescaler = (uint16_t)timer_prescaler;

    timer_base_structure.clockdivision = 0U;
    timer_base_structure.counterdirection = TIMER_COUNTER_UP;

    timer_init(TIMER2, &timer_base_structure);

    timer_interrupt_flag_clear(TIMER2, TIMER_INT_UP);

    timer_auto_reload_shadow_enable(TIMER2);

    /* TIM IT enable */
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);

    /* TIM2 enable counter */ 
    timer_enable(TIMER2);
}
