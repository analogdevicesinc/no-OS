/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file: adi_fpgagen6_prbs.h
 *
 * \brief Contains top level fpgagen6 clock related API functions
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _ADI_FPGAGEN6_PRBS_H_
#define _ADI_FPGAGEN6_PRBS_H_

#include "adi_fpgagen6_prbs_types.h"
#include "adi_fpgagen6_error.h"

/**
* \brief    Pseudo-random bit sequences (PRBS) are commonly used to test the signal integrity of
*           high-speed links. These sequences appear random but have specific properties that can be
*           used to measure the quality of a link. 
*           
*           The industry-standard patterns that are supported in this API can be seen in the
*           adi_fpgagen6_PrbsTestModes_e enum.
*           
*           The patterns can be transmitted on select
*           Serializer channels through the chanSelMask, where bit 0 = 1 would enable PRBS on
*           Serializer channel 0, etc.
*           
*           In order to enter PRBS mode some clock settings must be written to hardcoded values (see
*           Xilinx user guide for more details). Because of this the API will attempt to store the existing
*           values so that when PRBS is disabled those settings can be restored.
*           
*           If this function is called to disable PRBS and there is no pre-existing settings, e.g. the server has
*           been restarted since PRBS was enabled, this function will return and error and the user should instead
*           re-initialize the FPGA PHY modules.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
* \param[in] chSelMask to enable prbs on a channel set the corresponding bit to 1, ie bit 0 = channel 0
* \param[in] prbsModeSel selecting the PRBS test mode
*
* \retval adi_fpgagen6_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_PrbsSerializerEnable( adi_fpgagen6_Device_t* const device,
                                                                    const uint8_t chSelMask,
                                                                    const adi_fpgagen6_PrbsTestModes_e prbsModeSel);

/**
* \brief    The GTY receiver includes a built-in PRBS checker. This checker can be set
*           to check for one of five industry-standard PRBS patterns (see adi_fpgagen6_PrbsTestModes_e).
*           The checker is self-synchronizing and works on the incoming data before comma alignment or 
*           decoding. This function can be used to test the signal integrity of the channel.
*           
*           The pattern checkers can be turned on per receive channel using the chSelMask, where
*           bit 0 will enable the checker on channel 0, etc.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
* \param[in] chSelMask to enable prbs on a channel set the corresponding bit to 1, ie bit 0 = channel 0
* \param[in] prbsModeSel selecting the PRBS test mode
*
* \retval adi_fpgagen6_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_PrbsDeserializerEnable(   adi_fpgagen6_Device_t* const device,
                                                                        const uint8_t chSelMask,
                                                                        const adi_fpgagen6_PrbsTestModes_e prbsModeSel);

/**
* \brief Injects are PRBS error on the selected channels. Does not check if PRBS is enabled for the selected channels.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
* \param[in] chSelMask to inject an erroron a channel set the corresponding bit to 1, ie bit 0 = channel 0
*
* \retval adi_fpgagen6_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_PrbsSerializerErrorInject(adi_fpgagen6_Device_t* const device,
                                                                        const uint8_t chSelMask);

/**
* \brief    Reads FPGA deserializer PRBS error counts on all channels and stores the values in the corresopnding
*           channel error count in the adi_fpgagen6_PrbsErrorCount_t structure.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
* \param[out] prbsErrorCounts pointer to a structure that will hold the error count for each channel
*
* \retval adi_fpgagen6_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_PrbsErrorCountRead(   adi_fpgagen6_Device_t* const device,
                                                                    adi_fpgagen6_PrbsErrorCount_t* const prbsErrorCounts);

/**
* \brief  Clears the error counter on the selected deserializer channels.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
* \param[in] chSelMask to clear the errors on a channel set the corresponding bit to 1, ie bit 0 = channel 0
*
* \retval adi_fpgagen6_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_PrbsErrorClear(   adi_fpgagen6_Device_t* const device,
                                                                const uint8_t chSelMask);

#endif /* _ADI_FPGAGEN6_PRBS_H_ */
