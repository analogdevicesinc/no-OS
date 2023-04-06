#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "no_os_uart.h"
#include "no_os_gpio.h"
#include "maxim_irq.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200
#define UART_STOPBITS   NO_OS_UART_STOP_2_BIT
#define UART_IRQ_ID     UART0_IRQn
#define INTC_DEVICE_ID  0

// full-fledged firmware
#if (TARGET_NUM==32650)
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

#define MAX24287_MDIO_PORT	1
#define MAX24287_MDIO_PIN	6

#define MAX24287_MDC_PORT	1
#define MAX24287_MDC_PIN	7

#define BRD_SELECT_PORT		0
#define BRD_SELECT_PIN		20

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

// stripped down firmware that only controls the transceivers
#elif (TARGET_NUM==78000)
#define XCVR_RESET_PORT		0
#define XCVR_RESET_PIN		9

#define XCVR_EN_PORT		0
#define XCVR_EN_PIN		11

#define XCVR_CLK_PORT		0
#define XCVR_CLK_PIN		7

#define XCVR_DATA_PORT		0
#define XCVR_DATA_PIN		5

#define XCVR_SCANOUT_TX_PORT	0
#define XCVR_SCANOUT_TX_PIN	6

#define XCVR_SCANOUT_RX_PORT	0
#define XCVR_SCANOUT_RX_PIN 	8
#endif  

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
extern struct no_os_gpio_init_param max24287_mdc_gpio_ip;
extern struct no_os_gpio_init_param max24287_mdio_gpio_ip;
extern struct no_os_gpio_init_param brd_select_gpio_ip;
extern struct no_os_gpio_init_param led_tx_det_red_gpio_ip;
extern struct no_os_gpio_init_param led_tx_det_green_gpio_ip;
extern struct no_os_gpio_init_param led_rx_det_gpio_ip;
extern struct no_os_gpio_init_param led_tx_lock_gpio_ip;
extern struct no_os_gpio_init_param led_rx_lock_gpio_ip;

#endif /* __PARAMETERS_H__ */
