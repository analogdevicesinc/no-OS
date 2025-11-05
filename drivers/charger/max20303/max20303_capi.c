/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include "no_os_alloc.h"
#include "no_os_util.h"
#include "max20303_capi.h"

int max20303_reg_write(struct max20303_capi_desc *desc, uint8_t addr, uint16_t val)
{
	struct capi_i2c_transfer transfer;
	uint8_t frame[3];

	if (!desc)
		return -EINVAL;

	frame[0] = addr;
	no_os_put_unaligned_be16(val, &frame[1]);

	transfer.sub_address = frame;
	transfer.sub_address_len = 3;
	transfer.buf = NULL;
	transfer.len = 0;
	transfer.no_stop = false;
	transfer.repeated_start = false;

	return capi_i2c_transmit(&desc->fg_i2c_device, &transfer);
}

int max20303_reg_read(struct max20303_capi_desc *desc, uint8_t addr, uint16_t *val)
{
	struct capi_i2c_transfer transfer = {0};
	uint8_t frame[2];
	int ret;

	if (!desc)
		return -EINVAL;

	transfer.sub_address = &addr;
	transfer.sub_address_len = 1;
	transfer.buf = frame;
	transfer.len = 2;
	transfer.no_stop = false;
	transfer.repeated_start = false;

	ret = capi_i2c_receive(&desc->fg_i2c_device, &transfer);
	if (ret)
		return ret;

	*val = no_os_get_unaligned_be16(frame);

	return 0;
}

int max20303_read_vcell(struct max20303_capi_desc *desc, uint32_t *vcell_uv)
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

int max20303_set_hibernate(struct max20303_capi_desc *desc, bool enable)
{
	uint16_t reg_val = (enable) ? MAX20303_HIBERNATE_EN : MAX20303_HIBERNATE_DIS;

	return max20303_reg_write(desc, MAX20303_HIBRT_REG, reg_val);
}

int max20303_read_soc(struct max20303_capi_desc *desc, uint16_t *percentage)
{
	uint16_t reg_val;
	int ret;

	ret = max20303_reg_read(desc, MAX20303_SOC_REG, &reg_val);
	if (ret)
		return ret;

	*percentage = no_os_field_get(NO_OS_GENMASK(15, 8), reg_val);

	return 0;
}

int max20303_init(struct max20303_capi_desc **desc,
		  struct max20303_capi_init_param *param)
{
	struct max20303_capi_desc *descriptor;
	uint16_t reg_val;
	int ret;

	if (!param || !param->i2c_controller || !param->i2c_device || !param->fg_i2c_device)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->i2c_controller = param->i2c_controller;
	descriptor->i2c_device = *param->i2c_device;
	descriptor->fg_i2c_device = *param->fg_i2c_device;

	/* Set proper I2C addresses */
	descriptor->i2c_device.address = MAX20303_I2C_ADDR;
	descriptor->fg_i2c_device.address = MAX20303_FG_I2C_ADDR;

	/* Read and verify version register */
	ret = max20303_reg_read(descriptor, MAX20303_VERSION_REG, &reg_val);
	if (ret)
		goto error;

	if (!(reg_val & 0x10)) {
		ret = -ENODEV;
		goto error;
	}

	*desc = descriptor;

	return 0;

error:
	no_os_free(descriptor);
	return ret;
}

int max20303_remove(struct max20303_capi_desc *desc)
{
	if (!desc)
		return -EINVAL;

	no_os_free(desc);

	return 0;
}
