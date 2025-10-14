;/******************************************************************************
;* Copyright (c) 2018 - 2020 Xilinx, Inc.  All rights reserved.
;* SPDX-License-Identifier: MIT
;******************************************************************************/
;/*****************************************************************************/
;/**
;* @file translation_table.s
;*
;* @addtogroup a53_64_boot_code
;* @{
;* <h2> translation_table.S </h2>
;* translation_table.S contains a static page table required by MMU for
;* cortex-A53. This translation table is flat mapped (input address = output
;* address) with default memory attributes defined for zynq ultrascale+
;* architecture. It utilizes translation granule size of 4KB with 2MB section
;* size for initial 4GB memory and 1GB section size for memory after 4GB.
;* The overview of translation table memory attributes is described below.
;*
;*|                       | Memory Range                | Definition in Translation Table   |
;*|-----------------------|-----------------------------|-----------------------------------|
;*| DDR                   | 0x0000000000 - 0x007FFFFFFF | Normal write-back Cacheable       |
;*| PL                    | 0x0080000000 - 0x00BFFFFFFF | Strongly Ordered                  |
;*| QSPI, lower PCIe      | 0x00C0000000 - 0x00EFFFFFFF | Strongly Ordered                  |
;*| Reserved              | 0x00F0000000 - 0x00F7FFFFFF | Unassigned                        |
;*| STM Coresight         | 0x00F8000000 - 0x00F8FFFFFF | Strongly Ordered                  |
;*| GIC                   | 0x00F9000000 - 0x00F91FFFFF | Strongly Ordered                  |
;*| Reserved              | 0x00F9200000 - 0x00FCFFFFFF | Unassigned			     |
;*| FPS, LPS slaves       | 0x00FD000000 - 0x00FFBFFFFF | Strongly Ordered                  |
;*| CSU, PMU              | 0x00FFC00000 - 0x00FFDFFFFF | Strongly Ordered                  |
;*| TCM, OCM              | 0x00FFE00000 - 0x00FFFFFFFF | Normal inner write-back cacheable |
;*| Reserved              | 0x0100000000 - 0x03FFFFFFFF | Unassigned                        |
;*| PL, PCIe              | 0x0400000000 - 0x07FFFFFFFF | Strongly Ordered                  |
;*| DDR                   | 0x0800000000 - 0x0FFFFFFFFF | Normal inner write-back cacheable |
;*| PL, PCIe              | 0x1000000000 - 0xBFFFFFFFFF | Strongly Ordered                  |
;*| Reserved              | 0xC000000000 - 0xFFFFFFFFFF | Unassigned                        |
;*
;* @note
;*
;* For DDR region 0x0000000000 - 0x007FFFFFFF, a system where DDR is less than
;* 2GB, region after DDR and before PL is marked as undefined/reserved in
;* translation table. Region 0xF9100000 - 0xF91FFFFF is reserved memory in
;* 0x00F9000000 - 0x00F91FFFFF range, but it is marked as strongly ordered
;* because minimum section size in translation table section is 2MB. Region
;* 0x00FFC00000 - 0x00FFDFFFFF contains CSU and PMU memory which are marked as
;* Device since it is less than 1MB and falls in a region with device memory.
;*
;* <pre>
;* MODIFICATION HISTORY:
;*
;* Ver   Who  Date     Changes
;* ----- ---- -------- ---------------------------------------------------
;* 7.0   cjp  02/26/19 First release
;*
;******************************************************************************/

#include "xparameters.h"
#include "bspconfig.h"

	EXPORT MMUTableL0
	EXPORT MMUTableL1
	EXPORT MMUTableL2

   GBLA abscnt
   GBLA count
   GBLA sect

; Fault
Reserved  EQU 0
#if (EL1_NONSECURE == 1)
Memory    EQU 0x405:OR:(2:SHL:8):OR:0x0			; Normal writeback write allocate outer shared read write
#else
Memory    EQU 0x405:OR:(3:SHL:8):OR:0x0			; Normal writeback write allocate inner shared read write
#endif
Device    EQU 0x409:OR:(1:SHL:53):OR:(1:SHL:54):OR:0x0	; Strongly ordered read write non executable

	AREA |.mmu_tbl0|, CODE, ALIGN=12

MMUTableL0

   DCQU	MMUTableL1+0x3					; 0x0000_0000 -  0x7F_FFFF_FFFF
   DCQU	MMUTableL1+0x1000+0x3				; 0x80_0000_0000 - 0xFF_FFFF_FFFF

	AREA |.mmu_tbl1|, CODE, ALIGN=12

MMUTableL1

;
; 0x4000_0000 - 0xFFFF_FFFF
; 1GB DDR, 1GB PL, 2GB other devices n memory
;
count SETA 0
   WHILE count<0x4
   DCQU	MMUTableL2+count*0x1000+0x3
count SETA count+1
   WEND

Fixlocl1 EQU 0x100000000
abscnt SETA 0

;
; 0x0001_0000_0000 - 0x0003_FFFF_FFFF
; 12GB Reserved
;
count SETA 0
   WHILE count<0xc
   DCQU	Fixlocl1+abscnt*0x40000000+Reserved
count  SETA count+1
abscnt SETA abscnt+1
   WEND

;
; 0x0004_0000_0000 - 0x0007_FFFF_FFFF
; 8GB PL, 8GB PCIe
;
count SETA 0
   WHILE count<0x10
   DCQU	Fixlocl1+abscnt*0x40000000+Device
count  SETA count+1
abscnt SETA abscnt+1
   WEND

#ifdef XPAR_PSU_DDR_1_S_AXI_BASEADDR
DDR_1_START EQU XPAR_PSU_DDR_1_S_AXI_BASEADDR
DDR_1_END   EQU XPAR_PSU_DDR_1_S_AXI_HIGHADDR
DDR_1_SIZE  EQU (DDR_1_END - DDR_1_START + 1)
#if (DDR_1_SIZE > 0x800000000)
DDR_1_REG   EQU 0x20					; If DDR size is larger than 32GB, truncate to 32GB
#else
DDR_1_REG   EQU DDR_1_SIZE / 0x40000000
#endif
#else
DDR_1_REG   EQU 0
#endif

UNDEF_1_REG EQU (0x20 - DDR_1_REG)

; DDR based on size in hdf
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

;
; 0x0010_0000_0000 - 0x007F_FFFF_FFFF
; 448 GB PL
;
count SETA 0
   WHILE count<0x1C0
   DCQU Fixlocl1 + abscnt * 0x40000000 + Device
count  SETA count+1
abscnt SETA abscnt+1
   WEND

;
; 0x0080_0000_0000 - 0x00BF_FFFF_FFFF
; 256GB PCIe
;
count SETA 0
   WHILE count<0x100
   DCQU	Fixlocl1+abscnt*0x40000000+Device
count  SETA count+1
abscnt SETA abscnt+1
   WEND

;
; 0x00C0_0000_0000 - 0x00FF_FFFF_FFFF
; 256GB Reserved
;
count SETA 0
   WHILE count<0x100
   DCQU	Fixlocl1+abscnt*0x40000000+Reserved
count  SETA count+1
abscnt SETA abscnt+1
   WEND

	AREA |.mmu_tbl2|, CODE, ALIGN=12

MMUTableL2

abscnt SETA 0

#ifdef XPAR_PSU_DDR_0_S_AXI_BASEADDR
DDR_0_START EQU XPAR_PSU_DDR_0_S_AXI_BASEADDR
DDR_0_END   EQU XPAR_PSU_DDR_0_S_AXI_HIGHADDR
DDR_0_SIZE  EQU (DDR_0_END - DDR_0_START + 1)
#if (DDR_0_SIZE > 0x80000000)
DDR_0_REG   EQU 0x400					; If DDR size is larger than 2GB, truncate to 2GB
#else
DDR_0_REG   EQU DDR_0_SIZE / 0x200000
#endif
#else
DDR_0_REG   EQU 0
#endif

UNDEF_0_REG EQU (0x400 - DDR_0_REG)

; DDR based on size in hdf
count  SETA 0
   WHILE count<DDR_0_REG
   DCQU	abscnt*0x200000+Memory
count  SETA count+1
abscnt SETA abscnt+1
   WEND

; Reserved for region where ddr is absent
count  SETA 0
   WHILE count<UNDEF_0_REG
   DCQU	abscnt*0x200000+Reserved
count  SETA count+1
abscnt SETA abscnt+1
   WEND

;
; 0x8000_0000 - 0xBFFF_FFFF
; 1GB lower PL
;
count  SETA 0
   WHILE count<0x0200
   DCQU	abscnt*0x200000+Device
count  SETA count+1
abscnt SETA abscnt+1
   WEND

;
; 0xC000_0000 - 0xDFFF_FFFF
; 512MB QSPI
;
count  SETA 0
   WHILE count<0x0100
   DCQU	abscnt*0x200000+Device
count  SETA count+1
abscnt SETA abscnt+1
   WEND

;
; 0xE000_0000 - 0xEFFF_FFFF
; 256MB lower PCIe
;
count  SETA 0
   WHILE count<0x080
   DCQU	abscnt*0x200000+Device
count  SETA count+1
abscnt SETA abscnt+1
   WEND
;
; 0xF000_0000 - 0xF7FF_FFFF
; 128MB Reserved
;
count  SETA 0
   WHILE count<0x040
   DCQU	abscnt*0x200000+Reserved
count  SETA count+1
abscnt SETA abscnt+1
   WEND

;
; 0xF800_0000 - 0xF8FF_FFFF
; 16MB Coresight
;
count  SETA 0
   WHILE count<0x8
   DCQU	abscnt*0x200000+Device
count  SETA count+1
abscnt SETA abscnt+1
   WEND

;
; 1MB RPU LLP is marked for 2MB region as the minimum block size in translation
; table is 2MB and adjacent 63MB reserved region is converted to 62MB
;

;
; 0xF900_0000 - 0xF91F_FFFF
; 2MB RPU low latency port
;
count  SETA 0
   WHILE count<0x1
   DCQU	abscnt*0x200000+Device
count  SETA count+1
abscnt SETA abscnt+1
   WEND

;
; 0xF920_0000 - 0xFCFF_FFFF
; 62MB Reserved
;
count  SETA 0
   WHILE count<0x1f
   DCQU	abscnt*0x200000+Reserved
count  SETA count+1
abscnt SETA abscnt+1
   WEND

;
; 0xFD00_0000 - 0xFDFF_FFFF
; 16MB FPS
;
count  SETA 0
   WHILE count<0x8
   DCQU	abscnt*0x200000+Device
count  SETA count+1
abscnt SETA abscnt+1
   WEND

;
; 0xFE00_0000 -  0xFFBF_FFFF
; 28MB LPS
;
count  SETA 0
   WHILE count<0xE
   DCQU	abscnt*0x200000+Device
count  SETA count+1
abscnt SETA abscnt+1
   WEND

;
; 0xFFC0_0000 - 0xFFDF_FFFF
; 2MB PMU/CSU
;
   DCQU	abscnt*0x200000+Device

abscnt SETA abscnt+1

;
; 0xFFE0_0000 - 0xFFFF_FFFF
; 2MB OCM/TCM
;
   DCQU	abscnt*0x200000+Memory

    END

;
; @} End of "addtogroup a53_64_boot_code"
;
