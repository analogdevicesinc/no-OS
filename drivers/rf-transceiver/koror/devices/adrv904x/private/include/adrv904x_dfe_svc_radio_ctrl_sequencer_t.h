/**
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X Software license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adrv904x_dfe_svc_radio_ctrl_sequencer_t.h
 * \brief Contains data types for DFE SDK Radio Sequencer Service
 *
 * DFE SDK API Version: 2.9.0.4
 */

#ifndef __ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_T_H__
#define __ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_T_H__

#include "adi_adrv904x_dfe_svc_radio_ctrl_sequencer_t.h"
#include "adi_adrv904x_platform_pack.h"

/**
 *  \brief Maximum number of signals controllable by each sequencer
 */
 #define ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MAX_SIGNALS    (16)

/* The following macros define the layout of patterns in the radio sequencer image.
 * We reserve the first 32 pattern IDs for "shared" patterns, and then a block of
 * 32 pattern IDs for each sequencer:
 *
 * -----------------------------
 * 0-31: Shared patterns
 * -----------------------------
 * 32-63: Sequencer 0 patterns
 * -----------------------------
 * 64-95: Sequencer 1 patterns
 * -----------------------------
 * 96-127: Sequencer 2 patterns
 * -----------------------------
 * etc.
 *
 * If a sequencer doesn't use all of its reserved IDs, the unused IDs are mapped
 * to ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_NULL_PATTERN_GLOBAL_PTRN_ID (the null pattern).
 *
 * Then we define a simple formula for translating a "sequencer-relative" pattern ID
 * to a "global" pattern ID:
 *
 * globalId = (sequencerId * NUM_PATTERNS_PER_SEQUENCER) + sequencerRelativePtrnId + NUM_SHARED_PATTERNS
 *
 * So the global ID for sequencer 2, pattern 1 is:
 * 97 = (2 * 32) + 1 + 32
 *
 * 97 is then used as the index in the pattern lookup table to get the
 * memory address of sequencer 2 pattern 1.
 */

/**
 *  \brief Maximum number of "shared" patterns, i.e. patterns that
 *         are run from multiple sequencers (NULL pattern)
 */
#define ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_NUM_SHARED_PATTERNS           (32)

/**
 *  \brief Number of entries reserved in the global pattern table for each sequencer
 */
#define ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_NUM_PATTERNS_PER_SEQUENCER    (64)

/* NOTE: The macro below must NOT be multi-line in order for conversion scripts to pick it up */
/**
 *  \brief Translates a sequencer-relative pattern ID into a global pattern ID
 */
#define ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SEQ_PTRN_ID_TO_GLOBAL_ID(seqId, patternId)    ((((seqId) * ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_NUM_PATTERNS_PER_SEQUENCER) + (patternId)) + ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_NUM_SHARED_PATTERNS)

/**
 *  \brief List of shared patterns (using global patterns IDs)
 */
#define ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_NULL_PATTERN_GLOBAL_PTRN_ID           (0)
/* The startup pattern exists (and is NOT the first pattern) so that the
 * starting address for the radio sequencers is a non-zero value. This is
 * necessary so that we can distinguish between a radio sequencer that is still
 * in reset, and one that has started executing code.
 */
#define ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_STARTUP_PATTERN_GLOBAL_PTRN_ID        (1)

/**
 *  \brief List of per-sequencer patterns (using sequencer-relative pattern IDs)
 *         for ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_FRAME_TIMING_X
 */
#define ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_DEFAULT_FRAMING_TIMING_SEQ_PTRN_ID    (0)

/**
 *  \brief List of per-sequencer patterns (using sequencer-relative pattern IDs)
 *         for ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ORX_X_CAPTURE
 */
#define ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_DPD_VSWR_SEQ_PTRN_ID                  (0)
#define ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_CLGC_SEQ_PTRN_ID                      (1)

/**
 *  \brief Maximum number of patterns
 */
#define ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_PTRN_MAX_NUM_PATTERNS                 (2048)

/**
 *  \brief Maximum number of entries in the pattern lookup table
 */
#define ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_LOOKUP_TABLE_MAX_ENTRIES              (2048U)

/**
 * \brief Enumeration for the radio seq GPRs numbers
 *
 */
typedef enum
{
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_GPR_0       = 0,             /*!< ID for Radio Sequencer GPR 0  */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_GPR_1       = 1,             /*!< ID for Radio Sequencer GPR 1  */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_GPR_2       = 2,             /*!< ID for Radio Sequencer GPR 2  */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_GPR_3       = 3,             /*!< ID for Radio Sequencer GPR 3  */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_GPR_4       = 4,             /*!< ID for Radio Sequencer GPR 4  */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_GPR_5       = 5,             /*!< ID for Radio Sequencer GPR 5  */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_GPR_6       = 6,             /*!< ID for Radio Sequencer GPR 6  */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_GPR_7       = 7,             /*!< ID for Radio Sequencer GPR 7  */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_GPR_8       = 8,             /*!< ID for Radio Sequencer GPR 8  */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_GPR_9       = 9,             /*!< ID for Radio Sequencer GPR 9  */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_GPR_10      = 10,            /*!< ID for Radio Sequencer GPR 10 */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_GPR_11      = 11,            /*!< ID for Radio Sequencer GPR 11 */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_GPR_12      = 12,            /*!< ID for Radio Sequencer GPR 12 */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_GPR_13      = 13,            /*!< ID for Radio Sequencer GPR 13 */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_GPR_14      = 14,            /*!< ID for Radio Sequencer GPR 14 */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_GPR_15      = 15,            /*!< ID for Radio Sequencer GPR 15 */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_NUM_OF_GPRS = 16             /*!< Total number of GPRs  */
} adrv904x_DfeSvcRadioCtrlSequencerGpr_e;

/**
 * \brief Enumeration for the radio seq Tx functions
 *
 */
typedef enum
{
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_TX_FUNC_VSWR_ON                 = 0,    /*!< TX Func: TX DFE VSWR on           */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_TX_FUNC_TX_ATTEN_UPDATE_TRIGGER = 1,    /*!< TX Func: TX attenuation update    */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_TX_FUNC_TSSI_1_MEAS_RESET       = 2,    /*!< TX Func: TSSI 1 measurement reset */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_TX_FUNC_TSSI_2_MEAS_RESET       = 3,    /*!< TX Func: TSSI 2 measurement reset */
} adrv904x_DfeSvcRadioCtrlSequencerTxFuncId_e;

/**
 * \brief Enumeration for the radio seq GPRs numbers
 *
 */
typedef enum
{
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ORX_FUNC_TSSI_MEAS_ENABLE = 0,          /*!< ORX Func: TSSI measurement enable */
} adrv904x_DfeSvcRadioCtrlSequencerOrxFuncId_e;

/**
 * \brief Enumeration for the radio seq GPRs numbers
 *
 */
typedef enum
{
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_TX_SIGNAL_ID_0       = 0,       /*!< TX signal ID 0       */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_TX_SIGNAL_ID_1       = 1,       /*!< TX signal ID 1       */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_TX_SIGNAL_ID_2       = 2,       /*!< TX signal ID 2       */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_TX_SIGNAL_ID_3       = 3,       /*!< TX signal ID 3       */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_TX_SIGNAL_ID_4       = 4,       /*!< TX signal ID 4       */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_TX_SIGNAL_ID_5       = 5,       /*!< TX signal ID 5       */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_TX_SIGNAL_ID_6       = 6,       /*!< TX signal ID 6       */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_TX_SIGNAL_ID_7       = 7,       /*!< TX signal ID 7       */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_TX_SIGNAL_ID_INVALID = 8,       /*!< TX signal ID INVALID */
} adrv904x_DfeSvcRadioCtrlSequencerTxSignalId_e;

/**
 * \brief Enumeration for the radio seq GPRs numbers
 *
 */
typedef enum
{
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ORX_SIGNAL_ID_0       = 0,      /*!< ORX signal ID 0       */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ORX_SIGNAL_ID_1       = 1,      /*!< ORX signal ID 1       */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ORX_SIGNAL_ID_2       = 2,      /*!< ORX signal ID 2       */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ORX_SIGNAL_ID_3       = 3,      /*!< ORX signal ID 3       */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ORX_SIGNAL_ID_4       = 4,      /*!< ORX signal ID 4       */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ORX_SIGNAL_ID_5       = 5,      /*!< ORX signal ID 5       */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ORX_SIGNAL_ID_6       = 6,      /*!< ORX signal ID 6       */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ORX_SIGNAL_ID_7       = 7,      /*!< ORX signal ID 7       */
    ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ORX_SIGNAL_ID_INVALID = 8,      /*!< ORX signal ID INVALID */
} adrv904x_DfeSvcRadioCtrlSequencerOrxSignalId_e;

/**
 * \brief Radio sequencer initialization data populated
 *        by the BBIC for the DFE via adrv904x_DfeMemInitTemp_t
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcRadioCtrlSequencerInitData
{
    uint32_t sequenceLookupTable[ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_LOOKUP_TABLE_MAX_ENTRIES]; /*!< Pattern address lookup table */
    uint32_t numberPatterns;                                                              /*!< Number of entries in sequenceLookupTable[] */
    uint32_t sequenceLookupTableCrc;                                                      /*!< CRC of sequenceLookupTable[] */
} adrv904x_DfeSvcRadioCtrlSequencerInitData_t;
ADI_ADRV904X_PACK_FINISH

#endif /* __ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_T_H__ */


