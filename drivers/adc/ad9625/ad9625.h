/***************************************************************************//**
 * @file ad9625.h
 * @brief Header file of AD9625 Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 *******************************************************************************
 * Copyright 2014(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef AD9625_H_
#define AD9625_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_delay.h"
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD9625_REG_CHIP_PORT_CONF				0x000
#define AD9625_REG_CHIP_ID					0x001
#define AD9625_REG_POWER_MODE					0x008
#define AD9625_REG_PLL_STATUS					0x00A
#define AD9625_REG_TEST_CNTRL					0x00D
#define AD9625_REG_OUTPUT_MODE					0x014
#define AD9625_REG_OUTPUT_ADJUST				0x015
#define AD9625_REG_SYSREF_CONTROL				0x03A
#define AD9625_REG_JESD204B_LINK_CNTRL_1			0x05F
#define AD9625_REG_JESD204B_CONFIGURATION			0x072
#define AD9625_REG_JESD204B_LANE_POWER_MODE			0x080
#define AD9625_REG_TRANSFER					0x0FF
#define AD9625_REG_IRQ_STATUS					0x100
#define AD9625_REG_DIVCLK_OUT_CNTRL				0x120
#define AD9625_REG_SYSREF_SETUP_TIME_GUARDBAND			0x13C

#define AD9625_CHIP_ID						0x041

#define AD9625_TEST_OFF						0x000
#define AD9625_TEST_MID_SCALE					0x001
#define AD9625_TEST_POS_FSCALE					0x002
#define AD9625_TEST_NEG_FSCALE					0x003
#define AD9625_TEST_CHECKBOARD					0x004
#define AD9625_TEST_PNLONG					0x005
#define AD9625_TEST_ONE2ZERO					0x007
#define AD9625_TEST_PATTERN					0x008
#define AD9625_TEST_RAMP					0x00F

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct ad9625_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* Device Settings */
	uint32_t	lane_rate_kbps;
	uint32_t	test_samples[4];
};

struct ad9625_dev {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t ad9625_spi_read(struct ad9625_dev *dev,
			uint16_t reg_addr,
			uint8_t *reg_data);
int32_t ad9625_spi_write(struct ad9625_dev *dev,
			 uint16_t reg_addr,
			 uint8_t reg_data);
int32_t ad9625_setup(struct ad9625_dev **device,
		     struct ad9625_init_param init_param);
int32_t ad9625_remove(struct ad9625_dev *dev);
int32_t ad9625_test(struct ad9625_dev *dev, uint32_t test_mode);

#endif
