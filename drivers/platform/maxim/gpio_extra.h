#ifndef GPIO_EXTRA_H_
#define GPIO_EXTRA_H_

#include <stdint.h>

#define N_PINS	32
#define N_PORTS	5

int32_t gpio_register_callback(uint8_t, struct callback_desc *);
int32_t gpio_unregister_callback(uint8_t);

#endif

