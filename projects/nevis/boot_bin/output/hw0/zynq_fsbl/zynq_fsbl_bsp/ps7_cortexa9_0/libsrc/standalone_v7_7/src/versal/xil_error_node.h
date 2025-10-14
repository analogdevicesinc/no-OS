/******************************************************************************
* Copyright (c) 2021 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_error_node.h
*
* This is the file which contains node IDs information for error events.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who 	 Date        Changes
* ----- -------- -------- -----------------------------------------------------
* 7.7   bsv      12/22/2021 Initial release
*       ma       01/17/2022 Add PLM exceptions to SW errors list
*
* </pre>
*
* @note
*
******************************************************************************/
#ifndef XIL_ERROR_NODE_H
#define XIL_ERROR_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

/************************** Constant Definitions *****************************/
/**@name Versal Event Node IDs
 * @defgroup xileventnodes Event Node IDs
 * @ingroup xilnodeids
 * @{
 */
 /**
 * Error Event Node Ids
 */
#define XIL_NODETYPE_EVENT_ERROR_PMC_ERR1	(0x28100000U)
#define XIL_NODETYPE_EVENT_ERROR_PMC_ERR2	(0x28104000U)
#define XIL_NODETYPE_EVENT_ERROR_PSM_ERR1	(0x28108000U)
#define XIL_NODETYPE_EVENT_ERROR_PSM_ERR2	(0x2810C000U)
#define XIL_NODETYPE_EVENT_ERROR_SW_ERR		(0x28110000U)
/**
 * @}
 */

/**@name Versal Error event Mask
 * @defgroup xilerroreventmask Error Event Mask
 * @ingroup xilnodeids
 * @{
 * @defgroup pmcerr1 Error Event Mask for PMC ERR1
 * @ingroup xilerroreventmask
 * @{
 * @brief Error Events belong to PMC ERR1 Node
 */

/** Error event mask for PMC Boot Correctable Error.
 * Set by ROM code during ROM execution during Boot. */
#define XIL_EVENT_ERROR_MASK_BOOT_CR		(0x00000001U)

/** Error event mask for PMC Boot Non-Correctable Error.
 * Set by ROM code during ROM execution during Boot. */
#define XIL_EVENT_ERROR_MASK_BOOT_NCR		(0x00000002U)

/** Error event mask for PMC Firmware Boot Correctable Error.
 * Set by PLM during firmware execution during Boot. */
#define XIL_EVENT_ERROR_MASK_FW_CR		(0x00000004U)

/** Error event mask for PMC Firmware Boot Non-Correctable Error.
 * Set by PLM during firmware execution during Boot. */
#define XIL_EVENT_ERROR_MASK_FW_NCR		(0x00000008U)

/** Error event mask for General Software Correctable Error.
 * Set by any processors after Boot. */
#define XIL_EVENT_ERROR_MASK_GSW_CR		(0x00000010U)

/** Error event mask for General Software Non-Correctable Error.
 * Set by any processors after Boot. */
#define XIL_EVENT_ERROR_MASK_GSW_NCR		(0x00000020U)

/** Error event mask for CFU Error. */
#define XIL_EVENT_ERROR_MASK_CFU		(0x00000040U)

/** Error event mask for CFRAME Error. */
#define XIL_EVENT_ERROR_MASK_CFRAME		(0x00000080U)

/** Error event mask for PSM Correctable Error,
 * Summary from PSM Error Management. */
#define XIL_EVENT_ERROR_MASK_PMC_PSM_CR		(0x00000100U)

/** Error event mask for PSM Non-Correctable Error,
 * Summary from PSM Error Management. */
#define XIL_EVENT_ERROR_MASK_PMC_PSM_NCR	(0x00000200U)

/** Error event mask for DDRMC MB Correctable ECC Error. */
#define XIL_EVENT_ERROR_MASK_DDRMB_CR		(0x00000400U)

/** Error event mask for DDRMC MB Non-Correctable ECC Error. */
#define XIL_EVENT_ERROR_MASK_DDRMB_NCR		(0x00000800U)

/** Error event mask for NoC Type1 Correctable Error. */
#define XIL_EVENT_ERROR_MASK_NOCTYPE1_CR	(0x00001000U)

/** Error event mask for NoC Type1 Non-Correctable Error. */
#define XIL_EVENT_ERROR_MASK_NOCTYPE1_NCR	(0x00002000U)

/** Error event mask for NoC User Error. */
#define XIL_EVENT_ERROR_MASK_NOCUSER		(0x00004000U)

/** Error event mask for MMCM Lock Error. */
#define XIL_EVENT_ERROR_MASK_MMCM		(0x00008000U)

/** Error event mask for ME Correctable Error. */
#define XIL_EVENT_ERROR_MASK_AIE_CR		(0x00010000U)

/** Error event mask for ME Non-Correctable Error. */
#define XIL_EVENT_ERROR_MASK_AIE_NCR		(0x00020000U)

/** Error event mask for DDRMC MC Correctable ECC Error. */
#define XIL_EVENT_ERROR_MASK_DDRMC_CR		(0x00040000U)

/** Error event mask for DDRMC MC Non-Correctable ECC Error. */
#define XIL_EVENT_ERROR_MASK_DDRMC_NCR		(0x00080000U)

/** Error event mask for GT Correctable Error. */
#define XIL_EVENT_ERROR_MASK_GT_CR		(0x00100000U)

/** Error event mask for GT Non-Correctable Error. */
#define XIL_EVENT_ERROR_MASK_GT_NCR		(0x00200000U)

/** Error event mask for PL Sysmon Correctable Error. */
#define XIL_EVENT_ERROR_MASK_PLSMON_CR		(0x00400000U)

/** Error event mask for PL Sysmon Non-Correctable Error. */
#define XIL_EVENT_ERROR_MASK_PLSMON_NCR		(0x00800000U)

/** Error event mask for User defined PL generic error. */
#define XIL_EVENT_ERROR_MASK_PL0		(0x01000000U)

/** Error event mask for User defined PL generic error. */
#define XIL_EVENT_ERROR_MASK_PL1		(0x02000000U)

/** Error event mask for User defined PL generic error. */
#define XIL_EVENT_ERROR_MASK_PL2		(0x04000000U)

/** Error event mask for User defined PL generic error. */
#define XIL_EVENT_ERROR_MASK_PL3		(0x08000000U)

/** Error event mask for NPI Root Error. */
#define XIL_EVENT_ERROR_MASK_NPIROOT		(0x10000000U)

/** Error event mask for SSIT Error from Slave SLR1,
 * Only used in Master SLR. */
#define XIL_EVENT_ERROR_MASK_SSIT3		(0x20000000U)

/** Error event mask for SSIT Error from Slave SLR2,
 * Only used in Master SLR. */
#define XIL_EVENT_ERROR_MASK_SSIT4		(0x40000000U)

/** Error event mask for SSIT Error from Slave SLR3,
 * Only used in Master SLR. */
#define XIL_EVENT_ERROR_MASK_SSIT5		(0x80000000U)
/**
 * @}
 */

/**
 * @defgroup pmcerr2 Error Event Mask for PMC ERR2
 * @ingroup xilerroreventmask
 * @{
 * @brief Error Events belong to PMC ERR2 Node
 */
/** Error event mask for General purpose PMC error,
 * can be triggered by any of the following peripherals:,
 * - PMC Global Regsiters,- PMC Clock & Reset (CRP),- PMC IOU Secure SLCR,
 * - PMC IOU SLCR,- BBRAM Controller,- PMC Analog Control Registers,
 * - RTC Control Registers. */
#define XIL_EVENT_ERROR_MASK_PMCAPB		(0x00000001U)

/** Error event mask for PMC ROM Validation Error. */
#define XIL_EVENT_ERROR_MASK_PMCROM		(0x00000002U)

/** Error event mask for PMC PPU0 MB TMR Fatal Error. */
#define XIL_EVENT_ERROR_MASK_MB_FATAL0		(0x00000004U)

/** Error event mask for PMC PPU1 MB TMR Fatal Error. */
#define XIL_EVENT_ERROR_MASK_MB_FATAL1		(0x00000008U)

/** Error event mask for PMC Switch and PMC IOU Parity Errors. */
#define XIL_EVENT_ERROR_MASK_PMCPAR		(0x00000010U)

/** Error event mask for PMC Correctable Errors:,
 * PPU0 RAM correctable error.,PPU1 instruction RAM correctable error.,
 * PPU1 data RAM correctable error. */
#define XIL_EVENT_ERROR_MASK_PMC_CR		(0x00000020U)

/** Error event mask for PMC Non-Correctable Errors:
 * PPU0 RAM non-correctable error, PPU1 instruction RAM non-correctable error,
 * PPU1 data RAM non-correctable error, PRAM non-correctable error. */
#define XIL_EVENT_ERROR_MASK_PMC_NCR		(0x00000040U)

/** Error event mask for PMC Temperature Shutdown Alert and Power Supply
 * Failure Detection Errors from PMC Sysmon alarm[0].
 * Indicates an alarm condition on any of SUPPLY0 to SUPPLY31. */
#define XIL_EVENT_ERROR_MASK_PMCSMON0		(0x00000080U)

/** Error event mask for PMC Temperature Shutdown Alert and Power Supply
 * Failure Detection Errors from PMC Sysmon alarm[1].
 * Indicates an alarm condition on any of SUPPLY32 to SUPPLY63. */
#define XIL_EVENT_ERROR_MASK_PMCSMON1		(0x00000100U)

/** Error event mask for PMC Temperature Shutdown Alert and Power Supply
 * Failure Detection Errors from PMC Sysmon alarm[2].
 * Indicates an alarm condition on any of SUPPLY64 to SUPPLY95. */
#define XIL_EVENT_ERROR_MASK_PMCSMON2		(0x00000200U)

/** Error event mask for PMC Temperature Shutdown Alert and Power Supply
 * Failure Detection Errors from PMC Sysmon alarm[3].
 * Indicates an alarm condition on any of SUPPLY96 to SUPPLY127. */
#define XIL_EVENT_ERROR_MASK_PMCSMON3		(0x00000400U)

/** Error event mask for PMC Temperature Shutdown Alert and Power Supply
 * Failure Detection Errors from PMC Sysmon alarm[4].
 * Indicates an alarm condition on any of SUPPLY128 to SUPPLY159. */
#define XIL_EVENT_ERROR_MASK_PMCSMON4		(0x00000800U)

/** Error event mask for PMC Temperature Shutdown Alert and Power Supply
 * Failure Detection Errors from PMC Sysmon alarm[8].
 * Indicates an over-temperature alarm. */
#define XIL_EVENT_ERROR_MASK_PMCSMON8		(0x00008000U)

/** Error event mask for PMC Temperature Shutdown Alert and Power Supply
 * Failure Detection Errors from PMC Sysmon alarm[9].
 * Indicates a device temperature alarm. */
#define XIL_EVENT_ERROR_MASK_PMCSMON9		(0x00010000U)

/** Error event mask for CFI Non-Correctable Error. */
#define XIL_EVENT_ERROR_MASK_CFI		(0x00020000U)

/** Error event mask for CFRAME SEU CRC Error. */
#define XIL_EVENT_ERROR_MASK_SEUCRC		(0x00040000U)

/** Error event mask for CFRAME SEU ECC Error. */
#define XIL_EVENT_ERROR_MASK_SEUECC		(0x00080000U)

/** Error event mask for RTC Alarm Error. */
#define XIL_EVENT_ERROR_MASK_RTCALARM		(0x00400000U)

/** Error event mask for PMC NPLL Lock Error,
 * this error can be unmasked after the NPLL is locked to alert when the
 * NPLL loses lock. */
#define XIL_EVENT_ERROR_MASK_NPLL		(0x00800000U)

/** Error event mask for PMC PPLL Lock Error,
 * this error can be unmasked after the PPLL is locked to alert when the
 * PPLL loses lock. */
#define XIL_EVENT_ERROR_MASK_PPLL		(0x01000000U)

/** Error event mask for Clock Monitor Errors, collected from CRP's
 * CLKMON_STATUS register. */
#define XIL_EVENT_ERROR_MASK_CLKMON		(0x02000000U)

/** Error event mask for PMC Interconnect Timeout Errors.
 * Collected from: Interconnect mission interrupt status register,
 * Interconnect latent status register, Timeout interrupt status register
 * for SERBs. */
#define XIL_EVENT_ERROR_MASK_PMCTO		(0x04000000U)

/** Error event mask for PMC XMPU Errors: Register access error on APB,
 * Read permission violation, Write permission violation, Security violation. */
#define XIL_EVENT_ERROR_MASK_PMCXMPU		(0x08000000U)

/** Error event mask for PMC XPPU Errors: Register access error on APB,
 * Master ID not found, Read permission violation, Master ID parity error,
 * Master ID access violation, TrustZone violation, Aperture parity error. */
#define XIL_EVENT_ERROR_MASK_PMCXPPU		(0x10000000U)

/** Error event mask for For Master SLR: SSIT Error from Slave SLR1,
 * For Slave SLRs: SSIT Error0 from Master SLR. */
#define XIL_EVENT_ERROR_MASK_SSIT0		(0x20000000U)

/** Error event mask for For Master SLR: SSIT Error from Slave SLR2,
 * For Slave SLRs: SSIT Error1 from Master SLR. */
#define XIL_EVENT_ERROR_MASK_SSIT1		(0x40000000U)

/** Error event mask for For Master SLR: SSIT Error from Slave SLR3,
 * For Slave SLRs: SSIT Error2 from Master SLR. */
#define XIL_EVENT_ERROR_MASK_SSIT2		(0x80000000U)
/**
 * @}
 */

/**
 * @defgroup psmerr1 Error Event Mask for PSM ERR1
 * @ingroup xilerroreventmask
 * @{
 * @brief Error Events belong to PSM ERR1 Node
 */
/** Error event mask for PS Software can write to trigger register to
 * generate this Correctable Error. */
#define XIL_EVENT_ERROR_MASK_PS_SW_CR		(0x00000001U)

/** Error event mask for PS Software can write to trigger register to
 * generate this Non-Correctable Error. */
#define XIL_EVENT_ERROR_MASK_PS_SW_NCR		(0x00000002U)

/** Error event mask for PSM Firmware can write to trigger register to
 * generate this Correctable Error. */
#define XIL_EVENT_ERROR_MASK_PSM_B_CR		(0x00000004U)

/** Error event mask for PSM Firmware can write to trigger register to
 * generate this Non-Correctable Error. */
#define XIL_EVENT_ERROR_MASK_PSM_B_NCR		(0x00000008U)

/** Error event mask for Or of MB Fatal1, Fatal2, Fatal3 Error. */
#define XIL_EVENT_ERROR_MASK_MB_FATAL		(0x00000010U)

/** Error event mask for PSM Correctable. */
#define XIL_EVENT_ERROR_MASK_PSM_CR		(0x00000020U)

/** Error event mask for PSM Non-Correctable. */
#define XIL_EVENT_ERROR_MASK_PSM_NCR		(0x00000040U)

/** Error event mask for Non-Correctable ECC Error during an OCM access. */
#define XIL_EVENT_ERROR_MASK_OCM_ECC		(0x00000080U)

/** Error event mask for Non-Correctable ECC Error during APU L2 Cache access. */
#define XIL_EVENT_ERROR_MASK_L2_ECC		(0x00000100U)

/** Error event mask for ECC Errors during a RPU memory access.
 * Floating-point operation exceptions. RPU REG APB error. */
#define XIL_EVENT_ERROR_MASK_RPU_ECC		(0x00000200U)

/** Error event mask for RPU Lockstep Errors from R5_0.
 * The Lockstep error is not initialized until RPU clock is enabled.
 * Therefore the error outcomes are masked by default and are expected to be
 * unmasked after processor clock is enabled and before its reset is released. */
#define XIL_EVENT_ERROR_MASK_RPU_LS		(0x00000400U)

/** Error event mask for RPU Common Cause Failures ORed together.
 * The CCF Error register with the masking capability has to reside in the RPU. */
#define XIL_EVENT_ERROR_MASK_RPU_CCF		(0x00000800U)

/** Error event mask for APU GIC AXI Error by the AXI4 master port,
 * such as SLVERR or DECERR. */
#define XIL_EVENT_ERROR_MASK_GIC_AXI		(0x00001000U)

/** Error event mask for APU GIC ECC Error,
 * a Non-Correctable ECC error occurred in any ECC-protected RAM. */
#define XIL_EVENT_ERROR_MASK_GIC_ECC		(0x00002000U)

/** Error event mask for APLL Lock Errors.
 * The error can be unmasked after the PLL is locked to alert when the
 * PLL loses lock. */
#define XIL_EVENT_ERROR_MASK_APLL_LOCK		(0x00004000U)

/** Error event mask for RPLL Lock Errors.
 * The error can be unmasked after the PLL is locked to alert when the
 * PLL loses lock. */
#define XIL_EVENT_ERROR_MASK_RPLL_LOCK		(0x00008000U)

/** Error event mask for CPM Correctable Error. */
#define XIL_EVENT_ERROR_MASK_CPM_CR		(0x00010000U)

/** Error event mask for CPM Non-Correctable Error. */
#define XIL_EVENT_ERROR_MASK_CPM_NCR		(0x00020000U)

/** Error event mask for LPD APB Errors from: IPI REG, USB2 REG, CRL REG,
 * LPD AFIFM4 REG, LPD IOU REG, LPD IOU SECURE SLCR REG, LPD SLCR REG,
 * LPD SLCR SECURE REG. */
#define XIL_EVENT_ERROR_MASK_LPD_APB		(0x00040000U)

/** Error event mask for FPD APB Errors from: FPD AFIFM0 REG, FPD AFIFM2 REG,
 * FPD SLCR REG,FPD SLCR SECURE REG, CRF REG. */
#define XIL_EVENT_ERROR_MASK_FPD_APB		(0x00080000U)

/** Error event mask for Data parity errors from the interfaces connected
 * to the LPD interconnect. */
#define XIL_EVENT_ERROR_MASK_LPD_PAR		(0x00100000U)

/** Error event mask for Data parity errors from the interfaces connected
 * to the FPD interconnect. */
#define XIL_EVENT_ERROR_MASK_FPD_PAR		(0x00200000U)

/** Error event mask for LPD IO Peripheral Unit Parity Error. */
#define XIL_EVENT_ERROR_MASK_IOU_PAR		(0x00400000U)

/** Error event mask for Data parity errors from the interfaces connected
 * to the PSM interconnect. */
#define XIL_EVENT_ERROR_MASK_PSM_PAR		(0x00800000U)

/** Error event mask for LPD Interconnect Timeout errors.
 * Collected from: Timeout errors at the slaves connected to the LPD
 * interconnect, Address decode error, Interconnect mission errors for
 * the slaves connected to the LPD interconnect. */
#define XIL_EVENT_ERROR_MASK_LPD_TO		(0x01000000U)

/** Error event mask for FPD Interconnect Timeout errors.
 * Collected from: Coresight debug trace alarms, Timeout errors at the
 * slaves connected to the FPD interconnect, Address decode error,
 * Data parity errors on the interfaces connected to the FPD interconnect. */
#define XIL_EVENT_ERROR_MASK_FPD_TO		(0x02000000U)

/** Error event mask for PSM Interconnect Timeout Errors.
 * Collected from: Interconnect mission errors for PSM_LOCAL slave or
 * PSM_GLOBAL slave or MDM slave or LPD interconnect or PSM master,
 * Interconnect latent errors for PSM_LOCAL slave or PSM_GLOBAL slave or
 * MDM slave or LPD interconnect or PSM master, Timeout errors at the slaves
 * connected to the PSM interconnect. */
#define XIL_EVENT_ERROR_MASK_PSM_TO		(0x04000000U)

/** Error event mask for XRAM Correctable error.
 * Only applicable in devices that have XRAM. */
#define XIL_EVENT_ERROR_MASK_XRAM_CR		(0x08000000U)

/** Error event mask for XRAM Non-Correctable error.
 * Only applicable in devices that have XRAM. */
#define XIL_EVENT_ERROR_MASK_XRAM_NCR		(0x10000000U)
/**
 * @}
 */

/**
 * @defgroup psmerr2 Error Event Mask for PSM ERR2
 * @ingroup xilerroreventmask
 * @brief Error Events belong to PSM ERR2 Node
 * @{
 */
/** Error event mask for Error from Watchdog Timer in the LPD Subsystem. */
#define XIL_EVENT_ERROR_MASK_LPD_SWDT		(0x00000001U)

/** Error event mask for Error from Watchdog Timer in the FPD Subsystem. */
#define XIL_EVENT_ERROR_MASK_FPD_SWDT		(0x00000002U)

/** Error event mask for LPD XMPU Errors: Register access error on APB,
 * Read permission violation, Write permission violation, Security violation. */
#define XIL_EVENT_ERROR_MASK_LPD_XMPU		(0x00040000U)

/** Error event mask for LPD XPPU Errors: Register access error on APB,
 * Master ID not found, Read permission violation, Master ID parity error,
 * Master ID access violation, TrustZone violation, Aperture parity error. */
#define XIL_EVENT_ERROR_MASK_LPD_XPPU		(0x00080000U)

/** Error event mask for FPD XMPU Errors: Register access error on APB,
 * Read permission violation, Write permission violation, Security violation. */
#define XIL_EVENT_ERROR_MASK_FPD_XMPU		(0x00100000U)
/**
 * @}
 */

/**
 * @defgroup swerr Error Event Mask for Software error events
 * @ingroup xilerroreventmask
 * @{
 * @brief Error Events belong to SW ERR Node
 */
/** Error event mask for Software error events */
/** Health Boot Monitoring errors */
#define XIL_EVENT_ERROR_MASK_HB_MON_0		(0x00000001U)
#define XIL_EVENT_ERROR_MASK_HB_MON_1		(0x00000002U)
#define XIL_EVENT_ERROR_MASK_HB_MON_2		(0x00000004U)
#define XIL_EVENT_ERROR_MASK_HB_MON_3		(0x00000008U)
#define XIL_EVENT_ERROR_MASK_PLM_EXCEPTION	(0x00000010U)
/**
 * @}
 */

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /* XIL_ERROR_NODE_H */
