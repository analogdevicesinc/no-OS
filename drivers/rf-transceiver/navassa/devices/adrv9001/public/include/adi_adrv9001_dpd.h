/**
 * \file
 * \brief Contains ADRV9001 Digital Pre-Distortion (DPD) related function prototypes
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2019 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_DPD_H_
#define _ADI_ADRV9001_DPD_H_

#include "adi_adrv9001_types.h"
#include "adi_adrv9001_dpd_types.h"

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
 * \note Called by adi_adrv9001_Utilities_InitRadio_Load as part of device initialization
 *
 * \param[in] adrv9001		Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] channel       The channel to configure
 * \param[in] dpdConfig     The desired DPD configuration
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_dpd_Initial_Configure(adi_adrv9001_Device_t *adrv9001, 
                                           adi_common_ChannelNumber_e channel,
                                           adi_adrv9001_DpdInitCfg_t *dpdConfig);

/**
 * \brief Inspect initial DPD settings
 * 
 * Inspect pre-init calibration DPD settings via \ref timing_mailbox "mailbox command"
 *
 * \param[in]  adrv9001		Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  channel      The channel to configure
 * \param[out] dpdConfig    The current DPD configuration
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_dpd_Initial_Inspect(adi_adrv9001_Device_t *adrv9001, 
                                         adi_common_ChannelNumber_e channel,
                                         adi_adrv9001_DpdInitCfg_t *dpdConfig);

/**
 * \brief Configure DPD settings and reset DPD operation
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is CALIBRATED
 *
 * \param[in] adrv9001		Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] channel       The channel to configure
 * \param[in] dpdConfig     The desired DPD configuration
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_dpd_Configure(adi_adrv9001_Device_t *adrv9001,
                                   adi_common_ChannelNumber_e channel,
                                   adi_adrv9001_DpdCfg_t *dpdConfig);

/**
 * \brief Inspect DPD settings
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \param[in]  adrv9001		Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  channel      The channel to configure
 * \param[out] dpdConfig    The current DPD configuration
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_dpd_Inspect(adi_adrv9001_Device_t *adrv9001,
                                 adi_common_ChannelNumber_e channel,
                                 adi_adrv9001_DpdCfg_t *dpdConfig);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_DPD_H_ */
