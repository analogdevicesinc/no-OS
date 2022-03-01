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

void toggle(struct gpio_desc *led)
{
	uint8_t value;
	gpio_get_value(led, &value);
	gpio_set_value(led, value == GPIO_HIGH ? GPIO_LOW : GPIO_HIGH);
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

	struct gpio_init_param init_blue = {
		.number = 31,
		.platform_ops = &aducm_gpio_ops,
		.extra = NULL
	};
	struct gpio_init_param init_green = {
		.number = 32,
		.platform_ops = &aducm_gpio_ops,
		.extra = NULL
	};
	struct gpio_desc *blue;
	struct gpio_desc *green;

	gpio_get(&blue, &init_blue);
	gpio_get(&green, &init_green);

	gpio_direction_output(blue, GPIO_LOW);
	gpio_direction_output(green, GPIO_LOW);

	while (1) {
		toggle(blue);
		toggle(green);
		mdelay(1000);
		toggle(green);
		mdelay(1000);

	}

	return 0;
}
