/**
 * \file
 * \brief Contains ADRV9001 receive related function prototypes for adi_adrv9001_orx.c
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2019 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_ORX_H_
#define _ADI_ADRV9001_ORX_H_

#include "adi_adrv9001_types.h"
#include "adi_adrv9001_orx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Sets the Manual Gain Index for the given ORx channel
 *
 * The maximum index is 10 and the minimum index is 2.
 *
 * If the value passed in the gainIndex parameter is within range of the gain
 * table minimum and maximum indices, the ORx channel gain index will be written
 * to the transceiver.
 * Else, an error will be returned.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre ARM is initialized
 *
 * \param[in] adrv9001      Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel       The ORx Channel for which to set the gain
 * \param[in] gainIndex     The gain table index to set the channel to
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_ORx_Gain_Set(adi_adrv9001_Device_t *adrv9001,
                                  adi_common_ChannelNumber_e channel,
                                  uint8_t gainIndex);

/**
 * \brief Reads the ORx AGC Gain Index for the requested ORx channel.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function may be called any time after device initialization.
 *      However, gain indices are tracked only after the device goes into a Receiver mode.
 *
 * \param[in]  adrv9001     Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel      The ORx Channel from which to read the gain
 * \param[out] gainIndex    The current gain table index the channel is set to
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_ORx_Gain_Get(adi_adrv9001_Device_t *adrv9001,
                                  adi_common_ChannelNumber_e channel,
                                  uint8_t *gainIndex);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_ORX_H_ */
