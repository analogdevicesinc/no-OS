/***************************************************************************//**
 *   @file   adp1055.c
 *   @brief  Source file for the ADP1055 Driver
 *   @author Ivangil Mercano (Ivangil.mercano@analog.com)
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
#include "adp1055.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/**
 * @brief Send command byte/word to ADP1055
 * @param desc - ADP1055 device descriptor
 * @param command - Value of the command
 * @return 0 in case of sucess, negative error code otherwise
 */
int adp1055_send_command(struct adp1055_desc *desc, uint16_t command)
{
	uint8_t data[2];
	uint8_t command_val;

	if (!desc)
		return -EINVAL;

	if (command > ADP1055_EXTENDED_COMMAND) {
		data[0] = no_os_field_get(ADP1055_LSB_MASK, command);
		data[1] = no_os_field_get(ADP1055_MSB_MASK, command);

		return no_os_i2c_write(desc->i2c_desc, data, 2, 1);
	}

	command_val = no_os_field_get(ADP1055_LSB_MASK, command);

	return no_os_i2c_write(desc->i2c_desc, &command_val, 1, 1);
}

/**
 * @brief - Write data to ADP1055
 * @param desc - ADP1055 device structure
 * @param command - Command value
 * @param data - Data value to write in ADP1055
 * @param byte_num - Number of bytes to write.
 * @return 0 in case of success, negative error code otherwise
 */
int adp1055_write(struct adp1055_desc *desc, uint16_t command, uint16_t data,
		  uint8_t byte_num)
{
	uint8_t val[4];

	if (!desc)
		return -EINVAL;

	if (command > ADP1055_EXTENDED_COMMAND) {
		val[0] = no_os_field_get(ADP1055_MSB_MASK, command);
		val[1] = no_os_field_get(ADP1055_LSB_MASK, command);
		if (byte_num > 1) {
			val[2] = no_os_field_get(ADP1055_LSB_MASK, data);
			val[3] = no_os_field_get(ADP1055_MSB_MASK, data);
		} else
			val[2] = no_os_field_get(ADP1055_LSB_MASK, data);

		return no_os_i2c_write(desc->i2c_desc, val, byte_num + 2, 1);
	} else {
		val[0] = no_os_field_get(ADP1055_LSB_MASK, command);
		if (byte_num > 1) {
			val[1] = no_os_field_get(ADP1055_LSB_MASK, data);
			val[2] = no_os_field_get(ADP1055_MSB_MASK, data);
		} else
			val[1] = no_os_field_get(ADP1055_LSB_MASK, data);

		return no_os_i2c_write(desc->i2c_desc, val, byte_num + 1, 1);
	}
}

/**
 * @brief Read data from ADP1055
 * @param desc - ADP1055 device descriptor
 * @param command - Command value
 * @param data - Buffer with received data
 * @param byte_num - Number of bytes of received data
 * @return 0 if success, negative error otherwise
 */
int adp1055_read(struct adp1055_desc *desc, uint16_t command, uint8_t *data,
		 uint8_t byte_num)
{
	int ret;
	uint8_t command_val[2];
	uint8_t write_bytes;

	if (!desc)
		return -EINVAL;

	if (command > ADP1055_EXTENDED_COMMAND) {
		command_val[1] = no_os_field_get(ADP1055_LSB_MASK, command);
		command_val[0] = no_os_field_get(ADP1055_MSB_MASK, command);
		write_bytes = 2;
	} else {
		command_val[0] = no_os_field_get(ADP1055_LSB_MASK, command);
		write_bytes = 1;
	}
	ret = no_os_i2c_write(desc->i2c_desc, command_val, write_bytes, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(desc->i2c_desc, data, byte_num, 1);
}

/**
 * @brief Initialize the ADP1055 device.
 * @param desc - ADP1055 device descriptor
 * @param init_param - Initialization parameter containing information about the
 * 		       ADP1055 device to be initialized.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1055_init(struct adp1055_desc **desc,
		 struct adp1055_init_param *init_param)
{
	struct adp1055_desc *descriptor;
	int ret;
	uint8_t val;

	descriptor = (struct adp1055_desc *)no_os_calloc(sizeof(*descriptor), 1);
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
	    && !no_os_field_get(ADP1055_CTRL_PIN_ENABLE, init_param->on_off_config))
		goto free_desc;

	ret = adp1055_write(descriptor, ADP1055_ON_OFF_CONFIG,
			    (uint16_t)init_param->on_off_config, 1);
	if (ret)
		goto free_desc;

	val = no_os_field_get(ADP1055_ON_OFF_MASK, ADP1055_OPERATION_ON);

	ret = adp1055_write(descriptor, ADP1055_OPERATION, val, 1);
	if (ret)
		goto free_desc;

	/* Time needed for the adp1055 to power-up. */
	no_os_mdelay(52);

	ret = adp1055_send_command(descriptor, ADP1055_CLEAR_FAULTS);
	if (ret)
		goto free_desc;

	descriptor->freq_exp = no_os_field_prep(ADP1055_EXP_MASK,
						ADP1055_DEFAULT_FREQ_EXP);
	descriptor->freq_mant = no_os_field_prep(ADP1055_MANT_MASK,
				ADP1055_DEFAULT_FREQ_MANT);

	*desc = descriptor;

	return 0;

free_desc:
	adp1055_remove(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by the adp1055_init()
 * @param desc - ADP1055 device descriptor
 * @return 0 in case of success, negative error otherwise
 */
int adp1055_remove(struct adp1055_desc *desc)
{
	int ret;
	uint8_t val;

	if (!desc)
		return -ENODEV;

	if (desc->i2c_desc) {
		val = no_os_field_get(ADP1055_ON_OFF_MASK, ADP1055_OPERATION_ON);
		ret = adp1055_write(desc, ADP1055_OPERATION, val, 1);
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

/**
 * @brief Send statuses
 * @param desc - ADP1055 device descriptor
 * @param status - Status type
 * @param status_val - Status value to be returned
 * @return 0 in case of success, negative error otherwise
 */
int adp1055_read_status(struct adp1055_desc *desc,
			enum adp1055_status_type status,
			uint16_t *status_val)
{
	uint8_t read_byte;
	uint8_t data[2];
	int ret;

	switch (status) {
	case ADP1055_STATUS_VOUT_TYPE:
	case ADP1055_STATUS_IOUT_TYPE:
	case ADP1055_STATUS_INPUT_TYPE:
	case ADP1055_STATUS_TEMP_TYPE:
	case ADP1055_STATUS_CML_TYPE:
		ret = adp1055_read(desc, (uint16_t)status, &read_byte, 1);
		if (ret)
			return ret;

		*status_val = read_byte;

		return 0;
	case ADP1055_STATUS_WORD_TYPE:
		ret = adp1055_read(desc, (uint16_t)status, data, 2);
		if (ret)
			return ret;

		*status_val = no_os_get_unaligned_le16(data);

		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read VIN/IIN/VOUT/TEMP2/TEMP3/DUTY_CYCLE_FREQ/POUT raw value from the ADP1055
 * @param desc - AP1055 device descriptor
 * @param value - value type
 * @param mant - Raw mantissa value to be received
 * @param exp - Raw exponent value to be received
 * @return 0 in case of success, negative error otherwise
 */
int adp1055_read_value(struct adp1055_desc *desc, enum adp1055_value_type value,
		       uint16_t *mant, uint8_t *exp)
{
	int ret;
	uint8_t data[2];

	ret = adp1055_read(desc, value, data, 2);
	if (ret)
		return ret;

	*mant = no_os_field_get(ADP1055_MANT_MASK,
				no_os_get_unaligned_le16(data));
	*exp = no_os_field_get(ADP1055_EXP_MASK,
			       no_os_get_unaligned_le16(data));

	return 0;
}

/**
 * @brief Read Voltage Sense output raw value from the ADP1055
 * @param desc - ADP1055 device descriptor
 * @param vsense - VS(VOUT) raw value to be received of the parallel circuit.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1055_read_vsense(struct adp1055_desc *desc, uint16_t *vsense)
{
	int ret;
	uint8_t data[2];

	ret = adp1055_read(desc, ADP1055_READ_VOUT, data, 2);
	if (ret)
		return ret;

	*vsense = no_os_get_unaligned_le16(data);

	return 0;
}

/**
 * @brief Set VOUT_COMMAND and VOUT_MAX values
 * @param desc - ADP1055 descriptor
 * @param vout_command - VOUT_COMMAND value
 * @param vout_max - VOUT_MAX value
 * @return 0 in case of success, negative error code otherwise
*/
int adp1055_vout_value(struct adp1055_desc *desc, uint16_t vout_command,
		       uint16_t vout_max)
{
	int ret;

	ret = adp1055_write(desc, ADP1055_VOUT_COMMAND, vout_command, 2);
	if (ret)
		return ret;

	ret = adp1055_write(desc, ADP1055_GO_CMD, ADP1055_VOLT_REF_GO, 1);
	if (ret)
		return ret;

	return adp1055_write(desc, ADP1055_VOUT_MAX, vout_max, 2);
}

/**
 * @brief Set output voltage offset
 * @param desc - ADP1055 device descriptor
 * @param vout_offset - VOUT offset value.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1055_vout_offset(struct adp1055_desc *desc, int16_t vout_offset)
{
	return adp1055_write(desc, ADP1055_VOUT_CAL_OFFSET, vout_offset, 2);
}

/**
 * @brief Set ADP1055 VOUT transition rate
 * @param desc - ADP1055 device descriptor
 * @param exp - Exponent - 5-bit twos complement
 * @param mant - Mantissa - 11-bit twos complement
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1055_vout_tr(struct adp1055_desc *desc, uint8_t exp, uint16_t mant)
{
	uint16_t val;

	val = no_os_field_prep(ADP1055_EXP_MASK, exp) | mant;

	return adp1055_write(desc, ADP1055_VOUT_TRANSITION_RATE, val, 2);
}

/**
 * @brief Set ADP1055 Vout Droop
 * @param desc - ADP1055 device descriptor
 * @param exp - Exponent 5-bit two's compliment
 * @param mant - Mantissa 11-bit two's compliment
 * @return 0 in case of success, negative error code otherwise
 */
int adp1055_vout_droop(struct adp1055_desc *desc, uint8_t exp, uint16_t mant)
{
	uint16_t val;

	val = no_os_field_prep(ADP1055_EXP_MASK, exp) | mant;

	return adp1055_write(desc, ADP1055_VOUT_DROOP, val, 2);
}

/**
 * @brief Set ADP1055 vout scale loop
 * @param desc - ADP1055 device descriptor
 * @param exp - Exponent 5-bit two's compliment
 * @param mant - Mantissa 11-bit two's compliment
 * @return 0 in case of success, negative error code otherwise
 */
int adp1055_vout_scale_loop(struct adp1055_desc *desc, uint8_t exp,
			    uint16_t mant)
{
	uint16_t val;

	val = no_os_field_prep(ADP1055_EXP_MASK, exp) | mant;

	return adp1055_write(desc, ADP1055_VOUT_SCALE_LOOP, val, 2);
}

/**
 * @brief Set ADP1055 VOUT_SCALE_MONITOR
 * @param desc - ADP1055 device descriptor
 * @param exp - Exponent - 5-bit twos complement
 * @param mant - Mantissa - 11-bit twos complement
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1055_vout_scale(struct adp1055_desc *desc, uint8_t exp, uint16_t mant)
{
	uint16_t val;

	val = no_os_field_prep(ADP1055_EXP_MASK, exp) | mant;

	return adp1055_write(desc, ADP1055_VOUT_SCALE_MONITOR, val, 2);
}

/**
 * @brief Set ADP1055 VIN on/off raw value for input voltage limiting.
 * @param desc - ADP1055 device descriptor
 * @param mant - Mantissa value to be transmitted, 11 bit twos complement.
 * @param exp - Exponent value to be transmitted, 5 bit twos complement.
 * @param state_on - true - Set VIN_ON.
 * 		     false - Set VIN_OFF.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1055_set_vin(struct adp1055_desc *desc, int16_t mant, int8_t exp,
		    bool state_on)
{
	uint16_t val;

	val = no_os_field_prep(ADP1055_EXP_MASK, exp) | mant;

	return adp1055_write(desc, state_on ? ADP1055_VIN_ON : ADP1055_VIN_OFF,
			     val, 2);
}

/**
 * @brief Set ADP1055 IOUT calibration gain
 * @param desc - ADP1055 deveice descriptor
 * @param mant - Mantissa value to be transmitted, 11-bit twos compliment
 * @param exp - Exponent value to be transmitted, 5-bit twos compliment
 * @return 0 in case of success, negative error otherwise
 */
int adp1055_iout_cal_gain(struct adp1055_desc *desc, int16_t mant, int8_t exp)
{
	int16_t val;

	val = no_os_field_prep(ADP1055_EXP_MASK, exp) | mant;

	return adp1055_write(desc, ADP1055_IOUT_CAL_GAIN, val, 2);
}

/**
 * @brief Set ADP1055 IOUT calibration offset
 * @param desc - ADP1055 deveice descriptor
 * @param mant - Mantissa value to be transmitted, 11-bit twos compliment
 * @param exp - Exponent value to be transmitted, 5-bit twos compliment
 * @return 0 in case of success, negative error otherwise
 */
int adp1055_iout_cal_offset(struct adp1055_desc *desc, int16_t mant, int8_t exp)
{
	int16_t val;

	val = no_os_field_prep(ADP1055_EXP_MASK, exp) | mant;

	return adp1055_write(desc, ADP1055_IOUT_CAL_OFFSET, val, 2);
}

/**
 * @brief Set ADP1055 Normal Mode Digital Filter
 * @param desc - ADP1055 device descriptor
 * @param zero - light mode zero settings.
 * @param pole - light mode pole settings.
 * @param lf - light mode low filter gain settings.
 * @param hf - light mode high filter gain settings.
 * @return 0 in case of success, negative error otherwise
 */
int adp1055_normal_mode_df(struct adp1055_desc *desc, uint8_t zero,
			   uint8_t pole,
			   uint8_t lf, uint8_t hf)
{
	int ret;

	ret = adp1055_write(desc, ADP1055_NM_DIGFILT_LF_GAIN_SETTING, lf, 1);
	if (ret)
		return ret;

	ret = adp1055_write(desc, ADP1055_NM_DIGFILT_ZERO_SETTING, zero, 1);
	if (ret)
		return ret;

	ret = adp1055_write(desc, ADP1055_NM_DIGFILT_POLE_SETTING, pole, 1);
	if (ret)
		return ret;

	ret =  adp1055_write(desc, ADP1055_NM_DIGFILT_POLE_SETTING, hf, 1);
	if (ret)
		return ret;

	return adp1055_write(desc, ADP1055_GO_CMD, ADP1055_FILTER_GO, 1);

}

/**
 * @brief Set ADP1055 Light Load Mode Digital Filter
 * @param desc - ADP1055 device descriptor
 * @param zero - light mode zero settings.
 * @param pole - light mode pole settings.
 * @param lf - light mode low filter gain settings.
 * @param hf - light mode high filter gain settings.
 * @return 0 in case of success, negative error otherwise
 */
int adp1055_lightload_mode_df(struct adp1055_desc *desc, uint8_t zero,
			      uint8_t pole,
			      uint8_t lf, uint8_t hf)
{
	int ret;

	ret = adp1055_write(desc, ADP1055_LLM_DIGFILT_LF_GAIN_SETTING, lf, 1);
	if (ret)
		return ret;

	ret = adp1055_write(desc, ADP1055_LLM_DIGFILT_ZERO_SETTING, zero, 1);
	if (ret)
		return ret;

	ret = adp1055_write(desc, ADP1055_LLM_DIGFILT_POLE_SETTING, pole, 1);
	if (ret)
		return ret;

	ret =  adp1055_write(desc, ADP1055_LLM_DIGFILT_POLE_SETTING, hf, 1);
	if (ret)
		return ret;

	return adp1055_write(desc, ADP1055_GO_CMD, ADP1055_FILTER_GO, 1);
}

/**
 * @brief Set ADP1055 Single Shot Digital Filter
 * @param desc - ADP1055 device descriptor
 * @param zero - light mode zero settings.
 * @param pole - light mode pole settings.
 * @param lf - light mode low filter gain settings.
 * @param hf - light mode high filter gain settings.
 * @return 0 in case of success, negative error otherwise
 */
int adp1055_singleshot_mode_df(struct adp1055_desc *desc, uint8_t zero,
			       uint8_t pole,
			       uint8_t lf, uint8_t hf)
{
	int ret;

	ret = adp1055_write(desc, ADP1055_SS_DIGFILT_LF_GAIN_SETTING, lf, 1);
	if (ret)
		return ret;

	ret = adp1055_write(desc, ADP1055_SS_DIGFILT_ZERO_SETTING, zero, 1);
	if (ret)
		return ret;

	ret = adp1055_write(desc, ADP1055_SS_DIGFILT_POLE_SETTING, pole, 1);
	if (ret)
		return ret;

	ret =  adp1055_write(desc, ADP1055_SS_DIGFILT_POLE_SETTING, hf, 1);
	if (ret)
		return ret;

	return adp1055_write(desc, ADP1055_GO_CMD, ADP1055_FILTER_GO, 1);
}

/**
 * @brief PWM modulation configuration for the ADP1055
 * @param desc - ADP1055 device descriptor
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
 * @param mod_en - boolian for modulation enable
 * @param mod_sign - bolian for modulation sign
 * @param chan - requested channel
 * @return 0 in case of success, negative error otherwise
 */
int adp1055_pwm_config(struct adp1055_desc * desc, uint16_t pulse_width,
		       uint16_t pulse_start, bool mod_en, bool mod_sign, enum adp1055_channel chan)
{
	int ret;
	uint8_t redge, fedge;
	uint16_t reg_redge, reg_fedge;
	uint8_t pwm_disable = 0x3f;

	ret = adp1055_write(desc, ADP1055_PWM_DISABLE_SETTING, pwm_disable, 1);
	if (ret)
		return ret;

	fedge = no_os_field_get(ADP1055_PWM_TIMING_MASK, pulse_width + pulse_start);
	redge = no_os_field_get(ADP1055_PWM_TIMING_MASK, pulse_start);

	switch (chan) {
	case ADP1055_OUTA:
		reg_redge = ADP1055_OUTA_REDGE_SETTING;
		reg_fedge = ADP1055_OUTA_FEDGE_SETTING;
		break;
	case ADP1055_OUTB:
		reg_redge = ADP1055_OUTB_REDGE_SETTING;
		reg_fedge = ADP1055_OUTB_FEDGE_SETTING;
		break;
	case ADP1055_OUTC:
		reg_redge = ADP1055_OUTC_REDGE_SETTING;
		reg_fedge = ADP1055_OUTC_FEDGE_SETTING;
		break;
	case ADP1055_OUTD:
		reg_redge = ADP1055_OUTD_REDGE_SETTING;
		reg_fedge = ADP1055_OUTD_FEDGE_SETTING;
		break;
	case ADP1055_SR1:
		reg_redge = ADP1055_SR1_REDGE_SETTING;
		reg_fedge = ADP1055_SR1_FEDGE_SETTING;
		break;
	case ADP1055_SR2:
		reg_redge = ADP1055_SR2_REDGE_SETTING;
		reg_fedge = ADP1055_SR2_FEDGE_SETTING;
		break;

	default:
		return -EINVAL;
	}

	fedge = fedge | no_os_field_get(ADP1055_PWM_MODEN_MASK, (uint8_t)mod_en) |
		no_os_field_get(ADP1055_PWM_SIGN_MASK, (uint8_t)mod_sign);

	redge = redge | no_os_field_get(ADP1055_PWM_MODEN_MASK, (uint8_t)mod_en) |
		no_os_field_get(ADP1055_PWM_SIGN_MASK, (uint8_t)mod_sign);

	ret = adp1055_write(desc, reg_fedge, fedge, 2);
	if (ret)
		return ret;

	ret = adp1055_write(desc, reg_redge, redge, 2);
	if (ret)
		return ret;

	return adp1055_write(desc, ADP1055_GO_CMD, ADP1055_PWM_GO, 1);
}

/**
 * @brief Set PWM channel and frequency
 * @param desc - ADP1055 device descriptor
 * @param chan - Selected channel
 * @param exp - 5-bit twos compliment (-4 to 0)
 * @param mant - 11-bit twos compliment
 * @return 0 in case of success, negative error otherwise
 */
int adp1055_set_pwm(struct adp1055_desc * desc, enum adp1055_channel chan,
		    int8_t exp, uint16_t mant)
{
	int ret;
	uint8_t reg_val;
	uint16_t freq;

	freq = no_os_field_prep(ADP1055_EXP_MASK, exp) | mant;

	ret = adp1055_write(desc, ADP1055_FREQUENCY_SWITCH, freq, 2);
	if (ret)
		return ret;

	desc->freq_mant = no_os_field_prep(ADP1055_MANT_MASK, mant);
	desc->freq_exp = no_os_field_prep(ADP1055_EXP_MASK, exp);

	ret = adp1055_write(desc, ADP1055_GO_CMD, ADP1055_FREQ_GO, 1);
	if (ret)
		return ret;

	switch (chan) {
	case ADP1055_OUTA:
		reg_val = ADP1055_OUTA_ON;
		break;
	case ADP1055_OUTB:
		reg_val = ADP1055_OUTA_ON;
		break;
	case ADP1055_OUTC:
		reg_val = ADP1055_OUTA_ON;
		break;
	case ADP1055_OUTD:
		reg_val = ADP1055_OUTA_ON;
		break;
	case ADP1055_SR1:
		reg_val = ADP1055_OUTA_ON;
		break;
	case ADP1055_SR2:
		reg_val = ADP1055_OUTA_ON;
		break;
	case ADP1055_DISABLE_ALL:
		reg_val = ADP1055_PWM_OFF;
		break;
	default:
		return -EINVAL;
	}

	return adp1055_write(desc, ADP1055_PWM_DISABLE_SETTING, reg_val, 1);
}

/**
 * @brief Frequency synchronization of the PWM clock with an external clock
 * 	  Requires syni_desc to be initialized.
 * @param desc - ADP1055 device descriptor
 * @param pll - pll disable and enable
 * @param reso - 5ns resolution enable
 * @return 0 in case of success, negative error otherwise
 */
int adp1055_freq_sync(struct adp1055_desc * desc, bool pll, bool reso)
{
	uint8_t val;
	int ret;

	val = no_os_field_prep(ADP1055_FREQ_PLL_MASK, (uint8_t)pll) |
	      no_os_field_prep(ADP1055_FREQ_JITT_MASK, ADP1055_FREQ_JITT_VAL) |
	      no_os_field_prep(ADP1055_FREQ_RESO_MASK, (uint8_t)reso);

	ret = adp1055_write(desc, ADP1055_SYNC, val, 1);
	if (ret)
		return -EINVAL;

	return adp1055_write(desc,  ADP1055_GO_CMD, ADP1055_SYNC_GO, 1);
}
