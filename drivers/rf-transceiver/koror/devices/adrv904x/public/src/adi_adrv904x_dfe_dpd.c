/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_adrv904x_dfe_dpd.c
* \brief Contains DPD features related function implementations
*
* ADRV904X API Version: 2.15.0.4
*/
#include "adi_common_hal.h"
#include "adi_library_types.h"
#include "adi_adrv904x_dfe_dpd.h"
#include "adi_adrv904x_core.h"
#include "adi_adrv904x_tx.h"
#include "adi_adrv904x_hal.h"
#include "adi_adrv904x_dfe_cpu.h"
#include "adi_adrv904x_dfe_cal_dpd_types.h"
#include "adi_adrv904x_dfe_cal_dpd_int_types.h"
#include "adi_adrv904x_dfe_framework_tracking_cal_t.h"
#include "../../private/include/adrv904x_platform_byte_order.h"
#include "../../private/include/adrv904x_dfe_cpu.h"
#include "../../private/include/adrv904x_dfe_dpd.h"
#include "../../private/bf/adrv904x_bf_cap_buf_mmr.h"

#define ADI_FILE    ADI_ADRV904X_FILE_PUBLIC_DFE_DPD

#define DPD_CTRL_SET_CMD_FLAG (0x10)

#ifndef ADI_LIBRARY_RM_FLOATS
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdModelConfigDpdSet(adi_adrv904x_Device_t* const                      device,
                                                                   const uint32_t                                    dpdTxChannelMask,
                                                                   const adi_adrv904x_DfeAppCalDpdModelType_e        dpdModelType,
                                                                   const adi_adrv904x_DfeAppCalDpdModelDesc_t* const modelDesc)
{
        const uint16_t DPD_CTRL_MODEL_CONFIG_SET = 0 | DPD_CTRL_SET_CMD_FLAG;
    ADI_PLATFORM_LARGE_ARRAY_ALLOC(uint8_t, cpuCtrlData, sizeof(adi_adrv904x_DfeAppCalDpdModelType_t) + sizeof(adi_adrv904x_DfeAppCalDpdModelDesc_t));
    adi_adrv904x_DfeAppCalDpdModelDesc_t *pTmpModelDesc =
                 (adi_adrv904x_DfeAppCalDpdModelDesc_t *)(cpuCtrlData + sizeof(adi_adrv904x_DfeAppCalDpdModelType_t));

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuCtrlData, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, modelDesc, cleanup);

    recoveryAction = adrv904x_DpdModelConfigDpdSetRangeCheck(device, dpdTxChannelMask, dpdModelType, (const adi_adrv904x_DfeAppCalDpdModelDescInt_t* const)modelDesc);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DpdModelConfigDpdSet range check failed ");
        goto cleanup;
    }

    ADI_LIBRARY_MEMCPY((void*)cpuCtrlData, (const void *)&dpdModelType, sizeof(adi_adrv904x_DfeAppCalDpdModelType_t));
    ADI_LIBRARY_MEMCPY((void*)(cpuCtrlData + sizeof(adi_adrv904x_DfeAppCalDpdModelType_t)), (const void * const)modelDesc, sizeof(adi_adrv904x_DfeAppCalDpdModelDesc_t));

    /* Note that all the integer fields of *pTmpModelDesc and it's sub-strucutures are uint8_t so require no endianess
     * corrections. */

    for (i = 0U; i < pTmpModelDesc->features; ++i)
    {
        pTmpModelDesc->feature[i].a.real = ADRV904X_HTOCD(pTmpModelDesc->feature[i].a.real);
        pTmpModelDesc->feature[i].a.imag = ADRV904X_HTOCD(pTmpModelDesc->feature[i].a.imag);
    }
    pTmpModelDesc->features = ADRV904X_HTOCS(pTmpModelDesc->features);

    for (i = 0U; i < ADI_ADRV904X_MAX_TXCHANNELS; ++i)
    {
        uint32_t txChan = (1 << i);
        if (dpdTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               DPD_CTRL_MODEL_CONFIG_SET,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               cpuCtrlData,
                                                               sizeof(adi_adrv904x_DfeAppCalDpdModelType_t) + sizeof(adi_adrv904x_DfeAppCalDpdModelDesc_t),
                                                               &lengthResp,
                                                               NULL,
                                                               0);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChan, "Failed to send request");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdModelConfigDpdGet(adi_adrv904x_Device_t* const                device,
                                                                   const adi_adrv904x_TxChannels_e             dpdTxChannelSel,
                                                                   const adi_adrv904x_DfeAppCalDpdModelType_e  dpdModelType,
                                                                   adi_adrv904x_DfeAppCalDpdModelDesc_t* const modelDesc)
{
        const uint16_t DPD_CTRL_MODEL_CONFIG_GET = 0;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, modelDesc, cleanup);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)dpdTxChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dpdTxChannelSel, "channel parameter is invalid.");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                       ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                       DPD_CTRL_MODEL_CONFIG_GET,
                                                       (adi_adrv904x_Channels_e)dpdTxChannelSel,
                                                       (const uint8_t*)&dpdModelType,
                                                       sizeof(adi_adrv904x_DfeAppCalDpdModelType_t),
                                                       &lengthResp,
                                                       (uint8_t *)modelDesc,
                                                       sizeof(adi_adrv904x_DfeAppCalDpdModelDesc_t));

    if (recoveryAction == ADI_ADRV904X_ERR_ACT_NONE)
    {
        if (modelDesc->features == 0U)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, modelDesc->features, "The number of features is greater than maximum allowed");
            goto cleanup;
        }
    }

    if (recoveryAction == ADI_ADRV904X_ERR_ACT_NONE)
    {
        modelDesc->features = ADRV904X_CTOHS(modelDesc->features);

        for (i = 0U; i < modelDesc->features; ++i)
        {
            modelDesc->feature[i].a.real = ADRV904X_HTOCD(modelDesc->feature[i].a.real);
            modelDesc->feature[i].a.imag = ADRV904X_HTOCD(modelDesc->feature[i].a.imag);
        }
        
        /* mem set the unused features*/
        int memsetSize = sizeof(adi_adrv904x_DfeAppCalDpdAdpFeatureRow_t) * (ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_FEATURES - modelDesc->features);
        ADI_LIBRARY_MEMSET(&(modelDesc->feature[modelDesc->features]), 0, memsetSize);
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
#endif /* ADI_LIBRARY_RM_FLOATS */

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdModelConfigDpdSet_int(adi_adrv904x_Device_t* const                         device,
                                                                       const uint32_t                                       dpdTxChannelMask,
                                                                       const adi_adrv904x_DfeAppCalDpdModelType_e           dpdModelType,
                                                                       const adi_adrv904x_DfeAppCalDpdModelDescInt_t* const modelDesc)
{
    const uint16_t DPD_CTRL_MODEL_CONFIG_SET = 0 | DPD_CTRL_SET_CMD_FLAG;
    ADI_PLATFORM_LARGE_ARRAY_ALLOC(uint8_t, cpuCtrlData, sizeof(adi_adrv904x_DfeAppCalDpdModelType_t) + sizeof(adi_adrv904x_DfeAppCalDpdModelDescInt_t));
    adi_adrv904x_DfeAppCalDpdModelDescInt_t *pTmpModelDesc =
                 (adi_adrv904x_DfeAppCalDpdModelDescInt_t *)(cpuCtrlData + sizeof(adi_adrv904x_DfeAppCalDpdModelType_t));

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuCtrlData, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, modelDesc, cleanup);

    recoveryAction = adrv904x_DpdModelConfigDpdSetRangeCheck(device, dpdTxChannelMask, dpdModelType, modelDesc);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DpdModelConfigDpdSet range check failed ");
        goto cleanup;
    }

    ADI_LIBRARY_MEMCPY((void*)cpuCtrlData, (const void *)&dpdModelType, sizeof(adi_adrv904x_DfeAppCalDpdModelType_t));
    ADI_LIBRARY_MEMCPY((void*)(cpuCtrlData + sizeof(adi_adrv904x_DfeAppCalDpdModelType_t)), (const void * const)modelDesc, sizeof(adi_adrv904x_DfeAppCalDpdModelDescInt_t));

    /* Note that all the integer fields of *pTmpModelDesc and it's sub-strucutures are uint8_t so require no endianess
     * corrections. */

    for (i = 0U; i < pTmpModelDesc->features; ++i)
    {
        adi_library_scaledIntToFp64(pTmpModelDesc->feature[i].a.real_e6, 1000000, (uint64_t *)&pTmpModelDesc->feature[i].a.real_e6);
        pTmpModelDesc->feature[i].a.real_e6 = ADRV904X_HTOCLL(pTmpModelDesc->feature[i].a.real_e6);
        adi_library_scaledIntToFp64(pTmpModelDesc->feature[i].a.imag_e6, 1000000, (uint64_t *)&pTmpModelDesc->feature[i].a.imag_e6);
        pTmpModelDesc->feature[i].a.imag_e6 = ADRV904X_HTOCLL(pTmpModelDesc->feature[i].a.imag_e6);
    }
    pTmpModelDesc->features = ADRV904X_HTOCS(pTmpModelDesc->features);

    for (i = 0U; i < ADI_ADRV904X_MAX_TXCHANNELS; ++i)
    {
        uint32_t txChan = (1 << i);
        if (dpdTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               DPD_CTRL_MODEL_CONFIG_SET,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               cpuCtrlData,
                                                               sizeof(adi_adrv904x_DfeAppCalDpdModelType_t) + sizeof(adi_adrv904x_DfeAppCalDpdModelDescInt_t),
                                                               &lengthResp,
                                                               NULL,
                                                               0);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChan, "Failed to send request");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdModelConfigDpdGet_int(adi_adrv904x_Device_t* const                   device,
                                                                       const adi_adrv904x_TxChannels_e                dpdTxChannelSel,
                                                                       const adi_adrv904x_DfeAppCalDpdModelType_e     dpdModelType,
                                                                       adi_adrv904x_DfeAppCalDpdModelDescInt_t* const modelDesc)
{
    const uint16_t DPD_CTRL_MODEL_CONFIG_GET = 0;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, modelDesc, cleanup);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)dpdTxChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dpdTxChannelSel, "channel parameter is invalid.");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                       ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                       DPD_CTRL_MODEL_CONFIG_GET,
                                                       (adi_adrv904x_Channels_e)dpdTxChannelSel,
                                                       (const uint8_t*)&dpdModelType,
                                                       sizeof(adi_adrv904x_DfeAppCalDpdModelType_t),
                                                       &lengthResp,
                                                       (uint8_t *)modelDesc,
                                                       sizeof(adi_adrv904x_DfeAppCalDpdModelDescInt_t));

    if (recoveryAction == ADI_ADRV904X_ERR_ACT_NONE)
    {
        if (modelDesc->features == 0U)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, modelDesc->features, "The number of features is greater than maximum allowed");
            goto cleanup;
        }
    }

    if (recoveryAction == ADI_ADRV904X_ERR_ACT_NONE)
    {
        modelDesc->features = ADRV904X_CTOHS(modelDesc->features);

        for (i = 0U; i < modelDesc->features; ++i)
        {
            adi_library_fp64ToScaledInt(modelDesc->feature[i].a.real_e6, 1000000, &modelDesc->feature[i].a.real_e6);
            modelDesc->feature[i].a.real_e6 = ADRV904X_HTOCLL(modelDesc->feature[i].a.real_e6);
            adi_library_fp64ToScaledInt(modelDesc->feature[i].a.imag_e6, 1000000, &modelDesc->feature[i].a.imag_e6);
            modelDesc->feature[i].a.imag_e6 = ADRV904X_HTOCLL(modelDesc->feature[i].a.imag_e6);
        }
        
        /* mem set the unused features*/
        int memsetSize = sizeof(adi_adrv904x_DfeAppCalDpdAdpFeatureRowInt_t) * (ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_FEATURES - modelDesc->features);
        ADI_LIBRARY_MEMSET(&(modelDesc->feature[modelDesc->features]), 0, memsetSize);
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdReset(adi_adrv904x_Device_t* const            device,
                                                       const uint32_t                          txChannelMask,
                                                       const adi_adrv904x_DfeAppDpdResetMode_e dpdResetMode)
{
        const uint16_t DPD_CTRL_DPD_RESET = 4U; /* DPD_CTRL_RESET */
    uint16_t dpdResetCmd = 0U;
    uint32_t lengthResp = 0U;
    uint32_t chIdx = 0U;
    uint32_t chSelect = 0U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Validate txChannel: only one channel allowed */
    if ((txChannelMask == ADI_ADRV904X_TXOFF)    ||    /* No channel selected */
        ((txChannelMask & ~ADI_ADRV904X_TXALL) != 0))  /* Unsupported channel selected */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelMask, "Invalid TX channel selected.");
        goto cleanup;
    }

    /* Validate dpdResetMode */
    switch (dpdResetMode)
    {
        case ADI_ADRV904X_DFE_APP_DPD_RESET_FULL:
            dpdResetCmd = DPD_CTRL_DPD_RESET;
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dpdResetMode, "Invalid DPD Reset Mode selected.");
            goto cleanup;
    }

    for (chIdx = 0U; chIdx < ADI_ADRV904X_MAX_TXCHANNELS; chIdx++)
    {
        chSelect = 1 << chIdx;
        if ((chSelect & txChannelMask) == 0)
        {
            continue; /* Skip unselected TX channel */
        }

        recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                           ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                           dpdResetCmd,
                                                           (adi_adrv904x_Channels_e)chSelect,
                                                           NULL,
                                                           0,
                                                           &lengthResp,
                                                           NULL,
                                                           0);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chSelect, "Failed to send request for DPD Reset");
            goto cleanup;
        }
    }


    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Ctc2EnableSet(adi_adrv904x_Device_t* const                       device,
	const uint32_t                                     dpdTxChannelMask,
	const uint8_t enableCtc2)
{
		 	const uint16_t 	DPD_CTRL_CTC2_ENABLE_SET = 12 | DPD_CTRL_SET_CMD_FLAG;
	uint8_t tmpEnableCtc2;
	adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
	uint32_t lengthResp = 0;
	uint32_t i = 0;

	ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
	ADI_ADRV904X_API_ENTRY(&device->common);
	if (((dpdTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (dpdTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
	{
		recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
		ADI_PARAM_ERROR_REPORT(&device->common,
			recoveryAction,
			dpdTxChannelMask,
			"Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
		return recoveryAction;
	}
	tmpEnableCtc2 = enableCtc2;
	
	for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
	{
		uint32_t txChan = (1 << i);
		if (dpdTxChannelMask & txChan)
		{
			recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
				ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
				DPD_CTRL_CTC2_ENABLE_SET,
				(adi_adrv904x_Channels_e)txChan,
				(const uint8_t*)&tmpEnableCtc2,
				sizeof(tmpEnableCtc2),
				&lengthResp,
				NULL,
				0);
			if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
			{
				ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChan, "Failed to send request");
				goto cleanup;
			}
		}
	}

	cleanup:
	ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

#ifndef ADI_LIBRARY_RM_FLOATS
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Ctc2EnableGet(adi_adrv904x_Device_t* const                 device,
	const adi_adrv904x_TxChannels_e              dpdTxChannelSel,
	uint32_t* const ctc2Enable)
{
		 	const uint16_t DPD_CTRL_CAPTURE_CDPD_CTRL_CTC2_ENABLE_GET = 12;
	adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
	uint32_t lengthResp = 0;

	ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
	ADI_ADRV904X_API_ENTRY(&device->common);
	ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, ctc2Enable, cleanup);

	/*Check that if requested Tx Channel valid*/
	/* Channel must contain a single channel number (0-7) */
	recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)dpdTxChannelSel);
	if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
	{
		ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dpdTxChannelSel, "channel parameter is invalid.");
		goto cleanup;
	}

	recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
		ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
		DPD_CTRL_CAPTURE_CDPD_CTRL_CTC2_ENABLE_GET,
		(adi_adrv904x_Channels_e)dpdTxChannelSel,
		NULL,
		0,
		&lengthResp,
		(uint8_t*)ctc2Enable,
		sizeof(*ctc2Enable));

	

	cleanup:
	ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
#endif /* ADI_LIBRARY_RM_FLOATS */

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdActuatorGainMonitorConfigSet(adi_adrv904x_Device_t* const                          device,
                                                                              const uint32_t                                        dpdTxChannelMask,
                                                                              const adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t* const dpdPowerMeterCfg)
{
        const uint16_t DPD_CTRL_PWR_MON_CONFIG_SET = 2 | DPD_CTRL_SET_CMD_FLAG;
    adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t tmpDpdPowerMeterCfg;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdPowerMeterCfg, cleanup);

    ADI_LIBRARY_MEMCPY((void *)&tmpDpdPowerMeterCfg, dpdPowerMeterCfg, sizeof(adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t));

    recoveryAction = adrv904x_DpdActuatorGainMonitorConfigSetRangeCheck(device, dpdTxChannelMask, dpdPowerMeterCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DPD Actuator Gain monitor range check failed.");
        goto cleanup;
    }

    tmpDpdPowerMeterCfg.dpdInOutQualLim = ADRV904X_HTOCL(tmpDpdPowerMeterCfg.dpdInOutQualLim);
    tmpDpdPowerMeterCfg.dpdInOutMinGainLim = ADRV904X_HTOCL(tmpDpdPowerMeterCfg.dpdInOutMinGainLim);
    tmpDpdPowerMeterCfg.dpdInOutMaxGainLim = ADRV904X_HTOCL(tmpDpdPowerMeterCfg.dpdInOutMaxGainLim);
    tmpDpdPowerMeterCfg.dpdInOutPwrMeasContDlyCntr = ADRV904X_HTOCS(tmpDpdPowerMeterCfg.dpdInOutPwrMeasContDlyCntr);

    for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        uint32_t txChan = (1 << i);
        if (dpdTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               DPD_CTRL_PWR_MON_CONFIG_SET,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               (const uint8_t*)&tmpDpdPowerMeterCfg,
                                                               sizeof(tmpDpdPowerMeterCfg),
                                                               &lengthResp,
                                                               NULL,
                                                               0);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChan, "Failed to send request");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdActuatorGainMonitorConfigGet(adi_adrv904x_Device_t* const                    device,
                                                                              const adi_adrv904x_TxChannels_e                 dpdTxChannelSel,
                                                                              adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t* const dpdPowerMeterCfg)
{
        const uint16_t DPD_CTRL_PWR_MON_CONFIG_GET = 2;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdPowerMeterCfg, cleanup);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)dpdTxChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dpdTxChannelSel, "channel parameter is invalid.");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                       ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                       DPD_CTRL_PWR_MON_CONFIG_GET,
                                                       (adi_adrv904x_Channels_e)dpdTxChannelSel,
                                                       NULL,
                                                       0,
                                                       &lengthResp,
                                                       (uint8_t*)dpdPowerMeterCfg,
                                                       sizeof(*dpdPowerMeterCfg));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        dpdPowerMeterCfg->dpdInOutQualLim = ADRV904X_CTOHL(dpdPowerMeterCfg->dpdInOutQualLim);
        dpdPowerMeterCfg->dpdInOutMinGainLim = ADRV904X_CTOHL(dpdPowerMeterCfg->dpdInOutMinGainLim);
        dpdPowerMeterCfg->dpdInOutMaxGainLim = ADRV904X_CTOHL(dpdPowerMeterCfg->dpdInOutMaxGainLim);
        dpdPowerMeterCfg->dpdInOutPwrMeasContDlyCntr = ADRV904X_CTOHS(dpdPowerMeterCfg->dpdInOutPwrMeasContDlyCntr);
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdCaptureConfigSet(adi_adrv904x_Device_t* const                       device,
                                                                  const uint32_t                                     dpdTxChannelMask,
                                                                  const adi_adrv904x_DfeAppCalDpdCaptureCfg_t* const dpdCaptureCfg)
{
        const uint16_t DPD_CTRL_CAPTURE_CONFIG_SET = 3 | DPD_CTRL_SET_CMD_FLAG;
    adi_adrv904x_DfeAppCalDpdCaptureCfg_t tmpDpdCaptureCfg;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdCaptureCfg, cleanup);

    recoveryAction = adrv904x_DpdCaptureConfigSetRangeCheck(device, dpdTxChannelMask, dpdCaptureCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DPD Actuator Gain monitor range check failed.");
        goto cleanup;
    }

    ADI_LIBRARY_MEMCPY((void *)&tmpDpdCaptureCfg, dpdCaptureCfg, sizeof(adi_adrv904x_DfeAppCalDpdCaptureCfg_t));

    tmpDpdCaptureCfg.capPeakDetCfg.windowLength = ADRV904X_HTOCL(tmpDpdCaptureCfg.capPeakDetCfg.windowLength);
	
	for (i = 0; i < DPD_NUM_CAPTURE_BUFFER_MAX; i++)
	{
		tmpDpdCaptureCfg.ctc2FrameDelay[i] = ADRV904X_HTOCL(tmpDpdCaptureCfg.ctc2FrameDelay[i]);
	}

    for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        uint32_t txChan = (1 << i);
        if (dpdTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               DPD_CTRL_CAPTURE_CONFIG_SET,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               (const uint8_t*)&tmpDpdCaptureCfg,
                                                               sizeof(tmpDpdCaptureCfg),
                                                               &lengthResp,
                                                               NULL,
                                                               0);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChan, "Failed to send request");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdCaptureConfigGet(adi_adrv904x_Device_t* const                 device,
                                                                  const adi_adrv904x_TxChannels_e              dpdTxChannelSel,
                                                                  adi_adrv904x_DfeAppCalDpdCaptureCfg_t* const dpdCaptureCfg)
{
        const uint16_t DPD_CTRL_CAPTURE_CONFIG_GET = 3;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdCaptureCfg, cleanup);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)dpdTxChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dpdTxChannelSel, "channel parameter is invalid.");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                       ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                       DPD_CTRL_CAPTURE_CONFIG_GET,
                                                       (adi_adrv904x_Channels_e)dpdTxChannelSel,
                                                       NULL,
                                                       0,
                                                       &lengthResp,
                                                       (uint8_t*)dpdCaptureCfg,
                                                       sizeof(*dpdCaptureCfg));

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        dpdCaptureCfg->capPeakDetCfg.windowLength = ADRV904X_CTOHL(dpdCaptureCfg->capPeakDetCfg.windowLength);
	    for (uint8_t i = 0; i < DPD_NUM_CAPTURE_BUFFER_MAX; i++)
	    {
		    dpdCaptureCfg->ctc2FrameDelay[i] = ADRV904X_CTOHL(dpdCaptureCfg->ctc2FrameDelay[i]);
	    }

    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_WBBufSegConfigSet( adi_adrv904x_Device_t* const                 device,
	                                                             const adi_adrv904x_DfeAppCalDpdWbRegBufSeg_e wbBufNum)
{
		adi_adrv904x_ErrAction_e            recoveryAction                                      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
	const uint16_t                  offset = 0;
	uint8_t                         configDataSet[4] = { wbBufNum, 0x00, 0x00, 0x00 };
	/* Check device pointer is not null */
	ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
	ADI_ADRV904X_API_ENTRY(&device->common);

	/* Send command and receive response */
	recoveryAction = adi_adrv904x_DfeAppConfigSet(device,
		                                          ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
		                                          offset,
		                                          configDataSet,
		                                          sizeof(configDataSet));
	if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
	{
		goto cleanup;
	}

cleanup :
	ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_WBBufSegConfigGet( adi_adrv904x_Device_t* const    device,
	                                                             adi_adrv904x_DfeAppCalDpdWbRegBufSeg_e* wbBufNum)
{
		adi_adrv904x_ErrAction_e            recoveryAction                                      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
	const uint16_t                  offset = 0;
	uint8_t                         configDataGetPvt[4] = { 0x00, 0x00, 0x00, 0x00 };
	/* Check device pointer is not null */
	ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
	ADI_ADRV904X_API_ENTRY(&device->common);
	
	/* Send command and receive response */
	recoveryAction = adi_adrv904x_DfeAppConfigGet(device,
		                                          ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
		                                          offset,
		                                          configDataGetPvt,
		                                          sizeof(configDataGetPvt));
	if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
	{
		goto cleanup;
	}
	*wbBufNum = (adi_adrv904x_DfeAppCalDpdWbRegBufSeg_e)configDataGetPvt[0] ;

cleanup :
	ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


static adi_adrv904x_ErrAction_e adrv904x_DpdTrackingConfigSet(adi_adrv904x_Device_t* const                  device,
                                                              const uint32_t                                dpdTxChannelMask,
                                                              adi_adrv904x_DfeAppCalDpdTrackCfgInt_t* const dpdTrackCfg)
{
    const uint16_t DPD_CTRL_TRACK_CONFIG_SET = 1 | DPD_CTRL_SET_CMD_FLAG;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, dpdTrackCfg);

    dpdTrackCfg->adapt.numDpdSamples = ADRV904X_HTOCS(dpdTrackCfg->adapt.numDpdSamples);
    for (i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_COEFFICIENTS; i++)
    {
        dpdTrackCfg->filterCoef[i] = ADRV904X_HTOCS(dpdTrackCfg->filterCoef[i]);
    }
    dpdTrackCfg->mThresholdDB                 = ADRV904X_HTOCL(dpdTrackCfg->mThresholdDB);
    dpdTrackCfg->cThresholdDB                 = ADRV904X_HTOCL(dpdTrackCfg->cThresholdDB);
    dpdTrackCfg->thresholdOverlapDB           = ADRV904X_HTOCS(dpdTrackCfg->thresholdOverlapDB);
    dpdTrackCfg->numberOfMultiFrames          = ADRV904X_HTOCL(dpdTrackCfg->numberOfMultiFrames);
    dpdTrackCfg->wbRegAlpha_e6                = ADRV904X_HTOCL(dpdTrackCfg->wbRegAlpha_e6);
    dpdTrackCfg->wbRegBeta_e6                 = ADRV904X_HTOCL(dpdTrackCfg->wbRegBeta_e6);
    dpdTrackCfg->estSizeOfCoefBias            = ADRV904X_HTOCS(dpdTrackCfg->estSizeOfCoefBias);
    dpdTrackCfg->ctc2EstSizeOfCoefBias        = ADRV904X_HTOCS(dpdTrackCfg->ctc2EstSizeOfCoefBias);
    dpdTrackCfg->ctc1StatsSampleNum           = ADRV904X_HTOCL(dpdTrackCfg->ctc1StatsSampleNum);
    dpdTrackCfg->bwDetCfg.corrLagMax          = ADRV904X_HTOCS(dpdTrackCfg->bwDetCfg.corrLagMax);
    dpdTrackCfg->bwDetCfg.loBwBeta2Thres_e6   = ADRV904X_HTOCL(dpdTrackCfg->bwDetCfg.loBwBeta2Thres_e6);
    dpdTrackCfg->bwDetCfg.hiBwBeta2Thres_e6   = ADRV904X_HTOCL(dpdTrackCfg->bwDetCfg.hiBwBeta2Thres_e6);
    dpdTrackCfg->bwDetCfg.alphaTrack_e6       = ADRV904X_HTOCL(dpdTrackCfg->bwDetCfg.alphaTrack_e6);
    dpdTrackCfg->muPwrBasedCfg.pwrRangeHighDB = ADRV904X_HTOCL(dpdTrackCfg->muPwrBasedCfg.pwrRangeHighDB);
    dpdTrackCfg->muPwrBasedCfg.pwrRangeLowDB  = ADRV904X_HTOCL(dpdTrackCfg->muPwrBasedCfg.pwrRangeLowDB);

    for (i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS; i++)
    {
        dpdTrackCfg->capScreenCfg.parThreshDB[i] = ADRV904X_HTOCS(dpdTrackCfg->capScreenCfg.parThreshDB[i]);
    }
    dpdTrackCfg->txPfirFracDelAdjustThr = ADRV904X_HTOCL(dpdTrackCfg->txPfirFracDelAdjustThr);
    dpdTrackCfg->ctc2EnvPwrRatioThrDB   = ADRV904X_HTOCL(dpdTrackCfg->ctc2EnvPwrRatioThrDB);

    for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        uint32_t txChan = (1 << i);
        if (dpdTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               DPD_CTRL_TRACK_CONFIG_SET,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               (const uint8_t*)dpdTrackCfg,
                                                               sizeof(*dpdTrackCfg),
                                                               &lengthResp,
                                                               NULL,
                                                               0);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChan, "Failed to send request");
                return recoveryAction;
            }
        }
    }
    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_DpdTrackingConfigGet(adi_adrv904x_Device_t* const            device,
                                                              const adi_adrv904x_TxChannels_e         dpdTxChannelSel,
                                                              adi_adrv904x_DfeAppCalDpdTrackCfgInt_t* const dpdTrackCfg)
{
    const uint16_t DPD_CTRL_TRACK_CONFIG_GET = 1;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, dpdTrackCfg);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)dpdTxChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dpdTxChannelSel, "channel parameter is invalid.");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                       ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                       DPD_CTRL_TRACK_CONFIG_GET,
                                                       (adi_adrv904x_Channels_e)dpdTxChannelSel,
                                                       NULL,
                                                       0,
                                                       &lengthResp,
                                                       (uint8_t*)dpdTrackCfg,
                                                       sizeof(*dpdTrackCfg));

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        dpdTrackCfg->adapt.numDpdSamples = ADRV904X_CTOHS(dpdTrackCfg->adapt.numDpdSamples);

        for (i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_COEFFICIENTS; i++)
        {
            dpdTrackCfg->filterCoef[i] = ADRV904X_CTOHS(dpdTrackCfg->filterCoef[i]);
        }

        dpdTrackCfg->mThresholdDB                 = ADRV904X_CTOHL(dpdTrackCfg->mThresholdDB);
        dpdTrackCfg->cThresholdDB                 = ADRV904X_CTOHL(dpdTrackCfg->cThresholdDB);
        dpdTrackCfg->numberOfMultiFrames          = ADRV904X_CTOHL(dpdTrackCfg->numberOfMultiFrames);
        dpdTrackCfg->estSizeOfCoefBias            = ADRV904X_CTOHS(dpdTrackCfg->estSizeOfCoefBias);
        dpdTrackCfg->ctc2EstSizeOfCoefBias        = ADRV904X_CTOHS(dpdTrackCfg->ctc2EstSizeOfCoefBias);
        dpdTrackCfg->thresholdOverlapDB           = ADRV904X_CTOHS(dpdTrackCfg->thresholdOverlapDB);
        dpdTrackCfg->ctc1StatsSampleNum           = ADRV904X_CTOHL(dpdTrackCfg->ctc1StatsSampleNum);
        dpdTrackCfg->bwDetCfg.corrLagMax          = ADRV904X_CTOHS(dpdTrackCfg->bwDetCfg.corrLagMax);
	    dpdTrackCfg->bwDetCfg.loBwBeta2Thres_e6   = ADRV904X_CTOHL(dpdTrackCfg->bwDetCfg.loBwBeta2Thres_e6);
	    dpdTrackCfg->bwDetCfg.hiBwBeta2Thres_e6   = ADRV904X_CTOHL(dpdTrackCfg->bwDetCfg.hiBwBeta2Thres_e6);
	    dpdTrackCfg->bwDetCfg.alphaTrack_e6       = ADRV904X_CTOHL(dpdTrackCfg->bwDetCfg.alphaTrack_e6);
	    dpdTrackCfg->wbRegAlpha_e6                = ADRV904X_CTOHL(dpdTrackCfg->wbRegAlpha_e6);
	    dpdTrackCfg->wbRegBeta_e6                 = ADRV904X_CTOHL(dpdTrackCfg->wbRegBeta_e6);
        dpdTrackCfg->muPwrBasedCfg.pwrRangeHighDB = ADRV904X_CTOHL(dpdTrackCfg->muPwrBasedCfg.pwrRangeHighDB);
        dpdTrackCfg->muPwrBasedCfg.pwrRangeLowDB  = ADRV904X_CTOHL(dpdTrackCfg->muPwrBasedCfg.pwrRangeLowDB);
        for (i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS; i++)
        {
            dpdTrackCfg->capScreenCfg.parThreshDB[i] = ADRV904X_CTOHS(dpdTrackCfg->capScreenCfg.parThreshDB[i]);
        }
        dpdTrackCfg->txPfirFracDelAdjustThr   = ADRV904X_CTOHL(dpdTrackCfg->txPfirFracDelAdjustThr);
        dpdTrackCfg->ctc2EnvPwrRatioThrDB     = ADRV904X_CTOHL(dpdTrackCfg->ctc2EnvPwrRatioThrDB);
    }

    return recoveryAction;
}


#ifndef ADI_LIBRARY_RM_FLOATS
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdTrackingConfigSet(adi_adrv904x_Device_t* const                     device,
                                                                   const uint32_t                                   dpdTxChannelMask,
                                                                   const adi_adrv904x_DfeAppCalDpdTrackCfg_t* const dpdTrackCfg)
{
        adi_adrv904x_DfeAppCalDpdTrackCfg_t tmpDpdTrackCfg;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdTrackCfg, cleanup);

    recoveryAction = adrv904x_DpdTrackingConfigSetRangeCheckFloat(device, dpdTxChannelMask, dpdTrackCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "dpdTrackCfg range check failed.");
        goto cleanup;
    }

    ADI_LIBRARY_MEMCPY((void *)&tmpDpdTrackCfg, dpdTrackCfg, sizeof(adi_adrv904x_DfeAppCalDpdTrackCfg_t));

    recoveryAction = adrv904x_DpdTrackingConfigSet(device,
                                                   dpdTxChannelMask,
                                                   (adi_adrv904x_DfeAppCalDpdTrackCfgInt_t*)&tmpDpdTrackCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "dpdTrackingCfgSet failed");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdTrackingConfigGet(adi_adrv904x_Device_t* const               device,
                                                                   const adi_adrv904x_TxChannels_e            dpdTxChannelSel,
                                                                   adi_adrv904x_DfeAppCalDpdTrackCfg_t* const dpdTrackCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdTrackCfg, cleanup);

    recoveryAction = adrv904x_DpdTrackingConfigGet(device, dpdTxChannelSel, (adi_adrv904x_DfeAppCalDpdTrackCfgInt_t*)dpdTrackCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dpdTxChannelSel, "DpdTrackingConfigGet failed");
        goto cleanup;
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
#endif /* ADI_LIBRARY_RM_FLOATS */

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdTrackingConfigSet_int(adi_adrv904x_Device_t* const                       device,
                                                                       const uint32_t                                     dpdTxChannelMask,
                                                                       const adi_adrv904x_DfeAppCalDpdTrackCfgInt_t* const dpdTrackCfg)
{
    adi_adrv904x_DfeAppCalDpdTrackCfgInt_t tmpDpdTrackCfg;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdTrackCfg, cleanup);

    recoveryAction = adrv904x_DpdTrackingConfigSetRangeCheckInt(device, dpdTxChannelMask, dpdTrackCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "dpdTrackCfg range check failed.");
        goto cleanup;
    }

    ADI_LIBRARY_MEMCPY((void *)&tmpDpdTrackCfg, dpdTrackCfg, sizeof(adi_adrv904x_DfeAppCalDpdTrackCfgInt_t));

    adi_library_scaledIntToFp32(tmpDpdTrackCfg.wbRegAlpha_e6, 1000000, (uint32_t *)&tmpDpdTrackCfg.wbRegAlpha_e6);
    adi_library_scaledIntToFp32(tmpDpdTrackCfg.wbRegBeta_e6, 1000000, (uint32_t *)&tmpDpdTrackCfg.wbRegBeta_e6);
    adi_library_scaledIntToFp32(tmpDpdTrackCfg.bwDetCfg.loBwBeta2Thres_e6, 1000000, (uint32_t *)&tmpDpdTrackCfg.bwDetCfg.loBwBeta2Thres_e6);
    adi_library_scaledIntToFp32(tmpDpdTrackCfg.bwDetCfg.hiBwBeta2Thres_e6, 1000000, (uint32_t *)&tmpDpdTrackCfg.bwDetCfg.hiBwBeta2Thres_e6);
    adi_library_scaledIntToFp32(tmpDpdTrackCfg.bwDetCfg.alphaTrack_e6, 1000000, (uint32_t *)&tmpDpdTrackCfg.bwDetCfg.alphaTrack_e6);

    recoveryAction = adrv904x_DpdTrackingConfigSet(device, dpdTxChannelMask, &tmpDpdTrackCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "dpdTrackingCfgSet failed");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdTrackingConfigGet_int(adi_adrv904x_Device_t* const                 device,
                                                                      const adi_adrv904x_TxChannels_e               dpdTxChannelSel,
                                                                      adi_adrv904x_DfeAppCalDpdTrackCfgInt_t* const dpdTrackCfg)
{
	adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdTrackCfg, cleanup);

    recoveryAction = adrv904x_DpdTrackingConfigGet(device, dpdTxChannelSel, dpdTrackCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dpdTxChannelSel, "DpdTrackingConfigGet failed");
        goto cleanup;
    }

    adi_library_fp32ToScaledInt(dpdTrackCfg->wbRegAlpha_e6             , 1000000, &dpdTrackCfg->wbRegAlpha_e6);
    adi_library_fp32ToScaledInt(dpdTrackCfg->wbRegBeta_e6              , 1000000, &dpdTrackCfg->wbRegBeta_e6);
    adi_library_fp32ToScaledInt(dpdTrackCfg->bwDetCfg.loBwBeta2Thres_e6, 1000000, &dpdTrackCfg->bwDetCfg.loBwBeta2Thres_e6);
    adi_library_fp32ToScaledInt(dpdTrackCfg->bwDetCfg.hiBwBeta2Thres_e6, 1000000, &dpdTrackCfg->bwDetCfg.hiBwBeta2Thres_e6);
    adi_library_fp32ToScaledInt(dpdTrackCfg->bwDetCfg.alphaTrack_e6    , 1000000, &dpdTrackCfg->bwDetCfg.alphaTrack_e6);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdPowerMeterConfigSet(adi_adrv904x_Device_t* const                           device,
                                                                     const uint32_t                                         dpdTxChannelMask,
                                                                     const adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t* const  dpdPowerMeterCfg)
{
        const uint16_t DPD_POWER_METER_CONFIG_SET = 2 | DPD_CTRL_SET_CMD_FLAG;
    adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t tmpDpdPowerMeterCfg;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdPowerMeterCfg, cleanup);

    ADI_LIBRARY_MEMCPY((void *)&tmpDpdPowerMeterCfg, dpdPowerMeterCfg, sizeof(adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t));

    if (((dpdTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (dpdTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        uint32_t txChan = (1 << i);
        if (dpdTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec( device,
                                                                ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                                DPD_POWER_METER_CONFIG_SET,
                                                                (adi_adrv904x_Channels_e)txChan,
                                                                (const uint8_t*)&tmpDpdPowerMeterCfg,
                                                                sizeof(tmpDpdPowerMeterCfg),
                                                                &lengthResp,
                                                                NULL,
                                                                0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChan, "Failed to send request");
                goto cleanup;
            }
        }
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdPowerMeterConfigGet(adi_adrv904x_Device_t* const                   device,
                                                                     const adi_adrv904x_TxChannels_e                dpdTxChannelSel,
                                                                     adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t* const dpdPowerMeterCfg)
{
        const uint16_t DPD_POWER_METER_CONFIG_GET = 2;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdPowerMeterCfg, cleanup);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)dpdTxChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dpdTxChannelSel, "channel parameter is invalid.");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_DfeAppControlCmdExec( device,
                                                        ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                        DPD_POWER_METER_CONFIG_GET,
                                                        (adi_adrv904x_Channels_e)dpdTxChannelSel,
                                                        NULL,
                                                        0,
                                                        &lengthResp,
                                                        (uint8_t*)dpdPowerMeterCfg,
                                                        sizeof(*dpdPowerMeterCfg));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        dpdPowerMeterCfg->dpdInOutQualLim = ADRV904X_CTOHL(dpdPowerMeterCfg->dpdInOutQualLim);
        dpdPowerMeterCfg->dpdInOutMinGainLim = ADRV904X_CTOHL(dpdPowerMeterCfg->dpdInOutMinGainLim);
        dpdPowerMeterCfg->dpdInOutMaxGainLim = ADRV904X_CTOHL(dpdPowerMeterCfg->dpdInOutMaxGainLim);
        dpdPowerMeterCfg->dpdInOutPwrMeasContDlyCntr = ADRV904X_CTOHS(dpdPowerMeterCfg->dpdInOutPwrMeasContDlyCntr);
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdCaptureBuffersAccessSet(adi_adrv904x_Device_t* const                                    device,
                                                                         const uint32_t                                                  dpdTxChannelMask,
                                                                         adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessSet_t* const dpdCapBufAccess)
{
        const uint16_t DPD_CTRL_CAPTURE_BUF_ACCESS_SET = 5 | DPD_CTRL_SET_CMD_FLAG;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdCapBufAccess, cleanup);

    recoveryAction = adrv904x_DfeCaptureBuffersAccessSet(device,
                                                         dpdTxChannelMask,
                                                         ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                         DPD_CTRL_CAPTURE_BUF_ACCESS_SET,
                                                         dpdCapBufAccess);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to send request");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdCaptureBuffersAccessGet(adi_adrv904x_Device_t* const                                    device,
                                                                         const adi_adrv904x_TxChannels_e                                 dpdTxChannelSel,
                                                                         adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessGet_t* const dpdCapBufAccess)
{
        const uint16_t DPD_CTRL_CAPTURE_BUF_ACCESS_GET = 5;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdCapBufAccess, cleanup);

    recoveryAction =  adrv904x_DfeCaptureBuffersAccessGet(device,
                                                          dpdTxChannelSel,
                                                          ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                          DPD_CTRL_CAPTURE_BUF_ACCESS_GET,
                                                          dpdCapBufAccess);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to send request");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeDpdActSatStatusGet(adi_adrv904x_Device_t* const                        device,
                                                                    const adi_adrv904x_Channels_e                       channel,
                                                                    adi_adrv904x_DfeSvcDfeActuatorSaturation_t* const   calDpdActSatStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const adi_adrv904x_DfeAppFrameworkTrackingCalibrationMask_e calId = ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TX_DPD_MASK;
    adi_adrv904x_DfeAppCalDpdStatusInt_t specificCalStatus;

    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, calDpdActSatStatus, cleanup);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel(channel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "channel parameter is invalid.");
        goto cleanup;
    }

    /* Use the common cal status get handler to get the data from the CPU */
    recoveryAction = adi_adrv904x_DfeCalSpecificStatusGet(device,
                                                          channel,
                                                          adrv904x_DfeTrackingCalToObjId(calId),
                                                          (uint8_t *)&specificCalStatus,
                                                          sizeof(specificCalStatus));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting tracking cal status.");
        goto cleanup;
    }

    calDpdActSatStatus->dinsqCalcCmulSat = ADRV904X_CTOHL(specificCalStatus.dfeActSatStatus.dinsqCalcCmulSat);
    calDpdActSatStatus->gmpDdrDpSat = ADRV904X_CTOHL(specificCalStatus.dfeActSatStatus.gmpDdrDpSat);
    calDpdActSatStatus->magnitudeGainSat = ADRV904X_CTOHL(specificCalStatus.dfeActSatStatus.magnitudeGainSat);

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeDpdActiveModelTableGet(adi_adrv904x_Device_t* const    device,
                                                                        const adi_adrv904x_Channels_e   channel,
                                                                        uint8_t* const                  calDpdActiveModelTable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const adi_adrv904x_DfeAppFrameworkTrackingCalibrationMask_e calId = ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TX_DPD_MASK;
    adi_adrv904x_DfeAppCalDpdStatusInt_t specificCalStatus;

    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, calDpdActiveModelTable, cleanup);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel(channel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "channel parameter is invalid.");
        goto cleanup;
    }

    /* Use the common cal status get handler to get the data from the CPU */
    recoveryAction = adi_adrv904x_DfeCalSpecificStatusGet(  device,
                                                            channel,
                                                            adrv904x_DfeTrackingCalToObjId(calId),
                                                            (uint8_t *)&specificCalStatus,
                                                            sizeof(specificCalStatus));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting tracking cal status.");
        goto cleanup;
    }

    *calDpdActiveModelTable = specificCalStatus.activeModel;

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeDpdCaptureDataFromBufMemGet(adi_adrv904x_Device_t* const          device,
                                                                             const adi_adrv904x_Channels_e         orxChannelSel,
                                                                             adi_adrv904x_DfeAppDpdCaptureData_t*  dpdCaptureData)
{
             const uint32_t CAPBUFF0_ADDR = 0xb2080000U;
    const uint32_t CAPBUFF1_ADDR = 0xb2180000U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_Bf_CapBufMmr_Ch0SrcSel_e ch0SrcSel = ADRV904X_BF_CAP_BUF_MMR_CH0_SRC_SEL_RSVD;
    adrv904x_Bf_CapBufMmr_Ch1SrcSel_e ch1SrcSel = ADRV904X_BF_CAP_BUF_MMR_CH1_SRC_SEL_RSVD;
    adrv904x_Bf_CapBufMmr_Ch2SrcSel_e ch2SrcSel = ADRV904X_BF_CAP_BUF_MMR_CH2_SRC_SEL_RSVD;
    adrv904x_Bf_CapBufMmr_CapEnSrc_e capEnSrc = ADRV904X_BF_CAP_BUF_MMR_CAP_EN_SRC_CAP_EN_NONE;
    adrv904x_Bf_CapBufMmr_CapPauseSrc_e capPauseSrc = ADRV904X_BF_CAP_BUF_MMR_CAP_PAUSE_SRC_CAP_PAUSE_SLICE;
    ADI_PLATFORM_LARGE_ARRAY_ALLOC(uint32_t, captureBuffer, ADI_ADRV904X_DFE_APP_CAL_DPD_CAPTURE_SAMPLES_NUM);
    adrv904x_BfCapBufMmrChanAddr_e capBufMmrRegisterBankBaseAddr = ADRV904X_BF_PROC_DFE_PERIP_CAPBUF_0_;
    uint32_t captureBufferAddr[ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_NUM_OF_CH_SRC_SEL] = { 0U };
    uint32_t capBufMmrRamBaseAddr = CAPBUFF0_ADDR;
    adi_adrv904x_DfeSvcDfeCaptureChSrcSel_e srcSelForCh[ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_NUM_OF_CH_SRC_SEL] =
                                                         { ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_NUM_OF_CH_SRC_SEL };
    uint16_t capDepth = 0U;
    uint8_t capChannel = 0U;
    uint8_t capNumChannels = 0U;
    uint32_t sampleIdx = 0U;
    uint16_t sampleArrSize = 0U;
    int16_t* dpdCaptureDataI = NULL;
    int16_t* dpdCaptureDataQ = NULL;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, captureBuffer, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdCaptureData, cleanup);

    ADI_LIBRARY_MEMSET(dpdCaptureData, 0, sizeof(adi_adrv904x_DfeAppDpdCaptureData_t));

    /* orxChannelSel must contain a single ORx channel number (0-1) */
    if ((orxChannelSel != ADI_ADRV904X_CH0) &&
        (orxChannelSel != ADI_ADRV904X_CH1))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        goto cleanup;
    }

    /* Update the capture buffer register bank and RAM addresses base on TX channel */
    if (orxChannelSel > ADI_ADRV904X_CH0)
    {
        capBufMmrRamBaseAddr = CAPBUFF1_ADDR;
        capBufMmrRegisterBankBaseAddr = ADRV904X_BF_PROC_DFE_PERIP_CAPBUF_1_;
    }

    /* Get the number of captured samples from CAP_DEPTH.Capture_depth bitfield */
    recoveryAction = adrv904x_CapBufMmr_CaptureDepth_BfGet(device,
                                                           NULL,
                                                           capBufMmrRegisterBankBaseAddr,
                                                           &capDepth);
    if ((recoveryAction != ADI_ADRV904X_ERR_ACT_NONE) || (capDepth == 0))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, capDepth, "Invalid value of CAP_DEPTH.Capture_depth");
        goto cleanup;
    }

    sampleArrSize = 2 * (capDepth + 1);
    if(sampleArrSize > ADI_ADRV904X_DFE_APP_CAL_DPD_CAPTURE_SAMPLES_NUM)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, capDepth, "Unsupported sample array size");
        goto cleanup;
    }

    dpdCaptureData->txCaptureData.dpdCaptureSampleArrSize    = sampleArrSize;
    dpdCaptureData->txAltCaptureData.dpdCaptureSampleArrSize = sampleArrSize;
    dpdCaptureData->orxCaptureData.dpdCaptureSampleArrSize   = sampleArrSize;

    /* Get the number of channels for capture from CAP_NUM_CHANNELS.num_channels bitfield */
    recoveryAction = adrv904x_CapBufMmr_NumChannels_BfGet(device,
                                                          NULL,
                                                          capBufMmrRegisterBankBaseAddr,
                                                          &capNumChannels);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, capNumChannels, "Failed to retrieve the value of CAP_NUM_CHANNELS.num_channels");
        goto cleanup;
    }

    /* Check if capNumChannels parameter is valid */
    if(capNumChannels == 2U)
    {
        captureBufferAddr[0U] = capBufMmrRamBaseAddr;
        captureBufferAddr[1U] = captureBufferAddr[0U] + 0x8000;
        captureBufferAddr[2U] = 0U;
    }
    else if(capNumChannels == 3U)
    {
        captureBufferAddr[0U] = capBufMmrRamBaseAddr;
        captureBufferAddr[1U] = captureBufferAddr[0U] + 0x8000;
        captureBufferAddr[2U] = captureBufferAddr[0U] + 0x4000;

    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               capNumChannels,
                               "Invalid number of channels for capture");
        goto cleanup;
    }

    recoveryAction = adrv904x_CapBufMmr_Ch0SrcSel_BfGet(device,
                                                        NULL,
                                                        capBufMmrRegisterBankBaseAddr,
                                                        &ch0SrcSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ch0SrcSel, "Failed to retrieve the value of CAP_CH_SRC_SEL.ch0_src_sel");
        goto cleanup;
    }
    if (ch0SrcSel > ADRV904X_BF_CAP_BUF_MMR_CH0_SRC_SEL_ORX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ch0SrcSel, "Failed to retrieve the value of CAP_CH_SRC_SEL.ch0_src_sel");
        goto cleanup;
    }

    recoveryAction = adrv904x_CapBufMmr_Ch1SrcSel_BfGet(device,
                                                        NULL,
                                                        capBufMmrRegisterBankBaseAddr,
                                                        &ch1SrcSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ch1SrcSel, "Failed to retrieve the value of CAP_CH_SRC_SEL.ch1src_sel");
        goto cleanup;
    }
    if (ch1SrcSel > ADRV904X_BF_CAP_BUF_MMR_CH1_SRC_SEL_ORX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ch1SrcSel, "Failed to retrieve the value of CAP_CH_SRC_SEL.ch1_src_sel");
        goto cleanup;
    }

    if (capNumChannels == 3U)
    {
        recoveryAction = adrv904x_CapBufMmr_Ch2SrcSel_BfGet(device,
                                                            NULL,
                                                            capBufMmrRegisterBankBaseAddr,
                                                            &ch2SrcSel);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ch2SrcSel, "Failed to retrieve the value of CAP_CH_SRC_SEL.ch2src_sel");
            goto cleanup;
        }
        if (ch2SrcSel > ADRV904X_BF_CAP_BUF_MMR_CH2_SRC_SEL_ORX)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ch2SrcSel, "Failed to retrieve the value of CAP_CH_SRC_SEL.ch2src_sel");
            goto cleanup;
        }
    }

    srcSelForCh[0] = (adi_adrv904x_DfeSvcDfeCaptureChSrcSel_e)ch0SrcSel;
    srcSelForCh[1] = (adi_adrv904x_DfeSvcDfeCaptureChSrcSel_e)ch1SrcSel;
    srcSelForCh[2] = (adi_adrv904x_DfeSvcDfeCaptureChSrcSel_e)ch2SrcSel;

    /* Get cap_en_src mode  */
    recoveryAction = adrv904x_CapBufMmr_CapEnSrc_BfGet(device,
                                                       NULL,
                                                       capBufMmrRegisterBankBaseAddr,
                                                       &capEnSrc);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set cap_en_src to CAP_EN_SW mode");
        goto cleanup;
    }

    /* Get cap_pause_src */
    recoveryAction = adrv904x_CapBufMmr_CapPauseSrc_BfGet(device,
                                                          NULL,
                                                          capBufMmrRegisterBankBaseAddr,
                                                          &capPauseSrc);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set cap_pause_src to CAP_PAUSE_SW mode");
        goto cleanup;
    }

    /* Disable clock  */
    recoveryAction = adrv904x_CapBufMmr_ClockEnable_BfSet(device,
                                                          NULL,
                                                          capBufMmrRegisterBankBaseAddr,
                                                          0);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to disable clock");
        goto cleanup;
    }

    /* Enable the capture buffer debug mode */
    recoveryAction = adrv904x_CapBufMmr_DebugModeEn_BfSet(device,
                                                          NULL,
                                                          capBufMmrRegisterBankBaseAddr,
                                                          1);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable capture buffer debug mode");
        goto cleanup;
    }

    /* Enable clock  */
    recoveryAction = adrv904x_CapBufMmr_ClockEnable_BfSet(device,
                                                          NULL,
                                                          capBufMmrRegisterBankBaseAddr,
                                                          1);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to disable clock");
        goto cleanup;
    }

    /* Set cap_en_src to CAP_EN_SW mode */
    recoveryAction = adrv904x_CapBufMmr_CapEnSrc_BfSet(device,
                                                       NULL,
                                                       capBufMmrRegisterBankBaseAddr,
                                                       ADRV904X_BF_CAP_BUF_MMR_CAP_EN_SRC_CAP_EN_SW);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set cap_en_src to CAP_EN_SW mode");
        goto cleanup;
    }

    /* Set cap_pause_src to CAP_PAUSE_SW mode */
    recoveryAction = adrv904x_CapBufMmr_CapPauseSrc_BfSet(device,
                                                          NULL,
                                                          capBufMmrRegisterBankBaseAddr,
                                                          ADRV904X_BF_CAP_BUF_MMR_CAP_PAUSE_SRC_CAP_PAUSE_SW);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set cap_pause_src to CAP_PAUSE_SW mode");
        goto cleanup;
    }

    for (capChannel = 0; capChannel < capNumChannels; capChannel++)
    {
        /* Check address of buffer used for capture channel 2 */
        if ((capChannel == 2U) && (captureBufferAddr[2] == 0U))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Buffer address for capture channel 2 is NULL");
            goto cleanup;
        }

        /* Read the capture data through SPI */
        recoveryAction = adi_adrv904x_Registers32Read(device, NULL, captureBufferAddr[capChannel], captureBuffer, NULL, sampleArrSize);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DPD capture data");
            goto cleanup;
        }

        if (srcSelForCh[capChannel] == ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_CH_SRC_SEL_TX)
        {
            dpdCaptureDataI = dpdCaptureData->txCaptureData.dpdCaptureDataI;
            dpdCaptureDataQ = dpdCaptureData->txCaptureData.dpdCaptureDataQ;
        }
        else if (srcSelForCh[capChannel] == ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_CH_SRC_SEL_ALT_TX)
        {
            dpdCaptureDataI = dpdCaptureData->txAltCaptureData.dpdCaptureDataI;
            dpdCaptureDataQ = dpdCaptureData->txAltCaptureData.dpdCaptureDataQ;
        }
        else
        {
            dpdCaptureDataI = dpdCaptureData->orxCaptureData.dpdCaptureDataI;
            dpdCaptureDataQ = dpdCaptureData->orxCaptureData.dpdCaptureDataQ;
        }

        for (sampleIdx = 0U; sampleIdx < sampleArrSize; sampleIdx++)
        {
            dpdCaptureDataQ[sampleIdx] = captureBuffer[sampleIdx] & 0x0000FFFFU;
            dpdCaptureDataI[sampleIdx] = (captureBuffer[sampleIdx] >> 16U) & 0x0000FFFFU;
        }
    }

    recoveryAction = adrv904x_CapBufMmr_CapEnSrc_BfSet(device,
                                                       NULL,
                                                       capBufMmrRegisterBankBaseAddr,
                                                       capEnSrc);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set cap_en_src to CAP_EN_SW mode");
        goto cleanup;
    }

    recoveryAction = adrv904x_CapBufMmr_CapPauseSrc_BfSet(device,
                                                          NULL,
                                                          capBufMmrRegisterBankBaseAddr,
                                                          capPauseSrc);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set cap_pause_src to CAP_PAUSE_SW mode");
        goto cleanup;
    }

    /* Disable the capture buffer debug mode */
    recoveryAction = adrv904x_CapBufMmr_DebugModeEn_BfSet(device,
                                                          NULL,
                                                          capBufMmrRegisterBankBaseAddr,
                                                          0);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to disable capture buffer debug mode");
        goto cleanup;
    }
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


static adi_adrv904x_ErrAction_e adrv904x_DfeDpdCalSpecificStatusGet(adi_adrv904x_Device_t* const                device,
                                                                    const adi_adrv904x_Channels_e               channel,
                                                                    adi_adrv904x_DfeAppCalDpdStatusInt_t* const calDpdSpecStatus)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const adi_adrv904x_DfeAppFrameworkTrackingCalibrationMask_e calId = ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TX_DPD_MASK;

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, calDpdSpecStatus);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel(channel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "channel parameter is invalid.");
        return recoveryAction;
    }

    /* Use the common cal status get handler to get the data from the CPU */
    recoveryAction = adi_adrv904x_DfeCalSpecificStatusGet(device,
                                                          channel,
                                                          adrv904x_DfeTrackingCalToObjId(calId),
                                                          (uint8_t *)calDpdSpecStatus,
                                                          sizeof(*calDpdSpecStatus));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting tracking cal status.");
        return recoveryAction;
    }

    /* Translate the response from the CPU */
    calDpdSpecStatus->hdr.errorCode = ADRV904X_CTOHL(calDpdSpecStatus->hdr.errorCode);
    calDpdSpecStatus->hdr.iterCount = ADRV904X_CTOHL(calDpdSpecStatus->hdr.iterCount);
    calDpdSpecStatus->hdr.percentComplete = ADRV904X_CTOHL(calDpdSpecStatus->hdr.percentComplete);
    calDpdSpecStatus->hdr.performanceMetric = ADRV904X_CTOHL(calDpdSpecStatus->hdr.performanceMetric);
    calDpdSpecStatus->hdr.updateCount = ADRV904X_CTOHL(calDpdSpecStatus->hdr.updateCount);

    calDpdSpecStatus->dpdPathDlyStatus.intActDelay  = ADRV904X_CTOHL(calDpdSpecStatus->dpdPathDlyStatus.intActDelay);
    calDpdSpecStatus->dpdPathDlyStatus.intExtDelay  = ADRV904X_CTOHL(calDpdSpecStatus->dpdPathDlyStatus.intExtDelay);
    calDpdSpecStatus->dpdPathDlyStatus.fracExtDelay = ADRV904X_CTOHL(calDpdSpecStatus->dpdPathDlyStatus.fracExtDelay);

    calDpdSpecStatus->dfeActSatStatus.dinsqCalcCmulSat = ADRV904X_CTOHL(calDpdSpecStatus->dfeActSatStatus.dinsqCalcCmulSat);
    calDpdSpecStatus->dfeActSatStatus.gmpDdrDpSat = ADRV904X_CTOHL(calDpdSpecStatus->dfeActSatStatus.gmpDdrDpSat);
    calDpdSpecStatus->dfeActSatStatus.magnitudeGainSat = ADRV904X_CTOHL(calDpdSpecStatus->dfeActSatStatus.magnitudeGainSat);

    calDpdSpecStatus->activeModel = ADRV904X_CTOHL(calDpdSpecStatus->activeModel);
    calDpdSpecStatus->updatedModel = ADRV904X_CTOHL(calDpdSpecStatus->updatedModel);    
    calDpdSpecStatus->powerR = ADRV904X_CTOHL(calDpdSpecStatus->powerR);
    calDpdSpecStatus->txPower = ADRV904X_CTOHL(calDpdSpecStatus->txPower);
    calDpdSpecStatus->copiedModelMask = calDpdSpecStatus->copiedModelMask;

    for (uint8_t i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_PARTIAL_GRP; i++)
    {
        calDpdSpecStatus->powerM[i] = ADRV904X_CTOHL(calDpdSpecStatus->powerM[i]);
        calDpdSpecStatus->powerC[i] = ADRV904X_CTOHL(calDpdSpecStatus->powerC[i]);
    }

    for (int i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_METRICS_LEN; i++)
    {
        calDpdSpecStatus->stability_int[i] = ADRV904X_CTOHL(calDpdSpecStatus->stability_int[i]);
    }
    for (int i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_ERR_WORD_LEN; i++)
    {
        calDpdSpecStatus->errStatWord[i] = ADRV904X_CTOHS(calDpdSpecStatus->errStatWord[i]);
    }
    for (int i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_ERR_WORD_LEN; i++)
    {
        calDpdSpecStatus->actionWord[i] = ADRV904X_CTOHS(calDpdSpecStatus->actionWord[i]);
    }
    calDpdSpecStatus->capStartRetryCount     = ADRV904X_CTOHL(calDpdSpecStatus->capStartRetryCount);
    calDpdSpecStatus->capAbortRetryCount     = ADRV904X_CTOHL(calDpdSpecStatus->capAbortRetryCount);
    calDpdSpecStatus->capInvalidRetryCount   = ADRV904X_CTOHL(calDpdSpecStatus->capInvalidRetryCount);
    calDpdSpecStatus->maxFracEstErrorCount   = ADRV904X_CTOHL(calDpdSpecStatus->maxFracEstErrorCount);
    calDpdSpecStatus->cholSolverErrorCount   = ADRV904X_CTOHL(calDpdSpecStatus->cholSolverErrorCount);
    calDpdSpecStatus->appExitRequestCount    = ADRV904X_CTOHL(calDpdSpecStatus->appExitRequestCount);
    calDpdSpecStatus->lutEntrySatErrorCount  = ADRV904X_CTOHL(calDpdSpecStatus->lutEntrySatErrorCount);
    calDpdSpecStatus->flutEntrySatErrorCount = ADRV904X_CTOHL(calDpdSpecStatus->flutEntrySatErrorCount);
    calDpdSpecStatus->stabilityErrorCount    = ADRV904X_CTOHL(calDpdSpecStatus->stabilityErrorCount);
    calDpdSpecStatus->lowPowerErrorCount     = ADRV904X_CTOHL(calDpdSpecStatus->lowPowerErrorCount);
    calDpdSpecStatus->highPowerErrorCount    = ADRV904X_CTOHL(calDpdSpecStatus->highPowerErrorCount);
    calDpdSpecStatus->saturationErrorCount   = ADRV904X_CTOHL(calDpdSpecStatus->saturationErrorCount);
    calDpdSpecStatus->lowPeakErrorCount      = ADRV904X_CTOHL(calDpdSpecStatus->lowPeakErrorCount);
    calDpdSpecStatus->capAbortErrorCount     = ADRV904X_CTOHL(calDpdSpecStatus->capAbortErrorCount);
    calDpdSpecStatus->pathDelayErrorCount    = ADRV904X_CTOHL(calDpdSpecStatus->pathDelayErrorCount);
    calDpdSpecStatus->capInvalidErrorCount   = ADRV904X_CTOHL(calDpdSpecStatus->capInvalidErrorCount);
    calDpdSpecStatus->noLutUpdateCount       = ADRV904X_CTOHL(calDpdSpecStatus->noLutUpdateCount);
    calDpdSpecStatus->noFlutUpdateCount      = ADRV904X_CTOHL(calDpdSpecStatus->noFlutUpdateCount);
    
    calDpdSpecStatus->capStartErrorCount    = ADRV904X_CTOHL(calDpdSpecStatus->capStartErrorCount);
    calDpdSpecStatus->periodEndedErrorCount = ADRV904X_CTOHL(calDpdSpecStatus->periodEndedErrorCount);
    calDpdSpecStatus->lutUpdDmaNotTrigCount = ADRV904X_CTOHL(calDpdSpecStatus->lutUpdDmaNotTrigCount);
    calDpdSpecStatus->tooFewBatchesCount    = ADRV904X_CTOHL(calDpdSpecStatus->tooFewBatchesCount);
    calDpdSpecStatus->ctc2FlutSwapFailedCount = ADRV904X_CTOHL(calDpdSpecStatus->ctc2FlutSwapFailedCount);
    for (uint8_t i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS; i++)
    {
        calDpdSpecStatus->lutSaturationMask[i]  = ADRV904X_CTOHLL(calDpdSpecStatus->lutSaturationMask[i]);
    }
    
    for (uint8_t i = 0; i < ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_COUNT; i++)
    {
        calDpdSpecStatus->flutSaturationMask[i] = ADRV904X_CTOHLL(calDpdSpecStatus->flutSaturationMask[i]);
    }
    
    calDpdSpecStatus->lastIterationType = ADRV904X_CTOHL(calDpdSpecStatus->lastIterationType);
    for (uint8_t i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_CAP_BATCHES; i++)
    {
        calDpdSpecStatus->ctc1UpThresVals[i] =  ADRV904X_CTOHLL(calDpdSpecStatus->ctc1UpThresVals[i]); 
        calDpdSpecStatus->ctc1LowThresVals[i] =  ADRV904X_CTOHLL(calDpdSpecStatus->ctc1LowThresVals[i]);      
    }

    calDpdSpecStatus->ctc1MaxPeakVal = ADRV904X_CTOHLL(calDpdSpecStatus->ctc1MaxPeakVal);                              
    calDpdSpecStatus->ctc1MinPeakVal = ADRV904X_CTOHLL(calDpdSpecStatus->ctc1MinPeakVal);                               

    calDpdSpecStatus->bwDetStatus.beta2Curr_e6      = ADRV904X_CTOHL(calDpdSpecStatus->bwDetStatus.beta2Curr_e6);
    calDpdSpecStatus->bwDetStatus.beta2Prev_e6      = ADRV904X_CTOHL(calDpdSpecStatus->bwDetStatus.beta2Prev_e6);
    calDpdSpecStatus->bwDetStatus.beta2Min_e6       = ADRV904X_CTOHL(calDpdSpecStatus->bwDetStatus.beta2Min_e6);
    calDpdSpecStatus->bwDetStatus.beta2Max_e6       = ADRV904X_CTOHL(calDpdSpecStatus->bwDetStatus.beta2Max_e6);
    calDpdSpecStatus->bwDetStatus.extDelayTrack_e6  = ADRV904X_CTOHLL(calDpdSpecStatus->bwDetStatus.extDelayTrack_e6);
    calDpdSpecStatus->bwDetStatus.bwDetStateCurr = calDpdSpecStatus->bwDetStatus.bwDetStateCurr;
    calDpdSpecStatus->bwDetStatus.bwDetStateBest = calDpdSpecStatus->bwDetStatus.bwDetStateBest;
    calDpdSpecStatus->learningModeCurr           = calDpdSpecStatus->learningModeCurr;
    calDpdSpecStatus->mu                         = calDpdSpecStatus->mu;
    for (uint8_t i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_CAP_BATCHES; i++)
    {
        calDpdSpecStatus->capBatchInfo[i].avgPwrDB = ADRV904X_CTOHS(calDpdSpecStatus->capBatchInfo[i].avgPwrDB);
        calDpdSpecStatus->capBatchInfo[i].parDB = ADRV904X_CTOHS(calDpdSpecStatus->capBatchInfo[i].parDB);
        calDpdSpecStatus->capBatchInfo[i].selectedModel = calDpdSpecStatus->capBatchInfo[i].selectedModel;
        calDpdSpecStatus->capBatchInfo[i].status = calDpdSpecStatus->capBatchInfo[i].status;
    }
    calDpdSpecStatus->ctc2GainCalcErrorCount     = ADRV904X_CTOHL(calDpdSpecStatus->ctc2GainCalcErrorCount);
    calDpdSpecStatus->ctc2CaptureErrorCount      = ADRV904X_CTOHL(calDpdSpecStatus->ctc2CaptureErrorCount);
    calDpdSpecStatus->ctc2CapDelayValid          = calDpdSpecStatus->ctc2CapDelayValid;
    calDpdSpecStatus->ctc2FracDelayNormPfir_e6      = ADRV904X_CTOHL(calDpdSpecStatus->ctc2FracDelayNormPfir_e6);
    calDpdSpecStatus->ctc2PreDpdToOrxPathDelay   = ADRV904X_CTOHL(calDpdSpecStatus->ctc2PreDpdToOrxPathDelay); 
    for (uint8_t i = 0; i < ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW; i++)
    {
        calDpdSpecStatus->maxIIROutput[i] = ADRV904X_CTOHL(calDpdSpecStatus->maxIIROutput[i]);
        calDpdSpecStatus->minIIROutput[i] = ADRV904X_CTOHL(calDpdSpecStatus->minIIROutput[i]);
        calDpdSpecStatus->minIIRState[i] = ADRV904X_CTOHL(calDpdSpecStatus->minIIRState[i]);
        calDpdSpecStatus->ctc2FlutOutput[i].flutMaxReal = ADRV904X_CTOHL(calDpdSpecStatus->ctc2FlutOutput[i].flutMaxReal);
        calDpdSpecStatus->ctc2FlutOutput[i].flutMaxImag = ADRV904X_CTOHL(calDpdSpecStatus->ctc2FlutOutput[i].flutMaxImag);
        calDpdSpecStatus->ctc2FlutOutput[i].flutMinReal = ADRV904X_CTOHL(calDpdSpecStatus->ctc2FlutOutput[i].flutMinReal);
        calDpdSpecStatus->ctc2FlutOutput[i].flutMinImag = ADRV904X_CTOHL(calDpdSpecStatus->ctc2FlutOutput[i].flutMinImag);
        calDpdSpecStatus->ctc2RowOutput[i].rowMaxReal   = ADRV904X_CTOHL(calDpdSpecStatus->ctc2RowOutput[i].rowMaxReal);
        calDpdSpecStatus->ctc2RowOutput[i].rowMaxImag   = ADRV904X_CTOHL(calDpdSpecStatus->ctc2RowOutput[i].rowMaxImag);
        calDpdSpecStatus->ctc2RowOutput[i].rowMinReal   = ADRV904X_CTOHL(calDpdSpecStatus->ctc2RowOutput[i].rowMinReal);
        calDpdSpecStatus->ctc2RowOutput[i].rowMinImag   = ADRV904X_CTOHL(calDpdSpecStatus->ctc2RowOutput[i].rowMinImag);	
    }

    calDpdSpecStatus->ctc2FlutSatRowMap = calDpdSpecStatus->ctc2FlutSatRowMap;
    calDpdSpecStatus->magGainSatErrorCount = ADRV904X_CTOHL(calDpdSpecStatus->magGainSatErrorCount); 
    for (uint8_t i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS; i++)
    {
        calDpdSpecStatus->gmpUpdateCount[i] = ADRV904X_CTOHL(calDpdSpecStatus->gmpUpdateCount[i]);
    }
    calDpdSpecStatus->ctc2EnvPwrRtViolCount = ADRV904X_CTOHL(calDpdSpecStatus->ctc2EnvPwrRtViolCount);
    calDpdSpecStatus->ctc2EnvPwrRatioDB     = ADRV904X_CTOHL(calDpdSpecStatus->ctc2EnvPwrRatioDB);
    calDpdSpecStatus->ctc2EnvPwrAllZeroCount     = ADRV904X_CTOHL(calDpdSpecStatus->ctc2EnvPwrAllZeroCount);

    return recoveryAction;
}


#ifndef ADI_LIBRARY_RM_FLOATS
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeDpdCalSpecificStatusGet(adi_adrv904x_Device_t* const               device,
                                                                         const adi_adrv904x_Channels_e              channel,
                                                                         adi_adrv904x_DfeAppCalDpdStatus_t* const   calDpdSpecStatus)
{
    	adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, calDpdSpecStatus, cleanup);

    recoveryAction = adrv904x_DfeDpdCalSpecificStatusGet(device,
                                                         channel,
                                                         (adi_adrv904x_DfeAppCalDpdStatusInt_t*)calDpdSpecStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting tracking cal status.");
        goto cleanup;
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
#endif /* ADI_LIBRARY_RM_FLOATS */

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeDpdCalSpecificStatusGet_int(adi_adrv904x_Device_t* const                  device,
                                                                             const adi_adrv904x_Channels_e                 channel,
                                                                             adi_adrv904x_DfeAppCalDpdStatusInt_t* const   calDpdSpecStatus)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, calDpdSpecStatus, cleanup);

    recoveryAction = adrv904x_DfeDpdCalSpecificStatusGet(device, channel, calDpdSpecStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting tracking cal status.");
        goto cleanup;
    }

    for (int i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_METRICS_LEN; i++)
    {
        adi_library_fp32ToScaledInt(calDpdSpecStatus->stability_int[i], 1, &calDpdSpecStatus->stability_int[i]);
    }
    adi_library_fp32ToScaledInt(calDpdSpecStatus->bwDetStatus.beta2Curr_e6     ,1000000, &calDpdSpecStatus->bwDetStatus.beta2Curr_e6);
    adi_library_fp32ToScaledInt(calDpdSpecStatus->bwDetStatus.beta2Prev_e6     ,1000000, &calDpdSpecStatus->bwDetStatus.beta2Prev_e6);
    adi_library_fp32ToScaledInt(calDpdSpecStatus->bwDetStatus.beta2Min_e6      ,1000000, &calDpdSpecStatus->bwDetStatus.beta2Min_e6);
    adi_library_fp32ToScaledInt(calDpdSpecStatus->bwDetStatus.beta2Max_e6      ,1000000, &calDpdSpecStatus->bwDetStatus.beta2Max_e6);
    adi_library_fp64ToScaledInt(calDpdSpecStatus->bwDetStatus.extDelayTrack_e6 ,1000000, &calDpdSpecStatus->bwDetStatus.extDelayTrack_e6);
    adi_library_fp32ToScaledInt(calDpdSpecStatus->ctc2FracDelayNormPfir_e6     ,1000000, &calDpdSpecStatus->ctc2FracDelayNormPfir_e6);

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdStabilityCfgSet(adi_adrv904x_Device_t* const                   device,
                                                                 const uint32_t                                 dpdTxChannelMask,
                                                                 adi_adrv904x_DfeAppCalDpdStabilityCfg_t* const dpdStabilityCfg)
{
             const uint16_t DPD_CTRL_STABILITY_CFG_SET = 6 | DPD_CTRL_SET_CMD_FLAG;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    uint32_t lengthResp = 0;
    uint32_t i = 0;
    adi_adrv904x_DfeAppCalDpdStabilityCfg_t dpdStabilityCfgTmp;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdStabilityCfg, cleanup);

    if (((dpdTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (dpdTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                dpdTxChannelMask,
                                "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    ADI_LIBRARY_MEMSET(&dpdStabilityCfgTmp, 0, sizeof(adi_adrv904x_DfeAppCalDpdStabilityCfg_t));

	for (i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_CHECK_LEN; i++)
    {
        dpdStabilityCfgTmp.bit[i].LTGT = ADRV904X_HTOCS(dpdStabilityCfg->bit[i].LTGT);
        dpdStabilityCfgTmp.bit[i].persistentCnt = ADRV904X_HTOCS(dpdStabilityCfg->bit[i].persistentCnt);
        dpdStabilityCfgTmp.bit[i].threshold0 = ADRV904X_HTOCS(dpdStabilityCfg->bit[i].threshold0);
        dpdStabilityCfgTmp.bit[i].threshold1 = ADRV904X_HTOCS(dpdStabilityCfg->bit[i].threshold1);
    }

    for (i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_ERR_WORD_LEN; i++)
    {
        dpdStabilityCfgTmp.error[i].actionWord = ADRV904X_HTOCS(dpdStabilityCfg->error[i].actionWord);
        dpdStabilityCfgTmp.error[i].mask = ADRV904X_HTOCS(dpdStabilityCfg->error[i].mask);
    }

    for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        uint32_t txChan = (1 << i);
        if (dpdTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               DPD_CTRL_STABILITY_CFG_SET,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               (const uint8_t*)&dpdStabilityCfgTmp,
                                                               sizeof(dpdStabilityCfgTmp),
                                                               &lengthResp,
                                                               NULL,
                                                               0);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChan, "Failed to send request");
                goto cleanup;
            }
        }
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdStabilityCfgGet(adi_adrv904x_Device_t* const                   device,
                                                                 const adi_adrv904x_TxChannels_e                dpdTxChannelSel,
                                                                 adi_adrv904x_DfeAppCalDpdStabilityCfg_t* const dpdStabilityCfg)
{
             const uint16_t DPD_CTRL_STABILITY_CFG_GET = 6;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;
    adi_adrv904x_DfeAppCalDpdStabilityCfg_t dpdStabilityCfgTmp;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdStabilityCfg, cleanup);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)dpdTxChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dpdTxChannelSel, "channel parameter is invalid.");
        goto cleanup;
    }

    ADI_LIBRARY_MEMSET(&dpdStabilityCfgTmp, 0, sizeof(adi_adrv904x_DfeAppCalDpdStabilityCfg_t));

    recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                       ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                       DPD_CTRL_STABILITY_CFG_GET,
                                                       (adi_adrv904x_Channels_e)dpdTxChannelSel,
                                                       NULL,
                                                       0,
                                                       &lengthResp,
                                                       (uint8_t*)&dpdStabilityCfgTmp,
                                                       sizeof(dpdStabilityCfgTmp));

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dpdTxChannelSel, "Failed to send request");
        goto cleanup;
    }

	for (i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_CHECK_LEN; i++)
    {
        dpdStabilityCfg->bit[i].LTGT = ADRV904X_CTOHS(dpdStabilityCfgTmp.bit[i].LTGT);
        dpdStabilityCfg->bit[i].persistentCnt = ADRV904X_CTOHS(dpdStabilityCfgTmp.bit[i].persistentCnt);
        dpdStabilityCfg->bit[i].threshold0 = ADRV904X_CTOHS(dpdStabilityCfgTmp.bit[i].threshold0);
        dpdStabilityCfg->bit[i].threshold1 = ADRV904X_CTOHS(dpdStabilityCfgTmp.bit[i].threshold1);
    }

    for (i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_ERR_WORD_LEN; i++)
    {
        dpdStabilityCfg->error[i].actionWord = ADRV904X_CTOHS(dpdStabilityCfgTmp.error[i].actionWord);
        dpdStabilityCfg->error[i].mask = ADRV904X_CTOHS(dpdStabilityCfgTmp.error[i].mask);
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_WbRegBufferAccess(adi_adrv904x_Device_t* const                   device,
                                                                adi_adrv904x_DfeAppwbRegularization_t* const   dfeAppWbReg)
{
             adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_NO_ERROR;
    adrv904x_DfeSvcCmdStatus_t svcCmdStatus;
    adrv904x_DfeSvcCmdWbRegBufferAccessResp_t dfeSvcWbRegResp;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dfeAppWbReg, cleanup);

    ADI_LIBRARY_MEMSET(&dfeSvcWbRegResp, 0, sizeof(adrv904x_DfeSvcCmdWbRegBufferAccessResp_t));

    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_WBREG_ACCESS, 
                                            NULL,
                                            0,
                                            (void*)&dfeSvcWbRegResp,
                                            sizeof(adrv904x_DfeSvcCmdWbRegBufferAccessResp_t),
                                            &svcCmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(dfeSvcWbRegResp.status, (adrv904x_CpuCmdStatus_e)svcCmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    dfeAppWbReg->wbRegStartAddress = ADRV904X_CTOHLL(dfeSvcWbRegResp.wbRegStartAddress);
    dfeAppWbReg->wbRegBufferSize = ADRV904X_CTOHL(dfeSvcWbRegResp.wbRegBufferSize);

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_WbRegBufferWrite(adi_adrv904x_Device_t* const             device,
                                                               adi_adrv904x_DfeAppwbRegData_t* const    DfeAppWbRegData)
{
             uint32_t i = 0u;
    uint16_t* DfeAppWbRegDataTmp = (uint16_t*)DfeAppWbRegData;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adi_adrv904x_SpiCache_t spiCache;
    adi_adrv904x_DfeAppwbRegularization_t dfeAppWbRegTmp;
    adi_adrv904x_DfeAppwbRegularization_t dfeAppWbReg;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, DfeAppWbRegData, cleanup);

    ADI_LIBRARY_MEMSET(&dfeAppWbRegTmp, 0, sizeof(adi_adrv904x_DfeAppwbRegularization_t));
    ADI_LIBRARY_MEMSET(&spiCache, 0, sizeof(adi_adrv904x_SpiCache_t));

    recoveryAction = adi_adrv904x_WbRegBufferAccess(device,
                                                    &dfeAppWbRegTmp);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, 0, "Failed to send WBReg access request");
        goto cleanup;
    }

    dfeAppWbReg.wbRegStartAddress = ADRV904X_CTOHLL(dfeAppWbRegTmp.wbRegStartAddress);
    dfeAppWbReg.wbRegBufferSize = ADRV904X_CTOHL(dfeAppWbRegTmp.wbRegBufferSize);
    
    for (i = dfeAppWbReg.wbRegStartAddress; i < (dfeAppWbReg.wbRegStartAddress + dfeAppWbReg.wbRegBufferSize); i += 2U)
    {
        uint16_t val = *DfeAppWbRegDataTmp++;
        recoveryAction = adi_adrv904x_Register32Write(device, (adi_adrv904x_SpiCache_t* const)&spiCache, i, ADRV904X_HTOCS(val), 0x0000FFFFu);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, 0, "Failed to write WBReg data");
            goto cleanup;
        }
    }

    recoveryAction = adi_adrv904x_SpiFlush(device, spiCache.data, (uint32_t* const)&spiCache.count);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, 0, "Failed to send SPI Flush");
        goto cleanup;
    }

    /* call a 2nd time to clear the cache */
    recoveryAction = adi_adrv904x_WbRegBufferAccess(device, &dfeAppWbRegTmp);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, 0, "Failed to send WBReg cleanup");
        goto cleanup;
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdTddLutSwitchCfgSet(adi_adrv904x_Device_t* const                         device,
                                                                    const uint32_t                                       dfeTxChannelMask,
                                                                    const adi_adrv904x_DfeAppDpdActTddLutSwitch_t* const tddLutSwitchConfig)
{
             const uint16_t DPD_CTRL_TDD_LUT_SWITCH_CONFIG_SET = 8 | DPD_CTRL_SET_CMD_FLAG;
    adi_adrv904x_DfeAppDpdActTddLutSwitch_t tmpTddLutSwitchConfig;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, tddLutSwitchConfig, cleanup);

    ADI_LIBRARY_MEMSET(&tmpTddLutSwitchConfig, 0, sizeof(adi_adrv904x_DfeAppDpdActTddLutSwitch_t));
    tmpTddLutSwitchConfig.txOnMdl0SwitchEn = tddLutSwitchConfig->txOnMdl0SwitchEn;
    tmpTddLutSwitchConfig.txOnMdl1SwitchEn = tddLutSwitchConfig->txOnMdl1SwitchEn;
    tmpTddLutSwitchConfig.txOnMdlSwitchDlyCnt = ADRV904X_HTOCL(tddLutSwitchConfig->txOnMdlSwitchDlyCnt);
    
    for (i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_TDD_LUT_SWITCH_CAP_NUM; i++)
    {
        tmpTddLutSwitchConfig.capDelay[i] = ADRV904X_HTOCL(tddLutSwitchConfig->capDelay[i]);
    }
    
    for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        uint32_t txChan = 1 << i;
        if (dfeTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               DPD_CTRL_TDD_LUT_SWITCH_CONFIG_SET,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               (const uint8_t*)&tmpTddLutSwitchConfig,
                                                               sizeof(tmpTddLutSwitchConfig),
                                                               &lengthResp,
                                                               NULL,
                                                               0);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChan, "Failed to send TDD lut switch cfgset request");
                goto cleanup;
            }
        }
    }
    
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);

}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdTddLutSwitchCfgGet(adi_adrv904x_Device_t* const                     device,
                                                                    const adi_adrv904x_TxChannels_e                  dpdTxChannelSel,
                                                                    adi_adrv904x_DfeAppDpdActTddLutSwitch_t* const   tddLutSwitchConfig)
{
             const uint16_t DPD_CTRL_TDD_LUT_SWITCH_CONFIG_GET = 8;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;
    adi_adrv904x_DfeAppDpdActTddLutSwitch_t tmpTddLutSwitchConfig;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, tddLutSwitchConfig, cleanup);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)dpdTxChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dpdTxChannelSel, "channel parameter is invalid.");
        goto cleanup;
    }

    ADI_LIBRARY_MEMSET(&tmpTddLutSwitchConfig, 0, sizeof(adi_adrv904x_DfeAppDpdActTddLutSwitch_t));
    recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                       ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                       DPD_CTRL_TDD_LUT_SWITCH_CONFIG_GET,
                                                       (adi_adrv904x_Channels_e)dpdTxChannelSel,
                                                       NULL,
                                                       0,
                                                       &lengthResp,
                                                       (uint8_t*)&tmpTddLutSwitchConfig,
                                                       sizeof(tmpTddLutSwitchConfig));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dpdTxChannelSel, "Failed to send TDD lut switch cfgget request");
        goto cleanup;
    }

    tddLutSwitchConfig->txOnMdl0SwitchEn = tmpTddLutSwitchConfig.txOnMdl0SwitchEn;
    tddLutSwitchConfig->txOnMdl1SwitchEn = tmpTddLutSwitchConfig.txOnMdl1SwitchEn;
    tddLutSwitchConfig->txOnMdlSwitchDlyCnt = ADRV904X_CTOHL(tmpTddLutSwitchConfig.txOnMdlSwitchDlyCnt);
    
    for (i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_TDD_LUT_SWITCH_CAP_NUM; i++)
    {
        tddLutSwitchConfig->capDelay[i] = ADRV904X_CTOHL(tmpTddLutSwitchConfig.capDelay[i]);
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);

}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdPathdelayCfg(adi_adrv904x_Device_t* const                          device,
                                                              const uint32_t                                        dpdTxChannelMask,
                                                              const adi_adrv904x_DfeAppCalDpdPathDlyStatus_t* const dpdPathdelayCfg)
{
        const uint16_t DPD_CTRL_PATHDELAY_CONFIG = 9U;
    adi_adrv904x_DfeAppCalDpdPathDlyStatus_t tmpDpdPathdelayCfg;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdPathdelayCfg, cleanup);

    ADI_LIBRARY_MEMCPY((void *)&tmpDpdPathdelayCfg, dpdPathdelayCfg, sizeof(adi_adrv904x_DfeAppCalDpdPathDlyStatus_t));

    if ((tmpDpdPathdelayCfg.fracExtDelay > 63) || (tmpDpdPathdelayCfg.fracExtDelay < 0))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               tmpDpdPathdelayCfg.fracExtDelay,
                               "Invalid fracExtDelay is selected. Valid values are non-negative values less than 63.");
        goto cleanup;
    }

    if ((tmpDpdPathdelayCfg.intActDelay < -255) || (tmpDpdPathdelayCfg.intActDelay > 0))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               tmpDpdPathdelayCfg.intActDelay,
                               "Invalid intActDelay is selected. Valid values are greater than -255 and less than 0.");
        goto cleanup;
    }

    if ((tmpDpdPathdelayCfg.intExtDelay < -255) || (tmpDpdPathdelayCfg.intExtDelay > 0))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               tmpDpdPathdelayCfg.intExtDelay,
                               "Invalid intExtDelay is selected. Valid values are greater than -255 and less than 0.");
        goto cleanup;
    }

    if (((dpdTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (dpdTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    tmpDpdPathdelayCfg.intActDelay  = ADRV904X_HTOCL(tmpDpdPathdelayCfg.intActDelay);
    tmpDpdPathdelayCfg.intExtDelay  = ADRV904X_HTOCL(tmpDpdPathdelayCfg.intExtDelay);
    tmpDpdPathdelayCfg.fracExtDelay = ADRV904X_HTOCL(tmpDpdPathdelayCfg.fracExtDelay);

    for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        uint32_t txChan = (1 << i);
        if (dpdTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               DPD_CTRL_PATHDELAY_CONFIG,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               (const uint8_t*)&tmpDpdPathdelayCfg,
                                                               sizeof(tmpDpdPathdelayCfg),
                                                               &lengthResp,
                                                               NULL,
                                                               0);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChan, "Failed to send request");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdResetPathdelayInit(adi_adrv904x_Device_t* const device,
                                                                    const uint32_t dpdTxChannelMask,
                                                                    const adi_adrv904x_DfeAppDpdResetMode_e dpdResetMode,
                                                                    const adi_adrv904x_DfeAppCalDpdPathDlyStatus_t* const dpdPathdelayCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdPathdelayCfg, cleanup);

    if (((dpdTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (dpdTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_DpdReset(device, dpdTxChannelMask, dpdResetMode);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dpdTxChannelMask, "Failed to send DpdReset request");
        goto cleanup;
    }
    else
    {
        recoveryAction = adi_adrv904x_DpdPathdelayCfg(device, dpdTxChannelMask, dpdPathdelayCfg);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dpdTxChannelMask, "Failed to send DpdPathdelayCfg request");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdPathdelaySeed(adi_adrv904x_Device_t* const                        device,
                                                               const uint32_t                                      dpdTxChannelMask,
                                                               const adi_adrv904x_DfeAppCalDpdPathDlySeed_t* const dpdPathdelay)
{
        const uint16_t DPD_CTRL_PATHDELAY_SEED = 10 | DPD_CTRL_SET_CMD_FLAG;
    adi_adrv904x_DfeAppCalDpdPathDlySeed_t tmpDpdPathdelay;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    uint32_t lengthResp = 0;
    uint32_t i = 0;
    int32_t qualrange_max = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dpdPathdelay, cleanup);

    ADI_LIBRARY_MEMCPY((void *)&tmpDpdPathdelay, dpdPathdelay, sizeof(adi_adrv904x_DfeAppCalDpdPathDlySeed_t));

    if ((tmpDpdPathdelay.seed.fracExtDelay > 63) || (tmpDpdPathdelay.seed.fracExtDelay < 0))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               tmpDpdPathdelay.seed.fracExtDelay,
                               "Invalid fracExtDelay is selected. Valid values are non-negative values less than 63.");
        goto cleanup;
    }

    if ((tmpDpdPathdelay.seed.intActDelay < -255) || (tmpDpdPathdelay.seed.intActDelay > 0))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               tmpDpdPathdelay.seed.intActDelay,
                               "Invalid intActDelay is selected. Valid values are greater than -255 and less than 0.");
        goto cleanup;
    }

    if ((tmpDpdPathdelay.seed.intExtDelay < -255) || (tmpDpdPathdelay.seed.intExtDelay > 0))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               tmpDpdPathdelay.seed.intExtDelay,
                               "Invalid intExtDelay is selected. Valid values are greater than -255 and less than 0.");
        goto cleanup;
    }

    qualrange_max = abs(tmpDpdPathdelay.seed.intExtDelay);
    if (abs(tmpDpdPathdelay.seed.intExtDelay + 255) < abs(tmpDpdPathdelay.seed.intExtDelay))
    {
        qualrange_max = abs(tmpDpdPathdelay.seed.intExtDelay + 255);
    }

    if ((tmpDpdPathdelay.qualifyRange > qualrange_max) || (tmpDpdPathdelay.qualifyRange < 0))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, tmpDpdPathdelay.qualifyRange, "Invalid qualifyRange is selected.  0 is the valid minimum value allowed");
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, qualrange_max, "Maximum qualrange_max allowed");

        goto cleanup;
    }

    if (((dpdTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (dpdTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    tmpDpdPathdelay.seed.intActDelay  = ADRV904X_HTOCL(tmpDpdPathdelay.seed.intActDelay);
    tmpDpdPathdelay.seed.intExtDelay  = ADRV904X_HTOCL(tmpDpdPathdelay.seed.intExtDelay);
    tmpDpdPathdelay.seed.fracExtDelay = ADRV904X_HTOCL(tmpDpdPathdelay.seed.fracExtDelay);
    tmpDpdPathdelay.optSearchEnable   = ADRV904X_HTOCL(tmpDpdPathdelay.optSearchEnable);
    tmpDpdPathdelay.qualifyRange      = ADRV904X_HTOCL(tmpDpdPathdelay.qualifyRange);

    for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        uint32_t txChan = (1 << i);
        if (dpdTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               DPD_CTRL_PATHDELAY_SEED,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               (const uint8_t*)&tmpDpdPathdelay,
                                                               sizeof(tmpDpdPathdelay),
                                                               &lengthResp,
                                                               NULL,
                                                               0);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChan, "Failed to send request");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdPathdelayResetSeed(adi_adrv904x_Device_t* const device,
                                                                    const uint32_t               dpdTxChannelMask)
{
        const uint16_t DPD_CTRL_PATHDELAY_SEED = 10;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if (((dpdTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (dpdTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            dpdTxChannelMask,
            "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        uint32_t txChan = (1 << i);
        if (dpdTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               DPD_CTRL_PATHDELAY_SEED,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               NULL,
                                                               0,
                                                               &lengthResp,
                                                               NULL,
                                                               0);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChan, "Failed to send request");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

#ifndef ADI_LIBRARY_RM_FLOATS
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Ctc2ModelConfigSet(adi_adrv904x_Device_t* const                      device,
                                                                 const uint32_t                                    txChannelMask,
                                                                 const adi_adrv904x_DfeAppCalCtc2ModelDesc_t* const modelDesc)
{
        const uint16_t CTC2_CTRL_MODEL_CONFIG = 11U;
    const uint16_t CTC2_CTRL_MODEL_CONFIG_SET = CTC2_CTRL_MODEL_CONFIG | DPD_CTRL_SET_CMD_FLAG;
    ADI_PLATFORM_LARGE_ARRAY_ALLOC(uint8_t, cpuCtrlData, sizeof(adi_adrv904x_DfeAppCalCtc2ModelDesc_t));
    uint32_t svcErr = 0U;
    adi_adrv904x_DfeAppCalCtc2ModelDesc_t *pTmpModelDesc = (adi_adrv904x_DfeAppCalCtc2ModelDesc_t *)(cpuCtrlData);

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0U;
    uint32_t i = 0U;
    uint16_t numOfFeatures = 0U;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuCtrlData, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, modelDesc, cleanup);

    if (((txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }
    
    ADI_LIBRARY_MEMCPY((void*)(cpuCtrlData), (const void * const)modelDesc, sizeof(adi_adrv904x_DfeAppCalCtc2ModelDesc_t));

    /* Assign to a uint16 so that we can range check here, in case partials implemented to allow more than 255 features */
    numOfFeatures = pTmpModelDesc->numOfFeats;
    if (numOfFeatures > ADI_ADRV904X_DFE_APP_CAL_CTC2_DPD_MAX_NUM_FEATURES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numOfFeatures, "adi_adrv904x_Ctc2ModelConfigSet failed. Max number of features exceeded ");
        goto cleanup;
    }
    /* Note that all the integer fields of *pTmpModelDesc and it's sub-strucutures are uint8_t so require no endianess
     * corrections. */

    pTmpModelDesc->ctcInputSel = ADRV904X_HTOCL(pTmpModelDesc->ctcInputSel);
    pTmpModelDesc->modelSel = ADRV904X_HTOCL(pTmpModelDesc->modelSel);
    for (i = 0U; i < ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_DEC; ++i)
    {
        pTmpModelDesc->decConfig[i].kLutAddrScale = ADRV904X_HTOCD(pTmpModelDesc->decConfig[i].kLutAddrScale);
        pTmpModelDesc->decConfig[i].absScale = ADRV904X_HTOCD(pTmpModelDesc->decConfig[i].absScale);
        pTmpModelDesc->decConfig[i].absOffset = ADRV904X_HTOCD(pTmpModelDesc->decConfig[i].absOffset);
    }
    
    for (i = 0U; i < ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW; ++i)
    {
        pTmpModelDesc->rowCfg[i].fLutAddrScale = ADRV904X_HTOCD(pTmpModelDesc->rowCfg[i].fLutAddrScale);
        pTmpModelDesc->rowCfg[i].tau = ADRV904X_HTOCD(pTmpModelDesc->rowCfg[i].tau);
        pTmpModelDesc->rowCfg[i].decSelect = ADRV904X_HTOCL(pTmpModelDesc->rowCfg[i].decSelect);
    }
    
    for (i = 0U; i < pTmpModelDesc->numOfFeats; ++i)
    {
        pTmpModelDesc->feature[i].a.real = ADRV904X_HTOCD(pTmpModelDesc->feature[i].a.real);
        pTmpModelDesc->feature[i].a.imag = ADRV904X_HTOCD(pTmpModelDesc->feature[i].a.imag);
    }

    for (i = 0U; i < ADI_ADRV904X_MAX_TXCHANNELS; ++i)
    {
        uint32_t txChan = (1 << i);
        if (txChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               CTC2_CTRL_MODEL_CONFIG_SET,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               cpuCtrlData,
                                                               sizeof(adi_adrv904x_DfeAppCalCtc2ModelDesc_t),
                                                               &lengthResp,
                                                               (uint8_t*)&svcErr,
                                                               sizeof(uint32_t));
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                svcErr = ADRV904X_CTOHL(svcErr);
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, svcErr, "Service error, check model configuration parameters");
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error when configuring CTC2 model");

                goto cleanup;
            }
        }
    }
    
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Ctc2ModelConfigGet(adi_adrv904x_Device_t* const                      device,
                                                                 const adi_adrv904x_TxChannels_e                   txChannelSel,
                                                                 adi_adrv904x_DfeAppCalCtc2ModelType_e             ctc2ModelType,
                                                                 adi_adrv904x_DfeAppCalCtc2ModelDesc_t* const      modelDesc)
{
            const uint16_t CTC2_CTRL_MODEL_CONFIG = 11U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;
    uint16_t numOfFeatures = 0U;
    uint32_t modelSel = ADRV904X_HTOCL((uint32_t)ctc2ModelType);

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, modelDesc, cleanup);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)txChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Tx channel parameter is invalid.");
        goto cleanup;
    }
    
    if (ctc2ModelType != ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_TYPE_0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ctc2ModelType, "Invalid CTC2 model type selected.");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                       ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                       CTC2_CTRL_MODEL_CONFIG,
                                                       (adi_adrv904x_Channels_e)txChannelSel,
                                                       (const uint8_t*)&modelSel,
                                                       sizeof(modelSel),
                                                       &lengthResp,
                                                       (uint8_t *)modelDesc,
                                                       sizeof(adi_adrv904x_DfeAppCalCtc2ModelDesc_t));

    if (recoveryAction == ADI_ADRV904X_ERR_ACT_NONE)
    {
        /* Assign to a uint16 so that we can range check here, in case partials implemented to allow more than 255 features */
        numOfFeatures = modelDesc->numOfFeats;
        if (numOfFeatures > ADI_ADRV904X_DFE_APP_CAL_CTC2_DPD_MAX_NUM_FEATURES)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numOfFeatures, "The number of features is greater than maximum allowed");
            goto cleanup;
        }
    }
    else
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DpdModelConfigDpdGet failed.");
        goto cleanup;
    }

    if (recoveryAction == ADI_ADRV904X_ERR_ACT_NONE)
    {
        modelDesc->ctcInputSel = ADRV904X_CTOHL(modelDesc->ctcInputSel);
        modelDesc->modelSel  = ADRV904X_CTOHL(modelDesc->modelSel);
        for (i = 0U; i < ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_DEC; ++i)
        {
            modelDesc->decConfig[i].kLutAddrScale = ADRV904X_CTOHD(modelDesc->decConfig[i].kLutAddrScale);
            modelDesc->decConfig[i].absScale = ADRV904X_CTOHD(modelDesc->decConfig[i].absScale);
            modelDesc->decConfig[i].absOffset = ADRV904X_CTOHD(modelDesc->decConfig[i].absOffset);
        }
    
        for (i = 0U; i < ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW; ++i)
        {
            modelDesc->rowCfg[i].fLutAddrScale = ADRV904X_CTOHD(modelDesc->rowCfg[i].fLutAddrScale);
            modelDesc->rowCfg[i].tau = ADRV904X_CTOHD(modelDesc->rowCfg[i].tau);
            modelDesc->rowCfg[i].decSelect = ADRV904X_CTOHL(modelDesc->rowCfg[i].decSelect);
        }
    
        for (i = 0U; i < ADI_ADRV904X_DFE_APP_CAL_CTC2_DPD_MAX_NUM_FEATURES; ++i)
        {
            modelDesc->feature[i].a.real = ADRV904X_CTOHD(modelDesc->feature[i].a.real);
            modelDesc->feature[i].a.imag = ADRV904X_CTOHD(modelDesc->feature[i].a.imag);
        }
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
    
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Ctc2ModelConfigSet_v2(adi_adrv904x_Device_t* const                          device,
                                                                    const uint32_t                                        txChannelMask,
                                                                    const adi_adrv904x_DfeAppCalCtc2ModelDesc_v2_t* const modelDesc)
{
        const uint16_t CTC2_CTRL_MODEL_CONFIG_V2 = 13U;
    const uint16_t CTC2_CTRL_MODEL_CONFIG_SET_V2 = CTC2_CTRL_MODEL_CONFIG_V2 | DPD_CTRL_SET_CMD_FLAG;
    uint8_t cpuCtrlData[ sizeof(adi_adrv904x_DfeAppCalCtc2ModelDesc_v2_t)];
    uint32_t svcErr = 0U;
    adi_adrv904x_DfeAppCalCtc2ModelDesc_v2_t *pTmpModelDesc = (adi_adrv904x_DfeAppCalCtc2ModelDesc_v2_t *)(cpuCtrlData);

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0U;
    uint32_t i = 0U;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, modelDesc, cleanup);

    if (((txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }
    
    ADI_LIBRARY_MEMCPY((void*)(cpuCtrlData), (const void * const)modelDesc, sizeof(adi_adrv904x_DfeAppCalCtc2ModelDesc_v2_t));        

    if (pTmpModelDesc->ctcInputSel >= ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_NUM_INPUTS)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               pTmpModelDesc->ctcInputSel,
                               "Invalid CTCM2 input select");
        goto cleanup;
    }

    if ((pTmpModelDesc->modelSel >= ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_COUNT) && (pTmpModelDesc->modelSel != ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_TYPE_HARDCODED))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               pTmpModelDesc->modelSel,
                               "Invalid model selection");
        goto cleanup;
    }
    
	/* ctcInputSel, modelSel are uint8_t */
    for (i = 0U; i < ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_DEC; ++i)
    {
        pTmpModelDesc->decConfig[i].kLutAddrScale = ADRV904X_HTOCD(pTmpModelDesc->decConfig[i].kLutAddrScale);
        /* decRatio is uint8_t */
    }
    
    for (i = 0U; i < ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW; ++i)
    {
        pTmpModelDesc->rowCfg[i].fLutAddrScale  = ADRV904X_HTOCD(pTmpModelDesc->rowCfg[i].fLutAddrScale);
        pTmpModelDesc->rowCfg[i].absScale       = ADRV904X_HTOCD(pTmpModelDesc->rowCfg[i].absScale);
        pTmpModelDesc->rowCfg[i].absOffset      = ADRV904X_HTOCD(pTmpModelDesc->rowCfg[i].absOffset);
        /* kLut is uint8 */
    }
    
	for (i = 0U; i < ADI_ADRV904X_DFE_APP_CAL_CTC2_DPD_MAX_NUM_TAU; ++i)
    {
        pTmpModelDesc->timeConstants[i].tau             = ADRV904X_HTOCD(pTmpModelDesc->timeConstants[i].tau);
	    /* modelStructure, decSelect are uint8_t */
    }    
    
    for (i = 0U; i < ADI_ADRV904X_MAX_TXCHANNELS; ++i)
    {
        uint32_t txChan = (1 << i);
        if (txChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               CTC2_CTRL_MODEL_CONFIG_SET_V2,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               cpuCtrlData,
                                                               sizeof(cpuCtrlData),
                                                               &lengthResp,
                                                               (uint8_t*)&svcErr,
                                                               sizeof(uint32_t));
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                svcErr = ADRV904X_CTOHL(svcErr);
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, svcErr, "Service error, check model configuration parameters");
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error when configuring CTC2 model");

                goto cleanup;
            }
        }
    }
    
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Ctc2ModelConfigGet_v2(adi_adrv904x_Device_t* const                     device,
                                                                    const adi_adrv904x_TxChannels_e                  txChannelSel,
                                                                    adi_adrv904x_DfeAppCalCtc2ModelType_e            ctc2ModelType,
                                                                    adi_adrv904x_DfeAppCalCtc2ModelDesc_v2_t* const  modelDesc)
{
            const uint16_t CTC2_CTRL_MODEL_CONFIG_V2 = 13U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;
    uint32_t modelSel = ADRV904X_HTOCL((uint32_t)ctc2ModelType);

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, modelDesc, cleanup);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)txChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Tx channel parameter is invalid.");
        goto cleanup;
    }
    
    if (ctc2ModelType != ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_TYPE_0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ctc2ModelType, "Invalid CTC2 model type selected.");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                       ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                       CTC2_CTRL_MODEL_CONFIG_V2,
                                                       (adi_adrv904x_Channels_e)txChannelSel,
                                                       (const uint8_t*)&modelSel,
                                                       sizeof(modelSel),
                                                       &lengthResp,
                                                       (uint8_t *)modelDesc,
                                                       sizeof(adi_adrv904x_DfeAppCalCtc2ModelDesc_v2_t));

    if (recoveryAction == ADI_ADRV904X_ERR_ACT_NONE)
    {
	    /* ctcInputSel, modelSel are uint8_t */
	    for (i = 0U; i < ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_DEC; ++i)
        {
            modelDesc->decConfig[i].kLutAddrScale = ADRV904X_CTOHD(modelDesc->decConfig[i].kLutAddrScale);
        }
    
        for (i = 0U; i < ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW; ++i)
        {
            modelDesc->rowCfg[i].fLutAddrScale  = ADRV904X_CTOHD(modelDesc->rowCfg[i].fLutAddrScale);
            modelDesc->rowCfg[i].absScale       = ADRV904X_HTOCD(modelDesc->rowCfg[i].absScale);
            modelDesc->rowCfg[i].absOffset      = ADRV904X_HTOCD(modelDesc->rowCfg[i].absOffset);
            /* kPower is uint8 */
        }
    
        for (i = 0U; i < ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_DEC; ++i)
        {
            modelDesc->timeConstants[i].tau             = ADRV904X_HTOCD(modelDesc->timeConstants[i].tau);
	        /* decRatio, modelStructure, decSelect are uint8_t */
	}    
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
    
}
#endif /* ADI_LIBRARY_RM_FLOATS */

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdGainSet(adi_adrv904x_Device_t* const                    device,
                                                         const uint32_t                                  txChannelMask,
                                                         const adi_adrv904x_DfeAppCalDpdGainCfg_t* const gainCfg)
{
        const uint16_t DPD_CTRL_DPD_GAIN_CONFIG = 15U;
    const uint16_t DPD_CTRL_DPD_GAIN_CONFIG_SET = DPD_CTRL_DPD_GAIN_CONFIG | DPD_CTRL_SET_CMD_FLAG;
    adi_adrv904x_DfeAppCalDpdGainCfg_t gainCfgTmp;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0U;
    uint32_t i = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gainCfg, cleanup);

    if (((txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    for (i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS; ++i)
    {
        if (gainCfg->lutResolution[i] > 2u)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                    recoveryAction,
                                    gainCfg->lutResolution[i],
                                    "Invalid lutResolution. Valid value is <= 2");
            goto cleanup;
        }
        /* Min Q2.6 (0x01) linear values converts into -36.124dB, max (0xFF) converts into 12.007 */
        if ((gainCfg->magGain_mdB[i] < -36124) || (gainCfg->magGain_mdB[i] > 12007))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                    recoveryAction,
                                    gainCfg->magGain_mdB[i],
                                    "Invalid magGain_mdB. Valid value is -36124 to 12007");
            goto cleanup;
        }
    }

    /* Min Q2.6 (0x01) linear values converts into -36.124dB, max (0xFF) converts into 12.007 */
    if ((gainCfg->postCfrGain_mdB < -36124) || (gainCfg->postCfrGain_mdB > 12007))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               gainCfg->postCfrGain_mdB,
                               "Invalid postCfrGain_mdB. Valid value is -32768 to 12007");
        goto cleanup;
    }
    /* Min Q0.12 (0x001) linear values converts into -72.247dB, max (0xFFF) is -0.002dB. 0Db means feature is disabled */
    if ((gainCfg->postDpdGain_mdB != 0) && ((gainCfg->postDpdGain_mdB < -72247) || (gainCfg->postDpdGain_mdB > -2)))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               gainCfg->postDpdGain_mdB,
                               "Invalid postDpdGain_mdB. Valid value is -72247 to -2, or 0");
        goto cleanup;
    }

    /* Handle endianness */
    ADI_LIBRARY_MEMCPY(&gainCfgTmp, gainCfg, sizeof(gainCfgTmp));
    for (i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS; ++i)
    {
        gainCfgTmp.magGain_mdB[i] = ADRV904X_HTOCL(gainCfgTmp.magGain_mdB[i]);
    }
    gainCfgTmp.postCfrGain_mdB = ADRV904X_HTOCL(gainCfgTmp.postCfrGain_mdB);
    gainCfgTmp.postDpdGain_mdB = ADRV904X_HTOCL(gainCfgTmp.postDpdGain_mdB);

    for (i = 0U; i < ADI_ADRV904X_MAX_TXCHANNELS; ++i)
    {
        uint32_t txChan = (1 << i);
        if (txChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               DPD_CTRL_DPD_GAIN_CONFIG_SET,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               (const uint8_t *)&gainCfgTmp,
                                                               sizeof(gainCfgTmp),
                                                               &lengthResp,
                                                               NULL,
                                                               0);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error in DPD GAIN CONFIG set command");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdGainGet(adi_adrv904x_Device_t* const              device,
                                                         const adi_adrv904x_TxChannels_e           txChannelSel,
                                                         adi_adrv904x_DfeAppCalDpdGainCfg_t* const gainCfg)
{
        const uint16_t DPD_CTRL_DPD_GAIN_CONFIG = 15U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gainCfg, cleanup);

    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)txChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Tx channel parameter is invalid.");
        goto cleanup;
    }

    /* DFE command to get magGain and lutResolution arrays */
    recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                       ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                       DPD_CTRL_DPD_GAIN_CONFIG,
                                                       (adi_adrv904x_Channels_e)txChannelSel,
                                                       NULL,
                                                       0,
                                                       &lengthResp,
                                                       (uint8_t *)gainCfg,
                                                       sizeof(*gainCfg));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error in DPD GAIN CONFIG get command");
        goto cleanup;
    }

    /* Handle endianness */
    for (i = 0; i < ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS; ++i)
    {
        gainCfg->magGain_mdB[i] = ADRV904X_CTOHL(gainCfg->magGain_mdB[i]);
    }
    gainCfg->postCfrGain_mdB = ADRV904X_CTOHL(gainCfg->postCfrGain_mdB);
    gainCfg->postDpdGain_mdB = ADRV904X_CTOHL(gainCfg->postDpdGain_mdB);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

