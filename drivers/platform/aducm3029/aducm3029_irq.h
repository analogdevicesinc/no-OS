/***************************************************************************//**
 *   @file   aducm3029_irq.h
 *   @brief  Header file of IRQ driver for ADuCM302x
 *   @author Mihail Chindris (mihail.chindris@analog.com)
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

#ifndef ADUCM3029_IRQ_H
# define ADUCM3029_IRQ_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <drivers/xint/adi_xint.h>
#include <stdbool.h>
#include <stdint.h>
#include "no_os_irq.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/** RTC interrupt defines */
#define RTC_COUNT_INT		ADI_RTC_COUNT_INT
#define RTC_COUNT_ROLLOVER_INT	ADI_RTC_COUNT_ROLLOVER_INT

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum irq_id
 * @brief Interrupts IDs supported by the irq driver
 */
enum irq_id {
	/** UART interrupt ID*/
	ADUCM_UART_INT_ID,
	/** RTC interrupt ID*/
	ADUCM_RTC_INT_ID,
	/** TIMER1 interrupt ID */
	ADUCM_TIMER1_INT_ID,
	/** Number of available interrupts */
	NB_INTERRUPTS,
};

/**
 * @enum irq_mode
 * @brief Trigger condition for the external interrupt
 */
enum irq_mode {
	/** Rising edge */
	IRQ_RISING_EDGE		= ADI_XINT_IRQ_RISING_EDGE,
	/** Falling edge */
	IRQ_FALLING_EDGE	= ADI_XINT_IRQ_FALLING_EDGE,
	/** Either edge */
	IRQ_EITHER_EDGE		= ADI_XINT_IRQ_EITHER_EDGE,
	/** High level */
	IRQ_HIGH_LEVEL		= ADI_XINT_IRQ_HIGH_LEVEL,
	/** Low level */
	IRQ_LOW_LEVEL		= ADI_XINT_IRQ_LOW_LEVEL
};

/**
 * @enum gpio_irq_mode
 * @brief Interrupt conditions for GPIO group interrupts
 */
enum gpio_irq_mode {
	/** Falling edge */
	GPIO_GROUP_NEGATIVE_EDGE,
	/** Rising edge */
	GPIO_GROUP_POSITIVE_EDGE
};

/**
 * @struct aducm_irq_ctrl_desc
 * @brief Stores specific platform parameters
 */
struct aducm_irq_ctrl_desc {

};

/**
 * @brief ADuCM3029 specific IRQ platform ops structure
 */
extern const struct no_os_irq_platform_ops aducm_irq_ops;

/**
 * @brief Struct used to store a (peripheral, callback) pair
 */
struct irq_action {
	/** Interrupt event */
	uint32_t irq_id;
	/** Peripheral handler */
	void *handle;
	/** Pointer to callback routine */
	void (*callback)(void *context);
	/** Pointer to user defined context */
	void *ctx;
	/** Trigger level for interrupts;
	 *  Currently only in use for XINT interrupts */
	enum no_os_irq_trig_level trig_lv;
};

/** Action comparator function */
int32_t irq_action_cmp(void *data1, void *data2);

#endif // ADUCM3029_IRQ_H
