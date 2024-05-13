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
#include "periph_api.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! Enumeration of client characteristic configuration descriptors */
enum {
	PERIPH_GATT_SC_CCC_IDX, /*! GATT service, service changed characteristic */
	PERIPH_WP_DAT_CCC_IDX, /*! Arm Ltd. proprietary service, data transfer characteristic */
	PERIPH_NUM_CCC_IDX
};

/**************************************************************************************************
  Configurable Parameters
**************************************************************************************************/

/*! configurable parameters for advertising */
static const appAdvCfg_t periphAdvCfg = {
	{ 30000, 0, 0 }, /*! Advertising durations in ms, 0 corresponds to infinite */
	{ 96, 1600, 0 } /*! Advertising intervals in 0.625 ms units */
};

/*! configurable parameters for slave */
static const appSlaveCfg_t periphSlaveCfg = {
	1, /*! Maximum connections */
};

/*! ATT configurable parameters (increase MTU) */
static const attCfg_t periphAttCfg = {
	15, /* ATT server service discovery connection idle timeout in seconds */
	241, /* desired ATT MTU */
	ATT_MAX_TRANS_TIMEOUT, /* transcation timeout in seconds */
	4 /* number of queued prepare writes supported by server */
};

/**************************************************************************************************
  Advertising Data
**************************************************************************************************/

/*! advertising data, discoverable mode */
static const uint8_t periphAdvDataDisc[] = {
	/*! flags */
	2, /*! length */
	DM_ADV_TYPE_FLAGS, /*! AD type */
	DM_FLAG_LE_GENERAL_DISC | /*! flags */
	DM_FLAG_LE_BREDR_NOT_SUP,
	/*! device name */
	7, /*! length */
	DM_ADV_TYPE_LOCAL_NAME, /*! AD type */
	'P', 'e', 'r', 'i', 'p', 'h'
};

/**************************************************************************************************
  Client Characteristic Configuration Descriptors
**************************************************************************************************/

/*! client characteristic configuration descriptors settings, indexed by above enumeration */
static const attsCccSet_t periphCccSet[PERIPH_NUM_CCC_IDX] = {
	/* cccd handle          value range               security level */
	{ GATT_SC_CH_CCC_HDL, ATT_CLIENT_CFG_INDICATE, DM_SEC_LEVEL_NONE }, /* PERIPH_GATT_SC_CCC_IDX */
	{ WP_DAT_CH_CCC_HDL, ATT_CLIENT_CFG_NOTIFY, DM_SEC_LEVEL_NONE } /* DATS_WP_DAT_CCC_IDX */
};

/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/*! application control block */
static struct {
	wsfHandlerId_t handlerId; /* WSF handler ID */
	periphRxCb_t rxCb; /* RX Callback function */
	dmConnId_t connId; /* Connection ID */
	bool_t connected; /* Connection state */
} periphCb;

/*************************************************************************************************/
/*!
 *  \brief  Application DM callback.
 *
 *  \param  pDmEvt  DM callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void periphDmCback(dmEvt_t *pDmEvt)
{
	dmEvt_t *pMsg;
	uint16_t len;

	len = DmSizeOfEvt(pDmEvt);

	if ((pMsg = WsfMsgAlloc(len)) != NULL) {
		memcpy(pMsg, pDmEvt, len);
		WsfMsgSend(periphCb.handlerId, pMsg);
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
static void periphAttCback(attEvt_t *pEvt)
{
	attEvt_t *pMsg;

	if ((pMsg = WsfMsgAlloc(sizeof(attEvt_t) + pEvt->valueLen)) != NULL) {
		memcpy(pMsg, pEvt, sizeof(attEvt_t));
		pMsg->pValue = (uint8_t *)(pMsg + 1);
		memcpy(pMsg->pValue, pEvt->pValue, pEvt->valueLen);
		WsfMsgSend(periphCb.handlerId, pMsg);
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
static void periphCccCback(attsCccEvt_t *pEvt) {}

/*************************************************************************************************/
/*!
 *  \brief  ATTS write callback for proprietary data service.
 *
 *  \return ATT status.
 */
/*************************************************************************************************/
uint8_t periphWpWriteCback(dmConnId_t connId, uint16_t handle,
			   uint8_t operation, uint16_t offset,
			   uint16_t len, uint8_t *pValue, attsAttr_t *pAttr)
{
	/* Call callback with data */
	if (periphCb.rxCb != NULL) {
		periphCb.rxCb(pValue, len);
	}

	return ATT_SUCCESS;
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
static void periphSetup(dmEvt_t *pMsg)
{
	/* set advertising data for discoverable mode */
	AppAdvSetData(APP_ADV_DATA_DISCOVERABLE, sizeof(periphAdvDataDisc),
		      (uint8_t *)periphAdvDataDisc);

	/* set advertising data or connectable mode */
	AppAdvSetData(APP_ADV_DATA_CONNECTABLE, sizeof(periphAdvDataDisc),
		      (uint8_t *)periphAdvDataDisc);

	/* start advertising; automatically set connectable/discoverable mode and bondable mode */
	AppAdvStart(APP_MODE_AUTO_INIT);
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
static void periphProcMsg(dmEvt_t *pMsg)
{
	uint8_t uiEvent = APP_UI_NONE;

	switch (pMsg->hdr.event) {
	case DM_RESET_CMPL_IND:
		periphSetup(pMsg);
		uiEvent = APP_UI_RESET_CMPL;
		break;

	case DM_ADV_START_IND:
		uiEvent = APP_UI_ADV_START;
		break;

	case DM_ADV_STOP_IND:
		uiEvent = APP_UI_ADV_STOP;
		break;

	case DM_CONN_OPEN_IND:
		/* Save connId  */
		periphCb.connId = (dmConnId_t)pMsg->hdr.param;
		periphCb.connected = TRUE;
		uiEvent = APP_UI_CONN_OPEN;
		break;

	case DM_CONN_CLOSE_IND:
		periphCb.connected = FALSE;
		uiEvent = APP_UI_CONN_CLOSE;
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
 *  \brief  Send data to the peer.
 *
 *  \param  data     Pointer to received data.
 *  \param  len      Number of bytes received.
 *
 *  \return TRUE if data send, FALSE if error.
 */
/*************************************************************************************************/
bool_t PeriphTXData(uint8_t *data, uint16_t len)
{
	/* Make sure we're connected */
	if (!periphCb.connected) {
		return FALSE;
	}

	/* Check if a client characteristic configuration descriptor is enabled and if
	   the characteristic's security level has been met. */
	if (AttsCccEnabled(periphCb.connId, PERIPH_WP_DAT_CCC_IDX)) {
		/* send notification */
		AttsHandleValueNtf(periphCb.connId, WP_DAT_HDL, len, data);

		return TRUE;
	}

	return FALSE;
}

/*************************************************************************************************/
/*!
 *  \brief  Register the callback for received data.
 *
 *  \param  cb    Callback function for when data is received.
 *
 *  \return None.
 */
/*************************************************************************************************/
void PeriphRegisterRXCallback(periphRxCb_t cb)
{
	periphCb.rxCb = cb;
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
void PeriphHandlerInit(wsfHandlerId_t handlerId)
{
	uint8_t addr[6] = { 0 };
	APP_TRACE_INFO0("PeriphHandlerInit");
	AppGetBdAddr(addr);
	APP_TRACE_INFO6("MAC Addr: %02x:%02x:%02x:%02x:%02x:%02x", addr[5], addr[4],
			addr[3], addr[2],
			addr[1], addr[0]);
	APP_TRACE_INFO1("Adv local name: %s", &periphAdvDataDisc[5]);

	/* store handler ID */
	periphCb.handlerId = handlerId;

	/* Initialize the RX Callback to  NULL */
	periphCb.rxCb = NULL;

	/* Set configuration pointers */
	pAppSlaveCfg = (appSlaveCfg_t *)&periphSlaveCfg;
	pAppAdvCfg = (appAdvCfg_t *)&periphAdvCfg;
	pAttCfg = (attCfg_t *)&periphAttCfg;

	/* Initialize application framework */
	AppSlaveInit();
	AppServerInit();
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
void PeriphHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg)
{
	if (pMsg != NULL) {
		APP_TRACE_INFO1("Periph got evt %d", pMsg->event);

		/* process ATT messages */
		if (pMsg->event >= ATT_CBACK_START && pMsg->event <= ATT_CBACK_END) {
			/* process server-related ATT messages */
			AppServerProcAttMsg(pMsg);
		} else if (pMsg->event >= DM_CBACK_START && pMsg->event <= DM_CBACK_END) {
			/* process DM messages */
			/* process advertising and connection-related messages */
			AppSlaveProcDmMsg((dmEvt_t *)pMsg);
		}

		/* perform profile and user interface-related operations */
		periphProcMsg((dmEvt_t *)pMsg);
	}
}

/*************************************************************************************************/
/*!
 *  \brief  Start the application.
 *
 *  \return None.
 */
/*************************************************************************************************/
void PeriphStart(void)
{
	/* Register for stack callbacks */
	DmRegister(periphDmCback);
	DmConnRegister(DM_CLIENT_ID_APP, periphDmCback);
	AttRegister(periphAttCback);
	AttConnRegister(AppServerConnCback);
	AttsCccRegister(PERIPH_NUM_CCC_IDX, (attsCccSet_t *)periphCccSet,
			periphCccCback);

	/* Initialize attribute server database */
	SvcCoreGattCbackRegister(GattReadCback, GattWriteCback);
	SvcCoreAddGroup();
	SvcWpCbackRegister(NULL, periphWpWriteCback);
	SvcWpAddGroup();

	/* Set Service Changed CCCD index. */
	GattSetSvcChangedIdx(PERIPH_GATT_SC_CCC_IDX);

	/* Reset the device */
	DmDevReset();
}
