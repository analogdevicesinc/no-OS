/***************************************************************************//**
 *   @file   iperf_menu.h
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
#ifndef __IPERF_MENU_H__
#define __IPERF_MENU_H__

#include <stdint.h>
#include <stdbool.h>

/** iperf role/protocol the user selected at the boot menu. */
enum iperf_menu_mode {
	/** Start both the TCP and UDP servers (default). */
	IPERF_MENU_SERVERS,
	/** Run as a UDP client toward sel.ip. */
	IPERF_MENU_UDP_CLIENT,
	/** Run as a TCP client toward sel.ip (not yet implemented). */
	IPERF_MENU_TCP_CLIENT,
};

/** Result of the boot menu. For client modes, ip/rate_bps/duration_s are the
 *  chosen (or defaulted) parameters; unused for server mode. */
struct iperf_menu_selection {
	enum iperf_menu_mode mode;
	char ip[16];        /* dotted-quad target, e.g. "192.168.97.41" */
	uint32_t rate_bps;  /* client target rate, bits/s */
	uint32_t duration_s;/* client test duration, seconds */
};

/** PLCA (PHY-level collision avoidance) settings chosen at boot. */
struct iperf_plca_selection {
	bool enabled;       /* false => plain CSMA/CD */
	uint8_t node_id;    /* 0 = coordinator (emits beacons), >0 = follower */
	uint8_t node_cnt;   /* number of nodes; only meaningful on the coordinator */
};

/**
 * @brief Prompt over the console UART for the PLCA configuration.
 *
 * Always asks whether PLCA should be enabled. When enabled, additionally
 * prompts for the node ID (0 selects the coordinator role, which emits the
 * beacon that arbitrates the segment) and the node count. Each prompt accepts a
 * default on an empty line. If no console is available, @p sel is left at the
 * provided defaults.
 *
 * @param sel - filled with the chosen PLCA settings (pre-seed with defaults).
 */
void iperf_menu_plca_select(struct iperf_plca_selection *sel);

/**
 * @brief Prompt over the console UART and return the chosen iperf mode.
 *
 * Prints a menu and blocks until the user selects a valid option (no timeout;
 * an invalid or empty line simply re-prompts). If no console is available at
 * all, returns IPERF_MENU_SERVERS. For client modes, prompts for target IP,
 * rate and duration, each accepting a default on an empty line.
 *
 * @param sel      - filled with the user's selection.
 * @param def_ip   - default target IP shown for client modes (may be NULL/"").
 * @param def_rate - default client rate in bits/s.
 * @param def_secs - default client duration in seconds.
 */
void iperf_menu_select(struct iperf_menu_selection *sel, const char *def_ip,
		       uint32_t def_rate, uint32_t def_secs);

#endif /* __IPERF_MENU_H__ */
