/**
* \file
* \brief Contains SSI calibration related function implementation defined in
* adi_adrv9001_ssi.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2019 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv9001_ssi.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_error.h"
#include "adi_adrv9001_radio.h"
#include "adi_adrv9001_spi.h"
#include "adi_adrv9001_user.h"

#include "adrv9001_arm.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_bf.h"
#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"
#include "object_ids.h"

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Ssi_Rx_TestMode_Configure_Validate(adi_adrv9001_Device_t *device,
                                                                              adi_common_ChannelNumber_e channel,
                                                                              adi_adrv9001_SsiType_e ssiType,
                                                                              adi_adrv9001_SsiDataFormat_e dataFormat,
                                                                              adi_adrv9001_RxSsiTestModeCfg_t *ssiTestModeConfig)
{
    ADI_NULL_PTR_RETURN(&device->common, ssiTestModeConfig);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(device, ssiType, ADI_ADRV9001_SSI_TYPE_CMOS, ADI_ADRV9001_SSI_TYPE_LVDS);
    ADI_RANGE_CHECK(device, dataFormat, ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA, ADI_ADRV9001_SSI_FORMAT_22_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE_8_BIT_GAIN_INDEX);

    if (ADI_ADRV9001_SSI_TYPE_CMOS == ssiType)
    {
        ADI_RANGE_CHECK(device, ssiTestModeConfig->testData, ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL, ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_16_BIT);
    }
    else
    {
        ADI_RANGE_CHECK(device, ssiTestModeConfig->testData, ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL, ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS7);
        if (ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_NIBBLE == ssiTestModeConfig->testData)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                ssiTestModeConfig->testData,
                "Nibble ramp pattern is not supported in LVDS");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Ssi_Rx_TestMode_Configure(adi_adrv9001_Device_t *device,
                                               adi_common_ChannelNumber_e channel,
                                               adi_adrv9001_SsiType_e ssiType,
                                               adi_adrv9001_SsiDataFormat_e dataFormat,
                                               adi_adrv9001_RxSsiTestModeCfg_t *ssiTestModeConfig)
{
    int8_t i = 0;
    adrv9001_BfNvsRegmapRx_e instance = ADRV9001_BF_RX1_CORE;
    uint8_t channelIdx = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Ssi_Rx_TestMode_Configure_Validate, device, channel, ssiType, dataFormat, ssiTestModeConfig);

    adi_common_channel_to_index(channel, &channelIdx);
    instance = nvsRegmapRxInstances[channelIdx];

    if (ADI_ADRV9001_SSI_TYPE_CMOS == ssiType)
    {
        if (ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL == ssiTestModeConfig->testData)
        {
            ADI_EXPECT(adrv9001_NvsRegmapRx_CssiRxDebugMode_Set, device, instance, 0x0);
        }
        else
        {
            ADI_EXPECT(adrv9001_NvsRegmapRx_CssiRxDebugMode_Set, device, instance, 0x1);
            if (ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_NIBBLE == ssiTestModeConfig->testData)
            {
                ADI_EXPECT(adrv9001_NvsRegmapRx_CssiRxDebugLoad4bitsMode_Set, device, instance, 0x1);
                ADI_EXPECT(adrv9001_NvsRegmapRx_CssiRxDebugStartRamp_Set, device, instance, 0x1);
            }
            else if (ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_16_BIT == ssiTestModeConfig->testData)
            {
                ADI_EXPECT(adrv9001_NvsRegmapRx_CssiRxDebugLoad4bitsMode_Set, device, instance, 0x0);
                ADI_EXPECT(adrv9001_NvsRegmapRx_CssiRxDebugStartRamp_Set, device, instance, 0x1);
            }
            else /* Fixed pattern */
            {
                ADI_EXPECT(adrv9001_NvsRegmapRx_CssiRxDebugStartRamp_Set, device, instance, 0x0);

                for (i = 0; i < 2; i++)
                {
                    ADI_EXPECT(adrv9001_NvsRegmapRx_CssiRxDebugQSel_Set, device, instance, i);   // 0: I_data; 1: Q_data
                    ADI_EXPECT(adrv9001_NvsRegmapRx_CssiRxDebugNibbleSel_Set, device, instance, 0x3);
                    ADI_EXPECT(adrv9001_NvsRegmapRx_CssiRxDebugLoadValue_Set, device, instance, (uint8_t)(ssiTestModeConfig->fixedDataPatternToTransmit & 0xF));
                    ADI_EXPECT(adrv9001_NvsRegmapRx_CssiRxDebugLoad_Set, device, instance, 0x1);
	                adi_common_hal_Wait_us(&device->common, 1000);
                    ADI_EXPECT(adrv9001_NvsRegmapRx_CssiRxDebugLoad_Set, device, instance, 0x0);
                }
            }
        }
    }
    else /* LVDS */
    {
        ADI_EXPECT(adrv9001_NvsRegmapRx_LssiRxDebugPrbs7Enable_Set, device, instance, 0x0);
        ADI_EXPECT(adrv9001_NvsRegmapRx_LssiRxDebugPrbs15Enable_Set, device, instance, 0x0);
        ADI_EXPECT(adrv9001_NvsRegmapRx_LssiRxDebugStartRamp_Set, device, instance, 0x0);
        if (ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL == ssiTestModeConfig->testData)
        {
            ADI_EXPECT(adrv9001_NvsRegmapRx_LssiRxDebugMode_Set, device, instance, 0x0);
        }
        else
        {
            ADI_EXPECT(adrv9001_NvsRegmapRx_LssiRxDebugMode_Set, device, instance, 0x1);
            if (ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_16_BIT == ssiTestModeConfig->testData)
            {
                ADI_EXPECT(adrv9001_NvsRegmapRx_LssiRxDebugStartRamp_Set, device, instance, 0x1);
            }
            else if (ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS7 == ssiTestModeConfig->testData)
            {
                ADI_EXPECT(adrv9001_NvsRegmapRx_LssiRxDebugPrbs7Restart_Set, device, instance, 0x1);
                ADI_EXPECT(adrv9001_NvsRegmapRx_LssiRxDebugPrbs7Enable_Set, device, instance, 0x1);
            }
            else if (ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS15 == ssiTestModeConfig->testData)
            {
                ADI_EXPECT(adrv9001_NvsRegmapRx_LssiRxDebugPrbs15Restart_Set, device, instance, 0x1);
                ADI_EXPECT(adrv9001_NvsRegmapRx_LssiRxDebugPrbs15Enable_Set, device, instance, 0x1);
            }
            else /* Fixed Pattern */
            {
                ADI_EXPECT(adrv9001_NvsRegmapRx_LssiRxDebugLoadValue_Set, device, instance, (uint16_t)(ssiTestModeConfig->fixedDataPatternToTransmit & 0xFFFF));
                ADI_EXPECT(adrv9001_NvsRegmapRx_LssiRxDebugLoad_Set, device, instance, 0x1);
	            adi_common_hal_Wait_us(&device->common, 1000);
                ADI_EXPECT(adrv9001_NvsRegmapRx_LssiRxDebugLoad_Set, device, instance, 0x0);
            }
        }
    }

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Ssi_Tx_TestMode_Configure_Validate(adi_adrv9001_Device_t *device,
                                                                              adi_common_ChannelNumber_e channel,
                                                                              adi_adrv9001_SsiType_e ssiType,
                                                                              adi_adrv9001_SsiDataFormat_e dataFormat,
                                                                              adi_adrv9001_TxSsiTestModeCfg_t *ssiTestModeConfig)
{
    ADI_NULL_PTR_RETURN(&device->common, ssiTestModeConfig);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(device, ssiType, ADI_ADRV9001_SSI_TYPE_CMOS, ADI_ADRV9001_SSI_TYPE_LVDS);
    ADI_RANGE_CHECK(device, dataFormat, ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA, ADI_ADRV9001_SSI_FORMAT_22_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE_8_BIT_GAIN_INDEX);

    if (ADI_ADRV9001_SSI_TYPE_CMOS == ssiType)
    {
        ADI_RANGE_CHECK(device, ssiTestModeConfig->testData, ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL, ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_16_BIT);
    }
    else
    {
        ADI_RANGE_CHECK(device, ssiTestModeConfig->testData, ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL, ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS7);
        if (ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_NIBBLE == ssiTestModeConfig->testData)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                dataFormat,
                "Nibble ramp pattern is not supported in LVDS");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv9001_Ssi_Tx_CssiClearErrorFlags(adi_adrv9001_Device_t *device,
                                                       adrv9001_BfNvsRegmapTx_e baseAddress)
{
    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugStartRamp_Set, device, baseAddress, 0x0);
    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearRampShiftError_Set, device, baseAddress, 0x0);
    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearRampShiftError_Set, device, baseAddress, 0x1);
    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearRampShiftError_Set, device, baseAddress, 0x0);
    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxFifoClear_Set, device, baseAddress, 0x0);
    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxFifoClear_Set, device, baseAddress, 0x1);
    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxFifoClear_Set, device, baseAddress, 0x0);
    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, baseAddress, 0x0);
    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, baseAddress, 0x1);
    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, baseAddress, 0x0);
	ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, baseAddress, 0x0);
	ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, baseAddress, 0x1);
	ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, baseAddress, 0x0);
    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugStartRamp_Set, device, baseAddress, 0x1);
    ADI_API_RETURN(device);
}
    
int32_t adi_adrv9001_Ssi_Tx_TestMode_Configure(adi_adrv9001_Device_t *device,
                                               adi_common_ChannelNumber_e channel,
                                               adi_adrv9001_SsiType_e ssiType,
                                               adi_adrv9001_SsiDataFormat_e dataFormat,
                                               adi_adrv9001_TxSsiTestModeCfg_t *ssiTestModeConfig)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    adrv9001_BfNvsRegmapTx_e baseAddress = ADRV9001_BF_TX1_CORE;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Ssi_Tx_TestMode_Configure_Validate, device, channel, ssiType, dataFormat, ssiTestModeConfig);

    if (channel == ADI_CHANNEL_2)
    {
        baseAddress = ADRV9001_BF_TX2_CORE;
    }

    if (ADI_ADRV9001_SSI_TYPE_CMOS == ssiType)
    {
        if (ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL == ssiTestModeConfig->testData)
        {
            ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugMode_Set, device, baseAddress, 0x0);
        }
        else
        {
            ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugMode_Set, device, baseAddress, 0x1);
            /* Nothing to be configured for fixed pattern */
            if (ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_NIBBLE == ssiTestModeConfig->testData)
            {
                ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugLoad4bitsMode_Set, device, baseAddress, 0x1);
                ADI_EXPECT(adi_adrv9001_Ssi_Tx_CssiClearErrorFlags, device, baseAddress);
            }
            else if (ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_16_BIT == ssiTestModeConfig->testData)
            {
                ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugLoad4bitsMode_Set, device, baseAddress, 0x0);
                ADI_EXPECT(adi_adrv9001_Ssi_Tx_CssiClearErrorFlags, device, baseAddress);
            }
        }
    }
    else /* LVDS */
    {
        ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugStartRamp_Set, device, baseAddress, 0x0);
        ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugPrbs7Enable_Set, device, baseAddress, 0x0);
        ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugPrbs15Enable_Set, device, baseAddress, 0x0);
        ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugStartErrorCheck_Set, device, baseAddress, 0x0);
        if (ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL == ssiTestModeConfig->testData)
        {
            ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugMode_Set, device, baseAddress, 0x0);
        }
        else
        {
            ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugMode_Set, device, baseAddress, 0x1);
	        ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugAfterFifoErrorClear_Set, device, baseAddress, 0x0);
            ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugAfterFifoErrorClear_Set, device, baseAddress, 0x1);
            ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugAfterFifoErrorClear_Set, device, baseAddress, 0x0);
	        ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxClearStrobeAlignError_Set, device, baseAddress, 0x0);
	        ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxClearStrobeAlignError_Set, device, baseAddress, 0x1);
	        ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxClearStrobeAlignError_Set, device, baseAddress, 0x0);
	        ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxClearEarlyStrobeDetectedFlag_Set, device, baseAddress, 0x0);
	        ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxClearEarlyStrobeDetectedFlag_Set, device, baseAddress, 0x1);
	        ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxClearEarlyStrobeDetectedFlag_Set, device, baseAddress, 0x0);
	        ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxFifoClear_Set, device, baseAddress, 0x0); 
	        ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxFifoClear_Set, device, baseAddress, 0x1);
            ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxFifoClear_Set, device, baseAddress, 0x0);
            if (ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_16_BIT == ssiTestModeConfig->testData)
            {
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugStartRamp_Set, device, baseAddress, 0x1);

                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugStartErrorCheck_Set, device, baseAddress, 0x1);

                recoveryAction = adi_common_hal_Wait_us(&device->common, 1000);

                ADI_ERROR_REPORT(&device->common,
                                 ADI_ADRV9001_SRC_ARMCMD,
                                 recoveryAction,
                                 ADI_COMMON_ACT_ERR_CHECK_TIMER,
                                 device,
                                 "Timer not working in adi_adrv9001_Ssi_Tx_TestMode_Configure()");
                ADI_ERROR_RETURN(device->common.error.newAction);

                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugErrorCounterRead_Set, device, baseAddress, 0x1);
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugErrorCounterRead_Set, device, baseAddress, 0x0);
            }
            else if (ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS7 == ssiTestModeConfig->testData)
            {
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugPrbs7Enable_Set, device, baseAddress, 0x1);
	            ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugPrbs7ErrorClear_Set, device, baseAddress, 0x0);
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugPrbs7ErrorClear_Set, device, baseAddress, 0x1);
	            ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugPrbs7ErrorClear_Set, device, baseAddress, 0x0);
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugPrbs7Restart_Set, device, baseAddress, 0x1);
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugStartErrorCheck_Set, device, baseAddress, 0x1);
                recoveryAction = adi_common_hal_Wait_us(&device->common, 1000);

                ADI_ERROR_REPORT(&device->common,
                                 ADI_ADRV9001_SRC_ARMCMD,
                                 recoveryAction,
                                 ADI_COMMON_ACT_ERR_CHECK_TIMER,
                                 device,
                                 "Timer not working in adi_adrv9001_Ssi_Tx_TestMode_Configure()");
                ADI_ERROR_RETURN(device->common.error.newAction);

                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugErrorCounterRead_Set, device, baseAddress, 0x1);
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugErrorCounterRead_Set, device, baseAddress, 0x0);
            }
            else if (ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS15 == ssiTestModeConfig->testData)
            {
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugPrbs15Enable_Set, device, baseAddress, 0x1);
	            ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugAfterFifoErrorClear_Set, device, baseAddress, 0x0);
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugAfterFifoErrorClear_Set, device, baseAddress, 0x1);
	            ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugAfterFifoErrorClear_Set, device, baseAddress, 0x0);
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugPrbs15Restart_Set, device, baseAddress, 0x1);
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugStartErrorCheck_Set, device, baseAddress, 0x1);
                recoveryAction = adi_common_hal_Wait_us(&device->common, 1000);

                ADI_ERROR_REPORT(&device->common,
                                 ADI_ADRV9001_SRC_ARMCMD,
                                 recoveryAction,
                                 ADI_COMMON_ACT_ERR_CHECK_TIMER,
                                 device,
                                 "Timer not working in adi_adrv9001_Ssi_Tx_TestMode_Configure()");
                ADI_ERROR_RETURN(device->common.error.newAction);

                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugErrorCounterRead_Set, device, baseAddress, 0x1);
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugErrorCounterRead_Set, device, baseAddress, 0x0);
            }
            /* Nothing to be configured for fixed pattern */
        }
    }

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Ssi_Tx_TestMode_Status_Inspect_Validate(adi_adrv9001_Device_t *device,
                                                                                   adi_common_ChannelNumber_e channel,
                                                                                   adi_adrv9001_SsiType_e ssiType,
                                                                                   adi_adrv9001_SsiDataFormat_e dataFormat,
                                                                                   adi_adrv9001_TxSsiTestModeCfg_t *ssiTestModeConfig,
                                                                                   adi_adrv9001_TxSsiTestModeStatus_t *ssiTestModeStatus)
{
    ADI_NULL_PTR_RETURN(&device->common, ssiTestModeConfig);
    ADI_NULL_PTR_RETURN(&device->common, ssiTestModeStatus);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(device, ssiType, ADI_ADRV9001_SSI_TYPE_CMOS, ADI_ADRV9001_SSI_TYPE_LVDS);

    if (ADI_ADRV9001_SSI_TYPE_CMOS == ssiType)
    {
        ADI_RANGE_CHECK(device, ssiTestModeConfig->testData, ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL, ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_16_BIT);
        ADI_RANGE_CHECK(device, dataFormat, ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA, ADI_ADRV9001_SSI_FORMAT_22_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE_8_BIT_GAIN_INDEX);
        if (ADI_ADRV9001_SSI_FORMAT_12_BIT_I_Q_DATA == dataFormat)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                dataFormat,
                "Invalid SSI data format for the selected SSI calibration test type");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    else
    {
        ADI_RANGE_CHECK(device, ssiTestModeConfig->testData, ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL, ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS7);
        if (ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_NIBBLE == ssiTestModeConfig->testData)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                ssiTestModeConfig->testData,
                "Nibble ramp pattern is not supported in LVDS");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        ADI_RANGE_CHECK(device, dataFormat, ADI_ADRV9001_SSI_FORMAT_12_BIT_I_Q_DATA, ADI_ADRV9001_SSI_FORMAT_22_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE_8_BIT_GAIN_INDEX);
    }

    if ((ADI_ADRV9001_SSI_TESTMODE_DATA_FIXED_PATTERN != ssiTestModeConfig->testData) &&
        (dataFormat != ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA) &&
        (dataFormat != ADI_ADRV9001_SSI_FORMAT_15_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE) &&
        (dataFormat != ADI_ADRV9001_SSI_FORMAT_22_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE_8_BIT_GAIN_INDEX))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            dataFormat,
            "Invalid SSI data format for the selected SSI calibration test type");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Ssi_Tx_TestMode_Status_Inspect(adi_adrv9001_Device_t *device,
                                                    adi_common_ChannelNumber_e channel,
                                                    adi_adrv9001_SsiType_e ssiType,
                                                    adi_adrv9001_SsiDataFormat_e dataFormat,
                                                    adi_adrv9001_TxSsiTestModeCfg_t *ssiTestModeConfig,
                                                    adi_adrv9001_TxSsiTestModeStatus_t *ssiTestModeStatus)
{
    int8_t i = 0;
    int8_t nibSel = 0;
    uint16_t dataRead = 0;
    uint16_t iqValue[2] = { 0 };
    int32_t halError = 0;
    uint32_t waitInterval_us = 0;
    uint32_t numEventChecks = 1;
    uint32_t eventCheck = 0;
    uint8_t captureComplete = 0;
    uint32_t timeout_us = ADI_ADRV9001_SSI_DEBUG_TIMEOUT_US;
    adrv9001_BfNvsRegmapTx_e baseAddress = ADRV9001_BF_TX1_CORE;
	uint8_t ddrSet = 0;

    waitInterval_us = (ADI_ADRV9001_SSI_DEBUG_INTERVAL_US > timeout_us) ?
    timeout_us : ADI_ADRV9001_SSI_DEBUG_INTERVAL_US;
    numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

    ADI_PERFORM_VALIDATION(adi_adrv9001_Ssi_Tx_TestMode_Status_Inspect_Validate, device, channel, ssiType, dataFormat, ssiTestModeConfig, ssiTestModeStatus);

    if (channel == ADI_CHANNEL_2)
    {
        baseAddress = ADRV9001_BF_TX2_CORE;
    }

    if (ADI_ADRV9001_SSI_TYPE_CMOS == ssiType)
    {
        if (ADI_ADRV9001_SSI_TESTMODE_DATA_FIXED_PATTERN == ssiTestModeConfig->testData)
        {
            for(nibSel = 0 ; nibSel < 4 ; nibSel++)
            {
                ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugMode_Set, device, baseAddress, 0x1);
                ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugStartRamp_Set, device, baseAddress, 0x0);
                ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugQSel_Set, device, baseAddress, 0);   // 0: I_data; 1: Q_data
                ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugNibbleSel_Set, device, baseAddress, nibSel);
	            ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, baseAddress, 0x0);
	            ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, baseAddress, 0x1);
	            ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, baseAddress, 0x0);
	            ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, baseAddress, 0x0);
	            ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, baseAddress, 0x1);
	            ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, baseAddress, 0x0);
                ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugStartCapture_Set, device, baseAddress, 0x1);
                /* Wait for capture to complete */
                for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
                {
                    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugStartCapture_Get, device, baseAddress, &captureComplete);
                    if ((captureComplete == 1) &&
                        (eventCheck < numEventChecks))
                    {
                        /* Wait for capture to complete */
                        halError = adi_common_hal_Wait_us(&device->common, waitInterval_us);
                        ADI_ERROR_REPORT(&device->common,
                            ADI_COMMON_ERRSRC_ADI_HAL,
                            halError,
                            ADI_COMMON_ACT_ERR_CHECK_TIMER,
                            device,
                            "Timer not working");
                        ADI_ERROR_RETURN(device->common.error.newAction);
                    }
                    else
                    {
                        break;
                    }
                }
                /* if capture did not complete within the timeout period */
                if (captureComplete == 1)
                {
                    ADI_ERROR_REPORT(&device->common,
                        ADI_ADRV9001_SRC_ARMCMD,
                        ADI_COMMON_ERR_API_FAIL,
                        ADI_ADRV9001_ACT_ERR_RESET_ARM,
                        device,
                        "CssiTxDebugCapture failed due to time out ");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
                ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugCapturedData_Get, device, baseAddress, (uint8_t *)(&dataRead));
                iqValue[0] |= ((dataRead & 0xF) << 4 * nibSel);
                dataRead = 0;
            }
            if ((dataFormat == ADI_ADRV9001_SSI_FORMAT_12_BIT_I_Q_DATA) || 
                (dataFormat == ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA) ||
                (dataFormat == ADI_ADRV9001_SSI_FORMAT_15_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE) ||
                (dataFormat == ADI_ADRV9001_SSI_FORMAT_22_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE_8_BIT_GAIN_INDEX))
            {
                for (nibSel = 0; nibSel < 4; nibSel++)
                {
                    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugMode_Set, device, baseAddress, 0x1);
                    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugStartRamp_Set, device, baseAddress, 0x0);
                    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugQSel_Set, device, baseAddress, 1);     // 0: I_data; 1: Q_data
                    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugNibbleSel_Set, device, baseAddress, nibSel);
	                ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, baseAddress, 0x0);
                    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, baseAddress, 0x1);
	                ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, baseAddress, 0x0);
	                ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, baseAddress, 0x0);
	                ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, baseAddress, 0x1);
	                ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, baseAddress, 0x0);
                    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugStartCapture_Set, device, baseAddress, 0x1);
                    /* Wait for capture to complete */
                    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
                    {
                        ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugStartCapture_Get, device, baseAddress, &captureComplete);
                        if ((captureComplete == 1) &&
                            (eventCheck < numEventChecks))
                        {
                            /* Wait for capture to complete */
                            halError = adi_common_hal_Wait_us(&device->common, waitInterval_us);
                            ADI_ERROR_REPORT(&device->common,
                                ADI_COMMON_ERRSRC_ADI_HAL,
                                halError,
                                ADI_COMMON_ACT_ERR_CHECK_TIMER,
                                device,
                                "Timer not working");
                            ADI_ERROR_RETURN(device->common.error.newAction);
                        }
                        else
                        {
                            break;
                        }
                    }
                    /* if capture did not complete within the timeout period */
                    if (captureComplete == 1)
                    {
                        ADI_ERROR_REPORT(&device->common,
                            ADI_ADRV9001_SRC_ARMCMD,
                            ADI_COMMON_ERR_API_FAIL,
                            ADI_ADRV9001_ACT_ERR_RESET_ARM,
                            device,
                            "CssiTxDebugCapture failed due to time out ");
                        ADI_ERROR_RETURN(device->common.error.newAction);
                    }
                    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugCapturedData_Get, device, baseAddress, (uint8_t *)(&dataRead));
                    iqValue[1] |= ((dataRead & 0xF) << 4 * nibSel);
                    dataRead = 0;
                }
            }
            ssiTestModeStatus->dataError = 0;
            switch (dataFormat)
            {
            case ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA:
                /* Only 2 LSB */
                if (iqValue[0] != ((ssiTestModeConfig->fixedDataPatternToCheck >> 16) & 0x0003))
                {
                    ssiTestModeStatus->dataError = 1;
                }
                break;
            case ADI_ADRV9001_SSI_FORMAT_8_BIT_SYMBOL_DATA:
                /* Only 8 LSB */
                if (iqValue[0] != ((ssiTestModeConfig->fixedDataPatternToCheck >> 16) & 0x00FF))
                {
                    ssiTestModeStatus->dataError = 1;
                }
                break;
            case ADI_ADRV9001_SSI_FORMAT_16_BIT_SYMBOL_DATA:
                /* Only 16 bit I data */
                if (iqValue[0] != ((ssiTestModeConfig->fixedDataPatternToCheck >> 16) & 0xFFFF))
                {
                    ssiTestModeStatus->dataError = 1;
                }
                break;
            case ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA:
            case ADI_ADRV9001_SSI_FORMAT_15_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE:
            case ADI_ADRV9001_SSI_FORMAT_22_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE_8_BIT_GAIN_INDEX:
                /* Only 16 bit I and Q data */
                if ((iqValue[0] != ((ssiTestModeConfig->fixedDataPatternToCheck >> 16) & 0xFFFF)) ||
                    (iqValue[1] != (ssiTestModeConfig->fixedDataPatternToCheck & 0xFFFF)))
                {
                    ssiTestModeStatus->dataError = 1;
                }
                break;
            default:
                /* This should not happen */
                break;
            }
        }
        else
        {
            ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDebugRampShiftError_Get, device, baseAddress, &ssiTestModeStatus->dataError);
        }

	    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDdrSel_Get, device, baseAddress, &ddrSet);
	    if (ddrSet == 0)
	    {
		    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxStrobeAlignError_Get, device, baseAddress, &ssiTestModeStatus->strobeAlignError);
	    }
	    else
	    {
		    ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxDdrStrobeAlignError_Get, device, baseAddress, &ssiTestModeStatus->strobeAlignError);
	    }

        ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxFifoFull_Get, device, baseAddress, &ssiTestModeStatus->fifoFull);

        ADI_EXPECT(adrv9001_NvsRegmapTx_CssiTxFifoEmpty_Get, device, baseAddress, &ssiTestModeStatus->fifoEmpty);
    }
    else /* LVDS */
    {
        if ((ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_16_BIT == ssiTestModeConfig->testData) ||
            (ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS15 == ssiTestModeConfig->testData))
        {
            ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugPrbs15RampShiftErrorCount_Get, device, baseAddress, &ssiTestModeStatus->dataError);
        }
        else if (ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS7 == ssiTestModeConfig->testData)
        {
            ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugPrbs7ErrorCount_Get, device, baseAddress, &ssiTestModeStatus->dataError);
        }
        else /* Fixed Pattern */
        {
            for (i = 0; i < 2; i++)
            {
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugMode_Set, device, baseAddress, 0x1);
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugStartRamp_Set, device, baseAddress, 0x0);
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugQSel_Set, device, baseAddress, i);   // 0: I_data; 1: Q_data
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxClearStrobeAlignError_Set, device, baseAddress, 0x0);
	            ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxClearStrobeAlignError_Set, device, baseAddress, 0x1);
	            ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxClearStrobeAlignError_Set, device, baseAddress, 0x0);
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxClearEarlyStrobeDetectedFlag_Set, device, baseAddress, 0x0);
	            ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxClearEarlyStrobeDetectedFlag_Set, device, baseAddress, 0x1);
	            ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxClearEarlyStrobeDetectedFlag_Set, device, baseAddress, 0x0);
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugStartCapture_Set, device, baseAddress, 0x1);
                /* Wait for capture to complete */
                for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
                {
                    ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugStartCapture_Get, device, baseAddress, &captureComplete);
                    if ((captureComplete == 1) &&
                        (eventCheck < numEventChecks))
                    {
                        /* Wait for capture to complete */
                        halError = adi_common_hal_Wait_us(&device->common, waitInterval_us);
                        ADI_ERROR_REPORT(&device->common,
                                         ADI_COMMON_ERRSRC_ADI_HAL,
                                         halError,
                                         ADI_COMMON_ACT_ERR_CHECK_TIMER,
                                         device,
                                         "Timer not working");
                        ADI_ERROR_RETURN(device->common.error.newAction);
                    }
                    else
                    {
                        break;
                    }
                }
                /* if capture did not complete within the timeout period */
                if (captureComplete == 1)
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_ADRV9001_SRC_ARMCMD,
                                     ADI_COMMON_ERR_API_FAIL,
                                     ADI_ADRV9001_ACT_ERR_RESET_ARM,
                                     device,
                                     "LssiTxDebugCapture failed due to time out ");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
                ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxDebugCapturedData_Get, device, baseAddress, &dataRead);
                iqValue[i] = dataRead;
            }

            ssiTestModeStatus->dataError = 0;
            switch (dataFormat)
            {
            case ADI_ADRV9001_SSI_FORMAT_12_BIT_I_Q_DATA:
                /* Only 12 bit I and Q data */
                if ((iqValue[0] != ((ssiTestModeConfig->fixedDataPatternToCheck >> 16) & 0x0FFF)) ||
                    (iqValue[1] != (ssiTestModeConfig->fixedDataPatternToCheck & 0x0FFF)))
                {
                    ssiTestModeStatus->dataError = 1;
                }
                break;
            case ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA:
                /* 16/32 bit I and Q data */
                if ((iqValue[0] != ((ssiTestModeConfig->fixedDataPatternToCheck >> 16) & 0xFFFF)) ||
                    (iqValue[1] != (ssiTestModeConfig->fixedDataPatternToCheck & 0xFFFF)))
                {
                    ssiTestModeStatus->dataError = 1;
                }
                break;
            default:
                /* This should not happen */
                break;
            }
        }

        ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxStrobeAlignError_Get, device, baseAddress, &ssiTestModeStatus->strobeAlignError);

        ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxFifoFull_Get, device, baseAddress, &ssiTestModeStatus->fifoFull);

        ADI_EXPECT(adrv9001_NvsRegmapTx_LssiTxFifoEmpty_Get, device, baseAddress, &ssiTestModeStatus->fifoEmpty);
    }

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Ssi_Loopback_Set_Validate(adi_adrv9001_Device_t *device,
                                                                     adi_common_ChannelNumber_e channel,
                                                                     adi_adrv9001_SsiType_e ssiType)
{
    ADI_RANGE_CHECK(device, ssiType, ADI_ADRV9001_SSI_TYPE_CMOS, ADI_ADRV9001_SSI_TYPE_LVDS);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Ssi_Loopback_Set(adi_adrv9001_Device_t *device,
                                      adi_common_ChannelNumber_e channel,
                                      adi_adrv9001_SsiType_e ssiType,
                                      bool loopbackEnable)
{
    adrv9001_BfNvsRegmapRx_e instance = ADRV9001_BF_RX1_CORE;
    uint8_t channelIdx = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Ssi_Loopback_Set_Validate, device, channel, ssiType);

    adi_common_channel_to_index(channel, &channelIdx);
    instance = nvsRegmapRxInstances[channelIdx];

    if (ADI_ADRV9001_SSI_TYPE_CMOS == ssiType)
    {
        ADI_EXPECT(adrv9001_NvsRegmapRx_CssiRxLoopbackFromTx_Set, device, instance, (uint8_t)loopbackEnable);
    }
    else // LVDS
    {
        ADI_EXPECT(adrv9001_NvsRegmapRx_LssiRxLoopbackFromTx_Set, device, instance, (uint8_t)loopbackEnable);
    }
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv9001_Ssi_LvdsDelayConfigSet(adi_adrv9001_Device_t *device,
                                                   adi_adrv9001_SsiCalibrationCfg_t *ssiCalibrationCfg)
{
    ADI_EXPECT(adrv9001_NvsRegmapCore_Rx1ClkLvdsDelay_Set,      device, ssiCalibrationCfg->rxClkDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Rx1StrobeLvdsDelay_Set,   device, ssiCalibrationCfg->rxStrobeDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Rx1IdataLvdsDelay_Set,    device, ssiCalibrationCfg->rxIDataDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Rx1QdataLvdsDelay_Set,    device, ssiCalibrationCfg->rxQDataDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore_Rx2ClkLvdsDelay_Set,      device, ssiCalibrationCfg->rxClkDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Rx2StrobeLvdsDelay_Set,   device, ssiCalibrationCfg->rxStrobeDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Rx2IdataLvdsDelay_Set,    device, ssiCalibrationCfg->rxIDataDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Rx2QdataLvdsDelay_Set,    device, ssiCalibrationCfg->rxQDataDelay[1]);

    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx1ClkLvdsDelay_Set,      device, ssiCalibrationCfg->txClkDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx1RefclkLvdsDelay_Set,   device, ssiCalibrationCfg->txRefClkDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx1StrobeLvdsDelay_Set,   device, ssiCalibrationCfg->txStrobeDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx1IdataLvdsDelay_Set,    device, ssiCalibrationCfg->txIDataDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx1QdataLvdsDelay_Set,    device, ssiCalibrationCfg->txQDataDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx2ClkLvdsDelay_Set,      device, ssiCalibrationCfg->txClkDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx2RefclkLvdsDelay_Set,   device, ssiCalibrationCfg->txRefClkDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx2StrobeLvdsDelay_Set,   device, ssiCalibrationCfg->txStrobeDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx2IdataLvdsDelay_Set,    device, ssiCalibrationCfg->txIDataDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx2QdataLvdsDelay_Set,    device, ssiCalibrationCfg->txQDataDelay[1]);

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv9001_Ssi_CmosDelayConfigSet(adi_adrv9001_Device_t *device,
                                                   adi_adrv9001_SsiCalibrationCfg_t *ssiCalibrationCfg)
{
    /* Rx1 */
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx1ClkCmosNDelay_Set,    device, ssiCalibrationCfg->rxClkDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx1ClkCmosPDelay_Set,    device, ssiCalibrationCfg->rxClkDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx1StrobeCmosNDelay_Set, device, ssiCalibrationCfg->rxStrobeDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx1StrobeCmosPDelay_Set, device, ssiCalibrationCfg->rxStrobeDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx1IdataCmosNDelay_Set,  device, ssiCalibrationCfg->rxIDataDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx1IdataCmosPDelay_Set,  device, ssiCalibrationCfg->rxIDataDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx1QdataCmosNDelay_Set,  device, ssiCalibrationCfg->rxQDataDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx1QdataCmosPDelay_Set,  device, ssiCalibrationCfg->rxQDataDelay[0]);

    /* Rx2 */
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx2ClkCmosNDelay_Set,    device, ssiCalibrationCfg->rxClkDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx2ClkCmosPDelay_Set,    device, ssiCalibrationCfg->rxClkDelay[1]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx2StrobeCmosNDelay_Set, device, ssiCalibrationCfg->rxStrobeDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx2StrobeCmosPDelay_Set, device, ssiCalibrationCfg->rxStrobeDelay[1]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx2IdataCmosNDelay_Set,  device, ssiCalibrationCfg->rxIDataDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx2IdataCmosPDelay_Set,  device, ssiCalibrationCfg->rxIDataDelay[1]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx2QdataCmosNDelay_Set,  device, ssiCalibrationCfg->rxQDataDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx2QdataCmosPDelay_Set,  device, ssiCalibrationCfg->rxQDataDelay[1]);

    /* Tx1 */
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1ClkCmosNDelay_Set,    device, ssiCalibrationCfg->txClkDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1ClkCmosPDelay_Set,    device, ssiCalibrationCfg->txClkDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1RefclkCmosNDelay_Set, device, ssiCalibrationCfg->txRefClkDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1RefclkCmosPDelay_Set, device, ssiCalibrationCfg->txRefClkDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1StrobeCmosNDelay_Set, device, ssiCalibrationCfg->txStrobeDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1StrobeCmosPDelay_Set, device, ssiCalibrationCfg->txStrobeDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1IdataCmosNDelay_Set,  device, ssiCalibrationCfg->txIDataDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1IdataCmosPDelay_Set,  device, ssiCalibrationCfg->txIDataDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1QdataCmosNDelay_Set,  device, ssiCalibrationCfg->txQDataDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1QdataCmosPDelay_Set,  device, ssiCalibrationCfg->txQDataDelay[0]);

    /* Tx2 */
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2ClkCmosNDelay_Set,    device, ssiCalibrationCfg->txClkDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2ClkCmosPDelay_Set,    device, ssiCalibrationCfg->txClkDelay[1]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2RefclkCmosNDelay_Set, device, ssiCalibrationCfg->txRefClkDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2RefclkCmosPDelay_Set, device, ssiCalibrationCfg->txRefClkDelay[1]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2StrobeCmosNDelay_Set, device, ssiCalibrationCfg->txStrobeDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2StrobeCmosPDelay_Set, device, ssiCalibrationCfg->txStrobeDelay[1]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2IdataCmosNDelay_Set,  device, ssiCalibrationCfg->txIDataDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2IdataCmosPDelay_Set,  device, ssiCalibrationCfg->txIDataDelay[1]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2QdataCmosNDelay_Set,  device, ssiCalibrationCfg->txQDataDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2QdataCmosPDelay_Set,  device, ssiCalibrationCfg->txQDataDelay[1]);

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Ssi_Delay_Configure_Validate(adi_adrv9001_Device_t *device,
                                                                        adi_adrv9001_SsiType_e ssiType,
                                                                        adi_adrv9001_SsiCalibrationCfg_t *ssiCalibration)
{
    static const uint8_t MAX_DELAY = 7;
    ADI_RANGE_CHECK(device, ssiType, ADI_ADRV9001_SSI_TYPE_CMOS, ADI_ADRV9001_SSI_TYPE_LVDS);

    ADI_NULL_PTR_RETURN(&device->common, ssiCalibration);

    ADI_RANGE_CHECK(device, ssiCalibration->rxClkDelay[0],      0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->rxClkDelay[1],      0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->rxStrobeDelay[0],   0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->rxStrobeDelay[1],   0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->rxIDataDelay[0],    0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->rxIDataDelay[1],    0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->rxQDataDelay[0],    0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->rxQDataDelay[1],    0, MAX_DELAY);

    ADI_RANGE_CHECK(device, ssiCalibration->txClkDelay[0],      0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->txClkDelay[1],      0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->txRefClkDelay[0],   0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->txRefClkDelay[1],   0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->txStrobeDelay[0],   0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->txStrobeDelay[1],   0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->txIDataDelay[0],    0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->txIDataDelay[1],    0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->txQDataDelay[0],    0, MAX_DELAY);
    ADI_RANGE_CHECK(device, ssiCalibration->txQDataDelay[1],    0, MAX_DELAY);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Ssi_Delay_Configure(adi_adrv9001_Device_t *device,
                                         adi_adrv9001_SsiType_e ssiType,
                                         adi_adrv9001_SsiCalibrationCfg_t *ssiCalibration)
{
    ADI_PERFORM_VALIDATION(adi_adrv9001_Ssi_Delay_Configure_Validate, device, ssiType, ssiCalibration);

    if (ADI_ADRV9001_SSI_TYPE_LVDS == ssiType)
    {
        ADI_EXPECT(adi_adrv9001_Ssi_LvdsDelayConfigSet, device, ssiCalibration);
    }
    else
    {
        ADI_EXPECT(adi_adrv9001_Ssi_CmosDelayConfigSet, device, ssiCalibration);
    }

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv9001_Ssi_LvdsDelayConfigGet(adi_adrv9001_Device_t *device,
                                                   adi_adrv9001_SsiCalibrationCfg_t *ssiCalibrationCfg)
{
    ADI_EXPECT(adrv9001_NvsRegmapCore_Rx1ClkLvdsDelay_Get,      device, &ssiCalibrationCfg->rxClkDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Rx1StrobeLvdsDelay_Get,   device, &ssiCalibrationCfg->rxStrobeDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Rx1IdataLvdsDelay_Get,    device, &ssiCalibrationCfg->rxIDataDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Rx1QdataLvdsDelay_Get,    device, &ssiCalibrationCfg->rxQDataDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore_Rx2ClkLvdsDelay_Get,      device, &ssiCalibrationCfg->rxClkDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Rx2StrobeLvdsDelay_Get,   device, &ssiCalibrationCfg->rxStrobeDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Rx2IdataLvdsDelay_Get,    device, &ssiCalibrationCfg->rxIDataDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Rx2QdataLvdsDelay_Get,    device, &ssiCalibrationCfg->rxQDataDelay[1]);

    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx1ClkLvdsDelay_Get,      device, &ssiCalibrationCfg->txClkDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx1RefclkLvdsDelay_Get,   device, &ssiCalibrationCfg->txRefClkDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx1StrobeLvdsDelay_Get,   device, &ssiCalibrationCfg->txStrobeDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx1IdataLvdsDelay_Get,    device, &ssiCalibrationCfg->txIDataDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx1QdataLvdsDelay_Get,    device, &ssiCalibrationCfg->txQDataDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx2ClkLvdsDelay_Get,      device, &ssiCalibrationCfg->txClkDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx2RefclkLvdsDelay_Get,   device, &ssiCalibrationCfg->txRefClkDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx2StrobeLvdsDelay_Get,   device, &ssiCalibrationCfg->txStrobeDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx2IdataLvdsDelay_Get,    device, &ssiCalibrationCfg->txIDataDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore_Tx2QdataLvdsDelay_Get,    device, &ssiCalibrationCfg->txQDataDelay[1]);

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv9001_Ssi_CmosDelayConfigGet(adi_adrv9001_Device_t *device,
                                                   adi_adrv9001_SsiCalibrationCfg_t *ssiCalibrationCfg)
{
    /* Rx1 */
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx1ClkCmosNDelay_Get,    device, &ssiCalibrationCfg->rxClkDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx1ClkCmosPDelay_Get,    device, &ssiCalibrationCfg->rxClkDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx1StrobeCmosNDelay_Get, device, &ssiCalibrationCfg->rxStrobeDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx1StrobeCmosPDelay_Get, device, &ssiCalibrationCfg->rxStrobeDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx1IdataCmosNDelay_Get,  device, &ssiCalibrationCfg->rxIDataDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx1IdataCmosPDelay_Get,  device, &ssiCalibrationCfg->rxIDataDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx1QdataCmosNDelay_Get,  device, &ssiCalibrationCfg->rxQDataDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx1QdataCmosPDelay_Get,  device, &ssiCalibrationCfg->rxQDataDelay[0]);

    /* Rx2 */
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx2ClkCmosNDelay_Get,    device, &ssiCalibrationCfg->rxClkDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx2ClkCmosPDelay_Get,    device, &ssiCalibrationCfg->rxClkDelay[1]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx2StrobeCmosNDelay_Get, device, &ssiCalibrationCfg->rxStrobeDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx2StrobeCmosPDelay_Get, device, &ssiCalibrationCfg->rxStrobeDelay[1]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx2IdataCmosNDelay_Get,  device, &ssiCalibrationCfg->rxIDataDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx2IdataCmosPDelay_Get,  device, &ssiCalibrationCfg->rxIDataDelay[1]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx2QdataCmosNDelay_Get,  device, &ssiCalibrationCfg->rxQDataDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Rx2QdataCmosPDelay_Get,  device, &ssiCalibrationCfg->rxQDataDelay[1]);

    /* Tx1 */
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1ClkCmosNDelay_Get,    device, &ssiCalibrationCfg->txClkDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1ClkCmosPDelay_Get,    device, &ssiCalibrationCfg->txClkDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1RefclkCmosNDelay_Get, device, &ssiCalibrationCfg->txRefClkDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1RefclkCmosPDelay_Get, device, &ssiCalibrationCfg->txRefClkDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1StrobeCmosNDelay_Get, device, &ssiCalibrationCfg->txStrobeDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1StrobeCmosPDelay_Get, device, &ssiCalibrationCfg->txStrobeDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1IdataCmosNDelay_Get,  device, &ssiCalibrationCfg->txIDataDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1IdataCmosPDelay_Get,  device, &ssiCalibrationCfg->txIDataDelay[0]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1QdataCmosNDelay_Get,  device, &ssiCalibrationCfg->txQDataDelay[0]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1QdataCmosPDelay_Get,  device, &ssiCalibrationCfg->txQDataDelay[0]);

    /* Tx2 */
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2ClkCmosNDelay_Get,    device, &ssiCalibrationCfg->txClkDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2ClkCmosPDelay_Get,    device, &ssiCalibrationCfg->txClkDelay[1]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2RefclkCmosNDelay_Get, device, &ssiCalibrationCfg->txRefClkDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2RefclkCmosPDelay_Get, device, &ssiCalibrationCfg->txRefClkDelay[1]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2StrobeCmosNDelay_Get, device, &ssiCalibrationCfg->txStrobeDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2StrobeCmosPDelay_Get, device, &ssiCalibrationCfg->txStrobeDelay[1]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2IdataCmosNDelay_Get,  device, &ssiCalibrationCfg->txIDataDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2IdataCmosPDelay_Get,  device, &ssiCalibrationCfg->txIDataDelay[1]);

    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2QdataCmosNDelay_Get,  device, &ssiCalibrationCfg->txQDataDelay[1]);
    ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2QdataCmosPDelay_Get,  device, &ssiCalibrationCfg->txQDataDelay[1]);

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Ssi_Delay_Inspect_Validate(adi_adrv9001_Device_t *device,
                                                                      adi_adrv9001_SsiType_e ssiType,
                                                                      adi_adrv9001_SsiCalibrationCfg_t *ssiCalibration)
{
    ADI_RANGE_CHECK(device, ssiType, ADI_ADRV9001_SSI_TYPE_CMOS, ADI_ADRV9001_SSI_TYPE_LVDS);
    ADI_NULL_PTR_RETURN(&device->common, ssiCalibration);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Ssi_Delay_Inspect(adi_adrv9001_Device_t *device,
                                       adi_adrv9001_SsiType_e ssiType,
                                       adi_adrv9001_SsiCalibrationCfg_t *ssiCalibration)
{
    ADI_PERFORM_VALIDATION(adi_adrv9001_Ssi_Delay_Inspect_Validate, device, ssiType, ssiCalibration);

    if (ADI_ADRV9001_SSI_TYPE_LVDS == ssiType)
    {
        ADI_EXPECT(adi_adrv9001_Ssi_LvdsDelayConfigGet, device, ssiCalibration);
    }
    else
    {
        ADI_EXPECT(adi_adrv9001_Ssi_CmosDelayConfigGet, device, ssiCalibration);
    }

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Ssi_PowerDown_Set_Validate(adi_adrv9001_Device_t *device,
                                                                      adi_common_Port_e port,
                                                                      adi_common_ChannelNumber_e channel,
                                                                      adi_adrv9001_SsiPowerDown_e powerDownMode)
{
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;

    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_TX);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(device, powerDownMode, ADI_ADRV9001_SSI_POWER_DOWN_DISABLED, ADI_ADRV9001_SSI_POWER_DOWN_HIGH);

    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_RX, channel, &state);
    if (ADI_ADRV9001_CHANNEL_STANDBY != state)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            currentState.channelStates[port_index][chan_index],
            "Specified channel must be in STANDBY state to set LSSI power saving mode.");
        ADI_API_RETURN(device)
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Ssi_PowerDown_Set(adi_adrv9001_Device_t *device,
                                       adi_common_Port_e port,
                                       adi_common_ChannelNumber_e channel,
                                       adi_adrv9001_SsiPowerDown_e powerDownMode)
{
    uint8_t armData[5] = { 0 };
    uint8_t extData[3] = { 0 };
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Ssi_PowerDown_Set_Validate, device, port, channel, powerDownMode);

    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
    armData[offset++] = (uint8_t)powerDownMode;

    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(port, channel);
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_CFG_LSSI_PADS_WHEN_NOT_USED;

    ADI_EXPECT(adi_adrv9001_arm_Config_Write, device, armData, sizeof(armData), extData, sizeof(extData))

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Ssi_PowerDown_Get_Validate(adi_adrv9001_Device_t *device,
                                                                      adi_common_Port_e port,
                                                                      adi_common_ChannelNumber_e channel,
                                                                      adi_adrv9001_SsiPowerDown_e *powerDownMode)
{
    ADI_RANGE_CHECK(device, port, ADI_RX, ADI_TX);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, powerDownMode);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Ssi_PowerDown_Get(adi_adrv9001_Device_t *device,
                                       adi_common_Port_e port,
                                       adi_common_ChannelNumber_e channel,
                                       adi_adrv9001_SsiPowerDown_e *powerDownMode)
{
    uint8_t armReadBack[1] = { 0 };
    uint8_t channelMask = 0;
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Ssi_PowerDown_Get_Validate, device, port , channel, powerDownMode);

    channelMask = adi_adrv9001_Radio_MailboxChannel_Get(port, channel);

    ADI_EXPECT(adi_adrv9001_arm_Config_Read, device, OBJID_CFG_LSSI_PADS_WHEN_NOT_USED, channelMask, offset, armReadBack, sizeof(armReadBack))

    *powerDownMode = (adi_adrv9001_SsiPowerDown_e)armReadBack[0];

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Ssi_Rx_GainSelect_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                                adi_common_ChannelNumber_e channel,
                                                                                adi_adrv9001_SsiRxGainSelectCfg_t *ssiRxGainSelectCfg)
{
    static const uint8_t MAX_DELAY = 31;
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;

    ADI_NULL_PTR_RETURN(&adrv9001->common, ssiRxGainSelectCfg);
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(adrv9001, ssiRxGainSelectCfg->gainSelect, ADI_ADRV9001_RX_GAIN_INDEX_GAIN_CHANGE_ZEROES, ADI_ADRV9001_RX_GAIN_INDEX_GAIN_CHANGE_AGC);
    ADI_RANGE_CHECK(adrv9001, ssiRxGainSelectCfg->interfaceGainDelay, 0, MAX_DELAY);

    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, adrv9001, ADI_RX, channel, &state);
    switch (state)
    {
    case ADI_ADRV9001_CHANNEL_STANDBY:  /* Falls through */
    case ADI_ADRV9001_CHANNEL_CALIBRATED:
        break;
    default:
        ADI_ERROR_REPORT(&adrv9001->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            state,
            "Invalid channel state. State must be STANDBY or CALIBRATED");
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_Ssi_Rx_GainSelect_Configure(adi_adrv9001_Device_t *adrv9001,
                                                 adi_common_ChannelNumber_e channel,
                                                 adi_adrv9001_SsiRxGainSelectCfg_t *ssiRxGainSelectCfg)
{
    uint8_t armData[7] = { 0 };
    uint8_t extData[3] = { 0 };
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Ssi_Rx_GainSelect_Configure_Validate, adrv9001, channel, ssiRxGainSelectCfg);
    
    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
    armData[offset++] = (uint8_t)ssiRxGainSelectCfg->gainSelect;
    armData[offset++] = (uint8_t)ssiRxGainSelectCfg->agcGainDelay;
    armData[offset++] = (uint8_t)ssiRxGainSelectCfg->interfaceGainDelay;

    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_RX, channel);
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_CFG_RX_GAIN_OVER_SSI;

    ADI_EXPECT(adi_adrv9001_arm_Config_Write, adrv9001, armData, sizeof(armData), extData, sizeof(extData))

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Ssi_Rx_GainSelect_Inspect_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                              adi_common_ChannelNumber_e channel,
                                                                              adi_adrv9001_SsiRxGainSelectCfg_t *ssiRxGainSelectCfg)
{
    ADI_NULL_PTR_RETURN(&adrv9001->common, ssiRxGainSelectCfg);
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_Ssi_Rx_GainSelect_Inspect(adi_adrv9001_Device_t *adrv9001,
                                               adi_common_ChannelNumber_e channel,
                                               adi_adrv9001_SsiRxGainSelectCfg_t *ssiRxGainSelectCfg)
{
    uint8_t armReadBack[3] = { 0 };
    uint8_t channelMask = adi_adrv9001_Radio_MailboxChannel_Get(ADI_RX, channel);
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Ssi_Rx_GainSelect_Inspect_Validate, adrv9001, channel, ssiRxGainSelectCfg);
    ADI_EXPECT(adi_adrv9001_arm_Config_Read, adrv9001, OBJID_CFG_RX_GAIN_OVER_SSI, channelMask, offset, armReadBack, sizeof(armReadBack))
    
    ssiRxGainSelectCfg->gainSelect         = (adi_adrv9001_SsiRxGainSelect_e) armReadBack[offset++];
    ssiRxGainSelectCfg->agcGainDelay       = armReadBack[offset++];
    ssiRxGainSelectCfg->interfaceGainDelay = armReadBack[offset++];

    ADI_API_RETURN(adrv9001);
}
