/***************************************************************************//**
 *   @file   ad469x.h
 *   @brief  Header file for ad469x Driver.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#ifndef SRC_AD469X_H_
#define SRC_AD469X_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "spi_engine.h"
#include "clk_axi_clkgen.h"
#include "pwm.h"
#include "gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/* AD469x registers */
#define AD469x_REG_IF_CONFIG_A		0x000
#define AD469x_REG_IF_CONFIG_B		0x001
#define AD469x_REG_DEVICE_TYPE		0x003
#define AD469x_REG_DEVICE_ID_L		0x004
#define AD469x_REG_DEVICE_ID_H		0x005
#define AD469x_REG_SCRATCH_PAD		0x00A
#define AD469x_REG_VENDOR_L		0x00C
#define AD469x_REG_VENDOR_H		0x00D
#define AD469x_REG_LOOP_MODE		0x00E
#define AD469x_REG_IF_CONFIG_C		0x010
#define AD469x_REG_IF_STATUS		0x011
#define AD469x_REG_STATUS		0x014
#define AD469x_REG_ALERT_STATUS1	0x015
#define AD469x_REG_ALERT_STATUS2	0x016
#define AD469x_REG_ALERT_STATUS3	0x017
#define AD469x_REG_ALERT_STATUS4	0x018
#define AD469x_REG_CLAMP_STATUS1	0x01A
#define AD469x_REG_CLAMP_STATUS2	0x01B
#define AD469x_REG_SETUP		0x020
#define AD469x_REG_REF_CTRL		0x021
#define AD469x_REG_SEQ_CTRL		0x022
#define AD469x_REG_AC_CTRL		0x023
#define AD469x_REG_STD_SEQ_CONFIG	0x024
#define AD469x_REG_GPIO_CTRL		0x026
#define AD469x_REG_GP_MODE		0x027
#define AD469x_REG_GPIO_STATE		0x028
#define AD469x_REG_TEMP_CTRL		0x029
#define AD469x_REG_CONFIG_IN(x)		((x & 0x0F) | 0x30)
#define AD469x_REG_AS_SLOT(x)		((x & 0x7F) | 0x100)

/* 5-bit SDI Conversion Mode Commands */
#define AD469x_CMD_REG_CONFIG_MODE		(0x0A << 3)
#define AD469x_CMD_SEL_TEMP_SNSOR_CH		(0x0F << 3)
#define AD469x_CMD_CONFIG_CH_SEL(x)		((0x10 | (0x0F & x)) << 3)

/* AD469x_REG_SETUP */
#define AD469x_SETUP_IF_MODE_MASK		(0x01 << 2)
#define AD469x_SETUP_IF_MODE_CONV		(0x01 << 2)
#define AD469x_SETUP_CYC_CTRL_MASK		(0x01 << 1)
#define AD469x_SETUP_CYC_CTRL_SINGLE(x)		((x & 0x01) << 1)

/* AD469x_REG_GP_MODE */
#define AD469x_GP_MODE_BUSY_GP_EN_MASK		(0x01 << 1)
#define AD469x_GP_MODE_BUSY_GP_EN(x)		((x & 0x01) << 1)
#define AD469x_GP_MODE_BUSY_GP_SEL_MASK		(0x01 << 4)
#define AD469x_GP_MODE_BUSY_GP_SEL(x)		((x & 0x01) << 4)

/* AD469x_REG_SEQ_CTRL */
#define AD469x_SEQ_CTRL_STD_SEQ_EN_MASK		(0x01 << 7)
#define AD469x_SEQ_CTRL_STD_SEQ_EN(x)		((x & 0x01) << 7)
#define AD469x_SEQ_CTRL_NUM_SLOTS_AS_MASK	(0x7f << 0)
#define AD469x_SEQ_CTRL_NUM_SLOTS_AS(x)		((x & 0x7f) << 0)

/* AD469x_REG_TEMP_CTRL */
#define AD469x_REG_TEMP_CTRL_TEMP_EN_MASK	(0x01 << 0)
#define AD469x_REG_TEMP_CTRL_TEMP_EN(x)		((x & 0x01) << 0)

/* AD469x_REG_AS_SLOT */
#define AD469x_REG_AS_SLOT_INX(x)		((x & 0x0f) << 0)

/* AD469x_REG_IF_CONFIG_C */
#define AD469x_REG_IF_CONFIG_C_MB_STRICT_MASK	(0x01 << 5)
#define AD469x_REG_IF_CONFIG_C_MB_STRICT(x)	((x & 0x01) << 5)

/* AD469x_REG_CONFIG_INn */
#define AD469x_REG_CONFIG_IN_OSR_MASK		(0x03 << 0)
#define AD469x_REG_CONFIG_IN_OSR(x)		((x & 0x03) << 0)
#define AD469x_REG_CONFIG_IN_HIZ_EN_MASK	(0x01 << 3)
#define AD469x_REG_CONFIG_IN_HIZ_EN(x)		((x & 0x01) << 3)
#define AD469x_REG_CONFIG_IN_PAIR_MASK		(0x03 << 4)
#define AD469x_REG_CONFIG_IN_PAIR(x)		((x & 0x03) << 4)
#define AD469x_REG_CONFIG_IN_MODE_MASK		(0x01 << 6)
#define AD469x_REG_CONFIG_IN_MODE(x)		((x & 0x01) << 6)
#define AD469x_REG_CONFIG_IN_TD_EN_MASK		(0x01 << 7)
#define AD469x_REG_CONFIG_IN_TD_EN(x)		((x & 0x01) << 7)

#define AD469x_CHANNEL(x)			(BIT(x) & 0xFFFF)
#define AD469x_CHANNEL_NO			16
#define AD469x_SLOTS_NO				0x80
#define AD469x_CHANNEL_TEMP			16

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @enum ad469x_channel_sequencing
 * @brief Channel sequencing modes
 */
enum ad469x_channel_sequencing {
	/** Single cycle read */
	AD469x_single_cycle,
	/** Two cycle read */
	AD469x_two_cycle,
	/** Sequence trough channels, standard mode */
	AD469x_standard_seq,
	/** Sequence trough channels, advanced mode */
	AD469x_advanced_seq,
};

/**
  * @enum ad469x_busy_gp_sel
  * @brief Busy state, possible general purpose pin selections
  */
enum ad469x_busy_gp_sel {
	/** Busy on gp0 */
	AD469x_busy_gp0 = 0,
	/** Busy on gp3 */
	AD469x_busy_gp3 = 1,
};

/**
  * @enum ad469x_reg_access
  * @brief Register access modes
  */
enum ad469x_reg_access {
	AD469x_BYTE_ACCESS,
	AD469x_WORD_ACCESS,
};

/**
  * @enum ad469x_supported_dev_ids
  * @brief Supported devices
  */
enum ad469x_supported_dev_ids {
	ID_AD4695,
	ID_AD4696,
	ID_AD4697,
};

/**
  * @enum ad469x_osr_ratios
  * @brief Supported oversampling ratios
  */
enum ad469x_osr_ratios {
	AD469x_OSR_1,
	AD469x_OSR_4,
	AD469x_OSR_16,
	AD469x_OSR_64
};

/**
 * @struct ad469x_init_param
 * @brief  Structure containing the init parameters needed by the ad469x device
 */
struct ad469x_init_param {
	/* SPI */
	spi_init_param		*spi_init;
	/* SPI module offload init */
	struct spi_engine_offload_init_param *offload_init_param;
	/* PWM generator init structure */
	struct pwm_init_param	*trigger_pwm_init;
	/** RESET GPIO initialization structure. */
	struct gpio_init_param	*gpio_resetn;
	/* Clock gen for hdl design init structure */
	struct axi_clkgen_init	*clkgen_init;
	/* Clock generator rate */
	uint32_t		axi_clkgen_rate;
	/* Register access speed */
	uint32_t		reg_access_speed;
	/* Register data width */
	uint8_t		reg_data_width;
	/* Capture data width */
	uint8_t		capture_data_width;
	/* Device Settings */
	enum ad469x_supported_dev_ids dev_id;
	/** Invalidate the Data cache for the given address range */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
};

/**
 * @struct ad469x_dev
 * @brief  Structure representing an ad469x device
 */
struct ad469x_dev {
	/* SPI descriptor */
	spi_desc		*spi_desc;
	/* Clock gen for hdl design structure */
	struct axi_clkgen	*clkgen;
	/* Trigger conversion PWM generator descriptor */
	struct pwm_desc		*trigger_pwm_desc;
	/* SPI module offload init */
	struct spi_engine_offload_init_param *offload_init_param;
	/* Register access speed */
	uint32_t		reg_access_speed;
	/* Register data width */
	uint8_t		reg_data_width;
	/* Capture data width */
	uint8_t		capture_data_width;
	/* Device Settings */
	enum ad469x_supported_dev_ids dev_id;
	/** RESET GPIO handler. */
	struct gpio_desc	*gpio_resetn;
	/** Invalidate the Data cache for the given address range */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
	/** Current channel sequence */
	enum ad469x_channel_sequencing ch_sequence;
	/** OSR resolution corresponding to each channel, when advanced
	 * sequencer is selected. */
	enum ad469x_osr_ratios adv_seq_osr_resol[AD469x_CHANNEL_NO];
	/** Channel slots for advanced sequencer */
	uint8_t ch_slots[AD469x_SLOTS_NO];
	/** Temperature enabled for standard and advanced sequencer if set. */
	bool temp_enabled;
	/** Number of active channel slots, for advanced sequencer */
	uint8_t num_slots;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Read device register. */
int32_t ad469x_spi_reg_read(struct ad469x_dev *dev,
			    uint16_t reg_addr,
			    uint8_t *reg_data);

/* Write device register */
int32_t ad469x_spi_reg_write(struct ad469x_dev *dev,
			     uint16_t reg_addr,
			     uint8_t reg_data);

/* Read from device using a mask */
int32_t ad469x_spi_read_mask(struct ad469x_dev *dev,
			     uint16_t reg_addr,
			     uint8_t mask,
			     uint8_t *data);

/* Write to device using a mask */
int32_t ad469x_spi_write_mask(struct ad469x_dev *dev,
			      uint16_t reg_addr,
			      uint8_t mask,
			      uint8_t data);

/* Read data from device */
int32_t ad469x_read_data(struct ad469x_dev *dev,
			 uint8_t channel,
			 uint32_t *buf,
			 uint16_t samples);

/* Read from device when converter has the channel sequencer activated */
int32_t ad469x_seq_read_data(struct ad469x_dev *dev,
			     uint32_t *buf,
			     uint16_t samples);

/* Set channel sequence */
int32_t ad469x_set_channel_sequence(struct ad469x_dev *dev,
				    enum ad469x_channel_sequencing seq);

/* Configure standard sequencer enabled channels */
int32_t ad469x_std_sequence_ch(struct ad469x_dev *dev,
			       uint16_t ch_mask);

/* Configure advanced sequencer number of slots */
int32_t ad469x_adv_sequence_set_num_slots(struct ad469x_dev *dev,
		uint8_t num_slots);

/* Advanced sequencer, assign channel to a slot */
int32_t ad469x_adv_sequence_set_slot(struct ad469x_dev *dev,
				     uint8_t slot,
				     uint8_t channel);

/* Enable temperature read at the end of the sequence, for standard and */
int32_t ad469x_sequence_enable_temp(struct ad469x_dev *dev);

/* Disable temperature read at the end of the sequence, for standard and */
int32_t ad469x_sequence_disable_temp(struct ad469x_dev *dev);

/* Configure over sampling ratio in advanced sequencer mode */
int32_t ad469x_adv_seq_osr(struct ad469x_dev *dev, uint16_t ch,
			   enum ad469x_osr_ratios ratio);

/* Configure over sampling ratio in standard sequencer mode */
int32_t ad469x_std_seq_osr(struct ad469x_dev *dev,
			   enum ad469x_osr_ratios ratio);

/* Enter conversion mode */
int32_t ad469x_enter_conversion_mode(struct ad469x_dev *dev);

/* Exit conversion mode */
int32_t ad469x_exit_conversion_mode(struct ad469x_dev *dev);

/* Initialize the device. */
int32_t ad469x_init(struct ad469x_dev **device,
		    struct ad469x_init_param *init_param);

/* Remove the device and release resources. */
int32_t ad469x_remove(struct ad469x_dev *dev);

#endif /* SRC_AD469X_H_ */
