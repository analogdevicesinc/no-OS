/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ADE7758 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "ade7758.h"
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
extern struct no_os_spi_init_param ade7758_spi_ip ;
/* GPIO IRQ init params */
extern struct no_os_gpio_init_param ade7758_gpio_irq_ip;
/* GPIO interrupt init params */
extern struct no_os_irq_init_param ade7758_gpio_int_ip;

/* Read data interval in irq multiple */
#define PRINT_INTERVAL			50

/* Setup values for ADE7758 */
/* value for R_small defined by user */
#define RSMALL				1
/* value for R high defined by user */
#define RHIGH                       	0
#define V_GAIN                      	((RSMALL + RHIGH) / RSMALL)
/* Change the gain accordingly to the current sensor
and sense circuit */
#define I_GAIN                      	1
/* RMS full scale code value defined by user */
#define VOLTAGE_RMS_FS_CODE        	1639101
#define CURRENT_RMS_FS_CODE         	1914753
/* ADC input at full scale rms voltage mV */
#define ADC_FS_RMS_IN               	353.5
/* Frequency calibration coeffiecient */
#define FREQ_CC                     	0.0625
/* Temperature resolution 3 °C / LSB */
#define TEMP_G                     	3
/* Temperature offset - user defined */
#define ADE7758_AMB_T_CODE          	0
/* Ambient temperature °C - user defined */
#define ADE7758_AMB_T_CELSIUS       	0

/* Read measurements */
int read_measurements(struct ade7758_dev *dev);

/* Print measurements */
int print_measurements(struct ade7758_dev *dev);

#endif /* __COMMON_DATA_H__ */
