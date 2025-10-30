/**
 * Copyright 2015 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adrv903x_rx.h
 * \brief Contains ADRV903X Rx related private function prototypes for
 *        adrv903x_rx.c which helps adi_adrv903x_rx.c
 *
 * ADRV903X API Version: 2.12.1.4
 */ 

#ifndef _ADRV903X_RX_H_
#define _ADRV903X_RX_H_

#include "../../private/bf/adrv903x_bf_rx_dig_types.h"
#include "../../private/bf/adrv903x_bf_rx_funcs_types.h"
#include "../../private/bf/adrv903x_bf_rx_ddc_types.h"
#include "../../private/bf/adrv903x_bf_orx_dig_types.h"
#include "../../private/bf/adrv903x_bf_tdr_dpath_top.h"
#include "../../private/bf/adrv903x_bf_actrl_orx_west_regmap.h"
#include "adrv903x_reg_addr_macros.h"
#include "adrv903x_rx_types.h"


#include "adi_adrv903x_error.h"
#include "adi_adrv903x_rx.h"

/**
* \brief Sets RX Data Formatter configuration.
*
* The slicer/formatter work in conjunction with the gain compensation module.
* The digital gain compensation module compensates for the front end attenuation 
* thereby increasing the bit resolution of the Rx data word. The slicer is then
* applied to attenuate the data such that it can fit into the JESD data path resolution.
*
* This function will set the data format for the receive data on JESD link. This function 
* also configures the formatter / slicer if any of the integer slicer modes are required 
* to be configured. In addition to the above, any selected internal slicer GPIOs are acquired
* and configured by this function.
*
* When using integer formatting, the number of slicer bits as well as their position 
* in the data word are selectable.
* 
* The user can select the data format mode from one of the following 
*
*            rxDataFormat[i]->formatSelect      |  Description
*  ---------------------------------------------|--------------------------------------
*       ADI_ADRV903X_GAIN_COMPENSATION_DISABLED | Disables gain compensation and Rx data formatting
*                                               |
* ADI_ADRV903X_GAIN_WITH_INTERNAL_SLICER_NOGPIO | Setup Rx data format to an integer format such
*                                               | that the gain compensation data(slicer) is embedded
*                                               | in the I and Q data. It is possible to embed 2/3/4
*                                               | slicer bits providing a dynamic gain compensation range 
*                                               | of 50dB. Applicable for 16-bit / 12-bit resolution
*                                               |
*        ADI_ADRV903X_GAIN_WITH_INTERNAL_SLICER | Setup Rx data format to an integer format such
*                                               | that the gain compensation data(slicer) is encoded
*                                               | in the GPIO output from ADRV903X. It is possible to encode 2/3/4
*                                               | slicer bits providing a dynamic gain compensation range
*                                               | of 50dB. Applicable for 16-bit / 12-bit resolution
*                                               |
*      ADI_ADRV903X_EMBED_OVERLOAD_MONITOR_DATA | Setup overload indicators to be embedded in the 16 bit
*                                               | 2s complement data. The overload indicators can be embedded
*                                               | at LSB and/or LSB+1 positions of I and Q samples.
*         ADI_ADRV903X_GAIN_WITH_FLOATING_POINT | -- MODE NOT SUPPORTED --
*        ADI_ADRV903X_GAIN_WITH_EXTERNAL_SLICER | -- MODE NOT SUPPORTED --
*
* The ADRV903X data formatting should be used in conjunction with the
* gain compensation feature.
*
* Please note that for ORx channels ADI_ADRV903X_GAIN_WITH_INTERNAL_SLICER_NOGPIO is the only valid mode.
*
* Please note that while disabling gain compensation a valid data format represented by 
* rxDataFormat[i].integerConfigSettings.intSampleResolution has to be set to a valid mode. Please refer to enum
* adi_adrv903x_RxIntSampleResolution_e for valid intSampleResolution modes
*
* It has to be ensured that the ADRV903X JESD Framer Np for the selected Rx channel is at least equal to 
* integer sample resolution size.
*
* In a 2 bit embedded slicer mode (ADI_ADRV903X_GAIN_WITH_INTERNAL_SLICER_NOGPIO), slicer position bit 0 
* is embedded in Q data from JESD and slicer position bit 1 is embedded in I data from JESD.
*
* In a 4 bit embedded slicer mode (ADI_ADRV903X_GAIN_WITH_INTERNAL_SLICER_NOGPIO), slicer position bit 0
* and bit1 are embedded in Q data from JESD and slicer position bit 2 and bit 3 are embedded in I data from JESD.
*
* The slicer position can also be read back via adi_adrv903x_RxSlicerPositionGet() API
*
* The slicer position in the internal slicer mode (ADI_ADRV903X_GAIN_WITH_INTERNAL_SLICER) can be observed on
* any GPIO[23:0].
*
* Please note that only the least significant 3/4 bits of the slicer control word is output on the GPIOs.
*
* User needs to enable fast recovery and configure AGC block with adi_adrv903x_AgcCfgSet API before using
* HB2 int0/int1 threshold and counter exceeded embedded overload flags
*
* \pre Slicer/Formatter operation requires that the device be loaded with a gain table that compensates
*      for the analog front end attenuation applied.
* \pre JESD datawidth should match the data width for the requested Rx Format.
* \pre JESD initialization should be complete.
* \pre User must ensure that selected GPIOs are not already configured for another 
*      feature when configuring slicer formats involving GPIOS.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV903X device data structure
* \param[in] rxDataFormat Pointer to the Rx data format configuration structure
* \param[in] arraySize Size of rxDataFormat array representing no. of configs
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDataFormatSet(adi_adrv903x_Device_t * const       device,
                                                          const adi_adrv903x_RxDataFormatRt_t rxDataFormat[],
                                                          const uint32_t                      arraySize);

#ifndef CLIENT_IGNORE
/**
* \brief Formats gain table structure input into a format expected by ARM DMA
*
* Format Gain Table Data as follows :
* No. of Bytes per Gain Table Entry = 8
* Addr 8 x gainIndex     : {13'b0 , rx_fe_gain[7:0], dig_gain[10:0]}
* Addr 8 x gainIndex + 4 : {14'b0, ext_control[1:0], phase_offset[15:0]}
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Context variable - Structure pointer to the ADRV903X device data structure
* \param[in] gainTablePtr Input gain table
* \param[out] formattedGainTablePtr memory to store formatted gain table data
* \param[in] numGainIndicesInTable number of gain table entries
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxGainTableFormat(adi_adrv903x_Device_t* const        device,
                                                            const adi_adrv903x_RxGainTableRow_t gainTablePtr[],
                                                            uint8_t                             formattedGainTablePtr[],
                                                            const uint16_t                      numGainIndicesInTable);

/**
* \brief Calculates parameters required to read Rx Gain Table from ADRV903X SRAM
*  
*  This function calculates the number of gain indices to read and the SRAM base
*  address for the read operation given the maximum read size and gain index offset
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Context variable - Structure pointer to the ADRV903X device data structure
* \param[in] rxChannel is the channel for which gain table read is requested
* \param[in] maxReadGainIndices is the maximum no. of gain indices allowed to be read
* \param[in] gainIndexOffset is the off-set index from maximum gain index which is 
*        the starting gain index to read from
* \param[out] numGainIndicesToRead output from this function which provides the no. of gain table 
*        row entry reads to be performed
* \param[out] baseAddr is the starting SRAM address for read operation (output)
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxGainTableReadParamsCompute(adi_adrv903x_Device_t* const    device,
                                                                       const adi_adrv903x_RxChannels_e rxChannel,
                                                                       const uint32_t                  maxReadGainIndices,
                                                                       const uint8_t                   gainIndexOffset,
                                                                       uint16_t* const                 numGainIndicesToRead,
                                                                       uint32_t* const                 baseAddr);

/**
* \brief Parses gain table in ARM DMA data format into a gain table row structure
*
* Format Gain Table Data as follows :
* No. of Bytes per Gain Table Entry = 8
* cpuDmaDataGainIndex[8 x gainIndex]     : {13'b0 , rx_fe_gain[7:0], dig_gain[10:0]}
* cpuDmaDataGainIndex[8 x gainIndex + 4] : {14'b0,  ext_control[1:0], phase_offset[15:0]}
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Context variable - Structure pointer to the ADRV903X device data structure
* \param[out] gainTablePtr pointer to gain table row entry structure where parsed data is stored
* \param[in] cpuDmaDataGainTablePtr memory containing data from ARM DMA
* \param[in] numGainIndicesInTable number of gain table entries
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxGainTableParse(adi_adrv903x_Device_t* const    device,
                                                           adi_adrv903x_RxGainTableRow_t   gainTablePtr[],
                                                           const uint8_t                   cpuDmaDataGainTablePtr[],
                                                           const uint16_t                  numGainIndicesInTable);


/**
* \brief Performs range check on rx gain index setting.
*
* \pre This function is called automatically as part of adi_adrv903x_RxGainSet()
*      function if ADI_ADRV903X_RX_RANGE_CHECK > 0.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X device data structure
* \param[in] rxGain is the channelized gain setting parameter
* \param[in] arraySize Size of rxGain array representing no. of configs.
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxGainSetRangeCheck(adi_adrv903x_Device_t* const    device,
                                                              const adi_adrv903x_RxGain_t     rxGain[],
                                                              const uint32_t                  arraySize);


/**
* \brief Performs range check on AGC/MGC gain get API.
*
* \pre This function is called automatically as part of AGC/MGC gain get API.
*      function if ADI_ADRV903X_RX_RANGE_CHECK > 0.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] rxChannel Channel selection to read back gain index for (Valid Rx0-Rx7 only)
* \param[out] rxGain Pointer to the specified channel gain index value
*             which will be updated by this function
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxGainGetRangeCheck(adi_adrv903x_Device_t* const    device,
                                                              const adi_adrv903x_RxChannels_e rxChannel,
                                                              adi_adrv903x_RxGain_t * const   rxGain);

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
ADI_API adi_adrv903x_ErrAction_e adrv903x_DecPowerCfgRangeCheck(adi_adrv903x_Device_t* const device,
                                                                const adi_adrv903x_RxDecimatedPowerCfg_t* const decPowerCfg);

/**
* \brief Performs range check on Orx Dec power configuration.
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
ADI_API adi_adrv903x_ErrAction_e adrv903x_OrxDecPowerCfgRangeCheck(adi_adrv903x_Device_t* const device,
                                                                   const adi_adrv903x_ORxDecimatedPowerCfg_t* const decPowerCfg);


typedef adrv903x_BfRxDigChanAddr_e adrv903x_BfRxChanAddr_e;
#define ADRV903X_BF_RX_CH0 ADRV903X_BF_SLICE_RX_0__RX_DIG
#define ADRV903X_BF_RX_CH1 ADRV903X_BF_SLICE_RX_1__RX_DIG
#define ADRV903X_BF_RX_CH2 ADRV903X_BF_SLICE_RX_2__RX_DIG
#define ADRV903X_BF_RX_CH3 ADRV903X_BF_SLICE_RX_3__RX_DIG
#define ADRV903X_BF_RX_CH4 ADRV903X_BF_SLICE_RX_4__RX_DIG
#define ADRV903X_BF_RX_CH5 ADRV903X_BF_SLICE_RX_5__RX_DIG
#define ADRV903X_BF_RX_CH6 ADRV903X_BF_SLICE_RX_6__RX_DIG
#define ADRV903X_BF_RX_CH7 ADRV903X_BF_SLICE_RX_7__RX_DIG

/**
* \brief Look up the Rx digital bitfield address given a Rx channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] rxChannel Channel selection to read back gain index for (Valid Rx0-Rx7 only)
* \param[out] rxChannelBitfieldAddr Rx digital channel bitfield address which will be updated by this function
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxBitfieldAddressGet(adi_adrv903x_Device_t* const    device,
                                                               const adi_adrv903x_RxChannels_e rxChannel,
                                                               adrv903x_BfRxChanAddr_e* const  rxChannelBitfieldAddr);

/**
* \brief Look up the ORx digital bitfield address given a Rx channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] rxChannel Channel selection to read back gain index for (Valid Rx0-Rx7 only)
* \param[out] orxChannelBitfieldAddr ORx digital channel bitfield address which will be updated by this function
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_OrxBitfieldAddressGet(adi_adrv903x_Device_t* const    device,
                                                                const adi_adrv903x_RxChannels_e rxChannel,
                                                                adrv903x_BfOrxDigChanAddr_e* const orxChannelBitfieldAddr);

/**
* \brief Look up the Rx analog bitfield address given a Rx Channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] rxChannel Channel selection to read back gain index for (Valid Rx0-Rx7 only)
* \param[out] rxAnalogChannelIBitfieldAddr Rx analog channel I bitfield address which will be updated by this function
* \param[out] rxAnalogChannelQBitfieldAddr Rx analog channel Q bitfield address which will be updated by this function
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxAnalogBitfieldAddressGet(adi_adrv903x_Device_t* const device,
                                                                     const adi_adrv903x_RxChannels_e rxChannel,
                                                                     adrv903x_BfTdrDpathTopChanAddr_e* const rxAnalogChannelIBitfieldAddr,
                                                                     adrv903x_BfTdrDpathTopChanAddr_e* const rxAnalogChannelQBitfieldAddr);
    
/**
* \brief Look up the Rx function bitfield address given a Rx Channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] rxChannel Channel selection to read back gain index for (Valid Rx0-Rx7 only)
* \param[out] rxFuncsChannelBitfieldAddr Rx function channel bitfield address which will be updated by this function
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxFuncsBitfieldAddressGet(adi_adrv903x_Device_t* const device,
                                                                    const adi_adrv903x_RxChannels_e rxChannel,
                                                                    adrv903x_BfRxFuncsChanAddr_e* const rxFuncsChannelBitfieldAddr);
                                                                    

/**
* \brief Look up the Rx DDC bitfield address given a Rx Channel and Band
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] rxChannel Channel selection
* \param[in] ddcBand Band selection
* \param[out] rxDdcBitfieldAddr Rx DDC channel bitfield address which will be updated by this function
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDdcBitfieldAddressGet(adi_adrv903x_Device_t* const       device,
                                                                  const adi_adrv903x_RxChannels_e    rxChannel,
                                                                  const adi_adrv903x_RxDdcs_e        ddcBand,
                                                                  adrv903x_BfRxDdcChanAddr_e* const  rxDdcBitfieldAddr);

/**
* \brief Helper function for adrv903x_RxDataFormatSet. Validates data in configuration table.
*
* The ADRV903X data formatting should be used in conjunction with the
* gain compensation feature. This function is automatically called as 
* part of adrv903x_RxDataFormatSet() if ADI_ADRV903X_RX_RANGE_CHECK
* is set to 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the device settings structure
* \param[in] rxDataFormat Pointer to the Rx data format configuration structure
* \param[in] arraySize Size of rxDataFormat array representing no. of configs
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDataFormatSetRangeCheck(adi_adrv903x_Device_t * const       device,
                                                                    const adi_adrv903x_RxDataFormatRt_t rxDataFormat[],
                                                                    const uint32_t                      arraySize);

/**
* \brief Helper function for adrv903x_RxDataFormatSetFloatingPointRangeCheck to validate configuration of 
* floating point formatter 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Structure pointer to the ADRV903X device data structure
* \param[in] floatingPointConfig Structure pointer to floating point formatter settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDataFormatSetFloatingPointRangeCheck(adi_adrv903x_Device_t * const                            device,
                                                                                 const adi_adrv903x_FloatingPointConfigSettings_t * const floatingPointConfig);


/**
* \brief Helper function for adrv903x_CddcDataFormatSetFloatingPointRangeCheck to validate configuration of 
* floating point formatter 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Structure pointer to the ADRV903X device data structure
* \param[in] floatingPointConfig Structure pointer to floating point formatter settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_CddcDataFormatSetFloatingPointRangeCheck(adi_adrv903x_Device_t * const                            device,
                                                                                   const adi_adrv903x_CddcFloatingPointConfigSettings_t * const floatingPointConfig);

/**
* \brief Performs range check on Rx integer and slicer format settings
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Structure pointer to the ADRV903X device data structure
* \param[in] rxChannelMask Channels which need to be configured for one of the integer formats
* \param[in] formatSelect Target format mode
* \param[in] integerConfigSettings Structure pointer to integer formatter configuration settings
* \param[in] slicerConfigSettings Structure pointer to slicer configuration settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDataFormatSetIntegerRangeCheck(adi_adrv903x_Device_t * const                      device,
                                                                           const uint32_t                                     rxChannelMask,
                                                                           const adi_adrv903x_RxDataFormatModes_e             formatSelect,
                                                                           const adi_adrv903x_IntegerConfigSettings_t * const integerConfigSettings,
                                                                           const adi_adrv903x_SlicerConfigSettings_t * const  slicerConfigSettings);

/**
* \brief Helper function for adrv903x_RxDataFormatSetRangeCheck to validate configuration of 
* embedded overload monitor formatter 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Structure pointer to the ADRV903X device data structure
* \param[in] embOvldConfigSettings Structure pointer to embedded overload monitor formatter settings
* \param[in] intSampleResolution Integer sample resolution of the JESD Rx data samples
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDataFormatSetEmbOvldMonitorRangeCheck(adi_adrv903x_Device_t * const                                 device,
                                                                                  const adi_adrv903x_EmbOverloadMonitorConfigSettings_t * const embOvldConfigSettings,
                                                                                  const adi_adrv903x_RxIntSampleResolution_e                    intSampleResolution);

/**
* \brief Helper function for adrv903x_CddcDataFormatSetRangeCheck to validate configuration of
* embedded overload monitor formatter
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Structure pointer to the ADRV903X device data structure
* \param[in] embOvldConfigSettings Structure pointer to embedded overload monitor formatter settings
* \param[in] intSampleResolution Integer sample resolution of the JESD Rx data samples
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_CddcDataFormatSetEmbOvldMonitorRangeCheck(adi_adrv903x_Device_t* const                                 device,
                                                                                    const adi_adrv903x_CddcEmbOverloadMonitorConfigSettings_t* const cddcEmbOvldConfigSettings,
                                                                                    const adi_adrv903x_RxIntSampleResolution_e                    intSampleResolution);

/**
* \brief Disables gain compensation for the requested Rx Channels
*
* Please note that even though gain compensation is disabled,
* the embedded slicer bits have to be explicitly set to 0 failing
* which the received data will contain slicer bits if it was previously
* enabled.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Structure pointer to the ADRV903X device data structure
* \param[in] rxChannelMask specifies the channels for which gain compensation is to be disabled
* \param[in] intDataFormat integer data format on disabling gain compensation (2s complement vs signed)
* \param[in] intDataResolution integer data resolution (12/16) on disabling gain compensation
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDataFormatGainCompDisable(adi_adrv903x_Device_t * const device,
                                                                      const uint32_t                rxChannelMask,
                                                                      const uint8_t                 intDataFormat,
                                                                      const uint8_t                 intDataResolution);

/**
* \brief Sets gain compensation for external LNA gain enable
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Structure pointer to the ADRV903X device data structure
* \param[in] rxChannelMask channels which need to be configured
* \param[in] externalLnaGain enable(1) or disable(0) gain compensation to consider LNA gain
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxGainCompExtLnaSet(adi_adrv903x_Device_t * const device,
                                                              const uint32_t                rxChannelMask,
                                                              const uint8_t                 externalLnaGain);

/**
* \brief Sets gain compensation for temperature enable
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Structure pointer to the ADRV903X device data structure
* \param[in] rxChannelMask channels which need to be configured
* \param[in] tempCompensationEnable enable(1) or disable(0) gain compensation to consider temperature
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxGainCompTempEnableSet(adi_adrv903x_Device_t * const device,
                                                                  const uint32_t                rxChannelMask,
                                                                  const uint8_t                 tempCompensationEnable);

/**
* \brief Sets the Rx Data format to floating point format.
*
* Following settings are configured through this function
* -> Floating point data format ({Sign, Exp, Significand} or {Sign, Significand, Exp})
* -> Floating point round mode
* -> No. of exponent bits
* -> Attenuation step size for the floating point config
* -> Leading one for floating point and NaN encoding
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Structure pointer to the ADRV903X device data structure
* \param[in] rxChannelMask Rx channels for which floating point format has to be configured
* \param[in] floatingPointConfig Structure pointer to floating point formatter settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDataFormatFloatingPointSet(adi_adrv903x_Device_t * const                            device,
                                                                       const uint32_t                                           rxChannelMask,
                                                                       const adi_adrv903x_FloatingPointConfigSettings_t * const floatingPointConfig);

/**
* \brief Sets the Rx Data format to integer and configures slicer settings.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Structure pointer to the ADRV903X device data structure
* \param[in] rxChannelMask channels which need to be configured with integer settings requested
* \param[in] formatSelect Integer formatter mode select
* \param[in] integerConfigSettings contains the integer formatter settings for Rx channels requested
* \param[in] slicerConfigSettings contains the integer formatter settings for Rx channels requested
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDataFormatIntegerSet(adi_adrv903x_Device_t * const                      device,
                                                                 const uint32_t                                     rxChannelMask,
                                                                 const adi_adrv903x_RxDataFormatModes_e             formatSelect,
                                                                 const adi_adrv903x_IntegerConfigSettings_t * const integerConfigSettings,
                                                                 const adi_adrv903x_SlicerConfigSettings_t * const  slicerConfigSettings);

/**
* \brief Sets the Rx Data format to embedded overload indicator format and configures the embedded overload monitor settings.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Structure pointer to the ADRV903X device data structure
* \param[in] rxChannelMask Rx channels for which embedded overload indicator format has to be configured
* \param[in] embOvldConfigSettings Structure pointer to embedded overload monitor formatter settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDataFormatEmbOvldMonitorSet(adi_adrv903x_Device_t * const                                 device,
                                                                        const uint32_t                                                rxChannelMask,
                                                                        const adi_adrv903x_EmbOverloadMonitorConfigSettings_t * const embOvldConfigSettings);

/**
* \brief Sets the Rx Data format embedded overload bits
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device's data structure.
* \param[in] chanIdx Rx channels selection
* \param[in] lsbIData I data selection
* \param[in] lsbIPlusData I+1 data selection
* \param[in] lsbQData Q data selection
* \param[in] lsbQPlusData Q+1 data selection
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxEmbeddedOverloadBitsSet(adi_adrv903x_Device_t * const device,
                                                                    uint8_t chanIdx,
                                                                    uint8_t lsbIData,
                                                                    uint8_t lsbIPlusData,
                                                                    uint8_t lsbQData,
                                                                    uint8_t lsbQPlusData);

/**
* \brief Sets the CDDC Data format embedded overload bits
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device's data structure.
* \param[in] chanIdx Rx channels selection
* \param[in] lsbIData I data selection
* \param[in] lsbIPlusData I+1 data selection
* \param[in] lsbQData Q data selection
* \param[in] lsbQPlusData Q+1 data selection
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_CddcEmbeddedOverloadBitsSet(adi_adrv903x_Device_t * const device,
                                                                      uint8_t chanIdx,
                                                                      uint8_t lsbIData,
                                                                      uint8_t lsbIPlusData,
                                                                      uint8_t lsbQData,
                                                                      uint8_t lsbQPlusData);

/**
* \brief Resolves the integer data resolution and format for integer formatter.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Structure pointer to the ADRV903X device data structure
* \param[in] integerConfigSettings Structure pointer to integer formatter settings
* \param[out] intDataResolution pointer updated with integer resolution. 12bit = 0, 16bit = 1.
* \param[out] intDataFormat pointer updated integer data format. 2's complement = 0, signed magnitude = 1.
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxIntSampleResFormatResolve(adi_adrv903x_Device_t * const                      device,
                                                                      const adi_adrv903x_IntegerConfigSettings_t * const integerConfigSettings,
                                                                      uint8_t * const                                    intDataResolution,
                                                                      uint8_t * const                                    intDataFormat);

/**
* \brief Resolves the integer data resolution and format for integer formatter.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Structure pointer to the ADRV903X device data structure
* \param[in] cddcIntegerConfigSettings Structure pointer to integer formatter settings
* \param[out] intDataResolution pointer updated with integer resolution. 12bit = 0, 16bit = 1.
* \param[out] intDataFormat pointer updated integer data format. 2's complement = 0, signed magnitude = 1.
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_CddcIntSampleResFormatResolve(adi_adrv903x_Device_t* const                          device,
                                                                        const adi_adrv903x_CddcIntegerConfigSettings_t* const cddcIntegerConfigSettings,
                                                                        uint8_t* const                                        intDataResolution,
                                                                        uint8_t* const                                        intDataFormat);

/**
* \brief Deallocates GPIOs for internal slicer ctrl observation functionality and disconnects them.
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Structure pointer to the ADRV903X device data structure
* \param[in] rxChannel Receiver channel for which internal slicer ctrl GPIOs are to be disconnected
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxInternalSlicerGpioDisable(adi_adrv903x_Device_t * const   device,
                                                                      const adi_adrv903x_RxChannels_e rxChannel);

/**
* \brief Allocates GPIOs for internal slicer ctrl observation functionality and routes the signals to
*        selected GPIOs. Internal slicer ctrl GPIO outputs can be mapped to any GPIO[23:0].
*    
* It has to be noted that no two channels can share the same internal slicer ctrl GPIOs. Furthermore
* it is possible to observe Rx slicer positions only. There are no ORx slicers.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Structure pointer to the ADRV903X device data structure
* \param[in] rxChannel Receiver channel for which internal slicer ctrl GPIO observation is required
* \param[in] intSlicerFormatSel Internal slicer 3pin / 4 pin mode select
* \param[in] slicerConfigSettings Slicer Config settings that contain Gpio Selections for Internal Slicer Word outputs
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxInternalSlicerGpioEnable(adi_adrv903x_Device_t * const                       device,
                                                                     const adi_adrv903x_RxChannels_e                     rxChannel,
                                                                     const adi_adrv903x_RxDataFormatModes_e              intSlicerFormatSel,
                                                                     const adi_adrv903x_SlicerConfigSettings_t* const    slicerConfigSettings);

/**
* \brief Resolves the embedded slicer position and no. of embedded slicer bits for integer formatter.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in]  device Structure pointer to the ADRV903X device data structure
* \param[in]  formatSelect Requested integer format
* \param[in]  intEmbeddedBits Selected embedded bits mode
* \param[out] intEmbedSlicer Updated with embedded slicer enable(1) or disable(0)
* \param[out] intEmbedSlicerPos Updated with embedded slicer position. MSB = 0, LSB = 1
* \param[out] intEmbedSlicerNumber Updated with number of embedded slicer bits. 2 bits = 0, 1 bit = 1
* \param[out] staticSlicer3bitModeEn Updated with the enable value for a 3bit slicer. Enable = 1, Disable = 0
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxIntEmbSlicerConfigResolve(adi_adrv903x_Device_t * const              device,
                                                                      const adi_adrv903x_RxDataFormatModes_e     formatSelect,
                                                                      const adi_adrv903x_RxSlicerEmbeddedBits_e  intEmbeddedBits,
                                                                      const adi_adrv903x_RxIntParity_e           parity,
                                                                      uint8_t * const                            intEmbedSlicer,
                                                                      uint8_t * const                            intEmbedSlicerPos,
                                                                      uint8_t * const                            intEmbedSlicerNumber,
                                                                      uint8_t * const                            staticSlicer3bitModeEn);

/**
* \brief Resolves the embedded slicer position and no. of embedded slicer bits for integer formatter.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in]  device Structure pointer to the ADRV903X device data structure
* \param[in]  formatSelect Requested integer format
* \param[in]  intEmbeddedBits Selected embedded bits mode
* \param[out] intEmbedSlicer Updated with embedded slicer enable(1) or disable(0)
* \param[out] intEmbedSlicerPos Updated with embedded slicer position. MSB = 0, LSB = 1
* \param[out] intEmbedSlicerNumber Updated with number of embedded slicer bits. 2 bits = 0, 1 bit = 1
* \param[out] staticSlicer3bitModeEn Updated with the enable value for a 3bit slicer. Enable = 1, Disable = 0
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_CddcIntEmbSlicerConfigResolve(adi_adrv903x_Device_t* const              device,
                                                                        const adi_adrv903x_CddcDataFormatModes_e  formatSelect,
                                                                        const adi_adrv903x_RxSlicerEmbeddedBits_e intEmbeddedBits,
                                                                        const adi_adrv903x_RxIntParity_e          parity,
                                                                        uint8_t* const                            intEmbedSlicer,
                                                                        uint8_t* const                            intEmbedSlicerPos,
                                                                        uint8_t* const                            intEmbedSlicerNumber,
                                                                        uint8_t* const                            staticSlicer3bitModeEn);

/**
* \brief Resolves the integer parity support for integer formatter.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in]  device Structure pointer to the ADRV903X device data structure
* \param[in]  integerConfigSettings Structure containing integer formatter settings
* \param[out] intParitySupport Updated with integer parity support. Enabled = 1, Disabled = 0.
* \param[out] intEvenParity Updated with even/odd parity. Even = 0, Odd = 1.
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxIntParitySupportResolve(adi_adrv903x_Device_t * const                       device,
                                                                    const adi_adrv903x_IntegerConfigSettings_t * const  integerConfigSettings,
                                                                    uint8_t * const                                     intParitySupport,
                                                                    uint8_t * const                                     intEvenParity);

/**
* \brief Resolves the integer parity support for integer formatter.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in]  device Structure pointer to the ADRV903X device data structure
* \param[in]  integerConfigSettings Structure containing integer formatter settings
* \param[out] intParitySupport Updated with integer parity support. Enabled = 1, Disabled = 0.
* \param[out] intEvenParity Updated with even/odd parity. Even = 0, Odd = 1.
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_CddcIntParitySupportResolve(adi_adrv903x_Device_t * const                         device,
                                                                    const adi_adrv903x_CddcIntegerConfigSettings_t * const  cddcIntegerConfigSettings,
                                                                    uint8_t * const                                         intParitySupport,
                                                                    uint8_t * const                                         intEvenParity);

/**
* \brief Returns the Rx Data formatter mode currently configured
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in]  device Structure pointer to the ADRV903X device data structure
* \param[in]  rxChannel Receiver channel for which Rx data format select read back is requested
* \param[out] rxDataFormat output representing currently configured Rx data format
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDataFormatSelectGet(adi_adrv903x_Device_t * const            device,
                                                                const adi_adrv903x_RxChannels_e          rxChannel,
                                                                adi_adrv903x_RxDataFormatModes_e * const rxDataFormat);

/**
* \brief Retrieves the Rx floating point formatter settings configured in the device
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in]  device Structure pointer to the ADRV903X device data structure
* \param[in]  rxChannel RX channel for which to retrieve floating point formatter settings
* \param[out] floatingPointConfig Output structure to populate with floating point formatter settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDataFormatFloatingPointGet(adi_adrv903x_Device_t * const                      device,
                                                                       const adi_adrv903x_RxChannels_e                    rxChannel,
                                                                       adi_adrv903x_FloatingPointConfigSettings_t * const floatingPointConfig);

/**
* \brief Returns the Rx Data formatter integer and slicer settings.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in]  device Structure pointer to the ADRV903X device data structure
* \param[in]  rxChannel Channel for which to retrieve integer format and slicer settings
* \param[out] integerConfigSettings Populated with integer config settings
* \param[out] slicerConfigSettings Populated with slicer config settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDataFormatIntegerGet(adi_adrv903x_Device_t * const                device,
                                                                 const adi_adrv903x_RxChannels_e              rxChannel,
                                                                 adi_adrv903x_IntegerConfigSettings_t * const integerConfigSettings,
                                                                 adi_adrv903x_SlicerConfigSettings_t * const  slicerConfigSettings);

/**
* \brief Retrieves the Rx embedded overload indicator format settings configured in the device
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in]  device Structure pointer to the ADRV903X device data structure
* \param[in]  rxChannel RX channel for which to retrieve embedded overload indicator format settings
* \param[out] embOvldConfigSettings Output structure to populate with settings of the embedded overload
*                                    monitor formatter
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDataFormatEmbOvldMonitorGet(adi_adrv903x_Device_t * const                           device,
                                                                        const adi_adrv903x_RxChannels_e                         rxChannel,
                                                                        adi_adrv903x_EmbOverloadMonitorConfigSettings_t * const embOvldConfigSettings);

/**
* \brief Returns gain compensation for external LNA gain enable status
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in]  device Structure pointer to the ADRV903X device data structure
* \param[in]  rxChannel RX channel to retrieve settings for
* \param[out] externalLnaGain Current status of the external LNA gain compensation enable. 0 = Disabled, 1 = Enabled
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxGainCompExtLnaGet(adi_adrv903x_Device_t * const   device,
                                                              const adi_adrv903x_RxChannels_e rxChannel,
                                                              uint8_t * const                 externalLnaGain);

/**
* \brief Returns gain compensation for temperature enable status
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in]  device Structure pointer to the ADRV903X device data structure
* \param[in]  rxChannel RX channel to retrieve settings for
* \param[out] tempCompensationEnable Populated with wither temp compensation is enabled (1) or disabled (0).
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxGainCompTempEnableGet(adi_adrv903x_Device_t * const   device,
                                                                  const adi_adrv903x_RxChannels_e rxChannel,
                                                                  uint8_t * const                 tempCompensationEnable);

/**
* \brief Reads the GPIO source control register to determine if GPIOs are assigned to observe the 
*        internal slicer control word.
*
*
* ORx slicer positions cannot be observed via internal slicer observation GPIOs. There are not ORx slicers.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in]  device Structure pointer to the ADRV903X device data structure
* \param[in]  rxChannel RX channel to retrieve settings for
* \param[out] intSlicerFormatSel Output parameter that will be populated with the current RX data 
                format selection, if enableStatus is 1.
* \param[out] enableStatus Output parameter that will be populated with whether the slicer control 
                is routed to observation GPIOs(1) or not(0).
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxInternalSlicerGpioEnableGet(adi_adrv903x_Device_t * const            device,
                                                                        const adi_adrv903x_RxChannels_e          rxChannel,
                                                                        adi_adrv903x_RxDataFormatModes_e * const intSlicerFormatSel,
                                                                        uint8_t * const                          enableStatus);

/**
* \brief Encodes the embedded slicer position and no. of embedded slicer bits for integer formatter.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in]  device Structure pointer to the ADRV903X device data structure
* \param[in]  rxChannel RX channel which to retrieve settings for
* \param[in]  intEmbedSlicer embedded slicer enable/disable status read back from device
* \param[in]  intEmbedSlicerPos embedded slicer position status read back from device
* \param[in]  intEmbedSlicerNumber embedded slicer bits status read back from device
* \param[out] integerConfigSettingsEmbSlicer Output enumeration that is populated with 
                the value that corresponds to the combination of slicer configuration parameters
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxIntEmbSlicerConfigEncode(adi_adrv903x_Device_t * const               device,
                                                                     const adi_adrv903x_RxChannels_e             rxChannel,
                                                                     const uint8_t                               intEmbedSlicer,
                                                                     const uint8_t                               intEmbedSlicerPos,
                                                                     const uint8_t                               intEmbedSlicerNumber,
                                                                     adi_adrv903x_RxSlicerEmbeddedBits_e * const integerConfigSettingsEmbSlicer);

/**
* \brief Encodes the embedded slicer position and no. of embedded slicer bits for integer formatter for the CDDC.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in]  device Structure pointer to the ADRV903X device data structure
* \param[in]  rxChannel RX channel which to retrieve settings for
* \param[in]  intEmbedSlicer embedded slicer enable/disable status read back from device
* \param[in]  intEmbedSlicerPos embedded slicer position status read back from device
* \param[in]  intEmbedSlicerNumber embedded slicer bits status read back from device
* \param[out] integerConfigSettingsEmbSlicer Output enumeration that is populated with
                the value that corresponds to the combination of slicer configuration parameters
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_CddcIntEmbSlicerConfigEncode(adi_adrv903x_Device_t* const               device,
                                                                       const adi_adrv903x_RxChannels_e             rxChannel,
                                                                       const uint8_t                               intEmbedSlicer,
                                                                       const uint8_t                               intEmbedSlicerPos,
                                                                       const uint8_t                               intEmbedSlicerNumber,
                                                                       adi_adrv903x_RxSlicerEmbeddedBits_e* const integerConfigSettingsEmbSlicer);

/**
* \brief Encodes the integer data resolution and format for integer formatter.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in]  device Structure pointer to the ADRV903X device data structure
* \param[in]  intDataResolution Integer resolution of Rx data formatter. 
                12 bits = 0, 16 bits = 1.
* \param[in]  intDataFormat Integer data format of Rx Data Formatter. 
                2's complement = 0, signed-magnitude = 1.
* \param[out] intSampleResolution Output parameter populated with the value that 
                corresponds to the integer format represented by the input parameters.
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxIntSampleResFormatEncode(adi_adrv903x_Device_t * const                device,
                                                                     const uint8_t                                intDataResolution,
                                                                     const uint8_t                                intDataFormat,
                                                                     adi_adrv903x_RxIntSampleResolution_e * const intSampleResolution);

/**
* \brief Encodes the integer parity support for integer formatter.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in]  device Structure pointer to the ADRV903X device data structure
* \param[in]  rxChannel Receiver channel for which RX data format is requested
* \param[in]  intParitySupport Integer parity support from the device. Disabled = 0, enabled = 1.
* \param[in]  intEvenParity Integer parity from the device. Even = 0, odd = 1.
* \param[out] integerParity Output parameter populated with the value that corresponds to the parity
                settings represented by the input parameters
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxIntParitySupportEncode(adi_adrv903x_Device_t * const      device,
                                                                   const adi_adrv903x_RxChannels_e    rxChannel,
                                                                   const uint8_t                      intParitySupport,
                                                                   const uint8_t                      intEvenParity,
                                                                   adi_adrv903x_RxIntParity_e * const integerParity);

/**
* \brief Encodes the integer parity support for integer formatter for the CDDC.
*
* \dep_begin
* \dep{ device->common.devHalInfo }
* \dep{ device->devStateInfo }
* \dep_end
*
* \param[in]  device Structure pointer to the ADRV903X device data structure
* \param[in]  rxChannel Receiver channel for which RX data format is requested
* \param[in]  intParitySupport Integer parity support from the device.Disabled = 0, enabled = 1.
* \param[in]  intEvenParity Integer parity from the device.Even = 0, odd = 1.
* \param[out] integerParity Output parameter populated with the value that corresponds to the parity 
*               settings represented by the input parameters
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_CddcIntParitySupportEncode(adi_adrv903x_Device_t* const      device,
                                                                     const adi_adrv903x_RxChannels_e    rxChannel,
                                                                     const uint8_t                      intParitySupport,
                                                                     const uint8_t                      intEvenParity,
                                                                     adi_adrv903x_RxIntParity_e* const integerParity);

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
* \param[in] device Pointer to the ADRV903X device data structure
* \param[in] rxChannel Enum to select Rx Channel.
* \param[out] rxLoSource Pointer to store Rx channel LO source mapping read back (Output)
*
* \retval adi_adrv903x_ErrAction_e - ADI_COMMON_ACT_NO_ACTION if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxLoSourceGet(adi_adrv903x_Device_t* const device,
                                                        const adi_adrv903x_RxChannels_e rxChannel,
                                                        adi_adrv903x_LoSel_e* const rxLoSource);

/**
* \brief Performs range check on the Rx channel min and max gain indices to be updated 
*
* This function is called automatically as part of adi_adrv903x_RxMinMaxGainIndexSet()
* if the ADI_ADRV903X_RX_RANGE_CHECK is set to 1. This function can be called independently
* to perform range checks only on parameters for adi_adrv903x_RxMinMaxGainIndexSet() API
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Context variable - Structure pointer to the ADRV903X device data structure
* \param[in] rxChannelMask channel masks for which min and max gain indices are to be updated
* \param[in] minGainIndex is the lower limit of the gain index for a given channel
* \param[in] maxGainIndex is the upper limit of the gain index for a given channel
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxMinMaxGainIndexSetRangeCheck(adi_adrv903x_Device_t* const    device,
                                                                         const uint32_t                  rxChannelMask,
                                                                         const uint8_t                   minGainIndex,
                                                                         const uint8_t                   maxGainIndex);
                                                                                
/**
* \brief Performs range check on the gain table settings for Rx channels.
*
* This function called automatically as part of adi_adrv903x_RxGainTableWrite() API
* if ADI_ADRV903X_RX_RANGE_CHECK is set to 1. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in] device Context variable - Structure pointer to the ADRV903X device data structure
* \param[in] rxChannelMask is the set of channels for which current gain table settings are to be programmed.
*        Valid Rx channels include Rx0-Rx7
* \param[in] gainIndexOffset is the starting gain index from which gain table is programmed
* \param[in] gainTableRow Array of gain table row entries for programming
* \param[in] arraySize is the number of gain table rows to be programmed
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxGainTableWriteRangeCheck(adi_adrv903x_Device_t* const        device,
                                                                     const uint32_t                      rxChannelMask,
                                                                     const uint8_t                       gainIndexOffset,
                                                                     const adi_adrv903x_RxGainTableRow_t gainTableRow[],
                                                                     const uint32_t                      arraySize);
/**
* \brief Performs range check on the gain table read request for Rx channels.
*
* This function called automatically as part of adi_adrv903x_RxGainTableRead() API
* if ADI_ADRV903X_RX_RANGE_CHECK is set to 1.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in,out]  device Context variable - Structure pointer to the ADRV903X device data structure
* \param[in]  rxChannel represents the channels for which gain table read back is requested.
*             Valid Rx channels include Rx0-Rx7
* \param[in]  gainIndexOffset is the gain index from which gain table read back should start
* \param[out] gainTableRow Read back array for gain table row entries which will be updated with the read back values
* \param[in]  arraySize is the size of gainTableRow array. It is also the upper limit for the no. of gain indices to read
* \param[out] numGainIndicesRead is the actual no. of gain indices read from SRAM (output). A NULL can be passed
*             if the value of no. of gain indices actually read is not required.

* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxGainTableReadRangeCheck(adi_adrv903x_Device_t* const    device,
                                                                    const adi_adrv903x_RxChannels_e rxChannel,
                                                                    const uint8_t                   gainIndexOffset,
                                                                    const adi_adrv903x_RxGainTableRow_t gainTableRow[],
                                                                    const uint32_t                      arraySize);

/**
* \brief This function configures Hb2 peak detector block for selected Rx channel
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] rxChannel Enum to select Rx Channel.
* \param[in] enableHb2Overload 1:Enable Hb2 block, 0:Disable Hb2 block
* \param[in] hb2OverloadSignalSelection 2: Select input of DEC3 Block 1:Select input of Hb2 block, 0:Select output of Hb2 block
* \param[in] hb2OverloadPowerMode 1:Use I^2+Q^2 signal configuration, 0:Use max(|I|, |Q|) signal configuration
*
* \retval adi_adrv903x_ErrAction_e - ADI_COMMON_ACT_NO_ACTION if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxHb2OverloadCfgSet(adi_adrv903x_Device_t* const device,
                                                              const adi_adrv903x_RxChannels_e rxChannel,
                                                              const uint8_t enableHb2Overload,
                                                              const uint8_t hb2OverloadSignalSelection,
                                                              const uint8_t hb2OverloadPowerMode);

/**
* \brief This function reads back the Hb2 block configuration of selected Hb2 block
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] rxChannel Enum to select Rx Channel.
* \param[out] enableHb2Overload Pointer to readback value. enableHb2Overload 1:Enable Hb2 block, 0:Disable Hb2 block
* \param[out] hb2OverloadSignalSelection Pointer to readback value 1:Select input of Hb2 block, 0:Select output of Hb2 block
* \param[out] hb2OverloadPowerMode Pointer to readback value 1:Use I^2+Q^2 signal configuration, 0:Use max(|I|, |Q|) signal configuration
*
* \retval adi_adrv903x_ErrAction_e - ADI_COMMON_ACT_NO_ACTION if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxHb2OverloadCfgGet(adi_adrv903x_Device_t* const device,
                                                              const adi_adrv903x_RxChannels_e rxChannel,
                                                              uint8_t * const enableHb2Overload,
                                                              uint8_t * const hb2OverloadSignalSelection,
                                                              uint8_t * const hb2OverloadPowerMode);

/**
 * \brief Convert from ORx integer attenuation (dB) to TrmAtten and trmAttenBwCapEn bitfield values
 * required by the device to give effect to the requested ORx attenuation.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep{device->devStateInfo}
 * \dep_end
 *
 * \param[in,out]  device Context variable - Structure pointer to the ADRV903X device data structure
 * \param[in]  attenDb ORx attenuation. Valid range 0-16 dB.
 * \param[out] trmAtten. The value required for the trm_atten bitfield is written here.
 * \param[out] trmAtten. The value required for the trm_atten_bw_cap_en bitfield is written here.
 * 
 * \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv903x_ErrAction_e adrv903x_ORxAttenDbToRegValues(adi_adrv903x_Device_t* const device,
                                                                const uint8_t attenDb,
                                                                uint8_t* const trmAtten,
                                                                uint8_t* const trmAttenBwCapEn);

/**
 * \brief Convert from the value supplied by the TrmAtten bitfield read function
 * to the closest corresponding attenuation (dB) which it indicates.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep{device->devStateInfo}
 * \dep_end
 *
 * \param[in,out]  device Context variable - Structure pointer to the ADRV903X device data structure
 * \param[in]  attenDb ORx attenuation. Valid range 0-16 dB.
 * \param[out] trmAtten. The converted adrv903x_Bf_ActrlOrxWestRegmap_TrmAtten_e is written here.
 * 
 * \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv903x_ErrAction_e adrv903x_ORxTrmAttenToDb(adi_adrv903x_Device_t* const device,
                                                          const uint8_t trmAtten,
                                                          uint8_t* const attenDb);

/**
* \brief Returns 0-indexed channel id for a given adi_adrv903x_RxChannels_e.
*
* \param[in] rxChannel The value to convert.
*
* \retval uint8_t The channel id. Returns a value greater ADI_ADRV903X_CHAN_ID_MAX if the value supplied cannot be
*     converted (i.e. is not valid value for the enum or refers to RX_ALL or RX_NONE).
*/
ADI_API uint8_t adrv903x_RxChannelsToId(const adi_adrv903x_RxChannels_e rxChannel);

#endif //CLIENT_IGNORE
#endif
