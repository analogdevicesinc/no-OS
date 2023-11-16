/**
 * Copyright 2015 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adrv904x_datainterface.h
 * \brief Contains ADRV904X data interface related private function prototypes for
 *        adrv904x_datainterface.c which helps adi_adrv904x_datainterface.c
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef _ADRV904X_DATAINTERFACE_H_
#define _ADRV904X_DATAINTERFACE_H_

#include "../../private/bf/adrv904x_bf_jtx_link.h"
#include "../../private/bf/adrv904x_bf_jrx_link.h"
#include "../../private/bf/adrv904x_bf_jesd_common.h"
#include "../../private/bf/adrv904x_bf_serdes_txdig_phy_regmap_core1p2.h"
#include "../../private/bf/adrv904x_bf_serdes_rxdig_8pack_regmap_core1p2.h"

#include "adi_adrv904x_error.h"


#define ADI_ADRV904X_CAPTURE_LOC_DDC0_BASE_CONFIG 0x00005008U
#define ADI_ADRV904X_CAPTURE_LOC_DDC1_BASE_CONFIG 0x00004008U
#define ADI_ADRV904X_CAPTURE_LOC_DPD_BASE_CONFIG 0x00000001U
#define ADI_ADRV904X_CAPTURE_LOC_DPD_PRE_BASE_CONFIG 0x00000101U
#define ADI_ADRV904X_CAPTURE_LOC_DPD_POST_BASE_CONFIG 0x00000201U
#define ADI_ADRV904X_CAPTURE_LOC_ORX_TX0_BASE_CONFIG 0x00000011U
#define ADI_ADRV904X_CAPTURE_LOC_ORX_TX1_BASE_CONFIG 0x00010011U
#define ADI_ADRV904X_CAPTURE_LOC_ORX_TX2_BASE_CONFIG 0x00020011U
#define ADI_ADRV904X_CAPTURE_LOC_ORX_TX3_BASE_CONFIG 0x00030011U
#define ADI_ADRV904X_CAPTURE_LOC_ORX_TX4_BASE_CONFIG 0x00040011U
#define ADI_ADRV904X_CAPTURE_LOC_ORX_TX5_BASE_CONFIG 0x00050011U
#define ADI_ADRV904X_CAPTURE_LOC_ORX_TX6_BASE_CONFIG 0x00060011U
#define ADI_ADRV904X_CAPTURE_LOC_ORX_TX7_BASE_CONFIG 0x00070011U
#define ADI_ADRV904X_CAPTURE_LOC_FORMATTER_BASE_CONFIG 0x00000001U
#define ADI_ADRV904X_CAPTURE_LOC_SLICE_MASK 0xFFFF0000U
#define ADI_ADRV904X_CAPTURE_SIZE_MASK_16K 0x0F000000U
#define ADI_ADRV904X_CAPTURE_SIZE_MASK_12K 0x00000000U
#define ADI_ADRV904X_CAPTURE_SIZE_MASK_8K 0x01000000U
#define ADI_ADRV904X_CAPTURE_SIZE_MASK_4K 0x02000000U
#define ADI_ADRV904X_CAPTURE_SIZE_MASK_2K 0x03000000U
#define ADI_ADRV904X_CAPTURE_SIZE_MASK_1K 0x04000000U
#define ADI_ADRV904X_CAPTURE_SIZE_MASK_512 0x05000000U
#define ADI_ADRV904X_CAPTURE_SIZE_MASK_256 0x06000000U
#define ADI_ADRV904X_CAPTURE_SIZE_MASK_128 0x07000000U
#define ADI_ADRV904X_CAPTURE_SIZE_MASK_64 0x08000000U
#define ADI_ADRV904X_CAPTURE_SIZE_MASK_32 0x09000000U

/**
 * \brief Reads the contents of the Rx/ORx data capture RAM into
 *
 * \dep begin
 * \dep{device->common.devHalInfo}
 * \dep end
 *
 * \param[in,out] device Pointer to the device settings structure
 * \param[in] channelSelect Bit mask used to select the channel to read back
 * \param[in] wordOffset Offset (starting from 0) of where to begin reading the RAM memory
 * \param[out] ramData Pointer to the array that stores the captured data
 * \param[in] wordCount The number of words of data read back from the RAM
 *
* \retval adi_common_ErrAction_e - ADI_COMMON_ACT_NO_ACTION if Successful
 */

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxOrxDataCaptureRead(adi_adrv904x_Device_t* const    device,
                                                               const adi_adrv904x_RxChannels_e channelSelect,
                                                               uint32_t                        ramData[],
                                                               const uint32_t                  wordCount,
															   const adi_adrv904x_RxOrxDataCaptureLocation_e loc);

/**
 * \brief Helper function to decode the capture location
 *
 * Determines the value that should be written to the capture
 * configuration register based on the location passed in
 *
 * \dep begin
 * \dep{device->common.devHalInfo}
 * \dep end
 *
 * \param[in] device Pointer to the device settings structure
 * \param[in] captureLocation The location at which the data capture will occur
 * \param[in] size The length of data capture
 * \param[out] config 32-bit value representing the capture config register state for the selected location
* \retval adi_common_ErrAction_e - ADI_COMMON_ACT_NO_ACTION if Successful
 */

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxOrxDataCaptureConfigSet( adi_adrv904x_Device_t* const                  device,
                                                                     const adi_adrv904x_RxOrxDataCaptureLocation_e captureLocation,
                                                                     const uint32_t                                size,
                                                                     uint32_t*  const                              config);

/**
 * \brief Helper function to decode the channel number to select the base address for the memory region to read
 *
 * \dep begin
 * \dep{device->common.devHalInfo}
 * \dep end
 *
 * \param[in] device Pointer to the device settings structure
 * \param[in] channelSelect Channel mask indicating which channel is being captured
 * \param[out] address 32-bit register base address of the capture memory location
 *
* \retval adi_common_ErrAction_e - ADI_COMMON_ACT_NO_ACTION if Successful
 */

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxOrxDataCaptureConfigAddressGet(adi_adrv904x_Device_t* const    device,
                                                                           const adi_adrv904x_RxChannels_e channelSelect,
                                                                           uint32_t* const                 address);


/**
 * \brief Helper function to poll the channel stream debug register to determine the state of the ram capture flag
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in, out] device Pointer to the device settings structure
 * \param[out] bStreamDbgFlag read only value is set to 0 when the stream processor has initiated a data capture
 *
* \retval adi_common_ErrAction_e - ADI_COMMON_ACT_NO_ACTION if Successful
 */

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxOrxDataCaptureStreamDebugPoll(adi_adrv904x_Device_t* const    device,
                                                                          const adi_adrv904x_RxChannels_e channelSelect,
                                                                          uint32_t*  const                bStreamDbgFlag);


/**
* \brief Look up the framer function bitfield address given a framer index
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Pointer to the ADRV904X data structure
* \param[in]  framerIdx Framer selection index
* \param[out] framerBitfieldAddr Framer bitfield address which will be updated by this function
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_FramerBitfieldAddressGet(adi_adrv904x_Device_t* const        device,
                                                                   const adi_adrv904x_FramerSel_e      framerIdx,
                                                                   adrv904x_BfJtxLinkChanAddr_e* const framerBitfieldAddr);

/**
* \brief Look up the framer function bitfield address given a deframer index
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Pointer to the ADRV904X data structure
* \param[in]  deframerIdx Deframer selection index
* \param[out] deframerBitfieldAddr Deframer bitfield address which will be updated by this function
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_DeframerBitfieldAddressGet(adi_adrv904x_Device_t* const        device,
                                                                     const adi_adrv904x_DeframerSel_e    deframerIdx,
                                                                     adrv904x_BfJrxLinkChanAddr_e* const deframerBitfieldAddr);

/**
* \brief Look up the lane serdes PHY bitfield address given a lane index
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Pointer to the ADRV904X data structure
* \param[in]  laneIdx lane selection index
* \param[out] laneSerdesPhyBitfieldAddr lane serdes phy bitfield address which will be updated by this function
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_FramerLaneSerdesPhyBitfieldAddressGet(adi_adrv904x_Device_t* const                            device,
                                                                                const uint8_t                                           laneIdx,
                                                                                adrv904x_BfSerdesTxdigPhyRegmapCore1p2ChanAddr_e* const laneSerdesPhyBitfieldAddr);

/**
* \brief this private function is to get the lane crossbar for the selected framer.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Pointer to the ADRV904X data structure
* \param[in] framerSel selected of framer defined in adi_adrv904x_FramerSel_e
* \param[inout] framerCfg Pointer to the JESD Framer configuration read back
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_FramerLaneEnableGet(adi_adrv904x_Device_t* const device,
                                                              const adi_adrv904x_FramerSel_e   framerSel,
                                                              adi_adrv904x_FramerCfg_t* const  framerCfg);

/**
* \brief this private function is to get the lane crossbar for the selected deframer.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Pointer to the ADRV904X data structure
* \param[in]  deframerSel selected of framer defined in adi_adrv904x_DeframerSel_e
* \param[inout] deframerCfg Pointer to the JESD Deframer configuration read back
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_DeframerLaneEnableGet(adi_adrv904x_Device_t* const device,
                                                                const adi_adrv904x_DeframerSel_e   deframerSel,
                                                                adi_adrv904x_DeframerCfg_t* const  deframerCfg);

/**
* \brief this private function is to set the lanes serdes phy power up or down associated to the selected framer.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] framerSelMask selected of framers defined in adi_adrv904x_FramerSel_e
* \param[in] powerAct indicate the action power up or down.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_FramerLaneSerdesPowerSet(adi_adrv904x_Device_t* const device,
                                                                   const uint8_t                framerSelMask,
                                                                   const uint8_t                powerAct);

/**
* \brief this private function is to set the lanes serdes phy power up or down associated to the selected deframer.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] deframerSelMask selected of deframers defined in adi_adrv904x_DeframerSel_e
* \param[in] powerAct indicate the action power up or down.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_DeframerLaneSerdesPowerSet(adi_adrv904x_Device_t* const device,
                                                                     const uint8_t                deframerSelMask,
                                                                     const uint8_t                powerAct);

/**
* \brief this function calculates the clock divider setting required to set the DPD RAM capture clock rate to match the Rx output sample rate.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Pointer to the ADRV904X data structure
* \param[in] channelSelect The Rx channel that is doing the data capture
* \param[out] value The divider value required to set the DPD RAM clock rate to match the Rx output sample rate.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_RadClkDividerValueCalculate(adi_adrv904x_Device_t* const    device,
                                                                      const adi_adrv904x_RxChannels_e channelSelect,
                                                                      const adi_adrv904x_RxOrxDataCaptureLocation_e loc,
                                                                      uint8_t* const                  value);


/**
* \brief this function polls the capture busy bitfield in the appropriate capture status register indicated by the channelSelect parameter.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Pointer to the ADRV904X data structure
* \param[in] channelSelect The Rx channel that is doing the data capture
* \param[out] bCptBusy variable that stores the status of the capture busy bitfield
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_StatusRegisterPoll(adi_adrv904x_Device_t* const    device,
                                                             const adi_adrv904x_RxChannels_e channelSelect,
                                                             uint8_t* const                  bCptBusy);

/**
* \brief This Function Reads the Jrx Repair screen Id information to check if the device has undergone the CM screen
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device    Pointer to the ADRV904X data structure
* \param[out]    screenID  if 1 the device has undergone the CM screen, 0 otherwise
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxRepairScreenTestChecker(adi_adrv904x_Device_t* const   device,
                                                                     uint8_t* const                 screenID);

#endif  /* _ADRV904X_DATAINTERFACE_H_ */
