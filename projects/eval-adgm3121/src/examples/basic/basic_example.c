/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Source file for basic example.
 *   @author Ciobanu Radu (Radu-rares.Ciobanu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

#include "common_data.h"
#include "adgm3121.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"

/**
 * @brief Print the current state of all switches.
 *
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int print_switch_status(struct adgm3121_dev *dev)
{
	enum adgm3121_state state;
	int ret;
	int i;

	pr_info("\r\n=== Switch Status ===\r\n");
	for (i = 0; i < 4; i++) {
		ret = adgm3121_get_switch_state(dev, (enum adgm3121_switch)i,
						&state);
		if (ret) {
			pr_err("Failed to read SW%d state: %d\r\n", i + 1, ret);
			return ret;
		}
		pr_info("  SW%d: %s\r\n", i + 1,
			(state == ADGM3121_ENABLE) ? "ON (CLOSED)" : "OFF (OPEN)");
	}
	pr_info("=====================\r\n\n");

	return 0;
}

/**
 * @brief Print the menu options.
 */
static void print_menu(void)
{
	pr_info("\r\n--- ADGM3121 Switch Control ---\r\n");
	pr_info("  1 - Toggle SW1\r\n");
	pr_info("  2 - Toggle SW2\r\n");
	pr_info("  3 - Toggle SW3\r\n");
	pr_info("  4 - Toggle SW4\r\n");
	pr_info("  a - Enable ALL switches\r\n");
	pr_info("  r - Reset ALL switches (disable)\r\n");
	pr_info("  s - Show switch status\r\n");
	pr_info("  q - Quit\r\n");
	pr_info("  h - Show this menu\r\n");
	pr_info("-------------------------------\r\n");
	pr_info("Enter command: ");
}

/**
 * @brief Toggle a specific switch.
 *
 * @param dev - The device structure.
 * @param sw - The switch to toggle (0-3).
 * @return 0 in case of success, negative error code otherwise.
 */
static int toggle_switch(struct adgm3121_dev *dev, enum adgm3121_switch sw)
{
	enum adgm3121_state current_state;
	enum adgm3121_state new_state;
	int ret;

	ret = adgm3121_get_switch_state(dev, sw, &current_state);
	if (ret)
		return ret;

	new_state = (current_state == ADGM3121_ENABLE) ?
		    ADGM3121_DISABLE : ADGM3121_ENABLE;

	ret = adgm3121_set_switch_state(dev, sw, new_state);
	if (ret)
		return ret;

	pr_info("SW%d: %s -> %s\r\n", sw + 1,
		(current_state == ADGM3121_ENABLE) ? "ON" : "OFF",
		(new_state == ADGM3121_ENABLE) ? "ON" : "OFF");

	return 0;
}

/**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int example_main()
{
	struct adgm3121_dev *dev;
	struct no_os_uart_desc *uart_desc;
	int ret;
	uint8_t cmd;

	ret = no_os_uart_init(&uart_desc, &adgm3121_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	pr_info("\r\n========================================\r\n");
	pr_info("     ADGM3121 Interactive Example\r\n");
	pr_info("========================================\r\n\n");

	ret = adgm3121_init(&dev, &adgm3121_ip);
	if (ret) {
		pr_err("adgm3121_init failed with code %d\r\n", ret);
		no_os_uart_remove(uart_desc);
		return ret;
	}

	pr_info("ADGM3121 initialized successfully in GPIO parallel mode\r\n");
	pr_info("Switch control pins: P1_6 (SW1), P1_7 (SW2), P1_8 (SW3), P1_9 (SW4)\r\n");

	ret = print_switch_status(dev);
	if (ret)
		goto out;

	print_menu();

	while (1) {
		ret = no_os_uart_read(uart_desc, &cmd, 1);
		if (ret < 0)
			continue;

		if (cmd == '\r' || cmd == '\n')
			continue;

		pr_info("%c\r\n", cmd);

		switch (cmd) {
		case '1':
			ret = toggle_switch(dev, ADGM3121_SW1);
			if (ret)
				pr_err("Failed to toggle SW1: %d\r\n", ret);
			break;

		case '2':
			ret = toggle_switch(dev, ADGM3121_SW2);
			if (ret)
				pr_err("Failed to toggle SW2: %d\r\n", ret);
			break;

		case '3':
			ret = toggle_switch(dev, ADGM3121_SW3);
			if (ret)
				pr_err("Failed to toggle SW3: %d\r\n", ret);
			break;

		case '4':
			ret = toggle_switch(dev, ADGM3121_SW4);
			if (ret)
				pr_err("Failed to toggle SW4: %d\r\n", ret);
			break;

		case 'a':
		case 'A':
			ret = adgm3121_set_switches(dev, ADGM3121_SW1_EN |
						    ADGM3121_SW2_EN |
						    ADGM3121_SW3_EN |
						    ADGM3121_SW4_EN);
			if (ret)
				pr_err("Failed to enable all switches: %d\r\n", ret);
			else
				pr_info("All switches ENABLED\r\n");
			break;

		case 'r':
		case 'R':
			ret = adgm3121_reset_switches(dev);
			if (ret)
				pr_err("Failed to reset switches: %d\r\n", ret);
			else
				pr_info("All switches DISABLED\r\n");
			break;

		case 's':
		case 'S':
			print_switch_status(dev);
			break;

		case 'h':
		case 'H':
		case '?':
			print_menu();
			break;

		case 'q':
		case 'Q':
			pr_info("Exiting...\r\n");
			goto out;

		default:
			pr_info("Unknown command '%c'. Press 'h' for help.\r\n", cmd);
			break;
		}

		pr_info("\r\nEnter command: ");
	}

out:
	adgm3121_remove(dev);
	no_os_uart_remove(uart_desc);
	if (ret)
		pr_err("Terminated with error: %d\r\n", ret);

	return ret;
}
