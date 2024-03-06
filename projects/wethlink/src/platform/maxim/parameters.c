#include "parameters.h"

struct max_uart_init_param uart_extra_ip = {
	.flow = UART_FLOW_DIS
};

struct no_os_uart_init_param uart_console_ip = {
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = UART_STOPBITS,
	.platform_ops = &max_uart_ops,
	.extra = &uart_extra_ip,
};

struct max_gpio_init_param xgpio = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_gpio_init_param xgpio_1p8 = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct no_os_gpio_init_param xcvr_reset_gpio_ip = {
	.port = XCVR_RESET_PORT,
	.number = XCVR_RESET_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param xcvr_en_gpio_ip = {
	.port = XCVR_EN_PORT,
	.number = XCVR_EN_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param xcvr_clk_gpio_ip = {
	.port = XCVR_CLK_PORT,
	.number = XCVR_CLK_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param xcvr_data_gpio_ip = {
	.port = XCVR_DATA_PORT,
	.number = XCVR_DATA_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param xcvr_scanout_tx_gpio_ip = {
	.port = XCVR_SCANOUT_TX_PORT,
	.number = XCVR_SCANOUT_TX_PIN,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param xcvr_scanout_rx_gpio_ip = {
	.port = XCVR_SCANOUT_RX_PORT,
	.number = XCVR_SCANOUT_RX_PIN,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param adin1300_reset_gpio_ip = {
	.port = ADIN1300_RESET_PORT,
	.number = ADIN1300_RESET_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param max24287_reset_gpio_ip = {
	.port = MAX24287_RESET_PORT,
	.number = MAX24287_RESET_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param adin1300_mdc_gpio_ip = {
	.port = ADIN1300_MDC_PORT,
	.number = ADIN1300_MDC_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param adin1300_mdio_gpio_ip = {
	.port = ADIN1300_MDIO_PORT,
	.number = ADIN1300_MDIO_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param adin1300_int_gpio_ip = {
	.port = ADIN1300_INT_PORT,
	.number = ADIN1300_INT_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param max24287_mdc_gpio_ip = {
	.port = MAX24287_MDC_PORT,
	.number = MAX24287_MDC_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param max24287_mdio_gpio_ip = {
	.port = MAX24287_MDIO_PORT,
	.number = MAX24287_MDIO_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param max24287_int_gpio_ip = {
	.port = MAX24287_INT_PORT,
	.number = MAX24287_INT_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param brd_select_gpio_ip = {
	.port = BRD_SELECT_PORT,
	.number = BRD_SELECT_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio_1p8,
};

struct no_os_gpio_init_param factory_defaults_gpio_ip = {
	.port = FACTORY_DEFAULTS_PORT,
	.number = FACTORY_DEFAULTS_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param led_tx_lock_gpio_ip = {
	.port = LED_TX_LOCK_PORT,
	.number = LED_TX_LOCK_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param led_rx_lock_gpio_ip = {
	.port = LED_RX_LOCK_PORT,
	.number = LED_RX_LOCK_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param led_tx_det_green_gpio_ip = {
	.port = LED_TX_DET_GREEN_PORT,
	.number = LED_TX_DET_GREEN_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param led_tx_det_red_gpio_ip = {
	.port = LED_TX_DET_RED_PORT,
	.number = LED_TX_DET_RED_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param led_rx_det_red_gpio_ip = {
	.port = LED_RX_DET_RED_PORT,
	.number = LED_RX_DET_RED_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param led_rx_det_green_gpio_ip = {
	.port = LED_RX_DET_GREEN_PORT,
	.number = LED_RX_DET_GREEN_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param led_rj45s11_ip = {
	.port = LED_RJ45S11_PORT,
	.number = LED_RJ45S11_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param led_rj45s12_ip = {
	.port = LED_RJ45S12_PORT,
	.number = LED_RJ45S12_PIN,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_i2c_init_param eeprom_i2c_ip = {
	.device_id = 0,
	.max_speed_hz = 400000,
	.platform_ops = &max_i2c_ops,
	.slave_address = 0x54,
	.extra = &(struct max_i2c_init_param)
	{
		.vssel = MXC_GPIO_VSSEL_VDDIOH
	}
};

struct no_os_eeprom_init_param eeprom_ip = {
	.device_id = 0x54,
	.platform_ops = &eeprom_24xx32a_ops,
	.extra = &(struct eeprom_24xx32a_init_param)
	{
		.i2c_init = &eeprom_i2c_ip
	}
};