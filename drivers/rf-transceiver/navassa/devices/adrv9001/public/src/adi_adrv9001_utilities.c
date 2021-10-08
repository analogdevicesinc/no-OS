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

#define ADI_ADRV9001_ARM_BINARY_IMAGE_FILE_SIZE_BYTES (288*1024)

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
