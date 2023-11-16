/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_adrv904x_gpio.h
 * \brief Contains ADRV904X GPIO related function prototypes for
 *        adi_adrv904x_gpio.c
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef _ADI_ADRV904X_GPIO_H_
#define _ADI_ADRV904X_GPIO_H_

#include "adi_adrv904x_gpio_types.h"
#include "adi_adrv904x_error.h"

/****************************************************************************
 * GPIO related functions
 ****************************************************************************
 */
/**
* \brief This API function set all digital GPIO to function as input pins.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioForceHiZAllPins(adi_adrv904x_Device_t* const 	device);

/**
* \brief This API function set a given digital GPIO to be an input or output.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in]  gpio Selected digital GPIO pin
* \param[in]  oRide IF override = 0, gpio direction is set by the current OE/IE of the signal on Stg3 of pinmux.
*                      IF override = 1, gpio direction is forced to IE = 1.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioForceHiZ(adi_adrv904x_Device_t* const        device,
                                                             const adi_adrv904x_GpioPinSel_e     gpio,
                                                             const uint8_t                       oRide);

/**
 * \brief This API function returns the full status of all device GPIOs
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[out] status Full status readback for all Digital and Analog GPIO pins of the device
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioStatusRead(   adi_adrv904x_Device_t* const        device,
                                                                adi_adrv904x_GpioStatus_t* const    status);


/**
 * \brief This API function returns the currently routed signal/channelMask
 * for a selected digital GPIO pin.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] gpio The digital GPIO pin for which to readback the current configuration
 * \param[out] signal Signal currently configured for selected digital gpio
 * \param[out] channelMask Channel mask currently configured for selected digital gpio
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioConfigGet(adi_adrv904x_Device_t* const        device,
                                                            const adi_adrv904x_GpioPinSel_e     gpio,
                                                            adi_adrv904x_GpioSignal_e* const    signal,
                                                            uint32_t* const                     channelMask);

/**
 * \brief This API function returns the currently routed signal/channelMask
 * for all 24 digital GPIO pins.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[out] signalArray Array of signals currently configured for each digital GPIO
 * \param[out] channelMaskArray Array of channelMasks currently configured for each digital GPIO
 * \param[in] arraySize Size of user allocated arrays to hold returned signals and channel masks. 
                Must be at least 24. If more than 24, the remaining elements will be untouched.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioConfigAllGet(adi_adrv904x_Device_t* const    device,
                                                               adi_adrv904x_GpioSignal_e       signalArray[],
                                                               uint32_t                        channelMaskArray[],
                                                               const uint32_t                  arraySize);

/**
 * \brief This API function returns the currently routed signal/channelMask
 * for a selected Analog GPIO pin.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] gpio The analog GPIO pin for which to readback the current configuration
 * \param[out] signal Signal currently configured for selected analog gpio
 * \param[out] channelMask Channel mask currently configured for selected analog gpio
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogConfigGet(adi_adrv904x_Device_t* const        device,
                                                                  const adi_adrv904x_GpioAnaPinSel_e  gpio,
                                                                  adi_adrv904x_GpioSignal_e* const    signal,
                                                                  uint32_t* const                     channelMask);

/**
 * \brief This API function returns the currently routed signal/channelMask
 * for all 16 Analog GPIO pins.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[out] signalArray Array of signals currently configured for each analog GPIO
 * \param[out] channelMaskArray Array of channelMasks currently configured for each analog GPIO
 * \param[in] arraySize Size of user allocated arrays to hold returned signals and channel masks. 
                Must be at least 16. If more than 16, the remaining elements will be untouched.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogConfigAllGet(adi_adrv904x_Device_t* const    device,
                                                                     adi_adrv904x_GpioSignal_e       signalArray[],
                                                                     uint32_t                        channelMaskArray[],
                                                                     const uint32_t                  arraySize);

/**
 * \brief This API function configures a monitor output function for a GPIO.
 *
 * The monitor outputs allow visibility to some internal ADRV904X signals. Any 
 * monitor output signal is available on any of the low voltage GPIO[23:0] pins.
 *
 * If selected signal is ADI_ADRV904X_GPIO_SIGNAL_UNUSED, this function will automatically
 * try to release the GPIO pin from a currently routed Monitor Output Signal. This is
 * equivalent to calling adi_adrv904x_GpioMonitorOutRelease.
 * Else If selected signal is not a monitor output signal, an error will be returned.
 * 
 * If selected signal is not from a Rx/Tx/ORx channel, channel selection will be
 * ignored and not range checked.
 * If selected signal is from a Rx/Tx channel, channel selection must be in
 * in the range 0 to 7. Otherwise an error will be returned.
 * If selected signal is from a ORx channel, channel selection must be in
 * in the range 0 to 1. Otherwise an error will be returned.
 *
 * \pre This function may be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] gpio GPIO pin to configure as a Monitor Output
 * \param[in] signal Monitor Output signal to observe at selected GPIO
 * \param[in] channel Selection of Rx/Tx/ORx channel instance of monitor output signal to observe, if applicable
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioMonitorOutSet(adi_adrv904x_Device_t* const        device,
                                                                const adi_adrv904x_GpioPinSel_e     gpio,
                                                                const adi_adrv904x_GpioSignal_e     signal,
                                                                const uint8_t                       channel);

/**
 * \brief This API function releases a GPIO if it is currently routing a monitor output function.
 *
 * The monitor outputs allow visibility to some internal ADRV904X signals. Any 
 * monitor output signal is available on any of the low voltage GPIO[23:0] pins.
 *
 * If GPIO pin is not routing any signal (signal == ADI_ADRV904X_GPIO_SIGNAL_UNUSED), 
 * then nothing will change and the function will return successfully. 
 * If GPIO pin is currently routing a GPIO Monitor Output signal, it will be released,
 * the signal will be returned to ADI_ADRV904X_GPIO_SIGNAL_UNUSED, and the function will
 * return successfully.
 * If GPIO pin is currently routing another type of GPIO signal, then the GPIO will not be
 * released and the function will return an error.
 *
 * \pre This function may be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] gpio GPIO pin to release and disconnect from a Monitor Output function.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioMonitorOutRelease(adi_adrv904x_Device_t* const        device,
                                                                    const adi_adrv904x_GpioPinSel_e     gpio);

/**
 * \brief Allocates and configures selected digital GPIO pins for Manual Input Mode.
 *
 * A bitmask is used to enable/disable selected GPIO pins. Note that selected pins (pins included in the bitmask)
 * must be unallocated before enabling with this function. Otherwise an error is returned. Also note that de-selected pins
 * (pins not included in the bitmask) will be checked for current allocation. Those pins that are currently allocated as
 * Manual Inputs will be de-allocated. Pins allocated for other features will not be modified.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] gpioInputMask Bitmask to enable Manual Input mode for Digital GPIO pins. 
 *                      Mask bit0 represents GPIO[0], bit23 represents GPIO[23].
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioManualInputDirSet(    adi_adrv904x_Device_t* const    device,
                                                                        const uint32_t                  gpioInputMask);

/**
 * \brief Allocates and configures selected digital GPIO pins for Manual Output Mode.
 *
 * A bitmask is used to enable/disable selected GPIO pins. Note that selected pins (pins included in the bitmask)
 * must be unallocated before enabling with this function. Otherwise an error is returned. Also note that de-selected pins
 * (pins not included in the bitmask) will be checked for current allocation. Those pins that are currently allocated as
 * Manual Outputs will be de-allocated. Pins allocated for other features will not be modified.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] gpioOutputMask Bitmask to enable Manual Output mode for Digital GPIO pins. 
 *                          Mask bit0 represents GPIO[0], bit23 represents GPIO[23].
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioManualOutputDirSet(   adi_adrv904x_Device_t* const    device,
                                                                        const uint32_t                  gpioOutputMask);

/**
 * \brief Retrieves input read levels for all digital GPIO pins configured as Manual Inputs.
 *
 * Bits in gpioInPinLevel are only valid for Digital GPIO pins configured as Manual Input. Ignore bits for all other GPIOs.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[out] gpioInPinLevel Pin levels retrieved for Digital GPIO pins configured as Manual Inputs
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioManualInputPinLevelGet(   adi_adrv904x_Device_t* const    device,
                                                                            uint32_t * const                gpioInPinLevel);
/**
 * \brief Retrieves output drive levels for all digital GPIO pins configured as Manual Outputs.
 *
 * Bits in gpioOutPinLevel are only valid for Digital GPIO pins configured as Manual Outputs. Ignore bits for all other GPIOs.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[out] gpioOutPinLevel Pin levels retrieved for Digital GPIO pins configured as Manual Outputs
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioManualOutputPinLevelGet(  adi_adrv904x_Device_t* const    device,
                                                                            uint32_t * const                gpioOutPinLevel);

/**
 * \brief Sets output drive levels for all digital GPIO pins configured as Manual Outputs.
 *
 * Bits in gpioOutPinLevel are only valid for Digital GPIO pins configured as Manual Outputs. 
 * Bits for all other GPIOs are ignored by this function.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] gpioPinMask Pinmask to select which Digital GPIO pins will have levels set by gpioOutPinLevel
 * \param[in] gpioOutPinLevel Pin levels to be set for Digital GPIO pins configured as Manual Outputs
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioManualOutputPinLevelSet(  adi_adrv904x_Device_t* const    device,
                                                                            const uint32_t                  gpioPinMask,
                                                                            const uint32_t                  gpioOutPinLevel);

/**
* \brief This API function set all analog GPIO to be input pins.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogForceHiZAllPins(adi_adrv904x_Device_t* const 	device);

/**
* \brief This API function set a given analog GPIO to be an input or output.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in]  gpio Selected analog GPIO pin
* \param[in]  oRide IF override = 0, gpio direction is set by the current OE/IE of the signal on Stg1 of pinmux.
*                   IF override = 1, gpio direction is forced to IE = 1.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogForceHiZ(	adi_adrv904x_Device_t* const        	device,
                                                             	 	 const adi_adrv904x_GpioAnaPinSel_e     gpio,
																	 const uint8_t                       	oRide);

/**
 * \brief Allocates and configures selected analog GPIO pins for Manual Input Mode.
 *
 * A bitmask is used to enable/disable selected GPIO pins. Note that selected pins (pins included in the bitmask)
 * must be unallocated before enabling with this function. Otherwise an error is returned. Also note that de-selected pins
 * (pins not included in the bitmask) will be checked for current allocation. Those pins that are currently allocated as
 * Manual Inputs will be de-allocated. Pins allocated for other features will not be modified.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] gpioAnalogInputMask Bitmask to enable Manual Input mode for Analog GPIO pins. 
 *                      Mask bit0 represents GPIO[0], bit15 represents GPIO[15].
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogManualInputDirSet(  adi_adrv904x_Device_t* const    device,
                                                                            const uint16_t                  gpioAnalogInputMask);

/**
 * \brief Allocates and configures selected analog GPIO pins for Manual Output Mode.
 *
 * A bitmask is used to enable/disable selected GPIO pins. Note that selected pins (pins included in the bitmask)
 * must be unallocated before enabling with this function. Otherwise an error is returned. Also note that de-selected pins
 * (pins not included in the bitmask) will be checked for current allocation. Those pins that are currently allocated as
 * Manual Outputs will be de-allocated. Pins allocated for other features will not be modified.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] gpioAnalogOutputMask Bitmask to enable Manual Output mode for Analog GPIO pins. 
 *                          Mask bit0 represents GPIO[0], bit15 represents GPIO[15].
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogManualOutputDirSet( adi_adrv904x_Device_t* const    device,
                                                                            const uint16_t                  gpioAnalogOutputMask);

/**
 * \brief Retrieves input read levels for all analog GPIO pins configured as Manual Inputs.
 *
 * Bits in gpioInPinLevel are only valid for Analog GPIO pins configured as Manual Input. Ignore bits for all other GPIOs.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[out] gpioAnalogInPinLevel Pin levels retrieved for Analog GPIO pins configured as Manual Inputs
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogManualInputPinLevelGet( adi_adrv904x_Device_t* const    device,
                                                                                uint16_t * const                gpioAnalogInPinLevel);

/**
 * \brief Retrieves output drive levels for all analog GPIO pins configured as Manual Outputs.
 *
 * Bits in gpioOutPinLevel are only valid for Analog GPIO pins configured as Manual Outputs. Ignore bits for all other GPIOs.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[out] gpioAnalogOutPinLevel Pin levels retrieved for Analog GPIO pins configured as Manual Outputs
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogManualOutputPinLevelGet(    adi_adrv904x_Device_t* const    device,
                                                                                    uint16_t * const                gpioAnalogOutPinLevel);

/**
 * \brief Sets output drive levels for all analog GPIO pins configured as Manual Outputs.
 *
 * Bits in gpioOutPinLevel are only valid for Analog GPIO pins configured as Manual Outputs. 
 * Bits for all other GPIOs are ignored by this function.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] gpioAnalogPinMask Pinmask to select which Analog GPIO pins will have levels set by gpioAnalogOutPinLevel
 * \param[in] gpioAnalogOutPinLevel Pin levels to be set for Analog GPIO pins configured as Manual Outputs
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogManualOutputPinLevelSet(    adi_adrv904x_Device_t* const    device,
                                                                                    const uint16_t                  gpioAnalogPinMask,
                                                                                    const uint16_t                  gpioAnalogOutPinLevel);   


/****************************************************************************
 * GP Int (General Purpose Interrupt) related functions
 ***************************************************************************
 */

/**
 * \brief Sets the General Purpose (GP) Interrupt Pin Mask Config for GP Int pins: GP_INT0, GP_INT1, or both.
 * Note: It is the GP interrupts corresponding to the CLEARED bit positions in pinMaskCfg.gpIntXMask that
 * will cause the external GP_INTx signals to be asserted. E.g. setting all valid bits in that field to 1's
 * ensures the corresponding GP_INT signal can never be asserted.
 *
 * GP Interrupt Bit
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] pinSelect Enum indicating the which GP Interrupt pin masks to set
 * \param[in] pinMaskCfg Pointer to data structure holding the GP Interrupt Pin Mask Config to write
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpIntPinMaskCfgSet(   adi_adrv904x_Device_t* const                device,
                                                                    const adi_adrv904x_GpIntPinSelect_e         pinSelect,
                                                                    const adi_adrv904x_GpIntPinMaskCfg_t* const pinMaskCfg);

/**
 * \brief Retrieves the General Purpose (GP) Interrupt Pin Mask Config for GP Int pins: GP_INT0, GP_INT1, or both.
 * Note: It is the GP interrupts corresponding to the CLEARED bit positions in returned pinMaskCfg.gpIntXMask that
 * will cause the external GP_INTx signals to be asserted. If all valid bits in that field are returned as 1's
 * the corresponding GP_INT signal could never be asserted.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] pinSelect Enum indicating the which GP Interrupt pin masks to retrieve
 * \param[out] pinMaskCfg Pointer to data structure to contain the GP Interrupt Pin Mask Config retrieved
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpIntPinMaskCfgGet(   adi_adrv904x_Device_t* const            device,
                                                                    const adi_adrv904x_GpIntPinSelect_e     pinSelect,
                                                                    adi_adrv904x_GpIntPinMaskCfg_t* const   pinMaskCfg);

/**
 * \brief Reads the General Purpose (GP) Interrupt status bits and can be used to determine what caused a GP Interrupt pin to be asserted.
 *
 * The status word read-back will show the current value for all interrupt sources, even if they
 * are disabled by the mask. However, the GP Interrupt pin will only assert for the enabled sources.
 *
 * NOTE: This function DOES NOT clear any interrupt status bits.
 *
 * If a BBIC detects a rising edge on either General Purpose Interrupt pins GP_INT0 or GP_INT1, this function 
 * allows the BBIC to determine the source of the interrupt.  The value returned in the status parameter will show 
 * one or more sources for the interrupt based on the bit positioning table found in documentation for adi_adrv904x_GpIntWord_t.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[out] gpIntStatus Pointer to data structure to contain the GP Interrupt Status word retrieved
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpIntStatusGet(   adi_adrv904x_Device_t* const        device,
                                                                adi_adrv904x_GpIntMask_t* const     gpIntStatus);

/**
 * \brief Clears the General Purpose (GP) Interrupt status bits selected in the gpIntClear word.
 *
 * Note: The only status bits that can be cleared with this function are those from interrupt 
 * sources that are Pulse triggered. Those status bits that are Level triggered must be cleared 
 * in the sub-system in which the interrupt originated.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] gpIntClear Pointer to data structure containing GP Interrupt Clear word selecting the 
 *              status bits to clear. User can pass NULL if they want to clear all status bits. 
 *              Otherwise, a structure can be passed here that manually selects which status bits 
 *              to clear while leaving the others untouched.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpIntStatusClear( adi_adrv904x_Device_t* const            device,
                                                                const adi_adrv904x_GpIntMask_t* const   gpIntClear);

/**
 * \brief Sets the General Purpose (GP) Int Type(Category) for all interrupt sources. 
 * This function uses a bitmask word to select the Type: Pulse/Edge triggered (default) vs. Level Triggered.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] gpIntStickyBitMask Pointer to data structure containing GP Interrupt word to set interrupt 
 *              source types. Each bit in the word selects the type for the associated interrupt source:
 *              0 = Pulse/Edge triggered (default), 1 = Level Triggered
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpIntStickyBitMaskSet(adi_adrv904x_Device_t* const            device,
                                                                    const adi_adrv904x_GpIntMask_t* const   gpIntStickyBitMask);
/**
 * \brief Retrieves the General Purpose (GP) Int Type(Category) for all interrupt sources. 
 * This function uses a bitmask word to select the Type: Pulse/Edge triggered (default) vs. Level Triggered. 
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[out] gpIntStickyBitMask Pointer to data structure to contain retrieved GP Interrupt Type word.
 *              Each bit in the word selects the type for the associated interrupt source:
 *              0 = Pulse/Edge triggered (default), 1 = Level Triggered
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpIntStickyBitMaskGet(adi_adrv904x_Device_t* const    device,
                                                                    adi_adrv904x_GpIntMask_t* const gpIntStickyBitMask);

/**
 * \brief Configures Hysteresis for a given Pin
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] pinName Pin to Configure
 * \param[in] enable Enable Flag
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioHysteresisSet(adi_adrv904x_Device_t* const            device,
                                                                const adi_adrv904x_GpioDigitalPin_e     pinName,
                                                                const uint32_t                          enable);

/**
 * \brief Gets Hysteresis Status  for a given Pin
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] pinName Pin to Configure
 * \param[out] enable Enable Flag
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioHysteresisGet(adi_adrv904x_Device_t* const            device,
                                                                const adi_adrv904x_GpioDigitalPin_e     pinName,
                                                                uint32_t* const                         enable);

/**
 * \brief Sets Drive Strength Status for a given Pin
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] pinName Pin to Configure
 * \param[in] driveStrength Drive Strength State
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioDriveStrengthSet( adi_adrv904x_Device_t* const            device,
                                                                    const adi_adrv904x_GpioDigitalPin_e     pinName,
                                                                    const adi_adrv904x_CmosPadDrvStr_e      driveStrength);


/**
 * \brief Gets Drive Strength Status for a given Pin
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] pinName Pin to Configure
 * \param[out] driveStrength Drive Strength State
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioDriveStrengthGet( adi_adrv904x_Device_t* const            device,
                                                                    const adi_adrv904x_GpioDigitalPin_e     pinName,
                                                                    adi_adrv904x_CmosPadDrvStr_e* const     driveStrength);

#endif /* _ADI_ADRV904X_GPIO_H_ */
