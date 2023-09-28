/* SPDX-License-Identifier: GPL-2.0 */
/*!
* \file adi_adrv9025_types.h
* \brief Contains ADRV9025 API configuration and run-time type definitions
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
 * Copyright 2015 - 2020 Analog Devices Inc.
 * Released under the FPGA9025 API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

#ifndef _ADI_ADRV9025_TYPES_H_
#define _ADI_ADRV9025_TYPES_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#endif
#include "adi_adrv9025_tx_types.h"
#include "adi_adrv9025_rx_types.h"
#include "adi_adrv9025_cpu_types.h"
#include "adi_adrv9025_data_interface_types.h"
#include "adi_common.h"
#include "../../private/include/adrv9025_shared_resource_manager_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define PRIVATE_TYPE_9025 adrv9025_SharedResourcePool_t

#define ADI_ADRV9025_TX_PROFILE_VALID    0x01
#define ADI_ADRV9025_RX_PROFILE_VALID    0x02
#define ADI_ADRV9025_ORX_PROFILE_VALID   0x04
#define ADI_ADRV9025_LB_PROFILE_VALID    0x08

#define ADI_ADRV9025_MAX_TXCHANNELS 4
#define ADI_ADRV9025_MAX_RXCHANNELS 10
#define ADI_ADRV9025_MAX_RX_ONLY    4
#define ADI_ADRV9025_MAX_ORX_ONLY   4
#define ADI_ADRV9025_MAX_LB_CHANNEL_START 8
#define ADI_ADRV9025_MAX_LB_CHANNEL_END   9
#define ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET 12
#define ADI_ADRV9025_MAX_TX_CHANNEL_START     0
#define ADI_ADRV9025_MAX_TX_CHANNEL_END       3
#define ADI_ADRV9025_MAX_RX_CHANNEL_START     0
#define ADI_ADRV9025_MAX_RX_CHANNEL_END       3
#define ADI_ADRV9025_MAX_ORX_CHANNEL_START    4
#define ADI_ADRV9025_MAX_ORX_CHANNEL_END      7

#define ADI_ADRV9025_NUM_FRAMERS   3
#define ADI_ADRV9025_NUM_DEFRAMERS 2

#define ADI_ADRV9025_MAX_RXPFIR_COEFS     72
#define ADI_ADRV9025_MAX_TXPFIR_COEFS     80
#define ADI_ADRV9025_RXADC_COEFS		  41

#define ADI_ADRV9025_MAX_SERDES_LANES 4u /* Number of lanes in serializer and deserializer */

#define ADI_ADRV9025_MAX_AUXDACS 8U

/**
 *  \brief Data structure to hold ADRV9025 API State
 */
typedef enum adi_adrv9025_ApiStates
{
    ADI_ADRV9025_STATE_POWERONRESET     = 0x00,
    ADI_ADRV9025_STATE_INITIALIZED      = 0x01,
    ADI_ADRV9025_STATE_STREAMLOADED     = 0x02,
    ADI_ADRV9025_STATE_CPUDEBUGLOADED   = 0x04,
    ADI_ADRV9025_STATE_CPULOADED        = 0x08,
    ADI_ADRV9025_STATE_INITCALS_RUN     = 0x10,
    ADI_ADRV9025_STATE_RADIOON          = 0x20
} adi_adrv9025_ApiStates_e;

/**
 * \brief Enumerated list of CMOS pads drive strength options for SPI_DO signal
 */
typedef enum adi_adrv9025_CmosPadDrvStr
{
    ADI_ADRV9025_CMOSPAD_DRV_WEAK   = 0, /*!<    5pF load @ 75MHz */
    ADI_ADRV9025_CMOSPAD_DRV_STRONG = 1  /*!<  100pF load @ 20MHz */
} adi_adrv9025_CmosPadDrvStr_e;

/**
* \brief Enumerated list of options to use to set the LO source for the Rx/Tx and ORx mixers.
*/
typedef enum adi_adrv9025_LoSel
{
    ADI_ADRV9025_LOSEL_LO1      = 1,
    ADI_ADRV9025_LOSEL_LO2      = 2,
    ADI_ADRV9025_LOSEL_AUXLO    = 3
} adi_adrv9025_LoSel_e;

/**
* \brief Enumerated list of options to use to set the LO source for the ORx mixers.
*/
typedef enum adi_adrv9025_OrxLoSel
{
    ADI_ADRV9025_ORXLOSEL_TXLO  = 1,
    ADI_ADRV9025_ORXLOSEL_AUXLO = 2
} adi_adrv9025_OrxLoSel_e;

/**
* \brief Enumerated list of options to use to set the ADRV9025 PLL Output and Input modes.
*/
typedef enum adi_adrv9025_PllLoMode
{
    ADI_ADRV9025_INTLO_NOOUTPUT = 0,        /*!< Use internal RFPLL LO frequency, do not output from ADRV9025 */
    ADI_ADRV9025_INTLO_OUTPUT   = 1,        /*!< Internal RFPLL LO frequency will output from ADR9025 */
    ADI_ADRV9025_EXTLO_2X_INPUT = 2,        /*!< Provided external LO will be divided by 2 in ADRV9025 to generate the LO frequency */
    ADI_ADRV9025_EXTLO_4X_INPUT = 3,        /*!< Provided external LO will be divided by 4 in ADRV9025 to generate the LO frequency */
    ADI_ADRV9025_EXTLO_8X_INPUT = 4         /*!< Provided external LO will be divided by 8 in ADRV9025 to generate the LO frequency */
} adi_adrv9025_PllLoMode_e;

/**
 * \brief Enumerated list of RFPLL phase synchronization modes
 *
 * RFPLL Phase sync requires extra time to sync each time the RFPLL frequency
 * is changed. If RFPLL phase sync is not required, it may be desired to
 * disable the feature to allow the RFPLL to lock faster.
 *
 * Depending on the desired accuracy of the RFPLL phase sync, several options
 * are provided.
 */
typedef enum adi_adrv9025_RfPllMcs
{
    ADI_ADRV9025_RFPLLMCS_NOSYNC                = 0,    /*!< Disable RFPLL phase synchronization */
    ADI_ADRV9025_RFPLLMCS_INIT_AND_SYNC         = 1,    /*!< Enable RFPLL phase sync init only */
    ADI_ADRV9025_RFPLLMCS_INIT_AND_CONTTRACK    = 2     /*!< Enable RFPLL phase sync init and track continuously */
} adi_adrv9025_RfPllMcs_e;

/**
*  \brief Enum to select the proper gain for the FIR
*/
typedef enum adi_adrv9025_FirGain
{
    ADRV9025_FIR_GAIN_NEG12_DB  = -12,      /*!< ADRV9025 FIR gain -12 */
    ADRV9025_FIR_GAIN_NEG6_DB   = -6,       /*!< ADRV9025 FIR gain -6 */
    ADRV9025_FIR_GAIN_0_DB      = 0,        /*!< ADRV9025 FIR gain 0 */
    ADRV9025_FIR_GAIN_POS6_DB   = 6         /*!< ADRV9025 FIR gain 6 */
} adi_adrv9025_FirGain_e;

/**
*  \brief Data structure to hold ADRV9025 MCS Control
*/
typedef enum adi_adrv9025_McsControl
{
    ADI_ADRV9025_MCS_NONE           = 0x00,
    ADI_ADRV9025_MCS_ENABLE         = 0x01,
    ADI_ADRV9025_MCS_JESD_ENABLE    = 0x02,
    ADI_ADRV9025_MCS_DIGITAL_ENABLE = 0x04,
    ADI_ADRV9025_MCS_CLKGEN_ENABLE  = 0x08,
    ADI_ADRV9025_MCS_SDM_ENABLE     = 0x10,
    ADI_ADRV9025_MCS_DEVCLK_ENABLE  = 0x20
} adi_adrv9025_McsControl_e;

/**
*  \brief Enum of possible Capability Feature enables
*/
typedef enum adi_adrv9025_Feature
{
    ADI_ADRV9025_FEATUREOFF         = 0x00000000,       /*!< No Capability Feature enabled */
    ADI_ADRV9025_FEATURE_RESERVED1  = 0x00000001,       /*!< Reserve1 feature enabled */
    ADI_ADRV9025_FEATURE_SIP2       = 0x00000002,       /*!< SPI2 feature enabled */
    ADI_ADRV9025_FEATURE_DPD        = 0x00000004,       /*!< DPD feature enabled */
    ADI_ADRV9025_FEATURE_CFR        = 0x00000008,       /*!< CFR feature enabled */
    ADI_ADRV9025_FEATURE_TXSRL      = 0x00000010,       /*!< Tx Slew Rate Limiter feature enabled */
    ADI_ADRV9025_FEATURE_ARMDPD     = 0x00000020,       /*!< ARM D with DPD enabled */
    ADI_ADRV9025_FEATURE_RESERVED2  = 0x00000040,       /*!< Reserve2 feature enabled jesd compression */
    ADI_ADRV9025_FEATURE_RFLO1      = 0x00000080,       /*!< RF LO1 Channel1 enable */
    ADI_ADRV9025_FEATURE_RFLO2      = 0x00000100,       /*!< RF LO2 Channel1 enable */
    ADI_ADRV9025_FEATUREALL         = 0x000001FF,       /*!< All Capability feature enabled */
    ADI_ADRV9025_FEATUREMAX         = 11                /*!< Max Capability feature */
} adi_adrv9025_Feature_e;

/**
*  \brief Enum of possible Lane Mask enables
*/
typedef enum adi_adrv9025_Lane_Enable
{
    ADI_ADRV9025_ALL_LANES_DISABLED     = 0x00000000,   /*!< Disable all lanes */
    ADI_ADRV9025_LANE0_ENABLE           = 0x00000001,   /*!< Enable Lane 0 */
    ADI_ADRV9025_LANE1_ENABLE           = 0x00000002,   /*!< Enable Lane 1 */
    ADI_ADRV9025_LANE2_ENABLE           = 0x00000004,   /*!< Enable Lane 2 */
    ADI_ADRV9025_LANE3_ENABLE           = 0x00000008,   /*!< Enable Lane 3 */
    ADI_ADRV9025_LANE4_ENABLE           = 0x00000010,   /*!< Enable Lane 4 */
    ADI_ADRV9025_LANE5_ENABLE           = 0x00000020,   /*!< Enable Lane 5 */
    ADI_ADRV9025_LANE6_ENABLE           = 0x00000040,   /*!< Enable Lane 6 */
    ADI_ADRV9025_LANE7_ENABLE           = 0x00000080,   /*!< Enable Lane 7 */
    ADI_ADRV9025_ALL_LANES_ENABLED      = 0x000000FF,   /*!< Enable all lanes */
    ADI_ADRV9025_MAX_NUMBER_LANES       = 0x8           /*!< Maximum Number of Lanes */
} adi_adrv9025_Lane_Enable_e;

/**
*  \brief Data structure to hold ADRV9025 API version information
*/
typedef struct adi_adrv9025_ApiVersion
{
    uint32_t majorVer;       /*!< API Major Version number */
    uint32_t minorVer;       /*!< API Minor Version number */
    uint32_t maintenanceVer; /*!< API Maintenance number */
    uint32_t buildVer;       /*!< API Build Version number */
} adi_adrv9025_ApiVersion_t;

/**
 *  \brief Data structure to hold ADRV9025 Rx FIR filter settings
 */
typedef struct adi_adrv9025_RxFir
{
    int8_t  gain_dB;                              /*!< Filter gain in dB (-12dB, -6dB, 0dB, +6dB) */
    uint8_t numFirCoefs;                          /*!< Number of coefficients in the FIR filter */
    int16_t coefs[ADI_ADRV9025_MAX_RXPFIR_COEFS]; /*!< An array of filter coefficients */
} adi_adrv9025_RxFir_t;

/**
 *  \brief Data structure to hold ADRV9025 Tx FIR filter settings
 */
typedef struct adi_adrv9025_TxFir
{
    int8_t  gain_dB;                              /*!< Filter gain in dB (-12dB, -6dB, 0dB, +6dB) */
    uint8_t numFirCoefs;                          /*!< Number of coefficients in the FIR filter */
    int16_t coefs[ADI_ADRV9025_MAX_TXPFIR_COEFS]; /*!< An array of filter coefficients */
} adi_adrv9025_TxFir_t;

/**
 *  \brief Data structure to hold ADRV9025 Tx Profile Configuration settings
 */
typedef struct adi_adrv9025_TxProfile
{
    uint32_t             txInputRate_kHz;         /*!< Tx input data rate in kHz */
    uint32_t             primarySigBandwidth_kHz; /*!< Tx primary signal BW */
    uint32_t             rfBandwidth_kHz;         /*!< Tx RF passband bandwidth for the profile */
    uint32_t             txDac3dBCorner_kHz;      /*!< DAC filter 3dB corner in kHz */
    uint32_t             txBbf3dBCorner_kHz;      /*!< Tx BBF 3dB corner in kHz */
    uint8_t              dpdHb1Interpolation;     /*!< Tx DPD HB1 filter interpolation (1,2) */
    uint8_t              dpdHb2Interpolation;     /*!< Tx DPD HB2 filter interpolation (1,2) */
    uint8_t              txFirInterpolation;      /*!< Tx programmable FIR filter interpolation (1,2,4) */
    uint8_t              thb1Interpolation;       /*!< Tx Halfband1 (HB1) filter interpolation (1,2) */
    uint8_t              thb2Interpolation;       /*!< Tx Halfband2 (HB2) filter interpolation (1,2) */
    uint8_t              thb3Interpolation;       /*!< Tx Halfband3 (HB3) filter interpolation (1,2) */
    uint8_t              txInt5Interpolation;     /*!< Tx Int5 filter interpolation (1,5) */
    adi_adrv9025_TxFir_t txFir;                   /*!< Tx FIR filter structure */
    uint8_t              txBbfPowerMode;          /*!< Tx BBF power scaling mode selection = 0 for Maximum power. */
} adi_adrv9025_TxProfile_t;

/**
 *  \brief Data structure to hold ADRV9025 Tx Attenuation Control settings
 */
typedef struct adi_adrv9025_TxAttenControl
{
    adi_adrv9025_TxAttenStepSize_e txAttenStepSize;         /*!< Tx Attenuation step size */
    uint32_t                       txRampDownEvents;        /*!< Initial mask values for individual Tx ramp down events. Masks the event (Disable) if corresponding bit value equals to 1.
                                                                   Unmasks the event (Enable) if corresponding bit value equals to 0. For bit mapping and auto mode, please see adi_adrv9025_InitTxRampDownEvents_e */
    uint32_t                    reserved;                   /*!< Reserved field */
    adi_adrv9025_TxAttenMode_e  attenMode;                  /*!< Tx attenuation mode for init time. Init time mode selection shouldn't be either GPIO or SPI2 mode */
    adi_adrv9025_DacFullScale_e dacFullScale;               /*!< Tx Dac full scale Setting. WARNING: This field is not being used actively, left for backward compatibility. It's moved to TxChannelCfg_t*/
    uint16_t txAttenInit_mdB;                               /*!< Initial Tx Attenuation. Note: This initial Tx attenuation value is programmed only if initial Tx attenuation mode is selected as SPI mode */
} adi_adrv9025_TxAttenControl_t;

/**
 *  \brief Data structure to hold Tx data path settings
 */
typedef struct adi_adrv9025_TxChannelCfg
{
    adi_adrv9025_TxProfile_t      profile; /*!< Tx datapath profile, 3dB corner frequencies, and digital filter enables */
    adi_adrv9025_TxAttenControl_t txAttenCtrl;
    adi_adrv9025_DacFullScale_e   dacFullScale; /*!< Tx Dac full scale Setting*/
} adi_adrv9025_TxChannelCfg_t;

/**
 *  \brief Data structure to hold ADRV9025 Tx Channel configuration settings
 */
typedef struct adi_adrv9025_TxSettings
{
    uint32_t                    txInitChannelMask;                         /*!< Tx channel mask of which channels to initialize */
    adi_adrv9025_TxChannelCfg_t txChannelCfg[ADI_ADRV9025_MAX_TXCHANNELS]; /*!< Tx settings per Tx channel */
} adi_adrv9025_TxSettings_t;

/**
 *  \brief Data structure to hold settings for the current Rx specific use case profile
 */
typedef struct adi_adrv9025_RxProfile
{
    adi_adrv9025_RxChannels_e      channelType;                     /*!< Channel type described by this profile (Rx/ORx/Loopback) */
    uint8_t                        rxFirDecimation;                 /*!< Rx FIR decimation (1,2,4) */
    uint8_t                        rxDec5Decimation;                /*!< Decimation of Dec5 or Dec4 filter (5,4) */
    uint8_t                        rhb1Decimation;                  /*!< RX Halfband1 (HB1) decimation. Can be either 1 or 2 */
    uint8_t                        rhb1WideBandMode;                /*!< 1 = HB1 is wider, 0 = HB1 is narrow, ORx and loopback profiles ignore this field */
    uint8_t                        rhb2Decimation;                  /*!< RX Halfband2 (HB2) decimation. Can be either 1 or 2 */
    uint8_t                        rhb3Decimation;                  /*!< RX Halfband3 (HB3) decimation. Can be either 1 or 2 */
    uint8_t                        rxFir1Decimation;                /*!< Rx FIR decimation (1,2) */
    uint8_t                        rxFir2Decimation;                /*!< Rx FIR decimation (1,2), ORx and loopback profiles ignore this field */
    uint32_t                       rxOutputRate_kHz;                /*!< Rx Output data rate in kHz */
    uint32_t                       rfBandwidth_kHz;                 /*!< Rx RF passband bandwidth for the profile */
    uint32_t                       rxBbf3dBCorner_kHz;              /*!< Rx BBF (TIA) 3dB corner in kHz */
    uint32_t                       rxAdcBandWidth_kHz;              /*!< Rx ADC bandwidth - tunes the bandwidth of the passband and noise transfer functions of the ADC */
    adi_adrv9025_RxFir_t           rxFir;                           /*!< Rx FIR filter structure */
    adi_adrv9025_RxDdc_e           rxDdcMode;                       /*!< Rx DDC mode */
    adi_adrv9025_RxNcoShifterCfg_t rxNcoShifterCfg;                 /*!< Rx NCO Shift parameters used for ZIF->RIF, CIF->ZIF, ORX only uses band A members */
    uint8_t                        tiaPowerMode;                    /*!< 5 options for TIA power reduction modes (range 0-4, where 4 = allow ARM to set based on LUT for power saving) */
    adi_adrv9025_RxDataFormat_t    rxDataFormat;                    /*!< Rx Data Format settings structure */
    int16_t                        rxAdc[ADI_ADRV9025_RXADC_COEFS]; /*!< RxAdc profile for that channel*/
} adi_adrv9025_RxProfile_t;

/**
 *  \brief Data structure to hold Tx data path settings
 */
typedef struct adi_adrv9025_RxChannelCfg
{
    adi_adrv9025_RxProfile_t profile; /*!< Rx datapath profile, 3dB corner frequencies, and digital filter enables */
} adi_adrv9025_RxChannelCfg_t;

/**
 *  \brief Data structure to hold ADRV9025 Rx Channel configuration settings
 */
typedef struct adi_adrv9025_RxSettings
{
    uint32_t                    rxInitChannelMask;                         /*!< Rx channel mask of which channels to initialize */
    adi_adrv9025_RxChannelCfg_t rxChannelCfg[ADI_ADRV9025_MAX_RXCHANNELS]; /*!< Rx settings per Rx channel */
} adi_adrv9025_RxSettings_t;

/**
 *  \brief Data structure to hold digital clock settings
 */
typedef struct adi_adrv9025_ClockSettings
{
    uint32_t deviceClock_kHz;                       /*!< Device clock frequency in kHz */
    uint32_t clkPllVcoFreq_kHz;                     /*!< CLKPLL VCO frequency in kHz */
    uint32_t serdesPllVcoFreq_kHz;                  /*!< SERDESPLL VCO frequency in kHz */
    uint8_t                  ldoSelect;             /*!< LDO select flag 0-Non-Bypass, 1-Bypass, WARNING: Damage may occur if set incorrectly for your PCB layout. */
    uint32_t                 extLoFreq1_kHz;        /*!< EXT LO frequency 1 in kHz */
    uint32_t                 extLoFreq2_kHz;        /*!< EXT LO frequency 2 in kHz */
    adi_adrv9025_PllLoMode_e rfPll1LoMode;          /*!< internal LO generation for RF LO1, internal LO can be output, or external LO can be input */
    adi_adrv9025_PllLoMode_e rfPll2LoMode;          /*!< internal LO generation for RF LO2, internal LO can be output, or external LO can be input */
    uint8_t                  rfPll1LoOutDivider;    /*!< Only valid in LO output mode.  This divider is independent from internal LO gen divider (valid 1, 2, 4, 8, 16, 32, 64, 128) */
    uint8_t                 rfPll2LoOutDivider;     /*!< Only valid in LO output mode.  This divider is independent from internal LO gen divider (valid 1, 2, 4, 8, 16, 32, 64, 128) */
    adi_adrv9025_RfPllMcs_e rfPllPhaseSyncMode;     /*!< Set RF PLL phase synchronization mode. Adds extra time to lock RF PLL when PLL frequency changed. See enum for options */
    adi_adrv9025_LoSel_e    rx12LoSelect;           /*!< Rx1/Rx2 LO select: 1 = LO1, 2 = LO2  */
    adi_adrv9025_LoSel_e    rx34LoSelect;           /*!< Rx3/Rx4 LO select: 1 = LO1, 2 = LO2  */
    adi_adrv9025_LoSel_e    tx12LoSelect;           /*!< Tx1/Tx2 LO select: 1 = LO1, 2 = LO2  */
    adi_adrv9025_LoSel_e    tx34LoSelect;           /*!< Tx3/Tx4 LO select: 1 = LO1, 2 = LO2  */
    adi_adrv9025_OrxLoSel_e orx12LoSelect;          /*!< ORx1/ORx2 LO select: 1 = TX12 LO Source, 2 = AuxLO */
    adi_adrv9025_OrxLoSel_e orx34LoSelect;          /*!< ORx3/ORx4 LO select: 1 = TX34 LO Source, 2 = AuxLO */
} adi_adrv9025_ClockSettings_t;

/**
* \brief Data Structure to hold ADRV9025 device Rx Max and Min gain indices
*/
typedef struct
{
    uint8_t rx1MinGainIndex;      /*!< Current device minimum Rx1 gain index */
    uint8_t rx1MaxGainIndex;      /*!< Current device maximum Rx1 gain index */
    uint8_t rx2MinGainIndex;      /*!< Current device minimum Rx2 gain index */
    uint8_t rx2MaxGainIndex;      /*!< Current device maximum Rx2 gain index */
    uint8_t rx3MinGainIndex;      /*!< Current device minimum Rx3 gain index */
    uint8_t rx3MaxGainIndex;      /*!< Current device maximum Rx3 gain index */
    uint8_t rx4MinGainIndex;      /*!< Current device minimum Rx4 gain index */
    uint8_t rx4MaxGainIndex;      /*!< Current device maximum Rx4 gain index */
    uint8_t orx1orx2MinGainIndex; /*!< Current device minimum Orx1 gain index */
    uint8_t orx1orx2MaxGainIndex; /*!< Current device maximum Orx1 gain index */
    uint8_t orx3orx4MinGainIndex; /*!< Current device minimum Orx3 gain index */
    uint8_t orx3orx4MaxGainIndex; /*!< Current device maximum Orx3 gain index */
} adi_adrv9025_GainIndex_t;

/**
 * \brief Data structure to hold SPI settings for all system device types
 */
typedef struct adi_adrv9025_SpiSettings
{
    uint8_t msbFirst;                                /*!< 1 = MSB First, 0 = LSB First Bit order for SPI transaction */
    uint8_t enSpiStreaming;                          /*!< Not Recommended - most registers in ADRV9025 API are not consecutive */
    uint8_t autoIncAddrUp;                           /*!< For SPI Streaming, set address increment direction. 1= next addr = addr+1, 0:addr = addr-1 */
    uint8_t fourWireMode;                            /*!< 1: Use 4-wire SPI, 0: 3-wire SPI (SDIO pin is bidirectional). NOTE: ADI's FPGA platform always uses 4-wire mode */
    adi_adrv9025_CmosPadDrvStr_e cmosPadDrvStrength; /*!< Drive strength of CMOS pads when used as outputs (SDIO, SDO, GP_INTERRUPT, GPIO 1, GPIO 0) */
} adi_adrv9025_SpiSettings_t;

#define ADC_PROFILE_MAX 56

/**
 *  \brief Data structure to hold ADRV9025 ADC Profile settings
 */
typedef struct adi_adrv9025_AdcProfiles
{
    uint16_t RxChannel1[ADC_PROFILE_MAX];          /*!< Receiver channel ADC Profile.*/
    uint16_t RxChannel2[ADC_PROFILE_MAX];          /*!< Receiver channel ADC Profile.*/
    uint16_t RxChannel3[ADC_PROFILE_MAX];          /*!< Receiver channel ADC Profile.*/
    uint16_t RxChannel4[ADC_PROFILE_MAX];          /*!< Receiver channel ADC Profile.*/
    uint16_t OrxChannel1Profile0[ADC_PROFILE_MAX]; /*!< Orx channel 1, profile number 0 .*/
    uint16_t OrxChannel1Profile1[ADC_PROFILE_MAX]; /*!< Orx channel 1, profile number 1 .*/
    uint16_t OrxChannel1Profile2[ADC_PROFILE_MAX]; /*!< Orx channel 1, profile number 2 .*/
    uint16_t OrxChannel1Profile3[ADC_PROFILE_MAX]; /*!< Orx channel 1, profile number 3 .*/
    uint16_t OrxChannel2Profile0[ADC_PROFILE_MAX]; /*!< Orx channel 2, profile number 0 .*/
    uint16_t OrxChannel2Profile1[ADC_PROFILE_MAX]; /*!< Orx channel 2, profile number 1 .*/
    uint16_t OrxChannel2Profile2[ADC_PROFILE_MAX]; /*!< Orx channel 2, profile number 2 .*/
    uint16_t OrxChannel2Profile3[ADC_PROFILE_MAX]; /*!< Orx channel 2, profile number 3 .*/
    uint8_t  OrxChannel1Index;                     /*!< /Orx Channel 1, active profile. */
    uint8_t  OrxChannel2Index;                     /*!< /Orx Channel 2, active profile. */
} adi_adrv9025_AdcProfiles_t;

/**
 * \brief Data structure to hold ADRV9025 device instance initialization settings
 */
typedef struct adi_adrv9025_Init
{
    adi_adrv9025_ClockSettings_t       clocks;        /*!< Holds settings for CLKPLL and reference clock */
    adi_adrv9025_GpInterruptSettings_t gpInterrupts;  /*!< Holds settings for default gp interrupt masks for pin 0 and 1 */
    adi_adrv9025_RxSettings_t          rx;            /*!< Rx settings data structure */
    adi_adrv9025_TxSettings_t          tx;            /*!< Tx settings data structure */
    adi_adrv9025_DataInterfaceCfg_t    dataInterface; /*!< Holds the Data Interface JESD204B/C data link settings */
    adi_adrv9025_AdcProfiles_t         adcProfiles; /*!< Rx ADC Profiles*/
} adi_adrv9025_Init_t;

/**
* \brief Data structure for ADRV9025 enable MCS Mask
*/
typedef struct adi_adrv9025_enableMcsMask
{
    uint8_t clkMcsControlMask;
    uint8_t rf1McsControlMask;
    uint8_t rf2McsControlMask;
    uint8_t auxMcsControlMask;
} adi_adrv9025_enableMcsMask_t;

/**
* \brief Data structure to hold clock divide ratios
*/
typedef struct adi_adrv9025_ClkDivideRatios
{
    uint8_t armClkDivideRatio;             /*ARM clock divide ratio w.r.t hsDigClk*/
    uint8_t agcClkDivideRatio;             /*AGC module clock divide ratio w.r.t hsDigClk*/
    uint8_t regClkDivideRatio;             /*Register bus clock divide ratio w.r.t hsDigClk*/
    uint8_t txAttenDeviceClockDivideRatio; /*Tx Atten module clock divide ratio w.r.t hsDigClk*/
} adi_adrv9025_ClkDivideRatios_t;

/**
 * \brief Data structure to hold a ADRV9025 device instance status information
 *
 * Application layer allocates this memory, but only API writes to it to keep up with the API run time state.
 */
typedef struct adi_adrv9025_Info
{
    uint8_t                  deviceSiRev;                       /*!< ADRV9025 silicon rev read during HWOpen */
    uint8_t                  deviceProductId;                   /*!< ADRV9025 Product ID read during HWOpen */
    adi_adrv9025_ApiStates_e devState;                          /*!< Current device state of the part, i.e., radio on, radio off, arm loaded, etc., defined by deviceState enum */
    uint32_t                 initializedChannels;               /*!< Holds Rx/ORx/Tx channels that were initialized and calibrated for the current device */
    uint32_t                 profilesValid;                     /*!< Current device profilesValid bit field for use notification, i.e., Tx = 0x01, Rx = 0x02, Orx = 0x04 */

    adi_adrv9025_TxAttenStepSize_e txAttenStepSize[ADI_ADRV9025_MAX_TXCHANNELS]; /*!< Current tx Atten step size for each Tx channel */
    adi_adrv9025_TxAttenMode_e     txAttenMode[ADI_ADRV9025_MAX_TXCHANNELS];     /*!< Current tx Atten mode for each Tx channel */
    uint32_t                       swTest;                                       /*!< Software testmode signal */
    uint32_t                       hsDigClk_kHz;                                 /*!< Calculated in initialize() digital clock used throughout API functions */
    adi_adrv9025_ClkDivideRatios_t clkDivideRatios;                              /*!< Clock divide ratios w.r.t hsDigClk for various modules in the device */

    uint32_t                 txInputRate_kHz[ADI_ADRV9025_MAX_TXCHANNELS];  /*!< Tx Input sample rate from currently loaded profile */
    uint32_t                 rxOutputRate_kHz[ADI_ADRV9025_MAX_RXCHANNELS]; /*!< Rx Output sample rate from currently loaded profile */
    adi_adrv9025_GainIndex_t gainIndexes;                                   /*!< Current device Rx min max gain index values */
    uint32_t                 rxAgcPeakModeMask;                             /*!< Previous AGC config peak vs power mode */
    uint16_t                 chunkStreamImageSize[12];                      /*!< Stream Image Size */
    uint16_t                 chunkStreamImageOffset[12];                    /*!< Stream Image Offset */
    uint32_t                 currentStreamBinBaseAddr;                      /*!< Address to load current stream */
    uint32_t                 currentStreamBaseAddr;                         /*!< Address to load current stream base */
    uint8_t                  currentStreamNumberStreams;                    /*!< Number of Streams for current stream  */
    uint8_t                  currentStreamImageIndex;                       /*!< Index of current stream  */
    uint32_t                 currentStreamImageSize;                        /*!< Image size of current stream */
    uint8_t                  rxFramerNp;                                    /*!< Rx Framer Np - converter sample resolution (12, 16, 24) */
    uint8_t                  rxChannel3bitSlicerMode;                       /*!< If a bit is set - corresponding Rx Channel Data format slicer ctrl is 3 bits */

    PRIVATE_TYPE_9025                 sharedResourcePool[ADRV9025_NUM_SHARED_RESOURCES]; /*!< Shared resource pool for the given device instance*/
    adi_adrv9025_GpioPinSel_e         extDpdCaptureTriggerGpio;                          /*!< External DPD capture trigger GPIO */
    uint8_t                           extDpdLutSwapModeEn;                               /*!< External DPD LUT swap mode enable setting */
    uint8_t                           linkSharingEnabled;                                /*!< Link sharing enabled if 1, disabled otherwise */
    adi_adrv9025_Cpu_t                cpu;                                               /*!< Processor related info */
    adi_adrv9025_JesdDataProfileCfg_t jesdCfg;                                           /*!< JESD Profile Configuration */
    uint32_t                          txRfBandWidth_kHz[ADI_ADRV9025_MAX_TXCHANNELS];    /*!< Tx Rf Bandwidth from currently loaded profile */
    uint32_t                          rxRfBandWidth_kHz[ADI_ADRV9025_MAX_RXCHANNELS];    /*!< Rx Rf Bandwidth from currently loaded profile */
    uint8_t                           txSrlRateInvalidMask;                              /*!< TX bitmask indicates which TX channel has invalid SRL rate */
}                                     adi_adrv9025_Info_t;

/*
* \brief Data structure to hold ADRV9025 device capability settings
*/
typedef struct adi_adrv9025_CapabilityModel
{
    uint8_t  productId;                  /*!< Current device Product Id */
    uint8_t  siRevision;                 /*!< Current device silicon version */
    uint8_t  txNumber;                   /*!< Current device number of Tx */
    uint8_t  rxNumber;                   /*!< Current device number of Rx */
    uint8_t  orxNumber;                  /*!< Current device number of Orx */
    uint32_t txChannelMask;              /*!< Current device ChannelMask of Tx */
    uint32_t rxChannelMask;              /*!< Current device ChannelMask of Rx */
    uint32_t orxChannelMask;             /*!< Current device ChannelMask of Rx */
    uint32_t featureMask;                /*!< Current device featureMask*/
    uint32_t txMaxBw_kHz;                /*!< Current device max BW for Tx */
    uint32_t rxMaxBw_kHz;                /*!< Current device max BW for Rx */
    uint32_t orxMaxBw_kHz;               /*!< Current device max BW for Orx */
    uint32_t rfFreqRangeMin_kHz;         /*!< Current device RF Freq Range Min */
    uint32_t rfFreqRangeMax_kHz;         /*!< Current device RF Freq Range Max */
    uint32_t serializerLaneEnableMask;   /*!< Mask of Hardware Enabled Serializer Lanes */
    uint32_t deserializerLaneEnableMask; /*!< Mask of Hardware Enabled Deserializer Lanes */
} adi_adrv9025_CapabilityModel_t;

/**
*  \brief Data structure to hold ADRV9025 Tx Ramp down init time configuration
*/
typedef struct adi_adrv9025_TxRampDownInitCfg
{
    uint32_t                  txRampDownEvents;              /*!< Init time Tx ramp down event masks selected through profile. For details, please see adi_adrv9025_InitTxRampDownEvents_e */
    adi_adrv9025_TxChannels_e txChannel;                     /*!< Tx channel selection to apply init time config */
    adi_adrv9025_LoSel_e      txLoSel;                       /*!< Init time LO selection for Tx channel through profile */
    uint8_t                   deserializerLanesEnabledDfrm0; /*!< Init time lane enable selection for Jesd Dfrm 0 through profile*/
    uint8_t                   deserializerLanesEnabledDfrm1; /*!< Init time lane enable selection for Jesd Dfrm 1 through profile*/
    uint32_t                  serdesPllVcoFreq_kHz;          /*!< Init time SERDES Pll Frequency through profile*/
} adi_adrv9025_TxRampDownInitCfg_t;

/**
* \brief Data structure to hold ADRV9025 device instance settings
*/
typedef struct adi_adrv9025_Device
{
    adi_common_Device_t            common;           /*!< Common layer structure */
    adi_adrv9025_Info_t            devStateInfo;     /*!< ADRV9025 run time state information container */
    adi_adrv9025_SpiSettings_t     spiSettings;      /*!< ADRV9025 SPI Settings */
    adi_adrv9025_CapabilityModel_t deviceCapability; /*!< ADRV9025 Device Capability Settings */
} adi_adrv9025_Device_t;

#ifndef UNUSED_PARA
#define UNUSED_PARA(x) (void)(x)
#endif

#ifdef __cplusplus
}
#endif

#endif
