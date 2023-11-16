/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file fpgagen6_tdd.h
 * \brief Contains prototypes and macro definitions for ADI TDD wrapper
 *        functions implemented in adi_fpgagen6_tdd.c
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _FPGAGEN6_TDD_H_
#define _FPGAGEN6_TDD_H_

#include "fpgagen6_tdd_types.h"
#include "fpgagen6_data_interface.h"

/**
*  \brief   FPGAGEN6 TDD TRX Enable Control Region Function Pointers
*/
extern const enableRegionSetFnPtr_t trxEnableStartValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];
extern const enableRegionSetFnPtr_t trxEnableDurationValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];
extern const enableRegionGetFnPtr_t trxEnableStartValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];
extern const enableRegionGetFnPtr_t trxEnableDurationValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];

/**
*  \brief   FPGAGEN6 TDD ORX Enable Control Region Function Pointers
*/
extern const enableRegionSetFnPtr_t orxEnableStartValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];
extern const enableRegionSetFnPtr_t orxEnableDurationValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];
extern const enableRegionGetFnPtr_t orxEnableStartValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];
extern const enableRegionGetFnPtr_t orxEnableDurationValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];

/**
*  \brief   FPGAGEN6 TDD GPIO Enable Control Region Function Pointers
*/
extern const enableRegionSetFnPtr_t gpioEnableStartValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];
extern const enableRegionSetFnPtr_t gpioEnableDurationValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];
extern const enableRegionGetFnPtr_t gpioEnableStartValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];
extern const enableRegionGetFnPtr_t gpioEnableDurationValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];

/**
*  \brief   FPGAGEN6 TDD EXT Enable Control Region Function Pointers
*/
extern const enableRegionSetFnPtr_t extEnableStartValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];
extern const enableRegionSetFnPtr_t extEnableDurationValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];
extern const enableRegionGetFnPtr_t extEnableStartValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];
extern const enableRegionGetFnPtr_t extEnableDurationValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS];


/**
* \brief Service to Configure TDD Enable Control
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
* \param[in] tddEnableModule    TDD Enable Module Type
* \param[in] tddEnableIdx       TDD Enable Pin Index
* \param[in] tddControlCfg      TDD Configuration Structure
*
* \retval ADI_FPGAGEN6_ERR_ACT_NONE Function completed successfully, no action required
*
*/
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_TddEnableControlCfgSet(   adi_fpgagen6_Device_t* const                    device,
                                                                    const fpgagen6_TddEnableModule_e                tddEnableModule,
                                                                    const uint8_t                                   tddEnableIdx,
                                                                    const adi_fpgagen6_TddEnableControl_t* const    tddControlCfg);

/**
* \brief Service to Get TDD Enable Control Configuration
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
* \param[in] tddEnableModule    TDD Enable Module Type
* \param[in] tddEnableIdx       TDD Enable Pin Index
* \param[out] tddControlCfg     TDD Configuration Structure
*
* \retval ADI_FPGAGEN6_ERR_ACT_NONE Function completed successfully, no action required
*
*/
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_TddEnableControlCfgGet(   adi_fpgagen6_Device_t* const            device,
                                                                    const fpgagen6_TddEnableModule_e        tddEnableModule,
                                                                    const uint8_t                           tddEnableIdx,
                                                                    adi_fpgagen6_TddEnableControl_t* const  tddControlCfg);

/**
* \brief    To enable/disable TDD GPIO Sync
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device   Context variable -Pointer to the FPGA device data structure.
* \param[in]     enable   TDD GPIO Sync defined in adi_fpgagen6_TddGpioSync_e
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_TddGpioSyncEnableSet(adi_fpgagen6_Device_t* const      device,
                                                               const adi_fpgagen6_TddGpioSync_e  enable);

/**
* \brief    Read TDD GPIO Sync setting state. 
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device   Context variable -Pointer to the FPGA device data structure
* \param[out]    enable   Pointer to contain TDD GPIO Sync defined in adi_fpgagen6_TddGpioSync_e.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_TddGpioSyncEnableGet(adi_fpgagen6_Device_t* const          device,
                                                               adi_fpgagen6_TddGpioSync_e * const    enable);

/**
* \brief    To set selected TDD GPIO Sync.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device       Context variable -Pointer to the FPGA device data structure.
* \param[in]     gpioSyncSel  Selected GPIO pin.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_TddGpioSyncSelectSet(adi_fpgagen6_Device_t* const   device,
                                                               const adi_fpgagen6_GpioPinSel_e  gpioSyncSel);

/**
* \brief    Reading TDD GPIO Sync pin.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device        Context variable -Pointer to the FPGA device data structure.
* \param[out]    gpioSyncSel   Pointer to hold selected GPIO pin in adi_fpgagen6_GpioPinSel_e.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_TddGpioSyncSelectGet(adi_fpgagen6_Device_t* const       device,
                                                               adi_fpgagen6_GpioPinSel_e * const  gpioSyncSel);

#endif  /* _FPGAGEN6_TDD_H_ */