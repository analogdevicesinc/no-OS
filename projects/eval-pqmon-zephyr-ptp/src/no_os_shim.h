/********************************************************************************
 *   @file   no_os_shim.h
 *   @brief  no-OS API to Zephyr shim header file
 *   @author Radu Etz (radu.etz@analog.com)
 ********************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifndef __NO_OS_SHIM_H__
#define __NO_OS_SHIM_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* no-OS error codes */
#define NO_OS_SPI_MODE_0 0
#define NO_OS_SPI_MODE_1 1
#define NO_OS_SPI_MODE_2 2
#define NO_OS_SPI_MODE_3 3
#define NO_OS_SPI_BIT_ORDER_MSB_FIRST 0
#define NO_OS_PULL_NONE 0

/* BIT macro (like no_os_util.h) */
#ifndef BIT
#define BIT(x) (1UL << (x))
#endif
#ifndef NO_OS_BIT
#define NO_OS_BIT(x) BIT(x)
#endif

/* --- SPI --- */

struct no_os_spi_init_param {
	uint32_t device_id;
	uint32_t max_speed_hz;
	uint8_t  mode;
	uint8_t  bit_order;
	uint8_t  chip_select;
	void    *platform_ops;  /* unused in shim */
	void    *extra;         /* unused in shim */
};

struct no_os_spi_desc {
	const void *zephyr_dev;  /* struct device* for Zephyr SPI */
	struct no_os_spi_init_param param;
};

struct no_os_spi_msg {
	uint8_t *tx_buff;
	uint8_t *rx_buff;
	uint32_t bytes_number;
	uint8_t  cs_change;
};

int no_os_spi_init(struct no_os_spi_desc **desc,
		   const struct no_os_spi_init_param *param);
int no_os_spi_write_and_read(struct no_os_spi_desc *desc,
			     uint8_t *data, uint16_t bytes_number);
int no_os_spi_remove(struct no_os_spi_desc *desc);

/* --- GPIO --- */

struct no_os_gpio_init_param {
	uint8_t  port;
	uint8_t  number;
	uint8_t  pull;
	void    *platform_ops;  /* unused */
	void    *extra;         /* unused */
};

struct no_os_gpio_desc {
	uint8_t port;
	uint8_t number;
};

int no_os_gpio_get(struct no_os_gpio_desc **desc,
		   const struct no_os_gpio_init_param *param);
int no_os_gpio_direction_output(struct no_os_gpio_desc *desc, uint8_t value);
int no_os_gpio_set_value(struct no_os_gpio_desc *desc, uint8_t value);
int no_os_gpio_remove(struct no_os_gpio_desc *desc);

/* --- Delay --- */

void no_os_mdelay(uint32_t msecs);

/* --- Utility --- */

void no_os_memswap64(void *buf, uint32_t bytes, uint32_t step);

/* --- Global init params (provided by shim, replaces common_data.c) --- */

extern struct no_os_spi_init_param spi_egy_ip;
extern struct no_os_gpio_init_param reset_gpio_ip;

#endif /* __NO_OS_SHIM_H__ */
