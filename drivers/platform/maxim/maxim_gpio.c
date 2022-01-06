#include <errno.h>
#include <stdlib.h>
#include "no-os/gpio.h"
#include "no-os/irq.h"
#include "no-os/util.h"
#include "gpio.h"
#include "gpio_regs.h"
#include "gpio_extra.h"
#include "max32660.h"

#define N_INT	14

static struct callback_desc *gpio_callback[N_INT];

void GPIO0_IRQHandler()
{
	uint32_t pin = 0;
	mxc_gpio_regs_t *gpio_regs = MXC_GPIO_GET_GPIO(0);
	uint32_t stat_reg = gpio_regs->int_stat;
	
	/** Clear interrupt flags */	
	gpio_regs->int_clr = stat_reg;
	while(stat_reg){
		if(stat_reg & 1){
			if(!gpio_callback[pin])
				return;
			void *ctx = gpio_callback[pin]->ctx;
			gpio_callback[pin]->callback(ctx, pin, NULL);
		}
		pin++;
		stat_reg >>= 1;
	}
}

int32_t gpio_get(struct gpio_desc **desc,
		 const struct gpio_init_param *param)
{
	if(!param || param->number >= N_PINS)
		return -EINVAL;

	struct gpio_desc *descriptor = calloc(1, sizeof(*descriptor));
	if(!descriptor)
		return -ENOMEM;

	descriptor->number = param->number;
	descriptor->platform_ops = &gpio_ops;
	descriptor->extra = param->extra;
	((gpio_cfg_t *)descriptor->extra)->mask = BIT(param->number);

	GPIO_Config(descriptor->extra);
	*desc = descriptor;

	return 0;
}

int32_t gpio_get_optional(struct gpio_desc **desc,
			  const struct gpio_init_param *param)
{
	if(param == NULL) {
		*desc = NULL;
		return 0;
	}

	return gpio_get(desc, param);
}

int32_t gpio_remove(struct gpio_desc *desc)
{
	free(desc);

	return 0;
}

int32_t gpio_direction_input(struct gpio_desc *desc)
{
	if(!desc || !desc->extra || desc->number >= N_PINS)
		return -EINVAL;
	
	gpio_cfg_t *maxim_extra = (gpio_cfg_t *)desc->extra;

	if(!maxim_extra || maxim_extra->port >= N_PORTS)
		return -EINVAL;

	maxim_extra->mask = BIT(desc->number);
	maxim_extra->func = GPIO_FUNC_IN;
	GPIO_Config(maxim_extra);

	return 0;
}

int32_t gpio_direction_output(struct gpio_desc *desc, uint8_t value)
{
	if(!desc || !desc->extra || desc->number >= N_PINS)
		return -EINVAL;
	
	gpio_cfg_t *maxim_extra = (gpio_cfg_t *)desc->extra;
	if(maxim_extra->port >= N_PORTS)
		return -EINVAL;

	maxim_extra->mask = BIT(desc->number);
	maxim_extra->func = GPIO_FUNC_OUT;
	GPIO_Config(maxim_extra);

	if(value == 0)
		GPIO_OutClr(maxim_extra);
	else
		GPIO_OutSet(maxim_extra);

	return 0;
}

int32_t gpio_get_direction(struct gpio_desc *desc, uint8_t *direction)
{
	if(!desc || desc->number >= N_PINS)
		return -EINVAL;

	gpio_cfg_t *maxim_extra = (gpio_cfg_t *)desc->extra;
	if(!maxim_extra)
		return -EINVAL;
	
	if(maxim_extra->func == GPIO_FUNC_OUT)
		*direction = GPIO_OUT;
	else if(maxim_extra->func == GPIO_FUNC_IN)
		*direction = GPIO_IN;
	else
		return -EINVAL;

	return 0;
}

int32_t gpio_set_value(struct gpio_desc *desc, uint8_t value)
{

	if(!desc || !desc->extra || desc->number >= N_PINS)
		return -EINVAL;

	gpio_cfg_t *maxim_extra = (gpio_cfg_t *)desc->extra;
	mxc_gpio_regs_t *gpio_regs = MXC_GPIO_GET_GPIO(maxim_extra->port);
	
	switch(value) {
	case GPIO_LOW:
		GPIO_OutClr(maxim_extra);
		break;
	case GPIO_HIGH:
		GPIO_OutSet(maxim_extra);
		break;
	case GPIO_HIGH_Z:
		gpio_regs->en &= ~maxim_extra->mask;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

int32_t gpio_get_value(struct gpio_desc *desc, uint8_t *value)
{
	if(!desc || desc->number >= N_PINS)
		return -EINVAL;
	
	gpio_cfg_t *maxim_extra = (gpio_cfg_t *)desc->extra;
	if(!maxim_extra)
		return -EINVAL;

	if(maxim_extra->func == GPIO_FUNC_IN)
		*value = GPIO_InGet(maxim_extra);
	else if(maxim_extra->func == GPIO_FUNC_OUT)
		*value = GPIO_OutGet(maxim_extra);
	else
		return -EINVAL;

	return 0;
}

int32_t gpio_irq_set_trigger_level(struct gpio_desc *desc, enum irq_trig_level trig_l)
{
	if(!desc || !desc->extra || desc->number >= N_PINS)
		return -EINVAL;
	
	gpio_cfg_t *maxim_extra = (gpio_cfg_t *)desc->extra;
	mxc_gpio_regs_t *gpio_regs = MXC_GPIO_GET_GPIO(maxim_extra->port);

	/** Disable interrupts for pin desc->number */
	gpio_regs->int_en &= ~(BIT(desc->number));
	/** Clear pending interrupts for pin desc->number */
	gpio_regs->int_clr |= BIT(desc->number);

	switch(trig_l){
	case IRQ_EDGE_RISING:
		/** Select edge triggered interrupt mode */
		gpio_regs->int_mod |= BIT(desc->number);
		/** Select rising edge trigger condition */
		gpio_regs->int_pol &= ~(BIT(desc->number));
		break;
	case IRQ_EDGE_FALLING:
		/** Select edge triggered interrupt mode */
		gpio_regs->int_mod |= BIT(desc->number);
		/** Select falling edge trigger condition */
		gpio_regs->int_pol |= BIT(desc->number);
		break;
	case IRQ_LEVEL_HIGH:
		/** Select level triggered interrupt mode */
		gpio_regs->int_mod &= ~(BIT(desc->number));
		/** Select level high trigger condition */
		gpio_regs->int_pol &= ~(BIT(desc->number));
		break;
	case IRQ_LEVEL_LOW:
		/** Select level triggered interrupt mode */
		gpio_regs->int_mod &= ~(BIT(desc->number));
		/** Select level low trigger condition */
		gpio_regs->int_pol |= BIT(desc->number);
		break;
	case IRQ_EDGE_BOTH:
		/** Edge triggered on both rising and falling */
		gpio_regs->int_dual_edge |= BIT(desc->number);
		break;
	default:
		gpio_regs->int_en |= BIT(desc->number);
		return -EINVAL;	
	}
	/** Enable interupts for pin desc->number */
	gpio_regs->int_en |= BIT(desc->number);

	return 0;
}

int32_t gpio_register_callback(struct irq_ctrl_desc *ctrl_desc, struct callback_desc *desc)
{
	if(!desc || !ctrl_desc || !desc->config)
		return -EINVAL;
	
/*
	if(!gpio_callback[pin]){
		gpio_callback[pin] = calloc(1, sizeof(*gpio_callback));
		if(!gpio_callback[pin])
			return -ENOMEM;
	}
*/
	int32_t error = 0;
	struct gpio_irq_config *g_irq = desc->config;
	struct gpio_desc *g_desc = g_irq->desc;
	enum irq_trig_level trig_level = g_irq->mode;

	struct callback_desc *descriptor = calloc(1, sizeof(*descriptor));
	if(!descriptor)
		return -ENOMEM;
		
	error = gpio_direction_input(g_desc);
	error = gpio_irq_set_trigger_level(g_desc, trig_level);
	if(error){
		free(descriptor);
		return error;
	}
	descriptor->ctx = desc->ctx;
	descriptor->callback = desc->callback;
	descriptor->config = desc->config;

	gpio_callback[g_desc->number] = descriptor;
	
	return 0;	
}	

int32_t gpio_unregister_callback(uint8_t pin)
{
	if(!gpio_callback[pin])
		return -EINVAL;
	
	free(gpio_callback[pin]);
	
	return 0;
}

const struct gpio_platform_ops gpio_ops = {
	.gpio_ops_get = &gpio_get,
	.gpio_ops_get_optional = &gpio_get_optional,
	.gpio_ops_remove = &gpio_remove,
	.gpio_ops_direction_input = &gpio_direction_input,
	.gpio_ops_direction_output = &gpio_direction_output,
	.gpio_ops_get_direction = &gpio_get_direction,
	.gpio_ops_set_value = &gpio_set_value,
	.gpio_ops_get_value = &gpio_get_value
};

