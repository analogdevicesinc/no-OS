/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adi_adrv9025_rx.h
 * \brief Contains ADRV9025 receive related function prototypes for
 *        adi_adrv9025_rx.c
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_RX_H_
#define _ADI_ADRV9025_RX_H_

#include "adi_adrv9025_types.h"
#include "adi_adrv9025_rx_types.h"
#include "adi_common_error_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

/**
* \brief Programs the gain table settings for Rx / ORx channels.
*
* This function can be called by the user to load a custom gain table or
* to reconfigure the gain table.The gain table for a receiver type is set with the
* parameters passed by adi_adrv9025_RxGainTableRow_t gainTablePtr array.
* The array length (n) is dependent upon receiver type.
* The (n) value is conveyed by numGainIndicesInTable.
* All gain tables have a maximum index and a minimum index specified by
* MAX_RX_GAIN_TABLE_NUMINDICES and MIN_RX_GAIN_TABLE_INDEX
* The minimum gain index is application dependent, this can be modified
* in the user space, the absolute maximum and minimum indices are specified
* by MAX_GAIN_TABLE_INDEX and MIN_GAIN_TABLE_INDEX
*
* The Rx max gain index is user configurable. A separate call has to be made
* to adi_adrv9025_RxMinMaxGainIndexSet() API to update the min and max gain
* indices for a given Rx Channel. Updating min and max gain indices are
* decoupled from the main gain table loading so that the user has flexibility
* to load multiple gain table regions and switch between them during runtime.
*
* The gain table configs can be broadcast / multicast based on the channel mask
* parameter in adi_adrv9025_RxGainTableCfg_t structure.
*
*                        rxChannelMask | Rx Channels Programmed
*     ---------------------------------|--------------------------------------
*                         bit[0] = 1   |  Enables Rx1 gain table programming
*                                      |
*                         bit[1] = 1   |  Enables Rx2 gain table programming
*                                      |
*                         bit[2] = 1   |  Enables Rx3 gain table programming
*                                      |
*                         bit[3] = 1   |  Enables Rx4 gain table programming
*                                      |
*                         bit[4] = 1   |  Enables ORx1 gain table programming
*                                      |
*                         bit[5] = 1   |  Enables ORx2 gain table programming
*                                      |
*                         bit[6] = 1   |  Enables ORx3 gain table programming
*                                      |
*                         bit[7] = 1   |  Enables ORx4 gain table programming
*
*     Eg: To program the same gain table to channels ORx1 and Rx1, the rxChannelMask
*         should be set to 0x00000011
*
*     Eg: To program a single gain table to all channels, the rxChannelMask
*         should be set to 0x0000003F
*
*  Partial gain table loads can be done through this API in case of memory constraints / multiple region loading
*  For example, consider a 256 row gain table which needs to be loaded in 4 consecutive calls.
*  In this case the config parameters for partial loads would be
*  Partial Load 1 : gainTableRow[] = gainTable[63:0], gainIndexOffset = 63, numGainIndicesInTable = 64
*  Partial Load 2 : gainTableRow[] = gainTable[127:64], gainIndexOffset = 127, numGainIndicesInTable = 64
*  Partial Load 3 : gainTableRow[] = gainTable[191:128], gainIndexOffset = 191, numGainIndicesInTable = 64
*  Partial Load 4 : gainTableRow[] = gainTable[255:192], gainIndexOffset = 255, numGainIndicesInTable = 64
*
*  Post this multiple partial gain table load, call the function adi_adrv9025_RxMinMaxGainIndexSet(minIndex = 0, maxIndex = 255).
*
*  Please note that ORx1 and ORx2 are physically mapped to the same ORx channel and ORx3 and ORx4 are physically mapped
*  to the same ORx channel. Selecting ORx1 / ORx2 writes the same ORx gain table SRAM and similarly selecting ORx3 or ORx4
*  writes to the same ORx gain table SRAM.
*
* \pre This function called automatically in adi_adrv9025_Initialize() to load
*      the default gain table. If the BBIC desires to change or update the
*      gain table, it may call this function after initialization but before
*      running init cals.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannelMask is the set of channels for which current gain table settings are to be programmed.
*        Valid Rx channels include Rx1-Rx4 and ORx1-ORx4
* \param gainIndexOffset is the starting gain index from which gain table is programmed
* \param gainTableRow Array of gain table row entries for programming
* \param arraySize is the number of gain table rows to be programmed
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainTableWrite(adi_adrv9025_Device_t*        device,
                                      uint32_t                      rxChannelMask,
                                      uint8_t                       gainIndexOffset,
                                      adi_adrv9025_RxGainTableRow_t gainTableRow[],
                                      uint16_t                      arraySize);

/**
* \brief Performs range check on the gain table settings for Rx / ORx channels.
*
* This function called automatically as part of adi_adrv9025_RxGainTableWrite() API
* if ADI_ADRV9025_RX_RANGE_CHECK is set to 1. This function can be called independently
* for standalone range checking of Rx Gain Table write functionality.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannelMask is the set of channels for which current gain table settings are to be programmed.
*        Valid Rx channels include Rx1-Rx4 and ORx1-ORx4
* \param gainIndexOffset is the starting gain index from which gain table is programmed
* \param gainTableRow Array of gain table row entries for programming
* \param arraySize is the number of gain table rows to be programmed
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainTableWriteRangeCheck(adi_adrv9025_Device_t*        device,
                                                uint32_t                      rxChannelMask,
                                                uint8_t                       gainIndexOffset,
                                                adi_adrv9025_RxGainTableRow_t gainTableRow[],
                                                uint16_t                      arraySize);

/**
* \brief Reads the gain table entries for Rx / ORx channels requested.
*
* This function can be called by the user to read back the currently programmed gain table
* for a given channel. This function reads the current gain table settings from ADRV9025 gain table Static RAMs
* for the requested channel and stores it in the provided memory reference of type adi_adrv9025_RxGainTableCfg_t
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannel represents the channels for which gain table read back is requested.
*        Valid Rx channels include Rx1-Rx4 and ORx1-ORx4
* \param gainIndexOffset is the gain index from which gain table read back should start
* \param gainTableRow Read back array for gain table row entries which will be updated with the read back values
* \param arraySize is the size of gainTableRow array. It is also the upper limit for the no. of gain indices to read
* \param numGainIndicesRead is the actual no. of gain indices read from SRAM (output). A NULL can be passed
*        if the value of no. of gain indices actually read is not required.
*
* \pre This function can be called by the user anytime after initialization.
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainTableRead(adi_adrv9025_Device_t*        device,
                                     adi_adrv9025_RxChannels_e     rxChannel,
                                     uint8_t                       gainIndexOffset,
                                     adi_adrv9025_RxGainTableRow_t gainTableRow[],
                                     uint16_t                      arraySize,
                                     uint16_t*                     numGainIndicesRead);

/**
* \brief Performs range check on adi_adrv9025_RxGainTableRead() function
*
* This function called automatically as part of adi_adrv9025_RxGainTableRead() API
* if ADI_ADRV9025_RX_RANGE_CHECK is set to 1. This function can be called independently
* for standalone range checking of Rx Gain Table read functionality.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannel represents the channels for which gain table read back is requested.
*        Valid Rx channels include Rx1-Rx4 and ORx1-ORx4
* \param gainIndexOffset is the gain index from which gain table read back should start
* \param gainTableRow Read back array for gain table row entries which will be updated with the read back values
* \param arraySize is the size of gainTableRow array. It is also the upper limit for the no. of gain indices to read
* \param numGainIndicesRead is the actual no. of gain indices read from SRAM (output). A NULL can be passed
*        if the value of no. of gain indices actually read is not required.
*
* \pre This function can be called by the user anytime after initialization.
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainTableReadRangeCheck(adi_adrv9025_Device_t*        device,
                                               adi_adrv9025_RxChannels_e     rxChannel,
                                               uint8_t                       gainIndexOffset,
                                               adi_adrv9025_RxGainTableRow_t gainTableRow[],
                                               uint16_t                      arraySize,
                                               uint16_t*                     numGainIndicesRead);

/**
* \brief Updates the minimum and maximum gain indices for a requested Rx/ORx Channel
*        in the device data structure
*
* This function is required to be called by the user right after loading the gain table.
* This function is decoupled from the gain table loading function so that a user has
* flexibility to load multiple gain table regions and switch between multiple gain table
* regions during runtime
*
* The gain table configs can be broadcast / multicast based on the channel mask
* parameter in adi_adrv9025_RxGainTableCfg_t structure.
*
*                        rxChannelMask | Rx Channel min & max gain indices updated
*     ---------------------------------|--------------------------------------------
*                         bit[0] = 1   |  Updates Rx1 min & max gain indices
*     ---------------------------------|--------------------------------------------
*                         bit[1] = 1   |  Updates Rx2 min & max gain indices
*     ---------------------------------|--------------------------------------------
*                         bit[2] = 1   |  Updates Rx3 min & max gain indices
*     ---------------------------------|--------------------------------------------
*                         bit[3] = 1   |  Updates Rx4 min & max gain indices
*     ---------------------------------|--------------------------------------------
*                         bit[4] = 1   |  Updates ORx1 min & max gain indices
*     ---------------------------------|--------------------------------------------
*                         bit[5] = 1   |  Updates ORx2 min & max gain indices
*     ---------------------------------|--------------------------------------------
*                         bit[6] = 1   |  Updates ORx3 min & max gain indices
*     ---------------------------------|--------------------------------------------
*                         bit[7] = 1   |  Updates ORx4 min & max gain indices
*
*     Eg: To update min and max gain indices of channels ORx1 and Rx1, the rxChannelMask
*         should be set to 0x00000011
*
*     Eg: To update min and max gain indices of all channels(Rx + ORx), the rxChannelMask
*         should be set to 0x0000003F
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannelMask channel masks for which min and max gain indices are to be updated
* \param minGainIndex is the lower limit of the gain index for a given channel
* \param maxGainIndex is the upper limit of the gain index for a given channel
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxMinMaxGainIndexSet(adi_adrv9025_Device_t* device,
                                          uint32_t               rxChannelMask,
                                          uint8_t                minGainIndex,
                                          uint8_t                maxGainIndex);

/**
* \brief Performs range check on the Rx channel min and max gain indices to be updated
*
* This function is called automatically as part of adi_adrv9025_RxMinMaxGainIndexSet()
* if the ADI_ADRV9025_RX_RANGE_CHECK is set to 1. This function can be called independently
* to perform range checks only on parameters for adi_adrv9025_RxMinMaxGainIndexSet() API
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannelMask channel masks for which min and max gain indices are to be updated
* \param minGainIndex is the lower limit of the gain index for a given channel
* \param maxGainIndex is the upper limit of the gain index for a given channel
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxMinMaxGainIndexSetRangeCheck(adi_adrv9025_Device_t* device,
                                                    uint32_t               rxChannelMask,
                                                    uint8_t                minGainIndex,
                                                    uint8_t                maxGainIndex);

/**
 * \brief Programs the Dualband external LNA control gain table settings for
 * either Rx1, Rx2, Rx1 + Rx2 receiver types
 * \brief Not currently implemented
 *
 * The gain table for a receiver type is set with the parameters passed by
 * adi_adrv9025_DualBandLnaGtRow_t gainTablePtr array. The array length (n) is
 * conveyed by numGainIndicesInTable. The table can have a maximum of 4
 * entries. The first entry in the array corresponds to entry 0, second to
 * entry 1, and so on.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep{device->devStateInfo}
 * \dep_end
 *
 * \param device Structure pointer to the ADRV9025 device data structure
 * \param gainTableCfg[] Pointer to Dual Band LNA gain table array of adi_adrv9025_DualBandLnaGtRow_t structures
 * \param arraySize is the size of gainTableCfg array.
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_RxGainTableDualBandLnaWrite(adi_adrv9025_Device_t*                device,
                                                 adi_adrv9025_DualBandRxGainTableCfg_t gainTableCfg[],
                                                 uint8_t                               arraySize);

/* TODO: Needs doxygen header */
/*\brief Not currently implemented */

int32_t adi_adrv9025_RxGainTableDualBandLnaRead(adi_adrv9025_Device_t*                 device,
                                                adi_adrv9025_RxChannels_e              rxChannel,
                                                adi_adrv9025_DualBandRxGainTableCfg_t* gainTableCfg);

/**
* \brief Configures one or more RX FIR filters in the device
*
* The device stores 1 RX FIR filter per channel.
* The device stores an independent filter for each ORx front end (ORx1, ORx2, ORx3, ORx4).
* Internal Loopback 1 and 2 share a FIR filter, and Internal loopback 3 and 4 share a FIR filter.
* Rx and ORx channel can have 24, 48, 72 taps.
*
* The function is not called directly. It is called as a part of the adi_adrv9025_initialize() function.
* The function could be used to change A FIR filter later, but would require calibrations to be rerun.
*
* \pre This function is called during device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
* \param device Pointer to the ADRV9025 data structure
* \param rxChanMask One or more RX channel from adi_adrv9025_RxChannels_e.
* \param gain_dB The gain_dB value (-12, -6, 0, 6)
* \param numFirCoefs The number of Fir Coefs in coefs array (24, 48, 72)
* \param coefs[] Pointer to coefs array
* \param arraySize The size of coefs array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
* \
*/
int32_t adi_adrv9025_RxFirWrite(adi_adrv9025_Device_t* device,
                                uint32_t               rxChanMask,
                                int8_t                 gain_dB,
                                uint8_t                numFirCoefs,
                                int16_t                coefs[],
                                uint8_t                arraySize);

/**
* \brief Get one RX FIR filters in the device
*
* The device stores up to 1 RX FIR filter banks.
* The device stores up to 3 FIR Filter banks A,B,or C for ORX and LOOPBACK
* Rx and ORx channel can have 24, 48, 72 taps.
*
* The function is not called directly. It is called as a part of the adi_adrv9025_initialize() function.
* The function could be used to change A FIR filter later, but would require calibrations to be rerun.
*
* \pre This function is called during device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
* \param device Pointer to the ADRV9025 data structure
* \param rxChannel adi_adrv9025_RxChannels_e enum type to select.
* \param gain_dB Pointer to the gain_dB value read from device (output)
* \param numFirCoefs Pointer to the numFirCoefs value read from device (output)
* \param coefs[] Pointer to coefs array read from the device (output)
* \param arraySize The size of coefs array
*
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
* \
*/
int32_t adi_adrv9025_RxFirRead(adi_adrv9025_Device_t*    device,
                               adi_adrv9025_RxChannels_e rxChannel,
                               int8_t*                   gain_dB,
                               uint8_t*                  numFirCoefs,
                               int16_t                   coefs[],
                               uint8_t                   arraySize);

/****************************************************************************
 * Runtime functions
 ****************************************************************************
 */

/**
* \brief Sets the Rx1 or Rx2 Manual Gain Index
*
* If the value passed in the gainIndex parameter is within range of the gain
* table minimum and maximum indices, the Rx channel gain index will be written
* to the transceiver. Else, an error will be returned. The maximum index is 255
* and the minimum index is application specific.
*
* The default gain table can take values between 0xB7 and 0xFF,
* even though every index is accessible from 0x00 to 0xFF.
*
* \pre This function may be called any time after device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param rxGain is the channelized gain setting parameter
* \param arraySize Size of rxGain array representing no. of configs.
*
* The gain table configs can be broadcast / multicast based on the channel mask
* parameter in adi_adrv9025_RxGainTableCfg_t structure.
*
*             rxGain[i]->rxChannelMask | Rx Channels to set manual gain
*     ---------------------------------|--------------------------------------
*                              bit[0]  |  1 = Rx1 manual gain is updated
*                                      |
*                              bit[1]  |  1 = Rx2 manual gain is updated
*                                      |
*                              bit[2]  |  1 = Rx3 manual gain is updated
*                                      |
*                              bit[3]  |  1 = Rx4 manual gain is updated
*                                      |
*                              bit[4]  |  1 = ORx1 manual gain is updated
*                                      |
*                              bit[5]  |  1 = ORx2 manual gain is updated
*                                      |
*                              bit[6]  |  1 = ORx3 manual gain is updated
*                                      |
*                              bit[7]  |  1 = ORx4 manual gain is updated
*
*     Eg: To update manual gain for channels Rx4 and Rx1, the rxGain[i]->rxChannelMask
*         should be set to 0x00000009
*
*     Eg: To update manual gain for all Rx channels, the rxGain[i]->rxChannelMask
*         should be set to 0x0000000F
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainSet(adi_adrv9025_Device_t* device,
                               adi_adrv9025_RxGain_t  rxGain[],
                               uint8_t                arraySize);

/**
* \brief Performs range check on rx gain index setting.
*
* \pre This function is called automatically as part of adi_adrv9025_RxGainSet()
*      function if ADI_ADRV9025_RX_RANGE_CHECK > 0.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param rxGain is the channelized gain setting parameter
* \param arraySize Size of rxGain array representing no. of configs.
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainSetRangeCheck(adi_adrv9025_Device_t* device,
                                         adi_adrv9025_RxGain_t  rxGain[],
                                         uint8_t                arraySize);

/**
* \brief Reads the Rx MGC Gain Index for the requested Rx channel
*
* This function reads the gain index for the passed channel Rx1-Rx4, ORx1-ORx4
*
* This function reads back the Manual gain index per channel, depending on the gain control mode.
*
* \pre This function may be called any time after device initialization.
*  However, gain indices are tracked only after the device goes into a
*  Receiver mode.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param rxChannel Channel selection to read back gain index for (Valid Rx1-Rx4, ORx1-ORx4 only)
* \param rxGain Pointer to the specified channel gain index value
*        which will be updated by this function
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxMgcGainGet(adi_adrv9025_Device_t*    device,
                                  adi_adrv9025_RxChannels_e rxChannel,
                                  adi_adrv9025_RxGain_t*    rxGain);

/**
* \brief Reads the Rx AGC Gain Index for the requested Rx channel
*
* This function reads the gain index for the passed channel Rx1-Rx4, ORx1-ORx4
*
* This function reads back the AGC gain index per channel, depending on the gain control mode.
*
* @note: That this function taps into the agc_gain_index register which operates in two modes
*  depending on if agc_enable_gain_index_update is set to 0 or 1
*       if agc_enable_gain_index_update == 0: This register will latch to new gain index values
*               automatically.
*       if agc_enable_gain_index_update == 1: This register will require a dummy write in
*               order to latch to the correct gain index.  Furthermore in this mode, the
*               corresponding Rx/ORx channels MUST be enabled for the register to latch to the
*               correct gain index.
*
* \pre This function may be called any time after device initialization.
*  However, gain indices are tracked only after the device goes into a
*  Receiver mode.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param rxChannel Channel selection to read back gain index for (Valid Rx1-Rx4, ORx1-ORx4 only)
* \param rxGain Pointer to the specified channel gain index value
*        which will be updated by this function
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainGet(adi_adrv9025_Device_t*    device,
                               adi_adrv9025_RxChannels_e rxChannel,
                               adi_adrv9025_RxGain_t*    rxGain);

/**
* \brief Performs range check on AGC/MGC gain get API.
*
* \pre This function is called automatically as part of AGC/MGC gain get API.
*      function if ADI_ADRV9025_RX_RANGE_CHECK > 0.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param rxChannel Channel selection to read back gain index for (Valid Rx1-Rx4, ORx1-ORx4 only)
* \param rxGain Pointer to the specified channel gain index value
*        which will be updated by this function
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainGetRangeCheck(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_RxChannels_e rxChannel,
                                         adi_adrv9025_RxGain_t*    rxGain);

/**
* \brief Configures the Rx gain control mode
*
* \brief The Gain Ctrl Mode for Rx signal can be set to Manual Gain Ctrl, Fast attack AGC,
*        Slow Loop AGC, Hybrid mode for channels Rx1-Rx4, ORx1-ORx4.
*
* The ADRV9025 device defaults to Manual Gain Control (MGC) mode on power-up.
* This API is used to reconfigure the Rx Gain Control Mode.
*
* \pre This function should be called before Rx operation is initiated.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param gainMode Desired gain mode to be configured. Parameter is of enumerated type adi_adrv9025_RxAgcMode_e
* \param arraySize is the no. of elements in gainMode[] array
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainCtrlModeSet(adi_adrv9025_Device_t*   device,
                                       adi_adrv9025_RxAgcMode_t gainMode[],
                                       uint8_t                  arraySize);

/**
* \brief Retrieves the currently configured Rx gain control mode
*
* \brief The Gain Ctrl Mode for Rx signal can be set to Manual Gain Ctrl, Fast attack AGC,
*        Slow Loop AGC, Hybrid mode for channels Rx1-Rx4, ORx1-ORx4. This API is used to
*        read back the current Rx gain ctrl mode setting.
*
* \pre
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param rxChannel is the receiver for which gain ctrl mode read back is requested
* \param gainMode is the pointer to adi_adrv9025_RxAgcMode_t structure which will
*        be updated with the read back config
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainCtrlModeGet(adi_adrv9025_Device_t*    device,
                                       adi_adrv9025_RxChannels_e rxChannel,
                                       adi_adrv9025_RxAgcMode_t* gainMode);

/**
* \brief Range Check for adi_adrv9025_RxDataFormatSet function.
*
* The ADRV9025 data formatting should be used in conjunction with the
* gain compensation feature. This function is automatically called as
* part of adi_adrv9025_RxDataFormatSet() if ADI_ADRV9025_RX_RANGE_CHECK
* is set to 1
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param rxDataFormat Pointer to the Rx data format configuration structure
* \param arraySize Size of rxDataFormat array representing no. of configs
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxDataFormatSetRangeCheck(adi_adrv9025_Device_t*      device,
                                               adi_adrv9025_RxDataFormat_t rxDataFormat[],
                                               uint8_t                     arraySize);

/**
* \brief Sets the Rx Data format to integer or floating point format.
*
* The slicer/formatter work in conjunction with the gain compensation module.
* The digital gain compensation module compensates for the front end attenuation
* thereby increasing the bit resolution of the Rx data word. The slicer is then
* applied to attenuate the data such that it can fit into the JESD data path resolution.
*
* The function allows selecting the Rx data format (integer or floating point)
* and allows configuring the various data formatting options.The ADRV9025 defaults
* to integer format.
*
* The floating point format can be set for one of several 16 bit floating
* point formats.  The user can select the number of exponent and significand
* bits.  It is not possible to represent every possible sample value with
* 16bit floating point format. When using integer formatting, the number of slicer
* bits as well as their position in the data word are selectable.  The GPIO bits are
* also user selectable for external slicer control mode.
*
* The user can select the data format mode from one of the following
*
*            rxDataFormat[i]->formatSelect		|  Description
*  ---------------------------------------------|--------------------------------------
*       ADI_ADRV9025_GAIN_COMPENSATION_DISABLED	| Disables gain compensation and Rx data formatting
*												|
*         ADI_ADRV9025_GAIN_WITH_FLOATING_POINT | Setup formatting for one of the following
*												| floating point modes. 5/3/2 exp bits, 10/12/13
*												| significand bits and 1 sign bit.
*												|
* ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO | Setup Rx data format to an integer format such
*												| that the gain compensation data(slicer) is embedded
*												| in the I and Q data. It is possible to embed 2/3/4
*												| slicer bits providing a dynamic gain compensation range
*												| of 50dB. Applicable for 16-bit / 12-bit resolution
*												|
*        ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER | Setup Rx data format to an integer format such
*												| that the gain compensation data(slicer) is encoded
*												| in the GPIO output from ADRV9025. It is possible to encode 2/3/4
*												| slicer bits providing a dynamic gain compensation range
*												| of 50dB. Applicable for 16-bit / 12-bit resolution
*												|
*        ADI_ADRV9025_GAIN_WITH_EXTERNAL_SLICER | Setup Rx data format to an integer format such
*												| that the gain compensation data(slicer) is input
*												| to ADRV9025 via GPIO pins. It is possible to encode 3
*												| slicer bits for 16 bit resolution and 2 slicer bits for
*												| 12 bit resolution.
*                                               |
*      ADI_ADRV9025_EMBED_OVERLOAD_MONITOR_DATA | Setup overload indicators to be embedded in the 16 bit
*                                               | 2s complement data. The overload indicators can be embedded
*                                               | at LSB and/or LSB+1 positions of I and Q samples.
*
* The ADRV9025 data formatting should be used in conjunction with the
* gain compensation feature.
*
* Please note that for ORx channels ADI_ADRV9025_GAIN_WITH_FLOATING_POINT and ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO
* are the only valid modes
*
* Please note that while disabling gain compensation a valid data format represented by
* rxDataFormat[i].integerConfigSettings.intSampleResolution has to be set to a valid mode. Please refer to enum
* adi_adrv9025_RxIntSampleResolution_e for valid intSampleResolution modes
*
* It has to be ensured that the ADRV9025 JESD Framer Np for the selected Rx channel is at least equal to
* integer sample resolution size.
*
* In a 2 bit embedded slicer mode (ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO), slicer position bit 0
* is embedded in Q data from JESD and slicer position bit 1 is embedded in I data from JESD.
*
* In a 4 bit embedded slicer mode (ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO), slicer position bit 0
* and bit1 are embedded in Q data from JESD and slicer position bit 2 and bit 3 are embedded in I data from JESD.
*
* The slicer position can also be read back via adi_adrv9025_RxSlicerPositionGet() API
*
* The slicer position in the internal slicer mode (ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER) can be observed on
* the following GPIO lines which are configured as outputs from ADRV9025
*
*              rxChannel | ADRV9025 GPIOs set to internal slicer ctrl
*   ---------------------|-------------------------------------------------------------------------
*    ADI_ADRV9025_RX1    |  ADRV9025_GPIO_06, ADRV9025_GPIO_07, ADRV9025_GPIO_08 (MSB)
*    ADI_ADRV9025_RX2    |  ADRV9025_GPIO_09, ADRV9025_GPIO_10, ADRV9025_GPIO_11 (MSB)
*    ADI_ADRV9025_RX3    |  ADRV9025_GPIO_12, ADRV9025_GPIO_13, ADRV9025_GPIO_14 (MSB)
*    ADI_ADRV9025_RX4    |  ADRV9025_GPIO_15, ADRV9025_GPIO_16, ADRV9025_GPIO_17 (MSB)
*
* Please note that only the least significant 3 bits of the slicer control word is output on the GPIOs.
*
* User needs to enable fast recovery and configure AGC block with adi_adrv9025_AgcCfgSet API before using
* HB2 int0/int1 threshold and counter exceeded embedded overload flags

* \pre Slicer/Formatter operation requires that the device be loaded with a gain table that compensates
*      for the analog front end attenuation applied.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param rxDataFormat Pointer to the Rx data format configuration structure
* \param arraySize Size of rxDataFormat array representing no. of configs
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxDataFormatSet(adi_adrv9025_Device_t*      device,
                                     adi_adrv9025_RxDataFormat_t rxDataFormat[],
                                     uint8_t                     arraySize);

/**
* \brief Retrieve the Rx data path format configuration.
* \brief Not currently implemented
*
* Use this command to retrieve the Rx data path data formatting configuration
* set by the adi_adrv9025__RxDataFormatSet function.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param rxChannel adi_adrv9025_RxChannels_e enum type to select channel for reading configuration from.
* \param rxDataFormat Pointer to the Rx data format configuration structure
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxDataFormatGet(adi_adrv9025_Device_t*       device,
                                     adi_adrv9025_RxChannels_e    rxChannel,
                                     adi_adrv9025_RxDataFormat_t* rxDataFormat);

/**
* \brief Get the gain slicer position bits for both Rx1 and Rx2.
*
* Before this information is valid, the Rx gain compensation feature should be enabled with
* the integer gain slicer sample format enabled.  See the adi_adrv9025_RxDataFormatSet() function
* to enable gain compensation. This function is not useful in the case of using floating point
* as the gain slicer (data scaling) is applied in the floating point scaled sample. This function
* is also not valid if the selected data format is external slicer mode
*
* The slicer position is only needed for integer 12bit and 16bit formats.  The 24bit sample format
* already applies scaling in the 24bit sample. The data can be scaled in steps of 1dB, 2dB, 4dB,
* 6dB, 8dB configured through the adi_adrv9025_RxDataFormatSet() function.
*
* Slicer Position[2:0] | Description
* ---------------------|---------------------------------------------
*                0     | 0dB gain
*                1     | Rx Data samples should be scaled by 1 x step size(dB)
*                2     | Rx Data samples should be scaled by 2 x step size(dB)
*                3     | Rx Data samples should be scaled by 3 x step size(dB)
*                4     | Rx Data samples should be scaled by 4 x step size(dB)
*                5     | Rx Data samples should be scaled by 5 x step size(dB)
*                6     | Rx Data samples should be scaled by 6 x step size(dB)
*                7     | Rx Data samples should be scaled by 7 x step size(dB)
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param rxChannel adi_adrv9025_RxChannels_e enum type to select channel from.
* \param slicerPosition Pointer to byte which returns the slicer position bits for the requested channel.
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxSlicerPositionGet(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_RxChannels_e rxChannel,
                                         uint8_t*                  slicerPosition);

/**
* \brief Configures Gain steps and the GPIO inputs for Manual Rx gain control
*
* This API function configures the GPIO input pin and step size to allow the
* BBP to control gain changes in Rx signal chain.
* A high pulse on the 'rxGainIncPin' in pin control mode will increment the
* rx gain index by the value set in 'rxGainPinCtrlCfg.incStep' + 1.
* A high pulse on the 'rxGainDecPin' in pin control mode will decrement the
* rx gain index by the value set in 'rxGainPinCtrlCfg.decStep' + 1.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param rxChannel enum to select channel Rx channel for gpio gain ctrl config
* \param rxGainPinCtrlCfg Pointer to adi_adrv9025_RxGainPinCfg_t structure
*        containing configuration values for Manual Rx gain pin control
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainPinCtrlCfgSet(adi_adrv9025_Device_t*       device,
                                         adi_adrv9025_RxChannels_e    rxChannel,
                                         adi_adrv9025_RxGainPinCfg_t* rxGainPinCtrlCfg);

/**
* \brief Performs a range check on the GPIO mode gain control config
*
* This API function checks that the GPIO pins are in the range GPIO[15:0]
* This API also verifies that increment and decrement steps are in the range 0-7
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param rxChannel enum to select channel Rx channel for gpio gain ctrl config
* \param rxGainPinCtrlCfg Pointer to adi_adrv9025_RxGainPinCfg_t structure
*        containing configuration values for Manual Rx gain pin control
*        on which range check will be performed
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainPinCtrlCfgSetRangeCheck(adi_adrv9025_Device_t*       device,
                                                   adi_adrv9025_RxChannels_e    rxChannel,
                                                   adi_adrv9025_RxGainPinCfg_t* rxGainPinCtrlCfg);

/**
* \brief This API function returns the configuration (Gain steps and the GPIO
* inputs) for Manual Rx gain control.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param rxChannel enum type to select Rx channel for reading configuration from.
* \param rxGainPinCtrlCfg Pointer to adi_adrv9025_RxGainPinCfg_t structure that
* would contain configuration values for Manual Rx gain pin control read back (Output).
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainPinCtrlCfgGet(adi_adrv9025_Device_t*       device,
                                         adi_adrv9025_RxChannels_e    rxChannel,
                                         adi_adrv9025_RxGainPinCfg_t* rxGainPinCtrlCfg);

/**
* \brief This API function configures Analog GPIOs to output gain table external ctrl words
*        on analog GPIOs
*
*                       extCtrlGpioChannelEn          |   Ext Ctrl Word Output GPIOs
*    -------------------------------------------------|--------------------------------------------
*  ADI_ADRV9025_ENABLE_RX1_RX2_RX3_RX4_EXT_CTRL_GPIOS |  GPIO_ANA[1:0] -> Rx1 Ext Ctrl Word Output
*                                                     |  GPIO_ANA[3:2] -> Rx2 Ext Ctrl Word Output
*                                                     |  GPIO_ANA[5:4] -> Rx3 Ext Ctrl Word Output
*                                                     |  GPIO_ANA[7:6] -> Rx4 Ext Ctrl Word Output
*                                                     |
* ADI_ADRV9025_DISABLE_RX1_RX2_RX3_RX4_EXT_CTRL_GPIOS |  GPIO_ANA[7:0] -> Default(Input)
*                                                     |
*          ADI_ADRV9025_ENABLE_RX1_RX2_EXT_CTRL_GPIOS |  GPIO_ANA[1:0] -> Rx1 Ext Ctrl Word Output
*                                                     |  GPIO_ANA[3:2] -> Rx2 Ext Ctrl Word Output
*                                                     |
*         ADI_ADRV9025_DISABLE_RX1_RX2_EXT_CTRL_GPIOS |  GPIO_ANA[3:0] -> Default(Input)
*                                                     |
*          ADI_ADRV9025_ENABLE_RX3_RX4_EXT_CTRL_GPIOS |  GPIO_ANA[5:4] -> Rx3 Ext Ctrl Word Output
*                                                     |  GPIO_ANA[7:6] -> Rx4 Ext Ctrl Word Output
*                                                     |
*         ADI_ADRV9025_DISABLE_RX3_RX4_EXT_CTRL_GPIOS |  GPIO_ANA[7:4] -> Default(Input)
*
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
*
* \param extCtrlGpioChannelEn adi_adrv9025_RxChannels_e enum type to select which set of gain table
*        external ctrl words to output on analog GPIOs as per the table shown above
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainTableExtCtrlPinsSet(adi_adrv9025_Device_t*                 device,
                                               adi_adrv9025_RxExtCtrlPinOuputEnable_e extCtrlGpioChannelEn);

/**
* \brief Range check for adi_adrv9025_RxGainTableExtCtrlPinsSet() function
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
*
* \param extCtrlGpioChannelEnable adi_adrv9025_RxChannels_e enum type to select which set of gain table
*        external ctrl words to output on analog GPIOs
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainTableExtCtrlPinsSetRangeCheck(adi_adrv9025_Device_t*                 device,
                                                         adi_adrv9025_RxExtCtrlPinOuputEnable_e extCtrlGpioChannelEnable);

/**
* \brief This function gets the Dec Power for the specified channel.
*
* This function can be used to get the Dec Power of RX channel in runtime.
*
* Due to interdependencies between the AGC and power measurement the power measurement duration and
* where the measurement is taken is variable.
* The location of the power measurement is given by agcCfg->agcPower->powerInputSelect
* The number of samples the power measurement uses is given by 8*2^(agcCfg->agcPower->powerMeasurementDuration) at the IQ rate,
* if measured at RFIR output. This number of samples must be less than the agcCfg->agcGainUpdateCounter.
* If the receiver is disabled during the power measurement, this function returns a 0 value for rxDecPower_mdBFS
*
* The resolution of this function is 0.25dB.
* The dynamic range of this function is 60dB. Signals lower than 60dBFS may not be measured accurately.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param rxChannel adi_adrv9025_RxChannels_e enum type to select Rx Channel.
* \param rxDecPower_mdBFS Pointer to store the ADRV9025 Dec Power return. Value returned in mdBFS.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_ERR_RESET_FEATURE Recovery action for user GPIO reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxDecPowerGet(adi_adrv9025_Device_t*    device,
                                   adi_adrv9025_RxChannels_e rxChannel,
                                   uint16_t*                 rxDecPower_mdBFS);

/**
* \brief This function reads back the LO Source Mapping to the requested Rx channel.
*
* The LO source select for a given channel is configured during init time. This function
* can be used to read back the configuration during run time.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param rxChannel Enum to select Rx Channel.
* \param rxLoSource Pointer to store Rx channel LO source mapping read back (Output)
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_ERR_RESET_FEATURE Recovery action for user GPIO reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxLoSourceGet(adi_adrv9025_Device_t*    device,
                                   adi_adrv9025_RxChannels_e rxChannel,
                                   adi_adrv9025_LoSel_e*     rxLoSource);

/**
* \brief This function sets the temperature gain compensation parameter.
*
* This function can be called at any point. The gain can be configured
* from -6.3 dB to 6.3 dB in 0.1 dB increments.  Hence allowable range is
* [-63 to +63] with 0 representing 0 dB.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param rxChannelMask Bit mask representing channels to be programmed (can be multiple channels).
* \param gainValue Gain value to set
*
* The following table lists the rxChannelMask bits and the corresponding channels
* that are enabled/disabled in SPI mode
*
*                rxChannelMask | Channel Enable/Disable
*             -----------------|------------------------------------
*                       bit[0] | 1 = Rx1 Enable, 0 = Rx1 Disable
*                       bit[1] | 1 = Rx2 Enable, 0 = Rx2 Disable
*                       bit[2] | 1 = Rx3 Enable, 0 = Rx3 Disable
*                       bit[3] | 1 = Rx4 Enable, 0 = Rx4 Disable
*                       bit[4] | 1 = ORx1 Enable, 0 = ORx1 Disable
*                       bit[5] | 1 = ORx2 Enable, 0 = ORx2 Disable
*                       bit[6] | 1 = ORx3 Enable, 0 = ORx3 Disable
*                       bit[7] | 1 = ORx4 Enable, 0 = ORx4 Disable
*                       bit[8] | Reserved, do not use.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxTempGainCompSet(adi_adrv9025_Device_t* device,
                                       uint32_t               rxChannelMask,
                                       int8_t                 gainValue);

/**
* \brief This function gets the temperature gain compensation parameter. Only one channel
* can be retrieved per call.
*
* This function can be called at any point. The gain returned is in the
* range -63 to 63
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param rxChannel Bit mask representing which channel to retrieve (only one channel returned per call)
* \param gainValue Gain value retrieved (Output)
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxTempGainCompGet(adi_adrv9025_Device_t*    device,
                                       adi_adrv9025_RxChannels_e rxChannel,
                                       int8_t*                   gainValue);

/**
* \brief This API function configures Analog GPIOs to output Rx dualband external LNA ctrl signal
*        when all 4 RX channels are enabled.
*
* This function configures the dualband A and B controls for all 4 Rx channels, therefore all 8
* analog GPIO pins are consumed by this feature. The analog GPIO pin outputs from ADRV9025 are
* assigned as follows :-
*
*                              GPIO      |   Control Signal
*                        ----------------|----------------------
*                         Analog GPIO[0] | Rx1 Dual band ctrl A
*                         Analog GPIO[1] | Rx1 Dual band ctrl B
*                         Analog GPIO[2] | Rx2 Dual band ctrl A
*                         Analog GPIO[3] | Rx2 Dual band ctrl B
*                         Analog GPIO[4] | Rx3 Dual band ctrl A
*                         Analog GPIO[5] | Rx3 Dual band ctrl B
*                         Analog GPIO[6] | Rx4 Dual band ctrl A
*                         Analog GPIO[7] | Rx4 Dual band ctrl B
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param enable Param to enable(1) or disable(0) Rx dualband LNA ctrl via Analog GPIOs.
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_GpioForAnalogDualBandOperationSet(adi_adrv9025_Device_t* device,
                                                       uint8_t                enable);

/**
* \brief This API function configures Rx NCO mixers in the Rx data path.
*        This API can only be used at runtime after device initialization.
*
* Available configuration parameters are:
*  bandAInputBandWidth_kHz is the bandwidth in kHz of the BandA input signal
*  bandAInputCenterFreq_kHz is the center frequency in kHz of the BandA input signal
*  bandANco1Freq_kHz is the BandA NCO1 Frequency shift in kHz
*  bandANco2Freq_kHz is the BandA NCO2 Frequency shift in kHz
*  bandBInputBandWidth_kHz is the bandwidth in kHz of the BandB input signal
*  bandBInputCenterFreq_kHz is the center frequency in kHz of the BandB input signal
*  bandBNco1Freq_kHz is the BandB NCO1 Frequency shift in kHz
*  bandBNco2Freq_kHz is the BandB NCO2 Frequency shift in kHz
*  bandAbCombinedEnable is the BandA and BandB Combined at output, 1 = Combine dualband AB, 0 disable combine dualband on AB
*
* Enable parameters:
* If ADI_ADRV9025_RX_GAIN_PHASE init cal is run ADI_ADRV9025_BAND_A_NCO_1 needs to be enabled all the time
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param rxChannelMask Channels to apply the supplied configuration
* \param rxShiftConfig Rx NCO configuration
* \param rxDdcMode DDC Half Band Filter configuration
* \param enableMask This is a mask composed of type adi_adrv9025_RxNcoEnable_e
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxNcoShifterSet(adi_adrv9025_Device_t         *device,
                                     adi_adrv9025_RxChannels_e      rxChannelMask,
                                     adi_adrv9025_RxNcoShifterCfg_t rxShiftConfig,
                                     adi_adrv9025_RxDdc_e           rxDdcMode,
                                     uint32_t                       enableMask);

/**
* \brief This API function retrieves the selected channel's Rx NCO mixer status.
*        This API can only be used at runtime after device initialization.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param rxChannel Channel from which to get the configuration
* \param ncoBandSelect Signal Band A or B from which to get the configuration
* \param rxShiftConfigGet pointer to the configuration structure in which retrieved configuration is returned
* \param rxDdcMode DDC read back value
* \param enableMask pointer to read back NCO
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxNcoShifterGet(adi_adrv9025_Device_t          *device,
                                     adi_adrv9025_RxChannels_e       rxChannel,
                                     adi_adrv9025_RxNcoBandSelect_e  ncoBandSelect,
                                     adi_adrv9025_RxNcoShifterCfg_t *rxShiftConfigGet,
                                     adi_adrv9025_RxDdc_e           *rxDdcMode,
                                     uint32_t                       *enableMask);

/**
* \brief These bits configure the gain control mode that is used on RX channels.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param rxChannelMask Rx Channel(s) to be configured, Valid range (1-15)
* \param agcMode Data to be configured. Parameter is of type adi_adrv9025_RxAgcMode_e.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset.
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check.
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required.
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required.
*
*/
int32_t adi_adrv9025_RxAgcClkModeSet(adi_adrv9025_Device_t     *device,
                                     uint32_t                   rxChannelMask,
                                     adi_adrv9025_RxAgcMode_e   agcMode);

/**
* \brief This API function retrieves the selected channel's Rx AGC clock mode status.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param rxChannel Base Address of instance to be configured.
* \param agcMode value retrieved (output). Parameter is of type uint8_t.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset.
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check.
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required.
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required.
*
*/
int32_t adi_adrv9025_RxAgcClkModeGet(adi_adrv9025_Device_t     *device,
                                     adi_adrv9025_RxChannels_e  rxChannel,
                                     uint8_t                   *agcMode);




/**
* \brief This function maps Rx channels to a GPIO to enable
* the gain counter sync pulse feature, as such the Gain Update Counter
* gets synchronized with the rising edge of the pulse. This function should
* be used if agcEnableSyncPulseForGainCounter is set.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param rxChannel Rx channel selection
* \param hybridSelPin1 Gpio to be mapped
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxAgcSyncGpioPinSet(adi_adrv9025_Device_t     *device,
                                         uint8_t                   rxChannel,
                                         adi_adrv9025_GpioPinSel_e hybridSelPin1);



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

#endif /* _ADI_ADRV9025_RX_H_ */
