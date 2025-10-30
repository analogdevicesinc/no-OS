/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * \file adrv903x_radioctrl.h
 * \brief Contains ADRV903X radio control related private function prototypes for
 *        adrv903x_radioctrl.c which helps adi_adrv903x_radioctrl.c
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef _ADRV903X_RADIOCTRL_H_
#define _ADRV903X_RADIOCTRL_H_

#include "adrv903x_radioctrl_types.h"
#include "adrv903x_cpu_macros.h"
#include "adrv903x_cpu_cmd.h"

#include "adi_adrv903x_radioctrl.h"
#include "adi_adrv903x_error.h"

/**
* \brief This function directly triggers a stream on the Core Stream Processor.
* 
* This function utilizes the ARM mailbox with a particular opcode to trigger the Core
* Stream Processor directly. Mailbox must not be busy in order to send the command, but
* ARM is not involved and will not send a response.
*
* \pre This function can be called after the stream binary has been loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] streamId unique stream identification no. of the stream to execute
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_StreamTrigger(adi_adrv903x_Device_t* const    device,
                                                        const uint8_t                   streamId);

/**
* \brief This function indirectly triggers a stream on the TX Stream Processor.
*
* This function calls a core stream to trigger a stream on TX Stream Processor.
*
* \pre This function can be called after the stream binary has been loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] txChanMask TX channel mask
* \param[in] txStreamId  unique TX stream identification no. of the stream to execute
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_TxStreamTrigger(adi_adrv903x_Device_t* const           device,
                                                          const uint8_t                          txChanMask,
                                                          const uint8_t                          txStreamId);

/**
* \brief This function indirectly triggers a stream on the RX Stream Processor.
*
* This function calls a core stream to trigger a stream on RX Stream Processor.
*
* \pre This function can be called after the stream binary has been loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] rxChanMask RX channel mask
* \param[in] rxStreamId  unique RX stream identification no. of the stream to execute
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxStreamTrigger(adi_adrv903x_Device_t* const           device,
                                                          const uint8_t                          rxChanMask,
                                                          const uint8_t                          rxStreamId);
/**
* \brief Reads back the version of the ADC module for the cal type
*
* This function sends a control command to the ARM to read back the major.minor.maint.build
* version for specified ADC module (Rx, Tx, Orx where Tx and ORx have the same version parameters)
*
* \pre 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] calType - calibration type
* \param[in] channel - channel number
* \param[out] adcVersion - Pointer to four parts of ADC version
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_AdcVersionGet(adi_adrv903x_Device_t* const   device,
                                                        const uint8_t                  calType,
                                                        const adi_adrv903x_Channels_e  channel,
                                                        adi_adrv903x_Version_t* const  adcVersion);

/**
* \brief Reads back the data sizes and base addresses of the ADC module for the cal type
*
* This function sends a control command to the ARM to read back the sizes and the addresses
* for specified ADC module (Rx, Tx, Orx) and channel.
*
* \pre 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] calType - calibration type
* \param[in] channel - channel number
* \param[out] adcSizes - Pointer to adc sizes structure
* \param[out] adcAddrs - Pointer to adc addresses structure
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_AdcGetDataInfo(adi_adrv903x_Device_t* const             device,
                                                         const uint8_t                            calType,
                                                         const adi_adrv903x_Channels_e            channel,
                                                         adrv903x_CpuCmd_GetAdcSizeAddrs_t* const adcSizes,
                                                         adrv903x_CpuCmd_GetAdcSizeAddrs_t* const adcAddrs);

/**
* \brief Run the Init command of the ADC module for the cal type
*
* This function sends a control command to the ARM to run the Init command
* for specified ADC module (Rx, Tx, Orx) and channel.
*
* \pre 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] calType - calibration type
* \param[in] channel - channel number
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_AdcRunInit(adi_adrv903x_Device_t* const  device,
                                                     const uint8_t                 calType,
                                                     const adi_adrv903x_Channels_e channel);

/**
* \brief Set the FSM command of the ADC module for the cal type
*
* This function sends a control command to the ARM to send an FSM command
* for specified the ADC module (Rx, Tx, Orx) and channel.
*
* \pre 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] calType - calibration type
* \param[in] channel - channel number
* \param[in] fsmCmd  - command number
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_AdcSetFsmCmd(adi_adrv903x_Device_t* const    device,
                                                       const uint8_t                   calType,
                                                       const adi_adrv903x_Channels_e   channel,
                                                       const uint8_t                   fsmCmd);

/**
* \brief Reads back the ADC FSM state of the ADC module for the cal type
*
* This function sends a control command to the ARM to read back the FSM state
* for specified the ADC module (Rx, Tx, Orx) and channel.
*
* \pre 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] calType - calibration type
* \param[in] channel - channel number
* \param[out] fsmState - Pointer to state variable
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_AdcGetFsmState(adi_adrv903x_Device_t* const  device,
                                                         const uint8_t                 calType,
                                                         const adi_adrv903x_Channels_e channel,
                                                         uint8_t* const                fsmState);

/**
* \brief This command results in the FW calling the <adc_module>_Run() function
*
* Execute the <adc_module>_Run() function for the
* specified ADC module (Rx, Tx, Orx) and channel.
*
* \pre 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] calType - calibration type
* \param[in] channel - channel number
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_AdcRunCmd(adi_adrv903x_Device_t* const    device,
                                                    const uint8_t                   calType,
                                                    const adi_adrv903x_Channels_e   channel);

/**
* \brief Set the Tracking cal rate in milli seconds
*
* This function sends a set config command to the ARM to set the timer used for the specified tracking cal.
*
* \pre 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] objId - object ID
* \param[in] rateMsec - rate in milli seconds
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_SetTrackingPeriod(adi_adrv903x_Device_t* const device,
                                                            uint8_t   objId,
                                                            uint32_t  rateMsec);

/**
* \brief Get the Tracking cal rate in milli seconds
*
* This function sends a get config command to the ARM to get the timer used for the specified tracking cal.
*
* \pre 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] objId - object ID
* \param[out] rateMsec - rate in milli seconds
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_GetTrackingPeriod(adi_adrv903x_Device_t* const device,
                                                            uint8_t   objId,
                                                            uint32_t* const rateMsec);

#ifndef CLIENT_IGNORE
/**
* \brief Perform range check for adi_adrv903x_RxTxEnableSet.
*
* Checks that all the channels in the Rx and Tx masks exist. Any invalid channel
* results in failure. Also checks that CPU state is suitable for allowing enabling of
* Rx/Tx channels.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X device data structure containing settings
* \param[in] orxChannelMask Desired Orx signal chain (channel) to power up/down
* \param[in] orxChannelEnable Desired Orx signal chain (channel) to Enable(1)/Disable(0) - Unused parameter
* \param[in] rxChannelMask Desired Rx signal chain (channel) to power up/down
* \param[in] rxChannelEnable Desired Rx signal chain (channel) to Enable(1)/Disable(0) - Unused parameter
* \param[in] txChannelMask Desired Tx signal chains (channel) to power up/down
* \param[in] txChannelEnable Desired Tx signal chain (channel) to Enable(1)/Disable(0) - Unused parameter
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxTxEnableSetRangeCheck(adi_adrv903x_Device_t* const device,
                                                                  const uint32_t               orxChannelMask,
                                                                  const uint32_t               orxChannelEnable,
                                                                  const uint32_t               rxChannelMask,
                                                                  const uint32_t               rxChannelEnable,
                                                                  const uint32_t               txChannelMask,
                                                                  const uint32_t               txChannelEnable);

/**
* \brief Enables or disables all Rx channels that are in SPI control mode (see
* adi_adrv903x_RadioCtrlCfgSet. Has no effect on channels that are not set to SPI control mode.
*
* \pre This function should only be called after the transceiver is initialized and the stream
* images are loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param [in] device Pointer to an ADRV903X device
* \param [in] rxChannelMask Desired Rx signal chains (channels) to power up/down
* \param [in] rxChannelEnable Desired Rx signal chains (channels) to Enable(1)/Disable(0)
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxEnableSet(adi_adrv903x_Device_t* const device,
                                                      const uint32_t rxChannelMask,
                                                      const uint32_t rxChannelEnable);

/**
* \brief Enables or disables all Tx channels that are in SPI control mode (see
* adi_adrv903x_RadioCtrlCfgSet. Has no effect on channels that are not set to SPI control mode.
*
* \pre This function should only be called after the transceiver is initialized and the stream
* images are loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param [in] device Pointer to an ADRV903X device
* \param [in] txChannelMask Desired Tx signal chains (channels) to power up/down
* \param [in] txChannelEnable Desired Tx signal chains (channels) to Enable(1)/Disable(0)
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_TxEnableSet(adi_adrv903x_Device_t* const device,
                                                      const uint32_t txChannelMask,
                                                      const uint32_t txChannelEnable);

/**
* \brief Sets the ORx channels to be enabled or disabled in the transceiver if the signal chain
* control is configured to be in SPI mode.
*
* For use cases where pin mode is not required, this function can be used to
* enable/disable the ORx signal paths.
*
* \pre This function should be called after initialization and loading the stream
* processor.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param [in] device Pointer to the ADRV903X data structure
* \param [in] rxChannelMask Desired ORx signal chains (channel) to power up/down. Note only the bits indicating
* ORx channels are effective. Other bits indicating Rx channels are ignored. See rxChannels argument to
* adrv903x_RxTxEnableSet.
* \param [in] rxChannelEnable Desired ORx signal chains (channels) to Enable(1)/Disable(0)
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/

ADI_API adi_adrv903x_ErrAction_e adrv903x_OrxEnableSet(adi_adrv903x_Device_t* const device,
                                                       const uint32_t rxChannelMask,
                                                       const uint32_t rxChannelEnable);


/**
* \brief This function performs Range check on parameters passed to adi_adrv903x_RadioCtrlCfgSet() API
*
* This function is called automatically as part of adi_adrv903x_RadioCtrlCfgSet() if ADI_ADRV903X_RADIOCTRL_RANGE_CHECK
* is set to 1. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] radioCtrlCfg is comprised of radio control mode configurations for Rx,ORx and Tx channels
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RadioCtrlCfgSetRangeCheck(adi_adrv903x_Device_t * const                   device,
                                                                    const adi_adrv903x_RadioCtrlModeCfg_t* const    radioCtrlCfg);

/**
* \brief This function sets up the ORx signal chain Radio Ctrl configuration
* 
* In Pin mode, the Rx signal chain is controlled via dedicated pins RX_ENABLE<3:0>. In
* Non-pin mode, the API command is used to configure Rx radio ctrl settings.
*
* \pre This function can be called after initialization and completion of stream loading.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] orxRadioCtrlModeCfg Pointer to ORx radio ctrl mode configuration settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_OrxRadioCtrlCfgSet(adi_adrv903x_Device_t* const                device,
                                                             adi_adrv903x_ORxRadioCtrlModeCfg_t* const   orxRadioCtrlModeCfg);

/**
* \brief This function sets up the Rx signal chain Radio Ctrl configuration
*
*  In Pin mode, the Rx signal chain is controlled via dedicated pins RX_ENABLE<3:0>. In
*  Non-pin mode, the API command is used to configure Rx radio ctrl settings.
*
* \pre This function can be called after initialization and completion of stream loading.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] rxRadioCtrlModeCfg Pointer to ORx radio ctrl mode configuration settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxRadioCtrlCfgSet(adi_adrv903x_Device_t* const                device,
                                                            adi_adrv903x_RxRadioCtrlModeCfg_t* const    rxRadioCtrlModeCfg);

/**
* \brief This function sets up the Tx signal chain Radio Ctrl configuration
*
*  In Pin mode, the Tx signal chain is controlled via dedicated pins TX_ENABLE<3:0>. In
*  Non-pin mode, the API command is used to configure Tx radio ctrl settings.
*
* \pre This function can be called after initialization and completion of stream loading.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] txRadioCtrlModeCfg Pointer to ORx radio ctrl mode configuration settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_TxRadioCtrlCfgSet(adi_adrv903x_Device_t* const                device,
                                                            adi_adrv903x_TxRadioCtrlModeCfg_t* const    txRadioCtrlModeCfg);

/**
* \brief This function reads back the ORx signal chain Radio Ctrl configuration
*
* \pre This function can be called after initialization.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] rxChannel Desired RxChannel
* \param[out] orxRadioCtrlModeCfg Pointer to ORx radio ctrl mode configuration settings
*        which will be updated with read back configuration
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_OrxRadioCtrlCfgGet(adi_adrv903x_Device_t* const                device,
                                                             const adi_adrv903x_RxChannels_e             rxChannel,
                                                             adi_adrv903x_ORxRadioCtrlModeCfg_t* const   orxRadioCtrlModeCfg);

/**
* \brief This function reads back the Rx signal chain Radio Ctrl configuration
*
* \pre This function can be called after initialization.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] rxChannel Desired RxChannel
* \param[out] rxRadioCtrlModeCfg Pointer to Rx radio ctrl mode configuration settings
*        which will be updated with read back configuration
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxRadioCtrlCfgGet(adi_adrv903x_Device_t* const                device,
                                                            const adi_adrv903x_RxChannels_e             rxChannel,
                                                            adi_adrv903x_RxRadioCtrlModeCfg_t* const    rxRadioCtrlModeCfg);

/**
* \brief This function reads back the Tx signal chain Radio Ctrl configuration
*
* \pre This function can be called after initialization.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X data structure
* \param[in] txChannel Desired TxChannel
* \param[out] txRadioCtrlModeCfg Pointer to Tx radio ctrl mode configuration settings
*        which will be updated with read back configuration
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_TxRadioCtrlCfgGet(adi_adrv903x_Device_t* const                device,
                                                            const adi_adrv903x_TxChannels_e             txChannel,
                                                            adi_adrv903x_TxRadioCtrlModeCfg_t* const    txRadioCtrlModeCfg);

/**
* \brief This function masks/unmasks the LO unlock GP interrupt for a requested LO
*
* When the LO frequency is being updated, the LO is unlocked which generates
* a GP interrupt. For use cases where LO frequency is required to be changed,
* the GP interrupt needs to be masked so that it does not flag a false interrupt.
*
* \pre
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV903X data structure
* \param loName LO for which frequency needs to be set
* \param gpInterruptPin0Mask flag to mask(1) / unmask(0) the requested LO unlock interrupt on Pin0
* \param gpInterruptPin1Mask flag to mask(1) / unmask(0) the requested LO unlock interrupt on Pin1
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_LoUnlockGpInterruptMaskSet(adi_adrv903x_Device_t* const    device,
                                                                     adi_adrv903x_LoName_e const     loName,
                                                                     uint8_t const                   loGpInterruptPin0Mask,
                                                                     uint8_t const                   loGpInterruptPin1Mask);

/**
* \brief This reads the LO unlock GP interrupt mask for a requested LO
*
* This function can be used to retrieve current GP Interrupt mask status for the requested LO.
*
* \pre
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV903X data structure
* \param loName LO unlock GP Interrupt mask is requested
* \param loGpInterruptPin0Mask Current mask/unmask status of the requested LO unlock GP Interrupt on Pin0
* \param loGpInterruptPin1Mask Current mask/unmask status of the requested LO unlock GP Interrupt on Pin1
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_LoUnlockGpInterruptMaskGet(adi_adrv903x_Device_t* const    device,
                                                                     adi_adrv903x_LoName_e  const    loName,
                                                                     uint8_t* const                  loGpInterruptPin0Mask,
                                                                     uint8_t* const                  loGpInterruptPin1Mask);

/**
* \brief This function masks/unmasks the LO overrange GP interrupt for a requested LO
*
* When the LO frequency is being updated, the LO is unlocked which generates
* a GP interrupt. For use cases where LO frequency is required to be changed,
* the GP interrupt needs to be masked so that it does not flag a false interrupt.
*
* \pre
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV903X data structure
* \param loName LO for which frequency needs to be set
* \param gpInterruptPin0Mask flag to mask(1) / unmask(0) the requested LO unlock interrupt on Pin0
* \param gpInterruptPin1Mask flag to mask(1) / unmask(0) the requested LO unlock interrupt on Pin1
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_LoOverrangeGpInterruptMaskSet(adi_adrv903x_Device_t* const    device,
                                                                        const adi_adrv903x_LoName_e     loName,
                                                                        const uint8_t                   loGpInterruptPin0Mask,
                                                                        const uint8_t                   loGpInterruptPin1Mask);

/**
* \brief This reads the GP Interrupt overrange mask status
*
* This function can be used to retrieve current GP Interrupt mask status
* for the requested LO.
*
* \pre
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV903X data structure
* \param loName LO unlock GP Interrupt mask is requested
* \param loGpInterruptPin0Mask Current mask/unmask status of the requested LO unlock GP Interrupt on Pin0
* \param loGpInterruptPin1Mask Current mask/unmask status of the requested LO unlock GP Interrupt on Pin1
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_LoOverrangeGpInterruptMaskGet(adi_adrv903x_Device_t* const    device,
                                                                        const adi_adrv903x_LoName_e       loName,
                                                                        uint8_t* const                    loGpInterruptPin0Mask,
                                                                        uint8_t* const                    loGpInterruptPin1Mask);

/**
* \brief Performs range check on parameters for adrv903x_LoFrequencySetRangeCheck function
*
* \pre This function is automatically called as part of adi_adrv903x_LoFrequencySet if
*      ADI_ADRV903X_RADIOCTRL_RANGE_CHECK is set to 1.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X device data structure containing settings
* \param[in] loConfig Pointer to the LO config settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_LoFrequencySetRangeCheck(adi_adrv903x_Device_t* const device,
                                                                   const adi_adrv903x_LoConfig_t* const loConfig);

/**
* \brief Performs range check on parameters for adrv903x_LoLoopFilterSetRangeCheck function
*
* \pre This function is automatically called as part of adi_adrv903x_LoLoopFilterSet if
*      ADI_ADRV903X_RADIOCTRL_RANGE_CHECK is set to 1.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] loName The RF LO selected
* \param[in] loLoopFilterConfig Pointer to the LO config settings
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_LoLoopFilterSetRangeCheck(adi_adrv903x_Device_t* const device,
                                                                    const adi_adrv903x_LoName_e  loName,
                                                                    const adi_adrv903x_LoLoopFilterCfg_t* const loLoopFilterConfig);



/**
* \brief Performs range check on parameters for adi_adrv903x_StreamGpioConfigSet function.
*
** \pre This function is automatically called as part of adi_adrv903x_StreamGpioConfigSet if
*      ADI_ADRV903X_RADIOCTRL_RANGE_CHECK is set to 1.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in]     streamGpioPinCfg Pointer to the structure holding GPIO pin to stream GP input mappings (Input)
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_StreamGpioConfigSetRangeCheck(adi_adrv903x_Device_t* const          device,
                                                                        const adi_adrv903x_StreamGpioPinCfg_t* const streamGpioPinCfg);                                                                       
                                                                        
#endif //CLIENT_IGNORE

#endif
