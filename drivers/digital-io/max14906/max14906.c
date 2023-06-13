#include <errno.h>
#include <stdlib.h>
#include "max14906.h"
#include "no_os_util.h"
#include "mxc_device.h"

static uint8_t max14906_crc_encode(uint8_t *data)
{
	uint8_t crc5_start = 0x1f;
	uint8_t crc5_poly = 0x15;
	uint8_t crc5_result = crc5_start;
	uint8_t extra_byte = 0x00;
	uint8_t data_bit;
	uint8_t result_bit;

	for (int i = 0; i < 8; i++) {
		data_bit = (data[0] >> (7 - i)) & 0x01; 
		result_bit = (crc5_result & 0x10) >> 4;
		if (data_bit ^ result_bit)
			crc5_result = crc5_poly ^ ((crc5_result << 1) & 0x1f);
		else
			crc5_result = (crc5_result << 1) & 0x1f;
	}

	for (int i = 0; i < 8; i++) {
		data_bit = (data[1] >> (7 - i)) & 0x01; 
		result_bit = (crc5_result & 0x10) >> 4;
		if (data_bit ^ result_bit)
			crc5_result = crc5_poly ^ ((crc5_result << 1) & 0x1f);
		else
			crc5_result = (crc5_result << 1) & 0x1f;
	}

	for (int i = 0; i < 3; i++) {
		data_bit = (extra_byte >> (7 - i)) & 0x01; 
		result_bit = (crc5_result & 0x10) >> 4;
		if (data_bit ^ result_bit)
			crc5_result = crc5_poly ^ ((crc5_result << 1) & 0x1f);
		else
			crc5_result = (crc5_result << 1) & 0x1f;
	}

	return crc5_result;
}

static uint8_t max14906_crc_decode(uint8_t *data)
{
	uint8_t crc5_start = 0x1f;
	uint8_t crc5_poly = 0x15;
	uint8_t crc5_result = crc5_start;
	uint8_t extra_byte = 0x00;
	uint8_t data_bit;
	uint8_t result_bit;

	for (int i = 2; i < 8; i++) {
		data_bit = (data[0] >> (7 - i)) & 0x01;
		result_bit = (crc5_result & 0x10) >> 4;
		if (data_bit ^ result_bit)
			crc5_result = crc5_poly ^ ((crc5_result << 1) & 0x1f);
		else
			crc5_result = (crc5_result << 1) & 0x1f;
	}

	for (int i = 0; i < 8; i++) {
		data_bit = (data[1] >> (7 - i)) & 0x01;
		result_bit = (crc5_result & 0x10) >> 4;
		if (data_bit ^ result_bit)
			crc5_result = crc5_poly ^ ((crc5_result << 1) & 0x1f);
		else
			crc5_result = (crc5_result << 1) & 0x1f;
	}

	for (int i = 0; i < 3; i++) {
		data_bit = (extra_byte >> (7 - i)) & 0x01;
		result_bit = (crc5_result & 0x10) >> 4;
		if (data_bit ^ result_bit)
			crc5_result = crc5_poly ^ ((crc5_result << 1) & 0x1f);
		else
			crc5_result = (crc5_result << 1) & 0x1f;
	}

	return crc5_result;
}

int max14906_reg_write(struct max14906_desc *desc, uint32_t addr, uint32_t val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.bytes_number = MAX14906_FRAME_SIZE,
		.cs_change = 1,
	};
	uint32_t read_val;
	int ret;

	desc->buff[0] = no_os_field_prep(MAX14906_CHIP_ADDR_MASK, desc->chip_address);
	desc->buff[0] |= no_os_field_prep(MAX14906_ADDR_MASK, addr);
	desc->buff[0] |= no_os_field_prep(MAX14906_RW_MASK, 1);
	desc->buff[1] = (uint8_t)val;

	if (desc->crc_en) {
		xfer.bytes_number++;
		desc->buff[2] = max14906_crc_encode(desc->buff);
	}

	return no_os_spi_transfer(desc->comm_desc, &xfer, 1);
}

int max14906_reg_read(struct max14906_desc *desc, uint32_t addr, uint32_t *val)
{
	uint8_t val_reg[3];
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = val_reg,
		.bytes_number = MAX14906_FRAME_SIZE,
		.cs_change = 1,
	};
	uint8_t crc;
	int ret;

	if (desc->crc_en)
		xfer.bytes_number++;

	// ready = MXC_GPIO_OutGet(MXC_GPIO_GET_GPIO(1), 1 << 23);
	memset(desc->buff, 0, 3);
	memset(val_reg, 0, 3);
	desc->buff[0] = no_os_field_prep(MAX14906_CHIP_ADDR_MASK, desc->chip_address);
	desc->buff[0] |= no_os_field_prep(MAX14906_ADDR_MASK, addr);
	desc->buff[0] |= no_os_field_prep(MAX14906_RW_MASK, 0);

	ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	if (desc->crc_en) {
		crc = max14906_crc_decode(val_reg);
		if (crc != val_reg[2])
			return -EINVAL;
	}

	*val = val_reg[1];

	return 0;
}

int max14906_reg_update(struct max14906_desc *desc, uint32_t addr,
			uint32_t mask, uint32_t val)
{
	int ret;
	uint32_t reg_val = 0;

	ret = max14906_reg_read(desc, addr, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= mask & val;

	return max14906_reg_write(desc, addr, reg_val);
}

int max14906_ch_get(struct max14906_desc *desc, uint32_t ch, uint32_t *val)
{
	int ret;

	if (ch >= MAX14906_CHANNELS)
		return -EINVAL;

	ret = max14906_reg_read(desc, MAX14906_DOILEVEL_REG, val);
	if (ret)
		return ret;

	*val = no_os_field_get(MAX14906_DOI_LEVEL_MASK(ch), *val);

	return 0;
}

int max14906_ch_set(struct max14906_desc *desc, uint32_t ch, uint32_t val)
{
	if (ch >= MAX14906_CHANNELS)
		return -EINVAL;

	return max14906_reg_update(desc, MAX14906_SETOUT_REG,
				   MAX14906_HIGHO_MASK(ch), (val) ? MAX14906_HIGHO_MASK(ch) : 0);
}

int max14906_ch_func(struct max14906_desc *desc, uint32_t ch,
		     enum max14906_function function)
{
	int ret;

	if (function == MAX14906_HIGH_Z) {
		ret = max14906_reg_update(desc, MAX14906_CONFIG_DO_REG, MAX14906_DO_MASK(ch),
					  no_os_field_prep(MAX14906_DO_MASK(ch),
					  MAX14906_PUSH_PULL));
		if (ret)
			return ret;

		return max14906_reg_update(desc, MAX14906_SETOUT_REG,
					    MAX14906_CH_DIR_MASK(ch),
					    no_os_field_prep(MAX14906_CH_DIR_MASK(ch), 1));
	}

	return max14906_reg_update(desc, MAX14906_SETOUT_REG, MAX14906_CH_DIR_MASK(ch),
				   no_os_field_prep(MAX14906_CH_DIR_MASK(ch), function));
}

int max14906_init(struct max14906_desc **desc, struct max14906_init_param *param)
{
	struct max14906_desc *descriptor;
	uint32_t reg_val;
	int ret;

	descriptor = calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->comm_desc, param->comm_param);
	if (ret)
		goto err;

	descriptor->crc_en = param->crc_en;

	ret = max14906_reg_read(descriptor, MAX14906_DOILEVEL_REG, &reg_val);
	if (ret)
		goto spi_err;

	/* Clear the latched faults generated at power up */
	ret = max14906_reg_read(descriptor, MAX14906_OPN_WIR_FLT_REG, &reg_val);
	if (ret)
		goto spi_err;

	ret = max14906_reg_read(descriptor, MAX14906_SHD_VDD_FLT_REG, &reg_val);
	if (ret)
		goto spi_err;

	ret = max14906_reg_read(descriptor, MAX14906_GLOBAL_FLT_REG, &reg_val);
	if (ret)
		goto spi_err;

	ret = max14906_reg_update(descriptor, 0xA, NO_OS_GENMASK(1, 0), 0);
	if (ret)
		goto spi_err;

	for (int i = 0; i < 4; i++) {
		ret = max14906_ch_func(descriptor, i, MAX14906_HIGH_Z);
		if (ret)
			goto spi_err;
	}

	*desc = descriptor;

	return 0;

spi_err:
	no_os_spi_remove(descriptor->comm_desc);
err:
	free(descriptor);

	return ret;
}

int max14906_remove(struct max14906_desc *desc)
{
	int ret;

	if (!desc)
		return -ENODEV;

	for (int i = 0; i < 4; i++) {
		ret = max14906_ch_func(desc, i, MAX14906_HIGH_Z);
		if (ret)
			return ret;
	}

	ret = no_os_spi_remove(desc->comm_desc);
	if (ret)
		return ret;

	free(desc);

	return 0;
}