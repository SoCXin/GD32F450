/*!
    \file  main.c
    \brief USB main routine for HID device(USB keyboard)

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

#include "hid_core.h"
#include "usb_delay.h"

typedef enum
{
    CHAR_A = 1,
    CHAR_B,
    CHAR_C
}key_char_enum;

uint8_t key_buffer[HID_IN_PACKET] = {0};

__IO uint8_t prev_transfer_complete = 1U;

usb_core_handle_struct usbhs_core_dev = 
{
    .dev = 
    {
        .dev_desc = (uint8_t *)&device_descripter,
        .config_desc = (uint8_t *)&configuration_descriptor,
        .strings = usbd_strings,
        .class_init = usbd_hid_init,
        .class_deinit = usbd_hid_deinit,
        .class_req_handler = usbd_hid_classreq_handle,
        .class_data_handler = usbd_hid_data_handler,
    },

    .udelay = usb_udelay,
    .mdelay = usb_mdelay
};

void usb_clock_config(void);
void usb_gpio_config(void);
void usb_interrupt_config(void);

#ifdef USE_IRC48M
    void ctc_config(void);
#endif

void key_config(void);
uint8_t key_state(void);

/*!
    \brief      main routine will construct a USB keyboard
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure USB GPIO */
    usb_gpio_config();

    /* configure USB clock */
    usb_clock_config();

    /* configure key */
    key_config();

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

#ifdef USE_IRC48M
    /* CTC peripheral clock enable */
    rcu_periph_clock_enable(RCU_CTC);

    /* CTC config */
    ctc_config();

    while(ctc_flag_get(CTC_FLAG_CKOK) == RESET) {
    }
#endif

    /* check if USB device is enumerated successfully */
    while (usbhs_core_dev.dev.status != USB_STATUS_CONFIGURED) {
    }

    while (1) {
        if (prev_transfer_complete) {
            switch (key_state()) {
            case CHAR_A:
                key_buffer[2] = 0x04U;
                break;
            case CHAR_B:
                key_buffer[2] = 0x05U;
                break;
            case CHAR_C:
                key_buffer[2] = 0x06U;
                break;
            default:
                break;
            }

            if (key_buffer[2] != 0U) {
                usbd_hid_report_send (&usbhs_core_dev, key_buffer, 8U);
            }
        }
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

    #ifndef USE_IRC48M
        rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLQ);

        rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);
    #else
        /* enable IRC48M clock */
        rcu_osci_on(RCU_IRC48M);

        /* wait till IRC48M is ready */
        while (SUCCESS != rcu_osci_stab_wait(RCU_IRC48M)) {
        }

        rcu_ck48m_clock_config(RCU_CK48MSRC_IRC48M);
    #endif /* USE_IRC48M */

    rcu_periph_clock_enable(RCU_USBFS);

#endif /* USE_USBFS */

#ifdef USE_USBHS

    #ifdef USE_EMBEDDED_PHY

        #ifndef USE_IRC48M
            rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLQ);

            rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);
        #else
            /* enable IRC48M clock */
            rcu_osci_on(RCU_IRC48M);

            /* wait till IRC48M is ready */
            while (SUCCESS != rcu_osci_stab_wait(RCU_IRC48M)) {
            }

            rcu_ck48m_clock_config(RCU_CK48MSRC_IRC48M);
        #endif /* USE_IRC48M */

    #elif defined(USE_ULPI_PHY)
        rcu_periph_clock_enable(RCU_USBHSULPI);
    #endif /* USE_EMBEDDED_PHY */

    rcu_periph_clock_enable(RCU_USBHS);

#endif /* USB_USBHS */
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

    /* USBFS_DM(PA11) and USBFS_DP(PA12) GPIO pin configuration */
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
    nvic_irq_enable((uint8_t)USBFS_IRQn, 3U, 0U);
#elif defined(USE_USBHS)
    nvic_irq_enable((uint8_t)USBHS_IRQn, 3U, 0U);
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

#ifdef USE_IRC48M

/*!
    \brief      configure the CTC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ctc_config(void)
{
    /* config CTC reference signal source prescaler */
    ctc_refsource_prescaler_config(CTC_REFSOURCE_PSC_OFF);
    /* select reference signal source */
    ctc_refsource_signal_select(CTC_REFSOURCE_USBSOF);
    /* select USBFS SOF */
    ctc_usbsof_signal_select(CTC_USBSOFSEL_USBFS);
    /* select reference signal source polarity */
    ctc_refsource_polarity_config(CTC_REFSOURCE_POLARITY_RISING);
    /* config hardware automatically trim mode */
    ctc_hardware_trim_mode_config(CTC_HARDWARE_TRIM_MODE_ENABLE);

    /* config CTC counter reload value */
    ctc_counter_reload_value_config(0xBB7F);
    /* config clock trim base limit value */
    ctc_clock_limit_value_config(0x1D);

    /* CTC counter enable */
    ctc_counter_enable();
}

#endif

/*!
    \brief      configure key
    \param[in]  none
    \param[out] none
    \retval     none
*/
void key_config(void)
{
    /* configure the wakeup key in exti mode to remote wakeup */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);

    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);
    gd_eval_key_init(KEY_USER, KEY_MODE_GPIO);

    exti_interrupt_flag_clear(WAKEUP_KEY_EXTI_LINE);
}

/*!
    \brief      to get usb keyboard state
    \param[in]  none
    \param[out] none
    \retval     the char
*/
uint8_t key_state (void)
{
    /* have pressed tamper key */
    if (!gd_eval_key_state_get(KEY_TAMPER)) {
        usb_mdelay(100U);

        if (!gd_eval_key_state_get(KEY_TAMPER)) {
            return CHAR_A;
        }
    }

    /* have pressed wakeup key */
    if (!gd_eval_key_state_get(KEY_WAKEUP)) {
        usb_mdelay(100U);

        if (!gd_eval_key_state_get(KEY_WAKEUP)) {
            return CHAR_B;
        }
    }

    /* have pressed user key */
    if (!gd_eval_key_state_get(KEY_USER)) {
        usb_mdelay(100U);

        if (!gd_eval_key_state_get(KEY_USER)) {
            return CHAR_C;
        }
    }

    /* no pressed any key */
    return 0U;
}
