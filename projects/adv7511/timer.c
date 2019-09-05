/***************************************************************************//**
*   @file   timer.c
*   @brief  Timer and GIC control module source.
*   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright 2019(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "timer.h"
#include <stdlib.h>

/******************************************************************************/
/************************** Variable Definitions ******************************/
/******************************************************************************/

volatile uint32_t timer_counter_intr = 0;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * Timer ISR.
 *
 * Helps implement the software delay routine.
 *
 * @param [in] instance - Pointer to a application passed parameter, in this
 * 	                      case the pointer to the timer device handler.
 *
 * @return N/A, void
 */
#if defined(XSCUTIMER_H)
void timer_isr(void *instance)
#elif defined(XTMRCTR_H)
void timer_isr(void *instance, uint8_t timer_nr)
#endif
{
	timer_counter_intr++;
}

/**
 * Initialize hardware timer and the handler structure associated with it.
 *
 * @param [out] desc - Pointer to the reference of the device handler.
 * @param [in] init_param - Initialization structure.
 *
 * @return 0 in case of success, negative error code otherwise
 */
int32_t timer_setup(struct timer_desc **desc,
		    struct timer_init_param *init_param)
{
	int32_t ret;
	struct timer_desc *dev;

	dev = calloc(1, sizeof *dev);
	if(!dev)
		return -1;

	dev->id = init_param->id;
	dev->freq = init_param->start_freq;
	dev->auto_reload = init_param->auto_reload;

#if defined(XSCUTIMER_H)
	dev->load = CORE_PRIVATE_TIMER_CLOCK / dev->freq;
	dev->config = XScuTimer_LookupConfig(dev->id);

	ret = XScuTimer_CfgInitialize(&dev->instance, dev->config,
				      dev->config->BaseAddr);
	if(ret != 0)
		goto error;

	XScuTimer_LoadTimer(&dev->instance, dev->load);

	if(dev->auto_reload)
		XScuTimer_EnableAutoReload(&dev->instance);
#elif defined(XTMRCTR_H)
	uint8_t tmr_options = 0;

	dev->load = MB_AXI_TIMER_CLOCK / dev->freq;
	dev->active_tmr = init_param->active_tmr;

	ret = XTmrCtr_Initialize(&dev->instance, dev->id);
	if(ret != 0)
		goto error;

	ret = XTmrCtr_SelfTest(&dev->instance, dev->id);
	if(ret != 0)
		goto error;

	XTmrCtr_SetHandler(&dev->instance, timer_isr, &dev->instance);
	tmr_options = XTC_DOWN_COUNT_OPTION | XTC_INT_MODE_OPTION;
	tmr_options |= dev->auto_reload ? XTC_AUTO_RELOAD_OPTION : 0;
	XTmrCtr_SetOptions(&dev->instance, dev->active_tmr, tmr_options);

	XTmrCtr_SetResetValue(&dev->instance, dev->active_tmr, dev->load);
#else
	ret = -1;
	goto error;
#endif

	*desc = dev;

	return ret;
error:
	free(dev);

	return ret;
}

/**
 * Free the memory allocated by timer_setup().
 *
 * timer_setup() function needs to be called first.
 *
 * @param [in] desc - Pointer to the device handler.
 *
 * @return 0 in case of success, negative error code otherwise
 */
int32_t timer_remove(struct timer_desc *desc)
{
	if(!desc)
		return -1;

#if defined(XSCUTIMER_H)
	XScuTimer_Stop(&desc->instance);
#elif defined(XTMRCTR_H)
	XTmrCtr_Stop(&desc->instance, desc->active_tmr);
#endif

	free(desc);

	return 0;
}

/**
 * Start a timer.
 *
 * timer_setup() function needs to be called first.
 *
 * @param [in] desc - Pointer to the device handler.
 *
 * @return void
 */
void timer_start(struct timer_desc *desc)
{
	timer_counter_intr = 0;
#if defined(XSCUTIMER_H)
	XScuTimer_Start(&desc->instance);
#elif defined(XTMRCTR_H)
	XTmrCtr_Start(&desc->instance, desc->active_tmr);
#endif
}

/**
 * Stop a timer from counting.
 *
 * timer_setup() function needs to be called first.
 *
 * @param [in] desc - Pointer to the device handler.
 *
 * @return void
 */
void timer_stop(struct timer_desc *desc)
{
#if defined(XSCUTIMER_H)
	XScuTimer_Stop(&desc->instance);
#elif defined(XTMRCTR_H)
	XTmrCtr_Stop(&desc->instance, desc->active_tmr);
#endif
}

/**
 * Enable timer interrupt.
 *
 * timer_setup() function needs to be called first.
 *
 * @param [in] desc - Pointer to the device handler.
 *
 * @return void
 */
void timer_enable_interrupt(struct timer_desc *desc)
{
#if defined(XSCUTIMER_H)
	XScuTimer_EnableInterrupt(&desc->instance);
#elif defined(XTMRCTR_H)
#endif
}

/**
 * Disable timer interrupt.
 *
 * timer_setup() function needs to be called first.
 *
 * @param [in] desc - Pointer to the device handler.
 *
 * @return void
 */
void timer_disable_interrupt(struct timer_desc *desc)
{
#if defined(XSCUTIMER_H)
	XScuTimer_DisableInterrupt(&desc->instance);
#elif defined(XTMRCTR_H)
#endif
}

/**
 * Initialize the Generic Interrupt Controller (GIC) and get the pointer to its
 * device handler.
 *
 * @param [out] desc - Pointer to the reference of the device handler.
 * @param [in] init_param - Initialization structure.
 *
 * @return 0 in case of success, negative error code otherwise
 */
int32_t gic_setup(struct gic_desc **desc, struct gic_init_param *init_param)
{
	struct gic_desc *dev;
	int32_t ret;

	dev = calloc(1, sizeof *dev);
	if(!dev)
		return -1;

	dev->id = init_param->id;

#if defined(XSCUGIC_H)
	dev->config = XScuGic_LookupConfig(dev->id);
	ret = XScuGic_CfgInitialize(&dev->instance, dev->config,
				    dev->config->CpuBaseAddress);
	if(ret != 0)
		goto error;
	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
				     (Xil_ExceptionHandler)XScuGic_InterruptHandler, &dev->instance);
	Xil_ExceptionEnable();
	ret = XScuGic_SelfTest(&dev->instance);
	if(ret != 0)
		goto error;
#elif defined(XINTC_H)
	ret = XIntc_Initialize(&dev->instance, dev->id);
	if(ret != 0)
		goto error;

	XIntc_Start(&dev->instance, XIN_REAL_MODE);
	if(ret != 0)
		goto error;
#else
	ret = -1;
	goto error;
#endif

	*desc = dev;

	return ret;
error:
	free(dev);

	return ret;
}

/**
 * Free memory allocated by gic_setup().
 *
 * gic_setup() must be called first.
 *
 * @param [in] desc - Pointer to the device handler.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t gic_remove(struct gic_desc *desc)
{
	if(!desc)
		return -1;

#if defined(XSCUGIC_H)
	Xil_ExceptionRemoveHandler(XIL_EXCEPTION_ID_IRQ_INT);
	Xil_ExceptionDisable();
#elif defined(XINTC_H)
	XIntc_Stop(&desc->instance);
#endif

	free(desc);

	return 0;
}

/**
 * Connect an interrupt ID and an ISR to be called by the GIC.
 *
 * gic_setup() must be called first.
 *
 * @param [in] desc - Pointer to the device handler.
 * @param [in] int_id - ID of the interrupt.
 * @param [in] isr_ptr - Pointer to the ISR to be connected.
 * @param [in] instance_ptr - Pointer to an application supplied parameter in
 *                            in case the ISR needs to be called with it. This
 *                            will be passed to the specific ISR when calling
 *                            it. In this application, the device handler will
 *                            be used.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t gic_connect_handler(struct gic_desc *desc, uint32_t int_id,
			    Xil_ExceptionHandler isr_ptr, void *instance_ptr)
{
#if defined(XSCUGIC_H)
	return XScuGic_Connect(&desc->instance, int_id, isr_ptr, instance_ptr);
#elif defined(XINTC_H)
	return XIntc_Connect(&desc->instance, int_id, isr_ptr, instance_ptr);
#endif
}

/**
 * Disconnect a connected ISR from an interrupt ID in the GIC.
 *
 * gic_setup() must be called first.
 *
 * @param [in] desc - Pointer to the device handler.
 * @param [in] int_id - Interrupt ID to disconnect.
 *
 * @return void
 */
void gic_disconnect_handler(struct gic_desc *desc, uint32_t int_id)
{
#if defined(XSCUGIC_H)
	XScuGic_Disconnect(&desc->instance, int_id);
#elif defined(XINTC_H)
	XIntc_Disconnect(&desc->instance, int_id);
#endif
}

/**
 * Enable GIC interrupt for a specific ID.
 *
 * gic_setup() must be called first.
 *
 * @param [in] desc - Pointer to the device handler.
 * @param [in] int_id - Interrupt ID to be enabled.
 *
 * @return void
 */
void gic_enable_interrupt(struct gic_desc *desc, uint32_t int_id)
{
#if defined(XSCUGIC_H)
	XScuGic_Enable(&desc->instance, int_id);
#elif defined(XINTC_H)
	XIntc_Enable(&desc->instance, int_id);
#endif
}

/**
 * Disable GIC interrupt for a specific ID.
 *
 * gic_setup() must be called first.
 *
 * @param [in] desc - Pointer to the device handler.
 * @param [in] int_id - Interrupt ID to be disabled.
 *
 * @return void
 */
void gic_disable_interrupt(struct gic_desc *desc, uint32_t int_id)
{
#if defined(XSCUGIC_H)
	XScuGic_Disable(&desc->instance, int_id);
#elif defined(XINTC_H)
	XIntc_Disable(&desc->instance, int_id);
#endif
}
