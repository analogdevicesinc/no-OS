#include "parameters.h"

struct max_uart_init_param adiol100_uart_extra = {
    .flow = MAX_UART_FLOW_DIS,
};

struct max_spi_init_param adiol100_spi_extra = {
    .num_slaves = 2,
    .polarity   = SPI_SS_POL_LOW,
    .vssel      = MXC_GPIO_VSSEL_VDDIOH,
};
