/***************************************************************************//**
 *   @file   powrms_gpios.h
 *   @brief  Button handling definitions and declarations.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include "no_os_gpio.h"

// Timing constants for hardware operations
#define ADC_POWER_STABILIZE_MS      100
#define ADC_5V_STABILIZE_MS         50

/**
 * @brief Initialize interrupt-based button handling
 *
 * Configures GPIO pins 0.16, 0.17, and 0.18 as input pins with pull-up resistors
 * for button functionality and sets up GPIO interrupt handlers for falling edge
 * detection. Uses the Maxim GPIO IRQ interface (max_gpio_irq_ops) for proper
 * interrupt support on the Maxim platform. Also enables the GPIO interrupt
 * controller at the NVIC level for proper interrupt handling.
 *
 * @return 0 on success, negative error code on failure
 */
int buttons_init(void);

/**
 * @brief Read button states using interrupt flags
 *
 * Checks for button press events detected by GPIO interrupt handlers.
 * This function clears the interrupt flag when a button press is detected.
 * Call this function regularly to check for button presses.
 *
 * @return Button press code:
 *         - 0: No button pressed
 *         - 1: NEXT button pressed (GPIO 0.16)
 *         - 2: BACK button pressed (GPIO 0.17)
 *         - 3: ENTER button pressed (GPIO 0.18)
 *         - 4: ON_OFF latch held LOW for > 250 ms (GPIO 0.14) - triggers shutdown
 */
int buttons_read(void);

/**
 * @brief Configure GPIOs upon system startup
 *
 * Sets up necessary GPIO pins for system operation, including enabling
 * power rails and setting initial states for control signals.
 */
void configure_gpios_upon_startup(void);

/**
 * @brief Configure GPIOs upon system shutdown
 *
 * Safely powers down the system by disabling power rails and setting
 * control signals to their safe states. Finally, triggers a system reset.
 *
 * @return 0 on success, negative error code on failure
 */
int configure_gpios_upon_shutdown();


#endif /* __BUTTONS_H__ */