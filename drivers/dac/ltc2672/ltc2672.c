/***************************************************************************//**
 *   @file   ltc2672.c
 *   @brief  Implementation of ltc2672 Driver.
 *   @author JSanBuen (jose.sanbuenaventura@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "ltc2672.h"
#include "no_os_spi.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Device and communication init function
 * @param device - ltc2672 descriptor to be initialized
 * @param init_param - Initial parameter for descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2672_init(struct ltc2672_dev **device,
		 struct ltc2672_init_param *init_param)
{
	int ret;
	struct ltc2672_dev *descriptor;

	descriptor = (struct ltc2672_dev *)no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->comm_desc, &init_param->spi_init);
	if (ret)
		goto error_init;

	descriptor->id = init_param->id;

	*device = descriptor;

	return 0;

error_init:
	no_os_free(descriptor);
	return ret;
}

/**
 * @brief Remove resources allocated by the init function
 * @param device  - ltc2672 descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2672_remove(struct ltc2672_dev *device)
{
	int ret;

	if (!device)
		return -ENODEV;

	ret = no_os_spi_remove(device->comm_desc);

	no_os_free(device);

	return ret;
}

/**
 * @brief Write raw register value
 * @param device - ltc2672 descriptor
 * @param comm - command byte to send
 * @param is_32 - flag if command is 32 bit (True) or 24 bit (False) format
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_transaction(struct ltc2672_dev *device, uint32_t comm, bool is_32)
{
	int ret, i, bytes;
	uint8_t raw_array[4]; // highest bytes transmitted is 4 bytes
	uint32_t read_back = 0;

	if (is_32)
		bytes = 4;
	else
		bytes = 3;

	for (i = bytes; i > 0; i--)
		raw_array[i - 1] = (uint8_t)(comm >> (8 * (bytes - i)));

	ret = no_os_spi_write_and_read(device->comm_desc, raw_array, bytes);
	if (ret)
		return ret;

	for (i = bytes; i > 0; i --)
		read_back |= raw_array[i - 1] << (8 * (bytes - i));

	device->prev_command = read_back;

	return 0;
}

/**
 * @brief get dac code value from current of a selected DAC channel
 * @param device - ltc2672 descriptor
 * @param dac_current - current in uA
 * @param out_ch - DAC channel
 * @return code value
 */
uint32_t ltc2672_current_to_code(struct ltc2672_dev *device,
				 uint32_t dac_current,
				 enum ltc2672_dac_ch out_ch)
{
	uint32_t current_code;

	if (device->id == LTC2672_12)
		current_code = ((dac_current * LTC2672_12BIT_RESO) /
				device->max_currents[out_ch]);
	else
		current_code = ((dac_current * LTC2672_16BIT_RESO) /
				device->max_currents[out_ch]);

	return current_code;
}

/**
 * @brief sets the dac code for channel
 * @param device - ltc2672 descriptor
 * @param code - The code to be written
 * @param out_ch - channel to set the code
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_set_code_channel(struct ltc2672_dev *device, uint16_t code,
			     enum ltc2672_dac_ch out_ch)
{
	uint32_t command;

	if ((device->id == LTC2672_12 && code > LTC2672_12BIT_RESO)
	    || (device->id == LTC2672_16 && code > LTC2672_16BIT_RESO)
	    || out_ch < LTC2672_DAC0 || out_ch > LTC2672_DAC4)
		return -EINVAL;

	/* Switching to V- results in constant -80mA output */
	if (device->out_spans[out_ch] == LTC2672_VMINUS_VREF) {
		command = LTC2672_COMMAND24_GENERATE(LTC2672_CODE_PWRUP_UPD_CHANNEL_X,
						     out_ch,
						     LTC2672_DUMMY);

		return ltc2672_transaction(device, command, false);
	}

	if (device->id == LTC2672_12)
		code <<= LTC2672_BIT_SHIFT_12BIT;

	command = LTC2672_COMMAND24_GENERATE(LTC2672_CODE_PWRUP_UPD_CHANNEL_X,
					     out_ch,
					     code);

	return ltc2672_transaction(device, command, false);
}

/**
 * @brief set the current for a selected DAC channel
 * @param device - ltc2672 descriptor
 * @param current - current to set in uA
 * @param out_ch - channel to set the current
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_set_current_channel(struct ltc2672_dev *device,
				uint32_t current,
				enum ltc2672_dac_ch out_ch)
{
	uint32_t current_code;

	if (current > device->max_currents[out_ch] || current < LTC2672_OFF_CURRENT
	    || out_ch < LTC2672_DAC0 || out_ch > LTC2672_DAC4)
		return -EINVAL;

	current_code = ltc2672_current_to_code(device, current, out_ch);

	return ltc2672_set_code_channel(device, current_code, out_ch);
}

/**
 * @brief sets the same dac code for all channels
 * @param device - ltc2672 descriptor
 * @param code - The code to be written
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_set_code_all_channels(struct ltc2672_dev *device, uint16_t code)
{
	uint32_t command;

	if ((device->id == LTC2672_12 && code > LTC2672_12BIT_RESO)
	    || (device->id == LTC2672_16 && code > LTC2672_16BIT_RESO))
		return -EINVAL;

	if (device->out_spans[0] == LTC2672_VMINUS_VREF) {
		command = LTC2672_COMMAND24_GENERATE(LTC2672_CODE_PWRUP_UPD_CHANNEL_ALL,
						     LTC2672_DAC0,
						     LTC2672_DUMMY);

		return ltc2672_transaction(device, command, false);
	}

	if (device->id == LTC2672_12)
		code <<= LTC2672_BIT_SHIFT_12BIT;

	command = LTC2672_COMMAND24_GENERATE(LTC2672_CODE_PWRUP_UPD_CHANNEL_ALL,
					     LTC2672_DAC0,
					     (uint32_t)code);

	return ltc2672_transaction(device, command, false);
}

/**
 * @brief sets the same current for all channels
 * @param device - ltc2672 descriptor
 * @param current - current to set in uA
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_set_current_all_channels(struct ltc2672_dev *device,
				     uint32_t current)
{
	int i;
	uint32_t current_code;
	enum ltc2672_out_range out_range_sum = 0;

	for (i = 0; i < LTC2672_TOTAL_CHANNELS; i++)
		out_range_sum += device->out_spans[i];

	out_range_sum /= LTC2672_TOTAL_CHANNELS;

	/* Check if all channels have same span */
	if (out_range_sum != device->out_spans[0])
		return -EINVAL;

	current_code = ltc2672_current_to_code(device, current, 0);

	return ltc2672_set_code_all_channels(device, current_code);
}

/**
 * @brief sets output span for channel
 * @param device - ltc2672 descriptor
 * @param ch_span - max current span
 * @param out_ch - DAC channel output to set
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_set_span_channel(struct ltc2672_dev *device,
			     enum ltc2672_out_range ch_span
			     ,enum ltc2672_dac_ch out_ch) {
	int ret;
	uint32_t span = 0;
	uint32_t command;

	/* Check if span and channel are valid */
	if (ch_span < LTC2672_OFF || ch_span > LTC2672_4800VREF || out_ch < LTC2672_DAC0 || out_ch > LTC2672_DAC4)
		return -EINVAL;

	device->out_spans[out_ch] = ch_span;

	span = LTC2672_SPAN_SET(ch_span);

	command = LTC2672_COMMAND24_GENERATE(LTC2672_SPAN_TO_CHANNEL_X, out_ch, span);

	ret = ltc2672_transaction(device, command, false);
	if (ret)
		return ret;

	device->out_spans[out_ch] = ch_span;

	switch (ch_span)
	{
	case LTC2672_VMINUS_VREF:
		device->max_currents[out_ch] = LTC2672_VMINUS_FIXED_CURRENT *
		MICROAMPER_PER_MILLIAMPER;
		break;
	case LTC2672_4800VREF:
		device->max_currents[out_ch] = LTC2672_300MA_CURRENT *
		MICROAMPER_PER_MILLIAMPER;
		break;
	case LTC2672_OFF:
		device->max_currents[out_ch] = LTC2672_OFF_CURRENT * MICROAMPER_PER_MILLIAMPER;
		break;
	default:
		device->max_currents[out_ch] = NO_OS_BIT(ch_span - 1) *
		LTC2672_BASE_CURRENT * MICROAMPER_PER_MILLIAMPER;
		break;
	}

	return 0;
}

/**
 * @brief sets the same output span for all channels
 * @param device - ltc2672 descriptor
 * @param ch_span - max current span
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_set_span_all_channels(struct ltc2672_dev *device,
				  enum ltc2672_out_range ch_span)
{
	int ret, i;
	uint16_t span = 0;
	uint32_t command;

	if (ch_span < LTC2672_OFF || ch_span > LTC2672_4800VREF)
		return -EINVAL;

	for (i = 0; i < LTC2672_TOTAL_CHANNELS; i++)
		device->out_spans[i] = ch_span;

	span = LTC2672_SPAN_SET(ch_span);

	command = LTC2672_COMMAND24_GENERATE(LTC2672_SPAN_TO_CHANNEL_ALL, LTC2672_DAC0,
					     span);

	ret = ltc2672_transaction(device, command, false);
	if (ret)
		return ret;

	switch (ch_span) {
	case LTC2672_VMINUS_VREF:
		for (i = 0; i < LTC2672_TOTAL_CHANNELS; i++)
			device->max_currents[i] = LTC2672_VMINUS_FIXED_CURRENT *
						  MICROAMPER_PER_MILLIAMPER;
		break;
	case LTC2672_4800VREF:
		for (i = 0; i < LTC2672_TOTAL_CHANNELS; i++)
			device->max_currents[i] = LTC2672_300MA_CURRENT * MICROAMPER_PER_MILLIAMPER;
		break;
	case LTC2672_OFF:
		for (i = 0; i < LTC2672_TOTAL_CHANNELS; i++)
			device->max_currents[i] = LTC2672_OFF_CURRENT * MICROAMPER_PER_MILLIAMPER;
		break;
	default:
		for (i = 0; i < LTC2672_TOTAL_CHANNELS; i++)
			device->max_currents[i] = NO_OS_BIT(ch_span - 1) *
						  LTC2672_BASE_CURRENT * MICROAMPER_PER_MILLIAMPER;
		break;
	}

	return 0;
}

/**
 * @brief power down ltc2672
 * @param device - ltc2672 descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_chip_power_down(struct ltc2672_dev *device)
{
	uint32_t command;

	command = LTC2672_COMMAND24_GENERATE(LTC2672_PWRDWN_DEV, LTC2672_DAC0,
					     LTC2672_DUMMY);

	return ltc2672_transaction(device, command, false);
}

/**
 * @brief power down a channel
 * @param device - ltc2672 descriptor
 * @param out_ch - DAC channel output to set
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_power_down_channel(struct ltc2672_dev *device,
			       enum ltc2672_dac_ch out_ch)
{
	uint32_t command;

	if (out_ch < LTC2672_DAC0 || out_ch > LTC2672_DAC4)
		return -EINVAL;

	command = LTC2672_COMMAND24_GENERATE(LTC2672_PWRDWN_CHANNEL_X, out_ch,
					     LTC2672_DUMMY);

	return ltc2672_transaction(device, command, false);
}

/**
 * @brief power down ALL channels
 * @param device - ltc2672 descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_power_down_all_channels(struct ltc2672_dev *device)
{
	int ret;
	uint32_t command;
	enum ltc2672_dac_ch i;

	for (i = LTC2672_DAC0; i < LTC2672_TOTAL_CHANNELS; i++) {
		command = LTC2672_COMMAND24_GENERATE(LTC2672_PWRDWN_CHANNEL_X, i,
						     LTC2672_DUMMY);

		ret = ltc2672_transaction(device, command, false);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief configures mux output depending on target measurement
 * @param device - ltc2672 descriptor
 * @param mux_comm - multiplexer command code
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_monitor_mux(struct ltc2672_dev *device,
			enum ltc2672_mux_commands mux_comm)
{
	uint32_t mux = 0;
	uint32_t command;

	if (mux_comm < LTC2672_MUX_DISABLED || mux_comm > LTC2672_MUX_VOUT4)
		return -EINVAL;

	mux = LTC2672_MUX_SET(mux_comm);

	command = LTC2672_MUX24_GENERATE(LTC2672_MON_MUX, mux);

	return ltc2672_transaction(device, command, false);
}

/**
 * @brief setup toggle parameters for a channel
 * @param device - ltc2672 descriptor
 * @param out_ch - channel to toggle
 * @param current_reg_a - current to write in register A
 * @param current_reg_b - current to write in register B
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_setup_toggle_channel(struct ltc2672_dev *device,
				 enum ltc2672_dac_ch out_ch
				 ,uint32_t current_reg_a, uint32_t current_reg_b) {
	int ret;
	uint32_t current_code;
	uint32_t command;

	if (device->out_spans[out_ch] == LTC2672_VMINUS_VREF || device->out_spans[out_ch] == LTC2672_OFF)
		return -EINVAL;

	if (out_ch < LTC2672_DAC0 || out_ch > LTC2672_DAC4)
		return -EINVAL;

	if ((current_reg_a < 0 && device->out_spans[out_ch] != LTC2672_VMINUS_VREF)
	    || (current_reg_b < 0 && device->out_spans[out_ch] != LTC2672_VMINUS_VREF)
	    || (current_reg_a > device->max_currents[out_ch])
	    || (current_reg_b > device->max_currents[out_ch]))
		return -EINVAL;

	/* Write code to Register A of out_ch */
	current_code = ltc2672_current_to_code(device, current_reg_a, out_ch);

	if (device->id == LTC2672_12)
		current_code <<= LTC2672_BIT_SHIFT_12BIT;

	command = LTC2672_COMMAND24_GENERATE(LTC2672_CODE_TO_CHANNEL_X, out_ch, current_code);

	ret = ltc2672_transaction(device, command, false);
	if (ret)
		return ret;

	/* Toggle to enable register B*/
	command = LTC2672_COMMAND24_GENERATE(LTC2672_TOGGLE_SEL, LTC2672_DAC0, NO_OS_BIT(out_ch));

	ret = ltc2672_transaction(device, command, false);
	if (ret)
		return ret;

	/* Write code to Register B of out_ch */
	current_code = ltc2672_current_to_code(device, current_reg_b, out_ch);

	if (device->id == LTC2672_12)
		current_code <<= LTC2672_BIT_SHIFT_12BIT;

	command = LTC2672_COMMAND24_GENERATE(LTC2672_CODE_TO_CHANNEL_X, out_ch,
					     current_code);

	ret = ltc2672_transaction(device, command, false);
	if (ret)
		return ret;

	/* Toggle to enable register A*/
	command = LTC2672_COMMAND24_GENERATE(LTC2672_TOGGLE_SEL, LTC2672_DAC0, NO_OS_BIT(out_ch));

	command &= ~NO_OS_BIT(out_ch);

	return ltc2672_transaction(device, command, false);
}

/**
 * @brief enables or disables selected channel/s for toggling
 * @param device - ltc2672 descriptor
 * @param mask - mask that describes which channel/s to toggle
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_enable_toggle_channel(struct ltc2672_dev *device, uint32_t mask)
{
	uint32_t command;

	if (mask > LTC2672_MAX_TOGGLE_MASK)
		return -EINVAL;

	command = LTC2672_COMMAND24_GENERATE(LTC2672_TOGGLE_SEL, LTC2672_DAC0, mask);

	return ltc2672_transaction(device, command, false);
}

/**
 * @brief enables or disables global toggle bit
 * @param device - ltc2672 descriptor
 * @param is_enable - sets or resets the global toggle bit
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_global_toggle(struct ltc2672_dev *device, bool is_enable)
{
	uint32_t command;

	device->global_toggle = is_enable;

	command = LTC2672_COMMAND24_GENERATE(LTC2672_TOGGLE_GLBL, LTC2672_DAC0,
					     (uint8_t)device->global_toggle);

	return ltc2672_transaction(device, command, false);
}

/**
 * @brief configures the fault detection bits
 * @param device - ltc2672 descriptor
 * @param mask - mask that describes desired configuration
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_config_command(struct ltc2672_dev *device, uint8_t mask)
{
	uint32_t command;

	if (mask > LTC2672_MAX_CONFIG_MASK)
		return -EINVAL;

	command = LTC2672_COMMAND24_GENERATE(LTC2672_CNFG_CMD, LTC2672_DAC0, mask);

	return ltc2672_transaction(device, command, false);
}
