/**
 * \file
 * \brief FPGA910X Multi-Chip Synchronization (MCS) functions
 *
 * Copyright (c) 2019 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_MCS_H_
#define _ADI_FPGA910X_MCS_H_

#include "adi_fpga910x_mcs_types.h"
#include "adi_adrv910x_common_types.h"
#include "adi_fpga910x_types.h"
#include "stdbool.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Configure MCS
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] mcsCfg    The desired MCS configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_Mcs_Configure(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_McsCfg_t *mcsCfg);

/**
 * \brief Configure MCS
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_Mcs_SsiConfigure(adi_fpga910x_Device_t *fpga910x);

/**
 * \brief Initiate a sequence of MCS pulses
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_Mcs_Start(adi_fpga910x_Device_t *fpga910x);

/**
 * \brief Get Rx MCS To strobe latency in samples
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  channel   Channel to get MCS to strobe latency
 * \param[out] latency   MCS to strobe latency in samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_Mcs_RxMcsToStrobeSampleLatency_Get(adi_fpga910x_Device_t *fpga910x,
                                                        adi_common_ChannelNumber_e channel,
                                                        uint32_t *latency);

/**
 * \brief Get MCS done status
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[out] mcsDone   MCS done status. Set to 1 if the MCS sequence is complete, 0 otherwise.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_Mcs_Done_Get(adi_fpga910x_Device_t *fpga910x, bool *mcsDone);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_FPGA910X_MCS_H_ */
