/*******************************************************************************
 *   @file   ad5143.h
 *   @brief  Header file for the AD5143 digital potentiometer drivers
********************************************************************************
Copyright 2025(c) Analog Devices, Inc.

SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef AD5143_H_
#define AD5143_H_

#include "../common/dpot.h"
#include "no_os_i2c.h"
#include "no_os_util.h"
#include "ad51xx_cmd.h"

/**
 * @struct ad5143_dpot_init_param.
 * @brief AD5143 digital potentiometer init parameters.
 */
struct ad5143_dpot_init_param {
	/* I2C init parameters */
	struct no_os_i2c_init_param *i2c_init;
	/* Shutdown enable flag for  potentiometer mode */
	bool shutdown_enable_pt[4];
	/* Shutdown enable flag for  potentiometer mode */
	bool shutdown_enable_lg[8];

	/* Operating mode */
	enum dpot_operating_mode eoperating_mode;
};

/**
 * @struct ad5143_dpot_dev.
 * @brief AD5143 digital potentiometer device descriptor parameters.
 */
struct ad5143_dpot_dev {
	/* I2C descriptor */
	struct no_os_i2c_desc *i2c_desc;
	/* Shutdown enable flag */
	bool shutdown_enable[4];
	/* Operating mode */
	enum dpot_operating_mode operating_mode;
};

/* AD5143 digital potentiometer ops */
extern const struct dpot_ops ad5143_dpot_ops;

int ad5143_dpot_init(struct dpot_init_param *init_params,
		     struct dpot_dev **desc);
int ad5143_dpot_remove(struct dpot_dev *desc);
int ad5143_dpot_reset(struct dpot_dev *desc);
int ad5143_dpot_shutdown(struct dpot_dev *desc,
			 enum dpot_chn_type chn, bool shutdown_enable);
int ad5143_dpot_set_operating_mode(struct dpot_dev *desc,
				   enum dpot_operating_mode operating_mode);
int ad5143_dpot_input_reg_read(struct dpot_dev *desc,
			       enum dpot_chn_type chn, uint8_t *data);
int ad5143_dpot_input_reg_write(struct dpot_dev *desc,
				enum dpot_chn_type chn, uint8_t data);
int ad5143_dpot_sw_lrdac_update(struct dpot_dev *desc, enum dpot_chn_type chn);
int ad5143_dpot_chn_read(struct dpot_dev *desc,
			 enum dpot_chn_type chn, uint8_t *data);
int ad5143_dpot_chn_write(struct dpot_dev *desc,
			  enum dpot_chn_type chn, uint8_t data);
int ad5143_dpot_nvm_read(struct dpot_dev *desc,
			 enum dpot_chn_type chn, uint8_t *data);
int ad5143_dpot_nvm_write(struct dpot_dev *desc,
			  enum dpot_chn_type chn, uint8_t data);
int ad5143_dpot_copy_rdac_to_nvm(struct dpot_dev *desc, enum dpot_chn_type chn);
int ad5143_dpot_copy_nvm_to_rdac(struct dpot_dev *desc, enum dpot_chn_type chn);
int ad5143_dpot_rdac_linear_update(struct dpot_dev *desc,
				   enum dpot_chn_type chn,
				   enum dpot_rdac_linear_status status);
int ad5143_dpot_rdac_6db_update(struct dpot_dev *desc, enum dpot_chn_type chn,
				enum dpot_rdac_6db_status status);
int ad5143_dpot_send_cmd(struct dpot_dev *desc,
			 struct dpot_command *cmd);

#endif	// AD5143_H_
