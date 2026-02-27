#include "iio_lwip_example.h"
#include "common_data.h"
#include "iio_ad7124.h"
#include "iio_cn0391.h"
#include "iio_app.h"
#include "lwip_socket.h"
#include "lwip_adin1110.h"
#include "adin1110.h"
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

const struct no_os_spi_init_param adin1110_spi_ip = {
	.device_id = ADIN1110_SPI_DEVICE_ID,
	.max_speed_hz = ADIN1110_SPI_CLK_RATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS,
	.chip_select = ADIN1110_SPI_CS,
	.extra = &adin1110_spi_extra_ip,
};

struct adin1110_init_param adin1110_ip = {
	.chip_type = ADIN1110,
	.comm_param = adin1110_spi_ip,
	.reset_param = adin1110_reset_ip,
	.append_crc = false,
};

int iio_lwip_example_main(void)
{
	struct cn0391_dev *dev;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};
	int ret;

	ret = cn0391_init(&dev, &cn0391_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		IIO_APP_DEVICE("cn0391", dev,
			       &iio_cn0391_device, NULL, NULL, NULL),
		IIO_APP_DEVICE("ad7124-8", dev->ad7124_dev,
			       &iio_ad7124_device, NULL, NULL, NULL),
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

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		cn0391_remove(dev);
		return ret;
	}

	return iio_app_run(app);
}
