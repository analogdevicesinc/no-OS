/***************************************************************************//**
*   @file   cmd_list.c
*   @brief  Implementation of terminal command list for ADIN1320
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

#include <stdbool.h>
#include <stdint.h>
#include "adin1320.h"
#include "no_os_print_log.h"

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

void cmd_print_list(struct cmd_input *cmd_input)
{
	pr_info("\n================ ADIN1320 Command Menu ================\n");
	pr_info("mdioread <phy_add>,<reg_add> - MDIO read clause 22\n");
	pr_info("mdiowrite <phy_add>,<reg_add>,<reg_val> - MDIO write clause 22\n");
	pr_info("mdiord_cl45 <phy_add>,<reg_add> - MDIO read clause 45\n");
	pr_info("mdiowr_cl45 <phy_add>,<reg_add>,<reg_val> - MDIO write clause 45\n");
	pr_info("reset <port> - Input (0) or (1) to hard reset port\n");
	pr_info("help - Print command list\n");
	pr_info("? - Print command list\n");
	pr_info("=======================================================\n");

	cmd_input->is_cmd_processed = true;
}

void cmd_mdio_read(struct adin1320_desc *dev, struct cmd_input *cmd_input)
{
	int ret;
	/* Save and restore MDIO addr — user may target a different PHY address */
	uint8_t original_addr = dev->mdio->addr;
	uint32_t reg_add = (uint32_t)cmd_input->cmd_param[2];
	uint16_t reg_val = 0;

	dev->mdio->addr = (uint8_t)cmd_input->cmd_param[1];
	ret = adin1320_read(dev, reg_add, &reg_val);
	dev->mdio->addr = original_addr;

	if (ret)
		pr_err("Error: %d\n", ret);
	else
		pr_info("%X\n", reg_val);

	cmd_input->is_cmd_processed = true;
}

void cmd_mdio_write(struct adin1320_desc *dev, struct cmd_input *cmd_input)
{
	int ret;
	/* Save and restore MDIO addr — user may target a different PHY address */
	uint8_t original_addr = dev->mdio->addr;
	uint32_t reg_add = (uint32_t)cmd_input->cmd_param[2];
	uint16_t reg_val = (uint16_t)cmd_input->cmd_param[3];

	dev->mdio->addr = (uint8_t)cmd_input->cmd_param[1];
	ret = adin1320_write(dev, reg_add, reg_val);
	dev->mdio->addr = original_addr;

	if (ret)
		pr_err("Error: %d\n", ret);
	else
		pr_info("OK\n");

	cmd_input->is_cmd_processed = true;
}

void cmd_reset(struct adin1320_desc *dev_0, struct adin1320_desc *dev_8,
	       struct cmd_input *cmd_input, bool dev_init[2])
{
	int ret;

	switch (cmd_input->cmd_param[1]) {
	case 0:
		if (!dev_init[0] || dev_0 == NULL) {
			pr_err("Error: Port 0 not initialized\n");
		} else {
			ret = adin1320_hard_reset(dev_0);
			if (ret)
				pr_err("Reset failed for Port 0\n");
			else
				pr_info("Port 0 reset\n");
		}
		break;
	case 1:
		if (!dev_init[1] || dev_8 == NULL) {
			pr_err("Error: Port 1 not initialized\n");
		} else {
			ret = adin1320_hard_reset(dev_8);
			if (ret)
				pr_err("Reset failed for Port 1\n");
			else
				pr_info("Port 1 reset\n");
		}
		break;
	default:
		pr_err("Error: Invalid reset option.\n");
		break;
	}

	cmd_input->is_cmd_processed = true;
}
