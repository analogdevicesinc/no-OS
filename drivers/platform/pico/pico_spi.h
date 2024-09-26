/***************************************************************************//**
 *   @file   pico/pico_spi.h
 *   @brief  Header file for the pico spi driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#ifndef _PICO_SPI_H_
#define _PICO_SPI_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_spi.h"
#include "hardware/spi.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief Available GP config for SPI TX
 */
enum spi_tx_gp {
	/* SPI0 available configurations */
	SPI0_TX_GP3 = 3,
	SPI0_TX_GP7 = 7,
	SPI0_TX_GP19 = 19,
	/* SPI1 available configurations */
	SPI1_TX_GP11 = 11,
	SPI1_TX_GP15 = 15,
};

/**
 * @brief Available GP config for SPI RX
 */
enum spi_rx_gp {
	/* SPI0 available configurations */
	SPI0_RX_GP0 = 0,
	SPI0_RX_GP4 = 4,
	SPI0_RX_GP16 = 16,
	/* SPI1 available configurations */
	SPI1_RX_GP8 = 8,
	SPI1_RX_GP12 = 12,
};

/**
 * @brief Available GP config for SPI SCK
 */
enum spi_sck_gp {
	/* SPI0 available configurations */
	SPI0_SCK_GP2 = 2,
	SPI0_SCK_GP6 = 6,
	SPI0_SCK_GP18 = 18,
	/* SPI1 available configurations */
	SPI1_SCK_GP10 = 10,
	SPI1_SCK_GP14 = 14,
};

/**
 * @brief Available GP config for SPI CS
 */
enum spi_cs_gp {
	/* SPI0 available configurations */
	SPI0_CS_GP1 = 1,
	SPI0_CS_GP5 = 5,
	SPI0_CS_GP17 = 17,
	/* SPI1 available configurations */
	SPI1_CS_GP9 = 9,
	SPI1_CS_GP13 = 13,
};

/**
 * @struct pico_spi_desc
 * @brief pico platform specific SPI descriptor
 */
struct pico_spi_desc {
	/** SPI instance */
	spi_inst_t *spi_instance;
	/** SPI TX pin configuration */
	enum spi_tx_gp spi_tx_pin;
	/** SPI RX pin configuration */
	enum spi_rx_gp spi_rx_pin;
	/** SPI SCK pin configuration */
	enum spi_sck_gp spi_sck_pin;
	/** SPI CS pin configuration */
	enum spi_cs_gp spi_cs_pin;
};

/**
 * @brief Additional SPI config parameters
 */
struct pico_spi_init_param {
	/** SPI TX pin configuration */
	enum spi_tx_gp spi_tx_pin;
	/** SPI RX pin configuration */
	enum spi_rx_gp spi_rx_pin;
	/** SPI SCK pin configuration */
	enum spi_sck_gp spi_sck_pin;
	/** SPI CS pin configuration */
	enum spi_cs_gp spi_cs_pin;
};

/**
 * @brief pico specific SPI platform ops structure
 */
extern const struct no_os_spi_platform_ops pico_spi_ops;

#endif // _PICO_SPI_H_
