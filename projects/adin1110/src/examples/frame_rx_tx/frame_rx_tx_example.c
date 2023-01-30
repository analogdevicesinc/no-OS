/***************************************************************************//**
 *   @file   frame_rx_tx_example.c
 *   @brief  Frame receive and transmit example header for the ADIN1110 project
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include <string.h>
#include "frame_rx_tx_example.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

#include "adin1110.h"

uint8_t eth_frame[] = {
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44,
	0x11, 0x11,
};

uint8_t eth_rx_buff[ADIN1110_BUFF_LEN];

/***************************************************************************//**
 * @brief Frame receive an transmit example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int frame_rx_tx_example_main()
{
	uint32_t ucast_frame_rx_cnt;
	uint32_t bcast_frame_rx_cnt;
	uint32_t mcast_frame_rx_cnt;
	uint32_t ucast_frame_tx_cnt;
	uint32_t bcast_frame_tx_cnt;
	uint32_t mcast_frame_tx_cnt;
	size_t i;
	int ret;

	uint8_t mac_source[ADIN1110_ETH_ALEN] = {0xCA, 0x2F, 0xB7, 0x10, 0x23, 0x63};
	uint8_t mac_dest[ADIN1110_ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	struct adin1110_desc *adin1110;
	struct adin1110_init_param adin1110_ip = {
		.chip_type = ADIN2111,
		.comm_param = adin1110_spi_ip,
		.reset_param = adin1110_reset_gpio_ip,
		.append_crc = false,
	};

	struct adin1110_eth_buff eth_tx_buff = {
		.ethertype = {0x8, 0x0},
		.len = NO_OS_ARRAY_SIZE(eth_frame) + ADIN1110_ETH_HDR_LEN,
		.payload = eth_frame
	};

	struct adin1110_eth_buff eth_rx = {
		.payload = eth_rx_buff
	};

	memcpy(adin1110_ip.mac_address, mac_source, ADIN1110_ETH_ALEN);
	memcpy(eth_tx_buff.mac_source, mac_source, ADIN1110_ETH_ALEN);
	memcpy(eth_tx_buff.mac_dest, mac_dest, ADIN1110_ETH_ALEN);

	ret = adin1110_init(&adin1110, &adin1110_ip);
	if (ret)
		return ret;

	ret = adin1110_set_promisc(adin1110, 0, true);
	if (ret)
		return ret;

	while (1) {
		/* Clear the screen */
		printf("%c",27);
		printf("%c",'[');
		printf("%c",'2');
		printf("%c",'J');

		ret = adin1110_reg_read(adin1110, ADIN1110_TX_UCAST_CNT_REG,
					&ucast_frame_tx_cnt);
		if (ret)
			goto error;

		ret = adin1110_reg_read(adin1110, ADIN1110_TX_BCAST_CNT_REG,
					&bcast_frame_tx_cnt);
		if (ret)
			goto error;

		ret = adin1110_reg_read(adin1110, ADIN1110_TX_MCAST_CNT_REG,
					&mcast_frame_tx_cnt);
		if (ret)
			goto error;

		ret = adin1110_reg_read(adin1110, ADIN1110_RX_UCAST_CNT_REG,
					&ucast_frame_rx_cnt);
		if (ret)
			goto error;

		ret = adin1110_reg_read(adin1110, ADIN1110_RX_BCAST_CNT_REG,
					&bcast_frame_rx_cnt);
		if (ret)
			goto error;

		ret = adin1110_reg_read(adin1110, ADIN1110_RX_MCAST_CNT_REG,
					&mcast_frame_rx_cnt);
		if (ret)
			goto error;

		printf("TX unicast frames: %d\n", ucast_frame_tx_cnt);
		printf("TX boardcast frames: %d\n", bcast_frame_tx_cnt);
		printf("TX multicast frames: %d\n", mcast_frame_tx_cnt);
		printf("RX unicast frames: %d\n", ucast_frame_rx_cnt);
		printf("RX boardcast frames: %d\n", bcast_frame_rx_cnt);
		printf("RX multicast frames: %d\n\n", mcast_frame_rx_cnt);

		ret = adin1110_write_fifo(adin1110, 0, &eth_tx_buff);
		if (ret)
			goto error;

		eth_rx.len = 0;
		ret = adin1110_read_fifo(adin1110, 0, &eth_rx);
		if (ret)
			goto error;

		if (eth_rx.len) {
			printf("Received the frame:\n");
			printf("\nDestination MAC: %02X:%02X:%02X:%02X:%02X:%02X",
			       eth_rx.mac_dest[0], eth_rx.mac_dest[1], eth_rx.mac_dest[2],
			       eth_rx.mac_dest[3], eth_rx.mac_dest[4], eth_rx.mac_dest[5]);
			printf("\nSource MAC: %02X:%02X:%02X:%02X:%02X:%02X",
			       eth_rx.mac_source[0], eth_rx.mac_source[1], eth_rx.mac_source[2],
			       eth_rx.mac_source[3], eth_rx.mac_source[4], eth_rx.mac_source[5]);
			printf("\nEthertype: 0x%02X", eth_rx.ethertype);
			printf("\n\nPayload: \n");
			for (i = 0; i < eth_rx.len; i++) {
				printf("0x%02X ", eth_rx.payload[i]);
				if ((i + 1) % 20 == 0)
					printf("\n");
			}
		}

		no_os_mdelay(5000);
	}

error:
	printf("Error!\n");
	adin1110_remove(adin1110);

	return 0;
}
