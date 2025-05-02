/*******************************************************************************
 *   @file   dpot.h
 *   @brief  Header file for the digital potentiometer drivers
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
*******************************************************************************/
#ifndef DPOT_H_
#define DPOT_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "no_os_error.h"

/**
 * @brief Digital potentiometer device IDs.
 */
enum dpot_dev_id {
	DEV_AD5121,
	DEV_AD5122,
	DEV_AD5122A,
	DEV_AD5123,
	DEV_AD5124,
	DEV_AD5141,
	DEV_AD5142,
	DEV_AD5142A,
	DEV_AD5143,
	DEV_AD5144,
	DEV_AD5160,
	DEV_AD5161,
	DEV_AD5165,
	DEV_AD5171,
	DEV_AD5241,
	DEV_AD5242,
	DEV_AD5245,
	DEV_AD5246,
	DEV_AD5258,
	DEV_AD5259,
	DEV_AD5273
};
#define DPOT_NUM_SUPPORTED_DEVICES DEV_AD5273+1

/**
 * @brief digital potentiometer interface type.
 */
enum dpot_intf_type {
	/* SPI digital interface */
	AD_SPI_INTERFACE = 0x1,
	/* I2C digital interface */
	AD_I2C_INTERFACE = 0x2
};
/**
  * @brief Digital potentiometer channel type.
 */
enum dpot_chn_type {
	/* Channels applicable in potentiometer mode */
	DPOT_CHN_RDAC1, /* Use this channel for single channel devices */
	DPOT_CHN_RDAC2,
	DPOT_CHN_RDAC3,
	DPOT_CHN_RDAC4,
	/* Channel applicable in linear gain setting mode */
	/* Use R_AW1 and R_WB1 for single channel devices */
	DPOT_CHN_R_AW1,
	DPOT_CHN_R_WB1,
	DPOT_CHN_R_AW2,
	DPOT_CHN_R_WB2,
	DPOT_CHN_R_AW3,
	DPOT_CHN_R_WB3,
	DPOT_CHN_R_AW4,
	DPOT_CHN_R_WB4,
	NUM_OF_DPOT_CHN
};


/**
  * @brief Digital potentiometer operating mode.
 * @note Operating mode is applicable to only certain
 *       generics of digipots and not all.
 */
enum dpot_operating_mode {
	/* Potentiometer mode */
	DPOT_POTENTIOMETER_MODE,
	/* Linear gain setting mode */
	DPOT_LINEAR_GAIN_SETTING_MODE
};

/**
  * @brief Digital potentiometer RDAC linear update status.
 * @note Linear update is applicable to only certain
 *       generics of digipots and not all.
 */
enum dpot_rdac_linear_status {
	/* Linear increment */
	DPOT_RDAC_LINEAR_INCREMENT,
	/* Linear decrement */
	DPOT_RDAC_LINEAR_DECREMENT
};

/**
  * @brief Digital potentiometer RDAC 6dB update status.
 * @note 6dB update is applicable to only certain
 *       generics of digipots and not all.
 */
enum dpot_rdac_6db_status {
	/* 6dB increment */
	DPOT_RDAC_6DB_INCREMENT,
	/* 6dB decrement */
	DPOT_RDAC_6DB_DECREMENT
};

/**
 * @struct dpot_command.
 * @brief Digital potentiometer command structure.
 */
struct dpot_command {
	/* Control bits (DB15:DB12) */
	uint8_t control;
	/* Address bits (DB11:DB8) */
	uint8_t address;
	/* Data bits (DB7:DB0) */
	uint8_t data;
	/* Readback needed (optional response to command) */
	bool is_readback;
	/* Response (optional, if 'is_readback' is set true) */
	uint8_t response;
};
/**
 * @struct dpot_init_param.
 * @brief Digital potentiometer init parameters.
 */
struct dpot_init_param {
	/* Device Interface type */
	enum dpot_intf_type intf_type;
	/* Active device ID */
	enum dpot_dev_id device_id;
	/* Pointer to digipots ops (APIs) structure */
	const struct dpot_ops *dpot_ops;
	/* Device specific parameters */
	void *extra;
	/* Device operating mode  */
	enum dpot_operating_mode operating_mode;
};


/**
 * @struct dpot_dev.
 * @brief Digital potentiometer device descriptor parameters.
 */
struct dpot_dev {
	/* Active device ID */
	enum dpot_dev_id device_id;
	/* Pointer to digipots ops (APIs) structure */
	struct dpot_ops *dpot_ops;
	/* Device specific parameters */
	void *extra;
};

/**
 * @struct dpot_ops.
 * @brief Digital potentiometer common APIs.
 */
struct dpot_ops {
	/* Initialize the digital potentiometer */
	int(*dpot_init)(struct dpot_init_param *init_params, struct dpot_dev **desc);
	/* Free memory allocated for the digital potentiometer device structure */
	int(*dpot_remove)(struct dpot_dev *desc);
	/* Reset digital potentiometer */
	int(*dpot_reset)(struct dpot_dev *desc);
	/* Shutdown digital potentiometer channel */
	int(*dpot_shutdown)(struct dpot_dev *desc, enum dpot_chn_type chn,
			    bool shutdown_enable);
	/* Set the digital potentiometer operating mode */
	int(*dpot_set_operating_mode)(struct dpot_dev *desc,
				      enum dpot_operating_mode operating_mode);
	/* Read the digital potentiometer input register */
	int(*dpot_input_reg_read)(struct dpot_dev *desc,
				  enum dpot_chn_type chn, uint8_t *data);
	/* Write to digital potentiometer input register */
	int(*dpot_input_reg_write)(struct dpot_dev *desc,
				   enum dpot_chn_type chn, uint8_t data);
	/* Copy input register to RDAC (software LRDAC) */
	int(*dpot_sw_lrdac_update)(struct dpot_dev *desc, enum dpot_chn_type chn);
	/* Read digital potentiometer channel */
	int(*dpot_chn_read)(struct dpot_dev *desc,
			    enum dpot_chn_type chn, uint8_t *data);
	/* Write to digital potentiometer channel */
	int(*dpot_chn_write)(struct dpot_dev *desc,
			     enum dpot_chn_type chn, uint8_t data);
	/* Read NVM (non-volatile) memory of digital potentiometer */
	int(*dpot_nvm_read)(struct dpot_dev *desc,
			    enum dpot_chn_type chn, uint8_t *data);
	/* Write to NVM (non-volatile) memory of digital potentiometer */
	int(*dpot_nvm_write)(struct dpot_dev *desc,
			     enum dpot_chn_type chn, uint8_t data);
	/* Copy RDAC register to NVM/EEPROM */
	int(*dpot_copy_rdac_to_nvm)(struct dpot_dev *desc, enum dpot_chn_type chn);
	/* Copy NVM/EEPROM data to RDAC */
	int (*dpot_copy_nvm_to_rdac)(struct dpot_dev *desc, enum dpot_chn_type chn);
	/* Perform RDAC linear increment/decrement */
	int(*dpot_rdac_linear_update)(struct dpot_dev *desc, enum dpot_chn_type chn,
				      enum dpot_rdac_linear_status status);
	/* Perform RDAC 6dB increment/decrement */
	int(*dpot_rdac_6db_update)(struct dpot_dev *desc, enum dpot_chn_type chn,
				   enum dpot_rdac_6db_status status);
	/* Send digital potentiometer command */
	int(*dpot_send_cmd)(struct dpot_dev *desc, struct dpot_command *cmd);
	int(*dpot_tolerance_read)(struct dpot_dev *desc, enum dpot_chn_type chn,
				  uint8_t *data);

	int(*dpot_enable_top_bottom_scale)(struct dpot_dev *desc,
					   enum dpot_chn_type chn,
					   bool isTopScale,
					   uint8_t nEnter);
	int(*dpot_set_mid_scale)(struct dpot_dev *desc,
				 enum dpot_chn_type chn,
				 bool shutdown_enable);
};

/* Digipots APIS  */

int dpot_init(struct dpot_dev **desc, struct dpot_init_param  *pDpotInitParam);
int dpot_remove(struct dpot_dev *desc);
int dpot_reset(struct dpot_dev *desc);
int dpot_shutdown(struct dpot_dev *desc,
		  enum dpot_chn_type chn, bool shutdown_enable);
int dpot_set_operating_mode(struct dpot_dev *desc,
			    enum dpot_operating_mode operating_mode);
int dpot_input_reg_read(struct dpot_dev *desc,
			enum dpot_chn_type chn, uint8_t *data);
int dpot_input_reg_write(struct dpot_dev *desc,
			 enum dpot_chn_type chn, uint8_t data);
int dpot_sw_lrdac_update(struct dpot_dev *desc, enum dpot_chn_type chn);
int dpot_chn_read(struct dpot_dev *desc,
		  enum dpot_chn_type chn, uint8_t *data);
int dpot_chn_write(struct dpot_dev *desc,
		   enum dpot_chn_type chn, uint8_t data);
int dpot_nvm_read(struct dpot_dev *desc,
		  enum dpot_chn_type chn, uint8_t *data);
int dpot_nvm_write(struct dpot_dev *desc,
		   enum dpot_chn_type chn, uint8_t data);
int dpot_copy_rdac_to_nvm(struct dpot_dev *desc, enum dpot_chn_type chn);
int dpot_copy_nvm_to_rdac(struct dpot_dev *desc, enum dpot_chn_type chn);
int dpot_rdac_linear_update(struct dpot_dev *desc, enum dpot_chn_type chn,
			    enum dpot_rdac_linear_status status);
int dpot_rdac_6db_update(struct dpot_dev *desc, enum dpot_chn_type chn,
			 enum dpot_rdac_6db_status status);
int dpot_send_cmd(struct dpot_dev *desc, struct dpot_command *cmd);
int dpot_enable_top_scale(struct dpot_dev *desc, enum dpot_chn_type chn,
			  uint8_t nEnter);
int dpot_enable_bottom_scale(struct dpot_dev *desc, enum dpot_chn_type chn,
			     uint8_t nEnter);
int dpot_tolerance_read(struct dpot_dev *desc, enum dpot_chn_type chn,
			uint8_t *data);
int dpot_set_mid_scale(struct dpot_dev *desc, enum dpot_chn_type chn,
		       bool shutdown_enable);
#endif	// DPOT_H_
