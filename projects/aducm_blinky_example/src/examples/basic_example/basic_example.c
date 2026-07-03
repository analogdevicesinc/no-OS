/*****************************************************************************
 * basic_example.c
 * Blinky basic example for aducm_blinky_example project.
 *****************************************************************************/

#include "common_data.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"

/*****************************************************************************
 * @brief Toggle a GPIO-connected LED.
 *
 * @param led - GPIO descriptor of the LED to toggle.
 *****************************************************************************/
static void toggle(struct no_os_gpio_desc *led)
{
	uint8_t value;
	no_os_gpio_get_value(led, &value);
	no_os_gpio_set_value(led, value == NO_OS_GPIO_HIGH ? NO_OS_GPIO_LOW :
			     NO_OS_GPIO_HIGH);
}

/*****************************************************************************
 * @brief Blinky basic example main execution.
 *
 * @return ret - Result of the example execution. Does not return under
 *               normal operation.
 *****************************************************************************/
int example_main()
{
	int ret;
	struct no_os_gpio_desc *blue;
	struct no_os_gpio_desc *green;

	ret = no_os_gpio_get(&blue, &blinky_gpio_blue_ip);
	if (ret)
		return ret;
	ret = no_os_gpio_get(&green, &blinky_gpio_green_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_output(blue, NO_OS_GPIO_LOW);
	if (ret)
		return ret;
	ret = no_os_gpio_direction_output(green, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	while (1) {
		toggle(blue);
		toggle(green);
		no_os_mdelay(1000);
		toggle(green);
		no_os_mdelay(1000);
	}

	return 0;
}
