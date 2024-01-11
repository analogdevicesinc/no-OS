/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_rx_types.h
 * \brief Contains ADRV904X API Rx datapath data types
 *
* ADRV904X API Version: 2.10.0.4
 */

#ifndef _ADI_ADRV904X_RX_TYPES_H_
#define _ADI_ADRV904X_RX_TYPES_H_

#include "adi_library_types.h"
#include "adi_adrv904x_gpio_types.h"
#include "adi_adrv904x_rx_nco.h"

#define ADI_ADRV904X_RX_GAIN_TABLE_SIZE_ROWS 256
#define ADI_ADRV904X_RX_MAX_GAIN_INDEX       255

// TODO: This macro will be a duplicate when adi_adrv904x_types.h is eventually included in user.h
//       When adi_adrv904x_types.h is available, remove this duplicate macro define.
#define ADI_ADRV904X_MAX_RX_ONLY    8U
#define ADI_ADRV904X_MAX_ORX_ONLY   2U
#define ADI_ADRV904X_MAX_RX_DDC_BANDS    2
#define ADI_ADRV904X_MAX_RX_INT_SLICER_BITS 4
#define ADI_ADRV904X_MAX_ORX_ATTEN_DB 17U

#define ADI_ADRV904X_MAX_RX_CARRIERS_PER_CHANNEL    8U

/**
 *  \brief Enum of possible Rx channel enables
 */
typedef enum adi_adrv904x_RxChannels
{
    ADI_ADRV904X_RXOFF  = 0x00U,    /*!< No Rx/ORx channels are enabled */
    ADI_ADRV904X_RX0    = 0x01U,    /*!< Rx0 channel enabled */
    ADI_ADRV904X_RX1    = 0x02U,    /*!< Rx1 channel enabled */
    ADI_ADRV904X_RX2    = 0x04U,    /*!< Rx2 channel enabled */
    ADI_ADRV904X_RX3    = 0x08U,    /*!< Rx3 channel enabled */
    ADI_ADRV904X_RX4    = 0x10U,    /*!< Rx4 channel enabled */
    ADI_ADRV904X_RX5    = 0x20U,    /*!< Rx5 channel enabled */
    ADI_ADRV904X_RX6    = 0x40U,    /*!< Rx6 channel enabled */
    ADI_ADRV904X_RX7    = 0x80U,    /*!< Rx7 channel enabled */
    ADI_ADRV904X_ORX0   = 0x100U,   /*!< ORx0 channel enabled */
    ADI_ADRV904X_ORX1   = 0x200U    /*!< ORx1 channel enabled */
} adi_adrv904x_RxChannels_e;

#define ADI_ADRV904X_RX_MASK_ALL    (   ADI_ADRV904X_RX0 | \
                                        ADI_ADRV904X_RX1 | \
                                        ADI_ADRV904X_RX2 | \
                                        ADI_ADRV904X_RX3 | \
                                        ADI_ADRV904X_RX4 | \
                                        ADI_ADRV904X_RX5 | \
                                        ADI_ADRV904X_RX6 | \
                                        ADI_ADRV904X_RX7)

#define ADI_ADRV904X_ORX_MASK_ALL   (ADI_ADRV904X_ORX0 | ADI_ADRV904X_ORX1)

/**
 *  \brief Enum of DDC blocks within an Rx Channel
 */
typedef enum adi_adrv904x_RxDdcs
{
    ADI_ADRV904X_RX_DDC_BAND0 = 0,
    ADI_ADRV904X_RX_DDC_BAND1 = 1
} adi_adrv904x_RxDdcs_e;

/**
 *  \brief Enum of possible Rx gain table SRAM base addresses
 */
typedef enum adi_adrv904x_RxChanGainTableBaseAddr
{
    ADI_ADRV904X_RX0_GAIN_TABLE_BASEADDR = 0x600A0000, /*!< Select gain table SRAM base address for Rx0 Channel */
    ADI_ADRV904X_RX1_GAIN_TABLE_BASEADDR = 0x601A0000, /*!< Select gain table SRAM base address for Rx1 Channel */
    ADI_ADRV904X_RX2_GAIN_TABLE_BASEADDR = 0x602A0000, /*!< Select gain table SRAM base address for Rx2 Channel */
    ADI_ADRV904X_RX3_GAIN_TABLE_BASEADDR = 0x603A0000, /*!< Select gain table SRAM base address for Rx3 Channel */
    ADI_ADRV904X_RX4_GAIN_TABLE_BASEADDR = 0x604A0000, /*!< Select gain table SRAM base address for Rx4 Channel */
    ADI_ADRV904X_RX5_GAIN_TABLE_BASEADDR = 0x605A0000, /*!< Select gain table SRAM base address for Rx5 Channel */
    ADI_ADRV904X_RX6_GAIN_TABLE_BASEADDR = 0x606A0000, /*!< Select gain table SRAM base address for Rx6 Channel */
    ADI_ADRV904X_RX7_GAIN_TABLE_BASEADDR = 0x607A0000 /*!< Select gain table SRAM base address for Rx7 Channel */
} adi_adrv904x_RxChanGainTableBaseAddr_e;

/**
 *  \brief Enum to select ADRV904X Rx Data Formatting Options
 */
typedef enum adi_adrv904x_RxDataFormatModes
{
    ADI_ADRV904X_GAIN_COMPENSATION_DISABLED           = 0, /*!< Gain Compensation and Data Formatting are disabled */
    ADI_ADRV904X_GAIN_WITH_FLOATING_POINT,                 /*!< Gain Compensation enabled with floating point data formatting enabled and internal slicer enabled */
    ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_NOGPIO,         /*!< Gain Compensation enabled with integer data formatting and internal slicer enabled with no GPIO Slicer Position output. The slicer position is embedded in the JESD data */
    ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_2PIN_MODE,      /*!< Gain Compensation enabled with integer data formatting and internal slicer enabled with GPIO Slicer Position output. The least significant 2 bits of the slicer control word are output on the GPIO */
    ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE,      /*!< Gain Compensation enabled with integer data formatting and internal slicer enabled with GPIO Slicer Position output. The least significant 3 bits of the slicer control word are output on the GPIO */
    ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE,      /*!< Gain Compensation enabled with integer data formatting and internal slicer enabled with GPIO Slicer Position output. All 4 bits of the slicer control word are output on the GPIOs */
    ADI_ADRV904X_GAIN_WITH_EXTERNAL_SLICER,                /*!< NOT SUPPORTED: Gain Compensation enabled with integer data formatting and external slicer enabled */
    ADI_ADRV904X_EMBED_OVERLOAD_MONITOR_DATA               /*!< Embed 1-bit/2 bit monitor data on the 2 least significant bits of a 16-bit 2's Complement JESD Rx data */
} adi_adrv904x_RxDataFormatModes_e;

/**
 *  \brief Enum to select ADRV904X Rx Data Formatting Options
 */
typedef enum adi_adrv904x_CddcDataFormatModes
{
    ADI_ADRV904X_CDDC_DISABLED                   = 0, /*!< CDDC Data formatting monitoring disabled */
    ADI_ADRV904X_CDDC_FLOATING_POINT,                 /*!< CDDC Data formatting with floating point data formatting enabled */
    ADI_ADRV904X_CDDC_INTEGER,                        /*!< CDDC Data formatting with integer data formatting enabled */
    ADI_ADRV904X_CDDC_SLICER,                         /*!< CDDC Data formatting slicer configuration */
    ADI_ADRV904X_CDDC_EMBED_OVERLOAD_MONITOR_DATA     /*!< Embed 1-bit/2 bit monitor data on the 2 least significant bits of a 16-bit 2's Complement data */
} adi_adrv904x_CddcDataFormatModes_e;

/**
 *  \brief Enum to hold ADRV904X Floating Point Formatter format which indicates order of sign significand and exponent
 */
typedef enum adi_adrv904x_FpFloatDataFormat
{
    ADI_ADRV904X_FP_FORMAT_SIGN_EXP_SIGNIFICAND = 0, /*!< Floating point format of type {Sign, Exponent, Significand} */
    ADI_ADRV904X_FP_FORMAT_SIGN_SIGNIFICAND_EXP = 1  /*!< Floating point format of type {Sign, Significand, Exponent} */
} adi_adrv904x_FpFloatDataFormat_e;

/**
 *  \brief Enum to hold ADRV904X Floating Point Formatter rounding modes for the Rx data path
 */
typedef enum adi_adrv904x_FpRoundModes
{
    ADI_ADRV904X_ROUND_TO_EVEN         = 0, /*!< Round floating point ties to an even value          */
    ADI_ADRV904X_ROUNDTOWARDS_POSITIVE = 1, /*!< Round floating point toward the positive direction  */
    ADI_ADRV904X_ROUNDTOWARDS_NEGATIVE = 2, /*!< Round floating point toward the negative direction  */
    ADI_ADRV904X_ROUNDTOWARDS_ZERO     = 3, /*!< Round floating point toward the zero direction      */
    ADI_ADRV904X_ROUND_FROM_EVEN       = 4  /*!< Round floating point ties away from even value      */
} adi_adrv904x_FpRoundModes_e;

/**
 *  \brief Enum to hold ADRV904X Floating Point Formatter number of exponent bits
 */
typedef enum adi_adrv904x_FpExponentModes
{
    ADI_ADRV904X_2_EXPONENTBITS = 0, /*!< Floating point values have 2 exponent bits, 13 significand bits, 1 sign bit */
    ADI_ADRV904X_3_EXPONENTBITS = 1, /*!< Floating point values have 3 exponent bits, 12 significand bits, 1 sign bit */
    ADI_ADRV904X_4_EXPONENTBITS = 2, /*!< Floating point values have 4 exponent bits, 11 significand bits, 1 sign bit */
    ADI_ADRV904X_5_EXPONENTBITS = 3  /*!< Floating point values have 5 exponent bits, 10 significand bits, 1 sign bit  */
} adi_adrv904x_FpExponentModes_e;

/**
 *  \brief Enum to hold ADRV904X Rx attenuation values that are used when the
 *         floating point format is enabled
 */
typedef enum adi_adrv904x_FpAttenSteps
{
    ADI_ADRV904X_FPATTEN_24DB      = 4, /*!< Set Rx attenuation to 24 dB when Rx Data format set to floating point mode  */
    ADI_ADRV904X_FPATTEN_18DB      = 5, /*!< Set Rx attenuation to 18 dB when Rx Data format set to floating point mode  */
    ADI_ADRV904X_FPATTEN_12DB      = 6, /*!< Set Rx attenuation to 12 dB when Rx Data format set to floating point mode */
    ADI_ADRV904X_FPATTEN_6DB       = 7, /*!< Set Rx attenuation to 6 dB when Rx Data format set to floating point mode */
    ADI_ADRV904X_FPATTEN_0DB       = 0, /*!< Set Rx attenuation to 0 dB when Rx Data format set to floating point mode */
    ADI_ADRV904X_FPATTEN_MINUS6DB  = 1, /*!< Set Rx attenuation to -6 dB when Rx Data format set to floating point mode */
    ADI_ADRV904X_FPATTEN_MINUS12DB = 2, /*!< Set Rx attenuation to -12 dB when Rx Data format set to floating point mode */
    ADI_ADRV904X_FPATTEN_MINUS18DB = 3  /*!< Set Rx attenuation to -18 dB when Rx Data format set to floating point mode */
} adi_adrv904x_FpAttenSteps_e;

/**
 *  \brief Enum to hold ADRV904X Hide Leading One selection
 */
typedef enum adi_adrv904x_FpHideLeadingOne
{
    ADI_ADRV904X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE = 0, /*!< Sets leading one to be at the MSB of the significand */
    ADI_ADRV904X_FP_FORMAT_HIDE_LEADING_ONE_ENABLE  = 1  /*!< Hides the leading one in significand to be compatible to the IEEE754 specification */
} adi_adrv904x_FpHideLeadingOne_e;

/**
 *  \brief Enum to hold ADRV904X Floating Point Formatter NaN encode enable
 */
typedef enum adi_adrv904x_FpNanEncode
{
    ADI_ADRV904X_FP_FORMAT_NAN_ENCODE_DISABLE = 0, /*!< Does not reserve highest value of exponent for NaN */
    ADI_ADRV904X_FP_FORMAT_NAN_ENCODE_ENABLE  = 1  /*!< Reserve highest value of exponent for NaN */
} adi_adrv904x_FpNanEncode_e;


/**
 *  \brief Enum to select the overload peak detection input monitor signal source
 */
typedef enum adi_adrv904x_SignalMonitorSrc
{

	ADI_ADRV904X_SIG_MON_SRC_HB2_OUTPUT,   /*!< Use Hb2 block output as input signal */
	ADI_ADRV904X_SIG_MON_SRC_HB2_INPUT,    /*!< Use Hb2 block input as input signal */
	ADI_ADRV904X_MAX_SIG_MON_SRC_INVALID   /*!< Max number of signal monitor input source */
} adi_adrv904x_SignalMonitorSrc_e;


/**
 *  \brief Enum to hold ADRV904X integer mode number of embedded slicer bits and positions
 */
typedef enum adi_adrv904x_RxSlicerEmbeddedBits
{
    ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS                 = 0, /*!< Disabled all embedded slicer bits  */
    ADI_ADRV904X_EMBED_1_SLICERBIT_AT_MSB,                    /*!< Embeds 1 slicer bits on I and 1 slicer bits on Q and the MSB position in the data frame */
    ADI_ADRV904X_EMBED_1_SLICERBIT_AT_LSB,                    /*!< Embeds 1 slicer bits on I and 1 slicer bits on Q and the LSB position in the data frame */
    ADI_ADRV904X_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER,      /*!< Embeds 2 slicer bits on I and 2 slicer bits on Q and the MSB position in the data frame whereas only 3 bits are used for slicer ctrl (2 bits on I, 1 bit Q) */
    ADI_ADRV904X_EMBED_2_SLICERBITS_AT_LSB_3_BIT_SLICER,      /*!< Embeds 2 slicer bits on I and 2 slicer bits on Q and the LSB position in the data frame whereas only 3 bits are used for slicer ctrl (2 bits on I, 1 bit Q) */
    ADI_ADRV904X_EMBED_2_SLICERBITS_AT_MSB_4_BIT_SLICER,      /*!< Embeds 2 slicer bits on I and 2 slicer bits on Q and the MSB position in the data frame and all 4 bits are used for slicer ctrl*/
    ADI_ADRV904X_EMBED_2_SLICERBITS_AT_LSB_4_BIT_SLICER       /*!< Embeds 2 slicer bits on I and 2 slicer bits on Q and the LSB position in the data frame and all 4 bits are used for slicer ctrl*/
} adi_adrv904x_RxSlicerEmbeddedBits_e;

/**
 *  \brief Enum to hold ADRV904X integer sample resolution
 */
typedef enum adi_adrv904x_RxIntSampleResolution
{
    ADI_ADRV904X_INTEGER_12BIT_2SCOMP = 0, /*!< Selects integer sample 12 bit resolution with 2s compliment    */
    ADI_ADRV904X_INTEGER_12BIT_SIGNED,     /*!< Selects integer sample 12 bit resolution with signed magnitude */
    ADI_ADRV904X_INTEGER_16BIT_2SCOMP,     /*!< Selects integer sample 16 bit resolution with 2s compliment    */
    ADI_ADRV904X_INTEGER_16BIT_SIGNED      /*!< Selects integer sample 16 bit resolution with signed magnitude */
} adi_adrv904x_RxIntSampleResolution_e;

/**
 *  \brief Enum to hold ADRV904X integer mode parity selection for 3 bit slicer mode
 */
typedef enum adi_adrv904x_RxIntParity
{
    ADI_ADRV904X_3BIT_SLICER_EVEN_PARITY = 0, /*!< Even parity for the unused 4th bit in 3 bit slicer mode */
    ADI_ADRV904X_3BIT_SLICER_ODD_PARITY  = 1, /*!< Odd parity for the unused 4th bit in 3 bit slicer mode */
    ADI_ADRV904X_NO_PARITY               = 2  /*!< Disable parity */
} adi_adrv904x_RxIntParity_e;

/**
 *  \brief Enum to hold ADRV904X slicer control word nibble position
 */
typedef enum adi_adrv904x_RxSlicerEmbeddedPos
{
    ADI_ADRV904X_LOWER_NIBBLE_ON_I = 0,  /*!< Place the lower control nibble on I and the upper control nibble on Q */
    ADI_ADRV904X_LOWER_NIBBLE_ON_Q       /*!< Place the lower control nibble on Q and the upper control nibble on I */
        
} adi_adrv904x_RxSlicerEmbeddedPos_e;

/**
 *  \brief Enum to hold ADRV904X Gain Slicer external pin gain step size.  Slicer gain in this mode is determined by multiplying the step size by the step size selection from the base band processor.
 */
typedef enum adi_adrv904x_ExtSlicerStepSizes
{
    ADI_ADRV904X_EXTSLICER_STEPSIZE_1DB = 0, /*!< Set Gain Slicer External gain step size to 1dB  */
    ADI_ADRV904X_EXTSLICER_STEPSIZE_2DB,     /*!< Set Gain Slicer External gain step size to 2dB  */
    ADI_ADRV904X_EXTSLICER_STEPSIZE_3DB,     /*!< Set Gain Slicer External gain step size to 3dB  */
    ADI_ADRV904X_EXTSLICER_STEPSIZE_4DB,     /*!< Set Gain Slicer External gain step size to 4dB  */
    ADI_ADRV904X_EXTSLICER_STEPSIZE_6DB      /*!< Set Gain Slicer External gain step size to 6dB  */
} adi_adrv904x_ExtSlicerStepSizes_e;
    
/**
 *  \brief Enum to hold ADRV904X Internal Gain Slicer gain step size.  Slicer gain in this mode is determined by multiplying the step size by the step size selection from the base band processor.
 */
typedef enum adi_adrv904x_IntSlicerStepSizes
{
    ADI_ADRV904X_INTSLICER_STEPSIZE_1DB = 0, /*!< Set Gain Slicer Internal gain step size to 1dB  */
    ADI_ADRV904X_INTSLICER_STEPSIZE_2DB,     /*!< Set Gain Slicer Internal gain step size to 2dB  */
    ADI_ADRV904X_INTSLICER_STEPSIZE_3DB,     /*!< Set Gain Slicer Internal gain step size to 3dB  */
    ADI_ADRV904X_INTSLICER_STEPSIZE_4DB,     /*!< Set Gain Slicer Internal gain step size to 4dB  */
    ADI_ADRV904X_INTSLICER_STEPSIZE_6DB,     /*!< Set Gain Slicer Internal gain step size to 6dB  */
    ADI_ADRV904X_INTSLICER_STEPSIZE_8DB      /*!< Set Gain Slicer Internal gain step size to 8dB  */
} adi_adrv904x_IntSlicerStepSizes_e;

/**
 *  \brief Enum to hold ADRV904X Rx Gain Slicer external pin GPIO selection options
 */
typedef enum adi_adrv904x_RxExtSlicerGpioSel
{
    ADI_ADRV904X_EXTSLICER_RX_GPIO_DISABLE      = 0, /*!< No GPIO assigned to external slicer */
    ADI_ADRV904X_EXTSLICER_RX_GPIO_INVALID      = 9
} adi_adrv904x_RxExtSlicerGpioSel_e;

/**
*  \brief Enum to hold ADRV904X Embedded Monitor Mode sources
*/
typedef enum adi_adrv904x_RxEmbeddedMonitorSrc
{
    ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED = 0,  /*!< Embedded overload monitor data is not embedded for the selected bit position in 16 bit Rx data */
    ADI_ADRV904X_RX_EMB_MON_SRC_HB2_LOW_THRESH       = 3,  /*!< Selects digital HB2 low threshold monitor data to be embedded in Rx data */
    ADI_ADRV904X_RX_EMB_MON_SRC_HB2_HIGH_THRESH      = 4,  /*!< Selects digital HB2 high threshold monitor data to be embedded in Rx data */
    ADI_ADRV904X_RX_EMB_MON_SRC_APD_LOW_THRESH       = 5,  /*!< Selects analog peak detector low threshold monitor data to be embedded in Rx data */
    ADI_ADRV904X_RX_EMB_MON_SRC_APD_HIGH_THRESH      = 6,  /*!< Selects analog peak detector high threshold monitor data to be embedded in Rx data */
    ADI_ADRV904X_RX_EMB_MON_SRC_BAND0_LOW_THRESH     = 7,  /*!< Selects band0 low threshold monitor data to be embedded in Rx data */
    ADI_ADRV904X_RX_EMB_MON_SRC_BAND0_HIGH_THRESH    = 8,  /*!< Selects band0 high threshold monitor data to be embedded in Rx data */
    ADI_ADRV904X_RX_EMB_MON_SRC_BAND1_LOW_THRESH     = 9,  /*!< Selects band1 low threshold monitor data to be embedded in Rx data */
    ADI_ADRV904X_RX_EMB_MON_SRC_BAND1_HIGH_THRESH    = 10, /*!< Selects band1 high threshold monitor data to be embedded in Rx data */
    ADI_ADRV904X_RX_EMB_MON_SRC_INVALID              = 11  /*!< Invalid embedded monitoring source */
} adi_adrv904x_RxEmbeddedMonitorSrc_e;

/**
*  \brief Enum to hold ADRV904X Embedded Monitor Mode HB2 Low source
*/
typedef enum adi_adrv904x_RxHb2LowEmbeddedMonitorSrc
{
    ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW                           = 0, /*!< Selects digital HB2 lower threshold exceeded flag as HB2 low threshold monitor data */
    ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW_INT0                      = 1, /*!< Selects digital HB2 interval 0 lower threshold exceeded flag as HB2 low threshold monitor data */
    ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW_INT1                      = 2, /*!< Selects digital HB2 interval 1 lower threshold exceeded flag as HB2 low threshold monitor data */
    ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW_THRESH_CNTR_EXCEEDED      = 4, /*!< Selects digital HB2 lower threshold exceeded counter flag as HB2 low threshold monitor data */
    ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW_INT0_THRESH_CNTR_EXCEEDED = 5, /*!< Selects digital HB2 interval 0 lower threshold exceeded counter flag as HB2 low threshold monitor data */
    ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW_INT1_THRESH_CNTR_EXCEEDED = 6, /*!< Selects digital HB2 interval 1 lower threshold exceeded counter flag as HB2 low threshold monitor data */
    ADI_ADRV904X_HB2_LOW_SRC_INVALID                            = 7  /*!< Invalid HB2 low source */
} adi_adrv904x_RxHb2LowEmbeddedMonitorSrc_e;

/**
*  \brief Enum to hold ADRV904X Embedded Monitor Mode HB2 High source
*/
typedef enum adi_adrv904x_RxHb2HighEmbeddedMonitorSrc
{
    ADI_ADRV904X_HB2_HIGH_SRC_OVRG_HIGH               = 0, /*!< Selects digital HB2 overrange high threshold exceeded flag as HB2 high threshold monitor data */
    ADI_ADRV904X_HB2_HIGH_SRC_OVRG_HIGH_CNTR_EXCEEDED,     /*!< Selects digital HB2 overrange high threshold counter exceeded flag as HB2 high threshold monitor data */
    ADI_ADRV904X_HB2_HIGH_SRC_INVALID                      /*!< Invalid HB2 high source */
} adi_adrv904x_RxHb2HighEmbeddedMonitorSrc_e;

/**
*  \brief Enum to hold ADRV904X Embedded Monitor Mode Analog Peak Detector Low source
*/
typedef enum adi_adrv904x_RxApdLowEmbeddedMonitorSrc
{
    ADI_ADRV904X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED       = 0, /*!< Selects analog peak detector lower level blocker exceeded flag as APD low threshold monitor data */
    ADI_ADRV904X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_CNTR_EXCEEDED,      /*!< Selects analog peak detector lower level blocker counter exceeded flag as APD low threshold monitor data */
    ADI_ADRV904X_APD_LOW_SRC_INVALID                                 /*!< Invalid APD low source */
} adi_adrv904x_RxApdLowEmbeddedMonitorSrc_e;

/**
*  \brief Enum to hold ADRV904X Embedded Monitor Mode Analog Peak Detector High source
*/
typedef enum adi_adrv904x_RxApdHighEmbeddedMonitorSrc
{
    ADI_ADRV904X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED       = 0,  /*!< Selects analog peak detector upper level blocker exceeded flag as APD high threshold monitor data */
    ADI_ADRV904X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_CNTR_EXCEEDED,       /*!< Selects analog peak detector upper level blocker counter exceeded flag as APD high threshold monitor data */
    ADI_ADRV904X_APD_HIGH_SRC_INVALID                                  /*!< Invalid APD high source */
} adi_adrv904x_RxApdHighEmbeddedMonitorSrc_e;



/**
  * \brief Data structure to hold ADRV904X Rx gain table row entry
  */
typedef struct adi_adrv904x_RxGainTableRow
{
    uint8_t rxFeGain;     /*!< Rx Front End gain for a given gain index */
    uint8_t extControl;   /*!< External LNA control word */
    int16_t digGain;      /*!< Digital gain ranging from -18dB to 50dB (68dB total range) */
    uint16_t phaseOffset; /*!< 16 bit phase offset from 0 - 2pi in resolution of 0.005 degrees */
} adi_adrv904x_RxGainTableRow_t;


/**
* \brief Data structure to hold ADRV904X Rx gain index config
*/
typedef struct adi_adrv904x_RxGain
{
    uint32_t rxChannelMask; /*!< Rx Channels for which gain index needs to be updated. Bits 0-7 correspond to Rx0-Rx7 respectively */
    uint8_t gainIndex;      /*!< Gain Index for the channels selected in rxChannelMask */
} adi_adrv904x_RxGain_t;


/**
*  \brief Data structure to hold ADRV904X Floating Point Formatter Configuration
*/
typedef struct adi_adrv904x_FloatingPointConfigSettings
{
    adi_adrv904x_FpFloatDataFormat_e fpDataFormat;     /*!< Floating point format to select between {Sign, Significand, Exponent} and {Sign, Exponent, Significand} */
    adi_adrv904x_FpRoundModes_e      fpRoundMode;      /*!< Rounding mode for floating point format (See enum values) */
    adi_adrv904x_FpExponentModes_e   fpNumExpBits;     /*!< Indicates the number of exponent and significand bits in the floating point number */
    adi_adrv904x_FpAttenSteps_e      fpAttenSteps;     /*!< Attenuate integer data when floating point mode enabled, see enum for values from 0dB to 42dB in 6dB steps */
    adi_adrv904x_FpHideLeadingOne_e  fpHideLeadingOne; /*!< 1 =  Hides the leading one in significand to be compatible to the IEEE754 specification. 0 = a leading one exists at the MSB of the significand.  (Valid: 0, 1) */
    adi_adrv904x_FpNanEncode_e       fpEncodeNan;      /*!< 1 =  encodes the highest value of Exponent to mean NaN (Not a Number) to be compatible to IEEE754 specification (Valid: 0 or 1) */
} adi_adrv904x_FloatingPointConfigSettings_t;

/**
*  \brief Data structure to hold ADRV904X Floating Point Formatter Configuration
*/
typedef struct adi_adrv904x_CddcFloatingPointConfigSettings
{
    adi_adrv904x_FpFloatDataFormat_e cddcFpDataFormat;     /*!< Floating point format to select between {Sign, Significand, Exponent} and {Sign, Exponent, Significand} */
    adi_adrv904x_FpRoundModes_e      cddcFpRoundMode;      /*!< Rounding mode for floating point format (See enum values) */
    adi_adrv904x_FpExponentModes_e   cddcFpNumExpBits;     /*!< Indicates the number of exponent and significand bits in the floating point number */
    adi_adrv904x_FpHideLeadingOne_e  cddcFpHideLeadingOne; /*!< 1 =  Hides the leading one in significand to be compatible to the IEEE754 specification. 0 = a leading one exists at the MSB of the significand.  (Valid: 0, 1) */
    adi_adrv904x_FpNanEncode_e       cddcFpEncodeNan;      /*!< 1 =  encodes the highest value of Exponent to mean NaN (Not a Number) to be compatible to IEEE754 specification (Valid: 0 or 1) */
    uint8_t                          cddcFpEn;             /*!< 1 = Floating point CDDC data formatting enabled (Valid: 0 or 1) */
} adi_adrv904x_CddcFloatingPointConfigSettings_t;

/**
*  \brief Data structure to hold ADRV904X Integer Formatter Configuration
*/
typedef struct adi_adrv904x_IntegerConfigSettings
{
    adi_adrv904x_RxSlicerEmbeddedBits_e  intEmbeddedBits;     /*!< No. of embedded slicer bits in Rx Data */
    adi_adrv904x_RxIntSampleResolution_e intSampleResolution; /*!< Resolution of Rx Data  */
    adi_adrv904x_RxIntParity_e           intParity;           /*!< Parity - applicable only for 3bit slicer mode */
    adi_adrv904x_RxSlicerEmbeddedPos_e   intEmbeddedPos;      /*!< Selects the position of the upper and lower embedded slicer control word */
} adi_adrv904x_IntegerConfigSettings_t;

/**
*  \brief Data structure to hold ADRV904X Integer Formatter Configuration
*/
typedef struct adi_adrv904x_CddcIntegerConfigSettings
{
    adi_adrv904x_RxSlicerEmbeddedBits_e  intEmbeddedBits;     /*!< No. of embedded slicer bits in Rx Data */
    adi_adrv904x_RxIntSampleResolution_e intSampleResolution; /*!< Resolution of Rx Data  */
    adi_adrv904x_RxIntParity_e           intParity;           /*!< Parity - applicable only for 3bit slicer mode */
    adi_adrv904x_RxSlicerEmbeddedPos_e   intEmbeddedPos;      /*!< Selects the position of the upper and lower embedded slicer control word */
} adi_adrv904x_CddcIntegerConfigSettings_t;

/**
 * \brief Data structure to hold ADRV904X Slicer Configuration settings
 */
typedef struct adi_adrv904x_SlicerConfigSettings
{
    adi_adrv904x_ExtSlicerStepSizes_e extSlicerStepSize; /*!< Selects the external pin gain step size */
    adi_adrv904x_IntSlicerStepSizes_e intSlicerStepSize; /*!< Selects the internal pin gain step size */
    adi_adrv904x_RxExtSlicerGpioSel_e extSlicerGpioSelect; /*!< Selects the Ext Ctrl GPIO Configuration for the respective Rx Channel */
    adi_adrv904x_GpioPinSel_e         intSlicerGpioSelect[ADI_ADRV904X_MAX_RX_INT_SLICER_BITS]; /*!< Selects the Int Output GPIO Configuration for the respective Rx Channel */
} adi_adrv904x_SlicerConfigSettings_t;

/**
 * \brief Data structure to hold ADRV904X Slicer Configuration settings
 */
typedef struct adi_adrv904x_CddcSlicerConfigSettings
{
    uint8_t                           cddcSlicerCarrierSelect;      /*!< Selects the Slicer Carrier for CDDC */
    uint8_t                           cddcFormatterCarrierSelect;   /*!< Selects the Formatter Carrier for CDDC */
    uint8_t                           cddcEn3BitSlicerMode;         /*!< Enable 3bit slicer mode for CDDC */
    uint8_t                           cddcEnSlicerAgcSync;          /*!< Enable AGC Sync for CDDC Slicer */
} adi_adrv904x_CddcSlicerConfigSettings_t;

/**
*  \brief Data structure to hold ADRV904X Rx Embedded Monitor Formatter Configuration
*/
typedef struct adi_adrv904x_EmbOverloadMonitorConfigSettings
{
    adi_adrv904x_RxEmbeddedMonitorSrc_e        embeddedMonitorSrcLsbI;        /*!< Selects the monitor data to be embedded into LSB location of 16-bit Rx data - I sample */
    adi_adrv904x_RxEmbeddedMonitorSrc_e        embeddedMonitorSrcLsbQ;        /*!< Selects the monitor data to be embedded into LSB location of 16-bit Rx data - Q sample */
    adi_adrv904x_RxEmbeddedMonitorSrc_e        embeddedMonitorSrcLsbPlusOneI; /*!< Selects the monitor data to be embedded into LSB+1 location of 16-bit Rx data - I sample */
    adi_adrv904x_RxEmbeddedMonitorSrc_e        embeddedMonitorSrcLsbPlusOneQ; /*!< Selects the monitor data to be embedded into LSB+1 location of 16-bit Rx data - Q sample */
    adi_adrv904x_RxHb2LowEmbeddedMonitorSrc_e  embeddedMonitorHb2LowSrcSel;   /*!< If digital HB2 Low monitor is selected to be embedded into Rx data, the source of digital HB2 low threshold is selected by this param */
    adi_adrv904x_RxHb2HighEmbeddedMonitorSrc_e embeddedMonitorHb2HighSrcSel;  /*!< If digital HB2 High monitor is selected to be embedded into Rx data, the source of digital HB2 high threshold is selected by this param */
    adi_adrv904x_RxApdLowEmbeddedMonitorSrc_e  embeddedMonitorApdLowSrcSel;   /*!< If analog peak detector low monitor is selected to be embedded into Rx data, the source of analog peak detector low threshold is selected by this param */
    adi_adrv904x_RxApdHighEmbeddedMonitorSrc_e embeddedMonitorApdHighSrcSel;  /*!< If analog peak detector high monitor is selected to be embedded into Rx data, the source of analog peak detector low threshold is selected by this param */
    uint8_t                                    invertHb2Flag;                 /*!< When inverted, Hb2 embedded overload monitor flag operates as active low, 0:Don't invert / Other:Invert*/
    uint8_t                                    invertApdFlag;                 /*!< When inverted, Apd embedded overload monitor flag operates as active low, 0:Don't invert / Other:Invert*/
} adi_adrv904x_EmbOverloadMonitorConfigSettings_t;

/**
*  \brief Data structure to hold ADRV904X Rx Embedded Monitor Formatter Configuration
*/
typedef struct adi_adrv904x_CddcEmbOverloadMonitorConfigSettings
{
    uint8_t        embeddedMonitorSrcLsbI;        /*!< Selects the monitor data to be embedded into LSB location of 16-bit Rx data - I sample */
    uint8_t        embeddedMonitorSrcLsbQ;        /*!< Selects the monitor data to be embedded into LSB location of 16-bit Rx data - Q sample */
    uint8_t        embeddedMonitorSrcLsbPlusOneI; /*!< Selects the monitor data to be embedded into LSB+1 location of 16-bit Rx data - I sample */
    uint8_t        embeddedMonitorSrcLsbPlusOneQ; /*!< Selects the monitor data to be embedded into LSB+1 location of 16-bit Rx data - Q sample */
} adi_adrv904x_CddcEmbOverloadMonitorConfigSettings_t;

/**
*  \brief Configuration structure for Rx Data Formatter
*/
typedef struct adi_adrv904x_RxDataFormat
{
    adi_adrv904x_RxDataFormatModes_e                formatSelect;           /*!< Rx Channel format mode selects */
    adi_adrv904x_FloatingPointConfigSettings_t      floatingPointConfig;    /*!< Rx Channel floating point format configuration. Ignored if formatSelect is NOT set to ADI_ADRV904X_GAIN_WITH_FLOATING_POINT */
    adi_adrv904x_IntegerConfigSettings_t            integerConfigSettings;  /*!< Rx Channel integer format configuration. Please note that integerConfigSettings.intSampleResolution needs to be set to a valid value for all modes of formatSelect */
    adi_adrv904x_SlicerConfigSettings_t             slicerConfigSettings;   /*!< Rx channel integer slicer configuration. Ignored if formatSelect is NOT ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_NOGPIO, ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_2/3/4PIN_MODE or ADI_ADRV904X_GAIN_WITH_EXTERNAL_SLICER_NO_GPIO */
    adi_adrv904x_EmbOverloadMonitorConfigSettings_t embOvldMonitorSettings; /*!< Rx channel embedded overload monitor settings. Ignored if formatSelect is NOT set to ADI_ADRV904X_EMBED_MONITOR_DATA. Embedded overload monitoring is supported for Rx channels only */
    uint8_t                                         externalLnaGain;        /*!< Selects Slicer to compensate for external dualband LNA {0 - disabled, 1 - enabled} */
    uint8_t                                         tempCompensationEnable; /*!< Selects Slicer to compensate for temperature variations {0 - disabled, 1 - enabled} */
} adi_adrv904x_RxDataFormat_t;

/**
*  \brief Configuration structure for CDDC Data Formatter
*/
typedef struct adi_adrv904x_CddcDataFormat
{
    adi_adrv904x_CddcDataFormatModes_e                  cddcFormatSelect;           /*!< Rx Channel format mode selects */
    adi_adrv904x_CddcFloatingPointConfigSettings_t      cddcFloatingPointConfig;    /*!< Rx Channel floating point format configuration. Ignored if formatSelect is NOT set to ADI_ADRV904X_GAIN_WITH_FLOATING_POINT */
    adi_adrv904x_CddcIntegerConfigSettings_t            cddcIntegerConfigSettings;  /*!< Rx Channel integer format configuration. Please note that integerConfigSettings.intSampleResolution needs to be set to a valid value for all modes of formatSelect */
    adi_adrv904x_CddcSlicerConfigSettings_t             cddcSlicerConfigSettings;   /*!< Rx channel slicer configuration.  */
    adi_adrv904x_CddcEmbOverloadMonitorConfigSettings_t cddcEmbOvldMonitorSettings; /*!< Rx channel embedded overload monitor settings. Ignored if formatSelect is NOT set to ADI_ADRV904X_EMBED_MONITOR_DATA. Embedded overload monitoring is supported for Rx channels only */
} adi_adrv904x_CddcDataFormat_t;

/**
*  \brief Indicates the rx channels to which an instance of adi_adrv904x_RxDataFormat_t applies
*/
typedef struct adi_adrv904x_RxDataFormatRt
{
    uint32_t                                        rxChannelMask;  /*!< The rx channels to which rxDataFormat applies. Suggested to use adi_adrv904x_RxChannels_e to build.  */
    adi_adrv904x_RxDataFormat_t                     rxDataFormat;   /*!< RxDataFormat configuration used for channels specified in rxChannelMask */
} adi_adrv904x_RxDataFormatRt_t;


/**
*  \brief Indicates the rx channels to which an instance of adi_adrv904x_CddcDataFormat_t applies
*/
typedef struct adi_adrv904x_CddcDataFormatRt
{
    uint32_t                                        rxChannelMask;  /*!< The rx channels to which rxDataFormat applies. Suggested to use adi_adrv904x_RxChannels_e to build.  */
    adi_adrv904x_CddcDataFormat_t                   cddcDataFormat; /*!< RxDataFormat configuration used for channels specified in rxChannelMask */
} adi_adrv904x_CddcDataFormatRt_t;

/**
*  \brief Enum to hold ADRV904X Decimated power block selection
*/
typedef enum adi_adrv904x_DecPowerMeasurementBlock
{
    ADI_ADRV904X_DEC_POWER_MAIN_PATH_MEAS_BLOCK  = 0x1, /*!< Selects main path decimated power measurement block */
    ADI_ADRV904X_DEC_POWER_BAND_A_MEAS_BLOCK     = 0x2, /*!< Selects Band A decimated power measurement block */
    ADI_ADRV904X_DEC_POWER_BAND_B_MEAS_BLOCK     = 0x4, /*!< Selects Band B decimated power measurement block */
} adi_adrv904x_DecPowerMeasurementBlock_e;

/**
*  \brief Enum to select Rx Decimated power measurement control type
*/
typedef enum adi_adrv904x_DecPowerControl
{
    ADI_ADRV904X_DEC_POWER_MEAS_OFF = 0, /*!< Disable decimated power measurement for selected block */
    ADI_ADRV904X_DEC_POWER_MEAS_ON  = 1, /*!< Enable decimated power measurement for selected block. If user enables DEC power measurement with this option
                                              AGC block won't use decimated power measurements*/
    ADI_ADRV904X_DEC_POWER_AGC_MEAS = 2  /*!< Leave decimated power measurement block control to AGC. */
} adi_adrv904x_DecPowerControl_e;

/**
*  \brief Data structure for Rx decimated power block configuration
*/
typedef struct adi_adrv904x_RxDecimatedPowerCfg
{
    uint32_t    rxChannelMask;                       /*!< Rx channel selection */
    adi_adrv904x_DecPowerControl_e decPowerControl;  /*!< Decimated power measurements control mode */
    uint8_t     powerInputSelect;                    /*!< Use output of Rx for power measurement. 0: DC Offset output, 1: RFIR output, 2: QFIR output, 3:HB2 output. Valid Range from 0 to 3, only effective for main path Dec power*/
    uint8_t     measBlockSelectMask;                 /*!< Select Main path dec power, BandA dec power and/or BandB dec power. Please use adi_adrv904x_DecPowerMeasurementBlock_e to create this mask */
    uint8_t     powerMeasurementDuration;            /*!< Average power measurement duration = 8*2^powerMeasurementDuration. Valid range from 0 to 31 */
}adi_adrv904x_RxDecimatedPowerCfg_t;

/**
*  \brief Data structure for Orx decimated power block configuration
*/
typedef struct adi_adrv904x_ORxDecimatedPowerCfg
{
    uint32_t    orxChannelMask;           /*!< ORx channel selection */
    uint8_t     measurementEnable;        /*!< 0:Disable , 1:Enable power measurements */
    uint8_t     powerInputSelect;         /*!< Signal selection for power measurement. 0: Hb2 input, 1: Hb2 output, 2: Hb1 output */	
    uint8_t     powerMeasurementDuration; /*!< Average power measurement duration = 8*2^powerMeasurementDuration. Valid range from 0 to 21 */
}adi_adrv904x_ORxDecimatedPowerCfg_t;

/**
 *  \brief Enum to set the Rx gain control mode
 */
typedef enum adi_adrv904x_RxGainCtrlMode
{
    ADI_ADRV904X_MGC = 0,    /*!< MGC Mode */
    ADI_ADRV904X_AGC = 2,    /*!< AGC Mode */
} adi_adrv904x_RxGainCtrlMode_e;

/**
 *  \brief Data structure to hold Rx gain control mode configurations
 */
typedef struct adi_adrv904x_RxGainCtrlModeCfg
{
    uint32_t                      rxChannelMask;
    adi_adrv904x_RxGainCtrlMode_e gainCtrlMode;
} adi_adrv904x_RxGainCtrlModeCfg_t;

/**
 * /brief Indicates the frequency of the sine wave test signal with respect to the Rx ADC clock. e.g. DIV_40
 * indicates the sine frequency is 1/40th that of the ADC clock.
 */
typedef enum adi_adrv904x_AdcTestSineFreq
{ 
    ADI_ADRV904X_FS_DIV_40 = 0, 
    ADI_ADRV904X_FS_DIV_20 = 1, 
    ADI_ADRV904X_FS_DIV_10 = 2, 
    ADI_ADRV904X_FS_DIV_5  = 3
} adi_adrv904x_AdcTestSineFreq_e;

/**
 * /brief Indicates if the output of an Rx ADC is to be replaced by a test signal. Also describes the test
 *  signal. The signal is always a full-scale sine wave.
 */
typedef struct adi_adrv904x_RxTestDataCfg
{
    adi_adrv904x_AdcTestSineFreq_e sineFreq; /*<! The frequency of the test signal relative to that of the ADC clock. */
    uint8_t                        enable;   /*<! Non-zero indicates the test signal is to be enabled. */
} adi_adrv904x_RxTestDataCfg_t;

/**
 *  \brief Enum of Carriers within an Rx Channel
 */
typedef enum adi_adrv904x_RxCarrier
{
    ADI_ADRV904X_RX_CARRIER_NONE = 0x00U,
    ADI_ADRV904X_RX_CARRIER_0    = 0x01U,
    ADI_ADRV904X_RX_CARRIER_1    = 0x02U,
    ADI_ADRV904X_RX_CARRIER_2    = 0x04U,
    ADI_ADRV904X_RX_CARRIER_3    = 0x08U,
    ADI_ADRV904X_RX_CARRIER_4    = 0x10U,
    ADI_ADRV904X_RX_CARRIER_5    = 0x20U,
    ADI_ADRV904X_RX_CARRIER_6    = 0x40U,
    ADI_ADRV904X_RX_CARRIER_7    = 0x80U,
    ADI_ADRV904X_RX_CARRIER_ALL  = 0xFFU,
} adi_adrv904x_RxCarrier_e;

#define ADI_ADRV904X_MAX_RX_CARRIERS 8U

/**
 * \brief Data structure to hold Rx Carrier Mask selections (Rx Channel mask + Carrier mask)
 */
typedef struct adi_adrv904x_RxCarrierMask
{
    uint32_t rxChannelMask;     /*!< Rx Channel Mask selection */
    uint32_t carrierMask;       /*!< Carrier Mask selection */
} adi_adrv904x_RxCarrierMask_t;

/**
 * \brief Data structure to hold Rx Carrier Mask selections (Rx Channel + Carrier mask)
 */
typedef struct adi_adrv904x_RxCarrierSel
{
    adi_adrv904x_RxChannels_e   rxChannel;      /*!< Rx Channel selection */
    uint32_t                    carrierMask;    /*!< Carrier Mask selection */
} adi_adrv904x_RxCarrierSel_t;

/**
 *  \brief Enum of Rx Carrier RSSI Measurement modes
 */
typedef enum adi_adrv904x_RxCarrierRssiMeasMode
{
    ADI_ADRV904X_RX_CARRIER_RSSI_MEAS_MODE_IMMEDIATE_FDD  = 0U, /*!< Immediate mode. For use in FDD */
    ADI_ADRV904X_RX_CARRIER_RSSI_MEAS_MODE_TDD_SINGLE     = 1U, /*!< RxOn starts measurement and block waits for next RxOn */
    ADI_ADRV904X_RX_CARRIER_RSSI_MEAS_MODE_TDD_CONTINUOUS = 2U  /*!< RxOn starts first measurement and block waits for a specified delay then starts next measurement */
} adi_adrv904x_RxCarrierRssiMeasMode_e;

/**
 *  \brief Enum of Rx Carrier RSSI Operation modes
 */
typedef enum adi_adrv904x_RxCarrierRssiOpMode
{
    ADI_ADRV904X_RX_CARRIER_RSSI_OP_MODE_SUMPOWER2 = 0U,  /*!< RSSI*Meas Duration = Sum of [mult[n] * 2^(duration[n])] for n=0-3. Power calculation is accurate to 0.5dB */
    ADI_ADRV904X_RX_CARRIER_RSSI_OP_MODE_POWER2    = 1U   /*!< RSSI Meas Duration = 2^(duration0 + 4). Power calculation is accurate to 0.25dB */
} adi_adrv904x_RxCarrierRssiOpMode_e;

/**
 * \brief Data structure to hold Rx Carrier RSSI power meter configuration parameters
 */
typedef struct adi_adrv904x_RxCarrierRssiCfg
{
    adi_adrv904x_RxCarrierMask_t          rxCarrierMask;       /*!< Rx Channel/Carrier RSSI Mask selection */
    adi_adrv904x_RxCarrierRssiMeasMode_e  measMode;            /*!< RSSI Measurement Mode (FDD/TDD1/TDD2) */ /* rssi_mode 2bit */
    uint16_t                              startDelay;          /*!< Delay start of first measurement from RxOn edge (for TDD Single and Continuous measMode's), range: 1 - 0x1FFF */
    uint32_t                              waitDelay;           /*!< Delay start of next measurement (For TDD Continuous Mode only), range: 1 - 0xFFFFFF */
    uint16_t                              pauseDelay;          /*!< Delay resume of measurement after a pause (RxOff->On transition or Manual Pause), range: 1 - 0x3FFF */
    adi_adrv904x_RxCarrierRssiOpMode_e    opMode;              /*!< 1: power of 2 meas specified by duration0, 0: non-power of 2 */ /* rssi_default_mode 2bit */
    uint8_t                               duration0;           /*!< meas duration0 */
    uint8_t                               duration1;           /*!< meas duration1 */
    uint8_t                               duration2;           /*!< meas duration2 */
    uint8_t                               duration3;           /*!< meas duration3 */
    uint16_t                              mulFactor0;          /*!< non-power of 2 mode, the weight of power calculated for duration 0 , range:0-0xFF for Band RSSI, 0-0x3FF for carrier RSSI*/
    uint16_t                              mulFactor1;          /*!< non-power of 2 mode, the weight of power calculated for duration 1 , range:0-0xFF for Band RSSI, 0-0x3FF for carrier RSSI*/
    uint16_t                              mulFactor2;          /*!< non-power of 2 mode, the weight of power calculated for duration 2 , range:0-0xFF for Band RSSI, 0-0x3FF for carrier RSSI*/
    uint16_t                              mulFactor3;          /*!< non-power of 2 mode, the weight of power calculated for duration 3 , range:0-0xFF for Band RSSI, 0-0x3FF for carrier RSSI*/
    uint8_t                               resetAtGainChange;   /*!< Measurement is reset at gain change */ /* rssi_reset_on_gain_change 1bit */
    uint8_t                               fddPinMode;          /*!< For FDD mode only, trigger meas start using a GPIO */ /* rssi_pin_mode */
    uint8_t                               radioSeqTrigEnable;  /*!< 1: Radio Sequencer trigger enabled. RS Trigger selected by radioSeqTrigger, 0: GPIO pin selected as trigger */ /* rssi_trigger_source_select 1bit */
    uint8_t                               radioSeqTrigSelect;  /*!< Radio Sequencer trigger selection 0-7 */ /* rssi_rs_trigger_select 3bit */
    uint32_t                              captureTimeUsec;     /*!< the expected time (usec) it takes to perform an RSSI power measurement */
} adi_adrv904x_RxCarrierRssiCfg_t;


/**
 * \brief Data structure to hold a Carrier RSSI GPIO pin configuration for a single Rx Channel
 */
typedef struct adi_adrv904x_RxCarrierRssiGpioChannelCfg
{
    adi_adrv904x_GpioPinSel_e enablePin;                                            /*!< Input pins: RSSI Enable GPIO selection for all Carriers in a Rx Channel. User can select ADI_ADRV904X_GPIO_INVALID
                                                                                    if they don't need to use Rx Carrier RSSI Enable GPIO inputs for a channel. More than 1 Rx channel can select a single GPIO pin. */
    adi_adrv904x_GpioPinSel_e readyPin[ADI_ADRV904X_MAX_RX_CARRIERS_PER_CHANNEL];   /*!< Output pins: RSSI Ready GPIO selection for each Carrier in a Rx Channel. User can select ADI_ADRV904X_GPIO_INVALID
                                                                                    if they don't need to use a Rx Carrier Ready signal for a carrier. Only 1 Carrier on 1 Rx channel can select a single GPIO pin. */
} adi_adrv904x_RxCarrierRssiGpioChannelCfg_t;

/**
 * \brief Data structure to hold Rx Carrier RSSI GPIO pin configuration parameters
 */
typedef struct adi_adrv904x_RxCarrierRssiGpioCfg
{
    adi_adrv904x_RxCarrierRssiGpioChannelCfg_t rxChannel[ADI_ADRV904X_MAX_RX_ONLY];     /*!< Top-level GPIO structure to configure all Rx Carrier RSSI GPIO pins for all Rx Channels */
} adi_adrv904x_RxCarrierRssiGpioCfg_t;




/**
 *  \brief Enum of Rx Carrier RSSI default Operation modes
 */
typedef enum adi_adrv904x_RxDdcRssiOpMode
{
    ADI_ADRV904X_RX_DDC_RSSI_OP_MODE_SUMPOWER2 = 0U,  /*!< RSSI*Meas Duration = Sum of [mult[n] * 2^(duration[n])] for n=0-3. Power calculation is accurate to 0.5dB */
    ADI_ADRV904X_RX_DDC_RSSI_OP_MODE_POWER2    = 1U   /*!< RSSI Meas Duration = 2^(duration0 + 4). Power calculation is accurate to 0.25dB */
} adi_adrv904x_RxDdcRssiOpMode_e;


/**
 *  \brief Enum of Rx DDC RSSI default modes
 */
typedef enum adi_adrv904x_RxDdcRssiMeasMode
{
    ADI_ADRV904X_RX_DDC_RSSI_MEAS_MODE_IMMEDIATE           = 0U, /*!< Power measurement starts immediately when rssi_enable bit-field is set. */
    ADI_ADRV904X_RX_DDC_RSSI_MEAS_MODE_NEXT_RXON           = 1U, /*!< Power measurement starts at the next RxOn */
    ADI_ADRV904X_RX_DDC_RSSI_MEAS_MODE_NEXT_RXON_THEN_WAIT = 2U  /*!< Power measurement starts at the next RxOn.
                                                                      Once measurement is complete, the block waits for a period specified
                                                                      by rssi_wait_delay and then starts the next measuremen */
} adi_adrv904x_RxDdcRssiMeasMode_e;


/**
 * \brief Data structure to hold Rx (DDC/CDDC) RSSI power meter configuration parameters
 */
typedef struct adi_adrv904x_RxRssiCfg
{
    uint16_t                          meterMask;                  /*!< meter mask */
    uint32_t                          channelMask;                /*!< Rx Channel Mask selection */
    uint8_t                           enableGainCalc;             /*!< When this bit is set, the power measurement includes compensation 
                                                                   *   for the gain that is applied down the datapath.
                                                                   *   When this bit is disabled, the measured power is just what is observed by the
                                                                   *   RSSI block and does not include any gain/attenuation compensation.
                                                                   */
    uint8_t                           gaintableStepSize;          /*!< DDC - LSB resolution is 0.05dB. (6 bits, 0x3F) */
    uint8_t                           resetAtGainChange;          /*!< When this bit is set, the power measurement will be reset on any gain
                                                                   *   index change from the AGC gaintable.
                                                                   */
    uint8_t                           radioSeqPinModeEnable;      /*!< radio seq control, radio_seq_sel. NA for DDC RSSI blocks */
    adi_adrv904x_RxDdcRssiOpMode_e    opMode;                     /*!< RSSI Measurement Default operation Mode
                                                                   *   When this bit is 1, the duration of the power measurement is power of 2 and
                                                                   *   is specified by rssi_meas_duration0.
                                                                   *   The other durations do not affect the measurement.
                                                                   *   When this bit is 0, the duration of measurement is non-power of 2. 
                                                                   *   In this case, two or more of the rssi_meas_duration0/1/2/3 are used.
                                                                   *   The power from each duration is weighted by rssi_mul_factor0/1/2/3.
                                                                   *   If a given multiplication factor is 0, that duration is not included in the calculation.
                                                                   *   It is required that the lower most multiplication factors can be 0.
                                                                   *   For eg: mul0 = x, mul1 = x, mul2 = 0, mul3 = 0 is valid. mul0 = x, mul1 = 0, mul2 = x, mul3 = x is not valid.
                                                                   */
    adi_adrv904x_RxDdcRssiMeasMode_e  measMode;                   /*!< RSSI Measurement mode (2 bits)
                                                                   *   2'b00 - Power measurement starts immediately when rssi_enable bit-field is set.
                                                                   *   2'b01 - Power measurement starts at the next RxOn.
                                                                   *           There is a delay after the RxOn configured by rssi_start_delay that has to expire before measurement starts.
                                                                   *           Once measurement is complete, the block waits for next RxOn.
                                                                   *   2'b10 - Power measurement starts at the next RxOn.
                                                                   *           There is a delay after the RxOn configured by rssi_start_delay that has to expire before measurement starts.
                                                                   *           Once measurement is complete, the block waits for a period specified by rssi_wait_delay and then starts the next measurement.
                                                                   */
    uint16_t                          startDelay;                 /*!< Delay in start of Rx RSSI measurement in ddc_hb1_out_clk cycles after RxON goes high.
                                                                   *   Delay = rxon_start_delay[15:0] * 2^4 Max delay at 983MHz is 1msec.
                                                                   *   (13 bits, 0x1FFF)
                                                                   */
    uint32_t                          contDelay;                 /*!< Delay = rssi_cont_delay * 2^4
                                                                   *   This delay is measured at the end of measurement.
                                                                   *   It delays the start of next measurement in the rssi_mode=0 and 2.
                                                                   *   (24 bits, 0xFFFFFF)
                                                                   */
    uint32_t                          waitDelay;                  /*!< Delay = rssi_wait_delay * 2^4
                                                                   *   This delay is measured at the end of measurement.
                                                                   *   It delays the start of next measurement in the rssi_mode=0 and 2.
                                                                   *   (24 bits, 0xFFFFFF)
                                                                   */
    uint8_t                           fddPinMode;                 /*!< Enable RSSI via GPIO instead of AHB */
    uint8_t                           duration0;                  /*!< Measurement duration for RSSI power.
                                                                   *   Number of samples = 2 ^ (tx_tss_measurement_duration_duc*)
                                                                   *   This is the only duration that is used when rssi_default_mode = 1.
                                                                   *   (5 bits, 0x1F)
                                                                   */
    uint8_t                           duration1;                  /*!< Measurement duration for RSSI power.
                                                                   *   Number of samples = 2 ^ (tx_tss_measurement_duration_duc*)
                                                                   *   This is the only duration that is used when rssi_default_mode = 0.
                                                                   *   It is required to configure the multiplier factor correctly to use this duration.
                                                                   *   (5 bits, 0x1F)
                                                                   */
    uint8_t                           duration2;                  /*!< Measurement duration for RSSI power.
                                                                   *   Number of samples = 2 ^ (tx_tss_measurement_duration_duc*)
                                                                   *   This is the only duration that is used when rssi_default_mode = 0.
                                                                   *   It is required to configure the multiplier factor correctly to use this duration.
                                                                   *   (5 bits, 0x1F)
                                                                   */
    uint8_t                           duration3;                  /*!< Measurement duration for RSSI power.
                                                                   *   Number of samples = 2 ^ (tx_tss_measurement_duration_duc*)
                                                                   *   This is the only duration that is used when rssi_default_mode = 0.
                                                                   *   It is required to configure the multiplier factor correctly to use this duration.
                                                                   *   (5 bits, 0x1F)
                                                                   */
    uint16_t                          mulFactor0;                 /*!< This bit-field is used when rssi_default_mode = 0.
                                                                   *   This is a 0.8 number. This is the weight of power calculated during duration 0.
                                                                   *   (DDC = 8 bits, 0xFF. CDDC = 16 bits, 0xFFFF)
                                                                   */
    uint16_t                          mulFactor1;                 /*!< This bit-field is used when rssi_default_mode = 0.
                                                                   *   This is a 0.8 number. This is the weight of power calculated during duration 1.
                                                                   *   (DDC = 8 bits, 0xFF. CDDC = 16 bits, 0xFFFF)
                                                                   */    
    uint16_t                          mulFactor2;                 /*!< This bit-field is used when rssi_default_mode = 0.
                                                                   *   This is a 0.8 number. This is the weight of power calculated during duration 2.
                                                                   *   (DDC = 8 bits, 0xFF. CDDC = 16 bits, 0xFFFF)
                                                                   */
    uint16_t                          mulFactor3;                 /*!< This bit-field is used when rssi_default_mode = 0.
                                                                   *   This is a 0.8 number. This is the weight of power calculated during duration 3.
                                                                   *   (DDC = 8 bits, 0xFF. CDDC = 16 bits, 0xFFFF)
                                                                   */
    uint32_t                          debug;                      /*!< for debugging */
} adi_adrv904x_RxRssiCfg_t;

#endif /* _ADI_ADRV904X_RX_TYPES_H_ */
