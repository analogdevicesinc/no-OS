/**
* \file
* \brief Contains SSI calibration related function implementation defined in
* adi_adrv910x_ssi.h
*
* ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
*/

/**
* Copyright 2019 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv910x_ssi.h"
#include "adi_adrv910x_arm.h"
#include "adi_adrv910x_error.h"
#include "adi_adrv910x_radio.h"
#include "adi_adrv910x_spi.h"
#include "adi_adrv910x_user.h"

#include "adrv910x_arm.h"
#include "adrv910x_arm_macros.h"
#include "adrv910x_bf.h"
#include "adrv910x_init.h"
#include "adrv910x_reg_addr_macros.h"
#include "object_ids.h"

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Ssi_Rx_TestMode_Configure_Validate(adi_adrv910x_Device_t *device,
	adi_common_ChannelNumber_e channel,
	ssiType_e ssiType,
	ssiDataFormat_e dataFormat,
	adi_adrv910x_RxSsiTestModeCfg_t *ssiTestModeConfig)
{
	ADI_NULL_PTR_RETURN(&device->common, ssiTestModeConfig);
	ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
	ADI_RANGE_CHECK(device, ssiType, SSI_TYPE_CMOS, SSI_TYPE_LVDS);
	ADI_RANGE_CHECK(device, dataFormat, SSI_FORMAT_2_BIT_SYMBOL_DATA, SSI_FORMAT_32_BIT_I_Q_DATA_22_BIT_GAIN_INDEX_AND_CHANGE);

	if (SSI_TYPE_CMOS == ssiType)
	{
		ADI_RANGE_CHECK(device, ssiTestModeConfig->testData, ADI_ADRV910X_SSI_TESTMODE_DATA_NORMAL, ADI_ADRV910X_SSI_TESTMODE_DATA_RAMP_16_BIT);
	}
	else
	{
		ADI_RANGE_CHECK(device, ssiTestModeConfig->testData, ADI_ADRV910X_SSI_TESTMODE_DATA_NORMAL, ADI_ADRV910X_SSI_TESTMODE_DATA_PRBS7);
		if (ADI_ADRV910X_SSI_TESTMODE_DATA_RAMP_NIBBLE == ssiTestModeConfig->testData)
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

int32_t adi_adrv910x_ssi_Rx_TestMode_Configure(adi_adrv910x_Device_t *device,
	adi_common_ChannelNumber_e channel,
	ssiType_e ssiType,
	ssiDataFormat_e dataFormat,
	adi_adrv910x_RxSsiTestModeCfg_t *ssiTestModeConfig)
{
	int8_t i = 0;
	adrv910x_BfNvsRegmapRxCommon_e instance;
	uint8_t channelIdx = 0;

	ADI_PERFORM_VALIDATION(adi_adrv910x_Ssi_Rx_TestMode_Configure_Validate, device, channel, ssiType, dataFormat, ssiTestModeConfig);

	adi_common_channel_to_index(channel, &channelIdx);
	instance = nvsRegmapRxCommonInstances[channelIdx];

    ADI_MUTEX_AQUIRE(device);
	if (SSI_TYPE_CMOS == ssiType)
	{
		if (ADI_ADRV910X_SSI_TESTMODE_DATA_NORMAL == ssiTestModeConfig->testData)
		{
			ADI_EXPECT(adrv910x_NvsRegmapRxCommon_CssiRxDebugMode_Set, device, instance, 0x0);
		}
		else
		{
			ADI_EXPECT(adrv910x_NvsRegmapRxCommon_CssiRxDebugMode_Set, device, instance, 0x1);
			if (ADI_ADRV910X_SSI_TESTMODE_DATA_RAMP_NIBBLE == ssiTestModeConfig->testData)
			{
				ADI_EXPECT(adrv910x_NvsRegmapRxCommon_CssiRxDebugLoad4bitsMode_Set, device, instance, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapRxCommon_CssiRxDebugStartRamp_Set, device, instance, 0x1);
			}
			else if (ADI_ADRV910X_SSI_TESTMODE_DATA_RAMP_16_BIT == ssiTestModeConfig->testData)
			{
				ADI_EXPECT(adrv910x_NvsRegmapRxCommon_CssiRxDebugLoad4bitsMode_Set, device, instance, 0x0);
				ADI_EXPECT(adrv910x_NvsRegmapRxCommon_CssiRxDebugStartRamp_Set, device, instance, 0x1);
			}
			else /* Fixed pattern */
			{
				ADI_EXPECT(adrv910x_NvsRegmapRxCommon_CssiRxDebugStartRamp_Set, device, instance, 0x0);

				for (i = 0; i < 2; i++)
				{
					ADI_EXPECT(adrv910x_NvsRegmapRxCommon_CssiRxDebugQSel_Set, device, instance, i); // 0: I_data; 1: Q_data
					ADI_EXPECT(adrv910x_NvsRegmapRxCommon_CssiRxDebugNibbleSel_Set, device, instance, 0x3);
					ADI_EXPECT(adrv910x_NvsRegmapRxCommon_CssiRxDebugLoadValue_Set, device, instance, (uint8_t)(ssiTestModeConfig->fixedDataPatternToTransmit & 0xF));
					ADI_EXPECT(adrv910x_NvsRegmapRxCommon_CssiRxDebugLoad_Set, device, instance, 0x1);
					adi_common_hal_Wait_us(&device->common, 1000);
					ADI_EXPECT(adrv910x_NvsRegmapRxCommon_CssiRxDebugLoad_Set, device, instance, 0x0);
				}
			}
		}
	}
	else /* LVDS */
	{
		ADI_EXPECT(adrv910x_NvsRegmapRxCommon_LssiRxDebugPrbs7Enable_Set, device, instance, 0x0);
		ADI_EXPECT(adrv910x_NvsRegmapRxCommon_LssiRxDebugPrbs15Enable_Set, device, instance, 0x0);
		ADI_EXPECT(adrv910x_NvsRegmapRxCommon_LssiRxDebugStartRamp_Set, device, instance, 0x0);
		if (ADI_ADRV910X_SSI_TESTMODE_DATA_NORMAL == ssiTestModeConfig->testData)
		{
			ADI_EXPECT(adrv910x_NvsRegmapRxCommon_LssiRxDebugMode_Set, device, instance, 0x0);
		}
		else
		{
			ADI_EXPECT(adrv910x_NvsRegmapRxCommon_LssiRxDebugMode_Set, device, instance, 0x1);
			if (ADI_ADRV910X_SSI_TESTMODE_DATA_RAMP_16_BIT == ssiTestModeConfig->testData)
			{
				ADI_EXPECT(adrv910x_NvsRegmapRxCommon_LssiRxDebugStartRamp_Set, device, instance, 0x1);
			}
			else if (ADI_ADRV910X_SSI_TESTMODE_DATA_PRBS7 == ssiTestModeConfig->testData)
			{
				ADI_EXPECT(adrv910x_NvsRegmapRxCommon_LssiRxDebugPrbs7Restart_Set, device, instance, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapRxCommon_LssiRxDebugPrbs7Enable_Set, device, instance, 0x1);
			}
			else if (ADI_ADRV910X_SSI_TESTMODE_DATA_PRBS15 == ssiTestModeConfig->testData)
			{
				ADI_EXPECT(adrv910x_NvsRegmapRxCommon_LssiRxDebugPrbs15Restart_Set, device, instance, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapRxCommon_LssiRxDebugPrbs15Enable_Set, device, instance, 0x1);
			}
			else /* Fixed Pattern */
			{
				ADI_EXPECT(adrv910x_NvsRegmapRxCommon_LssiRxDebugLoadValue_Set, device, instance, (uint16_t)(ssiTestModeConfig->fixedDataPatternToTransmit & 0xFFFF));
				ADI_EXPECT(adrv910x_NvsRegmapRxCommon_LssiRxDebugLoad_Set, device, instance, 0x1);
				adi_common_hal_Wait_us(&device->common, 1000);
				ADI_EXPECT(adrv910x_NvsRegmapRxCommon_LssiRxDebugLoad_Set, device, instance, 0x0);
			}
		}
	}

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Ssi_Tx_TestMode_Configure_Validate(adi_adrv910x_Device_t *device,
	adi_common_ChannelNumber_e channel,
	ssiType_e ssiType,
	ssiDataFormat_e dataFormat,
	adi_adrv910x_TxSsiTestModeCfg_t *ssiTestModeConfig)
{
	ADI_NULL_PTR_RETURN(&device->common, ssiTestModeConfig);
	ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
	ADI_RANGE_CHECK(device, ssiType, SSI_TYPE_CMOS, SSI_TYPE_LVDS);
	ADI_RANGE_CHECK(device, dataFormat, SSI_FORMAT_2_BIT_SYMBOL_DATA, SSI_FORMAT_32_BIT_I_Q_DATA_22_BIT_GAIN_INDEX_AND_CHANGE);

	if (SSI_TYPE_CMOS == ssiType)
	{
		ADI_RANGE_CHECK(device, ssiTestModeConfig->testData, ADI_ADRV910X_SSI_TESTMODE_DATA_NORMAL, ADI_ADRV910X_SSI_TESTMODE_DATA_RAMP_16_BIT);
	}
	else
	{
		ADI_RANGE_CHECK(device, ssiTestModeConfig->testData, ADI_ADRV910X_SSI_TESTMODE_DATA_NORMAL, ADI_ADRV910X_SSI_TESTMODE_DATA_PRBS7);
		if (ADI_ADRV910X_SSI_TESTMODE_DATA_RAMP_NIBBLE == ssiTestModeConfig->testData)
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

static __maybe_unused int32_t adi_adrv910x_Ssi_Tx_CssiClearErrorFlags(adi_adrv910x_Device_t *device)
{
	ADI_MUTEX_AQUIRE(device);
	ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugStartRamp_Set, device, 0x0);
	ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearRampShiftError_Set, device, 0x0);
	ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearRampShiftError_Set, device, 0x1);
	ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearRampShiftError_Set, device, 0x0);
	ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxFifoClear_Set, device, 0x0);
	ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxFifoClear_Set, device, 0x1);
	ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxFifoClear_Set, device, 0x0);
	ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, 0x0);
	ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, 0x1);
	ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, 0x0);
	ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, 0x0);
	ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, 0x1);
	ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, 0x0);
	ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugStartRamp_Set, device, 0x1);
	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}
    
int32_t adi_adrv910x_Ssi_Tx_TestMode_Configure(adi_adrv910x_Device_t *device,
	adi_common_ChannelNumber_e channel,
	ssiType_e ssiType,
	ssiDataFormat_e dataFormat,
	adi_adrv910x_TxSsiTestModeCfg_t *ssiTestModeConfig)
{
	int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

	ADI_PERFORM_VALIDATION(adi_adrv910x_Ssi_Tx_TestMode_Configure_Validate, device, channel, ssiType, dataFormat, ssiTestModeConfig);

	ADI_MUTEX_AQUIRE(device);
	if (SSI_TYPE_CMOS == ssiType)
	{
		if (ADI_ADRV910X_SSI_TESTMODE_DATA_NORMAL == ssiTestModeConfig->testData)
		{
			ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugMode_Set, device, 0x0);
		}
		else
		{
			ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugMode_Set, device, 0x1);
			/* Nothing to be configured for fixed pattern */
			if (ADI_ADRV910X_SSI_TESTMODE_DATA_RAMP_NIBBLE == ssiTestModeConfig->testData)
			{
				ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugLoad4bitsMode_Set, device, 0x1);
				ADI_EXPECT(adi_adrv910x_Ssi_Tx_CssiClearErrorFlags, device);
			}
			else if (ADI_ADRV910X_SSI_TESTMODE_DATA_RAMP_16_BIT == ssiTestModeConfig->testData)
			{
				ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugLoad4bitsMode_Set, device, 0x0);
				ADI_EXPECT(adi_adrv910x_Ssi_Tx_CssiClearErrorFlags, device);
			}
		}
	}
	else /* LVDS */
	{
		ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugStartRamp_Set, device, 0x0);
		ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugPrbs7Enable_Set, device, 0x0);
		ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugPrbs15Enable_Set, device, 0x0);
		ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugStartErrorCheck_Set, device, 0x0);
		if (ADI_ADRV910X_SSI_TESTMODE_DATA_NORMAL == ssiTestModeConfig->testData)
		{
			ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugMode_Set, device, 0x0);
		}
		else
		{
			/* Disable TX1 interface  will clear all local flags (strobe_align, late_strobe, prbs_error, �)*/
			ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxFifoClkEnable_Set, device, 0); 
			ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxFifoEnable_Set, device, 0); 
			ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxEnable_Set, device, 0);
			ADI_EXPECT(adi_adrv910x_Ssi_Tx_Disable_LSSI_Pads, device, ADI_CHANNEL_1, SSI_TYPE_LVDS);

			/* Enable the TX1 FIFO interface and Reset TX1 FIFO pointers */ 
			ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxFifoClkEnable_Set, device, 1); 
			ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxFifoEnable_Set, device, 1); 
			ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxFifoWriteReset_Set, device, 1); 
			ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxFifoReadReset_Set, device, 1); 
 
			/* Enable TX1 LSSI interface pads */
			ADI_EXPECT(adi_adrv910x_Ssi_Tx_Enable_LSSI_Pads, device, ADI_CHANNEL_1, SSI_TYPE_LVDS);
 
			/* start TX1 interface � will start monitoring Strobe to set early/late flag */ 
			ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxEnable_Set, device, 1); 

			/* Release TX1 FIFO */ 
			ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxFifoWriteReset_Set, device, 0);  
			ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxFifoReadReset_Set, device, 0);
			if (ADI_ADRV910X_SSI_TESTMODE_DATA_RAMP_16_BIT == ssiTestModeConfig->testData)
			{
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugStartRamp_Set, device, 0x1);

				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugStartErrorCheck_Set, device, 0x1);

				recoveryAction = adi_common_hal_Wait_us(&device->common, 1000);

				ADI_ERROR_REPORT(&device->common,
					ADI_ADRV910X_SRC_ARMCMD,
					recoveryAction,
					ADI_COMMON_ACT_ERR_CHECK_TIMER,
					device,
					"Timer not working in adi_adrv910x_Ssi_Tx_TestMode_Configure()");
				ADI_MUTEX_ERROR_RELEASE(device);
				ADI_ERROR_RETURN(device->common.error.newAction);

				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugErrorCounterRead_Set, device, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugErrorCounterRead_Set, device, 0x0);
			}
			else if (ADI_ADRV910X_SSI_TESTMODE_DATA_PRBS7 == ssiTestModeConfig->testData)
			{
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugPrbs7Enable_Set, device, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugPrbs7ErrorClear_Set, device, 0x0);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugPrbs7ErrorClear_Set, device, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugPrbs7ErrorClear_Set, device, 0x0);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugPrbs7Restart_Set, device, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugStartErrorCheck_Set, device, 0x1);
				recoveryAction = adi_common_hal_Wait_us(&device->common, 1000);

				ADI_ERROR_REPORT(&device->common,
					ADI_ADRV910X_SRC_ARMCMD,
					recoveryAction,
					ADI_COMMON_ACT_ERR_CHECK_TIMER,
					device,
					"Timer not working in adi_adrv910x_Ssi_Tx_TestMode_Configure()");
				ADI_MUTEX_ERROR_RELEASE(device);
				ADI_ERROR_RETURN(device->common.error.newAction);

				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugErrorCounterRead_Set, device, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugErrorCounterRead_Set, device, 0x0);
			}
			else if (ADI_ADRV910X_SSI_TESTMODE_DATA_PRBS15 == ssiTestModeConfig->testData)
			{
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugPrbs15Enable_Set, device, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugAfterFifoErrorClear_Set, device, 0x0);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugAfterFifoErrorClear_Set, device, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugAfterFifoErrorClear_Set, device, 0x0);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugPrbs15Restart_Set, device, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugStartErrorCheck_Set, device, 0x1);
				recoveryAction = adi_common_hal_Wait_us(&device->common, 1000);

				ADI_ERROR_REPORT(&device->common,
					ADI_ADRV910X_SRC_ARMCMD,
					recoveryAction,
					ADI_COMMON_ACT_ERR_CHECK_TIMER,
					device,
					"Timer not working in adi_adrv910x_Ssi_Tx_TestMode_Configure()");
				ADI_MUTEX_ERROR_RELEASE(device);
				ADI_ERROR_RETURN(device->common.error.newAction);

				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugErrorCounterRead_Set, device, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugErrorCounterRead_Set, device, 0x0);
			}
			/* Nothing to be configured for fixed pattern */
		}
	}

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Ssi_Tx_TestMode_Status_Inspect_Validate(adi_adrv910x_Device_t *device,
	adi_common_ChannelNumber_e channel,
	ssiType_e ssiType,
	ssiDataFormat_e dataFormat,
	adi_adrv910x_TxSsiTestModeCfg_t *ssiTestModeConfig,
	adi_adrv910x_TxSsiTestModeStatus_t *ssiTestModeStatus)
{
	ADI_NULL_PTR_RETURN(&device->common, ssiTestModeConfig);
	ADI_NULL_PTR_RETURN(&device->common, ssiTestModeStatus);
	ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
	ADI_RANGE_CHECK(device, ssiType, SSI_TYPE_CMOS, SSI_TYPE_LVDS);

	if (SSI_TYPE_CMOS == ssiType)
	{
		ADI_RANGE_CHECK(device, ssiTestModeConfig->testData, ADI_ADRV910X_SSI_TESTMODE_DATA_NORMAL, ADI_ADRV910X_SSI_TESTMODE_DATA_RAMP_16_BIT);
		ADI_RANGE_CHECK(device, dataFormat, SSI_FORMAT_2_BIT_SYMBOL_DATA, SSI_FORMAT_32_BIT_I_Q_DATA_22_BIT_GAIN_INDEX_AND_CHANGE);
		if (SSI_FORMAT_12_BIT_I_Q_DATA == dataFormat)
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
		ADI_RANGE_CHECK(device, ssiTestModeConfig->testData, ADI_ADRV910X_SSI_TESTMODE_DATA_NORMAL, ADI_ADRV910X_SSI_TESTMODE_DATA_PRBS7);
		if (ADI_ADRV910X_SSI_TESTMODE_DATA_RAMP_NIBBLE == ssiTestModeConfig->testData)
		{
			ADI_ERROR_REPORT(&device->common,
				ADI_COMMON_ERRSRC_API,
				ADI_COMMON_ERR_INV_PARAM,
				ADI_COMMON_ACT_ERR_CHECK_PARAM,
				ssiTestModeConfig->testData,
				"Nibble ramp pattern is not supported in LVDS");
			ADI_ERROR_RETURN(device->common.error.newAction);
		}

		ADI_RANGE_CHECK(device, dataFormat, SSI_FORMAT_12_BIT_I_Q_DATA, SSI_FORMAT_32_BIT_I_Q_DATA_22_BIT_GAIN_INDEX_AND_CHANGE);
	}

	if ((ADI_ADRV910X_SSI_TESTMODE_DATA_FIXED_PATTERN != ssiTestModeConfig->testData) &&
	    (dataFormat != SSI_FORMAT_16_BIT_I_Q_DATA) &&
	    (dataFormat != SSI_FORMAT_16_BIT_I_Q_DATA_15_BIT_GAIN_CHANGE_ONLY) &&
	    (dataFormat != SSI_FORMAT_32_BIT_I_Q_DATA_22_BIT_GAIN_INDEX_AND_CHANGE))
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
#define ADI_ADRV910X_SSI_DEBUG_TIMEOUT_US 250000
#define ADI_ADRV910X_SSI_DEBUG_INTERVAL_US 25000

int32_t adi_adrv910x_Ssi_Tx_TestMode_Status_Inspect(adi_adrv910x_Device_t *device,
	adi_common_ChannelNumber_e channel,
	ssiType_e ssiType,
	ssiDataFormat_e dataFormat,
	adi_adrv910x_TxSsiTestModeCfg_t *ssiTestModeConfig,
	adi_adrv910x_TxSsiTestModeStatus_t *ssiTestModeStatus)
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
	uint32_t timeout_us = ADI_ADRV910X_SSI_DEBUG_TIMEOUT_US;
	uint8_t ddrSet = 0;

	waitInterval_us = (ADI_ADRV910X_SSI_DEBUG_INTERVAL_US > timeout_us) ?
	timeout_us : ADI_ADRV910X_SSI_DEBUG_INTERVAL_US;
	numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

	ADI_PERFORM_VALIDATION(adi_adrv910x_Ssi_Tx_TestMode_Status_Inspect_Validate, device, channel, ssiType, dataFormat, ssiTestModeConfig, ssiTestModeStatus);
    ADI_MUTEX_AQUIRE(device);
	if (SSI_TYPE_CMOS == ssiType)
	{
		if (ADI_ADRV910X_SSI_TESTMODE_DATA_FIXED_PATTERN == ssiTestModeConfig->testData)
		{
			for (nibSel = 0; nibSel < 4; nibSel++)
			{
				ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugMode_Set, device, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugStartRamp_Set, device, 0x0);
				ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugQSel_Set, device, 0); // 0: I_data; 1: Q_data
				ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugNibbleSel_Set, device, nibSel);
				ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, 0x0);
				ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, 0x0);
				ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, 0x0);
				ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, 0x0);
				ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugStartCapture_Set, device, 0x1);
				/* Wait for capture to complete */
				for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
				{
					ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugStartCapture_Get, device, &captureComplete);
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
						ADI_MUTEX_ERROR_RELEASE(device);
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
						ADI_ADRV910X_SRC_ARMCMD,
						ADI_COMMON_ERR_API_FAIL,
						ADI_ADRV910X_ACT_ERR_RESET_ARM,
						device,
						"CssiTxDebugCapture failed due to time out ");
					ADI_MUTEX_ERROR_RELEASE(device);
					ADI_ERROR_RETURN(device->common.error.newAction);
				}
				ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugCapturedData_Get, device, (uint8_t *)(&dataRead));
				iqValue[0] |= ((dataRead & 0xF) << 4 * nibSel);
				dataRead = 0;
			}
			if ((dataFormat == SSI_FORMAT_12_BIT_I_Q_DATA) || 
			    (dataFormat == SSI_FORMAT_16_BIT_I_Q_DATA) ||
			    (dataFormat == SSI_FORMAT_16_BIT_I_Q_DATA_15_BIT_GAIN_CHANGE_ONLY) ||
			    (dataFormat == SSI_FORMAT_32_BIT_I_Q_DATA_22_BIT_GAIN_INDEX_AND_CHANGE))
			{
				for (nibSel = 0; nibSel < 4; nibSel++)
				{
					ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugMode_Set, device, 0x1);
					ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugStartRamp_Set, device, 0x0);
					ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugQSel_Set, device, 1); // 0: I_data; 1: Q_data
					ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugNibbleSel_Set, device, nibSel);
					ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, 0x0);
					ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, 0x1);
					ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearStrobeAlignError_Set, device, 0x0);
					ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, 0x0);
					ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, 0x1);
					ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxClearDdrStrobeAlignError_Set, device, 0x0);
					ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugStartCapture_Set, device, 0x1);
					/* Wait for capture to complete */
					for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
					{
						ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugStartCapture_Get, device, &captureComplete);
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
							ADI_MUTEX_ERROR_RELEASE(device);
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
							ADI_ADRV910X_SRC_ARMCMD,
							ADI_COMMON_ERR_API_FAIL,
							ADI_ADRV910X_ACT_ERR_RESET_ARM,
							device,
							"CssiTxDebugCapture failed due to time out ");
						ADI_MUTEX_ERROR_RELEASE(device);
						ADI_ERROR_RETURN(device->common.error.newAction);
					}
					ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugCapturedData_Get, device, (uint8_t *)(&dataRead));
					iqValue[1] |= ((dataRead & 0xF) << 4 * nibSel);
					dataRead = 0;
				}
			}
			ssiTestModeStatus->dataError = 0;
			switch (dataFormat)
			{
			case SSI_FORMAT_2_BIT_SYMBOL_DATA:
				/* Only 2 LSB */
				if (iqValue[0] != ((ssiTestModeConfig->fixedDataPatternToCheck >> 16) & 0x0003))
				{
					ssiTestModeStatus->dataError = 1;
				}
				break;
			case SSI_FORMAT_8_BIT_SYMBOL_DATA:
				/* Only 8 LSB */
				if (iqValue[0] != ((ssiTestModeConfig->fixedDataPatternToCheck >> 16) & 0x00FF))
				{
					ssiTestModeStatus->dataError = 1;
				}
				break;
			case SSI_FORMAT_16_BIT_SYMBOL_DATA:
				/* Only 16 bit I data */
				if (iqValue[0] != ((ssiTestModeConfig->fixedDataPatternToCheck >> 16) & 0xFFFF))
				{
					ssiTestModeStatus->dataError = 1;
				}
				break;
			case SSI_FORMAT_16_BIT_I_Q_DATA:
			case SSI_FORMAT_16_BIT_I_Q_DATA_15_BIT_GAIN_CHANGE_ONLY:
			case SSI_FORMAT_32_BIT_I_Q_DATA_22_BIT_GAIN_INDEX_AND_CHANGE:
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
			ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDebugRampShiftError_Get, device, &ssiTestModeStatus->dataError);
		}

		ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDdrSel_Get, device, &ddrSet);
		if (ddrSet == 0)
		{
			ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxStrobeAlignError_Get, device, &ssiTestModeStatus->strobeAlignError);
		}
		else
		{
			ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxDdrStrobeAlignError_Get, device, &ssiTestModeStatus->strobeAlignError);
		}

		ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxFifoFull_Get, device, &ssiTestModeStatus->fifoFull);

		ADI_EXPECT(adrv910x_NvsRegmapTx_CssiTxFifoEmpty_Get, device, &ssiTestModeStatus->fifoEmpty);
	}
	else /* LVDS */
	{
		if ((ADI_ADRV910X_SSI_TESTMODE_DATA_RAMP_16_BIT == ssiTestModeConfig->testData) ||
		    (ADI_ADRV910X_SSI_TESTMODE_DATA_PRBS15 == ssiTestModeConfig->testData))
		{
			ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugPrbs15RampShiftErrorCount_Get, device, &ssiTestModeStatus->dataError);
		}
		else if (ADI_ADRV910X_SSI_TESTMODE_DATA_PRBS7 == ssiTestModeConfig->testData)
		{
			ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugPrbs7ErrorCount_Get, device, &ssiTestModeStatus->dataError);
		}
		else /* Fixed Pattern */
		{
			for (i = 0; i < 2; i++)
			{
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugMode_Set, device, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugStartRamp_Set, device, 0x0);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugQSel_Set, device, i); // 0: I_data; 1: Q_data
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxClearStrobeAlignError_Set, device, 0x0);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxClearStrobeAlignError_Set, device, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxClearStrobeAlignError_Set, device, 0x0);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxClearEarlyStrobeDetectedFlag_Set, device, 0x0);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxClearEarlyStrobeDetectedFlag_Set, device, 0x1);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxClearEarlyStrobeDetectedFlag_Set, device, 0x0);
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugStartCapture_Set, device, 0x1);
				/* Wait for capture to complete */
				for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
				{
					ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugStartCapture_Get, device, &captureComplete);
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
						ADI_MUTEX_ERROR_RELEASE(device);
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
						ADI_ADRV910X_SRC_ARMCMD,
						ADI_COMMON_ERR_API_FAIL,
						ADI_ADRV910X_ACT_ERR_RESET_ARM,
						device,
						"LssiTxDebugCapture failed due to time out ");
					ADI_MUTEX_ERROR_RELEASE(device);
					ADI_ERROR_RETURN(device->common.error.newAction);
				}
				ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxDebugCapturedData_Get, device, &dataRead);
				iqValue[i] = dataRead;
			}

			ssiTestModeStatus->dataError = 0;
			switch (dataFormat)
			{
			case SSI_FORMAT_12_BIT_I_Q_DATA:
				/* Only 12 bit I and Q data */
				if ((iqValue[0] != ((ssiTestModeConfig->fixedDataPatternToCheck >> 16) & 0x0FFF)) ||
				    (iqValue[1] != (ssiTestModeConfig->fixedDataPatternToCheck & 0x0FFF)))
				{
					ssiTestModeStatus->dataError = 1;
				}
				break;
			case SSI_FORMAT_16_BIT_I_Q_DATA:
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

		ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxStrobeAlignError_Get, device, &ssiTestModeStatus->strobeAlignError);

		ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxFifoFull_Get, device, &ssiTestModeStatus->fifoFull);

		ADI_EXPECT(adrv910x_NvsRegmapTx_LssiTxFifoEmpty_Get, device, &ssiTestModeStatus->fifoEmpty);
	}

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Ssi_Loopback_Set_Validate(adi_adrv910x_Device_t *device,
	adi_common_ChannelNumber_e channel,
	const deviceProfile_t * profile)
{
	ADI_NULL_PTR_RETURN(&device->common, profile);
	ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Ssi_Loopback_Set(adi_adrv910x_Device_t *device,
	adi_common_ChannelNumber_e channel,
	const deviceProfile_t *profile,
	bool loopbackEnable)
{
	adrv910x_BfNvsRegmapRxCommon_e instance = ADRV910X_BF_RX1_CORE_2;
	uint8_t channelIdx = 0;

	ADI_PERFORM_VALIDATION(adi_adrv910x_Ssi_Loopback_Set_Validate, device, channel, profile);

	adi_common_channel_to_index(channel, &channelIdx);
	instance = nvsRegmapRxCommonInstances[channelIdx];
    ADI_MUTEX_AQUIRE(device);
	if (loopbackEnable)
	{
		ADI_EXPECT(adrv910x_NvsRegmapRxCommon_SsiDataSourceSel_Set, device, instance, SSI_TX_LPBK);
	}
	else
	{
		uint8_t source = profile->rxConfig[channelIdx].rxSsiConfig.rxSsiDataSrc;
		ADI_EXPECT(adrv910x_NvsRegmapRxCommon_SsiDataSourceSel_Set, device, instance, source);
	}
	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv910x_Ssi_LvdsDelayConfigSet(adi_adrv910x_Device_t *device,
	adi_adrv910x_SsiCalibrationCfg_t *ssiCalibrationCfg)
{
	ADI_MUTEX_AQUIRE(device);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Rx1ClkLvdsDelay_Set, device, ssiCalibrationCfg->rxClkDelay[0]);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Rx1StrobeLvdsDelay_Set, device, ssiCalibrationCfg->rxStrobeDelay[0]);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Rx1IdataLvdsDelay_Set, device, ssiCalibrationCfg->rxIDataDelay[0]);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Rx1QdataLvdsDelay_Set, device, ssiCalibrationCfg->rxQDataDelay[0]);

	ADI_EXPECT(adrv910x_NvsRegmapCore_Rx2ClkLvdsDelay_Set, device, ssiCalibrationCfg->rxClkDelay[1]);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Rx2StrobeLvdsDelay_Set, device, ssiCalibrationCfg->rxStrobeDelay[1]);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Rx2IdataLvdsDelay_Set, device, ssiCalibrationCfg->rxIDataDelay[1]);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Rx2QdataLvdsDelay_Set, device, ssiCalibrationCfg->rxQDataDelay[1]);

	ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1ClkLvdsDelay_Set, device, ssiCalibrationCfg->txClkDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1RefclkLvdsDelay_Set, device, ssiCalibrationCfg->txRefClkDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1StrobeLvdsDelay_Set, device, ssiCalibrationCfg->txStrobeDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1IdataLvdsDelay_Set, device, ssiCalibrationCfg->txIDataDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1QdataLvdsDelay_Set, device, ssiCalibrationCfg->txQDataDelay);

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv910x_Ssi_CmosDelayConfigSet(adi_adrv910x_Device_t *device,
	adi_adrv910x_SsiCalibrationCfg_t *ssiCalibrationCfg)
{
    ADI_MUTEX_AQUIRE(device);
	/* Rx1 */
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx1ClkCmosNDelay_Set, device, ssiCalibrationCfg->rxClkDelay[0]);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx1ClkCmosPDelay_Set, device, ssiCalibrationCfg->rxClkDelay[0]);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx1StrobeCmosNDelay_Set, device, ssiCalibrationCfg->rxStrobeDelay[0]);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx1StrobeCmosPDelay_Set, device, ssiCalibrationCfg->rxStrobeDelay[0]);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx1IdataCmosNDelay_Set, device, ssiCalibrationCfg->rxIDataDelay[0]);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx1IdataCmosPDelay_Set, device, ssiCalibrationCfg->rxIDataDelay[0]);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx1QdataCmosNDelay_Set, device, ssiCalibrationCfg->rxQDataDelay[0]);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx1QdataCmosPDelay_Set, device, ssiCalibrationCfg->rxQDataDelay[0]);

	/* Rx2 */
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx2ClkCmosNDelay_Set, device, ssiCalibrationCfg->rxClkDelay[1]);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx2ClkCmosPDelay_Set, device, ssiCalibrationCfg->rxClkDelay[1]);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx2StrobeCmosNDelay_Set, device, ssiCalibrationCfg->rxStrobeDelay[1]);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx2StrobeCmosPDelay_Set, device, ssiCalibrationCfg->rxStrobeDelay[1]);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx2IdataCmosNDelay_Set, device, ssiCalibrationCfg->rxIDataDelay[1]);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx2IdataCmosPDelay_Set, device, ssiCalibrationCfg->rxIDataDelay[1]);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx2QdataCmosNDelay_Set, device, ssiCalibrationCfg->rxQDataDelay[1]);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx2QdataCmosPDelay_Set, device, ssiCalibrationCfg->rxQDataDelay[1]);

	/* Tx1 */
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1ClkCmosNDelay_Set, device, ssiCalibrationCfg->txClkDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1ClkCmosPDelay_Set, device, ssiCalibrationCfg->txClkDelay);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1RefclkCmosNDelay_Set, device, ssiCalibrationCfg->txRefClkDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1RefclkCmosPDelay_Set, device, ssiCalibrationCfg->txRefClkDelay);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1StrobeCmosNDelay_Set, device, ssiCalibrationCfg->txStrobeDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1StrobeCmosPDelay_Set, device, ssiCalibrationCfg->txStrobeDelay);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1IdataCmosNDelay_Set, device, ssiCalibrationCfg->txIDataDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1IdataCmosPDelay_Set, device, ssiCalibrationCfg->txIDataDelay);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1QdataCmosNDelay_Set, device, ssiCalibrationCfg->txQDataDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1QdataCmosPDelay_Set, device, ssiCalibrationCfg->txQDataDelay);

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Ssi_Delay_Configure_Validate(adi_adrv910x_Device_t *device,
	ssiType_e ssiType,
	adi_adrv910x_SsiCalibrationCfg_t *ssiCalibration)
{
	static const uint8_t MAX_DELAY = 7;
	ADI_RANGE_CHECK(device, ssiType, SSI_TYPE_CMOS, SSI_TYPE_LVDS);

	ADI_NULL_PTR_RETURN(&device->common, ssiCalibration);

	ADI_RANGE_CHECK(device, ssiCalibration->rxClkDelay[0], 0, MAX_DELAY);
	ADI_RANGE_CHECK(device, ssiCalibration->rxClkDelay[1], 0, MAX_DELAY);
	ADI_RANGE_CHECK(device, ssiCalibration->rxStrobeDelay[0], 0, MAX_DELAY);
	ADI_RANGE_CHECK(device, ssiCalibration->rxStrobeDelay[1], 0, MAX_DELAY);
	ADI_RANGE_CHECK(device, ssiCalibration->rxIDataDelay[0], 0, MAX_DELAY);
	ADI_RANGE_CHECK(device, ssiCalibration->rxIDataDelay[1], 0, MAX_DELAY);
	ADI_RANGE_CHECK(device, ssiCalibration->rxQDataDelay[0], 0, MAX_DELAY);
	ADI_RANGE_CHECK(device, ssiCalibration->rxQDataDelay[1], 0, MAX_DELAY);

	ADI_RANGE_CHECK(device, ssiCalibration->txClkDelay, 0, MAX_DELAY);
	ADI_RANGE_CHECK(device, ssiCalibration->txRefClkDelay, 0, MAX_DELAY);
	ADI_RANGE_CHECK(device, ssiCalibration->txStrobeDelay, 0, MAX_DELAY);
	ADI_RANGE_CHECK(device, ssiCalibration->txIDataDelay, 0, MAX_DELAY);
	ADI_RANGE_CHECK(device, ssiCalibration->txQDataDelay, 0, MAX_DELAY);

	ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Ssi_Delay_Configure(adi_adrv910x_Device_t *device,
	ssiType_e ssiType,
	adi_adrv910x_SsiCalibrationCfg_t *ssiCalibration)
{
	ADI_PERFORM_VALIDATION(adi_adrv910x_Ssi_Delay_Configure_Validate, device, ssiType, ssiCalibration);

    ADI_MUTEX_AQUIRE(device);
	if (SSI_TYPE_LVDS == ssiType)
	{
		ADI_EXPECT(adi_adrv910x_Ssi_LvdsDelayConfigSet, device, ssiCalibration);
	}
	else
	{
		ADI_EXPECT(adi_adrv910x_Ssi_CmosDelayConfigSet, device, ssiCalibration);
	}

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv910x_Ssi_LvdsDelayConfigGet(adi_adrv910x_Device_t *device,
	adi_adrv910x_SsiCalibrationCfg_t *ssiCalibrationCfg)
{
    ADI_MUTEX_AQUIRE(device);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Rx1ClkLvdsDelay_Get, device, &ssiCalibrationCfg->rxClkDelay[0]);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Rx1StrobeLvdsDelay_Get, device, &ssiCalibrationCfg->rxStrobeDelay[0]);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Rx1IdataLvdsDelay_Get, device, &ssiCalibrationCfg->rxIDataDelay[0]);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Rx1QdataLvdsDelay_Get, device, &ssiCalibrationCfg->rxQDataDelay[0]);

	ADI_EXPECT(adrv910x_NvsRegmapCore_Rx2ClkLvdsDelay_Get, device, &ssiCalibrationCfg->rxClkDelay[1]);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Rx2StrobeLvdsDelay_Get, device, &ssiCalibrationCfg->rxStrobeDelay[1]);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Rx2IdataLvdsDelay_Get, device, &ssiCalibrationCfg->rxIDataDelay[1]);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Rx2QdataLvdsDelay_Get, device, &ssiCalibrationCfg->rxQDataDelay[1]);

	ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1ClkLvdsDelay_Get, device, &ssiCalibrationCfg->txClkDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1RefclkLvdsDelay_Get, device, &ssiCalibrationCfg->txRefClkDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1StrobeLvdsDelay_Get, device, &ssiCalibrationCfg->txStrobeDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1IdataLvdsDelay_Get, device, &ssiCalibrationCfg->txIDataDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1QdataLvdsDelay_Get, device, &ssiCalibrationCfg->txQDataDelay);

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv910x_Ssi_CmosDelayConfigGet(adi_adrv910x_Device_t *device,
	adi_adrv910x_SsiCalibrationCfg_t *ssiCalibrationCfg)
{
	ADI_MUTEX_AQUIRE(device);
	/* Rx1 */
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx1ClkCmosNDelay_Get, device, &ssiCalibrationCfg->rxClkDelay[0]);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx1ClkCmosPDelay_Get, device, &ssiCalibrationCfg->rxClkDelay[0]);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx1StrobeCmosNDelay_Get, device, &ssiCalibrationCfg->rxStrobeDelay[0]);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx1StrobeCmosPDelay_Get, device, &ssiCalibrationCfg->rxStrobeDelay[0]);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx1IdataCmosNDelay_Get, device, &ssiCalibrationCfg->rxIDataDelay[0]);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx1IdataCmosPDelay_Get, device, &ssiCalibrationCfg->rxIDataDelay[0]);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx1QdataCmosNDelay_Get, device, &ssiCalibrationCfg->rxQDataDelay[0]);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx1QdataCmosPDelay_Get, device, &ssiCalibrationCfg->rxQDataDelay[0]);

	/* Rx2 */
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx2ClkCmosNDelay_Get, device, &ssiCalibrationCfg->rxClkDelay[1]);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx2ClkCmosPDelay_Get, device, &ssiCalibrationCfg->rxClkDelay[1]);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx2StrobeCmosNDelay_Get, device, &ssiCalibrationCfg->rxStrobeDelay[1]);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx2StrobeCmosPDelay_Get, device, &ssiCalibrationCfg->rxStrobeDelay[1]);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx2IdataCmosNDelay_Get, device, &ssiCalibrationCfg->rxIDataDelay[1]);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx2IdataCmosPDelay_Get, device, &ssiCalibrationCfg->rxIDataDelay[1]);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx2QdataCmosNDelay_Get, device, &ssiCalibrationCfg->rxQDataDelay[1]);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Rx2QdataCmosPDelay_Get, device, &ssiCalibrationCfg->rxQDataDelay[1]);

	/* Tx1 */
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1ClkCmosNDelay_Get, device, &ssiCalibrationCfg->txClkDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1ClkCmosPDelay_Get, device, &ssiCalibrationCfg->txClkDelay);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1RefclkCmosNDelay_Get, device, &ssiCalibrationCfg->txRefClkDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1RefclkCmosPDelay_Get, device, &ssiCalibrationCfg->txRefClkDelay);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1StrobeCmosNDelay_Get, device, &ssiCalibrationCfg->txStrobeDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1StrobeCmosPDelay_Get, device, &ssiCalibrationCfg->txStrobeDelay);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1IdataCmosNDelay_Get, device, &ssiCalibrationCfg->txIDataDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1IdataCmosPDelay_Get, device, &ssiCalibrationCfg->txIDataDelay);

	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1QdataCmosNDelay_Get, device, &ssiCalibrationCfg->txQDataDelay);
	ADI_EXPECT(adrv910x_NvsRegmapCore3_Tx1QdataCmosPDelay_Get, device, &ssiCalibrationCfg->txQDataDelay);

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Ssi_Delay_Inspect_Validate(adi_adrv910x_Device_t *device,
	ssiType_e ssiType,
	adi_adrv910x_SsiCalibrationCfg_t *ssiCalibration)
{
	ADI_RANGE_CHECK(device, ssiType, SSI_TYPE_CMOS, SSI_TYPE_LVDS);
	ADI_NULL_PTR_RETURN(&device->common, ssiCalibration);
	ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Ssi_Delay_Inspect(adi_adrv910x_Device_t *device,
	ssiType_e ssiType,
	adi_adrv910x_SsiCalibrationCfg_t *ssiCalibration)
{
	ADI_PERFORM_VALIDATION(adi_adrv910x_Ssi_Delay_Inspect_Validate, device, ssiType, ssiCalibration);
    ADI_MUTEX_AQUIRE(device);
	if (SSI_TYPE_LVDS == ssiType)
	{
		ADI_EXPECT(adi_adrv910x_Ssi_LvdsDelayConfigGet, device, ssiCalibration);
	}
	else
	{
		ADI_EXPECT(adi_adrv910x_Ssi_CmosDelayConfigGet, device, ssiCalibration);
	}

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Ssi_Tx_Disable_LSSI_Pads_Validate(adi_adrv910x_Device_t *device,
	adi_common_ChannelNumber_e channel,
	ssiType_e ssiType)
{
	ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
	ADI_RANGE_CHECK(device, ssiType, SSI_TYPE_CMOS, SSI_TYPE_LVDS);

	ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Ssi_Tx_Disable_LSSI_Pads(adi_adrv910x_Device_t *device,
	adi_common_ChannelNumber_e channel,
	ssiType_e ssiType)
{
	ADI_PERFORM_VALIDATION(adi_adrv910x_Ssi_Tx_Disable_LSSI_Pads_Validate, device, channel, ssiType);
    ADI_MUTEX_AQUIRE(device);
	if (SSI_TYPE_LVDS == ssiType)
	{
		if (channel == ADI_CHANNEL_1)
		{
			/* disable pads */
			ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1StrobeLvdsIe_Set, device, 0); 
			ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1ClkLvdsIe_Set, device, 0);
			ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1IdataLvdsIe_Set, device, 0);   
			ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1QdataLvdsIe_Set, device, 0);
		}
	}
	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Ssi_Tx_Enable_LSSI_Pads_Validate(adi_adrv910x_Device_t *device,
	adi_common_ChannelNumber_e channel,
	ssiType_e ssiType)
{
	ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
	ADI_RANGE_CHECK(device, ssiType, SSI_TYPE_CMOS, SSI_TYPE_LVDS);

	ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Ssi_Tx_Enable_LSSI_Pads(adi_adrv910x_Device_t *device,
	adi_common_ChannelNumber_e channel,
	ssiType_e ssiType)
{
	ADI_PERFORM_VALIDATION(adi_adrv910x_Ssi_Tx_Enable_LSSI_Pads_Validate, device, channel, ssiType);
    ADI_MUTEX_AQUIRE(device);
	if (SSI_TYPE_LVDS == ssiType)
	{
		if (channel == ADI_CHANNEL_1)
		{
			/* enable pads */ 
			ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1IdataLvdsIe_Set, device, 1);
			ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1QdataLvdsIe_Set, device, 1);
			ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1ClkLvdsIe_Set, device, 1);
			ADI_EXPECT(adrv910x_NvsRegmapCore_Tx1StrobeLvdsIe_Set, device, 1);
		}
	}
	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}
