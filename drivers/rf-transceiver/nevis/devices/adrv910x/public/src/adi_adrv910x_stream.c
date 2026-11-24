/**
* \file
* \brief Contains related function implementation defined in adi_adrv910x_stream.h
*
* ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv910x_user.h"
#include "adi_adrv910x_stream.h"
#include "adi_adrv910x_arm.h"
#include "adi_adrv910x.h"
#include "adi_adrv910x_error.h"
//#include "adi_adrv910x_gpio.h"
#include "adi_adrv910x_radio.h"
#include "adi_adrv910x_spi.h"

#include "adrv910x_arm.h"
#include "adrv910x_arm_macros.h"
#include "adrv910x_init.h"
#include "adrv910x_reg_addr_macros.h"
#include "object_ids.h"

#ifdef NEVIS_PS2
#include <adi_flash_api.h>
#include <adi_boot_api.h>
#endif /* NEVIS_PS2 */

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

static void adrv910x_BinaryImageInfoGet(adi_adrv910x_Device_t *device,
                                        const uint8_t binary[],
                                        uint16_t *binaryImageSize,
                                        uint16_t *binaryImageOffset)
{
    /* populating variables from binary array */
    *binaryImageSize = ((((uint16_t)binary[1]) << 8) | ((uint16_t)binary[0]));
    *binaryImageOffset = ((((uint16_t)binary[3]) << 8) | (uint16_t)(binary[2]));
}


static void adrv910x_BinaryParamsGetBaseAddr(adi_adrv910x_Device_t *device,
                                             const uint8_t binary[],
                                             uint32_t *word)
{
    /* populating variables from binary array */
    *word = (((uint32_t)binary[3]) << 24) | (((uint32_t)binary[2]) << 16) | (((uint32_t)binary[1]) << 8) | (uint32_t)(binary[0]);
}

static void adrv910x_BinaryParamsGetNumberStreamImageSize(adi_adrv910x_Device_t *device,
                                                          const uint8_t binary[],
                                                          uint8_t *numberStreams, uint32_t *imageSize)
{
    *numberStreams = binary[0];
    *imageSize = (((uint32_t)binary[3]) << 8) | (uint32_t)(binary[2]);
}

#ifndef NEVIS_PS2
int32_t adi_adrv910x_Stream_Image_Write(adi_adrv910x_Device_t *device,
                                      uint32_t byteOffset,
                                      uint8_t binary[],
                                      uint32_t byteCount,
                                      adi_adrv910x_ArmSingleSpiWriteMode_e spiWriteMode)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint16_t binaryImageSize = 0;
    uint16_t binaryImageOffset = 0;
    uint32_t i = 0;
    uint32_t addr = 0;
    uint32_t word1 = 0;
    uint32_t binOffset = 0;
    uint8_t  streamCtl = 0;
    uint8_t  byte1 = 0;
    uint32_t imageSize = 0;
    uint32_t imageOffset = 0;

    uint32_t streamAddr[] = {
        ADRV910X_ADDR_MAIN_STREAM_CTL_OFFSET,
        ADRV910X_ADDR_FLEX_SP0_STREAM_CTL_OFFSET,
        ADRV910X_ADDR_FLEX_SP1_STREAM_CTL_OFFSET,
        ADRV910X_ADDR_FLEX_SP2_STREAM_CTL_OFFSET,
        ADRV910X_ADDR_FLEX_SP3_STREAM_CTL_OFFSET
    };

    uint8_t streamControlDefaultVal[] = { 0xF0, 0xC0, 0xC0, 0xC0, 0xC0};

#ifdef ADRV910X_INIT_DEBUG
    static const char * imageName[] = { "MAIN_STREAM", "TX0_STREAM", "TX1_STREAM","RX0_STREAM","RX1_STREAM", };
#endif
    static const uint8_t MIN_BYTECOUNT = 68;
    static const uint8_t MAIN_IMAGE_OFFSET = 40;

    /* Arm stream download order: main, tx1/2, rx1/2 */
    static const uint32_t streamChannel[] = {
        0xFFFFFFFF,
        ADI_ADRV910X_TX1,
        ADI_ADRV910X_TXNB,
        ADI_ADRV910X_RX1 | ADI_ADRV910X_ORX1,
        ADI_ADRV910X_RXNB | ADI_ADRV910X_ORXNB
    };

    ADI_ENTRY_PTR_EXPECT(device, binary);
    ADI_RANGE_CHECK(device,
                    spiWriteMode,
                    ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4,
                    ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4);

    if ((byteCount % 4) > 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM,
            byteCount, "Invalid byteCount. Must be multiple of 4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((byteOffset % 4) > 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM,
            byteCount, "Invalid byteOffset. Must be multiple of 4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (byteOffset == 0)
    {
        if (byteCount < MIN_BYTECOUNT)
        {
            ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM,
                byteCount, "Invalid byteCount. Must be 68 or greater.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        for (i = 0; i < ADRV910X_MAX_NUM_STREAM; i++)
        {
            adrv910x_BinaryImageInfoGet(device, &binary[12 + i * 4], &binaryImageSize, &binaryImageOffset);
            device->devStateInfo.chunkStreamImageSize[i] = binaryImageSize;
            device->devStateInfo.chunkStreamImageOffset[i] = binaryImageOffset;
#ifdef ADRV910X_INIT_DEBUG
            ADRV910X_DEBUG_INFO(imageName[i]);
            ADRV910X_DEBUG_INFO_NUM("binaryImageOffset", (uint32_t)binaryImageOffset);
            ADRV910X_DEBUG_INFO_NUM("binaryImageSize", (uint32_t)binaryImageSize);
#endif
        }

        device->devStateInfo.currentStreamImageIndex = 0;
        device->devStateInfo.currentStreamBinBaseAddr = 0;
        device->devStateInfo.currentStreamBaseAddr = 0;
        device->devStateInfo.currentStreamImageSize = 0;
        device->devStateInfo.currentStreamNumberStreams = 0;
    }

    for (i = device->devStateInfo.currentStreamImageIndex; i < ADRV910X_MAX_NUM_STREAM; i++)
    {
#ifdef ADRV910X_INIT_DEBUG
        ADRV910X_DEBUG_INFO(imageName[i]);
        ADRV910X_DEBUG_INFO_NUM("binary", (uint32_t)binary);
        ADRV910X_DEBUG_INFO_NUM("byteCount", (uint32_t)byteCount);
#endif
        imageOffset = device->devStateInfo.chunkStreamImageOffset[i];
        if (i == 0)
        {
            imageOffset = MAIN_IMAGE_OFFSET;
        }
        if ((byteOffset <= imageOffset) &&
            ((byteOffset + byteCount) >= (imageOffset + 4)))
        {
            binOffset = imageOffset - byteOffset;
            adrv910x_BinaryParamsGetBaseAddr(device, &binary[binOffset], &word1);
            device->devStateInfo.currentStreamBinBaseAddr = word1;
            device->devStateInfo.currentStreamImageSize = device->devStateInfo.chunkStreamImageSize[i];
        }

        if ((byteOffset <= (imageOffset + 4)) &&
            ((byteOffset + byteCount) >= imageOffset + 8))
        {
            binOffset = imageOffset + 4 - byteOffset;
            adrv910x_BinaryParamsGetBaseAddr(device, &binary[binOffset], &word1);
            device->devStateInfo.currentStreamBaseAddr = word1;
        }

        if ((byteOffset <= (imageOffset + 8)) &&
            ((byteOffset + byteCount) >= (imageOffset + 12)))
        {
            binOffset = imageOffset + 8 - byteOffset;
            adrv910x_BinaryParamsGetNumberStreamImageSize(device, &binary[binOffset], &byte1, &word1);
            device->devStateInfo.currentStreamNumberStreams = byte1;
        }


        if ((device->devStateInfo.currentStreamBinBaseAddr > 0) &&
            (device->devStateInfo.currentStreamImageSize > 0))
        {
            addr = streamAddr[i];

            if (device->devStateInfo.currentStreamImageSize == device->devStateInfo.chunkStreamImageSize[i])
            {
                streamCtl = streamControlDefaultVal[i] | ADRV910X_STREAM_RESET;
                ADRV910X_SPIWRITEBYTE(device, "STREAM_CTL", addr, streamCtl);
            }

            /* Load the stream image to memory, starting at the memory address specified by Stream_binary_base */
            if (byteCount >= device->devStateInfo.currentStreamImageSize)
            {
                imageSize = device->devStateInfo.currentStreamImageSize;
            }
            else
            {
                imageSize = byteCount;
            }

            if (i == 0)
            {
                /* Load main SP binary image */
                /* Main Stream Processor is connected on the AHB system bus. It shares its memory with the ARM subsystem.
                    So the main SP memory can be read/loaded through SPI-to-AHB interface*/
                recoveryAction = adrv910x_DmaMemWrite(device, device->devStateInfo.currentStreamBinBaseAddr, &binary[0], imageSize, spiWriteMode, ADI_PS1);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else
            {
                /* Load flex SP 0-3 binary image */
                /* The 4 flex Stream Processors are connected on the AHB system bus have dedicated program code memory.
                    So the flex SP memory can be read/loaded only through a specific Flex_SP_SPI-to-AHB interface*/
                recoveryAction = adrv910x_FlexStreamProcessorMemWrite(device, device->devStateInfo.currentStreamBinBaseAddr, &binary[0], imageSize, i, spiWriteMode);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            byteCount -= imageSize;
            byteOffset += imageSize;
            binary += imageSize;
            device->devStateInfo.currentStreamImageSize -= imageSize;
            device->devStateInfo.currentStreamBinBaseAddr += imageSize;

            if (device->devStateInfo.currentStreamImageSize == 0)
            {
                /* 4.    Populate registers(stream_base_byte0) and (stream_base_byte1)with the lower 16 bits of the Stream_base */
                ADRV910X_SPIWRITEBYTE(device, "STREAM_BASE_BYTE0", addr + ADRV910X_STREAM_BASE_BYTE0_OFFSET, (uint8_t)(device->devStateInfo.currentStreamBaseAddr));

                ADRV910X_SPIWRITEBYTE(device, "STREAM_BASE_BYTE1", addr + ADRV910X_STREAM_BASE_BYTE1_OFFSET, (uint8_t)(device->devStateInfo.currentStreamBaseAddr >> 8));

                /* 5.    Populate register  (last_stream_num)with the No_of_streams value */
                ADRV910X_SPIWRITEBYTE(device, "LAST_STREAM_NUMBER", addr + ADRV910X_LAST_STREAM_NUMBER_OFFSET, device->devStateInfo.currentStreamNumberStreams - 1);

                /* 6.    Clear bit D0 in register  (stream_reset) */
                streamCtl = streamControlDefaultVal[i] & ~ADRV910X_STREAM_RESET;
                ADRV910X_SPIWRITEBYTE(device, "STREAM_CTL", addr, streamCtl);

                device->devStateInfo.currentStreamImageIndex++;
                device->devStateInfo.currentStreamBinBaseAddr = 0;
                device->devStateInfo.currentStreamBaseAddr = 0;
                device->devStateInfo.currentStreamImageSize = 0;
                device->devStateInfo.currentStreamNumberStreams = 0;
            }

        }
        else
        {
            break;
        }

        if (byteCount == 0)
        {
            break;
        }
    }

    if (device->devStateInfo.currentStreamImageIndex == ADRV910X_MAX_NUM_STREAM)
    {
        device->devStateInfo.devState = (adi_adrv910x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV910X_STATE_STREAM_LOADED);
    }

    return (recoveryAction);
}

#else  /* NEVIS_PS2 - Stream image from Flash */

#define STREAM_FLASH_CHUNK_SIZE 1040
static uint8_t streamChunkBuffer[STREAM_FLASH_CHUNK_SIZE] = {0};

int32_t adi_adrv910x_Stream_Image_Write(adi_adrv910x_Device_t *device,
                                      uint32_t byteOffset,
                                      uint8_t binary[],
                                      uint32_t byteCount,
                                      adi_adrv910x_ArmSingleSpiWriteMode_e spiWriteMode)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint16_t binaryImageSize = 0;
    uint16_t binaryImageOffset = 0;
    uint32_t i = 0;
    uint32_t addr = 0;
    uint32_t word1 = 0;
    uint8_t  streamCtl = 0;
    uint8_t  byte1 = 0;
    uint32_t imageSize = 0;
    uint32_t imageOffset = 0;

    uint32_t bytesToProcess = 0;
    adi_driver_Result_e retVal;

    uint32_t streamAddr[] = {
        ADRV910X_ADDR_MAIN_STREAM_CTL_OFFSET,
        ADRV910X_ADDR_FLEX_SP0_STREAM_CTL_OFFSET,
        ADRV910X_ADDR_FLEX_SP1_STREAM_CTL_OFFSET,
        ADRV910X_ADDR_FLEX_SP2_STREAM_CTL_OFFSET,
        ADRV910X_ADDR_FLEX_SP3_STREAM_CTL_OFFSET
    };

    uint8_t streamControlDefaultVal[] = { 0xF0, 0xC0, 0xC0, 0xC0, 0xC0};

    static const uint8_t MIN_BYTECOUNT = 68;
    static const uint8_t MAIN_IMAGE_OFFSET = 40;

    /* Arm stream download order: main, tx1/2, rx1/2 */
    static const uint32_t streamChannel[] = {
        0xFFFFFFFF,
        ADI_ADRV910X_TX1,
        ADI_ADRV910X_TXNB,
        ADI_ADRV910X_RX1 | ADI_ADRV910X_ORX1,
        ADI_ADRV910X_RXNB | ADI_ADRV910X_ORXNB
    };

    ADI_ENTRY_EXPECT(device);
    ADI_RANGE_CHECK(device,
                    spiWriteMode,
                    ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4,
                    ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4);

    adi_boot_SystemCfg_t bootConfig = {0};
    adi_driver_Result_e result = adi_boot_ConfigGet(&bootConfig);
    if (result != ADI_DRV_SUCCESS)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            result,
            "Failed to get bootloader configuration.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    
    adi_flash_Handle_t flashHdl = bootConfig.flash_handle;
    ADI_NULL_DEVICE_PTR_RETURN(flashHdl);

    adi_boot_ImageDesc_t streamImage = {0};

    result = adi_boot_ActiveImageGet(IMAGE_TYPE_STREAM, &streamImage);
    if (result != ADI_DRV_SUCCESS)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            result,
            "Failed to get active image from bootloader catalog.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    
	uint32_t flashReadAddress = streamImage.code_flash_start_addr;
    byteCount = streamImage.code_flash_size;
    uint32_t bytesRemaining = byteCount;

    (void)binary; /* Binary data is not used in PS2 boot flow since we're reading directly from flash */
    (void)streamChannel; /* Suppress unused variable warning */

    if ((byteCount % 4) > 0)
    {
        byteCount += (4 - (byteCount % 4));
        bytesRemaining = byteCount;
    }

    if ((byteOffset % 4) > 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM,
            byteOffset, "Invalid byteOffset. Must be multiple of 4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Adjust flash read address based on byteOffset */
    flashReadAddress += byteOffset;

    if (byteOffset == 0)
    {
        if (byteCount < MIN_BYTECOUNT)
        {
            ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM,
                byteCount, "Invalid byteCount. Must be 68 or greater.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Read header from flash to extract stream image info */
        bytesToProcess = (bytesRemaining > STREAM_FLASH_CHUNK_SIZE) ? STREAM_FLASH_CHUNK_SIZE : bytesRemaining;

        memset(streamChunkBuffer, 0, STREAM_FLASH_CHUNK_SIZE);

        retVal = adi_flash_Read(flashHdl, flashReadAddress, streamChunkBuffer, bytesToProcess);
        if (retVal != ADI_DRV_SUCCESS)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_API_FAIL,
                ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                retVal,
                "Stream Flash read failed (header processing)");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        while(true == adi_flash_CommandBusy(flashHdl));

        /* Extract stream image info from header */
        for (i = 0; i < ADRV910X_MAX_NUM_STREAM; i++)
        {
            adrv910x_BinaryImageInfoGet(device, &streamChunkBuffer[12 + i * 4], &binaryImageSize, &binaryImageOffset);
            device->devStateInfo.chunkStreamImageSize[i] = binaryImageSize;
            device->devStateInfo.chunkStreamImageOffset[i] = binaryImageOffset;
        }

        device->devStateInfo.currentStreamImageIndex = 0;
        device->devStateInfo.currentStreamBinBaseAddr = 0;
        device->devStateInfo.currentStreamBaseAddr = 0;
        device->devStateInfo.currentStreamImageSize = 0;
        device->devStateInfo.currentStreamNumberStreams = 0;
    }

    /* Process stream images reading from flash in chunks */
    for (i = device->devStateInfo.currentStreamImageIndex; i < ADRV910X_MAX_NUM_STREAM; i++)
    {
        imageOffset = device->devStateInfo.chunkStreamImageOffset[i];
        if (i == 0)
        {
            imageOffset = MAIN_IMAGE_OFFSET;
        }

        /* Read chunk from flash if needed for metadata extraction */
        if ((byteOffset <= imageOffset) &&
            ((byteOffset + byteCount) >= (imageOffset + 4)))
        {
            /* Read from flash at the appropriate offset */
            uint32_t metadataFlashAddr = streamImage.code_flash_start_addr + imageOffset;

            memset(streamChunkBuffer, 0, 16);
            retVal = adi_flash_Read(flashHdl, metadataFlashAddr, streamChunkBuffer, 16);
            if (retVal != ADI_DRV_SUCCESS)
            {
                ADI_ERROR_REPORT(&device->common,
                    ADI_COMMON_ERRSRC_API,
                    ADI_COMMON_ERR_API_FAIL,
                    ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                    retVal,
                    "Stream Flash read failed (metadata)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            while(true == adi_flash_CommandBusy(flashHdl));

            adrv910x_BinaryParamsGetBaseAddr(device, &streamChunkBuffer[0], &word1);
            device->devStateInfo.currentStreamBinBaseAddr = word1;
            device->devStateInfo.currentStreamImageSize = device->devStateInfo.chunkStreamImageSize[i];

            adrv910x_BinaryParamsGetBaseAddr(device, &streamChunkBuffer[4], &word1);
            device->devStateInfo.currentStreamBaseAddr = word1;

            adrv910x_BinaryParamsGetNumberStreamImageSize(device, &streamChunkBuffer[8], &byte1, &word1);
            device->devStateInfo.currentStreamNumberStreams = byte1;
        }

        if ((device->devStateInfo.currentStreamBinBaseAddr > 0) &&
            (device->devStateInfo.currentStreamImageSize > 0))
        {
            addr = streamAddr[i];

            if (device->devStateInfo.currentStreamImageSize == device->devStateInfo.chunkStreamImageSize[i])
            {
                streamCtl = streamControlDefaultVal[i] | ADRV910X_STREAM_RESET;
                ADRV910X_SPIWRITEBYTE(device, "STREAM_CTL", addr, streamCtl);
            }

            /* Load the stream image from flash to memory in chunks */
            imageSize = device->devStateInfo.currentStreamImageSize;

            /* Calculate flash address for this stream's binary data */
            uint32_t streamDataFlashAddr = streamImage.code_flash_start_addr + device->devStateInfo.chunkStreamImageOffset[i];
            if (i == 0)
            {
                streamDataFlashAddr = streamImage.code_flash_start_addr + MAIN_IMAGE_OFFSET;
            }
            /* Adjust for already processed bytes */
            streamDataFlashAddr += (device->devStateInfo.chunkStreamImageSize[i] - device->devStateInfo.currentStreamImageSize);

            while (imageSize > 0)
            {
                bytesToProcess = (imageSize > STREAM_FLASH_CHUNK_SIZE) ? STREAM_FLASH_CHUNK_SIZE : imageSize;

                memset(streamChunkBuffer, 0, bytesToProcess);

                retVal = adi_flash_Read(flashHdl, streamDataFlashAddr, streamChunkBuffer, bytesToProcess);
                if (retVal != ADI_DRV_SUCCESS)
                {
                    ADI_ERROR_REPORT(&device->common,
                        ADI_COMMON_ERRSRC_API,
                        ADI_COMMON_ERR_API_FAIL,
                        ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                        retVal,
                        "Stream Flash read failed");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
                while(true == adi_flash_CommandBusy(flashHdl));

                /* Align to 4-byte boundary for memory write */
                uint32_t bytesToWrite = bytesToProcess;
                if ((bytesToWrite % 4) != 0)
                {
                    bytesToWrite += (4 - (bytesToWrite % 4));
                }

                if (i == 0)
                {
                    /* Load main SP binary image */
                    recoveryAction = adrv910x_DmaMemWrite(device, device->devStateInfo.currentStreamBinBaseAddr, streamChunkBuffer, bytesToWrite, spiWriteMode, ADI_PS1);
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
                else
                {
                    /* Load flex SP 0-3 binary image */
                    recoveryAction = adrv910x_FlexStreamProcessorMemWrite(device, device->devStateInfo.currentStreamBinBaseAddr, streamChunkBuffer, bytesToWrite, i, spiWriteMode);
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }

                streamDataFlashAddr += bytesToProcess;
                device->devStateInfo.currentStreamBinBaseAddr += bytesToWrite;
                imageSize -= bytesToProcess;
            }

            device->devStateInfo.currentStreamImageSize = 0;

            /* Configure stream processor registers */
            ADRV910X_SPIWRITEBYTE(device, "STREAM_BASE_BYTE0", addr + ADRV910X_STREAM_BASE_BYTE0_OFFSET, (uint8_t)(device->devStateInfo.currentStreamBaseAddr));
            ADRV910X_SPIWRITEBYTE(device, "STREAM_BASE_BYTE1", addr + ADRV910X_STREAM_BASE_BYTE1_OFFSET, (uint8_t)(device->devStateInfo.currentStreamBaseAddr >> 8));
            ADRV910X_SPIWRITEBYTE(device, "LAST_STREAM_NUMBER", addr + ADRV910X_LAST_STREAM_NUMBER_OFFSET, device->devStateInfo.currentStreamNumberStreams - 1);

            streamCtl = streamControlDefaultVal[i] & ~ADRV910X_STREAM_RESET;
            ADRV910X_SPIWRITEBYTE(device, "STREAM_CTL", addr, streamCtl);

            device->devStateInfo.currentStreamImageIndex++;
            device->devStateInfo.currentStreamBinBaseAddr = 0;
            device->devStateInfo.currentStreamBaseAddr = 0;
            device->devStateInfo.currentStreamNumberStreams = 0;
        }
        else
        {
            break;
        }
    }

    if (device->devStateInfo.currentStreamImageIndex == ADRV910X_MAX_NUM_STREAM)
    {
        device->devStateInfo.devState = (adi_adrv910x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV910X_STATE_STREAM_LOADED);
    }

    return (recoveryAction);
}
#endif /* NEVIS_PS2 */

int32_t adi_adrv910x_Stream_Version(adi_adrv910x_Device_t *device, adi_adrv910x_StreamVersion_t *streamVersion)
{
    uint8_t ver[4] = { 0 };
    uint8_t bfValue1 = 0;
    uint8_t bfValue2 = 0;
    uint32_t streamVersionAddress = 0;
    uint32_t streamJumpTableAddress = 0;

    static uint32_t STREAM_BASE_ADDR = 0x20040000;
    static uint32_t STREAM_VERSION_OFFSET = 0x30;

    ADI_ENTRY_PTR_EXPECT(device, streamVersion);

    if ((device->devStateInfo.devState & ADI_ADRV910X_STATE_STREAM_LOADED) != ADI_ADRV910X_STATE_STREAM_LOADED)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_ADRV910X_SRC_STREAMCMD,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            device,
            "Stream processor binary must be loaded before calling adi_adrv910x_stream_Version()");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Read the jump table address from SPI register */
    ADRV910X_SPIREADBYTE(device, "STREAM_BASE_BYTE0", (ADRV910X_ADDR_MAIN_STREAM_CTL_OFFSET + ADRV910X_STREAM_BASE_BYTE0_OFFSET), &bfValue1);
    ADRV910X_SPIREADBYTE(device, "STREAM_BASE_BYTE1", (ADRV910X_ADDR_MAIN_STREAM_CTL_OFFSET + ADRV910X_STREAM_BASE_BYTE1_OFFSET), &bfValue2);
    streamJumpTableAddress = STREAM_BASE_ADDR + ((bfValue2 << 8) | bfValue1);

    streamVersionAddress = streamJumpTableAddress - STREAM_VERSION_OFFSET;
    if (streamVersionAddress < STREAM_BASE_ADDR)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_ADRV910X_SRC_STREAMCMD,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            device,
            "Stream processor version address is invalid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_EXPECT(adi_adrv910x_arm_Memory_Read, device, streamVersionAddress, &ver[0], sizeof(ver), ADRV910X_ARM_MEM_READ_AUTOINCR, ADI_PS1);

    streamVersion->buildVer = ver[3];
    streamVersion->maintVer = ver[2];
    streamVersion->minorVer = ver[1];
    streamVersion->majorVer = ver[0];

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv910x_Stream_Gpio_Debug_Set_Validate(adi_adrv910x_Device_t *adrv910x)
{
    adi_adrv910x_RadioState_t currentState = { 0 };
    uint8_t chanId = 0u;
    uint8_t portId = 0u;
    /* Validate current state. All the channels must be in STANDBY or CALIBRATED state. */
    ADI_EXPECT(adi_adrv910x_Radio_State_Get, adrv910x, &currentState);
    for (portId = 0u; portId < ADI_ADRV910X_NUM_PORTS; portId++)
    {
        for (chanId = 0u; chanId < ADI_ADRV910X_NUM_CHANNELS; chanId++)
        {
            if ((currentState.channelStates[portId][chanId] == ADI_ADRV910X_CHANNEL_PRIMED) ||
                (currentState.channelStates[portId][chanId] == ADI_ADRV910X_CHANNEL_RF_ENABLED))
            {
                ADI_ERROR_REPORT(&adrv910x->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 currentState.channelStates[portId][chanId],
                                 "Error while attempting to send stream to GPIO debug mailbox command to ARM firmware. All the channels must be in STANDBY or CALIBRATED  state.");
                ADI_API_RETURN(adrv910x)
            }
        }
    }   
    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_Stream_Gpio_Debug_Set(adi_adrv910x_Device_t *adrv910x, bool streamToGpioDebug)
{
    uint8_t armData[5] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_Stream_Gpio_Debug_Set_Validate, adrv910x);

    adrv910x_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
    armData[offset++] = (uint8_t)streamToGpioDebug;

    extData[0] =  0; /* Channel Mask; unused for this command */
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_CFG_GPIO_DEBUG_IN_STREAM;
    ADI_EXPECT(adi_adrv910x_arm_Config_Write, adrv910x, armData, sizeof(armData), extData, sizeof(extData))

    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t adi_adrv910x_Stream_Gpio_Debug_Get_Validate(adi_adrv910x_Device_t *adrv910x,
                                                                             bool *streamToGpioDebug)
{
    ADI_NULL_PTR_RETURN(&adrv910x->common, streamToGpioDebug);
    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_Stream_Gpio_Debug_Get(adi_adrv910x_Device_t *adrv910x, bool *streamToGpioDebug)
{
    uint8_t armReadBack[1] = { 0 };
    uint8_t channelMask = 0;
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_Stream_Gpio_Debug_Get_Validate, adrv910x, streamToGpioDebug);

	ADI_EXPECT(adi_adrv910x_arm_MailBox_Get, adrv910x, OBJID_GS_CONFIG, OBJID_CFG_GPIO_DEBUG_IN_STREAM, channelMask, offset, armReadBack, sizeof(armReadBack))

    *streamToGpioDebug = (bool)armReadBack[0];

    ADI_API_RETURN(adrv910x);
}

