/***************************************************************************//**
 *   @file   lattice_uart.c
 *   @brief  Source file of UART driver for Lattice RISC-V RX built in UART
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_error.h"
#include "no_os_uart.h"
#include "no_os_lf256fifo.h"
#include "no_os_alloc.h"
#include "lattice_uart.h"
#include "lattice_irq.h"
#include "hal.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc  - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t latt_uart_init(struct no_os_uart_desc **desc,
			      struct no_os_uart_init_param *param)
{
	struct latt_uart_desc *latt_uart;
	struct latt_uart_init_param *latt_uart_ip;
	struct no_os_uart_desc *descriptor;
	struct local_uart_instance *latt_uart_inst;
	volatile struct local_uart_dev *dev;
	int ret;

	if (!desc || !param || !param->extra)
		return -EINVAL;

	descriptor = (struct no_os_uart_desc *) no_os_malloc(sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	latt_uart = (struct latt_uart_desc *) no_os_malloc(sizeof(*latt_uart));
	if (!latt_uart) {
		ret = -ENOMEM;
		goto error;
	}
	latt_uart_inst = (struct local_uart_instance *) no_os_malloc(sizeof(*latt_uart_inst));
	if (!latt_uart_inst) {
		ret = -ENOMEM;
		goto error;
	}

	latt_uart->uart_instance = latt_uart_inst;
	descriptor->device_id = param->device_id;
	descriptor->baud_rate = param->baud_rate;
	descriptor->extra = latt_uart;
	latt_uart_ip = param->extra;

	if (!latt_uart_ip->base_addr) {
		ret = -EINVAL;
		goto error;
	}
	if (!latt_uart_ip->sys_clk) {
		ret = -EINVAL;
		goto error;
	}
	if (!descriptor->baud_rate) {
		ret = -EINVAL;
		goto error;
	}

	latt_uart_inst->base = latt_uart_ip->base_addr;
	latt_uart_inst->sys_clk = latt_uart_ip->sys_clk;
	latt_uart_inst->baudrate = param->baud_rate;
	dev = (volatile struct local_uart_dev *) (latt_uart_ip->base_addr);

	if (param->size > NO_OS_UART_CS_8) {
		ret = -EINVAL;
		goto error;
	}
	dev->lcr = param->size;

	switch (param->parity) {
	case NO_OS_UART_PAR_NO:
		dev->lcr &= (~0x18);
		break;
	case NO_OS_UART_PAR_ODD:
		dev->lcr |= 0x8;
		break;
	case NO_OS_UART_PAR_EVEN:
		dev->lcr |= 0x18;
		break;
	default:
		ret = -EINVAL;
		goto error;
	}

	dev->lcr |= param->stop == NO_OS_UART_STOP_1_BIT ? 0 : 4;

	uint32_t div = (latt_uart_inst->sys_clk) / latt_uart_inst->baudrate;
	dev->dlr_lsb = (uint8_t) (div);
	dev->dlr_msb = (uint8_t) (div >> 8);

	*desc = descriptor;

	return 0;

error:
	no_os_free(descriptor);
	no_os_free(latt_uart);

	return ret;
}

/**
 * @brief Free the resources allocated by latt_uart_init().
 * @param desc - The UART descriptor.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t latt_uart_remove(struct no_os_uart_desc *desc)
{
	struct latt_uart_desc *latt_uart;

	if (!desc)
		return -EINVAL;

	latt_uart = desc->extra;
	no_os_free(latt_uart->uart_instance);

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
};

/**
 * @brief Write data to UART device.
 * @param desc         - Instance of UART.
 * @param data         - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t latt_uart_write(struct no_os_uart_desc *desc,
			       const uint8_t *data,
			       uint32_t bytes_number)
{
	struct latt_uart_desc *latt_uart;
	volatile struct local_uart_dev *dev;

	int ret;
	uint32_t i;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	latt_uart = desc->extra;
	if (!latt_uart->uart_instance->base)
		return -EINVAL;

	dev = (volatile struct local_uart_dev *) (latt_uart->uart_instance->base);

	for (i = 0; i < bytes_number;) {
		if (dev->lsr & LOCAL_UART_LSR_TX_RDY_MASK) {
			dev->rxtx = data[i];
			i++;
		}
	}

	return bytes_number;
}

/**
 * @brief Read data from UART device.
 * @param desc         - Instance of UART.
 * @param data         - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return positive number of received bytes in case of success, error code otherwise.
 */
static int32_t latt_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
			      uint32_t bytes_number)
{
	struct latt_uart_desc *latt_uart;
	volatile struct local_uart_dev *dev;

	int ret;
	uint32_t i;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	latt_uart = desc->extra;
	if (!latt_uart->uart_instance->base)
		return -EINVAL;

	dev = (volatile struct local_uart_dev *) (latt_uart->uart_instance->base);

	for (i = 0; i < bytes_number;) {
		if (dev->lsr & LOCAL_UART_LSR_RX_RDY_MASK) {
			data[i] = dev->rxtx;
			i++;
		}
	}

	return bytes_number;
}

/**
 * @brief Lattice specific UART platform ops structure
 */
const struct no_os_uart_platform_ops latt_uart_ops = {
	.init = &latt_uart_init,
	.read = &latt_uart_read,
	.write = &latt_uart_write,
	.remove = &latt_uart_remove
};
