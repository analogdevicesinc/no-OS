/***************************************************************************//**
 *   @file   max22017.h
 *   @brief  Header file of MAX22017 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _MAX22017_H
#define _MAX22017_H

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"

/* GEN Registers */
#define MAX22017_GEN_ID			0x00
#define MAX22017_GEN_SERIAL_MSB		0x01
#define MAX22017_GEN_SERIAL_LSB		0x02
#define MAX22017_GEN_CNFG		0x03
#define MAX22017_GEN_GPIO_CTRL		0x04
#define MAX22017_GEN_GPIO_DATA		0x05
#define MAX22017_GEN_GPI_INT		0x06
#define MAX22017_GEN_GPI_INT_STA	0x07
#define MAX22017_GEN_INT		0x08
#define MAX22017_GEN_INTEN		0x09
#define MAX22017_GEN_RST_CTRL		0x0A

/* AO Registers */
#define MAX22017_AO_CMD			0x40
#define MAX22017_AO_STA			0x41
#define MAX22017_AO_CNFG		0x42
#define MAX22017_AO_SLEW_RATE_CH1	0x43
#define MAX22017_AO_SLEW_RATE_CH2	0x44
#define MAX22017_AO_DATA_CH1		0x45
#define MAX22017_AO_DATA_CH2		0x46
#define MAX22017_AO_OFFSET_CORR_CH1	0x47
#define MAX22017_AO_GAIN_CORR_CH1	0x48
#define MAX22017_AO_OFFSET_CORR_CH2	0x49
#define MAX22017_AO_GAIN_CORR_CH2	0x4A

#define MAX22017_FRAME_SIZE		3

#define MAX22017_ADRR_MASK		NO_OS_GENMASK(7, 1)
#define MAX22017_RW_MASK		NO_OS_BIT(0)
#define MAX22017_PAYLOAD_LSB_MASK	NO_OS_GENMASK(7, 0)
#define MAX22017_PAYLOAD_MSB_MASK	NO_OS_GENMASK(15, 8)
#define MAX22017_CRC_MASK		NO_OS_BIT(1)
#define MAX22017_EXT_DAC_REF_MASK	NO_OS_BIT(0)
#define MAX22017_GPIO_MASK(x)		NO_OS_BIT(8 + (x))
#define MAX22017_GPIO_DIR_MASK(x)	NO_OS_BIT(x)
#define MAX22017_GPIO_OUT_VAL_MASK(x)	NO_OS_BIT(8 + (x))
#define MAX22017_GPIO_IN_VAL_MASK(x)	NO_OS_BIT(x)
#define MAX22017_SR_EN_MASK		NO_OS_BIT(5)
#define MAX22017_SR_SEL_MASK		NO_OS_BIT(4)
#define MAX22017_SR_STEP_SIZE_MASK	NO_OS_GENMASK(3, 2)
#define MAX22017_SR_UPDATE_RATE_MASK	NO_OS_GENMASK(1, 0)
#define MAX22017_AO_EN_MASK(x)		NO_OS_BIT(x)
#define MAX22017_AO_EN_MODE_MASK	NO_OS_BIT(2)
#define MAX22017_AI_EN_MASK(x)		NO_OS_BIT(2 + (x))
#define MAX22017_AI_EN_MODE_MASK	NO_OS_BIT(1)
#define MAX22017_AO_MODE_MASK(x)	NO_OS_BIT(8 + (x))
#define MAX22017_AO_MODE_MODE_MASK	NO_OS_BIT(0)
#define MAX22017_OP_MODE_MASK(x)	(NO_OS_BIT(x) | NO_OS_BIT(2 + (x)) | NO_OS_BIT(8 + (x)))
#define MAX22017_LD_CNFG_MASK(x)	NO_OS_BIT(14 + (x))
#define MAX22017_CM_SENSE_MASK(x)	NO_OS_BIT(12 + (x))
#define MAX22017_UNI_MASK(x)		NO_OS_BIT(10 + (x))
#define MAX22017_OW_MASK(x)		NO_OS_BIT(4 + (x))
#define MAX22017_CNFG_MASK(x)		(NO_OS_BIT(14 + (x)) | NO_OS_BIT(12 + (x)) | NO_OS_BIT(10 + (x)) | NO_OS_BIT(4 + (x)))
#define MAX22017_COEFF_ONLY_MASK	NO_OS_GENMASK(15, 14)
#define MAX22017_RST_MASK(x)		NO_OS_BIT(14 + (x))
#define MAX22017_TIMEOUT_SEL_MASK	NO_OS_GENMASK(13, 10)
#define MAX22017_TIMEOUT_CNFG_MASK	NO_OS_BIT(9)
#define MAX22017_TIMEOUT_EN_MASK	NO_OS_BIT(8)
#define MAX22017_TIMEOUT_MASK		NO_OS_GENMASK(13, 8)

#define MAX22017_COEFF_ONLY_VAL		0x03

#define MAX22017_GPIO_NUMBER		4
#define MAX22017_EXTRA_GPIO_NUMBER	2

extern const struct no_os_gpio_platform_ops max22017_gpio_ops;

enum max22017_op_mode {
	MAX22017_HIGH_Z = 1,
	MAX22017_INPUT_VOLTAGE_MODE,
	MAX22017_INPUT_CURRENT_MODE,
	MAX22017_OUTPUT_VOLTAGE_NO_MONITORRING,
	MAX22017_OUTPUT_CURRENT_NO_MONITORRING,
	MAX22017_OUTPUT_VOLTAGE_MONITORRING,
	MAX22017_OUTPUT_CURRENT_MONITORRING,
};

enum max22017_out_mode {
	MAX22017_BIPOLAR,
	MAX22017_UNIPOLAR,
};

enum max22017_cm_sense {
	MAX22017_50_OHM_RESISTOR,
	MAX22017_250_OHM_RESISTOR
};

enum max22017_timeout_sel {
	MAX22017_TIMEOUT_100MS,
	MAX22017_TIMEOUT_200MS,
	MAX22017_TIMEOUT_300MS,
	MAX22017_TIMEOUT_400MS,
	MAX22017_TIMEOUT_500MS,
	MAX22017_TIMEOUT_600MS,
	MAX22017_TIMEOUT_700MS,
	MAX22017_TIMEOUT_800MS,
	MAX22017_TIMEOUT_900MS,
	MAX22017_TIMEOUT_1000MS,
	MAX22017_TIMEOUT_1100MS,
	MAX22017_TIMEOUT_1200MS,
	MAX22017_TIMEOUT_1300MS,
	MAX22017_TIMEOUT_1400MS,
	MAX22017_TIMEOUT_1500MS,
	MAX22017_TIMEOUT_1600MS,
};

enum max22017_sr_step_size {
	MAX22017_STEP_SIZE_512BITS,
	MAX22017_STEP_SIZE_1024BITS,
	MAX22017_STEP_SIZE_4096BITS,
	MAX22017_STEP_SIZE_16384BITS,
	MAX22017_STEP_SIZE_26BITS,
	MAX22017_STEP_SIZE_28BITS,
	MAX22017_STEP_SIZE_31BITS,
	MAX22017_STEP_SIZE_35BITS,
};

enum max22017_sr_update_rate {
	MAX22017_UPDATE_RATE_4KHZ_2KHZ,
	MAX22017_UPDATE_RATE_64KHZ_32KHZ,
	MAX22017_UPDATE_RATE_160KHZ_80KHZ,
	MAX22017_UPDATE_RATE_200KHZ_100KHZ,
	MAX22017_UPDATE_RATE_12KHZ_6KHZ,
	MAX22017_UPDATE_RATE_15KHZ_7KHZ,
	MAX22017_UPDATE_RATE_20KHZ_10KHZ,
	MAX22017_UPDATE_RATE_32KHZ_16KHZ,
};

enum max22017_channel {
	MAX22017_CH1,
	MAX22017_CH2
};

/**
 * @brief Structure holding the GPIO initialization parameter for the
 * 	  MAX22017 GPIO Controller
*/
struct max22017_gpio_init_param {
	struct max22017_desc *max22017_desc;
};

/**
 * @brief Structure holding the MAX22017 initialization parameter
*/
struct max22017_init_param {
	struct no_os_gpio_init_param *ldac_param;
	struct no_os_gpio_init_param *rstb_param;
	struct no_os_gpio_init_param *busy_param;
	struct no_os_gpio_init_param *fault_param;
	struct no_os_spi_init_param *comm_param;
	bool crc_en;
	bool ext_dac_ref;
};

/**
 * @brief Structure holding the MAX22017 device descriptor
*/
struct max22017_desc {
	struct no_os_gpio_desc *ldac_desc;
	struct no_os_gpio_desc *rstb_desc;
	struct no_os_gpio_desc *busy_desc;
	struct no_os_gpio_desc *fault_desc;
	struct no_os_spi_desc *comm_desc;
	uint8_t buff[MAX22017_FRAME_SIZE + 1];
	bool crc_en;
	bool uni;
};

/** Get GPIO descriptor for MAX22017. */
int32_t max22017_gpio_get(struct no_os_gpio_desc **,
			  const struct no_os_gpio_init_param *);

/** Free the resources allocated by no_os_gpio_get(). */
int32_t max22017_gpio_remove(struct no_os_gpio_desc *);

/** Set GPIO direction as input. */
int32_t max22017_gpio_direction_input(struct no_os_gpio_desc *);

/** Set GPIO direction as output. */
int32_t max22017_gpio_direction_output(struct no_os_gpio_desc *, uint8_t);

/** Get GPIO direction for MAX22017. */
int32_t max22017_gpio_get_direction(struct no_os_gpio_desc *, uint8_t *);

/** Set GPIO value for MAX22017. */
int32_t max22017_gpio_set_value(struct no_os_gpio_desc *, uint8_t);

/** Get GPIO value for MAX22017. */
int32_t max22017_gpio_get_value(struct no_os_gpio_desc *, uint8_t *);

/** Set timeout settings. */
int max22017_set_timeout(struct max22017_desc *, bool, bool,
			 enum max22017_timeout_sel);

/** Set data for specific channel. */
int max22017_set_data(struct max22017_desc *, enum max22017_channel, int32_t);

/** Get data from specific channel. */
int max22017_get_data(struct max22017_desc *, enum max22017_channel, int32_t *);

/** Set slew rate settings for specific channel. */
int max22017_slew_rate(struct max22017_desc *, enum max22017_channel,
		       enum max22017_sr_step_size, enum max22017_sr_update_rate,
		       bool, bool);

/** Calibrate the offset of a specific channel. */
int max22017_offset_calib(struct max22017_desc *, enum max22017_channel,
			  int16_t);

/** Adjust the gain of a specific channel. */
int max22017_gain_calib(struct max22017_desc *, enum max22017_channel,
			uint16_t);

/** Set operation mode for specific channel. */
int max22017_op_mode(struct max22017_desc *, enum max22017_channel,
		     enum max22017_op_mode);

/** Set AO configuration settings for specific channel. */
int max22017_config(struct max22017_desc *, enum max22017_channel,
		    enum max22017_out_mode, enum max22017_cm_sense, bool);

/** Soft-reset the MAX22017. */
int max22017_soft_reset(struct max22017_desc *, enum max22017_channel, bool);

/** Read register value from the MAX22017. */
int max22017_reg_read(struct max22017_desc *, uint32_t, uint32_t *);

/** Write to a register from MAX22017. */
int max22017_reg_write(struct max22017_desc *, uint32_t, uint32_t);

/** Update the value of a register from the MAX22017. */
int max22017_reg_update(struct max22017_desc *, uint32_t, uint32_t, uint32_t);

/** Initialize the MAX22017 device descriptor. */
int max22017_init(struct max22017_desc **, struct max22017_init_param *);

/** Free resources allocated by the init() function. */
int max22017_remove(struct max22017_desc *);

#endif /* _MAX22017_H */
