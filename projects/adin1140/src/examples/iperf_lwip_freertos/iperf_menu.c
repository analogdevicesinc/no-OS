/***************************************************************************//**
 *   @file   iperf_menu.c
 *   @brief  Boot-time UART menu to pick the iperf role/protocol at runtime.
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

#include <string.h>
#include <stdlib.h>

#include "iperf_menu.h"
#include "common_data.h"
#include "no_os_uart.h"
#include "no_os_print_log.h"

/**
 * @brief Read one line (terminated by CR or LF) into buf.
 *
 * Characters are echoed so the console is usable over a plain terminal.
 * Reads block per-character up to the UART timeout; @p max_idle_reads bounds how
 * many consecutive idle (timed-out) reads to tolerate before giving up on an
 * empty line - use it to implement the boot countdown. Passing 0 disables the
 * idle limit (wait indefinitely for the line to complete once typing starts).
 *
 * @return number of characters stored (>=0), or -1 if it gave up while the line
 *         was still empty (idle limit hit with nothing typed).
 */
static int iperf_menu_readline(char *buf, uint32_t buf_len,
			       uint32_t max_idle_reads)
{
	uint32_t len = 0;
	uint32_t idle = 0;
	uint8_t ch;
	int32_t ret;

	if (buf_len == 0)
		return -1;

	while (1) {
		ret = no_os_uart_read(adin1140_uart_desc, &ch, 1);
		if (ret != 1) {
			/* timeout / no data this slice */
			if (len == 0 && max_idle_reads != 0) {
				if (++idle >= max_idle_reads)
					return -1;
			}
			continue;
		}

		idle = 0;

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
			/* echo the typed character */
			no_os_uart_write(adin1140_uart_desc, &ch, 1);
		}
	}
}

/** Prompt for a string, keeping @p def when the user just presses Enter. */
static void iperf_menu_prompt_str(const char *label, const char *def,
				  char *out, uint32_t out_len)
{
	char line[32];
	int n;

	pr_info("%s [%s]: ", label, (def && def[0]) ? def : "");
	n = iperf_menu_readline(line, sizeof(line), 0);
	if (n > 0)
		strncpy(out, line, out_len - 1);
	else if (def)
		strncpy(out, def, out_len - 1);
	else
		out[0] = '\0';
	out[out_len - 1] = '\0';
}

/** Prompt for an unsigned integer, keeping @p def on an empty line. */
static uint32_t iperf_menu_prompt_u32(const char *label, uint32_t def)
{
	char line[16];
	int n;

	pr_info("%s [%lu]: ", label, (unsigned long)def);
	n = iperf_menu_readline(line, sizeof(line), 0);
	if (n > 0)
		return (uint32_t)strtoul(line, NULL, 0);
	return def;
}

/**
 * @brief Prompt for a rate in Mbit/s, returning the value in bit/s.
 *
 * The default and the entered value are both expressed in Mbit/s (decimals
 * allowed, e.g. "2.5"); the result is converted to bit/s for lwiperf. An empty
 * line keeps @p def_bps.
 */
static uint32_t iperf_menu_prompt_mbps(const char *label, uint32_t def_bps)
{
	char line[16];
	int n;
	uint32_t def_int = def_bps / 1000000;
	uint32_t def_frac = (def_bps % 1000000) / 1000; /* 3 fractional digits */

	pr_info("%s [%lu.%03lu]: ", label, (unsigned long)def_int,
		(unsigned long)def_frac);
	n = iperf_menu_readline(line, sizeof(line), 0);
	if (n <= 0)
		return def_bps;
	return (uint32_t)(strtod(line, NULL) * 1000000.0);
}

/** Prompt a yes/no question, returning @p def on an empty line. */
static bool iperf_menu_prompt_yesno(const char *label, bool def)
{
	char line[8];
	int n;

	pr_info("%s [%c/%c]: ", label, def ? 'Y' : 'y', def ? 'n' : 'N');
	n = iperf_menu_readline(line, sizeof(line), 0);
	if (n <= 0)
		return def;
	return (line[0] == 'y' || line[0] == 'Y');
}

void iperf_menu_plca_select(struct iperf_plca_selection *sel)
{
	if (!adin1140_uart_desc) {
		/* no console: keep the provided defaults */
		return;
	}

	pr_info("\r\n=== PLCA config ===\r\n");
	sel->enabled = iperf_menu_prompt_yesno("Enable PLCA?", sel->enabled);
	if (sel->enabled) {
		sel->node_id = (uint8_t)iperf_menu_prompt_u32(
				       "Node ID (0=coordinator)", sel->node_id);
		sel->node_cnt = (uint8_t)iperf_menu_prompt_u32(
					"Node count", sel->node_cnt);
	}
}

void iperf_menu_select(struct iperf_menu_selection *sel, const char *def_ip,
		       uint32_t def_rate, uint32_t def_secs)
{
	char line[16];
	int n;

	/* sane defaults */
	sel->mode = IPERF_MENU_SERVERS;
	sel->ip[0] = '\0';
	if (def_ip)
		strncpy(sel->ip, def_ip, sizeof(sel->ip) - 1);
	sel->ip[sizeof(sel->ip) - 1] = '\0';
	sel->rate_bps = def_rate;
	sel->duration_s = def_secs;

	if (!adin1140_uart_desc) {
		/* no console available: keep server default */
		return;
	}

	pr_info("\r\n=== iperf mode ===\r\n");
	pr_info("  1) TCP + UDP servers\r\n");
	pr_info("  2) UDP client\r\n");
	pr_info("  3) TCP client\r\n");

	/* Block until the user makes a choice - no timeout/default. An empty line
	 * just re-prompts. */
	while (1) {
		pr_info("Select [1-3]: ");
		n = iperf_menu_readline(line, sizeof(line), 0);
		if (n <= 0)
			continue;
		if (line[0] == '1') {
			sel->mode = IPERF_MENU_SERVERS;
			return;
		}
		if (line[0] == '2') {
			sel->mode = IPERF_MENU_UDP_CLIENT;
			break;
		}
		if (line[0] == '3') {
			sel->mode = IPERF_MENU_TCP_CLIENT;
			break;
		}
		pr_info("Invalid choice.\r\n");
	}

	/* client parameters. TCP is window/congestion controlled, so it has no
	 * target rate - only the UDP client prompts for one. */
	iperf_menu_prompt_str("Target IP", sel->ip, sel->ip, sizeof(sel->ip));
	if (sel->mode == IPERF_MENU_UDP_CLIENT)
		sel->rate_bps = iperf_menu_prompt_mbps("Rate Mbps",
						       sel->rate_bps);
	sel->duration_s = iperf_menu_prompt_u32("Duration s", sel->duration_s);
}
