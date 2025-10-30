/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * \file adrv903x_tx.h
 * \brief Contains ADRV903X Tx related private function prototypes for
 *        adrv903x_tx.c which helps adi_adrv903x_tx.c
 *
 * ADRV903X API Version: 2.12.1.4
 */ 

#ifndef _ADRV903X_TX_H_
#define _ADRV903X_TX_H_

#include "../../private/bf/adrv903x_bf_tx_dig_types.h"
#include "../../private/bf/adrv903x_bf_tx_funcs_types.h"
#include "../../private/bf/adrv903x_bf_tx_datapath.h"
#include "../../private/bf/adrv903x_bf_tx_datapath_types.h"

#include "adi_adrv903x_tx_types.h"
#include "adi_adrv903x_error.h"

/**
* \brief Returns the address for a given tx channel's attenuation table.
*
* \param[in] txChannel The channel for which tx atten table address is requested.
*
* \retval uint32_t The address of the attenuation table or 0 if txChannel is ADI_ADRV903X_TXOFF or
*     ADI_ADRV903X_TXNONE.
*/
ADI_API uint32_t adrv903x_txAttenAddrLookup(const adi_adrv903x_TxChannels_e txChannel);

/**
* \brief Returns 0-indexed channel id for a given adi_adrv903x_TxChannels_e.
*
* \param[in] txChannel The value to convert.
*
* \retval uint8_t The channel id. Returns a value greater ADI_ADRV903X_CHAN_ID_MAX if the value supplied cannot be
*     converted (i.e. is not valid value for the enum or refers to TX_ALL or TX_NONE).
*/
ADI_API uint8_t adrv903x_TxChannelsToId(const adi_adrv903x_TxChannels_e txChannel);

/* TODO: remove when adrv903x_Dig_TxEnableBySpi_BfSet and adrv903x_BfTxChanAddr_e are available */
typedef adrv903x_BfTxDigChanAddr_e adrv903x_BfTxChanAddr_e;
#define ADRV903X_BF_TX_CH0 ADRV903X_BF_SLICE_TX_0__TX_DIG
#define ADRV903X_BF_TX_CH1 ADRV903X_BF_SLICE_TX_1__TX_DIG
#define ADRV903X_BF_TX_CH2 ADRV903X_BF_SLICE_TX_2__TX_DIG
#define ADRV903X_BF_TX_CH3 ADRV903X_BF_SLICE_TX_3__TX_DIG
#define ADRV903X_BF_TX_CH4 ADRV903X_BF_SLICE_TX_4__TX_DIG
#define ADRV903X_BF_TX_CH5 ADRV903X_BF_SLICE_TX_5__TX_DIG
#define ADRV903X_BF_TX_CH6 ADRV903X_BF_SLICE_TX_6__TX_DIG
#define ADRV903X_BF_TX_CH7 ADRV903X_BF_SLICE_TX_7__TX_DIG

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxBitfieldAddressGet(adi_adrv903x_Device_t* const    device,
                                                               const adi_adrv903x_TxChannels_e txChannel,
                                                               adrv903x_BfTxChanAddr_e* const  txChannelBitfieldAddr);

/**
* \brief Look up the Tx function bitfield address given a Tx Channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] txChannel Channel selection to read back gain index for (Valid Tx0-Tx7 only)
* \param[out] txFuncsChannelBitfieldAddr Tx function channel bitfield address which will be updated by this function
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_TxFuncsBitfieldAddressGet(adi_adrv903x_Device_t* const        device,
                                                                    const adi_adrv903x_TxChannels_e     txChannel,
                                                                    adrv903x_BfTxFuncsChanAddr_e* const txFuncsChannelBitfieldAddr);

/**
* \brief Look up the Tx Datapath bitfield address given a Tx Channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] txChannel Channel selection to read back gain index for (Valid Tx0-Tx7 only)
* \param[out] txDatapathChannelBitfieldAddr Tx datapath channel bitfield address which will be updated by this function
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDatapathBitfieldAddressGet(adi_adrv903x_Device_t* const device,
                                                                       const adi_adrv903x_TxChannels_e txChannel,
                                                                       adrv903x_BfTxDatapathChanAddr_e* const txDatapathChannelBitfieldAddr);


/**
* \brief Look up the Tx function bitfield address given a Tx Channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] txChannel Channel selection to read back gain index for (Valid Tx0-Tx7 only)
* \param[out] txdIGChannelBitfieldAddr Tx function channel bitfield address which will be updated by this function
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDigBitfieldAddressGet(adi_adrv903x_Device_t* const device,
                                                                  const adi_adrv903x_TxChannels_e txChannel,
                                                                  adrv903x_BfTxDigChanAddr_e* const txDigChannelBitfieldAddr);

/**
* \brief Range check a given TxPowerMonitor configuration
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] txPowerMonitorCfg Pointer to power monitor configuration to be range checked
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_TxPowerMonitorCfgRangeCheck(adi_adrv903x_Device_t* const                    device,
                                                                      const adi_adrv903x_PowerMonitorCfg_t * const    txPowerMonitorCfg);

/**
* \brief Range check a given Srd configuration
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] txSlewRateDetectorCfg Pointer to srd configuration to be range checked
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_TxSlewRateDetectorCfgRangeCheck(adi_adrv903x_Device_t* const device,
                                                                          const adi_adrv903x_SlewRateDetectorCfg_t * const txSlewRateDetectorCfg);

/**
* \brief This function reads back the LO Source Mapping to the requested Tx channel.
*
* The LO source select for a given channel is configured during init time. This function
* can be used to read back the configuration during run time.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X device data structure
* \param[in] txChannel Enum to select Tx Channel.
* \param[out] txLoSource Pointer to store Tx channel LO source mapping read back (Output)
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_TxLoSourceGet(adi_adrv903x_Device_t* const device,
                                                        const adi_adrv903x_TxChannels_e txChannel,
                                                        adi_adrv903x_LoSel_e* const txLoSource);


/**
* \brief Performs range check on Dec power configuration.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device's data structure.
* \param[in] decPowerCfg Decimated power block configured to be range checked

*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDecPowerCfgRangeCheck(adi_adrv903x_Device_t* const device,
                                                                  const adi_adrv903x_TxDecimatedPowerCfg_t* const decPowerCfg);

#endif /* _ADRV903X_TX_H_ */
