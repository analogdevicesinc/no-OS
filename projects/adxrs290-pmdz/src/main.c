/***************************************************************************//**
 *   @file   adxrs290-pmdz/src/main.c
 *   @brief  Implementation of Main Function.
 *   @author Kister Genesis Jimenez  (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include "spi.h"
#include "spi_extra.h"
#include "adxrs290.h"
#include "iio_adxrs290.h"
#include "app_config.h"
#include "parameters.h"
#include "error.h"
#include "iio_app.h"
#include "platform_init.h"
#include "aducm3029_gpio.h"


#ifdef ADUCM_PLATFORM

#define MAX_SIZE_BASE_ADDR		3000
#define IIO_ADXRS290_BUFFER_SIZE	3000

static uint8_t in_buff[MAX_SIZE_BASE_ADDR];
static uint8_t adxrs290_buff[IIO_ADXRS290_BUFFER_SIZE];

#define IIO_ADXRS290_TRIGGER_NAME	"adxrs290_trigger_name"
#define GYRO_DDR_BASEADDR		((uint32_t)in_buff)
#define GPIO_SYNC_PIN_NUM		0x10

#define IRQ_CTRL_ID			0
#define IRQ_PLATFORM_OPS		(&aducm_irq_ops)

#endif

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main(void)
{
	int32_t status;

	struct adxrs290_dev *adxrs290_device;
	struct irq_ctrl_desc *irq_ctrl;

	/* Aducm platform dependent initialization for SPI. */
	struct aducm_spi_init_param spi_param = {
		.continuous_mode = true,
		.dma = false,
		.half_duplex = false,
		.master_mode = MASTER
	};

	struct spi_init_param init_param = {
		.device_id = 1,
		.chip_select = 0,
		.extra = &spi_param,
		.max_speed_hz = 900000,
		.mode = SPI_MODE_3,
		.platform_ops = &aducm_spi_ops
	};

	/* Initialization for Sync pin GPIO. */
	struct gpio_init_param gpio_sync_init_param = {
		.number = GPIO_SYNC_PIN_NUM,
		.platform_ops = &aducm_gpio_ops,
		.extra = NULL
	};

	struct adxrs290_init_param adxrs290_param = {
		.spi_init = init_param,
		.mode = ADXRS290_MODE_MEASUREMENT,
		.gpio_sync = &gpio_sync_init_param,
		.lpf = ADXRS290_LPF_480HZ,
		.hpf = ADXRS290_HPF_ALL_PASS
	};

	struct irq_init_param irq_param = {
		.irq_ctrl_id = IRQ_CTRL_ID,
		.platform_ops = IRQ_PLATFORM_OPS,
		.extra = NULL
	};

	status = platform_init();
	if (IS_ERR_VALUE(status))
		return status;

	status = irq_ctrl_init(&irq_ctrl, &irq_param);
	if (IS_ERR_VALUE(status))
		return status;

	status = adxrs290_init(&adxrs290_device, &adxrs290_param);
	if (IS_ERR_VALUE(status))
		return status;

	struct iio_adxrs290_desc iio_adxrs290_dev[1];
	struct iio_adxrs290_init_param iio_adxrs290_param = {
		.irq_nb = ADUCM_EXTERNAL_INT1_ID,
		.irq_config = IRQ_RISING_EDGE,
		.gpio_sync = &gpio_sync_init_param,
		.buf = adxrs290_buff,
		.buffer_size = IIO_ADXRS290_BUFFER_SIZE,
		.trigger_name = IIO_ADXRS290_TRIGGER_NAME,
		.irq_ctrl = irq_ctrl,
		.dev = adxrs290_device
	};

	status = iio_adxrs290_cfg(iio_adxrs290_dev, &iio_adxrs290_param);
	if (IS_ERR_VALUE(status))
		return status;

	struct iio_data_buffer rd_buf = {
		.buff = (void *)GYRO_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE_WITH_TRIGGER("adxrs290", iio_adxrs290_dev,
					    &adxrs290_iio_descriptor, &rd_buf,
					    NULL, IIO_ADXRS290_TRIGGER_NAME),
		IIO_APP_TRIGGER(IIO_ADXRS290_TRIGGER_NAME, NULL,
				&adxrs290_iio_trigger_descriptor)
	};

	printf("Startin iio app\n");

	return iio_app_run_irq_param(devices, ARRAY_SIZE(devices), irq_ctrl);
}
