/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by iio_adpd1080 examples.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "parameters.h"
#include "adpd188.h"
#include "iio_adpd188.h"
#include "no_os_uart.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_timer.h"

/* Sync GPIO pin number used during the 32kHz calibration. */
#define ADPD1080_SYNC_GPIO_NUM  GPIO_SYNC_NUM

extern struct no_os_uart_init_param adpd1080_uart_ip;
extern struct adpd188_iio_init_param adpd1080_iio_ip;

extern struct no_os_timer_init_param adpd1080_cal_timer_ip;
extern struct no_os_gpio_init_param adpd1080_sync_gpio_ip;
extern struct no_os_irq_init_param adpd1080_cal_irq_ip;

#endif /* COMMON_DATA_H_ */
