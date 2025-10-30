/**
 * \file adrv903x_cpu_cmd_getset_nco.h
 * 
 * \brief   Command definition for:
 *          ADRV903X_CPU_CMD_ID_GET_RX_NCO, 
 *          ADRV903X_CPU_CMD_ID_SET_RX_NCO,
 *          ADRV903X_CPU_CMD_ID_GET_ORX_NCO,
 *          ADRV903X_CPU_CMD_ID_SET_ORX_NCO,
 *          ADRV903X_CPU_CMD_ID_GET_TX_MIX_NCO,
 *          ADRV903X_CPU_CMD_ID_SET_TX_MIX_NCO,
 *          ADRV903X_CPU_CMD_ID_GET_TX_TEST_NCO,
 *          ADRV903X_CPU_CMD_ID_SET_TX_TEST_NCO
 *
 * \details Command definition for:
 *          ADRV903X_CPU_CMD_ID_GET_RX_NCO, 
 *          ADRV903X_CPU_CMD_ID_SET_RX_NCO,
 *          ADRV903X_CPU_CMD_ID_GET_ORX_NCO,
 *          ADRV903X_CPU_CMD_ID_SET_ORX_NCO,
 *          ADRV903X_CPU_CMD_ID_GET_TX_MIX_NCO,
 *          ADRV903X_CPU_CMD_ID_SET_TX_MIX_NCO,
 *          ADRV903X_CPU_CMD_ID_GET_TX_TEST_NCO,
 *          ADRV903X_CPU_CMD_ID_SET_TX_TEST_NCO
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_GETSET_NCO_H__
#define __ADRV903X_CPU_CMD_GETSET_NCO_H__

#include "adi_adrv903x_rx_nco.h"
#include "adi_adrv903x_tx_nco.h"
#include "adrv903x_cpu_error_codes_types.h"

/* SET_RX_NCO command structure is defined in adi_adrv903x_rx_nco.h */

/**
 * \brief SET_RX_NCO command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_RxNcoConfigResp
{
    adrv903x_CpuErrorCode_e status;  /*!< CPU error status code */
    uint8_t                 chanSelect; /*!< Channel mask of channels set */
} adrv903x_RxNcoConfigResp_t;)

/**
 * \brief GET_RX_NCO command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_RxNcoConfigReadback
{
    uint8_t                   chanSelect;             /*!< Rx channel (bit mapped) */
    adi_adrv903x_DdcNumber_t  bandSelect;             /*!< which band */
} adrv903x_RxNcoConfigReadback_t;)

/* GET_RX_NCO command response structure is defined in adi_adrv903x_rx_nco.h */

/* SET_ORX_NCO command structure is defined in adi_adrv903x_rx_nco.h */

/**
 * \brief SET_ORX_NCO command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_ORxNcoConfigResp
{
    adrv903x_CpuErrorCode_e status;  /*!< CPU error status code */
    uint8_t                 chanSelect; /*!< Rx channel (bit mapped) */
} adrv903x_ORxNcoConfigResp_t;)

/**
 * \brief GET_ORX_NCO command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_ORxNcoConfigReadback
{
    uint8_t                     chanSelect;          /*!< Select ORx channel (bit mapped) */
    uint8_t                     ncoSelect;           /*!< Select ORx NCO, 0 = DDC, 1 = datapath */
} adrv903x_ORxNcoConfigReadback_t;)

/* GET_ORX_NCO command response structure is defined in adi_adrv903x_rx_nco.h */

/* SET_TX_MIX_NCO command structure is defined in adi_adrv903x_tx_nco.h */

/**
 * \brief SET_TX_MIX_NCO command response structure
 */
ADI_ADRV903X_PACK_START
typedef struct adrv903x_TxNcoMixConfigResp
{
    adrv903x_CpuErrorCode_e     status;       /*!< CPU error status code */
    uint8_t                     chanSelect;   /*!< Tx channel (bit mapped) */
} adrv903x_TxNcoMixConfigResp_t;
ADI_ADRV903X_PACK_FINISH

/**
 * \brief GET_TX_MIX_NCO command structure
 */
ADI_ADRV903X_PACK_START
typedef struct adrv903x_TxNcoMixConfigReadback
{
    uint8_t         chanSelect;       /*!< Select the Tx channel (bit mapped) to get (only one channel at a time) */
} adrv903x_TxNcoMixConfigReadback_t;
ADI_ADRV903X_PACK_FINISH

/* GET_TX_MIX_NCO command response structure is defined in adi_adrv903x_tx_nco.h */

/* SET_TX_TEST_NCO command structure is defined in adi_adrv903x_tx_nco.h */

/**
 * \brief SET_TX_TEST_NCO command response structure
 */
ADI_ADRV903X_PACK_START
typedef struct adrv903x_TxTestNcoConfigResp
{
    adrv903x_CpuErrorCode_e     status;        /*!< CPU error status code */
    uint8_t                     chanSelect;    /*!< Tx channel (bit mapped) */
} adrv903x_TxTestNcoConfigResp_t;
ADI_ADRV903X_PACK_FINISH


/**
 * \brief GET_TX_TEST_NCO command structure
 */
ADI_ADRV903X_PACK_START
typedef struct adrv903x_TxTestNcoConfigReadback
{
    uint8_t                         chanSelect;      /*!< Select Tx channel (bit mapped) */
    adi_adrv903x_TxTestNcoSelect_t  ncoSelect;       /*!< 0 or 1 */
} adrv903x_TxTestNcoConfigReadback_t;
ADI_ADRV903X_PACK_FINISH

/* GET_TX_TEST_NCO command response structure is defined in adi_adrv903x_tx_nco.h */

#endif /* __ADRV903X_CPU_CMD_GETSET_NCO_H__ */


