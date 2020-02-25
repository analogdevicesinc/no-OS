#include "wifi.h"
#include "uart.h"
#include "uart_extra.h"
#include "error.h"
#include "irq.h"
#include "irq_extra.h"
#include <stdlib.h>

void uart_callback(void *ctx, uint32_t event, void *extra)
{
	struct wifi_desc *wifi = ctx;

	switch (event) {
	case READ_DONE:
		if (wifi->wait_full_read) {
			wifi->read_done = true;
			wifi->wait_full_read = false;
		} else if (*wifi->buff == ':') {
			wifi->wait_full_read = true;
			uart_read(wifi->uart, wifi->buff, 100);
			break;
		}
		uart_read(wifi->uart, wifi->buff, 1);
		break;
	case WRITE_DONE:
		wifi->write_done = true;
		break;
	case ERROR:
		break;
	}
}

uint32_t wifi_init(struct wifi_desc **desc,
		   const struct wifi_init_param *param)
{
	struct wifi_desc *wifi;
	struct callback_desc call;
	wifi = calloc(1, sizeof(*wifi));

	wifi->irq = param->irq;
	wifi->uart = param->uart;
	wifi->uart_irq_id = param->uart_irq_id;

	call.callback = uart_callback;
	call.callback_ctx = wifi;
	call.callback_config = wifi->uart;

	irq_register_callback(wifi->irq, wifi->uart_irq_id, &call);
	irq_enable(wifi->irq, wifi->uart_irq_id);

	wifi->read_done = false;
	uart_read(wifi->uart, wifi->buff, 1);

	*desc = wifi;
	return SUCCESS;
}

uint32_t wifi_write(struct wifi_desc *desc, uint8_t *buff, uint32_t len)
{
	desc->write_done = false;
	uart_write(desc->uart, buff, len);
	while (!desc->write_done)
		;
	return SUCCESS;
}

uint32_t wifi_read(struct wifi_desc *desc, uint8_t *buff, uint32_t len)
{
	while (!desc->read_done)
		;

	memcpy(buff, desc->buff, len);

	return SUCCESS;
}

#include <stdio.h>

void dummy_wifi_driver_test()
{

	struct uart_desc	*uart_desc;
	struct irq_ctrl_desc	*desc;

	struct aducm_uart_init_param aducm_param = {
		.parity = UART_NO_PARITY,
		.stop_bits = UART_ONE_STOPBIT,
		.word_length = UART_WORDLEN_8BITS
	};
	struct uart_init_param uart_param = {
		.device_id = 0,
		.baud_rate = BD_115200,
		.extra = &aducm_param
	};

	struct irq_init_param param = {
		.irq_ctrl_id = 0,
		.extra = 0
	};

	uart_init(&uart_desc, &uart_param);
	irq_ctrl_init(&desc, &param);

	struct wifi_desc *wifi;
	struct wifi_init_param wifi_param = {
		.irq = desc,
		.uart = uart_desc,
		.uart_irq_id = ADUCM_UART_INT_ID
	};

	wifi_init(&wifi, &wifi_param);

	wifi_write(wifi,"AT\r\n", 4);

	uint8_t buff[1000];
	wifi_read(wifi, buff, 100);
	buff[100] = 0;
	printf("%s\n", buff);
}
