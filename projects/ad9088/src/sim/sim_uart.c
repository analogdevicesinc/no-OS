/***************************************************************************//**
 *   @file   sim_uart.c
 *   @brief  Simulation stand-ins for the Altera JTAG-UART and Nios V IRQ
 *           controller ops the IIO example names.
 *
 *           Built only with -DAD9088_SIM=ON.  CMake renames altera_uart_ops
 *           and altera_irq_ops to these.  On the host, iio_app serves IIOD
 *           over TCP (linux-userspace platform), so it never opens the UART
 *           and needs no interrupts; the ops exist so the example links and
 *           its init calls succeed.
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_irq.h"
#include "no_os_uart.h"

/******************************************************************************/
/* UART                                                                       */
/******************************************************************************/

static int32_t sim_uart_init(struct no_os_uart_desc **desc,
			     const struct no_os_uart_init_param *param)
{
	struct no_os_uart_desc *d;

	if (!desc || !param)
		return -EINVAL;

	d = no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->device_id = param->device_id;
	d->baud_rate = param->baud_rate;
	*desc = d;

	return 0;
}

static int32_t sim_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
			     uint32_t bytes_number)
{
	return -ENOSYS;
}

static int32_t sim_uart_write(struct no_os_uart_desc *desc,
			      const uint8_t *data, uint32_t bytes_number)
{
	return -ENOSYS;
}

static int32_t sim_uart_remove(struct no_os_uart_desc *desc)
{
	no_os_free(desc);

	return 0;
}

const struct no_os_uart_platform_ops sim_uart_ops = {
	.init = sim_uart_init,
	.read = sim_uart_read,
	.write = sim_uart_write,
	.remove = sim_uart_remove,
};

/******************************************************************************/
/* IRQ controller                                                             */
/******************************************************************************/

static int sim_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
			     const struct no_os_irq_init_param *param)
{
	struct no_os_irq_ctrl_desc *d;

	if (!desc || !param)
		return -EINVAL;

	d = no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->irq_ctrl_id = param->irq_ctrl_id;
	*desc = d;

	return 0;
}

static int sim_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
				     uint32_t irq_id,
				     struct no_os_callback_desc *cb)
{
	return 0;
}

static int sim_irq_unregister_callback(struct no_os_irq_ctrl_desc *desc,
				       uint32_t irq_id,
				       struct no_os_callback_desc *cb)
{
	return 0;
}

static int sim_irq_ctrl(struct no_os_irq_ctrl_desc *desc)
{
	return 0;
}

static int sim_irq_line(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	return 0;
}

static int sim_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	no_os_free(desc);

	return 0;
}

const struct no_os_irq_platform_ops sim_irq_ops = {
	.init = sim_irq_ctrl_init,
	.register_callback = sim_irq_register_callback,
	.unregister_callback = sim_irq_unregister_callback,
	.global_enable = sim_irq_ctrl,
	.global_disable = sim_irq_ctrl,
	.enable = sim_irq_line,
	.disable = sim_irq_line,
	.clear_pending = sim_irq_line,
	.set_pending = sim_irq_line,
	.remove = sim_irq_ctrl_remove,
};
