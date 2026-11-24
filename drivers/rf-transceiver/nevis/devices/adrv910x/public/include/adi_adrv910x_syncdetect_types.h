/**
 * \file
 * \brief Contains ADRV910X Sync Detection data types
 *
 * Copyright (c) 2021 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADRV910X_SYNCDETECT_TYPES_H_
#define _ADI_ADRV910X_SYNCDETECT_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "adi_adrv910x_user.h"

#define SYNC_VECTOR_MAX 14

/**
 * \brief SYNC Search detection algorithm modes
 */
typedef enum adi_adrv910x_SyncDetect_Mode
{
    ADI_ADRV910X_SYNCDETECT_MODE_DMR,        /*!< SYNC Detection for DMR */
    ADI_ADRV910X_SYNCDETECT_MODE_P25_PHASE1, /*!< Sync Detection for P25 */
    ADI_ADRV910X_SYNCDETECT_MODE_NXDN,       /*!< Sync Detection for NXDN */
    ADI_ADRV910X_SYNCDETECT_MODE_TETRA1      /*!< Sync Detection for TETRA1 */
} adi_adrv910x_SyncDetect_Mode_e;

/**
 * \brief Data path selection for sync detection
 */
typedef enum syncDetectionDataPath
{
    ADI_ADRV910X_SYNCDETECT_DP1 = 0,    /*!< Datapath 1 */
    ADI_ADRV910X_SYNCDETECT_DP2 = 1     /*!< Datapath 2 */
} syncDetectionDataPath_e;

/**
 * \brief Data structure to hold Sync Detect correlator vector
 */
typedef struct adi_adrv910x_SyncDetect_SyncDetectVectorCfg
{
    uint64_t patternVect[SYNC_VECTOR_MAX];  /*!< Bits for each sync pattern to be written to hardware (+/-1 bits, not symbol dibits) */
    uint32_t zeroMask[SYNC_VECTOR_MAX];     /*!< Bits for the corresponding patternVect that will be 0'd off in the correlation */
    uint16_t vectorMask;                    /*!< Mask of which of the 14 patternVect's are active */
    uint16_t qMask;                         /*!< Mask of which of the 14 patternVect's take the Q channel as input instead of I */
    uint8_t patternWeight[SYNC_VECTOR_MAX]; /*!< The weight of the symbols in the corresponding patternVect */
} adi_adrv910x_SyncDetect_SyncDetectVectorCfg_t;

/**
 * \brief Data structure to hold SYNC search configuration
 */
typedef struct adi_adrv910x_SyncDetect_SyncSearchCfg {
    uint32_t pathDelay;                                                         /*!< Path delay from RxDmrPd calibration, 0-2047 samples */
    int32_t  powerThreshold_dBm;                                                /*!< S9.23 Power threshold in dBm, Expected negative value*/
    uint32_t detectMultiplier;                                                  /*!< U1.31, Minimum correlation detection multiplier, ranging from 0 to 1 (Suggested closer to 0 for ERR algorithm to work the best)
                                                                                       \note May read back a slightly different value than written due to floating point conversion error */
    uint16_t preCount;                                                           /*!< Number of samples power must be above threshold for sync to be detected */
    uint16_t postCount;                                                          /*!< Number of samples power must be above threshold after metric meets power threshold 
                                                                                       for sync to be detected */
	adi_adrv910x_SyncDetect_Mode_e  algoMode;                                    /*!<  0:  Mode of SYNC Search algorithm, 0-DMR 1-P25 2-NXDN and 3-TETRA1*/
	syncDetectionDataPath_e dataPath;											/*!< Data path selection for sync detection */
} adi_adrv910x_SyncDetect_SyncSearchCfg_t;

/**
 * \brief Data structure to hold the results of a continous sync detection
 */
typedef struct adi_adrv910x_SyncDetect_SyncDetectionResult
{
    int32_t carrier_freq_offset_hz;  /*!< Carrier frequency offset in Hz */
    uint32_t sync_sample_offset;     /*!< Number of samples offset from */
    int32_t sync_correlator_vals[5]; /*!< Outputs of correlator */
    uint16_t sync_correlator_num;    /*!< Correlator number */
    bool syncDetected;               /*!< True if a sync has been detected */
    uint8_t padding;
} adi_adrv910x_SyncDetect_SyncDetectionResult_t;

#endif /* _ADI_ADRV910X_SYNCDETECT_TYPES_H_ */
