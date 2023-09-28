/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adi_adrv9025_gpio_types.h
 * \brief Contains functions to allow control of the General Purpose IO functions on the ADRV9025 device
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_GPIO_TYPES_H_
#define _ADI_ADRV9025_GPIO_TYPES_H_

#include "adi_adrv9025_data_interface_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 *  \brief Enum to select desired GPIO pin used by the API
 */
typedef enum adi_adrv9025_GpioPinSel
{
    ADI_ADRV9025_GPIO_00 = 0,   /*!< Select GPIO_00*/
    ADI_ADRV9025_GPIO_01,       /*!< Select GPIO_01*/
    ADI_ADRV9025_GPIO_02,       /*!< Select GPIO_02*/
    ADI_ADRV9025_GPIO_03,       /*!< Select GPIO_03*/
    ADI_ADRV9025_GPIO_04,       /*!< Select GPIO_04*/
    ADI_ADRV9025_GPIO_05,       /*!< Select GPIO_05*/
    ADI_ADRV9025_GPIO_06,       /*!< Select GPIO_06*/
    ADI_ADRV9025_GPIO_07,       /*!< Select GPIO_07*/
    ADI_ADRV9025_GPIO_08,       /*!< Select GPIO_08*/
    ADI_ADRV9025_GPIO_09,       /*!< Select GPIO_09*/
    ADI_ADRV9025_GPIO_10,       /*!< Select GPIO_11*/
    ADI_ADRV9025_GPIO_11,       /*!< Select GPIO_12*/
    ADI_ADRV9025_GPIO_12,       /*!< Select GPIO_13*/
    ADI_ADRV9025_GPIO_13,       /*!< Select GPIO_14*/
    ADI_ADRV9025_GPIO_14,       /*!< Select GPIO_15*/
    ADI_ADRV9025_GPIO_15,       /*!< Select GPIO_16*/
    ADI_ADRV9025_GPIO_16,       /*!< Select GPIO_17*/
    ADI_ADRV9025_GPIO_17,       /*!< Select GPIO_18*/
    ADI_ADRV9025_GPIO_18,       /*!< Select GPIO_19*/
    ADI_ADRV9025_GPIO_INVALID   /*!< Invalid GPIO*/
} adi_adrv9025_GpioPinSel_e;

/**
*  \brief Enum to select desired Analog GPIO pins used by the API
*/
typedef enum adi_adrv9025_Gpio3v3PinSel
{
    ADI_ADRV9025_GPIO_ANA_00 = 0,   /*!< Select GPIO_ANA_00*/
    ADI_ADRV9025_GPIO_ANA_01,       /*!< Select GPIO_ANA_01*/
    ADI_ADRV9025_GPIO_ANA_02,       /*!< Select GPIO_ANA_02*/
    ADI_ADRV9025_GPIO_ANA_03,       /*!< Select GPIO_ANA_03*/
    ADI_ADRV9025_GPIO_ANA_04,       /*!< Select GPIO_ANA_04*/
    ADI_ADRV9025_GPIO_ANA_05,       /*!< Select GPIO_ANA_05*/
    ADI_ADRV9025_GPIO_ANA_06,       /*!< Select GPIO_ANA_06*/
    ADI_ADRV9025_GPIO_ANA_07,       /*!< Select GPIO_ANA_07*/
    ADI_ADRV9025_GPIO_ANA_INVALID   /*!< Invalid Analog Gpio*/
} adi_adrv9025_Gpio3v3PinSel_e;

/**
 *  \brief Enum to set the low voltage GPIO mode
 */
typedef enum adi_adrv9025_GpioOutputModes
{
    ADI_ADRV9025_GPIO_MONITOR_MODE      = 0,    /*!< Allows a choice of debug signals to output from ADRV9025 to monitor the state of the device */
    ADI_ADRV9025_GPIO_BITBANG_MODE      = 3,    /*!< Manual mode, API function sets output pin levels and reads input pin levels */
    ADI_ADRV9025_GPIO_ARM_OUT_MODE      = 9,    /*!< Allows internal ARM processor to output on GPIO pins */
    ADI_ADRV9025_GPIO_SLICER_OUT_MODE   = 10    /*!< Allows Slicer active configuration to the GPIO output  pins */
} adi_adrv9025_GpioOutputModes_e;

/**
 *  \brief Enum of unique error codes for the ADRV9025 GPIO API functions.
 * Each error condition in the library should get its own enum value to allow
 * easy debug of errors.
 */
typedef enum adi_adrv9025_GpioErrCodes
{
    ADI_ADRV9025_ERR_GPIO_OK = 0,
    ADI_ADRV9025_ERR_MONITOR_OUT_INDEX_RANGE,
    ADI_ADRV9025_ERR_GETGPIOMON_INDEX_NULL_PARM,
    ADI_ADRV9025_ERR_GETGPIOMON_MONITORMASK_NULL_PARM,
    ADI_ADRV9025_ERR_GETGPIO_OE_NULL_PARM,
    ADI_ADRV9025_ERR_GPIO_OE_INV_PARM,
    ADI_ADRV9025_ERR_GPIO_SRC_INV_PARM,
    ADI_ADRV9025_ERR_GETGPIO_SRC_NULL_PARM,
    ADI_ADRV9025_ERR_GPIO_LEVEL_INV_PARM,
    ADI_ADRV9025_ERR_GETGPIO_LEVEL_NULL_PARM,
    ADI_ADRV9025_ERR_GETGPIO_SETLEVEL_NULL_PARM,
    ADI_ADRV9025_ERR_SETUPAUXDAC_NULL_PARM,
    ADI_ADRV9025_ERR_SETUPAUXDAC_INV_10BIT_AUXDACCODE,
    ADI_ADRV9025_ERR_SETUPAUXDAC_INV_12BIT_AUXDACCODE,
    ADI_ADRV9025_ERR_WRITEAUXDAC_INV_10BIT_AUXDACCODE,
    ADI_ADRV9025_ERR_WRITEAUXDAC_INV_12BIT_AUXDACCODE,
    ADI_ADRV9025_ERR_WRITEAUXDAC_INV_AUXDACINDEX,
    ADI_ADRV9025_ERR_SETUPAUXDAC_INV_RESOLUTION,
    ADI_ADRV9025_ERR_GPIO3V3_OE_INV_PARM,
    ADI_ADRV9025_ERR_GETGPIO3V3_OE_NULL_PARM,
    ADI_ADRV9025_ERR_GPINT_OK,
    ADI_ADRV9025_ERR_GPINT_STATUS_NULL_PARM,
    ADI_ADRV9025_ERR_GPINT_GPINTDIAG_NULL_PARM,
    ADI_ADRV9025_ERR_GPINT_NO_SOURCE_FOUND,
    ADI_ADRV9025_ERR_GPINT_SOURCE_NOT_IMPLEMENTED,
    ADI_ADRV9025_ERR_GPINT_CLKPLL_UNLOCKED,
    ADI_ADRV9025_ERR_GPINT_RFPLL_UNLOCKED,
    ADI_ADRV9025_ERR_GPINT_AUXPLL_UNLOCKED,
    ADI_ADRV9025_ERR_GPINT_ARM_WATCHDOG_TIMEOUT,
    ADI_ADRV9025_ERR_GPINT_ARM_FORCE_GPINT,
    ADI_ADRV9025_ERR_GPINT_ARM_SYSTEM_ERROR,
    ADI_ADRV9025_ERR_GPINT_ARM_DATA_PARITY_ERROR,
    ADI_ADRV9025_ERR_GPINT_ARM_PROG_PARITY_ERROR,
    ADI_ADRV9025_ERR_GPINT_ARM_CALIBRATION_ERROR,
    ADI_ADRV9025_ERR_GPINT_FRAMERA,
    ADI_ADRV9025_ERR_GPINT_DEFRAMERA,
    ADI_ADRV9025_ERR_GPINT_FRAMERB,
    ADI_ADRV9025_ERR_GPINT_DEFRAMERB,
    ADI_ADRV9025_ERR_GPINT_PA_PROTECT_CH1,
    ADI_ADRV9025_ERR_GPINT_PA_PROTECT_CH2,
    ADI_ADRV9025_ERR_GPINT_STREAM_ERROR,
    ADI_ADRV9025_ERR_GPIO_NUMBER_OF_ERRORS /* Keep this ENUM last as a reference to the total number of error enum values */
} adi_adrv9025_GpioErrCodes_e;

/**
* \brief Enum for selecting the GP_INT channel
*/
typedef enum adi_adrv9025_gpMaskSelect
{
    ADI_ADRV9025_GPINT0,
    ADI_ADRV9025_GPINT1,
    ADI_ADRV9025_GPINTALL,
    ADI_ADRV9025_GPINT_NUMBER_OF_CHANNELS /* Keep this ENUM last as a reference to the total number of gp channel enum values */
} adi_adrv9025_gpMaskSelect_e;

/**
* \brief data structure holding the gp interrupt mask values
*/
typedef struct adi_adrv9025_gp_MaskArray
{
    uint64_t gpInt0Mask;
    uint64_t gpInt1Mask;
} adi_adrv9025_gp_MaskArray_t;

/**
 * \brief GPIO settings for SPI2 TxAtten select
 */
typedef enum adi_adrv9025_Spi2TxAttenGpioSel
{
    ADI_ADRV9025_SPI2_TXATTEN_DISABLE   = 0,    /*!< Disable GPIO for SPI2 Tx Attenuation select */
    ADI_ADRV9025_SPI2_TXATTEN_GPIO4     = 1,    /*!< Select GPIO4 for SPI2 Tx Attenuation select */
    ADI_ADRV9025_SPI2_TXATTEN_GPIO5     = 2,    /*!< Select GPIO5 for SPI2 Tx Attenuation select */
    ADI_ADRV9025_SPI2_TXATTEN_GPIO6     = 3,    /*!< Select GPIO6 for SPI2 Tx Attenuation select */
    ADI_ADRV9025_SPI2_TXATTEN_GPIO7     = 4,    /*!< Select GPIO7 for SPI2 Tx Attenuation select */
    ADI_ADRV9025_SPI2_TXATTEN_GPIO8     = 5,    /*!< Select GPIO8 for SPI2 Tx Attenuation select */
    ADI_ADRV9025_SPI2_TXATTEN_GPIO9     = 6,    /*!< Select GPIO9 for SPI2 Tx Attenuation select */
    ADI_ADRV9025_SPI2_TXATTEN_GPIO10    = 7,    /*!< Select GPIO10 for SPI2 Tx Attenuation select */
    ADI_ADRV9025_SPI2_TXATTEN_GPIO11    = 8,    /*!< Select GPIO11 for SPI2 Tx Attenuation select */
    ADI_ADRV9025_SPI2_TXATTEN_GPIO12    = 9,    /*!< Select GPIO12 for SPI2 Tx Attenuation select */
    ADI_ADRV9025_SPI2_TXATTEN_GPIO13    = 10,   /*!< Select GPIO13 for SPI2 Tx Attenuation select */
    ADI_ADRV9025_SPI2_TXATTEN_GPIO14    = 11,   /*!< Select GPIO14 for SPI2 Tx Attenuation select */
    ADI_ADRV9025_SPI2_TXATTEN_GPIO15    = 12,   /*!< Select GPIO15 for SPI2 Tx Attenuation select */
    ADI_ADRV9025_SPI2_TXATTEN_GPIO16    = 13,   /*!< Select GPIO16 for SPI2 Tx Attenuation select */
    ADI_ADRV9025_SPI2_TXATTEN_GPIO17    = 14,   /*!< Select GPIO17 for SPI2 Tx Attenuation select */
    ADI_ADRV9025_SPI2_TXATTEN_GPIO18    = 15    /*!< Select GPIO18 for SPI2 Tx Attenuation select */
} adi_adrv9025_Spi2TxAttenGpioSel_e;

/**
* \brief GP_INT status general structure
*/
typedef struct adi_adrv9025_gpIntStatus
{
    uint64_t    gp_Interrupt_Status;
    uint64_t    gp_Int_Irq_Mask;
    uint64_t    gp_Int_Active_Sources;
    uint64_t    gp_Int_Save_Irq_Mask0;
    uint64_t    gp_Int_Save_Irq_Mask1;
    uint64_t    gp_Int_StickyBit_Mask;
    int32_t     gp_Int_Recovery_Action;
    const char* gp_Int_Error_Message;
} adi_adrv9025_gpIntStatus_t;

/**
*  \brief Enum to select AuxDAC
*  AuxDAC0-3 use AuxDACs located at Analog Tx1
*  AuxDAC4-7 use AuxDACs located at Analog Tx2
*/
typedef enum adi_adrv9025_AuxDacs
{
    ADI_ADRV9025_AUXDAC0 = 0x01,    /*!< AuxDAC0 */
    ADI_ADRV9025_AUXDAC1 = 0x02,    /*!< AuxDAC1 */
    ADI_ADRV9025_AUXDAC2 = 0x04,    /*!< AuxDAC2 */
    ADI_ADRV9025_AUXDAC3 = 0x08,    /*!< AuxDAC3 */
    ADI_ADRV9025_AUXDAC4 = 0x10,    /*!< AuxDAC4 */
    ADI_ADRV9025_AUXDAC5 = 0x20,    /*!< AuxDAC5 */
    ADI_ADRV9025_AUXDAC6 = 0x40,    /*!< AuxDAC6 */
    ADI_ADRV9025_AUXDAC7 = 0x80     /*!< AuxDAC7 */
} adi_adrv9025_AuxDacs_e;

/**
*  \brief Data structure to hold ADRV9025 AuxDAC Configuration
*/
typedef struct adi_adrv9025_AuxDacCfg
{
    uint32_t auxDacMask; /*!< AuxDAC Selection - Bit0:AuxDAC0 | Bit1:AuxDAC1 | ... | Bit7:AuxDAC7 */
    uint8_t  enable;     /*!< 1:Enable selected AuxDAC, 0:Disable selected AuxDAC */
} adi_adrv9025_AuxDacCfg_t;

/**
*  \brief Data structure to hold ADRV9025 AuxDAC Value
*/
typedef struct adi_adrv9025_AuxDacValue
{
    uint32_t auxDacMask; /*!< AuxDAC Selection - Bit0:AuxDAC0 | Bit1:AuxDAC1 | ... | Bit7:AuxDAC7 */
    uint16_t dacWord;    /*!< 12 bit AuxDAC word - 0-4095 maps to 0-1.8V*/
} adi_adrv9025_AuxDacValue_t;

/**
*  \brief Data structure to hold ADRV9025 AuxDAC Value
*/
typedef struct adi_adrv9025_AuxDacVoltage
{
    uint32_t auxDacMask; /*!< AuxDAC Selection - Bit0:AuxDAC0 | Bit1:AuxDAC1 | ... | Bit7:AuxDAC7 */
    uint16_t dacVoltage; /*!< AuxDAC voltage between 0-1800mV*/
} adi_adrv9025_AuxDacVoltage_t;

/**
 * \brief Aux ADC input selection
 *
 * Two aux ADCs are available for use and both have two available inputs 0
 * and 1 which can be selected using a common set of enumerations. A specific
 * ADC input is target by selecting both the desired ADC
 * (see adi_adrv9025_AuxAdcSelect_e) and the desired input channel with this
 * enumeration.
 */
typedef enum adi_adrv9025_AuxAdcInputSelect
{
    ADI_ADRV9025_AUXADC_INPUT_0 = 0x3,  /*!< Aux ADC input 0 selection */
    ADI_ADRV9025_AUXADC_INPUT_1 = 0x2   /*!< Aux ADC input 1 selection */
} adi_adrv9025_AuxAdcInputSelect_e;

/**
 * \brief Aux ADC selection
 *
 * Select which ADC to target for either configuration (see adi_adrv9025_AuxAdcCfg_t)
 * or value reading (see adi_adrv9025_AuxAdcValue_t)
 */
typedef enum adi_adrv9025_AuxAdcSelect
{
    ADI_ADRV9025_AUXADC_A = 0x0,    /*!< Aux ADC A selection */
    ADI_ADRV9025_AUXADC_B = 0x1,    /*!< Aux ADC B selection */
    ADI_ADRV9025_AUXADC_NUM = 0x2   /*!< Number of aux ADCs */
} adi_adrv9025_AuxAdcSelect_e;

/**
 * \brief Aux ADC clock divide control selection
 *
 * The aux ADC's input clock is sourced from the device clock.
 * The ADC's input clock should be nominally set to 20 MHz.
 */
typedef enum adi_adrv9025_AuxAdcClkDivide
{
    ADI_ADRV9025_AUXADC_CLKDIVIDE_32 = 0,       /*!< Input clock divide by 32 */
    ADI_ADRV9025_AUXADC_CLKDIVIDE_1,            /*!< No clock divide */
    ADI_ADRV9025_AUXADC_CLKDIVIDE_2,            /*!< Input clock divide by 2  */
    ADI_ADRV9025_AUXADC_CLKDIVIDE_3,            /*!< Input clock divide by 3  */
    ADI_ADRV9025_AUXADC_CLKDIVIDE_4,            /*!< Input clock divide by 4  */
    ADI_ADRV9025_AUXADC_CLKDIVIDE_5,            /*!< Input clock divide by 5  */
    ADI_ADRV9025_AUXADC_CLKDIVIDE_6,            /*!< Input clock divide by 6  */
    ADI_ADRV9025_AUXADC_CLKDIVIDE_7,            /*!< Input clock divide by 7  */
    ADI_ADRV9025_AUXADC_CLKDIVIDE_8,            /*!< Input clock divide by 8  */
    ADI_ADRV9025_AUXADC_CLKDIVIDE_9,            /*!< Input clock divide by 9  */
    ADI_ADRV9025_AUXADC_CLKDIVIDE_10,           /*!< Input clock divide by 10 */
    ADI_ADRV9025_AUXADC_CLKDIVIDE_11,           /*!< Input clock divide by 11 */
    ADI_ADRV9025_AUXADC_CLKDIVIDE_12,           /*!< Input clock divide by 12 */
    ADI_ADRV9025_AUXADC_CLKDIVIDE_13,           /*!< Input clock divide by 13 */
    ADI_ADRV9025_AUXADC_CLKDIVIDE_14,           /*!< Input clock divide by 14 */
    ADI_ADRV9025_AUXADC_CLKDIVIDE_15,           /*!< Input clock divide by 15 */
    ADI_ADRV9025_AUXADC_CLKDIVIDE_NUM_DIVIDE    /*!< Number of clock divide settings*/
} adi_adrv9025_AuxAdcClkDivide_e;

/**
 * \brief Aux ADC Enable
 */
typedef enum adi_adrv9025_AuxAdcEnable
{
    ADI_ADRV9025_AUXADC_DISABLE = 0,    /*!< Aux ADC disabled */
    ADI_ADRV9025_AUXADC_ENABLE,         /*!< Aux ADC enabled */
    ADI_ADRV9025_AUXADC_ENABLE_NUM      /*!< Number of enable enumerations*/
} adi_adrv9025_AuxAdcEnable_e;

/**
 * \brief Aux ADC configuration
 *
 * Configuration structure used to configure the selected aux ADC.
 * Note that a specific ADC input can be selected by selecting the
 * appropriate ADC using auxAdcSelect and the ADC's input using auxAdcInputSelect.
 * The current mapping of ADC inputs to physical input pins can be seen below.
 *
 * auxAdcSelect            |  auxAdcInputSelect           |  Device AUXADC pin
 * ------------------------|------------------------------|--------------------
 * ADI_ADRV9025_AUXADC_A   | ADI_ADRV9025_AUXADC_INPUT_0  |  0
 * ADI_ADRV9025_AUXADC_A   | ADI_ADRV9025_AUXADC_INPUT_1  |  1
 * ADI_ADRV9025_AUXADC_B   | ADI_ADRV9025_AUXADC_INPUT_0  |  2
 * ADI_ADRV9025_AUXADC_B   | ADI_ADRV9025_AUXADC_INPUT_1  |  3
 *
 * The ADC's output equation is the following:
 * Dout = 4096*(Vin - 0.5v)/(Vdd - Vss) + 2048.
 * Where Vdd in this case is the 1.0V domain: VCONV1_1P0 for Aux ADC A or
 * VCONV2_1P0 for Aux ADC B.
 *
 */
typedef struct adi_adrv9025_AuxAdcCfg
{
    adi_adrv9025_AuxAdcEnable_e      auxAdcEnable;      /*!< Enable or disable the selected ADC */
    adi_adrv9025_AuxAdcSelect_e      auxAdcSelect;      /*!< Select which ADC to configure */
    adi_adrv9025_AuxAdcInputSelect_e auxAdcInputSelect; /*!< Select which ADC input to use */
    adi_adrv9025_AuxAdcClkDivide_e   auxAdcClkDivide;   /*!< Clock divide setting */
} adi_adrv9025_AuxAdcCfg_t;

/**
 * \brief Aux ADC sample
 */
typedef struct adi_adrv9025_AuxAdcValue
{
    adi_adrv9025_AuxAdcSelect_e auxAdcSelect; /*!< The ADC the sample originates from */
    uint16_t                    auxAdcValue;  /*!< The 12 bit ADC sample */
} adi_adrv9025_AuxAdcValue_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_GPIO_TYPES_H_ */
