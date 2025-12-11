
#ifndef _STM32_CAPI_UART_H_
#define _STM32_CAPI_UART_H_

#include "stm32_hal.h"

struct stm32_uart_extra_config {
	UART_HandleTypeDef *huart;
};

struct stm32_uart_priv_handle {
	UART_HandleTypeDef *huart;
};

extern struct capi_uart_ops stm32_capi_uart_ops;

int stm32_uart_stdio_enable(struct capi_uart_handle *handle);

#endif
