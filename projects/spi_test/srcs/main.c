/***************************************************************************//**
 *   @file   ad3552r/src/main.c
 *   @brief  Implementation of Main Function.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
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

#include "parameters.h"
#include "error.h"
#include "print_log.h"
#include "iio_app.h"
#include "spi.h"
#include "gpio.h"
#include "gpio_extra.h"
#include "delay.h"
#include "util.h"
#include "irq.h"
#include "irq_extra.h"
#include "timer.h"
#include "timer_extra.h"

#ifdef XILINX_PLATFORM

#include <xparameters.h>
#include <xil_cache.h>
#include <xil_cache.h>

#include "spi_extra.h"
#include "gpio_extra.h"
#endif

#include <stdint.h>

#include <math.h>


#ifdef XILINX_PLATFORM
struct gpio_platform_ops	*gpio_ops = &xil_gpio_platform_ops;
#define SPI_OPS 	(&xil_spi_reg_ops_pl)
#endif

struct gpio_desc *ldac;

struct timer_desc *tmr;
int val;

static void _tmr_callback(void *desc, uint32_t event,
			  void *extra)
{
	gpio_set_value(ldac, val);
	val = !val;
}

#define FREQ 100000000
#define LOAD 1000

int32_t update_freq(){
	uint32_t		freq;
	uint32_t		new_load;
	int32_t			ret;
	float			scale;

	ret = timer_count_clk_get(tmr, &freq);
	if (IS_ERR_VALUE(ret))
		return ret;

//	scale = (float)freq / (SEC_TO_10NS(1) * 10);
//	new_load = (desc->update_period_ns / 2.0) * scale;
	new_load = LOAD;
	ret = timer_counter_set(tmr, new_load);
	if (IS_ERR_VALUE(ret))
		return ret;

	ret = timer_start(tmr);
	if (IS_ERR_VALUE(ret))
		return ret;

	return 0;
}

int main(void)
{
	int32_t ret;

	pr_debug("Start\n");

	struct xil_gpio_init_param platorm_gpio_xil_param = {
			.device_id = GPIO_DEVICE_ID,
			.type = GPIO_PS
	};
	struct gpio_init_param	ldac_param = {
			.number = GPIO_OFFSET + GPIO_LDAC_N,
			.platform_ops = gpio_ops,
			.extra = &platorm_gpio_xil_param
	};
	ret = gpio_get(&ldac, &ldac_param);
	if (IS_ERR_VALUE(ret))
		return ret;

	ret = gpio_direction_output(ldac, 0);
	if (IS_ERR_VALUE(ret))
		return ret;


	struct xil_timer_init_param xil_tmr_param = {
			.type = TIMER_PS
	};

	struct xil_irq_init_param xil_irq_param = {
			.type = IRQ_PS,
	};
	struct irq_init_param	irq_param = {
			.irq_ctrl_id = INTC_DEVICE_ID,
			.extra = &xil_irq_param
	};
	struct irq_ctrl_desc	*irq_crtl;

	ret = irq_ctrl_init(&irq_crtl, &irq_param);
	PRINT_AND_RET_ON_ERR(ret, "irq_ctrl_init failed");

	struct callback_desc	call;
	struct timer_init_param	tmr_param;

	tmr_param.id = TIMER_DEVICE_ID;
	tmr_param.freq_hz = FREQ;
	tmr_param.extra = &xil_tmr_param;
	ret = timer_init(&tmr, &tmr_param);
	if (IS_ERR_VALUE(ret))
		return ret;

	call.callback = (void (*)(void *, uint32_t, void *))_tmr_callback;
	call.ctx = 0;
	call.config = NULL;
	ret = irq_register_callback(irq_crtl, TIMER_IRPT_INTR, &call);
	if (IS_ERR_VALUE(ret))
		return ret;

	ret = irq_enable(irq_crtl, TIMER_IRPT_INTR);
	if (IS_ERR_VALUE(ret))
		return ret;

	while (1) {
		update_freq();
		int ok = 1;
		while (ok)
			;
		ret = timer_stop(tmr);
		if (IS_ERR_VALUE(ret))
			return ret;
	}
}
