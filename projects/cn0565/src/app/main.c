#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "parameters.h"
#include "no_os_uart.h"
#include "no_os_gpio.h"
#include "no_os_i2c.h"
#include "no_os_spi.h"
#include "no_os_irq.h"
#include "no_os_delay.h"
#include "no_os_util.h"

#if defined(STM32_PLATFORM)
#include "stm32_spi.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_i2c.h"
#include "stm32_irq.h"
#include "stm32_uart_stdio.h"
#elif defined(ADUCM_PLATFORM)
#include "platform_init.h"
#include "spi_extra.h"
#include "aducm3029_gpio.h"
#include "uart_extra.h"
#include "i2c_extra.h"
#include "aducm3029_gpio_irq.h"
#include "irq_extra.h"
#include "uart_stdio.h"
#endif

#include "app.h"
#include "ad5940.h"

#ifdef IIO_SUPPORT
#include "iio_ad5940.h"
#include "iio_adg2128.h"
#include "iio_app.h"
#endif

struct no_os_spi_desc *spi;
struct no_os_i2c_desc *i2c;
struct no_os_uart_desc *uart;

#if defined(STM32_PLATFORM)
extern UART_HandleTypeDef huart5;
#endif

void ad5940_int_callback(void *ctx)
{
	ucInterrupted = 1;
}

int main(void)
{
	int ret;

#if defined(STM32_PLATFORM)
	stm32_init();
#elif defined(ADUCM_PLATFORM)
	ret = platform_init();
	if (ret)
		return ret;
#endif

#ifndef IIO_SUPPORT
#if defined(STM32_PLATFORM)
	struct stm32_uart_init_param xuip = {
		.huart = &huart5,
		.timeout = 10,
	};
#elif defined(ADUCM_PLATFORM)
	struct aducm_uart_init_param xuip = {
		.parity = UART_NO_PARITY,
		.stop_bits = UART_ONE_STOPBIT,
		.word_length = UART_WORDLEN_8BITS
	};
#endif
	struct no_os_uart_init_param uip = {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &xuip,
		.asynchronous_rx = true,
		.irq_id = UART_IRQ_ID,
	};

	ret = no_os_uart_init(&uart, &uip);
	if (ret < 0)
		return ret;

#if defined(STM32_PLATFORM)
	stm32_uart_stdio(uart);
#elif defined(ADUCM_PLATFORM)
	init_uart_stdio(uart);
#endif
	printf("Hello!\n");
#endif
	struct no_os_i2c_init_param i2cip = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = I2C_BAUDRATE,
		.slave_address = 0x70,
#if defined(STM32_PLATFORM)
		.platform_ops = &stm32_i2c_ops,
#elif defined(ADUCM_PLATFORM)
		.platform_ops = &aducm_i2c_ops,
#endif
	};
	ret = no_os_i2c_init(&i2c, &i2cip);
	if (ret)
		goto error;

#if defined(STM32_PLATFORM)
	struct stm32_spi_init_param xsip  = {
		.chip_select_port = SPI_CS_PORT,
		.get_input_clock = HAL_RCC_GetPCLK1Freq,
	};
#elif defined(ADUCM_PLATFORM)
	struct aducm_spi_init_param xsip = {
		.continuous_mode = true,
		.dma = false,
		.half_duplex = false,
		.master_mode = MASTER
	};
#endif
	struct no_os_spi_init_param sip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 3000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.extra = &xsip,
#if defined(STM32_PLATFORM)
		.platform_ops = &stm32_spi_ops,
#elif defined(ADUCM_PLATFORM)
		.platform_ops = &aducm_spi_ops,
#endif
		.chip_select = SPI_CS,
	};

#if defined(STM32_PLATFORM)
	struct stm32_gpio_init_param reset_xgip = {
		.port = GPIOD,
		.mode = GPIO_MODE_OUTPUT_PP,
		.pull = GPIO_NOPULL,
		.speed = GPIO_SPEED_FREQ_VERY_HIGH,
	};
#endif
	struct no_os_gpio_init_param reset_gip = {
		.number = RESET_PIN,
#if defined(STM32_PLATFORM)
		.platform_ops = &stm32_gpio_ops,
		.extra = &reset_xgip,
#elif defined(ADUCM_PLATFORM)
		.platform_ops = &aducm_gpio_ops,
#endif
	};

#if defined(STM32_PLATFORM)
	struct stm32_gpio_init_param gp0_xgip = {
		.port = GPIOG,
		.mode = GPIO_MODE_INPUT,
		.pull = GPIO_NOPULL,
		.speed = GPIO_SPEED_FREQ_VERY_HIGH,
	};
#endif

	struct no_os_gpio_init_param gp0_gip = {
		.number = GP0_PIN,
#if defined(STM32_PLATFORM)
		.platform_ops = &stm32_gpio_ops,
		.extra = &gp0_xgip,
#elif defined(ADUCM_PLATFORM)
		.platform_ops = &aducm_gpio_ops,
#endif
	};

	struct ad5940_init_param ad5940_ip = {
		.spi_init = sip,
		.reset_gpio_init = reset_gip,
		.gp0_gpio_init = gp0_gip,
	};
#ifndef IIO_SUPPORT
#if defined(STM32_PLATFORM)
	EXTI_ConfigTypeDef ad5940_int_exticonfig = {
		.Line = EXTI_LINE_7,
		.Trigger = EXTI_TRIGGER_FALLING,
		.GPIOSel = EXTI_GPIOG,
		.Mode = EXTI_MODE_INTERRUPT,
	};
	EXTI_HandleTypeDef ad5940_int_hexti;
	ret = HAL_EXTI_SetConfigLine(&ad5940_int_hexti, &ad5940_int_exticonfig);
	if (ret != HAL_OK)
		return ret;
#endif
	struct no_os_callback_desc ad5940_int_cb = {
		.callback = ad5940_int_callback,
		.ctx = NULL,
		.handle = NULL,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
	};

	struct no_os_irq_init_param ad5940_int_ip = {
#if defined(STM32_PLATFORM)
		.platform_ops = &stm32_irq_ops,
		.extra = &ad5940_int_hexti,
#elif defined(ADUCM_PLATFORM)
		.irq_ctrl_id = ADUCM_GPIO_B_GROUP_SOFT_CTRL,
		.platform_ops = &aducm_gpio_irq_ops,
#endif
	};

#if defined(ADUCM_PLATFORM)
	struct no_os_gpio_desc *gp0;
	ret = no_os_gpio_get(&gp0, &gp0_gip);
	if (ret < 0)
		return ret;
#endif

	struct no_os_irq_ctrl_desc *ad5940_int;
	ret = no_os_irq_ctrl_init(&ad5940_int, &ad5940_int_ip);
	if (ret < 0)
		return ret;

	ret = no_os_irq_register_callback(ad5940_int, INT_IRQn, &ad5940_int_cb);
	if (ret < 0)
		return ret;

	ret = no_os_irq_trigger_level_set(ad5940_int, INT_IRQn, NO_OS_IRQ_EDGE_FALLING);
	if (ret < 0)
		return ret;

	ret = no_os_irq_enable(ad5940_int, INT_IRQn);
	if (ret < 0)
		return ret;

	ret = app_main(i2c, &ad5940_ip);
	if (ret < 0)
		goto error;
#else
	struct ad5940_iio_dev *ad5940_iio = NULL;
	struct ad5940_iio_init_param ad5940_iio_ip = {
		.ad5940_init = &ad5940_ip,
	};
	ret = ad5940_iio_init(&ad5940_iio, &ad5940_iio_ip);
	if (ret < 0)
		goto error;

	struct adg2128_iio_dev *adg2128_iio = NULL;
	ret = adg2128_iio_init(&adg2128_iio, i2c);
	if (ret < 0)
		goto error;

	struct iio_app_device devices[] = {
		{
			.name = "ad5940",
			.dev = ad5940_iio,
			.dev_descriptor = ad5940_iio->iio,
			.read_buff = NULL,
			.write_buff = NULL
		},
		{
			.name = "adg2128",
			.dev = adg2128_iio,
			.dev_descriptor = adg2128_iio->iio_dev,
		},
	};

	return iio_app_run(devices, NO_OS_ARRAY_SIZE(devices));
#endif

	printf("Bye!\n");
	return 0;
error:
	printf("Bye! (%d)\n", ret);
	return ret;
}

