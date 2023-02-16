#include <errno.h>
#include <stdlib.h>
#include "max14906.h"
#include "no_os_util.h"

int max14906_reg_write(struct max14906_desc *desc, uint32_t addr, uint32_t val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.bytes_number = MAX14906_FRAME_SIZE,
		.cs_change = 1,
	};

	desc->buff[0] = no_os_field_prep(MAX14906_CHIP_ADDR_MASK, desc->chip_address);
	desc->buff[0] |= no_os_field_prep(MAX14906_ADDR_MASK, addr);
	desc->buff[0] |= no_os_field_prep(MAX14906_RW_MASK, 1);
	desc->buff[1] = val;

	return no_os_spi_transfer(desc->comm_desc, &xfer, 1);
}

int max14906_reg_read(struct max14906_desc *desc, uint32_t addr, uint32_t *val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.bytes_number = MAX14906_FRAME_SIZE,
		.cs_change = 1,
	};
	int ret;

	desc->buff[0] = no_os_field_prep(MAX14906_CHIP_ADDR_MASK, desc->chip_address);
	desc->buff[0] |= no_os_field_prep(MAX14906_ADDR_MASK, addr);
	desc->buff[0] |= no_os_field_prep(MAX14906_RW_MASK, 0);

	ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	*val = desc->buff[1];

	return 0;
}

int max14906_reg_update(struct max14906_desc *desc, uint32_t addr,
			uint32_t mask, uint32_t val)
{
	int ret;
	uint32_t reg_val;

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
					  MAX14906_PUSH_PULL_CLAMP));
		if (ret)
			return ret;
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

	ret = max14906_reg_read(descriptor, MAX14906_DOILEVEL_REG, &reg_val);
	if (ret)
		return ret;

	/* Clear the latched faults generated at power up */
	ret = max14906_reg_read(descriptor, MAX14906_OPN_WIR_FLT_REG, &reg_val);
	if (ret)
		return ret;

	ret = max14906_reg_read(descriptor, MAX14906_SHD_VDD_FLT_REG, &reg_val);
	if (ret)
		return ret;

	ret = max14906_reg_read(descriptor, MAX14906_GLOBAL_FLT_REG, &reg_val);
	if (ret)
		return ret;

	ret = max14906_reg_update(descriptor, 0xA, NO_OS_GENMASK(1, 0), 0);
	if (ret)
		return ret;

	*desc = descriptor;

	return 0;

err:
	free(descriptor);

	return ret;
}

int max14906_remove(struct max14906_desc *desc)
{
	int ret;

	if (!desc)
		return -ENODEV;

	ret = no_os_spi_remove(desc->comm_desc);
	if (ret)
		return ret;

	free(desc);

	return 0;
}