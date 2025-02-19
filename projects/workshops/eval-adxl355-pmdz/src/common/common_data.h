#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "adxl355.h"
#include "parameters.h"

extern struct no_os_uart_init_param uart_ip;
extern struct no_os_spi_init_param adxl355_spi_ip;
extern struct adxl355_init_param adxl355_ip;
extern struct max_uart_init_param max_uart_extra_ip;
extern struct max_spi_init_param adxl355_spi_extra_ip;

#endif