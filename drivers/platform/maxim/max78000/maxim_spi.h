/***************************************************************************//**
 *   @file   maxim_spi.h
 *   @brief  maxim specific header for SPI driver
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef MAXIM_SPI_H_
#define MAXIM_SPI_H_

#include <stdint.h>
#include "gpio.h"
#include "no_os_spi.h"
#include "no_os_dma.h"
#include "maxim_dma.h"

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
