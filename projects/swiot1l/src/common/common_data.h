/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by the swiot1l project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "ad74413r.h"
#include "max14906.h"
#include "adin1110.h"
#include "adt75.h"

#ifdef SWIOT1L_DEFAULT_FW
#include "iio_trigger.h"
#include "swiot.h"

#define AD74413R_GPIO_TRIG_NAME "ad74413r-dev0"
#endif

extern struct no_os_uart_init_param uart_ip;
extern struct adin1110_init_param adin1110_ip;
extern const struct no_os_spi_init_param adin1110_spi_ip;
extern const struct no_os_spi_init_param ad74413r_spi_ip;
extern struct no_os_spi_init_param max14906_spi_ip;
extern struct swiot_iio_desc_init_param swiot_ip;
extern struct max149x6_init_param max14906_ip;
extern struct ad74413r_init_param ad74413r_ip;
extern struct adt75_init_param adt75_ip;
extern const struct no_os_gpio_init_param adin1110_int_gpio_ip;
extern const struct no_os_gpio_init_param tx_perf_gpio_ip;
extern const struct no_os_gpio_init_param rx_perf_gpio_ip;
extern const struct no_os_gpio_init_param adin1110_rst_gpio_ip;
extern const struct no_os_gpio_init_param adin1110_swpd_ip;
extern const struct no_os_gpio_init_param adin1110_tx2p4_ip;
extern const struct no_os_gpio_init_param adin1110_mssel_ip;
extern const struct no_os_gpio_init_param adin1110_cfg1_ip;
extern const struct no_os_gpio_init_param adin1110_cfg0_ip;
extern const struct no_os_gpio_init_param adin1110_int_ip;
extern const struct no_os_gpio_init_param swiot_led1_ip;
extern const struct no_os_gpio_init_param swiot_led2_ip;
extern const struct no_os_gpio_init_param psu_gpio_ip;
extern struct no_os_gpio_init_param ad74413r_reset_ip;
extern const struct no_os_gpio_init_param ad74413r_ldac_ip;
extern const struct no_os_gpio_init_param ad74413r_irq_ip;
extern const struct no_os_gpio_init_param max14906_en_ip;
extern const struct no_os_gpio_init_param max14906_d1_ip;
extern const struct no_os_gpio_init_param max14906_d2_ip;
extern const struct no_os_gpio_init_param max14906_d3_ip;
extern const struct no_os_gpio_init_param max14906_d4_ip;
extern const struct no_os_gpio_init_param max14906_synch_ip;
extern const struct no_os_irq_init_param ad74413r_nvic_ip;

#ifdef SWIOT1L_DEFAULT_FW
extern struct iio_trigger ad74413r_iio_trig_desc;
extern struct iio_trigger swiot_trigger;
extern struct iio_hw_trig_init_param ad74413r_gpio_trig_ip;
extern struct no_os_irq_init_param ad74413r_gpio_irq_ip;
extern struct iio_trigger ad74413r_iio_trig_desc;
#endif

#endif /* __COMMON_DATA_H__ */
