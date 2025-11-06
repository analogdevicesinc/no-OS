/***************************************************************************//**
 *   @file   max20303.c
 *   @brief  Implementation of the MAX20303 battery charger driver
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#include <stdlib.h>
#include <stdint.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "max20303.h"

int max20303_reg_write(struct max20303_desc *desc, uint8_t addr, uint16_t val)
{
	uint8_t frame[3];

	if (!desc)
		return -EINVAL;

	frame[0] = addr;
	no_os_put_unaligned_be16(val, &frame[1]);

	return no_os_i2c_write(desc->fg_comm_desc, frame, 2, 1);
}

int max20303_reg_read(struct max20303_desc *desc, uint8_t addr, uint16_t *val)
{
	uint8_t frame[2];
	int ret;

	if (!desc)
		return -EINVAL;

	ret = no_os_i2c_write(desc->fg_comm_desc, &addr, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(desc->fg_comm_desc, frame, 2, 1);
	if (ret)
		return ret;

	*val = no_os_get_unaligned_be16(frame);

	return 0;
}

int max20303_read_vcell(struct max20303_desc *desc, uint32_t *vcell_uv)
{
	uint64_t voltage_nv;
	uint16_t reg_val;
	int ret;

	ret = max20303_reg_read(desc, MAX20303_VCELL_REG, &reg_val);
	if (ret)
		return ret;

	voltage_nv = reg_val;

	*vcell_uv = (MAX20303_VCELL_LSB_nV * voltage_nv) / 1000;

	return 0;
}

int max20303_set_hibernate(struct max20303_desc *desc, bool enable)
{
	uint16_t reg_val = (enable) ? MAX20303_HIBERNATE_EN : MAX20303_HIBERNATE_DIS;
	int ret;

	return max20303_reg_write(desc, MAX20303_HIBRT_REG, reg_val);
}

int max20303_read_soc(struct max20303_desc *desc, uint16_t *percentage)
{
	uint16_t reg_val;
	int ret;

	ret = max20303_reg_read(desc, MAX20303_SOC_REG, &reg_val);
	if (ret)
		return ret;

	*percentage = no_os_field_get(NO_OS_GENMASK(15, 8), reg_val);

	return 0;
}

int max20303_init(struct max20303_desc **desc, struct max20303_init_param *param)
{
	struct max20303_desc *descriptor;
	uint16_t reg_val;
	int ret;

	if (!param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	param->comm_param->slave_address = MAX20303_I2C_ADDR;
	ret = no_os_i2c_init(&descriptor->comm_desc, param->comm_param);
	if (ret)
		goto i2c_init_err;

	param->comm_param->slave_address = MAX20303_FG_I2C_ADDR;
	ret = no_os_i2c_init(&descriptor->fg_comm_desc, param->comm_param);
	if (ret)
		goto fg_i2c_init_err;

	ret = max20303_reg_read(descriptor, MAX20303_VERSION_REG, &reg_val);
	if (ret)
		goto free_fg_i2c;

	if (!(reg_val & 0x10))
		goto free_fg_i2c;

	printf("MAX20303 rev id = 0x%X\n", reg_val);

	*desc = descriptor;

	return 0;

free_fg_i2c:
	no_os_i2c_remove(descriptor->fg_comm_desc);
fg_i2c_init_err:
	no_os_i2c_remove(descriptor->comm_desc);
i2c_init_err:
	free(descriptor);

	return ret;
}

int max20303_remove(struct max20303_desc *desc)
{
	if (!desc)
		return -EINVAL;

	no_os_i2c_remove(desc->comm_desc);
	free(desc);

	return 0;
}