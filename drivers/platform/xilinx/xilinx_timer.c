/***************************************************************************//**
*   @file   xilinx/xilinx_timer.c
*   @brief  Timer and GIC control module source.
*   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright 2020(c) Analog Devices, Inc.
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

#include <stdlib.h>
#include <xparameters.h>
#include "no_os_timer.h"
#include "xilinx_timer.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#if defined(_XPARAMETERS_PS_H_)
#define CORE_PRIVATE_TIMER_CLOCK (XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ / 2)
#endif

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Start a timer.
 * @param [in] desc - Pointer to the device handler.
 * @return 0 in case of success, -1 otherwise
 */
int32_t xilinx_timer_start(struct no_os_timer_desc *desc)
{
	struct xil_timer_desc *xdesc;

	if (!desc)
		return -1;

	xdesc = desc->extra;

	switch (xdesc->type) {
	case TIMER_PS:
#ifdef XSCUTIMER_H
		XScuTimer_Start(xdesc->instance);
		break;
#endif
		return -1;
	case TIMER_PL:
#ifdef XTMRCTR_H
		XTmrCtr_Start(xdesc->instance, xdesc->active_tmr);
		break;
#endif
		return -1;
	}

	return 0;
}

/**
 * @brief Stop a timer from counting.
 * @param [in] desc - Pointer to the device handler.
 * @return 0 in case of success, -1 otherwise
 */
int32_t xilinx_timer_stop(struct no_os_timer_desc *desc)
{
	struct xil_timer_desc *xdesc;

	if (!desc)
		return -1;

	xdesc = desc->extra;

	switch (xdesc->type) {
	case TIMER_PS:
#ifdef XSCUTIMER_H
		XScuTimer_Stop(xdesc->instance);
		break;
#endif
		return -1;
	case TIMER_PL:
#ifdef XTMRCTR_H
		XTmrCtr_Stop(xdesc->instance, xdesc->active_tmr);
		break;
#endif
		return -1;
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief Get the current counter value of the timer.
 * @param [in] desc - Pointer to the device handler.
 * @param [out] counter - Pointer to the current counter value.
 * @return 0 in case of success, -1 otherwise
 */
int32_t xilinx_timer_counter_get(struct no_os_timer_desc *desc,
				 uint32_t *counter)
{
	struct xil_timer_desc *xdesc;

	if (!desc || !counter)
		return -1;

	xdesc = desc->extra;

	switch (xdesc->type) {
	case TIMER_PS:
#ifdef XSCUTIMER_H
		*counter = XScuTimer_GetCounterValue((XScuTimer *)xdesc->instance);
		++(*counter);
		break;
#endif
		return -1;
	case TIMER_PL:
#ifdef XTMRCTR_H
		*counter = XTmrCtr_GetValue(xdesc->instance, xdesc->active_tmr);
		break;
#endif
		return -1;
	default:
		*counter = 0;

		return -1;
	}

	return 0;
}

/**
 * @brief Set the current counter value of the timer.
 * @param [in] desc - Pointer to the device handler.
 * @param [in] new_val - New value for the timer counter.
 * @return 0 in case of success, -1 otherwise
 */
int32_t xilinx_timer_counter_set(struct no_os_timer_desc *desc,
				 uint32_t new_val)
{
	struct xil_timer_desc *xdesc;

	if (!desc)
		return -1;

	xdesc = desc->extra;

	switch (xdesc->type) {
	case TIMER_PS:
#ifdef XSCUTIMER_H
		XScuTimer_LoadTimer((XScuTimer *)xdesc->instance, new_val - 1);
		break;
#endif
		return -1;
	case TIMER_PL:
#ifdef XTMRCTR_H
		XTmrCtr_SetResetValue(xdesc->instance, xdesc->active_tmr, new_val);
		XTmrCtr_Reset(xdesc->instance, xdesc->active_tmr);
		break;
#endif
		return -1;
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief Get the timer clock frequency.
 * @param [in] desc - Pointer to the device handler.
 * @param [out] freq_hz - Pointer to the returned frequency.
 * @return 0 in case of success, -1 otherwise
 */
int32_t xilinx_timer_count_clk_get(struct no_os_timer_desc *desc,
				   uint32_t *freq_hz)
{
	struct xil_timer_desc *xdesc;

	if (!desc || !freq_hz)
		return -1;

	xdesc = desc->extra;

	switch (xdesc->type) {
	case TIMER_PS:
#ifdef XSCUTIMER_H
		*freq_hz = desc->freq_hz;
		break;
#endif
		return -1;
	case TIMER_PL:
#ifdef XTMRCTR_H
		*freq_hz = ((XTmrCtr_Config *)xdesc->config)->SysClockFreqHz;
		break;
#endif
		return -1;
	default:
		*freq_hz = 0;

		return -1;
	}

	return 0;
}

/**
 * @brief Set the timer clock frequency.
 * @param [in] desc - Pointer to the device handler.
 * @param [in] freq_hz - New timer frequency in hertz.
 * @return 0 in case of success, -1 otherwise
 */
int32_t xilinx_timer_count_clk_set(struct no_os_timer_desc *desc,
				   uint32_t freq_hz)
{
	struct xil_timer_desc *xdesc;

	if (!desc || !freq_hz)
		return -1;

	xdesc = desc->extra;

	switch (xdesc->type) {
	case TIMER_PS:
#ifdef XSCUTIMER_H
		;
		uint32_t prescaler = CORE_PRIVATE_TIMER_CLOCK / freq_hz;
		if (prescaler == 0 || prescaler >= 256)
			return -1;
		XScuTimer_SetPrescaler(xdesc->instance, prescaler - 1);
		desc->freq_hz = CORE_PRIVATE_TIMER_CLOCK / prescaler;
		break;
#endif
		return -1;
	case TIMER_PL:
#ifdef XTMRCTR_H
		(void *)xdesc;
		/** Cannot be done. */
#endif
		return -1;
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief Get the elapsed time in nsec for the timer.
 * @param [in] desc         - Pointer to the device handler.
 * @param [in] elapsed_time - The elapsed time in nsec.
 * @return -ENOSYS.
 */
int32_t xilinx_timer_get_elapsed_time_nsec(struct no_os_timer_desc *desc,
		uint64_t *elapsed_time)
{
	/* Function not implemented */
	return -ENOSYS;
}

/**
 * @brief Initialize hardware timer and the handler structure associated with
 *        it.
 * @param [out] desc - Pointer to the reference of the device handler.
 * @param [in] param - Initialization structure.
 * @return 0 in case of success, -1 otherwise
 */
int32_t xilinx_timer_init(struct no_os_timer_desc **desc,
			  const struct no_os_timer_init_param *param)
{
	int32_t ret;
	struct no_os_timer_desc *dev;
	struct xil_timer_desc *xdesc;
	struct xil_timer_init_param *xinit;

	if (!desc || !param)
		return -1;

	xinit = param->extra;

	dev = (struct no_os_timer_desc *)no_os_calloc(1, sizeof(*dev));
	if(!dev)
		return -1;
	xdesc = (struct xil_timer_desc *)no_os_calloc(1, sizeof(*xdesc));
	if(!xdesc)
		goto error_desc;

	dev->id = param->id;
	dev->freq_hz = param->freq_hz;
	dev->ticks_count = param->ticks_count;
	dev->extra = xdesc;
	xdesc->active_tmr = xinit->active_tmr;
	xdesc->type = xinit->type;

	switch (xdesc->type) {
	case TIMER_PS:
#ifdef XSCUTIMER_H
		xdesc->config = XScuTimer_LookupConfig(dev->id);
		xdesc->instance = no_os_calloc(1, sizeof(XScuTimer));
		if (!xdesc->instance)
			goto error_xdesc;
		ret = XScuTimer_CfgInitialize(xdesc->instance, xdesc->config,
					      ((XScuTimer_Config *)xdesc->config)->BaseAddr);
		if (ret != XST_SUCCESS) {
			no_os_free(xdesc->instance);
			goto error_xdesc;
		}
		ret = xilinx_timer_count_clk_set(dev, param->freq_hz);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto error_xdesc;

		ret = xilinx_timer_counter_set(dev, dev->ticks_count);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto error_xdesc;

		XScuTimer_EnableAutoReload((XScuTimer *)xdesc->instance);
		XScuTimer_EnableInterrupt((XScuTimer *)xdesc->instance);
		break;
#endif
		goto error_xdesc;
	case TIMER_PL:
#ifdef XTMRCTR_H
		xdesc->instance = (XTmrCtr *)no_os_calloc(1, sizeof(XTmrCtr));
		if (!xdesc->instance)
			return -1;

		xdesc->config = XTmrCtr_LookupConfig(dev->id);
		if (!xdesc->config) {
			no_os_free(xdesc->instance);
			goto error_desc;
		}
		XTmrCtr_CfgInitialize(xdesc->instance, xdesc->config,
				      ((XTmrCtr_Config *)xdesc->config)->BaseAddress);
		ret = XTmrCtr_InitHw(xdesc->instance);
		if (ret != 0) {
			no_os_free(xdesc->instance);
			goto error_desc;
		}
		dev->freq_hz = ((XTmrCtr_Config *)xdesc->config)->SysClockFreqHz;

		ret = XTmrCtr_SelfTest(xdesc->instance, dev->id);
		if(ret != 0)
			goto error_desc;
		XTmrCtr_SetOptions(xdesc->instance, xdesc->active_tmr,
				   XTC_DOWN_COUNT_OPTION | XTC_INT_MODE_OPTION |
				   XTC_AUTO_RELOAD_OPTION);
		XTmrCtr_SetResetValue(xdesc->instance, xdesc->active_tmr,
				      dev->ticks_count);
		break;
#endif
		goto error_xdesc;
	default:
		goto error_xdesc;
	}

	*desc = dev;

	return 0;

error_xdesc:
	no_os_free(xdesc);
error_desc:
	no_os_free(dev);

	return -1;
}

/**
 * @brief Free the memory allocated by timer_setup().
 * @param [in] desc - Pointer to the device handler.
 * @return 0 in case of success, -1 otherwise
 */
int32_t xilinx_timer_remove(struct no_os_timer_desc *desc)
{
	struct xil_timer_desc *xdesc;

	if(!desc)
		return -1;

	xdesc = desc->extra;

	switch (xdesc->type) {
	case TIMER_PS:
#ifdef XSCUTIMER_H
		XScuTimer_Stop(xdesc->instance);
		no_os_free(xdesc->instance);
		break;
#endif
		return -1;
	case TIMER_PL:
#ifdef XTMRCTR_H
		XTmrCtr_Stop(xdesc->instance, xdesc->active_tmr);
		break;
#endif
		return -1;
	default:
		return -1;
	}

	no_os_free(xdesc);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Xilinx platform specific timer platform ops structure
 */
const struct no_os_timer_platform_ops xil_timer_ops = {
	.init = &xilinx_timer_init,
	.start = &xilinx_timer_start,
	.stop = &xilinx_timer_stop,
	.counter_get = &xilinx_timer_counter_get,
	.counter_set = &xilinx_timer_counter_set,
	.count_clk_get = &xilinx_timer_count_clk_get,
	.count_clk_set = &xilinx_timer_count_clk_set,
	.get_elapsed_time_nsec = &xilinx_timer_get_elapsed_time_nsec,
	.remove = &xilinx_timer_remove
};
