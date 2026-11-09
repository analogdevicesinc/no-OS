/***************************************************************************//**
 *   @file   maxim_i2c.c
 *   @brief  Implementation of max32660 I2C driver.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "i2c.h"
#include "i2c_reva_regs.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "maxim_i2c.h"
#include "mxc_errors.h"
#include "max32665.h"

#define I2C_MASTER_MODE	1
/*
 * Poll iterations for the bus to go idle: several ms at typical CPU clocks,
 * while a STOP condition takes about 10 us at 100 kHz.
 */
#define I2C_BUS_IDLE_TIMEOUT	100000

/** Used to know how many instances are created */
static uint32_t nb_created_desc[MXC_I2C_INSTANCES];

/** Set when the last transaction on a bus ended with a repeated START */
static bool restart_pending[MXC_I2C_INSTANCES];

/**
 * @brief I2C0 interrupt handler.
 */
void I2C0_IRQHandler(void)
{
	MXC_I2C_AsyncHandler(MXC_I2C0_BUS0);
}

/**
 * @brief I2C1 interrupt handler.
 */
void I2C1_IRQHandler(void)
{
	MXC_I2C_AsyncHandler(MXC_I2C1_BUS0);
}

#ifdef MXC_I2C2
void I2C2_IRQHandler(void)
{
	MXC_I2C_AsyncHandler(MXC_I2C2_BUS0);
}
#endif

/**
 * @brief Configure the VDDIO level for a I2C interface
 * @param device_id - the interface number.
 * @param vssel - the VDDIO level.
 * @return 0 in case of success, -EINVAL otherwise.
 */
static int32_t _max_i2c_pins_config(uint32_t device_id, mxc_gpio_vssel_t vssel)
{
	mxc_gpio_cfg_t i2c_pins;

	switch (device_id) {
	case 0:
		i2c_pins = gpio_cfg_i2c0;
		break;
	case 1:
		i2c_pins = gpio_cfg_i2c1;
		break;
	case 2:
		i2c_pins = gpio_cfg_i2c2;
		break;
	default:
		return -EINVAL;
	}

	i2c_pins.vssel = vssel;
	MXC_GPIO_Config(&i2c_pins);

	return 0;
}

/**
 * @brief Wait for the bus to be released by the previous transaction.
 *
 * MXC_I2C_MasterTransaction() can return while the STOP condition is still
 * being generated (STATUS.BUSY set). A START issued in that window is lost by
 * MSDK v2024_10, leaving the transaction polling forever, and rejected with
 * E_BUSY by later MSDK releases (fixed in analogdevicesinc/msdk#1640).
 *
 * After a transaction that ended with a repeated START this controller still
 * holds the bus on purpose, so there is nothing to wait for. Otherwise wait
 * for both STATUS.BUSY and STATUS.MST_BUSY to clear: MST_BUSY is also still
 * set while the STOP is being generated, so it cannot tell the two cases
 * apart on its own.
 *
 * @param device_id - I2C controller index.
 * @param i2c_regs - I2C controller registers.
 * @return 0 when the bus is available, -ETIMEDOUT otherwise.
 */
static int32_t _max_i2c_wait_bus_idle(uint32_t device_id,
				      mxc_i2c_regs_t *i2c_regs)
{
	mxc_i2c_reva_regs_t *i2c = (mxc_i2c_reva_regs_t *)i2c_regs;
	uint32_t busy = MXC_F_I2C_REVA_STATUS_BUSY | MXC_F_I2C_REVA_STATUS_MST_BUSY;
	uint32_t timeout = I2C_BUS_IDLE_TIMEOUT;

	if (restart_pending[device_id])
		return 0;

	while (i2c->status & busy) {
		if (!--timeout)
			return -ETIMEDOUT;
	}

	return 0;
}

/**
 * @brief Initialize the I2C communication peripheral.
 * Supported bitrates are between 100kHz and 400 kHz.
 * Is slave address is 0, then this instance will be used for general call.
 * 10 bits addressing is not supported.
 * @param desc - Descriptor of the I2C device used in the call of the driver
 * functions.
 * @param param - Parameter used to configure the I2C device. The extra field
 * it is not used and must be set to NULL.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t max_i2c_init(struct no_os_i2c_desc **desc,
			    const struct no_os_i2c_init_param *param)
{
	struct max_i2c_init_param *eparam;
	int32_t ret;
	struct max_i2c_extra *max_i2c;
	mxc_i2c_regs_t *i2c_regs;
	uint32_t freq;

	if (!desc || !param || !param->extra)
		return -EINVAL;
	if (param->device_id >= MXC_I2C_INSTANCES)
		return -EINVAL;

	*desc = no_os_calloc(1, sizeof(**desc));
	if (!(*desc))
		return -ENOMEM;

	max_i2c = no_os_calloc(1, sizeof(*max_i2c));
	if (!max_i2c) {
		ret = -ENOMEM;
		goto error_desc;
	}

	eparam = param->extra;
	i2c_regs = MXC_I2C_GET_I2C(param->device_id);
	max_i2c->handler = i2c_regs;
	(*desc)->device_id = param->device_id;
	if (param->max_speed_hz <= MAX_I2C_STD_MODE) {
		(*desc)->max_speed_hz = MAX_I2C_STD_MODE;
	} else if (param->max_speed_hz <= MAX_I2C_FAST_MODE) {
		(*desc)->max_speed_hz = MAX_I2C_FAST_MODE;
	} else if (param->max_speed_hz <= MAX_I2C_FAST_PLUS_MODE) {
		(*desc)->max_speed_hz = MAX_I2C_FAST_PLUS_MODE;
	} else if (param->max_speed_hz <= MAX_I2C_HS_MODE) {
		(*desc)->max_speed_hz = MAX_I2C_HS_MODE;
	} else {
		ret = -EINVAL;
		goto error_extra;
	}
	(*desc)->slave_address = param->slave_address;
	(*desc)->extra = max_i2c;

	if (nb_created_desc[param->device_id] == 0) {
		MXC_I2C_Shutdown(max_i2c->handler);
		/** The last parameter (slave address) is ignored in master mode */
		if ((MXC_I2C_Init(i2c_regs, I2C_MASTER_MODE, 0)) != E_NO_ERROR) {
			ret = -1;
			goto error_extra;
		}
	}

	ret = _max_i2c_pins_config((*desc)->device_id, eparam->vssel);
	if (ret)
		return ret;

	freq = MXC_I2C_GetFrequency(i2c_regs);
	freq = no_os_min(freq, (*desc)->max_speed_hz);
	ret = MXC_I2C_SetFrequency(i2c_regs, freq);
	if (ret < 0)
		return -EINVAL;

	nb_created_desc[param->device_id]++;

	return 0;
error_extra:
	no_os_free(max_i2c);
error_desc:
	no_os_free(*desc);

	return ret;
}

/**
 * @brief Free the resources allocated by \ref i2c_init
 * @param desc - Descriptor of the I2C device
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t max_i2c_remove(struct no_os_i2c_desc *desc)
{
	struct max_i2c_extra *max_i2c = desc->extra;
	int32_t ret;

	if (!desc)
		return -EINVAL;
	nb_created_desc[desc->device_id]--;

	if (nb_created_desc[desc->device_id] == 0) {
		ret = MXC_I2C_Shutdown(max_i2c->handler);
		if (ret != E_NO_ERROR)
			return ret;
		NVIC_DisableIRQ(MXC_I2C_GET_IRQ(desc->device_id));
	}

	/* A write without a stop bit not followed by a read */
	if (max_i2c->prologue_data)
		no_os_free(max_i2c->prologue_data);
	no_os_free(max_i2c);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Write data to a slave device
 * @param desc - Descriptor of the I2C device
 * @param data - Buffer that stores the transmission data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t max_i2c_write(struct no_os_i2c_desc *desc,
			     uint8_t *data,
			     uint32_t bytes_number,
			     uint8_t stop_bit)
{
	mxc_i2c_regs_t *i2c_regs;
	mxc_i2c_req_t req;
	int32_t ret;
	struct max_i2c_extra *max_i2c_desc;
	void *ptr;

	if (!desc || !desc->extra)
		return -EINVAL;

	i2c_regs = MXC_I2C_GET_I2C(desc->device_id);
	max_i2c_desc = desc->extra;

	if (stop_bit == 0) {
		/* Drop any previous prologue, even if this one fails */
		if (max_i2c_desc->prologue_data)
			no_os_free(max_i2c_desc->prologue_data);
		max_i2c_desc->prologue_data = NULL;
		max_i2c_desc->prologue_size = 0;

		if (bytes_number) {
			ptr = no_os_malloc(bytes_number);
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, data, bytes_number);
			max_i2c_desc->prologue_data = ptr;
			max_i2c_desc->prologue_size = bytes_number;
		}

		return 0;
	}

	req.i2c = i2c_regs;
	req.addr = desc->slave_address;
	req.tx_buf = data;
	req.tx_len = bytes_number;
	/** Write transaction */
	req.rx_len = 0;
	req.rx_buf = data;
	req.restart = 0;

	ret = _max_i2c_wait_bus_idle(desc->device_id, i2c_regs);
	if (ret)
		return ret;

	ret = MXC_I2C_MasterTransaction(&req);
	restart_pending[desc->device_id] = false;

	return ret;
}

/**
 * @brief Read data from a slave device
 * @param desc - Descriptor of the I2C device
 * @param data - Buffer that stores the transmission data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Stop condition control. NOTE: not applicable in this case
 *                   Example: 0 - A stop condition will not be generated.
 *                            1 - A stop condition will be generated
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t max_i2c_read(struct no_os_i2c_desc *desc,
			    uint8_t *data,
			    uint32_t bytes_number,
			    uint8_t stop_bit)
{
	mxc_i2c_regs_t *i2c_regs;
	mxc_i2c_req_t req;
	int32_t ret;
	struct max_i2c_extra *max_i2c_desc;

	if (!desc || !desc->extra)
		return -EINVAL;

	max_i2c_desc = desc->extra;
	i2c_regs = MXC_I2C_GET_I2C(desc->device_id);
	req.i2c = i2c_regs;
	req.addr = desc->slave_address;
	/** Read transaction */
	req.tx_len = 0;
	req.tx_buf = NULL;
	req.rx_buf = data;
	req.rx_len = bytes_number;
	req.restart = stop_bit ? 0 : 1;

	if (max_i2c_desc->prologue_size != 0) {
		req.restart = 0;
		req.tx_len = max_i2c_desc->prologue_size;
		req.tx_buf = max_i2c_desc->prologue_data;
	}

	ret = _max_i2c_wait_bus_idle(desc->device_id, i2c_regs);
	if (!ret)
		ret = MXC_I2C_MasterTransaction(&req);
	restart_pending[desc->device_id] = !ret && req.restart;

	if (max_i2c_desc->prologue_size != 0) {
		no_os_free(max_i2c_desc->prologue_data);
		max_i2c_desc->prologue_size = 0;
		max_i2c_desc->prologue_data = NULL;
	}

	return ret;
}

/**
 * @brief MAXIM platform specific I2C platform ops structure
 */
const struct no_os_i2c_platform_ops max_i2c_ops = {
	.i2c_ops_init = &max_i2c_init,
	.i2c_ops_write = &max_i2c_write,
	.i2c_ops_read = &max_i2c_read,
	.i2c_ops_remove = &max_i2c_remove
};
