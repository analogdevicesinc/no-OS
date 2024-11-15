/***************************************************************************//**
 *   @file   AD7303.h
 *   @brief  Header file of AD7303 Driver.
 *   @author Mihai Bancisor(Mihai.Bancisor@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __AD7303_H__
#define __AD7303_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_spi.h"

/******************************************************************************/
/******************************** AD7303 **************************************/
/******************************************************************************/
/* Control Bits */
#define AD7303_INT       (0 << 7)    // Selects internal reference.
#define AD7303_EXT       (1 << 7)    // Selects external reference.
#define AD7303_LDAC      (1 << 5)    // Load DAC bit.
#define AD7303_PDB       (1 << 4)    // Power-down DAC B.
#define AD7303_PDA       (1 << 3)    // Power-down DAC A.
#define AD7303_A         (0 << 2)    // Address bit to select DAC A.
#define AD7303_B         (1 << 2)    // Address bit to select DAC B.
#define AD7303_CR1       (1 << 1)    // Control Bit 1.
#define AD7303_CR0       (1 << 0)    // Control Bit 0.

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct ad7303_dev {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
};

struct ad7303_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes SPI communication. */
int8_t ad7303_init(struct ad7303_dev **device,
		   struct ad7303_init_param init_param);

/*! Free the resources allocated by ad7303_init(). */
int32_t ad7303_remove(struct ad7303_dev *dev);

/*! Sends data to ad7303. */
void ad7303_write(struct ad7303_dev *dev,
		  uint8_t control_reg,
		  uint8_t data_reg);

#endif /* __AD7303_H__ */
