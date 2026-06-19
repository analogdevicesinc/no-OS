/***************************************************************************//**
*   @file   cmd_mode.c
*   @brief  Implementation of terminal command control FW modes for ADIN1320
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

#include "adin1320.h"
#include "common_data.h"
#include "mdio_spi.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

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

int device_init(struct adin1320_desc **dev_0, struct adin1320_desc **dev_8,
		bool dev_init[2])
{
	int ret;
	struct adin1320_init_param adin1320_ip = {
		.mdio_param = {
			.c45 = true,
			.ops = MDIO_OPS,
			.extra = &(struct mdio_spi_init_param)
			{
				.mdio = spi_ip
			}
		}
	};

	/* Initialize output pointers to NULL to prevent garbage pointer dereferences */
	*dev_0 = NULL;
	*dev_8 = NULL;

	/* Initialize for Port 0 (PHY Address 0) */
	adin1320_ip.mdio_param.addr = ADIN1320_0_PHY_ADDR;
	adin1320_ip.reset_param = &gpio_adin1320_0_reset_ip;
	ret = adin1320_init(dev_0, &adin1320_ip);
	if (ret) {
		*dev_0 = NULL;
		dev_init[0] = false;
	} else {
		dev_init[0] = true;
	}

	/* Check Port 1 (PHY Address 8) */
	adin1320_ip.mdio_param.addr = ADIN1320_8_PHY_ADDR;
	adin1320_ip.reset_param = &gpio_adin1320_8_reset_ip;
	ret = adin1320_init(dev_8, &adin1320_ip);
	if (ret) {
		*dev_8 = NULL;
		dev_init[1] = false;
	} else {
		dev_init[1] = true;
	}

	/* Re-init Port 0 due to Port 1 init failed
	   (shared SPI un-init in adin1320 init failed) */
	if (!dev_init[1] && dev_init[0]) {
		adin1320_remove(*dev_0);
		adin1320_ip.mdio_param.addr = ADIN1320_0_PHY_ADDR;
		adin1320_ip.reset_param = &gpio_adin1320_0_reset_ip;
		ret = adin1320_init(dev_0, &adin1320_ip);
		if (ret) {
			*dev_0 = NULL;
			dev_init[0] = false;
		} else {
			dev_init[0] = true;
		}
	}

	/* Print Initialization Status */
	pr_info("Detected PHY: ");
	if (dev_init[0] && dev_init[1])
		pr_info("Port 0 & 1 Initialized\n");
	else if (dev_init[0] && !dev_init[1])
		pr_info("Port 0 Initialized\n");
	else if (!dev_init[0] && dev_init[1])
		pr_info("Port 1 Initialized\n");
	else {
		pr_err("Error: Failed to initialize any PHY device\n");
		return -EFAULT;
	}

	return 0;
}

/*
 * This function is now used for FMCZ board Test - SGMII Path Test
 * Start with P0 CFG1/0 as MODE2/MODE1, & MDIX as MODE2 MACSEL=101
 * Start with P1 CFG1/0 as MODE4/MODE4, & MDIX as MODE4 MACSEL=010
 */
int fw_mode_sgmii_path_test(struct cmd_gpio *cmd_gpio,
			    struct adin1320_desc *dev_0,
			    struct adin1320_desc *dev_8)
{
	int ret;
	uint16_t data;
	uint32_t link_timeout;
	bool sgmii_test_passed = true;
	struct adin1320_mii_status mii_status = ADIN1320_MII_STATUS_STRUCT_INIT;

	pr_info("EV-ADIN1320FMCZ BRD Test - SGMII Path\n");

	/* Indicate Start of test by turning on BLUE LED */
	ret = no_os_gpio_direction_output(cmd_gpio->gpio_led_blue_desc,
					  NO_OS_GPIO_LOW);
	if (ret)
		pr_err("GPIO LED Blue Direction Output failed!\n");

	/* Link ST Check */
	/* Timeout calculation: 10000ms * 1000us/ms / 125us per iteration = ~80000 iterations */
	link_timeout = (10000 * 1000 / 125);
	mii_status.link_stat = ADIN1320_LINK_STAT_LINKDOWN;

	do {
		ret = adin1320_cu_get_mii_status(dev_8, &mii_status);
		if (ret)
			return ret;
	} while ((mii_status.link_stat != ADIN1320_LINK_STAT_LINKUP) &&
		 (--link_timeout)); /* Wait for link up */

	if (mii_status.link_stat != ADIN1320_LINK_STAT_LINKUP)
		sgmii_test_passed = false; /* Failed */

	pr_info("Link Status: %s, Test Result: %s\n",
		sgmii_test_passed ? "UP" : "DOWN",
		sgmii_test_passed ? "PASS" : "FAIL");

	/* Failed: Exit test */
	if (!sgmii_test_passed) {
		pr_info("Test Failed\n");
		return 0;
	}

	/* Indicate Link Up by turning on Yellow LED */
	ret = no_os_gpio_direction_output(cmd_gpio->gpio_led_yellow_desc,
					  NO_OS_GPIO_LOW);
	if (ret)
		pr_err("GPIO LED Yellow Direction Output failed!\n");

	/* Read FC Registers to Clear */
	ret = adin1320_read(dev_8, 0x0014, &data); /* RX_ERR_CNT */
	if (ret)
		return ret;

	ret = adin1320_read(dev_8, 0x1E9411, &data); /* FC_FRM_CNT_L */
	if (ret)
		return ret;

	ret = adin1320_read(dev_8, 0x1E9410, &data); /* FC_FRM_CNT_H */
	if (ret)
		return ret;

	/* Run FG in ADIN1320 */
	ret = adin1320_write(dev_8, 0x1E9412, 0xFFFF); /* FC_MAX_FRM_SIZE */
	if (ret)
		return ret;

	ret = adin1320_write(dev_8, 0x1E9406, 0x0001); /* FC_TX_SEL */
	if (ret)
		return ret;

	ret = adin1320_write(dev_8, ADIN1320_PHY_CTRL_1, 0x0404);
	if (ret)
		return ret;

	ret = adin1320_write(dev_8, 0x1E9427, 0x006B); /* FG_FRM_LEN */
	if (ret)
		return ret;

	ret = adin1320_write(dev_8, 0x1E942A, 0xFFFF); /* FG_NFRM_L */
	if (ret)
		return ret;

	ret = adin1320_write(dev_8, 0x1E9429, 0x0002); /* FG_NFRM_H */
	if (ret)
		return ret;

	ret = adin1320_write(dev_8, 0x1E9424, 0x0000); /* FG_CONT_MODE_EN */
	if (ret)
		return ret;

	ret = adin1320_write(dev_8, 0x1E9423, 0x0001); /* FG_CNTRL_RSTRT */
	if (ret)
		return ret;

	ret = adin1320_write(dev_8, 0x1E9422, 0x0001); /* FG_EN */
	if (ret)
		return ret;

	do {
		ret = adin1320_read(dev_8, 0x1E942B, &data); /* FG_DONE */
		if (ret)
			return ret;
	} while ((data & 0x0001) == 0); /* Wait for FG done */

	no_os_mdelay(10000); /* Allow time window for frames to come back */

	/* Read frame error count to enable reading of frame counts */
	ret = adin1320_read(dev_8, 0x0014, &data); /* RX_ERR_CNT */
	if (ret)
		return ret;

	/* Read frame count - lower half */
	ret = adin1320_read(dev_8, 0x1E9411, &data); /* FC_FRM_CNT_L */
	if (ret)
		return ret;

	/* Expecting value of at least one in FC_FRM_CNT_L */
	if (data == 0x0000)
		sgmii_test_passed = false; /* Failed */

	pr_info("0x1E9411: 0x%X, Test Result: %s\n", data,
		sgmii_test_passed ? "PASS" : "FAIL");

	/* Read frame count - upper/higher half */
	ret = adin1320_read(dev_8, 0x1E9410, &data); /* FC_FRM_CNT_H */
	if (ret)
		return ret;

	/* Expecting value of 2 in FC_FRM_CNT_H */
	if (data != 0x0002)
		sgmii_test_passed = false; /* Failed */

	pr_info("0x1E9410: 0x%X, Test Result: %s\n", data,
		sgmii_test_passed ? "PASS" : "FAIL");

	/* Failed: Exit test */
	if (!sgmii_test_passed) {
		pr_info("Test Failed\n");
		return 0;
	}

	pr_info("Test Passed\n");

	/* Indicate Test Passed by turning on Red LED */
	ret = no_os_gpio_direction_output(cmd_gpio->gpio_led_red_desc,
					  NO_OS_GPIO_LOW);
	if (ret)
		pr_err("GPIO LED Red Direction Output failed!\n");

	return 0;
}

/*
 * This function is now used for FMCZ board Test - MDI Path Test
 * Start with P0 CFG1/0 as MODE4/MODE4, & MDIX as MODE4 MACSEL=000
 * Start with P1 CFG1/0 as MODE4/MODE4, & MDIX as MODE4 MACSEL=001
 */
int fw_mode_mdi_path_test(struct cmd_gpio *cmd_gpio,
			  struct adin1320_desc *dev_0,
			  struct adin1320_desc *dev_8)
{
	int ret;
	uint16_t data;
	uint32_t link_timeout;
	bool mdi_test_passed = true;
	struct adin1320_mii_status mii_status = ADIN1320_MII_STATUS_STRUCT_INIT;

	pr_info("EV-ADIN1320FMCZ BRD Test - MDI Path\n");

	/* Indicate Start of test by turning on BLUE LED */
	ret = no_os_gpio_direction_output(cmd_gpio->gpio_led_blue_desc,
					  NO_OS_GPIO_LOW);
	if (ret)
		pr_err("GPIO LED Blue Direction Output failed!\n");

	/* Link ST Check */
	/* Timeout calculation: 10000ms * 1000us/ms / 125us per iteration = ~80000 iterations */
	link_timeout = (10000 * 1000 / 125);
	mii_status.link_stat = ADIN1320_LINK_STAT_LINKDOWN;

	do {
		ret = adin1320_cu_get_mii_status(dev_0, &mii_status);
		if (ret)
			return ret;
	} while ((mii_status.link_stat != ADIN1320_LINK_STAT_LINKUP) &&
		 (--link_timeout)); /* Wait for link up */

	if (mii_status.link_stat != ADIN1320_LINK_STAT_LINKUP)
		mdi_test_passed = false; /* Failed */

	/* Check Speed */
	ret = adin1320_read(dev_0, ADIN1320_MII_CONTROL, &data);
	if (ret)
		return ret;

	if (no_os_field_get(ADIN1320_SPEED_SEL_MSB_MASK | ADIN1320_SPEED_SEL_LSB_MASK,
			    data) !=
	    ADIN1320_SPEED_SEL_MSB_MASK)
		mdi_test_passed = false; /* Failed */

	pr_info("ADIN1320_MII_CONTROL: 0x%X, Test Result: %s\n", data,
		mdi_test_passed ? "PASS" : "FAIL");

	/* Failed: Exit test */
	if (!mdi_test_passed) {
		pr_info("Test Failed\n");
		return 0;
	}

	/* Indicate Link Up on correct speed by turning on Yellow LED */
	ret = no_os_gpio_direction_output(cmd_gpio->gpio_led_yellow_desc,
					  NO_OS_GPIO_LOW);
	if (ret)
		pr_err("GPIO LED Yellow Direction Output failed!\n");

	/* Read FC Registers to Clear */
	ret = adin1320_read(dev_0, 0x0014, &data); /* RX_ERR_CNT */
	if (ret)
		return ret;

	ret = adin1320_read(dev_0, 0x1E9411, &data); /* FC_FRM_CNT_L */
	if (ret)
		return ret;

	ret = adin1320_read(dev_0, 0x1E9410, &data); /* FC_FRM_CNT_H */
	if (ret)
		return ret;

	/* Run FG in ADIN1320 */
	ret = adin1320_write(dev_0, 0x1E9412, 0xFFFF); /* FC_MAX_FRM_SIZE */
	if (ret)
		return ret;

	ret = adin1320_write(dev_0, 0x1E9406, 0x0000); /* FC_TX_SEL */
	if (ret)
		return ret;

	ret = adin1320_write(dev_0, ADIN1320_PHY_CTRL_1, 0x0404);
	if (ret)
		return ret;

	ret = adin1320_write(dev_0, 0x1E9427, 0x006B); /* FG_FRM_LEN */
	if (ret)
		return ret;

	ret = adin1320_write(dev_0, 0x1E942A, 0xFFFF); /* FG_NFRM_L */
	if (ret)
		return ret;

	ret = adin1320_write(dev_0, 0x1E9429, 0x0002); /* FG_NFRM_H */
	if (ret)
		return ret;

	ret = adin1320_write(dev_0, 0x1E9424, 0x0000); /* FG_CONT_MODE_EN */
	if (ret)
		return ret;

	ret = adin1320_write(dev_0, 0x1E9423, 0x0001); /* FG_CNTRL_RSTRT */
	if (ret)
		return ret;

	ret = adin1320_write(dev_0, 0x1E9422, 0x0001); /* FG_EN */
	if (ret)
		return ret;

	do {
		ret = adin1320_read(dev_0, 0x1E942B, &data); /* FG_DONE */
		if (ret)
			return ret;
	} while ((data & 0x0001) == 0); /* Wait for FG done */

	no_os_mdelay(10000); /* Allow time window for frames to come back */

	/* Read frame error count to enable reading of frame counts */
	ret = adin1320_read(dev_0, 0x0014, &data); /* RX_ERR_CNT */
	if (ret)
		return ret;

	/* Read frame count - lower half */
	ret = adin1320_read(dev_0, 0x1E9411, &data); /* FC_FRM_CNT_L */
	if (ret)
		return ret;

	/* Expecting value of at least one in FC_FRM_CNT_L */
	if (data == 0x0000)
		mdi_test_passed = false; /* Failed */

	pr_info("0x1E9411: 0x%X, Test Result: %s\n", data,
		mdi_test_passed ? "PASS" : "FAIL");

	/* Read frame count - upper/higher half */
	ret = adin1320_read(dev_0, 0x1E9410, &data); /* FC_FRM_CNT_H */
	if (ret)
		return ret;

	/* Expecting value of 2 in FC_FRM_CNT_H */
	if (data != 0x0002)
		mdi_test_passed = false; /* Failed */

	pr_info("0x1E9410: 0x%X, Test Result: %s\n", data,
		mdi_test_passed ? "PASS" : "FAIL");

	/* Failed: Exit test */
	if (!mdi_test_passed) {
		pr_info("Test Failed\n");
		return 0;
	}

	pr_info("Test Passed\n");

	/* Indicate Test Passed by turning on Red LED */
	ret = no_os_gpio_direction_output(cmd_gpio->gpio_led_red_desc,
					  NO_OS_GPIO_LOW);
	if (ret)
		pr_err("GPIO LED Red Direction Output failed!\n");

	return 0;
}
