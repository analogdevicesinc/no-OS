/******************************************************************************
* Copyright (C) 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*****************************************************************************/
/**
* @file translation_table.s
*
* @addtogroup a72_64_boot_code
* @{
* <h2> translation_table.S </h2>
* translation_table.S contains a static page table required by MMU for
* cortex-A72. This translation table is flat mapped (input address = output
* address) with default memory attributes defined for versal
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
* 7.2   mus  01/09/20 Initial version
*
*
******************************************************************************/
#include "xparameters.h"
#include "bspconfig.h"

	EXPORT  MMUTableL0
	EXPORT  MMUTableL1
	EXPORT  MMUTableL2

    GBLA abscnt
    GBLA count
    GBLA sect

Reserved EQU 0x0 				; Fault
#if EL1_NONSECURE
Memory    EQU 0x405:OR:(2:SHL:8):OR:0x0		; normal writeback write allocate outer shared read write */
#else
Memory    EQU 0x405:OR:(3:SHL:8):OR:0x0		; normal writeback write allocate inner shared read write */
#endif
Device    EQU 0x409:OR:(1:SHL:53):OR:(1:SHL:54):OR:0x0	; strongly ordered read write non executable

	AREA |.mmu_tbl0|, CODE, ALIGN=12

MMUTableL0

count SETA 0
	WHILE count<0x1f
	DCQU MMUTableL1+count*0x1000+0x3		; 0x0000_0000 -  0x7F_FFFF_FFFF
count SETA count+1
	WEND

count SETA 1
	WHILE count<0x20
	DCQ MMUTableL1+count*0x1000+0x3		; 0x80_0000_0000 - 0xFFF_FFFF_FFFF
count SETA count+1
	WEND

	AREA |.mmu_tbl1|, CODE, ALIGN=12

MMUTableL1

	DCQU MMUTableL2+0x3		; 0x0000_0000 - 0x3FFF_FFFF
count SETA 1			; 0x4000_0000 - 0x1_3FFF_FFFF
	WHILE count<5
	DCQ MMUTableL2+count*0x1000+0x3		; 1GB DDR, 512MB LPD_AFI_FS, 448MB FPD_AFI_0, 512MB QSPI,
										; 256MB PCIe region 0, PMC 128MB, GIC 1 MB, reserved 47MB,
										; 2GB other devices and memory, 512 MB PMC
count SETA count+1
	WEND

Fixlocl1 EQU 0x140000000
abscnt SETA 0

count SETA 0
	WHILE count<0x3
	DCQU	Fixlocl1+abscnt*0x40000000+Reserved		; 0x1_4000_0000 - 0x1_FFFF_FFFF
													; 3GB Reserved
count  SETA count+1
abscnt SETA abscnt+1
	WEND
count SETA 0
	WHILE count<0x4
	DCQU	Fixlocl1+abscnt*0x40000000+Device	; 0x2_0000_0000 - 0x2_FFFF_FFFF
												; 4GB ME Array 0-3
count  SETA count+1
abscnt SETA abscnt+1
	WEND
count SETA 0
	WHILE count<0x4
	DCQU	Fixlocl1+abscnt*0x40000000+Reserved		; 0x3_0000_0000 - 0x3_FFFF_FFFF
													; 4GB Reserved
count  SETA count+1
abscnt SETA abscnt+1
	WEND

count SETA 0
	WHILE count<0x10
	DCQU	Fixlocl1+abscnt*0x40000000+Device	; 0x4_0000_0000 - 0x7_FFFF_FFFF
												; 8GB PL - via PS, 8GB PCIe region1
count  SETA count+1
abscnt SETA abscnt+1
	WEND

#ifdef XPAR_AXI_NOC_DDR_LOW_1_BASEADDR
DDR_1_START EQU XPAR_AXI_NOC_DDR_LOW_1_BASEADDR
DDR_1_END EQU XPAR_AXI_NOC_DDR_LOW_1_HIGHADDR
DDR_1_SIZE EQU (DDR_1_END - DDR_1_START+1)
#if DDR_1_SIZE > 0x800000000
; If DDR size is larger than 32GB, truncate to 32GB
DDR_1_REG EQU 0x20
#else
DDR_1_REG EQU DDR_1_SIZE/0x40000000
#endif
#else
DDR_1_REG EQU 0
#endif

UNDEF_1_REG EQU (0x20 - DDR_1_REG)

; DDR based on size in hw design
count SETA 0
	WHILE count<DDR_1_REG
	DCQU	Fixlocl1+abscnt*0x40000000+Memory
count  SETA count+1
abscnt SETA abscnt+1
	WEND

; Reserved for region where ddr is absent
count SETA 0
	WHILE count<UNDEF_1_REG
	DCQU	Fixlocl1+abscnt*0x40000000+Reserved
count  SETA count+1
abscnt SETA abscnt+1
	WEND

count SETA 0
	WHILE count<0xC0
	DCQU	Fixlocl1+abscnt*0x40000000+Reserved		; 0x10_0000_0000 - 0x3F_FFFF_FFFF
													; 192GB Reserved
count  SETA count+1
abscnt SETA abscnt+1
	WEND

count SETA 0
	WHILE count<0x100
	DCQU	Fixlocl1+abscnt*0x40000000+Device	; 0x40_0000_0000 - 0x7F_FFFF_FFFF
												; 256GB HBM 0-3
count  SETA count+1
abscnt SETA abscnt+1
	WEND
count SETA 0
	WHILE count<0x100
	DCQU	Fixlocl1+abscnt*0x40000000+Device	; 0x80_0000_0000 - 0xBF_FFFF_FFFF
												; 256GB PCIe 2
count  SETA count+1
abscnt SETA abscnt+1
	WEND

#ifdef XPAR_AXI_NOC_DDR_LOW_2_BASEADDR
DDR_2_START EQU XPAR_AXI_NOC_DDR_LOW_2_BASEADDR
DDR_2_END EQU XPAR_AXI_NOC_DDR_LOW_2_HIGHADDR
DDR_2_SIZE EQU (DDR_2_END - DDR_2_START+1)
#if DDR_2_SIZE > 0x4000000000
; If DDR size is larger than 256 GB, truncate to 256GB
DDR_2_REG EQU 0x100
#else
DDR_2_REG EQU DDR_2_SIZE/0x40000000
#endif
#else
DDR_2_REG EQU 0
#endif

UNDEF_2_REG EQU (0x100 - DDR_2_REG)

; DDR based on size in hw design
count SETA 0
	WHILE count<DDR_2_REG
	DCQU	Fixlocl1+abscnt*0x40000000+Memory
count  SETA count+1
abscnt SETA abscnt+1
	WEND

; Reserved for region where ddr is absent
count SETA 0
	WHILE count<UNDEF_2_REG
	DCQU	Fixlocl1+abscnt*0x40000000+Reserved
count  SETA count+1
abscnt SETA abscnt+1
	WEND

#ifdef XPAR_AXI_NOC_DDR_LOW_3_BASEADDR
DDR_3_START EQU XPAR_AXI_NOC_DDR_LOW_3_BASEADDR
DDR_3_END EQU XPAR_AXI_NOC_DDR_LOW_3_HIGHADDR
DDR_3_SIZE EQU (DDR_3_END - DDR_3_START+1)
#if DDR_3_SIZE > 0xB780000000
; If DDR size is larger than 734 GB, truncate to 734GB
DDR_3_REG EQU 0x2de
#else
DDR_3_REG EQU DDR_3_SIZE/0x40000000
#endif
#else
DDR_3_REG EQU 0
#endif

UNDEF_3_REG EQU (0x2de - DDR_3_REG)

; DDR based on size in hw design
count SETA 0
	WHILE count<DDR_3_REG
	DCQU	Fixlocl1+abscnt*0x40000000+Memory
count  SETA count+1
abscnt SETA abscnt+1
	WEND

; Reserved for region where ddr is absent
count SETA 0
	WHILE count<UNDEF_3_REG
	DCQU	Fixlocl1+abscnt*0x40000000+Reserved
count  SETA count+1
abscnt SETA abscnt+1
	WEND

count SETA 0
	WHILE count<0x122
	DCQU	Fixlocl1+abscnt*0x40000000+Reserved		; 0x1B7_8000_0000 - 0x1FF_FFFF_FFFF
													; 290GB reserved
count  SETA count+1
abscnt SETA abscnt+1
	WEND

count SETA 0
	WHILE count<0x800
	DCQU	Fixlocl1+abscnt*0x40000000+Device	; 0x200_0000_0000 - 0x3FF_FFFF_FFFF
												; 2TB PL- via NoC
count  SETA count+1
abscnt SETA abscnt+1
	WEND

count SETA 0
	WHILE count<0x400
	DCQU	Fixlocl1+abscnt*0x40000000+Device	; 0x400_0000_0000 - 0x4FF_FFFF_FFFF
												; 1TB PL- via PS
count  SETA count+1
abscnt SETA abscnt+1
	WEND

#ifdef XPAR_AXI_NOC_DDR_CH_1_BASEADDR
DDR_CH_1_START EQU XPAR_AXI_NOC_DDR_CH_1_BASEADDR
DDR_CH_1_END EQU XPAR_AXI_NOC_DDR_CH_1_HIGHADDR
DDR_CH_1_SIZE EQU (DDR_CH_1_END - DDR_CH_1_START + 1)
#if DDR_CH_1_SIZE > 0x010000000000
; If DDR size is larger than 1TB, truncate to 1 TB
DDR_CH_1_REG EQU 0x400		; 0x500_0000_0000 - 0x5FF_FFFF_FFFF
#else
DDR_CH_1_REG EQU DDR_CH_1_SIZE/0x40000000
#endif
#else
DDR_CH_1_REG EQU 0
#endif

UNDEF_CH_1_REG EQU (0x400 - DDR_CH_1_REG)

; DDR based on size in hw design, Max size 1 TB
count SETA 0
	WHILE count<DDR_CH_1_REG
	DCQU	Fixlocl1+abscnt*0x40000000+Memory
count  SETA count+1
abscnt SETA abscnt+1
	WEND

; Reserved for region where ddr is absent
count SETA 0
	WHILE count<UNDEF_CH_1_REG
	DCQU	Fixlocl1+abscnt*0x40000000+Reserved
count SETA count+1
abscnt SETA abscnt+1
	WEND

#ifdef XPAR_AXI_NOC_DDR_CH_2_BASEADDR
DDR_CH_2_START EQU XPAR_AXI_NOC_DDR_CH_2_BASEADDR
DDR_CH_2_END EQU XPAR_AXI_NOC_DDR_CH_2_HIGHADDR
DDR_CH_2_SIZE EQU (DDR_CH_2_END - DDR_CH_2_START + 1)
#if DDR_CH_2_SIZE > 0x010000000000
; If DDR_CH_2 size is larger than 1TB, truncate to 1 TB
DDR_CH_2_REG EQU 0x400		; 0x600_0000_0000 - 0x6FF_FFFF_FFFF
#else
DDR_CH_2_REG EQU DDR_CH_2_SIZE/0x40000000
#endif
#else
DDR_CH_2_REG EQU  0
#endif

UNDEF_CH_2_REG EQU (0x400 - DDR_CH_2_REG)

; DDR based on size in hw design, Max size 1 TB
count SETA 0
	WHILE count<DDR_CH_2_REG
	DCQU	Fixlocl1+abscnt*0x40000000+Memory
count  SETA count+1
abscnt SETA abscnt+1
	WEND

; Reserved for region where ddr is absent
count SETA 0
	WHILE count<UNDEF_CH_2_REG
	DCQU	Fixlocl1+abscnt*0x40000000+Reserved
count  SETA count+1
abscnt SETA abscnt+1
	WEND

#ifdef XPAR_AXI_NOC_DDR_CH_3_BASEADDR
DDR_CH_3_START EQU XPAR_AXI_NOC_DDR_CH_3_BASEADDR
DDR_CH_3_END EQU XPAR_AXI_NOC_DDR_CH_3_HIGHADDR
DDR_CH_3_SIZE EQU (DDR_CH_3_END - DDR_CH_3_START+1)
#if DDR_CH_3_SIZE > 0x010000000000
; If DDR_CH_3 size is larger than 1TB, truncate to 1 TB */
DDR_CH_3_REG EQU 0x400		; 0x700_0000_0000 - 0x7FF_FFFF_FFFF
#else
DDR_CH_3_REG EQU DDR_CH_3_SIZE/0x40000000
#endif
#else
DDR_CH_3_REG EQU 0
#endif

UNDEF_CH_3_REG EQU (0x400 - DDR_CH_3_REG)

; DDR based on size in hw design, Max size 1 TB
count SETA 0
	WHILE count<DDR_CH_3_REG
	DCQU	Fixlocl1+abscnt*0x40000000+Memory
count  SETA count+1
abscnt SETA abscnt+1
	WEND

; Reserved for region where ddr is absent
count SETA 0
	WHILE count<UNDEF_CH_3_REG
	DCQU	Fixlocl1+abscnt*0x40000000+Reserved
count  SETA count+1
abscnt SETA abscnt+1
	WEND


count SETA 0
	WHILE count<0x2000
	DCQU	Fixlocl1+abscnt*0x40000000+Device
count  SETA count+1
abscnt SETA abscnt+1
	WEND

	AREA |.mmu_tbl2|, CODE, ALIGN=12

MMUTableL2
abscnt SETA 0

#ifdef XPAR_AXI_NOC_DDR_LOW_0_BASEADDR
DDR_0_START EQU XPAR_AXI_NOC_DDR_LOW_0_BASEADDR
DDR_0_END EQU XPAR_AXI_NOC_DDR_LOW_0_HIGHADDR
DDR_0_SIZE EQU (DDR_0_END - DDR_0_START+1)
#if DDR_0_SIZE > 0x80000000
; If DDR size is larger than 2GB, truncate to 2GB
.set DDR_0_REG, 0x400
#else
DDR_0_REG EQU DDR_0_SIZE/0x200000
#endif
#else
DDR_0_REG EQU 0
#endif

UNDEF_0_REG EQU  (0x400 - DDR_0_REG)

; DDR based on size in hw design
count SETA 0
	WHILE count<DDR_0_REG
	DCQU	abscnt*0x200000+Memory
count  SETA count+1
abscnt SETA abscnt+1
	WEND

; Reserved for region where ddr is absent
count SETA 0
	WHILE count<UNDEF_0_REG
	DCQU	abscnt*0x200000+Reserved
count  SETA count+1
abscnt SETA abscnt+1
	WEND

count SETA 0
	WHILE count<0x100
	DCQU	abscnt*0x200000+Device
count  SETA count+1
abscnt SETA abscnt+1
	WEND
count SETA 0
	WHILE count<0x20				; 0xA000_0000 - 0xA3FF_FFFF
	DCQU	abscnt*0x200000+Device	; 64MB reserved
count  SETA count+1
abscnt SETA abscnt+1
	WEND

count SETA 0
	WHILE count<0x60				; 0xA400_0000 - 0xAFFF_FFFF
	DCQU	abscnt*0x200000+Device	;  192MB FPD AFI 0
count  SETA count+1
abscnt SETA abscnt+1
	WEND

count SETA 0
	WHILE count<0x80				; 0xB000_0000 - 0xBFFF_FFFF
   DCQU	abscnt*0x200000+Device		; 192MB FPD AFI 1
count  SETA count+1
abscnt SETA abscnt+1
	WEND

count SETA 0
	WHILE count<0x100					; 0xC000_0000 - 0xDFFF_FFFF
	DCQU	abscnt*0x200000+Device		; 512MB QSPI
count  SETA count+1
abscnt SETA abscnt+1
	WEND
count SETA 0
	WHILE count<0x80					; 0xE000_0000 - 0xEFFF_FFFF
	DCQU	abscnt*0x200000+Device		; 256MB lower PCIe
count  SETA count+1
abscnt SETA abscnt+1
	WEND

count SETA 0
	WHILE count<0x40					; 0xF000_0000 - 0xF7FF_FFFF
	DCQU	abscnt*0x200000+Device		; 128MB PMC
count  SETA count+1
abscnt SETA abscnt+1
	WEND

count SETA 0
	WHILE count<0x8						; 0xF800_0000 - 0xF8FF_FFFF
	DCQU	abscnt*0x200000+Device		; 16MB coresight
count  SETA count+1
abscnt SETA abscnt+1
	WEND


; 1MB GIC is marked for 2MB region as the minimum block size in
; translation table is 2MB and adjacent 47MB reserved region is
; converted to 46MB
	DCQU	abscnt*0x200000+Device			; 0xF910_0000 - 0xF90F_FFFF
abscnt SETA abscnt+1

; Reserved 46MB  0xF91FFFFF - 0xFBFFFFFF
count SETA 0
	WHILE count<0x17						; 0xF91F_FFFF - 0xFBFF_FFFF
	DCQU	abscnt*0x200000+Reserved		; 46MB reserved
count  SETA count+1
abscnt SETA abscnt+1
	WEND

count SETA 0
	WHILE count<0x1F					; 0xFC00_0000 - 0xFFDF_FFFF
	DCQU	abscnt*0x200000+Device		; 16MB CPM,16MB FPS, 30MB LPS slaves
count  SETA count+1
abscnt SETA abscnt+1
	WEND

	DCQU	abscnt*0x200000+Memory		; 0xFFE0_0000 - 0xFFFF_FFFF
abscnt SETA abscnt+1
count SETA 0
	WHILE count<0x100					; 0x1_0000_0000 - 0x1_1FFF_FFFF
	DCQU	abscnt*0x200000+Device		; 512MB PMC 0-3
count  SETA count+1
abscnt SETA abscnt+1
	WEND

count SETA 0
	WHILE count<0x100					; 0x1_2000_0000  - 0x1_3FFF_FFFF
	DCQU abscnt*0x200000+Device			; 512MB reserved
count  SETA count+1
abscnt SETA abscnt+1
	WEND
	END
/**
* @} End of "addtogroup a53_64_boot_code".
*/
