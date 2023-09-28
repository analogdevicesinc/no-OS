/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adrv9025_data_interface.h
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
 * Copyright 2015 - 2020 Analog Devices Inc.
 * Released under the ADRV9025 API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
* \file adrv9025_data_interface.h
* \brief Contains private ADRV9025 functions to aid the data interface public
*        API functions.
*
* ADRV9025 API Version: 6.4.0.14
*
* Copyright 2015-2019 Analog Devices Inc.
* Released under the AD9378-AD9379 API license, for more information see the "LICENSE.pdf" file.
*/

#ifndef _ADRV9025_DATA_INTERFACE_H_
#define _ADRV9025_DATA_INTERFACE_H_

#include "adi_adrv9025_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief Private function called by API's to get the serializer lane offset at
*        the die due to laminate lane swaps in the device package.
*
*   This function allows passing in a serializer lane number (0-7) at the
*   package ball, and returns the lane number at the die.  It corrects the
*   lane swaps in the package laminate.
*
* \pre This function can be called anytime.
*
* \param laneAtPackageBall Serializer package ball lane number (Valid: 0-7)
*
* \retval Returns the lane number at the die pad after the lamainate swaps have been applied.
*/
uint8_t adrv9025_SerializerLamLaneOffsetGet(uint8_t laneAtPackageBall);

/**
* \brief Private function called by API's to translate pad to ball number
*
* \pre This function can be called anytime.
*
* \param pad - pad number (Valid: 0-7)
*
* \retval Returns the ball number corresponding to the pad number
*/
uint8_t adrv9025_FramerPadToBallTranslate(uint8_t pad);

/**
* \brief Private function called by API's to translate pad mask to ball mask
*
* \pre This function can be called anytime.
*
* \param padMask - pad mask
*
* \retval Returns the ball mask corresponding to the pad mask
*/
uint8_t adrv9025_FramerPadToBallMaskTranslate(uint8_t padMask);

/**
* \brief Private function called by API's to get the deserializer lane offset
*        at the die due to laminate lane swaps in the device package.
*
*   This function receives the deserializer lane number at the package ball and
*   returns the lane number(offset) at the die after the laminate lane swaps
*   are applied.
*
*
* \pre This function can be called anytime.
*
* \param laneAtPackageBall Deserializer package ball lane number (Valid: 0-7)
*
* \retval Returns the lane number at the die pad after the laminate swaps have been applied.
*/
uint8_t adrv9025_DeserializerLamLaneOffsetGet(uint8_t laneAtPackageBall);

/**
* \brief Private function called by API's to get the deserializer lane
*        at the ball due to laminate lane swaps in the device package.
*
*   This function receives the deserializer lane number(offset) at the die and
*   returns the lane number at the package ball after the laminate lane swaps
*   are applied.
*
*
* \pre This function can be called anytime.
*
* \param laneAtDie Deserializer package die pad lane number (Valid: 0-7)
*
* \retval Returns the lane number at the ball lane after the laminate swaps have been applied.
*/
uint8_t adrv9025_DeframerLamLaneOffsetGet(uint8_t laneAtDie);

/**
* \brief Private function called by API's to translate deframer lane enable bits from lane
*  numbers at die to ball lane numbers
*
*   This function receives bitmask of deframer lane enable bits with lane numbers at die and
*   returns converted bitmask at ball lane numbers
*
*
* \pre This function can be called anytime.
*
* \param inBitMask Deframer lane enable bitmask with die lane numbers
*
* \retval Returns the deframer lane enable bitmask with ball lane numbers
*/
uint8_t adrv9025_DeframerLaneTranslateBitmask(uint8_t inBitMask);

/**
* \brief Private function to read from the specified CBUS register
*
*
* \pre This function can be called anytime.
*
* \param device - device structure pointer
* \param addr - CBUS address to read data from
* \param rxLaneSel - lane select mask
* \param data - pointer to a uint8_t variable used to hold data read back from a particular address i.e. addr param
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CbusRegisterRead(adi_adrv9025_Device_t* device,
                                  uint8_t                addr,
                                  uint8_t                rxLaneSel,
                                  uint8_t*               data);

/**
* \brief Private function to write from the specified CBUS register
*
*
* \pre This function can be called anytime.
*
*\param device - device structure pointer
* \param addr CBUS address
* \param rxLaneSel bitmask to select which rx lanes to write
* \param data - value to write to a given address i.e. the addr param
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CbusRegisterWrite(adi_adrv9025_Device_t* device,
                                   uint8_t                addr,
                                   uint8_t                rxLaneSel,
                                   uint8_t                data);

/**
* \brief Private function to read from the specified CBUS register using SPI reads
*
* \pre This function can be called anytime. Need to calculate lane mask (rxLaneSel) prior to calling API
*
* \param device - device structure pointer
* \param addr - CBUS address
* \param rxLaneSel - lane select mask
* \param data - pointer to a uint8_t variable used to hold data read back from a particular address i.e. addr param
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CbusRegisterSpiRead(adi_adrv9025_Device_t *device,
                                     uint8_t               addr,
                                     uint8_t               rxLaneSel,
                                     uint8_t               *data);

/**
* \brief Private function to write from the specified CBUS register using SPI Writes
*
*
* \pre This function can be called anytime. Need to calculate lane mask (rxLaneSel) prior to calling API
*
* \param device CBUS address
* \param addr CBUS address
* \param rxLaneSel lane select mask
* \param data pointer to uint8_t to hold data read back in addr
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CbusRegisterSpiWrite(adi_adrv9025_Device_t *device,
                                     uint8_t                addr,
                                     uint8_t                rxLaneSel,
                                     uint8_t                data);

/**
* \brief Private function to write a SPO value using SPI for the selected lanes. Sets & clears the latch bit.
*
*
* \pre This function can be called anytime.
*
* \param addr CBUS address
* \param rxLaneSel bitmask to select which rx lanes to write
* \param data value to write to addr
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_SpoSpiSet(adi_adrv9025_Device_t* device,
                           uint8_t                rxLaneSel,
                           uint8_t                data);

/**
* \brief Private function to write a SPO value for the selected lanes. Sets & clears the latch bit.
*
*
* \pre This function can be called anytime.
*
* \param addr CBUS address
* \param rxLaneSel bitmask to select which rx lanes to write
* \param data value to write to addr
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_SpoSet(adi_adrv9025_Device_t* device,
                        uint8_t                rxLaneSel,
                        uint8_t                data);

/**
* \brief Used to enable selected deserializer lanes using lane bitmask.
*
* \pre This function may be called anytime.
*
* \dep_begin
* \dep{device->devHalInfo}
* \dep_end
*
* \param device     is a pointer to the device settings structure
* \param laneMask   is an eight bit mask allowing selection of lanes. Bit 0 maps to Lane 0(A), Bit 7 maps to Lane 7(H).
* \param enable     0 = Disable the selected lanes, 1 = enable the selected lanes
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_DeserializerLaneEnable(adi_adrv9025_Device_t* device,
                                        uint8_t                laneMask,
                                        uint8_t                enable);

/**
* \brief This function generates eyeDiagram results for a programmed JESD Quarter Rate Configuration
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param lane - number describing which lane to generate a diagram for (0-3)
* \param results array of adi_adrv9025_EyeDiagramResultsQR_t results structures which contains output from arm command
* \param numResults number of members in adi_adrv9025_EyeDiagramResultsQR_t* results
* \param direction - which part of the eye direction to scan (1 = positive eye, 0 = negative eye)
*
* \retval ADI_ADRV9025_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_GenerateEyeDiagram_Direction(adi_adrv9025_Device_t              *device,
                                              uint8_t                            lane,
                                              adi_adrv9025_EyeDiagramResultsQR_t **results,
                                              uint8_t                            numResults,
                                              uint8_t                            direction);

/**
* \brief This function generates vertical isi results for a given SPO (static phase offset)
*        this function utilizes ARM serdes test mode in order to generate the results
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*

* \param device Pointer to the ADRV9025 data structure
* \param lane - number describing which lane to generate a diagram for (0-3)
* \param data - array of results structures which contains output from arm serdes test mode i.e. vertisi_sum_s0f1  vertisi_sum_s1f1 vertisi_diff_s0f1 and vertisi_diff_s1f1
* \param config - configuration structure to tell arm how to perform the vertical isi test mode task
*
* \retval ADI_ADRV9025_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_VerticalIsiGet(adi_adrv9025_Device_t            *device,
                                uint8_t                          lane,
                                uint8_t                          *data,
                                adi_adrv9025_VerticalISIConfig_t config);
#ifdef __cplusplus
}
#endif

#endif /* _ADRV9025_DATA_INTERFACE_H_ */
