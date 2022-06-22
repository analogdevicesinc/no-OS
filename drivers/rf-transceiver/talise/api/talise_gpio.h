// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_gpio.h
 * \brief Talise GPIO header file
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_GPIO_H_
#define TALISE_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "talise_types.h"
#include "talise_gpio_types.h"

/**
 * \brief Sets the Talise low voltage GPIO pin directions
 *
 * This function will set the GPIO direction given by the passed parameter.
 * The direction can be either output or input per pin. The gpioUsedMask
 * parameter allows the function to only affect the GPIO pins of interest.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
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
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setGpioOe(taliseDevice_t *device, uint32_t gpioOutEn,
			  uint32_t gpioUsedMask);

/**
 * \brief Gets the Talise low voltage GPIO pin directions
 *
 * This function will get the GPIO direction currently set in the device.
 * The direction can be either output or input per pin. The return gpioOutEn
 * function parameter returns a bit per GPIO pin.  1 = output from the Talise
 * Device, 0 = input into the Talise device.
 *
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param gpioOutEn Pointer to a single uint32_t variable that returns the
 *                  output enable reading per GPIO pin
 *
 * gpioOutEn[bit]  |  GPIO direction
 * ----------------|-------------------
 *        0        |        input
 *        1        |        output
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getGpioOe(taliseDevice_t *device, uint32_t *gpioOutEn);

/**
 * \brief Sets the Talise GPIO output source for different GPIO functionality
 *
 * This function will only affect the GPIO pins that have their OE direction
 * set to output.  Each four GPIO pins can be assigned a GPIO source.  Each
 * GPIO nibble (4 pins) must share that same GPIO output source. ENUM
 * taliseGpioMode_t can be bitshifted and bitwise OR-ed together to create the
 * value for the gpioSrcCtrl function parameter.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param gpioSrcCtrl Nibble based source control, this is a 32 bit value
 *                    containing 5 nibbles that will set the output source
 *                    control for each set of four GPIO pins.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setGpioSourceCtrl(taliseDevice_t *device, uint32_t gpioSrcCtrl);

/**
 * \brief Reads the Talise GPIO output source for each set of four low voltage
 *        GPIO pins.
 *
 *  See ENUM taliseGpioMode_t for possible GPIO output sources.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
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
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getGpioSourceCtrl(taliseDevice_t *device,
				  uint32_t *gpioSrcCtrl);

/**
 * \brief Sets the Talise low voltage GPIO output pins level
 *
 * This function will only affect the GPIO pins that have their OE direction set to output and
 * that have the correct source control for the nibbles in GPIO_BITBANG_MODE
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param gpioPinLevel Bit per GPIO pin, level to output for each GPIO pin. 0 = low output, 1 = high output
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setGpioPinLevel(taliseDevice_t *device, uint32_t gpioPinLevel);

/**
 * \brief Reads the Talise low voltage GPIO pin levels and returns their
 *        contents in a single 32-bit word
 *
 *  The GPIO pins that are set to be inputs in BITBANG mode will read back and
 *  be returned in the gpioPinLevel parameter. The return value is a bit per
 *  pin.  GPIO 0 returns on bit 0 of the gpioPinLevel parameter.  A logic low
 *  level returns a 0, a logic high level returns a 1.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param gpioPinLevel Pointer to a single uint32_t variable which returns the
 *                     GPIO pin levels read back on the pins assigned as
 *                     inputs (bit per pin)
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getGpioPinLevel(taliseDevice_t *device, uint32_t *gpioPinLevel);

/**
 * \brief Reads the Talise GPIO pin output levels for BITBANG mode
 *
 *  This function allows reading the value that the GPIO output pins are
 *  set to drive out the pins.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param gpioPinSetLevel Pointer to a single uint32_t variable which returns
 *                        the level set to output of each output GPIO pin
 *                        (bit per pin)
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 *
 */
uint32_t TALISE_getGpioSetLevel(taliseDevice_t *device,
				uint32_t *gpioPinSetLevel);

/**
 * \brief This API function configures the monitor output function for the GPIOs
 *
 * The monitor outputs allow visibility to some internal Talise signals.  Each
 * monitor index outputs a set of eight signals.  To output these signals on
 * the low voltage GPIO[18:0] pins, first set the desired GPIO[18:0] pin
 * direction, then set the GPIO nibble source control to allow the monitor
 * signals to route to a set of 4 GPIO pins.  If GPIO[18:16] nibble source is
 * set to monitor outs, monitor out[6:4] signals are routed to GPIO[18:16].
 * When the nibble source is set to monitor out for GPIO[15:0], monitorOut[7:0]
 * is routed to GPIO[7:0] and monitorOut[7:0] is also routed to GPIO[15:8].
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param monitorIndex The index that outputs a set of 8 monitor outputs. See
 *          table in documentation for which signals output on each GPIO pins
 *
 * \param monitorMask which GPIO monitor outputs are going to active, bit 0
 *          will represent monitor out 0 and bit 7 will represent monitor out 7.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 *
 */
uint32_t TALISE_setGpioMonitorOut(taliseDevice_t *device, uint8_t monitorIndex,
				  uint8_t monitorMask);

/**
 * \brief This API function reads the GPIO monitor index and mask from Talise
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param monitorIndex Pointer to single uint8_t variable which returns the
 *                     current monitor signal selection index
 *
 * \param monitorMask Pointer to single uint8_t variable which returns the
 *                    monitor out signal masking.  Bit per monitor out [7:0]
 *                    signal.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getGpioMonitorOut(taliseDevice_t *device, uint8_t *monitorIndex,
				  uint8_t *monitorMask);

/**
 * \brief Sets the General Purpose (GP) interrupt register bit mask (1 = disable IRQ from asserting GP interrupt pin)
 *
 * To enable just the JESD deframer IRQ signal:
 * TALISE_setGpIntMask(device, ~TAL_GP_MASK_JESD_DEFRMER_IRQ);
 *
 * \pre This function can be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param gpIntMask The bit-mask which masks (1=disable) the signals that may assert the GP interrupt pin.
 * taliseGpIntMask_t enumerated types are or'd together to form the GP interrupt mask word.
 * A bit value of 1 prevents the corresponding signal from asserting the GP interrupt
 *
 *       gpIntMask[bit] | Bit description
 *     -----------------|-----------------------
 *                 [15] | Reserved
 *                 [14] | Reserved
 *                 [13] | Reserved
 *                 [12] | TAL_GP_MASK_STREAM_ERROR
 *                 [11] | TAL_GP_MASK_ARM_CALIBRATION_ERROR
 *                 [10] | TAL_GP_MASK_ARM_SYSTEM_ERROR
 *                  [9] | TAL_GP_MASK_ARM_FORCE_INTERRUPT
 *                  [8] | TAL_GP_MASK_WATCHDOG_TIMEOUT
 *                  [7] | TAL_GP_MASK_PA_PROTECTION_TX2_ERROR
 *                  [6] | TAL_GP_MASK_PA_PROTECTION_TX1_ERROR
 *                  [5] | TAL_GP_MASK_JESD_DEFRMER_IRQ
 *                  [4] | TAL_GP_MASK_JESD_FRAMER_IRQ
 *                  [3] | TAL_GP_MASK_CLK_SYNTH_NONLOCK_ERROR
 *                  [2] | TAL_GP_MASK_AUX_SYNTH_NONLOCK_ERROR
 *                  [1] | TAL_GP_MASK_RF_SYNTH_NONLOCK_ERROR
 *                  [0] | Reserved
 *
 * \note The AUXPLL Interrupt is masked in TALISE_Initialize() since the ARM routinely
 * re-locks the AUXPLL.  If this interrupt is enabled, the user will observe spurious
 * GP_INT signal assertions approximately every 2 seconds.  It is recommended for the user
 * to NOT enable the AUXPLL interrupt.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setGpIntMask(taliseDevice_t *device, uint16_t gpIntMask);

/**
 * \brief Reads back which General Purpose (GP) interrupt signals are masked (disabled)
 *
 * \pre This function can be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param gpIntMask Bit-mask specifying which IRQ sources are masked from asserting the GP interrupt (1=disable IRQ).
 *
 *      enableMask[bit] | Bit description
 *     -----------------|-----------------------
 *                 [15] | Reserved
 *                 [14] | Reserved
 *                 [13] | Reserved
 *                 [12] | TAL_GP_MASK_STREAM_ERROR (1 = IRQ output masked (disabled); 0 = IRQ output unmasked (enabled))
 *                 [11] | TAL_GP_MASK_ARM_CALIBRATION_ERROR (1 = IRQ output masked (disabled); 0 = IRQ output unmasked (enabled))
 *                 [10] | TAL_GP_MASK_ARM_SYSTEM_ERROR (1 = IRQ output masked (disabled); 0 = IRQ output unmasked (enabled))
 *                  [9] | TAL_GP_MASK_ARM_FORCE_INTERRUPT (1 = IRQ output masked (disabled); 0 = IRQ output unmasked (enabled))
 *                  [8] | TAL_GP_MASK_WATCHDOG_TIMEOUT (1 = IRQ output masked (disabled); 0 = IRQ output unmasked (enabled))
 *                  [7] | TAL_GP_MASK_PA_PROTECTION_TX2_ERROR (1 = IRQ output masked (disabled); 0 = IRQ output unmasked (enabled))
 *                  [6] | TAL_GP_MASK_PA_PROTECTION_TX1_ERROR (1 = IRQ output masked (disabled); 0 = IRQ output unmasked (enabled))
 *                  [5] | TAL_GP_MASK_JESD_DEFRMER_IRQ (1 = IRQ output masked (disabled); 0 = IRQ output unmasked (enabled))
 *                  [4] | TAL_GP_MASK_JESD_FRAMER_IRQ (1 = IRQ output masked (disabled); 0 = IRQ output unmasked (enabled))
 *                  [3] | TAL_GP_MASK_CLK_SYNTH_NONLOCK_ERROR (1 = IRQ output masked (disabled); 0 = IRQ output unmasked (enabled))
 *                  [2] | TAL_GP_MASK_AUX_SYNTH_NONLOCK_ERROR (1 = IRQ output masked (disabled); 0 = IRQ output unmasked (enabled))
 *                  [1] | TAL_GP_MASK_RF_SYNTH_NONLOCK_ERROR (1 = IRQ output masked (disabled); 0 = IRQ output unmasked (enabled))
 *                  [0] | Reserved
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getGpIntMask(taliseDevice_t *device, uint16_t *gpIntMask);

/**
 * \brief Reads the General Purpose (GP) interrupt status to determine what caused the GP Interrupt pin to assert
 *
 *  When the BBIC detects a rising edge on the General Purpose Interrupt pin, this function
 *  allows the BBIC to determine the source of the interrupt.  The value returned in the status parameter
 *  will show one or more sources for the interrupt based on the following table.
 *
 *  The PLL unlock bits are not sticky.  They will follow the current status of the PLLs.  If the PLL relocks, the
 *  status bit will clear as well.  The GP Interrupt pin is the logical OR of all the sources.  When all the status
 *  bits are low, the GP Interrupt pin will be low.  The status word read-back will show the current value
 *  for all interrupt sources, even if they are disabled by the masked. However, the GP Interrupt pin will only assert
 *  for the enabled sources.
 *
 * \pre This function can be called any time after device initialization and after the interrupt mask bits have been set
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param gpIntStatus Pointer to status read-back word
 *
 *     gpIntStatus[bit] | Bit description
 *     -----------------|-----------------------
 *                 [15] | Reserved
 *                 [14] | Reserved
 *                 [13] | Reserved
 *                 [12] | TAL_GP_MASK_STREAM_ERROR
 *                 [11] | TAL_GP_MASK_ARM_CALIBRATION_ERROR
 *                 [10] | TAL_GP_MASK_ARM_SYSTEM_ERROR
 *                  [9] | TAL_GP_MASK_ARM_FORCE_INTERRUPT
 *                  [8] | TAL_GP_MASK_WATCHDOG_TIMEOUT
 *                  [7] | TAL_GP_MASK_PA_PROTECTION_TX2_ERROR
 *                  [6] | TAL_GP_MASK_PA_PROTECTION_TX1_ERROR
 *                  [5] | TAL_GP_MASK_JESD_DEFRMER_IRQ
 *                  [4] | TAL_GP_MASK_JESD_FRAMER_IRQ
 *                  [3] | TAL_GP_MASK_CLK_SYNTH_NONLOCK_ERROR (1 = unlocked)
 *                  [2] | TAL_GP_MASK_AUX_SYNTH_NONLOCK_ERROR (1 = unlocked)
 *                  [1] | TAL_GP_MASK_RF_SYNTH_NONLOCK_ERROR (1 = unlocked)
 *                  [0] | Reserved
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getGpIntStatus(taliseDevice_t *device, uint16_t *gpIntStatus);

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
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param temperatureDegC A pointer to a single uint16_t element that returns the
 *        current 12bit temperature sensor value in degrees C.
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_getTemperature(taliseDevice_t *device,
			       int16_t *temperatureDegC);


/**
 * \brief Sets up the 12 AuxDACs on the Talise device.
 *
 * This function uses the configuration in the Talise auxDac data structure
 * function parameter to setup each of the twelve DACs
 *
 * This function can be called any time after TALISE_initialize() to reconfigure, enable, disable the
 * different DAC outputs. The DACs are used in manual control mode.  After calling this setup
 * function, it is possible to change a specific DAC code by calling the TALISE_writeAuxDac() function.
 *
 * The AuxDAC outputs share the GPIO 3.3v pins.  When using an AuxDAC on a particular GPIO pin,
 * ensure that GPIO pin is set to be an input to tri-state the GPIO pad driver.
 *
 * AuxDac[n]   |  GPIO3.3v Pin[m]
 * ------------|-------------------
 *  AuxDac[0]  | GPIO3p3v[10]
 *  AuxDac[1]  | GPIO3p3v[8]
 *  AuxDac[2]  | GPIO3p3v[7]
 *  AuxDac[3]  | GPIO3p3v[11]
 *  AuxDac[4]  | GPIO3p3v[0]
 *  AuxDac[5]  | GPIO3p3v[1]
 *  AuxDac[6]  | GPIO3p3v[4]
 *  AuxDac[7]  | GPIO3p3v[5]
 *  AuxDac[8]  | GPIO3p3v[6]
 *  AuxDac[9]  | GPIO3p3v[9]
 *  AuxDac[10] | GPIO3p3v[2]
 *  AuxDac[11] | GPIO3p3v[3]
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param auxDac Pointer to the Talise auxiliary DAC data structure
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setupAuxDacs(taliseDevice_t *device, taliseAuxDac_t *auxDac);

/**
 * \brief Writes the current auxiliary DAC code for a specific auxiliary DAC
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param auxDacIndex Select the desired DAC to load the auxDacCode for DACs 0-11. Where, values 0-9 correspond to the
 * the ten 10-bit DACs, and 10-11 corresponds to the two 12-bit DACs
 * \param auxDacCode DAC code to write to the selected DAC. Sets the output voltage of the DAC (valid 0-1023 for auxDacIndex 0-9),
 * (valid 0-4095 for auxDacIndex 10-11)
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_writeAuxDac(taliseDevice_t *device, uint8_t auxDacIndex,
			    uint16_t auxDacCode);

/**
 * \brief Enables/Disables SPI2 on Talise
 *
 * The Talise device can enable a second SPI port on the low voltage GPIO[3:0]
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
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param spi2Enable Enable(=1)/Disable(=0) SPI2 protocol on Talise
 * \param spi2TxAttenGpioSel Sets up the GPIO that is used to select between
 *  two Tx attenuation values programmed through the SPI2 port. This GPIO is
 *  used only if SPI2 is enabled.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setSpi2Enable(taliseDevice_t *device, uint8_t spi2Enable,
			      taliseSpi2TxAttenGpioSel_t spi2TxAttenGpioSel);

/**
 * \brief Get current status of SPI2 configuration on Talise
 *
 * This function reads back whether the second SPI port on the low voltage
 * GPIO[3:0] is enabled.  It also reads back the GPIO pin selected for the
 * TxAttenuation switch used when SPI2 is enabled.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param spi2Enable Indicates whether SPI2 protocol is Enabled(=1)/Disabled(=0) on Talise
 * \param spi2TxAttenGpioSel Indicates the GPIO that is used to select between
 *  two Tx attenuation values programmed through the SPI2 port. This GPIO is
 *  used only if SPI2 is enabled.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getSpi2Enable(taliseDevice_t *device, uint8_t *spi2Enable,
			      taliseSpi2TxAttenGpioSel_t *spi2TxAttenGpioSel);

/**
 * \brief Sets the Talise 3.3v GPIO pin directions
 *
 * This function will set the 3.3V GPIO direction given by the passed parameter.
 * The direction can be either output or input per pin. The gpioUsedMask
 * parameter allows the function to only affect the GPIO pins of interest.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param gpio3v3OutEn (valid range 0 - 0x0FFF), bit per GPIO pin, the direction is
 *
 * gpio3v3OutEn[bit]  |  3.3V GPIO direction
 * -------------------|-------------------
 *        0           |        input
 *        1           |        output
 *
 * \param gpio3v3UsedMask Mask used to control which Oe bits are set/cleared.  If
 *                        mask bit =1, that bit will be modified by gpioOutEn bit
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setGpio3v3Oe(taliseDevice_t *device, uint16_t gpio3v3OutEn,
			     uint16_t gpio3v3UsedMask);

/**
 * \brief Gets the Talise low voltage GPIO pin directions
 *
 * This function will get the GPIO direction currently set in the device.
 * The direction can be either output or input per pin. The return gpioOutEn
 * function parameter returns a bit per GPIO pin.  1 = output from the Talise
 * Device, 0 = input into the Talise device.
 *
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param gpio3v3OutEn Pointer to a single uint16_t variable that returns the
 *                  output enable reading per GPIO pin
 *
 * gpio3v3OutEn[bit]  |  3.3V GPIO direction
 * -------------------|-------------------
 *        0           |        input
 *        1           |        output
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getGpio3v3Oe(taliseDevice_t *device, uint16_t *gpio3v3OutEn);

/**
 * \brief Sets the Talise 3.3V GPIO output source for different GPIO functionality
 *
 * This function will only affect the GPIO pins that have their OE direction
 * set to output. Each group of four GPIO pins can be assigned a GPIO source.  Each
 * GPIO nibble (4 pins) must share that same GPIO output source. ENUM
 * taliseGpio3v3Mode_t can be bitshifted and bitwise OR-ed together to create the
 * value for the gpioSrcCtrl function parameter.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param gpio3v3SrcCtrl Nibble based source control, this is a 12 bit value
 *                    containing 3 nibbles that will set the output source
 *                    control for each set of four GPIO pins.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setGpio3v3SourceCtrl(taliseDevice_t *device,
				     uint16_t gpio3v3SrcCtrl);

/**
 * \brief Reads the Talise 3.3V GPIO output source for different GPIO functionality
 *
 *  See ENUM taliseGpio3v3Mode_t for possible GPIO output sources.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param gpio3v3SrcCtrl Pointer to a single uint16_t variable where nibble based
 *          GPIO source is returned.  Each nibble of this return value describes
 *          the GPIO source for the GPIO output pins based on the table below
 *          gpio3v3SrcCtrl[bits]  |  Description
 *          ----------------------|---------------------
 *                         [3:0]  | GPIO output source for GPIO[3:0] pins
 *                         [7:4]  | GPIO output source for GPIO[7:4] pins
 *                        [11:8]  | GPIO output source for GPIO[11:8] pins
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getGpio3v3SourceCtrl(taliseDevice_t *device,
				     uint16_t *gpio3v3SrcCtrl);

/**
 * \brief Sets the Talise 3.3V GPIO output pins level
 *
 * This function will only affect the GPIO pins that have their OE direction set to output and
 * that have the correct source control for the nibbles in TAL_GPIO3V3_BITBANG_MODE
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param gpio3v3PinLevel Bit per GPIO pin, level to output for each GPIO pin. 0 = low output, 1 = high output
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setGpio3v3PinLevel(taliseDevice_t *device,
				   uint16_t gpio3v3PinLevel);

/**
 * \brief Reads the Talise 3.3V GPIO pin levels
 *
 *  The GPIO pins that are set to be inputs will read back and
 *  be returned in the gpioPinLevel parameter. The return value is a bit per
 *  pin.  GPIO 0 returns on bit 0 of the gpioPinLevel parameter.  A logic low
 *  level returns a 0, a logic high level returns a 1.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param gpio3v3PinLevel Pointer to a single uint16_t variable which returns the
 *                     GPIO pin levels read back on the pins assigned as
 *                     inputs (bit per pin)
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getGpio3v3PinLevel(taliseDevice_t *device,
				   uint16_t *gpio3v3PinLevel);

/**
 * \brief Reads the Talise GPIO pin output levels for BITBANG mode
 *
 *  This function allows reading the value that the GPIO output pins are
 *  set to drive out the pins.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param gpio3v3PinSetLevel Pointer to a single uint16_t variable which returns
 *                        the level set to output of each output GPIO pin
 *                        (bit per pin)
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 *
 */
uint32_t TALISE_getGpio3v3SetLevel(taliseDevice_t *device,
				   uint16_t *gpio3v3PinSetLevel);

/**
 * \brief Called whenever the BBIC detects a GP_INT assertion to find the source and clear it.
 *
 *  When the BBIC detects a rising edge on the General Purpose Interrupt pin, this function
 *  allows the BBIC an easy way to determine the GP_INT source, clear it if possible, and receive a recovery action.
 *
 *  The PLL unlock bits are not sticky.  They will follow the current status of the PLLs.  If the PLL relocks, the
 *  status bit will clear as well.  The GP Interrupt pin is the logical OR of all the sources.  When all the status
 *  bits are low, the GP Interrupt pin will be low.  The status word read-back will show the current value
 *  for all interrupt sources, even if they are disabled by the masked. However, the GP Interrupt pin will only assert
 *  for the enabled sources.
 *
 * \pre This function can be called any time after device initialization and after the interrupt mask bits have been set
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param gpIntStatus Pointer to status read-back word containing the GP_INT source registers. Bit mapping is:
 *
 *     bit[n] | GP Interrupt Mask Bit
 *     -------|-----------------------
 *     bit13  | 15 Reserved
 *     bit12  | TAL_GP_MASK_STREAM_ERROR
 *     bit11  | TAL_GP_MASK_ARM_CALIBRATION_ERROR
 *     bit10  | TAL_GP_MASK_ARM_SYSTEM_ERROR
 *     bit9   | TAL_GP_MASK_ARM_FORCE_INTERRUPT
 *     bit8   | TAL_GP_MASK_WATCHDOG_TIMEOUT
 *     bit7   | TAL_GP_MASK_PA_PROTECTION_TX2_ERROR
 *     bit6   | TAL_GP_MASK_PA_PROTECTION_TX1_ERROR
 *     bit5   | TAL_GP_MASK_JESD_DEFRMER_IRQ
 *     bit4   | TAL_GP_MASK_JESD_FRAMER_IRQ
 *     bit3   | TAL_GP_MASK_CLK_SYNTH_LOCK
 *     bit2   | TAL_GP_MASK_AUX_SYNTH_LOCK
 *     bit1   | TAL_GP_MASK_RF_SYNTH_LOCK
 *     bit0   | Reserved
 *
 * \param gpIntDiag Pointer to diagnostic structure that returns more specific error information from the
 *        GP_INT source.  If pointer is NULL, no diagnostic information is returned.
 *
 *     data[0] bit  | Bit Description
 *     -------------|-------------------
 *             bit0 | stream rx1_enable fall edge error
 *             bit1 | stream tx1_enable fall edge error
 *             bit2 | stream orx2_enable rise edge error
 *             bit3 | stream orx1_enable rise edge error
 *             bit4 | stream rx2_enable rise edge error
 *             bit5 | stream tx2_enable rise edge error
 *             bit6 | stream rx1_enable rise edge error
 *             bit7 | stream tx1_enable rise edge error
 *
 *     data[1] bit  | Bit Description
 *     -------------|-------------------
 *             bit0 | stream lpbk2_enable fall edge error
 *             bit1 | stream lpbk2_enable rise edge error
 *             bit2 | stream lpbk2_enable rise edge error
 *             bit3 | stream lpbk1_enable rise edge error
 *             bit4 | stream orx2_enable fall edge error
 *             bit5 | stream orx1_enable fall edge error
 *             bit6 | stream rx2_enable fall edge error
 *             bit7 | stream tx2_enable fall edge error
 *
 *     data[2] bit  | Bit Description
 *     -------------|-------------------
 *             bit0 | stream gpio3 fall edge error
 *             bit1 | stream gpio2 fall edge error
 *             bit2 | stream gpio1 fall edge error
 *             bit3 | stream gpio0 fall edge error
 *             bit4 | stream gpio3 rise edge error
 *             bit5 | stream gpio2 rise edge error
 *             bit6 | stream gpio1 rise edge error
 *             bit7 | stream gpio0 rise edge error
 *
 *     data[3] bit  | Bit Description
 *     -------------|-------------------
 *             bit0 | stream orx2low to rx2high stream error
 *             bit1 | stream orx1low to orx1high stream error
 *             bit2 | stream rx2low to orx2high stream error
 *             bit3 | stream rx1low to orx1high stream error
 *             bit4 | stream Erroneous completion of pin mode stream for gp_irq rise edge
 *             bit5 | deframerA BD - Bad Disparity Error
 *             bit6 | deframerA NIT - Not In Table Error
 *             bit7 | deframerA UEK - Unexpected K Error
 *
 *     data[4] bit  | Bit Description
 *     -------------|-------------------
 *             bit0 | deframerA ILD - Inter-Lane De-skew
 *             bit1 | deframerA ILS - InitialLane Sync
 *             bit2 | deframerA GCS - Good CheckSum
 *             bit3 | deframerA FS - Frame Sync
 *             bit4 | deframerA CSG - Code Group Sync
 *             bit5 | deframerA CMM - RESERVED
 *             bit6 | deframerA Pointers out of alignment
 *             bit7 | deframerA Sysref misalignment to global LMFC
 *
 *     data[5] bit  | Bit Description
 *     -------------|-------------------
 *             bit0 | deframerB BD - Bad Disparity Error
 *             bit1 | deframerB NIT - Not In Table Error
 *             bit2 | deframerB UEK - Unexpected K Error
 *             bit3 | deframerB ILD - Inter-Lane De-skew
 *             bit4 | deframerB ILS - InitialLane Sync
 *             bit5 | deframerB GCS - Good CheckSum
 *             bit6 | deframerB FS - Frame Sync
 *             bit7 | deframerB CSG - Code Group Sync
 *
 *     data[6] bit  | Bit Description
 *     -------------|-------------------
 *             bit0 | deframerB CMM - RESERVED
 *             bit1 | deframerB Pointers out of alignment
 *             bit2 | deframerB Sysref misalignment to global LMFC
 *             bit3 | framerA Asynchronous FIFO pointer offset error
 *             bit4 | framerA Misalignment to current LMFC error
 *             bit5 | framerB Asynchronous FIFO pointer offset error
 *             bit6 | framerB Misalignment to current LMFC error
 *             bit7 | PA protection error for chan 1
 *
 *     data[7] bit  | Bit Description
 *     -------------|-------------------
 *             bit0 | PA protection error for chan 2
 *             bit1 | ARM Calibration error
 *             bit2 | CLK PLL lock detect reset
 *             bit3 | AUX PLL lock detect reset
 *             bit4 | RF PLL lock detect reset
 *             bit5 | ARM WatchDog Timeout
 *             bit6 | ARM Force GP_INT
 *             bit7 | ARM System Error
 *
 *     data[8] bit  | Bit Description
 *     -------------|-------------------
 *             bit0 | ARM Data Memory Parity Error
 *             bit1 | ARM Program Memory Parity Error
 *             bit2 | ARM Command Wait TimeOut
 *             bit3 |
 *             bit4 |
 *             bit5 |
 *             bit6 |
 *             bit7 |
 *
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_ARM Recovery action for ARM/WATCHDOG errors
 * \retval TALACT_ERR_RERUN_INIT_CALS Recovery actions for INIT CALIBRATION errors
 * \retval TALACT_ERR_RESET_JESD204FRAMERA Recovery action for FramerA errors
 * \retval TALACT_ERR_RESET_JESD204FRAMERB Recovery action for FramerB errors
 * \retval TALACT_ERR_RESET_FULL Recovery action for catastrophic errors such as CLK errors
 * \retval TALACT_ERR_RESET_JESD204DEFRAMERA Recovery action for DeFramerA errors
 * \retval TALACT_ERR_RESET_JESD204DEFRAMERB Recovery action for DeFramerB errors
 * \retval TALACT_ERR_REDUCE_TXSAMPLE_PWR Recovery action for PA Protection errors
 * \retval TALACT_ERR_BBIC_LOG_ERROR Recovery action for errors the BBIC must track and decide
 *                                   if the interrupt is an error or not.
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_gpIntHandler(taliseDevice_t *device, uint32_t *gpIntStatus,
			     taliseGpIntInformation_t *gpIntDiag);

/**
 * \brief Assigns the requested GPIO pin to Aux ADC start signal in ARM
 *
 *  GPIO pin is assigned as input to ARM. The ARM processor expects a low(0) to
 *  high(1) pulse on the assigned GPIO pin to start Aux ADC conversion.
 *
 *  This function just associates the GPIO pin with aux adc start signal. However,
 *  to start A/D conversion, the user has to call TALISE_startAuxAdc() API.
 *
 *  Passing a valid GPIO 1p8v(TAL_GPIO0-TAL_GPIO15) will associate the GPIO
 *  with Aux ADC Start signal if the GPIO pin requested is not already
 *  consumed by another feature.
 *
 *  Passing TAL_GPIO_INVALID option will unassign the GPIO pin associated
 *  with Aux ADC Start signal in ARM
 *
 *  To start A/D conversion in Pin Mode, TALISE_startAuxAdc() API needs to be called with
 *  auxAdcConfig->auxAdcMode set to TAL_AUXADC_PIN_MODE.
 *
 *  NOTE: This API is only supported when the ARM processor in TALISE is in IDLE state.
 *  Hence, it is required to call TALISE_RadioOff() before setting up the
 *  GPIO pin for Aux ADC start signal.
 *
 *  NOTE: Only GPIO[0] to GPIO[15] are supported. GPIO[16:18] are not supported
 *  to be used for Aux ADC start signal.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param pinModeGpio GPIO Pin to be assigned / unassigned from AUX ADC START signal in ARM
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setAuxAdcPinModeGpio(taliseDevice_t *device,
				     taliseGpioPinSel_t pinModeGpio);

/**
 * \brief Returns the gpio pin associated with aux ADC start signal in ARM
 * for Pin mode Aux ADC operations.
 *
 *  Only GPIO[0] to GPIO[15] are valid GPIO pin selects. TAL_GPIO_INVALID
 *  is returned in case no valid GPIO is assigned to aux ADC start signal.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param pinModeGpio pointer to taliseGpioPinSel_t enum type to which GPIO pin currently
 *        associated with Aux ADC start signal is written to
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getAuxAdcPinModeGpio(taliseDevice_t *device,
				     taliseGpioPinSel_t *pinModeGpio);

/**
 * \brief Configures one of the four 12-bit AuxADC on the Talise device for sampling and A/D conversion.
 * AuxADC on Talise is also used by the temparature sensor. This API configures Talise to allow external
 * use of Aux ADC.
 *
 * This function uses the configuration in the taliseAuxAdcConfig_t data structure
 * function parameter to setup the AuxADC channel to be used for external A/D conversion use.
 *
 * This function can be called any time after TALISE_initialize() to schedule sampling and conversion
 * of AuxADC inputs. AuxADC A/D conversion results can be obtained through TALISE_readAuxAdc() API call.
 *
 * To setup the GPIO pin for Pin mode Aux ADC conversion, use TALISE_setAuxAdcPinModeGpio() API.
 * The ARM processor on TALISE needs to be in IDLE state to accept the GPIO ctrl command.
 * Hence, it is reqd to call the TALISE_RadioOff() API before calling TALISE_setAuxAdcPinModeGpio() API
 * following which TALISE_RadioOn() API can be called to move the ARM processor in Talise to RADIO_ON state.
 *
 * The Pin-mode AuxADC conversion expects a low(0) to high(1) pulse on the GPIO pin assigned to Aux ADC
 * start signal via TALISE_setAuxAdcPinModeGpio() API to schedule A/D Conversion.A rising edge on this pin is needed for
 * each AuxADC measurement up to the number of AuxADC samples requested. The rising edges will be ignored during
 * a temperature sensor measurement.
 *
 * When using a particular 1.8v GPIO pin for AuxADC Pin-mode, ensure that the GPIO pin selected is not in
 * use by another feature.
 *
 * NOTE: Temparature sensor measurements are prioritized over external AuxADC use.
 *
 * NOTE: The pulse on the GPIO pin is expected to be held high for at least 1 ARM clock cycle duration.
 *
 * NOTE: TALISE_startAuxAdc() will work in Radio ON or Radio OFF state
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param auxAdcConfig Pointer to taliseAuxAdcConfig_t Talise auxiliary ADC configuration data structure
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_startAuxAdc(taliseDevice_t *device,
			    taliseAuxAdcConfig_t *auxAdcConfig);

/**
 * \brief Reads the AuxADC conversion result that was scheduled prior to calling this API.
 *
 * This function reads the A/D conversion result from ARM mailbox and updates the auxAdcResult
 * data structure.
 *
 * This function can be called any time after TALISE_initialize() and following a TALISE_startAuxAdc() call.
 *
 * The auxAdcResult is valid depending on the complete indicator field.
 *
 *     auxAdcResult->     |
 *     completeIndicator  | Field Description
 *     -------------------|-------------------------------
 *                     0  | AuxADC Conversion not complete
 *                     1  | AuxADC Conversion complete
 *
 * NOTE: TALISE_readAuxAdc() will work in Radio ON or Radio OFF state
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param auxAdcResult Pointer to the taliseAuxAdcResult_t data structure where the result
 *        of aux ADC A/D conversion is written to.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_readAuxAdc(taliseDevice_t *device,
			   taliseAuxAdcResult_t *auxAdcResult);


/**
 * \brief Private Helper function to return GPIO and General Purpose Interrupt
 *  error strings to the TALISE_getErrorMessage function.
 *
 *  This is a private function and is automatically called by the API.  BBIC
 *  should use the TALISE_getErrorMessage function.
 *
 * \param errSrc A value that represents the error source from the Talise API
 * \param errCode Error code that along with the error source allows looking up
 *        a specific error message.
 *
 * \retval Returns a character array with the error message specified by
 *         errSrc and errCode.
 */
const char* talGetGpioErrorMessage(uint32_t errSrc, uint32_t errCode);

#ifdef __cplusplus
}
#endif

#endif /* TALISE_GPIO_H_ */
