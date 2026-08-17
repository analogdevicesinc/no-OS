/***************************************************************************//**
 *   @file   maxm86161.h
 *   @brief  Header file for MAXM86161 optical pulse oximeter driver.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
#define  MAXM86161_FIFO_OVF_COUNTER_MSK	NO_OS_GENMASK(6, 0)

#define MAXM86161_REG_FIFO_DATA_COUNT	0x07
#define MAXM86161_REG_FIFO_DATA		0x08

#define MAXM86161_REG_FIFO_CFG_1	0x09
#define  MAXM86161_FIFO_A_FULL_MSK	NO_OS_GENMASK(6, 0)
#define  MAXM86161_FIFO_A_FULL_MAX	127

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

#define MAXM86161_PPG_SR_MAX	0x13

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

#define MAXM86161_SHA_CHALLENGE_LEN	20

/* Memory */
#define MAXM86161_REG_MEM_CTRL		0xF2
#define  MAXM86161_MEM_WR_EN		NO_OS_BIT(1)
#define  MAXM86161_MEM_BANK_SEL		NO_OS_BIT(0)
#define MAXM86161_REG_MEM_IDX		0xF3
#define  MAXM86161_MEM_IDX_MAX		0x17F
#define  MAXM86161_MEM_IDX_BANK0_MSK	0xFF
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

struct maxm86161_int_handler {
	maxm86161_int_cb_t callback;
	void *args;
};

struct maxm86161_dev {
	struct maxm86161_int_handler int_handlers[MAXM86161_INT_COUNT];
	struct no_os_i2c_desc *i2c_desc;
	struct no_os_gpio_desc *gpio_intb;
	struct no_os_irq_ctrl_desc *irq_ctrl;
	uint8_t rev_id;
};

struct maxm86161_init_param {
	struct no_os_i2c_init_param i2c_init;
	struct no_os_gpio_init_param *gpio_intb_init;
	struct no_os_irq_init_param *irq_init;
	void *irq_handle;
};

/* Lifecycle */

int maxm86161_init(struct maxm86161_dev **device,
		   struct maxm86161_init_param *init_param);

int maxm86161_remove(struct maxm86161_dev *dev);

int maxm86161_reset(struct maxm86161_dev *dev);

int maxm86161_verify_id(struct maxm86161_dev *dev);

/* Register Access */

int maxm86161_reg_read(struct maxm86161_dev *dev, uint8_t reg_addr,
		       uint8_t *reg_data);

int maxm86161_reg_write(struct maxm86161_dev *dev, uint8_t reg_addr,
			uint8_t reg_data);

int maxm86161_reg_update_bits(struct maxm86161_dev *dev, uint8_t reg_addr,
			      uint8_t mask, uint8_t data);

int maxm86161_burst_reg_read(struct maxm86161_dev *dev, uint8_t reg_addr,
			     uint8_t *reg_data, uint16_t count);

int maxm86161_burst_reg_write(struct maxm86161_dev *dev, uint8_t reg_addr,
			      uint8_t *reg_data, uint16_t count);

/* Power Management */

int maxm86161_shutdown(struct maxm86161_dev *dev);

int maxm86161_set_shutdown(struct maxm86161_dev *dev, bool enable);

int maxm86161_set_low_power_mode(struct maxm86161_dev *dev, bool enable);

/* PPG Configuration */

int maxm86161_set_sample_rate(struct maxm86161_dev *dev, uint8_t ppg_sr);

int maxm86161_get_sample_rate(struct maxm86161_dev *dev, uint8_t *ppg_sr);

int maxm86161_set_integration_time(struct maxm86161_dev *dev,
				   enum maxm86161_ppg_tint tint);

int maxm86161_get_integration_time(struct maxm86161_dev *dev,
				   enum maxm86161_ppg_tint *tint);

int maxm86161_set_adc_range(struct maxm86161_dev *dev,
			    enum maxm86161_adc_range range);

int maxm86161_get_adc_range(struct maxm86161_dev *dev,
			    enum maxm86161_adc_range *range);

int maxm86161_set_sample_averaging(struct maxm86161_dev *dev,
				   enum maxm86161_smp_ave avg);

int maxm86161_get_sample_averaging(struct maxm86161_dev *dev,
				   enum maxm86161_smp_ave *avg);

int maxm86161_set_alc_disable(struct maxm86161_dev *dev, bool disable);

int maxm86161_get_alc_disable(struct maxm86161_dev *dev, bool *disable);

int maxm86161_set_add_offset(struct maxm86161_dev *dev, bool enable);

int maxm86161_get_add_offset(struct maxm86161_dev *dev, bool *enable);

int maxm86161_set_led_settling(struct maxm86161_dev *dev,
			       enum maxm86161_led_settling settling);

int maxm86161_get_led_settling(struct maxm86161_dev *dev,
			       enum maxm86161_led_settling *settling);

int maxm86161_set_digital_filter(struct maxm86161_dev *dev,
				 enum maxm86161_dig_filt_sel filter);

int maxm86161_get_digital_filter(struct maxm86161_dev *dev,
				 enum maxm86161_dig_filt_sel *filter);

int maxm86161_set_pd_bias(struct maxm86161_dev *dev,
			  enum maxm86161_pd_bias bias);

int maxm86161_get_pd_bias(struct maxm86161_dev *dev,
			  enum maxm86161_pd_bias *bias);

/* LED Sequence Control */

int maxm86161_set_led_sequence(struct maxm86161_dev *dev, uint8_t slot,
			       enum maxm86161_led_src src);

int maxm86161_get_led_sequence(struct maxm86161_dev *dev, uint8_t slot,
			       enum maxm86161_led_src *src);

/* LED Pulse Amplitude */

int maxm86161_set_led_pulse_amplitude(struct maxm86161_dev *dev,
				      enum maxm86161_led_num led_num, uint8_t amplitude);

int maxm86161_get_led_pulse_amplitude(struct maxm86161_dev *dev,
				      enum maxm86161_led_num led_num, uint8_t *amplitude);

int maxm86161_set_led_range(struct maxm86161_dev *dev,
			    enum maxm86161_led_num led_num,
			    enum maxm86161_led_range range);

int maxm86161_get_led_range(struct maxm86161_dev *dev,
			    enum maxm86161_led_num led_num,
			    enum maxm86161_led_range *range);


int maxm86161_set_led_cfg(struct maxm86161_dev *dev,
			  enum maxm86161_led_num led_num,
			  enum maxm86161_led_range range, uint8_t led_seq_num, uint8_t amplitude);

/* Hi-Res DAC */

int maxm86161_set_hires_dac(struct maxm86161_dev *dev, uint8_t slot,
			    bool override, uint8_t dac_val);

int maxm86161_get_hires_dac(struct maxm86161_dev *dev, uint8_t slot,
			    bool *override, uint8_t *dac_val);

/* FIFO Management */

int maxm86161_set_fifo_watermark(struct maxm86161_dev *dev, uint8_t watermark);

int maxm86161_get_fifo_watermark(struct maxm86161_dev *dev, uint8_t *watermark);

int maxm86161_fifo_flush(struct maxm86161_dev *dev);

int maxm86161_set_fifo_rollover(struct maxm86161_dev *dev, bool enable);

int maxm86161_get_fifo_rollover(struct maxm86161_dev *dev, bool *enable);

int maxm86161_set_fifo_a_full_type(struct maxm86161_dev *dev, bool type);

int maxm86161_get_fifo_a_full_type(struct maxm86161_dev *dev, bool *type);

int maxm86161_set_fifo_stat_clr(struct maxm86161_dev *dev, bool enable);

int maxm86161_get_fifo_count(struct maxm86161_dev *dev, uint8_t *count);

int maxm86161_fifo_overflow_check(struct maxm86161_dev *dev,
				  uint8_t *overflow_count);

int maxm86161_read_fifo_data(struct maxm86161_dev *dev, uint8_t *data,
			     uint16_t num_samples);

int maxm86161_decode_fifo_sample(uint8_t *raw, uint8_t *tag, uint32_t *data);

/* Interrupts */

int maxm86161_get_interrupt_status(struct maxm86161_dev *dev, uint16_t *status);

int maxm86161_clear_interrupt_status(struct maxm86161_dev *dev);

int maxm86161_set_irq_handler(struct maxm86161_dev *dev,
			      enum maxm86161_irq_sources int_source,
			      bool enable, maxm86161_int_cb_t callback,
			      void *cb_args);

void maxm86161_irq_handler(void *ctx);

/* Proximity Mode */

int maxm86161_set_prox_threshold(struct maxm86161_dev *dev, uint8_t threshold);

int maxm86161_get_prox_threshold(struct maxm86161_dev *dev, uint8_t *threshold);

/* Picket Fence */

int maxm86161_set_pf_enable(struct maxm86161_dev *dev, bool enable);

int maxm86161_get_pf_enable(struct maxm86161_dev *dev, bool *enable);

int maxm86161_set_pf_order(struct maxm86161_dev *dev, bool order);

int maxm86161_get_pf_order(struct maxm86161_dev *dev, bool *order);

int maxm86161_set_pf_iir_tc(struct maxm86161_dev *dev, uint8_t iir_tc);

int maxm86161_get_pf_iir_tc(struct maxm86161_dev *dev, uint8_t *iir_tc);

int maxm86161_set_pf_iir_init(struct maxm86161_dev *dev,
			      uint8_t iir_init_value);

int maxm86161_get_pf_iir_init(struct maxm86161_dev *dev,
			      uint8_t *iir_init_value);

int maxm86161_set_pf_threshold_sigma(struct maxm86161_dev *dev,
				     uint8_t threshold_sigma_mult);

int maxm86161_get_pf_threshold_sigma(struct maxm86161_dev *dev,
				     uint8_t *threshold_sigma_mult);

/* Die Temperature */

int maxm86161_read_die_temperature(struct maxm86161_dev *dev,
				   int32_t *temperature);

/* DAC Calibration */

int maxm86161_start_calibration(struct maxm86161_dev *dev);

int maxm86161_get_calibration_status(struct maxm86161_dev *dev,
				     bool *complete, bool *oor);

/* Burst Mode */

int maxm86161_set_burst_enable(struct maxm86161_dev *dev, bool enable);

int maxm86161_set_burst_rate(struct maxm86161_dev *dev,
			     enum maxm86161_burst_rate rate);

int maxm86161_set_burst_mode(struct maxm86161_dev *dev, bool enable,
			     enum maxm86161_burst_rate rate);

/* PPG Sync */

int maxm86161_set_timestamp_enable(struct maxm86161_dev *dev, bool enable);

int maxm86161_set_dac_code_tag(struct maxm86161_dev *dev, bool enable);

int maxm86161_set_gpio_ctrl(struct maxm86161_dev *dev,
			    enum maxm86161_gpio_ctrl ctrl);

int maxm86161_force_sync(struct maxm86161_dev *dev);

/* SHA-256 Authentication */

int maxm86161_sha_enable(struct maxm86161_dev *dev, bool enable);

int maxm86161_sha_start(struct maxm86161_dev *dev, uint8_t cmd);

int maxm86161_sha_write_challenge(struct maxm86161_dev *dev,
				  uint8_t *challenge, uint8_t len);

int maxm86161_sha_read_response(struct maxm86161_dev *dev,
				uint8_t *response, uint8_t len);

/* Memory Access */

int maxm86161_memory_read(struct maxm86161_dev *dev, uint16_t addr,
			  uint8_t *data);

int maxm86161_memory_write(struct maxm86161_dev *dev, uint16_t addr,
			   uint8_t data);

#endif /* __MAXM86161_H__ */
