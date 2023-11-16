/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_adrv904x_agc.h
 * \brief Contains ADRV904X receive related function prototypes for
 *        adi_adrv904x_agc.c
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef _ADI_ADRV904X_AGC_H_
#define _ADI_ADRV904X_AGC_H_

#include "adi_adrv904x_agc_types.h"
#include "adi_adrv904x_error.h"


/**
* \brief Function to configure AGC block.
*
* Rx AGC block allows the receive path to control the gain automatically based on feedback from a number
* of signal detectors. There are 2 types of triggers that can be used for automatic Rx gain control.
* Peak detectors (ADC overload detector and HB2 peak detector) and power detector (decimated power
* measurement block). These triggers can be configured to be used to adjust Rx gain automatically.
* 
* ADC overload detector has a configurable low threshold. 
* 
* HB2 peak detector has a high threshold and 3 low thresholds. The highest low threshold is used in all conditions.
* The other two low thresholds can only be used when fast recovery is enabled (agcConfig.agcEnableFastRecoveryLoop).
* 
* All peak detector thresholds have an exceeded peak counter (to trigger a gain change) and gain step
* sizes (configuring how much the gain will be increased/decreased). Additionally, HB2 peak detectors have 
* a common duration (agcConfig.agcPeak.hb2OverloadDurationCount) and a threshold 
* count (agcConfig.agcPeak.hb2OverloadThreshCount).  At least an hb2OverloadThreshCount number of individual samples
* within a batch of hb2OverloadDurationCount samples must exceed an HB2 threshold to increment that threshold's peak 
* counter. This is to avoid counting a single peak multiple times due to high sample rates before decimations in the
* digital datapath.
* 
* Decimated power measurements can be used by AGC to control the gain in Rx datapath. Decimated power measurement block
* in main signal path is being used for this purpose. These measurements are also available for user readback. 
* Power measurements have 4 configurable thresholds and gain steps associated with these thresholds. 
* Please see agcConfig.agcPower structure for details.
*
* Note: The function does not check for duplicated channel assignments across cfg structures.
* Invalid channel mask values will return an error.

* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] agcConfig array of structures of type adi_adrv904x_ AgcCfg_t which will configure one or more channels
* \param[in] numOfAgcCfgs number of configurations passed in the array
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcCfgSet(adi_adrv904x_Device_t * const device,
                                                        const adi_adrv904x_AgcCfg_t agcConfig[],
                                                        const uint32_t numOfAgcCfgs);

/**
* \brief This function reads AGC configuration of selected Rx channel

* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] rxChannel Rx channel selection to read AGC config from
* \param[out] agcConfigReadBack Pointer to structure which will be used to read configuration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcCfgGet(adi_adrv904x_Device_t * const device,
                                                        const adi_adrv904x_RxChannels_e rxChannel,
                                                        adi_adrv904x_AgcCfg_t * const agcConfigReadBack);

/**
* \brief Halt the AGC loop which freezes the AGC gain index at it's current value.
* 
* Can freeze or unfreeze multiple channels in a single call.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] rxChannelMask Bit0 (lsb) relates to Rx0, bit1 to Rx1 etc. If the bit for an
*    Rx channel is not set then that channel is not affected by the call.
* \param[in] freezeEnable Bit0 (lsb) relates to Rx0, bit1 to Rx1 etc. If the channel's bit in
*    rxChannelMask is set then if the channel's bit in freezeEnable is set the AGC loop for
*    the channel is frozen, if the bit is clear then the AGC loop is set to run.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcFreezeSet(adi_adrv904x_Device_t* const device,
                                                           const uint32_t rxChannelMask,
                                                           const uint32_t freezeEnable);
    
/**
* \brief Get the AGC freeze status for all Rx channels.
* 
* This status only applied to Rx channels whose AGC loop is halted by AgcFreezeSet function not
* those which have been halted by GPIO.
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[out] freezeEnable Bit0 (lsb) relates to Rx0, bit1 to Rx1 etc. If the channel's AGC
*    loop is halted by AgcFreezeSet then it's bit is set in freezeEnable. Otherwise it is
*    cleared.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcFreezeGet(adi_adrv904x_Device_t* const device,
                                                           uint32_t* const freezeEnable);

/**
* \brief Allow AGC loop to run or be halted by a GPIO signal.
* 
* A rising edge on the selected digital GPIO will cause the AGC loop to halt thereby fixing the 
* AGC index until such time as there is a falling edge, upon which the AGC loop will continue it's
* normal operation.
* 
* It is not possible to control an Rx AGC loop using GPIO_0. It is possible to control the  AGC
* loop of serveral channels from a single GPIO.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] rxChannels Rx channels
* \param[in] gpioPin The digital GPIO to be used to control AGC loop.
* \param[in] enable Set to ADI_TRUE to setup AGC control by gpioPin for the channels specifed
*    in rxChannels. Set to ADI_FALSE to disable AGC control by GPIO. gpioPin value as no effect
*    in this case.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcFreezeOnGpioSet(adi_adrv904x_Device_t* const device,
                                                                 const uint32_t rxChannels,
                                                                 const adi_adrv904x_GpioPinSel_e gpioPin,
                                                                 const uint8_t enable);
    
/**
* \brief Get the GPIO signal, if any, configured to halt an Rx channel's AGC loop.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] rxChannel The Rx channel whose configuration to get.
* \param[out] gpioPin Is set to the digital GPIO being used to control AGC loop or to ADI_ADRV904X_GPIO_00
*     if AGC loop is not being controlled by any GPIO.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcFreezeOnGpioGet(adi_adrv904x_Device_t* const device,
                                                                 const adi_adrv904x_RxChannels_e rxChannel,
                                                                 adi_adrv904x_GpioPinSel_e* const gpioPin);

/**
* \brief Function to configure min/max gain indices allowed for AGC operation
*
* Min/Max gain indices can also be set with adi_adrv904x_AgcCfgSet() function.
* This function allows user to update min/max gain indices without configuring other AGC related parameters.
* Note:   The function does not check for duplicated channel assignments across cfg structures.
          Invalid channel mask values will return an error.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] agcGainRange array of structures of type adi_adrv904x_ AgcGainRange _t which will configure one or more channels
* \param[in] numOfAgcRangeCfgs number of configurations passed in the array
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcGainIndexRangeSet(adi_adrv904x_Device_t * const device,
                                                                   const adi_adrv904x_AgcGainRange_t agcGainRange[],
                                                                   const uint32_t numOfAgcRangeCfgs);

/**
* \brief Function to read AGC Gain range for selected channel
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] rxChannel Rx channel selection to read AGC gain range configuration
* \param[out] agcGainConfigReadback Pointer to gain range structure to be used to read back configuration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcGainIndexRangeGet(adi_adrv904x_Device_t * const device,
                                                                   const adi_adrv904x_RxChannels_e rxChannel,
                                                                   adi_adrv904x_AgcGainRange_t * const agcGainConfigReadback);


/**
* \brief Function to reset all AGC state machines and peak detector counters for selected channel/s
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] rxChannelMask Mask to select Rx channel/s of which AGC block will be reset
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcReset(adi_adrv904x_Device_t * const device,
                                                       const uint32_t rxChannelMask);


/**
* \brief Programs the gain table settings for dual band external LNA output of selected Rx channels.
*
*
* Dual band LNA gain table has 4 rows to be programmed. User can program all 4 entries or 
* a subset of it with this API function.
*
* For single GPIO (per band) mode, user can configure only first 2 rows of the gain table.
* This mode can be enabled with setting adi_adrv904x_AgcDualBandCfg_t->agcDualBandMaxGainIndex = 1
* For 2 GPIO(per band), user should configure the gain table entries up to (including)
* adi_adrv904x_AgcDualBandCfg_t->agcDualBandMaxGainIndex.
* The gain table configs can be broadcast / multicast based on the channel mask
* parameter, rxChannelMask
*
*|                        rxChannelMask | Rx Channels Programmed                |
*|:------------------------------------:|:-------------------------------------:|
*|                         bit[0] = 1   |  Enables Rx0 gain table programming---|
*|                         bit[1] = 1   |  Enables Rx1 gain table programming---|
*|                         bit[2] = 1   |  Enables Rx2 gain table programming---|
*|                         bit[3] = 1   |  Enables Rx3 gain table programming---|
*|                         bit[4] = 1   |  Enables Rx4 gain table programming---|
*|                         bit[5] = 1   |  Enables Rx5 gain table programming---|
*|                         bit[6] = 1   |  Enables Rx6 gain table programming---|
*|                         bit[7] = 1   |  Enables Rx7 gain table programming---|
*
* Eg: To program the same gain table to channels Rx0 and Rx4, the rxChannelMask
*     should be set to 0x00000011
*
* Eg: To program a single gain table to all channels, the rxChannelMask
*     should be set to 0x000000FF
*
*  Partial gain table loads can be done through this API. User can select gainIndexOffset and arraySize
*  accordingly to write a partial table. This API writes 'arraySize' number of rows
*  up to row 'gainIndexOffset'. For example, when gainIndexOffset=3 and arraySize=2, API loads gainTableRow[0]
*  to row 2 and gainTableRow[1] to row 3.
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in,out] device Context variable - Context variable - Structure pointer to the ADRV904X device data structure
* \param[in] rxChannelMask is the set of channels for which current gain table settings are to be programmed.
*              Valid Rx channels include Rx0-Rx7
* \param[in] gainIndexOffset is the starting gain index from which gain table is programmed (backwards direction). Maximum value is 3.
* \param[in] gainTableRow Array of gain table row entries for programming
* \param[in] arraySize is the number of gain table rows to be programmed. Maximum value is 4
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDualBandLnaGainTableWrite(adi_adrv904x_Device_t* const device,
                                                                          const uint32_t rxChannelMask,
                                                                          const uint8_t gainIndexOffset,
                                                                          const adi_adrv904x_RxDualBandLnaGainTableRow_t gainTableRow[],
                                                                          const uint32_t arraySize);

/**
* \brief Reads the dual band external LNA gain table entries for Rx channels requested.
*
* This function can be called by the user to read back the currently programmed gain table
* for a given channel. This function reads the current gain table settings from ADRV904X gain table Static RAMs
* for the requested channel and stores it in the provided memory reference of type adi_adrv904x_RxDualBandLnaGainTableRow_t
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param[in,out] device Context variable - Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  rxChannelMask represents the channels for which gain table read back is requested.
*             Valid Rx channels include Rx0-Rx7. Multiple channel selections are not allowed.
* \param[in]  gainIndexOffset is the gain index from which gain table read back should start(backwards direction)
* \param[in,out] gainTableRow Read back array for gain table row entries which will be updated with the read back values
* \param[in]  arraySize is the size of gainTableRow array. It is also the upper limit for the no. of gain indices to read
* \param[out] numGainIndicesRead is the actual no. of gain indices read from SRAM (output). A NULL can be passed
*             if the value of no. of gain indices actually read is not required.
*
* \pre This function can be called by the user anytime after initialization. This API reads 'arraySize' number of rows
*  up to row 'gainIndexOffset' row. For example when gainIndexOffset=3 and arraySize=2, API reads row 2 to gainTableRow[0]
*  and row 3 to gainTableRow[1].
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDualBandLnaGainTableRead(adi_adrv904x_Device_t* const device,
                                                                         const uint32_t rxChannelMask,
                                                                         const uint8_t gainIndexOffset,
                                                                         adi_adrv904x_RxDualBandLnaGainTableRow_t gainTableRow[],
                                                                         const uint32_t arraySize,
                                                                         uint8_t* const numGainIndicesRead);


/**
* \brief Function to configure AGC Dual Band block.
*
* Rx Dual band AGC block allows the receive path to control external LNAs for Band A and Band B.
* 16 Analog GPIO pins can be used for this purpose. Each band can use 1 GPIO pin when 8 receivers 
* are being used. Each band can use 2 GPIO pins when 4 receivers are being used. These GPIOs can be
* configured with adi_adrv904x_AgcDualBandGpioCfgSet function.
* A special gain table needs to be written for dual band AGC functionality by using adi_adrv904x_RxDualBandLnaGainTableWrite.
* This table has 4 rows, each having 2 bit external LNA control words to control 2 GPIOs at maximum.
* 
* For dual band AGC, AGC block should also be configured and running with main path decimated power level detector
* enabled.
* 
*
* Note: The function does not check for duplicated channel assignments across cfg structures.
* Invalid channel mask values will return an error.

* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] agcDualBandConfig array of structures of type adi_adrv904x_AgcDualBandCfg_t which will configure one or more channels
* \param[in] numOfAgcDualBandCfgs number of configurations passed in the array
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcDualBandCfgSet(adi_adrv904x_Device_t * const device,
                                                                const adi_adrv904x_AgcDualBandCfg_t agcDualBandConfig[],
                                                                const uint32_t numOfAgcDualBandCfgs);

/**
* \brief This function reads AGC dual band configuration of selected Rx channel
* Rx channel requested should be selected with adi_adrv904x_AgcDualBandCfg_t->rxChannelMask.
* Multiple channel selection isn't allowed

* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in,out] agcDualBandConfigReadBack Pointer to structure which will be used to read configuration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcDualBandCfgGet(adi_adrv904x_Device_t * const device,
                                                                adi_adrv904x_AgcDualBandCfg_t * const agcDualBandConfigReadBack);

    
/**
* \brief Function to configure AGC Dual Band GPIO mapping.
*
* 16 analog GPIOs can be mapped to various external lna outputs as follows,
* 
*|  GPIO Analog Output |                                     Allowed Signals                                         |
*|:-------------------:|:-------------------------------------------------------------------------------------------:|
*|   ANALOG_GPIO_0     |   ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND0_0 or ADI_ADRV904X_GPIO_SIGNAL_RX2_DUALBAND_CONTROL_BAND0_0  |
*|   ANALOG_GPIO_1     |   ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND0_1 or ADI_ADRV904X_GPIO_SIGNAL_RX2_DUALBAND_CONTROL_BAND0_1  |
*|   ANALOG_GPIO_2     |   ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND1_0 or ADI_ADRV904X_GPIO_SIGNAL_RX2_DUALBAND_CONTROL_BAND1_0  |
*|   ANALOG_GPIO_3     |   ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND1_1 or ADI_ADRV904X_GPIO_SIGNAL_RX2_DUALBAND_CONTROL_BAND1_1  |
*|   ANALOG_GPIO_4     |   ADI_ADRV904X_GPIO_SIGNAL_RX1_DUALBAND_CONTROL_BAND0_0 or ADI_ADRV904X_GPIO_SIGNAL_RX3_DUALBAND_CONTROL_BAND0_0  |
*|   ANALOG_GPIO_5     |   ADI_ADRV904X_GPIO_SIGNAL_RX1_DUALBAND_CONTROL_BAND0_1 or ADI_ADRV904X_GPIO_SIGNAL_RX3_DUALBAND_CONTROL_BAND0_1  |
*|   ANALOG_GPIO_6     |   ADI_ADRV904X_GPIO_SIGNAL_RX1_DUALBAND_CONTROL_BAND1_0 or ADI_ADRV904X_GPIO_SIGNAL_RX3_DUALBAND_CONTROL_BAND1_0  |
*|   ANALOG_GPIO_7     |   ADI_ADRV904X_GPIO_SIGNAL_RX1_DUALBAND_CONTROL_BAND1_1 or ADI_ADRV904X_GPIO_SIGNAL_RX3_DUALBAND_CONTROL_BAND1_1  |
*|   ANALOG_GPIO_8     |   ADI_ADRV904X_GPIO_SIGNAL_RX4_DUALBAND_CONTROL_BAND0_0 or ADI_ADRV904X_GPIO_SIGNAL_RX6_DUALBAND_CONTROL_BAND0_0  |
*|   ANALOG_GPIO_9     |   ADI_ADRV904X_GPIO_SIGNAL_RX4_DUALBAND_CONTROL_BAND0_1 or ADI_ADRV904X_GPIO_SIGNAL_RX6_DUALBAND_CONTROL_BAND0_1  |
*|   ANALOG_GPIO_10    |   ADI_ADRV904X_GPIO_SIGNAL_RX4_DUALBAND_CONTROL_BAND1_0 or ADI_ADRV904X_GPIO_SIGNAL_RX6_DUALBAND_CONTROL_BAND1_0  |
*|   ANALOG_GPIO_11    |   ADI_ADRV904X_GPIO_SIGNAL_RX4_DUALBAND_CONTROL_BAND1_1 or ADI_ADRV904X_GPIO_SIGNAL_RX6_DUALBAND_CONTROL_BAND1_1  |
*|   ANALOG_GPIO_12    |   ADI_ADRV904X_GPIO_SIGNAL_RX5_DUALBAND_CONTROL_BAND0_0 or ADI_ADRV904X_GPIO_SIGNAL_RX7_DUALBAND_CONTROL_BAND0_0  |
*|   ANALOG_GPIO_13    |   ADI_ADRV904X_GPIO_SIGNAL_RX5_DUALBAND_CONTROL_BAND0_1 or ADI_ADRV904X_GPIO_SIGNAL_RX7_DUALBAND_CONTROL_BAND0_1  |
*|   ANALOG_GPIO_14    |   ADI_ADRV904X_GPIO_SIGNAL_RX5_DUALBAND_CONTROL_BAND1_0 or ADI_ADRV904X_GPIO_SIGNAL_RX7_DUALBAND_CONTROL_BAND1_0  |
*|   ANALOG_GPIO_15    |   ADI_ADRV904X_GPIO_SIGNAL_RX5_DUALBAND_CONTROL_BAND1_1 or ADI_ADRV904X_GPIO_SIGNAL_RX7_DUALBAND_CONTROL_BAND1_1  |
* 
* where 
* ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND0_0: Rx0 Band0 Ext LNA output word LSB
* ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND0_1: Rx0 Band0 Ext LNA output word MSB
* User can select the GPIOs desired to be configured through adi_adrv904x_AgcDualBandGpioCfg_t->gpioSelectionMask.
* API will skip configuring unselected GPIOs and analogGpioMapping[*] value for those GPIOs is a 'Don't care'.
* To release a selected GPIO, user can select ADI_ADRV904X_GPIO_SIGNAL_UNUSED.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] agcDualBandGpioConfig Dual band GPIO configuration structure
*
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcDualBandGpioCfgSet(adi_adrv904x_Device_t * const device,
                                                                    const adi_adrv904x_AgcDualBandGpioCfg_t * agcDualBandGpioConfig);

/**
* \brief Function to read back AGC Dual Band GPIO mapping configuration.
* User can select the GPIOs desired to be readback through adi_adrv904x_AgcDualBandGpioCfg_t->gpioSelectionMask.
* This API will set the unselected GPIOs to ADI_ADRV904X_GPIO_SIGNAL_INVALID.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in,out] agcDualBandGpioConfigReadBack Dual band GPIO configuration structure
*
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcDualBandGpioCfgGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_AgcDualBandGpioCfg_t* const agcDualBandGpioConfigReadBack);

/**
* \brief Function to configure AGC GPIO resync functionality. This feature allows user to configure
* a GPIO to reset the gain update counter. In AGC holdover mode, adi_adrv904x_AgcCfg_t ->agcResetOnRxon = 0,
* AGC counters only reset when gain update counter expires. Gain update counter doesn't realign with TDD timing,
* and resync feature allows pulses from selected GPIO to resync gain update counter with TDD timing.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] rxChannelMask Rx channel mask to select the Rx channels to configure. Multiple channels can be selected
* \param[in] gpioSelection GPIO selection for resync feature to be configured for selected channels. ADI_ADRV904X_GPIO_INVALID
*                          can be selected to release a currently configured GPIO from selected channels.
*                          ADI_ADRV904X_GPIO_00 cannot be used for this feature.
* \param[in] enable 0:Disable resync feature, 1:Enable resync feature for selected channel/s
* 
*
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcGpioReSyncSet(adi_adrv904x_Device_t * const device,
                                                               const uint32_t rxChannelMask,
                                                               const adi_adrv904x_GpioPinSel_e gpioSelection,
                                                               const uint8_t enable);

/**
* \brief Function to read AGC resync functionality. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] rxChannelMask Rx channel mask to select the Rx channels to configure. Multiple channels cannot be selected.
* \param[out] gpioSelection GPIO selection readback for selected channel. ADI_ADRV904X_GPIO_INVALID is returned if selected
*                           channel doesn't use resync functionality.
* \param[out] enable Readback value of resync enable status. 0:Resync feature is disabled, 1:Resync feature is enabled
* 
*
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcGpioReSyncGet(adi_adrv904x_Device_t * const device,
                                                               const uint32_t rxChannelMask,
                                                               adi_adrv904x_GpioPinSel_e * const gpioSelection,
                                                               uint8_t * const enable);


/**
* \brief Function to read active external LNA gain control word for Band A and Band B. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] rxChannel Rx channel selection to read band A/B active external lna control word
* \param[out] bandAExternalLnaGainWord Pointer to readback band A external lna control word
* \param[out] bandBExternalLnaGainWord Pointer to readback band B external lna control word
* 
*
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcDualBandActiveExternalLnaGainWordGet(adi_adrv904x_Device_t * const device,
                                                                                      const adi_adrv904x_RxChannels_e rxChannel,
                                                                                      uint8_t * const bandAExternalLnaGainWord,
                                                                                      uint8_t * const bandBExternalLnaGainWord);

/**
* \brief Function to read if the agc upper level was exceeded for all channels. The functions returns a byte
* that each bit represents a channel, from LSB being channel 0 and MSB being channel 7. Bit HIGH means the upper 
* level was exceeded, and LOW it hasn't.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[out] agcULBlockerBitMask Pointer to agc_ul_blocker which bit 0 contains bitfield for channel 0, bit 1 for ch 1, and so on.
*                                 Bit HIGH means the upper level was exceeded, and LOW it hasn't.
*
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcUpperLevelBlockerGet(adi_adrv904x_Device_t * const device,
                                                                      uint8_t * const               agcULBlockerBitMask);

/**
* \brief Function to read if the agc lower level was exceeded for all channels. The functions returns a byte
* that each bit represents a channel, from LSB being channel 0 and MSB being channel 7. Bit HIGH means the lower 
* level was exceeded, and LOW it hasn't.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[out] agcLLBlockerBitMask Pointer to agc_ll_blocker which bit 0 contains bitfield for channel 0, bit 1 for ch 1, and so on.
*                                 Bit HIGH means the lower level was exceeded, and LOW it hasn't.
*
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcLowerLevelBlockerGet(adi_adrv904x_Device_t * const device,
                                                                      uint8_t * const               agcLLBlockerBitMask);

/**
* \brief Function to read if the high threshold peak detector at the ADC or HB2 output was exceeded for all channels. 
* The functions returns a byte that each bit represents a channel, from LSB being channel 0 and MSB being channel 7. 
* Bit HIGH means the upper level was exceeded, and LOW it hasn't.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[out] thresholdPeakDetectorBitMask Pointer to byte which bit 0 contains bitfield for channel 0, bit 1 for ch 1, and so on.
*                                          Bit HIGH means the high threshold peak detector at ADC or HB2 was exceeded, and LOW it hasn't.
*
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcHighThresholdPeakDetectorGet(adi_adrv904x_Device_t * const device,
                                                                              uint8_t * const               thresholdPeakDetectorBitMask);

/**
* \brief Function to read if the low threshold peak detector at the ADC or HB2 output was exceeded for all channels. 
* The functions returns a byte that each bit represents a channel, from LSB being channel 0 and MSB being channel 7. 
* Bit HIGH means the lower level was exceeded, and LOW it hasn't.
*
*When high, this bit indicates that the high threshold peak detector at the Rx1 ADC or HB2 output was exceeded. 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[out] thresholdPeakDetectorBitMask Pointer to byte which bit 0 contains bitfield for channel 0, bit 1 for ch 1, and so on.
*                                          Bit HIGH means the low threshold peak detector at ADC or HB2 was exceeded, and LOW it hasn't.
*
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcLowThresholdPeakDetectorGet(adi_adrv904x_Device_t * const device,
                                                                              uint8_t * const              thresholdPeakDetectorBitMask);


#endif /* _ADI_ADRV904X_AGC_H_ */
