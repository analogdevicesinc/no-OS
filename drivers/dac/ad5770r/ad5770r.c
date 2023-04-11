/***************************************************************************//**
 *   @file   ad5770r.c
 *   @brief  Implementation of ad5770r Driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
********************************************************************************
 * Copyright 2018, 2020(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "no_os_error.h"
#include "ad5770r.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/
/**
 * Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_spi_reg_read(struct ad5770r_dev *dev,
			     uint8_t reg_addr,
			     uint8_t *reg_data)
{
	uint8_t buf[2];
	int32_t ret;

	if (!dev | !reg_data)
		return -1;

	buf[0] = AD5770R_REG_READ(reg_addr);
	buf[1] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, sizeof(buf));

	if (ret)
		return ret;

	*reg_data = buf[1];

	return ret;
}

/**
 * Multibyte read from device. A register read begins with the address
 * and autoincrements for each aditional byte in the transfer.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @param count - Number of bytes to read.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_spi_reg_read_multiple(struct ad5770r_dev *dev,
				      uint8_t reg_addr,
				      uint8_t *reg_data,
				      uint16_t count)
{
	uint8_t *buf;
	int32_t ret;

	if (!dev || !reg_data)
		return -1;

	buf = (uint8_t*)no_os_calloc(count + 1, sizeof(uint8_t));

	buf[0] = AD5770R_REG_READ(reg_addr);

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, count + 1);

	if (ret) {
		no_os_free(buf);
		return ret;
	}

	memcpy(reg_data, buf + 1, count);
	no_os_free(buf);

	return ret;
}
/**
 * Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_spi_reg_write(struct ad5770r_dev *dev,
			      uint8_t reg_addr,
			      uint8_t reg_data)
{
	uint8_t buf[2];

	if (!dev)
		return -1;

	buf[0] = AD5770R_REG_WRITE(reg_addr);
	buf[1] = reg_data;

	return no_os_spi_write_and_read(dev->spi_desc, buf, sizeof(buf));
}

/**
 * Multibyte write from device. A register write begins with the address
 * and autoincrements for each additional byte in the transfer.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @param count - Number of bytes to read.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_spi_reg_write_multiple(struct ad5770r_dev *dev,
				       uint8_t reg_addr,
				       uint8_t *reg_data,
				       uint16_t count)
{
	int32_t ret;
	uint8_t *data;

	if (!dev | !reg_data)
		return -1;

	data = (uint8_t*)no_os_calloc(count + 1, sizeof(uint8_t));

	data[0] = AD5770R_REG_WRITE(reg_addr);
	memcpy(&data[1], reg_data, count);

	ret = no_os_spi_write_and_read(dev->spi_desc, data, count + 1);
	no_os_free(data);

	return ret;
}

/**
 * SPI write to device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_spi_write_mask(struct ad5770r_dev *dev,
			       uint8_t reg_addr,
			       uint32_t mask,
			       uint8_t data)
{
	uint8_t reg_data;
	int32_t ret;

	if (!dev)
		return -1;

	ret = ad5770r_spi_reg_read(dev, reg_addr, &reg_data);
	reg_data &= ~mask;
	reg_data |= data;
	ret |= ad5770r_spi_reg_write(dev, reg_addr, reg_data);

	return ret;
}

/**
 * Set device spi settings.
 * @param dev - The device structure.
 * @param spi_settings - The structure that contains the device spi
 *		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_set_device_spi(struct ad5770r_dev *dev,
			       const struct ad5770r_device_spi_settings *spi_settings)
{
	int32_t ret;

	if (!dev || !spi_settings)
		return -1;

	ret = ad5770r_spi_write_mask(dev,
				     AD5770R_INTERFACE_CONFIG_A,
				     AD5770R_INTERFACE_CONFIG_A_ADDR_ASCENSION_MSB_MSK,
				     AD5770R_INTERFACE_CONFIG_A_ADDR_ASCENSION_MSB(spi_settings->addr_ascension));

	ret |= ad5770r_spi_write_mask(dev,
				      AD5770R_INTERFACE_CONFIG_B,
				      AD5770R_INTERFACE_CONFIG_B_SINGLE_INST_MSK,
				      AD5770R_INTERFACE_CONFIG_B_SINGLE_INST(spi_settings->single_instruction));

	ret |= ad5770r_spi_reg_write(dev,
				     AD5770R_STREAM_MODE,
				     spi_settings->stream_mode_length);

	if (ret)
		return ret;

	dev->dev_spi_settings.addr_ascension =
		spi_settings->addr_ascension;
	dev->dev_spi_settings.single_instruction =
		spi_settings->single_instruction;
	dev->dev_spi_settings.stream_mode_length =
		spi_settings->stream_mode_length;

	return 0;
}

/**
 * Set the channel configuration.
 * @param dev - The device structure.
 * @param channel_config - The structure that contains the channel
 *			configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_channel_config(struct ad5770r_dev *dev,
			       const struct ad5770r_channel_switches *channel_config)
{
	int32_t ret;

	if (!dev || !channel_config)
		return -1;

	ret = ad5770r_spi_reg_write(dev,
				    AD5770R_CHANNEL_CONFIG,
				    AD5770R_CHANNEL_CONFIG_CH0_SHUTDOWN_B(channel_config->en0) |
				    AD5770R_CHANNEL_CONFIG_CH1_SHUTDOWN_B(channel_config->en1) |
				    AD5770R_CHANNEL_CONFIG_CH2_SHUTDOWN_B(channel_config->en2) |
				    AD5770R_CHANNEL_CONFIG_CH3_SHUTDOWN_B(channel_config->en3) |
				    AD5770R_CHANNEL_CONFIG_CH4_SHUTDOWN_B(channel_config->en4) |
				    AD5770R_CHANNEL_CONFIG_CH5_SHUTDOWN_B(channel_config->en5) |
				    AD5770R_CHANNEL_CONFIG_CH0_SINK_EN(channel_config->sink0));

	if (ret)
		return ret;

	dev->channel_config = *channel_config;

	return ret;
}

/**
 * Set the channel configuration.
 * @param dev - The device structure.
 * @param output_mode - The structure that contains the channel output
 *			parameters.
 * @param channel - The channel number.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_set_output_mode(struct ad5770r_dev *dev,
				const struct ad5770r_output_range *output_mode,
				enum ad5770r_channels channel)
{

	if (!dev || !output_mode)
		return -1;

	return ad5770r_spi_reg_write(dev,
				     AD5770R_OUTPUT_RANGE_CH0 + channel,
				     AD5770R_OUTPUT_RANGE_OUTPUT_SCALING(output_mode->output_scale) |
				     AD5770R_OUTPUT_RANGE_MODE(output_mode->output_range_mode));
};

/**
 * Set reference configuration.
 * @param dev - The device structure.
 * @param external_reference - Enable external reference.
 * @param reference_selector - The reference to be selected.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_set_reference(struct ad5770r_dev *dev,
			      bool external_reference,
			      enum ad5770r_reference_voltage reference_selector)
{
	int32_t ret;

	if (!dev)
		return -1;

	ret = ad5770r_spi_reg_write(dev,
				    AD5770R_REFERENCE,
				    AD5770R_REFERENCE_RESISTOR_SEL(external_reference) |
				    AD5770R_REFERENCE_VOLTATE_SEL(reference_selector));

	if (ret)
		return ret;

	dev->reference_selector = reference_selector;
	dev->external_reference = external_reference;

	return ret;
};

/**
 * Set reference configuration.
 * @param dev - The device structure.
 * @param alarm_config - Pointer to the array that contains the alarms
 *			that will be enabled.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_set_alarm(struct ad5770r_dev *dev,
			  const struct ad5770r_alarm_cfg *const alarm_config)
{
	int32_t ret;

	if (!dev || !alarm_config)
		return -1;

	ret = ad5770r_spi_reg_write(dev,
				    AD5770R_ALARM_CONFIG,
				    AD5770R_ALARM_CONFIG_OPEN_DRAIN_EN(alarm_config->open_drain_en) |
				    AD5770R_ALARM_CONFIG_THERMAL_SHUTDOWN_EN(alarm_config->thermal_shutdown_en) |
				    AD5770R_ALARM_CONFIG_BACKGROUND_CRC_EN(alarm_config->background_crc_en) |
				    AD5770R_ALARM_CONFIG_TEMP_WARNING_ALARM_MASK(alarm_config->temp_warning_msk) |
				    AD5770R_ALARM_CONFIG_OVER_TEMP_ALARM_MASK(alarm_config->over_temp_msk) |
				    AD5770R_ALARM_CONFIG_NEGATIVE_CHANNEL0_ALARM_MASK(alarm_config->neg_ch0_msk) |
				    AD5770R_ALARM_CONFIG_IREF_FAULT_ALARM_MASK(alarm_config->iref_fault_msk) |
				    AD5770R_ALARM_CONFIG_BACKGROUND_CRC_ALARM_MASK(
					    alarm_config->background_crc_msk));

	if (ret)
		return ret;

	dev->alarm_config = *alarm_config;

	return ret;
};

/**
 * Set the channel output filter resistor.
 * @param dev - The device structure.
 * @param output_filter - The structure that contains the channel output
 *			filter resistor values.
 * @param channel - The channel number.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_set_output_filter(struct ad5770r_dev *dev,
				  enum ad5770r_output_filter_resistor output_filter,
				  enum ad5770r_channels channel)
{
	if (!dev)
		return -1;

	return ad5770r_spi_reg_write(dev,
				     AD5770R_OUTPUT_FILTER_CH0 + channel,
				     AD5770R_OUTPUT_FILTER_CH(output_filter));
};

/**
 * Set the hardware ldac configuration.
 * @param dev - The device structure.
 * @param mask_hw_ldac - The array contains HW LDAC channel masks.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_set_hw_ldac(struct ad5770r_dev *dev,
			    const struct ad5770r_channel_switches *mask_hw_ldac)
{
	int32_t ret;

	if (!dev || !mask_hw_ldac)
		return -1;

	ret = ad5770r_spi_reg_write(dev,
				    AD5770R_HW_LDAC,
				    AD5770R_HW_LDAC_MASK_CH(mask_hw_ldac->en0, AD5770R_CH0) |
				    AD5770R_HW_LDAC_MASK_CH(mask_hw_ldac->en1, AD5770R_CH1) |
				    AD5770R_HW_LDAC_MASK_CH(mask_hw_ldac->en2, AD5770R_CH2) |
				    AD5770R_HW_LDAC_MASK_CH(mask_hw_ldac->en3, AD5770R_CH3) |
				    AD5770R_HW_LDAC_MASK_CH(mask_hw_ldac->en4, AD5770R_CH4) |
				    AD5770R_HW_LDAC_MASK_CH(mask_hw_ldac->en5, AD5770R_CH5));

	if (ret)
		return ret;

	dev->mask_hw_ldac = *mask_hw_ldac;

	return ret;
};

/**
 * Set dac value.
 * @param dev - The device structure.
 * @param dac_value - value that will be set in the register.
 * @param channel - the selected channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_set_dac_value(struct ad5770r_dev *dev,
			      uint16_t dac_value, enum ad5770r_channels channel)
{
	uint8_t data[2];
	int32_t ret;

	if (!dev)
		return -1;

	data[1] = (uint8_t)AD5770R_CH_DAC_DATA_LSB(dac_value);
	data[0] = (uint8_t)((dac_value & 0x3FC0) >> 6);

	ret = ad5770r_spi_reg_write_multiple(dev,
					     AD5770R_CH0_DAC_MSB + 2 * channel,
					     data, sizeof(data));

	if (ret)
		return ret;

	dev->dac_value[channel] = dac_value;
	dev->input_value[channel] = dac_value;

	return ret;
};

/**
 * Set dac value.
 * @param dev - The device structure.
 * @param dac_input - value that will be set in the register.
 * @param channel - the selected channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_set_dac_input(struct ad5770r_dev *dev,
			      uint16_t dac_input, enum ad5770r_channels channel)
{
	uint8_t data[2];
	int32_t ret;

	if (!dev)
		return -1;

	data[1] = (uint8_t)AD5770R_CH_DAC_DATA_LSB(dac_input);
	data[0] = (uint8_t)((dac_input & 0x3FC0) >> 6);

	ret = ad5770r_spi_reg_write_multiple(dev,
					     AD5770R_CH0_INPUT_MSB + 2 * channel,
					     data, sizeof(data));
	if (ret)
		return ret;

	dev->input_value[channel] = dac_input;

	return ret;
};

/**
 * Set page mask for dac value and input.
 * @param dev - The device structure.
 * @param page_mask - The structure contains the page mask values.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_set_page_mask(struct ad5770r_dev *dev,
			      const struct ad5770r_dac_page_mask *page_mask)
{
	int32_t ret;
	uint8_t data[2];

	if (!dev || !page_mask)
		return -1;

	data[1] = (uint8_t)AD5770R_CH_DAC_DATA_LSB(page_mask->dac_data_page_mask);
	data[0] = (uint8_t)((page_mask->dac_data_page_mask & 0x3FC0) >> 6);

	ret = ad5770r_spi_reg_write_multiple(dev,
					     AD5770R_DAC_PAGE_MASK_MSB,
					     data, sizeof(data));

	if (ret)
		return ret;

	data[1] = (uint8_t)AD5770R_CH_DAC_DATA_LSB(page_mask->input_page_mask);
	data[0] = (uint8_t)((page_mask->input_page_mask & 0x3FC0) >> 6);

	return ad5770r_spi_reg_write_multiple(dev,
					      AD5770R_INPUT_PAGE_MASK_MSB,
					      data, sizeof(data));
}

/**
 * Set channel select.
 * @param dev - The device structure.
 * @param mask_channel_sel - The array contains channel selection options.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_set_mask_channel(struct ad5770r_dev *dev,
				 const struct ad5770r_channel_switches *mask_channel_sel)
{
	int32_t ret;

	if (!dev || !mask_channel_sel)
		return -1;

	ret = ad5770r_spi_reg_write(dev,
				    AD5770R_CH_SELECT,
				    AD5770R_CH_SELECT_SEL_CH(mask_channel_sel->en0, AD5770R_CH0) |
				    AD5770R_CH_SELECT_SEL_CH(mask_channel_sel->en1, AD5770R_CH1) |
				    AD5770R_CH_SELECT_SEL_CH(mask_channel_sel->en2, AD5770R_CH2) |
				    AD5770R_CH_SELECT_SEL_CH(mask_channel_sel->en3, AD5770R_CH3) |
				    AD5770R_CH_SELECT_SEL_CH(mask_channel_sel->en4, AD5770R_CH4) |
				    AD5770R_CH_SELECT_SEL_CH(mask_channel_sel->en5, AD5770R_CH5));

	if (ret)
		return ret;

	dev->mask_channel_sel = *mask_channel_sel;

	return ret;
}

/**
 * Set software LDAC.
 * @param dev - The device structure.
 * @param sw_ldac - The array contains channel selection options.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_set_sw_ldac(struct ad5770r_dev *dev,
			    const struct ad5770r_channel_switches *sw_ldac)
{
	int32_t ret;
	uint8_t sw_ldac_value;
	uint8_t  i;

	if (!dev || !sw_ldac)
		return -1;

	sw_ldac_value = AD5770R_SW_LDAC_CH(sw_ldac->en0, AD5770R_CH0) |
			AD5770R_SW_LDAC_CH(sw_ldac->en1, AD5770R_CH1) |
			AD5770R_SW_LDAC_CH(sw_ldac->en2, AD5770R_CH2) |
			AD5770R_SW_LDAC_CH(sw_ldac->en3, AD5770R_CH3) |
			AD5770R_SW_LDAC_CH(sw_ldac->en4, AD5770R_CH4) |
			AD5770R_SW_LDAC_CH(sw_ldac->en5, AD5770R_CH5);

	ret = ad5770r_spi_reg_write(dev, AD5770R_SW_LDAC, sw_ldac_value);

	if (ret)
		return ret;

	dev->sw_ldac = *sw_ldac;

	for (i = 0; i <= AD5770R_CH5; i++) {
		if ((sw_ldac_value >> i) & NO_OS_BIT(0)) {
			dev->dac_value[i] = dev->input_value[i];
		}
	}

	return ret;
}

/**
 * Get status value.
 * @param dev - The device structure.
 * @param status - The status of the device.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_get_status(struct ad5770r_dev *dev,
			   uint8_t *status)
{
	return ad5770r_spi_reg_read(dev,
				    AD5770R_STATUS,
				    status);
};

/**
 * Get interface status value.
 * @param dev - The device structure.
 * @param status - The INTERFACE STATUS A register value.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_get_interface_status(struct ad5770r_dev *dev,
				     uint8_t *status)
{
	return ad5770r_spi_reg_read(dev,
				    AD5770R_INTERFACE_STATUS_A,
				    status);
};

/**
 * Set the channel monitor configuration.
 * @param dev - The device structure.
 * @param mon_setup - The structure that contains the monitor setup
 *		values.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_set_monitor_setup(struct ad5770r_dev *dev,
				  const struct ad5770r_monitor_setup *mon_setup)
{
	int32_t ret;

	if (!dev || !mon_setup)
		return -1;

	ret = ad5770r_spi_reg_write(dev,
				    AD5770R_MONITOR_SETUP,
				    AD5770R_MONITOR_SETUP_MON_CH(mon_setup->monitor_channel) |
				    AD5770R_MONITOR_SETUP_IB_EXT_EN(mon_setup->ib_ext_en) |
				    AD5770R_MONITOR_SETUP_MUX_BUFFER(mon_setup->mux_buffer) |
				    AD5770R_MONITOR_SETUP_MON_FUNCTION(mon_setup->monitor_function));

	if (ret)
		return ret;

	dev->mon_setup = *mon_setup;

	return ret;
};

/**
 * @brief Deallocate memory for the GPIOs assigned.
 * @param dev - Device driver handler.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t ad5770r_remove_gpio(struct ad5770r_dev *dev)
{
	int32_t ret;

	if (dev->gpio_alarm_n) {
		ret = no_os_gpio_remove(dev->gpio_alarm_n);
		if(NO_OS_IS_ERR_VALUE(ret))
			return -1;
	}
	if (dev->gpio_ldac_n) {
		ret = no_os_gpio_remove(dev->gpio_ldac_n);
		if(NO_OS_IS_ERR_VALUE(ret))
			return -1;
	}
	if (dev->gpio_reset_n) {
		ret = no_os_gpio_remove(dev->gpio_reset_n);
		if(NO_OS_IS_ERR_VALUE(ret))
			return -1;
	}

	return 0;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_init(struct ad5770r_dev **device,
		     const struct ad5770r_init_param *init_param)
{
	struct ad5770r_dev	*dev;
	uint8_t product_id_l, product_id_h;
	int32_t ret;
	enum ad5770r_channels i;

	if (!device || !init_param)
		return -1;

	dev = (struct ad5770r_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error_dev;

	/* GPIO */
	ret = no_os_gpio_get_optional(&dev->gpio_alarm_n, init_param->gpio_alarm_n);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error_spi;
	ret = no_os_gpio_get_optional(&dev->gpio_ldac_n, init_param->gpio_ldac_n);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error_gpio;
	ret = no_os_gpio_get_optional(&dev->gpio_reset_n, init_param->gpio_reset_n);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error_gpio;
	if (dev->gpio_alarm_n) {
		ret = no_os_gpio_direction_input(dev->gpio_alarm_n);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto error_gpio;
	}
	if (dev->gpio_ldac_n) {
		ret = no_os_gpio_direction_output(dev->gpio_ldac_n, NO_OS_GPIO_HIGH);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto error_gpio;
	}
	if (dev->gpio_reset_n) {
		ret = no_os_gpio_direction_output(dev->gpio_reset_n, NO_OS_GPIO_HIGH);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto error_gpio;
	}

	/* Query device presence */
	ad5770r_spi_reg_read(dev, AD5770R_PRODUCT_ID_L, &product_id_l);
	ad5770r_spi_reg_read(dev, AD5770R_PRODUCT_ID_H, &product_id_h);

	if (product_id_l != 0x04 || product_id_h != 0x40) {
		printf("failed to read id (0x%X : 0x%X)\n", product_id_l,
		       product_id_h);
		return -1;
	}

	/* Device settings */
	ret = ad5770r_set_device_spi(dev,
				     &init_param->dev_spi_settings);

	ret |= ad5770r_channel_config(dev,
				      &init_param->channel_config);

	ret |= ad5770r_set_reference(dev,
				     init_param->external_reference,
				     init_param->reference_selector);
	ret |= ad5770r_set_alarm(dev, &init_param->alarm_config);

	ret |= ad5770r_set_hw_ldac(dev,
				   &init_param->mask_hw_ldac);

	for( i = AD5770R_CH0; i <=  AD5770R_CH5; i++) {
		ret |= ad5770r_set_output_mode(dev,
					       &init_param->output_mode[i], i);
		ret |= ad5770r_set_output_filter(dev,
						 init_param->output_filter[i], i);
		ret |= ad5770r_set_dac_value(dev,
					     init_param->dac_value[i], i);
		ret |= ad5770r_set_dac_input(dev,
					     init_param->input_value[i], i);

		dev->output_mode[i] = init_param->output_mode[i];
		dev->output_filter[i] = init_param->output_filter[i];
		dev->dac_value[i] = init_param->dac_value[i];
		dev->input_value[i] = init_param->input_value[i];
	}

	ret |= ad5770r_set_page_mask(dev,
				     &init_param->page_mask);
	ret |= ad5770r_set_mask_channel(dev,
					&init_param->mask_channel_sel);
	ret |= ad5770r_set_sw_ldac(dev,
				   &init_param->sw_ldac);
	ret |= ad5770r_set_monitor_setup(dev,
					 &init_param->mon_setup);

	*device = dev;

	if (!ret)
		printf("ad5770r successfully initialized\n");
	else
		printf("ad5770r initialization error (%d)\n", ret);

	return ret;

error_gpio:
	ad5770r_remove_gpio(dev);
error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
	no_os_free(dev);

	return -1;
}

/**
 * Delete and remove the device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5770r_remove(struct ad5770r_dev *dev)
{
	int32_t ret;

	if (!dev)
		return -1;

	ret = no_os_spi_remove(dev->spi_desc);

	ret = ad5770r_remove_gpio(dev);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	no_os_free(dev);

	return ret;
}
