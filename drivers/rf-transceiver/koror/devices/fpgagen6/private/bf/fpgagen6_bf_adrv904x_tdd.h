/** 
 * \file fpgagen6_bf_adrv904x_tdd.h Automatically generated file with generator ver 0.0.16.0.
 * 
 * \brief Contains BitField functions to support FPGAGEN6 transceiver device.
 * 
 * FPGAGEN6 BITFIELD VERSION: 0.0.0.11
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2023 Analog Devices Inc.
 * 
 * Released under the FPGAGEN6 API license, for more information see the "LICENSE.PDF" file in this zip file.
 */

#ifndef _FPGAGEN6_BF_ADRV904X_TDD_H_
#define _FPGAGEN6_BF_ADRV904X_TDD_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"
#include "../../private/bf/fpgagen6_bf_adrv904x_tdd_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif



/**
 * \brief This field indicates the number of active frames of the External Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableFramesActive_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint16_t bfValue);


/**
 * \brief This field indicates the number of active frames of the External Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableFramesActive_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint16_t* const bfValue);



/**
 * \brief This field indicates the number of inactive frames of the External Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableFramesInactive_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    const uint16_t bfValue);


/**
 * \brief This field indicates the number of inactive frames of the External Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableFramesInactive_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    uint16_t* const bfValue);



/**
 * \brief This field indicates the number of offset frames of the External Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableFramesOffset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint16_t bfValue);


/**
 * \brief This field indicates the number of offset frames of the External Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableFramesOffset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint16_t* const bfValue);



/**
 * \brief This bit field serves as a unique identifier of each External Enable Type.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);



/**
 * \brief This field indicates the Manual enable for External Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableManual_BfSet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);


/**
 * \brief This field indicates the Manual enable for External Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableManual_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);



/**
 * \brief The number of clock cycles that the External Enable signal is to remain active during Enable Region 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableRegion0Duration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint32_t bfValue);


/**
 * \brief The number of clock cycles that the External Enable signal is to remain active during Enable Region 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableRegion0Duration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint32_t* const bfValue);



/**
 * \brief Start value for region0 of the External Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableRegion0StartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint32_t bfValue);


/**
 * \brief Start value for region0 of the External Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableRegion0StartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       uint32_t* const bfValue);



/**
 * \brief The number of clock cycles that the External Enable signal is to remain active during Enable Region 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableRegion1Duration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint32_t bfValue);


/**
 * \brief The number of clock cycles that the External Enable signal is to remain active during Enable Region 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableRegion1Duration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint32_t* const bfValue);



/**
 * \brief Start value for region1 of the External Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableRegion1StartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint32_t bfValue);


/**
 * \brief Start value for region1 of the External Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableRegion1StartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       uint32_t* const bfValue);



/**
 * \brief The number of clock cycles that the External Enable signal is to remain active during Enable Region 2.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableRegion2Duration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint32_t bfValue);


/**
 * \brief The number of clock cycles that the External Enable signal is to remain active during Enable Region 2.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableRegion2Duration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint32_t* const bfValue);



/**
 * \brief Start value for region2 of the External Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableRegion2StartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint32_t bfValue);


/**
 * \brief Start value for region2 of the External Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableRegion2StartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       uint32_t* const bfValue);



/**
 * \brief The number of clock cycles that the External Enable signal is to remain active during Enable Region 3.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableRegion3Duration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint32_t bfValue);


/**
 * \brief The number of clock cycles that the External Enable signal is to remain active during Enable Region 3.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableRegion3Duration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint32_t* const bfValue);



/**
 * \brief Start value for region3 of the External Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableRegion3StartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint32_t bfValue);


/**
 * \brief Start value for region3 of the External Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableRegion3StartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       uint32_t* const bfValue);



/**
 * \brief This bit field indicates the type of enable that the Enable Cluster services.  Valid values include: 4'h0 TRX Enable; 4'h1 ORX Enable; 4'h2 DP Enable; 4'h3 GPIO Enable; 4'h4 EXT Enable
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtEnableType_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          uint8_t* const bfValue);



/**
 * \brief Enable for the Region(0 to 3) of the Enable signal, Each bit the corresponds to the regions respectively
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtRegionEnables_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             const uint8_t bfValue);


/**
 * \brief Enable for the Region(0 to 3) of the Enable signal, Each bit the corresponds to the regions respectively
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtRegionEnables_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);



/**
 * \brief This bit sets the TDD SM in trigger mode, which means the TDD SM can be set to active with the external trigger source
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtTriggerEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);


/**
 * \brief This bit sets the TDD SM in trigger mode, which means the TDD SM can be set to active with the external trigger source
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_ExtTriggerEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);



/**
 * \brief The value at which the frame counter truncates and loop again
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_FrameCnt_BfSet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                     const uint32_t bfValue);


/**
 * \brief The value at which the frame counter truncates and loop again
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_FrameCnt_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                     uint32_t* const bfValue);



/**
 * \brief This field indicates the number of active frames of the GPIO Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableFramesActive_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   const uint16_t bfValue);


/**
 * \brief This field indicates the number of active frames of the GPIO Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableFramesActive_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   uint16_t* const bfValue);



/**
 * \brief This field indicates the number of inactive frames of the GPIO Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableFramesInactive_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint16_t bfValue);


/**
 * \brief This field indicates the number of inactive frames of the GPIO Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableFramesInactive_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint16_t* const bfValue);



/**
 * \brief This field indicates the number of offset frames of the GPIO Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableFramesOffset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   const uint16_t bfValue);


/**
 * \brief This field indicates the number of offset frames of the GPIO Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableFramesOffset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   uint16_t* const bfValue);



/**
 * \brief This bit field serves as a unique identifier of each GPIO Enable Type.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         uint8_t* const bfValue);



/**
 * \brief This field indicates the Manual enable for GPIO Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableManual_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             const uint8_t bfValue);


/**
 * \brief This field indicates the Manual enable for GPIO Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableManual_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);



/**
 * \brief The number of clock cycles that the GPIO Enable signal is to remain active during Enable Region 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableRegion0Duration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                      uint8_t channelId, 
                                                                                      const uint32_t bfValue);


/**
 * \brief The number of clock cycles that the GPIO Enable signal is to remain active during Enable Region 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableRegion0Duration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                      uint8_t channelId, 
                                                                                      uint32_t* const bfValue);



/**
 * \brief Start value for region0 of the GPIO Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableRegion0StartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                        uint8_t channelId, 
                                                                                        const uint32_t bfValue);


/**
 * \brief Start value for region0 of the GPIO Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableRegion0StartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                        uint8_t channelId, 
                                                                                        uint32_t* const bfValue);



/**
 * \brief The number of clock cycles that the GPIO Enable signal is to remain active during Enable Region 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableRegion1Duration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                      uint8_t channelId, 
                                                                                      const uint32_t bfValue);


/**
 * \brief The number of clock cycles that the GPIO Enable signal is to remain active during Enable Region 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableRegion1Duration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                      uint8_t channelId, 
                                                                                      uint32_t* const bfValue);



/**
 * \brief Start value for region1 of the GPIO Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableRegion1StartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                        uint8_t channelId, 
                                                                                        const uint32_t bfValue);


/**
 * \brief Start value for region1 of the GPIO Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableRegion1StartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                        uint8_t channelId, 
                                                                                        uint32_t* const bfValue);



/**
 * \brief The number of clock cycles that the GPIO Enable signal is to remain active during Enable Region 2.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableRegion2Duration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                      uint8_t channelId, 
                                                                                      const uint32_t bfValue);


/**
 * \brief The number of clock cycles that the GPIO Enable signal is to remain active during Enable Region 2.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableRegion2Duration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                      uint8_t channelId, 
                                                                                      uint32_t* const bfValue);



/**
 * \brief Start value for region2 of the GPIO Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableRegion2StartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                        uint8_t channelId, 
                                                                                        const uint32_t bfValue);


/**
 * \brief Start value for region2 of the GPIO Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableRegion2StartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                        uint8_t channelId, 
                                                                                        uint32_t* const bfValue);



/**
 * \brief The number of clock cycles that the GPIO Enable signal is to remain active during Enable Region 3.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableRegion3Duration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                      uint8_t channelId, 
                                                                                      const uint32_t bfValue);


/**
 * \brief The number of clock cycles that the GPIO Enable signal is to remain active during Enable Region 3.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableRegion3Duration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                      uint8_t channelId, 
                                                                                      uint32_t* const bfValue);



/**
 * \brief Start value for region3 of the GPIO Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableRegion3StartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                        uint8_t channelId, 
                                                                                        const uint32_t bfValue);


/**
 * \brief Start value for region3 of the GPIO Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableRegion3StartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                        uint8_t channelId, 
                                                                                        uint32_t* const bfValue);



/**
 * \brief This bit field indicates the type of enable that the Enable Cluster services.  Valid values include: 4'h0 TRX Enable; 4'h1 ORX Enable; 4'h2 DP Enable; 4'h3 GPIO Enable; 4'h4 EXT Enable
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioEnableType_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           uint8_t* const bfValue);



/**
 * \brief Enable for the Region(0 to 3) of the Enable signal, Each bit the corresponds to the regions respectively
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioRegionEnables_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint8_t bfValue);


/**
 * \brief Enable for the Region(0 to 3) of the Enable signal, Each bit the corresponds to the regions respectively
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioRegionEnables_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);



/**
 * \brief With every GPIO detection, regardless of synchronization mode, the value of the frame counter is latched into this field to provide information of timing drift.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioSyncDrift_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                          uint32_t* const bfValue);



/**
 * \brief This bit field sets the mode of synchronization of the frame counter to a GPIO pin.  Valid values are: 
 *        '00: disabled;  
 *        '01: one-time synchronization - sync to GPIO at time 0 only;  
 *        '11: continuous synchronization - sync to GPIO always;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioSyncEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);


/**
 * \brief This bit field sets the mode of synchronization of the frame counter to a GPIO pin.  Valid values are: 
 *        '00: disabled;  
 *        '01: one-time synchronization - sync to GPIO at time 0 only;  
 *        '11: continuous synchronization - sync to GPIO always;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioSyncEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



/**
 * \brief Identify which GPIO the GPIO Sync feature uses.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioSyncGpioSel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);


/**
 * \brief Identify which GPIO the GPIO Sync feature uses.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_GpioSyncGpioSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);



/**
 * \brief All IP cores allow (if applicable) multiple instantiations in master-slave configuration. The master instance identifier is 0x0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                       uint32_t* const bfValue);



/**
 * \brief This value indicates the number of times the loop counter rolls over
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_LoopCnt_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                    const uint16_t bfValue);


/**
 * \brief This value indicates the number of times the loop counter rolls over
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_LoopCnt_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                    uint16_t* const bfValue);



/**
 * \brief This bit enables the looping of the frames
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_LoopCntEn_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);


/**
 * \brief This bit enables the looping of the frames
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_LoopCntEn_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);



/**
 * \brief This bit field identifies the number of enable register clusters that are present in the implementation of this IP block.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_NumEnables_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                       uint32_t* const bfValue);



/**
 * \brief This field indicates the number of active frames of the ORX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableFramesActive_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint16_t bfValue);


/**
 * \brief This field indicates the number of active frames of the ORX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableFramesActive_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint16_t* const bfValue);



/**
 * \brief This field indicates the number of inactive frames of the ORX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableFramesInactive_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    const uint16_t bfValue);


/**
 * \brief This field indicates the number of inactive frames of the ORX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableFramesInactive_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    uint16_t* const bfValue);



/**
 * \brief This field indicates the number of offset frames of the ORX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableFramesOffset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint16_t bfValue);


/**
 * \brief This field indicates the number of offset frames of the ORX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableFramesOffset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint16_t* const bfValue);



/**
 * \brief This bit field serves as a unique identifier of each ORX Enable Type.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);



/**
 * \brief This field indicates the Manual enable for ORX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableManual_BfSet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);


/**
 * \brief This field indicates the Manual enable for ORX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableManual_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);



/**
 * \brief The number of clock cycles that the ORX Enable signal is to remain active during Enable Region 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableRegion0Duration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint32_t bfValue);


/**
 * \brief The number of clock cycles that the ORX Enable signal is to remain active during Enable Region 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableRegion0Duration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint32_t* const bfValue);



/**
 * \brief Start value for region0 of the ORX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableRegion0StartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint32_t bfValue);


/**
 * \brief Start value for region0 of the ORX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableRegion0StartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       uint32_t* const bfValue);



/**
 * \brief The number of clock cycles that the ORX Enable signal is to remain active during Enable Region 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableRegion1Duration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint32_t bfValue);


/**
 * \brief The number of clock cycles that the ORX Enable signal is to remain active during Enable Region 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableRegion1Duration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint32_t* const bfValue);



/**
 * \brief Start value for region1 of the ORX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableRegion1StartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint32_t bfValue);


/**
 * \brief Start value for region1 of the ORX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableRegion1StartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       uint32_t* const bfValue);



/**
 * \brief The number of clock cycles that the ORX Enable signal is to remain active during Enable Region 2.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableRegion2Duration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint32_t bfValue);


/**
 * \brief The number of clock cycles that the ORX Enable signal is to remain active during Enable Region 2.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableRegion2Duration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint32_t* const bfValue);



/**
 * \brief Start value for region2 of the ORX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableRegion2StartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint32_t bfValue);


/**
 * \brief Start value for region2 of the ORX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableRegion2StartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       uint32_t* const bfValue);



/**
 * \brief The number of clock cycles that the ORX Enable signal is to remain active during Enable Region 3.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableRegion3Duration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint32_t bfValue);


/**
 * \brief The number of clock cycles that the ORX Enable signal is to remain active during Enable Region 3.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableRegion3Duration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint32_t* const bfValue);



/**
 * \brief Start value for region3 of the ORX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableRegion3StartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint32_t bfValue);


/**
 * \brief Start value for region3 of the ORX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableRegion3StartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       uint32_t* const bfValue);



/**
 * \brief This bit field indicates the type of enable that the Enable Cluster services.  Valid values include: 4'h0 TRX Enable; 4'h1 ORX Enable; 4'h2 DP Enable; 4'h3 GPIO Enable; 4'h4 EXT Enable
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxEnableType_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          uint8_t* const bfValue);



/**
 * \brief Enable for the Region(0 to 3) of the Enable signal, Each bit the corresponds to the regions respectively
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxRegionEnables_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             const uint8_t bfValue);


/**
 * \brief Enable for the Region(0 to 3) of the Enable signal, Each bit the corresponds to the regions respectively
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_OrxRegionEnables_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);



/**
 * \brief Start value for the trigger signal to the Receive Data Path.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_RxdpStartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           const uint32_t bfValue);


/**
 * \brief Start value for the trigger signal to the Receive Data Path.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_RxdpStartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           uint32_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_Scratch_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                    const uint32_t bfValue);


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                    uint32_t* const bfValue);



/**
 * \brief This bit enables the TDD SM to be active
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TddEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);


/**
 * \brief This bit enables the TDD SM to be active
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TddEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);



/**
 * \brief This field indicates the number of active frames of the TRX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableFramesActive_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint16_t bfValue);


/**
 * \brief This field indicates the number of active frames of the TRX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableFramesActive_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint16_t* const bfValue);



/**
 * \brief This field indicates the number of inactive frames of the TRX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableFramesInactive_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    const uint16_t bfValue);


/**
 * \brief This field indicates the number of inactive frames of the TRX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableFramesInactive_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    uint16_t* const bfValue);



/**
 * \brief This field indicates the number of offset frames of the TRX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableFramesOffset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint16_t bfValue);


/**
 * \brief This field indicates the number of offset frames of the TRX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableFramesOffset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint16_t* const bfValue);



/**
 * \brief This bit field serves as a unique identifier of each TRX Enable Type.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);



/**
 * \brief This field indicates the Manual enable for TRX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableManual_BfSet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);


/**
 * \brief This field indicates the Manual enable for TRX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableManual_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);



/**
 * \brief The number of clock cycles that the TRX Enable signal is to remain active during Enable Region 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableRegion0Duration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint32_t bfValue);


/**
 * \brief The number of clock cycles that the TRX Enable signal is to remain active during Enable Region 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableRegion0Duration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint32_t* const bfValue);



/**
 * \brief Start value for region0 of the TRX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableRegion0StartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint32_t bfValue);


/**
 * \brief Start value for region0 of the TRX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableRegion0StartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       uint32_t* const bfValue);



/**
 * \brief The number of clock cycles that the TRX Enable signal is to remain active during Enable Region 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableRegion1Duration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint32_t bfValue);


/**
 * \brief The number of clock cycles that the TRX Enable signal is to remain active during Enable Region 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableRegion1Duration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint32_t* const bfValue);



/**
 * \brief Start value for region1 of the TRX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableRegion1StartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint32_t bfValue);


/**
 * \brief Start value for region1 of the TRX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableRegion1StartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       uint32_t* const bfValue);



/**
 * \brief The number of clock cycles that the TRX Enable signal is to remain active during Enable Region 2.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableRegion2Duration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint32_t bfValue);


/**
 * \brief The number of clock cycles that the TRX Enable signal is to remain active during Enable Region 2.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableRegion2Duration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint32_t* const bfValue);



/**
 * \brief Start value for region2 of the TRX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableRegion2StartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint32_t bfValue);


/**
 * \brief Start value for region2 of the TRX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableRegion2StartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       uint32_t* const bfValue);



/**
 * \brief The number of clock cycles that the TRX Enable signal is to remain active during Enable Region 3.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableRegion3Duration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint32_t bfValue);


/**
 * \brief The number of clock cycles that the TRX Enable signal is to remain active during Enable Region 3.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableRegion3Duration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint32_t* const bfValue);



/**
 * \brief Start value for region3 of the TRX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableRegion3StartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint32_t bfValue);


/**
 * \brief Start value for region3 of the TRX Enable signal
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableRegion3StartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       uint32_t* const bfValue);



/**
 * \brief This bit field indicates the type of enable that the Enable Cluster services.  Valid values include: 4'h0 TRX Enable; 4'h1 ORX Enable; 4'h2 DP Enable; 4'h3 GPIO Enable; 4'h4 EXT Enable
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxEnableType_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          uint8_t* const bfValue);



/**
 * \brief Enable for the Region(0 to 3) of the Enable signal, Each bit the corresponds to the regions respectively
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxRegionEnables_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             const uint8_t bfValue);


/**
 * \brief Enable for the Region(0 to 3) of the Enable signal, Each bit the corresponds to the regions respectively
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TrxRegionEnables_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);



/**
 * \brief Start value for the trigger signal to the Transmit Data Path.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TxdpStartValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           const uint32_t bfValue);


/**
 * \brief Start value for the trigger signal to the Transmit Data Path.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_TxdpStartValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           uint32_t* const bfValue);



/**
 * \brief This is a local parameter in HDL and follows a MAJOR.MINOR.PATCH format, integers only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6TddChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Tdd_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                                    uint32_t* const bfValue);
#endif // _FPGAGEN6_BF_ADRV904X_TDD_H_

