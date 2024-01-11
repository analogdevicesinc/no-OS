/**
* Copyright 2015 - 2022 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_radioctrl.h
* \brief Contains ADRV904X function prototypes for
*    adi_adrv904x_radioctrl.c
*
* ADRV904X API Version: 2.10.0.4
*/


#ifndef _ADI_ADRV904X_RADIOCTRL_H_
#define _ADI_ADRV904X_RADIOCTRL_H_

#include "adi_adrv904x_radioctrl_types.h"
#include "adi_adrv904x_error.h"

/**
* \brief This function is used during the init sequence in order to load the stream
*        processor binary images from the stream_image.bin file. The stream_image.bin
*        file has a pre-defined structure this function relies on and so the user
*        must load the stream_image.bin starting from the beginning.
*        The user may load the file in chunks as low as 100 bytes by iterating through 
*        the entire stream_image.bin file and updating byteOffset on each iteration.
*        Once the final chunk of stream_image.bin has been processed the function
*        sets the appropriate status flag in device->devStateInfo.
*
* \pre This function is called after adi_adrv904x_Initialize before FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] byteOffset Offset (starting from 0) of where the current binary chunk is located
*                   in the stream_image.bin file.
* \param[in] binary Byte array containing the current chunk of stream binary file to process
* \param[in] byteCount The number of bytes in the binary parameter
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_StreamImageWrite(adi_adrv904x_Device_t* const    device,
                                                               uint32_t                        byteOffset,
                                                               const uint8_t                   binary[],
                                                               uint32_t                        byteCount);

/**
* \brief Enables or disables all Rx, ORx and Tx channels that are in SPI control mode (see
* adi_adrv904x_RadioCtrlCfgSet. Has no effect on channels that are not set to SPI control mode.
* Furthermore channels that are not initialized will not be enabled.
*
* For use cases where pin mode is not required, this function can be used to enable/disable the
* Rx/Orx/Tx signal paths. This function should be called after initialization and loading the
* stream processor.
* 
* orx/rx/tx/ChannelMask Parameters:
* The bits set in this bit-mask indicate the orx/rx/tx channels affected by this call respectively. 
* Channels whose bit are not set to 1 in this parameter will not be affected by this function call. 
* Whether an affected channel is to be enabled or disabled is indicated by the channel's bit in
* orx/rx/txChannelEnable parameters. The first channel is indicated by the least significant bit, 
* the second channel by the next least significant and so on. It is an error to set a bit corresponding
* to a non-existent channel.
*
* orx/rx/tx/ChannelEnable Parameters:
* A bit mask indicating if the channel is to be enabled or disabled. 
* Bits relate to channels in the same manner as the orx/rx/txChannelMask parameters.
* If the bit for a channel in the orx/rx/txChannelMask is not set then the channel's bit
* in these parameters has no effect.

* \pre This function should be called after initialization and loading the stream
* processor.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] orxChannelMask Mask to select Orx channels to be affected with this function call
* \param[in] orxChannelEnable Mask to enable/disable selected Orx channels
* \param[in] rxChannelMask Mask to select Rx channels to be affected with this function call
* \param[in] rxChannelEnable Mask to enable/disable selected Rx channels
* \param[in] txChannelMask Mask to select Tx channels to be affected with this function call
* \param[in] txChannelEnable  Mask to enable/disable selected Tx channels
*
* The following table lists the rxChannelMask bits and the corresponding channels
* that are enabled/disabled in SPI mode
*
*|orxChannelMask   | Channel Selected/Unselected              |
*|:----------------|:----------------------------------------:|
*|   bit[0-7]      |               Reserved                   |
*|   bit[8]        | 1 = ORx0 Selected, 0 = ORx0 Unselected   |
*|   bit[9]        | 1 = ORx1 Selected, 0 = ORx1 Unselected   |
*|   bit[10-31]    |               Reserved                   |
*
*|orxChannelEnable | Channel Enable/Disable                   |
*|:----------------|:----------------------------------------:|
*|   bit[0-7]      |               Reserved                   |
*|   bit[8]        | 1 = ORx0 Enable, 0 = ORx0 Disable        |
*|   bit[9]        | 1 = ORx1 Enable, 0 = ORx1 Disable        |
*|   bit[10-31]    |               Reserved                   |
*
*| rxChannelMask   | Channel Selected/Unselected              |
*|:----------------|:----------------------------------------:|
*|   bit[0]        | 1 = Rx0 Selected,  0 = Rx0 Unselected    |
*|   bit[1]        | 1 = Rx1 Selected,  0 = Rx1 Unselected    |
*|   bit[2]        | 1 = Rx2 Selected,  0 = Rx2 Unselected    |
*|   bit[3]        | 1 = Rx3 Selected,  0 = Rx3 Unselected    |
*|   bit[4]        | 1 = Rx4 Selected,  0 = Rx4 Unselected    |
*|   bit[5]        | 1 = Rx5 Selected,  0 = Rx5 Unselected    |
*|   bit[6]        | 1 = Rx6 Selected,  0 = Rx6 Unselected    |
*|   bit[7]        | 1 = Rx7 Selected,  0 = Rx7 Unselected    |
*|   bit[8-31]     |               Reserved                   |

*| rxChannelEnable | Channel Enable/Disable                   |
*|:----------------|:----------------------------------------:|
*|   bit[0]        | 1 = Rx0 Enable,    0 = Rx0 Disable       |
*|   bit[1]        | 1 = Rx1 Enable,    0 = Rx1 Disable       |
*|   bit[2]        | 1 = Rx2 Enable,    0 = Rx2 Disable       |
*|   bit[3]        | 1 = Rx3 Enable,    0 = Rx3 Disable       |
*|   bit[4]        | 1 = Rx4 Enable,    0 = Rx4 Disable       |
*|   bit[5]        | 1 = Rx5 Enable,    0 = Rx5 Disable       |
*|   bit[6]        | 1 = Rx6 Enable,    0 = Rx6 Disable       |
*|   bit[7]        | 1 = Rx7 Enable,    0 = Rx7 Disable       |
*|   bit[8-31]     |               Reserved                   |
*
*| txChannelMask   | Channel Selected/Unselected              |
*|:----------------|:----------------------------------------:|
*|   bit[0]        | 1 = Tx0 Selected,  0 = Tx0 Unselected    |
*|   bit[1]        | 1 = Tx1 Selected,  0 = Tx1 Unselected    |
*|   bit[2]        | 1 = Tx2 Selected,  0 = Tx2 Unselected    |
*|   bit[3]        | 1 = Tx3 Selected,  0 = Tx3 Unselected    |
*|   bit[4]        | 1 = Tx4 Selected,  0 = Tx4 Unselected    |
*|   bit[5]        | 1 = Tx5 Selected,  0 = Tx5 Unselected    |
*|   bit[6]        | 1 = Tx6 Selected,  0 = Tx6 Unselected    |
*|   bit[7]        | 1 = Tx7 Selected,  0 = Tx7 Unselected    |
*|   bit[8-31]     |               Reserved                   |

*| txChannelEnable | Channel Enable/Disable                   |
*|:----------------|:----------------------------------------:|
*|   bit[0]        | 1 = Tx0 Enable,    0 = Tx0 Disable       |
*|   bit[1]        | 1 = Tx1 Enable,    0 = Tx1 Disable       |
*|   bit[2]        | 1 = Tx2 Enable,    0 = Tx2 Disable       |
*|   bit[3]        | 1 = Tx3 Enable,    0 = Tx3 Disable       |
*|   bit[4]        | 1 = Tx4 Enable,    0 = Tx4 Disable       |
*|   bit[5]        | 1 = Tx5 Enable,    0 = Tx5 Disable       |
*|   bit[6]        | 1 = Tx6 Enable,    0 = Tx6 Disable       |
*|   bit[7]        | 1 = Tx7 Enable,    0 = Tx7 Disable       |
*|   bit[8-31]     |               Reserved                   |
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxTxEnableSet(adi_adrv904x_Device_t* const   device,
                                                            const uint32_t                 orxChannelMask,
                                                            const uint32_t                 orxChannelEnable,
                                                            const uint32_t                 rxChannelMask,
                                                            const uint32_t                 rxChannelEnable,
                                                            const uint32_t                 txChannelMask,
                                                            const uint32_t                 txChannelEnable);
/**
 * \brief Retrieve the Tx, Rx, and ORx SPI mode bit masks enabled/disabled status.
 *
 * This function retrieves SPI mode enabling status of  Rx/Tx/ORx. 
 *
 * Please see adi_adrv904x_RxTxEnableSet() for bit-to-channel assignments of
 * orxChannelMask/rxChannelMask/txChannelMask parameters.
 * \pre This function may be called after the device has been fully initialized
 *      any time during run-time operation, but only after the CPU and GPIO
 *      have been configured
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X data structure
 * \param[out] orxChannelMask Pointer to location of bit mask into which ORx SPI-mode enabled/disabled status will be written.
 * \param[out] rxChannelMask Pointer to location of bit mask into which Rx SPI-mode enabled/disabled status will be written.
 * \param[out] txChannelMask Pointer to location of bit mask into which Tx SPI-mode enabled/disabled status will be written.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxTxEnableGet(adi_adrv904x_Device_t* const    device,
                                                            uint32_t* const                 orxChannelMask,
                                                            uint32_t* const                 rxChannelMask,
                                                            uint32_t* const                 txChannelMask);

/**
 * \brief Retrieves the current effective channel enabled/disabled state regardless of whether the channel is
 * under SPI-mode or pin-mode control.
 *
 * The state of Rx/Tx/ORx channel 0 is written to the least significant bit in the corresponding argument. The
 * state of channel 1 to the next most significant bit and so on. A bit value of 1 indicates the channel is
 * enabled. Bits outside the range of the number of existing channels are set to 0.
 *
 * Compare to adi_adrv904x_RxTxEnableGet which retrieve the Tx, Rx, and ORx SPI mode bit masks enabled/disabled
 * status.
 *
 * \pre This function may be called after the device has been fully initialized
 *      any time during run-time operation, but only after the CPU and GPIO
 *      have been configured
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X data structure
 * \param[out] orxChannelMask the current state of the ORx channels
 * \param[out] rxChannelMask the current state of the Rx channels.
 * \param[out] txChannelMask the current state of the Tx channels.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ChannelEnableGet(adi_adrv904x_Device_t* const    device,
                                                               uint32_t* const                 orxChannelMask,
                                                               uint32_t* const                 rxChannelMask,
                                                               uint32_t* const                 txChannelMask);

/**
 * \brief This function sets up the mode (Pin Mode/Non-Pin Mode) through which the signal path is controlled
 *
 *  The ADRV904X device defaults to Non-pin mode on power up. This function is used to reconfigure
 *  the signal path control mode of Rx, ORx, Tx signal chains. Non-Pin mode signal chain control
 *  can be accomplished through adi_adrv904x_RxTxEnableSet() API. The pin mode signal chain control is
 *  accomplished through input pins to ADRV904X device.
 *
 *  In Pin mode, the Tx and Rx signal chains are controlled using dedicated pins TRXA_CTRL,
 *  TRXB_CTRL, ... TRXH_CTRL. Use adi_adrv904x_RadioCtrlTxRxEnCfgSet to configure which pins control which
 *  channels.
 *  
 * \pre This function may be called after the device has been fully initialized
 *       any time during run-time operation, but only after the CPU and GPIO
 *       have been configured
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X data structure
 * \param[in] radioCtrlCfg is comprised of radio control mode configurations for Rx, ORx and Tx channels
 *
 *  In order to skip configuration for a specific signal chain, set the respective enable mode to invalid. Eg: To skip
 *  Tx configuration, set radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode to ADI_ADRV904X_TX_EN_INVALID_MODE. Similarly
 *  to skip Rx configuration set radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode to ADI_ADRV904X_RX_EN_INVALID_MODE and
 *  to skip ORx configuration set radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode to ADI_ADRV904X_ORX_EN_INVALID_MODE.
 *
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlCfgSet(adi_adrv904x_Device_t* const            device,
                                                              adi_adrv904x_RadioCtrlModeCfg_t* const  radioCtrlCfg);

/**
 * \brief This function controls enablement of Tx & Rx channels and antenna by the eight TRX_CTRL pins: TRXA_CTRL -
 * TRXH_CTRL. configurations are only valid for channels in pin-mode; for channels in SPI-mode they are ignored.
 * 
 * The pinIndex and configSel parameters select the desired pin and config member to write. If a field within the 
 * adi_adrv904x_RadioCtrlTxRxEnCfg_t is not indicated by a bit in configSel parameter that field ignored. Any
 * TRX_CTRL pin that is not indicated by a bit in the pinIndex also remains unaffected by the call.
 *
 * The txRxEnCfg parameter holds an array for Tx/Rx channel enablement and Tx/Rx antenna enablement.
 * Each array has eight bytes corresponding to the eight TRX_CTRL pins. Element 0 of each array corresponds to
 * TRXA_CTRL, element 1 corresponds to TRXB_CTRL pin. Within each byte bitN corresponds to channel N where bit0
 * is the least-significant bit.
 *
 *  Example:
 *  txRxEnCfg->txEnMapping[0] = 0x0F --> then asserting TRXA_CTRL Pin will enable Tx Channels 0-3.
 *  NOTE: the configSel param must also have the TX_ENABLE bit set and the pinSel param must have PIN0 bit 
 *        set for TRX*A* set.
 *  
 *  txRxEnCfg->rxAltMapping[3] = 0xF0 --> then asserting TRXD_CTRL Pin will power up the data path for antenna
 *  calibrations for Rx Channels 4-7
 *  NOTE: the configSel param must also have the RX_ALT_ENABLE bit set and the pinSel param must have bit3 for TRX*D* set.
 *
 *  Each pin can be used to control any number of Tx or Rx channels. A single channel can also
 *  be controlled by multiple pins. Where a single channel is configured to be controlled by multiple pins the channel
 *  is enable is any one of the pins is asserted.
 *  
 *  This function is used during the Post-MCS bring up phase. It can also be called during runtime for calibration
 *  control.
 *  
 * \pre This function may be called after the device has been fully initialized
 *       any time during run-time operation, but only after the CPU and GPIO
 *       have been configured
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X data structure
 * \param[in] txRxEnCfg is comprised of radio control TXRX Pin configurations for Rx and Tx channels
 * \param[in] pinIndex is a bitmask built up of or'd adi_adrv904x_TxRxEnPin_e values
 * \param[in] configSel is a bitmask built up of or'd adi_adrv904x_TxRxEnCfg_e values
 * 
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlTxRxEnCfgSet(adi_adrv904x_Device_t* const                    device,
                                                                    const adi_adrv904x_RadioCtrlTxRxEnCfg_t* const  txRxEnCfg,
                                                                    uint8_t                                         pinIndex,
                                                                    uint8_t                                         configSel);

/**
 * \brief This function reads the Tx & Rx GPIO configuration bitfields and stores the values in the blank
 *        structure passed in.
 *  
 * \pre This function may be called after the device has been fully initialized
 *       any time during run-time operation, but only after the CPU and GPIO
 *       have been configured
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X data structure
 * \param[out] txRxEnCfg blank structure to hold the current TXRX Pin settings
 * 
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlTxRxEnCfgGet(adi_adrv904x_Device_t* const              device,
                                                                    adi_adrv904x_RadioCtrlTxRxEnCfg_t* const  txRxEnCfg);

/**
 * \brief Reads back the Tx, Rx, and ORx radio control mode configuration
 *
 * This function retrieves Pin mode / Non pin mode configuration of 
 * radio ctrl modes for Rx/Tx/ORx signal chains. For Rx, Tx, and ORx signal chains,
 * a SPI or Pin mode is returned. 
 *
 * \pre This function may be called after the device has been fully initialized
 *      any time during run-time operation, but only after the CPU and GPIO
 *      have been configured
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X data structure
 * \param[in] rxChannel Receiver channel for which mode config readback is requested (valid channels Rx0-Rx7, ORx0-ORx1)
 * \param[in] txChannel Transmitter channel for which mode config readback is requested (valid channels Tx0-Tx7)
 * \param[out] radioCtrlCfg Pointer to radio ctrl config data structure which will be updated with read back 
 *        values for Rx,Tx and ORx signal chain control configs.
 *
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlCfgGet(adi_adrv904x_Device_t* const            device,
                                                              const adi_adrv904x_RxChannels_e         rxChannel,
                                                              const adi_adrv904x_TxChannels_e         txChannel,
                                                              adi_adrv904x_RadioCtrlModeCfg_t* const  radioCtrlCfg);

/**
* \brief Sets the RF LO settings for the desired LO
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] loConfig Pointer to the LO config settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_LoFrequencySet(adi_adrv904x_Device_t* const device,
                                                             const adi_adrv904x_LoConfig_t* const loConfig);


/**
* \brief Gets a Local Oscillator (LO) frequency (RF carrier frequency).
*
* This function is used to get a LO current frequency. The loName field in adi_adrv904x_LoConfigReadback_t needs to be filled in with desired
* enumerate value LO0, LO1 in adi_adrv904x_LoName_e.
*
* This function reads back the maximum LO frequency(of all channels using that LO) after leaf dividers.
* \pre This function can be used after the device has been initialized, firmware loaded and the LO configured.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in,out] loConfig Pointer to the LO config read back settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_LoFrequencyGet(adi_adrv904x_Device_t* const            device,
                                                             adi_adrv904x_LoConfigReadback_t* const  loConfig);

/**
* \brief API to assign Tx and Rx Channels to PLL's during runtime. 
*  This API is called to configure PLL assigments for Tx and Rx channels.  
*  
* \pre This API function can be called after the CPU has been initialized and MCS done. 
* \post After the API function is called, strictly need to re-program the PLL's.   
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] rf0MuxTx0_3  If set to 1 East Tx channels (Tx0-3) are connected to PLL0 (East PLL). If set to 0 Tx0-3 are connected to PLL1 (West PLL). 
* \param[in] rf0MuxTx4_7  If set to 1 West Tx channels (Tx4-7) are connected to PLL0 (East PLL). If set to 0 Tx4-7 are connected to PLL1 (West PLL). 
* \param[in] rf0MuxRx0_3  If set to 1 East Rx channels (Rx0-3) are connected to PLL0 (East PLL). If set to 0 Rx0-3 are connected to PLL1 (West PLL). 
* \param[in] rf0MuxRx4_7  If set to 1 West Rx channels (Rx4-7) are connected to PLL0 (East PLL). If set to 0 Rx4-7 are connected to PLL1 (West PLL). 
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfgPllToChanCtrl(adi_adrv904x_Device_t* const device,
	                                                           uint8_t rf0MuxTx0_3, 
	                                                           uint8_t rf0MuxTx4_7, 
	                                                           uint8_t rf0MuxRx0_3, 
	                                                           uint8_t rf0MuxRx4_7);

/**
* \brief Sets the RF LO Loop filter parameters for the desired LO.
* This command only updates the loop config parameters in the internal table.
* adi_adrv904x_LoFrequencySet function needs to be called to make the new loop
* configuration effective.
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] loName The RF LO selected
* \param[in] loLoopFilterConfig Pointer to the LO config settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_LoLoopFilterSet(adi_adrv904x_Device_t* const device,
                                                              const adi_adrv904x_LoName_e loName,
                                                              const adi_adrv904x_LoLoopFilterCfg_t* const loLoopFilterConfig);

/**
* \brief Gets the RF LO Loop filter parameters for the desired LO
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] loName The RF LO selected
* \param[in,out] loLoopFilterConfig Pointer to the LO config settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_LoLoopFilterGet(adi_adrv904x_Device_t* const device,
                                                              const adi_adrv904x_LoName_e loName,
                                                              adi_adrv904x_LoLoopFilterCfg_t* const loLoopFilterConfig);

/**
* \brief Gets Local Oscillators (LO) lock status.
*
* This function is used to get all plls lock status. 
*
* pllLockStatus bit 0 = CLK PLL Lock status
* pllLockStatus bit 1 = RF0 PLL Lock status
* pllLockStatus bit 2 = RF1 PLL status
* pllLockStatus bit 3 = Serdes PLL Lock Status
*
* A bit value of 1 indicates the corresponding pll is locked
* A bit value of 0 indicates the corresponding pll is unlocked.
*
* \pre This function can be used after the device has been initialized, firmware loaded and the LO configured.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] pllLockStatus Pointer to the pll lock status read back 
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_PllStatusGet(adi_adrv904x_Device_t* const device,
                                                           uint32_t* const        pllLockStatus);

/**
* \brief Gets all TX and RX LO names and frequencies.
*
* \pre This function can be used after the device has been initialized, firmware loaded and the LO configured.
* It returns the LO frequencies of the RF channels after leaf dividers.
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] rxTxLoFreq Pointer to the structure contain all RX TX LO names and frequencies.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxTxLoFreqGet(adi_adrv904x_Device_t* const            device,
                                                            adi_adrv904x_RxTxLoFreqReadback_t* const  rxTxLoFreq);

/**
* \brief Get temperature information for the device
*
* This function is used to get temperature information for the device.
*
* \pre This function can be used after the device has been initialized and firmware loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] avgMask Bitmask of adi_adrv904x_DevTempSensorMask_e values indicating which temperature sensor
*                    readings should be averaged in the tempDegreesCelsiusAvg member of the deviceTemperature structure.
*                    If 0 is provided, -274 will be returned for the averaged value.
* \param[in,out] deviceTemperature Pointer to structure in which device temperature data should be placed
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TemperatureGet(adi_adrv904x_Device_t* const        device,
                                                             const uint16_t                      avgMask,
                                                             adi_adrv904x_DevTempData_t* const   deviceTemperature);

/**
* \brief Get enabled temperature sensor information for the device
*
* This function is used to get enabled temperature sensor information for the device.
*
* \pre This function can be used after the device has been initialized and firmware loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out]    tempEnData Pointer to uint16 in which device enabled temperature sensor data should be placed
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TemperatureEnableGet(adi_adrv904x_Device_t* const        device,
                                                                   uint16_t* const                     tempEnData);


/**
* \brief Set enabled temperature sensor information for the device
*
* This function is used to set enabled temperature sensor information for the device.
*
* \pre This function can be used after the device has been initialized and firmware loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in,out] tempEnData Pointer to uint16 in which device enabled temperature sensor data are set to
*                in the event a temp sensor is unavailable (i.e. LDO is off for a particular PLL temp sensor)
*                the returned value contains the sensors that are enabled.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TemperatureEnableSet(adi_adrv904x_Device_t* const        device,
                                                                   uint16_t* const                     tempEnData);


/**
* \brief This function associates a GPIO pin with stream processor GP inputs and enables stream trigger 
*        functionality if a valid GPIO(GPIO0-GPIO23) is assigned to the streamGpInput pins.
*
* There are 24 GPIO inputs available to trigger streams. These GPIO pins can 
* be mapped to one of GPIOs[0:23]. 
* GPIO input 0 must map to GPIO 0, GPIO input 1 must map to GPIO 1,,, etc....
*
* To unmap a GPIO association with a stream GP input, please set the GPIO input
* to ADI_ADRV904X_GPIO_INVALID.
*
* \pre This function requires CPU and Stream binaries to be loaded
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in]     streamGpioPinCfg Pointer to the structure holding GPIO pin to stream GP input mappings (Input)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_StreamGpioConfigSet(adi_adrv904x_Device_t* const          device,
                                                                  const adi_adrv904x_StreamGpioPinCfg_t* const streamGpioPinCfg);

/**
* \brief This function retrieves the current Tx-ORx mapping control GPIO pins
*
* This function retrieves the GPIOs assigned for Tx-ORx mapping stream triggering
*
* \pre This function can be called anytime after ADRV904X device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out]    streamGpioPinCfg Pointer to the structure which will be updated with the 
*                         active GPIO pin to stream GP input mappings (Output)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_StreamGpioConfigGet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_StreamGpioPinCfg_t* const streamGpioPinCfg);

/**
* \brief This function calculates the ORx NCO frequency shifts, given the Tx Synthesis Bandwidth limits
*
* \pre This function can be called anytime after ADRV904X device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in]     orxChannel                The ORx channel for which the NCO frequency shifts are to be calculated
* \param[in]     txSynthesisBwLower_kHz    Lower limit of TX Synthesis Bandwidth
* \param[in]     txSynthesisBwUpper_kHz    Upper limit of TX Synthesis Bandwidth
* \param[out]    ncoShiftFreqAdc_kHz       Computed ORx ADC NCO frequency shift
* \param[out]    ncoShiftFreqDatapath_kHz  Computed ORx Datapath NCO frequency shift
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_OrxNcoFreqCalculate(adi_adrv904x_Device_t* const    device,
                                                                  const adi_adrv904x_RxChannels_e orxChannel,
                                                                  const uint32_t                  txSynthesisBwLower_kHz,
                                                                  const uint32_t                  txSynthesisBwUpper_kHz,
                                                                  int32_t* const                  ncoShiftFreqAdc_kHz,
                                                                  int32_t* const                  ncoShiftFreqDatapath_kHz);

/**
* \brief Configures and allocates selected GPIO pins that will trigger a Stream that handle 
*           Tx to Orx Mapping changes at runtime.
* 
* This function will handle GPIO setup (if appropriate) and store the selected feature mode 
* in the device data structure for use at runtime.
*
* \pre This function should not be called outside the initialization process of adi_adrv904x_PostMcsInit and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingInit(adi_adrv904x_Device_t* const  device);

/**
* \brief Gets the Tx to ORx Mapping Configuration setup for the device.
*
* This function retrieves from device handle Tx to ORx mapping configuration setup during initialization.
*
* \pre This function may be called after device Post Msc Initialization.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] mappingConfig The Tx to ORx mapping configuration that was applied to the device during PostMcsInit
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingConfigGet(adi_adrv904x_Device_t* const                device,
                                                                      adi_adrv904x_TxToOrxMappingConfig_t * const mappingConfig);
/**
* \brief Sets the Tx to ORx external signal routing for Tx calibrations that use the ORx for observation.
*
* This function invokes the streams required to setup a Tx to ORx mapping configuration.
*
* \pre This function may be called after the Stream processor initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] mapping Tx to ORx mapping byte to apply. Corresponds to Tx to ORx Mapping Mode and 
*               Tx Observability Word selected during Initialization.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingSet(adi_adrv904x_Device_t* const    device,
                                                                const uint8_t                   mapping);

/**
* \brief Gets the Tx to ORx external signal routing for Tx calibrations that use the ORx for observation.
*
* This function retrieves from hardware the Tx channel currently mapped to the requested ORx channel
*
* \pre This function may be called after the Stream processors have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] orxChannel The ORx channel for which Tx channel mapping is requested
* \param[out] txChannel The Tx channel map to the ORx channel
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingGet(adi_adrv904x_Device_t* const        device,
                                                                const adi_adrv904x_RxChannels_e     orxChannel,
                                                                adi_adrv904x_TxChannels_e* const    txChannel);

/**
* \brief Updates ORx preset Atten value for selected Tx Channel(s) to be used when mapped to an ORx Channel. 
* 
* User also has the option to update active ORx atten, if appropriate, using immediateUpdate = 1. 
* If immediate update is not desired, use immediateUpdate = 0.
*
* \pre This function may be called after device initialization is complete.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] mapping mapping for which to populate preset data to be used by Tx to ORx Mapping feature.
*               (!TX_TO_ORX_EXTENDED_MAPPING_FLAG | mapping): Provide a txChannel Mask. See adi_adrv904x_TxChannels_e
*                (TX_TO_ORX_EXTENDED_MAPPING_FLAG | mapping): Provide a State Mapping. See State Mappings in adi_adrv904x_TxToOrxMappingPinTable_e
* \param[in] presetAtten_dB Preset value for ORx Atten that will be automatically applied when one 
*               of the selected Tx Channel(s) is mapped to an ORx channel.
* \param[in] immediateUpdate Option to immediately apply the updated Tx Channel presets to any ORx Channel it is currently mapped to
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingPresetAttenSet( adi_adrv904x_Device_t* const    device,
                                                                            const uint32_t                  mapping,
                                                                            const uint8_t                   presetAtten_dB,
                                                                            const uint8_t                   immediateUpdate);

/**
* \brief Retrieves ORx preset Atten value for selected Mapping to be used when mapped to an ORx Channel.
*
* \pre This function may be called after device initialization is complete.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] mapping Tx to Orx Mapping for which to retrieve preset data to be used by Tx to ORx Mapping feature.
* \param[out] presetAtten_dB Retrieved preset value for ORx Atten that will be automatically applied when the
*               selected Tx Channel is mapped to an ORx channel.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingPresetAttenGet_v2(  adi_adrv904x_Device_t* const        device,
                                                                                const adi_adrv904x_TxToOrxMappingPinTable_e mapping,
                                                                                uint8_t* const                      presetAtten_dB);

/**
* \brief Retrieves ORx preset Atten value for selected Tx Channel to be used when mapped to an ORx Channel.
*
* \pre This function may be called after device initialization is complete.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] txChannel Tx channel for which to retrieve preset data to be used by Tx to ORx Mapping feature.
*               Valid channels include adi_adrv904x_TxChannels_TX[0-7]
* \param[out] presetAtten_dB Retrieved preset value for ORx Atten that will be automatically applied when the
*               selected Tx Channel is mapped to an ORx channel.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingPresetAttenGet( adi_adrv904x_Device_t* const    device,
                                                                            const adi_adrv904x_TxChannels_e txChannel,
                                                                            uint8_t* const                  presetAtten_dB);

/**
* \brief Updates ORx preset NCO values for selected Tx Channel(s) to be used when mapped to an ORx Channel. 
* 
* The preset NCO setting is comprised of an ORx ADC NCO Freq and ORx Datapath NCO Freq. User also has the 
* option to update active ORx NCO, if appropriate, using immediateUpdate = 1. If immediate update is not 
* desired, use immediateUpdate = 0.
*
* \pre This function may be called after device initialization is complete.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] mapping mapping for which to populate preset data to be used by Tx to ORx Mapping feature.
*               (!TX_TO_ORX_EXTENDED_MAPPING_FLAG | mapping): Provide a txChannel Mask. See adi_adrv904x_TxChannels_e
*                (TX_TO_ORX_EXTENDED_MAPPING_FLAG | mapping): Provide a State Mapping. See State Mappings in adi_adrv904x_TxToOrxMappingPinTable_e
* \param[in] presetNco Configuration of preset values for ORx NCO Frequencies that will be automatically applied 
*               when one of the selected Tx Channel(s) is mapped to an ORx channel.
* \param[in] immediateUpdate Option to immediately apply the updated Tx Channel presets to any ORx Channel it is currently mapped to
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingPresetNcoSet(   adi_adrv904x_Device_t* const                        device,
                                                                            const uint32_t                                      mapping,
                                                                            const adi_adrv904x_TxToOrxMappingPresetNco_t* const presetNco,
                                                                            const uint8_t                                       immediateUpdate);

/**
* \brief Retrieves ORx preset NCO values for selected Mapping to be used when mapped to an ORx Channel. 
* 
* The preset config is comprised of ORx ADC NCO Freq and ORx Datapath NCO Freq. 
*
* \pre This function may be called after device initialization is complete.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] mapping Tx to Orx Mapping for which to retrieve preset data to be used by Tx to ORx Mapping feature.
* \param[out] presetNco Retrieved configuration of preset values for ORx NCO Frequencies that will be 
*               automatically applied when the selected Tx Channel is mapped to an ORx channel.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingPresetNcoGet_v2(adi_adrv904x_Device_t* const                    device,
                                                                            const adi_adrv904x_TxToOrxMappingPinTable_e     mapping,
                                                                            adi_adrv904x_TxToOrxMappingPresetNco_t* const   presetNco);

/**
* \brief Retrieves ORx preset NCO values for selected Tx Channel to be used when mapped to an ORx Channel. 
* 
* The preset config is comprised of ORx ADC NCO Freq and ORx Datapath NCO Freq. 
*
* \pre This function may be called after device initialization is complete.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] txChannel Tx channel for which to retrieve preset data to be used by Tx to ORx Mapping feature.
*               Valid channels include adi_adrv904x_TxChannels_TX[0-7]
* \param[out] presetNco Retrieved configuration of preset values for ORx NCO Frequencies that will be 
*               automatically applied when the selected Tx Channel is mapped to an ORx channel.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingPresetNcoGet(   adi_adrv904x_Device_t* const                    device,
                                                                            const adi_adrv904x_TxChannels_e                 txChannel,
                                                                            adi_adrv904x_TxToOrxMappingPresetNco_t* const   presetNco);

/**
* \brief Reads back the version of the Stream binary loaded into the CPU memory
*
* This function reads the CPU memory to read back the major.minor.maint.build
* version for the Stream binary loaded into CPU memory.
*
* \pre This function may be called after the Stream processors have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] streamVersion Pointer to four parts of StreamVersion
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_StreamVersionGet(adi_adrv904x_Device_t* const           device,
                                                               adi_adrv904x_Version_t* const          streamVersion);

/**
* \brief Configures the preset gain/nco variables for antenna cal
*
* Function to store predefined Antenna Cal configuration. Values in this configuration
* that correspond to user-enabled Antenna Cal options in the Stream Generator will be
* forced when Antenna Cal mode is enabled for a given Rx or Tx Channel. Configurations
* can be unique across channels. Configuration will not be forced immediately; it will
* be forced when Antenna On streams are triggered for a given Rx or Tx Channel by a
* TRX_CTRL pin. To change Tx attenuation, stream processor will use S0 index. User is
* responsible to configure the device to use S0 index(attenuation update trigger disabled)
* before using antenna cal feature.
*
* \pre This function may be called after the Stream processors have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] configRx Configuration structure for Rx channel
* \param[in] configTx Configuration structure for Tx channel
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalConfigSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_RxRadioCtrlAntennaCalConfig_t * const configRx,
                                                                       adi_adrv904x_TxRadioCtrlAntennaCalConfig_t * const configTx);

/**
* \brief Reads back the antenna calibration configuration for selected channel
*
*
* \pre This function may be called after the Stream processors have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in,out] configRx Pointer to configuration structure to be populated with readback values
* \param[in,out] configTx Pointer to configuration structure to be populated with readback values
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalConfigGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_RxRadioCtrlAntennaCalConfig_t * const configRx,
                                                                       adi_adrv904x_TxRadioCtrlAntennaCalConfig_t * const configTx);

/**
* \brief Reads back the antenna calibration error status
*
*
* \pre errStatus fields:  0b: Tx Low Stream Error  (Tx antenna cal pin de-asserted when Tx channel is enabled)
*                         1b: Tx High Stream Error (Tx antenna cal pin asserted when Tx channel is enabled)
*                         2b: Tx Low Stream Error  (Tx Low stream is triggered when antenna cal is ongoing for this channel)
*                         3b: Tx High Stream Error (Tx High stream is triggered when antenna cal is ongoing for this channel)
*                         4b: Rx Low Stream Error  (Rx antenna cal pin de-asserted when Rx channel is enabled)
*                         5b: Rx High Stream Error (Rx antenna cal pin asserted when Rx channel is enabled)
*                         6b: Rx Low Stream Error  (Rx Low stream is triggered when antenna cal is ongoing for this channel)
*                         7b: Rx High Stream Error (Rx High stream is triggered when antenna cal is ongoing for this channel)
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] channelSel Mask to select the channel to read the configuration from. Multiple channel selection isn't allowed
* \param[out] errStatus Pointer to error status byte
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalErrorGet(adi_adrv904x_Device_t* const device,
                                                                      const uint32_t channelSel,
                                                                      uint8_t * const errStatus);

/**
* \brief Sweeps through all 20 stream processors and checks for failures and errors. 
*   The stream error being reported is the latest to happen. 
*   This function changes the error data saved from first to last. On first read the errors might be the first ones.
* 
*   After the errors are reported, the function clears the bits in the error registers.
*
* 
* \param[in, out] device Context variable adi_adrv904x_Device_t pointer to the device data structure
* \param[out] streamErr Error array of the 20 stream processors stating which of them had errors and what these errors are
* 
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_StreamProcErrorGet(   adi_adrv904x_Device_t* const            device,
                                                                    adi_adrv904x_StreamErrArray_t* const    streamErr);


/**
* \brief Function to select the Rx/Tx channels to be turned on Rx/Tx antenna cal GPIO pins asserted
* GPIO pin can be selected by using Gpio*Selection fields of streamSettings structure in profile json file.
* Please check GpioFeatureSelection_e of Adrvgen6StreamSettings class.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] txChannelMask Selects the Tx channels to be turned when Rx antenna cal GPIO pin asserted. Bit0:Tx0, Bit1:Tx1 and so on
* \param[in] rxChannelMask Selects the Rx channels to be turned when Tx antenna cal GPIO pin asserted. Bit0:Rx0, Bit1:Rx1 and so on
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalGpioChannelSet(adi_adrv904x_Device_t* const device,
                                                                            const uint32_t txChannelMask,
                                                                            const uint32_t rxChannelMask);

/**
* \brief Function to read Tx/Rx channel selection for GPIO based antenna cal
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] txChannelMask Pointer to readback value for tx channel selection. Bit0:Tx0, Bit1:Tx1 and so on
* \param[out] rxChannelMask Pointer to readback value for rx channel selection. Bit0:Rx0, Bit1:Rx1 and so on
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalGpioChannelGet(adi_adrv904x_Device_t* const device,
                                                                            uint32_t * const txChannelMask,
                                                                            uint32_t * const rxChannelMask);

/**
* \brief Configures the preset gain/nco variables for antenna cal
*
* Function to store predefined Antenna Cal configuration. Values in this configuration
* that correspond to user-enabled Antenna Cal options in the Stream Generator will be
* forced when Antenna Cal mode is enabled for a given Rx or Tx Channel. Configurations
* can be unique across channels. Configuration will not be forced immediately; it will
* be forced when Antenna On streams are triggered for a given Rx or Tx Channel by a
* TRX_CTRL pin. To change Tx attenuation, stream processor will use S0 index. User is
* responsible to configure the device to use S0 index(attenuation update trigger disabled)
* before using antenna cal feature.
*
* \pre This function may be called after the Stream processors have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] configRx Configuration structure for Rx channel
* \param[in] configTx Configuration structure for Tx channel
* \param[in] rxGain - gain value to apply to rx antenna cal
* \param[in] enableFreeze - enable/disable of agc freeze
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalConfigSet_v2(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_RxRadioCtrlAntennaCalConfig_t * const configRx,
                                                                          adi_adrv904x_TxRadioCtrlAntennaCalConfig_t * const configTx,
                                                                          const uint8_t                                      rxGain,
                                                                          const uint8_t                                      enableFreeze);


/**
* \brief This function is used during the init sequence in order to load the radio sequencer
*        binary images from the radioSeqBinary.bin file. The radioSeqBinary.bin
*        file has a pre-defined structure this function relies on and so the user
*        must load the radioSeqBinary.bin starting from the beginning.
*        The user may load the file in chunks as low as 100 bytes by iterating through 
*        the entire radioSeqBinary.bin and updating byteOffset on each iteration.
*        Once the final chunk of radioSeqBinary.bin has been processed the function
*        sets the appropriate status flag in device->devStateInfo.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] byteOffset Offset (starting from 0) of where the current binary chunk is located
*                   in the radioSeqBinary.bin file.
* \param[in] binary Byte array containing the current chunk of radio sequence binary file to process
* \param[in] byteCount The number of bytes in the binary parameter
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerImageWrite(adi_adrv904x_Device_t* const    device,
                                                                       uint32_t                        byteOffset,
                                                                       const uint8_t                   binary[],
                                                                       uint32_t                        byteCount);

/**
* \brief Function to write Radio Sequencer multi frame phase adjustment
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] phaseAdjustment value for RS multi frame phase adjustment
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerPhaseSet(adi_adrv904x_Device_t* const device, 
                                                                     uint32_t phaseAdjustment);

/**
* \brief Function to read Radio Sequencer multi frame phase adjustment
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] phaseAdjustment Pointer to readback value for  RS multi frame phase adjustment
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerPhaseGet(adi_adrv904x_Device_t* const device, 
                                                                     uint32_t* phaseAdjustment);

/**
* \brief This function is used during the preMcsInit sequence in order to load the radio sequencer config.
*
* \pre This function is called after FW image is loaded.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerPreMcsCfg(adi_adrv904x_Device_t* const    device);

/**
* \brief This function is used during the postMcsInit sequence in order to load the radio sequencer config.
*
* \pre This function is called after FW image is loaded.
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerPostMcsCfg(adi_adrv904x_Device_t* const    device);

/**
* \brief Function to start radio operation using the Radio Sequencer
*
* Starts radio operation using the default frame timing pattern (pattern 0) for each of the frame timing sequencers (0-9). If a sequencer does not have a frame timing pattern defined, it will execute the null pattern.
*
* \pre This function may be called after the RS binary have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerRadioStart(adi_adrv904x_Device_t* const device);

/**
* \brief Function to stop Radio Sequencer Radio
*
* Stops radio operation by setting all 10 frame timing sequencers to execute the null pattern. This operation occurs immediately, i.e. it does not wait for a multi-frame boundary.
*
* \pre This function may be called after the RS binary have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerRadioStop(adi_adrv904x_Device_t* const device);

/**
* \brief Function to set special pattern for Radio Sequencer
*
* Set customer special frame timing pattern for each radio sequencers (0-9).
*
* \pre This function may be called after the RS binary have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] specialPatternInfo - Pointer to the special pattern ID data structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerFrameTimingPatternSet(adi_adrv904x_Device_t* const                   device,
                                                                                  const adi_adrv904x_RsSpecialSequencer_t* const specialPatternInfo);

/**
* \brief Function to get special pattern for Radio Sequencer
*
* Get customer special frame timing pattern for each radio sequencers (0-9).
*
* \pre This function may be called after the RS binary have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] specialPatternInfo - Pointer to the special pattern ID data structure pointer. if the specialPatternUsed is false, pattern ID will be 0
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerFrameTimingPatternGet(adi_adrv904x_Device_t* const             device,
                                                                                  adi_adrv904x_RsSpecialSequencer_t* const specialPatternInfo);

/**
* \brief Function to clear special pattern for Radio Sequencer
*
* Clear customer special frame timing pattern for each radio sequencers (0-9).
*
* \pre This function may be called after the RS binary have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings

* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerFrameTimingPatternClear(adi_adrv904x_Device_t* const device);

/**
* \brief Sets the Tx to ORx external signal routing for Tx calibrations that use the ORx for observation.
*
* This function invokes the streams required to setup a Tx to ORx mapping configuration. It also sets the 
* direction of VSWR capture. This function should only be used for VSWR calibration.
*
* \pre This function may be called after the Stream processor initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] mapping Tx to ORx mapping byte to apply. Corresponds to Tx to ORx Mapping Mode and 
*               Tx Observability Word selected during Initialization.
* \param[in] vswrDirection VSWR direction for this mapping, either FWD or REV. Bit 0: Orx0 direction // Bit 1: Orx1 direction
*            adi_adrv904x_DfeVswrDirection_e enum can be used to populate these bits
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingAndVswrDirSet(adi_adrv904x_Device_t* const    device,
                                                                          const uint8_t                   mapping,
                                                                          const uint8_t                   vswrDirection);

/**
* \brief Gets the Tx to ORx external signal routing for Tx calibrations that use the ORx for observation.
*
* This function retrieves from hardware the Tx channel currently mapped to the requested ORx channel. It
* also reads VSWR directions for the selected ORx channel. It should only be used for VSWR calibration
*
* \pre This function may be called after the Stream processors have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] orxChannel The ORx channel for which Tx channel mapping is requested
* \param[out] txChannel The Tx channel map to the ORx channel
* \param[out] vswrDirection VSWR direction for this mapping, either FWD or REV
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingAndVswrDirGet(adi_adrv904x_Device_t* const              device,
                                                                          const adi_adrv904x_RxChannels_e           orxChannel,
                                                                          adi_adrv904x_TxChannels_e* const          txChannel,
                                                                          adi_adrv904x_DfeVswrDirection_e* const    vswrDirection);

/**
* \brief Set the requested PA En input.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] paEnOutput The bitmask selection based on adi_adrv904x_PaEnOutputs_e.
* \param[in] paEnInput  The selection of either PA En or DTX.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlPaSelectInputSet(adi_adrv904x_Device_t* const        device,
                                                                        const uint32_t                      paEnOutput,
                                                                        const adi_adrv904x_PaEnInputSel_e   paEnInput);

/**
* \brief Get the requested PA En input.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] paEnOutput  The selection based on adi_adrv904x_PaEnOutputs_e.
* \param[out] paEnInput  Pointer to the value selection of either PA En or DTX.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlPaSelectInputGet(adi_adrv904x_Device_t* const          device,
                                                                        const adi_adrv904x_PaEnOutputs_e      paEnOutput,
                                                                        adi_adrv904x_PaEnInputSel_e * const   paEnInput);

/**
* \brief Set the requested DTX fanout to PA_EN configuration.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] txChannelMask - Mask to select Tx channels to be affected with this function call
* \param[in] dtxFanoutCfg - Pointer to the DTX fanout to PA_EN configuration data structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDtxFanoutToPaEnConfigSet(adi_adrv904x_Device_t* const    device,
                                                                                const uint32_t txChannelMask,
                                                                                const adi_adrv904x_DtxFanoutToPaEnCfg_t * const dtxFanoutCfg);

/**
* \brief Get the DTX fanout to PA_EN configuration.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] dtxFanoutCfg - Pointer to the DTX fanout to PA_EN configuration data structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDtxFanoutToPaEnConfigGet(adi_adrv904x_Device_t* const    device,
                                                                                adi_adrv904x_DtxFanoutToPaEnCfg_t * const dtxFanoutCfg);

/**
* \brief Configure the external alarm by updating corresponding entries in the External Alarm Configuration table.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] alarmMask  The selection bit mask based on adi_adrv904x_Alarms_e.
* \param[in] alarmCfg  Pointer to a structure adi_adrv904x_RadioCtrlAlarmConfig_t specifying external alarm configuration.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlExtAlarmConfigSet(adi_adrv904x_Device_t* const                      device,
                                                                         const uint32_t                                    alarmMask,
                                                                         const adi_adrv904x_RadioCtrlAlarmConfig_t * const alarmCfg);

/**
* \brief Read the configuration data of the external alarm.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in]  alarmId  Specify the alarm based on adi_adrv904x_Alarms_e.
* \param[out] alarmCfg  Pointer to a structure adi_adrv904x_RadioCtrlAlarmConfig_t to contain external alarm configuration.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlExtAlarmConfigGet(adi_adrv904x_Device_t* const                device,
                                                                         const adi_adrv904x_Alarms_e                 alarmId,
                                                                         adi_adrv904x_RadioCtrlAlarmConfig_t * const alarmCfg);

/**
* \brief Retrieve external alarm state for a specific alarm.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in]  alarmId  Specify the alarm based on adi_adrv904x_Alarms_e.
* \param[out] alarmState  Pointer to a variable containing value of alarm state.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlExtAlarmStateGet(adi_adrv904x_Device_t* const        device,
                                                                        const adi_adrv904x_Alarms_e         alarmId,
                                                                        adi_adrv904x_AlarmState_e * const   alarmState);

/**
* \brief Retrieve the state of the GPIO pin assigned to a specific alarm.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in]  alarmId  Specify the alarm based on adi_adrv904x_Alarms_e.
* \param[out] alarmState  Pointer to a variable containing value of alarm state.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlExtAlarmGpioStateGet(adi_adrv904x_Device_t* const        device,
                                                                            const adi_adrv904x_Alarms_e         alarmId,
                                                                            adi_adrv904x_AlarmState_e * const   alarmState);

/**
* \brief Clear a specific external alarm state.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] alarmId  The specific alarm based on adi_adrv904x_Alarms_e.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlExtAlarmStateClear(adi_adrv904x_Device_t* const      device,
                                                                          const adi_adrv904x_Alarms_e       alarmId);

/**
* \brief Activate or deactivate the LNA Always-On.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] lnaEnMask  The selection bit mask based on adi_adrv904x_LnaEnOutputs_e.
* \param[in] activate   Value to indicate the operation: 0= deactivate, 1=activate
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgLnaAlwaysOnSet(adi_adrv904x_Device_t* const      device,
                                                                         const uint32_t lnaEnMask,
                                                                         const uint8_t activate);

/**
* \brief Retrieving the LNA Always on activation state.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] lnaEnId  Specifying which LNA En to retrieve based on adi_adrv904x_LnaEnOutputs_e.
* \param[out] activate  Pointer to a value indicating the activation state of a specific LNA En (0=Deactivate, 1=Activate).
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgLnaAlwaysOnGet(adi_adrv904x_Device_t* const      device,
                                                                         const adi_adrv904x_LnaEnOutputs_e lnaEnId,
                                                                         uint8_t * const activate);

/**
* \brief Activate or deactivate the LNA Always-Off.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] lnaEnMask  The selection bit mask based on adi_adrv904x_LnaEnOutputs_e.
* \param[in] activate   Value to indicate the operation: 0= deactivate, 1=activate
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgLnaAlwaysOffSet(adi_adrv904x_Device_t* const      device,
                                                                          const uint32_t lnaEnMask,
                                                                          const uint8_t activate);

/**
* \brief Retrieving the LNA Always-Off activation state.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] lnaEnId  Specifying which LNA En to retrieve based on adi_adrv904x_LnaEnOutputs_e.
* \param[out] activate  Pointer to a value indicating the activation state of a specific LNA En (0=Deactivate, 1=Activate).
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgLnaAlwaysOffGet(adi_adrv904x_Device_t* const      device,
                                                                          const adi_adrv904x_LnaEnOutputs_e lnaEnId,
                                                                          uint8_t * const activate);

/**
* \brief Activate or deactivate the PA Always-On.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] paEnMask  The selection bit mask based on adi_adrv904x_PaEnOutputs_e.
* \param[in] activate   Value to indicate the operation: 0= deactivate, 1=activate
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgPaAlwaysOnSet(adi_adrv904x_Device_t* const      device,
                                                                        const uint32_t paEnMask,
                                                                        const uint8_t activate);

/**
* \brief Retrieving the PA Always-On activation state.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] paEnId  Specifying which PA En to retrieve based on adi_adrv904x_PaEnOutputs_e.
* \param[out] activate  Pointer to a value indicating the activation state of a specific PA En (0=Deactivate, 1=Activate).
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgPaAlwaysOnGet(adi_adrv904x_Device_t* const      device,
                                                                        const adi_adrv904x_PaEnOutputs_e  paEnId,
                                                                        uint8_t * const                   activate);
                                                                        
/**
* \brief Activate or deactivate the PA Always-Off.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] paEnMask  The selection bit mask based on adi_adrv904x_PaEnOutputs_e.
* \param[in] activate   Value to indicate the operation: 0= deactivate, 1=activate
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgPaAlwaysOffSet(adi_adrv904x_Device_t* const      device,
                                                                         const uint32_t                    paEnMask,
                                                                         const uint8_t                     activate);

/**
* \brief Retrieving the PA Always-Off activation state.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] paEnId  Specifying which PA En to retrieve based on adi_adrv904x_PaEnOutputs_e.
* \param[out] activate  Pointer to a value indicating the activation state of a specific PA En (0=Deactivate, 1=Activate).
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgPaAlwaysOffGet(adi_adrv904x_Device_t* const      device,
                                                                         const adi_adrv904x_PaEnOutputs_e  paEnId,
                                                                         uint8_t * const                   activate);

/**
* \brief Set the OC Fuse Enable Output state.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] alarmId  Specify the alarm based on adi_adrv904x_Alarms_e.
* \param[in] state  The state to be set based on adi_adrv904x_OutputState_e.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlOcFuseEnOutSet(adi_adrv904x_Device_t* const      device,
                                                                      const adi_adrv904x_Alarms_e       alarmId,
                                                                      const adi_adrv904x_OutputState_e  state);

/**
* \brief Set the GPIO output for selected signals.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] gpioSigType The GPIO signal type selection.
* \param[in] signalMask Multiple GPIO signals can be selected with selection bit masks.
* \param[in] gpioPinstate Requested GPIO pin state
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgGpioOutputSet(adi_adrv904x_Device_t* const device,
                                                                        const adi_adrv904x_OutGpioSigTypes_e gpioSigType,
                                                                        const uint8_t signalMask,
                                                                        const adi_adrv904x_OutGpioPinStates_e gpioPinstate);

/**
* \brief Retrieve the GPIO output state for selected signal.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] gpioSigType The GPIO signal type selection.
* \param[in] signalMask The GPIO signal selection bit mask. Only one selected signal allow.
* \param[out] gpioPinstate Pointer to a value indicating current GPIO pin state
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgGpioOutputGet(adi_adrv904x_Device_t* const device,
                                                                        const adi_adrv904x_OutGpioSigTypes_e gpioSigType,
                                                                        const uint8_t signalMask,
                                                                        adi_adrv904x_OutGpioPinStates_e * const gpioPinstate);



/**
* \brief Set the antenna cal carrier config init structure.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] antCalCarrierCfgInit Pointer to the antenna cal carrier config 
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalCarrierCfgInitSet(adi_adrv904x_Device_t* const device,
                                                                               const adi_adrv904x_RadioCtrlAntCalCarrierCfgInit_t* const antCalCarrierCfgInit);

/**
* \brief Set the antenna cal carrier config runtime structure.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] antCalCarrierCfg Pointer to the antenna cal carrier config 
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalCarrierCfgSet(adi_adrv904x_Device_t* const device,
                                                                           const adi_adrv904x_RadioCtrlAntCalCarrierCfg_t* const antCalCarrierCfg);

/**
* \brief Set the antenna cal carrier config runtime structure and use the provided patternId when done.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] antCalCarrierCfg Pointer to the antenna cal carrier config 
* \param[in] postCalPatternId pattern Id to use after antenna calibration 
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalCarrierCfgSet_v2(adi_adrv904x_Device_t* const device,
                                                                              const adi_adrv904x_RadioCtrlAntCalCarrierCfg_t* const antCalCarrierCfg,
                                                                              uint32_t postCalPatternId);

/**
* \brief Get the antenna cal carrier config runtime structure.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] antCalCarrierCfg Pointer to the antenna cal carrier config 
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalCarrierCfgGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_RadioCtrlAntCalCarrierCfg_t* const antCalCarrierCfg);

/**
* \brief Clear the antenna cal GPIO counter and gain table index.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalCarrierGainCountersClear(adi_adrv904x_Device_t* const device);

/**
* \brief Get the antenna cal GPIO counter and gain table index.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] antCalGpioCounter Pointer to the antenna cal GPIO rising edge counter 
* \param[out] antCalCarrierTableIndex Pointer to the antenna cal gain table index 
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalCarrierGainCountersGet(adi_adrv904x_Device_t* const device,
                                                                                    uint8_t* const antCalGpioCounter,
                                                                                    uint8_t* const antCalCarrierTableIndex);



/**
* \brief Gets the Tx to ORx Mapping Configuration setup for the device, in DFE Control Mode only.
*
* This function retrieves from device handle DFE Control mode Tx to ORx mapping configuration setup during initialization.
* This this function is called to reacback the config, but the device is in RCI mode, an error will be returned.
*
* \pre This function may be called after device Post Msc Initialization.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] mappingConfig The Dfe Control Tx to ORx mapping configuration that was applied to the device during PostMcsInit
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCtrlTxToOrxMappingConfigGet(adi_adrv904x_Device_t* const                       device,
                                                                             adi_adrv904x_DfeCtrlTxToOrxMappingConfig_t * const mappingConfig);

/**
* \brief Sets the DFE CTRL Tx to ORx mapping and drives GPIO outputs for a given Orx Channel to their expected state for Tx Channel
*
* This function invokes the streams required to apply a Tx to ORx mapping configuration
*
* \pre This function may be called after the Stream processor initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] orxChannel The ORx channel for which Tx channel mapping is updated
* \param[in] txChannel The Tx channel to map to the selected ORx channel
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCtrlTxToOrxMappingSet(adi_adrv904x_Device_t* const     device,
                                                                       const adi_adrv904x_RxChannels_e  orxChannel,
                                                                       const adi_adrv904x_TxChannels_e  txChannel);

/**
* \brief Sets the DFE CTRL Tx to ORx mapping and VSWR Direction and drives GPIO outputs for a given Orx Channel to their expected state for Tx Channel
*
* This function invokes the streams required to apply a Tx to ORx mapping configuration
*
* \pre This function may be called after the Stream processor initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] orxChannel The ORx channel for which Tx channel mapping is updated
* \param[in] txChannel The Tx channel to map to the selected ORx channel
* \param[in] vswrDirection The VSWR direction to set, either FWD or REV.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCtrlTxToOrxMappingAndVswrDirSet(adi_adrv904x_Device_t* const           device,
                                                                                 const adi_adrv904x_RxChannels_e        orxChannel,
                                                                                 const adi_adrv904x_TxChannels_e        txChannel,
                                                                                 const adi_adrv904x_DfeVswrDirection_e  vswrDirection);

/**
* \brief Updates the preset GPIO output levels/state to apply when TXOFF state is applied for a given ORx.
*
* \pre This function may be called after the device is initialized and Powerup Stream has run.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] orxChannel The ORx channel for which Tx channel mapping is updated
* \param[in] txOffGpioLevels 4bit (max) gpio state to drive when TX OFF state is selected for orxChannel by adi_adrv904x_DfeCtrlTxToOrxMappingSet() or adi_adrv904x_DfeCtrlTxToOrxMappingAndVswrDirSet()
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCtrlTxToOrxMappingTxOffLevelsSet(adi_adrv904x_Device_t* const       device,
                                                                                  const adi_adrv904x_RxChannels_e    orxChannel,
                                                                                  const uint8_t                      txOffGpioLevels);

/**
* \brief Readback of AntCal Output GPIO config structure loaded in Stream Processor Binary.
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out]    pCfg – pointer to Stream GPIO Ant Cal Output Config structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalOutGpioCfgGet(adi_adrv904x_Device_t* const                 device, 
                                                                           adi_adrv904x_StreamGpioAntCalOutCfg_t* const pCfg);

/**
* \brief Manually set the Stream GPIO Antenna Cal outputs. Temporary drive states will be overriden at next UL/DL rising/falling edge
*
* \pre This function is called after FW image is loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in]     drive – Value to manually drive onto Stream Output AntCal GPIOs. Valid range is 8bit (0-255).
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalOutGpioLevelSet(adi_adrv904x_Device_t* const   device,
                                                                             const uint32_t                 drive);

/**
* \brief Select GPIO output true table during antenna calibration
*
*
*
* \pre This function is called after the device has been initialized and radio multichip-sync
* (MCS) has been completed. This function shouldn't be called when antenna calibration is ongoing.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in] tableSelection - GPIO output true table selection: 0 - MAIN path, 1: SECONDARY path.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e  adi_adrv904x_RadioCtrlAntCalGpioTruthTableSet(adi_adrv904x_Device_t* const device,
                                                                                uint8_t tableSelection);

/**
* \brief Retrieve GPIO output true table selection during antenna calibration
*
*
*
* \pre This function is called after the device has been initialized and radio multichip-sync
* (MCS) has been completed
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in,out] pTableSelection - Pointer to GPIO output true table selection: 0 - MAIN path, 1 - SECONDARY path.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalGpioTruthTableGet(adi_adrv904x_Device_t* const device,
                                                                               uint8_t* pTableSelection);

#endif /* _ADI_ADRV904X_RADIOCTRL_H_ */
