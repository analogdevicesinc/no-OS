
#include "stdio.h"
#include "linux_timer.h"

#include <time.h>
#include <unistd.h>

int main()
{
	struct no_os_timer_desc *timer_desc;
	struct no_os_timer_init_param timer_init = {
		.id = 0,
		.freq_hz = 0,
		.ticks_count = 0,
		.platform_ops = &linux_timer_ops,
		.extra = NULL,
	};
	uint32_t counter;
	int ret;

	printf("Hello\n\r");

	ret = no_os_timer_init(&timer_desc, &timer_init);
	if (ret)
		return ret;

	no_os_timer_start(timer_desc);

	usleep(500000);

	no_os_timer_counter_get(timer_desc, &counter);

	printf("ms: %d\n\r", counter);

	usleep(1230000);

	no_os_timer_counter_get(timer_desc, &counter);

	printf("ms: %d\n\r", counter);

	usleep(200000);

	no_os_timer_counter_get(timer_desc, &counter);

	printf("ms: %d\n\r", counter);

	no_os_timer_counter_set(timer_desc, 0);

	no_os_timer_counter_get(timer_desc, &counter);
	
	printf("ms: %d\n\r", counter);

	usleep(500000);

	no_os_timer_counter_get(timer_desc, &counter);
	
	printf("ms: %d\n\r", counter);

	printf("Bye\n\r");

	return 0;
}

