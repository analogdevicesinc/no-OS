/***************************************************************************//**
 *   @file   iperf_lwip_example.c
 *   @brief  Implementation of iperf LWIP example for adin1140 project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

#include <string.h>

#include "common_data.h"
#include "lwip_socket.h"
#include "lwip_adin1140.h"
#include "adin1140.h"
#include "lwip/apps/lwiperf.h"
#include "lwip/ip_addr.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"

static uint8_t adin1140_mac_address[6] = {0x00, 0x18, 0x80, 0x03, 0x25, 0x80};

static struct adin1140_init_param adin1140_ip;
static struct no_os_gpio_desc *int_gpio;
static struct no_os_irq_ctrl_desc *nvic_irq_ctrl;
static struct no_os_irq_ctrl_desc *gpio_irq_ctrl;
static uint32_t interrupt_count = 0;
static volatile bool network_activity = false;
static struct lwip_network_desc *global_lwip_desc = NULL;

/***************************************************************************//**
 * @brief iperf report callback function
 *
 * @param arg        User-defined argument (unused)
 * @param report_type Type of the report
 * @param local_addr  Local IP address
 * @param local_port  Local port
 * @param remote_addr Remote IP address
 * @param remote_port Remote port
 * @param bytes_transferred Total bytes transferred
 * @param ms_duration Test duration in milliseconds
 * @param bandwidth_kbitpsec Bandwidth in kbits/sec
 *******************************************************************************/
static void
lwiperf_report(void *arg, enum lwiperf_report_type report_type,
               const ip_addr_t* local_addr, u16_t local_port,
               const ip_addr_t* remote_addr, u16_t remote_port,
               u32_t bytes_transferred, u32_t ms_duration, u32_t bandwidth_kbitpsec)
{
	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(local_addr);
	LWIP_UNUSED_ARG(local_port);

	pr_info("IPERF report: type=%d, remote: %s:%d, total bytes: %"U32_F
	        ", duration in ms: %"U32_F", kbits/s: %"U32_F"\n",
	        (int)report_type, ipaddr_ntoa(remote_addr), (int)remote_port,
	        bytes_transferred, ms_duration, bandwidth_kbitpsec);
}

/***************************************************************************//**
 * @brief ADIN1140 interrupt handler - processes received frames when
 * interrupt is triggered by RX data available.
 *
 * @param context - lwip network descriptor passed as context
 *******************************************************************************/
static void adin1140_int_handler(void *context)
{
	interrupt_count++;

	/* Signal network activity available - Linux-style wake up */
	network_activity = true;

	/* Disable interrupt to prevent storm while processing */
	no_os_irq_disable(gpio_irq_ctrl, ADIN1140_INT_PIN);
}

/***************************************************************************//**
 * @brief iperf LWIP example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously and provide iperf server functionality.
 *******************************************************************************/
int example_main()
{
	int ret;
	struct lwip_network_param lwip_param = { 0 };
	struct lwip_network_desc *lwip_desc;

	adin1140_ip.comm_param = adin1140_spi_ip;
	memcpy(adin1140_ip.mac_address, adin1140_mac_address, NETIF_MAX_HWADDR_LEN);

	lwip_param.platform_ops = &adin1140_lwip_ops;
	lwip_param.mac_param = &adin1140_ip;
	lwip_param.extra = NULL;
	memcpy(lwip_param.hwaddr, adin1140_mac_address, NETIF_MAX_HWADDR_LEN);

	ret = no_os_lwip_init(&lwip_desc, &lwip_param);
	if (ret)
		return ret;

	global_lwip_desc = lwip_desc;

	/* Initialize GPIO for interrupt pin */
	ret = no_os_gpio_get(&int_gpio, &adin1140_int_gpio_ip);
	if (ret) {
		pr_err("Failed to initialize GPIO: %d\n", ret);
		goto cleanup_lwip;
	}

	ret = no_os_gpio_direction_input(int_gpio);
	if (ret) {
		pr_err("Failed to set GPIO as input: %d\n", ret);
		goto cleanup_gpio;
	}

	/* Initialize NVIC IRQ controller first */
	ret = no_os_irq_ctrl_init(&nvic_irq_ctrl, &adin1140_nvic_irq_ip);
	if (ret) {
		pr_err("Failed to initialize NVIC IRQ controller: %d\n", ret);
		goto cleanup_gpio;
	}

	/* Enable GPIO1 IRQ on NVIC */
	ret = no_os_irq_enable(nvic_irq_ctrl, ADIN1140_INT_IRQn);
	if (ret) {
		pr_err("Failed to enable GPIO1 IRQ on NVIC: %d\n", ret);
		goto cleanup_nvic;
	}

	/* Initialize GPIO IRQ controller */
	ret = no_os_irq_ctrl_init(&gpio_irq_ctrl, &adin1140_gpio_irq_ip);
	if (ret) {
		pr_err("Failed to initialize GPIO IRQ controller: %d\n", ret);
		goto cleanup_nvic;
	}

	/* Register interrupt callback */
	struct no_os_callback_desc adin1140_int_cb = {
		.callback = adin1140_int_handler,
		.ctx = lwip_desc,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
	};

	ret = no_os_irq_register_callback(gpio_irq_ctrl, ADIN1140_INT_PIN, &adin1140_int_cb);
	if (ret) {
		pr_err("Failed to register IRQ callback: %d\n", ret);
		goto cleanup_irq;
	}

	/* Configure interrupt trigger (rising edge since we see 0->1) */
	ret = no_os_irq_trigger_level_set(gpio_irq_ctrl, ADIN1140_INT_PIN, NO_OS_IRQ_EDGE_RISING);
	if (ret) {
		pr_err("Failed to set IRQ trigger level: %d\n", ret);
		goto cleanup_irq;
	}
	pr_info("Set IRQ trigger to rising edge\n");

	/* Set interrupt priority */
	ret = no_os_irq_set_priority(gpio_irq_ctrl, ADIN1140_INT_PIN, 1);
	if (ret) {
		pr_err("Failed to set IRQ priority: %d\n", ret);
		goto cleanup_irq;
	}

	/* Enable the interrupt */
	ret = no_os_irq_enable(gpio_irq_ctrl, ADIN1140_INT_PIN);
	if (ret) {
		pr_err("Failed to enable IRQ: %d\n", ret);
		goto cleanup_irq;
	}

	pr_info("Starting lwiperf server on port %d\n", LWIPERF_TCP_PORT_DEFAULT);
	lwiperf_start_tcp_server_default(lwiperf_report, NULL);

	pr_info("iperf server started. Network is ready for iperf testing.\n");

	/* Adaptive event-driven processing */
	uint32_t idle_count = 0;
	uint32_t work_sessions = 0;
	uint32_t active_cycles = 0;
	uint32_t no_activity_count = 0;

	while (1) {
		/* Check if interrupt signaled network activity */
		if (network_activity) {
			network_activity = false;
			work_sessions++;
			active_cycles++;
			no_activity_count = 0;  /* Reset idle counter */

			/* Burst processing - continue until no more work */
			int iterations = 0;
			do {
				no_os_lwip_step(lwip_desc, NULL);
				iterations++;
			} while (iterations < 16);  /* Larger burst during traffic */

			/* Re-enable interrupt after burst */
			no_os_irq_enable(gpio_irq_ctrl, ADIN1140_INT_PIN);

		} else {
			/* Check for ongoing activity without interrupt */
			no_activity_count++;

			/* During active periods, keep processing without sleep */
			if (no_activity_count < 1000) {
				no_os_lwip_step(lwip_desc, NULL);
				active_cycles++;
			} else {
				/* Truly idle - sleep and allow other tasks */
				idle_count++;

				/* Light housekeeping only */
				if (idle_count % 100 == 0) {
					no_os_lwip_step(lwip_desc, NULL);
				}

				/* Sleep only when truly idle */
				if (idle_count % 10 == 0) {
					no_os_mdelay(1);
				}
			}

			/* Status reporting */
			if ((idle_count + active_cycles) % 50000 == 0) {
				pr_info("Active: %lu, Idle: %lu, Work sessions: %lu, Interrupts: %lu\n",
					active_cycles, idle_count, work_sessions, interrupt_count);
			}
		}
	}

	/* Cleanup on exit (unreachable in normal operation) */
	no_os_irq_disable(gpio_irq_ctrl, ADIN1140_INT_PIN);
	no_os_irq_disable(nvic_irq_ctrl, ADIN1140_INT_IRQn);

cleanup_irq:
	no_os_irq_ctrl_remove(gpio_irq_ctrl);

cleanup_nvic:
	no_os_irq_ctrl_remove(nvic_irq_ctrl);

cleanup_gpio:
	no_os_gpio_remove(int_gpio);

cleanup_lwip:
	no_os_lwip_remove(lwip_desc);

	return ret;
}