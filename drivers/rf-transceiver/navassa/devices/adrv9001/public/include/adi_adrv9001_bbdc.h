/**
 * \file
 * \brief Contains ADRV9001 BBDC-related function prototypes for adi_adrv9001_bbdc.c
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2019 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_BBDC_H_
#define _ADI_ADRV9001_BBDC_H_

#include "adi_adrv9001_types.h"
#include "adi_adrv9001_bbdc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Enable or disable baseband DC rejection algorithm for the given Rx/ORx channel.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in] adrv9001	             Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] port                   The port of the specified channel, ADI_RX or ADI_ORX
 * \param[in] channel                The channel of the specified port
 * \param[in] bbdcRejectionStatus    The status to set for the baseband DC rejection
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_bbdc_RejectionEnable_Set(adi_adrv9001_Device_t *adrv9001,
                                              adi_common_Port_e port,
                                              adi_common_ChannelNumber_e channel,
                                              adi_adrv9001_BbdcRejectionStatus_e bbdcRejectionStatus);

/**
 * \brief Get status of baseband DC rejection algorithm for the given Rx/ORx channel.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001	              Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  port                   The port that the channel refers to
 * \param[in]  channel                The channel of the specified port
 * \param[out] bbdcRejectionStatus    The status to get for the ORx baseband DC rejection
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_bbdc_RejectionEnable_Get(adi_adrv9001_Device_t *adrv9001,
                                              adi_common_Port_e port,
                                              adi_common_ChannelNumber_e channel,
                                              adi_adrv9001_BbdcRejectionStatus_e *bbdcRejectionStatus);

/**
 * \brief Set the loop/feedback gain for BBDC
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED
 *
 * \param[in] adrv9001	        Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] channel           The channel of interest - applies to both Rx and ORx
 * \param[in] loopGain          Fractional number (U1.31) representing feedback gain for BBDC (default: 1/2048)
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9010_bbdc_LoopGain_Set(adi_adrv9001_Device_t *adrv9001,
                                       adi_common_ChannelNumber_e channel,
                                       uint32_t loopGain);

/**
 * \brief Get the loop/feedback gain for BBDC
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in] adrv9001	        Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] channel           The channel of interest - applies to both Rx and ORx
 * \param[out] loopGain         Fractional number (U1.31) representing feedback gain for BBDC (default: 1/2048)
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9010_bbdc_LoopGain_Get(adi_adrv9001_Device_t *adrv9001,
                                       adi_common_ChannelNumber_e channel,
                                       uint32_t *loopGain);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_BBDC_H_ */
