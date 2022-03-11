#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parameters.h"
#include "no-os/uart.h"
#include "no-os/gpio.h"
#include "no-os/irq.h"
#include "no-os/i2c.h"
#include "no-os/spi.h"
#include "no-os/delay.h"
#include "no-os/util.h"
#include "stm32_spi.h"
#include "stm32_gpio.h"
#include "stm32_irq.h"
#include "app.h"
#include "ad5940.h"

#ifdef IIO_SUPPORT
#include "iio_ad5940.h"
#include "iio_app.h"
#endif

#include <errno.h>
#include "stm32_uart.h"
#include "stm32_i2c.h"
#include "stm32_uart_stdio.h"

struct spi_desc *spi;
struct i2c_desc *i2c;
struct uart_desc *uart;
struct uart_desc *uart4;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;

volatile int tx = 0;
volatile int rx = 0;

void uart_tx_callback(void *context) {
	tx++;
}

void uart_rx_callback(void *context) {
	rx++;
}

int main(void)
{
	int ret;

	stm32_init();

	struct stm32_uart_init_param xu5ip = {
		.huart = &huart5,
		.timeout = 10,
	};
	struct uart_init_param uip = {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = UART_CS_8,
		.parity = UART_PAR_NO,
		.stop = UART_STOP_1_BIT,
		.extra = &xu5ip,
	};

	ret = uart_init(&uart, &uip);
	if (ret < 0)
		goto error;

	stm32_uart_stdio(uart);
	printf("Hello!\n");

#define UART_IRQ
#ifdef UART_IRQ
	struct irq_callback uart_tx_cb = {
			.event = HAL_UART_TX_COMPLETE_CB_ID,
			.callback = uart_tx_callback,
			.context = (void *)1,
			.peripheral = NO_OS_UART_IRQ,
	};
	struct irq_callback uart_rx_cb = {
			.event = HAL_UART_RX_COMPLETE_CB_ID,
			.callback = uart_rx_callback,
			.context = (void *)2,
			.peripheral = NO_OS_UART_IRQ,
	};
	UART_HandleTypeDef *huart = ((struct stm32_uart_desc *)uart->extra)->huart;
	struct irq_init_param uart_int_ip = {
		.platform_ops = &stm32_irq_ops,
		.extra = huart,
	};
	struct irq_ctrl_desc *nvic;
	ret = irq_ctrl_init(&nvic, &uart_int_ip);
	if (ret < 0)
		return ret;

	ret = irq_register_callback(nvic, UART5_IRQn, &uart_tx_cb);
	if (ret < 0)
		return ret;

	ret = irq_register_callback(nvic, UART5_IRQn, &uart_rx_cb);
	if (ret < 0)
		return ret;

	ret = irq_enable(nvic, UART5_IRQn);
	if (ret < 0)
		return ret;

	uint8_t c;
	while(1) {
		c = '?';

		HAL_UART_Receive_IT(huart, &c, 1);

		while(!rx)
			;
		rx = 0;

		HAL_UART_Transmit_IT(huart, &c, 1);

		while(!tx)
			;
		tx = 0;
	}
#endif
// ------------------------------------------------------------------------------------------------
	struct i2c_init_param i2cip = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = 100000,
		.slave_address = 0x70,
		.platform_ops = &stm32_i2c_ops,
	};
	ret = i2c_init(&i2c, &i2cip);
	if (ret)
		goto error;

	struct stm32_spi_init_param xsip  = {
		.chip_select_port = SPI_CS_PORT,
		.get_input_clock = HAL_RCC_GetPCLK1Freq,
	};

	struct spi_init_param sip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 3000000,
		.bit_order = SPI_BIT_ORDER_MSB_FIRST,
		.mode = SPI_MODE_0,
		.extra = &xsip,
		.platform_ops = &stm32_spi_ops,
		.chip_select = SPI_CS,
	};

	struct stm32_gpio_init_param reset_xgip = {
		.port = GPIOD,
		.mode = GPIO_MODE_OUTPUT_PP,
		.pull = GPIO_NOPULL,
		.speed = GPIO_SPEED_FREQ_VERY_HIGH,
	};

	struct gpio_init_param reset_gip = {
		.number = 12,
		.platform_ops = &stm32_gpio_ops,
		.extra = &reset_xgip,
	};

	struct stm32_gpio_init_param gp0_xgip = {
		.port = GPIOG,
		.mode = GPIO_MODE_IT_FALLING,
		.pull = GPIO_NOPULL,
		.speed = GPIO_SPEED_FREQ_VERY_HIGH,
	};

	struct gpio_init_param gp0_gip = {
		.number = 7,
		.platform_ops = &stm32_gpio_ops,
		.extra = &gp0_xgip,
	};

	struct ad5940_init_param ad5940_ip = {
		.spi_init = sip,
		.reset_gpio_init = reset_gip,
		.gp0_gpio_init = gp0_gip,
	};

#ifndef IIO_SUPPORT
	ret = app_main(i2c, &ad5940_ip);
	if (ret < 0)
		goto error;
#else
	struct ad5940_iio_dev *ad5940_iio = NULL;
	struct ad5940_iio_chan chan_init[] = {
			{
					.name = "Vce-Vbias",
					.num = 0,
					.pos = ADCMUXP_VCE0,
					.neg = ADCMUXN_VSET1P1,
			},
			{
					.name = "Vre-Vbias",
					.num = 1,
					.pos = ADCMUXP_VRE0,
					.neg = ADCMUXN_VSET1P1,
			},
			{
					.name = "Vse-Vbias",
					.num = 2,
					.pos = ADCMUXP_VSE0,
					.neg = ADCMUXN_VSET1P1,
			},
			{
					.name = "Vde-Vbias",
					.num = 3,
					.pos = ADCMUXP_VDE0,
					.neg = ADCMUXN_VSET1P1,
			},
			{
					.name = "Ain0-Vbias",
					.num = 4,
					.pos = ADCMUXP_AIN0,
					.neg = ADCMUXN_VSET1P1,
			},
			{
					.name = "Ain1-Vbias",
					.num = 5,
					.pos = ADCMUXP_AIN1,
					.neg = ADCMUXN_VSET1P1,
			},
			{
					.name = "Ain2-Vbias",
					.num = 6,
					.pos = ADCMUXP_AIN2,
					.neg = ADCMUXN_VSET1P1,
			},
			{
					.name = "Ain3-Vbias",
					.num = 7,
					.pos = ADCMUXP_AIN3,
					.neg = ADCMUXN_VSET1P1,
			},
	};
	struct ad5940_iio_init_param ad5940_iio_ip = {
			.ad5940_init = &ad5940_ip,
			.chan_init = chan_init,
			.chan_init_count = ARRAY_SIZE(chan_init),
	};
	ret = ad5940_iio_init(&ad5940_iio, &ad5940_iio_ip);
	if (ret < 0)
		goto error;

	struct iio_app_device devices[] = {
			{
				.name = "ad5940",
				.dev = ad5940_iio,
				.dev_descriptor = ad5940_iio->iio,
				.read_buff = NULL,
				.write_buff = NULL
			}
		};

	return iio_app_run(devices, ARRAY_SIZE(devices));
#endif

	printf("Bye!\n");
	return 0;
error:
	printf("Bye! (%d)\n", ret);
	return ret;
}
