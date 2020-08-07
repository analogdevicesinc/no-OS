/***************************************************************************//**
 *   @file   ad400x.h
 *   @brief  Header file for ad400x Driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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

#ifndef SRC_AD400X_H_
#define SRC_AD400X_H_

#include "spi_engine.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD400X_READ_COMMAND	0x54
#define AD400X_WRITE_COMMAND	0x14
#define AD400X_RESERVED_MSK	0xE0

#define AD400X_TURBO_MODE(x)		(((x) & 0x1) << 1)
#define AD400X_HIGH_Z_MODE(x)		(((x) & 0x1) << 2)
#define AD400X_SPAN_COMPRESSION(x)	(((x) & 0x1) << 3)
#define AD400X_EN_STATUS_BITS(x)	(((x) & 0x1) << 4)

enum ad400x_supported_dev_ids {
	ID_AD4003,
	ID_AD4007,
	ID_AD4011,
	ID_AD4020,
};

extern const uint16_t ad400x_device_resol[];

struct ad400x_dev {
	/* SPI */
	spi_desc *spi_desc;
	/* Register access speed */
	uint32_t reg_access_speed;
	/* Device Settings */
	enum ad400x_supported_dev_ids dev_id;
};

struct ad400x_init_param {
	/* SPI */
	spi_init_param spi_init;
	/* Register access speed */
	uint32_t reg_access_speed;
	/* Device Settings */
	enum ad400x_supported_dev_ids dev_id;
	bool turbo_mode;
	bool high_z_mode;
	bool span_compression;
	bool en_status_bits;
};

int32_t ad400x_spi_reg_read(struct ad400x_dev *dev,
			    uint8_t *reg_data);
int32_t ad400x_spi_reg_write(struct ad400x_dev *dev,
			     uint8_t reg_data);
int32_t ad400x_init(struct ad400x_dev **device,
		    struct ad400x_init_param *init_param);
int32_t ad400x_remove(struct ad400x_dev *dev);
/* Execute a single conversion */
int32_t ad400x_spi_single_conversion(struct ad400x_dev *dev,
				     uint32_t *adc_data);

#endif /* SRC_AD400X_H_ */
