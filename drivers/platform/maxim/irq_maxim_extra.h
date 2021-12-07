#ifndef IRQ_MAXIM_EXTRA_H
#define IRQ_MAXIM_EXTRA_H


#include "irq.h"
#include "max32660.h"

enum irq_id{
	MAX_GPIO_INT_ID = GPIO0_IRQn,
	MAX_UART0_INT_ID = UART0_IRQn,
	MAX_UART1_INT_ID = UART1_IRQn,
	MAX_SPI_INT_ID = SPI17Y_IRQn,
	MAX_RTC_INT_ID = RTC_IRQn
};

struct maxim_irq_ctrl_desc{
	bool callback_configured[MXC_IRQ_COUNT];
};

#endif
