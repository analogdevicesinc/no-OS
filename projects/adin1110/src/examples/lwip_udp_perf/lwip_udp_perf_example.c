/***************************************************************************//**
 *   @file   lwip_udp_perf_example.c
 *   @brief  Implementation of the ADIN1110 lwIP UDP throughput example.
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
#include "lwip_socket.h"
#include "lwip_adin1110.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"
#include "lwip/etharp.h"

/* The largest datagram that fits a single 1514-byte Ethernet frame */
#define UDP_PERF_PAYLOAD_LEN		1472
#define UDP_PERF_PRINT_INTERVAL_S	5
/*
 * Datagrams sent per loop pass at most, so a pass always returns to the lwIP
 * poll. Matches the driver's TX pool, which bounds what can go out anyway.
 */
#define UDP_PERF_TX_BURST		2
#define UDP_PERF_ARP_TIMEOUT_MS		5000

static uint8_t udp_perf_payload[UDP_PERF_PAYLOAD_LEN];
static uint64_t rx_datagrams, rx_bytes;
static bool rx_seen;

static uint64_t time_diff_us(const struct no_os_time *start,
			     const struct no_os_time *end)
{
	int64_t sec_diff = (int64_t)end->s - (int64_t)start->s;
	int64_t us_diff = (int64_t)end->us - (int64_t)start->us;

	return (uint64_t)(sec_diff * 1000000 + us_diff);
}

static void udp_perf_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p,
			  const ip_addr_t *addr, u16_t port)
{
	rx_datagrams++;
	rx_bytes += p->tot_len;
	rx_seen = true;

	pbuf_free(p);
}

/*
 * Send one datagram. The payload pbuf references the static buffer, so lwIP
 * only adds its headers and the netif copies the data once, into the
 * ADIN1110 TX buffer.
 */
static int udp_perf_send(struct udp_pcb *pcb, const ip_addr_t *host,
			 uint32_t seq)
{
	struct pbuf *p;
	err_t err;

	no_os_put_unaligned_be32(seq, udp_perf_payload);

	p = pbuf_alloc(PBUF_RAW, UDP_PERF_PAYLOAD_LEN, PBUF_REF);
	if (!p)
		return -ENOMEM;

	p->payload = udp_perf_payload;
	err = udp_sendto(pcb, p, host, CONFIG_ADIN1110_UDP_PERF_PORT);
	pbuf_free(p);

	return err == ERR_OK ? 0 : -EIO;
}

/*
 * Until the host's MAC address is known lwIP queues datagrams in ARP and
 * reports success without sending anything, so resolve it before streaming.
 */
static int udp_perf_resolve_host(struct lwip_network_desc *lwip_desc,
				 struct udp_pcb *pcb, const ip_addr_t *host)
{
	const ip4_addr_t *ip_ret;
	struct eth_addr *eth_ret;
	uint32_t ms;

	for (ms = 0; ms < UDP_PERF_ARP_TIMEOUT_MS; ms++) {
		if (etharp_find_addr(lwip_desc->lwip_netif, ip_2_ip4(host),
				     &eth_ret, &ip_ret) >= 0)
			return 0;

		if (!(ms % 1000))
			udp_perf_send(pcb, host, 0);

		no_os_lwip_step(lwip_desc, NULL);
		no_os_mdelay(1);
	}

	return -ETIMEDOUT;
}

/***************************************************************************//**
 * @brief Stream UDP datagrams to the PC over lwIP while counting the
 *        datagrams the PC sends back, and periodically report the UDP payload
 *        throughput in both directions and the CPU load.
 *
 * @return ret - Result of the example execution. Does not return in the
 *		  absence of an error, as the benchmark runs indefinitely.
*******************************************************************************/
int example_main()
{
	struct lwip_network_desc *lwip_desc;
	struct adin1110_desc *adin1110;
	struct no_os_time last_print;
	struct no_os_time now;
	struct udp_pcb *pcb;
	ip_addr_t host;
	uint32_t cycles_last, idle_last;
	uint32_t link_state;
	uint32_t events;
	uint32_t tx_seq = 0;
	uint64_t tx_datagrams = 0;
	uint32_t burst;
	uint64_t elapsed_us;
	bool busy;
	int ret;

	struct adin1110_init_param adin1110_ip = {
		.chip_type = ADIN1110,
		.reset_param = adin1110_reset_gpio_ip,
		.mac_address = {0x00, 0x18, 0x80, 0x03, 0x25, 0x80},
		.oa_tc6_spi = true,
		.oa_tc6_async = true,
	};
	struct lwip_network_param lwip_ip = {
		.platform_ops = &adin1110_lwip_ops,
		.mac_param = &adin1110_ip,
	};

	printf("lwIP UDP perf\n");

	memcpy(lwip_ip.hwaddr, adin1110_ip.mac_address, ADIN1110_ETH_ALEN);

	if (!ipaddr_aton(CONFIG_ADIN1110_UDP_PERF_HOST_IP, &host)) {
		printf("Invalid host IP %s\n", CONFIG_ADIN1110_UDP_PERF_HOST_IP);
		return -EINVAL;
	}

	ret = adin1110_spi_init(&adin1110_ip);
	if (ret) {
		printf("Error during CAPI SPI init (%d)\n", ret);
		return ret;
	}

	ret = no_os_lwip_init(&lwip_desc, &lwip_ip);
	if (ret) {
		printf("Error during lwIP init (%d)\n", ret);
		return ret;
	}

	adin1110 = lwip_desc->mac_desc;

	printf("Waiting for link...\n");
	do {
		ret = adin1110_link_state(adin1110, &link_state);
		if (ret) {
			printf("Error reading link state (%d)\n", ret);
			goto out;
		}
		no_os_lwip_step(lwip_desc, NULL);
		no_os_mdelay(100);
	} while (!link_state);

	ret = adin1110_int_init(adin1110);
	if (ret) {
		printf("Error enabling the nINT interrupt (%d)\n", ret);
		goto out;
	}

	pcb = udp_new();
	if (!pcb) {
		ret = -ENOMEM;
		goto out;
	}

	ret = udp_bind(pcb, IP_ADDR_ANY, CONFIG_ADIN1110_UDP_PERF_PORT);
	if (ret) {
		printf("Error binding UDP port %d (%d)\n",
		       CONFIG_ADIN1110_UDP_PERF_PORT, ret);
		ret = -EIO;
		goto out;
	}
	udp_recv(pcb, udp_perf_recv, NULL);

	ret = udp_perf_resolve_host(lwip_desc, pcb, &host);
	if (ret) {
		printf("No ARP reply from %s\n", CONFIG_ADIN1110_UDP_PERF_HOST_IP);
		goto out;
	}

	printf("Link up. Streaming %d byte UDP datagrams to %s:%d.\n",
	       UDP_PERF_PAYLOAD_LEN, CONFIG_ADIN1110_UDP_PERF_HOST_IP,
	       CONFIG_ADIN1110_UDP_PERF_PORT);

	last_print = no_os_get_time();
	cycles_last = adin1110_cpu_cycles();
	idle_last = adin1110_idle_cycles;

	while (1) {
		events = adin1110_irq_events;
		rx_seen = false;
		busy = false;

		/* Timers and RX; the datapath poll also retires sent frames */
		no_os_lwip_step(lwip_desc, NULL);
		busy = rx_seen;

		/* Only build a datagram when the driver can take its frame */
		for (burst = 0; burst < UDP_PERF_TX_BURST &&
		     adin1110_tx_ready(adin1110); burst++) {
			if (udp_perf_send(pcb, &host, tx_seq))
				break;

			tx_seq++;
			tx_datagrams++;
			busy = true;
		}

		now = no_os_get_time();
		elapsed_us = time_diff_us(&last_print, &now);
		if (elapsed_us >= (uint64_t)UDP_PERF_PRINT_INTERVAL_S * 1000000) {
			double elapsed_s = elapsed_us / 1000000.0;
			double tx_mbps = (tx_datagrams * UDP_PERF_PAYLOAD_LEN * 8) /
					 elapsed_s / 1000000.0;
			double rx_mbps = (rx_bytes * 8) / elapsed_s / 1000000.0;
			uint32_t cycles = adin1110_cpu_cycles();
			uint32_t idle = adin1110_idle_cycles;
			double busy_cycles = (double)(cycles - cycles_last) -
					     (double)(idle - idle_last);

			printf("UDP TX: %llu datagrams, %.2f Mbps | RX: %llu datagrams, %.2f Mbps | CPU: %.1f%% of %lu MHz\n",
			       (unsigned long long)tx_datagrams, tx_mbps,
			       (unsigned long long)rx_datagrams, rx_mbps,
			       100.0 * busy_cycles / (adin1110_cpu_hz() * elapsed_s),
			       (unsigned long)(adin1110_cpu_hz() / 1000000));

			tx_datagrams = 0;
			rx_datagrams = 0;
			rx_bytes = 0;
			cycles_last = cycles;
			idle_last = idle;
			last_print = now;
		}

		if (!busy)
			adin1110_cpu_idle(events);
	}

out:
	no_os_lwip_remove(lwip_desc);

	return ret;
}
