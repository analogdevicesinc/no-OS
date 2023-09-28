/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adrv9025_rx.h
* \brief Contains ADRV9025 Rx related private function prototypes for
*        adrv9025_rx.c which helsp adi_adrv9025_rx.c
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADRV9025_RX_H_
#define _ADRV9025_RX_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "adi_adrv9025_rx.h"
#include "../devices/adrv9025/private/include/adrv9025_bf_rx.h"
#include "../devices/adrv9025/private/include/adrv9025_bf_orx.h"
#include "../devices/adrv9025/private/include/adrv9025_bf_analog_rx_mem_map.h"
#include "../devices/adrv9025/private/include/adrv9025_bf_analog_orx_mem_map.h"
#include "../devices/adrv9025/private/include/adrv9025_bf_orx.h"
#include "../devices/adrv9025/private/include/adrv9025_reg_addr_macros.h"

/**
* \brief Formats gain table structure input into a format expected by ARM DMA
*
* Format Gain Table Data as follows :
* No. of Bytes per Gain Table Entry = 8
* Addr 8 x gainIndex     : {13'b0 , rx_fe_gain[7:0], dig_gain[10:0]}
* Addr 8 x gainIndex + 4 : {12'b0, tia_control, adc_control, ext_control[1:0], phase_offset[15:0]}
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param gainTablePtr Input gain table
* \param formattedGainTablePtr memory to store formatted gain table data
* \param numGainIndicesInTable number of gain table entries
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxGainTableFormat(adi_adrv9025_Device_t*         device,
                                   adi_adrv9025_RxGainTableRow_t* gainTablePtr,
                                   uint8_t*                       formattedGainTablePtr,
                                   uint16_t                       numGainIndicesInTable);

/**
* \brief Parses gain table in ARM DMA data format into a gain table row structure
*
* Format Gain Table Data as follows :
* No. of Bytes per Gain Table Entry = 8
* armDmaDataGainIndex[8 x gainIndex]     : {13'b0 , rx_fe_gain[7:0], dig_gain[10:0]}
* armDmaDataGainIndex[8 x gainIndex + 4] : {12'b0, tia_control, adc_control, ext_control[1:0], phase_offset[15:0]}
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param gainTablePtr pointer to gain table row entry structure where parsed data is stored
* \param armDmaDataGainTablePtr memory containing data from ARM DMA
* \param numGainIndicesInTable number of gain table entries
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxGainTableParse(adi_adrv9025_Device_t*         device,
                                  adi_adrv9025_RxGainTableRow_t* gainTablePtr,
                                  uint8_t*                       armDmaDataGainTablePtr,
                                  uint16_t                       numGainIndicesInTable);

/**
* \brief Calculates parameters required to read Rx Gain Table from ADRV9025 SRAM
*
*  This function calculates the number of gain indices to read and the SRAM base
*  address for the read operation given the maximum read size and gain index offset
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannel is the channel for which gain table read is requested
* \param maxReadGainIndices is the maximum no. of gain indices allowed to be read
* \param gainIndexOffset is the offsetted index from maximum gain index which is
*        the starting gain index to read from
* \param numGainIndicesToRead output from this function which provides the no. of gain table
*        row entry reads to be performed
* \param baseAddr is the starting SRAM address for read operation (output)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxGainTableReadParamsCompute(adi_adrv9025_Device_t*    device,
                                              adi_adrv9025_RxChannels_e rxChannel,
                                              uint16_t                  maxReadGainIndices,
                                              uint8_t                   gainIndexOffset,
                                              uint16_t*                 numGainIndicesToRead,
                                              uint32_t*                 baseAddr);

/**
* \brief Returns the Rx Bitfield function channel base addresses for a given Rx Channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannel is the channel for which Rx bit field base address is requested
* \param rxChannelBitfieldAddr pointer to the Rx baseaddresses reqd by the bitfield.
*        This function updates the memory pointed to by the pointer which can be used
*        by the calling function.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxBitfieldAddressGet(adi_adrv9025_Device_t*    device,
                                      adi_adrv9025_RxChannels_e rxChannel,
                                      adrv9025_BfRxChanAddr_e*  rxChannelBitfieldAddr);

/**
* \brief Returns the ORx Bitfield function channel base addresses for a given Rx Channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param orxChannel is the channel for which ORx bit field base address is requested
* \param orxChannelBitfieldAddr pointer to the ORx baseaddresses reqd by the bitfield.
*        This function updates the pointer memory passed by the calling function
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_OrxBitfieldAddressGet(adi_adrv9025_Device_t*    device,
                                       adi_adrv9025_RxChannels_e orxChannel,
                                       adrv9025_BfOrxChanAddr_e* orxChannelBitfieldAddr);

/**
* \brief Returns the analog Rx Bitfield function channel base addresses for a given Rx Channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannel is the channel for which analog Rx bit field base address is requested
* \param rxChannelAnalogBitfieldAddr pointer to the Rx analog baseaddresses reqd by the bitfield(output)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxAnalogBitfieldAddressGet(adi_adrv9025_Device_t*               device,
                                            adi_adrv9025_RxChannels_e            rxChannel,
                                            adrv9025_BfAnalogRxMemMapChanAddr_e* rxChannelAnalogBitfieldAddr);

/**
* \brief Returns the analog ORx Bitfield function channel base addresses for a req ORx Channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param orxChannel is the channel for which analog Orx bit field base address is requested
* \param orxChannelAnalogBitfieldAddr pointer to the ORx analog baseaddresses reqd by the bitfield(output)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_OrxAnalogBitfieldAddressGet(adi_adrv9025_Device_t*                device,
                                             adi_adrv9025_RxChannels_e             orxChannel,
                                             adrv9025_BfAnalogOrxMemMapChanAddr_e* orxChannelAnalogBitfieldAddr);

/**
* \brief This function unassigns gain ctrl pin GPIOs and reports error if
*        the recovery action is an error from a previous function call
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param sharedResourceArr is the array containing currently assigned gain ctrl GPIOs
* \param recoveryAction is the recovery action from a previous function call
* \param customError is the error to be reported in case of an error recovery action
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxGainCtrlPinReportError(adi_adrv9025_Device_t* device,
                                          int32_t                sharedResourceArr[],
                                          int32_t                recoveryAction,
                                          const char*            customError);

/**
* \brief Returns the Rx SPI channel base addresses for a given Rx Channel
*
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param channelMask is the channel for which Rx base address is requested
*
* \retval uint16_t Return Base Address for Rx channel
*/
uint16_t adrv9025_RxAddrDecode(adi_adrv9025_Device_t* device,
                               uint32_t               channelMask);

/**
* \brief Configures one or more RX FIR filters in the device
*
* The device stores 1 RX FIR filter per channel.
* The device stores an independent filter for each ORx front end (ORx1, ORx2, ORx3, ORx4).
* Internal Loopback 1 and 2 share a FIR filter, and Internal loopback 3 and 4 share a FIR filter.
* Rx and ORx channel can have 24, 48, or 72 taps.
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
int32_t adrv9025_RxFirWrite(adi_adrv9025_Device_t* device,
                            uint32_t               rxChanMask,
                            int8_t                 gain_dB,
                            uint8_t                numFirCoefs,
                            int16_t                coefs[],
                            uint8_t                arraySize);

/**
* \brief This function unassigns ext ctrl word GPIOs and reports error if the
*        recovery action is an error from a previous function call
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param sharedResourceArr is the array containing currently assigned ext ctrl GPIOs
* \param numSharedResources is the number of array
* \param recoveryAction is the recovery action from a previous function call
* \param customError is the error to be reported in case of an error recovery action
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxExtCtrlPinReportError(adi_adrv9025_Device_t* device,
                                         int32_t                sharedResourceArr[],
                                         uint8_t                numSharedResources,
                                         int32_t                recoveryAction,
                                         const char*            customError);

/**
* \brief This function unassigns dualband ctrl word GPIOs and reports error if the
*        recovery action is an error from a previous function call
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param sharedResourceArr is the array containing currently assigned dualband ctrl GPIOs
* \param numSharedResources is the number of array
* \param recoveryAction is the recovery action from a previous function call
* \param customError is the error to be reported in case of an error recovery action
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxDualbandLnaCtrlPinReportError(adi_adrv9025_Device_t* device,
                                                 int32_t                sharedResourceArr[],
                                                 uint8_t                numSharedResources,
                                                 int32_t                recoveryAction,
                                                 const char*            customError);

/**
* \brief Performs range check on Rx floating point formatter configuration settings
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param floatingPointConfig Structure pointer to floating point formatter settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxDataFormatSetFloatingPointRangeCheck(adi_adrv9025_Device_t*                      device,
                                                        adi_adrv9025_FloatingPointConfigSettings_t* floatingPointConfig);

/**
* \brief Performs range check on Rx integer and slicer format settings
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannelMask channels which need to be configured with floating point rx
* \param formatSelect select from adi_adrv9025_RxDataFormatModes_e
* \param integerConfigSettings Structure pointer to integer formatter config settings
* \param slicerConfigSettings Structure pointer to slicer config settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxDataFormatSetIntegerRangeCheck(adi_adrv9025_Device_t*                device,
                                                  uint32_t                              rxChannelMask,
                                                  adi_adrv9025_RxDataFormatModes_e      formatSelect,
                                                  adi_adrv9025_IntegerConfigSettings_t* integerConfigSettings,
                                                  adi_adrv9025_SlicerConfigSettings_t*  slicerConfigSettings);

/**
* \brief Performs range check on Rx embedded overload monitor formatter configuration settings
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param embOvldConfigSettings Structure pointer to embedded overload monitor formatter settings
* \param intSampleResolution Integer sample resolution of the JESD Rx data samples
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxDataFormatSetEmbOvldMonitorRangeCheck(adi_adrv9025_Device_t*                           device,
                                                         adi_adrv9025_EmbOverloadMonitorConfigSettings_t* embOvldConfigSettings,
                                                         adi_adrv9025_RxIntSampleResolution_e             intSampleResolution);

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
* \param device Structure pointer to the ADRV9025 device data structure
* \param floatingPointConfig Structure pointer to floating point formatter settings
* \param rxChannelMask channels which need to be configured with floating point rx
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxDataFormatFloatingPointSet(adi_adrv9025_Device_t*                      device,
                                              uint32_t                                    rxChannelMask,
                                              adi_adrv9025_FloatingPointConfigSettings_t* floatingPointConfig);

/**
* \brief Returns the Rx Data format floating point format settings.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannel is the channel for which Rx Data Format is requested
* \param floatingPointConfig is the pointer to floating point config which
*        is updated with the floating point config readback value
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxDataFormatFloatingPointGet(adi_adrv9025_Device_t*                      device,
                                              adi_adrv9025_RxChannels_e                   rxChannel,
                                              adi_adrv9025_FloatingPointConfigSettings_t* floatingPointConfig);

/**
* \brief Sets the Rx Data format to integer and configures slicer settings.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannelMask channels which need to be configured with integer settings requested
* \param formatSelect Integer formatter mode select
* \param integerConfigSettings contains the integer formatter settings for Rx channels requested
* \param slicerConfigSettings contains the integer formatter settings for Rx channels requested
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxDataFormatIntegerSet(adi_adrv9025_Device_t*                device,
                                        uint32_t                              rxChannelMask,
                                        adi_adrv9025_RxDataFormatModes_e      formatSelect,
                                        adi_adrv9025_IntegerConfigSettings_t* integerConfigSettings,
                                        adi_adrv9025_SlicerConfigSettings_t*  slicerConfigSettings);

/**
* \brief Returns the Rx Data formatter integer and slicer settings.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannel channels which need to be configured with floating point rx
* \param integerConfigSettings Pointer to integer config settings updated with readback value
* \param slicerConfigSettings Pointer to slicer config settings updated with readback value
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxDataFormatIntegerGet(adi_adrv9025_Device_t*                device,
                                        adi_adrv9025_RxChannels_e             rxChannel,
                                        adi_adrv9025_IntegerConfigSettings_t* integerConfigSettings,
                                        adi_adrv9025_SlicerConfigSettings_t*  slicerConfigSettings);

/**
* \brief Sets the Rx Data format to embedded overload indicator format and configures the embedded overload monitor settings.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannelMask Rx channels for which embedded overload indicator format has to be configured
* \param embOvldConfigSettings Structure pointer to embedded overload monitor formatter settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxDataFormatEmbOvldMonitorSet(adi_adrv9025_Device_t*                           device,
                                               uint32_t                                         rxChannelMask,
                                               adi_adrv9025_EmbOverloadMonitorConfigSettings_t* embOvldConfigSettings);

/**
* \brief Retrieves the Rx embedded overload indicator format settings configured in the device
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannel Rx channel for which embedded overload indicator format has to be retrieved
* \param embOvldConfigSettings Structure pointer to embedded overload monitor formatter settings
*                              which will be updated with the settings retrieved from the device (output)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxDataFormatEmbOvldMonitorGet(adi_adrv9025_Device_t*                           device,
                                               adi_adrv9025_RxChannels_e                        rxChannel,
                                               adi_adrv9025_EmbOverloadMonitorConfigSettings_t* embOvldConfigSettings);

/**
* \brief Sets gain compensation for external LNA gain enable
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannelMask channels which need to be configured with floating point rx
* \param externalLnaGain enable(1) or disable(0) gain compensation to consider LNA gain
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxGainCompExtLnaSet(adi_adrv9025_Device_t* device,
                                     uint32_t               rxChannelMask,
                                     uint8_t                externalLnaGain);

/**
* \brief Returns gain compensation for external LNA gain enable status
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannel channels which need to be configured with floating point rx
* \param externalLnaGain returns the current status of external LNA gain comp enable
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxGainCompExtLnaGet(adi_adrv9025_Device_t*    device,
                                     adi_adrv9025_RxChannels_e rxChannel,
                                     uint8_t*                  externalLnaGain);

/**
* \brief Sets gain compensation for temperature enable
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannelMask channels which need to be configured with floating point rx
* \param tempCompensationEnable enable(1) or disable(0) gain compensation to consider temperature
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxGainCompTempEnableSet(adi_adrv9025_Device_t* device,
                                         uint32_t               rxChannelMask,
                                         uint8_t                tempCompensationEnable);

/**
* \brief Returns gain compensation for temperature enable status
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannel channels which need to be configured with floating point rx
* \param tempCompensationEnable returns the current status of temperature gain comp enable
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxGainCompTempEnableGet(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_RxChannels_e rxChannel,
                                         uint8_t*                  tempCompensationEnable);

/**
* \brief Returns the Rx Data formatter mode currently configured on ADRV9025 device
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannel Receiver channel for which Rx data format select read back is requested
* \param rxDataFormat output representing currently configured Rx data format
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxDataFormatSelectGet(adi_adrv9025_Device_t*            device,
                                       adi_adrv9025_RxChannels_e         rxChannel,
                                       adi_adrv9025_RxDataFormatModes_e* rxDataFormat);

/**
* \brief Resolves the embedded slicer position and no. of embedded slicer bits for integer formatter.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param formatSelect Input parameter conveying the integer format select requested
* \param intEmbeddedBits Enum conveying the embedded bits mode selected
* \param intEmbedSlicer pointer updated with embedded slicer enable/disable
* \param intEmbedSlicerPos pointer updated with embedded slicer position
* \param intEmbedSlicerNumber pointer updated with no. of embedded slicer bits
* \param staticSlicer3bitModeEn pointer updated with the enable value for a 3 bit slicer
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxIntEmbSlicerConfigResolve(adi_adrv9025_Device_t*              device,
                                             adi_adrv9025_RxDataFormatModes_e    formatSelect,
                                             adi_adrv9025_RxSlicerEmbeddedBits_e intEmbeddedBits,
                                             uint8_t*                            intEmbedSlicer,
                                             uint8_t*                            intEmbedSlicerPos,
                                             uint8_t*                            intEmbedSlicerNumber,
                                             uint8_t*                            staticSlicer3bitModeEn);

/**
* \brief Resolves the embedded slicer position and no. of embedded slicer bits for integer formatter.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannel Receiver channel for which Rx data format select read back is requested
* \param intEmbedSlicer embedded slicer enable/disable status read back from ADRV9025
* \param intEmbedSlicerPos embedded slicer position status read back from ADRV9025
* \param intEmbedSlicerNumber embedded slicer bits status read back from ADRV9025
* \param integerConfigSettingsEmbSlicer enum pointer to integer formatter settings updated with
*        encoded value for embedded slicer config
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxIntEmbSlicerConfigEncode(adi_adrv9025_Device_t*               device,
                                            adi_adrv9025_RxChannels_e            rxChannel,
                                            uint8_t                              intEmbedSlicer,
                                            uint8_t                              intEmbedSlicerPos,
                                            uint8_t                              intEmbedSlicerNumber,
                                            adi_adrv9025_RxSlicerEmbeddedBits_e* integerConfigSettingsEmbSlicer);

/**
* \brief Resolves the integer data resolution and format for integer formatter.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param integerConfigSettings Structure pointer to integer formatter settings
* \param intDataResolution pointer updated with integer resolution
* \param intDataFormat pointer updated integer data format (2's complement or signed magnitude)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxIntSampleResFormatResolve(adi_adrv9025_Device_t*                      device,
                                             const adi_adrv9025_IntegerConfigSettings_t* integerConfigSettings,
                                             uint8_t*                                    intDataResolution,
                                             uint8_t*                                    intDataFormat);

/**
* \brief Encodes the integer data resolution and format for integer formatter.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param intDataResolution integer resolution of Rx data formatter
* \param intDataFormat integer data format (2's complement or signed magnitude) of Rx Data formatter
* \param intSampleResolution Pointer to integer sample resolution updated with sample resolution
*        resolved from intDataResolution and intDataFormat parameters
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxIntSampleResFormatEncode(adi_adrv9025_Device_t*                device,
                                            uint8_t                               intDataResolution,
                                            uint8_t                               intDataFormat,
                                            adi_adrv9025_RxIntSampleResolution_e* intSampleResolution);

/**
* \brief Resolves the integer parity support for integer formatter.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param integerConfigSettings Structure pointer to integer formatter settings
* \param intParitySupport pointer updated with integer parity support (enable/disable)
* \param intEvenParity pointer updated integer even/odd parity
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxIntParitySupportResolve(adi_adrv9025_Device_t*                      device,
                                           const adi_adrv9025_IntegerConfigSettings_t* integerConfigSettings,
                                           uint8_t*                                    intParitySupport,
                                           uint8_t*                                    intEvenParity);

/**
* \brief Encodes the integer parity support for integer formatter.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannel Receiver channel for which Rx data format select read back is requested
* \param intParitySupport integer parity support (enable/disable)
* \param intEvenParity integer even/odd parity
* \param integerParity Enum pointer to integer formatter parity settings updated
*        with value encoded from intParitySupport & intEvenParity parameters
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxIntParitySupportEncode(adi_adrv9025_Device_t*      device,
                                          adi_adrv9025_RxChannels_e   rxChannel,
                                          uint8_t                     intParitySupport,
                                          uint8_t                     intEvenParity,
                                          adi_adrv9025_RxIntParity_e* integerParity);

/**
* \brief Allocates GPIOs for internal slicer ctrl observatio functionality and sets their direction to output.
*        Internal slicer ctrl GPIO outputs are mapped to the following GPIOs
*
* === Internal Slicer 4 Pin mode ====
*              rxChannel | ADRV9025 GPIOs set to internal slicer ctrl
*   ---------------------|-------------------------------------------------------------------------
*    ADI_ADRV9025_RX1    |  ADRV9025_GPIO_11, ADRV9025_GPIO_10, ADRV9025_GPIO_09, ADRV9025_GPIO_08
*    ADI_ADRV9025_RX2    |  ADRV9025_GPIO_15, ADRV9025_GPIO_14, ADRV9025_GPIO_13, ADRV9025_GPIO_12
*    ADI_ADRV9025_RX3    |  ADRV9025_GPIO_07, ADRV9025_GPIO_06, ADRV9025_GPIO_05, ADRV9025_GPIO_04
*    ADI_ADRV9025_RX4    |  ADRV9025_GPIO_03, ADRV9025_GPIO_02, ADRV9025_GPIO_01, ADRV9025_GPIO_00
*
* === Internal Slicer 3 Pin mode ====
*              rxChannel | ADRV9025 GPIOs set to internal slicer ctrl
*   ---------------------|-------------------------------------------------------------------------
*    ADI_ADRV9025_RX1    |  ADRV9025_GPIO_06, ADRV9025_GPIO_07, ADRV9025_GPIO_08
*    ADI_ADRV9025_RX2    |  ADRV9025_GPIO_09, ADRV9025_GPIO_10, ADRV9025_GPIO_11
*    ADI_ADRV9025_RX3    |  ADRV9025_GPIO_12, ADRV9025_GPIO_13, ADRV9025_GPIO_14
*    ADI_ADRV9025_RX4    |  ADRV9025_GPIO_15, ADRV9025_GPIO_16, ADRV9025_GPIO_17
*
* It has to be noted that no two channels can share the same internal slicer ctrl GPIOs. Furthermore
* it is possible to observe Rx slicer positions only. ORx slicer positions cannot be observed via
* internal slicer observation GPIOs
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param intSlicerFormatSel Internal slicer 3pin / 4 pin mode select
* \param rxChannel Receiver channel for which internal slicer ctrl GPIO observation is required
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxInternalSlicerGpioEnable(adi_adrv9025_Device_t*           device,
                                            adi_adrv9025_RxDataFormatModes_e intSlicerFormatSel,
                                            adi_adrv9025_RxChannels_e        rxChannel);

/**
* \brief Reads the GPIO source control register to determine if GPIOs are assigned to internal slicer
*        control word observation
*
* === For 4 Pin mode ====
*              rxChannel | ADRV9025 GPIOs set to internal slicer ctrl
*   ---------------------|-------------------------------------------------------------------------
*    ADI_ADRV9025_RX1    |  gpio_upper_byte_lower_nibble_source_control  = 0x0A
*    ADI_ADRV9025_RX2    |  gpio_upper_byte_upper_nibble_source_control  = 0x0A
*    ADI_ADRV9025_RX3    |  gpio_lower_byte_upper_nibble_source_control  = 0x0A
*    ADI_ADRV9025_RX4    |  gpio_lower_byte_lower_nibble_source_control  = 0x0A
*
* === For 3 Pin mode ====
*              rxChannel | ADRV9025 GPIOs set to internal slicer ctrl
*   ---------------------|-------------------------------------------------------------------------
*    ADI_ADRV9025_RX1    |  gpio_upper_byte_lower_nibble_source_control, gpio_lower_byte_upper_nibble_source_control  = 0x0B
*    ADI_ADRV9025_RX2    |  gpio_upper_byte_lower_nibble_source_control  = 0x0B
*    ADI_ADRV9025_RX3    |  gpio_upper_byte_upper_nibble_source_control  = 0x0B
*    ADI_ADRV9025_RX4    |  gpio_upper_byte_upper_nibble_source_control, gpio_extra_bits_source_control  = 0x0B
*
* ORx slicer positions cannot be observed via internal slicer observation GPIOs
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannel Receiver channel for which internal slicer ctrl GPIO observation is required
* \param intSlicerFormatSel Pointer to an Rx data format select enum which will be updated with the active Internal Slicer format in the device.
*                           This param is only valid if enableStatus is set to 1.
* \param enableStatus output flag indicating if the requested channel has it's slicer control
*        word routed to observation GPIOs based on GPIO source ctrl readback
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxInternalSlicerGpioEnableGet(adi_adrv9025_Device_t*            device,
                                               adi_adrv9025_RxChannels_e         rxChannel,
                                               adi_adrv9025_RxDataFormatModes_e* intSlicerFormatSel,
                                               uint8_t*                          enableStatus);

/**
* \brief Sets GPIO source ctrl assigned for internal slicer ctrl observation functionality
*        and sets their direction to input.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannel Receiver channel for which internal slicer ctrl GPIO observation is required to be disabled
* \param intSlicerFormatSel The target internal slicer mode to disable (3pin or 4pin mode)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxInternalSlicerGpioDisable(adi_adrv9025_Device_t*           device,
                                             adi_adrv9025_RxChannels_e        rxChannel,
                                             adi_adrv9025_RxDataFormatModes_e intSlicerFormatSel);

/**
* \brief Allocates GPIOs for external slicer ctrl functionality and sets their direction to input.
*        External ctrl GPIOs can be set to one of the following
*
*   slicerConfigSettings->rx(n)ExtSlicerGpioSelect | ADRV9025 GPIOs set to external slicer ctrl
*   -----------------------------------------------|-------------------------------------------------------
*    ADI_ADRV9025_EXTSLICER_RX_GPIO_2_DOWNTO_0     |  ADRV9025_GPIO_00, ADRV9025_GPIO_01, ADRV9025_GPIO_02
*    ADI_ADRV9025_EXTSLICER_RX_GPIO_5_DOWNTO_3     |  ADRV9025_GPIO_03, ADRV9025_GPIO_04, ADRV9025_GPIO_05
*    ADI_ADRV9025_EXTSLICER_RX_GPIO_8_DOWNTO_6     |  ADRV9025_GPIO_06, ADRV9025_GPIO_07, ADRV9025_GPIO_08
*    ADI_ADRV9025_EXTSLICER_RX_GPIO_11_DOWNTO_9    |  ADRV9025_GPIO_09, ADRV9025_GPIO_10, ADRV9025_GPIO_11
*    ADI_ADRV9025_EXTSLICER_RX_GPIO_14_DOWNTO_12   |  ADRV9025_GPIO_12, ADRV9025_GPIO_13, ADRV9025_GPIO_14
*    ADI_ADRV9025_EXTSLICER_RX_GPIO_17_DOWNTO_15   |  ADRV9025_GPIO_15, ADRV9025_GPIO_16, ADRV9025_GPIO_17
*
* It has to be noted that no two channels can share the same external slicer ctrl GPIOs
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param slicerConfigSettings Structure pointer to slicer configuration settings
* \param rxChannel Receiver channel for which external slicer ctrl is being rebuilt for
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxExternalSlicerGpioEnable(adi_adrv9025_Device_t*                     device,
                                            adi_adrv9025_RxChannels_e                  rxChannel,
                                            const adi_adrv9025_SlicerConfigSettings_t* slicerConfigSettings);

/**
* \brief Clears GPIO select assigned for external slicer ctrl functionality
*        and sets the external slicer ctrl GPIO direction to input.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannel Receiver channel for which internal slicer ctrl GPIO observation is required to be disabled
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxExternalSlicerGpioDisable(adi_adrv9025_Device_t*    device,
                                             adi_adrv9025_RxChannels_e rxChannel);

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
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxChannelMask specifies the channels for which gain compensation is to be disabled
* \param intDataFormat integer data format on disabling gain compensation (2s complement vs signed)
* \param intDataResolution integer data resolution (12/16/24 bit) on disabling gain compensation
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxDataFormatGainCompDisable(adi_adrv9025_Device_t* device,
                                             uint32_t               rxChannelMask,
                                             uint8_t                intDataFormat,
                                             uint8_t                intDataResolution);

/**
* \brief Performs range check on adi_adrv9025_RxDecPowerGet() API.
*
* \pre This function is called automatically as part of adi_adrv9025_RxDecPowerGet()
*      function if ADI_ADRV9025_RX_RANGE_CHECK > 0.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param rxChannel Channel selection to read back gain index for (Valid Rx1-Rx4, ORx1-ORx4 only)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxDecPowerGetRangeCheck(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_RxChannels_e rxChannel);

/**
* \brief This function performs range check on the parameters for adrv9025_RxLoSourceGet API.
*
* This function verifies that the requested Rx channel is valid and that it is initialized
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
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxLoSourceGetRangeCheck(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_RxChannels_e rxChannel,
                                         adi_adrv9025_LoSel_e*     rxLoSource);

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
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxLoSourceGet(adi_adrv9025_Device_t*    device,
                               adi_adrv9025_RxChannels_e rxChannel,
                               adi_adrv9025_LoSel_e*     rxLoSource);

/**
* \brief This function reads back the LO Source Mapping to the requested ORx channel.
*
* The LO source select for a given channel is configured during init time. This function
* can be used to read back the configuration during run time.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param rxChannel Enum to select ORx Channel.
* \param rxLoSource Pointer to store ORx channel LO source mapping read back (Output)
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_OrxLoSourceGet(adi_adrv9025_Device_t*    device,
                                adi_adrv9025_RxChannels_e rxChannel,
                                adi_adrv9025_LoSel_e*     rxLoSource);

/**
* \brief This function sets a dummy value to the agc_gain_index so it can latch.
*
* When agc_enable_gain_index_update is set to one, then agc_gain_index will not
* have valid data unless you write a dummy value to it first so that it latches.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9010 device data structure
* \param rxChannelBitFieldAddr Enum to select Rx Channel.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxAgcEnableGainIndexUpdateLatch(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfRxChanAddr_e rxChannelBitFieldAddr);

/**
* \brief This function sets a dummy value to the agc_gain_index for Orx1 so it can latch.
*
* When agc_enable_gain_index_update is set to one, then agc_gain_index will not
* have valid data unless you write a dummy value to it first so that it latches.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9010 device data structure
* \param rxChannelBitFieldAddr Enum to select Rx Channel.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_OrxOrx1AgcEnableGainIndexUpdateLatch(adi_adrv9025_Device_t*   device,
                                                      adrv9025_BfOrxChanAddr_e orxChannelBitFieldAddr);

/**
* \brief This function sets a dummy value to the agc_gain_index for Orx2 so it can latch.
*
* When agc_enable_gain_index_update is set to one, then agc_gain_index will not
* have valid data unless you write a dummy value to it first so that it latches.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9010 device data structure
* \param rxChannelBitFieldAddr Enum to select Rx Channel.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_OrxOrx2AgcEnableGainIndexUpdateLatch(adi_adrv9025_Device_t*   device,
                                                      adrv9025_BfOrxChanAddr_e orxChannelBitFieldAddr);

#ifdef __cplusplus
}
#endif

#endif
