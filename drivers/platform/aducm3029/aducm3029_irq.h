/***************************************************************************//**
 *   @file   aducm3029_irq.h
 *   @brief  Header file of IRQ driver for ADuCM302x
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
