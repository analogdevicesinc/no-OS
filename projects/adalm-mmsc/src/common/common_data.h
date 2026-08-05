/***************************************************************************//**
 *   @file common_data.h
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

extern struct no_os_timer_init_param timer_init_param;
extern struct no_os_irq_init_param timer_irq_ip;

extern struct no_os_gpio_init_param led_ctrl_init_param ;
extern struct no_os_gpio_init_param afe_ctrl_init_param;
extern struct no_os_gpio_init_param gate_ctrl_q4_init_param;
extern struct no_os_gpio_init_param gate_ctrl_q3_init_param;
extern struct no_os_gpio_init_param gate_ctrl_q2_init_param;
extern struct no_os_gpio_init_param gate_ctrl_q1_init_param;

extern struct no_os_gpio_init_param gp3_init_param;

extern struct no_os_gpio_init_param data_ss_init_param;
extern struct no_os_spi_init_param data_spi_init_param;

extern struct no_os_gpio_init_param cfg_ss_init_param;
extern struct no_os_spi_init_param cfg_spi_init_param;

extern struct no_os_uart_init_param uart_init_param;

#endif /* __COMMON_DATA_H__ */
