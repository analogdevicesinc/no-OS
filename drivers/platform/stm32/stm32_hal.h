/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef STM32_HAL_H_
#define STM32_HAL_H_

#include "main.h"

int stm32_init(void);
int stm32_get_exti_irq_id_from_pin(uint8_t pin_nb, IRQn_Type *irq_id);

#endif
