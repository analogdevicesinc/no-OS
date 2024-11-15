/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by the STOUT project.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
struct no_os_uart_init_param ade9113_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS
};

struct no_os_spi_init_param ade9113_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_3,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA
};

struct no_os_gpio_init_param ade9113_gpio_rdy_ip = {
	.port = GPIO_RDY_PORT_NUM,
	.number = GPIO_RDY_PIN_NUM,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param ade9113_gpio_reset_ip = {
	.port = GPIO_RESET_PORT_NUM,
	.number = GPIO_RESET_PIN_NUM,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param ade9113_gpio_led2_ip = { //led 2
	.port = 0,
	.number = 30,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param ade9113_gpio_relay_ip = {  //relay control pin
	.port = RELAY_CTRL_PORT,
	.number = RELAY_CTRL_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param ade9113_gpio_led1_ip = {  //led 1
	.port = 0,
	.number = 31,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param ade9113_gpio_ZX_ip = {  //zero cross detection
	.port = 0,
	.number = GPIO_ZC_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param ade9113_gpio_rcm_test_ip = {  //RCM test
	.port = 0,
	.number = GPIO_RCM_TEST_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param ade9113_gpio_rcdac_ip = {  //rcdac detection
	.port = 0,
	.number = GPIO_RCDAC_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param ade9113_gpio_rcddc_ip = {  //rcddc detection
	.port = 0,
	.number = GPIO_RCDDC_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param ade9113_gpio_led3_ip = { 	//led 3
	.port = 1,
	.number = 0,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param ade9113_gpio_led4_ip = {	//led 4
	.port = 1,
	.number = 1,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

#if defined(REV_D)
struct no_os_gpio_init_param gpio_ip = {
	.port = INT_PORT,
	.number = INT_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};
#endif

struct no_os_irq_init_param ade9113_gpio_irq_ip = {
	.platform_ops = GPIO_IRQ_OPS,
	.irq_ctrl_id = GPIO_CTRL_IRQ_ID,
	.extra = GPIO_IRQ_EXTRA,
};

const struct no_os_i2c_init_param adt75_param = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = 400000,
	.platform_ops = I2C_OPS,
	.slave_address = ADT75_ADDR(0),
	.extra = I2C_EXTRA,
};

struct adt75_init_param adt75_ip = {
	.comm_param = adt75_param,
};

struct ade9113_init_param ade9113_ip = { 0 };
