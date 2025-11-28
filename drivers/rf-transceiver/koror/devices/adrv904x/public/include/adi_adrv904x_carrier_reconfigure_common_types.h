/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2023 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adi_adrv904x_carrier_reconfigure_common_types.h
 *
 * \brief   Contains ADRV904X Carrier Reconfigure Common data structures.
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef __ADI_ADRV904X_CARRIER_RECONFIGURE_TYPES_COMMON_H__
#define __ADI_ADRV904X_CARRIER_RECONFIGURE_TYPES_COMMON_H__

#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_version_types.h"
#include "adi_adrv904x_cpu_error_codes_types.h"

/**
 *  \brief Total number of allowable links that can accommodate carrier data
 */
#define ADI_ADRV904X_MAX_CARRIER_LINKS             (2U)

/**
 *  \brief Total number of CC XBAR indices available for each link
 */
#define ADI_ADRV904X_MAX_CARRIER_SLOTS             (192U)

/**
 *  \brief Total number of allowable filter coefficients for all carriers
 */
#define ADI_ADRV904X_NUM_CF_COEFFICIENTS           (648U)

/**
 *  \brief Max number of coefficients loaded per firmware transaction
 */
#define ADI_ADRV904X_CHAN_FILTER_COEFF_LOAD_LEN    (256U)

/**
 *  \brief Total number of allowable carriers per channel
 */
#define ADI_ADRV904X_MAX_CARRIERS                  (8U)

/**
 *  \brief Enum of Channel Filter applications
 */
typedef enum adi_adrv904x_CarrierFilterApplicationType
{
    ADI_ADRV904X_CARRIER_FILTER_5G,
    ADI_ADRV904X_CARRIER_FILTER_LTE,
    ADI_ADRV904X_CARRIER_FILTER_HIGH_BW_5G,
    ADI_ADRV904X_CARRIER_FILTER_LTE_IOT
} adi_adrv904x_CarrierFilterApplicationType_e;

/**
 * \brief Enum to hold values when configuring adi_adrv904x_CarrierXbarCfg_t struct
 */
typedef enum adi_adrv904x_JesdComponentCarrierXbar
{
    ADI_ADRV904X_CARRIER_0_DATA_I       = 0,
    ADI_ADRV904X_CARRIER_0_DATA_Q       = 1,
    ADI_ADRV904X_CARRIER_1_DATA_I       = 2,
    ADI_ADRV904X_CARRIER_1_DATA_Q       = 3,
    ADI_ADRV904X_CARRIER_2_DATA_I       = 4,
    ADI_ADRV904X_CARRIER_2_DATA_Q       = 5,
    ADI_ADRV904X_CARRIER_3_DATA_I       = 6,
    ADI_ADRV904X_CARRIER_3_DATA_Q       = 7,
    ADI_ADRV904X_CARRIER_4_DATA_I       = 8,
    ADI_ADRV904X_CARRIER_4_DATA_Q       = 9,
    ADI_ADRV904X_CARRIER_5_DATA_I       = 10,
    ADI_ADRV904X_CARRIER_5_DATA_Q       = 11,
    ADI_ADRV904X_CARRIER_6_DATA_I       = 12,
    ADI_ADRV904X_CARRIER_6_DATA_Q       = 13,
    ADI_ADRV904X_CARRIER_7_DATA_I       = 14,
    ADI_ADRV904X_CARRIER_7_DATA_Q       = 15,
    ADI_ADRV904X_CARRIER_8_DATA_I       = 16,
    ADI_ADRV904X_CARRIER_8_DATA_Q       = 17,
    ADI_ADRV904X_CARRIER_9_DATA_I       = 18,
    ADI_ADRV904X_CARRIER_9_DATA_Q       = 19,
    ADI_ADRV904X_CARRIER_10_DATA_I      = 20,
    ADI_ADRV904X_CARRIER_10_DATA_Q      = 21,
    ADI_ADRV904X_CARRIER_11_DATA_I      = 22,
    ADI_ADRV904X_CARRIER_11_DATA_Q      = 23,
    ADI_ADRV904X_CARRIER_12_DATA_I      = 24,
    ADI_ADRV904X_CARRIER_12_DATA_Q      = 25,
    ADI_ADRV904X_CARRIER_13_DATA_I      = 26,
    ADI_ADRV904X_CARRIER_13_DATA_Q      = 27,
    ADI_ADRV904X_CARRIER_14_DATA_I      = 28,
    ADI_ADRV904X_CARRIER_14_DATA_Q      = 29,
    ADI_ADRV904X_CARRIER_15_DATA_I      = 30,
    ADI_ADRV904X_CARRIER_15_DATA_Q      = 31,
    ADI_ADRV904X_CARRIER_UNUSED_CC_XBAR = 128
} adi_adrv904x_JesdComponentCarrierXbar_e;

/**
 * \brief Data structure to hold Carrier Channel Filter Applications
 *        2-D array where first index maps to the profile and the second index is the carrier
 *        Each enabled carrier in each valid profile should have a valid application select
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_CarrierChannelFilterApplicationSel
{
    adi_adrv904x_CarrierFilterApplicationType_e channelFilterApplicationSel[ADI_ADRV904X_MAX_CARRIERS]; /*!< Application select for each carrier in each profile */
} adi_adrv904x_CarrierChannelFilterApplicationSel_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold Carrier reconfigure settings
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_CarrierCfg
{
    uint8_t  enable;                    /*!< Set to 1 to enable; 0 to disable, this carrier */
    uint32_t sampleRate_kHz;            /*!< Sample frequency in KHz for this carrier */
    uint32_t ibw_kHz;                   /*!< IBW frequency in KHz for this carrier */
    uint32_t centerFrequency_kHz;       /*!< Center frequency in KHz for this carrier */
    uint16_t absLatencyOverride_ns;     /*!< Option to specify a custom target for carrier's absolute latency. Set to 0 to ignore and use default latency */
} adi_adrv904x_CarrierCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold settings needed for runtime carrier operation
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_CarrierRuntime
{
    uint8_t  carrierEnable[ADI_ADRV904X_MAX_CARRIERS];             /*!<   Carrier enable flag */
    uint8_t  bandSelect[ADI_ADRV904X_MAX_CARRIERS];                /*!<   Band select */
    uint8_t  mixerEnable[ADI_ADRV904X_MAX_CARRIERS];               /*!<   Mixer enable flag */
    uint32_t decimationRatio[ADI_ADRV904X_MAX_CARRIERS];           /*!<   JESD interface decimation ratio */
    uint32_t interpolationRatio[ADI_ADRV904X_MAX_CARRIERS];        /*!<   JESD interface interpolation ratio */
    uint32_t ncoFreq_kHz[ADI_ADRV904X_MAX_CARRIERS];               /*!<   NCO frequency */
    uint32_t carrierRateRatio[ADI_ADRV904X_MAX_CARRIERS];          /*!<   JESD carrier rate ratio */
    uint32_t outputRate_kHz[ADI_ADRV904X_MAX_CARRIERS];            /*!<   Carrier output rate in relation to band */
} adi_adrv904x_CarrierRuntime_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold carrier xbar entry settings
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_CarrierXbarCfg
{
    uint8_t                                 channelSelect;      /*!< Channel select: 0 -> Channel 0, 1 -> Channel 1, etc */
    adi_adrv904x_JesdComponentCarrierXbar_e carrierSelect;      /*!< Carrier select */
    uint16_t                                sampleSelect;       /*!< Sample index. Number of samples per frame = Link rate / carrier sample rate */
} adi_adrv904x_CarrierXbarCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold carrier link settings
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_LinkCfg
{
    adi_adrv904x_CarrierXbarCfg_t carrierXbarCfg[ADI_ADRV904X_MAX_CARRIER_SLOTS];
} adi_adrv904x_LinkCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold carrier JESD settings
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_CarrierJesdCfg
{
    adi_adrv904x_LinkCfg_t linkCfg[ADI_ADRV904X_MAX_CARRIER_LINKS];     /*!< Holds JESD settings if required based on reconfig settings */
} adi_adrv904x_CarrierJesdCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold Carrier data for dynamic reconfiguration
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_CarrierRadioCfg
{
    uint32_t                  channelMask;                              /*!< Channel Mask selection */
    adi_adrv904x_CarrierCfg_t carriers[ADI_ADRV904X_MAX_CARRIERS];      /*!< Array of settings for all possible carriers */
} adi_adrv904x_CarrierRadioCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure for hold Carrier data for dynamic reconfiguration 
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_CarrierRadioCfgCmd
{
    adi_adrv904x_CarrierRadioCfg_t  carrierRadioCfg;                    /*!< carrierRadioCfg passed into the BBIC API */
    uint8_t                         apply;                              /*!< Set to 1 to apply config to HW; 0 to skip and leave HW unchanged */
} adi_adrv904x_CarrierRadioCfgCmd_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold Carrier data for dynamic reconfiguration
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_ChannelFilterCarrierCfg
{
    uint16_t numberOfFilterTaps[ADI_ADRV904X_MAX_CARRIERS];      /*!< Number of filter taps per carrier */
    uint8_t  asymmetricFilterTaps[ADI_ADRV904X_MAX_CARRIERS];    /*!< Asymmetric flag for each carrier */
} adi_adrv904x_ChannelFilterCarrierCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold Carrier data for dynamic reconfiguration
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_ChannelFilterOutputCfg
{
    uint16_t numberOfFilterTaps[ADI_ADRV904X_MAX_CARRIERS];      /*!< Number of filter taps per carrier */
    uint16_t oddFilterTaps[ADI_ADRV904X_MAX_CARRIERS];           /*!< Odd Filter Taps flag (0 or 1) per carrier */
    uint16_t dataPipeStop[ADI_ADRV904X_MAX_CARRIERS];            /*!< Channel Filter Data pipe Stop (End Bank) per carrier */
    uint16_t bypassFilter[ADI_ADRV904X_MAX_CARRIERS];            /*!< Channel Filter Bypass value per carrier */
} adi_adrv904x_ChannelFilterOutputCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold Carrier data for dynamic reconfiguration
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_ChannelFilterCfg
{
    int16_t                                coeffs[ADI_ADRV904X_NUM_CF_COEFFICIENTS];    /*!< Coefficient table for carriers. Zero-pad as necessary */
    adi_adrv904x_ChannelFilterCarrierCfg_t carrierFilterCfg;                            /*!< Holds channel filter config for each carrier */
} adi_adrv904x_ChannelFilterCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Channel Filter load
 */
ADI_ADRV904X_PACKED(
    typedef struct adi_adrv904x_ChannelFilterLoad
{
    uint16_t coeffIdx;               /*!< Starting index where to place the coefficients */
    uint16_t coeffNum;               /*!< Number of coefficients in payload */

    /* Debug data payload follows command header.
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * int16_t coeffs[];
     */
} adi_adrv904x_ChannelFilterLoad_t; )

/**
 * \brief Channel Filter load
 *        Used to determine the maximum debug payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef struct adi_adrv904x_ChannelFilterLoadMaxSize
{
    adi_adrv904x_ChannelFilterLoad_t channelFilterLoad;
    int16_t                          coeffs[ADI_ADRV904X_CHAN_FILTER_COEFF_LOAD_LEN];    /*!< Debug data */
} adi_adrv904x_ChannelFilterLoadMaxSize_t;

/**
 * \brief Carrier channel filter load response used to check for FW errors
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_ChannelFilterLoadResp
{
    adi_adrv904x_CpuErrorCode_t status;              /*!< CPU error status code */
} adi_adrv904x_ChannelFilterLoadResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Carrier channel filter load response used to check for FW errors
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_ChannelFilterResp
{
    adi_adrv904x_CpuErrorCode_t           status;              /*!< CPU error status code */
    adi_adrv904x_ChannelFilterOutputCfg_t carrierFilterOutCfg; /*!< Output of carrier filter calculation */
} adi_adrv904x_ChannelFilterResp_t;
ADI_ADRV904X_PACK_FINISH

#endif /* __ADI_ADRV904X_CARRIER_RECONFIGURE_TYPES_COMMON_H__ */


