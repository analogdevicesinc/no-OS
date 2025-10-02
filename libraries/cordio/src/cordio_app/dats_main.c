/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Data transmitter sample application.
 *
 *  Copyright (c) 2012-2019 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2019-2020 Packetcraft, Inc.
 *
 *  Portions Copyright (c) 2022-2023 Analog Devices, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*************************************************************************************************/

#include <string.h>
#include "wsf_types.h"
#include "util/bstream.h"
#include "wsf_msg.h"
#include "wsf_trace.h"
#include "wsf_buf.h"
#include "wsf_nvm.h"
#include "wsf_timer.h"
#include "hci_api.h"
#include "sec_api.h"
#include "dm_api.h"
#include "smp_api.h"
#include "att_api.h"
#include "app_api.h"
#include "app_main.h"
#include "app_db.h"
#include "app_ui.h"
#include "svc_ch.h"
#include "svc_core.h"
#include "svc_wp.h"
#include "util/calc128.h"
#include "gatt/gatt_api.h"
#include "dats_api.h"
#include "wut.h"
#include "trimsir_regs.h"
#include "pal_btn.h"
#include "tmr.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/
#if (BT_VER > 8)

/* PHY Test Modes */
#define DATS_PHY_1M 1
#define DATS_PHY_2M 2
#define DATS_PHY_CODED 3

#endif /* BT_VER */

#define TRIM_TIMER_EVT 0x99

#define TRIM_TIMER_PERIOD_MS 60000

/*! Button press handling constants */
#define BTN_SHORT_MS 200
#define BTN_MED_MS 500
#define BTN_LONG_MS 1000

#define BTN_1_TMR MXC_TMR2
#define BTN_2_TMR MXC_TMR3

/*! Enumeration of client characteristic configuration descriptors */
enum {
    DATS_GATT_SC_CCC_IDX, /*! GATT service, service changed characteristic */
    DATS_WP_DAT_CCC_IDX, /*! Arm Ltd. proprietary service, data transfer characteristic */
    DATS_NUM_CCC_IDX
};

/**************************************************************************************************
  Configurable Parameters
**************************************************************************************************/

/*! configurable parameters for advertising */
static const appAdvCfg_t datsAdvCfg = {
    { 0, 0, 0 }, /*! Advertising durations in ms */
    { 300, 1600, 0 } /*! Advertising intervals in 0.625 ms units */
};

/*! configurable parameters for slave */
static const appSlaveCfg_t datsSlaveCfg = {
    1, /*! Maximum connections */
};

/*! configurable parameters for security */
static const appSecCfg_t datsSecCfg = {
    DM_AUTH_BOND_FLAG | DM_AUTH_SC_FLAG, /*! Authentication and bonding flags */
    DM_KEY_DIST_IRK, /*! Initiator key distribution flags */
    DM_KEY_DIST_LTK | DM_KEY_DIST_IRK, /*! Responder key distribution flags */
    FALSE, /*! TRUE if Out-of-band pairing data is present */
    FALSE /*! TRUE to initiate security upon connection */
};

/*! TRUE if Out-of-band pairing data is to be sent */
static const bool_t datsSendOobData = FALSE;

/*! SMP security parameter configuration */
static const smpCfg_t datsSmpCfg = {
    500, /*! 'Repeated attempts' timeout in msec */
    SMP_IO_NO_IN_NO_OUT, /*! I/O Capability */
    7, /*! Minimum encryption key length */
    16, /*! Maximum encryption key length */
    1, /*! Attempts to trigger 'repeated attempts' timeout */
    0, /*! Device authentication requirements */
    64000, /*! Maximum repeated attempts timeout in msec */
    64000, /*! Time msec before attemptExp decreases */
    2 /*! Repeated attempts multiplier exponent */
};

/* iOS connection parameter update requirements

 The connection parameter request may be rejected if it does not meet the following guidelines:
 * Peripheral Latency of up to 30 connection intervals.
 * Supervision Timeout from 2 seconds to 6 seconds.
 * Interval Min of at least 15 ms.
 * Interval Min is a multiple of 15 ms.
 * One of the following:
   * Interval Max at least 15 ms greater than Interval Min.
   * Interval Max and Interval Min both set to 15 ms.
   * Interval Max * (Peripheral Latency + 1) of 2 seconds or less.
   * Supervision Timeout greater than Interval Max * (Peripheral Latency + 1) * 3.
*/

/*! configurable parameters for connection parameter update */
static const appUpdateCfg_t datsUpdateCfg = {
    5000,
    /*! ^ Connection idle period in ms before attempting
    connection parameter update; set to zero to disable */
    (50 / 1.25), /*! Minimum connection interval in 1.25ms units */
    (100 / 1.25), /*! Maximum connection interval in 1.25ms units */
    0, /*! Connection latency */
    600, /*! Supervision timeout in 10ms units */
    5 /*! Number of update attempts before giving up */
};

/*! ATT configurable parameters (increase MTU) */
static const attCfg_t datsAttCfg = {
    15, /* ATT server service discovery connection idle timeout in seconds */
    241, /* desired ATT MTU */
    ATT_MAX_TRANS_TIMEOUT, /* transcation timeout in seconds */
    4 /* number of queued prepare writes supported by server */
};

/*! local IRK */
static uint8_t localIrk[] = { 0x95, 0xC8, 0xEE, 0x6F, 0xC5, 0x0D, 0xEF, 0x93,
                              0x35, 0x4E, 0x7C, 0x57, 0x08, 0xE2, 0xA3, 0x85 };

/**************************************************************************************************
  Advertising Data
**************************************************************************************************/

/*! advertising data, discoverable mode */
static const uint8_t datsAdvDataDisc[] = {
    /*! flags */
    2, /*! length */
    DM_ADV_TYPE_FLAGS, /*! AD type */
    DM_FLAG_LE_GENERAL_DISC | /*! flags */
        DM_FLAG_LE_BREDR_NOT_SUP,

    /*! manufacturer specific data */
    3, /*! length */
    DM_ADV_TYPE_MANUFACTURER, /*! AD type */
    UINT16_TO_BYTES(HCI_ID_ANALOG) /*! company ID */
};

/*! scan data, discoverable mode */
static const uint8_t datsScanDataDisc[] = {
    /*! device name */
    5, /*! length */
    DM_ADV_TYPE_LOCAL_NAME, /*! AD type */
    'D',
    'A',
    'T',
    'S'
};

/**************************************************************************************************
  Client Characteristic Configuration Descriptors
**************************************************************************************************/

/*! client characteristic configuration descriptors settings, indexed by above enumeration */
static const attsCccSet_t datsCccSet[DATS_NUM_CCC_IDX] = {
    /* cccd handle          value range               security level */
    { GATT_SC_CH_CCC_HDL, ATT_CLIENT_CFG_INDICATE, DM_SEC_LEVEL_NONE }, /* DATS_GATT_SC_CCC_IDX */
    { WP_DAT_CH_CCC_HDL, ATT_CLIENT_CFG_NOTIFY, DM_SEC_LEVEL_NONE } /* DATS_WP_DAT_CCC_IDX */
};

/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/*! application control block */
static struct {
    wsfHandlerId_t handlerId; /* WSF handler ID */
    appDbHdl_t resListRestoreHdl; /*! Resolving List last restored handle */
    bool_t restoringResList; /*! Restoring resolving list from NVM */
} datsCb;

/* LESC OOB configuration */
static dmSecLescOobCfg_t *datsOobCfg;

/* Timer for trimming of the 32 kHz crystal */
wsfTimer_t trimTimer;

extern void setAdvTxPower(void);

/*************************************************************************************************/
/*!
 *  \brief  Send notification containing data.
 *
 *  \param  connId      DM connection ID.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void datsSendData(dmConnId_t connId)
{
    uint8_t str[] = "hello back";

    if (AttsCccEnabled(connId, DATS_WP_DAT_CCC_IDX)) {
        /* send notification */
        AttsHandleValueNtf(connId, WP_DAT_HDL, sizeof(str), str);
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Application DM callback.
 *
 *  \param  pDmEvt  DM callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void datsDmCback(dmEvt_t *pDmEvt)
{
    dmEvt_t *pMsg;
    uint16_t len;

    if (pDmEvt->hdr.event == DM_SEC_ECC_KEY_IND) {
        DmSecSetEccKey(&pDmEvt->eccMsg.data.key);

        /* If the local device sends OOB data. */
        if (datsSendOobData) {
            uint8_t oobLocalRandom[SMP_RAND_LEN];
            SecRand(oobLocalRandom, SMP_RAND_LEN);
            DmSecCalcOobReq(oobLocalRandom, pDmEvt->eccMsg.data.key.pubKey_x);
        }
    } else if (pDmEvt->hdr.event == DM_SEC_CALC_OOB_IND) {
        if (datsOobCfg == NULL) {
            datsOobCfg = WsfBufAlloc(sizeof(dmSecLescOobCfg_t));
        }

        if (datsOobCfg) {
            Calc128Cpy(datsOobCfg->localConfirm, pDmEvt->oobCalcInd.confirm);
            Calc128Cpy(datsOobCfg->localRandom, pDmEvt->oobCalcInd.random);
        }
    } else {
        len = DmSizeOfEvt(pDmEvt);

        if ((pMsg = WsfMsgAlloc(len)) != NULL) {
            memcpy(pMsg, pDmEvt, len);
            WsfMsgSend(datsCb.handlerId, pMsg);
        }
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Application ATT callback.
 *
 *  \param  pEvt    ATT callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void datsAttCback(attEvt_t *pEvt)
{
    attEvt_t *pMsg;

    if ((pMsg = WsfMsgAlloc(sizeof(attEvt_t) + pEvt->valueLen)) != NULL) {
        memcpy(pMsg, pEvt, sizeof(attEvt_t));
        pMsg->pValue = (uint8_t *)(pMsg + 1);
        memcpy(pMsg->pValue, pEvt->pValue, pEvt->valueLen);
        WsfMsgSend(datsCb.handlerId, pMsg);
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Application ATTS client characteristic configuration callback.
 *
 *  \param  pDmEvt  DM callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void datsCccCback(attsCccEvt_t *pEvt)
{
    appDbHdl_t dbHdl;

    /* If CCC not set from initialization and there's a device record and currently bonded */
    if ((pEvt->handle != ATT_HANDLE_NONE) &&
        ((dbHdl = AppDbGetHdl((dmConnId_t)pEvt->hdr.param)) != APP_DB_HDL_NONE) &&
        AppCheckBonded((dmConnId_t)pEvt->hdr.param)) {
        /* Store value in device database. */
        AppDbSetCccTblValue(dbHdl, pEvt->idx, pEvt->value);
        AppDbNvmStoreCccTbl(dbHdl);
    }
}

/*************************************************************************************************/
/*!
 *  \brief   Start the trim procedure for the 32 kHz crystal.
 *  \details 32 kHz crystal will drift over temperature, execute this trim procedure to align with 32 MHz clock.
 *  System will not be able to enter standby mode while this procedure is in progress (200-600 ms).
 *  Larger period values will save power. Do not execute this procedure while BLE hardware is disabled.
 *  Disable this periodic trim if under constant temperature. Refer to 32 kHz crystal data sheet for temperature variance.
 *
 *  \return  None.
 */
/*************************************************************************************************/
static void trimStart(void)
{
    int err;
    extern void wutTrimCb(int err);

    /* Start the 32 kHz crystal trim procedure */
    err = MXC_WUT_TrimCrystalAsync(MXC_WUT0, wutTrimCb);
    if (err != E_NO_ERROR) {
        APP_TRACE_INFO1("Error starting 32kHz crystal trim %d", err);
    }
}

/*************************************************************************************************/
/*!
 *  \brief  ATTS write callback for proprietary data service.
 *
 *  \return ATT status.
 */
/*************************************************************************************************/
uint8_t datsWpWriteCback(dmConnId_t connId, uint16_t handle, uint8_t operation, uint16_t offset,
                         uint16_t len, uint8_t *pValue, attsAttr_t *pAttr)
{
    if (len < 64) {
        /* print received data if not a speed test message */
        APP_TRACE_INFO0((const char *)pValue);

        /* send back some data */
        datsSendData(connId);
    }
    return ATT_SUCCESS;
}

/*************************************************************************************************/
/*!
*
*  \brief  Add device to resolving list.
*
*  \param  dbHdl   Device DB record handle.
*
*  \return None.
*/
/*************************************************************************************************/
static void datsPrivAddDevToResList(appDbHdl_t dbHdl)
{
    dmSecKey_t *pPeerKey;

    /* if peer IRK present */
    if ((pPeerKey = AppDbGetKey(dbHdl, DM_KEY_IRK, NULL)) != NULL) {
        /* set advertising peer address */
        AppSetAdvPeerAddr(pPeerKey->irk.addrType, pPeerKey->irk.bdAddr);
    }
}

/*************************************************************************************************/
/*!
*
*  \brief  Handle remove device from resolving list indication.
*
*  \param  pMsg    Pointer to DM callback event message.
*
*  \return None.
*/
/*************************************************************************************************/
static void datsPrivRemDevFromResListInd(dmEvt_t *pMsg)
{
    if (pMsg->hdr.status == HCI_SUCCESS) {
        if (AppDbGetHdl((dmConnId_t)pMsg->hdr.param) != APP_DB_HDL_NONE) {
            uint8_t addrZeros[BDA_ADDR_LEN] = { 0 };

            /* clear advertising peer address and its type */
            AppSetAdvPeerAddr(HCI_ADDR_TYPE_PUBLIC, addrZeros);
        }
    }
}

/*************************************************************************************************/
/*!
 *
 *  \brief  Display stack version.
 *
 *  \param  version    version number.
 *
 *  \return None.
 */
/*************************************************************************************************/
void datsDisplayStackVersion(const char *pVersion)
{
    APP_TRACE_INFO1("Stack Version: %s", pVersion);
}

/*************************************************************************************************/
/*!
 *  \brief  Set up advertising and other procedures that need to be performed after
 *          device reset.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void datsSetup(dmEvt_t *pMsg)
{
    /* Initialize control information */
    datsCb.restoringResList = FALSE;

    /* set advertising and scan response data for discoverable mode */
    AppAdvSetData(APP_ADV_DATA_DISCOVERABLE, sizeof(datsAdvDataDisc), (uint8_t *)datsAdvDataDisc);
    AppAdvSetData(APP_SCAN_DATA_DISCOVERABLE, sizeof(datsScanDataDisc),
                  (uint8_t *)datsScanDataDisc);

    /* set advertising and scan response data for connectable mode */
    AppAdvSetData(APP_ADV_DATA_CONNECTABLE, sizeof(datsAdvDataDisc), (uint8_t *)datsAdvDataDisc);
    AppAdvSetData(APP_SCAN_DATA_CONNECTABLE, sizeof(datsScanDataDisc), (uint8_t *)datsScanDataDisc);

    /* start advertising; automatically set connectable/discoverable mode and bondable mode */
    AppAdvStart(APP_MODE_AUTO_INIT);
}

/*************************************************************************************************/
/*!
 *  \brief  Begin restoring the resolving list.
 *
 *  \param  pMsg    Pointer to DM callback event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void datsRestoreResolvingList(dmEvt_t *pMsg)
{
    /* Restore first device to resolving list in Controller. */
    datsCb.resListRestoreHdl = AppAddNextDevToResList(APP_DB_HDL_NONE);

    if (datsCb.resListRestoreHdl == APP_DB_HDL_NONE) {
        /* No device to restore.  Setup application. */
        datsSetup(pMsg);
    } else {
        datsCb.restoringResList = TRUE;
    }
}

/*************************************************************************************************/
/*!
*  \brief  Handle add device to resolving list indication.
 *
 *  \param  pMsg    Pointer to DM callback event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void datsPrivAddDevToResListInd(dmEvt_t *pMsg)
{
    /* Check if in the process of restoring the Device List from NV */
    if (datsCb.restoringResList) {
        /* Set the advertising peer address. */
        datsPrivAddDevToResList(datsCb.resListRestoreHdl);

        /* Retore next device to resolving list in Controller. */
        datsCb.resListRestoreHdl = AppAddNextDevToResList(datsCb.resListRestoreHdl);

        if (datsCb.resListRestoreHdl == APP_DB_HDL_NONE) {
            /* No additional device to restore. Setup application. */
            datsSetup(pMsg);
        }
    } else {
        datsPrivAddDevToResList(AppDbGetHdl((dmConnId_t)pMsg->hdr.param));
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Process messages from the event handler.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void datsProcMsg(dmEvt_t *pMsg)
{
    uint8_t uiEvent = APP_UI_NONE;

    switch (pMsg->hdr.event) {
    case DM_RESET_CMPL_IND:
        AttsCalculateDbHash();
        DmSecGenerateEccKeyReq();
        AppDbNvmReadAll();
        datsRestoreResolvingList(pMsg);
        setAdvTxPower();
        uiEvent = APP_UI_RESET_CMPL;
        break;

    case DM_ADV_START_IND:
        WsfTimerStartMs(&trimTimer, TRIM_TIMER_PERIOD_MS);

        uiEvent = APP_UI_ADV_START;
        break;

    case DM_ADV_STOP_IND:
        WsfTimerStop(&trimTimer);
        uiEvent = APP_UI_ADV_STOP;
        break;

    case DM_CONN_OPEN_IND:
        uiEvent = APP_UI_CONN_OPEN;
        break;

    case DM_CONN_CLOSE_IND:
        WsfTimerStop(&trimTimer);

        APP_TRACE_INFO2("Connection closed status 0x%x, reason 0x%x", pMsg->connClose.status,
                        pMsg->connClose.reason);
        switch (pMsg->connClose.reason) {
        case HCI_ERR_CONN_TIMEOUT:
            APP_TRACE_INFO0(" TIMEOUT");
            break;
        case HCI_ERR_LOCAL_TERMINATED:
            APP_TRACE_INFO0(" LOCAL TERM");
            break;
        case HCI_ERR_REMOTE_TERMINATED:
            APP_TRACE_INFO0(" REMOTE TERM");
            break;
        case HCI_ERR_CONN_FAIL:
            APP_TRACE_INFO0(" FAIL ESTABLISH");
            break;
        case HCI_ERR_MIC_FAILURE:
            APP_TRACE_INFO0(" MIC FAILURE");
            break;
        }
        uiEvent = APP_UI_CONN_CLOSE;
        break;

    case DM_SEC_PAIR_CMPL_IND:
        DmSecGenerateEccKeyReq();
        AppDbNvmStoreBond(AppDbGetHdl((dmConnId_t)pMsg->hdr.param));
        uiEvent = APP_UI_SEC_PAIR_CMPL;
        break;

    case DM_SEC_PAIR_FAIL_IND:
        DmSecGenerateEccKeyReq();
        uiEvent = APP_UI_SEC_PAIR_FAIL;
        break;

    case DM_SEC_ENCRYPT_IND:
        uiEvent = APP_UI_SEC_ENCRYPT;
        break;

    case DM_SEC_ENCRYPT_FAIL_IND:
        uiEvent = APP_UI_SEC_ENCRYPT_FAIL;
        break;

    case DM_SEC_AUTH_REQ_IND:

        if (pMsg->authReq.oob) {
            dmConnId_t connId = (dmConnId_t)pMsg->hdr.param;

            /* TODO: Perform OOB Exchange with the peer. */

            /* TODO: Fill datsOobCfg peerConfirm and peerRandom with value passed out of band */

            if (datsOobCfg != NULL) {
                DmSecSetOob(connId, datsOobCfg);
            }

            DmSecAuthRsp(connId, 0, NULL);
        } else {
            AppHandlePasskey(&pMsg->authReq);
        }
        break;

    case DM_SEC_COMPARE_IND:
        AppHandleNumericComparison(&pMsg->cnfInd);
        break;

    case DM_PRIV_ADD_DEV_TO_RES_LIST_IND:
        datsPrivAddDevToResListInd(pMsg);
        break;

    case DM_PRIV_REM_DEV_FROM_RES_LIST_IND:
        datsPrivRemDevFromResListInd(pMsg);
        break;

    case DM_ADV_NEW_ADDR_IND:
        break;

    case DM_PRIV_CLEAR_RES_LIST_IND:
        APP_TRACE_INFO1("Clear resolving list status 0x%02x", pMsg->hdr.status);
        break;

#if (BT_VER > 8)
    case DM_PHY_UPDATE_IND:
        APP_TRACE_INFO2("DM_PHY_UPDATE_IND - RX: %d, TX: %d", pMsg->phyUpdate.rxPhy,
                        pMsg->phyUpdate.txPhy);
        break;
#endif /* BT_VER */

    case TRIM_TIMER_EVT:
        trimStart();
        WsfTimerStartMs(&trimTimer, TRIM_TIMER_PERIOD_MS);
        break;

    default:
        break;
    }

    if (uiEvent != APP_UI_NONE) {
        AppUiAction(uiEvent);
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Application handler init function called during system initialization.
 *
 *  \param  handlerID  WSF handler ID.
 *
 *  \return None.
 */
/*************************************************************************************************/
void DatsHandlerInit(wsfHandlerId_t handlerId)
{
    uint8_t addr[6] = { 0 };
    APP_TRACE_INFO0("DatsHandlerInit");
    AppGetBdAddr(addr);
    APP_TRACE_INFO6("MAC Addr: %02x:%02x:%02x:%02x:%02x:%02x", addr[5], addr[4], addr[3], addr[2],
                    addr[1], addr[0]);
    APP_TRACE_INFO1("Adv local name: %s", &datsScanDataDisc[2]);

    /* store handler ID */
    datsCb.handlerId = handlerId;

    /* Set configuration pointers */
    pAppSlaveCfg = (appSlaveCfg_t *)&datsSlaveCfg;
    pAppAdvCfg = (appAdvCfg_t *)&datsAdvCfg;
    pAppSecCfg = (appSecCfg_t *)&datsSecCfg;
    pAppUpdateCfg = (appUpdateCfg_t *)&datsUpdateCfg;
    pSmpCfg = (smpCfg_t *)&datsSmpCfg;
    pAttCfg = (attCfg_t *)&datsAttCfg;

    /* Initialize application framework */
    AppSlaveInit();
    AppServerInit();

    /* Set IRK for the local device */
    DmSecSetLocalIrk(localIrk);

    /* Setup 32 kHz crystal trim timer */
    trimTimer.handlerId = handlerId;
    trimTimer.msg.event = TRIM_TIMER_EVT;
}

/*************************************************************************************************/
/*!
 *  \brief  Button press callback.
 *
 *  \param  btn    Button press.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void datsBtnCback(uint8_t btn)
{
#if (BT_VER > 8)
    dmConnId_t connId;
    if ((connId = AppConnIsOpen()) != DM_CONN_ID_NONE)
#else
    if (AppConnIsOpen() != DM_CONN_ID_NONE)
#endif /* BT_VER */
    {
        switch (btn) {
#if (BT_VER > 8)
        case APP_UI_BTN_2_SHORT: {
            static uint32_t coded_phy_cnt = 0;
            /* Toggle PHY Test Mode */
            coded_phy_cnt++;
            switch (coded_phy_cnt & 0x3) {
            case 0:
                /* 1M PHY */
                APP_TRACE_INFO0("1 MBit TX and RX PHY Requested");
                DmSetPhy(connId, HCI_ALL_PHY_ALL_PREFERENCES, HCI_PHY_LE_1M_BIT, HCI_PHY_LE_1M_BIT,
                         HCI_PHY_OPTIONS_NONE);
                break;
            case 1:
                /* 2M PHY */
                APP_TRACE_INFO0("2 MBit TX and RX PHY Requested");
                DmSetPhy(connId, HCI_ALL_PHY_ALL_PREFERENCES, HCI_PHY_LE_2M_BIT, HCI_PHY_LE_2M_BIT,
                         HCI_PHY_OPTIONS_NONE);
                break;
            case 2:
                /* Coded S2 PHY */
                APP_TRACE_INFO0("LE Coded S2 TX and RX PHY Requested");
                DmSetPhy(connId, HCI_ALL_PHY_ALL_PREFERENCES, HCI_PHY_LE_CODED_BIT,
                         HCI_PHY_LE_CODED_BIT, HCI_PHY_OPTIONS_S2_PREFERRED);
                break;
            case 3:
                /* Coded S8 PHY */
                APP_TRACE_INFO0("LE Coded S8 TX and RX PHY Requested");
                DmSetPhy(connId, HCI_ALL_PHY_ALL_PREFERENCES, HCI_PHY_LE_CODED_BIT,
                         HCI_PHY_LE_CODED_BIT, HCI_PHY_OPTIONS_S8_PREFERRED);
                break;
            }
            break;
        }
#endif /* BT_VER */

        default:
            APP_TRACE_INFO0(" - No action assigned");
            break;
        }
    } else {
        switch (btn) {
        case APP_UI_BTN_1_SHORT:
            /* start advertising */
            AppAdvStart(APP_MODE_AUTO_INIT);
            break;

        case APP_UI_BTN_1_MED:
            /* Enter bondable mode */
            AppSetBondable(TRUE);
            break;

        case APP_UI_BTN_1_LONG:
            /* clear all bonding info */
            AppSlaveClearAllBondingInfo();
            AppDbNvmDeleteAll();
            break;

        case APP_UI_BTN_1_EX_LONG: {
            const char *pVersion;
            StackGetVersionNumber(&pVersion);
            datsDisplayStackVersion(pVersion);
        } break;

        case APP_UI_BTN_2_SHORT:
            /* stop advertising */
            AppAdvStop();
            break;

        default:
            APP_TRACE_INFO0(" - No action assigned");
            break;
        }
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Callback for WSF buffer diagnostic messages.
 *
 *  \param  pInfo     Diagnostics message
 *
 *  \return None.
 */
/*************************************************************************************************/
static void datsWsfBufDiagnostics(WsfBufDiag_t *pInfo)
{
    if (pInfo->type == WSF_BUF_ALLOC_FAILED) {
        APP_TRACE_INFO2("Dats got WSF Buffer Allocation Failure - Task: %d Len: %d",
                        pInfo->param.alloc.taskId, pInfo->param.alloc.len);
    }
}

/*************************************************************************************************/
/*!
 *  \brief     Platform button press handler.
 *
 *  \param[in] btnId  button ID.
 *  \param[in] state  button state. See ::PalBtnPos_t.
 *
 *  \return    None.
 */
/*************************************************************************************************/
static void btnPressHandler(uint8_t btnId, PalBtnPos_t state)
{
    if (btnId == 1) {
        /* Start/stop button timer */
        if (state == PAL_BTN_POS_UP) {
            /* Button Up, stop the timer, call the action function */
            unsigned btnUs = MXC_TMR_SW_Stop(BTN_1_TMR);
            if ((btnUs > 0) && (btnUs < BTN_SHORT_MS * 1000)) {
                AppUiBtnTest(APP_UI_BTN_1_SHORT);
            } else if (btnUs < BTN_MED_MS * 1000) {
                AppUiBtnTest(APP_UI_BTN_1_MED);
            } else if (btnUs < BTN_LONG_MS * 1000) {
                AppUiBtnTest(APP_UI_BTN_1_LONG);
            } else {
                AppUiBtnTest(APP_UI_BTN_1_EX_LONG);
            }
        } else {
            /* Button down, start the timer */
            MXC_TMR_SW_Start(BTN_1_TMR);
        }
    } else if (btnId == 2) {
        /* Start/stop button timer */
        if (state == PAL_BTN_POS_UP) {
            /* Button Up, stop the timer, call the action function */
            unsigned btnUs = MXC_TMR_SW_Stop(BTN_2_TMR);
            if ((btnUs > 0) && (btnUs < BTN_SHORT_MS * 1000)) {
                AppUiBtnTest(APP_UI_BTN_2_SHORT);
            } else if (btnUs < BTN_MED_MS * 1000) {
                AppUiBtnTest(APP_UI_BTN_2_MED);
            } else if (btnUs < BTN_LONG_MS * 1000) {
                AppUiBtnTest(APP_UI_BTN_2_LONG);
            } else {
                AppUiBtnTest(APP_UI_BTN_2_EX_LONG);
            }
        } else {
            /* Button down, start the timer */
            MXC_TMR_SW_Start(BTN_2_TMR);
        }
    } else {
        APP_TRACE_ERR0("Undefined button");
    }
}

/*************************************************************************************************/
/*!
 *  \brief  WSF event handler for application.
 *
 *  \param  event   WSF event mask.
 *  \param  pMsg    WSF message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void DatsHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg)
{
    if (pMsg != NULL) {
        APP_TRACE_INFO1("Dats got evt %d", pMsg->event);

        /* process ATT messages */
        if (pMsg->event >= ATT_CBACK_START && pMsg->event <= ATT_CBACK_END) {
            /* process server-related ATT messages */
            AppServerProcAttMsg(pMsg);
        } else if (pMsg->event >= DM_CBACK_START && pMsg->event <= DM_CBACK_END) {
            /* process DM messages */
            /* process advertising and connection-related messages */
            AppSlaveProcDmMsg((dmEvt_t *)pMsg);

            /* process security-related messages */
            AppSlaveSecProcDmMsg((dmEvt_t *)pMsg);
        }

        /* perform profile and user interface-related operations */
        datsProcMsg((dmEvt_t *)pMsg);
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Start the application.
 *
 *  \return None.
 */
/*************************************************************************************************/
void DatsStart(void)
{
    /* Register for stack callbacks */
    DmRegister(datsDmCback);
    DmConnRegister(DM_CLIENT_ID_APP, datsDmCback);
    AttRegister(datsAttCback);
    AttConnRegister(AppServerConnCback);
    AttsCccRegister(DATS_NUM_CCC_IDX, (attsCccSet_t *)datsCccSet, datsCccCback);

    /* Initialize attribute server database */
    SvcCoreGattCbackRegister(GattReadCback, GattWriteCback);
    SvcCoreAddGroup();
    SvcWpCbackRegister(NULL, datsWpWriteCback);
    SvcWpAddGroup();

    /* Set Service Changed CCCD index. */
    GattSetSvcChangedIdx(DATS_GATT_SC_CCC_IDX);

    /* Register for app framework button callbacks */
    AppUiBtnRegister(datsBtnCback);

#if (BT_VER > 8)
    DmPhyInit();
#endif /* BT_VER */

    WsfNvmInit();

    WsfBufDiagRegister(datsWsfBufDiagnostics);

    /* Initialize with button press handler */
    PalBtnInit(btnPressHandler);

    /* Reset the device */
    DmDevReset();
}
