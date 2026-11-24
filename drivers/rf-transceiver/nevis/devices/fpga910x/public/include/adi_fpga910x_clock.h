/**
 * \file
 * \brief Contains top level fpga910x MMCM related function prototypes. Refer to
 *        https://www.xilinx.com/support/documentation/application_notes/xapp888_7Series_DynamicRecon.pdf for more info
 *        on MMCM DRP registers.
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_CLOCK_H_
#define _ADI_FPGA910X_CLOCK_H_

#include "adi_fpga910x_clock_types.h"
#include "adi_fpga910x_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Get the lock status and clock frequencies of the FPGA910X
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[out] status    The current clock status
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_clock_Status_Get(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_ClockStatus_t *status);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_FPGA910X_CLOCK_H_ */
