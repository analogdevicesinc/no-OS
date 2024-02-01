/***************************************************************************//**
 *   @file   max2201x.h
 *   @brief  Header file of MAX2201X Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef _MAX2201X_H
#define _MAX2201X_H

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"

/* GEN Registers */
#define MAX2201X_GEN_ID			0x00
#define MAX2201X_GEN_SERIAL_MSB		0x01
#define MAX2201X_GEN_SERIAL_LSB		0x02
#define MAX2201X_GEN_CNFG		0x03
#define MAX2201X_GEN_GPIO_CTRL		0x04
#define MAX2201X_GEN_GPIO_DATA		0x05
#define MAX2201X_GEN_GPI_INT		0x06
#define MAX2201X_GEN_GPI_INT_STA	0x07
#define MAX2201X_GEN_INT		0x08
#define MAX2201X_GEN_INTEN		0x09
#define MAX2201X_GEN_RST_CTRL		0x0A

/* AO Registers */
#define MAX2201X_AO_CMD			0x40
#define MAX2201X_AO_STA			0x41
#define MAX2201X_AO_CNFG		0x42
#define MAX2201X_AO_SLEW_RATE_CH1	0x43
#define MAX2201X_AO_SLEW_RATE_CH2	0x44
#define MAX2201X_AO_DATA_CH1		0x45
#define MAX2201X_AO_DATA_CH2		0x46
#define MAX2201X_AO_OFFSET_CORR_CH1	0x47
#define MAX2201X_AO_GAIN_CORR_CH1	0x48
#define MAX2201X_AO_OFFSET_CORR_CH2	0x49
#define MAX2201X_AO_GAIN_CORR_CH2	0x4A

#define MAX2201X_FRAME_SIZE		3

#define MAX2201X_ADRR_MASK		NO_OS_GENMASK(7, 1)
#define MAX2201X_RW_MASK		NO_OS_BIT(0)
#define MAX2201X_PAYLOAD_LSB_MASK	NO_OS_GENMASK(7, 0)
#define MAX2201X_PAYLOAD_MSB_MASK	NO_OS_GENMASK(15, 8)
#define MAX2201X_CRC_MASK		NO_OS_BIT(1)
#define MAX2201X_EXT_DAC_REF_MASK	NO_OS_BIT(0)
#define MAX2201X_GPIO_MASK(x)		NO_OS_BIT(8 + (x))
#define MAX2201X_GPIO_DIR_MASK(x)	NO_OS_BIT(x)
#define MAX2201X_GPIO_OUT_VAL_MASK(x)	NO_OS_BIT(8 + (x))
#define MAX2201X_GPIO_IN_VAL_MASK(x)	NO_OS_BIT(x)
#define MAX2201X_SR_EN_MASK		NO_OS_BIT(5)
#define MAX2201X_SR_SEL_MASK		NO_OS_BIT(4)
#define MAX2201X_SR_STEP_SIZE_MASK	NO_OS_GENMASK(3, 2)
#define MAX2201X_SR_UPDATE_RATE_MASK	NO_OS_GENMASK(1, 0)
#define MAX2201X_AO_EN_MASK(x)		NO_OS_BIT(x)
#define MAX2201X_AO_EN_MODE_MASK	NO_OS_BIT(2)
#define MAX2201X_AI_EN_MASK(x)		NO_OS_BIT(2 + (x))
#define MAX2201X_AI_EN_MODE_MASK	NO_OS_BIT(1)
#define MAX2201X_AO_MODE_MASK(x)	NO_OS_BIT(8 + (x))
#define MAX2201X_AO_MODE_MODE_MASK	NO_OS_BIT(0)
#define MAX2201X_OP_MODE_MASK(x)	(NO_OS_BIT(x) | NO_OS_BIT(2 + (x)) | NO_OS_BIT(8 + (x)))
#define MAX2201X_LD_CNFG_MASK(x)	NO_OS_BIT(14 + (x))
#define MAX2201X_CM_SENSE_MASK(x)	NO_OS_BIT(12 + (x))
#define MAX2201X_UNI_MASK(x)		NO_OS_BIT(10 + (x))
#define MAX2201X_OW_MASK(x)		NO_OS_BIT(4 + (x))
#define MAX2201X_CNFG_MASK(x)		(NO_OS_BIT(14 + (x)) | NO_OS_BIT(12 + (x)) | NO_OS_BIT(10 + (x)) | NO_OS_BIT(4 + (x)))
#define MAX2201X_COEFF_ONLY_MASK	NO_OS_GENMASK(15, 14)
#define MAX2201X_RST_MASK(x)		NO_OS_BIT(14 + (x))
#define MAX2201X_TIMEOUT_SEL_MASK	NO_OS_GENMASK(13, 10)
#define MAX2201X_TIMEOUT_CNFG_MASK	NO_OS_BIT(9)
#define MAX2201X_TIMEOUT_EN_MASK	NO_OS_BIT(8)
#define MAX2201X_TIMEOUT_MASK		NO_OS_GENMASK(13, 8)

#define MAX2201X_COEFF_ONLY_VAL		0x03

#define MAX2201X_GPIO_NUMBER		4
#define MAX2201X_EXTRA_GPIO_NUMBER	2

extern const struct no_os_gpio_platform_ops max2201x_gpio_ops;

enum max2201x_type {
	ID_MAX22017,
	ID_MAX22018
};

enum max2201x_op_mode {
	MAX2201X_HIGH_Z = 1,
	MAX2201X_INPUT_VOLTAGE_MODE,
	MAX2201X_INPUT_CURRENT_MODE,
	MAX2201X_OUTPUT_VOLTAGE_NO_MONITORRING,
	MAX2201X_OUTPUT_CURRENT_NO_MONITORRING,
	MAX2201X_OUTPUT_VOLTAGE_MONITORRING,
	MAX2201X_OUTPUT_CURRENT_MONITORRING,
};

enum max2201x_out_mode {
	MAX2201X_BIPOLAR,
	MAX2201X_UNIPOLAR,
};

enum max2201x_cm_sense {
	MAX2201X_50_OHM_RESISTOR,
	MAX2201X_250_OHM_RESISTOR
};

enum max2201x_timeout_sel {
	MAX2201X_TIMEOUT_100MS,
	MAX2201X_TIMEOUT_200MS,
	MAX2201X_TIMEOUT_300MS,
	MAX2201X_TIMEOUT_400MS,
	MAX2201X_TIMEOUT_500MS,
	MAX2201X_TIMEOUT_600MS,
	MAX2201X_TIMEOUT_700MS,
	MAX2201X_TIMEOUT_800MS,
	MAX2201X_TIMEOUT_900MS,
	MAX2201X_TIMEOUT_1000MS,
	MAX2201X_TIMEOUT_1100MS,
	MAX2201X_TIMEOUT_1200MS,
	MAX2201X_TIMEOUT_1300MS,
	MAX2201X_TIMEOUT_1400MS,
	MAX2201X_TIMEOUT_1500MS,
	MAX2201X_TIMEOUT_1600MS,
};

enum max2201x_sr_step_size {
	MAX2201X_STEP_SIZE_512BITS,
	MAX2201X_STEP_SIZE_1024BITS,
	MAX2201X_STEP_SIZE_4096BITS,
	MAX2201X_STEP_SIZE_16384BITS,
	MAX2201X_STEP_SIZE_26BITS,
	MAX2201X_STEP_SIZE_28BITS,
	MAX2201X_STEP_SIZE_31BITS,
	MAX2201X_STEP_SIZE_35BITS,
};

enum max2201x_sr_update_rate {
	MAX2201X_UPDATE_RATE_4KHZ_2KHZ,
	MAX2201X_UPDATE_RATE_64KHZ_32KHZ,
	MAX2201X_UPDATE_RATE_160KHZ_80KHZ,
	MAX2201X_UPDATE_RATE_200KHZ_100KHZ,
	MAX2201X_UPDATE_RATE_12KHZ_6KHZ,
	MAX2201X_UPDATE_RATE_15KHZ_7KHZ,
	MAX2201X_UPDATE_RATE_20KHZ_10KHZ,
	MAX2201X_UPDATE_RATE_32KHZ_16KHZ,
};

enum max2201x_channel {
	MAX2201X_CH1,
	MAX2201X_CH2
};

/**
 * @brief Structure holding the GPIO initialization parameter for the
 * 	  MAX2201X GPIO Controller
*/
struct max2201x_gpio_init_param {
	struct max2201x_desc *max2201x_desc;
};

/**
 * @brief Structure holding the MAX2201X initialization parameter
*/
struct max2201x_init_param {
	struct no_os_gpio_init_param *ldac_param;
	struct no_os_gpio_init_param *rstb_param;
	struct no_os_gpio_init_param *busy_param;
	struct no_os_gpio_init_param *fault_param;
	struct no_os_spi_init_param *comm_param;
	enum max2201x_type chip_id;
	bool crc_en;
	bool ext_dac_ref;
};

/**
 * @brief Structure holding the MAX2201X device descriptor
*/
struct max2201x_desc {
	struct no_os_gpio_desc *ldac_desc;
	struct no_os_gpio_desc *rstb_desc;
	struct no_os_gpio_desc *busy_desc;
	struct no_os_gpio_desc *fault_desc;
	struct no_os_spi_desc *comm_desc;
	enum max2201x_type chip_id;
	uint8_t buff[MAX2201X_FRAME_SIZE + 1];
	bool crc_en;
	bool uni;
};

/** Get GPIO descriptor for MAX2201X. */
int32_t max2201x_gpio_get(struct no_os_gpio_desc **,
			  const struct no_os_gpio_init_param *);

/** Free the resources allocated by no_os_gpio_get(). */
int32_t max2201x_gpio_remove(struct no_os_gpio_desc *);

/** Set GPIO direction as input. */
int32_t max2201x_gpio_direction_input(struct no_os_gpio_desc *);

/** Set GPIO direction as output. */
int32_t max2201x_gpio_direction_output(struct no_os_gpio_desc *, uint8_t);

/** Get GPIO direction for MAX2201X. */
int32_t max2201x_gpio_get_direction(struct no_os_gpio_desc *, uint8_t *);

/** Set GPIO value for MAX2201X. */
int32_t max2201x_gpio_set_value(struct no_os_gpio_desc *, uint8_t);

/** Get GPIO value for MAX2201x. */
int32_t max2201x_gpio_get_value(struct no_os_gpio_desc *, uint8_t *);

/** Set timeout settings. */
int max2201x_set_timeout(struct max2201x_desc *, bool, bool,
			 enum max2201x_timeout_sel);

/** Set data for specific channel. */
int max2201x_set_data(struct max2201x_desc *, enum max2201x_channel, int32_t);

/** Get data from specific channel. */
int max2201x_get_data(struct max2201x_desc *, enum max2201x_channel, int32_t *);

/** Set slew rate settings for specific channel. */
int max2201x_slew_rate(struct max2201x_desc *, enum max2201x_channel,
		       enum max2201x_sr_step_size, enum max2201x_sr_update_rate,
		       bool, bool);

/** Calibrate the offset of a specific channel. */
int max2201x_offset_calib(struct max2201x_desc *, enum max2201x_channel,
			  int16_t);

/** Adjust the gain of a specific channel. */
int max2201x_gain_calib(struct max2201x_desc *, enum max2201x_channel,
			uint16_t);

/** Set operation mode for specific channel. */
int max2201x_op_mode(struct max2201x_desc *, enum max2201x_channel,
		     enum max2201x_op_mode);

/** Set AO configuration settings for specific channel. */
int max2201x_config(struct max2201x_desc *, enum max2201x_channel,
		    enum max2201x_out_mode, enum max2201x_cm_sense, bool);

/** Soft-reset the MAX2201X. */
int max2201x_soft_reset(struct max2201x_desc *, enum max2201x_channel, bool);

/** Read register value from the MAX2201X. */
int max2201x_reg_read(struct max2201x_desc *, uint32_t, uint32_t *);

/** Write to a register from MAX2201X. */
int max2201x_reg_write(struct max2201x_desc *, uint32_t, uint32_t);

/** Update the value of a register from the MAX2201X. */
int max2201x_reg_update(struct max2201x_desc *, uint32_t, uint32_t, uint32_t);

/** Initialize the MAX2201X device descriptor. */
int max2201x_init(struct max2201x_desc **, struct max2201x_init_param *);

/** Free resources allocated by the init() function. */
int max2201x_remove(struct max2201x_desc *);

#endif /* _MAX2201X_H */
