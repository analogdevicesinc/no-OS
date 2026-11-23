/**
 * \file
 * \brief Contains ADRV910X Digital Pre-Distortion (DPD) related function prototypes
 *
 * Copyright 2019-2025 Analog Devices Inc.
 * Released under the ADRV910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV910X_DPD_H_
#define _ADI_ADRV910X_DPD_H_

#include "adi_adrv910x_types.h"
#include "adi_adrv910x_dpd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Configure initial DPD settings
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is STANDBY
 * \pre Profile must indicate external loopback for channel exists
 *
 * \note Called by adi_adrv910x_Utilities_InitRadio_Load as part of device initialization
 *
 * \param[in] adrv910x		Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] channel       The channel to configure
 * \param[in] dpdConfig     The desired DPD configuration
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_dpd_Initial_Configure(adi_adrv910x_Device_t *adrv910x, 
                                           adi_common_ChannelNumber_e channel,
                                           adi_adrv910x_DpdInitCfg_t *dpdConfig);

/**
 * \brief Inspect initial DPD settings
 * 
 * Inspect pre-init calibration DPD settings via \ref timing_mailbox "mailbox command"
 *
 * \param[in]  adrv910x		Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  channel      The channel to configure
 * \param[out] dpdConfig    The current DPD configuration
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_dpd_Initial_Inspect(adi_adrv910x_Device_t *adrv910x, 
                                         adi_common_ChannelNumber_e channel,
                                         adi_adrv910x_DpdInitCfg_t *dpdConfig);

/**
 * \brief Configure DPD settings and reset DPD operation
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \param[in] adrv910x		Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] channel       The channel to configure
 * \param[in] dpdConfig     The desired DPD configuration
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_dpd_Configure(adi_adrv910x_Device_t *adrv910x,
                                   adi_common_ChannelNumber_e channel,
                                   adi_adrv910x_DpdCfg_t *dpdConfig);

/**
 * \brief Inspect DPD settings
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \param[in]  adrv910x		Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  channel      The channel to configure
 * \param[out] dpdConfig    The current DPD configuration
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_dpd_Inspect(adi_adrv910x_Device_t *adrv910x,
                                 adi_common_ChannelNumber_e channel,
                                 adi_adrv910x_DpdCfg_t *dpdConfig);
    
/**
 * \brief Set DPD coefficients to be used at the next start of DPD
 * 
 * By default, DPD starts from unity coefficients. This allows the restoration of a previously saved set of
 * coefficients to perform a "hot start."
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * 
 * \pre Channel state is CALIBRATED
 *
 * \param[in] adrv910x		Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] channel       The channel of interest
 * \param[in] coefficients	Coefficients to set
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_dpd_coefficients_Set(adi_adrv910x_Device_t *adrv910x,
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv910x_DpdCoefficients_t *coefficients);

/**
 * \brief Get DPD coefficients for the last solution
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * 
 * \pre Channel state is CALIBRATED
 *
 * \param[in]  adrv910x		Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  channel      The channel of interest
 * \param[in,out] coefficients	The latest coefficients
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_dpd_coefficients_Get(adi_adrv910x_Device_t *adrv910x,
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv910x_DpdCoefficients_t *coefficients);

/**
 * \brief Read DPD captured data
 * 
 * \note Message type: \ref timing_direct "Direct register access"
 * 
 * \pre Channel state is CALIBRATED or PRIMED
 *
 * \param[in]  adrv910x		   Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  channel         The channel of interest
 * \param[out] iData_tx        The returned 18-bit(1.17 format) I values of DPD captured Tx data
 * \param[out] qData_tx        The returned 18-bit(1.17 format) Q values of DPD captured Tx data
 * \param[out] iData_elb       The returned 18-bit(1.17 format) I values of DPD captured ELB data
 * \param[out] qData_elb       The returned 18-bit(1.17 format) Q values of DPD captured ELB data
 * \param[in]  length          The length of the iData_tx, qData_tx, iData_elb, and qData_elb arrays
 * \param[in]  autoIncrement   Boolean flag to enable or disable auto-increment of ARM register address
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_dpd_CaptureData_Read(adi_adrv910x_Device_t *adrv910x,
                                          adi_common_ChannelNumber_e channel,
                                          int32_t iData_tx[],
                                          int32_t qData_tx[],
                                          int32_t iData_elb[],
                                          int32_t qData_elb[],
                                          uint32_t length,
                                          bool autoIncrement);

/**
 * \brief Get information about DPD operation and power at some points on the data path
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is CALIBRATED or PRIMED or RF_ENABLED
 *
 * \param[in]  adrv910x		Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  channel      The channel of interest
 * \param[out] dpdChannelStatus	The latest DPD operational status
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_dpd_channel_Status_Get(adi_adrv910x_Device_t* adrv910x, adi_common_ChannelNumber_e channel, adi_adrv910x_DpdChannelStatus_t* dpdChannelStatus);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910X_DPD_H_ */
