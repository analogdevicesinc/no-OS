/***************************************************************************//**
 *   @file   axi_sysid.h
 *   @brief  Driver for the Analog Devices AXI-SYSID core.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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
#ifndef AXI_SYSID_H_
#define AXI_SYSOD_H_

#include <stdint.h>
#include <stdio.h>

#define AXI_SYSID_VER_1_00_A		0x10061
#define AXI_SYSID_VER_1_01_A		0x10161

#define AXI_SYSID_WORD_SIZE	 	sizeof(uint32_t)
#define AXI_SYSID_REG_ROM_ADDR_WIDTH	0x40

#define AXI_SYSID_REG_VERSION		0x0000
#define AXI_SYSID_REG_PERIPH_ID		0x0004
#define AXI_SYSID_REG_SCRATCH		0x0008
#define AXI_SYSID_REG_ID		0x000c
#define AXI_SYSID_REG_SYSROM_START	0x0800
#define AXI_SYSID_REG_PRROM_START	0x1000

#ifndef AXI_PCORE_VER_MAJOR
#define AXI_PCORE_VER_MAJOR(version)    (version >> 16)
#endif
#ifndef AXI_PCORE_VER_MINOR
#define AXI_PCORE_VER_MINOR(version)    ((version >> 8) & 0xff)
#endif
#ifndef AXI_PCORE_VER_LETTER
#define AXI_PCORE_VER_LETTER(version)   (version & 0xff)
#endif

enum sysid_header_version {
	AXI_SYSID_HEADER_V1 = 1,
	AXI_SYSID_HEADER_V2,
};

struct axi_sysid {
	uint32_t 				base;
	struct axi_sysid_core_info		*info;
	uint32_t				size;
	uint32_t				*mem;
};

struct __attribute__((__packed__)) sysid_header_v1 {
	uint32_t	version;
	uint32_t	build_info_offs;
	uint32_t	board_info_offs;
	uint32_t	product_info_offs;
	uint32_t	custom_info_offs;
	uint32_t	pr_custom_info_offs;
	uint32_t	padding[9];
	uint32_t	crc;
};

struct __attribute__((__packed__)) build_info_header_v1 {
	int8_t		git_hash[44];
	int8_t		git_clean_chk[4];
	int8_t		vadj_chk[4];
	int8_t		epoch[12];
	uint8_t		padding[4];
	uint32_t	crc;
};

struct __attribute__((__packed__)) build_info_header_v1_1 {
	int8_t		git_branch[28];
	int8_t		git_hash[44];
	int8_t		git_clean_chk[4];
	int8_t		vadj_chk[4];
	int8_t		epoch[12];
	uint8_t		padding[4];
	uint32_t	crc;
};

struct axi_sysid_core_info {
	uint32_t	version;
	uint32_t	header_version;
};

struct axi_sysid_init_param {
	uint32_t base;
};

// Initialization function for AXI_SYID.
int32_t axi_sysid_init(struct axi_sysid **sysid_core,
		       const struct axi_sysid_init_param *init);
// Remove the SYSID instance.
int32_t axi_sysid_remove(struct axi_sysid *sysid);
// Get the board listed in SYSID.
char *axi_sysid_get_fpga_board(struct axi_sysid *sysid);

#endif
