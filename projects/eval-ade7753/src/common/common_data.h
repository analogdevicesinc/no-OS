/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ADE7753 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "ade7753.h"
#include "no_os_uart.h"
#include "no_os_pwm.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_print_log.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"
#include "maxim_uart_stdio.h"
#include "maxim_pwm.h"
#include "maxim_spi.h"
#include "maxim_irq.h"

/* UART init params */
extern struct no_os_uart_init_param uart_ip;
/* GPIO LED init params */
extern struct no_os_gpio_init_param gpio_led1_ip;
/* SPI init params */
extern struct no_os_spi_init_param ade7753_spi_ip ;
/* GPIO RESET init params */
extern struct no_os_gpio_init_param gpio_reset_ip;
/* GPIO ZX init params */
extern struct no_os_gpio_init_param ade7753_gpio_zx_ip;
/* GPIO IRQ init params */
extern struct no_os_gpio_init_param ade7753_gpio_irq_ip;
/* GPIO interrupt init params */
extern struct no_os_irq_init_param ade7753_gpio_int_ip;

/* Read data interval in line periode multiple */
#define READ_INTERVAL               50

/* Setup values for ADE7753 */

/* value for R_small 1k */
#define RSMALL                      1000
/* value for R high 1Meg */
#define RHIGH                       1000000
#define V_GAIN                      ((RSMALL + RHIGH) / RSMALL)
/* Change the gain accordingly to the current sensor
and sense circuit */
#define I_GAIN                      1
/* RMS full scale code value */
#define VOLTAGE_RMS_FS_CODE         1729993
#define CURRENT_RMS_FS_CODE         1374901
/* ADC input at full scale rms voltage mV */
#define ADC_FS_RMS_IN               353.5
/* Frequency calibration coeffiecient */
#define FREQ_CC                     448280
/* Temperature resolution 1.5 LSB / °C */
#define TEMP_G                      1.5

/* Read measurements */
int read_measurements(struct ade7753_dev *dev);

#endif /* __COMMON_DATA_H__ */
