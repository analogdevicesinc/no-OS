/***************************************************************************//**
 *   @file   powrms_gpios.c
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

#include "powrms_gpios.h"
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
static struct no_os_gpio_desc *btn_on_off_desc = NULL; // GPIO 0.15

// IRQ descriptors
static struct no_os_irq_ctrl_desc *btn_irq_desc = NULL;

// Button press flags (set by IRQ handlers)
static volatile uint8_t btn_pressed_flags = 0;

// Button configuration constants
#define BTN_NEXT_FLAG           (1 << 0)
#define BTN_BACK_FLAG           (1 << 1)
#define BTN_ENTER_FLAG          (1 << 2)
#define BTN_ON_OFF_FALLING      (1 << 3)  // GPIO 0.14 falling edge

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
static void btn_on_off_combined_irq_callback(void *context)
{
	uint8_t pin_state;

	int ret = no_os_gpio_get_value(btn_on_off_desc, &pin_state);
	if (ret)
		return; // Error reading pin state

	if (pin_state == NO_OS_GPIO_LOW) {
		// Pin is LOW after interrupt - this was a falling edge
		btn_pressed_flags |= BTN_ON_OFF_FALLING;
		return;
	}
}

#define BTN_ON_OFF_PIN      14  // GPIO 0.14 with rising and falling edge detection
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
	if (ret)
		return ret;

	// Initialize NEXT button (GPIO 0.16) with pull-up resistor
	struct no_os_gpio_init_param btn_next_param = {
		.port = BTN_GPIO_PORT,
		.number = BTN_NEXT_PIN,
		.pull = NO_OS_PULL_DOWN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	ret = no_os_gpio_get(&btn_next_desc, &btn_next_param);
	if (ret)
		goto error_irq_ctrl;

	ret = no_os_gpio_direction_input(btn_next_desc);
	if (ret)
		goto error_btn_next_desc;

	// Initialize BACK button (GPIO 0.17) with pull-up resistor
	struct no_os_gpio_init_param btn_back_param = {
		.port = BTN_GPIO_PORT,
		.number = BTN_BACK_PIN,
		.pull = NO_OS_PULL_DOWN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	ret = no_os_gpio_get(&btn_back_desc, &btn_back_param);
	if (ret)
		goto error_btn_next_desc;

	ret = no_os_gpio_direction_input(btn_back_desc);
	if (ret)
		goto error_btn_back_desc;

	// Initialize ENTER button (GPIO 0.18) with pull-up resistor
	struct no_os_gpio_init_param btn_enter_param = {
		.port = BTN_GPIO_PORT,
		.number = BTN_ENTER_PIN,
		.pull = NO_OS_PULL_DOWN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	ret = no_os_gpio_get(&btn_enter_desc, &btn_enter_param);
	if (ret)
		goto error_btn_back_desc;

	ret = no_os_gpio_direction_input(btn_enter_desc);
	if (ret)
		goto error_btn_enter_desc;

	// Initialize ON/OFF pin (GPIO 0.14) with pull-up resistor for both edge detection
	struct no_os_gpio_init_param btn_on_off_param = {
		.port = BTN_GPIO_PORT,
		.number = BTN_ON_OFF_PIN,
		.pull = NO_OS_PULL_NONE,  // No pull-up or pull-down resistor
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	ret = no_os_gpio_get(&btn_on_off_desc, &btn_on_off_param);
	if (ret)
		goto error_btn_enter_desc;

	ret = no_os_gpio_direction_input(btn_on_off_desc);
	if (ret)
		goto error_btn_on_off_desc;

	uint8_t pin_state;
	ret = no_os_gpio_get_value(btn_on_off_desc, &pin_state);
	if (ret)
		goto error_btn_on_off_desc;
	// Read current pin state to determine edge type
	if (pin_state == NO_OS_GPIO_LOW)
		display_entry = DISPLAY_ENTRY_SHOW;
	else if (pin_state == NO_OS_GPIO_HIGH)
		display_entry = DISPLAY_ENTRY_BLANK;

	// Register IRQ callbacks for each button (falling edge trigger)
	struct no_os_callback_desc btn_next_cb = {
		.callback = btn_next_irq_callback,
		.ctx = NULL,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = btn_next_desc
	};

	ret = no_os_irq_register_callback(btn_irq_desc, BTN_NEXT_PIN, &btn_next_cb);
	if (ret)
		goto error_btn_on_off_desc;

	struct no_os_callback_desc btn_back_cb = {
		.callback = btn_back_irq_callback,
		.ctx = NULL,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = btn_back_desc
	};

	ret = no_os_irq_register_callback(btn_irq_desc, BTN_BACK_PIN, &btn_back_cb);
	if (ret) {
		no_os_irq_global_disable(btn_irq_desc);
		goto error_btn_next_cb;

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
		no_os_irq_global_disable(btn_irq_desc);
		goto error_btn_back_cb;
	}

	// Register callback for EN_LATCH pin (GPIO 0.15) - will detect both edges
	struct no_os_callback_desc btn_on_off_cb = {
		.callback = btn_on_off_combined_irq_callback,
		.ctx = NULL,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = btn_on_off_desc
	};

	ret = no_os_irq_register_callback(btn_irq_desc, BTN_ON_OFF_PIN,
					  &btn_on_off_cb);
	if (ret) {
		no_os_irq_global_disable(btn_irq_desc);
		goto error_btn_enter_cb;
	}

	// Configure interrupt trigger level for falling edge (button press)
	ret = no_os_irq_trigger_level_set(btn_irq_desc, BTN_NEXT_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		goto error_button_cleanup;

	ret = no_os_irq_trigger_level_set(btn_irq_desc, BTN_BACK_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		goto error_button_cleanup;

	ret = no_os_irq_trigger_level_set(btn_irq_desc, BTN_ENTER_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		goto error_button_cleanup;

	// Configure interrupt trigger for ON/OFF pin to detect both rising and falling edges
	ret = no_os_irq_trigger_level_set(btn_irq_desc, BTN_ON_OFF_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		goto error_button_cleanup;

	// Enable interrupts for all buttons

	ret = no_os_irq_enable(btn_irq_desc, BTN_NEXT_PIN);
	if (ret)
		goto error_button_cleanup;
	ret = no_os_irq_enable(btn_irq_desc, BTN_BACK_PIN);
	if (ret)
		goto error_button_cleanup;

	ret = no_os_irq_enable(btn_irq_desc, BTN_ENTER_PIN);
	if (ret)
		goto error_button_cleanup;

	ret = no_os_irq_enable(btn_irq_desc, BTN_ON_OFF_PIN);
	if (ret)
		goto error_button_cleanup;

	// Manually enable NVIC interrupt for GPIO port (required for Maxim platform)
	NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(BTN_GPIO_PORT));

	// Initialize button press flags
	btn_pressed_flags = 0;

	return 0;

error_button_cleanup:
	no_os_irq_global_disable(btn_irq_desc);
	no_os_irq_disable(btn_irq_desc, BTN_ON_OFF_PIN);
	no_os_irq_unregister_callback(btn_irq_desc, BTN_ON_OFF_PIN, &btn_on_off_cb);
error_btn_enter_cb:
	no_os_irq_disable(btn_irq_desc, BTN_ENTER_PIN);
	no_os_irq_unregister_callback(btn_irq_desc, BTN_ENTER_PIN, &btn_enter_cb);
error_btn_back_cb:
	no_os_irq_disable(btn_irq_desc, BTN_BACK_PIN);
	no_os_irq_unregister_callback(btn_irq_desc, BTN_BACK_PIN, &btn_back_cb);
error_btn_next_cb:
	no_os_irq_disable(btn_irq_desc, BTN_NEXT_PIN);
	no_os_irq_unregister_callback(btn_irq_desc, BTN_NEXT_PIN, &btn_next_cb);
error_btn_on_off_desc:
	no_os_gpio_remove(btn_on_off_desc);
error_btn_enter_desc:
	no_os_gpio_remove(btn_enter_desc);
error_btn_back_desc:
	no_os_gpio_remove(btn_back_desc);
error_btn_next_desc:
	no_os_gpio_remove(btn_next_desc);
error_irq_ctrl:
	no_os_irq_ctrl_remove(btn_irq_desc);
	return ret;
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
 *         - 4: ON_OFF button falling edge (GPIO 0.14)
 */
int buttons_read(void)
{
	if (!btn_irq_desc)
		return 0; // Not initialized

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

	if (btn_pressed_flags & BTN_ON_OFF_FALLING) {
		btn_pressed_flags &= ~BTN_ON_OFF_FALLING; // Clear only this flag
		return 4; // ON_OFF button falling edge
	}

	return 0; // No button press detected
}

void configure_gpios_upon_startup()
{
	int ret;

	// Configure en_latch with PULL_DOWN and set to HIGH
	ret = no_os_gpio_get(&en_latch_gpio_desc, &en_latch_vdrive_ip);
	if (ret)
		return;
	ret = no_os_gpio_direction_output(en_latch_gpio_desc, NO_OS_GPIO_LOW);
	if (ret)
		return;

	// Set MCU_LED to HIGH
	ret = no_os_gpio_get(&mcu_led_gpio_desc, &mcu_led_ip);
	if (ret)
		return;
	ret = no_os_gpio_direction_output(mcu_led_gpio_desc, NO_OS_GPIO_HIGH);
	if (ret)
		return;

	// Set EN_ADC_5V to HIGH
	ret = no_os_gpio_get(&en_adc_5v_gpio_desc, &en_adc_5v_ip);
	if (ret)
		return;
	ret = no_os_gpio_direction_output(en_adc_5v_gpio_desc, NO_OS_GPIO_HIGH);
	if (ret)
		return;

	// Wait for ADC 5V rail to stabilize
	no_os_mdelay(ADC_5V_STABILIZE_MS);

	// Set EN_ADC_VDRIVE to HIGH
	ret = no_os_gpio_get(&en_adc_vdrive_gpio_desc, &en_adc_vdrive_ip);
	if (ret)
		return;
	ret = no_os_gpio_direction_output(en_adc_vdrive_gpio_desc, NO_OS_GPIO_HIGH);
	if (ret)
		return;

	// Wait for ADC power to stabilize
	no_os_mdelay(ADC_POWER_STABILIZE_MS);
	return;
}


int configure_gpios_upon_shutdown()
{
	int ret;

	// Set EN_ADC_VDRIVE to HIGH
	ret = no_os_gpio_direction_output(en_adc_vdrive_gpio_desc, NO_OS_GPIO_LOW);
	if (ret)
		return ret;
	// Wait for ADC power to stabilize
	no_os_mdelay(ADC_POWER_STABILIZE_MS);

	// Set EN_ADC_5V to HIGH
	ret = no_os_gpio_direction_output(en_adc_5v_gpio_desc, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	// Wait for ADC 5V rail to stabilize
	no_os_mdelay(ADC_5V_STABILIZE_MS);

	// Set MCU_LED to HIGH
	ret = no_os_gpio_direction_output(mcu_led_gpio_desc, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	// Set EN_LATCH to LOW
	ret = no_os_gpio_direction_output(en_latch_gpio_desc, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	NVIC_SystemReset();

	return 0;
}
