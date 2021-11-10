#ifndef UART_H_
#define UART_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <uart.h>

#define BUART_BUFFER_SIZE 0x100;

struct buart_init_param {
	struct uart_init_param	uart_par;
	/* Should be initialized outside in order to fill uart_irq_conf */
	struct uart_desc	*uart_desc;
	struct irq_ctrl_desc	*irq_desc;
	uint32_t		uart_irq_id;
	void			*uart_irq_conf;
}

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Write data to UART. Blocking function */
int32_t buart_write(struct bu_desc *desc, const uint8_t *data,
		   uint32_t bytes_number);

/* Read data from UART. Non blocking function */
int32_t buart_read_nonblocking(struct bu_desc *desc, uint8_t *data,
			      uint32_t bytes_number);

/* Initialize the UART communication peripheral. */
int32_t buart_cfg(struct bu_desc *desc, struct buart_init_param *param);

/* Free the resources allocated by uart_init(). */
int32_t buart_remove(struct bu_desc *desc);

#endif /* UART_H_ */
