/***************************************************************************//**
 *   @file   iio_ad4080.h
 *   @brief  Implementation of iio_ad463x.c.
 *   @author Niel Acuna (niel.acuna@analog.com)
 *           Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#include <maxim_irq.h>

#include <common_data.h>
#include <iio_example.h>

int main(void)
{
	int err;
	struct no_os_uart_desc *serial_log;
	struct max_uart_init_param serial_log_extra = {
		.flow = MAX_UART_FLOW_DIS,
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};
	struct no_os_uart_init_param serial_log_init_param = {
		.device_id = SERIAL_LOG_DEVICE_ID,
		.irq_id = SERIAL_LOG_IRQ_ID,
		.asynchronous_rx = SERIAL_LOG_ASYNC_RX,
		.baud_rate = SERIAL_LOG_BAUD_RATE,
		.size = SERIAL_LOG_SIZE,
		.parity = SERIAL_LOG_PARITY,
		.stop = SERIAL_LOG_STOP,
		.platform_ops = &max_uart_ops,
		.extra = &serial_log_extra,
	};
	struct no_os_irq_ctrl_desc *nvic_desc;
	struct no_os_irq_init_param nvic_desc_param = {
		.platform_ops = &max_irq_ops,
	};

	/* initialize our logging system before anything else */
	err = no_os_uart_init(&serial_log, &serial_log_init_param);
	if (err)
		goto err_serial_log;
	no_os_uart_stdio(serial_log);

	err = no_os_irq_ctrl_init(&nvic_desc, &nvic_desc_param);
	if (err)
		goto err_irq_ctrl_init;

	/* nacuna??? update this to get user configured AD4080 FIFO Full Port */
	err = no_os_irq_enable(nvic_desc, GPIO0_IRQn);
	if (err)
		goto err_irq_enable;
	
	err = iio_example_main();
	
	no_os_irq_disable(nvic_desc, GPIO0_IRQn);
err_irq_enable:
	no_os_irq_ctrl_remove(nvic_desc);
err_irq_ctrl_init:
	no_os_uart_remove(serial_log);
err_serial_log:
	return err;
}

