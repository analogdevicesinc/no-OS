/***************************************************************************//**
 *   @file   ADF7023_Config.h
 *   @brief  Configuration file of ADF7023 Driver.
 *   @author DBogdan (Dragos.Bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
 *
*******************************************************************************/
#ifndef __ADF7023_CONFIG_H__
#define __ADF7023_CONFIG_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "adf7023.h"

/******************************************************************************/
/************************* Variables Declarations *****************************/
/******************************************************************************/
struct adf7023_bbram adf7023_bbram_default = {
	/* interrupt_mask0 - 0x100 */
	BBRAM_INTERRUPT_MASK_0_INTERRUPT_TX_EOF |
	BBRAM_INTERRUPT_MASK_0_INTERRUPT_CRC_CORRECT,
	/* interrupt_mask1 - 0x101 */
	0x00,
	/* number_of_wakeups0 - 0x102 */
	0x00,
	/* number_of_wakeups1 - 0x103 */
	0x00,
	/* number_of_wakeups_irq_threshold0 - 0x104 */
	0xFF,
	/* number_of_wakeups_irq_threshold1 - 0x105 */
	0xFF,
	/* rx_dwell_time - 0x106 */
	0x00,
	/* parmtime_divider - 0x107 */
	0x33,
	/* swm_rssi_thresh - 0x108 */
	0x31,
	/* channel_freq0 - 0x109 */
	0x51, // Channel Frequency: 433 MHz
	/* channel_freq1 - 0x10A */
	0xA7, // Channel Frequency: 433 MHz
	/* channel_freq2 - 0x10B */
	0x10, // Channel Frequency: 433 MHz
	/* radio_cfg0 - 0x10C */
	BBRAM_RADIO_CFG_0_DATA_RATE_7_0(0xE8),        // Data rate: 100 kbps
	/* radio_cfg1 - 0x10D */
	BBRAM_RADIO_CFG_1_FREQ_DEVIATION_11_8(0x00) | // Frequency deviation: 25 Hz
	BBRAM_RADIO_CFG_1_DATA_RATE_11_8(0x03),       // Data rate: 100 kbps
	/* radio_cfg2 - 0x10E */
	BBRAM_RADIO_CFG_2_FREQ_DEVIATION_7_0(0xFA),  // Frequency deviation: 25 Hz
	/* radio_cfg3 - 0x10F */
	0x31,
	/* radio_cfg4 - 0x110 */
	0x16,
	/* radio_cfg5 - 0x111 */
	0x00,
	/* radio_cfg6 - 0x112 */
	BBRAM_RADIO_CFG_6_DISCRIM_PHASE(0x2),
	/* radio_cfg7 - 0x113 */
	BBRAM_RADIO_CFG_7_AGC_LOCK_MODE(3),
	/* radio_cfg8 - 0x114 */
	BBRAM_RADIO_CFG_8_PA_SINGLE_DIFF_SEL |
	BBRAM_RADIO_CFG_8_PA_LEVEL(0xF) |
	BBRAM_RADIO_CFG_8_PA_RAMP(1),
	/* radio_cfg9 - 0x115 */
	BBRAM_RADIO_CFG_9_IFBW(2),
	/* radio_cfg10 - 0x116 */
	BBRAM_RADIO_CFG_10_AFC_SCHEME(2) |
	BBRAM_RADIO_CFG_10_AFC_LOCK_MODE(3),
	/* radio_cfg11 - 0x117 */
	BBRAM_RADIO_CFG_11_AFC_KP(3) |
	BBRAM_RADIO_CFG_11_AFC_KI(7),
	/* image_reject_cal_phase - 0x118 */
	0x00,
	/* image_reject_cal_amplitude - 0x119 */
	0x00,
	/* mode_control - 0x11A */
	BBRAM_MODE_CONTROL_BB_CAL,
	/* preamble_match - 0x11B */
	0x0C,
	/* symbol_mode - 0x11C */
	BBRAM_SYMBOL_MODE_MANCHESTER_ENC,
	/* preamble_len - 0x11D */
	0x20,
	/* crc_poly0 - 0x11E */
	0x00,
	/* crc_poly1 - 0x11F */
	0x00,
	/* sync_control - 0x120 */
	BBRAM_SYNC_CONTROL_SYNC_WORD_LENGTH(8),
	/* sync_byte0 - 0x121 */
	0x00,
	/* sync_byte1 - 0x122 */
	0x00,
	/* sync_byte2 - 0x123 */
	0x12,
	/* tx_base_adr - 0x124 */
	ADF7023_TX_BASE_ADR,
	/* rx_base_adr - 0x125 */
	ADF7023_RX_BASE_ADR,
	/* packet_length_control - 0x126 */
	0x24,
	/* packet_lengt_max - 0x127 */
	0xF0,
	/* static_reg_fix - 0x128 */
	0x00,
	/* address_match_offset - 0x129 */
	0x01,
	/* address_length - 0x12A */
	0x02,
	/* address_filtering0 - 0x12B */
	0x01,
	/* address_filtering1 - 0x12C */
	0xFF,
	/* address_filtering2 - 0x12D */
	0xFF,
	/* address_filtering3 - 0x12E */
	0xFF,
	/* address_filtering4 - 0x12F */
	0x02,
	/* address_filtering5 - 0x130 */
	0x0F,
	/* address_filtering6 - 0x131 */
	0xFF,
	/* address_filtering7 - 0x132 */
	0x0F,
	/* address_filtering8 - 0x133 */
	0xFF,
	/* address_filtering9 - 0x134 */
	0x00,
	/* address_filtering10 - 0x135 */
	0x00,
	/* address_filtering11 - 0x136 */
	0x00,
	/* address_filtering12 - 0x137 */
	0x00,
	/* rssi_wait_time - 0x138 */
	0x00,
	/* testmodes - 0x139 */
	0x00,
	/* transition_clock_div - 0x13A */
	0x00,
	/* reserved0 - 0x13B */
	0x00,
	/* reserved1 - 0x13C */
	0x00,
	/* reserved2 - 0x13D */
	0x00,
	/* rx_synth_lock_time - 0x13E */
	0x00,
	/* tx_synth_lock_time - 0x13F */
	0x00,
};

#endif // __ADF7023_CONFIG_H__
