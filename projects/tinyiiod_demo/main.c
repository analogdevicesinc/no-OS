#include <string.h>
#include <time.h>
#include "tinyiiod.h"
#include "no_os_uart.h"
#include "maxim_uart.h"
#include "no_os_delay.h"
#include <errno.h>
#include <iio/iio.h>

int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
	struct no_os_time now = no_os_get_time();
	tp->tv_sec = now.s;
	tp->tv_nsec = now.us * 1000;

	return 0;
}

int serial_init(struct no_os_uart_desc **huart)
{
	int ret;
	struct max_uart_init_param x = {
		.flow = UART_FLOW_DIS
	};
	struct no_os_uart_init_param ip = {
		.device_id = 0,
		.irq_id = UART0_IRQn,
		//.asynchronous_rx = true,
		.baud_rate = 115200,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &x,
		.platform_ops = &max_uart_ops,
	};

	ret = no_os_uart_init(huart, &ip);
	if (ret)
		return ret;

	no_os_uart_stdio(*huart);

	return 0;
}

ssize_t read_cb(struct iiod_pdata *pdata, void *buf, size_t size)
{
	int ret;

	ret = no_os_uart_read((struct no_os_uart_desc *)pdata, buf, size);
	if (ret == -EAGAIN)
		return 0;
	else if (ret < 0)
		return ret;
	
	return ret;
}

ssize_t write_cb(struct iiod_pdata *pdata, const void *buf, size_t size)
{
	int ret;
	static int i = 1;
	if (i == 2)
		; // for breakpoint
	i++;
	if (i)
	ret = no_os_uart_write((struct no_os_uart_desc *)pdata, buf, size);
	if (ret)
		return ret;
	
	return size;
}

int main()
{
	int ret;
	struct no_os_uart_desc *huart;
	
	ret = serial_init(&huart);
	if (ret)
		return ret;

	struct iio_context_params context_params;
	struct iio_context *ctx = iio_create_context(&context_params, "baremetal:");

	const char * ctxxml = iio_context_get_xml(ctx);
	ssize_t ctxlen = strlen(ctxxml);

	ret = iiod_interpreter(ctx, (struct iiod_pdata *)huart, read_cb, write_cb, ctxxml, ctxlen);

	return 0;
}
