/**
 * \file
 * \brief Contains top level fpga910x related function prototypes for TDD specific features
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_TDD_H_
#define _ADI_FPGA910X_TDD_H_

#include "adi_fpga910x_tdd_types.h"
#include "adi_fpga910x_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Control TDD enables and assertion modes
 *
 * This function provides software control of TDD (asserted low, high, or hardware modes).
 *
 * \pre The ADRV910X must be configured for pin enable mode by calling adi_adrv910x_Radio_ChannelEnableMode_Set()
 *
 * \param[in] fpga910x   Context variable - Pointer to the FPGA910X device data structure
 * \param[in] tddSelect  TDD enable to configure.
 * \param[in] mode       The desired mode of the pin
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_Tdd_Enable_Set(adi_fpga910x_Device_t *fpga910x,
                                    adi_fpga910x_TddSelect_e tddSelect,
                                    adi_fpga910x_TddMode_e mode);

/**
 * \brief Read back the TDD assertion modes
 *
 * \param[in]  fpga910x   Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  tddSelect  TDD enable for which to read assertion mode
 * \param[out] mode       The desired mode of the pin
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_Tdd_Enable_Get(adi_fpga910x_Device_t *fpga910x,
                                    adi_fpga910x_TddSelect_e tddSelect,
                                    adi_fpga910x_TddMode_e *mode);

/**
 * \brief Configure the FPGA TDD for programmed mode
 *
 * \param[in] fpga910x   Context variable - Pointer to the FPGA910X device data structure
 * \param[in] tddConfig  The desired TDD programmed mode configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_Tdd_Configure(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_TddConfig_t *tddConfig);

/**
 * \brief Inspect the FPGA TDD programmed mode configuration
 *
 * \param[in]  fpga910x   Context variable - Pointer to the FPGA910X device data structure
 * \param[out] tddConfig  The current TDD programmed mode configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_Tdd_Inspect(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_TddConfig_t *tddConfig);

/**
 * \brief Start the TDD state machine based on the trigger specified in the configuration
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_Tdd_Start(adi_fpga910x_Device_t *fpga910x);

/**
 * \brief Stop the TDD state machine, bringing all enable pins low at the end of the current frame
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_Tdd_Stop(adi_fpga910x_Device_t *fpga910x);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_FPGA910X_TDD_H_ */
