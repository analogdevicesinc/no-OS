/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file fpgagen6_tdd_types.h
 * \brief Contains FPGA TDD enum and struct definitions
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _FPGAGEN6_TDD_TYPES_H_
#define _FPGAGEN6_TDD_TYPES_H_

#include "adi_fpgagen6_tdd_types.h"

#include "../../private/bf/fpgagen6_bf_adrv904x_tdd.h"

/**
*   \brief  TDD Enable Module Types
*/
typedef enum fpgagen6_TddEnableModule
{
    FPGAGEN6_TDD_ENABLEMODULE_TRX = 0U, /* TRX Enable Module */
    FPGAGEN6_TDD_ENABLEMODULE_ORX,      /* ORX Enable Module */
    FPGAGEN6_TDD_ENABLEMODULE_GPIO,     /* GPIO Enable Module */
    FPGAGEN6_TDD_ENABLEMODULE_EXT       /* EXT Enable Module */
} fpgagen6_TddEnableModule_e;

/**
*  \brief TDD Enable Control Enable Region Set Function Pointer Type
*/
typedef adi_fpgagen6_ErrAction_e (*enableRegionSetFnPtr_t) (adi_fpgagen6_Device_t* const        device,
                                                            const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                            uint8_t                             channelId,
                                                            const uint32_t                      bfValue);

/**
*  \brief TDD Enable Control Enable Region Get Function Pointer Type
*/
typedef adi_fpgagen6_ErrAction_e (*enableRegionGetFnPtr_t) (adi_fpgagen6_Device_t* const        device,
                                                            const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                            uint8_t                             channelId,
                                                            uint32_t* const                     bfValue);

/**
*  \brief TDD Enable Control Enable Set Function Pointers
*/
typedef struct fpgagen6_TddEnableControlBfSet
{
    enableRegionSetFnPtr_t enableStartValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];
    enableRegionSetFnPtr_t enableDurationValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];

    adi_fpgagen6_ErrAction_e(*enableRegionBfSet)(   adi_fpgagen6_Device_t* const        device,
                                                    const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                    uint8_t                             channelId,
                                                    const uint8_t                       bfValue);

    adi_fpgagen6_ErrAction_e(*enableFramesOffsetBfSet)( adi_fpgagen6_Device_t* const        device,
                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                        uint8_t                             channelId,
                                                        const uint16_t                      bfValue);

    adi_fpgagen6_ErrAction_e(*enableFramesActiveBfSet)( adi_fpgagen6_Device_t* const         device,
                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e  baseAddr,
                                                        uint8_t                              channelId,
                                                        const uint16_t                       bfValue);

    adi_fpgagen6_ErrAction_e(*enableFramesInctiveBfSet)(adi_fpgagen6_Device_t* const         device,
                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e  baseAddr,
                                                        uint8_t                              channelId,
                                                        const uint16_t                       bfValue);
} fpgagen6_TddEnableControlBfSet_t;

/**
*  \brief TDD Enable Control Enable Get Function Pointers
*/
typedef struct fpgagen6_TddEnableControlBfGet
{
    enableRegionGetFnPtr_t enableStartValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];
    enableRegionGetFnPtr_t enableDurationValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];

    adi_fpgagen6_ErrAction_e(*enableRegionBfGet)(   adi_fpgagen6_Device_t* const        device,
                                                    const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                    uint8_t                             channelId,
                                                    const uint8_t                       bfValue);

    adi_fpgagen6_ErrAction_e(*enableFramesOffsetBfGet)( adi_fpgagen6_Device_t* const        device,
                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                        uint8_t                             channelId,
                                                        uint16_t* const                     bfValue);

    adi_fpgagen6_ErrAction_e(*enableFramesActiveBfGet)( adi_fpgagen6_Device_t* const        device,
                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                        uint8_t                             channelId,
                                                        uint16_t* const                     bfValue);

    adi_fpgagen6_ErrAction_e(*enableFramesInctiveBfGet)(adi_fpgagen6_Device_t* const        device,
                                                        const fpgagen6_BfAdrvgen6TddChanAddr_e baseAddr,
                                                        uint8_t                             channelId,
                                                        uint16_t* const                     bfValue);

} fpgagen6_TddEnableControlBfGet_t;

#endif /* _FPGAGEN6_TDD_TYPES_H_ */
