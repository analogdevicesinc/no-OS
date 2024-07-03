/*******************************************************************************
 *   @file   intrpt_handler.h
 *   @brief  Interrupt handler functionalities
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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

#include "intrpt_handler.h"

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/

extern volatile uint8_t rmsOneReady;
extern volatile uint64_t rtcIntrpTimeInMilliSeconds;
static volatile int64_t timestamp;
volatile uint8_t newSyncTimeAvailable = 0;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

void isr_afe0(void *context)
{
	rmsOneReady = 1;
}

int init_interrupt(void)
{
	int status = 0;
	struct no_os_gpio_desc *intr_gpio_desc;
	struct no_os_irq_ctrl_desc *afe0_callback_ctrl;
	afe0_callback_desc.callback = isr_afe0;

	// Setup GPIO input pin for data-ready interrupt
	status = no_os_gpio_get(&intr_gpio_desc, &intr_gpio_ip);
	if (status)
		return status;
	status = no_os_gpio_direction_input(intr_gpio_desc);
	if (status)
		goto error1;
	status = no_os_irq_ctrl_init(&afe0_callback_ctrl, &afe_callback_ctrl_ip);
	if (status)
		goto error1;
	status = no_os_irq_register_callback(afe0_callback_ctrl, INTR_GPIO_PIN_NUM,
					     &afe0_callback_desc);
	if (status)
		goto error2;
	status = no_os_irq_trigger_level_set(afe0_callback_ctrl, INTR_GPIO_PIN_NUM,
					     NO_OS_IRQ_EDGE_FALLING);
	if (status)
		goto error2;
	status = no_os_irq_enable(afe0_callback_ctrl, INTR_GPIO_PIN_NUM);
	if (status)
		goto error2;

	NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(INTR_GPIO_PORT_NUM));
	return 0;

error2:
	no_os_irq_ctrl_remove(afe0_callback_ctrl);
error1:
	no_os_gpio_remove(intr_gpio_desc);
	return status;
}

int64_t get_irq0_timestamp(void)
{
	return timestamp;
}
