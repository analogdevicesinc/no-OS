/***************************************************************************//**
 *   @file   ad7746.h
 *   @brief  Header file of AD7746 Driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#ifndef _AD7746_H_
#define _AD7746_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_util.h"
#include "no_os_i2c.h"

/* AD7746 Slave Address */
#define AD7746_ADDRESS			0x48

/* AD7746 Reset command */
#define AD7746_RESET_CMD		0xBF

/* AD7746 Register Definition */
#define AD7746_REG_STATUS		0u
#define AD7746_REG_CAP_DATA_HIGH	1u
#define AD7746_REG_CAP_DATA_MID		2u
#define AD7746_REG_CAP_DATA_LOW		3u
#define AD7746_REG_VT_DATA_HIGH		4u
#define AD7746_REG_VT_DATA_MID		5u
#define AD7746_REG_VT_DATA_LOW		6u
#define AD7746_REG_CAP_SETUP		7u
#define AD7746_REG_VT_SETUP		8u
#define AD7746_REG_EXC_SETUP		9u
#define AD7746_REG_CFG			10u
#define AD7746_REG_CAPDACA		11u
#define AD7746_REG_CAPDACB		12u
#define AD7746_REG_CAP_OFFH		13u
#define AD7746_REG_CAP_OFFL		14u
#define AD7746_REG_CAP_GAINH		15u
#define AD7746_REG_CAP_GAINL		16u
#define AD7746_REG_VOLT_GAINH		17u
#define AD7746_REG_VOLT_GAINL		18u

#define AD7746_NUM_REGISTERS		(AD7746_REG_VOLT_GAINL + 1u)

/* AD7746_REG_STATUS bits */
#define AD7746_STATUS_EXCERR_MSK	NO_OS_BIT(3)
#define AD7746_STATUS_RDY_MSK		NO_OS_BIT(2)
#define AD7746_STATUS_RDYVT_MSK		NO_OS_BIT(1)
#define AD7746_STATUS_RDYCAP_MSK	NO_OS_BIT(0)

/* AD7746_REG_CAP_SETUP bits */
#define AD7746_CAPSETUP_CAPEN_MSK	NO_OS_BIT(7)
#define AD7746_CAPSETUP_CIN2_MSK	NO_OS_BIT(6)
#define AD7746_CAPSETUP_CAPDIFF_MSK	NO_OS_BIT(5)
#define AD7746_CAPSETUP_CAPCHOP_MSK	NO_OS_BIT(0)

/* AD7746_REG_VT_SETUP bits */
#define AD7746_VTSETUP_VTEN_MSK		NO_OS_BIT(7)
#define AD7746_VTSETUP_VTMD_MSK		NO_OS_GENMASK(6,5)
#define AD7746_VTSETUP_EXTREF_MSK	NO_OS_BIT(4)
#define AD7746_VTSETUP_VTSHORT_MSK	NO_OS_BIT(1)
#define AD7746_VTSETUP_VTCHOP_MSK	NO_OS_BIT(0)

/* AD7746_REG_EXC_SETUP bits */
#define AD7746_EXCSETUP_CLKCTRL_MSK	NO_OS_BIT(7)
#define AD7746_EXCSETUP_EXCON_MSK	NO_OS_BIT(6)
#define AD7746_EXCSETUP_EXCB_MSK	NO_OS_GENMASK(5,4)
#define AD7746_EXCSETUP_EXCA_MSK	NO_OS_GENMASK(3,2)
#define AD7746_EXCSETUP_EXCLVL_MSK	NO_OS_GENMASK(1,0)

/* AD7746_REG_CFG bits */
#define AD7746_CONF_VTF_MSK		NO_OS_GENMASK(7,6)
#define AD7746_CONF_CAPF_MSK		NO_OS_GENMASK(5,3)
#define AD7746_CONF_MD_MSK		NO_OS_GENMASK(2,0)

/* AD7746_REG_CAPDACx bits */
#define AD7746_CAPDAC_DACEN_MSK		NO_OS_BIT(7)
#define AD7746_CAPDAC_DACP_MSK		NO_OS_GENMASK(6,0)

enum ad7746_id {
	ID_AD7745,
	ID_AD7746,
	ID_AD7747,
};

struct ad7746_cap {
	bool capen;
	bool cin2;
	bool capdiff;
	bool capchop;
};

enum ad7746_vtmd {
	AD7746_VTMD_INT_TEMP,
	AD7746_VTMD_EXT_TEMP,
	AD7746_VTMD_VDD_MON,
	AD7746_VIN_EXT_VIN
};

struct ad7746_vt {
	bool vten;
	enum ad7746_vtmd vtmd;
	bool extref;
	bool vtshort;
	bool vtchop;
};

enum ad7746_exc_pin {
	AD7746_EXC_PIN_DISABLED,
	AD7746_EXC_PIN_INVERTED,
	AD7746_EXC_PIN_NORMAL
};

enum ad7746_exclvl {
	AD7746_EXCLVL_1_DIV_8,
	AD7746_EXCLVL_2_DIV_8,
	AD7746_EXCLVL_3_DIV_8,
	AD7746_EXCLVL_4_DIV_8
};

struct ad7746_exc {
	bool clkctrl;
	bool excon;
	enum ad7746_exc_pin excb;
	enum ad7746_exc_pin exca;
	enum ad7746_exclvl exclvl;
};

enum ad7746_md {
	AD7746_MODE_IDLE,
	AD7746_MODE_CONT,
	AD7746_MODE_SINGLE,
	AD7746_MODE_POWERDOWN,
	AD7746_MODE_OFFSET_CALIB = 5,
	AD7746_MODE_GAIN_CALIB
};

struct ad7746_config {
	uint8_t vtf;
	uint8_t capf;
	enum ad7746_md md;
};

struct ad7746_setup {
	struct ad7746_cap cap;
	struct ad7746_vt vt;
	struct ad7746_exc exc;
	struct ad7746_config config;
};

struct ad7746_init_param {
	struct no_os_i2c_init_param i2c_init;
	enum ad7746_id id;
	struct ad7746_setup setup;
};

struct ad7746_dev {
	struct no_os_i2c_desc *i2c_dev;
	enum ad7746_id id;
	uint8_t buf[AD7746_NUM_REGISTERS + 1u];
	struct ad7746_setup setup;
};

int32_t ad7746_init(struct ad7746_dev **device,
		    struct ad7746_init_param *init_param);
int32_t ad7746_reg_write(struct ad7746_dev *dev, uint8_t reg,
			 uint8_t* data, uint16_t bytes_number);
int32_t ad7746_reg_read(struct ad7746_dev *dev, uint8_t reg,
			uint8_t* data, uint16_t bytes_number);
int32_t ad7746_reset(struct ad7746_dev *dev);
int32_t ad7746_remove(struct ad7746_dev *dev);
int32_t ad7746_set_cap(struct ad7746_dev *dev, struct ad7746_cap cap);
int32_t ad7746_set_vt(struct ad7746_dev *dev, struct ad7746_vt vt);
int32_t ad7746_set_exc(struct ad7746_dev *dev, struct ad7746_exc exc);
int32_t ad7746_set_config(struct ad7746_dev *dev, struct ad7746_config config);
int32_t ad7746_set_cap_dac_a(struct ad7746_dev *dev, bool enable, uint8_t code);
int32_t ad7746_set_cap_dac_b(struct ad7746_dev *dev, bool enable, uint8_t code);
int32_t ad7746_set_cap_offset(struct ad7746_dev *dev, uint16_t offset);
int32_t ad7746_set_cap_gain(struct ad7746_dev *dev, uint16_t gain);
int32_t ad7746_set_volt_gain(struct ad7746_dev *dev, uint16_t gain);
int32_t ad7746_get_vt_data(struct ad7746_dev *dev, uint32_t *vt_data);
int32_t ad7746_get_cap_data(struct ad7746_dev *dev, uint32_t *cap_data);
int32_t ad7746_calibrate(struct ad7746_dev *dev, enum ad7746_md md);

#endif // _AD7746_H
