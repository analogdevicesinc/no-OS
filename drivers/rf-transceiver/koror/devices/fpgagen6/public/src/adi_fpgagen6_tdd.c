/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_tdd.c
* \brief Contains top level tdd fpga related functions
*
* FPGA API Version: 2.9.0.4
*/

#include "adi_fpgagen6_tdd.h"
#include "adi_fpgagen6_hal.h"
#include "adi_fpgagen6_gpio.h"


#include "../../private/include/fpgagen6_tdd.h"
#include "../../private/bf/fpgagen6_bf_adrv904x_tdd.h"
#include "../../private/include/fpgagen6_data_interface.h"

#define ADI_FILE    ADI_FPGAGEN6_FILE_PUBLIC_TDD

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddFsmCfgSet( adi_fpgagen6_Device_t* const                    device,
                                                            const adi_fpgagen6_TddStateMachineCfg_t* const  tddFsmCfg)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     channelIdx      = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, tddFsmCfg, cleanup);

    /* Frame Counter Module */

    recoveryAction = fpgagen6_Adrvgen6Tdd_FrameCnt_BfSet(   device,
                                                            FPGAGEN6_BF_ADS10_TOP_TDD,
                                                            tddFsmCfg->frameCounter.endCount);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_Adrvgen6Tdd_LoopCnt_BfSet(device,
                                                        FPGAGEN6_BF_ADS10_TOP_TDD,
                                                        tddFsmCfg->frameCounter.loopCount);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_Adrvgen6Tdd_LoopCntEn_BfSet(  device,
                                                            FPGAGEN6_BF_ADS10_TOP_TDD,
                                                            tddFsmCfg->frameCounter.loopCountEnable);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_Adrvgen6Tdd_ExtTriggerEnable_BfSet(   device,
                                                                    FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                    tddFsmCfg->frameCounter.extTriggerEnable);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* TRX Enable Module */

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_TRX; ++channelIdx)
    {
        recoveryAction = fpgagen6_TddEnableControlCfgSet(   device,
                                                            FPGAGEN6_TDD_ENABLEMODULE_TRX,
                                                            channelIdx,
                                                            &tddFsmCfg->trx[channelIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* ORX Enable Module */

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_ORX; ++channelIdx)
    {
        recoveryAction = fpgagen6_TddEnableControlCfgSet(   device,
                                                            FPGAGEN6_TDD_ENABLEMODULE_ORX,
                                                            channelIdx,
                                                            &tddFsmCfg->orx[channelIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* GPIO Enable Module */

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_GPIO; ++channelIdx)
    {
        recoveryAction = fpgagen6_TddEnableControlCfgSet(   device,
                                                            FPGAGEN6_TDD_ENABLEMODULE_GPIO,
                                                            channelIdx,
                                                            &tddFsmCfg->gpio[channelIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* EXT Enable Module */

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_EXT; ++channelIdx)
    {
        recoveryAction = fpgagen6_TddEnableControlCfgSet(   device,
                                                            FPGAGEN6_TDD_ENABLEMODULE_EXT,
                                                            channelIdx,
                                                            &tddFsmCfg->ext[channelIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* TX DataPath Module */

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_NUM_TDD_DATAPATH_TX; ++channelIdx)
    {
        recoveryAction = fpgagen6_Adrvgen6Tdd_TxdpStartValue_BfSet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                    channelIdx,
                                                                    tddFsmCfg->tx[channelIdx].start);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* RX DataPath Module */

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_NUM_TDD_DATAPATH_RX; ++channelIdx)
    {
        recoveryAction = fpgagen6_Adrvgen6Tdd_RxdpStartValue_BfSet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                    channelIdx,
                                                                    tddFsmCfg->rx[channelIdx].start);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }


    {
        adi_fpgagen6_TddGpioSyncSelEn_t gpioSyncCfg;

        ADI_LIBRARY_MEMCPY(&gpioSyncCfg, &tddFsmCfg->gpioSyncSelEnable, sizeof(adi_fpgagen6_TddGpioSyncSelEn_t));

        /* TDD GPIO Sync Select Configuring */
        recoveryAction = adi_fpgagen6_TddGpioSyncCfgSet(device, &gpioSyncCfg);
        if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_GPIO_SYNC_SELECT,
                                                    &gpioSyncCfg.gpioSyncEnable,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddFsmCfgGet( adi_fpgagen6_Device_t* const                device,
                                                            adi_fpgagen6_TddStateMachineCfg_t* const    tddFsmCfg)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     channelIdx      = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, tddFsmCfg, cleanup);

    ADI_LIBRARY_MEMSET(tddFsmCfg, 0, sizeof(adi_fpgagen6_TddStateMachineCfg_t));
    
    /* Frame Counter Module */

    recoveryAction = fpgagen6_Adrvgen6Tdd_FrameCnt_BfGet(  device,
                                                        FPGAGEN6_BF_ADS10_TOP_TDD,
                                                        &tddFsmCfg->frameCounter.endCount);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_Adrvgen6Tdd_LoopCnt_BfGet(   device,
                                                        FPGAGEN6_BF_ADS10_TOP_TDD,
                                                        &tddFsmCfg->frameCounter.loopCount);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_Adrvgen6Tdd_LoopCntEn_BfGet( device,
                                                        FPGAGEN6_BF_ADS10_TOP_TDD,
                                                        &tddFsmCfg->frameCounter.loopCountEnable);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_Adrvgen6Tdd_ExtTriggerEnable_BfGet(  device,
                                                                FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                &tddFsmCfg->frameCounter.extTriggerEnable);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* TRX Enable Module */

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_TRX; ++channelIdx)
    {
        recoveryAction = fpgagen6_TddEnableControlCfgGet(   device,
                                                            FPGAGEN6_TDD_ENABLEMODULE_TRX,
                                                            channelIdx,
                                                            &tddFsmCfg->trx[channelIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* ORX Enable Module */

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_ORX; ++channelIdx)
    {
        recoveryAction = fpgagen6_TddEnableControlCfgGet(   device,
                                                            FPGAGEN6_TDD_ENABLEMODULE_ORX,
                                                            channelIdx,
                                                            &tddFsmCfg->orx[channelIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* GPIO Enable Module */

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_GPIO; ++channelIdx)
    {
        recoveryAction = fpgagen6_TddEnableControlCfgGet(   device,
                                                            FPGAGEN6_TDD_ENABLEMODULE_GPIO,
                                                            channelIdx,
                                                            &tddFsmCfg->gpio[channelIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* EXT Enable Module */

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_EXT; ++channelIdx)
    {
        recoveryAction = fpgagen6_TddEnableControlCfgGet(   device,
                                                            FPGAGEN6_TDD_ENABLEMODULE_EXT,
                                                            channelIdx,
                                                            &tddFsmCfg->ext[channelIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* TX DataPath Module */

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_NUM_TDD_DATAPATH_TX; ++channelIdx)
    {
        recoveryAction = fpgagen6_Adrvgen6Tdd_TxdpStartValue_BfGet(device,
                                                                FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                channelIdx,
                                                                &tddFsmCfg->tx[channelIdx].start);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* RX DataPath Module */

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_NUM_TDD_DATAPATH_RX; ++channelIdx)
    {
        recoveryAction = fpgagen6_Adrvgen6Tdd_RxdpStartValue_BfGet(device,
                                                                FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                channelIdx,
                                                                &tddFsmCfg->rx[channelIdx].start);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddFsmEnable(adi_fpgagen6_Device_t* const device)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    recoveryAction = fpgagen6_Adrvgen6Tdd_TddEnable_BfSet( device,
                                                        FPGAGEN6_BF_ADS10_TOP_TDD,
                                                        ADI_ENABLE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                ADI_FPGAGEN6_ERRCODE_TDD_STATE_MACHINE_ENABLE,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddFsmDisable(adi_fpgagen6_Device_t* const device)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     channelIdx      = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    /* Disable TDD Frame Looping */
    recoveryAction = fpgagen6_Adrvgen6Tdd_LoopCntEn_BfSet( device,
                                                        FPGAGEN6_BF_ADS10_TOP_TDD,
                                                        ADI_DISABLE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                ADI_FPGAGEN6_ERRCODE_TDD_FRAME_LOOP,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* Disable TDD State Machine */
    recoveryAction = fpgagen6_Adrvgen6Tdd_TddEnable_BfSet( device,
                                                        FPGAGEN6_BF_ADS10_TOP_TDD,
                                                        ADI_DISABLE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                ADI_FPGAGEN6_ERRCODE_TDD_STATE_MACHINE_DISABLE,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* Frame Counter Reset */
    recoveryAction = fpgagen6_Adrvgen6Tdd_FrameCnt_BfSet(  device,
                                                        FPGAGEN6_BF_ADS10_TOP_TDD,
                                                        0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                ADI_FPGAGEN6_ERRCODE_TDD_FRAME_COUNT,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* Loop Counter Reset */
    recoveryAction = fpgagen6_Adrvgen6Tdd_LoopCnt_BfSet(   device,
                                                        FPGAGEN6_BF_ADS10_TOP_TDD,
                                                        0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                ADI_FPGAGEN6_ERRCODE_TDD_FRAME_LOOP,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* Disable Enable Controls */
    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_TRX; ++channelIdx)
    {
        recoveryAction = fpgagen6_Adrvgen6Tdd_TrxRegionEnables_BfSet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                    channelIdx,
                                                                    0U);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_ENABLE_CTRL_DISABLE,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_ORX; ++channelIdx)
    {
        recoveryAction = fpgagen6_Adrvgen6Tdd_OrxRegionEnables_BfSet(  device,
                                                                  FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                  channelIdx,
                                                                  0U);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_ENABLE_CTRL_DISABLE,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_GPIO; ++channelIdx)
    {
        recoveryAction = fpgagen6_Adrvgen6Tdd_GpioRegionEnables_BfSet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                    channelIdx,
                                                                    0U);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_ENABLE_CTRL_DISABLE,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_EXT; ++channelIdx)
    {
        recoveryAction = fpgagen6_Adrvgen6Tdd_ExtRegionEnables_BfSet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                    channelIdx,
                                                                    0U);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_ENABLE_CTRL_DISABLE,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddManualEnableSet(   adi_fpgagen6_Device_t* const                        device,
                                                                    const adi_fpgagen6_TddManualEnableMasks_t* const    manualEnableMaskInfo)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     idx             = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, manualEnableMaskInfo, cleanup);

    /* TRX & GPIO Enable Mask Doesnt Require Checking */

    if ((manualEnableMaskInfo->orxMask >= (1 << ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_ORX)) ||
        (manualEnableMaskInfo->extMask >= (1 << ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_EXT)))
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                manualEnableMaskInfo,
                                "Invalid Mask Selection");
        goto cleanup;
    }

    /* TRX Manual Enable Configuration */
    for (idx = 0U; idx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_TRX; ++idx)
    {
        recoveryAction = fpgagen6_Adrvgen6Tdd_TrxEnableManual_BfSet(   device,
                                                                    FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                    idx,
                                                                    (uint8_t) ((manualEnableMaskInfo->trxMask >> idx) & 1U));
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_TRX_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* ORX Manual Enable Configuration */
    for (idx = 0U; idx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_ORX; ++idx)
    {
        recoveryAction = fpgagen6_Adrvgen6Tdd_OrxEnableManual_BfSet(   device,
                                                                 FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                 idx,
                                                                 (uint8_t) ((manualEnableMaskInfo->orxMask >> idx) & 1U));
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_ORX_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* GPIO Manual Enable Configuration */
    for (idx = 0U; idx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_GPIO; ++idx)
    {
        recoveryAction = fpgagen6_Adrvgen6Tdd_GpioEnableManual_BfSet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                    idx,
                                                                    (uint8_t) ((manualEnableMaskInfo->gpioMask >> idx) & 1U));
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_GPIO_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* External Manual Enable Configuration */
    for (idx = 0U; idx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_EXT; ++idx)
    {
        recoveryAction = fpgagen6_Adrvgen6Tdd_ExtEnableManual_BfSet(   device,
                                                                    FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                    idx,
                                                                    (uint8_t) ((manualEnableMaskInfo->extMask >> idx) & 1U));
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_EXT_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddManualEnableGet(   adi_fpgagen6_Device_t* const                device,
                                                                    adi_fpgagen6_TddManualEnableMasks_t* const  manualEnableMaskInfo)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     idx             = 0U;
    uint8_t                     bfValue         = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, manualEnableMaskInfo, cleanup);

    /* Clear Mask Set */
    manualEnableMaskInfo->trxMask   = 0U;
    manualEnableMaskInfo->orxMask   = 0U;
    manualEnableMaskInfo->gpioMask  = 0U;
    manualEnableMaskInfo->extMask   = 0U;

    /* TRX Manual Enable Configuration Fetch */
    for (idx = 0U; idx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_TRX; ++idx)
    {
        recoveryAction = fpgagen6_Adrvgen6Tdd_TrxEnableManual_BfGet(   device,
                                                                    FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                    idx,
                                                                    &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_TRX_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }

        manualEnableMaskInfo->trxMask |= (uint8_t) ((bfValue & 1U) << idx);
    }

    /* ORX Manual Enable Configuration Fetch */
    for (idx = 0U; idx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_ORX; ++idx)
    {
        recoveryAction = fpgagen6_Adrvgen6Tdd_OrxEnableManual_BfGet(   device,
                                                                    FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                    idx,
                                                                    &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_ORX_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }

        manualEnableMaskInfo->orxMask |= (uint8_t) ((bfValue & 1U) << idx);
    }

    /* ORX Manual Enable Configuration Fetch */
    for (idx = 0U; idx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_GPIO; ++idx)
    {
        recoveryAction = fpgagen6_Adrvgen6Tdd_GpioEnableManual_BfGet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                    idx,
                                                                    &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_GPIO_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }

        manualEnableMaskInfo->gpioMask |= (uint8_t) ((bfValue & 1U) << idx);
    }

    /* External Manual Enable Configuration Fetch */
    for (idx = 0U; idx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_EXT; ++idx)
    {
        recoveryAction = fpgagen6_Adrvgen6Tdd_ExtEnableManual_BfGet(   device,
                                                                    FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                    idx,
                                                                    &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_EXT_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
        manualEnableMaskInfo->extMask |= (uint8_t) ((bfValue & 1U) << idx);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddGpioSyncDriftGet(adi_fpgagen6_Device_t* const  device,
                                                                  uint32_t * const               gpioSyncDrift)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, gpioSyncDrift, cleanup);
 

    recoveryAction = fpgagen6_Adrvgen6Tdd_GpioSyncDrift_BfGet(device,
                                                              FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_TDD,
                                                              gpioSyncDrift);
    
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                ADI_FPGAGEN6_ERRCODE_TDD_GPIO_SYNC_SELECT,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddGpioSyncCfgSet(adi_fpgagen6_Device_t* const            device,
                                                                adi_fpgagen6_TddGpioSyncSelEn_t* const  gpioSyncCfg)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);


    {
        if (gpioSyncCfg->gpioSyncSel > ADI_FPGAGEN6_GPIO_INVALID)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    gpioSyncCfg->gpioSyncSel,
                                    "Invalid GPIO Value");
            goto cleanup;
        }

        /* Use Invalid Abstraction Value to Skip GPIO Input Direction Configuration */
        if (gpioSyncCfg->gpioSyncSel != ADI_FPGAGEN6_GPIO_INVALID)
        {
            uint32_t pinSelMask = 1U << gpioSyncCfg->gpioSyncSel;

            if (gpioSyncCfg->gpioSyncEnable == ADI_FPGAGEN6_TDD_GPIO_SYNC_DISABLE)
            {
                if (device->devStateInfo.gpioPinMode[gpioSyncCfg->gpioSyncSel] == ADI_FPGAGEN6_GPIO_MODE_SEL_TRIGGER)
                {
                    /* Mark the GPIO pin as unused */
                    device->devStateInfo.gpioPinMode[gpioSyncCfg->gpioSyncSel] = ADI_FPGAGEN6_GPIO_MODE_NONE;
                }
            }

            /* Set GPIO pin to input direction. It also checking for GPIO pin availability */
            recoveryAction = adi_fpgagen6_GpioInputDirSet(device, pinSelMask);
            if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
            {
                recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        pinSelMask,
                                        "Failed to set GPIO Input direction");
                goto cleanup;
            }

            /* Set GPIO Pin only when enable */
            if (gpioSyncCfg->gpioSyncEnable == ADI_FPGAGEN6_TDD_GPIO_SYNC_ENABLE)
            {
                /* TDD GPIO Sync Select Setting */
                recoveryAction = fpgagen6_TddGpioSyncSelectSet( device,
                                                                (adi_fpgagen6_GpioPinSel_e) gpioSyncCfg->gpioSyncSel);
                if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                            ADI_FPGAGEN6_ERRCODE_TDD_GPIO_SYNC_SELECT,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }

                /* Mark the GPIO pin as used */
                device->devStateInfo.gpioPinMode[gpioSyncCfg->gpioSyncSel] = ADI_FPGAGEN6_GPIO_MODE_SEL_TRIGGER;
            }
        }

        /* Enable/Disable TDD GPIO Sync */
        recoveryAction = fpgagen6_TddGpioSyncEnableSet(device, gpioSyncCfg->gpioSyncEnable);
        if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                    ADI_FPGAGEN6_ERRCODE_TDD_GPIO_SYNC,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TddGpioSyncCfgGet(adi_fpgagen6_Device_t* const  device,
                                                                adi_fpgagen6_TddGpioSyncSelEn_t * const  gpioSyncCfg)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);


    
    /* Get TDD GPIO Sync Select Setting */
    recoveryAction = fpgagen6_TddGpioSyncSelectGet(device, 
                                                  (adi_fpgagen6_GpioPinSel_e *) &gpioSyncCfg->gpioSyncSel);
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                ADI_FPGAGEN6_ERRCODE_TDD_GPIO_SYNC_SELECT,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    recoveryAction = fpgagen6_TddGpioSyncEnableGet(device, &gpioSyncCfg->gpioSyncEnable);
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TDD,
                                                ADI_FPGAGEN6_ERRCODE_TDD_GPIO_SYNC,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}