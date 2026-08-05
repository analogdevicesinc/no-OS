/***************************************************************************//**
 *   @file   ad5259.h
 *   @brief  Header file for the ad5259 digital potentiometer drivers
********************************************************************************
Copyright 2025(c) Analog Devices, Inc.

SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef AD5259_H_
#define AD5259_H_

#include "../common/dpot.h"
#include "no_os_spi.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

/**
 * @struct ad5259_dpot_init_param.
 * @brief ad5259 digital potentiometer init parameters.
 */
struct ad5259_dpot_init_param {
	/* Interface type */
	enum dpot_intf_type eIntfType;
	/* I2C init parameters */
	struct no_os_i2c_init_param *i2c_init;
	/* Digipot operating mode.*/
	enum  dpot_operating_mode operating_mode;
};

/**
 * @struct ad5259_dpot_dev.
 * @brief ad5259 digital potentiometer device descriptor parameters.
 */
struct ad5259_dpot_dev {
	enum dpot_intf_type intf_type;
	/* I2C descriptor */
	struct no_os_i2c_desc *i2c_desc;
};
/* ad5259 digital potentiometer ops */
int ad5259_dpot_init(struct dpot_init_param *init_params,
		     struct dpot_dev **desc);
int ad5259_dpot_remove(struct dpot_dev *desc);

int ad5259_dpot_chn_read(struct dpot_dev *desc,
			 enum dpot_chn_type chn, uint8_t *data);
int ad5259_dpot_chn_write(struct dpot_dev *desc,
			  enum dpot_chn_type chn, uint8_t data);
int ad5259_dpot_nvm_read(struct dpot_dev *desc,
			 enum dpot_chn_type chn, uint8_t *data);
int ad5259_dpot_nvm_write(struct dpot_dev *desc,
			  enum dpot_chn_type chn, uint8_t data);
int ad5259_dpot_copy_rdac_to_nvm(struct dpot_dev *desc,
				 enum dpot_chn_type chn);
int ad5259_dpot_copy_nvm_to_rdac(struct dpot_dev *desc,
				 enum dpot_chn_type chn);
#endif	// ad5259_H_
