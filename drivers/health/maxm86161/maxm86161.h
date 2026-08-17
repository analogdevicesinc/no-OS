/***************************************************************************//**
 *   @file   maxm86161.h
 *   @brief  Header file for MAXM86161 optical pulse oximeter driver.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
********************************************************************************
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef __MAXM86161_H__
#define __MAXM86161_H__

#include <stdint.h>
#include <stdbool.h>
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_util.h"

#define MAXM86161_I2C_ADDR		0x62
#define MAXM86161_PART_ID_VAL		0x36
#define MAXM86161_FIFO_DEPTH		128
#define MAXM86161_FIFO_DATA_SIZE	3
#define MAXM86161_NUM_LED_SEQ		6
#define MAXM86161_NUM_LEDS		3
#define MAXM86161_RESET_DELAY_MS	10

/* Status Registers */
#define MAXM86161_REG_INT_STATUS_1	0x00
#define  MAXM86161_INT1_A_FULL		NO_OS_BIT(7)
#define  MAXM86161_INT1_DATA_RDY	NO_OS_BIT(6)
#define  MAXM86161_INT1_ALC_OVF		NO_OS_BIT(5)
#define  MAXM86161_INT1_PROX_INT	NO_OS_BIT(4)
#define  MAXM86161_INT1_LED_COMPB	NO_OS_BIT(3)
#define  MAXM86161_INT1_DIE_TEMP_RDY	NO_OS_BIT(2)
#define  MAXM86161_INT1_PWR_RDY		NO_OS_BIT(0)

#define MAXM86161_REG_INT_STATUS_2	0x01
#define  MAXM86161_INT2_SHA_DONE	NO_OS_BIT(0)

/* Interrupt Enable Registers */
#define MAXM86161_REG_INT_EN_1		0x02
#define  MAXM86161_INT1_A_FULL_EN	NO_OS_BIT(7)
#define  MAXM86161_INT1_DATA_RDY_EN	NO_OS_BIT(6)
#define  MAXM86161_INT1_ALC_OVF_EN	NO_OS_BIT(5)
#define  MAXM86161_INT1_PROX_INT_EN	NO_OS_BIT(4)
#define  MAXM86161_INT1_LED_COMPB_EN	NO_OS_BIT(3)
#define  MAXM86161_INT1_DIE_TEMP_RDY_EN	NO_OS_BIT(2)

#define MAXM86161_REG_INT_EN_2		0x03
#define  MAXM86161_INT2_SHA_DONE_EN	NO_OS_BIT(0)

/* FIFO Registers */
#define MAXM86161_REG_FIFO_WR_PTR	0x04
#define MAXM86161_REG_FIFO_RD_PTR	0x05
#define MAXM86161_REG_OVF_COUNTER	0x06
#define MAXM86161_REG_FIFO_DATA_COUNT	0x07
#define MAXM86161_REG_FIFO_DATA		0x08

#define MAXM86161_REG_FIFO_CFG_1	0x09
#define  MAXM86161_FIFO_A_FULL_MSK	NO_OS_GENMASK(6, 0)

#define MAXM86161_REG_FIFO_CFG_2	 0x0A
#define  MAXM86161_FIFO_CFG2_FLUSH	 NO_OS_BIT(4)
#define  MAXM86161_FIFO_CFG2_STAT_CLR	 NO_OS_BIT(3)
#define  MAXM86161_FIFO_CFG2_A_FULL_TYPE NO_OS_BIT(2)
#define  MAXM86161_FIFO_CFG2_FIFO_RO	 NO_OS_BIT(1)

/* System Control */
#define MAXM86161_REG_SYS_CTRL		0x0D
#define  MAXM86161_SYS_CTRL_SINGLE_PPG	NO_OS_BIT(3)
#define  MAXM86161_SYS_CTRL_LP_MODE	NO_OS_BIT(2)
#define  MAXM86161_SYS_CTRL_SHDN	NO_OS_BIT(1)
#define  MAXM86161_SYS_CTRL_RESET	NO_OS_BIT(0)

/* PPG Configuration */
#define MAXM86161_REG_PPG_SYNC_CTRL	  0x10
#define  MAXM86161_PPG_SYNC_TIMESTAMP_EN  NO_OS_BIT(7)
#define  MAXM86161_PPG_SYNC_DAC_CODE_TAG  NO_OS_BIT(6)
#define  MAXM86161_PPG_SYNC_SW_FORCE	  NO_OS_BIT(4)
#define  MAXM86161_PPG_SYNC_GPIO_CTRL_MSK NO_OS_GENMASK(3, 0)

#define MAXM86161_REG_PPG_CFG_1		 0x11
#define  MAXM86161_PPG_CFG1_ALC_DISABLE	 NO_OS_BIT(7)
#define  MAXM86161_PPG_CFG1_ADD_OFFSET	 NO_OS_BIT(6)
#define  MAXM86161_PPG_CFG1_ADC_RGE_MSK  NO_OS_GENMASK(3, 2)
#define  MAXM86161_PPG_CFG1_PPG_TINT_MSK NO_OS_GENMASK(1, 0)

#define MAXM86161_REG_PPG_CFG_2		0x12
#define  MAXM86161_PPG_CFG2_PPG_SR_MSK	NO_OS_GENMASK(7, 3)
#define  MAXM86161_PPG_CFG2_SMP_AVE_MSK	NO_OS_GENMASK(2, 0)

#define MAXM86161_REG_PPG_CFG_3		   0x13
#define  MAXM86161_PPG_CFG3_LED_SETLNG_MSK NO_OS_GENMASK(7, 6)
#define  MAXM86161_PPG_CFG3_DIG_FILT_MSK   NO_OS_BIT(5)
#define  MAXM86161_PPG_CFG3_BURST_RATE_MSK NO_OS_GENMASK(2, 1)
#define  MAXM86161_PPG_CFG3_BURST_EN	   NO_OS_BIT(0)

/* Proximity Interrupt Threshold */
#define MAXM86161_REG_PROX_INT_THRESH	0x14

/* Photo Diode Bias */
#define MAXM86161_REG_PD_BIAS		0x15
#define  MAXM86161_PD_BIAS_MSK		NO_OS_GENMASK(2, 0)

/* Picket Fence */
#define MAXM86161_REG_PICKET_FENCE	0x16
#define  MAXM86161_PF_ENABLE		NO_OS_BIT(7)
#define  MAXM86161_PF_ORDER		NO_OS_BIT(6)
#define  MAXM86161_PF_IIR_TC_MSK	NO_OS_GENMASK(5, 4)
#define  MAXM86161_PF_IIR_INIT_MSK	NO_OS_GENMASK(3, 2)
#define  MAXM86161_PF_THRESH_SIGMA_MSK	NO_OS_GENMASK(1, 0)

/* LED Sequence Control */
#define MAXM86161_REG_LED_SEQ_1		0x20
#define MAXM86161_REG_LED_SEQ_2		0x21
#define MAXM86161_REG_LED_SEQ_3		0x22
#define  MAXM86161_LED_SEQ_ODD_MSK	NO_OS_GENMASK(3, 0)
#define  MAXM86161_LED_SEQ_EVEN_MSK	NO_OS_GENMASK(7, 4)

/* LED Pulse Amplitude */
#define MAXM86161_REG_LED1_PA		0x23
#define MAXM86161_REG_LED2_PA		0x24
#define MAXM86161_REG_LED3_PA		0x25
#define MAXM86161_REG_LED_PILOT_PA	0x29

/* LED Range */
#define MAXM86161_REG_LED_RANGE_1	0x2A
#define  MAXM86161_LED1_RGE_MSK		NO_OS_GENMASK(1, 0)
#define  MAXM86161_LED2_RGE_MSK		NO_OS_GENMASK(3, 2)
#define  MAXM86161_LED3_RGE_MSK		NO_OS_GENMASK(5, 4)

/* Hi-Res DAC */
#define MAXM86161_REG_S1_HIRES_DAC	0x2C
#define MAXM86161_REG_S2_HIRES_DAC	0x2D
#define MAXM86161_REG_S3_HIRES_DAC	0x2E
#define MAXM86161_REG_S4_HIRES_DAC	0x2F
#define MAXM86161_REG_S5_HIRES_DAC	0x30
#define MAXM86161_REG_S6_HIRES_DAC	0x31
#define  MAXM86161_HIRES_DAC_OVR	NO_OS_BIT(7)
#define  MAXM86161_HIRES_DAC_VAL_MSK	NO_OS_GENMASK(5, 0)

/* Die Temperature */
#define MAXM86161_REG_DIE_TEMP_CFG	  0x40
#define  MAXM86161_TEMP_EN		  NO_OS_BIT(0)
#define  MAXM86161_TEMP_MEAS_WAIT_TRIES   10
#define  MAXM86161_TEMP_MEAS_DELAY_MS	  10
#define MAXM86161_REG_DIE_TEMP_INT	  0x41
#define MAXM86161_REG_DIE_TEMP_FRAC	  0x42
#define  MAXM86161_REG_DIE_TEMP_FRAC_MASK NO_OS_GENMASK(3, 0)
#define  MAXM86161_REG_DIE_TEMP_INT_MASK  NO_OS_GENMASK(7, 0) /* Temp should be read in a burst */
#define  MAXM86161_DIE_TEMP_FRAC_OFFSET   8
#define  MAXM86161_DIE_TEMP_SCALE_UC	  1000000
#define  MAXM86161_DIE_TEMP_FRAC_SCALE_UC 62500

/* DAC Calibration */
#define MAXM86161_REG_DAC_CAL_EN	0x50
#define  MAXM86161_CAL_DAC_COMPLETE	NO_OS_BIT(6)
#define  MAXM86161_CAL_DAC1_OOR		NO_OS_BIT(4)
#define  MAXM86161_START_CAL		NO_OS_BIT(2)

/* SHA-256 */
#define MAXM86161_REG_SHA_CMD		0xF0
#define MAXM86161_REG_SHA_CFG		0xF1
#define  MAXM86161_SHA_EN		NO_OS_BIT(1)
#define  MAXM86161_SHA_START		NO_OS_BIT(0)

/* Memory */
#define MAXM86161_REG_MEM_CTRL		0xF2
#define  MAXM86161_MEM_WR_EN		NO_OS_BIT(1)
#define  MAXM86161_MEM_BANK_SEL		NO_OS_BIT(0)
#define MAXM86161_REG_MEM_IDX		0xF3
#define MAXM86161_REG_MEM_DATA		0xF4

/* Part ID */
#define MAXM86161_REG_REV_ID		0xFE
#define MAXM86161_REG_PART_ID		0xFF

/* FIFO Data Format */
#define MAXM86161_FIFO_TAG_MSK		NO_OS_GENMASK(23, 19)
#define MAXM86161_FIFO_DATA_MSK		NO_OS_GENMASK(18, 0)
#define MAXM86161_FIFO_TAG_SHIFT	19

/*
 * Pack a decoded FIFO sample into a single 32-bit transport word:
 * tag[23:19] | data[18:0]. This is the on-the-wire format pushed into the
 * IIO circular buffer and must match the channel scan_type (realbits = 24,
 * storagebits = 32). The host recovers the tag and the unsigned 19-bit
 * measurement from these fields.
 */
#define MAXM86161_FIFO_PACK(tag, data) \
	((((uint32_t)(tag) << MAXM86161_FIFO_TAG_SHIFT) & MAXM86161_FIFO_TAG_MSK) | \
	 ((uint32_t)(data) & MAXM86161_FIFO_DATA_MSK))

/* SHA-256 Authentication */
#define MAXM86161_SHA_CMD_MAC_W_ROM	0x35
#define MAXM86161_SHA_CMD_MAC_WO_ROM	0x36
#define MAXM86161_SHA_MEM_START		0x00
#define MAXM86161_SHA_MEM_END		0x09

#define MAXM86161_FIFO_SAMPLE_SIZE	3
#define MAXM86161_FIFO_BURST_RD_CAP	(255U - (255U % MAXM86161_FIFO_SAMPLE_SIZE))

enum maxm86161_fifo_tag {
	MAXM86161_TAG_PPG1_LEDC1 = 0x01,
	MAXM86161_TAG_PPG1_LEDC2,
	MAXM86161_TAG_PPG1_LEDC3,
	MAXM86161_TAG_PPG1_LEDC4,
	MAXM86161_TAG_PPG1_LEDC5,
	MAXM86161_TAG_PPG1_LEDC6,
	MAXM86161_TAG_PROX1 = 0x19,
	MAXM86161_TAG_SUB_DAC_UPDATE = 0x1D,
	MAXM86161_TAG_INVALID,
	MAXM86161_TAG_TIME_STAMP,
};

enum maxm86161_led_num {
	MAXM86161_LED_NUM_GREEN = 1,
	MAXM86161_LED_NUM_IR,
	MAXM86161_LED_NUM_RED,
	MAXM86161_LED_NUM_PILOT_GREEN,
};

enum maxm86161_led_src {
	MAXM86161_LED_SRC_NONE = 0,
	MAXM86161_LED_SRC_GREEN,
	MAXM86161_LED_SRC_IR,
	MAXM86161_LED_SRC_RED,
	MAXM86161_LED_SRC_PILOT_GREEN = 8,
	MAXM86161_LED_SRC_DIRECT_AMB,
};

enum maxm86161_ppg_tint {
	MAXM86161_TINT_14_8_US = 0,
	MAXM86161_TINT_29_4_US,
	MAXM86161_TINT_58_7_US,
	MAXM86161_TINT_117_3_US,
};

enum maxm86161_adc_range {
	MAXM86161_ADC_RGE_4UA = 0,
	MAXM86161_ADC_RGE_8UA,
	MAXM86161_ADC_RGE_16UA,
	MAXM86161_ADC_RGE_32UA,
};

enum maxm86161_smp_ave {
	MAXM86161_SMP_AVE_1 = 0,
	MAXM86161_SMP_AVE_2,
	MAXM86161_SMP_AVE_4,
	MAXM86161_SMP_AVE_8,
	MAXM86161_SMP_AVE_164,
	MAXM86161_SMP_AVE_325,
	MAXM86161_SMP_AVE_646,
	MAXM86161_SMP_AVE_128,
};

enum maxm86161_led_range {
	MAXM86161_LED_RGE_31MA = 0,
	MAXM86161_LED_RGE_62MA,
	MAXM86161_LED_RGE_93MA,
	MAXM86161_LED_RGE_124MA,
};

enum maxm86161_led_settling {
	MAXM86161_LED_SETLNG_4_US = 0,
	MAXM86161_LED_SETLNG_6_US,
	MAXM86161_LED_SETLNG_8_US,
	MAXM86161_LED_SETLNG_12_US,
};

enum maxm86161_dig_filt_sel {
	MAXM86161_DIG_FILT_CDM = 0,
	MAXM86161_DIG_FILT_FDM,
};

enum maxm86161_burst_rate {
	MAXM86161_BURST_RATE_8HZ = 0,
	MAXM86161_BURST_RATE_32HZ,
	MAXM86161_BURST_RATE_84HZ,
	MAXM86161_BURST_RATE_256HZ,
};

enum maxm86161_pd_bias {
	MAXM86161_PD_BIAS_0_65PF = 1,
	MAXM86161_PD_BIAS_65_130PF = 5,
	MAXM86161_PD_BIAS_130_260PF,
	MAXM86161_PD_BIAS_260_520PF,
};

enum maxm86161_gpio_ctrl {
	MAXM86161_GPIO_TRISTATE = 0,
	MAXM86161_GPIO_SAMPLE_TRIGGER = 2,
	MAXM86161_GPIO_EXPOSURE_TRIGGER = 6,
	MAXM86161_GPIO_HW_FORCE_SYNC = 9,
	MAXM86161_GPIO_SAMPLE_SYNC_ONESHOT,
};

enum maxm86161_irq_sources {
	MAXM86161_INT_PWR_RDY = 0,
	MAXM86161_INT_DIE_TEMP_RDY,
	MAXM86161_INT_LED_COMPB,
	MAXM86161_INT_PROX_INT,
	MAXM86161_INT_ALC_OVF,
	MAXM86161_INT_DATA_RDY,
	MAXM86161_INT_A_FULL,
	MAXM86161_INT_SHA_DONE,

	MAXM86161_INT_COUNT,
};

/* Maps each irq_sources enum entry to its bit in the combined status word
 * ((INT_STATUS_2 << 8) | INT_STATUS_1).
 */
static const uint16_t maxm86161_irq_status_bits[MAXM86161_INT_COUNT] = {
	[MAXM86161_INT_PWR_RDY]      = MAXM86161_INT1_PWR_RDY,
	[MAXM86161_INT_DIE_TEMP_RDY] = MAXM86161_INT1_DIE_TEMP_RDY,
	[MAXM86161_INT_LED_COMPB]    = MAXM86161_INT1_LED_COMPB,
	[MAXM86161_INT_PROX_INT]     = MAXM86161_INT1_PROX_INT,
	[MAXM86161_INT_ALC_OVF]      = MAXM86161_INT1_ALC_OVF,
	[MAXM86161_INT_DATA_RDY]     = MAXM86161_INT1_DATA_RDY,
	[MAXM86161_INT_A_FULL]       = MAXM86161_INT1_A_FULL,
	[MAXM86161_INT_SHA_DONE]     = (uint16_t)MAXM86161_INT2_SHA_DONE << 8,
};

typedef void (*maxm86161_int_cb_t)(void *ctx);

struct maxm86161_ppg_config {
	enum maxm86161_ppg_tint integration_time;
	enum maxm86161_adc_range adc_range;
	enum maxm86161_smp_ave sample_avg;
	enum maxm86161_led_settling led_settling;
	enum maxm86161_dig_filt_sel dig_filter_sel;
	uint8_t sample_rate;
	bool alc_disable;
	bool add_offset;
};

struct maxm86161_led_seq_config {
	enum maxm86161_led_src sequence[MAXM86161_NUM_LED_SEQ];
};

struct maxm86161_led_config {
	enum maxm86161_led_range range[MAXM86161_NUM_LEDS];
	uint8_t pulse_amplitude[MAXM86161_NUM_LEDS];
	uint8_t pilot_pa;
};

struct maxm86161_fifo_config {
	uint8_t watermark;
	bool rollover;
	bool a_full_type;
	bool stat_clr;
};

struct maxm86161_picket_fence_config {
	uint8_t iir_tc;
	uint8_t iir_init_value;
	uint8_t threshold_sigma_mult;
	bool enable;
	bool order;
};

struct maxm86161_int_handler {
	maxm86161_int_cb_t callback;
	void *args;
};

struct maxm86161_dev;
struct maxm86161_init_param;

/*
 * Bus abstraction. Every register access in the driver goes through one of
 * these callbacks, so the same device logic runs unchanged over either a
 * direct-I2C connection (maxm86161_i2c_bus_ops) or a MAX32664 sensor-hub
 * passthrough (maxm86161_max32664_bus_ops, in maxm86161_max32664.h).
 *
 * init()/remove() bring up and tear down the backend transport and may store
 * backend-specific context in dev->i2c_desc (I2C) or dev->bus_priv (other).
 */
struct maxm86161_bus_ops {
	int (*init)(struct maxm86161_dev *dev,
		    struct maxm86161_init_param *init_param);
	int (*remove)(struct maxm86161_dev *dev);
	int (*reg_read)(struct maxm86161_dev *dev, uint8_t reg_addr,
			uint8_t *reg_data);
	int (*reg_write)(struct maxm86161_dev *dev, uint8_t reg_addr,
			 uint8_t reg_data);
	int (*burst_read)(struct maxm86161_dev *dev, uint8_t reg_addr,
			  uint8_t *reg_data, uint16_t count);
	int (*burst_write)(struct maxm86161_dev *dev, uint8_t reg_addr,
			   uint8_t *reg_data, uint16_t count);
};

/* Direct-I2C backend (default when init_param.bus_ops is NULL). */
extern const struct maxm86161_bus_ops maxm86161_i2c_bus_ops;

struct maxm86161_dev {
	struct maxm86161_int_handler int_handlers[MAXM86161_INT_COUNT];
	const struct maxm86161_bus_ops *bus_ops;
	struct no_os_i2c_desc *i2c_desc;
	struct no_os_gpio_desc *gpio_intb;
	struct no_os_irq_ctrl_desc *irq_ctrl;
	struct maxm86161_ppg_config ppg_config;
	struct maxm86161_led_seq_config led_seq;
	struct maxm86161_led_config led_config;
	struct maxm86161_fifo_config fifo_config;
	struct maxm86161_picket_fence_config pf_config;
	void *bus_priv;
	uint8_t part_id;
	uint8_t rev_id;
};

struct maxm86161_init_param {
	struct no_os_i2c_init_param i2c_init;
	struct no_os_gpio_init_param *gpio_intb_init;
	struct no_os_irq_init_param *irq_init;
	void *irq_handle;
};

/* Lifecycle */

/**
 * @brief Initialize the MAXM86161 device driver.
 *
 * Allocates the device descriptor, selects the bus backend (defaulting to the
 * I2C backend when none is provided in the init parameters) and initializes it,
 * verifies the part ID, resets the device, places it into shutdown, clears the
 * interrupt status and selects single-PPG mode. Optionally configures the INTB
 * GPIO and registers a falling-edge interrupt callback when the corresponding
 * init parameters are supplied. On any failure all acquired resources are
 * released.
 * @param device     - Pointer that receives the allocated device descriptor.
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_init(struct maxm86161_dev **device,
		   struct maxm86161_init_param *init_param);

/**
 * @brief Free resources allocated by maxm86161_init().
 *
 * Puts the device into shutdown, disables and unregisters the interrupt
 * callback and removes the IRQ controller (if used), removes the INTB GPIO
 * (if used), tears down the bus backend and frees the device descriptor. The
 * first error encountered while releasing GPIO/bus resources is returned.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_remove(struct maxm86161_dev *dev);

/**
 * @brief Perform a software reset of the MAXM86161.
 *
 * Sets the RESET bit in the system control register and waits for the reset
 * settling delay before returning.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_reset(struct maxm86161_dev *dev);

/**
 * @brief Verify the MAXM86161 part identity and cache revision info.
 *
 * Reads the part ID register and checks it against the expected value; on
 * mismatch -ENODEV is returned. On success the part ID and revision ID are
 * stored in the device descriptor.
 * @param dev - Device descriptor.
 * @return 0 on success, -ENODEV if the part ID does not match, or another
 *         negative error code on communication failure.
 */
int maxm86161_verify_id(struct maxm86161_dev *dev);

/* Register Access */

/**
 * @brief Read a single register from the MAXM86161.
 * @param dev      - Device descriptor.
 * @param reg_addr - Address of the register to read.
 * @param reg_data - Pointer where the read register value is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_reg_read(struct maxm86161_dev *dev, uint8_t reg_addr,
		       uint8_t *reg_data);

/**
 * @brief Write a single register on the MAXM86161.
 * @param dev      - Device descriptor.
 * @param reg_addr - Address of the register to write.
 * @param reg_data - Value to write into the register.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_reg_write(struct maxm86161_dev *dev, uint8_t reg_addr,
			uint8_t reg_data);

/**
 * @brief Read-modify-write the bits selected by a mask in a register.
 *
 * Reads the register, clears the bits covered by @p mask and writes @p data
 * into that bit field (aligned to the mask) before writing the register back.
 * @param dev      - Device descriptor.
 * @param reg_addr - Address of the register to update.
 * @param mask     - Bit mask selecting the field to modify.
 * @param data     - New value for the masked field (right-aligned).
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_reg_update_bits(struct maxm86161_dev *dev, uint8_t reg_addr,
			      uint8_t mask, uint8_t data);

/**
 * @brief Burst-read multiple bytes starting at a register address.
 * @param dev      - Device descriptor.
 * @param reg_addr - Starting register address for the burst read.
 * @param reg_data - Buffer where the read bytes are stored.
 * @param count    - Number of bytes to read.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_burst_reg_read(struct maxm86161_dev *dev, uint8_t reg_addr,
			     uint8_t *reg_data, uint16_t count);

/**
 * @brief Burst-write multiple bytes starting at a register address.
 * @param dev      - Device descriptor.
 * @param reg_addr - Starting register address for the burst write.
 * @param reg_data - Buffer holding the bytes to write.
 * @param count    - Number of bytes to write.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_burst_reg_write(struct maxm86161_dev *dev, uint8_t reg_addr,
			      uint8_t *reg_data, uint16_t count);

/* Power Management */

/**
 * @brief Put the MAXM86161 into shutdown (power-down) mode.
 *
 * Convenience wrapper around maxm86161_set_shutdown() that always enables
 * shutdown.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_shutdown(struct maxm86161_dev *dev);

/**
 * @brief Enable or disable shutdown mode of the MAXM86161.
 * @param dev    - Device descriptor.
 * @param enable - true to enter shutdown, false to resume normal operation.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_shutdown(struct maxm86161_dev *dev, bool enable);

/**
 * @brief Enable or disable low-power mode of the MAXM86161.
 * @param dev    - Device descriptor.
 * @param enable - true to enable low-power mode, false to disable it.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_low_power_mode(struct maxm86161_dev *dev, bool enable);

/* PPG Configuration */

/**
 * @brief Set the PPG sample rate.
 *
 * Updates the PPG_SR field of PPG_CFG_2 and caches the value in the device
 * configuration.
 * @param dev    - Device descriptor.
 * @param ppg_sr - Sample rate selection code (valid range 0x00 to 0x13).
 * @return 0 on success, -EINVAL for an out-of-range value, or another negative
 *         error code on failure.
 */
int maxm86161_set_sample_rate(struct maxm86161_dev *dev, uint8_t ppg_sr);

/**
 * @brief Get the currently configured PPG sample rate.
 * @param dev    - Device descriptor.
 * @param ppg_sr - Pointer where the sample rate selection code is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_sample_rate(struct maxm86161_dev *dev, uint8_t *ppg_sr);

/**
 * @brief Set the PPG ADC integration time.
 *
 * Updates the PPG_TINT field of PPG_CFG_1 and caches the value in the device
 * configuration.
 * @param dev  - Device descriptor.
 * @param tint - Integration time selection.
 * @return 0 on success, -EINVAL for an out-of-range value, or another negative
 *         error code on failure.
 */
int maxm86161_set_integration_time(struct maxm86161_dev *dev,
				   enum maxm86161_ppg_tint tint);

/**
 * @brief Get the currently configured PPG ADC integration time.
 * @param dev  - Device descriptor.
 * @param tint - Pointer where the integration time selection is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_integration_time(struct maxm86161_dev *dev,
				   enum maxm86161_ppg_tint *tint);

/**
 * @brief Set the PPG ADC full-scale range.
 *
 * Updates the ADC_RGE field of PPG_CFG_1 and caches the value in the device
 * configuration.
 * @param dev   - Device descriptor.
 * @param range - ADC range selection.
 * @return 0 on success, -EINVAL for an out-of-range value, or another negative
 *         error code on failure.
 */
int maxm86161_set_adc_range(struct maxm86161_dev *dev,
			    enum maxm86161_adc_range range);

/**
 * @brief Get the currently configured PPG ADC full-scale range.
 * @param dev   - Device descriptor.
 * @param range - Pointer where the ADC range selection is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_adc_range(struct maxm86161_dev *dev,
			    enum maxm86161_adc_range *range);

/**
 * @brief Set the number of samples averaged per FIFO data point.
 *
 * Updates the SMP_AVE field of PPG_CFG_2 and caches the value in the device
 * configuration.
 * @param dev - Device descriptor.
 * @param avg - Sample averaging selection.
 * @return 0 on success, -EINVAL for an out-of-range value, or another negative
 *         error code on failure.
 */
int maxm86161_set_sample_averaging(struct maxm86161_dev *dev,
				   enum maxm86161_smp_ave avg);

/**
 * @brief Get the currently configured sample averaging.
 * @param dev - Device descriptor.
 * @param avg - Pointer where the sample averaging selection is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_sample_averaging(struct maxm86161_dev *dev,
				   enum maxm86161_smp_ave *avg);

/**
 * @brief Enable or disable the Ambient Light Cancellation (ALC) circuit.
 *
 * Updates the ALC_DISABLE bit of PPG_CFG_1 and caches the value in the device
 * configuration.
 * @param dev     - Device descriptor.
 * @param disable - true to disable ALC, false to enable it.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_alc_disable(struct maxm86161_dev *dev, bool disable);

/**
 * @brief Get the current Ambient Light Cancellation (ALC) disable state.
 * @param dev     - Device descriptor.
 * @param disable - Pointer set to true if ALC is disabled, false otherwise.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_alc_disable(struct maxm86161_dev *dev, bool *disable);

/**
 * @brief Enable or disable the ADC add-offset feature.
 *
 * Updates the ADD_OFFSET bit of PPG_CFG_1 and caches the value in the device
 * configuration.
 * @param dev    - Device descriptor.
 * @param enable - true to enable the added offset, false to disable it.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_add_offset(struct maxm86161_dev *dev, bool enable);

/**
 * @brief Get the current ADC add-offset enable state.
 * @param dev    - Device descriptor.
 * @param enable - Pointer set to true if the offset is enabled, false otherwise.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_add_offset(struct maxm86161_dev *dev, bool *enable);

/**
 * @brief Set the LED settling time.
 *
 * Updates the LED_SETLNG field of PPG_CFG_3 and caches the value in the device
 * configuration.
 * @param dev      - Device descriptor.
 * @param settling - LED settling time selection.
 * @return 0 on success, -EINVAL for an out-of-range value, or another negative
 *         error code on failure.
 */
int maxm86161_set_led_settling(struct maxm86161_dev *dev,
			       enum maxm86161_led_settling settling);

/**
 * @brief Get the currently configured LED settling time.
 * @param dev      - Device descriptor.
 * @param settling - Pointer where the LED settling time selection is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_led_settling(struct maxm86161_dev *dev,
			       enum maxm86161_led_settling *settling);

/**
 * @brief Select the PPG digital filter (CDM or FDM).
 *
 * Updates the DIG_FILT field of PPG_CFG_3 and caches the value in the device
 * configuration.
 * @param dev    - Device descriptor.
 * @param filter - Digital filter selection.
 * @return 0 on success, -EINVAL for an out-of-range value, or another negative
 *         error code on failure.
 */
int maxm86161_set_digital_filter(struct maxm86161_dev *dev,
				 enum maxm86161_dig_filt_sel filter);

/**
 * @brief Get the currently selected PPG digital filter.
 * @param dev    - Device descriptor.
 * @param filter - Pointer where the digital filter selection is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_digital_filter(struct maxm86161_dev *dev,
				 enum maxm86161_dig_filt_sel *filter);

/**
 * @brief Set the photodiode bias (based on PD capacitance).
 * @param dev  - Device descriptor.
 * @param bias - Photodiode bias selection.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_pd_bias(struct maxm86161_dev *dev,
			  enum maxm86161_pd_bias bias);

/**
 * @brief Get the currently configured photodiode bias.
 * @param dev  - Device descriptor.
 * @param bias - Pointer where the photodiode bias selection is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_pd_bias(struct maxm86161_dev *dev,
			  enum maxm86161_pd_bias *bias);

/* LED Sequence Control */

/**
 * @brief Set the LED/measurement source for a single sequence slot.
 *
 * Selects the target LED_SEQ register and nibble (even/odd) based on the slot
 * index, caches the source in the device descriptor and writes the field.
 * @param dev  - Device descriptor.
 * @param slot - Sequence slot index (0 to MAXM86161_NUM_LED_SEQ - 1).
 * @param src  - LED/measurement source assigned to the slot.
 * @return 0 on success, -EINVAL for an invalid slot, or another negative error
 *         code on failure.
 */
int maxm86161_set_led_sequence(struct maxm86161_dev *dev, uint8_t slot,
			       enum maxm86161_led_src src);

/**
 * @brief Get the LED/measurement source configured for a sequence slot.
 * @param dev  - Device descriptor.
 * @param slot - Sequence slot index (0 to MAXM86161_NUM_LED_SEQ - 1).
 * @param src  - Pointer where the slot's source is stored.
 * @return 0 on success, -EINVAL for an invalid slot, or another negative error
 *         code on failure.
 */
int maxm86161_get_led_sequence(struct maxm86161_dev *dev, uint8_t slot,
			       enum maxm86161_led_src *src);

/**
 * @brief Program the full LED measurement sequence.
 *
 * Writes each of the MAXM86161_NUM_LED_SEQ slots from the supplied
 * configuration by calling maxm86161_set_led_sequence() for every slot.
 * @param dev    - Device descriptor.
 * @param config - Sequence configuration containing all slot sources.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_configure_led_sequence(struct maxm86161_dev *dev,
				     struct maxm86161_led_seq_config *config);

/* LED Pulse Amplitude */

/**
 * @brief Set the pulse amplitude (drive current) of an LED.
 *
 * Writes the LEDn_PA register for the selected LED and caches the value in the
 * device descriptor.
 * @param dev       - Device descriptor.
 * @param led_num   - LED index (1 to MAXM86161_NUM_LEDS).
 * @param amplitude - 8-bit pulse amplitude code.
 * @return 0 on success, -EINVAL for an invalid LED number, or another negative
 *         error code on failure.
 */
int maxm86161_set_led_pulse_amplitude(struct maxm86161_dev *dev,
				      uint8_t led_num, uint8_t amplitude);

/**
 * @brief Get the pulse amplitude (drive current) of an LED.
 * @param dev       - Device descriptor.
 * @param led_num   - LED index (1 to MAXM86161_NUM_LEDS).
 * @param amplitude - Pointer where the 8-bit pulse amplitude code is stored.
 * @return 0 on success, -EINVAL for an invalid LED number, or another negative
 *         error code on failure.
 */
int maxm86161_get_led_pulse_amplitude(struct maxm86161_dev *dev,
				      uint8_t led_num, uint8_t *amplitude);

/**
 * @brief Set the pilot LED pulse amplitude used in proximity mode.
 *
 * Writes the LED_PILOT_PA register and caches the value in the device
 * descriptor.
 * @param dev       - Device descriptor.
 * @param amplitude - 8-bit pilot pulse amplitude code.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_led_pilot_pa(struct maxm86161_dev *dev, uint8_t amplitude);

/**
 * @brief Get the pilot LED pulse amplitude used in proximity mode.
 * @param dev       - Device descriptor.
 * @param amplitude - Pointer where the 8-bit pilot pulse amplitude is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_led_pilot_pa(struct maxm86161_dev *dev, uint8_t *amplitude);

/**
 * @brief Set the full-scale current range of an LED driver.
 *
 * Selects the appropriate mask in the LED_RANGE_1 register for the given LED,
 * caches the range in the device descriptor and updates the field.
 * @param dev     - Device descriptor.
 * @param led_num - LED index (1 to MAXM86161_NUM_LEDS).
 * @param range   - LED current range selection.
 * @return 0 on success, -EINVAL for an invalid LED number or range, or another
 *         negative error code on failure.
 */
int maxm86161_set_led_range(struct maxm86161_dev *dev, uint8_t led_num,
			    enum maxm86161_led_range range);

/**
 * @brief Get the full-scale current range of an LED driver.
 * @param dev     - Device descriptor.
 * @param led_num - LED index (1 to MAXM86161_NUM_LEDS).
 * @param range   - Pointer where the LED current range selection is stored.
 * @return 0 on success, -EINVAL for an invalid LED number, or another negative
 *         error code on failure.
 */
int maxm86161_get_led_range(struct maxm86161_dev *dev, uint8_t led_num,
			    enum maxm86161_led_range *range);


/**
 * @brief Get the full-scale current range of an LED driver.
 * @param dev     - Device descriptor.
 * @param led_num - LED index (1 to MAXM86161_NUM_LEDS).
 * @param range   - LED current range selection.
 * @param led_seq_num  - LED/measurement source assigned to the slot.
 * @param amplitude - Pointer where the 8-bit pilot pulse amplitude is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_led_cfg(struct maxm86161_dev *dev,
			  enum maxm86161_led_num led_num,
			  enum maxm86161_led_range range, uint8_t led_seq_num, uint8_t amplitude);

/* Hi-Res DAC */

/**
 * @brief Configure the high-resolution DAC override for a sequence slot.
 *
 * Writes the per-slot HIRES_DAC register, optionally setting the override bit
 * together with the 6-bit DAC value.
 * @param dev      - Device descriptor.
 * @param slot     - Sequence slot index (1 to MAXM86161_NUM_LED_SEQ).
 * @param override - true to force the DAC value, false to use auto-calibration.
 * @param dac_val  - 6-bit DAC value (only the lower 6 bits are used).
 * @return 0 on success, -EINVAL for an invalid slot, or another negative error
 *         code on failure.
 */
int maxm86161_set_hires_dac(struct maxm86161_dev *dev, uint8_t slot,
			    bool override, uint8_t dac_val);

/**
 * @brief Read back the high-resolution DAC configuration for a slot.
 * @param dev      - Device descriptor.
 * @param slot     - Sequence slot index (1 to MAXM86161_NUM_LED_SEQ).
 * @param override - Pointer set to true if the DAC override is active.
 * @param dac_val  - Pointer where the 6-bit DAC value is stored.
 * @return 0 on success, -EINVAL for an invalid slot, or another negative error
 *         code on failure.
 */
int maxm86161_get_hires_dac(struct maxm86161_dev *dev, uint8_t slot,
			    bool *override, uint8_t *dac_val);

/* FIFO Management */

/**
 * @brief Set the FIFO almost-full watermark threshold.
 *
 * Updates the A_FULL field of FIFO_CFG_1 and caches the value in the device
 * configuration.
 * @param dev       - Device descriptor.
 * @param watermark - Almost-full threshold (valid range 0 to 127).
 * @return 0 on success, -EINVAL for an out-of-range value, or another negative
 *         error code on failure.
 */
int maxm86161_set_fifo_watermark(struct maxm86161_dev *dev, uint8_t watermark);

/**
 * @brief Get the FIFO almost-full watermark threshold.
 * @param dev       - Device descriptor.
 * @param watermark - Pointer where the almost-full threshold is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_fifo_watermark(struct maxm86161_dev *dev, uint8_t *watermark);

/**
 * @brief Flush (clear) the FIFO contents.
 *
 * Sets the FLUSH bit in FIFO_CFG_2 to discard all currently buffered samples.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_fifo_flush(struct maxm86161_dev *dev);

/**
 * @brief Enable or disable FIFO roll-over on full.
 *
 * Updates the FIFO_RO bit of FIFO_CFG_2 and caches the value in the device
 * configuration. When enabled, new samples overwrite the oldest once the FIFO
 * is full.
 * @param dev    - Device descriptor.
 * @param enable - true to enable roll-over, false to disable it.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_fifo_rollover(struct maxm86161_dev *dev, bool enable);

/**
 * @brief Get the FIFO roll-over enable state.
 * @param dev    - Device descriptor.
 * @param enable - Pointer set to true if roll-over is enabled, false otherwise.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_fifo_rollover(struct maxm86161_dev *dev, bool *enable);

/**
 * @brief Set the FIFO almost-full interrupt assertion behavior.
 *
 * Updates the A_FULL_TYPE bit of FIFO_CFG_2 and caches the value in the device
 * configuration, selecting whether the almost-full interrupt is repeated for
 * each new sample or asserted only once.
 * @param dev  - Device descriptor.
 * @param type - Almost-full interrupt type selection.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_fifo_a_full_type(struct maxm86161_dev *dev, bool type);

/**
 * @brief Get the FIFO almost-full interrupt assertion behavior.
 * @param dev  - Device descriptor.
 * @param type - Pointer where the almost-full interrupt type is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_fifo_a_full_type(struct maxm86161_dev *dev, bool *type);

/**
 * @brief Configure whether reading FIFO data clears the status bit.
 *
 * Updates the STAT_CLR bit of FIFO_CFG_2 and caches the value in the device
 * configuration.
 * @param dev    - Device descriptor.
 * @param enable - true to clear the FIFO status on data read, false otherwise.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_fifo_stat_clr(struct maxm86161_dev *dev, bool enable);

/**
 * @brief Get the number of samples currently stored in the FIFO.
 * @param dev   - Device descriptor.
 * @param count - Pointer where the FIFO sample count is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_fifo_count(struct maxm86161_dev *dev, uint8_t *count);

/**
 * @brief Read the FIFO overflow counter.
 *
 * Reads the OVF_COUNTER register and returns the number of samples lost due to
 * FIFO overflow (lower 7 bits).
 * @param dev            - Device descriptor.
 * @param overflow_count - Pointer where the overflow sample count is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_fifo_overflow_check(struct maxm86161_dev *dev,
				  uint8_t *overflow_count);

/**
 * @brief Burst-read raw sample data from the FIFO.
 *
 * Reads num_samples samples from the FIFO_DATA register, each sample being
 * MAXM86161_FIFO_DATA_SIZE bytes wide.
 * @param dev         - Device descriptor.
 * @param data        - Buffer receiving the raw sample bytes; must hold at least
 *                      num_samples * MAXM86161_FIFO_DATA_SIZE bytes.
 * @param num_samples - Number of samples to read (must be non-zero).
 * @return 0 on success, -EINVAL for invalid arguments, or another negative
 *         error code on failure.
 */
int maxm86161_read_fifo_data(struct maxm86161_dev *dev, uint8_t *data,
			     uint16_t num_samples);

/**
 * @brief Decode a raw 3-byte FIFO sample into its tag and data fields.
 *
 * Combines the three raw bytes (big-endian) into a 24-bit value and extracts
 * the sample tag and the measurement data.
 * @param raw  - Pointer to the 3 raw sample bytes.
 * @param tag  - Pointer where the decoded sample tag is stored.
 * @param data - Pointer where the decoded measurement data is stored.
 * @return 0 on success, -EINVAL if any pointer is NULL.
 */
int maxm86161_decode_fifo_sample(uint8_t *raw, uint8_t *tag, uint32_t *data);

/**
 * @brief Count the number of active slots in the LED measurement sequence.
 *
 * Iterates over the cached LED sequence and counts consecutive slots whose
 * source is not MAXM86161_LED_SRC_NONE, stopping at the first empty slot.
 * @param dev   - Device descriptor.
 * @param count - Pointer where the number of active slots is stored.
 * @return 0 on success, -EINVAL if any pointer is NULL.
 */
int maxm86161_get_active_led_count(struct maxm86161_dev *dev, uint8_t *count);

/* Interrupts */

/**
 * @brief Read the combined interrupt status of the MAXM86161.
 *
 * Reads both INT_STATUS_1 and INT_STATUS_2 and packs them into a single 16-bit
 * value (STATUS_2 in the high byte, STATUS_1 in the low byte).
 * @param dev    - Device descriptor.
 * @param status - Pointer where the packed interrupt status is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_interrupt_status(struct maxm86161_dev *dev, uint16_t *status);

/**
 * @brief Clear pending interrupt status flags.
 *
 * Reads INT_STATUS_1 and INT_STATUS_2 (which are cleared on read) to
 * acknowledge and clear any pending interrupt flags.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_clear_interrupt_status(struct maxm86161_dev *dev);

/**
 * @brief Register a callback for an interrupt source and enable/disable it.
 *
 * Enables or disables the enable bit for the requested interrupt source in the
 * appropriate INT_EN register and stores the callback and its argument in the
 * device's handler table. The PWR_RDY source has no enable bit, so only the
 * callback is registered for it.
 * @param dev        - Device descriptor.
 * @param int_source - Interrupt source to configure.
 * @param enable     - true to enable the interrupt, false to disable it.
 * @param callback   - Callback invoked when the interrupt fires.
 * @param cb_args    - Opaque argument passed to the callback.
 * @return 0 on success, -EINVAL for an invalid interrupt source, or another
 *         negative error code on failure.
 */
int maxm86161_set_irq_handler(struct maxm86161_dev *dev,
			      enum maxm86161_irq_sources int_source,
			      bool enable, maxm86161_int_cb_t callback,
			      void *cb_args);

/**
 * @brief Top-level interrupt dispatcher for the MAXM86161.
 *
 * Intended to be registered as the GPIO/INTB interrupt callback. Reads the
 * combined interrupt status and, for every asserted source, invokes the
 * registered per-source callback (if any). Does nothing if the context is NULL
 * or the status read fails.
 * @param ctx - Opaque context, expected to be a struct maxm86161_dev pointer.
 */
void maxm86161_irq_handler(void *ctx);

/* Proximity Mode */

/**
 * @brief Set the proximity mode entry threshold.
 *
 * Writes the PROX_INT_THRESH register that determines the ADC count at which
 * the device transitions from proximity to normal PPG mode.
 * @param dev       - Device descriptor.
 * @param threshold - Proximity interrupt threshold value.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_prox_threshold(struct maxm86161_dev *dev, uint8_t threshold);

/**
 * @brief Get the proximity mode entry threshold.
 * @param dev       - Device descriptor.
 * @param threshold - Pointer where the proximity threshold value is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_prox_threshold(struct maxm86161_dev *dev, uint8_t *threshold);

/* Picket Fence */

/**
 * @brief Configure the picket-fence detect-and-replace feature.
 *
 * Packs the enable flag, ordering, IIR time constant, IIR init value and
 * threshold sigma multiplier from the configuration into the PICKET_FENCE
 * register, caches the configuration in the device descriptor and writes it.
 * @param dev    - Device descriptor.
 * @param config - Picket-fence configuration to apply.
 * @return 0 on success, -EINVAL if any pointer is NULL, or another negative
 *         error code on failure.
 */
int maxm86161_set_picket_fence(struct maxm86161_dev *dev,
			       struct maxm86161_picket_fence_config *config);

/**
 * @brief Read back the current picket-fence configuration.
 *
 * Reads the PICKET_FENCE register and unpacks its fields into the supplied
 * configuration structure.
 * @param dev    - Device descriptor.
 * @param config - Structure that receives the decoded configuration.
 * @return 0 on success, -EINVAL if any pointer is NULL, or another negative
 *         error code on failure.
 */
int maxm86161_get_picket_fence(struct maxm86161_dev *dev,
			       struct maxm86161_picket_fence_config *config);

/* Die Temperature */

/**
 * @brief Read and convert the die temperature.
 *
 * Reads the integer and fractional die-temperature registers and combines them
 * into a signed temperature in degrees Celsius (fractional LSB = 0.0625 degC).
 * @param dev         - Device descriptor.
 * @param temperature - Pointer where the temperature in micro-degrees Celsius
 *                      (1 degC = 1000000) is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_read_die_temperature(struct maxm86161_dev *dev,
				   int32_t *temperature);

/* DAC Calibration */

/**
 * @brief Start the DAC (offset) calibration procedure.
 *
 * Sets the START_CAL bit in the DAC calibration enable register to begin
 * calibration. Completion should be polled via maxm86161_get_calibration_status().
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_start_calibration(struct maxm86161_dev *dev);

/**
 * @brief Query the DAC calibration status.
 *
 * Reads the DAC calibration register and reports whether calibration has
 * completed and whether the result is out of range.
 * @param dev      - Device descriptor.
 * @param complete - Pointer set to true when calibration has completed.
 * @param oor      - Pointer set to true when the calibration result is out of range.
 * @return 0 on success, -EINVAL if any pointer is NULL, or another negative
 *         error code on failure.
 */
int maxm86161_get_calibration_status(struct maxm86161_dev *dev,
				     bool *complete, bool *oor);

/* Burst Mode */

/**
 * @brief Enable or disable burst sampling mode and set its rate.
 *
 * Updates the BURST_EN bit and, on success, the BURST_RATE field of PPG_CFG_3.
 * @param dev    - Device descriptor.
 * @param enable - true to enable burst mode, false to disable it.
 * @param rate   - Burst sampling rate selection.
 * @return 0 on success, -EINVAL for an out-of-range rate, or another negative
 *         error code on failure.
 */
int maxm86161_set_burst_mode(struct maxm86161_dev *dev, bool enable,
			     enum maxm86161_burst_rate rate);

/* PPG Sync */

/**
 * @brief Enable or disable timestamp tagging of PPG samples.
 *
 * Updates the TIMESTAMP_EN bit of the PPG sync control register.
 * @param dev    - Device descriptor.
 * @param enable - true to enable timestamps, false to disable them.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_timestamp_enable(struct maxm86161_dev *dev, bool enable);

/**
 * @brief Enable or disable DAC-code tagging of PPG samples.
 *
 * Updates the DAC_CODE_TAG bit of the PPG sync control register.
 * @param dev    - Device descriptor.
 * @param enable - true to enable DAC-code tagging, false to disable it.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_dac_code_tag(struct maxm86161_dev *dev, bool enable);

/**
 * @brief Configure the behavior of the GPIO/sync pin.
 *
 * Updates the GPIO_CTRL field of the PPG sync control register.
 * @param dev  - Device descriptor.
 * @param ctrl - GPIO control mode selection.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_gpio_ctrl(struct maxm86161_dev *dev,
			    enum maxm86161_gpio_ctrl ctrl);

/**
 * @brief Issue a software-forced PPG synchronization event.
 *
 * Sets the SW_FORCE bit in the PPG sync control register to trigger a sync.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_force_sync(struct maxm86161_dev *dev);

/* SHA-256 Authentication */

/**
 * @brief Enable or disable the SHA-256 authentication engine.
 *
 * Updates the SHA_EN bit of the SHA configuration register.
 * @param dev    - Device descriptor.
 * @param enable - true to enable the SHA engine, false to disable it.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_sha_enable(struct maxm86161_dev *dev, bool enable);

/**
 * @brief Issue a SHA command and start the SHA-256 operation.
 *
 * Writes the command to the SHA command register and then sets the SHA_START
 * bit in the SHA configuration register to begin the operation.
 * @param dev - Device descriptor.
 * @param cmd - SHA command code to execute.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_sha_start(struct maxm86161_dev *dev, uint8_t cmd);

/**
 * @brief Write a SHA-256 challenge into the device memory.
 *
 * Enables memory writes, sets the memory index to the SHA memory start and
 * burst-writes the challenge into the memory data register. The challenge must
 * be exactly 20 bytes (a 160-bit value).
 * @param dev       - Device descriptor.
 * @param challenge - Buffer holding the challenge bytes.
 * @param len       - Length of the challenge in bytes (must be 20).
 * @return 0 on success, -EINVAL for invalid arguments or an incorrect length,
 *         or another negative error code on failure.
 */
int maxm86161_sha_write_challenge(struct maxm86161_dev *dev,
				  uint8_t *challenge, uint8_t len);

/**
 * @brief Read the SHA-256 response from the device memory.
 *
 * Sets the memory index to the SHA memory start and burst-reads the response
 * from the memory data register.
 * @param dev      - Device descriptor.
 * @param response - Buffer receiving the response bytes.
 * @param len      - Number of bytes to read (must be non-zero).
 * @return 0 on success, -EINVAL for invalid arguments, or another negative
 *         error code on failure.
 */
int maxm86161_sha_read_response(struct maxm86161_dev *dev,
				uint8_t *response, uint8_t len);

/* Memory Access */

/**
 * @brief Read a single byte from the device memory.
 *
 * Selects the memory bank based on the address (bank 1 for addresses above
 * 0xFF), sets the memory index to the low byte of the address and reads the
 * memory data register.
 * @param dev  - Device descriptor.
 * @param addr - Memory address to read (valid range 0x000 to 0x17F).
 * @param data - Pointer where the read byte is stored.
 * @return 0 on success, -EINVAL for invalid arguments or an out-of-range
 *         address, or another negative error code on failure.
 */
int maxm86161_memory_read(struct maxm86161_dev *dev, uint16_t addr,
			  uint8_t *data);

/**
 * @brief Write a single byte to the device memory.
 *
 * Enables memory writes and selects the memory bank based on the address (bank
 * 1 for addresses above 0xFF), sets the memory index to the low byte of the
 * address and writes the memory data register.
 * @param dev  - Device descriptor.
 * @param addr - Memory address to write (valid range 0x000 to 0x17F).
 * @param data - Byte value to write.
 * @return 0 on success, -EINVAL for invalid arguments or an out-of-range
 *         address, or another negative error code on failure.
 */
int maxm86161_memory_write(struct maxm86161_dev *dev, uint16_t addr,
			   uint8_t data);

#endif /* __MAXM86161_H__ */
