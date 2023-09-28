/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adrv9025_tx.h
* \brief Contains ADRV9025 transmit related function prototypes for
*        adrv9025_tx.c which helps adi_adrv9025_tx.c
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADRV9025_TX_H_
#define _ADRV9025_TX_H_

#include "adi_adrv9025_tx.h"
#include "../../private/include/adrv9025_bf_tx.h"
#include "../../private/include/adrv9025_reg_addr_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief Returns the Tx SPI channel base addresses for a given Tx Channel
*
* \param device Pointer to the device settings structure
* \param channelMask is the channel for which Tx base address is requested
*
* \retval uint16_t Return Base Address for Tx channel
*/
uint16_t adrv9025_TxAddrDecode(adi_adrv9025_Device_t* device,
                               uint32_t               channelMask);

/**
* \brief Returns the Tx Dac SPI channel base addresses for a given Tx Channel
*
* \param device Pointer to the device settings structure
* \param channelMask is the channel for which Tx base address is requested
*
* \retval Return Base Address for Tx channel
*/
uint16_t adrv9025_TxDacAddrDecode(adi_adrv9025_Device_t* device,
                                  uint32_t               channelMask);

/**
* \brief Returns the Tx SPI channel base addresses for a given Tx Channel
*
* \param device Pointer to the device settings structure
* \param channelMask is the channel for which Tx base address is requested
*
* \retval uint32_t Return Base Address for Tx channel
*/
uint32_t adrv9025_txAttenAddrLookup(adi_adrv9025_Device_t* device,
                                    uint32_t               channelMask);

/**
* \brief
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device
* \param sharedResourceArr
* \param recoveryAction
* \param customError
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxAttenCtrlPinReportError(adi_adrv9025_Device_t* device,
                                           int32_t                sharedResourceArr[],
                                           int32_t                recoveryAction,
                                           const char*            customError);

/**
* \brief
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device
* \param sharedResourceArr
* \param recoveryAction
* \param customError
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxAttenSpi2CtrlPinReportError(adi_adrv9025_Device_t* device,
                                               int32_t                sharedResourceArr[],
                                               int32_t                recoveryAction,
                                               const char*            customError);

/**
* \brief Performs a range check on the GPIO mode attenuation control config
*
*  This API function checks that:
*   -The GPIO pins are in the range GPIO[15:0]
*   -Increment and decrement steps are in the range 0-31
*   -Increment and decrement are not sharing the same GPIO pin
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txAttenPinCtrlCfg Pointer to a adi_adrv9025_TxAttenPinCfg_t structure that
* configures the Tx attenuation pin control.
*
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxAttenPinCtrlCfgSetRangeCheck(adi_adrv9025_Device_t*        device,
                                                adi_adrv9025_TxAttenPinCfg_t* txAttenPinCtrlCfg);

/**
* \brief Performs a range check on the attenuation mode set parameters

* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txAttenMode Selected attenuation mode to be checked
* \param simultaneousUpdateCfg Selected simultaneous update config struct to be checked
* \param gpioModePinCfg Selected gpio config struct to be checked
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxAttenModeSetRangeCheck(adi_adrv9025_Device_t*                device,
                                          adi_adrv9025_TxAttenMode_e            txAttenMode,
                                          adi_adrv9025_TxAttenSimultUpdateCfg_t simultaneousUpdateCfg,
                                          adi_adrv9025_TxAttenPinCfg_t          gpioModePinCfg);

/**
* \brief Performs a range check on the attenuation control config
*
*  This API function checks that:
*   -The channel mask, Attenuation step size, dacScale and attenMode are in range
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txAttenCfg Pointer to a adi_adrv9025_TxAttenCfg_t structure that
* configures the Tx attenuation.
*
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxAttenCfgSetRangeCheck(adi_adrv9025_Device_t*     device,
                                         adi_adrv9025_TxAttenCfg_t* txAttenCfg);

/**
* \brief *
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txToneCfg Pointer to a adi_adrv9025_TxTestToneCfg_t structure that
* configures the Tx Test tone.
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxTestToneCfgSetRangeCheck(adi_adrv9025_Device_t*        device,
                                            adi_adrv9025_TxTestToneCfg_t* txToneCfg);

/**
* \brief Configures one or more TX FIR filters in the device
*
* The device stores 1 Tx filter per channel.
* Tx filters can have 20, 40, 60, or 80 taps.
*
* The function is not called directly. It is called as a part of the adi_adrv9025_initialize() function.
* The function could be used to change A FIR filter later, but would require calibrations to be rerun.
*
* \pre This function is called during device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param txChanMask One or more TX Channel from adi_adrv9025_TxChannels_e.
* \param gain_dB The gain_dB value (-12, -6, 0, 6).
* \param numFirCoefs The number of Fir Coefs in coefs array (20, 40, 60, 80).
* \param coefs[] Pointer to coefs array
* \param arraySize The size of coefs array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxFirWrite(adi_adrv9025_Device_t* device,
                            uint32_t               txChanMask,
                            int8_t                 gain_dB,
                            uint8_t                numFirCoefs,
                            int16_t                coefs[],
                            uint8_t                arraySize);

/**
* \brief Checks whether the pa protection configuration values are within valid range.
*
* \param device Pointer to the ADRV9025 data structure
* \param txPaProtectCfg pointer to the structure that is going to be checked
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxPaProtectCfgRangeCheck(adi_adrv9025_Device_t*         device,
                                          adi_adrv9025_TxPaProtectCfg_t* txPaProtectCfg);

/**
* \brief This API function will check the Tx RF output attenuation table for each Tx output channel
*
* txAttenTableRows->txAttenMult: Tx Atten Multi, set to (0-4095) from table. set to max value if not in table.
* txAttenTableRows->txAttenHp : Tx Atten HP, set to (0 - 63) from table. Set to max value if not in table.
*
* The full TxAtten table can be loaded in a single call.
* Or the table can be loaded in several calls, loading a subset of the table with each function call using the txAttenIndexOffset parameter.
*
* \pre This function may be called any time after device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device's data structure
* \param txChannelMask The Mask of one or more Tx Channels
* \param txAttenIndexOffset Offset of the TxAttentable in the Device from the minimum index (0 - 1023).
* \param txAttenTableRows The Array contain TxAttenTableRows
* \param numTxAttenEntries The number of elements in the TxAttenTableRows array (1 - 1024).
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxAttenTableWriteRangeCheck(adi_adrv9025_Device_t*         device,
                                             uint32_t                       txChannelMask,
                                             uint32_t                       txAttenIndexOffset,
                                             adi_adrv9025_TxAttenTableRow_t txAttenTableRows[],
                                             uint32_t                       numTxAttenEntries);

/**
* \brief This function performs range check on the parameters for adrv9025_TxLoSourceGet API.
*
* This function verifies that the requested Tx channel is valid and that it is initialized
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txChannel Enum to select Tx Channel.
* \param txLoSource Pointer to store Tx channel LO source mapping read back (Output)
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxLoSourceGetRangeCheck(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_TxChannels_e txChannel,
                                         adi_adrv9025_LoSel_e*     txLoSource);

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
* \param device Pointer to the ADRV9025 device data structure
* \param txChannel Enum to select Tx Channel.
* \param txLoSource Pointer to store Tx channel LO source mapping read back (Output)
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxLoSourceGet(adi_adrv9025_Device_t*    device,
                               adi_adrv9025_TxChannels_e txChannel,
                               adi_adrv9025_LoSel_e*     txLoSource);

/**
* \brief Configures GPIO pin for Tx attenuation simultaneous update for given Tx channel address
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannelBaseAddr Base address of Tx channel to be configured
* \param gpioSelection Gpio selection for Tx simultaneous update
* \param releaseCurrentPin 0: Don't release current pin, Other: Release current pin
* \param acquireNewPin 0: Don't acquire new pin, Other: Acquire new pin
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxAttenUpdateGpioPinCtrlCfgSet(adi_adrv9025_Device_t*    device,
                                                adrv9025_BfTxChanAddr_e   txChannelBaseAddr,
                                                adi_adrv9025_GpioPinSel_e gpioSelection,
                                                uint8_t                   releaseCurrentPin,
                                                uint8_t                   acquireNewPin);

/**
* \brief Configures GPIO pins for Tx attenuation GPIO control for given Tx channel address
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannelBaseAddr Base address of Tx channel to be configured
* \param txAttenPinCfg Gpio attenuation control configuration structure to be written
* \param releaseCurrentPins 0:Don't release current pins // Other:Release current pins
* \param acquireNewPins 0:Don't acquire new pins // Other:Acquire pins
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxAttenPinCtrlCfgSet(adi_adrv9025_Device_t*       device,
                                      adrv9025_BfTxChanAddr_e      txChannelBaseAddr,
                                      adi_adrv9025_TxAttenPinCfg_t txAttenPinCfg,
                                      uint8_t                      releaseCurrentPins,
                                      uint8_t                      acquireNewPins);

/**
* \brief This function sets master enable bits ('tx_data_ramp_down_on_pll_unlock' and 'tx_data_ramp_down_on_jesd_dfrm_irq')
         of Pll Unlock and Jesd Dfrm Tx ramp down functionality for selected Tx channel. It masks all the ramp down events
         and unmasks the ones selected through txRampDownInitCfg.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txRampDownInitCfg Init time Tx ramp down configuration populated from init struct.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxRampDownInit(adi_adrv9025_Device_t*            device,
                                adi_adrv9025_TxRampDownInitCfg_t* txRampDownInitCfg);

#ifdef __cplusplus
}
#endif

#endif /*__ADRV9025_TX_H__ */
