

void internal_irq(void *context)
{
    struct alt_ctx *ctx = context;
    struct alt_irq_ctrl_desc *alt_desc = ctx->irq_ctrl->extra;

    alt_desc->irq_handlers[ctx->id](ctx->context, NULL);
    
}

int32_t irq_register(struct irq_ctrl_desc *desc, uint32_t irq_id,
		     void (*irq_handler)(void *context, void *extra),
			 void *context, void *config);
{

    desc->extra->irq_handlers[irq_id] = irq_handler;
    desc->extra->context[irq_id].id = irq_id;
    desc->extra->context[irq_id].app_context = context;
    desc->extra->context[irq_id].irq_ctrl = desc;

    alt_ic_isr_register(irq_id, internal_irq, context);
	return SUCCESS;
} 
