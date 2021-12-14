#include <stdlib.h>
#include <stdio.h>
#include "parameters.h"
#include "no-os/uart.h"
#include "no-os/gpio.h"
#include "no-os/i2c.h"
#include "no-os/spi.h"
#include "app.h"

#include <errno.h>
#include "stm32_uart.h"
#include "stm32_i2c.h"
#include "stm32_uart_stdio.h"

struct spi_desc *spi;
struct i2c_desc *i2c;
struct uart_desc *uart;

int main(void)
{
	int ret;

	HAL_Init();
	SystemClock_Config();

	struct stm32_uart_init_param xuip = {
		.mode = UART_MODE_TX_RX,
		.timeout = 10,
	};
	struct uart_init_param uip = {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = UART_CS_8,
		.parity = UART_PAR_NO,
		.stop = UART_STOP_1_BIT,
		.extra = &xuip,
	};

	ret = uart_init(&uart, &uip);
	if (ret < 0)
		goto error;

	stm32_uart_stdio(uart);
	printf("Hello!\n");

	struct i2c_init_param i2cip = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = 100000,
		.slave_address = 0x70,
		.platform_ops = &stm32_i2c_ops,
	};
	ret = i2c_init(&i2c, &i2cip);
	if (ret)
		goto error;

	/*Configure GPIO interrupt pin : AD5940_INT_Pin */
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = AD5940_INT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(AD5940_INT_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	ret = app_main(i2c);
	if (ret < 0)
		goto error;

	printf("Bye!\n");
	return 0;
error:
	printf("Bye! (%d)\n", ret);
	return ret;
}

void EXTI9_5_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
	ucInterrupted = 1;
}
