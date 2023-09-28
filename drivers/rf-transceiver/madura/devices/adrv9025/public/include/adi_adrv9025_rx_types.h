/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adi_adrv9025_rx_types.h
 * \brief Contains ADRV9025 API Rx datapath data types
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_RX_TYPES_H_
#define _ADI_ADRV9025_RX_TYPES_H_

#include "adi_adrv9025_radioctrl_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 *  \brief Enum of possible Rx gain table SRAM base addresses
 */
typedef enum adrv9025_RxChanGainTableBaseAddr_e
{
    ADI_ADRV9025_ALL_RX_CHANNELS_GAIN_TABLE_BASEADDR    = 0xD0800000,   /*!< Select gain table SRAM base address for all Rx Channels */
    ADI_ADRV9025_ALL_ORX_CHANNELS_GAIN_TABLE_BASEADDR   = 0xE0800000,   /*!< Select gain table SRAM base address for all ORx Channels */
    ADI_ADRV9025_RX1_GAIN_TABLE_BASEADDR                = 0x80800000,   /*!< Select gain table SRAM base address for Rx1 Channel */
    ADI_ADRV9025_RX2_GAIN_TABLE_BASEADDR                = 0x88800000,   /*!< Select gain table SRAM base address for Rx2 Channel */
    ADI_ADRV9025_RX3_GAIN_TABLE_BASEADDR                = 0x90800000,   /*!< Select gain table SRAM base address for Rx3 Channel */
    ADI_ADRV9025_RX4_GAIN_TABLE_BASEADDR                = 0x98800000,   /*!< Select gain table SRAM base address for Rx4 Channel */
    ADI_ADRV9025_ORX1_ORX2_GAIN_TABLE_BASEADDR          = 0xC0800000,   /*!< Select gain table SRAM base address for ORx1 Channel */
    ADI_ADRV9025_ORX3_ORX4_GAIN_TABLE_BASEADDR          = 0xC8800000    /*!< Select gain table SRAM base address for ORx2 Channel */
} adrv9025_RxChanGainTableBaseAddr_e;

/**
 *  \brief Enum of possible Rx channel enables
 */
typedef enum adi_adrv9025_RxChannels
{
    ADI_ADRV9025_RXOFF  = 0x00,     /*!< No Rx/ORx channels are enabled */
    ADI_ADRV9025_RX1    = 0x01,     /*!< Rx1 channel enabled */
    ADI_ADRV9025_RX2    = 0x02,     /*!< Rx2 channel enabled */
    ADI_ADRV9025_RX3    = 0x04,     /*!< Rx3 channel enabled */
    ADI_ADRV9025_RX4    = 0x08,     /*!< Rx4 channel enabled */
    ADI_ADRV9025_ORX1   = 0x10,     /*!< ORx1 channel enabled */
    ADI_ADRV9025_ORX2   = 0x20,     /*!< ORx2 channel enabled */
    ADI_ADRV9025_ORX3   = 0x40,     /*!< ORx3 channel enabled */
    ADI_ADRV9025_ORX4   = 0x80,     /*!< ORx4 channel enabled */
    ADI_ADRV9025_LB12   = 0x100,    /*!< Tx1 or Tx2 internal loopback into ORx1/2 channel enabled, NOT Used for RxDDC Mode settings */
    ADI_ADRV9025_LB34   = 0x200     /*!< Tx3 or Tx4 internal loopback into ORx3/4 channel enabled, NOT Used for RxDDC Mode settings */
} adi_adrv9025_RxChannels_e;

/**
 *  \brief Enum to set the Rx Gain control mode
 */
typedef enum
{
    ADI_ADRV9025_MGC     = 0, /*!< Manual Gain Control */
    ADI_ADRV9025_AGCFAST = 1, /*!< Fast Attack AGC Mode */
    ADI_ADRV9025_AGCSLOW = 2, /*!< Slow Loop AGC  */
    ADI_ADRV9025_HYBRID  = 3  /*!< Hybrid AGC Gain Control */
} adi_adrv9025_RxAgcMode_e;

/**
 *  \brief Enum to hold ADRV9025 Floating Point Formatter format which indicates order of sign significand and exponent
 */
typedef enum adi_adrv9025_FpFloatDataFormat
{
    ADI_ADRV9025_FP_FORMAT_SIGN_EXP_SIGNIFICAND = 0,    /*!< Floating point format of type {Sign, Significand, Exponent} */
    ADI_ADRV9025_FP_FORMAT_SIGN_SIGNIFICAND_EXP = 1     /*!< Floating point format of type {Sign, Exponent, Significand} */
} adi_adrv9025_FpFloatDataFormat_e;

/**
 *  \brief Enum to hold ADRV9025 Floating Point Formatter NaN encode enable
 */
typedef enum adi_adrv9025_FpNanEncode
{
    ADI_ADRV9025_FP_FORMAT_NAN_ENCODE_DISABLE   = 0,  /*!< Does not reserve highest value of exponent for NaN */
    ADI_ADRV9025_FP_FORMAT_NAN_ENCODE_ENABLE    = 1    /*!< Eeserve highest value of exponent for NaN */
} adi_adrv9025_FpNanEncode_e;

/**
 *  \brief Enum to hold ADRV9025 Hide Leading One selection
 */
typedef enum adi_adrv9025_FpHideLeadingOne
{
    ADI_ADRV9025_FP_FORMAT_HIDE_LEADING_ONE_DISABLE = 0,    /*!< Sets leading one to be at the MSB of the significand */
    ADI_ADRV9025_FP_FORMAT_HIDE_LEADING_ONE_ENABLE  = 1     /*!< Hides the leading one in significand to be compatible to the IEEE754 specification */
} adi_adrv9025_FpHideLeadingOne_e;

/**
 *  \brief Enum to hold ADRV9025 Floating Point Formatter number of exponent bits
 */
typedef enum adi_adrv9025_FpExponentModes
{
    ADI_ADRV9025_2_EXPONENTBITS = 0,    /*!< Floating point values have 2 exponent bits, 13 significand bits, 1 sign bit */
    ADI_ADRV9025_3_EXPONENTBITS = 1,    /*!< Floating point values have 3 exponent bits, 12 significand bits, 1 sign bit */
    ADI_ADRV9025_4_EXPONENTBITS = 2,    /*!< Floating point values have 4 exponent bits, 11 significand bits, 1 sign bit */
    ADI_ADRV9025_5_EXPONENTBITS = 3     /*!< Floating point values have 5 exponent bits, 10 significand bits, 1 sign bit  */
} adi_adrv9025_FpExponentModes_e;

/**
 *  \brief Enum to hold ADRV9025 Floating Point Formatter rounding modes for the Rx data path
 */
typedef enum adi_adrv9025_FpRoundModes
{
    ADI_ADRV9025_ROUND_TO_EVEN          = 0,    /*!< Round floating point ties to an even value          */
    ADI_ADRV9025_ROUNDTOWARDS_POSITIVE  = 1,    /*!< Round floating point toward the positive direction  */
    ADI_ADRV9025_ROUNDTOWARDS_NEGATIVE  = 2,    /*!< Round floating point toward the negative direction  */
    ADI_ADRV9025_ROUNDTOWARDS_ZERO      = 3,    /*!< Round floating point toward the zero direction      */
    ADI_ADRV9025_ROUND_FROM_EVEN        = 4     /*!< Round floating point ties away from even value      */
} adi_adrv9025_FpRoundModes_e;

/**
 *  \brief Enum to hold ADRV9025 Rx attenuation values that are used when the
 *         floating point format is enabled
 */
typedef enum adi_adrv9025_FpAttenSteps
{
    ADI_ADRV9025_FPATTEN_24DB       = 4,    /*!< Set Rx attenuation to 24 dB when Rx Data format set to floating point mode  */
    ADI_ADRV9025_FPATTEN_18DB       = 5,    /*!< Set Rx attenuation to 18 dB when Rx Data format set to floating point mode  */
    ADI_ADRV9025_FPATTEN_12DB       = 6,    /*!< Set Rx attenuation to 12 dB when Rx Data format set to floating point mode */
    ADI_ADRV9025_FPATTEN_6DB        = 7,    /*!< Set Rx attenuation to 6 dB when Rx Data format set to floating point mode */
    ADI_ADRV9025_FPATTEN_0DB        = 0,    /*!< Set Rx attenuation to 0 dB when Rx Data format set to floating point mode */
    ADI_ADRV9025_FPATTEN_MINUS6DB   = 1,    /*!< Set Rx attenuation to -6 dB when Rx Data format set to floating point mode */
    ADI_ADRV9025_FPATTEN_MINUS12DB  = 2,    /*!< Set Rx attenuation to -12 dB when Rx Data format set to floating point mode */
    ADI_ADRV9025_FPATTEN_MINUS18DB  = 3     /*!< Set Rx attenuation to -18 dB when Rx Data format set to floating point mode */
} adi_adrv9025_FpAttenSteps_e;

/**
 *  \brief Enum to select the four Data Formatting options
 */
typedef enum adi_adrv9025_RxDataFormatModes
{
    ADI_ADRV9025_GAIN_COMPENSATION_DISABLED = 0,        /*!< Gain Compensation and Data Formatting are disabled */
    ADI_ADRV9025_GAIN_WITH_FLOATING_POINT,              /*!< Gain Compensation enabled with floating point data formatting enabled and internal slicer enabled */
    ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO,      /*!< Gain Compensation enabled with integer data formatting and internal slicer enabled with no GPIO Slicer Position output. The slicer position is embedded in the JESD data */
    ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE,   /*!< Gain Compensation enabled with integer data formatting and internal slicer enabled with GPIO Slicer Position output. The least significant 3 bits of the slicer control word are output on the GPIO */
    ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE,   /*!< Gain Compensation enabled with integer data formatting and internal slicer enabled with GPIO Slicer Position output. All 4 bits of the slicer control word are output on the GPIOs */
    ADI_ADRV9025_GAIN_WITH_EXTERNAL_SLICER,             /*!< Gain Compensation enabled with integer data formatting and external slicer enabled */
    ADI_ADRV9025_EMBED_OVERLOAD_MONITOR_DATA            /*!< Embed 1-bit/2 bit monitor data on the 2 least significant bits of a 16-bit 2's Complement JESD Rx data */
} adi_adrv9025_RxDataFormatModes_e;

/**
 *  \brief Enum to hold ADRV9025 Gain Slicer external pin gain step size.  Slicer gain in this mode is determined by multiplying the step size by the step size selection from the base band processor.
 */
typedef enum adi_adrv9025_ExtSlicerStepSizes
{
    ADI_ADRV9025_EXTSLICER_STEPSIZE_1DB = 0,    /*!< Set Gain Slicer External gain step size to 1dB  */
    ADI_ADRV9025_EXTSLICER_STEPSIZE_2DB,        /*!< Set Gain Slicer External gain step size to 2dB  */
    ADI_ADRV9025_EXTSLICER_STEPSIZE_3DB,        /*!< Set Gain Slicer External gain step size to 3dB  */
    ADI_ADRV9025_EXTSLICER_STEPSIZE_4DB,        /*!< Set Gain Slicer External gain step size to 4dB  */
    ADI_ADRV9025_EXTSLICER_STEPSIZE_6DB         /*!< Set Gain Slicer External gain step size to 6dB  */
} adi_adrv9025_ExtSlicerStepSizes_e;

/**
 *  \brief Enum to hold ADRV9025 Internal Gain Slicer gain step size.  Slicer gain in this mode is determined by multiplying the step size by the step size selection from the base band processor.
 */
typedef enum adi_adrv9025_IntSlicerStepSizes
{
    ADI_ADRV9025_INTSLICER_STEPSIZE_1DB = 0,    /*!< Set Gain Slicer Internal gain step size to 1dB  */
    ADI_ADRV9025_INTSLICER_STEPSIZE_2DB,        /*!< Set Gain Slicer Internal gain step size to 2dB  */
    ADI_ADRV9025_INTSLICER_STEPSIZE_3DB,        /*!< Set Gain Slicer Internal gain step size to 3dB  */
    ADI_ADRV9025_INTSLICER_STEPSIZE_4DB,        /*!< Set Gain Slicer Internal gain step size to 4dB  */
    ADI_ADRV9025_INTSLICER_STEPSIZE_6DB,        /*!< Set Gain Slicer Internal gain step size to 6dB  */
    ADI_ADRV9025_INTSLICER_STEPSIZE_8DB         /*!< Set Gain Slicer Internal gain step size to 8dB  */
} adi_adrv9025_IntSlicerStepSizes_e;

/**
 *  \brief Enum to hold ADRV9025 Rx Gain Slicer external pin GPIO selection options
 */
typedef enum adi_adrv9025_RxExtSlicerGpioSel_e
{
    ADI_ADRV9025_EXTSLICER_RX_GPIO_DISABLE      = 0,    /*!< No GPIO assigned to external slicer */
    ADI_ADRV9025_EXTSLICER_RX_GPIO_2_DOWNTO_0   = 1,    /*!< Select Rx Gain Slicer External GPIO2, GPIO1, GPIO0 */
    ADI_ADRV9025_EXTSLICER_RX_GPIO_5_DOWNTO_3   = 2,    /*!< Select Rx Gain Slicer External GPIO5, GPIO4, GPIO3 */
    ADI_ADRV9025_EXTSLICER_RX_GPIO_8_DOWNTO_6   = 3,    /*!< Select Rx Gain Slicer External GPIO8, GPIO7, GPIO6 */
    ADI_ADRV9025_EXTSLICER_RX_GPIO_11_DOWNTO_9  = 4,    /*!< Select Rx Gain Slicer External GPIO11, GPIO10, GPIO9 */
    ADI_ADRV9025_EXTSLICER_RX_GPIO_14_DOWNTO_12 = 5,    /*!< Select Rx Gain Slicer External GPIO14, GPIO13, GPIO12 */
    ADI_ADRV9025_EXTSLICER_RX_GPIO_17_DOWNTO_15 = 6,    /*!< Select Rx Gain Slicer External GPIO17, GPIO16, GPIO15 */
    ADI_ADRV9025_EXTSLICER_RX_GPIO_INVALID      = 7
} adi_adrv9025_RxExtSlicerGpioSel_e;

/**
 *  \brief Enum to hold ADRV9025 Rx1 Gain Slicer external pin GPIO selection options
 */
typedef enum adi_adrv9025_Rx1ExtSlicerGpioSel
{
    ADI_ADRV9025_EXTSLICER_RX1_GPIO0_1_2 = 0,   /*!< Select Rx1 Gain Slicer External GPIO0, GPIO1, GPIO2 */
    ADI_ADRV9025_EXTSLICER_RX1_GPIO5_6_7,       /*!< Select Rx1 Gain Slicer External GPIO5, GPIO6, GPIO7 */
    ADI_ADRV9025_EXTSLICER_RX1_GPIO8_9_10,      /*!< Select Rx1 Gain Slicer External GPIO8, GPIO9, GPIO10 */
    ADI_ADRV9025_EXTSLICER_RX1_GPIO_DISABLE     /*!< Select Rx1 Disable Gain Slicer External GPIO */
} adi_adrv9025_Rx1ExtSlicerGpioSel_e;

/**
 *  \brief Enum to hold ADRV9025 integer mode parity selection for 3 bit slicer mode
 */
typedef enum adi_adrv9025_RxIntParity
{
    ADI_ADRV9025_3BIT_SLICER_EVEN_PARITY    = 0,    /*!< Even parity for the unused 4th bit in 3 bit slicer mode */
    ADI_ADRV9025_3BIT_SLICER_ODD_PARITY     = 1,    /*!< Odd parity for the unused 4th bit in 3 bit slicer mode */
    ADI_ADRV9025_NO_PARITY                  = 2     /*!< Disable parity */
} adi_adrv9025_RxIntParity_e;

/**
 *  \brief Enum to hold ADRV9025 Rx2 Gain Slicer external pin GPIO selection options
 */
typedef enum adi_adrv9025_Rx2ExtSlicerGpioSel
{
    ADI_ADRV9025_EXTSLICER_RX2_GPIO11_12_13 = 0,    /*!< Select Rx2 Gain Slicer External GPIO11, GPIO12, GPIO13 */
    ADI_ADRV9025_EXTSLICER_RX2_GPIO5_6_7    = 1,    /*!< Select Rx2 Gain Slicer External GPIO5, GPIO6, GPIO7 */
    ADI_ADRV9025_EXTSLICER_RX2_GPIO_DISABLE = 3     /*!< Select Rx2 Disable Gain Slicer External GPIO */
} adi_adrv9025_Rx2ExtSlicerGpioSel_e;

/**
 *  \brief Enum to hold ADRV9025 integer mode number of embedded slicer bits and positions
 */
typedef enum adi_adrv9025_RxSlicerEmbeddedBits
{
    ADI_ADRV9025_NO_EMBEDDED_SLICER_BITS = 0,               /*!< Disabled all embedded slicer bits  */
    ADI_ADRV9025_EMBED_1_SLICERBIT_AT_MSB,                  /*!< Embeds 1 slicer bits on I and 1 slicer bits on Q and the MSB position in the data frame */
    ADI_ADRV9025_EMBED_1_SLICERBIT_AT_LSB,                  /*!< Embeds 1 slicer bits on I and 1 slicer bits on Q and the LSB position in the data frame */
    ADI_ADRV9025_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER,    /*!< Embeds 2 slicer bits on I and 2 slicer bits on Q and the MSB position in the data frame whereas only 3 bits are used for slicer ctrl (2 bits on I, 1 bit Q) */
    ADI_ADRV9025_EMBED_2_SLICERBITS_AT_LSB_3_BIT_SLICER,    /*!< Embeds 2 slicer bits on I and 2 slicer bits on Q and the LSB position in the data frame whereas only 3 bits are used for slicer ctrl (2 bits on I, 1 bit Q) */
    ADI_ADRV9025_EMBED_2_SLICERBITS_AT_MSB_4_BIT_SLICER,    /*!< Embeds 2 slicer bits on I and 2 slicer bits on Q and the MSB position in the data frame and all 4 bits are used for slicer ctrl*/
    ADI_ADRV9025_EMBED_2_SLICERBITS_AT_LSB_4_BIT_SLICER     /*!< Embeds 2 slicer bits on I and 2 slicer bits on Q and the LSB position in the data frame and all 4 bits are used for slicer ctrl*/
} adi_adrv9025_RxSlicerEmbeddedBits_e;

/**
 *  \brief Enum to hold ADRV9025 slicer control word nibble position
 */
typedef enum adi_adrv9025_RxSlicerEmbeddedPos
{
    ADI_ADRV9025_LOWER_NIBBLE_ON_Q = 0, /*!< Place the lower control nibble on Q and the upper control nibble on I */
    ADI_ADRV9025_LOWER_NIBBLE_ON_I      /*!< Place the lower control nibble on I and the upper control nibble on Q */
} adi_adrv9025_RxSlicerEmbeddedPos_e;

/**
*  \brief Enum to hold ADRV9025 integer sample resolution
*/
typedef enum adi_adrv9025_RxIntSampleResolution
{
    ADI_ADRV9025_INTEGER_12BIT_2SCOMP = 0,  /*!< Selects integer sample 12 bit resolution with 2s compliment    */
    ADI_ADRV9025_INTEGER_12BIT_SIGNED,      /*!< Selects integer sample 12 bit resolution with signed magnitude */
    ADI_ADRV9025_INTEGER_16BIT_2SCOMP,      /*!< Selects integer sample 16 bit resolution with 2s compliment    */
    ADI_ADRV9025_INTEGER_16BIT_SIGNED,      /*!< Selects integer sample 16 bit resolution with signed magnitude */
    ADI_ADRV9025_INTEGER_24BIT_2SCOMP,      /*!< Selects integer sample 24 bit resolution with 2s compliment    */
    ADI_ADRV9025_INTEGER_24BIT_SIGNED        /*!< Selects integer sample 24 bit resolution with signed magnitude */
} adi_adrv9025_RxIntSampleResolution_e;

/**
*  \brief Enum to hold ADRV9025 Embedded Monitor Mode sources
*/
typedef enum adi_adrv9025_RxEmbeddedMonitorSrc
{
    ADI_ADRV9025_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED    = 0,    /*!< Embedded overload monitor data is not embedded for the selected bit position in 16 bit Rx data */
    ADI_ADRV9025_RX_EMB_MON_SRC_HB2_LOW_THRESH          = 3,    /*!< Selects digital HB2 low threshold monitor data to be embedded in Rx data */
    ADI_ADRV9025_RX_EMB_MON_SRC_HB2_HIGH_THRESH         = 4,    /*!< Selects digital HB2 high threshold monitor data to be embedded in Rx data */
    ADI_ADRV9025_RX_EMB_MON_SRC_APD_LOW_THRESH          = 5,    /*!< Selects analog peak detector low threshold monitor data to be embedded in Rx data */
    ADI_ADRV9025_RX_EMB_MON_SRC_APD_HIGH_THRESH         = 6,    /*!< Selects analog peak detector high threshold monitor data to be embedded in Rx data */
    ADI_ADRV9025_RX_EMB_MON_SRC_INVALID                 = 7     /*!< Invalid embedded monitoring source */
} adi_adrv9025_RxEmbeddedMonitorSrc_e;

/**
*  \brief Enum to hold ADRV9025 Embedded Monitor Mode HB2 Low source
*/
typedef enum adi_adrv9025_RxHb2LowEmbeddedMonitorSrc
{
    ADI_ADRV9025_HB2_LOW_SRC_OVRG_LOW                           = 0,    /*!< Selects digital HB2 lower threshold exceeded flag as HB2 low threshold monitor data */
    ADI_ADRV9025_HB2_LOW_SRC_OVRG_LOW_INT0                      = 1,    /*!< Selects digital HB2 interval 0 lower threshold exceeded flag as HB2 low threshold monitor data */
    ADI_ADRV9025_HB2_LOW_SRC_OVRG_LOW_INT1                      = 2,    /*!< Selects digital HB2 interval 1 lower threshold exceeded flag as HB2 low threshold monitor data */
    ADI_ADRV9025_HB2_LOW_SRC_OVRG_LOW_THRESH_CNTR_EXCEEDED      = 4,    /*!< Selects digital HB2 lower threshold exceeded counter flag as HB2 low threshold monitor data */
    ADI_ADRV9025_HB2_LOW_SRC_OVRG_LOW_INT0_THRESH_CNTR_EXCEEDED = 5,    /*!< Selects digital HB2 interval 0 lower threshold exceeded counter flag as HB2 low threshold monitor data */
    ADI_ADRV9025_HB2_LOW_SRC_OVRG_LOW_INT1_THRESH_CNTR_EXCEEDED = 6,    /*!< Selects digital HB2 interval 1 lower threshold exceeded counter flag as HB2 low threshold monitor data */
    ADI_ADRV9025_HB2_LOW_SRC_INVALID                                    /*!< Invalid HB2 low source */
} adi_adrv9025_RxHb2LowEmbeddedMonitorSrc_e;

/**
*  \brief Enum to hold ADRV9025 Embedded Monitor Mode HB2 High source
*/
typedef enum adi_adrv9025_RxHb2HighEmbeddedMonitorSrc
{
    ADI_ADRV9025_HB2_HIGH_SRC_OVRG_HIGH = 0,            /*!< Selects digital HB2 overrange high threshold exceeded flag as HB2 high threshold monitor data */
    ADI_ADRV9025_HB2_HIGH_SRC_OVRG_HIGH_CNTR_EXCEEDED,  /*!< Selects digital HB2 overrange high threshold counter exceeded flag as HB2 high threshold monitor data */
    ADI_ADRV9025_HB2_HIGH_SRC_INVALID                   /*!< Invalid HB2 high source */
} adi_adrv9025_RxHb2HighEmbeddedMonitorSrc_e;

/**
*  \brief Enum to hold ADRV9025 Embedded Monitor Mode Analog Peak Detector High source
*/
typedef enum adi_adrv9025_RxApdHighEmbeddedMonitorSrc
{
    ADI_ADRV9025_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED = 0,     /*!< Selects analog peak detector upper level blocker exceeded flag as APD high threshold monitor data */
    ADI_ADRV9025_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_CNTR_EXCEEDED,    /*!< Selects analog peak detector upper level blocker counter exceeded flag as APD high threshold monitor data */
    ADI_ADRV9025_APD_HIGH_SRC_INVALID                               /*!< Invalid APD high source */
} adi_adrv9025_RxApdHighEmbeddedMonitorSrc_e;

/**
*  \brief Enum to hold ADRV9025 Embedded Monitor Mode Analog Peak Detector Low source
*/
typedef enum adi_adrv9025_RxApdLowEmbeddedMonitorSrc
{
    ADI_ADRV9025_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED = 0,      /*!< Selects analog peak detector lower level blocker exceeded flag as APD low threshold monitor data */
    ADI_ADRV9025_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_CNTR_EXCEEDED,     /*!< Selects analog peak detector lower level blocker counter exceeded flag as APD low threshold monitor data */
    ADI_ADRV9025_APD_LOW_SRC_INVALID                                /*!< Invalid APD low source */
} adi_adrv9025_RxApdLowEmbeddedMonitorSrc_e;

/**
* \brief Enum to hold ADRV9025 RxNcoChannel options
*/
typedef enum adi_adrv9025_RxNcoChannel
{
    ADI_ADRV9025_RX1_NCO1A = 0,
    ADI_ADRV9025_RX1_NCO1B,
    ADI_ADRV9025_RX1_NCO2A,
    ADI_ADRV9025_RX1_NCO2B,
    ADI_ADRV9025_RX2_NCO1A,
    ADI_ADRV9025_RX2_NCO1B,
    ADI_ADRV9025_RX2_NCO2A,
    ADI_ADRV9025_RX2_NCO2B
} adi_adrv9025_RxNcoChannel_e;

/**
* \brief Enum to hold ADRV9025 RxNcoBand Select options
*/
typedef enum adi_adrv9025_RxNcoBandSelect
{
    ADI_ADRV9025_NCO_BANDA = 0,
    ADI_ADRV9025_NCO_BANDB = 1
} adi_adrv9025_RxNcoBandSelect_e;

/**
*  \brief Enum of possible DDC Modes
*/
typedef enum adi_adrv9025_RxDdc
{
    ADI_ADRV9025_RXDDC_BYPASS = 0,          /*!< No Half Band Enabled */
    ADI_ADRV9025_RXDDC_FILTERONLY,          /*!< Half Band Filters only */
    ADI_ADRV9025_RXDDC_INT2,                /*!< Half Band Interpolation by 2 */
    ADI_ADRV9025_RXDDC_DEC2,                /*!< Half Band Decimate by 2 */
    ADI_ADRV9025_RXDDC_BYPASS_REALIF,       /*!< No Half Band Enabled */
    ADI_ADRV9025_RXDDC_FILTERONLY_REALIF,   /*!< Half Band Filters only */
    ADI_ADRV9025_RXDDC_INT2_REALIF,         /*!< Half Band Interpolation by 2 */
    ADI_ADRV9025_RXDDC_DEC2_REALIF          /*!< Half Band Decimate by 2 */
} adi_adrv9025_RxDdc_e;

/**
* \brief Enum to hold ADRV9025 RxNco enable options
*/
typedef enum adi_adrv9025_RxNcoEnable
{
    ADI_ADRV9025_NCO_OFF          = 0x00,  /*!< No NCO enabled */
    ADI_ADRV9025_BAND_A_NCO_1     = 0x01,  /*!< Enable Band A on NCO 1 */
    ADI_ADRV9025_BAND_B_NCO_1     = 0x02,  /*!< Enable Band B on NCO 1 */
    ADI_ADRV9025_BAND_A_NCO_2     = 0x04,  /*!< Enable Band A on NCO 2 */
    ADI_ADRV9025_BAND_B_NCO_2     = 0x08,  /*!< Enable Band B on NCO 2 */
    ADI_ADRV9025_BAND_A_B_NCO_1_2 = 0x0F   /*!< Enable NCO 1 & 2, Band A & B */
} adi_adrv9025_RxNcoEnable_e;

/**
*  \brief Enum of possible Ext Ctrl pin output for ADRV9025
*/
typedef enum adi_adrv9025_RxExtCtrlPinOuputEnable
{
    ADI_ADRV9025_DISABLE_RX1_RX2_EXT_CTRL_GPIOS,            /*!< Disable Rx1 and Rx2 Ext Ctrl Word output on Analog GPIOs */
    ADI_ADRV9025_ENABLE_RX1_RX2_EXT_CTRL_GPIOS,             /*!< Enable Rx1 and Rx2 Ext Ctrl Word output on Analog GPIOs */
    ADI_ADRV9025_DISABLE_RX3_RX4_EXT_CTRL_GPIOS,            /*!< Disable Rx3 and Rx4 Ext Ctrl Word output on Analog GPIOs */
    ADI_ADRV9025_ENABLE_RX3_RX4_EXT_CTRL_GPIOS,             /*!< Enable Rx1 and Rx2 Ext Ctrl Word output on Analog GPIOs */
    ADI_ADRV9025_DISABLE_RX1_RX2_RX3_RX4_EXT_CTRL_GPIOS,    /*!< Disable Rx1,Rx2,Rx3 and Rx4 Ext Ctrl Word output on Analog GPIOs */
    ADI_ADRV9025_ENABLE_RX1_RX2_RX3_RX4_EXT_CTRL_GPIOS      /*!< Enable Rx1,Rx2,Rx3 and Rx4 Ext Ctrl Word output on Analog GPIOs */
} adi_adrv9025_RxExtCtrlPinOuputEnable_e;

/**
  * \brief Data structure to hold ADRV9025 Rx gain table row entry
  */
typedef struct adi_adrv9025_RxGainTableRow
{
    uint8_t  rxFeGain;    /*!< Rx Front End gain for a given gain index */
    uint8_t  extControl;  /*!< External LNA control word */
    uint8_t  adcTiaGain;  /*!< ADC and TIA control for a given gain index */
    int16_t  digGain;     /*!< Digital gain ranging from -18dB to 50dB (68dB total range) */
    uint16_t phaseOffset; /*!< 16 bit phase offset from 0 - 2pi in resolution of 0.005 degrees */
} adi_adrv9025_RxGainTableRow_t;

/**
 * \brief Data structure to hold ADRV9025 Rx dualband LNA gain table entries
 */
typedef struct adi_adrv9025_DualBandLnaGtRow
{
    uint8_t dualbandControl; /*!< The external control value to be output on the 3.3V GPIO's to control the LNA (values 0-3). */
    uint8_t dualbandGain;    /*!< The gain compensation value for the corresponding external control, used for RSSI and gain compensation.
                                  Range of 0 to 63 (0 to +31.5db in 0.5db steps). */
} adi_adrv9025_DualBandLnaGtRow_t;

/**
 * \brief Data structure to hold ADRV9025 Rx dualband RxGainTable configurations.
 */
typedef struct adi_adrv9025_DualBandRxGainTableCfg
{
    uint32_t                         rxChannelMask;
    adi_adrv9025_DualBandLnaGtRow_t* dualBandGainTablePtr;
    uint8_t                          numgainIndicesInTable;
} adi_adrv9025_DualBandRxGainTableCfg_t;

/**
* \brief Data structure to hold ADRV9025 Rx gain index config
*/
typedef struct adi_adrv9025_RxGain
{
    uint32_t rxChannelMask; /*!< Rx Channels for which gain index needs to be updated. Bits 0-3, 4-7 correspond to Rx1-Rx4, ORx1-ORx4 respectively */
    uint8_t  gainIndex;     /*!< Gain Index for the channels selected in rxChannelMask */
} adi_adrv9025_RxGain_t;

/**
* \brief Data structure to hold ADRV9025 Rx Agc Mode configurations.
*/
typedef struct adi_adrv9025_RxAgcMode
{
    uint32_t                 rxChannelMask;
    adi_adrv9025_RxAgcMode_e agcMode;
} adi_adrv9025_RxAgcMode_t;

/**
*  \brief Data structure to hold ADRV9025 Floating Point Formatter Configuration
*/
typedef struct adi_adrv9025_FloatingPointConfigSettings
{
    adi_adrv9025_FpFloatDataFormat_e fpDataFormat;      /*!< Floating point format to select between {Sign, Significand, Exponent} and {Sign, Exponent, Significand} */
    adi_adrv9025_FpRoundModes_e    fpRoundMode;         /*!< Rounding mode for floating point format (See enum values) */
    adi_adrv9025_FpExponentModes_e fpNumExpBits;        /*!< Indicates the number of exponent and significand bits in the floating point number */
    adi_adrv9025_FpAttenSteps_e    fpAttenSteps;        /*!< Attenuate integer data when floating point mode enabled, see enum for values from 0dB to 42dB in 6dB steps */
    adi_adrv9025_FpHideLeadingOne_e fpHideLeadingOne;   /*!< 1 =  Hides the leading one in significand to be compatible to the IEEE754 specification. 0 = a leading one exists at the MSB of the significand.  (Valid: 0, 1) */
    adi_adrv9025_FpNanEncode_e fpEncodeNan;             /*!< 1 =  encodes the highest value of Exponent to mean NaN (Not a Number) to be compatible to IEEE754 specification (Valid: 0 or 1) */
} adi_adrv9025_FloatingPointConfigSettings_t;

/**
*  \brief Data structure to hold ADRV9025 Integer Formatter Configuration
*/
typedef struct adi_adrv9025_IntegerConfigSettings
{
    adi_adrv9025_RxSlicerEmbeddedBits_e  intEmbeddedBits;     /*!< No. of embedded slicer bits in Rx Data */
    adi_adrv9025_RxIntSampleResolution_e intSampleResolution; /*!< Resolution of Rx Data  */
    adi_adrv9025_RxIntParity_e           intParity;           /*!< Parity - applicable only for 3bit slicer mode */
    adi_adrv9025_RxSlicerEmbeddedPos_e   intEmbeddedPos;      /*!< Selects the position of the upper and lower embedded slicer control word */
} adi_adrv9025_IntegerConfigSettings_t;

/**
 * \brief Data structure to hold ADRV9025 Slicer Configuration settings
 */
typedef struct adi_adrv9025_SlicerConfigSettings
{
    adi_adrv9025_ExtSlicerStepSizes_e extSlicerStepSize;      /*!< Enum selects the external pin gain step size */
    adi_adrv9025_IntSlicerStepSizes_e intSlicerStepSize;      /*!< Enum selects the internal pin gain step size */
    adi_adrv9025_RxExtSlicerGpioSel_e rx1ExtSlicerGpioSelect; /*!< Enum selects the Rx1 Ext Ctrl GPIO Configuration */
    adi_adrv9025_RxExtSlicerGpioSel_e rx2ExtSlicerGpioSelect; /*!< Enum selects the Rx2 Ext Ctrl GPIO Configuration */
    adi_adrv9025_RxExtSlicerGpioSel_e rx3ExtSlicerGpioSelect; /*!< Enum selects the Rx3 Ext Ctrl GPIO Configuration */
    adi_adrv9025_RxExtSlicerGpioSel_e rx4ExtSlicerGpioSelect; /*!< Enum selects the Rx4 Ext Ctrl GPIO Configuration */
} adi_adrv9025_SlicerConfigSettings_t;

/**
*  \brief Data structure to hold ADRV9025 Rx Embedded Monitor Formatter Configuration
*/
typedef struct adi_adrv9025_EmbOverloadMonitorConfigSettings
{
    adi_adrv9025_RxEmbeddedMonitorSrc_e embeddedMonitorSrcLsbI;              /*!< Selects the monitor data to be embedded into LSB location of 16-bit Rx data - I sample */
    adi_adrv9025_RxEmbeddedMonitorSrc_e embeddedMonitorSrcLsbQ;              /*!< Selects the monitor data to be embedded into LSB location of 16-bit Rx data - Q sample */
    adi_adrv9025_RxEmbeddedMonitorSrc_e embeddedMonitorSrcLsbPlusOneI;       /*!< Selects the monitor data to be embedded into LSB+1 location of 16-bit Rx data - I sample */
    adi_adrv9025_RxEmbeddedMonitorSrc_e embeddedMonitorSrcLsbPlusOneQ;       /*!< Selects the monitor data to be embedded into LSB+1 location of 16-bit Rx data - Q sample */
    adi_adrv9025_RxHb2LowEmbeddedMonitorSrc_e embeddedMonitorHb2LowSrcSel;   /*!< If digital HB2 Low monitor is selected to be embedded into Rx data, the source of digital HB2 low threshold is selected by this param */
    adi_adrv9025_RxHb2HighEmbeddedMonitorSrc_e embeddedMonitorHb2HighSrcSel; /*!< If digital HB2 High monitor is selected to be embedded into Rx data, the source of digital HB2 high threshold is selected by this param */
    adi_adrv9025_RxApdLowEmbeddedMonitorSrc_e embeddedMonitorApdLowSrcSel;   /*!< If analog peak detector low monitor is selected to be embedded into Rx data, the source of analog peak detector low threshold is selected by this param */
    adi_adrv9025_RxApdHighEmbeddedMonitorSrc_e embeddedMonitorApdHighSrcSel; /*!< If analog peak detector high monitor is selected to be embedded into Rx data, the source of analog peak detector low threshold is selected by this param */
    uint8_t invertHb2Flag;                                                   /*!< When inverted, Hb2 emmbedded overload monitor flag operates as active low, 0:Don't invert / Other:Invert*/
    uint8_t invertApdFlag;                                                   /*!< When inverted, Apd emmbedded overload monitor flag operates as active low, 0:Don't invert / Other:Invert*/
} adi_adrv9025_EmbOverloadMonitorConfigSettings_t;

/**
*  \brief Data structure to hold ADRV9025 Floating Point Formatter Configuration
*/
typedef struct adi_adrv9025_RxDataFormat
{
    uint32_t                                   rxChannelMask;               /*!< Rx Channels for which the config settings are to be applied */
    adi_adrv9025_RxDataFormatModes_e           formatSelect;                /*!< Rx Channel format mode selects */
    adi_adrv9025_FloatingPointConfigSettings_t floatingPointConfig;         /*!< Rx Channel floating point format configuration. Ignored if formatSelect is
                                                                             *   NOT set to ADI_ADRV9025_GAIN_WITH_FLOATING_POINT */
    adi_adrv9025_IntegerConfigSettings_t integerConfigSettings;             /*!< Rx Channel integer format configuration. Please note that integerConfigSettings.intSampleResolution needs to be set to a valid value for all modes of formatSelect */
    adi_adrv9025_SlicerConfigSettings_t slicerConfigSettings;               /*!< Rx Channel integer slicer configuration. Ignored if formatSelect is NOT set to ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO, ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER or ADI_ADRV9025_GAIN_WITH_EXTERNAL_SLICER_NOGPIO */
    adi_adrv9025_EmbOverloadMonitorConfigSettings_t embOvldMonitorSettings; /*!< Rx Channel embedded overload monitor settings. Ignored if formatSelect is NOT set to ADI_ADRV9025_EMBED_MONITOR_DATA. Embedded overload monitoring is supported for Rx channels only */
    uint8_t externalLnaGain;                                                /*!< Selects Slicer to compensate for external dualband LNA {0 - disabled, 1 - enabled} */
    uint8_t tempCompensationEnable;                                         /*!< Selects Slicer to compensate for temperature variations {0 - disabled, 1 - enabled} */
} adi_adrv9025_RxDataFormat_t;

/**
 *  \brief Data structure to hold ADRV9025 Rx Gain Control Pin Configuration
 */
typedef struct adi_adrv9025_RxGainPinCfg
{
    //uint32_t rxChannelMask;
    uint8_t incStep;                        /*!< Increment in gain index applied when the increment gain pin is pulsed. A value of 0 to 7 applies a step size of 1 to 8 */
    uint8_t decStep;                        /*!< Decrement in gain index applied when the increment gain pin is pulsed. A value of 0 to 7 applies a step size of 1 to 8 */
    adi_adrv9025_GpioPinSel_e rxGainIncPin; /*!< GPIO used for the Increment gain input: ADI_ADRV9025_GPIO00 - ADI_ADRV9025_GPIO15 can be used */
    adi_adrv9025_GpioPinSel_e rxGainDecPin; /*!< GPIO used for the Decrement gain input: ADI_ADRV9025_GPIO00 - ADI_ADRV9025_GPIO15 can be used */
    uint8_t                   enable;       /*!< Enable (1) or disable (0) the gain pin control*/
} adi_adrv9025_RxGainPinCfg_t;

/**
  *  \brief Data structure to hold ADRV9025 Rx NCO shifter parameters for both BandA and BandB
  *  configurations.  Settings all four element values to zero for a band will disable the NCO
  *  for that shift.
  */
typedef struct adi_adrv9025_RxNcoShifterCfg
{
    uint32_t bandAInputBandWidth_kHz;  /*!< BandWidth in khz of the BandA input signal */
    int32_t  bandAInputCenterFreq_kHz; /*!< Center Frequency in khz of the BandA input signal */
    int32_t  bandANco1Freq_kHz;        /*!< BandA NCO1 Frequency shift in khz */
    int32_t  bandANco2Freq_kHz;        /*!< BandA NCO2 Frequency shift in khz */
    uint32_t bandBInputBandWidth_kHz;  /*!< BandWidth in khz of the BandB input signal */
    int32_t  bandBInputCenterFreq_kHz; /*!< Center Frequency in khz of the BandB input signal */
    int32_t  bandBNco1Freq_kHz;        /*!< BandB NCO1 Frequency shift in khz */
    int32_t  bandBNco2Freq_kHz;        /*!< BandB NCO2 Frequency shift in khz */
    int32_t  bandAbCombinedEnable;     /*!< BandA and BandB Combined at output, 1 = Combine dualband AB, 0 disable combine dualband on AB*/
} adi_adrv9025_RxNcoShifterCfg_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_RX_TYPES_H_ */
