/***************************************************************************//**
*   @file   basic_init.c
*   @brief  Basic initialization example for ADIN1320
*   @author John Roed Retuya (Johnroed.Retuya@analog.com)
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

#include "adin1320.h"
#include "common_data.h"
#include "mdio_spi.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

int example_main(void)
{
	int ret;
	bool is_linked;
	uint16_t autoneg_adv_speeds;
	uint8_t eee_speeds;
	struct adin1320_desc *adin1320_desc;
	enum adin1320_software_powerdown software_power_status_val;
	enum adin1320_cu_speed final_negotiated_speed;
	struct no_os_uart_desc *uart_desc;
	uint16_t data;
	uint16_t addr;

	/* UART Initialization */
	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);
	pr_info("Initialized UART\n");

	/* INITIALIZATION */
	/* Enable Tx and Rx delays and set to clock delays to 2 ns */
	struct adin1320_rgmii_config rgmii_config = {
		.tx_idelay_en = ADIN1320_RGMII_IDELAY_ENABLE,
		.tx_idelay = ADIN1320_RGMII_2_00_NS,
		.rx_idelay_en = ADIN1320_RGMII_IDELAY_ENABLE,
		.rx_idelay = ADIN1320_RGMII_2_00_NS
	};

	struct adin1320_mii_status mii_status = ADIN1320_MII_STATUS_STRUCT_INIT;
	/* Override default NO_CHECK (0) to LINKDOWN (1) for link polling */
	mii_status.link_stat = ADIN1320_LINK_STAT_LINKDOWN;

	struct adin1320_init_param adin1320_ip = {
		.reset_param = &gpio_adin1320_0_reset_ip,
		.mdio_param = {
			.c45 = true,
			.addr = ADIN1320_0_PHY_ADDR,
			.ops = MDIO_OPS,
			.extra = &(struct mdio_spi_init_param)
			{
				.mdio = spi_ip
			}
		}
	};

	pr_info("Running basic init example\n");

	/* Initializes the adin1320 descriptor */
	ret = adin1320_init(&adin1320_desc, &adin1320_ip);
	if (ret)
		goto err_uart;

	/* Resets the adin1320 device */
	/*  In this example, ADIN1320 is expected to be HW Strapped on
	    MAC_SELx = 0b000, PHY_CFG1/0 = MODE_3/MODE_4 */

	ret = adin1320_hard_reset(adin1320_desc);
	if (ret)
		goto err_adin;

	/* Reads the status of Software Powerdown */
	ret = adin1320_get_software_powerdown(adin1320_desc,
					      &software_power_status_val);
	if (ret)
		goto err_adin;

	/* Set the mac interface as RGMII */
	ret = adin1320_select_mac_interface(adin1320_desc,
					    ADIN1320_RGMII_MAC_INTERFACE);
	if (ret)
		goto err_adin;

	ret = adin1320_config_rgmii(adin1320_desc, rgmii_config);
	if (ret)
		goto err_adin;

#ifdef FORCE_SPEED
	/* Disable auto-negotiation */
	ret = adin1320_cu_autoneg(adin1320_desc, ADIN1320_AUTONEG_DISABLE);
	if (ret)
		goto err_adin;

	/* Force speed instead of auto-negotiation
	 * Forces 10 Mbps full duplex mode (auto-negotiation is disabled) */
	ret = adin1320_cu_forced_speed(adin1320_desc,
				       ADIN1320_CU_SPEED_10BASE_T_FD);
	if (ret)
		goto err_adin;
#else
	/* Enables auto-negotiation */
	ret = adin1320_cu_autoneg(adin1320_desc, ADIN1320_AUTONEG_ENABLE);
	if (ret)
		goto err_adin;

	/* Enables the advertisement of speeds 10, 100 and 1000 half duplex
	 * and full duplex */
	ret = adin1320_cu_autoneg_adv_cfg(adin1320_desc,
					  ADIN1320_CU_SPEED_10BASE_T_HD |
					  ADIN1320_CU_SPEED_10BASE_T_FD |
					  ADIN1320_CU_SPEED_100BASE_TX_HD |
					  ADIN1320_CU_SPEED_100BASE_TX_FD |
					  ADIN1320_CU_SPEED_1000BASE_T_HD |
					  ADIN1320_CU_SPEED_1000BASE_T_FD);
	if (ret)
		goto err_adin;

	/* Enables 100 and 1000 Mbps speed for EEE */
	ret = adin1320_set_eee(adin1320_desc, ADIN1320_EEE_100_ADV |
			       ADIN1320_EEE_1000_ADV);
	if (ret)
		goto err_adin;
#endif /* FORCE_SPEED */

	pr_info("Exiting SW PD\n");

	/* Exit software powerdown */
	ret = adin1320_set_software_powerdown(adin1320_desc,
					      ADIN1320_SOFTWARE_POWERUP);
	if (ret)
		goto err_adin;

	pr_info("Exiting SW PD Successful\n");

	/* Read back */
	/* Reading Link Status */
	pr_info("Linking...\n");
	do {
		ret = adin1320_cu_get_mii_status(adin1320_desc, &mii_status);
		if (ret)
			goto err_adin;

		is_linked = (mii_status.link_stat == ADIN1320_LINK_STAT_LINKUP);
	} while (!is_linked);

	pr_info("Linked up\n");

	/* Reads the autoneg advertised enabled speeds. */
	ret = adin1320_cu_get_autoneg_adv(adin1320_desc, &autoneg_adv_speeds);
	if (ret)
		goto err_adin;

	/* Reads the eee enabled speeds. */
	ret = adin1320_get_eee(adin1320_desc, &eee_speeds);
	if (ret)
		goto err_adin;

	/* Reads the final negotiated speed of the local and remote device */
	ret = adin1320_cu_resolved_speed(adin1320_desc,
					 &final_negotiated_speed);
	if (ret)
		goto err_adin;

	pr_info("Final Negotiated Speed: %d\n",
		no_os_log_base_2(final_negotiated_speed));

	pr_info("Printing address 0x00 to 0x1F\n");
	for (addr = 0x00; addr < 0x20; addr++) {
		ret = adin1320_read(adin1320_desc, addr, &data);
		if (ret)
			pr_err("Read failed at address 0x%X\n", addr);
		else
			pr_info("0x%02X = 0x%04X\n", addr, data);
	}

	/* Allow time for UART buffer to flush before cleanup */
	no_os_mdelay(1000);

err_adin:
	adin1320_remove(adin1320_desc);
err_uart:
	no_os_uart_remove(uart_desc);

	return ret;
}
