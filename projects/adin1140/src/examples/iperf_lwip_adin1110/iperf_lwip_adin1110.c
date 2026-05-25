/***************************************************************************//**
 *   @file   iperf_lwip_adin1110.c
 *   @brief  iperf LWIP example using the ADIN1110 (10BASE-T1L) on an
 *           apard32690-class max32690 board. Hosted in the adin1140 project as
 *           a fast-iteration testbed; the apard32690 project tree is untouched.
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

#include "lwip_socket.h"
#include "lwip_adin1110.h"
#include "adin1110.h"
#include "lwip/apps/lwiperf.h"
#include "lwip/ip_addr.h"
#include "no_os_print_log.h"
#include "maxim_gpio.h"
#include "maxim_spi.h"
#include "parameters.h"

/*
 * Board pinout matches the apard32690 (EVAL-ADIN1110 + MAX32690) and is
 * intentionally hard-coded here so the shared adin1140 platform parameters
 * (which target the EVAL-ADIN1140) stay untouched.
 */

/*
 * The adin1140 project compiles with -DCONFIG_SPI_DMA=1, which forces
 * max_spi_transfer() through the DMA path. dma_param must be set so
 * no_os_spi_init() actually initializes the DMA controller; otherwise
 * the first transfer hangs in the DMA-completion poll.
 */
static struct max_spi_init_param adin1110_spi_extra = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
	.dma_param = &adin1140_dma_ip,
};

static const struct no_os_gpio_init_param adin1110_rst_gpio_ip = {
	.port = 0,
	.number = 15,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &(struct max_gpio_init_param) {
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	},
};

static const struct no_os_spi_init_param adin1110_spi_ip = {
	.device_id = 3,
	.max_speed_hz = 25000000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = &max_spi_ops,
	.chip_select = 0,
	.extra = (void *)&adin1110_spi_extra,
};

static uint8_t adin1110_mac_address[NETIF_MAX_HWADDR_LEN] = {
	0x00, 0x18, 0x80, 0x03, 0x25, 0x80
};

static struct adin1110_init_param adin1110_ip = {
	.chip_type = ADIN1110,
	.append_crc = false,
};

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

int example_main()
{
	struct lwip_network_param lwip_param = { 0 };
	struct lwip_network_desc *lwip_desc;
	int ret;

	adin1110_ip.comm_param = adin1110_spi_ip;
	adin1110_ip.reset_param = adin1110_rst_gpio_ip;
	memcpy(adin1110_ip.mac_address, adin1110_mac_address,
	       NETIF_MAX_HWADDR_LEN);

	lwip_param.platform_ops = &adin1110_lwip_ops;
	lwip_param.mac_param = &adin1110_ip;
	lwip_param.extra = NULL;
	memcpy(lwip_param.hwaddr, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);

	ret = no_os_lwip_init(&lwip_desc, &lwip_param);
	if (ret)
		return ret;

	pr_info("Starting lwiperf server on port %d\n",
		LWIPERF_TCP_PORT_DEFAULT);
	lwiperf_start_tcp_server_default(lwiperf_report, NULL);

	pr_info("iperf server started. Network is ready for iperf testing.\n");

	while (1)
		no_os_lwip_step(lwip_desc, NULL);

	return 0;
}
