/*******************************************************************************
 *   @file   axi_sysid.c
 *   @brief  Driver for the Analog Devices AXI-SYSID core.
 *   @author GMois (george.mois@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "no_os_print_log.h"
#include "no_os_axi_io.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "axi_sysid.h"

static inline uint32_t axi_sysid_ioread(const struct axi_sysid *st,
					const uint32_t reg)
{
	uint32_t data;

	no_os_axi_io_read(st->base, reg, &data);

	return data;
}

static int axi_sysid_read(void *context, unsigned int offset,
			  void *val, size_t bytes)
{
	struct axi_sysid *st = context;
	unsigned int count = bytes >> 2;
	uint32_t index = offset >> 2;
	uint32_t *buf = val;
	int i;
	/*
	 * Reading the ROM memory requires an address of length (address_bits + 2)
	 * that has the two most significat bits set to "01".
	 * Because the size of the memory is calculated as (1 << address_bits) * 4
	 * we can use the size as an offset to generate correct addresses.
	 */
	uint32_t rom_offset = st->size;

	if (count > (st->size - index))
		count = st->size - index;

	for (i = index; i < (index + count); i++)
		*buf++ = axi_sysid_ioread(st, rom_offset + i *
					  AXI_SYSID_WORD_SIZE);

	return 0;
}

static uint8_t axi_sysid_checksum(uint8_t *ptr, size_t s)
{
	uint8_t sum = 0;

	while (s-- != 0)
		sum -= *ptr++;

	return sum;
}

static char *axi_sysid_get_str(struct axi_sysid *st, int this)
{
	if (this > 0) {
		char *str = (char *) &st->mem[this];

		if (*str)
			return str;
	}

	return NULL;
}

static int axi_sysid_validate_v1_1(struct axi_sysid *st,
				   struct build_info_header_v1_1 *build)
{
	struct sysid_header_v1 *header;
	char custom_info[516];

	if (axi_sysid_checksum((uint8_t *)build,
			       sizeof(struct build_info_header_v1_1))) {
		pr_err("verify build header checksum failed\n");
		return -EFAULT;
	}

	header = (struct sysid_header_v1 *) st->mem;

	if (axi_sysid_get_str(st, header->custom_info_offs))
		snprintf(custom_info, sizeof(custom_info), "%s%s%s",
			 " [", axi_sysid_get_str(st, header->custom_info_offs), "]");
	else
		custom_info[0] = 0;

	pr_info("[%s]%s on [%s] git branch <%.28s> git <%.40s> %s\n",
		axi_sysid_get_str(st, header->board_info_offs),
		custom_info,
		axi_sysid_get_str(st, header->product_info_offs),
		build->git_branch,
		build->git_hash,
		(build->git_clean_chk[0] == 't') ? "clean" : "dirty");

	return 0;
}

static int axi_sysid_validate_v1(struct axi_sysid *st,
				 struct build_info_header_v1 *build)
{
	struct sysid_header_v1 *header;
	char custom_info[516];

	header = (struct sysid_header_v1 *) st->mem;
	if (axi_sysid_checksum((uint8_t *)build,
			       sizeof(struct build_info_header_v1))) {
		pr_err("verify build header checksum failed\n");
		return -EFAULT;
	}

	if (axi_sysid_get_str(st, header->custom_info_offs))
		snprintf(custom_info, sizeof(custom_info), "%s%s%s",
			 " [", axi_sysid_get_str(st, header->custom_info_offs), "]");
	else
		custom_info[0] = 0;

	pr_info("[%s]%s on [%s] git <%.40s> %s UTC\n",
		axi_sysid_get_str(st, header->board_info_offs),
		custom_info,
		axi_sysid_get_str(st, header->product_info_offs),
		build->git_hash,
		(build->git_clean_chk[0] == 't') ? "clean" : "dirty");

	return 0;
}

static int axi_sysid_validate(struct axi_sysid *st)
{
	struct build_info_header_v1_1 *build_v1_1;
	struct build_info_header_v1 *build_v1;
	struct sysid_header_v1 *header;
	uint32_t *build_raw;

	st->mem = no_os_calloc(st->size, sizeof(uint32_t));
	if (!st->mem)
		return -ENOMEM;

	axi_sysid_read(st, 0, st->mem, st->size);
	header = (struct sysid_header_v1 *)st->mem;

	if (axi_sysid_checksum((uint8_t *)st->mem, sizeof(struct sysid_header_v1))) {
		pr_err("verify header checksum failed\n");
		return -EFAULT;
	}

	if (header->version != st->info->header_version) {
		pr_err("system ID header version mismatch. Expected %d, Reported %d\n",
		       st->info->header_version, header->version);
		return -EFAULT;
	}

	build_raw = &st->mem[header->build_info_offs];

	switch (header->version) {
	case AXI_SYSID_HEADER_V2:
		build_v1_1 = (struct build_info_header_v1_1 *) build_raw;

		return axi_sysid_validate_v1_1(st, build_v1_1);
	case AXI_SYSID_HEADER_V1:
		build_v1 = (struct build_info_header_v1 *) build_raw;

		return axi_sysid_validate_v1(st, build_v1);
	default:
		return -EINVAL;
	}
}

/*******************************************************************************
* @brief Initialization function for AXI_SYID.
*
* @param sysid_core - SYSID istance.
* @param init - Structure containing initializing data.
*
* @return 0 for success, -1 in case of failure.
*******************************************************************************/
int32_t axi_sysid_init(struct axi_sysid **sysid_core,
		       const struct axi_sysid_init_param *init)
{
	struct axi_sysid_core_info *info;
	struct axi_sysid *sysid;
	int ret;

	sysid = (struct axi_sysid *)no_os_calloc(1, sizeof(*sysid));
	if (!sysid)
		return -ENOMEM;

	sysid->base = init->base;
	sysid->size = (1 << axi_sysid_ioread(sysid, AXI_SYSID_REG_ROM_ADDR_WIDTH)) *
		      AXI_SYSID_WORD_SIZE;

	info = (struct axi_sysid_core_info *)no_os_calloc(1, sizeof(*info));
	if (!info) {
		ret = -ENOMEM;
		goto error;
	}

	info->version = axi_sysid_ioread(sysid, AXI_SYSID_REG_VERSION);

	sysid->info = info;

	switch(sysid->info->version) {
	case AXI_SYSID_VER_1_00_A:
		sysid->info->header_version = AXI_SYSID_HEADER_V1;
		break;
	case AXI_SYSID_VER_1_01_A:
		sysid->info->header_version = AXI_SYSID_HEADER_V2;
		break;
	default:
		ret = -EINVAL;
		goto error1;
	}

	ret = axi_sysid_validate(sysid);
	if (ret)
		goto error1;

	*sysid_core = sysid;

	return 0;
error1:
	no_os_free(info);
error:
	no_os_free(sysid);
	return ret;
}

/*******************************************************************************
* @brief Remove the SYSID instance.
*
* @param sysid - SYSID istance.
*
* @return 0 for success, -1 in case of failure.
*******************************************************************************/
int32_t axi_sysid_remove(struct axi_sysid *sysid)
{
	if (!sysid)
		return -1;

	no_os_free(sysid);

	return 0;
}

/*******************************************************************************
* @brief Get the FPGA board listed in SYSID.
*
* @param sysid - SYSID istance.
*
* @return board name.
*******************************************************************************/
char *axi_sysid_get_fpga_board(struct axi_sysid *sysid)
{
	struct sysid_header_v1 *header;

	header = (struct sysid_header_v1 *) sysid->mem;

	return axi_sysid_get_str(sysid, header->product_info_offs);
}
