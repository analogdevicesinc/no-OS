/*************************************************************************************************/
/*!
 * @file    cordio_init.c
 * @brief   Simple BLE app that advertises as "Periph".  Recommended app for getting started with MSDK BLE.
*
*  Copyright (c) 2013-2019 Arm Ltd. All Rights Reserved.
*
*  Copyright (c) 2019 Packetcraft, Inc.
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

#include <stdio.h>
#include <string.h>

#include "wsf_types.h"
#include "wsf_trace.h"
#include "wsf_bufio.h"
#include "wsf_msg.h"
#include "wsf_assert.h"
#include "wsf_buf.h"
#include "wsf_heap.h"
#include "wsf_cs.h"
#include "wsf_timer.h"
#include "wsf_os.h"

#include "sec_api.h"
#include "hci_handler.h"
#include "dm_handler.h"
#include "l2c_handler.h"
#include "att_handler.h"
#include "smp_handler.h"
#include "l2c_api.h"
#include "att_api.h"
#include "smp_api.h"
#include "app_api.h"
#include "hci_core.h"
#include "app_terminal.h"

#if defined(HCI_TR_EXACTLE) && (HCI_TR_EXACTLE == 1)
#include "ll_init_api.h"
#endif

#include "pal_bb.h"
#include "pal_cfg.h"

#include "periph_api.h"
#include "app_ui.h"
/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! \brief UART TX buffer size */
#define PLATFORM_UART_TERMINAL_BUFFER_SIZE 2048U

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/*! \brief  Pool runtime configuration. */
static wsfBufPoolDesc_t mainPoolDesc[] = { { 16, 8 }, { 32, 4 }, { 192, 8 }, { 256, 8 } };

#if defined(HCI_TR_EXACTLE) && (HCI_TR_EXACTLE == 1)
static LlRtCfg_t mainLlRtCfg;
#endif

/**************************************************************************************************
  Functions
**************************************************************************************************/

/*! \brief  Stack initialization for app. */
extern void StackInitPeriph(void);

/*************************************************************************************************/
/*!
 *  \brief  RX data received callback.
 *
 *  \param  data     Pointer to received data.
 *  \param  len      Number of bytes received.
 */
/*************************************************************************************************/
static void appRxCallback(uint8_t *data, uint16_t len)
{
	unsigned i;

	/* Print the received data */
	printf("App received: ");

	for (i = 0; i < len; i++) {
		printf("0x%02X ", data[i]);
	}
	printf("\n");

	/* Echo the data back to the central */
	if (!PeriphTXData(data, len)) {
		printf("Error sending data to peer\n");
	}
}

/*************************************************************************************************/
/*!
 *  \brief  Initialize WSF.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void mainWsfInit(void)
{
#if defined(HCI_TR_EXACTLE) && (HCI_TR_EXACTLE == 1)
	/* Configurations must be persistent. */
	static BbRtCfg_t mainBbRtCfg;

	PalBbLoadCfg((PalBbCfg_t *)&mainBbRtCfg);
	LlGetDefaultRunTimeCfg(&mainLlRtCfg);
	PalCfgLoadData(PAL_CFG_ID_LL_PARAM, &mainLlRtCfg.maxAdvSets,
		       sizeof(LlRtCfg_t) - 9);
#endif

	uint32_t memUsed;
	WsfCsEnter();
	memUsed = WsfBufIoUartInit(WsfHeapGetFreeStartAddress(),
				   PLATFORM_UART_TERMINAL_BUFFER_SIZE);
	WsfHeapAlloc(memUsed);
	WsfCsExit();

#if defined(HCI_TR_EXACTLE) && (HCI_TR_EXACTLE == 1)
	/* +12 for message headroom, + 2 event header, +255 maximum parameter length. */
	const uint16_t maxRptBufSize = 12 + 2 + 255;

	/* +12 for message headroom, +4 for header. */
	const uint16_t aclBufSize = 12 + mainLlRtCfg.maxAclLen + 4 +
				    BB_DATA_PDU_TAILROOM;

	/* Adjust buffer allocation based on platform configuration. */
	mainPoolDesc[2].len = maxRptBufSize;
	mainPoolDesc[2].num = mainLlRtCfg.maxAdvReports;
	mainPoolDesc[3].len = aclBufSize;
	mainPoolDesc[3].num = mainLlRtCfg.numTxBufs + mainLlRtCfg.numRxBufs;
#endif

	const uint8_t numPools = sizeof(mainPoolDesc) / sizeof(mainPoolDesc[0]);

	WsfCsEnter();
	memUsed = WsfBufInit(numPools, mainPoolDesc);
	WsfHeapAlloc(memUsed);
	WsfCsExit();

	WsfOsInit();
	WsfTimerInit();
#if (WSF_TOKEN_ENABLED == TRUE) || (WSF_TRACE_ENABLED == TRUE)
	WsfTraceRegisterHandler(WsfBufIoWrite);
	WsfTraceEnable(TRUE);
#endif

	AppTerminalInit();

#if defined(HCI_TR_EXACTLE) && (HCI_TR_EXACTLE == 1)
	WsfCsEnter();
	LlInitRtCfg_t llCfg = { .pBbRtCfg = &mainBbRtCfg,
				.wlSizeCfg = 4,
				.rlSizeCfg = 4,
				.plSizeCfg = 4,
				.pLlRtCfg = &mainLlRtCfg,
				.pFreeMem = WsfHeapGetFreeStartAddress(),
				.freeMemAvail = WsfHeapCountAvailable()
			      };

	memUsed = LlInit(&llCfg);
	WsfHeapAlloc(memUsed);
	WsfCsExit();

	bdAddr_t bdAddr;
	PalCfgLoadData(PAL_CFG_ID_BD_ADDR, bdAddr, sizeof(bdAddr_t));
	LlSetBdAddr((uint8_t *)&bdAddr);
#endif

	StackInitPeriph();
	PeriphStart();

	PeriphRegisterRXCallback(appRxCallback);
}

/*************************************************************************************************/
/*!
*  \fn     cordio_main
*
*  \brief  Inititalize the peripherals required by cordio.
*
*  \param  None.
*
*  \return None.
*/
/*************************************************************************************************/
int cordio_init(void)
{
	mainWsfInit();

	while (1) {
		/* Run the WSF OS */
		wsfOsDispatcher();

		if (!WsfOsActive()) {
			/* No WSF tasks are active, optionally sleep */
		}
	}

	return 0;
}
