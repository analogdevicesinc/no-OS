/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adi_adrv9025_gpio.h
 * \brief ADRV9025 GPIO header file
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_GPIO_H_
#define _ADI_ADRV9025_GPIO_H_

#include "adi_adrv9025_types.h"
#include "adi_adrv9025_gpio_types.h"
#include "adi_common_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Sets the ADRV9025 low voltage GPIO pin directions
 * \brief Not currently implemented
 *
 * This function will set the GPIO direction given by the passed parameter.
 * The direction can be either output or input per pin. The gpioUsedMask
 * parameter allows the function to only affect the GPIO pins of interest.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param gpioOutEn (valid range 0 - 0x07FFFF), bit per GPIO pin, the direction is
 *
 * gpioOutEn[bit]  |  GPIO direction
 * ----------------|-------------------
 *        0        |        input
 *        1        |        output
 *
 * \param gpioUsedMask Mask used to control which Oe bits are set/cleared.  If
 *                     mask bit =1, that bit will be modified by gpioOutEn bit
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_GpioOeSet(adi_adrv9025_Device_t* device,
                               uint32_t               gpioOutEn,
                               uint32_t               gpioUsedMask);

/**
 * \brief Gets the ADRV9025 low voltage GPIO pin directions
 *
 * This function will get the GPIO direction currently set in the device.
 * The direction can be either output or input per pin. The return gpioOutEn
 * function parameter returns a bit per GPIO pin.  1 = output from the ADRV9025
 * Device, 0 = input into the ADRV9025 device.
 *
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param gpioOutEn Pointer to a single uint32_t variable that returns the
 *                  output enable reading per GPIO pin
 *
 * gpioOutEn[bit]  |  GPIO direction
 * ----------------|-------------------
 *        0        |        input
 *        1        |        output
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_GpioOeGet(adi_adrv9025_Device_t* device,
                               uint32_t*              gpioOutEn);

/**
* \brief Sets the selected ADRV9025 low voltage GPIO pins I/O direction to Input
*
* This function will set the GPIO direction currently for the requested GPIOs to input.
* The direction can be either output or input per pin. The gpioInputMask
* determines the GPIO pins which can be set to input.
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param gpioInputMask ADRV9025 GPIO pins which are required to be set as input
*        in the range 0x00000 - 0x7FFFF
*
* GPIO_0 corresponds to LSB of gpioInputMask where as GPIO_18 corresponds
* to MSB of gpioInputMask. For example, to set GPIO_0, GPIO_1, GPIO_17,GPIO_18
* as inputs the gpioInputMask parameter would be equal to 0x60003.
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_GpioInputDirSet(adi_adrv9025_Device_t* device,
                                     uint32_t               gpioInputMask);

/**
* \brief Sets the selected ADRV9025 low voltage GPIO pins I/O direction to Output
*
* This function will set the GPIO direction currently for the requested GPIOs to output.
* The direction can be either output or input per pin. The gpioOutputMask
* determines the GPIO pins which can be set to output.
* \brief Not currently implemented
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param gpioOutputMask ADRV9025 GPIO pins which are required to be set as output
*        in the range 0x00000 - 0x7FFFF
*
* GPIO_0 corresponds to LSB of gpioOutputMask where as GPIO_18 corresponds
* to MSB of gpioOutputMask. For example, to set GPIO_0, GPIO_1, GPIO_17,GPIO_18
* as outputs the gpioInputMask parameter would be equal to 0x60003.
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_GpioOutputDirSet(adi_adrv9025_Device_t* device,
                                      uint32_t               gpioOutputMask);

/**
* \brief Sets the selected ADRV9025 analog GPIO pins I/O direction to Input
*
* This function will set the analog GPIO direction for the requested analog GPIO pins to input.
* The direction can be either output or input per pin. The gpioAnalogInputMask
* determines the GPIO pins which can be set to input.
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param gpioAnalogInputMask ADRV9025 analog GPIO pins which are required to be set as input
*        in the range 0x00 - 0xFF
*
*  GPIO_ANA_0 corresponds to LSB of gpioAnalogInputMask where as GPIO_ANA_7 corresponds
*  to MSB of gpioAnalogInputMask. For example, to set GPIO_ANA_0, GPIO_ANA_1, GPIO_ANA_7,GPIO_ANA_6
*  as inputs the gpioAnalogInputMask parameter would be equal to 0xC3.
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_GpioAnalogInputDirSet(adi_adrv9025_Device_t* device,
                                           uint8_t                gpioAnalogInputMask);

/**
* \brief Sets the selected ADRV9025 analog GPIO pins I/O direction to Output
*
* This function will set the analog GPIO direction for the requested analog GPIO pins to output.
* The direction can be either output or input per pin. The gpioAnalogInputMask
* determines the GPIO pins which can be set to input.
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param gpioAnalogOutputMask ADRV9025 analog GPIO pins which are required to be set as output
*        in the range 0x00 - 0xFF
*
*  GPIO_ANA_0 corresponds to LSB of gpioAnalogOutputMask where as GPIO_ANA_7 corresponds
*  to MSB of gpioAnalogOutputMask. For example, to set GPIO_ANA_0, GPIO_ANA_1, GPIO_ANA_7,GPIO_ANA_6
*  as outputs the gpioAnalogOutputMask parameter would be equal to 0xC3.
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_GpioAnalogOutputDirSet(adi_adrv9025_Device_t* device,
                                            uint8_t                gpioAnalogOutputMask);

/**
 * \brief Sets the ADRV9025 GPIO output source for different GPIO functionality
 *
 * This function will only affect the GPIO pins that have their OE direction
 * set to output.  Each four GPIO pins can be assigned a GPIO source.  Each
 * GPIO nibble (4 pins) must share that same GPIO output source. ENUM
 * adi_adrv9025_GpioOutputModes_e can be bitshifted and bitwise OR-ed together to create the
 * value for the gpioSrcCtrl function parameter.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param gpioSrcCtrl Nibble based source control, this is a 32 bit value
 *                    containing 5 nibbles that will set the output source
 *                    control for each set of four GPIO pins.
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_GpioOutSourceCtrlSet(adi_adrv9025_Device_t* device,
                                          uint32_t               gpioSrcCtrl);

/**
 * \brief Reads the ADRV9025 GPIO output source for each set of four low voltage
 *        GPIO pins.
 *
 *  See ENUM adi_adrv9025_GpioOutputModes_e for possible GPIO output sources.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param gpioSrcCtrl Pointer to a single uint32_t variable where nibble based
 *          GPIO source is returned.  Each nibble of this return value describes
 *          the GPIO source for the GPIO output pins based on the table below
 *          gpioSrcCtrl[bits]  |  Description
 *          -------------------|---------------------
 *                      [3:0]  | GPIO output source for GPIO[3:0] pins
 *                      [7:4]  | GPIO output source for GPIO[7:4] pins
 *                      [11:8] | GPIO output source for GPIO[11:8] pins
 *                      [15:12]| GPIO output source for GPIO[15:12] pins
 *                      [19:16]| GPIO output source for GPIO[18:16] pins
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_GpioOutSourceCtrlGet(adi_adrv9025_Device_t* device,
                                          uint32_t*              gpioSrcCtrl);

/**
 * \brief Sets the ADRV9025 low voltage GPIO output pins level
 *
 * This function will only affect the GPIO pins that have their OE direction set to output and
 * that have the correct source control for the nibbles in GPIO_BITBANG_MODE
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param gpioOutPinLevel Bit per GPIO pin, level to output for each GPIO pin. 0 = low output, 1 = high output
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_GpioOutPinLevelSet(adi_adrv9025_Device_t* device,
                                        uint32_t               gpioOutPinLevel);

/**
 * \brief Reads the ADRV9025 low voltage GPIO pin levels and returns their
 *        contents in a single 32-bit word
 *
 *  The GPIO pins that are set to be inputs in BITBANG mode will read back and
 *  be returned in the gpioPinLevel parameter. The return value is a bit per
 *  pin.  GPIO 0 returns on bit 0 of the gpioPinLevel parameter.  A logic low
 *  level returns a 0, a logic high level returns a 1.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param gpioInPinLevel Pointer to a single uint32_t variable which returns the
 *                     GPIO pin levels read back on the pins assigned as
 *                     inputs (bit per pin)
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_GpioInputPinLevelGet(adi_adrv9025_Device_t* device,
                                          uint32_t*              gpioInPinLevel);

/**
 * \brief Reads the ADRV9025 GPIO pin output levels for BITBANG mode
 *
 *  This function allows reading the value that the GPIO output pins are
 *  set to drive out the pins.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param gpioOutPinLevel Pointer to a single uint32_t variable which returns
 *                        the level set to output of each output GPIO pin
 *                        (bit per pin)
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 *
 */
int32_t adi_adrv9025_GpioOutPinLevelGet(adi_adrv9025_Device_t* device,
                                        uint32_t*              gpioOutPinLevel);

/**
 * \brief This API function configures the monitor output function for the GPIOs
 *
 * The monitor outputs allow visibility to some internal ADRV9025 signals.  Each
 * monitor index outputs a set of eight signals.  To output these signals on
 * the low voltage GPIO[18:0] pins, first set the desired GPIO[18:0] pin
 * direction, then set the GPIO nibble source control to allow the monitor
 * signals to route to a set of 4 GPIO pins.  If GPIO[18:16] nibble source is
 * set to monitor outs, monitor out[6:4] signals are routed to GPIO[18:16].
 * When the nibble source is set to monitor out for GPIO[15:0], monitorOut[7:0]
 * is routed to GPIO[7:0] and monitorOut[7:0] is also routed to GPIO[15:8].
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param monitorIndex The index that outputs a set of 8 monitor outputs. See
 *          table in documentation for which signals output on each GPIO pins
 *
 * \param monitorMask which GPIO monitor outputs are going to active, bit 0
 *          will represent monitor out 0 and bit 7 will represent monitor out 7.
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 *
 */
int32_t adi_adrv9025_GpioMonitorOutSrcSet(adi_adrv9025_Device_t* device,
                                          uint8_t                monitorIndex,
                                          uint8_t                monitorMask);

/**
 * \brief This API function reads the GPIO monitor index and mask from ADRV9025
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param monitorIndex Pointer to single uint8_t variable which returns the
 *                     current monitor signal selection index
 *
 * \param monitorMask Pointer to single uint8_t variable which returns the
 *                    monitor out signal masking.  Bit per monitor out [7:0]
 *                    signal.
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_GpioMonitorOutSrcGet(adi_adrv9025_Device_t* device,
                                          uint8_t*               monitorIndex,
                                          uint8_t*               monitorMask);

/**
 * \brief Sets the General Purpose (GP) interrupt register bit mask for either GP_INT0 ro GP_INT1 or both.
 *
 * \pre This function can be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param maskSelect Enum indicating the which gp interrupt mask to write
 * \param maskArray Data structure holding the gp interrupt masks to write
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_GpIntMaskSet(adi_adrv9025_Device_t*       device,
                                  adi_adrv9025_gpMaskSelect_e  maskSelect,
                                  adi_adrv9025_gp_MaskArray_t* maskArray);

/**
* \brief Gets the General Purpose (GP) interrupt register bit mask for either GP_INT0 ro GP_INT1 or both.
*
* \pre This function can be called any time after device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param maskSelect Enum indicating the which gp interrupt mask to write
* \param maskArray Pointer to data structure holding the gp interrupt masks retrieved
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_GpIntMaskGet(adi_adrv9025_Device_t*       device,
                                  adi_adrv9025_gpMaskSelect_e  maskSelect,
                                  adi_adrv9025_gp_MaskArray_t* maskArray);

/**
 * \brief Reads the General Purpose (GP) interrupt status to determine what caused the GP Interrupt pin to assert.
 *        WARNING: Reading the GP Status will clear the status bits.  DO NOT call this API before calling the API's adi_adrv9025_GpInt0Handler
                   of adi_adrv9025_GpInt1Handler.
 *
 *  When the BBIC detects a rising edge on either General Purpose Interrupt pins GP_INT0 or GP_INT1, this function
 *  allows the BBIC to determine the source of the interrupt.  The value returned in the status parameter
 *  will show one or more sources for the interrupt based on the following table.
 *
 *  The status word read-back will show the current value for all interrupt sources, even if they are disabled by the masked. However,
 *  the GP Interrupt pin will only assert for the enabled sources.
 *
 * \pre This function can be called any time after device initialization and after the interrupt mask bits have been set
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param gpIntStatus Pointer to status read-back word
 *
 *     bit[n] | GP Interrupt Mask Bit
 *     -------|-----------------------
 *     bit47  | RF1PLL_UNLOCK
 *     bit46  | RF2PLL_UNLOCK
 *     bit45  | AUXPLL_UNLOCK
 *     bit44  | CLKPLL_UNLOCK
 *     bit43  | RF1PLL_CP_OVR_RANGE
 *     bit42  | RF2PLL_CP_OVR_RANGE
 *     bit41  | AUXPLL_OVR_RANGE
 *     bit40  | CLKPLL_OVR_RANGE
 *     bit39  | SERDES_PLL_UNLOCK
 *     bit38  | DEFRAMER9_IRQ
 *     bit37  | DEFRAMER8_IRQ
 *     bit36  | DEFRAMER7_IRQ
 *     bit35  | DEFRAMER6_IRQ
 *     bit34  | DEFRAMER5_IRQ
 *     bit33  | DEFRAMER4_IRQ
 *     bit32  | DEFRAMER3_IRQ
 *     bit31  | DEFRAMER2_IRQ
 *     bit30  | DEFRAMER1_IRQ
 *     bit29  | DEFRAMER0_IRQ
 *     bit28  | FRAMER8_IRQ
 *     bit27  | FRAMER7_IRQ
 *     bit26  | FRAMER6_IRQ
 *     bit25  | FRAMER5_IRQ
 *     bit24  | FRAMER4_IRQ
 *     bit23  | FRAMER3_IRQ
 *     bit22  | FRAMER2_IRQ
 *     bit21  | FRAMER1_IRQ
 *     bit20  | FRAMER0_IRQ
 *     bit19  | PA_PROTECTION_TX4_ERROR
 *     bit18  | PA_PROTECTION_TX3_ERROR
 *     bit17  | PA_PROTECTION_TX2_ERROR
 *     bit16  | PA_PROTECTION_TX1_ERROR
 *     bit15  | ARM_FORCE_INTERRUPT
 *     bit14  | WATCHDOG_TIMEOUT
 *     bit13  | ARM_CALIBRATION_ERROR
 *     bit12  | ARM_SYSTEM_ERROR
 *     bit11  | ORX2_STREAM_ERROR
 *     bit10  | ORX1_STREAM_ERROR
 *     bit9   | TX4_STREAM_ERROR
 *     bit8   | TX3_STREAM_ERROR
 *     bit7   | TX2_STREAM_ERROR
 *     bit6   | TX1_STREAM_ERROR
 *     bit5   | RX4_STREAM_ERROR
 *     bit4   | RX3_STREAM_ERROR
 *     bit3   | RX2_STREAM_ERROR
 *     bit2   | RX1_STREAM_ERROR
 *     bit1   | CORE_STREAM_ERROR
 *     bit0   | Reserved
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_GpIntStatusGet(adi_adrv9025_Device_t* device,
                                    uint64_t*              gpIntStatus);

/**
 * \brief Sets the GPInt status register sticky bit mask.  This mask is a B0 only feature and masks which events
 * show up in the status register.  This bit mapping is exactly the same as the status register.
 *
 * \pre This function can be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param eventMask 64-bit unsigned integer mask representing which events to mask from the status register (1 = masked, 0 = active).
 *
 * \retval ADI_COMMON_ACT_ERR_RESET_FULL Recovery action when API has critical failure.
 * \retval ADI_COMMON_ACT_WARN_CHECK_PARAM Recovery action for bad parameter check.
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required.
 */
int32_t adi_adrv9025_GpIntStickyBitMaskSet(adi_adrv9025_Device_t* device,
                                           uint64_t               eventMask);

/**
 * \brief Gets the GPInt status register sticky bit mask.  This mask is a B0 only feature and masks which events
 * show up in the status register.  This bit mapping is exactly the same as the status register.
 *
 * \pre This function can be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param eventMask Pointer 64-bit unsigned integer mask representing which events are masked from the status register (1 = masked, 0 = active).
 *
 * \retval ADI_COMMON_ACT_ERR_RESET_FULL Recovery action when API has critical failure.
 * \retval ADI_COMMON_ACT_WARN_CHECK_PARAM Recovery action for bad parameter check.
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required.
 */
int32_t adi_adrv9025_GpIntStickyBitMaskGet(adi_adrv9025_Device_t* device,
                                           uint64_t*              eventMask);

/**
 * \brief Reads back the device temperature sensor
 *
 * This function reads the device's temperature sensor by requesting the latest
 * temperature sensor value from the ARM processor.  The temperature sensor
 * value read back is scaled to return the temperature as degrees C.
 *
 * \pre This function may be called after the device has been fully initialized
 *      any time during run-time operation, but only after the ARM has been
 *      configured
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param temperatureDegC A pointer to a single uint16_t element that returns the
 *        current 12bit temperature sensor value in degrees C.
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_RESET_SPI recovery action for SPI reset required
 * \retval ADI_ADRV9025_NO_ACTION function completed successfully, no action required
 */
int32_t adi_adrv9025_TemperatureGet(adi_adrv9025_Device_t* device,
                                    int16_t*               temperatureDegC);

/**
 * \brief Configures the selected auxiliary ADC with the supplied configuration.
 *
 * Multiple ADCs can be configured in a single invocation
 * of this function by supplying an array of adi_adrv9025_AuxAdcCfg_t structures.
 * Note that each of the supplied structures must target a unique ADC. The user may
 * not configure multiple ADCs using a single configuration structure because the
 * ADC to target is specified as a field within the structure. The function will
 * flag an error if this occurs.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param auxAdcConfig A pointer to the supplied ADC configuration structure(s)
 * \param arraySize The number of supplied configuration structures
 *
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for ARM/WATCHDOG errors
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_AuxAdcCfgSet(adi_adrv9025_Device_t*    device,
                                  adi_adrv9025_AuxAdcCfg_t* auxAdcConfig,
                                  uint8_t                   arraySize);

/**
 * \brief Reads back the configuration from the selected auxiliary ADC
 *
 * Only a single ADC configuration can be read with a single invocation of this function.
 * The supplied auxAdcConfig structure is populated with the configuration which
 * is based on the ADC's configuration registers. Note that supplied auxAdcConfig
 * structure does not need to be initialized in any way. For example, the supplied
 * auxAdcSelect argument is copied into auxAdcConfig by this function because
 * it is a member of adi_adrv9025_AuxAdcCfg_t.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param auxAdcSelect Select which auxiliary ADC to read the configuration from
 * \param auxAdcConfig A pointer to the supplied auxiliary ADC configuration
 * structure to populate
 *
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for ARM/WATCHDOG errors
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_AuxAdcCfgGet(adi_adrv9025_Device_t*      device,
                                  adi_adrv9025_AuxAdcSelect_e auxAdcSelect,
                                  adi_adrv9025_AuxAdcCfg_t*   auxAdcConfig);

/**
 * \brief Reads the selected ADC's sample value.
 *
 * This function obtains the selected ADC's sample value and populates the
 * supplied auxAdcValue structure with it.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \pre Before using this function to read back the sensor value:
 * adi_Adrv9025_AuxAdcCfgSet() function is needed in order to set up the selected
 * auxiliary ADC channel. The selected ADC will be started if a suitable configuration
 * is written using the adi_Adrv9025_AuxAdcCfgSet() function and that the associated
 * auxAdcConfig structure has its auxAdcEnable field set to ADI_ADRV9025_AUXADC_ENABLE.
 * All ADC sample values need to be obtained using this function.
 *
 * \post The auxAdcvalue structure will be populated with the ADC sample data as well
 * as an indication of the sample's validity and finally the ADC that the sample is from.
 * The ADC's output is dependent on the device's power supplies.
 * See the definition of the adi_adrv9025_AuxAdcCfg_t struct for the formula.
 * The ADC's output is linear only to 10 bits and as such, the user should either
 * round or truncate the output.
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param auxAdcSelect Select which auxiliary ADC to read a sample from
 * \param auxAdcValue A pointer to the supplied auxiliary ADC value structure to
 * populate
 *
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for ARM/WATCHDOG errors
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_AuxAdcValueGet(adi_adrv9025_Device_t*      device,
                                    adi_adrv9025_AuxAdcSelect_e auxAdcSelect,
                                    adi_adrv9025_AuxAdcValue_t* auxAdcValue);

/**
* \brief Starts AuxADC measurement.
*
* This function will start AuxADC measurement by toggling reset bit for selected AuxADC.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param auxAdcSelect Select which auxiliary ADC to start measurement
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for ARM/WATCHDOG errors
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AuxAdcMeasurementStart(adi_adrv9025_Device_t*      device,
                                            adi_adrv9025_AuxAdcSelect_e auxAdcSelect);

/**
 * \brief Enables/Disables SPI2 on ADRV9025
 * \brief Not currently implemented
 *
 * The ADRV9025 device can enable a second SPI port on the low voltage GPIO[3:0]
 * pins.  This SPI port allows read/write access to a limited set of
 * TxAttenuation and Rx gain index registers.
 *
 * The SPI2 uses the same configuration that is programmed for SPI. This
 * includes LSB/MSB first, 4 wire mode, streaming, and address increment.
 *
 * One unique feature about the TxAtten control is that the SPI register value
 * can be set and does not update to the transmitter until a GPIO pin is
 * toggled (See spi2TxAttenGpioSel function parameter). The GPIO pin is level
 * sensitive, selecting the TxAtten programmed in either the
 * tx(1/2)_attenuation_s1 or tx(1/2)_attenuation_s2 bitfields of the second SPI
 * registers.  The GPIO pin used to switch between the two TxAtten settings is
 * user selectable using the ENUM in the function parameter.
 *
 * For readback of TxAttenuation SPI registers, first write the desired register
 * to force the value to be updated, before reading it back across SPI.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param spi2Enable Enable(=1)/Disable(=0) SPI2 protocol on ADRV9025
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_Spi2CfgSet(adi_adrv9025_Device_t* device,
                                uint8_t                spi2Enable);

/**
 * \brief Get current status of SPI2 configuration on ADRV9025
 * \brief Not currently implemented *\
 *
 * This function reads back whether the second SPI port on the low voltage
 * GPIO[3:0] is enabled.  It also reads back the GPIO pin selected for the
 * TxAttenuation switch used when SPI2 is enabled.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param spi2Enable Indicates whether SPI2 protocol is Enabled(=1)/Disabled(=0) on ADRV9025
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_Spi2CfgGet(adi_adrv9025_Device_t* device,
                                uint8_t*               spi2Enable);

/**
 * \brief Clears GP Int status register
 *
 * \pre This function can be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 *
 * \retval ADI_COMMON_ACT_ERR_RESET_FULL Recovery action when API has critical failure.
 * \retval ADI_COMMON_ACT_WARN_CHECK_PARAM Recovery action for bad parameter check.
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required.
 */
int32_t adi_adrv9025_GPIntClearStatusRegister(adi_adrv9025_Device_t* device);

/**
* \brief Called whenever the BBIC detects a GP_INT1 assertion to find the source and clear it if possible.  GP_INT1 is
*  considered to be the HIGH priority interrupt service while GP_INT0 is considered the LOWER priority interrupt service.
*  A mutex around the GP_INT1 service routine insures that a GP_INT0 service request will be ignored while a GP_INT1 service
*  request is in process.  This is accomplished by disabling all GP_INT0 interrupts during the GP_INT1 service request.
*
*  When the BBIC detects a rising edge on the General Purpose Interrupt pin1, this function
*  allows the BBIC an easy way to determine the GP_INT1 source, clear it if possible, and receive a recovery action.
*
*  The GP Interrupt pin1 is the logical OR of all the sources and the gp_interrupts_mask_pin1. The gp_interrupts_mask_pin1
*  bit-field is used to control which of the 48 available interrupt sources can assert the GP_INT1 pin.  To enable an
*  available interrupt source for GP_INT1, write the corresponding bit in the gp_interrupts_mask0 bit-field to low.
*  Writing an interrupt source bit in the gp_interrupts_mask0 bit-field to high will disable that interrupt source from
*  asserting the GP_INT1 pin. The gp_interrupts_status_word read-back will show the current value for all interrupt sources,
*  even if they are disabled by the masked. However, the GP Interrupt pin will only assert for the enabled sources.
*
* \pre This function can be called any time after device initialization and after the interrupt mask bits have been set
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param gpInt1Status Pointer to status read-back word containing the GP_INT1 source registers. Bit mapping is:
*
*     bit[n] | GP Interrupt Mask
*     -------|-----------------------
*     bit47  | ADI_ADRV9025_GP_MASK_RF1PLL_UNLOCK
*     bit46  | ADI_ADRV9025_GP_MASK_RF2PLL_UNLOCK
*     bit45  | ADI_ADRV9025_GP_MASK_AUXPLL_UNLOCK
*     bit44  | ADI_ADRV9025_GP_MASK_CLKPLL_UNLOCK
*     bit43  | ADI_ADRV9025_GP_MASK_RF1PLL_CP_OVR_RANGE
*     bit42  | ADI_ADRV9025_GP_MASK_RF2PLL_CP_OVR_RANGE
*     bit41  | ADI_ADRV9025_GP_MASK_AUXPLL_OVR_RANGE
*     bit40  | ADI_ADRV9025_GP_MASK_CLKPLL_OVR_RANGE
*     bit39  | ADI_ADRV9025_GP_MASK_SERDES_PLL_UNLOCK
*     bit38  | ADI_ADRV9025_GP_MASK_DEFRAMER9_IRQ    204C Link1 Loss of Sync
*     bit37  | ADI_ADRV9025_GP_MASK_DEFRAMER8_IRQ    204B Link1 QBD IRQ
*     bit36  | ADI_ADRV9025_GP_MASK_DEFRAMER7_IRQ    Link1 Sysref Out of Phase
*     bit35  | ADI_ADRV9025_GP_MASK_DEFRAMER6_IRQ    Link1 Elastic Buffer Error
*     bit34  | ADI_ADRV9025_GP_MASK_DEFRAMER5_IRQ    Link1 Lane FIFO pointer error
*     bit33  | ADI_ADRV9025_GP_MASK_DEFRAMER4_IRQ    204C Link0 Loss of Sync
*     bit32  | ADI_ADRV9025_GP_MASK_DEFRAMER3_IRQ    204B Link0 QBD IRQ
*     bit31  | ADI_ADRV9025_GP_MASK_DEFRAMER2_IRQ    Link0 Sysref Out of Phase
*     bit30  | ADI_ADRV9025_GP_MASK_DEFRAMER1_IRQ    Link0 Elastic Buffer Error
*     bit29  | ADI_ADRV9025_GP_MASK_DEFRAMER0_IRQ    Link0 Lane FIFO pointer error
*     bit28  | ADI_ADRV9025_GP_MASK_FRAMER8_IRQ     Link2 Transport Not sending data
*     bit27  | ADI_ADRV9025_GP_MASK_FRAMER7_IRQ     Link2 Sysref out of Phase
*     bit26  | ADI_ADRV9025_GP_MASK_FRAMER6_IRQ     Link2 Lane FIFO pointer error
*     bit25  | ADI_ADRV9025_GP_MASK_FRAMER5_IRQ     Link1 Transport Not sending data
*     bit24  | ADI_ADRV9025_GP_MASK_FRAMER4_IRQ     Link1 Sysref out of Phase
*     bit23  | ADI_ADRV9025_GP_MASK_FRAMER3_IRQ     Link1 Lane FIFO pointer error
*     bit22  | ADI_ADRV9025_GP_MASK_FRAMER2_IRQ     Link0 Transport Not sending data
*     bit21  | ADI_ADRV9025_GP_MASK_FRAMER1_IRQ     Link0 Sysref out of Phase
*     bit20  | ADI_ADRV9025_GP_MASK_FRAMER0_IRQ     Link0 Lane FIFO pointer error
*     bit19  | ADI_ADRV9025_GP_MASK_PA_PROTECTION_TX4_ERROR
*     bit18  | ADI_ADRV9025_GP_MASK_PA_PROTECTION_TX3_ERROR
*     bit17  | ADI_ADRV9025_GP_MASK_PA_PROTECTION_TX2_ERROR
*     bit16  | ADI_ADRV9025_GP_MASK_PA_PROTECTION_TX1_ERROR
*     bit15  | ADI_ADRV9025_GP_MASK_ARM_FORCE_INTERRUPT
*     bit14  | ADI_ADRV9025_GP_MASK_WATCHDOG_TIMEOUT
*     bit13  | ADI_ADRV9025_GP_MASK_ARM_CALIBRATION_ERROR
*     bit12  | ADI_ADRV9025_GP_MASK_ARM_SYSTEM_ERROR
*     bit11  | ADI_ADRV9025_GP_MASK_ORX2_STREAM_ERROR
*     bit10  | ADI_ADRV9025_GP_MASK_ORX1_STREAM_ERROR
*     bit9   | ADI_ADRV9025_GP_MASK_TX4_STREAM_ERROR
*     bit8   | ADI_ADRV9025_GP_MASK_TX3_STREAM_ERROR
*     bit7   | ADI_ADRV9025_GP_MASK_TX2_STREAM_ERROR
*     bit6   | ADI_ADRV9025_GP_MASK_TX1_STREAM_ERROR
*     bit5   | ADI_ADRV9025_GP_MASK_RX4_STREAM_ERROR
*     bit4   | ADI_ADRV9025_GP_MASK_RX3_STREAM_ERROR
*     bit3   | ADI_ADRV9025_GP_MASK_RX2_STREAM_ERROR
*     bit2   | ADI_ADRV9025_GP_MASK_RX1_STREAM_ERROR
*     bit1   | ADI_ADRV9025_GP_MASK_CORE_STREAM_ERROR
*     bit0   | Reserved
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM/WATCHDOG errors
* \retval ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR Recovery action for errors the BBIC must track and decide
*                                   if the interrupt is an error or not.
* \retval ADI_ADRV9025_ACT_WARN_RERUN_TRCK_CAL ARM signals tracking cals need to be restarted
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_GpInt1Handler(adi_adrv9025_Device_t*      device,
                                   adi_adrv9025_gpIntStatus_t* gpInt1Status);

/**
* \brief Called whenever the BBIC detects a GP_INT0 assertion to find the source and clear it if possible.  GP_INT1 is
*  considered to be the HIGH priority interrupt service while GP_INT0 is considered the LOWER priority interrupt service.
*  GP_INT1 are not disabled during a GP_INT0 interrupt service request.  Thus, a GP_INT0 service request can be interrupted
*  by a GP_INT1 interrupt service request. When the BBIC detects a rising edge on the General Purpose Interrupt pin0, this function
*  allows the BBIC an easy way to determine the GP_INT0 source, clear it if possible, and receive a recovery action.
*
*  The GP Interrupt pin0 is the logical OR of all the sources and the gp_interrupts_mask_pin0. The gp_interrupts_mask_pin0
*  bit-field is used to control which of the 48 available interrupt sources can assert the GP_INT0 pin.  To enable an
*  available interrupt source for GP_INT0, write the corresponding bit in the gp_interrupts_mask0 bit-field to low.
*  Writing an interrupt source bit in the gp_interrupts_mask0 bit-field to high will disable that interrupt source from
*  asserting the GP_INT0 pin. The gp_interrupts_status_word read-back will show the current value for all interrupt sources,
*  even if they are disabled by the masked. However, the GP Interrupt pin will only assert for the enabled sources.
*
* \pre This function can be called any time after device initialization and after the interrupt mask bits have been set
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param gpInt0Status Pointer to status read-back word containing the GP_INT0 source registers. Bit mapping is:
*
*     bit[n] | GP Interrupt Mask
*     -------|-----------------------
*     bit47  | GP_MASK_RF1PLL_UNLOCK
*     bit46  | GP_MASK_RF2PLL_UNLOCK
*     bit45  | GP_MASK_AUXPLL_UNLOCK
*     bit44  | GP_MASK_CLKPLL_UNLOCK
*     bit43  | GP_MASK_RF1PLL_CP_OVR_RANGE
*     bit42  | GP_MASK_RF2PLL_CP_OVR_RANGE
*     bit41  | GP_MASK_AUXPLL_OVR_RANGE
*     bit40  | GP_MASK_CLKPLL_OVR_RANGE
*     bit39  | GP_MASK_SERDES_PLL_UNLOCK
*     bit38  | GP_MASK_DEFRAMER9_IRQ    204C Link1 Loss of Sync
*     bit37  | GP_MASK_DEFRAMER8_IRQ    204B Link1 QBD IRQ
*     bit36  | GP_MASK_DEFRAMER7_IRQ    Link1 Sysref Out of Phase
*     bit35  | GP_MASK_DEFRAMER6_IRQ    Link1 Elastic Buffer Error
*     bit34  | GP_MASK_DEFRAMER5_IRQ    Link1 Lane FIFO pointer error
*     bit33  | GP_MASK_DEFRAMER4_IRQ    204C Link0 Loss of Sync
*     bit32  | GP_MASK_DEFRAMER3_IRQ    204B Link0 QBD IRQ
*     bit31  | GP_MASK_DEFRAMER2_IRQ    Link0 Sysref Out of Phase
*     bit30  | GP_MASK_DEFRAMER1_IRQ    Link0 Elastic Buffer Error
*     bit29  | GP_MASK_DEFRAMER0_IRQ    Link0 Lane FIFO pointer error
*     bit28  | GP_MASK_FRAMER8_IRQ     Link2 Transport Not sending data
*     bit27  | GP_MASK_FRAMER7_IRQ     Link2 Sysref out of Phase
*     bit26  | GP_MASK_FRAMER6_IRQ     Link2 Lane FIFO pointer error
*     bit25  | GP_MASK_FRAMER5_IRQ     Link1 Transport Not sending data
*     bit24  | GP_MASK_FRAMER4_IRQ     Link1 Sysref out of Phase
*     bit23  | GP_MASK_FRAMER3_IRQ     Link1 Lane FIFO pointer error
*     bit22  | GP_MASK_FRAMER2_IRQ     Link0 Transport Not sending data
*     bit21  | GP_MASK_FRAMER1_IRQ     Link0 Sysref out of Phase
*     bit20  | GP_MASK_FRAMER0_IRQ     Link0 Lane FIFO pointer error
*     bit19  | GP_MASK_PA_PROTECTION_TX4_ERROR
*     bit18  | GP_MASK_PA_PROTECTION_TX3_ERROR
*     bit17  | GP_MASK_PA_PROTECTION_TX2_ERROR
*     bit16  | GP_MASK_PA_PROTECTION_TX1_ERROR
*     bit15  | GP_MASK_ARM_FORCE_INTERRUPT
*     bit14  | GP_MASK_WATCHDOG_TIMEOUT
*     bit13  | GP_MASK_ARM_CALIBRATION_ERROR
*     bit12  | GP_MASK_ARM_SYSTEM_ERROR
*     bit11  | GP_MASK_ORX2_STREAM_ERROR
*     bit10  | GP_MASK_ORX1_STREAM_ERROR
*     bit9   | GP_MASK_TX4_STREAM_ERROR
*     bit8   | GP_MASK_TX3_STREAM_ERROR
*     bit7   | GP_MASK_TX2_STREAM_ERROR
*     bit6   | GP_MASK_TX1_STREAM_ERROR
*     bit5   | GP_MASK_RX4_STREAM_ERROR
*     bit4   | GP_MASK_RX3_STREAM_ERROR
*     bit3   | GP_MASK_RX2_STREAM_ERROR
*     bit2   | GP_MASK_RX1_STREAM_ERROR
*     bit1   | GP_MASK_CORE_STREAM_ERROR
*     bit0   | Reserved
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM/WATCHDOG errors
* \retval ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR Recovery action for errors the BBIC must track and decide
*                                   if the interrupt is an error or not.
* \retval ADI_ADRV9025_ACT_WARN_RERUN_TRCK_CAL ARM signals tracking cals need to be restarted
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_GpInt0Handler(adi_adrv9025_Device_t*      device,
                                   adi_adrv9025_gpIntStatus_t* gpInt0Status);

/**
* \brief This function sets the configuration for AuxDACs.
*
* There are 8 AuxDACs at Adrv9025. AuxDAC0-7 outputs to GPIO Analog pin0-7 respectively.
* This function supports setting multiple configurations at the same call.
*
*     AuxDAC[n] | GPIO_ANA_n
*     -------   |-----------------------
*     AuxDAC0   | GPIO_ANA_0
*     AuxDAC1   | GPIO_ANA_1
*     AuxDAC2   | GPIO_ANA_2
*     AuxDAC3   | GPIO_ANA_3
*     AuxDAC4   | GPIO_ANA_4
*     AuxDAC5   | GPIO_ANA_5
*     AuxDAC6   | GPIO_ANA_6
*     AuxDAC7   | GPIO_ANA_7
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param auxDacConfig Array of AuxDAC configuration structures to be set.
* \param numberOfCfg Number of configurations at auxDacConfig array
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AuxDacCfgSet(adi_adrv9025_Device_t*   device,
                                  adi_adrv9025_AuxDacCfg_t auxDacConfig[],
                                  uint8_t                  numberOfCfg);

/**
* \brief This function gets the configuration of selected AuxDAC
*
* There are 8 AuxDACs at Adrv9025. AuxDAC0-7 outputs to GPIO Analog pin0-7 respectively.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param auxDacMask Enum to select AuxDAC
* \param auxDacConfig Pointer to AuxDAC configuration struct of selected AuxDAC
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AuxDacCfgGet(adi_adrv9025_Device_t*    device,
                                  adi_adrv9025_AuxDacs_e    auxDacMask,
                                  adi_adrv9025_AuxDacCfg_t* auxDacConfig);

/**
* \brief This function sets 12 bit DAC word of selected AuxDAC.
*
* There are 8 AuxDACs at Adrv9025. AuxDAC0-7 outputs to GPIO Analog pin0-7 respectively.
* This function supports setting multiple AuxDAC values at the same call
*
*     AuxDAC[n] | GPIO_ANA_n
*     -------   |-----------------------
*     AuxDAC0   | GPIO_ANA_0
*     AuxDAC1   | GPIO_ANA_1
*     AuxDAC2   | GPIO_ANA_2
*     AuxDAC3   | GPIO_ANA_3
*     AuxDAC4   | GPIO_ANA_4
*     AuxDAC5   | GPIO_ANA_5
*     AuxDAC6   | GPIO_ANA_6
*     AuxDAC7   | GPIO_ANA_7
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param auxDacValues Array of DAC value structs to be set
* \param numberOfCfg Number of DAC words at auxDacValues array
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AuxDacValueSet(adi_adrv9025_Device_t*     device,
                                    adi_adrv9025_AuxDacValue_t auxDacValues[],
                                    uint8_t                    numberOfCfg);

/**
* \brief This function reads the DAC word of selected AuxDAC.
*
* There are 8 AuxDACs at Adrv9025. AuxDAC0-7 uses a GPIO Analog pin0-7 respectively.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param auxDacMask AuxDAC selection to read DAC word
* \param auxDacValue Pointer to DAC value struct read from selected AuxDAC
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AuxDacValueGet(adi_adrv9025_Device_t*      device,
                                    adi_adrv9025_AuxDacs_e      auxDacMask,
                                    adi_adrv9025_AuxDacValue_t* auxDacValue);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_GPIO_H_ */
