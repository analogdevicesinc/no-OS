/***************************************************************************//**
 *   @file   frame_blast_example.c
 *   @brief  Raw L2 (Ethernet) throughput test for the ADIN1140 project.
 *
 *   Measures the true MAC-layer throughput of the 10BASE-T1S link by sending or
 *   receiving raw Ethernet frames back-to-back, with no IP/TCP/UDP overhead.
 *   Pairs with the host-side l2_send/l2_recv tools (see projects/adin1140/host).
 *   @author Raluca Bozdog
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
#include <stdlib.h>

#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"

#include "adin1140.h"

/* Experimental/local EtherType (IEEE 802 "local experimental 1"), so the host
 * can filter our test frames cleanly and no real protocol is disturbed. */
#define FRAME_BLAST_ETHERTYPE_HI  0x88
#define FRAME_BLAST_ETHERTYPE_LO  0xB5

/* Max standard payload: 1514-byte frame (14-byte header + 1500 payload). */
#define FRAME_BLAST_PAYLOAD_LEN   1500
#define FRAME_BLAST_FRAME_LEN     (FRAME_BLAST_PAYLOAD_LEN + ADIN1140_ETH_HDR_LEN)

/* Per-frame wire overhead beyond the L2 frame we count: 7-byte preamble +
 * 1-byte SFD + 12-byte inter-frame gap = 20 bytes. Reported separately so the
 * user can compare "L2 payload+header" against "on-the-wire" throughput. */
#define FRAME_BLAST_WIRE_OVERHEAD 20

#define FRAME_BLAST_RX_BUF_LEN    1530

static uint8_t tx_payload[FRAME_BLAST_PAYLOAD_LEN];
static uint8_t rx_buff[FRAME_BLAST_RX_BUF_LEN];

/* Board and peer L2 addresses. Broadcast dest so the host receives without the
 * board needing to know the host MAC; the host filters on the EtherType. */
static uint8_t mac_addr[ADIN1140_ETH_ALEN] = {0xCA, 0x2F, 0xB7, 0x10, 0x23, 0x63};
static uint8_t mac_bcast[ADIN1140_ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/** Milliseconds since power-up (no_os_get_time is ms-resolution here). */
static uint32_t now_ms(void)
{
	struct no_os_time t = no_os_get_time();

	return t.s * 1000U + t.us / 1000U;
}

/** Read a line from the console UART (echoed). Returns length, or -1 if no
 *  console. Blocks until CR/LF. */
static int blast_readline(char *buf, uint32_t buf_len)
{
	uint32_t len = 0;
	uint8_t ch;
	int32_t ret;

	if (!adin1140_uart_desc || buf_len == 0)
		return -1;

	while (1) {
		ret = no_os_uart_read(adin1140_uart_desc, &ch, 1);
		if (ret != 1)
			continue; /* timeout slice; keep waiting */
		if (ch == '\r' || ch == '\n') {
			buf[len] = '\0';
			pr_info("\r\n");
			return (int)len;
		}
		if ((ch == '\b' || ch == 0x7F) && len > 0) {
			len--;
			pr_info("\b \b");
			continue;
		}
		if (ch >= ' ' && len < buf_len - 1) {
			buf[len++] = (char)ch;
			no_os_uart_write(adin1140_uart_desc, &ch, 1);
		}
	}
}

static uint32_t blast_prompt_u32(const char *label, uint32_t def)
{
	char line[16];
	int n;

	pr_info("%s [%lu]: ", label, (unsigned long)def);
	n = blast_readline(line, sizeof(line));
	if (n > 0)
		return (uint32_t)strtoul(line, NULL, 0);
	return def;
}

/** Prompt for a target rate in Mbit/s (accepts a decimal such as 2.5) and
 *  return it in kbit/s, which is the unit the pacing/reporting code uses.
 *  Empty line -> def_kbps. 0 means "full line rate". */
static uint32_t blast_prompt_rate_kbps(uint32_t def_kbps)
{
	char line[16];
	uint32_t whole = 0, frac = 0, scale = 100; /* up to 3 fractional digits */
	const char *p;
	int n;

	pr_info("Target rate Mbit/s (0=max) [%lu.%03lu]: ",
		(unsigned long)(def_kbps / 1000),
		(unsigned long)(def_kbps % 1000));
	n = blast_readline(line, sizeof(line));
	if (n <= 0)
		return def_kbps;

	p = line;
	while (*p >= '0' && *p <= '9')
		whole = whole * 10U + (uint32_t)(*p++ - '0');
	if (*p == '.' || *p == ',') {
		p++;
		while (*p >= '0' && *p <= '9' && scale) {
			frac += (uint32_t)(*p++ - '0') * scale;
			scale /= 10U;
		}
	}
	return whole * 1000U + frac;
}

/** Blast raw frames for duration_s seconds at up to rate_kbps (0 = unlimited /
 *  full line rate); report the achieved L2 throughput. */
static int frame_blast_tx(struct adin1140_desc *adin1140, uint32_t duration_s,
			  uint32_t rate_kbps, uint32_t frame_len)
{
	struct adin1140_eth_buff eth_tx = {
		.ethertype = {FRAME_BLAST_ETHERTYPE_HI, FRAME_BLAST_ETHERTYPE_LO},
		.len = frame_len,
		.payload = tx_payload,
	};
	uint64_t frames = 0;
	uint64_t l2_bytes = 0;
	uint32_t start, elapsed, dur_ms = duration_s * 1000U;
	uint32_t kbps_l2, kbps_wire;
	struct adin1140_stats stats0, stats1;
	uint32_t hw_frames, hw_kbps_l2, hw_kbps_wire;
	uint64_t hw_l2_bytes;
	int ret;

	memcpy(eth_tx.mac_source, mac_addr, ADIN1140_ETH_ALEN);
	memcpy(eth_tx.mac_dest, mac_bcast, ADIN1140_ETH_ALEN);

	if (rate_kbps)
		pr_info("TX %lu-byte frames for %lu s at ~%lu.%03lu Mbit/s...\n",
			(unsigned long)frame_len,
			(unsigned long)duration_s,
			(unsigned long)(rate_kbps / 1000),
			(unsigned long)(rate_kbps % 1000));
	else
		pr_info("TX blasting %lu-byte frames for %lu s (max rate)...\n",
			(unsigned long)frame_len,
			(unsigned long)duration_s);

	/* Drain any frame left queued by a previous run before timing starts. With
	 * no lwIP step loop running between runs, nothing pumps the OA TC6 transfer
	 * while the menu is idle, so stale TX_READY buffers would otherwise make the
	 * next run's get_tx_frame() return -ENOBUFS forever. */
	adin1140_poll(adin1140);

	/* Baseline the PHY's hardware TX frame counter (reg 0xB1). The software
	 * count below tracks frames handed to the MAC over SPI (into the internal
	 * TX FIFO); the hardware counter increments only when a frame is actually
	 * transmitted on the wire. Comparing the two shows how far the FIFO-fill
	 * rate runs ahead of the true line rate. Ignore a stats read error - the
	 * hardware figure is just skipped if unavailable. */
	if (adin1140_get_stats(adin1140, &stats0))
		stats0.tx_frames = 0;

	start = now_ms();
	while ((elapsed = now_ms() - start) < dur_ms) {
		/* Rate limiting: if a target rate is set, only send when we are behind
		 * the schedule that rate implies. target_bytes = rate * elapsed. The L2
		 * frame size (header+payload) is what we pace on, matching the reported
		 * L2 throughput. rate_kbps == 0 disables the cap (full line rate). */
		if (rate_kbps) {
			uint64_t target_bytes =
				((uint64_t)rate_kbps * elapsed) / 8U;
			if (l2_bytes >= target_bytes) {
				/* ahead of schedule: keep the link idle for now */
				continue;
			}
		}

		/* write_fifo submits the frame then pumps it out over SPI (blocking),
		 * so at max rate the loop is naturally paced to the link rate. */
		ret = adin1140_write_fifo(adin1140, &eth_tx);
		if (ret == -ENOBUFS || ret == -EAGAIN) {
			/* TX buffers full: write_fifo returned WITHOUT pumping (it only
			 * pumps after a successful submit), so pump here to drain them,
			 * then retry. Without this the buffers never free and the whole
			 * run sends nothing. */
			adin1140_poll(adin1140);
			continue;
		}
		if (ret)
			return ret;
		frames++;
		l2_bytes += frame_len;
	}

	/* Flush the last submitted frame(s) so they reach the wire and the host
	 * count matches. */
	adin1140_poll(adin1140);

	/* Snapshot the hardware TX counter again for the wire-side (delivered)
	 * figure. */
	if (adin1140_get_stats(adin1140, &stats1))
		stats1.tx_frames = stats0.tx_frames;

	if (elapsed == 0)
		elapsed = 1;
	/* kbit/s = bytes*8 / ms. Do it in 64-bit to avoid overflow. */
	kbps_l2 = (uint32_t)((l2_bytes * 8ULL) / elapsed);
	kbps_wire = (uint32_t)(((l2_bytes +
		(uint64_t)frames * FRAME_BLAST_WIRE_OVERHEAD) * 8ULL) / elapsed);

	/* Hardware-counted frames actually transmitted on the wire (handles the
	 * 32-bit counter wrapping via unsigned subtraction). */
	hw_frames = stats1.tx_frames - stats0.tx_frames;
	hw_l2_bytes = (uint64_t)hw_frames * frame_len;
	hw_kbps_l2 = (uint32_t)((hw_l2_bytes * 8ULL) / elapsed);
	hw_kbps_wire = (uint32_t)(((hw_l2_bytes +
		(uint64_t)hw_frames * FRAME_BLAST_WIRE_OVERHEAD) * 8ULL) / elapsed);

	pr_info("TX done in %lu ms\n", (unsigned long)elapsed);
	pr_info("  FIFO (submitted to MAC):\n");
	pr_info("    %lu frames, %lu L2 bytes\n",
		(unsigned long)frames, (unsigned long)l2_bytes);
	pr_info("    L2   : %lu.%03lu Mbit/s\n",
		(unsigned long)(kbps_l2 / 1000), (unsigned long)(kbps_l2 % 1000));
	pr_info("    wire+: %lu.%03lu Mbit/s (incl preamble+IFG)\n",
		(unsigned long)(kbps_wire / 1000),
		(unsigned long)(kbps_wire % 1000));
	pr_info("  WIRE (HW TX counter, actually transmitted):\n");
	pr_info("    %lu frames, %lu L2 bytes\n",
		(unsigned long)hw_frames, (unsigned long)hw_l2_bytes);
	pr_info("    L2   : %lu.%03lu Mbit/s\n",
		(unsigned long)(hw_kbps_l2 / 1000),
		(unsigned long)(hw_kbps_l2 % 1000));
	pr_info("    wire+: %lu.%03lu Mbit/s (incl preamble+IFG)\n",
		(unsigned long)(hw_kbps_wire / 1000),
		(unsigned long)(hw_kbps_wire % 1000));
	return 0;
}

/** Count received raw frames for duration_s seconds; report L2 throughput. */
static int frame_blast_rx(struct adin1140_desc *adin1140, uint32_t duration_s)
{
	struct adin1140_eth_buff eth_rx = { .payload = rx_buff };
	uint64_t frames = 0;
	uint64_t l2_bytes = 0;
	uint32_t start, elapsed, dur_ms = duration_s * 1000U;
	uint32_t first_ms = 0;
	uint8_t started = 0;
	uint32_t kbps_l2;
	int ret;

	pr_info("RX counting for %lu s (waiting for frames)...\n",
		(unsigned long)duration_s);

	start = now_ms();
	while ((elapsed = now_ms() - start) < dur_ms) {
		/* pump the OA TC6 transfer so received frames land in the RX buffers */
		ret = adin1140_poll(adin1140);
		if (ret)
			return ret;

		while (1) {
			eth_rx.len = 0;
			ret = adin1140_read_fifo(adin1140, &eth_rx);
			if (ret == -ENOENT || !eth_rx.len)
				break;
			if (ret)
				return ret;
			if (!started) {
				started = 1;
				first_ms = now_ms();
			}
			frames++;
			l2_bytes += eth_rx.len;
		}
	}

	/* Measure the rate over the active window (first frame -> end), so idle
	 * time before the host starts sending does not dilute the result. */
	if (started) {
		uint32_t active = now_ms() - first_ms;
		if (active == 0)
			active = 1;
		kbps_l2 = (uint32_t)((l2_bytes * 8ULL) / active);
		pr_info("RX done: %lu frames, %lu L2 bytes in %lu ms (active)\n",
			(unsigned long)frames, (unsigned long)l2_bytes,
			(unsigned long)active);
		pr_info("  L2 throughput   : %lu.%03lu Mbit/s\n",
			(unsigned long)(kbps_l2 / 1000),
			(unsigned long)(kbps_l2 % 1000));
	} else {
		pr_info("RX done: no frames received\n");
	}
	return 0;
}

int example_main()
{
	struct adin1140_desc *adin1140;
	struct adin1140_init_param adin1140_ip = {
		.comm_param = adin1140_spi_ip,
		.mac_cfg = {
			.cps   = 0x6,
			.zarfe = true,
		},
		.plca_cfg = {
			.enabled   = true,
			.node_id   = 0, /* coordinator by default for a point-to-point link */
			.node_cnt  = 2,
			.to_tmr    = 0x20,
		},
	};
	uint32_t duration_s;
	uint32_t rate_kbps;
	uint32_t frame_len;
	char line[8];
	int n, ret;

	memcpy(adin1140_ip.mac_address, mac_addr, ADIN1140_ETH_ALEN);
	memset(tx_payload, 0xA5, sizeof(tx_payload));

	pr_info("ADIN1140 Raw L2 Throughput Test\n");

	ret = adin1140_init(&adin1140, &adin1140_ip);
	if (ret) {
		pr_info("adin1140_init failed: %d\n", ret);
		return ret;
	}

	ret = adin1140_set_promisc(adin1140, true);
	if (ret)
		goto cleanup;
	ret = adin1140_broadcast_filter(adin1140, true);
	if (ret)
		goto cleanup;

	while (1) {
		pr_info("\r\n=== raw L2 test ===\r\n");
		pr_info("  1) TX blast (board -> host)\r\n");
		pr_info("  2) RX count (host -> board)\r\n");
		pr_info("Select: ");
		n = blast_readline(line, sizeof(line));
		if (n <= 0)
			continue;

		duration_s = blast_prompt_u32("Duration s", 10);
		if (duration_s == 0)
			duration_s = 10;

		if (line[0] == '1') {
			/* L2 frame size in bytes (header+payload). Clamp to a valid
			 * Ethernet range: min 64 (the driver zero-pads shorter frames
			 * anyway) up to the 1514-byte buffer ceiling. */
			frame_len = blast_prompt_u32("Frame size bytes (64-1514)",
						     FRAME_BLAST_FRAME_LEN);
			if (frame_len < 64)
				frame_len = 64;
			else if (frame_len > FRAME_BLAST_FRAME_LEN)
				frame_len = FRAME_BLAST_FRAME_LEN;

			/* Target L2 rate in Mbit/s; 0 = full line rate. Lets you sweep
			 * requested rates and compare against what the RX side records. */
			rate_kbps = blast_prompt_rate_kbps(0);
			ret = frame_blast_tx(adin1140, duration_s, rate_kbps,
					     frame_len);
		} else if (line[0] == '2') {
			ret = frame_blast_rx(adin1140, duration_s);
		} else {
			continue;
		}

		if (ret) {
			pr_info("test error: %d\n", ret);
			goto cleanup;
		}
	}

cleanup:
	pr_info("Error: %d\n", ret);
	adin1140_remove(adin1140);
	return ret;
}
