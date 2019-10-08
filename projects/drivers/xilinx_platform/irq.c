#include <stdlib.h>
#include <xscugic.h>
#include "error.h"
#include "irq.h"
#include "xilinx_platform_drivers.h"

int32_t irq_init(struct irq_desc **desc,
		 const struct irq_init_param *param)
{
    int32_t status;
    XScuGic_Config *IntcConfig; /* Config for interrupt controller */
    XScuGic *gic;
    xil_irq_desc *xil_dev;

    struct irq_desc *descriptor = calloc(1, sizeof(struct irq_desc));
    if (!descriptor)
    	return FAILURE;

    descriptor->extra = calloc(1, sizeof(struct xil_irq_desc));
	if(!(descriptor->extra)) {
		free(descriptor);
		return FAILURE;
	}

    gic = calloc(1, sizeof(XScuGic));
	if (!gic) {
		free(descriptor->extra);
		free(descriptor);
		return FAILURE;
	}

	xil_dev = descriptor->extra;
	xil_dev->gic = gic;
    /* Initialize the interrupt controller driver */
    IntcConfig = XScuGic_LookupConfig(param->irq_id);
    if (NULL == IntcConfig) {
        return FAILURE;
    }

    status = XScuGic_CfgInitialize(gic, IntcConfig,
                                   IntcConfig->CpuBaseAddress);
    if (status != XST_SUCCESS) {
        return FAILURE;
    }

    /*
     * Connect the interrupt controller interrupt handler to the
     * hardware interrupt handling logic in the processor.
     */
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                 (Xil_ExceptionHandler) XScuGic_InterruptHandler,
								 gic);

    *desc = descriptor;

    return SUCCESS;
}

int32_t irq_enable(void) {
	/* Enable interrupts */
	Xil_ExceptionEnable();

	return SUCCESS;
}
int32_t irq_disable(void) {
	/* Disable interrupts */
	Xil_ExceptionDisable();

	return SUCCESS;
}

int32_t irq_source_enable(struct irq_desc *desc, uint32_t irq_id) {
	xil_irq_desc *xil_dev = desc->extra;
    XScuGic_Enable(xil_dev->gic, irq_id);

	return SUCCESS;
}

int32_t irq_source_disable(struct irq_desc *desc, uint32_t irq_id) {
	xil_irq_desc *xil_dev = desc->extra;
    XScuGic_Disable(xil_dev->gic, irq_id);

	return SUCCESS;
}

/* Registers a generic IRQ handling function */
int32_t irq_register(struct irq_desc *desc, uint32_t irq_id, void (*irq_handler)(void *data), void *extra) {

	int32_t status;
	xil_irq_desc *xil_dev = desc->extra;
    /*
     * Connect a device driver handler that will be called when an
     * interrupt for the device occurs, the device driver handler
     * performs the specific interrupt processing for the device
     */
	status = XScuGic_Connect(xil_dev->gic, irq_id,
                             irq_handler,
							 extra);
    if (status != XST_SUCCESS) {
        return FAILURE;
    }

	return SUCCESS;
}

int32_t irq_remove(struct irq_desc *desc) {
	xil_irq_desc *xil_dev = desc->extra;
	free(xil_dev->gic);
	free(desc);

	return SUCCESS;
}
