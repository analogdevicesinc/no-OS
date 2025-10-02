/*************************************************************************************************/
/*!
 *  \file   stack_dats.c
 *
 *  \brief  Stack initialization for dats.
 *
 *  Copyright (c) 2016-2019 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2019 Packetcraft, Inc.
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

#include "app_api.h"
#include "app_terminal.h"
#include "app_ui.h"
#include "att_api.h"
#include "att_handler.h"
#include "dats_api.h"
#include "dm_handler.h"
#include "hci_core.h"
#include "hci_defs.h"
#include "hci_handler.h"
#include "l2c_api.h"
#include "l2c_handler.h"
#include "ll_init_api.h"
#include "pal_bb.h"
#include "pal_cfg.h"
#include "sec_api.h"
#include "smp_api.h"
#include "smp_handler.h"
#include "svc_core.h"
#include "svc_dis.h"
#include "util/bstream.h"
#include "wdxs/wdxs_api.h"
#include "wsf_assert.h"
#include "wsf_buf.h"
#include "wsf_bufio.h"
#include "wsf_cs.h"
#include "wsf_heap.h"
#include "wsf_msg.h"
#include "wsf_os.h"
#include "wsf_timer.h"
#include "wsf_trace.h"
#include "wsf_types.h"
#include "pal_led.h"

#include "FreeRTOSConfig.h"

#include "wut.h"
#include "rtc.h"
#include "trimsir_regs.h"
#include "mxc_device.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! \brief UART TX buffer size */
#define PLATFORM_UART_TERMINAL_BUFFER_SIZE 2048U
#define DEFAULT_TX_POWER 0 /* dBm */

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/*! \brief  Pool runtime configuration. */
static wsfBufPoolDesc_t mainPoolDesc[] = { { 16, 8 },
                                           { 32, 4 },
                                           { 192, 8 },
                                           { 256, 16 },
                                           { 512, 4 } };

#if defined(HCI_TR_EXACTLE) && (HCI_TR_EXACTLE == 1)
static LlRtCfg_t mainLlRtCfg;
#endif

volatile int wutTrimComplete;

/**************************************************************************************************
  Functions
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \brief      Initialize stack.
 *
 *  \return     None.
 */
/*************************************************************************************************/
void StackInitDats(void)
{
    wsfHandlerId_t handlerId;

    SecInit();
    SecAesInit();
    SecCmacInit();
    SecEccInit();

    handlerId = WsfOsSetNextHandler(HciHandler);
    HciHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(DmHandler);
    DmDevVsInit(0);
    DmConnInit();
    DmAdvInit();
    DmConnSlaveInit();
    DmSecInit();
    DmSecLescInit();
    DmPrivInit();
    DmHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(L2cSlaveHandler);
    L2cSlaveHandlerInit(handlerId);
    L2cInit();
    L2cSlaveInit();

    handlerId = WsfOsSetNextHandler(AttHandler);
    AttHandlerInit(handlerId);
    AttsInit();
    AttsIndInit();

    handlerId = WsfOsSetNextHandler(SmpHandler);
    SmpHandlerInit(handlerId);
    SmprInit();
    SmprScInit();
    HciSetMaxRxAclLen(100);

    handlerId = WsfOsSetNextHandler(AppHandler);
    AppHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(DatsHandler);
    DatsHandlerInit(handlerId);
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
    /* +12 for message headroom, + 2 event header, +255 maximum parameter length. */
    const uint16_t maxRptBufSize = 12 + 2 + 255;

    /* +12 for message headroom, +4 for header. */
    const uint16_t aclBufSize = 12 + mainLlRtCfg.maxAclLen + 4 + BB_DATA_PDU_TAILROOM;

    /* Adjust buffer allocation based on platform configuration. */
    mainPoolDesc[2].len = maxRptBufSize;
    mainPoolDesc[2].num = mainLlRtCfg.maxAdvReports;
    mainPoolDesc[3].len = aclBufSize;
    mainPoolDesc[3].num = mainLlRtCfg.numTxBufs + mainLlRtCfg.numRxBufs;
#endif

    const uint8_t numPools = sizeof(mainPoolDesc) / sizeof(mainPoolDesc[0]);

    uint16_t memUsed;
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
}

/*************************************************************************************************/
/*!
*  \fn     wutTrimCb
*
*  \brief  Callback function for the WUT 32 kHz crystal trim.
*
*  \param  err    Error code from the WUT driver.
*
*  \return None.
*/
/*************************************************************************************************/
void wutTrimCb(int err)
{
    if (err != E_NO_ERROR) {
        APP_TRACE_INFO1("32 kHz trim error %d\n", err);
    } else {
        APP_TRACE_INFO1("32kHz trimed to 0x%x", (MXC_TRIMSIR->rtc & MXC_F_TRIMSIR_RTC_X1TRIM) >>
                                                    MXC_F_TRIMSIR_RTC_X1TRIM_POS);
    }
    wutTrimComplete = 1;
}

/*************************************************************************************************/
/*!
*  \fn     setAdvTxPower
*
*  \brief  Set the default advertising TX power.
*
*  \return None.
*/
/*************************************************************************************************/
void setAdvTxPower(void)
{
    LlSetAdvTxPower(DEFAULT_TX_POWER);
}

void setInterruptPriority(void)
{
    /* Interrupts using FreeRTOS functions must have priorities between configMAX_PRIORITIES and
    configMAX_SYSCALL_INTERRUPT_PRIORITY, lower priority number is higher priority */

    /* Setup BLE hardware interrupt priorities */
    NVIC_SetPriority(BTLE_TX_DONE_IRQn, (configMAX_PRIORITIES - 2));
    NVIC_SetPriority(BTLE_RX_RCVD_IRQn, (configMAX_PRIORITIES - 2));
    NVIC_SetPriority(BTLE_RX_ENG_DET_IRQn, (configMAX_PRIORITIES - 2));
    NVIC_SetPriority(BTLE_SFD_DET_IRQn, (configMAX_PRIORITIES - 2));
    NVIC_SetPriority(BTLE_SFD_TO_IRQn, (configMAX_PRIORITIES - 2));
    NVIC_SetPriority(BTLE_GP_EVENT_IRQn, (configMAX_PRIORITIES - 2));
    NVIC_SetPriority(BTLE_CFO_IRQn, (configMAX_PRIORITIES - 2));
    NVIC_SetPriority(BTLE_SIG_DET_IRQn, (configMAX_PRIORITIES - 2));
    NVIC_SetPriority(BTLE_AGC_EVENT_IRQn, (configMAX_PRIORITIES - 2));
    NVIC_SetPriority(BTLE_RFFE_SPIM_IRQn, (configMAX_PRIORITIES - 2));
    NVIC_SetPriority(BTLE_TX_AES_IRQn, (configMAX_PRIORITIES - 2));
    NVIC_SetPriority(BTLE_RX_AES_IRQn, (configMAX_PRIORITIES - 2));
    NVIC_SetPriority(BTLE_INV_APB_ADDR_IRQn, (configMAX_PRIORITIES - 2));
    NVIC_SetPriority(BTLE_IQ_DATA_VALID_IRQn, (configMAX_PRIORITIES - 2));

    /* Setup scheduler timer priorities */
    NVIC_SetPriority(TMR0_IRQn, (configMAX_PRIORITIES - 1));
    NVIC_SetPriority(TMR1_IRQn, (configMAX_PRIORITIES - 1));

    NVIC_SetPriority(WUT_IRQn, configMAX_PRIORITIES - 1);

    /* Setup additional peripheral timer priorities */
    NVIC_SetPriority(UART0_IRQn, (configMAX_PRIORITIES - 0));
    NVIC_SetPriority(UART1_IRQn, (configMAX_PRIORITIES - 0));
    NVIC_SetPriority(UART2_IRQn, (configMAX_PRIORITIES - 0));
    NVIC_SetPriority(UART3_IRQn, (configMAX_PRIORITIES - 0));

    NVIC_SetPriority(DMA0_IRQn, (configMAX_PRIORITIES - 0));
    NVIC_SetPriority(DMA1_IRQn, (configMAX_PRIORITIES - 0));
    NVIC_SetPriority(DMA2_IRQn, (configMAX_PRIORITIES - 0));
    NVIC_SetPriority(DMA3_IRQn, (configMAX_PRIORITIES - 0));

    NVIC_SetPriority(GPIO0_IRQn, (configMAX_PRIORITIES - 0));
    NVIC_SetPriority(GPIO1_IRQn, (configMAX_PRIORITIES - 0));
}

void trim32k(void)
{
    /* Start the 32 MHz crystal and the BLE DBB counter to trim the 32 kHz crystal */
    PalBbEnable();

    NVIC_EnableIRQ(WUT_IRQn);

    /* Output buffered square wave of 32 kHz clock to GPIO */
    // MXC_RTC_SquareWaveStart(MXC_RTC_F_32KHZ);

    /* Execute the trim procedure */
    wutTrimComplete = 0;
    if (MXC_WUT_TrimCrystalAsync(MXC_WUT0, wutTrimCb) != E_NO_ERROR) {
        APP_TRACE_INFO0("Error with 32k trim");
    } else {
        while (!wutTrimComplete) {}
    }

    /* Shutdown the 32 MHz crystal and the BLE DBB */
    PalBbDisable();
}

/*************************************************************************************************/
/*!
 *  \brief      Initialize BLE.
 *
 *  \return     None.
 */
/*************************************************************************************************/
void bleStartup(void)
{
#if defined(HCI_TR_EXACTLE) && (HCI_TR_EXACTLE == 1)
    /* Configurations must be persistent. */
    static BbRtCfg_t mainBbRtCfg;

    PalBbLoadCfg((PalBbCfg_t *)&mainBbRtCfg);
    LlGetDefaultRunTimeCfg(&mainLlRtCfg);
#if (BT_VER >= LL_VER_BT_CORE_SPEC_5_0)
    /* Set 5.0 requirements. */
    mainLlRtCfg.btVer = LL_VER_BT_CORE_SPEC_5_0;
#endif
    PalCfgLoadData(PAL_CFG_ID_LL_PARAM, &mainLlRtCfg.maxAdvSets, sizeof(LlRtCfg_t) - 9);
#if (BT_VER >= LL_VER_BT_CORE_SPEC_5_0)
    PalCfgLoadData(PAL_CFG_ID_BLE_PHY, &mainLlRtCfg.phy2mSup, 4);
#endif

    /* Set the 32k sleep clock accuracy into one of the following bins, default is 20
      HCI_CLOCK_500PPM
      HCI_CLOCK_250PPM
      HCI_CLOCK_150PPM
      HCI_CLOCK_100PPM
      HCI_CLOCK_75PPM
      HCI_CLOCK_50PPM
      HCI_CLOCK_30PPM
      HCI_CLOCK_20PPM
    */
    mainBbRtCfg.clkPpm = 20;

    /* Set the default connection power level */
    mainLlRtCfg.defTxPwrLvl = DEFAULT_TX_POWER;
#endif

    uint32_t memUsed;
    WsfCsEnter();
    memUsed = WsfBufIoUartInit(WsfHeapGetFreeStartAddress(), PLATFORM_UART_TERMINAL_BUFFER_SIZE);
    WsfHeapAlloc(memUsed);
    WsfCsExit();

    mainWsfInit();
    AppTerminalInit();

#if defined(HCI_TR_EXACTLE) && (HCI_TR_EXACTLE == 1)
    WsfCsEnter();
    LlInitRtCfg_t llCfg = { .pBbRtCfg = &mainBbRtCfg,
                            .wlSizeCfg = 4,
                            .rlSizeCfg = 4,
                            .plSizeCfg = 4,
                            .pLlRtCfg = &mainLlRtCfg,
                            .pFreeMem = WsfHeapGetFreeStartAddress(),
                            .freeMemAvail = WsfHeapCountAvailable() };

    memUsed = LlInit(&llCfg);
    WsfHeapAlloc(memUsed);
    WsfCsExit();

    bdAddr_t bdAddr;
    PalCfgLoadData(PAL_CFG_ID_BD_ADDR, bdAddr, sizeof(bdAddr_t));
    LlSetBdAddr((uint8_t *)&bdAddr);

    trim32k();
#endif

    setInterruptPriority();

#if configUSE_TICKLESS_IDLE
    /* Disable Cordio stack usage of the LEDs */
    PalLedDeInit();
#endif

    StackInitDats();
    DatsStart();
}
