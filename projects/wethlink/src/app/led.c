#include <stdio.h>
#include "no_os_gpio.h"
#include "parameters.h"
#include "led.h"

#if (TARGET_NUM == 32650)
struct no_os_gpio_desc *tx_det_red;
struct no_os_gpio_desc *tx_det_green;
struct no_os_gpio_desc *tx_lock;
struct no_os_gpio_desc *rx_lock;
struct no_os_gpio_desc *rx_det_green;
struct no_os_gpio_desc *rx_det_red;

int led_init(void)
{
	int ret;

	ret = no_os_gpio_get(&tx_lock, &led_tx_lock_gpio_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_get(&rx_lock, &led_rx_lock_gpio_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_get(&tx_det_red, &led_tx_det_red_gpio_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_get(&tx_det_green, &led_tx_det_green_gpio_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_get(&rx_det_red, &led_rx_det_red_gpio_ip);
	if (ret)
		return ret;
	
	ret = no_os_gpio_get(&rx_det_green, &led_rx_det_green_gpio_ip);
	if (ret)
		return ret;

	// Turn LED's off
	no_os_gpio_direction_output(tx_lock, NO_OS_GPIO_LOW);
	no_os_gpio_direction_output(rx_lock, NO_OS_GPIO_LOW);
	no_os_gpio_direction_output(tx_det_red, NO_OS_GPIO_LOW);
	no_os_gpio_direction_output(tx_det_green, NO_OS_GPIO_LOW);
	no_os_gpio_direction_output(rx_det_red, NO_OS_GPIO_LOW);
#if (HW_VERSION == 0)
	no_os_gpio_direction_output(rx_det_green, NO_OS_GPIO_HIGH);
#else
	no_os_gpio_direction_output(rx_det_green, NO_OS_GPIO_LOW);
#endif

	return 0;
}

void led_tx_lock(bool on)
{
	no_os_gpio_set_value(tx_lock, on ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
}

void led_rx_lock(bool on)
{
	no_os_gpio_set_value(rx_lock, on ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
}

void led_tx_det_green(bool on)
{
	no_os_gpio_set_value(tx_det_green, on ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
}

void led_tx_det_red(bool on)
{
	no_os_gpio_set_value(tx_det_red, on ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
}

void led_rx_det_green(bool on)
{
#if (HW_VERSION == 0)
	on = !on;
#endif
	no_os_gpio_set_value(rx_det_green, on ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
}

void led_rx_det_red(bool on)
{
#if (HW_VERSION == 0)
	on = !on;
#endif
	no_os_gpio_set_value(rx_det_red, on ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
}

#endif
