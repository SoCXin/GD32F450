/*!
    \file  main.c 
    \brief this file realizes the MSC host

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
#include "usbh_msc_core.h"
#include "usbh_std.h"
#include "usbh_ctrl.h"
#include "gd32f450i_eval.h"

#ifdef USE_USBFS
#define HOST_POWERSW_PORT_RCC     RCU_GPIOD
#define HOST_POWERSW_PORT         GPIOD
#define HOST_POWERSW_VBUS         GPIO_PIN_13
#elif defined(USE_USBHS)
    #ifdef USE_EMBEDDED_PHY
#define HOST_POWERSW_PORT_RCC     RCU_GPIOD
#define HOST_POWERSW_PORT         GPIOD
#define HOST_POWERSW_VBUS         GPIO_PIN_13
    #elif defined(USE_ULPI_PHY)
#define HOST_POWERSW_PORT_RCC     RCU_GPIOD
#define HOST_POWERSW_PORT         GPIOD
#define HOST_POWERSW_VBUS         GPIO_PIN_13
    #endif /* USE_EMBEDDED_PHY */
#endif /* USE_USBFS */

void usb_rcu_init                 (void);
void usb_gpio_init                (void);
void usb_hwp_interrupt_enable     (usb_core_handle_struct *pudev);
void usb_hwp_vbus_config          (usb_core_handle_struct *pudev);
void usb_hwp_vbus_drive           (void *pudev, uint8_t state);
void system_clock_config          (void);

usb_core_handle_struct usb_core_dev =
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
    .usbh_backup_state.host_backup_state      = HOST_IDLE,
    .usbh_backup_state.enum_backup_state      = ENUM_IDLE,
    .usbh_backup_state.ctrl_backup_state      = CTRL_IDLE,
    .usbh_backup_state.class_req_backup_state = 0,
    .usbh_backup_state.class_backup_state     = 0,

    .usr_cb         = &usr_cb,
    .class_init     = usbh_msc_interface_init,
    .class_deinit   = usbh_msc_interface_deinit,

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
  
    /* usb_timer init */
    usb_time_init();

    /* configure GPIO pin used for switching VBUS power */
    usb_hwp_vbus_config(&usb_core_dev);

    /* host de-initializations */
    usbh_deinit(&usb_core_dev, &usb_host, &usbh_state_core);

    /* start the USB core */
    hcd_init(&usb_core_dev, 
  
#ifdef USE_USBFS
    USB_FS_CORE_ID
#elif defined(USE_USBHS)
    USB_HS_CORE_ID
#endif /* USE_USBFS */
             );

    /* init usr call back */
    usb_host.usr_cb->init();

    /* enable interrupts */
    usb_hwp_interrupt_enable(&usb_core_dev);
  
    while (1) {
        host_state_polling_fun(&usb_core_dev, &usb_host, &usbh_state_core);
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
#ifdef USE_USBFS
    rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLQ);
    rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);
    rcu_periph_clock_enable(RCU_USBFS);
#elif defined(USE_USBHS)
    rcu_periph_clock_enable(RCU_USBHS);
    #ifdef USE_EMBEDDED_PHY
    #elif defined(USE_ULPI_PHY)
    rcu_periph_clock_enable(RCU_USBHSULPI);
    #endif /* USE_EMBEDDED_PHY */
#endif /* USE_USBFS */
}

/*!
    \brief      initialize usb gpio
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_gpio_init (void)
{
    rcu_periph_clock_enable(RCU_SYSCFG);
    rcu_periph_clock_enable(RCU_GPIOA);
#ifdef USE_USBFS
    /* enable the power module clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* USBFS_DM(PA11) and USBFS_DP(PA12) GPIO pin configuration */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11 | GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_11|GPIO_PIN_12);
    gpio_af_set(GPIOA, GPIO_AF_10, GPIO_PIN_11 | GPIO_PIN_12);
  
#elif defined(USE_USBHS)
        rcu_periph_clock_enable(RCU_GPIOB);
    #ifdef USE_EMBEDDED_PHY
        rcu_periph_clock_enable(RCU_GPIOB);
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14 | GPIO_PIN_15);
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_14 | GPIO_PIN_15);
        gpio_af_set(GPIOB, GPIO_AF_12, GPIO_PIN_14 | GPIO_PIN_15);
        rcu_periph_clock_enable(RCU_USBHS);
  
    #elif defined(USE_ULPI_PHY)
        rcu_periph_clock_enable(RCU_GPIOC);
        rcu_periph_clock_enable(RCU_GPIOF);
        rcu_periph_clock_enable(RCU_GPIOH);
        rcu_periph_clock_enable(RCU_GPIOI);
        /* ULPI_STP(PC0), ULPI_DIR(PI11) and ULPI_NXT(PH4) GPIO pin configuration */
        gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
        gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_0);
  
        gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
        gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_11);

        gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
        gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_4);
        /* ULPI_CK(PA5) GPIO pin configuration */
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_5);

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
        gpio_af_set(GPIOA, GPIO_AF_10, GPIO_PIN_5 | GPIO_PIN_3);
        gpio_af_set(GPIOB, GPIO_AF_10, GPIO_PIN_5 | GPIO_PIN_13 | GPIO_PIN_12 |\
                                       GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_1 | GPIO_PIN_0);
        gpio_af_set(GPIOH, GPIO_AF_10, GPIO_PIN_4);
        gpio_af_set(GPIOI, GPIO_AF_10, GPIO_PIN_11);
    #endif /* USE_EMBEDDED_PHY */
#endif /* USE_USBFS */
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
#ifdef USE_USBFS
    nvic_irq_enable((uint8_t)USBFS_IRQn, 2U, 0U);
#elif defined(USE_USBHS)
    nvic_irq_enable((uint8_t)USBHS_IRQn, 2U, 0U);
#endif /* USE_USBFS */
}

/*!
    \brief      drives the Vbus signal through GPIO
    \param[in]  pudev: pointer to usb device
    \param[out] none
    \retval     none
*/
void usb_hwp_vbus_drive(void *pudev, uint8_t state)
{
    if (0U == state){
        /* disable is needed on output of the power switch */
        gpio_bit_reset(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
    } else {
        /* enable the power switch by driving the enable high */
        gpio_bit_set(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
    }
}

/*!
    \brief      configures the gpio for the vbus and over-current
    \param[in]  pudev: pointer to usb device
    \param[out] none
    \retval     none
*/
void usb_hwp_vbus_config(usb_core_handle_struct *pudev)
{
    rcu_periph_clock_enable(HOST_POWERSW_PORT_RCC);

    gpio_mode_set(HOST_POWERSW_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, HOST_POWERSW_VBUS);
    gpio_output_options_set(HOST_POWERSW_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, HOST_POWERSW_VBUS);
  
    /* by default, disable is needed on output of the power switch */
    gpio_bit_set(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);

    /* delay is need for stabilising the vbus low in reset condition, 
     * when vbus = 1 and reset-button is pressed by user 
     */
    if (NULL != pudev->mdelay) {
        pudev->mdelay(200U);
    }
}
