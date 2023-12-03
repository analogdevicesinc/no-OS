
#include <stdio.h>
#include "no_os_init.h"
#include "no_os_uart.h"
#include "maxim_uart.h"
#include "no_os_spi.h"
#include "maxim_spi.h"
#include "no_os_delay.h"

#include "ad717x.h"
#include "ad411x_regs.h"

#include "no_os_gpio.h"
#include "maxim_gpio.h"

#include "lwip_socket.h"
#include "lwip_adin1110.h"
#include "adin1110.h"
#include "iio_app.h"
#include "iio_ad717x.h"

int main(void)
{
	struct max_uart_init_param uart_init_max = {
		.flow = UART_FLOW_DIS
	};
	struct no_os_uart_init_param uart_init = {
		.device_id = 0,
		.baud_rate = 115200,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = &max_uart_ops,
		.extra = &uart_init_max,
	};
	struct no_os_uart_desc *uart;

	struct max_spi_init_param cn0414_spi_extra  = {
		.num_slaves = 1,
		.polarity = SPI_SS_POL_LOW,
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};
	struct no_os_spi_init_param cn0414_spi_init = {
		.device_id = 1,
		.max_speed_hz = 4000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_3,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
		.extra = &cn0414_spi_extra,
	};
	struct ad717x_init_param ad4111_init = {
		.spi_init = cn0414_spi_init,
		.regs = ad4111_regs,
		.num_regs = NO_OS_ARRAY_SIZE(ad4111_regs),
		.active_device = ID_AD4111,
		.ref_en = true,
		.num_channels = 2,
		.num_setups = 1,
		.chan_map = {
			{
				.channel_enable = true,
				.setup_sel = 0,
				.analog_inputs.analog_input_pairs = VIN6_VIN7
			}
		},
		.setups = {
			{
				.bi_unipolar = false,
				.ref_buff = false,
				.input_buff = true,
				.ref_source = INTERNAL_REF
			},
		},
		.filter_configuration = {
			{
				.odr = sps_31250_a
			},
		},
		.mode =  CONTINUOUS,
	};
	struct ad717x_dev *ad4111;
	int ret;

	ret = no_os_init();
	if (ret < 0)
		return ret;

	ret = no_os_uart_init(&uart, &uart_init);
	if (ret < 0)
		return ret;
	
	no_os_uart_stdio(uart);

	printf("Hello\n");

	ret = ad717x_init(&ad4111, ad4111_init);
	if (ret < 0)
		return ret;



	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	struct iio_app_device iio_devices[] = {
		IIO_APP_DEVICE("ad4111", ad4111,
			       &ad717x_iio_descriptor, NULL, NULL, NULL),
	};



	uint8_t adin1110_mac_address[6] = {0x00, 0x18, 0x80, 0x03, 0x25, 0x80};

	struct max_gpio_init_param adin1110_reset_gpio_extra_ip = {
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};

	struct max_spi_init_param adin1110_spi_extra_ip  = {
		.num_slaves = 1,
		.polarity = SPI_SS_POL_LOW,
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};

	const struct no_os_gpio_init_param adin1110_reset_ip = {
		.port = 0,
		.number = 15,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &adin1110_reset_gpio_extra_ip,
	};

	const struct no_os_spi_init_param adin1110_spi_ip = {
		.device_id = 3,
		.max_speed_hz = 25000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
		.extra = &adin1110_spi_extra_ip,
	};

	struct adin1110_init_param adin1110_init = {
		.chip_type = ADIN1110,
		.comm_param = adin1110_spi_ip,
		.reset_param = adin1110_reset_ip,
		.append_crc = false,
	};

	memcpy(adin1110_init.mac_address, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);
	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.lwip_param.platform_ops = &adin1110_lwip_ops;
	app_init_param.lwip_param.mac_param = &adin1110_init;
	app_init_param.lwip_param.extra = NULL;
	memcpy(app_init_param.lwip_param.hwaddr, adin1110_mac_address,
	       NETIF_MAX_HWADDR_LEN);

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
