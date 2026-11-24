/**
 * \file
 * \brief Contains top level fpga910x related function prototypes for adi_fpga910x.c
 *
 * Copyright (c) 2015 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_H_
#define _ADI_FPGA910X_H_

#include "adi_fpga910x_types.h"
#include "adi_fpga910x_clock_types.h"
#include "adi_fpga910x_ssi_types.h"
#include "axi_sysid.h"
#include "device_profile_top_t.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Get the version of the FPGA binary
 *
 * \param[in]  fpga910x     Context variable - Pointer to the FPGA910X device data structure
 * \param[out] fpgaVersion  The current version
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_VersionGet(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_Version_t *fpgaVersion);

/**
 * \brief Initialize FPGA
 *
 * \param[in] fpga910x                    Context variable - Pointer to the FPGA910X device data structure
 * \param[in] profile                     System profile config
 * \param[in] ssiCalibration              SSI delay values to set
 * \param[in] adrv910xDeviceClockDivisor  Divisor for device clock
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_Initialize(adi_fpga910x_Device_t *fpga910x,
                                deviceProfile_t *profile,
                                adi_fpga910x_SsiCalibrationCfg_t *ssiCalibration,
                                adi_fpga910x_Mmcm_ClockOutput_Divisor_e adrv910xDeviceClockDivisor);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_FPGA910X_H_ */
