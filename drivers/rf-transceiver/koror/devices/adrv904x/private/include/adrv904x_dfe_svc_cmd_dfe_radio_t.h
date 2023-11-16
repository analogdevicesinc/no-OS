/**
 * Disclaimer Legal Disclaimer
 * Copyright 2022 - 2022 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adrv904x_dfe_svc_cmd_dfe_radio_t.h
 *
 * \brief   Contains shared SDK/DFE radio command typedefs
 *
 * \details Contains DFE error code definitions
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef __ADRV904X_DFE_SVC_CMD_DFE_RADIO_T_H__
#define __ADRV904X_DFE_SVC_CMD_DFE_RADIO_T_H__

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_dfe_svc_err_codes_t.h"
#include "adrv904x_dfe_svc_cmd_t.h"

/**
 * \brief SDK DFE Radio Tx LOL parameter set command (ADRV904X_DFE_SVC_CMD_ID_RADIO_TXLOL_PARAMS_SET)
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeTxLolParamsSetCmd
{
    uint8_t txChannel;  /*!< TX channel number (0 - 7) */
    uint8_t txAtten;    /*!< TX attenuation (0 - 63) */
    int16_t offsetI;    /*!< TxLOL offset for I */
    int16_t offsetQ;    /*!< TxLOL offset for Q */
} adrv904x_DfeSvcCmdDfeTxLolParamsSetCmd_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE Radio CTC 2 Frac Delay command (ADRV904X_DFE_SVC_CMD_ID_RADIO_CTC2_FRAC_DELAY_SET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvctxPfirFracDelaySet
{
    uint8_t txChannel;    /*!< TX channel number (0 - 7) */
    uint8_t fracDelay;    /*!< Fractional delay (0 - 63) */
} adrv904x_DfeSvctxPfirFracDelaySet_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE Radio Tx LOL parameter set command response (ADRV904X_DFE_SVC_CMD_ID_RADIO_TXLOL_PARAMS_SET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeTxLolParamsSetCmdResp
{
    adi_adrv904x_DfeSvcErrCode_e status;  /*!< SDK error status code */
} adrv904x_DfeSvcCmdDfeTxLolParamsSetCmdResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE Radio CTC 2 Frac Delay command (ADRV904X_DFE_SVC_CMD_ID_RADIO_CTC2_FRAC_DELAY_SET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvctxPfirFracDelaySetResp
{
    adi_adrv904x_DfeSvcErrCode_e status;  /*!< SDK error status code */
} adrv904x_DfeSvctxPfirFracDelaySetResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE Radio Tx LOL parameter get command (ADRV904X_DFE_SVC_CMD_ID_RADIO_TXLOL_PARAMS_GET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeTxLolParamsGetCmd
{
    uint8_t txChannel;  /*!< TX channel number (0 - 7) */
    uint8_t txAtten;    /*!< TX attenuation (0 - 63) */
} adrv904x_DfeSvcCmdDfeTxLolParamsGetCmd_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE Radio Tx LOL parameter get command response (ADRV904X_DFE_SVC_CMD_ID_RADIO_TXLOL_PARAMS_GET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeTxLolParamsGetCmdResp
{
    adi_adrv904x_DfeSvcErrCode_e status;                /*!< SDK error status code */
    uint8_t           txChannel;             /*!< TX channel number (0 - 7) */
    uint8_t           txAtten;               /*!< TX attenuation (0 - 63) */
    int16_t           offsetI;               /*!< TxLOL offset for I */
    int16_t           offsetQ;               /*!< TxLOL offset for Q */
} adrv904x_DfeSvcCmdDfeTxLolParamsGetCmdResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE Radio Tx LOL enable updates set command (ADRV904X_DFE_SVC_CMD_ID_RADIO_TXLOL_ENABLE_UPDATES_SET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeTxLolEnableUpdatesSetCmd
{
    uint8_t txChannel;  /*!< TX channel number (0 - 7) */
    uint8_t enable;     /*!< enable LOL updates when Tx atten changes */
} adrv904x_DfeSvcCmdDfeTxLolEnableUpdatesSetCmd_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE Radio Tx LOL enable updates set command response (ADRV904X_DFE_SVC_CMD_ID_RADIO_TXLOL_ENABLE_UPDATES_SET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeTxLolEnableUpdatesSetCmdResp
{
    adi_adrv904x_DfeSvcErrCode_e status;                /*!< SDK error status code */
    uint8_t           txChannel;             /*!< TX channel number (0 - 7) */
} adrv904x_DfeSvcCmdDfeTxLolEnableUpdatesSetCmdResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE Radio Tx LOL enable updates get command (ADRV904X_DFE_SVC_CMD_ID_RADIO_TXLOL_ENABLE_UPDATES_GET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeTxLolEnableUpdatesGetCmd
{
    uint8_t txChannel;  /*!< TX channel number (0 - 7) */
} adrv904x_DfeSvcCmdDfeTxLolEnableUpdatesGetCmd_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SDK DFE Radio Tx LOL enable updates get command response (ADRV904X_DFE_SVC_CMD_ID_RADIO_TXLOL_ENABLE_UPDATES_GET)
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcCmdDfeTxLolEnableUpdatesGetCmdResp
{
    adi_adrv904x_DfeSvcErrCode_e status;                /*!< SDK error status code */
    uint8_t           txChannel;             /*!< TX channel number (0 - 7) */
    uint8_t           enable;                /*!< enable flag */
} adrv904x_DfeSvcCmdDfeTxLolEnableUpdatesGetCmdResp_t;
ADI_ADRV904X_PACK_FINISH

#endif /* __ADRV904X_DFE_SVC_CMD_DFE_RADIO_T_H__ */


