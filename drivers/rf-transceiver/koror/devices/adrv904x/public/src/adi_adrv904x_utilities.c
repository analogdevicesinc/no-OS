/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_utilities.c
* \brief Contains ADI Transceiver CPU functions
*        Analog Devices maintains and provides updates to this code layer.
*        The end user should not modify this file or any code in this directory.
*
* ADRV904X API Version: 2.10.0.4
*/

#include "adi_adrv904x_utilities.h"
#include "adi_adrv904x_cpu.h"
#include "adi_adrv904x_datainterface.h"
#include "adi_adrv904x_radioctrl.h"
#include "adi_adrv904x_cals.h"
#include "adi_adrv904x_tx.h"
#include "adi_library.h"

#include "adi_adrv904x_dfe_cpu.h"

#include "../../private/include/adrv904x_init.h"
#include "../../private/include/adrv904x_cpu_archive_types.h"
#include "../../private/include/adrv904x_rx.h"
#include "../../private/include/adrv904x_cpu.h"
#include "../../private/include/adrv904x_gpio.h"
#include "../../private/include/adrv904x_datainterface.h"
#include "../../private/include/adrv904x_cpu_cmd_fast_attack.h"
#include "../../private/include/adrv904x_binloader.h"
#include "../../private/bf/adrv904x_bf_tx_dig.h"
#include "../../public/include/adi_adrv904x_version.h"
#include "../../public/include/adi_adrv904x_core.h"
#include "../../private/bf/adrv904x_bf_core.h"


#include "../../private/include/adrv904x_dfe_svc_bbic_bridge_t.h"
#include "../../private/include/adrv904x_dfe_svc_cmd_force_exception.h"
#include "../../private/include/adrv904x_dfe_cpu.h"
#include "../../private/include/adrv904x_dfe_cfr.h"
#include "../../private/include/adrv904x_dfe_svc_err_dfe_boot_status_t.h"
#include "../../private/include/adrv904x_carrier_reconfigure_types.h"
#include "../../private/include/adrv904x_dfe_vswr.h"


#include "../../private/include/adrv904x_radioctrl.h"
#include "../../private/include/adrv904x_dfe_svc_radio_ctrl_sequencer_t.h"
#include "../../private/bf/adrv904x_bf_sequencer.h"

#define ADI_FILE    ADI_ADRV904X_FILE_PUBLIC_UTILITIES

/*****************************************************************************/
/***** Helper functions' prototypes ******************************************/
/*****************************************************************************/
static adi_adrv904x_ErrAction_e adrv904x_CpuLoadUtil(adi_adrv904x_Device_t* const              device,
                                                     const adi_adrv904x_cpuBinaryInfo_t* const cpuBinaryInfo,
                                                     FILE*                                     cpuImageFilePtr,
                                                     const adi_adrv904x_CpuType_e              cpuType);
static uint32_t adrv904x_insert64Bits(  uint8_t*            array,
                                        uint64_t            storeVariable);
static uint32_t adrv904x_insert32Bits(  uint8_t*            array,
                                        uint32_t            storeVariable);
static uint32_t adrv904x_insert16Bits(  uint8_t*            array,
                                        uint16_t            storeVariable);
static uint32_t adrv904x_insert8Bits(   uint8_t*            array,
                                        uint8_t             storeVariable);

static adi_adrv904x_ErrAction_e adrv904x_dumpMemoryRegion(  adi_adrv904x_Device_t* const device, 
                                                            FILE **ofp,
                                                            uint32_t startAddr, 
                                                            uint32_t endAddr, 
                                                            uint32_t* const dumpSize,
                                                            uint32_t *recordCrc);

static adi_adrv904x_ErrAction_e adrv904x_dumpTraceCpuData(  adi_adrv904x_Device_t* const device, 
                                                            FILE **ofp,
                                                            uint32_t baseAddr, 
                                                            uint32_t *recordCrc);

static void adrv904x_EndiannessCheckAndConvert(void* const buffer, const size_t elementSize, const size_t elementCount);

#define VARIABLE_ASSIGNMENT(target, source)         \
        target = source;                            \
        if(ADRV904X_LITTLE_ENDIAN == 0)             \
        {                                           \
            adrv904x_EndiannessCheckAndConvert(&target, sizeof(target), 1);\
        }\

static size_t adrv904x_VariableFromFileExtract(adi_adrv904x_Device_t* const device, 
                                                void* const buffer, 
                                                const size_t elementSize, 
                                                const size_t elementCount, 
                                                FILE* file);

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuImageLoad(adi_adrv904x_Device_t* const              device,
                                                           const adi_adrv904x_cpuBinaryInfo_t* const cpuBinaryInfo)
{
        adi_adrv904x_ErrAction_e    recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    FILE*                       cpuImageFilePtr     = NULL;
    uint32_t                    fileSize            = 0U;
    uint32_t                    byteCount           = 0U;
    uint32_t                    expFileSize         = 0U;
    adrv904x_CpuArchiveHeader_t archiveHdr;
    uint32_t                    i                   = 0u;
    uint32_t                    expectedXSum        = 0U;
    uint8_t*                    archivePtr          = NULL;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuBinaryInfo, cleanup);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuBinaryInfo->filePath, cleanup);

    ADI_LIBRARY_MEMSET(&archiveHdr, 0, sizeof(adrv904x_CpuArchiveHeader_t));

    if (ADI_LIBRARY_STRNLEN((const char*)cpuBinaryInfo->filePath, ADI_ADRV904X_MAX_FILE_LENGTH) == ADI_ADRV904X_MAX_FILE_LENGTH)
    {
        /* Path is not terminated */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Unterminated path string");
        goto cleanup;
    }

    /* Open CPU archive file */
#ifdef __GNUC__
    cpuImageFilePtr = ADI_LIBRARY_FOPEN((const char *)cpuBinaryInfo->filePath, "rb");
#else
    if (ADI_LIBRARY_FOPEN_S(&cpuImageFilePtr, (const char *)cpuBinaryInfo->filePath, "rb") != 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Invalid Binary File or Path Detected");
        goto cleanup;
    }
#endif
    if (NULL == cpuImageFilePtr)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Invalid Binary File or Path Detected");
        goto cleanup;
    }

    /* Determine file size */
    if (ADI_LIBRARY_FSEEK(cpuImageFilePtr, 0, SEEK_END) != 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Seek to EOF Failed");
        goto cleanup;
    }

    /* ADI_LIBRARY_FTELL returns long type */
    fileSize = (uint32_t) ADI_LIBRARY_FTELL(cpuImageFilePtr);

    /* Check if CPU Image File is Empty */
    if (fileSize == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Zero Length CPU Image Detected");
        goto cleanup;
    }

    /* Check that size of the file is a multiple of 4 */
    if ((fileSize & 0x3U) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Incorrect CPU Binary File Block Size");
        goto cleanup;
    }

    /* Verify file contains at least the archive header */
    if (fileSize < sizeof(adrv904x_CpuArchiveHeader_t))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Invalid CPU image file detected (very small)");
        goto cleanup;
    }

    /* Rewind the file pointer to beginning of the file */
    if (ADI_LIBRARY_FSEEK(cpuImageFilePtr, 0U, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    /* Read out the archive header */
    byteCount = ADI_LIBRARY_FREAD(&archiveHdr, 1, sizeof(archiveHdr), cpuImageFilePtr);
    if (byteCount != sizeof(archiveHdr))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Error reading header from CPU image file");
        goto cleanup;
    }

    /* Extract archive header contents (stored in CPU byte order) */
    archiveHdr.magicNum = ADRV904X_CTOHL(archiveHdr.magicNum);
    archiveHdr.formatRev = ADRV904X_CTOHL(archiveHdr.formatRev);
    archiveHdr.xsum = ADRV904X_CTOHL(archiveHdr.xsum);

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
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Corrupted CPU image file detected");
        goto cleanup;
    }

    /* Verify header magic number is correct */
    if (archiveHdr.magicNum != ADRV904X_CPU_ARCHIVE_MAGIC_NUM)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Unsupported CPU image file detected");
        goto cleanup;
    }

    /* Verify archive format revision is correct */
    if (archiveHdr.formatRev != ADRV904X_CPU_ARCHIVE_REV_1)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Unsupported CPU image file detected");
        goto cleanup;
    }

    /* Verify file size is correct (header + two CPU images) */
    expFileSize = sizeof(adrv904x_CpuArchiveHeader_t) + ADI_ADRV904X_CPU_0_BINARY_IMAGE_FILE_SIZE_BYTES + ADI_ADRV904X_CPU_1_BINARY_IMAGE_FILE_SIZE_BYTES;
    if (fileSize != expFileSize)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Invalid CPU image file detected");
        goto cleanup;
    }

    /* Put the part into streaming mode to help facilitate the large, contiguous SPI writes in CpuImageWrite */
    recoveryAction = adrv904x_SpiStreamingEntry(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with entering Spi Streaming mode");
        goto cleanup;
    }

    /* Load CPU0 image */
    recoveryAction = adrv904x_CpuLoadUtil(device, cpuBinaryInfo, cpuImageFilePtr, ADI_ADRV904X_CPU_TYPE_0);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error loading CPU0 image");
        goto cleanup;
    }

    /* Load CPU1 image */
    recoveryAction = adrv904x_CpuLoadUtil(device, cpuBinaryInfo, cpuImageFilePtr, ADI_ADRV904X_CPU_TYPE_1);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error loading CPU1 image");
        goto cleanup;
    }

    /* Load successful. Update device state. */
    device->devStateInfo.devState = (adi_adrv904x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV904X_STATE_CPU0LOADED | ADI_ADRV904X_STATE_CPU1LOADED);

    /* Close CPU Image File */
    if (0 == ADI_LIBRARY_FCLOSE(cpuImageFilePtr))
    {
        cpuImageFilePtr = NULL;
    }
    else
    {
        cpuImageFilePtr = NULL;
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Cannot Close CPU Image File");
        goto cleanup;
    }

cleanup :
    if (cpuImageFilePtr != NULL)
    {
        /* Close CPU Image File */
        if (0 != ADI_LIBRARY_FCLOSE(cpuImageFilePtr))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close CPU Image File");
        }
    }

    if (device->devStateInfo.spiStreamingOn == ADI_TRUE)
    {
        if( ADI_ADRV904X_ERR_ACT_NONE != adrv904x_SpiStreamingExit(device))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_INTERFACE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Spi Streaming Exit Issue");
        }
    }

    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_StreamImageLoad(adi_adrv904x_Device_t* const                    device,
                                                              const adi_adrv904x_streamBinaryInfo_t* const    streamBinaryInfoPtr)
{
        const size_t                BIN_ELEMENT_SIZE        = 1U;
    adi_adrv904x_ErrAction_e    recoveryAction          = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    FILE*                       streamBinaryFilePtr     = NULL;
    uint32_t                    fileSize                = 0U;
    uint32_t                    numFileChunks           = 0U;
    uint32_t                    chunkIndex              = 0U;
    uint8_t                     streamBinaryImageBuffer[ADI_ADRV904X_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, streamBinaryInfoPtr, cleanup);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, streamBinaryInfoPtr->filePath, cleanup);

    if (ADI_LIBRARY_STRNLEN((const char*)streamBinaryInfoPtr->filePath, ADI_ADRV904X_MAX_FILE_LENGTH) == ADI_ADRV904X_MAX_FILE_LENGTH)
    {
        /* Path is not terminated */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, streamBinaryInfoPtr->filePath, "Unterminated path string");
        goto cleanup;
    }

    /* Open Stream Binary File */
#ifdef __GNUC__
    streamBinaryFilePtr = ADI_LIBRARY_FOPEN((const char *)streamBinaryInfoPtr->filePath, "rb");
#else
    if (ADI_LIBRARY_FOPEN_S(&streamBinaryFilePtr, (const char *)streamBinaryInfoPtr->filePath, "rb") != 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, streamBinaryInfoPtr->filePath, "Invalid Binary File or Path Detected");
        goto cleanup;
    }
#endif

    if (NULL == streamBinaryFilePtr)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, streamBinaryInfoPtr->filePath, "Invalid Binary File or Path Detected");
        goto cleanup;
    }

    /*Determine file size*/
    if (ADI_LIBRARY_FSEEK(streamBinaryFilePtr, 0U, SEEK_END) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, streamBinaryFilePtr, "Seek to EOF Failed");
        goto cleanup;
    }

    /* ADI_LIBRARY_FTELL returns long type */
    fileSize = (uint32_t) ADI_LIBRARY_FTELL(streamBinaryFilePtr);
    numFileChunks = (uint32_t)(fileSize / ADI_ADRV904X_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES);

    /* Check that Stream binary file is not empty */
    if (fileSize == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, streamBinaryFilePtr, "Zero Length Stream Image Detected");
        goto cleanup;
    }

    /* Check that Stream binary file size does not exceed maximum size */
    if (fileSize > ADI_ADRV904X_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileSize, "Stream Binary Size exceeds Maximum Limit");
        goto cleanup;
    }

    /* Check that size of the file is a multiple of 4 */
    if ((fileSize & 0x3U) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, streamBinaryInfoPtr->filePath, "Incorrect Stream Binary File Block Size");
        goto cleanup;
    }

    /* Rewind the file pointer to beginning of the file */
    if (ADI_LIBRARY_FSEEK(streamBinaryFilePtr, 0U, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, streamBinaryFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    /* Put the part into streaming mode to help facilitate the large, contiguous SPI writes in CpuImageWrite */
    recoveryAction = adrv904x_SpiStreamingEntry(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with entering Spi Streaming mode");
        goto cleanup;
    }

    /* Read Stream binary file */
    for (chunkIndex = 0U; chunkIndex < numFileChunks; ++chunkIndex)
    {
        if (ADI_LIBRARY_FREAD(&streamBinaryImageBuffer[0U], BIN_ELEMENT_SIZE, ADI_ADRV904X_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES, streamBinaryFilePtr) < ADI_ADRV904X_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numFileChunks, "Error Reading Block of Data from CPU Binary File");
            goto cleanup;
        }

        /* Write Stream binary */
        recoveryAction = adi_adrv904x_StreamImageWrite( device,
                                                        (chunkIndex * ADI_ADRV904X_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES),
                                                        &streamBinaryImageBuffer[0U],
                                                        ADI_ADRV904X_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
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
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close Stream Image File");
        }
    }

    if (device->devStateInfo.spiStreamingOn == ADI_TRUE)
    {
        if (ADI_ADRV904X_ERR_ACT_NONE != adrv904x_SpiStreamingExit(device))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_INTERFACE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Spi Streaming Exit Issue");
        }
    }

    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainTableLoad(adi_adrv904x_Device_t* const            device,
                                                              const adi_adrv904x_RxGainTableInfo_t    rxGainTableInfo[],
                                                              const uint32_t                          rxGainTableArrSize)
{
        const uint8_t                   LINE_BUFFER_SIZE    = 128U;
    const uint8_t                   HEADER_BUFFER_SIZE  = 16U;
    const uint8_t                   NUM_COLUMNS         = 5U;
    adi_adrv904x_ErrAction_e        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
    adi_adrv904x_RxGainTableRow_t   rxGainTableRowBuffer[ADI_ADRV904X_RX_GAIN_TABLE_SIZE_ROWS];
    adi_adrv904x_Version_t          tableVersion        = { 0, 0, 0, 0 };
    uint32_t                        checksum[4U]        = { 0, 0, 0, 0 };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    if (0U == rxGainTableArrSize)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableArrSize, "Invalid Array Size");
        goto cleanup;
    }

    for (arrayIndex = 0U; arrayIndex < rxGainTableArrSize; ++arrayIndex)
    {
        ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxGainTableInfo[arrayIndex].filePath, cleanup);

        if (0U == rxGainTableInfo[arrayIndex].channelMask)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfo[arrayIndex].channelMask, "Invalid Rx Gain Table Channel Mask Detected");
            goto cleanup;
        }

        if (ADI_LIBRARY_STRNLEN((const char*)rxGainTableInfo[arrayIndex].filePath, ADI_ADRV904X_MAX_FILE_LENGTH) == ADI_ADRV904X_MAX_FILE_LENGTH)
        {
            /* Path is not terminated */
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfo[arrayIndex].filePath, "Unterminated path string");
            goto cleanup;
        }

        /* Open Rx Gain Table CSV file */
#ifdef __GNUC__
        rxGainTableFilePtr = ADI_LIBRARY_FOPEN((const char *)rxGainTableInfo[arrayIndex].filePath, "r");
#else
        if (ADI_LIBRARY_FOPEN_S(&rxGainTableFilePtr, (const char *)rxGainTableInfo[arrayIndex].filePath, "r") != 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfo[arrayIndex].filePath, "File is already Open or Invalid CSV File/Path Detected");
            goto cleanup;
        }
#endif
        if (rxGainTableFilePtr == NULL)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfo[arrayIndex].filePath, "File is already Open or Invalid CSV File/Path Detected");
            goto cleanup;
        }



        /* Check for empty Rx Gain Table */
        if (fgets(rxGainTableLineBuffer, sizeof(rxGainTableLineBuffer), rxGainTableFilePtr) != NULL)
        {

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
                    recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfo[arrayIndex].filePath, "Invalid Rx Gain Table Format Detected");
                    goto cleanup;
                }

                /* Verify that Gain Table Version Format is correct */
                if (ADI_LIBRARY_STRSTR(headerStr1, "Version") == NULL)
                {
                    recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr1, "Version Column Expected First");
                    goto cleanup;
                }


#if ADI_ADRV904X_API_VERSION_RANGE_CHECK > 0
            {
                adi_adrv904x_Version_t currentVersion = { 0U, 0U, 0U, 0U };

                                recoveryAction = adi_adrv904x_ApiVersionGet(device, &currentVersion);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainTable Version Get Issue");
                    goto cleanup;
                }

                recoveryAction = adrv904x_ApiVersionRangeCheck(device, &tableVersion, &currentVersion, &currentVersion);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainTable Version Check Issue");
                    goto cleanup;
                }
            }
#endif

            if (fgets(rxGainTableLineBuffer, sizeof(rxGainTableLineBuffer), rxGainTableFilePtr) == NULL)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
                    recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfo[arrayIndex].filePath, "Invalid Rx Gain Table Format Detected");
                    goto cleanup;
                }

            /* Verify that Gain Table Version Format is correct */
            if (ADI_LIBRARY_STRSTR(headerStr1, "Checksum") == NULL)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr1, "Checksum Column Expected First");
                goto cleanup;
            }

            if (fgets(rxGainTableLineBuffer, sizeof(rxGainTableLineBuffer), rxGainTableFilePtr) == NULL)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
                {
                    recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfo[arrayIndex].filePath, "Invalid Rx Gain Table Format Detected");
                    goto cleanup;
                }

            /* Verify that Gain Table Format is correct */
            if (ADI_LIBRARY_STRSTR(headerStr1, "Gain Index") == NULL)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr1, "Gain Index Column Expected First");
                goto cleanup;
            }

            if (ADI_LIBRARY_STRSTR(headerStr2, "FE Control Word") == NULL)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr2, "FE Control Column Expected Second");
                goto cleanup;
            }

            if (ADI_LIBRARY_STRSTR(headerStr3, "Ext Control") == NULL)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr3, "Ext Control Column Expected Third");
                goto cleanup;
            }

            if (ADI_LIBRARY_STRSTR(headerStr4, "Phase Offset") == NULL)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr4, "Phase Offset Column Expected Fourth");
                goto cleanup;
            }

            if (ADI_LIBRARY_STRSTR(headerStr5, "Digital Gain") == NULL)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr5, "Digital Gain Column Expected Fifth");
                goto cleanup;
            }

            /* Put the part into streaming mode to help facilitate the large, contiguous SPI writes in CpuImageWrite */
            recoveryAction = adrv904x_SpiStreamingEntry(device);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with entering Spi Streaming mode");
                goto cleanup;
            }

            /* Loop until the gain table end is reached or no. of lines scanned exceeds maximum */
            while ((fgets(rxGainTableLineBuffer, sizeof(rxGainTableLineBuffer), rxGainTableFilePtr) != NULL) &&
                   (lineCount <  ADI_ADRV904X_RX_GAIN_TABLE_SIZE_ROWS))
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
                        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
                        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gainIndex, "Gain Indices not in Ascending Order");
                        goto cleanup;
                    }
                }

                prevGainIndex = gainIndex;
                lineCount++;
            }

            maxGainIndex = gainIndex;

            recoveryAction = adi_adrv904x_RxGainTableWrite( device,
                                                            (rxGainTableInfo[arrayIndex].channelMask & device->devStateInfo.initializedChannels),
                                                            maxGainIndex,
                                                            &rxGainTableRowBuffer[0U],
                                                            lineCount);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with RX Gain Table Writing");
                goto cleanup;
            }

            recoveryAction = adi_adrv904x_RxMinMaxGainIndexSet( device,
                                                                (rxGainTableInfo[arrayIndex].channelMask & device->devStateInfo.initializedChannels),
                                                                minGainIndex,
                                                                maxGainIndex);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with Setting Min/Max Gain Index");
                goto cleanup;
            }
        }
        else
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close CSV File");
        }
    }

    if (device->devStateInfo.spiStreamingOn == ADI_TRUE)
    {
        if (ADI_ADRV904X_ERR_ACT_NONE != adrv904x_SpiStreamingExit(device))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_INTERFACE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Spi Streaming Exit Issue");
        }
    }

    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuProfileImageLoad(adi_adrv904x_Device_t* const                        device,
                                                                  const adi_adrv904x_CpuProfileBinaryInfo_t* const    cpuProfileBinaryInfoPtr)
{
        const size_t                BIN_ELEMENT_SIZE    = 1U;
    adi_adrv904x_ErrAction_e    recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    FILE*                       cpuProfileFilePtr   = NULL;
    uint32_t                    totalFileSize       = 0U;
    uint32_t                    numFileChunks       = 0U;
    uint32_t                    chunkIndex          = 0U;
    uint32_t                    byteCount           = 0U;
    uint8_t                     cpuProfileBinaryImageBuffer[ADI_ADRV904X_CPU_PROFILE_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuProfileBinaryInfoPtr, cleanup);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuProfileBinaryInfoPtr->filePath, cleanup);

    if (ADI_LIBRARY_STRNLEN((const char*)cpuProfileBinaryInfoPtr->filePath, ADI_ADRV904X_MAX_FILE_LENGTH) == ADI_ADRV904X_MAX_FILE_LENGTH)
    {
        /* Path is not terminated */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileBinaryInfoPtr->filePath, "Unterminated path string");
        goto cleanup;
    }

    /* Open ARM binary file */
#ifdef __GNUC__
    cpuProfileFilePtr = ADI_LIBRARY_FOPEN((const char *)cpuProfileBinaryInfoPtr->filePath, "rb");
#else
    if (ADI_LIBRARY_FOPEN_S(&cpuProfileFilePtr, (const char *)cpuProfileBinaryInfoPtr->filePath, "rb") != 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileBinaryInfoPtr->filePath, "Unable to open CPU Profile Binary. Please check filepath is correct");
            goto cleanup;
        }
#endif

    if (NULL == cpuProfileFilePtr)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to open CPU Profile Binary. Please check filepath is correct")
        goto cleanup;
    }

    /* Determine file size */
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, 0U, SEEK_END) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Seek to EOF Failed");
        goto cleanup;
    }

    /* ADI_LIBRARY_FTELL returns long type */
    totalFileSize = (uint32_t) ADI_LIBRARY_FTELL(cpuProfileFilePtr);

    /* Check that FW binary file is not empty */
    if (0U == totalFileSize)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, totalFileSize, "Zero Length CPU Profile Binary Detected");
        goto cleanup;
    }

    /* Check that FW Profile binary file size is not smaller than server size */
    if (totalFileSize < ADRV904X_DEVICE_PROFILE_SIZE_BYTES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, totalFileSize, "CPU Profile Binary Size smaller than Server ADRV904X_DEVICE_PROFILE_SIZE_BYTES");
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ADRV904X_DEVICE_PROFILE_SIZE_BYTES, "ADRV904X_DEVICE_PROFILE_SIZE_BYTES bigger than CPU Profile Binary Size");
        goto cleanup;
    }
    
    /*check if the CPU Profile Binary Image Size is perfectly divisible by the chunk size*/
    if ((ADRV904X_DEVICE_PROFILE_SIZE_BYTES & 0x3U) != 0U) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Incorrect CPU Profile Binary Structure Size");
        goto cleanup;
    }

    /* Calculate the number of chunks to be written for radio profile */
    numFileChunks = (uint32_t)(ADRV904X_DEVICE_PROFILE_SIZE_BYTES / ADI_ADRV904X_CPU_PROFILE_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES) + 1;

    /* Rewind the file pointer to beginning of the file */
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, 0U, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    /* Put the part into streaming mode to help facilitate the large, contiguous SPI writes in CpuImageWrite */
    recoveryAction = adrv904x_SpiStreamingEntry(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with entering Spi Streaming mode");
        goto cleanup;
    }

    /* Read ARM binary file */
    for (chunkIndex = 0U; chunkIndex < numFileChunks; ++chunkIndex)
    {
        if ((byteCount = ADI_LIBRARY_FREAD(&cpuProfileBinaryImageBuffer[0U], BIN_ELEMENT_SIZE, ADI_ADRV904X_CPU_PROFILE_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES, cpuProfileFilePtr)) > 0)
        {
            /* In case the bin file has also the Init_t and PostMcsInit_t, keep the last chunkSize only the part belonging to profile */
            if (chunkIndex == (numFileChunks - 1U))
            {
                byteCount = ADRV904X_DEVICE_PROFILE_SIZE_BYTES - (chunkIndex) * ADI_ADRV904X_CPU_PROFILE_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES;
            }
            
            /* Write the CPU Profile binary chunk */
            recoveryAction = adi_adrv904x_CpuProfileWrite(  device,
                                                            (chunkIndex * ADI_ADRV904X_CPU_PROFILE_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES),
                                                            &cpuProfileBinaryImageBuffer[0U],
                                                            byteCount);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during CPU Profile Write");
                goto cleanup;
            }
        }
        else if (ferror(cpuProfileFilePtr))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close CPU Profile Binary File");
        }
    }

    if (device->devStateInfo.spiStreamingOn == ADI_TRUE)
    {
        if (ADI_ADRV904X_ERR_ACT_NONE != adrv904x_SpiStreamingExit(device))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_INTERFACE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Spi Streaming Exit Issue");
        }
    }

    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeviceInfoExtract(adi_adrv904x_Device_t* const                        device,
                                                                const adi_adrv904x_CpuProfileBinaryInfo_t* const    cpuProfileBinaryInfoPtr)
{
        adi_adrv904x_ErrAction_e        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    FILE*                           cpuProfileFilePtr   = NULL;
    uint32_t                        fileSize            = 0U;
    adrv904x_DeviceProfile_t* const deviceProfile       = NULL;
    uint32_t                        offset              = 0U;
    uint32_t                        readSize            = 0U;
    uint32_t                        idx                 = 0U;
    uint32_t                        bandIdx             = 0U;
    uint32_t                        targetIdx           = 0U;
    uint32_t                        digMask             = 0U;
    adi_adrv904x_InitExtract_t      InitExtractClear;
    adrv904x_TxConfig_t             txConfig;
    adrv904x_RxConfig_t             rxConfig;
    adrv904x_OrxConfig_t            orxConfig;
    adrv904x_JesdSettings_t         jesdProfile;

    uint32_t                                carrierIdx          = 0U;
    uint32_t                                minCarriersKhz      = UINT32_MAX;
    uint32_t                                minIfaceKhz         = ADRV904X_JESD_IQ_RATE_KHZ;
    adrv904x_DfeRadioSequencerConfig_t      radioSeqProfile; /*!<   Radio Sequencer configuration */
    adrv904x_CarrierDdcConfig_t             carrierDdcConfig;
    adrv904x_CarrierDucConfig_t             carrierDucConfig;
    adrv904x_DfeJesdSettings_t              dfeJesdProfile;
    adrv904x_TxDfeDpdConfig_t               dpdConfig;
    uint32_t                                txConfigOffset      = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuProfileBinaryInfoPtr, cleanup);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuProfileBinaryInfoPtr->filePath, cleanup);

    ADI_LIBRARY_MEMSET(&InitExtractClear, 0, sizeof(InitExtractClear));
    ADI_LIBRARY_MEMSET(&txConfig, 0, sizeof(txConfig));
    ADI_LIBRARY_MEMSET(&rxConfig, 0, sizeof(rxConfig));
    ADI_LIBRARY_MEMSET(&orxConfig, 0, sizeof(orxConfig));
    ADI_LIBRARY_MEMSET(&jesdProfile, 0, sizeof(jesdProfile));
    
        ADI_LIBRARY_MEMSET(&radioSeqProfile, 0, sizeof(radioSeqProfile));
    ADI_LIBRARY_MEMSET(&carrierDdcConfig, 0, sizeof(carrierDdcConfig));
    ADI_LIBRARY_MEMSET(&carrierDucConfig, 0, sizeof(carrierDucConfig));
    ADI_LIBRARY_MEMSET(&dfeJesdProfile, 0, sizeof(dfeJesdProfile));
    ADI_LIBRARY_MEMSET(&dpdConfig, 0, sizeof(dpdConfig));
    
    if (ADI_LIBRARY_STRNLEN((const char*)cpuProfileBinaryInfoPtr->filePath, ADI_ADRV904X_MAX_FILE_LENGTH) == ADI_ADRV904X_MAX_FILE_LENGTH)
    {
        /* Path is not terminated */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileBinaryInfoPtr->filePath, "Unterminated path string");
        goto cleanup;
    }

    /* Open ARM binary file */
#ifdef __GNUC__
    cpuProfileFilePtr = ADI_LIBRARY_FOPEN((const char *)cpuProfileBinaryInfoPtr->filePath, "rb");
#else
    if (ADI_LIBRARY_FOPEN_S(&cpuProfileFilePtr, (const char *)cpuProfileBinaryInfoPtr->filePath, "rb") != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileBinaryInfoPtr->filePath, "Unable to open CPU Profile Binary. Please check filepath is correct");
        goto cleanup;
    }
#endif

    if (NULL == cpuProfileFilePtr)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to open CPU Profile Binary. Please check filepath is correct")
        goto cleanup;
    }

    /* Determine file size */
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, 0U, SEEK_END) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Seek to EOF Failed");
        goto cleanup;
    }

    /* ADI_LIBRARY_FTELL returns long type */
    fileSize = (uint32_t) ADI_LIBRARY_FTELL(cpuProfileFilePtr);

    /* Check that FW binary file is not empty */
    if (0U == fileSize)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileSize, "Zero Length CPU Profile Binary Detected");
        goto cleanup;
    }

    /* Check that FW Profile binary file size is not smaller than server size */
    if (fileSize < ADRV904X_DEVICE_PROFILE_SIZE_BYTES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileSize, "CPU Profile Binary Size smaller than Server ADRV904X_DEVICE_PROFILE_SIZE_BYTES");
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ADRV904X_DEVICE_PROFILE_SIZE_BYTES, "ADRV904X_DEVICE_PROFILE_SIZE_BYTES bigger than CPU Profile Binary Size");
        goto cleanup;
    }

    /* Clear all variables */
    device->initExtract = InitExtractClear;

    /* Read scaled device clock frequency */
    offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, deviceClkScaledFreq_kHz);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((adrv904x_VariableFromFileExtract(device, &device->initExtract.clocks.deviceClockScaled_kHz, sizeof(deviceProfile->radioProfile.deviceClkScaledFreq_kHz), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read Pad Divider */
    offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, padDiv);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((adrv904x_VariableFromFileExtract(device, &device->initExtract.clocks.padDivideRatio, sizeof(deviceProfile->radioProfile.padDiv), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read armClkDiv */
    offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, armClkDiv);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((adrv904x_VariableFromFileExtract(device, &device->initExtract.clocks.armClkDivideRatio, sizeof(deviceProfile->radioProfile.armClkDiv), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read DevClk armClkDiv */
    offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, armClkDivDevClk);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((adrv904x_VariableFromFileExtract(device, &device->initExtract.clocks.armClkDevClkDivRatio, sizeof(deviceProfile->radioProfile.armClkDivDevClk), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read chanConfig */
    offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, chanConfig);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((adrv904x_VariableFromFileExtract(device, &device->initExtract.rx.rxInitChannelMask, sizeof(deviceProfile->radioProfile.chanConfig), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    device->initExtract.tx.txInitChannelMask = device->initExtract.rx.rxInitChannelMask & ADI_ADRV904X_TXALL;
    device->initExtract.rx.rxInitChannelMask = (device->initExtract.rx.rxInitChannelMask >> 8) & (ADI_ADRV904X_RX_MASK_ALL | ADI_ADRV904X_ORX_MASK_ALL);

    /* Read rxTxCpuConfig */
    offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, rxTxCpuConfig);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((adrv904x_VariableFromFileExtract(device, &device->initExtract.rxTxCpuConfig[0], sizeof(deviceProfile->radioProfile.rxTxCpuConfig), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read orxCpuConfig */
    offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, orxCpuConfig);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((adrv904x_VariableFromFileExtract(device, &device->initExtract.orxCpuConfig[0], sizeof(deviceProfile->radioProfile.orxCpuConfig), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read chanAssign */
    offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, chanAssign);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((adrv904x_VariableFromFileExtract(device, &device->initExtract.chanAssign[0], sizeof(deviceProfile->radioProfile.chanAssign), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read hsDigClk_kHz */
    offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, hsDigFreq_kHz);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((ADI_LIBRARY_FREAD(&device->initExtract.clocks.hsDigClk_kHz, sizeof(deviceProfile->radioProfile.hsDigFreq_kHz), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read feature mask */
    offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, featureMask);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((ADI_LIBRARY_FREAD(&device->initExtract.featureMask[0], sizeof(deviceProfile->radioProfile.featureMask), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    /* Read JESD Profile */
    offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, jesdProfile);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((ADI_LIBRARY_FREAD(&jesdProfile, sizeof(deviceProfile->radioProfile.jesdProfile), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }
    
    VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.rxdesQhfrate, jesdProfile.rxdesQhfrate);

    for (idx = 0U; idx < ADI_ADRV904X_MAX_FRAMERS; idx++)
    {
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerSetting[idx].iqRate_kHz, jesdProfile.framer[idx].iqRate_kHz) ;
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerSetting[idx].laneRate_kHz, jesdProfile.framer[idx].laneRate_kHz) ;
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerSetting[idx].jesdM, jesdProfile.framer[idx].M) ;
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerSetting[idx].jesdNp, jesdProfile.framer[idx].Np) ;
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerSetting[idx].serialLaneEnabled, jesdProfile.framer[idx].serializerLanesEnabled) ;
    }

    for (idx = 0U; idx < ADI_ADRV904X_MAX_DEFRAMERS; idx++)
    {
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.deframerSetting[idx].iqRate_kHz, jesdProfile.deframer[idx].iqRate_kHz) ;
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.deframerSetting[idx].laneRate_kHz, jesdProfile.deframer[idx].laneRate_kHz) ;
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.deframerSetting[idx].deserialLaneEnabled, jesdProfile.deframer[idx].deserializerLanesEnabled) ;
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.deframerSetting[idx].jesdM, jesdProfile.deframer[idx].M);
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.deframerSetting[idx].jesdNp, jesdProfile.deframer[idx].Np);
    }

    for (idx = 0U; idx < ADI_ADRV904X_MAX_FRAMERS_LS; idx++)
    {
        if (jesdProfile.linkSharingCfg[idx].linkSharingEnabled == ADI_ENABLE)
        {
            VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerLsSetting[idx].jesdM, jesdProfile.linkSharingCfg[idx].M) ;
            VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerLsSetting[idx].jesdNp, jesdProfile.linkSharingCfg[idx].Np) ;
        }
        else
        {
            device->initExtract.jesdSetting.framerLsSetting[idx].jesdM = 0;
            device->initExtract.jesdSetting.framerLsSetting[idx].jesdNp = 0;
        }
    }

    for (idx = 0U; idx < ADRV904X_JESD_MAX_LKSH_SAMPLE_XBAR_IDX; idx++)
    {
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerLsSetting[0].linkLsSampleXBar[idx], jesdProfile.linkSharingCfg[0].sampleXBar[idx]) ;
        VARIABLE_ASSIGNMENT(device->initExtract.jesdSetting.framerLsSetting[1].linkLsSampleXBar[idx], jesdProfile.linkSharingCfg[1].sampleXBar[idx]) ;
    }

    /* Read CPU ID that maps a deserializer lane to CPU0/1 for SERDES cals */
    for (idx = 0U; idx < ADI_ADRV904X_MAX_SERDES_LANES; idx++)
    {
        VARIABLE_ASSIGNMENT(device->initExtract.jesd204DesLaneCpuConfig[idx], jesdProfile.deserializerLane[idx].cpuId) ;
    }

    for (idx = 0U; idx < ADRV904X_NUM_TXRX_CHAN; idx++)
    {

        readSize = sizeof(rxConfig);
        switch (device->initExtract.chanAssign[idx])
        {
            case 0U:
                /* Read rxConfig[0]*/
                offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, rxConfig[0U]);
                break;

            case 1U:
                /* Read rxConfig[1]*/
                offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, rxConfig[1U]);
                break;
            case 2U:
                /* Read rxConfig[2]*/
                offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, rxConfig[2U]);
                break;
            case 3U:
                /* Read rxConfig[3]*/
                offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, rxConfig[3U]);
                break;
            case 255U:
                /* Channel disabled */
                offset = 0;
                break;
            default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
                goto cleanup;
            }

            if ((ADI_LIBRARY_FREAD(&rxConfig, readSize, 1, cpuProfileFilePtr)) <= 0)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
                goto cleanup;
            }

            VARIABLE_ASSIGNMENT(totalDecimationDDC1, rxConfig.rxddc[1].totalDecimation);
            VARIABLE_ASSIGNMENT(totalDecimationDDC0, rxConfig.rxddc[0].totalDecimation);
            VARIABLE_ASSIGNMENT(rxOutputRate_kHz, rxConfig.rxOutputRate_kHz);
            VARIABLE_ASSIGNMENT(rxBandEnbDDC0, rxConfig.rxddc[0].rxBandEnb);
            VARIABLE_ASSIGNMENT(rxBandEnbDDC1, rxConfig.rxddc[1].rxBandEnb);

            if (rxConfig.rxddc[1].rxBandEnb == 0U)
            {
                VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].rxDdc0OutputRate_kHz, rxConfig.rxOutputRate_kHz) ;
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
            
            for (bandIdx = 0U; bandIdx < ADI_ADRV904X_DDC_NUM_BAND; bandIdx++)
            {
                if (rxConfig.rxddc[bandIdx].rxBandEnb == 1U)
                {
                    uint32_t ncoFreqin_kHz = 0U;
                    uint32_t ibwCenterFreq_kHz = 0U;
                    uint32_t rfCenterFreq_kHz = 0U;
                    VARIABLE_ASSIGNMENT(ncoFreqin_kHz, rxConfig.rxddc[bandIdx].ncoFreqin_kHz);
                    VARIABLE_ASSIGNMENT(ibwCenterFreq_kHz, rxConfig.ibwCenterFreq_kHz);
                    VARIABLE_ASSIGNMENT(rfCenterFreq_kHz, rxConfig.rxddc[bandIdx].rfCenterFreq_kHz);
                    
                    device->initExtract.rx.rxChannelCfg[idx].bandSettings[bandIdx].enabled = ADI_TRUE;
                    VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].bandSettings[bandIdx].instBw_kHz, rxConfig.rxddc[bandIdx].instBw_kHz);
                    VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].bandSettings[bandIdx].rfCenterFreq_kHz, rxConfig.rxddc[bandIdx].rfCenterFreq_kHz);
                    device->initExtract.rx.rxChannelCfg[idx].bandSettings[bandIdx].sampleRate_kHz = device->initExtract.clocks.hsDigClk_kHz / (1U << rxConfig.rxddc[bandIdx].hb1OutputClkDiv);
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

            VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].rfBandwidth_kHz, rxConfig.ibw_kHz) ;
            VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].rxAdcSampleRate_kHz, rxConfig.adcClockRate_kHz) ;
            digMask = rxBandEnbDDC1;
            digMask <<= 1;
            digMask |= rxBandEnbDDC0;
            device->initExtract.rx.rxChannelCfg[idx].digChanMask = digMask;
        }
    }

    for (idx = 0U; idx < ADI_ADRV904X_MAX_ORX; idx++)
    {
        readSize = sizeof(orxConfig);
        switch (idx)
        {
        case 0U:
            offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, orxConfig[0U]);
            break;
        case 1U:
            offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, orxConfig[1U]);
            break;
#if ADI_ADRV904X_MAX_ORX > 2
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
            goto cleanup;
#endif
        }

        targetIdx = idx;

        if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
            goto cleanup;
        }

        if ((ADI_LIBRARY_FREAD(&orxConfig, readSize, 1, cpuProfileFilePtr)) <= 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
            goto cleanup;
        }

        VARIABLE_ASSIGNMENT(device->initExtract.orx.orxChannelCfg[targetIdx].rfBandwidth_kHz, orxConfig.sbw_kHz) ;
        VARIABLE_ASSIGNMENT(device->initExtract.orx.orxChannelCfg[targetIdx].orxOutputRate_kHz, orxConfig.orxOutputRate_kHz) ;
        VARIABLE_ASSIGNMENT(device->initExtract.orx.orxChannelCfg[targetIdx].orxAdcSampleRate_kHz, orxConfig.adcClockRate_kHz) ;
    }

    for (idx = 0U; idx < ADRV904X_NUM_TXRX_CHAN; idx++)
    {

        readSize = sizeof(txConfig);
        switch (device->initExtract.chanAssign[idx])
        {
            case 0U:
                /* Read txConfig[0] */
                offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, txConfig[0U]);
                break;

            case 1U:
                /* Read txConfig[1] */
                offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, txConfig[1U]);
                break;
            case 2U:
                /* Read txConfig[2] */
                offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, txConfig[2U]);
                break;
            case 3U:
                /* Read txConfig[3] */
                offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, txConfig[3U]);
                break;
            case 255U:
                /* Channel disabled */
                offset = 0U;
                break;
            default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
        }
        else
        {
            if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
                goto cleanup;
            }

            if ((ADI_LIBRARY_FREAD(&txConfig, readSize, 1, cpuProfileFilePtr)) <= 0)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
                goto cleanup;
            }

            VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].rfBandwidth_kHz,       txConfig.ibw_kHz) ;
            VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].totalDecimation,       txConfig.txduc[0].totalDecimation);
            VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].digChanMask,           txConfig.txduc[0].rxBandEnb);
            VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].txLbAdcSampleRate_kHz, txConfig.lpbkAdcClkRate_kHz);
            
            for (bandIdx = 0U; bandIdx < ADI_ADRV904X_DUC_NUM_BAND; bandIdx++)
            {
                if (txConfig.txduc[bandIdx].rxBandEnb == 1U)
                {
                    uint8_t tinClkDiv = 0U;
                    uint32_t ncoFreqin_kHz = 0U;
                    uint32_t ibwCenterFreq_kHz = 0U;
                    uint32_t rfCenterFreq_kHz = 0U;
                    VARIABLE_ASSIGNMENT(tinClkDiv, txConfig.txduc[bandIdx].tinClkDiv);
                    VARIABLE_ASSIGNMENT(ncoFreqin_kHz, txConfig.txduc[bandIdx].ncoFreqin_kHz);
                    VARIABLE_ASSIGNMENT(ibwCenterFreq_kHz, txConfig.ibwCenterFreq_kHz);
                    VARIABLE_ASSIGNMENT(rfCenterFreq_kHz, txConfig.txduc[bandIdx].rfCenterFreq_kHz);
                    
                    device->initExtract.tx.txChannelCfg[idx].bandSettings[bandIdx].enabled = ADI_TRUE;
                    VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].bandSettings[bandIdx].instBw_kHz, txConfig.txduc[bandIdx].instBw_kHz);
                    VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].bandSettings[bandIdx].rfCenterFreq_kHz, txConfig.txduc[bandIdx].rfCenterFreq_kHz);
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
            

            VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].pfirRate_kHz,          txConfig.txPfirClk_kHz) ;
            

            if (device->initExtract.tx.txChannelCfg[idx].totalDecimation > 8U)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
                goto cleanup;
            }
        }
    }

    /* DFE jesd settings */
    offset = ADI_LIBRARY_OFFSETOF(adrv904x_DeviceProfile_t, dfeProfile) +
             ADI_LIBRARY_OFFSETOF(adrv904x_DfeProfile_t, jesdProfile);

    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((ADI_LIBRARY_FREAD(&dfeJesdProfile, sizeof(dfeJesdProfile), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    
    for (idx = 0U; idx < ADI_ADRV904X_MAX_FRAMERS; idx++)
    {
        VARIABLE_ASSIGNMENT(device->initExtract.framerInterleaveMode[idx], dfeJesdProfile.framer[idx].jesd204cInterleave);
    }

    for (idx = 0U; idx < ADI_ADRV904X_MAX_DEFRAMERS; idx++)
    {
        VARIABLE_ASSIGNMENT(device->initExtract.deframerInterleaveMode[idx], dfeJesdProfile.deframer[idx].jesd204cInterleave);
    }
    
    /* Read radio sequence profile */
    offset = ADI_LIBRARY_OFFSETOF(adrv904x_DeviceProfile_t, dfeProfile) +
             ADI_LIBRARY_OFFSETOF(adrv904x_DfeProfile_t, radioSeqProfile);

    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((ADI_LIBRARY_FREAD(&radioSeqProfile, sizeof(radioSeqProfile), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    VARIABLE_ASSIGNMENT(device->initExtract.rs.radioSeqEn, radioSeqProfile.radioSeqEn) ;
    VARIABLE_ASSIGNMENT(device->initExtract.rs.sampleClkEn, radioSeqProfile.sampleClkEn) ;
    VARIABLE_ASSIGNMENT(device->initExtract.rs.sampleClkDiv, radioSeqProfile.sampleClkDiv) ;
    VARIABLE_ASSIGNMENT(device->initExtract.rs.ssbSyncMode, radioSeqProfile.ssbSyncMode) ;
    device->initExtract.rs.ssbSyncPeriod = ADRV904X_CTOHL(radioSeqProfile.ssbSyncPeriod);
    device->initExtract.rs.symWidth      = ADRV904X_CTOHS(radioSeqProfile.symWidth);
    device->initExtract.rs.extSymWidth   = ADRV904X_CTOHS(radioSeqProfile.extSymWidth);
    device->initExtract.rs.extSymSpacing = ADRV904X_CTOHS(radioSeqProfile.extSymSpacing);

    for (idx = 0U; idx < ADRV904X_NUM_TXRX_CHAN; idx++)
    {
        uint8_t carriersEnabled = 0U;
        
        switch (device->initExtract.chanAssign[idx])
        {
            case 0U:
                txConfigOffset = ADI_LIBRARY_OFFSETOF(adrv904x_DfeProfile_t, txConfig[0U]);
                break;
            case 1U:
                txConfigOffset = ADI_LIBRARY_OFFSETOF(adrv904x_DfeProfile_t, txConfig[1U]);
                break;
            case 2U:
                txConfigOffset = ADI_LIBRARY_OFFSETOF(adrv904x_DfeProfile_t, txConfig[2U]);
                break;
            case 3U:
                txConfigOffset = ADI_LIBRARY_OFFSETOF(adrv904x_DfeProfile_t, txConfig[3U]);
                break;
            case 255U:
                /* Channel disabled */
                txConfigOffset = 0U;
                break;
            default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Invalid chanAssign - cduc");
                goto cleanup;
                break;
        }
        
        offset = ADI_LIBRARY_OFFSETOF(adrv904x_DeviceProfile_t, dfeProfile) +
                 txConfigOffset +
                 ADI_LIBRARY_OFFSETOF(adrv904x_DfeTxConfig_t, cduc);

        if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary - carrier");
            goto cleanup;
        }

        if ((ADI_LIBRARY_FREAD(&carrierDucConfig, sizeof(carrierDucConfig), 1, cpuProfileFilePtr)) <= 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File - carrier");
            goto cleanup;
        }
        VARIABLE_ASSIGNMENT(device->initExtract.dfeCduc.cducOutputRate_kHz[idx], carrierDucConfig.carriers[0].inputRate_kHz);
        VARIABLE_ASSIGNMENT(carriersEnabled, carrierDucConfig.carriersEnabled);
        
        VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].bandRatio[0], carrierDucConfig.bandRatio[0]);
        VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].bandRatio[1], carrierDucConfig.bandRatio[1]);
        VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].maxSlot, carrierDucConfig.maxSlot);
        
        for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_CARRIERS; carrierIdx++)
        {
            if (((carriersEnabled) & (1U << carrierIdx)) > 0U) 
            {
                uint8_t bandSelect = 0U;
                int32_t ncoFreq_kHz = 0U;
                VARIABLE_ASSIGNMENT(bandSelect, carrierDucConfig.carriers[carrierIdx].bandSelect);
                VARIABLE_ASSIGNMENT(ncoFreq_kHz, carrierDucConfig.carriers[carrierIdx].ncoFreq_kHz);
                                
                device->initExtract.txCarrierConfigs[idx].carriers[carrierIdx].enable = ADI_ENABLE;
                VARIABLE_ASSIGNMENT(device->initExtract.txCarrierConfigs[idx].carriers[carrierIdx].sampleRate_kHz, carrierDucConfig.carriers[carrierIdx].inputRate_kHz);
                device->initExtract.txCarrierConfigs[idx].carriers[carrierIdx].centerFrequency_kHz = 
                    device->initExtract.tx.txChannelCfg[idx].bandSettings[bandSelect].rfCenterFreq_kHz + 
                    ncoFreq_kHz;
                VARIABLE_ASSIGNMENT(device->initExtract.txCarrierConfigs[idx].carriers[carrierIdx].ibw_kHz, carrierDucConfig.carriers[carrierIdx].ibw_kHz);
                
                VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].carrierRuntimeSettings.carrierEnable[carrierIdx], 1U);
                VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].carrierRuntimeSettings.bandSelect[carrierIdx], carrierDucConfig.carriers[carrierIdx].bandSelect);
                VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].carrierRuntimeSettings.mixerEnable[carrierIdx], carrierDucConfig.carriers[carrierIdx].mixerEnable);
                VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].carrierRuntimeSettings.decimationRatio[carrierIdx], carrierDucConfig.carriers[carrierIdx].decimationRatio);
                VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].carrierRuntimeSettings.interpolationRatio[carrierIdx], carrierDucConfig.carriers[carrierIdx].interpolationRatio);
                VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].carrierRuntimeSettings.ncoFreq_kHz[carrierIdx], carrierDucConfig.carriers[carrierIdx].ncoFreq_kHz);
                VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].carrierRuntimeSettings.carrierRateRatio[carrierIdx], carrierDucConfig.carriers[carrierIdx].carrierRateRatio);
                VARIABLE_ASSIGNMENT(device->initExtract.tx.txChannelCfg[idx].carrierRuntimeSettings.outputRate_kHz[carrierIdx], carrierDucConfig.carriers[carrierIdx].outputRate_kHz);
                
                if (device->initExtract.txCarrierConfigs[idx].carriers[carrierIdx].sampleRate_kHz < minCarriersKhz)
                {
                    minCarriersKhz = device->initExtract.txCarrierConfigs[idx].carriers[carrierIdx].sampleRate_kHz;
                }
            }
        }
        
        if (minCarriersKhz < minIfaceKhz)
        {
            device->initExtract.tx.txChannelCfg[idx].maxSlot /= (minIfaceKhz / minCarriersKhz);
        }

        /* DPD output profile */
        offset = ADI_LIBRARY_OFFSETOF(  adrv904x_DeviceProfile_t, dfeProfile) +
                                        txConfigOffset +
                                        ADI_LIBRARY_OFFSETOF(adrv904x_DfeTxConfig_t, dpd);

        if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary - dpd");
            goto cleanup;
        }

        if ((ADI_LIBRARY_FREAD(&dpdConfig, sizeof(dpdConfig), 1, cpuProfileFilePtr)) <= 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File - dpd");
            goto cleanup;
        }
        VARIABLE_ASSIGNMENT(device->initExtract.dfeCduc.dpdOutputRate_kHz[idx], dpdConfig.actRate_kHz);
        
        for (bandIdx = 0U; bandIdx < (uint32_t)ADI_ADRV904X_DUC_NUM_BAND; bandIdx++)
        {
            uint32_t bandLatencyOffset = 0U;
            switch (bandIdx)
            {
                case 0U:
                    bandLatencyOffset = ADI_LIBRARY_OFFSETOF(adrv904x_DfeTxConfig_t, bandDucLatency[0U]);
                    break;
                case 1U:
                    bandLatencyOffset = ADI_LIBRARY_OFFSETOF(adrv904x_DfeTxConfig_t, bandDucLatency[1U]);    
                    break;
                default:
                    recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Invalid bandIdx - tx duc");
                    goto cleanup;
                    break;
            }
            
            offset = ADI_LIBRARY_OFFSETOF(adrv904x_DeviceProfile_t, dfeProfile) +
                     txConfigOffset +
                     bandLatencyOffset;
                
            if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary - tx band duc latency");
                goto cleanup;
            }
            
            uint16_t tmpBandLatency = 0U;
            if ((ADI_LIBRARY_FREAD(&tmpBandLatency, sizeof(tmpBandLatency), 1, cpuProfileFilePtr)) <= 0)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File - carrier");
                goto cleanup;
            }
            
            VARIABLE_ASSIGNMENT(device->initExtract.txBandLatency[idx].duc_cc[bandIdx], tmpBandLatency);
        }
        
    }

    for (idx = 0U; idx < ADRV904X_NUM_TXRX_CHAN; idx++)
    {
        uint32_t rxConfigOffset;
        uint8_t carriersEnabled = 0U;

        switch (device->initExtract.chanAssign[idx])
        {
            case 0U:
                rxConfigOffset = ADI_LIBRARY_OFFSETOF(adrv904x_DfeProfile_t, rxConfig[0U]);
                break;
            case 1U:
                rxConfigOffset = ADI_LIBRARY_OFFSETOF(adrv904x_DfeProfile_t, rxConfig[1U]);
                break;
            case 2U:
                rxConfigOffset = ADI_LIBRARY_OFFSETOF(adrv904x_DfeProfile_t, rxConfig[2U]);
                break;
            case 3U:
                rxConfigOffset = ADI_LIBRARY_OFFSETOF(adrv904x_DfeProfile_t, rxConfig[3U]);
                break;
            case 255U:
                /* Channel disabled */
                rxConfigOffset = 0U;
                break;
            default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Invalid chanAssign - rx carrier");
                goto cleanup;
                break;
        }
        
        offset = ADI_LIBRARY_OFFSETOF(adrv904x_DeviceProfile_t, dfeProfile) +
                 rxConfigOffset +
                 ADI_LIBRARY_OFFSETOF(adrv904x_DfeRxConfig_t, cddc);
        
        if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary - carrier");
            goto cleanup;
        }

        if ((ADI_LIBRARY_FREAD(&carrierDdcConfig, sizeof(carrierDdcConfig), 1, cpuProfileFilePtr)) <= 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File - carrier");
            goto cleanup;
        }
        
        VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].bandRatio[0], carrierDdcConfig.bandRatio[0]);
        VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].bandRatio[1], carrierDdcConfig.bandRatio[1]);
        VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].maxSlot, carrierDdcConfig.maxSlot);
        
        VARIABLE_ASSIGNMENT(carriersEnabled, carrierDdcConfig.carriersEnabled);
        
        for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_CARRIERS; carrierIdx++)
        {
            if (((carrierDdcConfig.carriersEnabled) & (1U << carrierIdx)) > 0U)
            {
                device->initExtract.rxCarrierConfigs[idx].carriers[carrierIdx].enable = 1U;
                VARIABLE_ASSIGNMENT(device->initExtract.rxCarrierConfigs[idx].carriers[carrierIdx].sampleRate_kHz, carrierDdcConfig.carriers[carrierIdx].inputRate_kHz);
                device->initExtract.rxCarrierConfigs[idx].carriers[carrierIdx].centerFrequency_kHz = device->initExtract.rx.rxChannelCfg[idx].bandSettings[carrierDdcConfig.carriers[carrierIdx].bandSelect].rfCenterFreq_kHz + carrierDdcConfig.carriers[carrierIdx].ncoFreq_kHz;   //<<< TODO BOM TMP VARS
                VARIABLE_ASSIGNMENT(device->initExtract.rxCarrierConfigs[idx].carriers[carrierIdx].ibw_kHz, carrierDdcConfig.carriers[carrierIdx].ibw_kHz);
                
                VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].carrierRuntimeSettings.carrierEnable[carrierIdx], 1U);
                VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].carrierRuntimeSettings.bandSelect[carrierIdx], carrierDdcConfig.carriers[carrierIdx].bandSelect);
                VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].carrierRuntimeSettings.mixerEnable[carrierIdx], carrierDdcConfig.carriers[carrierIdx].mixerEnable);
                VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].carrierRuntimeSettings.decimationRatio[carrierIdx], carrierDdcConfig.carriers[carrierIdx].decimationRatio);
                VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].carrierRuntimeSettings.interpolationRatio[carrierIdx], carrierDdcConfig.carriers[carrierIdx].interpolationRatio);
                VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].carrierRuntimeSettings.ncoFreq_kHz[carrierIdx], carrierDdcConfig.carriers[carrierIdx].ncoFreq_kHz);
                VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].carrierRuntimeSettings.carrierRateRatio[carrierIdx], carrierDdcConfig.carriers[carrierIdx].carrierRateRatio);
                VARIABLE_ASSIGNMENT(device->initExtract.rx.rxChannelCfg[idx].carrierRuntimeSettings.outputRate_kHz[carrierIdx], carrierDdcConfig.carriers[carrierIdx].outputRate_kHz);
            }
        }
        
        for (bandIdx = 0U; bandIdx < (uint32_t)ADI_ADRV904X_DDC_NUM_BAND; bandIdx++)
        {
            uint32_t bandLatencyOffset = 0U;
            switch (bandIdx)
            {
                case 0U:
                    bandLatencyOffset = ADI_LIBRARY_OFFSETOF(adrv904x_DfeRxConfig_t, bandDdcLatency[0U]);
                    break;
                case 1U:
                    bandLatencyOffset = ADI_LIBRARY_OFFSETOF(adrv904x_DfeRxConfig_t, bandDdcLatency[1U]);    
                    break;
                default:
                    recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Invalid bandIdx - rx ddc");
                    goto cleanup;
                    break;
            }
            
            offset = ADI_LIBRARY_OFFSETOF(adrv904x_DeviceProfile_t, dfeProfile) +
                     rxConfigOffset +
                     bandLatencyOffset;
                
            if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary - rx band ddc latency");
                goto cleanup;
            }
            
            uint16_t tmpBandLatency = 0U;
            if ((ADI_LIBRARY_FREAD(&tmpBandLatency, sizeof(tmpBandLatency), 1, cpuProfileFilePtr)) <= 0)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File - rx band ddc latency");
                goto cleanup;
            }
            
            VARIABLE_ASSIGNMENT(device->initExtract.rxBandLatency[idx].ddc_cc[bandIdx], tmpBandLatency);
        }
        
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
    if (cpuProfileFilePtr != NULL)
    {
        /* Close CPU Profile binary file */
        if (0 != ADI_LIBRARY_FCLOSE(cpuProfileFilePtr))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close CPU Profile Binary File");
        }
    }
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeviceCopy(adi_adrv904x_Device_t* const    deviceSrc,
                                                         adi_adrv904x_Device_t* const    deviceDest)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(deviceSrc);
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(deviceDest);

    /* Ignore Common Device & SPI Settings */
    (void) ADI_LIBRARY_MEMCPY(&deviceDest->devStateInfo, &deviceSrc->devStateInfo, sizeof(adi_adrv904x_Info_t));
    (void) ADI_LIBRARY_MEMCPY(&deviceDest->initExtract, &deviceSrc->initExtract, sizeof(adi_adrv904x_InitExtract_t));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_PreMcsInit(adi_adrv904x_Device_t* const            device,
                                                         const adi_adrv904x_Init_t* const        init,
                                                         const adi_adrv904x_TrxFileInfo_t* const trxBinaryInfoPtr)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, init, cleanup);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, trxBinaryInfoPtr, cleanup);

    /* Prevent any Read Operation from device during Broadcast */
    ADI_ADRV904X_WRONLY_SET(device->common, ADI_TRUE);

    /* Extract Info from CPU Profile Binary */
    recoveryAction = adi_adrv904x_DeviceInfoExtract(device, &trxBinaryInfoPtr->cpuProfile);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during CPU Profile Binary Image Extract");
        goto cleanup;
    }

    /* Initialize TRX Device */
    recoveryAction = adi_adrv904x_Initialize(device, init);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TRX Device Initialization Failed");
        goto cleanup;
    }

    /* Load Stream Binary Image */
    recoveryAction = adi_adrv904x_StreamImageLoad(device, &trxBinaryInfoPtr->stream);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Stream Binary Image Loading");
        goto cleanup;
    }

    /* Load CPU Binary File(s) */
    recoveryAction = adi_adrv904x_CpuImageLoad(device, &trxBinaryInfoPtr->cpu);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during CPU Binary Image Loading");
        goto cleanup;
    }

    /* Prepare for DFE CPU Binary Loading */
    recoveryAction = adi_adrv904x_DfePrepareLoad(device, 0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during DFE CPU prepare for load.");
        goto cleanup;
    }

    /* Load CPU Profile Binary */
    recoveryAction = adi_adrv904x_CpuProfileImageLoad(device, &trxBinaryInfoPtr->cpuProfile);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during CPU Profile Binary Image Loading");
        goto cleanup;
    }

    /* Load Rx Gain Table(s) */
    recoveryAction = adi_adrv904x_RxGainTableLoad(device, &trxBinaryInfoPtr->rxGainTable[0U], ADI_ADRV904X_LOAD_ALL_RXGAIN_TABLES);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during RX Gain Table Loading");
        goto cleanup;
    }

        /* Load Images for all the DFE CPU cores*/
    if (ADI_LIBRARY_STRNLEN((const char*)trxBinaryInfoPtr->dfeCpu.filePath, ADI_ADRV904X_MAX_FILE_LENGTH) != 0)
    {
        recoveryAction = adi_adrv904x_DfeCpuImageLoad(device, &trxBinaryInfoPtr->dfeCpu);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during DFE CPU image load.");
            goto cleanup;
        }
    }

    /* Load Radio Sequence Binary Image */
    if (device->initExtract.rs.radioSeqEn == ADI_TRUE)
    {
        recoveryAction = adi_adrv904x_RadioSequencerImageLoad(device, &trxBinaryInfoPtr->radioSequencer);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Radio Sequence Binary Image Loading");
            goto cleanup;
        }
    }

    /* Read CFR pulse from profile binary and write it to pulse RAMs */
    recoveryAction = adi_adrv904x_CfrPulseExtractAndWrite(device, &trxBinaryInfoPtr->cpuProfile);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error happened while writing CFR pulse");
        goto cleanup;
    }

        /* CPU Bootup */
    recoveryAction = adi_adrv904x_CpuStart(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during TRX CPU Start");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_WRONLY_SET(device->common, ADI_FALSE);
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_PreMcsInit_NonBroadcast(adi_adrv904x_Device_t* const        device,
                                                                      const adi_adrv904x_Init_t* const    init)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_RxDataFormatRt_t rxDataFormatRt;
    adi_adrv904x_RxGain_t rxGain;
    uint32_t i            = 0u;
    uint32_t rxChannels[] = {
        (uint32_t)ADI_ADRV904X_RX0,
        (uint32_t)ADI_ADRV904X_RX1,
        (uint32_t)ADI_ADRV904X_RX2,
        (uint32_t)ADI_ADRV904X_RX3,
        (uint32_t)ADI_ADRV904X_RX4,
        (uint32_t)ADI_ADRV904X_RX5,
        (uint32_t)ADI_ADRV904X_RX6,
        (uint32_t)ADI_ADRV904X_RX7,
        (uint32_t)ADI_ADRV904X_ORX0,
        (uint32_t)ADI_ADRV904X_ORX1
    };

    adi_adrv904x_TxAtten_t                  txAtten;
    adi_adrv904x_TxAttenCfg_t               txAttenCfg;
    adi_adrv904x_PowerMonitorCfgRt_t        txPowerMonitorCfg;
    adi_adrv904x_SlewRateDetectorCfgRt_t    txSrlCfg;
    adi_adrv904x_ProtectionRampCfgRt_t      txProtectionRampCfg;
    uint8_t txInputInterpolation = 0U;
    uint32_t chanMask = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, init, cleanup);

    ADI_LIBRARY_MEMSET(&txAtten, 0, sizeof(txAtten));
    ADI_LIBRARY_MEMSET(&txAttenCfg, 0, sizeof(txAttenCfg));
    ADI_LIBRARY_MEMSET(&txPowerMonitorCfg, 0, sizeof(txPowerMonitorCfg));
    ADI_LIBRARY_MEMSET(&txSrlCfg, 0, sizeof(txSrlCfg));
    ADI_LIBRARY_MEMSET(&txProtectionRampCfg, 0, sizeof(txProtectionRampCfg));
    ADI_LIBRARY_MEMSET(&rxDataFormatRt, 0, sizeof(rxDataFormatRt));
    ADI_LIBRARY_MEMSET(&rxGain, 0, sizeof(rxGain));

    recoveryAction = adi_adrv904x_CpuStartStatusCheck(device, ADI_ADRV904X_GETCPUBOOTUP_TIMEOUT_US);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Boot Issue");
        goto cleanup;
    }

#if ADI_ADRV904X_SPI_VERIFY
    recoveryAction = adi_adrv904x_SpiVerify(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }
#endif

    recoveryAction = adi_adrv904x_DeviceRevGet(device, &(device->devStateInfo.deviceSiRev));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read device silicon revision");
        goto cleanup;
    }

    if (device->devStateInfo.profilesValid & ADI_ADRV904X_TX_PROFILE_VALID)
    {
        /* For each tx channel */
        for (i = 0U; i < ADI_ADRV904X_MAX_TXCHANNELS; ++i)
        {
            chanMask = 1U << i;
            if (ADRV904X_BF_EQUAL(device->initExtract.tx.txInitChannelMask, chanMask) == 0)
            {
                /* Channel disabled - skip */
                continue;
            }

            /* Tx channel is enabled */
            txAttenCfg = init->tx.txChannelCfg[i].txAttenCfg;

            recoveryAction = adi_adrv904x_TxAttenCfgSet(device, chanMask, &txAttenCfg);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx Atten Configuration Failed");
                goto cleanup;
            }

            txAtten.txChannelMask = chanMask;
            txAtten.txAttenuation_mdB = init->tx.txChannelCfg[i].txAttenInit_mdB;

            recoveryAction = adi_adrv904x_TxAttenSet(device, &txAtten, 1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_RESET_FEATURE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx Initial Attenuation Set Failed");
                    goto cleanup;
                }
            }

            txInputInterpolation = (uint8_t)device->initExtract.tx.txChannelCfg[i].totalDecimation;

            recoveryAction = adrv904x_TxPowerMonitorInitialize(device, chanMask, txInputInterpolation);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx Power Monitor Initialize Failed");
                goto cleanup;
            }

            txPowerMonitorCfg.txChannelMask   = chanMask;
            txPowerMonitorCfg.txPowerMonitorCfg = init->tx.txChannelCfg[i].txpowerMonitorCfg;

            recoveryAction = adi_adrv904x_TxPowerMonitorCfgSet(device, &txPowerMonitorCfg, 1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx Power Protection Configuration Failed");
                goto cleanup;
            }

            txSrlCfg.txChannelMask = chanMask;
            txSrlCfg.srlCfg = init->tx.txChannelCfg[i].srlCfg;

            recoveryAction = adi_adrv904x_TxSlewRateDetectorCfgSet(device, &txSrlCfg, 1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx Slew Rate Detector Configuration Failed");
                goto cleanup;
            }

            txProtectionRampCfg.txChannelMask = chanMask;
            recoveryAction = adi_adrv904x_TxProtectionRampCfgSet(device, &txProtectionRampCfg, 1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx Protection Ramp Configuration Failed");
                goto cleanup;
            }

            /* Store PA protection ramp down config and handle this part in the post MCS step */
            device->devStateInfo.txProtectionRampCfg[i].txChannelMask = chanMask;
            device->devStateInfo.txProtectionRampCfg[i].protectionRampCfg = init->tx.txChannelCfg[i].protectionRampCfg;
            
            if (device->devStateInfo.vswrWaveformLoaded[i] == 0U)
            {
                recoveryAction = adrv904x_VswrPlaybackDatNumOfSamplesInit(device, (adi_adrv904x_TxChannels_e)chanMask);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx VSWR playback init failed");
                    goto cleanup;
                }
            }
        }
    }

    /* Initializing RX configuration */
    if (device->devStateInfo.profilesValid & ADI_ADRV904X_RX_PROFILE_VALID)
    {
        for (i = 0U; i < (sizeof(rxChannels) / sizeof(rxChannels[0U])); ++i)
        {
            if ((device->initExtract.rx.rxInitChannelMask & rxChannels[i]) > 0U)
            {
                /* Correct the Rx Channel Mask from the profile */
                rxDataFormatRt.rxChannelMask = rxChannels[i];
                rxDataFormatRt.rxDataFormat = init->rx.rxChannelCfg[i].rxDataFormat;

                /* Setup the Rx data formatter */
                recoveryAction = adrv904x_RxDataFormatSet(device, &rxDataFormatRt, 1U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RX Data Format set Failed");
                    goto cleanup;
                }

                /* Setup Rx AGC overload protection registers */
                recoveryAction = adrv904x_RxOverloadProtectionSet(device, (adi_adrv904x_RxChannels_e)rxChannels[i]);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RX Overload Protection set Failed");
                    goto cleanup;
                }

                if (i < ADI_ADRV904X_MAX_RX_ONLY)
                {
                    /* Correct the Rx Channel Mask from the profile */
                    rxGain.rxChannelMask = rxChannels[i];
                    rxGain.gainIndex = init->rx.rxChannelCfg[i].rxGainIndexInit;

                    /* Setup the Rx Gain */
                    recoveryAction = adi_adrv904x_RxGainSet(device, &rxGain, 1U);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RX Gain set Failed");
                        goto cleanup;
                    }
                }
            }
        }
    }

    /* Initializing Radio Sequence configuration */
    ADI_LIBRARY_MEMCPY(&device->initExtract.radioCtrlPreInit,
                       &init->radioCtrlPreInit,
                       sizeof(device->initExtract.radioCtrlPreInit));
    recoveryAction = adi_adrv904x_RadioSequencerPreMcsCfg(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Radio Sequence configuration Failed");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_PostMcsInit(adi_adrv904x_Device_t* const            device,
                                                          const adi_adrv904x_PostMcsInit_t* const utilityInit)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const uint32_t INIT_CALS_TIMEOUT_MS = 60000U;   /* 60 Seconds Timeout */
    adi_adrv904x_InitCalErrData_t initCalErrData;
    uint8_t idx = 0U;
    adi_adrv904x_RadioCtrlTxRxEnCfg_t allDisabledTxRxEnCfg;
    

    uint8_t i = 0U;
    uint32_t chanMask = 0U;
    adi_adrv904x_GpIntMask_t gpIntClear = { 0ULL, 0ULL };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, utilityInit, cleanup);

    ADI_LIBRARY_MEMSET(&initCalErrData, 0, sizeof(adi_adrv904x_InitCalErrData_t));
    ADI_LIBRARY_MEMSET(&allDisabledTxRxEnCfg, 0, sizeof(allDisabledTxRxEnCfg));


    /* Initializing Radio Sequence configuration */
    recoveryAction = adi_adrv904x_RadioSequencerPostMcsCfg(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Radio Sequence configuration Failed");
        goto cleanup;
    }

    /*Initialize radio control. This is required to run before running init cals*/
    /* Sets up Radio Ctrl mode for Rx/ORx/Tx signal chains (SPI vs Pin mode) */
    if ((utilityInit->radioCtrlCfg.txRadioCtrlModeCfg.txChannelMask     != 0U) ||
        (utilityInit->radioCtrlCfg.rxRadioCtrlModeCfg.rxChannelMask     != 0U) ||
        (utilityInit->radioCtrlCfg.orxRadioCtrlModeCfg.orxChannelMask   != 0U))
    {
        recoveryAction = adi_adrv904x_RadioCtrlCfgSet(device, (adi_adrv904x_RadioCtrlModeCfg_t*) &utilityInit->radioCtrlCfg);
        if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Radio Control Config Set Failed");
            goto cleanup;
        }
    }

    /* Sets up Radio Ctrl TxRx Enable Config for Pin mode (Config will be ignore in SPI mode) */
    if ((utilityInit->radioCtrlTxRxEnCfgSel != 0U) || (utilityInit->radioCtrlTxRxEnPinSel != 0U))
    {
        recoveryAction = adi_adrv904x_RadioCtrlTxRxEnCfgSet(device,
                                                            &allDisabledTxRxEnCfg,
                                                            ADI_ADRV904X_TXRXEN_PINALL,
                                                            ADI_ADRV904X_TXRXEN_TX_ENABLE_MAP | ADI_ADRV904X_TXRXEN_RX_ENABLE_MAP);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Radio Control TxRxEnCfg set to default failed");
            goto cleanup;
        }
        
        recoveryAction = adi_adrv904x_RadioCtrlTxRxEnCfgSet(device,
                                                            &utilityInit->radioCtrlGpioCfg,
                                                            utilityInit->radioCtrlTxRxEnPinSel,
                                                            utilityInit->radioCtrlTxRxEnCfgSel);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Radio Control TxRxEnCfg Set Failed");
            goto cleanup;
        }
    }

    /* Sets up the GPIO pin mapping used by stream processor */
    recoveryAction = adi_adrv904x_StreamGpioConfigSet(device, &device->devStateInfo.streamGpioMapping);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Stream Gpio Config Set Failed");
        goto cleanup;
    }
  
    /* Sets up the Tx to Orx pin mapping */
    recoveryAction = adi_adrv904x_TxToOrxMappingInit(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx To Orx Mapping Init Failed");
        goto cleanup;
    }

    /* Initialize GP Interrupt settings for use during device initialization*/
    /* Set GP Int Pin Masks to selected states */
    recoveryAction = adi_adrv904x_GpIntPinMaskCfgSet(device, ADI_ADRV904X_GPINTALL, &utilityInit->gpIntPostInit);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing GP Interrupt Mask Array.");
        goto cleanup;
    }

    /* Clear all status bits */
    recoveryAction = adi_adrv904x_GpIntStatusClear(device, NULL);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
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
        recoveryAction = adi_adrv904x_InitCalsRun(device, &utilityInit->initCals);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "InitCalsRun Failed");
            goto cleanup;
        }

        recoveryAction = adi_adrv904x_InitCalsWait_v2(device, INIT_CALS_TIMEOUT_MS, &initCalErrData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Initial Calibration Wait Issue");
            goto cleanup;
        }

        for (idx = 0; idx < ADI_ADRV904X_NUM_INIT_CAL_CHANNELS; ++idx)
        {
            if (initCalErrData.channel[idx].errCode != 0)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "InitCals Error - Get More Information via Detailed Status Get");
                goto cleanup;
            }
        }
    }


    /* Config Tx PA protection ramp down */
    if (device->devStateInfo.profilesValid & ADI_ADRV904X_TX_PROFILE_VALID)
    {
        /* Clear the Tx PA GP Int */
        gpIntClear.lowerMask = 0x3FFC00ULL;
        gpIntClear.upperMask = 0x1C7FFFC00ULL;
        recoveryAction = adi_adrv904x_GpIntStatusClear(device, &gpIntClear);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GP INT Tx PA protection bit clear failed");
            goto cleanup;
        }

        /* For each tx channel */
        for (i = 0U; i < ADI_ADRV904X_MAX_TXCHANNELS; ++i)
        {
            chanMask = 1U << i;
            if (ADRV904X_BF_EQUAL(device->initExtract.tx.txInitChannelMask, chanMask) == 0)
            {
                /* Channel disabled - skip */
                continue;
            }

            /* Clear the Tx PA error event */
            recoveryAction = adi_adrv904x_TxProtectionErrorClear(device,
                                                                 (adi_adrv904x_TxChannels_e)chanMask,
                                                                 (uint32_t)(ADI_ADRV904X_RDT_PEAK_PA |
                                                                            ADI_ADRV904X_RDT_AVG_PA |
                                                                            ADI_ADRV904X_RDT_SRD |
                                                                            ADI_ADRV904X_RDT_CLK_PLL_UNLOCK |
                                                                            ADI_ADRV904X_RDT_RF0_PLL_UNLOCK |
                                                                            ADI_ADRV904X_RDT_RF1_PLL_UNLOCK |
                                                                            ADI_ADRV904X_RDT_SERDES_PLL_UNLOCK |
                                                                            ADI_ADRV904X_RDT_DFRM0_EVENT |
                                                                            ADI_ADRV904X_RDT_DFRM1_EVENT));
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanMask, "Tx PA protection event clear failed");
                goto cleanup;
            }

            recoveryAction = adi_adrv904x_TxProtectionRampCfgSet(device, &(device->devStateInfo.txProtectionRampCfg[i]), 1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanMask, "Tx Protection Ramp Configuration Failed");
                goto cleanup;
            }
        }
    }

    if (device->devStateInfo.devState & ADI_ADRV904X_STATE_DFECPULOADED)
    {
        recoveryAction = adi_adrv904x_DfeCpuStart(device);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE processor issue during start.");
            goto cleanup;
        }
        /* Check DFE processor boot status */
        recoveryAction = adi_adrv904x_DfeCpuStartStatusCheck(device, ADI_ADRV904X_DFE_CPUBOOTUP_TIMEOUT_US);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE CPU Boot Issue");
            goto cleanup;
        }
    }


    recoveryAction = adi_adrv904x_JrxRepairInitialization(device, utilityInit->initCals.warmBoot);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Jrx repair at initialization failed");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

static adi_adrv904x_ErrAction_e adrv904x_CpuMemDumpBinWrite(adi_adrv904x_Device_t* const                        device,
                                                            const adi_adrv904x_CpuMemDumpBinaryInfo_t* const    cpuMemDumpBinaryInfoPtr,
                                                            const uint8_t                                       forceException,
                                                            uint32_t* const                                     dumpSize,
                                                            const uint8_t                                       ramOnlyFlag)
{
    adi_adrv904x_ErrAction_e    recoveryAction                                    = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    FILE*                       ofp                                               = NULL;
    uint32_t                    byteCount                                         = 0U;
    uint32_t                    cpuIdx                                            = 0U;
    uint32_t                    exceptionValue                                    = 0U;
    uint8_t                     binaryRead[ADI_ADRV904X_MEM_DUMP_CHUNK_SIZE + 10] = { 0U };
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
    adrv904x_BfTxDigChanAddr_e  txDigBaseAddr[ADI_ADRV904X_MAX_TXCHANNELS] = { ADRV904X_BF_SLICE_TX_0__TX_DIG, ADRV904X_BF_SLICE_TX_1__TX_DIG, ADRV904X_BF_SLICE_TX_2__TX_DIG, ADRV904X_BF_SLICE_TX_3__TX_DIG, ADRV904X_BF_SLICE_TX_4__TX_DIG, ADRV904X_BF_SLICE_TX_5__TX_DIG, ADRV904X_BF_SLICE_TX_6__TX_DIG, ADRV904X_BF_SLICE_TX_7__TX_DIG };

    uint32_t recordCrc      = 0U;
    uint8_t  recordPadding  = 0U;


    uint32_t          offset                       = 0U;
    uint32_t          sizeFlag                     = 0U;
    const uint32_t    cpuCrashAddr[]               = { 0U }; /* Skip these addresses that causes CPU crash  */
    uint8_t           skipRead                     = ADI_FALSE;
    uint32_t          dfeTableSize                 = 0U;
    uint32_t          dfeExceptionFlag             = ADRV904X_DFE_PLATFORM_NO_EXCEPTION;
    uint32_t          count                        = 0U;
    uint8_t           bootStatus                   = 0U;
    const uint32_t    DFE_EXCEPTION_DONE_MAX_COUNT = 500U;
    const uint32_t    DFE_EXCEPTION_DONE_WAIT_US   = 1000U;
    const uint32_t    SPI_ONLY_REGS_ADDR           = 0x47000000U;
    const uint32_t    DIRECT_SPI_REGION_LEN        = 0x4000U;

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
    uint8_t  driverVersionPrerel[ADI_ADRV904X_VERSION_PREREL_SIZE] = {0U};
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


    /* ETM Trace Record Variables */
    uint32_t traceBufferLength = 0U;
    uint16_t traceFormat;
    uint8_t  numCpus = 0U;
    uint16_t cpuArch;
    uint16_t etmType;
    uint16_t cpuFormat;
    uint8_t  configLength;
    uint32_t traceDataLength;
    uint32_t etmOn[ADRV904X_ETM_MAX] = {0U};
    uint32_t read32 = 0U;

    /* Firmware Version Records */
    uint16_t firmwareSemverMajor;
    uint16_t firmwareSemverMinor;
    uint16_t firmwareSemverPatch;
    uint8_t  firmwareVersionPrerel[ADI_ADRV904X_VERSION_PREREL_SIZE] = {0U};
    uint32_t firmwareSemverBuild;
    uint32_t firmwareId;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuMemDumpBinaryInfoPtr, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuMemDumpBinaryInfoPtr->filePath, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dumpSize, cleanup);

    if ((forceException != 0U) &&
        (forceException != 1U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ofp, "Unable to open binary image file. Please check if the path is correct");
        goto cleanup;
    }


    /* Check ETM Values before exceptions */
    for(i = 0; i < ADRV904X_ETM_MAX; i++)
    {
        recoveryAction = adi_adrv904x_Register32Read(   device,
                                                        NULL,
                                                        adrv904x_DfeEtmMemDumpTable[i].baseAddr + ADRV904X_ETM_TRCPRGCTLR_OFFSET,
                                                        &read32,                    /* trace enabled for ETM*/
                                                        0xFFFFFFFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ofp, "Unable to Read trace memory");
            goto cleanup;
        }

        if(read32 != 0U)
        {
            etmOn[i] = 1U;
            numCpus++;
            configLength = adrv904x_DfeEtmMemDumpTable[i].configSize * adrv904x_DfeEtmMemDumpTable[i].dataSize;
            traceBufferLength += ADI_ADRV904X_ETM_RECORD_SIZE + configLength + ((configLength % 8) ? 8 - (configLength % 8) : 0);
        }
    }
    
    if (forceException == ADI_TRUE)
    {

                /* Poll until CPU is in exception state or timeout */
        timeout_us = ADI_ADRV904X_WRITECPUEXCEPTION_TIMEOUT_US;
        waitInterval_us = ADI_ADRV904X_WRITECPUEXCEPTION_INTERVAL_US;
#if ADI_ADRV904X_WRITECPUEXCEPTION_INTERVAL_US > ADI_ADRV904X_WRITECPUEXCEPTION_TIMEOUT_US
        waitInterval_us = timeout_us;
#elif ADI_ADRV904X_WRITECPUEXCEPTION_INTERVAL_US == 0
        waitInterval_us = timeout_us;
#endif
        numEventChecks = timeout_us / waitInterval_us;
        for (eventCheck = 0U; eventCheck <= numEventChecks; ++eventCheck)
        {
            if (exceptionValue == (uint32_t)ADRV904X_CPU_NO_EXCEPTION)
            {
                /* For each main CPU force an exception unless it is already in exception state */
                for (cpuIdx = ADI_ADRV904X_CPU_TYPE_0; cpuIdx < ADI_ADRV904X_CPU_TYPE_MAX_RADIO; cpuIdx++)
                {
                    recoveryAction = adi_adrv904x_CpuCheckException(device, (uint32_t*)&exceptionValue);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to Read Cpu memory");
                        goto cleanup;
                    }
                    if (exceptionValue == (uint32_t)ADRV904X_CPU_NO_EXCEPTION)
                    {
                        /* CPU is not in exception state; Force it into exception state */
                        recoveryAction = adrv904x_CpuForceException(device, (adi_adrv904x_CpuType_e)cpuIdx);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CpuForceException issue");
                            goto cleanup;
                        }
                        recoveryAction = adi_adrv904x_CpuCheckException(device, (uint32_t*)&exceptionValue);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to Read Cpu memory");
                            goto cleanup;
                        }
                        if (exceptionValue == (uint32_t)ADRV904X_CPU_NO_EXCEPTION)
                        {
                            recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, waitInterval_us);
                            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                            {
                                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Request Issue");
                                goto cleanup;
                            }
                        }
                        else
                        {
                            /* CPU is now in exception state so break out of the cpuIdx for loop*/
                            break;
                        }
                    }
                }
            }
            /* Force a DFE exception if no DFE core is currently in exception state */
            if (dfeExceptionFlag == (uint32_t)ADRV904X_DFE_PLATFORM_NO_EXCEPTION)
            {
                /* Read the exception data */
                recoveryAction = adi_adrv904x_DfeCpuCheckException(device, (uint32_t*)&dfeExceptionFlag);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE exception data registers");
                    goto cleanup;
                }
                if (dfeExceptionFlag == (uint32_t)ADRV904X_DFE_PLATFORM_NO_EXCEPTION)
                {
                    /* No DFE core is in exception state; Force them into exception state. */
                    recoveryAction = adrv904x_CpuForceException(device, ADI_ADRV904X_CPU_TYPE_DFE);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CpuForceException issue for DFE CPU");
                        goto cleanup;
                    }
                    /* Read the address of exception data  */
                    recoveryAction = adi_adrv904x_DfeCpuCheckException(device, (uint32_t*)&dfeExceptionFlag);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE exception data registers");
                        goto cleanup;
                    }
                    if (dfeExceptionFlag == (uint32_t)ADRV904X_DFE_PLATFORM_NO_EXCEPTION)
                    {
                        recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, waitInterval_us);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Request Issue for DFE CPU");
                            goto cleanup;
                        }
                    }
                }
            }
            if ((exceptionValue != (uint32_t)ADRV904X_CPU_NO_EXCEPTION) && (dfeExceptionFlag != (uint32_t)ADRV904X_DFE_PLATFORM_NO_EXCEPTION))
            {
                /* Break out of the eventCheck for loop */
                break;
            }
        }
    }

    /* Enable the Tx slice clock of the uninitialized TX channels to avoid the invalid memdump */
    sliceClkEn = 0x01U;
    for (txChanId = 0U; txChanId <= ADI_ADRV904X_TX_CHAN_ID_MAX; txChanId++)
    {
        if (((device->devStateInfo.initializedChannels >> ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET) & (0x01 << (uint32_t)txChanId)) == 0U)
        {
            adrv904x_TxDig_TxAttenClkEnable_BfSet(device, NULL, txDigBaseAddr[txChanId], sliceClkEn);
        }
    }
    
    *dumpSize = 0U;

    /* Generate Header for dump file*/
    fileFormatVersion  = 0x1U;
    productRevision     = device->devStateInfo.deviceSiRev;
    /* Determine product ID */
    adrv904x_Core_EfuseProductId_BfGet(device, NULL, ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS, (uint8_t*) &productId);
    productId = ADI_ADRV904X_PRODUCT_ID_MASK | productId;
    /* Find dump time */
    ADI_LIBRARY_TIME(&rawtime);
    tmPtr = ADI_LIBRARY_GMTIME(&rawtime);

    /* Generate header record*/
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    byteCount = 0U;
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], fileFormatVersion);
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], productId);
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], productRevision);
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], tmPtr->tm_year);
    byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], tmPtr->tm_mon);
    byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], tmPtr->tm_mday);
    byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], tmPtr->tm_hour);
    byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], tmPtr->tm_min);
    byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], tmPtr->tm_sec);
    byteCount += 3U;                                                                /* Padding to align to 64-bit boundary */
    /* calculate CRC*/
    recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_TRUE);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordCrc);
    byteCount += 4U;                                                                /* Padding to align to 64-bit boundary */

    /* Insert file header */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }

    /* Generate Device Driver Record */
    recordCrc = 0U;
    byteCount = 0U;
    recordType = ADI_ADRV904X_MEMDUMP_DEV_DRVR_STATE_RECORD;
    recordLength = ADI_ADRV904X_MEMDUMP_DEVICE_DRIVER_HEADER_SIZE + sizeof(*device);
    /* Generate basic header */
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordType);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordLength);
    /* Insert record header */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    byteCount = 0U;

    /* Get driver record values */
    driverId = 0U; /* TBD */
    instanceId = device->common.id;
    driverStateLength = sizeof(*device);
    driverVersionMajor = ADI_ADRV904X_CURRENT_MAJOR_VERSION;
    driverVersionMinor = ADI_ADRV904X_CURRENT_MINOR_VERSION;
    driverVersionPatch = ADI_ADRV904X_CURRENT_MAINTENANCE_VERSION;
    driverVersionBuild = ADI_ADRV904X_CURRENT_BUILD_VERSION;

    /* Only Koror enables init time */
    tmPtr = ADI_LIBRARY_GMTIME(&(device->devStateInfo.initGlobalTime));
    tmYear  =  tmPtr->tm_year;
    tmMonth =  tmPtr->tm_mon;
    tmDay   =  tmPtr->tm_mday;
    tmHour  =  tmPtr->tm_hour;
    tmMin   =  tmPtr->tm_min;
    tmSec   =  tmPtr->tm_sec;

    /* Compile driver record header */
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], driverId);
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], instanceId);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], driverStateLength);
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], driverVersionMajor);
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], driverVersionMinor);
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], driverVersionPatch);
    for(i = 0U; i < ADI_ADRV904X_VERSION_PREREL_SIZE; i++)
    {
        byteCount += adrv904x_insert8Bits(&binaryRead[byteCount], driverVersionPrerel[i]);
    }
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], driverVersionBuild);
    byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], endianness);
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], tmYear);
    byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], tmMonth);
    byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], tmDay);
    byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], tmHour);
    byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], tmMin);
    byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], tmSec);

    /* Publish driver header to file */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_FALSE);

    /* Begin publishing Device Handle */
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    byteCount = 0U;
    driverPtr = (uint8_t*) device;
    for(i = 0U; i < driverStateLength; i++)
    {
        if(byteCount >= ADI_ADRV904X_MEM_DUMP_CHUNK_SIZE) 
        {
            /* If at maximum length for buffer, flush */
            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }
            recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_FALSE);
            byteCount = 0U;
        }
        /* Set buffer to data and increment ptr */
        byteCount += adrv904x_insert8Bits(&binaryRead[byteCount], *(driverPtr++));
    }
    /* Flush remaining data, even if smaller than chunk size */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    /* Calculate final crc */
    recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_TRUE);
    
    /* Append CRC32 and padding*/
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    recordPadding = (recordLength % 8U) ? 8U - (recordLength % 8U) : 0U; 
    adrv904x_insert32Bits(&binaryRead[recordPadding], recordCrc);
    byteCount = 8U + recordPadding;
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }

    /* Begin Radio FW Record */
    recordCrc = 0U;
    byteCount = 0U;
    recordType = ADI_ADRV904X_MEMDUMP_FIRMWARE_VER_RECORD;
    recordLength = ADI_ADRV904X_MEMDUMP_FIRMWARE_VERSION_HEADER_SIZE;

    /* Generate basic header */
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordType);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordLength);
    /* Insert record header */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    byteCount = 0U;

    /* Get driver record values */
    firmwareSemverMajor = ADI_ADRV904X_CURRENT_MAJOR_VERSION;
    firmwareSemverMinor = ADI_ADRV904X_CURRENT_MINOR_VERSION;
    firmwareSemverPatch = ADI_ADRV904X_CURRENT_MAINTENANCE_VERSION;
    firmwareSemverBuild = ADI_ADRV904X_CURRENT_BUILD_VERSION;
    firmwareId = ADI_ADRV904X_MEMDUMP_RADIO_FW << ADI_ADRV904X_FIRMWARE_TYPE_ID_SHIFT;

    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], firmwareSemverMajor);
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], firmwareSemverMinor);
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], firmwareSemverPatch);
    for(i = 0U; i < ADI_ADRV904X_VERSION_PREREL_SIZE; i++)
    {
        byteCount += adrv904x_insert8Bits(&binaryRead[byteCount], firmwareVersionPrerel[i]);
    }
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], firmwareSemverBuild);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], firmwareId);

    /* Publish firmware record to file */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_TRUE);

    /* Append CRC32 and padding*/
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    recordPadding = (recordLength % 8U) ? 8U - (recordLength % 8U) : 0U; 
    adrv904x_insert32Bits(&binaryRead[recordPadding], recordCrc);
    byteCount = 8U + recordPadding;
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }


    /* Begin DFE FW Record */
    recordCrc = 0U;
    byteCount = 0U;
    recordType = ADI_ADRV904X_MEMDUMP_FIRMWARE_VER_RECORD;
    recordLength = ADI_ADRV904X_MEMDUMP_FIRMWARE_VERSION_HEADER_SIZE;

    /* Generate basic header */
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordType);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordLength);
    /* Insert record header */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    byteCount = 0U;

    /* Get driver record values */
    firmwareSemverMajor = device->devStateInfo.cpu.fwVersion.majorVer;
    firmwareSemverMinor = device->devStateInfo.cpu.fwVersion.minorVer;
    firmwareSemverPatch = device->devStateInfo.cpu.fwVersion.maintenanceVer;
    firmwareSemverBuild = device->devStateInfo.cpu.fwVersion.buildVer;
    firmwareId = ADI_ADRV904X_MEMDUMP_DFE_FW << ADI_ADRV904X_FIRMWARE_TYPE_ID_SHIFT;

    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], firmwareSemverMajor);
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], firmwareSemverMinor);
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], firmwareSemverPatch);
    for(i = 0U; i < ADI_ADRV904X_VERSION_PREREL_SIZE; i++)
    {
        byteCount += adrv904x_insert8Bits(&binaryRead[byteCount], firmwareVersionPrerel[i]);
    }
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], firmwareSemverBuild);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], firmwareId);

    /* Publish firmware record to file */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_TRUE);

    /* Append CRC32 and padding*/
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    recordPadding = (recordLength % 8U) ? 8U - (recordLength % 8U) : 0U; 
    adrv904x_insert32Bits(&binaryRead[recordPadding], recordCrc);
    byteCount = 8U + recordPadding;
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }

    /* Begin Stream FW Record */
    recordCrc = 0U;
    byteCount = 0U;
    recordType = ADI_ADRV904X_MEMDUMP_FIRMWARE_VER_RECORD;
    recordLength = ADI_ADRV904X_MEMDUMP_FIRMWARE_VERSION_HEADER_SIZE;

    /* Generate basic header */
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordType);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordLength);
    /* Insert record header */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    byteCount = 0U;

    /* Get driver record values */
    firmwareSemverMajor = device->devStateInfo.cpu.devProfileVersion.majorVer;
    firmwareSemverMinor = device->devStateInfo.cpu.devProfileVersion.minorVer;
    firmwareSemverPatch = device->devStateInfo.cpu.devProfileVersion.maintenanceVer;
    firmwareSemverBuild = device->devStateInfo.cpu.devProfileVersion.buildVer;
    firmwareId = ADI_ADRV904X_MEMDUMP_STREAM_FW << ADI_ADRV904X_FIRMWARE_TYPE_ID_SHIFT;

    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], firmwareSemverMajor);
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], firmwareSemverMinor);
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], firmwareSemverPatch);
    for(i = 0U; i < ADI_ADRV904X_VERSION_PREREL_SIZE; i++)
    {
        byteCount += adrv904x_insert8Bits(&binaryRead[byteCount], firmwareVersionPrerel[i]);
    }
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], firmwareSemverBuild);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], firmwareId);

    /* Publish firmware record to file */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_TRUE);

    /* Append CRC32 and padding*/
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    recordPadding = (recordLength % 8U) ? 8U - (recordLength % 8U) : 0U; 
    adrv904x_insert32Bits(&binaryRead[recordPadding], recordCrc);
    byteCount = 8U + recordPadding;
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }


    /* Begin trace record */
    recordCrc = 0U;
    byteCount = 0U;

    /* Find Trace ETF buffer size */
    recoveryAction = adi_adrv904x_Register32Read(   device,
                                                    NULL,
                                                    ADRV904X_A55_TRACE_FIFO_ADDR_BASE + ADRV904X_ETF_RSZ_OFFSET,
                                                    &read32,                    /* ETF Buffer Size / 4 */
                                                    0xFFFFFFFFU);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to Read ETM memory");
        goto cleanup;
    }
    traceDataLength = (read32 << ADI_ADRV904X_ETF_RAM_SIZE_OFFSET);

    /* Generate basic header */
    recordType = ADI_ADRV904X_MEMDUMP_ETM_TRACE_RECORD;
    recordLength = ADI_ADRV904X_MEMDUMP_TRACE_BUFFER_HEADER_SIZE + traceBufferLength + traceDataLength;

    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordType);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordLength);

    /* Insert record header */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    byteCount = 0U;

    /* Get remaining trace record values */
    traceFormat = ADI_ADRV904X_TRACE_FORMAT_CORESIGHT;
    endianness  = 0U;

    /* Compile trace record header */
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], traceBufferLength);
    byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], traceFormat);
    byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], numCpus);
    byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], endianness);
    /* Flush data */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_FALSE);
    byteCount = 0U;

    /* Begin loop through all relevant trace-generating devices */
    for(i = 0U; i < ADRV904X_ETM_MAX; i++)
    {
        if(etmOn[i] == ADI_TRUE)
        {
            cpuArch = ADI_ADRV904X_MEMDUMP_ARM_V8;
            
            recoveryAction = adi_adrv904x_Register32Read(   device,
                                                            NULL,
                                                            adrv904x_DfeEtmMemDumpTable[i].baseAddr + ADRV904X_ETM_TRCIDR1_OFFSET,
                                                            &read32,
                                                            ADI_ADRV904X_TRCIDR1_ARCH_MASK);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to Read ETM memory");
                goto cleanup;
            }
            etmType = read32 >> ADI_ADRV904X_TRCIDR1_ARCH_SHIFT;
            cpuFormat = adrv904x_DfeEtmMemDumpTable[i].format;
            configLength = adrv904x_DfeEtmMemDumpTable[i].configSize * adrv904x_DfeEtmMemDumpTable[i].dataSize;
            endianness  = 0U;

            byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], cpuArch);
            byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], etmType);
            byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], cpuFormat);
            byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], configLength);
            byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], endianness);

            /* Flush data */
            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }
            recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_FALSE);

            adrv904x_dumpTraceCpuData(  device, 
                                        &ofp,
                                        adrv904x_DfeEtmMemDumpTable[i].baseAddr, 
                                        &recordCrc);
            
            byteCount = 0U;
        }
    }

    /* Insert trace data size */
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], traceDataLength);
    /* Flush data */
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }
    recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_FALSE);
    byteCount = 0U;

    /* Insert trace data */
    traceDataLength = traceDataLength >> ADI_ADRV904X_ETF_RAM_SIZE_OFFSET;
    for(i = 0U; i < (traceDataLength); i++)
    {
        /* Read data in 4B incements*/
        recoveryAction = adi_adrv904x_Register32Read(   device,
                                                        NULL,
                                                        ADRV904X_A55_TRACE_FIFO_ADDR_BASE + ADRV904X_ETF_RRD_OFFSET,
                                                        &read32,
                                                        0xFFFFFFFFU);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to Read ETM memory");
            goto cleanup;
        }
        byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], read32);
        
        /* Flush data if next read would exceed chunk size */
        if((byteCount + sizeof(uint32_t)) > ADI_ADRV904X_MEM_DUMP_CHUNK_SIZE)
        {
            /* Flush data */
            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }
            recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_FALSE);
            ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                                  /* Set binary to 0's */
            byteCount = 0U;
        }
    }
    /* Push remaining data */
    if(byteCount != 0U)
    {
        /* Flush data */
        if (ADI_LIBRARY_FWRITE(binaryRead,
            1,
            byteCount,
            ofp) != byteCount)
        {
            recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
            goto cleanup;
        }
    }
    recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_TRUE);

    /* Append CRC32 and padding*/
    ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                                          /* Set binary to 0's */
    recordPadding = (recordLength % 8U) ? 8U - (recordLength % 8U) : 0U; 
    adrv904x_insert32Bits(&binaryRead[recordPadding], recordCrc);
    byteCount = 8U + recordPadding;
    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        goto cleanup;
    }

    /* Loop through the radio CPU RAM dump table a write memory to file */
    cpuTableSize = sizeof(adrv904x_CpuRamMemDumpTable) / sizeof(adrv904x_CpuRamMemDump_t);
    if(ramOnlyFlag == 1U && cpuTableSize > ADI_ADRV904X_VRAM_ONLY_SIZE)
    {
        cpuTableSize = ADI_ADRV904X_VRAM_ONLY_SIZE;
    }

    for (tableIdx = 0U; tableIdx < cpuTableSize; tableIdx++)
    {

        startAddr = adrv904x_CpuRamMemDumpTable[tableIdx].cpuMemAddr;
        endAddr = adrv904x_CpuRamMemDumpTable[tableIdx].cpuMemAddr + adrv904x_CpuRamMemDumpTable[tableIdx].cpuMemSize - 1;

        /* Generate CPU RAM Record */
        recordCrc = 0U;
        byteCount = 0U;
        ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */

        /* Insert basic record header */
        recordType = ADI_ADRV904X_MEMDUMP_CPU_RAM_RECORD;
        recordLength = ADI_ADRV904X_MEMDUMP_CPU_RAM_HEADER_SIZE + adrv904x_CpuRamMemDumpTable[tableIdx].cpuMemSize;
        byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordType);
        byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordLength);
        if (ADI_LIBRARY_FWRITE(binaryRead,
            1,
            byteCount,
            ofp) != byteCount)
        {
            recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
            goto cleanup;
        }

        /* Reset values for CPU record header */
        byteCount = 0U;
        ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */

        /* Construct CPU RAM record header */
        startAddress    = startAddr;
        memLength       = adrv904x_CpuRamMemDumpTable[tableIdx].cpuMemSize;
        ramId           = adrv904x_CpuRamMemDumpTable[tableIdx].ramId;
        endianness      = adrv904x_CpuRamMemDumpTable[tableIdx].cpuMemEndianness;
        if(startAddr >= STREAM_CPU_RAM_ADDR)
        {
            cpuType = ADI_ADRV904X_MEMDUMP_STREAM_CPU;
        }
        else
        {
            cpuType = ADI_ADRV904X_MEMDUMP_ARM_V7;
        }
        byteCount += adrv904x_insert64Bits(&binaryRead[byteCount], startAddress);
        byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], memLength);
        byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], ramId);
        byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], cpuType);
        byteCount += adrv904x_insert8Bits (&binaryRead[byteCount], endianness);
        byteCount += 7U; /* Padding */

        /* Insert CPU RAM record header */
        if (ADI_LIBRARY_FWRITE(binaryRead,
            1,
            byteCount,
            ofp) != byteCount)
        {
            recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
            goto cleanup;
        }
        recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_FALSE);
        byteCount = 0U;

        recoveryAction = adrv904x_dumpMemoryRegion(  device, 
                                    &ofp,
                                    startAddr, 
                                    endAddr, 
                                    dumpSize,
                                    &recordCrc);

        if(recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write CRC to a binary file.");
            goto cleanup;
        }

        /* Append CRC32 and padding*/
        ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
        recordPadding = (recordLength % 8U) ? 8U - (recordLength % 8U) : 0U; 
        adrv904x_insert32Bits(&binaryRead[recordPadding], recordCrc);
        byteCount = 8U + recordPadding;
        if (ADI_LIBRARY_FWRITE(binaryRead,
            1,
            byteCount,
            ofp) != byteCount)
        {
            recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
            goto cleanup;
        }

    }
    byteCount = 0U;
    recordCrc = 0U;

    /* Loop through the radio cpu telemetry dump table a write memory to file */
    cpuTableSize = sizeof(adrv904x_TelemetryMemDumpTable) / sizeof(adrv904x_CpuMemDump_t);
    if(ramOnlyFlag == 0U)
    {
        for (tableIdx = 0U; tableIdx < cpuTableSize; tableIdx++)
        {

            startAddr = adrv904x_TelemetryMemDumpTable[tableIdx].cpuMemAddr;
            endAddr = adrv904x_TelemetryMemDumpTable[tableIdx].cpuMemAddr + adrv904x_TelemetryMemDumpTable[tableIdx].cpuMemSize - 1;

            /* Generate Telemetry Buffer Record */
            recordCrc = 0U;
            byteCount = 0U;
            ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */

            /* Insert basic record header */
            recordType = ADI_ADRV904X_MEMDUMP_TELEM_RECORD;
            recordLength = ADI_ADRV904X_MEMDUMP_TELEM_BUFFER_HEADER_SIZE + adrv904x_TelemetryMemDumpTable[tableIdx].cpuMemSize;
            byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordType);
            byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordLength);
            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }

            /* Reset values for telemetry buffer record header */
            byteCount = 0U;
            ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */

            /* Construct Telemetry record header*/
            bufferLength    = adrv904x_TelemetryMemDumpTable[tableIdx].cpuMemSize;
            bufferId        = ADI_ADRV904X_PRODUCT_ID_MASK;
            endianness      = adrv904x_TelemetryMemDumpTable[tableIdx].cpuMemEndianness;

            byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], bufferLength);
            byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], bufferId);
            byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], formatVersion);
            byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], endianness);
            byteCount += 7U; /* Padding */

            /* Insert telemetry buffer record header */
            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }
            recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_FALSE);
            byteCount = 0U;

            recoveryAction = adrv904x_dumpMemoryRegion(  device, 
                                        &ofp,
                                        startAddr, 
                                        endAddr, 
                                        dumpSize,
                                        &recordCrc);

            if(recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write CRC to a binary file.");
                goto cleanup;
            }
            /* Append CRC32 and padding*/
            ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
            recordPadding = (recordLength % 8U) ? 8U - (recordLength % 8U) : 0U; 
            adrv904x_insert32Bits(&binaryRead[recordPadding], recordCrc);
            byteCount = 8U + recordPadding;
            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }
        }
    }
    byteCount = 0U;
    recordCrc = 0U;

    /* Loop through the radio cpu memory dump table a write memory to file */
    cpuTableSize = sizeof(adrv904x_CpuMemDumpTable) / sizeof(adrv904x_CpuMemDump_t);
    if(ramOnlyFlag == 0U)
    {
        for (tableIdx = 0U; tableIdx < cpuTableSize; tableIdx++)
        {

            startAddr = adrv904x_CpuMemDumpTable[tableIdx].cpuMemAddr;
            endAddr = adrv904x_CpuMemDumpTable[tableIdx].cpuMemAddr + adrv904x_CpuMemDumpTable[tableIdx].cpuMemSize - 1;

            /* Generate Register Record */
            recordCrc = 0U;
            byteCount = 0U;
            ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */

            /* Insert basic record header */
            recordType   = ADI_ADRV904X_MEMDUMP_REG_RECORD;
            recordLength = ADI_ADRV904X_MEMDUMP_REGISTER_HEADER_SIZE + adrv904x_CpuMemDumpTable[tableIdx].cpuMemSize;
            byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordType);
            byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordLength);
            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }

            /* Reset values for register record header */
            byteCount = 0U;
            ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
            

            /* Construct Register record header*/
            startAddress    = adrv904x_CpuMemDumpTable[tableIdx].cpuMemAddr;
            length          = adrv904x_CpuMemDumpTable[tableIdx].cpuMemSize;
            registerWidth   = adrv904x_CpuMemDumpTable[tableIdx].cpuMemWidth;
            endianness      = adrv904x_CpuMemDumpTable[tableIdx].cpuMemEndianness;

            byteCount += adrv904x_insert64Bits(&binaryRead[byteCount], startAddress);
            byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], length);
            byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], registerWidth);
            byteCount += adrv904x_insert8Bits( &binaryRead[byteCount], endianness);
            byteCount += 2U; /* Padding */

            /* Insert register buffer record header */
            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }
            recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_FALSE);
            byteCount = 0U;

            recoveryAction = adrv904x_dumpMemoryRegion(  device, 
                                        &ofp,
                                        startAddr, 
                                        endAddr, 
                                        dumpSize,
                                        &recordCrc);

            if(recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write CRC to a binary file.");
                goto cleanup;
            }

            /* Append CRC32 and padding*/
            ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */
            recordPadding = (recordLength % 8U) ? 8U - (recordLength % 8U) : 0U; 
            adrv904x_insert32Bits(&binaryRead[recordPadding], recordCrc);
            byteCount = 8U + recordPadding;
            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }
        }
    }
    byteCount = 0U;


    while ((forceException == 1U) && (count < DFE_EXCEPTION_DONE_MAX_COUNT))
    {
        /* Get the CPU boot status */
        recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
            NULL,
            ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
            ADRV904X_DFE_BOOT_STATUS_SCRATCH_REG_ID,
            (uint8_t*) &bootStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "bootStatus read failed");
            goto cleanup;
        }
        if (bootStatus == ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_EXCEPTION_DONE)
        {
            break;
        }
        else
        {
            count++;
            recoveryAction = (adi_adrv904x_ErrAction_e)adi_common_hal_Wait_us(&device->common, DFE_EXCEPTION_DONE_WAIT_US);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Request Issue");
                goto cleanup;
            }
        }
    }
    /* Loop through the dfe memory dump table a write dfe memory to file */
    dfeTableSize = sizeof(adrv904x_DfeMemDumpTable) / sizeof(adrv904x_CpuRamMemDump_t);
    for (tableIdx = 0U; tableIdx < dfeTableSize; tableIdx++)
    {
        startAddr = adrv904x_DfeMemDumpTable[tableIdx].cpuMemAddr;
        endAddr = adrv904x_DfeMemDumpTable[tableIdx].cpuMemAddr + adrv904x_DfeMemDumpTable[tableIdx].cpuMemSize - 1;
        /* Generate CPU RAM Record */
        recordCrc = 0U;
        byteCount = 0U;
        ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */

        /* Insert basic record header */
        recordType = ADI_ADRV904X_MEMDUMP_CPU_RAM_RECORD;
        recordLength = ADI_ADRV904X_MEMDUMP_CPU_RAM_HEADER_SIZE + adrv904x_DfeMemDumpTable[tableIdx].cpuMemSize;
        byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordType);
        byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], recordLength);
        if (ADI_LIBRARY_FWRITE(binaryRead,
            1,
            byteCount,
            ofp) != byteCount)
        {
            recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
            goto cleanup;
        }

        /* Reset values for DFE RAM record header */
        byteCount = 0U;
        ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));                          /* Set binary to 0's */

        /* Construct DFE RAM record header */
        startAddress    = startAddr;
        memLength       = adrv904x_DfeMemDumpTable[tableIdx].cpuMemSize;
        ramId           = adrv904x_DfeMemDumpTable[tableIdx].ramId;
        cpuType         = ADI_ADRV904X_MEMDUMP_ARM_V8;
        endianness      = adrv904x_DfeMemDumpTable[tableIdx].cpuMemEndianness;
        byteCount += adrv904x_insert64Bits(&binaryRead[byteCount], startAddress);
        byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], memLength);
        byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], ramId);
        byteCount += adrv904x_insert16Bits(&binaryRead[byteCount], cpuType);
        byteCount += adrv904x_insert8Bits (&binaryRead[byteCount], endianness);
        byteCount += 7U; /* Padding */

        /* Insert DFE RAM record header */
        if (ADI_LIBRARY_FWRITE(binaryRead,
            1,
            byteCount,
            ofp) != byteCount)
        {
            recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
            goto cleanup;
        }
        recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_FALSE);
        byteCount = 0U;

        if (startAddr == 0)
        {
            break; /* last entry */
        }

        /* if the cpuMemSize = 1, set the sizeFlag as 1 to ensure enter the below loop  */
        if (startAddr == endAddr)
        {
            sizeFlag = 1;
        }

        for (offset = startAddr; ((offset < endAddr) || (1 == sizeFlag)); offset += ADI_ADRV904X_MEM_DUMP_CHUNK_SIZE)
        {
            /* Clean the flag for waiting for next cpuMemSize = 1 */
            sizeFlag = 0;
            if (offset < (endAddr - ADI_ADRV904X_MEM_DUMP_CHUNK_SIZE))
            {
                byteCount = ADI_ADRV904X_MEM_DUMP_CHUNK_SIZE;
            }
            else
            {
                byteCount = endAddr + 1 - offset;
            }

            *dumpSize += byteCount;


            skipRead = ADI_FALSE;
            ADI_LIBRARY_MEMSET(binaryRead, 0, sizeof(binaryRead));

            for (i = 0; i < sizeof(cpuCrashAddr) / sizeof(uint32_t); i++)
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
                        recoveryAction = adi_adrv904x_Register32Read(device, NULL, offset + i, &read32, 0xFF);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset + i, "Unable to Read dfe memory");
                            goto cleanup;
                        }
                        binaryRead[i] = (uint8_t)read32;
                    }
                }
                else
                {
                    recoveryAction = adi_adrv904x_Registers32Read(device, NULL, offset, (uint32_t *)binaryRead, NULL, (byteCount + 3) / 4);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Unable to Read dfe memory");
                        goto cleanup;
                    }
                }
            }

            if (ADI_LIBRARY_FWRITE(binaryRead,
                1,
                byteCount,
                ofp) != byteCount)
            {
                recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
                goto cleanup;
            }

            if (offset >= endAddr - ADI_ADRV904X_MEM_DUMP_CHUNK_SIZE)
            {
                recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_TRUE);
            }
            else
            {
                recordCrc = adrv904x_Crc32ForChunk(binaryRead, byteCount, recordCrc, ADI_FALSE);
            }
        }
        /* Append CRC32 and padding*/
        ADI_LIBRARY_MEMSET(binaryRead, 0U, sizeof(binaryRead));                          /* Set binary to 0's */
        recordPadding = (recordLength % 8U) ? 8U - (recordLength % 8U) : 0U; 
        adrv904x_insert32Bits(&binaryRead[recordPadding], recordCrc);
        byteCount = 8U + recordPadding;
        if (ADI_LIBRARY_FWRITE(binaryRead,
            1,
            byteCount,
            ofp) != byteCount)
        {
            recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
            goto cleanup;
        }
    }

cleanup:
    /*Close ARM binary file*/
    if (ofp != NULL)
    {
        if (ADI_LIBRARY_FCLOSE(ofp) < 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to close a binary file. Possible memory shortage while flushing / other I/O errors.");
        }
    }
    
    /* Disable the Tx slice clock of the uninitialized TX channels to retrieve the normal status before memory dump */
    sliceClkEn = 0x00U;
    for (txChanId = 0U; txChanId <= ADI_ADRV904X_TX_CHAN_ID_MAX; txChanId++)
    {
        if (((device->devStateInfo.initializedChannels >> ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET) & (0x01 << (uint32_t)txChanId)) == 0U)
        {
            adrv904x_TxDig_TxAttenClkEnable_BfSet(device, NULL, txDigBaseAddr[txChanId], sliceClkEn);
        }
    }

    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
static uint32_t adrv904x_insert64Bits(  uint8_t*            array,
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
static uint32_t adrv904x_insert32Bits(  uint8_t*            array,
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
static uint32_t adrv904x_insert16Bits(  uint8_t*            array,
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
static uint32_t adrv904x_insert8Bits(   uint8_t*            array,
                                        uint8_t            storeVariable)
{
    if(array == NULL)
    {
        return 0U;                                          /* Return number of Bytes stored */
    }
    *(array) = storeVariable & 0xFFU;
    return 1U;                                              /* Return number of Bytes stored */
}

static adi_adrv904x_ErrAction_e adrv904x_dumpMemoryRegion(  adi_adrv904x_Device_t* const device, 
                                                            FILE **ofp,
                                                            uint32_t startAddr, 
                                                            uint32_t endAddr, 
                                                            uint32_t* const dumpSize,
                                                            uint32_t *recordCrc)
{
    adi_adrv904x_ErrAction_e  recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t sizeFlag  = 0U;
    uint32_t offset    = 0U;
    uint32_t byteCount = 0U;
    uint32_t read32    = 0U;
    uint32_t i         = 0U;
    uint8_t  skipRead  = ADI_FALSE;
    uint8_t  binaryRead[ADI_ADRV904X_MEM_DUMP_CHUNK_SIZE + 10] = { 0U };
    const uint32_t cpuCrashAddr[] = { 0U }; /* Skip these addresses that causes CPU crash  */
    const uint32_t              SPI_ONLY_REGS_ADDR                                = 0x47000000U;
    const uint32_t              DIRECT_SPI_REGION_LEN                             = 0x4000U;
    const uint32_t              HARDWARE_SEMAPHORE_ADDR                           = 0x46500000U;
    const uint32_t              HARDWARE_SEMAPHORE_LEN                            = 0x200000U;

    if (startAddr == 0)
    {
        return ADI_ADRV904X_ERR_ACT_NONE; /* last entry */
    }

    /* if the cpuMemSize = 1, set the sizeFlag as 1 to ensure enter the below loop  */
    if (startAddr == endAddr)
    {
        sizeFlag = 1U;
    }

    for (offset = startAddr; ((offset < endAddr) || (1 == sizeFlag)); offset += ADI_ADRV904X_MEM_DUMP_CHUNK_SIZE)
    {
        /* Clean the flag for waiting for next cpuMemSize = 1 */
        sizeFlag = 0U;
        if (offset < (endAddr - ADI_ADRV904X_MEM_DUMP_CHUNK_SIZE))
        {
            byteCount = ADI_ADRV904X_MEM_DUMP_CHUNK_SIZE;
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
                    recoveryAction = adi_adrv904x_Register32Read(device, NULL, offset + i, &read32, 0xFF);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset + i, "Unable to Read Cpu memory");
                        return recoveryAction;
                    }
                    binaryRead[i] = (uint8_t)read32;
                }
            }
            else
            {
                recoveryAction = adi_adrv904x_Registers32Read(device, NULL, offset, (uint32_t *)binaryRead, NULL, (byteCount + 3) / 4);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
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
                    recoveryAction = adi_adrv904x_Registers32Write(device, NULL, &offset, (uint32_t *)binaryRead, NULL, (byteCount + 3) / 4);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
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
            recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
            return recoveryAction;
        }
        /* Add the CRC value at the end of Radio side memory dump */
        if  (offset >= endAddr - ADI_ADRV904X_MEM_DUMP_CHUNK_SIZE)
        {
            *recordCrc  = adrv904x_Crc32ForChunk(binaryRead, byteCount, *recordCrc, ADI_TRUE);
        } 
        else 
        {
            *recordCrc  = adrv904x_Crc32ForChunk(binaryRead, byteCount, *recordCrc, ADI_FALSE);
        }
    }
    return ADI_ADRV904X_ERR_ACT_NONE;
}


static adi_adrv904x_ErrAction_e adrv904x_dumpTraceCpuData(  adi_adrv904x_Device_t* const device, 
                                                            FILE **ofp,
                                                            uint32_t baseAddr, 
                                                            uint32_t *recordCrc)
{
    adi_adrv904x_ErrAction_e    recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    if (ofp == NULL || *ofp == NULL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, *ofp, "Unable to open binary image file. Please check if the path is correct");
        return recoveryAction;
    }

    uint32_t                    byteCount       = 0x0U;
    uint32_t                    padding         = 0x0U;
    uint8_t  binaryRead[ADI_ADRV904X_MEM_DUMP_CHUNK_SIZE + 10] = { 0U };

    /* TODO: This should be ETM type specific? */
    uint32_t trcconfigr;
    uint32_t trctraceidr;
    uint32_t trcauthstatus;
    uint32_t trcidr0;
    uint32_t trcidr1;
    uint32_t trcidr2;
    uint32_t trcidr8;
    uint32_t trcidr9;
    uint32_t trcidr10;
    uint32_t trcidr11;
    uint32_t trcidr12;
    uint32_t trcidr13;

    /* Set config values */
    recoveryAction = adi_adrv904x_Register32Read(   device,
                                                    NULL,
                                                    baseAddr + ADRV904X_ETM_TRCCONFIGR_OFFSET,
                                                    &trcconfigr,
                                                    0xFFFFFFFFU);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to Read ETM memory");
        return recoveryAction;
    }
    recoveryAction = adi_adrv904x_Register32Read(   device,
                                                    NULL,
                                                    baseAddr + ADRV904X_ETM_TRCTRACEIDR_OFFSET,
                                                    &trctraceidr,
                                                    0xFFFFFFFFU);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to Read ETM memory");
        return recoveryAction;
    }
    trcauthstatus = 0x0U;
    recoveryAction = adi_adrv904x_Register32Read(   device,
                                                    NULL,
                                                    baseAddr + ADRV904X_ETM_TRCIDR0_OFFSET,
                                                    &trcidr0,
                                                    0xFFFFFFFFU);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to Read ETM memory");
        return recoveryAction;
    }
    recoveryAction = adi_adrv904x_Register32Read(   device,
                                                    NULL,
                                                    baseAddr + ADRV904X_ETM_TRCIDR1_OFFSET,
                                                    &trcidr1,
                                                    0xFFFFFFFFU);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to Read ETM memory");
        return recoveryAction;
    }
    recoveryAction = adi_adrv904x_Register32Read(   device,
                                                    NULL,
                                                    baseAddr + ADRV904X_ETM_TRCIDR2_OFFSET,
                                                    &trcidr2,
                                                    0xFFFFFFFFU);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to Read ETM memory");
        return recoveryAction;
    }
    trcidr8     = 0x0U;
    trcidr9     = 0x0U;
    trcidr10    = 0x0U;
    trcidr11    = 0x0U;
    trcidr12    = 0x0U;
    trcidr13    = 0x0U;

    /* Push config values */
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], trcconfigr);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], trctraceidr);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], trcauthstatus);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], trcidr0);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], trcidr1);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], trcidr2);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], trcidr8);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], trcidr9);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], trcidr10);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], trcidr11);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], trcidr12);
    byteCount += adrv904x_insert32Bits(&binaryRead[byteCount], trcidr13);

    padding = ((byteCount % 8U) ? 8U - (byteCount % 8U) : 0U);
    byteCount += padding;

    if (ADI_LIBRARY_FWRITE(binaryRead,
        1,
        byteCount,
        *ofp) != byteCount)
    {
        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Fatal error while trying to write a binary file.");
        return recoveryAction;
    }
    *recordCrc  = adrv904x_Crc32ForChunk(binaryRead, byteCount, *recordCrc, ADI_FALSE);

    return ADI_ADRV904X_ERR_ACT_NONE;
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuMemDump(adi_adrv904x_Device_t* const                       device,
                                                         const adi_adrv904x_CpuMemDumpBinaryInfo_t* const   cpuMemDumpBinaryInfoPtr,
                                                         const uint8_t                                      forceException,
                                                         uint32_t* const                                    dumpSize)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuMemDumpBinaryInfoPtr, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dumpSize, cleanup);


    recoveryAction = adrv904x_CpuMemDumpBinWrite(device, cpuMemDumpBinaryInfoPtr, forceException, dumpSize, 0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }


cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuMemDump_vRamOnly(adi_adrv904x_Device_t* const                      device,
                                                                  const adi_adrv904x_CpuMemDumpBinaryInfo_t* const  cpuMemDumpBinaryInfoPtr,
                                                                  const uint8_t                                     forceException,
                                                                  uint32_t* const                                   dumpSize)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuMemDumpBinaryInfoPtr, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dumpSize, cleanup);


    recoveryAction = adrv904x_CpuMemDumpBinWrite(device, cpuMemDumpBinaryInfoPtr, forceException, dumpSize, 1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }


cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_JrxRepairSwCEnableGet(adi_adrv904x_Device_t* const  device,
                                                                    uint8_t* const                swcEnMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    uint8_t ctrlDataSet[9U];
    uint8_t ctrlDataGet[9U];
    uint8_t usedDeserLanes = 0U;
    uint8_t localSwcEnMask = 0U;
    uint8_t laneSel = 0U;

    uint32_t lengthResp = 0U;
    uint32_t timeElapsedUs = 0U;
    uint32_t index = 0U;
    uint32_t laneCnt = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, swcEnMask, cleanup);

    /* Prepare the command payload */
    ctrlDataSet[0] = 0x00U;
    ctrlDataSet[1] = (uint8_t)ADRV904X_SERDES_TEST_CMD;
    ctrlDataSet[2] = (uint8_t)ADRV904X_SERDES_TEST_SET_VCM_SWC;
    ctrlDataSet[3] = 0x01U;
    ctrlDataSet[4] = 0x00U;
    ctrlDataSet[5] = 0x00U;
    ctrlDataSet[6] = 0x00U;
    ctrlDataSet[7] = 0x00U;
    ctrlDataSet[8] = 0x00U;

    for (index = 0U; index < (uint32_t)ADI_ADRV904X_MAX_DEFRAMERS; index++)
    {
        usedDeserLanes |= device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled;
    }

    /* Run the command for two used lanes */
    for (index = 0U; (index < (uint32_t)ADI_ADRV904X_MAX_DESERIALIZER_LANES) && (laneCnt < 2U); index++)
    {
        laneSel = (1U << index);

        if ((usedDeserLanes & laneSel) == 0U)
        {
            continue;
        }

        laneCnt++;
        ctrlDataSet[0] = (uint8_t)index;

        /* Exec command */
        recoveryAction = adi_adrv904x_CpuControlCmdExec(device,
                                                        ADRV904X_CPU_OBJID_IC_SERDES,
                                                        ADRV904X_SERDES_CTRL_SET_FSM_CMD,
                                                        (adi_adrv904x_Channels_e)laneSel,
                                                        ctrlDataSet,
                                                        sizeof(ctrlDataSet),
                                                        &lengthResp,
                                                        ctrlDataGet,
                                                        sizeof(ctrlDataGet));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to run Jrx VCM Switch C Set command");
            goto cleanup;
        }

        /* Poll Serdes Calibration Status for completion of the SwC set Command */
        for (timeElapsedUs = 0U; timeElapsedUs < ADI_ADRV904X_TRACKCALDISABLE_TIMEOUT_US; timeElapsedUs += ADI_ADRV904X_TRACKCALDISABLE_INTERVAL_US)
        {
            /* Get Serdes Calibration Status */
            recoveryAction = adi_adrv904x_CalSpecificStatusGet( device,
                                                                (adi_adrv904x_Channels_e)laneSel,
                                                                ADRV904X_CPU_OBJID_TC_SERDES,
                                                                ctrlDataGet,
                                                                sizeof(ctrlDataGet));
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get SwC set Command status failed");
                goto cleanup;
            }

            /* Break out here if SwC set Command have been completed */
            if (ctrlDataGet[1] == (uint8_t)ADRV904X_SERDES_TEST_CMD_DONE)
            {
                break;
            }

            /* SwC set Command is still in progress. Wait the specified wait interval, then check again for status. */
            recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, ADI_ADRV904X_TRACKCALDISABLE_INTERVAL_US);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Issue");
                goto cleanup;
            }
        }

        /* Check for timeout */
        if (timeElapsedUs >= ADI_ADRV904X_TRACKCALDISABLE_TIMEOUT_US)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_FEATURE;
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV904X_ERRSRC_API,
                             ADI_COMMON_ERRCODE_TIMEOUT,
                             recoveryAction,
                             timeElapsedUs,
                             "SwC set Command Timeout");
            goto cleanup;
        }

        localSwcEnMask |= ctrlDataGet[ADRV904X_SERDES_CTRL_FSM_CMD_RSP_HDR_SIZE_BYTES];
    }

    *swcEnMask = localSwcEnMask;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_JrxRepairSwCEnableSet(adi_adrv904x_Device_t* const  device,
                                                                    const uint8_t                 swcEnMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    uint8_t ctrlDataSet[9U];
    uint8_t ctrlDataGet[9U];
    uint8_t usedDeserLanes = 0U;
    uint8_t laneSel = 0U;

    uint32_t lengthResp = 0U;
    uint32_t timeElapsedUs = 0U;
    uint32_t index = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_LIBRARY_MEMSET(ctrlDataSet, 0U, sizeof(ctrlDataSet));
    ADI_LIBRARY_MEMSET(ctrlDataGet, 0U, sizeof(ctrlDataGet));

    for (index = 0U; index < (uint32_t)ADI_ADRV904X_MAX_DEFRAMERS; index++)
    {
        usedDeserLanes |= device->initExtract.jesdSetting.deframerSetting[index].deserialLaneEnabled;
    }

    /* get first used lane */
    for (index = 0U; index < (uint32_t)ADI_ADRV904X_MAX_DESERIALIZER_LANES; index++)
    {
        laneSel = (1U << index);
        if ((usedDeserLanes & laneSel) != 0U)
        {
            break;
        }
    }

    /* Prepare the command payload */
    ctrlDataSet[0] = (uint8_t)index;
    ctrlDataSet[1] = (uint8_t)ADRV904X_SERDES_TEST_CMD;
    ctrlDataSet[2] = (uint8_t)ADRV904X_SERDES_TEST_SET_VCM_SWC;
    ctrlDataSet[3] = 0x01U;
    ctrlDataSet[4] = usedDeserLanes;  
    ctrlDataSet[5] = swcEnMask;
    ctrlDataSet[6] = usedDeserLanes;  
    ctrlDataSet[7] = swcEnMask;
    ctrlDataSet[8] = (swcEnMask == 0x00U) ? 0x00U : 0x01U;

    /* Exec command */
    recoveryAction = adi_adrv904x_CpuControlCmdExec(device,
                                                    ADRV904X_CPU_OBJID_IC_SERDES,
                                                    ADRV904X_SERDES_CTRL_SET_FSM_CMD,
                                                    (adi_adrv904x_Channels_e)laneSel,
                                                    ctrlDataSet,
                                                    sizeof(ctrlDataSet),
                                                    &lengthResp,
                                                    ctrlDataGet,
                                                    sizeof(ctrlDataGet));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to run Jrx VCM Switch C Set command");
        goto cleanup;
    }

    /* Poll Serdes Calibration Status for completion of the SwC set Command */
    for (timeElapsedUs = 0U; timeElapsedUs < ADI_ADRV904X_TRACKCALDISABLE_TIMEOUT_US; timeElapsedUs += ADI_ADRV904X_TRACKCALDISABLE_INTERVAL_US)
    {
        /* Get Serdes Calibration Status */
        recoveryAction = adi_adrv904x_CalSpecificStatusGet( device,
                                                            (adi_adrv904x_Channels_e)laneSel,
                                                            ADRV904X_CPU_OBJID_TC_SERDES,
                                                            ctrlDataGet,
                                                            sizeof(ctrlDataGet));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get SwC set Command status failed");
            goto cleanup;
        }

        /* Break out here if SwC set Command have been completed */
        if (ctrlDataGet[1] == (uint8_t)ADRV904X_SERDES_TEST_CMD_DONE)
        {
            break;
        }

        /* SwC set Command is still in progress. Wait the specified wait interval, then check again for status. */
        recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, ADI_ADRV904X_TRACKCALDISABLE_INTERVAL_US);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Issue");
            goto cleanup;
        }
    }

    /* Check for timeout */
    if (timeElapsedUs >= ADI_ADRV904X_TRACKCALDISABLE_TIMEOUT_US)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_FEATURE;
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV904X_ERRSRC_API,
                         ADI_COMMON_ERRCODE_TIMEOUT,
                         recoveryAction,
                         timeElapsedUs,
                         "SwC set Command Timeout");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_JrxRepairVcmLanesFix(adi_adrv904x_Device_t* const device,
                                                                   uint8_t                laneMask,
                                                                   uint8_t                enableFix)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t index              = 0U;
    uint8_t numDeserLanesUsed  = 0U;
    uint8_t actionLaneSelector = 0U;

    const uint8_t swcVcmDisabled          = 0U;
    const uint8_t swcVcmEnabledLaneUsed   = 1U;
    const uint8_t swcVcmEnabledLaneUnused = 2U;

    static const uint32_t DESER_PD_REG_0_ADDRESS[] = { 
        ADRV904X_ADDR_DESER_PHY0_PD_REG_0,
        ADRV904X_ADDR_DESER_PHY1_PD_REG_0,
        ADRV904X_ADDR_DESER_PHY2_PD_REG_0,
        ADRV904X_ADDR_DESER_PHY3_PD_REG_0,
        ADRV904X_ADDR_DESER_PHY4_PD_REG_0,
        ADRV904X_ADDR_DESER_PHY5_PD_REG_0,
        ADRV904X_ADDR_DESER_PHY6_PD_REG_0,
        ADRV904X_ADDR_DESER_PHY7_PD_REG_0
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_PD_REG_0_VALUE[] = {
        0x01U, /* Reset value of register */
        0xFFU, /* No power-downs on the enabled lanes */
        0x35U  /* Several power-downs on the disabled lanes to save power */
    };

    static const uint32_t DESER_PD_REG_1_ADDRESS[] = { 
        ADRV904X_ADDR_DESER_PHY0_PD_REG_1,
        ADRV904X_ADDR_DESER_PHY1_PD_REG_1,
        ADRV904X_ADDR_DESER_PHY2_PD_REG_1,
        ADRV904X_ADDR_DESER_PHY3_PD_REG_1,
        ADRV904X_ADDR_DESER_PHY4_PD_REG_1,
        ADRV904X_ADDR_DESER_PHY5_PD_REG_1,
        ADRV904X_ADDR_DESER_PHY6_PD_REG_1,
        ADRV904X_ADDR_DESER_PHY7_PD_REG_1,
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_PD_REG_1_VALUE[] = {
        0x00U, /* Reset value of register */
        0x02U, /* Powers down the CM amp on the enabled lanes. */
        0x07U  /* More power-down facilities on the disabled lanes to save power */
    };

    static const uint32_t DESER_CORE1P2_TEST_ADDRESS[] = { 
        ADRV904X_ADDR_DESER_PHY0_CORE1P2_TEST,
        ADRV904X_ADDR_DESER_PHY1_CORE1P2_TEST,
        ADRV904X_ADDR_DESER_PHY2_CORE1P2_TEST,
        ADRV904X_ADDR_DESER_PHY3_CORE1P2_TEST,
        ADRV904X_ADDR_DESER_PHY4_CORE1P2_TEST,
        ADRV904X_ADDR_DESER_PHY5_CORE1P2_TEST,
        ADRV904X_ADDR_DESER_PHY6_CORE1P2_TEST,
        ADRV904X_ADDR_DESER_PHY7_CORE1P2_TEST
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_TEST_VALUE[] = {
        0x00U, /* Reset value of register */
        30U,   /*This connects the CM node of the used lanes to the AMUX*/
        29U,   /*This connects the CM bias node of the unused lanes to the AMUX*/
    };

    static const uint32_t DESER_CORE1P2_DECFE_CTL0_ADDRESS[] = { 
        ADRV904X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL0,
        ADRV904X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL0,
        ADRV904X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL0,
        ADRV904X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL0,
        ADRV904X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL0,
        ADRV904X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL0,
        ADRV904X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL0,
        ADRV904X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL0
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL0_VALUE[] = {
        0x00U, /* Reset value of register */
        0xFFU, /* No power-downs on the enabled lanes */
        0x0FU  /* More power-down facilities on the disabled lanes */
    };

    static const uint32_t DESER_CORE1P2_DECFE_CTL18_ADDRESS[] = { 
        ADRV904X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL18,
        ADRV904X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL18,
        ADRV904X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL18,
        ADRV904X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL18,
        ADRV904X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL18,
        ADRV904X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL18,
        ADRV904X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL18,
        ADRV904X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL18
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL18_VALUE[] = {
        0x00U, /* Reset value of register */
        0xFFU, /* No power-downs on the enabled lanes */
        0x01U  /* More power-down facilities on the disabled lanes */
    };
    static const uint32_t DESER_CORE1P2_DECFE_CTL28_ADDRESS[] = { 
        ADRV904X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL28,
        ADRV904X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL28,
        ADRV904X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL28,
        ADRV904X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL28,
        ADRV904X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL28,
        ADRV904X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL28,
        ADRV904X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL28,
        ADRV904X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL28
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL28_VALUE[] = {
        0x00U, /* Reset value of register */
        0xFFU, /* Not used on used lanes */
        0xFFU  /* More power-down facilities on the disabled lanes */
    };
    static const uint32_t DESER_CORE1P2_DECFE_CTL31_ADDRESS[] = { 
        ADRV904X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL31,
        ADRV904X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL31,
        ADRV904X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL31,
        ADRV904X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL31,
        ADRV904X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL31,
        ADRV904X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL31,
        ADRV904X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL31,
        ADRV904X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL31
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL31_VALUE[] = {
        0x00U, /* Reset value of register */
        0xFFU, /* Not used on used lanes */
        0x01U  /* More power-down facilities on the disabled lanes */
    };

    static const uint32_t DESER_CORE1P2_DECFE_CTL41_ADDRESS[] = { 
        ADRV904X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL41,
        ADRV904X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL41,
        ADRV904X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL41,
        ADRV904X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL41,
        ADRV904X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL41,
        ADRV904X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL41,
        ADRV904X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL41,
        ADRV904X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL41
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL41_VALUE[] = {
        0x00U, /* Reset value of register */
        0x00U, /* Not used on used lanes */
        0xFFU  /* More power-down facilities on the disabled lanes */
    };
    static const uint32_t DESER_CORE1P2_DECFE_CTL43_ADDRESS[] = { 
        ADRV904X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL43,
        ADRV904X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL43,
        ADRV904X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL43,
        ADRV904X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL43,
        ADRV904X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL43,
        ADRV904X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL43,
        ADRV904X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL43,
        ADRV904X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL43
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL43_VALUE[] = {
        0x00U, /* Reset value of register */
        0xFFU, /* Not used on used lanes */
        0x80U  /* More power-down facilities on the disabled lanes */
    };
    static const uint32_t DESER_CORE1P2_DECFE_CTL45_ADDRESS[] = { 
        ADRV904X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL45,
        ADRV904X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL45,
        ADRV904X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL45,
        ADRV904X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL45,
        ADRV904X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL45,
        ADRV904X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL45,
        ADRV904X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL45,
        ADRV904X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL45
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL45_VALUE[] = {
        0x00U, /* Reset value of register */
        0xFFU, /* Not used on used lanes */
        0x80U  /* More power-down facilities on the disabled lanes */
    };

    static const uint32_t DESER_CORE1P2_DECFE_CTL49_ADDRESS[] = { 
        ADRV904X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL49,
        ADRV904X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL49,
        ADRV904X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL49,
        ADRV904X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL49,
        ADRV904X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL49,
        ADRV904X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL49,
        ADRV904X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL49,
        ADRV904X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL49
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL49_VALUE[] = {
        0x20U, /* Reset value of register */
        0xFFU, /* Not used on used lanes */
        0x06U  /* Powers down the CM amp on the disabled lanes */
    };

    static const uint32_t DESER_CORE1P2_DECFE_CTL54_ADDRESS[] = { 
        ADRV904X_ADDR_DESER_PHY0_CORE1P2_DECFE_CTL54,
        ADRV904X_ADDR_DESER_PHY1_CORE1P2_DECFE_CTL54,
        ADRV904X_ADDR_DESER_PHY2_CORE1P2_DECFE_CTL54,
        ADRV904X_ADDR_DESER_PHY3_CORE1P2_DECFE_CTL54,
        ADRV904X_ADDR_DESER_PHY4_CORE1P2_DECFE_CTL54,
        ADRV904X_ADDR_DESER_PHY5_CORE1P2_DECFE_CTL54,
        ADRV904X_ADDR_DESER_PHY6_CORE1P2_DECFE_CTL54,
        ADRV904X_ADDR_DESER_PHY7_CORE1P2_DECFE_CTL54
    };
    /* reg add; disable swc/subs; enable used swc/subs; enabled unused swc/subs */
    static const uint32_t DESER_CORE1P2_DECFE_CTL54_VALUE[] = {
        0x00U, /* Reset value of register */
        0xFFU, /* Not used on used lanes */
        0x05U  /* More power-down facilities on the disabled lanes */
    };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    if ((enableFix != ADI_TRUE) && (enableFix != ADI_FALSE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
        recoveryAction = adi_adrv904x_Register32Write(device, 
                                                      NULL, 
                                                      ADRV904X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_TOP_8PACK, 
                                                      0x00,
                                                      0xFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV904X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_TOP_8PACK");
            goto cleanup;
        }
        for (index = 0U; index < (uint32_t)ADI_ADRV904X_MAX_DESERIALIZER_LANES; index++)
        {
            /* Lane NOT being used */
            actionLaneSelector = swcVcmEnabledLaneUnused;

            if ((laneMask & (1U << index)) == 0U)
            {
                recoveryAction = adi_adrv904x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL49_ADDRESS[index], DESER_CORE1P2_DECFE_CTL49_VALUE[actionLaneSelector], 0x07U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV904X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL49 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv904x_Register32Write(device, NULL, DESER_PD_REG_0_ADDRESS[index], DESER_PD_REG_0_VALUE[actionLaneSelector], 0x3FU);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV904X_ADDR_DESER_PHY_PD_REG_0 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv904x_Register32Write(device, NULL, DESER_PD_REG_1_ADDRESS[index], DESER_PD_REG_1_VALUE[actionLaneSelector], 0x07U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV904X_ADDR_DESER_PHY_PD_REG_1 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv904x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL18_ADDRESS[index], DESER_CORE1P2_DECFE_CTL18_VALUE[actionLaneSelector], 0x01U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV904X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL18 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv904x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL31_ADDRESS[index], DESER_CORE1P2_DECFE_CTL31_VALUE[actionLaneSelector], 0x01U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV904X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL31 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv904x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL54_ADDRESS[index], DESER_CORE1P2_DECFE_CTL54_VALUE[actionLaneSelector], 0x0FU);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV904X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL54 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv904x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL0_ADDRESS[index], DESER_CORE1P2_DECFE_CTL0_VALUE[actionLaneSelector], 0x0FU);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV904X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL0 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv904x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL28_ADDRESS[index], DESER_CORE1P2_DECFE_CTL28_VALUE[actionLaneSelector], 0xFFU);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV904X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL28 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv904x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL41_ADDRESS[index], DESER_CORE1P2_DECFE_CTL41_VALUE[actionLaneSelector], 0xFFU);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV904X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL41 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv904x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL43_ADDRESS[index], DESER_CORE1P2_DECFE_CTL43_VALUE[actionLaneSelector], 0xFFU);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV904X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL43 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv904x_Register32Write(device, NULL, DESER_CORE1P2_DECFE_CTL45_ADDRESS[index], DESER_CORE1P2_DECFE_CTL45_VALUE[actionLaneSelector], 0xFFU);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV904X_ADDR_DESER_PHY_CORE1P2_DECFE_CTL45 of one of the lanes");
                    goto cleanup;
                }
            }
        }
        if (numDeserLanesUsed > 0x4U)
        {
            recoveryAction = adi_adrv904x_JrxRepairSwCEnableSet(device, laneMask);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Set SwC Enabled");
                goto cleanup;
            }

            for (index = 0U; index < (uint32_t)ADI_ADRV904X_MAX_DESERIALIZER_LANES; index++)
            {
                if ((laneMask & (1U << index)) == 0U)
                {
                    /* Lane NOT being used */
                    actionLaneSelector = swcVcmEnabledLaneUnused;

                    recoveryAction = adi_adrv904x_Register32Write(device, NULL, DESER_CORE1P2_TEST_ADDRESS[index], DESER_CORE1P2_TEST_VALUE[actionLaneSelector], 0x1FU);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to DESERDES CORE1P2_TEST");
                        goto cleanup;
                    }
                }
            }
        }
        else
        {
            for (index = 0U; index < (uint32_t)ADI_ADRV904X_MAX_DESERIALIZER_LANES; index++)
            {
                actionLaneSelector = ((laneMask & (1U << index)) == 0U) ? swcVcmEnabledLaneUnused : swcVcmEnabledLaneUsed;

                recoveryAction = adi_adrv904x_Register32Write(device, NULL, DESER_PD_REG_1_ADDRESS[index], DESER_PD_REG_1_VALUE[actionLaneSelector], 0x07U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to DESERDES PD_REG_1");
                    goto cleanup;
                }

                recoveryAction = adi_adrv904x_Register32Write(device, NULL, DESER_CORE1P2_TEST_ADDRESS[index], DESER_CORE1P2_TEST_VALUE[actionLaneSelector], 0x1FU);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to DESERDES CORE1P2_TEST");
                    goto cleanup;
                }
            }
        }

        /* Indicate that JRxRepair has been applied */
        device->devStateInfo.devState = (adi_adrv904x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV904X_STATE_JRXREPAIRED);

    }
    else
    {
        if (numDeserLanesUsed <= 0x4U)
        {
            for (index = 0U; index < (uint32_t)ADI_ADRV904X_MAX_DESERIALIZER_LANES; index++)
            {
                actionLaneSelector = swcVcmDisabled;

                recoveryAction = adi_adrv904x_Register32Write(device, NULL, DESER_PD_REG_1_ADDRESS[index], DESER_PD_REG_1_VALUE[actionLaneSelector], 0x07U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV904X_ADDR_DESER_PHY_PD_REG_1 of one of the lanes");
                    goto cleanup;
                }

                recoveryAction = adi_adrv904x_Register32Write(device, NULL, DESER_CORE1P2_TEST_ADDRESS[index], DESER_CORE1P2_TEST_VALUE[actionLaneSelector], 0x1FU);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to DESERDES CORE1P2_TEST");
                    goto cleanup;
                }
            }
        }
        else
        {
            recoveryAction = adi_adrv904x_JrxRepairSwCEnableSet(device, 0x0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to Set SwC Disabled");
                goto cleanup;
            }
        }

        recoveryAction = adi_adrv904x_Register32Write(device, 
                                                        NULL, 
                                                        ADRV904X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_TOP_8PACK, 
                                                        ~laneMask,
                                                        0xFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write to ADRV904X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_TOP_8PACK");
            goto cleanup;
        }

        /* Indicate that JRxRepair is disabled */
        device->devStateInfo.devState = (adi_adrv904x_ApiStates_e)(device->devStateInfo.devState & ~ADI_ADRV904X_STATE_JRXREPAIRED);

    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_JrxRepairInitialization(adi_adrv904x_Device_t* const device,
                                                                      uint8_t                enableRepair)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t idx = 0U;
    uint8_t screenID = 0U;
    uint8_t usedDeserLanes = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    recoveryAction = adrv904x_JrxRepairScreenTestChecker(device, &screenID);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Screen check has failed.");
        goto cleanup;
    }

        if (((enableRepair & ADI_ADRV904X_JRXREPAIR_INIT_ALL) == ADI_ADRV904X_JRXREPAIR_INIT_ALL) ||
       (((enableRepair & ADI_ADRV904X_JRXREPAIR_INIT_NONSCREENED) == ADI_ADRV904X_JRXREPAIR_INIT_NONSCREENED) && (screenID == ADI_FALSE)))
    {
        for (idx = 0U; idx < (uint8_t)ADI_ADRV904X_MAX_DEFRAMERS; idx++)
        {
            usedDeserLanes |= device->initExtract.jesdSetting.deframerSetting[idx].deserialLaneEnabled;
        }

        recoveryAction = adi_adrv904x_JrxRepairVcmLanesFix(device, usedDeserLanes, ADI_TRUE);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to fix enable VCM fix");
            goto cleanup;
        }
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


/* Helper function to load CPU image from file. SHOULD ONLY BE CALLED FROM adi_adrv904x_CpuImageLoad() */
static adi_adrv904x_ErrAction_e adrv904x_CpuLoadUtil(adi_adrv904x_Device_t* const              device,
                                                     const adi_adrv904x_cpuBinaryInfo_t* const cpuBinaryInfo,
                                                     FILE*                                     cpuImageFilePtr,
                                                     const adi_adrv904x_CpuType_e              cpuType)
{
    adi_adrv904x_ErrAction_e  recoveryAction    = ADI_ADRV904X_ERR_ACT_NONE;
    uint32_t                  numFileChunks     = 0U;
    uint32_t                  chunkIndex        = 0U;
    uint8_t                   cpuImageChunk[ADI_ADRV904X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];
    uint32_t                  byteCount         = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cpuBinaryInfo);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cpuImageFilePtr);

    /* Calculate number of image chunks to write */
    if (cpuType == ADI_ADRV904X_CPU_TYPE_0)
    {
        numFileChunks = (uint32_t)(ADI_ADRV904X_CPU_0_BINARY_IMAGE_FILE_SIZE_BYTES / ADI_ADRV904X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES);
    }
    else if (cpuType == ADI_ADRV904X_CPU_TYPE_1)
    {
        numFileChunks = (uint32_t)(ADI_ADRV904X_CPU_1_BINARY_IMAGE_FILE_SIZE_BYTES / ADI_ADRV904X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES);
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuType, "Invalid CPU type");
        return recoveryAction;
    }

        for (chunkIndex = 0U; chunkIndex < numFileChunks; ++chunkIndex)
    {
        /* Read Segment of CPU Image */
        byteCount = ADI_LIBRARY_FREAD(&cpuImageChunk[0U], 1, ADI_ADRV904X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES, cpuImageFilePtr);
        if (byteCount == ADI_ADRV904X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES)
        {
            /* Write CPU Image Segment */
            recoveryAction = adi_adrv904x_CpuImageWrite(device,
                                                        cpuType,
                                                        (chunkIndex * ADI_ADRV904X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES),
                                                        &cpuImageChunk[0U],
                                                        byteCount);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error During Binary Loading (e.g. SPI Issue)");
                return recoveryAction;
            }
        }
        else
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Error Reading Block of Data from CPU Binary File");
            return recoveryAction;
        }
    }

    return recoveryAction;
}

static void adrv904x_EndiannessCheckAndConvert(void* const buffer, const size_t elementSize, const size_t elementCount)
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
            tmp16[i] = ADRV904X_HTOCS(tmp16[i]);
        }
    }
    else if (elementSize == 4U)
    {
        //biteswap large
        tmp32 = (uint32_t* const)buffer;
        for (i = 0; i < elementCount; i++) {
            tmp32[i] = ADRV904X_HTOCL(tmp32[i]);
        }
    }
    else if (elementSize == 8U)
    {
        //biteswap large large
        tmp64 = (uint64_t* const)buffer;
        for (i = 0; i < elementCount; i++) {
            tmp64[i] = ADRV904X_HTOCLL(tmp64[i]);
        }
    }
}
static size_t adrv904x_VariableFromFileExtract(adi_adrv904x_Device_t* const device, 
                                                void* const buffer, 
                                                const size_t elementSize, 
                                                const size_t elementCount, 
                                                FILE* file)
{
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, file);

    uint8_t returnValue = ADI_LIBRARY_FREAD(buffer, 1, elementSize * elementCount, file);
    if (returnValue > 0)
    {
        adrv904x_EndiannessCheckAndConvert(buffer, elementSize, elementCount);
    }
    return returnValue;
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainTableChecksumRead(adi_adrv904x_Device_t* const device,
                                                                      const adi_adrv904x_RxGainTableInfo_t* const  rxGainTableInfoPtr,
                                                                      uint32_t* const rxGainTableChecksum)
{
        adi_adrv904x_ErrAction_e    recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const uint8_t               LINE_BUFFER_SIZE    = 128U;
    const uint8_t               HEADER_BUFFER_SIZE  = 16U;
    const uint8_t               NUM_COLUMNS         = 5U;
    FILE*                       rxGainTableFilePtr  = NULL;
    char                        rxGainTableLineBuffer[LINE_BUFFER_SIZE];
    char                        headerStr1[HEADER_BUFFER_SIZE];
    adi_adrv904x_Version_t      tableVersion        = { 0U, 0U, 0U, 0U };
    uint32_t                    checksum[4U]        = { 0U, 0U, 0U, 0U };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxGainTableChecksum, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxGainTableInfoPtr, cleanup);

    if (ADI_LIBRARY_STRNLEN((const char*)rxGainTableInfoPtr->filePath, ADI_ADRV904X_MAX_FILE_LENGTH) == ADI_ADRV904X_MAX_FILE_LENGTH)
    {
        /* Path is not terminated */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfoPtr->filePath, "Unterminated path string");
        goto cleanup;
    }

    /* Open Rx Gain Table CSV file */
#ifdef __GNUC__
    rxGainTableFilePtr = ADI_LIBRARY_FOPEN((const char *)rxGainTableInfoPtr->filePath, "r");
#else
    if (ADI_LIBRARY_FOPEN_S(&rxGainTableFilePtr, (const char *)rxGainTableInfoPtr->filePath, "r") != 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxGainTableInfoPtr->filePath,
                               "Unable to open Rx Gain Table csv file. Please check if the path is correct or the file is open in another program");

        goto cleanup;
    }
#endif

    if (rxGainTableFilePtr == NULL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxGainTableInfoPtr->filePath,
                               "Invalid Rx Gain Table csv file path while attempting to load Rx Gain Table");
        goto cleanup;
    }

    if (fgets(rxGainTableLineBuffer, sizeof(rxGainTableLineBuffer), rxGainTableFilePtr) == NULL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfoPtr->filePath, "Invalid Rx Gain Table Format Detected");
        goto cleanup;
    }

    /* Verify that Gain Table Version Format is correct */
    if (ADI_LIBRARY_STRSTR(headerStr1, "Version") == NULL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr1, "Version Column Expected First");
        goto cleanup;
    }


    if (fgets(rxGainTableLineBuffer, sizeof(rxGainTableLineBuffer), rxGainTableFilePtr) == NULL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxGainTableInfoPtr->filePath, "Invalid Rx Gain Table Format Detected");
        goto cleanup;
    }

    /* Verify that Gain Table Checksum Format is correct */
    if (ADI_LIBRARY_STRSTR(headerStr1, "Checksum") == NULL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, headerStr1, "Checksum Column Expected First");
        goto cleanup;
    }

    *rxGainTableChecksum = checksum[0];

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:

    if (rxGainTableFilePtr != NULL)
    {
        /* Close Rx Gain Table csv file */
        if (ADI_LIBRARY_FCLOSE(rxGainTableFilePtr) != 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   rxGainTableInfoPtr->filePath,
                                   "Fatal error while trying to close Rx Gain Table csv file. Possible memory shortage while flushing / other I/O errors.");
        }
    }

    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainTableChecksumCalculate(adi_adrv904x_Device_t* const device,
                                                                           const adi_adrv904x_RxChannels_e rxChannel,
                                                                           uint32_t* const rxGainTableChecksum)
{
        adi_adrv904x_ErrAction_e  recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

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

    adi_adrv904x_RxGainTableRow_t gainTableRow[MAX_RX_GAIN_TABLE_LINES];
    uint8_t                       gainTableData[GAIN_TABLE_SIZE] = { 0U };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxGainTableChecksum, cleanup);
    ADI_LIBRARY_MEMSET(&gainTableRow, 0, sizeof(gainTableRow));

    /* Check that the channel requested is valid */
    if ((rxChannel != ADI_ADRV904X_RX0) &&
        (rxChannel != ADI_ADRV904X_RX1) &&
        (rxChannel != ADI_ADRV904X_RX2) &&
        (rxChannel != ADI_ADRV904X_RX3) &&
        (rxChannel != ADI_ADRV904X_RX4) &&
        (rxChannel != ADI_ADRV904X_RX5) &&
        (rxChannel != ADI_ADRV904X_RX6) &&
        (rxChannel != ADI_ADRV904X_RX7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannel,
                               "Invalid Rx Channel Requested for gain table read. Valid Rx channels include Rx0-Rx7");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_RxGainTableRead(device,
                                                  rxChannel,
                                                  ADI_ADRV904X_MAX_GAIN_TABLE_INDEX,
                                                  gainTableRow,
                                                  MAX_RX_GAIN_TABLE_LINES,
                                                  &numGainIndicesRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannel,
                               "Rx Gain Table read failure");
        goto cleanup;
    }

    if (numGainIndicesRead != MAX_RX_GAIN_TABLE_LINES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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

        checksum = adrv904x_Crc32ForChunk(gainTableData,
                                          GAIN_TABLE_SIZE,
                                          checksum,
                                          finalCrc);
    }

    *rxGainTableChecksum = checksum;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_InitDataExtract(adi_adrv904x_Device_t* const                     device,
                                                              const adi_adrv904x_CpuProfileBinaryInfo_t* const cpuBinaryInfo,
                                                              adi_adrv904x_Version_t* const                    apiVer,
                                                              adi_adrv904x_CpuFwVersion_t* const               fwVer,
                                                              adi_adrv904x_Version_t* const                    streamVer,
                                                              adi_adrv904x_Init_t* const                       init,
                                                              adi_adrv904x_PostMcsInit_t* const                postMcsInit,
                                                              adi_adrv904x_ExtractInitDataOutput_e* const      checkOutput)
{
        adi_adrv904x_ErrAction_e    recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    FILE*                       cpuProfileFilePtr   = NULL;
    uint32_t                    totalFileSize       = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, apiVer,        cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, fwVer,         cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, streamVer,     cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, init,          cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, postMcsInit,   cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, checkOutput,   cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuBinaryInfo, cleanup);

    if (ADI_LIBRARY_STRNLEN((const char*)cpuBinaryInfo->filePath, ADI_ADRV904X_MAX_FILE_LENGTH) == ADI_ADRV904X_MAX_FILE_LENGTH)
    {
        /* Path is not terminated */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo, "Unterminated path string");
        goto cleanup;
    }

    /* Open cpu profile binary file */
#ifdef __GNUC__
    cpuProfileFilePtr = ADI_LIBRARY_FOPEN((const char*)cpuBinaryInfo->filePath, "rb");
#else
    if (ADI_LIBRARY_FOPEN_S(&cpuProfileFilePtr, (const char*)cpuBinaryInfo->filePath, "rb") != 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileBinaryInfoPtr->filePath, "Unable to open CPU Profile Binary. Please check filepath is correct");
        goto cleanup;
    }
#endif

    if (NULL == cpuProfileFilePtr)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to open CPU Profile Binary. Please check filepath is correct")
        goto cleanup;
    }

    /* Determine file size */
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, 0U, SEEK_END) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Seek to EOF Failed");
        goto cleanup;
    }

    /* ADI_LIBRARY_FTELL returns long type */
    totalFileSize = (uint32_t) ADI_LIBRARY_FTELL(cpuProfileFilePtr);

    /* Check that cpu profile binary file is not empty */
    if (0U == totalFileSize)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, totalFileSize, "Zero Length CPU Profile Binary Detected");
        goto cleanup;
    }

    if ((uint32_t)sizeof(adrv904x_DeviceProfile_t) == totalFileSize)
    {
        recoveryAction  = ADI_ADRV904X_ERR_ACT_NONE;
        *checkOutput    = ADI_ADRV904X_EXTRACT_INIT_DATA_LEGACY_PROFILE_BIN;
        goto cleanup;
    }
    
    if(ADI_LIBRARY_FSEEK(cpuProfileFilePtr, 0, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Could not rewind to start");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_LoadBinFile(device, cpuProfileFilePtr, apiVer, fwVer, streamVer, init, postMcsInit, (uint32_t)sizeof(adrv904x_DeviceProfile_t));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuBinaryInfo->filePath, "Issue with file path for cpu profile");
        goto cleanup;
    }

    *checkOutput = ADI_ADRV904X_EXTRACT_INIT_DATA_POPULATED;

cleanup:
    if (cpuProfileFilePtr != NULL)
    {
        /* Close CPU Profile binary file */
        if (0 != ADI_LIBRARY_FCLOSE(cpuProfileFilePtr))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close CPU Profile Binary File");
        }
    }

    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerImageLoad(adi_adrv904x_Device_t* const device,
                                                                      const adi_adrv904x_RadioSequencerBinaryInfo_t*  const rsBinaryInfoPtr)
{
        const size_t                BIN_ELEMENT_SIZE        = 1U;
    adi_adrv904x_ErrAction_e    recoveryAction          = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    FILE*                       rsBinaryFilePtr         = NULL;
    uint32_t                    fileSize                = 0U;
    uint32_t                    numFileChunks           = 0U;
    uint32_t                    chunkIndex              = 0U;
    uint8_t                     rsBinaryImageBuffer[ADI_ADRV904X_RS_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES];
    uint32_t                    byteCount               = 0U;


    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rsBinaryInfoPtr, cleanup);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rsBinaryInfoPtr->filePath, cleanup);

    if (ADI_LIBRARY_STRNLEN((const char*)rsBinaryInfoPtr->filePath, ADI_ADRV904X_MAX_FILE_LENGTH) == ADI_ADRV904X_MAX_FILE_LENGTH)
    {
        /* Path is not terminated */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rsBinaryInfoPtr->filePath, "Unterminated path string");
        goto cleanup;
    }

    /* Open Stream Binary File */
#ifdef __GNUC__
    rsBinaryFilePtr = ADI_LIBRARY_FOPEN((const char *)rsBinaryInfoPtr->filePath, "rb");
#else
    if (ADI_LIBRARY_FOPEN_S(&rsBinaryFilePtr, (const char *)rsBinaryInfoPtr->filePath, "rb") != 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rsBinaryInfoPtr->filePath, "Invalid Binary File or Path Detected");
        goto cleanup;
    }
#endif

    if (NULL == rsBinaryFilePtr)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rsBinaryInfoPtr->filePath, "Invalid Binary File or Path Detected");
        goto cleanup;
    }

    /*Determine file size*/
    if (ADI_LIBRARY_FSEEK(rsBinaryFilePtr, 0U, SEEK_END) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rsBinaryFilePtr, "Seek to EOF Failed");
        goto cleanup;
    }

    /* ADI_LIBRARY_FTELL returns long type */
    fileSize = (uint32_t) ADI_LIBRARY_FTELL(rsBinaryFilePtr);
    numFileChunks = (uint32_t)(fileSize / ADI_ADRV904X_RS_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES + 1);

    /* Check that Radio Sequencer binary file is not empty */
    if (fileSize == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rsBinaryFilePtr, "Zero Length Radio Sequencer Image Detected");
        goto cleanup;
    }

    /* Check that Radio Sequencer binary file size does not exceed maximum size */
    if (fileSize > ADI_ADRV904X_RADIO_SEQ_IMAGE_FILE_MAX_SIZE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileSize, "Radio Sequencer Binary Size exceeds Maximum Limit");
        goto cleanup;
    }

    /* Check that size of the file is a multiple of 4 */
    if ((fileSize & 0x3U) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rsBinaryInfoPtr->filePath, "Incorrect Radio Sequencer Binary File Block Size");
        goto cleanup;
    }

    /* Rewind the file pointer to beginning of the file */
    if (ADI_LIBRARY_FSEEK(rsBinaryFilePtr, 0U, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rsBinaryFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    /* Put the part into streaming mode to help facilitate the large, contiguous SPI writes in CpuImageWrite */
    recoveryAction = adrv904x_SpiStreamingEntry(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with entering Spi Streaming mode");
        goto cleanup;
    }

    /* Read ARM binary file */
    for (chunkIndex = 0U; chunkIndex < numFileChunks; ++chunkIndex)
    {
        if ((byteCount = ADI_LIBRARY_FREAD(&rsBinaryImageBuffer[0U], BIN_ELEMENT_SIZE, ADI_ADRV904X_RS_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES, rsBinaryFilePtr)) > 0)
        {
            /* Write Radio Sequencer binary */
            recoveryAction = adi_adrv904x_RadioSequencerImageWrite(device,
                                                                   (chunkIndex * ADI_ADRV904X_RS_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES),
                                                                   &rsBinaryImageBuffer[0U],
                                                                   byteCount);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Radio Sequencer Write");
                goto cleanup;
            }
        }
        else if (ferror(rsBinaryFilePtr))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numFileChunks, "Error Reading Block of Data from Radio Sequencer Binary File");
            goto cleanup;
        }
    }
cleanup:
    if (rsBinaryFilePtr != NULL)
    {
        /* Close Radio Sequencer binary file */
        if (0 != ADI_LIBRARY_FCLOSE(rsBinaryFilePtr))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close Radio Sequencer Image File");
        }
    }

    if (device->devStateInfo.spiStreamingOn == ADI_TRUE)
    {
        if (ADI_ADRV904X_ERR_ACT_NONE != adrv904x_SpiStreamingExit(device))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_INTERFACE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Spi Streaming Exit Issue");
        }
    }

    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerImageReload(adi_adrv904x_Device_t* const device,
                                                                        const adi_adrv904x_RadioSequencerBinaryInfo_t*  const rsBinaryInfoPtr)
{
        const adrv904x_BfSequencerChanAddr_e sequencerBaseAddr  = ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS;
    const uint32_t RS_TOTAL_NUM_OF_SEQUENCERS               = 16U;
    const uint32_t RADIO_SEQUENCER_CONFIG_0_REG_OFFSET      = 0x20U;
    const uint32_t DBG_PROGRAM_COUNTER_BITP                 = 0U;
    const uint32_t DBG_PROGRAM_COUNTER_BITM                 = 0x0000FFFFU;
    const uint32_t RESET_HOLD_BITP                          = 28U;
    const uint32_t RADIO_SEQUENCER_NUM_SEQUENCERS           = 16U;
    const uint32_t RS_NUM_FRAME_TIMING_SEQUENCERS           = 10U;
    const uint32_t RS_NUM_GPR_PER_SEQUENCERS                = 16U;
    const uint32_t RS_STOP_WAIT_INTERVAL_US                 = 100U;
    const uint32_t RS_STOP_WAIT_CNT                         = 1000U;

    adi_adrv904x_ErrAction_e                    recoveryAction              = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t                                     channelId                   = 0U;
    uint8_t                                     resetHold                   = 1U;
    bool                                        rsRadioRunning              = false;
    uint32_t                                    pointerInit                 = 0x0U;
    uint8_t                                     stopComplete                = 0U;
    uint32_t                                    waitCounter                 = 0U;
    uint32_t                                    curPc                       = 0U;
    adrv904x_DfeSvcCmdStatus_t                  cmdStatus                   = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adrv904x_DfeSvcRadioCtrlSequencerInitData_t cmdStruct;
    adrv904x_DfeSvcCmdGenericResp_t             respStruct;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rsBinaryInfoPtr, cleanup);

    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Check that Radio Sequencer Enable */
    if (device->initExtract.rs.radioSeqEn == ADI_FALSE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        goto cleanup;
    }

    /* Check that Radio Sequencer has been loaded before getting RS error info */
    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_RSLOADED) != ADI_ADRV904X_STATE_RSLOADED)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Radio Sequencer should have been loaded before calling this API");
        goto cleanup;
    }

    rsRadioRunning = device->devStateInfo.rsRadioRunning;

    /* Check if radio operation is in progress */
    if (rsRadioRunning == ADI_TRUE)
    {
        /* Stop the radio operation */
        recoveryAction = adi_adrv904x_RadioSequencerRadioStop(device);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to stop the RS radio operation");
            goto cleanup;
        }

        /* Get the null pattern address */
        recoveryAction = adrv904x_RadioSequencerPatternAddrGet(device,
                                                               ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_NULL_PATTERN_GLOBAL_PTRN_ID,
                                                               &pointerInit);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get address of RS null pattern");
            goto cleanup;
        }

        /* Wait until all frame timing radio sequencers have started executing the null pattern before continuing. */
        stopComplete = 0U;
        for (waitCounter = 0U; waitCounter < RS_STOP_WAIT_CNT; waitCounter++)
        {
            /* Assume that all have started until proven otherwise below */
            stopComplete = 1U;

            /* Verify the the current PC == pointerInit */
            for (channelId = 0; channelId < RS_NUM_FRAME_TIMING_SEQUENCERS; channelId++)
            {
                recoveryAction = adi_adrv904x_Register32Read(device,
                                                             NULL,
                                                             ((uint32_t) sequencerBaseAddr + RADIO_SEQUENCER_CONFIG_0_REG_OFFSET + channelId * 4),
                                                             &curPc,
                                                             0xFFFFFFFFU);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading RS dbg_program_counter bitfield");
                    goto cleanup;
                }

                curPc = (curPc & DBG_PROGRAM_COUNTER_BITM) >> DBG_PROGRAM_COUNTER_BITP;
                if (curPc != (pointerInit & DBG_PROGRAM_COUNTER_BITM))
                {
                    /* One of the radio sequencers hasn't started executing the startup pattern yet.
                     * Continue checking for completion.
                     */
                    stopComplete = 0U;
                }
            }

            if (stopComplete == 0U)
            {
                /* Wait a bit before running the check again */
                recoveryAction = (adi_adrv904x_ErrAction_e)adi_common_hal_Wait_us(&device->common, RS_STOP_WAIT_INTERVAL_US);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while waiting for Radio Sequencer to stop");
                    goto cleanup;
                }
            }
            else
            {
                /* All radio sequencers are executing the null pattern, exit the wait loop */
                break;
            }
        }

        /* Radio sequencer didn't stop in the specified amount of time. */
        if (stopComplete == 0U)
        {
            /* Set the RS status to not started */
            recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_DEVICE;
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV904X_ERRSRC_API,
                             ADI_COMMON_ERRCODE_TIMEOUT,
                             recoveryAction,
                             ADI_NO_VARIABLE,
                             "Radio Sequencer Stop Timeout");
            goto cleanup;
        }
    }

    /* Set the reset_hold for all the 16 radio sequencers */
    for (channelId = 0U; channelId < RADIO_SEQUENCER_NUM_SEQUENCERS; channelId++)
    {
        uint32_t regVal = 0U;
        recoveryAction = adi_adrv904x_Register32Read(device,
            NULL,
            ((uint32_t) sequencerBaseAddr + RADIO_SEQUENCER_CONFIG_0_REG_OFFSET + channelId * 4U),
            &regVal,
            0xFFFFFFFFU);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading RS reset_hold bitfield");
            goto cleanup;
        }

        regVal = (resetHold == 0U) ? regVal & (~(1U << RESET_HOLD_BITP)) : regVal | (1U << RESET_HOLD_BITP);

        recoveryAction = adi_adrv904x_Register32Write(device,
            NULL,
            ((uint32_t) sequencerBaseAddr + RADIO_SEQUENCER_CONFIG_0_REG_OFFSET + channelId * 4U),
            regVal,
            0xFFFFFFFFU);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting RS reset_hold bitfield");
            goto cleanup;
        }
    }

    recoveryAction = adrv904x_RadioSequencerApplyImmediately(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting RS Apply Immediately");
        goto cleanup;
    }

    /* Confirm that RS has stopped executing by reading the dbg_state bitfield (RESET = 3'b100) */
    stopComplete = 0U;
    for (waitCounter = 0U; waitCounter < RS_STOP_WAIT_CNT; waitCounter++)
    {
        /* Assume that all have in reset state until proven otherwise below */
        stopComplete = 1U;

        for (channelId = 0U; channelId < RADIO_SEQUENCER_NUM_SEQUENCERS; channelId++)
        {
            uint8_t bfValue = 0xFF;
            recoveryAction = adrv904x_Sequencer_DbgState_BfGet(device,
                                                               NULL,
                                                               sequencerBaseAddr,
                                                               channelId,
                                                               &bfValue);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting dbg_state bitfield");
                goto cleanup;
            }

            /* Check the dbg_state bf value */
            if (bfValue != 0x4)
            {
                stopComplete = 0U;
            }
        }

        if (stopComplete == 0U)
        {
            /* Wait a bit before running the check again */
            recoveryAction = (adi_adrv904x_ErrAction_e)adi_common_hal_Wait_us(&device->common, RS_STOP_WAIT_INTERVAL_US);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while waiting for Radio Sequencer to stop");
                goto cleanup;
            }
        }
        else
        {
            /* All radio sequencers are executing the null pattern, exit the wait loop */
            break;
        }
    }

    /* Radio sequencer didn't stopped executing in the specified amount of time */
    if (stopComplete == 0U)
    {
        /* Set the RS status to not started */
        recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_DEVICE;
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV904X_ERRSRC_API,
                         ADI_COMMON_ERRCODE_TIMEOUT,
                         recoveryAction,
                         ADI_NO_VARIABLE,
                         "Radio sequencer didn't stopped executing in the specified amount of time");
        goto cleanup;
    }


    /* Load new RS image */
    recoveryAction = adi_adrv904x_RadioSequencerImageLoad(device, rsBinaryInfoPtr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while loading RS image");
        goto cleanup;
    }

    /* Initialize the command structure */
    cmdStruct.numberPatterns = ADRV904X_HTOCL(device->devStateInfo.rsImageHeader.numberPatterns);
    cmdStruct.sequenceLookupTableCrc = ADRV904X_HTOCL(device->devStateInfo.rsImageHeader.lookupCrc);

    for (uint32_t i = 0U; i < device->devStateInfo.rsImageHeader.numberPatterns; i++)
    {
        cmdStruct.sequenceLookupTable[i] = ADRV904X_HTOCL(device->devStateInfo.lookupTable[i]);
    }

    /* Issue mailbox cmd to DFE to reload its copy of pattern lookup table */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_RS_PATTERN_LUT_SET,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while sending command to update pattern lookup table");
        goto cleanup;
    }

    if (respStruct.status != ADI_ADRV904X_DFE_SVC_ERR_CODE_NO_ERROR)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_DEVICE;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, respStruct.status, "Unexpected DFE response");
        goto cleanup;
    }

    /* Re-initialize the XBAR settings */
    for (uint32_t idx = 0; idx < ADI_ADRV904X_RADIO_SEQ_XBAR_SIZE; idx++)
    {
        recoveryAction = adrv904x_Sequencer_XbarCtrlSel_BfSet(device,
                                                              NULL,
                                                              sequencerBaseAddr,
                                                              idx,
                                                              (device->devStateInfo.xbarSettings[idx] >> 4u));
                                                              if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting xbar control sel");
            goto cleanup;
        }

        recoveryAction = adrv904x_Sequencer_XbarSequencerSel_BfSet(device,
                                                                   NULL,
                                                                   sequencerBaseAddr,
                                                                   idx,
                                                                   (device->devStateInfo.xbarSettings[idx] & 0x0F));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting xbar sequencer sel");
            goto cleanup;
        }
    }

    /* Set relevant RS registers to reset value */
    for (channelId = 0U; channelId < RADIO_SEQUENCER_NUM_SEQUENCERS; channelId++)
    {
        recoveryAction = adrv904x_Sequencer_ExternalJumpEnable_BfSet(device,
                                                                     NULL,
                                                                     sequencerBaseAddr,
                                                                     channelId,
                                                                     0x00);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while resetting external_jump_enable bitfield");
            goto cleanup;
        }

        recoveryAction = adrv904x_Sequencer_ExternalJumpPointer_BfSet(device,
                                                                      NULL,
                                                                      sequencerBaseAddr,
                                                                      channelId,
                                                                      0xFFF0);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while resetting external_jump_pointer bitfield");
            goto cleanup;
        }
    }

    /* Set GPRs registers to reset value */
    for (uint32_t idx = 0U; idx < (RS_TOTAL_NUM_OF_SEQUENCERS * RS_NUM_GPR_PER_SEQUENCERS); idx++)
    {
        recoveryAction = adrv904x_Sequencer_GeneralPurposeReg_BfSet(device,
                                                                    NULL,
                                                                    sequencerBaseAddr,
                                                                    channelId,
                                                                    0);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while resetting general_purpose_reg bitfield");
            goto cleanup;
        }
    }

    recoveryAction = adrv904x_RadioSequencerApplyImmediately(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting RS Apply Immediately");
        goto cleanup;
    }


    /* Clear the rsStarted flag so RadioSequencerStart will re-init RS */
    device->devStateInfo.rsStarted = ADI_FALSE;

    /* Restart RS radio operation if it was in operation previous to this
     * call to Reload(). Otherwise, leave the RS in reset, waiting for the
     * user to call RadioStart() when ready to resume radio operation.
     */
    if (rsRadioRunning)
    {
        recoveryAction = adi_adrv904x_RadioSequencerRadioStart(device);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to start the RS radio operation");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrPulseExtractAndWrite(adi_adrv904x_Device_t* const                        device,
                                                                      const adi_adrv904x_CpuProfileBinaryInfo_t* const    cpuProfileBinaryInfoPtr)
{
        adi_adrv904x_ErrAction_e        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    FILE*                           cpuProfileFilePtr   = NULL;

    uint32_t                        fileSize            = 0U;
    adrv904x_DeviceProfile_t* const deviceProfile       = NULL;
    uint8_t                         pulseSelect         = 0U;
    uint32_t                        offset              = 0U;
    uint32_t                        txConfigOffset      = 0U;
    uint32_t                        i                   = 0U;
    uint8_t                         chanAssignArray[ADI_ADRV904X_MAX_CHANNELS] = { 0U };

    adrv904x_TxDfeCfrConfig_t cfrConfig;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuProfileBinaryInfoPtr, cleanup);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuProfileBinaryInfoPtr->filePath, cleanup);

    ADI_LIBRARY_MEMSET(&cfrConfig, 0, sizeof(adrv904x_TxDfeCfrConfig_t));

    if (ADI_LIBRARY_STRNLEN((const char*)cpuProfileBinaryInfoPtr->filePath, ADI_ADRV904X_MAX_FILE_LENGTH) == ADI_ADRV904X_MAX_FILE_LENGTH)
    {
        /* Path is not terminated */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileBinaryInfoPtr->filePath, "Unterminated path string");
        goto cleanup;
    }

    /* Open ARM binary file */
#ifdef __GNUC__
    cpuProfileFilePtr = ADI_LIBRARY_FOPEN((const char *)cpuProfileBinaryInfoPtr->filePath, "rb");
#else
    if (ADI_LIBRARY_FOPEN_S(&cpuProfileFilePtr, (const char *)cpuProfileBinaryInfoPtr->filePath, "rb") != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileBinaryInfoPtr->filePath, "Unable to open CPU Profile Binary. Please check filepath is correct");
        goto cleanup;
    }
#endif

    if (NULL == cpuProfileFilePtr)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to open CPU Profile Binary. Please check filepath is correct")
        goto cleanup;
    }

    /* Determine file size */
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, 0U, SEEK_END) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Seek to EOF Failed");
        goto cleanup;
    }

    /* ADI_LIBRARY_FTELL returns long type */
    fileSize = (uint32_t) ADI_LIBRARY_FTELL(cpuProfileFilePtr);

    /* Check that FW binary file is not empty */
    if (0U == fileSize)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileSize, "Zero Length CPU Profile Binary Detected");
        goto cleanup;
    }

    /* Check that FW Profile binary file size is not smaller than server size */
    if (fileSize < ADRV904X_DEVICE_PROFILE_SIZE_BYTES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fileSize, "CPU Profile Binary Size smaller than Server ADRV904X_DEVICE_PROFILE_SIZE_BYTES");
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ADRV904X_DEVICE_PROFILE_SIZE_BYTES, "ADRV904X_DEVICE_PROFILE_SIZE_BYTES bigger than CPU Profile Binary Size");
        goto cleanup;
    }

    /* Read chanAssign */
    offset = ADI_LIBRARY_OFFSETOF(adrv904x_RadioProfile_t, chanAssign);
    if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
        goto cleanup;
    }

    if ((ADI_LIBRARY_FREAD(&chanAssignArray[0], sizeof(deviceProfile->radioProfile.chanAssign), 1, cpuProfileFilePtr)) <= 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
        goto cleanup;
    }

    for (i = 0; i < ADI_ADRV904X_MAX_CHANNELS; ++i)
    {
        if (((device->devStateInfo.initializedChannels >> ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET) & (uint32_t)(1U << i)) == 0U)
        {
            /* Skip uninitialized channels */
            continue;
        }

        pulseSelect = chanAssignArray[i];
        switch (pulseSelect)
        {
            case 0:
                txConfigOffset = ADI_LIBRARY_OFFSETOF(adrv904x_DfeProfile_t, txConfig[0]);
                break;
            case 1:
                txConfigOffset = ADI_LIBRARY_OFFSETOF(adrv904x_DfeProfile_t, txConfig[1]);
                break;
            case 2:
                txConfigOffset = ADI_LIBRARY_OFFSETOF(adrv904x_DfeProfile_t, txConfig[2]);
                break;
            case 3:
                txConfigOffset = ADI_LIBRARY_OFFSETOF(adrv904x_DfeProfile_t, txConfig[3]);
                break;
            default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
                goto cleanup;
                break;
        }

        offset = ADI_LIBRARY_OFFSETOF(adrv904x_DeviceProfile_t, dfeProfile) +
                 txConfigOffset +
                 ADI_LIBRARY_OFFSETOF(adrv904x_DfeTxConfig_t, cfr);

        if (ADI_LIBRARY_FSEEK(cpuProfileFilePtr, offset, SEEK_SET) < 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuProfileFilePtr, "Unable to Rewind File Pointer for CPU Binary");
            goto cleanup;
        }

        if ((ADI_LIBRARY_FREAD(&cfrConfig, sizeof(adrv904x_TxDfeCfrConfig_t), 1, cpuProfileFilePtr)) <= 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, offset, "Error Reading Block of Data from CPU Binary File");
            goto cleanup;
        }

        if (cfrConfig.cfr0PulseData.numCoeffs != 0U)
        {
            recoveryAction = adrv904x_CfrCorrectionPulseWrite(device,
                                                              ADI_ADRV904X_CFR_PULSE0,
                                                              i,
                                                              cfrConfig.cfr0Interpolation,
                                                              (adi_adrv904x_CfrCorrectionPulse_t*)&cfrConfig.cfr0PulseData);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing pulse 0");
                goto cleanup;
            }
        }

        if (cfrConfig.cfr1PulseData.numCoeffs != 0U)
        {
            recoveryAction = adrv904x_CfrCorrectionPulseWrite(device,
                                                              ADI_ADRV904X_CFR_PULSE1,
                                                              i,
                                                              cfrConfig.cfr1Interpolation,
                                                              (adi_adrv904x_CfrCorrectionPulse_t *)&cfrConfig.cfr1PulseData);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing pulse 1");
                goto cleanup;
            }
        }
    }


    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
    if (cpuProfileFilePtr != NULL)
    {
        /* Close CPU Profile binary file */
        if (0 != ADI_LIBRARY_FCLOSE(cpuProfileFilePtr))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close CPU Profile Binary File");
        }
    }
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
