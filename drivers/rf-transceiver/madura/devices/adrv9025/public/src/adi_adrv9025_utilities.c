// SPDX-License-Identifier: GPL-2.0
/**
* \file adi_adrv9025_utilities.c
* \brief Contains Utility features related function implementation defined in
* adi_adrv9025_utilities.h
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/string.h>
#endif
#include <string.h>
#include "adi_adrv9025_utilities.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_rx.h"
#include "adi_adrv9025_cpu.h"
#include "adi_adrv9025_tx.h"
#include "adi_adrv9025_radioctrl.h"
#include "adi_adrv9025_data_interface.h"
#include "adi_adrv9025_cals.h"
#include "adi_adrv9025_cals_types.h"
#include "adi_adrv9025_gpio.h"
#include "adi_adrv9025_arm.h"
#include "../../private/include/adrv9025_utilities.h"
#include "../../private/include/adrv9025_init.h"
#include "../../private/include/adrv9025_tx.h"
#include "../../private/include/adrv9025_cpu.h"
#include "../../private/include/adrv9025_gpio.h"
#include "../../private/include/adrv9025_cpu_macros.h"
#include "../../private/include/adrv9025_bf_hal.h"
#include "../../private/include/adrv9025_crc32.h"

#include "adi_platform.h"
#ifdef _RELEASE_BUILD_
    #line __LINE__ "adi_adrv9025_utilities.c"
#endif

#define ADI_ADRV9025_CPU_C_BINARY_IMAGE_FILE_SIZE_BYTES (7*32*1024u) /* 7 blocks of 32kB memory */
#define ADI_ADRV9025_CPU_D_BINARY_IMAGE_FILE_SIZE_BYTES (3*32*1024u) /* 3 blocks of 32kB memory */
#define ADI_ADRV9025_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES (22*1024)
#define ADI_ADRV9025_RX_GAIN_TABLE_SIZE_ROWS 256
#define ADI_ADRV9025_TX_ATTEN_TABLE_SIZE_ROWS 1024

#if (OS_Windows == 64) && (_LABVIEW_INSTALLED != 0)

int32_t adi_adrv9025_ArmImageLoad(adi_adrv9025_Device_t *device, const char *armImagePath)
{
    return adi_adrv9025_CpuImageLoad(device, armImagePath);;
}

int32_t adi_adrv9025_CpuImageLoad(adi_adrv9025_Device_t *device, const char *fwImagePaths)
{
    static const size_t BIN_ELEMENT_SIZE = 1;

    int32_t retVal = ADI_COMMON_ACT_NO_ACTION;
    FILE *fwImageFilePointer = NULL;
    uint32_t fileSize = 0;
    //TODO: Load in smaller chunks
    static uint8_t fwBinaryImageBuffer[ADI_ADRV9025_CPU_C_BINARY_IMAGE_FILE_SIZE_BYTES];
    uint32_t i = 0;
    char paths[ADRV9025_MAX_CPUS][ADRV9025_MAX_CPU_FW_PATH] = { { 0 } };
    const char *temp = NULL;
    uint32_t len1 = 0u;
    uint32_t len2 = 0u;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    /* the paths might contain 2 paths separated by semi colon,
     * the first one should be for cpu-c and the second one,
     * if exists, is for the 2nd cpu-d */
    if ((temp = strchr(fwImagePaths, ADRV9025_CPU_FW_PATH_SEP)) == NULL)
    {
        strncpy(paths[0], fwImagePaths, ADRV9025_MAX_CPU_FW_PATH - 1u);
        paths[1][0] = 0u;
    }
    else
    {
        len1 = (uint32_t)(temp - fwImagePaths);
        len2 = (uint32_t)(strlen(temp + 1u));
        strncpy(paths[0], fwImagePaths, (len1 > ADRV9025_MAX_CPU_FW_PATH) ? ADRV9025_MAX_CPU_FW_PATH : len1);
        strncpy(paths[1], temp + 1u, (len2 > ADRV9025_MAX_CPU_FW_PATH) ? ADRV9025_MAX_CPU_FW_PATH : len2);
    }
    paths[0][ADRV9025_MAX_CPU_FW_PATH - 1u] = 0u;
    paths[1][ADRV9025_MAX_CPU_FW_PATH - 1u] = 0u;

    for (i = 0; i < ADRV9025_MAX_CPUS; i++)
    {
        char *fwImagePath = paths[i];

        /*Open FW binary file*/
#ifdef __GNUC__
        fwImageFilePointer = fopen(fwImagePath, "rb");
#else
        if (fopen_s(&fwImageFilePointer, fwImagePath, "rb") != 0)
        {
            if (i == 0)
            {
                ADI_ERROR_REPORT(&device->common,
                    ADI_COMMON_ERRSRC_API,
                    ADI_COMMON_ERR_INV_PARAM,
                    ADI_COMMON_ACT_ERR_CHECK_PARAM,
                    fwImagePath,
                    "Unable to open CPU FW binary image file. Please check if the path is correct");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else
            {
                continue;
            }
        }
#endif

        if (fwImageFilePointer == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                fwImagePath,
                "Invalid CPU FW binary image path encountered while attempting to load CPU FW binary image");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Determine file size*/
        if (fseek(fwImageFilePointer, 0, SEEK_END) < 0)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                fwImageFilePointer,
                "Unable to move file descriptor to the end of the CPU FW binary image file");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, fwImageFilePointer);
        }

        fileSize = ftell(fwImageFilePointer);

        /*Check that FW binary file is not empty*/
        if (fileSize == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                fwImagePath,
                "Empty FW binary image file encountered while attempting to load FW binary image");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, fwImageFilePointer);
        }

        /*Check that FW binary file size does not exceed maximum size*/
        if (fileSize > (((adi_adrv9025_CpuType_e)i == ADI_ADRV9025_CPU_TYPE_C) ? ADI_ADRV9025_CPU_C_BINARY_IMAGE_FILE_SIZE_BYTES : ADI_ADRV9025_CPU_D_BINARY_IMAGE_FILE_SIZE_BYTES))
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                fwImagePath,
                "FW binary image file exceeds maximum FW binary image size");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, fwImageFilePointer);
        }

        /*Check that size of the file is a multiple of 4*/
        if ((fileSize % 4) != 0)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                fwImagePath,
                "FW binary image file is expected to be a multiple of 4");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, fwImageFilePointer);
        }

        /*Rewind the file pointer to beginning of the file*/
        if (fseek(fwImageFilePointer, 0, SEEK_SET) < 0)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                fwImageFilePointer,
                "Unable to move file descriptor to the beginning of the FW binary image file");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, fwImageFilePointer);
        }

        /*Read FW binary file*/
        if (fread(&fwBinaryImageBuffer[0], BIN_ELEMENT_SIZE, fileSize, fwImageFilePointer) < fileSize)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                fwImagePath,
                "Fatal error while reading FW binary file. Possible memory shortage");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, fwImageFilePointer);
        }

        /*Write the FW binary*/
        retVal = adi_adrv9025_CpuImageWrite(device, (adi_adrv9025_CpuType_e)i, 0, &fwBinaryImageBuffer[0], fileSize);

        /*Close FW binary file*/
        if (fclose(fwImageFilePointer) < 0)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                armImagePath,
                "Fatal error while trying to close FW binary file. Possible memory shortage while flushing / other I/O errors.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return retVal;
}

int32_t adi_adrv9025_StreamImageLoad(adi_adrv9025_Device_t *device, const char *streamImagePath)
{
    static const size_t BIN_ELEMENT_SIZE = 1;

    int32_t retVal = ADI_COMMON_ACT_NO_ACTION;
    FILE *streamImageFilePointer = NULL;
    uint32_t fileSize = 0;
    //TODO: Load in smaller chunks
    static uint8_t streamBinaryImageBuffer[ADI_ADRV9025_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES];

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    /*Open ARM binary file*/
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

    /*Check that Stream binary file is not empty*/
    if (fileSize == 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, streamImagePath,
            "Empty Stream binary image file encountered while attempting to load the Stream processor");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, streamImageFilePointer);
    }

    /*Check that Stream binary file size does not exceed maximum size*/
    if (fileSize > ADI_ADRV9025_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, streamImagePath,
            "Stream binary image file exceeds maximum Stream binary image size");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, streamImageFilePointer);
    }

    /*Check that size of the file is a multiple of 4*/
    if ((fileSize % 4) != 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, streamImagePath,
            "Stream binary image file is expected to be a multiple of 4");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, streamImageFilePointer);
    }

    /*Rewind the file pointer to beginning of the file*/
    if (fseek(streamImageFilePointer, 0, SEEK_SET) < 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, streamImageFilePointer,
            "Unable to move file descriptor to the beginning of the stream binary image file");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, streamImageFilePointer);
    }

    /*Read Stream binary file*/
    if (fread(&streamBinaryImageBuffer[0], BIN_ELEMENT_SIZE, fileSize, streamImageFilePointer) < fileSize)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, streamImagePath,
            "Fatal error while reading stream binary file. Possible memory shortage");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, streamImageFilePointer);
    }

    /*Write Stream binary*/
    retVal = adi_adrv9025_StreamImageWrite(device, 0, &streamBinaryImageBuffer[0], fileSize);

    /*Close Stream binary file*/
    if (fclose(streamImageFilePointer) < 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, streamImagePath,
            "Fatal error while trying to close stream binary file. Possible memory shortage while flushing / other I/O errors.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }


    return retVal;
}

#else

int32_t adi_adrv9025_ArmImageLoad(adi_adrv9025_Device_t* device,
                                  const char*            armImagePath)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, armImagePath);

    return adi_adrv9025_CpuImageLoad(device,
                                     armImagePath);
}

int32_t adi_adrv9025_CpuImageLoad(adi_adrv9025_Device_t* device,
                                  const char*            fwImagePaths)
{
    static const size_t BIN_ELEMENT_SIZE = 1;

    FILE*       fwImageFilePointer = NULL;
    uint32_t    fileSize           = 0;
    uint32_t    numFileChunks      = 0;
    uint32_t    i                  = 0;
    uint32_t    j                  = 0;
    uint8_t     armBinaryImageBuffer[ADI_ADRV9025_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];
    char        paths[ADRV9025_MAX_CPUS][ADRV9025_MAX_CPU_FW_PATH] = {{0}};
    const char* temp                                               = NULL;
    uint32_t    len1                                               = 0u;
    uint32_t    len2                                               = 0u;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* the paths might contain 2 paths separated by semi colon,
     * the first one should be for cpu-c and the second one,
     * if exists, is for the 2nd cpu-d */
    if ((temp = strchr(fwImagePaths,
                       ADRV9025_CPU_FW_PATH_SEP)) == NULL)
    {
        strncpy(paths[0],
                fwImagePaths,
                ADRV9025_MAX_CPU_FW_PATH - 1u);
        paths[1][0] = 0u;
    }
    else
    {
        len1 = temp - fwImagePaths;
        len2 = strlen(temp + 1u);
        strncpy(paths[0],
                fwImagePaths,
                (len1 > ADRV9025_MAX_CPU_FW_PATH) ? ADRV9025_MAX_CPU_FW_PATH : len1);
        strncpy(paths[1],
                temp + 1u,
                (len2 > ADRV9025_MAX_CPU_FW_PATH) ? ADRV9025_MAX_CPU_FW_PATH : len2);
    }
    paths[0][ADRV9025_MAX_CPU_FW_PATH - 1u] = 0u;
    paths[1][ADRV9025_MAX_CPU_FW_PATH - 1u] = 0u;

    for (j = 0; j < ADRV9025_MAX_CPUS; j++)
    {
        char* fwImagePath = paths[j];

        /*Open ARM binary file*/
#ifdef __GNUC__
        fwImageFilePointer = fopen(fwImagePath,
                                   "rb");
#else
        if (fopen_s(&fwImageFilePointer, fwImagePath, "rb") != 0)
        {
            ADI_ERROR_REPORT(&device->common,  ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, fwImagePath,
                         "Unable to open FW binary image file. Please check if the path is correct");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
#endif

        if (fwImageFilePointer == NULL)
        {
            if (j == 0)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 fwImagePath,
                                 "Invalid FW binary image path encountered while attempting to load FW binary image");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else
            {
                continue;
            }
        }

        /*Determine file size*/
        if (fseek(fwImageFilePointer,
                  0,
                  SEEK_END) < 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             armImageFilePointer,
                             "Unable to move file descriptor to the end of the ARM binary image file");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   fwImageFilePointer);
        }

        fileSize      = ftell(fwImageFilePointer);
        numFileChunks = (uint32_t)(fileSize / ADI_ADRV9025_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES);

        /*Check that FW binary file is not empty*/
        if (fileSize == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             fwImagePath,
                             "Empty FW binary image file encountered while attempting to load FW binary image");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   fwImageFilePointer);
        }

        /*Check that FW binary file size does not exceed maximum size*/
        if (fileSize > (((adi_adrv9025_CpuType_e)j == ADI_ADRV9025_CPU_TYPE_C) ? ADI_ADRV9025_CPU_C_BINARY_IMAGE_FILE_SIZE_BYTES
                            : ADI_ADRV9025_CPU_D_BINARY_IMAGE_FILE_SIZE_BYTES))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             fwImagePath,
                             "FW binary image file exceeds maximum FW binary image size");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   fwImageFilePointer);
        }

        /*Check that size of the file is a multiple of 4*/
        if ((fileSize % 4) != 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             fwImagePath,
                             "FW binary image file is expected to be a multiple of 4");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   fwImageFilePointer);
        }

        /*Check that size of the file is divisible into equal sized chunks*/
        if ((fileSize % ADI_ADRV9025_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES) != 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             fwImagePath,
                             "FW binary image chunk size is expected to divide the stream file into equal chunks");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   fwImageFilePointer);
        }

        /*Rewind the file pointer to beginning of the file*/
        if (fseek(fwImageFilePointer,
                  0,
                  SEEK_SET) < 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             armImageFilePointer,
                             "Unable to move file descriptor to the beginning of the FW binary image file");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   fwImageFilePointer);
        }

        /*Read ARM binary file*/
        for (i = 0; i < numFileChunks; i++)
        {
            if (fread(&armBinaryImageBuffer[0],
                      BIN_ELEMENT_SIZE,
                      ADI_ADRV9025_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES,
                      fwImageFilePointer) < ADI_ADRV9025_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 armImagePath,
                                 "Fatal error while reading ARM binary file. Possible memory shortage");
                ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                       fwImageFilePointer);
            }

            /*Write the ARM binary chunk*/
            if ((adi_adrv9025_CpuImageWrite(device,
                                            (adi_adrv9025_CpuType_e)j,
                                            i * ADI_ADRV9025_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES,
                                            &armBinaryImageBuffer[0],
                                            ADI_ADRV9025_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES)) != ADI_COMMON_ACT_NO_ACTION)
            {
                ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                       fwImageFilePointer);
            }
        }

        /*Close ARM binary file*/
        if (fclose(fwImageFilePointer) < 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             fwImagePath,
                             "Fatal error while trying to close FW binary file");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_StreamImageLoad(adi_adrv9025_Device_t* device,
                                     const char*            streamImagePath)
{
    static const size_t BIN_ELEMENT_SIZE = 1;

    FILE*    streamImageFilePointer = NULL;
    uint32_t fileSize               = 0;
    uint32_t numFileChunks          = 0;
    uint32_t i                      = 0;
    uint8_t  streamBinaryImageBuffer[ADI_ADRV9025_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Open ARM binary file */
#ifdef __GNUC__
    streamImageFilePointer = fopen(streamImagePath,
                                   "rb");
#else
    if (fopen_s(&streamImageFilePointer, streamImagePath, "rb") != 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, armImagePath,
            "Unable to open stream binary image file. Please check if the path is correct");
        ADI_ERROR_RETURN(device->common.error.newAction);
}
#endif

    if (streamImageFilePointer == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         streamImagePath,
                         "Invalid Stream binary image path encountered while attempting to load Stream processor");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Determine file size*/
    if (fseek(streamImageFilePointer,
              0,
              SEEK_END) < 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         streamImageFilePointer,
                         "Unable to move file descriptor to the end of the stream binary image file");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                               streamImageFilePointer);
    }

    fileSize      = ftell(streamImageFilePointer);
    numFileChunks = (uint32_t)(fileSize / ADI_ADRV9025_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES);

    /* Check that Stream binary file is not empty */
    if (fileSize == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         streamImagePath,
                         "Empty Stream binary image file encountered while attempting to load the Stream processor");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                               streamImageFilePointer);
    }

    /* Check that Stream binary file size does not exceed maximum size */
    if (fileSize > ADI_ADRV9025_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         streamImagePath,
                         "Stream binary image file exceeds maximum Stream binary image size");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                               streamImageFilePointer);
    }

    /* Check that size of the file is divisible into equal sized chunks */
    if ((fileSize % numFileChunks) != 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         streamImagePath,
                         "Stream binary image chunk size is expected to divide the stream file into equal chunks");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                               streamImageFilePointer);
    }

    /* Check that size of the file is a multiple of 4 */
    if ((fileSize % 4) != 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         streamImagePath,
                         "Stream binary image file is expected to be a multiple of 4");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                               streamImageFilePointer);
    }

    /* Rewind the file pointer to beginning of the file */
    if (fseek(streamImageFilePointer,
              0,
              SEEK_SET) < 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         streamImageFilePointer,
                         "Unable to move file descriptor to the beginning of the stream binary image file");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                               streamImageFilePointer);
    }

    /* Read Stream binary file */
    for (i = 0; i < numFileChunks; i++)
    {
        if (fread(&streamBinaryImageBuffer[0],
                  BIN_ELEMENT_SIZE,
                  ADI_ADRV9025_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES,
                  streamImageFilePointer) < ADI_ADRV9025_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             streamImagePath,
                             "Fatal error while reading stream binary file. Possible memory shortage");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   streamImageFilePointer);
        }

        /* Write Stream binary */
        adi_adrv9025_StreamImageWrite(device,
                                      (i * ADI_ADRV9025_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES),
                                      &streamBinaryImageBuffer[0],
                                      ADI_ADRV9025_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES);
    }

    /* Close Stream binary file */
    if (fclose(streamImageFilePointer) < 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         streamImagePath,
                         "Fatal error while trying to close stream binary file. Possible memory shortage while flushing / other I/O errors.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}
#endif

int32_t adi_adrv9025_DmaMemoryRead(adi_adrv9025_Device_t* const device,
                                   const uint32_t               start_address,
                                   uint8_t                      buffer[],
                                   const uint32_t               size)
{
    int32_t recoveryAction = ADI_COMMON_ACT_ERR_CHECK_PARAM;

    /* Validate Device */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common,
                        buffer);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Validate Arguments */
    if (size > 0U) /* Size has to be greater than zero */
    {
        /* DMA Memory Read */
        recoveryAction = adrv9025_CpuDmaMemRead(device,
                                                start_address,
                                                &buffer[0],
                                                size,
                                                ADI_TRUE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         start_address,
                         "Unable to Read AHB Memory");
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         recoveryAction,
                         size,
                         "Invalid Array Size for Memory Read");
    }

    return recoveryAction;
}

int32_t adi_adrv9025_RxGainTableLoad(adi_adrv9025_Device_t* device,
                                     const char*            rxGainTablePath,
                                     uint32_t               rxChannelMask)
{
    static const uint8_t NUM_COLUMNS = 7;
#define LINE_BUFFER_SIZE 128
#define HEADER_BUFFER_SIZE 16

    uint8_t                              minGainIndex           = 0;
    uint8_t                              maxGainIndex           = 0;
    uint8_t                              prevGainIndex          = 0;
    uint8_t                              gainIndex              = 0;
    uint8_t                              tiaControl             = 0;
    uint8_t                              adcControl             = 0;
    uint16_t                             lineCount              = 0;
    FILE*                                rxGainTableFilePointer = NULL;
    char                                 rxGainTableLineBuffer[LINE_BUFFER_SIZE];
    char                                 headerStr1[HEADER_BUFFER_SIZE];
    char                                 headerStr2[HEADER_BUFFER_SIZE];
    char                                 headerStr3[HEADER_BUFFER_SIZE];
    char                                 headerStr4[HEADER_BUFFER_SIZE];
    char                                 headerStr5[HEADER_BUFFER_SIZE];
    char                                 headerStr6[HEADER_BUFFER_SIZE];
    char                                 headerStr7[HEADER_BUFFER_SIZE];
    static adi_adrv9025_RxGainTableRow_t rxGainTableRowBuffer[ADI_ADRV9025_RX_GAIN_TABLE_SIZE_ROWS];

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        rxGainTablePath);

    /*Open Rx Gain Table csv file*/
#ifdef __GNUC__
    rxGainTableFilePointer = fopen(rxGainTablePath,
                                   "r");
#else
    if (fopen_s(&rxGainTableFilePointer, rxGainTablePath, "r") != 0)
    {
        ADI_ERROR_REPORT(&device->common,  ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, rxGainTablePath,
                     "Unable to open Rx Gain Table csv file. Please check if the path is correct or the file is open in another program");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif

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

    /*Check for empty Rx Gain Table*/
    if (fgets(rxGainTableLineBuffer,
              sizeof(rxGainTableLineBuffer),
              rxGainTableFilePointer) != NULL)
    {
#ifdef __GNUC__
        if (sscanf(rxGainTableLineBuffer,
                   "%16[^,],%16[^,],%16[^,],%16[^,],%16[^,],%16[^,],%16[^\n]",
                   headerStr1,
                   headerStr2,
                   headerStr3,
                   headerStr4,
                   headerStr5,
                   headerStr6,
                   headerStr7) != NUM_COLUMNS)
#else
        if (sscanf_s(rxGainTableLineBuffer, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]", headerStr1, (uint32_t)sizeof(headerStr1),
                                                                                          headerStr2, (uint32_t)sizeof(headerStr2),
                                                                                          headerStr3, (uint32_t)sizeof(headerStr3),
                                                                                          headerStr4, (uint32_t)sizeof(headerStr4),
                                                                                          headerStr5, (uint32_t)sizeof(headerStr5),
                                                                                          headerStr6, (uint32_t)sizeof(headerStr6),
                                                                                          headerStr7, (uint32_t)sizeof(headerStr7)) != NUM_COLUMNS)
#endif
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGainTablePath,
                             "Invalid Rx Gain Table format encountered while attempting to load Rx Gain Table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   rxGainTableFilePointer);
        }

        /*Verify that Gain Table Format is correct*/
        if (strstr(headerStr1,
                   "Gain Index") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGainTablePath,
                             "Expected Gain Index to be the first column in Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   rxGainTableFilePointer);
        }

        if (strstr(headerStr2,
                   "FE Control Word") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGainTablePath,
                             "Expected FE Control Word to be the second column in Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   rxGainTableFilePointer);
        }

        if (strstr(headerStr3,
                   "TIA Control") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGainTablePath,
                             "Expected TIA Control Word to be the third column in Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   rxGainTableFilePointer);
        }

        if (strstr(headerStr4,
                   "ADC Control") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGainTablePath,
                             "Expected ADC Control Word to be the fourth column in Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   rxGainTableFilePointer);
        }

        if (strstr(headerStr5,
                   "Ext Control") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGainTablePath,
                             "Expected TIA Control Word to be the fifth column in Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   rxGainTableFilePointer);
        }

        if (strstr(headerStr6,
                   "Phase Offset") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGainTablePath,
                             "Expected Phase Offset to be the sixth column in Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   rxGainTableFilePointer);
        }

        if (strstr(headerStr7,
                   "Digital Gain") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGainTablePath,
                             "Expected Digital Gain to be the seventh column in Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   rxGainTableFilePointer);
        }

        /*Loop until the gain table end is reached or no. of lines scanned exceeds maximum*/
        while ((fgets(rxGainTableLineBuffer,
                      sizeof(rxGainTableLineBuffer),
                      rxGainTableFilePointer) != NULL) &&
            (lineCount < ADI_ADRV9025_RX_GAIN_TABLE_SIZE_ROWS))
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
            if (sscanf_s(rxGainTableLineBuffer, "%hhu,%hhu,%hhu,%hhu,%hhu,%hu,%hd", &gainIndex,
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
                ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                       rxGainTableFilePointer);
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
                    ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                           rxGainTableFilePointer);
                }
            }

            prevGainIndex = gainIndex;
            lineCount++;
        }

        maxGainIndex = gainIndex;

        if ((adi_adrv9025_RxGainTableWrite(device,
                                           rxChannelMask,
                                           maxGainIndex,
                                           &rxGainTableRowBuffer[0],
                                           lineCount)) != ADI_COMMON_ACT_NO_ACTION)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGainTablePath,
                             "Unable to Write Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   rxGainTableFilePointer);
        }

        if ((adi_adrv9025_RxMinMaxGainIndexSet(device,
                                               rxChannelMask,
                                               minGainIndex,
                                               maxGainIndex)) != ADI_COMMON_ACT_NO_ACTION)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGainTablePath,
                             "Unable to set Rx gain table min/max gain indices.");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   rxGainTableFilePointer);
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

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxAttenTableLoad(adi_adrv9025_Device_t* device,
                                      const char*            txAttenTablePath,
                                      uint32_t               txChannelMask)
{
    static const uint8_t NUM_COLUMNS = 3;
#define LINE_BUFFER_SIZE 128
#define HEADER_BUFFER_SIZE 16

    uint16_t                              prevAttenIndex          = 0;
    uint16_t                              attenIndex              = 0;
    uint16_t                              minAttenIndex           = 0;
    uint16_t                              maxAttenIndex           = 0;
    uint16_t                              lineCount               = 0;
    uint16_t                              tableSize               = 0;
    FILE*                                 txAttenTableFilePointer = NULL;
    char                                  txAttenTableLineBuffer[LINE_BUFFER_SIZE];
    char                                  headerStr1[HEADER_BUFFER_SIZE];
    char                                  headerStr2[HEADER_BUFFER_SIZE];
    char                                  headerStr3[HEADER_BUFFER_SIZE];
    static adi_adrv9025_TxAttenTableRow_t txAttenTableRowBuffer[ADI_ADRV9025_TX_ATTEN_TABLE_SIZE_ROWS];

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txAttenTablePath);

    maxAttenIndex = ADRV9025_TX_ATTEN_TABLE_MAX;

    /*Open Tx Atten Table csv file*/
#ifdef __GNUC__
    txAttenTableFilePointer = fopen(txAttenTablePath,
                                    "r");
#else
    if (fopen_s(&txAttenTableFilePointer, txAttenTablePath, "r") != 0)
    {
        ADI_ERROR_REPORT(&device->common,  ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, txAttenTablePath,
                     "Unable to open Tx Atten Table csv file. Please check if the path is correct or the file is open in another program");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif

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

    /*Check for empty Tx Atten Table*/
    if (fgets(txAttenTableLineBuffer,
              sizeof(txAttenTableLineBuffer),
              txAttenTableFilePointer) != NULL)
    {
#ifdef __GNUC__
        if (sscanf(txAttenTableLineBuffer,
                   "%16[^,],%16[^,],%16[^\n]",
                   headerStr1,
                   headerStr2,
                   headerStr3) != NUM_COLUMNS)
#else
        if (sscanf_s(txAttenTableLineBuffer, "%[^,],%[^,],%[^\n]", headerStr1, (uint32_t)sizeof(headerStr1),
                                                                   headerStr2, (uint32_t)sizeof(headerStr2),
                                                                   headerStr3, (uint32_t)sizeof(headerStr3)) != NUM_COLUMNS)
#endif
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             txAttenTablePath,
                             "Invalid Tx Atten Table format encountered while attempting to load Tx Atten Table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   txAttenTableFilePointer);
        }

        /*Verify that Gain Table Format is correct*/
        if (strstr(headerStr1,
                   "Tx Atten Index") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             txAttenTablePath,
                             "Expected Tx Atten Index to be the first column in Tx Atten table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   txAttenTableFilePointer);
        }

        if (strstr(headerStr2,
                   "Tx Atten Hp") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             txAttenTablePath,
                             "Expected Tx Atten Hp to be the second column in Tx Atten table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   txAttenTableFilePointer);
        }

        if (strstr(headerStr3,
                   "Tx Atten Mult") == NULL)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             txAttenTablePath,
                             "Expected Tx Atten Mult to be the third column in Tx Atten table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   txAttenTableFilePointer);
        }

        /*Loop until the atten table end is reached or no. of lines scanned exceeds maximum*/
        while ((fgets(txAttenTableLineBuffer,
                      sizeof(txAttenTableLineBuffer),
                      txAttenTableFilePointer) != NULL) &&
            (lineCount < maxAttenIndex))
        {
#ifdef __GNUC__
            if (sscanf(txAttenTableLineBuffer,
                       "%hu,%hhu,%hu",
                       &attenIndex,
                       &txAttenTableRowBuffer[lineCount].txAttenHp,
                       &txAttenTableRowBuffer[lineCount].txAttenMult) != NUM_COLUMNS)
#else
            if (sscanf_s(txAttenTableLineBuffer, "%hu,%hhu,%hu", &attenIndex,
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
                ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                       txAttenTableFilePointer);
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
                    ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                           txAttenTableFilePointer);
                }
            }

            prevAttenIndex = attenIndex;
            lineCount++;
        }

        tableSize = attenIndex - minAttenIndex + 1;

        if ((adi_adrv9025_TxAttenTableWrite(device,
                                            txChannelMask,
                                            minAttenIndex,
                                            &txAttenTableRowBuffer[0],
                                            tableSize)) != ADI_COMMON_ACT_NO_ACTION)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             txAttenTablePath,
                             "Unable to write Tx Atten Table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   txAttenTableFilePointer);
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

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PreMcsInit(adi_adrv9025_Device_t* device,
                                adi_adrv9025_Init_t*   init,
                                const char*            armImagePath,
                                const char*            streamImagePath,
                                const char*            rxGainTablePath,
                                const char*            txAttenTablePath)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    /*TODO : Initialize adc profile with appropriate values*/
    adi_adrv9025_AdcProfiles_t  adcProfile    = {{0}};
    adi_adrv9025_TxAttenCfg_t   txConfig      = {0};
    adi_adrv9025_DacFullScale_e dacFullScale  = ADI_ADRV9025_TX_DACFS_0DB;
    adi_adrv9025_TxAtten_t      txAtten       = {0};
    uint8_t                     i             = 0;
    int32_t                     origLogLevel  = 0;
    int32_t                     logLevelNoSpi = 0;
    uint32_t                    txChannels[]  = {
        (uint32_t)ADI_ADRV9025_TX1,
        (uint32_t)ADI_ADRV9025_TX2,
        (uint32_t)ADI_ADRV9025_TX3,
        (uint32_t)ADI_ADRV9025_TX4
    };

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Reset Hardware */
    recoveryAction = adi_adrv9025_HwReset(device);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Unable to Reset Adrv9025 device");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Setup SPI controller, master bias, enable pin pads, Load PFIRs */
    recoveryAction = adi_adrv9025_Initialize(device,
                                             init);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Unable to Initialize Adrv9025 device");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Disable SPI Logging */
    adi_common_LogLevelGet(&device->common,
                           &origLogLevel);
    ADI_ERROR_RETURN(device->common.error.newAction);

    logLevelNoSpi = ADI_COMMON_LOG_MSG | ADI_COMMON_LOG_WARN | ADI_COMMON_LOG_ERR | ADI_COMMON_LOG_API; /*Disable SPI logging*/

    adi_common_LogLevelSet(&device->common,
                           logLevelNoSpi);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Load Stream binary image from a .bin file */
    adi_adrv9025_StreamImageLoad(device,
                                 streamImagePath);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Load binary image for CPU-C from a.bin file */
    adi_adrv9025_CpuImageLoad(device,
                              armImagePath);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Load Default Rx gain table from a .csv file for initialized channels */
    adi_adrv9025_RxGainTableLoad(device,
                                 rxGainTablePath,
                                 (init->rx.rxInitChannelMask & 0xFF));
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (device->devStateInfo.profilesValid & ADI_ADRV9025_TX_PROFILE_VALID)
    {
        /* Load Default Tx atten table from a .csv file for initialized channels */
        adi_adrv9025_TxAttenTableLoad(device,
                                      txAttenTablePath,
                                      init->tx.txInitChannelMask);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Load ADC Profile */
    adrv9025_AdcProfilesInit(device,
                             init,
                             &adcProfile);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adi_adrv9025_CpuAdcProfilesWrite(device,
                                     &adcProfile);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Load ARM profile */
    adi_adrv9025_CpuProfileWrite(device,
                                 init);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* ARM Bootup */
    adi_adrv9025_CpuStart(device);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* ARM Bootup Status Check */
    adi_adrv9025_CpuStartStatusCheck(device,
                                     ADI_ADRV9025_GETCPUBOOTUP_TIMEOUT_US);
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (device->devStateInfo.profilesValid & ADI_ADRV9025_TX_PROFILE_VALID)
    {
        for (i = 0; i < ADI_ADRV9025_MAX_TXCHANNELS; i++)
        {
            /* For each tx channel enabled */
            if ((init->tx.txInitChannelMask & txChannels[i]) > 0)
            {
                /* For each tx channel enabled */
                if ((init->tx.txInitChannelMask & txChannels[i]) > 0)
                {
                    txConfig.txChannelMask   = txChannels[i];
                    txConfig.txAttenStepSize = init->tx.txChannelCfg[i].txAttenCtrl.txAttenStepSize;
                    txConfig.attenMode       = init->tx.txChannelCfg[i].txAttenCtrl.attenMode;

                    /* These 2 fields are not being used actively */
                    txConfig.disTxDataIfPllUnlock = ADI_ADRV9025_TXUNLOCK_TX_NOT_DISABLED;
                    txConfig.rampJesdDfrm         = ADI_ADRV9025_TXUNLOCK_TX_NOT_DISABLED;

                    adi_adrv9025_TxAttenCfgSet(device,
                                               &txConfig,
                                               1);
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    if (txConfig.attenMode == ADI_ADRV9025_TXATTEN_SPI_MODE)
                    {
                        txAtten.txChannelMask     = txChannels[i];
                        txAtten.txAttenuation_mdB = init->tx.txChannelCfg[i].txAttenCtrl.txAttenInit_mdB;
                        recoveryAction            = adi_adrv9025_TxAttenSet(device,
                                                                            &txAtten,
                                                                            1);
                        ADI_ERROR_RETURN(device->common.error.newAction);
                    }

                    dacFullScale = init->tx.txChannelCfg[i].dacFullScale;
                    adi_adrv9025_DacFullScaleSet(device,
                                                 (adi_adrv9025_TxChannels_e)txChannels[i],
                                                 dacFullScale);
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
        }
    }

    /* Enable SPI logging */
    adi_common_LogLevelSet(&device->common,
                           origLogLevel);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PreMcsInit_v2(adi_adrv9025_Device_t*          device,
                                   adi_adrv9025_Init_t*            init,
                                   const char*                     armImagePath,
                                   const char*                     streamImagePath,
                                   adi_adrv9025_RxGainTableFile_t  rxGainTableFileArr[],
                                   uint8_t                         rxGainTableFileArrSize,
                                   adi_adrv9025_TxAttenTableFile_t txAttenTableFileArr[],
                                   uint8_t                         txAttenTableFileArrSize)

{
    int32_t                    recoveryAction      = ADI_COMMON_ACT_NO_ACTION;
    //adi_adrv9025_AdcProfiles_t adcProfile          = {{0}};
    uint8_t                    i                   = 0;
    uint8_t                    wrCacheStateRestore = device->common.cacheInfo.wrCacheState;
    uint8_t                    wrOnlyRestore       = device->common.cacheInfo.wrOnly;
    int32_t                    origLogLevel        = 0;
    int32_t                    logLevelNoSpi       = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* If Write Cache is off, overwrite it to be HW_RMW to allow bf field write's during broadcast */
    if (device->common.cacheInfo.wrCacheState == (uint8_t)ADRV9025_WR_CACHE_OFF)
    {
        recoveryAction = adrv9025_BfWriteCacheEn(device,
                                                 ADRV9025_WR_HW_RMW_ON);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Cannot change write cache state to HW_RMW_ON");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Prevent any read operation from device during broadcast */
    device->common.cacheInfo.wrOnly = ADI_TRUE;

    /* Setup SPI controller, master bias, enable pin pads, Load PFIRs */
    recoveryAction = adi_adrv9025_Initialize(device,
                                             init);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Unable to Initialize Adrv9025 device");
    ADI_ADRV9025_PREMCS_RESTORE_WRONLY(device->common,
                                       wrOnlyRestore,
                                       recoveryAction);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Disable SPI Logging */
    adi_common_LogLevelGet(&device->common,
                           &origLogLevel);
    ADI_ADRV9025_PREMCS_RESTORE_WRONLY(device->common,
                                       wrOnlyRestore,
                                       device->common.error.newAction);
    ADI_ERROR_RETURN(device->common.error.newAction);

    logLevelNoSpi = ADI_COMMON_LOG_MSG | ADI_COMMON_LOG_WARN | ADI_COMMON_LOG_ERR | ADI_COMMON_LOG_API; /*Disable SPI logging*/

    adi_common_LogLevelSet(&device->common,
                           logLevelNoSpi);
    ADI_ADRV9025_PREMCS_RESTORE_WRONLY(device->common,
                                       wrOnlyRestore,
                                       device->common.error.newAction);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Load Stream binary image from a .bin file */
    recoveryAction = adi_adrv9025_StreamImageLoad(device,
                                                  streamImagePath);
    ADI_ADRV9025_PREMCS_RESTORE_WRONLY(device->common,
                                       wrOnlyRestore,
                                       recoveryAction);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* HW_RMW feature is enabled after stream image is loaded */
    device->common.cacheInfo.HW_RMW_Enabled = ADI_ADRV9025_HW_RMW_ENABLE;

    /* Load ARM binary image from a.bin file */
    recoveryAction = adi_adrv9025_CpuImageLoad(device,
                                               armImagePath);
    ADI_ADRV9025_PREMCS_RESTORE_WRONLY(device->common,
                                       wrOnlyRestore,
                                       recoveryAction);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Load Rx gain table for requested channels */
    for (i = 0; i < rxGainTableFileArrSize; i++)
    {
        recoveryAction = adi_adrv9025_RxGainTableLoad(device,
                                                      rxGainTableFileArr[i].rxGainTableCsvFileName,
                                                      (rxGainTableFileArr[i].rxChannelMask & device->devStateInfo.initializedChannels));
        ADI_ADRV9025_PREMCS_RESTORE_WRONLY(device->common,
                                           wrOnlyRestore,
                                           recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (device->devStateInfo.profilesValid & ADI_ADRV9025_TX_PROFILE_VALID)
    {
        /* Load Tx atten tables from a .csv file for requested channels */
        for (i = 0; i < txAttenTableFileArrSize; i++)
        {
            recoveryAction = adi_adrv9025_TxAttenTableLoad(device,
                                                           txAttenTableFileArr[i].txAttenTableCsvFileName,
                                                           (txAttenTableFileArr[i].txChannelMask & (device->devStateInfo.initializedChannels >>
                                                               ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET)));
            ADI_ADRV9025_PREMCS_RESTORE_WRONLY(device->common,
                                               wrOnlyRestore,
                                               recoveryAction);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /* Load ADC Profiles */
    if (init->adcProfiles.RxChannel1[0] == 0)
    {
        recoveryAction = adrv9025_AdcProfilesInit(device,
                                                  init,
                                                  &init->adcProfiles);
        ADI_ADRV9025_PREMCS_RESTORE_WRONLY(device->common,
                                           wrOnlyRestore,
                                           recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        /* use ADC profiles from init structure */
    }

    recoveryAction = adi_adrv9025_CpuAdcProfilesWrite(device,
                                                      &init->adcProfiles);

    ADI_ADRV9025_PREMCS_RESTORE_WRONLY(device->common,
                                       wrOnlyRestore,
                                       recoveryAction);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Load ARM profile */
    recoveryAction = adi_adrv9025_CpuProfileWrite(device,
                                                  init);
    ADI_ADRV9025_PREMCS_RESTORE_WRONLY(device->common,
                                       wrOnlyRestore,
                                       recoveryAction);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* ARM Bootup */
    recoveryAction = adi_adrv9025_CpuStart(device);
    ADI_ADRV9025_PREMCS_RESTORE_WRONLY(device->common,
                                       wrOnlyRestore,
                                       recoveryAction);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Enable SPI logging */
    adi_common_LogLevelSet(&device->common,
                           origLogLevel);
    ADI_ADRV9025_PREMCS_RESTORE_WRONLY(device->common,
                                       wrOnlyRestore,
                                       recoveryAction);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Restore Bf write cache state for future usage */
    recoveryAction = adrv9025_BfWriteCacheEn(device,
                                             (adrv9025_Bf_WrCache_State_e)wrCacheStateRestore);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Cannot restore write cache state while leaving PreMcsInit");
    ADI_ADRV9025_PREMCS_RESTORE_WRONLY(device->common,
                                       wrOnlyRestore,
                                       recoveryAction);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Restore wrOnly state for future usage */
    device->common.cacheInfo.wrOnly = wrOnlyRestore;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PreMcsInit_NonBroadCast(adi_adrv9025_Device_t* device,
                                             adi_adrv9025_Init_t*   init)
{
    adi_adrv9025_TxAttenCfg_t   txConfig     = {0};
    adi_adrv9025_DacFullScale_e dacFullScale = ADI_ADRV9025_TX_DACFS_0DB;
    adi_adrv9025_TxAtten_t      txAtten      = {0};

    uint32_t txChannels[] = {
        (uint32_t)ADI_ADRV9025_TX1,
        (uint32_t)ADI_ADRV9025_TX2,
        (uint32_t)ADI_ADRV9025_TX3,
        (uint32_t)ADI_ADRV9025_TX4
    };

    adi_adrv9025_RxDataFormat_t rxDataFormat = {0};

    uint32_t rxChannels[] = {
        (uint32_t)ADI_ADRV9025_RX1,
        (uint32_t)ADI_ADRV9025_RX2,
        (uint32_t)ADI_ADRV9025_RX3,
        (uint32_t)ADI_ADRV9025_RX4,
        (uint32_t)ADI_ADRV9025_ORX1,
        (uint32_t)ADI_ADRV9025_ORX2,
        (uint32_t)ADI_ADRV9025_ORX3,
        (uint32_t)ADI_ADRV9025_ORX4
    };

    uint8_t i              = 0;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check init pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        init);

#if ADI_ADRV9025_INIT_RANGE_CHECK > 0
    recoveryAction = adi_adrv9025_ProfilesVerify(device,
                                                 init);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Init structure check failure");
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /* Check processor status before continuing */
    recoveryAction = adi_adrv9025_CpuStartStatusCheck(device,
                                                      ADI_ADRV9025_GETCPUBOOTUP_TIMEOUT_US);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Silicon Revision */
    recoveryAction = adi_adrv9025_DeviceRevGet(device,
                                               &device->devStateInfo.deviceSiRev);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to read the device Silicon Rev");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Product ID */
    recoveryAction = adi_adrv9025_ProductIdGet(device,
                                               &device->devStateInfo.deviceProductId);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to read the device Product ID");
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (device->devStateInfo.profilesValid & ADI_ADRV9025_TX_PROFILE_VALID)
    {
        for (i = 0; i < ADI_ADRV9025_MAX_TXCHANNELS; i++)
        {
            /* For each tx channel enabled */
            if ((init->tx.txInitChannelMask & txChannels[i]) > 0)
            {
                txConfig.txChannelMask   = txChannels[i];
                txConfig.txAttenStepSize = init->tx.txChannelCfg[i].txAttenCtrl.txAttenStepSize;
                txConfig.attenMode       = init->tx.txChannelCfg[i].txAttenCtrl.attenMode;
                if ((txConfig.attenMode == ADI_ADRV9025_TXATTEN_GPIO_MODE) || (txConfig.attenMode == ADI_ADRV9025_TXATTEN_SPI2_MODE))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     attenMode,
                                     "Attenuation mode shouldn't be either GPIO or SPI2 during init time");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
                /* These 2 fields are not being used actively */
                txConfig.disTxDataIfPllUnlock = ADI_ADRV9025_TXUNLOCK_TX_NOT_DISABLED;
                txConfig.rampJesdDfrm         = ADI_ADRV9025_TXUNLOCK_TX_NOT_DISABLED;

                txConfig.simultaneousUpdateCfg.simultaneousUpdate = ADI_ADRV9025_TXATTEN_SIMULTAN_UPDATE_DISABLED;
                txConfig.simultaneousUpdateCfg.gpioUpdatePin      = ADI_ADRV9025_GPIO_INVALID;
                txConfig.gpioModePinCfg.stepSize                  = 0;
                txConfig.gpioModePinCfg.txAttenIncPin             = ADI_ADRV9025_GPIO_INVALID;
                txConfig.gpioModePinCfg.txAttenDecPin             = ADI_ADRV9025_GPIO_INVALID;

                recoveryAction = adi_adrv9025_TxAttenCfgSet(device,
                                                            &txConfig,
                                                            1);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 recoveryAction,
                                 NULL,
                                 device->common.error.errormessage);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if (txConfig.attenMode == ADI_ADRV9025_TXATTEN_SPI_MODE)
                {
                    txAtten.txChannelMask     = txChannels[i];
                    txAtten.txAttenuation_mdB = init->tx.txChannelCfg[i].txAttenCtrl.txAttenInit_mdB;
                    recoveryAction            = adi_adrv9025_TxAttenSet(device,
                                                                        &txAtten,
                                                                        1);
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_API_FAIL,
                                     recoveryAction,
                                     NULL,
                                     device->common.error.errormessage);
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }

                /* enable the pa protection ramp up */
                recoveryAction = adrv9025_TxPaProtectionGainRampUpEnBfSet(device,
                                                                          (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                                                                         txChannels[i]),
                                                                          ADI_TRUE);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 NULL,
                                 device->common.error.errormessage);
                ADI_ERROR_RETURN(device->common.error.newAction);

                recoveryAction = adi_adrv9025_TxAttenuationRampUpStickyModeEnable(device,
                                                                                  txChannels[i],
                                                                                  ADI_TRUE,
                                                                                  ADI_TRUE,
                                                                                  ADI_TRUE);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 recoveryAction,
                                 NULL,
                                 device->common.error.errormessage);
                ADI_ERROR_RETURN(device->common.error.newAction);

                dacFullScale   = init->tx.txChannelCfg[i].dacFullScale;
                recoveryAction = adi_adrv9025_DacFullScaleSet(device,
                                                              (adi_adrv9025_TxChannels_e)txChannels[i],
                                                              dacFullScale);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 recoveryAction,
                                 NULL,
                                 device->common.error.errormessage);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    if (device->devStateInfo.profilesValid & ADI_ADRV9025_RX_PROFILE_VALID)
    {
        for (i = 0; i < (sizeof(rxChannels) / sizeof(rxChannels[0])); i++)
        {
            if ((init->rx.rxInitChannelMask & rxChannels[i]) > 0)
            {
                rxDataFormat.rxChannelMask                               = rxChannels[i];
                rxDataFormat.externalLnaGain                             = init->rx.rxChannelCfg[i].profile.rxDataFormat.externalLnaGain;
                rxDataFormat.tempCompensationEnable                      = init->rx.rxChannelCfg[i].profile.rxDataFormat.tempCompensationEnable;
                rxDataFormat.formatSelect                                = init->rx.rxChannelCfg[i].profile.rxDataFormat.formatSelect;
                rxDataFormat.floatingPointConfig.fpDataFormat            = init->rx.rxChannelCfg[i].profile.rxDataFormat.floatingPointConfig.fpDataFormat;
                rxDataFormat.floatingPointConfig.fpAttenSteps            = init->rx.rxChannelCfg[i].profile.rxDataFormat.floatingPointConfig.fpAttenSteps;
                rxDataFormat.floatingPointConfig.fpNumExpBits            = init->rx.rxChannelCfg[i].profile.rxDataFormat.floatingPointConfig.fpNumExpBits;
                rxDataFormat.floatingPointConfig.fpRoundMode             = init->rx.rxChannelCfg[i].profile.rxDataFormat.floatingPointConfig.fpRoundMode;
                rxDataFormat.floatingPointConfig.fpHideLeadingOne        = init->rx.rxChannelCfg[i].profile.rxDataFormat.floatingPointConfig.fpHideLeadingOne;
                rxDataFormat.floatingPointConfig.fpEncodeNan             = init->rx.rxChannelCfg[i].profile.rxDataFormat.floatingPointConfig.fpEncodeNan;
                rxDataFormat.integerConfigSettings.intSampleResolution   = init->rx.rxChannelCfg[i].profile.rxDataFormat.integerConfigSettings.intSampleResolution;
                rxDataFormat.integerConfigSettings.intEmbeddedBits       = init->rx.rxChannelCfg[i].profile.rxDataFormat.integerConfigSettings.intEmbeddedBits;
                rxDataFormat.integerConfigSettings.intParity             = init->rx.rxChannelCfg[i].profile.rxDataFormat.integerConfigSettings.intParity;
                rxDataFormat.slicerConfigSettings.intSlicerStepSize      = init->rx.rxChannelCfg[i].profile.rxDataFormat.slicerConfigSettings.intSlicerStepSize;
                rxDataFormat.slicerConfigSettings.extSlicerStepSize      = init->rx.rxChannelCfg[i].profile.rxDataFormat.slicerConfigSettings.extSlicerStepSize;
                rxDataFormat.slicerConfigSettings.rx1ExtSlicerGpioSelect = init->rx.rxChannelCfg[i].profile.rxDataFormat.slicerConfigSettings.rx1ExtSlicerGpioSelect;
                rxDataFormat.slicerConfigSettings.rx2ExtSlicerGpioSelect = init->rx.rxChannelCfg[i].profile.rxDataFormat.slicerConfigSettings.rx2ExtSlicerGpioSelect;
                rxDataFormat.slicerConfigSettings.rx3ExtSlicerGpioSelect = init->rx.rxChannelCfg[i].profile.rxDataFormat.slicerConfigSettings.rx3ExtSlicerGpioSelect;
                rxDataFormat.slicerConfigSettings.rx4ExtSlicerGpioSelect = init->rx.rxChannelCfg[i].profile.rxDataFormat.slicerConfigSettings.rx4ExtSlicerGpioSelect;

                /* Setup the Rx formatter */
                recoveryAction = adi_adrv9025_RxDataFormatSet(device,
                                                              &rxDataFormat,
                                                              1);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 recoveryAction,
                                 NULL,
                                 device->common.error.errormessage);
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Setup Rx AGC overload protection registers */
                recoveryAction = adrv9025_RxOverloadProtectionSet(device,
                                                                  (adi_adrv9025_RxChannels_e)rxChannels[i]);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 recoveryAction,
                                 NULL,
                                 device->common.error.errormessage);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    /* Update desCfg */
    recoveryAction = adrv9025_JesdDesCfgSet(device, init);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PostMcsInit(adi_adrv9025_Device_t*      device,
                                 adi_adrv9025_PostMcsInit_t* utilityInit)
{
    static const uint16_t RX_TIA_TUNER_MODE_REG_ADDR = 0x0C3E;

    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t rxEnableRegs   = (((uint32_t)ADI_ADRV9025_RX1 |
                                (uint32_t)ADI_ADRV9025_RX2 |
                                (uint32_t)ADI_ADRV9025_RX3 |
                                (uint32_t)ADI_ADRV9025_RX4)
                               & device->devStateInfo.initializedChannels);

    uint8_t  bufferProtectionDisable      = 0x00;
    adi_adrv9025_DeframerSel_e dfrmSelect = ADI_ADRV9025_DEFRAMER_0_AND_1;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, utilityInit);

    /*******************************HB2_IN BIT TOGGLE ************************************/
    /* Enable the Rx Channels to setup AGC overload thresholds */
    recoveryAction = adi_adrv9025_RxTxEnableSet(device, rxEnableRegs, (uint32_t)ADI_ADRV9025_TXOFF);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* 1ms sync delay to allow the Rx Tx enable streams to finish executing */
    recoveryAction = adi_common_hal_Wait_ms(&device->common, (uint32_t)1);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Toggle force tia update bit */
    ADRV9025_SPIWRITEBYTE("ANALOG_RX_TIA_TUNER_MODE_REG", RX_TIA_TUNER_MODE_REG_ADDR, 0x01);
    ADRV9025_SPIWRITEBYTE("ANALOG_RX_TIA_TUNER_MODE_REG", RX_TIA_TUNER_MODE_REG_ADDR, 0x00);

    /* Power down all Rx and Tx channels */
    recoveryAction = adi_adrv9025_RxTxEnableSet(device, (uint32_t)ADI_ADRV9025_RXOFF, (uint32_t)ADI_ADRV9025_TXOFF);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*******************************END HB2_IN BIT TOGGLE ************************************/

    /*Initialize radio control. This is required to run before running init cals*/
    recoveryAction = adrv9025_RadioctrlInit(device, &utilityInit->radioCtrlInit);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Initialize cals*/
    recoveryAction = adrv9025_CalsInit(device, &utilityInit->initCals);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Disable Buffer Protection for both Deframers */
    recoveryAction = adi_adrv9025_DfrmBufferProtectionSet(device, bufferProtectionDisable, dfrmSelect);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_AdcProfilesInit(adi_adrv9025_Device_t*      device,
                                     adi_adrv9025_Init_t*        init,
                                     adi_adrv9025_AdcProfiles_t* adcProfile)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        init);

    ADI_NULL_PTR_RETURN(&device->common,
                        adcProfile);

#if ADI_ADRV9025_INIT_RANGE_CHECK > 0
    recoveryAction = adi_adrv9025_ProfilesVerify(device,
                                                 init);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Init structure check failure");
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /* Load ADC Profile */
    recoveryAction = adrv9025_AdcProfilesInit(device,
                                              init,
                                              adcProfile);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Unable to Initialize Adc profiles");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_AuxDacCompensatedSet(adi_adrv9025_Device_t*                  device,
                                          adi_adrv9025_AuxDacValueToCompensate_t* auxDacSet)
{
    static const int32_t INT_NUMERATOR_SHIFT   = 15;
    static const int32_t INT_DENOMINATOR_SHIFT = 24;

    /*Original values shifted by INT_NUMERATOR_SHIFT { 139.420, -9.530, -0.094, -3.290, -62.220, -103.900, -118.570, -112.420 } */
    static const int32_t b35CInt[8] = {4568515, -312279, -3080, -107807, -2038825, -3404595, -3885302, -3683779};

    /*Original values shifted by INT_NUMERATOR_SHIFT { -0.14, 0.141, 0.094, 0.1, 0.101, 0.258, 0.279, 0.237 } */
    static const int32_t bTDInt[8] = {-4588, 4620, 3080, 3277, 3310, 8454, 9142, 7766};

    /*Original values shifted by INT_DENOMINATOR_SHIFT { 0.359, 0.43, 0.431, 0.433, 0.462, 0.482, 0.482, 0.484 } */
    static const int32_t m35CInt[8] = {6023021, 7214203, 7230980, 7264535, 7751074, 8086618, 8086618, 8120173};

    /*Original values shifted by INT_DENOMINATOR_SHIFT { 0.0000022, -0.000162, -0.000111, -0.00014, -0.000167, -0.000273, -0.000204, -0.00018 } */
    static const int32_t mTDInt[8] = {37, -2718, -1862, -2349, -2802, -4580, -3423, -3020};

    static const uint16_t MAX_DAC_VOLTAGE = 1800;

    int32_t                    recoveryAction     = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                    i                  = 0;
    int32_t                    tempValInt         = 0;
    adi_adrv9025_AuxDacValue_t auxDacValueLocal   = {0, 0};
    uint8_t                    temperatureAddr[4] = {0};
    uint8_t                    temperatureData[2] = {0};
    uint16_t                   requestedDacWord   = 0;
    uint32_t                   temperatureAdd     = 0;
    int32_t                    dacVoltageShifted  = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        auxDacSet);

    if (auxDacSet->auxDacValue.dacVoltage > MAX_DAC_VOLTAGE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dacVoltage,
                         "Requested DAC voltage is out of range, valid range is [0-1800mV]");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((auxDacSet->auxDacValueCompensationMode != ADI_ADRV9025_NO_COMPENSATION) &&
        (auxDacSet->auxDacValueCompensationMode != ADI_ADRV9025_NO_TEMP_COMPENSATION) &&
        (auxDacSet->auxDacValueCompensationMode != ADI_ADRV9025_COMPENSATE_WITH_INT_TEMP) &&
        (auxDacSet->auxDacValueCompensationMode != ADI_ADRV9025_COMPENSATE_WITH_EXT_TEMP))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         auxDacValueCompensationMode,
                         "Invalid compensation mode is selected for AuxDAC");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    dacVoltageShifted = (int32_t)auxDacSet->auxDacValue.dacVoltage << INT_NUMERATOR_SHIFT;

    for (i = 0; i < ADI_ADRV9025_MAX_AUXDACS; i++)
    {
        if ((auxDacSet->auxDacValue.auxDacMask & (uint32_t)(1 << i)) == (uint32_t)(1 << i))
        {
            if (auxDacSet->auxDacValueCompensationMode != ADI_ADRV9025_NO_COMPENSATION)
            {
                if (auxDacSet->auxDacValueCompensationMode == ADI_ADRV9025_NO_TEMP_COMPENSATION)
                {
                    auxDacSet->extTemperatureDegC = 35;
                }
                else if (auxDacSet->auxDacValueCompensationMode == ADI_ADRV9025_COMPENSATE_WITH_INT_TEMP)
                {
                    recoveryAction = adrv9025_CpuDmaMemRead(device,
                                                            ADRV9025_CPU_ADDR_AVG_TEMP_ADDR,
                                                            temperatureAddr,
                                                            sizeof(temperatureAddr),
                                                            ADI_ADRV9025_CPU_MEM_AUTO_INCR);
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_API_FAIL,
                                     recoveryAction,
                                     NULL,
                                     "Failed to read ARM memory");
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    temperatureAdd = ((((uint32_t)temperatureAddr[3]) << 24) | (((uint32_t)temperatureAddr[2]) << 16) | (((uint32_t)temperatureAddr[1]) << 8) |
                        ((uint32_t)temperatureAddr[0]));

                    recoveryAction = adrv9025_CpuDmaMemRead(device,
                                                            temperatureAdd,
                                                            temperatureData,
                                                            sizeof(temperatureData),
                                                            ADI_ADRV9025_CPU_MEM_AUTO_INCR);
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_API_FAIL,
                                     recoveryAction,
                                     NULL,
                                     "Failed to read ARM memory");
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    auxDacSet->extTemperatureDegC = (int16_t)(((uint16_t)temperatureData[1]) << 8) | ((uint16_t)temperatureData[0]);
                }

                tempValInt = m35CInt[i] + mTDInt[i] * ((int32_t)auxDacSet->extTemperatureDegC - 35);
                tempValInt = tempValInt >> (INT_DENOMINATOR_SHIFT - INT_NUMERATOR_SHIFT);
                if (tempValInt == 0)
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     tempValInt,
                                     "Zero divide detected during AuxDAC compensation");
                    return device->common.error.newAction;
                }

                auxDacValueLocal.dacWord = (uint16_t)((dacVoltageShifted - (b35CInt[i] + bTDInt[i] * ((int32_t)auxDacSet->extTemperatureDegC - 35))) /
                    tempValInt);

                recoveryAction = adrv9025_AuxDacValueSet(device,
                                                         (adi_adrv9025_AuxDacs_e)((uint8_t)(1 << i)),
                                                         auxDacValueLocal.dacWord);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 NULL,
                                 device->common.error.errormessage);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else
            {
                requestedDacWord = (uint16_t)(((uint32_t)auxDacSet->auxDacValue.dacVoltage * 4095) / 1800);
                recoveryAction   = adrv9025_AuxDacValueSet(device,
                                                           (adi_adrv9025_AuxDacs_e)((uint8_t)(1 << i)),
                                                           requestedDacWord);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 NULL,
                                 device->common.error.errormessage);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_ConfigFileLoad(adi_adrv9025_Device_t* device,
                                    const char*            configFile,
                                    adi_adrv9025_Init_t*   init)
{
    int32_t recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    FILE*   initConifgJsonfilePtr                 = NULL;
    char    lineStr[ADRV9025_MAX_FILE_BUFFER + 1] = {0};
    int32_t currChar                              = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        configFile);

    ADI_NULL_PTR_RETURN(&device->common,
                        init);

#if ADI_ADRV9025_INIT_RANGE_CHECK > 0
    recoveryAction = adi_adrv9025_ProfilesVerify(device,
        init);
    ADI_ERROR_REPORT(&device->common,
        ADI_COMMON_ERRSRC_API,
        ADI_COMMON_ERR_API_FAIL,
        recoveryAction,
        NULL,
        "Init structure check failure");
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    initConifgJsonfilePtr = fopen(configFile,
                                  "r");

    if (initConifgJsonfilePtr == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         configFile,
                         "Invalid init config file path encountered while attempting to load ADRV9025 Init Structure");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    adrv9025_JsonFindStartOfFile(device,
                                 initConifgJsonfilePtr,
                                 &lineStr[0],
                                 "{",
                                 &currChar);
    ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                           initConifgJsonfilePtr);

    adrv9025_JsonParseInitDataInterfaceSettings(device,
                                                &init->dataInterface,
                                                initConifgJsonfilePtr);
    ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                           initConifgJsonfilePtr);

    adrv9025_JsonParseInitClockSettings(device,
                                        &init->clocks,
                                        initConifgJsonfilePtr);
    ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                           initConifgJsonfilePtr);

    adrv9025_JsonParseInitGpInterruptSettings(device,
                                              &init->gpInterrupts,
                                              initConifgJsonfilePtr);
    ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                           initConifgJsonfilePtr);

    adrv9025_JsonParseInitRxSettings(device,
                                     &init->rx,
                                     initConifgJsonfilePtr);
    ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                           initConifgJsonfilePtr);

    adrv9025_JsonParseInitTxSettings(device,
                                     &init->tx,
                                     initConifgJsonfilePtr);
    ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                           initConifgJsonfilePtr);

    adrv9025_JsonParseInitAdcProfileSettings(device,
                                             &init->adcProfiles,
                                             initConifgJsonfilePtr);
    if (init->adcProfiles.RxChannel1[0] != 0)
    {
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
            initConifgJsonfilePtr);
    }


    recoveryAction = adrv9025_JsonFindEndOfFile(device,
                                                initConifgJsonfilePtr,
                                                &lineStr[0],
                                                "}",
                                                &currChar);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Close JSON config file*/
    if (fclose(initConifgJsonfilePtr) < 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         initConifgJsonfilePtr,
                         "Fatal error while trying to close a JSON file. Possible memory shortage while flushing / other I/O errors.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_UtilityInitFileLoad(adi_adrv9025_Device_t*      device,
                                         const char*                 utilInitFile,
                                         adi_adrv9025_PostMcsInit_t* utilInit)
{
    FILE*   initConifgJsonfilePtr                 = NULL;
    int32_t recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    char    lineStr[ADRV9025_MAX_FILE_BUFFER + 1] = { 0 };
    int32_t currChar                              = 0;
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, utilInitFile);
    ADI_NULL_PTR_RETURN(&device->common, utilInit);


    initConifgJsonfilePtr = fopen(utilInitFile, "r");

    if (initConifgJsonfilePtr == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            configFile,
            "Invalid util init config file path encountered while attempting to load ADRV9025 Util Init Structure");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    adrv9025_JsonFindStartOfFile(device,
                                 initConifgJsonfilePtr,
                                 &lineStr[0],
                                 "{",
                                 &currChar);
    ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                           initConifgJsonfilePtr);

    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    initConifgJsonfilePtr,
                                                    &lineStr[0],
                                                    "radioCtrlInit",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    initConifgJsonfilePtr,
                                                    &lineStr[0],
                                                    "radioCtrlModeCfg",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    initConifgJsonfilePtr,
                                                    &lineStr[0],
                                                    "txRadioCtrlModeCfg",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "txEnableMode",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.radioCtrlModeCfg.txRadioCtrlModeCfg.txEnableMode);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "txChannelMask",
                                          &currChar,
                                          &utilInit->radioCtrlInit.radioCtrlModeCfg.txRadioCtrlModeCfg.txChannelMask);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                  initConifgJsonfilePtr,
                                                  &lineStr[0],
                                                  "txRadioCtrlModeCfg",
                                                  &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    initConifgJsonfilePtr,
                                                    &lineStr[0],
                                                    "rxRadioCtrlModeCfg",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "rxEnableMode",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.radioCtrlModeCfg.rxRadioCtrlModeCfg.rxEnableMode);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "rxChannelMask",
                                          &currChar,
                                          &utilInit->radioCtrlInit.radioCtrlModeCfg.rxRadioCtrlModeCfg.rxChannelMask);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                    initConifgJsonfilePtr,
                                                    &lineStr[0],
                                                    "rxRadioCtrlModeCfg",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    initConifgJsonfilePtr,
                                                    &lineStr[0],
                                                    "orxRadioCtrlModeCfg",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);


    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "orxEnableMode",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.radioCtrlModeCfg.orxRadioCtrlModeCfg.orxEnableMode);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "orxPinSelectSettlingDelay_armClkCycles",
                                          &currChar,
                                          &utilInit->radioCtrlInit.radioCtrlModeCfg.orxRadioCtrlModeCfg.orxPinSelectSettlingDelay_armClkCycles);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "singleChannel1PinModeOrxSel",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.radioCtrlModeCfg.orxRadioCtrlModeCfg.singleChannel1PinModeOrxSel);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "singleChannel2PinModeLowOrxSel",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.radioCtrlModeCfg.orxRadioCtrlModeCfg.singleChannel2PinModeLowOrxSel);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "singleChannel2PinModeHighOrxSel",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.radioCtrlModeCfg.orxRadioCtrlModeCfg.singleChannel2PinModeHighOrxSel);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "dualChannel2PinModeOrxSel",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.radioCtrlModeCfg.orxRadioCtrlModeCfg.dualChannel2PinModeOrxSel);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                    initConifgJsonfilePtr,
                                                    &lineStr[0],
                                                    "orxRadioCtrlModeCfg",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                    initConifgJsonfilePtr,
                                                    &lineStr[0],
                                                    "radioCtrlModeCfg",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    initConifgJsonfilePtr,
                                                    &lineStr[0],
                                                    "txToOrxMapping",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "orx1Map",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.txToOrxMapping.orx1Map);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "orx2Map",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.txToOrxMapping.orx2Map);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "orx3Map",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.txToOrxMapping.orx3Map);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "orx4Map",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.txToOrxMapping.orx4Map);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                    initConifgJsonfilePtr,
                                                    &lineStr[0],
                                                    "txToOrxMapping",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                    initConifgJsonfilePtr,
                                                    &lineStr[0],
                                                    "streamGpioCfg",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "streamGpInput0",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.streamGpioCfg.streamGpInput0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "streamGpInput1",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.streamGpioCfg.streamGpInput1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "streamGpInput2",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.streamGpioCfg.streamGpInput2);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "streamGpInput3",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.streamGpioCfg.streamGpInput3);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "streamGpInput4",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.streamGpioCfg.streamGpInput4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "streamGpInput5",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.streamGpioCfg.streamGpInput5);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "streamGpInput6",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.streamGpioCfg.streamGpInput6);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "streamGpInput7",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.streamGpioCfg.streamGpInput7);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "streamGpInput8",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.streamGpioCfg.streamGpInput8);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "streamGpInput9",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.streamGpioCfg.streamGpInput9);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "streamGpInput10",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.streamGpioCfg.streamGpInput10);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "streamGpInput11",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.streamGpioCfg.streamGpInput11);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "streamGpInput12",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.streamGpioCfg.streamGpInput12);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "streamGpInput13",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.streamGpioCfg.streamGpInput13);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "streamGpInput14",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.streamGpioCfg.streamGpInput14);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "streamGpInput15",
                                          &currChar,
                                          (uint8_t*)&utilInit->radioCtrlInit.streamGpioCfg.streamGpInput15);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                    initConifgJsonfilePtr,
                                                    &lineStr[0],
                                                    "streamGpioCfg",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonParseU64(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "lo1PllFreq_Hz",
                                          &currChar,
                                          &utilInit->radioCtrlInit.lo1PllFreq_Hz);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU64(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "lo2PllFreq_Hz",
                                          &currChar,
                                          &utilInit->radioCtrlInit.lo2PllFreq_Hz);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU64(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "auxPllFreq_Hz",
                                          &currChar,
                                          &utilInit->radioCtrlInit.auxPllFreq_Hz);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                    initConifgJsonfilePtr,
                                                    &lineStr[0],
                                                    "radioCtrlInit",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonFindStartOfObject(device,
                                                  initConifgJsonfilePtr,
                                                  &lineStr[0],
                                                  "initCals",
                                                  &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU32(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "calMask",
                                          &currChar,
                                          &utilInit->initCals.calMask);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "channelMask",
                                          &currChar,
                                          &utilInit->initCals.channelMask);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "warmBoot",
                                          &currChar,
                                          &utilInit->initCals.warmBoot);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonFindEndOfObject(device,
                                                    initConifgJsonfilePtr,
                                                    &lineStr[0],
                                                    "initCals",
                                                    &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*move to calrslt file */
    recoveryAction = adrv9025_JsonParseCalRsltFileStr(device,
                                                      initConifgJsonfilePtr,
                                                      &lineStr[0],
                                                      &currChar);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_JsonParseU8(device,
                                          initConifgJsonfilePtr,
                                          &lineStr[0],
                                          "checkFwVer",
                                          &currChar,
                                          &utilInit->checkFwVer);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_JsonFindEndOfFile(device,
                                                initConifgJsonfilePtr,
                                                &lineStr[0],
                                                "}",
                                                &currChar);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Close JSON config file*/
    if (fclose(initConifgJsonfilePtr) < 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         initConifgJsonfilePtr,
                         "Fatal error while trying to close a JSON file. Possible memory shortage while flushing / other I/O errors.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    return recoveryAction;
}

int32_t adi_adrv9025_ArmMemDump(adi_adrv9025_Device_t* device,
                                const char*            binaryFilename)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_NULL_PTR_RETURN(&device->common, binaryFilename);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    adi_adrv9025_CpuMemDump(device,
                            ADI_ADRV9025_CPU_TYPE_C,
                            binaryFilename);
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuMemDump(adi_adrv9025_Device_t* device,
                                adi_adrv9025_CpuType_e cpuType,
                                const char*            binaryFilename)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    int32_t                 recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    FILE*                   ofp;
    uint32_t                byteCount                                         = 0;
    uint32_t                offset                                            = 0;
    uint8_t                 armMailboxBusy                                    = 0;
    uint8_t                 cmdStatusByte                                     = 0;
    uint8_t                 exceptionArray[4]                                 = {0};
    uint8_t                 extArray[4]                                       = {0};
    uint32_t                exceptionValue                                    = 0;
    adi_adrv9025_CpuAddr_t* cpuAddr                                           = NULL;
    uint8_t                 binaryRead[ADI_ADRV9025_MEM_DUMP_CHUNK_SIZE + 10] = {0};

    const uint32_t armExceptionAddr = 0x20028210; /* Exception Flag Memory */

    ADI_NULL_PTR_RETURN(&device->common,
                        binaryFilename);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADRV9025_BUGINFO(__FUNCTION__);

#ifdef ADRV9025_INIT_DEBUG
    printf("open %s (%s) \n", binaryFilename, "wb");
#endif

#ifdef __GNUC__
    ofp = fopen(binaryFilename,
                "wb");
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

    /* get the particular processor's address map */
    cpuAddr = adrv9025_CpuAddrGet(&device->devStateInfo.cpu,
                                  cpuType);
    ADI_NULL_PTR_RETURN(&device->common,
                        cpuAddr);

    /* Check if exception has occurred */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             armExceptionAddr,
                                             &exceptionArray[0],
                                             4,
                                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Unable to Read Arm memory");
    ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                           ofp);

    exceptionValue = (uint32_t)(exceptionArray[0] | (exceptionArray[1] << 8) | (exceptionArray[2] << 16) | (exceptionArray[3] << 24));

    if (exceptionValue == 0)
    {
        /* Force an exception during ARM MEM dump for more useful information        */

        recoveryAction = adi_adrv9025_CpuMailboxBusyGet(device,
                                                        ADI_ADRV9025_CPU_TYPE_C,
                                                        &armMailboxBusy);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Invalid Get for armMailboxBusy");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                               ofp);

        if (armMailboxBusy == ADI_FALSE)
        {
            extArray[0]    = ADRV9025_CPU_OBJECTID_CPU_EXCEPTION;
            recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                                      ADI_ADRV9025_CPU_TYPE_C,
                                                      ADRV9025_CPU_SET_OPCODE,
                                                      &extArray[0],
                                                      1);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             recoveryAction,
                             NULL,
                             "Unable to Proccess Arm command");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   ofp);

            recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                           ADI_ADRV9025_CPU_TYPE_C,
                                                           ADRV9025_CPU_SET_OPCODE,
                                                           &cmdStatusByte,
                                                           ADI_ADRV9025_WRITECPUEXCEPTION_TIMEOUT_US,
                                                           ADI_ADRV9025_WRITECPUEXCEPTION_INTERVAL_US);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             recoveryAction,
                             NULL,
                             "Unable to Proccess Arm command");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   ofp);

            recoveryAction = adi_adrv9025_CpuMemRead(device,
                                                     armExceptionAddr,
                                                     &exceptionArray[0],
                                                     4,
                                                     ADI_ADRV9025_CPU_MEM_AUTO_INCR);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             recoveryAction,
                             NULL,
                             "Unable to Read Arm memory");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   ofp);

            exceptionValue = (uint32_t)(exceptionArray[0] | (exceptionArray[1] << 8) | (exceptionArray[2] << 16) | (exceptionArray[3] << 24));
            if (exceptionValue == 0)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 exceptionValue,
                                 "Unable to force ARM to throw exception");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    /* Program Binary */
    for (offset = cpuAddr->progStartAddr; offset < cpuAddr->progEndAddr; offset += ADI_ADRV9025_MEM_DUMP_CHUNK_SIZE)
    {
        if (offset < (cpuAddr->progEndAddr - ADI_ADRV9025_MEM_DUMP_CHUNK_SIZE))
        {
            byteCount = ADI_ADRV9025_MEM_DUMP_CHUNK_SIZE;
        }
        else
        {
            byteCount = cpuAddr->progEndAddr + 1 - offset;
        }

        recoveryAction = adi_adrv9025_CpuMemRead(device,
                                                 offset,
                                                 binaryRead,
                                                 byteCount,
                                                 ADI_ADRV9025_CPU_MEM_AUTO_INCR);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Unable to Read Arm memory");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                               ofp);

        if (fwrite(binaryRead,
                   1,
                   byteCount,
                   ofp) != byteCount)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             ofp,
                             "Fatal error while trying to write a binary file.");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   ofp);
        }
    }

    /* Data Binary */
    for (offset = cpuAddr->dataStartAddr; offset < cpuAddr->dataEndAddr; offset += ADI_ADRV9025_MEM_DUMP_CHUNK_SIZE)
    {
        if (offset < (cpuAddr->dataEndAddr - ADI_ADRV9025_MEM_DUMP_CHUNK_SIZE))
        {
            byteCount = ADI_ADRV9025_MEM_DUMP_CHUNK_SIZE;
        }
        else
        {
            byteCount = cpuAddr->dataEndAddr + 1 - offset;
        }

        recoveryAction = adi_adrv9025_CpuMemRead(device,
                                                 offset,
                                                 binaryRead,
                                                 byteCount,
                                                 ADI_ADRV9025_CPU_MEM_AUTO_INCR);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Unable to Read Arm memory");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                               ofp);

        if (fwrite(binaryRead,
                   1,
                   byteCount,
                   ofp) != byteCount)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             ofp,
                             "Fatal error while trying to write a binary file.");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   ofp);
        }
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

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuMemDumpWithoutException(adi_adrv9025_Device_t* device,
                                                adi_adrv9025_CpuType_e cpuType,
                                                const char*            binaryFilename)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    int32_t                 recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    FILE*                   ofp;
    uint32_t                byteCount = 0;
    uint32_t                offset    = 0;
    adi_adrv9025_CpuAddr_t* cpuAddr   = NULL;

    uint8_t binaryRead[ADI_ADRV9025_MEM_DUMP_CHUNK_SIZE + 10] = {0};

    ADI_NULL_PTR_RETURN(&device->common,
                        binaryFilename);

#ifdef __GNUC__
    ofp = fopen(binaryFilename,
                "wb");
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

    /* get the particular processor's address map */
    cpuAddr = adrv9025_CpuAddrGet(&device->devStateInfo.cpu,
                                  cpuType);
    ADI_NULL_PTR_RETURN(&device->common,
                        cpuAddr);

    /* Program Binary */
    for (offset = cpuAddr->progStartAddr; offset < cpuAddr->progEndAddr; offset += ADI_ADRV9025_MEM_DUMP_CHUNK_SIZE)
    {
        if (offset < (cpuAddr->progEndAddr - ADI_ADRV9025_MEM_DUMP_CHUNK_SIZE))
        {
            byteCount = ADI_ADRV9025_MEM_DUMP_CHUNK_SIZE;
        }
        else
        {
            byteCount = cpuAddr->progEndAddr + 1 - offset;
        }

        recoveryAction = adi_adrv9025_CpuMemRead(device,
                                                 offset,
                                                 binaryRead,
                                                 byteCount,
                                                 ADI_ADRV9025_CPU_MEM_AUTO_INCR);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Unable to Read Arm Program memory");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                               ofp);

        if (fwrite(binaryRead,
                   1,
                   byteCount,
                   ofp) != byteCount)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             ofp,
                             "Fatal error while trying to write program memory to binary file.");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   ofp);
        }
    }

    /* Data Binary */
    for (offset = cpuAddr->dataStartAddr; offset < cpuAddr->dataEndAddr; offset += ADI_ADRV9025_MEM_DUMP_CHUNK_SIZE)
    {
        if (offset < (cpuAddr->dataEndAddr - ADI_ADRV9025_MEM_DUMP_CHUNK_SIZE))
        {
            byteCount = ADI_ADRV9025_MEM_DUMP_CHUNK_SIZE;
        }
        else
        {
            byteCount = cpuAddr->dataEndAddr + 1 - offset;
        }

        recoveryAction = adi_adrv9025_CpuMemRead(device,
                                                 offset,
                                                 binaryRead,
                                                 byteCount,
                                                 ADI_ADRV9025_CPU_MEM_AUTO_INCR);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Unable to Read Arm Data memory");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                               ofp);

        if (fwrite(binaryRead,
                   1,
                   byteCount,
                   ofp) != byteCount)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             ofp,
                             "Fatal error while trying to write data memory to binary file.");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   ofp);
        }
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

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_GpIntInit(adi_adrv9025_Device_t*              device,
                               adi_adrv9025_GpInterruptSettings_t* gpIntInit)
{
    adi_adrv9025_gpMaskSelect_e maskSelect = ADI_ADRV9025_GPINTALL;
    adi_adrv9025_gp_MaskArray_t maskArray  = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Clear status register */
    adi_adrv9025_GPIntClearStatusRegister(device);

    /* Setup GP int masks */
    maskArray.gpInt0Mask = gpIntInit->gpIntMaskPin0;
    maskArray.gpInt1Mask = gpIntInit->gpIntMaskPin1;
    adi_adrv9025_GpIntMaskSet(device,
                              maskSelect,
                              &maskArray);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TxRampDownInit(adi_adrv9025_Device_t* device,
                                    adi_adrv9025_Init_t*   init)
{
    static const uint8_t  CLEAR_RAMPDOWN_MASK   = 0x07;
    static const uint32_t VALID_RAMPDOWN_EVENTS =
    ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_DFRMIRQ0 |
    ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_DFRMIRQ1 |
    ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_RF1PLLUNLOCK |
    ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_RF2PLLUNLOCK |
    ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_SERDESPLLUNLOCK |
    ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_CLKPLLUNLOCK |
    ADI_ADRV9025_TX_RAMP_DOWN_AUTOSELECT;
    int32_t                          recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    adi_adrv9025_TxRampDownInitCfg_t txRampDownInitCfg = {0};
    uint8_t                          i                 = 0;
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        init);

#if ADI_ADRV9025_INIT_RANGE_CHECK > 0
    recoveryAction = adi_adrv9025_ProfilesVerify(device,
                                                 init);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Init structure check failure");
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /* During Jesd link bring up, we're getting Dfrm Irq's. We should clear all previous Irq's before enabling ramp down */
    recoveryAction = adi_adrv9025_DfrmIrqSourceReset(device,
                                                     ADI_ADRV9025_DEFRAMER_0_AND_1);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Check that init->tx.txInitChannelMask is not zero*/
    if (init->tx.txInitChannelMask == ADI_ADRV9025_TXOFF)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         init->tx.txInitChannelMask,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < ADI_ADRV9025_MAX_TXCHANNELS; i++)
    {
        txRampDownInitCfg.txChannel = (adi_adrv9025_TxChannels_e)((uint8_t)(1 << i));

        if (init->tx.txInitChannelMask & txRampDownInitCfg.txChannel)
        {
            txRampDownInitCfg.txRampDownEvents = init->tx.txChannelCfg[i].txAttenCtrl.txRampDownEvents;
            if ((txRampDownInitCfg.txRampDownEvents & VALID_RAMPDOWN_EVENTS) != txRampDownInitCfg.txRampDownEvents)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 init->tx.txChannelCfg[i].txAttenCtrl.txRampDownEvents,
                                 "Invalid Tx Ramp Down Events Bitmask");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if ((txRampDownInitCfg.txChannel == ADI_ADRV9025_TX1) || (txRampDownInitCfg.txChannel == ADI_ADRV9025_TX2))
            {
                txRampDownInitCfg.txLoSel = init->clocks.tx12LoSelect;
            }
            else
            {
                txRampDownInitCfg.txLoSel = init->clocks.tx34LoSelect;
            }
            txRampDownInitCfg.deserializerLanesEnabledDfrm0 = init->dataInterface.deframer[0].deserializerLanesEnabled;
            txRampDownInitCfg.deserializerLanesEnabledDfrm1 = init->dataInterface.deframer[1].deserializerLanesEnabled;
            txRampDownInitCfg.serdesPllVcoFreq_kHz          = init->clocks.serdesPllVcoFreq_kHz;

            /* Clear previous ramp down events before enabling it */
            recoveryAction = adi_adrv9025_PaPllDfrmEventClear(device,
                                                              txRampDownInitCfg.txChannel,
                                                              CLEAR_RAMPDOWN_MASK);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             recoveryAction,
                             NULL,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Initialize Tx ramp down configuration */
            recoveryAction = adrv9025_TxRampDownInit(device,
                                                     &txRampDownInitCfg);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             recoveryAction,
                             NULL,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Enable TX Ramp up */
            recoveryAction = adi_adrv9025_PaProtectionRampUpEnableSet(device,
                                                                      (uint32_t)txRampDownInitCfg.txChannel,
                                                                      1);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             recoveryAction,
                             NULL,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_RxGainTableChecksumRead(adi_adrv9025_Device_t* device,
                                             const char*            rxGainTablePath,
                                             uint32_t*              rxGainTableChecksum)
{
#define RX_CHECKSUM_BUFFER_SIZE 9

    static const uint8_t NUM_COLUMNS     = 1;
    static const int8_t  CHECKSUM_OFFSET = -8;

    FILE*    rxGainTableFilePointer                             = NULL;
    char     rxGainTableChecksumBuffer[RX_CHECKSUM_BUFFER_SIZE] = {0};
    uint32_t checksum                                           = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_DEVICE_PTR_RETURN(rxGainTablePath);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    /* Open Rx Gain Table csv file */
#ifdef __GNUC__
    rxGainTableFilePointer = fopen(rxGainTablePath, "r");
#else
    if (fopen_s(&rxGainTableFilePointer, rxGainTablePath, "r") != 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, rxGainTablePath,
                         "Unable to open Rx Gain Table csv file. Please check if the path is correct or the file is open in another program");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif

    if (rxGainTableFilePointer == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxGainTablePath,
                         "Invalid Rx Gain Table csv file path while attempting to load Rx Gain Table");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Rewind the file pointer to ending of the file */
    if (fseek(rxGainTableFilePointer,
              CHECKSUM_OFFSET,
              SEEK_END) < 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxGainTablePath,
                         "Unable to move file descriptor to the ending of the Rx gain table file");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                               rxGainTableFilePointer);
    }

    /* Get the the last line when finding its corresponding beginning */
    if (fgets(rxGainTableChecksumBuffer,
              RX_CHECKSUM_BUFFER_SIZE,
              rxGainTableFilePointer) != NULL)
    {
        /* Convert characters to int */
        if (sscanf(rxGainTableChecksumBuffer,
                   "%Xu",
                   &checksum) != NUM_COLUMNS)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGainTablePath,
                             "checksum value wrong in the Rx gain table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   rxGainTableFilePointer);
        }
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxGainTablePath,
                         "Invalid checksum information in the Rx Gain Table");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                               rxGainTableFilePointer);
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

    *rxGainTableChecksum = checksum;

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_RxGainTableChecksumCalculate(adi_adrv9025_Device_t*    device,
                                                  adi_adrv9025_RxChannels_e rxChannel,
                                                  uint32_t*                 rxGainTableChecksum)
{
#define MAX_RX_GAIN_TABLE_LINES 256u
#define GAIN_TABLE_SIZE 2304u    //2304 = 256 * 9, the size of Rx gain table csv row is 9

    static const uint8_t INDEX_MASK         = 0xFF;
    static const uint8_t TIA_GAIN_MASK      = 0x1;
    static const uint8_t ADC_GAIN_MASK      = 0x1;
    static const uint8_t ADC_GAIN_SHIFT     = 1;
    static const uint8_t PHASE_OFFSET_MASK  = 0xFF;
    static const uint8_t PHASE_OFFSET_SHIFT = 8;
    static const uint8_t DIG_GAIN_MASK      = 0xFF;
    static const uint8_t DIG_GAIN_SHIFT     = 8;

    uint32_t recoveryAction     = 0;
    uint16_t numGainIndicesRead = 0;
    uint32_t i                  = 0;
    uint32_t offset             = 0;
    uint32_t checksum           = 0;

    adi_adrv9025_RxGainTableRow_t gainTableRow[MAX_RX_GAIN_TABLE_LINES] = { { 0 } };
    uint8_t                       gainTableData[GAIN_TABLE_SIZE]        = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that the channel requested is valid */
    if ((rxChannel != ADI_ADRV9025_RX1) &&
        (rxChannel != ADI_ADRV9025_RX2) &&
        (rxChannel != ADI_ADRV9025_RX3) &&
        (rxChannel != ADI_ADRV9025_RX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx Channel Requested for gain table read. Valid Rx channels include Rx1-Rx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adi_adrv9025_RxGainTableRead(device,
                                                  rxChannel,
                                                  ADI_ADRV9025_MAX_GAIN_TABLE_INDEX,
                                                  gainTableRow,
                                                  MAX_RX_GAIN_TABLE_LINES,
                                                  &numGainIndicesRead);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Rx Gain Table read failure");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (numGainIndicesRead != MAX_RX_GAIN_TABLE_LINES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numGainIndicesRead,
                         "Rx Gain Table read number of lines wrong");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < MAX_RX_GAIN_TABLE_LINES; i++)
    {
        gainTableData[offset++] = (uint8_t)(i & INDEX_MASK);
        gainTableData[offset++] = (uint8_t)(gainTableRow[i].rxFeGain);
        gainTableData[offset++] = (uint8_t)(gainTableRow[i].adcTiaGain & TIA_GAIN_MASK);
        gainTableData[offset++] = (uint8_t)((gainTableRow[i].adcTiaGain >> ADC_GAIN_SHIFT) & ADC_GAIN_MASK);
        gainTableData[offset++] = (uint8_t)(gainTableRow[i].extControl);
        gainTableData[offset++] = (uint8_t)(gainTableRow[i].phaseOffset & PHASE_OFFSET_MASK);
        gainTableData[offset++] = (uint8_t)((gainTableRow[i].phaseOffset >> PHASE_OFFSET_SHIFT) & PHASE_OFFSET_MASK);
        gainTableData[offset++] = (uint8_t)(gainTableRow[i].digGain & DIG_GAIN_MASK);
        gainTableData[offset++] = (uint8_t)((gainTableRow[i].digGain >> DIG_GAIN_SHIFT) & DIG_GAIN_MASK);
    }

    if (offset != GAIN_TABLE_SIZE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         offset,
                         "Rx Gain Table data size wrong");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    checksum = adrv9025_Crc32ForChunk(gainTableData,
                                      offset,
                                      checksum,
                                      1);

    *rxGainTableChecksum = checksum;

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TxAttenTableChecksumRead(adi_adrv9025_Device_t* device,
                                              const char*            txAttenTablePath,
                                              uint32_t*              txAttenTableChecksum)
{
#define TX_CHECKSUM_BUFFER_SIZE 9

    static const uint8_t NUM_COLUMNS     = 1;
    static const int8_t  CHECKSUM_OFFSET = -8;

    FILE*    txAttenTableFilePointer                             = NULL;
    char     txAttenTableChecksumBuffer[TX_CHECKSUM_BUFFER_SIZE] = {0};
    uint32_t checksum                                            = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_DEVICE_PTR_RETURN(txAttenTablePath);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    /* Open Tx Atten Table csv file */
#ifdef __GNUC__
    txAttenTableFilePointer = fopen(txAttenTablePath, "r");
#else
    if (fopen_s(&txAttenTableFilePointer, txAttenTablePath, "r") != 0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, txAttenTablePath,
                         "Unable to open Tx Atten Table csv file. Please check if the path is correct or the file is open in another program");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif

    if (txAttenTableFilePointer == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txAttenTablePath,
                         "Invalid Tx Atten Table csv file path while attempting to load Tx Atten Table");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Rewind the file pointer to ending of the file */
    if (fseek(txAttenTableFilePointer,
              CHECKSUM_OFFSET,
              SEEK_END) < 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txAttenTablePath,
                         "Unable to move file descriptor to the ending of the Tx atten table file");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                               txAttenTableFilePointer);
    }

    /* Get the the last line when finding its corresponding beginning */
    if (fgets(txAttenTableChecksumBuffer,
              TX_CHECKSUM_BUFFER_SIZE,
              txAttenTableFilePointer) != NULL)
    {
        /* Convert characters to int */
        if (sscanf(txAttenTableChecksumBuffer,
                   "%Xu",
                   &checksum) != NUM_COLUMNS)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             txAttenTablePath,
                             "checksum value wrong in the Tx atten table");
            ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                                   txAttenTableFilePointer);
        }
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txAttenTablePath,
                         "Invalid checksum information in the Rx Gain Table");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction,
                               txAttenTableFilePointer);
    }

    /* Close Rx Gain Table csv file */
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

    *txAttenTableChecksum = checksum;

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TxAttenTableChecksumCalculate(adi_adrv9025_Device_t*    device,
                                                   adi_adrv9025_TxChannels_e txChannel,
                                                   uint32_t*                 txAttenTableChecksum)
{
#define ATTEN_TABLE_SIZE 4800u   //4800 = 960 * 5, the size of Tx atten table row is 5

    static const uint8_t INDEX_MASK       = 0xFF;
    static const uint8_t INDEX_SHIFT      = 8;
    static const uint8_t ATTEN_MULT_MASK  = 0xFF;
    static const uint8_t ATTEN_MULT_SHIFT = 8;

    uint32_t recoveryAction = 0;
    uint32_t i              = 0;
    uint32_t offset         = 0;
    uint32_t checksum       = 0;

    adi_adrv9025_TxAttenTableRow_t txAttenTableRow[ADRV9025_TX_ATTEN_TABLE_MAX] = { { 0 } };
    uint8_t                        attenTableData[ATTEN_TABLE_SIZE]             = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that the channel requested is valid */
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
                         "Invalid Tx Channel Requested for atten table read. Valid Tx channels include Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adi_adrv9025_TxAttenTableRead(device,
                                                   txChannel,
                                                   0,
                                                   txAttenTableRow,
                                                   ADRV9025_TX_ATTEN_TABLE_MAX);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Tx Atten Table read failure");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < ADRV9025_TX_ATTEN_TABLE_MAX; i++)
    {
        attenTableData[offset++] = (uint8_t)(i & INDEX_MASK);
        attenTableData[offset++] = (uint8_t)((i >> INDEX_SHIFT) & INDEX_MASK);
        attenTableData[offset++] = (uint8_t)(txAttenTableRow[i].txAttenHp);
        attenTableData[offset++] = (uint8_t)(txAttenTableRow[i].txAttenMult & ATTEN_MULT_MASK);
        attenTableData[offset++] = (uint8_t)((txAttenTableRow[i].txAttenMult >> ATTEN_MULT_SHIFT) & ATTEN_MULT_MASK);
    }

    if (offset != ATTEN_TABLE_SIZE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         offset,
                         "Tx Atten Table data size wrong");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    checksum = adrv9025_Crc32ForChunk(attenTableData,
                                      offset,
                                      checksum,
                                      1);

    *txAttenTableChecksum = checksum;

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_RadioctrlInit(adi_adrv9025_Device_t        *device,
                                   adi_adrv9025_RadioctrlInit_t *radioCtrlInit)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    /* Check radioCtrlInit is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        radioCtrlInit);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    recoveryAction = adrv9025_RadioctrlInit(device,
                                            radioCtrlInit);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Unable to RadioctrlInit");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ApiArmStreamVersionCompare(adi_adrv9025_Device_t               *device,
                                                adi_adrv9025_ApiArmStreamVersions_t *srcVersion,
                                                uint8_t                             *result)
{
    const uint8_t API_VERSION_MASK    = 0x1u;
    const uint8_t ARM_VERSION_MASK    = 0x2u;
    const uint8_t STREAM_VERSION_MASK = 0x4u;

    uint32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    adi_adrv9025_ApiArmStreamVersions_t dstVersion = { {0 }};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Initial this result with 0 */
    *result = 0;

    recoveryAction = adi_adrv9025_ApiVersionGet(device,
                                                &(dstVersion.apiVersion));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "API version get failure");
    ADI_ERROR_RETURN(device->common.error.newAction);

    if ((srcVersion->apiVersion.majorVer != dstVersion.apiVersion.majorVer) ||
        (srcVersion->apiVersion.minorVer != dstVersion.apiVersion.minorVer) ||
        (srcVersion->apiVersion.maintenanceVer != dstVersion.apiVersion.maintenanceVer) ||
        (srcVersion->apiVersion.buildVer != dstVersion.apiVersion.buildVer))
    {
        *result |= API_VERSION_MASK;
    }

    recoveryAction = adi_adrv9025_ArmVersionGet(device,
                                                &(dstVersion.armVersion));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "ARM version get failure");
    ADI_ERROR_RETURN(device->common.error.newAction);

    if ((srcVersion->armVersion.majorVer != dstVersion.armVersion.majorVer) ||
        (srcVersion->armVersion.minorVer != dstVersion.armVersion.minorVer) ||
        (srcVersion->armVersion.maintVer != dstVersion.armVersion.maintVer) ||
        (srcVersion->armVersion.rcVer != dstVersion.armVersion.rcVer))
    {
        *result |= ARM_VERSION_MASK;
    }

    recoveryAction = adi_adrv9025_StreamVersionGet(device,
                                                   &(dstVersion.streamVersion));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Stream version get failure");
    ADI_ERROR_RETURN(device->common.error.newAction);

    if ((srcVersion->streamVersion.majorVer != dstVersion.streamVersion.majorVer) ||
        (srcVersion->streamVersion.minorVer != dstVersion.streamVersion.minorVer) ||
        (srcVersion->streamVersion.mainVer != dstVersion.streamVersion.mainVer) ||
        (srcVersion->streamVersion.buildVer != dstVersion.streamVersion.buildVer))
    {
        *result |= STREAM_VERSION_MASK;
    }

    return ADI_COMMON_ACT_NO_ACTION;
}
