/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  BASIC example for adgs6414d project
 *   @author GhisaA (Alexandru-nicolae.Ghisa@analog.com)
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
#include "adgs6414d.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_error.h"
#include "no_os_uart.h"

static void read_char(char *c)
{
	*c = getchar();
	pr_info("%c", *c);
	getchar();
	pr_info("\r\n");
}

static void print_menu(void)
{
	pr_info("\r\n");
	pr_info("ADGS6414D Switch Control Menu\r\n");
	pr_info("==============================\r\n");
	pr_info("1 - Set individual switch state\r\n");
	pr_info("2 - Get individual switch state\r\n");
	pr_info("3 - Set all switch states\r\n");
	pr_info("4 - Get all switch states\r\n");
	pr_info("5 - Software reset\r\n");
	pr_info("6 - Toggle CRC mode\r\n");
	pr_info("7 - Read error flags\r\n");
	pr_info("8 - Clear error flags\r\n");
	pr_info("h - Help\r\n");
	pr_info("q - Quit\r\n");
}

static int set_single_switch(struct adgs6414d_dev *dev)
{
	char c;
	int channel;
	bool state;
	int ret;

	pr_info("Enter switch number (1-8): ");
	read_char(&c);
	channel = c - '1';

	if (channel < 0 || channel >= ADGS6414D_NUM_SWITCHES) {
		pr_err("Invalid switch number\r\n");
		return -EINVAL;
	}

	pr_info("Enter state (1=ON/closed, 0=OFF/open): ");
	read_char(&c);
	if (c == '1')
		state = true;
	else
		state = false;

	ret = adgs6414d_set_switch(dev, channel, state);
	if (ret) {
		pr_err("Failed to set switch %d: %d\r\n", channel + 1, ret);
		return ret;
	}

	if (state)
		pr_info("Switch %d set to ON\r\n", channel + 1);
	else
		pr_info("Switch %d set to OFF\r\n", channel + 1);
	return 0;
}

static int get_single_switch(struct adgs6414d_dev *dev)
{
	char c;
	int channel;
	bool state;
	int ret;

	pr_info("Enter switch number (1-8): ");
	read_char(&c);
	channel = c - '1';

	if (channel < 0 || channel >= ADGS6414D_NUM_SWITCHES) {
		pr_err("Invalid switch number\r\n");
		return -EINVAL;
	}

	ret = adgs6414d_get_switch(dev, channel, &state);
	if (ret) {
		pr_err("Failed to get switch %d state: %d\r\n", channel + 1, ret);
		return ret;
	}

	if (state)
		pr_info("Switch %d is ON (closed)\r\n", channel + 1);
	else
		pr_info("Switch %d is OFF (open)\r\n", channel + 1);
	return 0;
}

static int set_all_switches(struct adgs6414d_dev *dev)
{
	char c;
	bool state;
	uint8_t mask;
	int ret;

	pr_info("Enter state (1=ON/closed, 0=OFF/open): ");
	read_char(&c);
	if (c == '1')
		state = true;
	else
		state = false;

	if (state)
		mask = 0xFF;
	else
		mask = 0x00;

	ret = adgs6414d_set_switches(dev, mask);
	if (ret) {
		pr_err("Failed to set all switches: %d\r\n", ret);
		return ret;
	}

	if (state)
		pr_info("All switches set to ON\r\n");
	else
		pr_info("All switches set to OFF\r\n");
	return 0;
}

static int get_all_switches(struct adgs6414d_dev *dev)
{
	uint8_t mask;
	int ret;
	int i;

	ret = adgs6414d_get_switches(dev, &mask);
	if (ret) {
		pr_err("Failed to get switch states: %d\r\n", ret);
		return ret;
	}

	pr_info("Switch states (mask=0x%02X):\r\n", mask);
	for (i = 0; i < ADGS6414D_NUM_SWITCHES; i++) {
		if (mask & (1 << i))
			pr_info("  SW%d: ON (closed)\r\n", i + 1);
		else
			pr_info("  SW%d: OFF (open)\r\n", i + 1);
	}
	return 0;
}

static int toggle_crc_mode(struct adgs6414d_dev *dev)
{
	int ret;
	bool new_state = !dev->crc_en;

	ret = adgs6414d_crc_enable(dev, new_state);
	if (ret) {
		pr_err("Failed to %s CRC: %d\r\n",
		       new_state ? "enable" : "disable", ret);
		return ret;
	}

	pr_info("CRC mode %s\r\n", new_state ? "ENABLED" : "DISABLED");
	return 0;
}

static int read_error_flags(struct adgs6414d_dev *dev)
{
	uint8_t flags;
	int ret;

	ret = adgs6414d_get_err_flags(dev, &flags);
	if (ret) {
		pr_err("Failed to read error flags: %d\r\n", ret);
		return ret;
	}

	pr_info("Error flags register: 0x%02X\r\n", flags);
	pr_info("  CRC error:  %s\r\n",
		(flags & ADGS6414D_ERR_CFG_CRC_EN) ? "YES" : "no");
	pr_info("  SCLK error: %s\r\n",
		(flags & ADGS6414D_ERR_CFG_SCLK_EN) ? "YES" : "no");
	pr_info("  R/W error:  %s\r\n",
		(flags & ADGS6414D_ERR_CFG_RW_EN) ? "YES" : "no");

	return 0;
}

static int clear_error_flags(struct adgs6414d_dev *dev)
{
	int ret;

	ret = adgs6414d_clear_err_flags(dev);
	if (ret) {
		pr_err("Failed to clear error flags: %d\r\n", ret);
		return ret;
	}

	pr_info("Error flags cleared\r\n");
	return 0;
}

/**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int example_main(void)
{
	struct adgs6414d_dev *dev;
	struct no_os_uart_desc *uart_desc;
	int ret;
	char cmd;

	ret = no_os_uart_init(&uart_desc, &adgs6414d_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = adgs6414d_init(&dev, &adgs6414d_ip);
	if (ret) {
		pr_err("Failed to initialize ADGS6414D: %d\r\n", ret);
		goto exit_uart;
	}

	pr_info("ADGS6414D initialized successfully\r\n");

	print_menu();
	while (1) {
		ret = 0;
		pr_info("\r\nSelect option: ");
		read_char(&cmd);

		switch (cmd) {
		case '1':
			ret = set_single_switch(dev);
			break;
		case '2':
			ret = get_single_switch(dev);
			break;
		case '3':
			ret = set_all_switches(dev);
			break;
		case '4':
			ret = get_all_switches(dev);
			break;
		case '5':
			ret = adgs6414d_soft_reset(dev);
			if (ret)
				pr_err("Failed to reset: %d\r\n", ret);
			else
				pr_info("Software reset completed (CRC disabled)\r\n");
			break;
		case '6':
			ret = toggle_crc_mode(dev);
			break;
		case '7':
			ret = read_error_flags(dev);
			break;
		case '8':
			ret = clear_error_flags(dev);
			break;
		case 'h':
			print_menu();
			break;
		case 'q':
			pr_info("Exiting...\r\n");
			goto exit;
		default:
			pr_info("Invalid option\r\n");
			break;
		}

		if (ret)
			goto exit;

		no_os_mdelay(1000);
	}


exit:
	adgs6414d_remove(dev);
exit_uart:
	no_os_uart_remove(uart_desc);
	if (ret)
		pr_err("Example terminated with error: %d\r\n", ret);
	return ret;
}
