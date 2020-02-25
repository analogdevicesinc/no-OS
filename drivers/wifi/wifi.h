#ifndef WIFI_H
# define WIFI_H

#include <stdint.h>
#include <stdbool.h>

struct wifi_desc {
	uint8_t			buff[1000];
	struct uart_desc	*uart;
	struct irq_ctrl_desc	*irq;
	uint32_t		uart_irq_id;
	bool			read_done;
	bool			write_done;
	bool			wait_full_read;
};
struct wifi_init_param {
	struct uart_desc	*uart;
	struct irq_ctrl_desc	*irq;
	uint32_t		uart_irq_id;
};

uint32_t wifi_init(struct wifi_desc **desc,
		   const struct wifi_init_param *param);

uint32_t wifi_write(struct wifi_desc *desc, uint8_t *buff, uint32_t len);

uint32_t wifi_read(struct wifi_desc *desc, uint8_t *buff, uint32_t len);

#endif
