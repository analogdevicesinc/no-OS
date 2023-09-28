// SPDX-License-Identifier: GPL-2.0
/**
* \file adrv9025_utilities.c
* \brief Contains Utility features related private function implementations
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "../include/adrv9025_utilities.h"
#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/ctype.h>
#endif
#include <ctype.h>
#include <string.h>
#include "adi_adrv9025_error.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_cpu.h"
#include "adi_adrv9025_rx.h"
#include "adi_adrv9025_tx.h"
#include "adi_adrv9025_radioctrl.h"
#include "adi_adrv9025_cals.h"
#include "adi_adrv9025_gpio.h"
#include "adi_adrv9025.h"

static const char* objectNameExceededMsg = "Object name string exceeds the maximum allowed length of 48";

int32_t adrv9025_RadioctrlInit(adi_adrv9025_Device_t*        device,
                               adi_adrv9025_RadioctrlInit_t* radioCtrlInit)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Sets up Radio Ctrl mode for Rx/ORx/Tx signal chains (SPI vs Pin mode) */
    recoveryAction = adi_adrv9025_RadioCtrlCfgSet(device,
                                                  &radioCtrlInit->radioCtrlModeCfg);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Set Frequency for LO1 PLL */
    if (radioCtrlInit->lo1PllFreq_Hz != 0)
    {
        recoveryAction = adi_adrv9025_PllFrequencySet(device,
                                                      ADI_ADRV9025_LO1_PLL,
                                                      radioCtrlInit->lo1PllFreq_Hz);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Set Frequency for LO2 PLL */
    if (radioCtrlInit->lo2PllFreq_Hz != 0)
    {
        recoveryAction = adi_adrv9025_PllFrequencySet(device,
                                                      ADI_ADRV9025_LO2_PLL,
                                                      radioCtrlInit->lo2PllFreq_Hz);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Set Frequency for Aux PLL */
    if (radioCtrlInit->auxPllFreq_Hz != 0)
    {
        recoveryAction = adi_adrv9025_PllFrequencySet(device,
                                                      ADI_ADRV9025_AUX_PLL,
                                                      radioCtrlInit->auxPllFreq_Hz);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Sets up Tx to ORx mapping. Required to run external LOL cals. Valid only in non-pin Mode */
    if (radioCtrlInit->txToOrxMapping.orx1Map == ADI_ADRV9025_MAP_NONE_ORX1)
    {
        recoveryAction = adi_adrv9025_TxToOrxMappingSet(device,
                                                        ADI_ADRV9025_ORX1,
                                                        ADI_ADRV9025_TXOFF);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (radioCtrlInit->txToOrxMapping.orx1Map == ADI_ADRV9025_MAP_TX1_ORX1)
    {
        recoveryAction = adi_adrv9025_TxToOrxMappingSet(device,
                                                        ADI_ADRV9025_ORX1,
                                                        ADI_ADRV9025_TX1);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (radioCtrlInit->txToOrxMapping.orx1Map == ADI_ADRV9025_MAP_TX2_ORX1)
    {
        recoveryAction = adi_adrv9025_TxToOrxMappingSet(device,
                                                        ADI_ADRV9025_ORX1,
                                                        ADI_ADRV9025_TX2);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         radioCtrlInit->txToOrxMapping.orx1Map,
                         "Invalid Tx-ORx1 map encountered in post mcs init. Valid map values are MAP_TX1,MAP_Tx2 and MAP_NONE");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (radioCtrlInit->txToOrxMapping.orx2Map == ADI_ADRV9025_MAP_NONE_ORX2)
    {
        recoveryAction = adi_adrv9025_TxToOrxMappingSet(device,
                                                        ADI_ADRV9025_ORX2,
                                                        ADI_ADRV9025_TXOFF);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (radioCtrlInit->txToOrxMapping.orx2Map == ADI_ADRV9025_MAP_TX1_ORX2)
    {
        recoveryAction = adi_adrv9025_TxToOrxMappingSet(device,
                                                        ADI_ADRV9025_ORX2,
                                                        ADI_ADRV9025_TX1);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (radioCtrlInit->txToOrxMapping.orx2Map == ADI_ADRV9025_MAP_TX2_ORX2)
    {
        recoveryAction = adi_adrv9025_TxToOrxMappingSet(device,
                                                        ADI_ADRV9025_ORX2,
                                                        ADI_ADRV9025_TX2);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         radioCtrlInit->txToOrxMapping.orx1Map,
                         "Invalid Tx-ORx2 map encountered in post mcs init. Valid map values are MAP_TX1,MAP_Tx2 and MAP_NONE");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (radioCtrlInit->txToOrxMapping.orx3Map == ADI_ADRV9025_MAP_NONE_ORX3)
    {
        recoveryAction = adi_adrv9025_TxToOrxMappingSet(device,
                                                        ADI_ADRV9025_ORX3,
                                                        ADI_ADRV9025_TXOFF);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (radioCtrlInit->txToOrxMapping.orx3Map == ADI_ADRV9025_MAP_TX3_ORX3)
    {
        recoveryAction = adi_adrv9025_TxToOrxMappingSet(device,
                                                        ADI_ADRV9025_ORX3,
                                                        ADI_ADRV9025_TX3);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (radioCtrlInit->txToOrxMapping.orx3Map == ADI_ADRV9025_MAP_TX4_ORX3)
    {
        recoveryAction = adi_adrv9025_TxToOrxMappingSet(device,
                                                        ADI_ADRV9025_ORX3,
                                                        ADI_ADRV9025_TX4);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         radioCtrlInit->txToOrxMapping.orx1Map,
                         "Invalid Tx-ORx3 map encountered in post mcs init. Valid map values are MAP_TX3,MAP_Tx4 and MAP_NONE");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (radioCtrlInit->txToOrxMapping.orx4Map == ADI_ADRV9025_MAP_NONE_ORX4)
    {
        recoveryAction = adi_adrv9025_TxToOrxMappingSet(device,
                                                        ADI_ADRV9025_ORX4,
                                                        ADI_ADRV9025_TXOFF);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (radioCtrlInit->txToOrxMapping.orx4Map == ADI_ADRV9025_MAP_TX3_ORX4)
    {
        recoveryAction = adi_adrv9025_TxToOrxMappingSet(device,
                                                        ADI_ADRV9025_ORX4,
                                                        ADI_ADRV9025_TX3);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (radioCtrlInit->txToOrxMapping.orx4Map == ADI_ADRV9025_MAP_TX4_ORX4)
    {
        recoveryAction = adi_adrv9025_TxToOrxMappingSet(device,
                                                        ADI_ADRV9025_ORX4,
                                                        ADI_ADRV9025_TX4);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         radioCtrlInit->txToOrxMapping.orx1Map,
                         "Invalid Tx-ORx4 map encountered in post mcs init. Valid map values are MAP_TX3,MAP_TX4 and MAP_NONE");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Sets up the GPIO pin mapping to the 4 stream trigger inputs to ADRV9025 */
    recoveryAction = adi_adrv9025_StreamGpioConfigSet(device,
                                                      &radioCtrlInit->streamGpioCfg);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_CalsInit(adi_adrv9025_Device_t*   device,
                          adi_adrv9025_InitCals_t* initCals)
{
    //TODO : Minimize timeout after lab eval
    static const uint32_t INIT_CALS_TIMEOUT_MS = 60000; /*60 seconds timeout*/

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t initCalsError  = 0;

    /* Run Init Cals */
    recoveryAction = adi_adrv9025_InitCalsRun(device,
                                              initCals);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adi_adrv9025_InitCalsWait(device,
                                               INIT_CALS_TIMEOUT_MS,
                                               &initCalsError);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_DdcMultDivGet(adi_adrv9025_Device_t* device,
                               adi_adrv9025_RxDdc_e   rxDdcMode,
                               uint8_t*               ddcMultiply,
                               uint8_t*               ddcDivide)
{
    switch (rxDdcMode)
    {
    case ADI_ADRV9025_RXDDC_BYPASS_REALIF:
    case ADI_ADRV9025_RXDDC_BYPASS:
        *ddcMultiply = 1;
        *ddcDivide = 1;
        break;
    case ADI_ADRV9025_RXDDC_DEC2_REALIF:
    case ADI_ADRV9025_RXDDC_DEC2:
        *ddcMultiply = 2;
        *ddcDivide = 1;
        break;
    case ADI_ADRV9025_RXDDC_INT2_REALIF:
    case ADI_ADRV9025_RXDDC_INT2:
        *ddcMultiply = 1;
        *ddcDivide = 2;
        break;
    case ADI_ADRV9025_RXDDC_FILTERONLY_REALIF:
    case ADI_ADRV9025_RXDDC_FILTERONLY:
        *ddcMultiply = 1;
        *ddcDivide = 1;
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxProfile,
                         "Rx Profile DDC mode is invalid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adrv9025_AdcProfilesInit(adi_adrv9025_Device_t*      device,
                                 adi_adrv9025_Init_t*        init,
                                 adi_adrv9025_AdcProfiles_t* adcProfile)
{
    int32_t                   recoveryAction                           = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                   ddcMultiply                              = 1;
    uint8_t                   ddcDivide                                = 1;
    uint8_t                   i                                        = 0;
    uint8_t                   j                                        = 0;
    uint32_t                  adcClk_MHz                               = 0;
    uint8_t                   profileIndex                             = 0;
    uint32_t                  maskBit                                  = 0;
    adi_adrv9025_RxProfile_t* rxProfile                                = NULL;
    adi_adrv9025_RxProfile_t* orxProfile                               = NULL;
    uint32_t                  rxMaskArray[ADI_ADRV9025_MAX_RXCHANNELS] = {
        ADI_ADRV9025_RX1,
        ADI_ADRV9025_RX2,
        ADI_ADRV9025_RX3,
        ADI_ADRV9025_RX4,
        ADI_ADRV9025_ORX1,
        ADI_ADRV9025_ORX2,
        ADI_ADRV9025_ORX3,
        ADI_ADRV9025_ORX4,
        ADI_ADRV9025_LB12,
        ADI_ADRV9025_LB34
    };

    const uint8_t         NUM_ADC_PROFILES          = 26;
    uint8_t               numAdcProfilesCoef        = 41;
    static const uint16_t adcProfileLutIndex[26][2] =
    {
        {20, 1228},
        {30, 1228},
        {37, 1474},
        {10, 1966},
        {50, 1966},
        {60, 1966},
        {100, 1966},
        {50, 2457},
        {62, 2457},
        {50, 2949},
        {75, 2949},
        {62, 3072},
        {70, 3072},
        {140, 3072},
        {75, 3686},
        {84, 3686},
        {150, 3686},
        {168, 3686},
        {100, 3932},
        {225, 3932},
        {100, 4915},
        {112, 4915},
        {140, 4915},
        {225, 4915},
        {101, 5000},
        {228, 5000}
    };

    static const uint16_t adcProfileLut[26 * 41] =
    {
        386, 273, 500, 154, 7, 1185, 619, 116, 12, 197, 132, 1, 10, 48, 0, 0, 32, 0, 0, 0, 12, 128, 3, 0, 15, 5, 0, 0, 3, 0, 13, 2, 0, 0, 14, 14, 0, 14, 0, 12, 0,
        386, 273, 506, 153, 15, 1182, 617, 116, 12, 198, 132, 1, 10, 48, 0, 0, 32, 0, 0, 0, 12, 128, 3, 0, 15, 5, 0, 0, 3, 0, 13, 2, 0, 0, 14, 14, 0, 14, 0, 12, 0,
        386, 273, 507, 192, 16, 984, 513, 121, 15, 206, 132, 1, 10, 48, 0, 0, 32, 0, 0, 0, 12, 128, 3, 0, 15, 5, 0, 0, 3, 0, 13, 2, 0, 0, 14, 14, 0, 14, 0, 12, 0,
        386, 273, 500, 256, 7, 739, 386, 120, 20, 205, 132, 1, 10, 48, 0, 0, 32, 0, 0, 0, 12, 128, 3, 0, 15, 5, 0, 0, 3, 0, 13, 2, 0, 0, 14, 14, 0, 14, 0, 12, 0,
        386, 273, 508, 255, 16, 738, 385, 121, 20, 206, 132, 1, 10, 48, 0, 0, 32, 0, 0, 0, 12, 128, 3, 7, 15, 5, 0, 0, 3, 7, 13, 2, 0, 0, 14, 14, 0, 14, 0, 12, 0,
        386, 273, 511, 255, 20, 737, 384, 122, 20, 206, 132, 1, 10, 48, 0, 0, 32, 0, 0, 0, 12, 128, 3, 7, 15, 5, 0, 0, 3, 7, 13, 2, 0, 0, 14, 14, 0, 14, 0, 12, 0,
        386, 273, 542, 240, 57, 730, 380, 120, 19, 199, 132, 1, 10, 48, 0, 0, 32, 0, 0, 0, 12, 128, 3, 7, 15, 5, 0, 0, 3, 7, 13, 2, 0, 0, 14, 14, 0, 14, 0, 12, 0,
        386, 273, 503, 320, 10, 590, 308, 121, 25, 211, 132, 1, 10, 48, 0, 0, 32, 0, 0, 0, 13, 128, 7, 7, 0, 5, 0, 7, 7, 7, 7, 7, 0, 7, 13, 13, 0, 13, 0, 12, 0,
        386, 273, 508, 319, 16, 590, 307, 121, 25, 211, 132, 1, 10, 48, 0, 0, 32, 0, 0, 0, 13, 128, 7, 7, 0, 5, 0, 7, 7, 7, 7, 7, 0, 7, 13, 13, 0, 13, 0, 12, 0,
        386, 273, 501, 384, 7, 492, 256, 120, 30, 210, 132, 1, 10, 48, 0, 0, 32, 0, 0, 0, 13, 128, 7, 7, 0, 5, 0, 7, 7, 7, 7, 7, 0, 7, 13, 13, 0, 13, 0, 12, 0,
        386, 273, 509, 383, 16, 491, 256, 121, 30, 211, 132, 1, 10, 48, 0, 0, 32, 0, 0, 0, 13, 128, 7, 7, 0, 5, 0, 7, 7, 7, 7, 7, 0, 7, 13, 13, 0, 13, 0, 12, 0,
        386, 273, 504, 396, 10, 472, 246, 120, 31, 204, 132, 1, 10, 32, 0, 0, 24, 0, 0, 0, 14, 128, 7, 7, 15, 5, 0, 7, 7, 7, 13, 2, 0, 7, 14, 14, 0, 14, 0, 12, 0,
        386, 273, 506, 396, 13, 471, 246, 120, 31, 204, 132, 1, 10, 32, 0, 0, 24, 0, 0, 0, 14, 128, 7, 7, 15, 5, 0, 7, 7, 7, 13, 2, 0, 7, 14, 14, 0, 14, 0, 12, 0,
        386, 273, 535, 381, 46, 467, 244, 120, 30, 200, 132, 1, 10, 32, 0, 0, 24, 0, 0, 0, 14, 128, 7, 7, 15, 5, 0, 7, 7, 7, 13, 2, 0, 7, 14, 14, 0, 14, 0, 12, 0,
        386, 273, 504, 486, 10, 393, 204, 122, 38, 208, 132, 1, 10, 32, 0, 0, 24, 0, 0, 0, 14, 128, 7, 7, 15, 5, 0, 7, 7, 7, 13, 2, 0, 7, 14, 14, 0, 14, 0, 12, 0,
        386, 273, 506, 486, 13, 392, 204, 123, 38, 209, 132, 1, 10, 32, 0, 0, 24, 0, 0, 0, 14, 128, 7, 7, 15, 5, 0, 7, 7, 7, 13, 2, 0, 7, 14, 14, 0, 14, 0, 12, 0,
        386, 273, 528, 470, 36, 390, 204, 122, 37, 205, 132, 1, 10, 32, 0, 0, 24, 0, 0, 0, 14, 128, 7, 7, 15, 5, 0, 7, 7, 7, 13, 2, 0, 7, 14, 14, 0, 14, 0, 12, 0,
        386, 273, 537, 469, 46, 389, 204, 123, 37, 206, 132, 1, 10, 32, 0, 0, 24, 0, 0, 0, 14, 128, 7, 7, 15, 5, 0, 7, 7, 7, 13, 2, 0, 7, 14, 14, 0, 14, 0, 12, 0,
        386, 273, 510, 511, 16, 367, 191, 121, 40, 206, 132, 1, 10, 32, 0, 0, 24, 0, 0, 0, 14, 128, 7, 7, 15, 5, 0, 7, 7, 7, 13, 2, 0, 7, 14, 14, 0, 14, 0, 12, 0,
        386, 273, 564, 479, 74, 362, 191, 121, 38, 200, 132, 1, 10, 32, 0, 0, 24, 0, 0, 0, 14, 128, 7, 7, 15, 5, 0, 7, 7, 7, 13, 2, 0, 7, 14, 14, 0, 14, 0, 12, 0,
        386, 273, 505, 512, 10, 294, 153, 96, 40, 148, 132, 1, 10, 16, 0, 0, 16, 0, 0, 0, 0, 128, 7, 7, 0, 5, 0, 7, 7, 7, 1, 2, 0, 7, 4, 0, 0, 4, 0, 12, 0,
        386, 273, 507, 511, 13, 294, 153, 96, 40, 148, 132, 1, 10, 16, 0, 0, 16, 0, 0, 0, 0, 128, 7, 7, 0, 5, 0, 7, 7, 7, 1, 2, 0, 7, 4, 0, 0, 4, 0, 12, 0,
        386, 273, 512, 511, 18, 293, 153, 97, 40, 149, 132, 1, 10, 16, 0, 0, 16, 0, 0, 0, 0, 128, 7, 7, 0, 5, 0, 7, 7, 7, 1, 2, 0, 7, 4, 0, 0, 4, 0, 12, 0,
        386, 273, 541, 507, 47, 291, 153, 99, 40, 150, 132, 1, 10, 16, 0, 0, 16, 0, 0, 0, 0, 128, 7, 7, 0, 5, 0, 7, 7, 7, 1, 2, 0, 7, 4, 0, 0, 4, 0, 12, 0,
        386, 273, 505, 512, 10, 289, 150, 94, 40, 146, 132, 1, 10, 16, 0, 0, 16, 0, 0, 0, 0, 128, 7, 7, 0, 5, 0, 7, 7, 7, 1, 2, 0, 7, 4, 0, 0, 4, 0, 12, 0,
        386, 273, 541, 507, 47, 286, 150, 98, 40, 148, 132, 1, 10, 16, 0, 0, 16, 0, 0, 0, 0, 128, 7, 7, 0, 5, 0, 7, 7, 7, 1, 2, 0, 7, 4, 0, 0, 4, 0, 12, 0
    };

    if (device->devStateInfo.profilesValid & ADI_ADRV9025_RX_PROFILE_VALID)
    {
        for (i = 0; i < ADI_ADRV9025_MAX_RX_ONLY; i++)
        {
            maskBit = rxMaskArray[i];
            if ((device->devStateInfo.initializedChannels & maskBit) == maskBit)
            {
                rxProfile      = &init->rx.rxChannelCfg[i].profile;
                recoveryAction = adrv9025_DdcMultDivGet(device,
                                                        rxProfile->rxDdcMode,
                                                        &ddcMultiply,
                                                        &ddcDivide);
                ADI_ERROR_RETURN(device->common.error.newAction);

                adcClk_MHz = (rxProfile->rxOutputRate_kHz * rxProfile->rxFirDecimation *
                    rxProfile->rxFir1Decimation * rxProfile->rxFir2Decimation *
                    rxProfile->rhb1Decimation * rxProfile->rhb2Decimation * rxProfile->rhb3Decimation *
                    rxProfile->rxDec5Decimation * ddcMultiply / ddcDivide) / 1000;

                /* find correct ADC profile in the LUT */
                profileIndex = NUM_ADC_PROFILES;
                for (j = 0; j < NUM_ADC_PROFILES; j++)
                {
                    /* Find a row in the LUT that matches the ADC clock frequency */
                    if ((adcProfileLutIndex[j][1] == adcClk_MHz) && (adcProfileLutIndex[j][0] >= (rxProfile->rxAdcBandWidth_kHz / 1000)))
                    {
                        profileIndex = j;
                        break;
                    }
                }

                /* Verify that a profile was found in the LUT, if not return error */
                if (profileIndex >= NUM_ADC_PROFILES)
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     profileIndex,
                                     "Invalid Rx profileIndex in adrv9025_AdcProfilesLoad");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }

                if (maskBit == ADI_ADRV9025_RX1)
                {
                    for (j = 0; j < numAdcProfilesCoef; j++)
                    {
                        adcProfile->RxChannel1[j] = adcProfileLut[profileIndex * numAdcProfilesCoef + j];
                    }
                }
                else if (maskBit == ADI_ADRV9025_RX2)
                {
                    for (j = 0; j < numAdcProfilesCoef; j++)
                    {
                        adcProfile->RxChannel2[j] = adcProfileLut[profileIndex * numAdcProfilesCoef + j];
                    }
                }
                else if (maskBit == ADI_ADRV9025_RX3)
                {
                    for (j = 0; j < numAdcProfilesCoef; j++)
                    {
                        adcProfile->RxChannel3[j] = adcProfileLut[profileIndex * numAdcProfilesCoef + j];
                    }
                }
                else if (maskBit == ADI_ADRV9025_RX4)
                {
                    for (j = 0; j < numAdcProfilesCoef; j++)
                    {
                        adcProfile->RxChannel4[j] = adcProfileLut[profileIndex * numAdcProfilesCoef + j];
                    }
                }
                else
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     profileIndex,
                                     "Invalid Rx channel in adrv9025_AdcProfilesLoad");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
        }
    }

    if (device->devStateInfo.profilesValid & ADI_ADRV9025_ORX_PROFILE_VALID)
    {
        adcProfile->OrxChannel1Index = 0;
        adcProfile->OrxChannel2Index = 0;

        for (i = ADI_ADRV9025_MAX_RX_ONLY; i < ADI_ADRV9025_MAX_LB_CHANNEL_START; i++)
        {
            maskBit = rxMaskArray[i];
            if ((device->devStateInfo.initializedChannels & maskBit) == maskBit)
            {
                orxProfile     = &init->rx.rxChannelCfg[i].profile;
                recoveryAction = adrv9025_DdcMultDivGet(device,
                                                        orxProfile->rxDdcMode,
                                                        &ddcMultiply,
                                                        &ddcDivide);
                ADI_ERROR_RETURN(device->common.error.newAction);

                adcClk_MHz = (orxProfile->rxOutputRate_kHz * orxProfile->rxFirDecimation *
                    orxProfile->rxFir1Decimation *
                    orxProfile->rhb1Decimation * orxProfile->rhb2Decimation * orxProfile->rhb3Decimation *
                    orxProfile->rxDec5Decimation * ddcMultiply / ddcDivide) / 1000;

                /* find correct ADC profile in the LUT */
                profileIndex = NUM_ADC_PROFILES;
                for (j = 0; j < NUM_ADC_PROFILES; j++)
                {
                    /* Find a row in the LUT that matches the ADC clock frequency */
                    if ((adcProfileLutIndex[j][1] == adcClk_MHz) && (adcProfileLutIndex[j][0] >= (orxProfile->rxAdcBandWidth_kHz / 1000)))
                    {
                        profileIndex = j;
                        break;
                    }
                }

                /* Verify that a profile was found in the LUT, if not return error */
                if (profileIndex >= NUM_ADC_PROFILES)
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     profileIndex,
                                     "Invalid Orx profileIndex in adrv9025_AdcProfilesLoad");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }

                if (maskBit == ADI_ADRV9025_ORX1)
                {
                    for (j = 0; j < numAdcProfilesCoef; j++)
                    {
                        adcProfile->OrxChannel1Profile0[j] = adcProfileLut[profileIndex * numAdcProfilesCoef + j];
                    }
                }
                else if (maskBit == ADI_ADRV9025_ORX2)
                {
                    for (j = 0; j < numAdcProfilesCoef; j++)
                    {
                        adcProfile->OrxChannel1Profile1[j] = adcProfileLut[profileIndex * numAdcProfilesCoef + j];
                    }
                }
                else if (maskBit == ADI_ADRV9025_ORX3)
                {
                    for (j = 0; j < numAdcProfilesCoef; j++)
                    {
                        adcProfile->OrxChannel2Profile0[j] = adcProfileLut[profileIndex * numAdcProfilesCoef + j];
                    }
                }
                else if (maskBit == ADI_ADRV9025_ORX4)
                {
                    for (j = 0; j < numAdcProfilesCoef; j++)
                    {
                        adcProfile->OrxChannel2Profile1[j] = adcProfileLut[profileIndex * numAdcProfilesCoef + j];
                    }
                }
                else
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     profileIndex,
                                     "Invalid ORx ADC Profile channel");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
        }
    }

    if (device->devStateInfo.profilesValid & ADI_ADRV9025_TX_PROFILE_VALID)
    {
        for (i = ADI_ADRV9025_MAX_LB_CHANNEL_START; i <= ADI_ADRV9025_MAX_LB_CHANNEL_END; i++)
        {
            maskBit = rxMaskArray[i];
            if ((device->devStateInfo.initializedChannels & maskBit) == maskBit)
            {
                orxProfile     = &init->rx.rxChannelCfg[i].profile;
                recoveryAction = adrv9025_DdcMultDivGet(device,
                                                        orxProfile->rxDdcMode,
                                                        &ddcMultiply,
                                                        &ddcDivide);
                ADI_ERROR_RETURN(device->common.error.newAction);

                adcClk_MHz = (orxProfile->rxOutputRate_kHz * orxProfile->rxFirDecimation *
                    orxProfile->rxFir1Decimation *
                    orxProfile->rhb1Decimation * orxProfile->rhb2Decimation * orxProfile->rhb3Decimation *
                    orxProfile->rxDec5Decimation * ddcMultiply / ddcDivide) / 1000;

                /* find correct ADC profile in the LUT */
                profileIndex = NUM_ADC_PROFILES;
                for (j = 0; j < NUM_ADC_PROFILES; j++)
                {
                    /* Find a row in the LUT that matches the ADC clock frequency */
                    if ((adcProfileLutIndex[j][1] == adcClk_MHz) && (adcProfileLutIndex[j][0] >= (orxProfile->rxAdcBandWidth_kHz / 1000)))
                    {
                        profileIndex = j;
                        break;
                    }
                }

                /* Verify that a profile was found in the LUT, if not return error */
                if (profileIndex >= NUM_ADC_PROFILES)
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     profileIndex,
                                     "Invalid Loopback ADC profileIndex ");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }

                if (maskBit == ADI_ADRV9025_LB12)
                {
                    for (j = 0; j < numAdcProfilesCoef; j++)
                    {
                        adcProfile->OrxChannel1Profile2[j] = adcProfileLut[profileIndex * numAdcProfilesCoef + j];
                        adcProfile->OrxChannel1Profile3[j] = adcProfileLut[profileIndex * numAdcProfilesCoef + j];
                    }
                }
                else if (maskBit == ADI_ADRV9025_LB34)
                {
                    for (j = 0; j < numAdcProfilesCoef; j++)
                    {
                        adcProfile->OrxChannel2Profile2[j] = adcProfileLut[profileIndex * numAdcProfilesCoef + j];
                        adcProfile->OrxChannel2Profile3[j] = adcProfileLut[profileIndex * numAdcProfilesCoef + j];
                    }
                }
                else
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     profileIndex,
                                     "Invalid Loopback ADC Profile channel");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
        }
    }

    return recoveryAction;
}

static void RemoveWhiteSpace(char* strUntrimmed)
{
    char* strTrimmed = strUntrimmed;

    while (*strUntrimmed != '\0')
    {
        if (!isspace(*strUntrimmed))
        {
            *strTrimmed = *strUntrimmed;
            strTrimmed++;
        }

        strUntrimmed++;
    }

    /* Null terminate the trimmed character array */
    *strTrimmed = '\0';
}

int32_t adrv9025_JsonFindStartOfFile(adi_adrv9025_Device_t* device,
                                     FILE*                  fp,
                                     char*                  fullStr,
                                     const char*            objNameStr,
                                     int32_t*               currChar)
{
    char* fileRead = NULL;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        objNameStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);

    if (fileRead != NULL)
    {
        RemoveWhiteSpace(fullStr);
        if (strstr(&fullStr[*currChar],
                   "{") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             fullStr,
                             "Could not find the start of JSON file");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_JsonFindEndOfFile(adi_adrv9025_Device_t* device,
                                   FILE*                  fp,
                                   char*                  fullStr,
                                   const char*            objNameStr,
                                   int32_t*               currChar)
{
    char* fileRead = NULL;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        objNameStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (strstr(&fullStr[*currChar],
                   "}") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             fullStr,
                             "Could not find the end of JSON file");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_JsonFindStartOfObject(adi_adrv9025_Device_t* device,
                                       FILE*                  fp,
                                       char*                  fullStr,
                                       const char*            objNameStr,
                                       int32_t*               currChar)
{
    char* fileRead     = NULL;
    char  matchStr[53] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        objNameStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

    /* going to be adding 5 additional characters to the string to be searched
     * including the null terminator */
    if (strlen(objNameStr) + 5 > sizeof(matchStr))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         objNameStr,
                         objectNameExceededMsg);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    strncat(matchStr,
            "\"",
            2);
    strncat(matchStr,
            objNameStr,
            strlen(objNameStr));
    strncat(matchStr,
            "\":{",
            4);

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (strstr(&fullStr[*currChar],
                   matchStr) == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             objNameStr,
                             "Could not find JSON object");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_JsonFindEndOfObject(adi_adrv9025_Device_t* device,
                                     FILE*                  fp,
                                     char*                  fullStr,
                                     const char*            objNameStr,
                                     int32_t*               currChar)
{
    char* fileRead = NULL;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        objNameStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (strstr(&fullStr[*currChar],
                   "}") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             objNameStr,
                             "Could not find end of JSON object");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_JsonFindStartOfArray(adi_adrv9025_Device_t* device,
                                      FILE*                  fp,
                                      char*                  fullStr,
                                      const char*            objNameStr,
                                      int32_t*               currChar)
{
    char* fileRead     = NULL;
    char  matchStr[53] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        objNameStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

    /* going to be adding 5 additional characters to the string to be searched
     * including the null terminator */
    if (strlen(objNameStr) + 5 > sizeof(matchStr))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         objNameStr,
                         objectNameExceededMsg);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    strncat(matchStr,
            "\"",
            2);
    strncat(matchStr,
            objNameStr,
            strlen(objNameStr));
    strncat(matchStr,
            "\":[",
            4);

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (strstr(&fullStr[*currChar],
                   matchStr) == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             objNameStr,
                             "Could not find end of JSON array");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_JsonFindEndOfArray(adi_adrv9025_Device_t* device,
                                    FILE*                  fp,
                                    char*                  fullStr,
                                    const char*            objNameStr,
                                    int32_t*               currChar)
{
    char* fileRead = NULL;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        objNameStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (strstr(&fullStr[*currChar],
                   "]") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             objNameStr,
                             "Could not find end of JSON array");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_JsonFindOpenCurlBrace(adi_adrv9025_Device_t* device,
                                       FILE*                  fp,
                                       char*                  fullStr,
                                       const char*            objNameStr,
                                       int32_t*               currChar)
{
    char* fileRead = NULL;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (strstr(&fullStr[*currChar],
                   "{") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             objNameStr,
                             "Could not find the open curled brace");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_JsonFindClosedCurlBrace(adi_adrv9025_Device_t* device,
                                         FILE*                  fp,
                                         char*                  fullStr,
                                         const char*            objNameStr,
                                         int32_t*               currChar)
{
    char* fileRead = NULL;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (strstr(&fullStr[*currChar],
                   "}") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             objNameStr,
                             "Could not find closed curled brace");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_JsonPeekNextKeyword(adi_adrv9025_Device_t* device,
                                     FILE*                  fp,
                                     char*                  fullStr,
                                     const char*            keyword)
{
    char*   fileRead   = NULL;
    int32_t matched    = 0;
    long    fileMarker = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);
    ADI_NULL_PTR_RETURN(&device->common,
                        fp);
    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);
    ADI_NULL_PTR_RETURN(&device->common,
                        keyword);

    fileMarker = ftell(fp);

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (strstr(fullStr,
                   keyword))
        {
            matched = 1;
        }
        /* reset the file marker back to where it was */
        fseek(fp,
              fileMarker,
              SEEK_SET);
    }

    return (matched);
}
int32_t adrv9025_JsonParseU64(adi_adrv9025_Device_t* device,
                              FILE*                  fp,
                              char*                  fullStr,
                              const char*            objNameStr,
                              int32_t*               currChar,
                              uint64_t*              value)
{
    char*    fileRead     = NULL;
    char     matchStr[48] = {0};
    uint64_t val          = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        objNameStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

    ADI_NULL_PTR_RETURN(&device->common,
                        value);

    if (strlen(objNameStr) > sizeof(matchStr))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         objNameStr,
                         "Object name string exceeds the maximum allowed length of 48");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    strcat(matchStr,
           "\"");
    strcat(matchStr,
           objNameStr);
    strcat(matchStr,
           "\":%llu");

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (sscanf(&fullStr[*currChar],
                   matchStr,
                   &val) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             objNameStr,
                             "Could not parse UInt64 value of the object");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            *value = val;
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}
int32_t adrv9025_JsonParseCalRsltFileStr(adi_adrv9025_Device_t* device,
                                         FILE*                  fp,
                                         char*                  fullStr,
                                         int32_t*               currChar)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

     fgets(fullStr, ADRV9025_MAX_FILE_BUFFER, fp);
    RemoveWhiteSpace(fullStr);

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_JsonParseI32(adi_adrv9025_Device_t* device,
                              FILE*                  fp,
                              char*                  fullStr,
                              const char*            objNameStr,
                              int32_t*               currChar,
                              int32_t*               value)
{
    char*   fileRead     = NULL;
    char    matchStr[54] = {0};
    int32_t val          = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        objNameStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

    ADI_NULL_PTR_RETURN(&device->common,
                        value);

    /* going to be adding 6 additional characters to the string to be searched
     * including the null terminator */
    if (strlen(objNameStr) + 6 > sizeof(matchStr))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         objNameStr,
                         objectNameExceededMsg);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    strncat(matchStr,
            "\"",
            2);
    strncat(matchStr,
            objNameStr,
            strlen(objNameStr));
    strncat(matchStr,
            "\":%d",
            5);

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (sscanf(&fullStr[*currChar],
                   matchStr,
                   &val) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             objNameStr,
                             "Could not parse Int32 value of the object");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            *value = val;
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_JsonParseU32(adi_adrv9025_Device_t* device,
                              FILE*                  fp,
                              char*                  fullStr,
                              const char*            objNameStr,
                              int32_t*               currChar,
                              uint32_t*              value)
{
    char*    fileRead     = NULL;
    char     matchStr[54] = {0};
    uint32_t val          = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        objNameStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

    ADI_NULL_PTR_RETURN(&device->common,
                        value);

    /* going to be adding 6 additional characters to the string to be searched
     * including the null terminator */
    if (strlen(objNameStr) + 6 > sizeof(matchStr))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         objNameStr,
                         objectNameExceededMsg);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    strncat(matchStr,
            "\"",
            2);
    strncat(matchStr,
            objNameStr,
            strlen(objNameStr));
    strncat(matchStr,
            "\":%u",
            5);

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (sscanf(&fullStr[*currChar],
                   matchStr,
                   &val) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             objNameStr,
                             "Could not parse UInt32 value of the object");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            *value = val;
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_JsonParseU16(adi_adrv9025_Device_t* device,
                              FILE*                  fp,
                              char*                  fullStr,
                              const char*            objNameStr,
                              int32_t*               currChar,
                              uint16_t*              value)
{
    char*    fileRead     = NULL;
    char     matchStr[55] = {0};
    uint16_t val          = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        objNameStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

    ADI_NULL_PTR_RETURN(&device->common,
                        value);

    /* going to be adding 7 additional characters to the string to be searched
     * including the null terminator */
    if (strlen(objNameStr) + 7 > sizeof(matchStr))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         objNameStr,
                         objectNameExceededMsg);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    strncat(matchStr,
            "\"",
            2);
    strncat(matchStr,
            objNameStr,
            strlen(objNameStr));
    strncat(matchStr,
            "\":%hu",
            6);

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (sscanf(&fullStr[*currChar],
                   matchStr,
                   &val) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             objNameStr,
                             "Could not parse UInt16 value of the object");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            *value = val;
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_JsonParseU8(adi_adrv9025_Device_t* device,
                             FILE*                  fp,
                             char*                  fullStr,
                             const char*            objNameStr,
                             int32_t*               currChar,
                             uint8_t*               value)
{
    char*   fileRead     = NULL;
    char    matchStr[56] = {0};
    uint8_t val          = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        objNameStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

    ADI_NULL_PTR_RETURN(&device->common,
                        value);

    /* going to be adding 8 additional characters to the string to be searched
     * including the null terminator */
    if (strlen(objNameStr) + 8 > sizeof(matchStr))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         objNameStr,
                         objectNameExceededMsg);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    strncat(matchStr,
            "\"",
            2);
    strncat(matchStr,
            objNameStr,
            strlen(objNameStr));
    strncat(matchStr,
            "\":%hhu",
            7);

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (sscanf(&fullStr[*currChar],
                   matchStr,
                   &val) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             objNameStr,
                             "Could not parse UInt8 value of the object");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            *value = val;
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_JsonParseI8(adi_adrv9025_Device_t* device,
                             FILE*                  fp,
                             char*                  fullStr,
                             const char*            objNameStr,
                             int32_t*               currChar,
                             int8_t*                value)
{
    char*  fileRead     = NULL;
    char   matchStr[56] = { 0 };
    int8_t val          = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        objNameStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

    ADI_NULL_PTR_RETURN(&device->common,
                        value);

    /* going to be adding 8 additional characters to the string to be searched
     * including the null terminator */
    if (strlen(objNameStr) + 8 > sizeof(matchStr))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         objNameStr,
                         objectNameExceededMsg);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    strncat(matchStr,
            "\"",
            2);
    strncat(matchStr,
            objNameStr,
            strlen(objNameStr));
    strncat(matchStr,
            "\":%hhd",
            7);

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (sscanf(&fullStr[*currChar],
                   matchStr,
                   &val) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             objNameStr,
                             "Could not parse Int8 value of the object");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            *value = val;
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_JsonParseUI8(adi_adrv9025_Device_t* device,
                              FILE*                  fp,
                              char*                  fullStr,
                              const char*            objNameStr,
                              int32_t*               currChar,
                              uint8_t*               value)
{
    char*  fileRead     = NULL;
    char   matchStr[56] = { 0 };
    int8_t val          = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        objNameStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

    ADI_NULL_PTR_RETURN(&device->common,
                        value);

    /* going to be adding 8 additional characters to the string to be searched
     * including the null terminator */
    if (strlen(objNameStr) + 8 > sizeof(matchStr))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         objNameStr,
                         objectNameExceededMsg);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    strncat(matchStr,
            "\"",
            2);
    strncat(matchStr,
            objNameStr,
            strlen(objNameStr));
    strncat(matchStr,
            "\":%hhd",
            7);

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (sscanf(&fullStr[*currChar],
                   matchStr,
                   &val) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             objNameStr,
                             "Could not parse Uint8 value of the object");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            *value = val;
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_JsonParseArrayElementI16(adi_adrv9025_Device_t* device,
                                          FILE*                  fp,
                                          char*                  fullStr,
                                          int*                   currChar,
                                          int16_t*               value)
{
    char*   fileRead = NULL;
    int16_t val      = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
                        currChar);

    ADI_NULL_PTR_RETURN(&device->common,
                        value);

    fileRead = fgets(fullStr,
                     ADRV9025_MAX_FILE_BUFFER,
                     fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (sscanf(&fullStr[*currChar],
                   "%hd",
                   &val) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             objNameStr,
                             "Could not parse Int16 array element value of the object");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            *value = val;
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}
uint32_t adrv9025_JsonParseArrayElementU16(adi_adrv9025_Device_t* device,
                                           FILE*                  fp,
                                           char*                  fullStr,
                                           int*                   currChar,
                                           uint16_t*               value)
{
    char*   fileRead = NULL;
    uint16_t val      = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
        ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
        fp);

    ADI_NULL_PTR_RETURN(&device->common,
        fullStr);

    ADI_NULL_PTR_RETURN(&device->common,
        currChar);

    ADI_NULL_PTR_RETURN(&device->common,
        value);

    fileRead = fgets(fullStr,
        ADRV9025_MAX_FILE_BUFFER,
        fp);
    RemoveWhiteSpace(fullStr);

    if (fileRead != NULL)
    {
        if (sscanf(&fullStr[*currChar],
            "%hd",
            &val) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                objNameStr,
                "Could not parse UInt16 array element value of the object");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            *value = val;
        }
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_JsonParseInitSpiSettings(adi_adrv9025_Device_t*      device,
                                          adi_adrv9025_SpiSettings_t* spiSettings,
                                          FILE*                       fp)
{
    int32_t recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    int32_t currChar                              = 0;
    char    lineStr[ADRV9025_MAX_FILE_BUFFER + 1] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        spiSettings);

    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    fp,
                                                    &lineStr[0],
                                                    "spiSettings",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonParseU8(device,
                                          fp,
                                          &lineStr[0],
                                          "msbFirst",
                                          &currChar,
                                          &spiSettings->msbFirst);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          fp,
                                          &lineStr[0],
                                          "enSpiStreaming",
                                          &currChar,
                                          &spiSettings->enSpiStreaming);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          fp,
                                          &lineStr[0],
                                          "autoIncAddrUp",
                                          &currChar,
                                          &spiSettings->autoIncAddrUp);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          fp,
                                          &lineStr[0],
                                          "fourWireMode",
                                          &currChar,
                                          &spiSettings->fourWireMode);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "cmosPadDrvStrength",
                                           &currChar,
                                           (uint32_t*)&spiSettings->cmosPadDrvStrength);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                  fp,
                                                  &lineStr[0],
                                                  "spiSettings",
                                                  &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_JsonParseInitClockSettings(adi_adrv9025_Device_t*        device,
                                            adi_adrv9025_ClockSettings_t* clockSettings,
                                            FILE*                         fp)
{
    int32_t recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    int32_t currChar                              = 0;
    char    lineStr[ADRV9025_MAX_FILE_BUFFER + 1] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        clockSettings);

    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    fp,
                                                    &lineStr[0],
                                                    "clocks",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "deviceClock_kHz",
                                           &currChar,
                                           &clockSettings->deviceClock_kHz);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "clkPllVcoFreq_kHz",
                                           &currChar,
                                           &clockSettings->clkPllVcoFreq_kHz);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "serdesPllVcoFreq_kHz",
                                           &currChar,
                                           &clockSettings->serdesPllVcoFreq_kHz);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          fp,
                                          &lineStr[0],
                                          "ldoSelect",
                                          &currChar,
                                          &clockSettings->ldoSelect);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "extLoFreq1_kHz",
                                           &currChar,
                                           &clockSettings->extLoFreq1_kHz);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "extLoFreq2_kHz",
                                           &currChar,
                                           &clockSettings->extLoFreq2_kHz);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "rfPll1LoMode",
                                           &currChar,
                                           (uint32_t*)&clockSettings->rfPll1LoMode);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "rfPll2LoMode",
                                           &currChar,
                                           (uint32_t*)&clockSettings->rfPll2LoMode);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          fp,
                                          &lineStr[0],
                                          "rfPll1LoOutDivider",
                                          &currChar,
                                          &clockSettings->rfPll1LoOutDivider);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          fp,
                                          &lineStr[0],
                                          "rfPll2LoOutDivider",
                                          &currChar,
                                          &clockSettings->rfPll2LoOutDivider);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "rfPllPhaseSyncMode",
                                           &currChar,
                                           (uint32_t*)&clockSettings->rfPllPhaseSyncMode);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "rx12LoSelect",
                                           &currChar,
                                           (uint32_t*)&clockSettings->rx12LoSelect);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "rx34LoSelect",
                                           &currChar,
                                           (uint32_t*)&clockSettings->rx34LoSelect);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "tx12LoSelect",
                                           &currChar,
                                           (uint32_t*)&clockSettings->tx12LoSelect);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "tx34LoSelect",
                                           &currChar,
                                           (uint32_t*)&clockSettings->tx34LoSelect);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "orx12LoSelect",
                                           &currChar,
                                           (uint32_t*)&clockSettings->orx12LoSelect);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "orx34LoSelect",
                                           &currChar,
                                           (uint32_t*)&clockSettings->orx34LoSelect);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                  fp,
                                                  &lineStr[0],
                                                  "clocks",
                                                  &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_JsonParseInitRxSettings(adi_adrv9025_Device_t*     device,
                                         adi_adrv9025_RxSettings_t* rxSettings,
                                         FILE*                      fp)
{
    int32_t  recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    int32_t  currChar                              = 0;
    char     lineStr[ADRV9025_MAX_FILE_BUFFER + 1] = {0};
    uint32_t i                                     = 0;
    uint32_t j                                     = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        rxSettings);

    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    fp,
                                                    &lineStr[0],
                                                    "rx",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "rxInitChannelMask",
                                           &currChar,
                                           &rxSettings->rxInitChannelMask);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "rxChannelCfg",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (i = 0; i < ADI_ADRV9025_MAX_RXCHANNELS; i++)
    {
        recoveryAction = adrv9025_JsonFindOpenCurlBrace(device,
                                                        fp,
                                                        &lineStr[0],
                                                        NULL,
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                        fp,
                                                        &lineStr[0],
                                                        "profile",
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "channelType",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.channelType);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "rxFirDecimation",
                                              &currChar,
                                              &rxSettings->rxChannelCfg[i].profile.rxFirDecimation);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "rxDec5Decimation",
                                              &currChar,
                                              &rxSettings->rxChannelCfg[i].profile.rxDec5Decimation);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "rhb1Decimation",
                                              &currChar,
                                              &rxSettings->rxChannelCfg[i].profile.rhb1Decimation);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "rhb1WideBandMode",
                                              &currChar,
                                              &rxSettings->rxChannelCfg[i].profile.rhb1WideBandMode);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "rhb2Decimation",
                                              &currChar,
                                              &rxSettings->rxChannelCfg[i].profile.rhb2Decimation);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "rhb3Decimation",
                                              &currChar,
                                              &rxSettings->rxChannelCfg[i].profile.rhb3Decimation);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "rxFir1Decimation",
                                              &currChar,
                                              &rxSettings->rxChannelCfg[i].profile.rxFir1Decimation);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "rxFir2Decimation",
                                              &currChar,
                                              &rxSettings->rxChannelCfg[i].profile.rxFir2Decimation);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "rxOutputRate_kHz",
                                               &currChar,
                                               &rxSettings->rxChannelCfg[i].profile.rxOutputRate_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "rfBandwidth_kHz",
                                               &currChar,
                                               &rxSettings->rxChannelCfg[i].profile.rfBandwidth_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "rxBbf3dBCorner_kHz",
                                               &currChar,
                                               &rxSettings->rxChannelCfg[i].profile.rxBbf3dBCorner_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "rxAdcBandWidth_kHz",
                                               &currChar,
                                               &rxSettings->rxChannelCfg[i].profile.rxAdcBandWidth_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                        fp,
                                                        &lineStr[0],
                                                        "rxFir",
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseI8(device,
                                              fp,
                                              &lineStr[0],
                                              "gain_dB",
                                              &currChar,
                                              &rxSettings->rxChannelCfg[i].profile.rxFir.gain_dB);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "numFirCoefs",
                                              &currChar,
                                              &rxSettings->rxChannelCfg[i].profile.rxFir.numFirCoefs);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Find Start of Rx PFIR Array */
        recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                       fp,
                                                       &lineStr[0],
                                                       "coefs",
                                                       &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        for (j = 0; j < ADI_ADRV9025_MAX_RXPFIR_COEFS; j++)
        {
            recoveryAction = adrv9025_JsonParseArrayElementI16(device,
                                                               fp,
                                                               &lineStr[0],
                                                               &currChar,
                                                               &(rxSettings->rxChannelCfg[i].profile.rxFir.coefs[j]));
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Find End of Rx PFIR Array */
        recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                     fp,
                                                     &lineStr[0],
                                                     "coefs",
                                                     &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                      fp,
                                                      &lineStr[0],
                                                      "rxFir",
                                                      &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "rxDdcMode",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDdcMode);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                        fp,
                                                        &lineStr[0],
                                                        "rxNcoShifterCfg",
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "bandAInputBandWidth_kHz",
                                               &currChar,
                                               &rxSettings->rxChannelCfg[i].profile.rxNcoShifterCfg.bandAInputBandWidth_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseI32(device,
                                               fp,
                                               &lineStr[0],
                                               "bandAInputCenterFreq_kHz",
                                               &currChar,
                                               &rxSettings->rxChannelCfg[i].profile.rxNcoShifterCfg.bandAInputCenterFreq_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseI32(device,
                                               fp,
                                               &lineStr[0],
                                               "bandANco1Freq_kHz",
                                               &currChar,
                                               &rxSettings->rxChannelCfg[i].profile.rxNcoShifterCfg.bandANco1Freq_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseI32(device,
                                               fp,
                                               &lineStr[0],
                                               "bandANco2Freq_kHz",
                                               &currChar,
                                               &rxSettings->rxChannelCfg[i].profile.rxNcoShifterCfg.bandANco2Freq_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "bandBInputBandWidth_kHz",
                                               &currChar,
                                               &rxSettings->rxChannelCfg[i].profile.rxNcoShifterCfg.bandBInputBandWidth_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseI32(device,
                                               fp,
                                               &lineStr[0],
                                               "bandBInputCenterFreq_kHz",
                                               &currChar,
                                               &rxSettings->rxChannelCfg[i].profile.rxNcoShifterCfg.bandBInputCenterFreq_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseI32(device,
                                               fp,
                                               &lineStr[0],
                                               "bandBNco1Freq_kHz",
                                               &currChar,
                                               &rxSettings->rxChannelCfg[i].profile.rxNcoShifterCfg.bandBNco1Freq_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseI32(device,
                                               fp,
                                               &lineStr[0],
                                               "bandBNco2Freq_kHz",
                                               &currChar,
                                               &rxSettings->rxChannelCfg[i].profile.rxNcoShifterCfg.bandBNco2Freq_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseI32(device,
                                               fp,
                                               &lineStr[0],
                                               "bandAbCombinedEnable",
                                               &currChar,
                                               &rxSettings->rxChannelCfg[i].profile.rxNcoShifterCfg.bandAbCombinedEnable);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                      fp,
                                                      &lineStr[0],
                                                      "rxNcoShifterCfg",
                                                      &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "tiaPowerMode",
                                              &currChar,
                                              &rxSettings->rxChannelCfg[i].profile.tiaPowerMode);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                        fp,
                                                        &lineStr[0],
                                                        "rxDataFormat",
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "rxChannelMask",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.rxChannelMask);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "formatSelect",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.formatSelect);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                        fp,
                                                        &lineStr[0],
                                                        "floatingPointConfig",
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "fpDataFormat",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.floatingPointConfig.fpDataFormat);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "fpRoundMode",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.floatingPointConfig.fpRoundMode);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "fpNumExpBits",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.floatingPointConfig.fpNumExpBits);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "fpAttenSteps",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.floatingPointConfig.fpAttenSteps);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "fpHideLeadingOne",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.floatingPointConfig.fpHideLeadingOne);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "fpEncodeNan",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.floatingPointConfig.fpEncodeNan);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                      fp,
                                                      &lineStr[0],
                                                      "floatingPointConfig",
                                                      &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                        fp,
                                                        &lineStr[0],
                                                        "integerConfigSettings",
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "intEmbeddedBits",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.integerConfigSettings.intEmbeddedBits);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "intSampleResolution",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.integerConfigSettings.intSampleResolution);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "intParity",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.integerConfigSettings.intParity);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "intEmbeddedPos",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.integerConfigSettings.intEmbeddedPos);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                      fp,
                                                      &lineStr[0],
                                                      "integerConfigSettings",
                                                      &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                        fp,
                                                        &lineStr[0],
                                                        "slicerConfigSettings",
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "extSlicerStepSize",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.slicerConfigSettings.extSlicerStepSize);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "intSlicerStepSize",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.slicerConfigSettings.intSlicerStepSize);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "rx1ExtSlicerGpioSelect",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.slicerConfigSettings.rx1ExtSlicerGpioSelect);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "rx2ExtSlicerGpioSelect",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.slicerConfigSettings.rx2ExtSlicerGpioSelect);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "rx3ExtSlicerGpioSelect",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.slicerConfigSettings.rx3ExtSlicerGpioSelect);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "rx4ExtSlicerGpioSelect",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.slicerConfigSettings.rx4ExtSlicerGpioSelect);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                      fp,
                                                      &lineStr[0],
                                                      "slicerConfigSettings",
                                                      &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                        fp,
                                                        &lineStr[0],
                                                        "embOvldMonitorSettings",
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "embeddedMonitorSrcLsbI",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.embOvldMonitorSettings.embeddedMonitorSrcLsbI);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "embeddedMonitorSrcLsbQ",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.embOvldMonitorSettings.embeddedMonitorSrcLsbQ);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "embeddedMonitorSrcLsbPlusOneI",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i]
                                                           .profile.rxDataFormat.embOvldMonitorSettings.embeddedMonitorSrcLsbPlusOneI);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "embeddedMonitorSrcLsbPlusOneQ",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i]
                                                           .profile.rxDataFormat.embOvldMonitorSettings.embeddedMonitorSrcLsbPlusOneQ);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "embeddedMonitorHb2LowSrcSel",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.embOvldMonitorSettings.embeddedMonitorHb2LowSrcSel);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "embeddedMonitorHb2HighSrcSel",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i]
                                                           .profile.rxDataFormat.embOvldMonitorSettings.embeddedMonitorHb2HighSrcSel);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "embeddedMonitorApdLowSrcSel",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i].profile.rxDataFormat.embOvldMonitorSettings.embeddedMonitorApdLowSrcSel);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "embeddedMonitorApdHighSrcSel",
                                               &currChar,
                                               (uint32_t*)&rxSettings->rxChannelCfg[i]
                                                           .profile.rxDataFormat.embOvldMonitorSettings.embeddedMonitorApdHighSrcSel);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "invertHb2Flag",
                                              &currChar,
                                              &rxSettings->rxChannelCfg[i].profile.rxDataFormat.embOvldMonitorSettings.invertHb2Flag);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "invertApdFlag",
                                              &currChar,
                                              &rxSettings->rxChannelCfg[i].profile.rxDataFormat.embOvldMonitorSettings.invertApdFlag);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                      fp,
                                                      &lineStr[0],
                                                      "embOvldMonitorSettings",
                                                      &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "externalLnaGain",
                                              &currChar,
                                              &rxSettings->rxChannelCfg[i].profile.rxDataFormat.externalLnaGain);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "tempCompensationEnable",
                                              &currChar,
                                              &rxSettings->rxChannelCfg[i].profile.rxDataFormat.tempCompensationEnable);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                      fp,
                                                      &lineStr[0],
                                                      "rxDataFormat",
                                                      &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                      fp,
                                                      &lineStr[0],
                                                      "profile",
                                                      &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindClosedCurlBrace(device,
                                                          fp,
                                                          &lineStr[0],
                                                          NULL,
                                                          &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "rxChannelCfg",
                                                 &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                  fp,
                                                  &lineStr[0],
                                                  "rx",
                                                  &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}
int32_t adrv9025_JsonParseInitAdcProfileSettings(adi_adrv9025_Device_t*      device,
                                                 adi_adrv9025_AdcProfiles_t* adcProfiles,
                                                 FILE*                       fp)
{
    int32_t  recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    int32_t  currChar                              = 0;
    char     lineStr[ADRV9025_MAX_FILE_BUFFER + 1] = { 0 };
    uint32_t j                                     = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        adcProfiles);

    /* Find Start of Rx ADC Profiles Object*/
    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    fp,
                                                    &lineStr[0],
                                                    "adcProfiles",
                                                    &currChar);
    if (recoveryAction == ADI_COMMON_ACT_ERR_CHECK_PARAM)
    {
        /* if not found then set the inti struct ADC profiles to 0, clear the error and return no error
         * this is to keep backwards compatibility*/
        memset(adcProfiles->RxChannel1, 0, ADC_PROFILE_MAX*sizeof(uint16_t));
        memset(adcProfiles->RxChannel2, 0, ADC_PROFILE_MAX*sizeof(uint16_t));
        memset(adcProfiles->RxChannel3, 0, ADC_PROFILE_MAX*sizeof(uint16_t));
        memset(adcProfiles->RxChannel4, 0, ADC_PROFILE_MAX*sizeof(uint16_t));
        memset(adcProfiles->OrxChannel1Profile0, 0, ADC_PROFILE_MAX*sizeof(uint16_t));
        memset(adcProfiles->OrxChannel1Profile1, 0, ADC_PROFILE_MAX*sizeof(uint16_t));
        memset(adcProfiles->OrxChannel1Profile2, 0, ADC_PROFILE_MAX*sizeof(uint16_t));
        memset(adcProfiles->OrxChannel1Profile3, 0, ADC_PROFILE_MAX*sizeof(uint16_t));
        memset(adcProfiles->OrxChannel2Profile0, 0, ADC_PROFILE_MAX*sizeof(uint16_t));
        memset(adcProfiles->OrxChannel2Profile1, 0, ADC_PROFILE_MAX*sizeof(uint16_t));
        memset(adcProfiles->OrxChannel2Profile2, 0, ADC_PROFILE_MAX*sizeof(uint16_t));
        memset(adcProfiles->OrxChannel2Profile3, 0, ADC_PROFILE_MAX*sizeof(uint16_t));
        adcProfiles->OrxChannel1Index = 0;
        adcProfiles->OrxChannel2Index = 0;
        recoveryAction = adrv9025_ErrorClear(&device->common);
        return recoveryAction;
    }
    else
    {
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Find Start of RxChannel1 ADC profile*/
    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "RxChannel1",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (j = 0; j < ADC_PROFILE_MAX; j++)
    {
        recoveryAction = adrv9025_JsonParseArrayElementU16(device,
                                                           fp,
                                                           &lineStr[0],
                                                           &currChar,
                                                           &(adcProfiles->RxChannel1[j]));
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Find End of RxChannel1 ADC profile array*/
    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "RxChannel1",
                                                 &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Find Start of RxChannel2 array*/
    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "RxChannel2",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (j = 0; j < ADC_PROFILE_MAX; j++)
    {
        recoveryAction = adrv9025_JsonParseArrayElementU16(device,
                                                           fp,
                                                           &lineStr[0],
                                                           &currChar,
                                                           &(adcProfiles->RxChannel2[j]));
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Find End of RxChannel2 array*/
    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "RxChannel2",
                                                 &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Find Start of RxChannel3 array*/
    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "RxChannel3",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (j = 0; j < ADC_PROFILE_MAX; j++)
    {
        recoveryAction = adrv9025_JsonParseArrayElementU16(device,
                                                           fp,
                                                           &lineStr[0],
                                                           &currChar,
                                                           &(adcProfiles->RxChannel3[j]));
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Find End of RxChannel3 array*/
    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "RxChannel3",
                                                 &currChar);

    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Find Start of RxChannel4 array*/
    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "RxChannel4",
                                                   &currChar);

    for (j = 0; j < ADC_PROFILE_MAX; j++)
    {
        recoveryAction = adrv9025_JsonParseArrayElementU16(device,
                                                           fp,
                                                           &lineStr[0],
                                                           &currChar,
                                                           &(adcProfiles->RxChannel4[j]));
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Find End of RxChannel4 array*/
    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "RxChannel4",
                                                 &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Find Start of OrxChannel1Profile0 array*/
    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "OrxChannel1Profile0",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (j = 0; j < ADC_PROFILE_MAX; j++)
    {
        recoveryAction = adrv9025_JsonParseArrayElementU16(device,
                                                           fp,
                                                           &lineStr[0],
                                                           &currChar,
                                                           &(adcProfiles->OrxChannel1Profile0[j]));
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Find End of OrxChannel1Profile0 array*/
    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "OrxChannel1Profile0",
                                                 &currChar);

    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Find Start of OrxChannel1Profile1 array*/
    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "OrxChannel1Profile1",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (j = 0; j < ADC_PROFILE_MAX; j++)
    {
        recoveryAction = adrv9025_JsonParseArrayElementU16(device,
                                                           fp,
                                                           &lineStr[0],
                                                           &currChar,
                                                           &(adcProfiles->OrxChannel1Profile1[j]));
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Find End of OrxChannel1Profile1 array*/
    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "OrxChannel1Profile1",
                                                 &currChar);

    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Find Start of OrxChannel1Profile2 array*/
    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "OrxChannel1Profile2",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (j = 0; j < ADC_PROFILE_MAX; j++)
    {
        recoveryAction = adrv9025_JsonParseArrayElementU16(device,
                                                           fp,
                                                           &lineStr[0],
                                                           &currChar,
                                                           &(adcProfiles->OrxChannel1Profile2[j]));
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Find End of OrxChannel1Profile2 array*/
    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "OrxChannel1Profile2",
                                                 &currChar);

    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Find Start of OrxChannel1Profile3 array*/
    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "OrxChannel1Profile3",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (j = 0; j < ADC_PROFILE_MAX; j++)
    {
        recoveryAction = adrv9025_JsonParseArrayElementU16(device,
                                                           fp,
                                                           &lineStr[0],
                                                           &currChar,
                                                           &(adcProfiles->OrxChannel1Profile3[j]));
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Find End of OrxChannel1Profile3 array*/
    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "OrxChannel1Profile3",
                                                 &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Find Start of OrxChannel2Profile0 array*/
    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "OrxChannel2Profile0",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (j = 0; j < ADC_PROFILE_MAX; j++)
    {
        recoveryAction = adrv9025_JsonParseArrayElementU16(device,
                                                           fp,
                                                           &lineStr[0],
                                                           &currChar,
                                                           &(adcProfiles->OrxChannel2Profile0[j]));
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Find End of OrxChannel2Profile0 array*/
    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "OrxChannel2Profile0",
                                                 &currChar);

    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Find Start of OrxChannel2Profile1 array*/
    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "OrxChannel2Profile1",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (j = 0; j < ADC_PROFILE_MAX; j++)
    {
        recoveryAction = adrv9025_JsonParseArrayElementU16(device,
                                                           fp,
                                                           &lineStr[0],
                                                           &currChar,
                                                           &(adcProfiles->OrxChannel2Profile1[j]));
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Find End of OrxChannel2Profile1 array*/
    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "OrxChannel2Profile1",
                                                 &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Find Start of OrxChannel2Profile2 array*/
    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "OrxChannel2Profile2",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (j = 0; j < ADC_PROFILE_MAX; j++)
    {
        recoveryAction = adrv9025_JsonParseArrayElementU16(device,
                                                           fp,
                                                           &lineStr[0],
                                                           &currChar,
                                                           &(adcProfiles->OrxChannel2Profile2[j]));
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Find End of OrxChannel2Profile2 array*/
    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "OrxChannel2Profile2",
                                                 &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Find Start of OrxChannel2Profile3 array*/
    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "OrxChannel2Profile3",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (j = 0; j < ADC_PROFILE_MAX; j++)
    {
        recoveryAction = adrv9025_JsonParseArrayElementU16(device,
                                                           fp,
                                                           &lineStr[0],
                                                           &currChar,
                                                           &(adcProfiles->OrxChannel2Profile3[j]));
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Find End of OrxChannel2Profile3 array*/
    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "OrxChannel2Profile3",
                                                 &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);


    recoveryAction = adrv9025_JsonParseUI8(device,
                                           fp,
                                           &lineStr[0],
                                           "OrxChannel1Index",
                                           &currChar,
                                           &(adcProfiles->OrxChannel1Index));
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonParseUI8(device,
                                           fp,
                                           &lineStr[0],
                                           "OrxChannel2Index",
                                           &currChar,
                                           &(adcProfiles->OrxChannel2Index));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Find End of ADC profiles */
    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                  fp,
                                                  &lineStr[0],
                                                  "rxadcprofiles",
                                                  &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_JsonParseInitTxSettings(adi_adrv9025_Device_t*     device,
                                         adi_adrv9025_TxSettings_t* txSettings,
                                         FILE*                      fp)
{
    int32_t  recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    int32_t  currChar                              = 0;
    char     lineStr[ADRV9025_MAX_FILE_BUFFER + 1] = {0};
    uint32_t i                                     = 0;
    uint32_t j                                     = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        txSettings);

    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    fp,
                                                    &lineStr[0],
                                                    "tx",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "txInitChannelMask",
                                           &currChar,
                                           &txSettings->txInitChannelMask);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "txChannelCfg",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (i = 0; i < ADI_ADRV9025_MAX_TXCHANNELS; i++)
    {
        recoveryAction = adrv9025_JsonFindOpenCurlBrace(device,
                                                        fp,
                                                        &lineStr[0],
                                                        NULL,
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                        fp,
                                                        &lineStr[0],
                                                        "profile",
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "txInputRate_kHz",
                                               &currChar,
                                               &txSettings->txChannelCfg[i].profile.txInputRate_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "primarySigBandwidth_kHz",
                                               &currChar,
                                               &txSettings->txChannelCfg[i].profile.primarySigBandwidth_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "rfBandwidth_kHz",
                                               &currChar,
                                               &txSettings->txChannelCfg[i].profile.rfBandwidth_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "txDac3dBCorner_kHz",
                                               &currChar,
                                               &txSettings->txChannelCfg[i].profile.txDac3dBCorner_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "txBbf3dBCorner_kHz",
                                               &currChar,
                                               &txSettings->txChannelCfg[i].profile.txBbf3dBCorner_kHz);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "dpdHb1Interpolation",
                                              &currChar,
                                              &txSettings->txChannelCfg[i].profile.dpdHb1Interpolation);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "dpdHb2Interpolation",
                                              &currChar,
                                              &txSettings->txChannelCfg[i].profile.dpdHb2Interpolation);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "txFirInterpolation",
                                              &currChar,
                                              &txSettings->txChannelCfg[i].profile.txFirInterpolation);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "thb1Interpolation",
                                              &currChar,
                                              &txSettings->txChannelCfg[i].profile.thb1Interpolation);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "thb2Interpolation",
                                              &currChar,
                                              &txSettings->txChannelCfg[i].profile.thb2Interpolation);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "thb3Interpolation",
                                              &currChar,
                                              &txSettings->txChannelCfg[i].profile.thb3Interpolation);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "txInt5Interpolation",
                                              &currChar,
                                              &txSettings->txChannelCfg[i].profile.txInt5Interpolation);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                        fp,
                                                        &lineStr[0],
                                                        "txFir",
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseI8(device,
                                              fp,
                                              &lineStr[0],
                                              "gain_dB",
                                              &currChar,
                                              &txSettings->txChannelCfg[i].profile.txFir.gain_dB);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "numFirCoefs",
                                              &currChar,
                                              &txSettings->txChannelCfg[i].profile.txFir.numFirCoefs);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Find Start of Tx PFIR Array */
        recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                       fp,
                                                       &lineStr[0],
                                                       "coefs",
                                                       &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        for (j = 0; j < ADI_ADRV9025_MAX_TXPFIR_COEFS; j++)
        {
            recoveryAction = adrv9025_JsonParseArrayElementI16(device,
                                                               fp,
                                                               &lineStr[0],
                                                               &currChar,
                                                               &(txSettings->txChannelCfg[i].profile.txFir.coefs[j]));
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Find End of Tx PFIR Array */
        recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                     fp,
                                                     &lineStr[0],
                                                     "coefs",
                                                     &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                      fp,
                                                      &lineStr[0],
                                                      "txFir",
                                                      &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "txBbfPowerMode",
                                              &currChar,
                                              &txSettings->txChannelCfg[i].profile.txBbfPowerMode);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Find End of Tx Profile Array */
        recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                      fp,
                                                      &lineStr[0],
                                                      "profile",
                                                      &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                        fp,
                                                        &lineStr[0],
                                                        "txAttenCtrl",
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "txAttenStepSize",
                                               &currChar,
                                               (uint32_t*)&txSettings->txChannelCfg[i].txAttenCtrl.txAttenStepSize);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "txRampDownEvents",
                                               &currChar,
                                               (uint32_t*)&txSettings->txChannelCfg[i].txAttenCtrl.txRampDownEvents);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "reserved",
                                               &currChar,
                                               (uint32_t*)&txSettings->txChannelCfg[i].txAttenCtrl.reserved);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "attenMode",
                                               &currChar,
                                               (uint32_t*)&txSettings->txChannelCfg[i].txAttenCtrl.attenMode);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* The "dacFUllScale" was moved out of the block but some profiles still have it to maintain init structure  */
        if ((recoveryAction = adrv9025_JsonPeekNextKeyword(device,
                                                           fp,
                                                           &lineStr[0],
                                                           "dacFullScale")) > 0)
        {
            recoveryAction = adrv9025_JsonParseU32(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "dacFullScale",
                                                   &currChar,
                                                   (uint32_t*)&txSettings->txChannelCfg[i].txAttenCtrl.dacFullScale);
            //reserved field to maintain init structure (not used)
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        recoveryAction = adrv9025_JsonParseU16(device,
                                               fp,
                                               &lineStr[0],
                                               "txAttenInit_mdB",
                                               &currChar,
                                               &txSettings->txChannelCfg[i].txAttenCtrl.txAttenInit_mdB);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                      fp,
                                                      &lineStr[0],
                                                      "txAttenCtrl",
                                                      &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "dacFullScale",
                                               &currChar,
                                               (uint32_t*)&txSettings->txChannelCfg[i].dacFullScale);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindClosedCurlBrace(device,
                                                          fp,
                                                          &lineStr[0],
                                                          NULL,
                                                          &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "txChannelCfg",
                                                 &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                  fp,
                                                  &lineStr[0],
                                                  "tx",
                                                  &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_JsonParseInitDataInterfaceSettings(adi_adrv9025_Device_t*           device,
                                                    adi_adrv9025_DataInterfaceCfg_t* dataInterfaceSettings,
                                                    FILE*                            fp)
{
    int32_t  recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    int32_t  currChar                              = 0;
    char     lineStr[ADRV9025_MAX_FILE_BUFFER + 1] = {0};
    uint32_t i                                     = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        dataInterfaceSettings);

    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    fp,
                                                    &lineStr[0],
                                                    "dataInterface",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "framer",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (i = 0; i < ADI_ADRV9025_NUM_FRAMERS; i++)
    {
        recoveryAction = adrv9025_JsonFindOpenCurlBrace(device,
                                                        fp,
                                                        &lineStr[0],
                                                        NULL,
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                        fp,
                                                        &lineStr[0],
                                                        "serializerLaneCrossbar",
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "lane0FramerOutSel",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].serializerLaneCrossbar.lane0FramerOutSel);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "lane1FramerOutSel",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].serializerLaneCrossbar.lane1FramerOutSel);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "lane2FramerOutSel",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].serializerLaneCrossbar.lane2FramerOutSel);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "lane3FramerOutSel",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].serializerLaneCrossbar.lane3FramerOutSel);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                      fp,
                                                      &lineStr[0],
                                                      "serializerLaneCrossbar",
                                                      &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                        fp,
                                                        &lineStr[0],
                                                        "adcCrossbar",
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv0",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv0);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv1",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv1);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv2",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv2);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv3",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv3);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv4",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv4);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv5",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv5);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv6",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv6);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv7",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv7);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv8",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv8);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv9",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv9);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv10",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv10);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv11",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv11);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv12",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv12);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv13",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv13);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv14",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv14);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv15",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv15);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv16",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv16);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv17",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv17);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv18",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv18);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv19",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv19);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv20",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv20);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv21",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv21);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv22",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv22);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "conv23",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->framer[i].adcCrossbar.conv23);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                      fp,
                                                      &lineStr[0],
                                                      "adcCrossbar",
                                                      &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "enableJesd204C",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].enableJesd204C);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "bankId",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].bankId);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "deviceId",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].deviceId);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "lane0Id",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].lane0Id);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "jesd204M",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].jesd204M);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU16(device,
                                               fp,
                                               &lineStr[0],
                                               "jesd204K",
                                               &currChar,
                                               &dataInterfaceSettings->framer[i].jesd204K);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "jesd204F",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].jesd204F);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "jesd204Np",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].jesd204Np);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "jesd204E",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].jesd204E);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "scramble",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].scramble);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "serializerLanesEnabled",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].serializerLanesEnabled);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU16(device,
                                               fp,
                                               &lineStr[0],
                                               "lmfcOffset",
                                               &currChar,
                                               &dataInterfaceSettings->framer[i].lmfcOffset);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "syncbInSelect",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].syncbInSelect);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "overSample",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].overSample);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "syncbInLvdsMode",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].syncbInLvdsMode);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "syncbInLvdsPnInvert",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].syncbInLvdsPnInvert);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "newSysrefOnRelink",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].newSysrefOnRelink);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "sysrefForStartup",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].sysrefForStartup);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "sysrefNShotEnable",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].sysrefNShotEnable);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "sysrefNShotCount",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].sysrefNShotCount);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "sysrefIgnoreWhenLinked",
                                              &currChar,
                                              &dataInterfaceSettings->framer[i].sysrefIgnoreWhenLinked);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindClosedCurlBrace(device,
                                                          fp,
                                                          &lineStr[0],
                                                          NULL,
                                                          &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "framer",
                                                 &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "deframer",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (i = 0; i < ADI_ADRV9025_NUM_DEFRAMERS; i++)
    {
        recoveryAction = adrv9025_JsonFindOpenCurlBrace(device,
                                                        fp,
                                                        &lineStr[0],
                                                        NULL,
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                        fp,
                                                        &lineStr[0],
                                                        "deserializerLaneCrossbar",
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "deframerInput0LaneSel",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].deserializerLaneCrossbar.deframerInput0LaneSel);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "deframerInput1LaneSel",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].deserializerLaneCrossbar.deframerInput1LaneSel);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "deframerInput2LaneSel",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].deserializerLaneCrossbar.deframerInput2LaneSel);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "deframerInput3LaneSel",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].deserializerLaneCrossbar.deframerInput3LaneSel);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                      fp,
                                                      &lineStr[0],
                                                      "deserializerLaneCrossbar",
                                                      &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "enableJesd204C",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].enableJesd204C);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "bankId",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].bankId);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "deviceId",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].deviceId);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "lane0Id",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].lane0Id);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "jesd204M",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].jesd204M);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU16(device,
                                               fp,
                                               &lineStr[0],
                                               "jesd204K",
                                               &currChar,
                                               &dataInterfaceSettings->deframer[i].jesd204K);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "jesd204F",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].jesd204F);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "jesd204Np",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].jesd204Np);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "jesd204E",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].jesd204E);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "scramble",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].scramble);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "deserializerLanesEnabled",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].deserializerLanesEnabled);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU16(device,
                                               fp,
                                               &lineStr[0],
                                               "lmfcOffset",
                                               &currChar,
                                               &dataInterfaceSettings->deframer[i].lmfcOffset);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "syncbOutSelect",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].syncbOutSelect);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "syncbOutLvdsMode",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].syncbOutLvdsMode);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "syncbOutLvdsPnInvert",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].syncbOutLvdsPnInvert);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "syncbOutCmosSlewRate",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].syncbOutCmosSlewRate);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "syncbOutCmosDriveLevel",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].syncbOutCmosDriveLevel);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                        fp,
                                                        &lineStr[0],
                                                        "dacCrossbar",
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "tx1DacChanI",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->deframer[i].dacCrossbar.tx1DacChanI);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "tx1DacChanQ",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->deframer[i].dacCrossbar.tx1DacChanQ);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "tx2DacChanI",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->deframer[i].dacCrossbar.tx2DacChanI);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "tx2DacChanQ",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->deframer[i].dacCrossbar.tx2DacChanQ);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "tx3DacChanI",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->deframer[i].dacCrossbar.tx3DacChanI);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "tx3DacChanQ",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->deframer[i].dacCrossbar.tx3DacChanQ);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "tx4DacChanI",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->deframer[i].dacCrossbar.tx4DacChanI);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "tx4DacChanQ",
                                               &currChar,
                                               (uint32_t*)&dataInterfaceSettings->deframer[i].dacCrossbar.tx4DacChanQ);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                      fp,
                                                      &lineStr[0],
                                                      "dacCrossbar",
                                                      &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "newSysrefOnRelink",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].newSysrefOnRelink);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "sysrefForStartup",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].sysrefForStartup);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "sysrefNShotEnable",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].sysrefNShotEnable);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "sysrefNShotCount",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].sysrefNShotCount);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "sysrefIgnoreWhenLinked",
                                              &currChar,
                                              &dataInterfaceSettings->deframer[i].sysrefIgnoreWhenLinked);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindClosedCurlBrace(device,
                                                          fp,
                                                          &lineStr[0],
                                                          NULL,
                                                          &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "deframer",
                                                 &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "serCfg",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (i = 0; i < ADI_ADRV9025_NUM_LANES; i++)
    {
        recoveryAction = adrv9025_JsonFindOpenCurlBrace(device,
                                                        fp,
                                                        &lineStr[0],
                                                        NULL,
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "serAmplitude",
                                              &currChar,
                                              &dataInterfaceSettings->serCfg[i].serAmplitude);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "serPreEmphasis",
                                              &currChar,
                                              &dataInterfaceSettings->serCfg[i].serPreEmphasis);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "serPostEmphasis",
                                              &currChar,
                                              &dataInterfaceSettings->serCfg[i].serPostEmphasis);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "serInvertLanePolarity",
                                              &currChar,
                                              &dataInterfaceSettings->serCfg[i].serInvertLanePolarity);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindClosedCurlBrace(device,
                                                          fp,
                                                          &lineStr[0],
                                                          NULL,
                                                          &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "serCfg",
                                                 &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindStartOfArray(device,
                                                   fp,
                                                   &lineStr[0],
                                                   "desCfg",
                                                   &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (i = 0; i < ADI_ADRV9025_NUM_LANES; i++)
    {
        recoveryAction = adrv9025_JsonFindOpenCurlBrace(device,
                                                        fp,
                                                        &lineStr[0],
                                                        NULL,
                                                        &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "highBoost",
                                              &currChar,
                                              &dataInterfaceSettings->desCfg[i].highBoost);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "configOption1",
                                               &currChar,
                                               &dataInterfaceSettings->desCfg[i].configOption1);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "configOption2",
                                               &currChar,
                                               &dataInterfaceSettings->desCfg[i].configOption2);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "configOption3",
                                               &currChar,
                                               &dataInterfaceSettings->desCfg[i].configOption3);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "configOption4",
                                               &currChar,
                                               &dataInterfaceSettings->desCfg[i].configOption4);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "configOption5",
                                               &currChar,
                                               &dataInterfaceSettings->desCfg[i].configOption5);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "configOption6",
                                               &currChar,
                                               &dataInterfaceSettings->desCfg[i].configOption6);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "configOption7",
                                               &currChar,
                                               &dataInterfaceSettings->desCfg[i].configOption7);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "configOption8",
                                               &currChar,
                                               &dataInterfaceSettings->desCfg[i].configOption8);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "configOption9",
                                               &currChar,
                                               &dataInterfaceSettings->desCfg[i].configOption9);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU32(device,
                                               fp,
                                               &lineStr[0],
                                               "configOption10",
                                               &currChar,
                                               &dataInterfaceSettings->desCfg[i].configOption10);
        ADI_ERROR_RETURN(device->common.error.newAction);
        recoveryAction = adrv9025_JsonParseU8(device,
                                              fp,
                                              &lineStr[0],
                                              "desInvertLanePolarity",
                                              &currChar,
                                              &dataInterfaceSettings->desCfg[i].desInvertLanePolarity);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_JsonFindClosedCurlBrace(device,
                                                          fp,
                                                          &lineStr[0],
                                                          NULL,
                                                          &currChar);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_JsonFindEndOfArray(device,
                                                 fp,
                                                 &lineStr[0],
                                                 "desCfg",
                                                 &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    fp,
                                                    &lineStr[0],
                                                    "linkSharingCfg",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonParseU8(device,
                                          fp,
                                          &lineStr[0],
                                          "linkSharingEnabled",
                                          &currChar,
                                          &dataInterfaceSettings->linkSharingCfg.linkSharingEnabled);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          fp,
                                          &lineStr[0],
                                          "linkSharingM",
                                          &currChar,
                                          &dataInterfaceSettings->linkSharingCfg.linkSharingM);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          fp,
                                          &lineStr[0],
                                          "linkSharingS",
                                          &currChar,
                                          &dataInterfaceSettings->linkSharingCfg.linkSharingS);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          fp,
                                          &lineStr[0],
                                          "linkSharingNp",
                                          &currChar,
                                          &dataInterfaceSettings->linkSharingCfg.linkSharingNp);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    fp,
                                                    &lineStr[0],
                                                    "linkSharingAdcCrossbar",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "conv0",
                                           &currChar,
                                           (uint32_t*)&dataInterfaceSettings->linkSharingCfg.linkSharingAdcCrossbar.conv0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "conv1",
                                           &currChar,
                                           (uint32_t*)&dataInterfaceSettings->linkSharingCfg.linkSharingAdcCrossbar.conv1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "conv2",
                                           &currChar,
                                           (uint32_t*)&dataInterfaceSettings->linkSharingCfg.linkSharingAdcCrossbar.conv2);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "conv3",
                                           &currChar,
                                           (uint32_t*)&dataInterfaceSettings->linkSharingCfg.linkSharingAdcCrossbar.conv3);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "conv4",
                                           &currChar,
                                           (uint32_t*)&dataInterfaceSettings->linkSharingCfg.linkSharingAdcCrossbar.conv4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "conv5",
                                           &currChar,
                                           (uint32_t*)&dataInterfaceSettings->linkSharingCfg.linkSharingAdcCrossbar.conv5);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "conv6",
                                           &currChar,
                                           (uint32_t*)&dataInterfaceSettings->linkSharingCfg.linkSharingAdcCrossbar.conv6);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "conv7",
                                           &currChar,
                                           (uint32_t*)&dataInterfaceSettings->linkSharingCfg.linkSharingAdcCrossbar.conv7);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "conv8",
                                           &currChar,
                                           (uint32_t*)&dataInterfaceSettings->linkSharingCfg.linkSharingAdcCrossbar.conv8);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "conv9",
                                           &currChar,
                                           (uint32_t*)&dataInterfaceSettings->linkSharingCfg.linkSharingAdcCrossbar.conv9);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "conv10",
                                           &currChar,
                                           (uint32_t*)&dataInterfaceSettings->linkSharingCfg.linkSharingAdcCrossbar.conv10);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "conv11",
                                           &currChar,
                                           (uint32_t*)&dataInterfaceSettings->linkSharingCfg.linkSharingAdcCrossbar.conv11);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "conv12",
                                           &currChar,
                                           (uint32_t*)&dataInterfaceSettings->linkSharingCfg.linkSharingAdcCrossbar.conv12);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "conv13",
                                           &currChar,
                                           (uint32_t*)&dataInterfaceSettings->linkSharingCfg.linkSharingAdcCrossbar.conv13);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "conv14",
                                           &currChar,
                                           (uint32_t*)&dataInterfaceSettings->linkSharingCfg.linkSharingAdcCrossbar.conv14);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                           fp,
                                           &lineStr[0],
                                           "conv15",
                                           &currChar,
                                           (uint32_t*)&dataInterfaceSettings->linkSharingCfg.linkSharingAdcCrossbar.conv15);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                  fp,
                                                  &lineStr[0],
                                                  "linkSharingAdcCrossbar",
                                                  &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                  fp,
                                                  &lineStr[0],
                                                  "linkSharingCfg",
                                                  &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    fp,
                                                    &lineStr[0],
                                                    "dataCfg",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonParseU8(device,
                                          fp,
                                          &lineStr[0],
                                          "enable",
                                          &currChar,
                                          &dataInterfaceSettings->dataCfg.enable);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          fp,
                                          &lineStr[0],
                                          "configOption1",
                                          &currChar,
                                          &dataInterfaceSettings->dataCfg.configOption1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          fp,
                                          &lineStr[0],
                                          "configOption2",
                                          &currChar,
                                          &dataInterfaceSettings->dataCfg.configOption2);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                  fp,
                                                  &lineStr[0],
                                                  "dataCfg",
                                                  &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonParseU8(device,
                                          fp,
                                          &lineStr[0],
                                          "channelSelect",
                                          &currChar,
                                          &dataInterfaceSettings->channelSelect);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          fp,
                                          &lineStr[0],
                                          "channelMode",
                                          &currChar,
                                          &dataInterfaceSettings->channelMode);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                  fp,
                                                  &lineStr[0],
                                                  "datapath",
                                                  &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_JsonParseUtilRadioctrlInit(adi_adrv9025_Device_t*        device,
                                            adi_adrv9025_RadioctrlInit_t* radioCtrlInit,
                                            FILE*                         fp)
{
    UNUSED_PARA(device);
    UNUSED_PARA(radioCtrlInit);
    UNUSED_PARA(fp);
    /* TODO */
    return 0;
}

int32_t adrv9025_JsonParseUtilInitCals(adi_adrv9025_Device_t*   device,
                                       adi_adrv9025_InitCals_t* initCals,
                                       FILE*                    fp)
{
    UNUSED_PARA(device);
    UNUSED_PARA(initCals);
    UNUSED_PARA(fp);
    /* TODO */
    return 0;
}

int32_t adrv9025_JsonParseInitGpInterruptSettings(adi_adrv9025_Device_t*              device,
                                                  adi_adrv9025_GpInterruptSettings_t* gpInterruptSettings,
                                                  FILE*                               fp)
{
    int32_t recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    int32_t currChar                              = 0;
    char    lineStr[ADRV9025_MAX_FILE_BUFFER + 1] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        fp);

    ADI_NULL_PTR_RETURN(&device->common,
                        gpInterruptSettings);

    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    fp,
                                                    &lineStr[0],
                                                    "gpInterrupts",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonParseU64(device,
                                           fp,
                                           &lineStr[0],
                                           "gpIntMaskPin0",
                                           &currChar,
                                           &gpInterruptSettings->gpIntMaskPin0);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonParseU64(device,
                                           fp,
                                           &lineStr[0],
                                           "gpIntMaskPin1",
                                           &currChar,
                                           &gpInterruptSettings->gpIntMaskPin1);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                  fp,
                                                  &lineStr[0],
                                                  "gpInterrupts",
                                                  &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}
