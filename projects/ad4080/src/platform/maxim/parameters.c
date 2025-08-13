#include "parameters.h"

struct max_spi_init_param data_spi_extra = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_spi_init_param cfg_spi_extra = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_usb_uart_init_param iio_uart_extra = {
	.vid = 0x0456,
	.pid = 0x8102,
};

struct max_gpio_init_param max_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

