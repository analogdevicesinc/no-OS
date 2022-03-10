/*****************************************************************************
 * blinky_example.c
 *****************************************************************************/

#include <sys/platform.h>
#include "adi_initialize.h"
#include <drivers/pwr/adi_pwr.h>
#include "no_os_gpio.h"
#include "aducm3029_gpio.h"
#include "no_os_delay.h"

int initPower()
{
	if (ADI_PWR_SUCCESS != adi_pwr_Init())
		return 0;

	if (ADI_PWR_SUCCESS != adi_pwr_SetClockDivider(ADI_CLOCK_HCLK, 1u))
		return 0;

	if (ADI_PWR_SUCCESS != adi_pwr_SetClockDivider(ADI_CLOCK_PCLK, 1u))
		return 0;

	return 1;
}

void toggle(struct no_os_gpio_desc *led)
{
	uint8_t value;
	no_os_gpio_get_value(led, &value);
	no_os_gpio_set_value(led, value == NO_OS_GPIO_HIGH ? NO_OS_GPIO_LOW :
			     NO_OS_GPIO_HIGH);
}

int main(int argc, char *argv[])
{
	/**
	 * Initialize managed drivers and/or services that have been added to
	 * the project.
	 * @return zero on success
	 */
	adi_initComponents();
	/* Begin adding your custom code here */
	if (!initPower())
		return 1;

	struct no_os_gpio_init_param init_blue = {
		.number = 31,
		.platform_ops = &aducm_gpio_ops,
		.extra = NULL
	};
	struct no_os_gpio_init_param init_green = {
		.number = 32,
		.platform_ops = &aducm_gpio_ops,
		.extra = NULL
	};
	struct no_os_gpio_desc *blue;
	struct no_os_gpio_desc *green;

	no_os_gpio_get(&blue, &init_blue);
	no_os_gpio_get(&green, &init_green);

	no_os_gpio_direction_output(blue, NO_OS_GPIO_LOW);
	no_os_gpio_direction_output(green, NO_OS_GPIO_LOW);

	while (1) {
		toggle(blue);
		toggle(green);
		no_os_mdelay(1000);
		toggle(green);
		no_os_mdelay(1000);

	}

	return 0;
}
