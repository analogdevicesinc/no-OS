/***************************************************************************//**
 *   @file   pulsar-adc/src/common/common_data.h
 *   @brief  Defines common data to be used by pulsar-adc examples.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "platform_includes.h"
#include "pulsar_adc.h"

#define SAMPLES_PER_CHANNEL     SAMPLES_PER_CHANNEL_PLATFORM

extern struct pulsar_adc_init_param pulsar_adc_init_param;
extern struct no_os_uart_init_param pulsar_adc_uart_ip;
#ifndef USE_STANDARD_SPI
extern struct spi_engine_offload_init_param spi_engine_offload_init_param;
#endif
#endif /* COMMON_DATA_H_ */
