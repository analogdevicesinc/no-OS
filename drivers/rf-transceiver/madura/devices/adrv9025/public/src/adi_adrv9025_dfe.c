// SPDX-License-Identifier: GPL-2.0
/**
* \file adi_adrv9025_dfe.c
* \brief Contains DPD features related function implementation defined in
* adi_adrv9025_dpd.h
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/


#include "adi_adrv9025_user.h"
#include "adi_adrv9025_dfe.h"
#include "adi_adrv9025_dfe_types.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025.h"
#include "adi_adrv9025_cpu.h"
#include "adi_adrv9025_cals.h"
#include "../../private/include/adrv9025_dfe.h"
#include "../../private/include/adrv9025_init.h"
#include "../../private/include/adrv9025_cpu.h"
#include "../../private/include/adrv9025_tx.h"
#include "../../private/include/adrv9025_cpu_macros.h"
#include "../../private/include/adrv9025_reg_addr_macros.h"
#include "../../private/include/adrv9025_bf_tx.h"

#ifdef _RELEASE_BUILD_
    #line __LINE__ "adi_adrv9025_dfe.c"
#endif

int32_t adi_adrv9025_DpdStatusGet(adi_adrv9025_Device_t*    device,
                                  adi_adrv9025_TxChannels_e txChannel,
                                  adi_adrv9025_DpdStatus_t* dpdStatus)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;
    static const uint8_t ARM_ERR_CODE = 0x0E;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[3] = { ADRV9025_CPU_OBJECTID_CAL_STATUS, ADRV9025_CPU_OBJECTID_DPD_TRACKING, 0 };
    uint8_t armData[112] = { 0 };
    uint8_t cmdStatusByte = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        dpdStatus);

    switch (txChannel)
    {
        case ADI_ADRV9025_TX1:
        {
            extData[2] = 0;    /*!< Tx1 channel enabled */
            break;
        }
        case ADI_ADRV9025_TX2:
        {
            extData[2] = 1;    /*!< Tx2 channel enabled */
            break;
        }
        case ADI_ADRV9025_TX3:
        {
            extData[2] = 2;    /*!< Tx3 channel enabled */
            break;
        }
        case ADI_ADRV9025_TX4:
        {
            extData[2] = 3;    /*!< Tx4 channel enabled */
            break;
        }
        default:
        {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                             "Invalid Tx Channel requested to read back DPD status");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /* Executing DPD tracking cal status get Arm Command */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     extData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
        ADI_ADRV9025_CPU_TYPE_C,
        (uint8_t)ADRV9025_CPU_GET_OPCODE,
        &cmdStatusByte,
        (uint32_t)ADI_ADRV9025_GETTXDPDSTATUS_TIMEOUT_US,
        (uint32_t)ADI_ADRV9025_GETTXDPDSTATUS_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
            ADI_ADRV9025_CPU_TYPE_C,
            ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                             extData[0],
                                                             cmdStatusByte),
            recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     cmdStatusByte,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read the DPD status from ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                                             &armData[0],
                                             sizeof(armData),
                                             ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing DPD Status into the structure. */
    adrv9025_DpdStatusDeserialize(device,
                                  dpdStatus,
                                  &armData[0],
                                  sizeof(armData));

    return recoveryAction;
}

int32_t adi_adrv9025_DpdReset(adi_adrv9025_Device_t*      device,
                              adi_adrv9025_TxChannels_e   txChannel,
                              adi_adrv9025_DpdResetMode_e dpdResetMode)
{

    static const uint8_t ARM_ERR_CODE = 0x0E;

    uint8_t DPD_RESET_CTRL_PARAM= 0;
    uint8_t armData[1] = { 0 };

    if (dpdResetMode == ADI_ADRV9025_DPD_COEFF_SAVE)
    {
        DPD_RESET_CTRL_PARAM = 0x01;
        armData[0] = 0; /* Mtable */
    }
    else if (dpdResetMode == ADI_ADRV9025_DPD_COEFF_SAVE_C_TABLE)
    {
        DPD_RESET_CTRL_PARAM = 0x01;
        armData[0] = 1; /* Ctable */
    }
    else if (dpdResetMode == ADI_ADRV9025_DPD_COEFF_SAVE_R_TABLE)
    {
        DPD_RESET_CTRL_PARAM = 0x01;
        armData[0] = 2; /* Rtable */
    }
    else
    {
        DPD_RESET_CTRL_PARAM = 0x11;
        armData[0] = (uint8_t)dpdResetMode;
    }

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4] = { ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_DPD_TRACKING, (uint8_t)txChannel, DPD_RESET_CTRL_PARAM };

    uint8_t cmdStatusByte = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    /* Return an error if the requested Tx channel is invalid */
    if ((txChannel != ADI_ADRV9025_TX1) &&
        (txChannel != ADI_ADRV9025_TX2) &&
        (txChannel != ADI_ADRV9025_TX3) &&
        (txChannel != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid Tx Channel requested to reset DPD engine");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Return an error if the requested DPD reset mode is invalid */
    if ((dpdResetMode != ADI_ADRV9025_DPD_RESET_FULL) &&
        (dpdResetMode != ADI_ADRV9025_DPD_LUT_RESTORE) &&
        (dpdResetMode != ADI_ADRV9025_DPD_COEFF_SAVE) &&
        (dpdResetMode != ADI_ADRV9025_DPD_LUT_RESTORE_C_TABLE) &&
        (dpdResetMode != ADI_ADRV9025_DPD_COEFF_SAVE_C_TABLE) &&
        (dpdResetMode != ADI_ADRV9025_DPD_LUT_RESTORE_R_TABLE) &&
        (dpdResetMode != ADI_ADRV9025_DPD_COEFF_SAVE_R_TABLE))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdResetMode,
                         "Invalid DPD reset mode encountered while attempting to reset DPD");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Write the reset mode config to ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                                              &armData[0],
                                              sizeof(armData),
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Executing DPD reset Arm Command */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     extData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_SETDPDRESET_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_SETDPDRESET_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                             extData[0],
                                                             cmdStatusByte),
                                    recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     cmdStatusByte,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DpdModelConfigSet(adi_adrv9025_Device_t*         device,
                                       adi_adrv9025_DpdModelConfig_t* dpdModelConfig)
{
    static const uint8_t DPD_MODE_NUM_PER_CHUNK = 20u;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t armConfigData[ADI_ADRV9025_NUM_BYTES_DPD_MODEL_CONFIG] = { 0 };
    uint8_t dpdFeatureIndex = 0;
    uint16_t dpdModelMemOffset = 0;
    uint8_t chunkNum = 0;
    uint8_t remainNum = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        dpdModelConfig);

#if ADI_ADRV9025_DPD_RANGE_CHECK > 0
    adrv9025_DpdModelConfigSetRangeCheck(device,
                                         dpdModelConfig);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /* Encode the DPD model config into a format expected by the ADRV9025 firmware */
    recoveryAction = adrv9025_DpdModelConfigEncode(device,
                                                   dpdModelConfig,
                                                   &armConfigData[0],
                                                   sizeof(armConfigData));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armConfigData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Write the DPD model index, no. of features, compander size and the bank index to the ARM mailbox */
    recoveryAction = adi_adrv9025_CpuConfigWrite(device,
                                                 ADI_ADRV9025_CPU_TYPE_C,
                                                 ADRV9025_CPU_OBJECTID_DPD_TRACKING,
                                                 0,
                                                 &armConfigData[0],
                                                 ADI_ADRV9025_NUM_BYTES_DPD_MODEL_METADATA);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armConfigData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Program each feature into the ARM memory */
    chunkNum = dpdModelConfig->dpdNumFeatures / DPD_MODE_NUM_PER_CHUNK;
    remainNum = dpdModelConfig->dpdNumFeatures % DPD_MODE_NUM_PER_CHUNK;

    for (dpdFeatureIndex = 0; dpdFeatureIndex < chunkNum; dpdFeatureIndex++)
    {
        dpdModelMemOffset = ADI_ADRV9025_NUM_BYTES_DPD_MODEL_METADATA + (dpdFeatureIndex * ADI_ADRV9025_NUM_BYTES_PER_FEATURE * DPD_MODE_NUM_PER_CHUNK);
        recoveryAction    = adi_adrv9025_CpuConfigWrite(device,
                                                        ADI_ADRV9025_CPU_TYPE_C,
                                                        ADRV9025_CPU_OBJECTID_DPD_TRACKING,
                                                        dpdModelMemOffset,
                                                        &armConfigData[dpdModelMemOffset],
                                                        ADI_ADRV9025_NUM_BYTES_PER_FEATURE * DPD_MODE_NUM_PER_CHUNK);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         armConfigData,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (remainNum != 0)
    {
        dpdModelMemOffset = ADI_ADRV9025_NUM_BYTES_DPD_MODEL_METADATA + (chunkNum * ADI_ADRV9025_NUM_BYTES_PER_FEATURE * DPD_MODE_NUM_PER_CHUNK);
        recoveryAction    = adi_adrv9025_CpuConfigWrite(device,
                                                        ADI_ADRV9025_CPU_TYPE_C,
                                                        ADRV9025_CPU_OBJECTID_DPD_TRACKING,
                                                        dpdModelMemOffset,
                                                        &armConfigData[dpdModelMemOffset],
                                                        ADI_ADRV9025_NUM_BYTES_PER_FEATURE * remainNum);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         armConfigData,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DpdModelConfigGet(adi_adrv9025_Device_t*         device,
                                       adi_adrv9025_TxChannels_e      txChannel,
                                       adi_adrv9025_DpdModelConfig_t* dpdModelConfig)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t armConfigData[ADI_ADRV9025_NUM_BYTES_DPD_MODEL_CONFIG] = { 0 };
    uint8_t dpdFeatureIndex = 0;
    uint16_t dpdModelMemOffset = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        dpdModelConfig);

    /* Read the DPD model index, no. of features, compander size and the bank index from the ARM mailbox */
    recoveryAction = adi_adrv9025_CpuConfigRead(device,
                                                ADI_ADRV9025_CPU_TYPE_C,
                                                ADRV9025_CPU_OBJECTID_DPD_TRACKING,
                                                0,
                                                &armConfigData[0],
                                                ADI_ADRV9025_NUM_BYTES_DPD_MODEL_METADATA);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armConfigData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read each feature from the ARM memory and store it in a local buffer for parsing */
    for (dpdFeatureIndex = 0; dpdFeatureIndex < ADI_ADRV9025_MAX_DPD_FEATURES; dpdFeatureIndex++)
    {
        dpdModelMemOffset = ADI_ADRV9025_NUM_BYTES_DPD_MODEL_METADATA + (dpdFeatureIndex * ADI_ADRV9025_NUM_BYTES_PER_FEATURE);
        recoveryAction    = adi_adrv9025_CpuConfigRead(device,
                                                       ADI_ADRV9025_CPU_TYPE_C,
                                                       ADRV9025_CPU_OBJECTID_DPD_TRACKING,
                                                       dpdModelMemOffset,
                                                       &armConfigData[dpdModelMemOffset],
                                                       ADI_ADRV9025_NUM_BYTES_PER_FEATURE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         armConfigData,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Decode the serialized DPD model config read from the device */
    recoveryAction = adrv9025_DpdModelConfigDecode(device,
                                                   &armConfigData[0],
                                                   sizeof(armConfigData),
                                                   dpdModelConfig);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armConfigData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adi_adrv9025_DpdActuatorGainMonitorConfigSet(adi_adrv9025_Device_t*                  device,
                                                     adi_adrv9025_DpdActGainMonitorConfig_t* dpdActGainMonitorConfig)
{
    static const uint8_t DPD_SET_GAIN_MONITOR_CFG_CTRL_PARAM = 0x15;
    static const uint8_t DPD_GAIN_MON_CONFIG_LEN = 12;
    static const uint8_t ARM_ERR_CODE = 0x0E;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t              extData[4]                          = {
        ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL,
        ADRV9025_CPU_OBJECTID_DPD_TRACKING,
        0,
        DPD_SET_GAIN_MONITOR_CFG_CTRL_PARAM
    };
    uint8_t armData[12u + 2u] = { 0 };
    uint8_t cmdStatusByte = 0;
    uint8_t i = 0;
    uint8_t iirBypass = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        dpdActGainMonitorConfig);

#if ADI_ADRV9025_DPD_RANGE_CHECK > 0
    recoveryAction = adrv9025_DpdActuatorGainMonitorConfigRangeCheck(device,
                                                                     dpdActGainMonitorConfig);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    if (dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorIIREnable == 1)
    {
        iirBypass = 0;
    }
    else
    {
        iirBypass = 1;
    }

    armData[0] = 0u; /* offset within the structure to be set */
    armData[1] = DPD_GAIN_MON_CONFIG_LEN; /* number of bytes within the structures to be set */
    armData[2] = (uint8_t)(dpdActGainMonitorConfig->dpdGainMonitorThresh.dpdGainMonitorQualThresh & 0xFF);
    armData[3] = (uint8_t)((dpdActGainMonitorConfig->dpdGainMonitorThresh.dpdGainMonitorQualThresh >> 8) & 0xFF);
    armData[4] = dpdActGainMonitorConfig->dpdGainMonitorThresh.dpdGainMonitorLowerThresh;
    armData[5] = dpdActGainMonitorConfig->dpdGainMonitorThresh.dpdGainMonitorUpperThresh;
    armData[6] = dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorEnable;
    armData[7] = dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorIIREnable;
    armData[8] = iirBypass;
    armData[9] = (uint8_t)dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorLowerThreshModelSel;
    armData[10] = dpdActGainMonitorConfig->dpdGainMonitorCtrl.lowGainModelAutoLoadEnable;
    armData[11] = (uint8_t)dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorUpperThreshModelSel;
    armData[12] = dpdActGainMonitorConfig->dpdGainMonitorCtrl.highGainModelAutoLoadEnable;
    armData[13] = dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorIIRDecay;

    for (i = 0; i < ADRV9025_NUMBER_OF_TX_CHANNELS; i++)
    {
        if ((dpdActGainMonitorConfig->txChannelMask & (0x1 << i)) != 0)
        {
            extData[2] = 0x1 << i;

            /* Write the tracking config to ARM mailbox */
            recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                                      (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                                                      &armData[0],
                                                      sizeof(armData),
                                                      ADI_ADRV9025_CPU_MEM_AUTO_INCR);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             armData,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Executing tracking config set Arm Command */
            recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                                      ADI_ADRV9025_CPU_TYPE_C,
                                                      (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                      &extData[0],
                                                      sizeof(extData));
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             extData,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Wait for command to finish executing */
            recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                           ADI_ADRV9025_CPU_TYPE_C,
                                                           (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                           &cmdStatusByte,
                                                           (uint32_t)ADI_ADRV9025_SETDPDTRACKCONFIG_TIMEOUT_US,
                                                           (uint32_t)ADI_ADRV9025_SETDPDTRACKCONFIG_INTERVAL_US);

            /* If cmdStatusByte is non-zero then flag an ARM error */
            if ((cmdStatusByte & ARM_ERR_CODE) > 0)
            {
                adrv9025_CpuCmdErrorHandler(device,
                    ADI_ADRV9025_CPU_TYPE_C,
                    ADI_ADRV9025_SRC_CPUCMD,
                                            ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                                     extData[0],
                                                                     cmdStatusByte),
                    recoveryAction);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             cmdStatusByte,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adi_adrv9025_DpdActuatorGainMonitorConfigGet(adi_adrv9025_Device_t*                  device,
                                                     adi_adrv9025_TxChannels_e               txChannel,
                                                     adi_adrv9025_DpdActGainMonitorConfig_t* dpdActGainMonitorConfig)
{
    static const uint8_t DPD_GET_GAIN_MONITOR_CFG_CTRL_PARAM = 0x05;
    static const uint8_t ARM_ERR_CODE = 0x0E;
    static const uint8_t DPD_GAIN_MONITOR_CONFIG_LEN = 12u;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4]     = {
        ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL,
        ADRV9025_CPU_OBJECTID_DPD_TRACKING,
        (uint8_t)txChannel,
        DPD_GET_GAIN_MONITOR_CFG_CTRL_PARAM
    };
    uint8_t armData[12u] = { 0 };
    uint8_t cmdStatusByte = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        dpdActGainMonitorConfig);

    /* Check that Tx channel selection is valid */
    if ((txChannel != ADI_ADRV9025_TX1) &&
        (txChannel != ADI_ADRV9025_TX2) &&
        (txChannel != ADI_ADRV9025_TX3) &&
        (txChannel != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
            "Invalid Tx channel select, Valid selections Tx1/2/3/4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    armData[0] = 0;
    armData[1] = DPD_GAIN_MONITOR_CONFIG_LEN;

    /* Write offset and size of tracking config to get to ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                                              &armData[0],
                                              2u,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Executing tracking config set Arm Command */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     extData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_GETDPDTRACKCONFIG_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETDPDTRACKCONFIG_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
            ADI_ADRV9025_CPU_TYPE_C,
            ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                             extData[0],
                                                             cmdStatusByte),
            recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     cmdStatusByte,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read the tracking config from ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                                             &armData[0],
                                             sizeof(armData),
                                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    dpdActGainMonitorConfig->dpdGainMonitorThresh.dpdGainMonitorQualThresh = (uint16_t)((uint16_t)armData[0] | ((uint16_t)armData[1] << 8));
    dpdActGainMonitorConfig->dpdGainMonitorThresh.dpdGainMonitorLowerThresh = armData[2];
    dpdActGainMonitorConfig->dpdGainMonitorThresh.dpdGainMonitorUpperThresh = armData[3];
    dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorEnable = armData[4];
    dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorIIREnable = armData[5];
    dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorLowerThreshModelSel = (adi_adrv9025_DpdModelSel_e)armData[7];
    dpdActGainMonitorConfig->dpdGainMonitorCtrl.lowGainModelAutoLoadEnable = armData[8];
    dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorUpperThreshModelSel = (adi_adrv9025_DpdModelSel_e)armData[9];
    dpdActGainMonitorConfig->dpdGainMonitorCtrl.highGainModelAutoLoadEnable = armData[10];
    dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorIIRDecay = armData[11];
    dpdActGainMonitorConfig->txChannelMask = (adi_adrv9025_TxChannels_e)txChannel;
    return recoveryAction;
}

int32_t adi_adrv9025_DpdTrackingConfigSet(adi_adrv9025_Device_t*            device,
                                          adi_adrv9025_DpdTrackingConfig_t* dpdTrackingConfig)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, dpdTrackingConfig);

    /* call private function with normal dpd */
    recoveryAction = adrv9025_dpdTrackingConfigSet(device, dpdTrackingConfig, NULL, ADI_FALSE);

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adi_adrv9025_DpdTrackingConfigGet(adi_adrv9025_Device_t*            device,
                                          adi_adrv9025_TxChannels_e         txChannel,
                                          adi_adrv9025_DpdTrackingConfig_t* dpdTrackingConfig)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, dpdTrackingConfig);

    recoveryAction = adrv9025_dpdTrackingConfigGet(device,txChannel, dpdTrackingConfig, NULL, ADI_FALSE);

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adi_adrv9025_EnhancedDpdTrackingConfigSet(adi_adrv9025_Device_t*            device,
                                                  adi_adrv9025_EnhancedDpdTrackingConfig_t* dpdTrackingConfig)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, dpdTrackingConfig);

    /* call private function with enhanced dpd */
    recoveryAction = adrv9025_dpdTrackingConfigSet(device, NULL, dpdTrackingConfig, ADI_TRUE);

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adi_adrv9025_EnhancedDpdTrackingConfigGet(adi_adrv9025_Device_t*            device,
                                                  adi_adrv9025_TxChannels_e         txChannel,
                                                  adi_adrv9025_EnhancedDpdTrackingConfig_t* dpdTrackingConfig)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, dpdTrackingConfig);
    /* call private function with enhanced dpd */
    recoveryAction = adrv9025_dpdTrackingConfigGet(device, txChannel, NULL, dpdTrackingConfig, ADI_TRUE);

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adi_adrv9025_DpdFaultConditionSet(adi_adrv9025_Device_t*           device,
                                          uint32_t                         txChannelMask,
                                          adi_adrv9025_DpdFaultCondition_t dpdFaultCond[],
                                          uint8_t                          numOfCond)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t i = 0u;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        dpdFaultCond);

    for (i = 0u; i < numOfCond; i++)
    {
        recoveryAction = adrv9025_DpdFaultConditionSet(device,
                                                       txChannelMask,
                                                       &dpdFaultCond[i]);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         dpdFaultCond[i],
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adi_adrv9025_DpdFaultConditionGet(adi_adrv9025_Device_t*            device,
                                          adi_adrv9025_TxChannels_e         txChannel,
                                          adi_adrv9025_DpdMetric_e          dpdMetric,
                                          adi_adrv9025_DpdFaultCondition_t* dpdFaultCond)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        dpdFaultCond);

#if ADI_ADRV9025_DPD_RANGE_CHECK > 0
    /* Check that the target Tx channel is valid */
    if (txChannel != ADI_ADRV9025_TX1 &&
        txChannel != ADI_ADRV9025_TX2 &&
        txChannel != ADI_ADRV9025_TX3 &&
        txChannel != ADI_ADRV9025_TX4)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            txChannel,
            "Invalid Tx channel mask encountered while attempting to get DPD Fault Condition.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdMetric != ADI_ADRV9025_DPD_METRIC_MEAN_TU_POWER &&
        dpdMetric != ADI_ADRV9025_DPD_METRIC_PEAK_TU_POWER &&
        dpdMetric != ADI_ADRV9025_DPD_METRIC_MEAN_TX_POWER &&
        dpdMetric != ADI_ADRV9025_DPD_METRIC_PEAK_TX_POWER &&
        dpdMetric != ADI_ADRV9025_DPD_METRIC_MEAN_ORX_POWER &&
        dpdMetric != ADI_ADRV9025_DPD_METRIC_PEAK_ORX_POWER &&
        dpdMetric != ADI_ADRV9025_DPD_METRIC_DIRECT_EVM &&
        dpdMetric != ADI_ADRV9025_DPD_METRIC_INDIRECT_EVM &&
        dpdMetric != ADI_ADRV9025_DPD_METRIC_SELECT_ERROR &&
        dpdMetric != ADI_ADRV9025_DPD_METRIC_INDIRECT_ERROR)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            dpdMetric,
            "Invalid DPD metric");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif

    recoveryAction = adrv9025_DpdFaultConditionGet(device,
                                                   txChannel,
                                                   dpdMetric,
                                                   dpdFaultCond);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     dpdFaultCond,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adi_adrv9025_DpdRecoveryActionSet(adi_adrv9025_Device_t*                 device,
                                          uint32_t                               txChannelMask,
                                          adi_adrv9025_DpdRecoveryActionConfig_t recoveryActionConfig[],
                                          uint8_t                                numOfConfig)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t i = 0u;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        recoveryActionConfig);

    for (i = 0u; i < numOfConfig; i++)
    {
        recoveryAction = adrv9025_DpdRecoveryActionSet(device,
                                                       txChannelMask,
                                                       &recoveryActionConfig[i]);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         recoveryActionConfig[i],
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adi_adrv9025_DpdRecoveryActionGet(adi_adrv9025_Device_t*                  device,
                                          adi_adrv9025_TxChannels_e               txChannel,
                                          adi_adrv9025_DpdErrorState_e            dpdErrorState,
                                          adi_adrv9025_DpdRecoveryActionConfig_t* recoveryActionConfig)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        recoveryActionConfig);

#if ADI_ADRV9025_DPD_RANGE_CHECK > 0
    /* Check that the target Tx channel is valid */
    if (txChannel != ADI_ADRV9025_TX1 &&
        txChannel != ADI_ADRV9025_TX2 &&
        txChannel != ADI_ADRV9025_TX3 &&
        txChannel != ADI_ADRV9025_TX4)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            txChannel,
            "Invalid Tx channel mask encountered while attempting to get DPD Recovery Action.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdErrorState != ADI_ADRV9025_DPD_ERR_STATE_0 &&
        dpdErrorState != ADI_ADRV9025_DPD_ERR_STATE_1 &&
        dpdErrorState != ADI_ADRV9025_DPD_ERR_STATE_PERSISTENT_0 &&
        dpdErrorState != ADI_ADRV9025_DPD_ERR_STATE_PERSISTENT_1)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            dpdErrorState,
            "Invalid Error state");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif

    recoveryAction = adrv9025_DpdRecoveryActionGet(device,
                                                   txChannel,
                                                   dpdErrorState,
                                                   recoveryActionConfig);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     recoveryActionConfig,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adi_adrv9025_CfrThresholdsRunTimeUpdate(adi_adrv9025_Device_t* device,
                                                uint32_t               txChannelMask)
{
    static const uint8_t CFR_CTRL_UPDATE_THRESHOLD_ID = 18;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t dummyByte = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    recoveryAction = adrv9025_CfrCtrlCmdExecute(device,
                                                txChannelMask,
                                                CFR_CTRL_UPDATE_THRESHOLD_ID,
                                                &dummyByte,
                                                (uint8_t)1);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     dummyByte,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adi_adrv9025_CfrCoefficientsRunTimeUpdate(adi_adrv9025_Device_t *device,
                                                  uint32_t               txChannelMask)
{
    static const uint8_t CFR_CTRL_UPDATE_COEFFICIENTS_ID = 19;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t dummyByte = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    recoveryAction = adrv9025_CfrCtrlCmdExecute(device,
                                                txChannelMask,
                                                CFR_CTRL_UPDATE_COEFFICIENTS_ID,
                                                &dummyByte,
                                                (uint8_t)1);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     dummyByte,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adi_adrv9025_CfrHardClipperConfigSet(adi_adrv9025_Device_t*              device,
                                             adi_adrv9025_CfrHardClipperConfig_t cfrHardClipperConfig[],
                                             uint8_t                             cfrHardClipperCfgArraySize)
{
    static const uint8_t CFR_CTRL_HCLIP_ENABLE_ID = 0x06;
    static const uint8_t CFR_HARD_CLIP_ENABLE = 0x01;
    static const uint8_t CFR_HARD_CLIP_DISABLE = 0x00;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t armData[5] = { 0 };
    uint8_t configIndex = 0;
    uint8_t *fpSerializerPtr = NULL;
    float f;
    uint32_t* temp;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        cfrHardClipperConfig);

#if ADI_ADRV9025_CFR_RANGE_CHECK > 0
    adrv9025_CfrHardClipperConfigSetRangeCheck(device,
                                               cfrHardClipperConfig,
                                               cfrHardClipperCfgArraySize);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    for (configIndex = 0; configIndex < cfrHardClipperCfgArraySize; configIndex++)
    {
        /* Serialize single precision floating point normalized hard clipper threshold */

        f = cfrHardClipperConfig[configIndex].cfrHardClipperThreshold;
        temp = (uint32_t*) &f;
        *temp = HTOCL(*temp);

        fpSerializerPtr = (uint8_t*)&f;
        armData[0] = fpSerializerPtr[0];
        armData[1] = fpSerializerPtr[1];
        armData[2] = fpSerializerPtr[2];
        armData[3] = fpSerializerPtr[3];

        /* Tag CFR Hard clipper enable / disable data to the ARM config data */
        armData[4] = (cfrHardClipperConfig[configIndex].cfrHardClipperEnable > 0) ? CFR_HARD_CLIP_ENABLE : CFR_HARD_CLIP_DISABLE;

        /* Execute CFR Ctrl command to setup hard clipper */
        recoveryAction = adrv9025_CfrCtrlCmdExecute(device,
                                                    cfrHardClipperConfig[configIndex].txChannelMask,
                                                    CFR_CTRL_HCLIP_ENABLE_ID,
                                                    &armData[0],
                                                    sizeof(armData));
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         armData,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adi_adrv9025_CfrHardClipperConfigGet(adi_adrv9025_Device_t*               device,
                                             adi_adrv9025_TxChannels_e            txChannel,
                                             adi_adrv9025_CfrHardClipperConfig_t* cfrHardClipperConfig)
{
    static const uint32_t HCLIP_THRESH_OFFSET = 60;
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;
    static const uint8_t ARM_ERR_CODE = 0x0E;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[3] = { ADRV9025_CPU_OBJECTID_CAL_STATUS, ADRV9025_CPU_OBJECTID_CFR_INIT, 0 };
    uint8_t armData[5] = { 0 };
    uint8_t cmdStatusByte = 0;
    uint32_t fpDeserializerTemp = 0;
    float* fpDeserializerPtr = NULL;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        cfrHardClipperConfig);

    switch (txChannel)
    {
    case ADI_ADRV9025_TX1:
        {
            extData[2] = 0; /*!< Tx1 channel enabled */
            break;
        }
    case ADI_ADRV9025_TX2:
        {
            extData[2] = 1; /*!< Tx2 channel enabled */
            break;
        }
    case ADI_ADRV9025_TX3:
        {
            extData[2] = 2; /*!< Tx3 channel enabled */
            break;
        }
    case ADI_ADRV9025_TX4:
        {
            extData[2] = 3; /*!< Tx4 channel enabled */
            break;
        }
    default:
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                txChannel,
                "Invalid Tx Channel requested to read back CFR status");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /* Executing CFR status get Arm Command */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     extData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
        ADI_ADRV9025_CPU_TYPE_C,
        (uint8_t)ADRV9025_CPU_GET_OPCODE,
        &cmdStatusByte,
        (uint32_t)ADI_ADRV9025_GETTXCFRSTATUS_TIMEOUT_US,
        (uint32_t)ADI_ADRV9025_GETTXCFRSTATUS_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
            ADI_ADRV9025_CPU_TYPE_C,
            ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                             extData[0],
                                                             cmdStatusByte),
            recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     cmdStatusByte,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read the CFR status from ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)(ADRV9025_CPU_C_ADDR_MAILBOX_GET + HCLIP_THRESH_OFFSET),
                                             &armData[0],
                                             sizeof(armData),
                                             ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing CFR Status into the structure. */
    fpDeserializerTemp = (uint32_t)((uint32_t)armData[0] |
                                    ((uint32_t)armData[1] << 8) |
                                    ((uint32_t)armData[2] << 16) |
                                    ((uint32_t)armData[3] << 24));
    fpDeserializerTemp = HTOCL(fpDeserializerTemp);
    fpDeserializerPtr = (float*)&fpDeserializerTemp;
    cfrHardClipperConfig->cfrHardClipperThreshold = *fpDeserializerPtr;
    cfrHardClipperConfig->cfrHardClipperEnable = armData[4];
    cfrHardClipperConfig->txChannelMask = (uint32_t)txChannel;

    return recoveryAction;
}

int32_t adi_adrv9025_CfrCtrlConfigSet(adi_adrv9025_Device_t*       device,
                                      adi_adrv9025_CfrCtrlConfig_t cfrCtrlConfig[],
                                      uint8_t                      cfrCtrlCfgArraySize)
{
    static const uint8_t CFR_PULSE_MODE_PROG_ID = 0;
    static const uint8_t CFR_TARGET_THRESH_ID = 2;
    static const uint8_t CFR_INTERPOLATION_ID = 3;
    static const uint8_t CFR_CTRL_TX_DELAY_ID = 7;
    static const uint8_t CFR_MAX_NUM_OF_PEAKS_ID = 10;
    static const uint8_t PULSE_MODE_1A_SELECTION = 3;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t armData[20] = { 0 };
    uint32_t cfrCtrlCfgIndex = 0;
    uint8_t *fpSerializerPtr = NULL;
    float f;
    uint32_t* temp;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        cfrCtrlConfig);

#if ADI_ADRV9025_CFR_RANGE_CHECK > 0
    adrv9025_CfrCtrlConfigSetRangeCheck(device,
                                        cfrCtrlConfig,
                                        cfrCtrlCfgArraySize);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    for (cfrCtrlCfgIndex = 0; cfrCtrlCfgIndex < cfrCtrlCfgArraySize; cfrCtrlCfgIndex++)
    {
        /* Program the CFR pulse mode - Defaulted to Mode1A */
        armData[0] = (uint8_t)PULSE_MODE_1A_SELECTION;
        recoveryAction = adrv9025_CfrCtrlCmdExecute(device,
                                                    cfrCtrlConfig[cfrCtrlCfgIndex].txChannelMask,
                                                    CFR_PULSE_MODE_PROG_ID,
                                                    &armData[0],
                                                    (uint8_t)1);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         armData,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Program the CFR Peak threshold and scalars */
        f = cfrCtrlConfig[cfrCtrlCfgIndex].cfrPeakThreshold;
        temp = (uint32_t*)&f;
        *temp = HTOCL(*temp);
        fpSerializerPtr = (uint8_t*)&f;
        armData[0] = fpSerializerPtr[0];
        armData[1] = fpSerializerPtr[1];
        armData[2] = fpSerializerPtr[2];
        armData[3] = fpSerializerPtr[3];

        f = cfrCtrlConfig[cfrCtrlCfgIndex].cfrEngine1PeakThresholdScaler;
        temp = (uint32_t*)&f;
        *temp = HTOCL(*temp);
        fpSerializerPtr = (uint8_t*)&f;
        armData[4] = fpSerializerPtr[0];
        armData[5] = fpSerializerPtr[1];
        armData[6] = fpSerializerPtr[2];
        armData[7] = fpSerializerPtr[3];

        f = cfrCtrlConfig[cfrCtrlCfgIndex].cfrEngine2PeakThresholdScaler;
        temp = (uint32_t*)&f;
        *temp = HTOCL(*temp);
        fpSerializerPtr = (uint8_t*)&f;
        armData[8]  = fpSerializerPtr[0];
        armData[9]  = fpSerializerPtr[1];
        armData[10] = fpSerializerPtr[2];
        armData[11] = fpSerializerPtr[3];

        f = cfrCtrlConfig[cfrCtrlCfgIndex].cfrEngine3PeakThresholdScaler;
        temp = (uint32_t*)&f;
        *temp = HTOCL(*temp);
        fpSerializerPtr = (uint8_t*)&f;
        armData[12] = fpSerializerPtr[0];
        armData[13] = fpSerializerPtr[1];
        armData[14] = fpSerializerPtr[2];
        armData[15] = fpSerializerPtr[3];

        f = cfrCtrlConfig[cfrCtrlCfgIndex].cfrCorrectionThresholdScaler;
        temp = (uint32_t*)&f;
        *temp = HTOCL(*temp);
        fpSerializerPtr = (uint8_t*)&f;
        armData[16] = fpSerializerPtr[0];
        armData[17] = fpSerializerPtr[1];
        armData[18] = fpSerializerPtr[2];
        armData[19] = fpSerializerPtr[3];
        recoveryAction  = adrv9025_CfrCtrlCmdExecute(device,
                                                     cfrCtrlConfig[cfrCtrlCfgIndex].txChannelMask,
                                                     CFR_TARGET_THRESH_ID,
                                                     &armData[0],
                                                     (uint8_t)20);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         armData,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Program the CFR interpolation factor */
        armData[0] = (uint8_t)cfrCtrlConfig[cfrCtrlCfgIndex].cfrInterpolationFactor;
        recoveryAction = adrv9025_CfrCtrlCmdExecute(device,
                                                    cfrCtrlConfig[cfrCtrlCfgIndex].txChannelMask,
                                                    CFR_INTERPOLATION_ID,
                                                    &armData[0],
                                                    (uint8_t)1);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         armData,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Program the CFR Tx Delay, all 3 engines need to have the same delay since they use the same correction pulse */
        uint16_t t = cfrCtrlConfig[cfrCtrlCfgIndex].cfrTxDelay;
        t = HTOCS(t);
        fpSerializerPtr = (uint8_t*)&t;
        armData[0] = fpSerializerPtr[0];
        armData[1] = fpSerializerPtr[1];
        armData[2] = fpSerializerPtr[0];
        armData[3] = fpSerializerPtr[1];
        armData[4] = fpSerializerPtr[0];
        armData[5] = fpSerializerPtr[1];
        recoveryAction  = adrv9025_CfrCtrlCmdExecute(device,
                                                     cfrCtrlConfig[cfrCtrlCfgIndex].txChannelMask,
                                                     CFR_CTRL_TX_DELAY_ID,
                                                     &armData[0],
                                                     (uint8_t)6);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         armData,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Program maximum number of peaks */
        armData[0] = cfrCtrlConfig[cfrCtrlCfgIndex].cfrEngine1MaxNumOfPeaks;
        armData[1] = cfrCtrlConfig[cfrCtrlCfgIndex].cfrEngine2MaxNumOfPeaks;
        armData[2] = cfrCtrlConfig[cfrCtrlCfgIndex].cfrEngine3MaxNumOfPeaks;
        recoveryAction = adrv9025_CfrCtrlCmdExecute(device,
                                                    cfrCtrlConfig[cfrCtrlCfgIndex].txChannelMask,
                                                    CFR_MAX_NUM_OF_PEAKS_ID,
                                                    &armData[0],
                                                    (uint8_t)3);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         armData,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adi_adrv9025_CfrCtrlConfigGet(adi_adrv9025_Device_t*        device,
                                      adi_adrv9025_TxChannels_e     txChannel,
                                      adi_adrv9025_CfrCtrlConfig_t* cfrCtrlConfig)
{
    static const uint8_t CFR_TARGET_THRESH_ID = 2;
    static const uint32_t CFR_PULSE_MODE_OFFSET = 20;
    static const uint32_t CFR_INTERP_OFFSET = 65;
    static const uint32_t CFR_MAX_NUMBER_OF_PEAKS = 75;
    static const uint32_t CFR_DELAY_OFFSET = 84;
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;
    static const uint8_t ARM_ERR_CODE = 0x0E;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[3] = { ADRV9025_CPU_OBJECTID_CAL_STATUS, ADRV9025_CPU_OBJECTID_CFR_INIT, 0 };
    uint8_t armData[20] = { 0 };
    uint8_t cmdStatusByte = 0;
    uint32_t fpDeserializerTemp = 0;
    uint32_t pulseMode = 0;
    float* fpDeserializerPtr = NULL;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        cfrCtrlConfig);

    switch (txChannel)
    {
    case ADI_ADRV9025_TX1:
        {
            extData[2] = 0; /*!< Tx1 channel enabled */
            break;
        }
    case ADI_ADRV9025_TX2:
        {
            extData[2] = 1; /*!< Tx2 channel enabled */
            break;
        }
    case ADI_ADRV9025_TX3:
        {
            extData[2] = 2; /*!< Tx3 channel enabled */
            break;
        }
    case ADI_ADRV9025_TX4:
        {
            extData[2] = 3; /*!< Tx4 channel enabled */
            break;
        }
    default:
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                txChannel,
                "Invalid Tx Channel requested to read back CFR status");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /* Executing CFR status get Arm Command */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     extData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
        ADI_ADRV9025_CPU_TYPE_C,
        (uint8_t)ADRV9025_CPU_GET_OPCODE,
        &cmdStatusByte,
        (uint32_t)ADI_ADRV9025_GETTXCFRSTATUS_TIMEOUT_US,
        (uint32_t)ADI_ADRV9025_GETTXCFRSTATUS_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
            ADI_ADRV9025_CPU_TYPE_C,
            ADI_ADRV9025_SRC_CPUCMD,
                                                     ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                                              extData[0],
                                                                              cmdStatusByte),
            recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     cmdStatusByte,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read the CFR status from ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)(ADRV9025_CPU_C_ADDR_MAILBOX_GET + CFR_PULSE_MODE_OFFSET),
                                             &armData[0],
                                             (uint8_t)4,
                                             ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing CFR mode info into the structure. */
    pulseMode = ((uint32_t)armData[0] |
        ((uint32_t)armData[1] << 8) |
        ((uint32_t)armData[2] << 16) |
        ((uint32_t)armData[3] << 24));

    /* Interpret both pulse mode 1 and 1A as MODE1*/
    if ((pulseMode == 0) || (pulseMode == 3))
    {
        cfrCtrlConfig->cfrMode = ADI_ADRV9025_CFR_MODE1;
    }
    else
    {
        cfrCtrlConfig->cfrMode = (adi_adrv9025_CfrModeSel_e)pulseMode;
    }

    /* Read the CFR Interpolation factor from ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)(ADRV9025_CPU_C_ADDR_MAILBOX_GET + CFR_INTERP_OFFSET),
                                             &armData[0],
                                             (uint8_t)1,
                                             ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing CFR Interpolation factor into the structure. */
    cfrCtrlConfig->cfrInterpolationFactor = (adi_adrv9025_CfrInterpolationSel_e)armData[0];

    /* Read the CFR Tx delay from ARM mailbox, the 3 engines have the same delay, so reading 1 is sufficient */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)(ADRV9025_CPU_C_ADDR_MAILBOX_GET + CFR_DELAY_OFFSET),
                                             &armData[0],
                                             (uint8_t)2,
                                             ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    cfrCtrlConfig->cfrTxDelay = (uint16_t)armData[0] | ((uint16_t)armData[1] << 8);

    /* Read max number of peaks from ARM mailbox for the 3 engines separately*/
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)(ADRV9025_CPU_C_ADDR_MAILBOX_GET + CFR_MAX_NUMBER_OF_PEAKS),
                                             &armData[0],
                                             (uint8_t)3,
                                             ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    cfrCtrlConfig->cfrEngine1MaxNumOfPeaks = armData[0];
    cfrCtrlConfig->cfrEngine2MaxNumOfPeaks = armData[1];
    cfrCtrlConfig->cfrEngine3MaxNumOfPeaks = armData[2];

    /* Get the target threshold from ARM */
    recoveryAction = adrv9025_CfrCtrlCmdGet(device,
                                            txChannel,
                                            CFR_TARGET_THRESH_ID,
                                            &armData[0],
                                            20u);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing CFR Peak threshold into the structure. */
    fpDeserializerTemp = (uint32_t)((uint32_t)armData[0] |
                                    ((uint32_t)armData[1] << 8) |
                                    ((uint32_t)armData[2] << 16) |
                                    ((uint32_t)armData[3] << 24));
    fpDeserializerTemp = HTOCL(fpDeserializerTemp);
    fpDeserializerPtr = (float*)&fpDeserializerTemp;
    cfrCtrlConfig->cfrPeakThreshold = *fpDeserializerPtr;

    /* Deserializing CFR engine 1 peak threshold scaler into the structure. */
    fpDeserializerTemp = (uint32_t)((uint32_t)armData[4] |
                                    ((uint32_t)armData[5] << 8) |
                                    ((uint32_t)armData[6] << 16) |
                                    ((uint32_t)armData[7] << 24));
    fpDeserializerTemp = HTOCL(fpDeserializerTemp);
    fpDeserializerPtr = (float*)&fpDeserializerTemp;
    cfrCtrlConfig->cfrEngine1PeakThresholdScaler = *fpDeserializerPtr;

    /* Deserializing CFR engine 2 peak threshold scaler into the structure. */
    fpDeserializerTemp = (uint32_t)((uint32_t)armData[8] |
                                    ((uint32_t)armData[9] << 8) |
                                    ((uint32_t)armData[10] << 16) |
                                    ((uint32_t)armData[11] << 24));
    fpDeserializerTemp = HTOCL(fpDeserializerTemp);
    fpDeserializerPtr = (float*)&fpDeserializerTemp;
    cfrCtrlConfig->cfrEngine2PeakThresholdScaler = *fpDeserializerPtr;

    /* Deserializing CFR engine 3 peak threshold scaler into the structure. */
    fpDeserializerTemp = (uint32_t)((uint32_t)armData[12] |
                                    ((uint32_t)armData[13] << 8) |
                                    ((uint32_t)armData[14] << 16) |
                                    ((uint32_t)armData[15] << 24));
    fpDeserializerTemp = HTOCL(fpDeserializerTemp);
    fpDeserializerPtr = (float*)&fpDeserializerTemp;
    cfrCtrlConfig->cfrEngine3PeakThresholdScaler = *fpDeserializerPtr;

    /* Deserializing CFR correction threshold scaler into the structure. */
    fpDeserializerTemp = (uint32_t)((uint32_t)armData[16] |
                                    ((uint32_t)armData[17] << 8) |
                                    ((uint32_t)armData[18] << 16) |
                                    ((uint32_t)armData[19] << 24));
    fpDeserializerTemp = HTOCL(fpDeserializerTemp);
    fpDeserializerPtr = (float*)&fpDeserializerTemp;
    cfrCtrlConfig->cfrCorrectionThresholdScaler = *fpDeserializerPtr;

    /* Set the Tx channel mask equal to the requested Tx channel */
    cfrCtrlConfig->txChannelMask = (uint32_t)txChannel;

    return recoveryAction;
}

int32_t adi_adrv9025_CfrEnableSet(adi_adrv9025_Device_t*   device,
                                  adi_adrv9025_CfrEnable_t cfrEnable[],
                                  uint8_t                  cfrEnableArraySize)
{
    static const uint8_t CFR_ENGINE_ENABLE_ID = 4;
    static const uint8_t CFR_ENGINE_BYPASS_ENABLE_ID = 5;
    static const uint8_t CFR_ENABLE_MASK = 0x01;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t armData[3] = { 0 };
    uint32_t cfrEnableIndex = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        cfrEnable);

    for (cfrEnableIndex = 0; cfrEnableIndex < cfrEnableArraySize; cfrEnableIndex++)
    {
        /* Program the CFR enable modes */
        armData[0] = (cfrEnable[cfrEnableIndex].cfrEngine1Enable & CFR_ENABLE_MASK);
        armData[1] = (cfrEnable[cfrEnableIndex].cfrEngine2Enable & CFR_ENABLE_MASK);
        armData[2] = (cfrEnable[cfrEnableIndex].cfrEngine3Enable & CFR_ENABLE_MASK);
        recoveryAction = adrv9025_CfrCtrlCmdExecute(device,
                                                    cfrEnable[cfrEnableIndex].txChannelMask,
                                                    CFR_ENGINE_ENABLE_ID,
                                                    &armData[0],
                                                    (uint8_t)3);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         armData,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Program the CFR bypass enable modes */
        armData[0] = (cfrEnable[cfrEnableIndex].cfrEngine1BypassEnable & CFR_ENABLE_MASK);
        armData[1] = (cfrEnable[cfrEnableIndex].cfrEngine2BypassEnable & CFR_ENABLE_MASK);
        armData[2] = (cfrEnable[cfrEnableIndex].cfrEngine3BypassEnable & CFR_ENABLE_MASK);
        recoveryAction = adrv9025_CfrCtrlCmdExecute(device,
                                                    cfrEnable[cfrEnableIndex].txChannelMask,
                                                    CFR_ENGINE_BYPASS_ENABLE_ID,
                                                    &armData[0],
                                                    (uint8_t)3);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         armData,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adi_adrv9025_CfrEnableGet(adi_adrv9025_Device_t*    device,
                                  adi_adrv9025_TxChannels_e txChannel,
                                  adi_adrv9025_CfrEnable_t* cfrEnable)
{
    static const uint32_t CFR_ENABLE_OFFSET = 66;
    static const uint8_t  ARM_ERR_CODE = 0x0E;
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[3] = { ADRV9025_CPU_OBJECTID_CAL_STATUS, ADRV9025_CPU_OBJECTID_CFR_INIT, 0 };
    uint8_t armData[6] = { 0 };
    uint8_t cmdStatusByte = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        cfrEnable);

    switch (txChannel)
    {
    case ADI_ADRV9025_TX1:
        {
            extData[2] = 0; /*!< Tx1 channel enabled */
            break;
        }
    case ADI_ADRV9025_TX2:
        {
            extData[2] = 1; /*!< Tx2 channel enabled */
            break;
        }
    case ADI_ADRV9025_TX3:
        {
            extData[2] = 2; /*!< Tx3 channel enabled */
            break;
        }
    case ADI_ADRV9025_TX4:
        {
            extData[2] = 3; /*!< Tx4 channel enabled */
            break;
        }
    default:
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                txChannel,
                "Invalid Tx Channel requested to read back CFR status");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /* Executing CFR status get Arm Command */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     extData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
        ADI_ADRV9025_CPU_TYPE_C,
        (uint8_t)ADRV9025_CPU_GET_OPCODE,
        &cmdStatusByte,
        (uint32_t)ADI_ADRV9025_GETTXCFRSTATUS_TIMEOUT_US,
        (uint32_t)ADI_ADRV9025_GETTXCFRSTATUS_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
            ADI_ADRV9025_CPU_TYPE_C,
            ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                             extData[0],
                                                             cmdStatusByte),
            recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     cmdStatusByte,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read the CFR status from ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)(ADRV9025_CPU_C_ADDR_MAILBOX_GET + CFR_ENABLE_OFFSET),
                                             &armData[0],
                                             sizeof(armData),
                                             ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing CFR enable and bypass info into the structure. */
    cfrEnable->cfrEngine1Enable = armData[0];
    cfrEnable->cfrEngine2Enable = armData[1];
    cfrEnable->cfrEngine3Enable = armData[2];
    cfrEnable->cfrEngine1BypassEnable = armData[3];
    cfrEnable->cfrEngine2BypassEnable = armData[4];
    cfrEnable->cfrEngine3BypassEnable = armData[5];

    /* Set the Tx channel mask equal to the requested Tx channel */
    cfrEnable->txChannelMask = (uint32_t)txChannel;

    return recoveryAction;
}

int32_t adi_adrv9025_CfrCorrectionPulseWrite_v2(adi_adrv9025_Device_t *device,
                                                uint32_t txChannelMask,
                                                adi_adrv9025_CfrCorrectionPulse_t cfrCorrectionPulses[],
                                                uint8_t numCorrectionPulses)
{
    static const uint8_t CFR_PULSE_DONE_ID = 1;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t armData[3] = { 0 };
    adi_adrv9025_CfrCorrectionPulse_t cfrCorrectionPulsesConcatenated = { {0}, {0}, 0 };

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        cfrCorrectionPulses);

#if ADI_ADRV9025_CFR_RANGE_CHECK > 0
    adrv9025_CfrCorrectionPulseWriteRangeCheck(device,
                                               cfrCorrectionPulses,
                                               numCorrectionPulses);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    if (numCorrectionPulses == 1)
    {
        recoveryAction = adrv9025_CfrCorrectionPulseWrite(device,
                                                          txChannelMask,
                                                          cfrCorrectionPulses[0].coeffRealHalfPulse,
                                                          cfrCorrectionPulses[0].coeffImaginaryHalfPulse,
                                                          cfrCorrectionPulses[0].numCoeffs);
        ADI_ERROR_REPORT(&device->common,
                         device->common.error.errSource,
                         device->common.error.errCode,
                         recoveryAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if ((numCorrectionPulses == 2) &&
             (cfrCorrectionPulses[0].numCoeffs <= ((uint16_t)ADI_ADRV9025_MAX_CFR_CORRECTION_PULSE_LEN / 2)) &&
             (cfrCorrectionPulses[1].numCoeffs <= ((uint16_t)ADI_ADRV9025_MAX_CFR_CORRECTION_PULSE_LEN / 2)) &&
             (cfrCorrectionPulses[0].numCoeffs == cfrCorrectionPulses[1].numCoeffs))
    {
        /* Concatenate the two correction pulses */
        recoveryAction = adrv9025_CfrCorrectionPulsesConcatenate(device,
                                                                 cfrCorrectionPulses,
                                                                 numCorrectionPulses,
                                                                 &cfrCorrectionPulsesConcatenated);
        ADI_ERROR_REPORT(&device->common,
                         device->common.error.errSource,
                         device->common.error.errCode,
                         recoveryAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Program the concatenated correction pulses */
        recoveryAction = adrv9025_CfrCorrectionPulseWrite(device,
                                                          txChannelMask,
                                                          cfrCorrectionPulsesConcatenated.coeffRealHalfPulse,
                                                          cfrCorrectionPulsesConcatenated.coeffImaginaryHalfPulse,
                                                          cfrCorrectionPulsesConcatenated.numCoeffs);
        ADI_ERROR_REPORT(&device->common,
                         device->common.error.errSource,
                         device->common.error.errCode,
                         recoveryAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Overwrite the programming done signal to the ARM alongwith the half pulse length */
        armData[0] = (uint8_t)((cfrCorrectionPulsesConcatenated.numCoeffs / 2) & 0x00FF);
        armData[1] = (uint8_t)(((cfrCorrectionPulsesConcatenated.numCoeffs / 2) & 0xFF00) >> 8);
        armData[2] = (uint8_t)0; /* Setting Component carrier to 0 for mode 1 support. Has to be modified for mode 2 and mode 3 */
        recoveryAction = adrv9025_CfrCtrlCmdExecute(device,
                                                    txChannelMask,
                                                    CFR_PULSE_DONE_ID,
                                                    &armData[0],
                                                    (uint8_t)sizeof(armData));
        ADI_ERROR_REPORT(&device->common,
                         device->common.error.errSource,
                         device->common.error.errCode,
                         recoveryAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         cfrCorrectionPulses,
                         "Invalid no. of CFR correction pulse coefficients or invalid no. of CFR correction pulses requested to be programmed");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adi_adrv9025_CfrCorrectionPulseRead_v2(adi_adrv9025_Device_t *device,
                                               adi_adrv9025_TxChannels_e txChannel,
                                               uint8_t maxCorrectionPulsesToRead,
                                               adi_adrv9025_CfrCorrectionPulse_t cfrCorrectionPulses[],
                                               uint8_t *numCorrectionPulsesReturned)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint16_t coeffIndex = 0;
    uint16_t currentHalfPulseLen = 0;
    uint8_t tempNumCorrPulsesRead = 0;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr = (adrv9025_BfTxChanAddr_e)0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        cfrCorrectionPulses);

    /* Ensure that Tx channel is valid */
    if ((txChannel != ADI_ADRV9025_TX1) &&
        (txChannel != ADI_ADRV9025_TX2) &&
        (txChannel != ADI_ADRV9025_TX3) &&
        (txChannel != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel requested for reading back CFR pulse(s). Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Resolve the Tx channel base address */
    txChannelBaseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                       (uint32_t)txChannel);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Retrieve the current half pulse len */
    recoveryAction = adrv9025_TxCfrHalfPulseLenBfGet(device,
                                                     txChannelBaseAddr,
                                                     &currentHalfPulseLen);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     recoveryAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read the coefficients from the device for the requested Tx channel */
    if (maxCorrectionPulsesToRead > 0)
    {
        /* Read the maximum no. of coefficients and store it in cfrCorrectionPulse[0] */
        recoveryAction = adrv9025_CfrCorrectionPulseRead(device,
                                                         txChannel,
                                                         cfrCorrectionPulses[0].coeffRealHalfPulse,
                                                         cfrCorrectionPulses[0].coeffImaginaryHalfPulse,
                                                         (uint16_t)ADI_ADRV9025_MAX_CFR_CORRECTION_PULSE_LEN);
        ADI_ERROR_REPORT(&device->common,
                         device->common.error.errSource,
                         device->common.error.errCode,
                         recoveryAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        tempNumCorrPulsesRead++;
        cfrCorrectionPulses[0].numCoeffs = currentHalfPulseLen;

        /* De-concatenate the two half pulses if the no. of correction pulses to read are greater than 2 and the current half pulse length is less than 2 */
        if ((maxCorrectionPulsesToRead > 1) &&
            (currentHalfPulseLen <= ((uint16_t)ADI_ADRV9025_MAX_CFR_CORRECTION_PULSE_LEN/2)))
        {
            for (coeffIndex = currentHalfPulseLen; coeffIndex < (2 * currentHalfPulseLen); coeffIndex++)
            {
                cfrCorrectionPulses[1].coeffRealHalfPulse[(coeffIndex - currentHalfPulseLen)] = cfrCorrectionPulses[0].coeffRealHalfPulse[coeffIndex];
                cfrCorrectionPulses[1].coeffImaginaryHalfPulse[(coeffIndex - currentHalfPulseLen)] = cfrCorrectionPulses[0].coeffImaginaryHalfPulse[coeffIndex];
            }

            tempNumCorrPulsesRead++;
            cfrCorrectionPulses[1].numCoeffs = currentHalfPulseLen;
        }
    }

    /* Update the no. of correction pulses retrieved from the device */
    if (numCorrectionPulsesReturned != NULL)
    {
        *numCorrectionPulsesReturned = tempNumCorrPulsesRead;
    }

    return recoveryAction;
}

int32_t adi_adrv9025_CfrActiveCorrectionPulseSet(adi_adrv9025_Device_t *device,
                                                 uint32_t txChannelMask,
                                                 adi_adrv9025_CfrCarrierHotSwapCorrPulseSel_e cfrCorrectionPulseSel)
{
    static const adi_adrv9025_TxChannels_e txChannelArr[] = { ADI_ADRV9025_TX1, ADI_ADRV9025_TX2, ADI_ADRV9025_TX3, ADI_ADRV9025_TX4 };
    static const uint32_t ALL_TX_CHANNELS = (uint32_t)(ADI_ADRV9025_TXALL);

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t txChannelIndex = (uint32_t)ADI_ADRV9025_TXOFF;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr = (adrv9025_BfTxChanAddr_e)0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if ((txChannelMask & ~ALL_TX_CHANNELS) != 0)
    {
        /* Tx channel selection is not valid */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelMask,
                         "Invalid Tx channel selection, any combination of TxOff/Tx1/2/3/4 channels are valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

#if ADI_ADRV9025_CFR_RANGE_CHECK > 0
    adrv9025_CfrActiveCorrectionPulseSetRangeCheck(device,
                                                   txChannelMask,
                                                   cfrCorrectionPulseSel);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    for (txChannelIndex = 0; txChannelIndex < (sizeof(txChannelArr) / sizeof(txChannelArr[0])); txChannelIndex++)
    {
        if ((txChannelArr[txChannelIndex] & txChannelMask) == txChannelArr[txChannelIndex])
        {
            /* Resolve the base address for the requested Tx channel */
            txChannelBaseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                               (uint32_t)txChannelArr[txChannelIndex]);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Execute correction pulse switch */
            recoveryAction = adrv9025_TxCfrBasePulse21bBfSet(device,
                                                             txChannelBaseAddr,
                                                             (uint8_t)cfrCorrectionPulseSel);
            ADI_ERROR_REPORT(&device->common,
                             device->common.error.errSource,
                             device->common.error.errCode,
                             recoveryAction,
                             device->common.error.varName,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adi_adrv9025_CfrActiveCorrectionPulseGet(adi_adrv9025_Device_t *device,
                                                 adi_adrv9025_TxChannels_e txChannel,
                                                 adi_adrv9025_CfrCarrierHotSwapCorrPulseSel_e *cfrCorrectionPulseSel)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t cfrActivePulseStatus = 0;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr = (adrv9025_BfTxChanAddr_e)0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        cfrCorrectionPulseSel);

    /* Ensure that Tx channel is valid */
    if ((txChannel != ADI_ADRV9025_TX1) &&
        (txChannel != ADI_ADRV9025_TX2) &&
        (txChannel != ADI_ADRV9025_TX3) &&
        (txChannel != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel requested for reading back active correction pulse status. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Resolve the base address for the requested Tx channel */
    txChannelBaseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                       (uint32_t)txChannel);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read back active correction pulse */
    recoveryAction = adrv9025_TxCfrBasePulse21bBfGet(device,
                                                     txChannelBaseAddr,
                                                     &cfrActivePulseStatus);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     recoveryAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Update the pointer with the active correction pusle status */
    *cfrCorrectionPulseSel = (adi_adrv9025_CfrCarrierHotSwapCorrPulseSel_e)cfrActivePulseStatus;

    return recoveryAction;
}

int32_t adi_adrv9025_CfrPipelineDelayGet(adi_adrv9025_Device_t *device, adi_adrv9025_TxChannels_e txChannel, uint16_t *pipelineDelay)
{
    static const uint8_t ARM_ERR_CODE = 0x0E;
    static const uint32_t PIPELINE_DELAY_ADDR_OFFSET = 0x005C;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[3] = { ADRV9025_CPU_OBJECTID_CAL_STATUS, ADRV9025_CPU_OBJECTID_CFR_INIT, 0 };
    uint8_t armData[2] = { 0 };
    uint8_t cmdStatusByte = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, pipelineDelay);

    switch (txChannel)
    {
    case ADI_ADRV9025_TX1:
    {
        extData[2] = 0; /*!< Tx1 channel enabled */
        break;
    }
    case ADI_ADRV9025_TX2:
    {
        extData[2] = 1; /*!< Tx2 channel enabled */
        break;
    }
    case ADI_ADRV9025_TX3:
    {
        extData[2] = 2; /*!< Tx3 channel enabled */
        break;
    }
    case ADI_ADRV9025_TX4:
    {
        extData[2] = 3; /*!< Tx4 channel enabled */
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            txChannel,
            "Invalid Tx Channel requested to read back CFR pipeline delay");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Executing CFR tracking cal status get Arm Command */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device, ADI_ADRV9025_CPU_TYPE_C, (uint8_t)ADRV9025_CPU_GET_OPCODE, &extData[0], sizeof(extData));
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, extData, device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
        ADI_ADRV9025_CPU_TYPE_C,
        (uint8_t)ADRV9025_CPU_GET_OPCODE,
        &cmdStatusByte,
        (uint32_t)ADI_ADRV9025_GETTXCFRSTATUS_TIMEOUT_US,
        (uint32_t)ADI_ADRV9025_GETTXCFRSTATUS_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
            ADI_ADRV9025_CPU_TYPE_C,
            ADI_ADRV9025_SRC_CPUCMD,
            ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE, extData[0], cmdStatusByte),
            recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, cmdStatusByte, device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read the CFR status from ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device, ((uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET + PIPELINE_DELAY_ADDR_OFFSET), &armData[0], sizeof(armData), ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, armData, device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing CFR Pipeline delay into 16 bit variable */
    *pipelineDelay = (uint16_t)armData[0] | ((uint16_t)armData[1] << 8);

    return recoveryAction;
}

int32_t adi_adrv9025_CfrStatusGet(adi_adrv9025_Device_t *device,
                                  adi_adrv9025_TxChannels_e txChannel,
                                  adi_adrv9025_CfrStatus_t *cfrStatus)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;
    static const uint8_t ARM_ERR_CODE = 0x0E;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[3] = { ADRV9025_CPU_OBJECTID_CAL_STATUS, ADRV9025_CPU_OBJECTID_CFR_INIT, 0 };
    uint8_t armData[4] = { 0 };
    uint8_t cmdStatusByte = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        cfrStatus);

    switch (txChannel)
    {
    case ADI_ADRV9025_TX1:
        {
            extData[2] = 0; /*!< Tx1 channel enabled */
            break;
        }
    case ADI_ADRV9025_TX2:
        {
            extData[2] = 1; /*!< Tx2 channel enabled */
            break;
        }
    case ADI_ADRV9025_TX3:
        {
            extData[2] = 2; /*!< Tx3 channel enabled */
            break;
        }
    case ADI_ADRV9025_TX4:
        {
            extData[2] = 3; /*!< Tx4 channel enabled */
            break;
        }
    default:
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                txChannel,
                "Invalid Tx Channel requested to read back CFR status");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /* Executing CFR tracking cal status get Arm Command */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     extData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
        ADI_ADRV9025_CPU_TYPE_C,
        (uint8_t)ADRV9025_CPU_GET_OPCODE,
        &cmdStatusByte,
        (uint32_t)ADI_ADRV9025_GETTXCFRSTATUS_TIMEOUT_US,
        (uint32_t)ADI_ADRV9025_GETTXCFRSTATUS_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
            ADI_ADRV9025_CPU_TYPE_C,
            ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                             extData[0],
                                                             cmdStatusByte),
            recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     cmdStatusByte,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read the CFR status from ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                                             &armData[0],
                                             sizeof(armData),
                                             ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing CFR Status into the structure. */
    cfrStatus->cfrErrorCode = (adi_adrv9025_CfrError_e)(((uint32_t)armData[0]) |
                                                       ((uint32_t)armData[1] << 8) |
                                                       ((uint32_t)armData[2] << 16) |
                                                       ((uint32_t)armData[3] << 24));

    /* Read CFR statistics into the CFR status struct */
    recoveryAction = adrv9025_CfrStatisticsGet(device,
                                               txChannel,
                                               &cfrStatus->cfrStatistics);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     cmdStatusByte,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adi_adrv9025_CfrStatisticsReset(adi_adrv9025_Device_t* device,
                                        uint32_t               txChannelMask)
{
    static const adi_adrv9025_TxChannels_e txChannelArr[] = { ADI_ADRV9025_TX1, ADI_ADRV9025_TX2, ADI_ADRV9025_TX3, ADI_ADRV9025_TX4 };
    static const uint8_t ARM_ERR_CODE = 0x0E;
    static const uint8_t CFR_ENGINE_STATISTICS_ID = 16;
    static const uint8_t CFR_STATS_INFO_CUSTOM_SUB_CMD = 0x80;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4]     = {
        ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL,
        ADRV9025_CPU_OBJECTID_CFR_INIT,
        (uint8_t)0,
        (CFR_ENGINE_STATISTICS_ID | CFR_STATS_INFO_CUSTOM_SUB_CMD)
    };
    uint8_t txChannelIndex = 0;
    uint8_t cmdStatusByte = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    for (txChannelIndex = 0; txChannelIndex < (uint8_t)(sizeof(txChannelArr) / sizeof(txChannelArr[0])); txChannelIndex++)
    {
        if ((txChannelMask & txChannelArr[txChannelIndex]) == txChannelArr[txChannelIndex])
        {
            /* Set the target Tx channel */
            extData[2] = (uint8_t)txChannelArr[txChannelIndex];

            /* Executing CFR statistics set Cpu Command */
            recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                                      ADI_ADRV9025_CPU_TYPE_C,
                                                      (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                      &extData[0],
                                                      sizeof(extData));
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             extData,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Wait for command to finish executing */
            recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                           ADI_ADRV9025_CPU_TYPE_C,
                                                           (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                           &cmdStatusByte,
                                                           (uint32_t)ADI_ADRV9025_GETTXCFRSTATUS_TIMEOUT_US,
                                                           (uint32_t)ADI_ADRV9025_GETTXCFRSTATUS_INTERVAL_US);

            /* If cmdStatusByte is non-zero then flag an ARM error */
            if ((cmdStatusByte & ARM_ERR_CODE) > 0)
            {
                adrv9025_CpuCmdErrorHandler(device,
                                            ADI_ADRV9025_CPU_TYPE_C,
                                            ADI_ADRV9025_SRC_CPUCMD,
                                            ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                                     extData[0],
                                                                     cmdStatusByte),
                                                             recoveryAction);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             cmdStatusByte,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adi_adrv9025_ClgcConfigSet(adi_adrv9025_Device_t*    device,
                                   adi_adrv9025_ClgcConfig_t clgcConfig[],
                                   uint8_t                   clgcConfigArraySize)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t clgcCfgIndex = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that clgcConfig is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        clgcConfig);

#if ADI_ADRV9025_CLGC_RANGE_CHECK > 0
    adrv9025_ClgcConfigSetRangeCheck(device,
                                     clgcConfig,
                                     clgcConfigArraySize);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    for (clgcCfgIndex = 0; clgcCfgIndex < clgcConfigArraySize; clgcCfgIndex++)
    {
        recoveryAction = adrv9025_ClgcConfigSet(device,
                                                &clgcConfig[clgcCfgIndex]);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         clgcConfig[clgcCfgIndex],
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adi_adrv9025_ClgcConfigGet(adi_adrv9025_Device_t*     device,
                                   adi_adrv9025_TxChannels_e  txChannel,
                                   adi_adrv9025_ClgcConfig_t* clgcConfig)
{
    static const uint16_t CLGC_CONFIG_SIZE_BYTES = 56;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint16_t clgcConfigOffsetBytes = 0;
    uint8_t armConfigData[48] = { 0 };
    uint32_t fpDeserializerTemp = 0;
    float* fpDeserializerPtr = NULL;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that clgcConfig is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        clgcConfig);

    switch (txChannel)
    {
        case(ADI_ADRV9025_TX1):
        {
            clgcConfigOffsetBytes = 0;
            break;
        }
        case(ADI_ADRV9025_TX2):
        {
            clgcConfigOffsetBytes = CLGC_CONFIG_SIZE_BYTES;
            break;
        }
        case(ADI_ADRV9025_TX3):
        {
            clgcConfigOffsetBytes = CLGC_CONFIG_SIZE_BYTES * 2;
            break;
        }
        case(ADI_ADRV9025_TX4):
        {
            clgcConfigOffsetBytes = CLGC_CONFIG_SIZE_BYTES * 3;
            break;
        }
        default:
        {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                             "Invalid target Tx channel selected for reading back CLGC config. Valid Tx channels are Tx1-Tx4");
            ADI_ERROR_RETURN(device->common.error.newAction);
            break;
        }
    }

    /* Read the DPD actuator gain monitor config from ARM */
    recoveryAction = adi_adrv9025_CpuConfigRead(device,
                                                ADI_ADRV9025_CPU_TYPE_C,
                                                ADRV9025_CPU_OBJECTID_CLGC_TRACKING,
                                                clgcConfigOffsetBytes,
                                                &armConfigData[0],
                                                sizeof(armConfigData));
    ADI_ERROR_RETURN(recoveryAction);

    /* Deserialize the ARM config data into the CLGC config array */
    clgcConfig->txChannelMask = (uint32_t)txChannel;

    clgcConfig->clgcMeasurementBatchTime_us = (uint16_t)((uint16_t)armConfigData[0] | ((uint16_t)armConfigData[1] << 8));

    clgcConfig->clgcEnableGainControl = armConfigData[4];

     fpDeserializerTemp = (uint32_t)((uint32_t)armConfigData[16] |
                                    ((uint32_t)armConfigData[17] << 8) |
                                    ((uint32_t)armConfigData[18] << 16) |
                                    ((uint32_t)armConfigData[19] << 24));
     fpDeserializerTemp = HTOCL(fpDeserializerTemp);
     fpDeserializerPtr = (float*)&fpDeserializerTemp;
     clgcConfig->clgcTxQualifyingThreshold_dBFS = *fpDeserializerPtr;

     fpDeserializerTemp = (uint32_t)((uint32_t)armConfigData[20] |
                                    ((uint32_t)armConfigData[21] << 8) |
                                    ((uint32_t)armConfigData[22] << 16) |
                                    ((uint32_t)armConfigData[23] << 24));
     fpDeserializerTemp = HTOCL(fpDeserializerTemp);
     fpDeserializerPtr = (float*)&fpDeserializerTemp;
     clgcConfig->clgcOrxQualifyingThreshold_dBFS = *fpDeserializerPtr;

     fpDeserializerTemp = (uint32_t)((uint32_t)armConfigData[24] |
                                    ((uint32_t)armConfigData[25] << 8) |
                                    ((uint32_t)armConfigData[26] << 16) |
                                    ((uint32_t)armConfigData[27] << 24));
     fpDeserializerTemp = HTOCL(fpDeserializerTemp);
     fpDeserializerPtr = (float*)&fpDeserializerTemp;
     clgcConfig->clgcExpectedLoopGainRipple_dB = *fpDeserializerPtr;

     fpDeserializerTemp = (uint32_t)((uint32_t)armConfigData[28] |
                                    ((uint32_t)armConfigData[29] << 8) |
                                    ((uint32_t)armConfigData[30] << 16) |
                                    ((uint32_t)armConfigData[31] << 24));
     fpDeserializerTemp = HTOCL(fpDeserializerTemp);
     fpDeserializerPtr = (float*)&fpDeserializerTemp;
     clgcConfig->clgcExpectedLoopGain_dB = *fpDeserializerPtr;

     fpDeserializerTemp = (uint32_t)((uint32_t)armConfigData[32] |
                                    ((uint32_t)armConfigData[33] << 8) |
                                    ((uint32_t)armConfigData[34] << 16) |
                                    ((uint32_t)armConfigData[35] << 24));
     fpDeserializerTemp = HTOCL(fpDeserializerTemp);
     fpDeserializerPtr = (float*)&fpDeserializerTemp;
     clgcConfig->clgcMaxGainAdjustmentStepSize_dB = *fpDeserializerPtr;

    fpDeserializerTemp = (uint32_t)((uint32_t)armConfigData[36] |
                                   ((uint32_t)armConfigData[37] << 8) |
                                   ((uint32_t)armConfigData[38] << 16) |
                                   ((uint32_t)armConfigData[39] << 24));
    fpDeserializerTemp = HTOCL(fpDeserializerTemp);
    fpDeserializerPtr = (float*)&fpDeserializerTemp;
    clgcConfig->clgcMinTxAttenAdjust_dB = *fpDeserializerPtr;

    fpDeserializerTemp = (uint32_t)((uint32_t)armConfigData[40] |
                                   ((uint32_t)armConfigData[41] << 8) |
                                   ((uint32_t)armConfigData[42] << 16) |
                                   ((uint32_t)armConfigData[43] << 24));
    fpDeserializerTemp = HTOCL(fpDeserializerTemp);
    fpDeserializerPtr = (float*)&fpDeserializerTemp;
    clgcConfig->clgcMaxTxAttenAdjust_dB = *fpDeserializerPtr;

     fpDeserializerTemp = (uint32_t)((uint32_t)armConfigData[44] |
                                    ((uint32_t)armConfigData[45] << 8) |
                                    ((uint32_t)armConfigData[46] << 16) |
                                    ((uint32_t)armConfigData[47] << 24));
     fpDeserializerTemp = HTOCL(fpDeserializerTemp);
     fpDeserializerPtr = (float*)&fpDeserializerTemp;
     clgcConfig->clgcDampingParam = *fpDeserializerPtr;

    return recoveryAction;
}

int32_t adi_adrv9025_ClgcStatusGet(adi_adrv9025_Device_t*     device,
                                   adi_adrv9025_TxChannels_e  txChannel,
                                   adi_adrv9025_ClgcStatus_t* clgcStatus)
{
     static const uint8_t ARM_ERR_STATUS = 0x0E;

     int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
     uint8_t extData[3] = { ADRV9025_CPU_OBJECTID_CAL_STATUS, ADRV9025_CPU_OBJECTID_CLGC_TRACKING, 0 };
     uint8_t armData[140] = { 0 };
     uint8_t cmdStatusByte = 0;
     uint32_t fpDeserializerTemp = 0;
     float *fpDeserializerPtr = NULL;

     ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        clgcStatus);

     switch (txChannel)
     {
         case ADI_ADRV9025_TX1:
              extData[2] = 0;    /*!< Tx0 channel enabled */
              break;
         case ADI_ADRV9025_TX2:
              extData[2] = 1;    /*!< Tx1 channel enabled */
              break;
         case ADI_ADRV9025_TX3:
              extData[2] = 2;    /*!< Tx2 channel enabled */
              break;
         case ADI_ADRV9025_TX4:
              extData[2] = 3;    /*!< Tx3 channel enabled */
              break;
         default:
         {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                              "Invalid Tx Channel");
             ADI_ERROR_RETURN(device->common.error.newAction);
         }
     }

     /* Executing Tracking cals Tx LOL status get Arm Command */
    adi_adrv9025_CpuCmdWrite(device,
                             ADI_ADRV9025_CPU_TYPE_C,
                             (uint8_t)ADRV9025_CPU_GET_OPCODE,
                             &extData[0],
                             sizeof(extData));
     ADI_ERROR_RETURN(device->common.error.newAction);

     /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_GETTXCLGCSTATUS_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETTXCLGCSTATUS_INTERVAL_US);

     /* If cmdStatusByte is non-zero then flag an ARM error */
     if ((cmdStatusByte & ARM_ERR_STATUS) > 0)
     {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                             ADRV9025_CPU_OBJECTID_CLGC_TRACKING,
                                                             cmdStatusByte),
                                                      recoveryAction);
         ADI_ERROR_RETURN(device->common.error.newAction);
     }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     cmdStatusByte,
                     device->common.error.errormessage);
     ADI_ERROR_RETURN(device->common.error.newAction);

     /* Read Tx LOL config from ARM mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            sizeof(armData),
                            ADI_ADRV9025_CPU_MEM_AUTO_INCR);
     ADI_ERROR_RETURN(device->common.error.newAction);

     /* Deserializing CLGC Status into the structure. */
     clgcStatus->clgcTrackingCalStatus.errorCode = (adi_adrv9025_ClgcTrackingCalError_e)((uint32_t)armData[0] |
                                                                                         ((uint32_t)armData[1] << 8) |
                                                                                         ((uint32_t)armData[2] << 16) |
                                                                                         ((uint32_t)armData[3] << 24));

     clgcStatus->clgcTrackingCalStatus.percentComplete = (uint32_t)((uint32_t)armData[4] |
                                                                    ((uint32_t)armData[5] << 8) |
                                                                    ((uint32_t)armData[6] << 16) |
                                                                    ((uint32_t)armData[7] << 24));

     clgcStatus->clgcTrackingCalStatus.varianceMetric = (uint32_t)((uint32_t)armData[8] |
                                                                   ((uint32_t)armData[9] << 8) |
                                                                   ((uint32_t)armData[10] << 16) |
                                                                   ((uint32_t)armData[11] << 24));

     clgcStatus->clgcTrackingCalStatus.iterCount = (uint32_t)((uint32_t)armData[12] |
                                                              ((uint32_t)armData[13] << 8) |
                                                              ((uint32_t)armData[14] << 16) |
                                                              ((uint32_t)armData[15] << 24));

     clgcStatus->clgcTrackingCalStatus.updateCount = (uint32_t)((uint32_t)armData[16] |
                                                                ((uint32_t)armData[17] << 8) |
                                                                ((uint32_t)armData[18] << 16) |
                                                                ((uint32_t)armData[19] << 24));

     clgcStatus->clgcState = (adi_adrv9025_ClgcRunState_e)armData[20];

     clgcStatus->clgcCaptureStatus = (adi_adrv9025_ClgcCaptureStatus_e)armData[24];

     clgcStatus->clgcSyncStatus = (adi_adrv9025_TrackingCalSyncStatus_e)armData[28];

     fpDeserializerTemp = (uint32_t)((uint32_t)armData[88] |
                                     ((uint32_t)armData[89] << 8) |
                                     ((uint32_t)armData[90] << 16) |
                                     ((uint32_t)armData[91] << 24));
     fpDeserializerTemp = HTOCL(fpDeserializerTemp);
     fpDeserializerPtr = (float*)&fpDeserializerTemp;
     clgcStatus->clgcTxRmsPower = *fpDeserializerPtr;

     fpDeserializerTemp = (uint32_t)((uint32_t)armData[92] |
                                     ((uint32_t)armData[93] << 8) |
                                     ((uint32_t)armData[94] << 16) |
                                     ((uint32_t)armData[95] << 24));
     fpDeserializerTemp = HTOCL(fpDeserializerTemp);
     fpDeserializerPtr = (float*)&fpDeserializerTemp;
     clgcStatus->clgcOrxRmsPower = *fpDeserializerPtr;

     fpDeserializerTemp = (uint32_t)((uint32_t)armData[96] |
                                     ((uint32_t)armData[97] << 8) |
                                     ((uint32_t)armData[98] << 16) |
                                     ((uint32_t)armData[99] << 24));
     fpDeserializerTemp = HTOCL(fpDeserializerTemp);
     fpDeserializerPtr = (float*)&fpDeserializerTemp;
     clgcStatus->clgcLoopGain = *fpDeserializerPtr;

     clgcStatus->activeOrxGainIndex = armData[44];

     clgcStatus->activeTxAttenIndex = (((uint16_t)armData[43] << 8) | ((uint16_t)armData[42]));
     clgcStatus->activeTxAttenIndex = (clgcStatus->activeTxAttenIndex >> 4) & 0x0FFF;
     ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adi_adrv9025_EnhancedDpdWidebandRegularizationConfigSet(adi_adrv9025_Device_t* device,
                                                                uint16_t               waveformDataReal[],
                                                                uint16_t               waveformDataImag[],
                                                                uint32_t               dataCount)
{
    static const uint32_t WAVEFORM_DATA_COUNT = 2048u;
    static const uint32_t ARM_DATA_COUNT = 8192u;
    static const uint32_t PM_ARMD_CUSTOM_WAVEFORM_BUF = 0x01005000u;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t armData[8192] = { 0 }; /* 8192 should be ARM_DATA_COUNT */

    uint32_t i = 0;
    uint32_t j = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        waveformDataReal);
    ADI_NULL_PTR_RETURN(&device->common,
                        waveformDataImag);

    if (dataCount > WAVEFORM_DATA_COUNT)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dataCount,
                         "Invalid waveform data count, the expected data count is less than 2048");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < dataCount; i++)
    {
        j = i * 4;
        armData[j]     = (uint8_t)(waveformDataReal[i] & 0xFF);
        armData[j + 1] = (uint8_t)((waveformDataReal[i] >> 8) & 0xFF);
        armData[j + 2] = (uint8_t)(waveformDataImag[i] & 0xFF);
        armData[j + 3] = (uint8_t)((waveformDataImag[i] >> 8) & 0xFF);
    }

    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              PM_ARMD_CUSTOM_WAVEFORM_BUF,
                                              &armData[0],
                                              ARM_DATA_COUNT,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Customer waveform data write error");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}
