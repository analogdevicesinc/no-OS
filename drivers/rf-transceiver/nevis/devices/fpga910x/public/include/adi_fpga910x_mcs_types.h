/**
 * \file
 * \brief FPGA910X MCS types
 *
 * Copyright (c) 2019 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_MCS_TYPES_H_
#define _ADI_FPGA910X_MCS_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

/**
 * \brief MCS edge alignment with device clock
 */
typedef enum adi_fpga910x_McsEdge
{
    ADI_FPGA910X_MCS_EDGE_RISING = 0,   /*!< Align MCS assert/deassert to rising edge of device clock */
    ADI_FPGA910X_MCS_EDGE_FALLING       /*!< Align MCS assert/deassert to falling edge of device clock */
} adi_fpga910x_McsEdge_e;

/**
 * \brief The desired configuration to initiate a sequence of MCS pulses
 */
typedef struct adi_fpga910x_McsCfg
{
    uint8_t numberOfPulses;      /*!< Number of pulses to generate(valid values : 1 - 16) */
    uint32_t mcsPeriod;          /*!< Number of device clock cycles between consecutive MCS pulses */
    uint32_t mcsPulseWidth;      /*!< Width of the pulse, denoted in number of device clock cycles */
    adi_fpga910x_McsEdge_e edge; /*!< The device clock edge to which to align the pulse */
} adi_fpga910x_McsCfg_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_FPGA910X_MCS_TYPES_H_ */
