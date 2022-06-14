#ifndef _STM32_HAL_H
#define _STM32_HAL_H

#include "main.h"

int stm32_init(void);
int stm32_get_exti_irq_id_from_pin(uint8_t pin_nb, IRQn_Type *irq_id);

#endif
