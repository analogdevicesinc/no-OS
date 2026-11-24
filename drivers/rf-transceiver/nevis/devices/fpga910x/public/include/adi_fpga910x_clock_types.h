/**
 * \file
 * \brief FPGA910X MMCM types
 *
 * Copyright (c) 2019 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_CLOCK_TYPES_H_
#define _ADI_FPGA910X_CLOCK_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

/**
 * \brief Device clock output divisor
 */
typedef enum adi_fpga910x_Mmcm_ClockOutput_Divisor
{
    ADI_FPGA910X_MMCM_CLKDIV_BYPASS = 0,
    ADI_FPGA910X_MMCM_CLKDIV_2 = 1,
    ADI_FPGA910X_MMCM_CLKDIV_4 = 2,
    ADI_FPGA910X_MMCM_CLKDIV_8 = 3,
    ADI_FPGA910X_MMCM_CLKDIV_16 = 4,
    ADI_FPGA910X_MMCM_CLKDIV_32 = 5,
    ADI_FPGA910X_MMCM_CLKDIV_64 = 6,
    ADI_FPGA910X_MMCM_CLKDIV_DISABLED = 7 /*!< Arbitrary value, just to select in case to disable device clock output */
} adi_fpga910x_Mmcm_ClockOutput_Divisor_e;

/**
 * \brief Data structure to hold FPGA910X device MMCM configuration settings
 */
typedef struct adi_fpga910x_ClockStatus
{
    bool locked;                        /*!< Whether clocks are locked */
    uint32_t mcsClock_kHz;              /*!< The programmable period of MCS pulses will be dictated by this clock */
    uint32_t observationGpiosClock_kHz; /*!< Target clock is the frequency at which gainTable updates the gainIndex */
    uint32_t tddClock_kHz;              /*!< TDD clock frequency to support TDD mode */
    uint32_t refClock_kHz;              /*!< Reference clock */
} adi_fpga910x_ClockStatus_t;

#endif /* _ADI_FPGA910X_CLOCK_TYPES_H_ */
