/***************************************************************************//**
 *   @file   lt8722.c
 *   @brief  Source file for the LT8722 Driver
 *   @author Ramon Cristopher Calam (ramoncristopher.calam@analog.com)
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
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_crc8.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "lt8722.h"

NO_OS_DECLARE_CRC8_TABLE(lt8722_crc8);

struct lt8722_reg lt8722_regs[LT8722_NUM_REGISTERS] = {
	{
		LT8722_SPIS_COMMAND, LT8722_SPIS_COMMAND_DEFAULT_VALUE,
		LT8722_SPIS_COMMAND_SIZE
	},
	{
		LT8722_SPIS_STATUS, LT8722_SPIS_STATUS_DEFAULT_VALUE,
		LT8722_SPIS_STATUS_SIZE
	},
	{
		LT8722_SPIS_DAC_ILIMN, LT8722_SPIS_DAC_ILIMN_DEFAULT_VALUE,
		LT8722_SPIS_DAC_ILIMN_SIZE
	},
	{
		LT8722_SPIS_DAC_ILIMP, LT8722_SPIS_DAC_ILIMP_DEFAULT_VALUE,
		LT8722_SPIS_DAC_ILIMP_SIZE
	},
	{
		LT8722_SPIS_DAC, LT8722_SPIS_DAC_DEFAULT_VALUE,
		LT8722_SPIS_DAC_SIZE
	},
	{
		LT8722_SPIS_OV_CLAMP, LT8722_SPIS_OV_CLAMP_DEFAULT_VALUE,
		LT8722_SPIS_OV_CLAMP_SIZE
	},
	{
		LT8722_SPIS_UV_CLAMP, LT8722_SPIS_UV_CLAMP_DEFAULT_VALUE,
		LT8722_SPIS_UV_CLAMP_SIZE
	},
	{
		LT8722_SPIS_AMUX, LT8722_SPIS_AMUX_DEFAULT_VALUE,
		LT8722_SPIS_AMUX_SIZE
	},
};

/**
 * @brief Convert voltage to DAC code.
 * @param voltage - Voltage value in nanovolts.
 * @return DAC code.
 */
int32_t lt8722_voltage_to_dac(int64_t voltage)
{
	return (LT8722_DAC_OFFSET - voltage) * (1 << LT8722_DAC_RESOLUTION) /
	       LT8722_DAC_VREF;
}

/**
 * @brief Convert DAC code to nanovolts.
 * @param dac - DAC code.
 * @return Voltage value in nanovolts.
 */
int64_t lt8722_dac_to_voltage(int32_t dac)
{
	return LT8722_DAC_OFFSET - dac * LT8722_DAC_VREF /
	       (1 << LT8722_DAC_RESOLUTION);
}

/**
 * @brief LT8722 device SPI transaction.
 * @param dev - LT8722 device descriptor
 * @param packet - LT8722 packet.
 * @return 0 in case of succes, negative error code otherwise
*/
int lt8722_transaction(struct lt8722_dev *dev, struct lt8722_packet *packet)
{
	int ret;
	uint8_t buffer[8] = {0};

	if (!dev || !packet)
		return -EINVAL;

	buffer[0] = packet->command.byte;
	buffer[1] = packet->reg.address << 1;

	if (packet->command.byte == LT8722_DATA_WRITE_COMMAND) {
		no_os_put_unaligned_be32(packet->data, &buffer[2]);
		buffer[6] = no_os_crc8(lt8722_crc8, buffer, 6, 0);
	} else
		buffer[2] = no_os_crc8(lt8722_crc8, buffer, 2, 0);

	ret = no_os_spi_write_and_read(dev->spi, buffer, packet->command.size);
	if (ret)
		return ret;

	packet->status = (no_os_get_unaligned_be16(&buffer[0]) &
			  NO_OS_GENMASK(10, 0));

	if (packet->command.byte == LT8722_DATA_WRITE_COMMAND) {
		packet->crc = buffer[2];
		packet->ack = buffer[7];
	} else if (packet->command.byte == LT8722_DATA_READ_COMMAND) {
		packet->data = (no_os_get_unaligned_be32(&buffer[2]) &
				NO_OS_GENMASK(packet->reg.size - 1, 0));
		packet->crc = buffer[6];
		packet->ack = buffer[7];
	} else {
		packet->crc = buffer[2];
		packet->ack = buffer[3];
	}

	dev->status = packet->status;

	if (packet->ack != LT8722_ACK_ACKNOWLEDGE)
		return -EBADMSG;

	return 0;
}

/**
 * @brief Read data from LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param address - Register address.
 * @param data - Received data.
 * @return 0 in case of succes, negative error code otherwise
*/
int lt8722_reg_read(struct lt8722_dev *dev, uint8_t address, uint32_t *data)
{
	int ret;
	struct lt8722_packet packet;
	struct lt8722_command command = {
		LT8722_DATA_READ_COMMAND,
		LT8722_DATA_READ_COMMAND_SIZE
	};

	if (!dev || !data)
		return -EINVAL;

	packet.command = command;
	packet.reg = lt8722_regs[address];

	ret = lt8722_transaction(dev, &packet);
	if (ret)
		return ret;

	*data = packet.data;

	return 0;
}

/**
 * @brief Write data to LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param address - Register address.
 * @param data - Data to be written.
 * @return 0 in case of succes, negative error code otherwise
*/
int lt8722_reg_write(struct lt8722_dev *dev, uint8_t address, uint32_t data)
{
	struct lt8722_packet packet;
	struct lt8722_command command = {
		LT8722_DATA_WRITE_COMMAND,
		LT8722_DATA_WRITE_COMMAND_SIZE
	};

	if (!dev)
		return -EINVAL;

	packet.command = command;
	packet.reg = lt8722_regs[address];
	packet.data = data;

	return lt8722_transaction(dev, &packet);
}

/**
 * @brief Write to LT8722 device register with mask.
 * @param dev - LT8722 device descriptor
 * @param address - Register address.
 * @param mask - Mask to be applied.
 * @param data - Data to be written.
 * @return 0 in case of succes, negative error code otherwise
*/
int lt8722_reg_write_mask(struct lt8722_dev *dev, uint8_t address,
			  uint32_t mask, uint32_t data)
{
	int ret;
	uint32_t reg_data;

	if (!dev)
		return -EINVAL;

	ret = lt8722_reg_read(dev, address, &reg_data);
	if (ret)
		return ret;

	reg_data &= ~mask;
	reg_data |= no_os_field_prep(mask, data);

	return lt8722_reg_write(dev, address, reg_data);
}

/**
 * @brief Get LT8722 device status.
 * @param dev - LT8722 device descriptor
 * @param status - Status value to be returned.
 * @return 0 in case of succes, negative error code otherwise
*/
int lt8722_get_status(struct lt8722_dev *dev, uint16_t *status)
{
	int ret;
	struct lt8722_packet packet;
	struct lt8722_command command = {
		LT8722_STATUS_ACQUISITION_COMMAND,
		LT8722_STATUS_ACQUISITION_COMMAND_SIZE
	};

	if (!dev || !status)
		return -EINVAL;

	packet.command = command;
	packet.reg = lt8722_regs[LT8722_SPIS_STATUS];

	ret = lt8722_transaction(dev, &packet);
	if (ret)
		return ret;

	*status = packet.status;

	return 0;
}

/**
 * @brief Clear LT8722 device faults.
 * @param dev - LT8722 device descriptor
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_clear_faults(struct lt8722_dev *dev)
{
	int ret;
	uint16_t status;

	if (!dev)
		return -EINVAL;

	ret = lt8722_get_status(dev, &status);
	if (ret)
		return ret;

	return lt8722_reg_write_mask(dev, LT8722_SPIS_STATUS,
				     LT8722_FAULTS_MASK, 0);
}

/**
 * @brief Set ENABLE_REQ field in LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Enable if true, disabled otherwise
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_enable_req(struct lt8722_dev *dev, bool value)
{
	if (!dev)
		return -EINVAL;

	return lt8722_reg_write_mask(dev, LT8722_SPIS_COMMAND,
				     LT8722_ENABLE_REQ_MASK, value);
}

/**
 * @brief Get ENABLE_REQ field in LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Enable if true, disabled otherwise
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_get_enable_req(struct lt8722_dev *dev, bool *value)
{
	int ret;
	uint32_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = lt8722_reg_read(dev, LT8722_SPIS_COMMAND, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LT8722_ENABLE_REQ_MASK, data);

	return 0;
}

/**
 * @brief Set switching enable of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Enable if true, disabled otherwise
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_swen_req(struct lt8722_dev *dev, bool value)
{
	if (!dev)
		return -EINVAL;

	return lt8722_reg_write_mask(dev, LT8722_SPIS_COMMAND,
				     LT8722_SWEN_REQ_MASK, value);
}

/**
 * @brief Get switching enable of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Enable if true, disabled otherwise
 * @return 0 in case of succes, negative error code otherwise
*/
int lt8722_get_swen_req(struct lt8722_dev *dev, bool *value)
{
	int ret;
	uint32_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = lt8722_reg_read(dev, LT8722_SPIS_COMMAND, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LT8722_SWEN_REQ_MASK, data);

	return 0;
}

/**
 * @brief Set switching frequency of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Switching frequency value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_sw_frq_set(struct lt8722_dev *dev, enum lt8722_sw_frq_set value)
{
	if (!dev)
		return -EINVAL;

	return lt8722_reg_write_mask(dev, LT8722_SPIS_COMMAND,
				     LT8722_SW_FRQ_SET_MASK, value);
}

/**
 * @brief Get switching frequency of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Switching frequency value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_get_sw_frq_set(struct lt8722_dev *dev, enum lt8722_sw_frq_set *value)
{
	int ret;
	uint32_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = lt8722_reg_read(dev, LT8722_SPIS_COMMAND, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LT8722_SW_FRQ_SET_MASK, data);

	return 0;
}

/**
 * @brief Set switching frequency adjustment of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Switching frequency adjustment value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_sw_frq_adj(struct lt8722_dev *dev, enum lt8722_sw_frq_adj value)
{
	if (!dev)
		return -EINVAL;

	return lt8722_reg_write_mask(dev, LT8722_SPIS_COMMAND,
				     LT8722_SW_FRQ_ADJ_MASK, value);
}

/**
 * @brief Get switching frequency adjustment of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Switching frequency value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_get_sw_frq_adj(struct lt8722_dev *dev, enum lt8722_sw_frq_adj *value)
{
	int ret;
	uint32_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = lt8722_reg_read(dev, LT8722_SPIS_COMMAND, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LT8722_SW_FRQ_ADJ_MASK, data);

	return 0;
}

/**
 * @brief Set internal circuitry voltage source of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - VCC voltage regulator output
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_vcc_vreg(struct lt8722_dev *dev, enum lt8722_vcc_vreg value)
{
	if (!dev)
		return -EINVAL;

	return lt8722_reg_write_mask(dev, LT8722_SPIS_COMMAND,
				     LT8722_VCC_VREG_MASK, value);
}

/**
 * @brief Get internal circuitry voltage source of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - VCC voltage regulator output
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_get_vcc_vreg(struct lt8722_dev *dev, enum lt8722_vcc_vreg *value)
{
	int ret;
	uint32_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = lt8722_reg_read(dev, LT8722_SPIS_COMMAND, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LT8722_VCC_VREG_MASK, data);

	return 0;
}

/**
 * @brief Set initial peak inductor current of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Peak inductor current value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_sw_vc_int(struct lt8722_dev *dev,
			 enum lt8722_sw_vc_int value)
{
	if (!dev)
		return -EINVAL;

	return lt8722_reg_write_mask(dev, LT8722_SPIS_COMMAND,
				     LT8722_SW_VC_INT_MASK, value);
}

/**
 * @brief Get initial peak inductor current of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Peak inductor current value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_get_sw_vc_int(struct lt8722_dev *dev, enum lt8722_sw_vc_int *value)
{
	int ret;
	uint32_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = lt8722_reg_read(dev, LT8722_SPIS_COMMAND, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LT8722_SW_VC_INT_MASK, data);

	return 0;
}

/**
 * @brief Set LDR power dissipation limit of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - LDR power dissipation limit value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_pwr_lim(struct lt8722_dev *dev, enum lt8722_pwr_lim value)
{
	if (!dev)
		return -EINVAL;

	return lt8722_reg_write_mask(dev, LT8722_SPIS_COMMAND,
				     LT8722_PWR_LIM_MASK, value);
}

/**
 * @brief Get LDR power dissipation limit of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - LDR power dissipation limit value
 * @return 0 in case of success, negative error code otherwise
 */
int lt8722_get_pwr_lim(struct lt8722_dev *dev, enum lt8722_pwr_lim *value)
{
	int ret;
	uint32_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = lt8722_reg_read(dev, LT8722_SPIS_COMMAND, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LT8722_PWR_LIM_MASK, data);

	return 0;
}

/**
 * @brief Set PWM duty cycle of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - PWM duty cycle value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_sys_dc(struct lt8722_dev *dev, enum lt8722_sys_dc value)
{
	if (!dev)
		return -EINVAL;

	return lt8722_reg_write_mask(dev, LT8722_SPIS_COMMAND,
				     LT8722_SYS_DC_MASK, value);
}

/**
 * @brief Get PWM duty cycle of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - PWM duty cycle value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_get_sys_dc(struct lt8722_dev *dev, enum lt8722_sys_dc *value)
{
	int ret;
	uint32_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = lt8722_reg_read(dev, LT8722_SPIS_COMMAND, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LT8722_SYS_DC_MASK, data);

	return 0;
}

/**
 * @brief Set negative output current limit of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Negative output current limit value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_spis_dac_ilimn(struct lt8722_dev *dev, uint16_t value)
{
	if (!dev)
		return -EINVAL;

	if ((value < LT8722_SPIS_DAC_ILIMN_MIN)
	    || (value > LT8722_SPIS_DAC_ILIMN_MAX)) {
		pr_err("Value: 0x%08x is out of range\n", value);
		return -EINVAL;
	}

	return lt8722_reg_write_mask(dev, LT8722_SPIS_DAC_ILIMN,
				     LT8722_SPIS_DAC_ILIMN_MASK, value);
}

/**
 * @brief Get negative output current limit of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Negative output current limit value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_get_spis_dac_ilimn(struct lt8722_dev *dev, uint16_t *value)
{
	int ret;
	uint32_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = lt8722_reg_read(dev, LT8722_SPIS_DAC_ILIMN, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LT8722_SPIS_DAC_ILIMN_MASK, data);

	return 0;
}

/**
 * @brief Set positive output current limit of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Positive output current limit value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_spis_dac_ilimp(struct lt8722_dev *dev, uint16_t value)
{
	if (!dev)
		return -EINVAL;

	if ((value < LT8722_SPIS_DAC_ILIMP_MIN)
	    || (value > LT8722_SPIS_DAC_ILIMP_MAX)) {
		pr_err("Value: 0x%08x is out of range\n", value);
		return -EINVAL;
	}

	return lt8722_reg_write_mask(dev, LT8722_SPIS_DAC_ILIMP,
				     LT8722_SPIS_DAC_ILIMP_MASK, value);
}

/**
 * @brief Get positive output current limit of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Positive output current limit value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_get_spis_dac_ilimp(struct lt8722_dev *dev, uint16_t *value)
{
	int ret;
	uint32_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = lt8722_reg_read(dev, LT8722_SPIS_DAC_ILIMP, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LT8722_SPIS_DAC_ILIMP_MASK, data);

	return 0;
}

/**
 * @brief Set DAC code of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - DAC value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_dac(struct lt8722_dev *dev, uint32_t value)
{
	if (!dev)
		return -EINVAL;

	return lt8722_reg_write_mask(dev, LT8722_SPIS_DAC,
				     LT8722_SPIS_DAC_MASK, value);
}

/**
 * @brief Get DAC code of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - DAC value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_get_dac(struct lt8722_dev *dev, uint32_t *value)
{
	int ret;
	uint32_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = lt8722_reg_read(dev, LT8722_SPIS_DAC, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LT8722_SPIS_DAC_MASK, data);

	return 0;
}

/**
 * @brief Set positive output voltage limit of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Positive output voltage limit value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_spis_ov_clamp(struct lt8722_dev *dev, uint8_t value)
{
	if (!dev)
		return -EINVAL;

	return lt8722_reg_write_mask(dev, LT8722_SPIS_OV_CLAMP,
				     LT8722_SPIS_OV_CLAMP_MASK, value);
}

/**
 * @brief Get positive output voltage limit of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Positive output voltage limit value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_get_spis_ov_clamp(struct lt8722_dev *dev, uint8_t *value)
{
	int ret;
	uint32_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = lt8722_reg_read(dev, LT8722_SPIS_OV_CLAMP, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LT8722_SPIS_OV_CLAMP_MASK, data);

	return 0;
}

/**
 * @brief Set negative output voltage limit of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Negative output voltage limit value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_spis_uv_clamp(struct lt8722_dev *dev, uint8_t value)
{
	if (!dev)
		return -EINVAL;

	return lt8722_reg_write_mask(dev, LT8722_SPIS_UV_CLAMP,
				     LT8722_SPIS_UV_CLAMP_MASK, value);
}

/**
 * @brief Get negative output voltage limit of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Negative output voltage limit value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_get_spis_uv_clamp(struct lt8722_dev *dev, uint8_t *value)
{
	int ret;
	uint32_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = lt8722_reg_read(dev, LT8722_SPIS_UV_CLAMP, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LT8722_SPIS_UV_CLAMP_MASK, data);

	return 0;
}

/**
 * @brief Set analog output buffer status of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Analog output buffer status value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_aout_en(struct lt8722_dev *dev, bool value)
{
	if (!dev)
		return -EINVAL;

	return lt8722_reg_write_mask(dev, LT8722_SPIS_AMUX,
				     LT8722_AOUT_EN_MASK, value);
}

/**
 * @brief Get analog output buffer status of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Analog output buffer status value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_get_aout_en(struct lt8722_dev *dev, bool *value)
{
	int ret;
	uint32_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = lt8722_reg_read(dev, LT8722_SPIS_AMUX, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LT8722_AOUT_EN_MASK, data);

	return 0;
}

/**
 * @brief Set analot output signal source of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Analog output signal source value
 * @param test - AMUX_TEST value
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_amux(struct lt8722_dev *dev, enum lt8722_amux value,
		    uint8_t test)
{
	int ret;
	uint32_t data;

	if (!dev)
		return -EINVAL;

	ret = lt8722_reg_read(dev, LT8722_SPIS_AMUX, &data);
	if (ret)
		return ret;

	data &= ~LT8722_AMUX_MASK;
	data |= no_os_field_prep(LT8722_AMUX_MASK, value);
	data &= ~LT8722_AMUX_TEST_MASK;
	data |= no_os_field_prep(LT8722_AMUX_TEST_MASK, test);

	return lt8722_reg_write(dev, LT8722_SPIS_AMUX, data);
}

/**
 * @brief Get analot output signal source of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Analog output signal source value.
 * @param test - AMUX_TEST value.
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_get_amux(struct lt8722_dev *dev, enum lt8722_amux *value,
		    uint8_t *test)
{
	int ret;
	uint32_t data;

	if (!dev || !value || !test)
		return -EINVAL;

	ret = lt8722_reg_read(dev, LT8722_SPIS_AMUX, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LT8722_AMUX_MASK, data);
	*test = no_os_field_get(LT8722_AMUX_TEST_MASK, data);

	return 0;
}

/**
 * @brief Set output volatge of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Output voltage value in nanovolts.
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_output_voltage(struct lt8722_dev *dev, int64_t value)
{
	int32_t dac;
	int64_t vdac;

	if (!dev)
		return -EINVAL;

	vdac = LT8722_DAC_OFFSET - value / 16;
	dac = lt8722_voltage_to_dac(vdac);

	return lt8722_set_dac(dev, dac);
}

/**
 * @brief Get output volatge of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - Output voltage value in nanovolts.
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_get_output_voltage(struct lt8722_dev *dev, int64_t *value)
{
	int ret;
	uint32_t dac;
	int64_t vdac;

	if (!dev || !value)
		return -EINVAL;

	ret = lt8722_get_dac(dev, &dac);
	if (ret)
		return ret;

	vdac = lt8722_dac_to_voltage(dac);
	*value = -16 * (vdac - LT8722_DAC_OFFSET);

	return 0;
}

/**
 * @brief Reset the LT8722 device.
 * @param dev - LT8722 device descriptor
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_shutdown(struct lt8722_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = lt8722_set_enable_req(dev, LT8722_ENABLE_REQ_DISABLED);
	if (ret)
		return ret;

	return no_os_gpio_set_value(dev->gpio_en, NO_OS_GPIO_LOW);
}

/**
 * @brief Shutdown the LT8722 device.
 * @param dev - LT8722 device descriptor
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_reset(struct lt8722_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return lt8722_reg_write_mask(dev,LT8722_SPIS_COMMAND,
				     LT8722_SPI_RST_MASK, LT8722_SPI_RST_RESET);
}

/**
 * @brief Set SWEN pin of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - SWEN pin value.
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_swen_pin(struct lt8722_dev *dev, uint8_t value)
{
	if (!dev)
		return -EINVAL;

	return no_os_gpio_set_value(dev->gpio_swen, value);
}

/**
 * @brief Get SWEN pin of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - SWEN pin value.
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_get_swen_pin(struct lt8722_dev *dev, uint8_t *value)
{
	if (!dev || !value)
		return -EINVAL;

	return no_os_gpio_get_value(dev->gpio_swen, value);
}

/**
 * @brief Set EN pin of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - EN pin value.
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_set_en_pin(struct lt8722_dev *dev, uint8_t value)
{
	if (!dev)
		return -EINVAL;

	return no_os_gpio_set_value(dev->gpio_en, value);
}

/**
 * @brief Get EN pin of LT8722 device.
 * @param dev - LT8722 device descriptor
 * @param value - EN pin value.
 * @return 0 in case of succes, negative error code otherwise
 */
int lt8722_get_en_pin(struct lt8722_dev *dev, uint8_t *value)
{
	if (!dev || !value)
		return -EINVAL;

	return no_os_gpio_get_value(dev->gpio_en, value);
}

/**
 * @brief Initialize the LT8722 device.
 * @param device - LT8722 device descriptor
 * @param init_param - Initialization parameter containing information about the
 * 		LT8722 device to be initialized.
 * @return 0 in case of succes, negative error code otherwise
*/
int lt8722_init(struct lt8722_dev **device,
		struct lt8722_init_param *init_param)
{
	struct lt8722_dev *dev;
	int ret;
	uint32_t i;
	int32_t dac;
	int64_t voltage;
	int64_t start_voltage;
	int64_t end_voltage;

	if (!device || !init_param)
		return -EINVAL;

	dev = (struct lt8722_dev *)no_os_calloc(sizeof(*dev), 1);
	if (!dev)
		return -ENOMEM;

	ret = no_os_spi_init(&dev->spi, init_param->spi);
	if (ret)
		goto free_desc;

	ret = no_os_gpio_get_optional(&dev->gpio_swen, init_param->gpio_swen);
	if (ret)
		goto free_desc;

	ret = no_os_gpio_direction_output(dev->gpio_swen, NO_OS_GPIO_LOW);
	if (ret)
		goto free_desc;

	ret = no_os_gpio_get_optional(&dev->gpio_en, init_param->gpio_en);
	if (ret)
		goto free_desc;

	ret = no_os_gpio_direction_output(dev->gpio_en, NO_OS_GPIO_LOW);
	if (ret)
		goto free_desc;

	no_os_crc8_populate_msb(lt8722_crc8, LT8722_CRC_POLYNOMIAL);

	// Reset LT8722
	ret = lt8722_reset(dev);
	if (ret)
		goto free_desc;

	if (init_param->start_up_sequence) {
		/*
		 * Start-up sequence
		 * 1. Apply proper VIN and VDDIO voltages
		 *
		 * 2. Enable VCC LDO and other LT8722 circuitry
		 */
		ret = lt8722_clear_faults(dev);
		if (ret)
			goto free_desc;

		ret = lt8722_set_en_pin(dev, NO_OS_GPIO_HIGH);
		if (ret)
			goto free_desc;

		ret = lt8722_set_enable_req(dev, LT8722_ENABLE_REQ_ENABLED);
		if (ret)
			goto free_desc;

		// 3. Configure output voltage control DAC to 0xFF000000
		ret = lt8722_set_dac(dev, 0xFF000000);
		if (ret)
			goto free_desc;

		// 4. Write all SPIS_STATUS registers to 0
		ret = lt8722_reg_write(dev, LT8722_SPIS_STATUS, 0);
		if (ret)
			goto free_desc;

		no_os_mdelay(1);

		// 5. Ramp the output voltage control DAC from 0xFF000000 to 0x00000000
		start_voltage = lt8722_dac_to_voltage(0xFF000000);
		end_voltage = lt8722_dac_to_voltage(0x00000000);

		for (i = 0;  i < 5; i++) {
			voltage = (start_voltage + (end_voltage - start_voltage) * i / 4);
			dac = lt8722_voltage_to_dac(voltage);

			ret = lt8722_set_dac(dev, dac);
			if (ret)
				goto free_desc;

			no_os_mdelay(1);
		}

		// 6. Enable the PWM switching behavior
		ret = lt8722_set_swen_pin(dev, NO_OS_GPIO_HIGH);
		if (ret)
			goto free_desc;

		ret = lt8722_set_swen_req(dev, LT8722_SWEN_REQ_ENABLED);
		if (ret)
			goto free_desc;

		no_os_udelay(160);

		// 7. Set the desired output voltage
	}

	dev->start_up_sequence = init_param->start_up_sequence;
	*device = dev;

	return 0;

free_desc:
	lt8722_remove(dev);

	return ret;
}

/**
 * @brief Free the resources allocated by the lt8722_init()
 * @param dev - LT8722 device descriptor
 * @return- 0 in case of succes, negative error code otherwise
*/
int lt8722_remove(struct lt8722_dev *dev)
{
	if (!dev)
		return -ENODEV;

	no_os_gpio_remove(dev->gpio_en);
	no_os_gpio_remove(dev->gpio_swen);
	no_os_spi_remove(dev->spi);
	no_os_free(dev);

	return 0;
}
