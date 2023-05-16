#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"
#include "chprintf.h"
#include <stdio.h>
#include "hal.h"
#include "ch.h"

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef IIO_TRIGGER_EXAMPLE
#include "iio_trigger_example.h"
#endif

#ifdef DUMMY_EXAMPLE
#include "dummy_example.h"
#endif

static THD_WORKING_AREA(waThreadBlinker, 256);

static THD_FUNCTION(ThreadBlinker, arg)
{
	(void)arg;
	chRegSetThreadName("blinker");
	while (true) {

		palToggleLine(LINE_LED_GREEN);
		chThdSleepMilliseconds(200);
	}
}

int main(void)
{
	int ret = -EINVAL;
	adxl355_ip.comm_init.spi_init = adxl355_spi_ip;

	halInit();
	chSysInit();

	chThdCreateStatic(waThreadBlinker, sizeof(waThreadBlinker), NORMALPRIO + 1,
			  ThreadBlinker, NULL);
	ret = -EINVAL;
	adxl355_ip.comm_init.spi_init = adxl355_spi_ip;

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
#endif

#ifdef IIO_TRIGGER_EXAMPLE
	ret = iio_trigger_example_main();
#endif

#ifdef DUMMY_EXAMPLE
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &adxl355_uart_ip);
	if (ret)
		return ret;

	ret = dummy_example_main();
#endif

#if (DUMMY_EXAMPLE + IIO_EXAMPLE + IIO_TRIGGER_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (DUMMY_EXAMPLE + IIO_EXAMPLE + IIO_TRIGGER_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and re-build the project.
#endif
	return ret;
}
