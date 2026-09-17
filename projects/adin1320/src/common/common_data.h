/***************************************************************************//**
*   @file   common_data.h
*   @brief  Defines common data to be used by ADIN1320 examples.
*   @author John Roed Retuya (Johnroed.Retuya@analog.com)
********************************************************************************
* Copyright 2026(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "mdio_spi.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_spi.h"
#include "no_os_uart.h"
#include "parameters.h"

/* UART config */

#define UART_DEV_ID		0
#define UART_IRQ_ID		UART0_IRQn
#define UART_BAUDRATE		115200

/* SPI Configuration */

#define SPI_DEVICE_ID		0
#define SPI_CS			0
#define SPI_BAUDRATE		2500000

/* MDIO Configuration */

#define MDIO_OPS		&mdio_spi_ops

/* GPIO Pins configuration */

#define PORT_NUMBER		0

/* LED0 (P0.26) = Blue, LED1 (P0.25) = Yellow,
 * LED2 (P0.24) = Red, LED3 (P0.23) = Green */
#define LED0_NUMBER		26
#define LED1_NUMBER		25
#define LED2_NUMBER		24
#define LED3_NUMBER		23

#define CFG0_NUMBER		27
#define CFG1_NUMBER		28
#define CFG2_NUMBER		29
#define CFG3_NUMBER		30

/* ADIN1320 PHY Address 0 device */
#define ADIN1320_0_PHY_ADDR	0
#define ADIN1320_0_RESET_BUTTON	11
#define ADIN1320_0_INT_N_PIN	19

/* ADIN1320 PHY Address 8 device */
#define ADIN1320_8_PHY_ADDR	8
#define ADIN1320_8_RESET_BUTTON	10
#define ADIN1320_8_INT_N_PIN	21

#define ADIN1320_INT_N_IRQn	GPIO0_IRQn

extern struct no_os_uart_init_param uart_ip;
extern struct no_os_spi_init_param spi_ip;
extern struct no_os_gpio_init_param gpio_uc_cfg0_ip;
extern struct no_os_gpio_init_param gpio_uc_cfg1_ip;
extern struct no_os_gpio_init_param gpio_uc_cfg2_ip;
extern struct no_os_gpio_init_param gpio_uc_cfg3_ip;
extern struct no_os_gpio_init_param gpio_led_green_ip;
extern struct no_os_gpio_init_param gpio_led_red_ip;
extern struct no_os_gpio_init_param gpio_led_yellow_ip;
extern struct no_os_gpio_init_param gpio_led_blue_ip;
extern struct no_os_gpio_init_param gpio_adin1320_0_reset_ip;
extern struct no_os_gpio_init_param gpio_adin1320_8_reset_ip;
extern struct no_os_gpio_init_param gpio_adin1320_0_int_n_ip;
extern struct no_os_gpio_init_param gpio_adin1320_8_int_n_ip;
extern struct no_os_irq_init_param irq_nvic_ip;
extern struct no_os_irq_init_param irq_int_n_ip;

#endif /* __COMMON_DATA_H__ */
