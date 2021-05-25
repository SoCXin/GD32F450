/*!
    \file  app.c
    \brief USB main routine for HID/printer composite device

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

#include "hid_printer_wrapper.h"
#include "usb_delay.h"

usb_core_handle_struct usbfs_core_dev =
{
    .dev = 
    {
        .dev_desc = (uint8_t *)&device_descripter,
        .config_desc = (uint8_t *)&configuration_descriptor,
        .strings = usbd_strings,
        .class_init = hid_printer_init,
        .class_deinit = hid_printer_deinit,
        .class_req_handler = hid_printer_req_handler,
        .class_data_handler = hid_printer_data_handler
    },

    .udelay = delay_us,
    .mdelay = delay_ms
};

void usb_clock_config(void);
void usb_interrupt_config(void);
void key_config(void);
void led_config(void);

uint8_t timer_prescaler = 0;
uint32_t usbfs_prescaler = 0;

/*!
    \brief      main routine will construct a USB mass storage device
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main (void)
{
    /* configure USB clock */
    usb_clock_config();

    /* configure key */
    key_config();

    /* configure led */
    led_config();

    /* timer nvic initialization */
    timer_nvic_init();

    /* USB device stack configure */
    usbd_init(&usbfs_core_dev, USB_FS_CORE_ID);

    /* USB interrupt configure */
    usb_interrupt_config();

    /* check if USB device is enumerated successfully */
    while (usbfs_core_dev.dev.status != USB_STATUS_CONFIGURED) {}

    while(1){
    }
}

/*!
    \brief      configure USB clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_clock_config (void)
{
#ifdef USE_USBFS
    rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLQ);

    rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);

    rcu_periph_clock_enable(RCU_USBFS);

#elif defined(USE_USBHS)

    #ifdef USE_EMBEDDED_PHY
        rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLQ);

        rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);

    #elif defined(USE_ULPI_PHY)
        rcu_periph_clock_enable(RCU_USBHSULPI);

    #endif /* USE_EMBEDDED_PHY */

    rcu_periph_clock_enable(RCU_USBHS);
#endif /* USB_USBFS */
}

/*!
    \brief      configure USB interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_interrupt_config (void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable((uint8_t)USBFS_IRQn, 2U, 0U);

    /* enable the power module clock */
    rcu_periph_clock_enable(RCU_PMU);

    /* USB wakeup EXTI line configuration */
    exti_interrupt_flag_clear(EXTI_18);
    exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_enable(EXTI_18);

    nvic_irq_enable((uint8_t)USBFS_WKUP_IRQn, 1U, 0U);
}

/*!
    \brief      configure key
    \param[in]  none
    \param[out] none
    \retval     none
*/
void key_config (void)
{
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_EXTI);
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);
}

/*!
    \brief      configure the leds
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config (void)
{
    /* initialize LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
}
