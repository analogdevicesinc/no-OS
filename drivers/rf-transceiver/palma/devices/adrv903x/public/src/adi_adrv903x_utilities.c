/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_adrv903x_utilities.c
* \brief Contains ADI Transceiver CPU functions
*        Analog Devices maintains and provides updates to this code layer.
*        The end user should not modify this file or any code in this directory.
*
* ADRV903X API Version: 2.12.1.4
*/

#include "adi_adrv903x_utilities.h"
#include "adi_adrv903x_cpu.h"
#include "adi_adrv903x_datainterface.h"
#include "adi_adrv903x_radioctrl.h"
#include "adi_adrv903x_cals.h"
#include "adi_adrv903x_tx.h"
#include "adi_library.h"

#include "../../private/include/adrv903x_init.h"
#include "../../private/include/adrv903x_cpu_archive_types.h"
#include "../../private/include/adrv903x_rx.h"
#include "../../private/include/adrv903x_cpu.h"
#include "../../private/include/adrv903x_gpio.h"
#include "../../private/include/adrv903x_datainterface.h"
#include "../../private/include/adrv903x_cpu_cmd_fast_attack.h"
#include "../../private/include/adrv903x_binloader.h"
#include "../../private/bf/adrv903x_bf_tx_dig.h"
#include "../../public/include/adi_adrv903x_version.h"
#include "../../public/include/adi_adrv903x_core.h"
#include "../../private/bf/adrv903x_bf_core.h"



#include "../../private/bf/adrv903x_bf_serdes_rxdig_8pack_regmap_core1p2.h"
#include "../../private/bf/adrv903x_bf_jesd_common.h"
#include "../../private/bf/adrv903x_bf_jrx_link.h"


#include "../../private/bf/adrv903x_bf_serdes_txdig_phy_regmap_core1p2.h"
#include "../../private/bf/adrv903x_bf_tx_funcs_types.h"
#include "../../private/bf/adrv903x_bf_orx_dig.h"
#include "../../private/bf/adrv903x_bf_pll_mem_map.h"
#include "../../private/bf/adrv903x_bf_jtx_link.h"

#define ADI_FILE    ADI_ADRV903X_FILE_PUBLIC_UTILITIES

/*****************************************************************************/
/***** Helper functions' prototypes ******************************************/
/*****************************************************************************/
static adi_adrv903x_ErrAction_e adrv903x_CpuLoadUtil(adi_adrv903x_Device_t* const              device,
                                                     const adi_adrv903x_cpuBinaryInfo_t* const cpuBinaryInfo,
                                                     FILE*                                     cpuImageFilePtr,
                                                     const adi_adrv903x_CpuType_e              cpuType);
static uint32_t adrv903x_insert64Bits(  uint8_t*            array,
                                        uint64_t            storeVariable);
static uint32_t adrv903x_insert32Bits(  uint8_t*            array,
                                        uint32_t            storeVariable);
static uint32_t adrv903x_insert16Bits(  uint8_t*            array,
                                        uint16_t            storeVariable);
static uint32_t adrv903x_insert8Bits(   uint8_t*            array,
                                        uint8_t             storeVariable);

static adi_adrv903x_ErrAction_e adrv903x_dumpMemoryRegion(  adi_adrv903x_Device_t* const device, 
                                                            FILE **ofp,
                                                            uint32_t startAddr, 
                                                            uint32_t endAddr, 
                                                            uint32_t* const dumpSize,
                                                            uint32_t *recordCrc);
static adi_adrv903x_ErrAction_e adrv903x_JrxRepairHistoryRangeCheck(adi_adrv903x_Device_t* const           device,
                                                                    adi_adrv903x_JrxRepairHistory_t* const RepairHistory);

static void adrv903x_EndiannessCheckAndConvert(void* const buffer, const size_t elementSize, const size_t elementCount);

#define VARIABLE_ASSIGNMENT(target, source)         \
        (target) = (source);                            \
        if(ADRV903X_LITTLE_ENDIAN == 0)             \
        {                                           \
            adrv903x_EndiannessCheckAndConvert(&(target), sizeof((target)), 1);\
        }\

static size_t adrv903x_VariableFromFileExtract(adi_adrv903x_Device_t* const device, 
                                                void* const buffer, 
                                                const size_t elementSize, 
                                                const size_t elementCount, 
                                                FILE* file);

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_CpuImageLoad(adi_adrv903x_Device_t* const              device,
                                                           const adi_adrv903x_cpuBinaryInfo_t* const cpuBinaryInfo)
{
        adi_adrv903x_ErrAction_e    recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    FILE*                       cpuImageFilePtr     = NULL;
    uint32_t                    fileSize            = 0U;
    uint32_t                    byteCount           = 0U;
    uint32_t                    expFileSize         = 0U;
    adrv903x_CpuArchiveHeader_t archiveHdr;
    uint32_t                    i                   = 0u;
    uint32_t                    expectedXSum        = 0U;
    uint8_t*                    archivePtr          = NULL;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, cpuBinaryInfo, cleanup);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, cpuBinaryInfo->filePath, cleanup);

    ADI_LIBRARY_MEMSET(&archiveHdr, 0, sizeof(adrv903x_CpuArchiveHeader_t));

    if (ADI_LIBRARY_STRNLEN((const char*)cpuBinaryInfo->filePath, ADI_ADRV903X_MAX_FILE_LENGTH) == ADI_ADRV903X_MAX_FILE_LENGTH)
    {
        /* Path is not terminated */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Unterminated path string");
        goto cleanup;
    }

    /* Open CPU archive file */
#ifdef __GNUC__
    cpuImageFilePtr = ADI_LIBRARY_FOPEN((const char *)cpuBinaryInfo->filePath, "rb");
#else
    if (ADI_LIBRARY_FOPEN_S(&cpuImageFilePtr, (const char *)cpuBinaryInfo->filePath, "rb") != 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Invalid Binary File or Path Detected");
        goto cleanup;
    }
#endif
    if (NULL == cpuImageFilePtr)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Invalid Binary File or Path Detected");
        goto cleanup;
    }

    /* Determine file size */
    if (ADI_LIBRARY_FSEEK(cpuImageFilePtr, 0, SEEK_END) != 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Seek to EOF Failed");
        goto cleanup;
    }

    /* ADI_LIBRARY_FTELL returns long type */
    fileSize = (uint32_t) ADI_LIBRARY_FTELL(cpuImageFilePtr);

    /* Check if CPU Image File is Empty */
    if (fileSize == 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Zero Length CPU Image Detected");
        goto cleanup;
    }

    /* Check that size of the file is a multiple of 4 */
    if ((fileSize & 0x3U) != 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Incorrect CPU Binary File Block Size");
        goto cleanup;
    }

    /* Verify file contains at least the archive header */
    if (fileSize < sizeof(adrv903x_CpuArchiveHeader_t))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Invalid CPU image file detected (very small)");
        goto cleanup;
    }

    /* Rewind the file pointer to beginning of the file */
    if (ADI_LIBRARY_FSEEK(cpuImageFilePtr, 0U, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    /* Read out the archive header */
    byteCount = ADI_LIBRARY_FREAD(&archiveHdr, 1, sizeof(archiveHdr), cpuImageFilePtr);
    if (byteCount != sizeof(archiveHdr))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Error reading header from CPU image file");
        goto cleanup;
    }

    /* Extract archive header contents (stored in CPU byte order) */
    archiveHdr.magicNum = ADRV903X_CTOHL(archiveHdr.magicNum);
    archiveHdr.formatRev = ADRV903X_CTOHL(archiveHdr.formatRev);
    archiveHdr.xsum = ADRV903X_CTOHL(archiveHdr.xsum);

    /* Calculate the expected archive header checksum
     * (inverted byte sum of magicNum and formatRev)
     */
    expectedXSum = 0U;
    archivePtr = (uint8_t*)&archiveHdr;
    for (i = 0U; i < sizeof(archiveHdr) - sizeof(archiveHdr.xsum); i++)
    {
        expectedXSum += archivePtr[i];
    }
    expectedXSum = ~expectedXSum;

    /* Verify header checksum is correct */
    if (expectedXSum != archiveHdr.xsum)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Corrupted CPU image file detected");
        goto cleanup;
    }

    /* Verify header magic number is correct */
    if (archiveHdr.magicNum != ADRV903X_CPU_ARCHIVE_MAGIC_NUM)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Unsupported CPU image file detected");
        goto cleanup;
    }

    /* Verify archive format revision is correct */
    if (archiveHdr.formatRev != ADRV903X_CPU_ARCHIVE_REV_1)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Unsupported CPU image file detected");
        goto cleanup;
    }

    /* Verify file size is correct (header + two CPU images) */
    expFileSize = sizeof(adrv903x_CpuArchiveHeader_t) + ADI_ADRV903X_CPU_0_BINARY_IMAGE_FILE_SIZE_BYTES + ADI_ADRV903X_CPU_1_BINARY_IMAGE_FILE_SIZE_BYTES;
    if (fileSize != expFileSize)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Invalid CPU image file detected");
        goto cleanup;
    }

    /* Put the part into streaming mode to help facilitate the large, contiguous SPI writes in CpuImageWrite */
    recoveryAction = adrv903x_SpiStreamingEntry(device);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with entering Spi Streaming mode");
        goto cleanup;
    }

    /* Load CPU0 image */
    recoveryAction = adrv903x_CpuLoadUtil(device, cpuBinaryInfo, cpuImageFilePtr, ADI_ADRV903X_CPU_TYPE_0);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error loading CPU0 image");
        goto cleanup;
    }

    /* Load CPU1 image */
    recoveryAction = adrv903x_CpuLoadUtil(device, cpuBinaryInfo, cpuImageFilePtr, ADI_ADRV903X_CPU_TYPE_1);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error loading CPU1 image");
        goto cleanup;
    }

    /* Load successful. Update device state. */
    device->devStateInfo.devState = (adi_adrv903x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV903X_STATE_CPU0LOADED | ADI_ADRV903X_STATE_CPU1LOADED);

    /* Close CPU Image File */
    if (0 == ADI_LIBRARY_FCLOSE(cpuImageFilePtr))
    {
        cpuImageFilePtr = NULL;
    }
    else
    {
        cpuImageFilePtr = NULL;
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Cannot Close CPU Image File");
        goto cleanup;
    }

cleanup :
    if (cpuImageFilePtr != NULL)
    {
        /* Close CPU Image File */
        if (0 != ADI_LIBRARY_FCLOSE(cpuImageFilePtr))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close CPU Image File");
        }
    }

    if (device->devStateInfo.spiStreamingOn == ADI_TRUE)
    {
        if( ADI_ADRV903X_ERR_ACT_NONE != adrv903x_SpiStreamingExit(device))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_RESET_INTERFACE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Spi Streaming Exit Issue");
        }
    }

    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_StreamImageLoad(adi_adrv903x_Device_t* const                    device,
                                                              const adi_adrv903x_streamBinaryInfo_t* const    streamBinaryInfoPtr)
{
        const size_t                BIN_ELEMENT_SIZE        = 1U;
    adi_adrv903x_ErrAction_e    recoveryAction          = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    FILE*                       streamBinaryFilePtr     = NULL;
    uint32_t                    fileSize                = 0U;
    uint32_t                    numFileChunks           = 0U;
    uint32_t                    chunkIndex              = 0U;
    uint8_t                     streamBinaryImageBuffer[ADI_ADRV903X_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, streamBinaryInfoPtr, cleanup);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, streamBinaryInfoPtr->filePath, cleanup);

    if (ADI_LIBRARY_STRNLEN((const char*)streamBinaryInfoPtr->filePath, ADI_ADRV903X_MAX_FILE_LENGTH) == ADI_ADRV903X_MAX_FILE_LENGTH)
    {
        /* Path is not terminated */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, streamBinaryInfoPtr->filePath, "Unterminated path string");
        goto cleanup;
    }

    /* Open Stream Binary File */
#ifdef __GNUC__
    streamBinaryFilePtr = ADI_LIBRARY_FOPEN((const char *)streamBinaryInfoPtr->filePath, "rb");
#else
    if (ADI_LIBRARY_FOPEN_S(&streamBinaryFilePtr, (const char *)streamBinaryInfoPtr->filePath, "rb") != 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, streamBinaryInfoPtr->filePath, "Invalid Binary File or Path Detected");
        goto cleanup;
    }
#endif

    if (NULL == streamBinaryFilePtr)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, streamBinaryInfoPtr->filePath, "Invalid Binary File or Path Detected");
        goto cleanup;
    }

    /*Determine file size*/
    if (ADI_LIBRARY_FSEEK(streamBinaryFilePtr, 0U, SEEK_END) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, streamBinaryFilePtr, "Seek to EOF Failed");
        goto cleanup;
    }

    /* ADI_LIBRARY_FTELL returns long type */
    fileSize = (uint32_t) ADI_LIBRARY_FTELL(streamBinaryFilePtr);
    numFileChunks = (uint32_t)(fileSize / ADI_ADRV903X_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES);

    /* Check that Stream binary file is not empty */
    if (fileSize == 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, streamBinaryFilePtr, "Zero Length Stream Image Detected");
        goto cleanup;
    }

    /* Check that Stream binary file size does not exceed maximum size */
    if (fileSize > ADI_ADRV903X_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileSize, "Stream Binary Size exceeds Maximum Limit");
        goto cleanup;
    }

    /* Check that size of the file is a multiple of 4 */
    if ((fileSize & 0x3U) != 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, streamBinaryInfoPtr->filePath, "Incorrect Stream Binary File Block Size");
        goto cleanup;
    }

    /* Rewind the file pointer to beginning of the file */
    if (ADI_LIBRARY_FSEEK(streamBinaryFilePtr, 0U, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, streamBinaryFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    /* Put the part into streaming mode to help facilitate the large, contiguous SPI writes in CpuImageWrite */
    recoveryAction = adrv903x_SpiStreamingEntry(device);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with entering Spi Streaming mode");
        goto cleanup;
    }

    /* Read Stream binary file */
    for (chunkIndex = 0U; chunkIndex < numFileChunks; ++chunkIndex)
    {
        if (ADI_LIBRARY_FREAD(&streamBinaryImageBuffer[0U], BIN_ELEMENT_SIZE, ADI_ADRV903X_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES, streamBinaryFilePtr) < ADI_ADRV903X_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numFileChunks, "Error Reading Block of Data from CPU Binary File");
            goto cleanup;
        }

        /* Write Stream binary */
        recoveryAction = adi_adrv903x_StreamImageWrite( device,
                                                        (chunkIndex * ADI_ADRV903X_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES),
                                                        &streamBinaryImageBuffer[0U],
                                                        ADI_ADRV903X_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue During Binary Loading (e.g. SPI Issue)");
            goto cleanup;
        }
    }


cleanup :
    if (streamBinaryFilePtr != NULL)
    {
        /* Close Stream binary file */
        if (0 != ADI_LIBRARY_FCLOSE(streamBinaryFilePtr))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close Stream Image File");
        }
    }

    if (device->devStateInfo.spiStreamingOn == ADI_TRUE)
    {
        if (ADI_ADRV903X_ERR_ACT_NONE != adrv903x_SpiStreamingExit(device))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_RESET_INTERFACE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Spi Streaming Exit Issue");
        }
    }

    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainTableLoad(adi_adrv903x_Device_t* const            device,
                                                              const adi_adrv903x_RxGainTableInfo_t    rxGainTableInfo[],
                                                              const uint32_t                          rxGainTableArrSize)
{
        const uint8_t                   LINE_BUFFER_SIZE    = 128U;
    const uint8_t                   HEADER_BUFFER_SIZE  = 16U;
    const uint8_t                   NUM_COLUMNS         = 5U;
    adi_adrv903x_ErrAction_e        recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t                         arrayIndex          = 0U;
    uint8_t                         minGainIndex        = 0U;
    uint8_t                         maxGainIndex        = 0U;
    uint8_t                         prevGainIndex       = 0U;
    uint8_t                         gainIndex           = 0U;
    uint16_t                        lineCount           = 0U;
    FILE*                           rxGainTableFilePtr  = NULL;
    char                            rxGainTableLineBuffer[LINE_BUFFER_SIZE];
    char                            headerStr1[HEADER_BUFFER_SIZE];
    char                            headerStr2[HEADER_BUFFER_SIZE];
    char                            headerStr3[HEADER_BUFFER_SIZE];
    char                            headerStr4[HEADER_BUFFER_SIZE];
    char                            headerStr5[HEADER_BUFFER_SIZE];
    adi_adrv903x_RxGainTableRow_t   rxGainTableRowBuffer[ADI_ADRV903X_RX_GAIN_TABLE_SIZE_ROWS];
    adi_adrv903x_Version_t          tableVersion        = { 0, 0, 0, 0 };
    uint32_t                        checksum[4U]        = { 0, 0, 0, 0 };

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    if (0U == rxGainTableArrSize)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableArrSize, "Invalid Array Size");
        goto cleanup;
    }

    for (arrayIndex = 0U; arrayIndex < rxGainTableArrSize; ++arrayIndex)
    {
        ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxGainTableInfo[arrayIndex].filePath, cleanup);

        if (0U == rxGainTableInfo[arrayIndex].channelMask)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfo[arrayIndex].channelMask, "Invalid Rx Gain Table Channel Mask Detected");
            goto cleanup;
        }

        if (ADI_LIBRARY_STRNLEN((const char*)rxGainTableInfo[arrayIndex].filePath, ADI_ADRV903X_MAX_FILE_LENGTH) == ADI_ADRV903X_MAX_FILE_LENGTH)
        {
            /* Path is not terminated */
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfo[arrayIndex].filePath, "Unterminated path string");
            goto cleanup;
        }

        /* Open Rx Gain Table CSV file */
#ifdef __GNUC__
        rxGainTableFilePtr = ADI_LIBRARY_FOPEN((const char *)rxGainTableInfo[arrayIndex].filePath, "r");
#else
        if (ADI_LIBRARY_FOPEN_S(&rxGainTableFilePtr, (const char *)rxGainTableInfo[arrayIndex].filePath, "r") != 0)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfo[arrayIndex].filePath, "File is already Open or Invalid CSV File/Path Detected");
            goto cleanup;
        }
#endif
        if (rxGainTableFilePtr == NULL)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfo[arrayIndex].filePath, "File is already Open or Invalid CSV File/Path Detected");
            goto cleanup;
        }



        /* Check for empty Rx Gain Table */
        if (ADI_LIBRARY_FGETS(rxGainTableLineBuffer, sizeof(rxGainTableLineBuffer), rxGainTableFilePtr) != NULL)
        {

            /* Parse the first line of the Rx Gain Table file which contains the version info */
#ifdef __GNUC__
#ifdef __KERNEL__
            /* Copy the substring before the comma, should be Version */
            strncpy(headerStr1, rxGainTableLineBuffer, 7);
            if (sscanf(rxGainTableLineBuffer,
                       "Version,%u,%u,%u,%u",
                       (uint32_t*)&tableVersion.majorVer,
                       (uint32_t*)&tableVersion.minorVer,
                       (uint32_t*)&tableVersion.maintenanceVer,
                       (uint32_t*)&tableVersion.buildVer) != (NUM_COLUMNS - 1))
#else
            if (sscanf(rxGainTableLineBuffer,
                       "%[^,],%u,%u,%u,%u",
                       headerStr1,
                       (uint32_t*)&tableVersion.majorVer,
                       (uint32_t*)&tableVersion.minorVer,
                       (uint32_t*)&tableVersion.maintenanceVer,
                       (uint32_t*)&tableVersion.buildVer) != NUM_COLUMNS)
#endif
#else
                    if (sscanf_s(rxGainTableLineBuffer,
                                 "%[^,],%d,%d,%d,%d",
                                 headerStr1,
                                 (uint32_t)sizeof(headerStr1),
                                 &version.majorVer,
                                 &version.minorVer,
                                 &version.maintenanceVer,
                                 &version.buildVer) != NUM_COLUMNS)
#endif
                {
                    recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfo[arrayIndex].filePath, "Invalid Rx Gain Table Format Detected");
                    goto cleanup;
                }

                /* Verify that Gain Table Version Format is correct */
                if (ADI_LIBRARY_STRSTR(headerStr1, "Version") == NULL)
                {
                    recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr1, "Version Column Expected First");
                    goto cleanup;
                }


#if ADI_ADRV903X_API_VERSION_RANGE_CHECK > 0
            {
                adi_adrv903x_Version_t currentVersion = { 0U, 0U, 0U, 0U };

                                recoveryAction = adi_adrv903x_ApiVersionGet(device, &currentVersion);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainTable Version Get Issue");
                    goto cleanup;
                }

                recoveryAction = adrv903x_ApiVersionRangeCheck(device, &tableVersion, &currentVersion, &currentVersion);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainTable Version Check Issue");
                    goto cleanup;
                }
            }
#endif

            if (ADI_LIBRARY_FGETS(rxGainTableLineBuffer, sizeof(rxGainTableLineBuffer), rxGainTableFilePtr) == NULL)
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableFilePtr, "Empty Rx Gain Table Detected");
                goto cleanup;
            }

            /* Parse the second line of the Rx Gain Table file which contains the checksum info */
#ifdef __GNUC__
#ifdef __KERNEL__
            /* Copy the substring before the comma, should be Version */
            strncpy(headerStr1, rxGainTableLineBuffer, 8);
            if (sscanf(rxGainTableLineBuffer,
                       "Checksum,%u,%u,%u,%u",
                       (uint32_t*)&checksum[0],
                       (uint32_t*)&checksum[1],
                       (uint32_t*)&checksum[2],
                       (uint32_t*)&checksum[3]) != NUM_COLUMNS -1)
#else
            if (sscanf(rxGainTableLineBuffer,
                       "%[^,],%u,%u,%u,%u",
                       headerStr1,
                       (uint32_t*)&checksum[0],
                       (uint32_t*)&checksum[1],
                       (uint32_t*)&checksum[2],
                       (uint32_t*)&checksum[3]) != NUM_COLUMNS)
#endif
#else
                if (sscanf_s(rxGainTableLineBuffer,
                             "%[^,],%d,%d,%d,%d",
                             headerStr1,
                             (uint32_t)sizeof(headerStr1),
                             &checksum[0],
                             &checksum[1],
                             &checksum[2],
                             &checksum[3]) != NUM_COLUMNS)
#endif
                {
                    recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfo[arrayIndex].filePath, "Invalid Rx Gain Table Format Detected");
                    goto cleanup;
                }

            /* Verify that Gain Table Version Format is correct */
            if (ADI_LIBRARY_STRSTR(headerStr1, "Checksum") == NULL)
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr1, "Checksum Column Expected First");
                goto cleanup;
            }

            if (ADI_LIBRARY_FGETS(rxGainTableLineBuffer, sizeof(rxGainTableLineBuffer), rxGainTableFilePtr) == NULL)
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableFilePtr, "Empty Rx Gain Table Detected");
                goto cleanup;
            }

            /* Parse the third line of the Rx Gain Table file which contains header strings for each column in the table */
#ifdef __GNUC__
            if (sscanf( rxGainTableLineBuffer,
                        "%[^,],%[^,],%[^,],%[^,],%[^\n]",
                        headerStr1,
                        headerStr2,
                        headerStr3,
                        headerStr4,
                        headerStr5) != NUM_COLUMNS)
#else
                if (sscanf_s(   rxGainTableLineBuffer,
                                "%[^,],%[^,],%[^,],%[^,],%[^\n]",
                                headerStr1,
                                (uint32_t)sizeof(headerStr1),
                                headerStr2,
                                (uint32_t)sizeof(headerStr2),
                                headerStr3,
                                (uint32_t)sizeof(headerStr3),
                                headerStr4,
                                (uint32_t)sizeof(headerStr4),
                                headerStr5,
                                (uint32_t)sizeof(headerStr5)) != NUM_COLUMNS)
#endif
#ifdef __KERNEL__
        for(unsigned int z = 0; z < HEADER_BUFFER_SIZE; z++) {
            headerStr1[z] = '\0';
            headerStr2[z] = '\0';
            headerStr3[z] = '\0';
            headerStr4[z] = '\0';
            headerStr5[z] = '\0';
        }

            strncpy(headerStr1, rxGainTableLineBuffer, 10);
            strncpy(headerStr2, &rxGainTableLineBuffer[11], 15);
            strncpy(headerStr3, &rxGainTableLineBuffer[27], 11);
            strncpy(headerStr4, &rxGainTableLineBuffer[39], 12);
            strncpy(headerStr5, &rxGainTableLineBuffer[52], 12);
#else
                {
                    recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfo[arrayIndex].filePath, "Invalid Rx Gain Table Format Detected");
                    goto cleanup;
                }
#endif

            /* Verify that Gain Table Format is correct */
            if (ADI_LIBRARY_STRSTR(headerStr1, "Gain Index") == NULL)
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr1, "Gain Index Column Expected First");
                goto cleanup;
            }

            if (ADI_LIBRARY_STRSTR(headerStr2, "FE Control Word") == NULL)
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr2, "FE Control Column Expected Second");
                goto cleanup;
            }

            if (ADI_LIBRARY_STRSTR(headerStr3, "Ext Control") == NULL)
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr3, "Ext Control Column Expected Third");
                goto cleanup;
            }

            if (ADI_LIBRARY_STRSTR(headerStr4, "Phase Offset") == NULL)
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr4, "Phase Offset Column Expected Fourth");
                goto cleanup;
            }

            if (ADI_LIBRARY_STRSTR(headerStr5, "Digital Gain") == NULL)
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr5, "Digital Gain Column Expected Fifth");
                goto cleanup;
            }

            /* Put the part into streaming mode to help facilitate the large, contiguous SPI writes in CpuImageWrite */
            recoveryAction = adrv903x_SpiStreamingEntry(device);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with entering Spi Streaming mode");
                goto cleanup;
            }

            /* Clear line counter */
            lineCount = 0U;

            /* Loop until the gain table end is reached or no. of lines scanned exceeds maximum */
            while ((ADI_LIBRARY_FGETS(rxGainTableLineBuffer, sizeof(rxGainTableLineBuffer), rxGainTableFilePtr) != NULL) &&
                   (lineCount <  ADI_ADRV903X_RX_GAIN_TABLE_SIZE_ROWS))
            {
#ifdef __GNUC__
                if (sscanf(rxGainTableLineBuffer,
                    "%hhu,%hhu,%hhu,%hu,%hd",
                    &gainIndex,
                    &rxGainTableRowBuffer[lineCount].rxFeGain,
                    &rxGainTableRowBuffer[lineCount].extControl,
                    &rxGainTableRowBuffer[lineCount].phaseOffset,
                    &rxGainTableRowBuffer[lineCount].digGain) != NUM_COLUMNS)
#else
                    if (sscanf_s(rxGainTableLineBuffer,
                        "%hhu,%hhu,%hhu,%hu,%hd",
                        &gainIndex,
                        &rxGainTableRowBuffer[lineCount].rxFeGain,
                        &rxGainTableRowBuffer[lineCount].extControl,
                        &rxGainTableRowBuffer[lineCount].phaseOffset,
                        &rxGainTableRowBuffer[lineCount].digGain) != NUM_COLUMNS)
#endif
                    {
                        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gainIndex, "Insufficient entries in Rx gain table row entry");
                        goto cleanup;
                    }

                if (lineCount == 0U)
                {
                    minGainIndex = gainIndex;
                }
                else
                {
                    /* Check that gain indices are arranged in ascending order */
                    if (prevGainIndex != (gainIndex - 1U))
                    {
                        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gainIndex, "Gain Indices not in Ascending Order");
                        goto cleanup;
                    }
                }

                prevGainIndex = gainIndex;
                lineCount++;
            }

            maxGainIndex = gainIndex;

            recoveryAction = adi_adrv903x_RxGainTableWrite( device,
                                                            (rxGainTableInfo[arrayIndex].channelMask & device->devStateInfo.initializedChannels),
                                                            maxGainIndex,
                                                            &rxGainTableRowBuffer[0U],
                                                            lineCount);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with RX Gain Table Writing");
                goto cleanup;
            }

            recoveryAction = adi_adrv903x_RxMinMaxGainIndexSet( device,
                                                                (rxGainTableInfo[arrayIndex].channelMask & device->devStateInfo.initializedChannels),
                                                                minGainIndex,
                                                                maxGainIndex);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with Setting Min/Max Gain Index");
                goto cleanup;
            }
        }
        else
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableFilePtr, "Empty Rx Gain Table Detected");
            goto cleanup;
        }

        /* Close Rx Gain Table csv file, (part of for loop)  */
        if (0 == ADI_LIBRARY_FCLOSE(rxGainTableFilePtr))
        {
            rxGainTableFilePtr = NULL;
        }
        else
        {
            rxGainTableFilePtr = NULL;
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close CSV File");
            goto cleanup;
        }
    }

cleanup :
    if (rxGainTableFilePtr != NULL)
    {
        /* Close Rx Gain Table csv file */
        if (0 != ADI_LIBRARY_FCLOSE(rxGainTableFilePtr))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close CSV File");
        }
    }

    if (device->devStateInfo.spiStreamingOn == ADI_TRUE)
    {
        if (ADI_ADRV903X_ERR_ACT_NONE != adrv903x_SpiStreamingExit(device))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_RESET_INTERFACE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Spi Streaming Exit Issue");
        }
    }

    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_CpuProfileImageLoad(adi_adrv903x_Device_t* const                        device,
                                                                  const adi_adrv903x_CpuProfileBinaryInfo_t* const    cpuProfileBinaryInfoPtr)
{
        const size_t                BIN_ELEMENT_SIZE    = 1U;
    adi_adrv903x_ErrAction_e    recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    FILE*                       cpuProfileFilePtr   = NULL;
    uint32_t                    totalFileSize       = 0U;
    uint32_t                    numFileChunks       = 0U;
    uint32_t                    chunkIndex          = 0U;
    uint32_t                    byteCount           = 0U;
    uint8_t                     cpuProfileBinaryImageBuffer[ADI_ADRV903X_CPU_PROFILE_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, cpuProfileBinaryInfoPtr, cleanup);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, cpuProfileBinaryInfoPtr->filePath, cleanup);

    if (ADI_LIBRARY_STRNLEN((const char*)cpuProfileBinaryInfoPtr->filePath, ADI_ADRV903X_MAX_FILE_LENGTH) == ADI_ADRV903X_MAX_FILE_LENGTH)
    {
        /* Path is not terminated */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileBinaryInfoPtr->filePath, "Unterminated path string");
        goto cleanup;
    }

    /* Open ARM binary file */
#ifdef __GNUC__
    cpuProfileFilePtr = ADI_LIBRARY_FOPEN((const char *)cpuProfileBinaryInfoPtr->filePath, "rb");
#else
    if (ADI_LIBRARY_FOPEN_S(&cpuProfileFilePtr, (const char *)cpuProfileBinaryInfoPtr->filePath, "rb") != 0)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileBinaryInfoPtr->filePath, "Unable to open CPU Profile Binary. Please check filepath is correct");
            goto cleanup;
        }
#endif

    if (NULL == cpuProfileFilePtr)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to open CPU Profile Binary. Please check filepath is correct")
        goto cleanup;
    }

    /* Determine file size */
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, 0U, SEEK_END) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Seek to EOF Failed");
        goto cleanup;
    }

    /* ADI_LIBRARY_FTELL returns long type */
    totalFileSize = (uint32_t) ADI_LIBRARY_FTELL(cpuProfileFilePtr);

    /* Check that FW binary file is not empty */
    if (0U == totalFileSize)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, totalFileSize, "Zero Length CPU Profile Binary Detected");
        goto cleanup;
    }

    /* Check that FW Profile binary file size is not smaller than server size */
    if (totalFileSize < ADRV903X_DEVICE_PROFILE_SIZE_BYTES)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, totalFileSize, "CPU Profile Binary Size smaller than Server ADRV903X_DEVICE_PROFILE_SIZE_BYTES");
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ADRV903X_DEVICE_PROFILE_SIZE_BYTES, "ADRV903X_DEVICE_PROFILE_SIZE_BYTES bigger than CPU Profile Binary Size");
        goto cleanup;
    }
    
    /*check if the CPU Profile Binary Image Size is perfectly divisible by the chunk size*/
    if ((ADRV903X_DEVICE_PROFILE_SIZE_BYTES & 0x3U) != 0U) 
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Incorrect CPU Profile Binary Structure Size");
        goto cleanup;
    }

    /* Calculate the number of chunks to be written for radio profile */
    numFileChunks = (uint32_t)(ADRV903X_DEVICE_PROFILE_SIZE_BYTES / ADI_ADRV903X_CPU_PROFILE_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES) + 1;

    /* Rewind the file pointer to beginning of the file */
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, 0U, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    /* Put the part into streaming mode to help facilitate the large, contiguous SPI writes in CpuImageWrite */
    recoveryAction = adrv903x_SpiStreamingEntry(device);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with entering Spi Streaming mode");
        goto cleanup;
    }

    /* Read ARM binary file */
    for (chunkIndex = 0U; chunkIndex < numFileChunks; ++chunkIndex)
    {
        if ((byteCount = ADI_LIBRARY_FREAD(&cpuProfileBinaryImageBuffer[0U], BIN_ELEMENT_SIZE, ADI_ADRV903X_CPU_PROFILE_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES, cpuProfileFilePtr)) > 0)
        {
            /* In case the bin file has also the Init_t and PostMcsInit_t, keep the last chunkSize only the part belonging to profile */
            if (chunkIndex == (numFileChunks - 1U))
            {
                byteCount = ADRV903X_DEVICE_PROFILE_SIZE_BYTES - (chunkIndex) * ADI_ADRV903X_CPU_PROFILE_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES;
            }
            
            /* Write the CPU Profile binary chunk */
            recoveryAction = adi_adrv903x_CpuProfileWrite(  device,
                                                            (chunkIndex * ADI_ADRV903X_CPU_PROFILE_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES),
                                                            &cpuProfileBinaryImageBuffer[0U],
                                                            byteCount);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during CPU Profile Write");
                goto cleanup;
            }
        }
        else if (ferror(cpuProfileFilePtr))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numFileChunks, "Error Reading Block of Data from CPU Binary File");
            goto cleanup;
        }
    }

cleanup:
    if (cpuProfileFilePtr != NULL)
    {
        /* Close CPU Profile binary file */
        if (0 != ADI_LIBRARY_FCLOSE(cpuProfileFilePtr))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close CPU Profile Binary File");
        }
    }

    if (device->devStateInfo.spiStreamingOn == ADI_TRUE)
    {
        if (ADI_ADRV903X_ERR_ACT_NONE != adrv903x_SpiStreamingExit(device))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_RESET_INTERFACE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Spi Streaming Exit Issue");
        }
    }

    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_DeviceInfoExtract(adi_adrv903x_Device_t* const                        device,
                                                                const adi_adrv903x_CpuProfileBinaryInfo_t* const    cpuProfileBinaryInfoPtr)
{
        typedef struct {
        adrv903x_TxConfig_t             txConfig;
        adrv903x_RxConfig_t             rxConfig;
        adrv903x_OrxConfig_t            orxConfig;
        adrv903x_JesdSettings_t         jesdProfile;
    } configs_t;
    adi_adrv903x_ErrAction_e        recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    FILE*                           cpuProfileFilePtr   = NULL;
    uint32_t                        fileSize            = 0U;
    adrv903x_DeviceProfile_t* const deviceProfile       = NULL;
    uint32_t                        offset              = 0U;
    uint32_t                        readSize            = 0U;
    uint32_t                        idx                 = 0U;
    uint32_t                        bandIdx             = 0U;
    uint32_t                        targetIdx           = 0U;
    uint32_t                        digMask             = 0U;

    ADI_PLATFORM_LARGE_VAR_ALLOC(configs_t, config);

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, cpuProfileBinaryInfoPtr, cleanup);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, cpuProfileBinaryInfoPtr->filePath, cleanup);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, config, cleanup);

    if (ADI_LIBRARY_STRNLEN((const char*)cpuProfileBinaryInfoPtr->filePath, ADI_ADRV903X_MAX_FILE_LENGTH) == ADI_ADRV903X_MAX_FILE_LENGTH)
    {
        /* Path is not terminated */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileBinaryInfoPtr->filePath, "Unterminated path string");
        goto cleanup;
    }

    /* Open ARM binary file */
#ifdef __GNUC__
    cpuProfileFilePtr = ADI_LIBRARY_FOPEN((const char *)cpuProfileBinaryInfoPtr->filePath, "rb");
#else
    if (ADI_LIBRARY_FOPEN_S(&cpuProfileFilePtr, (const char *)cpuProfileBinaryInfoPtr->filePath, "rb") != 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileBinaryInfoPtr->filePath, "Unable to open CPU Profile Binary. Please check filepath is correct");
        goto cleanup;
    }
#endif

    if (NULL == cpuProfileFilePtr)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to open CPU Profile Binary. Please check filepath is correct")
        goto cleanup;
    }

    /* Determine file size */
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, 0U, SEEK_END) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Seek to EOF Failed");
        goto cleanup;
    }

    /* ADI_LIBRARY_FTELL returns long type */
    fileSize = (uint32_t) ADI_LIBRARY_FTELL(cpuProfileFilePtr);

    /* Check that FW binary file is not empty */
    if (0U == fileSize)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileSize, "Zero Length CPU Profile Binary Detected");
        goto cleanup;
    }

    /* Check that FW Profile binary file size is not smaller than server size */
    if (fileSize < ADRV903X_DEVICE_PROFILE_SIZE_BYTES)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileSize, "CPU Profile Binary Size smaller than Server ADRV903X_DEVICE_PROFILE_SIZE_BYTES");
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ADRV903X_DEVICE_PROFILE_SIZE_BYTES, "ADRV903X_DEVICE_PROFILE_SIZE_BYTES bigger than CPU Profile Binary Size");
        goto cleanup;
    }

    /* Clear all variables */
    ADI_LIBRARY_MEMSET(&device->initExtract, 0, sizeof(device->initExtract));

    /* Read scaled device clock frequency */
    offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, deviceClkScaledFreq_kHz);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((adrv903x_VariableFromFileExtract(device, &device->initExtract.clocks.deviceClockScaled_kHz, sizeof(deviceProfile->radioProfile.deviceClkScaledFreq_kHz), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read Pad Divider */
    offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, padDiv);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((adrv903x_VariableFromFileExtract(device, &device->initExtract.clocks.padDivideRatio, sizeof(deviceProfile->radioProfile.padDiv), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read armClkDiv */
    offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, armClkDiv);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((adrv903x_VariableFromFileExtract(device, &device->initExtract.clocks.armClkDivideRatio, sizeof(deviceProfile->radioProfile.armClkDiv), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read DevClk armClkDiv */
    offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, armClkDivDevClk);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((adrv903x_VariableFromFileExtract(device, &device->initExtract.clocks.armClkDevClkDivRatio, sizeof(deviceProfile->radioProfile.armClkDivDevClk), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read chanConfig */
    offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, chanConfig);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((adrv903x_VariableFromFileExtract(device, &device->initExtract.rx.rxInitChannelMask, sizeof(deviceProfile->radioProfile.chanConfig), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    device->initExtract.tx.txInitChannelMask = device->initExtract.rx.rxInitChannelMask & ADI_ADRV903X_TXALL;
    device->initExtract.rx.rxInitChannelMask = (device->initExtract.rx.rxInitChannelMask >> 8) & (ADI_ADRV903X_RX_MASK_ALL | ADI_ADRV903X_ORX_MASK_ALL);

    /* Read rxTxCpuConfig */
    offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, rxTxCpuConfig);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((adrv903x_VariableFromFileExtract(device, &device->initExtract.rxTxCpuConfig[0], sizeof(deviceProfile->radioProfile.rxTxCpuConfig), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read orxCpuConfig */
    offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, orxCpuConfig);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((adrv903x_VariableFromFileExtract(device, &device->initExtract.orxCpuConfig[0], sizeof(deviceProfile->radioProfile.orxCpuConfig), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read chanAssign */
    offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, chanAssign);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((adrv903x_VariableFromFileExtract(device, &device->initExtract.chanAssign[0], sizeof(deviceProfile->radioProfile.chanAssign), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read hsDigClk_kHz */
    offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, hsDigFreq_kHz);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((ADI_LIBRARY_FREAD(&device->initExtract.clocks.hsDigClk_kHz, sizeof(deviceProfile->radioProfile.hsDigFreq_kHz), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read feature mask */
    offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, featureMask);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((ADI_LIBRARY_FREAD(&device->initExtract.featureMask[0], sizeof(deviceProfile->radioProfile.featureMask), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read JESD Profile */
    offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, jesdProfile);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((ADI_LIBRARY_FREAD(&config->jesdProfile, sizeof(deviceProfile->radioProfile.jesdProfile), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }
    
    VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.rxdesQhfrate, config->jesdProfile.rxdesQhfrate);

    for (idx = 0U; idx < ADI_ADRV903X_MAX_FRAMERS; idx++)
    {
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerSetting[idx].iqRate_kHz, config->jesdProfile.framer[idx].iqRate_kHz) ;
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerSetting[idx].laneRate_kHz, config->jesdProfile.framer[idx].laneRate_kHz) ;
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerSetting[idx].jesdM, config->jesdProfile.framer[idx].M) ;
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerSetting[idx].jesdNp, config->jesdProfile.framer[idx].Np) ;
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerSetting[idx].serialLaneEnabled, config->jesdProfile.framer[idx].serializerLanesEnabled) ;
    }

    for (idx = 0U; idx < ADI_ADRV903X_MAX_DEFRAMERS; idx++)
    {
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.deframerSetting[idx].iqRate_kHz, config->jesdProfile.deframer[idx].iqRate_kHz) ;
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.deframerSetting[idx].laneRate_kHz, config->jesdProfile.deframer[idx].laneRate_kHz) ;
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.deframerSetting[idx].deserialLaneEnabled, config->jesdProfile.deframer[idx].deserializerLanesEnabled) ;
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.deframerSetting[idx].jesdM, config->jesdProfile.deframer[idx].M);
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.deframerSetting[idx].jesdNp, config->jesdProfile.deframer[idx].Np);
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.deframerSetting[idx].interleavingEnabled, config->jesdProfile.deframer[idx].interleavingEnabled);
    }

    for (idx = 0U; idx < ADI_ADRV903X_MAX_FRAMERS_LS; idx++)
    {
        if (config->jesdProfile.linkSharingCfg[idx].linkSharingEnabled == ADI_ENABLE)
        {
            VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerLsSetting[idx].jesdM, config->jesdProfile.linkSharingCfg[idx].M) ;
            VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerLsSetting[idx].jesdNp, config->jesdProfile.linkSharingCfg[idx].Np) ;
        }
        else
        {
            device->initExtract.jesdSetting.framerLsSetting[idx].jesdM = 0;
            device->initExtract.jesdSetting.framerLsSetting[idx].jesdNp = 0;
        }
    }

    for (idx = 0U; idx < ADRV903X_JESD_MAX_LKSH_SAMPLE_XBAR_IDX; idx++)
    {
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerLsSetting[0].linkLsSampleXBar[idx], config->jesdProfile.linkSharingCfg[0].sampleXBar[idx]) ;
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerLsSetting[1].linkLsSampleXBar[idx], config->jesdProfile.linkSharingCfg[1].sampleXBar[idx]) ;
    }

    /* Read CPU ID that maps a deserializer lane to CPU0/1 for SERDES cals */
    for (idx = 0U; idx < ADI_ADRV903X_MAX_SERDES_LANES; idx++)
    {
        VARIABLE_ASSIGNMENT(device->initExtract.jesd204DesLaneCpuConfig[idx], config->jesdProfile.deserializerLane[idx].cpuId) ;
    }

    for (idx = 0U; idx < ADRV903X_NUM_TXRX_CHAN; idx++)
    {

        readSize = sizeof(config->rxConfig);
        switch (device->initExtract.chanAssign[idx])
        {
            case 0U:
                /* Read rxConfig[0]*/
                offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, rxConfig[0U]);
                break;

            case 1U:
                /* Read rxConfig[1]*/
                offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, rxConfig[1U]);
                break;
            case 2U:
                /* Read rxConfig[2]*/
                offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, rxConfig[2U]);
                break;
            case 3U:
                /* Read rxConfig[3]*/
                offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, rxConfig[3U]);
                break;
            case 255U:
                /* Channel disabled */
                offset = 0;
                break;
            default:
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
                goto cleanup;
                break;
        }


        if (offset == 0U)
        {    /* channel disabled */
            device->initExtract.rx.rxChannelCfg[idx].rfBandwidth_kHz = 0U;
            device->initExtract.rx.rxChannelCfg[idx].rxDdc0OutputRate_kHz = 0U;
            device->initExtract.rx.rxChannelCfg[idx].rxDdc1OutputRate_kHz = 0U;
            device->initExtract.rx.rxChannelCfg[idx].digChanMask = 0U;
            device->initExtract.rx.rxChannelCfg[idx].rxAdcSampleRate_kHz = 0U;
        }
        else
        {
            uint32_t totalDecimationDDC1 = 0U;
            uint32_t totalDecimationDDC0 = 0U; 
            uint32_t rxOutputRate_kHz    = 0U;
            uint8_t rxBandEnbDDC0        = 0U;
            uint8_t rxBandEnbDDC1        = 0U;

            if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
                goto cleanup;
            }

            if ((ADI_LIBRARY_FREAD(&config->rxConfig, readSize, 1, cpuProfileFilePtr)) <= 0)
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
                goto cleanup;
            }

            VARIABLE_ASSIGNMENT(totalDecimationDDC1, config->rxConfig.rxddc[1].totalDecimation);
            VARIABLE_ASSIGNMENT(totalDecimationDDC0, config->rxConfig.rxddc[0].totalDecimation);
            VARIABLE_ASSIGNMENT(rxOutputRate_kHz, config->rxConfig.rxOutputRate_kHz);
            VARIABLE_ASSIGNMENT(rxBandEnbDDC0, config->rxConfig.rxddc[0].rxBandEnb);
            VARIABLE_ASSIGNMENT(rxBandEnbDDC1, config->rxConfig.rxddc[1].rxBandEnb);

            if (config->rxConfig.rxddc[1].rxBandEnb == 0U)
            {
                VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].rxDdc0OutputRate_kHz, config->rxConfig.rxOutputRate_kHz) ;
                device->initExtract.rx.rxChannelCfg[idx].rxDdc1OutputRate_kHz = 0U;

            }
            else if (totalDecimationDDC1 > totalDecimationDDC0)
            {
                //Band 0 has lower totalDecimation
                device->initExtract.rx.rxChannelCfg[idx].rxDdc0OutputRate_kHz = rxOutputRate_kHz;
                device->initExtract.rx.rxChannelCfg[idx].rxDdc1OutputRate_kHz = (rxOutputRate_kHz * totalDecimationDDC0) / totalDecimationDDC1 ;
            }
            else
            {
                //Band 1 has lower totalDecimation
                device->initExtract.rx.rxChannelCfg[idx].rxDdc0OutputRate_kHz = (rxOutputRate_kHz * totalDecimationDDC1) / totalDecimationDDC0;
                device->initExtract.rx.rxChannelCfg[idx].rxDdc1OutputRate_kHz = rxOutputRate_kHz;
            }
            
            for (bandIdx = 0U; bandIdx < ADI_ADRV903X_DDC_NUM_BAND; bandIdx++)
            {
                if (config->rxConfig.rxddc[bandIdx].rxBandEnb == 1U)
                {
                    uint32_t ncoFreqin_kHz = 0U;
                    uint32_t ibwCenterFreq_kHz = 0U;
                    uint32_t rfCenterFreq_kHz = 0U;
                    VARIABLE_ASSIGNMENT(ncoFreqin_kHz, config->rxConfig.rxddc[bandIdx].ncoFreqin_kHz);
                    VARIABLE_ASSIGNMENT(ibwCenterFreq_kHz, config->rxConfig.ibwCenterFreq_kHz);
                    VARIABLE_ASSIGNMENT(rfCenterFreq_kHz, config->rxConfig.rxddc[bandIdx].rfCenterFreq_kHz);
                    
                    device->initExtract.rx.rxChannelCfg[idx].bandSettings[bandIdx].enabled = ADI_TRUE;
                    VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].bandSettings[bandIdx].instBw_kHz, config->rxConfig.rxddc[bandIdx].instBw_kHz);
                    VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].bandSettings[bandIdx].rfCenterFreq_kHz, config->rxConfig.rxddc[bandIdx].rfCenterFreq_kHz);
                    device->initExtract.rx.rxChannelCfg[idx].bandSettings[bandIdx].sampleRate_kHz = device->initExtract.clocks.hsDigClk_kHz / (1U << config->rxConfig.rxddc[bandIdx].hb1OutputClkDiv);
                    device->initExtract.rx.rxChannelCfg[idx].bandSettings[bandIdx].bandOffset_kHz = ncoFreqin_kHz - (ibwCenterFreq_kHz - rfCenterFreq_kHz);
                }
                else
                {
                    device->initExtract.rx.rxChannelCfg[idx].bandSettings[bandIdx].enabled = ADI_FALSE;
                    device->initExtract.rx.rxChannelCfg[idx].bandSettings[bandIdx].instBw_kHz = 0U;
                    device->initExtract.rx.rxChannelCfg[idx].bandSettings[bandIdx].rfCenterFreq_kHz = 0U;
                    device->initExtract.rx.rxChannelCfg[idx].bandSettings[bandIdx].sampleRate_kHz = 0U;
                    device->initExtract.rx.rxChannelCfg[idx].bandSettings[bandIdx].bandOffset_kHz = 0U;
                }
            }

            VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].rfBandwidth_kHz, config->rxConfig.ibw_kHz) ;
            VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].rxAdcSampleRate_kHz, config->rxConfig.adcClockRate_kHz) ;
            digMask = rxBandEnbDDC1;
            digMask <<= 1;
            digMask |= rxBandEnbDDC0;
            device->initExtract.rx.rxChannelCfg[idx].digChanMask = digMask;
        }
    }

    for (idx = 0U; idx < ADI_ADRV903X_MAX_ORX; idx++)
    {
        readSize = sizeof(config->orxConfig);
        switch (idx)
        {
        case 0U:
            offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, orxConfig[0U]);
            break;
        case 1U:
            offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, orxConfig[1U]);
            break;
#if ADI_ADRV903X_MAX_ORX > 2
        default:
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
            goto cleanup;
#endif
        }

        targetIdx = idx;

        if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
            goto cleanup;
        }

        if ((ADI_LIBRARY_FREAD(&config->orxConfig, readSize, 1, cpuProfileFilePtr)) <= 0)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
            goto cleanup;
        }

        VARIABLE_ASSIGNMENT(device->initExtract.orx.orxChannelCfg[targetIdx].rfBandwidth_kHz, config->orxConfig.sbw_kHz) ;
        VARIABLE_ASSIGNMENT(device->initExtract.orx.orxChannelCfg[targetIdx].orxOutputRate_kHz, config->orxConfig.orxOutputRate_kHz) ;
        VARIABLE_ASSIGNMENT(device->initExtract.orx.orxChannelCfg[targetIdx].orxAdcSampleRate_kHz, config->orxConfig.adcClockRate_kHz) ;
    }

    for (idx = 0U; idx < ADRV903X_NUM_TXRX_CHAN; idx++)
    {

        readSize = sizeof(config->txConfig);
        switch (device->initExtract.chanAssign[idx])
        {
            case 0U:
                /* Read txConfig[0] */
                offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, txConfig[0U]);
                break;

            case 1U:
                /* Read txConfig[1] */
                offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, txConfig[1U]);
                break;
            case 2U:
                /* Read txConfig[2] */
                offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, txConfig[2U]);
                break;
            case 3U:
                /* Read txConfig[3] */
                offset = ADI_LIBRARY_OFFSETOF(adrv903x_RadioProfile_t, txConfig[3U]);
                break;
            case 255U:
                /* Channel disabled */
                offset = 0U;
                break;
            default:
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
                goto cleanup;
                break;
        }

        if (offset == 0U)
        {   /* channel disabled */
            device->initExtract.tx.txChannelCfg[idx].rfBandwidth_kHz       = 0U;
            device->initExtract.tx.txChannelCfg[idx].totalDecimation       = 0U;
            device->initExtract.tx.txChannelCfg[idx].digChanMask           = 0U;
            device->initExtract.tx.txChannelCfg[idx].txLbAdcSampleRate_kHz = 0U;
            device->initExtract.tx.txChannelCfg[idx].txLbAdcClkDiv         = 0U;
        }
        else
        {
            if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
                goto cleanup;
            }

            if ((ADI_LIBRARY_FREAD(&config->txConfig, readSize, 1, cpuProfileFilePtr)) <= 0)
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
                goto cleanup;
            }

            VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].rfBandwidth_kHz,       config->txConfig.ibw_kHz) ;
            VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].totalDecimation,       config->txConfig.txduc[0].totalDecimation);
            VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].digChanMask,           config->txConfig.txduc[0].rxBandEnb);
            VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].txLbAdcSampleRate_kHz, config->txConfig.lpbkAdcClkRate_kHz);
            /* clkDiv from profile.bin holds a bitfield value. txLbAdcClkDiv is the logical divider value. Adding 1 converts
             * from bitfield value to logical value. */
            VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].txLbAdcClkDiv,         config->txConfig.lpbkClkGenConfig.clkDiv + 1);
            
            for (bandIdx = 0U; bandIdx < ADI_ADRV903X_DUC_NUM_BAND; bandIdx++)
            {
                if (config->txConfig.txduc[bandIdx].rxBandEnb == 1U)
                {
                    uint8_t tinClkDiv = 0U;
                    uint32_t ncoFreqin_kHz = 0U;
                    uint32_t ibwCenterFreq_kHz = 0U;
                    uint32_t rfCenterFreq_kHz = 0U;
                    VARIABLE_ASSIGNMENT(tinClkDiv, config->txConfig.txduc[bandIdx].tinClkDiv);
                    VARIABLE_ASSIGNMENT(ncoFreqin_kHz, config->txConfig.txduc[bandIdx].ncoFreqin_kHz);
                    VARIABLE_ASSIGNMENT(ibwCenterFreq_kHz, config->txConfig.ibwCenterFreq_kHz);
                    VARIABLE_ASSIGNMENT(rfCenterFreq_kHz, config->txConfig.txduc[bandIdx].rfCenterFreq_kHz);
                    
                    device->initExtract.tx.txChannelCfg[idx].bandSettings[bandIdx].enabled = ADI_TRUE;
                    VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].bandSettings[bandIdx].instBw_kHz, config->txConfig.txduc[bandIdx].instBw_kHz);
                    VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].bandSettings[bandIdx].rfCenterFreq_kHz, config->txConfig.txduc[bandIdx].rfCenterFreq_kHz);
                    device->initExtract.tx.txChannelCfg[idx].bandSettings[bandIdx].sampleRate_kHz = device->initExtract.clocks.hsDigClk_kHz / (1U << tinClkDiv);
                    device->initExtract.tx.txChannelCfg[idx].bandSettings[bandIdx].bandOffset_kHz = ncoFreqin_kHz - (ibwCenterFreq_kHz - rfCenterFreq_kHz);
                }
                else
                {
                    device->initExtract.tx.txChannelCfg[idx].bandSettings[bandIdx].enabled = ADI_FALSE;
                    device->initExtract.tx.txChannelCfg[idx].bandSettings[bandIdx].instBw_kHz = 0U;
                    device->initExtract.tx.txChannelCfg[idx].bandSettings[bandIdx].rfCenterFreq_kHz = 0U;
                    device->initExtract.tx.txChannelCfg[idx].bandSettings[bandIdx].sampleRate_kHz = 0U;
                    device->initExtract.tx.txChannelCfg[idx].bandSettings[bandIdx].bandOffset_kHz = 0U;
                }
            }
            
            VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].pfirRate_kHz,          config->txConfig.txPfirClk_kHz) ;
            

            if (device->initExtract.tx.txChannelCfg[idx].totalDecimation > 8U)
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
                goto cleanup;
            }
        }
    }

    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

cleanup:
    if (cpuProfileFilePtr != NULL)
    {
        /* Close CPU Profile binary file */
        if (0 != ADI_LIBRARY_FCLOSE(cpuProfileFilePtr))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close CPU Profile Binary File");
        }
    }
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_DeviceCopy(adi_adrv903x_Device_t* const    deviceSrc,
                                                         adi_adrv903x_Device_t* const    deviceDest)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(deviceSrc);
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(deviceDest);

    /* Ignore Common Device & SPI Settings */
    (void) ADI_LIBRARY_MEMCPY(&deviceDest->devStateInfo, &deviceSrc->devStateInfo, sizeof(adi_adrv903x_Info_t));
    (void) ADI_LIBRARY_MEMCPY(&deviceDest->initExtract, &deviceSrc->initExtract, sizeof(adi_adrv903x_InitExtract_t));

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_PreMcsInit(adi_adrv903x_Device_t* const            device,
                                                         const adi_adrv903x_Init_t* const        init,
                                                         const adi_adrv903x_TrxFileInfo_t* const trxBinaryInfoPtr)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, init, cleanup);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, trxBinaryInfoPtr, cleanup);

    /* Prevent any Read Operation from device during Broadcast */
    ADI_ADRV903X_WRONLY_SET(device->common, ADI_TRUE);

    /* Extract Info from CPU Profile Binary */
    recoveryAction = adi_adrv903x_DeviceInfoExtract(device, &trxBinaryInfoPtr->cpuProfile);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during CPU Profile Binary Image Extract");
        goto cleanup;
    }

    /* Initialize TRX Device */
    recoveryAction = adi_adrv903x_Initialize(device, init);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TRX Device Initialization Failed");
        goto cleanup;
    }

    /* Load Stream Binary Image */
    recoveryAction = adi_adrv903x_StreamImageLoad(device, &trxBinaryInfoPtr->stream);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Stream Binary Image Loading");
        goto cleanup;
    }

    /* Load CPU Binary File(s) */
    recoveryAction = adi_adrv903x_CpuImageLoad(device, &trxBinaryInfoPtr->cpu);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during CPU Binary Image Loading");
        goto cleanup;
    }

    /* Load CPU Profile Binary */
    recoveryAction = adi_adrv903x_CpuProfileImageLoad(device, &trxBinaryInfoPtr->cpuProfile);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during CPU Profile Binary Image Loading");
        goto cleanup;
    }

    /* Load Rx Gain Table(s) */
    recoveryAction = adi_adrv903x_RxGainTableLoad(device, &trxBinaryInfoPtr->rxGainTable[0U], ADI_ADRV903X_LOAD_ALL_RXGAIN_TABLES);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during RX Gain Table Loading");
        goto cleanup;
    }

            /* CPU Bootup */
    recoveryAction = adi_adrv903x_CpuStart(device);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during TRX CPU Start");
        goto cleanup;
    }

cleanup :
    ADI_ADRV903X_WRONLY_SET(device->common, ADI_FALSE);
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_PreMcsInit_NonBroadcast(adi_adrv903x_Device_t* const        device,
                                                                      const adi_adrv903x_Init_t* const    init)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_adrv903x_RxDataFormatRt_t rxDataFormatRt;
    adi_adrv903x_RxGain_t rxGain;
    uint32_t i            = 0u;
    uint32_t rxChannels[] = {
        (uint32_t)ADI_ADRV903X_RX0,
        (uint32_t)ADI_ADRV903X_RX1,
        (uint32_t)ADI_ADRV903X_RX2,
        (uint32_t)ADI_ADRV903X_RX3,
        (uint32_t)ADI_ADRV903X_RX4,
        (uint32_t)ADI_ADRV903X_RX5,
        (uint32_t)ADI_ADRV903X_RX6,
        (uint32_t)ADI_ADRV903X_RX7,
        (uint32_t)ADI_ADRV903X_ORX0,
        (uint32_t)ADI_ADRV903X_ORX1
    };

    adi_adrv903x_TxAtten_t                  txAtten;
    adi_adrv903x_TxAttenCfg_t               txAttenCfg;
    adi_adrv903x_PowerMonitorCfgRt_t        txPowerMonitorCfg;
    adi_adrv903x_SlewRateDetectorCfgRt_t    txSrlCfg;
    adi_adrv903x_ProtectionRampCfgRt_t      txProtectionRampCfg;
    uint8_t txInputInterpolation = 0U;
    uint32_t chanMask = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, init, cleanup);

    ADI_LIBRARY_MEMSET(&txAtten, 0, sizeof(txAtten));
    ADI_LIBRARY_MEMSET(&txAttenCfg, 0, sizeof(txAttenCfg));
    ADI_LIBRARY_MEMSET(&txPowerMonitorCfg, 0, sizeof(txPowerMonitorCfg));
    ADI_LIBRARY_MEMSET(&txSrlCfg, 0, sizeof(txSrlCfg));
    ADI_LIBRARY_MEMSET(&txProtectionRampCfg, 0, sizeof(txProtectionRampCfg));
    ADI_LIBRARY_MEMSET(&rxDataFormatRt, 0, sizeof(rxDataFormatRt));
    ADI_LIBRARY_MEMSET(&rxGain, 0, sizeof(rxGain));

    recoveryAction = adi_adrv903x_CpuStartStatusCheck(device, ADI_ADRV903X_GETCPUBOOTUP_TIMEOUT_US);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Boot Issue");
        goto cleanup;
    }

#if ADI_ADRV903X_SPI_VERIFY
    recoveryAction = adi_adrv903x_SpiVerify(device);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }
#endif

    recoveryAction = adi_adrv903x_DeviceRevGet(device, &(device->devStateInfo.deviceSiRev));
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read device silicon revision");
        goto cleanup;
    }

    if (device->devStateInfo.profilesValid & ADI_ADRV903X_TX_PROFILE_VALID)
    {
        /* For each tx channel */
        for (i = 0U; i < ADI_ADRV903X_MAX_TXCHANNELS; ++i)
        {
            chanMask = 1U << i;
            if (ADRV903X_BF_EQUAL(device->initExtract.tx.txInitChannelMask, chanMask) == 0)
            {
                /* Channel disabled - skip */
                continue;
            }

            /* Tx channel is enabled */
            txAttenCfg = init->tx.txChannelCfg[i].txAttenCfg;

            recoveryAction = adi_adrv903x_TxAttenCfgSet(device, chanMask, &txAttenCfg);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx Atten Configuration Failed");
                goto cleanup;
            }

            txAtten.txChannelMask = chanMask;
            txAtten.txAttenuation_mdB = init->tx.txChannelCfg[i].txAttenInit_mdB;

            recoveryAction = adi_adrv903x_TxAttenSet(device, &txAtten, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_RESET_FEATURE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx Initial Attenuation Set Failed");
                    goto cleanup;
                }
            }

            txInputInterpolation = (uint8_t)device->initExtract.tx.txChannelCfg[i].totalDecimation;

            recoveryAction = adrv903x_TxPowerMonitorInitialize(device, chanMask, txInputInterpolation);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx Power Monitor Initialize Failed");
                goto cleanup;
            }

            txPowerMonitorCfg.txChannelMask   = chanMask;
            txPowerMonitorCfg.txPowerMonitorCfg = init->tx.txChannelCfg[i].txpowerMonitorCfg;

            recoveryAction = adi_adrv903x_TxPowerMonitorCfgSet(device, &txPowerMonitorCfg, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx Power Protection Configuration Failed");
                goto cleanup;
            }

            txSrlCfg.txChannelMask = chanMask;
            txSrlCfg.srlCfg = init->tx.txChannelCfg[i].srlCfg;

            recoveryAction = adi_adrv903x_TxSlewRateDetectorCfgSet(device, &txSrlCfg, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx Slew Rate Detector Configuration Failed");
                goto cleanup;
            }

            txProtectionRampCfg.txChannelMask = chanMask;

            txProtectionRampCfg.protectionRampCfg = init->tx.txChannelCfg[i].protectionRampCfg;
            recoveryAction = adi_adrv903x_TxProtectionRampCfgSet(device, &txProtectionRampCfg, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx Protection Ramp Configuration Failed");
                goto cleanup;
            }
        }
    }

    /* Initializing RX configuration */
    if (device->devStateInfo.profilesValid & ADI_ADRV903X_RX_PROFILE_VALID)
    {
        for (i = 0U; i < (sizeof(rxChannels) / sizeof(rxChannels[0U])); ++i)
        {
            if ((device->initExtract.rx.rxInitChannelMask & rxChannels[i]) > 0U)
            {
                /* Correct the Rx Channel Mask from the profile */
                rxDataFormatRt.rxChannelMask = rxChannels[i];
                rxDataFormatRt.rxDataFormat = init->rx.rxChannelCfg[i].rxDataFormat;

                /* Setup the Rx data formatter */
                recoveryAction = adrv903x_RxDataFormatSet(device, &rxDataFormatRt, 1U);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RX Data Format set Failed");
                    goto cleanup;
                }

                /* Setup Rx AGC overload protection registers */
                recoveryAction = adrv903x_RxOverloadProtectionSet(device, (adi_adrv903x_RxChannels_e)rxChannels[i]);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RX Overload Protection set Failed");
                    goto cleanup;
                }

                if (i < ADI_ADRV903X_MAX_RX_ONLY)
                {
                    /* Correct the Rx Channel Mask from the profile */
                    rxGain.rxChannelMask = rxChannels[i];
                    rxGain.gainIndex = init->rx.rxChannelCfg[i].rxGainIndexInit;

                    /* Setup the Rx Gain */
                    recoveryAction = adi_adrv903x_RxGainSet(device, &rxGain, 1U);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RX Gain set Failed");
                        goto cleanup;
                    }
                }
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_PostMcsInit(adi_adrv903x_Device_t* const            device,
                                                          const adi_adrv903x_PostMcsInit_t* const utilityInit)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    const uint32_t INIT_CALS_TIMEOUT_MS = 60000U;   /* 60 Seconds Timeout */
    ADI_PLATFORM_LARGE_VAR_ALLOC(adi_adrv903x_InitCalErrData_t, initCalErrDataPtr);
    uint8_t idx = 0U;
    adi_adrv903x_RadioCtrlTxRxEnCfg_t allDisabledTxRxEnCfg;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, initCalErrDataPtr, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, utilityInit, cleanup);

    ADI_LIBRARY_MEMSET(&allDisabledTxRxEnCfg, 0, sizeof(allDisabledTxRxEnCfg));

    /*Initialize radio control. This is required to run before running init cals*/
    /* Sets up Radio Ctrl mode for Rx/ORx/Tx signal chains (SPI vs Pin mode) */
    if ((utilityInit->radioCtrlCfg.txRadioCtrlModeCfg.txChannelMask     != 0U) ||
        (utilityInit->radioCtrlCfg.rxRadioCtrlModeCfg.rxChannelMask     != 0U) ||
        (utilityInit->radioCtrlCfg.orxRadioCtrlModeCfg.orxChannelMask   != 0U))
    {
        recoveryAction = adi_adrv903x_RadioCtrlCfgSet(device, (adi_adrv903x_RadioCtrlModeCfg_t*) &utilityInit->radioCtrlCfg);
        if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Radio Control Config Set Failed");
            goto cleanup;
        }
    }

    /* Sets up Radio Ctrl TxRx Enable Config for Pin mode (Config will be ignore in SPI mode) */
    if ((utilityInit->radioCtrlTxRxEnCfgSel != 0U) || (utilityInit->radioCtrlTxRxEnPinSel != 0U))
    {
        recoveryAction = adi_adrv903x_RadioCtrlTxRxEnCfgSet(device,
                                                            &allDisabledTxRxEnCfg,
                                                            ADI_ADRV903X_TXRXEN_PINALL,
                                                            ADI_ADRV903X_TXRXEN_TX_ENABLE_MAP | ADI_ADRV903X_TXRXEN_RX_ENABLE_MAP);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Radio Control TxRxEnCfg set to default failed");
            goto cleanup;
        }
        
        recoveryAction = adi_adrv903x_RadioCtrlTxRxEnCfgSet(device,
                                                            &utilityInit->radioCtrlGpioCfg,
                                                            utilityInit->radioCtrlTxRxEnPinSel,
                                                            utilityInit->radioCtrlTxRxEnCfgSel);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Radio Control TxRxEnCfg Set Failed");
            goto cleanup;
        }
    }

    /* Sets up the GPIO pin mapping used by stream processor */
    recoveryAction = adi_adrv903x_StreamGpioConfigSet(device, &device->devStateInfo.streamGpioMapping);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Stream Gpio Config Set Failed");
        goto cleanup;
    }
  
    /* Sets up the Tx to Orx pin mapping */
    recoveryAction = adi_adrv903x_TxToOrxMappingInit(device);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx To Orx Mapping Init Failed");
        goto cleanup;
    }

    /* Initialize GP Interrupt settings for use during device initialization*/
    /* Set GP Int Pin Masks to selected states */
    recoveryAction = adi_adrv903x_GpIntPinMaskCfgSet(device, ADI_ADRV903X_GPINTALL, &utilityInit->gpIntPostInit);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing GP Interrupt Mask Array.");
        goto cleanup;
    }

    /* Clear all status bits */
    recoveryAction = adi_adrv903x_GpIntStatusClear(device, NULL);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error clearing GP Interrupt Status bits.");
        goto cleanup;
    }

    /*Initialize cals*/
    if ((utilityInit->initCals.rxChannelMask  != 0U) ||
        (utilityInit->initCals.txChannelMask  != 0U) ||
        (utilityInit->initCals.orxChannelMask != 0U) ||
        (utilityInit->initCals.calMask != 0U))
    {
        recoveryAction = adi_adrv903x_InitCalsRun(device, &utilityInit->initCals);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "InitCalsRun Failed");
            goto cleanup;
        }

        recoveryAction = adi_adrv903x_InitCalsWait_v2(device, INIT_CALS_TIMEOUT_MS, initCalErrDataPtr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Initial Calibration Wait Issue");
            goto cleanup;
        }

        for (idx = 0; idx < ADI_ADRV903X_NUM_INIT_CAL_CHANNELS; ++idx)
        {
            if (initCalErrDataPtr->channel[idx].errCode != 0)
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_FEATURE;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "InitCals Error - Get More Information via Detailed Status Get");
                goto cleanup;
            }
        }
    }

    recoveryAction = adi_adrv903x_JrxRepairInitialization(device, utilityInit->initCals.warmBoot);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Jrx repair at initialization failed");
        goto cleanup;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_StandbyEnter(adi_adrv903x_Device_t* const         device,
                                                           adi_adrv903x_StandbyRecover_t* const standbyRecover)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    uint32_t txInitChMask  = ((device->devStateInfo.initializedChannels >> ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET) & ADI_ADRV903X_TXALL);
    uint32_t rxInitChMask  =  (device->devStateInfo.initializedChannels & ADI_ADRV903X_RX_MASK_ALL);
    uint32_t orxInitChMask =  (device->devStateInfo.initializedChannels & ADI_ADRV903X_ORX_MASK_ALL);
    uint32_t trackingCal = 0U;
    uint32_t trackingCalMask = 0U;
    uint32_t channelMask = 0U;
    uint32_t timeElapsedUs = 0U;
    uint32_t index = 0U;
    uint32_t cmdExecRespLength = 0U;
    uint32_t regAddr = 0U;
    uint16_t deserializerPdCfg = 0xFFFFU;
    uint8_t  regData[4] = { 0U, 0U, 0U, 0U };
    uint8_t  trackingCalActive = 0U;
    uint8_t  lanesEnabled = 0U;
    adrv903x_CpuCmd_JtxLanePower_t jtxPwrCmd;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;

    adi_adrv903x_ChannelTrackingCals_t channelMaskv2;
    adi_adrv903x_TrackingCalState_t    trackingCalState;

    adi_adrv903x_RadioCtrlModeCfg_t radioCtrlSpiModeCfg = {
        { ADI_ADRV903X_TX_EN_SPI_MODE,  txInitChMask },
        { ADI_ADRV903X_RX_EN_SPI_MODE,  rxInitChMask },
        { ADI_ADRV903X_ORX_EN_SPI_MODE, orxInitChMask }
    };

    adrv903x_CpuCmd_EnFastAttack_t     enFastAttackCmd;
    adrv903x_CpuCmd_EnFastAttackResp_t enFastAttackCmdRsp;

    const adi_adrv903x_GpIntPinMaskCfg_t pinMaskCfgAll = {
        { 0xFFFFFFFFFFFFU, 0xFFFFFFFFFFFFU },
        { 0xFFFFFFFFFFFFU, 0xFFFFFFFFFFFFU }
    };

    const uint32_t TX_DAC_PWD_DOWN_I_ADDRS[ADI_ADRV903X_MAX_TXCHANNELS] = {
        ADRV903X_ADDR_TX0_DAC_PWD_DOWN_I,
        ADRV903X_ADDR_TX1_DAC_PWD_DOWN_I,
        ADRV903X_ADDR_TX2_DAC_PWD_DOWN_I,
        ADRV903X_ADDR_TX3_DAC_PWD_DOWN_I,
        ADRV903X_ADDR_TX4_DAC_PWD_DOWN_I,
        ADRV903X_ADDR_TX5_DAC_PWD_DOWN_I,
        ADRV903X_ADDR_TX6_DAC_PWD_DOWN_I,
        ADRV903X_ADDR_TX7_DAC_PWD_DOWN_I
    };
    const uint32_t RX_ADC_REGMAP0_CTRL_FD_PD_ADDRS[ADI_ADRV903X_MAX_RX_ONLY] = {
        ADRV903X_ADDR_RX0_ADC_REGMAP0_CTRL_FD_PD,
        ADRV903X_ADDR_RX1_ADC_REGMAP0_CTRL_FD_PD,
        ADRV903X_ADDR_RX2_ADC_REGMAP0_CTRL_FD_PD,
        ADRV903X_ADDR_RX3_ADC_REGMAP0_CTRL_FD_PD,
        ADRV903X_ADDR_RX4_ADC_REGMAP0_CTRL_FD_PD,
        ADRV903X_ADDR_RX5_ADC_REGMAP0_CTRL_FD_PD,
        ADRV903X_ADDR_RX6_ADC_REGMAP0_CTRL_FD_PD,
        ADRV903X_ADDR_RX7_ADC_REGMAP0_CTRL_FD_PD
    };
    const uint32_t RX_ADC_REGMAP0_CTRL_FL_PD_ADDRS[ADI_ADRV903X_MAX_RX_ONLY] = {
        ADRV903X_ADDR_RX0_ADC_REGMAP0_CTRL_FL_PD,
        ADRV903X_ADDR_RX1_ADC_REGMAP0_CTRL_FL_PD,
        ADRV903X_ADDR_RX2_ADC_REGMAP0_CTRL_FL_PD,
        ADRV903X_ADDR_RX3_ADC_REGMAP0_CTRL_FL_PD,
        ADRV903X_ADDR_RX4_ADC_REGMAP0_CTRL_FL_PD,
        ADRV903X_ADDR_RX5_ADC_REGMAP0_CTRL_FL_PD,
        ADRV903X_ADDR_RX6_ADC_REGMAP0_CTRL_FL_PD,
        ADRV903X_ADDR_RX7_ADC_REGMAP0_CTRL_FL_PD
    };
    const uint32_t TX_LB_ADC_WEST_TRM_PDN_CTRL_ADDRS[ADI_ADRV903X_MAX_TXCHANNELS] = {
        ADRV903X_ADDR_TX0_LB_ADC_WEST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX1_LB_ADC_WEST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX2_LB_ADC_WEST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX3_LB_ADC_WEST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX4_LB_ADC_WEST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX5_LB_ADC_WEST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX6_LB_ADC_WEST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX7_LB_ADC_WEST_TRM_PDN_CTRL
    };
    const uint32_t TX_LB_ADC_EAST_TRM_PDN_CTRL_ADDRS[ADI_ADRV903X_MAX_TXCHANNELS] = {
        ADRV903X_ADDR_TX0_LB_ADC_EAST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX1_LB_ADC_EAST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX2_LB_ADC_EAST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX3_LB_ADC_EAST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX4_LB_ADC_EAST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX5_LB_ADC_EAST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX6_LB_ADC_EAST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX7_LB_ADC_EAST_TRM_PDN_CTRL
    };

    const uint8_t PLL_MISC_PD_DISABLE        = 0xBCU;
    const uint8_t PLL_MISC_PD_KILLS_DISABLE  = 0x8FU;
    const uint8_t PLL_LO_GEN_SYNC_PD_DISABLE = 0xF3U;
    const uint8_t TX_DAC_PD_DISABLE          = 0x1CU;
    const uint8_t RX_ADC_CTRL_FD_PD_DISABLE  = 0x1FU;
    const uint8_t RX_ADC_CTRL_FL_PD_DISABLE  = 0xBFU;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, standbyRecover, cleanup);
    ADI_LIBRARY_MEMSET(&jtxPwrCmd, 0U, sizeof(jtxPwrCmd));

    if ((device->devStateInfo.devState & ADI_ADRV903X_STATE_STANDBY) == ADI_ADRV903X_STATE_STANDBY)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Device is already in Standby Mode");
        goto cleanup;
    }

    /* 1. Disable GPINT IRQs */
    recoveryAction = adi_adrv903x_GpIntPinMaskCfgSet(device, ADI_ADRV903X_GPINTALL, &pinMaskCfgAll);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing GP Interrupt Mask.");
        goto cleanup;
    }

    /* 2. Set radio control to SPI mode */
    recoveryAction = adi_adrv903x_RadioCtrlCfgSet(device, &radioCtrlSpiModeCfg);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing GP Interrupt Mask Array.");
        goto cleanup;
    }

    /* 3. Turn off All Channels */
    recoveryAction = adi_adrv903x_RxTxEnableGet(device, &(standbyRecover->orxEnabledMask), &(standbyRecover->rxEnabledMask), &(standbyRecover->txEnabledMask));
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting enabled channels.");
        goto cleanup;
    }

    recoveryAction = adi_adrv903x_RxTxEnableSet(device, orxInitChMask, 0U, rxInitChMask, 0U, txInitChMask, 0U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while disabling channels.");
        goto cleanup;
    }

    /* 4. Disable Tracking cals*/
    recoveryAction = adi_adrv903x_TrackingCalsEnableGet(device, &(standbyRecover->tcEnableMasks));
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while storing enabled tracking cals.");
        goto cleanup;
    }

    for (trackingCal = (uint32_t)ADI_ADRV903X_TC_RX_QEC_MASK; trackingCal <= (uint32_t)ADI_ADRV903X_TC_ORX_ADC_MASK; trackingCal <<= 1U)
    {
        channelMask = 0U;

        for (index = 0U; index < ADI_ADRV903X_NUM_TRACKING_CAL_CHANNELS; index++)
        {
            if (standbyRecover->tcEnableMasks.enableMask[index] & trackingCal)
            {
                channelMask |= (1U << index);
            }
        }

        if (channelMask)
        {
            trackingCalMask |= trackingCal;
            channelMaskv2.rxChannel  = channelMask & 0xFFU;
            channelMaskv2.txChannel  = channelMask & 0xFFU;
            channelMaskv2.orxChannel = channelMask & 0x03U;
            channelMaskv2.laneSerdes = channelMask & 0xFFU;
            recoveryAction = adi_adrv903x_TrackingCalsEnableSet_v2(device, (adi_adrv903x_TrackingCalibrationMask_e)trackingCal, &channelMaskv2, ADI_ADRV903X_TRACKING_CAL_DISABLE);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, trackingCal, "Error while disabling Tracking cal");
                goto cleanup;
            }
        }
    }

    /* Poll tracking cals state to be inactive */
    for (timeElapsedUs = 0U; timeElapsedUs < ADI_ADRV903X_TRACKCALDISABLE_TIMEOUT_US; timeElapsedUs += ADI_ADRV903X_TRACKCALDISABLE_INTERVAL_US)
    {
        trackingCalActive = 0U;
        /* Retrieve Tracking Cal state */
        recoveryAction = adi_adrv903x_TrackingCalAllStateGet(device, &trackingCalState);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting Tracking Cal State");
            goto cleanup;
        }

        /* check if every tracking cal is inactive */
        for (trackingCal = (uint32_t)ADI_ADRV903X_TC_RX_QEC; trackingCal < (uint32_t)ADI_ADRV903X_TC_NUM_CALS; trackingCal++)
        {
            for (index = 0U; index < ADI_ADRV903X_NUM_TRACKING_CAL_CHANNELS; index++)
            {
                if (((standbyRecover->tcEnableMasks.enableMask[index] & (1U << trackingCal)) != 0U) && /* previously enabled */
                    ((trackingCalState.calState[index][trackingCal] & ADI_ADRV903X_TC_STATE_INACTIVE) != ADI_ADRV903X_TC_STATE_INACTIVE))
                {
                    trackingCalActive = 1U;
                    break;
                }
            }
            if (trackingCalActive == 1U)
            {
                break;
            }
        }

        /* Break out here if all tracking cals are Inactive */
        if (trackingCalActive == 0U)
        {
            break;
        }

        /* Some tracking cals are still running. Wait the specified wait interval, then check again for inactivity. */
        recoveryAction = (adi_adrv903x_ErrAction_e) adi_common_hal_Wait_us(&device->common, ADI_ADRV903X_TRACKCALDISABLE_INTERVAL_US);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Issue");
            goto cleanup;
        }
    }

    /* Check for Tracking Cal Inactive timeout */
    if (timeElapsedUs >= ADI_ADRV903X_TRACKCALDISABLE_TIMEOUT_US)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_RESET_FEATURE;
        ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_TIMEOUT, recoveryAction, trackingCal, "Tracking Cal Inactive Timeout");
        goto cleanup;
    }

    /* 5. Enable Fast Attack mode for tracking cals and serdes initcal */
    enFastAttackCmd.calMask = ADRV903X_HTOCL(trackingCalMask &
        (~(uint32_t)(ADI_ADRV903X_TC_RX_ADC_MASK | ADI_ADRV903X_TC_ORX_ADC_MASK))); /* skip RX ADC and ORX ADC from Fast attack */

    for (index = 0U; index < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; index++)
    {
        recoveryAction = adrv903x_CpuCmdSend(device,
                                             (adi_adrv903x_CpuType_e)index,
                                             ADRV903X_LINK_ID_0,
                                             ADRV903X_CPU_CMD_ID_ENABLE_FAST_ATTACK,
                                             (void*)&enFastAttackCmd,
                                             sizeof(adrv903x_CpuCmd_EnFastAttack_t),
                                             (void*)&enFastAttackCmdRsp,
                                             sizeof(adrv903x_CpuCmd_EnFastAttackResp_t),
                                             NULL);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Failed to set Cals in Fast attack mode");
            goto cleanup;
        }
    }

    /* Still need to send control commands for RXQEC, TXQEC and TXLOL Cals */
    for (index = 0U; index < ADI_ADRV903X_MAX_RX_ONLY; index++)
    {
        if ((standbyRecover->tcEnableMasks.enableMask[index] & ADI_ADRV903X_TC_RX_QEC_MASK) != 0U)
        {
            recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                            ADRV903X_CPU_OBJID_TC_RXQEC,
                                                            0x2U,
                                                            (adi_adrv903x_Channels_e)(1U << index),
                                                            regData,
                                                            0U,
                                                            &cmdExecRespLength,
                                                            regData,
                                                            4U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Failed to set RX QEC cal in Fast Attack");
                goto cleanup;
            }
        }
    }

    for (index = 0U; index < ADI_ADRV903X_MAX_TXCHANNELS; index++)
    {
        if (standbyRecover->tcEnableMasks.enableMask[index] & ADI_ADRV903X_TC_TX_LOL_MASK)
        {
            recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                            ADRV903X_CPU_OBJID_TC_TX_LOL,
                                                            0x2U,
                                                            (adi_adrv903x_Channels_e)(1U << index),
                                                            regData,
                                                            0U,
                                                            &cmdExecRespLength,
                                                            regData,
                                                            4U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Failed to set TX LOL cal in Fast Attack");
                goto cleanup;
            }

            recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                            ADRV903X_CPU_OBJID_TC_TX_LOL,
                                                            0x0U,
                                                            (adi_adrv903x_Channels_e)(1U << index),
                                                            regData,
                                                            0U,
                                                            &cmdExecRespLength,
                                                            regData,
                                                            4U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Failed to soft reset TX LOL");
                goto cleanup;
            }
        }

        if (standbyRecover->tcEnableMasks.enableMask[index] & ADI_ADRV903X_TC_TX_QEC_MASK)
        {
            recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                            ADRV903X_CPU_OBJID_TC_TXQEC,
                                                            0x0U,
                                                            (adi_adrv903x_Channels_e)(1U << index),
                                                            regData,
                                                            0U,
                                                            &cmdExecRespLength,
                                                            regData,
                                                            4U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Failed to set TX QEC cal in Fast Attack");
                goto cleanup;
            }
        }
    }

    /* 6. Disable serializer lanes */
    lanesEnabled = 0U;
    for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_FRAMERS; index++)
    {
        lanesEnabled |= device->initExtract.jesdSetting.framerSetting[index].serialLaneEnabled;
    }

    /* Prepare the command payload to power-down all enabled jtx lanes. uint8_t fields don't require HTOC conversion. */
    jtxPwrCmd.jtxLaneMask = lanesEnabled;
    jtxPwrCmd.jtxLanePower = 0x00;
    
    /* Send command. There is no cmd-specific response expected.
     * This command is always sent to CPU0 regardless of lane-CPU assignment. */
    recoveryAction = adrv903x_CpuCmdSend(device,
                                         ADI_ADRV903X_CPU_TYPE_0,
                                         ADRV903X_LINK_ID_0,
                                         ADRV903X_CPU_CMD_ID_JESD_TX_LANE_POWER,
                                         (void*)&jtxPwrCmd,
                                         sizeof(jtxPwrCmd),
                                         NULL,
                                         0U,
                                         &cmdStatus);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                goto cleanup;
            }
    
    if (cmdStatus != ADRV903X_CPU_CMD_STATUS_NO_ERROR)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_FEATURE;        
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERRCODE_API,
                         recoveryAction,
                         cmdStatus,
                         "CPU cmd ADRV903X_CPU_CMD_ID_JESD_TX_LANE_POWER failed");
        goto cleanup;
    }
    /* 7. Disable All deserializer lanes */
    lanesEnabled = 0U;
    for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DEFRAMERS; index++)
    {
        lanesEnabled |= device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled;
    }

    regAddr = ADRV903X_ADDR_SERDES_RXDIG_PHY_PD_0;
    for (index = 0U; index < (uint32_t)ADRV903X_JESD_MAX_DESERIALIZER_LANES; index++)
    {
        if ((lanesEnabled & (1U << index)) != 0U)
        {
            if (deserializerPdCfg == 0xFFFFU)
            {
                recoveryAction = adi_adrv903x_RegistersByteRead(device, NULL, regAddr, regData, NULL, 2U);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Reading Deserrializer Power Down Register");
                    goto cleanup;
                }
                deserializerPdCfg = (((uint16_t)regData[0]) | (((uint16_t)regData[1]) << 8U));
            }

            regData[0] = 0x3FU;
            regData[1] = 0x07U;
            recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, regAddr, regData, 2U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Reading Deserrializer Power Down Register");
                goto cleanup;
            }
        }
        regAddr += 0x800U;
    }
    standbyRecover->deserializerPowerDownReg = deserializerPdCfg;

    /* 8. Disable Framers */
    recoveryAction = adi_adrv903x_FramerLinkStateSet(device, (uint8_t)ADI_ADRV903X_ALL_FRAMERS, ADI_DISABLE);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Disabling Framers");
        goto cleanup;
    }

    /* 9. Disable Deframers */
    recoveryAction = adi_adrv903x_DeframerLinkStateSet(device, (uint8_t)ADI_ADRV903X_ALL_DEFRAMER, ADI_DISABLE);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Disabling Deframers");
        goto cleanup;
    }

    /* 10. Disable SERDES PLL */
    recoveryAction = adi_adrv903x_RegistersByteRead(device, NULL, ADRV903X_ADDR_SERDES_PLL_MISC_PD, regData, NULL, 3U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Reading SERDES PLL Power Down Register");
        goto cleanup;
    }
    standbyRecover->serdesPllPowerDownCfg = (((uint32_t)regData[0]) |
                                            (((uint32_t)regData[1]) << 8U) |
                                            (((uint32_t)regData[2]) << 16U));

    recoveryAction = adi_adrv903x_RegistersByteRead(device, NULL, ADRV903X_ADDR_SERDES_PLL_OUTPUT_DIVIDER_CTL, regData, NULL, 1U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Reading SERDES PLL Output Control Register");
        goto cleanup;
    }
    standbyRecover->serdesPllPowerDownCfg |= (((uint32_t)regData[0]) << 24U);

    regData[0] = PLL_MISC_PD_DISABLE;
    regData[1] = PLL_MISC_PD_KILLS_DISABLE;
    regData[2] = PLL_LO_GEN_SYNC_PD_DISABLE;
    recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_SERDES_PLL_MISC_PD, regData, 3U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Writing SERDES PLL Power Down Register");
        goto cleanup;
    }

    regData[0] = 0xFFU;
    recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_SERDES_PLL_OUTPUT_DIVIDER_CTL, regData, 1U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Writing SERDES PLL Output Control Register");
        goto cleanup;
    }

    /* 11. Disable RF LO1 PLL */
    recoveryAction = adi_adrv903x_RegistersByteRead(device, NULL, ADRV903X_ADDR_RFLO1_PLL_MISC_PD, regData, NULL, 3U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Reading RF LO1 PLL Power Down Register");
        goto cleanup;
    }
    standbyRecover->lo1PllPowerDownCfg = (((uint32_t)regData[0]) |
                                         (((uint32_t)regData[1]) <<  8U) |
                                         (((uint32_t)regData[2]) << 16U));

    regData[0] = PLL_MISC_PD_DISABLE;
    regData[1] = PLL_MISC_PD_KILLS_DISABLE;
    regData[2] = PLL_LO_GEN_SYNC_PD_DISABLE;
    recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_RFLO1_PLL_MISC_PD, regData, 3U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Writing RF LO1 PLL Power Down Register");
        goto cleanup;
    }

    /* 12. Disable RF LO0 PLL */
    recoveryAction = adi_adrv903x_RegistersByteRead(device, NULL, ADRV903X_ADDR_RFLO0_PLL_MISC_PD, regData, NULL, 3U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Reading RF LO0 PLL Power Down Register");
        goto cleanup;
    }
    standbyRecover->lo0PllPowerDownCfg = (((uint32_t)regData[0]) |
                                         (((uint32_t)regData[1]) <<  8U) |
                                         (((uint32_t)regData[2]) << 16U));

    regData[0] = PLL_MISC_PD_DISABLE;
    regData[1] = PLL_MISC_PD_KILLS_DISABLE;
    regData[2] = PLL_LO_GEN_SYNC_PD_DISABLE;
    recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_RFLO0_PLL_MISC_PD, regData, 3U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Writing RF LO0 PLL Power Down Register");
        goto cleanup;
    }

    /* 13. Disable TX DAC */
    regData[0] = TX_DAC_PD_DISABLE;
    regData[1] = TX_DAC_PD_DISABLE;
    for (index = 0U; index < ADI_ADRV903X_MAX_TXCHANNELS; index++)
    {
        if (txInitChMask & (1U << index))
        {
            recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, TX_DAC_PWD_DOWN_I_ADDRS[index], regData, 2U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Error while disabling TX channel DAC");
                goto cleanup;
            }
        }
    }

    /* 14. Disable RX ADC */
    for (index = 0U; index < ADI_ADRV903X_MAX_RX_ONLY; index++)
    {
        if (rxInitChMask & (1U << index))
        {
            regAddr = RX_ADC_REGMAP0_CTRL_FD_PD_ADDRS[index];
            recoveryAction = adi_adrv903x_RegistersByteRead(device, NULL, regAddr, regData, NULL, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, regAddr, "Error while Reading Rx ADC Power Down Registers");
                goto cleanup;
            }
            standbyRecover->rxAdcPowerDownCtrl = ((uint32_t)regData[0]);

            regAddr = RX_ADC_REGMAP0_CTRL_FL_PD_ADDRS[index];
            recoveryAction = adi_adrv903x_RegistersByteRead(device, NULL, regAddr, regData, NULL, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, regAddr, "Error while Reading Rx ADC Power Down Registers");
                goto cleanup;
            }
            standbyRecover->rxAdcPowerDownCtrl |= (((uint32_t)regData[0]) << 8U);

            regAddr = RX_ADC_REGMAP0_CTRL_FD_PD_ADDRS[index] + 0x80U;
            recoveryAction = adi_adrv903x_RegistersByteRead(device, NULL, regAddr, regData, NULL, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, regAddr, "Error while Reading Rx ADC Power Down Registers");
                goto cleanup;
            }
            standbyRecover->rxAdcPowerDownCtrl |= (((uint32_t)regData[0]) << 16U);

            regAddr = RX_ADC_REGMAP0_CTRL_FL_PD_ADDRS[index] + 0x80U;
            recoveryAction = adi_adrv903x_RegistersByteRead(device, NULL, regAddr, regData, NULL, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, regAddr, "Error while Reading Rx ADC Power Down Registers");
                goto cleanup;
            }
            standbyRecover->rxAdcPowerDownCtrl |= (((uint32_t)regData[0]) << 24U);
            break;
        }
    }

    for (index = 0U; index < ADI_ADRV903X_MAX_RX_ONLY; index++)
    {
        if (rxInitChMask & (1U << index))
        {
            regAddr = RX_ADC_REGMAP0_CTRL_FD_PD_ADDRS[index];
            recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, regAddr, &RX_ADC_CTRL_FD_PD_DISABLE, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, regAddr, "Error while disabling RX channel ADC");
                goto cleanup;
            }

            regAddr = RX_ADC_REGMAP0_CTRL_FL_PD_ADDRS[index];
            recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, regAddr, &RX_ADC_CTRL_FL_PD_DISABLE, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, regAddr, "Error while disabling RX channel ADC");
                goto cleanup;
            }

            regAddr = RX_ADC_REGMAP0_CTRL_FD_PD_ADDRS[index] + 0x80U;
            recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, regAddr, &RX_ADC_CTRL_FD_PD_DISABLE, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, regAddr, "Error while disabling RX channel ADC");
                goto cleanup;
            }

            regAddr = RX_ADC_REGMAP0_CTRL_FL_PD_ADDRS[index] + 0x80U;
            recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, regAddr, &RX_ADC_CTRL_FL_PD_DISABLE, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, regAddr, "Error while disabling RX channel ADC");
                goto cleanup;
            }
        }
    }

    /* 15. Disable ORX ADC */
    for (index = 0U; index < ADI_ADRV903X_MAX_ORX; index++)
    {
        if ((orxInitChMask >> ADI_ADRV903X_MAX_RX_ONLY) & (1U << index))
        {
            regAddr = (index == 0U) ? ADRV903X_BF_SLICE_ORX_0__ORX_DIG : ADRV903X_BF_SLICE_ORX_1__ORX_DIG;
            recoveryAction = adrv903x_OrxDig_AdcPdN_BfSet(device, NULL, (adrv903x_BfOrxDigChanAddr_e)regAddr, 0U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Error while disabling ORX channel ADC");
                goto cleanup;
            }
        }
    }

    /* 16. Disable TX LB ADC */
    for (index = 0U; index < ADI_ADRV903X_MAX_TXCHANNELS; index++)
    {
        if (txInitChMask & (1U << index))
        {
            regAddr = TX_LB_ADC_WEST_TRM_PDN_CTRL_ADDRS[index];
            recoveryAction = adi_adrv903x_RegistersByteRead(device, NULL, regAddr, regData, NULL, 4U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, regAddr, "Error while Reading West Tx LB ADC Power Down Registers");
                goto cleanup;
            }
            standbyRecover->txLbAdcTrmPowerDownCtrl = (((uint32_t)regData[0] & 0x3FU) |
                                                      (((uint32_t)regData[1] & 0x3FU) <<  6U) |
                                                      (((uint32_t)regData[2] & 0x03U) << 12U) |
                                                      (((uint32_t)regData[3] & 0x03U) << 14U));

            regAddr = TX_LB_ADC_EAST_TRM_PDN_CTRL_ADDRS[index];
            recoveryAction = adi_adrv903x_RegistersByteRead(device, NULL, regAddr, regData, NULL, 4U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, regAddr, "Error while Reading East Tx LB ADC Power Down Registers");
                goto cleanup;
            }
            standbyRecover->txLbAdcTrmPowerDownCtrl |= ((((uint32_t)regData[0] & 0xCFU) << 16U) |
                                                        (((uint32_t)regData[1] & 0x03U) << 24U) |
                                                        (((uint32_t)regData[2] & 0x03U) << 26U) |
                                                        (((uint32_t)regData[3] & 0x03U) << 28U));
            break;
        }
    }

    for (index = 0U; index < ADI_ADRV903X_MAX_TXCHANNELS; index++)
    {
        if (txInitChMask & (1U << index))
        {
            regAddr = TX_LB_ADC_WEST_TRM_PDN_CTRL_ADDRS[index];
            regData[0] = 0x3FU;
            regData[1] = 0x3FU;
            regData[2] = 0x03U;
            regData[3] = 0x03U;
            recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, regAddr, regData, 4U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, regAddr, "Error while Writing West Tx LB ADC Power Down Registers");
                goto cleanup;
            }

            regAddr = TX_LB_ADC_EAST_TRM_PDN_CTRL_ADDRS[index];
            regData[0] = 0xCFU;
            regData[1] = 0x03U;
            regData[2] = 0x03U;
            regData[3] = 0x03U;
            recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, regAddr, regData, 4U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, regAddr, "Error while Writing East Tx LB ADC Power Down Registers");
                goto cleanup;
            }
        }
    }

    /* 17. Change hsDigClk source to DevClk */
    recoveryAction = adrv903x_Core_UseDeviceClkAsHsdigclk_BfSet(device, NULL, ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS, 1U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Changing hsDigClk source to DevClk");
        goto cleanup;
    }

    /* 18. Disable CLK PLL */
    recoveryAction = adi_adrv903x_RegistersByteRead(device, NULL, ADRV903X_ADDR_CLK_PLL_MISC_PD, regData, NULL, 3U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Reading CLK PLL Power Down Register");
        goto cleanup;
    }
    standbyRecover->clkPllPowerDownCfg = (((uint32_t)regData[0]) |
                                         (((uint32_t)regData[1]) <<  8U) |
                                         (((uint32_t)regData[2]) << 16U));

    regData[0] = PLL_MISC_PD_DISABLE;
    regData[1] = PLL_MISC_PD_KILLS_DISABLE;
    regData[2] = PLL_LO_GEN_SYNC_PD_DISABLE;
    recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_CLK_PLL_MISC_PD, regData, 3U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Writing RF LO1 PLL Power Down Register");
        goto cleanup;
    }

    /* Set device state to STANDBY */
    device->devStateInfo.devState = (adi_adrv903x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV903X_STATE_STANDBY);

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_StandbyRecover(adi_adrv903x_Device_t* const         device,
                                                             adi_adrv903x_StandbyRecover_t* const standbyRecover)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    uint8_t rf0PllRxMux = 0U;
    uint8_t rf1PllRxMux = 0U;
    uint8_t rf0PllTxMux = 0U;
    uint8_t rf1PllTxMux = 0U;

    uint8_t lanesEnabled = 0U;

    uint32_t txInitChMask  = ((device->devStateInfo.initializedChannels >> ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET) & ADI_ADRV903X_TXALL);
    uint32_t rxInitChMask  =  (device->devStateInfo.initializedChannels & ADI_ADRV903X_RX_MASK_ALL);
    uint32_t orxInitChMask =  (device->devStateInfo.initializedChannels & ADI_ADRV903X_ORX_MASK_ALL);
    uint32_t index = 0U;
    uint32_t regAddr = 0U;
    uint8_t  regData[4] = { 0U, 0U, 0U, 0U };

    adrv903x_CpuCmd_ReprogramPll_t reprogramPllCmd;
    adrv903x_CpuCmd_ReprogramPllResp_t reprogramPllCmdRsp;
    adrv903x_CpuCmd_JtxLanePower_t jtxPwrCmd;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;

    const uint32_t TX_DAC_PWD_DOWN_I_ADDRS[ADI_ADRV903X_MAX_TXCHANNELS] = {
        ADRV903X_ADDR_TX0_DAC_PWD_DOWN_I,
        ADRV903X_ADDR_TX1_DAC_PWD_DOWN_I,
        ADRV903X_ADDR_TX2_DAC_PWD_DOWN_I,
        ADRV903X_ADDR_TX3_DAC_PWD_DOWN_I,
        ADRV903X_ADDR_TX4_DAC_PWD_DOWN_I,
        ADRV903X_ADDR_TX5_DAC_PWD_DOWN_I,
        ADRV903X_ADDR_TX6_DAC_PWD_DOWN_I,
        ADRV903X_ADDR_TX7_DAC_PWD_DOWN_I
    };
    const uint32_t RX_ADC_REGMAP0_CTRL_FD_PD_ADDRS[ADI_ADRV903X_MAX_RX_ONLY] = {
        ADRV903X_ADDR_RX0_ADC_REGMAP0_CTRL_FD_PD,
        ADRV903X_ADDR_RX1_ADC_REGMAP0_CTRL_FD_PD,
        ADRV903X_ADDR_RX2_ADC_REGMAP0_CTRL_FD_PD,
        ADRV903X_ADDR_RX3_ADC_REGMAP0_CTRL_FD_PD,
        ADRV903X_ADDR_RX4_ADC_REGMAP0_CTRL_FD_PD,
        ADRV903X_ADDR_RX5_ADC_REGMAP0_CTRL_FD_PD,
        ADRV903X_ADDR_RX6_ADC_REGMAP0_CTRL_FD_PD,
        ADRV903X_ADDR_RX7_ADC_REGMAP0_CTRL_FD_PD
    };
    const uint32_t RX_ADC_REGMAP0_CTRL_FL_PD_ADDRS[ADI_ADRV903X_MAX_RX_ONLY] = {
        ADRV903X_ADDR_RX0_ADC_REGMAP0_CTRL_FL_PD,
        ADRV903X_ADDR_RX1_ADC_REGMAP0_CTRL_FL_PD,
        ADRV903X_ADDR_RX2_ADC_REGMAP0_CTRL_FL_PD,
        ADRV903X_ADDR_RX3_ADC_REGMAP0_CTRL_FL_PD,
        ADRV903X_ADDR_RX4_ADC_REGMAP0_CTRL_FL_PD,
        ADRV903X_ADDR_RX5_ADC_REGMAP0_CTRL_FL_PD,
        ADRV903X_ADDR_RX6_ADC_REGMAP0_CTRL_FL_PD,
        ADRV903X_ADDR_RX7_ADC_REGMAP0_CTRL_FL_PD
    };
    const uint32_t TX_LB_ADC_WEST_TRM_PDN_CTRL_ADDRS[ADI_ADRV903X_MAX_TXCHANNELS] = {
        ADRV903X_ADDR_TX0_LB_ADC_WEST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX1_LB_ADC_WEST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX2_LB_ADC_WEST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX3_LB_ADC_WEST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX4_LB_ADC_WEST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX5_LB_ADC_WEST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX6_LB_ADC_WEST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX7_LB_ADC_WEST_TRM_PDN_CTRL
    };
    const uint32_t TX_LB_ADC_EAST_TRM_PDN_CTRL_ADDRS[ADI_ADRV903X_MAX_TXCHANNELS] = {
        ADRV903X_ADDR_TX0_LB_ADC_EAST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX1_LB_ADC_EAST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX2_LB_ADC_EAST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX3_LB_ADC_EAST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX4_LB_ADC_EAST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX5_LB_ADC_EAST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX6_LB_ADC_EAST_TRM_PDN_CTRL,
        ADRV903X_ADDR_TX7_LB_ADC_EAST_TRM_PDN_CTRL
    };

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, standbyRecover, cleanup);

    if ((device->devStateInfo.devState & ADI_ADRV903X_STATE_STANDBY) != ADI_ADRV903X_STATE_STANDBY)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Device is not in Standby Mode");
        goto cleanup;
    }

    /* 1.a Power up CLK PLL */
    regData[0] =  (standbyRecover->clkPllPowerDownCfg & 0xFFU);
    regData[1] = ((standbyRecover->clkPllPowerDownCfg >> 8U) & 0xFFU);
    regData[2] = ((standbyRecover->clkPllPowerDownCfg >> 16U) & 0xFFU);
    recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_CLK_PLL_MISC_PD, regData, 3U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while enabling CLK PLL");
        goto cleanup;
    }

    /* 1.b Reprogram CLK PLL */
    reprogramPllCmd.pllSel = ADI_ADRV903X_CLK_PLL;
    recoveryAction = adrv903x_CpuCmdSend(device,
                                         ADI_ADRV903X_CPU_TYPE_0,
                                         ADRV903X_LINK_ID_0,
                                         ADRV903X_CPU_CMD_ID_REPROGRAM_PLL,
                                         (void*)&reprogramPllCmd,
                                         sizeof(adrv903x_CpuCmd_ReprogramPll_t),
                                         (void*)&reprogramPllCmdRsp,
                                         sizeof(adrv903x_CpuCmd_ReprogramPllResp_t),
                                         NULL);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Reprogram CLK PLL");
        goto cleanup;
    }

    /* 1.c Change hsDigClk back to CLK PLL */
    recoveryAction = adrv903x_Core_UseDeviceClkAsHsdigclk_BfSet(device, NULL, ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS, 0U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Changing hsDigClk source to CLK PLL");
        goto cleanup;
    }

    /* 1.d Power up TX DAC */
    regData[0] = 0x00U;
    regData[1] = 0x00U;
    for (index = 0U; index < ADI_ADRV903X_MAX_TXCHANNELS; index++)
    {
        if (txInitChMask & (1U << index))
        {
            recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, TX_DAC_PWD_DOWN_I_ADDRS[index], regData, 2U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Error while Enabling TX channel DAC");
                goto cleanup;
            }
        }
    }

    /* 1.e Power up RX ADC */
    for (index = 0U; index < ADI_ADRV903X_MAX_RX_ONLY; index++)
    {
        if (rxInitChMask & (1U << index))
        {
            regAddr = RX_ADC_REGMAP0_CTRL_FD_PD_ADDRS[index];
            regData[0] =  (standbyRecover->rxAdcPowerDownCtrl & 0x2F); /* Ensure PD_DAC_CK is 0 */
            recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, regAddr, regData, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, regAddr, "Failed to Power up RX channel ADC");
                goto cleanup;
            }

            regAddr = RX_ADC_REGMAP0_CTRL_FL_PD_ADDRS[index];
            regData[0] = ((standbyRecover->rxAdcPowerDownCtrl >>  8U) & 0xFF);
            recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, regAddr, regData, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, regAddr, "Failed to Power up RX channel ADC");
                goto cleanup;
            }

            regAddr = RX_ADC_REGMAP0_CTRL_FD_PD_ADDRS[index] + 0x80U;
            regData[0] = ((standbyRecover->rxAdcPowerDownCtrl >> 16U) & 0x2F); /* Ensure PD_DAC_CK is 0 */
            recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, regAddr, regData, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, regAddr, "Failed to Power up RX channel ADC");
                goto cleanup;
            }

            regAddr = RX_ADC_REGMAP0_CTRL_FL_PD_ADDRS[index] + 0x80U;
            regData[0] = ((standbyRecover->rxAdcPowerDownCtrl >> 24U) & 0xFF);
            recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, regAddr, regData, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, regAddr, "Failed to Power up RX channel ADC");
                goto cleanup;
            }
        }
    }

    /* 1.f Power up ORX ADC */
    for (index = 0U; index < ADI_ADRV903X_MAX_ORX; index++)
    {
        if ((orxInitChMask >> ADI_ADRV903X_MAX_RX_ONLY) & (1U << index))
        {
            regAddr = (index == 0U) ? ADRV903X_BF_SLICE_ORX_0__ORX_DIG : ADRV903X_BF_SLICE_ORX_1__ORX_DIG;
            recoveryAction = adrv903x_OrxDig_AdcPdN_BfSet(device, NULL, (adrv903x_BfOrxDigChanAddr_e)regAddr, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Error while enabling ORX channel ADC");
                goto cleanup;
            }
        }
    }

    /* 1.g Power up LB ADC */
    for (index = 0U; index < ADI_ADRV903X_MAX_TXCHANNELS; index++)
    {
        if (txInitChMask & (1U << index))
        {
            regAddr = TX_LB_ADC_WEST_TRM_PDN_CTRL_ADDRS[index];
            regData[0] =  (standbyRecover->txLbAdcTrmPowerDownCtrl & 0x3FU);
            regData[1] = ((standbyRecover->txLbAdcTrmPowerDownCtrl >>  6U) & 0x3FU);
            regData[2] = ((standbyRecover->txLbAdcTrmPowerDownCtrl >> 12U) & 0x03U);
            regData[3] = ((standbyRecover->txLbAdcTrmPowerDownCtrl >> 14U) & 0x03U);
            recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, regAddr, regData, 4U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Error while Enabling West Tx channel LB ADC");
                goto cleanup;
            }

            regAddr = TX_LB_ADC_EAST_TRM_PDN_CTRL_ADDRS[index];
            regData[0] = ((standbyRecover->txLbAdcTrmPowerDownCtrl >> 16U) & 0xCFU);
            regData[1] = ((standbyRecover->txLbAdcTrmPowerDownCtrl >> 24U) & 0x03U);
            regData[2] = ((standbyRecover->txLbAdcTrmPowerDownCtrl >> 26U) & 0x03U);
            regData[3] = ((standbyRecover->txLbAdcTrmPowerDownCtrl >> 28U) & 0x03U);
            recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, regAddr, regData, 4U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Error while Enabling East Tx channel LB ADC");
                goto cleanup;
            }
        }
    }

    /* Read Channel RF PLL Selectors */
    recoveryAction = adrv903x_PllMemMap_SelRxLo_BfGet(device, NULL, ADRV903X_BF_DIGITAL_CORE_EAST_RFPLL, &rf0PllRxMux);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read RX Channel RF0 PLL Selector");
        goto cleanup;
    }
    recoveryAction = adrv903x_PllMemMap_SelRxLo_BfGet(device, NULL, ADRV903X_BF_DIGITAL_CORE_WEST_RFPLL, &rf1PllRxMux);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read RX Channel RF1 PLL Selector");
        goto cleanup;
    }
    recoveryAction = adrv903x_PllMemMap_SelTxLo_BfGet(device, NULL, ADRV903X_BF_DIGITAL_CORE_EAST_RFPLL, &rf0PllTxMux);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read TX Channel RF0 PLL Selector");
        goto cleanup;
    }
    recoveryAction = adrv903x_PllMemMap_SelTxLo_BfGet(device, NULL, ADRV903X_BF_DIGITAL_CORE_WEST_RFPLL, &rf1PllTxMux);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read TX Channel RF1 PLL Selector");
        goto cleanup;
    }

    /* 1.h Power up RF0 PLL (if it is being used)*/
    if (((rf0PllTxMux == 1U) && ((txInitChMask & 0x0FU) != 0U)) | /* tx channel 0-3 connected to PLL0 */
        ((rf1PllTxMux == 0U) && ((txInitChMask & 0xF0U) != 0U)) | /* tx channel 4-7 connected to PLL0 */
        ((rf0PllRxMux == 1U) && ((rxInitChMask & 0x0FU) != 0U)) | /* rx channel 0-3 connected to PLL0 */
        ((rf1PllRxMux == 0U) && ((rxInitChMask & 0xF0U) != 0U)))  /* rx channel 4-7 connected to PLL0 */
    {
        regData[0] =  (standbyRecover->lo0PllPowerDownCfg & 0xFFU);
        regData[1] = ((standbyRecover->lo0PllPowerDownCfg >> 8U) & 0xFFU);
        regData[2] = ((standbyRecover->lo0PllPowerDownCfg >> 16U) & 0xFFU);
        recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_RFLO0_PLL_MISC_PD, regData, 3U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Power up RF0 PLL");
            goto cleanup;
        }

        /* 1.i Reprogram RF0 PLL */
        reprogramPllCmd.pllSel = ADI_ADRV903X_RF0_PLL;
        recoveryAction = adrv903x_CpuCmdSend(device,
                                             ADI_ADRV903X_CPU_TYPE_0,
                                             ADRV903X_LINK_ID_0,
                                             ADRV903X_CPU_CMD_ID_REPROGRAM_PLL,
                                             (void*)&reprogramPllCmd,
                                             sizeof(adrv903x_CpuCmd_ReprogramPll_t),
                                             (void*)&reprogramPllCmdRsp,
                                             sizeof(adrv903x_CpuCmd_ReprogramPllResp_t),
                                             NULL);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Reprogram RF0 PLL");
            goto cleanup;
        }
    }

    /* 1.j Power up RF1 PLL (if it is being used)*/
    if (((rf0PllTxMux == 0U) && ((txInitChMask & 0x0FU) != 0U)) | /* tx channel 0-3 connected to PLL1 */
        ((rf1PllTxMux == 1U) && ((txInitChMask & 0xF0U) != 0U)) | /* tx channel 4-7 connected to PLL1 */
        ((rf0PllRxMux == 0U) && ((rxInitChMask & 0x0FU) != 0U)) | /* rx channel 0-3 connected to PLL1 */
        ((rf1PllRxMux == 1U) && ((rxInitChMask & 0xF0U) != 0U)))  /* rx channel 4-7 connected to PLL1 */
    {
        regData[0] =  (standbyRecover->lo1PllPowerDownCfg & 0xFFU);
        regData[1] = ((standbyRecover->lo1PllPowerDownCfg >> 8U) & 0xFFU);
        regData[2] = ((standbyRecover->lo1PllPowerDownCfg >> 16U) & 0xFFU);
        recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_RFLO1_PLL_MISC_PD, regData, 3U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Power up RF1 PLL");
            goto cleanup;
        }

        /* 1.k Reprogram RF1 PLL */
        reprogramPllCmd.pllSel = ADI_ADRV903X_RF1_PLL;
        recoveryAction = adrv903x_CpuCmdSend(device,
                                             ADI_ADRV903X_CPU_TYPE_0,
                                             ADRV903X_LINK_ID_0,
                                             ADRV903X_CPU_CMD_ID_REPROGRAM_PLL,
                                             (void*)&reprogramPllCmd,
                                             sizeof(adrv903x_CpuCmd_ReprogramPll_t),
                                             (void*)&reprogramPllCmdRsp,
                                             sizeof(adrv903x_CpuCmd_ReprogramPllResp_t),
                                             NULL);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Reprogram RF1 PLL");
            goto cleanup;
        }
    }

    /* 1.l Power up Serdes PLL */
    regData[0] =  (standbyRecover->serdesPllPowerDownCfg & 0xFFU);
    regData[1] = ((standbyRecover->serdesPllPowerDownCfg >> 8U) & 0xFFU);
    regData[2] = ((standbyRecover->serdesPllPowerDownCfg >> 16U) & 0xFFU);
    recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_SERDES_PLL_MISC_PD, regData, 3U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Power up Serdes PLL");
        goto cleanup;
    }

    regData[0] = ((standbyRecover->serdesPllPowerDownCfg >> 24U) & 0xFFU);
    recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_SERDES_PLL_OUTPUT_DIVIDER_CTL, regData, 1U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Serdes PLL output divider control");
        goto cleanup;
    }

    /* 1.m Reprogram Serdes PLL */
    reprogramPllCmd.pllSel = ADI_ADRV903X_SERDES_PLL;
    recoveryAction = adrv903x_CpuCmdSend(device,
                                         ADI_ADRV903X_CPU_TYPE_0,
                                         ADRV903X_LINK_ID_0,
                                         ADRV903X_CPU_CMD_ID_REPROGRAM_PLL,
                                         (void*)&reprogramPllCmd,
                                         sizeof(adrv903x_CpuCmd_ReprogramPll_t),
                                         (void*)&reprogramPllCmdRsp,
                                         sizeof(adrv903x_CpuCmd_ReprogramPllResp_t),
                                         NULL);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Reprogram SERDES PLL");
        goto cleanup;
    }

    /* 1.n Power up Serializer lanes and Framer */
    lanesEnabled = 0U;
    for(index = 0U ; index < (uint32_t)ADI_ADRV903X_MAX_FRAMERS ; index++)
    {
        lanesEnabled |= device->initExtract.jesdSetting.framerSetting[index].serialLaneEnabled;
    }

    /* Prepare the command payload to power-down all enabled jtx lanes */
    jtxPwrCmd.jtxLaneMask = lanesEnabled;
    jtxPwrCmd.jtxLanePower = 0xFF;
    
    /* Send command. There is no cmd-specific response expected.
     * This command is always sent to CPU0 regardless of lane-CPU assignment. */
    recoveryAction = adrv903x_CpuCmdSend(device,
                                         ADI_ADRV903X_CPU_TYPE_0,
                                         ADRV903X_LINK_ID_0,
                                         ADRV903X_CPU_CMD_ID_JESD_TX_LANE_POWER,
                                         (void*)&jtxPwrCmd,
                                         sizeof(jtxPwrCmd),
                                         NULL,
                                         0U,
                                         &cmdStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to call CPU cmd ADRV903X_CPU_CMD_ID_JESD_TX_LANE_POWER");
        goto cleanup;
    }
     
    if (cmdStatus != ADRV903X_CPU_CMD_STATUS_NO_ERROR)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_FEATURE;        
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERRCODE_API,
                         recoveryAction,
                         cmdStatus,
                         "CPU cmd ADRV903X_CPU_CMD_ID_JESD_TX_LANE_POWER failed");
        goto cleanup;
    }

    for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_FRAMERS; index++)
    {
        if (device->initExtract.jesdSetting.framerSetting[index].serialLaneEnabled != 0U)
        {
            recoveryAction = adi_adrv903x_FramerLinkStateSet(device, (1U << index), ADI_ENABLE);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Failed to Enable Framer");
                goto cleanup;
            }
        }
    }

    /* 1.o Power up Deserializer and Deframer */
    lanesEnabled = 0U;
    for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DEFRAMERS; index++)
    {
        lanesEnabled |= device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled;
    }

    regData[0] = standbyRecover->deserializerPowerDownReg & 0xFFU;
    regData[1] = (standbyRecover->deserializerPowerDownReg >> 8U) & 0xFFU;
    regAddr = ADRV903X_ADDR_SERDES_RXDIG_PHY_PD_0;
    for (index = 0U; index < (uint32_t)ADRV903X_JESD_MAX_DESERIALIZER_LANES; index++)
    {
        if ((lanesEnabled & (1U << index)) != 0U)
        {
            recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, regAddr, regData, 2U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Writing Deserializer Power Down Register");
                goto cleanup;
            }
        }
        regAddr += 0x800U;
    }

    for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DEFRAMERS; index++)
    {
        if (device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled != 0U)
        {
            recoveryAction = adi_adrv903x_DeframerLinkStateSet(device, (1U << index), ADI_ENABLE);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Failed to Enable Deframer");
                goto cleanup;
            } 
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_StandbyExit(adi_adrv903x_Device_t* const            device,
                                                          adi_adrv903x_StandbyRecover_t* const    standbyRecover)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    uint32_t txInitChMask  = ((device->devStateInfo.initializedChannels >> ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET) & ADI_ADRV903X_TXALL);
    uint32_t rxInitChMask  =  (device->devStateInfo.initializedChannels & ADI_ADRV903X_RX_MASK_ALL);
    uint32_t orxInitChMask =  (device->devStateInfo.initializedChannels & ADI_ADRV903X_ORX_MASK_ALL);

    uint32_t trackingCal = 0U;
    uint32_t index = 0U;
    uint32_t channelMask = 0U;
    uint32_t baseAddr = 0U;

    adi_adrv903x_ChannelTrackingCals_t channelMaskv2;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, standbyRecover, cleanup);

    if ((device->devStateInfo.devState & ADI_ADRV903X_STATE_STANDBY) != ADI_ADRV903X_STATE_STANDBY)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Device is not in Standby Mode");
        goto cleanup;
    }

    /* Clear possible Framer errors */
    baseAddr = (uint32_t) ADRV903X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;
    for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_FRAMERS; ++index)
    {
        if (device->initExtract.jesdSetting.framerSetting[index].serialLaneEnabled != 0U)
        {
            /* clear PCLK error */
            recoveryAction = adrv903x_JtxLink_JtxPclkErrorClear_BfSet(device, NULL, (adrv903x_BfJtxLinkChanAddr_e)baseAddr, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Failed to Clear Framer PCLK Errors");
                goto cleanup;
            }
            recoveryAction = adrv903x_JtxLink_JtxPclkErrorClear_BfSet(device, NULL, (adrv903x_BfJtxLinkChanAddr_e)baseAddr, 0U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Error after clearing Framer PCLK Errors");
                goto cleanup;
            }
            /* clear SYSREF error */
            recoveryAction = adrv903x_JtxLink_JtxTplSysrefClrPhaseErr_BfSet(device, NULL, (adrv903x_BfJtxLinkChanAddr_e)baseAddr, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Failed to Clear Framer SYSREF Errors");
                goto cleanup;
            }
            recoveryAction = adrv903x_JtxLink_JtxTplSysrefClrPhaseErr_BfSet(device, NULL, (adrv903x_BfJtxLinkChanAddr_e)baseAddr, 0U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Error after clearing Framer SYSREF Errors");
                goto cleanup;
            }
        }
        baseAddr += 0x10000U;
    }

    /* Re-enable channels when we're done (restore the SPI mode registers) */
    recoveryAction = adi_adrv903x_RxTxEnableSet(device,
                                                orxInitChMask, standbyRecover->orxEnabledMask,
                                                rxInitChMask,  standbyRecover->rxEnabledMask,
                                                txInitChMask,  standbyRecover->txEnabledMask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Re-enabling channels.");
        goto cleanup;
    }

    /* Re-enable Tracking Cals (starting from the ADC cals) */
    for (trackingCal = (uint32_t)ADI_ADRV903X_TC_ORX_ADC_MASK; trackingCal >= (uint32_t)ADI_ADRV903X_TC_RX_QEC_MASK; trackingCal >>= 1U)
    {
        channelMask = 0U;

        for (index = 0U; index < ADI_ADRV903X_NUM_TRACKING_CAL_CHANNELS; index++)
        {
            if (standbyRecover->tcEnableMasks.enableMask[index] & trackingCal)
            {
                channelMask |= (1U << index);
            }
        }

        if (channelMask)
        {
            channelMaskv2.rxChannel  = channelMask & 0xFFU;
            channelMaskv2.txChannel  = channelMask & 0xFFU;
            channelMaskv2.orxChannel = channelMask & 0x03U;
            channelMaskv2.laneSerdes = channelMask & 0xFFU;
            recoveryAction = adi_adrv903x_TrackingCalsEnableSet_v2(device, (adi_adrv903x_TrackingCalibrationMask_e)trackingCal, &channelMaskv2, ADI_ADRV903X_TRACKING_CAL_ENABLE);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, trackingCal, "Error while enabling Tracking cal");
                goto cleanup;
            }
        }
    }

    /* Clear STANDBY state */
    device->devStateInfo.devState = (adi_adrv903x_ApiStates_e)(device->devStateInfo.devState & ~ADI_ADRV903X_STATE_STANDBY);

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_StandbyExitStatusGet(adi_adrv903x_Device_t* const         device,
                                                                   adi_adrv903x_StandbyRecover_t* const standbyRecover,
                                                                   uint8_t * const                      done)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    uint32_t txInitChMask  = ((device->devStateInfo.initializedChannels >> ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET) & ADI_ADRV903X_TXALL);

    uint32_t chanSel = 0U;
    uint32_t index = 0U;

    uint32_t adcCalStatus[ADRV903X_TXLB_ADC_FAST_ATTACK_STATUS_IDX + 1U] = { 0 };

    uint8_t  localDone   = ADI_TRUE;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, standbyRecover, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, done, cleanup);

    for (index = 0U; index < ADI_ADRV903X_MAX_TXCHANNELS; index++)
    {
        chanSel = (1U << index);
        if (txInitChMask & chanSel)
        {
            /* Get TXLB ADC Calibration Status */
            recoveryAction = adi_adrv903x_CalSpecificStatusGet( device,
                                                                (adi_adrv903x_Channels_e)(chanSel),
                                                                ADRV903X_CPU_OBJID_TC_TXLB_ADC,
                                                                (uint8_t *)adcCalStatus,
                                                                (ADRV903X_TXLB_ADC_FAST_ATTACK_STATUS_IDX + 1U) * sizeof(uint32_t));
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, chanSel, "Get TXLB ADC calibration status failed");
                goto cleanup;
            }

            if (ADRV903X_HTOCL(adcCalStatus[ADRV903X_TXLB_ADC_FAST_ATTACK_STATUS_IDX]) != 0U)
            {
                localDone = ADI_FALSE;
                goto cleanup;
            }
        }
    }

cleanup:
    if (recoveryAction == ADI_ADRV903X_ERR_ACT_NONE)
    {
        *done = localDone;
    }
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

static adi_adrv903x_ErrAction_e adrv903x_CpuMemDumpBinWrite(adi_adrv903x_Device_t* const                        device,
                                                            const adi_adrv903x_CpuMemDumpBinaryInfo_t* const    cpuMemDumpBinaryInfoPtr,
                                                            const uint8_t                                       forceException,
                                                            uint32_t* const                                     dumpSize,
                                                            const uint8_t                                       ramOnlyFlag)
{
    adi_adrv903x_ErrAction_e    recoveryAction                                    = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    FILE*                       ofp                                               = NULL;
    uint32_t                    byteCount                                         = 0U;
    uint32_t                    cpuIdx                                            = 0U;
    uint32_t                    exceptionValue                                    = 0U;
    uint8_t                     binaryRead[ADI_ADRV903X_MEM_DUMP_CHUNK_SIZE + 10] = { 0U };
    uint32_t                    tableIdx                                          = 0U;
    uint32_t                    i                                                 = 0U;
    uint32_t                    startAddr                                         = 0U;
    uint32_t                    endAddr                                           = 0U;
    uint32_t                    timeout_us                                        = 0U;
    uint32_t                    waitInterval_us                                   = 0U;
    uint32_t                    eventCheck                                        = 0U;
    uint32_t                    numEventChecks                                    = 0U;
    uint32_t                    cpuTableSize                                      = 0U;
    uint8_t                     txChanId                                          = 0U;
    uint8_t                     sliceClkEn                                        = 0x01U;
    const uint32_t              STREAM_CPU_RAM_ADDR                               = 0x46A00000U;
    adrv903x_BfTxDigChanAddr_e  txDigBaseAddr[ADI_ADRV903X_MAX_TXCHANNELS] = { ADRV903X_BF_SLICE_TX_0__TX_DIG, ADRV903X_BF_SLICE_TX_1__TX_DIG, ADRV903X_BF_SLICE_TX_2__TX_DIG, ADRV903X_BF_SLICE_TX_3__TX_DIG, ADRV903X_BF_SLICE_TX_4__TX_DIG, ADRV903X_BF_SLICE_TX_5__TX_DIG, ADRV903X_BF_SLICE_TX_6__TX_DIG, ADRV903X_BF_SLICE_TX_7__TX_DIG };


    
    const uint32_t cpuExceptionAddr[ADI_ADRV903X_CPU_TYPE_MAX_RADIO] = { ADRV903X_CPU_0_PM_EXCEPTION_FLAG, ADRV903X_CPU_1_PM_EXCEPTION_FLAG }; /* Exception Flag Memory */

    uint32_t recordCrc      = 0U;
    uint8_t  recordPadding  = 0U;

    /* File header variables */
    uint16_t   fileFormatVersion    = 0U;
    uint16_t   productId            = 0U;
    uint16_t   productRevision      = 0U;
    time_t     rawtime;
    struct tm *tmPtr;

    /* Generic record header varibales */
    uint32_t recordType;
    uint32_t recordLength;
    uint8_t  endianness = 0U;

    /* Device Driver record variables */
    uint16_t driverId;
    uint16_t instanceId;
    uint32_t driverStateLength;
    uint16_t driverVersionMajor;
    uint16_t driverVersionMinor;
    uint16_t driverVersionPatch;
    uint8_t  driverVersionPrerel[ADI_ADRV903X_VERSION_PREREL_SIZE] = {0U};
    uint32_t driverVersionBuild;
    uint16_t tmYear     = 0U;
    uint8_t  tmMonth    = 0U;
    uint8_t  tmDay      = 0U;
    uint8_t  tmHour     = 0U;
    uint8_t  tmMin      = 0U;
    uint8_t  tmSec      = 0U;
    uint8_t* driverPtr;

    /* CPU RAM Record Variables */
    uint64_t startAddress;
    uint32_t memLength;
    uint16_t ramId;
    uint16_t cpuType;

    /* Telemetry Buffer Record Variables */
    uint32_t bufferLength;
    uint16_t bufferId;
    uint16_t formatVersion = 0x1U;

    /* Register Record Variables */
    uint32_t length;
    uint8_t registerWidth;

    /* Firmware Version Records */
    uint16_t firmwareSemverMajor;
    uint16_t firmwareSemverMinor;
    uint16_t firmwareSemverPatch;
    uint8_t  firmwareVersionPrerel[ADI_ADRV903X_VERSION_PREREL_SIZE] = {0U};
    uint32_t firmwareSemverBuild;
    uint32_t firmwareId;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, cpuMemDumpBinaryInfoPtr, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, cpuMemDumpBinaryInfoPtr->filePath, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, dumpSize, cleanup);

    if ((forceException != 0U) &&
        (forceException != 1U))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, forceException, "forceException param must be 0 or 1");
        goto cleanup;
    }

#ifdef __GNUC__
    ofp = ADI_LIBRARY_FOPEN((const char*)cpuMemDumpBinaryInfoPtr->filePath, "wb");
#else
    ADI_LIBRARY_FOPEN_S(&ofp, (const char*)cpuMemDumpBinaryInfoPtr->filePath, "wb");
#endif

    if (ofp == NULL)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ofp, "Unable to open binary image file. Please check if the path is correct");
        goto cleanup;
    }

    if (forceException == ADI_TRUE)
    {


                /* For each main CPU force an exception unless it is already in exception state */
        for (cpuIdx = ADI_ADRV903X_CPU_TYPE_0; cpuIdx < ADI_ADRV903X_CPU_TYPE_MAX_RADIO; cpuIdx++)
        {
            recoveryAction = adi_adrv903x_Register32Read(device, NULL, cpuExceptionAddr[cpuIdx], &exceptionValue, 0xFFFFFFFF);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to Read Cpu memory");
                goto cleanup;
            }

            if (exceptionValue == (uint32_t)ADRV903X_CPU_NO_EXCEPTION)
            {
                /* CPU is not in exception state; Force it into exception state */
                recoveryAction = adrv903x_CpuForceException(device, (adi_adrv903x_CpuType_e)cpuIdx);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CpuForceException issue");
                    goto cleanup;
                }

                /* Poll until CPU is in exception state or timeout */
                timeout_us = ADI_ADRV903X_WRITECPUEXCEPTION_TIMEOUT_US;
                waitInterval_us = ADI_ADRV903X_WRITECPUEXCEPTION_INTERVAL_US;
#if ADI_ADRV903X_WRITECPUEXCEPTION_INTERVAL_US > ADI_ADRV903X_WRITECPUEXCEPTION_TIMEOUT_US
                waitInterval_us = timeout_us;
#elif ADI_ADRV903X_WRITECPUEXCEPTION_INTERVAL_US == 0
                waitInterval_us = timeout_us;
#endif
                numEventChecks = timeout_us / waitInterval_us;

                /* timeout event check loop */
                for (eventCheck = 0U; eventCheck <= numEventChecks; ++eventCheck)
                {
                    recoveryAction = adi_adrv903x_Register32Read(device, NULL, cpuExceptionAddr[cpuIdx], &exceptionValue, 0xFFFFFFFF);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to Read Cpu memory");
                        goto cleanup;
                    }

                    if (exceptionValue == (uint32_t)ADRV903X_CPU_NO_EXCEPTION)
                    {
                        recoveryAction = (adi_adrv903x_ErrAction_e) adi_common_hal_Wait_us(&device->common, waitInterval_us);
                        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Request Issue");
                            goto cleanup;
                        }
                    }
                    else
                    {
                        /* CPU is now in exception state */
                        break;
                    }
                }

                if (exceptionValue == (uint32_t)ADRV903X_CPU_NO_EXCEPTION)
                {
                    /* Timeout polling for CPU to be in exception state */
                    recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM; 
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, exceptionValue, "Unable to force CPU to throw exception");
                    goto cleanup;
                }

                /* CPU is now in exception state */
            }
        }

    }

    /* Enable the Tx slice clock of the uninitialized TX channels to avoid the invalid memdump */
    sliceClkEn = 0x01U;
    for (txChanId = 0U; txChanId <= ADI_ADRV903X_TX_CHAN_ID_MAX; txChanId++)
    {
        if (((device->devStateInfo.initializedChannels >> ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET) & (0x01 << (uint32_t)txChanId)) == 0U)
        {
            adrv903x_TxDig_TxAttenClkEnable_BfSet(device, NULL, txDigBaseAddr[txChanId], sliceClkEn);
        }
    }
    
    *dumpSize = 0U;

    /* Generate Header for dump file*/
    fileFormatVersion  = 0x1U;
    productRevision     = device->devStateInfo.deviceSiRev;
    /* Determine product ID */
    adrv903x_Core_EfuseProductId_BfGet(device, NULL, ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS, (uint8_t*) &productId);
    productId = ADI_ADRV903X_PRODUCT_ID_MASK | productId;
    /* Find dump time */
    ADI_LIBRARY_TIME(&rawtime);
    tmPtr = ADI_LIBRARY_GMTIME(&rawtime);

    /* Generate header record*/
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    byteCount = 0U;
    byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], fileFormatVersion);
    byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], productId);
    byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], productRevision);
    byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], tmPtr->tm_year);
    byteCount += adrv903x_insert8Bits( &binaryRead[byteCount], tmPtr->tm_mon);
    byteCount += adrv903x_insert8Bits( &binaryRead[byteCount], tmPtr->tm_mday);
    byteCount += adrv903x_insert8Bits( &binaryRead[byteCount], tmPtr->tm_hour);
    byteCount += adrv903x_insert8Bits( &binaryRead[byteCount], tmPtr->tm_min);
    byteCount += adrv903x_insert8Bits( &binaryRead[byteCount], tmPtr->tm_sec);
    byteCount += 3U;                                                                /* Padding to align to 64-bit boundary */
    /* calculate CRC*/
    recordCrc = adrv903x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_TRUE);
    byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], recordCrc);
    byteCount += 4U;                                                                /* Padding to align to 64-bit boundary */

    /* Insert file header */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }

    /* Generate Device Driver Record */
    recordCrc = 0U;
    byteCount = 0U;
    recordType = ADI_ADRV903X_MEMDUMP_DEV_DRVR_STATE_RECORD;
    recordLength = ADI_ADRV903X_MEMDUMP_DEVICE_DRIVER_HEADER_SIZE + sizeof(*device);
    /* Generate basic header */
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], recordType);
    byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], recordLength);
    /* Insert record header */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    byteCount = 0U;

    /* Get driver record values */
    driverId = 0U; /* TBD */
    instanceId = device->common.id;
    driverStateLength = sizeof(*device);
    driverVersionMajor = ADI_ADRV903X_CURRENT_MAJOR_VERSION;
    driverVersionMinor = ADI_ADRV903X_CURRENT_MINOR_VERSION;
    driverVersionPatch = ADI_ADRV903X_CURRENT_MAINTENANCE_VERSION;
    driverVersionBuild = ADI_ADRV903X_CURRENT_BUILD_VERSION;
    /* Compile driver record header */
    byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], driverId);
    byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], instanceId);
    byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], driverStateLength);
    byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], driverVersionMajor);
    byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], driverVersionMinor);
    byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], driverVersionPatch);
    for(i = 0U; i < ADI_ADRV903X_VERSION_PREREL_SIZE; i++)
    {
        byteCount += adrv903x_insert8Bits(&binaryRead[byteCount], driverVersionPrerel[i]);
    }
    byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], driverVersionBuild);
    byteCount += adrv903x_insert8Bits( &binaryRead[byteCount], endianness);
    byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], tmYear);
    byteCount += adrv903x_insert8Bits( &binaryRead[byteCount], tmMonth);
    byteCount += adrv903x_insert8Bits( &binaryRead[byteCount], tmDay);
    byteCount += adrv903x_insert8Bits( &binaryRead[byteCount], tmHour);
    byteCount += adrv903x_insert8Bits( &binaryRead[byteCount], tmMin);
    byteCount += adrv903x_insert8Bits( &binaryRead[byteCount], tmSec);

    /* Publish driver header to file */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    recordCrc = adrv903x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_FALSE);

    /* Begin publishing Device Handle */
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    byteCount = 0U;
    driverPtr = (uint8_t*) device;
    for(i = 0U; i < driverStateLength; i++)
    {
        if(byteCount >= ADI_ADRV903X_MEM_DUMP_CHUNK_SIZE) 
        {
            /* If at maximum length for buffer, flush */
            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }
            recordCrc = adrv903x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_FALSE);
            byteCount = 0U;
        }
        /* Set buffer to data and increment ptr */
        byteCount += adrv903x_insert8Bits(&binaryRead[byteCount], *(driverPtr++));
    }
    /* Flush remaining data, even if smaller than chunk size */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    /* Calculate final crc */
    recordCrc = adrv903x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_TRUE);
    
    /* Append CRC32 and padding*/
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    recordPadding = (recordLength % 8U) ? 8U - (recordLength % 8U) : 0U; 
    adrv903x_insert32Bits(&binaryRead[recordPadding], recordCrc);
    byteCount = 8U + recordPadding;
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }

    /* Begin Radio FW Record */
    recordCrc = 0U;
    byteCount = 0U;
    recordType = ADI_ADRV903X_MEMDUMP_FIRMWARE_VER_RECORD;
    recordLength = ADI_ADRV903X_MEMDUMP_FIRMWARE_VERSION_HEADER_SIZE;

    /* Generate basic header */
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], recordType);
    byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], recordLength);
    /* Insert record header */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    byteCount = 0U;

    /* Get driver record values */
    firmwareSemverMajor = ADI_ADRV903X_CURRENT_MAJOR_VERSION;
    firmwareSemverMinor = ADI_ADRV903X_CURRENT_MINOR_VERSION;
    firmwareSemverPatch = ADI_ADRV903X_CURRENT_MAINTENANCE_VERSION;
    firmwareSemverBuild = ADI_ADRV903X_CURRENT_BUILD_VERSION;
    firmwareId = ADI_ADRV903X_MEMDUMP_RADIO_FW << ADI_ADRV903X_FIRMWARE_TYPE_ID_SHIFT;

    byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], firmwareSemverMajor);
    byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], firmwareSemverMinor);
    byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], firmwareSemverPatch);
    for(i = 0U; i < ADI_ADRV903X_VERSION_PREREL_SIZE; i++)
    {
        byteCount += adrv903x_insert8Bits(&binaryRead[byteCount], firmwareVersionPrerel[i]);
    }
    byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], firmwareSemverBuild);
    byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], firmwareId);

    /* Publish firmware record to file */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    recordCrc = adrv903x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_TRUE);

    /* Append CRC32 and padding*/
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    recordPadding = (recordLength % 8U) ? 8U - (recordLength % 8U) : 0U; 
    adrv903x_insert32Bits(&binaryRead[recordPadding], recordCrc);
    byteCount = 8U + recordPadding;
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }

    /* Begin Stream FW Record */
    recordCrc = 0U;
    byteCount = 0U;
    recordType = ADI_ADRV903X_MEMDUMP_FIRMWARE_VER_RECORD;
    recordLength = ADI_ADRV903X_MEMDUMP_FIRMWARE_VERSION_HEADER_SIZE;

    /* Generate basic header */
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], recordType);
    byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], recordLength);
    /* Insert record header */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    byteCount = 0U;

    /* Get driver record values */
    firmwareSemverMajor = device->devStateInfo.cpu.devProfileVersion.majorVer;
    firmwareSemverMinor = device->devStateInfo.cpu.devProfileVersion.minorVer;
    firmwareSemverPatch = device->devStateInfo.cpu.devProfileVersion.maintenanceVer;
    firmwareSemverBuild = device->devStateInfo.cpu.devProfileVersion.buildVer;
    firmwareId = ADI_ADRV903X_MEMDUMP_STREAM_FW << ADI_ADRV903X_FIRMWARE_TYPE_ID_SHIFT;

    byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], firmwareSemverMajor);
    byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], firmwareSemverMinor);
    byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], firmwareSemverPatch);
    for(i = 0U; i < ADI_ADRV903X_VERSION_PREREL_SIZE; i++)
    {
        byteCount += adrv903x_insert8Bits(&binaryRead[byteCount], firmwareVersionPrerel[i]);
    }
    byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], firmwareSemverBuild);
    byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], firmwareId);

    /* Publish firmware record to file */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    recordCrc = adrv903x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_TRUE);

    /* Append CRC32 and padding*/
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    recordPadding = (recordLength % 8U) ? 8U - (recordLength % 8U) : 0U; 
    adrv903x_insert32Bits(&binaryRead[recordPadding], recordCrc);
    byteCount = 8U + recordPadding;
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }

    /* Loop through the radio CPU RAM dump table a write memory to file */
    cpuTableSize = sizeof(adrv903x_CpuRamMemDumpTable) / sizeof(adrv903x_CpuRamMemDump_t);
    if(ramOnlyFlag == 1U && cpuTableSize > ADI_ADRV903X_VRAM_ONLY_SIZE)
    {
        cpuTableSize = ADI_ADRV903X_VRAM_ONLY_SIZE;
    }

    for (tableIdx = 0U; tableIdx < cpuTableSize; tableIdx++)
    {

        startAddr = adrv903x_CpuRamMemDumpTable[tableIdx].cpuMemAddr;
        endAddr = adrv903x_CpuRamMemDumpTable[tableIdx].cpuMemAddr + adrv903x_CpuRamMemDumpTable[tableIdx].cpuMemSize - 1;

        /* Generate CPU RAM Record */
        recordCrc = 0U;
        byteCount = 0U;
        ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */

        /* Insert basic record header */
        recordType = ADI_ADRV903X_MEMDUMP_CPU_RAM_RECORD;
        recordLength = ADI_ADRV903X_MEMDUMP_CPU_RAM_HEADER_SIZE + adrv903x_CpuRamMemDumpTable[tableIdx].cpuMemSize;
        byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], recordType);
        byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], recordLength);
        if (ADI_LIBRARY_FWRITE(binaryRead,
            1,
            byteCount,
            ofp) != byteCount)
        {
            recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
            goto cleanup;
        }

        /* Reset values for CPU record header */
        byteCount = 0U;
        ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */

        /* Construct CPU RAM record header */
        startAddress    = startAddr;
        memLength       = adrv903x_CpuRamMemDumpTable[tableIdx].cpuMemSize;
        ramId           = adrv903x_CpuRamMemDumpTable[tableIdx].ramId;
        endianness      = adrv903x_CpuRamMemDumpTable[tableIdx].cpuMemEndianness;
        if(startAddr >= STREAM_CPU_RAM_ADDR)
        {
            cpuType = ADI_ADRV903X_MEMDUMP_STREAM_CPU;
        }
        else
        {
            cpuType = ADI_ADRV903X_MEMDUMP_ARM_V7;
        }
        byteCount += adrv903x_insert64Bits(&binaryRead[byteCount], startAddress);
        byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], memLength);
        byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], ramId);
        byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], cpuType);
        byteCount += adrv903x_insert8Bits (&binaryRead[byteCount], endianness);
        byteCount += 7U; /* Padding */

        /* Insert CPU RAM record header */
        if (ADI_LIBRARY_FWRITE(binaryRead,
            1,
            byteCount,
            ofp) != byteCount)
        {
            recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
            goto cleanup;
        }
        recordCrc = adrv903x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_FALSE);
        byteCount = 0U;

        recoveryAction = adrv903x_dumpMemoryRegion(  device, 
                                    &ofp,
                                    startAddr, 
                                    endAddr, 
                                    dumpSize,
                                    &recordCrc);

        if(recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write CRC to a binary file.");
            goto cleanup;
        }

        /* Append CRC32 and padding*/
        ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
        recordPadding = (recordLength % 8U) ? 8U - (recordLength % 8U) : 0U; 
        adrv903x_insert32Bits(&binaryRead[recordPadding], recordCrc);
        byteCount = 8U + recordPadding;
        if (ADI_LIBRARY_FWRITE(binaryRead,
            1,
            byteCount,
            ofp) != byteCount)
        {
            recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
            goto cleanup;
        }

    }
    byteCount = 0U;
    recordCrc = 0U;

    /* Loop through the radio cpu telemetry dump table a write memory to file */
    cpuTableSize = sizeof(adrv903x_TelemetryMemDumpTable) / sizeof(adrv903x_CpuMemDump_t);
    if(ramOnlyFlag == 0U)
    {
        for (tableIdx = 0U; tableIdx < cpuTableSize; tableIdx++)
        {

            startAddr = adrv903x_TelemetryMemDumpTable[tableIdx].cpuMemAddr;
            endAddr = adrv903x_TelemetryMemDumpTable[tableIdx].cpuMemAddr + adrv903x_TelemetryMemDumpTable[tableIdx].cpuMemSize - 1;

            /* Generate Telemetry Buffer Record */
            recordCrc = 0U;
            byteCount = 0U;
            ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */

            /* Insert basic record header */
            recordType = ADI_ADRV903X_MEMDUMP_TELEM_RECORD;
            recordLength = ADI_ADRV903X_MEMDUMP_TELEM_BUFFER_HEADER_SIZE + adrv903x_TelemetryMemDumpTable[tableIdx].cpuMemSize;
            byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], recordType);
            byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], recordLength);
            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }

            /* Reset values for telemetry buffer record header */
            byteCount = 0U;
            ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */

            /* Construct Telemetry record header*/
            bufferLength    = adrv903x_TelemetryMemDumpTable[tableIdx].cpuMemSize;
            bufferId        = ADI_ADRV903X_PRODUCT_ID_MASK;
            endianness      = adrv903x_TelemetryMemDumpTable[tableIdx].cpuMemEndianness;

            byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], bufferLength);
            byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], bufferId);
            byteCount += adrv903x_insert16Bits(&binaryRead[byteCount], formatVersion);
            byteCount += adrv903x_insert8Bits( &binaryRead[byteCount], endianness);
            byteCount += 7U; /* Padding */

            /* Insert telemetry buffer record header */
            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }
            recordCrc = adrv903x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_FALSE);
            byteCount = 0U;

            recoveryAction = adrv903x_dumpMemoryRegion(  device, 
                                        &ofp,
                                        startAddr, 
                                        endAddr, 
                                        dumpSize,
                                        &recordCrc);

            if(recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write CRC to a binary file.");
                goto cleanup;
            }
            /* Append CRC32 and padding*/
            ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
            recordPadding = (recordLength % 8U) ? 8U - (recordLength % 8U) : 0U; 
            adrv903x_insert32Bits(&binaryRead[recordPadding], recordCrc);
            byteCount = 8U + recordPadding;
            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }
        }
    }
    byteCount = 0U;
    recordCrc = 0U;

    /* Loop through the radio cpu memory dump table a write memory to file */
    cpuTableSize = sizeof(adrv903x_CpuMemDumpTable) / sizeof(adrv903x_CpuMemDump_t);
    if(ramOnlyFlag == 0U)
    {
        for (tableIdx = 0U; tableIdx < cpuTableSize; tableIdx++)
        {

            startAddr = adrv903x_CpuMemDumpTable[tableIdx].cpuMemAddr;
            endAddr = adrv903x_CpuMemDumpTable[tableIdx].cpuMemAddr + adrv903x_CpuMemDumpTable[tableIdx].cpuMemSize - 1;

            /* Generate Register Record */
            recordCrc = 0U;
            byteCount = 0U;
            ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */

            /* Insert basic record header */
            recordType   = ADI_ADRV903X_MEMDUMP_REG_RECORD;
            recordLength = ADI_ADRV903X_MEMDUMP_REGISTER_HEADER_SIZE + adrv903x_CpuMemDumpTable[tableIdx].cpuMemSize;
            byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], recordType);
            byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], recordLength);
            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }

            /* Reset values for register record header */
            byteCount = 0U;
            ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
            

            /* Construct Register record header*/
            startAddress    = adrv903x_CpuMemDumpTable[tableIdx].cpuMemAddr;
            length          = adrv903x_CpuMemDumpTable[tableIdx].cpuMemSize;
            registerWidth   = adrv903x_CpuMemDumpTable[tableIdx].cpuMemWidth;
            endianness      = adrv903x_CpuMemDumpTable[tableIdx].cpuMemEndianness;

            byteCount += adrv903x_insert64Bits(&binaryRead[byteCount], startAddress);
            byteCount += adrv903x_insert32Bits(&binaryRead[byteCount], length);
            byteCount += adrv903x_insert8Bits( &binaryRead[byteCount], registerWidth);
            byteCount += adrv903x_insert8Bits( &binaryRead[byteCount], endianness);
            byteCount += 2U; /* Padding */

            /* Insert register buffer record header */
            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }
            recordCrc = adrv903x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_FALSE);
            byteCount = 0U;

            recoveryAction = adrv903x_dumpMemoryRegion(  device, 
                                        &ofp,
                                        startAddr, 
                                        endAddr, 
                                        dumpSize,
                                        &recordCrc);

            if(recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write CRC to a binary file.");
                goto cleanup;
            }

            /* Append CRC32 and padding*/
            ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
            recordPadding = (recordLength % 8U) ? 8U - (recordLength % 8U) : 0U; 
            adrv903x_insert32Bits(&binaryRead[recordPadding], recordCrc);
            byteCount = 8U + recordPadding;
            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }
        }
    }
    byteCount = 0U;

cleanup:
    /*Close ARM binary file*/
    if (ofp != NULL)
    {
        if (ADI_LIBRARY_FCLOSE(ofp) < 0)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to close a binary file. Possible memory shortage while flushing / other I/O errors.");
        }
    }
    
    /* Disable the Tx slice clock of the uninitialized TX channels to retrieve the normal status before memory dump */
    sliceClkEn = 0x00U;
    for (txChanId = 0U; txChanId <= ADI_ADRV903X_TX_CHAN_ID_MAX; txChanId++)
    {
        if (((device->devStateInfo.initializedChannels >> ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET) & (0x01 << (uint32_t)txChanId)) == 0U)
        {
            adrv903x_TxDig_TxAttenClkEnable_BfSet(device, NULL, txDigBaseAddr[txChanId], sliceClkEn);
        }
    }

    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}
static uint32_t adrv903x_insert64Bits(  uint8_t*            array,
                                        uint64_t            storeVariable)
{
    if(array == NULL)
    {
        return 0U;                                          /* Return number of Bytes stored */
    }
    *(array++) = storeVariable & 0xFFU;
    *(array++) = (storeVariable >> 8)  & 0xFFU;
    *(array++) = (storeVariable >> 16) & 0xFFU;
    *(array++) = (storeVariable >> 24) & 0xFFU;
    *(array++) = (storeVariable >> 32) & 0xFFU;
    *(array++) = (storeVariable >> 40) & 0xFFU;
    *(array++) = (storeVariable >> 48) & 0xFFU;
    *(array)   = (storeVariable >> 56) & 0xFFU;
    return 8U;                                              /* Return number of Bytes stored */
} 
static uint32_t adrv903x_insert32Bits(  uint8_t*            array,
                                        uint32_t            storeVariable)
{
    if(array == NULL)
    {
        return 0U;                                          /* Return number of Bytes stored */
    }
    *(array++) = storeVariable & 0xFFU;
    *(array++) = (storeVariable >> 8)  & 0xFFU;
    *(array++) = (storeVariable >> 16) & 0xFFU;
    *(array)   = (storeVariable >> 24) & 0xFFU;
    return 4U;
}
static uint32_t adrv903x_insert16Bits(  uint8_t*            array,
                                        uint16_t            storeVariable)
{
    if(array == NULL)
    {
        return 0U;                                          /* Return number of Bytes stored */
    }
    *(array++) = storeVariable & 0xFFU;
    *(array)   = (storeVariable >> 8)  & 0xFFU;
    return 2U;                                              /* Return number of Bytes stored */
}
static uint32_t adrv903x_insert8Bits(   uint8_t*            array,
                                        uint8_t            storeVariable)
{
    if(array == NULL)
    {
        return 0U;                                          /* Return number of Bytes stored */
    }
    *(array) = storeVariable & 0xFFU;
    return 1U;                                              /* Return number of Bytes stored */
}

static adi_adrv903x_ErrAction_e adrv903x_dumpMemoryRegion(  adi_adrv903x_Device_t* const device, 
                                                            FILE **ofp,
                                                            uint32_t startAddr, 
                                                            uint32_t endAddr, 
                                                            uint32_t* const dumpSize,
                                                            uint32_t *recordCrc)
{
    adi_adrv903x_ErrAction_e  recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t sizeFlag  = 0U;
    uint32_t offset    = 0U;
    uint32_t byteCount = 0U;
    uint32_t read32    = 0U;
    uint32_t i         = 0U;
    uint8_t  skipRead  = ADI_FALSE;
    uint8_t  binaryRead[ADI_ADRV903X_MEM_DUMP_CHUNK_SIZE + 10] = { 0U };
    const uint32_t cpuCrashAddr[] = { 0U }; /* Skip these addresses that causes CPU crash  */
    const uint32_t              SPI_ONLY_REGS_ADDR                                = 0x47000000U;
    const uint32_t              DIRECT_SPI_REGION_LEN                             = 0x4000U;
    const uint32_t              HARDWARE_SEMAPHORE_ADDR                           = 0x46500000U;
    const uint32_t              HARDWARE_SEMAPHORE_LEN                            = 0x200000U;

    if (startAddr == 0)
    {
        return ADI_ADRV903X_ERR_ACT_NONE; /* last entry */
    }

    /* if the cpuMemSize = 1, set the sizeFlag as 1 to ensure enter the below loop  */
    if (startAddr == endAddr)
    {
        sizeFlag = 1U;
    }

    for (offset = startAddr; ((offset < endAddr) || (1 == sizeFlag)); offset += ADI_ADRV903X_MEM_DUMP_CHUNK_SIZE)
    {
        /* Clean the flag for waiting for next cpuMemSize = 1 */
        sizeFlag = 0U;
        if (offset < (endAddr - ADI_ADRV903X_MEM_DUMP_CHUNK_SIZE))
        {
            byteCount = ADI_ADRV903X_MEM_DUMP_CHUNK_SIZE;
        }
        else
        {
            byteCount = endAddr + 1 - offset;
        }

        *dumpSize += byteCount;


        skipRead = ADI_FALSE;
        ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));

        for (i = 0U; i < sizeof(cpuCrashAddr) / sizeof(uint32_t); i++)
        {
            if (offset == cpuCrashAddr[i])
            {
                skipRead = ADI_TRUE;
                break;
            }
        }
        
        if (skipRead == ADI_FALSE)
        {

            /* DIRECT READ Byte */
            if ((offset >= SPI_ONLY_REGS_ADDR) &&
                (offset < (SPI_ONLY_REGS_ADDR + DIRECT_SPI_REGION_LEN)))
            {
                for (i = 0; i < byteCount; i++)
                {
                    recoveryAction = adi_adrv903x_Register32Read(device, NULL, offset + i, &read32, 0xFF);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset + i, "Unable to Read Cpu memory");
                        return recoveryAction;
                    }
                    binaryRead[i] = (uint8_t)read32;
                }
            }
            else
            {
                recoveryAction = adi_adrv903x_Registers32Read(device, NULL, offset, (uint32_t *)binaryRead, NULL, (byteCount + 3) / 4);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Unable to Read Cpu memory");
                    return recoveryAction;
                }
                /*
                    * Hardware semaphore register check and reset operation:
                    * Hardware semaphore register will be set to 0x00 on read,
                    * The value will be set to all 1s on read if current value is all 0s,
                    * So need to retrieve the value after reading it in the CpuMemDump
                    */
                if (((offset + byteCount) < (HARDWARE_SEMAPHORE_ADDR + HARDWARE_SEMAPHORE_LEN)) && 
                    (offset >= HARDWARE_SEMAPHORE_ADDR))
                {
                    recoveryAction = adi_adrv903x_Registers32Write(device, NULL, &offset, (uint32_t *)binaryRead, NULL, (byteCount + 3) / 4);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Unable to re-write the hardware semaphore");
                        return recoveryAction;
                    }
                }
            }
        } 
        
        if (ADI_LIBRARY_FWRITE(binaryRead,
            1,
            byteCount,
            *ofp) != byteCount)
        {
            recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
            return recoveryAction;
        }
        /* Add the CRC value at the end of Radio side memory dump */
        if  (offset >= endAddr - ADI_ADRV903X_MEM_DUMP_CHUNK_SIZE)
        {
            *recordCrc  = adrv903x_Crc32ForChunk(binaryRead, byteCount, *recordCrc, ADI_TRUE);
        } 
        else 
        {
            *recordCrc  = adrv903x_Crc32ForChunk(binaryRead, byteCount, *recordCrc, ADI_FALSE);
        }
    }
    return ADI_ADRV903X_ERR_ACT_NONE;
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_CpuMemDump(adi_adrv903x_Device_t* const                       device,
                                                         const adi_adrv903x_CpuMemDumpBinaryInfo_t* const   cpuMemDumpBinaryInfoPtr,
                                                         const uint8_t                                      forceException,
                                                         uint32_t* const                                    dumpSize)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, cpuMemDumpBinaryInfoPtr, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, dumpSize, cleanup);


    recoveryAction = adrv903x_CpuMemDumpBinWrite(device, cpuMemDumpBinaryInfoPtr, forceException, dumpSize, 0U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }


cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_CpuMemDump_vRamOnly(adi_adrv903x_Device_t* const                      device,
                                                                  const adi_adrv903x_CpuMemDumpBinaryInfo_t* const  cpuMemDumpBinaryInfoPtr,
                                                                  const uint8_t                                     forceException,
                                                                  uint32_t* const                                   dumpSize)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, cpuMemDumpBinaryInfoPtr, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, dumpSize, cleanup);


    recoveryAction = adrv903x_CpuMemDumpBinWrite(device, cpuMemDumpBinaryInfoPtr, forceException, dumpSize, 1U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }


cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairFileSave(adi_adrv903x_Device_t* const                     device,
                                                                const adi_adrv903x_JrxRepairBinaryInfo_t * const fileInfo,
                                                                adi_adrv903x_JrxRepairHistory_t* const           repairHistory)
{
        adi_adrv903x_ErrAction_e recoveryAction    = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    FILE*                    repairHistoryFile = NULL;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, repairHistory, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, fileInfo, cleanup);

    if (ADI_LIBRARY_STRNLEN((const char*)fileInfo->filePath, ADI_ADRV903X_MAX_FILE_LENGTH) == ADI_ADRV903X_MAX_FILE_LENGTH)
    {
        /* File path is not terminated */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileInfo->filePath, "Unterminated path string");
        goto cleanup;
    }

    /* Open JrxRepair History file */
#ifdef __GNUC__
    repairHistoryFile = ADI_LIBRARY_FOPEN((const char *)fileInfo->filePath, "w");
#else
    if (ADI_LIBRARY_FOPEN_S(&repairHistoryFile, (const char *)fileInfo->filePath, "w") != 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileInfo->filePath, "Invalid File or Path Detected");
        goto cleanup;
    }
#endif
    if (NULL == repairHistoryFile)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileInfo->filePath, "Invalid Binary File or Path Detected");
        goto cleanup;
    }

    /* Write to File */
    if (-1 < ADI_LIBRARY_FPRINTF(repairHistoryFile, "%016hi%08hhu%08hhu%08hhu\n", repairHistory->lastTemp, repairHistory->badLaneMask, repairHistory->weakLaneMask, repairHistory->goodLaneMask))
    {
        if (0 != ADI_LIBRARY_FFLUSH(repairHistoryFile))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileInfo->filePath, "Error writing to repair history file");
        }
        else
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        }
    }
    else
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
    }

cleanup:
    /* Close JrxRepair History File */
    if (0 == ADI_LIBRARY_FCLOSE(repairHistoryFile))
    {
        repairHistoryFile = NULL;
    }
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairFileLoad(adi_adrv903x_Device_t* const                     device,
                                                                const adi_adrv903x_JrxRepairBinaryInfo_t* const  fileInfo,
                                                                adi_adrv903x_JrxRepairHistory_t* const           repairHistory)
{
        const uint32_t                  expectedFileSize  = 41U;

    adi_adrv903x_JrxRepairHistory_t repairHistoryRd;
    adi_adrv903x_ErrAction_e        recoveryAction    = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    FILE*                           repairHistoryFile = NULL;
    uint32_t                        fileSize          = 0U;
    int16_t                         lastTempConvert   = 0;  /* Variable used to prevent truncation of 16 bit signed when parsing */
    char                            fileBuf[expectedFileSize];

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, repairHistory, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, fileInfo, cleanup);

    if (ADI_LIBRARY_STRNLEN((const char*)fileInfo->filePath, ADI_ADRV903X_MAX_FILE_LENGTH) == ADI_ADRV903X_MAX_FILE_LENGTH)
    {
        /* Path is not terminated */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileInfo->filePath, "Unterminated path string");
        goto cleanup;
    }

    /* Open JrxRepair History file */
#ifdef __GNUC__
    repairHistoryFile = ADI_LIBRARY_FOPEN((const char *)fileInfo->filePath, "rb");
#else
    if (ADI_LIBRARY_FOPEN_S(&repairHistoryFile, (const char *)fileInfo->filePath, "rb") != 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, filePath, "Invalid File or Path Detected");
        goto cleanup;
    }
#endif
    if (NULL == repairHistoryFile)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileInfo->filePath, "Invalid Binary File or Path Detected");
        goto cleanup;
    }

    /* Determine file size */
    if (ADI_LIBRARY_FSEEK(repairHistoryFile, 0, SEEK_END) != 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_INTERFACE;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileInfo->filePath, "Seek to EOF Failed");
        goto cleanup;
    }

    /* ADI_LIBRARY_FTELL returns long type */
    fileSize = (uint32_t) ADI_LIBRARY_FTELL(repairHistoryFile);

    /* Check if File is Empty */
    if (fileSize == 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, repairHistoryFile, "Zero Length File Detected");
        goto cleanup;
    }

    /* Verify file contains at least the archive header */
    if (fileSize < sizeof(adi_adrv903x_JrxRepairHistory_t))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileInfo->filePath, "Invalid file detected (very small)");
        goto cleanup;
    }

    /* Rewind the file pointer to beginning of the file */
    if (ADI_LIBRARY_FSEEK(repairHistoryFile, 0, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileInfo->filePath, "Unable to Rewind File Pointer");
        goto cleanup;
    }

    ADI_LIBRARY_FREAD(&fileBuf, 1U, expectedFileSize, repairHistoryFile);
    fileBuf[expectedFileSize - 1U] = '\0';

    if (4U != sscanf(fileBuf, "%16d%8hhu%8hhu%8hhu", (int*)&lastTempConvert, (uint8_t*)&repairHistoryRd.badLaneMask, (uint8_t*)&repairHistoryRd.weakLaneMask, (uint8_t*)&repairHistoryRd.goodLaneMask))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_FEATURE;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileInfo->filePath, "Error reading repairHistory from file");
        goto cleanup;
    }

    repairHistoryRd.lastTemp = lastTempConvert;

    if (NULL != ADI_LIBRARY_MEMCPY(repairHistory, &repairHistoryRd, sizeof(adi_adrv903x_JrxRepairHistory_t)))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
    }

cleanup:
    if (repairHistoryFile != NULL)
    {
        /* Close repairHistory File */
        if (0 == ADI_LIBRARY_FCLOSE(repairHistoryFile))
        {
            repairHistoryFile = NULL;
        }
    }
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairHistoryCheck(adi_adrv903x_Device_t* const     device,
                                                                    adi_adrv903x_JrxRepair_t* const  jrxRepair,
                                                                    int16_t* const                   checkTemp)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_adrv903x_DevTempData_t deviceTemperature;

    uint8_t screenID = 0U;
    uint8_t usedDeserLanes = 0U;

    uint32_t index = 0U;
    uint32_t localCheckResult = 0x00U;

    int16_t localCheckTemperature = 0;

    static const uint8_t SERDES_TEMP_INDEX = 11U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, jrxRepair, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, checkTemp, cleanup);

    /* check CM screen ID */
    recoveryAction = adrv903x_JrxRepairScreenTestChecker(device, &screenID);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to find screenID for part.");
        goto cleanup;
    }
    /* check temperature to see if the problem is caused by the VCM */
    recoveryAction = adi_adrv903x_TemperatureGet(device, 0U, &deviceTemperature);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get the temperature of the part.");
        goto cleanup;
    }

    localCheckTemperature = deviceTemperature.tempDegreesCelsius[SERDES_TEMP_INDEX];
    if (screenID  == 0U)
    {
        /* check if we already applied a self repair */
        if ((device->devStateInfo.devState & ADI_ADRV903X_STATE_JRXREPAIRED) == ADI_ADRV903X_STATE_JRXREPAIRED)
        {
            /* If the conditions have changed then we need to rerun the repair for bad lanes */
            localCheckResult = (uint32_t)(ADI_ADRV903X_JRX_REPAIR_SWC_ALREADY_ENABLED | ADI_ADRV903X_JRX_REPAIR_UNKNOWN_ERROR);
        }
        else
        {
            /* first time checking the for History */
            if ((device->devStateInfo.devState & ADI_ADRV903X_STATE_INITCALS_RUN) != ADI_ADRV903X_STATE_INITCALS_RUN)
            {
                /* if Post MCS Init is not complete, just load the history */
                localCheckResult = (uint32_t)(ADI_ADRV903X_JRX_REPAIR_LOAD_HISTORY);
            }
            else
            {
                /* Not at start-up, assume history.lastTemp is initialized */
                if (localCheckTemperature < (jrxRepair->history.lastTemp + ADI_ADRV903X_FACTORY_TEMPERATURE_MARGIN))
                {
                    for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DEFRAMERS; index++)
                    {
                        usedDeserLanes |= device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled;
                    }

                    localCheckResult = (uint32_t)(ADI_ADRV903X_JRX_REPAIR_FAULTY_VCM_AMP);
                    jrxRepair->history.badLaneMask  = 0x00U;
                    jrxRepair->history.goodLaneMask = usedDeserLanes; /* default all good */
                    jrxRepair->history.weakLaneMask = 0x00U;
                }
                else
                {
                    /* temperature not matching the failure mechanism, the errors are not due to VCM */
                    localCheckResult =  (uint32_t)(ADI_ADRV903X_JRX_REPAIR_TEMP_GT_LAST | ADI_ADRV903X_JRX_REPAIR_UNKNOWN_ERROR);
                }
            }
        }
    }
    else
    {
        /* Part is screened, no need to check for VCM issue */
        localCheckResult =  (uint32_t)(ADI_ADRV903X_JRX_REPAIR_SCREENED | ADI_ADRV903X_JRX_REPAIR_UNKNOWN_ERROR);
    }

cleanup:
    if (recoveryAction == ADI_ADRV903X_ERR_ACT_NONE)
    {
        *checkTemp = localCheckTemperature;
        jrxRepair->historyCheck = (localCheckResult | (uint32_t)(ADI_ADRV903X_JRX_REPAIR_CHECKED));
    }

    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairEnter(adi_adrv903x_Device_t* const    device,
                                                             adi_adrv903x_JrxRepair_t* const jrxRepair)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t localTcEnabledLanes = 0U;
    uint8_t trackingCalActive = 0U;
    uint32_t index = 0U;
    uint32_t timeElapsedUs = 0U;

    adi_adrv903x_ChannelTrackingCals_t chanMaskv2;
    adi_adrv903x_TrackingCalState_t trackingCalState;
    adi_adrv903x_TrackingCalEnableMasks_t tcEnableMask;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, jrxRepair, cleanup);

    /* Store Enabled Lanes for tracking calibration */
    recoveryAction = adi_adrv903x_TrackingCalsEnableGet(device, &tcEnableMask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while storing enabled tracking cals.");
        goto cleanup;
    }

    for (index = 0U; index < ADI_ADRV903X_NUM_TRACKING_CAL_CHANNELS; index++)
    {
        if ((tcEnableMask.enableMask[index] & ADI_ADRV903X_TC_TX_SERDES_MASK) != 0x0U)
        {
            localTcEnabledLanes |= (1U << index);
        }
    }

    if (localTcEnabledLanes != 0U)
    {
        /* Disable Serdes Tracking Cal */
        chanMaskv2.rxChannel  = 0x00U;
        chanMaskv2.txChannel  = 0x00U;
        chanMaskv2.orxChannel = 0x00U;
        chanMaskv2.laneSerdes = localTcEnabledLanes;
        recoveryAction = adi_adrv903x_TrackingCalsEnableSet_v2(device, ADI_ADRV903X_TC_TX_SERDES_MASK, &chanMaskv2, ADI_ADRV903X_TRACKING_CAL_DISABLE);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Disable Serdes Tracking Calibration");
            goto cleanup;
        }

        /* Poll tracking cals state to be inactive */
        for (timeElapsedUs = 0U; timeElapsedUs < ADI_ADRV903X_TRACKCALDISABLE_TIMEOUT_US; timeElapsedUs += ADI_ADRV903X_TRACKCALDISABLE_INTERVAL_US)
        {
            trackingCalActive = ADI_DISABLE;
            /* Retrieve Tracking Cals state */
            recoveryAction = adi_adrv903x_TrackingCalAllStateGet(device, &trackingCalState);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting Tracking Cals State");
                goto cleanup;
            }

            /* check if serdes tracking cal is inactive */
            for (index = 0U; index < ADI_ADRV903X_NUM_TRACKING_CAL_CHANNELS; index++)
            {
                if (((localTcEnabledLanes & (1U << index)) != 0U) &&
                    ((trackingCalState.calState[index][ADI_ADRV903X_TC_TX_SERDES] & ADI_ADRV903X_TC_STATE_INACTIVE) != ADI_ADRV903X_TC_STATE_INACTIVE))
                {
                    trackingCalActive = ADI_ENABLE;
                    break;
                }
            }

            /* Break out here if serdes tracking cals is Inactive */
            if (trackingCalActive == ADI_DISABLE)
            {
                break;
            }

            /* Serdes tracking cals is still running. Wait the specified wait interval, then check again for inactivity. */
            recoveryAction = (adi_adrv903x_ErrAction_e) adi_common_hal_Wait_us(&device->common, ADI_ADRV903X_TRACKCALDISABLE_INTERVAL_US);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Issue");
                goto cleanup;
            }
        }

        /* Check for Tracking Cal Inactive timeout */
        if (timeElapsedUs >= ADI_ADRV903X_TRACKCALDISABLE_TIMEOUT_US)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_RESET_FEATURE;
            ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_TIMEOUT, recoveryAction, timeElapsedUs, "Serdes Tracking Cal Inactive Timeout");
            goto cleanup;
        }
    }

    /* enable PRBS if required */
    if (jrxRepair->dfrmPrbsCfg.polyOrder != ADI_ADRV903X_PRBS_DISABLE)
    {
        recoveryAction = adi_adrv903x_DfrmPrbsCheckerStateSet(device, &jrxRepair->dfrmPrbsCfg);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set the PRBS mode");
            goto cleanup;
        }
    }

    jrxRepair->tcEnabledLaneMask = localTcEnabledLanes;

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairExit(adi_adrv903x_Device_t* const    device,
                                                            adi_adrv903x_JrxRepair_t* const jrxRepair)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_adrv903x_DfrmPrbsCfg_t dfrmPrbsCfg;
    adi_adrv903x_ChannelTrackingCals_t chanMaskv2;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    /* disable PRBS if enabled */
    recoveryAction = adi_adrv903x_DfrmPrbsCheckerStateGet(device, &dfrmPrbsCfg);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get the PRBS mode");
        goto cleanup;
    }

    if (dfrmPrbsCfg.polyOrder != ADI_ADRV903X_PRBS_DISABLE)
    {
        dfrmPrbsCfg.polyOrder = ADI_ADRV903X_PRBS_DISABLE;
        recoveryAction = adi_adrv903x_DfrmPrbsCheckerStateSet(device, &dfrmPrbsCfg);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set the PRBS mode");
            goto cleanup;
        }
    }
    else
    {
        /* Re-enable tracking cals if it was enabled previously */
        if (jrxRepair->tcEnabledLaneMask != 0U)
        {
            chanMaskv2.rxChannel  = 0x00U;
            chanMaskv2.txChannel  = 0x00U;
            chanMaskv2.orxChannel = 0x00U;
            chanMaskv2.laneSerdes = jrxRepair->tcEnabledLaneMask;
            recoveryAction = adi_adrv903x_TrackingCalsEnableSet_v2(device, ADI_ADRV903X_TC_TX_SERDES_MASK, &chanMaskv2, ADI_ADRV903X_TRACKING_CAL_ENABLE);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Enable Serdes Tracking Calibration");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairBiasSurvey(adi_adrv903x_Device_t* const              device,
                                                                  adi_adrv903x_JrxRepairHistory_t* const    repairHistory,
                                                                  adi_adrv903x_JrxRepairBiasSurvey_t* const biasSurvey)
{
        static const uint8_t  MAX_SCORE  = (1U << ADI_ADRV903X_JRX_REPAIR_BIAS_NUM) - 1U;

    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t  ibiasFix        = 0U;
    uint8_t  ibiasFixDefault = 0U;
    uint8_t  usedDeserLanes  = 0U;
    uint8_t  calSincePowerUp = 0U;
    uint8_t  laneSel         = 0U;
    uint8_t  goodLanes       = 0U;
    uint8_t  badLanes        = 0U;
    uint8_t  weakLanes       = 0U;
    uint8_t  badScore        = 0U;
    uint32_t index           = 0U;
    uint32_t laneIdx         = 0U;

    uint8_t  laneScore[ADI_ADRV903X_MAX_DESERIALIZER_LANES];
    ADI_PLATFORM_LARGE_VAR_ALLOC(adi_adrv903x_InitCalErrData_t, initCalErrDataPtr);

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, initCalErrDataPtr, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, repairHistory, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, biasSurvey, cleanup);


    recoveryAction = adrv903x_JrxRepairHistoryRangeCheck(device, repairHistory);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid Jrx Repair History.");
        goto cleanup;
    }

    ADI_LIBRARY_MEMSET(biasSurvey, 0, sizeof(adi_adrv903x_JrxRepairBiasSurvey_t));

    for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DEFRAMERS; index++)
    {
        usedDeserLanes |= device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled;
    }

    recoveryAction = adrv903x_SerdesRxdig8packRegmapCore1p2_CtrlIbiasFix_BfGet(device,
                                                                               NULL,
                                                                               ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_TOP_8PACK,
                                                                               &ibiasFixDefault);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Get Default bias setting");
        goto cleanup;
    }

    if (ibiasFixDefault == ADI_ADRV903X_JRX_REPAIR_MAX_BIAS)
    {
        recoveryAction = adi_adrv903x_InitCalsDetailedStatusGet_v2(device, initCalErrDataPtr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Get Initcal Status");
            goto cleanup;
        }
        for (laneIdx = 0U; laneIdx < ADI_ADRV903X_MAX_DESERIALIZER_LANES; laneIdx++)
        {
            if (((usedDeserLanes & (1 << laneIdx)) != 0U) &&
                ((initCalErrDataPtr->channel[laneIdx].calsSincePowerUp & ADI_ADRV903X_IC_SERDES) == ADI_ADRV903X_IC_SERDES))
            {
                calSincePowerUp |= (1 << laneIdx);
            }
        }
    }

    badScore = (1U << (ibiasFixDefault - ADI_ADRV903X_JRX_REPAIR_MIN_BIAS)) - 1U;

    for (ibiasFix = (uint8_t)ADI_ADRV903X_JRX_REPAIR_MAX_BIAS, index = (ADI_ADRV903X_JRX_REPAIR_BIAS_NUM - 1U);
         ibiasFix >= (uint8_t)ADI_ADRV903X_JRX_REPAIR_MIN_BIAS;
         ibiasFix--, index--)
    {
        /* skip fast attack for the first run if nominal bias is maximum bias */
        if ((ibiasFix != ibiasFixDefault) || (ibiasFix != ADI_ADRV903X_JRX_REPAIR_MAX_BIAS) || (calSincePowerUp != usedDeserLanes))
        {
            recoveryAction = adrv903x_SerdesRxdig8packRegmapCore1p2_CtrlIbiasFix_BfSet(device,
                                                                                       NULL,
                                                                                       ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_TOP_8PACK,
                                                                                       ibiasFix);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set new bias setting");
                goto cleanup;
            }

            recoveryAction = adi_adrv903x_JrxRepairFastAttackRun(device, initCalErrDataPtr);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Run Serdes Fast Attack Initcal");
                goto cleanup;
            }
        }

        /* Get Errors with JrxRepairTest */
        recoveryAction = adi_adrv903x_JrxRepairTest(device, &(biasSurvey->biasTests[index]));
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, ibiasFix, "Failed to test new Bias settings");
            goto cleanup;
        }

    }

    recoveryAction = adrv903x_SerdesRxdig8packRegmapCore1p2_CtrlIbiasFix_BfSet(device,
                                                                               NULL,
                                                                               ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_TOP_8PACK,
                                                                               ibiasFixDefault);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Set Default bias setting");
        goto cleanup;
    }

    /* evaluate bias survey */
    for (laneIdx = 0U; laneIdx < ADI_ADRV903X_MAX_DESERIALIZER_LANES; laneIdx++)
    {
        laneScore[laneIdx] = MAX_SCORE;
        for (index = 0U; index < ADI_ADRV903X_JRX_REPAIR_BIAS_NUM; index++)
        {
            if (biasSurvey->biasTests[index].laneErrors[laneIdx] != 0U)
            {
                laneScore[laneIdx] &= ~(1U << index);
            }
        }
    }

    /* update history */
    for (laneIdx = 0U; laneIdx < ADI_ADRV903X_MAX_DESERIALIZER_LANES; laneIdx++)
    {
        laneSel = (1U << laneIdx);
        if ((usedDeserLanes & laneSel) == 0U)
        {
            continue;
        }

        if (laneScore[laneIdx] == MAX_SCORE)
        {
            goodLanes |= laneSel;
        }
        else if (laneScore[laneIdx] <= badScore)
        {
            badLanes |= laneSel;
        }
        else
        {
            weakLanes |= laneSel;
        }
    }

    /* update history */
    repairHistory->goodLaneMask &= goodLanes;
    repairHistory->badLaneMask  |= badLanes;
    repairHistory->weakLaneMask  = weakLanes;

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairFastAttackRun(adi_adrv903x_Device_t* const          device,
                                                                     adi_adrv903x_InitCalErrData_t* const  initCalErrData)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    uint8_t  usedDeserLanes = 0U;
    uint32_t index = 0U;

    adrv903x_CpuCmd_EnFastAttack_t enFastAttackCmd;
    adrv903x_CpuCmd_EnFastAttackResp_t enFastAttackCmdRsp;
    adi_adrv903x_InitCals_t initCalCfg;

    static const uint32_t INIT_CALS_TIMEOUT_MS = 10000U; /* 10 Seconds Timeout */
    uint32_t init_cals_timeout_ms = INIT_CALS_TIMEOUT_MS;
        ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, initCalErrData, cleanup);

        /* Enable Fast Attack mode for Serdes initcal */
    enFastAttackCmd.calMask = ADRV903X_HTOCL(ADI_ADRV903X_TC_TX_SERDES_MASK);

    for (index = 0U; index < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; index++)
    {
        recoveryAction = adrv903x_CpuCmdSend(device,
                                             (adi_adrv903x_CpuType_e)index,
                                             ADRV903X_LINK_ID_0,
                                             ADRV903X_CPU_CMD_ID_ENABLE_FAST_ATTACK,
                                             (void*)&enFastAttackCmd,
                                             sizeof(adrv903x_CpuCmd_EnFastAttack_t),
                                             (void*)&enFastAttackCmdRsp,
                                             sizeof(adrv903x_CpuCmd_EnFastAttackResp_t),
                                             NULL);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Failed to set Serdes InitCal in Fast attack mode");
            goto cleanup;
        }
    }

    for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DEFRAMERS; index++)
    {
        usedDeserLanes |= device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled;
    }

    /* Run Serdes Initcal */
    initCalCfg.calMask        = ADI_ADRV903X_IC_SERDES;
    initCalCfg.txChannelMask  = 0x00U;
    initCalCfg.orxChannelMask = 0x00U;
    initCalCfg.rxChannelMask  = usedDeserLanes;
    initCalCfg.warmBoot       = 0U;

    recoveryAction = adi_adrv903x_InitCalsRun(device, &initCalCfg);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "InitCalsRun Failed");
        goto cleanup;
    }

    recoveryAction = adi_adrv903x_InitCalsWait_v2(device, init_cals_timeout_ms, initCalErrData);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Initial Calibration Wait Issue");
        goto cleanup;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairTest(adi_adrv903x_Device_t* const        device,
                                                            adi_adrv903x_JrxRepairTest_t* const testResult)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;


    typedef struct
    {
        uint8_t dfrmIdx;
        uint8_t logicLaneId;
    } JrxLinkLogicLaneInfo_t;

    JrxLinkLogicLaneInfo_t  logicLaneInfo[ADI_ADRV903X_MAX_DESERIALIZER_LANES];

    uint8_t  pclkErrClear[ADI_ADRV903X_MAX_DEFRAMERS] = { 0U };

    uint8_t  pclkSlowErr = 0U;
    uint8_t  pclkFastErr = 0U;


    uint8_t  linkType         = 0U;
    uint8_t  laneId           = 0U;
    uint8_t  bfValue          = 0U;
    uint8_t  usedDeserLanes   = 0U;

    uint32_t usedDfrmBaseAddr = ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    uint32_t index            = 0U;

    adi_adrv903x_DeframerSel_e usedDfrm = ADI_ADRV903X_DEFRAMER_0;
    adi_adrv903x_DfrmPrbsCfg_t dfrmPrbsCfg;

    adi_adrv903x_Dfrm204cErrCounterStatus_t dfrm204cErrCnt;
    adi_adrv903x_DfrmErrCounterStatus_t     dfrm204bErrCnt;
    adi_adrv903x_DfrmPrbsErrCounters_t      dfrmPrbsErrCnt;

    static const uint8_t  JRX204C_STATE_FEC_READY = 6U;
    static const uint32_t JRX_LINK_BASE_ADDR[ADI_ADRV903X_MAX_DEFRAMERS] =
    {
        ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_,
        ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_
    };

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, testResult, cleanup);

    for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DEFRAMERS; index++)
    {
        if (device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled != 0)
        {
            usedDfrm = (adi_adrv903x_DeframerSel_e)(1U << index);
            usedDfrmBaseAddr = JRX_LINK_BASE_ADDR[index];
        }
        usedDeserLanes |= device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled;
    }

    /* Mode: PRBS, 204C or 204B */
    recoveryAction = adi_adrv903x_DfrmPrbsCheckerStateGet(device, &dfrmPrbsCfg);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get the PRBS mode");
        goto cleanup;
    }

    /* Get the Deframer link type */
    recoveryAction = adrv903x_JrxLink_JrxLinkType_BfGet(device,
                                                        NULL,
                                                        (adrv903x_BfJrxLinkChanAddr_e)usedDfrmBaseAddr,
                                                        &linkType);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link type for Deframer");
        goto cleanup;
    }

    /* Init/Reset Error Counters and Link Layer Error Status */
    if (dfrmPrbsCfg.polyOrder == ADI_ADRV903X_PRBS_DISABLE)
    {

        /* Init Logic Lane Info */
        for (laneId = 0U; laneId < (uint8_t)ADI_ADRV903X_MAX_DESERIALIZER_LANES; laneId++)
        {
            logicLaneInfo[laneId].dfrmIdx     = 0U;
            logicLaneInfo[laneId].logicLaneId = (uint8_t)ADI_ADRV903X_MAX_DESERIALIZER_LANES;
        }

        for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DEFRAMERS; index++)
        {
            if (device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled == 0U)
            {
                continue;
            }

            recoveryAction = adrv903x_JrxLink_JrxCorePclkErrorClear_BfGet(device, NULL, (adrv903x_BfJrxLinkChanAddr_e)JRX_LINK_BASE_ADDR[index], &pclkErrClear[index]);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Failed to Read Deframer PCLK Error Clear status");
                goto cleanup;
            }

            for (laneId = 0U; laneId < (uint8_t)ADI_ADRV903X_MAX_DESERIALIZER_LANES; laneId++)
            {
                recoveryAction =  adrv903x_JrxLink_JrxCoreLaneSel_BfGet(device, NULL, (adrv903x_BfJrxLinkChanAddr_e)JRX_LINK_BASE_ADDR[index], laneId, &bfValue);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, laneId, "Error while reading Jrx Link Xbar");
                    goto cleanup;
                }
                if (bfValue < ADI_ADRV903X_MAX_DESERIALIZER_LANES)
                {
                    logicLaneInfo[bfValue].logicLaneId = laneId;
                    logicLaneInfo[bfValue].dfrmIdx = index;
                }
            }
        }


        if (linkType != 0U)
        {
            /* 204C link type case */
            for (index = 0; index < ADI_ADRV903X_MAX_DEFRAMERS; index++)
            {
                if(device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled != 0U){
                    recoveryAction = adi_adrv903x_Dfrm204cErrCounterReset(device, (adi_adrv903x_DeframerSel_e)(1U << index));
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Reseting 204C Deframer Error Counters");
                        goto cleanup;
                    }
                }
            }

            recoveryAction = adrv903x_JesdCommon_JrxDl204cHoldErrCnt_BfSet(device, NULL, ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON, ADI_ENABLE);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Holding 204C Deframer Error Counters");
                goto cleanup;
            }
        }
        else
        {
            /* 204B link type case*/
            for (laneId = 0; laneId < (uint8_t)ADI_ADRV903X_MAX_DESERIALIZER_LANES; laneId++)
            {
                if ((usedDeserLanes & (1U << laneId)) != 0U)
                {
                    recoveryAction = adrv903x_JesdCommon_JrxDl204bEcntTch_BfSet(device, NULL, ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON, laneId, 0x7U);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Holding 204B Error Counters");
                        goto cleanup;
                    }
                    recoveryAction = adrv903x_JesdCommon_JrxDl204bEcntEna_BfSet(device, NULL, ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON, laneId, 0x7U);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Enabling 204B Error Counters");
                        goto cleanup;
                    }
                    /* Deframer Selection doesn't really matter */
                    recoveryAction = adi_adrv903x_DfrmErrCounterReset(device, usedDfrm, laneId, 0x7U);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Reseting 204B Deframer Error Counters");
                        goto cleanup;
                    }
                }
            }

            /* Clear 204B IRQ */
            recoveryAction = adi_adrv903x_DfrmIrqSourceReset(device);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to clear 204B IRQs");
                goto cleanup;
            }


            /* Enable PCLK Errors (if it is not enabled) in 204B case */
            for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DEFRAMERS; index++)
            {
                if ((device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled == 0U) || (pclkErrClear[index] == 0U))
                {
                    continue;
                }

                recoveryAction = adrv903x_JrxLink_JrxCorePclkErrorClear_BfSet(device, NULL, (adrv903x_BfJrxLinkChanAddr_e)JRX_LINK_BASE_ADDR[index], 0U);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Failed to Enable Deframer PCLK Error");
                    goto cleanup;
                }
            }

        }

        /* clear possible PCLK errors */
        recoveryAction = adi_adrv903x_DeframerErrorCtrl(device, ADI_ADRV903X_ALL_DEFRAMER, ADI_ADRV903X_SERDES_PCLK_ERR_CLEAR);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to clear PCLK Errors");
            goto cleanup;
        }
    }
    else
    {
        /* PRBS Checker is enabled */
        recoveryAction = adi_adrv903x_DfrmPrbsCountReset(device);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to clear PRBS counters");
            goto cleanup;
        }
    }

    /* Wait a bit */
    recoveryAction = (adi_adrv903x_ErrAction_e) adi_common_hal_Wait_us(&device->common, ADI_ADRV903X_JRXREPAIR_TEST_WAIT_US);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Issue");
        goto cleanup;
    }

    /* Read Counter */
    if (dfrmPrbsCfg.polyOrder == ADI_ADRV903X_PRBS_DISABLE)
    {
        for (laneId = 0U; laneId < ADI_ADRV903X_MAX_DESERIALIZER_LANES; laneId++)
        {
            if ((usedDeserLanes & (1U << laneId)) == 0U)
            {
                testResult->laneErrors[laneId] = 0U;
                continue;
            }

            if (linkType != 0U)
            {
                /* 204C link type case */
                recoveryAction = adrv903x_JesdCommon_JrxDl204cState_BfGet(device, NULL, ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON, laneId, &bfValue);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get 204C Lane State");
                    goto cleanup;
                }
                if (bfValue != JRX204C_STATE_FEC_READY)
                {
                    /* Lane is not in a good state */
                    testResult->laneErrors[laneId] = 0xFFFFFFFFU;
                    continue;
                }

                recoveryAction = adi_adrv903x_Dfrm204cErrCounterStatusGet(device, usedDfrm, laneId, &dfrm204cErrCnt);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get 204C Error Counters");
                    goto cleanup;
                }
                testResult->laneErrors[laneId] = (((uint32_t)dfrm204cErrCnt.shCntValue  << 24U) |
                                                  ((uint32_t)dfrm204cErrCnt.embCntValue << 16U) |
                                                  ((uint32_t)dfrm204cErrCnt.mbCntValue  <<  8U) |
                                                   (uint32_t)dfrm204cErrCnt.crcCntValue);
            }
            else
            {
                /* 204B link type case */
                recoveryAction = adrv903x_JesdCommon_JrxDl204bSyncN_BfGet(device, NULL, ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON, laneId, &bfValue);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get 204B Lane Sync N");
                    goto cleanup;
                }
                if (bfValue == ADI_DISABLE)
                {
                    /* Lane is not in a good state */
                    testResult->laneErrors[laneId] = 0xFFFFFFFFU;
                    continue;
                }
                recoveryAction = adrv903x_JesdCommon_JrxDl204bCgs_BfGet(device, NULL, ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON, laneId, &bfValue);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get 204B Lane CGS sync");
                    goto cleanup;
                }
                if (bfValue == ADI_DISABLE)
                {
                    /* Lane is not in a good state */
                    testResult->laneErrors[laneId] = 0xFFFFFFFFU;
                    continue;
                }


                if (logicLaneInfo[laneId].logicLaneId < ADI_ADRV903X_MAX_DESERIALIZER_LANES)
                {
                    /* Verify Logic Lane PCLK fast/slow errors */
                    recoveryAction = adrv903x_JrxLink_JrxCorePclkFastError_BfGet(device, NULL,
                                                                                 (adrv903x_BfJrxLinkChanAddr_e)JRX_LINK_BASE_ADDR[logicLaneInfo[laneId].dfrmIdx],
                                                                                 logicLaneInfo[laneId].logicLaneId, &pclkFastErr);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Read Logic Lane PCLK Fast Error");
                        goto cleanup;
                    }
                    recoveryAction = adrv903x_JrxLink_JrxCorePclkSlowError_BfGet(device, NULL,
                                                                                 (adrv903x_BfJrxLinkChanAddr_e)JRX_LINK_BASE_ADDR[logicLaneInfo[laneId].dfrmIdx],
                                                                                 logicLaneInfo[laneId].logicLaneId, &pclkSlowErr);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Read Logic Lane PCLK Slow Error");
                        goto cleanup;
                    }
                    if ((pclkFastErr == ADI_ENABLE) && (pclkSlowErr == ADI_ENABLE))
                    {
                        /* Lane is not in a good state */
                        testResult->laneErrors[laneId] = 0xFFFFFFFFU;
                        continue;
                    }
                }


                recoveryAction = adi_adrv903x_DfrmErrCounterStatusGet(device, usedDfrm, laneId, &dfrm204bErrCnt);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get 204B Error Counters");
                    goto cleanup;
                }
                testResult->laneErrors[laneId] = (((uint32_t)dfrm204bErrCnt.bdCntValue  << 16U) |
                                                  ((uint32_t)dfrm204bErrCnt.nitCntValue <<  8U) |
                                                   (uint32_t)dfrm204bErrCnt.uekCntValue);
            }
        }

        /* some clean up */
        if (linkType != 0U)
        {
            recoveryAction = adrv903x_JesdCommon_JrxDl204cHoldErrCnt_BfSet(device, NULL, ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON, ADI_DISABLE);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Releasing 204C Deframer Error Counters");
                goto cleanup;
            }
        }
        else
        {
            /* 204B link type case*/
            for (laneId = 0; laneId < (uint8_t)ADI_ADRV903X_MAX_DESERIALIZER_LANES; laneId++)
            {
                if ((usedDeserLanes & (1U << laneId)) != 0U)
                {
                    recoveryAction = adrv903x_JesdCommon_JrxDl204bEcntTch_BfSet(device, NULL, ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON, laneId, 0x0U);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Releasing 204B Error Counters");
                        goto cleanup;
                    }
                }
            }


            /* Restore PCLK Errors status in 204B case */
            for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DEFRAMERS; index++)
            {
                if ((device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled == 0U) || (pclkErrClear[index] == 0U))
                {
                    continue;
                }

                recoveryAction = adrv903x_JrxLink_JrxCorePclkErrorClear_BfSet(device, NULL, (adrv903x_BfJrxLinkChanAddr_e)JRX_LINK_BASE_ADDR[index], 1U);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, index, "Failed to Disable Deframer PCLK Error");
                    goto cleanup;
                }
            }

        }
    }
    else
    {
        /* PRBS Mode */
        dfrmPrbsErrCnt.sampleSource = ADI_ADRV903X_PRBSCHECK_LANEDATA;
        recoveryAction = adi_adrv903x_DfrmPrbsErrCountGet(device, &dfrmPrbsErrCnt);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get PRBS Error Counters");
            goto cleanup;
        }
        for (laneId = 0U; laneId < (uint8_t)ADI_ADRV903X_MAX_DESERIALIZER_LANES; laneId++)
        {
            if ((usedDeserLanes & (1U << laneId)) == 0U)
            {
                testResult->laneErrors[laneId] = 0U;
            }
            else
            {
                testResult->laneErrors[laneId] = dfrmPrbsErrCnt.laneErrors[laneId];
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairApply(adi_adrv903x_Device_t* const                    device,
                                                             adi_adrv903x_JrxRepairHistory_t* const          repairHistory)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    uint8_t  usedDeserLanes = 0U;
    uint8_t  useLanesFix    = 0U;
    uint32_t index          = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, repairHistory, cleanup);

    recoveryAction = adrv903x_JrxRepairHistoryRangeCheck(device, repairHistory);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid Jrx Repair History.");
        goto cleanup;
    }

    for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DEFRAMERS; index++)
    {
        usedDeserLanes |= device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled;
    }

    if ((repairHistory->badLaneMask != 0U) || (repairHistory->weakLaneMask != 0U))
    {
        useLanesFix = ADI_TRUE;
    }
    else
    {
        useLanesFix = ADI_FALSE;
    }

    /* Enable SwC */
    recoveryAction = adi_adrv903x_JrxRepairVcmLanesFix(device, usedDeserLanes, useLanesFix);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Set SwC Enabled");
        goto cleanup;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairSwCEnableGet(adi_adrv903x_Device_t* const  device,
                                                                    uint8_t* const                swcEnMask)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    uint8_t ctrlDataSet[9U];
    uint8_t ctrlDataGet[9U];
    uint8_t usedDeserLanes = 0U;
    uint8_t localSwcEnMask = 0U;
    uint8_t laneSel = 0U;

    uint32_t lengthResp = 0U;
    uint32_t timeElapsedUs = 0U;
    uint32_t index = 0U;
    uint32_t laneCnt = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, swcEnMask, cleanup);

    /* Prepare the command payload */
    ctrlDataSet[0] = 0x00U;
    ctrlDataSet[1] = (uint8_t)ADRV903X_SERDES_TEST_CMD;
    ctrlDataSet[2] = (uint8_t)ADRV903X_SERDES_TEST_SET_VCM_SWC;
    ctrlDataSet[3] = 0x01U;
    ctrlDataSet[4] = 0x00U;
    ctrlDataSet[5] = 0x00U;
    ctrlDataSet[6] = 0x00U;
    ctrlDataSet[7] = 0x00U;
    ctrlDataSet[8] = 0x00U;

    for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DEFRAMERS; index++)
    {
        usedDeserLanes |= device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled;
    }

    /* Run the command for two used lanes */
    for (index = 0U; (index < (uint32_t)ADI_ADRV903X_MAX_DESERIALIZER_LANES) && (laneCnt < 2U); index++)
    {
        laneSel = (1U << index);

        if ((usedDeserLanes & laneSel) == 0U)
        {
            continue;
        }

        laneCnt++;
        ctrlDataSet[0] = (uint8_t)index;

        /* Exec command */
        recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                        ADRV903X_CPU_OBJID_IC_SERDES,
                                                        ADRV903X_SERDES_CTRL_SET_FSM_CMD,
                                                        (adi_adrv903x_Channels_e)laneSel,
                                                        ctrlDataSet,
                                                        sizeof(ctrlDataSet),
                                                        &lengthResp,
                                                        ctrlDataGet,
                                                        sizeof(ctrlDataGet));
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to run Jrx VCM Switch C Set command");
            goto cleanup;
        }

        /* Poll Serdes Calibration Status for completion of the SwC set Command */
        for (timeElapsedUs = 0U; timeElapsedUs < ADI_ADRV903X_TRACKCALDISABLE_TIMEOUT_US; timeElapsedUs += ADI_ADRV903X_TRACKCALDISABLE_INTERVAL_US)
        {
            /* Get Serdes Calibration Status */
            recoveryAction = adi_adrv903x_CalSpecificStatusGet( device,
                                                                (adi_adrv903x_Channels_e)laneSel,
                                                                ADRV903X_CPU_OBJID_TC_SERDES,
                                                                ctrlDataGet,
                                                                sizeof(ctrlDataGet));
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get SwC set Command status failed");
                goto cleanup;
            }

            /* Break out here if SwC set Command have been completed */
            if (ctrlDataGet[1] == (uint8_t)ADRV903X_SERDES_TEST_CMD_DONE)
            {
                break;
            }

            /* SwC set Command is still in progress. Wait the specified wait interval, then check again for status. */
            recoveryAction = (adi_adrv903x_ErrAction_e) adi_common_hal_Wait_us(&device->common, ADI_ADRV903X_TRACKCALDISABLE_INTERVAL_US);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Issue");
                goto cleanup;
            }
        }

        /* Check for timeout */
        if (timeElapsedUs >= ADI_ADRV903X_TRACKCALDISABLE_TIMEOUT_US)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_RESET_FEATURE;
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV903X_ERRSRC_API,
                             ADI_COMMON_ERRCODE_TIMEOUT,
                             recoveryAction,
                             timeElapsedUs,
                             "SwC set Command Timeout");
            goto cleanup;
        }

        localSwcEnMask |= ctrlDataGet[ADRV903X_SERDES_CTRL_FSM_CMD_RSP_HDR_SIZE_BYTES];
    }

    *swcEnMask = localSwcEnMask;

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairSwCEnableSet(adi_adrv903x_Device_t* const  device,
                                                                    const uint8_t                 swcEnMask)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    uint8_t ctrlDataSet[9U];
    uint8_t ctrlDataGet[9U];
    uint8_t usedDeserLanes = 0U;
    uint8_t laneSel = 0U;

    uint32_t lengthResp = 0U;
    uint32_t timeElapsedUs = 0U;
    uint32_t index = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_LIBRARY_MEMSET(ctrlDataSet, 0U, sizeof(ctrlDataSet));
    ADI_LIBRARY_MEMSET(ctrlDataGet, 0U, sizeof(ctrlDataGet));

    for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DEFRAMERS; index++)
    {
        usedDeserLanes |= device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled;
    }

    /* get first used lane */
    for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DESERIALIZER_LANES; index++)
    {
        laneSel = (1U << index);
        if ((usedDeserLanes & laneSel) != 0U)
        {
            break;
        }
    }

    /* Prepare the command payload */
    ctrlDataSet[0] = (uint8_t)index;
    ctrlDataSet[1] = (uint8_t)ADRV903X_SERDES_TEST_CMD;
    ctrlDataSet[2] = (uint8_t)ADRV903X_SERDES_TEST_SET_VCM_SWC;
    ctrlDataSet[3] = 0x01U;
    ctrlDataSet[4] = usedDeserLanes;  
    ctrlDataSet[5] = swcEnMask;
    ctrlDataSet[6] = usedDeserLanes;  
    ctrlDataSet[7] = swcEnMask;
    ctrlDataSet[8] = (swcEnMask == 0x00U) ? 0x00U : 0x01U;

    /* Exec command */
    recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                    ADRV903X_CPU_OBJID_IC_SERDES,
                                                    ADRV903X_SERDES_CTRL_SET_FSM_CMD,
                                                    (adi_adrv903x_Channels_e)laneSel,
                                                    ctrlDataSet,
                                                    sizeof(ctrlDataSet),
                                                    &lengthResp,
                                                    ctrlDataGet,
                                                    sizeof(ctrlDataGet));
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to run Jrx VCM Switch C Set command");
        goto cleanup;
    }

    /* Poll Serdes Calibration Status for completion of the SwC set Command */
    for (timeElapsedUs = 0U; timeElapsedUs < ADI_ADRV903X_TRACKCALDISABLE_TIMEOUT_US; timeElapsedUs += ADI_ADRV903X_TRACKCALDISABLE_INTERVAL_US)
    {
        /* Get Serdes Calibration Status */
        recoveryAction = adi_adrv903x_CalSpecificStatusGet( device,
                                                            (adi_adrv903x_Channels_e)laneSel,
                                                            ADRV903X_CPU_OBJID_TC_SERDES,
                                                            ctrlDataGet,
                                                            sizeof(ctrlDataGet));
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get SwC set Command status failed");
            goto cleanup;
        }

        /* Break out here if SwC set Command have been completed */
        if (ctrlDataGet[1] == (uint8_t)ADRV903X_SERDES_TEST_CMD_DONE)
        {
            break;
        }

        /* SwC set Command is still in progress. Wait the specified wait interval, then check again for status. */
        recoveryAction = (adi_adrv903x_ErrAction_e) adi_common_hal_Wait_us(&device->common, ADI_ADRV903X_TRACKCALDISABLE_INTERVAL_US);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Issue");
            goto cleanup;
        }
    }

    /* Check for timeout */
    if (timeElapsedUs >= ADI_ADRV903X_TRACKCALDISABLE_TIMEOUT_US)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_RESET_FEATURE;
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV903X_ERRSRC_API,
                         ADI_COMMON_ERRCODE_TIMEOUT,
                         recoveryAction,
                         timeElapsedUs,
                         "SwC set Command Timeout");
        goto cleanup;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairExecute(adi_adrv903x_Device_t* const    device,
                                                               adi_adrv903x_JrxRepair_t* const jrxRepair)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    uint8_t  failingLanes = 0x00U;
    uint8_t  swcEnabled   = 0x00U;

    uint32_t index = 0U;
    int16_t  checkTemp = 0;

    adi_adrv903x_DevTempData_t deviceTemperature;
    adi_adrv903x_JrxRepairTest_t execTest;
    adi_adrv903x_JrxRepairHistory_t updatedHistory;
    ADI_PLATFORM_LARGE_VAR_ALLOC(adi_adrv903x_InitCalErrData_t, initCalErrDataPtr);

    static const uint8_t  SERDES_TEMP_INDEX = 11U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, initCalErrDataPtr, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, jrxRepair, cleanup);

    if ((jrxRepair->runMode != ADI_ADRV903X_JRX_REPAIR_NORMAL_MODE) &&
        (jrxRepair->runMode != ADI_ADRV903X_JRX_REPAIR_FAST_MODE))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, jrxRepair->runMode, "Invalid Run Mode");
        goto cleanup;
    }

    if ((jrxRepair->historyCheck & ADI_ADRV903X_JRX_REPAIR_CHECKED) == 0x0U)
    {
        /* If history unchecked verify conditions to run Jrx Repair */
        recoveryAction = adi_adrv903x_JrxRepairHistoryCheck(device, jrxRepair, &checkTemp);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "JrxRepairHistoryCheck failed");
            goto cleanup;
        }
    }
    else
    {
        /* readback temperature */
        recoveryAction = adi_adrv903x_TemperatureGet(device, 0U, &deviceTemperature);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get the temperature of the part.");
            goto cleanup;
        }

        checkTemp = deviceTemperature.tempDegreesCelsius[SERDES_TEMP_INDEX];
    }

    /* if any error condition is being set by adi_adrv903x_JrxRepairHistoryCheck function we can exit gracefully */
    if ((jrxRepair->historyCheck & ADI_ADRV903X_JRX_REPAIR_UNKNOWN_ERROR) == ADI_ADRV903X_JRX_REPAIR_UNKNOWN_ERROR)
    {
        /* uncheck */
        jrxRepair->historyCheck &= ~ADI_ADRV903X_JRX_REPAIR_CHECKED;
        goto cleanup;  /* exit gracefully before JrxRepairEnter */
    }

    /* Disable Serdes tracking calibration and configure PRBS */
    recoveryAction = adi_adrv903x_JrxRepairEnter(device, jrxRepair);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Enter JrxRepair routine");
        goto cleanup;
    }

    /* Check SWC*/
    recoveryAction = adi_adrv903x_JrxRepairSwCEnableGet(device, &swcEnabled);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Get SwC enabled mask");
        goto cleanup;
    }
    if (swcEnabled != 0U)
    {
        /* SwC Already Enabled */
        jrxRepair->historyCheck |= (ADI_ADRV903X_JRX_REPAIR_SWC_ALREADY_ENABLED | ADI_ADRV903X_JRX_REPAIR_UNKNOWN_ERROR);
        goto exit; /* exit gracefully (Need to go to JrxRepairExit)*/
    }

    /* copy previous history to the next to be updated */
    (void) ADI_LIBRARY_MEMCPY(&updatedHistory, &jrxRepair->history, sizeof(adi_adrv903x_JrxRepairHistory_t));

    if (((jrxRepair->historyCheck & ADI_ADRV903X_JRX_REPAIR_ASSESS_LANES) == ADI_ADRV903X_JRX_REPAIR_ASSESS_LANES) && 
         (jrxRepair->runMode == ADI_ADRV903X_JRX_REPAIR_NORMAL_MODE))
    {
        /* Run Bias Survey */
        recoveryAction = adi_adrv903x_JrxRepairBiasSurvey(device, &updatedHistory, &jrxRepair->biasSurvey);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv903x_JrxRepairBiasSurvey failed");
            goto cleanup;
        }

        if (((updatedHistory.badLaneMask  | jrxRepair->history.badLaneMask)  == jrxRepair->history.badLaneMask) &&
            ((updatedHistory.weakLaneMask | jrxRepair->history.weakLaneMask) == jrxRepair->history.weakLaneMask))
        {
            /* No new bad/weak lanes */
            jrxRepair->historyCheck |= (ADI_ADRV903X_JRX_REPAIR_NO_LANE_ERRORS | ADI_ADRV903X_JRX_REPAIR_UNKNOWN_ERROR);
            jrxRepair->history.lastTemp = checkTemp; /* No VCM Issue: Update Temp */
            goto exit; /* exit gracefully (Need to go to JrxRepairExit)*/
        }
    }

    if (jrxRepair->runMode == ADI_ADRV903X_JRX_REPAIR_FAST_MODE)
    {
        /* If fast mode is required we apply the SwC and test if there is any error in the links */
        updatedHistory.badLaneMask  |= (updatedHistory.goodLaneMask | updatedHistory.weakLaneMask);
        updatedHistory.goodLaneMask = 0x00U;
        updatedHistory.weakLaneMask = 0x00U;
    }

    if ((jrxRepair->historyCheck & ADI_ADRV903X_JRX_REPAIR_LOAD_HISTORY) == ADI_ADRV903X_JRX_REPAIR_LOAD_HISTORY)
    {
        /* Apply History config */
        recoveryAction = adi_adrv903x_JrxRepairApply(device, &updatedHistory);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv903x_JrxRepairApply failed");
            goto cleanup;
        }

        /* Wait a bit */
        recoveryAction = (adi_adrv903x_ErrAction_e) adi_common_hal_Wait_us(&device->common, ADI_ADRV903X_JRXREPAIR_APPLY_WAIT_US);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Issue");
            goto cleanup;
        }
    }

    if ((jrxRepair->historyCheck & ADI_ADRV903X_JRX_REPAIR_FAULTY_VCM_AMP) == ADI_ADRV903X_JRX_REPAIR_FAULTY_VCM_AMP)
    {
        /* Last temperature update */
        updatedHistory.lastTemp = checkTemp;

        /* Run Fast attack Initcal */
        recoveryAction = adi_adrv903x_JrxRepairFastAttackRun(device, initCalErrDataPtr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Run Serdes Fast Attack Initcal");
            goto cleanup;
        }

        /* Reset Errors and Test new configuration */
        recoveryAction = adi_adrv903x_JrxRepairTest(device, &execTest);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv903x_JrxRepairTest failed");
            goto cleanup;
        }

        /* Update failingLanes with JrxRepairTest output */
        for (index = 0; index < ADI_ADRV903X_MAX_DESERIALIZER_LANES; index++)
        {
            if (execTest.laneErrors[index] != 0U)
            {
                failingLanes |= (1U << index);
            }
        }

        /* check if the updated settings have worked ok */
        if (failingLanes != 0U)
        {
            /* Restore previous history */
            recoveryAction = adi_adrv903x_JrxRepairApply(device, &jrxRepair->history);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Restore previous JrxRepair history");
                goto cleanup;
            }
            jrxRepair->historyCheck |= ADI_ADRV903X_JRX_REPAIR_UNKNOWN_ERROR;
            jrxRepair->history.lastTemp = checkTemp; /* No VCM Issue: Update Temp */
        }
        else
        {
            /* Fix succeeded. Update current history */
            (void) ADI_LIBRARY_MEMCPY(&jrxRepair->history, &updatedHistory, sizeof(adi_adrv903x_JrxRepairHistory_t));
            jrxRepair->historyCheck |= ADI_ADRV903X_JRX_REPAIR_APPLY_SUCCESS;
        }
    }

exit:
    /* Re-enable Serdes tracking calibration or Disable PRBS Checker */
    recoveryAction = adi_adrv903x_JrxRepairExit(device, jrxRepair);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Exit JrxRepair routine");
        goto cleanup;
    }
    jrxRepair->historyCheck &= ~ADI_ADRV903X_JRX_REPAIR_CHECKED; /* uncheck history */

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairLaneAssess(adi_adrv903x_Device_t* const  device,
                                                                  uint8_t *const                badLaneMask)
{
        adi_adrv903x_ErrAction_e  recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
    adi_adrv903x_DeframerSel_e usedDfrm      = ADI_ADRV903X_DEFRAMER_0;

    uint32_t usedDfrmBaseAddr = 0U;
    uint32_t index = 0U;
    uint32_t laneId = 0U;
    uint8_t  linkType = 0U;
    uint8_t  bfValue = 0U;
    uint8_t  laneSel = 0U;
    uint8_t  localBadLaneMask = 0U;
    uint8_t  gpintStatus[2] = { 0U };

    adi_adrv903x_Dfrm204cErrCounterStatus_t dfrm204cErrCnt;
    adi_adrv903x_DfrmErrCounterStatus_t     dfrm204bErrCnt;

    static const uint32_t  GPINT_STATUS_BYTE1_ADDR = 0x16BU;
    static const uint32_t JRX_LINK_BASE_ADDR[2] =
    {
        ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_,
        ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_
    };

    static const uint8_t  GPINT_STATUS_DFRM_ALL_MASK[2] = { 0xFCU, 0x3FU };
    static const uint8_t  JRX204C_STATE_FEC_READY = 6U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, badLaneMask);

    /* Read GPINT STATUS registers */
    recoveryAction = adi_adrv903x_RegistersByteRead(device, NULL, GPINT_STATUS_BYTE1_ADDR, gpintStatus, NULL, 2U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv903x_RegistersByteRead GPINT_STATUS issue");
        goto cleanup;
    }

    for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DEFRAMERS; index++)
    {
        if ((device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled == 0U) ||
            ((gpintStatus[index] & GPINT_STATUS_DFRM_ALL_MASK[index]) == 0U))
        {
            /* skip if deframer is unused or if there is no lane related issue on that deframer*/
            continue;
        }

        usedDfrm = (adi_adrv903x_DeframerSel_e)(1U << index);
        usedDfrmBaseAddr = JRX_LINK_BASE_ADDR[index];

        /* Get the Deframer link type */
        recoveryAction = adrv903x_JrxLink_JrxLinkType_BfGet(device,
                                                            NULL,
                                                            (adrv903x_BfJrxLinkChanAddr_e)usedDfrmBaseAddr,
                                                            &linkType);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link type for Deframer");
            goto cleanup;
        }

        for (laneId = 0U; laneId < ADI_ADRV903X_MAX_DESERIALIZER_LANES; laneId++)
        {
            laneSel = (1U << laneId);
            if ((device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled & laneSel) == 0U)
            {
                continue;
            }

            /* Verify Sync/State Status */
            if (linkType != 0U)
            {
                /* 204C link type case - Read Lane State */
                recoveryAction = adrv903x_JesdCommon_JrxDl204cState_BfGet(device, NULL, ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON, laneId, &bfValue);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get 204C Lane State");
                    goto cleanup;
                }
                if (bfValue != JRX204C_STATE_FEC_READY)
                {
                    localBadLaneMask |= laneSel;
                    continue;
                }
            }
            else
            {
                /* 204B link type case - Read Sync_N and CGS sync*/
                recoveryAction = adrv903x_JesdCommon_JrxDl204bSyncN_BfGet(device, NULL, ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON, laneId, &bfValue);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get 204B Lane Sync N");
                    goto cleanup;
                }
                if (bfValue == ADI_DISABLE)
                {
                    localBadLaneMask |= laneSel;
                    continue;
                }
                recoveryAction = adrv903x_JesdCommon_JrxDl204bCgs_BfGet(device, NULL, ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON, laneId, &bfValue);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get 204B Lane CGS sync");
                    goto cleanup;
                }
                if (bfValue == ADI_DISABLE)
                {
                    localBadLaneMask |= laneSel;
                    continue;
                }
            }

            /* Verify Link Layer Error Counters */
            if (linkType != 0U)
            {
                /* 204C link type case */
                recoveryAction = adi_adrv903x_Dfrm204cErrCounterStatusGet(device, usedDfrm, index, &dfrm204cErrCnt);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get 204C Error Counters");
                    goto cleanup;
                }
                if ((dfrm204cErrCnt.crcCntValue != 0U) || (dfrm204cErrCnt.embCntValue != 0U) ||
                    (dfrm204cErrCnt.mbCntValue  != 0U) || (dfrm204cErrCnt.shCntValue  != 0U))
                {
                    localBadLaneMask |= laneSel;
                    continue;
                }
            }
            else
            {
                /* 204B link type case */
                recoveryAction = adi_adrv903x_DfrmErrCounterStatusGet(device, usedDfrm, index, &dfrm204bErrCnt);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get 204B Error Counters");
                    goto cleanup;
                }
                if ((dfrm204bErrCnt.bdCntValue  != 0U) || (dfrm204bErrCnt.nitCntValue != 0U) ||
                    (dfrm204bErrCnt.uekCntValue != 0U))
                {
                    localBadLaneMask |= laneSel;
                    continue;
                }
            }
        } /* for each phy lane */
    } /* for each deframer */

    *badLaneMask = localBadLaneMask;

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairVcmLanesFix(adi_adrv903x_Device_t* const device,
                                                                   uint8_t                laneMask,
                                                                   uint8_t                enableFix)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t index              = 0U;
    uint8_t numDeserLanesUsed  = 0U;
    uint8_t actionLaneSelector = 0U;

    const uint8_t swcVcmDisabled          = 0U;
    const uint8_t swcVcmEnabledLaneUsed   = 1U;
    const uint8_t swcVcmEnabledLaneUnused = 2U;

    static const uint32_t DESER_PD_REG_0_ADDRESS[] = { 
        ADRV903X_ADDR_DESER_PHY0_PD_REG_0,
        ADRV903X_ADDR_DESER_PHY1_PD_REG_0,
        ADRV903X_ADDR_DESER_PHY2_PD_REG_0,
        ADRV903X_ADDR_DESER_PHY3_PD_REG_0,
        ADRV903X_ADDR_DESER_PHY4_PD_REG_0,
        ADRV903X_ADDR_DESER_PHY5_PD_REG_0,
        ADRV903X_ADDR_DESER_PHY6_PD_REG_0,
        ADRV903X_ADDR_DESER_PHY7_PD_REG_0
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_PD_REG_0_VALUE[] = {
        0x01U, /* Reset value of register */
        0xFFU, /* No power-downs on the enabled lanes */
        0x35U  /* Several power-downs on the disabled lanes to save power */
    };

    static const uint32_t DESER_PD_REG_1_ADDRESS[] = { 
        ADRV903X_ADDR_DESER_PHY0_PD_REG_1,
        ADRV903X_ADDR_DESER_PHY1_PD_REG_1,
        ADRV903X_ADDR_DESER_PHY2_PD_REG_1,
        ADRV903X_ADDR_DESER_PHY3_PD_REG_1,
        ADRV903X_ADDR_DESER_PHY4_PD_REG_1,
        ADRV903X_ADDR_DESER_PHY5_PD_REG_1,
        ADRV903X_ADDR_DESER_PHY6_PD_REG_1,
        ADRV903X_ADDR_DESER_PHY7_PD_REG_1,
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_PD_REG_1_VALUE[] = {
        0x00U, /* Reset value of register */
        0x02U, /* Powers down the CM amp on the enabled lanes. */
        0x07U  /* More power-down facilities on the disabled lanes to save power */
    };

    static const uint32_t DESER_CORE1P2_TEST_ADDRESS[] = { 
        ADRV903X_ADDR_DESER_PHY0_CORE1P2_TEST,
        ADRV903X_ADDR_DESER_PHY1_CORE1P2_TEST,
        ADRV903X_ADDR_DESER_PHY2_CORE1P2_TEST,
        ADRV903X_ADDR_DESER_PHY3_CORE1P2_TEST,
        ADRV903X_ADDR_DESER_PHY4_CORE1P2_TEST,
        ADRV903X_ADDR_DESER_PHY5_CORE1P2_TEST,
        ADRV903X_ADDR_DESER_PHY6_CORE1P2_TEST,
        ADRV903X_ADDR_DESER_PHY7_CORE1P2_TEST
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_TEST_VALUE[] = {
        0x00U, /* Reset value of register */
        30U,   /*This connects the CM node of the used lanes to the AMUX*/
        29U,   /*This connects the CM bias node of the unused lanes to the AMUX*/
    };

    static const uint32_t DESER_CORE1P2_DECFE_CTL0_ADDRESS[] = { 
        ADRV903X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL0,
        ADRV903X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL0,
        ADRV903X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL0,
        ADRV903X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL0,
        ADRV903X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL0,
        ADRV903X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL0,
        ADRV903X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL0,
        ADRV903X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL0
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL0_VALUE[] = {
        0x00U, /* Reset value of register */
        0xFFU, /* No power-downs on the enabled lanes */
        0x0FU  /* More power-down facilities on the disabled lanes */
    };

    static const uint32_t DESER_CORE1P2_DECFE_CTL18_ADDRESS[] = { 
        ADRV903X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL18,
        ADRV903X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL18,
        ADRV903X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL18,
        ADRV903X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL18,
        ADRV903X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL18,
        ADRV903X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL18,
        ADRV903X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL18,
        ADRV903X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL18
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL18_VALUE[] = {
        0x00U, /* Reset value of register */
        0xFFU, /* No power-downs on the enabled lanes */
        0x01U  /* More power-down facilities on the disabled lanes */
    };
    static const uint32_t DESER_CORE1P2_DECFE_CTL28_ADDRESS[] = { 
        ADRV903X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL28,
        ADRV903X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL28,
        ADRV903X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL28,
        ADRV903X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL28,
        ADRV903X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL28,
        ADRV903X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL28,
        ADRV903X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL28,
        ADRV903X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL28
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL28_VALUE[] = {
        0x00U, /* Reset value of register */
        0xFFU, /* Not used on used lanes */
        0xFFU  /* More power-down facilities on the disabled lanes */
    };
    static const uint32_t DESER_CORE1P2_DECFE_CTL31_ADDRESS[] = { 
        ADRV903X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL31,
        ADRV903X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL31,
        ADRV903X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL31,
        ADRV903X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL31,
        ADRV903X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL31,
        ADRV903X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL31,
        ADRV903X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL31,
        ADRV903X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL31
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL31_VALUE[] = {
        0x00U, /* Reset value of register */
        0xFFU, /* Not used on used lanes */
        0x01U  /* More power-down facilities on the disabled lanes */
    };

    static const uint32_t DESER_CORE1P2_DECFE_CTL41_ADDRESS[] = { 
        ADRV903X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL41,
        ADRV903X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL41,
        ADRV903X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL41,
        ADRV903X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL41,
        ADRV903X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL41,
        ADRV903X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL41,
        ADRV903X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL41,
        ADRV903X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL41
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL41_VALUE[] = {
        0x00U, /* Reset value of register */
        0x00U, /* Not used on used lanes */
        0xFFU  /* More power-down facilities on the disabled lanes */
    };
    static const uint32_t DESER_CORE1P2_DECFE_CTL43_ADDRESS[] = { 
        ADRV903X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL43,
        ADRV903X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL43,
        ADRV903X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL43,
        ADRV903X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL43,
        ADRV903X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL43,
        ADRV903X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL43,
        ADRV903X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL43,
        ADRV903X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL43
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL43_VALUE[] = {
        0x00U, /* Reset value of register */
        0xFFU, /* Not used on used lanes */
        0x80U  /* More power-down facilities on the disabled lanes */
    };
    static const uint32_t DESER_CORE1P2_DECFE_CTL45_ADDRESS[] = { 
        ADRV903X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL45,
        ADRV903X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL45,
        ADRV903X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL45,
        ADRV903X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL45,
        ADRV903X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL45,
        ADRV903X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL45,
        ADRV903X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL45,
        ADRV903X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL45
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL45_VALUE[] = {
        0x00U, /* Reset value of register */
        0xFFU, /* Not used on used lanes */
        0x80U  /* More power-down facilities on the disabled lanes */
    };

    static const uint32_t DESER_CORE1P2_DECFE_CTL49_ADDRESS[] = { 
        ADRV903X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL49,
        ADRV903X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL49,
        ADRV903X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL49,
        ADRV903X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL49,
        ADRV903X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL49,
        ADRV903X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL49,
        ADRV903X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL49,
        ADRV903X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL49
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL49_VALUE[] = {
        0x20U, /* Reset value of register */
        0xFFU, /* Not used on used lanes */
        0x06U  /* Powers down the CM amp on the disabled lanes */
    };

    static const uint32_t DESER_CORE1P2_DECFE_CTL54_ADDRESS[] = { 
        ADRV903X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL54,
        ADRV903X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL54,
        ADRV903X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL54,
        ADRV903X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL54,
        ADRV903X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL54,
        ADRV903X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL54,
        ADRV903X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL54,
        ADRV903X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL54
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL54_VALUE[] = {
        0x00U, /* Reset value of register */
        0xFFU, /* Not used on used lanes */
        0x05U  /* More power-down facilities on the disabled lanes */
    };

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    
    if ((enableFix != ADI_TRUE) && (enableFix != ADI_FALSE))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, enableFix, "input parameter \"enableFix\" should be either True or False.");
        goto cleanup;
    }

    numDeserLanesUsed = ((laneMask >> 7U) & 1U) + \
                        ((laneMask >> 6U) & 1U) + \
                        ((laneMask >> 5U) & 1U) + \
                        ((laneMask >> 4U) & 1U) + \
                        ((laneMask >> 3U) & 1U) + \
                        ((laneMask >> 2U) & 1U) + \
                        ((laneMask >> 1U) & 1U) + \
                        (laneMask & 1U);

    if (enableFix == ADI_TRUE)
    {
        recoveryAction = adi_adrv903x_Register32Write(device, 
                                                      NULL, 
                                                      ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_TOP_8PACK, 
                                                      0x00,
                                                      0xFF);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_TOP_8PACK");
            goto cleanup;
        }
        for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DESERIALIZER_LANES; index++)
        {
            /* Lane NOT being used */
            actionLaneSelector = swcVcmEnabledLaneUnused;

            if ((laneMask & (1U << index)) == 0U)
            {
                recoveryAction = adi_adrv903x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL49_ADDRESS[index], DESER_CORE1P2_DECFE_CTL49_VALUE[actionLaneSelector], 0x07U);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV903X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL49 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv903x_Register32Write(device, NULL, DESER_PD_REG_0_ADDRESS[index], DESER_PD_REG_0_VALUE[actionLaneSelector], 0x3FU);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV903X_ADDR_DESER_PHY_PD_REG_0 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv903x_Register32Write(device, NULL, DESER_PD_REG_1_ADDRESS[index], DESER_PD_REG_1_VALUE[actionLaneSelector], 0x07U);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV903X_ADDR_DESER_PHY_PD_REG_1 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv903x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL18_ADDRESS[index], DESER_CORE1P2_DECFE_CTL18_VALUE[actionLaneSelector], 0x01U);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV903X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL18 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv903x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL31_ADDRESS[index], DESER_CORE1P2_DECFE_CTL31_VALUE[actionLaneSelector], 0x01U);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV903X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL31 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv903x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL54_ADDRESS[index], DESER_CORE1P2_DECFE_CTL54_VALUE[actionLaneSelector], 0x0FU);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV903X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL54 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv903x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL0_ADDRESS[index], DESER_CORE1P2_DECFE_CTL0_VALUE[actionLaneSelector], 0x0FU);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV903X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL0 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv903x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL28_ADDRESS[index], DESER_CORE1P2_DECFE_CTL28_VALUE[actionLaneSelector], 0xFFU);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV903X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL28 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv903x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL41_ADDRESS[index], DESER_CORE1P2_DECFE_CTL41_VALUE[actionLaneSelector], 0xFFU);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV903X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL41 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv903x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL43_ADDRESS[index], DESER_CORE1P2_DECFE_CTL43_VALUE[actionLaneSelector], 0xFFU);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV903X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL43 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv903x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL45_ADDRESS[index], DESER_CORE1P2_DECFE_CTL45_VALUE[actionLaneSelector], 0xFFU);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV903X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL45 of one of the lanes");
                    goto cleanup;
                }
            }
        }
        if (numDeserLanesUsed > 0x4U)
        {
            recoveryAction = adi_adrv903x_JrxRepairSwCEnableSet(device, laneMask);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Set SwC Enabled");
                goto cleanup;
            }

            for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DESERIALIZER_LANES; index++)
            {
                if ((laneMask & (1U << index)) == 0U)
                {
                    /* Lane NOT being used */
                    actionLaneSelector = swcVcmEnabledLaneUnused;

                    recoveryAction = adi_adrv903x_Register32Write(device, NULL, DESER_CORE1P2_TEST_ADDRESS[index], DESER_CORE1P2_TEST_VALUE[actionLaneSelector], 0x1FU);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to DESERDES CORE1P2_TEST");
                        goto cleanup;
                    }
                }
            }
        }
        else
        {
            for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DESERIALIZER_LANES; index++)
            {
                actionLaneSelector = ((laneMask & (1U << index)) == 0U) ? swcVcmEnabledLaneUnused : swcVcmEnabledLaneUsed;

                recoveryAction = adi_adrv903x_Register32Write(device, NULL, DESER_PD_REG_1_ADDRESS[index], DESER_PD_REG_1_VALUE[actionLaneSelector], 0x07U);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to DESERDES PD_REG_1");
                    goto cleanup;
                }

                recoveryAction = adi_adrv903x_Register32Write(device, NULL, DESER_CORE1P2_TEST_ADDRESS[index], DESER_CORE1P2_TEST_VALUE[actionLaneSelector], 0x1FU);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to DESERDES CORE1P2_TEST");
                    goto cleanup;
                }
            }
        }

        /* Indicate that JRxRepair has been applied */
        device->devStateInfo.devState = (adi_adrv903x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV903X_STATE_JRXREPAIRED);

    }
    else
    {
        if (numDeserLanesUsed <= 0x4U)
        {
            for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DESERIALIZER_LANES; index++)
            {
                actionLaneSelector = swcVcmDisabled;

                recoveryAction = adi_adrv903x_Register32Write(device, NULL, DESER_PD_REG_1_ADDRESS[index], DESER_PD_REG_1_VALUE[actionLaneSelector], 0x07U);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV903X_ADDR_DESER_PHY_PD_REG_1 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv903x_Register32Write(device, NULL, DESER_CORE1P2_TEST_ADDRESS[index], DESER_CORE1P2_TEST_VALUE[actionLaneSelector], 0x1FU);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to DESERDES CORE1P2_TEST");
                    goto cleanup;
                }
            }
        }
        else
        {
            recoveryAction = adi_adrv903x_JrxRepairSwCEnableSet(device, 0x0U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Set SwC Disabled");
                goto cleanup;
            }
        }

        recoveryAction = adi_adrv903x_Register32Write(device, 
                                                        NULL, 
                                                        ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_TOP_8PACK, 
                                                        ~laneMask,
                                                        0xFF);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_TOP_8PACK");
            goto cleanup;
        }

        /* Indicate that JRxRepair is disabled */
        device->devStateInfo.devState = (adi_adrv903x_ApiStates_e)(device->devStateInfo.devState & ~ADI_ADRV903X_STATE_JRXREPAIRED);

    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairInitialization(adi_adrv903x_Device_t* const device,
                                                                      uint8_t                enableRepair)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t idx = 0U;
    uint8_t screenID = 0U;
    uint8_t usedDeserLanes = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);

    recoveryAction = adrv903x_JrxRepairScreenTestChecker(device, &screenID);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Screen check has failed.");
        goto cleanup;
    }

        if (((enableRepair & ADI_ADRV903X_JRXREPAIR_INIT_ALL) == ADI_ADRV903X_JRXREPAIR_INIT_ALL) ||
       (((enableRepair & ADI_ADRV903X_JRXREPAIR_INIT_NONSCREENED) == ADI_ADRV903X_JRXREPAIR_INIT_NONSCREENED) && (screenID == ADI_FALSE)))
    {
        for (idx = 0U; idx < (uint8_t)ADI_ADRV903X_MAX_DEFRAMERS; idx++)
        {
            usedDeserLanes |= device->initExtract.jesdSetting.deframerSetting[idx].deserialLaneEnabled;
        }

        recoveryAction = adi_adrv903x_JrxRepairVcmLanesFix(device, usedDeserLanes, ADI_TRUE);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to fix enable VCM fix");
            goto cleanup;
        }
    }

    cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


/* Helper function to load CPU image from file. SHOULD ONLY BE CALLED FROM adi_adrv903x_CpuImageLoad() */
static adi_adrv903x_ErrAction_e adrv903x_CpuLoadUtil(adi_adrv903x_Device_t* const              device,
                                                     const adi_adrv903x_cpuBinaryInfo_t* const cpuBinaryInfo,
                                                     FILE*                                     cpuImageFilePtr,
                                                     const adi_adrv903x_CpuType_e              cpuType)
{
    adi_adrv903x_ErrAction_e  recoveryAction    = ADI_ADRV903X_ERR_ACT_NONE;
    uint32_t                  numFileChunks     = 0U;
    uint32_t                  chunkIndex        = 0U;
    uint8_t                   cpuImageChunk[ADI_ADRV903X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];
    uint32_t                  byteCount         = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, cpuBinaryInfo);

    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, cpuImageFilePtr);

    /* Calculate number of image chunks to write */
    if (cpuType == ADI_ADRV903X_CPU_TYPE_0)
    {
        numFileChunks = (uint32_t)(ADI_ADRV903X_CPU_0_BINARY_IMAGE_FILE_SIZE_BYTES / ADI_ADRV903X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES);
    }
    else if (cpuType == ADI_ADRV903X_CPU_TYPE_1)
    {
        numFileChunks = (uint32_t)(ADI_ADRV903X_CPU_1_BINARY_IMAGE_FILE_SIZE_BYTES / ADI_ADRV903X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES);
    }
    else
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuType, "Invalid CPU type");
        return recoveryAction;
    }

        for (chunkIndex = 0U; chunkIndex < numFileChunks; ++chunkIndex)
    {
        /* Read Segment of CPU Image */
        byteCount = ADI_LIBRARY_FREAD(&cpuImageChunk[0U], 1, ADI_ADRV903X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES, cpuImageFilePtr);
        if (byteCount == ADI_ADRV903X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES)
        {
            /* Write CPU Image Segment */
            recoveryAction = adi_adrv903x_CpuImageWrite(device,
                                                        cpuType,
                                                        (chunkIndex * ADI_ADRV903X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES),
                                                        &cpuImageChunk[0U],
                                                        byteCount);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error During Binary Loading (e.g. SPI Issue)");
                return recoveryAction;
            }
        }
        else
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Error Reading Block of Data from CPU Binary File");
            return recoveryAction;
        }
    }

    return recoveryAction;
}


static adi_adrv903x_ErrAction_e adrv903x_JrxRepairHistoryRangeCheck(adi_adrv903x_Device_t* const           device,
                                                                    adi_adrv903x_JrxRepairHistory_t* const repairHistory)
{
    adi_adrv903x_ErrAction_e    recoveryAction    = ADI_ADRV903X_ERR_ACT_NONE;
    const int16_t               minTemp           = -274;
    const int16_t               maxTemp           = (ADI_ADRV903X_FACTORY_TEMPERATURE_TEST + ADI_ADRV903X_FACTORY_TEMPERATURE_MARGIN);
    uint8_t                     usedDeserLanes    = 0U;
    uint32_t                    index             = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, repairHistory);

    if ((repairHistory->lastTemp < minTemp) || (repairHistory->lastTemp > maxTemp))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, repairHistory->lastTemp, "Temperature outside of range");
        return recoveryAction;
    }

    for (index = 0U; index < (uint32_t)ADI_ADRV903X_MAX_DEFRAMERS; index++)
    {
        usedDeserLanes |= device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled;
    }

    if ((repairHistory->goodLaneMask | repairHistory->weakLaneMask | repairHistory->badLaneMask) != usedDeserLanes)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, repairHistory->goodLaneMask, "good+bad+weak not compatible with lanes enabled");
        return recoveryAction;
    }

    return recoveryAction;
}

static void adrv903x_EndiannessCheckAndConvert(void* const buffer, const size_t elementSize, const size_t elementCount)
{
    /* new function entry reporting */
    uint8_t i = 0U;
    uint16_t* tmp16;
    uint32_t* tmp32;
    uint64_t* tmp64;

    if (elementSize == 2U)
    {
        //biteswap small
        tmp16 = (uint16_t* const)buffer;
        for (i = 0; i < elementCount; i++) {
            tmp16[i] = ADRV903X_HTOCS(tmp16[i]);
        }
    }
    else if (elementSize == 4U)
    {
        //biteswap large
        tmp32 = (uint32_t* const)buffer;
        for (i = 0; i < elementCount; i++) {
            tmp32[i] = ADRV903X_HTOCL(tmp32[i]);
        }
    }
    else if (elementSize == 8U)
    {
        //biteswap large large
        tmp64 = (uint64_t* const)buffer;
        for (i = 0; i < elementCount; i++) {
            tmp64[i] = ADRV903X_HTOCLL(tmp64[i]);
        }
    }
}
static size_t adrv903x_VariableFromFileExtract(adi_adrv903x_Device_t* const device, 
                                                void* const buffer, 
                                                const size_t elementSize, 
                                                const size_t elementCount, 
                                                FILE* file)
{
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, file);

    uint8_t returnValue = ADI_LIBRARY_FREAD(buffer, 1, elementSize * elementCount, file);
    if (returnValue > 0)
    {
        adrv903x_EndiannessCheckAndConvert(buffer, elementSize, elementCount);
    }
    return returnValue;
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainTableChecksumRead(adi_adrv903x_Device_t* const device,
                                                                      const adi_adrv903x_RxGainTableInfo_t* const  rxGainTableInfoPtr,
                                                                      uint32_t* const rxGainTableChecksum)
{
        adi_adrv903x_ErrAction_e    recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    const uint8_t               LINE_BUFFER_SIZE    = 128U;
    const uint8_t               HEADER_BUFFER_SIZE  = 16U;
    const uint8_t               NUM_COLUMNS         = 5U;
    FILE*                       rxGainTableFilePtr  = NULL;
    char                        rxGainTableLineBuffer[LINE_BUFFER_SIZE];
    char                        headerStr1[HEADER_BUFFER_SIZE];
    adi_adrv903x_Version_t      tableVersion        = { 0U, 0U, 0U, 0U };
    uint32_t                    checksum[4U]        = { 0U, 0U, 0U, 0U };

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxGainTableChecksum, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxGainTableInfoPtr, cleanup);

    if (ADI_LIBRARY_STRNLEN((const char*)rxGainTableInfoPtr->filePath, ADI_ADRV903X_MAX_FILE_LENGTH) == ADI_ADRV903X_MAX_FILE_LENGTH)
    {
        /* Path is not terminated */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfoPtr->filePath, "Unterminated path string");
        goto cleanup;
    }

    /* Open Rx Gain Table CSV file */
#ifdef __GNUC__
    rxGainTableFilePtr = ADI_LIBRARY_FOPEN((const char *)rxGainTableInfoPtr->filePath, "r");
#else
    if (ADI_LIBRARY_FOPEN_S(&rxGainTableFilePtr, (const char *)rxGainTableInfoPtr->filePath, "r") != 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxGainTableInfoPtr->filePath,
                               "Unable to open Rx Gain Table csv file. Please check if the path is correct or the file is open in another program");

        goto cleanup;
    }
#endif

    if (rxGainTableFilePtr == NULL)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxGainTableInfoPtr->filePath,
                               "Invalid Rx Gain Table csv file path while attempting to load Rx Gain Table");
        goto cleanup;
    }

    if (ADI_LIBRARY_FGETS(rxGainTableLineBuffer, sizeof(rxGainTableLineBuffer), rxGainTableFilePtr) == NULL)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableFilePtr, "Empty Rx Gain Table Detected");
        goto cleanup;
    }


    /* Parse the first line of the Rx Gain Table file which contains the version info */
#ifdef __GNUC__
    if (sscanf(rxGainTableLineBuffer,
               "%[^,],%u,%u,%u,%u",
               headerStr1,
               (uint32_t*)&tableVersion.majorVer,
               (uint32_t*)&tableVersion.minorVer,
               (uint32_t*)&tableVersion.maintenanceVer,
               (uint32_t*)&tableVersion.buildVer) != NUM_COLUMNS)
#else
        if (sscanf_s(rxGainTableLineBuffer,
                     "%[^,],%d,%d,%d,%d",
                     headerStr1,
                     (uint32_t)sizeof(headerStr1),
                     &version.majorVer,
                     &version.minorVer,
                     &version.maintenanceVer,
                     &version.buildVer) != NUM_COLUMNS)
#endif
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfoPtr->filePath, "Invalid Rx Gain Table Format Detected");
        goto cleanup;
    }

    /* Verify that Gain Table Version Format is correct */
    if (ADI_LIBRARY_STRSTR(headerStr1, "Version") == NULL)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr1, "Version Column Expected First");
        goto cleanup;
    }


    if (ADI_LIBRARY_FGETS(rxGainTableLineBuffer, sizeof(rxGainTableLineBuffer), rxGainTableFilePtr) == NULL)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableFilePtr, "Empty Rx Gain Table Detected");
        goto cleanup;
    }

    /* Parse the second line of the Rx Gain Table file which contains the checksum info */
#ifdef __GNUC__
    if (sscanf(rxGainTableLineBuffer,
               "%[^,],%u,%u,%u,%u",
               headerStr1,
               (uint32_t*)&checksum[0],
               (uint32_t*)&checksum[1],
               (uint32_t*)&checksum[2],
               (uint32_t*)&checksum[3]) != NUM_COLUMNS)
#else
        if (sscanf_s(rxGainTableLineBuffer,
                     "%[^,],%d,%d,%d,%d",
                     headerStr1,
                     (uint32_t)sizeof(headerStr1),
                     &checksum[0],
                     &checksum[1],
                     &checksum[2],
                     &checksum[3]) != NUM_COLUMNS)
#endif
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfoPtr->filePath, "Invalid Rx Gain Table Format Detected");
        goto cleanup;
    }

    /* Verify that Gain Table Checksum Format is correct */
    if (ADI_LIBRARY_STRSTR(headerStr1, "Checksum") == NULL)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr1, "Checksum Column Expected First");
        goto cleanup;
    }

    *rxGainTableChecksum = checksum[0];

    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

cleanup:

    if (rxGainTableFilePtr != NULL)
    {
        /* Close Rx Gain Table csv file */
        if (ADI_LIBRARY_FCLOSE(rxGainTableFilePtr) != 0)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   rxGainTableInfoPtr->filePath,
                                   "Fatal error while trying to close Rx Gain Table csv file. Possible memory shortage while flushing / other I/O errors.");
        }
    }

    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainTableChecksumCalculate(adi_adrv903x_Device_t* const device,
                                                                           const adi_adrv903x_RxChannels_e rxChannel,
                                                                           uint32_t* const rxGainTableChecksum)
{
        adi_adrv903x_ErrAction_e  recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

#define MAX_RX_GAIN_TABLE_LINES  256U
#define GAIN_TABLE_SIZE          7U
    static const uint8_t INDEX_MASK               = 0xFFU;
    static const uint8_t PHASE_OFFSET_MASK        = 0xFFU;
    static const uint8_t PHASE_OFFSET_SHIFT       = 8U;
    static const uint8_t DIG_GAIN_MASK            = 0xFFU;
    static const uint8_t DIG_GAIN_SHIFT           = 8U;

    uint16_t numGainIndicesRead = 0U;
    uint32_t i                  = 0U;
    uint32_t checksum           = 0U;
    uint8_t  finalCrc           = ADI_FALSE;

    adi_adrv903x_RxGainTableRow_t gainTableRow[MAX_RX_GAIN_TABLE_LINES];
    uint8_t                       gainTableData[GAIN_TABLE_SIZE] = { 0U };

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxGainTableChecksum, cleanup);
    ADI_LIBRARY_MEMSET(&gainTableRow, 0, sizeof(gainTableRow));

    /* Check that the channel requested is valid */
    if ((rxChannel != ADI_ADRV903X_RX0) &&
        (rxChannel != ADI_ADRV903X_RX1) &&
        (rxChannel != ADI_ADRV903X_RX2) &&
        (rxChannel != ADI_ADRV903X_RX3) &&
        (rxChannel != ADI_ADRV903X_RX4) &&
        (rxChannel != ADI_ADRV903X_RX5) &&
        (rxChannel != ADI_ADRV903X_RX6) &&
        (rxChannel != ADI_ADRV903X_RX7))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannel,
                               "Invalid Rx Channel Requested for gain table read. Valid Rx channels include Rx0-Rx7");
        goto cleanup;
    }

    recoveryAction = adi_adrv903x_RxGainTableRead(device,
                                                  rxChannel,
                                                  ADI_ADRV903X_MAX_GAIN_TABLE_INDEX,
                                                  gainTableRow,
                                                  MAX_RX_GAIN_TABLE_LINES,
                                                  &numGainIndicesRead);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannel,
                               "Rx Gain Table read failure");
        goto cleanup;
    }

    if (numGainIndicesRead != MAX_RX_GAIN_TABLE_LINES)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               numGainIndicesRead,
                               "Rx Gain Table read number of lines wrong");
        goto cleanup;
    }

    for (i = 0; i < MAX_RX_GAIN_TABLE_LINES; i++)
    {
        gainTableData[0] = (uint8_t)(i & INDEX_MASK);
        gainTableData[1] = (uint8_t)(gainTableRow[i].rxFeGain);
        gainTableData[2] = (uint8_t)(gainTableRow[i].extControl);
        gainTableData[3] = (uint8_t)(gainTableRow[i].phaseOffset & PHASE_OFFSET_MASK);
        gainTableData[4] = (uint8_t)((gainTableRow[i].phaseOffset >> PHASE_OFFSET_SHIFT) & PHASE_OFFSET_MASK);
        gainTableData[5] = (uint8_t)(gainTableRow[i].digGain & DIG_GAIN_MASK);
        gainTableData[6] = (uint8_t)((gainTableRow[i].digGain >> DIG_GAIN_SHIFT) & DIG_GAIN_MASK);

        if (i == (MAX_RX_GAIN_TABLE_LINES - 1))
        {
            finalCrc = ADI_TRUE;
        }

        checksum = adrv903x_Crc32ForChunk(gainTableData,
                                          GAIN_TABLE_SIZE,
                                          checksum,
                                          finalCrc);
    }

    *rxGainTableChecksum = checksum;

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_InitDataExtract(adi_adrv903x_Device_t* const                     device,
                                                              const adi_adrv903x_CpuProfileBinaryInfo_t* const cpuBinaryInfo,
                                                              adi_adrv903x_Version_t* const                    apiVer,
                                                              adi_adrv903x_CpuFwVersion_t* const               fwVer,
                                                              adi_adrv903x_Version_t* const                    streamVer,
                                                              adi_adrv903x_Init_t* const                       init,
                                                              adi_adrv903x_PostMcsInit_t* const                postMcsInit,
                                                              adi_adrv903x_ExtractInitDataOutput_e* const      checkOutput)
{
        adi_adrv903x_ErrAction_e    recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    FILE*                       cpuProfileFilePtr   = NULL;
    uint32_t                    totalFileSize       = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, apiVer,        cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, fwVer,         cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, streamVer,     cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, init,          cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, postMcsInit,   cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, checkOutput,   cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, cpuBinaryInfo, cleanup);

    if (ADI_LIBRARY_STRNLEN((const char*)cpuBinaryInfo->filePath, ADI_ADRV903X_MAX_FILE_LENGTH) == ADI_ADRV903X_MAX_FILE_LENGTH)
    {
        /* Path is not terminated */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo, "Unterminated path string");
        goto cleanup;
    }

    /* Open cpu profile binary file */
#ifdef __GNUC__
    cpuProfileFilePtr = ADI_LIBRARY_FOPEN((const char*)cpuBinaryInfo->filePath, "rb");
#else
    if (ADI_LIBRARY_FOPEN_S(&cpuProfileFilePtr, (const char*)cpuBinaryInfo->filePath, "rb") != 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileBinaryInfoPtr->filePath, "Unable to open CPU Profile Binary. Please check filepath is correct");
        goto cleanup;
    }
#endif

    if (NULL == cpuProfileFilePtr)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to open CPU Profile Binary. Please check filepath is correct")
        goto cleanup;
    }

    /* Determine file size */
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, 0U, SEEK_END) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Seek to EOF Failed");
        goto cleanup;
    }

    /* ADI_LIBRARY_FTELL returns long type */
    totalFileSize = (uint32_t) ADI_LIBRARY_FTELL(cpuProfileFilePtr);

    /* Check that cpu profile binary file is not empty */
    if (0U == totalFileSize)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, totalFileSize, "Zero Length CPU Profile Binary Detected");
        goto cleanup;
    }

    if ((uint32_t)sizeof(adrv903x_DeviceProfile_t) == totalFileSize)
    {
        recoveryAction  = ADI_ADRV903X_ERR_ACT_NONE;
        *checkOutput    = ADI_ADRV903X_EXTRACT_INIT_DATA_LEGACY_PROFILE_BIN;
        goto cleanup;
    }
    
    if(ADI_LIBRARY_FSEEK(cpuProfileFilePtr, 0, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Could not rewind to start");
        goto cleanup;
    }
    
    recoveryAction = adrv903x_LoadBinFile(device, cpuProfileFilePtr, apiVer, fwVer, streamVer, init, postMcsInit, (uint32_t)sizeof(adrv903x_DeviceProfile_t));
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Issue with file path for cpu profile");
        goto cleanup;
    }

    *checkOutput = ADI_ADRV903X_EXTRACT_INIT_DATA_POPULATED;

cleanup:
    if (cpuProfileFilePtr != NULL)
    {
        /* Close CPU Profile binary file */
        if (0 != ADI_LIBRARY_FCLOSE(cpuProfileFilePtr))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close CPU Profile Binary File");
        }
    }

    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


