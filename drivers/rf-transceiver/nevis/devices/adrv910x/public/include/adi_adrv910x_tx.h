/**
* \file
* \brief Contains ADRV910X transmit related function prototypes for
*        adi_adrv910x_tx.c
*
* ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
*/

/**
* Copyright 2022 Analog Devices Inc.
* Released under the FPGA910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV910X_TX_H_
#define _ADI_ADRV910X_TX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "adi_adrv910x_common_types.h"
#include "adi_adrv910x_tx_types.h"
#include "adi_adrv910x_types.h"

/**
 * \brief Write the attenuation table for the specified Tx channels
 *
 * The full attenuation table can be loaded in a single call or the table can be loaded in several calls,
 * loading a subset of the table with each function call using the indexOffset parameter.
 *
 * \pre This function may be called any time after device initialization
 *
 * \todo Automatically set unused entries to max attenuation
 * \note All unused attenuation table entries should be written to max attenuation so that if an
 * index that doesn't have an entry is selected, the output is known and deterministic.
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv910x              Context variable - Pointer to the ADRV910X device data structure
 * \param[in] channelMask           Channel configuration mask
 * \param[in] indexOffset           The attenuation table index at which to start writing (0 - 839).
 * \param[in] attenTableRows        Attenuation table entries to write
 * \param[in] arraySize             The number of attenuation table rows to write (1 - 840).
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Tx_AttenuationTable_Write(adi_adrv910x_Device_t *adrv910x,
                                               uint32_t channelMask,
                                               uint32_t indexOffset,
                                               adi_adrv910x_TxAttenTableRow_t attenTableRows[],
                                               uint32_t arraySize);

/**
 * \brief Read the atten table entries for the specified Tx channel
 *
 * The full TxAtten table can be read in a single call or the table can be read in several calls,
 * reading a subset of the table with each function call using the txAttenIndexOffset parameter.
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \pre This function may be called any time after device initialization
 *
 * \param[in]  adrv910x             Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  indexOffset          The attenuation table index at which to start reading (0 - 839).
 * \param[out] attenTableRows       Attenuation table entries to write
 * \param[in]  arraySize            The size of the attenTableRows array; the max number of atten table rows to read
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Tx_AttenuationTable_Read(adi_adrv910x_Device_t *adrv910x,
                                              uint32_t indexOffset,
                                              adi_adrv910x_TxAttenTableRow_t attenTableRows[],
                                              uint32_t arraySize);

/**
 * \brief Set the attenuation control mode
 * 
 * \note Message type: \ref timing_direct "Direct register acccess"
 * 
 * \pre Channel state any of STANDBY, CALIBRATED
 * 
 * \param[in] adrv910x          Context variable - Pointer to the ADRV910X device data structure
 * \param[in] mode              The desired tx attenuation mode
 *  
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Tx_AttenuationMode_Set(adi_adrv910x_Device_t *adrv910x,
                                            adi_adrv910x_TxAttenuationControlMode_e mode);

/**
 * \brief Get the current attenuation control mode
 * 
 * \note Message type: \ref timing_direct "Direct register acccess"
 * 
 * \param[in]  adrv910x             Context variable - Pointer to the ADRV910X device data structure
 * \param[out] mode                 The current Tx attenuation mode
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Tx_AttenuationMode_Get(adi_adrv910x_Device_t *adrv910x,
                                            adi_adrv910x_TxAttenuationControlMode_e *mode);

/**
 * \brief Configure the Tx attenuation
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \pre Channel state any of STANDBY, CALIBRATED
 * \pre Attenuation mode is not ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_CLGC
 * 
 * \param[in] adrv910x                  Context variable - Pointer to the ADRV910X device data structure     
 * \param[in] config                    The desired Tx attenuation configuration
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover 
 */
int32_t adi_adrv910x_Tx_Attenuation_Configure(adi_adrv910x_Device_t *adrv910x,
                                            adi_adrv910x_TxAttenuationConfig_t *config);

/**
 * \brief Inspect the Tx attenuation configuration
 * 
 * \note Message type: \ref timing_direct "Direct register acccess"
 * 
 * \pre Channel state any of STANDBY, CALIBRATED
 * 
 * \param[in]  adrv910x                  Context variable - Pointer to the ADRV910X device data structure
 * \param[out] config                    The current Tx attenuation configuration
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover 
 */
int32_t adi_adrv910x_Tx_Attenuation_Inspect(adi_adrv910x_Device_t *adrv910x,
                                            adi_adrv910x_TxAttenuationConfig_t *config);

/**
 * \brief Set the Tx attenuation
 *
 * \pre If attenuation mode is ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_PIN:
 *      channel state must be any of STANDBY, CALIBRATED, PRIMED.
 * \pre If attenuation mode is ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_SPI:
 *      channel state must be any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 * \pre Attenuation mode is ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_CLGC
 *      channel state must be any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *      clgcLoopOpen must be true
 * 
 * \note Message type: \ref timing_direct "Direct register acccess"
 * \note The new attenuation only takes effect in the RF_ENABLED state - may read back incorrect value otherwise
 *
 * \param[in] adrv910x                     Context variable - Pointer to the ADRV910X device data structure 
 * \param[in] attenuation_mdB              The desired attenuation in milli-dB (Range: 0 to 41950 mdB)
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover 
 */
int32_t adi_adrv910x_Tx_Attenuation_Set(adi_adrv910x_Device_t *adrv910x,
                                        uint16_t attenuation_mdB);

/**
 * \brief Get the Tx attenuation
 * 
 * \pre This feature requires the initialization to be complete and the attenuation table to be loaded.
 * 
 * \note Message type: \ref timing_direct "Direct register acccess"
 * \note During the transition from RF_ENABLED to PRIMED, attenuation will be ramped up to 40dB to protect the analog
 * front-end. During the reverse transition, the attenuation will be ramped to the value last set by the user. As a
 * result, unexpected values may be returned during TDD operation.
 * 
 * \param[in]  adrv910x                 Context variable - Pointer to the ADRV910X device data structure 
 * \param[out] attenuation_mdB          The current attenuation in milli-dB (Range: 0 to 41950 mdB)
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover  
 */
int32_t adi_adrv910x_Tx_Attenuation_Get(adi_adrv910x_Device_t *adrv910x,
                                        uint16_t *attenuation_mdB);

/**
 * \brief Enable or disable the Tx output power boost
 * Enable or disable the Tx output power boost. Enabling the boost increases the output power by 3dB. Linearity will be
 * degraded. Boost is disabled by default.
 *
 * \pre Channel state is STANDBY
  * \pre Attenuation mode is not ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_CLGC
 * 
 * \note Message type: \ref timing_direct "Direct register acccess"
 * 
 * \param[in] adrv910x                      Context variable - Pointer to the ADRV910X device data structure 
 * \param[in] boostEnable                   Whether or not to enable the Tx output power boost
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover   
 */
int32_t adi_adrv910x_Tx_OutputPowerBoost_Set(adi_adrv910x_Device_t *adrv910x,
                                            bool boostEnable);

/**
 * \brief Get the current Tx output power boost enable status
 * 
 * \note Message type: \ref timing_direct "Direct register acccess"
 * 
 * \param[in] adrv910x                      Context variable - Pointer to the ADRV910X device data structure
 * \param[out] boostEnable                  Whether or not the Tx output power boost is currently enabled
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover   
 */
int32_t adi_adrv910x_Tx_OutputPowerBoost_Get(adi_adrv910x_Device_t *adrv910x,
                                            bool *boostEnable);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910X_TX_H_ */
