/***************************************************************************//**
 *   @file   ad7689.h
 *   @brief  Header file for the ad7689 driver
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#ifndef AD7689_H_
#define AD7689_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"

#define AD7689_CFG_CFG_MSK      NO_OS_BIT(13)
#define AD7689_CFG_INCC_MSK     NO_OS_GENMASK(12,10)
#define AD7689_CFG_INX_MSK      NO_OS_GENMASK(9,7)
#define AD7689_CFG_BW_MSK       NO_OS_BIT(6)
#define AD7689_CFG_REF_MSK      NO_OS_GENMASK(5,3)
#define AD7689_CFG_SEQ_MSK      NO_OS_GENMASK(2,1)
#define AD7689_CFG_RB_MSK       NO_OS_BIT(0)

/**
 * @enum ad7689_device_id
 * @brief Device ID definitions
 */
enum ad7689_device_id {
	/** 16-Bit, 8-Channel, 250 kSPS PulSAR ADC */
	ID_AD7689,
	/** 16-Bit, 4-Channel, 250 kSPS PulSAR ADC */
	ID_AD7682,
	/** 14-Bit, 8-Channel, 250 kSPS PulSAR ADC */
	ID_AD7949,
	/** 16-Bit, 8-Channel, 500 kSPS PulSAR ADC */
	ID_AD7699,
};

/**
 * @enum ad7689_incc
 * @brief Input channel configuration
 */
enum ad7689_incc {
	/** Bipolar differential pairs; INx− referenced to VREF/2 ± 0.1 V. */
	AD7689_BIPOLAR_DIFFERENTIAL_PAIRS = 0x1,
	/** Bipolar; INx referenced to COM = V REF /2 ± 0.1 V. */
	AD7689_BIPOLAR_COM,
	/** Temperature sensor. */
	AD7689_TEMPERATURE_SENSOR,
	/** Unipolar differential pairs; INx− referenced to GND ± 0.1 V. */
	AD7689_UNIPOLAR_DIFFERENTIAL_PAIRS = 0x5,
	/** Unipolar, INx referenced to COM = GND ± 0.1 V. */
	AD7689_UNIPOLAR_COM,
	/** Unipolar, INx referenced to GND. */
	AD7689_UNIPOLAR_GND
};

/**
 * @enum ad7689_bw
 * @brief Low-pass filter bandwidth selection
 */
enum ad7689_bw {
	/** 1⁄4 of BW, uses an additional series resistor to further bandwidth limit the noise. Maximum throughput must be reduced to 1⁄4. */
	AD7689_BW_QUARTER,
	/** Full bandwidth. */
	AD7689_BW_FULL
};

/**
 * @enum ad7689_ref
 * @brief Reference/buffer selection
 */
enum ad7689_ref {
	/** Internal reference and temperature sensor enabled. REF = 2.5 V buffered output. */
	AD7689_REF_INTERNAL_2p5V,
	/** Internal reference and temperature sensor enabled. REF = 4.096 V buffered output. */
	AD7689_REF_INTERNAL_4p096V,
	/** Use external reference. Temperature sensor enabled. Internal buffer disabled. */
	AD7689_REF_EXTERNAL_TEMP,
	/** Use external reference. Internal buffer and temperature sensor enabled. */
	AD7689_REF_EXTERNAL_TEMP_IBUF,
	/** Use external reference. Internal reference, internal buffer, and temperature sensor disabled. */
	AD7689_REF_EXTERNAL = 0x6,
	/** Use external reference. Internal buffer enabled. Internal reference and temperature sensor disabled. */
	AD7689_REF_IBUF
};

/**
 * @enum ad7689_seq
 * @brief Channel sequencer configuration
 */
enum ad7689_seq {
	/** Disable sequencer. */
	AD7689_SEQ_DISABLE,
	/** Update configuration during sequence. */
	AD7689_SEQ_UPDATE_CFG,
	/** Scan IN0 to INX, then temperature. */
	AD7689_SEQ_SCAN_ALL_THEN_TEMP,
	/** Scan IN0 to INX. */
	AD7689_SEQ_SCAN_ALL
};

/**
 * @struct ad7689_config
 * @brief AD7689 configuration
 */
struct ad7689_config {
	/** Input channel configuration */
	enum ad7689_incc incc;
	/** INX channel selection (sequencer iterates from IN0 to INX) */
	uint8_t inx;
	/** Low-pass filter bandwidth selection */
	enum ad7689_bw bw;
	/**  Reference/buffer selection */
	enum ad7689_ref ref;
	/** Channel sequencer configuration */
	enum ad7689_seq seq;
	/** Read back the CFG register */
	bool rb;
};

struct ad7689_init_param {
	/** Device ID */
	enum ad7689_device_id id;
	/** ADC specific parameters */
	struct ad7689_config config;
	/** SPI initialization parameters */
	struct no_os_spi_init_param spi_init;
};

struct ad7689_dev {
	/** Device name string */
	const char *name;
	/** Device ID */
	enum ad7689_device_id id;
	/** AD7689 configs (configs[1] - in use, configs[0] - will be in use during next transaction) */
	struct ad7689_config configs[2];
	/** SPI descriptor*/
	struct no_os_spi_desc *spi_desc;
};

int32_t ad7689_init(struct ad7689_dev **dev,
		    struct ad7689_init_param *init_param);
int32_t ad7689_write_config(struct ad7689_dev *dev,
			    struct ad7689_config *config);
int32_t ad7689_read_config(struct ad7689_dev *dev,
			   struct ad7689_config *config);
int32_t ad7689_read(struct ad7689_dev *dev, uint16_t *data,
		    uint32_t nb_samples);
int32_t ad7689_remove(struct ad7689_dev *dev);

#endif
