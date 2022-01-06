#ifndef GPIO_EXTRA_H_
#define GPIO_EXTRA_H_

#include "stdbool.h"
#include <stdint.h>
#include "no-os/irq.h"

#define N_PINS	14
#define N_PORTS	1

struct gpio_irq_config {
	struct gpio_desc *desc;
	enum irq_trig_level mode;
};

int32_t gpio_register_callback(struct irq_ctrl_desc *, struct callback_desc *);
int32_t gpio_unregister_callback(uint8_t);
int32_t gpio_irq_set_trigger_level(struct gpio_desc *, enum irq_trig_level);

#endif

