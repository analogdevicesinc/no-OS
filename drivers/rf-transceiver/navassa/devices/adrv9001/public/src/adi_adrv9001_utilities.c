/**
* \file
* \brief Contains Utility features related function implementation defined in
* adi_adrv9001_utilities.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/slab.h>
#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif

#include "adi_common_macros.h"
#include "adi_adrv9001_user.h"
#include "adi_adrv9001_utilities.h"
#include "adi_adrv9001_hal.h"
#include "adi_adrv9001_rx.h"
#include "adi_adrv9001_orx_types.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_tx.h"
#include "adi_adrv9001_stream.h"
#include "adi_adrv9001_dpd.h"

#include "adi_adrv9001_fh.h"

#include "adi_adrv9001_gpio.h"
#include "adi_adrv9001_ssi.h"

#include "adrv9001_reg_addr_macros.h"
#include "adrv9001_bf_hal.h"
#include "adrv9001_bf.h"

#ifdef __KERNEL__
#define printf(...)     pr_info(__VA_ARGS__)
#endif

#define ADI_ADRV9001_ARM_BINARY_IMAGE_FILE_SIZE_BYTES (303*1024)
#define ADI_ADRV9001_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES (32*1024)

#define ADI_ADRV9001_RX_GAIN_TABLE_SIZE_ROWS 256
#define ADI_ADRV9001_TX_ATTEN_TABLE_SIZE_ROWS 1024

int32_t adi_adrv9001_Utilities_ArmImage_Load(adi_adrv9001_Device_t *device, const char *armImagePath, adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i = 0;
#ifndef __KERNEL__
    uint8_t armBinaryImageBuffer[ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];
#else
    /*
     * linux stack is not that big which means we need to be carefull. Some archs like arm set
     * Wframe-larger-than=1024
     */
     static uint8_t armBinaryImageBuffer[ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];
#endif

    /* Check device pointer is not null */
    ADI_ENTRY_EXPECT(device);
    ADI_RANGE_CHECK(device, spiWriteMode, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4);

    /*Read ARM binary file*/
    for (i = 0; i < (ADI_ADRV9001_ARM_BINARY_IMAGE_FILE_SIZE_BYTES/ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES); i++)
    {
        if (adi_hal_ArmImagePageGet(device->common.devHalInfo, armImagePath, i, ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES, armBinaryImageBuffer))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Fatal error while reading ARM binary file. Possible memory shortage");
        }

        /*Write the ARM binary chunk*/
        if ((recoveryAction = adi_adrv9001_arm_Image_Write(device, (i*ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES), &armBinaryImageBuffer[0],
            ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES, spiWriteMode)) != ADI_COMMON_ACT_NO_ACTION)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_RESET_FULL,
                             NULL,
                             "Fatal error while writing ARM binary file");
        }
    }

    return recoveryAction;
}

int32_t adi_adrv9001_Utilities_StreamImage_Load(adi_adrv9001_Device_t *device, const char *streamImagePath, adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode)
{

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i = 0;
#ifndef __KERNEL__
    uint8_t streamBinaryImageBuffer[ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];
#else
    /*
     * linux stack is not that big which means we need to be carefull. Some archs like arm set
     * Wframe-larger-than=1024
     */
    static uint8_t streamBinaryImageBuffer[ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];
#endif

    /* Check device pointer is not null */
    ADI_ENTRY_EXPECT(device);
    ADI_RANGE_CHECK(device, spiWriteMode, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4);

    /*Read stream binary file*/
    for (i = 0; i < (ADI_ADRV9001_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES / ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES); i++)
    {
        if (adi_hal_StreamImagePageGet(device->common.devHalInfo, streamImagePath, i, ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES, streamBinaryImageBuffer))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Fatal error while reading stream binary file. Possible memory shortage");
        }

        /*Write the Stream binary chunk*/
        if ((recoveryAction = adi_adrv9001_Stream_Image_Write(device,
                                                              (i*ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES),
                                                              &streamBinaryImageBuffer[0],
                                                              ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES, spiWriteMode)) != ADI_COMMON_ACT_NO_ACTION)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_RESET_FULL,
                             NULL,
                             "Fatal error while writing stream binary file");
        }
    }

    return recoveryAction;
}

int32_t adi_adrv9001_Utilities_RxGainTable_Load(adi_adrv9001_Device_t *device, adi_common_Port_e port, const char *rxGainTablePath, adi_common_ChannelNumber_e channel, adi_adrv9001_RxLnaConfig_t *lnaConfig, adi_adrv9001_RxGainTableType_e gainTableType)
{
    uint8_t maxGainIndex = 0;
    uint8_t prevGainIndex = 0;
    uint8_t gainIndex = 0;
    uint8_t tiaControl = 0;
    uint8_t adcControl = 0;
    uint16_t lineCount = 0;

    static adi_adrv9001_RxGainTableRow_t rxGainTableRowBuffer[ADI_ADRV9001_RX_GAIN_TABLE_SIZE_ROWS];
    static const uint8_t NUM_COLUMNS = 7;

    int32_t returnTableEntry = NUM_COLUMNS;

    uint8_t minIndex = 0;
    adrv9001_BfNvsRegmapRxb_e instances[] = { ADRV9001_BF_RXB1_CORE, ADRV9001_BF_RXB2_CORE };
    adrv9001_BfNvsRegmapRxb_e instance = ADRV9001_BF_RXB1_CORE;
    uint8_t instanceIdx = 0;

    /* Check device pointer is not null */
    ADI_ENTRY_PTR_EXPECT(device, rxGainTablePath);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = instances[instanceIdx];
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcMinimumGainIndex_Get, device, instance, &minIndex);

    /*Loop until the gain table end is reached or no. of lines scanned exceeds maximum*/
    while (lineCount <  ADI_ADRV9001_RX_GAIN_TABLE_SIZE_ROWS)
    {
        returnTableEntry = adi_hal_RxGainTableEntryGet(device->common.devHalInfo,
                                                       rxGainTablePath,
                                                       lineCount,
                                                       &gainIndex,
                                                       &rxGainTableRowBuffer[lineCount].rxFeGain,
                                                       &tiaControl,
                                                       &adcControl,
                                                       &rxGainTableRowBuffer[lineCount].extControl,
                                                       &rxGainTableRowBuffer[lineCount].phaseOffset,
                                                       &rxGainTableRowBuffer[lineCount].digGain);
        if (returnTableEntry != NUM_COLUMNS)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             gainIndex,
                             "Insufficient entries in Rx gain table row entry");
        }

        rxGainTableRowBuffer[lineCount].adcTiaGain = ((adcControl << 1) | tiaControl);

        if (gainIndex < minIndex)
        {
            if ((gainIndex < ADI_ADRV9001_ORX_GAIN_INDEX_MIN) ||
                (gainIndex > ADI_ADRV9001_ORX_GAIN_INDEX_MAX))
            {
                break;
            }
        }

        /*Check that gain indices are arranged in ascending order*/
        if ((lineCount > 0)
         && (prevGainIndex != (gainIndex - 1)))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             gainIndex,
                             "Gain indices not arranged in ascending order in Rx Gain Table file");
        }

        prevGainIndex = gainIndex;
        lineCount++;
    }

    maxGainIndex = prevGainIndex;
    ADI_EXPECT(adi_adrv9001_Rx_GainTable_Write, device, port, channel, maxGainIndex, &rxGainTableRowBuffer[0], lineCount, lnaConfig, gainTableType);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Utilities_TxAttenTable_Load(adi_adrv9001_Device_t *device, const char *txAttenTablePath, uint32_t txChannelMask)
{
    uint16_t prevAttenIndex = 0;
    uint16_t attenIndex = 0;
    uint16_t minAttenIndex = 0;
    uint16_t lineCount = 0;
    uint16_t tableSize = 0;

    static adi_adrv9001_TxAttenTableRow_t txAttenTableRowBuffer[ADI_ADRV9001_TX_ATTEN_TABLE_SIZE_ROWS];
    static const uint8_t NUM_COLUMNS = 3;

    int32_t returnTableEntry = NUM_COLUMNS;

    /* Check device pointer is not null */
    ADI_ENTRY_PTR_EXPECT(device, txAttenTablePath);

    /*Loop until the atten table end is reached or no. of lines scanned exceeds maximum*/
    while (lineCount < ADRV9001_TX_ATTEN_TABLE_MAX)
    {
        returnTableEntry = adi_hal_TxAttenTableEntryGet(device->common.devHalInfo,
                                                        txAttenTablePath,
                                                        lineCount,
                                                        &attenIndex,
                                                        &txAttenTableRowBuffer[lineCount].txAttenHp,
                                                        &txAttenTableRowBuffer[lineCount].txAttenMult);

        if (returnTableEntry != NUM_COLUMNS)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                gainIndex,
                "Insufficient entries in Tx atten table row entry");
        }

        if (0 == txAttenTableRowBuffer[lineCount].txAttenMult)
        {
            break;
        }

        if (lineCount == 0)
        {
            minAttenIndex = attenIndex;
        }
        else
        {
            /*Check that atten indices are arranged in ascending order*/
            if (prevAttenIndex != (attenIndex - 1))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 attenIndex,
                                 "Atten indices not arranged in ascending order in Tx Atten Table file");
            }
        }

        prevAttenIndex = attenIndex;
        lineCount++;
    }

    tableSize = attenIndex - minAttenIndex + 1;

    ADI_EXPECT(adi_adrv9001_Tx_AttenuationTable_Write, device, txChannelMask, minAttenIndex, &txAttenTableRowBuffer[0], tableSize);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Utilities_WaitMs(adi_adrv9001_Device_t *adrv9001, uint32_t waitInterval_ms)
{
	int32_t halError = 0;

	/* Check device pointer is not null */
	ADI_ENTRY_EXPECT(adrv9001);

	halError = adi_common_hal_Wait_us(&adrv9001->common, (1000*waitInterval_ms));

	ADI_ERROR_REPORT(&adrv9001->common,
		ADI_COMMON_ERRSRC_ADI_HAL,
		halError,
		ADI_COMMON_ACT_ERR_CHECK_TIMER,
		device,
		"Timer not working");

	return halError;
}

int32_t adi_adrv9001_Utilities_SystemDebugPreCalibrate(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_Init_t *init, const char *armImagePath, const char *streamImagePath, const char *armImageVer)
{
	int32_t status = 0;
	uint8_t gp1LdoResistorValue = 0;
	uint8_t devClkLdoRegisterValue = 0;
	uint8_t clkSynthLdoReg = 0;
	uint8_t lpClkSynthLdoReg = 0;
	uint32_t armStatusCheckTimeoutUs = 5000000;

	adi_common_ApiVersion_t apiVersion_0 = {
		.major = 0,
		.minor = 0,
		.patch = 0
	};

	adi_adrv9001_SpiSettings_t spiSettings = {
		.msbFirst = 1,
		.enSpiStreaming = 0,
		.autoIncAddrUp = 1,
		.fourWireMode = 1,
		.cmosPadDrvStrength = ADI_ADRV9001_CMOSPAD_DRV_STRONG
	};

	adi_adrv9001_StreamVersion_t StreamVersion = {
		.majorVer = 0,
		.minorVer = 0,
		.maintVer = 0,
		.buildVer = 0,
	};

	adi_adrv9001_ArmVersion_t armVersion = {
		.majorVer = 0,
		.minorVer = 0,
		.maintVer = 0,
		.armBuildType = 0,
	};

	uint8_t armImageVersion[3] = { 0 };

	adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode = ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_252;

	/* NULL pointer check */
	ADI_ENTRY_EXPECT(adrv9001);
	ADI_NULL_PTR_RETURN(adrv9001, init);
	ADI_NULL_PTR_RETURN(adrv9001, armImagePath);
	ADI_NULL_PTR_RETURN(adrv9001, streamImagePath);

	printf("*** ADRV9001 Pre-Calibrate System Debugging Started ***\r\n");

	printf("--> . Hardware Reset\r\n");
	ADI_MSG_EXPECT("Failed to reset device and set SPI config.", adi_adrv9001_HwReset, adrv9001);
	printf("      OK\r\n");

	printf("--> . API Version\r\n");
	ADI_MSG_EXPECT("Error fetching API Version.", adi_adrv9001_ApiVersion_Get, adrv9001, &apiVersion_0);
	printf("      OK - API Version = %u.%u.%u\r\n", apiVersion_0.major, apiVersion_0.minor, apiVersion_0.patch);

	printf("--> . Configure SPI\r\n");
	ADI_MSG_EXPECT("Problem Closing HW.", adi_adrv9001_HwClose, adrv9001);
	ADI_MSG_EXPECT("Problem Opening HW.", adi_adrv9001_HwOpen, adrv9001, &spiSettings);
	printf("      OK\r\n");

	printf("--> . Check SPI\r\n");
	ADI_MSG_EXPECT("SPI Verify failed.", adi_adrv9001_spi_Verify, adrv9001);
	printf("      OK\r\n");

	printf("--> . Check Power Supplies\r\n");
	status = adi_bf_hal_Register_Read(adrv9001, 0x01ac, &gp1LdoResistorValue);
	ADI_ERROR_RETURN(status);
	status = adi_bf_hal_Register_Read(adrv9001, ADRV9001_ADDR_DEV_CLK_LDO_BYTE1, &devClkLdoRegisterValue);
	ADI_ERROR_RETURN(status);
	if ((gp1LdoResistorValue & (1 << 4)) && (devClkLdoRegisterValue & (1 << 4)))
	{
		printf("      OK\r\n");
	}
	else
	{
		printf("      NOK - Check power supply is connected correctly\r\n");
		ADI_ERROR_RETURN(ADI_COMMON_ERR_API_FAIL);
	}

	ADI_MSG_EXPECT("Problem Init Analog.", adi_adrv9001_InitAnalog, adrv9001, init, ADI_ADRV9001_DEVICECLOCKDIVISOR_2);
	ADI_MSG_EXPECT("Problem AhbSpiBridge Enable.", adi_adrv9001_arm_AhbSpiBridge_Enable, adrv9001);

	printf("--> . Load Stream Processor Image\r\n");
	ADI_MSG_EXPECT("Stream Processor Image Load Error. Check Dev_Clk, SPI and Power Supply.", adi_adrv9001_Utilities_StreamImage_Load, adrv9001, streamImagePath, spiWriteMode);
	printf("      OK\r\n");

	printf("--> . Verify Stream Processor Image Load\r\n");
	ADI_MSG_EXPECT("Stream Processor Image not loaded correctly. Check Dev_Clk, SPI and Power Supply.", adi_adrv9001_Stream_Version, adrv9001, &StreamVersion);
	printf("      OK -  Version = %u.%u.%u.%u\r\n", StreamVersion.majorVer, StreamVersion.minorVer, StreamVersion.maintVer, StreamVersion.buildVer);

	printf("--> . Load ARM Image\r\n");
	ADI_MSG_EXPECT("ARM Image Load Error. Check Dev_Clk, SPI and Power Supply.", adi_adrv9001_Utilities_ArmImage_Load, adrv9001, armImagePath, spiWriteMode);
	ADI_MSG_EXPECT("ARM Profile Write Error.", adi_adrv9001_arm_Profile_Write, adrv9001, init);
	ADI_MSG_EXPECT("ARM Pfir Profiles Write Error.", adi_adrv9001_arm_PfirProfiles_Write, adrv9001, init);
	ADI_MSG_EXPECT("ARM Start Error.", adi_adrv9001_arm_Start, adrv9001);
	adi_adrv9001_arm_System_Program(adrv9001, (uint8_t)(init->tx.txInitChannelMask | (init->rx.rxInitChannelMask & 0x33)));
	printf("      OK\r\n");

	printf("--> . Verify ARM Image Load\r\n");
	ADI_MSG_EXPECT("ARM Start Status Check Error.", adi_adrv9001_arm_StartStatus_Check, adrv9001, armStatusCheckTimeoutUs);
	ADI_MSG_EXPECT("ARM Check Firmware Version.", adi_adrv9001_arm_Version, adrv9001, &armVersion);
	armImageVersion[0] = (armImageVer[0] - '0');
	armImageVersion[1] = (armImageVer[2] - '0') * 10  + (armImageVer[3] - '0');
	armImageVersion[2] = (armImageVer[5] - '0') * 10  + (armImageVer[6] - '0');
	if ((armImageVersion[0] == armVersion.majorVer) && (armImageVersion[1] == armVersion.minorVer) && (armImageVersion[2] == armVersion.maintVer))
	{
		printf("      OK -  Version = %u.%u.%u.%u\r\n", armImageVersion[0], armImageVersion[1], armImageVersion[2], armVersion.armBuildType);
	}
	else
	{
		printf("      NOK - ARM Image Load verification failed\r\n");
		ADI_ERROR_RETURN(ADI_COMMON_ERR_API_FAIL);
	}

	printf("--> . Check Dev_Clk \r\n");
	status = adi_bf_hal_Register_Read(adrv9001, ADRV9001_ADDR_CLK_SYNTH_LDO_BYTE1, &clkSynthLdoReg);
	ADI_ERROR_RETURN(status);
	status = adi_bf_hal_Register_Read(adrv9001, ADRV9001_ADDR_CLK_SYNTH_LP_LDO_BYTE2, &lpClkSynthLdoReg);
	ADI_ERROR_RETURN(status);
	if ((clkSynthLdoReg & (1 << 4)) || (lpClkSynthLdoReg & (1 << 4)))
	{
		printf("      OK\r\n");
	}
	else
	{
		printf("      NOK - Check if device clock is connected correctly\r\n");
		ADI_ERROR_RETURN(ADI_COMMON_ERR_API_FAIL);
	}

	printf("*** ADRV9001 Pre-Calibrate System Debugging Completed ***\r\n");

	ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_Utilities_SystemDebugPostCalibrate(adi_adrv9001_Device_t *adrv9001)
{
	bool pllLO1LockStatus = 0;
	bool pllLO2LockStatus = 0;

	/* Check device pointer is not null */
	ADI_ENTRY_EXPECT(adrv9001);

	printf("*** ADRV9001 Post-Calibrate System Debugging Started ***\r\n");

	printf("--> . Check RF PLLs \r\n");
	ADI_MSG_EXPECT("RF Pll Error. Can't prime channel. The device should be calibrated", adi_adrv9001_Radio_Channel_ToState, adrv9001, ADI_RX, ADI_CHANNEL_1,
		       ADI_ADRV9001_CHANNEL_PRIMED);
	ADI_MSG_EXPECT("RF Pll Error. Can't prime channel. The device should be calibrated", adi_adrv9001_Radio_Channel_ToState, adrv9001, ADI_TX, ADI_CHANNEL_1,
		       ADI_ADRV9001_CHANNEL_PRIMED);

	ADI_MSG_EXPECT("Error fetching PLL LO1 lock status", adi_adrv9001_Radio_PllStatus_Get, adrv9001, ADI_ADRV9001_PLL_LO1, &pllLO1LockStatus);
	ADI_MSG_EXPECT("Error fetching PLL LO2 lock status", adi_adrv9001_Radio_PllStatus_Get, adrv9001, ADI_ADRV9001_PLL_LO2, &pllLO2LockStatus);

	if (pllLO1LockStatus | pllLO2LockStatus)
	{
		printf("      OK\r\n");
	}
	else
	{
		printf("      NOK - RF Plls can't lock\r\n");
		ADI_ERROR_RETURN(ADI_COMMON_ERR_API_FAIL);
	}

	printf("*** ADRV9001 Post-Calibrate System Debugging Completed ***\r\n");

	ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_Utilities_InitCals_WarmBoot_Coefficients_VectTblChunkRead(adi_adrv9001_Device_t *adrv9001,
																		       uint16_t calNo,
																			   uint32_t maskChannel1,
																			   uint32_t maskChannel2,
																			   uint32_t *addr,
																			   uint32_t *size,
																			   uint32_t *initMask,
																			   uint32_t *profMask)
{
	uint32_t address = 0x20020004;
	uint32_t vecTbl[4] = { 0 };

	/* Check device pointer is not null */
	ADI_ENTRY_EXPECT(adrv9001);

	ADI_EXPECT(adi_adrv9001_arm_Memory_Read32, adrv9001, (address + (16*calNo)), vecTbl, 16, 0);

	*addr = vecTbl[0];
	*size =  vecTbl[1];
	*initMask = vecTbl[2];
	*profMask = vecTbl[3];

	ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_Utilities_InitCals_WarmBoot_Coefficients_MaxArrayChunk_Get(adi_adrv9001_Device_t *adrv9001,
																				uint32_t maskChannel1,
																				uint32_t maskChannel2,
																				uint32_t addr,
																				uint32_t initMask,
																				uint32_t profMask,
																				uint8_t calVecTblData[],
																				uint32_t size)
{
	/* Check device pointer is not null */
	ADI_ENTRY_EXPECT(adrv9001);

	adi_common_ChannelNumber_e channel;
	for (channel = ADI_CHANNEL_1; channel <= ADI_CHANNEL_2; channel++)
	{
		uint32_t chInitMask;
		if (ADI_CHANNEL_1 == channel)
		{
			chInitMask = maskChannel1;
		}
		else
		{
			chInitMask = maskChannel2;
		}

		profMask = profMask >> (8 * (channel - 1));
		if (profMask == 0)
			continue;
		if (((initMask & chInitMask) != 0) && ((profMask & adrv9001->devStateInfo.chProfEnMask[channel - 1]) != 0))
		{
			ADI_EXPECT(adi_adrv9001_arm_Memory_Read, adrv9001, addr, &calVecTblData[0], size, 0);
		}
	}

	ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_Utilities_InitCals_WarmBoot_Coefficients_MaxArrayChunk_Set(adi_adrv9001_Device_t *adrv9001,
																				uint32_t maskChannel1,
																			    uint32_t maskChannel2,
																				uint32_t addr,
																				uint32_t initMask,
																				uint32_t profMask,
																				uint8_t calVecTblData[],
																				uint32_t size)
{
	/* Check device pointer is not null */
	ADI_ENTRY_EXPECT(adrv9001);

	adi_common_ChannelNumber_e channel;
	for (channel = ADI_CHANNEL_1; channel <= ADI_CHANNEL_2; channel++)
	{
		uint32_t chInitMask;
		if (ADI_CHANNEL_1 == channel)
		{
			chInitMask = maskChannel1;
		}
		else
		{
			chInitMask = maskChannel2;
		}

		profMask = profMask >> (8 * (channel - 1));
		if (profMask == 0)
			continue;
		if (((initMask & chInitMask) != 0) && ((profMask & adrv9001->devStateInfo.chProfEnMask[channel - 1]) != 0))
		{
			ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, addr, &calVecTblData[0], size, 0);
		}
	}

	ADI_API_RETURN(adrv9001);
}

