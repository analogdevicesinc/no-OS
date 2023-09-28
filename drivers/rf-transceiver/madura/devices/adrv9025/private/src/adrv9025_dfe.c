// SPDX-License-Identifier: GPL-2.0
/**
* \file adrv9025_dfe.c
* \brief Contains ADRV9025 dfe related private function implementations
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025_cpu.h"
#include "../../private/include/adrv9025_tx.h"
#include "../../private/include/adrv9025_cpu.h"
#include "../../private/include/adrv9025_dfe.h"
#include "../../private/include/adrv9025_cpu_macros.h"

int32_t adrv9025_DpdModelConfigSetRangeCheck(adi_adrv9025_Device_t*         device,
                                             adi_adrv9025_DpdModelConfig_t* dpdModelConfig)
{
    static const uint8_t MAX_I_VAL = 15;
    static const uint8_t MAX_J_VAL = 15;
    static const uint8_t MAX_K_VAL = 15;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t featureIndex   = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        dpdModelConfig);

    if (dpdModelConfig->dpdNumFeatures > ADI_ADRV9025_MAX_DPD_FEATURES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdModelConfig->dpdNumFeatures,
                         "The no. of features for the DPD model cannot exceed ADI_ADRV9025_MAX_DPD_FEATURES");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (featureIndex = 0; featureIndex < dpdModelConfig->dpdNumFeatures; featureIndex++)
    {
        if (dpdModelConfig->dpdFeatures[featureIndex].i > MAX_I_VAL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             dpdModelConfig->dpdFeatures[featureIndex].i,
                             "The memory term(i) in the DPD feature exceeds the maximum allowed value");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if (dpdModelConfig->dpdFeatures[featureIndex].j > MAX_J_VAL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             dpdModelConfig->dpdFeatures[featureIndex].j,
                             "The cross term(j) in the DPD feature exceeds the maximum allowed value");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if (dpdModelConfig->dpdFeatures[featureIndex].k > MAX_K_VAL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             dpdModelConfig->dpdFeatures[featureIndex].k,
                             "The power term(k) in the DPD feature exceeds the maximum allowed value");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT0) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT1) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT2) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT3) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT4) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT5) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT6) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT7) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT8) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT9) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT10) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT11) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT12) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT13) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT14) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT15) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT16) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT17) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT18) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT19) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT20) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT21) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT22) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT23) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT24) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT25) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT26) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT27) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT28) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT29) &&
            (dpdModelConfig->dpdFeatures[featureIndex].lut != ADI_ADRV9025_DPD_LUT30))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             dpdModelConfig->dpdFeatures[featureIndex].lut,
                             "The LUT assignment in the DPD feature in the feature set is invalid");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_DpdModelConfigEncode(adi_adrv9025_Device_t*         device,
                                      adi_adrv9025_DpdModelConfig_t* dpdModelConfig,
                                      uint8_t                        armConfigData[],
                                      uint16_t                       arraySize)
{
    static const uint16_t ARM_CONFIG_DATA_SIZE_BYTES = (uint16_t)ADI_ADRV9025_NUM_BYTES_DPD_MODEL_CONFIG;
    static const uint8_t  DPD_BANK_INDEX             = 0; /* Value hardcoded for firmware. This is most likely to be removed for */
    static const uint8_t DPD_MODEL_INDEX             = 0; /* Value hardcoded for firmware. */

    int32_t  recoveryAction     = ADI_COMMON_ACT_NO_ACTION;
    uint16_t armConfigDataIndex = 0;
    uint8_t  featureIndex       = 0;
    uint8_t* fpSerializerPtr    = NULL;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common, dpdModelConfig);

    ADI_NULL_PTR_RETURN(&device->common, armConfigData);

    if (arraySize != ARM_CONFIG_DATA_SIZE_BYTES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "Invalid ARM config data size encountered while attempting to encode DPD model config");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Encode DPD model metadata */
    armConfigData[armConfigDataIndex++] = dpdModelConfig->dpdNumFeatures;
    armConfigData[armConfigDataIndex++] = DPD_MODEL_INDEX;
    armConfigData[armConfigDataIndex++] = DPD_BANK_INDEX;
    armConfigData[armConfigDataIndex++] = ADI_ADRV9025_DPD_ACT_COMPANDER_8_BITS;

    /* Encode valid DPD features */
    for (featureIndex = 0; featureIndex < dpdModelConfig->dpdNumFeatures; featureIndex++)
    {
        armConfigData[armConfigDataIndex++] = dpdModelConfig->dpdFeatures[featureIndex].i;
        armConfigData[armConfigDataIndex++] = dpdModelConfig->dpdFeatures[featureIndex].j;
        armConfigData[armConfigDataIndex++] = dpdModelConfig->dpdFeatures[featureIndex].k;
        armConfigData[armConfigDataIndex++] = (uint8_t)dpdModelConfig->dpdFeatures[featureIndex].lut;

        fpSerializerPtr                     = (uint8_t*)&dpdModelConfig->dpdFeatures[featureIndex].coeffReal;
        armConfigData[armConfigDataIndex++] = fpSerializerPtr[0];
        armConfigData[armConfigDataIndex++] = fpSerializerPtr[1];
        armConfigData[armConfigDataIndex++] = fpSerializerPtr[2];
        armConfigData[armConfigDataIndex++] = fpSerializerPtr[3];

        fpSerializerPtr                     = (uint8_t*)&dpdModelConfig->dpdFeatures[featureIndex].coeffImaginary;
        armConfigData[armConfigDataIndex++] = fpSerializerPtr[0];
        armConfigData[armConfigDataIndex++] = fpSerializerPtr[1];
        armConfigData[armConfigDataIndex++] = fpSerializerPtr[2];
        armConfigData[armConfigDataIndex++] = fpSerializerPtr[3];
    }

    /* Zero out uninitialized features */
    for (featureIndex = dpdModelConfig->dpdNumFeatures; featureIndex < (uint8_t)ADI_ADRV9025_MAX_DPD_FEATURES; featureIndex++)
    {
        /* Zero out the feature indices */
        armConfigData[armConfigDataIndex++] = (uint8_t)0;
        armConfigData[armConfigDataIndex++] = (uint8_t)0;
        armConfigData[armConfigDataIndex++] = (uint8_t)0;
        armConfigData[armConfigDataIndex++] = (uint8_t)0;

        /* Zero out the coefficients */
        armConfigData[armConfigDataIndex++] = (uint8_t)0;
        armConfigData[armConfigDataIndex++] = (uint8_t)0;
        armConfigData[armConfigDataIndex++] = (uint8_t)0;
        armConfigData[armConfigDataIndex++] = (uint8_t)0;

        armConfigData[armConfigDataIndex++] = (uint8_t)0;
        armConfigData[armConfigDataIndex++] = (uint8_t)0;
        armConfigData[armConfigDataIndex++] = (uint8_t)0;
        armConfigData[armConfigDataIndex++] = (uint8_t)0;
    }

    return recoveryAction;
}

int32_t adrv9025_DpdModelConfigDecode(adi_adrv9025_Device_t*         device,
                                      uint8_t                        armConfigData[],
                                      uint16_t                       arraySize,
                                      adi_adrv9025_DpdModelConfig_t* dpdModelConfig)
{
    static const uint16_t ARM_CONFIG_DATA_SIZE_BYTES = (uint16_t)ADI_ADRV9025_NUM_BYTES_DPD_MODEL_CONFIG;

    uint16_t armConfigDataIndex = 0;
    uint8_t  featureIndex       = 0;
    uint32_t fpDeserializerTemp = 0;
    float*   fpDeserializerPtr  = NULL;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        dpdModelConfig);

    ADI_NULL_PTR_RETURN(&device->common,
                        armConfigData);

    if (arraySize != ARM_CONFIG_DATA_SIZE_BYTES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "Invalid ARM config data size encountered while attempting to decode DPD model config");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Decode DPD model metadata */
    dpdModelConfig->dpdNumFeatures = armConfigData[armConfigDataIndex++];
    armConfigDataIndex++; /* Skip Model Index */
    armConfigDataIndex++; /* Skip Bank Index */
    armConfigDataIndex++; /* Skip Compander Size */

    /* Decode valid DPD features */
    for (featureIndex = 0; featureIndex < ADI_ADRV9025_MAX_DPD_FEATURES; featureIndex++)
    {
        dpdModelConfig->dpdFeatures[featureIndex].i   = armConfigData[armConfigDataIndex++];
        dpdModelConfig->dpdFeatures[featureIndex].j   = armConfigData[armConfigDataIndex++];
        dpdModelConfig->dpdFeatures[featureIndex].k   = armConfigData[armConfigDataIndex++];
        dpdModelConfig->dpdFeatures[featureIndex].lut = (adi_adrv9025_DpdLut_e)armConfigData[armConfigDataIndex++];

        fpDeserializerTemp = (uint32_t)((uint32_t)armConfigData[armConfigDataIndex] |
            ((uint32_t)armConfigData[armConfigDataIndex + 1] << 8) |
            ((uint32_t)armConfigData[armConfigDataIndex + 2] << 16) |
            ((uint32_t)armConfigData[armConfigDataIndex + 3] << 24));

        fpDeserializerPtr                                   = (float*)&fpDeserializerTemp;
        dpdModelConfig->dpdFeatures[featureIndex].coeffReal = *fpDeserializerPtr;
        armConfigDataIndex += 4;

        fpDeserializerTemp = (uint32_t)((uint32_t)armConfigData[armConfigDataIndex] |
            ((uint32_t)armConfigData[armConfigDataIndex + 1] << 8) |
            ((uint32_t)armConfigData[armConfigDataIndex + 2] << 16) |
            ((uint32_t)armConfigData[armConfigDataIndex + 3] << 24));

        fpDeserializerPtr                                        = (float*)&fpDeserializerTemp;
        dpdModelConfig->dpdFeatures[featureIndex].coeffImaginary = *fpDeserializerPtr;
        armConfigDataIndex += 4;
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_DpdStatusDeserialize(adi_adrv9025_Device_t*    device,
                                      adi_adrv9025_DpdStatus_t* dpdStatus,
                                      uint8_t*                  buf,
                                      uint8_t                   bufSize)
{
    static const uint8_t DPD_STATUS_SIZE    = 112u;
    uint32_t             fpDeserializerTemp = 0;
    float*               fpDeserializerPtr  = NULL;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        dpdStatus);
    ADI_NULL_PTR_RETURN(&device->common,
                        buf);

    if (bufSize < DPD_STATUS_SIZE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         size,
                         "Invalid buffer size. Buffer needs to be 112 bytes or larger");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    dpdStatus->dpdErrorCode = (adi_adrv9025_DpdError_e)(((uint32_t)buf[0]) |
        ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16) |
        ((uint32_t)buf[3] << 24));

    dpdStatus->dpdPercentComplete = (uint32_t)(((uint32_t)buf[4]) |
        ((uint32_t)buf[5] << 8) |
        ((uint32_t)buf[6] << 16) |
        ((uint32_t)buf[7] << 24));

    dpdStatus->dpdPerformanceMetric = (uint32_t)(((uint32_t)buf[8]) |
        ((uint32_t)buf[9] << 8) |
        ((uint32_t)buf[10] << 16) |
        ((uint32_t)buf[11] << 24));

    dpdStatus->dpdIterCount = (uint32_t)(((uint32_t)buf[12]) |
        ((uint32_t)buf[13] << 8) |
        ((uint32_t)buf[14] << 16) |
        ((uint32_t)buf[15] << 24));

    dpdStatus->dpdUpdateCount = (uint32_t)(((uint32_t)buf[16]) |
        ((uint32_t)buf[17] << 8) |
        ((uint32_t)buf[18] << 16) |
        ((uint32_t)buf[19] << 24));

    /* Not yet implemented in FW */
    /*dpdStatus->dpdCurrentMinTxSampleLevel = (uint32_t)(((uint32_t)buf[20]) |
                                                      ((uint32_t)buf[21] << 8) |
                                                      ((uint32_t)buf[22] << 16) |
                                                      ((uint32_t)buf[23] << 24));

    dpdStatus->dpdModelErrorAverage = (uint32_t)(((uint32_t)buf[24]) |
                                                ((uint32_t)buf[25] << 8) |
                                                ((uint32_t)buf[26] << 16) |
                                                ((uint32_t)buf[27] << 24));

    dpdStatus->dpdMaxAdaptationCurrent = (uint32_t)(((uint32_t)buf[28]) |
                                                   ((uint32_t)buf[29] << 8));

    dpdStatus->dpdMaxAdaptation = (uint32_t)(((uint32_t)buf[30]) |
                                             ((uint32_t)buf[31] << 8));

    dpdStatus->dpdDeltaActuatorDb = (uint32_t)(((uint32_t)buf[32]) |
                                              ((uint32_t)buf[33] << 8) |
                                              ((uint32_t)buf[34] << 16) |
                                              ((uint32_t)buf[35] << 24));*/

    dpdStatus->reservedPM = (uint32_t)(((uint32_t)buf[36]) |
        ((uint32_t)buf[37] << 8) |
        ((uint32_t)buf[38] << 16) |
        ((uint32_t)buf[39] << 24));

    dpdStatus->reservedTP = (uint32_t)(((uint32_t)buf[40]) |
        ((uint32_t)buf[41] << 8) |
        ((uint32_t)buf[42] << 16) |
        ((uint32_t)buf[43] << 24));

    dpdStatus->dpdModelTable = (adi_adrv9025_DpdModelTableSel_e)buf[44];

    /* Deserializing dpdMeanTuPower into the structure. */
    fpDeserializerTemp = (uint32_t)((uint32_t)buf[48] |
        ((uint32_t)buf[49] << 8) |
        ((uint32_t)buf[50] << 16) |
        ((uint32_t)buf[51] << 24));
    fpDeserializerPtr                       = (float*)&fpDeserializerTemp;
    dpdStatus->dpdStatistics.dpdMeanTuPower = *fpDeserializerPtr;

    /* Deserializing dpdPeakTuPower into the structure. */
    fpDeserializerTemp = (uint32_t)((uint32_t)buf[52] |
        ((uint32_t)buf[53] << 8) |
        ((uint32_t)buf[54] << 16) |
        ((uint32_t)buf[55] << 24));
    fpDeserializerPtr                       = (float*)&fpDeserializerTemp;
    dpdStatus->dpdStatistics.dpdPeakTuPower = *fpDeserializerPtr;

    /* Deserializing dpdMeanTxPower into the structure. */
    fpDeserializerTemp = (uint32_t)((uint32_t)buf[56] |
        ((uint32_t)buf[57] << 8) |
        ((uint32_t)buf[58] << 16) |
        ((uint32_t)buf[59] << 24));
    fpDeserializerPtr                       = (float*)&fpDeserializerTemp;
    dpdStatus->dpdStatistics.dpdMeanTxPower = *fpDeserializerPtr;

    /* Deserializing dpdPeakTxPower into the structure. */
    fpDeserializerTemp = (uint32_t)((uint32_t)buf[60] |
        ((uint32_t)buf[61] << 8) |
        ((uint32_t)buf[62] << 16) |
        ((uint32_t)buf[63] << 24));
    fpDeserializerPtr                       = (float*)&fpDeserializerTemp;
    dpdStatus->dpdStatistics.dpdPeakTxPower = *fpDeserializerPtr;

    /* Deserializing dpdMeanOrxPower into the structure. */
    fpDeserializerTemp = (uint32_t)((uint32_t)buf[64] |
        ((uint32_t)buf[65] << 8) |
        ((uint32_t)buf[66] << 16) |
        ((uint32_t)buf[67] << 24));
    fpDeserializerPtr                        = (float*)&fpDeserializerTemp;
    dpdStatus->dpdStatistics.dpdMeanOrxPower = *fpDeserializerPtr;

    /* Deserializing dpdPeakOrxPower into the structure. */
    fpDeserializerTemp = (uint32_t)((uint32_t)buf[68] |
        ((uint32_t)buf[69] << 8) |
        ((uint32_t)buf[70] << 16) |
        ((uint32_t)buf[71] << 24));
    fpDeserializerPtr                        = (float*)&fpDeserializerTemp;
    dpdStatus->dpdStatistics.dpdPeakOrxPower = *fpDeserializerPtr;

    /* Deserializing dpdDirectEvm into the structure. */
    fpDeserializerTemp = (uint32_t)((uint32_t)buf[72] |
        ((uint32_t)buf[73] << 8) |
        ((uint32_t)buf[74] << 16) |
        ((uint32_t)buf[75] << 24));
    fpDeserializerPtr                     = (float*)&fpDeserializerTemp;
    dpdStatus->dpdStatistics.dpdDirectEvm = *fpDeserializerPtr;

    /* Deserializing dpdIndirectEvm into the structure. */
    fpDeserializerTemp = (uint32_t)((uint32_t)buf[76] |
        ((uint32_t)buf[77] << 8) |
        ((uint32_t)buf[78] << 16) |
        ((uint32_t)buf[79] << 24));
    fpDeserializerPtr                       = (float*)&fpDeserializerTemp;
    dpdStatus->dpdStatistics.dpdIndirectEvm = *fpDeserializerPtr;

    /* Deserializing dpdSelectError into the structure. */
    fpDeserializerTemp = (uint32_t)((uint32_t)buf[80] |
        ((uint32_t)buf[81] << 8) |
        ((uint32_t)buf[82] << 16) |
        ((uint32_t)buf[83] << 24));
    fpDeserializerPtr                       = (float*)&fpDeserializerTemp;
    dpdStatus->dpdStatistics.dpdSelectError = *fpDeserializerPtr;

    /* Deserializing dpdIndirectError into the structure. */
    fpDeserializerTemp = (uint32_t)((uint32_t)buf[84] |
        ((uint32_t)buf[85] << 8) |
        ((uint32_t)buf[86] << 16) |
        ((uint32_t)buf[87] << 24));
    fpDeserializerPtr                         = (float*)&fpDeserializerTemp;
    dpdStatus->dpdStatistics.dpdIndirectError = *fpDeserializerPtr;

    dpdStatus->dpdErrorStatus0.dpdMetricsMask           = (uint16_t)(((uint16_t)buf[88]) | ((uint16_t)buf[89] << 8));
    dpdStatus->dpdPersistentErrorStatus0.dpdMetricsMask = (uint16_t)(((uint16_t)buf[90]) | ((uint16_t)buf[91] << 8));
    dpdStatus->dpdErrorStatus1.dpdMetricsMask           = (uint16_t)(((uint16_t)buf[92]) | ((uint16_t)buf[93] << 8));
    dpdStatus->dpdPersistentErrorStatus1.dpdMetricsMask = (uint16_t)(((uint16_t)buf[94]) | ((uint16_t)buf[95] << 8));

    dpdStatus->reservedPR = (uint32_t)(((uint32_t)buf[96]) |
        ((uint32_t)buf[97] << 8) |
        ((uint32_t)buf[98] << 16) |
        ((uint32_t)buf[99] << 24));

    dpdStatus->dpdErrorStatus0.dpdActionMask           = (uint16_t)(((uint16_t)buf[100]) | ((uint16_t)buf[101] << 8));
    dpdStatus->dpdPersistentErrorStatus0.dpdActionMask = (uint16_t)(((uint16_t)buf[102]) | ((uint16_t)buf[103] << 8));
    dpdStatus->dpdErrorStatus1.dpdActionMask           = (uint16_t)(((uint16_t)buf[104]) | ((uint16_t)buf[105] << 8));
    dpdStatus->dpdPersistentErrorStatus1.dpdActionMask = (uint16_t)(((uint16_t)buf[106]) | ((uint16_t)buf[107] << 8));

    dpdStatus->dpdSyncStatus = (adi_adrv9025_TrackingCalSyncStatus_e)(((uint32_t)buf[108]) |
        ((uint32_t)buf[109] << 8) |
        ((uint32_t)buf[110] << 16) |
        ((uint32_t)buf[111] << 24));

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_DpdActuatorGainMonitorConfigRangeCheck(adi_adrv9025_Device_t*                  device,
                                                        adi_adrv9025_DpdActGainMonitorConfig_t* dpdActGainMonitorConfig)
{
    static const uint8_t DPD_TRACKING_CONFIG_MAX_IIR_DECAY_VALUE = 16u;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        dpdActGainMonitorConfig);

    /* Check that the target Tx channel is valid */
    if (dpdActGainMonitorConfig->txChannelMask < ADI_ADRV9025_TX1 ||
        dpdActGainMonitorConfig->txChannelMask > ADI_ADRV9025_TXALL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelMask,
                         "Invalid Tx channel mask encountered while attempting to set DPD gain monitor config.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that model selection is valid */
    if ((dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorLowerThreshModelSel != ADI_ADRV9025_DPD_MODEL0) &&
        (dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorLowerThreshModelSel != ADI_ADRV9025_DPD_MODEL1) &&
        (dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorLowerThreshModelSel != ADI_ADRV9025_DPD_MODEL2) &&
        (dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorLowerThreshModelSel != ADI_ADRV9025_DPD_MODEL3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdGainMonitorLowerThreshModelSel,
                         "Invalid model select for low gain model Valid models, Model0/1/2/3.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that model selection is valid */
    if ((dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorUpperThreshModelSel != ADI_ADRV9025_DPD_MODEL0) &&
        (dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorUpperThreshModelSel != ADI_ADRV9025_DPD_MODEL1) &&
        (dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorUpperThreshModelSel != ADI_ADRV9025_DPD_MODEL2) &&
        (dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorUpperThreshModelSel != ADI_ADRV9025_DPD_MODEL3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdGainMonitorUpperThreshModelSel,
                         "Invalid model select for high gain model Valid models, Model0/1/2/3.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that IIRDecay value is valid */
    if (dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorIIRDecay > DPD_TRACKING_CONFIG_MAX_IIR_DECAY_VALUE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdGainMonitorIIRDecay,
                         "IIR decay should be equal or less than 16");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorEnable > 1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdGainMonitorEnable,
                         "dpdGainMonitorEnable should be either 0 or 1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdActGainMonitorConfig->dpdGainMonitorCtrl.dpdGainMonitorIIREnable > 1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdGainMonitorIIREnable,
                         "dpdGainMonitorIIREnable should be either 0 or 1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdActGainMonitorConfig->dpdGainMonitorCtrl.highGainModelAutoLoadEnable > 1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         highGainModelAutoLoadEnable,
                         "highGainModelAutoLoadEnable should be either 0 or 1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdActGainMonitorConfig->dpdGainMonitorCtrl.lowGainModelAutoLoadEnable > 1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         lowGainModelAutoLoadEnable,
                         "lowGainModelAutoLoadEnable should be either 0 or 1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_DpdTrackingConfigRangeCheck(adi_adrv9025_Device_t*            device,
                                             adi_adrv9025_DpdTrackingConfig_t* dpdTrackingConfig)
{
    static const uint8_t  DPD_TRACKING_CONFIG_MAX_REGULARIZATION_VALUE = 63u;
    static const uint32_t DPD_TRACKING_CONFIG_MAX_M_THRESH             = 2147483647u;
    static const uint32_t DPD_TRACKING_CONFIG_MAX_PEAK_WINDOW_SIZE     = 0x4B0000;
    static const uint16_t DPD_TRACKING_CONFIG_MAX_MIN_AVG_SIGNAL_LEVEL = 32768;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        dpdTrackingConfig);

    /* Check that the target Tx channel is valid */
    if (dpdTrackingConfig->txChannelMask < ADI_ADRV9025_TX1 ||
        dpdTrackingConfig->txChannelMask > ADI_ADRV9025_TXALL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->txChannelMask,
                         "Invalid Tx channel mask encountered while attempting to set DPD tracking config.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that update mode is valid */
    if ((dpdTrackingConfig->dpdUpdateMode != ADI_ADRV9025_DPD_TRACKING_UPDATE_MODE_0) &&
        (dpdTrackingConfig->dpdUpdateMode != ADI_ADRV9025_DPD_TRACKING_UPDATE_MODE_1) &&
        (dpdTrackingConfig->dpdUpdateMode != ADI_ADRV9025_DPD_TRACKING_UPDATE_MODE_2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->dpdUpdateMode,
                         "Invalid update mode encountered while attempting to set DPD tracking config.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that M threshold is valid */
    if (dpdTrackingConfig->dpdMThreshold >= DPD_TRACKING_CONFIG_MAX_M_THRESH)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->dpdMThreshold,
                         "M threshold should be less than 2147483648");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that indirect regularization value is valid */
    if (dpdTrackingConfig->dpdIndirectRegularizationValue >= DPD_TRACKING_CONFIG_MAX_REGULARIZATION_VALUE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->dpdIndirectRegularizationValue,
                         "Indirect regularization value should be less than 64");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that peak search window size is valid */
    if (dpdTrackingConfig->dpdPeakSearchWindowSize > DPD_TRACKING_CONFIG_MAX_PEAK_WINDOW_SIZE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->dpdPeakSearchWindowSize,
                         "Peak search window size value should be less than 0x4B0000");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdTrackingConfig->minAvgSignalLevel > DPD_TRACKING_CONFIG_MAX_MIN_AVG_SIGNAL_LEVEL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->minAvgSignalLevel,
                         "Min average signal level should be less than 32768");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdTrackingConfig->minAvgSignalLevelOrx > DPD_TRACKING_CONFIG_MAX_MIN_AVG_SIGNAL_LEVEL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->minAvgSignalLevelOrx,
                         "ORX Min average signal level should be less than 32768");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdTrackingConfig->dpdFilterSel > 1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->dpdFilterSel,
                         "OBW filter select valid values 0-1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if (dpdTrackingConfig->enableDirectLearning > 1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->enableDirectLearning,
                         "enable direct learning valid values 0 - disabled  and enabled - 1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if (dpdTrackingConfig->dpdMu > 100)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->dpdMu,
                         "Valid values for DPDmu are between 0-100");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if (dpdTrackingConfig->dpdIndirectRegularizationLowPowerValue > DPD_TRACKING_CONFIG_MAX_REGULARIZATION_VALUE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->dpdIndirectRegularizationLowPowerValue,
                         "Valid values for dpdIndirectRegularizationLowPowerValue are between 0-63");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_DpDFaultConditionRangeCheck(adi_adrv9025_Device_t*                  device,
                                             uint32_t                                txChannelMask,
                                             const adi_adrv9025_DpdFaultCondition_t* dpdFaultCond,
                                             const uint8_t                           numOfCond)
{
#if ADI_ADRV9025_DPD_RANGE_CHECK > 0
    uint8_t i = 0u;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        dpdFaultCond);

    if (numOfCond == 0u)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numOfCond,
                         "Number of fault conditions in the array cannot be 0.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that the target Tx channel is valid */
    if (txChannelMask < ADI_ADRV9025_TX1 ||
        txChannelMask > ADI_ADRV9025_TXALL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelMask,
                         "Invalid Tx channel mask encountered while attempting to set DPD Fault Condition.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0u; i < numOfCond; i++)
    {
        if (dpdFaultCond[i].dpdMetric != ADI_ADRV9025_DPD_METRIC_MEAN_TU_POWER &&
            dpdFaultCond[i].dpdMetric != ADI_ADRV9025_DPD_METRIC_PEAK_TU_POWER &&
            dpdFaultCond[i].dpdMetric != ADI_ADRV9025_DPD_METRIC_MEAN_TX_POWER &&
            dpdFaultCond[i].dpdMetric != ADI_ADRV9025_DPD_METRIC_PEAK_TX_POWER &&
            dpdFaultCond[i].dpdMetric != ADI_ADRV9025_DPD_METRIC_MEAN_ORX_POWER &&
            dpdFaultCond[i].dpdMetric != ADI_ADRV9025_DPD_METRIC_PEAK_ORX_POWER &&
            dpdFaultCond[i].dpdMetric != ADI_ADRV9025_DPD_METRIC_DIRECT_EVM &&
            dpdFaultCond[i].dpdMetric != ADI_ADRV9025_DPD_METRIC_INDIRECT_EVM &&
            dpdFaultCond[i].dpdMetric != ADI_ADRV9025_DPD_METRIC_SELECT_ERROR &&
            dpdFaultCond[i].dpdMetric != ADI_ADRV9025_DPD_METRIC_INDIRECT_ERROR)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             dpdFaultCond[i].dpdMetric,
                             "Invalid DPD metric");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if (dpdFaultCond[i].comparator != ADI_ADRV9025_DPD_COMPARATOR_LESS_THAN &&
            dpdFaultCond[i].comparator != ADI_ADRV9025_DPD_COMPARATOR_GREATER_THAN)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             dpdFaultCond[i].comparator,
                             "Invalid comparator");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
#endif

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_DpdRecoveryActionConfigRangeCheck(adi_adrv9025_Device_t*                        device,
                                                   uint32_t                                      txChannelMask,
                                                   const adi_adrv9025_DpdRecoveryActionConfig_t* recoveryActionConfig,
                                                   const uint8_t                                 numOfConfig)
{
#if ADI_ADRV9025_DPD_RANGE_CHECK > 0
    uint8_t i = 0u;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        recoveryActionConfig);

    if (numOfConfig == 0u)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numOfCond,
                         "Number of configurations in the array cannot be 0.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that the target Tx channel is valid */
    if (txChannelMask < ADI_ADRV9025_TX1 ||
        txChannelMask > ADI_ADRV9025_TXALL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid Tx channel mask encountered while attempting to set DPD Recovery action.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0u; i < numOfConfig; i++)
    {
        if (recoveryActionConfig[i].dpdErrorState != ADI_ADRV9025_DPD_ERR_STATE_0 &&
            recoveryActionConfig[i].dpdErrorState != ADI_ADRV9025_DPD_ERR_STATE_1 &&
            recoveryActionConfig[i].dpdErrorState != ADI_ADRV9025_DPD_ERR_STATE_PERSISTENT_0 &&
            recoveryActionConfig[i].dpdErrorState != ADI_ADRV9025_DPD_ERR_STATE_PERSISTENT_1)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             recoveryActionConfig[i].dpdErrorState,
                             "Invalid Error state");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
#endif

    return ADI_COMMON_ACT_NO_ACTION;
}

static uint8_t adrv9025_DpdFaultConditionOffsetGet(adi_adrv9025_DpdMetric_e metric)
{
    uint8_t offset = 0u;

    switch (metric)
    {
    case ADI_ADRV9025_DPD_METRIC_MEAN_TU_POWER:
        offset = 0u;
        break;
    case ADI_ADRV9025_DPD_METRIC_PEAK_TU_POWER:
        offset = 8u;
        break;
    case ADI_ADRV9025_DPD_METRIC_MEAN_TX_POWER:
        offset = 16u;
        break;
    case ADI_ADRV9025_DPD_METRIC_PEAK_TX_POWER:
        offset = 24u;
        break;
    case ADI_ADRV9025_DPD_METRIC_MEAN_ORX_POWER:
        offset = 32u;
        break;
    case ADI_ADRV9025_DPD_METRIC_PEAK_ORX_POWER:
        offset = 40u;
        break;
    case ADI_ADRV9025_DPD_METRIC_DIRECT_EVM:
        offset = 48u;
        break;
    case ADI_ADRV9025_DPD_METRIC_INDIRECT_EVM:
        offset = 56u;
        break;
    case ADI_ADRV9025_DPD_METRIC_SELECT_ERROR:
        offset = 64u;
        break;
    case ADI_ADRV9025_DPD_METRIC_INDIRECT_ERROR:
        offset = 72u;
        break;
    }

    return offset;
}

static uint8_t adrv9025_DpdRecoveryActionOffsetGet(adi_adrv9025_DpdErrorState_e dpdErrorState)
{
    uint8_t offset = 0u;

    switch (dpdErrorState)
    {
    case ADI_ADRV9025_DPD_ERR_STATE_0:
        offset = 80u;
        break;
    case ADI_ADRV9025_DPD_ERR_STATE_1:
        offset = 88u;
        break;
    case ADI_ADRV9025_DPD_ERR_STATE_PERSISTENT_0:
        offset = 84u;
        break;
    case ADI_ADRV9025_DPD_ERR_STATE_PERSISTENT_1:
        offset = 92u;
        break;
    }

    return offset;
}

int32_t adrv9025_DpdFaultConditionSet(adi_adrv9025_Device_t*                  device,
                                      uint32_t                                txChannelMask,
                                      const adi_adrv9025_DpdFaultCondition_t* dpdFaultCond)
{
    static const uint8_t DPD_SET_STABILITY_CONFIG_CTRL_PARAM = 0x14;
    static const uint8_t DPD_STABILITY_CONFIG_METRIC_LEN     = 8u;
    static const uint8_t ARM_ERR_CODE                        = 0x0E;

    int32_t recoveryAction   = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4]       = {ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_DPD_TRACKING, 0, DPD_SET_STABILITY_CONFIG_CTRL_PARAM};
    uint8_t armData[8u + 2u] = {0u};
    uint8_t cmdStatusByte    = 0u;
    uint8_t offset           = 0u;
    uint8_t i                = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        dpdFaultCond);

    recoveryAction = adrv9025_DpDFaultConditionRangeCheck(device,
                                                          txChannelMask,
                                                          dpdFaultCond,
                                                          1u);
    ADI_ERROR_RETURN(device->common.error.newAction);

    offset = adrv9025_DpdFaultConditionOffsetGet(dpdFaultCond->dpdMetric);

    armData[0] = offset;
    armData[1] = DPD_STABILITY_CONFIG_METRIC_LEN;
    armData[2] = (uint8_t)dpdFaultCond->comparator;
    armData[3] = 0u;
    armData[4] = (uint8_t)(dpdFaultCond->threshold0 & 0xFF);
    armData[5] = (uint8_t)((dpdFaultCond->threshold0 >> 8) & 0xFF);
    armData[6] = (uint8_t)(dpdFaultCond->threshold1 & 0xFF);
    armData[7] = (uint8_t)((dpdFaultCond->threshold1 >> 8) & 0xFF);
    armData[8] = (uint8_t)(dpdFaultCond->persistentCount & 0xFF);
    armData[9] = (uint8_t)((dpdFaultCond->persistentCount >> 8) & 0xFF);

    for (i = 0; i < ADRV9025_NUMBER_OF_TX_CHANNELS; i++)
    {
        if ((txChannelMask & (0x1 << i)) != 0)
        {
            extData[2] = 0x1 << i;

            /* Write data to ARM mailbox */
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

            /* Executing Arm set Command */
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
                                                           (uint32_t)ADI_ADRV9025_SETDPDFAULTCONDITION_TIMEOUT_US,
                                                           (uint32_t)ADI_ADRV9025_SETDPDFAULTCONDITION_INTERVAL_US);

            /* If cmdStatusByte is non-zero then flag an ARM error */
            if ((cmdStatusByte & ARM_ERR_CODE) > 0)
            {
                recoveryAction = adrv9025_CpuCmdErrorHandler(device,
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

int32_t adrv9025_DpdFaultConditionGet(adi_adrv9025_Device_t*            device,
                                      adi_adrv9025_TxChannels_e         txChannel,
                                      adi_adrv9025_DpdMetric_e          dpdMetric,
                                      adi_adrv9025_DpdFaultCondition_t* dpdFaultCond)
{
    static const uint8_t DPD_GET_STABILITY_CONFIG_CTRL_PARAM = 0x04;
    static const uint8_t DPD_STABILITY_CONFIG_METRIC_LEN     = 8u;
    static const uint8_t ARM_ERR_CODE                        = 0x0E;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4]     = {
        ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL,
        ADRV9025_CPU_OBJECTID_DPD_TRACKING,
        (uint8_t)txChannel,
        DPD_GET_STABILITY_CONFIG_CTRL_PARAM
    };
    uint8_t armData[8u]   = {0u};
    uint8_t cmdStatusByte = 0u;
    uint8_t offset        = 0u;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        dpdFaultCond);

    offset = adrv9025_DpdFaultConditionOffsetGet(dpdMetric);

    armData[0] = offset;
    armData[1] = DPD_STABILITY_CONFIG_METRIC_LEN;

    /* Write offset and size to ARM mailbox */
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

    /* Executing Arm Command */
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
                                                   (uint32_t)ADI_ADRV9025_GETDPDFAULTCONDITION_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETDPDFAULTCONDITION_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
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

    /* Read the data from ARM mailbox */
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

    /* Deserialize data tracking config */
    dpdFaultCond->dpdMetric       = dpdMetric;
    dpdFaultCond->comparator      = (adi_adrv9025_DpdComparator_e)armData[0];
    dpdFaultCond->threshold0      = (uint16_t)((uint16_t)armData[2] | ((uint16_t)armData[3] << 8));
    dpdFaultCond->threshold1      = (uint16_t)((uint16_t)armData[4] | ((uint16_t)armData[5] << 8));
    dpdFaultCond->persistentCount = (uint16_t)((uint16_t)armData[6] | ((uint16_t)armData[7] << 8));

    return recoveryAction;
}

int32_t adrv9025_DpdRecoveryActionSet(adi_adrv9025_Device_t*                        device,
                                      uint32_t                                      txChannelMask,
                                      const adi_adrv9025_DpdRecoveryActionConfig_t* recoveryActionConfig)
{
    static const uint8_t DPD_SET_STABILITY_CONFIG_CTRL_PARAM      = 0x14;
    static const uint8_t DPD_STABILITY_CONFIG_RECOVERY_ACTION_LEN = 4u;
    static const uint8_t ARM_ERR_CODE                             = 0x0E;

    int32_t recoveryAction   = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4u]      = {ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_DPD_TRACKING, 0, DPD_SET_STABILITY_CONFIG_CTRL_PARAM};
    uint8_t armData[4u + 2u] = {0u};
    uint8_t cmdStatusByte    = 0u;
    uint8_t offset           = 0u;
    uint8_t i                = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        recoveryActionConfig);

    recoveryAction = adrv9025_DpdRecoveryActionConfigRangeCheck(device,
                                                                txChannelMask,
                                                                recoveryActionConfig,
                                                                1u);
    ADI_ERROR_RETURN(device->common.error.newAction);

    offset = adrv9025_DpdRecoveryActionOffsetGet(recoveryActionConfig->dpdErrorState);

    armData[0] = offset;
    armData[1] = DPD_STABILITY_CONFIG_RECOVERY_ACTION_LEN;
    armData[2] = (uint8_t)(recoveryActionConfig->dpdRecoveryAction.dpdMetricsMask & 0xFF);
    armData[3] = (uint8_t)((recoveryActionConfig->dpdRecoveryAction.dpdMetricsMask >> 8) & 0xFF);
    armData[4] = (uint8_t)(recoveryActionConfig->dpdRecoveryAction.dpdActionMask & 0xFF);
    armData[5] = (uint8_t)((recoveryActionConfig->dpdRecoveryAction.dpdActionMask >> 8) & 0xFF);

    for (i = 0; i < ADRV9025_NUMBER_OF_TX_CHANNELS; i++)
    {
        if ((txChannelMask & (0x1 << i)) != 0)
        {
            extData[2] = 0x1 << i;

            /* Write data to ARM mailbox */
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

            /* Executing Arm set Command */
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
                                                           (uint32_t)ADI_ADRV9025_SETDPDRECOVERYACTION_TIMEOUT_US,
                                                           (uint32_t)ADI_ADRV9025_SETDPDRECOVERYACTION_INTERVAL_US);

            /* If cmdStatusByte is non-zero then flag an ARM error */
            if ((cmdStatusByte & ARM_ERR_CODE) > 0)
            {
                recoveryAction = adrv9025_CpuCmdErrorHandler(device,
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

int32_t adrv9025_DpdRecoveryActionGet(adi_adrv9025_Device_t*                  device,
                                      adi_adrv9025_TxChannels_e               txChannel,
                                      adi_adrv9025_DpdErrorState_e            dpdErrorState,
                                      adi_adrv9025_DpdRecoveryActionConfig_t* recoveryActionConfig)
{
    static const uint8_t DPD_GET_STABILITY_CONFIG_CTRL_PARAM      = 0x04;
    static const uint8_t DPD_STABILITY_CONFIG_RECOVERY_ACTION_LEN = 4u;
    static const uint8_t ARM_ERR_CODE                             = 0x0E;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4u]    = {
        ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL,
        ADRV9025_CPU_OBJECTID_DPD_TRACKING,
        (uint8_t)txChannel,
        DPD_GET_STABILITY_CONFIG_CTRL_PARAM
    };
    uint8_t armData[4u]   = {0u};
    uint8_t cmdStatusByte = 0u;
    uint8_t offset        = 0u;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        recoveryActionConfig);

    offset = adrv9025_DpdRecoveryActionOffsetGet(dpdErrorState);

    armData[0] = offset;
    armData[1] = DPD_STABILITY_CONFIG_RECOVERY_ACTION_LEN;

    /* Write offset and size to ARM mailbox */
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

    /* Executing Arm Command */
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
                                                   (uint32_t)ADI_ADRV9025_GETDPDRECOVERYACTION_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETDPDRECOVERYACTION_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
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

    /* Read the data from ARM mailbox */
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

    /* Deserialize data tracking config */
    recoveryActionConfig->dpdErrorState                    = dpdErrorState;
    recoveryActionConfig->dpdRecoveryAction.dpdMetricsMask = (uint16_t)((uint16_t)armData[0] | ((uint16_t)armData[1] << 8));
    recoveryActionConfig->dpdRecoveryAction.dpdActionMask  = (uint16_t)((uint16_t)armData[2] | ((uint16_t)armData[3] << 8));

    return recoveryAction;
}

int32_t adrv9025_CfrHardClipperConfigSetRangeCheck(adi_adrv9025_Device_t*              device,
                                                   adi_adrv9025_CfrHardClipperConfig_t cfrHardClipperConfig[],
                                                   uint8_t                             cfrHardClipperCfgArraySize)
{
    static const float CFR_HARD_CLIPPER_MIN_THRESH = 0.0;
    static const float CFR_HARD_CLIPPER_MAX_THRESH = 1.0;
    uint8_t            i                           = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        cfrHardClipperConfig);

    for (i = 0; i < cfrHardClipperCfgArraySize; i++)
    {
        if ((cfrHardClipperConfig[i].cfrHardClipperEnable > 0) &&
            (cfrHardClipperConfig[i].txChannelMask != (uint32_t)ADI_ADRV9025_TXOFF))
        {
            if (cfrHardClipperConfig[i].cfrHardClipperThreshold > CFR_HARD_CLIPPER_MAX_THRESH)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 cfrHardClipperConfig[i].cfrHardClipperThreshold,
                                 "Invalid CFR Hard clipper threshold encountered. Valid range is 0 to 1");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (cfrHardClipperConfig[i].cfrHardClipperThreshold < CFR_HARD_CLIPPER_MIN_THRESH)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 cfrHardClipperConfig[i].cfrHardClipperThreshold,
                                 "Invalid CFR Hard clipper threshold encountered. Valid range is 0 to 1");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_CfrCtrlConfigSetRangeCheck(adi_adrv9025_Device_t*       device,
                                            adi_adrv9025_CfrCtrlConfig_t cfrCtrlConfig[],
                                            uint8_t                      cfrCtrlCfgArraySize)
{
    static const float    MIN_CFR_THRESH   = 0.0;
    static const float    MAX_CFR_THRESH   = 1.0;
    static const uint16_t MAX_CFR_TX_DELAY = 511u;
    static const uint16_t MIN_CFR_TX_DELAY = 129u;
    uint8_t               cfrCfgCtrlIndex  = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        cfrCtrlConfig);

    for (cfrCfgCtrlIndex = 0; cfrCfgCtrlIndex < cfrCtrlCfgArraySize; cfrCfgCtrlIndex++)
    {
        if (cfrCtrlConfig[cfrCfgCtrlIndex].cfrMode != ADI_ADRV9025_CFR_MODE1)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             cfrCtrlConfig[cfrCfgCtrlIndex].cfrMode,
                             "Invalid CFR mode encountered while attempting to program the CFR control config. Only Mode1 is supported.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((cfrCtrlConfig[cfrCfgCtrlIndex].cfrInterpolationFactor != ADI_ADRV9025_CFR_INTERPOLATION_1X) &&
            (cfrCtrlConfig[cfrCfgCtrlIndex].cfrInterpolationFactor != ADI_ADRV9025_CFR_INTERPOLATION_2X) &&
            (cfrCtrlConfig[cfrCfgCtrlIndex].cfrInterpolationFactor != ADI_ADRV9025_CFR_INTERPOLATION_4X))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             cfrCtrlConfig[cfrCfgCtrlIndex].cfrInterpolationFactor,
                             "Invalid CFR interpolation factor encountered while attempting to program the CFR control config. Interpolation factors supported are 1x, 2x and 4x.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((cfrCtrlConfig[cfrCfgCtrlIndex].cfrPeakThreshold < MIN_CFR_THRESH) ||
            (cfrCtrlConfig[cfrCfgCtrlIndex].cfrPeakThreshold > MAX_CFR_THRESH))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             cfrCtrlConfig[cfrCfgCtrlIndex].cfrPeakThreshold,
                             "Invalid CFR peak threshold encountered while attempting to program the CFR control config. Peak threshold is expected to be in the range 0 to 1.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((cfrCtrlConfig[cfrCfgCtrlIndex].cfrEngine1PeakThresholdScaler < MIN_CFR_THRESH) ||
            (cfrCtrlConfig[cfrCfgCtrlIndex].cfrEngine1PeakThresholdScaler > MAX_CFR_THRESH))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             cfrCtrlConfig[cfrCfgCtrlIndex].cfrEngine1PeakThresholdScaler,
                             "Invalid CFR engine 1 peak threshold scaler encountered while attempting to program the CFR control config. Engine 1 peak threshold scaler is expected to be in the range 0 to 1.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((cfrCtrlConfig[cfrCfgCtrlIndex].cfrEngine2PeakThresholdScaler < MIN_CFR_THRESH) ||
            (cfrCtrlConfig[cfrCfgCtrlIndex].cfrEngine2PeakThresholdScaler > MAX_CFR_THRESH))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             cfrCtrlConfig[cfrCfgCtrlIndex].cfrEngine2PeakThresholdScaler,
                             "Invalid CFR engine 2 peak threshold scaler encountered while attempting to program the CFR control config. Engine 2 peak threshold scaler is expected to be in the range 0 to 1.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((cfrCtrlConfig[cfrCfgCtrlIndex].cfrEngine3PeakThresholdScaler < MIN_CFR_THRESH) ||
            (cfrCtrlConfig[cfrCfgCtrlIndex].cfrEngine3PeakThresholdScaler > MAX_CFR_THRESH))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             cfrCtrlConfig[cfrCfgCtrlIndex].cfrEngine3PeakThresholdScaler,
                             "Invalid CFR engine 3 peak threshold scaler encountered while attempting to program the CFR control config. Engine 3 peak threshold scaler is expected to be in the range 0 to 1.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((cfrCtrlConfig[cfrCfgCtrlIndex].cfrCorrectionThresholdScaler < MIN_CFR_THRESH) ||
            (cfrCtrlConfig[cfrCfgCtrlIndex].cfrCorrectionThresholdScaler > MAX_CFR_THRESH))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             cfrCtrlConfig[cfrCfgCtrlIndex].cfrCorrectionThresholdScaler,
                             "Invalid CFR correction threshold threshold scaler encountered while attempting to program the CFR control config. Correction threshold scaler is expected to be in the range 0 to 1.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if (cfrCtrlConfig[cfrCfgCtrlIndex].cfrTxDelay > MAX_CFR_TX_DELAY ||
            cfrCtrlConfig[cfrCfgCtrlIndex].cfrTxDelay < MIN_CFR_TX_DELAY)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             cfrCtrlConfig[cfrCfgCtrlIndex].cfrTxDelay,
                             "Invalid CFR Tx delay while attempting to program the CFR control config. Tx delay is expected to be in the range 129 to 511.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if (cfrCtrlConfig[cfrCfgCtrlIndex].cfrEngine1MaxNumOfPeaks > 5 ||
            cfrCtrlConfig[cfrCfgCtrlIndex].cfrEngine2MaxNumOfPeaks > 5 ||
            cfrCtrlConfig[cfrCfgCtrlIndex].cfrEngine3MaxNumOfPeaks > 5)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             maxNumberOfPeaks,
                             "Invalid maximum number of peaks parameters. Valid range is [0-5]");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_CfrCorrectionPulseWriteRangeCheck(adi_adrv9025_Device_t*            device,
                                                   adi_adrv9025_CfrCorrectionPulse_t cfrCorrectionPulses[],
                                                   uint8_t                           numCorrectionPulses)
{
    static const uint8_t MAX_CORRECTION_PULSES = 2;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        cfrCorrectionPulses);

    if (numCorrectionPulses == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numCorrectionPulses,
                         "A minimum of 1 final CFR correction pulse expected for programming the correction pulses");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (numCorrectionPulses > MAX_CORRECTION_PULSES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numCorrectionPulses,
                         "A maximum of 2 final CFR correction pulses are allowed to be programmed");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((numCorrectionPulses == 1) &&
        (cfrCorrectionPulses[0].numCoeffs > (uint16_t)ADI_ADRV9025_MAX_CFR_CORRECTION_PULSE_LEN))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         cfrCorrectionPulses[0].numCoeffs,
                         "No. of coefficients in the CFR correction pulse exceeds the maximum allowed value of 512");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((numCorrectionPulses == 2) &&
        (cfrCorrectionPulses[0].numCoeffs > ((uint16_t)ADI_ADRV9025_MAX_CFR_CORRECTION_PULSE_LEN / 2)))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         cfrCorrectionPulses[0].numCoeffs,
                         "No. of coefficients in the CFR correction pulse exceeds the maximum allowed value of 256 for programming more than 1 correction pulse");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((numCorrectionPulses == 2) &&
        (cfrCorrectionPulses[1].numCoeffs > ((uint16_t)ADI_ADRV9025_MAX_CFR_CORRECTION_PULSE_LEN / 2)))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         cfrCorrectionPulses[1].numCoeffs,
                         "No. of coefficients in the CFR correction pulse exceeds the maximum allowed value of 256 for programming more than 1 correction pulse");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((numCorrectionPulses == 2) &&
        (cfrCorrectionPulses[0].numCoeffs != cfrCorrectionPulses[1].numCoeffs))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         cfrCorrectionPulses[1].numCoeffs,
                         "The half pulse length of the two correction pulses are required to be the same");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_CfrActiveCorrectionPulseSetRangeCheck(adi_adrv9025_Device_t*                       device,
                                                       uint32_t                                     txChannelMask,
                                                       adi_adrv9025_CfrCarrierHotSwapCorrPulseSel_e cfrCorrectionPulseSel)
{
    static const uint8_t CFR_MODE_STATUS_OFFSET = 20;
    static const uint8_t CPU_ERR_CODE           = 0x0E;
    static const uint8_t CPU_DMA_AUTO_INCR      = 0x01;
    static const uint8_t PROGRAM_PULSE_MODE1    = 0;
    static const uint8_t PROGRAM_PULSE_MODE1A   = 3;

    int32_t                 recoveryAction      = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                 txChannelIndex      = 0;
    uint16_t                halfPulseLen        = 0;
    uint32_t                activeTxChannelMask = (uint32_t)ADI_ADRV9025_TX1;
    uint8_t                 extData[3]          = {ADRV9025_CPU_OBJECTID_CAL_STATUS, ADRV9025_CPU_OBJECTID_CFR_INIT, 0};
    uint32_t                cfrModeStatus       = 0;
    uint8_t                 cmdStatusByte       = 0;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr   = (adrv9025_BfTxChanAddr_e)0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    if ((cfrCorrectionPulseSel != ADI_ADRV9025_CFR_CARRIER_HOT_SWAP_CORR_PULSE_1) &&
        (cfrCorrectionPulseSel != ADI_ADRV9025_CFR_CARRIER_HOT_SWAP_CORR_PULSE_2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         cfrCorrectionPulseSel,
                         "Invalid target correction pulse requested to be activated. Valid correction pulses are ADI_ADRV9025_CFR_CARRIER_HOT_SWAP_CORR_PULSE_1/ADI_ADRV9025_CFR_CARRIER_HOT_SWAP_CORR_PULSE_2");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (txChannelIndex = 0; txChannelIndex < ADI_ADRV9025_MAX_TXCHANNELS; txChannelIndex++)
    {
        activeTxChannelMask = ((uint32_t)ADI_ADRV9025_TX1 << txChannelIndex);

        if ((activeTxChannelMask & txChannelMask) != ADI_ADRV9025_TXOFF)
        {
            /* Retrieve the mode info and ensure that the CFR mode configured is MODE1 */
            /* Executing CFR status get Cpu Command */
            extData[2]     = txChannelIndex;
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
            if ((cmdStatusByte & CPU_ERR_CODE) > 0)
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
                             device->common.error.errSource,
                             device->common.error.errCode,
                             device->common.error.newAction,
                             device->common.error.varName,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Read the CFR mode status from CPU mailbox */
            recoveryAction = adi_adrv9025_CpuMemRead(device,
                                                     (uint32_t)(ADRV9025_CPU_C_ADDR_MAILBOX_GET + CFR_MODE_STATUS_OFFSET),
                                                     (uint8_t*)&cfrModeStatus,
                                                     (uint8_t)sizeof(cfrModeStatus),
                                                     CPU_DMA_AUTO_INCR);
            ADI_ERROR_REPORT(&device->common,
                             device->common.error.errSource,
                             device->common.error.errCode,
                             device->common.error.newAction,
                             device->common.error.varName,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (((uint8_t)cfrModeStatus != PROGRAM_PULSE_MODE1) && ((uint8_t)cfrModeStatus != PROGRAM_PULSE_MODE1A))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 cfrModeStatus,
                                 "CFR must be configured to operate in ADI_ADRV9025_CFR_MODE1 for runtime switching of correction pulses");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* Decode the Tx channel bitfield address */
            txChannelBaseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                               activeTxChannelMask);
            ADI_ERROR_REPORT(&device->common,
                             device->common.error.errSource,
                             device->common.error.errCode,
                             device->common.error.newAction,
                             device->common.error.varName,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Retrieve the half pulse length programmed in the device */
            recoveryAction = adrv9025_TxCfrHalfPulseLenBfGet(device,
                                                             txChannelBaseAddr,
                                                             &halfPulseLen);
            ADI_ERROR_REPORT(&device->common,
                             device->common.error.errSource,
                             device->common.error.errCode,
                             device->common.error.newAction,
                             device->common.error.varName,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Return an error if pulse 2 is requested to be activated and a single correction pulse is programmed */
            if ((halfPulseLen > ((uint16_t)ADI_ADRV9025_MAX_CFR_CORRECTION_PULSE_LEN / 2)) &&
                (cfrCorrectionPulseSel == ADI_ADRV9025_CFR_CARRIER_HOT_SWAP_CORR_PULSE_2))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 halfPulseLen,
                                 "CFR correction pulse switching to pulse 2 cannot be executed since CFR engine is configured with single correction pulse of (half length > 256)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return recoveryAction;
}

int32_t adrv9025_CfrCtrlCmdGet(adi_adrv9025_Device_t*    device,
                               adi_adrv9025_TxChannels_e txChannel,
                               uint8_t                   subCmdId,
                               uint8_t                   armCtrlData[],
                               uint8_t                   armCtrlDataSize)
{
    static const uint8_t ARM_ERR_CODE = 0x0E;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4]     = {ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_CFR_INIT, (uint8_t)txChannel, subCmdId};
    uint8_t cmdStatusByte  = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        armCtrlData);

    /* Executing Set Arm Command */
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
                                                   (uint32_t)ADI_ADRV9025_SETCFRCTRLCFG_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_SETCFRCTRLCFG_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
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

    /* Read the ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                                             &armCtrlData[0],
                                             armCtrlDataSize,
                                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armCtrlData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_CfrCtrlCmdExecute(adi_adrv9025_Device_t* device,
                                   uint8_t                txChannelMask,
                                   uint8_t                subCmdId,
                                   uint8_t                armCtrlData[],
                                   uint8_t                armCtrlDataSize)
{
    static const uint8_t ARM_ERR_CODE    = 0x0E;
    static const uint8_t CAL_CTRL_SET_BF = 0x80;

    int32_t recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4]        = {ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_CFR_INIT, (uint8_t)ADI_ADRV9025_TXOFF, CAL_CTRL_SET_BF};
    uint8_t cmdStatusByte     = 0;
    uint8_t txChannelIndex    = 0;
    uint8_t loopTxChannelMask = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        armCtrlData);

    for (txChannelIndex = 0; txChannelIndex < ADI_ADRV9025_MAX_TXCHANNELS; txChannelIndex++)
    {
        loopTxChannelMask = (1 << txChannelIndex);

        if ((loopTxChannelMask & (uint8_t)txChannelMask) > 0)
        {
            extData[2] = (loopTxChannelMask & (uint8_t)ADI_ADRV9025_TXALL);
            extData[3] |= subCmdId;

            /* Write the CFR ctrl config to ARM mailbox */
            recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                                      (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                                                      &armCtrlData[0],
                                                      armCtrlDataSize,
                                                      ADI_ADRV9025_CPU_MEM_AUTO_INCR);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             armCtrlData,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Executing CFR Hard clipper set Arm Command */
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
                                                           (uint32_t)ADI_ADRV9025_SETCFRCTRLCFG_TIMEOUT_US,
                                                           (uint32_t)ADI_ADRV9025_SETCFRCTRLCFG_INTERVAL_US);

            /* If cmdStatusByte is non-zero then flag an ARM error */
            if ((cmdStatusByte & ARM_ERR_CODE) > 0)
            {
                recoveryAction = adrv9025_CpuCmdErrorHandler(device,
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

int32_t adrv9025_CfrStatisticsGet(adi_adrv9025_Device_t*        device,
                                  adi_adrv9025_TxChannels_e     txChannel,
                                  adi_adrv9025_CfrStatistics_t* cfrStatistics)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR    = 1;
    static const uint8_t ARM_ERR_CODE             = 0x0E;
    static const uint8_t CFR_ENGINE_STATISTICS_ID = 16;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4]     = {ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_CFR_INIT, (uint8_t)txChannel, CFR_ENGINE_STATISTICS_ID};
    uint8_t armData[64]    = {0};
    uint8_t cmdStatusByte  = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        cfrStatistics);

    /* Executing CFR statistics get Arm Command */
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
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
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

    /* Read the CFR status from ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET,
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

    /* Deserializing CFR Statistics threshold into the structure. */
    cfrStatistics->cfrEngine1PeaksDetected = (uint64_t)(((uint64_t)armData[8]) |
        ((uint64_t)armData[9] << 8) |
        ((uint64_t)armData[10] << 16) |
        ((uint64_t)armData[11] << 24) |
        ((uint64_t)armData[12] << 32) |
        ((uint64_t)armData[13] << 40) |
        ((uint64_t)armData[14] << 48) |
        ((uint64_t)armData[15] << 56));

    cfrStatistics->cfrEngine2PeaksDetected = (uint64_t)(((uint64_t)armData[16]) |
        ((uint64_t)armData[17] << 8) |
        ((uint64_t)armData[18] << 16) |
        ((uint64_t)armData[19] << 24) |
        ((uint64_t)armData[20] << 32) |
        ((uint64_t)armData[21] << 40) |
        ((uint64_t)armData[22] << 48) |
        ((uint64_t)armData[23] << 56));

    cfrStatistics->cfrEngine3PeaksDetected = (uint64_t)(((uint64_t)armData[24]) |
        ((uint64_t)armData[25] << 8) |
        ((uint64_t)armData[26] << 16) |
        ((uint64_t)armData[27] << 24) |
        ((uint64_t)armData[28] << 32) |
        ((uint64_t)armData[29] << 40) |
        ((uint64_t)armData[30] << 48) |
        ((uint64_t)armData[31] << 56));

    cfrStatistics->cfrEngine1PeaksSkipped = (uint64_t)(((uint64_t)armData[32]) |
        ((uint64_t)armData[33] << 8) |
        ((uint64_t)armData[34] << 16) |
        ((uint64_t)armData[35] << 24) |
        ((uint64_t)armData[36] << 32) |
        ((uint64_t)armData[37] << 40) |
        ((uint64_t)armData[38] << 48) |
        ((uint64_t)armData[39] << 56));

    cfrStatistics->cfrEngine2PeaksSkipped = (uint64_t)(((uint64_t)armData[40]) |
        ((uint64_t)armData[41] << 8) |
        ((uint64_t)armData[42] << 16) |
        ((uint64_t)armData[43] << 24) |
        ((uint64_t)armData[44] << 32) |
        ((uint64_t)armData[45] << 40) |
        ((uint64_t)armData[46] << 48) |
        ((uint64_t)armData[47] << 56));

    cfrStatistics->cfrEngine3PeaksSkipped = (uint64_t)(((uint64_t)armData[48]) |
        ((uint64_t)armData[49] << 8) |
        ((uint64_t)armData[50] << 16) |
        ((uint64_t)armData[51] << 24) |
        ((uint64_t)armData[52] << 32) |
        ((uint64_t)armData[53] << 40) |
        ((uint64_t)armData[54] << 48) |
        ((uint64_t)armData[55] << 56));

    cfrStatistics->cfrNumSamplesClipped = (uint64_t)(((uint64_t)armData[56]) |
        ((uint64_t)armData[57] << 8) |
        ((uint64_t)armData[58] << 16) |
        ((uint64_t)armData[59] << 24) |
        ((uint64_t)armData[60] << 32) |
        ((uint64_t)armData[61] << 40) |
        ((uint64_t)armData[62] << 48) |
        ((uint64_t)armData[63] << 56));

    return (device->common.error.newAction);
}

int32_t adrv9025_CfrPulseCoeffRead(adi_adrv9025_Device_t*    device,
                                   adi_adrv9025_TxChannels_e txChannel,
                                   uint16_t                  pulseCoeffIndex,
                                   int16_t*                  pulseCoeffReal,
                                   int16_t*                  pulseCoeffImg)
{
    static const uint8_t  ARM_ERR_MASK          = 0x0E;
    static const uint8_t  AUTO_INC_BIT          = 0;
    static const uint8_t  NUM_BYTES_PER_COEFF   = 4;
    static const uint16_t MAX_PULSE_COEFF_INDEX = 511;

    int32_t  recoveryAction           = ADI_COMMON_ACT_NO_ACTION;
    uint8_t  extendedData[4]          = {0};
    uint8_t  armCfgData[4]            = {0};
    uint8_t  cmdStatusByte            = 0;
    uint32_t txPulseCoeffReadCtrlWord = 0;
    uint16_t byteOffset               = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        pulseCoeffReal);

    ADI_NULL_PTR_RETURN(&device->common,
                        pulseCoeffImg);

    /* Ensure that the requested coeff index is less than the maximum allowed size of 512 */
    if (pulseCoeffIndex > MAX_PULSE_COEFF_INDEX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pulseCoeffIndex,
                         "Invalid coefficient index encountered while attempting to read CFR correction pulse coefficient");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Resolve the pulse coefficient base address */
    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        txPulseCoeffReadCtrlWord = 0x00;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        txPulseCoeffReadCtrlWord = 0x01;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        txPulseCoeffReadCtrlWord = 0x02;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        txPulseCoeffReadCtrlWord = 0x03;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid Tx channel encountered while attempting to read CFR pulse coefficients");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    extendedData[0] = NUM_BYTES_PER_COEFF;
    extendedData[1] = txPulseCoeffReadCtrlWord;
    extendedData[2] = 0;
    extendedData[3] = 0;
    recoveryAction  = adi_adrv9025_CpuMemWrite(device,
                                               ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                                               &extendedData[0],
                                               2,
                                               ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "ArmConfigRead failed to write ARM mem");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* ARM Object id, byte offset LSB, offset MSB = 0, byteCount will read that number of bytes */
    byteOffset      = (pulseCoeffIndex * NUM_BYTES_PER_COEFF);
    extendedData[0] = ADRV9025_CPU_OBJECTID_CONFIG;
    extendedData[1] = ADRV9025_CPU_OBJECTID_CFR_INIT;
    extendedData[2] = (byteOffset & 0xFF);
    extendedData[3] = ((byteOffset >> 8) & 0xFF);

    /* send the read config ARM opcode */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              ADRV9025_CPU_GET_OPCODE,
                                              &extendedData[0],
                                              4);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* check the command status for timeout */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   ADI_ADRV9025_READCPUCFG_TIMEOUT_US,
                                                   ADI_ADRV9025_READCPUCFG_INTERVAL_US);

    if ((cmdStatusByte & ARM_ERR_MASK) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
                                                     ADI_ADRV9025_CPU_TYPE_C,
                                                     ADI_ADRV9025_SRC_CPUCMD,
                                                     ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                                              ADRV9025_CPU_OBJECTID_CONFIG,
                                                                              cmdStatusByte),
                                                     recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         device->common.error.errSource,
                         device->common.error.errCode,
                         device->common.error.newAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                                             &armCfgData[0],
                                             sizeof(armCfgData),
                                             AUTO_INC_BIT);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read the cfr correction pulse coefficient byte stream from the ARM mailbox and decode it into the pulse coefficients */
    *pulseCoeffReal = ((uint16_t)armCfgData[0] |
                       (uint16_t)armCfgData[1] << 8);

    *pulseCoeffImg = -((uint16_t)armCfgData[2] |
                      (uint16_t)armCfgData[3] << 8);

    return (device->common.error.newAction);
}

int32_t adrv9025_CfrCorrectionPulsesConcatenate(adi_adrv9025_Device_t*             device,
                                                adi_adrv9025_CfrCorrectionPulse_t  cfrCorrPulses[],
                                                uint8_t                            numCorrectionPulses,
                                                adi_adrv9025_CfrCorrectionPulse_t* cfrCorrPulseConcatenated)
{
    uint8_t  correctionPulseIndex = 0;
    uint16_t coeffIndex           = 0;
    uint16_t coeffOffset          = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        cfrCorrPulses);

    ADI_NULL_PTR_RETURN(&device->common,
                        cfrCorrPulseConcatenated);

    if (numCorrectionPulses == 1)
    {
        for (correctionPulseIndex = 0; correctionPulseIndex < 2; correctionPulseIndex++)
        {
            for (coeffIndex = 0; coeffIndex < cfrCorrPulses[0].numCoeffs; coeffIndex++)
            {
                cfrCorrPulseConcatenated->coeffRealHalfPulse[coeffOffset + coeffIndex]      = cfrCorrPulses[0].coeffRealHalfPulse[coeffIndex];
                cfrCorrPulseConcatenated->coeffImaginaryHalfPulse[coeffOffset + coeffIndex] = cfrCorrPulses[0].coeffImaginaryHalfPulse[coeffIndex];
                cfrCorrPulseConcatenated->numCoeffs++;
            }

            coeffOffset += cfrCorrPulses[0].numCoeffs;
        }
    }
    else
    {
        for (correctionPulseIndex = 0; correctionPulseIndex < numCorrectionPulses; correctionPulseIndex++)
        {
            for (coeffIndex = 0; coeffIndex < cfrCorrPulses[correctionPulseIndex].numCoeffs; coeffIndex++)
            {
                cfrCorrPulseConcatenated->coeffRealHalfPulse[coeffOffset + coeffIndex] = cfrCorrPulses[correctionPulseIndex].coeffRealHalfPulse[coeffIndex];
                cfrCorrPulseConcatenated->coeffImaginaryHalfPulse[coeffOffset + coeffIndex] = cfrCorrPulses[correctionPulseIndex].coeffImaginaryHalfPulse[coeffIndex];
                cfrCorrPulseConcatenated->numCoeffs++;
            }

            coeffOffset += cfrCorrPulses[correctionPulseIndex].numCoeffs;
        }
    }

    return (device->common.error.newAction);
}

int32_t adrv9025_ClgcConfigSetRangeCheck(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_ClgcConfig_t clgcConfig[],
                                         uint8_t                   clgcConfigArraySize)
{
    static const float MAX_TX_ATTEN_STEP_SIZE_dB = 6.0;

    uint8_t clgcConfigIndex = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that clgcConfig is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        clgcConfig);

    for (clgcConfigIndex = 0; clgcConfigIndex < clgcConfigArraySize; clgcConfigIndex++)
    {
        if ((clgcConfig[clgcConfigIndex].clgcMaxGainAdjustmentStepSize_dB <= 0) ||
            (clgcConfig[clgcConfigIndex].clgcMaxGainAdjustmentStepSize_dB > MAX_TX_ATTEN_STEP_SIZE_dB))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             clgcConfig[clgcConfigIndex].clgcMaxGainAdjustmentStepSize_dB,
                             "Invalid Tx atten adjustment step requested for CLGC control loop. The step size must be in the range 0-6dB");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if (clgcConfig[clgcConfigIndex].clgcMinTxAttenAdjust_dB > clgcConfig[clgcConfigIndex].clgcMaxTxAttenAdjust_dB)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             clgcConfig[clgcConfigIndex].clgcMinTxAttenAdjust_dB,
                             "Minimum Tx attenuation adjustment cannot be larger than the maximum value configured");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return (device->common.error.newAction);
}

int32_t adrv9025_ClgcConfigSet(adi_adrv9025_Device_t*     device,
                               adi_adrv9025_ClgcConfig_t* clgcConfig)
{
#define ADRV9025_CLGC_CONFIG_SIZE_BYTES 52u /* Last 3 variables in config struct shouldn't be written by API. But offset should remain 56 */
#define ADRV9025_CLGC_CONFIG_OFFSET_BYTES 56u
    static const uint8_t                   PA_PROTECTION_EN                              = 1u;
    static const uint8_t                   CLGC_DPD_SYNC_EN                              = 1u;
    static const int16_t                   FRACTIONAL_TUNING_DELAY                       = 0;
    static const uint32_t                  CLGC_MAX_SAMPLES_BATCHES_PER_CLGC_RUN_DEFAULT = 514u;
    static const float                     ORX_NOISE_DENSITY_dBFS_PER_Hz                 = -153;
    static const float                     ORX_NOISE_TRACK_PARAM                         = 0.98f;
    static const float                     EXPECTED_LOOP_GAIN_RIPPLE_dB                  = 0.05f;
    static const float                     DAMPING_PARAM                                 = 0.1f;
    static const adi_adrv9025_TxChannels_e txChannelsArr[]                               = {
        ADI_ADRV9025_TX1,
        ADI_ADRV9025_TX2,
        ADI_ADRV9025_TX3,
        ADI_ADRV9025_TX4
    };

    int32_t  recoveryAction                                 = ADI_COMMON_ACT_NO_ACTION;
    uint16_t clgcConfigOffsetBytes                          = 0u;
    uint8_t  armConfigData[ADRV9025_CLGC_CONFIG_SIZE_BYTES] = {0};
    uint32_t txChannelIndex                                 = 0u;
    uint8_t* fpSerializerPtr                                = NULL;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that clgcConfig is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        clgcConfig);

    for (txChannelIndex = 0; txChannelIndex < (uint32_t)(sizeof(txChannelsArr) / sizeof(txChannelsArr[0])); txChannelIndex++)
    {
        if ((clgcConfig->txChannelMask & (uint32_t)txChannelsArr[txChannelIndex]) != ADI_ADRV9025_TXOFF)
        {
            switch (txChannelsArr[txChannelIndex])
            {
            case(ADI_ADRV9025_TX1):
            {
                clgcConfigOffsetBytes = 0u;
                break;
            }
            case(ADI_ADRV9025_TX2):
            {
                clgcConfigOffsetBytes = (uint16_t)ADRV9025_CLGC_CONFIG_OFFSET_BYTES;
                break;
            }
            case(ADI_ADRV9025_TX3):
            {
                clgcConfigOffsetBytes = (uint16_t)ADRV9025_CLGC_CONFIG_OFFSET_BYTES * 2;
                break;
            }
            case(ADI_ADRV9025_TX4):
            {
                clgcConfigOffsetBytes = (uint16_t)ADRV9025_CLGC_CONFIG_OFFSET_BYTES * 3;
                break;
            }
            default:
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 clgcConfig->txChannelMask,
                                 "Invalid target Tx channel selected for CLGC config set. Valid Tx channels are Tx1-Tx4");
                ADI_ERROR_RETURN(device->common.error.newAction);
                break;
            }
            }

            /* Serialize the clgc config */
            armConfigData[0] = (uint8_t)(clgcConfig->clgcMeasurementBatchTime_us & 0xFF);
            armConfigData[1] = (uint8_t)((clgcConfig->clgcMeasurementBatchTime_us >> 8) & 0xFF);
            armConfigData[2]  = (uint8_t)((uint16_t)FRACTIONAL_TUNING_DELAY & 0xFF);
            armConfigData[3]  = (uint8_t)(((uint16_t)FRACTIONAL_TUNING_DELAY >> 8) & 0xFF);
            armConfigData[4]  = (uint8_t)(clgcConfig->clgcEnableGainControl & 0xFF);
            armConfigData[5]  = 0;
            armConfigData[6]  = CLGC_DPD_SYNC_EN;
            armConfigData[7]  = PA_PROTECTION_EN;
            armConfigData[8]  = (uint8_t)(CLGC_MAX_SAMPLES_BATCHES_PER_CLGC_RUN_DEFAULT & 0xFF);
            armConfigData[9]  = (uint8_t)((CLGC_MAX_SAMPLES_BATCHES_PER_CLGC_RUN_DEFAULT >> 8) & 0xFF);
            armConfigData[10] = (uint8_t)((CLGC_MAX_SAMPLES_BATCHES_PER_CLGC_RUN_DEFAULT >> 16) & 0xFF);
            armConfigData[11] = (uint8_t)((CLGC_MAX_SAMPLES_BATCHES_PER_CLGC_RUN_DEFAULT >> 24) & 0xFF);
            fpSerializerPtr   = (uint8_t*)&ORX_NOISE_DENSITY_dBFS_PER_Hz;
            armConfigData[12] = fpSerializerPtr[0];
            armConfigData[13] = fpSerializerPtr[1];
            armConfigData[14] = fpSerializerPtr[2];
            armConfigData[15] = fpSerializerPtr[3];
            fpSerializerPtr   = (uint8_t*)&clgcConfig->clgcTxQualifyingThreshold_dBFS;
            armConfigData[16] = fpSerializerPtr[0];
            armConfigData[17] = fpSerializerPtr[1];
            armConfigData[18] = fpSerializerPtr[2];
            armConfigData[19] = fpSerializerPtr[3];
            fpSerializerPtr   = (uint8_t*)&clgcConfig->clgcOrxQualifyingThreshold_dBFS;
            armConfigData[20] = fpSerializerPtr[0];
            armConfigData[21] = fpSerializerPtr[1];
            armConfigData[22] = fpSerializerPtr[2];
            armConfigData[23] = fpSerializerPtr[3];
            fpSerializerPtr   = (uint8_t*)&EXPECTED_LOOP_GAIN_RIPPLE_dB;
            armConfigData[24] = fpSerializerPtr[0];
            armConfigData[25] = fpSerializerPtr[1];
            armConfigData[26] = fpSerializerPtr[2];
            armConfigData[27] = fpSerializerPtr[3];
            fpSerializerPtr   = (uint8_t*)&clgcConfig->clgcExpectedLoopGain_dB;
            armConfigData[28] = fpSerializerPtr[0];
            armConfigData[29] = fpSerializerPtr[1];
            armConfigData[30] = fpSerializerPtr[2];
            armConfigData[31] = fpSerializerPtr[3];
            fpSerializerPtr   = (uint8_t*)&clgcConfig->clgcMaxGainAdjustmentStepSize_dB;
            armConfigData[32] = fpSerializerPtr[0];
            armConfigData[33] = fpSerializerPtr[1];
            armConfigData[34] = fpSerializerPtr[2];
            armConfigData[35] = fpSerializerPtr[3];
            fpSerializerPtr   = (uint8_t*)&clgcConfig->clgcMinTxAttenAdjust_dB;
            armConfigData[36] = fpSerializerPtr[0];
            armConfigData[37] = fpSerializerPtr[1];
            armConfigData[38] = fpSerializerPtr[2];
            armConfigData[39] = fpSerializerPtr[3];
            fpSerializerPtr   = (uint8_t*)&clgcConfig->clgcMaxTxAttenAdjust_dB;
            armConfigData[40] = fpSerializerPtr[0];
            armConfigData[41] = fpSerializerPtr[1];
            armConfigData[42] = fpSerializerPtr[2];
            armConfigData[43] = fpSerializerPtr[3];
            fpSerializerPtr   = (uint8_t*)&DAMPING_PARAM;
            armConfigData[44] = fpSerializerPtr[0];
            armConfigData[45] = fpSerializerPtr[1];
            armConfigData[46] = fpSerializerPtr[2];
            armConfigData[47] = fpSerializerPtr[3];
            fpSerializerPtr   = (uint8_t*)&ORX_NOISE_TRACK_PARAM;
            armConfigData[48] = fpSerializerPtr[0];
            armConfigData[49] = fpSerializerPtr[1];
            armConfigData[50] = fpSerializerPtr[2];
            armConfigData[51] = fpSerializerPtr[3];

            /* Pass the CLGC config to ARM */
            recoveryAction = adi_adrv9025_CpuConfigWrite(device,
                                                         ADI_ADRV9025_CPU_TYPE_C,
                                                         ADRV9025_CPU_OBJECTID_CLGC_TRACKING,
                                                         clgcConfigOffsetBytes,
                                                         &armConfigData[0],
                                                         sizeof(armConfigData));
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_CfrCorrectionPulseWrite(adi_adrv9025_Device_t* device,
                                         uint32_t               txChannelMask,
                                         int16_t                coeffReal[],
                                         int16_t                coeffImaginary[],
                                         uint16_t               numCoeffs)
{
    static const uint32_t ADDR_OFFSET_PER_CHANNEL = 2048;
    static const uint16_t MAX_COEFF_SIZE          = 512;
    static const uint16_t NUM_BYTES_PER_COEFF     = 4;
    static const uint8_t  CFR_PULSE_DONE_ID       = 1;
    static const uint8_t  CFR_PULSE_MODE_ID       = 0;
    static const uint8_t  ARM_MEM_READ_AUTOINCR   = 1;
    static const uint8_t  PULSE_MODE_1A_SELECTION = 3;
    static const uint32_t ALL_TX_MASK             = (uint32_t)(ADI_ADRV9025_TX1 | ADI_ADRV9025_TX2 | ADI_ADRV9025_TX3 | ADI_ADRV9025_TX4);

    int32_t  recoveryAction         = ADI_COMMON_ACT_NO_ACTION;
    uint8_t  armData[4]             = {0};
    uint16_t coeffIndex             = 0;
    uint16_t coeffMemOffset         = 0;
    uint32_t txOffset               = 0;
    uint32_t cfrPulseBufferBaseAddr = 0;
    uint8_t  k                      = 0;
    int16_t tempImagCoef;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        coeffReal);

    ADI_NULL_PTR_RETURN(&device->common,
                        coeffImaginary);

    /* Ensure that the requested coeff size is less than the maximum allowed size of 512 */
    if (numCoeffs > MAX_COEFF_SIZE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numCoeffs,
                         "No. of coefficients in the CFR correction pulse exceeds the maximum allowed value of 512.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Retrieve the base address for the CFR correction pulse buffer */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)(ADRV9025_CPU_C_ADDR_CFR_PULSE),
                                             &armData[0],
                                             sizeof(armData),
                                             ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armCtrlData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    cfrPulseBufferBaseAddr = ((uint32_t)armData[0] |
        ((uint32_t)armData[1] << 8) |
        ((uint32_t)armData[2] << 16) |
        ((uint32_t)armData[3] << 24));

    /* Set the pulse mode to 1A to support different pulses for different Tx channels */
    armData[0]     = (uint8_t)PULSE_MODE_1A_SELECTION;
    recoveryAction = adrv9025_CfrCtrlCmdExecute(device,
                                                ALL_TX_MASK,
                                                CFR_PULSE_MODE_ID,
                                                &armData[0],
                                                (uint8_t)1);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (k = 0; k < ADI_ADRV9025_MAX_TXCHANNELS; k++)
    {
        if (ADRV9025_BF_EQUAL(txChannelMask, ((uint32_t)1 << (uint32_t)k)))
        {
            txOffset = (uint32_t)k * ADDR_OFFSET_PER_CHANNEL;

            /* Write the CFR correction pulse to pulse memory */
            for (coeffIndex = 0; coeffIndex < numCoeffs; coeffIndex++)
            {
                /* Conjugate Pulse before programming the pulse coefficients into the hardware. */
                tempImagCoef = -coeffImaginary[coeffIndex];
                coeffMemOffset = (coeffIndex * NUM_BYTES_PER_COEFF);
                armData[0]     = (uint8_t)((uint16_t)coeffReal[coeffIndex] & 0x00FF);
                armData[1]     = (uint8_t)((uint16_t)(coeffReal[coeffIndex] & 0xFF00) >> 8);
                armData[2]     = (uint8_t)((uint16_t)tempImagCoef & 0x00FF);
                armData[3]     = (uint8_t)((uint16_t)(tempImagCoef & 0xFF00) >> 8);

                recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                                          (uint32_t)(cfrPulseBufferBaseAddr + txOffset + coeffMemOffset),
                                                          &armData[0],
                                                          sizeof(armData),
                                                          ADI_ADRV9025_CPU_MEM_AUTO_INCR);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 armCtrlData,
                                 device->common.error.errormessage);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    /* Send the programming done signal to the ARM alongwith the half pulse length */
    armData[0]     = (uint8_t)(numCoeffs & 0x00FF);
    armData[1]     = (uint8_t)((numCoeffs & 0xFF00) >> 8);
    armData[2]     = (uint8_t)0; /* Setting Component carrier to 0 for mode 1 support. Has to be modified for mode 2 and mode 3 */
    recoveryAction = adrv9025_CfrCtrlCmdExecute(device,
                                                txChannelMask,
                                                CFR_PULSE_DONE_ID,
                                                &armData[0],
                                                (uint8_t)3);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adrv9025_CfrCorrectionPulseRead(adi_adrv9025_Device_t*    device,
                                        adi_adrv9025_TxChannels_e txChannel,
                                        int16_t                   coeffReal[],
                                        int16_t                   coeffImaginary[],
                                        uint16_t                  numCoeffs)
{
    static const uint8_t CFR_RESET_SUB_CMD = 0x80;
    static const uint8_t CFR_RESET_CMD_ID  = 17;
    static const uint8_t ARM_ERR_CODE      = 0x0E;
    int32_t              recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    uint16_t             coeffIndex        = 0;
    uint8_t              extData[4]        = {
        ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL,
        ADRV9025_CPU_OBJECTID_CFR_INIT,
        (uint8_t)txChannel,
        (CFR_RESET_CMD_ID | CFR_RESET_SUB_CMD)
    };
    uint8_t cmdStatusByte = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        coeffReal);

    ADI_NULL_PTR_RETURN(&device->common,
                        coeffImaginary);

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
                         "Invalid target Tx channel requested for reading back CFR pulse. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Ensure that the requested coeff size is less than the maximum allowed size of 512 */
    if (numCoeffs > (uint16_t)ADI_ADRV9025_MAX_CFR_CORRECTION_PULSE_LEN)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numCoeffs,
                         "No. of coefficients in the CFR correction pulse exceeds the maximum allowed value of 512.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (coeffIndex = 0; coeffIndex < numCoeffs; coeffIndex++)
    {
        recoveryAction = adrv9025_CfrPulseCoeffRead(device,
                                                    txChannel,
                                                    coeffIndex,
                                                    &coeffReal[coeffIndex],
                                                    &coeffImaginary[coeffIndex]);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         coeffIndex,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Send a CFR Reset command, this is required for HW to continue operation properly */
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
                                                   (uint32_t)ADI_ADRV9025_SETCFRCTRLCFG_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_SETCFRCTRLCFG_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
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

int32_t adrv9025_dpdTrackingConfigSet(adi_adrv9025_Device_t*                    device,
                                      adi_adrv9025_DpdTrackingConfig_t*         trackingConfig,
                                      adi_adrv9025_EnhancedDpdTrackingConfig_t* enhancedTrackingConfig,
                                      uint8_t                                   useEnhanced)
{
    static const uint8_t DPD_SET_TRACKING_CONFIG_CTRL_PARAM = 0x13;
    static const uint8_t ARM_ERR_CODE = 0x0E;
    static const uint8_t DPD_TRACKING_CONFIG_LEN = 124u;
    static const uint8_t DPD_DELTA_DEFAULT = 2u; /* 0.2dB */
    static const uint8_t DPD_DECAY_P_DEFAULT = 2u; /* 0.2dB */
    static const uint8_t DPD_TX_POW_SEL_DEFAULT = 0u; /* 0 = mean, 1 = median, 2 = max */
    static const uint16_t DPD_MODEL_DEFAULT = 0x0010;
    static const uint16_t DPD_MAG_SQ_DEFAULT = 0x40;
    static const uint16_t SELECTED_ERR_SAMPLES_DEFAULT = 128u;
    static const uint16_t DPD_INDIRECT_ERR_SAMPLES_DEFAULT = 256u;
    static const uint16_t DPD_SATURATION_THRES_DEFAULT = 40000u;
    static const uint16_t DPD_CLGC_SYNC_EN_DEFAULT = 1u;
    static const uint16_t DPD_OUTLIER_THRES_DEFAULT          = 32768u;
    /* threshold for sample in AM-AM plot outside of 1:1 line to be thrown out. (default: `100%' = 32768) */
    static const uint16_t DPD_LINEAR_TERM_DEFAULT = 1u;
    static const uint16_t DPD_STARTUP_ITER_COUNT_DEFAULT = 3u;
    static const uint16_t DPD_XCORR_TIMEOUT_DEFAULT = 1000u;
    static const uint32_t DPD_SAT_TX_COUNT_LIM_DEFAULT = 1u;
    static const uint32_t DPD_SAT_ORX_COUNT_LIM_DEFAULT = 3u;
    static const uint32_t DPD_ERR_COUNT_LIM_DEFAULT = 2u;
    static const uint16_t DPD_PART_SUB_ITERATION_COUNT_DEFAULT = 0x0001;
    static const uint16_t DPD_SELECTED_SIGNAL_LEVEL_DEFAULT    = 1024;
    static const uint16_t EDPD_CAPTURE_DEFAULT = 0;
    static const uint16_t EDPD_PEAK_SEARCH_SIZE_DEFAULT = 32767; /* half of the maximum value */
    static const uint16_t EDPD_MIN_RAND_CAPTURE_DELAY_DEFAULT = 32767;
    static const uint16_t EDPD_MAX_RAND_CAPTURE_DELAY_DEFAULT = 32767;
    static const uint16_t EDPD_SAMPLES_PER_CAPTURE_DEFAULT = 4096;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4] = { ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_DPD_TRACKING, (uint8_t)0, DPD_SET_TRACKING_CONFIG_CTRL_PARAM };
    uint8_t armData[126] = { 0 }; /* 126 should be DPD_TRACKING_CONFIG_LEN + 2 */
    uint8_t cmdStatusByte = 0;
    uint8_t i = 0;
    uint32_t txChannelMask = 0;
    uint32_t* fpSerializerPtr = NULL;
    uint32_t  fpSerializerTmp = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);


    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    if (useEnhanced == ADI_FALSE)
    {
        ADI_NULL_PTR_RETURN(&device->common, trackingConfig);
        #if ADI_ADRV9025_DPD_RANGE_CHECK > 0
            recoveryAction = adrv9025_DpdTrackingConfigRangeCheck(device, trackingConfig);
            ADI_ERROR_RETURN(device->common.error.newAction);
        #endif

        txChannelMask = trackingConfig->txChannelMask;
        armData[0] = 0u; /* offset within the structure to be set */
        armData[1] = DPD_TRACKING_CONFIG_LEN; /* number of bytes within the structures to be set */
        armData[2] = (uint8_t)(DPD_MODEL_DEFAULT & 0xFF);
        armData[3] = (uint8_t)((DPD_MODEL_DEFAULT >> 8) & 0xFF);
        armData[4] = (uint8_t)(trackingConfig->dpdUpdateMode);
        armData[5] = (DPD_TX_POW_SEL_DEFAULT & 0xFF);
        armData[6] = (uint8_t)(DPD_DELTA_DEFAULT);
        armData[7] = (uint8_t)(DPD_DECAY_P_DEFAULT);
        armData[8] = (uint8_t)(DPD_MAG_SQ_DEFAULT & 0xFF);
        armData[9] = (uint8_t)((DPD_MAG_SQ_DEFAULT >> 8) & 0xFF);
        armData[10] = (uint8_t)(trackingConfig->dpdMThreshold & 0xFF);
        armData[11] = (uint8_t)((trackingConfig->dpdMThreshold >> 8) & 0xFF);
        armData[12] = (uint8_t)((trackingConfig->dpdMThreshold >> 16) & 0xFF);
        armData[13] = (uint8_t)((trackingConfig->dpdMThreshold >> 24) & 0xFF);
        armData[14] = (uint8_t)(trackingConfig->dpdSamples & 0xFF);
        armData[15] = (uint8_t)((trackingConfig->dpdSamples >> 8) & 0xFF);
        armData[16] = (uint8_t)(trackingConfig->dpdFilterSel & 0xFF);
        armData[17] = (uint8_t)((trackingConfig->dpdFilterSel >> 8) & 0xFF);
        armData[18] = (uint8_t)(SELECTED_ERR_SAMPLES_DEFAULT & 0xFF);
        armData[19] = (uint8_t)((SELECTED_ERR_SAMPLES_DEFAULT >> 8) & 0xFF);
        armData[20] = (uint8_t)(DPD_INDIRECT_ERR_SAMPLES_DEFAULT & 0xFF);
        armData[21] = (uint8_t)((DPD_INDIRECT_ERR_SAMPLES_DEFAULT >> 8) & 0xFF);
        armData[22] = (uint8_t)(DPD_SATURATION_THRES_DEFAULT & 0xFF);
        armData[23] = (uint8_t)((DPD_SATURATION_THRES_DEFAULT >> 8) & 0xFF);
        armData[24] = (uint8_t)(DPD_OUTLIER_THRES_DEFAULT & 0xFF);
        armData[25] = (uint8_t)((DPD_OUTLIER_THRES_DEFAULT >> 8) & 0xFF);
        armData[26] = (uint8_t)(DPD_SAT_TX_COUNT_LIM_DEFAULT & 0xFF);
        armData[27] = (uint8_t)((DPD_SAT_TX_COUNT_LIM_DEFAULT >> 8) & 0xFF);
        armData[28] = (uint8_t)((DPD_SAT_TX_COUNT_LIM_DEFAULT >> 16) & 0xFF);
        armData[29] = (uint8_t)((DPD_SAT_TX_COUNT_LIM_DEFAULT >> 24) & 0xFF);
        armData[30] = (uint8_t)(DPD_SAT_ORX_COUNT_LIM_DEFAULT & 0xFF);
        armData[31] = (uint8_t)((DPD_SAT_ORX_COUNT_LIM_DEFAULT >> 8) & 0xFF);
        armData[32] = (uint8_t)((DPD_SAT_ORX_COUNT_LIM_DEFAULT >> 16) & 0xFF);
        armData[33] = (uint8_t)((DPD_SAT_ORX_COUNT_LIM_DEFAULT >> 24) & 0xFF);
        armData[34] = (uint8_t)(DPD_ERR_COUNT_LIM_DEFAULT & 0xFF);
        armData[35] = (uint8_t)((DPD_ERR_COUNT_LIM_DEFAULT >> 8) & 0xFF);
        armData[36] = (uint8_t)((DPD_ERR_COUNT_LIM_DEFAULT >> 16) & 0xFF);
        armData[37] = (uint8_t)((DPD_ERR_COUNT_LIM_DEFAULT >> 24) & 0xFF);
        armData[38] = (uint8_t)(DPD_PART_SUB_ITERATION_COUNT_DEFAULT & 0xFF);
        armData[39] = (uint8_t)((DPD_PART_SUB_ITERATION_COUNT_DEFAULT >> 8) & 0xFF);
        armData[40] = (uint8_t)(DPD_LINEAR_TERM_DEFAULT & 0xFF);
        armData[41] = (uint8_t)((DPD_LINEAR_TERM_DEFAULT >> 8) & 0xFF);

        for (i = 0; i < ADI_ADRV9025_MAX_DPD_FILTER; i++)
        {
            armData[42 + (i * 2)] = (uint8_t)(trackingConfig->dpdFilterCoeffWeight[i].real);
            armData[42 + (i * 2) + 1] = (uint8_t)(trackingConfig->dpdFilterCoeffWeight[i].imag);
        }

        armData[72] = (uint8_t)(DPD_STARTUP_ITER_COUNT_DEFAULT & 0xFF);
        armData[73] = (uint8_t)((DPD_STARTUP_ITER_COUNT_DEFAULT >> 8) & 0xFF);
        armData[74] = trackingConfig->dpdDirectRegularizationValue;
        armData[75] = (uint8_t)(trackingConfig->dpdIndirectRegularizationValue);

        armData[76] = (uint8_t)(trackingConfig->minAvgSignalLevel & 0xFF);
        armData[77] = (uint8_t)((trackingConfig->minAvgSignalLevel >> 8) & 0xFF);
        armData[78] = (uint8_t)(DPD_SELECTED_SIGNAL_LEVEL_DEFAULT & 0xFF);
        armData[79] = (uint8_t)((DPD_SELECTED_SIGNAL_LEVEL_DEFAULT >> 8) & 0xFF);
        armData[80] = (uint8_t)(trackingConfig->minAvgSignalLevelOrx & 0xFF);
        armData[81] = (uint8_t)((trackingConfig->minAvgSignalLevelOrx >> 8) & 0xFF);
        /* eDpd configuration */
        armData[82] = (uint8_t)(EDPD_PEAK_SEARCH_SIZE_DEFAULT & 0xFF);
        armData[83] = (uint8_t)((EDPD_PEAK_SEARCH_SIZE_DEFAULT >> 8) & 0xFF);
        armData[84] = (uint8_t)(EDPD_CAPTURE_DEFAULT & 0xFF);
        armData[85] = (uint8_t)((EDPD_CAPTURE_DEFAULT >> 8) & 0xFF);
        armData[86] = (uint8_t)(EDPD_MIN_RAND_CAPTURE_DELAY_DEFAULT & 0xFF);
        armData[87] = (uint8_t)((EDPD_MIN_RAND_CAPTURE_DELAY_DEFAULT >> 8) & 0xFF);

        armData[88] = (uint8_t)(DPD_XCORR_TIMEOUT_DEFAULT & 0xFF);
        armData[89] = (uint8_t)((DPD_XCORR_TIMEOUT_DEFAULT >> 8) & 0xFF);
        armData[90] = (uint8_t)(trackingConfig->dpdPeakSearchWindowSize & 0xFF);
        armData[91] = (uint8_t)((trackingConfig->dpdPeakSearchWindowSize >> 8) & 0xFF);
        armData[92] = (uint8_t)((trackingConfig->dpdPeakSearchWindowSize >> 16) & 0xFF);
        armData[93] = (uint8_t)((trackingConfig->dpdPeakSearchWindowSize >> 24) & 0xFF);
        armData[94] = (uint8_t)(trackingConfig->dpdMu);
        armData[95] = (uint8_t)(trackingConfig->enableDirectLearning);

        armData[96] = (uint8_t)(EDPD_SAMPLES_PER_CAPTURE_DEFAULT & 0xFF);
        armData[97] = (uint8_t)((EDPD_SAMPLES_PER_CAPTURE_DEFAULT >> 8) & 0xFF);
        armData[98] = (uint8_t)(EDPD_MAX_RAND_CAPTURE_DELAY_DEFAULT & 0xFF);
        armData[99] = (uint8_t)((EDPD_MAX_RAND_CAPTURE_DELAY_DEFAULT >> 8) & 0xFF);

        armData[100] = (uint8_t)(DPD_CLGC_SYNC_EN_DEFAULT & 0xFF);
        armData[101] = (uint8_t)((DPD_CLGC_SYNC_EN_DEFAULT >> 8 ) & 0xFF);
        armData[102] = (uint8_t)(trackingConfig->dpdIndirectRegularizationLowPowerValue & 0xFF);
        armData[103] = (uint8_t)((trackingConfig->dpdIndirectRegularizationLowPowerValue >> 8 ) & 0xFF);
    } /* end tracking config*/
    else
    {
        ADI_NULL_PTR_RETURN(&device->common, enhancedTrackingConfig);
        #if ADI_ADRV9025_DPD_RANGE_CHECK > 0
            recoveryAction = adrv9025_EnhancedDpdTrackingConfigRangeCheck(device, enhancedTrackingConfig);
            ADI_ERROR_RETURN(device->common.error.newAction);
        #endif

        txChannelMask = enhancedTrackingConfig->txChannelMask;

        armData[0] = 0u; /* offset within the structure to be set */
        armData[1] = DPD_TRACKING_CONFIG_LEN; /* number of bytes within the structures to be set */
        armData[2] = (uint8_t)(DPD_MODEL_DEFAULT & 0xFF);
        armData[3] = (uint8_t)((DPD_MODEL_DEFAULT >> 8) & 0xFF);
        armData[4] = (uint8_t)(enhancedTrackingConfig->dpdUpdateMode);
        armData[5] = (DPD_TX_POW_SEL_DEFAULT & 0xFF);
        armData[6] = (uint8_t)(DPD_DELTA_DEFAULT);
        armData[7] = (uint8_t)(DPD_DECAY_P_DEFAULT);
        armData[8] = (uint8_t)(DPD_MAG_SQ_DEFAULT & 0xFF);
        armData[9] = (uint8_t)((DPD_MAG_SQ_DEFAULT >> 8) & 0xFF);
        armData[10] = (uint8_t)(enhancedTrackingConfig->dpdMThreshold & 0xFF);
        armData[11] = (uint8_t)((enhancedTrackingConfig->dpdMThreshold >> 8) & 0xFF);
        armData[12] = (uint8_t)((enhancedTrackingConfig->dpdMThreshold >> 16) & 0xFF);
        armData[13] = (uint8_t)((enhancedTrackingConfig->dpdMThreshold >> 24) & 0xFF);
        armData[14] = (uint8_t)(enhancedTrackingConfig->dpdSamples & 0xFF);
        armData[15] = (uint8_t)((enhancedTrackingConfig->dpdSamples >> 8) & 0xFF);
        armData[16] = (uint8_t)(enhancedTrackingConfig->dpdFilterSel & 0xFF);
        armData[17] = (uint8_t)((enhancedTrackingConfig->dpdFilterSel >> 8) & 0xFF);
        armData[18] = (uint8_t)(SELECTED_ERR_SAMPLES_DEFAULT & 0xFF);
        armData[19] = (uint8_t)((SELECTED_ERR_SAMPLES_DEFAULT >> 8) & 0xFF);
        armData[20] = (uint8_t)(DPD_INDIRECT_ERR_SAMPLES_DEFAULT & 0xFF);
        armData[21] = (uint8_t)((DPD_INDIRECT_ERR_SAMPLES_DEFAULT >> 8) & 0xFF);
        armData[22] = (uint8_t)(DPD_SATURATION_THRES_DEFAULT & 0xFF);
        armData[23] = (uint8_t)((DPD_SATURATION_THRES_DEFAULT >> 8) & 0xFF);
        armData[24] = (uint8_t)(DPD_OUTLIER_THRES_DEFAULT & 0xFF);
        armData[25] = (uint8_t)((DPD_OUTLIER_THRES_DEFAULT >> 8) & 0xFF);
        armData[26] = (uint8_t)(DPD_SAT_TX_COUNT_LIM_DEFAULT & 0xFF);
        armData[27] = (uint8_t)((DPD_SAT_TX_COUNT_LIM_DEFAULT >> 8) & 0xFF);
        armData[28] = (uint8_t)((DPD_SAT_TX_COUNT_LIM_DEFAULT >> 16) & 0xFF);
        armData[29] = (uint8_t)((DPD_SAT_TX_COUNT_LIM_DEFAULT >> 24) & 0xFF);
        armData[30] = (uint8_t)(DPD_SAT_ORX_COUNT_LIM_DEFAULT & 0xFF);
        armData[31] = (uint8_t)((DPD_SAT_ORX_COUNT_LIM_DEFAULT >> 8) & 0xFF);
        armData[32] = (uint8_t)((DPD_SAT_ORX_COUNT_LIM_DEFAULT >> 16) & 0xFF);
        armData[33] = (uint8_t)((DPD_SAT_ORX_COUNT_LIM_DEFAULT >> 24) & 0xFF);
        armData[34] = (uint8_t)(DPD_ERR_COUNT_LIM_DEFAULT & 0xFF);
        armData[35] = (uint8_t)((DPD_ERR_COUNT_LIM_DEFAULT >> 8) & 0xFF);
        armData[36] = (uint8_t)((DPD_ERR_COUNT_LIM_DEFAULT >> 16) & 0xFF);
        armData[37] = (uint8_t)((DPD_ERR_COUNT_LIM_DEFAULT >> 24) & 0xFF);
        armData[38] = (uint8_t)(DPD_PART_SUB_ITERATION_COUNT_DEFAULT & 0xFF);
        armData[39] = (uint8_t)((DPD_PART_SUB_ITERATION_COUNT_DEFAULT >> 8) & 0xFF);
        armData[40] = (uint8_t)(DPD_LINEAR_TERM_DEFAULT & 0xFF);
        armData[41] = (uint8_t)((DPD_LINEAR_TERM_DEFAULT >> 8) & 0xFF);

        for (i = 0; i < ADI_ADRV9025_MAX_DPD_FILTER; i++)
        {
            armData[42 + (i * 2)] = (uint8_t)(enhancedTrackingConfig->dpdFilterCoeffWeight[i].real);
            armData[42 + (i * 2) + 1] = (uint8_t)(enhancedTrackingConfig->dpdFilterCoeffWeight[i].imag);
        }

        armData[72] = (uint8_t)(DPD_STARTUP_ITER_COUNT_DEFAULT & 0xFF);
        armData[73] = (uint8_t)((DPD_STARTUP_ITER_COUNT_DEFAULT >> 8) & 0xFF);
        armData[74] = enhancedTrackingConfig->dpdDirectRegularizationValue;
        armData[75] = (uint8_t)(enhancedTrackingConfig->dpdIndirectRegularizationValue);

        armData[76] = (uint8_t)(enhancedTrackingConfig->minAvgSignalLevel & 0xFF);
        armData[77] = (uint8_t)((enhancedTrackingConfig->minAvgSignalLevel >> 8) & 0xFF);
        armData[78] = (uint8_t)(DPD_SELECTED_SIGNAL_LEVEL_DEFAULT & 0xFF);
        armData[79] = (uint8_t)((DPD_SELECTED_SIGNAL_LEVEL_DEFAULT >> 8) & 0xFF);
        armData[80] = (uint8_t)(enhancedTrackingConfig->minAvgSignalLevelOrx & 0xFF);
        armData[81] = (uint8_t)((enhancedTrackingConfig->minAvgSignalLevelOrx >> 8) & 0xFF);

        /* eDpd configuration */
        armData[82] = (uint8_t)(enhancedTrackingConfig->enhancedDpdPeakSearchSize & 0xFF);
        armData[83] = (uint8_t)((enhancedTrackingConfig->enhancedDpdPeakSearchSize >> 8) & 0xFF);
        armData[84] = (uint8_t)(enhancedTrackingConfig->enhancedDPDCaptures & 0xFF);
        armData[85] = (uint8_t)((enhancedTrackingConfig->enhancedDPDCaptures >> 8) & 0xFF);
        armData[86] = (uint8_t)(enhancedTrackingConfig->enhancedDPDMinRandCapDelay & 0xFF);
        armData[87] = (uint8_t)((enhancedTrackingConfig->enhancedDPDMinRandCapDelay >> 8) & 0xFF);

        armData[88] = (uint8_t)(DPD_XCORR_TIMEOUT_DEFAULT & 0xFF);
        armData[89] = (uint8_t)((DPD_XCORR_TIMEOUT_DEFAULT >> 8) & 0xFF);
        armData[90] = (uint8_t)(enhancedTrackingConfig->dpdPeakSearchWindowSize & 0xFF);
        armData[91] = (uint8_t)((enhancedTrackingConfig->dpdPeakSearchWindowSize >> 8) & 0xFF);
        armData[92] = (uint8_t)((enhancedTrackingConfig->dpdPeakSearchWindowSize >> 16) & 0xFF);
        armData[93] = (uint8_t)((enhancedTrackingConfig->dpdPeakSearchWindowSize >> 24) & 0xFF);
        armData[94] = (uint8_t)(enhancedTrackingConfig->dpdMu);
        armData[95] = (uint8_t)(enhancedTrackingConfig->enableDirectLearning);

        armData[96] = (uint8_t)(enhancedTrackingConfig->samplesPerCap & 0xFF);
        armData[97] = (uint8_t)((enhancedTrackingConfig->samplesPerCap >> 8) & 0xFF);
        armData[98] = (uint8_t)(enhancedTrackingConfig->enhancedDPDMaxRandCapDelay & 0xFF);
        armData[99] = (uint8_t)((enhancedTrackingConfig->enhancedDPDMaxRandCapDelay >> 8) & 0xFF);

        armData[100] = (uint8_t)(DPD_CLGC_SYNC_EN_DEFAULT & 0xFF);
        armData[101] = (uint8_t)((DPD_CLGC_SYNC_EN_DEFAULT >> 8 ) & 0xFF);
        armData[102] = (uint8_t)(enhancedTrackingConfig->dpdIndirectRegularizationLowPowerValue & 0xFF);
        armData[103] = (uint8_t)((enhancedTrackingConfig->dpdIndirectRegularizationLowPowerValue >> 8 ) & 0xFF);

        armData[104] = (uint8_t)(enhancedTrackingConfig->tddLutSwitchModelADelay[0] & 0xFF);
        armData[105] = (uint8_t)((enhancedTrackingConfig->tddLutSwitchModelADelay[0] >> 8) & 0xFF);
        armData[106] = (uint8_t)(enhancedTrackingConfig->tddLutSwitchModelADelay[1] & 0xFF);
        armData[107] = (uint8_t)((enhancedTrackingConfig->tddLutSwitchModelADelay[1] >> 8) & 0xFF);
        armData[108] = (uint8_t)(enhancedTrackingConfig->tddLutSwitchModelADelay[2] & 0xFF);
        armData[109] = (uint8_t)((enhancedTrackingConfig->tddLutSwitchModelADelay[2] >> 8) & 0xFF);
        armData[110] = (uint8_t)(enhancedTrackingConfig->tddLutSwitchModelADelay[3] & 0xFF);
        armData[111] = (uint8_t)((enhancedTrackingConfig->tddLutSwitchModelADelay[3] >> 8) & 0xFF);
        armData[112] = (uint8_t)(enhancedTrackingConfig->tddLutSwitchModelBDelay & 0xFF);
        armData[113] = (uint8_t)((enhancedTrackingConfig->tddLutSwitchModelBDelay >> 8) & 0xFF);
        armData[114] = enhancedTrackingConfig->enableTddLutSwitch;
        armData[115] = enhancedTrackingConfig->enableWidebandRegularization;
        armData[116] = enhancedTrackingConfig->widebandRegularizationDnSampleRate;

        fpSerializerPtr = (uint32_t*)(&(enhancedTrackingConfig->widebandRegularizationFactor));
        fpSerializerTmp = *fpSerializerPtr;
        armData[118] = (uint8_t)(fpSerializerTmp & 0xFF);
        armData[119] = (uint8_t)((fpSerializerTmp >> 8) & 0xFF);
        armData[120] = (uint8_t)((fpSerializerTmp >> 16) & 0xFF);
        armData[121] = (uint8_t)((fpSerializerTmp >> 24) & 0xFF);
        fpSerializerPtr = (uint32_t*)(&(enhancedTrackingConfig->widebandRegularizationFactorAlpha));
        fpSerializerTmp = *fpSerializerPtr;
        armData[122] = (uint8_t)(fpSerializerTmp & 0xFF);
        armData[123] = (uint8_t)((fpSerializerTmp >> 8) & 0xFF);
        armData[124] = (uint8_t)((fpSerializerTmp >> 16) & 0xFF);
        armData[125] = (uint8_t)((fpSerializerTmp >> 24) & 0xFF);
    }
    for (i = 0; i < ADRV9025_NUMBER_OF_TX_CHANNELS; i++)
    {
        if ((txChannelMask & (0x1 << i)) != 0)
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

int32_t adrv9025_dpdTrackingConfigGet(adi_adrv9025_Device_t*                    device,
                                      adi_adrv9025_TxChannels_e                 txChannel,
                                      adi_adrv9025_DpdTrackingConfig_t*         trackingConfig,
                                      adi_adrv9025_EnhancedDpdTrackingConfig_t* enhancedTrackingConfig,
                                      uint8_t                                   useEnhanced)
{
    static const uint8_t DPD_GET_TRACKING_CONFIG_CTRL_PARAM = 0x03;
    static const uint8_t ARM_ERR_CODE = 0x0E;
    static const uint8_t DPD_TRACKING_CONFIG_LEN = 124u;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4]     = { ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_DPD_TRACKING, (uint8_t)txChannel, DPD_GET_TRACKING_CONFIG_CTRL_PARAM };
    uint8_t armData[124] = { 0 }; /* 124 should be DPD_TRACKING_CONFIG_LEN */
    uint8_t cmdStatusByte  = 0;
    uint8_t i              = 0;
    float*  fpDeserializerPtr = NULL;
    uint32_t fpDeserializerTmp = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    armData[0] = 0;
    armData[1] = DPD_TRACKING_CONFIG_LEN;

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
    if (useEnhanced == ADI_FALSE)
    {

        trackingConfig->txChannelMask = txChannel;
        /* Deserialize data tracking config */
        trackingConfig->dpdUpdateMode = (adi_adrv9025_DpdTrackingUpdateMode_e)armData[2];
        trackingConfig->dpdMThreshold = (uint32_t)((uint32_t)armData[8] |
                                                   (uint32_t)armData[9] << 8 |
                                                   (uint32_t)armData[10] << 16 |
                                                   (uint32_t)armData[11] << 24);
        trackingConfig->dpdSamples = (uint16_t)((uint16_t)armData[12] | ((uint16_t)armData[13] << 8));

        trackingConfig->dpdFilterSel = (uint16_t)((uint16_t)armData[14] | ((uint16_t)armData[15] << 8));
        for (i = 0; i < ADI_ADRV9025_MAX_DPD_FILTER; i++)
        {
            trackingConfig->dpdFilterCoeffWeight[i].real = armData[40 + (i * 2)];
            trackingConfig->dpdFilterCoeffWeight[i].imag = armData[40 + (i * 2) + 1];
        }

        trackingConfig->dpdDirectRegularizationValue = armData[72];
        trackingConfig->dpdIndirectRegularizationValue = armData[73];

        trackingConfig->minAvgSignalLevel = (uint16_t)((uint16_t)armData[74] | ((uint16_t)armData[75] << 8));
        trackingConfig->minAvgSignalLevelOrx = (uint16_t)((uint16_t)armData[78] | ((uint16_t)armData[79] << 8));

        trackingConfig->dpdPeakSearchWindowSize = (uint32_t)((uint32_t)armData[88] |
                                                             (uint32_t)armData[89] << 8 |
                                                             (uint32_t)armData[90] << 16 |
                                                             (uint32_t)armData[91] << 24);

        trackingConfig->dpdMu = armData[92];
        trackingConfig->enableDirectLearning = armData[93];
        trackingConfig->dpdIndirectRegularizationLowPowerValue = (uint16_t)((uint16_t)armData[100] | ((uint16_t)armData[101] << 8));
    }
    else
    {
       ADI_NULL_PTR_RETURN(&device->common, enhancedTrackingConfig);

        enhancedTrackingConfig->txChannelMask = txChannel;
        /* Deserialize data tracking config */
        enhancedTrackingConfig->dpdUpdateMode = (adi_adrv9025_DpdTrackingUpdateMode_e)armData[2];
        enhancedTrackingConfig->dpdMThreshold = (uint32_t)((uint32_t)armData[8] |
                                                           (uint32_t)armData[9] << 8 |
                                                           (uint32_t)armData[10] << 16 |
                                                           (uint32_t)armData[11] << 24);
        enhancedTrackingConfig->dpdSamples = (uint16_t)((uint16_t)armData[12] | ((uint16_t)armData[13] << 8));

        enhancedTrackingConfig->dpdFilterSel = (uint16_t)((uint16_t)armData[14] | ((uint16_t)armData[15] << 8));
        for (i = 0; i < ADI_ADRV9025_MAX_DPD_FILTER; i++)
        {
            enhancedTrackingConfig->dpdFilterCoeffWeight[i].real = armData[40 + (i * 2)];
            enhancedTrackingConfig->dpdFilterCoeffWeight[i].imag = armData[40 + (i * 2) + 1];
        }

        enhancedTrackingConfig->dpdDirectRegularizationValue = armData[72];
        enhancedTrackingConfig->dpdIndirectRegularizationValue = armData[73];

        enhancedTrackingConfig->minAvgSignalLevel = (uint16_t)((uint16_t)armData[74] | ((uint16_t)armData[75] << 8));
        enhancedTrackingConfig->minAvgSignalLevelOrx = (uint16_t)((uint16_t)armData[78] | ((uint16_t)armData[79] << 8));

        enhancedTrackingConfig->enhancedDpdPeakSearchSize = (uint16_t)((uint16_t)armData[80] | ((uint16_t)armData[81] << 8));
        enhancedTrackingConfig->enhancedDPDCaptures = (uint16_t)((uint16_t)armData[82] | ((uint16_t)armData[83] << 8));
        enhancedTrackingConfig->enhancedDPDMinRandCapDelay = (uint16_t)((uint16_t)armData[84] | ((uint16_t)armData[85] << 8));

        enhancedTrackingConfig->dpdPeakSearchWindowSize = (uint32_t)((uint32_t)armData[88] |
                                                                     (uint32_t)armData[89] << 8 |
                                                                     (uint32_t)armData[90] << 16 |
                                                                     (uint32_t)armData[91] << 24);

        enhancedTrackingConfig->dpdMu = armData[92];
        enhancedTrackingConfig->enableDirectLearning = armData[93];

        enhancedTrackingConfig->samplesPerCap = (adi_adrv9025_dpdSamplesPerCapture_e)(uint32_t)(((uint16_t)armData[94] | ((uint16_t)armData[95] << 8)));
        enhancedTrackingConfig->enhancedDPDMaxRandCapDelay = (uint16_t)((uint16_t)armData[96] | ((uint16_t)armData[97] << 8));

        enhancedTrackingConfig->dpdIndirectRegularizationLowPowerValue = (uint16_t)((uint16_t)armData[100] | ((uint16_t)armData[101] << 8));

        enhancedTrackingConfig->tddLutSwitchModelADelay[0] = (uint16_t)((uint16_t)armData[102] | ((uint16_t)armData[103] << 8));
        enhancedTrackingConfig->tddLutSwitchModelADelay[1] = (uint16_t)((uint16_t)armData[104] | ((uint16_t)armData[105] << 8));
        enhancedTrackingConfig->tddLutSwitchModelADelay[2] = (uint16_t)((uint16_t)armData[106] | ((uint16_t)armData[107] << 8));
        enhancedTrackingConfig->tddLutSwitchModelADelay[3] = (uint16_t)((uint16_t)armData[108] | ((uint16_t)armData[109] << 8));
        enhancedTrackingConfig->tddLutSwitchModelBDelay = (uint16_t)((uint16_t)armData[110] | ((uint16_t)armData[111] << 8));
        enhancedTrackingConfig->enableTddLutSwitch = armData[112];
        enhancedTrackingConfig->enableWidebandRegularization = armData[113];
        enhancedTrackingConfig->widebandRegularizationDnSampleRate = armData[114];

        fpDeserializerTmp = (uint32_t)((uint32_t)armData[116] |
                                       (uint32_t)armData[117] << 8 |
                                       (uint32_t)armData[118] << 16 |
                                       (uint32_t)armData[119] << 24);
        fpDeserializerPtr = (float*)&fpDeserializerTmp;
        enhancedTrackingConfig->widebandRegularizationFactor = *fpDeserializerPtr;
        fpDeserializerTmp = (uint32_t)((uint32_t)armData[120] |
                                       (uint32_t)armData[121] << 8 |
                                       (uint32_t)armData[122] << 16 |
                                       (uint32_t)armData[123] << 24);
        fpDeserializerPtr = (float*)&fpDeserializerTmp;
        enhancedTrackingConfig->widebandRegularizationFactorAlpha = *fpDeserializerPtr;
    }
    return recoveryAction;
}

int32_t adrv9025_EnhancedDpdTrackingConfigRangeCheck(adi_adrv9025_Device_t*                    device,
                                                     adi_adrv9025_EnhancedDpdTrackingConfig_t* dpdTrackingConfig)
{
    static const uint8_t  DPD_TRACKING_CONFIG_MAX_REGULARIZATION_VALUE = 63u;
    static const uint32_t DPD_TRACKING_CONFIG_MAX_M_THRESH             = 2147483647u;
    static const uint32_t DPD_TRACKING_CONFIG_MAX_PEAK_WINDOW_SIZE     = 0x4B0000;
    static const uint16_t DPD_TRACKING_CONFIG_MAX_MIN_AVG_SIGNAL_LEVEL = 32768;
    static const uint16_t EDPD_PEAK_SEARCH_MAX = 65535;
    static const uint8_t  EDPD_MAX_CAPTURES = 16;
    static const uint16_t EDPD_MAX_CAPTURE_DELAY = 65535;
    static const uint8_t  TDD_LUT_SWITCH_ENABLE = 1u;
    static const uint8_t  WB_REGULARIZATION_ENABLE = 1u;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        dpdTrackingConfig);

    /* Check that the target Tx channel is valid */
    if (dpdTrackingConfig->txChannelMask < ADI_ADRV9025_TX1 ||
        dpdTrackingConfig->txChannelMask > ADI_ADRV9025_TXALL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->txChannelMask,
                         "Invalid Tx channel mask encountered while attempting to set DPD tracking config.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that update mode is valid */
    if ((dpdTrackingConfig->dpdUpdateMode != ADI_ADRV9025_DPD_TRACKING_UPDATE_MODE_0) &&
        (dpdTrackingConfig->dpdUpdateMode != ADI_ADRV9025_DPD_TRACKING_UPDATE_MODE_1) &&
        (dpdTrackingConfig->dpdUpdateMode != ADI_ADRV9025_DPD_TRACKING_UPDATE_MODE_2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->dpdUpdateMode,
                         "Invalid update mode encountered while attempting to set DPD tracking config.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that M threshold is valid */
    if (dpdTrackingConfig->dpdMThreshold >= DPD_TRACKING_CONFIG_MAX_M_THRESH)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->dpdMThreshold,
                         "M threshold should be less than 2147483648");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that indirect regularization value is valid */
    if (dpdTrackingConfig->dpdIndirectRegularizationValue >= DPD_TRACKING_CONFIG_MAX_REGULARIZATION_VALUE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->dpdIndirectRegularizationValue,
                         "Indirect regularization value should be less than 64");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that peak search window size is valid */
    if (dpdTrackingConfig->dpdPeakSearchWindowSize > DPD_TRACKING_CONFIG_MAX_PEAK_WINDOW_SIZE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->dpdPeakSearchWindowSize,
                         "Peak search window size value should be less than 0x4B0000");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdTrackingConfig->minAvgSignalLevel > DPD_TRACKING_CONFIG_MAX_MIN_AVG_SIGNAL_LEVEL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->minAvgSignalLevel,
                         "Min average signal level should be less than 32768");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdTrackingConfig->minAvgSignalLevelOrx > DPD_TRACKING_CONFIG_MAX_MIN_AVG_SIGNAL_LEVEL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->minAvgSignalLevelOrx,
                         "ORX Min average signal level should be less than 32768");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdTrackingConfig->dpdFilterSel > 1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->dpdFilterSel,
                         "OBW filter select valid values 0-1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if (dpdTrackingConfig->enableDirectLearning > 1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->enableDirectLearning,
                         "enable direct learning valid values 0 - disabled  and enabled - 1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if (dpdTrackingConfig->dpdMu > 100)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->dpdMu,
                         "Valid values for DPDmu are between 0-100");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if (dpdTrackingConfig->dpdIndirectRegularizationLowPowerValue > DPD_TRACKING_CONFIG_MAX_REGULARIZATION_VALUE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->dpdIndirectRegularizationLowPowerValue,
                         "Valid values for dpdIndirectRegularizationLowPowerValue are between 0-63");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if (dpdTrackingConfig->enhancedDpdPeakSearchSize > EDPD_PEAK_SEARCH_MAX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->enhancedDpdPeakSearchSize,
                         "Valid values for enhancedDpdPeakSearchSize are between 0-65535");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if (dpdTrackingConfig->enhancedDPDCaptures > EDPD_MAX_CAPTURES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->enhancedDPDCaptures,
                         "Valid values for enhancedDPDCaptures are between 0-16");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if (dpdTrackingConfig->enhancedDPDMinRandCapDelay > EDPD_MAX_CAPTURE_DELAY)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->enhancedDPDMinRandCapDelay,
                         "Valid values for enhancedDPDMinRandCapDelay are between 0-65535");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if (dpdTrackingConfig->enhancedDPDMaxRandCapDelay > EDPD_MAX_CAPTURE_DELAY)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->enhancedDPDMaxRandCapDelay,
                         "Valid values for enhancedDPDMaxRandCapDelay are between 0-65535");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if (dpdTrackingConfig->enableTddLutSwitch > TDD_LUT_SWITCH_ENABLE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->enableTddLutSwitch,
                         "Valid values for enableTddLutSwitch is 0 or 1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if (dpdTrackingConfig->enableWidebandRegularization > WB_REGULARIZATION_ENABLE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdTrackingConfig->enableTddLutSwitch,
                         "Valid values for enableWidebandRegularization is 0 or 1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return ADI_COMMON_ACT_NO_ACTION;
}
