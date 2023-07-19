/***************************************************************************//**
 *   @file   maxim_i2c.c
 *   @brief  Implementation of max32660 I2C driver.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "i2c.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "maxim_i2c.h"
#include "mxc_errors.h"
#include "max32650.h"

#define I2C_MASTER_MODE	1

/** Used to know how many instances are created */
static uint32_t nb_created_desc[MXC_I2C_INSTANCES];

/**
 * @brief I2C0 interrupt handler.
 * @return void
 */
void I2C0_IRQHandler(void)
{
	MXC_I2C_AsyncHandler(MXC_I2C0);
}

/**
 * @brief I2C1 interrupt handler.
 * @return void
 */
void I2C1_IRQHandler(void)
{
	MXC_I2C_AsyncHandler(MXC_I2C1);
}

#ifdef MXC_I2C2
void I2C2_IRQHandler(void)
{
	MXC_I2C_AsyncHandler(MXC_I2C2);
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
	default:
		return -EINVAL;
	}

	i2c_pins.vssel = vssel;
	MXC_GPIO_Config(&i2c_pins);

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
		if((MXC_I2C_Init(i2c_regs, I2C_MASTER_MODE, 0)) != E_NO_ERROR) {
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

	if(nb_created_desc[desc->device_id] == 0) {
		ret = MXC_I2C_Shutdown(max_i2c->handler);
		if (ret != E_NO_ERROR)
			return ret;
		NVIC_DisableIRQ(MXC_I2C_GET_IRQ(desc->device_id));
	}

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
			     uint8_t bytes_number,
			     uint8_t stop_bit)
{
	mxc_i2c_regs_t *i2c_regs;
	mxc_i2c_req_t req;
	struct max_i2c_extra *max_i2c_desc;
	void *ptr;

	if (!desc || !desc->extra)
		return -EINVAL;

	i2c_regs = MXC_I2C_GET_I2C(desc->device_id);
	max_i2c_desc = desc->extra;

	if (stop_bit == 0) {
		max_i2c_desc->prologue_size = bytes_number;
		if (max_i2c_desc->prologue_data) {
			ptr = realloc(max_i2c_desc->prologue_data, bytes_number);
			max_i2c_desc->prologue_data = ptr;
		} else {
			max_i2c_desc->prologue_data = no_os_malloc(bytes_number);
			if (!max_i2c_desc->prologue_data)
				return -ENOMEM;
		}
		memcpy(max_i2c_desc->prologue_data, data, bytes_number);

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

	return MXC_I2C_MasterTransaction(&req);
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
			    uint8_t bytes_number,
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

	ret = MXC_I2C_MasterTransaction(&req);

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
