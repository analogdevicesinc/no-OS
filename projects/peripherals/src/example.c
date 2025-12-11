
#include <stdio.h>

#include "capi_uart.h"
#include "stm32_capi_uart.h"

extern UART_HandleTypeDef huart3;

int main(void)
{
	stm32_init();

	struct stm32_uart_extra_config stm32_uart_extra_init = {
		.huart = &huart3,
	};
	struct capi_uart_line_config stm32_line_config = {
		.baudrate = 115200,
		.size = CAPI_UART_DATA_BITS_8,
		.parity = CAPI_UART_PARITY_NONE,
		.stop_bits = CAPI_UART_STOP_1_BIT,
		.sticky_parity = false,
		.loopback = false,
	};
	struct capi_uart_config	stm32_uart_config = {
		.identifier = 3,
		.dma_handle = NULL,
		.clk_freq_hz = 0,
		.line_config = &stm32_line_config,
		.extra = &stm32_uart_extra_init,
		.ops = &stm32_capi_uart_ops,
	};
	struct capi_uart_handle	*stm32_uart_handle;
	int ret;

	ret = capi_uart_init(&stm32_uart_handle, &stm32_uart_config);
	if (ret)
		return ret;

	ret = capi_uart_transmit(stm32_uart_handle, "Hello\n\r", 7);
	if (ret)
		return ret;

	ret = stm32_uart_stdio_enable(stm32_uart_handle);
	if (ret)
		return ret;

	printf("stdio redirect test\n\r");

	ret = capi_uart_deinit(&stm32_uart_handle);
	if (ret)
		return ret;

	return 0;
}
