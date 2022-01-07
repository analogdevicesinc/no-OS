#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include "no-os/irq.h"
#include "irq_extra.h"
#include "nvic_table.h"
#include "gpio.h"
#include "gpio_extra.h"
#include "rtc_extra.h"

static struct callback_desc *irq_callback_desc;

int32_t max_irq_ctrl_init(struct irq_ctrl_desc **desc,
		      const struct irq_init_param *param)
{
	if(!param)
		return -EINVAL;

	struct irq_ctrl_desc *descriptor = calloc(1, sizeof(*descriptor));
	if(!descriptor)
		return -ENOMEM;

	descriptor->irq_ctrl_id = param->irq_ctrl_id;
	descriptor->platform_ops = &irq_ops;
	descriptor->extra = param->extra;
	
	*desc = descriptor;	

	return 0;
}

int32_t max_irq_ctrl_remove(struct irq_ctrl_desc *desc)
{
	for(uint32_t i = 0; i < MXC_IRQ_COUNT; i++) {
		NVIC_DisableIRQ(BASE_IRQ + i);
	}

	free(desc);

	return 0;
}

int32_t max_irq_register_callback(struct irq_ctrl_desc *desc, uint32_t irq_id,
			      struct callback_desc *callback_desc)
{
	if(!desc)
		return -EINVAL;
	
	int32_t ret = 0;
	
	switch(irq_id){
	case MAX_UART0_INT_ID:
		ret = uart_register_callback(0, callback_desc);
		break;
	case MAX_UART1_INT_ID:
		ret = uart_register_callback(1, callback_desc);
		break;
	case MAX_GPIO_INT_ID:
		ret = max_gpio_register_callback(desc, callback_desc);
		break;
	case MAX_RTC_INT_ID:
		ret = rtc_register_callback(callback_desc);
		break;
	default:
		return -EINVAL;
	}

	return ret;
}

int32_t max_irq_unregister(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	if(!desc)
		return -EINVAL;
	
	int32_t ret = 0;
	
	switch(irq_id){
	case MAX_UART0_INT_ID:
		ret = uart_unregister_callback(0);
		break;
	case MAX_UART1_INT_ID:
		ret = uart_unregister_callback(1);
		break;
	case MAX_GPIO_INT_ID:
		ret = max_gpio_unregister_callback(desc);
		break;
	case MAX_RTC_INT_ID:
		ret = rtc_unregister_callback();
		break;
	default:
		return -EINVAL;
	}

	return ret;
}

int32_t max_irq_global_enable(struct irq_ctrl_desc *desc)
{
	if(!desc)
		return -EINVAL;

	for(uint32_t i = 0; i < MXC_IRQ_COUNT; i++) {
		NVIC_EnableIRQ(BASE_IRQ + i);
		if(i == MAX_GPIO_INT_ID)
			max_gpio_enable_irq(desc);
		if(i == MAX_RTC_INT_ID) {
			rtc_enable_irq(RTC_TIMEOFDAY_INT);
			rtc_enable_irq(RTC_SUBSEC_INT);
		}
	}

	return 0;
}

int32_t max_irq_global_disable(struct irq_ctrl_desc *desc)
{
	if(!desc)
		return -EINVAL;

	for(uint32_t i = 0; i < MXC_IRQ_COUNT; i++) {
		NVIC_DisableIRQ(BASE_IRQ + i);
		if(i == MAX_GPIO_INT_ID)
			max_gpio_disable_irq(desc);
		if(i == MAX_RTC_INT_ID) {
			rtc_enable_irq(RTC_TIMEOFDAY_INT);
			rtc_enable_irq(RTC_SUBSEC_INT);
		}

	}

	return 0;
}

int32_t max_irq_enable(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	if(!desc)
		return -EINVAL;
	
	int32_t ret = 0;
		
	switch(irq_id){
	case MAX_UART0_INT_ID:
	case MAX_UART1_INT_ID:
		break;
	case MAX_GPIO_INT_ID:
		max_gpio_enable_irq(desc);
		break;
	case MAX_RTC_INT_ID:
		rtc_enable_irq();
		break;
	default:
		return -EINVAL;
	}

	NVIC_EnableIRQ(BASE_IRQ + irq_id);

	return 0;
}

int32_t max_irq_disable(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	if(!desc)
		return -EINVAL;
		
	switch(irq_id){
	case MAX_UART0_INT_ID:
	case MAX_UART1_INT_ID:
		break;
	case MAX_GPIO_INT_ID:
		max_gpio_enable_irq(desc);
		break;
	case MAX_RTC_INT_ID:
		rtc_enable_irq();
		break;
	default:
		return -EINVAL;
	}

	NVIC_ClearPendingIRQ(BASE_IRQ + irq_id);
	NVIC_DisableIRQ(BASE_IRQ + irq_id);

	return 0;
}

const struct irq_platform_ops irq_ops = {
	.init = &max_irq_ctrl_init,
	.register_callback = &max_irq_register_callback,
	.unregister = &max_irq_unregister,
	.global_enable = &max_irq_global_enable,
	.global_disable = &max_irq_global_disable,
	.enable = &max_irq_enable,
	.disable = &max_irq_disable,
	.remove = &max_irq_ctrl_remove
};

