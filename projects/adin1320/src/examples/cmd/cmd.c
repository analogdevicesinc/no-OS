/***************************************************************************//**
*   @file   cmd.c
*   @brief  Implementation of terminal command control for ADIN1320
*   @author Johnson Ralph Perez (Johnsonralph.Perez@analog.com)
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "adin1320.h"
#include "common_data.h"
#include "mdio_spi.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_mdio.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/* From cmd.h */
#define FW_VERSION_MAJOR	1
#define FW_VERSION_MINOR	0
#define FW_VERSION_PATCH	0
#define EVAL_BOARD_NAME		"EVAL-ADIN1320FMCZ"

struct cmd_config {
	uint8_t fw_version_major;
	uint8_t fw_version_minor;
	uint8_t fw_version_patch;
	char eval_board_name[30];
	uint8_t fw_mode;
};

/* From cmd_list.h */
#define CMD_PARAM_EMPTY	-1
#define CMD_BUFFER_SIZE	100
#define CMD_PARAM_SIZE	10

enum cmd_type {
	CMD_TYPE_NONE = 0,
	CMD_TYPE_MDIO_READ,
	CMD_TYPE_MDIO_WRITE,
	CMD_TYPE_HELP,
	CMD_TYPE_RESET,
	CMD_TYPE_MAX
};

struct cmd_input {
	uint8_t cmd[CMD_BUFFER_SIZE];
	bool is_cmd_available;
	int32_t cmd_param[CMD_PARAM_SIZE];
	bool is_cmd_processed;
};

/* From cmd_mode.h */
struct cmd_gpio {
	struct no_os_gpio_desc *gpio_uc_cfg0_desc;
	struct no_os_gpio_desc *gpio_uc_cfg1_desc;
	struct no_os_gpio_desc *gpio_uc_cfg2_desc;
	struct no_os_gpio_desc *gpio_uc_cfg3_desc;
	struct no_os_gpio_desc *gpio_led_green_desc;
	struct no_os_gpio_desc *gpio_led_red_desc;
	struct no_os_gpio_desc *gpio_led_yellow_desc;
	struct no_os_gpio_desc *gpio_led_blue_desc;
};

/* Forward declarations from cmd_list.c */
void cmd_print_list(struct cmd_input *cmd_input);
void cmd_mdio_read(struct adin1320_desc *dev, struct cmd_input *cmd_input);
void cmd_mdio_write(struct adin1320_desc *dev, struct cmd_input *cmd_input);
void cmd_reset(struct adin1320_desc *dev_0, struct adin1320_desc *dev_8,
	       struct cmd_input *cmd_input, bool dev_init[2]);

/* Forward declarations from cmd_mode.c */
int device_init(struct adin1320_desc **dev_0, struct adin1320_desc **dev_8,
		bool dev_init[2]);
int fw_mode_sgmii_path_test(struct cmd_gpio *cmd_gpio,
			    struct adin1320_desc *dev_0,
			    struct adin1320_desc *dev_8);
int fw_mode_mdi_path_test(struct cmd_gpio *cmd_gpio,
			  struct adin1320_desc *dev_0,
			  struct adin1320_desc *dev_8);

/* Check UART for input commands */
void get_cmd(struct no_os_uart_desc *uart_desc, struct cmd_input *cmd_input)
{
	uint8_t cmdi = 0;
	uint8_t temp_cmd_buffer[CMD_BUFFER_SIZE] = {0};

	do {
		no_os_uart_read(uart_desc, &temp_cmd_buffer[cmdi], 1);
		if ((temp_cmd_buffer[cmdi] == '\0') ||
		    (temp_cmd_buffer[cmdi] == '\r'))
			continue;

		if (cmdi < CMD_BUFFER_SIZE - 1) {
			cmd_input->cmd[cmdi] = temp_cmd_buffer[cmdi];
		} else {
			cmd_input->cmd[CMD_BUFFER_SIZE - 1] = '\0';
			cmd_input->is_cmd_available = true;
			break;
		}

		if (cmd_input->cmd[cmdi] == '\n')
			cmd_input->is_cmd_available = true;
		else
			cmdi++;
	} while (!cmd_input->is_cmd_available);
}

/* Read command to determine process to perform */
void select_cmd(struct cmd_input *cmd_input)
{
	char cmd_keyword[16];
	unsigned int phy_add;
	unsigned int reg_add;
	unsigned int reg_val;
	unsigned int input;
	char *cmd_str = (char *)cmd_input->cmd;

	if ((strcmp(cmd_str, "?\n") == 0) ||
	    (strcmp(cmd_str, "help\n") == 0)) {
		cmd_input->cmd_param[0] = CMD_TYPE_HELP;
		return;
	}

	if (sscanf(cmd_str, "%15s", cmd_keyword) != 1) {
		cmd_input->cmd_param[0] = CMD_TYPE_NONE;
		return;
	}

	if ((strcmp(cmd_keyword, "mdioread") == 0) ||
	    (strcmp(cmd_keyword, "mdiord_cl45") == 0)) {
		if (sscanf(cmd_str + strlen(cmd_keyword),
			   " %x,%x", &phy_add, &reg_add) == 2) {
			cmd_input->cmd_param[0] = CMD_TYPE_MDIO_READ;
			cmd_input->cmd_param[1] = (uint8_t) phy_add;
			cmd_input->cmd_param[2] = (uint32_t) reg_add;
			return;
		}
	} else if ((strcmp(cmd_keyword, "mdiowrite") == 0) ||
		   (strcmp(cmd_keyword, "mdiowr_cl45") == 0)) {
		if (sscanf(cmd_str + strlen(cmd_keyword),
			   " %x,%x,%x", &phy_add, &reg_add, &reg_val) == 3) {
			cmd_input->cmd_param[0] = CMD_TYPE_MDIO_WRITE;
			cmd_input->cmd_param[1] = (uint8_t) phy_add;
			cmd_input->cmd_param[2] = (uint32_t) reg_add;
			cmd_input->cmd_param[3] = (uint16_t) reg_val;
			return;
		}
	} else if (strcmp(cmd_keyword, "reset") == 0) {
		if (sscanf(cmd_str + strlen(cmd_keyword),
			   " %d", &input) == 1) {
			cmd_input->cmd_param[0] = CMD_TYPE_RESET;
			cmd_input->cmd_param[1] = input;
			return;
		}
	}

	cmd_input->cmd_param[0] = CMD_TYPE_NONE;
}

/* Select and perform command process */
void process_cmd(struct adin1320_desc *dev_0, struct adin1320_desc *dev_8,
		 struct cmd_input *cmd_input, bool dev_init[2])
{
	switch (cmd_input->cmd_param[0]) {
	case CMD_TYPE_HELP:
		cmd_print_list(cmd_input);
		break;
	case CMD_TYPE_MDIO_READ:
		if (cmd_input->cmd_param[1] == ADIN1320_8_PHY_ADDR &&
		    dev_init[1])
			cmd_mdio_read(dev_8, cmd_input);
		else if (dev_init[0])
			cmd_mdio_read(dev_0, cmd_input);
		else
			pr_err("Error: No device initialized.\n");
		break;
	case CMD_TYPE_MDIO_WRITE:
		if (cmd_input->cmd_param[1] == ADIN1320_8_PHY_ADDR &&
		    dev_init[1])
			cmd_mdio_write(dev_8, cmd_input);
		else if (dev_init[0])
			cmd_mdio_write(dev_0, cmd_input);
		else
			pr_err("Error: No device initialized.\n");
		break;
	case CMD_TYPE_RESET:
		cmd_reset(dev_0, dev_8, cmd_input, dev_init);
		break;
	default:
		pr_err("Error: Invalid input.\n");
		cmd_input->is_cmd_processed = true;
		break;
	}

	if (cmd_input->is_cmd_processed) {
		int param_idx;

		for (param_idx = 0; param_idx < CMD_PARAM_SIZE; param_idx++)
			cmd_input->cmd_param[param_idx] = CMD_PARAM_EMPTY;

		cmd_input->is_cmd_processed = false;
	}
}

/* Initialize board peripherals */
int board_init(struct no_os_uart_desc **uart_desc,
	       struct cmd_config *cmd_cfg,
	       struct cmd_gpio **cmd_gpio)
{
	int ret;
	uint8_t gpio_uc_cfg0_val;
	uint8_t gpio_uc_cfg1_val;
	uint8_t gpio_uc_cfg2_val;
	uint8_t gpio_uc_cfg3_val;

	/* UART Initialization */
	ret = no_os_uart_init(uart_desc, &uart_ip);
	if (ret) {
		pr_err("UART Initialization failed!\n");
		return ret;
	}

	no_os_uart_stdio(*uart_desc);

	struct cmd_gpio *temp_cmd_gpio = (struct cmd_gpio *) no_os_calloc(1,
					 sizeof(*temp_cmd_gpio));
	if (!temp_cmd_gpio) {
		ret = -ENOMEM;
		goto remove_uart;
	}

	/* GPIO Initialization for Config Switches */
	ret = no_os_gpio_get(&temp_cmd_gpio->gpio_uc_cfg0_desc,
			     &gpio_uc_cfg0_ip);
	if (ret) {
		pr_err("GPIO CFG0 Initialization failed!\n");
		goto remove_calloc;
	}

	ret = no_os_gpio_direction_input(temp_cmd_gpio->gpio_uc_cfg0_desc);
	if (ret) {
		pr_err("GPIO CFG0 Direction Input failed!\n");
		goto remove_cfg0;
	}

	ret = no_os_gpio_get_value(temp_cmd_gpio->gpio_uc_cfg0_desc,
				   &gpio_uc_cfg0_val);
	if (ret) {
		pr_err("GPIO CFG0 Get Direction Input failed!\n");
		goto remove_cfg0;
	}

	ret = no_os_gpio_get(&temp_cmd_gpio->gpio_uc_cfg1_desc,
			     &gpio_uc_cfg1_ip);
	if (ret) {
		pr_err("GPIO CFG1 Initialization failed!\n");
		goto remove_cfg0;
	}

	ret = no_os_gpio_direction_input(temp_cmd_gpio->gpio_uc_cfg1_desc);
	if (ret) {
		pr_err("GPIO CFG1 Direction Input failed!\n");
		goto remove_cfg1;
	}

	ret = no_os_gpio_get_value(temp_cmd_gpio->gpio_uc_cfg1_desc,
				   &gpio_uc_cfg1_val);
	if (ret) {
		pr_err("GPIO CFG1 Get Direction Input failed!\n");
		goto remove_cfg1;
	}

	ret = no_os_gpio_get(&temp_cmd_gpio->gpio_uc_cfg2_desc,
			     &gpio_uc_cfg2_ip);
	if (ret) {
		pr_err("GPIO CFG2 Initialization failed!\n");
		goto remove_cfg1;
	}

	ret = no_os_gpio_direction_input(temp_cmd_gpio->gpio_uc_cfg2_desc);
	if (ret) {
		pr_err("GPIO CFG2 Direction Input failed!\n");
		goto remove_cfg2;
	}

	ret = no_os_gpio_get_value(temp_cmd_gpio->gpio_uc_cfg2_desc,
				   &gpio_uc_cfg2_val);
	if (ret) {
		pr_err("GPIO CFG2 Get Direction Input failed!\n");
		goto remove_cfg2;
	}

	ret = no_os_gpio_get(&temp_cmd_gpio->gpio_uc_cfg3_desc,
			     &gpio_uc_cfg3_ip);
	if (ret) {
		pr_err("GPIO CFG3 Initialization failed!\n");
		goto remove_cfg2;
	}

	ret = no_os_gpio_direction_input(temp_cmd_gpio->gpio_uc_cfg3_desc);
	if (ret) {
		pr_err("GPIO CFG3 Direction Input failed!\n");
		goto remove_cfg3;
	}

	ret = no_os_gpio_get_value(temp_cmd_gpio->gpio_uc_cfg3_desc,
				   &gpio_uc_cfg3_val);
	if (ret) {
		pr_err("GPIO CFG3 Get Direction Input failed!\n");
		goto remove_cfg3;
	}

	/* GPIO Initialization for LEDs */
	ret = no_os_gpio_get(&temp_cmd_gpio->gpio_led_green_desc,
			     &gpio_led_green_ip);
	if (ret) {
		pr_err("GPIO LED Green Initialization failed!\n");
		goto remove_cfg3;
	}

	ret = no_os_gpio_direction_output(temp_cmd_gpio->gpio_led_green_desc,
					  NO_OS_GPIO_HIGH);
	if (ret) {
		pr_err("GPIO LED Green Direction Output failed!\n");
		goto remove_led_green;
	}

	ret = no_os_gpio_get(&temp_cmd_gpio->gpio_led_red_desc,
			     &gpio_led_red_ip);
	if (ret) {
		pr_err("GPIO LED Red Initialization failed!\n");
		goto remove_led_green;
	}

	ret = no_os_gpio_direction_output(temp_cmd_gpio->gpio_led_red_desc,
					  NO_OS_GPIO_HIGH);
	if (ret) {
		pr_err("GPIO LED Red Direction Output failed!\n");
		goto remove_led_red;
	}

	ret = no_os_gpio_get(&temp_cmd_gpio->gpio_led_yellow_desc,
			     &gpio_led_yellow_ip);
	if (ret) {
		pr_err("GPIO LED Yellow Initialization failed!\n");
		goto remove_led_red;
	}

	ret = no_os_gpio_direction_output(temp_cmd_gpio->gpio_led_yellow_desc,
					  NO_OS_GPIO_HIGH);
	if (ret) {
		pr_err("GPIO LED Yellow Direction Output failed!\n");
		goto remove_led_yellow;
	}

	ret = no_os_gpio_get(&temp_cmd_gpio->gpio_led_blue_desc,
			     &gpio_led_blue_ip);
	if (ret) {
		pr_err("GPIO LED Blue Initialization failed!\n");
		goto remove_led_yellow;
	}

	ret = no_os_gpio_direction_output(temp_cmd_gpio->gpio_led_blue_desc,
					  NO_OS_GPIO_HIGH);
	if (ret) {
		pr_err("GPIO LED Blue Direction Output failed!\n");
		goto remove_led_blue;
	}

	*cmd_gpio = temp_cmd_gpio;
	cmd_cfg->fw_mode = (gpio_uc_cfg3_val << 3) |
			   (gpio_uc_cfg2_val << 2) |
			   (gpio_uc_cfg1_val << 1) |
			   (gpio_uc_cfg0_val << 0);

	cmd_cfg->fw_version_major = FW_VERSION_MAJOR;
	cmd_cfg->fw_version_minor = FW_VERSION_MINOR;
	cmd_cfg->fw_version_patch = FW_VERSION_PATCH;
	strncpy(cmd_cfg->eval_board_name, EVAL_BOARD_NAME,
		sizeof(cmd_cfg->eval_board_name) - 1);
	cmd_cfg->eval_board_name[sizeof(cmd_cfg->eval_board_name) - 1] = '\0';

	return 0;

remove_led_blue:
	no_os_gpio_remove(temp_cmd_gpio->gpio_led_blue_desc);
remove_led_yellow:
	no_os_gpio_remove(temp_cmd_gpio->gpio_led_yellow_desc);
remove_led_red:
	no_os_gpio_remove(temp_cmd_gpio->gpio_led_red_desc);
remove_led_green:
	no_os_gpio_remove(temp_cmd_gpio->gpio_led_green_desc);
remove_cfg3:
	no_os_gpio_remove(temp_cmd_gpio->gpio_uc_cfg3_desc);
remove_cfg2:
	no_os_gpio_remove(temp_cmd_gpio->gpio_uc_cfg2_desc);
remove_cfg1:
	no_os_gpio_remove(temp_cmd_gpio->gpio_uc_cfg1_desc);
remove_cfg0:
	no_os_gpio_remove(temp_cmd_gpio->gpio_uc_cfg0_desc);
remove_calloc:
	no_os_free(temp_cmd_gpio);
remove_uart:
	no_os_uart_remove(*uart_desc);
	return ret;
}

/* Initialize firmware mode */
int fw_mode_init(struct cmd_config cmd_cfg, struct cmd_gpio *cmd_gpio,
		 struct adin1320_desc **dev_0, struct adin1320_desc **dev_8,
		 bool *dev_init)
{
	int ret;

	pr_info("================================================\n");
	pr_info("ANALOG DEVICES Ethernet Phy Eval Firmware\n");
	pr_info("================================================\n");
	pr_info("(c) 2025 Analog Devices Inc. All rights reserved\n");
	pr_info("================================================\n");
	pr_info("Firmware ver.: %d.%d.%d\n", cmd_cfg.fw_version_major,
		cmd_cfg.fw_version_minor,
		cmd_cfg.fw_version_patch);
	pr_info("Hardware type  : %s\n", cmd_cfg.eval_board_name);
	pr_info("================================================\n");
	pr_info("uC CFG-3-2-1-0: %d%d%d%d\n",
		no_os_field_get(NO_OS_BIT(3), cmd_cfg.fw_mode),
		no_os_field_get(NO_OS_BIT(2), cmd_cfg.fw_mode),
		no_os_field_get(NO_OS_BIT(1), cmd_cfg.fw_mode),
		no_os_field_get(NO_OS_BIT(0), cmd_cfg.fw_mode));

	ret = device_init(dev_0, dev_8, dev_init);
	if (ret)
		return ret;

	switch (cmd_cfg.fw_mode) {
	case 0:
		pr_info("Firmware mode  : GUI Mode\n");
		break;
	case 1: /* fall through */
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
		/* Reserved for future firmware modes */
		break;
	case 14:
		/* FMCZ board Test - SGMII Path Test */
		ret = fw_mode_sgmii_path_test(cmd_gpio, *dev_0,
					      *dev_8);
		if (ret)
			pr_err("SGMII Path Test failed\n");

		/* Indicate Test Done by turning ON Green LED */
		ret = no_os_gpio_direction_output(cmd_gpio->gpio_led_green_desc,
						  NO_OS_GPIO_LOW);
		if (ret)
			pr_err("GPIO LED Green Direction Output failed!\n");

		pr_info("Test Done\n");
		break;
	case 15:
		/* FMCZ board Test - MDI Path Test */
		ret = fw_mode_mdi_path_test(cmd_gpio, *dev_0,
					    *dev_8);
		if (ret)
			pr_err("MDI Path Test failed\n");

		/* Indicate Test Done by turning ON Green LED */
		ret = no_os_gpio_direction_output(cmd_gpio->gpio_led_green_desc,
						  NO_OS_GPIO_LOW);
		if (ret)
			pr_err("GPIO LED Green Direction Output failed!\n");

		pr_info("Test Done\n");
		break;
	default:
		pr_info("Firmware mode  : Unsupported Mode\n");
		break;
	}

	pr_info("================================================\n");
	pr_info("Type '?' or 'help' for a list of commands.\n");
	pr_info("================================================\n");

	return 0;
}

/* Main command processing loop */
int example_main(void)
{
	int ret;
	int param_idx;
	bool dev_init[2] = {false, false};
	struct no_os_uart_desc *uart_desc;
	struct cmd_config cmd_cfg;
	struct cmd_gpio *cmd_gpio;
	struct adin1320_desc *adin1320_0_desc;
	struct adin1320_desc *adin1320_8_desc;

	/* Command Input Structure Initialization */
	struct cmd_input cmd_input = {
		.cmd = {0},
		.is_cmd_available = false,
		.is_cmd_processed = false
	};

	for (param_idx = 0; param_idx < CMD_PARAM_SIZE; param_idx++)
		cmd_input.cmd_param[param_idx] = CMD_PARAM_EMPTY;

	/* Board Initialization */
	ret = board_init(&uart_desc, &cmd_cfg, &cmd_gpio);
	if (ret)
		return ret;

	/* Initialization based on Firmware Mode */
	ret = fw_mode_init(cmd_cfg, cmd_gpio, &adin1320_0_desc,
			   &adin1320_8_desc, dev_init);
	if (ret)
		goto err_board;

	/* Main command processing loop - runs indefinitely */
	while (1) {
		get_cmd(uart_desc, &cmd_input);

		if (cmd_input.is_cmd_available) {
			select_cmd(&cmd_input);
			process_cmd(adin1320_0_desc, adin1320_8_desc,
				    &cmd_input, dev_init);

			cmd_input.is_cmd_available = false;
			memset(cmd_input.cmd, 0, sizeof(cmd_input.cmd));
		}
	}

err_board:
	no_os_gpio_remove(cmd_gpio->gpio_led_blue_desc);
	no_os_gpio_remove(cmd_gpio->gpio_led_yellow_desc);
	no_os_gpio_remove(cmd_gpio->gpio_led_red_desc);
	no_os_gpio_remove(cmd_gpio->gpio_led_green_desc);
	no_os_gpio_remove(cmd_gpio->gpio_uc_cfg3_desc);
	no_os_gpio_remove(cmd_gpio->gpio_uc_cfg2_desc);
	no_os_gpio_remove(cmd_gpio->gpio_uc_cfg1_desc);
	no_os_gpio_remove(cmd_gpio->gpio_uc_cfg0_desc);
	no_os_free(cmd_gpio);
	no_os_uart_remove(uart_desc);
	return ret;
}
