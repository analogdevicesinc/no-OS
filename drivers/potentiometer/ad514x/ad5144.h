/***************************************************************************//**
 *   @file   ad5144.h
 *   @brief  Header file for the AD5144 digital potentiometer drivers
********************************************************************************
Copyright 2025(c) Analog Devices, Inc.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of Analog Devices, Inc. nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef AD5144_H_
#define AD5144_H_

#include "../common/dpot.h"
#include "no_os_spi.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_util.h"
#include "ad51xx_cmd.h"
/* Number of channels in Potentiometer mode of operation*/
#define AD5144_NUM_POTENTIOMETER_CH 4
/* Number of channels in Linear gain  mode of operation*/
#define AD5144_NUM_LINEAR_GAIN_CH   8
/*Required delay after resetting the AD5144 */
#define AD5144_RESET_DELAY  30



/**
 * @struct ad5144_dpot_init_param.
 * @brief AD5144 digital potentiometer init parameters.
 */
struct ad5144_dpot_init_param {
	/* Interface type */
	enum dpot_intf_type eintf_type;
	/* SPI init parameters */
	struct no_os_spi_init_param *spi_init;
	/* I2C init parameters */
	struct no_os_i2c_init_param *i2c_init;
	/* Reset GPIO init parameters */
	struct no_os_gpio_init_param *reset_gpio_init;
	/* Digital interface select GPIO init parameters */
	struct no_os_gpio_init_param *dis_gpio_init;
	/* LRDAC GPIO init parameters */
	struct no_os_gpio_init_param *lrdac_gpio_init;
	/* EEPROM Write Protect GPIO init parameters */
	struct no_os_gpio_init_param *wp_gpio_init;
	/* INDEP GPIO init parameters */
	struct no_os_gpio_init_param *indep_gpio_init;
	/* Shutdown enable flag for status in the potentiometer mode*/
	bool shutdown_enable_pt[AD5144_NUM_POTENTIOMETER_CH];
	/* Shutdown enable flag for status in the Linear gain  mode*/
	bool shutdown_enable_lg[AD5144_NUM_LINEAR_GAIN_CH];
	/* Operating mode */
	enum dpot_operating_mode eoperating_mode;

};

/**
 * @struct ad5144_dpot_dev.
 * @brief AD5144 digital potentiometer device descriptor parameters.
 */
struct ad5144_dpot_dev {
	enum dpot_intf_type intf_type;
	/* SPI descriptor */
	struct no_os_spi_desc *spi_desc;
	/* I2C descriptor */
	struct no_os_i2c_desc *i2c_desc;
	/* Reset GPIO desc */
	struct no_os_gpio_desc *reset_gpio_desc;
	/* Digital interface select GPIO descriptor */
	struct no_os_gpio_desc *dis_gpio_desc;
	/* LRDAC GPIO descriptor */
	struct no_os_gpio_desc *lrdac_gpio_desc;
	/* EEPROM Write Protect descriptor */
	struct no_os_gpio_desc *wp_gpio_desc;
	/* INDEP GPIO init parameters */
	struct no_os_gpio_desc *indep_gpio_desc;
	/* Shutdown enable flag */
	bool shutdown_enable[AD5144_NUM_LINEAR_GAIN_CH];
	/* Operating mode */
	enum dpot_operating_mode eoperating_mode;

};
/* AD5144 digital potentiometer ops */
int ad5144_dpot_init(struct dpot_init_param *init_params,
		     struct dpot_dev **desc);
int ad5144_dpot_remove(struct dpot_dev *desc);
int ad5144_dpot_reset(struct dpot_dev *desc);
int ad5144_dpot_shutdown(struct dpot_dev *desc,
			 enum dpot_chn_type chn, bool shutdown_enable);
int ad5144_dpot_set_operating_mode(struct dpot_dev *desc,
				   enum dpot_operating_mode operating_mode);
int ad5144_dpot_input_reg_read(struct dpot_dev *desc,
			       enum dpot_chn_type chn, uint8_t *data);
int ad5144_dpot_input_reg_write(struct dpot_dev *desc,
				enum dpot_chn_type chn, uint8_t data);
int ad5144_dpot_sw_lrdac_update(struct dpot_dev *desc, enum dpot_chn_type chn);
int ad5144_dpot_chn_read(struct dpot_dev *desc,
			 enum dpot_chn_type chn, uint8_t *data);
int ad5144_dpot_chn_write(struct dpot_dev *desc,
			  enum dpot_chn_type chn, uint8_t data);
int ad5144_dpot_nvm_read(struct dpot_dev *desc,
			 enum dpot_chn_type chn, uint8_t *data);
int ad5144_dpot_nvm_write(struct dpot_dev *desc,
			  enum dpot_chn_type chn, uint8_t data);
int ad5144_dpot_copy_rdac_to_nvm(struct dpot_dev *desc, enum dpot_chn_type chn);
int ad5144_dpot_copy_nvm_to_rdac(struct dpot_dev *desc, enum dpot_chn_type chn);
int ad5144_dpot_rdac_linear_update(struct dpot_dev *desc,
				   enum dpot_chn_type chn,
				   enum dpot_rdac_linear_status status);
int ad5144_dpot_rdac_6db_update(struct dpot_dev *desc, enum dpot_chn_type chn,
				enum dpot_rdac_6db_status status);
int ad5144_dpot_send_cmd(struct dpot_dev *desc,
			 struct dpot_command *cmd);

#endif	// AD5144_H_
