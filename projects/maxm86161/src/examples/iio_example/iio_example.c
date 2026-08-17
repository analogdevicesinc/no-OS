/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example for the MAXM86161 driver.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include "common_data.h"
#include "iio_maxm86161.h"
#include "iio_app.h"
#include "no_os_circular_buffer.h"
#include "no_os_delay.h"
#include "no_os_irq.h"
#include "no_os_print_log.h"
#include "parameters.h"

#define DATA_BUFFER_SIZE 512

/*
 * Set to 1 to run an in-code drain self-test before the IIO app starts. It
 * exercises the same ISR -> circular buffer -> decode path that submit() uses,
 * printing decoded tag/data over the UART stdio. Useful under GDB when no
 * libiio host is available. Set back to 0 for normal operation.
 */
#define MAXM86161_SELFTEST 1

#if MAXM86161_SELFTEST
static void maxm86161_selftest(struct maxm86161_iio_desc *desc)
{
	uint32_t sample;
	int ret;

	pr_info("selftest: starting capture\n");

	/* Start capture the same way a buffer_enable=1 write would. */
	desc->data_capture = true;
	ret = maxm86161_set_shutdown(desc->drv_dev, false);
	if (ret) {
		pr_err("selftest: exit shutdown failed: %d\n", ret);
		desc->data_capture = false;
		return;
	}

	for (int loop = 0; loop < 20; loop++) {
		uint32_t drained = 0;

		no_os_mdelay(200);

		/*
		 * Manually run the ISR body to pull the HW FIFO into the
		 * circular buffer. The real INTB IRQ may also fire; this just
		 * guarantees a drain each iteration for the test.
		 */
		maxm86161_iio_fifo_irq_handler(desc);

		/* Now do what submit() does: pop decoded words from the CB. */
		while (no_os_cb_read(desc->fifo_buf, &sample,
				     sizeof(sample)) == 0) {
			uint8_t tag = (sample >> MAXM86161_FIFO_TAG_SHIFT) &
				      NO_OS_GENMASK(4, 0);
			uint32_t data = sample & MAXM86161_FIFO_DATA_MSK;

			pr_info("selftest: tag=%u data=%lu\n", tag,
				(unsigned long)data);
			drained++;
		}

		pr_info("selftest: loop %d drained %lu samples\n", loop,
			drained);
	}

	desc->data_capture = false;
	maxm86161_set_shutdown(desc->drv_dev, true);
	pr_info("selftest: done\n");
}
#endif /* MAXM86161_SELFTEST */

int example_main(void)
{
	struct maxm86161_iio_desc *maxm86161_iio;
	struct no_os_irq_ctrl_desc *irq_ctrl;
	struct iio_device *iio_dev;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	int ret;

	ret = no_os_irq_ctrl_init(&irq_ctrl, &maxm86161_gpio_irq_ip);
	if (ret) {
		pr_err("IRQ ctrl init failed: %d\n", ret);
		return ret;
	}

	struct maxm86161_iio_init_param maxm86161_iio_ip = {
		.drv_init_param = &maxm86161_ip,
		.fifo_watermark = 50,
		.fifo_buf_size = DATA_BUFFER_SIZE,
		.irq_ctrl = irq_ctrl,
		.irq_id = MAXM86161_GPIO_TRIG_IRQ_ID,
	};

	pr_info("MAXM86161 IIO Example\n");

	ret = maxm86161_iio_init(&maxm86161_iio, &maxm86161_iio_ip);
	if (ret) {
		pr_err("MAXM86161 IIO init failed: %d\n", ret);
		no_os_irq_ctrl_remove(irq_ctrl);
		return ret;
	}

	ret = maxm86161_iio_get_dev_descriptor(maxm86161_iio, &iio_dev);
	if (ret)
		goto err;

	struct iio_app_device iio_devices[] = {
		{
			.name = "maxm86161",
			.dev = maxm86161_iio,
			.dev_descriptor = iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = (struct no_os_uart_init_param) {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
		.extra = UART_EXTRA,
	};

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto err;

	ret = iio_app_run(app);

	iio_app_remove(app);

err:
	maxm86161_iio_remove(maxm86161_iio);
	no_os_irq_ctrl_remove(irq_ctrl);
	return ret;
}
