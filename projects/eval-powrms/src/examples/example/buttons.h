/***************************************************************************//**
 *   @file   buttons.h
 *   @brief  Button handling definitions and declarations.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include "no_os_gpio.h"

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
 *         - 4: SPECIAL pin rising edge (GPIO 0.15)
 *         - 5: SPECIAL pin falling edge (GPIO 0.15) - triggers blank screen
 */
int buttons_read(void);

/**
 * @brief Clean up button interrupt resources
 *
 * Disables GPIO interrupts, unregisters callbacks, and releases GPIO and IRQ
 * controller resources. Should be called when button functionality is no longer needed.
 */
void buttons_cleanup(void);

#endif /* __BUTTONS_H__ */