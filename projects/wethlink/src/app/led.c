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
struct no_os_gpio_desc *rj45s11;
struct no_os_gpio_desc *rj45s12;

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

	ret = no_os_gpio_get(&rj45s11, &led_rj45s11_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_get(&rj45s12, &led_rj45s12_ip);
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
	no_os_gpio_direction_output(rj45s11, NO_OS_GPIO_LOW);
	no_os_gpio_direction_output(rj45s12, NO_OS_GPIO_LOW);

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

void led_rj45(enum rj45_led state)
{
	no_os_gpio_set_value(rj45s11,
			     state & rj45_led_green ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
	no_os_gpio_set_value(rj45s12,
			     state & rj45_led_yellow ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
}

void led_blink_all(unsigned int times, unsigned int duration_ms)
{
	int i;
	for (i = 0; i < times; i++) {
		led_tx_lock(true);
		led_rx_lock(true);
		led_tx_det_green(true);
		led_rx_det_green(true);
		no_os_mdelay(duration_ms / times / 2);
		led_tx_lock(false);
		led_rx_lock(false);
		led_tx_det_green(false);
		led_rx_det_green(false);
		no_os_mdelay(duration_ms / times / 2);
	}
}

#endif
