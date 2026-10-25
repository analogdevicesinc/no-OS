/***************************************************************************//**
 *   @file   oa_tc6_perf_example.c
 *   @brief  Implementation of the ADIN1110 OA TC6 full duplex performance example.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "common_data.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "adin1110.h"

/*
 * IEEE 802 Local Experimental Ethertype 1. Used to tag the test frames so
 * they can be told apart from unrelated traffic on the link.
 */
#define OA_TC6_PERF_ETHERTYPE		0x88B5
#define OA_TC6_PERF_PAYLOAD_LEN		1500
#define OA_TC6_PERF_PRINT_INTERVAL_S	5

/*
 * SPI protocol/protection combination selected via Kconfig. Each combination
 * sets the ADIN1110 init params that select the generic register-based or the
 * OA TC6 native chunk SPI protocol, with or without control-transaction
 * protection, and provides a human readable mode string for the banner.
 */
#if defined(CONFIG_ADIN1110_OA_TC6_PERF_GENERIC_UNPROTECTED)
#define OA_TC6_PERF_APPEND_CRC		false
#define OA_TC6_PERF_OA_TC6_SPI		false
#define OA_TC6_PERF_MODE_STR		"generic SPI, unprotected"
#elif defined(CONFIG_ADIN1110_OA_TC6_PERF_GENERIC_PROTECTED)
#define OA_TC6_PERF_APPEND_CRC		true
#define OA_TC6_PERF_OA_TC6_SPI		false
#define OA_TC6_PERF_MODE_STR		"generic SPI, protected"
#elif defined(CONFIG_ADIN1110_OA_TC6_PERF_OA_TC6_PROTECTED)
#error "The ADIN1110 driver does not expose protected OA TC6 SPI"
#else /* CONFIG_ADIN1110_OA_TC6_PERF_OA_TC6_UNPROTECTED (default) */
#define OA_TC6_PERF_APPEND_CRC		false
#define OA_TC6_PERF_OA_TC6_SPI		true
#define OA_TC6_PERF_MODE_STR		"OA TC6 SPI, unprotected"
#endif

static const uint8_t broadcast_mac[ADIN1110_ETH_ALEN] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static uint64_t time_diff_us(const struct no_os_time *start,
			     const struct no_os_time *end)
{
	int64_t sec_diff = (int64_t)end->s - (int64_t)start->s;
	int64_t us_diff = (int64_t)end->us - (int64_t)start->us;

	return (uint64_t)(sec_diff * 1000000 + us_diff);
}

/***************************************************************************//**
 * @brief Continuously transmit frames over the ADIN1110 (configured for the
 *        SPI protocol/protection combination selected via Kconfig) while
 *        concurrently receiving frames sent by a companion PC script, and
 *        periodically report the achieved full duplex throughput.
 *
 * @return ret - Result of the example execution. Does not return in the
 *		  absence of an error, as the benchmark runs indefinitely.
*******************************************************************************/
int example_main()
{
	struct adin1110_desc *adin1110;
	struct adin1110_eth_buff tx_buff;
	struct adin1110_eth_buff rx_buff;
	struct no_os_time last_print;
	struct no_os_time now;
	uint8_t tx_payload[OA_TC6_PERF_PAYLOAD_LEN];
	uint8_t rx_payload[OA_TC6_PERF_PAYLOAD_LEN];
	uint32_t link_state;
	uint32_t tx_seq = 0;
	uint64_t tx_frames = 0, tx_bytes = 0;
	uint64_t rx_frames = 0, rx_bytes = 0;
	uint64_t elapsed_us;
	bool busy;
	int ret;
#if defined(CONFIG_OA_TC6)
	struct oa_tc6_frame_buffer *tx_frame;
	struct oa_tc6_frame_buffer *rx_frame;
	uint32_t cycles_last, idle_last;
	uint32_t events;
#endif

	struct adin1110_init_param adin1110_ip = {
		.chip_type = ADIN1110,
		.reset_param = adin1110_reset_gpio_ip,
		.mac_address = {0x00, 0x18, 0x80, 0x03, 0x25, 0x80},
		.append_crc = OA_TC6_PERF_APPEND_CRC,
		.oa_tc6_spi = OA_TC6_PERF_OA_TC6_SPI,
		.oa_tc6_async = OA_TC6_PERF_OA_TC6_SPI,
	};
	printf("OA TC6 perf\n");

	ret = adin1110_spi_init(&adin1110_ip);
	if (ret) {
		printf("Error during CAPI SPI init (%d)\n", ret);
		return ret;
	}

	ret = adin1110_init(&adin1110, &adin1110_ip);
	if (ret) {
		printf("Error during ADIN1110 config (%d)\n", ret);
		return ret;
	}

	/* Accept frames regardless of their destination MAC address */
	ret = adin1110_set_promisc(adin1110, 0, true);
	if (ret) {
		printf("Error enabling promiscuous mode (%d)\n", ret);
		goto out;
	}

	printf("Waiting for link...\n");
	do {
		ret = adin1110_link_state(adin1110, &link_state);
		if (ret) {
			printf("Error reading link state (%d)\n", ret);
			goto out;
		}
		no_os_mdelay(100);
	} while (!link_state);

#if defined(CONFIG_OA_TC6)
	ret = adin1110_int_init(adin1110);
	if (ret) {
		printf("Error enabling the nINT interrupt (%d)\n", ret);
		goto out;
	}
#endif

	printf("Link up. Starting %s full duplex benchmark.\n",
	       OA_TC6_PERF_MODE_STR);

	memset(tx_payload, 0xA5, sizeof(tx_payload));

	memcpy(tx_buff.mac_dest, broadcast_mac, ADIN1110_ETH_ALEN);
	memcpy(tx_buff.mac_source, adin1110_ip.mac_address, ADIN1110_ETH_ALEN);
	no_os_put_unaligned_be16(OA_TC6_PERF_ETHERTYPE, tx_buff.ethertype);
	tx_buff.len = ADIN1110_ETH_HDR_LEN + OA_TC6_PERF_PAYLOAD_LEN;
	tx_buff.payload = tx_payload;

	rx_buff.payload = rx_payload;

	last_print = no_os_get_time();

#if defined(CONFIG_OA_TC6)
	cycles_last = adin1110_cpu_cycles();
	idle_last = adin1110_idle_cycles;
#endif

	while (1) {
#if defined(CONFIG_OA_TC6)
		events = adin1110_irq_events;
#endif
		busy = false;

		/*
		 * Drain RX first: the datapath poll it runs also retires sent
		 * frames, so the TX queue has room below.
		 */
#if defined(CONFIG_OA_TC6)
		/* Zero copy: frames are checked and filled in the driver's pools */
		while (!adin1110_get_rx_frame(adin1110, 0, &rx_frame)) {
			busy = true;
			if (rx_frame->len >= ADIN1110_ETH_HDR_LEN &&
			    no_os_get_unaligned_be16(&rx_frame->data[12]) == OA_TC6_PERF_ETHERTYPE) {
				rx_frames++;
				rx_bytes += rx_frame->len;
			}
			adin1110_put_rx_frame(adin1110, rx_frame);
		}

		ret = adin1110_get_tx_frame(adin1110, &tx_frame);
		if (!ret) {
			/* The payload stays as handed out (zeroed) */
			memcpy(tx_frame->data, tx_buff.mac_dest, ADIN1110_ETH_HDR_LEN);
			no_os_put_unaligned_be32(tx_seq,
						 &tx_frame->data[ADIN1110_ETH_HDR_LEN]);
			tx_frame->len = tx_buff.len;
			ret = adin1110_send_frame(adin1110, 0, tx_frame);
		}
		if (!ret) {
			busy = true;
			tx_seq++;
			tx_frames++;
			tx_bytes += tx_buff.len;
		}
#else
		do {
			rx_buff.len = 0;
			ret = adin1110_read_fifo(adin1110, 0, &rx_buff);
			if (ret || !rx_buff.len)
				break;

			busy = true;
			if (rx_buff.len >= ADIN1110_ETH_HDR_LEN &&
			    no_os_get_unaligned_be16(rx_buff.ethertype) == OA_TC6_PERF_ETHERTYPE) {
				rx_frames++;
				rx_bytes += rx_buff.len;
			}
		} while (1);

		no_os_put_unaligned_be32(tx_seq, tx_payload);
		ret = adin1110_write_fifo(adin1110, 0, &tx_buff);
		if (!ret) {
			busy = true;
			tx_seq++;
			tx_frames++;
			tx_bytes += tx_buff.len;
		}
#endif

		now = no_os_get_time();
		elapsed_us = time_diff_us(&last_print, &now);
		if (elapsed_us >= (uint64_t)OA_TC6_PERF_PRINT_INTERVAL_S * 1000000) {
			double elapsed_s = elapsed_us / 1000000.0;
			double tx_mbps = (tx_bytes * 8) / elapsed_s / 1000000.0;
			double rx_mbps = (rx_bytes * 8) / elapsed_s / 1000000.0;

			printf("TX: %llu frames, %llu bytes, %.2f Mbps | RX: %llu frames, %llu bytes, %.2f Mbps",
			       (unsigned long long)tx_frames, (unsigned long long)tx_bytes, tx_mbps,
			       (unsigned long long)rx_frames, (unsigned long long)rx_bytes, rx_mbps);
#if defined(CONFIG_OA_TC6)
			uint32_t cycles = adin1110_cpu_cycles();
			uint32_t idle = adin1110_idle_cycles;
			double busy_cycles = (double)(cycles - cycles_last) -
					     (double)(idle - idle_last);

			printf(" | CPU: %.1f%% of %lu MHz",
			       100.0 * busy_cycles / (adin1110_cpu_hz() * elapsed_s),
			       (unsigned long)(adin1110_cpu_hz() / 1000000));
			cycles_last = cycles;
			idle_last = idle;
#endif
			printf("\n");

			tx_frames = 0;
			tx_bytes = 0;
			rx_frames = 0;
			rx_bytes = 0;
			last_print = now;
		}

#if defined(CONFIG_OA_TC6)
		if (!busy)
			adin1110_cpu_idle(events);
#else
		(void)busy;
#endif
	}

out:
	adin1110_remove(adin1110);

	return ret;
}
