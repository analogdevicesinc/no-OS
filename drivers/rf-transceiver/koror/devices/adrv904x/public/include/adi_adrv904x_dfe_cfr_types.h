/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_dfe_cfr_types.h
*
* \brief Contains ADRV904X data types for on board dfe cfr feature
*
* ADRV904X API Version: 2.9.0.4
*/


#ifndef _ADI_ADRV904X_DFE_CFR_TYPES_H_
#define _ADI_ADRV904X_DFE_CFR_TYPES_H_



#include "adi_adrv904x_user.h"
#include "adi_library_types.h"
#include "adi_adrv904x_platform_pack.h"

#define ADI_ADRV904X_NUM_OF_CFR_ENGINES 3                 /*!< Number of CFR engines  */
#define ADI_ADRV904X_MAX_CFR_CORRECTION_PULSE_LEN 1024    /*!< Max CFR Pulse length  */

#define ADI_ADVRV904X_TX0_PULSE_RAM_BASEADDR 0x608D2000U  /*!< Base Address of Pulse RAM for TX0 */
#define ADI_ADVRV904X_TX1_PULSE_RAM_BASEADDR 0x609D2000U  /*!< Base Address of Pulse RAM for TX1 */
#define ADI_ADVRV904X_TX2_PULSE_RAM_BASEADDR 0x60AD2000U  /*!< Base Address of Pulse RAM for TX2 */
#define ADI_ADVRV904X_TX3_PULSE_RAM_BASEADDR 0x60BD2000U  /*!< Base Address of Pulse RAM for TX3 */
#define ADI_ADVRV904X_TX4_PULSE_RAM_BASEADDR 0x60CD2000U  /*!< Base Address of Pulse RAM for TX4 */
#define ADI_ADVRV904X_TX5_PULSE_RAM_BASEADDR 0x60DD2000U  /*!< Base Address of Pulse RAM for TX5 */
#define ADI_ADVRV904X_TX6_PULSE_RAM_BASEADDR 0x60ED2000U  /*!< Base Address of Pulse RAM for TX6 */
#define ADI_ADVRV904X_TX7_PULSE_RAM_BASEADDR 0x60FD2000U  /*!< Base Address of Pulse RAM for TX7 */


#define ADI_ADRV904X_CFR_PULSE_RAM_SIZE 4096U             /*!< CFR Pulse RAM size */
#define ADI_ADRV904X_CFR_PULSE_RAM_ROW_SAMPLE_SIZE 4U     /*!< CFR Pulse RAM row sample size */
#define ADI_ADRV904X_CFR_PULSE_RAM_ROW_SIZE  (ADI_ADRV904X_CFR_PULSE_RAM_ROW_SAMPLE_SIZE * 4U)                       /*!< CFR Pulse RAM row size (bytes) */
#define ADI_ADRV904X_CFR_PULSE_RAM_NUM_OF_ROWS ADI_ADRV904X_CFR_PULSE_RAM_SIZE / ADI_ADRV904X_CFR_PULSE_RAM_ROW_SIZE /*!< Number of CFR Pulse RAM rows   */
#define ADI_ADRV904X_CFR_NUM_OF_PULSE_RAMS 6U             /*!< Number of CFR Pulse RAMs */


/**
 * \brief Cfr Pulse Ram selection. If pulse RAMs are not being used, user 
 * might select single shot mode to broadcast the pulse data to save time.
 */
typedef enum adi_adrv904x_CfrPulseRamSel
{
    ADI_ADRV904X_CFR_PULSE_RAM_SINGLESHOT = 0x0001,                 /*!< Write the pulse data to all RAMs at the same time by broadcasting */
    ADI_ADRV904X_CFR_PULSE_RAM_0 = 0x0002,                          /*!< Write the pulse data to RAM0 through shadow RAM */
    ADI_ADRV904X_CFR_PULSE_RAM_1 = 0x0004,                          /*!< Write the pulse data to RAM1 through shadow RAM */
    ADI_ADRV904X_CFR_PULSE_RAM_2 = 0x0008,                          /*!< Write the pulse data to RAM2 through shadow RAM */
    ADI_ADRV904X_CFR_PULSE_RAM_3 = 0x0010,                          /*!< Write the pulse data to RAM3 through shadow RAM */
    ADI_ADRV904X_CFR_PULSE_RAM_4 = 0x0020,                          /*!< Write the pulse data to RAM4 through shadow RAM */
    ADI_ADRV904X_CFR_PULSE_RAM_ALL = 0x003E                         /*!< This selects all RAM blocks to be written through shadow RAM one at a time */
} adi_adrv904x_CfrPulseRamSel_e;



/**
 * \brief Cfr Config set selection. Each engine has 2 separate configuration set which
 * can be configured and switched to on the fly
 */
typedef enum adi_adrv904x_CfrConfigSel
{
    ADI_ADRV904X_CFR_CFG0 = 0x0001,         /*!< Select config0 set for CFR engine */
    ADI_ADRV904X_CFR_CFG1 = 0x0002,         /*!< Select config1 set for CFR engine */
    ADI_ADRV904X_CFR_CFGALL = 0x0003,       /*!< Select both configs for CFR engine */
}adi_adrv904x_CfrConfigSel_e;

/**
 * \brief Cfr Pulse selection. Each engine can store 2 different pulses and user can select
 * the pulse to be used through adi_adrv904x_CfrActivePulseSet function 
 */
typedef enum adi_adrv904x_CfrPulseSel
{
    ADI_ADRV904X_CFR_PULSE0 = 0x0001,   /*!< Select pulse 0 to be used by CFR engine */
    ADI_ADRV904X_CFR_PULSE1 = 0x0002,   /*!< Select pulse 1 to be used by CFR engine */
    ADI_ADRV904X_CFR_PULSEALL = 0x0003  /*!< Select both pulses */
}adi_adrv904x_CfrPulseSel_e;

/**
 * \brief Cfr Threshold config structure
 */
typedef struct adi_adrv904x_CfrThrConfig
{
    uint32_t cfrPeakThreshold;              /*!< Cfr base threshold value to be scaled with peak and correction thresholds. Value to be passed to API can be calculated as
                                                      cfrPeakThreshold = 10^(Target PAR_dB * RMS_Input_dBFS / 20) * 2^16 . Range[0 - 64*2^16] */
    uint32_t cfrEngine0PeakThresholdScaler; /*!< Cfr Engine 1 Peak threshold scaler to be applied to cfrPeakThreshold. Range[0 - 2^16] where 2^16 corresponds to a scaler
                                                      value of 1 and 0 corresponds to a scaler value of 0 */
    uint32_t cfrEngine1PeakThresholdScaler; /*!< Cfr Engine 2 Peak threshold scaler to be applied to cfrPeakThreshold. Range[0 - 2^16] where 2^16 corresponds to a scaler
                                                      value of 1 and 0 corresponds to a scaler value of 0 */
    uint32_t cfrEngine2PeakThresholdScaler; /*!< Cfr Engine 3 Peak threshold scaler to be applied to cfrPeakThreshold. Range[0 - 2^16] where 2^16 corresponds to a scaler
                                                      value of 1 and 0 corresponds to a scaler value of 0 */
    uint32_t cfrCorrectionThresholdScaler; /*!< Cfr correction threshold scaler to be applied to cfrPeakThreshold. Correction threshold should always be equal or lower
                                                      than peak detection thresholds. i.e. cfrCorrectionThresholdScaler should be lower than other peak detection scalers.
                                                      Range[0 - 2^16] where 2^16 corresponds to a scaler value of 1 and 0 corresponds to a scaler value of 0 */
} adi_adrv904x_CfrThrConfig_t;

/**
 * \brief Cfr Configuration structure 
 */
typedef struct adi_adrv904x_CfrConfig
{
    adi_adrv904x_CfrThrConfig_t cfrThreshold;     /*!< CFR engine detection and correction thresholds. For details please check adi_adrv904x_CfrThrConfig_t */
    uint32_t cfrEngineEnable[ADI_ADRV904X_NUM_OF_CFR_ENGINES]; /*!< Engine enable selection array for 3 engines. 1:Enable, 0:Disable */
    uint32_t cfrEngineBypass[ADI_ADRV904X_NUM_OF_CFR_ENGINES]; /*!< Engine bypass selection array for 3 engines. 1:Enable, 0:Disable */
} adi_adrv904x_CfrConfig_t;




/**
 * \brief Cfr Statistics. For details please check adi_adrv904x_CfrStatisticsGet function
 */
typedef struct adi_adrv904x_CfrStatistics
{
    uint16_t cfrEngine1PeaksDetected;        /*!<  Number of peaks detected at engine 1 */
    uint8_t cfrEngine1PeaksSkippedWFifo;    /*!<  Number of peaks skipped at engine 1 weight FIFO */
    uint8_t cfrEngine1PeaksSkippedPFifo;    /*!<  Number of peaks skipped at engine 1 peaks FIFO */
    uint16_t cfrEngine1PeaksSkippedCPC;      /*!<  Number of peaks skipped at engine 1 CPC module */
    uint16_t cfrEngine2PeaksDetected;        /*!<  Number of peaks detected at engine 2 */
    uint8_t cfrEngine2PeaksSkippedWFifo;    /*!<  Number of peaks skipped at engine 2 weight FIFO */
    uint8_t cfrEngine2PeaksSkippedPFifo;    /*!<  Number of peaks skipped at engine 2 peaks FIFO */
    uint16_t cfrEngine2PeaksSkippedCPC; /*!<  Number of peaks skipped at engine 2 CPC module */
    uint16_t cfrEngine3PeaksDetected;        /*!<  Number of peaks detected at engine 3 */
    uint8_t cfrEngine3PeaksSkippedWFifo;    /*!<  Number of peaks skipped at engine 3 weight FIFO */
    uint8_t cfrEngine3PeaksSkippedPFifo;    /*!<  Number of peaks skipped at engine 3 peaks FIFO */
    uint16_t cfrEngine3PeaksSkippedCPC; /*!<  Number of peaks skipped at engine 3 CPC module */
} adi_adrv904x_CfrStatistics_t;

/**
 * \brief Cfr statistics configuration structure
 */
typedef struct adi_adrv904x_CfrStatisticsCfg
{
    uint8_t windowEn;                       /*!< Windowed statitstics enable/disable selection. 1: Enable // 0: Disable. Wnen windowed statistics is disabled
                                                 user should always read the statistics of current window. Please see adi_adrv904x_CfrStatisticsGet for details */
    uint8_t windowSize;                     /*!< When windowed statistics enabled (i.e. windowEn = 1)  this parameter determines the size of the window
                                                 of which CFR HW use to collect statistics. Each time this window expires, CFR HW will automatically transfer
                                                 all the statisitcs to previous window and clear the current window counters. Window size is calculated in terms
                                                 of peak detection clock cycles. Window size = 2^(windowSize + 7) peak detect clock cycles. Range[0-15] */
}adi_adrv904x_CfrStatisticsCfg_t;

/**
 * \brief Cfr pulse data
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_CfrCorrectionPulse
{
    int16_t coeffRealHalfPulse[ADI_ADRV904X_MAX_CFR_CORRECTION_PULSE_LEN];         /*!< An array consisting of the first half of the Real part of the complex CFR correction pulse coefficients */
    int16_t coeffImagHalfPulse[ADI_ADRV904X_MAX_CFR_CORRECTION_PULSE_LEN];         /*!< An array consisting of the first half of the Imag part of the complex CFR correction pulse coefficients */
    uint16_t numCoeffs;                                                            /*!< No. of coefficients contained in coeffReal and coeffImaginary arrays */
    uint8_t pulseSelectMask;                                                       /*!< This field selects which pulses to write. adi_adrv904x_CfrPulseSel_e should be used to create this mask. 
                                                                                        User can select multiple pulses */
    uint8_t ramSelectMask;                                                         /*!< Selects the RAM's to write the pulse data. adi_adrv904x_CfrPulseRamSel_e should be used to create this mask.
                                                                                        Please check adi_adrv904x_CfrPulseRamSel_e for detailed info */ 
}adi_adrv904x_CfrCorrectionPulse_t;
ADI_ADRV904X_PACK_FINISH

#endif /* _ADI_ADRV904X_DFE_CFR_TYPES_H_ */