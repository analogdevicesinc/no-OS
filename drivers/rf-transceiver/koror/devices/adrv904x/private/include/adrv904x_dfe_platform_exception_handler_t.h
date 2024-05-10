/**
 * Copyright 2021 Analog Devices Inc.
 * Released under the ADRV904X Software license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file	platform_exception_handler_t.h
 * \brief   This file contains the platform specific definitions for the DFE
 *          exception handler.
 *
 * DFE SDK API Version: 2.10.0.4
 */
#include "adi_adrv904x_platform_pack.h"

#ifndef __ADRV904X_DFE_PLATFORM_EXCEPTION_HANDLER_T_H__
#define __ADRV904X_DFE_PLATFORM_EXCEPTION_HANDLER_T_H__

#define ADRV904X_DFE_PLATFORM_NUM_CORES    (4u)

typedef enum adrv904x_DfePlatformExceptionFlag
{
    ADRV904X_DFE_PLATFORM_NO_EXCEPTION        = 0u,           /*!< 0x00000000 - CPU running normally */
    ADRV904X_DFE_PLATFORM_EXCEPTION_ENTERED   = 0x11111111u,  /*!< 0x11111111 - Exception handler entered, no snapshot data  */
    ADRV904X_DFE_PLATFORM_EXCEPTION_COMPLETED = 0x33333333u   /*!< 0x33333333 - Exception handler complete, valid snapshot data */
} adrv904x_DfePlatformExceptionFlag_e;

/**
 * \brief adrv904x_DfePlatformExceptionFlag_e size translation
 */
typedef uint32_t adrv904x_DfePlatformExceptionFlag_t;

typedef enum adrv904x_DfePlatformExceptionEntry
{
    ADRV904X_DFE_PLATFORM_EXCEPTION_ENTRY_NONE   = 0u,        /*!< 0x00000000 - CPU running normally */
    ADRV904X_DFE_PLATFORM_EXCEPTION_ENTRY_SYNC   = 0x1u,      /*!< 0x00000001 - Exception handler entered from the "syncFirstLevelHandler"  */
    ADRV904X_DFE_PLATFORM_EXCEPTION_ENTRY_NMI    = 0x2u,      /*!< 0x00000002 - Exception handler entered from the "NMI Handler" */
    ADRV904X_DFE_PLATFORM_EXCEPTION_ENTRY_ASSERT = 0x4u       /*!< 0x00000004 - Exception handler entered from the assertion */
}adrv904x_DfePlatformExceptionEntry_e;

/**
 * \brief adrv904x_DfePlatformExceptionEntry_t size translation
 */
typedef uint8_t adrv904x_DfePlatformExceptionEntry_t;

/* System registers which are collected from the demo code
 * https://git.trustedfirmware.org/TF-A/trusted-firmware-a.git/tree/bl31/aarch64/crash_reporting.S#n334
 * All the definitions can be found in the ARM Architecture Reference Manual for ARMv8
 */

/* The definition of A55 CPU exception register snapshot data */
ADI_ADRV904X_PACK_START
typedef struct
{
    uint64_t sp_el1;            /*!< Stack pointer of EL1 */
    uint64_t spsr_el1;          /*!< Saved Program Status Register of EL1 */
    uint64_t elr_el1;           /*!< Exception Link Register */
    uint64_t sctlr_el1;         /*!< System Control Registers of EL1 */
    uint64_t actlr_el1;         /*!< Auxiliary Control Register of EL1 */
    uint64_t cpacr_el1;         /*!< Architectural Feature Access Control Register of EL1 */
    uint64_t csselr_el1;        /*!< Cache Size Selection Register of EL1 */
    uint64_t esr_el1;           /*!< Exception Syndrome Register of EL1ss */
    uint64_t ttbr0_el1;         /*!< Translation Table Base Register 0 of EL1 */
    uint64_t ttbr1_el1;         /*!< Translation Table Base Register 1 of EL1 */
    uint64_t mair_el1;          /*!< Memory Attribute Indirection Register */
    uint64_t amair_el1;         /*!< Auxiliary Memory Attribute Indirection Register */
    uint64_t tcr_el1;           /*!< Translation Control Register */
    uint64_t tpidr_el1;         /*!< EL1 Software Thread ID Register */
    uint64_t par_el1;           /*!< Physical Address Register of EL1 */
    uint64_t mpidr_el1;         /*!< Multiprocessor Affinity Register of EL1 */
    uint64_t afsr0_el1;         /*!< Auxiliary Fault Status Register 0 of EL1 */
    uint64_t afsr1_el1;         /*!< Auxiliary Fault Status Register 1 of EL1 */
    uint64_t contextidr_el1;    /*!< Context ID Register of EL1 */
    uint64_t vbar_el1;          /*!< Vector Base Address Register of EL1 */
    uint64_t cntkctl_el1;       /*!< Counter-timer Kernel Control Register of EL1 */
    uint64_t isr_el1;           /*!< Interrupt Status Register of EL1 */
    uint64_t far_el1;           /*!< Fault Address Register of EL1 */

    /*
     * Some registers from GIC which are collected from the GICv3_gicc.h file and all the descriptions
     * can be found in the Arm® Generic Interrupt Controller Architecture Specification
     * GIC architecture version 3 and version 4
     */
    uint64_t icc_sre_el1;       /* Interrupt Controller System Register Enable register of EL1 */
    uint64_t icc_igrpen0_el1;   /* Interrupt Controller Interrupt Group 0 Enable register of EL1 */
    uint64_t icc_igrpen1_el1;   /* Interrupt Controller Interrupt Group 1 Enable register of EL1 */
    uint64_t icc_ctlr_el1;      /* Interrupt Controller Control Register of EL1 */
    uint64_t icc_iar0_el1;      /* Interrupt Controller Interrupt Acknowledge Register 0 of EL1 */
    uint64_t icc_iar1_el1;      /* Interrupt Controller Interrupt Acknowledge Register 1 of EL1 */
    uint64_t icc_pmr_el1;       /* Interrupt Controller Interrupt Priority Mask Register of EL1 */
    uint64_t icc_bpr0_el1;      /* Interrupt Controller Virtual Binary Point Register 0 of EL1 */
    uint64_t icc_bpr1_el1;      /* Interrupt Controller Virtual Binary Point Register 1 of EL1 */
    uint64_t icc_rpr_el1;       /* Interrupt Controller Virtual Running Priority Register of EL1 */
} adrv904x_DfePlatformExceptionRegs_t;
ADI_ADRV904X_PACK_FINISH

/* The definition of paltform specific CPU exception data */
    ADI_ADRV904X_PACK_START
typedef struct
{
    adrv904x_DfePlatformExceptionFlag_e  ExceptionFlag[ADRV904X_DFE_PLATFORM_NUM_CORES];
    adrv904x_DfePlatformExceptionRegs_t  ExceptionRegs[ADRV904X_DFE_PLATFORM_NUM_CORES];
    adrv904x_DfePlatformExceptionEntry_e ExceptionEntry[ADRV904X_DFE_PLATFORM_NUM_CORES];
    uint64_t                       ExceptionSyncTimeStampUs[ADRV904X_DFE_PLATFORM_NUM_CORES];
    uint64_t                       ExceptionNmiTimeStampUs[ADRV904X_DFE_PLATFORM_NUM_CORES];
} adrv904x_DfePlatformExceptionData_t;
ADI_ADRV904X_PACK_FINISH

#endif /* __ADRV904X_DFE_PLATFORM_EXCEPTION_HANDLER_T_H__*/


