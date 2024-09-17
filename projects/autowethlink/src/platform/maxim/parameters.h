#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "no_os_uart.h"
#include "no_os_gpio.h"
#include "no_os_i2c.h"
#include "no_os_eeprom.h"
#include "maxim_irq.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_i2c.h"
#include "24xx32a.h"

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   345600
#define UART_STOPBITS   NO_OS_UART_STOP_1_BIT
#define UART_IRQ_ID     UART0_IRQn
#define INTC_DEVICE_ID  0

#define XCVR_RESET_PORT		2
#define XCVR_RESET_PIN		0

#define XCVR_EN_PORT		2
#define XCVR_EN_PIN		6

#define XCVR_CLK_PORT		2
#define XCVR_CLK_PIN		3

#define XCVR_DATA_PORT		2
#define XCVR_DATA_PIN		2

#define XCVR_SCANOUT_TX_PORT	2
#define XCVR_SCANOUT_TX_PIN	4

#define XCVR_SCANOUT_RX_PORT	2
#define XCVR_SCANOUT_RX_PIN 	5

#define DP83TG_RESET_PORT	1
#define DP83TG_RESET_PIN	23

#define DP83TG_MDIO_PORT	1
#define DP83TG_MDIO_PIN	25

#define DP83TG_MDC_PORT	1
#define DP83TG_MDC_PIN	26

#define DP83TG_INT_PORT	1
#define DP83TG_INT_PIN	2
#define DP83TG_INT_IRQn	GPIO1_IRQn

#define DP83TG_LINK_PORT	1
#define DP83TG_LINK_PIN		2

#define BRD_SELECT_PORT		0
#define BRD_SELECT_PIN		20

#define FACTORY_DEFAULTS_PORT	3
#define FACTORY_DEFAULTS_PIN	9

extern struct no_os_uart_init_param uart_console_ip; // ok
extern struct no_os_gpio_init_param xcvr_reset_gpio_ip; // ok
extern struct no_os_gpio_init_param xcvr_en_gpio_ip; // ok
extern struct no_os_gpio_init_param xcvr_clk_gpio_ip; // ok
extern struct no_os_gpio_init_param xcvr_data_gpio_ip; // ok
extern struct no_os_gpio_init_param xcvr_scanout_tx_gpio_ip; // ok
extern struct no_os_gpio_init_param xcvr_scanout_rx_gpio_ip; // ok
extern struct no_os_gpio_init_param dp83tg_reset_gpio_ip; // ok
extern struct no_os_gpio_init_param dp83tg_mdc_gpio_ip; // ok
extern struct no_os_gpio_init_param dp83tg_mdio_gpio_ip; // ok
extern struct no_os_gpio_init_param dp83tg_int_gpio_ip; // ok
extern struct no_os_gpio_init_param dp83tg_link_gpio_ip; // ok
extern struct no_os_gpio_init_param brd_select_gpio_ip; // ok
extern struct no_os_gpio_init_param factory_defaults_gpio_ip;
extern struct no_os_eeprom_init_param eeprom_ip; // ok

#endif /* __PARAMETERS_H__ */
