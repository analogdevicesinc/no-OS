/**
 * \file myk_init.c
 *
 * \brief Contains init setting structure declarations for the _instance API
 *
 * The top level structure mykonosDevice_t mykDevice uses keyword
 * extern to allow the application layer main() to have visibility
 * to these settings.
 *
 * All data structures required for operation have been initialized with values which reflect
 * these settings:
 *
 * Device Clock = $245.76$ Mhz
 * Rx enabled channels = $2$
 * Tx enabled channels = $2$
 *
 * Mykonos Framer:
 * Lane Rate = $4915.2$ Mbs
 * L = $2$
 * K = $32$
 * M = $4$
 * F = $4$
 *
 * Mykonos Deframer:
 * Lane Rate = $4915.2$ Mbs
 * L = $4$
 * K = $32$
 * M = $4$
 * F = $2$
 *
 * Mykonos Observation framer:
 * Lane Rate = $4915.2$ Mbs
 * L = $2$
 * K = $32$
 * M = $2$
 * F = $2$
 *
 * Using these profiles:
 * $Rx 100Mhz, IQrate 122.88Mhz, Dec5$
 * $Tx 75/200Mhz, IQrate 245.76Mhz, Dec5$
 * $ORX 200Mhz, IQrate 245.76Mhz, Dec5$
 * $SRx 20Mhz, IQrate 30.72Mhz, Dec5$
 *
 */

/**
* \page Disclaimer Legal Disclaimer
* Copyright 2015-2017 Analog Devices Inc.
* Released under the AD9371 API license, for more information see the "LICENSE.txt" file in this zip file.
*
*/

#include <stddef.h>
#include "t_mykonos.h"
#include "myk_init.h"

static int16_t txFirCoefs[] = {-94,-26,282,177,-438,-368,756,732,-1170,-1337,1758,2479,-2648,-5088,4064,16760,16759,4110,-4881,-2247,2888,1917,-1440,-1296,745,828,-358,-474,164,298,-16,-94};

static mykonosFir_t txFir =
{
    6,              /* Filter gain in dB*/
    32,             /* Number of coefficients in the FIR filter*/
    &txFirCoefs[0]  /* A pointer to an array of filter coefficients*/
};

static int16_t rxFirCoefs[] = {-13,-53,-50,-20,88,197,231,80,-239,-576,-654,-268,538,1359,1585,749,-1060,-3028,-3847,-2340,1835,7799,13660,17289,17289,13660,7799,1835,-2340,-3847,-3028,-1060,749,1585,1359,538,-268,-654,-576,-239,80,231,197,88,-20,-50,-53,-13};

static mykonosFir_t rxFir =
{
    -6,             /* Filter gain in dB*/
    48,             /* Number of coefficients in the FIR filter*/
    &rxFirCoefs[0]  /* A pointer to an array of filter coefficients*/
};

static int16_t obsrxFirCoefs[] = {-14,-19,44,41,-89,-95,175,178,-303,-317,499,527,-779,-843,1184,1317,-1781,-2059,2760,3350,-4962,-7433,9822,32154,32154,9822,-7433,-4962,3350,2760,-2059,-1781,1317,1184,-843,-779,527,499,-317,-303,178,175,-95,-89,41,44,-19,-14};
static mykonosFir_t obsrxFir =
{
    -6,             /* Filter gain in dB*/
    48,             /* Number of coefficients in the FIR filter*/
    &obsrxFirCoefs[0]/* A pointer to an array of filter coefficients*/
};

static int16_t snifferFirCoefs[] = {-1,-5,-14,-23,-16,24,92,137,80,-120,-378,-471,-174,507,1174,1183,98,-1771,-3216,-2641,942,7027,13533,17738,17738,13533,7027,942,-2641,-3216,-1771,98,1183,1174,507,-174,-471,-378,-120,80,137,92,24,-16,-23,-14,-5,-1};
static mykonosFir_t snifferRxFir=
{
    -6,             /* Filter gain in dB*/
    48,             /* Number of coefficients in the FIR filter*/
    &snifferFirCoefs[0]/* A pointer to an array of filter coefficients*/
};

static mykonosJesd204bFramerConfig_t rxFramer =
{
    0,              /* JESD204B Configuration Bank ID -extension to Device ID (Valid 0..15)*/
    0,              /* JESD204B Configuration Device ID - link identification number. (Valid 0..255)*/
    0,              /* JESD204B Configuration starting Lane ID.  If more than one lane used, each lane will increment from the Lane0 ID. (Valid 0..31)*/
    4,              /* number of ADCs (0, 2, or 4) - 2 ADCs per receive chain*/
    32,             /* number of frames in a multiframe (default=32), F*K must be a multiple of 4. (F=2*M/numberOfLanes)*/
    1,              /* scrambling off if framerScramble= 0, if framerScramble>0 scramble is enabled.*/
    1,              /* 0=use internal SYSREF, 1= use external SYSREF*/
    0x03,           /* serializerLanesEnabled - bit per lane, [0] = Lane0 enabled, [1] = Lane1 enabled*/
    0xE4,           /* serializerLaneCrossbar*/
    22,             /* serializerAmplitude - default 22 (valid (0-31)*/
    4,              /* preEmphasis - < default 4 (valid 0 - 7)*/
    0,              /* invertLanePolarity - default 0 ([0] will invert lane [0], bit1 will invert lane1)*/
    0,              /* lmfcOffset - LMFC offset value for deterministic latency setting*/
    0,              /* Flag for determining if SYSREF on relink should be set. Where, if > 0 = set, 0 = not set*/
    0,              /* Flag for determining if auto channel select for the xbar should be set. Where, if > 0 = set, '0' = not set*/
    0,              /* Selects SYNCb input source. Where, 0 = use RXSYNCB for this framer, 1 = use OBSRX_SYNCB for this framer*/
    0,              /* Flag for determining if CMOS mode for RX Sync signal is used. Where, if > 0 = CMOS, '0' = LVDS*/
    0               /* Selects framer bit repeat or oversampling mode for lane rate matching. Where, 0 = bitRepeat mode (changes effective lanerate), 1 = overSample (maintains same lane rate between ObsRx framer and Rx framer and oversamples the ADC samples)*/
};

static mykonosJesd204bFramerConfig_t obsRxFramer =
{
    0,              /* JESD204B Configuration Bank ID -extension to Device ID (Valid 0..15)*/
    0,              /* JESD204B Configuration Device ID - link identification number. (Valid 0..255)*/
    0,              /* JESD204B Configuration starting Lane ID.  If more than one lane used, each lane will increment from the Lane0 ID. (Valid 0..31)*/
    2,              /* number of ADCs (0, 2, or 4) - 2 ADCs per receive chain*/
    32,             /* number of frames in a multiframe (default=32), F*K must be a multiple of 4. (F=2*M/numberOfLanes)*/
    1,              /* scrambling off if framerScramble= 0, if framerScramble>0 scramble is enabled.*/
    1,              /* 0=use internal SYSREF, 1= use external SYSREF*/
    0x0C,           /* serializerLanesEnabled - bit per lane, [0] = Lane0 enabled, [1] = Lane1 enabled*/
    0xE4,           /* Lane crossbar to map framer lane outputs to physical lanes*/
    22,             /* serializerAmplitude - default 22 (valid (0-31)*/
    4,              /* preEmphasis - < default 4 (valid 0 - 7)*/
    0,              /* invertLanePolarity - default 0 ([0] will invert lane [0], bit1 will invert lane1)*/
    0,              /* lmfcOffset - LMFC offset value for deterministic latency setting*/
    0,              /* Flag for determining if SYSREF on relink should be set. Where, if > 0 = set, 0 = not set*/
    0,              /* Flag for determining if auto channel select for the xbar should be set. Where, if > 0 = set, '0' = not set*/
    1,              /* Selects SYNCb input source. Where, 0 = use RXSYNCB for this framer, 1 = use OBSRX_SYNCB for this framer*/
    0,              /* Flag for determining if CMOS mode for RX Sync signal is used. Where, if > 0 = CMOS, '0' = LVDS*/
    1               /* Selects framer bit repeat or oversampling mode for lane rate matching. Where, 0 = bitRepeat mode (changes effective lanerate), 1 = overSample (maintains same lane rate between ObsRx framer and Rx framer and oversamples the ADC samples)*/
};

static mykonosJesd204bDeframerConfig_t deframer =
{
    0,              /* bankId extension to Device ID (Valid 0..15)*/
    0,              /* deviceId  link identification number. (Valid 0..255)*/
    0,              /* lane0Id Lane0 ID. (Valid 0..31)*/
    4,              /* M  number of DACss (0, 2, or 4) - 2 DACs per transmit chain */
    32,             /* K  #frames in a multiframe (default=32), F*K=multiple of 4. (F=2*M/numberOfLanes)*/
    1,              /* Scrambling off if scramble = 0, if framerScramble > 0 scrambling is enabled*/
    1,              /* External SYSREF select. 0 = use internal SYSREF, 1 = external SYSREF*/
    0x0F,           /* Deserializer lane select bit field. Where, [0] = Lane0 enabled, [1] = Lane1 enabled, etc */
    0xE4,           /* Lane crossbar to map physical lanes to deframer lane inputs [1:0] = Deframer Input 0 Lane section, [3:2] = Deframer Input 1 lane select, etc */
    1,              /* Equalizer setting. Applied to all deserializer lanes. Range is 0..4*/
    0,              /* PN inversion per each lane.  bit[0] = 1 Invert PN of Lane 0, bit[1] = Invert PN of Lane 1, etc).*/
    0,              /* LMFC offset value to adjust deterministic latency. Range is 0..31*/
    0,              /* Flag for determining if SYSREF on relink should be set. Where, if > 0 = set, '0' = not set*/
    0,              /* Flag for determining if auto channel select for the xbar should be set. Where, if > 0 = set, '0' = not set*/
    0               /* Flag for determining if CMOS mode for TX Sync signal is used. Where, if > 0 = CMOS, '0' = LVDS*/
};

static mykonosRxGainControl_t rxGainControl =
{
    MGC,            /* Current Rx gain control mode setting*/
    255,            /* Rx1 Gain Index, can be used in different ways for manual and AGC gain control*/
    255,            /* Rx2 Gain Index, can be used in different ways for manual and AGC gain control*/
    255,            /* Max gain index for the currently loaded Rx1 Gain table*/
    195,            /* Min gain index for the currently loaded Rx1 Gain table*/
    255,            /* Max gain index for the currently loaded Rx2 Gain table*/
    195,            /* Min gain index for the currently loaded Rx2 Gain table*/
    0,              /* Stores Rx1 RSSI value read back from the Mykonos*/
    0               /* Stores Rx2 RSSI value read back from the Mykonos*/
};

static mykonosORxGainControl_t orxGainControl =
{
    MGC,            /* Current ORx gain control mode setting*/
    255,            /* ORx1 Gain Index, can be used in different ways for manual and AGC gain control*/
    255,            /* ORx2 Gain Index, can be used in different ways for manual and AGC gain control*/
    255,            /* Max gain index for the currently loaded ORx Gain table*/
    237             /* Min gain index for the currently loaded ORx Gain table*/
};

static mykonosSnifferGainControl_t snifferGainControl =
{
    MGC,            /* Current Sniffer gain control mode setting*/
    255,            /* Current Sniffer gain index. Can be used differently for Manual Gain control/AGC*/
    255,            /* Max gain index for the currently loaded Sniffer Gain table*/
    203             /* Min gain index for the currently loaded Sniffer Gain table*/
};

static mykonosPeakDetAgcCfg_t rxPeakAgc =
{
    0x1F,    /* apdHighThresh: */
    0x16,    /* apdLowThresh */
    0xB5,    /* hb2HighThresh */
    0x80,    /* hb2LowThresh */
    0x40,    /* hb2VeryLowThresh */
    0x06,    /* apdHighThreshExceededCnt */
    0x04,    /* apdLowThreshExceededCnt */
    0x06,    /* hb2HighThreshExceededCnt */
    0x04,    /* hb2LowThreshExceededCnt */
    0x04,    /* hb2VeryLowThreshExceededCnt */
    0x4,    /* apdHighGainStepAttack */
    0x2,    /* apdLowGainStepRecovery */
    0x4,    /* hb2HighGainStepAttack */
    0x2,    /* hb2LowGainStepRecovery */
    0x4,    /* hb2VeryLowGainStepRecovery */
    0x1,    /* apdFastAttack */
    0x1,    /* hb2FastAttack */
    0x1,    /* hb2OverloadDetectEnable */
    0x1,    /* hb2OverloadDurationCnt */
    0x1    /* hb2OverloadThreshCnt */
};

static mykonosPowerMeasAgcCfg_t rxPwrAgc =
{
    0x01,    /* pmdUpperHighThresh */
    0x03,    /* pmdUpperLowThresh */
    0x0C,    /* pmdLowerHighThresh */
    0x04,    /* pmdLowerLowThresh */
    0x4,    /* pmdUpperHighGainStepAttack */
    0x2,    /* pmdUpperLowGainStepAttack */
    0x2,    /* pmdLowerHighGainStepRecovery */
    0x4,    /* pmdLowerLowGainStepRecovery */
    0x08,    /* pmdMeasDuration */
    0x02    /* pmdMeasConfig */
};

static mykonosAgcCfg_t rxAgcConfig =
{
    255,    /* AGC peak wait time */
    195,    /* agcRx1MinGainIndex */
    255,    /* agcRx2MaxGainIndex */
    195,    /* agcRx2MinGainIndex: */
    255,    /* agcObsRxMaxGainIndex */
    203,    /* agcObsRxMinGainIndex */
    1,        /* agcObsRxSelect */
    1,        /* agcPeakThresholdMode */
    1,        /* agcLowThsPreventGainIncrease */
    30720,    /* agcGainUpdateCounter */
    3,    /* agcSlowLoopSettlingDelay */
    2,    /* agcPeakWaitTime */
    0,    /* agcResetOnRxEnable */
    0,    /* agcEnableSyncPulseForGainCounter */
    &rxPeakAgc,
    &rxPwrAgc
};

static mykonosPeakDetAgcCfg_t obsRxPeakAgc =
{
    0x1F,    /* apdHighThresh: */
    0x16,    /* apdLowThresh */
    0xB5,    /* hb2HighThresh */
    0x80,    /* hb2LowThresh */
    0x40,    /* hb2VeryLowThresh */
    0x06,    /* apdHighThreshExceededCnt */
    0x04,    /* apdLowThreshExceededCnt */
    0x06,    /* hb2HighThreshExceededCnt */
    0x04,    /* hb2LowThreshExceededCnt */
    0x04,    /* hb2VeryLowThreshExceededCnt */
    0x4,    /* apdHighGainStepAttack */
    0x2,    /* apdLowGainStepRecovery */
    0x4,    /* hb2HighGainStepAttack */
    0x2,    /* hb2LowGainStepRecovery */
    0x4,    /* hb2VeryLowGainStepRecovery */
    0x1,    /* apdFastAttack */
    0x1,    /* hb2FastAttack */
    0x1,    /* hb2OverloadDetectEnable */
    0x1,    /* hb2OverloadDurationCnt */
    0x1        /* hb2OverloadThreshCnt */
};

static mykonosPowerMeasAgcCfg_t obsRxPwrAgc =
{
    0x01,    /* pmdUpperHighThresh */
    0x03,    /* pmdUpperLowThresh */
    0x0C,    /* pmdLowerHighThresh */
    0x04,    /* pmdLowerLowThresh */
    0x4,    /* pmdUpperHighGainStepAttack */
    0x2,    /* pmdUpperLowGainStepAttack */
    0x2,    /* pmdLowerHighGainStepRecovery */
    0x4,    /* pmdLowerLowGainStepRecovery */
    0x08,    /* pmdMeasDuration */
    0x02    /* pmdMeasConfig */
};

static mykonosAgcCfg_t obsRxAgcConfig =
{
    255,    /* agcRx1MaxGainIndex */
    195,    /* agcRx1MinGainIndex */
    255,    /* agcRx2MaxGainIndex */
    195,    /* agcRx2MinGainIndex: */
    255,    /* agcObsRxMaxGainIndex */
    203,    /* agcObsRxMinGainIndex */
    1,        /* agcObsRxSelect */
    1,        /* agcPeakThresholdMode */
    1,        /* agcLowThsPreventGainIncrease */
    30720,    /* agcGainUpdateCounter */
    3,        /* agcSlowLoopSettlingDelay */
    2,        /* agcPeakWaitTime */
    0,        /* agcResetOnRxEnable */
    0,        /* agcEnableSyncPulseForGainCounter */
    &obsRxPeakAgc,
    &obsRxPwrAgc
};

static uint16_t rxAdcCustom[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static mykonosRxProfile_t rxProfile =
{/* Rx 20MHz, IQrate 30.72MHz, Dec5 */
    1,              /* The divider used to generate the ADC clock*/
    &rxFir,         /* Pointer to Rx FIR filter structure*/
    4,              /* Rx FIR decimation (1,2,4)*/
    5,              /* Decimation of Dec5 or Dec4 filter (5,4)*/
    1,              /* If set, and DEC5 filter used, will use a higher rejection DEC5 FIR filter (1=Enabled, 0=Disabled)*/
    2,              /* RX Half band 1 decimation (1 or 2)*/
    30720,          /* Rx IQ data rate in kHz*/
    20000000,       /* The Rx RF passband bandwidth for the profile*/
    20000,          /* Rx BBF 3dB corner in kHz*/
    &rxAdcCustom[0]            /* pointer to custom ADC profile*/
};

static uint16_t orxAdcCustom[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static mykonosRxProfile_t orxProfile =
{/* ORX 100MHz, IQrate 122.88MHz, Dec5 */
    1,              /* The divider used to generate the ADC clock*/
    &obsrxFir,      /* Pointer to Rx FIR filter structure or NULL*/
    2,              /* Rx FIR decimation (1,2,4)*/
    5,              /* Decimation of Dec5 or Dec4 filter (5,4)*/
    0,              /* If set, and DEC5 filter used, will use a higher rejection DEC5 FIR filter (1=Enabled, 0=Disabled)*/
    1,              /* RX Half band 1 decimation (1 or 2)*/
    122880,         /* Rx IQ data rate in kHz*/
    100000000,      /* The Rx RF passband bandwidth for the profile*/
    100000,         /* Rx BBF 3dB corner in kHz*/
    &orxAdcCustom[0]   /* Pointer to custom ADC profile*/
};

static uint16_t snifferAdcCustom[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static mykonosRxProfile_t snifferProfile =
{ /* SRx 20MHz, IQrate 30.72MHz, Dec5 */
    1,              /* The divider used to generate the ADC clock*/
    &snifferRxFir,  /* Pointer to Rx FIR filter structure or NULL*/
    4,              /* Rx FIR decimation (1,2,4)*/
    5,              /* Decimation of Dec5 or Dec4 filter (5,4)*/
    0,              /* If set, and DEC5 filter used, will use a higher rejection DEC5 FIR filter (1=Enabled, 0=Disabled)*/
    2,              /* RX Half band 1 decimation (1 or 2)*/
    30720,          /* Rx IQ data rate in kHz*/
    20000000,       /* The Rx RF passband bandwidth for the profile*/
    100000,         /* Rx BBF 3dB corner in kHz*/
    &snifferAdcCustom[0]            /* pointer to custom ADC profile*/
};



static mykonosTxProfile_t txProfile =
{ /* Tx 20/100MHz, IQrate 122.88MHz, Dec5 */
    DACDIV_2p5,     /* The divider used to generate the DAC clock*/
    &txFir,         /* Pointer to Tx FIR filter structure*/
    2,              /* The Tx digital FIR filter interpolation (1,2,4)*/
    2,              /* Tx Halfband1 filter interpolation (1,2)*/
    1,              /* Tx Halfband2 filter interpolation (1,2)*/
    1,              /* TxInputHbInterpolation (1,2)*/
    122880,         /* Tx IQ data rate in kHz*/
    20000000,       /* Primary Signal BW*/
    100000000,      /* The Tx RF passband bandwidth for the profile*/
    710539,         /* The DAC filter 3dB corner in kHz*/
    50000,          /* Tx BBF 3dB corner in kHz*/
    0               /* Enable DPD, only valid for AD9373*/
};

static mykonosDigClocks_t mykonosClocks =
{
    122880,         /* CLKPLL and device reference clock frequency in kHz*/
    9830400,        /* CLKPLL VCO frequency in kHz*/
    VCODIV_2,       /* CLKPLL VCO divider*/
    4               /* CLKPLL high speed clock divider*/
};

static mykonosRxSettings_t  rxSettings =
{
    &rxProfile,     /* Rx datapath profile, 3dB corner frequencies, and digital filter enables*/
    &rxFramer,      /* Rx JESD204b framer configuration structure*/
    &rxGainControl, /* Rx Gain control settings structure*/
    &rxAgcConfig,   /* Rx AGC control settings structure*/
    3,              /* The desired Rx Channels to enable during initialization*/
    0,              /* Internal LO = 0, external LO*2 = 1*/
    2550000000U,    /* Rx PLL LO Frequency (internal or external LO)*/
    0               /* Flag to choose if complex baseband or real IF data are selected for Rx and ObsRx paths. Where, if > 0 = real IF data, '0' = zero IF (IQ) data*/
};

static mykonosDpdConfig_t dpdConfig =
{
    5,              /* 1/2^(damping + 8) fraction of power `forgotten' per sample (default: `1/8192' = 5, valid 0 to 15), 0 = infinite damping*/
    1,              /* number of weights to use for int8_cpx weights weights member of this structure (default = 1)*/
    2,              /* DPD model version: one of four different generalized polynomial models: 0 = same as R0 silicon, 1-3 are new and the best one depends on the PA (default: 2)*/
    1,              /* 1 = Update saved model whenever peak Tx digital RMS is within 1dB of historical peak Tx RMS*/
    20,             /* Determines how much weight the loaded prior model has on DPD modeling (Valid 0 - 32, default 20)*/
    0,              /* Default off = 0, 1=enables automatic outlier removal during DPD modeling */
    512,            /* Number of samples to capture (default: 512, valid 64-32768)*/
    4096,           /* threshold for sample in AM-AM plot outside of 1:1 line to be thrown out. (default: 50% = 8192/2, valid 8192 to 1)*/
    0,              /* 16th of an ORx sample (16=1sample), (default 0, valid -64 to 64)*/
    255,            /* Default 255 (-30dBFs=(20Log10(value/8192)), (valid range  1 to 8191)*/
    {{64,0},{0,0},{0,0}}/* DPD model error weighting (real/imag valid from -128 to 127)*/
};

static mykonosClgcConfig_t clgcConfig =
{
    -2000,          /* (value = 100 * dB (valid range -32768 to 32767) - total gain and attenuation from Mykonos Tx1 output to ORx1 input in (dB * 100)*/
    -2000,          /* (value = 100 * dB (valid range -32768 to 32767) - total gain and attenuation from Mykonos Tx2 output to ORx2 input in (dB * 100)*/
    0,              /* (valid range 0 - 40dB), no default, depends on PA, Protects PA by making sure Tx1Atten is not reduced below the limit*/
    0,              /* (valid range 0 - 40dB), no default, depends on PA, Protects PA by making sure Tx2Atten is not reduced below the limit*/
    75,             /* valid range 1-100, default 75*/
    75,             /* valid range 1-100, default 45*/
    0,              /* 0= allow CLGC to run, but Tx1Atten will not be updated. User can still read back power measurements.  1=CLGC runs, and Tx1Atten automatically updated*/
    0,              /* 0= allow CLGC to run, but Tx2Atten will not be updated. User can still read back power measurements.  1=CLGC runs, and Tx2Atten automatically updated*/
    0,              /* 16th of an ORx sample (16=1sample), (default 0, valid -64 to 64)*/
    255,            /* Default 255 (-30dBFs=(20Log10(value/8192)), (valid range  1 to 8191)*/
    6,              /*!< Threshold for Tx1 in order to stop tracking, value = 100 * dB */
    6,              /*!< Threshold for Tx2 in order to stop tracking, value = 100 * dB */
    0,              /*!< Threshold feature enable for Tx1, 0 = disable, 1 = enable */
    0               /*!< Threshold feature enable for Tx2, 0 = disable, 1 = enable */
};

static mykonosVswrConfig_t vswrConfig =
{
    0,              /* 16th of an ORx sample (16=1sample), (default 0, valid -64 to 64)*/
    255,            /* Default 255 (-30dBFs=(20Log10(value/8192)), (valid range  1 to 8191)*/
    0,              /* 3p3V GPIO pin to use to control VSWR switch for Tx1 (valid 0-11) (output from Mykonos)*/
    1,              /* 3p3V GPIO pin to use to control VSWR switch for Tx2 (valid 0-11) (output from Mykonos)*/
    0,              /* 3p3v GPIO pin polarity for forward path of Tx1, opposite used for reflection path (1 = high level, 0 = low level)*/
    0,              /* 3p3v GPIO pin polarity for forward path of Tx2, opposite used for reflection path (1 = high level, 0 = low level)*/
    1,              /* Delay for Tx1 after flipping the VSWR switch until measurement is made. In ms resolution*/
    1               /* Delay for Tx2 after flipping the VSWR switch until measurement is made. In ms resolution*/
};

static mykonosTxSettings_t txSettings =
{
    &txProfile,     /* Tx datapath profile, 3dB corner frequencies, and digital filter enables*/
    &deframer,      /* Mykonos JESD204b deframer config for the Tx data path*/
    TX1_TX2,        /* The desired Tx channels to enable during initialization*/
    0,              /* Internal LO=0, external LO*2 if =1*/
    2500000000U,    /* Tx PLL LO frequency (internal or external LO)*/
    TXATTEN_0P05_DB,/* Initial and current Tx1 Attenuation*/
    10000,          /* Initial and current Tx1 Attenuation mdB*/
    10000,          /* Initial and current Tx2 Attenuation mdB*/
    &dpdConfig,           /* DPD,CLGC,VSWR settings. Only valid for AD9373 device, set pointer to NULL otherwise*/
    &clgcConfig,           /* CLGC Config Structure. Only valid for AD9373 device, set pointer to NULL otherwise*/
    &vswrConfig            /* VSWR Config Structure. Only valid for AD9373 device, set pointer to NULL otherwise*/
};

static uint16_t lpbkAdcCustom[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static mykonosObsRxSettings_t obsRxSettings =
{
    &orxProfile,    /* ORx datapath profile, 3dB corner frequencies, and digital filter enables*/
    &orxGainControl,/* ObsRx gain control settings structure*/
    &obsRxAgcConfig,/* ORx AGC control settings structure*/
    &snifferProfile,/* Sniffer datapath profile, 3dB corner frequencies, and digital filter enables*/
    &snifferGainControl,/* SnRx gain control settings structure*/
    &obsRxFramer,   /* ObsRx JESD204b framer configuration structure */
    (MYK_ORX1_ORX2 | MYK_SNRXA_B_C),/* obsRxChannel */
    OBSLO_TX_PLL,   /* (obsRxLoSource) The Obs Rx mixer can use the Tx Synth(TX_PLL) or Sniffer Synth (SNIFFER_PLL) */
    2600000000U,    /* SnRx PLL LO frequency in Hz */
    0,              /* Flag to choose if complex baseband or real IF data are selected for Rx and ObsRx paths. Where if > 0 = real IF data, '0' = complex data*/
    &lpbkAdcCustom[0],           /* Custom Loopback ADC profile to set the bandwidth of the ADC response */
    OBS_RXOFF       /* Default ObsRx channel to enter when radioOn called */
};

static mykonosArmGpioConfig_t armGpio =
{
    0,    /*!< 0= RX1_ENABLE controls RX1 and RX2, 1 = separate RX1_ENABLE/RX2_ENABLE pins */
    0,    /*!< 0= TX1_ENABLE controls TX1 and TX2, 1 = separate TX1_ENABLE/TX2_ENABLE pins */
    0,    /*!< 0= ARM command mode, 1 = Pin mode to power up Tx/Rx chains */
    0,    /*!< 0= ARM command mode, 1 = Pin mode to power up ObsRx receiver*/

    /*Mykonos ARM input GPIO pins -- Only valid if orxPinMode = 1 */
    0,    /*!< Select desired GPIO pin (valid 4-15) */
    0,    /*!< Select desired GPIO pin (valid 0-18) */
    0,    /*!< Select desired GPIO pin (valid 0-18) */
    0,    /*!< Select desired GPIO pin (valid 0-18) */

    /* Mykonos ARM output GPIO pins  --  always available, even when pin mode not enabled*/
    0,    /*!< Select desired GPIO pin (0-15), [4] = Output Enable */
    0,    /*!< Select desired GPIO pin (0-15), [4] = Output Enable */
    0,    /*!< Select desired GPIO pin (0-15), [4] = Output Enable */
    0,    /*!< Select desired GPIO pin (0-15), [4] = Output Enable */
    0,    /*!< Select desired GPIO pin (0-15), [4] = Output Enable */
    0,    /*!< Select desired GPIO pin (0-15), [4] = Output Enable */
    0,    /*!< Select desired GPIO pin (0-15), [4] = Output Enable */
    0     /*!< Select desired GPIO pin (0-15), [4] = Output Enable */
    /* When 2Tx are used with only 1 ORx input, this GPIO tells the BBIC which Tx channel is   */
    /* active for calibrations, so BBIC can route correct RF Tx path into the single ORx input*/
};

static mykonosGpio3v3_t gpio3v3 =
{
    0,                       /*!< Oe per pin, 1=output, 0 = input */
    GPIO3V3_BITBANG_MODE,    /*!< Mode for GPIO3V3[3:0] */
    GPIO3V3_BITBANG_MODE,    /*!< Mode for GPIO3V3[7:4] */
    GPIO3V3_BITBANG_MODE,    /*!< Mode for GPIO3V3[11:8] */
};

static mykonosGpioLowVoltage_t gpio =
{
    255,               /* Oe per pin, 1=output, 0 = input */
    GPIO_MONITOR_MODE, /* Mode for GPIO[3:0] */
    GPIO_MONITOR_MODE, /* Mode for GPIO[7:4] */
    GPIO_MONITOR_MODE, /* Mode for GPIO[11:8] */
    GPIO_MONITOR_MODE, /* Mode for GPIO[15:12] */
    GPIO_MONITOR_MODE, /* Mode for GPIO[18:16] */
};

static mykonosAuxIo_t mykonosAuxIo =
{
    0,                         /* auxDacEnableMask uint16_t */
    {0,0,0,0,0,0,0,0,0,0},     /* AuxDacValue uint16[10] */
    {0,0,0,0,0,0,0,0,0,0},     /* AuxDacSlope uint8[10] */
    {0,0,0,0,0,0,0,0,0,0},     /* AuxDacVref uint8[10] */
    &gpio3v3,                  /* pointer to gpio3v3 struct */
    &gpio,                     /* pointer to gpio1v8 struct*/
    &armGpio
};

static spiSettings_t mykSpiSettings =
{
    1, /* chip select index - valid 1~8 */
    0, /* the level of the write bit of a SPI write instruction word, value is inverted for SPI read operation */
    1, /* 1 = 16-bit instruction word, 0 = 8-bit instruction word */
    1, /* 1 = MSBFirst, 0 = LSBFirst */
    0, /* clock phase, sets which clock edge the data updates (valid 0 or 1) */
    0, /* clock polarity 0 = clock starts low, 1 = clock starts high */
    0, /* Not implemented in ADIs platform layer. SW feature to improve SPI throughput */
    1, /* Not implemented in ADIs platform layer. For SPI Streaming, set address increment direction. 1= next addr = addr+1, 0:addr=addr-1 */
    1  /* 1: Use 4-wire SPI, 0: 3-wire SPI (SDIO pin is bidirectional). NOTE: ADI's FPGA platform always uses 4-wire mode */
};

mykonosDevice_t mykDevice =
{
    &mykSpiSettings,    /* SPI settings data structure pointer */
    &rxSettings,        /* Rx settings data structure pointer */
    &txSettings,        /* Tx settings data structure pointer */
    &obsRxSettings,     /* ObsRx settings data structure pointer */
    &mykonosAuxIo,      /* Auxiliary IO settings data structure pointer */
    &mykonosClocks,     /* Holds settings for CLKPLL and reference clock */
    0                   /* Mykonos initialize function uses this as an output to remember which profile data structure pointers are valid */
};
