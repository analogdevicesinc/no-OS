/*******************************************************************************
 *   @file   zephyr/zephyr_spi.h
 *   @brief  Header file for the zephyr spi driver.
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#ifndef ZEPHYR_SPI_H_
#define ZEPHYR_SPI_H_

#include <stdint.h>
#include "no_os_spi.h"
#include <zephyr/drivers/spi.h>

/**
 * @struct zephyr_spi_init_param
 * @brief Structure holding the initialization parameters for zephyr platform
 * specific SPI parameters.
 */

/*
 * TO DO: extend SPI for more than 8 byte transfers (?)
 * 1/2 word configuration
 */

struct zephyr_spi_init_param {
	/** SPI bus */
	struct device *bus;
	/** Slave specific configuration */
	struct spi_config *config;
};

/**
 * @struct zephyr_spi_desc
 * @brief zephyr platform specific SPI descriptor
 */
struct zephyr_spi_desc {
	/** SPI bus */
	struct device *bus;
	/** Slave specific configuration */
	struct spi_config *config;
};

/**
 * @brief zephyr specific SPI platform ops structure
 */
extern const struct no_os_spi_platform_ops zephyr_spi_ops;

#endif // ZEPHYR_SPI_H_
