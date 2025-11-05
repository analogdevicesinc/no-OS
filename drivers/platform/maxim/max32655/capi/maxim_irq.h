#ifndef MAXIM_IRQ_H_
#define MAXIM_IRQ_H_

#include "capi_irq.h"

struct capi_isr_table{
	capi_isr_callback_t isr;
	void *arg;
};

#endif