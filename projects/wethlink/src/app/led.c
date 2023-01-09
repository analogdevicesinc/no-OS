#include "no_os_gpio.h"
#include "parameters.h"

#if (TARGET_NUM == 32650)

struct no_os_gpio_desc *tx_red;
struct no_os_gpio_desc *tx_green;
struct no_os_gpio_desc *rx_n;
struct no_os_gpio_desc *tx_lock;
struct no_os_gpio_desc *rx_lock;

int led_init(void)
{
	int ret;

	ret = no_os_gpio_get(&tx_lock, &led_tx_lock_gpio_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_get(&rx_lock, &led_rx_lock_gpio_ip);
	if (ret)
		return ret;

	// Turn LED's off
	no_os_gpio_direction_output(tx_lock, NO_OS_GPIO_LOW);
	no_os_gpio_direction_output(rx_lock, NO_OS_GPIO_LOW);

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

#endif
