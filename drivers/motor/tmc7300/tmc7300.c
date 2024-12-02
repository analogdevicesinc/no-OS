/***************************************************************************//**
 *   @file   tmc7300.c
 *   @brief  Source file for the TMC7300 driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "no_os_uart.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"

#include "tmc7300.h"

/**
 * @brief Calculate the CRC8 checksum.
 * @param data - Data to calculate the checksum for.
 * @param len - Length of the data.
 */
static uint8_t tmc7300_crc(uint8_t *data, uint8_t len)
{
	int i, j;
	uint8_t crc = 0;
	uint8_t current_byte;

	for (i = 0; i < len; i++) {
		current_byte = data[i];
		for (j = 0; j < 8; j++) {
			if ((crc >> 7) ^ (current_byte & 0x01))
				crc = (crc << 1) ^ 0x07;
			else
				crc <<= 1;

			current_byte >>= 1;
		}
	}

	return crc;
}

/**
 * @brief Write a register.
 * @param desc - The TMC7300 descriptor.
 * @param addr - The register address.
 * @param val - The value to write.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_reg_write(struct tmc7300_desc *desc, uint32_t addr, uint32_t val)
{
	int ret;
	uint8_t tx_data[8] = {0};
	uint32_t rx_cnt = 0;

	if (!desc)
		return -ENODEV;

	tx_data[0] = 0x05;
	tx_data[1] = desc->addr;
	tx_data[2] = addr | NO_OS_BIT(7);
	no_os_put_unaligned_be32(val, &tx_data[3]);
	tx_data[7] = tmc7300_crc(tx_data, 7);

	ret = no_os_uart_write(desc->comm_desc, tx_data, 8);
	if (ret < 0)
		return ret;

	/* Flush the UART RX response */
	while (rx_cnt < 8) {
		ret = no_os_uart_read(desc->comm_desc, &tx_data[rx_cnt], 8 - rx_cnt);
		if (ret > 0)
			rx_cnt += ret;
	}

	return 0;
}

/**
 * @brief Read a register.
 * @param desc - The TMC7300 descriptor.
 * @param addr - The register address.
 * @param val - The value to read.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_reg_read(struct tmc7300_desc *desc, uint32_t addr, uint32_t *val)
{
	int ret;
	uint32_t rx_cnt = 0;
	uint8_t tx_data[4] = {0};
	uint8_t rx_data[12] = {0};
	uint8_t crc;

	if (!desc)
		return -ENODEV;

	tx_data[0] = 0x05;
	tx_data[1] = desc->addr;
	tx_data[2] = addr;
	tx_data[3] = tmc7300_crc(tx_data, 3);

	/* Flush the UART FIFO */
	do {
		ret = no_os_uart_read(desc->comm_desc, rx_data, 1);
	} while (ret != -EAGAIN);

	ret = no_os_uart_write(desc->comm_desc, tx_data, 4);
	if (ret < 0)
		return ret;

	while (rx_cnt < 12) {
		ret = no_os_uart_read(desc->comm_desc, &rx_data[rx_cnt], 12 - rx_cnt);
		if (ret > 0)
			rx_cnt += ret;
	}

	crc = tmc7300_crc(&rx_data[4], 7);
	if (crc != rx_data[11])
		return -EINVAL;

	*val = no_os_get_unaligned_be32(&rx_data[7]);

	return 0;
}

/**
 * @brief Update a register.
 * @param desc - The TMC7300 descriptor.
 * @param addr - The register address.
 * @param mask - The mask to apply.
 * @param val - The value of the field specified by the mask.
 * 		Will be shifted to the correct position.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_reg_update(struct tmc7300_desc *desc, uint32_t addr, uint32_t mask,
		       uint32_t val)
{
	uint32_t reg_val;
	int ret;

	ret = tmc7300_reg_read(desc, addr, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= no_os_field_prep(mask, val);

	ret = tmc7300_reg_write(desc, addr, reg_val);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the load indicator.
 * @param desc - The TMC7300 descriptor.
 * @param bridge - The bridge to get the load indicator for.
 * @param val - The load indicator value.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_get_load_indicator(struct tmc7300_desc *desc,
			       enum tmc7300_bridge bridge,
			       uint32_t *val)
{
	int ret;

	ret = tmc7300_reg_read(desc, TMC7300_DRV_STATUS_REG, val);
	if (ret)
		return ret;

	*val = no_os_field_get(TMC7300_LI_MASK(bridge), *val);

	return 0;
}

/**
 * @brief Enable the bridge driver.
 * @param desc - The TMC7300 descriptor.
 * @param enable - Enable or disable the driver.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_drv_enable(struct tmc7300_desc *desc, bool enable)
{
	int ret;

	if (!desc)
		return -ENODEV;

	ret = no_os_gpio_set_value(desc->en_gpio, enable);
	if (ret)
		return ret;

	return tmc7300_reg_update(desc, TMC7300_CHOPCONF_REG,
				  TMC7300_DRV_ENABLE_MASK, enable);
}

/**
 * @brief Set the PWM duty cycle.
 * @param desc - The TMC7300 descriptor.
 * @param bridge - The bridge to set the PWM duty cycle for.
 * @param duty - The duty cycle value.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_set_pwm_duty(struct tmc7300_desc *desc, enum tmc7300_bridge bridge,
			 int32_t duty)
{
	int ret;

	if (duty > TMC7300_DUTY_MAX_VALUE)
		return -EINVAL;

	ret = tmc7300_reg_update(desc, TMC7300_PWM_AB_REG,
				 TMC7300_PWM_MASK(bridge), duty);
	if (ret)
		return ret;

	if (duty < 0)
		desc->bridge_priv[bridge].motor_dir = TMC7300_DIR_CCW;
	else
		desc->bridge_priv[bridge].motor_dir = TMC7300_DIR_CW;

	desc->bridge_priv[bridge].pwm_duty = duty;

	return ret;
}

/**
 * @brief Set the current limit.
 * @param desc - The TMC7300 descriptor.
 * @param val - The current limit value.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_set_current_limit(struct tmc7300_desc *desc, uint32_t val)
{
	uint32_t reg_val;
	int ret;

	/* I_max = ((val + 1) / 32) * (0.325 / (R_sense + 0.03)) (A)*/
	if (val > 31)
		return -EINVAL;

	reg_val = no_os_field_prep(TMC7300_IRUN_MASK, val) |
		  (uint32_t)!desc->motor_drive;

	ret = tmc7300_reg_write(desc, TMC7300_CURRENT_LIMIT_REG, reg_val);
	if (ret)
		return ret;

	desc->irun = val;

	return 0;
}

/**
 * @brief Set the UART response delay.
 * @param desc - The TMC7300 descriptor.
 * @param delay - The delay value.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_set_send_delay(struct tmc7300_desc *desc, uint8_t delay)
{
	if (delay > TMC7300_SENDDELAY_MAX_VALUE || delay % 2)
		return -EINVAL;

	return tmc7300_reg_write(desc, TMC7300_SLAVECONF_REG,
				 no_os_field_prep(TMC7300_SLAVECONF_MASK, delay));
}

/**
 * @brief Get the number of UART valid write accesses. Wraps around at 255.
 * @param desc - The TMC7300 descriptor.
 * @param ifcnt - The number of valid write accesses.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_get_ifcnt(struct tmc7300_desc *desc, uint8_t *ifcnt)
{
	int ret;
	uint32_t reg_val;

	ret = tmc7300_reg_read(desc, TMC7300_IFCNT_REG, &reg_val);
	if (ret)
		return ret;

	*ifcnt = reg_val;

	return 0;
}

/**
 * @brief Read the driver status register.
 * @param desc - The TMC7300 descriptor.
 * @param status - The driver status.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_get_drv_status(struct tmc7300_desc *desc,
			   union tmc7300_drv_status *status)
{
	int ret;
	uint32_t reg_val;

	ret = tmc7300_reg_read(desc, TMC7300_DRV_STATUS_REG, &reg_val);
	if (ret)
		return ret;

	status->val = reg_val;

	return 0;
}

/**
 * @brief Read the IOIN register.
 * @param desc - The TMC7300 descriptor.
 * @param ioin - The IOIN register value.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_get_ioin(struct tmc7300_desc *desc, union tmc7300_ioin *ioin)
{
	int ret;
	uint32_t reg_val;

	ret = tmc7300_reg_read(desc, TMC7300_IOIN_REG, &reg_val);
	if (ret)
		return ret;

	ioin->val = reg_val;

	return 0;
}

/**
 * @brief Set the standstill mode. The motorrun bit will be set to 0.
 * @param desc - The TMC7300 descriptor.
 * @param mode - The standstill mode setting (freewheeling or break).
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_set_standstill_mode(struct tmc7300_desc *desc,
				enum tmc7300_standstill_mode mode)
{
	uint32_t reg_val;
	int ret;

	if (!desc)
		return -ENODEV;

	reg_val = no_os_field_prep(TMC7300_IRUN_MASK, desc->irun);
	ret = tmc7300_reg_write(desc, TMC7300_CURRENT_LIMIT_REG, reg_val);
	if (ret)
		return ret;

	reg_val = mode;

	return tmc7300_reg_update(desc, TMC7300_PWMCONF_REG,
				  TMC7300_FREEWHEEL_MASK, reg_val);
}

/**
 * @brief Set the frequency of the PWM signal.
 * @param desc - The TMC7300 descriptor.
 * @param freq - Division factor for the internal clock oscillator (12 MHz).
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_set_pwm_freq(struct tmc7300_desc *desc, enum tmc7300_pwm_freq freq)
{
	uint32_t reg_val;

	reg_val = freq;

	return tmc7300_reg_update(desc, TMC7300_PWMCONF_REG,
				  TMC7300_PWM_FREQ_MASK, reg_val);
}

/**
 * @brief Get the frequency of the PWM signal.
 * @param desc - The TMC7300 descriptor.
 * @param freq - Division factor for the internal clock oscillator (12 MHz).
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_get_pwm_freq(struct tmc7300_desc *desc, enum tmc7300_pwm_freq *freq)
{
	int ret;
	uint32_t reg_val;

	ret = tmc7300_reg_read(desc, TMC7300_PWMCONF_REG, &reg_val);
	if (ret)
		return ret;

	*freq = no_os_field_get(TMC7300_PWM_FREQ_MASK, reg_val);

	return 0;
}

/**
 * @brief Set the comparator blank time register.
 * @param desc - The TMC7300 descriptor.
 * @param time - The blank time setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_set_blank_time(struct tmc7300_desc *desc,
			   enum tmc7300_blank_time time)
{
	uint32_t reg_val;

	reg_val = time;

	return tmc7300_reg_update(desc, TMC7300_CHOPCONF_REG,
				  TMC7300_BLANK_TIME_MASK, reg_val);
}

/**
 * @brief Get the comparator blank time register.
 * @param desc - The TMC7300 descriptor.
 * @param time - The blank time setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_get_blank_time(struct tmc7300_desc *desc,
			   enum tmc7300_blank_time *time)
{
	int ret;
	uint32_t reg_val;

	ret = tmc7300_reg_read(desc, TMC7300_CHOPCONF_REG, &reg_val);
	if (ret)
		return ret;

	*time = no_os_field_get(TMC7300_BLANK_TIME_MASK, reg_val);

	return 0;
}

/**
 * @brief Set the PWM duty cycle and direction.
 * @param desc - The TMC7300 descriptor.
 * @param bridge - The bridge to set the PWM duty cycle for.
 * @param duty - The duty cycle value.
 * @param dir - The direction of the motor.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_set_pwm_duty_dir(struct tmc7300_desc *desc,
			     enum tmc7300_bridge bridge,
			     uint8_t duty, enum tmc7300_motor_dir dir)
{
	int32_t reg_val;
	int ret;

	if (dir == TMC7300_DIR_CW)
		reg_val = duty;
	else
		reg_val = -duty;

	ret = tmc7300_reg_update(desc, TMC7300_PWM_AB_REG,
				 TMC7300_PWM_MASK(bridge), (uint32_t)reg_val);
	if (ret)
		return ret;

	desc->bridge_priv[bridge].pwm_duty = duty;
	desc->bridge_priv[bridge].motor_dir = dir;

	return 0;
}

/**
 * @brief Initialize the TMC7300 driver.
 * @param desc - The TMC7300 descriptor.
 * @param param - The initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_init(struct tmc7300_desc **desc,
		 struct tmc7300_init_param *param)
{
	struct tmc7300_desc *descriptor;
	int ret;

	if (!param || !param->comm_desc)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->comm_desc = param->comm_desc;

	descriptor->addr = param->addr;
	descriptor->en_gpio = param->en_gpio;
	descriptor->vio_gpio = param->vio_gpio;

	ret = no_os_gpio_set_value(descriptor->en_gpio, 0);
	if (ret)
		goto free_desc;

	ret = no_os_gpio_set_value(descriptor->vio_gpio, 1);
	if (ret)
		goto free_desc;

	ret = tmc7300_reg_update(descriptor, TMC7300_GCONF_REG,
				 TMC7300_PWM_DIRECT_MASK, 1);
	if (ret)
		goto free_gpio;

	if (param->parallel_mode) {
		ret = tmc7300_reg_update(descriptor, TMC7300_GCONF_REG,
					 TMC7300_PAR_MODE_MASK, 1);
		if (ret)
			goto free_gpio;
	}

	*desc = descriptor;

	return 0;

free_gpio:
	no_os_gpio_remove(descriptor->en_gpio);
free_desc:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by tmc7300_init().
 * @param desc - The TMC7300 descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc7300_remove(struct tmc7300_desc *desc)
{
	if (!desc)
		return -ENODEV;

	no_os_gpio_remove(desc->en_gpio);
	no_os_free(desc);

	return 0;
}