/*!
    \file  app.c
    \brief USB main routine for HID IAP(In Application Programming) device

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

#include "iap_core.h"
#include "usb_delay.h"

usb_core_handle_struct usbhs_core_dev = 
{
    .dev = 
    {
        .class_init = iap_init,
        .class_deinit = iap_deinit,
        .class_req_handler = iap_req_handler,
        .class_data_handler = iap_data_handler,
    },

    .udelay = usb_udelay,
    .mdelay = usb_mdelay
};

pAppFunction application;
uint32_t app_address;

extern uint8_t usbd_serial_string[];

void usb_clock_config(void);
void usb_gpio_config(void);
void usb_interrupt_config(void);
void serial_string_create(void);


/*!
    \brief      main routine will construct a USB keyboard
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure Tamper key to run firmware */
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);

    /* tamper key must be pressed on GD32150R-EVAL when power on */
    if (gd_eval_key_state_get(KEY_TAMPER) != 0) {
        /* test if user code is programmed starting from address 0x8004000 */
        if (((*(__IO uint32_t*)APP_LOADED_ADDR) & 0x2FFE0000) == 0x20000000) {
            app_address = *(__IO uint32_t*) (APP_LOADED_ADDR + 4);
            application = (pAppFunction) app_address;

            /* initialize user application's stack pointer */
            __set_MSP(*(__IO uint32_t*) APP_LOADED_ADDR);

            /* jump to user application */
            application();
        }
    }

    /* configure USB GPIO */
    usb_gpio_config();

    /* configure USB clock */
    usb_clock_config();

    serial_string_create();

    usbhs_core_dev.dev.dev_desc = (uint8_t *)&device_descripter;
    usbhs_core_dev.dev.config_desc = (uint8_t *)&configuration_descriptor;
    usbhs_core_dev.dev.strings = usbd_strings;

    /* USB device stack configure */
    usbd_init(&usbhs_core_dev, 
#ifdef USE_USBFS
    USB_FS_CORE_ID
#elif defined(USE_USBHS)
    USB_HS_CORE_ID
#endif
    );

    /* USB interrupt configure */
    usb_interrupt_config();

    /* check if USB device is enumerated successfully */
    while (usbhs_core_dev.dev.status != USB_STATUS_CONFIGURED) {
    }

    while (1) {
    }
}

/*!
    \brief      configure USB clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_clock_config(void)
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
    \brief      configure USB data line gpio
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_SYSCFG);

#ifdef USE_USBFS

    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11 | GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_11 | GPIO_PIN_12);

    gpio_af_set(GPIOA, GPIO_AF_10, GPIO_PIN_11 | GPIO_PIN_12);

#elif defined(USE_USBHS)

    #ifdef USE_ULPI_PHY
        rcu_periph_clock_enable(RCU_GPIOA);
        rcu_periph_clock_enable(RCU_GPIOB);
        rcu_periph_clock_enable(RCU_GPIOC);
        rcu_periph_clock_enable(RCU_GPIOH);
        rcu_periph_clock_enable(RCU_GPIOI);

        /* ULPI_STP(PC0) GPIO pin configuration */
        gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
        gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_0);

        /* ULPI_CK(PA5) GPIO pin configuration */
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_5);

        /* ULPI_NXT(PH4) GPIO pin configuration */
        gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
        gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_4);

        /* ULPI_DIR(PI11) GPIO pin configuration */
        gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
        gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_11);

        /* ULPI_D1(PB0), ULPI_D2(PB1), ULPI_D3(PB10), ULPI_D4(PB11) \
           ULPI_D5(PB12), ULPI_D6(PB13) and ULPI_D7(PB5) GPIO pin configuration */
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, \
                        GPIO_PIN_5 | GPIO_PIN_13 | GPIO_PIN_12 |\
                        GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_1 | GPIO_PIN_0);
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, \
                        GPIO_PIN_5 | GPIO_PIN_13 | GPIO_PIN_12 |\
                        GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_1 | GPIO_PIN_0);

        /* ULPI_D0(PA3) GPIO pin configuration */
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_3);

        gpio_af_set(GPIOC, GPIO_AF_10, GPIO_PIN_0);
        gpio_af_set(GPIOH, GPIO_AF_10, GPIO_PIN_4);
        gpio_af_set(GPIOI, GPIO_AF_10, GPIO_PIN_11);
        gpio_af_set(GPIOA, GPIO_AF_10, GPIO_PIN_5 | GPIO_PIN_3);
        gpio_af_set(GPIOB, GPIO_AF_10, GPIO_PIN_5 | GPIO_PIN_13 | GPIO_PIN_12 |\
                                       GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_1 | GPIO_PIN_0);
    #elif defined(USE_EMBEDDED_PHY)

        rcu_periph_clock_enable(RCU_GPIOB);

        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

        gpio_af_set(GPIOB, GPIO_AF_12, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    #endif /* USE_ULPI_PHY */

#endif /* USE_USBFS */
}

/*!
    \brief      configure USB interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_interrupt_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

#ifdef USE_USBFS
    nvic_irq_enable((uint8_t)USBFS_IRQn, 2U, 0U);
#elif defined(USE_USBHS)
    nvic_irq_enable((uint8_t)USBHS_IRQn, 2U, 0U);
#endif /* USE_USBFS */

#ifdef USBFS_LOW_PWR_MGMT_SUPPORT

    /* enable the power module clock */
    rcu_periph_clock_enable(RCU_PMU);

    /* USB wakeup EXTI line configuration */
    exti_interrupt_flag_clear(EXTI_18);
    exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_enable(EXTI_18);

    nvic_irq_enable((uint8_t)USBFS_WKUP_IRQn, 1U, 0U);

#elif defined(USBHS_LOW_PWR_MGMT_SUPPORT)

    /* enable the power module clock */
    rcu_periph_clock_enable(RCU_PMU);

    /* USB wakeup EXTI line configuration */
    exti_interrupt_flag_clear(EXTI_20);
    exti_init(EXTI_20, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_enable(EXTI_20);

    nvic_irq_enable((uint8_t)USBHS_WKUP_IRQn, 1U, 0U);

#endif /* USBHS_LOW_PWR_MGMT_SUPPORT */

#ifdef USBHS_DEDICATED_EP1_ENABLED
    nvic_irq_enable(USBHS_EP1_Out_IRQn, 1, 0);
    nvic_irq_enable(USBHS_EP1_In_IRQn, 1, 0);
#endif /* USBHS_DEDICATED_EP1_ENABLED */
}

/*!
    \brief      create the serial number string descriptor
    \param[in]  none
    \param[out] none
    \retval     none
*/
void  serial_string_create (void)
{
    uint32_t device_serial = *(uint32_t*)DEVICE_ID;

    if(0 != device_serial) {
        usbd_serial_string[2] = (uint8_t)device_serial;
        usbd_serial_string[3] = (uint8_t)(device_serial >> 8);
        usbd_serial_string[4] = (uint8_t)(device_serial >> 16);
        usbd_serial_string[5] = (uint8_t)(device_serial >> 24);
    }
}
