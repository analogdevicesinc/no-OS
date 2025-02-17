/***************************************************************************//**
 *   @file   adp5055.h
 *   @brief  Header file for the ADP5055 Driver
 *   @author Angelo Catapang (angelo.catapang@analog.com)
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
#ifndef __ADP_5055_H__
#define __ADP_5055_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "no_os_gpio.h"
#include "no_os_i2c.h"
#include "no_os_pwm.h"
#include "no_os_util.h"
#include "no_os_units.h"

#define ADP5055_CAPABILITY		0x19
#define ADP5055_STATUS_CML		0x7E
#define ADP5055_MODEL_ID		0xD0
#define ADP5055_CTRL123			0xD1
#define ADP5055_VID_GO			0xD2
#define ADP5055_CTRL_MODE1		0xD3
#define ADP5055_CTRL_MODE2		0xD4
#define ADP5055_DLY1			0xD5
#define ADP5055_DLY2			0xD6
#define ADP5055_DLY3			0xD7
#define ADP5055_VID1			0xD8
#define ADP5055_VID2			0xD9
#define ADP5055_VID3			0xDA
#define ADP5055_DVS_CFG			0xDB
#define ADP5055_DVS_LIM1		0xDC
#define ADP5055_DVS_LIM2		0xDD
#define ADP5055_DVS_LIM3		0xDE
#define ADP5055_FT_CFG			0xDF
#define ADP5055_PG_CFG			0xE0
#define ADP5055_PG_READ			0xE1
#define ADP5055_STATUS_LCH		0xE2

#define ADP5055_CH1_ON			NO_OS_BIT(0)
#define ADP5055_CH2_ON			NO_OS_BIT(1)
#define ADP5055_CH3_ON			NO_OS_BIT(2)

#define ADP5055_VID1_GO			NO_OS_BIT(0)
#define ADP5055_VID2_GO			NO_OS_BIT(1)
#define ADP5055_VID3_GO			NO_OS_BIT(2)

#define ADP5055_EN_MODE_MASK		NO_OS_GENMASK(1, 0)
#define ADP5055_DVS_AUTO		NO_OS_BIT(4)

#define ADP5055_DSCG1_ON		NO_OS_BIT(0)
#define ADP5055_DSCG2_ON		NO_OS_BIT(1)
#define ADP5055_DSCG3_ON		NO_OS_BIT(2)
#define ADP5055_PSM1_ON			NO_OS_BIT(4)
#define ADP5055_PSM2_ON			NO_OS_BIT(5)
#define ADP5055_PSM3_ON			NO_OS_BIT(6)
#define ADP5055_OCP_BLANKING		NO_OS_BIT(7)

#define ADP5055_EN_DLY_MASK		NO_OS_GENMASK(2, 0)
#define ADP5055_DIS_DLY_MASK		NO_OS_GENMASK(6, 4)

#define ADP5055_DVS_INTVAL1		NO_OS_GENMASK(1, 0)
#define ADP5055_DVS_INTVAL2		NO_OS_GENMASK(3, 2)
#define ADP5055_DVS_INTVAL3		NO_OS_GENMASK(5, 4)

#define ADP5055_VIDX_LOW_MASK		NO_OS_GENMASK(3, 0)
#define ADP5055_VIDX_HIGH_MASK		NO_OS_GENMASK(7, 4)

#define ADP5055_FT1_TH			NO_OS_GENMASK(1, 0)
#define ADP5055_FT2_TH			NO_OS_GENMASK(3, 2)
#define ADP5055_FT3_TH			NO_OS_GENMASK(5, 4)

#define ADP5055_PG1_MASK		NO_OS_BIT(0)
#define ADP5055_PG2_MASK		NO_OS_BIT(1)
#define ADP5055_PG3_MASK		NO_OS_BIT(2)
#define ADP5055_PWRGD_DLY		NO_OS_BIT(4)

#define ADP5055_PWRGD1			NO_OS_BIT(0)
#define ADP5055_PWRGD2			NO_OS_BIT(1)
#define ADP5055_PWRGD3			NO_OS_BIT(2)

#define ADP5055_PG1_LCH			NO_OS_BIT(0)
#define ADP5055_PG2_LCH			NO_OS_BIT(1)
#define ADP5055_PG3_LCH			NO_OS_BIT(2)
#define ADP5055_TSD_LCH			NO_OS_BIT(3)
#define ADP5055_OCP1_LCH		NO_OS_BIT(4)
#define ADP5055_OCP2_LCH		NO_OS_BIT(5)
#define ADP5055_OCP3_LCH		NO_OS_BIT(6)
#define ADP5055_INT_LCH			NO_OS_BIT(7)

/* PMBus Addresses */
#define ADP5055_PMBUS_OPEN_ADDRESS	0x70
#define ADP5055_PMBUS_0OHM_ADDRESS	0x70
#define ADP5055_PMBUS_14KOHM_ADDRESS	0x71
#define ADP5055_PMBUS_16KOHM_ADDRESS	0x72
#define ADP5055_PMBUS_20KOHM_ADDRESS	0x73
#define ADP5055_PMBUS_23KOHM_ADDRESS	0x70
#define ADP5055_PMBUS_32KOHM_ADDRESS	0x71
#define ADP5055_PMBUS_39KOHM_ADDRESS	0x73
#define ADP5055_PMBUS_47KOHM_ADDRESS	0x71
#define ADP5055_PMBUS_57KOHM_ADDRESS	0x72
#define ADP5055_PMBUS_71KOHM_ADDRESS	0x73
#define ADP5055_PMBUS_90KOHM_ADDRESS	0x70
#define ADP5055_PMBUS_127KOHM_ADDRESS	0x71
#define ADP5055_PMBUS_200KOHM_ADDRESS	0x72
#define ADP5055_PMBUS_511KOHM_ADDRESS	0x73

enum adp5055_latch_status {
	ADP5055_LCH_INIT_FAIL,
	ADP5055_LCH_OVERCURRENT_HICCUP,
	ADP5055_LCH_THERMAL_SHUTDOWN,
	ADP5055_LCH_PG_FAIL
};

enum adp5055_en_mode {
	ADP5055_EN_MODE_HW_ONLY,
	ADP5055_EN_MODE_SW_ONLY,
	ADP5055_EN_MODE_HW_AND_SW,
	ADP5055_EN_MODE_HW_OR_SW
};

enum adp5055_vidx_limit {
	ADP5055_VIDX_LOW,
	ADP5055_VIDX_HIGH
};

enum adp5055_ft_th {
	ADP5055_FT_NONE,
	ADP5055_FT_1_5WINDOW_3G,	// 1.5% window with 3xGm, Gm=350uA/V
	ADP5055_FT_1_5WINDOW_5G,	// 1.5% window with 5xGm, Gm=350uA/V
	ADP5055_FT_2_5WINDOW_5G		// 2.5% window with 5xGm, Gm=350uA/V
};

enum adp5055_dvs_intval {
	ADP5055_DVS_INTVAL_125US,	// 125uS at 12mV/ms slew rate
	ADP5055_DVS_INTVAL_62US,	// 62.5uS at 24mV/ms slew rate
	ADP5055_DVS_INTVAL_31US,	// 31.2uS at 48mV/ms slew rate
	ADP5055_DVS_INTVAL_15US,	// 15.6uS at 96mV/ms slew rate
};

enum adp5055_en_dly {
	ADP5055_EN_NO_DELAY,
	ADP5055_EN_1TSET,
	ADP5055_EN_2TSET,
	ADP5055_EN_3TSET,
	ADP5055_EN_4TSET,
	ADP5055_EN_5TSET,
	ADP5055_EN_6TSET,
	ADP5055_EN_7TSET,
};

enum adp5055_dis_dly {
	ADP5055_DIS_NO_DELAY,
	ADP5055_DIS_2TSET,
	ADP5055_DIS_4TSET,
	ADP5055_DIS_6TSET,
	ADP5055_DIS_8TSET,
	ADP5055_DIS_10TSET,
	ADP5055_DIS_12TSET,
	ADP5055_DIS_14TSET,
};

enum adp5055_channel {
	ADP5055_VOUT1,
	ADP5055_VOUT2,
	ADP5055_VOUT3
};

/**
 * @brief Initialization parameter for the ADP5055 device.
*/
struct adp5055_init_param {
	struct no_os_i2c_init_param *i2c_param;
};

/**
 * @brief Device descriptor for ADP5055.
*/
struct adp5055_desc {
	struct no_os_i2c_desc *i2c_desc;
};

/** Read command from ADP5055 device. */
int adp5055_read(struct adp5055_desc *desc, uint8_t address, uint8_t *data);

/** Write command to ADP5055 device. */
int adp5055_write(struct adp5055_desc *desc, uint8_t address, uint8_t data);

/** Read ADP5055 CML status. */
int adp5055_read_status(struct adp5055_desc *desc, uint8_t *status_val);

/** Set ADP5055 HW/SW enable mode. */
int adp5055_set_enable_mode(struct adp5055_desc *desc,
			    enum adp5055_en_mode mode);

/** Enable/disable ADP5055 dynamic voltage scaling (DVS) auto mode. */
int adp5055_set_dvs_auto(struct adp5055_desc *desc, bool en);

/** Enable/disable ADP5055 overcurrent protection blanking during DVS transition. */
int adp5055_set_ocp_blanking(struct adp5055_desc *desc, bool en);

/** Enable/disable ADP5055 auto PWM/PSM mode per channel. */
int adp5055_set_auto_pwm_psm(struct adp5055_desc *desc, enum adp5055_channel ch,
			     bool en);

/** Enable/disable ADP5055 output discharge path per channel. */
int adp5055_set_output_discharge(struct adp5055_desc *desc,
				 enum adp5055_channel ch, bool en);

/** Set ADP5055 enable and disable delay time per channel. */
int adp5055_set_enable_disable_delay(struct adp5055_desc *desc,
				     enum adp5055_channel ch, enum adp5055_en_dly en_dly,
				     enum adp5055_dis_dly dis_dly);

/** Set ADP5055 VID code per channel. */
int adp5055_set_vid_code(struct adp5055_desc *desc, enum adp5055_channel ch,
			 uint8_t val);

/** Start ADP5055 Vout transition per channel. */
int adp5055_start_vout(struct adp5055_desc *desc, enum adp5055_channel ch);

/** Enable/disable ADP5055 output channels. */
int adp5055_enable_channel(struct adp5055_desc *desc, enum adp5055_channel ch,
			   bool en);

/** Set ADP5055 DVS interval per channel. */
int adp5055_set_dvs_interval(struct adp5055_desc *desc, enum adp5055_channel ch,
			     enum adp5055_dvs_intval intval);

/** Set ADP5055 VID code high/low limits per channel. */
int adp5055_set_vidx_lim(struct adp5055_desc *desc, enum adp5055_channel ch,
			 enum adp5055_vidx_limit lim_type, uint8_t lim_val);

/** Set ADP5055 fast transient. */
int adp5055_set_fast_transient(struct adp5055_desc *desc,
			       enum adp5055_channel ch, enum adp5055_ft_th ft);

/** Enable/disable ADP5055 power-good signal outputs per channel. */
int adp5055_set_pg_channel_output(struct adp5055_desc *desc,
				  enum adp5055_channel ch, bool en);

/** Enable/disable hardware power-good signal delay. */
int adp5055_set_pg_hw_delay(struct adp5055_desc *desc, bool en);

/** Read ADP5055 power-good status per channel. */
int adp5055_read_pg(struct adp5055_desc *desc, enum adp5055_channel ch,
		    uint8_t *pg_val);

/** Read ADP5055 latched status. */
int adp5055_read_latched_status(struct adp5055_desc *desc,
				enum adp5055_channel ch, enum adp5055_latch_status lch_status_type,
				uint8_t *lch_status_val);

/** Initialize the ADP5055 device descriptor. */
int adp5055_init(struct adp5055_desc **desc,
		 struct adp5055_init_param *init_param);

/** Remove resources allocated by the init function. */
int adp5055_remove(struct adp5055_desc *desc);

#endif /** __ADP_5055_H__ */
