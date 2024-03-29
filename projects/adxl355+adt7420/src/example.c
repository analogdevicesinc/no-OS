
#include <stdio.h>
#include "no_os_init.h"
#include "no_os_uart.h"
#include "maxim_uart.h"


#include "maxim_i2c.h"
#include "maxim_spi.h"

#include "adt7420.h"
#include "adxl355.h"

#include "iio_app.h"
#include "iio_adt7420.h"
#include "iio_adxl355.h"

#define DATA_BUFFER_SIZE 400
uint8_t iio_data_buffer[DATA_BUFFER_SIZE * sizeof(int16_t)];

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
	int ret;

	ret = no_os_init();
	if (ret < 0)
		return ret;
#if 0
	ret = no_os_uart_init(&uart, &uart_init);
	if (ret < 0)
		return ret;

	no_os_uart_stdio(uart);

	printf("Hello\n\r");

	printf("Bye\n\r");

	return 0;
#endif

#if 0
	struct max_i2c_init_param adt7420_i2c_extra = {
		.vssel = MXC_GPIO_VSSEL_VDDIOH
	};
	const struct no_os_i2c_init_param iip = {
		.device_id = 0,
		.max_speed_hz = 100000,
		.slave_address = 0x48,
		.extra = NULL,
		.platform_ops = &max_i2c_ops,
		.extra = &adt7420_i2c_extra,
	};
	struct adt7420_init_param adt7420_user_init = {
		.interface_init = iip,
		.active_device = ID_ADT7420
	};
	struct adt7420_iio_dev *adt7420_iio_desc;
	struct adt7420_iio_init_param adt7420_init_param;
	struct iio_app_desc *app;
	struct iio_data_buffer dbuff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * sizeof(uint16_t)
	};
	struct iio_app_init_param app_init_param = { 0 };

	adt7420_init_param.adt7420_dev_init = &adt7420_user_init;
	ret = adt7420_iio_init(&adt7420_iio_desc, &adt7420_init_param);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adt7420",
			.dev = adt7420_iio_desc,
			.dev_descriptor = adt7420_iio_desc->iio_dev,
			.read_buff = &dbuff
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uart_init;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
#endif

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
		.extra = &adxl355_spi_extra_ip,
	};

	struct adxl355_init_param adxl355_ip = {
		.comm_type = ADXL355_SPI_COMM,
		.comm_init.spi_init = adxl355_spi_ip,
		.dev_type = ID_ADXL355,
	};

	struct adxl355_iio_dev *adxl355_iio_desc;
	struct adxl355_iio_dev_init_param adxl355_iio_ip;
	struct iio_app_desc *app;
	struct iio_data_buffer accel_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE*3*sizeof(int)
	};
	struct iio_app_init_param app_init_param = { 0 };

	adxl355_iio_ip.adxl355_dev_init = &adxl355_ip;
	ret = adxl355_iio_init(&adxl355_iio_desc, &adxl355_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adxl355",
			.dev = adxl355_iio_desc,
			.dev_descriptor = adxl355_iio_desc->iio_dev,
			.read_buff = &accel_buff,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uart_init;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
