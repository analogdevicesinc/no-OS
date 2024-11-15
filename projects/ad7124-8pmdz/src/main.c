/***************************************************************************//**
 *   @file   ad7124-8pmdz/src/main.c
 *   @brief  Implementation of Main Function.
 *   @author Drimbarean Andrei (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "app_config.h"
#include "no_os_error.h"
#include "iio.h"
#include "no_os_irq.h"
#include "no_os_uart.h"
#include "iio_ad7124.h"
#include "ad7124_regs.h"
#include "iio_app.h"
#include "parameters.h"

#include <sys/platform.h>
#include "adi_initialize.h"
#include <drivers/pwr/adi_pwr.h>
#ifdef XILINX_PLATFORM
#include <xil_cache.h>
#include "xilinx_irq.h"
#include "xilinx_uart.h"
#include "xilinx_spi.h"
#endif
#ifdef ADUCM_PLATFORM
#include "aducm3029_uart.h"
#include "aducm3029_irq.h"
#include "aducm3029_spi.h"
#endif

#define MAX_SIZE_BASE_ADDR		1024

static uint8_t in_buff[MAX_SIZE_BASE_ADDR];

#define ADC_DDR_BASEADDR	((uint32_t)in_buff)
#define NUMBER_OF_DEVICES	1

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main(void)
{
	int32_t status;

	/* iio descriptor. */
	struct iio_desc  *iio_device;

	/* iio initialization structure */
	struct iio_init_param iio_inital;

	/* Initialization for UART. */
	struct no_os_uart_init_param uart_init_par;

	/* IRQ initial configuration. */
	struct no_os_irq_init_param irq_init_param;

	/* IRQ instance. */
	struct no_os_irq_ctrl_desc *irq_desc;

	/* IIO application data. */
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

#ifdef XILINX_PLATFORM
	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();
	ret
	struct xil_uart_init_param platform_uart_init_par = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
		.type = UART_PL,
#else
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
#endif
	};
#endif //XILINX_PLATFORM

	struct no_os_uart_init_param ad7124_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
#ifdef XILINX_PLATFORM
		.extra = &platform_uart_init_par,
#else
		.extra = NULL,
#endif
		.platform_ops = UART_OPS,
	};

	status = platform_init();
	if (NO_OS_IS_ERR_VALUE(status))
		return status;

	struct ad7124_dev *ad7124_device;
	struct aducm_spi_init_param aducm_spi_ini = {
		.continuous_mode = true,
		.dma = false,
		.half_duplex = false,
		.master_mode = MASTER
	};
	struct no_os_spi_init_param spi_initial = {
		.chip_select = 0x00,
		.max_speed_hz = 10000000,
		.mode = NO_OS_SPI_MODE_3,
		.device_id = 1,
		.platform_ops = &aducm_spi_ops,
		.extra = &aducm_spi_ini
	};
	struct ad7124_init_param ad7124_initial = {
		.spi_init = &spi_initial,
		.regs = ad7124_init_regs_default,
		.spi_rdy_poll_cnt = 1000
	};
	struct iio_data_buffer iio_ad7124_read_buff = {
		.buff = ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR,
	};

	status = ad7124_setup(&ad7124_device, &ad7124_initial);
	if (status < 0)
		return status;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad7124-8", ad7124_device, &iio_ad7124_device,
			       &iio_ad7124_read_buff, NULL, NULL)
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NUMBER_OF_DEVICES;
	app_init_param.uart_init_params = ad7124_uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	return iio_app_run(app);
}

