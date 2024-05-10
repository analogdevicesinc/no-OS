/**
 * Copyright 2015 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adrv904x_gpio.h
 * \brief Contains ADRV904X gpio related private function prototypes for
 *        adrv904x_gpio.c
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef _ADRV904X_GPIO_H_
#define _ADRV904X_GPIO_H_

#include "adrv904x_gpio_types.h"

#include "adi_adrv904x_error.h"
#include "adi_adrv904x_gpio.h"


/**
* \brief Helper function to retrieve GPIO signal information struct from a lookup table
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  signal Selected Digital GPIO signal
* \param[out] info Returned struct containing Signal Information required for signal routing/checking
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioSignalInfoGet(adi_adrv904x_Device_t* const        device,
                                                            const adi_adrv904x_GpioSignal_e     signal,
                                                            adrv904x_GpioSignalInfo_t* const    info);

/**
* \brief Top Level entry point to private GPIO helper utility to configure a Digital GPIO pin for a signal
*
* Configures all routing for GPIO pin to/from selected signal. Handles all routing types internally, checks 
* for GPIO allocation errors, and allocates GPIO to the feature associated with the signal.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected digital GPIO pin
* \param[in]  signal Selected GPIO signal
* \param[in]  channelMask Channel mask to select the instance(S) of the signal. If not a channel signal, 
*                   set to ADI_ADRV904X_CHOFF = 0U.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioSignalSet(adi_adrv904x_Device_t* const        device,
                                                        const adi_adrv904x_GpioPinSel_e     gpio,
                                                        const adi_adrv904x_GpioSignal_e     signal,
                                                        const uint32_t                      channelMask);

/**
* \brief Top Level entry point to private GPIO helper utility to retrieve the current signal on a Digital GPIO
* 
* Returns the signal/channelMask currently being routed to the GPIO pin.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected digital GPIO pin
* \param[out] signal Returned signal for 
* \param[out] channelMask Returned Channel mask for the current signal, if signal is channel-instanced. 
*                   If not a channel signal, this value will be set to 0U = ADI_ADRV904X_CHOFF.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioSignalGet(adi_adrv904x_Device_t* const        device,
                                                        const adi_adrv904x_GpioPinSel_e     gpio,
                                                        adi_adrv904x_GpioSignal_e* const    signal,
                                                        uint32_t* const                     channelMask);

/**
* \brief Top Level entry point to private GPIO helper utility to release a Digital GPIO from it's current signal
*
* Configures all routing for GPIO pin back to default settings to disconnect selected GPIO to previously 
* connected signal. Deallocates GPIO back for use in Shared Resource Manager.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected digital GPIO pin to release
* \param[in]  signal Selected GPIO signal to release
* \param[in]  channelMask Channel mask to select the instance(S) of the signal to release. If not a channel signal, 
*                   set to ADI_ADRV904X_CHOFF = 0U.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioSignalRelease(adi_adrv904x_Device_t* const        device,
                                                            const adi_adrv904x_GpioPinSel_e     gpio,
                                                            const adi_adrv904x_GpioSignal_e     signal,
                                                            const uint32_t                      channelMask);

/**
* \brief Top Level entry point to private GPIO helper utility to configure an Analog GPIO pin for a signal
*
* Configures all routing for GPIO pin to/from selected signal. Handles are routing types internally, 
* checks for GPIO allocation errors, and allocates GPIO to the feature associated with the signal.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected digital GPIO pin
* \param[in]  signal Selected GPIO signal
* \param[in]  channelMask Channel mask to select the instance(S) of the signal. If not a channel signal, 
*                   set to ADI_ADRV904X_CHOFF = 0U.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioAnalogSignalSet(adi_adrv904x_Device_t* const        device,
                                                              const adi_adrv904x_GpioAnaPinSel_e  gpio,
                                                              const adi_adrv904x_GpioSignal_e     signal,
                                                              const uint32_t                      channelMask);

/**
* \brief Top Level entry point to private GPIO helper utility to retrieve the current signal on an Analog GPIO
* 
* Returns the signal/channelMask currently being routed to the Analog GPIO pin.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected analog GPIO pin
* \param[out] signal Returned signal for 
* \param[out] channelMask Returned Channel mask for the current signal, if signal is channel-instanced. 
*                   If not a channel signal, this value will be set to 0U = ADI_ADRV904X_CHOFF.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioAnalogSignalGet(adi_adrv904x_Device_t* const        device,
                                                              const adi_adrv904x_GpioAnaPinSel_e  gpio,
                                                              adi_adrv904x_GpioSignal_e* const    signal,
                                                              uint32_t* const                     channelMask);


/**
* \brief Top Level entry point to private GPIO helper utility to release an Analog GPIO from it's current signal
*
* Configures all routing for GPIO pin back to default settings to disconnect selected GPIO to previously 
* connected signal. Deallocates GPIO back for use in Shared Resource Manager.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected analog GPIO pin to release
* \param[in]  signal Selected GPIO signal to release
* \param[in]  channelMask Channel mask to select the instance(S) of the signal to release. If not a channel signal, 
*                   set to ADI_ADRV904X_CHOFF = 0U.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioAnalogSignalRelease(adi_adrv904x_Device_t* const        device,
                                                                  const adi_adrv904x_GpioAnaPinSel_e  gpio,
                                                                  const adi_adrv904x_GpioSignal_e     signal,
                                                                  const uint32_t                      channelMask);

/**
 * \brief Private helper function to set the GP Int Type(Category) for all interrupt sources. 
 *
 * This function uses a bitmask word to select the Type: Pulse/Edge triggered (default) vs. Level Triggered. 
 * This function is meant to be called during Initialization to set correct default settings for each source. 
 * These default settings are based on hardware design are meant to be static over all device configurations.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] gpIntType Pointer to data structure containing GP Interrupt word to set interrupt 
 *              source types. Each bit in the word selects the type for the associated interrupt source:
 *              0 = Pulse/Edge triggered (default), 1 = Level Triggered
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpIntTypeSet( adi_adrv904x_Device_t* const            device,
                                                       const adi_adrv904x_GpIntMask_t* const   gpIntType);

/**
 * \brief Private helper function to retrieve the GP Int Type(Category) for all interrupt sources. 
 *
 * This function uses a bitmask word to select the Type: Pulse/Edge triggered (default) vs. Level Triggered. 
 * This function is meant to be called during Initialization to set correct default settings for each source. 
 * These default settings are based on hardware design are meant to be static over all device configurations.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[out] gpIntType Pointer to data structure to contain retrieved GP Interrupt Type word.
 *              Each bit in the word selects the type for the associated interrupt source:
 *              0 = Pulse/Edge triggered (default), 1 = Level Triggered
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpIntTypeGet( adi_adrv904x_Device_t* const        device,
                                                       adi_adrv904x_GpIntMask_t* const     gpIntType);

#ifndef CLIENT_IGNORE
/****************************************************************************
 * Top Level GPIO Utility Functions
 ****************************************************************************
 */

/**
* \brief Top Level entry point to private GPIO helper utility to retrieve the GPIO of a selected signal & channel
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[out] gpio GPIO assignment of selected signal, INVALID if signal isn't mapped to any GPIO
*                         If not a channel signal, this value should be set to 0U = ADI_ADRV904X_CHOFF.
* \param[in] signal Signal to check GPIO mapping
* \param[in] channelSel Channel selection to check GPIO mapping
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioSignalFind(adi_adrv904x_Device_t* const        device,
                                                         adi_adrv904x_GpioPinSel_e* const    gpio,
                                                         const adi_adrv904x_GpioSignal_e     signal,
                                                         const adi_adrv904x_Channels_e       channelSel);



/**
* \brief Top Level entry point to private GPIO helper utility to retrieve the Analog GPIO of a selected signal & channel
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[out] gpio GPIO assignment of selected signal, INVALID if signal isn't mapped to any GPIO
*                         If not a channel signal, this value should be set to 0U = ADI_ADRV904X_CHOFF.
* \param[in] signal Signal to check GPIO mapping
* \param[in] channelSel Channel selection to check GPIO mapping
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioAnalogSignalFind(adi_adrv904x_Device_t* const           device,
                                                               adi_adrv904x_GpioAnaPinSel_e* const    gpio,
                                                               const adi_adrv904x_GpioSignal_e        signal,
                                                               const adi_adrv904x_Channels_e          channelSel);

/**
* \brief High level helper function to check validity of a digital GPIO signal/channel as a Monitor Output Signal.
*
* Returns result in parameter isValid
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  signal Selected GPIO signal to check
* \param[in]  channel Selected channel number to check
* \param[out] isValid Pointer to result of validity check
* \param[out] channelMask Pointer to channelMask conversion of signal/channel dependent on Route type
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioMonitorOutSignalValidCheck(adi_adrv904x_Device_t* const       device,
                                                                         const adi_adrv904x_GpioSignal_e    signal,
                                                                         const uint8_t                      channel,
                                                                         uint8_t* const                     isValidFlag,
                                                                         uint32_t* const                    channelMask);

/****************************************************************************
 * Low Level GPIO Utility Helper Functions
 ****************************************************************************
 */



/**
* \brief Helper function to get the associated Shared Resource Manager FeatureID for a given GPIO signal
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  signal Selected GPIO signal
* \param[out] featureID Returned featureID associated with the GPIO signal
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_FromSignalToFeatureGet(adi_adrv904x_Device_t* const    device,
                                                                 const adi_adrv904x_GpioSignal_e     signal,
                                                                 adrv904x_FeatureID_e* const     featureID);

/**
* \brief Helper function to get the associated GPIO signal for a Shared Resource Manager FeatureID
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  featureID Selected Shared Resource Manager featureID
* \param[out] signal Returned GPIO signal associated with the featureID
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_FromFeatureToSignalGet(adi_adrv904x_Device_t* const    device,
                                                                 const adrv904x_FeatureID_e      featureID,
                                                                 adi_adrv904x_GpioSignal_e* const    signal);


/**
* \brief Helper function to set bitfields for configuring Digital GPIO Pinmux Stage3 (Top-level digital mux)
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected digital GPIO pin
* \param[in]  muxSelect Stage 3 mux selection (16:1 mux)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg3Set(adi_adrv904x_Device_t* const        device,
                                                            const adi_adrv904x_GpioPinSel_e     gpio,
                                                            const uint8_t                       muxSelect);

/**
* \brief Helper function to get bitfields for configuring Digital GPIO Pinmux Stage3 (Top-level digital mux)
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]   gpio Selected digital GPIO pin
* \param[out]  muxSelect Pointer to hold value of muxSelect Stage 3 mux selection (16:1 mux)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg3Get(adi_adrv904x_Device_t* const        device,
                                                            const adi_adrv904x_GpioPinSel_e     gpio,
                                                            uint8_t  * const                    muxSelect);

/**
* \brief Helper function to set bitfields for configuring Digital GPIO Pinmux Stage2_Rx
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected digital GPIO pin
* \param[in]  muxSelect Stage 2 Rx mux selection (8:1 mux)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg2RxSet(adi_adrv904x_Device_t* const        device,
                                                              const adi_adrv904x_GpioPinSel_e     gpio,
                                                              const uint8_t                       muxSelect);



/**
* \brief Helper function to set bitfields for configuring Digital GPIO Pinmux Stage2_Tx
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected digital GPIO pin
* \param[in]  muxSelect Stage 2 Tx mux selection (8:1 mux)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg2TxSet(adi_adrv904x_Device_t* const        device,
                                                              const adi_adrv904x_GpioPinSel_e     gpio,
                                                              const uint8_t                       muxSelect);



/**
* \brief Helper function to set bitfields for configuring Digital GPIO Pinmux Stage2_Orx
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected digital GPIO pin
* \param[in]  muxSelect Stage 2 ORx mux selection (2:1 mux)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg2OrxSet(adi_adrv904x_Device_t* const        device,
                                                               const adi_adrv904x_GpioPinSel_e     gpio,
                                                               const uint8_t                       muxSelect);



/**
* \brief Helper function to set bitfields for configuring Digital GPIO Pinmux Stage2_Actrl (Analog Control Observation Signals)
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected digital GPIO pin
* \param[in]  muxSelect Stage 2 Actrl mux selection (512:1 mux, only 247 inputs are being used currently)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg2ActrlSet(adi_adrv904x_Device_t* const        device,
                                                                 const adi_adrv904x_GpioPinSel_e     gpio,
                                                                 const uint16_t                      muxSelect);

/**
* \brief Helper function to set bitfields for configuring Digital GPIO Pinmux Stage1_Rx
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected digital GPIO pin
* \param[in]  muxSelect Stage 1 Rx mux selection (N:1 mux, 1 output of a N:24 Xbar)
* \param[in] channelIdx The Rx channel index 
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg1RxSet(adi_adrv904x_Device_t* const        device,
                                                              const adi_adrv904x_GpioPinSel_e     gpio,
                                                              const uint8_t                       muxSelect,
                                                              const uint8_t                       channelIdx);

/**
* \brief Helper function to set bitfields for configuring Digital GPIO Pinmux Stage1_Tx
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected digital GPIO pin
* \param[in]  muxSelect Stage 1 Tx mux selection (N:1 mux, 1 output of a N:24 Xbar)
* \param[in] channelIdx The Tx channel index
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg1TxSet(adi_adrv904x_Device_t* const        device,
                                                              const adi_adrv904x_GpioPinSel_e     gpio,
                                                              const uint8_t                       muxSelect,
                                                              const uint8_t                       channelIdx);
       
/**
* \brief Helper function to set bitfields for configuring Digital GPIO Pinmux Stage1_Orx
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected digital GPIO pin
* \param[in]  muxSelect Stage 1 Orx mux selection (N:1 mux, 1 output of a N:24 Xbar)
* \param[in] channelIdx The Orx channel index
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg1OrxSet(adi_adrv904x_Device_t* const        device,
                                                               const adi_adrv904x_GpioPinSel_e     gpio,
                                                               const uint8_t                       muxSelect,
                                                               const uint8_t                       channelIdx);

/**
* \brief Helper function to set IE Override bit for a given GPIO.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected digital GPIO pin
* \param[in]  override IF override = 0, gpio direction is set by the current OE/IE of the signal on Stg3 of pinmux. 
*                      IF override = 1, gpio direction is forced to IE = 1.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioIeOverride(adi_adrv904x_Device_t* const        device,
                                                         const adi_adrv904x_GpioPinSel_e     gpio,
                                                         const uint8_t                       override);

/**
* \brief Helper function to set bitfields for configuring Digital GPIO Rx Destination signals
*
* NOTE: If configuring more than 1 channel for use with the same GPIO, this helper function must be
*       called multiple times: 1 call per channel desired.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  destIdx Index of the selected Rx destination signal
* \param[in]  channelIdx Rx channel index to enable GPIO input for signal
* \param[in]  muxSelect mux selection to make for the destination signal (i.e. select GPIO1-23 or 0 to disable)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioDestRxSet(adi_adrv904x_Device_t* const        device,
                                                        const uint8_t                       destIdx,
                                                        const uint8_t                       channelIdx,
                                                        const uint8_t                       muxSelect);
/**
* \brief Helper function to set bitfields for configuring Digital GPIO Tx Destination signals
*
* NOTE: If configuring more than 1 channel for use with the same GPIO, this helper function must be
*       called multiple times: 1 call per channel desired.
*       
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  destIdx Index of the selected Tx destination signal
* \param[in]  channelIdx Tx channel index to enable GPIO input for signal
* \param[in]  muxSelect mux selection to make for the destination signal (i.e. select GPIO1-23 or 0 to disable)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioDestTxSet(adi_adrv904x_Device_t* const        device,
                                                        const uint8_t                       destIdx,
                                                        const uint8_t                       channelIdx,
                                                        const uint8_t                       muxSelect);


/**
* \brief Helper function to set bitfield for configuring Digital GPIO Stream Trigger Destinations
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected digital GPIO pin
* \param[in]  maskTrigger Stream Trigger Mask selection for the GPIO pin. 0 = Disable GPIO trigger. 1 = Enable GPIO trigger
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioDestStreamTrigSet(adi_adrv904x_Device_t* const       device,
                                                                const adi_adrv904x_GpioPinSel_e    gpio,
                                                                const uint8_t                      maskTrigger);
    
/**
* \brief Check if channelMask is valid for the given signal route. If valid, return ADI_TRUE.Else return ADI_FALSE
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  route The route type of the signal to check channelMask for
* \param[in]  channelMask The channelMask to check
* \param[out] isValid the result of validity check
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioRouteValidChannelMaskCheck(adi_adrv904x_Device_t* const        device,
                                                                         const adrv904x_GpioRoute_e          route,
                                                                         const uint32_t                      channelMask,
                                                                         uint8_t* const                      isValid);


/**
* \brief Helper function to connect or disconnect a signal to/from a GPIO
*
* Configures all routing for GPIO pin to/from selected signal.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  connect Connect/Disconnect selection for function. 0 = Disconnect.1 = Connect
* \param[in]  gpio Selected digital GPIO pin
* \param[in]  sigInfo Selected GPIO signal information struct
* \param[in]  channelIdx Channel index to select the instance(S) of the signal to connect/disconnect. 
*                   If not a channel signal, channelIdx is ignored
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioConnect(adi_adrv904x_Device_t* const            device,
                                                      const uint8_t                           connect,
                                                      const adi_adrv904x_GpioPinSel_e         gpio,
                                                      const adrv904x_GpioSignalInfo_t* const  sigInfo,
                                                      uint32_t const                          channelIdx);

/**
* \brief Helper function to set IE Override bit for a given analog GPIO.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected analog GPIO pin
* \param[in]  override IF override = 0, analog gpio direction is set by the current OE/IE of the signal on Stg3 of pinmux.
*                      IF override = 1, analog gpio direction is forced to IE = 1.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioAnalogIeOverride(	adi_adrv904x_Device_t* const        device,
                                                    			const adi_adrv904x_GpioAnaPinSel_e     gpio,
																const uint8_t                       override);

/**
* \brief Helper function to set bitfields for configuring Analog GPIO Pinmux
* 
* NOTE: Analog pinmux is much simpler than Digital GPIO Pinmux. Only one level and simple selection bitfields per GPIO.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected Analog GPIO pin
* \param[in]  muxSelect Analog mux selection (16:1 mux)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioAnalogPinmuxSet(adi_adrv904x_Device_t* const        device,
                                                              const adi_adrv904x_GpioAnaPinSel_e  gpio,
                                                              const uint8_t                       muxSelect);

/**
* \brief Helper function to get bitfields for configuring Analog GPIO Pinmux
* 
* NOTE: Analog pinmux is much simpler than Digital GPIO Pinmux. Only one level and simple selection bitfields per GPIO.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  gpio Selected Analog GPIO pin
* \param[out]  muxSelect pointer to hold value of Analog mux selection (16:1 mux)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioAnalogPinmuxGet(adi_adrv904x_Device_t* const        device,
                                                              const adi_adrv904x_GpioAnaPinSel_e  gpio,
                                                              uint8_t  * const                    muxSelect);

/**
* \brief Helper function to connect or disconnect a signal to/from an Analog GPIO
*
* Configures all routing for Analog GPIO pin to/from selected signal.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in]  connect Connect/Disconnect selection for function. 0 = Disconnect.1 = Connect
* \param[in]  gpio Selected analog GPIO pin
* \param[in]  sigInfo Selected GPIO signal information struct
* \param[in]  channelIdx Channel index to select the instance(S) of the signal to connect/disconnect. 
*                   If not a channel signal, channelIdx is ignored. Unused parameter
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioAnalogConnect(adi_adrv904x_Device_t* const            device,
                                                            const uint8_t                           connect,
                                                            const adi_adrv904x_GpioAnaPinSel_e      gpio,
                                                            const adrv904x_GpioSignalInfo_t* const  sigInfo,
                                                            uint32_t const                          channelIdx);



/****************************************************************************
 * GP Int (General Purpose Interrupt) related functions
 ***************************************************************************
 */

/**
 * \brief Private utility function intended for use during PreMcsInit and initializes GP interrupt masks for all GP Int Pins.
 *
 * The intent of this function is that it gets called during init time to set the GP interrupt 
 * masks BEFORE main initialization sequence. Masks are applied as selected in init struct.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] pinMaskCfg Pointer to data structure containing GP Interrupt Pin Mask Config to be 
 *              applied to GP Interrupt pins during PreMcsInit
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpIntPreMcsInit( adi_adrv904x_Device_t* const                 device,
                                                           const adi_adrv904x_GpIntPinMaskCfg_t* const  pinMaskCfg);

/**
 * \brief Private utility function intended for use during PostMcsInit and initializes GP interrupt masks for all GP Int Pins.
 *
 * The intent of this function is that it gets called during init time to set the GP interrupt 
 * masks AFTER main initialization sequence. Masks are applied as selected in postMcsInit struct.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] pinMaskCfg Pointer to data structure containing GP Interrupt Pin Mask Config to be 
 *              applied to GP Interrupt pins during PostMcsInit
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpIntPostMcsInit( adi_adrv904x_Device_t* const                device,
                                                            const adi_adrv904x_GpIntPinMaskCfg_t* const pinMaskCfg);

#endif //CLIENT_IGNORE
#endif // ! _ADRV904X_GPIO_H_
