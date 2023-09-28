/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_adrv9025_tx.h
* \brief Contains ADRV9025 transmit related function prototypes for
*        adi_adrv9025_tx.c
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_TX_H_
#define _ADI_ADRV9025_TX_H_

#include "adi_adrv9025_types.h"
#include "adi_adrv9025_tx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
* Initialization functions
****************************************************************************
*/

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
*
* \param device Pointer to the ADRV9025 data structure
* \param txChannelMask One or more TX Channel from adi_adrv9025_TxChannels_e.
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
int32_t adi_adrv9025_TxFirWrite(adi_adrv9025_Device_t* device,
                                uint32_t               txChannelMask,
                                int8_t                 gain_dB,
                                uint8_t                numFirCoefs,
                                int16_t                coefs[],
                                uint8_t                arraySize);

/**
* \brief Get one Tx FIR filters information for the requested Tx channel in the device
*
* \pre This function is called during device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param txChannel adi_adrv9025_TxChannels_e enum type to select.
* \param gain_dB Pointer to the gain_dB value read from device (output)
* \param numFirCoefs Pointer to the numFirCoefs value read from device (output)
* \param coefs[] Pointer to coefs array read from the device (output)
* \param arraySize The size of coefs array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
* \
*/
int32_t adi_adrv9025_TxFirRead(adi_adrv9025_Device_t*    device,
                               adi_adrv9025_TxChannels_e txChannel,
                               int8_t*                   gain_dB,
                               uint8_t*                  numFirCoefs,
                               int16_t                   coefs[],
                               uint8_t                   arraySize);

/****************************************************************************
* Runtime functions
****************************************************************************
*/

/**
* \brief Function to set the Tx attenuation configuration
*
* \pre This function is called after the device is initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param txAttenCfg array of structures of type adi_adrv9025_TxAttenCfg_t which will configure one or more channels
* \param attenCfgs number of configurations passed in the array

*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenCfgSet(adi_adrv9025_Device_t*    device,
                                   adi_adrv9025_TxAttenCfg_t txAttenCfg[],
                                   uint8_t                   attenCfgs);

/**
* \brief Function to set the Tx attenuation configuration
*
* \pre This function is called after the device is initialized
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param txChannel channel selection to read the attenuation configuration of type adi_adrv9025_TxChannels_e
* \param txAttenCfg pointer to structure of type adi_adrv9025_TxAttenCfg_t which will contain the configuration for the givven channel
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenCfgGet(adi_adrv9025_Device_t*     device,
                                   adi_adrv9025_TxChannels_e  txChannel,
                                   adi_adrv9025_TxAttenCfg_t* txAttenCfg);

/**
* \brief This API function will set the Tx RF output attenuation table for each Tx output channel
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
int32_t adi_adrv9025_TxAttenTableWrite(adi_adrv9025_Device_t*         device,
                                       uint32_t                       txChannelMask,
                                       uint32_t                       txAttenIndexOffset,
                                       adi_adrv9025_TxAttenTableRow_t txAttenTableRows[],
                                       uint32_t                       numTxAttenEntries);

/**
* \brief This API function will get the Tx RF output attenuation table for each Tx output channel
*
*  The return table are array of Entries.
*  txAttenTableRows->txAttenMult: Tx Atten Multi, get value (0-4095)
*  txAttenTableRows->txAttenHp : Tx Atten HP, get value of (0 - 63)
*
* The full TxAtten table can be read in a single call.
* Or the table can be read in several calls, reading a subset of the table with each function call using the txAttenIndexOffset parameter.
*
* \pre This function may be called any time after device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device's data structure
* \param txChannel The Mask of one or more Tx Channels
* \param txAttenIndexOffset Offset of the TxAttentable in the Device from the minimum index (0 - 1023).
* \param txAttenTableRows The Array contain TxAttenTableEntries
* \param numTxAttenEntries The number of elements in the TxAttenTableRows array (1 - 1024).
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenTableRead(adi_adrv9025_Device_t*         device,
                                      adi_adrv9025_TxChannels_e      txChannel,
                                      uint32_t                       txAttenIndexOffset,
                                      adi_adrv9025_TxAttenTableRow_t txAttenTableRows[],
                                      uint32_t                       numTxAttenEntries);

/**
* \brief Function to set the attenuation control mode
*
*
* \param device Pointer to the ADRV9025 device's data structure
* \param txChannelMask The Mask of one or more Tx Channels
* \param txAttenMode Tx attenuation mode selection
* \param simultaneousUpdateCfg Simultaneous update configuration, valid only when txAttenMode is selected as SPI
* \param gpioModePinCfg GPIO control mode pin configuration, valid only when txAttenMode is selected as Mode
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenModeSet(adi_adrv9025_Device_t*                device,
                                    uint32_t                              txChannelMask,
                                    adi_adrv9025_TxAttenMode_e            txAttenMode,
                                    adi_adrv9025_TxAttenSimultUpdateCfg_t simultaneousUpdateCfg,
                                    adi_adrv9025_TxAttenPinCfg_t          gpioModePinCfg);

/**
* \brief Function to get the attenuation control mode programed to the part
*
*
* \param device Pointer to the ADRV9025 device's data structure
* \param txChannel Tx channel selection to read attenuation configuration
* \param txAttenMode Pointer to attenuation mode enum to read back
* \param simultaneousUpdateCfg Pointer to simultaneous config struct to read back
* \param gpioModePinCfg Pointer to GPIO pin config struct to read back
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenModeGet(adi_adrv9025_Device_t*                 device,
                                    adi_adrv9025_TxChannels_e              txChannel,
                                    adi_adrv9025_TxAttenMode_e*            txAttenMode,
                                    adi_adrv9025_TxAttenSimultUpdateCfg_t* simultaneousUpdateCfg,
                                    adi_adrv9025_TxAttenPinCfg_t*          gpioModePinCfg);

/**
* \brief This API function has to be used in order to set the Tx RF output attenuation
* for the Tx output channels
*
* The attenuation given by txAttenuation.txAttenuation_mdB will be applied to the txChannel or channels
* indicated by the txAttenuation.txChannelMask as follows:
*
*         txChannel       |  Description
* ------------------------|------------------------------------
*   ADI_ADRV9025_TX1      | sets attenuation txAttenuation_mdB for TX1
*   ADI_ADRV9025_TX2      | sets attenuation txAttenuation_mdB for TX2
*   ADI_ADRV9025_TX3      | sets attenuation txAttenuation_mdB for TX3
*   ADI_ADRV9025_TX4      | sets attenuation txAttenuation_mdB for TX4
*
* \pre This function may be called any time after device initialization
* \pre This feature requires the initialization to be complete and the attenuation table to be loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device's data structure
* \param txAttenuation array of structure type adi_adrv9025_TxAtten_t
* which indicates the channel and the desired TxAttenuation in milli-dB
* (Range: 0 to 41950 mdB)
* \param numTxAttenConfigs array size passed in txAttenuation array
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenSet(adi_adrv9025_Device_t* device,
                                adi_adrv9025_TxAtten_t txAttenuation[],
                                uint8_t                numTxAttenConfigs);

/**
* \brief Reads back the attenuation setting for the Tx channel output for the selected channel
*
*  This function reads back the Tx attenuation setting for either the Tx1, Tx2, Tx3 or Tx4 RF channel currently applied to the transmit chain.
*  This function can work with SPI mode or pin controlled Tx attenuation mode using the increment/decrement GPIO
*  pins.
*
* \pre This feature requires the initialization to be complete and the attenuation table to be loaded.
* \pre The Tx data path must be powered up for the current attenuation value to be valid.  If the Tx data path
*  is powered down or the radio is off, the last Tx attenuation setting when the Tx output was previously active will be
*  read back.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device's data structure
* \param txChannel channel selection to read the attenuation is of type adi_adrv9025_TxChannels_e
* \param txAttenuation Pointer to structure of adi_adrv9025_TxAtten_t type to store the readback value of the desired Tx channel attenuation in milli-dB (Range: 0 to 41950 mdB)
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenGet(adi_adrv9025_Device_t*    device,
                                adi_adrv9025_TxChannels_e txChannel,
                                adi_adrv9025_TxAtten_t*   txAttenuation);

/**
* \brief Sets the DAC full scale current boost
*
* This function can be used to adjust the DAC fullscale.
* This will change the Tx output power. The DAC fullscale of zero DB is the
* default setting.
*
* \pre This function must be called before loading the ADRV9025 ARM processor
*  WARNING: This function should only be called once during init time before performing init calibrations
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device's data structure
* \param txChannel Channel to set the Dac scale
* \param dacFullScale Is an enum that sets the DAC boost levels.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DacFullScaleSet(adi_adrv9025_Device_t*      device,
                                     adi_adrv9025_TxChannels_e   txChannel,
                                     adi_adrv9025_DacFullScale_e dacFullScale);

/**
* \brief Gets the DAC full scale current boost
*
* \pre This function must be called before loading the ADRV9025 ARM processor
*
* \param device Pointer to the ADRV9025 device's data structure
* \param txChannel Channel to get the Dac scale
* \param dacFullScale pointer to an enum that will contain the DAC boost levels.
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DacFullScaleGet(adi_adrv9025_Device_t*       device,
                                     adi_adrv9025_TxChannels_e    txChannel,
                                     adi_adrv9025_DacFullScale_e* dacFullScale);

/**
* \brief Enables/Disables the Tx NCO test tone
*
*  This function enables/disables a digital numerically controlled oscillator
*  in the ADRV9025 Digital to create a test CW tone on the selected Tx RF output/s.
*
*  The TxAttenuation is forced in this function to max analog output power.
*  Ensure no other API's are called that change the Tx attenuation mode when using this
*  function
*
*  When the Tx NCO test tone is disabled, the function sets the Tx attenuation
*  to the SPI mode. User may need to use other functions to change the mode.
*
* \param device Pointer to the ADRV9025 device's data structure
* \param txNcoTestToneCfg Pointer to adi_adrv9025_TxTestToneCfg_t structure that
* contains the configuration for the Tx NCO test tone
* \param arraySize
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxTestToneSet(adi_adrv9025_Device_t*       device,
                                   adi_adrv9025_TxTestToneCfg_t txNcoTestToneCfg[],
                                   uint8_t                      arraySize);

/**
* \brief Read back the Tx NCO test tone
*
*  This function reads the settings for test CW tone on the selected Tx RF output/s.
*
* \param device Pointer to the ADRV9025 device's data structure
* \param txChannel Channel to get the test tone settings
* \param txNcoTestToneCfg Pointer to adi_adrv9025_TxTestToneCfg_t structure to write
* the configuration for the Tx NCO test tone read back from the part
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxTestToneGet(adi_adrv9025_Device_t*        device,
                                   adi_adrv9025_TxChannels_e     txChannel,
                                   adi_adrv9025_TxTestToneCfg_t* txNcoTestToneCfg);

/**
* \brief Configures Attenuation steps and the GPIO inputs for Tx attenuation control
*
* This API function configures the GPIO input pin and step size to allow the
* BBP to control attenuation changes in Tx signal chain.
* A high pulse on the 'txAttenIncPin' in pin control mode will increment the
* gain by the value set in 'stepSize'.
* A high pulse on the 'txAttenDecPin' in pin control mode will decrement the
* gain by the value set in 'stepSize'.
* This function should only be used to change GPIO pins which are currently used for
* Tx attenuation control GPIO mode.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
*
*
* \param txAttenPinCfg Pointer to an array of adi_adrv9025_TxAttenPinCfg_t structure that
* configures the Tx attenuation pin control.
* \param numTxAttenPinConfigs Number of channelized Tx Atten Pin configuration passed in the array txAttenPinCfg
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenPinCtrlCfgSet(adi_adrv9025_Device_t*       device,
                                          adi_adrv9025_TxAttenPinCfg_t txAttenPinCfg[],
                                          uint8_t                      numTxAttenPinConfigs);

/**
* \brief This API function returns the configuration (Gain steps and the GPIO
* inputs) for Tx Attenuation pin control.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannel enum type to select the Tx channel to read the Tx Attenuation pin control
* configuration from.
* \param txAttenPinCfg Pointer to structure that return values will be returned in
* for Tx attenuation pin control.
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenPinCtrlCfgGet(adi_adrv9025_Device_t*        device,
                                          adi_adrv9025_TxChannels_e     txChannel,
                                          adi_adrv9025_TxAttenPinCfg_t* txAttenPinCfg);

/**
* \brief Configures GPIO pin for Tx attenuation simultaneous update of selected Tx channels
* This function should only be used to change GPIO pin which is currently used for Tx attenuation
* simultaneous update.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannelMask Multiple Tx channel selection for configuration
* \param gpioSelection GPIO pin selection for simultaneous Tx attenuation update
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenUpdateGpioPinCtrlCfgSet(adi_adrv9025_Device_t*    device,
                                                    uint32_t                  txChannelMask,
                                                    adi_adrv9025_GpioPinSel_e gpioSelection);

/**
* \brief Read GPIO pin configuration for Tx attenuation simultaneous update of selected Tx channels
*
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannelSelect Tx channel selection to read configuration
* \param gpioSelection Pointer to enum which will be used to readback configuration
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenUpdateGpioPinCtrlCfgGet(adi_adrv9025_Device_t*     device,
                                                    adi_adrv9025_TxChannels_e  txChannelSelect,
                                                    adi_adrv9025_GpioPinSel_e* gpioSelection);

/**
* \brief This function updates Tx attenuation simultaneously for selected Tx channels.
*
*  Update happens from tx attenuation value on spi register into the internal hardware
*  Simultaneous update with this function can be triggered only when attenuation mode is SPI.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannelMask Multiple Tx channel selection to trigger Tx attenuation update
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenUpdate(adi_adrv9025_Device_t* device,
                                   uint32_t               txChannelMask);

/**
* \brief Writes ADRV9025 device registers with settings for the PA Protection feature.
*
* This function sets up the PA Protection functionality and enables Tx sample
* power measurements. It does not enable the ability to change Tx Attenuation
* automatically.
*
* The PA Protection feature allows for error flags to go high if the
* average TX IQ sample power (before interpolation at JESD204 interface) in
* the data path exceeds a programmable threshold level based on samples taken
* in a programmable duration. Also, the peak power measurement mode allows
* flagging an error when the instantaneous power exceeds a threshold more than a
* programmable count value within the average duration.
*
* \pre Complete normal ADRV9025 initialization and init cals before running this function.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* Peak threshold value table for Silicon A
* peakThreshold Value |  dBFS threshold = 10 * Log10(peakThresholdVal/128)
* --------------------|----------------------------------------------------------
*                0    | Invalid
*                1    | -21.07 dBFS
*                2    | -18.06 dBFS
*                3    | -16.30 dBFS
*                4    | -15.05 dBFS
*                5    | -14.08 dBFS
*                6    | -13.29 dBFS
*                7    | -12.62 dBFS
*                8    | -12.04 dBFS
*                ...  | Calculate with EQ above
*                16   | -9.03 dBFS
*                ...  | ...
*                32   | -6.02 dBFS
*                ...  | ...
*                64   | -3.01 dBFS
*                ...  | ...
*                128  | 0 dBFS
*                ...  | ...
*                255  | 3.01 dBFS
*
* Peak threshold value table for Silicon B
* peakThreshold Value |  dBFS threshold = 10 * Log10(peakThresholdVal/8192)
* --------------------|----------------------------------------------------------
*                0    | Invalid
*                1    | -39.12 dBFS
*                2    | -36.11 dBFS
*                3    | -34.35 dBFS
*                4    | -33.10 dBFS
*                5    | -32.13 dBFS
*                6    | -31.34 dBFS
*                7    | -30.67 dBFS
*                8    | -30.09 dBFS
*                ...  | Calculate with EQ above
*                16   | -27.08 dBFS
*                ...  | ...
*                32   | -24.07 dBFS
*                ...  | ...
*                64   | -21.06 dBFS
*                ...  | ...
*                128  | -18.05 dBFS
*                ...  | ...
*                256  | -15.04 dBFS
*                ...  | ...
*                512  | -12.03 dBFS
*                ...  | ...
*                1024 | -9.02 dBFS
*                ...  | ...
*                2048 | -6.01 dBFS
*                ...  | ...
*                8191 |  0 dBFS
*
* Avg Power threshold value table for Silicon A
* powerThreshold Value|  dBFS threshold = 10 * Log10(powerThreshold/4096)
* --------------------|----------------------------------------------------------
*                0    | Invalid
*                1    | -36.12 dBFS
*                2    | -33.11 dBFS
*                3    | -31.35 dBFS
*                4    | -30.10 dBFS
*                5    | -29.13 dBFS
*                6    | -28.34 dBFS
*                7    | -27.67 dBFS
*                8    | -27.09 dBFS
*                ...  | Calculate with EQ above
*                16   | -24.08 dBFS
*                ...  | ...
*                32   | -21.07 dBFS
*                ...  | ...
*                64   | -18.06 dBFS
*                ...  | ...
*                128  | -15.05 dBFS
*                ...  | ...
*                256  | -12.04 dBFS
*                ...  | ...
*                512  | -9.03 dBFS
*                1024 | -6.02 dBFS
*                ...  | ...
*                2048 | -3.01 dBFS
*                ...  | ...
*                8191 |  3 dBFS
*
* Avg Power threshold value table for Silicon B
* powerThreshold Value|  dBFS threshold = 10 * Log10(powerThreshold/8192)
* --------------------|----------------------------------------------------------
*                0    | Invalid
*                1    | -39.12 dBFS
*                2    | -36.11 dBFS
*                3    | -34.35 dBFS
*                4    | -33.10 dBFS
*                5    | -32.13 dBFS
*                6    | -31.34 dBFS
*                7    | -30.67 dBFS
*                8    | -30.09 dBFS
*                ...  | Calculate with EQ above
*                16   | -27.08 dBFS
*                ...  | ...
*                32   | -24.07 dBFS
*                ...  | ...
*                64   | -21.06 dBFS
*                ...  | ...
*                128  | -18.05 dBFS
*                ...  | ...
*                256  | -15.04 dBFS
*                ...  | ...
*                512  | -12.03 dBFS
*                ...  | ...
*                1024 | -9.02 dBFS
*                ...  | ...
*                2048 | -6.01 dBFS
*                ...  | ...
*                8191 |  0 dBFS
*
* \param device Pointer to the device settings structure
* \param txPaProtectCfg array of structure holding the setup values for the Tx PA Protection feature
* \param arraySize number of txChannels to be configured
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxPaProtectionCfgSet(adi_adrv9025_Device_t*        device,
                                          adi_adrv9025_TxPaProtectCfg_t txPaProtectCfg[],
                                          uint8_t                       arraySize);

/**
* \brief Reads ADRV9025 device registers to retrieve settings for the PA Protection feature.
*
* The PA Protection feature allows for error flags to go high if the
* average TX IQ sample power (before interpolation at JESD204 interface) in
* the data path exceeds a programmable threshold level based on samples taken
* in a programmable duration. Also, the peak power measurement mode allows
* flagging an error when the instantaneous power exceeds a threshold more than a
* programmable count value within the average duration.
*
* \pre Complete normal ADRV9025 initialization and init cals before running this function.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannel enum type to select the Tx channel to read the Tx Attenuation pin control
* configuration from.
* \param txPaProtectCfg Structure to return the setup values for the Tx PA Protection feature
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxPaProtectionCfgGet(adi_adrv9025_Device_t*         device,
                                          adi_adrv9025_TxChannels_e      txChannel,
                                          adi_adrv9025_TxPaProtectCfg_t* txPaProtectCfg);

/**
* \brief Read back Tx average IQ sample power (input samples at JESD204 port)
*        based on settings from adi_adrv9025_TxPaProtectionCfgSet()
*
* avgPower and  avgErrorPower return value is not returned in dBFS.  To calculate the dBFS value
* perform the follow calculation:
* Average Power in dBFS = 10 * log10(avgPower/2^16)
* Power Error in dBFS = 10 * log10(avgErrorPower/2^16)
*
* Ratio calculation for avgPeakRatio : avgPower/peakPower = ((avgPeakRatio) / 2^15)
*
* \pre Complete normal ADRV9025 initialization and init cals, and call
*      adi_adrv9025_TxPaProtectionCfgSet() before using adi_adrv9025_TxSamplePowerGet()
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannel enum type to select the Tx channel to read the Tx Attenuation pin control
* configuration from.
* \param status pointer to structure that will be populated with the most recent
* tx average IQ sample power for specified tx channel (linear power with 65536 = 0dB)
* and the avgPeakRatio
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxPaProtectionStatusGet(adi_adrv9025_Device_t*            device,
                                             adi_adrv9025_TxChannels_e         txChannel,
                                             adi_adrv9025_TxPaProtectStatus_t* status);

/**
* \brief Read back Tx PA Protection error IRQ flags
*
* If Tx PA Protection is configured and enabled, the error flags per channel
* can be read back to determine which transmitter exceeded the set peak or
* average power threshold.  The PA protection error flags can also be set
* to assert the GP interrupt pin for immediate feedback to the BBIC when the
* Tx power into the PA is of concern.
*
* \pre Complete normal ADRV9025 initialization and init cals, and call
*      adi_adrv9025_TxPaProtectionCfgSet() first
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannel enum type to select the Tx channel to read the Tx Attenuation pin control
* configuration from.
* \param errorFlags Returns error flag per channel.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxPaProtectionErrFlagsGet(adi_adrv9025_Device_t*            device,
                                               adi_adrv9025_TxChannels_e         txChannel,
                                               adi_adrv9025_TxPaProtectionErr_t* errorFlags);

/**
* \brief Clears PA Protection error IRQ flags for the given channel.
*
* If Tx PA Protection is configured and enabled, the error flags per channel
* can be read back using the adi_adrv9025_TxPaProtectionErrFlagsGet function. The
* error flags will stay asserted until manually cleared if they're set to be sticky
* by using adi_adrv9025_TxAttenuationRampUpStickyModeEnable API function
*
* If the PA protection error flags are sticky, it is preferred to assign Pa protection
* events to any of the GPINT pins to monitor the error status and clear the error flags
* with this function
*
* Calling adi_adrv9025_TxPaProtectionCfgSet() will also clear the error flags at the
* end of the function to make sure PA protection is enabled with the error
* flags in a known state.
*
* \pre Complete normal ADRV9025 initialization and init cals, and call
*      adi_adrv9025_TxPaProtectionCfgSet() first
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannel of which flags are to be cleared.
* \param errorFlags to be cleared.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxPaProtectionErrFlagsReset(adi_adrv9025_Device_t*             device,
                                                 adi_adrv9025_TxChannels_e          txChannel,
                                                 adi_adrv9025_TxPaProtectErrFlags_e errorFlags);

/**
* \brief Configures Attenuation SPI2 GPIO inputs for Tx attenuation control
*
* This API function configures the SPI2 GPIO input pin
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txAttenSpi2PinCfg[] Pointer to an array of adi_adrv9025_TxAttenSpi2PinCfg_t structure that
* configures the Tx attenuation Spi2 pin control.
* \param numTxAttenSpi2PinConfigs Number of channelized Tx Atten Spi2 Pin configuration passed in the array txAttenSpi2PinCfg
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenSpi2PinCtrlCfgSet(adi_adrv9025_Device_t*           device,
                                              adi_adrv9025_TxAttenSpi2PinCfg_t txAttenSpi2PinCfg[],
                                              uint8_t                          numTxAttenSpi2PinConfigs);

/**
* \brief This API function returns the configuration for Tx Attenuation SPi2 pin control.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannel enum type to select the Tx channel to read the Tx configuration from.
* \param txAttenSpi2PinCfg Pointer to structure that return values will be returned in
* for Tx attenuation SPI2 pin control.
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenSpi2PinCtrlCfgGet(adi_adrv9025_Device_t*            device,
                                              adi_adrv9025_TxChannels_e         txChannel,
                                              adi_adrv9025_TxAttenSpi2PinCfg_t* txAttenSpi2PinCfg);

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
* \param txLoSource Pointer to store Rx channel LO source mapping read back (Output)
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxLoSourceGet(adi_adrv9025_Device_t*    device,
                                   adi_adrv9025_TxChannels_e txChannel,
                                   adi_adrv9025_LoSel_e*     txLoSource);

/**
* \brief This function reads back SRL Irq status(read only) of selected Tx channel.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txChannel Enum to select Tx Channel.
* \param enable Pointer to memory location where read back status will be written
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_SrlIrqGet(adi_adrv9025_Device_t*    device,
                               adi_adrv9025_TxChannels_e txChannel,
                               uint8_t*                  enable);

/**
* \brief This function enables/disables SRL interrupts for selected Tx channel.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txChannel Enum to select Tx Channel.
* \param enable 0:Disable SRL interrupts for selected channel / Other:Enable SRL interrupts for selected channel
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_SrlIrqEnableSet(adi_adrv9025_Device_t*    device,
                                     adi_adrv9025_TxChannels_e txChannel,
                                     uint8_t                   enable);

/**
* \brief This function reads whether SRL interrupts are enabled for selected Tx channel.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txChannel Enum to select Tx Channel.
* \param enable Pointer to memory location where readback value will be written
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_SrlIrqEnableGet(adi_adrv9025_Device_t*    device,
                                     adi_adrv9025_TxChannels_e txChannel,
                                     uint8_t*                  enable);

/**
* \brief This function reads SRL statistics which is number of samples which have been
*        slew rate limited(Maximum = 65536 sample count)
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txChannel Enum to select Tx Channel.
* \param clearStats 0:Don't clear statistics after reading / Other: Clear statistics after reading.
* \param statisticsReadBack Pointer to memory location where readback value will be written
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_SrlStatisticsRead(adi_adrv9025_Device_t*    device,
                                       adi_adrv9025_TxChannels_e txChannel,
                                       uint8_t                   clearStats,
                                       uint16_t*                 statisticsReadBack);

/**
* \brief This function sets the configuration of slew rate limiter for tx channels
*        selected within the configuration struct (txChannelMask)
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param slewRateLimiterCfg SRL configuration to be written
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_SrlCfgSet(adi_adrv9025_Device_t*            device,
                               adi_adrv9025_SlewRateLimiterCfg_t slewRateLimiterCfg);

/**
* \brief This function reads the configuration of slew rate limiter of selected Tx channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txChannel Enum to select Tx Channel.
* \param slewRateLimiterCfg Pointer to memory location where the configuration read back value will be written
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_SrlCfgGet(adi_adrv9025_Device_t*             device,
                               adi_adrv9025_TxChannels_e          txChannel,
                               adi_adrv9025_SlewRateLimiterCfg_t* slewRateLimiterCfg);

/**
* \brief This function sets the configuration of slew rate limiter for tx channels
*        selected within the configuration struct (txChannelMask)
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param timerMicroSec SRL ramp timer in microseconds. Min is 80 us and max is 5000000 us (5 seconds).
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_SrlRampTimerSet(adi_adrv9025_Device_t* device,
                                     uint32_t               timerMicroSec);

/**
* \brief This function reads the slew rate limiter ramp timer value.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param timerMicroSec Pointer to memory location where the ramp timer read back value will be written
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_SrlRampTimerGet(adi_adrv9025_Device_t* device,
                                     uint32_t*              timerMicroSec);

/**
* \brief This function enables/disables ramp-down of Tx power in case of a PLL unlock or
*        Dfrm IRQ(0-1) or PA protection events(Average/Peak Power Errors and SRL errors).
*
* There are 10 different events that can trigger Tx power ramp-down.
* Each Tx Channel(1-4) has independent ramp-down configurations. Prior to enabling ramp down
* for Deframer interrupts, user should enable the deframer links. Deframer interrupts should also be
* enabled by using adi_adrv9025_DfrmIrqMaskSet API.
* A powered down PLL will be seen as an unlocked PLL by the device, hence ramping should be enabled
* only for PLL's that are enabled.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txChannelMask Mask to select one or more Tx channels from adi_adrv9025_TxChannels_e.
* \param irqMask Mask to select one or more events to be enabled/disabled.
*                    Bit   |  Description
*             -------------|-----------------------------------------------------------------------------
*                  [8-31]  | Unused
*                  [7]     | PA PROTECTION ERROR - This bit enables/disables Tx power ramp down on Average/Peak power errors and Slew Rate Limiter Errors(SRL IRQ should be enabled)
*                  [6]     | SERDES PLL UNLOCK
*                  [5]     | RF2 PLL UNLOCK
*                  [4]     | RF1 PLL UNLOCK
*                  [3]     | AUX PLL UNLOCK
*                  [2]     | CLK PLL UNLOCK
*                  [1]     | Deframer Irq - 1
*                  [0]     | Deframer Irq - 0
*
* \param enable Byte to enable/disable ramp-down for selected events 0:Disable Other:Enable
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PaPllDfrmEventRampDownEnableSet(adi_adrv9025_Device_t* device,
                                                     uint32_t               txChannelMask,
                                                     uint32_t               irqMask,
                                                     uint8_t                enable);

/**
* \brief This function enables/disables ramp-down of Tx power. The user can set SRL Mode 4 then
* call this function.
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txChannelMask Mask to select one or more Tx channels from adi_adrv9025_TxChannels_e.
* \param enable Byte to enable/disable ramp-down for selected events 0:Disable Other:Enable
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PaProtectionRampDownEnableSet(adi_adrv9025_Device_t* device,
                                                   uint32_t               txChannelMask,
                                                   uint8_t                enable);

/**
* \brief This function to read pa protection ramp-down enable bit.
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txChannelSelect Enum to select Tx channel to read pa protection ramp-down enable bit.
* \param enable Pointer to byte to read pa protection ramp-down enable bit.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PaProtectionRampDownEnableGet(adi_adrv9025_Device_t*    device,
                                                   adi_adrv9025_TxChannels_e txChannelSelect,
                                                   uint8_t*                  enable);

/**
* \brief This function enables/disables ramp-up of Tx power. The user can set SRL Mode 4 then
* call this function.
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txChannelMask Mask to select one or more Tx channels from adi_adrv9025_TxChannels_e.
* \param enable Byte to enable/disable ramp-up for selected events 0:Disable Other:Enable
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PaProtectionRampUpEnableSet(adi_adrv9025_Device_t* device,
                                                 uint32_t               txChannelMask,
                                                 uint8_t                enable);

/**
* \brief This function to read pa protection ramp-up enable bit.
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txChannelSelect Enum to select Tx channel to read pa protection ramp-up enable bit.
* \param enable Pointer to byte to read pa protection ramp-up enable bit.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PaProtectionRampUpEnableGet(adi_adrv9025_Device_t*    device,
                                                 adi_adrv9025_TxChannels_e txChannelSelect,
                                                 uint8_t*                  enable);

/**
* \brief This function reads the status of Tx power ramp-down functionality for PLL Unlock
*        and Dfrm IRQ(0-1) and PA protection events(Average/Peak Power Errors and SRL errors).
*
* There are 10 different events that can trigger Tx power ramp-down.
* Each Tx Channel(1-4) has independent ramp-down configurations.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txChannelSelect Enum to select Tx channels to read power ramp-down enabled status.
* \param irqSelect Enum to select the event to read power ramp-down enabled status.
* \param enable Pointer to byte to read power ramp-down enabled status.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PaPllDfrmEventRampDownEnableGet(adi_adrv9025_Device_t*                device,
                                                     adi_adrv9025_TxChannels_e             txChannelSelect,
                                                     adi_adrv9025_PaPllDfrmRampDownEnSel_e irqSelect,
                                                     uint8_t*                              enable);

/**
* \brief This function reads the status of events causing Tx power ramp down
*
* Each Tx Channel(1-4) has independent ramp-down event status bits.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txChannelSelect Enum to select Tx channels to read event bits
* \param eventBits
*                    Bit   |  Description
*             -------------|-----------------------------------------------------------------------------
*                  [3-7]   | Unused
*                  [2]     | Pll Unlock / Dfrm Error (B0 Only)
*                  [1]     | Pa Protection Peak Power Error
*                  [0]     | Pa Protection Average Power Error
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PaPllDfrmEventGet(adi_adrv9025_Device_t*    device,
                                       adi_adrv9025_TxChannels_e txChannelSelect,
                                       uint8_t*                  eventBits);

/**
* \brief This function will clear the sticky bits of selected events causing Tx power ramp down
*
* Each Tx Channel(1-4) has independent ramp-down event sticky bits.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txChannelSelect Enum to select Tx channels to clear sticky event bits
* \param eventBits
*                    Bit   |  Description
*             -------------|-----------------------------------------------------------------------------
*                  [3-7]   | Unused
*                  [2]     | Pll Unlock / Dfrm Error Sticky Bit (B0 Only)
*                  [1]     | Pa Protection Peak Power Error Sticky Bit
*                  [0]     | Pa Protection Average Power Error Sticky Bit

* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PaPllDfrmEventClear(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_TxChannels_e txChannelSelect,
                                         uint8_t                   eventBits);

/**
* \brief This function configures Tx attenuation ramp up sticky mode for selected Tx channel. If
* an event is configured as non-sticky, Tx power will ramp up automatically once the error is
* recovered. To allow ramp-up in case of jesd events, user should manually clear the interrupt with
* adi_adrv9025_DfrmIrqSourceReset API, even if Pll/Jesd event is non-sticky.
* If an event is configured as sticky, user should use adi_adrv9025_PaPllDfrmEventClear API to clear
* the error and to allow ramp-up.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param channelMask Tx channel selection to enable Tx attenuation ramp up and sticky mode
* \param txPllJesdProtClrReqd Clear required for pll and jesd gain ramp down events 0-disable, 1-enable. This bit
*        should always be 1
* \param txPaProtectionAvgpowerErrorClearRequired PA Protection avg power error clearing required 0-disable, 1-enable
* \param txPaProtectionPeakpowerErrorClearRequired PA Protection peak power error clearing required 0-disable, 1-enable
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenuationRampUpStickyModeEnable(adi_adrv9025_Device_t* device,
                                                         uint32_t               channelMask,
                                                         uint8_t                txPllJesdProtClrReqd,
                                                         uint8_t                txPaProtectionAvgpowerErrorClearRequired,
                                                         uint8_t                txPaProtectionPeakpowerErrorClearRequired);

/**
* \brief This function configures Tx NCO for two modes of operation.
* 1- Inject a tone at the specified frequency and gain.
* 2- Mix the signal up or down the band by the specified frequency amount.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param shifterCfg Tx NCO configuration container
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxNcoShifterSet(adi_adrv9025_Device_t*         device,
                                     adi_adrv9025_TxNcoShifterCfg_t shifterCfg);

/**
* \brief This function retrieves the Tx NCO configuration for the specified Tx Channel.
* Note:  The Tx Channel is specified by setting the txChannelMask member of the adi_adrv9025_TxNcoShifterCfg_t
* structure, whose pointer is passed to the function, with bitmask of the desired channel to get. This txChannelMask
* member is over written by the value of the actual txChannel from which the returned configuration was read.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param shifterCfg Pointer to Tx NCO configuration container
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxNcoShifterGet(adi_adrv9025_Device_t          *device,
                                     adi_adrv9025_TxNcoShifterCfg_t *shifterCfg);

/**
* \brief This function is to set the Tx Analog Attenuation Early Delay Counter.
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txChannelMask Mask to select one or more Tx channels from adi_adrv9025_TxChannels_e.
* \param delayValue Byte value of delay counter.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAnalogAttenEarlyDelaySet(adi_adrv9025_Device_t* device,
                                                uint32_t               txChannelMask,
                                                uint8_t                delayValue);

/**
* \brief This function to read the Tx Analog Attenuation Early Delay Counter.
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param txChannelSelect Enum to select Tx channel to read pa protection ramp-down enable bit.
* \param delayValue Pointer to byte to read value of delay counter.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAnalogAttenEarlyDelayGet(adi_adrv9025_Device_t*    device,
                                                adi_adrv9025_TxChannels_e txChannelSelect,
                                                uint8_t*                  delayValue);

/****************************************************************************
* Helper functions
****************************************************************************
*/

/****************************************************************************
* Debug functions
****************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_TX_H_ */
