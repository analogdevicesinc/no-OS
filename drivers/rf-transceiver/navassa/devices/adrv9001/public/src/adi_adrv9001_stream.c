/**
* \file
* \brief Contains related function implementation defined in adi_adrv9001_stream.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv9001_user.h"
#include "adi_adrv9001_stream.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001.h"
#include "adi_adrv9001_error.h"
//#include "adi_adrv9001_gpio.h"
#include "adi_adrv9001_radio.h"
#include "adi_adrv9001_spi.h"

#include "adrv9001_arm.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"
#include "object_ids.h"

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

static void adrv9001_BinaryImageInfoGet(adi_adrv9001_Device_t *device,
                                        const uint8_t binary[],
                                        uint16_t *binaryImageSize,
                                        uint16_t *binaryImageOffset)
{
    /* populating variables from binary array */
    *binaryImageSize = ((((uint16_t)binary[1]) << 8) | ((uint16_t)binary[0]));
    *binaryImageOffset = ((((uint16_t)binary[3]) << 8) | (uint16_t)(binary[2]));
}


static void adrv9001_BinaryParamsGetBaseAddr(adi_adrv9001_Device_t *device,
                                             const uint8_t binary[],
                                             uint32_t *word)
{
    /* populating variables from binary array */
    *word = (((uint32_t)binary[3]) << 24) | (((uint32_t)binary[2]) << 16) | (((uint32_t)binary[1]) << 8) | (uint32_t)(binary[0]);
}

static void adrv9001_BinaryParamsGetNumberStreamImageSize(adi_adrv9001_Device_t *device,
                                                          const uint8_t binary[],
                                                          uint8_t *numberStreams, uint32_t *imageSize)
{
    *numberStreams = binary[0];
    *imageSize = (((uint32_t)binary[3]) << 8) | (uint32_t)(binary[2]);
}

int32_t adi_adrv9001_Stream_Image_Write(adi_adrv9001_Device_t *device,
                                      uint32_t byteOffset,
                                      uint8_t binary[],
                                      uint32_t byteCount,
                                      adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode)
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
        ADRV9001_ADDR_MAIN_STREAM_CTL_OFFSET,
        ADRV9001_ADDR_FLEX_SP0_STREAM_CTL_OFFSET,
        ADRV9001_ADDR_FLEX_SP1_STREAM_CTL_OFFSET,
        ADRV9001_ADDR_FLEX_SP2_STREAM_CTL_OFFSET,
        ADRV9001_ADDR_FLEX_SP3_STREAM_CTL_OFFSET
    };

    uint8_t streamControlDefaultVal[] = { 0xF0, 0xC0, 0xC0, 0xC0, 0xC0};

#ifdef ADRV9001_INIT_DEBUG
    static const char * imageName[] = { "MAIN_STREAM", "TX0_STREAM", "TX1_STREAM","RX0_STREAM","RX1_STREAM", };
#endif
    static const uint8_t MIN_BYTECOUNT = 68;
    static const uint8_t MAIN_IMAGE_OFFSET = 40;

    /* Arm stream download order: main, tx1/2, rx1/2 */
    static const uint32_t streamChannel[] = {
        0xFFFFFFFF,
        ADI_ADRV9001_TX1,
        ADI_ADRV9001_TX2,
        ADI_ADRV9001_RX1 | ADI_ADRV9001_ORX1,
        ADI_ADRV9001_RX2 | ADI_ADRV9001_ORX2
    };

    ADI_ENTRY_PTR_EXPECT(device, binary);
    ADI_RANGE_CHECK(device,
                    spiWriteMode,
                    ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4,
                    ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4);

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

        for (i = 0; i < ADRV9001_MAX_NUM_STREAM; i++)
        {
            adrv9001_BinaryImageInfoGet(device, &binary[12 + i * 4], &binaryImageSize, &binaryImageOffset);
            device->devStateInfo.chunkStreamImageSize[i] = binaryImageSize;
            device->devStateInfo.chunkStreamImageOffset[i] = binaryImageOffset;
#ifdef ADRV9001_INIT_DEBUG
            ADRV9001_DEBUG_INFO(imageName[i]);
            ADRV9001_DEBUG_INFO_NUM("binaryImageOffset", (uint32_t)binaryImageOffset);
            ADRV9001_DEBUG_INFO_NUM("binaryImageSize", (uint32_t)binaryImageSize);
#endif
        }

        device->devStateInfo.currentStreamImageIndex = 0;
        device->devStateInfo.currentStreamBinBaseAddr = 0;
        device->devStateInfo.currentStreamBaseAddr = 0;
        device->devStateInfo.currentStreamImageSize = 0;
        device->devStateInfo.currentStreamNumberStreams = 0;
    }

    for (i = device->devStateInfo.currentStreamImageIndex; i < ADRV9001_MAX_NUM_STREAM; i++)
    {
#ifdef ADRV9001_INIT_DEBUG
        ADRV9001_DEBUG_INFO(imageName[i]);
        ADRV9001_DEBUG_INFO_NUM("binary", (uint32_t)binary);
        ADRV9001_DEBUG_INFO_NUM("byteCount", (uint32_t)byteCount);
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
            adrv9001_BinaryParamsGetBaseAddr(device, &binary[binOffset], &word1);
            device->devStateInfo.currentStreamBinBaseAddr = word1;
            device->devStateInfo.currentStreamImageSize = device->devStateInfo.chunkStreamImageSize[i];
        }

        if ((byteOffset <= (imageOffset + 4)) &&
            ((byteOffset + byteCount) >= imageOffset + 8))
        {
            binOffset = imageOffset + 4 - byteOffset;
            adrv9001_BinaryParamsGetBaseAddr(device, &binary[binOffset], &word1);
            device->devStateInfo.currentStreamBaseAddr = word1;
        }

        if ((byteOffset <= (imageOffset + 8)) &&
            ((byteOffset + byteCount) >= (imageOffset + 12)))
        {
            binOffset = imageOffset + 8 - byteOffset;
            adrv9001_BinaryParamsGetNumberStreamImageSize(device, &binary[binOffset], &byte1, &word1);
            device->devStateInfo.currentStreamNumberStreams = byte1;
        }


        if ((device->devStateInfo.currentStreamBinBaseAddr > 0) &&
            (device->devStateInfo.currentStreamImageSize > 0))
        {
            addr = streamAddr[i];

            if (device->devStateInfo.currentStreamImageSize == device->devStateInfo.chunkStreamImageSize[i])
            {
                if ((device->devStateInfo.initializedChannels & streamChannel[i]) > 0)
                {
                    streamCtl = streamControlDefaultVal[i] | ADRV9001_STREAM_RESET;
                    ADRV9001_SPIWRITEBYTE(device, "STREAM_CTL", addr, streamCtl);
                }
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

            if ((device->devStateInfo.initializedChannels & streamChannel[i]) > 0)
            {
                if (i == 0)
                {
                    /* Load main SP binary image */
                    /* Main Stream Processor is connected on the AHB system bus. It shares its memory with the ARM subsystem.
                       So the main SP memory can be read/loaded through SPI-to-AHB interface*/
                    recoveryAction = adrv9001_DmaMemWrite(device, device->devStateInfo.currentStreamBinBaseAddr, &binary[0], imageSize, spiWriteMode);
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
                else
                {
                    /* Load flex SP 0-3 binary image */
                    /* The 4 flex Stream Processors are connected on the AHB system bus have dedicated program code memory.
                       So the flex SP memory can be read/loaded only through a specific Flex_SP_SPI-to-AHB interface*/
                    recoveryAction = adrv9001_FlexStreamProcessorMemWrite(device, device->devStateInfo.currentStreamBinBaseAddr, &binary[0], imageSize, i, spiWriteMode);
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }

            byteCount -= imageSize;
            byteOffset += imageSize;
            binary += imageSize;
            device->devStateInfo.currentStreamImageSize -= imageSize;
            device->devStateInfo.currentStreamBinBaseAddr += imageSize;

            if (device->devStateInfo.currentStreamImageSize == 0)
            {
                if ((device->devStateInfo.initializedChannels & streamChannel[i]) > 0)
                {
                    /* 4.    Populate registers(stream_base_byte0) and (stream_base_byte1)with the lower 16 bits of the Stream_base */
                    ADRV9001_SPIWRITEBYTE(device, "STREAM_BASE_BYTE0", addr + ADRV9001_STREAM_BASE_BYTE0_OFFSET, (uint8_t)(device->devStateInfo.currentStreamBaseAddr));

                    ADRV9001_SPIWRITEBYTE(device, "STREAM_BASE_BYTE1", addr + ADRV9001_STREAM_BASE_BYTE1_OFFSET, (uint8_t)(device->devStateInfo.currentStreamBaseAddr >> 8));

                    /* 5.    Populate register  (last_stream_num)with the No_of_streams value */
                    ADRV9001_SPIWRITEBYTE(device, "LAST_STREAM_NUMBER", addr + ADRV9001_LAST_STREAM_NUMBER_OFFSET, device->devStateInfo.currentStreamNumberStreams - 1);

                    /* 6.    Clear bit D0 in register  (stream_reset) */
                    streamCtl = streamControlDefaultVal[i] & ~ADRV9001_STREAM_RESET;
                    ADRV9001_SPIWRITEBYTE(device, "STREAM_CTL", addr, streamCtl);
                }

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

    if (device->devStateInfo.currentStreamImageIndex == ADRV9001_MAX_NUM_STREAM)
    {
        device->devStateInfo.devState = (adi_adrv9001_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV9001_STATE_STREAM_LOADED);
    }

    return (recoveryAction);
}

int32_t adi_adrv9001_Stream_Version(adi_adrv9001_Device_t *device, adi_adrv9001_StreamVersion_t *streamVersion)
{
    uint8_t ver[4] = { 0 };
    uint8_t bfValue1 = 0;
    uint8_t bfValue2 = 0;
    uint32_t streamVersionAddress = 0;
    uint32_t streamJumpTableAddress = 0;

    static uint32_t STREAM_BASE_ADDR = 0x20040000;
    static uint32_t STREAM_VERSION_OFFSET = 0x30;

    ADI_ENTRY_PTR_EXPECT(device, streamVersion);

    if ((device->devStateInfo.devState & ADI_ADRV9001_STATE_STREAM_LOADED) != ADI_ADRV9001_STATE_STREAM_LOADED)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_ADRV9001_SRC_STREAMCMD,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            device,
            "Stream processor binary must be loaded before calling adi_adrv9001_stream_Version()");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Read the jump table address from SPI register */
    ADRV9001_SPIREADBYTE(device, "STREAM_BASE_BYTE0", (ADRV9001_ADDR_MAIN_STREAM_CTL_OFFSET + ADRV9001_STREAM_BASE_BYTE0_OFFSET), &bfValue1);
    ADRV9001_SPIREADBYTE(device, "STREAM_BASE_BYTE1", (ADRV9001_ADDR_MAIN_STREAM_CTL_OFFSET + ADRV9001_STREAM_BASE_BYTE1_OFFSET), &bfValue2);
    streamJumpTableAddress = STREAM_BASE_ADDR + ((bfValue2 << 8) | bfValue1);

    streamVersionAddress = streamJumpTableAddress - STREAM_VERSION_OFFSET;
    if (streamVersionAddress < STREAM_BASE_ADDR)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_ADRV9001_SRC_STREAMCMD,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            device,
            "Stream processor version address is invalid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_EXPECT(adi_adrv9001_arm_Memory_Read, device, streamVersionAddress, &ver[0], sizeof(ver), ADRV9001_ARM_MEM_READ_AUTOINCR);

    streamVersion->buildVer = ver[3];
    streamVersion->maintVer = ver[2];
    streamVersion->minorVer = ver[1];
    streamVersion->majorVer = ver[0];

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Stream_Gpio_Debug_Set_Validate(adi_adrv9001_Device_t *adrv9001)
{
    adi_adrv9001_RadioState_t currentState = { 0 };
    uint8_t chanId = 0u;
    uint8_t portId = 0u;
    /* Validate current state. All the channels must be in STANDBY or CALIBRATED state. */
    ADI_EXPECT(adi_adrv9001_Radio_State_Get, adrv9001, &currentState);
    for (portId = 0u; portId < ADI_ADRV9001_NUM_PORTS; portId++)
    {
        for (chanId = 0u; chanId < ADI_ADRV9001_NUM_CHANNELS; chanId++)
        {
            if ((currentState.channelStates[portId][chanId] == ADI_ADRV9001_CHANNEL_PRIMED) ||
                (currentState.channelStates[portId][chanId] == ADI_ADRV9001_CHANNEL_RF_ENABLED))
            {
                ADI_ERROR_REPORT(&adrv9001->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 currentState.channelStates[portId][chanId],
                                 "Error while attempting to send stream to GPIO debug mailbox command to ARM firmware. All the channels must be in STANDBY or CALIBRATED  state.");
                ADI_API_RETURN(adrv9001)
            }
        }
    }   
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_Stream_Gpio_Debug_Set(adi_adrv9001_Device_t *adrv9001, bool streamToGpioDebug)
{
    uint8_t armData[5] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Stream_Gpio_Debug_Set_Validate, adrv9001);

    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
    armData[offset++] = (uint8_t)streamToGpioDebug;

    extData[0] =  0; /* Channel Mask; unused for this command */
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_CFG_GPIO_DEBUG_IN_STREAM;
    ADI_EXPECT(adi_adrv9001_arm_Config_Write, adrv9001, armData, sizeof(armData), extData, sizeof(extData))

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Stream_Gpio_Debug_Get_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                             bool *streamToGpioDebug)
{
    ADI_NULL_PTR_RETURN(&adrv9001->common, streamToGpioDebug);
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_Stream_Gpio_Debug_Get(adi_adrv9001_Device_t *adrv9001, bool *streamToGpioDebug)
{
    uint8_t armReadBack[1] = { 0 };
    uint8_t channelMask = 0;
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Stream_Gpio_Debug_Get_Validate, adrv9001, streamToGpioDebug);

    ADI_EXPECT(adi_adrv9001_arm_Config_Read, adrv9001, OBJID_CFG_GPIO_DEBUG_IN_STREAM, channelMask, offset, armReadBack, sizeof(armReadBack))

    *streamToGpioDebug = (bool)armReadBack[0];

    ADI_API_RETURN(adrv9001);
}

