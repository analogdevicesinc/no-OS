/***************************************************************************//**
 *   @file   maxim_spi.h
 *   @brief  maxim specific header for SPI driver
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MAXIM_SPI_H_
#define MAXIM_SPI_H_

#include <stdint.h>
#include "no_os_spi.h"
#include "gpio.h"
#include "no_os_dma.h"

/**
 * @brief maxim specific SPI platform ops structure
 */
extern const struct no_os_spi_platform_ops max_spi_ops;

enum spi_ss_polarity {
	SPI_SS_POL_LOW,
	SPI_SS_POL_HIGH
};

struct max_spi_init_param {
	uint32_t num_slaves;
	enum spi_ss_polarity polarity;
	mxc_gpio_vssel_t vssel;
	struct no_os_dma_init_param *dma_param;
	uint32_t dma_rx_priority;
	uint32_t dma_tx_priority;
};

struct max_spi_state {
	struct max_spi_init_param *init_param;
	uint32_t cs_delay_first;
	uint32_t cs_delay_last;
	struct no_os_dma_desc *dma;
	uint32_t dma_req_rx;
	uint32_t dma_req_tx;
};

#endif
