/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_fpgagen6_tdd.h
 * \brief Contains prototypes and macro definitions for ADI TDD wrapper
 *        functions implemented in adi_fpgagen6_tdd.c
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _ADI_FPGAGEN6_TDD_H_
#define _ADI_FPGAGEN6_TDD_H_

#include "adi_fpgagen6_tdd_types.h"
#include "adi_fpgagen6_error.h"

/**
* \brief Service to Configure TDD State Machine
*
*       The configuration is done as follows:
*               1) Frame & Loop Counters
*               2) External Trigger Flag
*               3) TRX, ORX, GPIO & EXT Enable Modules
*                   Start & Duration Values per 4 Regions per Pin Enable Control
*                   Frame Offset, Active, Inactive per Pin Enable Control
*                   NOTE: Manual Enable Signal are handled via separate API
*               4) TX & RX Datapaths
*                   Start Value per Data Path
*                   NOTE: Modules are enabled by Default
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
* \param[in] tddFsmCfg TDD State Machine Configuration Data
*
* \retval ADI_FPGAGEN6_ERR_ACT_NONE Function completed successfully, no action required
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddFsmCfgSet( adi_fpgagen6_Device_t* const                    device,
                                                            const adi_fpgagen6_TddStateMachineCfg_t* const  tddFsmCfg);


/**
* \brief Service to Get TDD State Machine Configuration
*
*
*       The configuration is retrieved as follows:
*               1) Frame & Loop Counters
*               2) External Trigger Flag
*               3) TRX, ORX, GPIO & EXT Enable Modules
*                   Start & Duration Values per 4 Regions per Pin Enable Control
*                   Frame Offset, Active, Inactive per Pin Enable Control
*                   NOTE: Manual Enable Signal are handled via separate API
*               4) TX & RX Datapaths
*                   Start Value per Data Path
*                   NOTE: Data Path Signals Always Enabled (i.e. DMA decides to use or not)
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
* \param[out] tddFsmCfg TDD State Machine Configuration Data
*
* \retval ADI_FPGAGEN6_ERR_ACT_NONE Function completed successfully, no action required
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddFsmCfgGet( adi_fpgagen6_Device_t* const                device,
                                                            adi_fpgagen6_TddStateMachineCfg_t* const    tddFsmCfg);


/**
* \brief Service Enable TDD State Machine
*
*           This API only sets the FSM Enable Flag. Configuration should be done first
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
*
* \retval ADI_FPGAGEN6_ERR_ACT_NONE Function completed successfully, no action required
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddFsmEnable(adi_fpgagen6_Device_t* const device);


/**
* \brief Service to Disable TDD State Machine
*           The following actions are taken:
*                           1) Disable Looping of Frames
*                           2) Disable TDD State Machine
*                           3) Reset Frame & Loop Counts
*                           4) Enable Control Pin Regions Disabled - Reconfiguration of FSM Required before restart
*                           5) No action taken on Data Paths
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
*
* \retval ADI_FPGAGEN6_ERR_ACT_NONE Function completed successfully, no action required
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddFsmDisable(adi_fpgagen6_Device_t* const device);


/**
* \brief    Service to Configure Manual Enables for TRX, ORX, GPIO & External
*
*           This is a manual control used for debug and is overridden
*           anytime the TDD state machine is enabled. The function will
*           NOT disable all TDD modules prior to asserting the enables, it
*           is incumbent on the user to disable TDD prior to using manual
*           enable assertion. The value of the bit for each TDD enable module
*           is what will be written to the manual assert value.
*
* *         Note: LSB -> TRX0, ORX0, External0
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device             Context variable -Pointer to the FPGA device data structure.
* \param[in] manualEnableMaskInfo   Pointer to Structure containing BitMasks
* bits for assertion.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddManualEnableSet(   adi_fpgagen6_Device_t* const                        device,
                                                                    const adi_fpgagen6_TddManualEnableMasks_t* const    manualEnableMaskInfo);


/**
* \brief    Service to Read Manual Enables for TRX, ORX, GPIO & External
*
*           This function will read back the manual enable assert value that is currently written in each
*           Enable Control module. Note: If TDD is not properly disabled, the manual enable value might not
*           correspond with the actual pin logic state.
*
*           Note: LSB -> TRX0, ORX0, External0
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device             Context variable -Pointer to the FPGA device data structure
* \param[out] manualEnableMaskInfo  Pointer to Structure containing BitMasks
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddManualEnableGet(   adi_fpgagen6_Device_t* const                device,
                                                                    adi_fpgagen6_TddManualEnableMasks_t* const  manualEnableMaskInfo);


/**
* \brief    To get TDD GPIO Sync drift.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device          Context variable -Pointer to the FPGA device data structure.
* \param[out]    gpioSyncDrift   Pointer to hold TDD GPIO Syn drift value.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddGpioSyncDriftGet(adi_fpgagen6_Device_t* const  device,
                                                                  uint32_t * const               gpioSyncDrift);

/**
* \brief    To set TDD GPIO Sync configuration.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device          Context variable -Pointer to the FPGA device data structure.
* \param[in]     gpioSyncCfg     Pointer to configuration values in structure adi_fpgagen6_TddGpioSyncSelEn_t.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddGpioSyncCfgSet(adi_fpgagen6_Device_t* const  device,
                                                                adi_fpgagen6_TddGpioSyncSelEn_t * const gpioSyncCfg);

/**
* \brief    To get TDD GPIO Sync configuration.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device           Context variable -Pointer to the FPGA device data structure.
* \param[out]    gpioSyncCfg      Pointer to structure adi_fpgagen6_TddGpioSyncSelEn_t.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddGpioSyncCfgGet(adi_fpgagen6_Device_t* const  device,
                                                                adi_fpgagen6_TddGpioSyncSelEn_t * const gpioSyncCfg);
#endif  /* _ADI_FPGAGEN6_TDD_H_ */