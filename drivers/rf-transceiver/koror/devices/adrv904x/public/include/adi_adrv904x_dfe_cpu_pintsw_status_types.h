/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2023 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_cpu_pintsw_status_types.h
 *
 * \brief   Contains ADRV904X DFE CPU Pin Interrupt types for PINTSW0 and PINTSW1.
 *
 * ADRV904X API Version: 2.10.0.4
 */


#ifndef __ADI_ADRV904X_DFE_CPU_PINTSW_STATUS_TYPES_H__
#define __ADI_ADRV904X_DFE_CPU_PINTSW_STATUS_TYPES_H__

#define ADI_ADRV904X_DFE_CPU_PINTSW_GPINT_MAX            (2U)
#define ADI_ADRV904X_DFE_CPU_PINTSW_SOURCES_PER_GPINT    (8U)

/**
 * \brief Enumerated list of PINT SW0 aggregated interrupts.
 */
typedef enum
{
    ADI_ADRV904X_DFE_CPU_PINTSW0_STATUS_INT_NONE             = 0x00U,
    ADI_ADRV904X_DFE_CPU_PINTSW0_STATUS_INT_0_FATAL_ERROR    = 0x01U,
    ADI_ADRV904X_DFE_CPU_PINTSW0_STATUS_INT_0_CRITICAL_ERROR = 0x02U,
} adi_adrv904x_DfeCpuPintSw0Status_e;

/**
 * \brief Enumerated list of PINT SW1 aggregated interrupts.
 */
typedef enum
{
    ADI_ADRV904X_DFE_CPU_PINTSW1_STATUS_INT_NONE                              = 0x00U,
    ADI_ADRV904X_DFE_CPU_PINTSW1_STATUS_INT_0_TDD_LUT_UPDATE_IN_PROGRESS_TXON = 0x01U,
    ADI_ADRV904X_DFE_CPU_PINTSW1_STATUS_INT_1_VSWR_ALARM                      = 0x02U,
} adi_adrv904x_DfeCpuPintSw1Status_e;


#endif /* __ADI_ADRV904X_DFE_CPU_PINTSW_STATUS_TYPES_H__ */


