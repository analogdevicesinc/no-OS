/***************************************************************************//**
 *   @file   pico/pico_spi.h
 *   @brief  Header file for the pico spi driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
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
