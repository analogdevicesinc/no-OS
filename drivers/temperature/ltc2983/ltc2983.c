/***************************************************************************//**
 *   @file   ltc2983.c
 *   @brief  Implementation of LTC2983 Driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include <errno.h>
#include "ltc2983.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/******************************************************************************/

/**
 * @brief Device and comm init function
 * @param device - LTC2983 descriptor to be initialized
 * @param init_param - Init parameter for descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_init(struct ltc2983_desc **device,
		 struct ltc2983_init_param *init_param)
{
	int ret, i;
	struct ltc2983_desc *descriptor;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->comm_desc, &init_param->spi_init);
	if (ret)
		goto free_err;

	switch (init_param->dev_type) {
	case ID_LTC2983:
	case ID_LTC2984:
		descriptor->max_channels_nr = 20;
		break;
	case ID_LTC2986:
		descriptor->max_channels_nr = 10;
		break;
	default:
		return -EINVAL;
	}

	descriptor->mux_delay_config_us = init_param->mux_delay_config_us;
	descriptor->filter_notch_freq = init_param->filter_notch_freq;

	descriptor->num_channels = 0;
	for (i = 0; i < descriptor->max_channels_nr; i++) {
		descriptor->sensors[i] = init_param->sensors[i];
		if (init_param->sensors[i] &&
		    init_param->sensors[i]->type != LTC2983_RSENSE)
			descriptor->num_channels++;
	}
	if (!descriptor->num_channels) // should be at least one channel
		goto spi_err;

	ret = no_os_gpio_get_optional(&descriptor->gpio_rstn,
				      &init_param->gpio_rstn);
	if (ret)
		goto spi_err;
	ret = no_os_gpio_direction_output(descriptor->gpio_rstn,
					  NO_OS_GPIO_LOW);
	if (ret)
		goto gpio_err;

	/* bring the device out of reset */
	no_os_udelay(1200);
	ret = no_os_gpio_set_value(descriptor->gpio_rstn, NO_OS_GPIO_HIGH);
	if (ret)
		goto gpio_err;

	ret = ltc2983_setup(descriptor);
	if (ret)
		goto gpio_err;

	*device = descriptor;
	return 0;

gpio_err:
	no_os_gpio_remove(descriptor->gpio_rstn);
spi_err:
	no_os_spi_remove(descriptor->comm_desc);
free_err:
	no_os_free(descriptor);
	return ret;
}

/**
 * @brief Remove resources allocated by the init function
 * @param device - LTC2983 descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_remove(struct ltc2983_desc *device)
{
	int ret;
	if (!device)
		return -ENODEV;

	ret = no_os_gpio_remove(device->gpio_rstn);
	if (ret)
		return -EINVAL;

	ret = no_os_spi_remove(device->comm_desc);
	if (ret)
		return -EINVAL;

	no_os_free(device);

	return 0;
}

/**
 * @brief Read raw register value
 * @param device - LTC2983 descriptor
 * @param reg_addr - register address
 * @param val - register value
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2983_reg_read(struct ltc2983_desc *device, uint16_t reg_addr,
		     uint8_t *val)
{
	int ret;
	uint8_t raw_array[4];

	raw_array[0] = LTC2983_SPI_READ_BYTE;
	no_os_put_unaligned_be16(reg_addr, raw_array + 1);
	raw_array[3] = 0;

	ret = no_os_spi_write_and_read(device->comm_desc, raw_array,
				       NO_OS_ARRAY_SIZE(raw_array));
	if (ret)
		return ret;
	*val = raw_array[3];

	return 0;
}

/**
 * @brief Write raw register value
 * @param device - LTC2983 descriptor
 * @param reg_addr - register address
 * @param val - register value
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2983_reg_write(struct ltc2983_desc *device, uint16_t reg_addr,
		      uint8_t val)
{
	int ret;
	uint8_t raw_array[4];

	raw_array[0] = LTC2983_SPI_WRITE_BYTE;
	no_os_put_unaligned_be16(reg_addr, raw_array + 1);
	raw_array[3] = val;

	ret = no_os_spi_write_and_read(device->comm_desc, raw_array,
				       NO_OS_ARRAY_SIZE(raw_array));
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Update register value
 * @param device - LTC2983 descriptor
 * @param reg_addr - register address
 * @param mask - Mask for specific register bits to be updated
 * @param val - register value to be written
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2983_reg_update_bits(struct ltc2983_desc *device, uint16_t reg_addr,
			    uint8_t mask, uint8_t val)
{
	int ret;
	uint8_t data;

	ret = ltc2983_reg_read(device, reg_addr, &data);
	if (ret)
		return ret;

	data = (data & ~mask) | no_os_field_prep(mask, val);

	return ltc2983_reg_write(device, reg_addr, data);
}

/**
 * @brief Device setup
 * @param device - LTC2983 descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_setup(struct ltc2983_desc *device)
{
	int ret, i;
	uint8_t status = 0;
	uint32_t timeout = 250000;

	/**
	 * make sure the device is up: start bit (7) is 0 and done bit (6) is 1
	 */
	do {
		ret = ltc2983_reg_read(device, LTC2983_STATUS_REG, &status);
		if (ret)
			return ret;
		no_os_udelay(25000);
	} while (LTC2983_STATUS_UP(status) != 1 && --timeout);
	if (!timeout)
		return -EINVAL;

	ret = ltc2983_reg_update_bits(device, LTC2983_GLOBAL_CONFIG_REG,
				      LTC2983_NOTCH_FREQ_MASK,
				      device->filter_notch_freq);
	if (ret)
		return ret;

	ret = ltc2983_reg_write(device, LTC2983_MUX_CONFIG_REG,
				device->mux_delay_config_us / 100);
	if (ret)
		return ret;

	for (i = 0; i < device->max_channels_nr; i++) {
		if (!device->sensors[i])
			continue;

		switch (device->sensors[i]->type) {
		case LTC2983_THERMOCOUPLE_J:
		case LTC2983_THERMOCOUPLE_K:
		case LTC2983_THERMOCOUPLE_E:
		case LTC2983_THERMOCOUPLE_N:
		case LTC2983_THERMOCOUPLE_R:
		case LTC2983_THERMOCOUPLE_S:
		case LTC2983_THERMOCOUPLE_T:
		case LTC2983_THERMOCOUPLE_B:
		case LTC2983_THERMOCOUPLE_CUSTOM:
			ltc2983_thermocouple_assign_chan(device,
							 device->sensors[i]);
			break;
		case LTC2983_RTD_PT_10:
		case LTC2983_RTD_PT_50:
		case LTC2983_RTD_PT_100:
		case LTC2983_RTD_PT_200:
		case LTC2983_RTD_PT_500:
		case LTC2983_RTD_PT_1000:
		case LTC2983_RTD_1000:
		case LTC2983_RTD_NI_120:
		case LTC2983_RTD_CUSTOM:
			ltc2983_rtd_assign_chan(device, device->sensors[i]);
			break;
		case LTC2983_THERMISTOR_44004_44033:
		case LTC2983_THERMISTOR_44005_44030:
		case LTC2983_THERMISTOR_44007_44034:
		case LTC2983_THERMISTOR_44006_44031:
		case LTC2983_THERMISTOR_44008_44032:
		case LTC2983_THERMISTOR_YSI_400:
		case LTC2983_THERMISTOR_SPECTRUM_1003K:
		case LTC2983_THERMISTOR_CUSTOM_STEINHART_HART:
		case LTC2983_THERMISTOR_CUSTOM:
			ltc2983_thermistor_assign_chan(device,
						       device->sensors[i]);
			break;
		case LTC2983_DIODE:
			ltc2983_diode_assign_chan(device, device->sensors[i]);
			break;
		case LTC2983_RSENSE:
			ltc2983_r_sense_assign_chan(device, device->sensors[i]);
			break;
		case LTC2983_DIRECT_ADC:
			ltc2983_adc_assign_chan(device, device->sensors[i]);
			break;
		case LTC2983_ACTIVE_TEMP:
			ltc2983_temp_assign_chan(device, device->sensors[i]);
			break;
		default:
			return -EINVAL;
		}
	}

	return 0;
}

/**
 * @brief Read channel data / temperature
 * @param device - LTC2983 descriptor
 * @param chan - channel number
 * @param val - channel data / temperature
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_chan_read(struct ltc2983_desc *device, const int chan, int *val)
{
	uint32_t raw_val, scale_val, scale_val2;
	int ret;

	if (device->sensors[chan]->type == LTC2983_RSENSE) {
		*val = -1;
		return 0;
	}

	ret = ltc2983_chan_read_raw(device, chan, &raw_val);
	if (ret)
		return ret;

	ltc2983_chan_read_scale(device, chan, &scale_val, &scale_val2);
	if (ret)
		return ret;

	*val = no_os_sign_extend32(raw_val, 23) * scale_val / scale_val2;

	return 0;
}

/**
 * @brief Read raw channel data / temperature
 * @param device - LTC2983 descriptor
 * @param chan - channel number
 * @param val - raw channel data / temperature
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_chan_read_raw(struct ltc2983_desc *device, const int chan,
			  uint32_t *val)
{
	uint32_t start_conversion = 0;
	uint8_t raw_array[7];
	int ret;

	start_conversion = LTC2983_STATUS_START(true);
	start_conversion |= LTC2983_STATUS_CHAN_SEL(chan);
	/* start conversion */
	ret = ltc2983_reg_write(device, LTC2983_STATUS_REG, start_conversion);
	if (ret)
		return ret;

	no_os_mdelay(300);

	/* read the converted data */
	raw_array[0] = LTC2983_SPI_READ_BYTE;
	no_os_put_unaligned_be16(LTC2983_CHAN_RES_ADDR(chan), raw_array + 1);
	no_os_put_unaligned_be32(0, raw_array + 3);
	ret = no_os_spi_write_and_read(device->comm_desc, raw_array,
				       NO_OS_ARRAY_SIZE(raw_array));
	if (ret)
		return ret;

	*val = no_os_get_unaligned_be32(raw_array + 3);

	if (!(LTC2983_RES_VALID_MASK & *val)) {
		pr_err("Channel %d: Invalid conversion detected\r\n", chan);
		return -EIO;
	}

	if (device->sensors[chan]->type <= LTC2983_THERMOCOUPLE_CUSTOM)
		ret = ltc2983_thermocouple_fault_handler(*val);
	else
		ret = ltc2983_common_fault_handler(*val);
	if (ret)
		return ret;

	*val = no_os_sign_extend32((*val) & LTC2983_DATA_MASK,
				   LTC2983_DATA_SIGN_BIT);
	return 0;
}

/**
 * @brief Set scale of raw channel data / temperature
 * @param device - LTC2983 descriptor
 * @param chan - channel number
 * @param val - scale numerator
 * @param val2 - scale denominator
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_chan_read_scale(struct ltc2983_desc *device, const int chan,
			    uint32_t *val, uint32_t *val2)
{
	if (device->sensors[chan]->type == LTC2983_DIRECT_ADC) {
		/* value in millivolt */
		*val = 1000;
		/* 2^21 */
		*val2 = 2097152;
	} else {
		/* value in milli degrees */
		*val = 1000;
		/* 2^10 */
		*val2 = 1024;
	}
	return 0;
}

/**
 * @brief Channel assignment for common register fields
 * @param device - LTC2983 descriptor
 * @param sensor - LTC2983 common sensor descriptor
 * @param chan_val - channel assignment value to be written
 * @return 0 in case of success, errno errors otherwise
 */
static int __ltc2983_chan_assign_common(struct ltc2983_desc *device,
					const struct ltc2983_sensor *sensor,
					uint32_t chan_val)
{
	uint8_t raw_array[7];

	chan_val |= LTC2983_CHAN_TYPE(sensor->type);

	raw_array[0] = LTC2983_SPI_WRITE_BYTE;
	no_os_put_unaligned_be16(LTC2983_CHAN_START_ADDR(sensor->chan),
				 raw_array + 1);
	no_os_put_unaligned_be32(chan_val, raw_array + 3);

	return no_os_spi_write_and_read(device->comm_desc, raw_array,
					NO_OS_ARRAY_SIZE(raw_array));
}

/**
 * @brief Channel assignment for custom register fields
 * @param device - LTC2983 descriptor
 * @param custom - LTC2983 custom sensor descriptor
 * @param chan_val - channel assignment value to be written
 * @return 0 in case of success, errno errors otherwise
 */
static int __ltc2983_chan_custom_sensor_assign(struct ltc2983_desc *device,
		struct ltc2983_custom_sensor *custom,
		uint32_t *chan_val)
{
	int ret;
	uint32_t i;
	uint8_t raw_array[6];
	uint8_t step = custom->is_steinhart ? 4 : 3;

	if (device->custom_addr_ptr + (custom->len * step) >
	    LTC2983_CUST_SENS_TBL_SIZE)
		return -EINVAL;

	*chan_val |= LTC2983_CUSTOM_LEN(custom->len - 1);
	*chan_val |= LTC2983_CUSTOM_ADDR(device->custom_addr_ptr);

	raw_array[0] = LTC2983_SPI_WRITE_BYTE;
	for (i = 0; i < custom->len; i++, device->custom_addr_ptr += step) {
		no_os_put_unaligned_be16(device->custom_addr_ptr,
					 raw_array + 1);
		no_os_put_unaligned_be24(custom->table[i], raw_array + 3);
		ret = no_os_spi_write_and_read(device->comm_desc, raw_array,
					       NO_OS_ARRAY_SIZE(raw_array));
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Channel assignment for thermocouple sensors
 * @param device - LTC2983 descriptor
 * @param sensor - LTC2983 common sensor descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_thermocouple_assign_chan(struct ltc2983_desc *device,
				     const struct ltc2983_sensor *sensor)
{
	struct ltc2983_thermocouple *thermo = to_thermocouple(sensor);
	uint32_t chan_val;

	chan_val = LTC2983_CHAN_ASSIGN(thermo->cold_junction_chan);
	chan_val |= LTC2983_THERMOCOUPLE_CFG(thermo->sensor_config);

	if (thermo->custom) {
		int ret;

		ret = __ltc2983_chan_custom_sensor_assign(device,
				thermo->custom,
				&chan_val);
		if (ret)
			return ret;
	}
	return __ltc2983_chan_assign_common(device, sensor, chan_val);
}

/**
 * @brief Channel assignment for RTD sensors
 * @param device - LTC2983 descriptor
 * @param sensor - LTC2983 common sensor descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_rtd_assign_chan(struct ltc2983_desc *device,
			    const struct ltc2983_sensor *sensor)
{
	struct ltc2983_rtd *rtd = to_rtd(sensor);
	uint32_t chan_val;

	chan_val = LTC2983_CHAN_ASSIGN(rtd->r_sense_chan);
	chan_val |= LTC2983_RTD_CFG(rtd->sensor_config);
	chan_val |= LTC2983_RTD_EXC_CURRENT(rtd->excitation_current);
	chan_val |= LTC2983_RTD_CURVE(rtd->rtd_curve);

	if (rtd->custom) {
		int ret;

		ret = __ltc2983_chan_custom_sensor_assign(device, rtd->custom,
				&chan_val);
		if (ret)
			return ret;
	}
	return __ltc2983_chan_assign_common(device, sensor, chan_val);
}

/**
 * @brief Channel assignment for thermistor sensors
 * @param device - LTC2983 descriptor
 * @param sensor - LTC2983 common sensor descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_thermistor_assign_chan(struct ltc2983_desc *device,
				   const struct ltc2983_sensor *sensor)
{
	struct ltc2983_thermistor *thermistor = to_thermistor(sensor);
	uint32_t chan_val;

	chan_val = LTC2983_CHAN_ASSIGN(thermistor->r_sense_chan);
	chan_val |= LTC2983_THERMISTOR_CFG(thermistor->sensor_config);
	chan_val |=
		LTC2983_THERMISTOR_EXC_CURRENT(thermistor->excitation_current);

	if (thermistor->custom) {
		int ret;

		ret = __ltc2983_chan_custom_sensor_assign(device,
				thermistor->custom,
				&chan_val);
		if (ret)
			return ret;
	}
	return __ltc2983_chan_assign_common(device, sensor, chan_val);
}

/**
 * @brief Channel assignment for diode sensors
 * @param device - LTC2983 descriptor
 * @param sensor - LTC2983 common sensor descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_diode_assign_chan(struct ltc2983_desc *device,
			      const struct ltc2983_sensor *sensor)
{
	struct ltc2983_diode *diode = to_diode(sensor);
	uint32_t chan_val;

	chan_val = LTC2983_DIODE_CFG(diode->sensor_config);
	chan_val |= LTC2983_DIODE_EXC_CURRENT(diode->excitation_current);
	chan_val |=
		LTC2983_DIODE_IDEAL_FACTOR(
			(diode->ideal_factor_val.ideal_factor_val_int << 20) +
			diode->ideal_factor_val.ideal_factor_val_dec_1048576);

	return __ltc2983_chan_assign_common(device, sensor, chan_val);
}

/**
 * @brief Channel assignment for rsense
 * @param device - LTC2983 descriptor
 * @param sensor - LTC2983 common sensor descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_r_sense_assign_chan(struct ltc2983_desc *device,
				const struct ltc2983_sensor *sensor)
{
	struct ltc2983_rsense *rsense = to_rsense(sensor);
	uint32_t chan_val;

	chan_val =
		LTC2983_R_SENSE_VAL((rsense->r_sense_val.r_sense_val_int << 10) +
				    rsense->r_sense_val.r_sense_val_dec_1024);

	return __ltc2983_chan_assign_common(device, sensor, chan_val);
}

/**
 * @brief Channel assignment for direct ADC
 * @param device - LTC2983 descriptor
 * @param sensor - LTC2983 common sensor descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_adc_assign_chan(struct ltc2983_desc *device,
			    const struct ltc2983_sensor *sensor)
{
	struct ltc2983_adc *adc = to_adc(sensor);
	uint32_t chan_val;

	chan_val = LTC2983_ADC_SINGLE_ENDED(adc->single_ended);

	return __ltc2983_chan_assign_common(device, sensor, chan_val);
}

int ltc2983_temp_assign_chan(struct ltc2983_desc *device,
			     const struct ltc2983_sensor *sensor)
{
	struct ltc2983_temp *temp = to_temp(sensor);
	uint32_t chan_val;
	int ret;

	chan_val = LTC2983_ADC_SINGLE_ENDED(temp->single_ended);

	ret = __ltc2983_chan_custom_sensor_assign(device, temp->custom,
			&chan_val);
	if (ret)
		return ret;

	return __ltc2983_chan_assign_common(device, sensor, chan_val);
}

/**
 * @brief Fault handling of LTC2983 sensors
 * @param result - result captured that contains the faults
 * @param hard_mask - mask for hard faults
 * @param soft_mask - mask for soft faults
 * @return 0 in case of success, errno errors otherwise
 */
static int __ltc2983_fault_handler(const uint32_t result,
				   const uint32_t hard_mask,
				   const uint32_t soft_mask)
{
	if (result & hard_mask) {
		pr_info("Hard fault!\r\n");
		return -EIO;
	} else if (result & soft_mask) {
		/* just print a warning */
		pr_info("Soft fault!\r\n");
	}

	return 0;
}

/**
 * @brief Fault handling of thermocouple sensors
 * @param result - result captured that contains the faults
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_thermocouple_fault_handler(const uint32_t result)
{
	return __ltc2983_fault_handler(result,
				       LTC2983_THERMOCOUPLE_HARD_FAULT_MASK,
				       LTC2983_THERMOCOUPLE_SOFT_FAULT_MASK);
}

/**
 * @brief Fault handling of sensors other than thermocouple
 * @param result - result captured that contains the faults
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_common_fault_handler(const uint32_t result)
{
	return __ltc2983_fault_handler(result,
				       LTC2983_COMMON_HARD_FAULT_MASK,
				       LTC2983_COMMON_SOFT_FAULT_MASK);
}
