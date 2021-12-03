#ifndef IRQ_MAXIM_EXTRA_H
#define IRQ_MAXIM_EXTRA_H

#include "no-os/irq.h"
#include "max32660.h"

#define BASE_IRQ	(0x40)

enum irq_id {
	MAX_GPIO_INT_ID = GPIO0_IRQn,
	MAX_UART0_INT_ID = UART0_IRQn,
	MAX_UART1_INT_ID = UART1_IRQn,
	MAX_RTC_INT_ID = RTC_IRQn,
};

enum gpio_irq_mode {
	POSITIVE_EDGE,
	NEGATIVE_EDGE
};

struct gpio_irq_config {
	gpio_cfg_t		*gpio_cfg; 
	enum gpio_irq_mode	mode;							
};

struct rtc_irq_config {
	uint32_t active_interrupts;
};

union irq_config{
	struct uart_desc	*uart_conf;
	uint32_t		rtc_interrupts;		
};

struct maxim_irq_desc {
	uint8_t		callback_configured[MXC_IRQ_COUNT];
	uint32_t	enabled;
};

#endif

