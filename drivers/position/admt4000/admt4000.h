/***************************************************************************//**
 *   @file   admt4000.h
 *   @brief  Header Implementation of ADMT4000 Driver.
 *   @author Marc Sosa (marcpaolo.sosa@analog.com)
 *   @author Jose Ramon San Buenaventura (jose.sanbuenaventura@analog.com)
 *   @author Louijie Compo  (louijie.compo@analog.com)
 *******************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 ******************************************************************************/

#ifndef __ADMT4000_H__
#define __ADMT4000_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_util.h"
#include "no_os_units.h"

/* ADMT4000 Registers */
#define ADMT4000_REG_RST		0x00

/* ADMT4000 Agnostic Page Registers */
#define ADMT4000_AGP_REG_CNVPAGE	0x01
#define ADMT4000_AGP_REG_ABSANGLE	0x03
#define ADMT4000_AGP_REG_DGIO		0x04
#define ADMT4000_AGP_REG_ANGLE		0x05
#define ADMT4000_AGP_REG_FAULT		0x06

/* ADMT 0x00 Page Registers */
#define ADMT4000_00_REG_SINE		0x10
#define ADMT4000_00_REG_COSINE		0x11
#define ADMT4000_00_REG_RADIUS		0x18
#define ADMT4000_00_REG_TMP		0x20

/* ADMT 0x02 Page Registers */
#define ADMT4000_02_REG_GENERAL		0x10 /* ECC0 */
#define ADMT4000_02_REG_DIGIOEN		0x12 /* ECC0 */
#define ADMT4000_02_REG_ANGLECK		0x13 /* ECC0 */
#define ADMT4000_02_REG_CNVCNT		0x14
#define ADMT4000_02_REG_H1MAG		0x15 /* ECC0 */
#define ADMT4000_02_REG_H1PH		0x16 /* ECC0 */
#define ADMT4000_02_REG_H2MAG		0x17 /* ECC0 */
#define ADMT4000_02_REG_H2PH		0x18 /* ECC0 */
#define ADMT4000_02_REG_H3MAG		0x19 /* ECC1/0 */
#define ADMT4000_02_REG_H3PH		0x1A /* ECC1 */
#define ADMT4000_02_REG_H8MAG		0x1B /* ECC1 */
#define ADMT4000_02_REG_H8PH		0x1C /* ECC1 */
#define ADMT4000_02_REG_ECCEDC		0x1D
#define ADMT4000_02_REG_UNIQD0		0x1E
#define ADMT4000_02_REG_UNIQD1		0x1F
#define ADMT4000_02_REG_UNIQD2		0x20
#define ADMT4000_02_REG_UNIQD3		0x21
#define ADMT4000_02_REG_ECCDIS		0x23

/* ADMT4000 Other Macros, Masks etc. */
#define ADMT4000_RW_MASK		NO_OS_GENMASK(5, 0)
#define ADMT4000_WR_EN			NO_OS_BIT(6)
#define ADMT4000_FAULT_MASK		NO_OS_BIT(7)
#define ADMT4000_LIFE_CTR		NO_OS_GENMASK(6, 5)
#define ADMT4000_RCV_CRC		NO_OS_GENMASK(4, 0)

#define ADMT4000_MIN_PAGE		0x00
#define ADMT4000_MAX_PAGE		0x02

#define ADMT4000_DIGIO3FNC_MASK		NO_OS_BIT(3)
#define ADMT4000_DIGIO0FNC_MASK		NO_OS_BIT(0)

/* Upper and Lower Byte Masking */
#define ADMT4000_LOW_BYTE		NO_OS_GENMASK(7, 0)
#define ADMT4000_HI_BYTE		NO_OS_GENMASK(15, 8)

/* Register 01 */
#define ADMT4000_CNV_EDGE_MASK		NO_OS_GENMASK(15, 14)
#define ADMT4000_PAGE_MASK		NO_OS_GENMASK(4, 0)
#define ADMT4000_FALLING_EDGE		0x00
#define ADMT4000_RISING_EDGE		0x3

/* Register 03 */
#define ADMT4000_ABS_ANGLE_MASK		NO_OS_GENMASK(15, 0)
#define ADMT4000_TURN_CNT_MASK		NO_OS_GENMASK(15, 10)
#define ADMT4000_QUARTER_TURN_CNT_MASK	NO_OS_GENMASK(15, 8)
#define ADMT4000_ABS_ANGLE_ANGLE_MASK	NO_OS_GENMASK(9, 0)

/* Invalid turn count indicator (0x53 = 83 decimal)
 * Values above ADMT4000_QUARTER_TURNS_MAX (215) indicate invalid state */
#define ADMT4000_INVALID_TURN		0x53

/* Register 04*/
#define ADMT4000_MAX_GPIO_INDEX		5
#define ADMT4000_GPIO_LOGIC(x)		NO_OS_BIT(x)

/* Register 05 and 08 */
#define ADMT4000_ANGLE_MASK		NO_OS_GENMASK(15, 4)

/* Register 06 */
#define ADMT4000_ALL_FAULTS		NO_OS_GENMASK(15, 0)
#define ADMT4000_AGP_INDIV_FAULT(x)	NO_OS_BIT((x))

/* Register 0x08/10/11/12/13/18, page 0 */
#define ADMT4000_ANGLE_STAT_MASK	NO_OS_BIT(0)

/* Register 0x10/11/12/13, page 0 */
#define ADMT4000_RAW_ANGLE_MASK		NO_OS_GENMASK(15, 2)
#define ADMT4000_RAW_COSINE_MASK	NO_OS_GENMASK(15, 2)
#define ADMT4000_RAW_SINE_MASK		NO_OS_GENMASK(15, 2)
#define ADMT4000_NEW_DATA_MASK		NO_OS_BIT(0)

/* Register 18, page 0 */
#define ADMT4000_RADIUS_MASK		NO_OS_GENMASK(15, 1)

/* Register 0x20, page 0 */
#define ADMT4000_TEMP_MASK		NO_OS_GENMASK(15, 4)

/* Register 0x10, page 2 */
#define ADMT4000_STORAGE_BIT7		NO_OS_BIT(7)
#define ADMT4000_STORAGE_BIT6		NO_OS_BIT(6)
#define ADMT4000_STORAGE_BIT5_3		NO_OS_GENMASK(5, 3)
#define ADMT4000_STORAGE_BIT2_0		NO_OS_GENMASK(2, 0)
#define ADMT4000_STORAGE_MASK_BIT7	NO_OS_BIT(15)
#define ADMT4000_STORAGE_MASK_BIT6	NO_OS_BIT(11)
#define ADMT4000_STORAGE_MASK_BIT5_3		NO_OS_GENMASK(8, 6)
#define ADMT4000_STORAGE_MASK_BIT2_0		NO_OS_GENMASK(3, 1)
#define ADMT4000_STORAGE_MASK_FULL	ADMT4000_STORAGE_MASK_BIT7 | \
					ADMT4000_STORAGE_MASK_BIT6 | \
					ADMT4000_STORAGE_MASK_BIT5_3 | \
					ADMT4000_STORAGE_MASK_BIT2_0

#define ADMT4000_CONV_SYNC_MODE_MASK	NO_OS_GENMASK(14, 13)
#define ADMT4000_ANGL_FILT_MASK		NO_OS_BIT(12)
#define ADMT4000_H8_CTRL_MASK		NO_OS_BIT(10)
#define ADMT4000_CNV_MODE_MASK		NO_OS_BIT(0)

/* Register 0x12, page 2 */
#define ADMT4000_DIG_IO_EN(x)		NO_OS_BIT(8 + (x))
#define ADMT4000_GPIO_FUNC(x)		NO_OS_BIT(x)

/* Register 0x14, page 2 */
#define ADMT4000_CNV_CTR_MASK		NO_OS_GENMASK(7, 0)

/* Register 0x15 / 0x18, page 2 */
#define ADMT4000_H_11BIT_MAG_MASK	NO_OS_GENMASK(10, 0)
#define ADMT4000_11BIT_MAX		2047
#define ADMT4000_H_12BIT_PHA_MASK	NO_OS_GENMASK(11, 0)
#define ADMT4000_12BIT_MAX		4095

/* Register 0x1A / 0x1B, page 2 */
#define ADMT4000_H_8BIT_MAG_MASK	NO_OS_GENMASK(7, 0)
#define ADMT4000_8BIT_MAX		127

/* Register 0x1D, page 2 */
#define ADMT4000_ECC_CFG1		NO_OS_GENMASK(15, 8)
#define ADMT4000_ECC_CFG0		NO_OS_GENMASK(7, 0)

/* Register 0x1E 0x1F 0x20 0x21 , page 2 */
#define ADMT4000_ID0_MASK		NO_OS_GENMASK(14, 0)
#define ADMT4000_ID_PROD_MASK		NO_OS_GENMASK(10, 8)
#define ADMT4000_ID_SUPPLY_MASK		NO_OS_GENMASK(7, 6)
#define ADMT4000_ID_ASIL_MASK		NO_OS_GENMASK(5, 3)
#define ADMT4000_ID_SIL_REV_MASK	NO_OS_GENMASK(2, 0)

/* Register 0x23, page 2 */
#define ADMT4000_ECC_EN_COMM		0x0000
#define ADMT4000_ECC_DIS_COMM		0x4D54

/* Angle to degree conversion */
#define ADMT4000_FULL_TURN_DEGREES	360
#define ADMT4000_ABS_ANGLE_RES		1024
#define ADMT4000_ANGLE_RES		4096

/* Turn Count Conversion */
#define ADMT4000_TURN_CNT_THRES		0x35
#define ADMT4000_TURN_CNT_TWOS		64
#define ADMT4000_QUARTER_TURNS_MAX	215
#define ADMT4000_QUARTER_TURNS_RES	256

/* Temperature Conversion */
#define ADMT4000_TEMP_OFFSET		-1168
#define ADMT4000_TEMP_SCALE_MILLIS	15660

/* (0.000924 * 1000000) nV/V */
#define ADMT4000_RADIUS_RES		924
/* *(0.005493/0.6072) hmag resolution/cordic = 0.009046 */
#define ADMT4000_HMAG_SCALER		0.009046f
/* (0.087891) hpha resolution */
#define ADMT4000_HPHA_SCALER		0.087891f

/* Timing delays */
/* GPIO settle time in milliseconds */
#define ADMT4000_GPIO_SETTLE_DELAY_MS	10
/* CNV pulse width in microseconds */
#define ADMT4000_CNV_PULSE_WIDTH_US	1
/* Conversion delay in milliseconds */
#define ADMT4000_CONVERSION_DELAY_MS	20
/* Device startup time after reset in milliseconds */
#define ADMT4000_STARTUP_TIME_MS	10
/* Reset hold time in milliseconds */
#define ADMT4000_RESET_HOLD_TIME_MS	1

/* Maximum data bit length for CRC */
#define ADMT4000_MAX_CRC_BIT_LEN	25

enum admt4000_faults {
	ADMT4000_FAULT_VDD_UV = 0,
	ADMT4000_FAULT_VDD_OV,
	ADMT4000_FAULT_VDRIVE_UV,
	ADMT4000_FAULT_VDRIVE_OV,
	ADMT4000_FAULT_NVM_CRC = 5,
	ADMT4000_FAULT_ECC_DOUBLE = 7,
	ADMT4000_FAULT_GMR_REF_RES = 9,
	ADMT4000_FAULT_TURN_CTRS = 13,
	ADMT4000_FAULT_AMR_RAD_AMP,
};

enum admt4000_vdd {
	ADMT4000_3P3V,
	ADMT4000_5V,
};

enum admt4000_conv_sync_mode {
	ADMT4000_SEQ_CTRL = 0,
	ADMT4000_START_EDGE = 3,
};

enum admt4000_harmonic_corr_src {
	ADMT4000_FACTORY = 0,
	ADMT4000_USER = 1,
};

enum admt4000_conversion_mode {
	ADMT4000_CONTINUOUS = 0,
	ADMT4000_ONE_SHOT = 1,
};

/**
 * @struct admt4000_harmonic_coeff
 * @brief Harmonic calibration coefficient
 */
struct admt4000_harmonic_coeff {
	/** Raw magnitude coefficient */
	uint16_t magnitude_raw;
	/** Raw phase coefficient */
	uint16_t phase_raw;
};

/**
 * @struct admt4000_harmonics_calib
 * @brief Harmonic calibration data for all harmonics
 */
struct admt4000_harmonics_calib {
	/** Harmonic 1 coefficients */
	struct admt4000_harmonic_coeff h1;
	/** Harmonic 2 coefficients */
	struct admt4000_harmonic_coeff h2;
	/** Harmonic 3 coefficients */
	struct admt4000_harmonic_coeff h3;
	/** Harmonic 8 coefficients */
	struct admt4000_harmonic_coeff h8;
};

/**
 * @struct admt4000_gpio
 * @brief GPIO state and configuration
 */
struct admt4000_gpio {
	/** Current logic level */
	bool logic_state;
	/** True if GPIO is in alternate function mode */
	bool alt_mode;
	/** True if GPIO is configured as output */
	bool output_en;
};

/**
 * @struct admt4000_gpio_init_param
 * @brief GPIO initialization parameters
 */
struct admt4000_gpio_init_param {
	/** True = alternate function, False = normal GPIO */
	bool alt_mode;
	/** Direction: True = output, False = input */
	bool output_en;
	/** Initial output state: True = high, False = low */
	bool init_state;
};

/**
 * @struct admt4000_init_param
 * @brief ADMT4000 initialization parameters
 */
struct admt4000_init_param {
	/** SPI communication parameters (chip select, max speed, mode, etc.) */
	struct no_os_spi_init_param spi_init_param;

	/** Reset GPIO pin configuration (active low hardware reset) */
	struct no_os_gpio_init_param gpio_reset_ip;

	/** CNV GPIO pin configuration (conversion trigger, input to device) */
	struct no_os_gpio_init_param gpio_cnv_ip;

	/** ACALC GPIO pin configuration (angle calculation indicator, output from device) */
	struct no_os_gpio_init_param gpio_acalc_ip;

	/** FAULT GPIO pin configuration (fault indicator, output from device) */
	struct no_os_gpio_init_param gpio_fault_ip;

	/** GPIO configuration for all 6 device GPIOs (GPIO0-GPIO5)
	 * @details Each GPIO can be configured as normal I/O or alternate function:
	 * - GPIO0 alt function: BUSY (output only)
	 * - GPIO1 alt function: CNV (active low, input only)
	 * - GPIO2 alt function: Reserved (must be normal GPIO)
	 * - GPIO3 alt function: ACALC (output only)
	 * - GPIO4 alt function: FAULT (output only, on some variants only)
	 * - GPIO5 alt function: BOOTLOAD (output only)
	 */
	struct admt4000_gpio_init_param admt4000_gpio_ip[6];
};

/**
 * @struct admt4000_dev
 * @brief ADMT4000 device descriptor
 */
struct admt4000_dev {
	/** SPI communication descriptor for register access */
	struct no_os_spi_desc *spi_desc;
	/** Copy of initialization parameters for reference and reconfiguration */
	struct admt4000_init_param default_config;
	/** Reset GPIO descriptor (active low hardware reset control) */
	struct no_os_gpio_desc *gpio_reset_desc;
	/** CNV GPIO descriptor (triggers angle/position conversion) */
	struct no_os_gpio_desc *gpio_cnv_desc;
	/** ACALC GPIO descriptor (monitors angle calculation status) */
	struct no_os_gpio_desc *gpio_acalc_desc;
	/** FAULT GPIO descriptor (monitors device fault status) */
	struct no_os_gpio_desc *gpio_fault_desc;

	/** Current register page (0=measurement, 2=configuration).
	 *  Cached to minimize SPI transactions when accessing same page */
	uint8_t current_page;
	/** Page cache validity flag. True when current_page reflects actual
	 *  hardware state. Invalidated after reset or page-affecting operations */
	bool page_cache_valid;

	/** GPIO state and configuration for all 6 device GPIOs.
	 *  Tracks logic level, direction, and alternate function mode */
	struct admt4000_gpio gpios[6];

	/** Conversion mode tracker. True for one-shot mode, false for continuous */
	bool is_one_shot;
};

/** Convert raw angle value to scaled degrees */
int admt4000_angle_raw_to_scaled(uint16_t raw_angle, float *scaled_angle);

/** Convert raw harmonic coefficients to scaled format */
int admt4000_harmonic_raw_to_scaled(const struct admt4000_harmonic_coeff
				    *raw_coeff,
				    float *scaled_mag, float *scaled_phase);

/** Convert scaled harmonic values to raw coefficients */
int admt4000_harmonic_scaled_to_raw(uint32_t scaled_mag, uint32_t scaled_phase,
				    struct admt4000_harmonic_coeff *raw_coeff);

/** Reinitialize device using stored configuration */
int admt4000_reinitialize(struct admt4000_dev *device);

/** Perform hardware reset via reset pin */
int admt4000_hard_reset(struct admt4000_dev *device);

/** Initialize the ADMT4000 device */
int admt4000_init(struct admt4000_dev **device,
		  struct admt4000_init_param *init_param);

/** Free resources allocated by admt4000_init() */
int admt4000_remove(struct admt4000_dev *device);

/** Read from an ADMT4000 register */
int admt4000_reg_read(struct admt4000_dev *device, uint8_t reg_addr,
		      uint16_t *reg_data, uint8_t *verif);

/** Write to an ADMT4000 register */
int admt4000_reg_write(struct admt4000_dev *device, uint8_t reg_addr,
		       uint16_t reg_data);

/** Read-modify-write an ADMT4000 register field */
int admt4000_reg_update(struct admt4000_dev *device, uint8_t reg_addr,
			uint16_t update_mask, uint16_t update_val);

/** Toggle the CNV pin to trigger conversion */
int admt4000_toggle_cnv(struct admt4000_dev *device);

/** Get raw angle data from ABSANGLE and ANGLE registers */
int admt4000_raw_angle_read(struct admt4000_dev *device, uint16_t *angle_data);

/** Get raw turns and angle data */
int admt4000_get_raw_turns_and_angle(struct admt4000_dev *device,
				     uint8_t *turns,
				     uint16_t *angle);

/** Convert raw quarter turn count to signed value */
int admt4000_quarter_turns_cnt(uint8_t raw_turns, int16_t *quarter_turns);

/** Get GPIO logic level */
int admt4000_get_gpio(struct admt4000_dev *device, uint8_t gpio, bool *logic);

/** Set GPIO logic level */
int admt4000_set_gpio(struct admt4000_dev *device, uint8_t gpio, bool logic);

/** Clear all fault flags */
int admt4000_clear_all_faults(struct admt4000_dev *device);

/** Get fault register value */
int admt4000_get_faults(struct admt4000_dev *device, uint16_t *faults);

/** Get radius raw data */
int admt4000_get_radius(struct admt4000_dev *device, uint16_t *radius,
			bool *is_new_data);

/** Convert raw radius value to scaled mV/V */
int admt4000_radius_raw_to_scaled(uint16_t raw_radius, float *radius);

/** Get raw temperature value */
int admt4000_get_temp(struct admt4000_dev *device, uint16_t *temp);

/** Convert raw temperature to degrees Celsius */
int admt4000_temp_raw_to_scaled(uint16_t temp_raw, float *temp);

/** Get storage configuration bits */
int admt4000_get_storage_config(struct admt4000_dev *device, uint8_t *storage);

/** Set storage configuration bits */
int admt4000_set_storage_config(struct admt4000_dev *device, uint8_t storage);

/** Get conversion synchronization mode */
int admt4000_get_conv_sync_mode(struct admt4000_dev *device,
				enum admt4000_conv_sync_mode *mode);

/** Set conversion synchronization mode */
int admt4000_set_conv_sync_mode(struct admt4000_dev *device,
				enum admt4000_conv_sync_mode mode);

/** Get angle filter status */
int admt4000_get_angle_filt(struct admt4000_dev *device, bool *is_filtered);

/** Set angle filter enable */
int admt4000_set_angle_filt(struct admt4000_dev *device, bool is_filtered);

/** Get H8 harmonic coefficient source */
int admt4000_get_h8_ctrl(struct admt4000_dev *device,
			 enum admt4000_harmonic_corr_src *source);

/** Set H8 harmonic coefficient source */
int admt4000_set_h8_ctrl(struct admt4000_dev *device,
			 enum admt4000_harmonic_corr_src source);

/** Get conversion mode */
int admt4000_get_conv_mode(struct admt4000_dev *device, bool *is_one_shot);

/** Set conversion mode */
int admt4000_set_conv_mode(struct admt4000_dev *device, bool is_one_shot);

/** Configure GPIO mode, direction, and initial state */
int admt4000_gpio_config(struct admt4000_dev *device, uint8_t gpio,
			 bool alt_mode, bool output_en, bool init_state);

/** Get GPIO function configuration */
int admt4000_get_gpio_func(struct admt4000_dev *device, uint8_t gpio,
			   bool *alt_mode);


/** Get conversion count value */
int admt4000_get_cnv_cnt(struct admt4000_dev *device, uint8_t *cnt);

/** Read device ID register */
int admt4000_get_id(struct admt4000_dev *device, uint8_t id_num, uint16_t *id);


/** Get cosine value */
int admt4000_get_cos(struct admt4000_dev *device, int16_t *val,
		     bool *is_new_data);

/** Get sine value */
int admt4000_get_sin(struct admt4000_dev *device, int16_t *val,
		     bool *is_new_data);

/** Get harmonic calibration coefficients */
int admt4000_get_harmonic(struct admt4000_dev *device, uint8_t harmonic,
			  struct admt4000_harmonic_coeff *config);

/** Set harmonic calibration coefficients */
int admt4000_set_harmonic(struct admt4000_dev *device, uint8_t harmonic,
			  const struct admt4000_harmonic_coeff *config);

/** Get all harmonic calibration data */
int admt4000_get_harmonics_calib(struct admt4000_dev *device,
				 struct admt4000_harmonics_calib *calib);

/** Set all harmonic calibration data */
int admt4000_set_harmonics_calib(struct admt4000_dev *device,
				 const struct admt4000_harmonics_calib *calib);

#ifdef __cplusplus
}
#endif

#endif  /* __ADMT4000_H__ */
