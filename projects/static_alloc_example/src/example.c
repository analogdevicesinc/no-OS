
#include <stdio.h>
#include "no_os_init.h"
#include "no_os_uart.h"
#include "maxim_uart.h"

#include "maxim_i2c.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"

#include "adt7420.h"
#include "adxl355.h"

#include "iio_app.h"
#include "iio_adt7420.h"
#include "iio_adxl355.h"

#include "lwip_socket.h"
#include "lwip_adin1110.h"
#include "adin1110.h"

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
	struct no_os_uart_desc uart_desc_mem = {};
	struct no_os_uart_desc *uart = &uart_desc_mem;
	float adt7420_temp;
	int ret;

	ret = no_os_init();
	if (ret < 0)
		return ret;

	struct max_i2c_init_param adt7420_i2c_extra = {
		.sda_pinctrl = MAX_GPIO_PINCTRL(0, 30, 1, NO_OS_PULL_NONE, MXC_GPIO_VSSEL_VDDIOH),
		.scl_pinctrl = MAX_GPIO_PINCTRL(0, 31, 1, NO_OS_PULL_NONE, MXC_GPIO_VSSEL_VDDIOH),
	};
	const struct no_os_i2c_init_param iip = {
		.device_id = 0,
		.max_speed_hz = 100000,
		.slave_address = 0x4B,
		.extra = NULL,
		.platform_ops = &max_i2c_ops,
		.extra = &adt7420_i2c_extra,
	};
	struct adt7420_init_param adt7420_user_init = {
		.interface_init = iip,
		.active_device = ID_ADT7420
	};
	struct max_spi_init_param adxl355_spi_extra_ip  = {
		.num_slaves = 1,
		.polarity = SPI_SS_POL_LOW,
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};

	struct no_os_spi_init_param adxl355_spi_ip = {
		.device_id = 4,
		.max_speed_hz = 1000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
		.platform_delays = {
			.cs_delay_first = 1000,
			.cs_delay_last = 1000,
		},
		.extra = &adxl355_spi_extra_ip,
	};

	struct adxl355_init_param adxl355_ip = {
		.comm_type = ADXL355_SPI_COMM,
		.comm_init.spi_init = adxl355_spi_ip,
		.dev_type = ID_ADXL355,
	};

	struct adxl355_dev adxl355_mem = {};
	struct adxl355_desc *adxl355 = &adxl355_mem;
	struct adt7420_dev adt7420_mem = {};
	struct adt7420_dev *adt7420 = &adt7420_mem;

	ret = no_os_uart_init(&uart, &uart_init);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);

	ret = adt7420_init(&adt7420, adt7420_user_init);
	if (ret)
		return ret;

	// ret = adxl355_init(&adxl355, adxl355_ip);
	// if (ret)
	// 	return ret;

	while(1){
		adt7420_temp = adt7420_get_temperature(adt7420);
		printf("Temperature: %f\n", adt7420_temp);
		no_os_mdelay(1000);
	}
}
