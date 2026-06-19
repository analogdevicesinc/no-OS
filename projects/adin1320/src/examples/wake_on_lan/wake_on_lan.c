/***************************************************************************//**
*   @file   wake_on_lan.c
*   @brief  Implementation of wake-on-LAN configuration for ADIN1320 devices example
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

#include "adin1320.h"
#include "common_data.h"
#include "mdio_spi.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

static struct irq_context {
	struct adin1320_desc *adin1320_desc;
	struct no_os_irq_ctrl_desc *nvic;
} irq_ctx;

static void adin1320_int_n_callback(void *context)
{
	int ret;
	uint16_t irq_status;
	uint16_t irq_latches;
	uint16_t wol_status;
	struct irq_context *ctx = context;

	no_os_irq_disable(ctx->nvic, ADIN1320_INT_N_IRQn);

	pr_info("Interrupt received!\n");
	pr_info("Wake-on-LAN event occurred\n");

	ret = adin1320_read(ctx->adin1320_desc, ADIN1320_GE_WOL_STAT,
			    &wol_status);
	if (ret)
		pr_err("Failed to read WOL status\n");

	if (wol_status & ADIN1320_GE_WOL_LS_CHG_MASK)
		pr_info("- Link status changed\n");

	if (wol_status & ADIN1320_GE_MGC_0_FRM_MATCH_MASK)
		pr_info("- Magic packet matched frame/s\n");

	if (wol_status & ADIN1320_GE_MGC_0_KEY_ERR_MASK)
		pr_info("- Magic packet SecureOn key error\n");

	pr_info("\n");

	ret = adin1320_read(ctx->adin1320_desc, ADIN1320_IRQ_STATUS, &irq_status);
	if (ret)
		pr_err("Failed to read IRQ status\n");

	pr_info("IRQ Status: %x\n", irq_status);

	ret = adin1320_read(ctx->adin1320_desc, ADIN1320_GE_IRQ_LAT,
			    &irq_latches);
	if (ret)
		pr_err("Failed to read IRQ latches\n");

	pr_info("IRQ Latches: %x\n", irq_latches);

	no_os_irq_enable(ctx->nvic, ADIN1320_INT_N_IRQn);
}

int example_main(void)
{
	int ret;
	struct no_os_uart_desc *uart_desc;
	struct adin1320_desc *adin1320_desc;
	struct no_os_irq_ctrl_desc *irq_nvic_desc;
	struct no_os_irq_ctrl_desc *irq_int_n_desc;
	struct no_os_gpio_desc *gpio_int_n_desc;
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
	struct no_os_callback_desc callback_wol_desc = {
		.callback = adin1320_int_n_callback,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
	};

	/* UART Initialization */
	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);
	pr_info("Initialized UART\n");

	pr_info("Running wake-on-LAN example\n");

	/* Initializes the adin1320 descriptor */
	ret = adin1320_init(&adin1320_desc, &adin1320_ip);
	if (ret)
		goto err_uart;

	pr_info("Initialized ADIN1320 device\n");

	ret = adin1320_write_bits(adin1320_desc, ADIN1320_GE_IRQ_EN,
				  ADIN1320_GE_WOL_WAKE_IRQ_EN_MASK,
				  ADIN1320_GE_WOL_WAKE_IRQ_EN_MASK);
	if (ret)
		goto err_adin;

	pr_info("Enabled wake-on-LAN interrupt\n");

	ret = adin1320_write_bits(adin1320_desc, ADIN1320_IRQ_MASK, 0,
				  ADIN1320_LNK_STAT_CHNG_IRQ_EN_MASK);
	if (ret)
		goto err_adin;

	pr_info("Disabled link status change interrupt\n");

	ret = adin1320_master_slave_config(adin1320_desc,
					   ADIN1320_MAN_ADV_SLAVE);
	if (ret)
		goto err_adin;

	pr_info("Set master/slave configuration to slave\n");

	/* Initialize INT_N GPIO interrupt */
	ret = no_os_gpio_get(&gpio_int_n_desc, &gpio_adin1320_0_int_n_ip);
	if (ret)
		goto err_adin;

	ret = no_os_gpio_direction_input(gpio_int_n_desc);
	if (ret)
		goto err_gpio;

	ret = no_os_irq_ctrl_init(&irq_nvic_desc, &irq_nvic_ip);
	if (ret)
		goto err_gpio;

	ret = no_os_irq_enable(irq_nvic_desc, ADIN1320_INT_N_IRQn);
	if (ret)
		goto err_nvic;

	/* Initialize callback context with device and NVIC descriptors */
	irq_ctx.adin1320_desc = adin1320_desc;
	irq_ctx.nvic = irq_nvic_desc;
	callback_wol_desc.ctx = &irq_ctx;

	ret = no_os_irq_ctrl_init(&irq_int_n_desc, &irq_int_n_ip);
	if (ret)
		goto err_nvic;

	ret = no_os_irq_register_callback(irq_int_n_desc, ADIN1320_0_INT_N_PIN,
					  &callback_wol_desc);
	if (ret)
		goto err_irq_int_n;

	ret = no_os_irq_trigger_level_set(irq_int_n_desc, ADIN1320_0_INT_N_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		goto err_irq_int_n;

	/* Set interrupt priority to 1 (high priority) */
	ret = no_os_irq_set_priority(irq_int_n_desc, ADIN1320_0_INT_N_PIN, 1);
	if (ret)
		goto err_irq_int_n;

	ret = no_os_irq_enable(irq_int_n_desc, ADIN1320_0_INT_N_PIN);
	if (ret)
		goto err_irq_int_n;

	pr_info("Initialized interrupt GPIO\n");

	/* Disable wake-on-LAN for the configuration */
	ret = adin1320_wol_en_cfg(adin1320_desc, ADIN1320_WOL_DISABLE);
	if (ret)
		goto err_irq_int_n;

	pr_info("Disabled wake-on-LAN\n");

	ret = adin1320_wol_wake_cfg(adin1320_desc,
				    ADIN1320_WOL_WAKE_KEY_DISABLE,
				    ADIN1320_WOL_WAKE_LINK_ENABLE);
	if (ret)
		goto err_irq_int_n;

	ret = adin1320_wol_sys_cfg(adin1320_desc, ADIN1320_WOL_FI_DIS,
				   ADIN1320_WOL_INV_ACTIVE_HIGH);
	if (ret)
		goto err_irq_int_n;

	/* Configure WOL signal as pulse with maximum width (0xF = 15) */
	ret = adin1320_wol_sig_cfg(adin1320_desc, ADIN1320_WOL_SIG_PULSE, 0xF);
	if (ret)
		goto err_irq_int_n;

	/* Example MAC address - replace with actual device MAC */
	uint8_t mac_address[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
	ret = adin1320_wol_address(adin1320_desc, mac_address);
	if (ret)
		goto err_irq_int_n;

	pr_info("Configured wake-on-LAN\n");

	/* Magic packet configuration */
	ret = adin1320_mgc_match_cfg(adin1320_desc, ADIN1320_MGC_MATCH_EN_ST |
				     ADIN1320_MGC_MATCH_EN_UC |
				     ADIN1320_MGC_MATCH_EN_MC |
				     ADIN1320_MGC_MATCH_EN_BC);
	if (ret)
		goto err_irq_int_n;

	ret = adin1320_mgc_check_cfg(adin1320_desc, ADIN1320_MGC_CHK_EN_CRC |
				     ADIN1320_MGC_CHK_EN_LEN |
				     ADIN1320_MGC_CHK_EN_KEY);
	if (ret)
		goto err_irq_int_n;

	pr_info("Configured magic packet\n");

	/* Enable wake-on-LAN */
	ret = adin1320_wol_en_cfg(adin1320_desc, ADIN1320_WOL_ENABLE);
	if (ret)
		goto err_irq_int_n;

	pr_info("Enabled wake-on-LAN\n");

	/* Wait for wake-on-LAN occurrences */
	pr_info("Waiting for wake-on-LAN signals...\n\n");

	/* Infinite loop waiting for interrupt-driven wake-on-LAN events */
	while (1);

	/* Cleanup on error - unreachable in normal operation */
err_irq_int_n:
	no_os_irq_ctrl_remove(irq_int_n_desc);
err_nvic:
	no_os_irq_ctrl_remove(irq_nvic_desc);
err_gpio:
	no_os_gpio_remove(gpio_int_n_desc);
err_adin:
	adin1320_remove(adin1320_desc);
err_uart:
	no_os_uart_remove(uart_desc);

	return ret;
}
