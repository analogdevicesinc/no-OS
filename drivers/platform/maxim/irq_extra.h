#ifndef IRQ_MAXIM_EXTRA_H
#define IRQ_MAXIM_EXTRA_H

#include "no-os/irq.h"
#include "gpio.h"
#include "max32660.h"

#define BASE_IRQ	(0x40)

enum irq_id {
	MAX_GPIO_INT_ID = GPIO0_IRQn,
	MAX_UART0_INT_ID = UART0_IRQn,
	MAX_UART1_INT_ID = UART1_IRQn,
	MAX_RTC_INT_ID = RTC_IRQn,
};

extern const struct irq_platform_ops irq_ops;

#endif

