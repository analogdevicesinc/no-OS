/******************************************************************************
* Copyright (c) 2015 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*****************************************************************************/
/**
* @file translation_table.s
*
* @addtogroup a53_32_boot_code
* @{
* <h2> translation_table.S </h2>
* The translation_table.S contains a static page table required by MMU for
* cortex-A53. This translation table is flat mapped (input address = output
* address) with default memory attributes defined for zynq ultrascale+
* architecture. It utilizes short descriptor translation table format with each
* section defining 1MB of memory.
*
* For DDR in region 0x00000000 - 0x7FFFFFFF, a system where DDR is less than
* 2GB, region after DDR and before PL is marked as undefined/reserved in
* translation table. In region 0xFFC00000 - 0xFFDFFFFF, it contains CSU
* and PMU memory which are marked as Device since it is less than 1MB and
* falls in a region with device memory.
*
* The overview of translation table memory attributes is described below.
*
*|                 | Memory Range            | Definition in Translation Table |
*|-----------------|-------------------------|---------------------------------|
*| DDR             | 0x00000000 - 0x7FFFFFFF | Normal write-back Cacheable     |
*| PL              | 0x80000000 - 0xBFFFFFFF | Strongly Ordered                |
*| QSPI, lower PCIe| 0xC0000000 - 0xEFFFFFFF | Device Memory                   |
*| Reserved        | 0xF0000000 - 0xF7FFFFFF | Unassigned                      |
*| STM Coresight   | 0xF8000000 - 0xF8FFFFFF | Device Memory                   |
*| GIC             | 0xF9000000 - 0xF90FFFFF | Device memory                   |
*| Reserved        | 0xF9100000 - 0xFCFFFFFF | Unassigned		       |
*| FPS, LPS slaves | 0xFD000000 - 0xFFBFFFFF | Device memory                   |
*| CSU, PMU        | 0xFFC00000 - 0xFFDFFFFF | Device Memory                   |
*| TCM, OCM        | 0xFFE00000 - 0xFFFFFFFF | Normal write-back cacheable     |
*
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 5.2	pkp  28/05/15 First release
* 5.4	pkp  18/12/15 Updated the address map according to proper address map
* 6.0   mus  20/07/16 Added warning for ddrless HW design CR-954977
* </pre>
*
*
******************************************************************************/
#include "xparameters.h"
	.globl  MMUTable

	.section .mmu_tbl,"a"

MMUTable:
	/* Each table entry occupies one 32-bit word and there are
	 * 4096 entries, so the entire table takes up 16KB.
	 * Each entry covers a 1MB section.
	 */

.set SECT, 0

#ifdef XPAR_PSU_DDR_0_S_AXI_BASEADDR
.set DDR_START, XPAR_PSU_DDR_0_S_AXI_BASEADDR
.set DDR_END, XPAR_PSU_DDR_0_S_AXI_HIGHADDR
.set DDR_SIZE, (DDR_END - DDR_START)+1
.if DDR_SIZE > 0x80000000
/* If DDR size is larger than 2GB, truncate to 2GB */
.set DDR_REG, 0x800
.else
.set DDR_REG, DDR_SIZE/0x100000
.endif
#else
.set DDR_REG, 0
#warning "There's no DDR in the HW design. MMU translation table marks 2 GB DDR address space as undefined"
#endif
.set UNDEF_REG, 0x800 - DDR_REG

.rept	DDR_REG			/* DDR Cacheable */
.word	SECT + 0x15de6		/* S=b1 TEX=b101 AP=b11, Domain=b1111, C=b0, B=b1 */
.set	SECT, SECT+0x100000
.endr

.rept	UNDEF_REG		/* unassigned/reserved */
				/* Generates a translation fault if accessed */
.word	SECT + 0x0		/* S=b0 TEX=b000 AP=b00, Domain=b0, C=b0, B=b0 */
.set	SECT, SECT+0x100000
.endr

.rept	0x0200			/* 0x80000000 - 0x9fffffff (FPGA slave0) */
.word	SECT + 0xc02		/* S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b1 */
.set	SECT, SECT+0x100000
.endr

.rept	0x0200			/* 0xA0000000 - 0xbfffffff (FPGA slave1) */
.word	SECT + 0xc02		/* S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b1 */
.set	SECT, SECT+0x100000
.endr

.rept	0x0200			/* 0xc0000000 - 0xdfffffff (OSPI IOU)*/
.word	SECT + 0xc06		/* S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b1 */
.set	SECT, SECT+0x100000
.endr

.rept	0x0100			/* 0xe0000000 - 0xefffffff (Lower PCIe)*/
.word	SECT + 0xc06		/* S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b1 */
.set	SECT, SECT+0x100000
.endr

.rept	0x80			/* 0xf0000000 - 0xf7ffffff (unassigned/reserved).
				 * Generates a translation fault if accessed */
.word	SECT + 0x0		/* S=b0 TEX=b000 AP=b00, Domain=b0, C=b0, B=b0 */
.set	SECT, SECT+0x100000
.endr

.rept	0x10			/* 0xf8000000 - 0xf8ffffff (STM Coresight) */
.word	SECT + 0xc06		/* S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b1 */
.set	SECT, SECT+0x100000
.endr

.rept	0x1			/* 0xf9000000 - 0xf90fffff (RPU_A53_GIC) */
.word	SECT + 0xc06		/* S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b1 */
.set	SECT, SECT+0x100000
.endr

.rept	0x3f			/* 0xf9100000 - 0xfcffffff (reserved).*/
.word	SECT + 0x0		/* S=b0 TEX=b000 AP=b00, Domain=b0, C=b0, B=b0 */
.set	SECT, SECT+0x100000
.endr

.rept	0x10			/* 0xfd000000 - 0xfdffffff (FPS Slaves) */
.word	SECT + 0xc06		/* S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b1 */
.set	SECT, SECT+0x100000
.endr

.rept	0x1C			/* 0xfe0000000 - 0xfeffffff (LPS Slaves) */
.word	SECT + 0xc06		/* S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b1 */
.set	SECT, SECT+0x100000
.endr

.rept	0x2			/* 0xffc000000 - 0xffdfffff (CSU and PMU) */
.word	SECT + 0xc06		/* S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b1 */
.set	SECT, SECT+0x100000
.endr

.rept	0x02			/* 0xffe00000 - 0xffffffff (TCM and OCM Cacheable) */
.word	SECT + 0x15de6		/* S=b1 TEX=b101 AP=b11, Domain=b1111, C=b0, B=b1 */
.set	SECT, SECT+0x100000
.endr
.end
/**
* @} End of "addtogroup a53_32_boot_code".
*/
