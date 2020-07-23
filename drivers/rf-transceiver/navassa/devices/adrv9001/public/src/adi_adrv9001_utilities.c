/**
* \file
* \brief Contains Utility features related function implementation defined in
* adi_adrv9001_utilities.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/slab.h>

#ifndef free
#define free kfree
#endif

#ifndef calloc
#define calloc(n, s) kcalloc(n, s, GFP_KERNEL)
#endif

#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif

#include "adi_adrv9001_user.h"
#include "adi_common_macros.h"
#include "adi_adrv9001_utilities.h"
#include "adi_adrv9001_error.h"
#include "adi_adrv9001_spi.h"
#include "adi_adrv9001_rx.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_tx.h"
#include "adi_adrv9001_radio.h"
#include "adi_adrv9001_stream.h"
#include "adi_adrv9001_cals.h"
#include "adi_adrv9001_mcs.h"
#include "adi_adrv9001_dpd.h"
#include "adi_adrv9001_fh.h"
#include "adi_adrv9001_Init_t_parser.h"
#include "adrv9001_arm.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_reg_addr_macros.h"
#include "adrv9001_bf_hal.h"
#include "adrv9001_init.h"
#include "adrv9001_utilities.h"
#include "adi_adrv9001_gpio.h"
#include "adi_adrv9001_gpio_types.h"

#include "adrv9001_bf.h"
#include "adi_adrv9001_ssi.h"

#ifdef ADI_DYNAMIC_PROFILE_LOAD
int32_t adi_adrv9001_Utilities_DeviceProfile_Parse(adi_adrv9001_Device_t *device, adi_adrv9001_Init_t *init, char * jsonBuffer, uint32_t length)
{
    static const int16_t ADI_ADRV9001_TOKEN_MAX_LENGTH = 32;

    uint16_t ii = 0;
    int16_t numTokens = 0;
    jsmn_parser parser = { 0 };
    jsmntok_t * tokens = NULL;
    char parsingBuffer[ADI_ADRV9001_TOKEN_MAX_LENGTH]; /* This buffer only needs to hold a stringified number like '123.4567'. */


    /* initialize the JSMN parser and determine the number of JSON tokens */
    jsmn_init(&parser);
    numTokens = jsmn_parse(&parser, jsonBuffer, length, NULL, 0);

    /* The JSON file must be tokenized successfully. */
    if (numTokens < 1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Fatal error while parsing profile file. The JSON may be invalid, or the token buffer may be too small.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* allocate space for tokens */
    tokens = (jsmntok_t*)calloc(numTokens, sizeof(jsmntok_t));

    if (NULL == tokens)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Fatal error while reading profile file. Possible memory shortage.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* initialize the JSMN parser and parse the profile file into the tokens array */
    jsmn_init(&parser);
    numTokens = jsmn_parse(&parser, jsonBuffer, length, tokens, numTokens);

    /* The top-level element must be an object. */
    if (numTokens < 1 || tokens[0].type != JSMN_OBJECT)
    {
        free(tokens);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Fatal error while parsing profile file. The JSON may be invalid, or the token buffer may be too small.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Loop over all keys of the root object, searching for matching fields. */
    for (ii = 1; ii < numTokens; ii++)
    {
        ADRV9001_INIT_T(tokens, ii, jsonBuffer, parsingBuffer, (*init));
    }

    free(tokens);
    tokens = NULL;

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Utilities_DeviceProfile_Load(adi_adrv9001_Device_t *device, const char *profileFilename, adi_adrv9001_Init_t *init)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t length = 0;
    char * jsonBuffer = 0;

    ADI_EXPECT(adrv9001_SafeFileLoad, device, profileFilename, &jsonBuffer, &length);

    recoveryAction = adi_adrv9001_Utilities_DeviceProfile_Parse(device, init, jsonBuffer, length);
    free(jsonBuffer);

    if (recoveryAction < ADI_COMMON_ACT_NO_ACTION)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            device->common.error.errCode,
            recoveryAction,
            NULL,
            device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_API_RETURN(device);
}
#endif

#ifdef ADI_FILESYSTEM_AVAILABLE
int32_t adi_adrv9001_Utilities_ArmImage_Load(adi_adrv9001_Device_t *device, const char *armImagePath)
{
    static const size_t BIN_ELEMENT_SIZE = 1;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    FILE *armImageFilePointer = NULL;
    uint32_t fileSize = 0;
    uint32_t numFileChunks = 0;
    uint32_t i = 0;
    uint8_t armBinaryImageBuffer[ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];

    /* Check device pointer is not null */
    ADI_API_ENTRY_EXPECT(device);

    /*Open ARM binary file*/
#ifdef __GNUC__
    armImageFilePointer = fopen(armImagePath, "rb");
#else
    if (fopen_s(&armImageFilePointer, armImagePath, "rb") != 0)
    {
        ADI_ERROR_REPORT(&device->common,  ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, armImagePath,
                     "Unable to open ARM binary image file. Please check if the path is correct");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif

    if (armImageFilePointer == NULL)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, armImagePath,
                     "Invalid ARM binary image path encountered while attempting to load ARM binary image");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Determine file size*/
    if (fseek(armImageFilePointer, 0, SEEK_END) < 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, armImageFilePointer,
                     "Unable to move file descriptor to the end of the ARM binary image file");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, armImageFilePointer);
    }

    fileSize = ftell(armImageFilePointer);
    numFileChunks = (uint32_t)(fileSize / ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES);

    /*Check that ARM binary file is not empty*/
    if (fileSize == 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, armImagePath,
                     "Empty ARM binary image file encountered while attempting to load ARM binary image");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, armImageFilePointer);
    }

    /*Check that ARM binary file size does not exceed maximum size*/
    if (fileSize > ADI_ADRV9001_ARM_BINARY_IMAGE_FILE_SIZE_BYTES)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, armImagePath,
                     "ARM binary image file exceeds maximum ARM binary image size");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, armImageFilePointer);
    }

    /*Check that size of the file is a multiple of 4*/
    if ((fileSize % 4) != 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, armImagePath,
                     "ARM binary image file is expected to be a multiple of 4");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, armImageFilePointer);
    }

    /*Check that size of the file is divisible into equal sized chunks*/
    if ((fileSize % numFileChunks) != 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, armImagePath,
            "ARM binary image chunk size is expected to divide the stream file into equal chunks");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, armImageFilePointer);
    }

    /*Rewind the file pointer to beginning of the file*/
    if (fseek(armImageFilePointer, 0, SEEK_SET) < 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, armImageFilePointer,
                     "Unable to move file descriptor to the beginning of the ARM binary image file");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, armImageFilePointer);
    }

    /*Read ARM binary file*/
    for (i = 0; i < numFileChunks; i++)
    {
        if (fread(&armBinaryImageBuffer[0], BIN_ELEMENT_SIZE, ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES, armImageFilePointer) < ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES)
        {
            ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, armImagePath,
                "Fatal error while reading ARM binary file. Possible memory shortage");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, armImageFilePointer);
        }

        /*Write the ARM binary chunk*/
        if ((recoveryAction = adi_adrv9001_arm_Image_Write(device, (i*ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES), &armBinaryImageBuffer[0],
                                                ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES)) != ADI_COMMON_ACT_NO_ACTION)
        {
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, armImageFilePointer);
        }
    }


    /*Close ARM binary file*/
    if (fclose(armImageFilePointer) < 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, armImagePath,
                         "Fatal error while trying to close ARM binary file");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}
#else
int32_t adi_adrv9001_Utilities_ArmImage_Load(adi_adrv9001_Device_t *device, const char *armImagePath)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i = 0;
    uint8_t armBinaryImageBuffer[ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];

    /* Check device pointer is not null */
    ADI_API_ENTRY_EXPECT(device);

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
                                                ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES)) != ADI_COMMON_ACT_NO_ACTION)
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
#endif

#ifdef ADI_FILESYSTEM_AVAILABLE
int32_t adi_adrv9001_Utilities_StreamImage_Load(adi_adrv9001_Device_t *device, const char *streamImagePath)
{
    static const size_t BIN_ELEMENT_SIZE = 1;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    FILE *streamImageFilePointer = NULL;
    uint32_t fileSize = 0;
    uint32_t numFileChunks = 0;
    uint32_t i = 0;
    uint8_t streamBinaryImageBuffer[ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];

    /* Check device pointer is not null */
    ADI_API_ENTRY_EXPECT(device);

    /* Open ARM binary file */
#ifdef __GNUC__
    streamImageFilePointer = fopen(streamImagePath, "rb");
#else
    if (fopen_s(&streamImageFilePointer, streamImagePath, "rb") != 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, streamImagePath,
            "Unable to open stream binary image file. Please check if the path is correct");
        ADI_ERROR_RETURN(device->common.error.newAction);
}
#endif

    if (streamImageFilePointer == NULL)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, streamImagePath,
            "Invalid Stream binary image path encountered while attempting to load Stream processor");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Determine file size*/
    if (fseek(streamImageFilePointer, 0, SEEK_END) < 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, streamImageFilePointer,
            "Unable to move file descriptor to the end of the stream binary image file");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, streamImageFilePointer);
    }

    fileSize = ftell(streamImageFilePointer);
    numFileChunks = (uint32_t)(fileSize / ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES);

    /* Check that Stream binary file is not empty */
    if (fileSize == 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, streamImagePath,
            "Empty Stream binary image file encountered while attempting to load the Stream processor");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, streamImageFilePointer);
    }

    /* Check that Stream binary file size does not exceed maximum size */
    if (fileSize > ADI_ADRV9001_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, streamImagePath,
            "Stream binary image file exceeds maximum Stream binary image size");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, streamImageFilePointer);
    }

    /* Check that size of the file is divisible into equal sized chunks */
    if ((fileSize % numFileChunks) != 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, streamImagePath,
           "Stream binary image chunk size is expected to divide the stream file into equal chunks");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, streamImageFilePointer);
    }

    /* Check that size of the file is a multiple of 4 */
    if ((fileSize % 4) != 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, streamImagePath,
            "Stream binary image file is expected to be a multiple of 4");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, streamImageFilePointer);
    }

    /* Rewind the file pointer to beginning of the file */
    if (fseek(streamImageFilePointer, 0, SEEK_SET) < 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, streamImageFilePointer,
            "Unable to move file descriptor to the beginning of the stream binary image file");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, streamImageFilePointer);
    }

    /* Read Stream binary file */
    for (i = 0; i < numFileChunks; i++)
    {
        if (fread(&streamBinaryImageBuffer[0], BIN_ELEMENT_SIZE, ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES, streamImageFilePointer) < ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES)
        {
            ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, streamImagePath,
                "Fatal error while reading stream binary file. Possible memory shortage");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, streamImageFilePointer);
        }

        /* Write Stream binary */
        recoveryAction = adi_adrv9001_Stream_Image_Write(device, (i*ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES), &streamBinaryImageBuffer[0], ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES);
    }

    /* Close Stream binary file */
    if (fclose(streamImageFilePointer) < 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, streamImagePath,
            "Fatal error while trying to close stream binary file. Possible memory shortage while flushing / other I/O errors.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    device->devStateInfo.devState = (adi_adrv9001_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV9001_STATE_STREAM_LOADED);
    return recoveryAction;
}
# else
int32_t adi_adrv9001_Utilities_StreamImage_Load(adi_adrv9001_Device_t *device, const char *streamImagePath)
{

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i = 0;
    uint8_t streamBinaryImageBuffer[ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];

    /* Check device pointer is not null */
    ADI_API_ENTRY_EXPECT(device);

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
                                                              ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES)) != ADI_COMMON_ACT_NO_ACTION)
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
#endif

int32_t adi_adrv9001_Utilities_StreamImageBinary_Program(adi_adrv9001_Device_t *device,
                                                         uint8_t (*streamImageBinary)[ADI_ADRV9001_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES])
{
    int32_t i = 0;
    uint32_t numFileChunks = 0;

    /* Check device pointer is not null */
    ADI_API_ENTRY_EXPECT(device);

    numFileChunks = ADI_ADRV9001_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES / ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES;

    /*Read ARM binary file*/
    for (i = 0; i < numFileChunks; i++)
    {
        /* Write the stream binary chunk */
        ADI_EXPECT(adi_adrv9001_Stream_Image_Write,
                   device,
                   (i*ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES),
                   &(*streamImageBinary)[i*ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES],
                   ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES);
    }

    device->devStateInfo.devState = (adi_adrv9001_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV9001_STATE_STREAM_LOADED);
    ADI_API_RETURN(device);
}

static int32_t adi_adrv9001_Utilities_Tables_Load(adi_adrv9001_Device_t *device, adi_adrv9001_ResourceCfg_t *resourceCfg)
{
    uint8_t mask = 0;

    /* Load ORx gain table */
    if ((ADRV9001_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV9001_ORX_PROFILE_VALID)) ||
        (ADRV9001_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV9001_TX_PROFILE_VALID)))
    {
        mask |= ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, ADI_ADRV9001_ORX1) ? ADI_CHANNEL_1 : 0;
        mask |= ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, ADI_ADRV9001_ORX2) ? ADI_CHANNEL_2 : 0;
        mask |= ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, ADI_ADRV9001_TX1) ? ADI_CHANNEL_1 : 0;
        mask |= ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, ADI_ADRV9001_TX2) ? ADI_CHANNEL_2 : 0;
        ADI_EXPECT(adi_adrv9001_Utilities_RxGainTable_Load,
                   device,
                   (const char*)resourceCfg->adrv9001PlatformFiles->orxGainTableFile,
                   mask);
    }

    if ((ADRV9001_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV9001_RX_PROFILE_VALID)) ||
        (ADRV9001_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV9001_TX_PROFILE_VALID)))
    {
        mask = 0;
        mask |= ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, ADI_ADRV9001_RX1) ? ADI_CHANNEL_1 : 0;
        mask |= ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, ADI_ADRV9001_RX2) ? ADI_CHANNEL_2 : 0;
        mask |= ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, ADI_ADRV9001_TX1) ? ADI_CHANNEL_1 : 0;
        mask |= ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, ADI_ADRV9001_TX2) ? ADI_CHANNEL_2 : 0;
        /* Load Rx gain table for requested channels */
        ADI_EXPECT(adi_adrv9001_Utilities_RxGainTable_Load,
                   device,
                   (const char*)resourceCfg->adrv9001PlatformFiles->rxGainTableFile,
                   mask);
    }

    if (ADRV9001_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV9001_TX_PROFILE_VALID))
    {
        mask = 0;
        mask |= ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, ADI_ADRV9001_TX1) ? ADI_CHANNEL_1 : 0;
        mask |= ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, ADI_ADRV9001_TX2) ? ADI_CHANNEL_2 : 0;
        /* Load Tx atten tables from a .csv file for requested channels */
        ADI_EXPECT(adi_adrv9001_Utilities_TxAttenTable_Load,
                   device,
                   (const char*)resourceCfg->adrv9001PlatformFiles->txAttenTableFile,
                   mask);
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Utilities_Resources_Load(adi_adrv9001_Device_t *device, adi_adrv9001_ResourceCfg_t *resourceCfg)
{
    int32_t origLogLevel = 0;
    int32_t logLevelNoSpi = 0;
    adi_adrv9001_Init_t *init = NULL;
    const char *armImagePath = NULL;
    const char *streamImagePath = NULL;

    /* Check device pointer is not null */
    ADI_API_ENTRY_PTR_EXPECT(device, resourceCfg);
    ADI_NULL_PTR_RETURN(&device->common, resourceCfg->adrv9001Init);
    ADI_NULL_PTR_RETURN(&device->common, resourceCfg->adrv9001PlatformFiles);
    ADI_NULL_PTR_RETURN(&device->common, resourceCfg->adrv9001PlatformFiles->armImageFile);
    ADI_NULL_PTR_RETURN(&device->common, resourceCfg->adrv9001PlatformFiles->streamImageFile);

    init = resourceCfg->adrv9001Init;
    armImagePath = (char *)resourceCfg->adrv9001PlatformFiles->armImageFile;
    streamImagePath = (char *)resourceCfg->adrv9001PlatformFiles->streamImageFile;

    ADI_EXPECT(adi_adrv9001_arm_AhbSpiBridge_Enable, device);

    /* Disable SPI Logging */
    adi_common_LogLevelGet(&device->common, &origLogLevel);
    ADI_ERROR_RETURN(device->common.error.newAction);

    logLevelNoSpi = origLogLevel & ~ADI_COMMON_LOG_SPI; /*Disable SPI logging*/

    adi_common_LogLevelSet(&device->common, logLevelNoSpi);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Load Stream binary image from a .bin file */
    if (resourceCfg->adrv9001PlatformFiles->streamImageFile[0] != 0)
    {
        ADI_EXPECT(adi_adrv9001_Utilities_StreamImage_Load, device, streamImagePath);
    }
    else
    {
        ADI_EXPECT(adi_adrv9001_Utilities_StreamImageBinary_Program,
                   device,
                   resourceCfg->radioCtrlInit->streamImageBinary);
    }

    /* Load ARM binary image */
    ADI_EXPECT(adi_adrv9001_Utilities_ArmImage_Load, device, armImagePath);

    /* Load ARM profile */
    ADI_EXPECT(adi_adrv9001_arm_Profile_Write, device, init);

    /* Load PFIR coefficients */
    ADI_EXPECT(adi_adrv9001_arm_PfirProfiles_Write, device, init);

    ADI_EXPECT(adi_adrv9001_Utilities_Tables_Load, device, resourceCfg);

    /* ARM Bootup */
    ADI_EXPECT(adi_adrv9001_arm_Start, device);

    /* ARM Bootup Status Check (Non Broadcast mode) */
#if ADI_ADRV9001_PRE_MCS_BROADCAST_DISABLE > 0
    ADI_EXPECT(adi_adrv9001_arm_StartStatus_Check, device, ADI_ADRV9001_GETARMBOOTUP_TIMEOUT_US);
#endif

    /* Enable SPI logging */
    adi_common_LogLevelSet(&device->common, origLogLevel);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Save whether we are in FH mode to state */
    device->devStateInfo.frequencyHoppingEnabled = init->sysConfig.fhModeOn;

    ADI_API_RETURN(device);
}

#ifdef ADI_FILESYSTEM_AVAILABLE
int32_t adi_adrv9001_Utilities_RxGainTable_Load(adi_adrv9001_Device_t *device, const char *rxGainTablePath, uint32_t rxChannelMask)
{
    static const uint8_t NUM_COLUMNS = 7;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t minGainIndex = 0;
    uint8_t maxGainIndex = 0;
    uint8_t prevGainIndex = 0;
    uint8_t gainIndex = 0;
    uint8_t tiaControl = 0;
    uint8_t adcControl = 0;
    uint16_t lineCount = 0;
    FILE *rxGainTableFilePointer = NULL;
    char rxGainTableLineBuffer[ADI_ADRV9001_LINE_BUFFER_SIZE];
    char headerStr1[ADI_ADRV9001_HEADER_BUFFER_SIZE];
    char headerStr2[ADI_ADRV9001_HEADER_BUFFER_SIZE];
    char headerStr3[ADI_ADRV9001_HEADER_BUFFER_SIZE];
    char headerStr4[ADI_ADRV9001_HEADER_BUFFER_SIZE];
    char headerStr5[ADI_ADRV9001_HEADER_BUFFER_SIZE];
    char headerStr6[ADI_ADRV9001_HEADER_BUFFER_SIZE];
    char headerStr7[ADI_ADRV9001_HEADER_BUFFER_SIZE];
    static adi_adrv9001_RxGainTableRow_t rxGainTableRowBuffer[ADI_ADRV9001_RX_GAIN_TABLE_SIZE_ROWS];

    /* Don't load the Rx gain tables if the rxChannelMask is 0 */
    if (rxChannelMask == 0)
    {
        return recoveryAction;
    }

    /* Check device pointer is not null */
    ADI_API_ENTRY_PTR_EXPECT(device, rxGainTablePath);

    /*Open Rx Gain Table csv file*/
#ifdef __GNUC__
    rxGainTableFilePointer = fopen(rxGainTablePath, "r");

    if (rxGainTableFilePointer == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            rxGainTablePath,
            "Invalid Rx Gain Table csv file path encountered while attempting to load Rx Gain Table");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#else
    if (fopen_s(&rxGainTableFilePointer, rxGainTablePath, "r") != 0)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            rxGainTablePath,
            "Unable to open Rx Gain Table csv file. Please check if the path is correct or the file is open in another program");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif

    /*Check for empty Rx Gain Table*/
    if (fgets(rxGainTableLineBuffer, sizeof(rxGainTableLineBuffer), rxGainTableFilePointer) != NULL)
    {
#ifdef __GNUC__
        if (sscanf(rxGainTableLineBuffer,
            "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",
            headerStr1,
            headerStr2,
            headerStr3,
            headerStr4,
            headerStr5,
            headerStr6,
            headerStr7) != NUM_COLUMNS)
#else
        if (sscanf_s(rxGainTableLineBuffer,
            "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",
             headerStr1,
             (uint32_t)sizeof(headerStr1),
             headerStr2,
             (uint32_t)sizeof(headerStr2),
             headerStr3,
             (uint32_t)sizeof(headerStr3),
             headerStr4,
             (uint32_t)sizeof(headerStr4),
             headerStr5,
             (uint32_t)sizeof(headerStr5),
             headerStr6,
             (uint32_t)sizeof(headerStr6),
             headerStr7,
             (uint32_t)sizeof(headerStr7)) != NUM_COLUMNS)
#endif
        {
             ADI_ERROR_REPORT(&device->common,
                 ADI_COMMON_ERRSRC_API,
                 ADI_COMMON_ERR_INV_PARAM,
                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                 rxGainTablePath,
                 "Invalid Rx Gain Table format encountered while attempting to load Rx Gain Table");
                ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, rxGainTableFilePointer);
        }

        /*Verify that Gain Table Format is correct*/
        if (strstr(headerStr1, "Gain Index") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                rxGainTablePath,
                "Expected 'Gain Index' to be the first column in Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, rxGainTableFilePointer);
        }

        if (strstr(headerStr2, "FE Control Word") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                rxGainTablePath,
                "Expected 'FE Control Word' to be the second column in Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, rxGainTableFilePointer);
        }

        if (strstr(headerStr3, "TIA Control") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                rxGainTablePath,
                "Expected 'TIA Control' to be the third column in Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, rxGainTableFilePointer);
        }

        if (strstr(headerStr4, "ADC Control") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                rxGainTablePath,
                "Expected 'ADC Control' to be the fourth column in Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, rxGainTableFilePointer);
        }

        if (strstr(headerStr5, "Ext Control") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                rxGainTablePath,
                "Expected 'Ext Control' to be the fifth column in Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, rxGainTableFilePointer);
        }

        if (strstr(headerStr6, "Phase Offset") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                rxGainTablePath,
                "Expected 'Phase Offset' to be the sixth column in Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, rxGainTableFilePointer);
        }

        if (strstr(headerStr7, "Digital Gain") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                rxGainTablePath,
                "Expected Digital Gain to be the seventh column in Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, rxGainTableFilePointer);
        }

        /*Loop until the gain table end is reached or no. of lines scanned exceeds maximum*/
        while ((fgets(rxGainTableLineBuffer, sizeof(rxGainTableLineBuffer), rxGainTableFilePointer) != NULL) &&
               (lineCount <  ADI_ADRV9001_RX_GAIN_TABLE_SIZE_ROWS))
        {
#ifdef __GNUC__
            if (sscanf(rxGainTableLineBuffer,
                "%hhu,%hhu,%hhu,%hhu,%hhu,%hu,%hd",
                &gainIndex,
                &rxGainTableRowBuffer[lineCount].rxFeGain,
                &tiaControl,
                &adcControl,
                &rxGainTableRowBuffer[lineCount].extControl,
                &rxGainTableRowBuffer[lineCount].phaseOffset,
                &rxGainTableRowBuffer[lineCount].digGain) != NUM_COLUMNS)
#else
            if (sscanf_s(rxGainTableLineBuffer,
                "%hhu,%hhu,%hhu,%hhu,%hhu,%hu,%hd",
                 &gainIndex,
                 &rxGainTableRowBuffer[lineCount].rxFeGain,
                 &tiaControl,
                 &adcControl,
                 &rxGainTableRowBuffer[lineCount].extControl,
                 &rxGainTableRowBuffer[lineCount].phaseOffset,
                 &rxGainTableRowBuffer[lineCount].digGain) != NUM_COLUMNS)
#endif
            {
                ADI_ERROR_REPORT(&device->common,
                    ADI_COMMON_ERRSRC_API,
                    ADI_COMMON_ERR_INV_PARAM,
                    ADI_COMMON_ACT_ERR_CHECK_PARAM,
                    lineCount,
                    "Insufficient entries in Rx gain table row entry");
                    ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, rxGainTableFilePointer);
            }

            rxGainTableRowBuffer[lineCount].adcTiaGain = ((adcControl << 1) | tiaControl);

            if (lineCount == 0)
            {
                minGainIndex = gainIndex;
            }
            else
            {
                /*Check that gain indices are arranged in ascending order*/
                if (prevGainIndex != (gainIndex - 1))
                {
                    ADI_ERROR_REPORT(&device->common,
                        ADI_COMMON_ERRSRC_API,
                        ADI_COMMON_ERR_INV_PARAM,
                        ADI_COMMON_ACT_ERR_CHECK_PARAM,
                        gainIndex,
                        "Gain indices not arranged in ascending order in Rx Gain Table file");
                    ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, rxGainTableFilePointer);
                }
            }

            prevGainIndex = gainIndex;
            lineCount++;
        }

        maxGainIndex = gainIndex;
        recoveryAction = adi_adrv9001_Rx_GainTable_Write(device, rxChannelMask, maxGainIndex, &rxGainTableRowBuffer[0], lineCount);
        if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                rxGainTablePath,
                "Unable to Write Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, rxGainTableFilePointer);
        }

        recoveryAction = adi_adrv9001_Rx_MinMaxGainIndex_Set(device, rxChannelMask, minGainIndex, maxGainIndex);
        if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                rxGainTablePath,
                "Unable to set Rx gain table min/max gain indices.");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, rxGainTableFilePointer);
        }
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            rxGainTablePath,
            "Empty Rx Gain Table encountered");
        /* no need for ADI_ERROR_CLOSE_RETURN here as the file will be closed below anyway */
    }

    /* Close Rx Gain Table csv file */
    if (fclose(rxGainTableFilePointer) < 0)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            rxGainTablePath,
            "Fatal error while trying to close Rx Gain Table csv file. Possible memory shortage while flushing / other I/O errors.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adi_adrv9001_Utilities_TxAttenTable_Load(adi_adrv9001_Device_t *device, const char *txAttenTablePath, uint32_t txChannelMask)
{
    static const uint8_t NUM_COLUMNS = 3;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint16_t prevAttenIndex = 0;
    uint16_t attenIndex = 0;
    uint16_t minAttenIndex = 0;
    uint16_t maxAttenIndex = 0;
    uint16_t lineCount = 0;
    uint16_t tableSize = 0;
    FILE *txAttenTableFilePointer = NULL;
    char txAttenTableLineBuffer[ADI_ADRV9001_LINE_BUFFER_SIZE];
    char headerStr1[ADI_ADRV9001_HEADER_BUFFER_SIZE] ;
    char headerStr2[ADI_ADRV9001_HEADER_BUFFER_SIZE];
    char headerStr3[ADI_ADRV9001_HEADER_BUFFER_SIZE];
    static adi_adrv9001_TxAttenTableRow_t txAttenTableRowBuffer[ADI_ADRV9001_TX_ATTEN_TABLE_SIZE_ROWS];

    /* Don't load the Tx attenuation tables if the txChannelMask is 0 */
    if (txChannelMask == 0)
    {
        return recoveryAction;
    }

    /* Check device pointer is not null */
    ADI_API_ENTRY_PTR_EXPECT(device, txAttenTablePath);

    maxAttenIndex = ADRV9001_TX_ATTEN_TABLE_MAX;

    /*Open Tx Atten Table csv file*/
#ifdef __GNUC__
    txAttenTableFilePointer = fopen(txAttenTablePath, "r");

    if (txAttenTableFilePointer == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            txAttenTablePath,
            "Invalid Tx Atten Table csv file path encountered while attempting to load Tx Atten Table");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#else
    if (fopen_s(&txAttenTableFilePointer, txAttenTablePath, "r") != 0)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            txAttenTablePath,
            "Unable to open Tx Atten Table csv file. Please check if the path is correct or the file is open in another program");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif

    /*Check for empty Tx Atten Table*/
    if (fgets(txAttenTableLineBuffer, sizeof(txAttenTableLineBuffer), txAttenTableFilePointer) != NULL)
    {
#ifdef __GNUC__
        if (sscanf(txAttenTableLineBuffer, "%[^,],%[^,],%[^\n]", headerStr1, headerStr2, headerStr3) != NUM_COLUMNS)
#else
        if (sscanf_s(txAttenTableLineBuffer,
                "%[^,],%[^,],%[^\n]",
                headerStr1,
                (uint32_t)sizeof(headerStr1),
                headerStr2,
                (uint32_t)sizeof(headerStr2),
                headerStr3,
                (uint32_t)sizeof(headerStr3)) != NUM_COLUMNS)
#endif
        {
            ADI_ERROR_REPORT(&device->common,
                 ADI_COMMON_ERRSRC_API,
                 ADI_COMMON_ERR_INV_PARAM,
                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                 txAttenTablePath,
                 "Invalid Tx Atten Table format encountered while attempting to load Tx Atten Table");
             ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, txAttenTableFilePointer);
        }

        /*Verify that Gain Table Format is correct*/
        if (strstr(headerStr1, "Tx Atten Index") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                txAttenTablePath,
                "Expected 'Tx Atten Index' to be the first column in Tx Atten table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, txAttenTableFilePointer);
        }

        if (strstr(headerStr2, "Tx Atten Hp") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                txAttenTablePath,
                "Expected 'Tx Atten Hp' to be the second column in Tx Atten table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, txAttenTableFilePointer);
        }

        if (strstr(headerStr3, "Tx Atten Mult") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                txAttenTablePath,
                "Expected 'Tx Atten Mult' to be the third column in Tx Atten table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, txAttenTableFilePointer);
        }

        /*Loop until the atten table end is reached or no. of lines scanned exceeds maximum*/
        while ((fgets(txAttenTableLineBuffer, sizeof(txAttenTableLineBuffer), txAttenTableFilePointer) != NULL) &&
               (lineCount < maxAttenIndex))
        {
#ifdef __GNUC__
            if (sscanf(txAttenTableLineBuffer,
                "%hu,%hhu,%hu",
                &attenIndex,
                &txAttenTableRowBuffer[lineCount].txAttenHp,
                &txAttenTableRowBuffer[lineCount].txAttenMult) != NUM_COLUMNS)
#else
            if (sscanf_s(txAttenTableLineBuffer,
                    "%hu,%hhu,%hu",
                    &attenIndex,
                    &txAttenTableRowBuffer[lineCount].txAttenHp,
                    &txAttenTableRowBuffer[lineCount].txAttenMult) != NUM_COLUMNS)
#endif
            {
                ADI_ERROR_REPORT(&device->common,
                    ADI_COMMON_ERRSRC_API,
                    ADI_COMMON_ERR_INV_PARAM,
                    ADI_COMMON_ACT_ERR_CHECK_PARAM,
                    lineCount,
                    "Insufficient entries in Tx atten table row entry");
                    ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, txAttenTableFilePointer);
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
                    ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, txAttenTableFilePointer);
                }
            }

            prevAttenIndex = attenIndex;
            lineCount++;
        }

        tableSize = attenIndex - minAttenIndex + 1;

        recoveryAction = adi_adrv9001_Tx_AttenuationTable_Write(device, txChannelMask, minAttenIndex, &txAttenTableRowBuffer[0], tableSize);
        if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                txAttenTablePath,
                "Unable to write Tx Atten Table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, txAttenTableFilePointer);
        }

    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            txAttenTablePath,
            "Empty Tx Atten Table encountered");
        /* no need for ADI_ERROR_CLOSE_RETURN here as the file will be closed below anyway */
    }

    /* Close Tx Atten Table csv file */
    if (fclose(txAttenTableFilePointer) < 0)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            txAttenTablePath,
            "Fatal error while trying to close Tx Atten Table csv file. Possible memory shortage while flushing / other I/O errors.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}
#else
int32_t adi_adrv9001_Utilities_RxGainTable_Load(adi_adrv9001_Device_t *device, const char *rxGainTablePath, uint32_t rxChannelMask)
{
    uint8_t minGainIndex = 0;
    uint8_t maxGainIndex = 0;
    uint8_t prevGainIndex = 0;
    uint8_t gainIndex = 0;
    uint8_t tiaControl = 0;
    uint8_t adcControl = 0;
    uint16_t lineCount = 0;

    static adi_adrv9001_RxGainTableRow_t rxGainTableRowBuffer[ADI_ADRV9001_RX_GAIN_TABLE_SIZE_ROWS];
    static const uint8_t NUM_COLUMNS = 7;

    static const uint8_t MIN_GAIN_INDEX_RX = 183;
    static const uint8_t MIN_GAIN_INDEX_ORX = 2;
    static const uint8_t MAX_GAIN_INDEX_ORX = 14;

    int32_t returnTableEntry = NUM_COLUMNS;

    /* Check device pointer is not null */
    ADI_API_ENTRY_PTR_EXPECT(device, rxGainTablePath);

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

        if (gainIndex < MIN_GAIN_INDEX_RX)
        {
            if ((gainIndex < MIN_GAIN_INDEX_ORX) ||
                (gainIndex > MAX_GAIN_INDEX_ORX))
            {
                break;
            }
        }

        if (lineCount == 0)
        {
            minGainIndex = gainIndex;
        }
        else
        {
            /*Check that gain indices are arranged in ascending order*/
            if (prevGainIndex != (gainIndex - 1))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 gainIndex,
                                 "Gain indices not arranged in ascending order in Rx Gain Table file");
            }
        }

        prevGainIndex = gainIndex;
        lineCount++;
    }

    maxGainIndex = prevGainIndex;
    ADI_EXPECT(adi_adrv9001_Rx_GainTable_Write, device, rxChannelMask, maxGainIndex, &rxGainTableRowBuffer[0], lineCount);

    ADI_EXPECT(adi_adrv9001_Rx_MinMaxGainIndex_Set, device, rxChannelMask, minGainIndex, maxGainIndex);

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
    ADI_API_ENTRY_PTR_EXPECT(device, txAttenTablePath);

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
#endif

#ifdef ADI_FILESYSTEM_AVAILABLE
int32_t adi_adrv9001_Utilities_ArmMemory_Dump(adi_adrv9001_Device_t *device, const char *binaryFilename)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    FILE *ofp;
    uint32_t byteCount = 0;
    uint32_t offset = 0;
    bool armMailboxBusy = false;
    uint8_t cmdStatusByte = 0;
    uint8_t exceptionArray[4] = { 0 };
    uint8_t extArray[4] = { 0 };
    uint32_t exceptionValue = 0;
    uint8_t armException = 0;

    uint8_t binaryRead[ADI_ADRV9001_MEM_DUMP_CHUNK_SIZE + 10] = { 0 };

    static const uint32_t armExceptionAddr = 0x20000214; /* Exception Flag Memory */

    ADI_API_ENTRY_PTR_EXPECT(device, binaryFilename);

#ifdef ADRV9001_INIT_DEBUG
    printf("open %s (%s) \n", binaryFilename, "wb");
#endif

#ifdef __GNUC__
    ofp = fopen(binaryFilename, "wb");
#else
    fopen_s(&ofp, binaryFilename, "wb");
#endif

    if (ofp == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            ofp,
            "Unable to open binary image file. Please check if the path is correct");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check if exception has occurred */
    recoveryAction = adi_adrv9001_arm_Memory_Read(device, armExceptionAddr, &exceptionArray[0], 4, ADRV9001_ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_REPORT(&device->common,
        ADI_COMMON_ERRSRC_API,
        ADI_COMMON_ERR_API_FAIL,
        recoveryAction,
        NULL,
        "Unable to Read ARM memory");
    ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, ofp);

    exceptionValue = (uint32_t)(exceptionArray[0] | (exceptionArray[1] << 8) | (exceptionArray[2] << 16) | (exceptionArray[3] << 24));

    if (exceptionValue == 0)
    {
        /* Force an exception during ARM MEM dump for more useful information        */

        recoveryAction = adi_adrv9001_arm_MailboxBusy_Get(device, &armMailboxBusy);
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Invalid Get for armMailboxBusy");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, ofp);

        if (armMailboxBusy == false)
        {
            extArray[0] = ADRV9001_ARM_OBJECTID_ARM_EXCEPTION;
            recoveryAction = adi_adrv9001_arm_Cmd_Write(device, ADRV9001_ARM_SET_OPCODE, &extArray[0], 1);
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_API_FAIL,
                recoveryAction,
                NULL,
                "Unable to Process ARM command");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, ofp);

            recoveryAction = adi_adrv9001_arm_CmdStatus_Wait(device,
                                                             ADRV9001_ARM_SET_OPCODE,
                                                             &cmdStatusByte,
                                                             ADI_ADRV9001_WRITEARMEXCEPTION_TIMEOUT_US,
                                                             ADI_ADRV9001_WRITEARMEXCEPTION_INTERVAL_US);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             recoveryAction,
                             NULL,
                             "Unable to Proccess ARM command");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, ofp);

            recoveryAction = adi_adrv9001_arm_Memory_Read(device, armExceptionAddr, &exceptionArray[0], 4, ADRV9001_ARM_MEM_READ_AUTOINCR);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             recoveryAction,
                             NULL,
                             "Unable to Read ARM memory");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, ofp);

            exceptionValue = (uint32_t)(exceptionArray[0] | (exceptionArray[1] << 8) | (exceptionArray[2] << 16) | (exceptionArray[3] << 24));
            if (exceptionValue > 0)
            {
                armException = 1;
            }
        }
    }

    /* Program Binary */
    for (offset = ADRV9001_ADDR_ARM_START_PROG; offset < ADRV9001_ADDR_ARM_END_PROG; offset += ADI_ADRV9001_MEM_DUMP_CHUNK_SIZE)
    {
        if (offset < (ADRV9001_ADDR_ARM_END_PROG - ADI_ADRV9001_MEM_DUMP_CHUNK_SIZE))
        {
            byteCount = ADI_ADRV9001_MEM_DUMP_CHUNK_SIZE;
        }
        else
        {
            byteCount = ADRV9001_ADDR_ARM_END_PROG + 1 - offset;
        }

        recoveryAction = adi_adrv9001_arm_Memory_Read(device, offset, binaryRead, byteCount, ADRV9001_ARM_MEM_READ_AUTOINCR);
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            recoveryAction,
            NULL,
            "Unable to Read ARM memory");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, ofp);

        if (fwrite(binaryRead, 1, byteCount, ofp) != byteCount)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                ofp,
                "Fatal error while trying to write a binary file.");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, ofp);
        }
    }

    /* Data Binary */
    for (offset = ADRV9001_ADDR_ARM_START_DATA; offset < ADRV9001_ADDR_ARM_END_DATA; offset += ADI_ADRV9001_MEM_DUMP_CHUNK_SIZE)
    {
        if (offset < (ADRV9001_ADDR_ARM_END_DATA - ADI_ADRV9001_MEM_DUMP_CHUNK_SIZE))
        {
            byteCount = ADI_ADRV9001_MEM_DUMP_CHUNK_SIZE;
        }
        else
        {
            byteCount = ADRV9001_ADDR_ARM_END_DATA + 1 - offset;
        }

        recoveryAction = adi_adrv9001_arm_Memory_Read(device, offset, binaryRead, byteCount, ADRV9001_ARM_MEM_READ_AUTOINCR);
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            recoveryAction,
            NULL,
            "Unable to Read ARM memory");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, ofp);

        if (fwrite(binaryRead, 1, byteCount, ofp) != byteCount)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                ofp,
                "Fatal error while trying to write a binary file.");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, ofp);
        }
    }

    if (armException > 0)
    {
        /*if we forced an exception, clear the exception so the ARM will continue to run*/
        exceptionArray[0] = 0;
        exceptionArray[1] = 0;
        exceptionArray[2] = 0;
        exceptionArray[3] = 0;

        recoveryAction = adi_adrv9001_arm_Memory_Write(device, armExceptionAddr, &exceptionArray[0], 4);
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            recoveryAction,
            NULL,
            "Unable to Write ARM memory");
        /* no need for ADI_ERROR_CLOSE_RETURN here as the file will be closed below anyway */
    }

    /*Close ARM binary file*/
    if (fclose(ofp) < 0)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            ofp,
            "Fatal error while trying to close a binary file. Possible memory shortage while flushing / other I/O errors.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_API_RETURN(device);
}
#endif

int32_t adi_adrv9001_Utilities_InitRadio_Load(adi_adrv9001_Device_t *device,
                                              adi_adrv9001_ResourceCfg_t *initConfig,
                                              uint8_t channelMask)
{
    adi_adrv9001_SsiType_e ssiType = ADI_ADRV9001_SSI_TYPE_DISABLE;

    /* Check device pointer is not null */
    ADI_API_ENTRY_PTR_EXPECT(device, initConfig);
    ADI_NULL_PTR_RETURN(&device->common, initConfig->radioCtrlInit);
    ADI_NULL_PTR_RETURN(&device->common, initConfig->adrv9001PlatformFiles);

    ssiType = initConfig->adrv9001Init->rx.rxChannelCfg[0].profile.rxSsiConfig.ssiType |
        initConfig->adrv9001Init->rx.rxChannelCfg[1].profile.rxSsiConfig.ssiType |
        initConfig->adrv9001Init->tx.txProfile[0].txSsiConfig.ssiType |
        initConfig->adrv9001Init->tx.txProfile[1].txSsiConfig.ssiType;

    /* LVDS forced mode */
    ADI_MSG_EXPECT("Error programming SSI delay configuration", adi_adrv9001_Ssi_Delay_Configure, device, ssiType, &(initConfig->radioCtrlInit->ssiConfig));

    ADI_EXPECT(adi_adrv9001_gpio_ControlInit_Configure, device, &initConfig->radioCtrlInit->gpioCtrlInitCfg);

    /*Initialize radio control. This is required to run before running init cals*/
    ADI_EXPECT(adrv9001_RadioCtrlInit, device, initConfig->radioCtrlInit, channelMask, ssiType);

    if (ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, ADI_ADRV9001_TX1) &&
        (true == initConfig->radioCtrlInit->txDpdInit[0].enable))
    {
        ADI_EXPECT(adi_adrv9001_dpd_Initial_Configure, device, ADI_CHANNEL_1, &initConfig->radioCtrlInit->txDpdInit[0]);
    }
    if (ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, ADI_ADRV9001_TX2) &&
        (true == initConfig->radioCtrlInit->txDpdInit[1].enable))
    {
        ADI_EXPECT(adi_adrv9001_dpd_Initial_Configure, device, ADI_CHANNEL_2, &initConfig->radioCtrlInit->txDpdInit[1]);
    }
    /* Load frequency hopping configuration if set in the device profile */
    if (true == initConfig->adrv9001Init->sysConfig.fhModeOn)
    {
        /* Load configuration */
        ADI_EXPECT(adi_adrv9001_fh_Configure, device, &initConfig->radioCtrlInit->fhConfig);
    }

    /* Send SYSTEM_CONFIG mailbox command to ARM */
    ADI_EXPECT(adi_adrv9001_arm_System_Program, device, channelMask);

    ADI_API_RETURN(device);
}