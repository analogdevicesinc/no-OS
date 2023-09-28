// SPDX-License-Identifier: GPL-2.0
/**
* \file adrv9025_data_interface.c
* \brief Contains private helper functions to assist the ADRV9025 data interface
*        API functions.
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_error.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_cpu.h"
#include "../../private/include/adrv9025_data_interface.h"
#include "../../private/include/adrv9025_bf_deser.h"
#include "../../private/include/adrv9025_init.h"
#include "../../private/include/adrv9025_reg_addr_macros.h"
#include "../devices/adrv9025/private/include/adrv9025_cpu.h"
#include "../devices/adrv9025/private/include/adrv9025_cpu_macros.h"

uint8_t adrv9025_SerializerLamLaneOffsetGet(uint8_t laneAtPackageBall)
{
    uint8_t laneAtDie = 0;

    static const uint8_t framerLaneLamTbl[] =
    {
        1u, /* [0] SEROUT A */
        3u, /* [1] SEROUT B */
        5u, /* [2] SEROUT C */
        7u, /* [3] SEROUT D */
        0u, /* [4] SEROUT E */
        2u, /* [5] SEROUT F */
        4u, /* [6] SEROUT G */
        6u  /* [7] SEROUT H */
    };

    if (laneAtPackageBall <= 7u)
    {
        laneAtDie = framerLaneLamTbl[laneAtPackageBall];
    }
    else
    {
        laneAtDie = 7u;
    }

    return laneAtDie;
}

uint8_t adrv9025_FramerPadToBallTranslate(uint8_t pad)
{
    uint8_t       result                   = pad;
    const uint8_t framerTranslationTable[] =
    {
        0u, /*SERDOUTA*/
        1u, /*SERDOUTB*/
        2u, /*SERDOUTC*/
        3u  /*SERDOUTD*/
    };

    if (pad < 4u)
    {
        result = framerTranslationTable[pad];
    }

    return result;
}

uint8_t adrv9025_FramerPadToBallMaskTranslate(uint8_t padMask)
{
    uint8_t i      = 0u;
    uint8_t max    = 8u * sizeof(uint8_t);
    uint8_t result = 0u;

    for (i = 0u; i < max; ++i)
    {
        if (padMask & (0x1 << i))
        {
            result |= 0x1 << adrv9025_FramerPadToBallTranslate(i);
        }
    }
    return result;
}

uint8_t adrv9025_DeserializerLamLaneOffsetGet(uint8_t laneAtPackageBall)
{
    uint8_t laneAtDie = 0;

    static const uint8_t deframerLaneLamTbl[] =
    {
        7u, /* [0] SEROUT A */
        5u, /* [1] SEROUT B */
        3u, /* [2] SEROUT C */
        1u, /* [3] SEROUT D */
        6u, /* [4] SEROUT E */
        4u, /* [5] SEROUT F */
        2u, /* [6] SEROUT G */
        0u  /* [7] SEROUT H */
    };

    if (laneAtPackageBall <= 7u)
    {
        laneAtDie = deframerLaneLamTbl[laneAtPackageBall];
    }
    else
    {
        laneAtDie = 7u;
    }

    return laneAtDie;
}

uint8_t adrv9025_DeframerLamLaneOffsetGet(uint8_t laneAtDie)
{
    uint8_t laneAtPackageBall = 0;

    static const uint8_t deframerInvLaneLamTbl[] =
    {
        7u, /* [0] pad_ch0 */
        3u, /* [1] pad_ch1 */
        6u, /* [2] pad_ch2 */
        2u, /* [3] pad_ch3 */
        5u, /* [4] pad_ch4 */
        1u, /* [5] pad_ch5 */
        4u, /* [6] pad_ch6 */
        0u  /* [7] pad_ch7 */
    };

    /* Return value from table if index is valid, 31(invalid) corresponds to lane select 7*/
    if (laneAtDie <= 7u)
    {
        laneAtPackageBall = deframerInvLaneLamTbl[laneAtDie];
    }
    else if (laneAtDie == 31u)
    {
        laneAtPackageBall = 7u;
    }
    else
    {
        laneAtPackageBall = laneAtDie;
    }

    return laneAtPackageBall;
}

uint8_t adrv9025_DeframerLaneTranslateBitmask(uint8_t inBitMask)
{
    uint8_t i          = 0u;
    uint8_t outBitMask = 0u;

    static const uint8_t deframerInvLaneLamTbl[] =
    {
        7u, /* [0] pad_ch0 */
        3u, /* [1] pad_ch1 */
        6u, /* [2] pad_ch2 */
        2u, /* [3] pad_ch3 */
        5u, /* [4] pad_ch4 */
        1u, /* [5] pad_ch5 */
        4u, /* [6] pad_ch6 */
        0u  /* [7] pad_ch7 */
    };

    for (i = 0u; i < 8u; i++)
    {
        if (inBitMask & (1u << i))
        {
            outBitMask |= (1u << deframerInvLaneLamTbl[i]);
        }
    }

    return outBitMask;
}

int32_t adrv9025_CbusRegisterRead(adi_adrv9025_Device_t* device,
                                  uint8_t                addr,
                                  uint8_t                rxLaneSel,
                                  uint8_t*               data)
{
    int32_t retVal = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common,
                        data);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if ((rxLaneSel & (rxLaneSel - 1)) > 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         framerSel,
                         "Only one lane is allowed at a time for readback");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    retVal = adrv9025_DeserCbusAddrDesRcBfSet(device,
                                              ADRV9025_BF_JESD_DESERIALIZER,
                                              addr);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to write cbus addr");
    ADI_ERROR_RETURN(device->common.error.newAction);

    retVal = adrv9025_DeserCbusRenDesRcChBfSet(device,
                                               ADRV9025_BF_JESD_DESERIALIZER,
                                               rxLaneSel);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to write read enable");
    ADI_ERROR_RETURN(device->common.error.newAction);

    retVal = adrv9025_DeserCbusRdataDesRsBfGet(device,
                                               ADRV9025_BF_JESD_DESERIALIZER,
                                               data);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to read cbus rdata");
    ADI_ERROR_RETURN(device->common.error.newAction);

    retVal = adrv9025_DeserCbusRenDesRcChBfSet(device,
                                               ADRV9025_BF_JESD_DESERIALIZER,
                                               0);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to clear read enable");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adrv9025_CbusRegisterWrite(adi_adrv9025_Device_t* device,
                                   uint8_t                addr,
                                   uint8_t                rxLaneSel,
                                   uint8_t                data)
{
    int32_t retVal = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if (rxLaneSel > 0x0F)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         framerSel,
                         "Invalid lane selected");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    retVal = adrv9025_DeserCbusAddrDesRcBfSet(device,
                                              ADRV9025_BF_JESD_DESERIALIZER,
                                              addr);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to write cbus addr");
    ADI_ERROR_RETURN(device->common.error.newAction);

    retVal = adrv9025_DeserCbusWdataDesRcBfSet(device,
                                               ADRV9025_BF_JESD_DESERIALIZER,
                                               data);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to write cbus wdata");
    ADI_ERROR_RETURN(device->common.error.newAction);

    retVal = adrv9025_DeserCbusWstrobeDesRcChBfSet(device,
                                                   ADRV9025_BF_JESD_DESERIALIZER,
                                                   rxLaneSel);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to write cbus wstrobe");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Must reset strobe register on exit */
    retVal = adrv9025_DeserCbusWstrobeDesRcChBfSet(device,
                                                   ADRV9025_BF_JESD_DESERIALIZER,
                                                   0);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     retVal,
                     NULL,
                     "Failed to clear cbus wstrobe");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adrv9025_CbusRegisterSpiRead(adi_adrv9025_Device_t *device,
                                     uint8_t                  addr,
                                     uint8_t                  rxLaneSel,
                                     uint8_t*                 data)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common, data);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    ADRV9025_SPIWRITEBYTE("CBUS_CONTROL_ADDRESS", ADRV9025_DES_CBUS_CONTROL_ADDRESS, addr);    /* Write Cbus control address */
    ADRV9025_SPIWRITEBYTE("CBUS_CONTROL_CHANNEL", ADRV9025_DES_CBUS_CONTROL_CHANNEL, 0x00);    /* select channel to read back data from cbus_rdata_ser<7:0>. Active high. <0> = ch0, <1>=ch1. */
    ADRV9025_SPIWRITEBYTE("CBUS_CONTROL_LANE", ADRV9025_DES_CBUS_CONTROL_CHANNEL, rxLaneSel);  /* Write down which lane is being selected. */

    ADRV9025_SPIREADBYTE("CBUS_READBACK_VALUE", ADRV9025_DES_CBUS_DATA_READBACK, data);        /* Readback value from bus */

    ADRV9025_SPIWRITEBYTE("CBUS_PLL_SELECT_ADDRESS", ADRV9025_DES_CBUS_PLL_SELECT, 0x00);      /* Select PLL */
    ADRV9025_SPIWRITEBYTE("CBUS_CONTROL_ADDRESS", ADRV9025_DES_CBUS_CONTROL_CHANNEL, 0x00);

    return (device->common.error.newAction);
}

int32_t adrv9025_CbusRegisterSpiWrite(adi_adrv9025_Device_t *device,
                                      uint8_t                addr,
                                      uint8_t                rxLaneSel,
                                      uint8_t                data)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    ADRV9025_SPIWRITEBYTE("CBUS_CONTROL_ADDRESS", ADRV9025_DES_CBUS_CONTROL_ADDRESS, addr);   /* Write Cbus control address */
    ADRV9025_SPIWRITEBYTE("CBUS_CONTROL_VALUE", ADRV9025_DES_CBUS_DATA_WRITE, data);         /* Write Cbus data */
    ADRV9025_SPIWRITEBYTE("CBUS_CONTROL_LANE", ADRV9025_DES_CBUS_PHY_WSTROBE, rxLaneSel);    /* Strobe the cbus lane */
    ADRV9025_SPIWRITEBYTE("CBUS_CONTROL_LANE", ADRV9025_DES_CBUS_PHY_WSTROBE, 0x00);         /* Strobe the cbus lane */
    return (device->common.error.newAction);
}

int32_t adrv9025_SpoSpiSet(adi_adrv9025_Device_t *device,
                           uint8_t                rxLaneSel,
                           uint8_t                data)
{
    int32_t retVal = ADI_COMMON_ACT_NO_ACTION;

    static const uint8_t LF_SPO = 0x0D;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    /* write to sel_lf_spo_des_rc and CLEAR the latch bit */
    retVal = adrv9025_CbusRegisterSpiWrite(device,
                                           LF_SPO,
                                           rxLaneSel,
                                           (data));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     retVal,
                     NULL,
                     "Failed to write LF_SPO");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* write to sel_lf_spo_des_rc and SET the latch bit */
    retVal = adrv9025_CbusRegisterSpiWrite(device,
                                           LF_SPO,
                                           rxLaneSel,
                                           (data | 0x80));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     retVal,
                     NULL,
                     "Failed to write LF_SPO");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* write to sel_lf_spo_des_rc and CLEAR the latch bit */
    retVal = adrv9025_CbusRegisterSpiWrite(device,
                                           LF_SPO,
                                           rxLaneSel,
                                           (data));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     retVal,
                     NULL,
                     "Failed to write LF_SPO");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adrv9025_SpoSet(adi_adrv9025_Device_t* device,
                        uint8_t                rxLaneSel,
                        uint8_t                data)
{
    int32_t retVal = ADI_COMMON_ACT_NO_ACTION;

    static const uint8_t LF_SPO = 0x0D;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* write to sel_lf_spo_des_rc and CLEAR the latch bit */
    retVal = adrv9025_CbusRegisterWrite(device,
                                        LF_SPO,
                                        rxLaneSel,
                                        (data & 0x7F));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     retVal,
                     NULL,
                     "Failed to write LF_SPO");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* write to sel_lf_spo_des_rc and SET the latch bit */
    retVal = adrv9025_CbusRegisterWrite(device,
                                        LF_SPO,
                                        rxLaneSel,
                                        (data | 0x80));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     retVal,
                     NULL,
                     "Failed to write LF_SPO");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* write to sel_lf_spo_des_rc and CLEAR the latch bit */
    retVal = adrv9025_CbusRegisterWrite(device,
                                        LF_SPO,
                                        rxLaneSel,
                                        (data & 0x7F));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     retVal,
                     NULL,
                     "Failed to write LF_SPO");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adrv9025_DeserializerLaneEnable(adi_adrv9025_Device_t* device,
                                        uint8_t                laneMask,
                                        uint8_t                enable)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t lanePdVal      = 0xFF; /* Initialize to ones because we'll clear the bit only when enable is true */
    uint8_t i              = 0;
    uint8_t currentLane    = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    for (i = 0; i < 4; i++)
    {
        currentLane = 1 << i;
        if ((laneMask & currentLane) > 0)
        {
            if (enable > 0)
            {
                lanePdVal &= ~(1 << i);
            }
        }
    }

    /* Note that we are not using a bitfield, since the destination fields are not channelized. */
    ADRV9025_SPIFIELDWRITE(device,
                           ADRV9025_ADDR_SYSREF_DES_PHY_PD,
                           lanePdVal,
                           laneMask,
                           0,
                           "ADRV9025_ADDR_SYSREF_DES_PHY_PD");

    return recoveryAction;
}

int32_t adrv9025_VerticalIsiGet(adi_adrv9025_Device_t            *device,
                                uint8_t                          lane,
                                uint8_t                          *data,
                                adi_adrv9025_VerticalISIConfig_t config)

{
    int32_t  recoverAction               = ADI_COMMON_ACT_NO_ACTION;
    uint32_t error_threshold             = config.error_threshold;
    uint8_t  start_eyemon_step           = config.start_eyemon_step;
    uint8_t  num_flashes                 = config.num_flashes;
    uint8_t  c0_notc1                    = config.c0_notc1;
    uint8_t  search_up_down              = config.search_up_down;
    uint8_t  extData[4]                  = { 0 };
    uint8_t  armData[16]                 = { 0 };
    uint8_t  cmdStatusByte               = 0;
    uint8_t  readBackData[24]            = { 0 };

    static const uint8_t  ARM_ERR_CODE   = 0x0E;
    static const uint32_t N              = 0x01F4;

     ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_DEVICE_PTR_RETURN(data);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    /* Vertical ISI ARM Command configuration structure */
    armData[0]  = (uint8_t)(N & 0x000000FF);
    armData[1]  = (uint8_t)((N & 0x0000FF00) >> 8);
    armData[2]  = 0x00;
    armData[3]  = 0x00;
    armData[4]  = (uint8_t)(error_threshold & 0x000000FF);
    armData[5]  = (uint8_t)((error_threshold & 0x0000FF00) >> 8);
    armData[6]  = (uint8_t)((error_threshold & 0x00FF0000) >> 16);
    armData[7]  = (uint8_t)((error_threshold & 0xFF000000) >> 24);
    armData[8]  = start_eyemon_step;
    armData[9]  = num_flashes;
    armData[10] = c0_notc1;
    armData[11] = search_up_down;
    armData[12] = 0x00;
    armData[13] = 0x00;
    armData[14] = 0x00;
    armData[15] = 0x00;

    /* Arm Serdes Test mode run ISI task command */
    extData[0] = ADRV9025_CPU_OBJECTID_TM_SERDES;
    extData[1] = lane;
    extData[2] = ADRV9025_CPU_OBJECTID_TM_SERDES_TASK_VERT_ISI;

    /* Enter Serdes test Mode */
    recoverAction = adrv9025_CpuSerdesTestModeEnter(device);
    if (recoverAction != ADI_COMMON_ACT_NO_ACTION)
    {
        adrv9025_CpuSerdesTestModeExit(device);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoverAction,
                         NULL,
                         "Failed to enter serdes test mode");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Send Test mode configuration data to arm */
    recoverAction = adi_adrv9025_CpuMemWrite(device,
                                             ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                                             &armData[0],
                                             16,
                                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    if (recoverAction != ADI_COMMON_ACT_NO_ACTION)
    {
        adrv9025_CpuSerdesTestModeExit(device);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoverAction,
                         NULL,
                         "Failed to write Arm Data");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoverAction = adi_adrv9025_CpuCmdWrite(device,
                                             ADI_ADRV9025_CPU_TYPE_C,
                                             (uint8_t)ADRV9025_CPU_TEST_OPCODE,
                                             &extData[0],
                                             4);
    if (recoverAction != ADI_COMMON_ACT_NO_ACTION)
    {
        adrv9025_CpuSerdesTestModeExit(device);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoverAction,
                         NULL,
                         "Failed to write ARM Command");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    recoverAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                  ADI_ADRV9025_CPU_TYPE_C,
                                                  (uint8_t)ADRV9025_CPU_TEST_OPCODE,
                                                  &cmdStatusByte,
                                                  (uint32_t)ADI_ADRV9025_TEST_MODE_TIMEOUT_US,
                                                  (uint32_t)ADI_ADRV9025_TEST_MODE_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        recoverAction = adrv9025_CpuCmdErrorHandler(device,
                                                    ADI_ADRV9025_CPU_TYPE_C,
                                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_TEST_OPCODE,
                                                                             extData[0],
                                                                             cmdStatusByte),
                                                    recoverAction,
                                                    ADI_ADRV9025_ACT_ERR_RESET_CPU);
        adrv9025_CpuSerdesTestModeExit(device);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoverAction = adi_adrv9025_CpuMemRead(device,
                                            ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                                            &readBackData[0],
                                            sizeof(readBackData),
                                            ADI_ADRV9025_CPU_MEM_AUTO_INCR);

    data[0] = readBackData[4]; /* vertisi_sum_s0f1  */
    data[1] = readBackData[5]; /* vertisi_sum_s1f1  */
    data[2] = readBackData[6]; /* vertisi_diff_s0f1 */
    data[3] = readBackData[7]; /* vertisi_diff_s1f1 */

    /* Exit Serdes Test mode */
    recoverAction = adrv9025_CpuSerdesTestModeExit(device);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoverAction,
                     NULL,
                     "Failed to exit serdes test mode");
    ADI_ERROR_RETURN(device->common.error.newAction);
    return recoverAction;
}

int32_t adrv9025_GenerateEyeDiagram_Direction(adi_adrv9025_Device_t              *device,
                                              uint8_t                            lane,
                                              adi_adrv9025_EyeDiagramResultsQR_t **results,
                                              uint8_t                            numResults,
                                              uint8_t                            direction)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    uint8_t loopFilterDLLSlewRegVal    = 0;                     /* Variable to store value of loop filter dll slew register */
    uint8_t enFlashMaskDesRCValOrig    = 0;                     /*  */
    uint8_t s0BThresholdb1Val          = 0;                     /* slice 0 threshold b1 value register */
    uint8_t sweepRange                 = 18;                    /* how far to sweep along signal phase in a given direction (1/2 a UI plus a margin either side for transition) */
    uint8_t resultPos                  = 0;
    int8_t  invertVal                  = 0;                     /* depending on direction we need to do a different calculation, this val can be used to change a value negative */
    int     phasePosition              = 0;                     /* Loop counter */
    int     phaseStart                 = sweepRange - 1 ;       /* Keep track of which position you are at in the array offsetting based on phase i.e. position 15 = phase 0 */
    int     phaseCurrent               = phasePosition;
    int     laneSelMask                = 0;

    adi_adrv9025_VerticalISIConfig_t configuration      = { 0 };
    uint8_t                          verticalisiData[4] = { 0 };

    static const uint8_t DLL_SLEW_SETTINGS_REG    = 0x08;
    static const uint8_t EN_FLASH_MASK_DES_RC_REG = 0xFA;
    static const uint8_t S0_B_THRB1_REG           = 0xCA;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_DEVICE_PTR_RETURN(results);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    if (direction == 1)
    {
        configuration.c0_notc1 = 1; /* Choose edge comparitor 0 */
        invertVal = 1;
    }
    else
    {
        resultPos = 1;
        invertVal = -1;
        configuration.c0_notc1 = 0; /* Invert edge comparitor to use comparitor 1 */
    }

    configuration.error_threshold   = 0x0000; /* Error threshold value */
    configuration.num_flashes       = 2;
    configuration.search_up_down    = 1;
    configuration.start_eyemon_step = 0x20;  /* Step size */

    laneSelMask = 1 << lane;   /* Calculate lane select mask */

    /* Read loop filter dll slew value from register */
    recoveryAction = adrv9025_CbusRegisterSpiRead(device, DLL_SLEW_SETTINGS_REG, laneSelMask, &loopFilterDLLSlewRegVal);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to read Loopfilter DLL Slew Settings Register");
    ADI_ERROR_RETURN(device->common.error.newAction);

     /* Read en_flash_mask_des_rc from DFE Control Register */
    recoveryAction = adrv9025_CbusRegisterSpiRead(device, EN_FLASH_MASK_DES_RC_REG, laneSelMask, &enFlashMaskDesRCValOrig);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to read en_flash_mask_des_rc from DFE Control Register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read s0_b_thrb1 from DFE Control Register */
    recoveryAction = adrv9025_CbusRegisterSpiRead(device, S0_B_THRB1_REG, laneSelMask, &s0BThresholdb1Val);
    s0BThresholdb1Val &= 0x3f;
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to read s0_b_thrb1 from DFE Control Register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_SpoSpiSet(device, laneSelMask, 0x00);  /* Clear and setup the SPO */
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to write static phase offset");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Enable digital counting of flash outputs toward DLL phase accumulator */
    recoveryAction = adrv9025_CbusRegisterSpiWrite(device, EN_FLASH_MASK_DES_RC_REG, laneSelMask, 0xA0 | enFlashMaskDesRCValOrig);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to write updated flash mask des rc value");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Slew SPO to negative value */
    for (phasePosition = 127; phasePosition > 127 - sweepRange; phasePosition --)
    {
        recoveryAction = adrv9025_SpoSpiSet(device, laneSelMask, phasePosition);  /* Adjust the SPO */
        ADI_ERROR_REPORT(&device->common,
                            ADI_COMMON_ERRSRC_API,
                            ADI_COMMON_ERR_API_FAIL,
                            recoveryAction,
                            NULL,
                            "Failed to write static phase offset");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Use ARM to Calculate Vertical ISI at particular phase position */
        adrv9025_VerticalIsiGet(device, lane, &verticalisiData[0], configuration);
        results[resultPos][phaseStart].phase       = phasePosition;
        results[resultPos][phaseStart].b1Threshold = s0BThresholdb1Val;
        results[resultPos][phaseStart].eyeStepUp   = (s0BThresholdb1Val * invertVal) + verticalisiData[0]; /* s0BThresholdb1Val + vertisi_sum_s0f1  (-b1 or +b1 threshold value) */
        results[resultPos][phaseStart].eyeStepDown = (s0BThresholdb1Val * invertVal) - verticalisiData[1]; /* s0BThresholdb1Val - vertisi_sum_s1f1  (-b1 or +b1 threshold value) */
        phaseStart--;                               /* decrement phaseStart array position accessor */
        phaseCurrent = phasePosition;
    }

    phaseCurrent--;

    // slew phase / spo back to starting point
    for (phasePosition = phaseCurrent; phasePosition < 128; phasePosition ++)
    {
        recoveryAction = adrv9025_SpoSpiSet(device, laneSelMask, phasePosition);  /* Adjust the SPO */
        ADI_ERROR_REPORT(&device->common,
                            ADI_COMMON_ERRSRC_API,
                            ADI_COMMON_ERR_API_FAIL,
                            recoveryAction,
                            NULL,
                            "Failed to write static phase offset");
        ADI_ERROR_RETURN(device->common.error.newAction);
        phaseStart++;                               /* increment phaseStart array position accessor */
        phaseCurrent = phasePosition;
    }

    /* Iterate through all SPO's based negative and positive directions */
    for (phasePosition = 0; phasePosition < sweepRange; phasePosition ++)
    {
        recoveryAction = adrv9025_SpoSpiSet(device, laneSelMask, phasePosition);  /* Adjust the SPO */
        ADI_ERROR_REPORT(&device->common,
                            ADI_COMMON_ERRSRC_API,
                            ADI_COMMON_ERR_API_FAIL,
                            recoveryAction,
                            NULL,
                            "Failed to write static phase offset");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Use FW Serdes Test mode to calculate eye metrics */
        adrv9025_VerticalIsiGet(device, lane, &verticalisiData[0], configuration);
        results[resultPos][phaseStart].phase       = phasePosition;
        results[resultPos][phaseStart].b1Threshold = s0BThresholdb1Val;
        results[resultPos][phaseStart].eyeStepUp   = (s0BThresholdb1Val * invertVal) + verticalisiData[0]; /* s0BThresholdb1Val + vertisi_sum_s0f1  (-b1 or +b1 threshold value) */
        results[resultPos][phaseStart].eyeStepDown = (s0BThresholdb1Val * invertVal) - verticalisiData[1]; /* s0BThresholdb1Val -  vertisi_sum_s1f1 (-b1 or +b1 threshold value) */
        phaseStart++; /* increment phaseStart array position accessor */
        phaseCurrent = phasePosition;
    }

    phaseCurrent++;

    for (phasePosition = phaseCurrent; phasePosition > 0; phasePosition --)
    {
        recoveryAction = adrv9025_SpoSpiSet(device, laneSelMask, phasePosition);  /* Adjust the SPO */
        ADI_ERROR_REPORT(&device->common,
                            ADI_COMMON_ERRSRC_API,
                            ADI_COMMON_ERR_API_FAIL,
                            recoveryAction,
                            NULL,
                            "Failed to write static phase offset");
        ADI_ERROR_RETURN(device->common.error.newAction);

        phaseStart--; /* decrement phaseStart array position accessor */
        phaseCurrent = phasePosition;
    }

    recoveryAction = adrv9025_SpoSpiSet(device, laneSelMask, 0x00);  /* Clear and setup the SPO */
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to write static phase offset");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* restore original system state of DLL slew settings register */
    recoveryAction = adrv9025_CbusRegisterSpiWrite(device, DLL_SLEW_SETTINGS_REG, laneSelMask, loopFilterDLLSlewRegVal);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to write original Loopfilter DLL Slew Settings Register value");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* restore original system state of flash mask des rc */
    recoveryAction = adrv9025_CbusRegisterSpiWrite(device, EN_FLASH_MASK_DES_RC_REG, laneSelMask, enFlashMaskDesRCValOrig);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to write original flash mask des rc value");
    ADI_ERROR_RETURN(device->common.error.newAction);
    return recoveryAction;
}