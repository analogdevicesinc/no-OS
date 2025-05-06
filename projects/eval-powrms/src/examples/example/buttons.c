/***************************************************************************//**
 *   @file   buttons.c
 *   @brief  Button handling implementation for eval-powrms project
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

#include "buttons.h"
#include "parameters.h"
#include "no_os_gpio.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_irq.h"
#include "maxim_gpio_irq.h"
#include "example.h"

// Button GPIO descriptors
static struct no_os_gpio_desc *btn_next_desc = NULL;    // GPIO 0.16
static struct no_os_gpio_desc *btn_back_desc = NULL;    // GPIO 0.17
static struct no_os_gpio_desc *btn_enter_desc = NULL;   // GPIO 0.18
static struct no_os_gpio_desc *btn_en_latch_desc = NULL; // GPIO 0.15

// IRQ descriptors
static struct no_os_irq_ctrl_desc *btn_irq_desc = NULL;

// Button press flags (set by IRQ handlers)
static volatile uint8_t btn_pressed_flags = 0;

// Button configuration constants
#define BTN_NEXT_FLAG         (1 << 0)
#define BTN_BACK_FLAG         (1 << 1)
#define BTN_ENTER_FLAG        (1 << 2)
#define BTN_EN_LATCH_RISING    (1 << 3)  // GPIO 0.15 rising edge
#define BTN_EN_LATCH_FALLING   (1 << 4)  // GPIO 0.15 falling edge

/**
 * @brief IRQ callback for NEXT button (GPIO 0.16)
 *
 * This function is called when the NEXT button generates an interrupt.
 * It sets the appropriate flag to indicate a button press.
 *
 * @param context Callback context (unused)
 */
static void btn_next_irq_callback(void *context)
{
	(void)context; // Unused parameter
	btn_pressed_flags |= BTN_NEXT_FLAG;

}

/**
 * @brief IRQ callback for BACK button (GPIO 0.17)
 *
 * This function is called when the BACK button generates an interrupt.
 * It sets the appropriate flag to indicate a button press.
 *
 * @param context Callback context (unused)
 */
static void btn_back_irq_callback(void *context)
{
	(void)context; // Unused parameter
	btn_pressed_flags |= BTN_BACK_FLAG;
}

/**
 * @brief IRQ callback for ENTER button (GPIO 0.18)
 *
 * This function is called when the ENTER button generates an interrupt.
 * It sets the appropriate flag to indicate a button press.
 *
 * @param context Callback context (unused)
 */
static void btn_enter_irq_callback(void *context)
{
	(void)context; // Unused parameter
	btn_pressed_flags |= BTN_ENTER_FLAG;
}

/**
 * @brief Combined IRQ callback for SPECIAL pin (GPIO 0.15)
 *
 * This function is called when GPIO 0.15 generates either a rising or falling edge interrupt.
 * It reads the current pin state to determine which edge occurred and sets the appropriate flag.
 *
 * @param context Callback context (unused)
 */
static void btn_en_latch_combined_irq_callback(void *context)
{
	(void)context; // Unused parameter
	uint8_t pin_state;

	int ret = no_os_gpio_get_value(btn_en_latch_desc, &pin_state);
	if (ret) {
		return; // Error reading pin state
	}

	// Read current pin state to determine edge type
	if (pin_state == NO_OS_GPIO_HIGH) {
		// Pin is HIGH after interrupt - this was a rising edge
		btn_pressed_flags |= BTN_EN_LATCH_RISING;
		return;
	}
	if (pin_state == NO_OS_GPIO_LOW) {
		// Pin is LOW after interrupt - this was a falling edge
		btn_pressed_flags |= BTN_EN_LATCH_FALLING;
		return;
	}
}

/**
 * @brief IRQ callback for SPECIAL pin rising edge (GPIO 0.15)
 *
 * This function is called when GPIO 0.15 generates a rising edge interrupt.
 * It sets the appropriate flag to indicate a rising edge event.
 *
 * @param context Callback context (unused)
 */
static void __attribute__((unused)) btn_en_latch_rising_irq_callback(
	void *context)
{
	(void)context; // Unused parameter
	btn_pressed_flags |= BTN_EN_LATCH_RISING;
}

/**
 * @brief IRQ callback for SPECIAL pin falling edge (GPIO 0.15)
 *
 * This function is called when GPIO 0.15 generates a falling edge interrupt.
 * It sets the appropriate flag to indicate a falling edge event.
 *
 * @param context Callback context (unused)
 */
static void __attribute__((unused)) btn_en_latch_falling_irq_callback(
	void *context)
{
	(void)context; // Unused parameter
	btn_pressed_flags |= BTN_EN_LATCH_FALLING;
}

#define BTN_EN_LATCH_PIN    15  // GPIO 0.15 with rising and falling edge detection
#define BTN_BACK_PIN        16
#define BTN_NEXT_PIN        17
#define BTN_ENTER_PIN       18
#define BTN_GPIO_PORT       0

/**
 * @brief Initialize button GPIO pins
 *
 * Configures GPIO pins 0.16, 0.17, and 0.18 as input pins with pull-up resistors
 * for button functionality (NEXT, BACK, ENTER).
 *
 * @return 0 on success, negative error code on failure
 */
int buttons_init(void)
{
	int ret;

	// Initialize GPIO IRQ controller for GPIO interrupts
	struct no_os_irq_init_param btn_irq_param = {
		.irq_ctrl_id = BTN_GPIO_PORT,  // GPIO port 0 (not an IRQ number)
		.platform_ops = &max_gpio_irq_ops,
		.extra = NULL
	};

	ret = no_os_irq_ctrl_init(&btn_irq_desc, &btn_irq_param);
	if (ret) {
		return ret;
	}

	// Initialize NEXT button (GPIO 0.16) with pull-up resistor
	struct no_os_gpio_init_param btn_next_param = {
		.port = BTN_GPIO_PORT,
		.number = BTN_NEXT_PIN,
		.pull = NO_OS_PULL_DOWN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	ret = no_os_gpio_get(&btn_next_desc, &btn_next_param);
	if (ret) {
		no_os_irq_ctrl_remove(btn_irq_desc);
		return ret;
	}

	ret = no_os_gpio_direction_input(btn_next_desc);
	if (ret) {
		no_os_gpio_remove(btn_next_desc);
		no_os_irq_ctrl_remove(btn_irq_desc);
		return ret;
	}

	// Initialize BACK button (GPIO 0.17) with pull-up resistor
	struct no_os_gpio_init_param btn_back_param = {
		.port = BTN_GPIO_PORT,
		.number = BTN_BACK_PIN,
		.pull = NO_OS_PULL_DOWN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	ret = no_os_gpio_get(&btn_back_desc, &btn_back_param);
	if (ret) {
		no_os_gpio_remove(btn_next_desc);
		no_os_irq_ctrl_remove(btn_irq_desc);
		return ret;
	}

	ret = no_os_gpio_direction_input(btn_back_desc);
	if (ret) {
		no_os_gpio_remove(btn_next_desc);
		no_os_gpio_remove(btn_back_desc);
		no_os_irq_ctrl_remove(btn_irq_desc);
		return ret;
	}

	// Initialize ENTER button (GPIO 0.18) with pull-up resistor
	struct no_os_gpio_init_param btn_enter_param = {
		.port = BTN_GPIO_PORT,
		.number = BTN_ENTER_PIN,
		.pull = NO_OS_PULL_DOWN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	ret = no_os_gpio_get(&btn_enter_desc, &btn_enter_param);
	if (ret) {
		no_os_gpio_remove(btn_next_desc);
		no_os_gpio_remove(btn_back_desc);
		no_os_irq_ctrl_remove(btn_irq_desc);
		return ret;
	}

	ret = no_os_gpio_direction_input(btn_enter_desc);
	if (ret) {
		no_os_gpio_remove(btn_next_desc);
		no_os_gpio_remove(btn_back_desc);
		no_os_gpio_remove(btn_enter_desc);
		no_os_irq_ctrl_remove(btn_irq_desc);
		return ret;
	}

	// Initialize SPECIAL pin (GPIO 0.15) with pull-up resistor for both edge detection
	struct no_os_gpio_init_param btn_en_latch_param = {
		.port = BTN_GPIO_PORT,
		.number = BTN_EN_LATCH_PIN,
		.pull = NO_OS_PULL_UP,  // Using pull-up for both edge detection
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	ret = no_os_gpio_get(&btn_en_latch_desc, &btn_en_latch_param);
	if (ret) {
		no_os_gpio_remove(btn_next_desc);
		no_os_gpio_remove(btn_back_desc);
		no_os_gpio_remove(btn_enter_desc);
		no_os_irq_ctrl_remove(btn_irq_desc);
		return ret;
	}

	ret = no_os_gpio_direction_input(btn_en_latch_desc);
	if (ret) {
		no_os_gpio_remove(btn_next_desc);
		no_os_gpio_remove(btn_back_desc);
		no_os_gpio_remove(btn_enter_desc);
		no_os_gpio_remove(btn_en_latch_desc);
		no_os_irq_ctrl_remove(btn_irq_desc);
		return ret;
	}

	uint8_t pin_state;
	ret = no_os_gpio_get_value(btn_en_latch_desc, &pin_state);
	if (ret) {
		return ret; // Error reading pin state
	}
	// Read current pin state to determine edge type
	if (pin_state == NO_OS_GPIO_LOW) {
		display_entry = DISPLAY_INPUT_FREQUENCY;
	} else if (pin_state == NO_OS_GPIO_HIGH) {
		display_entry = DISPLAY_ENTRY_BLANK;
	}

	// Register IRQ callbacks for each button (falling edge trigger)
	struct no_os_callback_desc btn_next_cb = {
		.callback = btn_next_irq_callback,
		.ctx = NULL,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = btn_next_desc
	};

	ret = no_os_irq_register_callback(btn_irq_desc, BTN_NEXT_PIN, &btn_next_cb);
	if (ret) {
		no_os_gpio_remove(btn_next_desc);
		// no_os_gpio_remove(btn_back_desc);
		// no_os_gpio_remove(btn_enter_desc);
		no_os_irq_ctrl_remove(btn_irq_desc);
		return ret;
	}

	struct no_os_callback_desc btn_back_cb = {
		.callback = btn_back_irq_callback,
		.ctx = NULL,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = btn_back_desc
	};

	ret = no_os_irq_register_callback(btn_irq_desc, BTN_BACK_PIN, &btn_back_cb);
	if (ret) {
		no_os_irq_unregister_callback(btn_irq_desc, BTN_NEXT_PIN, &btn_next_cb);
		no_os_gpio_remove(btn_next_desc);
		no_os_gpio_remove(btn_back_desc);
		no_os_gpio_remove(btn_enter_desc);
		no_os_irq_ctrl_remove(btn_irq_desc);
		return ret;
	}

	struct no_os_callback_desc btn_enter_cb = {
		.callback = btn_enter_irq_callback,
		.ctx = NULL,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = btn_enter_desc
	};

	ret = no_os_irq_register_callback(btn_irq_desc, BTN_ENTER_PIN, &btn_enter_cb);
	if (ret) {
		no_os_irq_unregister_callback(btn_irq_desc, BTN_NEXT_PIN, &btn_next_cb);
		no_os_irq_unregister_callback(btn_irq_desc, BTN_BACK_PIN, &btn_back_cb);
		no_os_gpio_remove(btn_next_desc);
		no_os_gpio_remove(btn_back_desc);
		no_os_gpio_remove(btn_enter_desc);
		no_os_irq_ctrl_remove(btn_irq_desc);
		return ret;
	}

	// Register callback for EN_LATCH pin (GPIO 0.15) - will detect both edges
	struct no_os_callback_desc btn_en_latch_cb = {
		.callback = btn_en_latch_combined_irq_callback,
		.ctx = NULL,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = btn_en_latch_desc
	};

	ret = no_os_irq_register_callback(btn_irq_desc, BTN_EN_LATCH_PIN,
					  &btn_en_latch_cb);
	if (ret) {
		no_os_irq_unregister_callback(btn_irq_desc, BTN_NEXT_PIN, &btn_next_cb);
		no_os_irq_unregister_callback(btn_irq_desc, BTN_BACK_PIN, &btn_back_cb);
		no_os_irq_unregister_callback(btn_irq_desc, BTN_ENTER_PIN, &btn_enter_cb);
		no_os_gpio_remove(btn_next_desc);
		no_os_gpio_remove(btn_back_desc);
		no_os_gpio_remove(btn_enter_desc);
		no_os_gpio_remove(btn_en_latch_desc);
		no_os_irq_ctrl_remove(btn_irq_desc);
		return ret;
	}

	// Configure interrupt trigger level for falling edge (button press)
	ret = no_os_irq_trigger_level_set(btn_irq_desc, BTN_NEXT_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret) {
		buttons_cleanup();
		return ret;
	}

	ret = no_os_irq_trigger_level_set(btn_irq_desc, BTN_BACK_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret) {
		buttons_cleanup();
		return ret;
	}

	ret = no_os_irq_trigger_level_set(btn_irq_desc, BTN_ENTER_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret) {
		buttons_cleanup();
		return ret;
	}

	// Configure interrupt trigger for SPECIAL pin to detect both rising and falling edges
	ret = no_os_irq_trigger_level_set(btn_irq_desc, BTN_EN_LATCH_PIN,
					  NO_OS_IRQ_EDGE_BOTH);
	if (ret) {
		buttons_cleanup();
		return ret;
	}

	// Enable interrupts for all buttons

	ret = no_os_irq_enable(btn_irq_desc, BTN_NEXT_PIN);
	if (ret) {
		buttons_cleanup();
		return ret;
	}

	ret = no_os_irq_enable(btn_irq_desc, BTN_BACK_PIN);
	if (ret) {
		buttons_cleanup();
		return ret;
	}

	ret = no_os_irq_enable(btn_irq_desc, BTN_ENTER_PIN);
	if (ret) {
		buttons_cleanup();
		return ret;
	}

	ret = no_os_irq_enable(btn_irq_desc, BTN_EN_LATCH_PIN);
	if (ret) {
		buttons_cleanup();
		return ret;
	}

	// Manually enable NVIC interrupt for GPIO port (required for Maxim platform)
	NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(BTN_GPIO_PORT));

	// Initialize button press flags
	btn_pressed_flags = 0;

	return 0;
}

/**
 * @brief Read button states with debouncing
 *
 * Reads the current state of all buttons and applies debouncing logic
 * to prevent false triggers from mechanical switch bounce.
 *
 * @return Button press code:
 *         - 0: No button pressed
 *         - 1: NEXT button pressed (GPIO 0.17)
 *         - 2: BACK button pressed (GPIO 0.16)
 *         - 3: ENTER button pressed (GPIO 0.18)
 *         - 4: SPECIAL pin rising edge (GPIO 0.15)
 *         - 5: SPECIAL pin falling edge (GPIO 0.15)
 */
int buttons_read(void)
{
	if (!btn_irq_desc) {
		return 0; // Not initialized
	}

	// Check interrupt flags set by IRQ handlers
	if (btn_pressed_flags & BTN_NEXT_FLAG) {
		btn_pressed_flags &= ~BTN_NEXT_FLAG; // Clear only this flag
		return 1; // NEXT button pressed
	}

	if (btn_pressed_flags & BTN_BACK_FLAG) {
		btn_pressed_flags &= ~BTN_BACK_FLAG; // Clear only this flag
		return 2; // BACK button pressed
	}

	if (btn_pressed_flags & BTN_ENTER_FLAG) {
		btn_pressed_flags &= ~BTN_ENTER_FLAG; // Clear only this flag
		return 3; // ENTER button pressed
	}

	if (btn_pressed_flags & BTN_EN_LATCH_RISING) {
		btn_pressed_flags &= ~BTN_EN_LATCH_RISING; // Clear only this flag
		return 4; // SPECIAL pin rising edge
	}

	if (btn_pressed_flags & BTN_EN_LATCH_FALLING) {
		btn_pressed_flags &= ~BTN_EN_LATCH_FALLING; // Clear only this flag
		return 5; // SPECIAL pin falling edge
	}

	return 0; // No button press detected
}

/**
 * @brief Clean up button resources
 *
 * Releases GPIO resources allocated for button handling.
 * Should be called when button functionality is no longer needed.
 */
void buttons_cleanup(void)
{
	// Disable interrupts first
	if (btn_irq_desc) {
		// Manually disable NVIC interrupt for GPIO port
		NVIC_DisableIRQ(MXC_GPIO_GET_IRQ(BTN_GPIO_PORT));

		// Disable global GPIO interrupt controller (NVIC level)
		no_os_irq_global_disable(btn_irq_desc);

		no_os_irq_disable(btn_irq_desc, BTN_NEXT_PIN);
		no_os_irq_disable(btn_irq_desc, BTN_BACK_PIN);
		no_os_irq_disable(btn_irq_desc, BTN_ENTER_PIN);
		no_os_irq_disable(btn_irq_desc, BTN_EN_LATCH_PIN);

		// Unregister callbacks
		struct no_os_callback_desc btn_next_cb = {
			.callback = btn_next_irq_callback,
			.ctx = NULL,
			.event = NO_OS_EVT_GPIO,
			.peripheral = NO_OS_GPIO_IRQ,
			.handle = btn_next_desc
		};

		struct no_os_callback_desc btn_back_cb = {
			.callback = btn_back_irq_callback,
			.ctx = NULL,
			.event = NO_OS_EVT_GPIO,
			.peripheral = NO_OS_GPIO_IRQ,
			.handle = btn_back_desc
		};

		struct no_os_callback_desc btn_enter_cb = {
			.callback = btn_enter_irq_callback,
			.ctx = NULL,
			.event = NO_OS_EVT_GPIO,
			.peripheral = NO_OS_GPIO_IRQ,
			.handle = btn_enter_desc
		};

		no_os_irq_unregister_callback(btn_irq_desc, BTN_NEXT_PIN, &btn_next_cb);
		no_os_irq_unregister_callback(btn_irq_desc, BTN_BACK_PIN, &btn_back_cb);
		no_os_irq_unregister_callback(btn_irq_desc, BTN_ENTER_PIN, &btn_enter_cb);

		// Remove IRQ controller
		no_os_irq_ctrl_remove(btn_irq_desc);
		btn_irq_desc = NULL;
	}

	// Clean up GPIO descriptors
	if (btn_next_desc) {
		no_os_gpio_remove(btn_next_desc);
		btn_next_desc = NULL;
	}

	if (btn_back_desc) {
		no_os_gpio_remove(btn_back_desc);
		btn_back_desc = NULL;
	}

	if (btn_enter_desc) {
		no_os_gpio_remove(btn_enter_desc);
		btn_enter_desc = NULL;
	}

	// Clear button flags
	btn_pressed_flags = 0;
}
