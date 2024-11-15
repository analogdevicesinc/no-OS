/***************************************************************************//**
 *   @file   pico/pico_spi.h
 *   @brief  Header file for the pico spi driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
