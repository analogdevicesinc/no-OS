/******************************************************************************
* Copyright (C) 2018 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*****************************************************************************/
/**
* @file translation_table.s
*
* @addtogroup a53_64_boot_code
* @{
* <h2> translation_table.S </h2>
* The translation_table.S contains a static page table required by MMU for
* cortex-A72. This translation table is flat mapped (input address = output
* address) with default memory attributes defined for Versal
* architecture. It utilizes translation granual size of 4KB with 2MB section
* size for initial 5GB memory and 1GB section size for memory after 5GB.
* The overview of translation table memory attributes is described below.
*
*| Name                  | Memory Range                      | Def. in Translation Table   |
*|-----------------------|-----------------------------------|-----------------------------|
*| DDR                   | 0x000_0000_0000 - 0x000_7FFF_FFFF | Normal WB Cacheable         |
*| LPD_AFI_FS            | 0x000_8000_0000 - 0x000_9FFF_FFFF | Strongly Ordered            |
*| Reserved              | 0x000_A000_0000 - 0x000_A3FF_FFFF | Unassigned                  |
*| FPD_AFI_0             | 0x000_A400_0000 - 0x000_AFFF_FFFF | Strongly Ordered            |
*| FPD_AFI_1             | 0x000_B000_0000 - 0x000_BFFF_FFFF | Strongly Ordered            |
*| QSPI                  | 0x000_C000_0000 - 0x000_DFFF_FFFF | Strongly Ordered            |
*| PCIE region 0         | 0x000_E000_0000 - 0x000_EFFF_FFFF | Strongly Ordered            |
*| PMC                   | 0x000_F000_0000 - 0x000_F7FF_FFFF | Strongly Ordered            |
*| STM_CORESIGHT         | 0x000_F800_0000 - 0x000_F8FF_FFFF | Strongly Ordered            |
*| GIC                   | 0x000_F900_0000 - 0x000_F90F_FFFF | Strongly Ordered            |
*| Reserved              | 0x000_F910_0000 - 0x000_FBFF_FFFF | Unassigned                  |
*| CPM                   | 0x000_FC00_0000 - 0x000_FCFF_FFFF | Strongly Ordered            |
*| FPD slaves            | 0x000_FD00_0000 - 0x000_FDFF_FFFF | Strongly Ordered            |
*| LPD slaves            | 0x000_FE00_0000 - 0x000_FFDF_FFFF | Strongly Ordered            |
*| OCM                   | 0x000_FFE0_0000 - 0xFFF_FFFF_FFFF | Normal WB Cacheable         |
*| PMC region 0-3        | 0x001_0000_0000 - 0x001_1FFF_FFFF | Strongly Ordered            |
*| Reserved              | 0x001_2000_0000 - 0x001_FFFF_FFFF | Unassigned                  |
*| ME Array 0-3          | 0x002_0000_0000 - 0x002_FFFF_FFFF | Strongly Ordered            |
*| Reserved              | 0x003_0000_0000 - 0x003_FFFF_FFFF | Unassigned                  |
*| PL- via PS            | 0x004_0000_0000 - 0x005_FFFF_FFFF | Strongly Ordered            |
*| PCIe region 1         | 0x006_0000_0000 - 0x007_FFFF_FFFF | Strongly Ordered            |
*| DDR                   | 0x008_0000_0000 - 0x00F_FFFF_FFFF | Normal WB Cacheable         |
*| Reserved              | 0x010_0000_0000 - 0x03F_FFFF_FFFF | Unassigned                  |
*| HBM 0-3               | 0x040_0000_0000 - 0x07F_FFFF_FFFF | Strongly Ordered            |
*| PCIe region 2         | 0x080_0000_0000 - 0x0BF_FFFF_FFFF | Strongly Ordered            |
*| DDR                   | 0x0C0_0000_0000 - 0x1B7_7FFF_FFFF | Normal WB Cacheable         |
*| Reserved              | 0x1B7_8000_0000 - 0x1FF_FFFF_FFFF | Unassigned                  |
*| PL- Via NoC           | 0x200_0000_0000 - 0x3FF_FFFF_FFFF | Strongly Ordered            |
*| PL- Via PS            | 0x400_0000_0000 - 0x4FF_FFFF_FFFF | Strongly Ordered            |
*| DDR CH1-CH3           | 0x500_0000_0000 - 0x7FF_FFFF_FFFF | Normal WB Cacheable         |
*| PL- Via NoC           | 0x800_0000_0000 - 0xFFF_FFFF_FFFF | Strongly Ordered            |
*
* @note
*
* For DDR region 0x0000000000 - 0x007FFFFFFF, a system where DDR is less than
* 2GB, region after DDR and before PL is marked as undefined/reserved in
* translation table. Region 0xF9100000 - 0xF91FFFFF is reserved memory in
* 0x00F9000000 - 0x00F91FFFFF range, but it is marked as strongly ordered
* because minimum section size in translation table section is 2MB.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 7.00  mus  05/21/14 Initial version
* 7.00  mus  03/16/19 Updated translation table to mark DDR regions as
*                     memory, based on the DDR size in hdf
* 7.1   mus  08/29/19 Updated translation table entries for DDR_CH_1,
*                     DDR_CH_2 and DDR_CH_3 based on respective size in hdf
* 7.3   mus  09/10/20 Updated translation table entries for DDR regions, to
*                     support HW designs, where DDR memory is mapped at
*                     address other than DDR region base address. It fixes
*                     CR#1073099.
*
*
******************************************************************************/
#include "xparameters.h"
#include "bspconfig.h"

	.globl  MMUTableL0
	.globl  MMUTableL1
	.globl  MMUTableL2

	.set UNDEF_START, 0

	/* Base addresses for all DDR regions in Versal SoC */
	.set DDR_LOW_0_REGION_START_ADDR, 0x0000000000		/* 0x0000_0000_0000 */
	.set DDR_LOW_1_REGION_START_ADDR, 0x800000000		/* 0x0008_0000_0000 */
	.set DDR_LOW_2_REGION_START_ADDR, 0xC000000000		/* 0x00C0_0000_0000 */
	.set DDR_LOW_3_REGION_START_ADDR, 0x10000000000		/* 0x0100_0000_0000 */
	.set DDR_CH_1_REGION_START_ADDR, 0x50000000000		/* 0x0500_0000_0000 */
	.set DDR_CH_2_REGION_START_ADDR, 0x60000000000		/* 0x0600_0000_0000 */
	.set DDR_CH_3_REGION_START_ADDR, 0x70000000000		/* 0x0700_0000_0000 */

	.set reserved,	0x0 					/* Fault*/
	#if EL1_NONSECURE
	.set Memory,	0x405 | (2 << 8) | (0x0)		/* normal writeback write allocate outer shared read write */
	#else
	.set Memory,	0x405 | (3 << 8) | (0x0)		/* normal writeback write allocate inner shared read write */
	#endif
	.set Device,	0x409 | (1 << 53)| (1 << 54) |(0x0)	/* strongly ordered read write non executable*/
	.section .mmu_tbl0,"a"

MMUTableL0:

.set SECT, MMUTableL1		/* 0x0000_0000 -  0x7F_FFFF_FFFF */
.8byte	SECT + 0x3

.rept	0x1f
.set SECT, SECT + 0x1000	/* 0x80_0000_0000 - 0xFFF_FFFF_FFFF */
.8byte	SECT + 0x3
.endr


	.section .mmu_tbl1,"a"

MMUTableL1:

.set SECT, MMUTableL2		/* 0x0000_0000 - 0x3FFF_FFFF */
.8byte	SECT + 0x3		/* 1GB DDR */

.rept	0x4			/* 0x4000_0000 - 0x1_3FFF_FFFF */
.set SECT, SECT + 0x1000	/*1GB DDR, 512MB LPD_AFI_FS, 448MB FPD_AFI_0, 512MB QSPI,
				  256MB PCIe region 0, PMC 128MB, GIC 1 MB, reserved 47MB,
				  2GB other devices and memory, 512 MB PMC */
.8byte	SECT + 0x3
.endr

.set SECT,0x140000000

.rept	0x3			/* 0x1_4000_0000 - 0x1_FFFF_FFFF */
.8byte	SECT + reserved		/* 3GB Reserved */
.set SECT, SECT + 0x40000000
.endr

.rept	0x4			/* 0x2_0000_0000 - 0x2_FFFF_FFFF */
.8byte	SECT + Device		/* 4GB ME Array 0-3*/
.set SECT, SECT + 0x40000000
.endr

.rept	0x4			/* 0x3_0000_0000 - 0x3_FFFF_FFFF */
.8byte	SECT + reserved		/* 4GB Reserved */
.set SECT, SECT + 0x40000000
.endr

.rept	0x10			/* 0x4_0000_0000 - 0x7_FFFF_FFFF */
.8byte	SECT + Device		/* 8GB PL - via PS, 8GB PCIe region1 */
.set SECT, SECT + 0x40000000
.endr

.set UNDEF_START, 0
#ifdef XPAR_AXI_NOC_DDR_LOW_1_BASEADDR
.set DDR_1_START, XPAR_AXI_NOC_DDR_LOW_1_BASEADDR
.set DDR_1_END, XPAR_AXI_NOC_DDR_LOW_1_HIGHADDR

.if DDR_1_START > DDR_LOW_1_REGION_START_ADDR
.set UNDEF_START, (DDR_1_START - DDR_LOW_1_REGION_START_ADDR)
.set UNDEF_START, (UNDEF_START)/0x40000000
.endif

.set DDR_1_SIZE, (DDR_1_END - DDR_1_START)+1
.if DDR_1_SIZE > 0x800000000
/* If DDR size is larger than 32GB, truncate to 32GB */
.set DDR_1_REG, 0x20
.else
.set DDR_1_REG, DDR_1_SIZE/0x40000000
.endif
#else
.set DDR_1_REG, 0
#warning "There's no DDR_1 in the HW design. MMU translation table marks 32 GB DDR address space as undefined"
#endif

.set UNDEF_1_REG, (0x20 - DDR_1_REG - UNDEF_START)

.rept	UNDEF_START				/* reserved for region where ddr is absent */
.8byte	SECT + reserved
.set	SECT, SECT+0x40000000
.endr

.rept	DDR_1_REG			/* DDR based on size in hdf*/
.8byte	SECT + Memory
.set	SECT, SECT+0x40000000
.endr

.rept	UNDEF_1_REG		/* reserved for region where ddr is absent */
.8byte	SECT + reserved
.set	SECT, SECT+0x40000000
.endr
.rept	0xC0			/* 0x10_0000_0000 - 0x3F_FFFF_FFFF */
.8byte	SECT + reserved		/* 192GB Reserved */
.set SECT, SECT + 0x40000000
.endr

.rept	0x100			/* 0x40_0000_0000 - 0x7F_FFFF_FFFF */
.8byte	SECT + Device		/* 256GB HBM 0-3*/
.set SECT, SECT + 0x40000000
.endr

.rept	0x100			/* 0x80_0000_0000 - 0xBF_FFFF_FFFF */
.8byte	SECT + Device		/* 256GB PCIe 2 */
.set SECT, SECT + 0x40000000
.endr

.set UNDEF_START, 0
#ifdef XPAR_AXI_NOC_DDR_LOW_2_BASEADDR
.set DDR_2_START, XPAR_AXI_NOC_DDR_LOW_2_BASEADDR
.set DDR_2_END, XPAR_AXI_NOC_DDR_LOW_2_HIGHADDR

.if DDR_2_START > DDR_LOW_2_REGION_START_ADDR
.set UNDEF_START, (DDR_2_START - DDR_LOW_2_REGION_START_ADDR)
.set UNDEF_START, (UNDEF_START)/0x40000000
.endif

.set DDR_2_SIZE, (DDR_2_END - DDR_2_START)+1
.if DDR_2_SIZE > 0x4000000000
/* If DDR size is larger than 256 GB, truncate to 256GB */
.set DDR_2_REG, 0x100
.else
.set DDR_2_REG, DDR_2_SIZE/0x40000000
.endif
#else
.set DDR_2_REG, 0
#warning "There's no DDR_LOW_2 in the HW design. MMU translation table marks 256 GB DDR address space as undefined"
#endif

.set UNDEF_2_REG, (0x100 - DDR_2_REG - UNDEF_START)

.rept	UNDEF_START				/* reserved for region where ddr is absent */
.8byte	SECT + reserved
.set	SECT, SECT+0x40000000
.endr

.rept	DDR_2_REG		/*  DDR based on size in hdf 0xC0_0000_0000 - 0xFF_FFFF_FFFF */
.8byte	SECT + Memory		/* Maximum DDR region size - 256GB */
.set SECT, SECT + 0x40000000
.endr

.rept	UNDEF_2_REG		/* reserved for region where ddr is absent */
.8byte	SECT + reserved
.set	SECT, SECT+0x40000000
.endr

.set UNDEF_START, 0
#ifdef XPAR_AXI_NOC_DDR_LOW_3_BASEADDR
.set DDR_3_START, XPAR_AXI_NOC_DDR_LOW_3_BASEADDR
.set DDR_3_END, XPAR_AXI_NOC_DDR_LOW_3_HIGHADDR

.if DDR_3_START > DDR_LOW_3_REGION_START_ADDR
.set UNDEF_START, (DDR_3_START - DDR_LOW_3_REGION_START_ADDR)
.set UNDEF_START, (UNDEF_START)/0x40000000
.endif

.set DDR_3_SIZE, (DDR_3_END - DDR_3_START)+1
.if DDR_3_SIZE > 0xB780000000
/* If DDR size is larger than 734 GB, truncate to 734GB */
.set DDR_3_REG, 0x2de
.else
.set DDR_3_REG, DDR_3_SIZE/0x40000000
.endif
#else
.set DDR_3_REG, 0
#warning "There's no DDR_LOW_3 in the HW design. MMU translation table marks 734 GB DDR address space as undefined"
#endif

.set UNDEF_3_REG, (0x2de - DDR_3_REG - UNDEF_START)

.rept	UNDEF_START				/* reserved for region where ddr is absent */
.8byte	SECT + reserved
.set	SECT, SECT+0x40000000
.endr

.rept	DDR_3_REG		/* DDR based on size in hdf 0x100_0000_0000 - 0x1B7_7FFF_FFFF */
.8byte	SECT + Memory		/* Maximum DDR region size - 734GB DDR */
.set SECT, SECT + 0x40000000
.endr

.rept	UNDEF_3_REG		/* reserved for region where ddr is absent */
.8byte	SECT + reserved
.set	SECT, SECT+0x40000000
.endr

.rept	0x122			/* 0x1B7_8000_0000 - 0x1FF_FFFF_FFFF */
.8byte	SECT + reserved		/* 290GB reserved */
.set SECT, SECT + 0x40000000
.endr

.rept	0x800			/* 0x200_0000_0000 - 0x3FF_FFFF_FFFF */
.8byte	SECT + Device		/* 2TB PL- via NoC */
.set SECT, SECT + 0x40000000
.endr

.rept	0x400			/* 0x400_0000_0000 - 0x4FF_FFFF_FFFF */
.8byte	SECT + Device		/* 1TB PL- via PS */
.set SECT, SECT + 0x40000000
.endr

.set UNDEF_START, 0
#ifdef XPAR_AXI_NOC_DDR_CH_1_BASEADDR
.set DDR_CH_1_START, XPAR_AXI_NOC_DDR_CH_1_BASEADDR
.set DDR_CH_1_END, XPAR_AXI_NOC_DDR_CH_1_HIGHADDR

.if DDR_CH_1_START > DDR_CH_1_REGION_START_ADDR
.set UNDEF_START, (DDR_CH_1_START - DDR_CH_1_REGION_START_ADDR)
.set UNDEF_START, (UNDEF_START)/0x40000000
.endif

.set DDR_CH_1_SIZE, (DDR_CH_1_END - DDR_CH_1_START)+1
.if DDR_CH_1_SIZE > 0x010000000000
/* If DDR size is larger than 1TB, truncate to 1 TB */
.set DDR_CH_1_REG, 0x400		/* 0x500_0000_0000 - 0x5FF_FFFF_FFFF */
.else
.set DDR_CH_1_REG, DDR_CH_1_SIZE/0x40000000
.endif
#else
.set DDR_CH_1_REG, 0
#warning "There's no DDR_CH_1 in the HW design. MMU translation table marks 1 TB DDR address space as undefined"
#endif

.set UNDEF_CH_1_REG, (0x400 - DDR_CH_1_REG - UNDEF_START)

.rept	UNDEF_START				/* reserved for region where ddr is absent */
.8byte	SECT + reserved
.set	SECT, SECT+0x40000000
.endr

.rept	DDR_CH_1_REG			/* DDR based on size in hdf, Max size 1 TB*/
.8byte	SECT + Memory
.set	SECT, SECT+0x40000000
.endr

.rept	UNDEF_CH_1_REG		/* reserved for region where ddr is absent */
.8byte	SECT + reserved
.set	SECT, SECT+0x40000000
.endr

.set UNDEF_START, 0
#ifdef XPAR_AXI_NOC_DDR_CH_2_BASEADDR
.set DDR_CH_2_START, XPAR_AXI_NOC_DDR_CH_2_BASEADDR
.set DDR_CH_2_END, XPAR_AXI_NOC_DDR_CH_2_HIGHADDR

.if DDR_CH_2_START > DDR_CH_2_REGION_START_ADDR
.set UNDEF_START, (DDR_CH_2_START - DDR_CH_2_REGION_START_ADDR)
.set UNDEF_START, (UNDEF_START)/0x40000000
.endif

.set DDR_CH_2_SIZE, (DDR_CH_2_END - DDR_CH_2_START)+1
.if DDR_CH_2_SIZE > 0x010000000000
/* If DDR_CH_2 size is larger than 1TB, truncate to 1 TB */
.set DDR_CH_2_REG, 0x400		/* 0x600_0000_0000 - 0x6FF_FFFF_FFFF */
.else
.set DDR_CH_2_REG, DDR_CH_2_SIZE/0x40000000
.endif
#else
.set DDR_CH_2_REG, 0
#warning "There's no DDR_CH_2 in the HW design. MMU translation table marks 1 TB DDR address space as undefined"
#endif

.set UNDEF_CH_2_REG, (0x400 - DDR_CH_2_REG - UNDEF_START)

.rept	UNDEF_START				/* reserved for region where ddr is absent */
.8byte	SECT + reserved
.set	SECT, SECT+0x40000000
.endr

.rept	DDR_CH_2_REG			/* DDR based on size in hdf, Max size 1 TB*/
.8byte	SECT + Memory
.set	SECT, SECT+0x40000000
.endr

.rept	UNDEF_CH_2_REG		/* reserved for region where ddr is absent */
.8byte	SECT + reserved
.set	SECT, SECT+0x40000000
.endr

.set UNDEF_START, 0
#ifdef XPAR_AXI_NOC_DDR_CH_3_BASEADDR
.set DDR_CH_3_START, XPAR_AXI_NOC_DDR_CH_3_BASEADDR
.set DDR_CH_3_END, XPAR_AXI_NOC_DDR_CH_3_HIGHADDR

.if DDR_CH_3_START > DDR_CH_3_REGION_START_ADDR
.set UNDEF_START, (DDR_CH_3_START - DDR_CH_3_REGION_START_ADDR)
.set UNDEF_START, (UNDEF_START)/0x40000000
.endif

.set DDR_CH_3_SIZE, (DDR_CH_3_END - DDR_CH_3_START)+1
.if DDR_CH_3_SIZE > 0x010000000000
/* If DDR_CH_3 size is larger than 1TB, truncate to 1 TB */
.set DDR_CH_3_REG, 0x400		/* 0x700_0000_0000 - 0x7FF_FFFF_FFFF */
.else
.set DDR_CH_3_REG, DDR_CH_3_SIZE/0x40000000
.endif
#else
.set DDR_CH_3_REG, 0
#warning "There's no DDR_CH_3 in the HW design. MMU translation table marks 1 TB DDR address space as undefined"
#endif

.set UNDEF_CH_3_REG, (0x400 - DDR_CH_3_REG - UNDEF_START)

.rept	UNDEF_START				/* reserved for region where ddr is absent */
.8byte	SECT + reserved
.set	SECT, SECT+0x40000000
.endr

.rept	DDR_CH_3_REG			/* DDR based on size in hdf, Max size 1 TB*/
.8byte	SECT + Memory
.set	SECT, SECT+0x40000000
.endr

.rept	UNDEF_CH_3_REG		/* reserved for region where ddr is absent */
.8byte	SECT + reserved
.set	SECT, SECT+0x40000000
.endr


.rept	0x2000			/* 0x800_0000_0000 - 0xFFF_FFFF_FFFF */
.8byte	SECT + Device		/* 8TB PL- via NoC*/
.set SECT, SECT + 0x40000000
.endr

.section .mmu_tbl2,"a"

MMUTableL2:

.set SECT, 0

.set UNDEF_START, 0
#ifdef XPAR_AXI_NOC_DDR_LOW_0_BASEADDR
.set DDR_0_START, XPAR_AXI_NOC_DDR_LOW_0_BASEADDR
.set DDR_0_END, XPAR_AXI_NOC_DDR_LOW_0_HIGHADDR

.if DDR_0_START > DDR_LOW_0_REGION_START_ADDR
.set UNDEF_START, (DDR_0_START - DDR_LOW_0_REGION_START_ADDR)
.set UNDEF_START, (UNDEF_START)/0x200000
.endif

.set DDR_0_SIZE, (DDR_0_END - DDR_0_START)+1
.if DDR_0_SIZE > 0x80000000
/* If DDR size is larger than 2GB, truncate to 2GB */
.set DDR_0_REG, 0x400
.else
.set DDR_0_REG, DDR_0_SIZE/0x200000
.endif
#else
.set DDR_0_REG, 0
#warning "There's no DDR_0 in the HW design. MMU translation table marks 2 GB DDR address space as undefined"
#endif

.set UNDEF_0_REG, (0x400 - DDR_0_REG - UNDEF_START)

.rept	UNDEF_START				/* reserved for region where ddr is absent */
.8byte	SECT + reserved
.set	SECT, SECT+0x200000
.endr

.rept	DDR_0_REG			/* DDR based on size in hdf*/
.8byte	SECT + Memory
.set	SECT, SECT+0x200000
.endr

.rept	UNDEF_0_REG		/* reserved for region where ddr is absent */
.8byte	SECT + reserved
.set	SECT, SECT+0x200000
.endr


.rept	0x0100			/* 0x8000_0000 - 0xBFFF_FFFF */
.8byte	SECT + Device		/* 512MB LPD AFI */
.set	SECT, SECT+0x200000
.endr

.rept	0x020			/* 0xA000_0000 - 0xA3FF_FFFF */
.8byte	SECT + reserved		/* 64MB reserved*/
.set	SECT, SECT+0x200000
.endr


.rept	0x60			/* 0xA400_0000 - 0xAFFF_FFFF */
.8byte	SECT + Device		/* 192MB FPD AFI 0 */
.set	SECT, SECT+0x200000
.endr

.rept	0x80			/* 0xB000_0000 - 0xBFFF_FFFF */
.8byte	SECT + Device		/* 192MB FPD AFI 1 */
.set	SECT, SECT+0x200000
.endr

.rept	0x100			/* 0xC000_0000 - 0xDFFF_FFFF */
.8byte	SECT + Device		/* 512MB QSPI */
.set	SECT, SECT+0x200000
.endr

.rept	0x080			/* 0xE000_0000 - 0xEFFF_FFFF */
.8byte	SECT + Device		/* 256MB lower PCIe */
.set	SECT, SECT+0x200000
.endr

.rept	0x040			/* 0xF000_0000 - 0xF7FF_FFFF */
.8byte	SECT + Device		/* 128MB PMC */
.set	SECT, SECT+0x200000
.endr

.rept	0x08			/* 0xF800_0000 - 0xF8FF_FFFF */
.8byte	SECT + Device		/* 16MB coresight */
.set	SECT, SECT+0x200000
.endr

/* 1MB GIC is marked for 2MB region as the minimum block size in
   translation table is 2MB and adjacent 47MB reserved region is
   converted to 46MB */
.8byte	SECT + Device		/* 0xF910_0000 - 0xF90F_FFFF */

/* Reserved 46MB  0xF91FFFFF - 0xFBFFFFFF*/
.rept	0x17			/* 0xF91F_FFFF - 0xFBFF_FFFF */
.set	SECT, SECT+0x200000
.8byte	SECT + reserved		/* 46MB reserved */
.endr

.rept	0x1F			/* 0xFC00_0000 - 0xFFDF_FFFF */
.set	SECT, SECT+0x200000
.8byte	SECT + Device		/* 16MB CPM,16MB FPS, 30MB LPS slaves */
.endr

.set	SECT, SECT+0x200000	/* 0xFFE0_0000 - 0xFFFF_FFFF*/
.8byte  SECT + Memory		/*2MB OCM/TCM*/

.rept	0x100			/* 0x1_0000_0000  - 0x1_1FFF_FFFF */
.set	SECT, SECT+0x200000
.8byte	SECT + Device		/* 512MB PMC 0-3 */
.endr

.rept	0x100			/* 0x1_2000_0000  - 0x1_3FFF_FFFF */
.set	SECT, SECT+0x200000
.8byte	SECT + reserved		/* 512MB reserved */
.endr
.end
/**
* @} End of "addtogroup a53_64_boot_code".
*/
