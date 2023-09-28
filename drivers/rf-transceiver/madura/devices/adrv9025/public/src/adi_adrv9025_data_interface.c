// SPDX-License-Identifier: GPL-2.0
/**
* \file adi_adrv9025_data_interface.c
* \brief Contains Data interface features related function implementation defined in
*        adi_adrv9025_data_interface.h
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "adi_adrv9025_data_interface.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025.h"
#include "../../private/include/adrv9025_data_interface.h"
#include "../../private/include/adrv9025_init.h"
#include "../../private/include/adrv9025_reg_addr_macros.h"
#include "../../private/include/adrv9025_bf_tx.h"
#include "../../private/include/adrv9025_bf_jesd_common.h"
#include "../../private/include/adrv9025_bf_jrx_link.h"
#include "../../private/include/adrv9025_bf_jtx_link.h"
#include "../../private/include/adrv9025_bf_rx.h"
#include "../../private/include/adrv9025_bf_core.h"

#ifdef _RELEASE_BUILD_
    #line __LINE__ "adi_adrv9025_data_interface.c"
#endif

int32_t adi_adrv9025_FramerCfgGet(adi_adrv9025_Device_t*   device,
                                  adi_adrv9025_FramerSel_e framerSel,
                                  adi_adrv9025_FrmCfg_t*   framerCfg)
{
    adrv9025_BfJtxLinkChanAddr_e baseAddr          = ADRV9025_BF_JTX_LINK0;
    int32_t                      retVal            = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                      channelId         = 0;
    uint8_t                      dataByte          = 0;
    uint8_t                      converter         = 0;
    uint8_t                      enabledFramers    = 0;
    uint8_t                      framerIdx         = 0;
    uint8_t*                     pLaneFramerOutSel = NULL;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_NULL_PTR_RETURN(&device->common,
                        framerCfg);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    retVal = adrv9025_JesdCommonJtxLinkEnBfGet(device,
                                               ADRV9025_BF_JESD_COMMON,
                                               &enabledFramers);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read jtx_link_en");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* check if the framer is enabled */
    if ((framerSel & enabledFramers) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         framerSel,
                         "Error framer is not enabled");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (framerSel == ADI_ADRV9025_FRAMER_0)
    {
        baseAddr = ADRV9025_BF_JTX_LINK0;
    }
    else if (framerSel == ADI_ADRV9025_FRAMER_1)
    {
        baseAddr  = ADRV9025_BF_JTX_LINK1;
        framerIdx = 1;
    }
    else if (framerSel == ADI_ADRV9025_FRAMER_2)
    {
        baseAddr  = ADRV9025_BF_JTX_LINK2;
        framerIdx = 2;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         framerSel,
                         "Error invalid  Framer Config Get framerSel Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    framerCfg->enableJesd204C = device->devStateInfo.jesdCfg.framerCfg[framerIdx].enableJesd204C;

    retVal = adrv9025_JtxLinkJtxBidCfgBfGet(device,
                                            baseAddr,
                                            &framerCfg->bankId);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read framer config: bankId");
    ADI_ERROR_RETURN(device->common.error.newAction);

    retVal = adrv9025_JtxLinkJtxDidCfgBfGet(device,
                                            baseAddr,
                                            &framerCfg->deviceId);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read framer config: deviceId");
    ADI_ERROR_RETURN(device->common.error.newAction);

    retVal = adrv9025_JtxLinkJtxLidCfgBfGet(device,
                                            baseAddr,
                                            &framerCfg->lane0Id);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read framer config: lane0Id");
    ADI_ERROR_RETURN(device->common.error.newAction);

    retVal = adrv9025_JtxLinkJtxMCfgBfGet(device,
                                          baseAddr,
                                          &framerCfg->jesd204M);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read framer config: jesd204M");
    ADI_ERROR_RETURN(device->common.error.newAction);
    ++framerCfg->jesd204M;

    retVal = adrv9025_JtxLinkJtxKCfgBfGet(device,
                                          baseAddr,
                                          (uint8_t*)&framerCfg->jesd204K);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read framer config: jesd204K");
    ADI_ERROR_RETURN(device->common.error.newAction);
    ++framerCfg->jesd204K;

    retVal = adrv9025_JtxLinkJtxFCfgBfGet(device,
                                          baseAddr,
                                          &framerCfg->jesd204F);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read framer config: jesd204F");
    ADI_ERROR_RETURN(device->common.error.newAction);
    ++framerCfg->jesd204F;

    retVal = adrv9025_JtxLinkJtxNpCfgBfGet(device,
                                           baseAddr,
                                           &framerCfg->jesd204Np);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read framer config: jesd204Np");
    ADI_ERROR_RETURN(device->common.error.newAction);
    ++framerCfg->jesd204Np;

    framerCfg->jesd204E = 0;
    if (framerCfg->enableJesd204C)
    {
        retVal = adrv9025_JtxLinkJtxECfgBfGet(device,
                                              baseAddr,
                                              &framerCfg->jesd204E);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         retVal,
                         NULL,
                         "Error failed to read framer config: jesd204E");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    retVal = adrv9025_JtxLinkJtxScrCfgBfGet(device,
                                            baseAddr,
                                            &framerCfg->scramble);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read framer config: scramble");
    ADI_ERROR_RETURN(device->common.error.newAction);

    framerCfg->serializerLanesEnabled = 0;
    for (channelId = 0; channelId < ADI_ADRV9025_MAX_SERDES_LANES; ++channelId)
    {
        retVal = adrv9025_JtxLinkJtxLaneSelBfGet(device,
                                                 baseAddr,
                                                 channelId,
                                                 &dataByte);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         retVal,
                         NULL,
                         "Error failed to read framer config: serializerLanesEnabled");
        ADI_ERROR_RETURN(device->common.error.newAction);
        if (dataByte < 0x1F)
        {
            framerCfg->serializerLanesEnabled |= (0x1 << channelId);
        }
    }

    retVal = adrv9025_JtxLinkJtxTplPhaseAdjustBfGet(device,
                                                    baseAddr,
                                                    &framerCfg->lmfcOffset);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read framer config: lmfcOffset");
    ADI_ERROR_RETURN(device->common.error.newAction);

    retVal = adrv9025_JtxLinkJtxSysrefForRelinkBfGet(device,
                                                     baseAddr,
                                                     &framerCfg->newSysrefOnRelink);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read framer config: newSysrefOnRelink");
    ADI_ERROR_RETURN(device->common.error.newAction);

    retVal = adrv9025_JtxLinkJtxSyncNSelBfGet(device,
                                              baseAddr,
                                              &framerCfg->syncbInSelect);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read framer config: syncbInSelect");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* currently not used */
    framerCfg->overSample = 0;

    framerCfg->syncbInLvdsMode     = 0;
    framerCfg->syncbInLvdsPnInvert = 0;
    if (framerCfg->syncbInSelect == 0)
    {
        retVal = adrv9025_CoreRxSync1PadLvdsModeBfGet(device,
                                                      ADRV9025_BF_CORE,
                                                      &framerCfg->syncbInLvdsMode);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         retVal,
                         NULL,
                         "Error failed to read framer config: syncbInLvdsMode");
        ADI_ERROR_RETURN(device->common.error.newAction);

        retVal = adrv9025_CoreRxSync1PadLvdsPnInvBfGet(device,
                                                       ADRV9025_BF_CORE,
                                                       &framerCfg->syncbInLvdsPnInvert);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         retVal,
                         NULL,
                         "Error failed to read framer config: syncbInLvdsPnInvert");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (framerCfg->syncbInSelect == 1)
    {
        retVal = adrv9025_CoreRxSync2PadLvdsModeBfGet(device,
                                                      ADRV9025_BF_CORE,
                                                      &framerCfg->syncbInLvdsMode);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         retVal,
                         NULL,
                         "Error failed to read framer config: syncbInLvdsMode");
        ADI_ERROR_RETURN(device->common.error.newAction);

        retVal = adrv9025_CoreRxSync2PadLvdsPnInvBfGet(device,
                                                       ADRV9025_BF_CORE,
                                                       &framerCfg->syncbInLvdsPnInvert);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         retVal,
                         NULL,
                         "Error failed to read framer config: syncbInLvdsPnInvert");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (framerCfg->syncbInSelect == 2)
    {
        retVal = adrv9025_CoreRxSync3PadLvdsModeBfGet(device,
                                                      ADRV9025_BF_CORE,
                                                      &framerCfg->syncbInLvdsMode);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         retVal,
                         NULL,
                         "Error failed to read framer config: syncbInLvdsMode");
        ADI_ERROR_RETURN(device->common.error.newAction);

        retVal = adrv9025_CoreRxSync3PadLvdsPnInvBfGet(device,
                                                       ADRV9025_BF_CORE,
                                                       &framerCfg->syncbInLvdsPnInvert);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         retVal,
                         NULL,
                         "Error failed to read framer config: syncbInLvdsPnInvert");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         retVal,
                         NULL,
                         "Error syncbInSelect is invalid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    pLaneFramerOutSel = &framerCfg->serializerLaneCrossbar.lane0FramerOutSel;
    for (channelId = 0; channelId < ADI_ADRV9025_MAX_SERDES_LANES; ++channelId)
    {
        /* get physical lane selection */
        if ((framerCfg->serializerLanesEnabled & (1 << channelId)) > 0)
        {
            retVal = adrv9025_JtxLinkJtxLaneSelBfGet(device,
                                                     baseAddr,
                                                     channelId,
                                                     pLaneFramerOutSel);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             retVal,
                             NULL,
                             "Error failed to read framer config: serializerLaneCrossbar");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            *pLaneFramerOutSel = 0x04;
        }

        pLaneFramerOutSel++;
    }

    retVal = adi_adrv9025_AdcSampleXbarGet(device,
                                           framerSel,
                                           &framerCfg->adcCrossbar);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read framer config: adcCrossbar");
    ADI_ERROR_RETURN(device->common.error.newAction);
    /* Check jesd204M for number of ADCs, mark the unused ones as ADI_ADRV9025_ADC_DISABLE */
    for (converter = framerCfg->jesd204M; converter < 24; converter++)
    {
        *(&framerCfg->adcCrossbar.conv0 + converter) = ADI_ADRV9025_ADC_DISABLE;
    }

    retVal = adrv9025_JtxLinkJtxSysrefForStartupBfGet(device,
                                                      baseAddr,
                                                      &framerCfg->sysrefForStartup);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read framer config: sysrefForStartup");
    ADI_ERROR_RETURN(device->common.error.newAction);

    retVal = adrv9025_JtxLinkJtxTplSysrefNShotEnableBfGet(device,
                                                          baseAddr,
                                                          &framerCfg->sysrefNShotEnable);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read framer config: sysrefNShotEnable");
    ADI_ERROR_RETURN(device->common.error.newAction);

    retVal = adrv9025_JtxLinkJtxTplSysrefNShotCountBfGet(device,
                                                         baseAddr,
                                                         &framerCfg->sysrefNShotCount);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read framer config: sysrefNShotCount");
    ADI_ERROR_RETURN(device->common.error.newAction);

    retVal = adrv9025_JtxLinkJtxTplSysrefIgnoreWhenLinkedBfGet(device,
                                                               baseAddr,
                                                               &framerCfg->sysrefIgnoreWhenLinked);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Error failed to read framer config: sysrefIgnoreWhenLinked");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_DeframerCfgGet(adi_adrv9025_Device_t*     device,
                                    adi_adrv9025_DeframerSel_e deframerSel,
                                    adi_adrv9025_DfrmCfg_t*    deframerCfg)
{
    int32_t                      retVal                                      = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfJrxLinkChanAddr_e baseAddr                                    = ADRV9025_BF_JRX_LINK0;
    uint8_t                      tempDataByte                                = 0;
    uint8_t                      channelId                                   = 0;
    uint8_t                      arrayLaneSel[ADI_ADRV9025_MAX_SERDES_LANES] = {0};
    uint8_t                      deframerIdx                                 = 0;
    uint8_t                      enabledLinks                                = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_NULL_PTR_RETURN(&device->common,
                        deframerCfg);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if (deframerSel == ADI_ADRV9025_DEFRAMER_0)
    {
        baseAddr    = ADRV9025_BF_JRX_LINK0;
        deframerIdx = 0;
    }

    else if (deframerSel == ADI_ADRV9025_DEFRAMER_1)
    {
        baseAddr    = ADRV9025_BF_JRX_LINK1;
        deframerIdx = 1;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSel,
                         "Error Invalid Deframer deframerSel Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* get Enabled Links */
    retVal = adrv9025_JesdCommonJrxLinkEnBfGet(device,
                                               ADRV9025_BF_JESD_COMMON,
                                               &enabledLinks);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for enabledLinks");
    ADI_ERROR_RETURN(device->common.error.newAction);

    if ((enabledLinks & (uint8_t)deframerSel) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSel,
                         "Selected deframer is not enabled");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* get jesd204C Enabled */
    retVal = adrv9025_JrxLinkJrxLinkTypeBfGet(device,
                                              baseAddr,
                                              &deframerCfg->enableJesd204C);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for enableJesd204C");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* get bankID*/
    /* Bank ID for Jrx is not available, assign it to zero */
    deframerCfg->bankId = 0;

    /* get deviceID */
    retVal = adrv9025_JrxLinkJrxDidCfgBfGet(device,
                                            baseAddr,
                                            &deframerCfg->deviceId);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for deviceId");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* get lane0Id*/
    // TODO: Deframer has 4 lanes each has its own lane ID.
    retVal = adrv9025_JrxLinkJrxLidCfgBfGet(device,
                                            baseAddr,
                                            0,
                                            &deframerCfg->lane0Id);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for lane0Id");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* get M */
    retVal = adrv9025_JrxLinkJrxMCfgBfGet(device,
                                          baseAddr,
                                          &tempDataByte);
    deframerCfg->jesd204M = tempDataByte + 1;
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for jesd204M");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* get K */
    retVal = adrv9025_JrxLinkJrxKCfgBfGet(device,
                                          baseAddr,
                                          &tempDataByte);
    deframerCfg->jesd204K = ((uint16_t)tempDataByte) + 1;
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for jesd204K");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* get Np */
    retVal = adrv9025_JrxLinkJrxNpCfgBfGet(device,
                                           baseAddr,
                                           &tempDataByte);
    deframerCfg->jesd204Np = tempDataByte + 1;
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for jesd204Np");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* get F */
    retVal = adrv9025_JrxLinkJrxFCfgBfGet(device,
                                          baseAddr,
                                          &tempDataByte);
    deframerCfg->jesd204F = tempDataByte + 1;
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for jesd204F");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* get E */
    /* This parameter is only valid for Jesd204C */
    if (deframerCfg->enableJesd204C == 1)
    {
        retVal = adrv9025_JrxLinkJrxDl204cMbReqdCfgBfGet(device,
                                                         baseAddr,
                                                         &deframerCfg->jesd204E);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         retVal,
                         NULL,
                         "Invalid Get for jesd204E");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        deframerCfg->jesd204E = 0;
    }

    /* get scramble */
    retVal = adrv9025_JrxLinkJrxDscrCfgBfGet(device,
                                             baseAddr,
                                             &deframerCfg->scramble);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for scramble");
    ADI_ERROR_RETURN(device->common.error.newAction);

    deframerCfg->deserializerLanesEnabled = 0;

    for (channelId = 0; channelId < ADI_ADRV9025_MAX_SERDES_LANES; channelId++)
    {
        /* get physical lane selection */
        if ((device->devStateInfo.jesdCfg.deframerCfg[deframerIdx].deserializerLanesEnabled & (1 << channelId)) > 0)
        {
            retVal = adrv9025_JrxLinkJrxLinkLaneSelBfGet(device,
                                                         baseAddr,
                                                         channelId,
                                                         &arrayLaneSel[channelId]);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             retVal,
                             NULL,
                             "Invalid Get for deserializerLaneCrossbar");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            arrayLaneSel[channelId] = 0x04;
        }
    }
    /* apply conversion to bitmask to convert from pad lane numbers to ball lane numbers*/
    deframerCfg->deserializerLanesEnabled = device->devStateInfo.jesdCfg.deframerCfg[deframerIdx].deserializerLanesEnabled;

    /* apply conversion to lane selections to convert from pad lane numbers to ball lane numbers*/
    deframerCfg->deserializerLaneCrossbar.deframerInput0LaneSel = arrayLaneSel[0];
    deframerCfg->deserializerLaneCrossbar.deframerInput1LaneSel = arrayLaneSel[1];
    deframerCfg->deserializerLaneCrossbar.deframerInput2LaneSel = arrayLaneSel[2];
    deframerCfg->deserializerLaneCrossbar.deframerInput3LaneSel = arrayLaneSel[3];

    /* get lmfc offset */
    retVal = adrv9025_JrxLinkJrxTplPhaseAdjustBfGet(device,
                                                    baseAddr,
                                                    &deframerCfg->lmfcOffset);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for lmfcOffset");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* get sync out select */
    retVal = adrv9025_JrxLinkJrxSyncNSelBfGet(device,
                                              baseAddr,
                                              &tempDataByte);
    deframerCfg->syncbOutSelect = tempDataByte - 1;
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for syncbOutSelect");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* get syncbOutCmosDriveLevel for both Txsync1 and Txsync2 */
    adrv9025_CoreTxSyncDrvSelectBfGet(device,
                                      ADRV9025_BF_CORE,
                                      &deframerCfg->syncbOutCmosDriveLevel);

    if ((deframerCfg->syncbOutSelect == 0) || (deframerCfg->syncbOutSelect == 2))
    {
        /* get syncbOut Lvds mode */
        retVal = adrv9025_CoreTxSync1PadLvdsModeBfGet(device,
                                                      ADRV9025_BF_CORE,
                                                      &deframerCfg->syncbOutLvdsMode);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         retVal,
                         NULL,
                         "Invalid Get for syncbOutLvdsMode");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* get lvds pin invert */
        retVal = adrv9025_CoreTxSync1PadLvdsPnInvBfGet(device,
                                                       ADRV9025_BF_CORE,
                                                       &deframerCfg->syncbOutLvdsPnInvert);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         retVal,
                         NULL,
                         "Invalid Get for syncbOutLvdsPnInvert");
        ADI_ERROR_RETURN(device->common.error.newAction);

        deframerCfg->syncbOutCmosDriveLevel = deframerCfg->syncbOutCmosDriveLevel & ((uint8_t)0x01);
    }
    else if (deframerCfg->syncbOutSelect == 1)
    {
        /* get syncbOut Lvds mode */
        retVal = adrv9025_CoreTxSync2PadLvdsModeBfGet(device,
                                                      ADRV9025_BF_CORE,
                                                      &deframerCfg->syncbOutLvdsMode);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         retVal,
                         NULL,
                         "Invalid Get for syncbOutLvdsMode");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* get lvds pin invert */
        retVal = adrv9025_CoreTxSync2PadLvdsPnInvBfGet(device,
                                                       ADRV9025_BF_CORE,
                                                       &deframerCfg->syncbOutLvdsPnInvert);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         retVal,
                         NULL,
                         "Invalid Get for syncbOutLvdsPnInvert");
        ADI_ERROR_RETURN(device->common.error.newAction);

        deframerCfg->syncbOutCmosDriveLevel = (deframerCfg->syncbOutCmosDriveLevel & ((uint8_t)0x02)) >> 1;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_ADRV9025_ACT_ERR_RESET_SPI,
                         NULL,
                         "Invalid 'syncbOutSelect', should be 0-2");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* TODO: slew rate is not used, remove this once adi_adrv9025_DfrmCfg_t is updated */
    deframerCfg->syncbOutCmosSlewRate = 0;

    /* get Deframer output to DAC mapping */
    retVal = adi_adrv9025_DacSampleXbarGet(device,
                                           deframerSel,
                                           &deframerCfg->dacCrossbar);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for dacCrossbar");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* get sysref on relink enable */
    retVal = adrv9025_JrxLinkJrxSysrefForRelinkBfGet(device,
                                                     baseAddr,
                                                     &deframerCfg->newSysrefOnRelink);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for newSysrefOnRelink");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* get sysrefForStartup */
    retVal = adrv9025_JrxLinkJrxSysrefForStartupBfGet(device,
                                                      baseAddr,
                                                      &deframerCfg->sysrefForStartup);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for sysrefForStartup");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* get sysrefNhot Enable*/
    retVal = adrv9025_JrxLinkJrxTplSysrefNShotEnableBfGet(device,
                                                          baseAddr,
                                                          &deframerCfg->sysrefNShotEnable);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for sysrefNShotEnable");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* get sysrefNshotCount */
    retVal = adrv9025_JrxLinkJrxTplSysrefNShotCountBfGet(device,
                                                         baseAddr,
                                                         &deframerCfg->sysrefNShotCount);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for sysrefNShotCount");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* get sysrefIgnoreWhenLinked*/
    retVal = adrv9025_JrxLinkJrxTplSysrefIgnoreWhenLinkedBfGet(device,
                                                               baseAddr,
                                                               &deframerCfg->sysrefIgnoreWhenLinked);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Invalid Get for sysrefIgnoreWhenLinked");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_FramerLinkStateSet(adi_adrv9025_Device_t* device,
                                        uint8_t                framerSelMask,
                                        uint8_t                enable)
{
    uint8_t enableBits = (enable > 0) ? 7 : 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if ((framerSelMask <= 0) || (framerSelMask >= 8))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         framerSel,
                         "Error invalid JESD Framer Select Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((enable != 1) && (enable != 0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         enable,
                         "Error invalid JESD Framer Enable Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Note that we are not using a bitfield, since the destination fields are not channelized. */
    ADRV9025_SPIFIELDWRITE(device,
                           ADRV9025_ADDR_JTX_COMMON_0,
                           enableBits,
                           framerSelMask,
                           0,
                           "ADRV9025_ADDR_JTX_COMMON_0");

    return device->common.error.newAction;
}

int32_t adi_adrv9025_FramerLinkStateGet(adi_adrv9025_Device_t* device,
                                        uint8_t*               enable)
{
    uint8_t mask = 0x7;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        enable);

    /* Note that we are not using a bitfield, since the destination fields are not channelized. */
    ADRV9025_SPIFIELDREAD(device,
                          ADRV9025_ADDR_JTX_COMMON_0,
                          enable,
                          mask,
                          0,
                          "ADRV9025_ADDR_JTX_COMMON_0");

    return device->common.error.newAction;
}

int32_t adi_adrv9025_DfrmLinkStateSet(adi_adrv9025_Device_t* device,
                                      uint8_t                deframerSelMask,
                                      uint8_t                enable)
{
    int32_t retVal      = ADI_COMMON_ACT_NO_ACTION;
    uint8_t enableBits  = (enable > 0) ? 3 : 0;
    uint8_t desLaneMask = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if ((deframerSelMask < ADI_ADRV9025_DEFRAMER_0) || (deframerSelMask > ADI_ADRV9025_DEFRAMER_0_AND_1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSelMask,
                         "Error invalid JESD Deframer Select Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((enable != 1) && (enable != 0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         enable,
                         "Error invalid JESD Framer Enable Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Power up/down the appropriate lanes */
    if ((deframerSelMask & (uint8_t)ADI_ADRV9025_DEFRAMER_0) == (uint8_t)ADI_ADRV9025_DEFRAMER_0)
    {
        desLaneMask |= device->devStateInfo.jesdCfg.deframerCfg[0].deserializerLanesEnabled;
    }

    if ((deframerSelMask & (uint8_t)ADI_ADRV9025_DEFRAMER_1) == (uint8_t)ADI_ADRV9025_DEFRAMER_1)
    {
        desLaneMask |= device->devStateInfo.jesdCfg.deframerCfg[1].deserializerLanesEnabled;
    }

    adrv9025_DeserializerLaneEnable(device,
                                    desLaneMask,
                                    enable);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    retVal = adi_common_hal_Wait_ms(&device->common,
                                    50);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     retVal,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Note that we are not using a bitfield, since the destination fields are not channelized. */
    ADRV9025_SPIFIELDWRITE(device,
                           ADRV9025_ADDR_JRX_COMMON_0,
                           enableBits,
                           deframerSelMask,
                           0,
                           "ADRV9025_ADDR_JRX_COMMON_0");

    return device->common.error.newAction;
}

int32_t adi_adrv9025_DfrmLinkStateGet(adi_adrv9025_Device_t* device,
                                      uint8_t*               enable)
{
    uint8_t mask = 0x3;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        enable);

    /* Note that we are not using a bitfield, since the destination fields are not channelized. */
    ADRV9025_SPIFIELDREAD(device,
                          ADRV9025_ADDR_JRX_COMMON_0,
                          enable,
                          mask,
                          0,
                          "ADRV9025_ADDR_JRX_COMMON_0");

    return device->common.error.newAction;
}

int32_t adi_adrv9025_FramerSysrefCtrlSet(adi_adrv9025_Device_t* device,
                                         uint8_t                framerSelMask,
                                         uint8_t                enable)
{
    uint8_t maskSysref = (enable > 0) ? 0 : 1;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if ((framerSelMask <= 0) || (framerSelMask >= 8))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         framerSel,
                         "Error invalid  Framer Sysref framerSel Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((enable != 1) && (enable != 0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         enable,
                         "Error invalid Framer Sysref Enable Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((framerSelMask & (uint8_t)ADI_ADRV9025_FRAMER_0) == (uint8_t)ADI_ADRV9025_FRAMER_0)
    {
        adrv9025_JtxLinkJtxTplSysrefMaskBfSet(device,
                                              ADRV9025_BF_JTX_LINK0,
                                              maskSysref);

        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((framerSelMask & (uint8_t)ADI_ADRV9025_FRAMER_1) == (uint8_t)ADI_ADRV9025_FRAMER_1)
    {
        adrv9025_JtxLinkJtxTplSysrefMaskBfSet(device,
                                              ADRV9025_BF_JTX_LINK1,
                                              maskSysref);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((framerSelMask & (uint8_t)ADI_ADRV9025_FRAMER_2) == (uint8_t)ADI_ADRV9025_FRAMER_2)
    {
        adrv9025_JtxLinkJtxTplSysrefMaskBfSet(device,
                                              ADRV9025_BF_JTX_LINK2,
                                              maskSysref);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_FramerSysrefCtrlGet(adi_adrv9025_Device_t*   device,
                                         adi_adrv9025_FramerSel_e framerSel,
                                         uint8_t*                 enable)
{
    uint8_t maskSysref = 0;
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        enable);

    if (framerSel == ADI_ADRV9025_FRAMER_0)
    {
        adrv9025_JtxLinkJtxTplSysrefMaskBfGet(device,
                                              ADRV9025_BF_JTX_LINK0,
                                              &maskSysref);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (framerSel == ADI_ADRV9025_FRAMER_1)
    {
        adrv9025_JtxLinkJtxTplSysrefMaskBfGet(device,
                                              ADRV9025_BF_JTX_LINK1,
                                              &maskSysref);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (framerSel == ADI_ADRV9025_FRAMER_2)
    {
        adrv9025_JtxLinkJtxTplSysrefMaskBfGet(device,
                                              ADRV9025_BF_JTX_LINK2,
                                              &maskSysref);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         framerSel,
                         "Error invalid  Framer Sysref framerSel Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    *enable = ((~maskSysref) & 0x01);
    return device->common.error.newAction;
}

int32_t adi_adrv9025_DeframerSysrefCtrlSet(adi_adrv9025_Device_t*     device,
                                           adi_adrv9025_DeframerSel_e deframerSel,
                                           uint8_t                    enable)
{
    uint8_t maskSysref = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if ((deframerSel <= 0) || (deframerSel > ADI_ADRV9025_DEFRAMER_0_AND_1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSel,
                         "Error invalid Deframer Sysref deframerSel Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((enable != 1) && (enable != 0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         enable,
                         "Error invalid Deframer Sysref Enable Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    maskSysref = ((~enable) & 0x01);
    if ((deframerSel == ADI_ADRV9025_DEFRAMER_0) ||
        (deframerSel == ADI_ADRV9025_DEFRAMER_0_AND_1))
    {
        adrv9025_JrxLinkJrxTplSysrefMaskBfSet(device,
                                              ADRV9025_BF_JRX_LINK0,
                                              maskSysref);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((deframerSel == ADI_ADRV9025_DEFRAMER_1) ||
        (deframerSel == ADI_ADRV9025_DEFRAMER_0_AND_1))
    {
        adrv9025_JrxLinkJrxTplSysrefMaskBfSet(device,
                                              ADRV9025_BF_JRX_LINK1,
                                              maskSysref);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_DeframerSysrefCtrlGet(adi_adrv9025_Device_t*     device,
                                           adi_adrv9025_DeframerSel_e deframerSel,
                                           uint8_t*                   enable)
{
    uint8_t maskSysref = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        enable);

    if (deframerSel == ADI_ADRV9025_DEFRAMER_0)

    {
        adrv9025_JrxLinkJrxTplSysrefMaskBfGet(device,
                                              ADRV9025_BF_JRX_LINK0,
                                              &maskSysref);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (deframerSel == ADI_ADRV9025_DEFRAMER_1)
    {
        adrv9025_JrxLinkJrxTplSysrefMaskBfGet(device,
                                              ADRV9025_BF_JRX_LINK1,
                                              &maskSysref);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    *enable = ((~maskSysref) & 0x01);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_FramerStatusGet(adi_adrv9025_Device_t       *device,
                                     adi_adrv9025_FramerSel_e     framerSel,
                                     adi_adrv9025_FramerStatus_t* framerStatus)
{
    uint8_t                      sysrefReceived         = 0;
    uint8_t                      sysrefPhaseErr         = 0;
    uint8_t                      syncbLevel             = 0;
    adrv9025_BfJtxLinkChanAddr_e baseAddr               = ADRV9025_BF_JTX_LINK0;
    adrv9025_BfJtxLinkChanAddr_e baseAddrLs             = ADRV9025_BF_JTX_LINK0;
    adrv9025_BfJtxLinkChanAddr_e baseAddrSyncbInSelect  = ADRV9025_BF_JTX_LINK0;
    int32_t                      recoveryAction         = ADI_COMMON_ACT_NO_ACTION;
    adi_adrv9025_FrmCfg_t        framerCfg              = { 0 };
    adi_adrv9025_FrmCfg_t        *framerCfgPtr          = &framerCfg;
    uint8_t                      frameridx              = 0;
    uint8_t                      frameridxLs            = 0;
    static const uint8_t         syncNSelValueZeroCheck = 0;
    static const uint8_t         syncNSelValueOneCheck  = 1;
    static const uint8_t         syncNSelValueTwoCheck  = 2;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, framerStatus);

    if (framerSel == ADI_ADRV9025_FRAMER_0)
    {
        baseAddr = ADRV9025_BF_JTX_LINK0;
        frameridx = 0;
    }
    else if (framerSel == ADI_ADRV9025_FRAMER_1)
    {
        baseAddr = ADRV9025_BF_JTX_LINK1;
        frameridx = 1;
    }
    else if (framerSel == ADI_ADRV9025_FRAMER_2)
    {
        baseAddr = ADRV9025_BF_JTX_LINK2;
        frameridx = 2;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         framerSel,
                         "Error invalid  Framer Status framerSel Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* In Link Sharing profiles framer 0 is used to transmit data from both Rx and ORx. */
    /* When querying the JESD link status, the SYSREF phase error bit should be read from framer 1 for link sharing profiles.*/
    if ((device->devStateInfo.linkSharingEnabled > 0) && (framerSel == ADI_ADRV9025_FRAMER_0))
    {
        baseAddrLs = ADRV9025_BF_JTX_LINK1;
        frameridxLs = 1;
    }
    else
    {
        baseAddrLs = baseAddr;
        frameridxLs = frameridx;
    }

    // Read state for JESD204C
    if(device->devStateInfo.jesdCfg.framerCfg[frameridxLs].enableJesd204C == 1)
    {
        adrv9025_JtxLinkJtxDl204cSysrefRcvdGet(device, baseAddr, &framerStatus->status);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    // Read state for JESD204B
    else
    {
        /* Calling adrv9025_JtxLinkJtxSyncNSelBfGet to return the current value for syncbInSelect */
        /* syncbInSelect is needed for the API to target the correct register when syncbInSelect is not set to default value of zero */
        recoveryAction = adrv9025_JtxLinkJtxSyncNSelBfGet(device, baseAddr, &framerCfgPtr->syncbInSelect);

        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Error failed to read framer config: syncbInSelect");
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_JtxLinkJtxTplCfgInvalidBfGet(device, baseAddr, &framerStatus->status);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_JtxLinkJtxTplSysrefRcvdBfGet(device, baseAddr, &sysrefReceived);
        ADI_ERROR_RETURN(device->common.error.newAction);

        framerStatus->status |= ((sysrefReceived << 1) & 0x2);

        adrv9025_JtxLinkJtxTplSysrefPhaseErrBfGet(device, baseAddrLs, &sysrefPhaseErr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        framerStatus->status |= ((sysrefPhaseErr << 2) & 0x4);

        /* Read the syncbInSelect and then select the correct register to read */
        if (framerCfg.syncbInSelect == syncNSelValueZeroCheck)
        {
            /* If syncNSel is equal to zero, register 0x6E68 should be read */
            /* Base address 0x6E00, i.e ADRV9025_BF_JTX_LINK0 needs to be passed */
            baseAddrSyncbInSelect = ADRV9025_BF_JTX_LINK0;
        }
        else if (framerCfg.syncbInSelect == syncNSelValueOneCheck)
        {
            /* If syncNSel is equal to one, then the API needs to read from register 0x7068 */
            /* Base aaddress 0x7000 i.e ADRV9025_BF_JTX_LINK1 needs to be passed */
            baseAddrSyncbInSelect = ADRV9025_BF_JTX_LINK1;
        }
        else if (framerCfg.syncbInSelect == syncNSelValueTwoCheck)
        {
            /* If syncNSel is equal to two, then the API needs to read from register 0x7268 */
            /* Base aaddress 0x7200 i.e ADRV9025_BF_JTX_LINK2 needs to be passed */
            baseAddrSyncbInSelect = ADRV9025_BF_JTX_LINK2;
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             framerCfg.syncbInSelect,
                             "Error invalid syncbInSelect Value");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        adrv9025_JtxLinkJtxDl204bSyncNBfGet(device, baseAddrSyncbInSelect, &syncbLevel);
        ADI_ERROR_RETURN(device->common.error.newAction);

        framerStatus->status |= ((syncbLevel << 3) & 0x8);

        adrv9025_JtxLinkJtxSyncNSelBfGet(device, baseAddr, &framerStatus->syncNSel);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_JtxLinkJtxDl204bSyncNeCountBfGet(device, baseAddr, &framerStatus->framerSyncNeCount);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_JtxLinkJtxDl204bStateBfGet(device, baseAddr, &framerStatus->qbfStateStatus);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    return device->common.error.newAction;
}
int32_t adi_adrv9025_DeframerStatusGet(adi_adrv9025_Device_t*         device,
                                       adi_adrv9025_DeframerSel_e     deframerSel,
                                       adi_adrv9025_DeframerStatus_t* deframerStatus)
{
    uint8_t                      dfrmSysrefReceived    = 0;
    uint8_t                      dfrm204bUserDataState = 0;
    uint8_t                      dfrmFsLost            = 0;
    uint8_t                      dfrmEomfEvent         = 0;
    uint8_t                      dfrmEofEvent          = 0;
    uint8_t                      dfrmValidChecksum     = 0;
    uint8_t                      channelId             = 0;
    uint8_t                      deframerIdx           = 0;
    uint8_t                      laneSel               = 0;
    uint8_t                      laneIdx               = 0;
    adrv9025_BfJrxLinkChanAddr_e baseAddr              = ADRV9025_BF_JRX_LINK0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, deframerStatus);

    if (deframerSel == ADI_ADRV9025_DEFRAMER_0)
    {
        baseAddr    = ADRV9025_BF_JRX_LINK0;
        deframerIdx = 0;
    }
    else if (deframerSel == ADI_ADRV9025_DEFRAMER_1)
    {
        baseAddr    = ADRV9025_BF_JRX_LINK1;
        deframerIdx = 1;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         JESDdata->deframerSel,
                         "Error invalid  Deframer Status deframerSel Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (channelId = 0; channelId < ADI_ADRV9025_NUM_LANES; channelId++)
    {
        laneSel = 1 << channelId;
        if ((device->devStateInfo.jesdCfg.deframerCfg[deframerIdx].deserializerLanesEnabled & laneSel) == laneSel)
        {
            // First lane in use.
            laneIdx = channelId;
            break;
        }
    }

    // Read state for JESD204C
    if (device->devStateInfo.jesdCfg.deframerCfg[deframerIdx].enableJesd204C == 1)
    {
        adrv9025_JrxLinkJrxDl204cStateGet(device, baseAddr, laneIdx, &deframerStatus->status);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    // Read state for JESD204B
    else
    {
        adrv9025_JrxLinkJrxDl204bSyncNBfGet(device,
                                         baseAddr,
                                         laneIdx,
                                         &deframerStatus->status);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_JrxLinkJrxTplSysrefRcvdBfGet(device,
                                          baseAddr,
                                          &dfrmSysrefReceived);
        ADI_ERROR_RETURN(device->common.error.newAction);
        deframerStatus->status |= ((dfrmSysrefReceived << 1) & 0x2);

        adrv9025_JrxLinkJrxDl204bUserDataBfGet(device,
                                           baseAddr,
                                           laneIdx,
                                           &dfrm204bUserDataState);
        ADI_ERROR_RETURN(device->common.error.newAction);
        deframerStatus->status |= ((dfrm204bUserDataState << 2) & 0x4);

        adrv9025_JrxLinkJrxDl204bFsLostBfGet(device,
                                         baseAddr,
                                         laneIdx,
                                         &dfrmFsLost);
        ADI_ERROR_RETURN(device->common.error.newAction);
        deframerStatus->status |= ((dfrmFsLost << 4) & 0x10);

        adrv9025_JrxLinkJrxDl204bEomfEventBfGet(device,
                                            baseAddr,
                                            laneIdx,
                                            &dfrmEomfEvent);
        ADI_ERROR_RETURN(device->common.error.newAction);
        deframerStatus->status |= ((dfrmEomfEvent << 5) & 0x20);

        adrv9025_JrxLinkJrxDl204bEofEventBfGet(device,
                                           baseAddr,
                                           laneIdx,
                                           &dfrmEofEvent);
        ADI_ERROR_RETURN(device->common.error.newAction);
        deframerStatus->status |= ((dfrmEofEvent << 6) & 0x40);

        adrv9025_JrxLinkJrxDl204bValidCksumBfGet(device,
                                             baseAddr,
                                             laneIdx,
                                             &dfrmValidChecksum);
        ADI_ERROR_RETURN(device->common.error.newAction);
        deframerStatus->status |= ((dfrmValidChecksum << 7) & 0x80);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_DacSampleXbarSet(adi_adrv9025_Device_t*           device,
                                      adi_adrv9025_DeframerSel_e       deframerSel,
                                      adi_adrv9025_DacSampleXbarCfg_t* dacXbar)
{
    int32_t                      recoveryAction   = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                      mValue           = 0;
    uint8_t                      channelIndex     = 0;
    uint8_t                      disabledCount    = 0;
    adrv9025_BfJrxLinkChanAddr_e baseAddr         = ADRV9025_BF_JRX_LINK0;
    static const uint8_t         MAXNUMCONVERTERS = 8;
    uint8_t                      arrayXBarCfg[]   = {0, 0, 0, 0, 0, 0, 0, 0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        dacXbar);

    if (deframerSel == ADI_ADRV9025_DEFRAMER_0)
    {
        baseAddr = ADRV9025_BF_JRX_LINK0;
    }
    else if (deframerSel == ADI_ADRV9025_DEFRAMER_1)
    {
        baseAddr = ADRV9025_BF_JRX_LINK1;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSel,
                         "Invalid Deframer Selection");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* read back the configured M value for the deframer to write */
    adrv9025_JrxLinkJrxMCfgBfGet(device,
                                 baseAddr,
                                 &mValue);
    ADI_ERROR_RETURN(recoveryAction);

    /* load the config array */
    arrayXBarCfg[0] = (uint8_t)dacXbar->tx1DacChanI;
    arrayXBarCfg[1] = (uint8_t)dacXbar->tx1DacChanQ;
    arrayXBarCfg[2] = (uint8_t)dacXbar->tx2DacChanI;
    arrayXBarCfg[3] = (uint8_t)dacXbar->tx2DacChanQ;
    arrayXBarCfg[4] = (uint8_t)dacXbar->tx3DacChanI;
    arrayXBarCfg[5] = (uint8_t)dacXbar->tx3DacChanQ;
    arrayXBarCfg[6] = (uint8_t)dacXbar->tx4DacChanI;
    arrayXBarCfg[7] = (uint8_t)dacXbar->tx4DacChanQ;

    /* Count number of disabled selections, this should be equal to 'MAXNUMCONVERTERS -(mValue + 1)' */
    for (channelIndex = 0; channelIndex < MAXNUMCONVERTERS; channelIndex++)
    {
        if (arrayXBarCfg[channelIndex] == (uint8_t)ADI_ADRV9025_DEFRAMER_OUT_DISABLE)
        {
            disabledCount++;
        }
        else if ((arrayXBarCfg[channelIndex] != (uint8_t)ADI_ADRV9025_DEFRAMER_OUT0) && (arrayXBarCfg[channelIndex] != (uint8_t)ADI_ADRV9025_DEFRAMER_OUT1)
            && (arrayXBarCfg[channelIndex] != (uint8_t)ADI_ADRV9025_DEFRAMER_OUT2) && (arrayXBarCfg[channelIndex] != (uint8_t)ADI_ADRV9025_DEFRAMER_OUT3)
            && (arrayXBarCfg[channelIndex] != (uint8_t)ADI_ADRV9025_DEFRAMER_OUT4) && (arrayXBarCfg[channelIndex] != (uint8_t)ADI_ADRV9025_DEFRAMER_OUT5)
            && (arrayXBarCfg[channelIndex] != (uint8_t)ADI_ADRV9025_DEFRAMER_OUT6) && (arrayXBarCfg[channelIndex] != (uint8_t)ADI_ADRV9025_DEFRAMER_OUT7))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             arrayXBarCfg,
                             "Invalid DAC Output selection");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    if (disabledCount != (MAXNUMCONVERTERS - (mValue + 1)))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         disabledCount,
                         "Number of valid converter selections doesn't match with deframer m value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (channelIndex = 0; channelIndex < MAXNUMCONVERTERS; channelIndex++)
    {
        recoveryAction = adrv9025_JrxLinkJrxConvSelBfSet(device,
                                                         baseAddr,
                                                         channelIndex,
                                                         arrayXBarCfg[channelIndex]);
        ADI_ERROR_RETURN(recoveryAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_DacSampleXbarGet(adi_adrv9025_Device_t*           device,
                                      adi_adrv9025_DeframerSel_e       deframerSel,
                                      adi_adrv9025_DacSampleXbarCfg_t* dacXbar)
{
    int32_t                      recoveryAction   = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                      channelIndex     = 0;
    adrv9025_BfJrxLinkChanAddr_e baseAddr         = ADRV9025_BF_JRX_LINK0;
    static const uint8_t         MAXNUMCONVERTERS = 8;
    uint8_t                      arrayXBarCfg[]   = {0, 0, 0, 0, 0, 0, 0, 0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        dacXbar);

    if (deframerSel == ADI_ADRV9025_DEFRAMER_0)
    {
        baseAddr = ADRV9025_BF_JRX_LINK0;
    }
    else if (deframerSel == ADI_ADRV9025_DEFRAMER_1)
    {
        baseAddr = ADRV9025_BF_JRX_LINK1;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSel,
                         "Invalid Deframer Selection");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (channelIndex = 0; channelIndex < MAXNUMCONVERTERS; channelIndex++)
    {
        recoveryAction = adrv9025_JrxLinkJrxConvSelBfGet(device,
                                                         baseAddr,
                                                         channelIndex,
                                                         &arrayXBarCfg[channelIndex]);
        ADI_ERROR_RETURN(recoveryAction);
    }

    /* load the dacXbar struct */
    dacXbar->tx1DacChanI = (adi_adrv9025_DacSampleXbarSel_e)arrayXBarCfg[0];
    dacXbar->tx1DacChanQ = (adi_adrv9025_DacSampleXbarSel_e)arrayXBarCfg[1];
    dacXbar->tx2DacChanI = (adi_adrv9025_DacSampleXbarSel_e)arrayXBarCfg[2];
    dacXbar->tx2DacChanQ = (adi_adrv9025_DacSampleXbarSel_e)arrayXBarCfg[3];
    dacXbar->tx3DacChanI = (adi_adrv9025_DacSampleXbarSel_e)arrayXBarCfg[4];
    dacXbar->tx3DacChanQ = (adi_adrv9025_DacSampleXbarSel_e)arrayXBarCfg[5];
    dacXbar->tx4DacChanI = (adi_adrv9025_DacSampleXbarSel_e)arrayXBarCfg[6];
    dacXbar->tx4DacChanQ = (adi_adrv9025_DacSampleXbarSel_e)arrayXBarCfg[7];

    return device->common.error.newAction;
}

int32_t adi_adrv9025_AdcSampleXbarSet(adi_adrv9025_Device_t*           device,
                                      adi_adrv9025_FramerSel_e         framerSel,
                                      adi_adrv9025_AdcSampleXbarCfg_t* adcXbar)
{
    int32_t                      recoveryAction   = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfJtxLinkChanAddr_e baseAddress      = ADRV9025_BF_JTX_LINK0;
    static const uint8_t         MAXNUMCONVERTERS = 24;
    static const uint8_t         CONVERTERDISABLE = 0x7F;
    static const uint8_t         OFFSET           = 1;
    uint8_t                      channelIndex     = 0;
    uint8_t                      mValue           = 0;
    uint8_t                      adcXbardata[]    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        adcXbar);

    switch (framerSel)
    {
    case ADI_ADRV9025_FRAMER_0:
        baseAddress = ADRV9025_BF_JTX_LINK0;
        break;
    case ADI_ADRV9025_FRAMER_1:
        baseAddress = ADRV9025_BF_JTX_LINK1;
        break;
    case ADI_ADRV9025_FRAMER_2:
        baseAddress = ADRV9025_BF_JTX_LINK2;
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         framerSel,
                         "Invalid  Framer Selection");
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;
    }

    /* read back the configured M value and set the offset */
    recoveryAction = adrv9025_JtxLinkJtxMCfgBfGet(device,
                                                  baseAddress,
                                                  &mValue);
    ADI_ERROR_RETURN(recoveryAction);
    mValue += OFFSET;

    /* load the adc config array */
    adcXbardata[0]  = (uint8_t)adcXbar->conv0;
    adcXbardata[1]  = (uint8_t)adcXbar->conv1;
    adcXbardata[2]  = (uint8_t)adcXbar->conv2;
    adcXbardata[3]  = (uint8_t)adcXbar->conv3;
    adcXbardata[4]  = (uint8_t)adcXbar->conv4;
    adcXbardata[5]  = (uint8_t)adcXbar->conv5;
    adcXbardata[6]  = (uint8_t)adcXbar->conv6;
    adcXbardata[7]  = (uint8_t)adcXbar->conv7;
    adcXbardata[8]  = (uint8_t)adcXbar->conv8;
    adcXbardata[9]  = (uint8_t)adcXbar->conv9;
    adcXbardata[10] = (uint8_t)adcXbar->conv10;
    adcXbardata[11] = (uint8_t)adcXbar->conv11;
    adcXbardata[12] = (uint8_t)adcXbar->conv12;
    adcXbardata[13] = (uint8_t)adcXbar->conv13;
    adcXbardata[14] = (uint8_t)adcXbar->conv14;
    adcXbardata[15] = (uint8_t)adcXbar->conv15;
    adcXbardata[16] = (uint8_t)adcXbar->conv16;
    adcXbardata[17] = (uint8_t)adcXbar->conv17;
    adcXbardata[18] = (uint8_t)adcXbar->conv18;
    adcXbardata[19] = (uint8_t)adcXbar->conv19;
    adcXbardata[20] = (uint8_t)adcXbar->conv20;
    adcXbardata[21] = (uint8_t)adcXbar->conv21;
    adcXbardata[22] = (uint8_t)adcXbar->conv22;
    adcXbardata[23] = (uint8_t)adcXbar->conv23;

    /* program the converters */
    for (channelIndex = 0; channelIndex < MAXNUMCONVERTERS; channelIndex++)
    {
        /* program converter if channel is not greater than M-1 */
        if (channelIndex < mValue)
        {
            /* range check the converter settings */
            if ((adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DISABLE) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_RX1_I) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_RX1_Q) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_RX2_I) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_RX2_Q) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_RX3_I) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_RX3_Q) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_RX4_I) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_RX4_Q) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DUALBAND_RX1_BAND_A_I) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DUALBAND_RX1_BAND_A_Q) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DUALBAND_RX2_BAND_A_I) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DUALBAND_RX2_BAND_A_Q) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DUALBAND_RX3_BAND_A_I) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DUALBAND_RX3_BAND_A_Q) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DUALBAND_RX4_BAND_A_I) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DUALBAND_RX4_BAND_A_Q) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DUALBAND_RX1_BAND_B_I) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DUALBAND_RX1_BAND_B_Q) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DUALBAND_RX2_BAND_B_I) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DUALBAND_RX2_BAND_B_Q) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DUALBAND_RX3_BAND_B_I) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DUALBAND_RX3_BAND_B_Q) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DUALBAND_RX4_BAND_B_I) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_DUALBAND_RX4_BAND_B_Q) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_ORX1_I) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_ORX1_Q) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_ORX2_I) &&
                (adcXbardata[channelIndex] != ADI_ADRV9025_ADC_ORX2_Q))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 adcXbardata[channelIndex],
                                 "Invalid ADC Converter Selection");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else
            {
                adrv9025_JtxLinkJtxConvSelBfSet(device,
                                                baseAddress,
                                                channelIndex,
                                                adcXbardata[channelIndex]);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

            /* disable converter if channel greater than M */
        else
        {
            adrv9025_JtxLinkJtxConvSelBfSet(device,
                                            baseAddress,
                                            channelIndex,
                                            CONVERTERDISABLE);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_AdcSampleXbarGet(adi_adrv9025_Device_t*           device,
                                      adi_adrv9025_FramerSel_e         framerSel,
                                      adi_adrv9025_AdcSampleXbarCfg_t* adcXbar)
{
    adrv9025_BfJtxLinkChanAddr_e baseAddress = ADRV9025_BF_JTX_LINK0;
    uint8_t                      bfValue     = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        adcXbar);

    switch (framerSel)
    {
    case ADI_ADRV9025_FRAMER_0:
        baseAddress = ADRV9025_BF_JTX_LINK0;
        break;
    case ADI_ADRV9025_FRAMER_1:
        baseAddress = ADRV9025_BF_JTX_LINK1;
        break;
    case ADI_ADRV9025_FRAMER_2:
        baseAddress = ADRV9025_BF_JTX_LINK2;
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         framerSel,
                         "Invalid Framer Selection");
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;
    }

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    0,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv0 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    1,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv1 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    2,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv2 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    3,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv3 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    4,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv4 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    5,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv5 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    6,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv6 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    7,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv7 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    8,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv8 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    9,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv9 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    10,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv10 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    11,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv11 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    12,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv12 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    13,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv13 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    14,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv14 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    15,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv15 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    16,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv16 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    17,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv17 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    18,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv18 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    19,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv19 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    20,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv20 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    21,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv21 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    22,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv22 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    adrv9025_JtxLinkJtxConvSelBfGet(device,
                                    baseAddress,
                                    23,
                                    &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adcXbar->conv23 = (adi_adrv9025_AdcSampleXbarSel_e)bfValue;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_FramerTestDataSet(adi_adrv9025_Device_t*         device,
                                       adi_adrv9025_FrmTestDataCfg_t* frmTestDataCfg)
{
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        frmTestDataCfg);

    if (((frmTestDataCfg->testDataSource < ADI_ADRV9025_FTD_ADC_DATA) || (frmTestDataCfg->testDataSource > ADI_ADRV9025_FTD_RAMP)) &&
        (frmTestDataCfg->testDataSource != ADI_ADRV9025_FTD_PATTERN_REPEAT) &&
        (frmTestDataCfg->testDataSource != ADI_ADRV9025_FTD_PATTERN_ONCE))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         frmTestDataCfg->testDataSource,
                         "Invalid DataSource Selection");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (frmTestDataCfg->framerSelMask >= 8)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         framerSel,
                         "Invalid Framer Selection");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((frmTestDataCfg->injectPoint > ADI_ADRV9025_FTD_POST_LANEMAP) || (frmTestDataCfg->injectPoint < ADI_ADRV9025_FTD_FRAMERINPUT))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         frmTestDataCfg->injectPoint,
                         "Invalid DataInject Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((frmTestDataCfg->framerSelMask & 0x01) == 0x01)
    {
        adrv9025_JtxLinkJtxTestGenModeBfSet(device,
                                            ADRV9025_BF_JTX_LINK0,
                                            frmTestDataCfg->testDataSource);
        ADI_ERROR_RETURN(device->common.error.newAction);
        adrv9025_JtxLinkJtxTestGenSelBfSet(device,
                                           ADRV9025_BF_JTX_LINK0,
                                           frmTestDataCfg->injectPoint);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((frmTestDataCfg->framerSelMask & 0x02) == 0x02)
    {
        adrv9025_JtxLinkJtxTestGenModeBfSet(device,
                                            ADRV9025_BF_JTX_LINK1,
                                            frmTestDataCfg->testDataSource);
        ADI_ERROR_RETURN(device->common.error.newAction);
        adrv9025_JtxLinkJtxTestGenSelBfSet(device,
                                           ADRV9025_BF_JTX_LINK1,
                                           frmTestDataCfg->injectPoint);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((frmTestDataCfg->framerSelMask & 0x04) == 0x4)
    {
        adrv9025_JtxLinkJtxTestGenModeBfSet(device,
                                            ADRV9025_BF_JTX_LINK2,
                                            frmTestDataCfg->testDataSource);
        ADI_ERROR_RETURN(device->common.error.newAction);
        adrv9025_JtxLinkJtxTestGenSelBfSet(device,
                                           ADRV9025_BF_JTX_LINK2,
                                           frmTestDataCfg->injectPoint);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_FramerTestDataGet(adi_adrv9025_Device_t*         device,
                                       adi_adrv9025_FramerSel_e       framerSelect,
                                       adi_adrv9025_FrmTestDataCfg_t* frmTestDataCfg)
{
    adrv9025_BfJtxLinkChanAddr_e baseAddr       = ADRV9025_BF_JTX_LINK0;
    uint8_t                      injectionPoint = 0;
    uint8_t                      dataSource     = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        frmTestDataCfg);

    if (framerSelect == ADI_ADRV9025_FRAMER_0)
    {
        baseAddr = ADRV9025_BF_JTX_LINK0;
    }
    else if (framerSelect == ADI_ADRV9025_FRAMER_1)
    {
        baseAddr = ADRV9025_BF_JTX_LINK1;
    }
    else if (framerSelect == ADI_ADRV9025_FRAMER_2)
    {
        baseAddr = ADRV9025_BF_JTX_LINK2;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         framerSelect,
                         "Error invalid  Framer Status framerSel Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    adrv9025_JtxLinkJtxTestGenSelBfGet(device,
                                       baseAddr,
                                       &injectionPoint);
    ADI_ERROR_RETURN(device->common.error.newAction);
    frmTestDataCfg->injectPoint = (adi_adrv9025_FramerDataInjectPoint_e)injectionPoint;

    adrv9025_JtxLinkJtxTestGenModeBfGet(device,
                                        baseAddr,
                                        &dataSource);
    ADI_ERROR_RETURN(device->common.error.newAction);
    frmTestDataCfg->testDataSource = (adi_adrv9025_FramerDataSource_e)dataSource;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_FramerTestDataInjectError(adi_adrv9025_Device_t*   device,
                                               adi_adrv9025_FramerSel_e framerSelect,
                                               uint8_t                  laneMask)
{
    adrv9025_BfJtxLinkChanAddr_e baseAddr         = ADRV9025_BF_JTX_LINK0;
    uint8_t                      ballLane         = 0;
    uint8_t                      invert           = 0;
    uint8_t                      pnInvertBit      = 0;
    uint8_t                      dieLaneId        = 0;
    uint8_t                      framerOutForLane = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if (framerSelect == ADI_ADRV9025_FRAMER_0)
    {
        baseAddr = ADRV9025_BF_JTX_LINK0;
    }
    else if (framerSelect == ADI_ADRV9025_FRAMER_1)
    {
        baseAddr = ADRV9025_BF_JTX_LINK1;
    }
    else if (framerSelect == ADI_ADRV9025_FRAMER_2)
    {
        baseAddr = ADRV9025_BF_JTX_LINK2;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSel,
                         "Error invalid framerSel Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (ballLane = 0; ballLane <= 7; ballLane++)
    {
        invert = ((laneMask >> ballLane) & 0x01);

        if (invert == 1)
        {
            dieLaneId = adrv9025_SerializerLamLaneOffsetGet(ballLane);

            /* PN Invert occurs at framer output before lane crossbar */
            adrv9025_JtxLinkJtxLaneSelBfGet(device,
                                            baseAddr,
                                            dieLaneId,
                                            &framerOutForLane);

            if (framerOutForLane <= 7)
            {
                /* Read current PN invert setting and set it back at end*/
                adrv9025_JtxLinkJtxLaneInvBfGet(device,
                                                baseAddr,
                                                framerOutForLane,
                                                &pnInvertBit);
                ADI_ERROR_RETURN(device->common.error.newAction);

                adrv9025_JtxLinkJtxLaneInvBfSet(device,
                                                baseAddr,
                                                framerOutForLane,
                                                ((~pnInvertBit) & 0x01));
                ADI_ERROR_RETURN(device->common.error.newAction);

                adrv9025_JtxLinkJtxLaneInvBfSet(device,
                                                baseAddr,
                                                framerOutForLane,
                                                pnInvertBit);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else
            {
                /* Lane disabled - do not invert */
            }
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_DfrmPrbsCheckerStateSet(adi_adrv9025_Device_t*      device,
                                             adi_adrv9025_DfrmPrbsCfg_t* dfrmPrbsCfg)
{
    int32_t recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    uint8_t prbsCheckLocation = 0;
    uint8_t pbrsPolyOrder     = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        dfrmPrbsCfg);

    if (((dfrmPrbsCfg->polyOrder == ADI_ADRV9025_PRBS_DISABLE) ||
            (dfrmPrbsCfg->polyOrder == ADI_ADRV9025_PRBS7) ||
            (dfrmPrbsCfg->polyOrder == ADI_ADRV9025_PRBS9) ||
            (dfrmPrbsCfg->polyOrder == ADI_ADRV9025_PRBS15) ||
            (dfrmPrbsCfg->polyOrder == ADI_ADRV9025_PRBS31) ||
            (dfrmPrbsCfg->polyOrder == ADI_ADRV9025_USERDATA)) &&
        ((dfrmPrbsCfg->checkerLocation == ADI_ADRV9025_PRBSCHECK_LANEDATA) ||
            (dfrmPrbsCfg->checkerLocation == ADI_ADRV9025_PRBSCHECK_SAMPLEDATA)))
    {
        prbsCheckLocation = (uint8_t)dfrmPrbsCfg->checkerLocation;
        pbrsPolyOrder     = dfrmPrbsCfg->polyOrder;

        recoveryAction = adi_adrv9025_DfrmPrbsCountReset(device);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to reset Deframer PRBS count");
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_JesdCommonJrxTestSourceBfSet(device,
                                              ADRV9025_BF_JESD_COMMON,
                                              prbsCheckLocation);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_JesdCommonJrxTestModeBfSet(device,
                                            ADRV9025_BF_JESD_COMMON,
                                            pbrsPolyOrder);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dfrmPrbsCfg->polyOrder,
                         "Invalid polyOrder/checkerLocation value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_DfrmPrbsCheckerStateGet(adi_adrv9025_Device_t*      device,
                                             adi_adrv9025_DfrmPrbsCfg_t* dfrmPrbsCfg)
{
    uint32_t recoveryAction    = (int32_t)ADI_COMMON_HAL_OK;
    uint8_t  prbsCheckLocation = 0;
    uint8_t  pbrsPolyOrder     = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    recoveryAction = adrv9025_JesdCommonJrxTestModeBfGet(device,
                                                         ADRV9025_BF_JESD_COMMON,
                                                         &pbrsPolyOrder);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to get Deframer PRBS mode");
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_JesdCommonJrxTestSourceBfGet(device,
                                          ADRV9025_BF_JESD_COMMON,
                                          &prbsCheckLocation);
    ADI_ERROR_RETURN(device->common.error.newAction);

    dfrmPrbsCfg->checkerLocation = (adi_adrv9025_DeframerPrbsCheckLoc_e)prbsCheckLocation;
    dfrmPrbsCfg->polyOrder       = (adi_adrv9025_DeframerPrbsOrder_e)pbrsPolyOrder;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_DfrmPrbsCountReset(adi_adrv9025_Device_t* device)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Write the bit value to a 1 and then to a zero in order to clear the register, its not self clearing */
    recoveryAction = adrv9025_JesdCommonJrxTestLaneClearErrorsBfSet(device,
                                                                    ADRV9025_BF_JESD_COMMON,
                                                                    1);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to clear deframer lane PRBS errors");
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JesdCommonJrxTestLaneClearErrorsBfSet(device,
                                                                    ADRV9025_BF_JESD_COMMON,
                                                                    0);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to clear deframer lane PRBS errors");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Write the bit value to a 1 and then to a zero in order to clear the register, its not self clearing */
    recoveryAction = adrv9025_JesdCommonJrxTestSampleClearErrorsBfSet(device,
                                                                      ADRV9025_BF_JESD_COMMON,
                                                                      1);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to clear deframer sample PRBS errors");
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JesdCommonJrxTestSampleClearErrorsBfSet(device,
                                                                      ADRV9025_BF_JESD_COMMON,
                                                                      0);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to clear deframer sample PRBS errors");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_DfrmPrbsErrCountGet(adi_adrv9025_Device_t*              device,
                                         adi_adrv9025_DfrmPrbsErrCounters_t* counters)
{
    int32_t recoveryAction      = ADI_COMMON_ACT_NO_ACTION;
    uint8_t prbsInvalidDataFlag = 0;
    uint8_t prbsErrorFlag       = 0;
    uint8_t prbsLaneInv         = 0;
    uint8_t lane                = 0;
    uint8_t sampleSource        = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common,
                        counters);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    recoveryAction = adrv9025_JesdCommonJrxTestSourceBfGet(device,
                                                           ADRV9025_BF_JESD_COMMON,
                                                           &sampleSource);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to read deframer PRBS errors");
    ADI_ERROR_RETURN(device->common.error.newAction);
    counters->sampleSource = (adi_adrv9025_DeframerPrbsCheckLoc_e)sampleSource;

    if (counters->sampleSource == ADI_ADRV9025_PRBSCHECK_SAMPLEDATA)
    {
        /* Trigger update of the sample error counters */
        adrv9025_JesdCommonJrxTestSampleUpdateErrorCountBfSet(device,
                                                              ADRV9025_BF_JESD_COMMON,
                                                              1);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Now read the sample counter value. We use the zeroth index only in sample mode. */
        adrv9025_JesdCommonJrxTestSampleErrorCountBfGet(device,
                                                        ADRV9025_BF_JESD_COMMON,
                                                        (uint8_t *)&counters->laneErrors[0]);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_JesdCommonJrxTestSampleErrorFlagBfGet(device,
                                                       ADRV9025_BF_JESD_COMMON,
                                                       &prbsErrorFlag);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_JesdCommonJrxTestSampleClearErrorsBfGet(device,
                                                         ADRV9025_BF_JESD_COMMON,
                                                         &prbsInvalidDataFlag);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Create the errorStatus. Note that there is no inverted value flag specific to sample mode */
        counters->errorStatus[0] = (prbsInvalidDataFlag << 1) & (prbsErrorFlag << 2);
    }
    else if (counters->sampleSource == ADI_ADRV9025_PRBSCHECK_LANEDATA)
    {
        /* Trigger update of lane error counters */
        recoveryAction = adrv9025_JesdCommonJrxTestLaneUpdateErrorCountBfSet(device,
                                                                             ADRV9025_BF_JESD_COMMON,
                                                                             1);
        ADI_ERROR_RETURN(device->common.error.newAction);
        for (lane = 0; lane < ADI_ADRV9025_NUM_LANES; lane++)
        {
            prbsInvalidDataFlag = 0;
            prbsErrorFlag       = 0;
            prbsLaneInv         = 0;

            adrv9025_JesdCommonJrxTestLaneErrorCountBfGet(device,
                                                          ADRV9025_BF_JESD_COMMON,
                                                          lane,
                                                          &counters->laneErrors[lane]);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_JesdCommonJrxTestLaneInvBfGet(device,
                                                   ADRV9025_BF_JESD_COMMON,
                                                   lane,
                                                   &prbsLaneInv);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_JesdCommonJrxTestLaneInvalidDataFlagBfGet(device,
                                                               ADRV9025_BF_JESD_COMMON,
                                                               lane,
                                                               &prbsInvalidDataFlag);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_JesdCommonJrxTestLaneErrorFlagBfGet(device,
                                                         ADRV9025_BF_JESD_COMMON,
                                                         lane,
                                                         &prbsErrorFlag);
            ADI_ERROR_RETURN(device->common.error.newAction);

            counters->errorStatus[lane] = prbsLaneInv | (prbsInvalidDataFlag << 1) | (prbsErrorFlag << 2);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_DfrmIlasMismatchGet(adi_adrv9025_Device_t*          device,
                                         adi_adrv9025_DeframerSel_e      deframerSelect,
                                         adi_adrv9025_DfrmCompareData_t* dfrmData)
{
    int32_t                      retVal       = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfJrxLinkChanAddr_e baseAddrRx   = ADRV9025_BF_JRX_LINK0;
    uint8_t                      readData     = 0;
    uint32_t                     mismatchMask = 0x0001;
    uint8_t                      channelId    = 0;
    uint8_t                      deframerIdx  = 0;
    uint8_t                      laneSel      = 0;
    uint8_t                      laneIdx      = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        dfrmData);

    dfrmData->zeroCheckFlag    = 0;
    dfrmData->ilasMismatchDfrm = 0;

    /* Range Check deframerSelect */
    if (deframerSelect == ADI_ADRV9025_DEFRAMER_0)
    {
        baseAddrRx  = ADRV9025_BF_JRX_LINK0;
        deframerIdx = 0;
    }
    else if (deframerSelect == ADI_ADRV9025_DEFRAMER_1)
    {
        baseAddrRx  = ADRV9025_BF_JRX_LINK1;
        deframerIdx = 1;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         JESDdata->deframerSel,
                         "Error invalid JESD Deframer Select Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (channelId = 0; channelId < ADI_ADRV9025_NUM_LANES; channelId++)
    {
        laneSel = 1 << channelId;
        if ((device->devStateInfo.jesdCfg.deframerCfg[deframerIdx].deserializerLanesEnabled & laneSel) == laneSel)
        {
            // First lane in use.
            laneIdx = channelId;
            break;
        }
    }

    mismatchMask = 0x00000001;

    /* ILAS DID */
    retVal = adrv9025_JrxLinkJrxDl204bL0Rxcfg0BfGet(device,
                                                    baseAddrRx,
                                                    laneIdx,
                                                    &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read ILAS DID");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmIlasData.dfrmDID = readData;
    dfrmData->zeroCheckFlag        = dfrmData->zeroCheckFlag | (uint32_t)readData;

    /* CFG DID */
    retVal = adrv9025_JrxLinkJrxDidCfgBfGet(device,
                                            baseAddrRx,
                                            &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read CFG DID");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmCfgData.dfrmDID = readData;

    /* compare DID */
    if (dfrmData->dfrmIlasData.dfrmDID != dfrmData->dfrmCfgData.dfrmDID)
    {
        dfrmData->ilasMismatchDfrm = dfrmData->ilasMismatchDfrm | mismatchMask;
    }
    mismatchMask = mismatchMask << 1;

    /* ILAS BID */
    retVal = adrv9025_JrxLinkJrxDl204bL0Rxcfg1BfGet(device,
                                                    baseAddrRx,
                                                    laneIdx,
                                                    &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read ILAS BID");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmIlasData.dfrmBID = (readData & 0x0F);
    dfrmData->zeroCheckFlag        = dfrmData->zeroCheckFlag | (uint32_t)readData;

    /* CFG BID */
    /* BID is not supported in TOK deframer assign it to value read */
    dfrmData->dfrmCfgData.dfrmBID = dfrmData->dfrmIlasData.dfrmBID;

    /* compare BID */
    if (dfrmData->dfrmIlasData.dfrmBID != dfrmData->dfrmCfgData.dfrmBID)
    {
        dfrmData->ilasMismatchDfrm = dfrmData->ilasMismatchDfrm | mismatchMask;
    }
    mismatchMask = mismatchMask << 1;

    /* ILAS LID0 */
    retVal = adrv9025_JrxLinkJrxDl204bL0Rxcfg2BfGet(device,
                                                    baseAddrRx,
                                                    laneIdx,
                                                    &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read ILAS LID0");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmIlasData.dfrmLID0 = (readData & 0x1F);
    dfrmData->zeroCheckFlag         = dfrmData->zeroCheckFlag | (uint32_t)readData;

    /* CFG LID0 */
    retVal = adrv9025_JrxLinkJrxLidCfgBfGet(device,
                                            baseAddrRx,
                                            laneIdx,
                                            &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read CFG LID0");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmCfgData.dfrmLID0 = readData;

    /* compare LID0 */
    if (dfrmData->dfrmIlasData.dfrmLID0 != dfrmData->dfrmCfgData.dfrmLID0)
    {
        dfrmData->ilasMismatchDfrm = dfrmData->ilasMismatchDfrm | mismatchMask;
    }
    mismatchMask = mismatchMask << 1;

    /* ILAS L */
    retVal = adrv9025_JrxLinkJrxDl204bL0Rxcfg3BfGet(device,
                                                    baseAddrRx,
                                                    laneIdx,
                                                    &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read ILAS L");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmIlasData.dfrmL = (readData & 0x1F);
    dfrmData->zeroCheckFlag      = dfrmData->zeroCheckFlag | (uint32_t)readData;

    /* CFG L */
    retVal = adrv9025_JrxLinkJrxLCfgBfGet(device,
                                          baseAddrRx,
                                          &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read CFG L");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmCfgData.dfrmL = readData;

    /* compare L */
    if (dfrmData->dfrmIlasData.dfrmL != dfrmData->dfrmCfgData.dfrmL)
    {
        dfrmData->ilasMismatchDfrm = dfrmData->ilasMismatchDfrm | mismatchMask;
    }
    mismatchMask = mismatchMask << 1;

    /* ILAS SCR */
    retVal = adrv9025_JrxLinkJrxDl204bL0Rxcfg3BfGet(device,
                                                    baseAddrRx,
                                                    laneIdx,
                                                    &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read ILAS SCR");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmIlasData.dfrmSCR = ((readData & 0x80) >> 7);
    dfrmData->zeroCheckFlag        = dfrmData->zeroCheckFlag | (uint32_t)readData;

    /* CFG SCR */
    retVal = adrv9025_JrxLinkJrxDscrCfgBfGet(device,
                                             baseAddrRx,
                                             &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read CFG SCR");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmCfgData.dfrmSCR = readData;

    /* compare SCR */
    if (dfrmData->dfrmIlasData.dfrmSCR != dfrmData->dfrmCfgData.dfrmSCR)
    {
        dfrmData->ilasMismatchDfrm = dfrmData->ilasMismatchDfrm | mismatchMask;
    }
    mismatchMask = mismatchMask << 1;

    /* ILAS F */
    retVal = adrv9025_JrxLinkJrxDl204bL0Rxcfg4BfGet(device,
                                                    baseAddrRx,
                                                    laneIdx,
                                                    &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read ILAS F");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmIlasData.dfrmF = readData;
    dfrmData->zeroCheckFlag      = dfrmData->zeroCheckFlag | (uint32_t)readData;

    /* CFG F */
    retVal = adrv9025_JrxLinkJrxFCfgBfGet(device,
                                          baseAddrRx,
                                          &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read CFG F");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmCfgData.dfrmF = readData;

    /* compare F */
    if (dfrmData->dfrmIlasData.dfrmF != dfrmData->dfrmCfgData.dfrmF)
    {
        dfrmData->ilasMismatchDfrm = dfrmData->ilasMismatchDfrm | mismatchMask;
    }
    mismatchMask = mismatchMask << 1;

    /* ILAS K */
    retVal = adrv9025_JrxLinkJrxDl204bL0Rxcfg5BfGet(device,
                                                    baseAddrRx,
                                                    laneIdx,
                                                    &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read ILAS K");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmIlasData.dfrmK = (readData & 0x1F);
    dfrmData->zeroCheckFlag      = dfrmData->zeroCheckFlag | (uint32_t)readData;

    /* CFG K */
    retVal = adrv9025_JrxLinkJrxKCfgBfGet(device,
                                          baseAddrRx,
                                          &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read CFG K");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmCfgData.dfrmK = readData;

    /* compare K */
    if (dfrmData->dfrmIlasData.dfrmK != dfrmData->dfrmCfgData.dfrmK)
    {
        dfrmData->ilasMismatchDfrm = dfrmData->ilasMismatchDfrm | mismatchMask;
    }
    mismatchMask = mismatchMask << 1;

    /* ILAS M */
    retVal = adrv9025_JrxLinkJrxDl204bL0Rxcfg6BfGet(device,
                                                    baseAddrRx,
                                                    laneIdx,
                                                    &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read ILAS M");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmIlasData.dfrmM = readData;
    dfrmData->zeroCheckFlag      = dfrmData->zeroCheckFlag | (uint32_t)readData;

    /* CFG M */
    retVal = adrv9025_JrxLinkJrxMCfgBfGet(device,
                                          baseAddrRx,
                                          &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read CFG M");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmCfgData.dfrmM = readData;

    /* compare M*/
    if (dfrmData->dfrmIlasData.dfrmM != dfrmData->dfrmCfgData.dfrmM)
    {
        dfrmData->ilasMismatchDfrm = dfrmData->ilasMismatchDfrm | mismatchMask;
    }
    mismatchMask = mismatchMask << 1;

    /* ILAS N */
    retVal = adrv9025_JrxLinkJrxDl204bL0Rxcfg7BfGet(device,
                                                    baseAddrRx,
                                                    laneIdx,
                                                    &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read ILAS N");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmIlasData.dfrmN = (readData & 0x1F);
    dfrmData->zeroCheckFlag      = dfrmData->zeroCheckFlag | (uint32_t)readData;

    /* CFG N */
    retVal = adrv9025_JrxLinkJrxNCfgBfGet(device,
                                          baseAddrRx,
                                          &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read CFG N");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmCfgData.dfrmN = readData;

    /* compare N */
    if (dfrmData->dfrmIlasData.dfrmN != dfrmData->dfrmCfgData.dfrmN)
    {
        dfrmData->ilasMismatchDfrm = dfrmData->ilasMismatchDfrm | mismatchMask;
    }
    mismatchMask = mismatchMask << 1;

    /* ILAS CS */
    retVal = adrv9025_JrxLinkJrxDl204bL0Rxcfg7BfGet(device,
                                                    baseAddrRx,
                                                    laneIdx,
                                                    &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read ILAS CS");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmIlasData.dfrmCS = ((readData & 0xC0) >> 6);
    dfrmData->zeroCheckFlag       = dfrmData->zeroCheckFlag | (uint32_t)readData;

    /* CFG CS */
    retVal = adrv9025_JrxLinkJrxCsCfgBfGet(device,
                                           baseAddrRx,
                                           &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read CFG S");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmCfgData.dfrmCS = readData;

    /* compare CS*/
    if (dfrmData->dfrmIlasData.dfrmCS != dfrmData->dfrmCfgData.dfrmCS)
    {
        dfrmData->ilasMismatchDfrm = dfrmData->ilasMismatchDfrm | mismatchMask;
    }
    mismatchMask = mismatchMask << 1;

    /* ILAS NP */
    retVal = adrv9025_JrxLinkJrxDl204bL0Rxcfg8BfGet(device,
                                                    baseAddrRx,
                                                    laneIdx,
                                                    &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read ILAS NP");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmIlasData.dfrmNP = (readData & 0x1F);
    dfrmData->zeroCheckFlag       = dfrmData->zeroCheckFlag | (uint32_t)readData;

    /* CFG NP */
    retVal = adrv9025_JrxLinkJrxNpCfgBfGet(device,
                                           baseAddrRx,
                                           &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read CFG NP");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmCfgData.dfrmNP = readData;

    /* compare NP */
    if (dfrmData->dfrmIlasData.dfrmNP != dfrmData->dfrmCfgData.dfrmNP)
    {
        dfrmData->ilasMismatchDfrm = dfrmData->ilasMismatchDfrm | mismatchMask;
    }
    mismatchMask = mismatchMask << 1;

    /* ILAS S */
    retVal = adrv9025_JrxLinkJrxDl204bL0Rxcfg9BfGet(device,
                                                    baseAddrRx,
                                                    laneIdx,
                                                    &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read ILAS S");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmIlasData.dfrmS = (readData & 0x1F);
    dfrmData->zeroCheckFlag      = dfrmData->zeroCheckFlag | (uint32_t)readData;

    /* CFG S */
    retVal = adrv9025_JrxLinkJrxSCfgBfGet(device,
                                          baseAddrRx,
                                          &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read CFG S");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmCfgData.dfrmS = readData;

    /* compare S */
    if (dfrmData->dfrmIlasData.dfrmS != dfrmData->dfrmCfgData.dfrmS)
    {
        dfrmData->ilasMismatchDfrm = dfrmData->ilasMismatchDfrm | mismatchMask;
    }
    mismatchMask = mismatchMask << 1;

    /* ILAS CF */
    retVal = adrv9025_JrxLinkJrxDl204bL0Rxcfg10BfGet(device,
                                                     baseAddrRx,
                                                     laneIdx,
                                                     &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read ILAS CF");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmIlasData.dfrmCF = (readData & 0x1F);
    dfrmData->zeroCheckFlag       = dfrmData->zeroCheckFlag | (uint32_t)readData;

    /* CFG CF */
    /* CF is not supported in TOK deframer assign it to value read */
    dfrmData->dfrmCfgData.dfrmCF = dfrmData->dfrmIlasData.dfrmCF;

    /* compare CF */
    if (dfrmData->dfrmIlasData.dfrmCF != dfrmData->dfrmCfgData.dfrmCF)
    {
        dfrmData->ilasMismatchDfrm = dfrmData->ilasMismatchDfrm | mismatchMask;
    }
    mismatchMask = mismatchMask << 1;

    /* ILAS HD */
    retVal = adrv9025_JrxLinkJrxDl204bL0Rxcfg10BfGet(device,
                                                     baseAddrRx,
                                                     laneIdx,
                                                     &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read ILAS HD");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmIlasData.dfrmHD = ((readData & 0x80) >> 7);
    dfrmData->zeroCheckFlag       = dfrmData->zeroCheckFlag | (uint32_t)readData;

    /* CFG HD */
    retVal = adrv9025_JrxLinkJrxHdCfgBfGet(device,
                                           baseAddrRx,
                                           &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read CFG HD");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmCfgData.dfrmHD = readData;

    /* compare HD */
    if (dfrmData->dfrmIlasData.dfrmHD != dfrmData->dfrmCfgData.dfrmHD)
    {
        dfrmData->ilasMismatchDfrm = dfrmData->ilasMismatchDfrm | mismatchMask;
    }
    mismatchMask = mismatchMask << 1;

    /* ILAS FCHK0 */
    retVal = adrv9025_JrxLinkJrxDl204bL0Rxcfg13BfGet(device,
                                                     baseAddrRx,
                                                     laneIdx,
                                                     &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read ILAS FCHK0");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmIlasData.dfrmFCHK0 = readData;

    /* CFG FCHK0 */
    retVal = adrv9025_JrxLinkJrxChksumCfgBfGet(device,
                                               baseAddrRx,
                                               laneIdx,
                                               &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read CFG FCHK0");
    ADI_ERROR_RETURN(device->common.error.newAction);
    dfrmData->dfrmCfgData.dfrmFCHK0 = readData;

    /* compare FCHK0 */
    if (dfrmData->dfrmIlasData.dfrmFCHK0 != dfrmData->dfrmCfgData.dfrmFCHK0)
    {
        dfrmData->ilasMismatchDfrm = dfrmData->ilasMismatchDfrm | mismatchMask;
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DfrmIrqMaskGet(adi_adrv9025_Device_t*                  device,
                                    adi_adrv9025_DeframerSel_e              deframerSelect,
                                    adi_adrv9025_DeframerIrq_Mask_Vector_t* irqMask)
{
    adrv9025_BfJrxLinkChanAddr_e baseAddr = ADRV9025_BF_JRX_LINK0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common,
                        irqMask);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Range Check deframerSelect */
    if ((deframerSelect != ADI_ADRV9025_DEFRAMER_0) && (deframerSelect != ADI_ADRV9025_DEFRAMER_1) && (deframerSelect != ADI_ADRV9025_DEFRAMER_0_AND_1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSelect,
                         "Error invalid JESD Deframer Select Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    else
    {
        /* read back the Dfrm IRQ Vector */
        if ((deframerSelect == ADI_ADRV9025_DEFRAMER_0) || (deframerSelect == ADI_ADRV9025_DEFRAMER_0_AND_1))
        {
            baseAddr = ADRV9025_BF_JRX_LINK0;
            adrv9025_JrxLinkJrxDl204bIrqClrBfGet(device,
                                                 baseAddr,
                                                 &irqMask->deframer0);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if (deframerSelect != ADI_ADRV9025_DEFRAMER_0)
        {
            baseAddr = ADRV9025_BF_JRX_LINK1;
            adrv9025_JrxLinkJrxDl204bIrqClrBfGet(device,
                                                 baseAddr,
                                                 &irqMask->deframer1);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DfrmIrqMaskSet(adi_adrv9025_Device_t*                 device,
                                    adi_adrv9025_DeframerSel_e             deframerSelect,
                                    adi_adrv9025_DeframerIrq_Mask_Vector_t irqMask)
{
    adrv9025_BfJrxLinkChanAddr_e baseAddr = ADRV9025_BF_JRX_LINK0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Range Check deframerSelect */
    if ((deframerSelect != ADI_ADRV9025_DEFRAMER_0) && (deframerSelect != ADI_ADRV9025_DEFRAMER_1) && (deframerSelect != ADI_ADRV9025_DEFRAMER_0_AND_1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSelect,
                         "Error invalid JESD Deframer Select Value");
        return (device->common.error.newAction);
    }

    else
    {
        /* read back the Dfrm IRQ Vector */
        if ((deframerSelect == ADI_ADRV9025_DEFRAMER_0) || (deframerSelect == ADI_ADRV9025_DEFRAMER_0_AND_1))
        {
            baseAddr = ADRV9025_BF_JRX_LINK0;
            adrv9025_JrxLinkJrxDl204bIrqClrBfSet(device,
                                                 baseAddr,
                                                 irqMask.deframer0);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_JrxLinkJrxDl204hIrqClrBfSet(device,
                                                 baseAddr,
                                                 irqMask.deframer0);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((deframerSelect == ADI_ADRV9025_DEFRAMER_1) || (deframerSelect == ADI_ADRV9025_DEFRAMER_0_AND_1))
        {
            baseAddr = ADRV9025_BF_JRX_LINK1;
            adrv9025_JrxLinkJrxDl204bIrqClrBfSet(device,
                                                 baseAddr,
                                                 irqMask.deframer1);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_JrxLinkJrxDl204hIrqClrBfSet(device,
                                                 baseAddr,
                                                 irqMask.deframer1);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DfrmIrqSourceReset(adi_adrv9025_Device_t*     device,
                                        adi_adrv9025_DeframerSel_e deframerSelect)
{
    adrv9025_BfJrxLinkChanAddr_e                        baseAddr     = ADRV9025_BF_JRX_LINK0;
    adi_adrv9025_DeframerIrq_Mask_Vector_t              irqMask_Save = {0, 0};
    const static adi_adrv9025_DeframerIrq_Mask_Vector_t irqMask_Clr  = {0xFFFF, 0xFFFF};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Range Check deframerSelect */
    if ((deframerSelect != ADI_ADRV9025_DEFRAMER_0) && (deframerSelect != ADI_ADRV9025_DEFRAMER_1) && (deframerSelect != ADI_ADRV9025_DEFRAMER_0_AND_1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSelect,
                         "Error invalid JESD Deframer Select Value");
        return (device->common.error.newAction);
    }

    else
    {
        /* read back the Dfrm IRQ Vector, clear interrupts, restore original mask */
        if ((deframerSelect == ADI_ADRV9025_DEFRAMER_0) || (deframerSelect == ADI_ADRV9025_DEFRAMER_0_AND_1))
        {
            baseAddr = ADRV9025_BF_JRX_LINK0;

            adrv9025_JrxLinkJrxDl204bIrqClrBfGet(device,
                                                 baseAddr,
                                                 &irqMask_Save.deframer0);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_JrxLinkJrxDl204bIrqClrBfSet(device,
                                                 baseAddr,
                                                 irqMask_Clr.deframer0);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_JrxLinkJrxDl204bIrqClrBfSet(device,
                                                 baseAddr,
                                                 irqMask_Save.deframer0);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_JrxLinkJrxDl204hIrqClrBfGet(device,
                                                 baseAddr,
                                                 &irqMask_Save.deframer0);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_JrxLinkJrxDl204hIrqClrBfSet(device,
                                                 baseAddr,
                                                 irqMask_Clr.deframer0);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_JrxLinkJrxDl204hIrqClrBfSet(device,
                                                 baseAddr,
                                                 irqMask_Save.deframer0);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((deframerSelect == ADI_ADRV9025_DEFRAMER_1) || (deframerSelect == ADI_ADRV9025_DEFRAMER_0_AND_1))
        {
            baseAddr = ADRV9025_BF_JRX_LINK1;
            adrv9025_JrxLinkJrxDl204bIrqClrBfGet(device,
                                                 baseAddr,
                                                 &irqMask_Save.deframer1);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_JrxLinkJrxDl204bIrqClrBfSet(device,
                                                 baseAddr,
                                                 irqMask_Clr.deframer1);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_JrxLinkJrxDl204bIrqClrBfSet(device,
                                                 baseAddr,
                                                 irqMask_Save.deframer1);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_JrxLinkJrxDl204hIrqClrBfGet(device,
                                                 baseAddr,
                                                 &irqMask_Save.deframer1);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_JrxLinkJrxDl204hIrqClrBfSet(device,
                                                 baseAddr,
                                                 irqMask_Clr.deframer1);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_JrxLinkJrxDl204hIrqClrBfSet(device,
                                                 baseAddr,
                                                 irqMask_Save.deframer1);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DfrmIrqSourceGet(adi_adrv9025_Device_t*                  device,
                                      adi_adrv9025_DeframerSel_e              deframerSelect,
                                      adi_adrv9025_DeframerIrq_Mask_Vector_t* irqSourceValue)
{
    uint8_t                      channelId     = 0;
    adrv9025_BfJrxLinkChanAddr_e baseAddr      = ADRV9025_BF_JRX_LINK0;
    uint8_t                      laneSel       = 0;
    uint8_t                      deframer0Done = ADI_FALSE;
    uint8_t                      deframer1Done = ADI_FALSE;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    ADI_NULL_PTR_RETURN(&device->common,
                        irqSourceValue);

    /* Range Check deframerSelect */
    if ((deframerSelect != ADI_ADRV9025_DEFRAMER_0) &&
        (deframerSelect != ADI_ADRV9025_DEFRAMER_1) &&
        (deframerSelect != ADI_ADRV9025_DEFRAMER_0_AND_1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSelect,
                         "Error invalid JESD Deframer Select Value");
        return (device->common.error.newAction);
    }
    else
    {
        /* read back the Dfrm IRQ Vector */
        for (channelId = 0; channelId < ADI_ADRV9025_NUM_LANES; channelId++)
        {
            laneSel = 1 << channelId;

            if ((deframerSelect == ADI_ADRV9025_DEFRAMER_0) || (deframerSelect == ADI_ADRV9025_DEFRAMER_0_AND_1))
            {
                // Only get first lane in used.
                if ((device->devStateInfo.jesdCfg.deframerCfg[0].deserializerLanesEnabled & laneSel) && (deframer0Done == ADI_FALSE))
                {
                    baseAddr = ADRV9025_BF_JRX_LINK0;
                    adrv9025_JrxLinkJrxDl204bIrqVecBfGet(device,
                                                         baseAddr,
                                                         channelId,
                                                         &irqSourceValue->deframer0);
                    ADI_ERROR_RETURN(device->common.error.newAction);
                    deframer0Done = ADI_TRUE;
                }
            }

            if ((deframerSelect != ADI_ADRV9025_DEFRAMER_0) || (deframerSelect == ADI_ADRV9025_DEFRAMER_0_AND_1))
            {
                // Only get first lane in used.
                if ((device->devStateInfo.jesdCfg.deframerCfg[1].deserializerLanesEnabled & laneSel) && (deframer1Done == ADI_FALSE))
                {
                    baseAddr = ADRV9025_BF_JRX_LINK1;
                    adrv9025_JrxLinkJrxDl204bIrqVecBfGet(device,
                                                         baseAddr,
                                                         channelId,
                                                         &irqSourceValue->deframer1);
                    ADI_ERROR_RETURN(device->common.error.newAction);
                    deframer1Done = ADI_TRUE;
                }
            }
        }
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_SerializerReset(adi_adrv9025_Device_t* device,
                                     uint32_t               pllClkSource)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t readData       = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if (pllClkSource > 0)
    {
        recoveryAction = adrv9025_JesdCommonJtxOverrideSrstEnBfSet(device,
                                                                   ADRV9025_BF_JESD_COMMON,
                                                                   1);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to set jtx_override_srst_en to 1");
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JesdCommonJtxOverrideSrstEnBfGet(device,
                                                                   ADRV9025_BF_JESD_COMMON,
                                                                   &readData);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to get jtx_override_srst_en");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (readData != 1)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             recoveryAction,
                             NULL,
                             "Failed to set jtx_override_srst_en to 1");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        recoveryAction = adrv9025_JesdCommonJtxOverrideSrstEnBfSet(device,
                                                                   ADRV9025_BF_JESD_COMMON,
                                                                   0);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to set jtx_override_srst_en to 0");
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JesdCommonJtxOverrideSrstEnBfGet(device,
                                                                   ADRV9025_BF_JESD_COMMON,
                                                                   &readData);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to get jtx_override_srst_en");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (readData != 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             recoveryAction,
                             NULL,
                             "Failed to set jtx_override_srst_en to 0");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    else
    {
        recoveryAction = adrv9025_JesdCommonJtxTriggerSrstNBfSet(device,
                                                                 ADRV9025_BF_JESD_COMMON,
                                                                 1);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to set jtx_trigger_srst_n to 1");
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JesdCommonJtxTriggerSrstNBfGet(device,
                                                                 ADRV9025_BF_JESD_COMMON,
                                                                 &readData);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to get jtx_trigger_srst_n");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (readData != 1)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             recoveryAction,
                             NULL,
                             "Failed to set jtx_trigger_srst_n to 1");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        recoveryAction = adrv9025_JesdCommonJtxTriggerSrstNBfSet(device,
                                                                 ADRV9025_BF_JESD_COMMON,
                                                                 0);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to set jtx_trigger_srst_n to 0");
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JesdCommonJtxTriggerSrstNBfGet(device,
                                                                 ADRV9025_BF_JESD_COMMON,
                                                                 &readData);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to get jtx_trigger_srst_n");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (readData != 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             recoveryAction,
                             NULL,
                             "Failed to set jtx_trigger_srst_n to 0");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DfrmErrCntrCntrlSet(adi_adrv9025_Device_t*              device,
                                         adi_adrv9025_DeframerSel_e          deframerSelect,
                                         adi_adrv9025_DfrmErrCounterIrqSel_e interruptEnable,
                                         uint8_t                             laneNumber,
                                         uint8_t                             errCounterControl,
                                         uint8_t                             errCounterHoldCfg)
{
    static const uint16_t IRQERRCOUNTERMASK   = 0x00E0;
    static const uint8_t  ERRCOUNTERRSTMASK   = 0X70;
    static const uint8_t  ERRCOUNTERCNTRLMASK = 0x07;
    static const uint8_t  ERRCOUNTERHOLDMASK  = 0x07;
    static const uint8_t  ERRCOUNTERRSTSHIFT  = 4;

    int32_t                                retVal             = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfJrxLinkChanAddr_e           baseAddrRx         = ADRV9025_BF_JRX_LINK0;
    adi_adrv9025_DeframerIrq_Mask_Vector_t irqMask            = {0};
    uint8_t                                errorCounterReset  = 0x00;
    uint8_t                                errorCounterConfig = 0x00;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Read current irq mask for selected deframer for read/modify/write */
    retVal = adi_adrv9025_DfrmIrqMaskGet(device,
                                         deframerSelect,
                                         &irqMask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read irq mask");
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (deframerSelect == ADI_ADRV9025_DEFRAMER_0)
    {
        baseAddrRx = ADRV9025_BF_JRX_LINK0;

        if (interruptEnable == ADI_ADRV9025_DFRM_ERR_COUNT_ENABLE_IRQ)
        {
            irqMask.deframer0 = irqMask.deframer0 & (~IRQERRCOUNTERMASK);
        }
        else if (interruptEnable == ADI_ADRV9025_DFRM_ERR_COUNT_DISABLE_IRQ)
        {
            irqMask.deframer0 = irqMask.deframer0 | IRQERRCOUNTERMASK;
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             interruptEnable,
                             "Error Invalid Interrupt selection");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    else if (deframerSelect == ADI_ADRV9025_DEFRAMER_1)
    {
        baseAddrRx = ADRV9025_BF_JRX_LINK1;

        if (interruptEnable == ADI_ADRV9025_DFRM_ERR_COUNT_ENABLE_IRQ)
        {
            irqMask.deframer1 = irqMask.deframer1 & (~IRQERRCOUNTERMASK);
        }
        else if (interruptEnable == ADI_ADRV9025_DFRM_ERR_COUNT_DISABLE_IRQ)
        {
            irqMask.deframer1 = irqMask.deframer1 | IRQERRCOUNTERMASK;
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             interruptEnable,
                             "Error Invalid Interrupt selection");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSelect,
                         "Error Invalid Deframer deframerSelect Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (laneNumber > 7)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         laneNumber,
                         "Error invalid Lane number Select Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Write modified irq mask for selected deframer */
    retVal = adi_adrv9025_DfrmIrqMaskSet(device,
                                         deframerSelect,
                                         irqMask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to write modified irq mask");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Extract 3 reset bits from input argument */
    errorCounterReset = (errCounterControl & ERRCOUNTERRSTMASK) >> ERRCOUNTERRSTSHIFT;

    /* Mask 3 config bits from input argument */
    errorCounterConfig = errCounterControl & ERRCOUNTERCNTRLMASK;
    errCounterHoldCfg  = errCounterHoldCfg & ERRCOUNTERHOLDMASK;

    /* Reset the requested error counters */
    retVal = adrv9025_JrxLinkJrxDl204bEcntRstBfSet(device,
                                                   baseAddrRx,
                                                   laneNumber,
                                                   errorCounterReset);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to reset error counter");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Clear reset bits */
    retVal = adrv9025_JrxLinkJrxDl204bEcntRstBfSet(device,
                                                   baseAddrRx,
                                                   laneNumber,
                                                   0x00);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to clear reset flag for error counters");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Set error counter threshold to 255 */
    retVal = adrv9025_JrxLinkJrxDl204bEthBfSet(device,
                                               baseAddrRx,
                                               0xFF);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to clear set error threshold");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Write counter enable bits */
    retVal = adrv9025_JrxLinkJrxDl204bEcntEnaBfSet(device,
                                                   baseAddrRx,
                                                   laneNumber,
                                                   errorCounterConfig);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to write error counter enable");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Write the error counter hold configuration */
    retVal = adrv9025_JrxLinkJrxDl204bEcntTchBfSet(device,
                                                   baseAddrRx,
                                                   laneNumber,
                                                   errCounterHoldCfg);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to write error counter hold configuration");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DfrmErrCntrCntrlGet(adi_adrv9025_Device_t*     device,
                                         adi_adrv9025_DeframerSel_e deframerSelect,
                                         uint8_t                    laneNumber,
                                         uint8_t*                   errCounterControl,
                                         uint8_t*                   errCounterHoldCfg)
{
    int32_t                      retVal     = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfJrxLinkChanAddr_e baseAddrRx = ADRV9025_BF_JRX_LINK0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common,
                        errCounterControl);

    ADI_NULL_PTR_RETURN(&device->common,
                        errCounterHoldCfg);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if (deframerSelect == ADI_ADRV9025_DEFRAMER_0)
    {
        baseAddrRx = ADRV9025_BF_JRX_LINK0;
    }
    else if (deframerSelect == ADI_ADRV9025_DEFRAMER_1)
    {
        baseAddrRx = ADRV9025_BF_JRX_LINK1;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSelect,
                         "Error Invalid Deframer deframerSelect Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (laneNumber > 7)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         laneNumber,
                         "Error invalid Lane number Select Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* read out the contents of the error counter control */
    retVal = adrv9025_JrxLinkJrxDl204bEcntEnaBfGet(device,
                                                   baseAddrRx,
                                                   laneNumber,
                                                   errCounterControl);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read error counter control");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* read out the contents of the error counter hold configuration */
    retVal = adrv9025_JrxLinkJrxDl204bEcntTchBfGet(device,
                                                   baseAddrRx,
                                                   laneNumber,
                                                   errCounterHoldCfg);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read error counter hold configuration");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DfrmErrCounterStatusGet(adi_adrv9025_Device_t*               device,
                                             adi_adrv9025_DeframerSel_e           deframerSelect,
                                             uint8_t                              laneNumber,
                                             adi_adrv9025_DfrmErrCounterStatus_t* errCounterStatus)
{
    int32_t                      retVal     = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfJrxLinkChanAddr_e baseAddrRx = ADRV9025_BF_JRX_LINK0;
    uint8_t                      readData   = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common,
                        errCounterStatus);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if (deframerSelect == ADI_ADRV9025_DEFRAMER_0)
    {
        baseAddrRx = ADRV9025_BF_JRX_LINK0;
    }
    else if (deframerSelect == ADI_ADRV9025_DEFRAMER_1)
    {
        baseAddrRx = ADRV9025_BF_JRX_LINK1;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSelect,
                         "Error invalid JESD Deframer Select Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (laneNumber > 7)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         laneNumber,
                         "Error invalid Lane number Select Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Reset lane status */
    errCounterStatus->laneStatus = 0;

    /* Read deframer status */
    retVal = adrv9025_JrxLinkJrxDl204bUekBfGet(device,
                                               baseAddrRx,
                                               laneNumber,
                                               &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read deframer status - UEK Errors");
    ADI_ERROR_RETURN(device->common.error.newAction);
    errCounterStatus->laneStatus |= readData;
    errCounterStatus->laneStatus <<= 1;

    retVal = adrv9025_JrxLinkJrxDl204bNitBfGet(device,
                                               baseAddrRx,
                                               laneNumber,
                                               &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read deframer status - NIT Errors");
    ADI_ERROR_RETURN(device->common.error.newAction);
    errCounterStatus->laneStatus |= readData;
    errCounterStatus->laneStatus <<= 1;

    retVal = adrv9025_JrxLinkJrxDl204bIlsBfGet(device,
                                               baseAddrRx,
                                               laneNumber,
                                               &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read deframer status - ILAS Status");
    ADI_ERROR_RETURN(device->common.error.newAction);
    errCounterStatus->laneStatus |= readData;
    errCounterStatus->laneStatus <<= 1;

    retVal = adrv9025_JrxLinkJrxDl204bIldBfGet(device,
                                               baseAddrRx,
                                               laneNumber,
                                               &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read deframer status - Inter-Lane De-skew Status");
    ADI_ERROR_RETURN(device->common.error.newAction);
    errCounterStatus->laneStatus |= readData;
    errCounterStatus->laneStatus <<= 1;

    retVal = adrv9025_JrxLinkJrxDl204bFsBfGet(device,
                                              baseAddrRx,
                                              laneNumber,
                                              &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read deframer status - Frame Sync Status");
    ADI_ERROR_RETURN(device->common.error.newAction);
    errCounterStatus->laneStatus |= readData;
    errCounterStatus->laneStatus <<= 1;

    retVal = adrv9025_JrxLinkJrxDl204bCksBfGet(device,
                                               baseAddrRx,
                                               laneNumber,
                                               &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read deframer status - Completed Checksum Status");
    ADI_ERROR_RETURN(device->common.error.newAction);
    errCounterStatus->laneStatus |= readData;
    errCounterStatus->laneStatus <<= 1;

    retVal = adrv9025_JrxLinkJrxDl204bCgsBfGet(device,
                                               baseAddrRx,
                                               laneNumber,
                                               &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read deframer status - CGS Status");
    ADI_ERROR_RETURN(device->common.error.newAction);
    errCounterStatus->laneStatus |= readData;
    errCounterStatus->laneStatus <<= 1;

    retVal = adrv9025_JrxLinkJrxDl204bBdeBfGet(device,
                                               baseAddrRx,
                                               laneNumber,
                                               &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read deframer status - BDE Status");
    ADI_ERROR_RETURN(device->common.error.newAction);
    errCounterStatus->laneStatus |= readData;

    /* read deframer bad disparity counter value */
    retVal = adrv9025_JrxLinkJrxDl204bBdCntBfGet(device,
                                                 baseAddrRx,
                                                 laneNumber,
                                                 &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read deframer bad disparity counter value");
    ADI_ERROR_RETURN(device->common.error.newAction);
    errCounterStatus->bdCntValue = readData;

    /* read deframer UEK counter value */
    retVal = adrv9025_JrxLinkJrxDl204bUekCntBfGet(device,
                                                  baseAddrRx,
                                                  laneNumber,
                                                  &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read deframer UEK counter value");
    ADI_ERROR_RETURN(device->common.error.newAction);
    errCounterStatus->uekCntValue = readData;

    /* read deframer NITcounter value */
    retVal = adrv9025_JrxLinkJrxDl204bNitCntBfGet(device,
                                                  baseAddrRx,
                                                  laneNumber,
                                                  &readData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read deframer NIT counter value");
    ADI_ERROR_RETURN(device->common.error.newAction);
    errCounterStatus->nitCntValue = readData;

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DfrmLinkConditionGet(adi_adrv9025_Device_t*     device,
                                          adi_adrv9025_DeframerSel_e deframerSelect,
                                          uint8_t*                   dfrmLinkCondition)
{
    int32_t                      retVal     = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfJrxLinkChanAddr_e baseAddrRx = ADRV9025_BF_JRX_LINK0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_NULL_PTR_RETURN(&device->common,
                        dfrmLinkCondition);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if (deframerSelect == ADI_ADRV9025_DEFRAMER_0)
    {
        baseAddrRx = ADRV9025_BF_JRX_LINK0;
    }
    else if (deframerSelect == ADI_ADRV9025_DEFRAMER_1)
    {
        baseAddrRx = ADRV9025_BF_JRX_LINK1;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSelect,
                         "Error invalid JESD Deframer Link Condition Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* read deframer link condition value */
    *dfrmLinkCondition = 0;
    retVal             = adrv9025_JrxLinkJrxTplUsrDataRdyBfGet(device,
                                                               baseAddrRx,
                                                               dfrmLinkCondition);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read deframer link condition");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DesEyeSweep(adi_adrv9025_Device_t*             device,
                                 uint8_t                            laneNumber,
                                 adi_adrv9025_DesEyeSweepResults_t* spoSweepResults)
{
    int32_t              retVal        = ADI_COMMON_ACT_NO_ACTION;
    static const uint8_t LF_GENERAL    = 0x0E;
    static const uint8_t CAL_DCD_0     = 0x14;
    int16_t              spo           = 0;
    uint8_t              prbsErrorFlag = 0;
    uint8_t              laneConverted = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common, spoSweepResults);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if ((laneNumber != 0x01) && (laneNumber != 0x02) && (laneNumber != 0x04) && (laneNumber != 0x08))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         framerSel,
                         "Only one lane is allowed at a time for SPO sweep");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Lane conversion from laminate swaps */
    laneConverted = laneNumber; //adrv9025_DeserializerLamLaneOffsetGet(laneNumber);

    spoSweepResults->goodSpoLeft  = 0;
    spoSweepResults->goodSpoRight = 0;
    spoSweepResults->goodSpoTotal = 0;

    /* disable ck90 skew */
    retVal = adrv9025_CbusRegisterWrite(device,
                                        LF_GENERAL,
                                        laneConverted,
                                        0x03);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     retVal,
                     NULL,
                     "Failed to write LF_GENERAL");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* freeze content of slice adjust loop */
    retVal = adrv9025_CbusRegisterWrite(device,
                                        CAL_DCD_0,
                                        laneConverted,
                                        0x01);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     retVal,
                     NULL,
                     "Failed to write CAL_DCD_0");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Remember, SPO uses two's complement & is 7 bits; step down SPO to -31 */
    for (spo = 127; spo > 96; spo--)
    {
        retVal = adrv9025_SpoSet(device,
                                 laneConverted,
                                 (uint8_t)(spo & 0x7F));
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         retVal,
                         NULL,
                         "Failed to write SPO");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Sweep SPO -31 to -1 */
    for (spo = 97; spo < 128; spo++)
    {
        retVal = adrv9025_SpoSet(device,
                                 laneConverted,
                                 (uint8_t)(spo & 0x7F));
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         retVal,
                         NULL,
                         "Failed to write SPO");
        ADI_ERROR_RETURN(device->common.error.newAction);

        retVal = adi_adrv9025_DfrmPrbsCountReset(device);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         retVal,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        retVal = adrv9025_hal_Wait_us(&device->common,
                                        500);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         retVal,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        retVal = adrv9025_JesdCommonJrxTestLaneErrorFlagBfGet(device,
                                                              ADRV9025_BF_JESD_COMMON,
                                                              laneConverted,
                                                              &prbsErrorFlag);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         retVal,
                         NULL,
                         "Failed to read deframer link condition");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (prbsErrorFlag == 0)
        {
            spoSweepResults->goodSpoLeft++;
        }
    }

    /* Sweep SPO 0 to 31 */
    for (spo = 0x0; spo < 32; spo++)
    {
        retVal = adrv9025_SpoSet(device,
                                 laneConverted,
                                 (uint8_t)(spo & 0x7F));
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         retVal,
                         NULL,
                         "Failed to write SPO");
        ADI_ERROR_RETURN(device->common.error.newAction);

        retVal = adi_adrv9025_DfrmPrbsCountReset(device);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         retVal,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        retVal = adrv9025_hal_Wait_us(&device->common,
                                        500);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         retVal,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        retVal = adrv9025_JesdCommonJrxTestLaneErrorFlagBfGet(device,
                                                              ADRV9025_BF_JESD_COMMON,
                                                              laneConverted,
                                                              &prbsErrorFlag);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         retVal,
                         NULL,
                         "Failed to read deframer link condition");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (prbsErrorFlag == 0)
        {
            spoSweepResults->goodSpoRight++;
        }
    }

    /* Return SPO to zero offset */
    for (spo = 31; spo > -1; spo--)
    {
        retVal = adrv9025_SpoSet(device,
                                 laneConverted,
                                 (uint8_t)(spo & 0x7F));
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         retVal,
                         NULL,
                         "Failed to write SPO");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* enable ck0/ck90 skew correction */
    retVal = adrv9025_CbusRegisterWrite(device,
                                        LF_GENERAL,
                                        laneConverted,
                                        0x01);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     retVal,
                     NULL,
                     "Failed to write LF_GENERAL");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* enable slice adjust loop */
    retVal = adrv9025_CbusRegisterWrite(device,
                                        CAL_DCD_0,
                                        laneConverted,
                                        0x00);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     retVal,
                     NULL,
                     "Failed to write CAL_DCD_0");
    ADI_ERROR_RETURN(device->common.error.newAction);

    spoSweepResults->goodSpoTotal = spoSweepResults->goodSpoRight + spoSweepResults->goodSpoLeft;

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DfrmFifoDepthGet(adi_adrv9025_Device_t     *device,
                                      adi_adrv9025_DeframerSel_e deframerSelect,
                                      uint8_t                    channelId,
                                      uint8_t                   *fifoDepth)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    adrv9025_BfJrxLinkChanAddr_e baseAddr = ADRV9025_BF_JRX_LINK0;

    /* NULL pointer check */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common, fifoDepth);

    /* Write to log file */
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    if (deframerSelect == ADI_ADRV9025_DEFRAMER_0)
    {
        baseAddr = ADRV9025_BF_JRX_LINK0;
    }
    else if (deframerSelect == ADI_ADRV9025_DEFRAMER_1)
    {
        baseAddr = ADRV9025_BF_JRX_LINK1;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSelect,
                         "Error invalid JESD Deframer Select Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_JrxLinkJrxTplBufDepthBfGet(device, baseAddr, channelId, fifoDepth);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to return the elastic FIFO depth");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DfrmBufferProtectionStatusGet(adi_adrv9025_Device_t     *device,
                                                   adi_adrv9025_DeframerSel_e deframerSelect,
                                                   uint8_t                   *bufferProtectionStatus)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    adrv9025_BfJrxLinkChanAddr_e baseAddr = ADRV9025_BF_JRX_LINK0;

    /* NULL pointer check */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common, bufferProtectionStatus);

    /* Write to log file */
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    if (deframerSelect == ADI_ADRV9025_DEFRAMER_0)
    {
        baseAddr = ADRV9025_BF_JRX_LINK0;
    }
    else if (deframerSelect == ADI_ADRV9025_DEFRAMER_1)
    {
        baseAddr = ADRV9025_BF_JRX_LINK1;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSelect,
                         "Error invalid JESD Deframer Select Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_JrxLinkJrxTplBufProtectionBfGet(device, baseAddr, bufferProtectionStatus);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to read Buffer Protection status");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DfrmBufferProtectionSet(adi_adrv9025_Device_t     *device,
                                             uint8_t                    bufferProtectionSet,
                                             adi_adrv9025_DeframerSel_e deframerSelect)
{
    uint8_t  i              = 0;
    uint32_t dfrmSelect     = deframerSelect;
    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    adrv9025_BfJrxLinkChanAddr_e baseAddr = ADRV9025_BF_JRX_LINK0;

    /* NULL pointer check */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    /* Write to log file */
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    if ((deframerSelect != ADI_ADRV9025_DEFRAMER_0) &&
        (deframerSelect != ADI_ADRV9025_DEFRAMER_1) &&
        (deframerSelect != ADI_ADRV9025_DEFRAMER_0_AND_1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSelect,
                         "Error invalid JESD Deframer Select Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < deframerSelect; i++)
    {
        if ((dfrmSelect & ADI_ADRV9025_DEFRAMER_0) == ADI_ADRV9025_DEFRAMER_0)
        {
            baseAddr    = ADRV9025_BF_JRX_LINK0;
            dfrmSelect -= ADI_ADRV9025_DEFRAMER_0;
        }
        else if ((dfrmSelect & ADI_ADRV9025_DEFRAMER_1) == ADI_ADRV9025_DEFRAMER_1)
        {
            baseAddr    = ADRV9025_BF_JRX_LINK1;
            dfrmSelect -= ADI_ADRV9025_DEFRAMER_1;
        }
        else
        {
            break;
        }

        recoveryAction = adrv9025_JrxLinkJrxTplBufProtectEnBfSet(device, baseAddr, bufferProtectionSet);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Failed to disable/enable the underflow protection for the buffer");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DfrmBufferProtectionGet(adi_adrv9025_Device_t     *device,
                                             adi_adrv9025_DeframerSel_e deframerSelect,
                                             uint8_t                   *bufferProtectionGet)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    adrv9025_BfJrxLinkChanAddr_e baseAddr = ADRV9025_BF_JRX_LINK0;

    /* NULL pointer check */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common, bufferProtectionGet);

    /* Write to log file */
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    if (deframerSelect == ADI_ADRV9025_DEFRAMER_0)
    {
        baseAddr = ADRV9025_BF_JRX_LINK0;
    }
    else if (deframerSelect == ADI_ADRV9025_DEFRAMER_1)
    {
        baseAddr = ADRV9025_BF_JRX_LINK1;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSelect,
                         "Error invalid JESD Deframer Select Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_JrxLinkJrxTplBufProtectEnBfGet(device, baseAddr, bufferProtectionGet);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Failed to return current state of the buffer protection");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DfrmLmfcOffsetSet(adi_adrv9025_Device_t     *device,
                                       adi_adrv9025_DeframerSel_e deframerSelect,
                                       uint16_t                   lmfcOffset)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfJrxLinkChanAddr_e baseAddrRx = ADRV9025_BF_JRX_LINK0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    /* Range Check deframerSelect */
    if ((deframerSelect != ADI_ADRV9025_DEFRAMER_0) && (deframerSelect != ADI_ADRV9025_DEFRAMER_1) && (deframerSelect != ADI_ADRV9025_DEFRAMER_0_AND_1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSelect,
                         "Error invalid JESD Deframer Select Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((deframerSelect == ADI_ADRV9025_DEFRAMER_0) || (deframerSelect == ADI_ADRV9025_DEFRAMER_0_AND_1))
    {
        baseAddrRx = ADRV9025_BF_JRX_LINK0;
        recoveryAction = adrv9025_JrxLinkJrxTplPhaseAdjustBfSet(device, baseAddrRx, lmfcOffset);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to set LMFC Offset for Deframer 0");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((deframerSelect == ADI_ADRV9025_DEFRAMER_1) || (deframerSelect == ADI_ADRV9025_DEFRAMER_0_AND_1))
    {
        baseAddrRx = ADRV9025_BF_JRX_LINK1;
        recoveryAction = adrv9025_JrxLinkJrxTplPhaseAdjustBfSet(device, baseAddrRx, lmfcOffset);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to set LMFC Offset for Deframer 1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DfrmLmfcOffsetGet(adi_adrv9025_Device_t     *device,
                                       adi_adrv9025_DeframerSel_e deframerSelect,
                                       uint16_t                  *lmfcOffset)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfJrxLinkChanAddr_e baseAddrRx = ADRV9025_BF_JRX_LINK0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common, lmfcOffset);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, lmfcOffset);

    /* Range Check deframerSelect */
    if ((deframerSelect != ADI_ADRV9025_DEFRAMER_0) && (deframerSelect != ADI_ADRV9025_DEFRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSelect,
                         "Error invalid JESD Deframer Select Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (deframerSelect == ADI_ADRV9025_DEFRAMER_0)
    {
        baseAddrRx = ADRV9025_BF_JRX_LINK0;
        recoveryAction = adrv9025_JrxLinkJrxTplPhaseAdjustBfGet(device, baseAddrRx, lmfcOffset);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to set LMFC Offset for Deframer 0");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (deframerSelect == ADI_ADRV9025_DEFRAMER_1)
    {
        baseAddrRx = ADRV9025_BF_JRX_LINK1;
        recoveryAction = adrv9025_JrxLinkJrxTplPhaseAdjustBfGet(device, baseAddrRx, lmfcOffset);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to set LMFC Offset for Deframer 1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}


int32_t adi_adrv9025_DfrmPhaseDiffGet(adi_adrv9025_Device_t     *device,
                                      adi_adrv9025_DeframerSel_e deframerSelect,
                                      uint8_t                    channelId,
                                      uint8_t                   *phaseDiff)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    adrv9025_BfJrxLinkChanAddr_e baseAddr = ADRV9025_BF_JRX_LINK0;

    /* NULL pointer check */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common, phaseDiff);

    /* Write to log file */
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    if (deframerSelect == ADI_ADRV9025_DEFRAMER_0)
    {
        baseAddr = ADRV9025_BF_JRX_LINK0;
    }
    else if (deframerSelect == ADI_ADRV9025_DEFRAMER_1)
    {
        baseAddr = ADRV9025_BF_JRX_LINK1;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         deframerSelect,
                         "Error invalid JESD Deframer Select Value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_JrxLinkJrxTplPhaseDiffBfGet(device, baseAddr, channelId, phaseDiff);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to return the elastic FIFO depth");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_GenerateEyeDiagramQRMode(adi_adrv9025_Device_t              *device,
                                              uint8_t                            lane,
                                              adi_adrv9025_EyeDiagramResultsQR_t **results)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    static const uint8_t numPhasesPerUI = ADRV9025_QR_EYE_NUM_PHASES;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_DEVICE_PTR_RETURN(results);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    /* calculate positive eye i.e. direction 1 */
    recoveryAction = adrv9025_GenerateEyeDiagram_Direction(device, lane, results, numPhasesPerUI, 1);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to read calculate positive eye");
    ADI_ERROR_RETURN(device->common.error.newAction);
     /* calculate negative eye i.e. direction 0 */
    recoveryAction = adrv9025_GenerateEyeDiagram_Direction(device, lane, results, numPhasesPerUI, 0);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to read calculate negative eye");
    ADI_ERROR_RETURN(device->common.error.newAction);


    return recoveryAction;
}
