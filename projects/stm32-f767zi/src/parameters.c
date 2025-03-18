
#include "parameters.h"

struct stm32_uart_init_param stm32_uart_extra_ip = {
    .huart = UART_INSTANCE,
};

struct no_os_uart_init_param uart_ip = {
    .device_id = UART_DEVICE_ID,
    .irq_id = UART_IRQ_ID,
    .asynchronous_rx = true,
    .baud_rate = UART_BAUDRATE,
    .size = NO_OS_UART_CS_8,
    .parity = NO_OS_UART_PAR_NO,
    .stop = NO_OS_UART_STOP_1_BIT,
    .platform_ops = UART_OPS,
    .extra = &stm32_uart_extra_ip,
};
