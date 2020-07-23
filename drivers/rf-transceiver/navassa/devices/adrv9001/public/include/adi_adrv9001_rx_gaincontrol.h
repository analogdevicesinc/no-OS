/**
 * \file
 * \brief Gain Control
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2019 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_RX_GAINCONTROL_H_
#define _ADI_ADRV9001_RX_GAINCONTROL_H_

#include "adi_adrv9001_types.h"
#include "adi_adrv9001_rx_types.h"
#include "adi_adrv9001_rx_gaincontrol_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Configures the Rx gain control mode for the specified channel
 *
 * The ADRV9001 device defaults to Manual Gain Control (MGC) mode on power-up.
 * This API is used to reconfigure the Rx Gain Control Mode.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function should be called before Rx operation is initiated.
 *
 * \param[in] adrv9001      Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] device        Pointer to the ADRV9001 device data structure
 * \param[in] channel       The Rx Channel for which to set the gain control mode
 * \param[in] mode          Desired gain control mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_GainControl_Mode_Set(adi_adrv9001_Device_t *adrv9001,
                                             adi_common_ChannelNumber_e channel,
                                             adi_adrv9001_RxGainControlMode_e mode);

/**
 * \brief Retrieves the currently configured Rx gain control mode
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv9001         Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel          Rx channel for which gain control mode read back is requested
 * \param[out] mode             The current gain control mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_GainControl_Mode_Get(adi_adrv9001_Device_t *adrv9001,
                                             adi_common_ChannelNumber_e channel,
                                             adi_adrv9001_RxGainControlMode_e *mode);
    
/**
 * \brief Configure Automatic Gain Control (AGC) for the specified Rx channel
 *
 * \pre This function may be called any time after device initialization
 *
 * \note This function does NOT enable AGC mode. Call adi_adrv9001_Rx_GainCtrlMode_Set() afterwards
 * \note Message type: \ref timing_direct "Direct register acccess"
 * 
 * \param[in] adrv9001       Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel        The Rx Channel for which to configure AGC
 * \param[in] agcCfg         The AGC configuration to apply
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_GainControl_Configure(adi_adrv9001_Device_t *adrv9001,
                                              adi_common_ChannelNumber_e channel,
                                              adi_adrv9001_GainControlCfg_t *agcCfg);

/**
 * \brief Inspect the Automatic Gain Control (AGC) configuration for the specified Rx channel
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \pre This function may be used only if AGC is active
 *
 * \param[in]  adrv9001      Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel       The Rx Channel for which to configure AGC
 * \param[out] agcCfg        The current AGC configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_GainControl_Inspect(adi_adrv9001_Device_t *adrv9001,
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv9001_GainControlCfg_t *agcCfg);

/**
 * \brief Set the min and max gain indexes for AGC for the specified channel.
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001           Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel            The Rx Channel for which to set min and max gain indices
 * \param[in] minGainIndex       The lower limit of the gain index. Valid range is from 0 to 255.
 * \param[in] maxGainIndex       The upper limit of the gain index. Valid range is from 0 to 255.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_GainControl_MinMaxGainIndex_Set(adi_adrv9001_Device_t *adrv9001,
                                                        adi_common_ChannelNumber_e channel,
                                                        uint8_t minGainIndex,
                                                        uint8_t maxGainIndex);
    
/**
 * \brief Get the min and max gain indexes for AGC for the specified Rx channel
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in]  adrv9001          Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel           The Rx Channel from which to get min and max gain indices
 * \param[out] minGainIndex      The lower limit of the gain index
 * \param[out] maxGainIndex      The upper limit of the gain index
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_GainControl_MinMaxGainIndex_Get(adi_adrv9001_Device_t *adrv9001,
                                                        adi_common_ChannelNumber_e channel,
                                                        uint8_t *minGainIndex,
                                                        uint8_t *maxGainIndex);

/**
 * \brief Reset the AGC state machine
 *
 * \note On reset, gain reverts to the maximum
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001      Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel       The Rx Channel for which to reset the AGC state machine
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_GainControl_Reset(adi_adrv9001_Device_t *adrv9001, adi_common_ChannelNumber_e channel);

/**
 * \brief Configure gain control for pin mode
 * 
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001      Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel       The Rx Channel for which to configure pin gain control
 * \param[in] config        The desired pin gain control configuration
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_GainControl_PinMode_Configure(adi_adrv9001_Device_t *adrv9001, 
                                                      adi_common_ChannelNumber_e channel,
                                                      adi_adrv9001_RxGainControlPinCfg_t *config);

/**
 * \brief Inspect gain control for pin mode
 * 
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in]  adrv9001     Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel      The Rx Channel for which to configure pin gain control
 * \param[out] config       The current pin gain control configuration
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_GainControl_PinMode_Inspect(adi_adrv9001_Device_t *adrv9001, 
                                                    adi_common_ChannelNumber_e channel,
                                                    adi_adrv9001_RxGainControlPinCfg_t *config);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_RX_GAINCONTROL_H_ */
