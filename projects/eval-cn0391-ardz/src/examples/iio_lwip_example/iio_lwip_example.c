/***************************************************************************//**
 *   @file   iio_lwip_example.c
 *   @brief  IIO over lwIP (ADIN1110 10BASE-T1L) example for eval-cn0391-ardz.
 *   @author Mircea Vlasin (mircea.vlasin@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <string.h>
#include "common_data.h"
#include "iio_cn0391.h"
#include "iio_app.h"
#include "lwip_socket.h"
#include "lwip_adin1110.h"
#include "adin1110.h"
#include "capi_spi.h"
#include "stm32_capi_spi.h"
#include "no_os_util.h"
#include "parameters.h"

uint8_t adin1110_mac_address[NETIF_MAX_HWADDR_LEN] = {0x00, 0x18, 0x80, 0x03, 0x25, 0x81};

const struct no_os_gpio_init_param adin1110_reset_ip = {
	.port = ADIN1110_GPIO_RESET_PORT,
	.number = ADIN1110_GPIO_RESET_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = &adin1110_reset_gpio_extra_ip,
};

extern SPI_HandleTypeDef hspi2;

static struct stm32_spi_extra_config adin1110_capi_spi_extra = {
	.hspi = &hspi2,
	.get_input_clock = HAL_RCC_GetPCLK1Freq,
};

static struct stm32_spi_device_extra adin1110_capi_spi_dev_extra = {
	.chip_select_port = ADIN1110_SPI_CS_PORT,
	.chip_select_pin = ADIN1110_SPI_CS,
};

static struct capi_spi_config adin1110_capi_spi_cfg = {
	.ops = &stm32_capi_spi_ops,
	.identifier = (uint64_t)(uintptr_t)SPI2,
	.extra = &adin1110_capi_spi_extra,
};

static struct capi_spi_device adin1110_capi_spi_dev = {
	.max_speed_hz = ADIN1110_SPI_CLK_RATE,
	.mode = CAPI_SPI_MODE_0,
	.extra = &adin1110_capi_spi_dev_extra,
};

struct adin1110_init_param adin1110_ip = {
	.chip_type = ADIN1110,
	.reset_param = adin1110_reset_ip,
	.append_crc = false,
};

static int adin1110_spi_init(struct adin1110_init_param *param)
{
	struct capi_spi_controller_handle *spi_ctrl = NULL;
	int ret;

	ret = capi_spi_init(&spi_ctrl, &adin1110_capi_spi_cfg);
	if (ret)
		return ret;

	adin1110_capi_spi_dev.controller = spi_ctrl;
	param->comm_desc = &adin1110_capi_spi_dev;

	return 0;
}

int example_main(void)
{
	struct cn0391_dev *dev;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};
	int ret;

	ret = cn0391_init(&dev, &cn0391_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		IIO_APP_DEVICE("ad7124-8", dev,
			       &iio_ad7124_cn0391_device, NULL, NULL, NULL),
#ifdef CN0391_IIO_SUPPORT
		IIO_APP_DEVICE("cn0391", dev,
			       &iio_cn0391_device, NULL, NULL, NULL),
#endif
	};

	memcpy(adin1110_ip.mac_address, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);
	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = cn0391_uart_ip;
	app_init_param.lwip_param.platform_ops = &adin1110_lwip_ops;
	app_init_param.lwip_param.mac_param = &adin1110_ip;
	app_init_param.lwip_param.extra = NULL;
	memcpy(app_init_param.lwip_param.hwaddr, adin1110_mac_address,
	       NETIF_MAX_HWADDR_LEN);

	ret = adin1110_spi_init(&adin1110_ip);
	if (ret) {
		cn0391_remove(dev);
		return ret;
	}

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		cn0391_remove(dev);
		return ret;
	}

	ret = iio_app_run(app);
	iio_app_remove(app);
	cn0391_remove(dev);

	return ret;
}
