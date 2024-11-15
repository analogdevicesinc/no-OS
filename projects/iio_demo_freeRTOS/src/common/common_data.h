/***************************************************************************//**
 *   @file   iio_demo/src/common/common_data.h
 *   @brief  Defines common data to be used by iio_demo_freeRTOS examples.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "no_os_util.h"
#include "adc_demo.h"
#include "dac_demo.h"

#ifdef FREERTOS
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "portmacro.h"
#include "task.h"
#include "semphr.h"
#include "wut.h"
#include "uart.h"
#include "lp.h"
#include "no_os_delay.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern struct no_os_uart_init_param iio_demo_uart_ip;

#define DEMO_CHANNELS no_os_max(TOTAL_ADC_CHANNELS, TOTAL_DAC_CHANNELS)

#ifdef ENABLE_LOOPBACK
#define SAMPLES_PER_CHANNEL	SAMPLES_PER_CHANNEL_PLATFORM
#else //ENABLE_LOOPBACK
#define SAMPLES_PER_CHANNEL	0
#define loopback_buffs		NULL
#endif //ENABLE_LOOPBACK

extern uint8_t in_buff[];
extern uint8_t out_buff[];

#define MAX_SIZE_BASE_ADDR	(SAMPLES_PER_CHANNEL * DEMO_CHANNELS * \
					sizeof(uint16_t))

#define DAC_DDR_BASEADDR	out_buff
#define ADC_DDR_BASEADDR	in_buff

extern struct no_os_gpio_init_param gpio_ip;
extern struct adc_demo_init_param adc_init_par;
extern struct dac_demo_init_param dac_init_par;

#endif /* __COMMON_DATA_H__ */
