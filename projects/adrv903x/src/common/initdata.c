
            /**
            * Copyright 2015 - 2025 Analog Devices Inc.
            * SPDX-License-Identifier: Apache-2.0
            */#include "initdata.h"

adi_adrv903x_Version_t initStructApiVersion = {2, 12, 1, 4};

adi_adrv903x_CpuFwVersion_t initStructArmVersion = { {2, 12, 1, 4} , ADI_ADRV903X_CPU_FW_BUILD_RELEASE};

adi_adrv903x_Version_t initStructStreamVersion = {2, 12, 1, 4};

adi_adrv903x_Init_t deviceInitStruct = 
{
    { // spiOptionsInit
        1,  // allowSpiStreaming
        1,  // allowAhbAutoIncrement
        1,  // allowAhbSpiFifoMode
    },
    { // clocks
        0,  // DevClkOnChipTermResEn
    },
    { // cpuMemDump
        { // filePath (array)
            68, 101, 118, 105, 99, 101, 67, 112, 117, 77, 101, 109, 68, 117, 109, 112,
            46, 98, 105, 110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        }, // filePath (end of array)
    },
    { // rx
        { // rxChannelCfg (array)
            {  // rxChannelCfg[0]
                { // rxDataFormat
                    ADI_ADRV903X_GAIN_COMPENSATION_DISABLED,  // formatSelect
                    { // floatingPointConfig
                        ADI_ADRV903X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,  // fpDataFormat
                        ADI_ADRV903X_ROUND_TO_EVEN,  // fpRoundMode
                        ADI_ADRV903X_2_EXPONENTBITS,  // fpNumExpBits
                        ADI_ADRV903X_FPATTEN_0DB,  // fpAttenSteps
                        ADI_ADRV903X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,  // fpHideLeadingOne
                        ADI_ADRV903X_FP_FORMAT_NAN_ENCODE_DISABLE,  // fpEncodeNan
                    },
                    { // integerConfigSettings
                        ADI_ADRV903X_NO_EMBEDDED_SLICER_BITS,  // intEmbeddedBits
                        ADI_ADRV903X_INTEGER_16BIT_2SCOMP,  // intSampleResolution
                        ADI_ADRV903X_NO_PARITY,  // intParity
                        ADI_ADRV903X_LOWER_NIBBLE_ON_I,  // intEmbeddedPos
                    },
                    { // slicerConfigSettings
                        ADI_ADRV903X_EXTSLICER_STEPSIZE_1DB,  // extSlicerStepSize
                        ADI_ADRV903X_INTSLICER_STEPSIZE_1DB,  // intSlicerStepSize
                        ADI_ADRV903X_EXTSLICER_RX_GPIO_DISABLE,  // extSlicerGpioSelect
                        { // intSlicerGpioSelect (array)
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    { // embOvldMonitorSettings
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbQ
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneQ
                        ADI_ADRV903X_HB2_LOW_SRC_OVRG_LOW,  // embeddedMonitorHb2LowSrcSel
                        ADI_ADRV903X_HB2_HIGH_SRC_OVRG_HIGH,  // embeddedMonitorHb2HighSrcSel
                        ADI_ADRV903X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdLowSrcSel
                        ADI_ADRV903X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdHighSrcSel
                        0,  // invertHb2Flag
                        0,  // invertApdFlag
                    },
                    0,  // externalLnaGain
                    0,  // tempCompensationEnable
                },
                255,  // rxGainIndexInit
            }, // rxChannelCfg[0]
            {  // rxChannelCfg[1]
                { // rxDataFormat
                    ADI_ADRV903X_GAIN_COMPENSATION_DISABLED,  // formatSelect
                    { // floatingPointConfig
                        ADI_ADRV903X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,  // fpDataFormat
                        ADI_ADRV903X_ROUND_TO_EVEN,  // fpRoundMode
                        ADI_ADRV903X_2_EXPONENTBITS,  // fpNumExpBits
                        ADI_ADRV903X_FPATTEN_0DB,  // fpAttenSteps
                        ADI_ADRV903X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,  // fpHideLeadingOne
                        ADI_ADRV903X_FP_FORMAT_NAN_ENCODE_DISABLE,  // fpEncodeNan
                    },
                    { // integerConfigSettings
                        ADI_ADRV903X_NO_EMBEDDED_SLICER_BITS,  // intEmbeddedBits
                        ADI_ADRV903X_INTEGER_16BIT_2SCOMP,  // intSampleResolution
                        ADI_ADRV903X_NO_PARITY,  // intParity
                        ADI_ADRV903X_LOWER_NIBBLE_ON_I,  // intEmbeddedPos
                    },
                    { // slicerConfigSettings
                        ADI_ADRV903X_EXTSLICER_STEPSIZE_1DB,  // extSlicerStepSize
                        ADI_ADRV903X_INTSLICER_STEPSIZE_1DB,  // intSlicerStepSize
                        ADI_ADRV903X_EXTSLICER_RX_GPIO_DISABLE,  // extSlicerGpioSelect
                        { // intSlicerGpioSelect (array)
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    { // embOvldMonitorSettings
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbQ
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneQ
                        ADI_ADRV903X_HB2_LOW_SRC_OVRG_LOW,  // embeddedMonitorHb2LowSrcSel
                        ADI_ADRV903X_HB2_HIGH_SRC_OVRG_HIGH,  // embeddedMonitorHb2HighSrcSel
                        ADI_ADRV903X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdLowSrcSel
                        ADI_ADRV903X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdHighSrcSel
                        0,  // invertHb2Flag
                        0,  // invertApdFlag
                    },
                    0,  // externalLnaGain
                    0,  // tempCompensationEnable
                },
                255,  // rxGainIndexInit
            }, // rxChannelCfg[1]
            {  // rxChannelCfg[2]
                { // rxDataFormat
                    ADI_ADRV903X_GAIN_COMPENSATION_DISABLED,  // formatSelect
                    { // floatingPointConfig
                        ADI_ADRV903X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,  // fpDataFormat
                        ADI_ADRV903X_ROUND_TO_EVEN,  // fpRoundMode
                        ADI_ADRV903X_2_EXPONENTBITS,  // fpNumExpBits
                        ADI_ADRV903X_FPATTEN_0DB,  // fpAttenSteps
                        ADI_ADRV903X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,  // fpHideLeadingOne
                        ADI_ADRV903X_FP_FORMAT_NAN_ENCODE_DISABLE,  // fpEncodeNan
                    },
                    { // integerConfigSettings
                        ADI_ADRV903X_NO_EMBEDDED_SLICER_BITS,  // intEmbeddedBits
                        ADI_ADRV903X_INTEGER_16BIT_2SCOMP,  // intSampleResolution
                        ADI_ADRV903X_NO_PARITY,  // intParity
                        ADI_ADRV903X_LOWER_NIBBLE_ON_I,  // intEmbeddedPos
                    },
                    { // slicerConfigSettings
                        ADI_ADRV903X_EXTSLICER_STEPSIZE_1DB,  // extSlicerStepSize
                        ADI_ADRV903X_INTSLICER_STEPSIZE_1DB,  // intSlicerStepSize
                        ADI_ADRV903X_EXTSLICER_RX_GPIO_DISABLE,  // extSlicerGpioSelect
                        { // intSlicerGpioSelect (array)
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    { // embOvldMonitorSettings
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbQ
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneQ
                        ADI_ADRV903X_HB2_LOW_SRC_OVRG_LOW,  // embeddedMonitorHb2LowSrcSel
                        ADI_ADRV903X_HB2_HIGH_SRC_OVRG_HIGH,  // embeddedMonitorHb2HighSrcSel
                        ADI_ADRV903X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdLowSrcSel
                        ADI_ADRV903X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdHighSrcSel
                        0,  // invertHb2Flag
                        0,  // invertApdFlag
                    },
                    0,  // externalLnaGain
                    0,  // tempCompensationEnable
                },
                255,  // rxGainIndexInit
            }, // rxChannelCfg[2]
            {  // rxChannelCfg[3]
                { // rxDataFormat
                    ADI_ADRV903X_GAIN_COMPENSATION_DISABLED,  // formatSelect
                    { // floatingPointConfig
                        ADI_ADRV903X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,  // fpDataFormat
                        ADI_ADRV903X_ROUND_TO_EVEN,  // fpRoundMode
                        ADI_ADRV903X_2_EXPONENTBITS,  // fpNumExpBits
                        ADI_ADRV903X_FPATTEN_0DB,  // fpAttenSteps
                        ADI_ADRV903X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,  // fpHideLeadingOne
                        ADI_ADRV903X_FP_FORMAT_NAN_ENCODE_DISABLE,  // fpEncodeNan
                    },
                    { // integerConfigSettings
                        ADI_ADRV903X_NO_EMBEDDED_SLICER_BITS,  // intEmbeddedBits
                        ADI_ADRV903X_INTEGER_16BIT_2SCOMP,  // intSampleResolution
                        ADI_ADRV903X_NO_PARITY,  // intParity
                        ADI_ADRV903X_LOWER_NIBBLE_ON_I,  // intEmbeddedPos
                    },
                    { // slicerConfigSettings
                        ADI_ADRV903X_EXTSLICER_STEPSIZE_1DB,  // extSlicerStepSize
                        ADI_ADRV903X_INTSLICER_STEPSIZE_1DB,  // intSlicerStepSize
                        ADI_ADRV903X_EXTSLICER_RX_GPIO_DISABLE,  // extSlicerGpioSelect
                        { // intSlicerGpioSelect (array)
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    { // embOvldMonitorSettings
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbQ
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneQ
                        ADI_ADRV903X_HB2_LOW_SRC_OVRG_LOW,  // embeddedMonitorHb2LowSrcSel
                        ADI_ADRV903X_HB2_HIGH_SRC_OVRG_HIGH,  // embeddedMonitorHb2HighSrcSel
                        ADI_ADRV903X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdLowSrcSel
                        ADI_ADRV903X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdHighSrcSel
                        0,  // invertHb2Flag
                        0,  // invertApdFlag
                    },
                    0,  // externalLnaGain
                    0,  // tempCompensationEnable
                },
                255,  // rxGainIndexInit
            }, // rxChannelCfg[3]
            {  // rxChannelCfg[4]
                { // rxDataFormat
                    ADI_ADRV903X_GAIN_COMPENSATION_DISABLED,  // formatSelect
                    { // floatingPointConfig
                        ADI_ADRV903X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,  // fpDataFormat
                        ADI_ADRV903X_ROUND_TO_EVEN,  // fpRoundMode
                        ADI_ADRV903X_2_EXPONENTBITS,  // fpNumExpBits
                        ADI_ADRV903X_FPATTEN_0DB,  // fpAttenSteps
                        ADI_ADRV903X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,  // fpHideLeadingOne
                        ADI_ADRV903X_FP_FORMAT_NAN_ENCODE_DISABLE,  // fpEncodeNan
                    },
                    { // integerConfigSettings
                        ADI_ADRV903X_NO_EMBEDDED_SLICER_BITS,  // intEmbeddedBits
                        ADI_ADRV903X_INTEGER_16BIT_2SCOMP,  // intSampleResolution
                        ADI_ADRV903X_NO_PARITY,  // intParity
                        ADI_ADRV903X_LOWER_NIBBLE_ON_I,  // intEmbeddedPos
                    },
                    { // slicerConfigSettings
                        ADI_ADRV903X_EXTSLICER_STEPSIZE_1DB,  // extSlicerStepSize
                        ADI_ADRV903X_INTSLICER_STEPSIZE_1DB,  // intSlicerStepSize
                        ADI_ADRV903X_EXTSLICER_RX_GPIO_DISABLE,  // extSlicerGpioSelect
                        { // intSlicerGpioSelect (array)
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    { // embOvldMonitorSettings
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbQ
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneQ
                        ADI_ADRV903X_HB2_LOW_SRC_OVRG_LOW,  // embeddedMonitorHb2LowSrcSel
                        ADI_ADRV903X_HB2_HIGH_SRC_OVRG_HIGH,  // embeddedMonitorHb2HighSrcSel
                        ADI_ADRV903X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdLowSrcSel
                        ADI_ADRV903X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdHighSrcSel
                        0,  // invertHb2Flag
                        0,  // invertApdFlag
                    },
                    0,  // externalLnaGain
                    0,  // tempCompensationEnable
                },
                255,  // rxGainIndexInit
            }, // rxChannelCfg[4]
            {  // rxChannelCfg[5]
                { // rxDataFormat
                    ADI_ADRV903X_GAIN_COMPENSATION_DISABLED,  // formatSelect
                    { // floatingPointConfig
                        ADI_ADRV903X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,  // fpDataFormat
                        ADI_ADRV903X_ROUND_TO_EVEN,  // fpRoundMode
                        ADI_ADRV903X_2_EXPONENTBITS,  // fpNumExpBits
                        ADI_ADRV903X_FPATTEN_0DB,  // fpAttenSteps
                        ADI_ADRV903X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,  // fpHideLeadingOne
                        ADI_ADRV903X_FP_FORMAT_NAN_ENCODE_DISABLE,  // fpEncodeNan
                    },
                    { // integerConfigSettings
                        ADI_ADRV903X_NO_EMBEDDED_SLICER_BITS,  // intEmbeddedBits
                        ADI_ADRV903X_INTEGER_16BIT_2SCOMP,  // intSampleResolution
                        ADI_ADRV903X_NO_PARITY,  // intParity
                        ADI_ADRV903X_LOWER_NIBBLE_ON_I,  // intEmbeddedPos
                    },
                    { // slicerConfigSettings
                        ADI_ADRV903X_EXTSLICER_STEPSIZE_1DB,  // extSlicerStepSize
                        ADI_ADRV903X_INTSLICER_STEPSIZE_1DB,  // intSlicerStepSize
                        ADI_ADRV903X_EXTSLICER_RX_GPIO_DISABLE,  // extSlicerGpioSelect
                        { // intSlicerGpioSelect (array)
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    { // embOvldMonitorSettings
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbQ
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneQ
                        ADI_ADRV903X_HB2_LOW_SRC_OVRG_LOW,  // embeddedMonitorHb2LowSrcSel
                        ADI_ADRV903X_HB2_HIGH_SRC_OVRG_HIGH,  // embeddedMonitorHb2HighSrcSel
                        ADI_ADRV903X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdLowSrcSel
                        ADI_ADRV903X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdHighSrcSel
                        0,  // invertHb2Flag
                        0,  // invertApdFlag
                    },
                    0,  // externalLnaGain
                    0,  // tempCompensationEnable
                },
                255,  // rxGainIndexInit
            }, // rxChannelCfg[5]
            {  // rxChannelCfg[6]
                { // rxDataFormat
                    ADI_ADRV903X_GAIN_COMPENSATION_DISABLED,  // formatSelect
                    { // floatingPointConfig
                        ADI_ADRV903X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,  // fpDataFormat
                        ADI_ADRV903X_ROUND_TO_EVEN,  // fpRoundMode
                        ADI_ADRV903X_2_EXPONENTBITS,  // fpNumExpBits
                        ADI_ADRV903X_FPATTEN_0DB,  // fpAttenSteps
                        ADI_ADRV903X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,  // fpHideLeadingOne
                        ADI_ADRV903X_FP_FORMAT_NAN_ENCODE_DISABLE,  // fpEncodeNan
                    },
                    { // integerConfigSettings
                        ADI_ADRV903X_NO_EMBEDDED_SLICER_BITS,  // intEmbeddedBits
                        ADI_ADRV903X_INTEGER_16BIT_2SCOMP,  // intSampleResolution
                        ADI_ADRV903X_NO_PARITY,  // intParity
                        ADI_ADRV903X_LOWER_NIBBLE_ON_I,  // intEmbeddedPos
                    },
                    { // slicerConfigSettings
                        ADI_ADRV903X_EXTSLICER_STEPSIZE_1DB,  // extSlicerStepSize
                        ADI_ADRV903X_INTSLICER_STEPSIZE_1DB,  // intSlicerStepSize
                        ADI_ADRV903X_EXTSLICER_RX_GPIO_DISABLE,  // extSlicerGpioSelect
                        { // intSlicerGpioSelect (array)
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    { // embOvldMonitorSettings
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbQ
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneQ
                        ADI_ADRV903X_HB2_LOW_SRC_OVRG_LOW,  // embeddedMonitorHb2LowSrcSel
                        ADI_ADRV903X_HB2_HIGH_SRC_OVRG_HIGH,  // embeddedMonitorHb2HighSrcSel
                        ADI_ADRV903X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdLowSrcSel
                        ADI_ADRV903X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdHighSrcSel
                        0,  // invertHb2Flag
                        0,  // invertApdFlag
                    },
                    0,  // externalLnaGain
                    0,  // tempCompensationEnable
                },
                255,  // rxGainIndexInit
            }, // rxChannelCfg[6]
            {  // rxChannelCfg[7]
                { // rxDataFormat
                    ADI_ADRV903X_GAIN_COMPENSATION_DISABLED,  // formatSelect
                    { // floatingPointConfig
                        ADI_ADRV903X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,  // fpDataFormat
                        ADI_ADRV903X_ROUND_TO_EVEN,  // fpRoundMode
                        ADI_ADRV903X_2_EXPONENTBITS,  // fpNumExpBits
                        ADI_ADRV903X_FPATTEN_0DB,  // fpAttenSteps
                        ADI_ADRV903X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,  // fpHideLeadingOne
                        ADI_ADRV903X_FP_FORMAT_NAN_ENCODE_DISABLE,  // fpEncodeNan
                    },
                    { // integerConfigSettings
                        ADI_ADRV903X_NO_EMBEDDED_SLICER_BITS,  // intEmbeddedBits
                        ADI_ADRV903X_INTEGER_16BIT_2SCOMP,  // intSampleResolution
                        ADI_ADRV903X_NO_PARITY,  // intParity
                        ADI_ADRV903X_LOWER_NIBBLE_ON_I,  // intEmbeddedPos
                    },
                    { // slicerConfigSettings
                        ADI_ADRV903X_EXTSLICER_STEPSIZE_1DB,  // extSlicerStepSize
                        ADI_ADRV903X_INTSLICER_STEPSIZE_1DB,  // intSlicerStepSize
                        ADI_ADRV903X_EXTSLICER_RX_GPIO_DISABLE,  // extSlicerGpioSelect
                        { // intSlicerGpioSelect (array)
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    { // embOvldMonitorSettings
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbQ
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneQ
                        ADI_ADRV903X_HB2_LOW_SRC_OVRG_LOW,  // embeddedMonitorHb2LowSrcSel
                        ADI_ADRV903X_HB2_HIGH_SRC_OVRG_HIGH,  // embeddedMonitorHb2HighSrcSel
                        ADI_ADRV903X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdLowSrcSel
                        ADI_ADRV903X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdHighSrcSel
                        0,  // invertHb2Flag
                        0,  // invertApdFlag
                    },
                    0,  // externalLnaGain
                    0,  // tempCompensationEnable
                },
                255,  // rxGainIndexInit
            }, // rxChannelCfg[7]
            {  // rxChannelCfg[8]
                { // rxDataFormat
                    ADI_ADRV903X_GAIN_COMPENSATION_DISABLED,  // formatSelect
                    { // floatingPointConfig
                        ADI_ADRV903X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,  // fpDataFormat
                        ADI_ADRV903X_ROUND_TO_EVEN,  // fpRoundMode
                        ADI_ADRV903X_2_EXPONENTBITS,  // fpNumExpBits
                        ADI_ADRV903X_FPATTEN_0DB,  // fpAttenSteps
                        ADI_ADRV903X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,  // fpHideLeadingOne
                        ADI_ADRV903X_FP_FORMAT_NAN_ENCODE_DISABLE,  // fpEncodeNan
                    },
                    { // integerConfigSettings
                        ADI_ADRV903X_NO_EMBEDDED_SLICER_BITS,  // intEmbeddedBits
                        ADI_ADRV903X_INTEGER_16BIT_2SCOMP,  // intSampleResolution
                        ADI_ADRV903X_NO_PARITY,  // intParity
                        ADI_ADRV903X_LOWER_NIBBLE_ON_I,  // intEmbeddedPos
                    },
                    { // slicerConfigSettings
                        ADI_ADRV903X_EXTSLICER_STEPSIZE_1DB,  // extSlicerStepSize
                        ADI_ADRV903X_INTSLICER_STEPSIZE_1DB,  // intSlicerStepSize
                        ADI_ADRV903X_EXTSLICER_RX_GPIO_DISABLE,  // extSlicerGpioSelect
                        { // intSlicerGpioSelect (array)
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    { // embOvldMonitorSettings
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbQ
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneQ
                        ADI_ADRV903X_HB2_LOW_SRC_OVRG_LOW,  // embeddedMonitorHb2LowSrcSel
                        ADI_ADRV903X_HB2_HIGH_SRC_OVRG_HIGH,  // embeddedMonitorHb2HighSrcSel
                        ADI_ADRV903X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdLowSrcSel
                        ADI_ADRV903X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdHighSrcSel
                        0,  // invertHb2Flag
                        0,  // invertApdFlag
                    },
                    0,  // externalLnaGain
                    0,  // tempCompensationEnable
                },
                255,  // rxGainIndexInit
            }, // rxChannelCfg[8]
            {  // rxChannelCfg[9]
                { // rxDataFormat
                    ADI_ADRV903X_GAIN_COMPENSATION_DISABLED,  // formatSelect
                    { // floatingPointConfig
                        ADI_ADRV903X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,  // fpDataFormat
                        ADI_ADRV903X_ROUND_TO_EVEN,  // fpRoundMode
                        ADI_ADRV903X_2_EXPONENTBITS,  // fpNumExpBits
                        ADI_ADRV903X_FPATTEN_0DB,  // fpAttenSteps
                        ADI_ADRV903X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,  // fpHideLeadingOne
                        ADI_ADRV903X_FP_FORMAT_NAN_ENCODE_DISABLE,  // fpEncodeNan
                    },
                    { // integerConfigSettings
                        ADI_ADRV903X_NO_EMBEDDED_SLICER_BITS,  // intEmbeddedBits
                        ADI_ADRV903X_INTEGER_16BIT_2SCOMP,  // intSampleResolution
                        ADI_ADRV903X_NO_PARITY,  // intParity
                        ADI_ADRV903X_LOWER_NIBBLE_ON_I,  // intEmbeddedPos
                    },
                    { // slicerConfigSettings
                        ADI_ADRV903X_EXTSLICER_STEPSIZE_1DB,  // extSlicerStepSize
                        ADI_ADRV903X_INTSLICER_STEPSIZE_1DB,  // intSlicerStepSize
                        ADI_ADRV903X_EXTSLICER_RX_GPIO_DISABLE,  // extSlicerGpioSelect
                        { // intSlicerGpioSelect (array)
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV903X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    { // embOvldMonitorSettings
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbQ
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneI
                        ADI_ADRV903X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,  // embeddedMonitorSrcLsbPlusOneQ
                        ADI_ADRV903X_HB2_LOW_SRC_OVRG_LOW,  // embeddedMonitorHb2LowSrcSel
                        ADI_ADRV903X_HB2_HIGH_SRC_OVRG_HIGH,  // embeddedMonitorHb2HighSrcSel
                        ADI_ADRV903X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdLowSrcSel
                        ADI_ADRV903X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,  // embeddedMonitorApdHighSrcSel
                        0,  // invertHb2Flag
                        0,  // invertApdFlag
                    },
                    0,  // externalLnaGain
                    0,  // tempCompensationEnable
                },
                255,  // rxGainIndexInit
            }, // rxChannelCfg[9]
        }, // rxChannelCfg (end of array)
    },
    { // tx
        { // txChannelCfg (array)
            {  // txChannelCfg[0]
                { // txAttenCfg
                    { // updateCfg
                        { // srcCfg
                            ADI_ADRV903X_TXATTEN_UPD_SRC_S0,  // updateSrc
                            ADI_ADRV903X_GPIO_INVALID,  // s0OrS1Gpio
                        },
                        { // trgCfg
                            ADI_ADRV903X_TXATTEN_UPD_TRG_NONE,  // updateTrg
                            ADI_ADRV903X_GPIO_INVALID,  // triggerGpio
                        },
                    },
                    ADI_ADRV903X_TXATTEN_0P05_DB,  // txAttenStepSize
                },
                0,  // txAttenInit_mdB
                { // txpowerMonitorCfg
                    ADI_ADRV903X_HB1_OUTPUT,  // inputSel
                    0,  // peakThreshold
                    0,  // measDuration
                    0,  // peakCount
                    0,  // peakErrorClearRequired
                    0,  // peakPowerEnable
                    0,  // peakPowerIrqEnable
                    0,  // avgThreshold
                    0,  // avgErrorClearRequired
                    0,  // avgPowerEnable
                    0,  // avgPowerIrqEnable
                    0,  // avgPeakRatioEnable
                },
                { // srlCfg
                    ADI_ADRV903X_HB1_OUTPUT,  // inputSel
                    0,  // srdOffset
                    0,  // srdEnable
                    0,  // srdIrqEnable
                    0,  // autoRecoveryWaitTime
                    0,  // autoRecoveryEnable
                    0,  // autoRecoveryDisableTimerWhenTxOff
                    0,  // srdStatisticsEnable
                    0,  // srdStatisticsMode
                },
                { // protectionRampCfg
                    0x00,  // rampDownMask
                    0,  // altEventClearReqd
                },
            }, // txChannelCfg[0]
            {  // txChannelCfg[1]
                { // txAttenCfg
                    { // updateCfg
                        { // srcCfg
                            ADI_ADRV903X_TXATTEN_UPD_SRC_S0,  // updateSrc
                            ADI_ADRV903X_GPIO_INVALID,  // s0OrS1Gpio
                        },
                        { // trgCfg
                            ADI_ADRV903X_TXATTEN_UPD_TRG_NONE,  // updateTrg
                            ADI_ADRV903X_GPIO_INVALID,  // triggerGpio
                        },
                    },
                    ADI_ADRV903X_TXATTEN_0P05_DB,  // txAttenStepSize
                },
                0,  // txAttenInit_mdB
                { // txpowerMonitorCfg
                    ADI_ADRV903X_HB1_OUTPUT,  // inputSel
                    0,  // peakThreshold
                    0,  // measDuration
                    0,  // peakCount
                    0,  // peakErrorClearRequired
                    0,  // peakPowerEnable
                    0,  // peakPowerIrqEnable
                    0,  // avgThreshold
                    0,  // avgErrorClearRequired
                    0,  // avgPowerEnable
                    0,  // avgPowerIrqEnable
                    0,  // avgPeakRatioEnable
                },
                { // srlCfg
                    ADI_ADRV903X_HB1_OUTPUT,  // inputSel
                    0,  // srdOffset
                    0,  // srdEnable
                    0,  // srdIrqEnable
                    0,  // autoRecoveryWaitTime
                    0,  // autoRecoveryEnable
                    0,  // autoRecoveryDisableTimerWhenTxOff
                    0,  // srdStatisticsEnable
                    0,  // srdStatisticsMode
                },
                { // protectionRampCfg
                    0x00,  // rampDownMask
                    0,  // altEventClearReqd
                },
            }, // txChannelCfg[1]
            {  // txChannelCfg[2]
                { // txAttenCfg
                    { // updateCfg
                        { // srcCfg
                            ADI_ADRV903X_TXATTEN_UPD_SRC_S0,  // updateSrc
                            ADI_ADRV903X_GPIO_INVALID,  // s0OrS1Gpio
                        },
                        { // trgCfg
                            ADI_ADRV903X_TXATTEN_UPD_TRG_NONE,  // updateTrg
                            ADI_ADRV903X_GPIO_INVALID,  // triggerGpio
                        },
                    },
                    ADI_ADRV903X_TXATTEN_0P05_DB,  // txAttenStepSize
                },
                0,  // txAttenInit_mdB
                { // txpowerMonitorCfg
                    ADI_ADRV903X_HB1_OUTPUT,  // inputSel
                    0,  // peakThreshold
                    0,  // measDuration
                    0,  // peakCount
                    0,  // peakErrorClearRequired
                    0,  // peakPowerEnable
                    0,  // peakPowerIrqEnable
                    0,  // avgThreshold
                    0,  // avgErrorClearRequired
                    0,  // avgPowerEnable
                    0,  // avgPowerIrqEnable
                    0,  // avgPeakRatioEnable
                },
                { // srlCfg
                    ADI_ADRV903X_HB1_OUTPUT,  // inputSel
                    0,  // srdOffset
                    0,  // srdEnable
                    0,  // srdIrqEnable
                    0,  // autoRecoveryWaitTime
                    0,  // autoRecoveryEnable
                    0,  // autoRecoveryDisableTimerWhenTxOff
                    0,  // srdStatisticsEnable
                    0,  // srdStatisticsMode
                },
                { // protectionRampCfg
                    0x00,  // rampDownMask
                    0,  // altEventClearReqd
                },
            }, // txChannelCfg[2]
            {  // txChannelCfg[3]
                { // txAttenCfg
                    { // updateCfg
                        { // srcCfg
                            ADI_ADRV903X_TXATTEN_UPD_SRC_S0,  // updateSrc
                            ADI_ADRV903X_GPIO_INVALID,  // s0OrS1Gpio
                        },
                        { // trgCfg
                            ADI_ADRV903X_TXATTEN_UPD_TRG_NONE,  // updateTrg
                            ADI_ADRV903X_GPIO_INVALID,  // triggerGpio
                        },
                    },
                    ADI_ADRV903X_TXATTEN_0P05_DB,  // txAttenStepSize
                },
                0,  // txAttenInit_mdB
                { // txpowerMonitorCfg
                    ADI_ADRV903X_HB1_OUTPUT,  // inputSel
                    0,  // peakThreshold
                    0,  // measDuration
                    0,  // peakCount
                    0,  // peakErrorClearRequired
                    0,  // peakPowerEnable
                    0,  // peakPowerIrqEnable
                    0,  // avgThreshold
                    0,  // avgErrorClearRequired
                    0,  // avgPowerEnable
                    0,  // avgPowerIrqEnable
                    0,  // avgPeakRatioEnable
                },
                { // srlCfg
                    ADI_ADRV903X_HB1_OUTPUT,  // inputSel
                    0,  // srdOffset
                    0,  // srdEnable
                    0,  // srdIrqEnable
                    0,  // autoRecoveryWaitTime
                    0,  // autoRecoveryEnable
                    0,  // autoRecoveryDisableTimerWhenTxOff
                    0,  // srdStatisticsEnable
                    0,  // srdStatisticsMode
                },
                { // protectionRampCfg
                    0x00,  // rampDownMask
                    0,  // altEventClearReqd
                },
            }, // txChannelCfg[3]
            {  // txChannelCfg[4]
                { // txAttenCfg
                    { // updateCfg
                        { // srcCfg
                            ADI_ADRV903X_TXATTEN_UPD_SRC_S0,  // updateSrc
                            ADI_ADRV903X_GPIO_INVALID,  // s0OrS1Gpio
                        },
                        { // trgCfg
                            ADI_ADRV903X_TXATTEN_UPD_TRG_NONE,  // updateTrg
                            ADI_ADRV903X_GPIO_INVALID,  // triggerGpio
                        },
                    },
                    ADI_ADRV903X_TXATTEN_0P05_DB,  // txAttenStepSize
                },
                0,  // txAttenInit_mdB
                { // txpowerMonitorCfg
                    ADI_ADRV903X_HB1_OUTPUT,  // inputSel
                    0,  // peakThreshold
                    0,  // measDuration
                    0,  // peakCount
                    0,  // peakErrorClearRequired
                    0,  // peakPowerEnable
                    0,  // peakPowerIrqEnable
                    0,  // avgThreshold
                    0,  // avgErrorClearRequired
                    0,  // avgPowerEnable
                    0,  // avgPowerIrqEnable
                    0,  // avgPeakRatioEnable
                },
                { // srlCfg
                    ADI_ADRV903X_HB1_OUTPUT,  // inputSel
                    0,  // srdOffset
                    0,  // srdEnable
                    0,  // srdIrqEnable
                    0,  // autoRecoveryWaitTime
                    0,  // autoRecoveryEnable
                    0,  // autoRecoveryDisableTimerWhenTxOff
                    0,  // srdStatisticsEnable
                    0,  // srdStatisticsMode
                },
                { // protectionRampCfg
                    0x00,  // rampDownMask
                    0,  // altEventClearReqd
                },
            }, // txChannelCfg[4]
            {  // txChannelCfg[5]
                { // txAttenCfg
                    { // updateCfg
                        { // srcCfg
                            ADI_ADRV903X_TXATTEN_UPD_SRC_S0,  // updateSrc
                            ADI_ADRV903X_GPIO_INVALID,  // s0OrS1Gpio
                        },
                        { // trgCfg
                            ADI_ADRV903X_TXATTEN_UPD_TRG_NONE,  // updateTrg
                            ADI_ADRV903X_GPIO_INVALID,  // triggerGpio
                        },
                    },
                    ADI_ADRV903X_TXATTEN_0P05_DB,  // txAttenStepSize
                },
                0,  // txAttenInit_mdB
                { // txpowerMonitorCfg
                    ADI_ADRV903X_HB1_OUTPUT,  // inputSel
                    0,  // peakThreshold
                    0,  // measDuration
                    0,  // peakCount
                    0,  // peakErrorClearRequired
                    0,  // peakPowerEnable
                    0,  // peakPowerIrqEnable
                    0,  // avgThreshold
                    0,  // avgErrorClearRequired
                    0,  // avgPowerEnable
                    0,  // avgPowerIrqEnable
                    0,  // avgPeakRatioEnable
                },
                { // srlCfg
                    ADI_ADRV903X_HB1_OUTPUT,  // inputSel
                    0,  // srdOffset
                    0,  // srdEnable
                    0,  // srdIrqEnable
                    0,  // autoRecoveryWaitTime
                    0,  // autoRecoveryEnable
                    0,  // autoRecoveryDisableTimerWhenTxOff
                    0,  // srdStatisticsEnable
                    0,  // srdStatisticsMode
                },
                { // protectionRampCfg
                    0x00,  // rampDownMask
                    0,  // altEventClearReqd
                },
            }, // txChannelCfg[5]
            {  // txChannelCfg[6]
                { // txAttenCfg
                    { // updateCfg
                        { // srcCfg
                            ADI_ADRV903X_TXATTEN_UPD_SRC_S0,  // updateSrc
                            ADI_ADRV903X_GPIO_INVALID,  // s0OrS1Gpio
                        },
                        { // trgCfg
                            ADI_ADRV903X_TXATTEN_UPD_TRG_NONE,  // updateTrg
                            ADI_ADRV903X_GPIO_INVALID,  // triggerGpio
                        },
                    },
                    ADI_ADRV903X_TXATTEN_0P05_DB,  // txAttenStepSize
                },
                0,  // txAttenInit_mdB
                { // txpowerMonitorCfg
                    ADI_ADRV903X_HB1_OUTPUT,  // inputSel
                    0,  // peakThreshold
                    0,  // measDuration
                    0,  // peakCount
                    0,  // peakErrorClearRequired
                    0,  // peakPowerEnable
                    0,  // peakPowerIrqEnable
                    0,  // avgThreshold
                    0,  // avgErrorClearRequired
                    0,  // avgPowerEnable
                    0,  // avgPowerIrqEnable
                    0,  // avgPeakRatioEnable
                },
                { // srlCfg
                    ADI_ADRV903X_HB1_OUTPUT,  // inputSel
                    0,  // srdOffset
                    0,  // srdEnable
                    0,  // srdIrqEnable
                    0,  // autoRecoveryWaitTime
                    0,  // autoRecoveryEnable
                    0,  // autoRecoveryDisableTimerWhenTxOff
                    0,  // srdStatisticsEnable
                    0,  // srdStatisticsMode
                },
                { // protectionRampCfg
                    0x00,  // rampDownMask
                    0,  // altEventClearReqd
                },
            }, // txChannelCfg[6]
            {  // txChannelCfg[7]
                { // txAttenCfg
                    { // updateCfg
                        { // srcCfg
                            ADI_ADRV903X_TXATTEN_UPD_SRC_S0,  // updateSrc
                            ADI_ADRV903X_GPIO_INVALID,  // s0OrS1Gpio
                        },
                        { // trgCfg
                            ADI_ADRV903X_TXATTEN_UPD_TRG_NONE,  // updateTrg
                            ADI_ADRV903X_GPIO_INVALID,  // triggerGpio
                        },
                    },
                    ADI_ADRV903X_TXATTEN_0P05_DB,  // txAttenStepSize
                },
                0,  // txAttenInit_mdB
                { // txpowerMonitorCfg
                    ADI_ADRV903X_HB1_OUTPUT,  // inputSel
                    0,  // peakThreshold
                    0,  // measDuration
                    0,  // peakCount
                    0,  // peakErrorClearRequired
                    0,  // peakPowerEnable
                    0,  // peakPowerIrqEnable
                    0,  // avgThreshold
                    0,  // avgErrorClearRequired
                    0,  // avgPowerEnable
                    0,  // avgPowerIrqEnable
                    0,  // avgPeakRatioEnable
                },
                { // srlCfg
                    ADI_ADRV903X_HB1_OUTPUT,  // inputSel
                    0,  // srdOffset
                    0,  // srdEnable
                    0,  // srdIrqEnable
                    0,  // autoRecoveryWaitTime
                    0,  // autoRecoveryEnable
                    0,  // autoRecoveryDisableTimerWhenTxOff
                    0,  // srdStatisticsEnable
                    0,  // srdStatisticsMode
                },
                { // protectionRampCfg
                    0x00,  // rampDownMask
                    0,  // altEventClearReqd
                },
            }, // txChannelCfg[7]
        }, // txChannelCfg (end of array)
    },
    { // uart (array)
        {  // uart[0]
            0,  // enable
            ADI_ADRV903X_GPIO_09,  // pinSelect
        }, // uart[0]
        {  // uart[1]
            0,  // enable
            ADI_ADRV903X_GPIO_10,  // pinSelect
        }, // uart[1]
        {  // uart[2]
            0,  // enable
            ADI_ADRV903X_GPIO_11,  // pinSelect
        }, // uart[2]
        {  // uart[3]
            0,  // enable
            ADI_ADRV903X_GPIO_12,  // pinSelect
        }, // uart[3]
    }, // uart (end of array)
    { // gpIntPreInit
        { // gpInt0Mask
            0xFFFFFFFFFFFF,  // lowerMask
            0xFFFFFFFFFFFF,  // upperMask
        },
        { // gpInt1Mask
            0xFFFFFFFFFFFF,  // lowerMask
            0xFFFFFFFFFFFF,  // upperMask
        },
    },
};
adi_adrv903x_PostMcsInit_t utilityInit = 
{
    { // radioCtrlCfg
        { // txRadioCtrlModeCfg
            ADI_ADRV903X_TX_EN_SPI_MODE,  // txEnableMode
            0xFF,  // txChannelMask
        },
        { // rxRadioCtrlModeCfg
            ADI_ADRV903X_RX_EN_SPI_MODE,  // rxEnableMode
            0xFF,  // rxChannelMask
        },
        { // orxRadioCtrlModeCfg
            ADI_ADRV903X_ORX_EN_SPI_MODE,  // orxEnableMode
            0x300,  // orxChannelMask
        },
    },
    { // radioCtrlGpioCfg
        { // txEnMapping (array)
            0, 0, 0, 0, 0, 0, 0, 0,
        }, // txEnMapping (end of array)
        { // txAltMapping (array)
            0, 0, 0, 0, 0, 0, 0, 0,
        }, // txAltMapping (end of array)
        { // rxEnMapping (array)
            0, 0, 0, 0, 0, 0, 0, 0,
        }, // rxEnMapping (end of array)
        { // rxAltMapping (array)
            0, 0, 0, 0, 0, 0, 0, 0,
        }, // rxAltMapping (end of array)
    },
    0,  // radioCtrlTxRxEnPinSel
    0,  // radioCtrlTxRxEnCfgSel
    { // gpIntPostInit
        { // gpInt0Mask
            0xFFFFFFFFFFFF,  // lowerMask
            0xFFFFFFFFFFFF,  // upperMask
        },
        { // gpInt1Mask
            0xFFFFFFFFFFFF,  // lowerMask
            0xFFFFFFFFFFFF,  // upperMask
        },
    },
    { // initCals
        0x7E,  // calMask
        0xFF,  // rxChannelMask
        0xFF,  // txChannelMask
        0x03,  // orxChannelMask
        0,  // warmBoot
    },
};
