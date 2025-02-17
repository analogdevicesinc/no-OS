/***************************************************************************//**
 *   @file   adp5055.c
 *   @brief  Source file for the ADP5055 Driver
 *   @author Angelo Catapang (angelo.catapang@analog.com)
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
#include <string.h>
#include "adp5055.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"

/**
 * @brief Update specific register bits of ADP5055 through a specified mask
 * @param desc - ADP5055 device descriptor
 * @param address - 8-bit ADP5055 register address
 * @param mask - 8-bit register mask for target bit changes
 * @param val - Data byte value to write
 * @return 0 in case of succes, negative error code otherwise
*/
static int adp5055_update_register(struct adp5055_desc *desc, uint8_t address,
				   uint8_t mask, uint8_t val)
{
	int ret;
	uint8_t data;

	ret = adp5055_read(desc, address, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= no_os_field_prep(mask, val);

	return adp5055_write(desc, address, val);
}

/**
 * @brief Read data from ADP5055
 * @param desc - ADP5055 device descriptor
 * @param address - 8-bit ADP5055 register address
 * @param data - Buffer with received data
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_read(struct adp5055_desc *desc, uint8_t address, uint8_t *data)
{
	int ret;

	if (!desc)
		return -EINVAL;

	ret = no_os_i2c_write(desc->i2c_desc, &address, 1, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(desc->i2c_desc, data, 1, 1);
}

/**
 * @brief Write data to ADP5055
 * @param desc - ADP5055 device descriptor
 * @param address - 8-bit ADP5055 register address
 * @param data - Data byte value to write to the ADP5055
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_write(struct adp5055_desc *desc, uint8_t address, uint8_t data)
{
	uint8_t val[2] = {0, 0};

	if (!desc)
		return -EINVAL;

	val[0] = address;
	val[1] = data;

	return no_os_i2c_write(desc->i2c_desc, val, 2, 1);
}

/**
 * @brief Read statuses
 * @param desc - ADP5055 device descriptor
 * @param status_val - Status value to be returned.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_read_status(struct adp5055_desc *desc, uint8_t *status_val)
{
	return adp5055_read(desc, ADP5055_STATUS_CML, status_val);
}

/**
 * @brief Configures enabling for all individual channels by hardware and/or software
 * @param desc - ADP5055 device descriptor
 * @param mode - Enable mode
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_set_enable_mode(struct adp5055_desc *desc,
			    enum adp5055_en_mode mode)
{
	return adp5055_update_register(desc, ADP5055_CTRL_MODE1, ADP5055_EN_MODE_MASK,
				       mode);
}

/**
 * @brief Enables or disables dynamic voltage scaling across all channels
 * @param desc - ADP5055 device descriptor
 * @param en - If true, output voltage transitions only with a write to the VIDx_GO
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_set_dvs_auto(struct adp5055_desc *desc, bool en)
{
	return adp5055_update_register(desc, ADP5055_CTRL_MODE1, ADP5055_DVS_AUTO,
				       en ? 1 : 0);
}

/**
 * @brief Enables or disables overcurrent protection (OCP) blanking for all channels
 * @param desc - ADP5055 device descriptor
 * @param en - If true, enables OCP blanking
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_set_ocp_blanking(struct adp5055_desc *desc, bool en)
{
	return adp5055_update_register(desc, ADP5055_CTRL_MODE2, ADP5055_OCP_BLANKING,
				       en ? 1 : 0);
}

/**
 * @brief Enables or disables automatic PWM/PSM per channel
 * @param desc - ADP5055 device descriptor
 * @param ch - ADP5055 output channel
 * @param en - If true, enables automatic PWM/PSM on specified channel
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_set_auto_pwm_psm(struct adp5055_desc *desc, enum adp5055_channel ch,
			     bool en)
{
	uint32_t mask;

	if (ch > ADP5055_VOUT3)
		return -EINVAL;

	mask = ADP5055_PSM1_ON << ch;

	return adp5055_update_register(desc, ADP5055_CTRL_MODE2, mask, en ? 1 : 0);
}

/**
 * @brief Enables or disables output discharge per channel
 * @param desc - ADP5055 device descriptor
 * @param ch - ADP5055 output channel
 * @param en - If true, enables output discharge on the specified channel
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_set_output_discharge(struct adp5055_desc *desc,
				 enum adp5055_channel ch, bool en)
{
	uint32_t mask;

	if (ch > ADP5055_VOUT3)
		return -EINVAL;

	mask = ADP5055_DSCG1_ON << ch;

	return adp5055_update_register(desc, ADP5055_CTRL_MODE2, mask, en ? 1 : 0);
}

/**
 * @brief Sets both enable and disable delay per channel
 * @param desc - ADP5055 device descriptor
 * @param ch - ADP5055 output channel
 * @param en_dly - Enable delay on the specified channel
 * @param dis_dly - Disable delay on the specified channel
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_set_enable_disable_delay(struct adp5055_desc *desc,
				     enum adp5055_channel ch, enum adp5055_en_dly en_dly,
				     enum adp5055_dis_dly dis_dly)
{
	int ret;
	uint8_t address, data;
	uint32_t mask;

	if (ch > ADP5055_VOUT3)
		return -EINVAL;

	address = ADP5055_DLY1 + ch;
	mask = ADP5055_EN_DLY_MASK | ADP5055_DIS_DLY_MASK;

	ret = adp5055_read(desc, address, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= no_os_field_prep(ADP5055_EN_DLY_MASK, en_dly);
	data |= no_os_field_prep(ADP5055_DIS_DLY_MASK, dis_dly);

	return adp5055_write(desc, address, data);
}

/**
 * @brief Sets 8-bit code of output voltage reference of a channel
 * @param desc - ADP5055 device descriptor
 * @param ch - ADP5055 output channel
 * @param val - 8-bit code of output voltage reference (Vref = 408mV + (1.5mV * val))
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_set_vid_code(struct adp5055_desc *desc, enum adp5055_channel ch,
			 uint8_t val)
{
	uint8_t address;

	if (ch > ADP5055_VOUT3)
		return -EINVAL;

	address = ADP5055_VID1 + ch;

	return adp5055_write(desc, address, val);
}

/**
 * @brief Initiates output voltage transition on specified channel when dynamic voltage scaling is enabled
 * @param desc - ADP5055 device descriptor
 * @param ch - ADP5055 output channel
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_start_vout(struct adp5055_desc *desc, enum adp5055_channel ch)
{
	uint32_t mask;

	if (ch > ADP5055_VOUT3)
		return -EINVAL;

	mask = ADP5055_VID1_GO << ch;

	return adp5055_update_register(desc, ADP5055_VID_GO, mask, 1);
}

/**
 * @brief Enables output voltage channel in software
 * @param desc - ADP5055 device descriptor
 * @param ch - ADP5055 output channel
 * @param en - Channel enable or disable
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_enable_channel(struct adp5055_desc *desc, enum adp5055_channel ch,
			   bool en)
{
	uint32_t mask;

	if (ch > ADP5055_VOUT3)
		return -EINVAL;

	mask = ADP5055_CH1_ON << ch;

	return adp5055_update_register(desc, ADP5055_CTRL123, mask, en ? 1 : 0);
}

/**
 * @brief Sets dynamic voltage scaling interval for each channel
 * @param desc - ADP5055 device descriptor
 * @param ch - ADP5055 output channel
 * @param intval - Dynamic voltage scaling interval setting
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_set_dvs_interval(struct adp5055_desc *desc, enum adp5055_channel ch,
			     enum adp5055_dvs_intval intval)
{
	uint32_t mask;

	if (ch > ADP5055_VOUT3)
		return -EINVAL;

	mask = ADP5055_DVS_INTVAL1 << (ch * 2);

	return adp5055_update_register(desc, ADP5055_DVS_CFG, mask, intval);
}

/**
 * @brief Sets high or low limit for output voltage reference
 * @param desc - ADP5055 device descriptor
 * @param ch - ADP5055 output channel
 * @param lim_type - Selects whether high or low limit is set
 * @param lim_val - 4-bit code of high / low output voltage reference limit
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_set_vidx_lim(struct adp5055_desc *desc, enum adp5055_channel ch,
			 enum adp5055_vidx_limit lim_type, uint8_t lim_val)
{
	uint8_t address;
	uint32_t mask;

	if (ch > ADP5055_VOUT3)
		return -EINVAL;

	address = ADP5055_DVS_LIM1 + ch;

	switch (lim_type) {
	case ADP5055_VIDX_LOW:
		mask = ADP5055_VIDX_LOW_MASK;
		break;
	case ADP5055_VIDX_HIGH:
		mask = ADP5055_VIDX_HIGH_MASK;
		break;
	default:
		return -EINVAL;
	}

	return adp5055_update_register(desc, address, mask, lim_val);
}

/**
 * @brief Configures fast transient sensitivity per channel
 * @param desc - ADP5055 device descriptor
 * @param ch - ADP5055 output channel
 * @param ft - Fast transient setting
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_set_fast_transient(struct adp5055_desc *desc,
			       enum adp5055_channel ch, enum adp5055_ft_th ft)
{
	uint32_t mask;

	if (ch > ADP5055_VOUT3)
		return -EINVAL;

	mask = ADP5055_FT1_TH << (ch * 2);

	return adp5055_update_register(desc, ADP5055_FT_CFG, mask, ft);
}

/**
 * @brief Enables power good signal to external PWRGD hardware pin
 * @param desc - ADP5055 device descriptor
 * @param ch - ADP5055 output channel
 * @param en - If true, power good signal of specified channel is outputted at external PWRGD hardware pin
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_set_pg_channel_output(struct adp5055_desc *desc,
				  enum adp5055_channel ch, bool en)
{
	uint32_t mask;

	if (ch > ADP5055_VOUT3)
		return -EINVAL;

	mask = ADP5055_PG1_MASK << ch;

	return adp5055_update_register(desc, ADP5055_PG_CFG, mask, en ? 0 : 1);
}

/**
 * @brief Enables a delay in the PWRGD hardware pin
 * @param desc - ADP5055 device descriptor
 * @param en - If false, PWRGD pin responds immediately
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_set_pg_hw_delay(struct adp5055_desc *desc, bool en)
{
	return adp5055_update_register(desc, ADP5055_PG_CFG, ADP5055_PWRGD_DLY,
				       en ? 1 : 0);
}

/**
 * @brief Reads real-time power good status of specified channel
 * @param desc - ADP5055 device descriptor
 * @param ch - ADP5055 output channel
 * @param pg_val - Stores the value of the real-time power good status
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_read_pg(struct adp5055_desc *desc, enum adp5055_channel ch,
		    uint8_t *pg_val)
{
	int ret;
	uint8_t data;
	uint32_t mask;

	if (ch > ADP5055_VOUT3)
		return -EINVAL;

	mask = ADP5055_PWRGD1 << ch;

	ret = adp5055_read(desc, ADP5055_PG_READ, &data);
	if (ret)
		return ret;

	*pg_val = no_os_field_get(mask, data);

	return 0;
}

/**
 * @brief Reads the latched status of a specified channel
 * @param desc - ADP5055 device descriptor
 * @param ch - ADP5055 output channel
 * @param lch_status_type - Selects which latch status to read
 * @param lch_status_val - Stores the latch status value of the specified channel
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_read_latched_status(struct adp5055_desc *desc,
				enum adp5055_channel ch, enum adp5055_latch_status lch_status_type,
				uint8_t *lch_status_val)
{
	int ret;
	uint8_t data;
	uint32_t mask;

	if (ch > ADP5055_VOUT3)
		return -EINVAL;

	switch (lch_status_type) {
	case ADP5055_LCH_INIT_FAIL:
		mask = ADP5055_INT_LCH;
		break;
	case ADP5055_LCH_OVERCURRENT_HICCUP:
		mask = ADP5055_OCP1_LCH << ch;
		break;
	case ADP5055_LCH_THERMAL_SHUTDOWN:
		mask = ADP5055_TSD_LCH;
		break;
	case ADP5055_LCH_PG_FAIL:
		mask = ADP5055_PG1_LCH << ch;
		break;
	default:
		return -EINVAL;
	}

	ret = adp5055_read(desc, ADP5055_STATUS_LCH, &data);
	if (ret)
		return ret;

	*lch_status_val = no_os_field_get(mask, data);

	return 0;
}

/**
 * @brief Initialize the ADP5055 device.
 * @param desc - ADP5055 device descriptor
 * @param init_param - Initialization parameter containing information about the
 * 		       ADP5055 device to be initialized.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp5055_init(struct adp5055_desc **desc,
		 struct adp5055_init_param *init_param)
{
	struct adp5055_desc *descriptor;
	int ret;

	descriptor = (struct adp5055_desc *)no_os_calloc(sizeof(*descriptor), 1);
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_i2c_init(&descriptor->i2c_desc, init_param->i2c_param);
	if (ret)
		goto free_desc;

	*desc = descriptor;

	return 0;

free_desc:
	adp5055_remove(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by the adp5055_init()
 * @param desc - ADP5055 device descriptor
 * @return- 0 in case of succes, negative error code otherwise
*/
int adp5055_remove(struct adp5055_desc *desc)
{
	int ret;

	if (!desc)
		return -ENODEV;

	if (desc->i2c_desc) {
		ret = adp5055_set_enable_mode(desc, ADP5055_EN_MODE_HW_AND_SW);
		if (ret)
			return ret;

		ret = adp5055_write(desc, ADP5055_CTRL123, 0);
		if (ret)
			return ret;
	}

	no_os_i2c_remove(desc->i2c_desc);
	no_os_free(desc);

	return 0;
}
