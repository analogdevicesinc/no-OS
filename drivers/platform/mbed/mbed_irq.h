/***************************************************************************//**
 *   @file   mbed_irq.h
 *   @brief  Header containing extra types required for IRQ drivers
********************************************************************************
 * Copyright (c) 2020-2022 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MBED_IRQ_H
#define MBED_IRQ_H

// Platform support needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdbool.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum irq_id
 * @brief Interrupt IDs supported by the mbed irq driver
 * @note Every interrupt instance should have unique interrupt ID
 * The IDs can be mapped randomly to any of the instances as Mbed layer abstracts
 * most of the low level MCU details such as IRQn_type, IRQn_number, etc
 * Example: mbed_gpio_irq_enable(irq_desc, UART_RX_INT_ID1);
 */
enum irq_id {
	/** UART Rx interrupt ID1 (Multiple instances of UART Rx IRQs are possible
	 ** but only one implemented in mbed irq module) */
	UART_RX_INT_ID1,

	/** Ticker interrupt ID (Only one instance of Ticker IRQ possible) */
	TICKER_INT_ID,

	/* Number of available interrupts */
	NB_INTERRUPTS
};

/**
 * @struct mbed_irq_init_param
 * @brief Structure holding the extra parameters for Interrupt Request
 * Example:
 *		mbed::UnBufferedSerial my_uart;
 *		struct mbed_irq_init_param my_uart_rx_mbed_irq_init_param = {
 *			.int_obj_type = &my_uart
 *		}
 */
struct mbed_irq_init_param {
	uint32_t ticker_period_usec;	// Time period in usec for ticker event
	void *int_obj_type;		// Other app created Mbed driver instance (e.g. UnBuffered uart)
};

/**
 * @struct mbed_irq_desc
 * @brief Structure holding the platform descriptor for Interrupt Request
 */
struct mbed_irq_desc {
	uint32_t ticker_period_usec;	// Time period in usec for ticker event
	void *int_obj;			// Mbed driver instance (e.g. Ticker, uart)
};

/**
 * @brief Mbed specific IRQ platform ops structure
 */
extern const struct no_os_irq_platform_ops mbed_irq_ops;

#ifdef __cplusplus // Closing extern c
}
#endif

#endif // MBED_IRQ_H
