/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_adrv904x_rx.h
 * \brief Contains ADRV904X receive related function prototypes for
 *        adi_adrv904x_rx.c
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef _ADI_ADRV904X_RX_H_
#define _ADI_ADRV904X_RX_H_

#include "adi_adrv904x_rx_types.h"
#include "adi_adrv904x_error.h"

#include "adi_adrv904x_carrier_reconfigure_types.h"

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

 /**
 * \brief Programs the gain table settings for Rx channels.
 *
 * This function can be called by the user to load a custom gain table or
 * to reconfigure the gain table.The gain table for a receiver type is set with the
 * parameters passed by adi_adrv904x_RxGainTableRow_t gainTablePtr array.
 * The array length (n) is dependent upon receiver type.
 * The (n) value is conveyed by numGainIndicesInTable.
 * All gain tables have a maximum index and a minimum index specified by
 * MAX_RX_GAIN_TABLE_NUMINDICES and MIN_RX_GAIN_TABLE_INDEX
 * The minimum gain index is application dependent, this can be modified
 * in the user space, the absolute maximum and minimum indices are specified
 * by MAX_GAIN_TABLE_INDEX and MIN_GAIN_TABLE_INDEX
 *
 * The Rx max gain index is user configurable. A separate call has to be made
 * to adi_adrv904x_RxMinMaxGainIndexSet() API to update the min and max gain
 * indices for a given Rx Channel. Updating min and max gain indices are
 * decoupled from the main gain table loading so that the user has flexibility
 * to load multiple gain table regions and switch between them during runtime.
 *
 * The gain table configs can be broadcast / multicast based on the channel mask
 * parameter in adi_adrv904x_RxGainTableCfg_t structure.
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
 *  Post this multiple partial gain table load, call the function adi_adrv904x_RxMinMaxGainIndexSet(minIndex = 0, maxIndex = 255).
 *
 * \pre This function called automatically in adi_adrv904x_Initialize() to load
 *      the default gain table. If the BBIC desires to change or update the
 *      gain table, it may call this function after initialization but before
 *      running init cals.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep{device->devStateInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Context variable - Structure pointer to the ADRV904X device data structure
 * \param[in] rxChannelMask is the set of channels for which current gain table settings are to be programmed.
 *              Valid Rx channels include Rx0-Rx7
 * \param[in] gainIndexOffset is the starting gain index from which gain table is programmed
 * \param[in] gainTableRow Array of gain table row entries for programming
 * \param[in] arraySize is the number of gain table rows to be programmed
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainTableWrite(adi_adrv904x_Device_t* const        device,
                                                               const uint32_t                      rxChannelMask,
                                                               const uint8_t                       gainIndexOffset,
                                                               const adi_adrv904x_RxGainTableRow_t gainTableRow[],
                                                               const uint32_t                      arraySize );
/**
* \brief Reads the gain table entries for Rx channels requested.
*
* This function can be called by the user to read back the currently programmed gain table
* for a given channel. This function reads the current gain table settings from ADRV904X gain table Static RAMs
* for the requested channel and stores it in the provided memory reference of type adi_adrv904x_RxGainTableCfg_t
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in,out] device Context variable - Context variable - Structure pointer to the ADRV904X device data structure
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
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainTableRead(adi_adrv904x_Device_t* const    device,
                                                              const adi_adrv904x_RxChannels_e rxChannel,
                                                              const uint8_t                   gainIndexOffset,
                                                              adi_adrv904x_RxGainTableRow_t   gainTableRow[],
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
* parameter in adi_adrv904x_RxGainTableCfg_t structure.
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
* \param[in,out] device Context variable - Context variable - Structure pointer to the ADRV904X device data structure
* \param[in] rxChannelMask channel masks for which min and max gain indices are to be updated
* \param[in] minGainIndex is the lower limit of the gain index for a given channel
* \param[in] maxGainIndex is the upper limit of the gain index for a given channel
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxMinMaxGainIndexSet(adi_adrv904x_Device_t* const    device,
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
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in] rxChannelMask Channel bit-mask indicating the Rx channels affected by the call.
* \param[in] channelEnable Enable bit-mask indicating the final state of the channels included in rxChannelMask
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainTableExtCtrlPinsSet(adi_adrv904x_Device_t* const    device,
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
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] rxGain is the channelized gain setting parameter
 * \param[in] arraySize Size of rxGain array representing no. of configs.
 *
 * The gain table configs can be broadcast / multicast based on the channel mask
 * parameter in adi_adrv904x_RxGain_t structure.
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
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainSet(adi_adrv904x_Device_t* const    device,
                                                        const adi_adrv904x_RxGain_t     rxGain[],
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
* \param[in,out] device Context variable - Pointer to the ADRV904X data structure
* \param[in] rxChannel Channel selection to read back gain index for (Valid Rx0-Rx7 only)
* \param[out] rxGain Pointer to the specified channel gain index value
*             which will be updated by this function
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxMgcGainGet(adi_adrv904x_Device_t * const   device,
                                                           const adi_adrv904x_RxChannels_e rxChannel,
                                                           adi_adrv904x_RxGain_t * const   rxGain);
    
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
* \param[in,out] device Context variable - Pointer to the ADRV904X data structure
* \param[in] rxChannel Channel selection to read back gain index for (Valid Rx0-Rx7 only)
* \param[out] rxGain Pointer to the specified channel gain index value
*             which will be updated by this function
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainGet(adi_adrv904x_Device_t * const   device,
                                                        const adi_adrv904x_RxChannels_e rxChannel,
                                                        adi_adrv904x_RxGain_t * const   rxGain);
                                                        

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
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDataFormatGet(adi_adrv904x_Device_t * const       device,
                                                              const adi_adrv904x_RxChannels_e     rxChannel,
                                                              adi_adrv904x_RxDataFormatRt_t * const rxDataFormat);

/**
* \brief Sets RX CDDC Formatter configuration (see adrv904x_RxDataFormatSet for details)
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in] cddcDataFormat Pointer to the Rx data format configuration structure
* \param[in] arraySize Size of rxDataFormat array representing no. of configs
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CddcDataFormatSet(adi_adrv904x_Device_t* const         device,
    const adi_adrv904x_CddcDataFormatRt_t cddcDataFormat[],
    const uint32_t                        arraySize);

/**
* \brief Retrieves the Cddc data path format configuration.
*
* This function will read the current configuration of Cddc Data format over the JESD link. 
* This function also reads back the current configuration of slicer and formatter blocks 
* in the receive signal chain.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the device settings structure
* \param[in] rxChannel The channel for which Rx data path format configuration is requested
* \param[out] cddcDataFormat Pointer to the CDDC data format configuration structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CddcDataFormatGet(adi_adrv904x_Device_t * const           device,
                                                                const adi_adrv904x_RxChannels_e         rxChannel,
                                                                adi_adrv904x_CddcDataFormatRt_t * const cddcDataFormat);


/**
* \brief Gets the gain slicer position
*
* Before this information is valid, the Rx gain compensation feature should be enabled with
* the integer gain slicer sample format enabled.  See the adrv904x_RxDataFormatSet function
* to enable gain compensation. This function is not useful in the case of using floating point
* as the gain slicer (data scaling) is applied in the floating point scaled sample. This function
* is also not valid if the selected data format is external slicer mode.
*
* The slicer position is only needed for integer 12bit and 16bit formats.  The 24bit sample format
* already applies scaling in the 24bit sample. The data can be scaled in steps of 1dB, 2dB, 4dB,
* 6dB, 8dB configured through the adrv904x_RxDataFormatSet function.
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
* \param[in,out] device Context variable - Pointer to the ADRV904X data structure
* \param[in]  rxChannel Channel selection to read back gain index for
* \param[out] slicerPosition Pointer to byte which returns the slicer position bits for the requested channel.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxSlicerPositionGet(adi_adrv904x_Device_t * const   device,
                                                                  const adi_adrv904x_RxChannels_e rxChannel,
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
* \param[in,out] device Context variable - Pointer to the ADRV904X data structure
* \param[in]  rxChannel Enum to select Rx Channel
* \param[out] rxLoSource Pointer to store Rx channel LO source mapping read back
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxLoSourceGet(adi_adrv904x_Device_t * const   device,
                                                            const adi_adrv904x_RxChannels_e rxChannel,
                                                            adi_adrv904x_LoSel_e * const    rxLoSource);

/**
* \brief Sets the specified Rx NCOs to the given frequency, band number, and enable state in the adi_adrv904x_RxNcoConfig_t structure.
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in,out] rxNcoConfig Pointer to the RX NCO config settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxNcoShifterSet(adi_adrv904x_Device_t* const                device,
                                                              const adi_adrv904x_RxNcoConfig_t * const    rxNcoConfig);

/**
* \brief Gets RX NCO parameters in adi_adrv904x_RxNcoConfigReadbackResp_t structure.
*
* \pre This function can be called after the CPU has been initialized, firmware loaded and the RX NCO configured.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in,out] rxRbConfig Pointer to the RX NCO config read back settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxNcoShifterGet(adi_adrv904x_Device_t* const                    device,
                                                              adi_adrv904x_RxNcoConfigReadbackResp_t* const   rxRbConfig);

/**
* \brief Set RX CDDC NCO parameters in adi_adrv904x_RxCddcNcoConfig_t structure.
*
* \pre This function can be called after the CPU has been initialized, firmware loaded and the RX CDDC NCO configured.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in,out] rxNcoConfig Pointer to the RX CDDC NCO config settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCddcNcoSet(adi_adrv904x_Device_t* const device,
                                                           const adi_adrv904x_RxCddcNcoConfig_t* const rxNcoConfig);

/**
* \brief Set RX CDDC NCO parameters in adi_adrv904x_RxCddcNcoConfigReadbackResp_t structure.
*
* \pre This function can be called after the CPU has been initialized, firmware loaded and the RX CDDC NCO configured.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in,out] rxRbConfig Pointer to the RX CDDC NCO config get settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCddcNcoGet(adi_adrv904x_Device_t* const device,
                                                           adi_adrv904x_RxCddcNcoConfigReadbackResp_t* const rxRbConfig);

/**
* \brief Sets the specified Orx NCOs to the given frequency, band number, and enable state in the adi_adrv904x_ORxNcoConfig_t structure.
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
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in,out] orxNcoConfig Pointer to the ORX NCO config settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_OrxNcoSet(adi_adrv904x_Device_t* const                device,
                                                        const adi_adrv904x_ORxNcoConfig_t * const   orxNcoConfig);

/**
* \brief Sets the specified Orx NCOs to the given frequency, band number, and enable state in the adi_adrv904x_ORxNcoConfig_t structure.
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
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in,out] orxNcoConfig Pointer to the ORX NCO config settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_OrxNcoSet_v2(adi_adrv904x_Device_t* const                device,
                                                           const adi_adrv904x_ORxNcoConfig_t * const   orxNcoConfig);

/**
* \brief Gets ORX NCO parameters in adi_adrv904x_OrxNcoConfigReadback_t structure.
*
* \pre This function can be called after the CPU has been initialized, firmware loaded and the ORX NCO configured.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in,out] orxRbConfig Pointer to the ORX NCO config read back settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_OrxNcoGet(adi_adrv904x_Device_t* const                    device,
                                                        adi_adrv904x_ORxNcoConfigReadbackResp_t* const  orxRbConfig);

/**
* \brief Function to configure Rx decimated power measurement block.
*
* This function can be used to configure decimated power measurements. User can either manually enable/disable
* decimated power measurements or allow AGC block to control power measurements. Measurement point can be selected
* for main path decimated power block only. This function configures Rx dec power block. For Orx dec power block 
* please use adi_adrv904x_ORxDecimatedPowerCfgSet function
* Note: The function does not check for duplicated channel assignments across cfg structures.
* Invalid channel mask values will return an error.

* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] rxDecPowerCfg array of structures of type adi_adrv904x_RxDecimatedPowerCfg_t which will configure one or more channels
* \param[in] numOfDecPowerCfgs number of configurations passed in the array
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDecimatedPowerCfgSet(adi_adrv904x_Device_t * const device,
                                                                     adi_adrv904x_RxDecimatedPowerCfg_t rxDecPowerCfg[],
                                                                     const uint32_t numOfDecPowerCfgs);

/**
* \brief Function to read back Rx decimated power measurement configuration
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] rxChannel Rx channel selection to read decimated power block configuration
* \param[in] decPowerBlockSelection Decimated power block selection to read back configuration
* \param[out] rxDecPowerCfg Decimated power configuration of selected channel/dec power block
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDecimatedPowerCfgGet(adi_adrv904x_Device_t * const device,
                                                                     const adi_adrv904x_RxChannels_e rxChannel,
                                                                     const adi_adrv904x_DecPowerMeasurementBlock_e decPowerBlockSelection,
                                                                     adi_adrv904x_RxDecimatedPowerCfg_t * const rxDecPowerCfg);

/**
* \brief Function to readback the decimated power measurement for selected Rx/Orx channel.
* Power readback value is an unsigned 8 bit value with 0.25dB resolution.
* This function can be used to read power measurements for both Rx and Orx channels

* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] rxChannel Channel selection to read decimated power measurement
* \param[in] decPowerBlockSelection Decimated power block selection (unused for Orx channel selections)
* \param[out] powerReadBack Pointer to power measurement readback value
* 
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDecimatedPowerGet(adi_adrv904x_Device_t * const device,
                                                                  const adi_adrv904x_RxChannels_e rxChannel,
                                                                  const adi_adrv904x_DecPowerMeasurementBlock_e decPowerBlockSelection,
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
 * \param [in,out] device Context variable adi_adrv904x_Device_t pointer to the device data structure.
 * \param [in] channelMask uint32_t indicates the channels affected.
 * \param [in] attenDb uint8_t indicates the attenuation level to apply in Db.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_OrxAttenSet(adi_adrv904x_Device_t* const device,
                                                          const uint32_t channelMask,
                                                          const uint8_t attenDb);

/**
 * \brief Get the attenuation of an ORx channel.
 * 
 * \param [in, out] device Context variable adi_adrv904x_Device_t pointer to the device data structure.
 * \param [in] channelId uint8_t indicates the channels affected.
 * \param [out] attenDb uint8_t the channel's attenuation value is written here.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_OrxAttenGet(adi_adrv904x_Device_t* const device,
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
 * \param [in,out] device Context variable adi_adrv904x_Device_t pointer to the device data structure
 * \param [in] gainCtrlModeCfg array to be configured. Parameter is of structure type adi_adrv904x_RxGainCtrlModeCfg_t
 * \param [in] arraySize is the no. of elements in gainCtrlModeCfg[] array
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainCtrlModeSet(adi_adrv904x_Device_t* const            device,
                                                                const adi_adrv904x_RxGainCtrlModeCfg_t  gainCtrlModeCfg[],
                                                                const uint32_t                          arraySize);

/**
 * \brief get the Rx gain control mode with Rx channel index
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in,out] device Context variable adi_adrv904x_Device_t pointer to the device data structure
 * \param [in] rxChannel indicates the Rx channel affected.
 * \param [out] gainCtrlMode is the pointer to adi_adrv904x_RxGainCtrlMode_e enum which will
 *              be updated with the read back config
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainCtrlModeGet(adi_adrv904x_Device_t* const     device,
                                                                const adi_adrv904x_RxChannels_e  rxChannel,
                                                                adi_adrv904x_RxGainCtrlMode_e*   gainCtrlMode);

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
 * \param [in,out] device Context variable adi_adrv904x_Device_t pointer to the device data structure
 * \param [in] rxChannelMask Bit mask representing channels to be programmed (can be multiple channels).
 * \param [in] gainValue Gain value to set
 * 
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxTempGainCompSet(adi_adrv904x_Device_t* const  device,
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
 * \param [in,out] device Context variable adi_adrv904x_Device_t pointer to the device data structure
 * \param [in] rxChannel Bit mask representing which channel to retrieve (only one channel returned per call)
 * \param [out] gainValue Gain value retrieved
 * 
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxTempGainCompGet(adi_adrv904x_Device_t* const     device,
                                                                const adi_adrv904x_RxChannels_e  rxChannel,
                                                                int8_t* const                    gainValue);

/**
 * \brief Allows the data from one or more Rx ADCs to be replaced with a test signal.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in,out] device Context variable adi_adrv904x_Device_t pointer to the device data structure
 * \param [in] rxChannelMask Indicates the channels to be affected by the call.
 * \param [in] rxTestDataCfg Indicates whether the ADC output signal should be replaced by a test signal for the 
 *      channels in rxChannelMask or whether the normal ADC output should be used. Also describes the test signal.
 * 
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxTestDataSet(adi_adrv904x_Device_t* device,
                                                            const uint32_t rxChannelMask,
                                                            const adi_adrv904x_RxTestDataCfg_t* const rxTestDataCfg);

/**
 * \brief Check if the data from an Rx ADC is currently being replaced with a test signal and
 * if so retrieves the parameters of the test signal.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in,out] device Context variable adi_adrv904x_Device_t pointer to the device data structure
 * \param [in] rxChannel The channel whose information to get.
 * \param [out] rxTestDataCfg Indicates whether the Rx ADC signal is being replaced by a test signal
 *     and if so also the parameters of the test signal.
 * 
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxTestDataGet(adi_adrv904x_Device_t* device,
                                                            const adi_adrv904x_RxChannels_e rxChannel,
                                                            adi_adrv904x_RxTestDataCfg_t* const rxTestDataCfg);

/**
* \brief Controls whether Rx LO mixer is powered down or not when an Rx channel is disabled.
* 
* By default the LO is powered down when the Rx is disabled.
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param [in,out] device Context variable adi_adrv904x_Device_t pointer to the device data structure
* \param [in] rxChannelMask The Rx channels to affect. Must include at least one channel from Rx0 to Rx7.
* \param [in] enable To change the default behavour and skip the Rx LO mixer power down for the channels in
*     rxChannelMask set 'enable' to ADI_DISABLE. Restore the default by setting to ADI_ENABLE.
* 
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxLoPowerDownSet(adi_adrv904x_Device_t* const    device, 
                                                               const adi_adrv904x_RxChannels_e rxChannelMask,
                                                               const uint8_t                   enable);


/**
 * \brief Configure Rx Carrier RSSI (Recieved Signal Strength Indicator).
 *
 * This function can be used to configure Rx Carrier RSSI's.
 * Note: The function does not check for duplicated channel assignments across cfg structures.
 * Invalid rxCarrier mask values will return an error
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param[in] rxCarrierRssiCfg array of structures of type adi_adrv904x_RxCarrierRssiCfg_t which will configure one or more channels/carriers
 * \param[in] numCfgs number of configurations passed in the array
 * 
 * \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierRssiCfgSet(adi_adrv904x_Device_t * const   device,
                                                                  adi_adrv904x_RxCarrierRssiCfg_t rxCarrierRssiCfg[],
                                                                  const uint32_t                  numCfgs);

/**
 * \brief Readback Rx Carrier RSSI (Recieved Signal Strength Indicator) configuration.
 *
 * Invalid channel mask values will return an error. Must select a single 
 * rxChannel and single rxCarrier combination in mask to be valid.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param[in] rxCarrierMask Rx Carrier selection to read RSSI configuration.
 *  carrierMask takes single value from adi_adrv904x_RxCarrier_e, rxChannelMask allows only single value from adi_adrv904x_RxChannels_e.
 * \param[out] cfg Rx Carrier RSSI configuration of selected Rx Carrier
 * 
 * \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierRssiCfgGet(adi_adrv904x_Device_t * const              device,
                                                                  const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask,
                                                                  adi_adrv904x_RxCarrierRssiCfg_t * const    cfg);

/**
* \brief Enable/Disable Rx Carrier RSSIs
*
* This function is used to enable/disable Rx Carrier RSSIs after configuring
* them as desired using adi_adrv904x_RxCarrierRssiCfgSet(). If measMode = IMMEDIATE,
* an RSSI transition from Disabled to Enabled will automatically trigger the start
* of a measurement. In other measMode's, enabled RSSI measurements will start after
* it's Channels RxOn signal (delayed by RSSI's configured startDelay value).
*
* rxCarrierMask Parameter:
* The bits set in this bit-mask indicate the Rx Carrier RSSIs affected by this call respectively. 
* Rx Channels and Carriers whose bit are not set to 1 in this parameter will not be affected by this function call. 
* Whether an affected carrier is to be enabled or disabled is indicated by the bit in 
* the rxCarrierEnable parameter. The first carrier is indicated by the least significant bit, 
* the second carrier by the next least significant and so on. It is an error to set a bit corresponding
* to a non-existent Rx Carrier.
*
* rxCarrierEnable Parameter:
* A bit mask indicating if the Rx Carrier is to be enabled (1) or disabled. (0).
* Bits relate to Rx Carriers in the same manner as the rxCarrierMask parameters.
* If the bit for a Rx Carrier in the rxCarrierMask is not set, then the bit
* in this parameters has no effect.
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] rxCarrierMask Mask to select Rx Carrier RSSIs to be affected with this function call
* \param[in] rxCarrierEnable Mask to enable/disable selected Rx Carrier RSSIs
* 
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierRssiEnable(adi_adrv904x_Device_t * const              device,
                                                                  const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask,
                                                                  const uint32_t rxCarrierEnable);

/**
* \brief Manual Start Rx Carrier RSSIs
*
* This function is used to manually start any Enabled Rx Carrier RSSIs. No matter which
* measMode is configured, if a target RSSI is enabled, a measurement will start.
*
* rxCarrierMask Parameter:
* The bits set in this bit-mask indicate the Rx Carrier RSSIs affected by this call respectively. 
* Rx Channel and Carriers whose bit are set to 1 in this parameter will be started by this function call. 
* The first carrier is indicated by the least significant bit, the second carrier by
* the next least significant and so on. It is an error to set a bit corresponding
* to a non-existent Rx Carrier.
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] rxCarrierMask Mask to select Rx Carrier RSSIs to be manually started with this function call
* 
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierRssiManualStart(adi_adrv904x_Device_t * const              device,
                                                                       const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask);

/**
 * \brief Manual Pause/Resume Rx Carrier RSSIs
 *
 * This function is used to manually pause/resume Rx Carrier RSSIs.
 *
 * rxCarrierMask Parameter:
 * The bits set in this bit-mask indicate the Rx Carrier RSSIs affected by this call respectively. 
 * Rx Channel and Carriers whose bit are not set to 1 in this parameter will not be affected by this function call. 
 * Whether an affected carrier is to be paused or resumed is indicated by the bit in
 * the rxCarrierPause parameter. The first carrier is indicated by the least significant bit, 
 * the second carrier by the next least significant and so on. It is an error to set a bit corresponding
 * to a non-existent Rx Carrier.
 *
 * rxCarrierPause Parameter:
 * A bit mask indicating if the Rx Carrier is to be paused (1) or resumed (0). 
 * Bits relate to Rx Carriers in the same manner as the rxCarrierMask parameters.
 * If the bit for a Rx Carrier in the rxCarrierMask is not set, then the bit
 * in this parameters has no effect.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param[in] rxCarrierMask Mask to select Rx Carrier RSSIs to be affected with this function call
 * \param[in] rxCarrierPause Mask to pause/resume selected Rx Carrier RSSIs
 * 
 * \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierRssiManualPause(adi_adrv904x_Device_t * const              device,
                                                                       const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask,
                                                                       const uint32_t rxCarrierPause);

/**
* \brief Configure Rx Carrier RSSI GPIO pin selections
*
* This function can be used to configure Rx Carrier RSSI's GPIO pins. All Rx Channels
* are configured at once, both input and output pins. Rx Channels can share Input GPIOs,
* but Output GPIO selections must be unique. In general, GPIOs that are selected must
* not be allocated for use in other features when calling this function. If any GPIO
* selection made in this function call is already allocated, an error will be returned.
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] gpioCfg Pointer to structure containing all Rx Carrier RSSI GPIO pin selections
* 
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierRssiGpioCfgSet(adi_adrv904x_Device_t * const                     device,
                                                                      const adi_adrv904x_RxCarrierRssiGpioCfg_t * const gpioCfg);

/**
 * \brief Readback Rx Carrier RSSI GPIO pin configuration.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param[out] gpioCfg Pointer to structure to populate with all Rx Carrier RSSI GPIO pin selections
 * 
 * \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierRssiGpioCfgGet(adi_adrv904x_Device_t * const               device,
                                                                      adi_adrv904x_RxCarrierRssiGpioCfg_t * const gpioCfg);


/**
 * \brief Readback most recent power measurement from a Rx Carrier RSSI in mdB.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param[in] rxCarrierMask Rx Carrier selection from which to read RSSI power measurement.
 *  carrierMask takes single value from adi_adrv904x_RxCarrier_e, rxChannelMask allows only single value from adi_adrv904x_RxChannels_e.
 * \param[out] gain_mdB Pointer to location to store power measurement result for selected Rx Carrier. Signed 32bit value, resolution 1 mdB.
 * 
 * \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierRssiPowerRead(adi_adrv904x_Device_t * const              device,
                                                                     const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask,
                                                                     int32_t * const                            gain_mdB);

 /**
 * \brief Adjust Rx Carrier Gain in mdB. Valid range is -90dB to +36dB.
 *
 * Note: Gain adjustment is converted to a linear gain value in 7.16 format 
 * internally, which will affect actual value applied. It is suggested to
 * readback the actual value applied in this function by calling 
 * adi_adrv904x_RxCarrierGainAdjustGet() afterward.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param[in] rxCarrierMask Rx Carrier selection from which to set the gain adjustment
 * \param[in] gain_mdB Gain adjustment to make to selected Rx Carriers. Signed 32bit value, resolution 1 mdB
 * 
 * \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierGainAdjustSet(adi_adrv904x_Device_t * const              device,
                                                                     const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask,
                                                                     const int32_t                              gain_mdB);   

/**
 * \brief Readback current Rx Carrier Gain Adjustment in mdB. Expected range is -90dB to +36dB.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param[in] rxCarrierMask Rx Carrier selection from which to get the gain adjustment
 *  carrierMask takes single value from adi_adrv904x_RxCarrier_e, rxChannelMask allows only single value from adi_adrv904x_RxChannels_e.
 * \param[out] gain_mdB Pointer to location to store current gain adjustment for selected Rx Carrier. Signed 32bit value, resolution 1 mdB.
 * 
 * \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierGainAdjustGet(adi_adrv904x_Device_t * const              device,
                                                                     const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask,
                                                                     int32_t * const                            gain_mdB);

/** 
* \brief Reconfigure Rx carriers dynamically without reinitialization - Solving without applying solution to HW
* 
* \dep_begin 
* \dep{device->common.devHalInfo} 
* \dep_end 
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings 
* \param[in] jesdCfg - Pointer that holds the updated JESD settings to accomdate the new carrier settings
* \param[in] rxCarrierConfigs - Rx Carrier Reconfigure Settings. Struct may be modified by API for endianness before being passed to firmware.
* \param[in] rxCarrierChannelFilterApplicationSel - Rx Carrier Channel Filter application select for each carrier in each profile. The carriers here are applied to the corresponding channelMask in rxCarrierConfigs.
* \param[in] rxCarrierChannelFilter - Rx Carrier Channel Filter Settings. Struct may be modified by API for endianness before being passed to firmware.
* \param[in] numCarrierProfiles number of profiles passed in the arrays. Max is four.
* \param[in] useCustomFilters Select option to use custom filters or ADI presets: 0: Use rxCarrierChannelFilterApplicationSel, 1: Use rxCarrierChannelFilter
*
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful 
*/ 
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDynamicCarrierCalculate(adi_adrv904x_Device_t* const                                device,
                                                                        adi_adrv904x_CarrierJesdCfg_t* const                        jesdCfg,
                                                                        adi_adrv904x_CarrierRadioCfg_t                              rxCarrierConfigs[],
                                                                        const adi_adrv904x_CarrierChannelFilterApplicationSel_t     rxCarrierChannelFilterApplicationSel[],
                                                                        adi_adrv904x_ChannelFilterCfg_t                             rxCarrierChannelFilter[],
                                                                        const uint32_t                                              numCarrierProfiles,
                                                                        const uint8_t                                               useCustomFilters);

/** 
* \brief Reconfigure Rx carriers dynamically without reinitialization - Apply previously solved configuration
* 
* \dep_begin 
* \dep{device->common.devHalInfo} 
* \dep_end 
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings 
*
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful 
*/ 
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDynamicCarrierWrite(adi_adrv904x_Device_t* const device);

/** 
* \brief Reconfigure Rx carriers dynamically without reinitialization. Wrapper for adi_adrv904x_RxDynamicCarrierReconfigure to select channel filter coefficients
*           provided by ADI
* 
*           Note: The API currently only uses the first profile in rxCarrierConfigs.carrierCfgs input struct.
* 
* \pre Part must be put in the appropriate state for this function to work as intendend. At a minimum the following steps must have been performed:
*           Disable tracking cals
*           Disable PAs
*           Disable Tx/Rx/ORx channels. GPIOs are not toggling
* 
* 
* \post After this function is called at a minimum the following steps must be performed before the device can be used as intended:
*           Provide new CFR pulse/config thru API
*           Change RFLO
*           Re- Enable channels then tracking cals
*           Re-Enable PAs
*           
* \dep_begin 
* \dep{device->common.devHalInfo} 
* \dep_end 
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings 
* \param[in] jesdCfg - Pointer that holds the updated JESD settings to accomdate the new carrier settings
* \param[in] rxCarrierConfigs - Rx Carrier Reconfigure Settings. Struct may be modified by API for endianness before being passed to firmware.
* \param[in] rxCarrierChannelFilterApplicationSel - Rx Carrier Channel Filter application select for each carrier in each profile. The carriers here are applied to the corresponding channelMask in rxCarrierConfigs.
* \param[in] numCarrierProfiles number of profiles passed in the arrays. Max is four.
*
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful 
*/ 
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDynamicCarrierReconfigureWithFilterSelect(  adi_adrv904x_Device_t* const                                device,
                                                                                            adi_adrv904x_CarrierJesdCfg_t* const                        jesdCfg,
                                                                                            adi_adrv904x_CarrierRadioCfg_t                              rxCarrierConfigs[],
                                                                                            const adi_adrv904x_CarrierChannelFilterApplicationSel_t     rxCarrierChannelFilterApplicationSel[],
                                                                                            const uint32_t                                              numCarrierProfiles);

/** 
* \brief Reconfigure Rx carriers dynamically without reinitialization
* 
*           Note: The API currently only uses the first profile in rxCarrierConfigs.carrierCfgs input struct.
* 
* \pre Part must be put in the appropriate state for this function to work as intendend. At a minimum the following steps must have been performed:
*           Disable tracking cals
*           Disable PAs
*           Disable Tx/Rx/ORx channels. GPIOs are not toggling
* 
* 
* \post After this function is called at a minimum the following steps must be performed before the device can be used as intended:
*           Provide new CFR pulse/config thru API
*           Change RFLO
*           Re- Enable channels then tracking cals
*           Re-Enable PAs
*           
* \dep_begin 
* \dep{device->common.devHalInfo} 
* \dep_end 
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings 
* \param[in] jesdCfg - Pointer that holds the updated JESD settings to accomdate the new carrier settings
* \param[in] rxCarrierConfigs - Rx Carrier Reconfigure Settings. Struct may be modified by API for endianness before being passed to firmware.
* \param[in] rxCarrierChannelFilter - Rx Carrier Channel Filter Settings. Struct may be modified by API for endianness before being passed to firmware.
* \param[in] numCarrierProfiles number of profiles passed in the arrays. Max is four.
*
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful 
*/ 
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDynamicCarrierReconfigure(  adi_adrv904x_Device_t* const            device,
                                                                            adi_adrv904x_CarrierJesdCfg_t* const    jesdCfg,
                                                                            adi_adrv904x_CarrierRadioCfg_t          rxCarrierConfigs[],
                                                                            adi_adrv904x_ChannelFilterCfg_t         rxCarrierChannelFilter[],
                                                                            const uint32_t                          numCarrierProfiles);

/** 
* \brief Retrieve the current Rx Carrier settings for the selected channel
*           
* \dep_begin 
* \dep{device->common.devHalInfo} 
* \dep_end 
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings 
* \param[in] rxChannel - Rx Channel to be retrieved
* \param[out] rxCarrierConfigs - Rx Carrier Reconfigure Settings
* \param[out] carrierRuntimeSettings - Rx Carrier Runtime Settings
*
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful 
*/ 
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierSettingsGet(  adi_adrv904x_Device_t* const    device,
                                                                    const uint32_t rxChannel,
                                                                    adi_adrv904x_CarrierRadioCfg_t* const rxCarrierConfigs,
                                                                    adi_adrv904x_CarrierRuntime_t* const  carrierRuntimeSettings);

/** 
* \brief Retrieve the current rx jesd settings
*           
* \dep_begin 
* \dep{device->common.devHalInfo} 
* \dep_end 
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] rxCarrierJesdCfg - Rx Carrier Reconfigure Settings
*
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful 
*/ 
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierJesdCfgGet(  adi_adrv904x_Device_t* const    device,
                                                                    adi_adrv904x_CarrierJesdCfg_t* const rxCarrierJesdCfg);

/**
* \brief Retrieve the current Rx Carrier delay value for the selected channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] rxChannel Channel selection to read latency values
* \param[out] rxCarrierLatencyCfg - Rx Carrier delay settings 
*
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierLatencyGet(  adi_adrv904x_Device_t* const device,
                                                                    const adi_adrv904x_RxChannels_e rxChannel,
                                                                    adi_adrv904x_CarrierReconfigLatencyCfg_t* const rxCarrierLatencyCfg);

/**
 * \brief       Set the RSSI (DDC/CDDC) power measurement configuration
 * This function sets the configuration of the RSSI (DDC/CDDC) power measurement.
 * Must use adi_adrv904x_DfePwrMtrRssiEnable() to enable the meter.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in]  meterMask       - DDC or CDDC type
 * \param [in]  channelMask     - Rx channel number mask; multiple channels allowed
 * \param [in]  pPwrMtrRssiCfg  - pointer to power meter config structure
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxRssiConfigSet(adi_adrv904x_Device_t* const          device,
                                                              const uint16_t                        meterMask,
                                                              const uint32_t                        channelMask,
                                                              const adi_adrv904x_RxRssiCfg_t* const pPwrMtrRssiCfg);
/**
 * \brief       Get the RSSI (DDC/CDDC)power measurement configuration
 * This function gets the configuration of the RSSI (DDC/CDDC) power measurement
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in]  meterSel        - Meter mask; only one channel allowed
 * \param [in]  channelSel      - Rx channel number mask; only one channel allowed
 * \param [out] pPwrMtrRssiCfg  - pointer to power meter config structure
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxRssiConfigGet(adi_adrv904x_Device_t* const    device,
                                                              const uint16_t                  meterSel,
                                                              const uint32_t                  channelSel,
                                                              adi_adrv904x_RxRssiCfg_t* const pPwrMtrRssiCfg);

/**
 * \brief       Enable RSSI (DDC/CDDC) power measurement
 * This function enables the RSSI (DDC/CDDC) power measurement
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in]  meterMask   - Meter mask; multiple meters allowed
 * \param [in]  channelMask - Rx channel number mask; multiple channels allowed
 * \param [in]  enable      - enable/disable power meter
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxRssiEnable(adi_adrv904x_Device_t* const device,
                                                           const uint16_t               meterMask,
                                                           const uint32_t               channelMask,
                                                           const uint8_t                enable);
/**
 * \brief       Readback RSSI (DDC/CDDC) power measurement
 * This function reads back the RSSI (DDC/CDDC) power measurement
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in]  meterSel   - Meter mask (only one allowed)
 * \param [in]  channelSel - Rx channel number mask (only one channel allowed for readback)
 * \param [out] pPwrMeasDb - Pointer to power meter read back in dB when applicable
 * \param [out] pPwrMeasLinear - Pointer to power meter read back linear value (in dBm)
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxRssiReadBack(adi_adrv904x_Device_t* const device,
                                                             const uint16_t               meterSel,
                                                             const uint32_t               channelSel,
                                                             float* const                 pPwrMeasDb,
                                                             float* const                 pPwrMeasLinear);

#endif /* _ADI_ADRV904X_RX_H_ */
