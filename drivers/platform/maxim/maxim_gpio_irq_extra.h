#ifndef MAXIM_GPIO_IRQ_EXTRA_H_
#define MAXIM_GPIO_IRQ_EXTRA_H_

#include "stdbool.h"
#include "no-os/irq.h"
#include "no-os/gpio.h"

enum gpio_irq_edge {
	RISING,
	FALLING
};

int32_t gpio_irq_register_callback(struct irq_ctrl_desc *, uint8_t, struct callback_desc *);
int32_t gpio_irq_unregister_callback(uint8_t);

#endif

