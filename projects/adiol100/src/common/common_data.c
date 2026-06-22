#include "common_data.h"

struct no_os_uart_init_param adiol100_uart_ip = {
    .device_id    = UART_DEVICE_ID,
    .baud_rate    = UART_BAUDRATE,
    .size         = NO_OS_UART_CS_8,
    .parity       = NO_OS_UART_PAR_NO,
    .stop         = NO_OS_UART_STOP_1_BIT,
    .extra        = UART_EXTRA,
    .platform_ops = UART_OPS,
};

struct no_os_spi_init_param adiol100_spi_ip = {
    .device_id    = SPI_DEVICE_ID,
    .max_speed_hz = SPI_BAUDRATE,
    .bit_order    = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
    .mode         = NO_OS_SPI_MODE_0,
    .platform_ops = SPI_OPS,
    .chip_select  = SPI_CS,
    .extra        = SPI_EXTRA,
};

struct adiol100_init_param adiol100_ip = {
    .spi_ip    = &adiol100_spi_ip,
    .chip_addr = 0x01,   /* ADRSEL=HIGH on FPGA board */
};
