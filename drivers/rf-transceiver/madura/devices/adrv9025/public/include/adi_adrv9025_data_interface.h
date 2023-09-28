/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adi_adrv9025_data_interface.h
 * \brief Contains ADRV9025 JESD204b data path related function prototypes for
 *        adi_adrv9025_jesd204.c
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_DATA_INTERFACE_H_
#define _ADI_ADRV9025_DATA_INTERFACE_H_

#include "adi_adrv9025_types.h"
#include "adi_adrv9025_tx_types.h"
#include "adi_common_error_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

/**
* \brief Gets the JESD204B Framer's configuration
*
* This function reads the JESD204B framer settings.
*
* \pre This function may be called any time after device initialization
*
* \dep_begin
* \dep{device->devHalInfo}
* \dep{init->rxChannels}
* \dep{init->jesd204Settings.framerA :All members}
* \dep{init->jesd204Settings.framerB :All members}
* \dep_end
*
* \param device Structure pointer to the device settings structure
* \param framerSel Select the framer to read
* \param framerCfg Structure pointer to return framer's configuration
*
* \retval ADI_ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_FramerCfgGet(adi_adrv9025_Device_t*   device,
                                  adi_adrv9025_FramerSel_e framerSel,
                                  adi_adrv9025_FrmCfg_t*   framerCfg);

/**
 * \brief Reads Configuration of JESD204B Deframer	 *
 *
 * This function reads the JESD204B deframer settings.
 * deframerCfg->bankId is returned zero, since Jrx doesn't support bank id.
 * deframerCfg->externalSysref is not used, will be returned 0.
 * deframerCfg->syncbOutCmosSlewRate is not used, will be returned 0.
 * deframerCfg->enableManualLaneXbar is not used, will be returned 0.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep{init->jesd204Settings}
 * \dep{init->tx}
 * \dep_end
 *
 * \param device Structure pointer to device settings data structure
 * \param deframerSel Desired deframer to read
 * \param deframerCfg Structure pointer to read deframer configuration
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_DeframerCfgGet(adi_adrv9025_Device_t*     device,
                                    adi_adrv9025_DeframerSel_e deframerSel,
                                    adi_adrv9025_DfrmCfg_t*    deframerCfg);

/**
 * \brief Enables/Disables the JESD204B/C Framer
 * \brief Not currently implemented
 * This function is normally not necessary.  In the event that the link needs to be reset, this
 * function allows a framer to be disabled and re-enabled, adi_adrv9025_FramerSel_e contains the links
 * which OR'd together and passed as the selFramerMask parameter.
 *
 * framerSelMask |  Description
 * --------------|-----------------------------------------------------------------------------
 *   bit    2    | Select Framer 2
 *   bit    1    | Select Framer 1
 *   bit    0    | Select Framer 0
 *
 * \pre This function may be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param framerSelMask Desired framer(s)  to set/reset.
 * \param enable 0 = Disable the selected framers, 1 = enable the selected framer link
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_FramerLinkStateSet(adi_adrv9025_Device_t* device,
                                        uint8_t                framerSelMask,
                                        uint8_t                enable);

/**
* \brief Get the  JESD204B/C Framer Link enable values
*
* This allows reading which framers are enabled/disabled.
*
* enable |  Description
* --------------|-----------------------------------------------------------------------------
*   bit    1    |  1 = Deframer 1 Enabled, 0 = Deframer 1 Disabled
*   bit    0    |  1 = Deframer 0 Enabled, 0 = Deframer 0 Disabled
*
* \pre This function may be called any time after device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device is a pointer to the device settings structure
* \param enable is a pointer to the value returned.  Valid values are 0 to 7.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_FramerLinkStateGet(adi_adrv9025_Device_t* device,
                                        uint8_t*               enable);

/**
 * \brief Enables/Disables the JESD204B/C Deframer
 *
 * This function is normally not necessary.  In the event that the link needs to be reset, this
 * function allows a deframer to be disabled and re-enabled.  During disable, the lane FIFOs
 * for the selected deframer are also disabled.  When the deframer link is enabled, the
 * lane FIFOs for the selected deframer are re-enabled (reset).  The BBIC should send valid
 * serializer data before enabling the link so the ADRV9025 CDR (recovered clock) is locked.
 * In the event that the deframer needs to be enabled or disabled, this
 * function allows a framer to be disabled and re-enabled, adi_adrv9025_DeframerSel_e contains the links
 * which OR'd together and passed as the selDeframerMask parameter.
 *
 * framerSelMask |  Description
 * --------------|-----------------------------------------------------------------------------
 *   bit    1    | Select Deframer 1
 *   bit    0    | Select Deframer 0
 *
 * \pre This function may be called any time after device initialization.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 device settings data structure
 * \param deframerSelMask Desired deframer to reset, deframer 1 or deframer 2
 * \param enable 0 = Disable the selected deframer, 1 = enable the selected deframer link
 *
  \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_DfrmLinkStateSet(adi_adrv9025_Device_t* device,
                                      uint8_t                deframerSelMask,
                                      uint8_t                enable);

/**
* \brief Get the  JESD204B/C Deframer Link enable values
*
* This allows reading which deframers are enabled/disabled.
*
* \pre This function may be called any time after device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device is a pointer to the device settings structure
* \param enable is a pointer to the value returned.  Valid values are 1 to 3.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DfrmLinkStateGet(adi_adrv9025_Device_t* device,
                                      uint8_t*               enable);

/**
 * \brief Enables or disables the external SYSREF JESD204b signal to the transceiver's framers
 *
 * For the framer to retime its LMFC (local multi frame clock), a SYSREF rising edge is required.
 * The external SYSREF signal at the pin can be gated off internally so the framer does not see
 * a potential invalid SYSREF pulse before it is configured correctly.
 *
 * By default ADRV9025 has the SYSREF signal ungated, however, the ADRV9025 Multichip Sync state machine
 * still does not allow the external SYSREF to reach the framer until the other stages of multichip
 * sync have completed.  As long as the external SYSREF is correctly configured before performing MCS,
 * this function may not be needed by the BBIC, since the MCS state machine gates the SYSREF to the
 * framer.
 *
 * \pre This function is called after the device has been initialized and the JESD204B framer is enabled
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param framerSelMask Select framer to enable/disable SYSREF input for (Valid Any ORed combination of enums ADI_ADRV9025_FRAMER_0, ADI_ADRV9025_FRAMER_1, ADI_ADRV9025_FRAMER_1 or ADI_ADRV9025_ALL_FRAMERS)
 * \param enable = '1' enables SYSREF to deframer, '0' disables SYSREF to deframer
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 *
 * TODO: Could pass structure instead of just enable, allow to set sysref options */
int32_t adi_adrv9025_FramerSysrefCtrlSet(adi_adrv9025_Device_t* device,
                                         uint8_t                framerSelMask,
                                         uint8_t                enable);

/**
* \brief Reads the bit indicating external SYSREF JESD204b signal of the transceiver's framers. Only status for one channel
* \ at a time can be read.
*
*
* \pre This function is called after the device has been initialized and the JESD204B framer is enabled
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device is a pointer to the device settings structure
* \param framerSel Select framer to get SYSREF input for (Valid ADI_ADRV9025_FRAMER_0, ADI_ADRV9025_FRAMER_1, ADI_ADRV9025_FRAMER_2)
* \param enable is a pointer to the value returned.  Valid values are 0 and 1.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_FramerSysrefCtrlGet(adi_adrv9025_Device_t*   device,
                                         adi_adrv9025_FramerSel_e framerSel,
                                         uint8_t*                 enable);

/**
 * \brief Enables or disables the external SYSREF JESD204b signal to the transceiver's deframers
 *
 * \pre This function is called after the device has been initialized and the JESD204B deframer is enabled
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param deframerSel Select deframer to enable/disable SYSREF input for (Valid ADI_ADRV9025_DEFRAMER_0, ADI_ADRV9025_DEFRAMER_1 or ADI_ADRV9025_DEFRAMER_0_AND_1)
 * \param enable = '1' enables SYSREF to deframer, '0' disables SYSREF to deframer
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 *
 * TODO: Could pass structure instead of just enable, allow to set sysref options */
int32_t adi_adrv9025_DeframerSysrefCtrlSet(adi_adrv9025_Device_t*     device,
                                           adi_adrv9025_DeframerSel_e deframerSel,
                                           uint8_t                    enable);

/**
* \brief Reads the bit indicating external SYSREF JESD204b signal of the transceiver's deframers. Only status for one channel
* \ at a time can be read.
*
*
* \pre This function is called after the device has been initialized and the JESD204B framer is enabled
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device is a pointer to the device settings structure
* \param deframerSel Select deframer to get SYSREF input for (Valid ADI_ADRV9025_DEFRAMER_0, ADI_ADRV9025_DEFRAMER_1)
* \param enable enable is a pointer to the value returned.  Valid values are 0 and 1.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DeframerSysrefCtrlGet(adi_adrv9025_Device_t*     device,
                                           adi_adrv9025_DeframerSel_e deframerSel,
                                           uint8_t*                   enable);

/**
* \brief Reads the ADRV9025 framer status for the framer selected by the framerSel input parameter
*
* \pre The Rx JESD204B/JESD204C link(s) needs to be configured and running to use this function
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
*   Status for JESD204B:
*  framerStatus->status | Description
*----------------------|-----------------------------------------------------------------------------
*                   [7] | Reserved
*                   [6] | Reserved
*                   [5] | Reserved
*                   [4] | Reserved
*                   [3] |Current SYNCBIN level(1 = high, 0 = low)
*                   [2] |SYSREF phase error - a new SYSREF had different timing than the first that set the LMFC timing.
*                   [1] |SYSREF phase established by framer
*                   [0] |Flag indicating that configuration parameters are not supported when set(1)
*
* framerSynbcNeCount :   8 bit counter of negative edge Sync transitions
*
*    qbfStateStatus     | Description
*-----------------------|-----------------------------------------------------------------------------
*               [7]     | Reserved
*               [6]     | Reserved
*               [5]     | Reserved
*               [4]     | Reserved
*               [3]     | Reserved
*               [2]     | QBF bit 2
*               [1]     | QBF bit 1
*               [0]     | QBF bit 0
*
*          syncNSel     | Description
*-----------------------|-----------------------------------------------------------------------------
*               [7]     | Reserved
*               [6]     | Reserved
*               [5]     | Reserved
*               [4]     | Reserved
*               [3]     | Reserved
*               [2]     | Syncb crossbar select
*               [1]     | Syncb crossbar select
*               [0]     | Syncb crossbar select
*
*   Status for JESD204C:
*  framerStatus->status | Description
*-----------------------|-----------------------------------------------------------------------------
*               [0]     | Sysref received bit
*
* \param device is a pointer to the device settings structure
* \param framerSel Read back the framer status of the selected framer (Framer0, Framer1  or Framer2)
* \param framerStatus is the RX framer status structure read
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_FramerStatusGet(adi_adrv9025_Device_t*       device,
                                     adi_adrv9025_FramerSel_e     framerSel,
                                     adi_adrv9025_FramerStatus_t* framerStatus);

/**
 * \brief Reads the status of the requested deframer
 *
 * \pre The Tx JESD204B/JESD204C link(s) needs to be configured and running to use this function
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 *   Status for JESD204B:
 *   deframerStatus  |  Bit Name                |  Description
 *   ----------------|--------------------------|--------------------------------------------------
 *              [7]  | Valid checksum           | 1 if the checksum calculated by the ADRV9025 matched the one sent in the ILAS data.
 *              [6]  | EOF Event                | This bit captures the internal status of the framer End of Frame event. Value =1 if framing error during ILAS
 *              [5]  | EOMF Event               | This bit captures the internal status of the framer End of Multi-Frame event. Value =1 if framing error during ILAS
 *              [4]  | FS Lost                  | This bit captures the internal status of the framer Frame Symbol event. Value =1 if framing error during ILAS or user data (invalid replacement characters)
 *              [3]  | Reserved                 | Reserved
 *              [2]  | User Data Valid          | =1 when in user data (deframer link is up and sending valid DAC data)
 *              [1]  | SYSREF Received          | Deframer has received the external SYSREF signal
 *              [0]  | Syncb level              | Current level of Syncb signal internal to deframer (=1 means link is up)
 *
 *   Status for JESD204C:
 *   deframerStatus  |  Bit Name                |  Description
 *   ----------------|--------------------------|--------------------------------------------------
 *            [2:0]  |  Current lock state      | These 3 bits indicate state of JESD204C Lock
 *
 *   An explanation of what each the value of the 3 bits relates to is below:
 *            value  |  Description
 *   ----------------|-----------------------------------------------------------------------------
 *              0    |  Reset
 *              1    |  Unlocked
 *              2    |  Block(Blocks aligned)
 *              3    |  M_Block(Lanes aligned)
 *              4    |  E_M_Block(Multiblock aligned)
 *              5    |  FEC_BUF
 *              6    |  FEC_READY (Good State!)
 *              7    |  Reserved
 *
 * \param device is a pointer to the device settings structure
 * \param deframerSel Select the deframer to read back the status of
 * \param deframerStatus 8bit deframer status word return value for each lane
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 *
 * TODO: Update to return a structure with more detailed info? */
int32_t adi_adrv9025_DeframerStatusGet(adi_adrv9025_Device_t*         device,
                                       adi_adrv9025_DeframerSel_e     deframerSel,
                                       adi_adrv9025_DeframerStatus_t* deframerStatus);

/****************************************************************************
 * Runtime functions
 ****************************************************************************
 */

/****************************************************************************
 * Helper functions
 ****************************************************************************
 */

/**
 * \brief Sets the DAC sample crossbar for the specified ADRV9025 deframer
 *
 * \pre This function is called after JESD204B initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to ADRV9025 device data structure containing settings
 * \param deframerSel adi_adrv9025_DeframerSel_e enumerated data type for DAC crossbar link selection
 * \param dacXbar Pointer to the adi_adrv9025_DacSampleXbarCfg_t enumerated data
 *                type used to map any deframer output to a specific DAC channel
 *                I or Q converter input for Tx
 *
 * Note: Outputs greater than the JESD204 M-1 value programmed at initialization for the selected deframer will be disabled.
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_DacSampleXbarSet(adi_adrv9025_Device_t*           device,
                                      adi_adrv9025_DeframerSel_e       deframerSel,
                                      adi_adrv9025_DacSampleXbarCfg_t* dacXbar);

/**
 * \brief Gets the DAC sample crossbar for the specified ADRV9025 deframer
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to ADRV9025 device data structure containing settings
 * \param deframerSel adi_adrv9025_DeframerSel_e enumerated data type for DAC crossbar link selection
 * \param dacXbar Pointer to the adi_adrv9025_DacSampleXbarCfg_t enumerated data
 *                        type used to map any deframer output to a specific DAC
 *                        channel I or Q converter input for Tx where read back
 *                        values are returned.
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_DacSampleXbarGet(adi_adrv9025_Device_t*           device,
                                      adi_adrv9025_DeframerSel_e       deframerSel,
                                      adi_adrv9025_DacSampleXbarCfg_t* dacXbar);

/**
 * \brief Sets the ADC sample crossbar to map Rx1/Rx2/Rx3/Rx4/ORx1/ORx2 'I/Q' data to
 * the chosen JESD204B framer's converter inputs. During standard operation, ADC crossbar
 * selection is being managed by streams. This function should be used for debug purposes.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to ADRV9025 device data structure containing settings
 * \param framerSel Selects the JESD204 framer to set the ADC crossbar for.
 *                  Only 1 framer may be selected at a time from the enum.
 *
 * \param adcXbar Pointer to structure containing ADC converter crossbar settings for the specified JESD204 framer.
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_AdcSampleXbarSet(adi_adrv9025_Device_t*           device,
                                      adi_adrv9025_FramerSel_e         framerSel,
                                      adi_adrv9025_AdcSampleXbarCfg_t* adcXbar);

/**
 * \brief Gets the ADC sample crossbar converter configuration map for
 * the chosen JESD204B framer converter
 *
 * \pre This function is called after JESD204B initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to ADRV9025 device data structure containing settings
 * \param framerSel Selects the JESD204 framer to get the ADC crossbar for.
 *                  Only 1 framer may be selected at a time from the enum.
 *
 * \param adcXbar Pointer to structure containing ADC converter crossbar settings for the specified JESD204 framer.
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_AdcSampleXbarGet(adi_adrv9025_Device_t*           device,
                                      adi_adrv9025_FramerSel_e         framerSel,
                                      adi_adrv9025_AdcSampleXbarCfg_t* adcXbar);

/****************************************************************************
 * Debug functions
 ****************************************************************************
 */

/**
 * \brief Selects the PRBS type and enables or disables RX Framer PRBS generation
 *
 * This is a debug function to be used for debug of the Rx JESD204B lanes.
 * Rx data transmission on the JESD204B link(s) is not possible
 * when the framer test data is activated.  To disable PRBS call this function
 * again with the framer data source set to FTD_ADC_DATA.
 *
 * \pre This function may be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param frmTestDataCfg is a pointer to a structure which contains the framer(s) of interest, testDataSource and injectPoint

 *
 *\retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_FramerTestDataSet(adi_adrv9025_Device_t*         device,
                                       adi_adrv9025_FrmTestDataCfg_t* frmTestDataCfg);

/**
* \brief Gets  the PRBS Framer Test Mode and Inject Points
*
* This is a debug function to be used for debug of the Rx JESD204B lanes.
* Rx data transmission on the JESD204B link(s) is not possible
* when the framer test data is activated.
*
* \pre This function may be called any time after device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device is a pointer to the device settings structure
* \param framerSelect contains the framer of interest
* \param frmTestDataCfg is a pointer to a structure which contains testDataSource and injectPoint
*
*\retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_FramerTestDataGet(adi_adrv9025_Device_t*         device,
                                       adi_adrv9025_FramerSel_e       framerSelect,
                                       adi_adrv9025_FrmTestDataCfg_t* frmTestDataCfg);

/**
 * \brief Injects an error into the Framer test data by inverting the data
 *
 * This is a debug function to be used for debug of the the Rx JESD204B lanes.
 * Rx data transmission on the JESD204B link(s) is not possible
 * when the framer test data is activated.
 *
 * \pre This function is called after the framer test data is enabled.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param framerSelect Select the desired framer ADI_ADRV9025_FRAMER_0, ADI_ADRV9025_FRAMER_1, ADI_ADRV9025_FRAMER_2
 * \param laneMask is an eight bit mask allowing selection of lanes 0-7 (at package balls) for the selected framer
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_FramerTestDataInjectError(adi_adrv9025_Device_t*   device,
                                               adi_adrv9025_FramerSel_e framerSelect,
                                               uint8_t                  laneMask);

/**
 * \brief Configures and enables or disables the transceiver's lane/sample PRBS
 *        checker.
 *
 * This is a debug function to be used for debug of the Tx JESD204B lanes.
 * The Tx link(s) need to be configured and on to use this function. If the
 * checkerLocation is TAL_PRBSCHECK_LANEDATA, the PRBS is checked at the
 * output of the deserializer. If the checkLocation is TAL_PRBSCHECK_SAMPLEDATA
 * the PRBS data is expected to be framed JESD204b data and the PRBS is checked
 * after the JESD204b data is deframed.  For the sample data, there is only
 * a PRBS checker on deframer output 0.  The lane PRBS has a checker on each
 * deserializer lane.
 *
 * \pre This function may be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param dfrmPrbsCfg Pointer to adi_adrv9025_DfrmPrbsCfg_t data structure.
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_DfrmPrbsCheckerStateSet(adi_adrv9025_Device_t*      device,
                                             adi_adrv9025_DfrmPrbsCfg_t* dfrmPrbsCfg);

int32_t adi_adrv9025_DfrmPrbsCheckerStateGet(adi_adrv9025_Device_t*      device,
                                             adi_adrv9025_DfrmPrbsCfg_t* dfrmPrbsCfg);

/**
* \brief Clears the deserializer lane and deframer sample PRBS error counters
*
* \pre The device must be initialized to a profile, so that the CLKPLL and
*      deserializer clock dividers are setup. If reading PRBS samples framed
*      in the JESD204 data, the JESD204 deframer must be configured.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device is a pointer to the device settings structure

* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DfrmPrbsCountReset(adi_adrv9025_Device_t* device);

/**
 * \brief Clears the deserializer lane and deframer sample PRBS error counters
 *
 * In the case that the PRBS checker is set to check at the deframer output
 * sample, there is only a checker on the Deframer Sample 0 output.  In this
 * case the lane function parameter is ignored and the sample 0 PRBS counter
 * is returned.
 *
 * \pre The device must be initialized to a profile, so that the CLKPLL and
 *      deserializer clock dividers are setup. If reading PRBS samples framed
 *      in the JESD204 data, the JESD204 deframer must be configured.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param counters  Pointer to PRBS Error counter structure to be returned
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_DfrmPrbsErrCountGet(adi_adrv9025_Device_t*              device,
                                         adi_adrv9025_DfrmPrbsErrCounters_t* counters);

/**
 * \brief Compares received Lane0 ILAS configuration to ADRV9025 deframer
 *        configuration and returns 32-bit mask indicating values that
 *        mismatched.  Actual lane0 ILAS configuration and deframer
 *        configuration values can be obtained by passing a pointer to a
 *        structure of type adi_adrv9025_DfrmCompareData_t.
 *
 * \pre The Rx JESD204B link(s) needs to be configured and running to use
 *      this function
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * mismatch Mask|  Description
 * -------------|------------------------------------------
 *         [14] | Lane0 Checksum, 0 = match, 1 = mismatch
 *         [13] | HD, 0 = match, 1 = mismatch
 *         [12] | CF, 0 = match, 1 = mismatch
 *         [11] | S, 0 = match, 1 = mismatch
 *         [10] | NP, 0 = match, 1 = mismatch
 *         [9]  | CS, 0 = match, 1 = mismatch
 *         [8]  | N, 0 = match, 1 = mismatch
 *         [7]  | M, 0 = match, 1 = mismatch
 *         [6]  | K, 0 = match, 1 = mismatch
 *         [5]  | F, 0 = match, 1 = mismatch
 *         [4]  | SCR, 0 = match, 1 = mismatch
 *         [3]  | L, 0 = match, 1 = mismatch
 *         [2]  | LID0, 0 = match, 1 = mismatch
 *         [1]  | BID, 0 = match, 1 = mismatch
 *         [0]  | DID, 0 = match, 1 = mismatch
 *
 * \param device Pointer to the device settings structure
 * \param deframerSelect Enum indicating which deframer to address.
 * \param dfrmData Pointer to a adi_adrv9025_DfrmCompareData_t structure that
 *                returns the deframer ILAS and configuration settings as well as the
 *                mismatch flag and zero data flag.  If the zero flag is set to zero,
 *                then no valid ILAS data found indicating link is not enabled.
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_DfrmIlasMismatchGet(adi_adrv9025_Device_t*          device,
                                         adi_adrv9025_DeframerSel_e      deframerSelect,
                                         adi_adrv9025_DfrmCompareData_t* dfrmData);

/**
 * \brief Reads the IRQ interrupt clear register of the specified deframer.
 * This function reads the contents of the deframer IRQ clear register.  Use this function whenever a general purpose (GP)
 * deframer IRQ asserts to find the maskable deframer IRQ sources.  Note: Deframer IRQ sources Elastic Buffer Error Flag,
 * Sysref Buffer Error, and the four Lane FIFO Async Error IRQ sources are always enabled and can not be masked in the
 * interrupt clear register.
 * \brief Not currently implemented
 *
 * \pre This function may be called any time after JESD204B link initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param deframerSelect selects the deframer to interrogate.
 * \param irqMask is a pointer to the bit mask value containing the contents of the deframer IRQ Clear Register
 *
 *                 Bit   |  Description
 *          -------------|-----------------------------------------------------------------------------
 *                  [15] | UnUsed
 *                  [14] | UnUsed
 *                  [13] | UnUsed
 *                  [12] | UnUsed
 *                  [11] | UnUsed
 *                  [10] | UnUsed
 *                  [9]  | UnUsed
 *                  [8]  | CMM - Configuration mismatch for lane0  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [7]  | BD - Bad Disparity error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [6]  | Not-In-Table error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [5]  | Unexpected K error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [4]  | ILD - Inter-lane De-skew  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [3]  | ILS - Initial lane synce  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [2]  | GCS - Good Check Sum  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [1]  | FS - Frame Sync 0 = No Interrupt, 1 = Interrupt Asserted
 *                  [0]  | CSG - Code Group Sync  0 = No Interrupt, 1 = Interrupt Asserted
 *
 *
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_DfrmIrqMaskGet(adi_adrv9025_Device_t*                  device,
                                    adi_adrv9025_DeframerSel_e              deframerSelect,
                                    adi_adrv9025_DeframerIrq_Mask_Vector_t* irqMask);

/**
 * \brief Writes the IRQ interrupt clear register of the specified deframer.
 * This function writes the specified IRQ mask value to the deframer IRQ clear register.  Use this function whenever a general purpose (GP)
 * deframer IRQ asserts to clear the pending maskable deframer IRQ or to enable/disable deframer interrupt sources. Note: Deframer IRQ sources
 * Elastic Buffer Error Flag, Sysref Buffer Error, and the four Lane FIFO Async Error IRQ sources are always enabled and can not be masked in the
 * interrupt clear register.  This function does not read-modify-write the interrupt clear register. To manually clear the interrupt, write
 * a one (set) to disable or mask the bit of the interrupt followed by writing a zero (clear) to enable the bit of the interrupt.  However, if the interrupt condition
 * still exists after setting the mask bit, the corresponding IRQ vector bit will re-assert.
 * \brief Not currently implemented
 *
 * \pre This function may be called any time after Jesd204B link initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param deframerSelect selects the deframer to interrogate.
 * \param irqMask is the bit mask value to be written to the deframer IRQ Clear Register (this is not a read-modify-write)
 *
 *                 Bit   |  Description
 *          -------------|-----------------------------------------------------------------------------
 *                  [15] | UnUsed
 *                  [14] | UnUsed
 *                  [13] | UnUsed
 *                  [12] | UnUsed
 *                  [11] | UnUsed
 *                  [10] | UnUsed
 *                  [9]  | UnUsed
 *                  [8]  | CMM - Configuration mismatch for lane0  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [7]  | BD - Bad Disparity error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [6]  | Not-In-Table error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [5]  | Unexpected K error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [4]  | ILD - Inter-lane De-skew  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [3]  | ILS - Initial lane synce  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [2]  | GCS - Good Check Sum  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [1]  | FS - Frame Sync 0 = No Interrupt, 1 = Interrupt Asserted
 *                  [0]  | CSG - Code Group Sync  0 = No Interrupt, 1 = Interrupt Asserted
 *
 *
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_DfrmIrqMaskSet(adi_adrv9025_Device_t*                 device,
                                    adi_adrv9025_DeframerSel_e             deframerSelect,
                                    adi_adrv9025_DeframerIrq_Mask_Vector_t irqMask);

/**
 * \brief Write the IRQ interrupt clear register for the specified deframer.
 * This function clears all deframer IRQ sources.  Use this function whenever a general purpose (GP)
 * deframer IRQ asserts to clear the pending deframer IRQ.
 * \brief Not currently implemented
 *
 * \pre This function may be called any time after JESD204 link initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param deframerSelect selects the deframer to interrogate.
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_DfrmIrqSourceReset(adi_adrv9025_Device_t*     device,
                                        adi_adrv9025_DeframerSel_e deframerSelect);

/**
 * \brief Read the IRQ interrupt source registers for the specified deframer.
 * This function fetches the contents of the deframer IRQ Vector register and other IRQ sources.  Use this function whenever
 * a general purpose (GP) deframer IRQ asserts to determine the source of the deframer IRQ.  Common IRQ sources are Bad Disparity (BD),
 * Not-In-Table (NIT), and Unexpected K-char (UEK) counters greater than the specified error threshold count value.
 * \brief Not currently implemented
 *
 * \pre This function may be called any time after JESD204 link initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param deframerSelect selects the deframer to interrogate.
 * \param irqSourceValue is a pointer to a bit mask containing the status of the IRQ Vector source register at read time.
 *
 *                 Bit   |  Description
 *          -------------|-----------------------------------------------------------------------------
 *                  [15] | UnUsed
 *                  [14] | UnUsed
 *                  [13] | UnUsed
 *                  [12] | UnUsed
 *                  [11] | UnUsed
 *                  [10] | UnUsed
 *                  [9]  | UnUsed
 *                  [8]  | CMM - Configuration mismatch for lane0  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [7]  | BD - Bad Disparity error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [6]  | Not-In-Table error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [5]  | Unexpected K error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [4]  | ILD - Inter-lane De-skew  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [3]  | ILS - Initial lane synce  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [2]  | GCS - Good Check Sum  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [1]  | FS - Frame Sync 0 = No Interrupt, 1 = Interrupt Asserted
 *                  [0]  | CSG - Code Group Sync  0 = No Interrupt, 1 = Interrupt Asserted
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_DfrmIrqSourceGet(adi_adrv9025_Device_t*                  device,
                                      adi_adrv9025_DeframerSel_e              deframerSelect,
                                      adi_adrv9025_DeframerIrq_Mask_Vector_t* irqSourceValue);

/**
* \brief Reset the serializer
*
*  Used in the initialization sequence by passing the serdes PLL frequency.  If a serdes PLL frequency is provided, it
*  will be greater than zero in the init structure member.  Otherwise, the init structure member will be initialized to
*  a value of zero causing the CLK PLL to be the selected.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device is a pointer to the device settings structure
* \param pllClkSource is flag indicating which pll is the serializer clock source: >0 SERDES PLL, 0 = Clocking PLL.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_SerializerReset(adi_adrv9025_Device_t* device,
                                     uint32_t               pllClkSource);

/**
* \brief Used to configure and reset the deframer error counters Bad Disparity Error (BD),
*        Not-In-Table (NIT), Unexpected K-char (UEK).
*
* This function allows individual counter enable/disable, reset, and configuration control
* for all lanes of a single deframer.
* This function will set the error counter interrupt bits at deframer interrupt request mask according to interruptEnable input
*
* \pre This function may be called any time after Jesd link initialization
*
* \dep_begin
* \dep{device->devHalInfo}
* \dep_end
*
* \param device is a pointer to the device settings structure
* \param deframerSelect selects the deframer of interest, DEFRAMER_0, DEFRAMER_1
* \param interruptEnable argument to enable/disable GPINT in case error counters overflow 255
* \param laneNumber Enum selecting the lane number to set config.
* \param errCounterControl 8-bit mask for enabling/disabling and resetting individual counters.
*
*     bit  |  Lane
*     -----|------------------
*       0  |  Bad Disparity Error 0 = disable, 1 = enable
*       1  |  Not-In-Table (NIT) 0 = disable, 1 = enable
*       2  |  Unexpected K-char (UEK) 0 = disable, 1 = enable
*       3  |  Reserved
*       4  |  Bad Disparity Error 0 = Do not reset counter, 1 = Reset counter to zero
*       5  |  Not-In-Table (NIT) 0 = Do not reset counter, 1 = Reset counter to zero
*       6  |  Unexpected K-char (UEK) 0 = Do not reset counter, 1 = Reset counter to zero
*       7  |  Reserved
*
* \param errCounterHoldCfg 8-bit mask for configuring the deframer error counters to stop counting
*        on overflow from 0xFF or to reset and continue counting.
*
*     bit  |  Lane
*     -----|------------------
*       0  |  Bad Disparity Error 0 = Reset and continue counting, 1 = Stop counting at 0xFF overflow
*       1  |  Not-In-Table (NIT) 0 = Reset and continue counting, 1 = Stop counting at 0xFF overflow
*       2  |  Unexpected K-char (UEK) 0 = Reset and continue counting, 1 = Stop counting at 0xFF overflow
*       3  |  Reserved
*       4  |  Reserved
*       5  |  Reserved
*       6  |  Reserved
*       7  |  Reserved
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DfrmErrCntrCntrlSet(adi_adrv9025_Device_t*              device,
                                         adi_adrv9025_DeframerSel_e          deframerSelect,
                                         adi_adrv9025_DfrmErrCounterIrqSel_e interruptEnable,
                                         uint8_t                             laneNumber,
                                         uint8_t                             errCounterControl,
                                         uint8_t                             errCounterHoldCfg);

/**
* \brief Used to read the configuration for the deframer error counters Bad Disparity Error (BD),
*        Not-In-Table (NIT), Unexpected K-char (UEK).
*
* \pre This function may be called any time after Jesd link initialization
*
* \dep_begin
* \dep{device->devHalInfo}
* \dep_end
*
* \param device is a pointer to the device settings structure
* \param deframerSelect selects the deframer of interest, DEFRAMER_A, DEFRAMER_B
* \param laneNumber Enum selecting the lane number to set config.
* \param errCounterControl Pointer to 8-bit variable containing individual counters enable bits.
*
*     bit  |  Lane
*     -----|------------------
*       0  |  Bad Disparity Error 0 = disable, 1 = enable
*       1  |  Not-In-Table (NIT) 0 = disable, 1 = enable
*       2  |  Unexpected K-char (UEK) 0 = disable, 1 = enable
*       3  |  Reserved
*       4  |  Reserved
*       5  |  Reserved
*       6  |  Reserved
*       7  |  Reserved
*
* \param errCounterHoldCfg Pointer to 8-bit variable containing configuration for deframer error counters to stop counting
*        on overflow from 0xFF or to reset and continue counting.
*
*     bit  |  Lane
*     -----|------------------
*       0  |  Bad Disparity Error 0 = Reset and continue counting, 1 = Stop counting at 0xFF overflow
*       1  |  Not-In-Table (NIT) 0 = Reset and continue counting, 1 = Stop counting at 0xFF overflow
*       2  |  Unexpected K-char (UEK) 0 = Reset and continue counting, 1 = Stop counting at 0xFF overflow
*       3  |  Reserved
*       4  |  Reserved
*       5  |  Reserved
*       6  |  Reserved
*       7  |  Reserved
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DfrmErrCntrCntrlGet(adi_adrv9025_Device_t*     device,
                                         adi_adrv9025_DeframerSel_e deframerSelect,
                                         uint8_t                    laneNumber,
                                         uint8_t*                   errCounterControl,
                                         uint8_t*                   errCounterHoldCfg);

/**
* \brief Used to read the deframer lane status and error counter values.
*
* This function returns the status and error counter values for the selected deframer
* and lane.  Returned counter values include UnExpected K-charactoer (UEK),
* Not-In-Table (NIT), Initial Lane Synchronization (ILS), Inter-Lane De-skew (ILD),
* Frame Sync (FS), Computed CheckSum (CCS), Code Group Sync (CGS), Bad Disparity (BD).
*
* Lane Status
*   Bit   |                     Description
*---------|-------------------------------------------------------------
*   [7]   | UEK Error, 0 = No Error, 1 = Error
*   [6]   | NIT Error, 0 = No Error, 1 = Error
*   [5]   | ILS Status, 0 = Not Completed, 1 = Completed Successfully
*   [4]   | ILD Status, 0 = Not Completed, 1 = Completed Successfully
*   [3]   | FS Status, 0 = Not Completed, 1 = Completed Successfully
*   [2]   | CCS Status, 0 = Not Completed, 1 = Completed Successfully
*   [1]   | CGS Status, 0 = Not Completed, 1 = Completed Successfully
*   [0]   | BD Error, 0 = No Error, 1 = Error
*
* \pre This function may be called any time after Jesd link initialization
*
* \dep_begin
* \dep{device->devHalInfo}
* \dep_end
*
* \param device is a pointer to the device settings structure
* \param deframerSelect selects the deframer of interest, DEFRAMER_A, DEFRAMER_B
* \param laneNumber selects the lane to read (values from 0 - 7)
* \param errCounterStatus structure for returning the lane status and error count values.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DfrmErrCounterStatusGet(adi_adrv9025_Device_t*               device,
                                             adi_adrv9025_DeframerSel_e           deframerSelect,
                                             uint8_t                              laneNumber,
                                             adi_adrv9025_DfrmErrCounterStatus_t* errCounterStatus);

/**
* \brief Used to get a deframer link condition up or down
*
* This function is called to get a deframer User Data Ready state bit.
*
* \dep_begin
* \dep{device->devHalInfo}
* \dep_end
*
* \param device is a pointer to the device settings structure
* \param deframerSelect Enum indicating which deframer to address.
* \param dfrmLinkCondition pointer to a 8 bit value
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DfrmLinkConditionGet(adi_adrv9025_Device_t*     device,
                                          adi_adrv9025_DeframerSel_e deframerSelect,
                                          uint8_t*                   dfrmLinkCondition);

/**
* \brief For the lane selected, runs a Static Phase Offset sweep to measure the horizontal eye opening and stores results in
*        the spoSweepResults struct
*
*        The function assumes valid PRBS test is running for the selected lane
*
* \dep_begin
* \dep{device->devHalInfo}
* \dep_end
*
* \param device is a pointer to the device settings structure
* \param laneNumber selects the lane to read
* \param spoSweepResults pointer to an empty structure that will hold the results of the sweep
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset.
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check.
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required.
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required.
*/
int32_t adi_adrv9025_DesEyeSweep(adi_adrv9025_Device_t*             device,
                                 uint8_t                            laneNumber,
                                 adi_adrv9025_DesEyeSweepResults_t* spoSweepResults);

/**
* \brief Enable/Disable the automatic underflow protection for the buffer.
*        By default the protection is enabled
*
* \pre This API must be called before the device is programmed
*
* \dep_begin
* \dep{device->devHalInfo}
* \dep_end
*
* \param device is a pointer to the device settings structure
* \param bufferProtectionSet Data to be configured, 0 = Disable, 1 = Enable
* \param deframerSelect selects the deframer of interest, DEFRAMER_A, DEFRAMER_B
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DfrmBufferProtectionSet(adi_adrv9025_Device_t     *device,
                                             uint8_t                    bufferProtectionSet,
                                             adi_adrv9025_DeframerSel_e deframerSelect);

/**
* \brief API will return the current state of the buffer protection status i.e. If buffer protection is enabled or disabled
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param deframerSelect Desired deframer to read
* \param bufferProtectionGet Pointer the will return the status. 0 = Disabled, 1 = Enabled
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset.
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check.
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required.
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required.
*
*/
int32_t adi_adrv9025_DfrmBufferProtectionGet(adi_adrv9025_Device_t     *device,
                                             adi_adrv9025_DeframerSel_e deframerSelect,
                                             uint8_t                   *bufferProtectionGet);

/**
* \brief This function will return the elastic FIFO depth
*
* \pre Buffer protection is enabled
* \pre Buffer protection is active
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param deframerSelect selects the deframer of interest, DEFRAMER_A, DEFRAMER_B
* \param channelId Register that will be read (0 = A, 1 = B, 2 = C, 3 = D)
* \param fifoDepth Pointer that will hold the elastic FIFO depth.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset.
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check.
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required.
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required.
*/
int32_t adi_adrv9025_DfrmFifoDepthGet(adi_adrv9025_Device_t     *device,
                                      adi_adrv9025_DeframerSel_e deframerSelect,
                                      uint8_t                    channelId,
                                      uint8_t                   *fifoDepth);

/**
* \brief Reads buffer protection active bit
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param deframerSelect selects the deframer of interest, DEFRAMER_A, DEFRAMER_B
* \param bufferProtectionStatus Pointer that will hold the current status of active bit.
*        0 = Buffer protection not active
*        1 = Buffer protection active. Buffer read and write pointers were too close with the chosen LMFC offset setting
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset.
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check.
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required.
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required.
*
*/
int32_t adi_adrv9025_DfrmBufferProtectionStatusGet(adi_adrv9025_Device_t     *device,
                                                   adi_adrv9025_DeframerSel_e deframerSelect,
                                                   uint8_t                   *bufferProtectionStatus);

/**
* \brief This API allows setting the LMFC offset value for the selected Deframer
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param deframerSelect selects the deframer of interest, DEFRAMER_0, DEFRAMER_1 or both
* \param lmfcOffset The LMFC offset value sets to the selected deframer, from 0-65535
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset.
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check.
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required.
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required.
*/
int32_t adi_adrv9025_DfrmLmfcOffsetSet(adi_adrv9025_Device_t     *device,
                                       adi_adrv9025_DeframerSel_e deframerSelect,
                                       uint16_t                   lmfcOffset);

/**
* \brief This API allows getting the LMFC offset value for the selected Deframer
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025s device data structure.
* \param deframerSelect selects the deframer of interest, DEFRAMER_0, DEFRAMER_1. DEFRAMER_0_AND_1 is invalid.
* \param lmfcOffset Pointer to LMFC offset value got from the selected deframer
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset.
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check.
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required.
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required.
*/
int32_t adi_adrv9025_DfrmLmfcOffsetGet(adi_adrv9025_Device_t *device,
                                       adi_adrv9025_DeframerSel_e deframerSelect,
                                       uint16_t                  *lmfcOffset);

/**
* \brief This function will return the phase difference from when the global LMFC is triggered
* to the point at which the received LMFC is flagged from the lane data.
*
* \pre Buffer protection is enabled
* \pre Buffer protection is active
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param deframerSelect selects the deframer of interest, DEFRAMER_A, DEFRAMER_B
* \param channelId Register that will be read (0 = A, 1 = B, 2 = C, 3 = D)
* \param phaseDiff Pointer that will hold the phase difference.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset.
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check.
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required.
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required.
*/
int32_t adi_adrv9025_DfrmPhaseDiffGet(adi_adrv9025_Device_t       *device,
                                        adi_adrv9025_DeframerSel_e deframerSelect,
                                        uint8_t                    channelId,
                                        uint8_t                   *phaseDiff);

/**
* \brief Generates eye diagram metrics for JESD profiles running in QR mode
*
* This function uses the ARM firmware to generate information about the signal eye related to a particular profile
* This method produces a multi level eye, this eye is generated using information based on intersymbol interference post cursors
* ADRV9025 uses a Baud Rate Phase detection method to determine signal eye layout this is to account for higher lane rates which are possible in madura
* This will produce eyediagram output which is reffered to as a 4 level eye.
*
* /pre Dependancies are as follows:
* - Device must be programmed prior to use
* - Requires the use of a profile that operates in Quarter Rate Mode (typically JESD204C Mode link speeds > 16Gbps)
*
* \param device is structure pointer to the ADRV9025 data structure containing settings
* \param lane is a variable which instructs the API with lane to test
* \param results an 2d array of adi_adrv9025_EyeDiagramResultsQR_t structures that will be populated with the results of the eye diagram for the given profile
*        results[0][x] will contain members - will contain to positive eye data
*        results[1][x] will contain members - will contain negative eye data
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_GenerateEyeDiagramQRMode(adi_adrv9025_Device_t               *device,
                                              uint8_t                             lane,
                                              adi_adrv9025_EyeDiagramResultsQR_t  **results);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_DATA_INTERFACE_H_ */
