/**
 * \file adrv904x_cpu_cmd_getset_nco.h
 * 
 * \brief   Command definition for:
 *          ADRV904X_CPU_CMD_ID_GET_RX_NCO, 
 *          ADRV904X_CPU_CMD_ID_SET_RX_NCO,
 *          ADRV904X_CPU_CMD_ID_GET_ORX_NCO,
 *          ADRV904X_CPU_CMD_ID_SET_ORX_NCO,
 *          ADRV904X_CPU_CMD_ID_GET_TX_MIX_NCO,
 *          ADRV904X_CPU_CMD_ID_SET_TX_MIX_NCO,
 *          ADRV904X_CPU_CMD_ID_GET_TX_TEST_NCO,
 *          ADRV904X_CPU_CMD_ID_SET_TX_TEST_NCO
 *
 * \details Command definition for:
 *          ADRV904X_CPU_CMD_ID_GET_RX_NCO, 
 *          ADRV904X_CPU_CMD_ID_SET_RX_NCO,
 *          ADRV904X_CPU_CMD_ID_GET_ORX_NCO,
 *          ADRV904X_CPU_CMD_ID_SET_ORX_NCO,
 *          ADRV904X_CPU_CMD_ID_GET_TX_MIX_NCO,
 *          ADRV904X_CPU_CMD_ID_SET_TX_MIX_NCO,
 *          ADRV904X_CPU_CMD_ID_GET_TX_TEST_NCO,
 *          ADRV904X_CPU_CMD_ID_SET_TX_TEST_NCO
 *
 * ADRV904X API Version: 2.10.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_GETSET_NCO_H__
#define __ADRV904X_CPU_CMD_GETSET_NCO_H__

#include "adi_adrv904x_rx_nco.h"
#include "adi_adrv904x_tx_nco.h"
#include "adrv904x_cpu_error_codes_types.h"

/* SET_RX_NCO command structure is defined in adi_adrv904x_rx_nco.h */

/**
 * \brief SET_RX_NCO command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_RxNcoConfigResp
{
    adrv904x_CpuErrorCode_e status;  /*!< CPU error status code */
    uint8_t                 chanSelect; /*!< Channel mask of channels set */
} adrv904x_RxNcoConfigResp_t;)

/**
 * \brief GET_RX_NCO command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_RxNcoConfigReadback
{
    uint8_t                   chanSelect;             /*!< Rx channel (bit mapped) */
    adi_adrv904x_DdcNumber_t  bandSelect;             /*!< which band */
} adrv904x_RxNcoConfigReadback_t;)

/* GET_RX_NCO command response structure is defined in adi_adrv904x_rx_nco.h */

/* SET_ORX_NCO command structure is defined in adi_adrv904x_rx_nco.h */

/**
 * \brief SET_ORX_NCO command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_ORxNcoConfigResp
{
    adrv904x_CpuErrorCode_e status;  /*!< CPU error status code */
    uint8_t                 chanSelect; /*!< Rx channel (bit mapped) */
} adrv904x_ORxNcoConfigResp_t;)

/**
 * \brief GET_ORX_NCO command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_ORxNcoConfigReadback
{
    uint8_t                     chanSelect;          /*!< Select ORx channel (bit mapped) */
    uint8_t                     ncoSelect;           /*!< Select ORx NCO, 0 = DDC, 1 = datapath */
} adrv904x_ORxNcoConfigReadback_t;)

/* GET_ORX_NCO command response structure is defined in adi_adrv904x_rx_nco.h */

/* SET_TX_MIX_NCO command structure is defined in adi_adrv904x_tx_nco.h */

/**
 * \brief SET_TX_MIX_NCO command response structure
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_TxNcoMixConfigResp
{
    adrv904x_CpuErrorCode_e     status;       /*!< CPU error status code */
    uint8_t                     chanSelect;   /*!< Tx channel (bit mapped) */

    uint8_t                     bandSelect;   /*!< Select Tx band number */
} adrv904x_TxNcoMixConfigResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief GET_TX_MIX_NCO command structure
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_TxNcoMixConfigReadback
{
    uint8_t         chanSelect;       /*!< Select the Tx channel (bit mapped) to get (only one channel at a time) */

    uint8_t                     bandSelect;   /*!< Select Tx band number */
} adrv904x_TxNcoMixConfigReadback_t;
ADI_ADRV904X_PACK_FINISH

/* GET_TX_MIX_NCO command response structure is defined in adi_adrv904x_tx_nco.h */

/* SET_TX_TEST_NCO command structure is defined in adi_adrv904x_tx_nco.h */

/**
 * \brief SET_TX_TEST_NCO command response structure
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_TxTestNcoConfigResp
{
    adrv904x_CpuErrorCode_e     status;        /*!< CPU error status code */
    uint8_t                     chanSelect;    /*!< Tx channel (bit mapped) */

    uint8_t                     bandSelect;   /*!< Select Tx band number */
} adrv904x_TxTestNcoConfigResp_t;
ADI_ADRV904X_PACK_FINISH


/**
 * \brief GET_TX_TEST_NCO command structure
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_TxTestNcoConfigReadback
{
    uint8_t                         chanSelect;      /*!< Select Tx channel (bit mapped) */

    uint8_t                     bandSelect;   /*!< Select Tx band number */
    adi_adrv904x_TxTestNcoSelect_t  ncoSelect;       /*!< 0 or 1 */
} adrv904x_TxTestNcoConfigReadback_t;
ADI_ADRV904X_PACK_FINISH

/* GET_TX_TEST_NCO command response structure is defined in adi_adrv904x_tx_nco.h */


/**
 * \brief SET_TX_CDUC_NCO command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_TxNcoCducConfigResp
{
    adrv904x_CpuErrorCode_e     status;        /*!< CPU error status code */
    uint8_t                     chanSelect;    /*!< Tx channel (bit mapped) */
    uint8_t                     carrierSelect; /*!< Select Tx carrier (bit mapped) */
} adrv904x_TxNcoCducConfigResp_t;)

/**
 * \brief SET_RX_CDDC_NCO command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_RxNcoCddcConfigResp
{
    adrv904x_CpuErrorCode_e     status;        /*!< CPU error status code */
    uint8_t                     chanSelect;    /*!< Rx channel (bit mapped) */
    uint8_t                     carrierSelect; /*!< Select Rx carrier (bit mapped) */
} adrv904x_RxNcoCddcConfigResp_t;)

#endif /* __ADRV904X_CPU_CMD_GETSET_NCO_H__ */


