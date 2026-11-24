/**
 * \file
 * \brief Functions for FPGA910X Synchronous Serial Interface (SSI) configuration
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_SSI_H_
#define _ADI_FPGA910X_SSI_H_

#include "adi_fpga910x_ssi_types.h"
#include "adi_adrv910x_common_types.h"
#include "adi_adrv910x_types.h"
#include "adi_fpga910x_types.h"
#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Calibrate SSI
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port      The port that the channel refers to
 * \param[in]  channel   The channel of the specified port for which to set the SSI mode
 * \param[out] delay     The calculated delay value
 * \param[out] sweep     The observed sweep value
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_Calibrate(adi_fpga910x_Device_t *fpga910x,
                                   adi_common_Port_e port,
                                   adi_common_ChannelNumber_e channel,
                                   uint32_t *delay,
                                   uint32_t *sweep);

/**
 * \brief Set the SSI clock polarity of the selected Tx channel
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] channel   The Tx channel for which to change the SSI clock polarity
 * \param[in] inverted  For the selected channel, True:perform SSI clock inversion
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_TxClockPolarity_Set(adi_fpga910x_Device_t *fpga910x,
                                             adi_common_ChannelNumber_e channel,
                                             bool inverted);

/**
 * \brief Get the SSI clock polarity of the selected Tx channel
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  channel   The Tx channel for which to get the SSI clock polarity
 * \param[out] inverted  Get the clock inversion status for the selected channel
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_TxClockPolarity_Get(adi_fpga910x_Device_t *fpga910x,
                                             adi_common_ChannelNumber_e channel,
                                             bool *inverted);

/**
 * \brief Configure SSI
 *
 * \param[in] fpga910x          Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port              The port that the channel refers to
 * \param[in] channel           The channel of the specified port for which to set the SSI mode
 * \param[in] interfaceRate_Hz  Interface sample rate at serial interface
 * \param[in] sampleRate_Hz     Tx input data rate in Hz or Rx output data rate in Hz
 * \param[in] ssiCfg            The desried SSI configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_Configure(adi_fpga910x_Device_t *fpga910x,
                                   adi_common_Port_e port,
                                   adi_common_ChannelNumber_e channel,
                                   uint32_t interfaceRate_Hz,
                                   uint32_t sampleRate_Hz,
                                   adi_fpga910x_SsiConfig_t *ssiCfg);

/**
 * \brief Get the current SSI configuration
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port      The port that the channel refers to
 * \param[in]  channel   The channel of the specified port for which to get the SSI mode
 * \param[out] ssiCfg    The current SSI configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_Inspect(adi_fpga910x_Device_t *fpga910x,
                                 adi_common_Port_e port,
                                 adi_common_ChannelNumber_e channel,
                                 adi_fpga910x_SsiConfig_t *ssiCfg);

/**
 * \brief Get the number of bytes per sample for the specified SSI Lane
 *
 * \param[in]  fpga910x        Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port            The port that the channel refers to
 * \param[in]  channel         The channel of the specified port for which to determine the number of bytes per sample
 * \param[out] bytesPerSample  The number of bytes per sample
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_BytesPerSample_Get(adi_fpga910x_Device_t *fpga910x,
                                            adi_common_Port_e port,
                                            adi_common_ChannelNumber_e channel,
                                            uint8_t *bytesPerSample);

/**
 * \brief Reset the SSI interface
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_Reset(adi_fpga910x_Device_t *fpga910x);

/**
 * \brief Configure the SSI test mode for the specified Rx channel
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] fpga910x           Context variable - Pointer to the FPGA910X device data structure
 * \param[in] channel            The Rx channel for which to configure SSI test mode
 * \param[in] ssiTestModeConfig  The desired SSI test mode to configure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_Rx_TestMode_Configure(adi_fpga910x_Device_t *fpga910x,
                                               adi_common_ChannelNumber_e channel,
                                               adi_fpga910x_RxSsiTestModeCfg_t *ssiTestModeConfig);

/**
 * \brief Inspect the SSI test mode status for the specified Tx channel
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  fpga910x   Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  channel    The Tx channel for which to inspect SSI test mode status
 * \param[out] dataError  The current test mode status of the received data over interface against a fixed pattern
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_Rx_TestMode_Inspect(adi_fpga910x_Device_t *fpga910x,
                                             adi_common_ChannelNumber_e channel,
                                             bool *dataError);

/**
 * \brief Configure the SSI test mode for the specified Tx channel
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] fpga910x           Context variable - Pointer to the FPGA910X device data structure
 * \param[in] channel            The Tx channel for which to configure SSI test mode
 * \param[in] ssiTestModeConfig  The desired SSI test mode to configure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_Tx_TestMode_Configure(adi_fpga910x_Device_t *fpga910x,
                                               adi_common_ChannelNumber_e channel,
                                               adi_fpga910x_TxSsiTestModeCfg_t *ssiTestModeConfig);

/**
 * \brief This function programs the SSI delay configuration in FPGA910X device.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] fpga910x        Context variable - Pointer to the FPGA910X device data structure
 * \param[in] ssiType         LVDS or CMOS mode
 * \param[in] ssiCalibration  The desired SSI calibration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_Delay_Configure(adi_fpga910x_Device_t *fpga910x,
                                         adi_fpga910x_SsiType_e ssiType,
                                         adi_fpga910x_SsiCalibrationCfg_t *ssiCalibration);

/**
 * \brief This function gets the SSI delay configuration from FPGA910X device.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  fpga910x        Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  ssiType         LVDS or CMOS mode
 * \param[out] ssiCalibration  The current SSI calibration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_Delay_Inspect(adi_fpga910x_Device_t *fpga910x,
                                       adi_fpga910x_SsiType_e ssiType,
                                       adi_fpga910x_SsiCalibrationCfg_t *ssiCalibration);

/**
 * \brief Calibrates the SSI delay in FPGA910X for each initialized Rx/Tx channel
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre Channel state is #ADI_ADRV910X_CHANNEL_CALIBRATED
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  channel   The channel of interest
 * \param[out] ssiDelay  The delay value calculated by the calibration routine
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_Delay_Calibrate(adi_fpga910x_Device_t *fpga910x,
                                         adi_common_ChannelNumber_e channel,
                                         uint32_t *ssiDelay);

/**
 * \brief Calculates Tx SS ratio for the Tx channel and writes in FPGA register
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre Channel state is any of #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in] fpga910x          Context variable - Pointer to the FPGA910X device data structure
 * \param[in] channel           The channel of interest
 * \param[in] interfaceRate_Hz  Interface sample rate at serial interface
 * \param[in] sampleRate_Hz     Tx input data rate in Hz
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_TxSs_Ratio_Set(adi_fpga910x_Device_t *fpga910x,
                                        adi_common_ChannelNumber_e channel,
                                        uint32_t interfaceRate_Hz,
                                        uint32_t sampleRate_Hz);

/**
 * \brief Set Tx SS mode for the desired channel
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre Channel state is any of #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] channel   The channel of interest
 * \param[in] data      The desired SS mode value
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_TxSs_Mode_Set(adi_fpga910x_Device_t *fpga910x,
                                       adi_common_ChannelNumber_e channel,
                                       uint32_t data);

/**
 * \brief Calculates Rx SS ratio for the Tx channel and writes in FPGA register
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre Channel state is any of #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in] fpga910x          Context variable - Pointer to the FPGA910X device data structure
 * \param[in] channel           The channel of interest
 * \param[in] interfaceRate_Hz  Interface sample rate at serial interface
 * \param[in] sampleRate_Hz     Rx output data rate in Hz
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_RxSs_Ratio_Set(adi_fpga910x_Device_t *fpga910x,
                                        adi_common_ChannelNumber_e channel,
                                        uint32_t interfaceRate_Hz,
                                        uint32_t sampleRate_Hz);

/**
 * \brief Calculates ORx SS ratio for the Tx channel and writes in FPGA register
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre Channel state is any of #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in] fpga910x          Context variable - Pointer to the FPGA910X device data structure
 * \param[in] channel           The channel of interest
 * \param[in] interfaceRate_Hz  Interface sample rate at serial interface
 * \param[in] sampleRate_Hz     ORx output data rate in Hz
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_OrxSs_Ratio_Set(adi_fpga910x_Device_t *fpga910x,
                                         adi_common_ChannelNumber_e channel,
                                         uint32_t interfaceRate_Hz,
                                         uint32_t sampleRate_Hz);

/**
 * \brief Convert from ssiConfig_t struct defined in profile types to adi_fpga910x_SsiConfig_t
 *
 * \param[in]  fpga910x       Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  profileConfig  ssiConfig_t struct from profile
 * \param[out] fpgaConfig     Updated adi_fpga910x_SsiConfig_t
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_ssi_ConvertSsiConfig(adi_fpga910x_Device_t *fpga910x,
                                          ssiConfig_t *profileConfig,
                                          adi_fpga910x_SsiConfig_t *fpgaConfig);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_FPGA910X_SSI_H_ */
