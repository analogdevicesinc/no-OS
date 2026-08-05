/***************************************************************************//**
 *   @file   ad5171.h
 *   @brief  Header file for the ad5171 digital potentiometer drivers
********************************************************************************
Copyright 2025(c) Analog Devices, Inc.

SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef AD5171_H_
#define AD5171_H_

#include "../common/dpot.h"
#include "no_os_spi.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

#define ENABLE_OTP_PROGRAMMING 0x80
/**
 * @struct ad5171_dpot_init_param.
 * @brief ad5171 digital potentiometer init parameters.
 */
struct ad5171_dpot_init_param {
	/* Interface type */
	enum dpot_intf_type eIntfType;
	/* I2C init parameters */
	struct no_os_i2c_init_param *i2c_init;
};
/**
 * @struct ad5171_dpot_dev.
 * @brief ad1271/ad5273 digital potentiometer device descriptor parameters.
 */
struct ad5171_dpot_dev {
	enum dpot_intf_type intf_type;
	/* I2C descriptor */
	struct no_os_i2c_desc *i2c_desc;
};

/* AD5171 digital potentiometer ops */
int ad5171_dpot_init(struct dpot_init_param *init_params,
		     struct dpot_dev **desc);
int ad5171_dpot_remove(struct dpot_dev *desc);

int ad5171_dpot_chn_read(struct dpot_dev *desc,
			 enum dpot_chn_type chn, uint8_t *data);
int ad5171_dpot_chn_write(struct dpot_dev *desc,
			  enum dpot_chn_type chn, uint8_t data);
int ad5171_dpot_send_cmd(struct dpot_dev *desc,
			 struct dpot_command *cmd);
int ad5171_dpot_shutdown(struct dpot_dev *desc,	enum dpot_chn_type chn,
			 bool shutdown_enable);

#endif	// AD5171_H_
