/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adrv9025_radioctrl.h
* \brief Contains ADRV9025 radio control related private function prototypes for
*        adrv9025_radioctrl.c that helps adi_adrv9025_radioctrl.c
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADRV9025_RADIOCTRL_H_
#define _ADRV9025_RADIOCTRL_H_

#include "../devices/adrv9025/public/include/adi_adrv9025_types.h"
#include "../devices/adrv9025/public/include/adi_adrv9025_radioctrl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief This private sets the Tx channels to be enabled or disabled in the
*        transceiver if the signal chain control is configured to be in SPI mode
*
* For use cases where pin mode is not required, this function can be used to
* enable/disable the Tx signal paths.  This
* function should be called after initialization and loading the stream
* processor.
*
* \pre This function should be called after initialization and loading the stream
* processor.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param txChannelMask Desired Tx signal chains (channel) to power up
*
* \retval ADI_ADRV9025_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxEnableSet(adi_adrv9025_Device_t* device,
                             uint32_t               txChannelMask);

/**
* \brief This private sets the Rx channels to be enabled or disabled in the
*        transceiver if the signal chain control is configured to be in SPI mode
*
* For use cases where pin mode is not required, this function can be used to
* enable/disable the Rx signal paths.  This
* function should be called after initialization and loading the stream
* processor.
*
* \pre This function should be called after initialization and loading the stream
* processor.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param rxChannelMask Desired Rx signal chains (channel) to power up
*
* \retval ADI_ADRV9025_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxEnableSet(adi_adrv9025_Device_t* device,
                             uint32_t               rxChannelMask);

/**
* \brief This private sets the ORx channels to be enabled or disabled in the
*        transceiver if the signal chain control is configured to be in SPI mode
*
* For use cases where pin mode is not required, this function can be used to
* enable/disable the ORx signal paths.  This
* function should be called after initialization and loading the stream
* processor.
*
* \pre This function should be called after initialization and loading the stream
* processor.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param rxChannelMask Desired ORx signal chains (channel) to power up
*
* \retval ADI_ADRV9025_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_OrxEnableSet(adi_adrv9025_Device_t* device,
                              uint32_t               rxChannelMask);

/**
* \brief This API reads back the Tx channels that are powered up in the
*        transceiver
*
* For use cases where pin mode is not required, this function can be used to
* read the current state of the Tx channel enable (power up) signals.
* This function should be called after initialization and loading the stream
* processor.
*
* \pre This function can be called after initialization.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param txChannelMask Current Tx signal chains enabled returned to this pointer address
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxEnableGet(adi_adrv9025_Device_t* device,
                             uint32_t*              txChannelMask);

/**
* \brief This API reads back the Rx channels that are powered up in the
*        transceiver
*
* For use cases where pin mode is not required, this function can be used to
* read the current state of the Rx channel enable (power up) signals.
* This function should be called after initialization and loading the stream
* processor.
*
* \pre This function can be called after initialization.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param rxChannelMask Current Rx/ORx signal chains enabled returned to this pointer address
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxEnableGet(adi_adrv9025_Device_t* device,
                             uint32_t*              rxChannelMask);

/**
* \brief This API reads back the ORx channels that are powered up in the
*        transceiver
*
* For use cases where pin mode is not required, this function can be used to
* read the current state of the ORx channel enable (power up) signals.
* This function should be called after initialization and loading the stream
* processor.
*
* \pre This function can be called after initialization.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param rxChannelMask Current Rx/ORx signal chains enabled returned to this pointer address
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_OrxEnableGet(adi_adrv9025_Device_t* device,
                              uint32_t*              rxChannelMask);

/**
* \brief This function masks/unmasks the PLL unlock GP interrupt for a requested PLL
*
* When the PLL frequency is being updated, the PLL is unlocked which generates
* a GP interrupt. For use cases where PLL frequency is required to be changed,
* the GP interrupt needs to be masked so that it does not flag a false interrupt.
*
* \pre
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param pllName PLL for which frequency needs to be set
* \param gpInterruptPin0Mask flag to mask(1) / unmask(0) the requested PLL unlock interrupt on Pin0
* \param gpInterruptPin1Mask flag to mask(1) / unmask(0) the requested PLL unlock interrupt on Pin1
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_PllUnlockGpInterruptMaskSet(adi_adrv9025_Device_t* device,
                                             adi_adrv9025_PllName_e pllName,
                                             uint8_t                gpInterruptPin0Mask,
                                             uint8_t                gpInterruptPin1Mask);

/**
* \brief This reads the PLL unlock GP interrupt mask for a requested PLL
*
* This function can be used to retrieve current GP Interrupt mask status
* for the requested PLL.
*
* \pre
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param pllName PLL for which PLL unlock GP Interrupt mask is requested
* \param pllGpInterruptPin0Mask Current mask/unmask status of the requested PLL unlock GP Interrupt on Pin0
* \param pllGpInterruptPin1Mask Current mask/unmask status of the requested PLL unlock GP Interrupt on Pin1
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_PllUnlockGpInterruptMaskGet(adi_adrv9025_Device_t* device,
                                             adi_adrv9025_PllName_e pllName,
                                             uint8_t*               pllGpInterruptPin0Mask,
                                             uint8_t*               pllGpInterruptPin1Mask);

/**
* \brief This function masks/unmasks the PLL overrange GP interrupt for a requested PLL
*
* When the PLL frequency is being updated, the PLL is unlocked which generates
* a GP interrupt. For use cases where PLL frequency is required to be changed,
* the GP interrupt needs to be masked so that it does not flag a false interrupt.
*
* \pre
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param pllName PLL for which frequency needs to be set
* \param gpInterruptPin0Mask flag to mask(1) / unmask(0) the requested PLL unlock interrupt on Pin0
* \param gpInterruptPin1Mask flag to mask(1) / unmask(0) the requested PLL unlock interrupt on Pin1
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_PllOverrangeGpInterruptMaskSet(adi_adrv9025_Device_t* device,
                                                adi_adrv9025_PllName_e pllName,
                                                uint8_t                gpInterruptPin0Mask,
                                                uint8_t                gpInterruptPin1Mask);

/**
* \brief This reads the GP Interrupt overrange mask status
*
* This function can be used to retrieve current GP Interrupt mask status
* for the requested PLL.
*
* \pre
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param pllName PLL for which GP Interrupt overrange mask is requested
* \param pllGpInterruptPin0Mask Current mask/unmask status of the requested PLL unlock GP Interrupt on Pin0
* \param pllGpInterruptPin1Mask Current mask/unmask status of the requested PLL unlock GP Interrupt on Pin1
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_PllOverrangeGpInterruptMaskGet(adi_adrv9025_Device_t* device,
                                                adi_adrv9025_PllName_e pllName,
                                                uint8_t*               pllGpInterruptPin0Mask,
                                                uint8_t*               pllGpInterruptPin1Mask);

/**
* \brief This function sets up the ORx signal chain Radio Ctrl configuration
*
*  The table below outlines the various ORx enable modes and corresponding ORx select mechanisms.
*
*                  ORx PIN Mode     |    ORx Select Mechanism
*      -----------------------------|------------------------------------------------------------------------------------
*                        SPI Mode   | ORx enable and channel select is accomplished through SPI registers
*         Single Channel 3 Pin Mode | ORX_ENABLE[0] is enable/disable ctrl. ORx select is accomplished by ORX_ENABLE[2:1]
*         Single Channel 2 Pin Mode | ORX_ENABLE[0] is enable/disable ctrl. ORx select is accomplished by SPI registers.
*                                   | ORx select is multiplexed between 2 possible ORx channels depending on the level of ORx_ENABLE[1] pin
*         Single Channel 1 Pin Mode | ORX_ENABLE[0] is enable/disable ctrl. ORx select is accomplished by SPI registers.
*         Dual Channel 4 Pin Mode   | ORX_ENABLE[1:0] pins are enable/disable ctrl. ORx select is accomplished by ORX_ENABLE[3:2]
*         Dual Channel 2 Pin Mode   | ORX_ENABLE[1:0] pins are enable/disable ctrl. ORx select is accomplished by SPI registers
*
* \pre This function can be called after initialization and completion of stream loading.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param orxRadioCtrlModeCfg Pointer to ORx radio ctrl mode configuration settings
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_OrxRadioCtrlCfgSet(adi_adrv9025_Device_t*              device,
                                    adi_adrv9025_ORxRadioCtrlModeCfg_t* orxRadioCtrlModeCfg);

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
* \param device Pointer to the ADRV9025 data structure
* \param rxRadioCtrlModeCfg Pointer to ORx radio ctrl mode configuration settings
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxRadioCtrlCfgSet(adi_adrv9025_Device_t*             device,
                                   adi_adrv9025_RxRadioCtrlModeCfg_t* rxRadioCtrlModeCfg);

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
* \param device Pointer to the ADRV9025 data structure
* \param txRadioCtrlModeCfg Pointer to ORx radio ctrl mode configuration settings
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxRadioCtrlCfgSet(adi_adrv9025_Device_t*             device,
                                   adi_adrv9025_TxRadioCtrlModeCfg_t* txRadioCtrlModeCfg);

/**
* \brief This function reads back the ORx signal chain Radio Ctrl configuration
*
* \pre This function can be called after initialization.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param orxRadioCtrlModeCfg Pointer to ORx radio ctrl mode configuration settings
*        which will be updated with read back configuration
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_OrxRadioCtrlCfgGet(adi_adrv9025_Device_t*              device,
                                    adi_adrv9025_ORxRadioCtrlModeCfg_t* orxRadioCtrlModeCfg);

/**
* \brief This function reads back the Rx signal chain Radio Ctrl configuration
*
* \pre This function can be called after initialization.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param rxChannel Desired RxChannel
* \param rxRadioCtrlModeCfg Pointer to Rx radio ctrl mode configuration settings
*        which will be updated with read back configuration
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxRadioCtrlCfgGet(adi_adrv9025_Device_t*             device,
                                   adi_adrv9025_RxChannels_e          rxChannel,
                                   adi_adrv9025_RxRadioCtrlModeCfg_t* rxRadioCtrlModeCfg);

/**
* \brief This function reads back the Tx signal chain Radio Ctrl configuration
*
* \pre This function can be called after initialization.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param txChannel Desired TxChannel
* \param txRadioCtrlModeCfg Pointer to Tx radio ctrl mode configuration settings
*        which will be updated with read back configuration
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxRadioCtrlCfgGet(adi_adrv9025_Device_t*             device,
                                   adi_adrv9025_TxChannels_e          txChannel,
                                   adi_adrv9025_TxRadioCtrlModeCfg_t* txRadioCtrlModeCfg);

/**
* \brief This function attempts to configure the ADRV9025 ARM TDD Control GPIO pin for the requested signal ID
*
* The GPIO configuration includes
*
* \pre This function can be called after ARM binary has been loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param signalId Input parameter to this function representing signal for which GPIO assignment is requested
* \param armGpioPin Pointer to ARM GPIO pin structure requested for configuration
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ArmGpioPinSet(adi_adrv9025_Device_t*        device,
                               uint8_t                       signalId,
                               adi_adrv9025_ArmGpioPinCfg_t* armGpioPin);

/**
* \brief This function attempts to retrieve the ADRV9025 ARM TDD Control GPIO pin for the requested signal ID
*
* \pre This function can be called after ARM binary has been loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param signalId Represents the signal for which GPIO pin config is requested
* \param gpioPinSel Output parameter which is a pointer to a GPIO pin sel enum. The pointer
*        memory will be updated with the GPIO pin assigned to the requested signal ID.
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ArmGpioPinGet(adi_adrv9025_Device_t*     device,
                               uint8_t                    signalId,
                               adi_adrv9025_GpioPinSel_e* gpioPinSel);

/**
* \brief This function commands the ARM to trigger the execution of a stream
*
* \pre This function can be called after the stream binary has been loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param streamId unique stream identification no. of the stream to execute
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_StreamTrigger(adi_adrv9025_Device_t* device,
                               uint8_t                streamId);

/**
* \brief This function configures the stream trigger GPIO pin for the requested stream signal ID
*
* This function assigns the GPIO pin for the requested stream signal, enables the stream GPIO signal
* and sets the direction of the GPIO to input w.r.t ADRV9025
*
* \pre This function can be called after the stream binary has been loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param signalId Represents the stream trigger GPIO signal for which GPIO pin is required to be configured
* \param streamTriggerGpio GPIO pin select to be mapped to stream trigger functionality
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_StreamGpioPinSet(adi_adrv9025_Device_t*    device,
                                  uint8_t                   signalId,
                                  adi_adrv9025_GpioPinSel_e streamTriggerGpio);

/**
* \brief This function retrieves the current GPIO assignments to stream trigger signals
*
* \pre This function can be called after the Stream binary has been loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param signalId Represents the stream trigger GPIO signal for which GPIO pin config is requested
* \param streamTriggerGpio GPIO pin currently assigned to the requested stream trigger signal
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_StreamGpioPinGet(adi_adrv9025_Device_t*     device,
                                  uint8_t                    signalId,
                                  adi_adrv9025_GpioPinSel_e* streamTriggerGpio);

/**
* \brief This function performs range check for adi_adrv9025_StreamGpioConfigSet API params
*
* \pre This function is automatically called as part of adi_adrv9025_StreamGpioConfigSet if
*      ADI_ADRV9025_RADIOCTRL_RANGE_CHECK is set to 1.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param streamGpioPinCfg pointer to structure holding stream GPIO control signals
*
* \retval ADI_ADRV9025_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_StreamGpioConfigSetRangeCheck(adi_adrv9025_Device_t*           device,
                                               adi_adrv9025_StreamGpioPinCfg_t* streamGpioPinCfg);

/**
* \brief This function performs range check for adi_adrv9025_TxToOrxMappingSet API
*
* \pre This function is automatically called as part of adi_adrv9025_TxToOrxMappingSet if
*      ADI_ADRV9025_RADIOCTRL_RANGE_CHECK is set to 1.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param orxChannel Target ORx Front end for Tx-ORx mapping
* \param txChannel Target Tx channel that is routed back into the requested ORx channel
*
* \retval ADI_ADRV9025_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_TxToOrxMappingSetRangeCheck(adi_adrv9025_Device_t*    device,
                                             adi_adrv9025_RxChannels_e orxChannel,
                                             adi_adrv9025_TxChannels_e txChannel);

/**
* \brief This function decodes the encoded Tx-ORx mapping received from ARM
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param encodedTxToORxMapping Encoded Tx-ORx mapping received from ARM
* \param orxChannel ORx channel for which Tx channel is requested
* \param txChannel Tx channel mapped to the requested ORx channel
*
* \retval ADI_ADRV9025_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_OrxMapDecode(adi_adrv9025_Device_t*     device,
                              uint16_t                   encodedTxToORxMapping,
                              adi_adrv9025_RxChannels_e  orxChannel,
                              adi_adrv9025_TxChannels_e* txChannel);

#ifdef __cplusplus
}
#endif

#endif
