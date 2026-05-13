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

static uint8_t adin1140_mac_address[6] = {0x00, 0x18, 0x80, 0x03, 0x25, 0x80};

static struct adin1140_init_param adin1140_ip;

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

	pr_info("Starting lwiperf server on port %d\n", LWIPERF_TCP_PORT_DEFAULT);
	lwiperf_start_tcp_server_default(lwiperf_report, NULL);

	pr_info("iperf server started. Network is ready for iperf testing.\n");

	/* Keep the application running */
	while (1) {
		no_os_lwip_step(lwip_desc, NULL);
		no_os_mdelay(10);
	}

	return 0;
}