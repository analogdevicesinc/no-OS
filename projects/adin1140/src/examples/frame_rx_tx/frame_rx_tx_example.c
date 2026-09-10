/***************************************************************************//**
 *   @file   frame_rx_tx_example.c
 *   @brief  Frame receive and transmit example for the ADIN1140 project.
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
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

#include "adin1140.h"

#define RX_BUFF_LEN	1530

static uint8_t tx_payload[] = {
	0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x41, 0x44,
	0x49, 0x4E, 0x31, 0x31, 0x34, 0x30, 0x21, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static uint8_t rx_buff[RX_BUFF_LEN];

/***************************************************************************//**
 * @brief Frame receive and transmit example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	struct adin1140_desc *adin1140;
	uint32_t link_state;
	uint32_t tx_cnt;
	uint32_t rx_cnt;
	size_t i;
	int ret;

	uint8_t mac_addr[ADIN1140_ETH_ALEN] = {0xCA, 0x2F, 0xB7, 0x10, 0x23, 0x63};
	uint8_t mac_dest[ADIN1140_ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	struct adin1140_init_param adin1140_ip = {
		.comm_param = adin1140_spi_ip,
		.mac_cfg = {
			.cps   = 0x6,
			.zarfe = true,
		},
	};

	struct adin1140_eth_buff eth_tx = {
		.ethertype = {0x08, 0x00},
		.len = sizeof(tx_payload) + ADIN1140_ETH_HDR_LEN,
		.payload = tx_payload,
	};

	struct adin1140_eth_buff eth_rx = {
		.payload = rx_buff,
	};

	memcpy(adin1140_ip.mac_address, mac_addr, ADIN1140_ETH_ALEN);
	memcpy(eth_tx.mac_source, mac_addr, ADIN1140_ETH_ALEN);
	memcpy(eth_tx.mac_dest, mac_dest, ADIN1140_ETH_ALEN);

	printf("ADIN1140 Frame RX/TX Example\n");

	ret = adin1140_init(&adin1140, &adin1140_ip);
	if (ret) {
		printf("adin1140_init failed: %d\n", ret);
		return ret;
	}

	printf("ADIN1140 initialized successfully\n");

	ret = adin1140_set_promisc(adin1140, true);
	if (ret) {
		printf("Failed to set promiscuous mode: %d\n", ret);
		goto cleanup;
	}

	ret = adin1140_broadcast_filter(adin1140, true);
	if (ret) {
		printf("Failed to set broadcast filter: %d\n", ret);
		goto cleanup;
	}

	while (1) {
		ret = adin1140_link_state(adin1140, &link_state);
		if (ret)
			goto cleanup;

		ret = adin1140_reg_read(adin1140, ADIN1140_TX_FRAME_CNT_REG,
					&tx_cnt);
		if (ret)
			goto cleanup;

		ret = adin1140_reg_read(adin1140, ADIN1140_RX_FRAME_CNT_REG,
					&rx_cnt);
		if (ret)
			goto cleanup;

		printf("Link: %s | TX: %lu | RX: %lu\n",
		       link_state ? "UP" : "DOWN",
		       (unsigned long)tx_cnt, (unsigned long)rx_cnt);

		ret = adin1140_write_fifo(adin1140, &eth_tx);
		if (ret && ret != -EAGAIN)
			goto cleanup;

		eth_rx.len = 0;
		ret = adin1140_read_fifo(adin1140, &eth_rx);
		if (ret && ret != -ENOENT)
			goto cleanup;

		if (eth_rx.len) {
			printf("RX frame (%lu bytes): "
			       "%02X:%02X:%02X:%02X:%02X:%02X -> "
			       "%02X:%02X:%02X:%02X:%02X:%02X\n",
			       (unsigned long)eth_rx.len,
			       eth_rx.mac_source[0], eth_rx.mac_source[1],
			       eth_rx.mac_source[2], eth_rx.mac_source[3],
			       eth_rx.mac_source[4], eth_rx.mac_source[5],
			       eth_rx.mac_dest[0], eth_rx.mac_dest[1],
			       eth_rx.mac_dest[2], eth_rx.mac_dest[3],
			       eth_rx.mac_dest[4], eth_rx.mac_dest[5]);

			printf("  Payload: ");
			for (i = 0; i < eth_rx.len - ADIN1140_ETH_HDR_LEN &&
			     i < 32; i++)
				printf("%02X ", eth_rx.payload[i]);
			if (eth_rx.len - ADIN1140_ETH_HDR_LEN > 32)
				printf("...");
			printf("\n");
		}

		no_os_mdelay(200);
	}

cleanup:
	printf("Error: %d\n", ret);
	adin1140_remove(adin1140);

	return ret;
}
