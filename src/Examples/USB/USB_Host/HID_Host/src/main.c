/*!
    \file  main.c 
    \brief this file realizes the HID host

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
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_hid_core.h"
#include "gd32f450i_eval.h"

#define HOST_POWERSW_PORT_RCC    RCU_GPIOD
#define HOST_POWERSW_PORT        GPIOD
#define HOST_POWERSW_VBUS        GPIO_PIN_13

void usb_rcu_init               (void);
void usb_gpio_init              (void);
void usb_hwp_interrupt_enable   (usb_core_handle_struct *pudev);
void usb_hwp_vbus_config        (usb_core_handle_struct *pudev);
void usb_hwp_vbus_drive         (void *pudev, uint8_t state);
void system_clock_config        (void);

usb_core_handle_struct usbfs_core_dev = 
{
    .host = 
    {
        .vbus_drive = usb_hwp_vbus_drive,
    },

    .udelay = usb_udelay,
    .mdelay = usb_mdelay,
};

usbh_host_struct usb_host = 
{
    .usbh_backup_state  =
    {
        .host_backup_state      = HOST_IDLE,
        .enum_backup_state      = ENUM_IDLE,
        .ctrl_backup_state      = CTRL_IDLE,
        .class_req_backup_state = 0,
        .class_backup_state     = 0,
    },

    .usr_cb                     = &user_callback_funs,
    .class_init                 = usbh_hid_interface_init,
    .class_deinit               = usbh_hid_interface_deinit,
};

usbh_state_handle_struct usbh_state_core;

/*!
    \brief      main routine
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* config system clock */
    system_clock_config();

    /* usb gpio init */
    usb_gpio_init();

    /* usb rcu init */
    usb_rcu_init();

    /* usb timer init */
    usb_timer_init();

    /* configure GPIO pin used for switching VBUS power */
    usb_hwp_vbus_config(&usbfs_core_dev);
  
    /* host de-initializations */
    usbh_deinit(&usbfs_core_dev, &usb_host, &usbh_state_core);

    /* start the USBFS core */
    hcd_init(&usbfs_core_dev, USB_FS_CORE_ID);

    /* init user callback */
    usb_host.usr_cb->init();

    /* enable interrupts */
    usb_hwp_interrupt_enable(&usbfs_core_dev);

    while (1) {
        host_state_polling_fun(&usbfs_core_dev, &usb_host, &usbh_state_core);
    }
}

/*!
    \brief      configure system clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void system_clock_config(void)
{
    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_HXTAL;

    /* wait until PLL is selected as system clock */
    while (0U == (RCU_CFG0 & RCU_SCSS_HXTAL)) {
    }
    
    rcu_osci_off(RCU_PLL_CK);

    RCU_PLL = (25U | (384U << 6U) | (((2U >> 1U) - 1U) << 16U) |
               (RCU_PLLSRC_HXTAL) | (8U << 24U));

    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while (0U == (RCU_CTL & RCU_CTL_PLLSTB)) {
    }

    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLLP;

    /* wait until PLL is selected as system clock */
    while (0U == (RCU_CFG0 & RCU_SCSS_PLLP)) {
    }
}

/*!
    \brief      initialize usb rcu
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_rcu_init (void)
{
    rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLQ);
    rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);
    rcu_periph_clock_enable(RCU_USBFS);
}

/*!
    \brief      initialize usb gpio
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_gpio_init (void)
{
    rcu_periph_clock_enable(RCU_PMU);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_SYSCFG);
    /* USBFS_DM(PA11) and USBFS_DP(PA12) GPIO pin configuration */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11 | GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_11|GPIO_PIN_12);
    gpio_af_set(GPIOA, GPIO_AF_10, GPIO_PIN_11 | GPIO_PIN_12);
}

/*!
    \brief      drives the Vbus signal through GPIO
    \param[in]  pudev: pointer to usb device
    \param[out] none
    \retval     none
*/
void usb_hwp_vbus_drive(void *pudev, uint8_t state)
{
    if (0U == state) {
        /* disable is needed on output of the power switch */
        gpio_bit_reset(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
    } else {
        /* enable the power switch by driving the enable high */
        gpio_bit_set(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
    }
}

/*!
    \brief      configures the gpio for the vbus
    \param[in]  pudev: pointer to usb device
    \param[out] none
    \retval     none
*/
void  usb_hwp_vbus_config(usb_core_handle_struct *pudev)
{
    rcu_periph_clock_enable(HOST_POWERSW_PORT_RCC);

    /* USBFS_VBUS_CTRL(PD13) GPIO pin configuration */
    gpio_mode_set(HOST_POWERSW_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, HOST_POWERSW_VBUS);
    gpio_output_options_set(HOST_POWERSW_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, HOST_POWERSW_VBUS);

    /* by default, disable is needed on output of the power switch */
    gpio_bit_set(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);

    /* delay is need for stabilizing the vbus low in reset condition, 
     * when vbus = 1 and reset-button is pressed by user 
     */
    if ((void *)0 != pudev->mdelay) {
        pudev->mdelay(200U);
    }
}

/*!
    \brief      configure usb global interrupt
    \param[in]  pudev: pointer to usb device
    \param[out] none
    \retval     none
*/
void usb_hwp_interrupt_enable(usb_core_handle_struct *pudev)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

    nvic_irq_enable((uint8_t)USBFS_IRQn, 2U, 0U);
}
