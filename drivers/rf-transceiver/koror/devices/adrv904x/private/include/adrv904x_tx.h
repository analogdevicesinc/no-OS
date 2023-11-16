/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adrv904x_tx.h
 * \brief Contains ADRV904X Tx related private function prototypes for
 *        adrv904x_tx.c which helps adi_adrv904x_tx.c
 *
 * ADRV904X API Version: 2.9.0.4
 */ 

#ifndef _ADRV904X_TX_H_
#define _ADRV904X_TX_H_

#include "../../private/bf/adrv904x_bf_tx_dig_types.h"
#include "../../private/bf/adrv904x_bf_tx_funcs_types.h"

#include "../../private/bf/adrv904x_bf_tx_band_duc_types.h"
#include "../../private/bf/adrv904x_bf_tx_dfe_dig_regs_types.h"

#include "adi_adrv904x_tx_types.h"
#include "adi_adrv904x_error.h"


#include "../../private/bf/adrv904x_bf_cduc_hb_dpath.h"
#include "../../private/bf/adrv904x_bf_tx_dpd_act_mmr.h"

/**
* \brief Returns the address for a given tx channel's attenuation table.
*
* \param[in] txChannel The channel for which tx atten table address is requested.
*
* \retval uint32_t The address of the attenuation table or 0 if txChannel is ADI_ADRV904X_TXOFF or
*     ADI_ADRV904X_TXNONE.
*/
ADI_API uint32_t adrv904x_txAttenAddrLookup(const adi_adrv904x_TxChannels_e txChannel);

/**
* \brief Returns 0-indexed channel id for a given adi_adrv904x_TxChannels_e.
*
* \param[in] txChannel The value to convert.
*
* \retval uint8_t The channel id. Returns a value greater ADI_ADRV904X_CHAN_ID_MAX if the value supplied cannot be
*     converted (i.e. is not valid value for the enum or refers to TX_ALL or TX_NONE).
*/
ADI_API uint8_t adrv904x_TxChannelsToId(const adi_adrv904x_TxChannels_e txChannel);

/* TODO: remove when adrv904x_Dig_TxEnableBySpi_BfSet and adrv904x_BfTxChanAddr_e are available */
typedef adrv904x_BfTxDigChanAddr_e adrv904x_BfTxChanAddr_e;
#define ADRV904X_BF_TX_CH0 ADRV904X_BF_SLICE_TX_0__TX_DIG
#define ADRV904X_BF_TX_CH1 ADRV904X_BF_SLICE_TX_1__TX_DIG
#define ADRV904X_BF_TX_CH2 ADRV904X_BF_SLICE_TX_2__TX_DIG
#define ADRV904X_BF_TX_CH3 ADRV904X_BF_SLICE_TX_3__TX_DIG
#define ADRV904X_BF_TX_CH4 ADRV904X_BF_SLICE_TX_4__TX_DIG
#define ADRV904X_BF_TX_CH5 ADRV904X_BF_SLICE_TX_5__TX_DIG
#define ADRV904X_BF_TX_CH6 ADRV904X_BF_SLICE_TX_6__TX_DIG
#define ADRV904X_BF_TX_CH7 ADRV904X_BF_SLICE_TX_7__TX_DIG

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxBitfieldAddressGet(adi_adrv904x_Device_t* const    device,
                                                               const adi_adrv904x_TxChannels_e txChannel,
                                                               adrv904x_BfTxChanAddr_e* const  txChannelBitfieldAddr);

/**
* \brief Look up the Tx function bitfield address given a Tx Channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] txChannel Channel selection to read back gain index for (Valid Tx0-Tx7 only)
* \param[out] txFuncsChannelBitfieldAddr Tx function channel bitfield address which will be updated by this function
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncsBitfieldAddressGet(adi_adrv904x_Device_t* const        device,
                                                                    const adi_adrv904x_TxChannels_e     txChannel,
                                                                    adrv904x_BfTxFuncsChanAddr_e* const txFuncsChannelBitfieldAddr);
/**
* \brief Look up the Tx function bitfield address given a Tx Channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] txChannel Channel selection to read back gain index for (Valid Tx0-Tx7 only)
* \param[out] txdIGChannelBitfieldAddr Tx function channel bitfield address which will be updated by this function
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxDigBitfieldAddressGet(adi_adrv904x_Device_t* const device,
                                                                  const adi_adrv904x_TxChannels_e txChannel,
                                                                  adrv904x_BfTxDigChanAddr_e* const txDigChannelBitfieldAddr);

/**
* \brief Range check a given TxPowerMonitor configuration
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] txPowerMonitorCfg Pointer to power monitor configuration to be range checked
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxPowerMonitorCfgRangeCheck(adi_adrv904x_Device_t* const                    device,
                                                                      const adi_adrv904x_PowerMonitorCfg_t * const    txPowerMonitorCfg);

/**
* \brief Range check a given Srd configuration
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] txSlewRateDetectorCfg Pointer to srd configuration to be range checked
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxSlewRateDetectorCfgRangeCheck(adi_adrv904x_Device_t* const device,
                                                                          const adi_adrv904x_SlewRateDetectorCfg_t * const txSlewRateDetectorCfg);

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
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] txChannel Enum to select Tx Channel.
* \param[out] txLoSource Pointer to store Tx channel LO source mapping read back (Output)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxLoSourceGet(adi_adrv904x_Device_t* const device,
                                                        const adi_adrv904x_TxChannels_e txChannel,
                                                        adi_adrv904x_LoSel_e* const txLoSource);


/**
* \brief Performs range check on Dec power configuration.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] decPowerCfg Decimated power block configured to be range checked

*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxDecPowerCfgRangeCheck(adi_adrv904x_Device_t* const device,
                                                                  const adi_adrv904x_TxDecimatedPowerCfg_t* const decPowerCfg);

/**
* \brief Performs mask validation
*
* \param[in] maskVal - mask value to check
* \param[in] allowOnlyOne - when 1: only one bit should be set; when false: multiples allowed
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_ValidateMask(uint32_t maskVal, uint8_t allowOnlyOne);

/**
* \brief Look up the Tx DUC band 0 bitfield address given a Tx Channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] txChannel Channel selection to read back gain index for (Valid Tx0-Tx7 only)
* \param[out] txBandDucChannelBitfieldAddr Tx DUC band 0 bitfield address which will be updated by this function
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxBandDuc0BitfieldAddressGet(adi_adrv904x_Device_t* const device,
                                                                       const adi_adrv904x_TxChannels_e txChannel,
                                                                       adrv904x_BfTxBandDucChanAddr_e* const txBandDucChannelBitfieldAddr);

/**
* \brief Look up the Tx DUC band 1 bitfield address given a Tx Channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] txChannel Channel selection to read back gain index for (Valid Tx0-Tx7 only)
* \param[out] txBandDucChannelBitfieldAddr Tx DUC band 1 bitfield address which will be updated by this function
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxBandDuc1BitfieldAddressGet(adi_adrv904x_Device_t* const device,
                                                                       const adi_adrv904x_TxChannels_e txChannel,
                                                                       adrv904x_BfTxBandDucChanAddr_e* const txBandDucChannelBitfieldAddr);

/**
* \brief Performs range check on tx cduc carrier setting.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] chanSelect is the tx channel parameter
* \param[in] carrierSelect is the tx carrier parameter
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCducCarrierCheck(adi_adrv904x_Device_t* const device,
                                                             const uint8_t chanSelect,
                                                             const uint8_t carrierSelect);
                                                                
/**
* \brief Look up the Tx Hb Datapath bitfield address given a Tx channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] txChannel Channel selection to read back gain index for(Valid Tx0 - Tx7 only)
* \param[out] cducHbDpathChannelBitfieldAddr Tx cduc Hb Datapath channel bitfield address which will be updated by this function
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpathBitfieldAddressGet(adi_adrv904x_Device_t* const device,
                                                                        const adi_adrv904x_TxChannels_e txChannel,
                                                                        adrv904x_BfCducHbDpathChanAddr_e* const cducHbDpathChannelBitfieldAddr);

/**
* \brief Look up the Tx Dpd Act Mmr bitfield address given a Tx channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] txChannel Channel selection to read back gain index for(Valid Tx0 - Tx7 only)
* \param[out] dpdActMmrChannelBitfieldAddr Tx dpd act mmr channel bitfield address which will be updated by this function
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_DpdActMmrBitfieldAddressGet(adi_adrv904x_Device_t* const device,
                                                                      const adi_adrv904x_TxChannels_e txChan,
                                                                      adrv904x_BfTxDpdActMmrChanAddr_e* const dpdActMmrChannelBitfieldAddr);

/**
* \brief Returns Tx Dfe Dig base address for selected channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] txChannel Tx channel selection
* \param[out] txDfeDigChannelBitfieldAddr Tx Dfe Dig base address of selected channel
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxDfeDigBitfieldAddressGet(adi_adrv904x_Device_t* const device,
                                                                  const adi_adrv904x_TxChannels_e txChannel,
                                                                  adrv904x_BfTxDfeDigRegsChanAddr_e* const txDfeDigChannelBitfieldAddr);
#endif /* _ADRV904X_TX_H_ */
