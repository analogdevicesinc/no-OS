/**
 * \file
 * \brief ADRV9001 GPIO header file
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_GPIO_H_
#define _ADI_ADRV9001_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *********************************************************************************************************
 *                                             INCLUDE FILES
 *********************************************************************************************************
 */

#include "adi_common_error.h"
#include "adi_adrv9001_gpio_types.h"
#include "adi_adrv9001_types.h"

/*
 *********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 *********************************************************************************************************
 */

/**
 * \brief Called whenever the BBIC detects a GP_INT assertion to find the source and clear it if possible.
 *
 *  When the BBIC detects a rising edge on the General Purpose Interrupt pin GP_INT, this function
 *  allows the BBIC an easy way to determine the GP_INT source, clear it if possible, and receive a recovery action.
 *
 *  The GP Interrupt pin is the logical OR of all the sources and the GP_Interrupt_Mask. The GP_Interrupt_Mask
 *  bit-field is used to control which of the 25 available interrupt sources can assert the GP_INT pin.  To enable an
 *  available interrupt source for GP_INT, write the corresponding bit in the GP_Interrupt_Mask bit-field to low.
 *  Writing an interrupt source bit in the GP_Interrupt_Mask bit-field to high will disable that interrupt source from
 *  asserting the GP_INT pin. The GP_Interrupt_status read-back will show the current value for all interrupt sources,
 *  even if they are disabled. However, the GP Interrupt pin will only assert for the enabled sources.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function can be called any time after device initialization and after the interrupt mask bits have been set
 *
 * \param[in]  adrv9001	        Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[out] gpIntStatus      Pointer to status read-back word containing the GP_INT source registers.
 * \parblock
 *     bit[n] | GP Interrupt Mask
 *     -------|-----------------------
 *     bit27  | ADI_ADRV9001_GP_MASK_RX_DP_RECEIVE_ERROR
 *     bit26  | ADI_ADRV9001_GP_MASK_TX_DP_TRANSMIT_ERROR
 *     bit25  | ADI_ADRV9001_GP_MASK_RX_DP_READ_REQUEST_FROM_BBIC
 *     bit24  | ADI_ADRV9001_GP_MASK_TX_DP_WRITE_REQUEST_TO_BBIC
 *     bit23  | Not used
 *     bit22  | Not used
 *     bit21  | Not used
 *     bit20  | ADI_ADRV9001_GP_MASK_STREAM_PROCESSOR_3_ERROR
 *     bit19  | ADI_ADRV9001_GP_MASK_STREAM_PROCESSOR_2_ERROR
 *     bit18  | ADI_ADRV9001_GP_MASK_STREAM_PROCESSOR_1_ERROR
 *     bit17  | ADI_ADRV9001_GP_MASK_STREAM_PROCESSOR_0_ERROR
 *     bit16  | ADI_ADRV9001_GP_MASK_MAIN_STREAM_PROCESSOR_ERROR
 *     bit15  | ADI_ADRV9001_GP_MASK_LSSI_RX2_CLK_MCS
 *     bit14  | ADI_ADRV9001_GP_MASK_LSSI_RX1_CLK_MCS
 *     bit13  | ADI_ADRV9001_GP_MASK_CLK_1105_MCS_SECOND
 *     bit12  | ADI_ADRV9001_GP_MASK_CLK_1105_MCS
 *     bit11  | ADI_ADRV9001_GP_MASK_CLK_PLL_LOCK
 *     bit10  | ADI_ADRV9001_GP_MASK_AUX_PLL_LOCK
 *     bit9   | ADI_ADRV9001_GP_MASK_RF_PLL_LOCK2
 *     bit8   | ADI_ADRV9001_GP_MASK_RF_PLL_LOCK1
 *     bit7   | ADI_ADRV9001_GP_MASK_CLK_PLL_LOW_POWER_LOCK
 *     bit6   | ADI_ADRV9001_GP_MASK_TX2_PA_PROTECTION_ERROR
 *     bit5   | ADI_ADRV9001_GP_MASK_TX1_PA_PROTECTION_ERROR
 *     bit4   | ADI_ADRV9001_GP_MASK_CORE_ARM_MONITOR_ERROR
 *     bit3   | ADI_ADRV9001_GP_MASK_CORE_ARM_CALIBRATION_ERROR
 *     bit2   | ADI_ADRV9001_GP_MASK_CORE_ARM_SYSTEM_ERROR
 *     bit1   | ADI_ADRV9001_GP_MASK_CORE_FORCE_GP_INTERRUPT
 *     bit0   | ADI_ADRV9001_GP_MASK_CORE_ARM_ERROR
 * \endparblock
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_gpio_GpIntHandler(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_gpIntStatus_t *gpIntStatus);

/**
 * \brief Sets the General Purpose (GP) interrupt register bit mask for GP_INT.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function can be called any time after device initialization
 *
 * \param[in] adrv9001	        Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] maskSelect        Enum indicating the GP interrupt mask register to write
 * \param[in] maskArray         Data structure holding the GP interrupt masks to write
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_gpio_GpIntMask_Set(adi_adrv9001_Device_t *adrv9001, 
                                        adi_adrv9001_gpMaskSelect_e maskSelect, 
                                        adi_adrv9001_gpMaskArray_t *maskArray);

/**
 * \brief Gets the General Purpose (GP) interrupt register bit mask for GP_INT.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function can be called any time after device initialization
 *
 * \param[in]  adrv9001	        Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  maskSelect       Enum indicating the GP interrupt mask register to read
 * \param[out] maskArray        Pointer to data structure holding the GP interrupt masks retrieved
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_gpio_GpIntMask_Get(adi_adrv9001_Device_t *adrv9001, 
                                        adi_adrv9001_gpMaskSelect_e maskSelect, 
                                        adi_adrv9001_gpMaskArray_t *maskArray);

/**
 * \brief Reads the General Purpose (GP) interrupt status to determine what caused the GP Interrupt pin to assert.
 *        WARNING: Reading the GP Status will clear the status bits.
 *        DO NOT call this API before calling the API's adi_adrv9001_gpio_GpIntHandler
 *
 *  When the BBIC detects a rising edge on either General Purpose Interrupt pin GP_INT, this function
 *  allows the BBIC to determine the source of the interrupt.  The value returned in the status parameter
 *  will show one or more sources for the interrupt based table given below.
 *
 *  The status word read-back will show the current value for all interrupt sources, even if they are disabled by mask register.
 *  However, the GP Interrupt pin will only assert for the enabled sources.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function can be called any time after device initialization and after the interrupt mask bits have been set
 *
 * \param[in]  adrv9001	        Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[out] gpIntStatus      Pointer to status read-back word
 * \parblock
 *     bit[n] | GP Interrupt Mask Bit
 *     -------|-----------------------
 *     bit27  | RX_DP_RECEIVE_ERROR
 *     bit26  | TX_DP_TRANSMIT_ERROR
 *     bit25  | RX_DP_READ_REQUEST_FROM_BBIC
 *     bit24  | TX_DP_WRITE_REQUEST_TO_BBIC
 *     bit23  | Not used
 *     bit22  | Not used
 *     bit21  | Not used
 *     bit20  | STREAM_PROCESSOR_3_ERROR
 *     bit19  | STREAM_PROCESSOR_2_ERROR
 *     bit18  | STREAM_PROCESSOR_1_ERROR
 *     bit17  | STREAM_PROCESSOR_0_ERROR
 *     bit16  | MAIN_STREAM_PROCESSOR_ERROR
 *     bit15  | LSSI_RX2_CLK_MCS
 *     bit14  | LSSI_RX1_CLK_MCS
 *     bit13  | CLK_1105_MCS_SECOND
 *     bit12  | CLK_1105_MCS
 *     bit11  | CLK_PLL_LOCK
 *     bit10  | AUX_PLL_LOCK
 *     bit9   | RF_PLL_LOCK2
 *     bit8   | RF_PLL_LOCK1
 *     bit7   | CLK_PLL_LOW_POWER_LOCK
 *     bit6   | TX2_PA_PROTECTION_ERROR
 *     bit5   | TX1_PA_PROTECTION_ERROR
 *     bit4   | CORE_ARM_MONITOR_ERROR
 *     bit3   | CORE_ARM_CALIBRATION_ERROR
 *     bit2   | CORE_ARM_SYSTEM_ERROR
 *     bit1   | CORE_FORCE_GP_INTERRUPT
 *     bit0   | CORE_ARM_ERROR
 * \endparblock
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_gpio_GpIntStatus_Get(adi_adrv9001_Device_t *adrv9001, uint32_t *gpIntStatus);

/**
 * \brief Set the specified GPIO pin to the desired level
 *
 * This function will only affect the GPIO pins that have their OE direction set to output and
 * that have the correct source control for the nibbles in GPIO_BITBANG_MODE
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv9001	Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] pin        The GPIO pin to set to the desired level
 * \param[in] level      The logic level to set the pin to
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_gpio_OutputPinLevel_Set(adi_adrv9001_Device_t *adrv9001,
                                             adi_adrv9001_GpioPin_e pin, 
                                             adi_adrv9001_GpioPinLevel_e level);

/**
 * \brief Reads the ADRV9001 GPIO pin output levels for BITBANG mode
 *
 *  This function allows reading the value that the GPIO output pins are
 *  set to drive out the pins.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv9001	        Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  pin               The pin for which to get the level
 * \param[out] gpioOutPinLevels  Current level for each output
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_gpio_OutputPinLevel_Get(adi_adrv9001_Device_t *adrv9001,
                                             adi_adrv9001_GpioPin_e pin,
                                             adi_adrv9001_GpioPinLevel_e *gpioOutPinLevel);

/**
 * \brief Reads the ADRV9001 low voltage GPIO pin levels and returns their
 *        contents in a single 16-bit word
 *
 *  The GPIO pins that are set to be inputs in BITBANG mode will read back and
 *  be returned in the gpioPinLevel parameter. The return value is a bit per
 *  pin.  GPIO 0 returns on bit 0 of the gpioPinLevel parameter.  A logic low
 *  level returns a 0, a logic high level returns a 1.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv9001	        Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  pin               The pin for which to get the level
 * \param[out] gpioInPinLevels   Current level read for each input
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_gpio_InputPinLevel_Get(adi_adrv9001_Device_t *adrv9001, 
                                            adi_adrv9001_GpioPin_e pin,
                                            adi_adrv9001_GpioPinLevel_e *gpioInPinLevels);
    
/**
 * \brief Configure specified pin as manual input
 * 
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv9001	Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] pin       The GPIO pin to configure
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_gpio_ManualInput_Configure(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_GpioPin_e pin);
    
/**
 * \brief Configure specified pin crumb as manual output
 * 
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv9001	Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] crumb     The GPIO pin crumb to configure
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_gpio_ManualOutput_Configure(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_GpioPinCrumbSel_e crumb);

/**
 * \brief Configure specified analog GPIO pin as manual input
 * 
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv9001	Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] pin       The analog GPIO pin to configure
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_gpio_ManualAnalogInput_Configure(adi_adrv9001_Device_t *adrv9001, 
                                                      adi_adrv9001_GpioPin_e pin);
/**
 * \brief Configure specified analog GPIO pin nibble as manual output
 * 
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv9001	Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] nibble    The analog GPIO pin nibble to configure
 * \param[in] source    The source signal to be output on the pins
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_gpio_ManualAnalogOutput_Configure(adi_adrv9001_Device_t *adrv9001, 
                                                       adi_adrv9001_GpioAnalogPinNibbleSel_e nibble);

/**
 * \brief Sets the configuration for the Front End GPIO
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \param[in] adrv9001          Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] gpioCtrlInitCfg   GPIO signal configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_gpio_ControlInit_Configure(adi_adrv9001_Device_t *adrv9001,
                                                adi_adrv9001_GpioCtrlInitCfg_t *gpioCtrlInitCfg);
    
/**
 * \brief Configure the ADRV9001 GPIO for the specified signal
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in] adrv9001      Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] signal        The signal to configure
 * \param[in] gpioConfig    Desired GPIO configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_gpio_Configure(adi_adrv9001_Device_t *adrv9001,
                                    adi_adrv9001_GpioSignal_e signal,
                                    adi_adrv9001_GpioCfg_t *gpioConfig);
    
/**
 * \brief Retrieve the ADRV9001 GPIO configuration for the requested signal
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001         Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in]  signal           The signal for which to retrieve the GPIO configuration
 * \param[out] gpioConfig       The current GPIO configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_gpio_Inspect(adi_adrv9001_Device_t *adrv9001,
                                  adi_adrv9001_GpioSignal_e signal,
                                  adi_adrv9001_GpioCfg_t *gpioConfig);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_GPIO_H_ */
