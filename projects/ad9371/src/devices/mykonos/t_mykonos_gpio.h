/*!
 * \file t_mykonos_gpio.h
 * \brief Mykonos GPIO error handling and type defines
 *
 * Mykonos API version: 1.5.2.3566
 */

/**
* \page Disclaimer Legal Disclaimer
* Copyright 2015-2017 Analog Devices Inc.
* Released under the AD9371 API license, for more information see the "LICENSE.txt" file in this zip file.
*
*/

#ifndef T_MYKONOSGPIO_H_
#define T_MYKONOSGPIO_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "common.h"

/**
 *  \brief Enum of unique error codes from the Mykonos GPIO API functions.
 * Each error condition in the library should get its own enum value to allow
 * easy debug of errors.
 */
typedef enum
{
	MYKONOS_ERR_GPIO_OK = 0,
	MYKONOS_ERR_EN_MONITOR_OUT_NOT_ENABLED,
    MYKONOS_ERR_EN_MONITOR_OUT_SRC_CTRL,
    MYKONOS_ERR_MONITOR_OUT_INDEX_RANGE,
    MYKONOS_ERR_GETGPIOMON_INDEX_NULL_PARM,
    MYKONOS_ERR_GETGPIOMON_MONITORMASK_NULL_PARM,

    MYKONOS_ERR_MGCRX2_GPIO_DECPIN_INV_PARAM,
    MYKONOS_ERR_MGCRX2_GPIO_INCPIN_INV_PARAM,
    MYKONOS_ERR_MGCRX2_SOURCE_CONFIG,
    MYKONOS_ERR_MGCRX2_STEP_INV_PARAM,
    MYKONOS_ERR_MGCRX1_GPIO_DECPIN_INV_PARAM,
    MYKONOS_ERR_MGCRX1_GPIO_INCPIN_INV_PARAM,
    MYKONOS_ERR_MGCRX1_STEP_INV_PARAM,
    MYKONOS_ERR_GETRX2PIN_INCSTEP_NULL_PARM,
    MYKONOS_ERR_GETRX2PIN_DECSTEP_NULL_PARM,
    MYKONOS_ERR_GETRX2PIN_INCPIN_NULL_PARM,
    MYKONOS_ERR_GETRX2PIN_DECPIN_NULL_PARM,
    MYKONOS_ERR_GETRX2PIN_EN_NULL_PARM,
    MYKONOS_ERR_GETRX1PIN_INCSTEP_NULL_PARM,
    MYKONOS_ERR_GETRX1PIN_DECSTEP_NULL_PARM,
    MYKONOS_ERR_GETRX1PIN_INCPIN_NULL_PARM,
    MYKONOS_ERR_GETRX1PIN_DECPIN_NULL_PARM,
    MYKONOS_ERR_GETRX1PIN_EN_NULL_PARM,

    MYKONOS_ERR_TPCTX2_GPIO_DECPIN_INV_PARAM,
    MYKONOS_ERR_TPCTX2_GPIO_INCPIN_INV_PARAM,
    MYKONOS_ERR_TPCTX2_GPIO_STEP_INV_PARAM,
    MYKONOS_ERR_TPCTX1_GPIO_DECPIN_INV_PARAM,
    MYKONOS_ERR_TPCTX1_GPIO_INCPIN_INV_PARAM,
    MYKONOS_ERR_TPCTX1_GPIO_STEP_INV_PARAM,
    MYKONOS_ERR_GETTX2PIN_STEP_NULL_PARM,
    MYKONOS_ERR_GETTX2PIN_INC_NULL_PARM,
    MYKONOS_ERR_GETTX2PIN_DEC_NULL_PARM,
    MYKONOS_ERR_GETTX2PIN_EN_NULL_PARM,
    MYKONOS_ERR_GETTX2PIN_TX1TX2_NULL_PARM,
    MYKONOS_ERR_GETTX1PIN_STEP_NULL_PARM,
    MYKONOS_ERR_GETTX1PIN_INC_NULL_PARM,
    MYKONOS_ERR_GETTX1PIN_DEC_NULL_PARM,
    MYKONOS_ERR_GETTX1PIN_EN_NULL_PARM,
    MYKONOS_ERR_GETTX1PIN_TX1TX2_NULL_PARM,

	MYKONOS_ERR_SPI2_INV_GPIO,
	MYKONOS_ERR_SPI2_INV_SETUP,

	MYKONOS_ERR_SET_GPIO_1V8_INV_POINTER,

	MYKONOS_ERR_GETGPIOSETLEVEL_NULL_PARM,
	MYKONOS_ERR_READGPIOSPI_NULL_PARM,
    MYKONOS_ERR_SET_GPIO_1V8_INV_MODE,
    MYKONOS_ERR_GETGPIO_OE_NULL_PARM,
    MYKONOS_ERR_GPIO_OE_INV_PARAM,
    MYKONOS_ERR_GPIO_SRC_PARAM_INV,
    MYKONOS_ERR_GET_GPIO_SOURCE_CONTROL_NULL_PARM,

    MYKONOS_ERR_INV_GP_INT_MASK_PARM,
    MYKONOS_ERR_GP_INT_STATUS_NULL_PARAM,

    MYKONOS_ERR_GETGPIO3V3OUT_NULL_PARM,
    MYKONOS_ERR_SET_GPIO_3V3_INV_POINTER,
    MYKONOS_ERR_GETGPIO3V3SPI_NULL_PARM,
    MYKONOS_ERR_GPIO3V3OUTEN_NULL_PARM,
    MYKONOS_ERR_SET_GPIO_3V3_INV_MODE,
    MYKONOS_ERR_SET_GPIO_3V3_INV_SRC_CTRL,

    MYKONOS_ERR_SET_ARMGPIO_INV_POINTER,
    MYKONOS_ERR_SET_ARMGPIO_PINS_ARMERROR,
    MYKONOS_ERR_SET_ARMGPIO_PINS_INV_SIGNALID,
    MYKONOS_ERR_SET_ARMGPIO_PINS_INV_GPIOPIN,

    MYKONOS_ERR_SETUPAUXDAC_NULL_PARAM,
    MYKONOS_ERR_WRITEAUXDAC_NULL_AUXIO,
    MYKONOS_ERR_READAUXADC_NULL_PARAM,
    MYKONOS_ERR_SETUPAUXDAC_INV_AUXDACCODE,
    MYKONOS_ERR_WRITEAUXDAC_INV_AUXDACCODE,
    MYKONOS_ERR_WRITEAUXDAC_INV_AUXDACINDEX,
    MYKONOS_ERR_INV_AUX_ADC_CHAN_PARM,
    MYKONOS_ERR_SETUPAUXADC_INV_VCODIV,
    MYKONOS_ERR_INV_AUX_ADC_DEC_PARM,

    MYKONOS_ERR_GAINCOMP_NULL_STRUCT,
    MYKONOS_ERR_GAINCOMP_EN_NULL_PARM,
    MYKONOS_ERR_GAINCOMP_SET_NULL_STRUCT,
    MYKONOS_ERR_GAINCOMP_INV_RX1_OFFSET,
    MYKONOS_ERR_GAINCOMP_INV_RX2_OFFSET,
    MYKONOS_ERR_GAINCOMP_INV_STEP,
    MYKONOS_ERR_GAINCOMP_INV_EN,

    MYKONOS_ERR_OBS_RX_GAINCOMP_SET_NULL_STRUCT,
    MYKONOS_ERR_OBS_RX_GAINCOMP_EN_NULL_PARAM,
    MYKONOS_ERR_OBS_RX_GAINCOMP_INV_EN,
    MYKONOS_ERR_OBS_RX_GAINCOMP_INV_OFFSET,
    MYKONOS_ERR_OBS_RX_GAINCOMP_INV_STEP,
    MYKONOS_ERR_OBS_RX_GAINCOMP_NULL_STRUCT,

    MYKONOS_ERR_SLICER_STEP_OUT_OF_RANGE,
    MYKONOS_ERR_SLICER_INV_RX1_SEL,
    MYKONOS_ERR_SLICER_INV_RX2_SEL,
    MYKONOS_ERR_SLICER_EN_INV,

    MYKONOS_ERR_SLICER_RX1PIN_NULL_PARM,
    MYKONOS_ERR_SLICER_RX2PIN_NULL_PARM,
    MYKONOS_ERR_SLICER_STEP_NULL_PARM,
    MYKONOS_ERR_SLICER_EN_NULL_PARM,

    MYKONOS_ERR_SLICER_INV_OBS_RX_SEL,
    MYKONOS_ERR_SLICER_OBS_RX_STEP_OUT_OF_RANGE,
    MYKONOS_ERR_SLICER_OBS_RX_EN_INV,
    MYKONOS_ERR_SLICER_OBS_RX_EN_NULL_PARM,
    MYKONOS_ERR_SLICER_OBS_RX_STEP_NULL_PARM,
    MYKONOS_ERR_SLICER_OBS_RXPIN_NULL_PARM,

    MYKONOS_ERR_FLOATFRMT_NULL_STRUCT,
    MYKONOS_ERR_FLOATFRMT_SET_NULL_STRUCT,
    MYKONOS_ERR_FLOATFRMT_INV_ROUND_MODE,
    MYKONOS_ERR_FLOATFRMT_INV_DATA_FORMAT,
    MYKONOS_ERR_FLOATFRMT_INV_ENC_NAN,
    MYKONOS_ERR_FLOATFRMT_INV_EXP_BITS,
    MYKONOS_ERR_FLOATFRMT_INV_LEADING,
    MYKONOS_ERR_FLOATFRMT_INV_RX1ATT,
    MYKONOS_ERR_FLOATFRMT_INV_RX2ATT,
    MYKONOS_ERR_FLOATFRMT_INV_EN,
    MYKONOS_ERR_FLOATFRMT_NULL_RX1ATT,
    MYKONOS_ERR_FLOATFRMT_NULL_RX2ATT,
    MYKONOS_ERR_FLOATFRMT_NULL_ENABLE,
    MYKONOS_ERR_FLOATFRMT_SET_INV_EN,
    MYKONOS_ERR_FLOATFRMT_SET_INV_RX1ATT,
    MYKONOS_ERR_FLOATFRMT_SET_INV_RX2ATT,

    MYKONOS_ERR_FLOATFRMT_INV_ORXATT,
    MYKONOS_ERR_FLOATFRMT_INV_ORXEN,
    MYKONOS_ERR_FLOATFRMT_NULL_ORXATT,
    MYKONOS_ERR_FLOATFRMT_NULL_ORXENABLE,

    MYKONOS_ERR_SETUPTEMPSENSOR_NULL_PARAM,
    MYKONOS_ERR_SETUPTEMPSENSOR_INV_TEMPDECIMATION,
    MYKONOS_ERR_SETUPTEMPSENSOR_INV_OFFSET,
    MYKONOS_ERR_SETUPTEMPSENSOR_INV_TEMPWINDOW,
    MYKONOS_ERR_GETTEMPSENSORCFG_NULL_PARAM,
    MYKONOS_ERR_READTEMPSENSOR_NULL_PARAM,
    MYKONOS_ERR_READTEMPSENSOR_NOT_LOCKED,

    MYKONOS_ERR_GAIN_CONTROL_NOT_HYBRID,
    MYKONOS_ERR_GPIO_HYBRID_RX1_PIN,
    MYKONOS_ERR_GPIO_HYBRID_RX2_PIN,
    MYKONOS_ERR_GPIO_HYBRID_RX1_PIN_NULL_PARM,
    MYKONOS_ERR_GPIO_HYBRID_RX2_PIN_NULL_PARM,
    MYKONOS_ERR_GPIO_HYBRID_RX1_PIN_READ,
    MYKONOS_ERR_GPIO_HYBRID_RX2_PIN_READ,
    MYKONOS_ERR_AGC_OBS_NOT_IN_HYBRID,
    MYKONOS_ERR_GPIO_HYBRID_ORX_PIN,
    MYKONOS_ERR_GPIO_HYBRID_ORX_PIN_NULL_PARM,

    MYKONOS_ERR_GAIN_CONTROL_NOT_AGC,
    MYKONOS_ERR_GPIO_AGC_SYNC_RX1_PIN,
    MYKONOS_ERR_GPIO_AGC_SYNC_RX2_PIN,
    MYKONOS_ERR_GPIO_AGC_SYNC_RX1_PIN_NULL_PARM,
    MYKONOS_ERR_GPIO_AGC_SYNC_RX2_PIN_NULL_PARM,
    MYKONOS_ERR_OBS_GAIN_CONTROL_NOT_AGC,
    MYKONOS_ERR_GPIO_AGC_SYNC_ORX_PIN,
    MYKONOS_ERR_GPIO_AGC_SYNC_ORX_PIN_NULL_PARM,

    MYKONOS_ERR_GETGPIODRV_NULL_PARAM,
    MYKONOS_ERR_GPIO_DRV_INV_PARAM,

    MYKONOS_ERR_GPIO_SLEW_RATE_INV_PARAM,
    MYKONOS_ERR_GPIO_GETSLEW_NULL_PARAM,

    MYKONOS_ERR_CMOS_DRV_INV_PARAM,
    MYKONOS_ERR_CMOS_DRV_NULL_PARAM
} mykonosGpioErr_t;

/**
 *  \brief Enum of possible Rx Slicer pin combinations
 */
typedef enum
{
    GPIO_0_1_2      = 0,    /*!< GPIO combination for RX1        */
    GPIO_5_6_7      = 1,    /*!< GPIO combination for RX1 or RX2 */
    GPIO_8_9_10     = 2,    /*!< GPIO combination for RX1        */
    GPIO_11_12_13   = 3     /*!< GPIO combination for RX2        */
} mykonosRxSlicer_t;

/**
 *  \brief Enum of possible observation channel Slicer pin combinations
 */
typedef enum
{
    GPIO_18_17_16      = 0,    /*!< GPIO combination for observation channel */
    GPIO_16_15_14      = 1     /*!< GPIO combination for observation channel */
} mykonosObsRxSlicer_t;

/**
 *  \brief Enum of possible GPIO slew rate settings
 */
typedef enum
{
    MYK_SLEWRATE_NONE    = 0,    /*!< Lower slew rate for the GPIO  */
    MYK_SLEWRATE_LOW     = 1,    /*!< Low slew rate for the GPIO    */
    MYK_SLEWRATE_MEDIUM  = 2,    /*!< Medium slew rate for the GPIO */
    MYK_SLEWRATE_HIGH    = 3     /*!< High slew rate for the GPIO   */
} mykonosGpioSlewRate_t;

/**
 * \brief Enumerated list of CMOS pads drive strength options
 */
typedef enum
{
    MYK_CMOSPAD_DRV_1X  = 0x00,       /*!<  2.5pF  load @ 65MHz */
    MYK_CMOSPAD_DRV_2X  = 0x01,       /*!<    5pF  load @ 65MHz */
    MYK_CMOSPAD_DRV_3X  = 0x03,       /*!<  7.5pF  load @ 65MHz */
    MYK_CMOSPAD_DRV_4X  = 0x07,       /*!<   10pF  load @ 65MHz */
    MYK_CMOSPAD_DRV_5X  = 0x0F        /*!< 12.5pF  load @ 65MHz */
} mykonosCmosPadDrvStr_t;


/**
 * \brief Enumerated list for Aux ADCs
 */
typedef enum
{
    MYK_AUXADC_0        = 0x00,       /*!< Aux ADC channel 0 */
    MYK_AUXADC_1        = 0x01,       /*!< Aux ADC channel 1 */
    MYK_AUXADC_2        = 0x02,       /*!< Aux ADC channel 2 */
    MYK_AUXADC_3        = 0x03,       /*!< Aux ADC channel 3 */
    MYK_AUXADC_0_DIV2   = 0x04,       /*!< Aux ADC channel 0 with the divider by 2 set */
    MYK_TEMPSENSOR      = 0x10        /*!< Temperature sensor channel */
} mykonosAuxAdcChannels_t;


/**
 *  \brief Data structure to hold Gain compensation settings for the main receive channels
 **/
typedef struct
{
    uint8_t rx1Offset;  /*!< These parameter contains the Rx1 offset word used for the gain compensation
                        when the gain index is at its maximum setting.
                         It has a range of 0 to 0x1F with a resolution is 0.5dB per LSB. */
    uint8_t rx2Offset;  /*!< These parameter contains the Rx2 offset word used for the gain compensation
                        when the gain index is at its maximum setting.
                        It has a range of 0 to 0x1F with a resolution is 0.5dB per LSB. */
    uint8_t compStep;   /*!< These bits contains the value in dB that the total Rx gain changes
                         when there is an LSB change in the gain index according to the following settings:
                         compStep |  dB ramp
                         ---------|------------
                            0     |   0.25dB
                            1     |   0.5dB
                            2     |   1.0dB
                            3     |   2.0dB
                            4     |   3.0dB
                            5     |   4.0dB
                            6     |   6.0dB
                            7     |   Not valid defaulted to 0.25dB */
} mykonosGainComp_t;

/**
 *  \brief Data structure to hold Gain compensation settings for the observation channel
 **/
typedef struct
{
    uint8_t obsRxOffset;    /*!< These parameter contains the Rx1 offset word used for the gain compensation
                            when the gain index is at its maximum setting.
                            It has a range of 0 to 0x1F with a resolution is 0.5dB per LSB. */
    uint8_t compStep;       /*!< These bits contains the value in dB that the total Rx gain changes
                            when there is an LSB change in the gain index according to the following settings:
                            compStep |  dB ramp
                            ---------|------------
                               0     |   0.25dB
                               1     |   0.5dB
                               2     |   1.0dB
                               3     |   2.0dB
                               4     |   3.0dB
                               5     |   4.0dB
                               6     |   6.0dB
                               7     |   Not valid defaulted to 0.25dB */
} mykonosObsRxGainComp_t;

/**
 * \brief Data structure to hold floating point formatter settings for the floating point
 *  number generation
 **/
typedef struct
{
   uint8_t roundMode;   /*!<These parameter set the round modes for the significand.
                        The following settings are defined in the IEEE754 specification:
                        roundMode |  Round type
                        ----------|-----------------
                             0    |  RoundTiesToEven
                             1    |  RoundTowardsPositive
                             2    |  RoundTowardsNegative
                             3    |  RoundTowardsZero
                             4    |  RoundTiesToAway*/
   uint8_t dataFormat;  /*!< These parameter sets the format of the 16-bit output on the JESD interface:
                        Setting this to 1 then the format is from MSB to LSB to {sign, significand, exponent}.
                        Clearing this bit sets the format to {sign, exponent, significand}.*/
   uint8_t encNan;      /*!< if this parameter is set to 1 then the floating point formatter reserves the highest value
                        of exponent for NaN to be compatible to the IEEE754 specification.
                        Clearing this parameter increases the range of the exponent by one.*/
   uint8_t expBits;     /*!<These parameter is used to indicate the number of exponent bits in the floating point number
                        according to the following settings:
                          expBits|  Round type
                         --------|------------------------------------------------
                            0    |  2 bit exponent, 13 bit significand, 1 bit sign
                            1    |  3 bit exponent, 12 bit significand, 1 bit sign
                            2    |  4 bit exponent, 11 bit significand, 1 bit sign
                            3    |  5 bit exponent, 10 bit significand, 1 bit sign */
   uint8_t leading;     /*!< Setting this parameter hides the leading one in the the significand to be compatible to the IEEE754 specification.
                        Clearing this parameter causes the leading one to be at the MSB of the significand.*/
}mykonosFloatPntFrmt_t;

/**
 *  \brief Data structure used to configure the on-die Temperature Sensor
 */
typedef struct
{
    uint8_t tempDecimation;      /*!<3-bit value that controls the AuxADC decimation factor when used for temp sensor calculations;
                                 AuxADC_decimation = 256 * 2^tempDecimation */
    uint8_t offset;              /*!< 8-bit offset that gets added to temp sensor code internally */
    uint8_t overrideFusedOffset; /*!< This bit overrides the factory-calibrated fuse offset
                                 and uses the value stored in the offset member */
    uint8_t tempWindow;          /*!<  4-bit code with a resolution of 1?C/LSB, each time a temperature measurement is performed,
                                 the device compares the current temperature against the previous value.*/
}mykonosTempSensorConfig_t;


/**
 *  \brief Data structure used to store Temperature Sensor related values
 */
typedef struct
{
    int16_t tempCode;       /*!< 16-bit signed temperature value (in deg C) that is read back */
    uint8_t windowExceeded; /*!< If the absolute value of the difference is greater than the value in temperature configuration
                            tempWindow, the windowExceeded flag is set.*/
    uint8_t windowHiLo;     /*!<when windowExceeded member gets set,
                            this bit is set to 1 if current value is greater than previous value, else reset */
    uint8_t tempValid;      /*!< When the reading is complete and a valid temperature value stored in tempCode*/
}mykonosTempSensorStatus_t;


#ifdef __cplusplus
}
#endif

#endif /* T_MYKONOSGPIO_H_ */
