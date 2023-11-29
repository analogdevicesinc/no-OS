/***************************************************************************//**
 *   @file   adp1050.c
 *   @brief  Source file for the ADP1050 Driver
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <string.h>
#include "adp1050.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"

/**
 * @brief Send command byte/word to ADP1050
 * @param desc - ADP1050 device descriptor
 * @param command - Value of the command.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_send_command(struct adp1050_desc *desc, uint16_t command)
{
	uint8_t data[2];
	uint8_t command_val;

	if (!desc)
		return -EINVAL;

	if (command > ADP1050_EXTENDED_COMMAND) {
		data[0] = no_os_field_get(ADP1050_LSB_MASK, command);
		data[1] = no_os_field_get(ADP1050_MSB_MASK, command);

		return no_os_i2c_write(desc->i2c_desc, data, 2, 1);
	}

	command_val = no_os_field_get(ADP1050_LSB_MASK, command);

	return no_os_i2c_write(desc->i2c_desc, &command_val, 1, 1);
}

/**
 * @brief Read data from ADP1050
 * @param desc - ADP1050 device descriptor
 * @param command - Command value.
 * @param data - Buffer with received data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_read(struct adp1050_desc *desc, uint16_t command, uint8_t *data,
		 uint8_t bytes_number)
{
	int ret;
	uint8_t command_val[2] = {0, 0};
	uint8_t write_bytes;

	if (!desc)
		return -EINVAL;

	if (command > ADP1050_EXTENDED_COMMAND) {
		command_val[1] = no_os_field_get(ADP1050_LSB_MASK, command);
		command_val[0] = no_os_field_get(ADP1050_MSB_MASK, command);
		write_bytes = 2;
	} else {
		command_val[0] = no_os_field_get(ADP1050_LSB_MASK, command);
		write_bytes = 1;
	}
	ret = no_os_i2c_write(desc->i2c_desc, command_val, write_bytes, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(desc->i2c_desc, data, bytes_number, 1);
}

/**
 * @brief Write data to ADP1050
 * @param desc - ADP1050 device descriptor
 * @param command - Command value
 * @param data - Data value to write to the ADP1050. Can be either just a byte
 * 		 or a word.
 * @param bytes_number - Number of bytes to write.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_write(struct adp1050_desc *desc, uint16_t command, uint16_t data,
		  uint8_t bytes_number)
{
	uint8_t val[4] = {0, 0, 0, 0};

	if (!desc)
		return -EINVAL;

	if (command > ADP1050_EXTENDED_COMMAND) {
		val[0] = no_os_field_get(ADP1050_MSB_MASK, command);
		val[1] = no_os_field_get(ADP1050_LSB_MASK, command);
		if (bytes_number > 1) {
			val[2] = no_os_field_get(ADP1050_LSB_MASK, data);
			val[3] = no_os_field_get(ADP1050_MSB_MASK, data);
		} else
			val[2] = no_os_field_get(ADP1050_LSB_MASK, data);

		return no_os_i2c_write(desc->i2c_desc, val, bytes_number + 2, 1);
	} else {
		val[0] = no_os_field_get(ADP1050_LSB_MASK, command);
		if (bytes_number > 1) {
			val[1] = no_os_field_get(ADP1050_LSB_MASK, data);
			val[2] = no_os_field_get(ADP1050_MSB_MASK, data);
		} else
			val[1] = no_os_field_get(ADP1050_LSB_MASK, data);

		return no_os_i2c_write(desc->i2c_desc, val, bytes_number + 1, 1);
	}
}

/**
 * @brief Read statuses
 * @param desc - ADP1050 device descriptor
 * @param status - Status type.
 * @param status_val - Status value to be returned.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_read_status(struct adp1050_desc *desc,
			enum adp1050_status_type status,
			uint16_t *status_val)
{
	uint8_t read_byte;
	uint8_t data[2];
	int ret;

	switch (status) {
	case ADP1050_STATUS_VOUT_TYPE:
	case ADP1050_STATUS_INPUT_TYPE:
	case ADP1050_STATUS_TEMPERATURE_TYPE:
	case ADP1050_STATUS_CML_TYPE:
		ret = adp1050_read(desc, (uint16_t)status, &read_byte, 1);
		if (ret)
			return ret;

		*status_val = read_byte;

		return 0;
	case ADP1050_STATUS_WORD_TYPE:
		ret = adp1050_read(desc, (uint16_t)status, data, 2);
		if (ret)
			return ret;

		*status_val = no_os_get_unaligned_le16(data);

		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read Voltage Sense output raw value from the ADP1050
 * @param desc - ADP1050 device descriptor
 * @param vsense - VS(VOUT) raw value to be received of the parallel circuit.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_read_vsense(struct adp1050_desc *desc, uint16_t *vsense)
{
	int ret;
	uint8_t data[2];

	ret = adp1050_read(desc, ADP1050_READ_VOUT, data, 2);
	if (ret)
		return ret;

	*vsense = no_os_get_unaligned_le16(data);

	return 0;
}

/**
 * @brief Read VIN/IIN/TEMP/DUTY_CYCLE/FREQ raw value from the ADP1050
 * @param desc - ADP1050 device descriptor
 * @param mant - Raw mantissa value to be received.
 * @param exp - Raw exponent value to be received.
 * @param val_type - Type of avlue to be read.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_read_value(struct adp1050_desc *desc, uint16_t *mant, uint8_t *exp,
		       enum adp1050_value_type val_type)
{
	int ret;
	uint8_t data[2];

	ret = adp1050_read(desc, val_type, data, 2);
	if (ret)
		return ret;

	*mant = no_os_field_get(ADP1050_MANT_MASK,
				no_os_get_unaligned_le16(data));
	*exp = no_os_field_get(ADP1050_EXP_MASK,
			       no_os_get_unaligned_le16(data));

	return 0;
}

/**
 * @brief Set VOUT_COMMAND and VOUT_MAX values
 * @param desc - ADP1050 device descriptor
 * @param vout_command - VOUT_COMMAND value.
 * @param vout_max - VOUT_MAX value.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_vout_value(struct adp1050_desc *desc, uint16_t vout_command,
		       uint16_t vout_max)
{
	int ret;

	ret = adp1050_write(desc, ADP1050_VOUT_COMMAND, vout_command, 2);
	if (ret)
		return ret;

	return adp1050_write(desc, ADP1050_VOUT_MAX, vout_max, 2);
}

/**
 * @brief Set output voltage offset
 * @param desc - ADP1050 device descriptor
 * @param vout_offset - VOUT offset value.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_vout_offset(struct adp1050_desc *desc, int16_t vout_offset)
{
	return adp1050_write(desc, ADP1050_VOUT_CAL_OFFSET, vout_offset, 2);
}

/**
 * @brief Set ADP1050 VOUT transition rate
 * @param desc - ADP1050 device descriptor
 * @param vout_tr - VOUT transition rate value to be set.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_vout_tr(struct adp1050_desc *desc, enum adp1050_vout_tr vout_tr)
{
	return adp1050_write(desc, ADP1050_VOUT_TRANSITION_RATE, vout_tr, 2);
}

/**
 * @brief Set ADP1050 VOUT scale, regarding the resistor divider assigned to
 * 	  the VS pins
 * @param desc - ADP1050 device descriptor
 * @param exp - Exponent - 5-bit twos complement
 * @param mant - Mantissa - 11-bit twos complement
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_vout_scale(struct adp1050_desc *desc, int8_t exp, uint16_t mant)
{
	uint16_t vout_scale;

	if (exp > ADP1050_EXP_MAX || mant > ADP1050_MANT_MAX)
		return -EINVAL;

	vout_scale = no_os_field_prep(ADP1050_EXP_MASK, exp) | mant;

	return adp1050_write(desc, ADP1050_VOUT_SCALE_MONITOR, vout_scale, 2);
}

/**
 * @brief Set VOUT margins value
 * @param desc - ADP1050 device descriptor
 * @param margin_high - VOUT margin high value (Y value for V = Y x 2^N format),
 * 			N defined in VOUT_MODE (-10).
 * @param margin_low - VOUT margin low value (Y value for V = Y x 2^N format),
 * 		       N defined in VOUT_MODE (-10).
 * @return 0 in case of succes, negative error code otherwise.
*/
int adp1050_vout_margin(struct adp1050_desc *desc, uint16_t margin_high,
			uint16_t margin_low)
{
	int ret;

	ret = adp1050_write(desc, ADP1050_VOUT_MARGIN_HIGH, margin_high, 2);
	if (ret)
		return ret;

	return adp1050_write(desc, ADP1050_VOUT_MARGIN_LOW, margin_low, 2);
}

/**
 * @brief Set VOUT duty cycle for requested channel
 * @param desc - ADP1050 device descriptor
 * @param pulse_width - Width of the pulse wanted for the signal (used for
 * 			calculation of the 12-bit rising and falling edge
 * 			timings for the requested channels, it depends on each
 * 			channel how the falling and rising edges are calculated
 * 			using this value). Each LSB corresponds to a 5ns
 * 			resolution.
 * @param pulse_start - Start of the pulse wanted for the signal (used for
 * 			calculation of the 12-bit rising and falling edge
 * 			timings for the requested channels, it depends on each
 * 			channel how the falling and rising edges are calculated
 * 			using this value). Each LSB corresponds to a 5ns
 * 			resolution.
 * @param chan - Requested channel.
 * @return 0 in case of succes, negative error code otherwise.
*/
int adp1050_pwm_duty_cycle(struct adp1050_desc *desc, uint16_t pulse_width,
			   uint16_t pulse_start, enum adp1050_channel chan)
{
	int ret;
	uint8_t redge_msb, fedge_msb, lsb;
	uint16_t reg_redge, reg_fedge, reg_lsb;

	if (no_os_field_get(ADP1050_FLOAT_FREQ_MASK, desc->freq)
	    && pulse_width + pulse_start > ADP1050_MAX_PERIOD_FLOAT(desc->freq))
		return -EINVAL;

	if (!no_os_field_get(ADP1050_FLOAT_FREQ_MASK, desc->freq)
	    && pulse_width + pulse_start > ADP1050_MAX_PERIOD_INTEGER(desc->freq))
		return -EINVAL;

	ret = adp1050_set_pwm(desc, ADP1050_DISABLE_ALL, desc->freq);
	if (ret)
		return ret;

	switch (chan) {
	case ADP1050_OUTA:
		fedge_msb = no_os_field_get(ADP1050_EDGE_MSB_MASK, pulse_width + pulse_start);
		redge_msb = no_os_field_get(ADP1050_EDGE_MSB_MASK, pulse_start);
		lsb = no_os_field_get(ADP1050_FALLING_EDGE_LSB_MASK,
				      pulse_width + pulse_start) | no_os_field_get(ADP1050_RISING_EDGE_LSB_MASK,
						      pulse_start);

		reg_fedge = ADP1050_OUTA_FALLING_EDGE_TIMING;
		reg_redge = ADP1050_OUTA_RISING_EDGE_TIMING;
		reg_lsb = ADP1050_OUTA_RISING_FALLING_TIMING_LSB;

		break;
	case ADP1050_OUTB:
		fedge_msb = no_os_field_get(ADP1050_EDGE_MSB_MASK, pulse_width + pulse_start);
		redge_msb = no_os_field_get(ADP1050_EDGE_MSB_MASK, pulse_start);
		lsb = no_os_field_get(ADP1050_FALLING_EDGE_LSB_MASK,
				      pulse_width + pulse_start) | no_os_field_get(ADP1050_RISING_EDGE_LSB_MASK,
						      pulse_start);

		reg_fedge = ADP1050_OUTB_FALLING_EDGE_TIMING;
		reg_redge = ADP1050_OUTB_RISING_EDGE_TIMING;
		reg_lsb = ADP1050_OUTB_RISING_FALLING_TIMING_LSB;

		break;
	case ADP1050_SR1:
		redge_msb = no_os_field_get(ADP1050_EDGE_MSB_MASK, pulse_width + pulse_start);
		fedge_msb = no_os_field_get(ADP1050_EDGE_MSB_MASK, pulse_start);
		lsb = no_os_field_get(ADP1050_RISING_EDGE_LSB_MASK,
				      pulse_width + pulse_start) | no_os_field_get(ADP1050_FALLING_EDGE_LSB_MASK,
						      pulse_start);

		reg_fedge = ADP1050_SR1_FALLING_EDGE_TIMING;
		reg_redge = ADP1050_SR1_RISING_EDGE_TIMING;
		reg_lsb = ADP1050_SR1_RISING_FALLING_TIMING_LSB;

		break;
	case ADP1050_SR2:
		redge_msb = no_os_field_get(ADP1050_EDGE_MSB_MASK, pulse_width + pulse_start);
		fedge_msb = no_os_field_get(ADP1050_EDGE_MSB_MASK, pulse_start);
		lsb = no_os_field_get(ADP1050_RISING_EDGE_LSB_MASK,
				      pulse_width + pulse_start) | no_os_field_get(ADP1050_FALLING_EDGE_LSB_MASK,
						      pulse_start);

		reg_fedge = ADP1050_SR2_FALLING_EDGE_TIMING;
		reg_redge = ADP1050_SR2_RISING_EDGE_TIMING;
		reg_lsb = ADP1050_SR2_RISING_FALLING_TIMING_LSB;

		break;
	default:
		return -EINVAL;
	}

	ret = adp1050_write(desc, reg_fedge, fedge_msb, 1);
	if (ret)
		return ret;

	ret = adp1050_write(desc, reg_redge, redge_msb, 1);
	if (ret)
		return ret;

	return adp1050_write(desc, reg_lsb, lsb, 1);
}

/**
 * @brief Set PWM modulation for requested channel
 * @param desc - ADP1050 device descriptor
 * @param mod - Type of modulation.
 * @param chan - Requested channel.
 * @param sign - true - Positive modulation sign
 * 		 false - Negative modulation sign.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_pwm_modulation(struct adp1050_desc *desc, enum adp1050_mod mod,
			   enum adp1050_channel chan, bool sign)
{
	int ret;
	uint8_t data;
	uint32_t mask, reg;

	switch (mod) {
	case ADP1050_OUTA_SR1_FALLING_MOD:
		mask = ADP1050_OUTA_SR1_FALLING_MOD_MASK;
		break;
	case ADP1050_OUTA_SR1_RISING_MOD:
		mask = ADP1050_OUTA_SR1_RISING_MOD_MASK;
		break;
	case ADP1050_OUTB_SR2_FALLING_MOD:
		mask = ADP1050_OUTB_SR2_FALLING_MOD_MASK;
		break;
	case ADP1050_OUTB_SR2_RISING_MOD:
		mask = ADP1050_OUTB_SR2_RISING_MOD_MASK;
		break;
	default:
		return -EINVAL;
	}

	switch (chan) {
	case ADP1050_OUTA:
	case ADP1050_OUTB:
		reg = ADP1050_OUTA_OUTB_MODULATION_SETTINGS;
		break;
	case ADP1050_SR1:
	case ADP1050_SR2:
		reg = ADP1050_SR1_SR2_MODULATION_SETTINGS;
		break;
	default:
		return -EINVAL;
	}

	ret = adp1050_read(desc, reg, &data, 1);
	if (ret)
		return ret;

	data &= ~mask;
	data |= no_os_field_prep(mask,
				 sign ? ADP1050_NEGATIVE_MOD_SIGN : ADP1050_POSITIVE_MOD_SIGN);

	return adp1050_write(desc, reg, data, 1);
}

/**
 * @brief Set PWM channel and frequency
 * @param desc - ADP1050 device descriptor
 * @param chan - Selected channel.
 * @param freq - Selected frequency.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_set_pwm(struct adp1050_desc *desc, enum adp1050_channel chan,
		    enum adp1050_freq freq)
{
	int ret;
	uint16_t reg_val;

	ret = adp1050_write(desc, ADP1050_FREQUENCY_SWITCH, (uint16_t)freq, 2);
	if (ret)
		return ret;

	desc->freq = freq;

	switch (chan) {
	case ADP1050_OUTA:
		reg_val = ADP1050_OUTA_ON;
		break;
	case ADP1050_OUTB:
		reg_val = ADP1050_OUTB_ON;
		break;
	case ADP1050_SR1:
		reg_val = ADP1050_SR1_ON;
		break;
	case ADP1050_SR2:
		reg_val = ADP1050_SR2_ON;
		break;
	case ADP1050_DISABLE_ALL:
		reg_val = ADP1050_ALL_PWM_OFF;
		break;
	default:
		return -EINVAL;
	}

	ret = adp1050_write(desc, ADP1050_PWM_OUTPUT_DISABLE, reg_val, 1);
	if (ret)
		return ret;

	return adp1050_write(desc, ADP1050_GO_COMMANDS, ADP1050_VOUT_GO, 1);
}

/**
 * @brief Set ADP1050 VIN on/off raw value for input voltage limiting.
 * @param desc - ADP1050 device descriptor
 * @param mantissa - Mantissa value to be transmitted, 11 bit twos complement.
 * @param exp - Exponent value to be transmitted, 5 bit twos complement.
 * @param state_on - true - Set VIN_ON.
 * 		     false - Set VIN_OFF.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_set_vin(struct adp1050_desc *desc, int16_t mantissa, int8_t exp,
		    bool state_on)
{
	uint16_t val;

	if (mantissa > ADP1050_MANT_MAX || exp > ADP1050_EXP_MAX)
		return -EINVAL;

	val = no_os_field_prep(ADP1050_EXP_MASK, exp) | mantissa;

	return adp1050_write(desc, state_on ? ADP1050_VIN_ON : ADP1050_VIN_OFF,
			     val, 2);
}

/**
 * @brief Set current limit fast fault for overcurrent protection and current
 * 	  limit cycle-by-cycle reference
 * @param desc - ADP1050_device_descriptor.
 * @param fast_fault - Fast fault limit value for N counter.
 * @param climit_ref - Current limit cylcle-by-cycle reference.
 * @return 0 in case of succes, negative error code otherwise.
*/
int adp1050_set_climit(struct adp1050_desc *desc,
		       enum adp1050_iin_oc_fast_fault fast_fault,
		       enum adp1050_climit_ref climit_ref)
{
	int ret;

	ret = adp1050_write(desc, ADP1050_IIN_OC_FAST_FAULT_LIMIT,
			    (uint16_t)fast_fault, 1);
	if (ret)
		return ret;

	return adp1050_write(desc, ADP1050_CS1_CBC_CURR_LIM_REF,
			     (uint16_t)climit_ref, 1);
}

/**
 * @brief Set current sense settings
 * @param desc - ADP1050 device descriptor
 * @param leading_edge - Leading edge blanking time.
 * @param debounce - Current sense cycle-by-cycle current-limit debounce time.
 * @param ignore - CS1 overcurrent protection comparator to be ignored.
 * @return 0 in case of succes, negative error code otherwise.
*/
int adp1050_set_cs1_settings(struct adp1050_desc *desc,
			     enum adp1050_cs1_leading_edge leading_edge,
			     enum adp1050_cs1_debounce debounce,
			     bool ignore)
{
	uint16_t val;

	if (ignore)
		return adp1050_write(desc, ADP1050_CS1_CBC_CURR_LIM_SETTINGS,
				     ADP1050_CS1_CLIMIT_COMP_IGNORE, 1);

	val = no_os_field_prep(ADP1050_CS1_LEADING_EDGE_MASK, leading_edge) |
	      no_os_field_prep(ADP1050_CS1_DEBOUNCE_MASK, debounce);

	return adp1050_write(desc, ADP1050_CS1_CBC_CURR_LIM_SETTINGS, val, 1);
}

/**
 * @brief Set requested ADP1050 channel in Open Loop operation mode.
 * @param desc - ADP1050 device descriptor.
 * @param rising_edge - Rising edge timing.
 * @param falling_edge - Falling edge timing.
 * @param chan - Channel t be set in open-loop.
 * @return 0 in case of succes, negative error code otherwise.
*/
int adp1050_set_open_loop(struct adp1050_desc *desc, uint8_t rising_edge,
			  uint8_t falling_edge, enum adp1050_channel chan)
{
	int ret;

	if ((chan == ADP1050_OUTA || chan == ADP1050_OUTB)
	    && falling_edge < rising_edge)
		return -EINVAL;

	if ((chan == ADP1050_SR1 || chan == ADP1050_SR2) && rising_edge < falling_edge)
		return -EINVAL;

	ret = adp1050_set_pwm(desc, ADP1050_DISABLE_ALL, desc->freq);
	if (ret)
		return ret;

	ret = adp1050_write(desc, ADP1050_FEEDFORWARD_SS_FILTER_GAIN,
			    ADP1050_CL_FEEDFORWARD_DISABLE, 1);
	if (ret)
		return ret;

	switch (chan) {
	case ADP1050_OUTA:
	case ADP1050_OUTB:
		ret = adp1050_pwm_duty_cycle(desc, falling_edge, falling_edge - rising_edge,
					     chan);
		break;
	case ADP1050_SR1:
	case ADP1050_SR2:
		ret = adp1050_pwm_duty_cycle(desc, rising_edge - falling_edge, falling_edge,
					     chan);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	/* Modulation limit needs to be set to 0us. */
	ret = adp1050_write(desc, ADP1050_MODULATION_LIMIT, 0, 1);
	if (ret)
		return ret;

	ret = adp1050_write(desc, ADP1050_OL_OPERATION_SETTINGS,
			    ADP1050_OPEN_LOOP_ENABLE, 1);
	if (ret)
		return ret;

	if (chan == ADP1050_OUTA || chan == ADP1050_OUTB) {
		ret = adp1050_write(desc, ADP1050_SOFT_START_SETTING_OL,
				    ADP1050_OL_SS_64_CYCLES, 1);
		if (ret)
			return ret;
	}

	desc->loop = ADP1050_OPEN_LOOP;

	return 0;
}

/**
 * @brief Set close loop operation
 * @param desc - ADP1050 device descriptor
 * @return 0 in case of succes, negative error code otherwise.
*/
int adp1050_set_close_loop(struct adp1050_desc *desc)
{
	int ret;

	ret = adp1050_set_pwm(desc, ADP1050_DISABLE_ALL, desc->freq);
	if (ret)
		return ret;

	ret = adp1050_write(desc, ADP1050_OL_OPERATION_SETTINGS,
			    ADP1050_OPEN_LOOP_DISABLE, 1);
	if (ret)
		return ret;

	desc->loop = ADP1050_CLOSE_LOOP;

	return 0;
}

/**
 * @brief Enable or disbale the pulse skipping mode
 * @param desc - ADP1050 device descriptor.
 * @param pulse_threshold - Pulse threshold value, each LSB corresponds to a
 * 			    5ns resolution.
 * @param state_on - true - enable pulse skipping
 * 		     false - disable pulse skipping
 * @return 0 in case of succes, negative error code otherwise.
*/
int adp1050_pulse_skipping(struct adp1050_desc *desc, uint8_t pulse_threshold,
			   bool state_on)
{
	int ret;

	if (desc->loop == ADP1050_CLOSE_LOOP)
		return -EINVAL;

	if (state_on) {
		ret = adp1050_write(desc, ADP1050_PULSE_SKIPPING_MODE_THR, pulse_threshold, 1);
		if (ret)
			return ret;

		return adp1050_write(desc, ADP1050_OL_OPERATION_SETTINGS,
				     ADP1050_PULSE_SKIPPING_ENABLE, 1);
	}

	return adp1050_write(desc, ADP1050_OL_OPERATION_SETTINGS,
			     ADP1050_PULSE_SKIPPING_DISABLE, 1);
}

/**
 * @brief Enable or disable voltage input feedforward for edge modulation.
 * @param desc - ADP1050 device descriptor.
 * @param state_on - State of the feedforward to be set. If set true, every
 * 		     value above the 1V up until the 1.6V on the VF pin will
 * 		     decrease the time of the rising and falling edges, and
 * 		     every value below the 1V down until the 0.5V on the VF pin
 * 		     will increase the value of the rising and falling edges.
 * @return 0 in case of succes, negative error code otherwise.
 */
int adp1050_set_feedforward(struct adp1050_desc *desc, bool state_on)
{
	if (state_on) {
		if (desc->loop == ADP1050_CLOSE_LOOP) {
			return adp1050_write(desc, ADP1050_FEEDFORWARD_SS_FILTER_GAIN,
					     ADP1050_FEEDFORWARD_CL_ENABLE, 1);
		}

		return adp1050_write(desc, ADP1050_FEEDFORWARD_SS_FILTER_GAIN,
				     ADP1050_FEEDFORWARD_OL_ENABLE, 1);
	}

	return adp1050_write(desc, ADP1050_FEEDFORWARD_SS_FILTER_GAIN,
			     ADP1050_FEEDFORWARD_OFF, 1);
}

/**
 * @brief Frequency synchronization of the PWM clock with an external clock
 * 	  Requires syni_desc to be initialized.
 * @param desc - ADP1050 device descriptor
 * @param state_on - true - Turn on frequency synchronization.
 * 		     false - Turn off frequency synchronization.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_freq_sync(struct adp1050_desc *desc, bool state_on)
{
	if (!desc->syni_desc)
		return -EINVAL;

	return adp1050_write(desc, ADP1050_SYNCH_GENERAL_SETTINGS,
			     state_on ? ADP1050_FREQ_SYNC_ON : ADP1050_FREQ_SYNC_OFF, 1);
}

/**
 * @brief Set FLAGIN response to FLGI pin signal
 * @param desc - ADP1050 device descriptor
 * @param fault_response - Device response to flag being set.
 * @param after_fault - Action when the falg is cleared.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_set_flgi_response(struct adp1050_desc *desc,
			      enum adp1050_flgi_fault_response fault_response,
			      enum adp1050_flgi_after_fault after_fault)
{
	uint8_t reg_val;

	if (!desc->flgi_desc)
		return -EINVAL;

	reg_val = no_os_field_prep(ADP1050_FAULT_RESPONSE_MASK, fault_response)
		  | no_os_field_prep(ADP1050_AFTER_FAULT_MASK, after_fault);

	return adp1050_write(desc, ADP1050_FLAGIN_RESPONSE, reg_val, 1);
}

/**
 * @brief Adjust filter settings of the normal mode compensator
 * @param desc - ADP1050 device descriptor
 * @param zero - Normal mode zero settings.
 * @param pole - Normal mode pole settings.
 * @param lf - Normal mode low filter gain settings.
 * @param hf - Normal mode high filter gain settings.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_filter(struct adp1050_desc *desc, uint8_t zero, uint8_t pole,
		   uint8_t lf, uint8_t hf)
{
	int ret;

	ret = adp1050_write(desc, ADP1050_NORMAL_MODE_COMP_LOW_FREQ, lf, 1);
	if (ret)
		return ret;

	ret = adp1050_write(desc, ADP1050_NORMAL_MODE_COMP_ZERO, zero, 1);
	if (ret)
		return ret;

	ret = adp1050_write(desc, ADP1050_NORMAL_MODE_COMP_POLE, pole, 1);
	if (ret)
		return ret;

	return adp1050_write(desc, ADP1050_NORMAL_MODE_COMP_HIGH_FREQ, hf, 1);
}

/**
 * @brief Trim IIN(CS1 pin)/VIN(VF pin)/VOUT(VS pin).
 * @param desc - ADP1050 device descriptor
 * @param trim_type - Type of trim requested.
 * @param vout_command - VOUT_COMMAND value to be set in case of a VOUT trim,
 * 			 if a VOUT trim isn't requested, this parameter won't be
 * 			 used.
 * @param vout_scale - VOUT_SCALE value to be set in case of a VOUT trim,
 * 		       if a VOUT trim isn't requested, this parameter won't be
 * 		       used.
 * @param trim_val - This parameter will only be used in case of either a VIN or
 * 		     VOUT trim.
 * 		     VIN trim - Value of VIN_SCALE_MONITOR to be set.
 * 		     VOUT trim - Value of output voltage set on the VS pin.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_trim(struct adp1050_desc *desc, enum adp1050_trim_type trim_type,
		 uint16_t vout_command, uint16_t vout_scale, uint16_t trim_val)
{
	int ret;
	uint8_t data[2];
	uint16_t read_val;
	uint16_t init_val = 0;

	switch (trim_type) {
	case ADP1050_CS1_IIN_TRIM:
		ret = adp1050_read(desc, ADP1050_READ_IIN, data, 2);
		if (ret)
			return ret;

		read_val = no_os_field_get(ADP1050_MSB_MASK, data[1]) |
			   no_os_field_get(ADP1050_LSB_MASK, data[0]);

		/* If CS1 still isn't trimmed right after program exits this
		   whle loop, it means CS1 voltage is either to high or too low
		   for the device to trim it. CS1 voltage needs to be applied
		   according to datasheet specifications. */
		while (read_val != ADP1050_CS1_TRIM_VAL
		       && init_val < ADP1050_CS1_MAX_INIT_VAL) {
			ret = adp1050_write(desc, ADP1050_CS1_GAIN_TRIM, init_val++, 1);
			if (ret)
				return ret;

			ret = adp1050_read(desc, ADP1050_READ_IIN, data, 2);
			if (ret)
				return ret;

			read_val = no_os_field_get(ADP1050_MSB_MASK, data[1]) |
				   no_os_field_get(ADP1050_LSB_MASK, data[0]);
		}

		return 0;
	case ADP1050_VF_VIN_TRIM:
		ret = adp1050_write(desc, ADP1050_VIN_SCALE_MONITOR,
				    no_os_field_get(ADP1050_EXP_MASK, trim_val)
				    | no_os_field_get(ADP1050_MANT_MASK, trim_val), 2);
		if (ret)
			return ret;

		ret = adp1050_read(desc, ADP1050_READ_VIN, data, 2);
		if (ret)
			return ret;

		read_val = no_os_field_get(ADP1050_MSB_MASK, data[1]) |
			   no_os_field_get(ADP1050_LSB_MASK, data[0]);

		/* If VF still isn't trimmed right after program exits this
		   whle loop, it means VF voltage is either to high or too low
		   for the device to trim it. VF voltage needs to be applied
		   according to datasheet specifications. */
		while (read_val != ADP1050_VF_TRIM_VAL && init_val < ADP1050_VF_MAX_INIT_VAL) {
			ret = adp1050_write(desc, ADP1050_VF_GAIN_TRIM, init_val++, 1);
			if (ret)
				return ret;

			ret = adp1050_read(desc, ADP1050_READ_IIN, data, 2);
			if (ret)
				return ret;

			read_val = no_os_field_get(ADP1050_MSB_MASK, data[1]) |
				   no_os_field_get(ADP1050_LSB_MASK, data[0]);
		}

		return 0;
	case ADP1050_VS_VOUT_TRIM:
		ret = adp1050_vout_value(desc, vout_command, vout_command + 1);
		if (ret)
			return ret;

		ret = adp1050_vout_scale(desc, no_os_field_get(ADP1050_EXP_MASK, vout_scale),
					 no_os_field_get(ADP1050_MANT_MASK, vout_scale));
		if (ret)
			return ret;

		ret = adp1050_read_vsense(desc, &read_val);
		if (ret)
			return ret;

		while (read_val != trim_val
		       && no_os_field_get(ADP1050_VS_INIT_MASK,
					  init_val) < ADP1050_VS_OFFSET_MAX_INIT_VAL) {
			ret = adp1050_vout_offset(desc,
						  (read_val < trim_val) ? init_val++ : init_val--);
			if (ret)
				return ret;

			ret = adp1050_read_vsense(desc, &read_val);
			if (ret)
				return ret;
		}

		ret = adp1050_read_vsense(desc, &read_val);
		if (ret)
			return ret;

		init_val = 0;
		while (read_val != trim_val && init_val < ADP1050_VS_MAX_INIT_VAL) {
			ret = adp1050_write(desc, ADP1050_VS_GAIN_TRIM, init_val++, 1);
			if (ret)
				return ret;

			ret = adp1050_read_vsense(desc, &read_val);
			if (ret)
				return ret;
		}

		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Unlock CHIP/EEPROM/TRIM password
 * @param desc - ADP1050 device descriptor
 * @param password - Password value.
 * @param pass_type - Type of password to be unlocked.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_unlock_pass(struct adp1050_desc *desc, uint16_t password,
			enum adp1050_pass_type pass_type)
{
	int ret;
	uint8_t check_reg;

	switch (pass_type) {
	case ADP1050_CHIP_PASS:
		ret = adp1050_write(desc, ADP1050_CHIP_PASSWORD, password, 2);
		if (ret)
			return ret;

		ret = adp1050_write(desc, ADP1050_CHIP_PASSWORD, password, 2);
		if (ret)
			return ret;

		ret = adp1050_read(desc, ADP1050_FLAG_REGISTER1, &check_reg, 1);
		if (ret)
			return ret;

		return no_os_field_get(ADP1050_CHECK_CHIP_PASS_MASK, check_reg) ? 0 : -EINVAL;
	case ADP1050_EEPROM_PASS:
		if (password > ADP1050_EEPROM_DEFAULT_PASS)
			return -EINVAL;

		ret = adp1050_write(desc, ADP1050_EEPROM_PASSWORD, password, 1);
		if (ret)
			return ret;

		ret = adp1050_write(desc, ADP1050_EEPROM_PASSWORD, password, 1);
		if (ret)
			return ret;

		ret = adp1050_read(desc, ADP1050_FLAG_REGISTER3, &check_reg, 1);
		if (ret)
			return ret;

		return no_os_field_get(ADP1050_CHECK_EEPROM_PASS_MASK, check_reg) ? 0 : -EINVAL;
	case ADP1050_TRIM_PASS:
		if (password > ADP1050_EEPROM_DEFAULT_PASS)
			return -EINVAL;

		ret = adp1050_write(desc, ADP1050_TRIM_PASSWORD, password, 1);
		if (ret)
			return ret;

		ret = adp1050_write(desc, ADP1050_TRIM_PASSWORD, password, 1);
		if (ret)
			return ret;

		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Lock CHIP/EEPROM password. TRIM passwor dis locked of the EEPROM
 * 	  password is locked.
 * @param desc - ADP1050 device descriptor
 * @param password - Wrong password to be introduced for password lockage.
 * @param pass_type - Password type to be used for register lockage.
 * @return 0 in case of succes, negative error code otherwise.
*/
int adp1050_lock_pass(struct adp1050_desc *desc, uint16_t password,
		      enum adp1050_pass_type pass_type)
{
	switch (pass_type) {
	case ADP1050_CHIP_PASS:
		return adp1050_write(desc, ADP1050_CHIP_PASSWORD, password, 2);
	case ADP1050_EEPROM_PASS:
		return adp1050_write(desc, ADP1050_EEPROM_PASSWORD,
				     password, 1);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Change the CHIP/EEPROM password. TRIM password is the same with
 * 	  EEPROM password.
 * @param desc - ADP1050 device descriptor
 * @param old_pass - Old password value.
 * @param new_pass - New password value.
 * @param pass_type - Password type (CHIP/EEPROM).
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_change_pass(struct adp1050_desc *desc, uint16_t old_pass,
			uint16_t new_pass, enum adp1050_pass_type pass_type)
{
	int ret;

	if (pass_type == ADP1050_EEPROM_PASS && new_pass > ADP1050_EEPROM_DEFAULT_PASS)
		return -EINVAL;

	switch (pass_type) {
	case ADP1050_CHIP_PASS:
		ret = adp1050_write(desc, ADP1050_CHIP_PASSWORD, old_pass, 2);
		if (ret)
			return ret;

		ret = adp1050_write(desc, ADP1050_CHIP_PASSWORD, new_pass, 2);
		if (ret)
			return ret;

		return adp1050_send_command(desc, ADP1050_STORE_USER_ALL);
	case ADP1050_EEPROM_PASS:
		ret = adp1050_write(desc, ADP1050_EEPROM_PASSWORD, old_pass, 1);
		if (ret)
			return ret;

		return adp1050_write(desc, ADP1050_EEPROM_PASSWORD,
				     new_pass, 1);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Software reset the ADP1050.
 * @param desc - ADP1050 device descriptor
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_software_reset(struct adp1050_desc *desc)
{
	int ret;

	ret = adp1050_write(desc, ADP1050_SOFTWARE_RESET_SETTINGS,
			    ADP1050_SW_RES_DELAY_500MS
			    | ADP1050_SW_RES_NO_DELAY, 1);
	if (ret)
		return ret;

	return adp1050_write(desc, ADP1050_SOFTWARE_RESET_GO,
			     ADP1050_SOFTWARE_RESET_ON, 1);
}

/**
 * @brief Initialize the ADP1050 device.
 * @param desc - ADP1050 device descriptor
 * @param init_param - Initialization parameter containing information about the
 * 		       ADP1050 device to be initialized.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1050_init(struct adp1050_desc **desc,
		 struct adp1050_init_param *init_param)
{
	struct adp1050_desc *descriptor;
	int ret;

	descriptor = (struct adp1050_desc *)no_os_calloc(sizeof(*descriptor), 1);
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_i2c_init(&descriptor->i2c_desc, init_param->i2c_param);
	if (ret)
		goto free_desc;

	ret = no_os_gpio_get_optional(&descriptor->pg_alt_desc,
				      init_param->pg_alt_param);
	if (ret)
		goto free_desc;

	ret = no_os_gpio_direction_input(descriptor->pg_alt_desc);
	if (ret)
		goto free_desc;

	if (init_param->ext_syni) {
		ret = no_os_pwm_init(&descriptor->syni_desc,
				     init_param->syni_param);
		if (ret)
			goto free_desc;

		descriptor->flgi_desc = NULL;
	} else {
		ret = no_os_gpio_get_optional(&descriptor->flgi_desc,
					      init_param->flgi_param);
		if (ret)
			goto free_desc;

		ret = no_os_gpio_direction_output(descriptor->flgi_desc,
						  NO_OS_GPIO_LOW);
		if (ret)
			goto free_desc;

		descriptor->syni_desc = NULL;
	}

	ret = no_os_gpio_get_optional(&descriptor->ctrl_desc,
				      init_param->ctrl_param);
	if (ret)
		goto free_desc;

	if (descriptor->ctrl_desc
	    && !no_os_field_get(ADP1050_CTRL_PIN_ENABLE, init_param->on_off_config))
		goto free_desc;

	ret = adp1050_write(descriptor, ADP1050_ON_OFF_CONFIG,
			    (uint16_t)init_param->on_off_config, 1);
	if (ret)
		goto free_desc;

	ret = adp1050_write(descriptor, ADP1050_OPERATION, ADP1050_OPERATION_ON, 1);
	if (ret)
		goto free_desc;

	/* Time needed for the ADP1050 to power-up. */
	no_os_mdelay(52);

	ret = adp1050_send_command(descriptor, ADP1050_CLEAR_FAULTS);
	if (ret)
		goto free_desc;

	descriptor->loop = ADP1050_CLOSE_LOOP;
	descriptor->freq = ADP1050_49KHZ;

	*desc = descriptor;

	return 0;

free_desc:
	adp1050_remove(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by the adp1050_init()
 * @param desc - ADP1050 device descriptor
 * @return- 0 in case of succes, negative error code otherwise
*/
int adp1050_remove(struct adp1050_desc *desc)
{
	int ret;

	if (!desc)
		return -ENODEV;

	if (desc->i2c_desc) {
		ret = adp1050_write(desc, ADP1050_OPERATION, ADP1050_OPERATION_OFF, 1);
		if (ret)
			return ret;
	}

	no_os_gpio_remove(desc->ctrl_desc);
	no_os_gpio_remove(desc->flgi_desc);
	no_os_pwm_remove(desc->syni_desc);
	no_os_gpio_remove(desc->pg_alt_desc);
	no_os_i2c_remove(desc->i2c_desc);
	no_os_free(desc);

	return 0;
}
