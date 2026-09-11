/***************************************************************************//**
 *   @file   iperf_lwip_freertos.c
 *   @brief  FreeRTOS-based iperf LWIP example for the ADIN1140 project.
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
#include <errno.h>
#include <stdbool.h>

#include "common_data.h"
#include "lwip_socket.h"
#include "lwip_adin1140.h"
#include "adin1140.h"
#include "lwip/apps/lwiperf.h"
#include "lwip/ip_addr.h"
#include "lwip/timeouts.h"
#include "iperf_menu.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_irq.h"

#include "FreeRTOS.h"
#include "task.h"

#define NET_INT_IRQ_PRIORITY \
	(configMAX_SYSCALL_INTERRUPT_PRIORITY >> (8 - configPRIO_BITS))

struct adin1140_net_data {
	struct lwip_network_desc *lwip;
	struct no_os_irq_ctrl_desc *nvic;
	struct no_os_irq_ctrl_desc *gpio_irq;
	TaskHandle_t net_task_handle;
};

static uint8_t adin1140_mac_address[6] = {0x00, 0x18, 0x80, 0x03, 0x25, 0x80};
static struct adin1140_net_data net_data;

static void adin1140_irq_cb(void *ctx)
{
	struct adin1140_net_data *data = ctx;
	BaseType_t higher_prio_woken = pdFALSE;

	no_os_irq_disable(data->gpio_irq, ADIN1140_INT_PIN);

	xTaskNotifyFromISR(data->net_task_handle, 1, eSetBits,
			   &higher_prio_woken);
	portYIELD_FROM_ISR(higher_prio_woken);
}

static void
lwiperf_report(void *arg, enum lwiperf_report_type report_type,
	       const ip_addr_t *local_addr, u16_t local_port,
	       const ip_addr_t *remote_addr, u16_t remote_port,
	       u32_t bytes_transferred, u32_t ms_duration,
	       u32_t bandwidth_kbitpsec)
{
	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(local_addr);
	LWIP_UNUSED_ARG(local_port);

	pr_info("IPERF report: type=%d, remote: %s:%d, total bytes: %"U32_F
		", duration in ms: %"U32_F", Mbits/s: %"U32_F".%03"U32_F"\n",
		(int)report_type, ipaddr_ntoa(remote_addr), (int)remote_port,
		bytes_transferred, ms_duration, bandwidth_kbitpsec / 1000,
		bandwidth_kbitpsec % 1000);
}

static int setup_interrupt(struct adin1140_net_data *data)
{
	struct no_os_irq_init_param nvic_ip = {
		.irq_ctrl_id = ADIN1140_NVIC_IRQ_ID,
		.platform_ops = ADIN1140_NVIC_IRQ_OPS,
		.extra = NULL,
	};
	struct no_os_irq_init_param gpio_irq_ip = {
		.irq_ctrl_id = ADIN1140_INT_IRQ_ID,
		.platform_ops = ADIN1140_INT_IRQ_OPS,
		.extra = ADIN1140_INT_IRQ_EXTRA,
	};
	struct no_os_callback_desc gpio_cb = {
		.callback = adin1140_irq_cb,
		.ctx = data,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
	};
	int ret;

	ret = no_os_irq_ctrl_init(&data->nvic, &nvic_ip);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_init(&data->gpio_irq, &gpio_irq_ip);
	if (ret)
		return ret;

	ret = no_os_irq_register_callback(data->gpio_irq, ADIN1140_INT_PIN,
					  &gpio_cb);
	if (ret)
		return ret;

	no_os_irq_set_priority(data->nvic, ADIN1140_INT_GPIO_IRQn,
			       NET_INT_IRQ_PRIORITY);

	ret = no_os_irq_trigger_level_set(data->gpio_irq, ADIN1140_INT_PIN,
					  ADIN1140_INT_TRIG);
	if (ret)
		return ret;

	ret = no_os_irq_enable(data->gpio_irq, ADIN1140_INT_PIN);
	if (ret)
		return ret;

	ret = no_os_irq_enable(data->nvic, ADIN1140_INT_GPIO_IRQn);
	if (ret)
		return ret;

	return 0;
}

static void net_task(void *param)
{
	struct adin1140_init_param adin1140_ip = { 0 };
	struct lwip_network_param lwip_param = { 0 };
	struct adin1140_net_data *data = param;
	struct iperf_menu_selection sel;
	struct iperf_plca_selection plca = {
		.enabled  = true,
		.node_id  = 1,
		.node_cnt = 8,
	};
	const char *def_ip = "";
	uint32_t def_rate = 5000000;
	uint32_t def_secs = 10;
	uint32_t reg_val;
	int ret;

	adin1140_ip.comm_param = adin1140_spi_ip;
	adin1140_ip.mac_cfg = (struct adin1140_mac_cfg) {
		.cps   = 0x6,
		.zarfe = true,
	};

	/* Prompt for PLCA settings before bringing up the MAC/PHY, so the chosen
	 * enable/role/count are applied by no_os_lwip_init(). Pressing Enter through
	 * keeps the defaults (PLCA on, follower id 1, 8 nodes). */
	iperf_menu_plca_select(&plca);

	adin1140_ip.plca_cfg = (struct adin1140_plca_cfg) {
		.enabled   = plca.enabled,
		.node_id   = plca.node_id,
		.node_cnt  = plca.node_cnt,
		.to_tmr    = 0x20,
		.burst_cnt = 0,
		.burst_tmr = 0,
	};
	memcpy(adin1140_ip.mac_address, adin1140_mac_address,
	       NETIF_MAX_HWADDR_LEN);

	lwip_param.platform_ops = &adin1140_lwip_ops;
	lwip_param.mac_param = &adin1140_ip;
	lwip_param.extra = NULL;
	memcpy(lwip_param.hwaddr, adin1140_mac_address, NETIF_MAX_HWADDR_LEN);

	ret = no_os_lwip_init(&data->lwip, &lwip_param);
	if (ret) {
		pr_info("no_os_lwip_init failed: %d\n", ret);
		vTaskDelete(NULL);
		return;
	}

	ret = setup_interrupt(data);
	if (ret) {
		pr_info("setup_interrupt failed: %d\n", ret);
		vTaskDelete(NULL);
		return;
	}

	/* Defaults for the client prompt: prefer the build-time CONFIG_* values
	 * when present, otherwise sensible built-ins. */
#ifdef CONFIG_ADIN1140_IPERF_UDP_CLIENT_IP
	def_ip = CONFIG_ADIN1140_IPERF_UDP_CLIENT_IP;
	def_rate = CONFIG_ADIN1140_IPERF_UDP_CLIENT_RATE;
	def_secs = CONFIG_ADIN1140_IPERF_UDP_CLIENT_TIME;
#endif

	iperf_menu_select(&sel, def_ip, def_rate, def_secs);

	if (sel.mode == IPERF_MENU_UDP_CLIENT ||
	    sel.mode == IPERF_MENU_TCP_CLIENT) {
		ip_addr_t remote;

		if (!ipaddr_aton(sel.ip, &remote)) {
			pr_info("Invalid client IP '%s' - starting servers\n",
				sel.ip);
			sel.mode = IPERF_MENU_SERVERS;
		} else if (sel.mode == IPERF_MENU_UDP_CLIENT) {
			pr_info("Starting lwiperf UDP client to %s (%lu.%03lu Mbit/s, %lu s)\n",
				sel.ip,
				(unsigned long)(sel.rate_bps / 1000000),
				(unsigned long)((sel.rate_bps % 1000000) / 1000),
				(unsigned long)sel.duration_s);
			lwiperf_start_udp_client(&remote, LWIPERF_UDP_PORT_DEFAULT,
						 sel.rate_bps,
						 -100 * (s32_t)sel.duration_s,
						 lwiperf_report, NULL);
		} else {
			pr_info("Starting lwiperf TCP client to %s (%lu s)\n",
				sel.ip, (unsigned long)sel.duration_s);
			lwiperf_start_tcp_client(&remote, LWIPERF_TCP_PORT_DEFAULT,
						 LWIPERF_CLIENT,
						 -100 * (s32_t)sel.duration_s,
						 lwiperf_report, NULL);
		}
	}

	if (sel.mode == IPERF_MENU_SERVERS) {
		pr_info("Starting lwiperf TCP server on port %d\n",
			LWIPERF_TCP_PORT_DEFAULT);
		lwiperf_start_tcp_server_default(lwiperf_report, NULL);

		pr_info("Starting lwiperf UDP server on port %d\n",
			LWIPERF_UDP_PORT_DEFAULT);
		lwiperf_start_udp_server_default(lwiperf_report, NULL);
	}

	while (1) {
		uint32_t sleep_ms;

		no_os_lwip_step(data->lwip, NULL);

		no_os_irq_enable(data->gpio_irq, ADIN1140_INT_PIN);

		/* Block only until the next lwIP timeout is due, so timer-driven
		 * work (e.g. the iperf UDP client's pacing) runs on schedule
		 * instead of waiting out a fixed 100 ms. RX still wakes us early
		 * via the task notification from the ADIN1140 IRQ. Clamp to 100 ms
		 * so we always service the stack periodically. */
		sleep_ms = sys_timeouts_sleeptime();
		if (sleep_ms > 100)
			sleep_ms = 100;

		ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(sleep_ms));
	}
}

int example_main()
{
	BaseType_t task_ret;

	task_ret = xTaskCreate(net_task, "net_task", configIIO_APP_STACK_SIZE,
			       &net_data, 3, &net_data.net_task_handle);
	if (task_ret != pdPASS)
		return -ENOMEM;

	vTaskStartScheduler();

	return -1;
}
