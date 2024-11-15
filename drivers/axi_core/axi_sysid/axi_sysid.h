/***************************************************************************//**
 *   @file   axi_sysid.h
 *   @brief  Driver for the Analog Devices AXI-SYSID core.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef AXI_SYSID_H_
#define AXI_SYSOD_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdio.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
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

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
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

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
// Initialization function for AXI_SYID.
int32_t axi_sysid_init(struct axi_sysid **sysid_core,
		       const struct axi_sysid_init_param *init);
// Remove the SYSID instance.
int32_t axi_sysid_remove(struct axi_sysid *sysid);
// Get the board listed in SYSID.
char *axi_sysid_get_fpga_board(struct axi_sysid *sysid);

#endif
