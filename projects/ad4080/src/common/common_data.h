#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"

extern struct no_os_gpio_init_param afe_ctrl_init_param;
extern struct no_os_gpio_init_param gate_ctrl_q4_init_param;
extern struct no_os_gpio_init_param gate_ctrl_q3_init_param;
extern struct no_os_gpio_init_param gate_ctrl_q2_init_param;
extern struct no_os_gpio_init_param gate_ctrl_q1_init_param;

extern struct no_os_gpio_init_param gp3_init_param;

extern struct no_os_gpio_init_param data_ss_init_param;
extern struct no_os_spi_init_param data_spi_init_param;

extern struct no_os_gpio_init_param cfg_ss_init_param;
extern struct no_os_spi_init_param cfg_spi_init_param;

extern struct no_os_uart_init_param uart_init_param;

extern struct no_os_uart_init_param serial_log_init_param;

#endif /* __COMMON_DATA_H__ */

