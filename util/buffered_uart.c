#include <stdlib.h>
#include "circular_buffer.h"
#include "error.h"

struct bu_desc {
	struct uart_desc *uart;
	struct circular_buffer cb[1];
	uint8_t buf[BUART_BUFFER_SIZE];
	struct irq_ctrl_desc *irq_desc;
	uint32_t uart_irq_id;
	uint8_t ch;

};

static void buart_call(struct bu_desc *desc, uint32_t event, uint8_t *data)
{
	if (event == IRQ_READ_DONE)
		cb_write(desc->cb, &desc->ch, 1);

	uart_read_nonblocking(desc->uart, &desc->ch, 1);
}

/* Initialize the UART communication peripheral. */
int32_t buart_cfg(struct bu_desc *desc, struct buart_init_param *param)
{
	int32_t ret;
	uint32_t len;
	struct callback_desc call;

	if (!desc || !param)
		return -EINVAL;

	memset(desc, 0, sizeof(*desc));

	cb_cfg(desc->cb, desc->buf, BUART_BUFFER_SIZE);

	desc->uart = param->uart_desc;
	desc->irq_desc = param->irq_desc;
	desc->uart_irq_id = param->uart_irq_id;

	call.callback = (int (*)())buart_call;
	call.ctx = desc;
	call.config = param->uart_irq_conf;
	ret = irq_register_callback(desc->irq_desc, desc->uart_irq_id, &call);
	if (IS_ERR_VALUE(ret))
		return ret;

	ret = irq_enable(desc->irq_desc, desc->uart_irq_id);
	if (IS_ERR_VALUE(ret))
		return ret;

	return uart_read_nonblocking(desc->uart, &desc->ch, 1);
}


/* Free the resources allocated by uart_init(). */
void buart_remove(struct bu_desc *desc) {
	irq_unregister(desc->irq_desc, desc->uart_irq_id);
	memset(desc, 0, sizeof(*desc));
}

/* Write data to UART. Blocking function */
int32_t buart_write(struct bu_desc *desc, const uint8_t *data,
		   uint32_t bytes_number)
{
	return uart_write(desc->uart, data, bytes_number);
}

/* Read data from UART. Non blocking function */
int32_t buart_read_nonblocking(struct bu_desc *desc, uint8_t *data,
			      uint32_t bytes_number)
{
	uint32_t len;
	int32_t ret;

	cb_size(desc->cb, &len);
	if (!len)
		return 0;

	ret = cb_read(desc->cb, data, len);
	if (IS_ERR_VALUE(ret))
		return ret;

	return len;
}

