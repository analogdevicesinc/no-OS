#include "common_data.h"

struct max_uart_init_param max_uart_extra_ip = {
    .flow = UART_FLOW_DIS
};

struct no_os_uart_init_param uart_ip = {
    .device_id = UART_DEVICE_ID,
    .baud_rate = UART_BAUDRATE,
    .irq_id = UART_IRQ_ID,
    .asynchronous_rx = true,
    .size = NO_OS_UART_CS_8,
    .parity = NO_OS_UART_PAR_NO,
    .stop = NO_OS_UART_STOP_1_BIT,
    .extra = &max_uart_extra_ip,
    .platform_ops = UART_OPS,
};

struct max_spi_init_param adxl355_spi_extra_ip  = {
    .num_slaves = 1,
    .polarity = SPI_SS_POL_LOW,
    .vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct no_os_spi_init_param adxl355_spi_ip = {
    .device_id = 1,
    .max_speed_hz = 15000000, 
    .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
    .mode = NO_OS_SPI_MODE_0,
    .platform_ops = &max_spi_ops,
    .chip_select = 1,
    .extra = &adxl355_spi_extra_ip,
};

struct adxl355_init_param adxl355_ip= {
    .comm_type = ADXL355_SPI_COMM,
    .dev_type = ID_ADXL355,
};

// struct adxl355_init_param adxl355_ip = {
//     .comm_type = ADXL355_SPI_COMM,
// #ifdef ADXL359_DEV
//     .dev_type = ID_ADXL359,
// #elif defined ADXL357_DEV
//     .dev_type = ID_ADXL357,
// #else
//     .dev_type = ID_ADXL355,
// #endif
// };