#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include "no-os/irq.h"
#include "irq_maxim_extra.h"
#include "nvic_table.h"
#include "gpio.h"

static struct callback_desc *irq_callback_desc;
const struct irq_platform_ops maxim_irq_ops;

int32_t irq_ctrl_init(struct irq_ctrl_desc **desc,
		      const struct irq_init_param *param)
{
	if(!param)
		return -EINVAL;

	struct irq_ctrl_desc *descriptor = calloc(1, sizeof(*descriptor));
	if(!descriptor)
		return -ENOMEM;

	descriptor->irq_ctrl_id = param->irq_ctrl_id;
	descriptor->platform_ops = &maxim_irq_ops;
	descriptor->extra = (struct maxim_irq_desc)param->extra;

	GPIO_Init();
	*desc = descriptor;	

	return 0;

error:
	free(*desc);
}

int32_t irq_ctrl_remove(struct irq_ctrl_desc *desc)
{
	for(uint32_t i = 0; i < MXC_IRQ_COUNT; i++) {
		NVIC_DisableIRQ(BASE_IRQ + i);
	}

	free(desc);

	return 0;
}

int32_t irq_register_callback(struct irq_ctrl_desc *desc, uint32_t irq_id,
			      struct callback_desc *callback_desc)
{
	if(!desc || irq_id >= MXC_IRQ_COUNT)
		return -EINVAL;

	switch(irq_id){
	case MAX_UART0_INT_ID:
		uart_register_callback(0, callback_desc);
		break;
	case MAX_UART1_INT_ID:
		uart_register_callback(1, callback_desc);
		break;
	case MAX_GPIO_INT_ID:
		gpio_register_callback(0, callback_desc);
		break;
	case MAX_RTC_INT_ID:
		rtc_register_callback(callback_desc);
		break;
	}

	return 0;
}

int32_t irq_unregister(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	if(!desc || irq_id >= MXC_IRQ_COUNT)
		return -EINVAL;

	switch(irq_id){
	case MAX_UART0_INT_ID:
		uart_unregister_callback(0);
		break;
	case MAX_UART1_INT_ID:
		uart_unregister_callback(1);
		break;
	case MAX_GPIO_INT_ID:
		gpio_unregister_callback(0);
		break;
	case MAX_RTC_INT_ID:
		rtc_unregister_callback();
		break;
	}

	return 0;
}

int32_t irq_global_enable(struct irq_ctrl_desc *desc)
{
	if(!desc)
		return -EINVAL;

	for(uint32_t i = 0; i < MXC_IRQ_COUNT; i++) {
		NVIC_EnableIRQ(BASE_IRQ + i);
	}

	return 0;
}

int32_t irq_global_disable(struct irq_ctrl_desc *desc)
{
	if(!desc)
		return -EINVAL;

	for(uint32_t i = 0; i < MXC_IRQ_COUNT; i++) {
		NVIC_DisableIRQ(BASE_IRQ + i);
	}

	return 0;
}

int32_t irq_enable(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	if(!desc || irq_id > MXC_IRQ_COUNT - 1)
		return -EINVAL;

	NVIC_EnableIRQ(BASE_IRQ + irq_id);

	return 0;
}

int32_t irq_disable(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	if(!desc || irq_id > MXC_IRQ_COUNT - 1)
		return -EINVAL;
	
	NVIC_ClearPendingIRQ(BASE_IRQ + irq_id);
	NVIC_DisableIRQ(BASE_IRQ + irq_id);

	return 0;
}

const struct irq_platform_ops maxim_irq_ops = {
	.init = &irq_ctrl_init,
	.register_callback = &irq_register_callback,
	.unregister = &irq_unregister,
	.global_enable = &irq_global_enable,
	.global_disable = &irq_global_disable,
	.enable = &irq_enable,
	.disable = &irq_disable,
	.remove = &irq_ctrl_remove
};

