#include "parameters.h"
#include "maxim_spi.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"

struct max_uart_init_param adxl37x_uart_extra = {
	.flow = MAX_UART_FLOW_DIS,
};

struct max_gpio_init_param adxl37x_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_spi_init_param adxl37x_spi_extra = {
	.num_slaves = 1,
	.polarity   = SPI_SS_POL_LOW,
	.vssel      = MXC_GPIO_VSSEL_VDDIOH,
};
