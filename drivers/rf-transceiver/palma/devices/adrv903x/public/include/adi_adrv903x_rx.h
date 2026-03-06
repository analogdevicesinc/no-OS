/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * \file adi_adrv903x_rx.h
 * \brief Contains ADRV903X receive related function prototypes for
 *        adi_adrv903x_rx.c
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef _ADI_ADRV903X_RX_H_
#define _ADI_ADRV903X_RX_H_

#include "adi_adrv903x_rx_types.h"
#include "adi_adrv903x_rx_nco.h"
#include "adi_adrv903x_error.h"

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

 /**
 * \brief Programs the gain table settings for Rx channels.
 *
 * This function can be called by the user to load a custom gain table or
 * to reconfigure the gain table.The gain table for a receiver type is set with the
 * parameters passed by adi_adrv903x_RxGainTableRow_t gainTablePtr array.
 * The array length (n) is dependent upon receiver type.
 * The (n) value is conveyed by numGainIndicesInTable.
 * All gain tables have a maximum index and a minimum index specified by
 * MAX_RX_GAIN_TABLE_NUMINDICES and MIN_RX_GAIN_TABLE_INDEX
 * The minimum gain index is application dependent, this can be modified
 * in the user space, the absolute maximum and minimum indices are specified
 * by MAX_GAIN_TABLE_INDEX and MIN_GAIN_TABLE_INDEX
 *
 * The Rx max gain index is user configurable. A separate call has to be made
 * to adi_adrv903x_RxMinMaxGainIndexSet() API to update the min and max gain
 * indices for a given Rx Channel. Updating min and max gain indices are
 * decoupled from the main gain table loading so that the user has flexibility
 * to load multiple gain table regions and switch between them during runtime.
 *
 * The gain table configs can be broadcast / multicast based on the channel mask
 * parameter in adi_adrv903x_RxGainTableCfg_t structure.
 *
 *|                        rxChannelMask | Rx Channels Programmed                |
 *|:------------------------------------:|:-------------------------------------:|
 *|                         bit[0] = 1   |  Enables Rx0 gain table programming---|
 *|                         bit[1] = 1   |  Enables Rx1 gain table programming---|
 *|                         bit[2] = 1   |  Enables Rx2 gain table programming---|
 *|                         bit[3] = 1   |  Enables Rx3 gain table programming---|
 *|                         bit[4] = 1   |  Enables Rx4 gain table programming---|
 *|                         bit[5] = 1   |  Enables Rx5 gain table programming---|
 *|                         bit[6] = 1   |  Enables Rx6 gain table programming---|
 *|                         bit[7] = 1   |  Enables Rx7 gain table programming---|
 *
 * Eg: To program the same gain table to channels Rx0 and Rx4, the rxChannelMask
 *     should be set to 0x00000011
 *
 * Eg: To program a single gain table to all channels, the rxChannelMask
 *     should be set to 0x000000FF
 *
 *  Partial gain table loads can be done through this API in case of memory constraints / multiple region loading
 *  For example, consider a 256 row gain table which needs to be loaded in 4 consecutive calls.
 *  In this case the config parameters for partial loads would be
 *  Partial Load 1 : gainTableRow[] = gainTable[63:0], gainIndexOffset = 63, numGainIndicesInTable = 64
 *  Partial Load 2 : gainTableRow[] = gainTable[127:64], gainIndexOffset = 127, numGainIndicesInTable = 64
 *  Partial Load 3 : gainTableRow[] = gainTable[191:128], gainIndexOffset = 191, numGainIndicesInTable = 64
 *  Partial Load 4 : gainTableRow[] = gainTable[255:192], gainIndexOffset = 255, numGainIndicesInTable = 64
 *
 *  Post this multiple partial gain table load, call the function adi_adrv903x_RxMinMaxGainIndexSet(minIndex = 0, maxIndex = 255).
 *
 * \pre This function called automatically in adi_adrv903x_Initialize() to load
 *      the default gain table. If the BBIC desires to change or update the
 *      gain table, it may call this function after initialization but before
 *      running init cals.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep{device->devStateInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Context variable - Structure pointer to the ADRV903X device data structure
 * \param[in] rxChannelMask is the set of channels for which current gain table settings are to be programmed.
 *              Valid Rx channels include Rx0-Rx7
 * \param[in] gainIndexOffset is the starting gain index from which gain table is programmed
 * \param[in] gainTableRow Array of gain table row entries for programming
 * \param[in] arraySize is the number of gain table rows to be programmed
 *
 * \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainTableWrite(adi_adrv903x_Device_t* const        device,
                                                               const uint32_t                      rxChannelMask,
                                                               const uint8_t                       gainIndexOffset,
                                                               const adi_adrv903x_RxGainTableRow_t gainTableRow[],
                                                               const uint32_t                      arraySize );
/**
* \brief Reads the gain table entries for Rx channels requested.
*
* This function can be called by the user to read back the currently programmed gain table
* for a given channel. This function reads the current gain table settings from ADRV903X gain table Static RAMs
* for the requested channel and stores it in the provided memory reference of type adi_adrv903x_RxGainTableCfg_t
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in,out] device Context variable - Context variable - Structure pointer to the ADRV903X device data structure
* \param[in]  rxChannel represents the channels for which gain table read back is requested.
*             Valid Rx channels include Rx0-Rx7
* \param[in]  gainIndexOffset is the gain index from which gain table read back should start
* \param[in,out] gainTableRow Read back array for gain table row entries which will be updated with the read back values
* \param[in]  arraySize is the size of gainTableRow array. It is also the upper limit for the no. of gain indices to read
* \param[out] numGainIndicesRead is the actual no. of gain indices read from SRAM (output). A NULL can be passed
*             if the value of no. of gain indices actually read is not required.
*
* \pre This function can be called by the user anytime after initialization.
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainTableRead(adi_adrv903x_Device_t* const    device,
                                                              const adi_adrv903x_RxChannels_e rxChannel,
                                                              const uint8_t                   gainIndexOffset,
                                                              adi_adrv903x_RxGainTableRow_t   gainTableRow[],
                                                              const uint32_t                  arraySize,
                                                              uint16_t* const                 numGainIndicesRead);

/**                                               
* \brief Updates the minimum and maximum gain indices for a requested Rx Channel
*        in the device data structure
*
* This function is required to be called by the user right after loading the gain table.
* This function is decoupled from the gain table loading function so that a user has
* flexibility to load multiple gain table regions and switch between multiple gain table
* regions during runtime
*
* The gain table configs can be broadcast / multicast based on the channel mask
* parameter in adi_adrv903x_RxGainTableCfg_t structure.
*
*|                        rxChannelMask | Rx Channel min & max gain indices updated      |
*|:------------------------------------:|:----------------------------------------------:|
*|                         bit[0] = 1   |  Updates Rx0 min & max gain indices------------|
*|                         bit[1] = 1   |  Updates Rx1 min & max gain indices------------|
*|                         bit[2] = 1   |  Updates Rx2 min & max gain indices------------|
*|                         bit[3] = 1   |  Updates Rx3 min & max gain indices------------|
*|                         bit[4] = 1   |  Updates Rx4 min & max gain indices------------|
*|                         bit[5] = 1   |  Updates Rx5 min & max gain indices------------|
*|                         bit[6] = 1   |  Updates Rx6 min & max gain indices------------|
*|                         bit[7] = 1   |  Updates Rx7 min & max gain indices------------|

*
* Eg: To update min and max gain indices of channels Rx4 and Rx0, the rxChannelMask
*     should be set to 0x00000011
*
* Eg: To update min and max gain indices of all Rx channels, the rxChannelMask
*     should be set to 0x000000FF
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in,out] device Context variable - Context variable - Structure pointer to the ADRV903X device data structure
* \param[in] rxChannelMask channel masks for which min and max gain indices are to be updated
* \param[in] minGainIndex is the lower limit of the gain index for a given channel
* \param[in] maxGainIndex is the upper limit of the gain index for a given channel
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxMinMaxGainIndexSet(adi_adrv903x_Device_t* const    device,
                                                                   const uint32_t                  rxChannelMask,
                                                                   const uint8_t                   minGainIndex,
                                                                   const uint8_t                   maxGainIndex);

/**
* \brief Enable or disable the routing of the external control word from an Rx channel's 
*  gain table to its external analog GPIO pins. 
* 
* Typically, these signals are used to control an external LNA.
* The two LSBs of the external control word for Rx channel 0 are output on GPIO_ANA[0:1] 
* (the LSB itself on GPIO_ANA0) and so on for the other channels up to Rx channel 7 
* whose external control word is output on GPIO_ANA[14:15]. The mapping from Rx channel 
* to analog GPIO pins is fixed. It can be enabled or disabled but the mapping cannot 
* be changed. See Full Table below.
* 
* If external control for a channel is disabled then it's corresponding analog GPIOs 
* are placed in input/high-Z mode.
*
* If rxChannelMask = 0, this is allowed and function will reduce to a No-op command
* 
* Rx Channel Analog GPIO pins Table:
*       RX0 Gain Control Word [1:0]  :  GPIO_ANA[1:0]
*       RX1 Gain Control Word [1:0]  :  GPIO_ANA[3:2]
*       RX2 Gain Control Word [1:0]  :  GPIO_ANA[5:4]
*       RX3 Gain Control Word [1:0]  :  GPIO_ANA[7:6]
*       RX4 Gain Control Word [1:0]  :  GPIO_ANA[9:8]
*       RX5 Gain Control Word [1:0]  :  GPIO_ANA[11:10]
*       RX6 Gain Control Word [1:0]  :  GPIO_ANA[13:12]
*       RX7 Gain Control Word [1:0]  :  GPIO_ANA[15:14]
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV903X device data structure
* \param[in] rxChannelMask Channel bit-mask indicating the Rx channels affected by the call.
* \param[in] channelEnable Enable bit-mask indicating the final state of the channels included in rxChannelMask
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainTableExtCtrlPinsSet(adi_adrv903x_Device_t* const    device,
                                                                        const uint32_t                  rxChannelMask,
                                                                        const uint32_t                  channelEnable);

/****************************************************************************
 * Runtime functions
 ****************************************************************************
 */

 /**
 * \brief Sets the Rx Channel Manual Gain Index
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
        The device must NOT be in spi streaming mode due to this function utilizing the hal layer directly.
        If the device is in spi streaming mode, this function may not behave as expected.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV903X device data structure
 * \param[in] rxGain is the channelized gain setting parameter
 * \param[in] arraySize Size of rxGain array representing no. of configs.
 *
 * The gain table configs can be broadcast / multicast based on the channel mask
 * parameter in adi_adrv903x_RxGain_t structure.
 *
 *|           rxGain[i]->rxChannelMask | Rx Channels to set manual gain     |
 *|:----------------------------------:|:----------------------------------:|
 *|                            bit[0]  |  1 = Rx0 manual gain is updated    |
 *|                            bit[1]  |  1 = Rx1 manual gain is updated    |
 *|                            bit[2]  |  1 = Rx2 manual gain is updated    |
 *|                            bit[3]  |  1 = Rx3 manual gain is updated    |
 *|                            bit[4]  |  1 = Rx4 manual gain is updated    |
 *|                            bit[5]  |  1 = Rx5 manual gain is updated    |
 *|                            bit[6]  |  1 = Rx6 manual gain is updated    |
 *|                            bit[7]  |  1 = Rx7 manual gain is updated    |
 *
 * Eg: To update manual gain for channels Rx3 and Rx0, the rxGain[i]->rxChannelMask
 *     should be set to 0x00000009
 *
 * Eg: To update manual gain for all Rx channels, the rxGain[i]->rxChannelMask
 *     should be set to 0x000000FF
 *
 * \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainSet(adi_adrv903x_Device_t* const    device,
                                                        const adi_adrv903x_RxGain_t     rxGain[],
                                                        const uint32_t                  arraySize);

/**
* \brief Reads the Rx MGC Gain Index for the requested Rx channel
*
* This function reads the gain index for the passed channel Rx0-Rx7
*
* This function reads back the Manual gain index per channel, depending on the gain control mode.
*
* \pre This function may be called any time after device initialization.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X data structure
* \param[in] rxChannel Channel selection to read back gain index for (Valid Rx0-Rx7 only)
* \param[out] rxGain Pointer to the specified channel gain index value
*             which will be updated by this function
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxMgcGainGet(adi_adrv903x_Device_t * const   device,
                                                           const adi_adrv903x_RxChannels_e rxChannel,
                                                           adi_adrv903x_RxGain_t * const   rxGain);
    
/**
* \brief Reads the Rx AGC Gain Index for the requested Rx channel
*
* This function reads the gain index for the passed channel Rx0-Rx7
*
* This function reads back the AGC gain index per channel, depending on the gain control mode.
*
*
* \pre This function may be called any time after device initialization.
*  However, gain indices are tracked only after the device goes into a
*  Receiver mode.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X data structure
* \param[in] rxChannel Channel selection to read back gain index for (Valid Rx0-Rx7 only)
* \param[out] rxGain Pointer to the specified channel gain index value
*             which will be updated by this function
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainGet(adi_adrv903x_Device_t * const   device,
                                                        const adi_adrv903x_RxChannels_e rxChannel,
                                                        adi_adrv903x_RxGain_t * const   rxGain);
                                                        

/**
* \brief Retrieves the Rx data path format configuration.
*
* This function will read the current configuration of Rx Data format over the JESD link. 
* This function also reads back the current configuration of slicer and formatter blocks 
* in the receive signal chain.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the device settings structure
* \param[in] rxChannel The channel for which Rx data path format configuration is requested
* \param[out] rxDataFormat Pointer to the Rx data format configuration structure
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxDataFormatGet(adi_adrv903x_Device_t * const       device,
                                                              const adi_adrv903x_RxChannels_e     rxChannel,
                                                              adi_adrv903x_RxDataFormatRt_t * const rxDataFormat);

/**
* \brief Gets the gain slicer position
*
* Before this information is valid, the Rx gain compensation feature should be enabled with
* the integer gain slicer sample format enabled.  See the adrv903x_RxDataFormatSet function
* to enable gain compensation. This function is not useful in the case of using floating point
* as the gain slicer (data scaling) is applied in the floating point scaled sample. This function
* is also not valid if the selected data format is external slicer mode.
*
* The slicer position is only needed for integer 12bit and 16bit formats.  The 24bit sample format
* already applies scaling in the 24bit sample. The data can be scaled in steps of 1dB, 2dB, 4dB,
* 6dB, 8dB configured through the adrv903x_RxDataFormatSet function.
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
* \param[in,out] device Context variable - Pointer to the ADRV903X data structure
* \param[in]  rxChannel Channel selection to read back gain index for
* \param[out] slicerPosition Pointer to byte which returns the slicer position bits for the requested channel.
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxSlicerPositionGet(adi_adrv903x_Device_t * const   device,
                                                                  const adi_adrv903x_RxChannels_e rxChannel,
                                                                  uint8_t * const                 slicerPosition);

/**
* \brief This function reads back the LO Source Mapping to the requested Rx channel.
*
* The LO source select for a given channel is configured during init time. This function
* can be used to read back the configuration during run time.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X data structure
* \param[in]  rxChannel Enum to select Rx Channel
* \param[out] rxLoSource Pointer to store Rx channel LO source mapping read back
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxLoSourceGet(adi_adrv903x_Device_t * const   device,
                                                            const adi_adrv903x_RxChannels_e rxChannel,
                                                            adi_adrv903x_LoSel_e * const    rxLoSource);

/**
* \brief Sets the specified Rx NCOs to the given frequency, band number, and enable state in the adi_adrv903x_RxNcoConfig_t structure.
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in,out] rxNcoConfig Pointer to the RX NCO config settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxNcoShifterSet(adi_adrv903x_Device_t* const                device,
                                                              const adi_adrv903x_RxNcoConfig_t * const    rxNcoConfig);

/**
* \brief Gets RX NCO parameters in adi_adrv903x_RxNcoConfigReadbackResp_t structure.
*
* \pre This function can be called after the CPU has been initialized, firmware loaded and the RX NCO configured.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in,out] rxRbConfig Pointer to the RX NCO config read back settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxNcoShifterGet(adi_adrv903x_Device_t* const                    device,
                                                              adi_adrv903x_RxNcoConfigReadbackResp_t* const   rxRbConfig);

/**
* \brief Sets the specified Orx NCOs to the given frequency, band number, and enable state in the adi_adrv903x_ORxNcoConfig_t structure.
*
* This function will also update all Tx To Orx Mapping preset NCO ADC frequencies when called, with the frequency passed. If no preset 
* frequencies should be overwritten, refer to OrxNcoSet_v2.
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in,out] orxNcoConfig Pointer to the ORX NCO config settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_OrxNcoSet(adi_adrv903x_Device_t* const                device,
                                                        const adi_adrv903x_ORxNcoConfig_t * const   orxNcoConfig);

/**
* \brief Sets the specified Orx NCOs to the given frequency, band number, and enable state in the adi_adrv903x_ORxNcoConfig_t structure.
*
* Different than OrxNcoSet(), this function does not interact or update the Tx to Orx Mapping presets when called, it writes and updates 
* the NCO frequencies directly. If the Tx To Orx Mapping selection is changed the NCO frequencies will revert to the preset values.
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in,out] orxNcoConfig Pointer to the ORX NCO config settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_OrxNcoSet_v2(adi_adrv903x_Device_t* const                device,
                                                           const adi_adrv903x_ORxNcoConfig_t * const   orxNcoConfig);

/**
* \brief Gets ORX NCO parameters in adi_adrv903x_OrxNcoConfigReadback_t structure.
*
* \pre This function can be called after the CPU has been initialized, firmware loaded and the ORX NCO configured.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in,out] orxRbConfig Pointer to the ORX NCO config read back settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_OrxNcoGet(adi_adrv903x_Device_t* const                    device,
                                                        adi_adrv903x_ORxNcoConfigReadbackResp_t* const  orxRbConfig);

/**
* \brief Function to configure Rx decimated power measurement block.
*
* This function can be used to configure decimated power measurements. User can either manually enable/disable
* decimated power measurements or allow AGC block to control power measurements. Measurement point can be selected
* for main path decimated power block only. This function configures Rx dec power block. For Orx dec power block 
* please use adi_adrv903x_ORxDecimatedPowerCfgSet function
* Note: The function does not check for duplicated channel assignments across cfg structures.
* Invalid channel mask values will return an error.

* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] rxDecPowerCfg array of structures of type adi_adrv903x_RxDecimatedPowerCfg_t which will configure one or more channels
* \param[in] numOfDecPowerCfgs number of configurations passed in the array
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxDecimatedPowerCfgSet(adi_adrv903x_Device_t * const device,
                                                                     adi_adrv903x_RxDecimatedPowerCfg_t rxDecPowerCfg[],
                                                                     const uint32_t numOfDecPowerCfgs);

/**
* \brief Function to read back Rx decimated power measurement configuration
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] rxChannel Rx channel selection to read decimated power block configuration
* \param[in] decPowerBlockSelection Decimated power block selection to read back configuration
* \param[out] rxDecPowerCfg Decimated power configuration of selected channel/dec power block
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxDecimatedPowerCfgGet(adi_adrv903x_Device_t * const device,
                                                                     const adi_adrv903x_RxChannels_e rxChannel,
                                                                     const adi_adrv903x_DecPowerMeasurementBlock_e decPowerBlockSelection,
                                                                     adi_adrv903x_RxDecimatedPowerCfg_t * const rxDecPowerCfg);

/**
* \brief Function to configure ORx decimated power measurement block.
*
* This function can be used to configure decimated power measurements for Orx channel.
* Measurement point and measurement duration can be selected by user through configuration structure.
* This function configures ORx dec power block. For Rx dec power block 
* please use adi_adrv903x_RxDecimatedPowerCfgSet function
* 
* Note: The function does not check for duplicated channel assignments across cfg structures.
* Invalid channel mask values will return an error.

* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] orxDecPowerCfg array of structures of type adi_adrv903x_ORxDecimatedPowerCfg_t which will configure one or more channels
* \param[in] numOfDecPowerCfgs number of configurations passed in the array
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_ORxDecimatedPowerCfgSet(adi_adrv903x_Device_t * const device,
                                                                      adi_adrv903x_ORxDecimatedPowerCfg_t orxDecPowerCfg[],
                                                                      const uint32_t numOfDecPowerCfgs);

/**
* \brief Function to read back Orx decimated power measurement configuration
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] orxChannel ORx channel selection to read decimated power block configuration
* \param[out] orxDecPowerCfg Decimated power configuration of selected channel/dec power block
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_ORxDecimatedPowerCfgGet(adi_adrv903x_Device_t * const device,
                                                                      const adi_adrv903x_RxChannels_e orxChannel,
                                                                      adi_adrv903x_ORxDecimatedPowerCfg_t * const orxDecPowerCfg);

/**
* \brief Function to readback the decimated power measurement for selected Rx/Orx channel.
* Power readback value is an unsigned 8 bit value with 0.25dB resolution.
* This function can be used to read power measurements for both Rx and Orx channels

* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] rxChannel Channel selection to read decimated power measurement
* \param[in] decPowerBlockSelection Decimated power block selection (unused for Orx channel selections)
* \param[out] powerReadBack Pointer to power measurement readback value
* 
* \retval adi_common_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxDecimatedPowerGet(adi_adrv903x_Device_t * const device,
                                                                  const adi_adrv903x_RxChannels_e rxChannel,
                                                                  const adi_adrv903x_DecPowerMeasurementBlock_e decPowerBlockSelection,
                                                                  uint8_t  * const powerReadBack);

/**
 * \brief Set the attenuation of one or several ORx channels. Attenuation is 0 - 17dB, in steps of 1dB.
 * 
 * \pre This function can be called after the CPU has been initialized, firmware loaded and the ORX NCO configured.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 * 
 * \param [in,out] device Context variable adi_adrv903x_Device_t pointer to the device data structure.
 * \param [in] channelMask uint32_t indicates the channels affected.
 * \param [in] attenDb uint8_t indicates the attenuation level to apply in Db.
 *
 * \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_OrxAttenSet(adi_adrv903x_Device_t* const device,
                                                          const uint32_t channelMask,
                                                          const uint8_t attenDb);

/**
 * \brief Get the attenuation of an ORx channel.
 * 
 * \param [in, out] device Context variable adi_adrv903x_Device_t pointer to the device data structure.
 * \param [in] channelId uint8_t indicates the channels affected.
 * \param [out] attenDb uint8_t the channel's attenuation value is written here.
 *
 * \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_OrxAttenGet(adi_adrv903x_Device_t* const device,
                                                          const uint8_t channelId,
                                                          uint8_t* const attenDb);

/**
 * \brief set the Rx gain control mode
 *
 * \brief The Gain Control Mode for Rx signal can be set to MGC and AGC
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in,out] device Context variable adi_adrv903x_Device_t pointer to the device data structure
 * \param [in] gainCtrlModeCfg array to be configured. Parameter is of structure type adi_adrv903x_RxGainCtrlModeCfg_t
 * \param [in] arraySize is the no. of elements in gainCtrlModeCfg[] array
 *
 * \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainCtrlModeSet(adi_adrv903x_Device_t* const            device,
                                                                const adi_adrv903x_RxGainCtrlModeCfg_t  gainCtrlModeCfg[],
                                                                const uint32_t                          arraySize);

/**
 * \brief get the Rx gain control mode with Rx channel index
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in,out] device Context variable adi_adrv903x_Device_t pointer to the device data structure
 * \param [in] rxChannel indicates the Rx channel affected.
 * \param [out] gainCtrlMode is the pointer to adi_adrv903x_RxGainCtrlMode_e enum which will
 *              be updated with the read back config
 *
 * \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainCtrlModeGet(adi_adrv903x_Device_t* const     device,
                                                                const adi_adrv903x_RxChannels_e  rxChannel,
                                                                adi_adrv903x_RxGainCtrlMode_e*   gainCtrlMode);

/**
 * \brief This function sets the temperature gain compensation parameter for Rx channel only
 *
 * This function can be called at any point. The gain can be configured
 * from -6.3 dB to 6.3 dB in 0.1 dB step.  Hence allowable range is
 * [-63 to +63] with 0 representing 0 dB.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in,out] device Context variable adi_adrv903x_Device_t pointer to the device data structure
 * \param [in] rxChannelMask Bit mask representing channels to be programmed (can be multiple channels).
 * \param [in] gainValue Gain value to set
 * 
 * \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxTempGainCompSet(adi_adrv903x_Device_t* const  device,
                                                                const uint32_t                rxChannelMask,
                                                                const int8_t                  gainValue);

/**
 * \brief This function gets the temperature gain compensation parameter for Rx channel only.
 *        Only one channel can be retrieved per call
 * 
 * This function can be called at any point. The gain returned is in the
 * range -63 to 63, this means -6.3 dB to 6.3 dB
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in,out] device Context variable adi_adrv903x_Device_t pointer to the device data structure
 * \param [in] rxChannel Bit mask representing which channel to retrieve (only one channel returned per call)
 * \param [out] gainValue Gain value retrieved
 * 
 * \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxTempGainCompGet(adi_adrv903x_Device_t* const     device,
                                                                const adi_adrv903x_RxChannels_e  rxChannel,
                                                                int8_t* const                    gainValue);

/**
 * \brief Allows the data from one or more Rx ADCs to be replaced with a test signal.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in,out] device Context variable adi_adrv903x_Device_t pointer to the device data structure
 * \param [in] rxChannelMask Indicates the channels to be affected by the call.
 * \param [in] rxTestDataCfg Indicates whether the ADC output signal should be replaced by a test signal for the 
 *      channels in rxChannelMask or whether the normal ADC output should be used. Also describes the test signal.
 * 
 * \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxTestDataSet(adi_adrv903x_Device_t* device,
                                                            const uint32_t rxChannelMask,
                                                            const adi_adrv903x_RxTestDataCfg_t* const rxTestDataCfg);

/**
 * \brief Check if the data from an Rx ADC is currently being replaced with a test signal and
 * if so retrieves the parameters of the test signal.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in,out] device Context variable adi_adrv903x_Device_t pointer to the device data structure
 * \param [in] rxChannel The channel whose information to get.
 * \param [out] rxTestDataCfg Indicates whether the Rx ADC signal is being replaced by a test signal
 *     and if so also the parameters of the test signal.
 * 
 * \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxTestDataGet(adi_adrv903x_Device_t* device,
                                                            const adi_adrv903x_RxChannels_e rxChannel,
                                                            adi_adrv903x_RxTestDataCfg_t* const rxTestDataCfg);

/**
* \brief Controls whether Rx LO mixer is powered down or not when an Rx channel is disabled.
* 
* By default the LO is powered down when the Rx is disabled.
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param [in,out] device Context variable adi_adrv903x_Device_t pointer to the device data structure
* \param [in] rxChannelMask The Rx channels to affect. Must include at least one channel from Rx0 to Rx7.
* \param [in] enable To change the default behavour and skip the Rx LO mixer power down for the channels in
*     rxChannelMask set 'enable' to ADI_DISABLE. Restore the default by setting to ADI_ENABLE.
* 
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxLoPowerDownSet(adi_adrv903x_Device_t* const    device, 
                                                               const adi_adrv903x_RxChannels_e rxChannelMask,
                                                               const uint8_t                   enable);
/**
* \brief API configures a custom baseband spur frequency. The spur cancellation calibration
*        will aim to cancel any tone/spur found at this frequency.
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param [in,out] device Context variable adi_adrv903x_Device_t pointer to the device data structure
* \param [in] rxChannelMask The Rx channels to affect.
* \param [in] bbFreqKhz Input BaseBand Freq.
* \param [in] enable or disable Tracking Cal.
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxSpurBaseBandFreqSet(adi_adrv903x_Device_t* const    device, 
                                                                    const adi_adrv903x_RxChannels_e rxChannelMask,
                                                                    const int32_t                   bbFreqKhz, 
                                                                    const uint8_t                   enable);

/**
* \brief API to readback custom baseband spur frequency.
* 
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param [in,out] device context variable adi_adrv903x_Device_t pointer to the device data structure.
* \param [in]     rxChannelMask The Rx channels to affect. Must include at least one channel from Rx0 to Rx7.
* \param[in,out]  rxSpurRbConfig Pointer to struct adi_adrv903x_RxSpurFreqConfigResp_t to Readback Baseband Frequency.
* 
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxSpurBaseBandFreqGet(adi_adrv903x_Device_t* const                device,
                                                                    const adi_adrv903x_RxChannels_e             rxChannelMask,
                                                                    adi_adrv903x_RxSpurFreqConfigResp_t* const  rxSpurRbConfig);


#endif /* _ADI_ADRV903X_RX_H_ */
