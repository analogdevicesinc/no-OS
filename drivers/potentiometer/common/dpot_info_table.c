/***************************************************************************
 *   @file    dpot_info_table.c
 *   @brief   Digipots information table which gives the details about the parts.
********************************************************************************
 * Copyright 2023-24(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdbool.h>
#include "app_config.h"

/******************************************************************************/
/************************ Macros/Constants ************************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
dpot_device_info dpot_info[NUM_SUPPORTED_DEVICES] = {
	[DEV_AD5121] = {
		.device_name = "AD5121",
		.max_position = 127,
		.device_i2c_addr = 0x2F,
		.num_of_channels = 1,
		.nSupportedInterface = AD_SPI_INTERFACE | AD_I2C_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5141_dpot_ops,
		.dpot_init_params.extra = &ad5141_init_params,
		.dpot_init_params.device_id = DEV_AD5121,
		.dpot_init_params.intf_type = AD_SPI_INTERFACE
	},
	[DEV_AD5122] = {
		.device_name = "AD5122",
		.max_position = 127,
		.device_i2c_addr = 0x2F,
		.num_of_channels = 2,
		.nSupportedInterface = AD_SPI_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5142_dpot_ops,
		.dpot_init_params.extra = &ad5142_init_params,
		.dpot_init_params.device_id = DEV_AD5122,
		.dpot_init_params.intf_type = AD_SPI_INTERFACE
	},
	[DEV_AD5122A] = {
		.device_name = "AD5122A",
		.max_position = 127,
		.device_i2c_addr = 0x2F,
		.num_of_channels = 2,
		.nSupportedInterface = AD_I2C_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5142_dpot_ops,
		.dpot_init_params.extra = &ad5142_init_params,
		.dpot_init_params.device_id = DEV_AD5122A,
		.dpot_init_params.intf_type = AD_I2C_INTERFACE
	},

	[DEV_AD5123] = {
		.device_name = "AD5123",
		.max_position = 127,
		.device_i2c_addr = 0x2B,
		.num_of_channels = 4,
		.nSupportedInterface = AD_I2C_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5144_dpot_ops,
		.dpot_init_params.extra = &ad5144_init_params,
		.dpot_init_params.device_id = DEV_AD5123,
		.dpot_init_params.intf_type = AD_I2C_INTERFACE
	},
	[DEV_AD5124] = {
		.device_name = "AD5124",
		.max_position = 127,
		.device_i2c_addr = 0x2F,
		.num_of_channels = 4,
		.nSupportedInterface = AD_SPI_INTERFACE | AD_I2C_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5144_dpot_ops,
		.dpot_init_params.extra = &ad5144_init_params,
		.dpot_init_params.device_id = DEV_AD5124,
		.dpot_init_params.intf_type = AD_SPI_INTERFACE
	},
	[DEV_AD5141] = {
		.device_name = "AD5141",
		.max_position = 255,
		.device_i2c_addr = 0x2F,
		.num_of_channels = 1,
		.nSupportedInterface = AD_SPI_INTERFACE | AD_I2C_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5141_dpot_ops,
		.dpot_init_params.extra = &ad5141_init_params,
		.dpot_init_params.device_id = DEV_AD5141,
		.dpot_init_params.intf_type = AD_SPI_INTERFACE

	},
	[DEV_AD5142] = {
		.device_name = "AD5142",
		.max_position = 255,
		.device_i2c_addr = 0x2F,
		.num_of_channels = 2,
		.nSupportedInterface = AD_SPI_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5142_dpot_ops,
		.dpot_init_params.extra = &ad5142_init_params,
		.dpot_init_params.device_id = DEV_AD5142,
		.dpot_init_params.intf_type = AD_SPI_INTERFACE
	},
	[DEV_AD5142A] = {
		.device_name = "AD5142A",
		.max_position = 255,
		.device_i2c_addr = 0x2F,
		.num_of_channels = 2,
		.nSupportedInterface = AD_I2C_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5142_dpot_ops,
		.dpot_init_params.extra = &ad5142_init_params,
		.dpot_init_params.device_id = DEV_AD5142A,
		.dpot_init_params.intf_type = AD_I2C_INTERFACE

	},
	[DEV_AD5143] = {
		.device_name = "AD5143",
		.max_position = 255,
		.device_i2c_addr = 0x2B,
		.num_of_channels = 4,
		.nSupportedInterface = AD_I2C_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5144_dpot_ops,
		.dpot_init_params.extra = &ad5144_init_params,
		.dpot_init_params.device_id = DEV_AD5143,
		.dpot_init_params.intf_type = AD_I2C_INTERFACE
	},

	[DEV_AD5144] = {
		.device_name = "AD5144",
		.max_position = 255,
		.device_i2c_addr = 0x2F,
		.num_of_channels = 4,
		.nSupportedInterface = AD_SPI_INTERFACE | AD_I2C_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5144_dpot_ops,
		.dpot_init_params.extra = &ad5144_init_params,
		.dpot_init_params.device_id = DEV_AD5144,
		.dpot_init_params.intf_type = AD_SPI_INTERFACE
	},
	[DEV_AD5160] = {
		.device_name = "AD5160",
		.max_position = 255,
		.device_i2c_addr = 0XFF,
		.num_of_channels = 1,
		.nSupportedInterface = AD_SPI_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5161_dpot_ops,
		.dpot_init_params.extra = &ad5161_init_params,
		.dpot_init_params.device_id = DEV_AD5160,
		.dpot_init_params.intf_type = AD_SPI_INTERFACE
	},

	[DEV_AD5161] = {
		.device_name = "AD5161",
		.device_i2c_addr = 0x2C,
		.max_position = 255,
		.num_of_channels = 1,
		.nSupportedInterface = AD_I2C_INTERFACE | AD_SPI_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5161_dpot_ops,
		.dpot_init_params.extra = &ad5161_init_params,
		.dpot_init_params.device_id = DEV_AD5161,
		.dpot_init_params.intf_type = AD_SPI_INTERFACE
	},
	[DEV_AD5165] = {
		.device_name = "AD5165",
		.num_of_channels = 1,
		.max_position = 255,
		.nSupportedInterface = AD_SPI_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5165_dpot_ops,
		.dpot_init_params.extra =   &ad5165_init_params,
		.dpot_init_params.device_id = DEV_AD5273,
		.dpot_init_params.intf_type = AD_SPI_INTERFACE
	},
	[DEV_AD5171] = {
		.device_name = "AD5171",
		.max_position = 63,
		.device_i2c_addr = 0x2C,
		.num_of_channels = 1,
		.nSupportedInterface = AD_I2C_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5171_dpot_ops,
		.dpot_init_params.extra =   &ad5171_init_params,
		.dpot_init_params.device_id = DEV_AD5171,
		.dpot_init_params.intf_type = AD_I2C_INTERFACE
	},
	[DEV_AD5241] = {
		.device_name = "AD5241",
		.max_position = 255,
		.device_i2c_addr = 0x2C,
		.num_of_channels = 1,
		.nSupportedInterface = AD_I2C_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5242_dpot_ops,
		.dpot_init_params.extra =   &ad5242_init_params,
		.dpot_init_params.device_id = DEV_AD5241,
		.dpot_init_params.intf_type = AD_I2C_INTERFACE
	},

	[DEV_AD5242] = {
		.device_name = "AD5242",
		.max_position = 255,
		.device_i2c_addr = 0x2C,
		.num_of_channels = 2,
		.nSupportedInterface = AD_I2C_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5242_dpot_ops,
		.dpot_init_params.extra =   &ad5242_init_params,
		.dpot_init_params.device_id = DEV_AD5242,
		.dpot_init_params.intf_type = AD_I2C_INTERFACE
	},
	[DEV_AD5245] = {
		.device_name = "AD5245",
		.device_i2c_addr = 0x2C,
		.max_position = 255,
		.num_of_channels = 1,
		.nSupportedInterface = AD_I2C_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5161_dpot_ops,
		.dpot_init_params.extra = &ad5161_init_params,
		.dpot_init_params.device_id = DEV_AD5245,
		.dpot_init_params.intf_type = AD_I2C_INTERFACE
	},
	[DEV_AD5246] = {
		.device_name = "AD5246",
		.max_position = 127,
		.device_i2c_addr = 0x2E,
		.num_of_channels = 1,
		.nSupportedInterface = AD_I2C_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5246_dpot_ops,
		.dpot_init_params.extra = &ad5246_init_params,
		.dpot_init_params.device_id = DEV_AD5246,
		.dpot_init_params.intf_type = AD_I2C_INTERFACE
	},
	[DEV_AD5258] = {
		.device_name = "AD5258",
		.max_position = 63,
		.device_i2c_addr = 0x4E,
		.num_of_channels = 1,
		.nSupportedInterface =  AD_I2C_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5259_dpot_ops,
		.dpot_init_params.extra = &ad5259_init_params,
		.dpot_init_params.device_id = DEV_AD5258,
		.dpot_init_params.intf_type = AD_I2C_INTERFACE
	},
	[DEV_AD5259] = {
		.device_name = "AD5259",
		.max_position = 255,
		.device_i2c_addr = 0x4E,
		.num_of_channels = 1,
		.nSupportedInterface =  AD_I2C_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5259_dpot_ops,
		.dpot_init_params.extra = &ad5259_init_params,
		.dpot_init_params.device_id = DEV_AD5259,
		.dpot_init_params.intf_type = AD_I2C_INTERFACE
	},
	[DEV_AD5273] = {
		.device_name = "AD5273",
		.max_position = 63,
		.device_i2c_addr = 0x2C,
		.num_of_channels = 1,
		.nSupportedInterface = AD_I2C_INTERFACE,
		.dpot_init_params.dpot_ops = &ad5171_dpot_ops,
		.dpot_init_params.extra =   &ad5171_init_params,
		.dpot_init_params.device_id = DEV_AD5273,
		.dpot_init_params.intf_type = AD_I2C_INTERFACE
	},
};

