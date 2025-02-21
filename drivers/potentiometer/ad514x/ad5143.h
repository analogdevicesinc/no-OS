/***************************************************************************//**
 *   @file   ad5143.h
 *   @brief  Header file for the AD5143 digital potentiometer drivers
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef AD5143_H_
#define AD5143_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "dpot.h"
#include "no_os_i2c.h"
#include "no_os_util.h"
#include "ad51xx_cmd.h"
/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

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
