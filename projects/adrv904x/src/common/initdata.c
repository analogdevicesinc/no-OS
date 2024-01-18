#include "initdata.h"

adi_adrv904x_Version_t initStructApiVersion = {2, 10, 0, 4};

adi_adrv904x_CpuFwVersion_t initStructArmVersion = { {2, 10, 0, 4} , ADI_ADRV904X_CPU_FW_BUILD_RELEASE};

adi_adrv904x_Version_t initStructStreamVersion = {2, 10, 0, 4};

adi_adrv904x_Init_t deviceInitStruct = 
{
    .spiOptionsInit =
    {
        .allowSpiStreaming = 1,
        .allowAhbAutoIncrement = 1,
        .allowAhbSpiFifoMode = 1,
    },
    .clocks =
    {
        .DevClkOnChipTermResEn = 0,
    },
    .cpuMemDump =
    {
        .filePath =
        {
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
    .rx =
    {
        .rxChannelCfg =
        {
            {  // rxChannelCfg[0]
                .rxDataFormat =
                {
                    .formatSelect = ADI_ADRV904X_GAIN_COMPENSATION_DISABLED,
                    .floatingPointConfig =
                    {
                        .fpDataFormat = ADI_ADRV904X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,
                        .fpRoundMode = ADI_ADRV904X_ROUND_TO_EVEN,
                        .fpNumExpBits = ADI_ADRV904X_2_EXPONENTBITS,
                        .fpAttenSteps = ADI_ADRV904X_FPATTEN_0DB,
                        .fpHideLeadingOne = ADI_ADRV904X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,
                        .fpEncodeNan = ADI_ADRV904X_FP_FORMAT_NAN_ENCODE_DISABLE,
                    },
                    .integerConfigSettings =
                    {
                        .intEmbeddedBits = ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS,
                        .intSampleResolution = ADI_ADRV904X_INTEGER_16BIT_2SCOMP,
                        .intParity = ADI_ADRV904X_NO_PARITY,
                        .intEmbeddedPos = ADI_ADRV904X_LOWER_NIBBLE_ON_I,
                    },
                    .slicerConfigSettings =
                    {
                        .extSlicerStepSize = ADI_ADRV904X_EXTSLICER_STEPSIZE_1DB,
                        .intSlicerStepSize = ADI_ADRV904X_INTSLICER_STEPSIZE_1DB,
                        .extSlicerGpioSelect = ADI_ADRV904X_EXTSLICER_RX_GPIO_DISABLE,
                        .intSlicerGpioSelect =
                        {
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    .embOvldMonitorSettings =
                    {
                        .embeddedMonitorSrcLsbI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorHb2LowSrcSel = ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW,
                        .embeddedMonitorHb2HighSrcSel = ADI_ADRV904X_HB2_HIGH_SRC_OVRG_HIGH,
                        .embeddedMonitorApdLowSrcSel = ADI_ADRV904X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,
                        .embeddedMonitorApdHighSrcSel = ADI_ADRV904X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,
                        .invertHb2Flag = 0,
                        .invertApdFlag = 0,
                    },
                    .externalLnaGain = 0,
                    .tempCompensationEnable = 0,
                },
                .rxGainIndexInit = 255,
            }, // rxChannelCfg[0]
            {  // rxChannelCfg[1]
                .rxDataFormat =
                {
                    .formatSelect = ADI_ADRV904X_GAIN_COMPENSATION_DISABLED,
                    .floatingPointConfig =
                    {
                        .fpDataFormat = ADI_ADRV904X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,
                        .fpRoundMode = ADI_ADRV904X_ROUND_TO_EVEN,
                        .fpNumExpBits = ADI_ADRV904X_2_EXPONENTBITS,
                        .fpAttenSteps = ADI_ADRV904X_FPATTEN_0DB,
                        .fpHideLeadingOne = ADI_ADRV904X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,
                        .fpEncodeNan = ADI_ADRV904X_FP_FORMAT_NAN_ENCODE_DISABLE,
                    },
                    .integerConfigSettings =
                    {
                        .intEmbeddedBits = ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS,
                        .intSampleResolution = ADI_ADRV904X_INTEGER_16BIT_2SCOMP,
                        .intParity = ADI_ADRV904X_NO_PARITY,
                        .intEmbeddedPos = ADI_ADRV904X_LOWER_NIBBLE_ON_I,
                    },
                    .slicerConfigSettings =
                    {
                        .extSlicerStepSize = ADI_ADRV904X_EXTSLICER_STEPSIZE_1DB,
                        .intSlicerStepSize = ADI_ADRV904X_INTSLICER_STEPSIZE_1DB,
                        .extSlicerGpioSelect = ADI_ADRV904X_EXTSLICER_RX_GPIO_DISABLE,
                        .intSlicerGpioSelect =
                        {
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    .embOvldMonitorSettings =
                    {
                        .embeddedMonitorSrcLsbI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorHb2LowSrcSel = ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW,
                        .embeddedMonitorHb2HighSrcSel = ADI_ADRV904X_HB2_HIGH_SRC_OVRG_HIGH,
                        .embeddedMonitorApdLowSrcSel = ADI_ADRV904X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,
                        .embeddedMonitorApdHighSrcSel = ADI_ADRV904X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,
                        .invertHb2Flag = 0,
                        .invertApdFlag = 0,
                    },
                    .externalLnaGain = 0,
                    .tempCompensationEnable = 0,
                },
                .rxGainIndexInit = 255,
            }, // rxChannelCfg[1]
            {  // rxChannelCfg[2]
                .rxDataFormat =
                {
                    .formatSelect = ADI_ADRV904X_GAIN_COMPENSATION_DISABLED,
                    .floatingPointConfig =
                    {
                        .fpDataFormat = ADI_ADRV904X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,
                        .fpRoundMode = ADI_ADRV904X_ROUND_TO_EVEN,
                        .fpNumExpBits = ADI_ADRV904X_2_EXPONENTBITS,
                        .fpAttenSteps = ADI_ADRV904X_FPATTEN_0DB,
                        .fpHideLeadingOne = ADI_ADRV904X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,
                        .fpEncodeNan = ADI_ADRV904X_FP_FORMAT_NAN_ENCODE_DISABLE,
                    },
                    .integerConfigSettings =
                    {
                        .intEmbeddedBits = ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS,
                        .intSampleResolution = ADI_ADRV904X_INTEGER_16BIT_2SCOMP,
                        .intParity = ADI_ADRV904X_NO_PARITY,
                        .intEmbeddedPos = ADI_ADRV904X_LOWER_NIBBLE_ON_I,
                    },
                    .slicerConfigSettings =
                    {
                        .extSlicerStepSize = ADI_ADRV904X_EXTSLICER_STEPSIZE_1DB,
                        .intSlicerStepSize = ADI_ADRV904X_INTSLICER_STEPSIZE_1DB,
                        .extSlicerGpioSelect = ADI_ADRV904X_EXTSLICER_RX_GPIO_DISABLE,
                        .intSlicerGpioSelect =
                        {
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    .embOvldMonitorSettings =
                    {
                        .embeddedMonitorSrcLsbI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorHb2LowSrcSel = ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW,
                        .embeddedMonitorHb2HighSrcSel = ADI_ADRV904X_HB2_HIGH_SRC_OVRG_HIGH,
                        .embeddedMonitorApdLowSrcSel = ADI_ADRV904X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,
                        .embeddedMonitorApdHighSrcSel = ADI_ADRV904X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,
                        .invertHb2Flag = 0,
                        .invertApdFlag = 0,
                    },
                    .externalLnaGain = 0,
                    .tempCompensationEnable = 0,
                },
                .rxGainIndexInit = 255,
            }, // rxChannelCfg[2]
            {  // rxChannelCfg[3]
                .rxDataFormat =
                {
                    .formatSelect = ADI_ADRV904X_GAIN_COMPENSATION_DISABLED,
                    .floatingPointConfig =
                    {
                        .fpDataFormat = ADI_ADRV904X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,
                        .fpRoundMode = ADI_ADRV904X_ROUND_TO_EVEN,
                        .fpNumExpBits = ADI_ADRV904X_2_EXPONENTBITS,
                        .fpAttenSteps = ADI_ADRV904X_FPATTEN_0DB,
                        .fpHideLeadingOne = ADI_ADRV904X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,
                        .fpEncodeNan = ADI_ADRV904X_FP_FORMAT_NAN_ENCODE_DISABLE,
                    },
                    .integerConfigSettings =
                    {
                        .intEmbeddedBits = ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS,
                        .intSampleResolution = ADI_ADRV904X_INTEGER_16BIT_2SCOMP,
                        .intParity = ADI_ADRV904X_NO_PARITY,
                        .intEmbeddedPos = ADI_ADRV904X_LOWER_NIBBLE_ON_I,
                    },
                    .slicerConfigSettings =
                    {
                        .extSlicerStepSize = ADI_ADRV904X_EXTSLICER_STEPSIZE_1DB,
                        .intSlicerStepSize = ADI_ADRV904X_INTSLICER_STEPSIZE_1DB,
                        .extSlicerGpioSelect = ADI_ADRV904X_EXTSLICER_RX_GPIO_DISABLE,
                        .intSlicerGpioSelect =
                        {
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    .embOvldMonitorSettings =
                    {
                        .embeddedMonitorSrcLsbI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorHb2LowSrcSel = ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW,
                        .embeddedMonitorHb2HighSrcSel = ADI_ADRV904X_HB2_HIGH_SRC_OVRG_HIGH,
                        .embeddedMonitorApdLowSrcSel = ADI_ADRV904X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,
                        .embeddedMonitorApdHighSrcSel = ADI_ADRV904X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,
                        .invertHb2Flag = 0,
                        .invertApdFlag = 0,
                    },
                    .externalLnaGain = 0,
                    .tempCompensationEnable = 0,
                },
                .rxGainIndexInit = 255,
            }, // rxChannelCfg[3]
            {  // rxChannelCfg[4]
                .rxDataFormat =
                {
                    .formatSelect = ADI_ADRV904X_GAIN_COMPENSATION_DISABLED,
                    .floatingPointConfig =
                    {
                        .fpDataFormat = ADI_ADRV904X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,
                        .fpRoundMode = ADI_ADRV904X_ROUND_TO_EVEN,
                        .fpNumExpBits = ADI_ADRV904X_2_EXPONENTBITS,
                        .fpAttenSteps = ADI_ADRV904X_FPATTEN_0DB,
                        .fpHideLeadingOne = ADI_ADRV904X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,
                        .fpEncodeNan = ADI_ADRV904X_FP_FORMAT_NAN_ENCODE_DISABLE,
                    },
                    .integerConfigSettings =
                    {
                        .intEmbeddedBits = ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS,
                        .intSampleResolution = ADI_ADRV904X_INTEGER_16BIT_2SCOMP,
                        .intParity = ADI_ADRV904X_NO_PARITY,
                        .intEmbeddedPos = ADI_ADRV904X_LOWER_NIBBLE_ON_I,
                    },
                    .slicerConfigSettings =
                    {
                        .extSlicerStepSize = ADI_ADRV904X_EXTSLICER_STEPSIZE_1DB,
                        .intSlicerStepSize = ADI_ADRV904X_INTSLICER_STEPSIZE_1DB,
                        .extSlicerGpioSelect = ADI_ADRV904X_EXTSLICER_RX_GPIO_DISABLE,
                        .intSlicerGpioSelect =
                        {
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    .embOvldMonitorSettings =
                    {
                        .embeddedMonitorSrcLsbI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorHb2LowSrcSel = ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW,
                        .embeddedMonitorHb2HighSrcSel = ADI_ADRV904X_HB2_HIGH_SRC_OVRG_HIGH,
                        .embeddedMonitorApdLowSrcSel = ADI_ADRV904X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,
                        .embeddedMonitorApdHighSrcSel = ADI_ADRV904X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,
                        .invertHb2Flag = 0,
                        .invertApdFlag = 0,
                    },
                    .externalLnaGain = 0,
                    .tempCompensationEnable = 0,
                },
                .rxGainIndexInit = 255,
            }, // rxChannelCfg[4]
            {  // rxChannelCfg[5]
                .rxDataFormat =
                {
                    .formatSelect = ADI_ADRV904X_GAIN_COMPENSATION_DISABLED,
                    .floatingPointConfig =
                    {
                        .fpDataFormat = ADI_ADRV904X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,
                        .fpRoundMode = ADI_ADRV904X_ROUND_TO_EVEN,
                        .fpNumExpBits = ADI_ADRV904X_2_EXPONENTBITS,
                        .fpAttenSteps = ADI_ADRV904X_FPATTEN_0DB,
                        .fpHideLeadingOne = ADI_ADRV904X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,
                        .fpEncodeNan = ADI_ADRV904X_FP_FORMAT_NAN_ENCODE_DISABLE,
                    },
                    .integerConfigSettings =
                    {
                        .intEmbeddedBits = ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS,
                        .intSampleResolution = ADI_ADRV904X_INTEGER_16BIT_2SCOMP,
                        .intParity = ADI_ADRV904X_NO_PARITY,
                        .intEmbeddedPos = ADI_ADRV904X_LOWER_NIBBLE_ON_I,
                    },
                    .slicerConfigSettings =
                    {
                        .extSlicerStepSize = ADI_ADRV904X_EXTSLICER_STEPSIZE_1DB,
                        .intSlicerStepSize = ADI_ADRV904X_INTSLICER_STEPSIZE_1DB,
                        .extSlicerGpioSelect = ADI_ADRV904X_EXTSLICER_RX_GPIO_DISABLE,
                        .intSlicerGpioSelect =
                        {
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    .embOvldMonitorSettings =
                    {
                        .embeddedMonitorSrcLsbI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorHb2LowSrcSel = ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW,
                        .embeddedMonitorHb2HighSrcSel = ADI_ADRV904X_HB2_HIGH_SRC_OVRG_HIGH,
                        .embeddedMonitorApdLowSrcSel = ADI_ADRV904X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,
                        .embeddedMonitorApdHighSrcSel = ADI_ADRV904X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,
                        .invertHb2Flag = 0,
                        .invertApdFlag = 0,
                    },
                    .externalLnaGain = 0,
                    .tempCompensationEnable = 0,
                },
                .rxGainIndexInit = 255,
            }, // rxChannelCfg[5]
            {  // rxChannelCfg[6]
                .rxDataFormat =
                {
                    .formatSelect = ADI_ADRV904X_GAIN_COMPENSATION_DISABLED,
                    .floatingPointConfig =
                    {
                        .fpDataFormat = ADI_ADRV904X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,
                        .fpRoundMode = ADI_ADRV904X_ROUND_TO_EVEN,
                        .fpNumExpBits = ADI_ADRV904X_2_EXPONENTBITS,
                        .fpAttenSteps = ADI_ADRV904X_FPATTEN_0DB,
                        .fpHideLeadingOne = ADI_ADRV904X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,
                        .fpEncodeNan = ADI_ADRV904X_FP_FORMAT_NAN_ENCODE_DISABLE,
                    },
                    .integerConfigSettings =
                    {
                        .intEmbeddedBits = ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS,
                        .intSampleResolution = ADI_ADRV904X_INTEGER_16BIT_2SCOMP,
                        .intParity = ADI_ADRV904X_NO_PARITY,
                        .intEmbeddedPos = ADI_ADRV904X_LOWER_NIBBLE_ON_I,
                    },
                    .slicerConfigSettings =
                    {
                        .extSlicerStepSize = ADI_ADRV904X_EXTSLICER_STEPSIZE_1DB,
                        .intSlicerStepSize = ADI_ADRV904X_INTSLICER_STEPSIZE_1DB,
                        .extSlicerGpioSelect = ADI_ADRV904X_EXTSLICER_RX_GPIO_DISABLE,
                        .intSlicerGpioSelect =
                        {
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    .embOvldMonitorSettings =
                    {
                        .embeddedMonitorSrcLsbI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorHb2LowSrcSel = ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW,
                        .embeddedMonitorHb2HighSrcSel = ADI_ADRV904X_HB2_HIGH_SRC_OVRG_HIGH,
                        .embeddedMonitorApdLowSrcSel = ADI_ADRV904X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,
                        .embeddedMonitorApdHighSrcSel = ADI_ADRV904X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,
                        .invertHb2Flag = 0,
                        .invertApdFlag = 0,
                    },
                    .externalLnaGain = 0,
                    .tempCompensationEnable = 0,
                },
                .rxGainIndexInit = 255,
            }, // rxChannelCfg[6]
            {  // rxChannelCfg[7]
                .rxDataFormat =
                {
                    .formatSelect = ADI_ADRV904X_GAIN_COMPENSATION_DISABLED,
                    .floatingPointConfig =
                    {
                        .fpDataFormat = ADI_ADRV904X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,
                        .fpRoundMode = ADI_ADRV904X_ROUND_TO_EVEN,
                        .fpNumExpBits = ADI_ADRV904X_2_EXPONENTBITS,
                        .fpAttenSteps = ADI_ADRV904X_FPATTEN_0DB,
                        .fpHideLeadingOne = ADI_ADRV904X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,
                        .fpEncodeNan = ADI_ADRV904X_FP_FORMAT_NAN_ENCODE_DISABLE,
                    },
                    .integerConfigSettings =
                    {
                        .intEmbeddedBits = ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS,
                        .intSampleResolution = ADI_ADRV904X_INTEGER_16BIT_2SCOMP,
                        .intParity = ADI_ADRV904X_NO_PARITY,
                        .intEmbeddedPos = ADI_ADRV904X_LOWER_NIBBLE_ON_I,
                    },
                    .slicerConfigSettings =
                    {
                        .extSlicerStepSize = ADI_ADRV904X_EXTSLICER_STEPSIZE_1DB,
                        .intSlicerStepSize = ADI_ADRV904X_INTSLICER_STEPSIZE_1DB,
                        .extSlicerGpioSelect = ADI_ADRV904X_EXTSLICER_RX_GPIO_DISABLE,
                        .intSlicerGpioSelect =
                        {
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    .embOvldMonitorSettings =
                    {
                        .embeddedMonitorSrcLsbI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorHb2LowSrcSel = ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW,
                        .embeddedMonitorHb2HighSrcSel = ADI_ADRV904X_HB2_HIGH_SRC_OVRG_HIGH,
                        .embeddedMonitorApdLowSrcSel = ADI_ADRV904X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,
                        .embeddedMonitorApdHighSrcSel = ADI_ADRV904X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,
                        .invertHb2Flag = 0,
                        .invertApdFlag = 0,
                    },
                    .externalLnaGain = 0,
                    .tempCompensationEnable = 0,
                },
                .rxGainIndexInit = 255,
            }, // rxChannelCfg[7]
            {  // rxChannelCfg[8]
                .rxDataFormat =
                {
                    .formatSelect = ADI_ADRV904X_GAIN_COMPENSATION_DISABLED,
                    .floatingPointConfig =
                    {
                        .fpDataFormat = ADI_ADRV904X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,
                        .fpRoundMode = ADI_ADRV904X_ROUND_TO_EVEN,
                        .fpNumExpBits = ADI_ADRV904X_2_EXPONENTBITS,
                        .fpAttenSteps = ADI_ADRV904X_FPATTEN_0DB,
                        .fpHideLeadingOne = ADI_ADRV904X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,
                        .fpEncodeNan = ADI_ADRV904X_FP_FORMAT_NAN_ENCODE_DISABLE,
                    },
                    .integerConfigSettings =
                    {
                        .intEmbeddedBits = ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS,
                        .intSampleResolution = ADI_ADRV904X_INTEGER_16BIT_2SCOMP,
                        .intParity = ADI_ADRV904X_NO_PARITY,
                        .intEmbeddedPos = ADI_ADRV904X_LOWER_NIBBLE_ON_I,
                    },
                    .slicerConfigSettings =
                    {
                        .extSlicerStepSize = ADI_ADRV904X_EXTSLICER_STEPSIZE_1DB,
                        .intSlicerStepSize = ADI_ADRV904X_INTSLICER_STEPSIZE_1DB,
                        .extSlicerGpioSelect = ADI_ADRV904X_EXTSLICER_RX_GPIO_DISABLE,
                        .intSlicerGpioSelect =
                        {
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    .embOvldMonitorSettings =
                    {
                        .embeddedMonitorSrcLsbI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorHb2LowSrcSel = ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW,
                        .embeddedMonitorHb2HighSrcSel = ADI_ADRV904X_HB2_HIGH_SRC_OVRG_HIGH,
                        .embeddedMonitorApdLowSrcSel = ADI_ADRV904X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,
                        .embeddedMonitorApdHighSrcSel = ADI_ADRV904X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,
                        .invertHb2Flag = 0,
                        .invertApdFlag = 0,
                    },
                    .externalLnaGain = 0,
                    .tempCompensationEnable = 0,
                },
                .rxGainIndexInit = 255,
            }, // rxChannelCfg[8]
            {  // rxChannelCfg[9]
                .rxDataFormat =
                {
                    .formatSelect = ADI_ADRV904X_GAIN_COMPENSATION_DISABLED,
                    .floatingPointConfig =
                    {
                        .fpDataFormat = ADI_ADRV904X_FP_FORMAT_SIGN_EXP_SIGNIFICAND,
                        .fpRoundMode = ADI_ADRV904X_ROUND_TO_EVEN,
                        .fpNumExpBits = ADI_ADRV904X_2_EXPONENTBITS,
                        .fpAttenSteps = ADI_ADRV904X_FPATTEN_0DB,
                        .fpHideLeadingOne = ADI_ADRV904X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE,
                        .fpEncodeNan = ADI_ADRV904X_FP_FORMAT_NAN_ENCODE_DISABLE,
                    },
                    .integerConfigSettings =
                    {
                        .intEmbeddedBits = ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS,
                        .intSampleResolution = ADI_ADRV904X_INTEGER_16BIT_2SCOMP,
                        .intParity = ADI_ADRV904X_NO_PARITY,
                        .intEmbeddedPos = ADI_ADRV904X_LOWER_NIBBLE_ON_I,
                    },
                    .slicerConfigSettings =
                    {
                        .extSlicerStepSize = ADI_ADRV904X_EXTSLICER_STEPSIZE_1DB,
                        .intSlicerStepSize = ADI_ADRV904X_INTSLICER_STEPSIZE_1DB,
                        .extSlicerGpioSelect = ADI_ADRV904X_EXTSLICER_RX_GPIO_DISABLE,
                        .intSlicerGpioSelect =
                        {
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[0]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[1]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[2]
                            ADI_ADRV904X_GPIO_INVALID,  // intSlicerGpioSelect[3]
                        }, // intSlicerGpioSelect (end of array)
                    },
                    .embOvldMonitorSettings =
                    {
                        .embeddedMonitorSrcLsbI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneI = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorSrcLsbPlusOneQ = ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                        .embeddedMonitorHb2LowSrcSel = ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW,
                        .embeddedMonitorHb2HighSrcSel = ADI_ADRV904X_HB2_HIGH_SRC_OVRG_HIGH,
                        .embeddedMonitorApdLowSrcSel = ADI_ADRV904X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,
                        .embeddedMonitorApdHighSrcSel = ADI_ADRV904X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,
                        .invertHb2Flag = 0,
                        .invertApdFlag = 0,
                    },
                    .externalLnaGain = 0,
                    .tempCompensationEnable = 0,
                },
                .rxGainIndexInit = 255,
            }, // rxChannelCfg[9]
        }, // rxChannelCfg (end of array)
    },
    .tx =
    {
        .txChannelCfg =
        {
            {  // txChannelCfg[0]
                .txAttenCfg =
                {
                    .updateCfg =
                    {
                        .srcCfg =
                        {
                            .updateSrc = ADI_ADRV904X_TXATTEN_UPD_SRC_S0,
                            .s0OrS1Gpio = ADI_ADRV904X_GPIO_INVALID,
                        },
                        .trgCfg =
                        {
                            .updateTrg = ADI_ADRV904X_TXATTEN_UPD_TRG_NONE,
                            .triggerGpio = ADI_ADRV904X_GPIO_INVALID,
                        },
                    },
                    .txAttenStepSize = ADI_ADRV904X_TXATTEN_0P05_DB,
                },
                .txAttenInit_mdB = 0,
                .txpowerMonitorCfg =
                {
                    .inputSel = ADI_ADRV904X_POST_DPD_HC_OUTPUT,
                    .peakThreshold = 0,
                    .measDuration = 0,
                    .peakCount = 0,
                    .peakErrorClearRequired = 0,
                    .peakPowerEnable = 0,
                    .peakPowerIrqEnable = 0,
                    .avgThreshold = 0,
                    .avgErrorClearRequired = 0,
                    .avgPowerEnable = 0,
                    .avgPowerIrqEnable = 0,
                    .avgPeakRatioEnable = 0,
                },
                .srlCfg =
                {
                    .inputSel = ADI_ADRV904X_POST_DPD_HC_OUTPUT,
                    .srdOffset = 0,
                    .srdEnable = 0,
                    .srdIrqEnable = 0,
                    .autoRecoveryWaitTime = 0,
                    .autoRecoveryEnable = 0,
                    .autoRecoveryDisableTimerWhenTxOff = 0,
                    .srdStatisticsEnable = 0,
                    .srdStatisticsMode = 0,
                },
                .protectionRampCfg =
                {
                    .rampDownMask = 0x00,
                    .altEventClearReqd = 0,
                },
            }, // txChannelCfg[0]
            {  // txChannelCfg[1]
                .txAttenCfg =
                {
                    .updateCfg =
                    {
                        .srcCfg =
                        {
                            .updateSrc = ADI_ADRV904X_TXATTEN_UPD_SRC_S0,
                            .s0OrS1Gpio = ADI_ADRV904X_GPIO_INVALID,
                        },
                        .trgCfg =
                        {
                            .updateTrg = ADI_ADRV904X_TXATTEN_UPD_TRG_NONE,
                            .triggerGpio = ADI_ADRV904X_GPIO_INVALID,
                        },
                    },
                    .txAttenStepSize = ADI_ADRV904X_TXATTEN_0P05_DB,
                },
                .txAttenInit_mdB = 0,
                .txpowerMonitorCfg =
                {
                    .inputSel = ADI_ADRV904X_POST_DPD_HC_OUTPUT,
                    .peakThreshold = 0,
                    .measDuration = 0,
                    .peakCount = 0,
                    .peakErrorClearRequired = 0,
                    .peakPowerEnable = 0,
                    .peakPowerIrqEnable = 0,
                    .avgThreshold = 0,
                    .avgErrorClearRequired = 0,
                    .avgPowerEnable = 0,
                    .avgPowerIrqEnable = 0,
                    .avgPeakRatioEnable = 0,
                },
                .srlCfg =
                {
                    .inputSel = ADI_ADRV904X_POST_DPD_HC_OUTPUT,
                    .srdOffset = 0,
                    .srdEnable = 0,
                    .srdIrqEnable = 0,
                    .autoRecoveryWaitTime = 0,
                    .autoRecoveryEnable = 0,
                    .autoRecoveryDisableTimerWhenTxOff = 0,
                    .srdStatisticsEnable = 0,
                    .srdStatisticsMode = 0,
                },
                .protectionRampCfg =
                {
                    .rampDownMask = 0x00,
                    .altEventClearReqd = 0,
                },
            }, // txChannelCfg[1]
            {  // txChannelCfg[2]
                .txAttenCfg =
                {
                    .updateCfg =
                    {
                        .srcCfg =
                        {
                            .updateSrc = ADI_ADRV904X_TXATTEN_UPD_SRC_S0,
                            .s0OrS1Gpio = ADI_ADRV904X_GPIO_INVALID,
                        },
                        .trgCfg =
                        {
                            .updateTrg = ADI_ADRV904X_TXATTEN_UPD_TRG_NONE,
                            .triggerGpio = ADI_ADRV904X_GPIO_INVALID,
                        },
                    },
                    .txAttenStepSize = ADI_ADRV904X_TXATTEN_0P05_DB,
                },
                .txAttenInit_mdB = 0,
                .txpowerMonitorCfg =
                {
                    .inputSel = ADI_ADRV904X_POST_DPD_HC_OUTPUT,
                    .peakThreshold = 0,
                    .measDuration = 0,
                    .peakCount = 0,
                    .peakErrorClearRequired = 0,
                    .peakPowerEnable = 0,
                    .peakPowerIrqEnable = 0,
                    .avgThreshold = 0,
                    .avgErrorClearRequired = 0,
                    .avgPowerEnable = 0,
                    .avgPowerIrqEnable = 0,
                    .avgPeakRatioEnable = 0,
                },
                .srlCfg =
                {
                    .inputSel = ADI_ADRV904X_POST_DPD_HC_OUTPUT,
                    .srdOffset = 0,
                    .srdEnable = 0,
                    .srdIrqEnable = 0,
                    .autoRecoveryWaitTime = 0,
                    .autoRecoveryEnable = 0,
                    .autoRecoveryDisableTimerWhenTxOff = 0,
                    .srdStatisticsEnable = 0,
                    .srdStatisticsMode = 0,
                },
                .protectionRampCfg =
                {
                    .rampDownMask = 0x00,
                    .altEventClearReqd = 0,
                },
            }, // txChannelCfg[2]
            {  // txChannelCfg[3]
                .txAttenCfg =
                {
                    .updateCfg =
                    {
                        .srcCfg =
                        {
                            .updateSrc = ADI_ADRV904X_TXATTEN_UPD_SRC_S0,
                            .s0OrS1Gpio = ADI_ADRV904X_GPIO_INVALID,
                        },
                        .trgCfg =
                        {
                            .updateTrg = ADI_ADRV904X_TXATTEN_UPD_TRG_NONE,
                            .triggerGpio = ADI_ADRV904X_GPIO_INVALID,
                        },
                    },
                    .txAttenStepSize = ADI_ADRV904X_TXATTEN_0P05_DB,
                },
                .txAttenInit_mdB = 0,
                .txpowerMonitorCfg =
                {
                    .inputSel = ADI_ADRV904X_POST_DPD_HC_OUTPUT,
                    .peakThreshold = 0,
                    .measDuration = 0,
                    .peakCount = 0,
                    .peakErrorClearRequired = 0,
                    .peakPowerEnable = 0,
                    .peakPowerIrqEnable = 0,
                    .avgThreshold = 0,
                    .avgErrorClearRequired = 0,
                    .avgPowerEnable = 0,
                    .avgPowerIrqEnable = 0,
                    .avgPeakRatioEnable = 0,
                },
                .srlCfg =
                {
                    .inputSel = ADI_ADRV904X_POST_DPD_HC_OUTPUT,
                    .srdOffset = 0,
                    .srdEnable = 0,
                    .srdIrqEnable = 0,
                    .autoRecoveryWaitTime = 0,
                    .autoRecoveryEnable = 0,
                    .autoRecoveryDisableTimerWhenTxOff = 0,
                    .srdStatisticsEnable = 0,
                    .srdStatisticsMode = 0,
                },
                .protectionRampCfg =
                {
                    .rampDownMask = 0x00,
                    .altEventClearReqd = 0,
                },
            }, // txChannelCfg[3]
            {  // txChannelCfg[4]
                .txAttenCfg =
                {
                    .updateCfg =
                    {
                        .srcCfg =
                        {
                            .updateSrc = ADI_ADRV904X_TXATTEN_UPD_SRC_S0,
                            .s0OrS1Gpio = ADI_ADRV904X_GPIO_INVALID,
                        },
                        .trgCfg =
                        {
                            .updateTrg = ADI_ADRV904X_TXATTEN_UPD_TRG_NONE,
                            .triggerGpio = ADI_ADRV904X_GPIO_INVALID,
                        },
                    },
                    .txAttenStepSize = ADI_ADRV904X_TXATTEN_0P05_DB,
                },
                .txAttenInit_mdB = 0,
                .txpowerMonitorCfg =
                {
                    .inputSel = ADI_ADRV904X_POST_DPD_HC_OUTPUT,
                    .peakThreshold = 0,
                    .measDuration = 0,
                    .peakCount = 0,
                    .peakErrorClearRequired = 0,
                    .peakPowerEnable = 0,
                    .peakPowerIrqEnable = 0,
                    .avgThreshold = 0,
                    .avgErrorClearRequired = 0,
                    .avgPowerEnable = 0,
                    .avgPowerIrqEnable = 0,
                    .avgPeakRatioEnable = 0,
                },
                .srlCfg =
                {
                    .inputSel = ADI_ADRV904X_POST_DPD_HC_OUTPUT,
                    .srdOffset = 0,
                    .srdEnable = 0,
                    .srdIrqEnable = 0,
                    .autoRecoveryWaitTime = 0,
                    .autoRecoveryEnable = 0,
                    .autoRecoveryDisableTimerWhenTxOff = 0,
                    .srdStatisticsEnable = 0,
                    .srdStatisticsMode = 0,
                },
                .protectionRampCfg =
                {
                    .rampDownMask = 0x00,
                    .altEventClearReqd = 0,
                },
            }, // txChannelCfg[4]
            {  // txChannelCfg[5]
                .txAttenCfg =
                {
                    .updateCfg =
                    {
                        .srcCfg =
                        {
                            .updateSrc = ADI_ADRV904X_TXATTEN_UPD_SRC_S0,
                            .s0OrS1Gpio = ADI_ADRV904X_GPIO_INVALID,
                        },
                        .trgCfg =
                        {
                            .updateTrg = ADI_ADRV904X_TXATTEN_UPD_TRG_NONE,
                            .triggerGpio = ADI_ADRV904X_GPIO_INVALID,
                        },
                    },
                    .txAttenStepSize = ADI_ADRV904X_TXATTEN_0P05_DB,
                },
                .txAttenInit_mdB = 0,
                .txpowerMonitorCfg =
                {
                    .inputSel = ADI_ADRV904X_POST_DPD_HC_OUTPUT,
                    .peakThreshold = 0,
                    .measDuration = 0,
                    .peakCount = 0,
                    .peakErrorClearRequired = 0,
                    .peakPowerEnable = 0,
                    .peakPowerIrqEnable = 0,
                    .avgThreshold = 0,
                    .avgErrorClearRequired = 0,
                    .avgPowerEnable = 0,
                    .avgPowerIrqEnable = 0,
                    .avgPeakRatioEnable = 0,
                },
                .srlCfg =
                {
                    .inputSel = ADI_ADRV904X_POST_DPD_HC_OUTPUT,
                    .srdOffset = 0,
                    .srdEnable = 0,
                    .srdIrqEnable = 0,
                    .autoRecoveryWaitTime = 0,
                    .autoRecoveryEnable = 0,
                    .autoRecoveryDisableTimerWhenTxOff = 0,
                    .srdStatisticsEnable = 0,
                    .srdStatisticsMode = 0,
                },
                .protectionRampCfg =
                {
                    .rampDownMask = 0x00,
                    .altEventClearReqd = 0,
                },
            }, // txChannelCfg[5]
            {  // txChannelCfg[6]
                .txAttenCfg =
                {
                    .updateCfg =
                    {
                        .srcCfg =
                        {
                            .updateSrc = ADI_ADRV904X_TXATTEN_UPD_SRC_S0,
                            .s0OrS1Gpio = ADI_ADRV904X_GPIO_INVALID,
                        },
                        .trgCfg =
                        {
                            .updateTrg = ADI_ADRV904X_TXATTEN_UPD_TRG_NONE,
                            .triggerGpio = ADI_ADRV904X_GPIO_INVALID,
                        },
                    },
                    .txAttenStepSize = ADI_ADRV904X_TXATTEN_0P05_DB,
                },
                .txAttenInit_mdB = 0,
                .txpowerMonitorCfg =
                {
                    .inputSel = ADI_ADRV904X_POST_DPD_HC_OUTPUT,
                    .peakThreshold = 0,
                    .measDuration = 0,
                    .peakCount = 0,
                    .peakErrorClearRequired = 0,
                    .peakPowerEnable = 0,
                    .peakPowerIrqEnable = 0,
                    .avgThreshold = 0,
                    .avgErrorClearRequired = 0,
                    .avgPowerEnable = 0,
                    .avgPowerIrqEnable = 0,
                    .avgPeakRatioEnable = 0,
                },
                .srlCfg =
                {
                    .inputSel = ADI_ADRV904X_POST_DPD_HC_OUTPUT,
                    .srdOffset = 0,
                    .srdEnable = 0,
                    .srdIrqEnable = 0,
                    .autoRecoveryWaitTime = 0,
                    .autoRecoveryEnable = 0,
                    .autoRecoveryDisableTimerWhenTxOff = 0,
                    .srdStatisticsEnable = 0,
                    .srdStatisticsMode = 0,
                },
                .protectionRampCfg =
                {
                    .rampDownMask = 0x00,
                    .altEventClearReqd = 0,
                },
            }, // txChannelCfg[6]
            {  // txChannelCfg[7]
                .txAttenCfg =
                {
                    .updateCfg =
                    {
                        .srcCfg =
                        {
                            .updateSrc = ADI_ADRV904X_TXATTEN_UPD_SRC_S0,
                            .s0OrS1Gpio = ADI_ADRV904X_GPIO_INVALID,
                        },
                        .trgCfg =
                        {
                            .updateTrg = ADI_ADRV904X_TXATTEN_UPD_TRG_NONE,
                            .triggerGpio = ADI_ADRV904X_GPIO_INVALID,
                        },
                    },
                    .txAttenStepSize = ADI_ADRV904X_TXATTEN_0P05_DB,
                },
                .txAttenInit_mdB = 0,
                .txpowerMonitorCfg =
                {
                    .inputSel = ADI_ADRV904X_POST_DPD_HC_OUTPUT,
                    .peakThreshold = 0,
                    .measDuration = 0,
                    .peakCount = 0,
                    .peakErrorClearRequired = 0,
                    .peakPowerEnable = 0,
                    .peakPowerIrqEnable = 0,
                    .avgThreshold = 0,
                    .avgErrorClearRequired = 0,
                    .avgPowerEnable = 0,
                    .avgPowerIrqEnable = 0,
                    .avgPeakRatioEnable = 0,
                },
                .srlCfg =
                {
                    .inputSel = ADI_ADRV904X_POST_DPD_HC_OUTPUT,
                    .srdOffset = 0,
                    .srdEnable = 0,
                    .srdIrqEnable = 0,
                    .autoRecoveryWaitTime = 0,
                    .autoRecoveryEnable = 0,
                    .autoRecoveryDisableTimerWhenTxOff = 0,
                    .srdStatisticsEnable = 0,
                    .srdStatisticsMode = 0,
                },
                .protectionRampCfg =
                {
                    .rampDownMask = 0x00,
                    .altEventClearReqd = 0,
                },
            }, // txChannelCfg[7]
        }, // txChannelCfg (end of array)
    },
    .uart =
    {
        {  // uart[0]
            .enable = 0,
            .pinSelect = ADI_ADRV904X_GPIO_09,
        }, // uart[0]
        {  // uart[1]
            .enable = 0,
            .pinSelect = ADI_ADRV904X_GPIO_10,
        }, // uart[1]
        {  // uart[2]
            .enable = 0,
            .pinSelect = ADI_ADRV904X_GPIO_11,
        }, // uart[2]
        {  // uart[3]
            .enable = 0,
            .pinSelect = ADI_ADRV904X_GPIO_12,
        }, // uart[3]
    }, // uart (end of array)
    .gpIntPreInit =
    {
        .gpInt0Mask =
        {
            .lowerMask = 0xFFFFFFFFFFFF,
            .upperMask = 0xFFFFFFFFFFFF,
        },
        .gpInt1Mask =
        {
            .lowerMask = 0xFFFFFFFFFFFF,
            .upperMask = 0xFFFFFFFFFFFF,
        },
    },
    .radioCtrlPreInit =
    {
        .radioSequencerSsbSyncGpioCtrl = ADI_ADRV904X_GPIO_INVALID,
        .radioSequencerGpioDigOut =
        {
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[0]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[1]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[2]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[3]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[4]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[5]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[6]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[7]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[8]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[9]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[10]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[11]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[12]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[13]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[14]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[15]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[16]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[17]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[18]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[19]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[20]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[21]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[22]
            ADI_ADRV904X_GPIO_INVALID,  // radioSequencerGpioDigOut[23]
        }, // radioSequencerGpioDigOut (end of array)
        .radioSequencerGpioAnaOut =
        {
            ADI_ADRV904X_GPIO_ANA_INVALID,  // radioSequencerGpioAnaOut[0]
            ADI_ADRV904X_GPIO_ANA_INVALID,  // radioSequencerGpioAnaOut[1]
            ADI_ADRV904X_GPIO_ANA_INVALID,  // radioSequencerGpioAnaOut[2]
            ADI_ADRV904X_GPIO_ANA_INVALID,  // radioSequencerGpioAnaOut[3]
            ADI_ADRV904X_GPIO_ANA_INVALID,  // radioSequencerGpioAnaOut[4]
            ADI_ADRV904X_GPIO_ANA_INVALID,  // radioSequencerGpioAnaOut[5]
            ADI_ADRV904X_GPIO_ANA_INVALID,  // radioSequencerGpioAnaOut[6]
            ADI_ADRV904X_GPIO_ANA_INVALID,  // radioSequencerGpioAnaOut[7]
            ADI_ADRV904X_GPIO_ANA_INVALID,  // radioSequencerGpioAnaOut[8]
            ADI_ADRV904X_GPIO_ANA_INVALID,  // radioSequencerGpioAnaOut[9]
            ADI_ADRV904X_GPIO_ANA_INVALID,  // radioSequencerGpioAnaOut[10]
            ADI_ADRV904X_GPIO_ANA_INVALID,  // radioSequencerGpioAnaOut[11]
            ADI_ADRV904X_GPIO_ANA_INVALID,  // radioSequencerGpioAnaOut[12]
            ADI_ADRV904X_GPIO_ANA_INVALID,  // radioSequencerGpioAnaOut[13]
            ADI_ADRV904X_GPIO_ANA_INVALID,  // radioSequencerGpioAnaOut[14]
            ADI_ADRV904X_GPIO_ANA_INVALID,  // radioSequencerGpioAnaOut[15]
        }, // radioSequencerGpioAnaOut (end of array)
    },
    .dfeUart0 =
    {
        .enableGpioTx = 0,
        .enableGpioRx = 0,
        .enableGpioCts = 0,
        .enableGpioRts = 0,
    },
};
adi_adrv904x_PostMcsInit_t utilityInit = 
{
    .radioCtrlCfg =
    {
        .txRadioCtrlModeCfg =
        {
            .txEnableMode = ADI_ADRV904X_TX_EN_SPI_MODE,
            .txChannelMask = 0xFF,
        },
        .rxRadioCtrlModeCfg =
        {
            .rxEnableMode = ADI_ADRV904X_RX_EN_SPI_MODE,
            .rxChannelMask = 0xFF,
        },
        .orxRadioCtrlModeCfg =
        {
            .orxEnableMode = ADI_ADRV904X_ORX_EN_SPI_MODE,
            .orxChannelMask = 0x300,
        },
    },
    .radioCtrlGpioCfg =
    {
        .txEnMapping =
        {
            0, 0, 0, 0, 0, 0, 0, 0,
        }, // txEnMapping (end of array)
        .txAltMapping =
        {
            0, 0, 0, 0, 0, 0, 0, 0,
        }, // txAltMapping (end of array)
        .rxEnMapping =
        {
            0, 0, 0, 0, 0, 0, 0, 0,
        }, // rxEnMapping (end of array)
        .rxAltMapping =
        {
            0, 0, 0, 0, 0, 0, 0, 0,
        }, // rxAltMapping (end of array)
    },
    .radioCtrlTxRxEnPinSel = 0,
    .radioCtrlTxRxEnCfgSel = 0,
    .gpIntPostInit =
    {
        .gpInt0Mask =
        {
            .lowerMask = 0xFFFFFFFFFFFF,
            .upperMask = 0xFFFFFFFFFFFF,
        },
        .gpInt1Mask =
        {
            .lowerMask = 0xFFFFFFFFFFFF,
            .upperMask = 0xFFFFFFFFFFFF,
        },
    },
    .initCals =
    {
        .calMask = 0x4EFE,
        .rxChannelMask = 0xFF,
        .txChannelMask = 0xFF,
        .orxChannelMask = 0x03,
        .warmBoot = 0,
    },
};
