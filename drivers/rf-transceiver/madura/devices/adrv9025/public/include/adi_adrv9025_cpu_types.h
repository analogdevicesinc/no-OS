/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_adrv9025_cpu_types.h
* \brief Contains ADRV9025 data types for on board cpus feature
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV9025_CPU_TYPES_H_
#define _ADI_ADRV9025_CPU_TYPES_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#endif
#include "../../private/include/adrv9025_cpu_macros.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum adi_adrv9025_CpuType
{
    ADI_ADRV9025_CPU_TYPE_UNKNOWN = -1,
    ADI_ADRV9025_CPU_TYPE_C,
    ADI_ADRV9025_CPU_TYPE_D,
    ADI_ADRV9025_CPU_TYPE_MAX
} adi_adrv9025_CpuType_e;

/**
* \brief Enumerated list of FW image build options.
*/
typedef enum adi_adrv9025_CpuFwBuildType
{
    ADI_ADRV9025_CPU_FW_BUILD_RELEASE   = 0,
    ADI_ADRV9025_CPU_FW_BUILD_DEBUG     = 1,
    ADI_ADRV9025_CPU_FW_BUILD_TESTOBJ   = 2
} adi_adrv9025_CpuFwBuildType_e;

/**
* \brief Data structure to FW version information
*/
typedef struct adi_adrv9025_CpuFwVersion
{
    uint8_t                       majorVer;
    uint8_t                       minorVer;
    uint8_t                       maintVer;
    uint16_t                      rcVer;
    adi_adrv9025_CpuFwBuildType_e cpuFwBuildType;
} adi_adrv9025_CpuFwVersion_t;

/* structure to hold memory map of a cpu */
typedef struct adi_adrv9025_CpuAddr
{
    uint8_t enabled;
    /* subsystem registers */
    uint16_t ctlAddr;      /* control register */
    uint16_t bootAddr;     /* boot address */
    uint16_t stackPtrAddr; /* stack pointer */

    /* Mailbox registers */
    uint16_t cmdAddr;       /* command. b7 = command_busy. b5:0 command */
    uint16_t extCmdAddr;    /* command payload. 4 bytes */
    uint16_t cmdStatusAddr; /* command status. 16 bytes */

    /* Program section */
    uint32_t progStartAddr;      /* start address of program memory section */
    uint32_t progEndAddr;        /* end address of program memory section */
    uint32_t versionAddr;        /* 48-bytes set aside for FW Rev Info address */
    uint32_t structChecksumAddr; /* checksum structure address */
    uint32_t debugPointersAddr;  /* Debug Pointers */
    uint32_t buildChecksumAddr;  /* FW build time checksum address */

    /* Data section */
    uint32_t dataStartAddr;      /* start address of data memory section */
    uint32_t dataEndAddr;        /* end address of data memory section */
    uint32_t cfrPulseAddr;       /*  */
    uint32_t mailboxSetAddr;     /* Mailbox SET command buffer */
    uint32_t mailboxGetAddr;     /* Mailbox GET command buffer */
    uint32_t mailboxRunInitAddr; /* Mailbox RUN_INIT command buffer */
    uint32_t mailboxFhmAddr;     /* Mailbox Frequency Hopping Mode */
} adi_adrv9025_CpuAddr_t;

typedef struct adi_adrv9025_Cpu
{
    uint32_t               devProfileAddr; /* Device Profile structure address */
    uint32_t               adcProfileAddr; /* ADC Profile structure address */
    adi_adrv9025_CpuAddr_t cpuAddr[ADRV9025_MAX_CPUS];
} adi_adrv9025_Cpu_t;

/**
* \brief Data structure to hold ARM image build and runtime calculated checksums.
*/
typedef struct adi_adrv9025_CpuChecksum
{
    uint32_t buildChecksum;
    uint32_t runChecksum;
} adi_adrv9025_CpuChecksum_t;

/**
* \brief Data structure to hold a table of additional ARM checksums
*/
typedef struct adi_adrv9025_CpuChecksumTable
{
    adi_adrv9025_CpuChecksum_t fwCheckSums;
    adi_adrv9025_CpuChecksum_t streamsCheckSum[ADRV9025_STREAM_MAX];
    adi_adrv9025_CpuChecksum_t deviceProfileCheckSum;
    adi_adrv9025_CpuChecksum_t adcProfilefwCheckSum;
    uint32_t                   fwError;
    uint32_t                   fwErrorCode;
} adi_adrv9025_CpuChecksumTable_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_CPU_TYPES_H_ */
