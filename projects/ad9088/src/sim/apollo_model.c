/***************************************************************************//**
 *   @file   apollo_model.c
 *   @brief  Software model of the AD9084 (Apollo) SPI slave, for the host
 *           simulation build (-DAD9088_SIM=ON).
 *
 *           The device is a sparse 32-bit byte-addressed memory.  It is
 *           reached three ways, as the vendor HAL does:
 *             - direct:   SPI offset 0..0x3FFF -> 0x47000000 + offset
 *             - paged:    SPI offset 0..0x3FFF -> page base + offset
 *                         (page base in regs 0x126..0x129, MSB first)
 *             - indirect: the SPIDMA engine (CTL, ADDR3..0, DATA3..0);
 *                         DATA0 (the LSB) is always the last byte the HAL
 *                         touches, so accessing it completes the transfer.
 *           A frame is two address bytes (bit 7 read, bit 6 paged) and data
 *           bytes at incrementing addresses; in SPI FIFO mode a frame aimed
 *           at DATA0 stays on DATA0.
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "apollo_model.h"

#define APOLLO_DIRECT_BASE	0x47000000u
#define APOLLO_DIRECT_LEN	0x4000u

#define APOLLO_FIFO_MODE	0x010u
#define APOLLO_SPIDMA_CTL	0x04Fu
#define APOLLO_SPIDMA_ADDR3	0x050u
#define APOLLO_SPIDMA_ADDR0	0x053u
#define APOLLO_SPIDMA_DATA3	0x054u
#define APOLLO_SPIDMA_DATA0	0x057u
#define APOLLO_PAGE_BASE	0x126u	/* 0x126..0x129, bits 31..0 */
#define APOLLO_ARM1_RUN		0x0C6u	/* bit 0: release Core1 */
#define APOLLO_CORE1_STATUS	0x201u	/* RAM boot step (scratch 1) */
#define APOLLO_XFER_DONE	0x558u	/* bit 0: profile written */
#define APOLLO_TE_BOOT_READY	0x56Fu	/* host done loading the images */

#define APOLLO_MBOX_CMD		0x0F4u	/* bits 5:0 opcode, bit 7 busy */
#define MBOX_BUF		0x21000000u	/* command in, response out */
#define MBOX_RESP_LEN		1024u	/* largest response struct */
#define MBOX_OP_PING		0u
#define MBOX_OP_INIT_COMPLETION	2u
#define MBOX_OP_GET_CAL_STATUS	8u
#define MBOX_OP_GET_FW_VERSION	0x29u

/*
 * get_cal_status: request object id at +1, reply data at +4, length at
 * +1020.  The MCS init cal reply is adi_apollo_mcs_cal_init_status_t, the
 * MCS tracking cal reply adi_apollo_mcs_cal_status_t.
 */
#define MBOX_DATA		4u
#define MBOX_DATA_LEN		1020u
#define CAL_OBJID_IC_MCS	4u
#define MCS_INIT_LEN		128u
#define MCS_INIT_LOCKED_C	24u	/* is_C/A/B_Locked follow */
#define CAL_OBJID_TC_MCS	50u
#define MCS_TRACK_LEN		72u
#define MCS_TRACK_FG_DONE	24u
#define MCS_TRACK_HALT_ACTIVE	29u

/* TinyE secure-boot stage results: 0xFF = encrypted images verified. */
#define TE_STAGE0		0x4C006342u
#define TE_STAGE1		0x4C006343u
#define TE_STAGE_ENCR_OK	0xFFu

/*
 * Pointers Core1's firmware publishes once it runs. Both locations sit
 * inside the TinyE image loaded at 0x21000000, so until then they read as
 * image bytes. The firmware's own target addresses are not visible from
 * the host; the model uses free regions clear of the loaded images
 * (TinyE ends below 0x21006000) and of the mailbox buffer.
 */
#define FW_PROFILE_PTR		0x21000414u	/* -> device profile */
#define FW_BOOT_ERROR_PTR	0x21000424u	/* -> last_cmd, system, track_cal */
#define FW_BOOT_ERROR_BLOCK	0x21008000u
#define FW_PROFILE_AREA		0x21010000u

/* RAM boot steps reported by the Core1 firmware. */
#define BOOT_WAIT_FOR_CONFIG	0x1Du
#define BOOT_APP_RUNNING	0x34u

#define SPIDMA_CTL_RD		0x80u
#define SPIDMA_CTL_SIZE(c)	(((c) >> 2) & 0x3u)
#define SPIDMA_CTL_AUTO_INC	0x02u

/******************************************************************************/
/* Sparse memory                                                              */
/******************************************************************************/

#define APOLLO_PAGE_SHIFT	12
#define APOLLO_PAGE_SIZE	(1u << APOLLO_PAGE_SHIFT)
#define APOLLO_HASH_SIZE	1024u

struct apollo_page {
	uint32_t base;
	struct apollo_page *next;
	uint8_t data[APOLLO_PAGE_SIZE];
};

static struct apollo_page *apollo_pages[APOLLO_HASH_SIZE];

static uint8_t *apollo_byte(uint32_t addr, bool create)
{
	uint32_t base = addr >> APOLLO_PAGE_SHIFT;
	struct apollo_page **slot = &apollo_pages[base % APOLLO_HASH_SIZE];
	struct apollo_page *p;

	for (p = *slot; p; p = p->next)
		if (p->base == base)
			return &p->data[addr & (APOLLO_PAGE_SIZE - 1)];

	if (!create)
		return NULL;

	p = calloc(1, sizeof(*p));
	if (!p) {
		fprintf(stderr, "SIM: apollo out of memory\n");
		exit(1);
	}
	p->base = base;
	p->next = *slot;
	*slot = p;

	return &p->data[addr & (APOLLO_PAGE_SIZE - 1)];
}

/*
 * Status the JESD204 blocks drive for a healthy link: always read back
 * this value.  Per side (A at 0x606../0x616.., B at 0x60E../0x61E..).
 */
struct apollo_ro {
	uint32_t addr;
	uint8_t count;	/* consecutive bytes with the same value */
	uint8_t val;
};

static const struct apollo_ro apollo_ro_regs[] = {
	/* JRx core: user data ready, SYSREF received, config valid */
	{ 0x6160415Cu, 2, 0x06 }, { 0x61E0415Cu, 2, 0x06 },
	/* JRx 204B lanes: frame sync, CGS, data ready, checksum good */
	{ 0x6160C500u, 8, 0x38 }, { 0x61E0C500u, 8, 0x38 },
	/* JTx: SYNC~ deasserted, PLL locked, phase established */
	{ 0x606100C3u, 1, 0x20 }, { 0x60E100C3u, 1, 0x20 },
	{ 0x60618001u, 1, 0x80 }, { 0x60E18001u, 1, 0x80 },
	{ 0x60618013u, 1, 0x01 }, { 0x60E18013u, 1, 0x01 },
};

static bool apollo_ro_get(uint32_t addr, uint8_t *val)
{
	uint32_t i;

	for (i = 0; i < sizeof(apollo_ro_regs) / sizeof(apollo_ro_regs[0]);
	     i++) {
		if (addr - apollo_ro_regs[i].addr < apollo_ro_regs[i].count) {
			*val = apollo_ro_regs[i].val;
			return true;
		}
	}

	return false;
}

uint8_t apollo_mem_read8(uint32_t addr)
{
	uint8_t *b;
	uint8_t val;

	if (apollo_ro_get(addr, &val))
		return val;

	b = apollo_byte(addr, false);

	return b ? *b : 0;
}

void apollo_mem_write8(uint32_t addr, uint8_t val)
{
	*apollo_byte(addr, true) = val;
}

uint32_t apollo_mem_read32(uint32_t addr)
{
	return apollo_mem_read8(addr) |
	       (apollo_mem_read8(addr + 1) << 8) |
	       (apollo_mem_read8(addr + 2) << 16) |
	       ((uint32_t)apollo_mem_read8(addr + 3) << 24);
}

void apollo_mem_write32(uint32_t addr, uint32_t val)
{
	apollo_mem_write8(addr, val);
	apollo_mem_write8(addr + 1, val >> 8);
	apollo_mem_write8(addr + 2, val >> 16);
	apollo_mem_write8(addr + 3, val >> 24);
}

/******************************************************************************/
/* MCS sync                                                                   */
/******************************************************************************/

/* Sync control, one per MCS block (CENTER, EAST, WEST). */
static const uint32_t mcs_sync_ctrl[] = {
	0x4C001418u, 0x4C001818u, 0x4C001C18u,
};

#define MCS_DYN_SYNC		0x01u	/* -> MCS_DYN_SYNC_DONE */
#define MCS_ONESHOT_SYNC	0x02u	/* -> MCS_ONESHOT_SYNC_DONE */
#define MCS_DYN_SYNC_DONE	0x10u
#define MCS_ONESHOT_SYNC_DONE	0x20u

/* A sync request completes at once (SYSREF assumed present). */
static void mcs_on_write(uint32_t addr, uint8_t val)
{
	uint32_t i;

	for (i = 0; i < sizeof(mcs_sync_ctrl) / sizeof(mcs_sync_ctrl[0]); i++) {
		if (addr != mcs_sync_ctrl[i])
			continue;
		if (val & MCS_DYN_SYNC)
			val |= MCS_DYN_SYNC_DONE;
		if (val & MCS_ONESHOT_SYNC)
			val |= MCS_ONESHOT_SYNC_DONE;
		apollo_mem_write8(addr, val);
	}
}

/******************************************************************************/
/* SPIDMA engine                                                              */
/******************************************************************************/

static uint32_t spidma_addr;

static uint8_t direct_reg(uint32_t off)
{
	return apollo_mem_read8(APOLLO_DIRECT_BASE + off);
}

static void spidma_load_addr(void)
{
	uint32_t i;

	spidma_addr = 0;
	for (i = 0; i < 4; i++)
		spidma_addr = (spidma_addr << 8) |
			      direct_reg(APOLLO_SPIDMA_ADDR3 + i);
}

/* Bytes per SPIDMA access, from CTL (FIFO mode overrides). */
static uint32_t spidma_size(void)
{
	uint8_t fifo = direct_reg(APOLLO_FIFO_MODE) & 0x7;

	if (fifo >= 1 && fifo <= 3)
		return 1u << (fifo - 1);

	return 1u << SPIDMA_CTL_SIZE(direct_reg(APOLLO_SPIDMA_CTL));
}

/* Copy the word at the SPIDMA address into DATA3..0 (DATA0 = LSB). */
static void spidma_fetch(void)
{
	uint32_t n = spidma_size();
	uint32_t i;

	for (i = 0; i < 4; i++)
		apollo_mem_write8(APOLLO_DIRECT_BASE + APOLLO_SPIDMA_DATA0 - i,
				  i < n ? apollo_mem_read8(spidma_addr + i) : 0);
}

static void spidma_advance(void)
{
	if (direct_reg(APOLLO_SPIDMA_CTL) & SPIDMA_CTL_AUTO_INC)
		spidma_addr += spidma_size();
}

/* DATA0 was written: store DATA0..n-1 at the SPIDMA address. */
static void spidma_commit_write(void)
{
	uint32_t n = spidma_size();
	uint32_t i;

	for (i = 0; i < n; i++) {
		apollo_mem_write8(spidma_addr + i,
				  direct_reg(APOLLO_SPIDMA_DATA0 - i));
		mcs_on_write(spidma_addr + i,
			     direct_reg(APOLLO_SPIDMA_DATA0 - i));
	}
	spidma_advance();
}

/* DATA0 was read: step to the next word and prefetch it. */
static void spidma_after_read(void)
{
	spidma_advance();
	spidma_fetch();
}

/******************************************************************************/
/* Core1 boot                                                                 */
/******************************************************************************/

/*
 * Stand-in for the secure boot and Core1 firmware boot.  TinyE verifies the
 * images when the host sets Boot Ready; Core1, once released, it waits for the
 * device profile; once the host flags the profile written it passes the
 * CRC check and runs.
 */
static void boot_on_write(uint32_t off, uint8_t val)
{
	uint32_t status = APOLLO_DIRECT_BASE + APOLLO_CORE1_STATUS;
	uint32_t i;

	if (off == APOLLO_TE_BOOT_READY && val) {
		apollo_mem_write8(TE_STAGE0, TE_STAGE_ENCR_OK);
		apollo_mem_write8(TE_STAGE1, TE_STAGE_ENCR_OK);
		printf("SIM: apollo TinyE boot ready -> stages verified\n");
	} else if (off == APOLLO_ARM1_RUN && (val & 0x1)) {
		for (i = 0; i < 12; i += 4)	/* no boot error */
			apollo_mem_write32(FW_BOOT_ERROR_BLOCK + i, 0);
		apollo_mem_write32(FW_BOOT_ERROR_PTR, FW_BOOT_ERROR_BLOCK);
		apollo_mem_write32(FW_PROFILE_PTR, FW_PROFILE_AREA);
		apollo_mem_write8(status, BOOT_WAIT_FOR_CONFIG);
		printf("SIM: apollo Core1 released -> boot 0x%02X\n",
		       BOOT_WAIT_FOR_CONFIG);
	} else if (off == APOLLO_XFER_DONE && (val & 0x1)) {
		apollo_mem_write8(status, BOOT_APP_RUNNING);
		printf("SIM: apollo profile done -> boot 0x%02X\n",
		       BOOT_APP_RUNNING);
	}
}

/******************************************************************************/
/* Mailbox                                                                    */
/******************************************************************************/

static void mailbox_cal_status(uint32_t objid)
{
	uint32_t i;

	switch (objid) {
	case CAL_OBJID_IC_MCS:
		/* SYSREF locked on every side, zero alignment error */
		for (i = 0; i < 3; i++)
			apollo_mem_write8(MBOX_BUF + MBOX_DATA +
					  MCS_INIT_LOCKED_C + i, 1);
		apollo_mem_write32(MBOX_BUF + MBOX_DATA_LEN, MCS_INIT_LEN);
		break;
	case CAL_OBJID_TC_MCS:
		/* foreground run done; halt acknowledged when asked */
		apollo_mem_write8(MBOX_BUF + MBOX_DATA + MCS_TRACK_FG_DONE, 1);
		apollo_mem_write8(MBOX_BUF + MBOX_DATA + MCS_TRACK_HALT_ACTIVE,
				  1);
		apollo_mem_write32(MBOX_BUF + MBOX_DATA_LEN, MCS_TRACK_LEN);
		break;
	default:
		break;
	}
}

/*
 * Stand-in for the Core1 command handler.  Every command succeeds at once:
 * the response buffer is cleared (status 0 = OK) and busy stays clear.
 * Ping echoes its word, init cals report done and successful, the MCS
 * cal status reports SYSREF locked with no alignment error, and
 * get_fw_version reports a fixed version.
 */
static void mailbox_on_command(uint8_t opcode)
{
	uint32_t echo = apollo_mem_read32(MBOX_BUF);
	uint32_t objid = apollo_mem_read32(MBOX_BUF + 1);
	uint32_t i;

	for (i = 0; i < MBOX_RESP_LEN; i++)
		apollo_mem_write8(MBOX_BUF + i, 0);

	switch (opcode) {
	case MBOX_OP_PING:
		apollo_mem_write32(MBOX_BUF + 4, echo);
		break;
	case MBOX_OP_INIT_COMPLETION:
		apollo_mem_write8(MBOX_BUF + 5, 1);	/* done, success */
		break;
	case MBOX_OP_GET_CAL_STATUS:
		mailbox_cal_status(objid);
		break;
	case MBOX_OP_GET_FW_VERSION:
		apollo_mem_write8(MBOX_BUF + 4, 1);	/* major 1, rest 0 */
		break;
	default:
		break;
	}

	printf("SIM: apollo mailbox opcode %u -> OK\n", opcode);
}

/******************************************************************************/
/* SPI frames                                                                 */
/******************************************************************************/

static uint32_t page_base(void)
{
	uint32_t i, base = 0;

	for (i = 0; i < 4; i++)
		base = (base << 8) | direct_reg(APOLLO_PAGE_BASE + i);

	return base;
}

static uint8_t direct_access(uint32_t off, bool rd, uint8_t val)
{
	uint32_t addr = APOLLO_DIRECT_BASE + off;
	uint8_t ret;

	if (rd) {
		ret = apollo_mem_read8(addr);
		if (off == APOLLO_SPIDMA_DATA0 &&
		    (direct_reg(APOLLO_SPIDMA_CTL) & SPIDMA_CTL_RD))
			spidma_after_read();
		return ret;
	}

	if (off == APOLLO_MBOX_CMD) {
		apollo_mem_write8(addr, val & 0x3F);	/* never busy */
		mailbox_on_command(val & 0x3F);
		return 0;
	}

	apollo_mem_write8(addr, val);
	boot_on_write(off, val);

	if (off == APOLLO_SPIDMA_ADDR0) {
		spidma_load_addr();
		if (direct_reg(APOLLO_SPIDMA_CTL) & SPIDMA_CTL_RD)
			spidma_fetch();
	} else if (off == APOLLO_SPIDMA_DATA0 &&
		   !(direct_reg(APOLLO_SPIDMA_CTL) & SPIDMA_CTL_RD)) {
		spidma_commit_write();
	}

	return 0;
}

void apollo_model_xfer(uint8_t *data, uint16_t len)
{
	bool rd, paged, fifo_hold;
	uint32_t off, base = 0;
	uint16_t i;

	if (len < 3)
		return;

	rd = data[0] & 0x80;
	paged = data[0] & 0x40;
	off = ((data[0] & 0x3F) << 8) | data[1];
	if (paged)
		base = page_base();

	fifo_hold = !paged && off == APOLLO_SPIDMA_DATA0 &&
		    (direct_reg(APOLLO_FIFO_MODE) & 0x7);

	for (i = 2; i < len; i++) {
		if (paged) {
			if (rd)
				data[i] = apollo_mem_read8(base + off);
			else
				apollo_mem_write8(base + off, data[i]);
		} else {
			data[i] = direct_access(off & (APOLLO_DIRECT_LEN - 1),
						rd, data[i]);
		}

		if (!fifo_hold)
			off++;
	}
}
