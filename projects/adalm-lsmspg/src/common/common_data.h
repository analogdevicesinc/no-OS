/***************************************************************************//**
 *   @file   common_data.h
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

extern struct no_os_spi_init_param ad5592r_spi_ip;
extern struct no_os_uart_init_param uart_ip;
extern struct no_os_irq_init_param timer_irq_ip;
extern struct no_os_timer_init_param timer_ip;
extern struct no_os_gpio_init_param led_gpio_ip;
extern struct no_os_i2c_init_param ad5593r_i2c_ip;
extern struct no_os_i2c_init_param lm75_i2c_ip;

#endif /* COMMON_DATA_H_*/
