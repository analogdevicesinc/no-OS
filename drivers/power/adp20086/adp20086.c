/***************************************************************************//**
 *   @file   adp20086.c
 *   @brief  Source file for the ADP20086 driver.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "adp20086.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_crc8.h"

NO_OS_DECLARE_CRC8_TABLE(adp20086_crc8_table);

/* internal helpers */
static int adp20086_enable_channel_param(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_parameters param);
static int adp20086_disable_channel_param(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_parameters param);
static int adp20086_read_adc_code(struct adp20086_dev *dev, uint8_t reg, uint8_t *code);
static int adp20086_read_adc_microunits(struct adp20086_dev *dev, uint8_t reg, uint32_t lsb, uint32_t *val_micro);
static int adp20086_set_param(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_parameters param, uint32_t value);
static int adp20086_get_param_code(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_parameters param, uint8_t *code);
static int adp20086_get_param_microunits(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_parameters param, uint32_t *value_micro);

int adp20086_init(struct adp20086_dev **dev,
		  struct adp20086_init_param *init_param)
{
	struct adp20086_dev *device;
	uint8_t id;
	int ret;

	if (!dev || !init_param)
		return -EINVAL;

	no_os_crc8_populate_msb(adp20086_crc8_table, ADP20086_CRC8_POLY);

	device = (struct adp20086_dev *)no_os_calloc(sizeof(*device), 1);
	if (!device)
		return -ENOMEM;

	ret = no_os_i2c_init(&device->i2c_desc, init_param->i2c_param);
	if (ret)
		goto error_dev;

	ret = adp20086_read(device, ADP20086_REG_ID, &id);
	if (ret)
		goto error_dev;

	device->pece = no_os_field_get(ADP20086_ID_PECE, id);
	device->part_id = no_os_field_get(ADP20086_ID_ID, id);
	device->revision = no_os_field_get(ADP20086_ID_REV, id);

	ret = adp20086_check_self_test(device);
	if (ret)
		goto error_dev;

	*dev = device;

	return 0;

error_dev:
	adp20086_remove(device);

	return ret;
}

int adp20086_remove(struct adp20086_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

int adp20086_read(struct adp20086_dev *dev, uint8_t reg, uint8_t *data)
{
	uint8_t vals[ADP20086_RD_FRAME_SIZE];
	int num_bytes = 1;
	uint8_t crc;
	int ret;

	if (!dev)
		return -EINVAL;

	vals[0] = dev->i2c_desc->slave_address << 1;
	vals[1] = reg;
	vals[2] = (dev->i2c_desc->slave_address << 1) | 0x1;


	ret = no_os_i2c_write(dev->i2c_desc, &vals[1], num_bytes, 0);
	if (ret)
		return ret;

	// If PEC enabled, change num_bytes (1-byte data, 1-byte PEC)
	if (dev->pece)
		num_bytes = 2;

	ret = no_os_i2c_read(dev->i2c_desc, &vals[3], num_bytes, 1);
	if (ret)
		return ret;

	if (dev->pece) {
		crc = no_os_crc8(adp20086_crc8_table, vals, ADP20086_RD_FRAME_SIZE - 1, 0);
		if (vals[4] != crc)
			return -EIO;
	}

	*data = vals[3];

	return 0;
}

int adp20086_write(struct adp20086_dev *dev, uint8_t reg, uint8_t data)
{
	uint8_t vals[ADP20086_WR_FRAME_SIZE];
	int num_bytes = 2;

	if (!dev)
		return -EINVAL;

	vals[0] = dev->i2c_desc->slave_address << 1;
	vals[1] = reg;
	vals[2] = data;

	// If PEC enabled, change num_bytes (1-byte register, 1-byte data, 1-byte PEC)
	if (dev->pece) {
		num_bytes = 3;
		vals[3] = no_os_crc8(adp20086_crc8_table, vals, ADP20086_WR_FRAME_SIZE - 1, 0);
	}

	return no_os_i2c_write(dev->i2c_desc, &vals[1], num_bytes, 1);
}

int adp20086_update_register(struct adp20086_dev *dev, uint8_t reg, uint8_t mask, uint8_t data)
{
	int ret;
	uint8_t reg_data;

	if (!dev)
		return -EINVAL;

	ret = adp20086_read(dev, reg, &reg_data);
	if(ret)
		return ret;

	reg_data &= ~mask;
	reg_data |= no_os_field_prep(mask, data);

	return adp20086_write(dev, reg, reg_data);
}

int adp20086_get_register_field(struct adp20086_dev *dev, uint8_t reg, uint8_t mask, uint8_t *data)
{
	int ret;
	uint8_t reg_data;

	if (!dev)
		return -EINVAL;

	ret = adp20086_read(dev, reg, &reg_data);
	if(ret)
		return ret;

	*data = no_os_field_get(mask, reg_data);
	
	return 0;
}

int adp20086_configure_mux(struct adp20086_dev *dev, enum adp20086_legacy_adc_mux_config config)
{
	if (!dev || config < ADP20086_LEGACY_MUX_IOUT || config > ADP20086_LEGACY_MUX_VIN_VDD)
		return -EINVAL;
	
	return adp20086_update_register(dev, ADP20086_REG_CONFIG, ADP20086_CONFIG_MUX, (uint8_t)config);
}

int adp20086_configure_enc(struct adp20086_dev *dev, bool enabled)
{
	if (!dev)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_CONFIG, ADP20086_CONFIG_ENC, (uint8_t)enabled);
}

int adp20086_configure_clr(struct adp20086_dev *dev, bool enabled)
{
	if (!dev)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_CONFIG, ADP20086_CONFIG_CLR, (uint8_t)enabled);
}

int adp20086_get_mux_config(struct adp20086_dev *dev, enum adp20086_legacy_adc_mux_config *config)
{
	uint8_t val;
	int ret;

	if (!dev || !config)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_CONFIG, ADP20086_CONFIG_MUX, &val);
	if (ret)
		return ret;

	*config = val;

	return 0;
}

int adp20086_get_enc_config(struct adp20086_dev *dev, bool *enabled)
{
	uint8_t val;
	int ret;

	if (!dev || !enabled)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_CONFIG, ADP20086_CONFIG_ENC, &val);
	if (ret)
		return ret;

	*enabled = val;

	return 0;
}

int adp20086_get_clr_config(struct adp20086_dev *dev, bool *enabled)
{
	uint8_t val;
	int ret;

	if (!dev || !enabled)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_CONFIG, ADP20086_CONFIG_CLR, &val);
	if (ret)
		return ret;

	*enabled = val;

	return 0;
}

int adp20086_enable_channel(struct adp20086_dev *dev, enum adp20086_channel ch)
{
	return adp20086_enable_channel_param(dev, ch, ADP20086_PARAM_CHANNELS);
}

int adp20086_disable_channel(struct adp20086_dev *dev, enum adp20086_channel ch)
{
	return adp20086_disable_channel_param(dev, ch, ADP20086_PARAM_CHANNELS);
}

int adp20086_enable_load_detection(struct adp20086_dev *dev, enum adp20086_channel ch)
{
	return adp20086_enable_channel_param(dev, ch, ADP20086_PARAM_LOAD_DETECTION);
}

int adp20086_disable_load_detection(struct adp20086_dev *dev, enum adp20086_channel ch)
{
	return adp20086_disable_channel_param(dev, ch, ADP20086_PARAM_LOAD_DETECTION);
}

int adp20086_enable_open_protection(struct adp20086_dev *dev, enum adp20086_channel ch)
{
	return adp20086_enable_channel_param(dev, ch, ADP20086_PARAM_OPEN_PROTECTION);
}

int adp20086_disable_open_protection(struct adp20086_dev *dev, enum adp20086_channel ch)
{
	return adp20086_disable_channel_param(dev, ch, ADP20086_PARAM_OPEN_PROTECTION);
}

static int adp20086_get_channel_param_status(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_parameters param, uint8_t *status)
{
	uint8_t reg, mask;
	if (!dev || ch < ADP20086_ALL_CHANNELS || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	if (ch == ADP20086_ALL_CHANNELS){
		switch(param){
			case ADP20086_PARAM_CHANNELS:
				reg = ADP20086_REG_CONFIG;
				mask = ADP20086_CONFIG_EN_MASK;
				break;
			case ADP20086_PARAM_LOAD_DETECTION:
				reg = ADP20086_REG_LOAD_INT_EN;
				mask = ADP20086_LDET_EN_MASK;
				break;
			case ADP20086_PARAM_OPEN_PROTECTION:
				reg = ADP20086_REG_LOAD_INT_EN;
				mask = ADP20086_OPEN_PROT_MASK;
				break;
			default:
				return -EINVAL;
		}
		
	}
	else
	{
		switch(param){
			case ADP20086_PARAM_CHANNELS:
				reg = ADP20086_REG_CONFIG;
				mask = ADP20086_CONFIG_EN(ch);
				break;
			case ADP20086_PARAM_LOAD_DETECTION:
				reg = ADP20086_REG_LOAD_INT_EN;
				mask = ADP20086_LDET_EN(ch);
				break;
			case ADP20086_PARAM_OPEN_PROTECTION:
				reg = ADP20086_REG_LOAD_INT_EN;
				mask = ADP20086_OPEN_PROT(ch);
				break;
			default:
				return -EINVAL;
		}
	}

	return adp20086_get_register_field(dev, reg, mask, status);
}

static int adp20086_enable_channel_param(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_parameters param)
{
	uint8_t reg, mask, enabled;

	if (!dev || ch < ADP20086_ALL_CHANNELS || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	if (ch == ADP20086_ALL_CHANNELS){
		switch(param){
			case ADP20086_PARAM_CHANNELS:
				reg = ADP20086_REG_CONFIG;
				mask = ADP20086_CONFIG_EN_MASK;
				enabled = ADP20086_ALL_CH_ASSERTED;
				break;
			case ADP20086_PARAM_LOAD_DETECTION:
				reg = ADP20086_REG_LOAD_INT_EN;
				mask = ADP20086_LDET_EN_MASK;
				enabled = ADP20086_ALL_CH_ASSERTED;
				break;
			case ADP20086_PARAM_OPEN_PROTECTION:
				reg = ADP20086_REG_LOAD_INT_EN;
				mask = ADP20086_OPEN_PROT_MASK;
				enabled = ADP20086_ALL_CH_ASSERTED;
				break;
			default:
				return -EINVAL;
		}
		
	}
	else
	{
		switch(param){
			case ADP20086_PARAM_CHANNELS:
				reg = ADP20086_REG_CONFIG;
				mask = ADP20086_CONFIG_EN(ch);
				enabled = ADP20086_SINGLE_CH_ASSERTED;
				break;
			case ADP20086_PARAM_LOAD_DETECTION:
				reg = ADP20086_REG_LOAD_INT_EN;
				mask = ADP20086_LDET_EN(ch);
				enabled = ADP20086_SINGLE_CH_ASSERTED;
				break;
			case ADP20086_PARAM_OPEN_PROTECTION:
				reg = ADP20086_REG_LOAD_INT_EN;
				mask = ADP20086_OPEN_PROT(ch);
				enabled = ADP20086_SINGLE_CH_ASSERTED;
				break;
			default:
				return -EINVAL;
		}
	}

	return adp20086_update_register(dev, reg, mask, enabled);
}

static int adp20086_disable_channel_param(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_parameters param)
{
	uint8_t reg, mask;

	if (!dev || ch < ADP20086_ALL_CHANNELS || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	if (ch == ADP20086_ALL_CHANNELS){
		switch(param){
			case ADP20086_PARAM_CHANNELS:
				reg = ADP20086_REG_CONFIG;
				mask = ADP20086_CONFIG_EN_MASK;
				break;
			case ADP20086_PARAM_LOAD_DETECTION:
				reg = ADP20086_REG_LOAD_INT_EN;
				mask = ADP20086_LDET_EN_MASK;
				break;
			case ADP20086_PARAM_OPEN_PROTECTION:
				reg = ADP20086_REG_LOAD_INT_EN;
				mask = ADP20086_OPEN_PROT_MASK;
				break;
			default:
				return -EINVAL;
		}
		
	}
	else
	{
		switch(param){
			case ADP20086_PARAM_CHANNELS:
				reg = ADP20086_REG_CONFIG;
				mask = ADP20086_CONFIG_EN(ch);
				break;
			case ADP20086_PARAM_LOAD_DETECTION:
				reg = ADP20086_REG_LOAD_INT_EN;
				mask = ADP20086_LDET_EN(ch);
				break;
			case ADP20086_PARAM_OPEN_PROTECTION:
				reg = ADP20086_REG_LOAD_INT_EN;
				mask = ADP20086_OPEN_PROT(ch);
				break;
			default:
				return -EINVAL;
		}
	}

	return adp20086_update_register(dev, reg, mask, false);
}

int adp20086_get_pece(struct adp20086_dev *dev, bool *pec_enabled)
{
	if (!dev || !pec_enabled)
		return -EINVAL;

	*pec_enabled = dev->pece;

	return 0;
}

int adp20086_get_part_id(struct adp20086_dev *dev, uint8_t *part_id)
{
	if (!dev || !part_id)
		return -EINVAL;

	*part_id = dev->part_id;

	return 0;
}

int adp20086_get_revision(struct adp20086_dev *dev, uint8_t *revision)
{
	if (!dev || !revision)
		return -EINVAL;

	*revision = dev->revision;

	return 0;
}

int adp20086_set_interrupt_mask(struct adp20086_dev *dev, enum adp20086_interrupt_source src, bool masked)
{
	uint8_t reg, mask;

	switch (src){
		case ADP20086_INT_ACCM:
			reg = ADP20086_REG_MASK;
			mask = ADP20086_MASK_ACCM;
			break;
		case ADP20086_INT_TSM:
			reg = ADP20086_REG_MASK;
			mask = ADP20086_MASK_TSM;
			break;
		case ADP20086_INT_VDDM:
			reg = ADP20086_REG_MASK;
			mask = ADP20086_MASK_VDD;
			break;
		case ADP20086_INT_VINM:
			reg = ADP20086_REG_MASK;
			mask = ADP20086_MASK_VIN;
			break;
		case ADP20086_INT_OCM:
			reg = ADP20086_REG_MASK;
			mask = ADP20086_MASK_OC;
			break;
		case ADP20086_INT_OVM:
			reg = ADP20086_REG_MASK;
			mask = ADP20086_MASK_OV;
			break;
		case ADP20086_INT_UVM:
			reg = ADP20086_REG_MASK;
			mask = ADP20086_MASK_UV;
			break;
		case ADP20086_INT_UV1M:
			reg = ADP20086_REG_MASK2;
			mask = ADP20086_MASK2_UV1M;
			break;
		case ADP20086_INT_UV2M:
			reg = ADP20086_REG_MASK2;
			mask = ADP20086_MASK2_UV2M;
			break;
		case ADP20086_INT_UV3M:
			reg = ADP20086_REG_MASK2;
			mask = ADP20086_MASK2_UV3M;
			break;
		case ADP20086_INT_UV4M:
			reg = ADP20086_REG_MASK2;
			mask = ADP20086_MASK2_UV4M;
			break;
		case ADP20086_INT_OPNM1:
			reg = ADP20086_REG_MASK2;
			mask = ADP20086_MASK2_OPNM1;
			break;
		case ADP20086_INT_OPNM2:
			reg = ADP20086_REG_MASK2;
			mask = ADP20086_MASK2_OPNM2;
			break;
		case ADP20086_INT_OPNM3:
			reg = ADP20086_REG_MASK2;
			mask = ADP20086_MASK2_OPNM3;
			break;
		case ADP20086_INT_OPNM4:
			reg = ADP20086_REG_MASK2;
			mask = ADP20086_MASK2_OPNM4;
			break;
		case ADP20086_INT_FCALM:
			reg = ADP20086_REG_MASK3;
			mask = ADP20086_MASK3_FCALM;
			break;
		case ADP20086_INT_BISTM:
			reg = ADP20086_REG_MASK3;
			mask = ADP20086_MASK3_BISTM;
			break;
		case ADP20086_INT_PARERRM:
			reg = ADP20086_REG_MASK3;
			mask = ADP20086_MASK3_PARERRM;
			break;
		case ADP20086_INT_ECC_CORM:
			reg = ADP20086_REG_MASK3;
			mask = ADP20086_MASK3_ECC_CORM;
			break;
		case ADP20086_INT_ECC_DETM:
			reg = ADP20086_REG_MASK3;
			mask = ADP20086_MASK3_ECC_DETM;
			break;
		default:
			return -EINVAL;

	}
	
	return adp20086_update_register(dev, reg, mask, (uint8_t) masked);
}

int adp20086_set_ovtst(struct adp20086_dev *dev, bool enabled)
{
	int ret;
	uint8_t channels_enabled;

	if (!dev)
		return -EINVAL;

	ret = adp20086_read(dev, ADP20086_REG_CONFIG, &channels_enabled);
	if (ret)
		return ret;

	channels_enabled = no_os_field_get(ADP20086_CONFIG_EN_MASK, channels_enabled);

	if (channels_enabled)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_MASK, ADP20086_MASK_OVTST, (uint8_t) enabled);
}

int adp20086_set_iir_filter(struct adp20086_dev *dev, bool enabled)
{
	if (!dev)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_MASK3, ADP20086_MASK3_IIR_EN, (uint8_t) enabled);
}

int adp20086_set_discharge_resistors(struct adp20086_dev *dev, bool enabled)
{
	if (!dev)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_MASK3, ADP20086_MASK3_DISCH_EN, (uint8_t) enabled);
}

int adp20086_get_interrupt_mask(struct adp20086_dev *dev, enum adp20086_interrupt_source src, bool *masked)
{
	uint8_t reg, mask, val;
	int ret;

	if (!dev || !masked)
		return -EINVAL;

	switch (src){
		case ADP20086_INT_ACCM:
			reg = ADP20086_REG_MASK;
			mask = ADP20086_MASK_ACCM;
			break;
		case ADP20086_INT_TSM:
			reg = ADP20086_REG_MASK;
			mask = ADP20086_MASK_TSM;
			break;
		case ADP20086_INT_VDDM:
			reg = ADP20086_REG_MASK;
			mask = ADP20086_MASK_VDD;
			break;
		case ADP20086_INT_VINM:
			reg = ADP20086_REG_MASK;
			mask = ADP20086_MASK_VIN;
			break;
		case ADP20086_INT_OCM:
			reg = ADP20086_REG_MASK;
			mask = ADP20086_MASK_OC;
			break;
		case ADP20086_INT_OVM:
			reg = ADP20086_REG_MASK;
			mask = ADP20086_MASK_OV;
			break;
		case ADP20086_INT_UVM:
			reg = ADP20086_REG_MASK;
			mask = ADP20086_MASK_UV;
			break;
		case ADP20086_INT_UV1M:
			reg = ADP20086_REG_MASK2;
			mask = ADP20086_MASK2_UV1M;
			break;
		case ADP20086_INT_UV2M:
			reg = ADP20086_REG_MASK2;
			mask = ADP20086_MASK2_UV2M;
			break;
		case ADP20086_INT_UV3M:
			reg = ADP20086_REG_MASK2;
			mask = ADP20086_MASK2_UV3M;
			break;
		case ADP20086_INT_UV4M:
			reg = ADP20086_REG_MASK2;
			mask = ADP20086_MASK2_UV4M;
			break;
		case ADP20086_INT_OPNM1:
			reg = ADP20086_REG_MASK2;
			mask = ADP20086_MASK2_OPNM1;
			break;
		case ADP20086_INT_OPNM2:
			reg = ADP20086_REG_MASK2;
			mask = ADP20086_MASK2_OPNM2;
			break;
		case ADP20086_INT_OPNM3:
			reg = ADP20086_REG_MASK2;
			mask = ADP20086_MASK2_OPNM3;
			break;
		case ADP20086_INT_OPNM4:
			reg = ADP20086_REG_MASK2;
			mask = ADP20086_MASK2_OPNM4;
			break;
		case ADP20086_INT_FCALM:
			reg = ADP20086_REG_MASK3;
			mask = ADP20086_MASK3_FCALM;
			break;
		case ADP20086_INT_BISTM:
			reg = ADP20086_REG_MASK3;
			mask = ADP20086_MASK3_BISTM;
			break;
		case ADP20086_INT_PARERRM:
			reg = ADP20086_REG_MASK3;
			mask = ADP20086_MASK3_PARERRM;
			break;
		case ADP20086_INT_ECC_CORM:
			reg = ADP20086_REG_MASK3;
			mask = ADP20086_MASK3_ECC_CORM;
			break;
		case ADP20086_INT_ECC_DETM:
			reg = ADP20086_REG_MASK3;
			mask = ADP20086_MASK3_ECC_DETM;
			break;
		default:
			return -EINVAL;

	}

	ret = adp20086_get_register_field(dev, reg, mask, &val);
	if (ret)
		return ret;

	*masked = val;

	return 0;
}

int adp20086_get_ovtst(struct adp20086_dev *dev, bool *enabled)
{
	uint8_t val;
	int ret;

	if (!dev || !enabled)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_MASK, ADP20086_MASK_OVTST, &val);
	if (ret)
		return ret;

	*enabled = val;

	return 0;
}

int adp20086_get_iir_filter(struct adp20086_dev *dev, bool *enabled)
{
	uint8_t val;
	int ret;

	if (!dev || !enabled)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_MASK3, ADP20086_MASK3_IIR_EN, &val);
	if (ret)
		return ret;

	*enabled = val;

	return 0;
}

int adp20086_get_discharge_resistors(struct adp20086_dev *dev, bool *enabled)
{
	uint8_t val;
	int ret;

	if (!dev || !enabled)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_MASK3, ADP20086_MASK3_DISCH_EN, &val);
	if (ret)
		return ret;

	*enabled = val;

	return 0;
}

int adp20086_get_channel_status(struct adp20086_dev *dev, enum adp20086_channel ch, struct adp20086_channel_status *status)
{
	struct adp20086_channel_status st;
	uint8_t stat_reg, stat, stat4;
	int ret;

	if (!dev || !status || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	if (ch < ADP20086_CHANNEL3)
		stat_reg = ADP20086_REG_STAT2;
	else
		stat_reg = ADP20086_REG_STAT3;

	ret = adp20086_read(dev, stat_reg, &stat);
	if (ret)
		return ret;

	ret = adp20086_read(dev, ADP20086_REG_STAT4, &stat4);
	if (ret)
		return ret;

	st.thermal_shutdown = no_os_field_get(ADP20086_CHAN_STATUS_TS(ch), stat);
	st.overcurrent = no_os_field_get(ADP20086_CHAN_STATUS_OC(ch), stat);
	st.overvoltage= no_os_field_get(ADP20086_CHAN_STATUS_OV(ch), stat);
	st.undervoltage = no_os_field_get(ADP20086_CHAN_STATUS_UV(ch), stat);
	st.load_detected = no_os_field_get(ADP20086_CHAN_STATUS_LOAD_DET(ch), stat4);
	st.open_load = no_os_field_get(ADP20086_CHAN_STATUS_OPEN(ch), stat4);

	*status = st;

	return 0;
}

int adp20086_get_device_status(struct adp20086_dev *dev, struct adp20086_device_status *status)
{
	struct adp20086_device_status st;
	uint8_t stat1, stat5;
	int ret;

	if (!dev || !status)
		return -EINVAL;

	ret = adp20086_read(dev, ADP20086_REG_STAT1, &stat1);
	if (ret)
		return ret;
	
	ret = adp20086_read(dev, ADP20086_REG_STAT5, &stat5);
	if (ret)
		return ret;
	
	st.tlim_serv = no_os_field_get(ADP20086_STAT1_TLIM_SERV, stat1);
	st.acc = no_os_field_get(ADP20086_STAT1_ACC, stat1);
	st.ovin = no_os_field_get(ADP20086_STAT1_OVIN, stat1);
	st.uvin = no_os_field_get(ADP20086_STAT1_UVIN, stat1);
	st.ovdd = no_os_field_get(ADP20086_STAT1_OVDD, stat1);
	st.uvdd = no_os_field_get(ADP20086_STAT1_UVDD, stat1);
	st.adc_fcal = no_os_field_get(ADP20086_STAT5_ADC_FCAL, stat5);
	st.intbsts = no_os_field_get(ADP20086_STAT5_INTBSTS, stat5);
	st.bist = no_os_field_get(ADP20086_STAT5_BIST, stat5);
	st.parerr = no_os_field_get(ADP20086_STAT5_PARERR, stat5);
	st.ecc_cor = no_os_field_get(ADP20086_STAT5_ECC_COR, stat5);
	st.ecc_det = no_os_field_get(ADP20086_STAT5_ECC_DET, stat5);

	*status = st;

	return 0;
}

int adp20086_check_self_test(struct adp20086_dev *dev)
{
	uint8_t stat5;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adp20086_read(dev, ADP20086_REG_STAT5, &stat5);
	if (ret)
		return ret;

	if (stat5 & ADP20086_SELFTEST_FAULT_MASK)
		return -ENODEV;

	return 0;
}

int adp20086_read_legacy_adc(struct adp20086_dev *dev, enum adp20086_legacy_adc adc, uint8_t *data)
{
	if (!dev || !data || adc < ADP20086_ADC1_LEGACY || adc > ADP20086_ADC4_LEGACY)
		return -EINVAL;

	return adp20086_read(dev, ADP20086_REG_ADC_LEGACY(adc), data);
}

int adp20086_set_ilim(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_ilim_available ilim)
{
	int ret;

	if (!dev || ch < ADP20086_ALL_CHANNELS || ch > ADP20086_CHANNEL4 || ilim < ADP20086_ILIM_52MA || ilim > ADP20086_ILIM_832MA)
		return -EINVAL;

	if (ch == ADP20086_ALL_CHANNELS){
		for (int c = 0; c <= ADP20086_CHANNEL4; c++){
			ret = adp20086_update_register(dev, ADP20086_REG_ILIM(c), ADP20086_ILIM_MASK(c), ilim);
			if (ret)
				return ret;
		}

		return 0;
	}

	return adp20086_update_register(dev, ADP20086_REG_ILIM(ch), ADP20086_ILIM_MASK(ch), ilim);
}

int adp20086_get_ilim_code(struct adp20086_dev *dev, enum adp20086_channel ch, uint8_t *code)
{
	if (!dev || ch < ADP20086_ALL_CHANNELS || ch > ADP20086_CHANNEL4 || !code)
		return -EINVAL;

	return adp20086_get_register_field(dev, ADP20086_REG_ILIM(ch), ADP20086_ILIM_MASK(ch), code);
}

int adp20086_get_ilim_ua(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t *ilim_ua)
{
	int ret;
	uint8_t code;

	if (!dev || ch < ADP20086_ALL_CHANNELS || ch > ADP20086_CHANNEL4 || !ilim_ua)
		return -EINVAL;

	ret = adp20086_get_ilim_code(dev, ch, &code);
	if (ret)
		return ret;

	*ilim_ua = code * ADP20086_ILIM_LSB_UA;

	return 0;
}

int adp20086_set_ldet_threshold(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t threshold_ua)
{
	return adp20086_set_param(dev, ch, ADP20086_PARAM_LDET, threshold_ua);	
}

int adp20086_get_ldet_threshold_code(struct adp20086_dev *dev, enum adp20086_channel ch, uint8_t *code)
{
	return adp20086_get_param_code(dev, ch, ADP20086_PARAM_LDET, code);
}

int adp20086_get_ldet_threshold_ua(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t *threshold_ua)
{
	return adp20086_get_param_microunits(dev, ch, ADP20086_PARAM_LDET, threshold_ua);
}

int adp20086_set_ovin(struct adp20086_dev *dev, uint32_t ovin_uv)
{
	return adp20086_set_param(dev, ADP20086_ALL_CHANNELS, ADP20086_PARAM_OVIN, ovin_uv);	
}

int adp20086_get_ovin_code(struct adp20086_dev *dev, uint8_t *code)
{
	return adp20086_get_param_code(dev, ADP20086_ALL_CHANNELS, ADP20086_PARAM_OVIN, code);
}

int adp20086_get_ovin_uv(struct adp20086_dev *dev, uint32_t *ovin_uv)
{
	return adp20086_get_param_microunits(dev, ADP20086_ALL_CHANNELS, ADP20086_PARAM_OVIN, ovin_uv);
}

int adp20086_set_uvin(struct adp20086_dev *dev, uint32_t uvin_uv)
{
	return adp20086_set_param(dev, ADP20086_ALL_CHANNELS, ADP20086_PARAM_UVIN, uvin_uv);	
}

int adp20086_get_uvin_code(struct adp20086_dev *dev, uint8_t *code)
{
	return adp20086_get_param_code(dev, ADP20086_ALL_CHANNELS, ADP20086_PARAM_UVIN, code);
}

int adp20086_get_uvin_uv(struct adp20086_dev *dev, uint32_t *uvin_uv)
{
	return adp20086_get_param_microunits(dev, ADP20086_ALL_CHANNELS, ADP20086_PARAM_UVIN, uvin_uv);
}

int adp20086_set_iopen(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t iopen_ua)
{
	return adp20086_set_param(dev, ch, ADP20086_PARAM_IOPEN, iopen_ua);
}

int adp20086_get_iopen_code(struct adp20086_dev *dev, enum adp20086_channel ch, uint8_t *code)
{
	return adp20086_get_param_code(dev, ch, ADP20086_PARAM_IOPEN, code);
}

int adp20086_get_iopen_ua(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t *iopen_ua)
{
	return adp20086_get_param_microunits(dev, ch, ADP20086_PARAM_IOPEN, iopen_ua);
}

int adp20086_set_uvout(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t uvout_uv)
{
	return adp20086_set_param(dev, ch, ADP20086_PARAM_UVOUT, uvout_uv);
}

int adp20086_get_uvout_code(struct adp20086_dev *dev, enum adp20086_channel ch, uint8_t *code)
{
	return adp20086_get_param_code(dev, ch, ADP20086_PARAM_UVOUT, code);
}

int adp20086_get_uvout_uv(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t *uvout_uv)
{
	return adp20086_get_param_microunits(dev, ch, ADP20086_PARAM_UVOUT, uvout_uv);
}

int adp20086_set_soft_start(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_soft_start ssu)
{
	uint8_t data = 0;
	
	if (!dev || ch < ADP20086_ALL_CHANNELS || ch > ADP20086_CHANNEL4 || ssu < ADP20086_SSU_0R5MS || ssu > ADP20086_SSU_4MS)
		return -EINVAL;
	
	if (ch == ADP20086_ALL_CHANNELS){
		for (int c = ADP20086_CHANNEL1; c <= ADP20086_CHANNEL4; c++){
			data |= no_os_field_prep(ADP20086_START_SLEW_MASK(c), ssu);
		}
		return adp20086_write(dev, ADP20086_REG_START_SLEW, data);
	}

	return adp20086_update_register(dev, ADP20086_REG_START_SLEW, ADP20086_START_SLEW_MASK(ch), ssu);
}

int adp20086_get_soft_start(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_soft_start *ssu)
{
	uint8_t val;
	int ret;

	if (!dev || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4 || !ssu)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_START_SLEW, ADP20086_START_SLEW_MASK(ch), &val);
	if (ret)
		return ret;

	*ssu = val;

	return 0;
}


int adp20086_set_soft_shutdown(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_soft_shutdown ssd)
{
	uint8_t data = 0;
	
	if (!dev || ch < ADP20086_ALL_CHANNELS || ch > ADP20086_CHANNEL4 || ssd < ADP20086_SSD_0R25MS || ssd > ADP20086_SSD_2MS)
		return -EINVAL;
	
	if (ch == ADP20086_ALL_CHANNELS){
		for (int c = ADP20086_CHANNEL1; c <= ADP20086_CHANNEL4; c++){
			data |= no_os_field_prep(ADP20086_SHDN_SLEW_MASK(c), ssd);
		}
		return adp20086_write(dev, ADP20086_REG_SHDN_SLEW, data);
	}

	return adp20086_update_register(dev, ADP20086_REG_SHDN_SLEW, ADP20086_SHDN_SLEW_MASK(ch), ssd);
}

int adp20086_get_soft_shutdown(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_soft_shutdown *ssd)
{
	uint8_t val;
	int ret;

	if (!dev || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4 || !ssd)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_SHDN_SLEW, ADP20086_SHDN_SLEW_MASK(ch), &val);
	if (ret)
		return ret;

	*ssd = val;

	return 0;
}

int adp20086_read_vin_code(struct adp20086_dev *dev, uint8_t *code)
{
	return adp20086_read_adc_code(dev, ADP20086_REG_VIN_READ, code);
}

int adp20086_read_vin_uv(struct adp20086_dev *dev, uint32_t *vin_uv)
{
	return adp20086_read_adc_microunits(dev, ADP20086_REG_VIN_READ, ADP20086_VIN_LSB_UV, vin_uv);
}

int adp20086_read_vout_code(struct adp20086_dev *dev, enum adp20086_channel ch, uint8_t *code)
{
	return adp20086_read_adc_code(dev, ADP20086_REG_VOUT_READ(ch), code);
}

int adp20086_read_vout_uv(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t *vout_uv)
{
	if (ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_read_adc_microunits(dev, ADP20086_REG_VOUT_READ(ch), ADP20086_VOUT_LSB_UV, vout_uv);
}

int adp20086_read_iout_code(struct adp20086_dev *dev, enum adp20086_channel ch, uint8_t *code)
{
	return adp20086_read_adc_code(dev, ADP20086_REG_IOUT_READ(ch), code);
}

int adp20086_read_iout_ua(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t *iout_ua)
{
	if (ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_read_adc_microunits(dev, ADP20086_REG_IOUT_READ(ch), ADP20086_IOUT_LSB_UA, iout_ua);
}

int adp20086_read_vdd_code(struct adp20086_dev *dev, uint8_t *code)
{
	return adp20086_read_adc_code(dev, ADP20086_REG_VDD_READ, code);
}

int adp20086_read_vdd_uv(struct adp20086_dev *dev, uint32_t *vdd_uv)
{
	return adp20086_read_adc_microunits(dev, ADP20086_REG_VDD_READ, ADP20086_VDD_LSB_UV, vdd_uv);
}

static int adp20086_read_adc_code(struct adp20086_dev *dev, uint8_t reg, uint8_t *code)
{
	if (!dev || !code)
		return -EINVAL;
	
	return adp20086_read(dev, reg, code);
}

static int adp20086_read_adc_microunits(struct adp20086_dev *dev, uint8_t reg, uint32_t lsb, uint32_t *val_micro)
{
	uint8_t code;
	int ret;

	if (!dev || !val_micro)
		return -EINVAL;
	
	ret = adp20086_read(dev, reg, &code);
	if (ret)
		return ret;

	*val_micro = code * lsb;

	return 0;
}

static int adp20086_set_param(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_parameters param, uint32_t value)
{
	uint8_t reg, code;
	uint32_t max, lsb;
	int ret;

	switch(param){
		case ADP20086_PARAM_LDET:
			lsb = ADP20086_LDET_LSB_UA;
			max = ADP20086_MAX_LDET_UA;
			break;
		case ADP20086_PARAM_UVIN:
			lsb = ADP20086_VIN_LSB_UV;
			max = ADP20086_MAX_VIN_UV;
			break;
		case ADP20086_PARAM_OVIN:
			lsb = ADP20086_VIN_LSB_UV;
			max = ADP20086_MAX_VIN_UV;
			break;
		case ADP20086_PARAM_IOPEN:
			lsb = ADP20086_IOPEN_LSB_UA;
			max = ADP20086_MAX_IOPEN_UA;
			break;
		case ADP20086_PARAM_UVOUT:
			lsb = ADP20086_VOUT_LSB_UV;
			max = ADP20086_MAX_VOUT_UV;
			break;
		default:
			return -EINVAL;
	}

	if (!dev || value <  lsb || value > max)
		return -EINVAL;
			
	code = value / lsb;

	if (ch == ADP20086_ALL_CHANNELS){
		for (int c = ADP20086_CHANNEL1; c <= ADP20086_CHANNEL4; c++){
			switch(param){
				case ADP20086_PARAM_LDET:
					reg = ADP20086_REG_LDET_SET(c);
					break;
				case ADP20086_PARAM_IOPEN:
					reg = ADP20086_REG_IOPEN_SET(c);
					break;
				case ADP20086_PARAM_UVOUT:
					reg = ADP20086_REG_UVOUT_SET(c);
					break;
				case ADP20086_PARAM_UVIN:
					reg = ADP20086_REG_UVIN_SET;
					break;
				case ADP20086_PARAM_OVIN:
					reg = ADP20086_REG_OVIN_SET;
				break;

				default:
					return -EINVAL;
			}
			ret = adp20086_write(dev, reg, code);	
			if (ret)
				return ret;
			
			if (param == ADP20086_PARAM_UVIN || param == ADP20086_PARAM_OVIN)
				break;
		}
	} else {
		switch(param){
			case ADP20086_PARAM_LDET:
				reg = ADP20086_REG_LDET_SET(ch);
				break;
			case ADP20086_PARAM_IOPEN:
				reg = ADP20086_REG_IOPEN_SET(ch);
				break;
			case ADP20086_PARAM_UVOUT:
				reg = ADP20086_REG_UVOUT_SET(ch);
				break;
			case ADP20086_PARAM_UVIN:
				reg = ADP20086_REG_UVIN_SET;
				break;
			case ADP20086_PARAM_OVIN:
				reg = ADP20086_REG_OVIN_SET;
			break;

			default:
				return -EINVAL;
		}
		ret = adp20086_write(dev, reg, code);	
		if (ret)
			return ret;
	}

	return 0;
}

static int adp20086_get_param_code(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_parameters param, uint8_t *code)
{
	uint8_t reg;

	if (!dev || !code)
		return -EINVAL;
	if (ch == ADP20086_ALL_CHANNELS && param != ADP20086_PARAM_UVIN && param != ADP20086_PARAM_OVIN)
		return -EINVAL;
	else if (ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	switch(param){
		case ADP20086_PARAM_LDET:
			reg = ADP20086_REG_LDET_SET(ch);
			break;
		case ADP20086_PARAM_IOPEN:
			reg = ADP20086_REG_IOPEN_SET(ch);
			break;
		case ADP20086_PARAM_UVOUT:
			reg = ADP20086_REG_UVOUT_SET(ch);
			break;
		case ADP20086_PARAM_UVIN:
			reg = ADP20086_REG_UVIN_SET;
			break;
		case ADP20086_PARAM_OVIN:
			reg = ADP20086_REG_OVIN_SET;
		break;

		default:
			return -EINVAL;
	}

	return adp20086_read(dev, reg, code);
}

static int adp20086_get_param_microunits(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_parameters param, uint32_t *value_micro)
{
	uint8_t reg, code;
	uint32_t lsb;
	int ret;

	if (!dev || !value_micro)
		return -EINVAL;
	if (ch == ADP20086_ALL_CHANNELS && param != ADP20086_PARAM_UVIN && param != ADP20086_PARAM_OVIN)
		return -EINVAL;
	else if (ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	switch(param){
		case ADP20086_PARAM_LDET:
			reg = ADP20086_REG_LDET_SET(ch);
			lsb = ADP20086_LDET_LSB_UA;
			break;
		case ADP20086_PARAM_IOPEN:
			reg = ADP20086_REG_IOPEN_SET(ch);
			lsb = ADP20086_IOPEN_LSB_UA;
			break;
		case ADP20086_PARAM_UVOUT:
			reg = ADP20086_REG_UVOUT_SET(ch);
			lsb = ADP20086_VOUT_LSB_UV;
			break;
		case ADP20086_PARAM_UVIN:
			reg = ADP20086_REG_UVIN_SET;
			lsb = ADP20086_VIN_LSB_UV;
			break;
		case ADP20086_PARAM_OVIN:
			reg = ADP20086_REG_OVIN_SET;
			lsb = ADP20086_VIN_LSB_UV;
		break;
		default:
			return -EINVAL;
	}

	ret = adp20086_read(dev, reg, &code);
	if (ret)
		return ret;

	*value_micro = code * lsb;
	
	return 0;
}