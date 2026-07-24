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

#include "common_data.h"
#include "lwip_socket.h"
#include "lwip_adin1140.h"
#include "adin1140.h"
#include "oa_tc6.h"
#include "lwip/apps/lwiperf.h"
#include "lwip/ip_addr.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_irq.h"

#include "FreeRTOS.h"
#include "task.h"

struct adin1140_net_data {
	struct lwip_network_desc *lwip;
	struct adin1140_desc *adin1140;
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

	adin1140_set_irq_flag(data->adin1140);

	no_os_irq_disable(data->gpio_irq, ADIN1140_INT_PIN);
	no_os_irq_disable(data->nvic, ADIN1140_INT_GPIO_IRQn);

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
		", duration in ms: %"U32_F", kbits/s: %"U32_F"\n",
		(int)report_type, ipaddr_ntoa(remote_addr), (int)remote_port,
		bytes_transferred, ms_duration, bandwidth_kbitpsec);
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

	no_os_irq_set_priority(data->nvic, ADIN1140_INT_GPIO_IRQn, 5);
	no_os_irq_set_priority(data->nvic, SysTick_IRQn, 1);

	ret = no_os_irq_trigger_level_set(data->gpio_irq, ADIN1140_INT_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
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
	struct adin1140_net_data *data = param;
	struct adin1140_init_param adin1140_ip = { 0 };
	struct lwip_network_param lwip_param = { 0 };
	uint32_t reg_val;
	int ret;

	adin1140_ip.comm_param = adin1140_spi_ip;
	adin1140_ip.mac_cfg = (struct adin1140_mac_cfg) {
		.cps   = 0x6,
		.zarfe = true,
	};
	/*
	 * PLCA follower (node 1). The media converter is the coordinator/beacon
	 * source. PLCA_NODE_CNT is being swept to find the value the converter
	 * expects: on some coordinators a follower is only admitted if its
	 * node_id is within the coordinator's configured node count, so this
	 * must be >= 2 and typically matches the converter's setting (common
	 * defaults: 8).
	 */
	adin1140_ip.plca_cfg = (struct adin1140_plca_cfg) {
		.enabled   = true,
		.node_id   = 1,
		.node_cnt  = 8,
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

	data->adin1140 = data->lwip->mac_desc;

	ret = setup_interrupt(data);
	if (ret) {
		pr_info("setup_interrupt failed: %d\n", ret);
		vTaskDelete(NULL);
		return;
	}

	pr_info("Starting lwiperf server on port %d\n",
		LWIPERF_TCP_PORT_DEFAULT);
	lwiperf_start_tcp_server_default(lwiperf_report, NULL);

	struct oa_tc6_stats sw_prev = {0};
	struct oa_tc6_stats sw_now;
	struct adin1140_stats hw;
	struct no_os_time t_prev = no_os_get_time();

	while (1) {
		ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000));

		no_os_lwip_step(data->lwip, NULL);

		no_os_irq_enable(data->gpio_irq, ADIN1140_INT_PIN);

		struct no_os_time t_now = no_os_get_time();
		uint32_t elapsed_ms = (t_now.s - t_prev.s) * 1000 +
				      (t_now.us - t_prev.us) / 1000;
		if (elapsed_ms >= 10000) {
			int sret = oa_tc6_get_stats(data->adin1140->oa_desc, &sw_now);
			int hret = adin1140_get_stats(data->adin1140, &hw);
			if (!sret && !hret) {
				uint64_t rx_dbytes = sw_now.rx_bytes - sw_prev.rx_bytes;
				uint64_t tx_dbytes = sw_now.tx_bytes - sw_prev.tx_bytes;
				/* kbit/s = bytes*8 / ms  (bytes/ms * 8 = kbit/s) */
				uint32_t rx_kbps = (uint32_t)((rx_dbytes * 8) / elapsed_ms);
				uint32_t tx_kbps = (uint32_t)((tx_dbytes * 8) / elapsed_ms);

				pr_info("stats: rx %lu kbit/s tx %lu kbit/s | "
					"rx_fr=%llu tx_fr=%llu drop_fd=%lu nobuf=%lu exst=%lu | "
					"hw rx=%lu tx=%lu crc=%lu algn=%lu\n",
					(unsigned long)rx_kbps, (unsigned long)tx_kbps,
					(unsigned long long)sw_now.rx_frames,
					(unsigned long long)sw_now.tx_frames,
					(unsigned long)sw_now.rx_drop_fd,
					(unsigned long)sw_now.rx_drop_nobuf,
					(unsigned long)sw_now.exst_events,
					(unsigned long)hw.rx_frames,
					(unsigned long)hw.tx_frames,
					(unsigned long)hw.rx_crc_err,
					(unsigned long)hw.rx_align_err);

				sw_prev = sw_now;
				t_prev = t_now;
			}
		}
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
