
#include <stdio.h>
#include "no_os_init.h"
#include "no_os_uart.h"
#include "maxim_uart.h"
#include "no_os_spi.h"
#include "maxim_spi.h"

#include "ad717x.h"
#include "ad411x_regs.h"

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
		.num_channels = 1,
		.num_setups = 1,
		.chan_map = {
			{
				.channel_enable = true,
				.setup_sel = 0,
				.analog_inputs.analog_input_pairs = TEMPERATURE_SENSOR
			}
		},
		.setups = {
			{
				.bi_unipolar = false,
				.ref_buff = true,
				.input_buff = true,
				.ref_source = INTERNAL_REF
			},
		},
		.filter_configuration = {
			{
				.odr = sps_1p25
			},
		},
		.mode =  CONTINUOUS,
	};
	struct ad717x_dev *ad4111;
	int32_t code;
	uint64_t vin;
	float vin_volts;
	float temp;
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

	while(1) {
		ret = ad717x_wait_for_ready(ad4111 , 100000);
		if(ret < 0)
			return ret;

		ret = ad717x_read_data(ad4111 , &code);
		if(ret < 0)
			return ret;

		// Code = (16777216 x Vin x 0.1) / Vref
		vin_volts = ((float)code * 2.5) / 1677721.6;

		// Temperature (°C) = (Conversion Result ÷ 477 μV) − 273.15

		temp = (vin_volts / 0.000477) - 273.15;

		printf("temperature: %.3f V (code: %d)\n", temp, code);
	}

	return 0;
}
