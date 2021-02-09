/**
* \file
* \brief ADRV9001 Multi-Chip Synchronization (MCS) functions
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2020 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV9001_MCS_H_
#define _ADI_ADRV9001_MCS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "adi_adrv9001_mcs_types.h"
#include "adi_common_macros.h"
#include "adi_adrv9001_user.h"
#include "adi_adrv9001_types.h"
#include "adi_adrv9001_radio_types.h"

/**
 * \brief Reads back the multi-chip sync status
 *
 *  This function can be used to verify the synchronization status of the
 *  transceiver. 
 *  After issuing one or all of the MCS pulses, this function can be used to 
 *  check the synchronization status of the analog and digital subsystems.
 * 
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre The device must be in CALIBRATED state before using the MCS functions.
 *
 * \param[in]  adrv9001	        Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[out] mcsStatus        Mcs status structure
 * \parblock
 *  Depending on the current state of the MCS procedure when this function is called,
 *  some, or all, of the structure may be populated.
 * 
 *  Pulse number | Status populated                   
 * --------------|--------------------------------------------------------
 *       [1]     | Nothing updated
 *       [2]     | RF1/RF2/CLK/CLKLP Sync status updated
 *       [3]     | RF1/RF2/CLK/CLKLP Sync status updated
 *       [4]     | RF1/RF2/CLK/CLKLP Sync status updated
 *       [5]     | RF1/RF2/CLK/CLKLP updated, first digital sync status updated, and First Rx LVDS sync status updated
 *       [6]     | second digital sync status updated, and second Rx LVDS sync status updated
 * 
 * A field being set to true indicates the corresponding synchronization occured.
 * A field being set to false indicates the corresponding synchronization did not occur.
 * \endparblock
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_Mcs_Status_Get(adi_adrv9001_Device_t *adrv9001,
		                            adi_adrv9001_McsStatus_t *mcsStatus);         

/**
 * \brief Reads back the multi-chip sync software status
 *
 *  This function can be used to verify the current state of adrv9001
 *  software while issuing MCS pulses. 
 * 
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is called when the device system state is ADI_ADRV9001_ARM_SYSTEM_MCS.
 *
 * \param[in]  adrv9001	        Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[out] mcsSwStatus      Current status of adrv9001 software
 * \parblock
 *  mcsSwStatus will readback as ADI_ADRV9001_MCSSWSTATUS_READY until the second MCS
 *  pulse is issued. 
 *  Depending on the current state of the MCS procedure, the SW state may read back differently.
 *  Pulse number | Sw status                   
 * --------------|--------------------------------------------------------
 *       [0]     | ADI_ADRV9001_MCSSWSTATUS_READY (no pulse received) 
 *       [1]     | ADI_ADRV9001_MCSSWSTATUS_READY
 *       [2]     | ADI_ADRV9001_MCSSWSTATUS_PULSE2_RECEIVED
 *       [3]     | ADI_ADRV9001_MCSSWSTATUS_PULSE3_RECEIVED
 *       [4]     | ADI_ADRV9001_MCSSWSTATUS_PULSE4_RECEIVED
 *       [5]     | ADI_ADRV9001_MCSSWSTATUS_PULSE5_RECEIVED/ADI_ADRV9001_MCSSWSTATUS_DEVICE_SWITCHED_TO_HSCLK
 *       [6]     | ADI_ADRV9001_MCSSWSTATUS_DEVICE_SWITCHED_TO_HSCLK
 * \endparblock
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_Mcs_SwStatus_Get(adi_adrv9001_Device_t *adrv9001,
                                      adi_adrv9001_McsSwStatus_e *mcsSwStatus);                                                            
	                              
/**
 * \brief Gets MCS to strobe delay for Tx
 *
 * Returns the delay, in samples, between the MCS received signal and the
 * first strobe received on the interface.
 * 
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv9001      Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] channel       Tx channel number
 * \param[out] latency      Output delay specified in samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Mcs_TxMcsToStrobeSampleLatency_Get (adi_adrv9001_Device_t *adrv9001, 
                                                         adi_common_ChannelNumber_e channel, 
                                                         uint16_t *latency);

/**
 * \brief Sets MCS sample delay for Tx
 *
 * Delays the MCS received by specified number of samples
 * 
 * \note Message type: \ref timing_direct "Direct register access"
*
 * \param[in] adrv9001      Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] channel       Tx channel number
 * \param[in] delay         Delay specified in samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Mcs_TxMcsSampleDelay_Set (adi_adrv9001_Device_t *adrv9001, 
                                               adi_common_ChannelNumber_e channel, 
                                               uint16_t delay);
                          
#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_MCS_H_ */
