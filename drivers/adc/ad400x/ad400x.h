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

#include <stdbool.h>

#if !defined(USE_STANDARD_SPI)
#include "spi_engine.h"
#include "clk_axi_clkgen.h"
#include "no_os_pwm.h"
#else
#include "no_os_spi.h"
#endif
#include "no_os_gpio.h"

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
	ID_AD4000,
	ID_AD4001,
	ID_AD4002,
	ID_AD4003,
	ID_AD4004,
	ID_AD4005,
	ID_AD4006,
	ID_AD4007,
	ID_AD4008,
	ID_AD4010,
	ID_AD4011,
	ID_AD4020,
	ID_AD4021,
	ID_AD4022,
	ID_ADAQ4003,
	ID_AD7690,
	ID_AD7691,
	ID_AD7693,
	ID_AD7942,
	ID_AD7944,
	ID_AD7946,
	ID_AD7980,
	ID_AD7982,
	ID_AD7983,
	ID_AD7984,
	ID_AD7985,
	ID_AD7986,
};

struct ad400x_dev_info {
	uint16_t resolution;
	char sign;
	uint16_t max_rate;
};

struct ad400x_dev {
	/* SPI */
	struct no_os_spi_desc *spi_desc;
	/* Clock gen for hdl design structure */
	struct axi_clkgen *clkgen;
	/* Trigger conversion PWM generator descriptor */
	struct no_os_pwm_desc *trigger_pwm_desc;
	/** Conversion Start GPIO descriptor. */
	struct no_os_gpio_desc *gpio_cnv;
	/* Register access speed */
	uint32_t reg_access_speed;
	/* Device Settings */
	enum ad400x_supported_dev_ids dev_id;
	/** SPI module offload init */
	struct spi_engine_offload_init_param *offload_init_param;
	/** device info */
	const struct ad400x_dev_info *dev_info;
	/** Invalidate the Data cache for the given address range */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
	/* enable offload */
	bool offload_enable;
};

struct ad400x_init_param {
	/* SPI */
	struct no_os_spi_init_param *spi_init;
	/* PWM generator init structure */
	struct no_os_pwm_init_param *trigger_pwm_init;
	/* Clock gen for hdl design init structure */
	struct axi_clkgen_init *clkgen_init;
	/* Clock generator rate */
	uint32_t axi_clkgen_rate;
	/** Conversion Start GPIO configuration. */
	struct no_os_gpio_init_param *gpio_cnv;
	/* Register access speed */
	uint32_t reg_access_speed;
	/* Device Settings */
	enum ad400x_supported_dev_ids dev_id;
	/** SPI module offload init */
	struct spi_engine_offload_init_param *offload_init_param;
	/** Invalidate the Data cache for the given address range */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
	/* buffer to store data samples */
	uint32_t *buffer;
	/* buffer size */
	uint32_t buffer_size;
	bool offload_enable;
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
/* read data samples */
int32_t ad400x_read_data(struct ad400x_dev *dev, uint32_t *buf,
			 uint16_t samples);
#endif /* SRC_AD400X_H_ */
