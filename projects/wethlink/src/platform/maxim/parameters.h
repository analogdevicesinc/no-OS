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

#ifndef HW_VERSION
// 0 is for Revision A, 1 is for Revision B etc.
// Use the highest available revision as default.
#define HW_VERSION	1
#endif

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200
#define UART_STOPBITS   NO_OS_UART_STOP_2_BIT
#define UART_IRQ_ID     UART0_IRQn
#define INTC_DEVICE_ID  0

#define XCVR_RESET_PORT		2
#define XCVR_RESET_PIN		0

#define XCVR_EN_PORT		2
#define XCVR_EN_PIN		1

#define XCVR_CLK_PORT		2
#define XCVR_CLK_PIN		3

#define XCVR_DATA_PORT		2
#define XCVR_DATA_PIN		2

#define XCVR_SCANOUT_TX_PORT	2
#define XCVR_SCANOUT_TX_PIN	4

#define XCVR_SCANOUT_RX_PORT	2
#define XCVR_SCANOUT_RX_PIN 	5

#define ADIN1300_RESET_PORT	1
#define ADIN1300_RESET_PIN	23

#define MAX24287_RESET_PORT	1
#define MAX24287_RESET_PIN	24

#define ADIN1300_MDIO_PORT	1
#define ADIN1300_MDIO_PIN	4

#define ADIN1300_MDC_PORT	1
#define ADIN1300_MDC_PIN	5

#define ADIN1300_INT_PORT	2
#define ADIN1300_INT_PIN	16
#define ADIN1300_INT_IRQn	GPIO2_IRQn

#define MAX24287_MDIO_PORT	1
#define MAX24287_MDIO_PIN	6

#define MAX24287_MDC_PORT	1
#define MAX24287_MDC_PIN	7

#define MAX24287_INT_PORT	1
#define MAX24287_INT_PIN	0
#define MAX24287_INT_IRQn	GPIO1_IRQn

#define BRD_SELECT_PORT		0
#define BRD_SELECT_PIN		20

#define FACTORY_DEFAULTS_PORT	3
#define FACTORY_DEFAULTS_PIN	9

#define LED_TX_DET_RED_PORT	0
#define LED_TX_DET_RED_PIN	4
#define LED_TX_DET_GREEN_PORT	0
#define LED_TX_DET_GREEN_PIN	5
#define LED_RX_DET_PORT		3
#define LED_RX_DET_PIN		4
#define LED_TX_LOCK_PORT	0
#define LED_TX_LOCK_PIN		6
#define LED_RX_LOCK_PORT	0
#define LED_RX_LOCK_PIN		7

extern struct no_os_uart_init_param uart_console_ip;
extern struct no_os_gpio_init_param xcvr_reset_gpio_ip;
extern struct no_os_gpio_init_param xcvr_en_gpio_ip;
extern struct no_os_gpio_init_param xcvr_clk_gpio_ip;
extern struct no_os_gpio_init_param xcvr_data_gpio_ip;
extern struct no_os_gpio_init_param xcvr_scanout_tx_gpio_ip;
extern struct no_os_gpio_init_param xcvr_scanout_rx_gpio_ip;
extern struct no_os_gpio_init_param adin1300_reset_gpio_ip;
extern struct no_os_gpio_init_param max24287_reset_gpio_ip;
extern struct no_os_gpio_init_param adin1300_mdc_gpio_ip;
extern struct no_os_gpio_init_param adin1300_mdio_gpio_ip;
extern struct no_os_gpio_init_param adin1300_int_gpio_ip;
extern struct no_os_gpio_init_param max24287_mdc_gpio_ip;
extern struct no_os_gpio_init_param max24287_mdio_gpio_ip;
extern struct no_os_gpio_init_param max24287_int_gpio_ip;
extern struct no_os_gpio_init_param brd_select_gpio_ip;
extern struct no_os_gpio_init_param factory_defaults_gpio_ip;
extern struct no_os_gpio_init_param led_tx_det_red_gpio_ip;
extern struct no_os_gpio_init_param led_tx_det_green_gpio_ip;
extern struct no_os_gpio_init_param led_rx_det_gpio_ip;
extern struct no_os_gpio_init_param led_tx_lock_gpio_ip;
extern struct no_os_gpio_init_param led_rx_lock_gpio_ip;
extern struct no_os_eeprom_init_param eeprom_ip;

#endif /* __PARAMETERS_H__ */
