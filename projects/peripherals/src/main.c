
#include <stdio.h>
#include "no_os_uart.h"
#include "no_os_pwm.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_pwm.h"

int main(void)
{
	struct max_uart_init_param max_uart_extra_ip = {
		.flow = UART_FLOW_DIS
	};
	struct no_os_uart_init_param uart_ip = {
		.device_id = 0,
		.baud_rate = 57600,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = &max_uart_ops,
		.extra = &max_uart_extra_ip,
	};
	struct max_pwm_init_param max_pwm_extra_ip = {
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};
	struct no_os_pwm_init_param pwm_ip= {
		.id = 3,
		.period_ns = 1000000,
		.duty_cycle_ns = 100000,
		.polarity = NO_OS_PWM_POLARITY_LOW,
		.platform_ops = &max_pwm_ops,
		.extra = &max_pwm_extra_ip,
	};
	struct no_os_uart_desc *uart_desc;
	struct no_os_pwm_desc *pwm_desc;
	int ret;

	printf("Hello\n\r");

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret < 0)
		return ret;
	
	no_os_uart_stdio(uart_desc);

	ret = no_os_pwm_init(&pwm_desc, &pwm_ip);
	if (ret < 0)
		goto remove_uart;

	printf("Bye\n\r");

	no_os_pwm_remove(pwm_desc);
remove_uart:
	no_os_uart_remove(uart_desc);

	return ret;
}
