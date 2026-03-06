/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_adrv903x_radioctrl.c
* \brief Contains CPU features related function implementation defined in
* adi_adrv903x_radioctrl.h
*
* ADRV903X API Version: 2.12.1.4
*/

#include "adi_adrv903x_radioctrl.h"
#include "adi_adrv903x_cals.h"
#include "adi_adrv903x_cpu.h"
#include "adi_adrv903x_tx.h"
#include "adi_adrv903x_core.h"
#include "adi_adrv903x_types.h"
#include "adi_adrv903x_version_types.h"

#include "../../private/include/adrv903x_radioctrl.h"
#include "../../private/bf/adrv903x_bf_pll_mem_map.h"
#include "../../private/include/adrv903x_cpu_scratch_registers.h"
#include "../../private/include/adrv903x_cpu.h"
#include "../../private/include/adrv903x_rx.h"
#include "../../private/include/adrv903x_tx.h"
#include "../../private/include/adrv903x_gpio.h"
#include "../../private/include/adrv903x_radioctrl.h"
#include "../../private/bf/adrv903x_bf_tx_funcs.h"
#include "../../private/bf/adrv903x_bf_streamproc.h"
#include "../../private/include/adrv903x_init.h"
#include "../../private/include/adrv903x_stream_proc_types.h"


#define ADI_FILE    ADI_ADRV903X_FILE_PUBLIC_RADIOCTRL


/****************************************************************************
 * Helper functions
 ****************************************************************************
 */

/**
* \brief    Based off the structure of the stream_image.bin this function extracts
*           the size and offset of a single slice processor.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param binary Pointer to the start of slice data in stream_image.bin
* \param binaryImageSize Pointer to a short where the slice size in bytes will be placed
* \param binaryImageOffset Pointer to a short that will store the slice image offset 
*                           in the stream_image.bin file
*/
static void adrv903x_BinaryImageInfoGet(const uint8_t   binary[],
                                        uint32_t* const binaryImageSize,
                                        uint32_t* const binaryImageOffset)
{
    /* populating variables from binary array */
    *binaryImageSize = ((((uint32_t)binary[3U]) << 24U) | (((uint32_t)binary[2U]) << 16U) | (((uint32_t)binary[1U]) << 8U) | ((uint32_t)binary[0U]));
    *binaryImageOffset =  ((((uint32_t)binary[7U]) << 24U) | (((uint32_t)binary[6U]) << 16U) | (((uint32_t)binary[5U]) << 8U) | ((uint32_t)binary[4U]));
}

/**
* \brief    Based off the structure of the stream_image.bin this function extracts
*           the base address of a single slice.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param binary Pointer to the start of slice image in stream_image.bin
* \param word Pointer to a word where the slice base address will be stored
*/
static void adrv903x_BinaryParamsGetBaseAddr(const uint8_t binary[], uint32_t* const word)
{
    /* populating variables from binary array */
    *word = (((uint32_t)binary[3U]) << 24U) | (((uint32_t)binary[2U]) << 16U) | (((uint32_t)binary[1U]) << 8U) | (uint32_t)(binary[0U]);
}

/**
* \brief    Based off the structure of the stream_image.bin this function extracts
*           the number of streams and slice image size.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param binary Pointer to the start of slice image in stream_image.bin
* \param numberStreams Pointer to a byte that will store the number of streams for this slice
* \param imageSize Pointer to a short that will store the slice size in bytes
*/
static void adrv903x_BinaryParamsGetNumberStreamImageSize(  const uint8_t   binary[],
                                                            uint8_t* const  numberStreams,
                                                            uint32_t* const imageSize)
{
    *numberStreams = binary[0U];
    *imageSize = (((uint32_t)binary[3U]) << 8) | (uint32_t)(binary[2U]);
}

static adi_adrv903x_GpioPinSel_e  adrv903x_StreamGleanGpioNumberGet(uint32_t gpioSelectionArr[], adrv903x_StreamGpioFeatureSelection_e featureSelect)
{
    uint32_t gpioIdx = 0U;
    
    if (gpioSelectionArr == NULL)
    {
        return ADI_ADRV903X_GPIO_INVALID;
    }
    
    switch (featureSelect)
    {
        case ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT0:
        case ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT1:
        case ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT2:
        case ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT3:
        case ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT4:
        case ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT5:
        case ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT6:
        case ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT7:
            break;

        case ADRV903X_STREAM_GPIO_TX_ANTENNA_CAL:
        case ADRV903X_STREAM_GPIO_RX_ANTENNA_CAL:
        case ADRV903X_STREAM_GPIO_TX_PAP_FOR_EXT_LO0_UNLOCK:
        case ADRV903X_STREAM_GPIO_TX_PAP_FOR_EXT_LO1_UNLOCK:
            /* Fall Through */

        default:
            return ADI_ADRV903X_GPIO_INVALID;
            break;
    }

    for (gpioIdx = 0U; gpioIdx < ADI_ADRV903X_GPIO_COUNT; gpioIdx++)
    {
        if (gpioSelectionArr[gpioIdx] == (uint32_t)featureSelect)
        {
            return (adi_adrv903x_GpioPinSel_e)gpioIdx;
        }
    }
    
    return ADI_ADRV903X_GPIO_INVALID;
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_StreamImageWrite(adi_adrv903x_Device_t* const    device,
                                                               uint32_t                        byteOffset,
                                                               const uint8_t                   binary[],
                                                               uint32_t                        byteCount)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t binaryImageSize = 0U;
    uint32_t binaryImageOffset = 0U;
    uint32_t i = 0U;
    uint32_t addr = 0U;
    uint32_t mask = 0U;
    uint32_t word1 = 0U;
    uint32_t binOffset = 0U;
    uint32_t  streamCtl = 0U;
    uint8_t  byte1 = 0U;
    uint32_t imageSize = 0U;
    uint32_t imageOffset = 0U;
    uint32_t streamCtlAddr[] = {
        ADRV903X_ADDR_MAIN_STREAM_CTL,
        //todo- this value needs to be update when the kfa stream_reset bit is enabled
        ADRV903X_ADDR_KFA_STREAM_CTL,
        ADRV903X_ADDR_TX0_STREAM_CTL,
        ADRV903X_ADDR_TX1_STREAM_CTL,
        ADRV903X_ADDR_TX2_STREAM_CTL,
        ADRV903X_ADDR_TX3_STREAM_CTL,
        ADRV903X_ADDR_TX4_STREAM_CTL,
        ADRV903X_ADDR_TX5_STREAM_CTL,
        ADRV903X_ADDR_TX6_STREAM_CTL,
        ADRV903X_ADDR_TX7_STREAM_CTL,
        ADRV903X_ADDR_RX0_STREAM_CTL,
        ADRV903X_ADDR_RX1_STREAM_CTL,
        ADRV903X_ADDR_RX2_STREAM_CTL,
        ADRV903X_ADDR_RX3_STREAM_CTL,
        ADRV903X_ADDR_RX4_STREAM_CTL,
        ADRV903X_ADDR_RX5_STREAM_CTL,
        ADRV903X_ADDR_RX6_STREAM_CTL,
        ADRV903X_ADDR_RX7_STREAM_CTL,
        ADRV903X_ADDR_ORX0_STREAM_CTL,
        ADRV903X_ADDR_ORX1_STREAM_CTL
    };
    
    /* Default value assigned by hardware */
    uint32_t coreSliceControlDefaultVal = 0x80U;
    
    /* Number of bytes in the stream_image.bin header that contains
     * all the meta-data required to parse stream_image.bin
     * This also indicates the byte address where the core processor
     * starts.
     */
    static const uint32_t STREAM_METADATA_HEADER_SIZE_START_BYTE                            = 24U;
    static const uint32_t STREAM_METADATA_HEADER_SIZE_DATA_LENGTH                           = 4U;
    static const uint32_t STREAM_IMAGE_SLICE_INFO_START_BYTE                                = 28U;
    static const uint32_t STREAM_PROCESSOR_META_DATA_SIZE_BYTES                             = 8U;
    static const uint32_t STREAM_IMAGE_GPIO00_START_BYTE                                    = 190U;
    static const uint32_t STREAM_IMAGE_TXORXMAPPINGMODE_START_BYTE                          = 287U;
    static const uint32_t STREAM_IMAGE_TXORXOBSERVABILITY_START_BYTE                        = 289U;
    static const uint32_t STREAM_IMAGE_TXTOORXMAPPINGPINTABLEORX0_00_START_BYTE             = 291U;
    static const uint32_t STREAM_IMAGE_TXTOORXMAPPINGPINTABLEORX1_00_START_BYTE             = 355U;
    static const uint32_t STREAM_IMAGE_TXTOORXMAPPINAUTOSWITCHORXATTENENABLE_START_BYTE     = 419U;
    static const uint32_t STREAM_IMAGE_TXTOORXMAPPINAUTOSWITCHORXNCOENABLE_START_BYTE       = 420U;

    uint32_t FINAL_STREAM_IMAGE_HEADER_SIZE_BYTES = 0U;

    uint32_t gpioAssignments[ADI_ADRV903X_GPIO_COUNT] = { 0U };
    uint32_t gpioIdx = 0U;
    uint32_t idx = 0U;
    
    
    /* CPU stream download order: main, kfa, tx0/1/2/3/4/5/6/7, rx0/1/2/3/4/5/6/7, orx0/1 */
    static const uint32_t streamChannel[] = {
        0xFFFFFFFFU,
        0x0U,
        ADI_ADRV903X_TX0 << ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV903X_TX1 << ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV903X_TX2 << ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV903X_TX3 << ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV903X_TX4 << ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV903X_TX5 << ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV903X_TX6 << ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV903X_TX7 << ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV903X_RX0,
        ADI_ADRV903X_RX1,
        ADI_ADRV903X_RX2,
        ADI_ADRV903X_RX3,
        ADI_ADRV903X_RX4,
        ADI_ADRV903X_RX5,
        ADI_ADRV903X_RX6,
        ADI_ADRV903X_RX7,
        ADI_ADRV903X_ORX0,
        ADI_ADRV903X_ORX1
    };
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, binary, cleanup);

    if ((byteCount == 0U) ||
        ((byteCount % 4U) > 0U))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                byteCount,
                                "Invalid byteCount. Must be multiple of 4.");
        goto cleanup;
    }

    if ((byteOffset % 4U) > 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                byteCount,
                                "Invalid byteOffset. Must be multiple of 4.");
        goto cleanup;
    }

    /* When byteOffset is zero that indicates the first time this function.
     * has been called for a particular stream_image.bin so the header info
     * must be extracted. We need all the information extracted in a single
     * call which is why min chunk size is the same as FINAL_STREAM_IMAGE_HEADER_SIZE_BYTES.
     * */
    if (byteOffset == 0U)
    {
        /* Check that enough data was passed in to read the stream header size from the stream */
        if (byteCount < STREAM_METADATA_HEADER_SIZE_START_BYTE + STREAM_METADATA_HEADER_SIZE_DATA_LENGTH)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    byteCount,
                                    "Must pass in chunk size greater than current stream image header.");
            goto cleanup;
        }
        
        /* Glean stream header size from the binary itself */
        FINAL_STREAM_IMAGE_HEADER_SIZE_BYTES = adrv903x_CpuIntFromBytesGet(binary + STREAM_METADATA_HEADER_SIZE_START_BYTE, 
                                                                           STREAM_METADATA_HEADER_SIZE_DATA_LENGTH);
        
        /* Verify that enough data was passed in to read the entire stream header */
        if (byteCount < FINAL_STREAM_IMAGE_HEADER_SIZE_BYTES)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    byteCount,
                                    "Must pass in chunk size greater than current stream image header.");
            goto cleanup;
        }

#if ADI_ADRV903X_API_VERSION_RANGE_CHECK > 0
        {
            adi_adrv903x_Version_t version = { 0U, 0U, 0U, 0U };
            adi_adrv903x_Version_t minVersion = { 0U, 0U, 0U, 0U };
            static const uint32_t STREAM_IMAGE_VERSION_ID_START_BYTE = 8U;

                        version.majorVer        = adrv903x_CpuIntFromBytesGet(binary + STREAM_IMAGE_VERSION_ID_START_BYTE + 0U, 4U);
            version.minorVer        = adrv903x_CpuIntFromBytesGet(binary + STREAM_IMAGE_VERSION_ID_START_BYTE + 4U, 4U);
            version.maintenanceVer  = adrv903x_CpuIntFromBytesGet(binary + STREAM_IMAGE_VERSION_ID_START_BYTE + 8U, 4U);
            version.buildVer        = adrv903x_CpuIntFromBytesGet(binary + STREAM_IMAGE_VERSION_ID_START_BYTE + 12U, 4U);

            recoveryAction = adi_adrv903x_ApiVersionGet(device, &minVersion);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Stream Binary Version Get Issue");
                goto cleanup;
            }

            recoveryAction = adrv903x_ApiVersionRangeCheck(device, &version, &minVersion, &minVersion);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Stream Binary Range Check Issue");
                goto cleanup;
            }
        }
#endif

        /* Glean the 24 GPIO selections made in the Stream Binary */
        for (gpioIdx = 0U; gpioIdx < ADI_ADRV903X_GPIO_COUNT; gpioIdx++)
        {
            gpioAssignments[gpioIdx] = adrv903x_CpuIntFromBytesGet(binary + STREAM_IMAGE_GPIO00_START_BYTE + (gpioIdx * 4U), 4U);

            /* If stream feature is a basic stream input that requires no special handling, add it to generic Stream triggers list */
            switch (gpioAssignments[gpioIdx])
            {
                case (ADRV903X_STREAM_GPIO_TX_ANTENNA_CAL):             /* Fallthrough */
                case (ADRV903X_STREAM_GPIO_RX_ANTENNA_CAL):             /* Fallthrough */
                case (ADRV903X_STREAM_GPIO_TX_PAP_FOR_EXT_LO0_UNLOCK):  /* Fallthrough */
                case (ADRV903X_STREAM_GPIO_TX_PAP_FOR_EXT_LO1_UNLOCK):  /* Fallthrough */
                    device->devStateInfo.streamGpioMapping.streamGpInput[gpioIdx] = (adi_adrv903x_GpioPinSel_e)gpioIdx;
                    break;

                // Default: Do Nothing
                default:
                    device->devStateInfo.streamGpioMapping.streamGpInput[gpioIdx] = ADI_ADRV903X_GPIO_INVALID;
                    break;
            }

        }

        /* Update device handle Tx Orx Mapping config */
        device->devStateInfo.txToOrxMappingConfig.gpioSelect[0U] = adrv903x_StreamGleanGpioNumberGet(gpioAssignments, ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT0);
        device->devStateInfo.txToOrxMappingConfig.gpioSelect[1U] = adrv903x_StreamGleanGpioNumberGet(gpioAssignments, ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT1);
        device->devStateInfo.txToOrxMappingConfig.gpioSelect[2U] = adrv903x_StreamGleanGpioNumberGet(gpioAssignments, ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT2);
        device->devStateInfo.txToOrxMappingConfig.gpioSelect[3U] = adrv903x_StreamGleanGpioNumberGet(gpioAssignments, ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT3);
        device->devStateInfo.txToOrxMappingConfig.gpioSelect[4U] = adrv903x_StreamGleanGpioNumberGet(gpioAssignments, ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT4);
        device->devStateInfo.txToOrxMappingConfig.gpioSelect[5U] = adrv903x_StreamGleanGpioNumberGet(gpioAssignments, ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT5);
        device->devStateInfo.txToOrxMappingConfig.gpioSelect[6U] = adrv903x_StreamGleanGpioNumberGet(gpioAssignments, ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT6);
        device->devStateInfo.txToOrxMappingConfig.gpioSelect[7U] = adrv903x_StreamGleanGpioNumberGet(gpioAssignments, ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT7);

        device->devStateInfo.txToOrxMappingConfig.txObservability = (uint16_t)adrv903x_CpuIntFromBytesGet(binary + STREAM_IMAGE_TXORXOBSERVABILITY_START_BYTE, 2U);

        device->devStateInfo.txToOrxMappingConfig.mode = (adi_adrv903x_TxToOrxMappingMode_e)adrv903x_CpuIntFromBytesGet(binary + STREAM_IMAGE_TXORXMAPPINGMODE_START_BYTE, 1U);

        for (idx = 0U; idx < ADI_ADRV903X_TX_TO_ORX_MAPPING_PIN_TABLE_SIZE; idx++)
        {
            device->devStateInfo.txToOrxMappingConfig.pinTableOrx0[idx] = (adi_adrv903x_TxToOrxMappingPinTable_e)adrv903x_CpuIntFromBytesGet(binary + STREAM_IMAGE_TXTOORXMAPPINGPINTABLEORX0_00_START_BYTE + (idx * 4U), 4U);

            device->devStateInfo.txToOrxMappingConfig.pinTableOrx1[idx] = (adi_adrv903x_TxToOrxMappingPinTable_e)adrv903x_CpuIntFromBytesGet(binary + STREAM_IMAGE_TXTOORXMAPPINGPINTABLEORX1_00_START_BYTE + (idx * 4U), 4U);
        }
        device->devStateInfo.txToOrxMappingConfig.autoSwitchOrxAttenEnable = (uint8_t)adrv903x_CpuIntFromBytesGet(binary + STREAM_IMAGE_TXTOORXMAPPINAUTOSWITCHORXATTENENABLE_START_BYTE, 1U);

        device->devStateInfo.txToOrxMappingConfig.autoSwitchOrxNcoEnable = (uint8_t)adrv903x_CpuIntFromBytesGet(binary + STREAM_IMAGE_TXTOORXMAPPINAUTOSWITCHORXNCOENABLE_START_BYTE, 1U);

        for (i = 0; i < ADI_ADRV903X_STREAM_MAX; ++i)
        {
            adrv903x_BinaryImageInfoGet(&binary[STREAM_IMAGE_SLICE_INFO_START_BYTE + i * STREAM_PROCESSOR_META_DATA_SIZE_BYTES], &binaryImageSize, &binaryImageOffset);
            device->devStateInfo.chunkStreamImageSize[i] = binaryImageSize;
            device->devStateInfo.chunkStreamImageOffset[i] = binaryImageOffset;
        }

        /* Variables used to track the status of stream_image.bin parsing
         * across multiple calls to this function. 
         * */
        device->devStateInfo.currentStreamImageIndex = 0U;
        device->devStateInfo.currentStreamBinBaseAddr = 0U;
        device->devStateInfo.currentStreamBaseAddr = 0U;
        device->devStateInfo.currentStreamImageSize = 0U;
        device->devStateInfo.currentStreamNumberStreams = 0U;
    }

    for (i = device->devStateInfo.currentStreamImageIndex; i < ADI_ADRV903X_STREAM_MAX; ++i)
    {
        imageOffset = device->devStateInfo.chunkStreamImageOffset[i];
        
        /* NOTE: the i == 0 if-statements are for the most part required because core
         * slice control registers are in Direct SPI address range instead of AHB memory. 
         * */
        
        /* The core processor binary header starts after the stream_image.bin header.
         * But what gets written to the core AHB address includes the stream_image.bin
         * header. */
        if (i == 0U)
        {
            imageOffset = FINAL_STREAM_IMAGE_HEADER_SIZE_BYTES;
        }
        
        /* The if-statements below are to see if the current chunk of stream_image.bin
         * includes certain necessary information for a particular slice, e.g. the binary
         * base address which is the first word of a slice binary image. */
        if ((byteOffset <= imageOffset) &&
            ((byteOffset + byteCount) >= imageOffset + 4U))
        {
            binOffset = imageOffset - byteOffset;
            adrv903x_BinaryParamsGetBaseAddr(&binary[binOffset], &word1);
            device->devStateInfo.currentStreamBinBaseAddr = word1;
            device->devStateInfo.currentStreamImageSize = device->devStateInfo.chunkStreamImageSize[i];
        }

        if ((byteOffset <= imageOffset + 4U) &&
            ((byteOffset + byteCount) >= imageOffset + 8U))
        {
            binOffset = imageOffset + 4U - byteOffset;
            adrv903x_BinaryParamsGetBaseAddr(&binary[binOffset], &word1);
            device->devStateInfo.currentStreamBaseAddr = word1;
        }

        if ((byteOffset <= imageOffset + 8U) &&
            ((byteOffset + byteCount) >= imageOffset + 12U))
        {
            binOffset = imageOffset + 8U - byteOffset;
            adrv903x_BinaryParamsGetNumberStreamImageSize(&binary[binOffset], &byte1, &word1);
            device->devStateInfo.currentStreamNumberStreams = byte1;
        }
        
        /* If these tracking variables have been set for the current slice that means we are in a state
         * where we can write the chunk to ARM memory.
         * */
        if ((device->devStateInfo.currentStreamBinBaseAddr > 0U) &&
            (device->devStateInfo.currentStreamImageSize > 0U))
        {
            addr = streamCtlAddr[i];

            /* This if-statement will only be true the first time we have encounter a slice image because
             * currentStreamImageSize gets decremented on each function call, so we can use this as a way
             * to initialize slice startup items, e.g. putting the slice into reset while it's image is getting
             * written. */
            if (device->devStateInfo.currentStreamImageSize == device->devStateInfo.chunkStreamImageSize[i])
            {
                if ((device->devStateInfo.initializedChannels & streamChannel[i]) > 0U)
                {
                    //todo: this assumes kfa stream_reset is in spi-only re.g.s
                    if ((i == 0U) || (i == 1U))
                    {
                        streamCtl = coreSliceControlDefaultVal | (1U << ADRV903X_STREAM_RESET_BIT);
                        mask = 0xFFU;
                    }
                    else
                    {
                        /* Default for Rx stream_config_0 is zero*/
                        streamCtl = (1U << ADRV903X_RX_STREAM_RESET_BIT);
                        mask = 0xFFFFFFFFU;
                    }
                    recoveryAction = adi_adrv903x_Register32Write(device, NULL, addr, streamCtl, mask);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Stream Ctrl");
                        goto cleanup;
                    }
                }
            }

            /* This if statement is in the case a byte chunk has a boundary between two slice images so we only write
             * the remaining bytes of the current slice. 
             * */
            if (byteCount >= device->devStateInfo.currentStreamImageSize)
            {
                imageSize = device->devStateInfo.currentStreamImageSize;
            }
            else
            {
                imageSize = byteCount;
            }

            /* Load the stream image to memory, starting at the memory address specified by Stream_binary_base
             * if the corresponding channel has been initialized. 
             * */
            if ((device->devStateInfo.initializedChannels & streamChannel[i]) > 0)
            {
                            recoveryAction = adi_adrv903x_RegistersByteWrite(device,
                                                                 NULL,
                                                                 device->devStateInfo.currentStreamBinBaseAddr,
                                                                 binary,
                                                                 imageSize);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write stream binary");
                    goto cleanup;
                }
                        }
            else
            {
                //stream is disabled but no need to report an error
                recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
            }

            byteCount -= imageSize;
            byteOffset += imageSize;
            binary += imageSize;
            device->devStateInfo.currentStreamImageSize -= imageSize;
            device->devStateInfo.currentStreamBinBaseAddr += imageSize;

            /* After an entire slice image has been written we need to set the corresponding stream control register. */
            if (device->devStateInfo.currentStreamImageSize == 0U)
            {
                
                if ((device->devStateInfo.initializedChannels & streamChannel[i]) > 0U)
                {    
                    //todo: this assumes kfa stream_reset is in spi-only regs
                    if((i == 0U) || (i == 1U))
                    {
                        streamCtl = coreSliceControlDefaultVal;
                        mask = 0xFFU;
                        
                        /* Populate registers(stream_base_byte0) and (stream_base_byte1)with the lower 16 bits of the Stream_base. */
                        recoveryAction = adi_adrv903x_Register32Write(device,
                                                                      NULL,
                                                                      addr + ADRV903X_STREAM_BASE_BYTE0_REG_OFFSET,
                                                                      (uint8_t)(device->devStateInfo.currentStreamBaseAddr),
                                                                      mask);
                        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Stream Ctrl - Lower Byte");
                            goto cleanup;
                        }

                        recoveryAction = adi_adrv903x_Register32Write(device,
                                                                      NULL,
                                                                      addr + ADRV903X_STREAM_BASE_BYTE1_REG_OFFSET,
                                                                      (uint8_t)(device->devStateInfo.currentStreamBaseAddr >> 8U),
                                                                      mask);
                        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Stream Ctrl - Upper Byte");
                            goto cleanup;
                        }

                        /* Populate register  (last_stream_num)with the No_of_streams value. */
                        recoveryAction = adi_adrv903x_Register32Write(device,
                                                                      NULL,
                                                                      addr + ADRV903X_LAST_STREAM_NUMBER_REG_OFFSET,
                                                                      (uint8_t)(device->devStateInfo.currentStreamNumberStreams - 1U),
                                                                      mask);
                        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Stream Ctrl - Number of Streams");
                            goto cleanup;
                        }
                        
                    }
                    else 
                    {
                        mask = 0xFFFFFFFFU;
                        streamCtl = 0U;
                        
                        /* Populate registers(stream_base_byte0) and (stream_base_byte1)with the lower 16 bits of the Stream_base. */
                        streamCtl |= (device->devStateInfo.currentStreamBaseAddr & 0xFFU) << ADRV903X_RX_STREAM_BASE_BYTE0_BIT;
                        streamCtl |= ((device->devStateInfo.currentStreamBaseAddr >> 8U) & 0xFFU) << ADRV903X_RX_STREAM_BASE_BYTE1_BIT;

                        /* Populate register  (last_stream_num)with the No_of_streams value. */
                        streamCtl |= ((device->devStateInfo.currentStreamNumberStreams - 1U) & 0xFFU) << ADRV903X_RX_LAST_STREAM_NUMBER_BIT;
                        
                        /* Stream config 1 holds the base address & last stream number */
                        recoveryAction = adi_adrv903x_Register32Write(device, NULL, addr + 4U, streamCtl, mask);
                        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Stream Ctrl 1");
                            goto cleanup;
                        }
                        
                        /* Just clear reset bit on other channel processors */
                        streamCtl = 0U;
                        
                    }

                    recoveryAction = adi_adrv903x_Register32Write(device, NULL, addr, streamCtl, mask);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Stream Ctrl");
                        goto cleanup;
                    }
                }

                ++device->devStateInfo.currentStreamImageIndex;
                device->devStateInfo.currentStreamBinBaseAddr = 0U;
                device->devStateInfo.currentStreamBaseAddr = 0U;
                device->devStateInfo.currentStreamImageSize = 0U;
                device->devStateInfo.currentStreamNumberStreams = 0U;
            }
            else
            {
                //stream is disabled but no need to report an error
                recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
            }
        }
        else
        {
            break;
        }

        if (byteCount == 0U)
        {
            break;
        }
    }

    if (device->devStateInfo.currentStreamImageIndex == ADI_ADRV903X_STREAM_MAX)
    {
        device->devStateInfo.devState = (adi_adrv903x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV903X_STATE_STREAMLOADED);
        recoveryAction  = ADI_ADRV903X_ERR_ACT_NONE;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxTxEnableSet(adi_adrv903x_Device_t* const   device,
                                                            const uint32_t                 orxChannelMask,
                                                            const uint32_t                 orxChannelEnable,
                                                            const uint32_t                 rxChannelMask,
                                                            const uint32_t                 rxChannelEnable,
                                                            const uint32_t                 txChannelMask,
                                                            const uint32_t                 txChannelEnable)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);

#if ADI_ADRV903X_RADIOCTRL_RANGE_CHECK > 0
    recoveryAction = adrv903x_RxTxEnableSetRangeCheck(device, orxChannelMask, orxChannelEnable, rxChannelMask, rxChannelEnable, txChannelMask, txChannelEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxTxEnableSetRangeCheck Issue");
        goto cleanup;
    }
#endif

    /*Enable requested Rx Channel signal chains*/
    recoveryAction = adrv903x_RxEnableSet(device, rxChannelMask, rxChannelEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxEnableSet Issue");
        goto cleanup;
    }

    /* Enable requested ORx Channel signal chains */
    recoveryAction = adrv903x_OrxEnableSet(device, orxChannelMask, orxChannelEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "OrxEnableSet Issue");
        goto cleanup;
    }

    /* Enable requested Tx Channel signal chains */
    recoveryAction = adrv903x_TxEnableSet(device, txChannelMask, txChannelEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TxEnableSet Issue");
        goto cleanup;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxTxEnableGet(adi_adrv903x_Device_t* const    device,
                                                            uint32_t* const                 orxChannelMask,
                                                            uint32_t* const                 rxChannelMask,
                                                            uint32_t* const                 txChannelMask)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t regVal = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, orxChannelMask, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxChannelMask, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txChannelMask, cleanup);

    /* Get RX Channel Enabling bit mask */
    recoveryAction =  adrv903x_Core_RadioControlInterfaceRxSpiEn_BfGet(device,
                                                                       NULL,
                                                                       (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                       &regVal);
    
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RX Channel status get failed.");
        goto cleanup;
    }
    *rxChannelMask = (uint32_t) regVal;

    /* Get ORX Channel Enabling bit mask */
    recoveryAction =  adrv903x_Core_RadioControlInterfaceOrxSpiEn_BfGet(device,
                                                                        NULL,
                                                                        (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                        &regVal);
    
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "ORX Channel status get failed.");
        goto cleanup;
    }
    *orxChannelMask = ((uint32_t) regVal) << ADI_ADRV903X_MAX_RX_ONLY;
    
    /* Get TX Channel Enabling bit mask */
    recoveryAction =  adrv903x_Core_RadioControlInterfaceTxSpiEn_BfGet(device,
                                                                       NULL,
                                                                       (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                       &regVal);
    
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TX Channel status get failed.");
        goto cleanup;
    }
    *txChannelMask = (uint32_t) regVal;
    
cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_ChannelEnableGet(adi_adrv903x_Device_t* const    device,
                                                               uint32_t* const                orxChannelMask,
                                                               uint32_t* const                rxChannelMask,
                                                               uint32_t* const                txChannelMask)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t                  tmpByte        = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, orxChannelMask, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxChannelMask, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txChannelMask, cleanup);

    *orxChannelMask = 0U;
    *rxChannelMask = 0U;
    *txChannelMask = 0U;
    
    recoveryAction = adrv903x_Core_RadioControlInterfaceTxonReadback_BfGet(device,
                                                                           NULL,
                                                                           ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                           &tmpByte);
    
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TxonReadback issue");
        goto cleanup;
    }

    *txChannelMask = tmpByte;

    recoveryAction = adrv903x_Core_RadioControlInterfaceRxonReadback_BfGet(device,
                                                                           NULL,
                                                                           ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                           &tmpByte);
    
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxonReadback issue");
        goto cleanup;
    }

    *rxChannelMask = tmpByte;

    recoveryAction = adrv903x_Core_RadioControlInterfaceOrxonReadback_BfGet(device,
                                                                            NULL,
                                                                            ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                            &tmpByte);
    
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "OrxonReadback issue");
        goto cleanup;
    }

    *orxChannelMask = tmpByte;

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);

}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RadioCtrlCfgSet(adi_adrv903x_Device_t* const            device,
                                                              adi_adrv903x_RadioCtrlModeCfg_t* const  radioCtrlCfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, radioCtrlCfg, cleanup);

#if ADI_ADRV903X_RADIOCTRL_RANGE_CHECK > 0
    recoveryAction = adrv903x_RadioCtrlCfgSetRangeCheck(device, radioCtrlCfg);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RadioCtrlCfgSetRangeCheck Issue");
        goto cleanup;
    }
#endif

    if (((device->devStateInfo.profilesValid & ADI_ADRV903X_RX_PROFILE_VALID) == ADI_ADRV903X_RX_PROFILE_VALID) &&
         (radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode != ADI_ADRV903X_RX_EN_INVALID_MODE) &&
         (radioCtrlCfg->rxRadioCtrlModeCfg.rxChannelMask != ADI_ADRV903X_RXOFF))
    {
        recoveryAction = adrv903x_RxRadioCtrlCfgSet(device, &radioCtrlCfg->rxRadioCtrlModeCfg);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxRadioCtrlCfgSet Issue");
            goto cleanup;
        }
    }

    if (((device->devStateInfo.profilesValid & ADI_ADRV903X_TX_PROFILE_VALID) == ADI_ADRV903X_TX_PROFILE_VALID) &&
         (radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode != ADI_ADRV903X_TX_EN_INVALID_MODE) &&
         (radioCtrlCfg->txRadioCtrlModeCfg.txChannelMask != ADI_ADRV903X_TXOFF))
    {
        recoveryAction = adrv903x_TxRadioCtrlCfgSet(device, &radioCtrlCfg->txRadioCtrlModeCfg);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TxRadioCtrlCfgSet Issue");
            goto cleanup;
        } 
    }
    
    if (((device->devStateInfo.profilesValid & ADI_ADRV903X_ORX_PROFILE_VALID) == ADI_ADRV903X_ORX_PROFILE_VALID) &&
     (radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV903X_ORX_EN_INVALID_MODE) &&
     (radioCtrlCfg->orxRadioCtrlModeCfg.orxChannelMask != ADI_ADRV903X_RXOFF))
    {
        recoveryAction = adrv903x_OrxRadioCtrlCfgSet(device, &radioCtrlCfg->orxRadioCtrlModeCfg);
        if(recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "OrxRadioCtrlCfgSet Issue");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RadioCtrlCfgGet(adi_adrv903x_Device_t* const            device,
                                                              const adi_adrv903x_RxChannels_e         rxChannel,
                                                              const adi_adrv903x_TxChannels_e         txChannel,
                                                              adi_adrv903x_RadioCtrlModeCfg_t* const  radioCtrlCfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, radioCtrlCfg, cleanup);

    if (rxChannel > ADI_ADRV903X_ORX1)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, rxChannel, "Invalid Rx/ORx Channel");
        goto cleanup;
    }

    if (txChannel > ADI_ADRV903X_TX7)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel");
        goto cleanup;
    }
    
    if ((txChannel == ADI_ADRV903X_TXOFF) &&
        (rxChannel == ADI_ADRV903X_RXOFF))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "No valid Tx and Rx channels");
        goto cleanup;
    }

    if ((rxChannel >= ADI_ADRV903X_RX0) && (rxChannel <= ADI_ADRV903X_RX7))
    {
        recoveryAction = adrv903x_RxRadioCtrlCfgGet(device, rxChannel, &radioCtrlCfg->rxRadioCtrlModeCfg);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxRadioCtrlCfgGet Issue");
            goto cleanup;
        }
    }

    if ((txChannel >= ADI_ADRV903X_TX0) && (txChannel <= ADI_ADRV903X_TX7))
    {
        recoveryAction = adrv903x_TxRadioCtrlCfgGet(device, txChannel, &radioCtrlCfg->txRadioCtrlModeCfg);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TxRadioCtrlCfgGet Issue");
            goto cleanup;
        }
    }

    if ((rxChannel >= ADI_ADRV903X_ORX0) && (rxChannel <= ADI_ADRV903X_ORX1))
    {
        recoveryAction = adrv903x_OrxRadioCtrlCfgGet(device, rxChannel, &radioCtrlCfg->orxRadioCtrlModeCfg);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "OrxRadioCtrlCfgGet Issue");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_LoFrequencySet(adi_adrv903x_Device_t* const          device,
                                                             const adi_adrv903x_LoConfig_t* const  loConfig)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;

    adrv903x_CpuCmd_SetLoFreq_t loInfo;
    adrv903x_CpuCmd_SetLoFreqResp_t cmdRsp;
    adrv903x_CpuErrorCode_e cpuErrorCode = ADRV903X_CPU_SYSTEM_SIMULATED_ERROR;

    uint8_t writeGpIntPin1Bool = 0U;
    uint8_t writeGpIntPin0Bool = 0U;
    uint8_t writePllUnlockMaskBool[ADI_ADRV903X_MAX_TXCHANNELS];

    uint8_t tmpPllRampDownMask = 0U;
    uint8_t origPllRampDownMask[ADI_ADRV903X_MAX_TXCHANNELS];
    uint8_t i = 0U;
    uint8_t pllUnlockRampDownMask = 0U;
    uint32_t tmpTxChannelMask = 0U;

    uint8_t  gpIntMaskRegsOrig[4] = { 0U, 0U, 0U, 0U };
    uint8_t  gpIntMaskRegsTmp[4]  = { 0U, 0U, 0U, 0U };
    uint8_t* gpIntMaskPin1Byte9   = &gpIntMaskRegsOrig[0];
    uint8_t* gpIntMaskPin0Byte9   = &gpIntMaskRegsOrig[2];

    static const uint8_t GPINT_RF1_PLL_OVERRANGE_BYTE9_MASK = (1U << 4U);
    static const uint8_t GPINT_RF0_PLL_OVERRANGE_BYTE9_MASK = (1U << 5U);
    static const uint8_t GPINT_RF1_PLL_UNLOCK_BYTE9_MASK    = (1U << 7U);
    static const uint8_t GPINT_RF0_PLL_UNLOCK_BYTE10_MASK   = (1U << 0U);
        /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, loConfig, cleanup);

    ADI_LIBRARY_MEMSET(&loInfo, 0, sizeof(adrv903x_CpuCmd_SetLoFreq_t));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv903x_CpuCmd_SetLoFreqResp_t));
    ADI_LIBRARY_MEMSET(&writePllUnlockMaskBool, 0, sizeof(writePllUnlockMaskBool));
    ADI_LIBRARY_MEMSET(&origPllRampDownMask, 0, sizeof(origPllRampDownMask));

#if ADI_ADRV903X_RADIOCTRL_RANGE_CHECK > 0
    recoveryAction = adrv903x_LoFrequencySetRangeCheck(device, loConfig);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "LoFrequencySetRangeCheck Issue");
        goto cleanup;
    }
#endif

        /* Read GPINT registers */
    recoveryAction = adi_adrv903x_RegistersByteRead(device, NULL, ADRV903X_ADDR_GPINT_MASK_PIN1_BYTE9, gpIntMaskPin1Byte9, NULL, 2U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Read GPINT_PIN1 issue");
        goto cleanup;
    }
    recoveryAction = adi_adrv903x_RegistersByteRead(device, NULL, ADRV903X_ADDR_GPINT_MASK_PIN0_BYTE9, gpIntMaskPin0Byte9, NULL, 2U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Read GPINT_PIN0 issue");
        goto cleanup;
    }
    ADI_LIBRARY_MEMCPY(gpIntMaskRegsTmp, gpIntMaskRegsOrig, sizeof(gpIntMaskRegsOrig));
    gpIntMaskPin1Byte9 = &gpIntMaskRegsTmp[0];
    gpIntMaskPin0Byte9 = &gpIntMaskRegsTmp[2];

    /* Based on loName determine the GPINT and PLL-unlock Tx rampdown to disable */
    switch (loConfig->loName) 
    {
    case ADI_ADRV903X_LO0:
        pllUnlockRampDownMask = (uint8_t)ADI_ADRV903X_RDT_RF0_PLL_UNLOCK;
        gpIntMaskPin1Byte9[1] |= GPINT_RF0_PLL_UNLOCK_BYTE10_MASK;
        gpIntMaskPin0Byte9[1] |= GPINT_RF0_PLL_UNLOCK_BYTE10_MASK;
        break;
    case ADI_ADRV903X_LO1:
        pllUnlockRampDownMask = (uint8_t)ADI_ADRV903X_RDT_RF1_PLL_UNLOCK;
        gpIntMaskPin1Byte9[0] |= GPINT_RF1_PLL_UNLOCK_BYTE9_MASK;
        gpIntMaskPin0Byte9[0] |= GPINT_RF1_PLL_UNLOCK_BYTE9_MASK;
        break;
            default:
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loConfig->loName, "Invalid LO selected for setting LO frequency");
        goto cleanup;
    }

    pllUnlockRampDownMask >>= 3U;

    /* Disable various GP INTs while changing frequency */
    gpIntMaskPin1Byte9[0] |= (GPINT_RF0_PLL_OVERRANGE_BYTE9_MASK | GPINT_RF1_PLL_OVERRANGE_BYTE9_MASK);
    gpIntMaskPin0Byte9[0] |= (GPINT_RF0_PLL_OVERRANGE_BYTE9_MASK | GPINT_RF1_PLL_OVERRANGE_BYTE9_MASK);
        /* Write GPINT registers if Byte9 or Byte10 have been changed */
    writeGpIntPin1Bool = (gpIntMaskPin1Byte9[0] != gpIntMaskRegsOrig[0]) || (gpIntMaskPin1Byte9[1] != gpIntMaskRegsOrig[1]);
    writeGpIntPin0Bool = (gpIntMaskPin0Byte9[0] != gpIntMaskRegsOrig[2]) || (gpIntMaskPin0Byte9[1] != gpIntMaskRegsOrig[3]);
    if (writeGpIntPin1Bool)
    {
        recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_GPINT_MASK_PIN1_BYTE9, gpIntMaskPin1Byte9, 2U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Write GPINT_PIN1 issue");
            goto cleanup;
        }
    }
    if (writeGpIntPin0Bool)
    {
        recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_GPINT_MASK_PIN0_BYTE9, gpIntMaskPin0Byte9, 2U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Write GPINT_PIN0 issue");
            goto cleanup;
        }
    }

    /* Store current PLL Unlock mask for ramp down configuration for each channel and update them temporarily */
    for (i = 0U; i < ADI_ADRV903X_MAX_TXCHANNELS; ++i)
    {
        tmpTxChannelMask = (uint32_t)((uint32_t)1U << i);

        if (((device->devStateInfo.initializedChannels >> ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET) & tmpTxChannelMask) == 0U)
        {
            /* skip for uninitialized channels */
            continue;
        }

        /* Retrieve the base address for selected tx channel */
        recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)tmpTxChannelMask, &txBaseAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, tmpTxChannelMask, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }

        /* Store pll unlock mask */
        recoveryAction = adrv903x_TxFuncs_PllUnlockMask_BfGet(device, NULL, txBaseAddr, &origPllRampDownMask[i]);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading Tx Pll Unlock Mask for ramp-down config");
            goto cleanup;
        }

        tmpPllRampDownMask =  origPllRampDownMask[i] | pllUnlockRampDownMask; /* 1 means disabled */
        /* store boolean on whether temporary values to be written are different from the original */
        writePllUnlockMaskBool[i] = (tmpPllRampDownMask != origPllRampDownMask[i]);

        /* update pll unlock mask if tmp value is different */
        if (writePllUnlockMaskBool[i])
        {
            recoveryAction = adrv903x_TxFuncs_PllUnlockMask_BfSet(device, NULL, txBaseAddr, tmpPllRampDownMask);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing temporary Tx Pll Unlock Mask for ramp-down config");
                goto cleanup;
            }
        }
    }

        /* Prepare the command payload */
    loInfo.loName = (adi_adrv903x_LoName_t) loConfig->loName;
    loInfo.loConfigSel = (adi_adrv903x_LoOption_t) ADRV903X_HTOCL(loConfig->loConfigSel) ;
    loInfo.loFrequency_Hz = (uint64_t) ADRV903X_HTOCLL(loConfig->loFrequency_Hz);
        /* Send command and receive response */
    recoveryAction = adrv903x_CpuCmdSend( device,
                                          ADI_ADRV903X_CPU_TYPE_0,
                                          ADRV903X_LINK_ID_0,
                                          ADRV903X_CPU_CMD_ID_SET_LO_FREQUENCY,
                                          (void*)&loInfo,
                                          sizeof(loInfo),
                                          (void*)&cmdRsp,
                                          sizeof(cmdRsp),
                                          &cmdStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

        /* Write back original GPINT settings */
    gpIntMaskPin1Byte9 = &gpIntMaskRegsOrig[0];
    gpIntMaskPin0Byte9 = &gpIntMaskRegsOrig[2];
    if (writeGpIntPin1Bool)
    {
        recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_GPINT_MASK_PIN1_BYTE9, gpIntMaskPin1Byte9, 2U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Write GPINT_PIN1 issue");
            goto cleanup;
        }
    }
    if (writeGpIntPin0Bool)
    {
        recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_GPINT_MASK_PIN0_BYTE9, gpIntMaskPin0Byte9, 2U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Write GPINT_PIN0 issue");
            goto cleanup;
        }
    }

    /* Restore Pll unlock masks for ramp-down configuration */
    for (i = 0; i < ADI_ADRV903X_MAX_TXCHANNELS; ++i) 
    {
        tmpTxChannelMask = (uint32_t)((uint32_t)1U << i);

        if ((((device->devStateInfo.initializedChannels >> ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET) & tmpTxChannelMask) == 0U) ||
            (writePllUnlockMaskBool[i] == 0U))
        {
            /* skip for uninitialized channels or unchanged Pll unlock masks*/
            continue;
        }

        /* Retrieve the base address for selected tx channel */
        recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)tmpTxChannelMask, &txBaseAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, tmpTxChannelMask, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }

        recoveryAction = adrv903x_TxFuncs_PllUnlockMask_BfSet(device, NULL, txBaseAddr, origPllRampDownMask[i]);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing temporary Tx Pll Unlock Mask for ramp-down config");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_LoFrequencyGet(adi_adrv903x_Device_t* const            device,
                                                             adi_adrv903x_LoConfigReadback_t* const  loConfig)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_CpuCmd_GetLoFreqResp_t cmdRsp;
    adrv903x_CpuCmd_GetLoFreq_t loCmd;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e cpuErrorCode = ADRV903X_CPU_SYSTEM_SIMULATED_ERROR;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, loConfig, cleanup);

    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv903x_CpuCmd_GetLoFreqResp_t));
    ADI_LIBRARY_MEMSET(&loCmd, 0, sizeof(adrv903x_CpuCmd_GetLoFreq_t));
    
    /* Executing the GET LO Freq command */ 
    loCmd.loName = (adi_adrv903x_LoName_t) loConfig->loName;

    /* Send command and receive response */
    recoveryAction = adrv903x_CpuCmdSend(   device,
                                            ADI_ADRV903X_CPU_TYPE_0,
                                            ADRV903X_LINK_ID_0,
                                            ADRV903X_CPU_CMD_ID_GET_LO_FREQUENCY,
                                            (void*)&loCmd,
                                            sizeof(loCmd),
                                            (void*)&cmdRsp,
                                            sizeof(cmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    loConfig->loFrequency_Hz = (uint64_t) ADRV903X_CTOHLL(cmdRsp.loFrequency_Hz);

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_CfgPllToChanCtrl(adi_adrv903x_Device_t* const device, 
	                                                            uint8_t rf0MuxTx0_3,
	                                                            uint8_t rf0MuxTx4_7, 
	                                                            uint8_t rf0MuxRx0_3,
	                                                            uint8_t rf0MuxRx4_7)
{
		adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
	adrv903x_CpuCmd_ChanCtrlToPlls_t ChanCtrlToPllsPayload; 
	adrv903x_CpuCmd_ChanCtrlToPllsResp_t cmdRsp; 
	adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
	adrv903x_CpuErrorCode_e cpuErrorCode = ADRV903X_CPU_SYSTEM_SIMULATED_ERROR;

	/* check Device pointer is not null */	
	ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
	ADI_ADRV903X_API_ENTRY(&device->common);
	
	ADI_LIBRARY_MEMSET(&ChanCtrlToPllsPayload, 0, sizeof(adrv903x_CpuCmd_ChanCtrlToPlls_t));
	ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv903x_CpuCmd_ChanCtrlToPllsResp_t));

	/* Prepare the command Payload */
	ChanCtrlToPllsPayload.rf0MuxTx0_3 = (uint8_t) rf0MuxTx0_3;
	ChanCtrlToPllsPayload.rf0MuxTx4_7 = (uint8_t) rf0MuxTx4_7;
	ChanCtrlToPllsPayload.rf0MuxRx0_3 = (uint8_t) rf0MuxRx0_3;
	ChanCtrlToPllsPayload.rf0MuxRx4_7 = (uint8_t) rf0MuxRx4_7;

	/* Send Command and Receive Response */
	recoveryAction = adrv903x_CpuCmdSend(device, 
		                                 ADI_ADRV903X_CPU_TYPE_0, 
		                                 ADRV903X_LINK_ID_0, 
		                                 ADRV903X_CPU_CMD_ID_SET_CHAN_TO_PLLS, 
		                                 (void*)&ChanCtrlToPllsPayload, 
		                                 sizeof(ChanCtrlToPllsPayload),
		                                 (void*)&cmdRsp,
		                                 sizeof(cmdRsp),
		                                 &cmdStatus);
	if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
	{
		ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
	}

cleanup:
	ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);		
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_LoLoopFilterSet(adi_adrv903x_Device_t* const device,
                                                              const adi_adrv903x_LoName_e loName,
                                                              const adi_adrv903x_LoLoopFilterCfg_t* const loLoopFilterConfig)
{
        adi_adrv903x_ErrAction_e recoveryAction     = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_CpuCmd_SetLoopfilter_t loopFilterPayload;
    adrv903x_CpuCmd_SetLoopfilterResp_t cmdRsp;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e cpuErrorCode = ADRV903X_CPU_SYSTEM_SIMULATED_ERROR;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, loLoopFilterConfig, cleanup);

    ADI_LIBRARY_MEMSET(&loopFilterPayload, 0, sizeof(adrv903x_CpuCmd_SetLoopfilter_t));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv903x_CpuCmd_SetLoopfilterResp_t));

#if ADI_ADRV903X_RADIOCTRL_RANGE_CHECK > 0
        recoveryAction = adrv903x_LoLoopFilterSetRangeCheck(device, loName, loLoopFilterConfig);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "LoLoopFilterRangeCheck Issue");
            goto cleanup;
        }
#endif

    loopFilterPayload.loName = (adi_adrv903x_LoName_t)(loName);
    loopFilterPayload.loopBandwidth = ADRV903X_HTOCL((uint32_t)(loLoopFilterConfig->loopBandwidth_kHz * 1000U));
    loopFilterPayload.phaseMargin = ADRV903X_HTOCL((uint32_t)loLoopFilterConfig->phaseMargin_degrees);
    
    /* Send command and receive response */
    recoveryAction = adrv903x_CpuCmdSend(   device,
                                            ADI_ADRV903X_CPU_TYPE_0,
                                            ADRV903X_LINK_ID_0,
                                            ADRV903X_CPU_CMD_ID_SET_LOOPFILTER,
                                            (void*)&loopFilterPayload,
                                            sizeof(loopFilterPayload),
                                            (void*)&cmdRsp,
                                            sizeof(cmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_LoLoopFilterGet(adi_adrv903x_Device_t* const device,
                                                              const adi_adrv903x_LoName_e loName,
                                                              adi_adrv903x_LoLoopFilterCfg_t* const loLoopFilterConfig)
{
        adi_adrv903x_ErrAction_e recoveryAction     = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_CpuCmd_GetLoopfilter_t loopFilterPayload;
    adrv903x_CpuCmd_GetLoopfilterResp_t cmdRsp;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e cpuErrorCode = ADRV903X_CPU_SYSTEM_SIMULATED_ERROR;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, loLoopFilterConfig, cleanup);

    ADI_LIBRARY_MEMSET(&loopFilterPayload, 0, sizeof(adrv903x_CpuCmd_GetLoopfilter_t));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv903x_CpuCmd_GetLoopfilterResp_t));

    if ((loName != ADI_ADRV903X_LO0) && (loName != ADI_ADRV903X_LO1))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, loName, "Invalid LO Name");
        goto cleanup;
    }
    loopFilterPayload.loName = (adi_adrv903x_LoName_t)(loName); 
    /* Send command and receive response */
    recoveryAction = adrv903x_CpuCmdSend(   device,
                                            ADI_ADRV903X_CPU_TYPE_0,
                                            ADRV903X_LINK_ID_0,
                                            ADRV903X_CPU_CMD_ID_GET_LOOPFILTER,
                                            (void*)&loopFilterPayload,
                                            sizeof(loopFilterPayload),
                                            (void*)&cmdRsp,
                                            sizeof(cmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    loLoopFilterConfig->loopBandwidth_kHz = (uint16_t) (ADRV903X_CTOHL(cmdRsp.loopBandwidth) / 1000);
    loLoopFilterConfig->phaseMargin_degrees = (uint8_t) ADRV903X_CTOHL(cmdRsp.phaseMargin);

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}
                                                              
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_PllStatusGet(adi_adrv903x_Device_t* const device,
                                                           uint32_t* const        pllLockStatus)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    static const uint8_t  CLK_PLL_LOCK_STATUS_SHIFT = 0U;
    static const uint8_t  LO0_LOCK_STATUS_SHIFT = 1U;
    static const uint8_t  LO1_LOCK_STATUS_SHIFT = 2U;
    static const uint8_t  SERDES_PLL_LOCK_STATUS_SHIFT = 3U;
    static const uint32_t LOCK_BIT_MASK = 0x00000001U;

    uint8_t pllLockStatusRead = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, pllLockStatus, cleanup);

    /* Clear status of all PLLs */
    *pllLockStatus = 0U;

    /* Read CLK Pll status */
    recoveryAction =  adrv903x_PllMemMap_SynLock_BfGet(device,
                                                       NULL,
                                                       ADRV903X_BF_DIGITAL_CORE_JESD_CLKPLL,
                                                       &pllLockStatusRead);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get CLK PLL Lock Status Failed.");
        goto cleanup;
    }

    /* Update pllLockStatus bit 0 with Clk Pll Status */
    *pllLockStatus |= ((uint32_t)pllLockStatusRead & LOCK_BIT_MASK) << CLK_PLL_LOCK_STATUS_SHIFT;

    /* Read LO0 Pll status */
    recoveryAction =  adrv903x_PllMemMap_SynLock_BfGet(device,
                                                       NULL,
                                                       ADRV903X_BF_DIGITAL_CORE_EAST_RFPLL,
                                                       &pllLockStatusRead);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get LO0 Lock Status Failed.");
        goto cleanup;
    }

    /* Update pllLockStatus bit 1 with LO0 Pll Status */
    *pllLockStatus |= ((uint32_t)pllLockStatusRead & LOCK_BIT_MASK) << LO0_LOCK_STATUS_SHIFT;

    /* Read LO1 Pll status */
    recoveryAction =  adrv903x_PllMemMap_SynLock_BfGet(device,
                                                       NULL,
                                                       ADRV903X_BF_DIGITAL_CORE_WEST_RFPLL,
                                                       &pllLockStatusRead);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get LO1 Lock Status Failed.");
        goto cleanup;
    }

    /* Update pllLockStatus bit 2 with LO1 Pll Status */
    *pllLockStatus |= ((uint32_t)pllLockStatusRead & LOCK_BIT_MASK) << LO1_LOCK_STATUS_SHIFT;

    /* Read SERDES Pll status */
    recoveryAction =  adrv903x_PllMemMap_SynLock_BfGet(device,
                                                       NULL,
                                                       ADRV903X_BF_DIGITAL_CORE_JESD_SERDES_PLL,
                                                       &pllLockStatusRead);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get SERDES PLL Lock Status Failed.");
        goto cleanup;
    }

    /* Update pllLockStatus bit 3 with Serdes Pll Status */
    *pllLockStatus |= ((uint32_t)pllLockStatusRead & LOCK_BIT_MASK) << SERDES_PLL_LOCK_STATUS_SHIFT;

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxTxLoFreqGet(adi_adrv903x_Device_t* const            device,
                                                            adi_adrv903x_RxTxLoFreqReadback_t* const  rxTxLoFreq)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_CpuCmd_GetRxTxLoFreqResp_t cmdRsp;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e cpuErrorCode = ADRV903X_CPU_SYSTEM_SIMULATED_ERROR;
    uint8_t i = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxTxLoFreq, cleanup);

    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv903x_CpuCmd_GetRxTxLoFreqResp_t));
    ADI_LIBRARY_MEMSET(rxTxLoFreq, 0, sizeof(adi_adrv903x_RxTxLoFreqReadback_t));
    
    /* Send command and receive response */
    recoveryAction = adrv903x_CpuCmdSend(   device,
                                            ADI_ADRV903X_CPU_TYPE_0,
                                            ADRV903X_LINK_ID_0,
                                            ADRV903X_CPU_CMD_ID_GET_RXTXLOFREQ,
                                            (void*) NULL, /* No CMD load needed for this cmd */
                                            0U,
                                            (void*)&cmdRsp,
                                            sizeof(cmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* Copying CPU responses back to user structure */
    for(i = 0 ; i < ADI_ADRV903X_MAX_RX_ONLY ; i++)
    {
        rxTxLoFreq->rxLoName[i] =  (adi_adrv903x_LoName_e)ADRV903X_CTOHL((uint32_t)cmdRsp.rxLoName[i]);
        rxTxLoFreq->rxFreq_Khz[i] =  ADRV903X_CTOHL(cmdRsp.rxFreq[i]);
    }

    for (i = 0U; i < ADI_ADRV903X_MAX_TXCHANNELS; i++)
    {
        rxTxLoFreq->txLoName[i] =  (adi_adrv903x_LoName_e)ADRV903X_CTOHL((uint32_t)cmdRsp.txLoName[i]);
        rxTxLoFreq->txFreq_Khz[i] =  ADRV903X_CTOHL(cmdRsp.txFreq[i]);
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RadioCtrlTxRxEnCfgSet(adi_adrv903x_Device_t* const                    device,
                                                                    const adi_adrv903x_RadioCtrlTxRxEnCfg_t* const  txRxEnCfg,
                                                                    uint8_t                                         pinIndex,
                                                                    uint8_t                                         configSel)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t i = 0U;

    adrv903xCoreBfSet8FnPtr_t adrvCoreRadioControlInterfaceTRxEnPinXBfSet[4][ADI_ADRV903X_TRX_CTRL_PIN_COUNT] = 
    {
        {
            adrv903x_Core_RadioControlInterfaceTxEnPin0_BfSet,
            adrv903x_Core_RadioControlInterfaceTxEnPin1_BfSet,
            adrv903x_Core_RadioControlInterfaceTxEnPin2_BfSet,
            adrv903x_Core_RadioControlInterfaceTxEnPin3_BfSet,
            adrv903x_Core_RadioControlInterfaceTxEnPin4_BfSet,
            adrv903x_Core_RadioControlInterfaceTxEnPin5_BfSet,
            adrv903x_Core_RadioControlInterfaceTxEnPin6_BfSet,
            adrv903x_Core_RadioControlInterfaceTxEnPin7_BfSet
        },
        {
            adrv903x_Core_RadioControlInterfaceTxAntEnPin0_BfSet, 
            adrv903x_Core_RadioControlInterfaceTxAntEnPin1_BfSet, 
            adrv903x_Core_RadioControlInterfaceTxAntEnPin2_BfSet, 
            adrv903x_Core_RadioControlInterfaceTxAntEnPin3_BfSet, 
            adrv903x_Core_RadioControlInterfaceTxAntEnPin4_BfSet, 
            adrv903x_Core_RadioControlInterfaceTxAntEnPin5_BfSet, 
            adrv903x_Core_RadioControlInterfaceTxAntEnPin6_BfSet, 
            adrv903x_Core_RadioControlInterfaceTxAntEnPin7_BfSet 
        },
        { 
            adrv903x_Core_RadioControlInterfaceRxEnPin0_BfSet, 
            adrv903x_Core_RadioControlInterfaceRxEnPin1_BfSet, 
            adrv903x_Core_RadioControlInterfaceRxEnPin2_BfSet, 
            adrv903x_Core_RadioControlInterfaceRxEnPin3_BfSet, 
            adrv903x_Core_RadioControlInterfaceRxEnPin4_BfSet, 
            adrv903x_Core_RadioControlInterfaceRxEnPin5_BfSet, 
            adrv903x_Core_RadioControlInterfaceRxEnPin6_BfSet, 
            adrv903x_Core_RadioControlInterfaceRxEnPin7_BfSet 
        },
        { 
            adrv903x_Core_RadioControlInterfaceRxAntEnPin0_BfSet, 
            adrv903x_Core_RadioControlInterfaceRxAntEnPin1_BfSet, 
            adrv903x_Core_RadioControlInterfaceRxAntEnPin2_BfSet, 
            adrv903x_Core_RadioControlInterfaceRxAntEnPin3_BfSet, 
            adrv903x_Core_RadioControlInterfaceRxAntEnPin4_BfSet, 
            adrv903x_Core_RadioControlInterfaceRxAntEnPin5_BfSet, 
            adrv903x_Core_RadioControlInterfaceRxAntEnPin6_BfSet, 
            adrv903x_Core_RadioControlInterfaceRxAntEnPin7_BfSet
        }
    };

    adi_adrv903x_TxRxEnPin_e arrayTxRxPin[ADI_ADRV903X_TRX_CTRL_PIN_COUNT] = 
    {
        ADI_ADRV903X_TXRXEN_PIN0,
        ADI_ADRV903X_TXRXEN_PIN1,
        ADI_ADRV903X_TXRXEN_PIN2,
        ADI_ADRV903X_TXRXEN_PIN3,
        ADI_ADRV903X_TXRXEN_PIN4,
        ADI_ADRV903X_TXRXEN_PIN5,
        ADI_ADRV903X_TXRXEN_PIN6,
        ADI_ADRV903X_TXRXEN_PIN7
    };

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txRxEnCfg, cleanup);

    if (pinIndex < ADI_ADRV903X_TXRXEN_PIN0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, pinIndex, "Invalid pinIndex");
        goto cleanup;
    }

    if ((configSel < ADI_ADRV903X_TXRXEN_TX_ENABLE_MAP) ||
        (configSel > ADI_ADRV903X_TXRXEN_ALL))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,  recoveryAction, configSel, "Invalid configSel");
        goto cleanup;
    }

    for (i = 0; i < ADI_ADRV903X_TRX_CTRL_PIN_COUNT; i++)
    {
        if (ADRV903X_BF_EQUAL(pinIndex, arrayTxRxPin[i]))
        {
            if (ADRV903X_BF_EQUAL(configSel, ADI_ADRV903X_TXRXEN_TX_ENABLE_MAP))
            {
                recoveryAction = adrvCoreRadioControlInterfaceTRxEnPinXBfSet[0][i](device,
                                                                                   NULL,
                                                                                   ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                                   txRxEnCfg->txEnMapping[i]);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API TX Radio Control En Pin Failed.");
                    goto cleanup;
                }
            }

            if (ADRV903X_BF_EQUAL(configSel, (uint8_t)ADI_ADRV903X_TXRXEN_TX_ALTENABLE_MAP))
            {
                recoveryAction = adrvCoreRadioControlInterfaceTRxEnPinXBfSet[1][i](device,
                                                                                   NULL,
                                                                                   ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                                   txRxEnCfg->txAltMapping[i]);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API TX Radio Control Alternate En Pin Failed.");
                    goto cleanup; 
                }
            }

            if (ADRV903X_BF_EQUAL(configSel, ADI_ADRV903X_TXRXEN_RX_ENABLE_MAP))
            {
                recoveryAction = adrvCoreRadioControlInterfaceTRxEnPinXBfSet[2][i](device,
                                                                                   NULL,
                                                                                   ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                                   txRxEnCfg->rxEnMapping[i]);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API RX Radio Control En Pin Failed.");
                    goto cleanup;
                }
            }

            if (ADRV903X_BF_EQUAL(configSel, ADI_ADRV903X_TXRXEN_RX_ALTENABLE_MAP))
            {
                recoveryAction = adrvCoreRadioControlInterfaceTRxEnPinXBfSet[3][i](device,
                                                                                   NULL,
                                                                                   ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                                   txRxEnCfg->rxAltMapping[i]);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API RX Radio Control Alternate En Pin Failed.");
                    goto cleanup;
                }
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RadioCtrlTxRxEnCfgGet(adi_adrv903x_Device_t* const              device,
                                                                    adi_adrv903x_RadioCtrlTxRxEnCfg_t* const  txRxEnCfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t i = 0U;

    adrv903xCoreBfGet8FnPtr_t adrvCoreRadioControlInterfaceTRxEnPinXBfGet[4][ADI_ADRV903X_TRX_CTRL_PIN_COUNT] = 
    {
        {
            adrv903x_Core_RadioControlInterfaceTxEnPin0_BfGet,
            adrv903x_Core_RadioControlInterfaceTxEnPin1_BfGet,
            adrv903x_Core_RadioControlInterfaceTxEnPin2_BfGet,
            adrv903x_Core_RadioControlInterfaceTxEnPin3_BfGet,
            adrv903x_Core_RadioControlInterfaceTxEnPin4_BfGet,
            adrv903x_Core_RadioControlInterfaceTxEnPin5_BfGet,
            adrv903x_Core_RadioControlInterfaceTxEnPin6_BfGet,
            adrv903x_Core_RadioControlInterfaceTxEnPin7_BfGet
        },
        {
            adrv903x_Core_RadioControlInterfaceTxAntEnPin0_BfGet, 
            adrv903x_Core_RadioControlInterfaceTxAntEnPin1_BfGet, 
            adrv903x_Core_RadioControlInterfaceTxAntEnPin2_BfGet, 
            adrv903x_Core_RadioControlInterfaceTxAntEnPin3_BfGet, 
            adrv903x_Core_RadioControlInterfaceTxAntEnPin4_BfGet, 
            adrv903x_Core_RadioControlInterfaceTxAntEnPin5_BfGet, 
            adrv903x_Core_RadioControlInterfaceTxAntEnPin6_BfGet, 
            adrv903x_Core_RadioControlInterfaceTxAntEnPin7_BfGet 
        },
        { 
            adrv903x_Core_RadioControlInterfaceRxEnPin0_BfGet, 
            adrv903x_Core_RadioControlInterfaceRxEnPin1_BfGet, 
            adrv903x_Core_RadioControlInterfaceRxEnPin2_BfGet, 
            adrv903x_Core_RadioControlInterfaceRxEnPin3_BfGet, 
            adrv903x_Core_RadioControlInterfaceRxEnPin4_BfGet, 
            adrv903x_Core_RadioControlInterfaceRxEnPin5_BfGet, 
            adrv903x_Core_RadioControlInterfaceRxEnPin6_BfGet, 
            adrv903x_Core_RadioControlInterfaceRxEnPin7_BfGet 
        },
        { 
            adrv903x_Core_RadioControlInterfaceRxAntEnPin0_BfGet, 
            adrv903x_Core_RadioControlInterfaceRxAntEnPin1_BfGet, 
            adrv903x_Core_RadioControlInterfaceRxAntEnPin2_BfGet, 
            adrv903x_Core_RadioControlInterfaceRxAntEnPin3_BfGet, 
            adrv903x_Core_RadioControlInterfaceRxAntEnPin4_BfGet, 
            adrv903x_Core_RadioControlInterfaceRxAntEnPin5_BfGet, 
            adrv903x_Core_RadioControlInterfaceRxAntEnPin6_BfGet, 
            adrv903x_Core_RadioControlInterfaceRxAntEnPin7_BfGet
        }
    };

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txRxEnCfg, cleanup);

    for (i = 0; i < ADI_ADRV903X_TRX_CTRL_PIN_COUNT; i++)
    {
        recoveryAction = adrvCoreRadioControlInterfaceTRxEnPinXBfGet[0][i](device,
                                                                           NULL,
                                                                           ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                           &txRxEnCfg->txEnMapping[i]);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API TX Radio Control En Pin Failed.");
            goto cleanup;
        }

        recoveryAction = adrvCoreRadioControlInterfaceTRxEnPinXBfGet[1][i](device,
                                                                           NULL,
                                                                           ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                           &txRxEnCfg->txAltMapping[i]);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API TX Radio Control Alternate En Pin Failed.");
            goto cleanup;
        }

        recoveryAction = adrvCoreRadioControlInterfaceTRxEnPinXBfGet[2][i](device,
                                                                           NULL,
                                                                           ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                           &txRxEnCfg->rxEnMapping[i]);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API RX Radio Control En Pin Failed.");
            goto cleanup;
        }

        recoveryAction = adrvCoreRadioControlInterfaceTRxEnPinXBfGet[3][i](device,
                                                                           NULL,
                                                                           ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                           &txRxEnCfg->rxAltMapping[i]);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API RX Radio Control Alternate En Pin Failed.");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TemperatureGet(adi_adrv903x_Device_t* const        device,
                                                             const uint16_t                      avgMask,
                                                             adi_adrv903x_DevTempData_t* const   deviceTemperature)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_CpuCmd_GetDevTemp_t devTempCmd;
    adrv903x_CpuCmd_GetDevTempResp_t devTempCmdResp;
    adrv903x_CpuCmd_GetDevTempResp_t devTempCmdResp0;
    adrv903x_CpuCmd_GetDevTempResp_t devTempCmdResp1;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e cpuErrorCode = ADRV903X_CPU_SYSTEM_SIMULATED_ERROR;
    const int16_t ADRV903X_TEMP_UNUSED = -274;
    uint32_t cpuTypeIdx = 0U;
    uint32_t curTempSensor = 0U;
    uint16_t avgDiv = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, deviceTemperature, cleanup);

    ADI_LIBRARY_MEMSET(&devTempCmd, 0, sizeof(adrv903x_CpuCmd_GetDevTemp_t));
    ADI_LIBRARY_MEMSET(&devTempCmdResp, 0, sizeof(adrv903x_CpuCmd_GetDevTempResp_t));
    ADI_LIBRARY_MEMSET(&devTempCmdResp0, 0, sizeof(adrv903x_CpuCmd_GetDevTempResp_t));
    ADI_LIBRARY_MEMSET(&devTempCmdResp1, 0, sizeof(adrv903x_CpuCmd_GetDevTempResp_t));

    /* Set ADRV903X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE command parameters */
    devTempCmd.avgMask = ADRV903X_HTOCS(avgMask);

    for (cpuTypeIdx = (uint32_t) ADI_ADRV903X_CPU_TYPE_0; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(   device,
                                                (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                                ADRV903X_LINK_ID_0,
                                                ADRV903X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE,
                                                (void*)&devTempCmd,
                                                sizeof(devTempCmd),
                                                (void*)&devTempCmdResp,
                                                sizeof(devTempCmdResp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(devTempCmdResp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
        else
        {
            if (cpuTypeIdx == (uint32_t) ADI_ADRV903X_CPU_TYPE_0)
            {
                devTempCmdResp0 = devTempCmdResp;
            }
            if (cpuTypeIdx == (uint32_t) ADI_ADRV903X_CPU_TYPE_1)
            {
                devTempCmdResp1 = devTempCmdResp;
            }
        }
    }

    /* Copy/translate the temperature data to the caller's buffer */
    deviceTemperature->avgMask = ADRV903X_CTOHS((uint16_t)(devTempCmdResp0.tempData.avgMask | devTempCmdResp1.tempData.avgMask));
    deviceTemperature->tempDegreesCelsiusAvg = 0;
    for( curTempSensor = 0U; curTempSensor < ADI_ADRV903X_DEVTEMP_MAX_SENSORS; curTempSensor++)
    {
        if ((int16_t)ADRV903X_CTOHS(devTempCmdResp0.tempData.tempDegreesCelsius[curTempSensor]) == ADRV903X_TEMP_UNUSED )
        {
            deviceTemperature->tempDegreesCelsius[curTempSensor] = ADRV903X_CTOHS(devTempCmdResp1.tempData.tempDegreesCelsius[curTempSensor]);
        }
        else
        {
            deviceTemperature->tempDegreesCelsius[curTempSensor] = ADRV903X_CTOHS(devTempCmdResp0.tempData.tempDegreesCelsius[curTempSensor]);
        }

        if ((deviceTemperature->tempDegreesCelsius[curTempSensor] != ADRV903X_TEMP_UNUSED) && 
            (deviceTemperature->avgMask & (0x0001 << curTempSensor)))
        {
            deviceTemperature->tempDegreesCelsiusAvg += deviceTemperature->tempDegreesCelsius[curTempSensor];
            ++avgDiv;
        }
    }

    if ((avgDiv > 0) && (deviceTemperature->avgMask > 0))
    {
        deviceTemperature->tempDegreesCelsiusAvg /= avgDiv;
    }
    else
    {
        deviceTemperature->tempDegreesCelsiusAvg = ADRV903X_TEMP_UNUSED;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TemperatureEnableGet(adi_adrv903x_Device_t* const        device,
                                                                   uint16_t* const                     tempEnData)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_CpuCmd_GetDevTempSnsEnResp_t devTempEnCmdResp;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e cpuErrorCode = ADRV903X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t cpuTypeIdx = 0U;

    /* Check for pointer not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(tempEnData);

    ADI_LIBRARY_MEMSET(&devTempEnCmdResp, 0, sizeof(adrv903x_CpuCmd_GetDevTempSnsEnResp_t));
    *tempEnData = 0;

    for (cpuTypeIdx = (uint32_t) ADI_ADRV903X_CPU_TYPE_0; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response*/
        recoveryAction = adrv903x_CpuCmdSend(   device,
                                                (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                                ADRV903X_LINK_ID_0,
                                                ADRV903X_CPU_CMD_ID_GET_ENABLED_TEMPSENSORS,
                                                (void*) NULL, /* No CMD load needed for this cmd */
                                                0U,
                                                (void*)&devTempEnCmdResp,
                                                sizeof(devTempEnCmdResp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(devTempEnCmdResp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
        else
        {
            /* Copy/translate the temperature data to the caller's buffer */
            *tempEnData |= (ADRV903X_CTOHS(devTempEnCmdResp.tempEnData));
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TemperatureEnableSet(adi_adrv903x_Device_t* const        device,
                                                                   uint16_t* const                     tempEnData)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_CpuCmd_SetDevTempSnsEn_t devTempCmd;
    adrv903x_CpuCmd_SetDevTempSnsEnResp_t devTempEnCmdResp;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e cpuErrorCode = ADRV903X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t cpuTypeIdx = 0U;

    /* Check for pointer not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(tempEnData);
    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_LIBRARY_MEMSET(&devTempEnCmdResp, 0, sizeof(adrv903x_CpuCmd_SetDevTempSnsEnResp_t));
    /* Set ADRV903X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE command parameters */ 
    devTempCmd.tempEnData = ADRV903X_HTOCS(*tempEnData);
    *tempEnData = 0;

    for (cpuTypeIdx = (uint32_t) ADI_ADRV903X_CPU_TYPE_0; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response; cpu0*/
        recoveryAction = adrv903x_CpuCmdSend(   device,
                                                (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                                ADRV903X_LINK_ID_0,
                                                ADRV903X_CPU_CMD_ID_SET_ENABLED_TEMPSENSORS,
                                                (void*)&devTempCmd,
                                                sizeof(devTempCmd), 
                                                (void*)&devTempEnCmdResp,
                                                sizeof(devTempEnCmdResp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(devTempEnCmdResp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
        else
        {
            *tempEnData |= ADRV903X_CTOHS(devTempEnCmdResp.tempEnData);
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_StreamGpioConfigSet(adi_adrv903x_Device_t* const          device,
                                                                  const adi_adrv903x_StreamGpioPinCfg_t* const streamGpioPinCfg)
{  
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    int idx = 0;

    adi_adrv903x_GpioSignal_e signals[ADI_ADRV903X_MAX_STREAMGPIO] = { 
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_0,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_1,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_2,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_3,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_4,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_5,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_6,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_7,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_8,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_9,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_10,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_11,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_12,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_13,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_14,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_15,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_16,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_17,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_18,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_19,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_20,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_21,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_22,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_23 
    };

    adi_adrv903x_GpioPinSel_e pinSelect = ADI_ADRV903X_GPIO_INVALID;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, streamGpioPinCfg, cleanup);

#if ADI_ADRV903X_RADIOCTRL_RANGE_CHECK > 0
    recoveryAction = adrv903x_StreamGpioConfigSetRangeCheck(device,
                                                            streamGpioPinCfg);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "StreamGpioSignalSetRangeCheck issue");
        goto cleanup;
    }
#endif

    /* Release all Stream GPIO if valid */
    for (idx = 0U; idx < ADI_ADRV903X_MAX_STREAMGPIO; idx++)
    {    
        /* Find the signal for this gpio index */
        recoveryAction = adrv903x_GpioSignalFind(device, &pinSelect, signals[idx], ADI_ADRV903X_CHOFF);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   idx,
                                   "GpioSignalFind issue");
            goto cleanup;
        }

        /* Release if pinSelect is valid */
        if (pinSelect != ADI_ADRV903X_GPIO_INVALID)
        {
            recoveryAction = adrv903x_GpioSignalRelease(device, pinSelect, signals[idx], ADI_ADRV903X_CHOFF);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       idx,
                                       "GpioSignalRelease issue");
                goto cleanup;
            }
        }
    }

    /* Config Stream GPIO if valid */
    for (idx = 0U; idx < ADI_ADRV903X_MAX_STREAMGPIO; idx++)
    {
        /* Set if pinSelect is valid */
        if (streamGpioPinCfg->streamGpInput[idx] != ADI_ADRV903X_GPIO_INVALID)
        {    
            pinSelect = (adi_adrv903x_GpioPinSel_e)(idx);

            /* Set the signal for this gpio */
            recoveryAction = adrv903x_GpioSignalSet(device, pinSelect, signals[idx], ADI_ADRV903X_CHOFF);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       idx,
                                       "GpioSignalSet issue");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_StreamGpioConfigGet(adi_adrv903x_Device_t* const         device,
                                                                  adi_adrv903x_StreamGpioPinCfg_t* const streamGpioPinCfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    int idx = 0;
    adi_adrv903x_GpioSignal_e signals[ADI_ADRV903X_MAX_STREAMGPIO] = { 
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_0,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_1,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_2,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_3,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_4,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_5,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_6,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_7,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_8,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_9,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_10,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_11,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_12,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_13,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_14,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_15,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_16,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_17,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_18,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_19,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_20,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_21,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_22,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_23 
    };

    adi_adrv903x_GpioPinSel_e pinSelect = ADI_ADRV903X_GPIO_INVALID;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, streamGpioPinCfg, cleanup);

    for (idx = 0U; idx < ADI_ADRV903X_MAX_STREAMGPIO; idx++)
    {    
        /* Find the signal for this gpio index */
        recoveryAction = adrv903x_GpioSignalFind(device, &pinSelect, signals[idx], ADI_ADRV903X_CHOFF);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "GpioSignalGet issue");
            goto cleanup;
        }

        streamGpioPinCfg->streamGpInput[idx] = pinSelect;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_OrxNcoFreqCalculate(adi_adrv903x_Device_t* const    device,
                                                                  const adi_adrv903x_RxChannels_e orxChannel,
                                                                  const uint32_t                  txSynthesisBwLower_kHz,
                                                                  const uint32_t                  txSynthesisBwUpper_kHz,
                                                                  int32_t* const                  ncoShiftFreqAdc_kHz,
                                                                  int32_t* const                  ncoShiftFreqDatapath_kHz)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    uint32_t orxChannelIdx = 0U;
    uint32_t centreTxSynthesisBw_kHz = (txSynthesisBwLower_kHz + txSynthesisBwUpper_kHz)/2U;
    uint32_t boundaryDistanceLower = 0U;
    uint32_t boundaryDistanceUpper = 0U;
    uint32_t nyquistZone1SynthesisBwUpper = 0U;
    uint32_t nyquistZone1SynthesisBwLower = 0U;
    uint32_t orxAdcSampleRate_kHz = 0U;
    int32_t  passbandEdgeShift = 0;
    int32_t  localNcoShiftFreqAdc_kHz = 0;
    uint8_t  validRangeSynthesisBw = 0U;

    const uint32_t ORX_ADC_RATE_GUARD_REGION_KHZ = 100000U; /* Distance between the RF synthesis BW edges and multiples of FS/2 */
    const int32_t HB_FILTER_PASSBAND_EDGE_BY_100 = 44;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, ncoShiftFreqAdc_kHz, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, ncoShiftFreqDatapath_kHz, cleanup);

    /* Range check orxChannel enum */
    if ((orxChannel != ADI_ADRV903X_ORX0) &&
        (orxChannel != ADI_ADRV903X_ORX1))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, orxChannel, "ORx Channel selected is invalid. Must select ORx0-1.");
        goto cleanup;
    }

    orxChannelIdx = (orxChannel == ADI_ADRV903X_ORX0) ? 0U : 1U;
    orxAdcSampleRate_kHz = device->initExtract.orx.orxChannelCfg[orxChannelIdx].orxAdcSampleRate_kHz;
    /* Pick only the data that lies in the 0 to Fs region and shift that to DC */
    localNcoShiftFreqAdc_kHz = -(int32_t)(centreTxSynthesisBw_kHz % orxAdcSampleRate_kHz);
    /* Recalculate the upper and lower synthesis Bw edges such that it lies in the 0 to Fs region*/
    nyquistZone1SynthesisBwUpper = txSynthesisBwUpper_kHz % orxAdcSampleRate_kHz;
    nyquistZone1SynthesisBwLower = txSynthesisBwLower_kHz % orxAdcSampleRate_kHz;

    /* if the RF synthesis BW lies across an Fs boundary, the values of the upper synthesis BW
     * would end up lower than the lower synthesis BW edge */
    if (nyquistZone1SynthesisBwUpper < nyquistZone1SynthesisBwLower)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txSynthesisBwUpper_kHz,
                               "Invalid relation between Tx synthesis bandwidth upper and lower frequencies in the first Nyquist Zone");
        goto cleanup;
    }

    /* Make sure the Synthesis Bandwidth is at least 100 MHz away from DC and Fs/2 or Fs/2 and Fs */
    /* If the signal lies in a odd nyquist zones (0 to 0.5*Fs) we need a -ve frequency shift at the NCO */
    if ((nyquistZone1SynthesisBwLower >= (ORX_ADC_RATE_GUARD_REGION_KHZ)) &&
        (nyquistZone1SynthesisBwUpper <= ((orxAdcSampleRate_kHz / 2U) - ORX_ADC_RATE_GUARD_REGION_KHZ)))
    {
        validRangeSynthesisBw = ADI_TRUE;
        boundaryDistanceUpper = (orxAdcSampleRate_kHz / 2U) - nyquistZone1SynthesisBwUpper;
        boundaryDistanceLower = nyquistZone1SynthesisBwLower;
    }

    /* If the signal lies in a odd nyquist zones (0.5*Fs to Fs, 1.5*Fs to 2*Fs, 2.5Fs to 3*Fs)
     * we need a +ve frequency shift at the NCO */
    if ((nyquistZone1SynthesisBwLower >= ((orxAdcSampleRate_kHz / 2U) + ORX_ADC_RATE_GUARD_REGION_KHZ)) &&
        (nyquistZone1SynthesisBwUpper <= (orxAdcSampleRate_kHz - ORX_ADC_RATE_GUARD_REGION_KHZ)))
    {
        validRangeSynthesisBw = ADI_TRUE;
        boundaryDistanceUpper = orxAdcSampleRate_kHz - nyquistZone1SynthesisBwUpper;
        boundaryDistanceLower = nyquistZone1SynthesisBwLower - (orxAdcSampleRate_kHz / 2U);
    }

    /* Case when the Synthesis Bandwidth is very close to the Fs or Fs/2 boundary*/
    if (validRangeSynthesisBw == ADI_FALSE)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txSynthesisBwUpper_kHz,
                               "Invalid range for Tx synthesis bandwidth upper and lower frequencies");
        goto cleanup;
    }

    passbandEdgeShift = (int32_t)((HB_FILTER_PASSBAND_EDGE_BY_100 *
                                  device->initExtract.orx.orxChannelCfg[orxChannelIdx].orxOutputRate_kHz) -
                                  ((txSynthesisBwUpper_kHz - centreTxSynthesisBw_kHz) * 100)) / 100;
    /* If passbandEdgeShift is less than 0, the synthesis BW is greater than the 88% bandwidth supported by the ORx*/
    if (passbandEdgeShift < 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txSynthesisBwUpper_kHz,
                               "Tx synthesis bandwidth is greater than the maximum supported by the ORX channel");
        goto cleanup;
    }

    if (boundaryDistanceLower < boundaryDistanceUpper)
    {
        /* If the closest image is on the lower side of the SBW, we shift the
         * observed synthesis BW to the lower edge the passband (- 0.44 * Fs)*/
        *ncoShiftFreqAdc_kHz = localNcoShiftFreqAdc_kHz - passbandEdgeShift;
        *ncoShiftFreqDatapath_kHz = passbandEdgeShift;
    }
    else
    {
        /* If the closest image is on the upper side of the SBW, we shift the
         * observed synthesis BW to the upper edge the passband (+ 0.44 * Fs)*/
        *ncoShiftFreqAdc_kHz = localNcoShiftFreqAdc_kHz + passbandEdgeShift;
        *ncoShiftFreqDatapath_kHz = -passbandEdgeShift;
    }

    /* Success */
    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxToOrxMappingInit(adi_adrv903x_Device_t* const device)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    

        /* Initializes the Tx To Orx Mapping Config, including its GPIOs */
    recoveryAction = adrv903x_TxToOrxMappingInit(device, &device->devStateInfo.txToOrxMappingConfig);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx To Orx Mapping Init Failed");
        goto cleanup;
    }
        cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxToOrxMappingConfigGet(adi_adrv903x_Device_t* const                  device,
                                                                      adi_adrv903x_TxToOrxMappingConfig_t* const    mappingConfig)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common) ;
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, mappingConfig, cleanup);
    
    /* Grab configuration stored in the device handle*/
    ADI_LIBRARY_MEMCPY(mappingConfig, &device->devStateInfo.txToOrxMappingConfig, sizeof(adi_adrv903x_TxToOrxMappingConfig_t));

    /*Success*/
    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
    
cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxToOrxMappingSet(adi_adrv903x_Device_t* const    device,
                                                                const uint8_t                   mapping)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t modeMask = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    
    /* Get modeMask from the mode*/
    switch (device->devStateInfo.txToOrxMappingConfig.mode)
    {
        case ADI_ADRV903X_TX_ORX_MAPPING_MODE_2BIT:
            modeMask = 0x03U;
            break;
        case ADI_ADRV903X_TX_ORX_MAPPING_MODE_3BIT:
            modeMask = 0x07U;
            break;
        case ADI_ADRV903X_TX_ORX_MAPPING_MODE_4BIT:
            modeMask = 0x0FU;
            break;
        case ADI_ADRV903X_TX_ORX_MAPPING_MODE_6BIT:
            modeMask = 0x3FU;
            break;
        case ADI_ADRV903X_TX_ORX_MAPPING_MODE_8BIT: /* Fallthrough */
        default:
            modeMask = 0xFFU;
            break;
    }

    /* Range check mapping byte against Mapping Mode */
    if ((mapping & modeMask) != mapping)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               mapping,
                               "Tx to Orx Mapping byte requested is not valid for use with the device's Mapping Mode configuration");
        goto cleanup;
    }

    /* Write mapping byte to scratch reg for the stream to readback */
    recoveryAction = adrv903x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                    ADRV903X_CPU_TX_ORX_MAPPING_SET_SCRATCH_REG_ID,
                                                    mapping);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while setting Tx to ORx mapping selection.");
        goto cleanup;
    }

    /* Trigger stream */
    recoveryAction = adrv903x_StreamTrigger(device, ADRV903X_STREAM_MAIN_TX_TO_ORX_MAP);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while triggering Core Stream to apply Tx to Orx mapping");
        goto cleanup;
    }
    
cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxToOrxMappingGet(adi_adrv903x_Device_t* const        device,
                                                                const adi_adrv903x_RxChannels_e     orxChannel,
                                                                adi_adrv903x_TxChannels_e* const    txChannel)
{
        static const uint8_t NIBBLE_TX_MASK   = 0x07U;

    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t readNibble = 0U;
    uint8_t txIdx = 0U;
    uint8_t noMapState = 0U;
    uint32_t txChanSel = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common) ;
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txChannel, cleanup);

    /* Range check orxChannel enum */
    if ((orxChannel != ADI_ADRV903X_ORX0) &&
        (orxChannel != ADI_ADRV903X_ORX1))
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               orxChannel,
                               "ORx Channel selected is invalid. Must select ORx0-1.") ;
        goto cleanup;
    }

    /* Readback current mapping */
    if (orxChannel == ADI_ADRV903X_ORX0)
    {
        recoveryAction = adrv903x_Core_RadioControlInterfaceOrx0Map_BfGet(device,
                                                                          NULL,
                                                                          (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                                          &readNibble);
    }
    else
    {
        recoveryAction = adrv903x_Core_RadioControlInterfaceOrx1Map_BfGet(device,
                                                                          NULL,
                                                                          (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                                          &readNibble);
    }

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while retrieving current Tx to ORx map for selected ORx channel.");
        goto cleanup;
    }


    /* Use mapping mode and tx observability to translate value to tx channel*/
    static const uint8_t NIBBLE_MSB = 0x08U;
    noMapState = (NIBBLE_MSB & readNibble) ? ADI_TRUE : ADI_FALSE;
    
    if (noMapState )
    {
        *txChannel = ADI_ADRV903X_TXOFF;
    }
    else
    {
        txIdx = NIBBLE_TX_MASK & readNibble;
        txChanSel = 1U << (uint32_t)txIdx;
        *txChannel = (adi_adrv903x_TxChannels_e)(txChanSel);
    }
    
cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxToOrxMappingPresetAttenSet( adi_adrv903x_Device_t* const    device,
                                                                            const uint32_t                  mapping,
                                                                            const uint8_t                   presetAtten_dB,
                                                                            const uint8_t                   immediateUpdate)
{
        static const uint8_t MAX_ORX_ATTEN_DB = 16U;
    
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t txIdx = 0U;
    uint32_t txChannelMask = (mapping & (~TX_TO_ORX_EXTENDED_MAPPING_FLAG));
    uint32_t extendedMappingFlag = (mapping & TX_TO_ORX_EXTENDED_MAPPING_FLAG);
    uint8_t tmpMask = 0U;
    adi_adrv903x_Channels_e tmpChannel = ADI_ADRV903X_CHOFF;
    uint8_t sendToCpu[ADI_ADRV903X_CPU_TYPE_MAX_RADIO] = { ADI_FALSE, ADI_FALSE };
    uint8_t txChanMaskForCpu[ADI_ADRV903X_CPU_TYPE_MAX_RADIO] = { 0U };
    uint32_t cpuIdx = 0U;
    adi_adrv903x_CpuType_e cpuType = ADI_ADRV903X_CPU_TYPE_UNKNOWN;

    adrv903x_CpuCmd_SetTxToOrxPresetAtten_t cmd;
    adrv903x_CpuCmd_SetTxToOrxPresetAttenResp_t cmdRsp;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e cpuErrorCode = ADRV903X_CPU_SYSTEM_MAILBOX_ERROR;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_LIBRARY_MEMSET(&cmd, 0, sizeof(adrv903x_CpuCmd_SetTxToOrxPresetAtten_t));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv903x_CpuCmd_SetTxToOrxPresetAttenResp_t));

    /* Check that requested mapping are valid */
    if ((txChannelMask  != (txChannelMask & 0x00FFU))       ||  /* Value Not in Range 0x00 to 0xFF */
        (mapping        == TX_TO_ORX_EXTENDED_MAPPING_FLAG) ||  /* Extended Flag & 0x00 Bitmask */
        (txChannelMask  == 0U))                                 /* 0x00 Bitmask */
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                txChannelMask,
                                "Requested Mapping is Invalid");
        goto cleanup;
    }

    /* Check preset value is valid */
    if (presetAtten_dB > MAX_ORX_ATTEN_DB)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                presetAtten_dB,
                                "presetAtten_dB outside maximum range");
        goto cleanup;
    }
    
    /* Check that immediateUpdate is valid */
    if ((immediateUpdate != ADI_TRUE) &&
        (immediateUpdate != ADI_FALSE))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                immediateUpdate,
                                "immediateUpdate is invalid. Must be 0 or 1.");
        goto cleanup;
    }

    /* Check that immediateUpdate is valid for State Mappings */
    if ((immediateUpdate        == ADI_TRUE)                            &&
        (extendedMappingFlag    == TX_TO_ORX_EXTENDED_MAPPING_FLAG))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                immediateUpdate,
                                "Immediate Update Cannot Be Performed with a State Mapping");
        goto cleanup;
    }

    if (extendedMappingFlag == TX_TO_ORX_EXTENDED_MAPPING_FLAG)
    {
        /* State Mapping Specific Command */
        cmd.extendedMappingFlag = ADI_TRUE;
        sendToCpu[0U]           = ADI_TRUE;
        cmd.chanSelect          = txChannelMask;
    }
    else
    {
        for (txIdx = 0U; txIdx < ADI_ADRV903X_MAX_TXCHANNELS; ++txIdx)
        {
            /* Get channel number enum for this txIdx */
            tmpMask = (1U << txIdx);
            tmpMask = (tmpMask & ((uint8_t) txChannelMask));
            tmpChannel = (adi_adrv903x_Channels_e)(tmpMask);

            /* If channel enum is TXOFF, skip this channel */
            if (tmpChannel == ADI_ADRV903X_CHOFF)
            {
                continue;
            }

            /* Get the CPU assigned to this channel*/
            recoveryAction = adrv903x_CpuChannelMappingGet( device, tmpChannel, ADRV903X_CPU_OBJID_SYSTEM_END, &cpuType);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get CPU channel mapping");
                goto cleanup;
            }

            /* Set bit for target CPU */
            cpuIdx = (uint8_t)(cpuType);
            sendToCpu[cpuIdx] = ADI_TRUE;
            txChanMaskForCpu[cpuIdx] |= tmpMask;
        }
    }

    /* Loop through CPUs and send CPU cmd to only those required */
    for (cpuIdx = 0U; cpuIdx < (uint32_t)ADI_ADRV903X_CPU_TYPE_MAX_RADIO; cpuIdx++)
    {
        if (sendToCpu[cpuIdx] == ADI_TRUE)
        {
            /* Set cpuType */
            cpuType = (adi_adrv903x_CpuType_e)cpuIdx;

            /* Setup cmd-specific payload */
            if (extendedMappingFlag != TX_TO_ORX_EXTENDED_MAPPING_FLAG)
            {
                cmd.chanSelect = txChanMaskForCpu[cpuIdx];
            }
            cmd.presetAtten_dB = (uint8_t) presetAtten_dB;
            cmd.immediateUpdate = (uint8_t) immediateUpdate;

            /* Send command and receive response */
            recoveryAction = adrv903x_CpuCmdSend(   device,
                                                    cpuType,
                                                    ADRV903X_LINK_ID_0,
                                                    ADRV903X_CPU_CMD_ID_SET_TX_TO_ORX_PRESET_ATTEN,
                                                    (void*)&cmd,
                                                    sizeof(cmd),
                                                    (void*)&cmdRsp,
                                                    sizeof(cmdRsp),
                                                    &cmdStatus);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxToOrxMappingPresetAttenGet_v2(  adi_adrv903x_Device_t* const                device,
                                                                                const adi_adrv903x_TxToOrxMappingPinTable_e mapping,
                                                                                uint8_t* const                              presetAtten_dB)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t scratchRegId = 0U;
    uint8_t scratchAttenVal = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, presetAtten_dB, cleanup);

    *presetAtten_dB = 0U;

    /* Check that requested Mapping is valid and determine scratchpadRegId for selected Mapping */
    switch (mapping)
    {
        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX0:
            scratchRegId = ADRV903X_CPU_TX_0_ORX_ATTEN;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX1:
            scratchRegId = ADRV903X_CPU_TX_1_ORX_ATTEN;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX2:
            scratchRegId = ADRV903X_CPU_TX_2_ORX_ATTEN;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX3:
            scratchRegId = ADRV903X_CPU_TX_3_ORX_ATTEN;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX4:
            scratchRegId = ADRV903X_CPU_TX_4_ORX_ATTEN;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX5:
            scratchRegId = ADRV903X_CPU_TX_5_ORX_ATTEN;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX6:
            scratchRegId = ADRV903X_CPU_TX_6_ORX_ATTEN;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX7:
            scratchRegId = ADRV903X_CPU_TX_7_ORX_ATTEN;
            break;


        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_0:
            scratchRegId = ADRV903X_CPU_MAPVAL_9_ATTEN;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_1:
            scratchRegId = ADRV903X_CPU_MAPVAL_A_ATTEN;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_2:
            scratchRegId = ADRV903X_CPU_MAPVAL_B_ATTEN;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_3:
            scratchRegId = ADRV903X_CPU_MAPVAL_C_ATTEN;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_4:
            scratchRegId = ADRV903X_CPU_MAPVAL_D_ATTEN;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_5:
            scratchRegId = ADRV903X_CPU_MAPVAL_E_ATTEN;
            break;

        default:
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    mapping,
                                    "Requested Mapping is Invalid - Check adi_adrv903x_TxToOrxMapping_e for Valid Values");
            goto cleanup;
            break;
    }

    /* Readback Scratchpad value */
    recoveryAction = adrv903x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                    scratchRegId,
                                                    &scratchAttenVal);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read Tx to Orx Preset value for Orx Atten");
        goto cleanup;
    }

    /* Convert to dB, same as in OrxAttenGet*/
    recoveryAction = adrv903x_ORxTrmAttenToDb(device, scratchAttenVal, presetAtten_dB);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to resolve Orx Atten value in dB for Tx to Orx Preset");
        goto cleanup;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxToOrxMappingPresetAttenGet( adi_adrv903x_Device_t* const    device,
                                                                            const adi_adrv903x_TxChannels_e txChannel,
                                                                            uint8_t* const                  presetAtten_dB)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t scratchRegId = 0U;
    uint8_t scratchAttenVal = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, presetAtten_dB, cleanup);

    /* Check that requested Tx channel is valid and determine scratchpadRegId for selected channel */
    switch (txChannel)
    {
        case ADI_ADRV903X_TX0:
            scratchRegId = ADRV903X_CPU_TX_0_ORX_ATTEN;
            break;

        case ADI_ADRV903X_TX1:
            scratchRegId = ADRV903X_CPU_TX_1_ORX_ATTEN;
            break;

        case ADI_ADRV903X_TX2:
            scratchRegId = ADRV903X_CPU_TX_2_ORX_ATTEN;
            break;

        case ADI_ADRV903X_TX3:
            scratchRegId = ADRV903X_CPU_TX_3_ORX_ATTEN;
            break;

        case ADI_ADRV903X_TX4:
            scratchRegId = ADRV903X_CPU_TX_4_ORX_ATTEN;
            break;

        case ADI_ADRV903X_TX5:
            scratchRegId = ADRV903X_CPU_TX_5_ORX_ATTEN;
            break;

        case ADI_ADRV903X_TX6:
            scratchRegId = ADRV903X_CPU_TX_6_ORX_ATTEN;
            break;

        case ADI_ADRV903X_TX7:
            scratchRegId = ADRV903X_CPU_TX_7_ORX_ATTEN;
            break;

        default:
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     txChannel,
                                     "Requested Tx channel is invalid. Must be Tx Channel 0-7.");
            goto cleanup;
            break;
    }

    /* Readback Scratchpad value */
    recoveryAction = adrv903x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                    scratchRegId,
                                                    &scratchAttenVal);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read Tx to Orx Preset value for Orx Atten");
        goto cleanup;
    }

    /* Convert to dB, same as in OrxAttenGet*/
    recoveryAction = adrv903x_ORxTrmAttenToDb(device, scratchAttenVal, presetAtten_dB);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to resolve Orx Atten value in dB for Tx to Orx Preset");
        goto cleanup;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxToOrxMappingPresetNcoSet(   adi_adrv903x_Device_t* const                        device,
                                                                            const uint32_t                                      mapping,
                                                                            const adi_adrv903x_TxToOrxMappingPresetNco_t* const presetNco,
                                                                            const uint8_t                                       immediateUpdate)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t txIdx = 0U;
    uint32_t txChannelMask = (mapping & (~TX_TO_ORX_EXTENDED_MAPPING_FLAG));
    uint32_t extendedMappingFlag = (mapping & TX_TO_ORX_EXTENDED_MAPPING_FLAG);
    uint8_t tmpMask = 0U;
    adi_adrv903x_Channels_e tmpChannel = ADI_ADRV903X_CHOFF;
    uint8_t sendToCpu[ADI_ADRV903X_CPU_TYPE_MAX_RADIO] = { ADI_FALSE, ADI_FALSE };
    uint8_t txChanMaskForCpu[ADI_ADRV903X_CPU_TYPE_MAX_RADIO] = { 0U };
    uint32_t cpuIdx = 0U;
    adi_adrv903x_CpuType_e cpuType = ADI_ADRV903X_CPU_TYPE_UNKNOWN;

    adrv903x_CpuCmd_SetTxToOrxPresetNco_t cmd;
    adrv903x_CpuCmd_SetTxToOrxPresetNcoResp_t cmdRsp;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e cpuErrorCode = ADRV903X_CPU_SYSTEM_MAILBOX_ERROR;
    
    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common) ;
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, presetNco, cleanup);

    ADI_LIBRARY_MEMSET(&cmd, 0, sizeof(adrv903x_CpuCmd_SetTxToOrxPresetNco_t));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv903x_CpuCmd_SetTxToOrxPresetNcoResp_t));

    /* Check that requested mapping are valid */
    if ((txChannelMask  != (txChannelMask & 0x00FFU))       ||  /* Value Not in Range 0x00 to 0xFF */
        (mapping        == TX_TO_ORX_EXTENDED_MAPPING_FLAG) ||  /* Extended Flag & 0x00 Bitmask */
        (txChannelMask  == 0U))                                 /* 0x00 Bitmask */
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                txChannelMask,
                                "Requested Mapping is Invalid");
        goto cleanup;
    }

    /* Check preset adc nco freq value is valid */
    if ((presetNco->ncoFreqAdc_Khz > 8388607) ||
        (presetNco->ncoFreqAdc_Khz < -8388608))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                presetNco->ncoFreqAdc_Khz,
                                "presetNco->ncoFreqAdc_Khz outside maximum range. Must be in range -8388608 to 8388607");
        goto cleanup;
    }

    /* Check preset datapath nco freq value is valid */
    if ((presetNco->ncoFreqDatapath_Khz > 8388607) ||
        (presetNco->ncoFreqDatapath_Khz < -8388608))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                presetNco->ncoFreqDatapath_Khz,
                                "presetNco->ncoFreqDatapath_Khz outside maximum range. Must be in range -8388608 to 8388607");
        goto cleanup;
    }

    /* Check that immediateUpdate is valid */
    if ((immediateUpdate != ADI_TRUE) &&
        (immediateUpdate != ADI_FALSE))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                immediateUpdate,
                                "immediateUpdate is invalid. Must be 0 or 1.");
        goto cleanup;
    }

    /* Check that immediateUpdate is valid for State Mappings */
    if ((immediateUpdate        == ADI_TRUE)                            &&
        (extendedMappingFlag    == TX_TO_ORX_EXTENDED_MAPPING_FLAG))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                immediateUpdate,
                                "Immediate Update Cannot Be Performed with a State Mapping");
        goto cleanup;
    }

    if (extendedMappingFlag == TX_TO_ORX_EXTENDED_MAPPING_FLAG)
    {
        /* State Mapping Specific Command */
        cmd.extendedMappingFlag = ADI_TRUE;
        sendToCpu[0U]           = ADI_TRUE;
        cmd.chanSelect          = txChannelMask;
    }
    else
    {
        for (txIdx = 0U; txIdx < ADI_ADRV903X_MAX_TXCHANNELS; ++txIdx)
        {
            /* Get channel number enum for this txIdx */
            tmpMask = (1U << txIdx);
            tmpMask &= txChannelMask;
            tmpChannel = (adi_adrv903x_Channels_e)(tmpMask);

            /* If channel enum is TXOFF, skip this channel */
            if (tmpChannel == ADI_ADRV903X_CHOFF)
            {
                continue;
            }

            /* Get the CPU assigned to this channel */
            recoveryAction = adrv903x_CpuChannelMappingGet(device, tmpChannel, ADRV903X_CPU_OBJID_SYSTEM_END, &cpuType);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get CPU channel mapping");
                goto cleanup;
            }

            /* Set bit for target CPU */
            cpuIdx = (uint8_t)(cpuType);
            sendToCpu[cpuIdx] = ADI_TRUE;
            txChanMaskForCpu[cpuIdx] |= tmpMask;
        }
    }

    /* Loop through CPUs and send ARM cmd to only those required */
    for (cpuIdx = 0U; cpuIdx < (uint32_t)ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuIdx)
    {
        if (sendToCpu[cpuIdx] == ADI_TRUE)
        {
            /* Set cpuType */
            cpuType = (adi_adrv903x_CpuType_e)cpuIdx;

            /* Setup cmd-specific payload */
            if (extendedMappingFlag != TX_TO_ORX_EXTENDED_MAPPING_FLAG)
            {
                cmd.chanSelect = txChanMaskForCpu[cpuIdx];
            }
            cmd.ncoFreqAdc_Khz = ADRV903X_HTOCL(presetNco->ncoFreqAdc_Khz);
            cmd.ncoFreqDatapath_Khz = ADRV903X_HTOCL(presetNco->ncoFreqDatapath_Khz);
            cmd.immediateUpdate = immediateUpdate;

            /* Send command and receive response */
            recoveryAction = adrv903x_CpuCmdSend(   device,
                                                    cpuType,
                                                    ADRV903X_LINK_ID_0,
                                                    ADRV903X_CPU_CMD_ID_SET_TX_TO_ORX_PRESET_NCO,
                                                    (void*)&cmd,
                                                    sizeof(cmd),
                                                    (void*)&cmdRsp,
                                                    sizeof(cmdRsp),
                                                    &cmdStatus);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxToOrxMappingPresetNcoGet_v2(adi_adrv903x_Device_t* const                    device,
                                                                            const adi_adrv903x_TxToOrxMappingPinTable_e     mapping,
                                                                            adi_adrv903x_TxToOrxMappingPresetNco_t* const   presetNco)
{
        const uint8_t NCO_NUM_BYTES = 3U;
    
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t scratchRegIdStart_adc = 0U;
    uint8_t scratchRegIdStart_dp = 0U;
    uint8_t tmpByte = 0U;
    uint32_t tmpAdc = 0U;
    uint32_t tmpDp = 0U;
    uint8_t idx = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, presetNco, cleanup);

    ADI_LIBRARY_MEMSET(presetNco, 0, sizeof(adi_adrv903x_TxToOrxMappingPresetNco_t));

    /* Check that requested Tx channel is valid and determine scratchpadRegIdStart for selected channel */
    switch (mapping)
    {
        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX0:
            scratchRegIdStart_adc = ADRV903X_CPU_TX_0_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_TX_0_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX1:
            scratchRegIdStart_adc = ADRV903X_CPU_TX_1_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_TX_1_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX2:
            scratchRegIdStart_adc = ADRV903X_CPU_TX_2_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_TX_2_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX3:
            scratchRegIdStart_adc = ADRV903X_CPU_TX_3_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_TX_3_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX4:
            scratchRegIdStart_adc = ADRV903X_CPU_TX_4_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_TX_4_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX5:
            scratchRegIdStart_adc = ADRV903X_CPU_TX_5_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_TX_5_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX6:
            scratchRegIdStart_adc = ADRV903X_CPU_TX_6_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_TX_6_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX7:
            scratchRegIdStart_adc = ADRV903X_CPU_TX_7_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_TX_7_ORX_NCO_DP_FREQ_MSB;
            break;


        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_0:
            scratchRegIdStart_adc = ADRV903X_CPU_MAPVAL_9_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_MAPVAL_9_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_1:
            scratchRegIdStart_adc = ADRV903X_CPU_MAPVAL_A_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_MAPVAL_A_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_2:
            scratchRegIdStart_adc = ADRV903X_CPU_MAPVAL_B_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_MAPVAL_B_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_3:
            scratchRegIdStart_adc = ADRV903X_CPU_MAPVAL_C_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_MAPVAL_C_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_4:
            scratchRegIdStart_adc = ADRV903X_CPU_MAPVAL_D_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_MAPVAL_D_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_5:
            scratchRegIdStart_adc = ADRV903X_CPU_MAPVAL_E_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_MAPVAL_E_NCO_DP_FREQ_MSB;
            break;

        default:
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    mapping,
                                    "Requested Mapping is Invalid - Check adi_adrv903x_TxToOrxMappingPinTable_e for Valid Values");
            goto cleanup;
            break;
    }

    /* Readback Scratchpad values */
    for (idx = 0U; idx < NCO_NUM_BYTES; ++idx)
    {
        recoveryAction = adrv903x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                        (scratchRegIdStart_adc - idx),
                                                        &tmpByte);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read Tx to Orx Preset value for Orx ADC NCO");
            goto cleanup;
        }
        tmpAdc <<= 8U;
        tmpAdc |= ((uint32_t)tmpByte);

        recoveryAction = adrv903x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                        (scratchRegIdStart_dp - idx),
                                                        &tmpByte);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read Tx to Orx Preset value for Orx Datapath NCO");
            goto cleanup;
        }
        tmpDp <<= 8U;
        tmpDp |= ((uint32_t)tmpByte);
    }

    /* Readback values are 24bits. Returned results are int32_t, so sign extend data as needed */
    if ((tmpAdc & 0x00800000U) != 0U)
    {
        tmpAdc |= 0xFF000000U;
    }
    
    if ((tmpDp & 0x00800000U) != 0U)
    {
        tmpDp |= 0xFF000000U;
    }

    /* Store results */
    presetNco->ncoFreqAdc_Khz = (int32_t)tmpAdc;
    presetNco->ncoFreqDatapath_Khz = (int32_t)tmpDp;

    cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxToOrxMappingPresetNcoGet(   adi_adrv903x_Device_t* const                    device,
                                                                            const adi_adrv903x_TxChannels_e                 txChannel,
                                                                            adi_adrv903x_TxToOrxMappingPresetNco_t* const   presetNco)
{
        const uint8_t NCO_NUM_BYTES = 3U;
    
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t scratchRegIdStart_adc = 0U;
    uint8_t scratchRegIdStart_dp = 0U;
    uint8_t tmpByte = 0U;
    uint32_t tmpAdc = 0U;
    uint32_t tmpDp = 0U;
    uint8_t idx = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, presetNco, cleanup);

    ADI_LIBRARY_MEMSET(presetNco, 0, sizeof(adi_adrv903x_TxToOrxMappingPresetNco_t));

    /* Check that requested Tx channel is valid and determine scratchpadRegIdStart for selected channel */
    switch (txChannel)
    {
        case ADI_ADRV903X_TX0:
            scratchRegIdStart_adc = ADRV903X_CPU_TX_0_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_TX_0_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX1:
            scratchRegIdStart_adc = ADRV903X_CPU_TX_1_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_TX_1_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX2:
            scratchRegIdStart_adc = ADRV903X_CPU_TX_2_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_TX_2_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX3:
            scratchRegIdStart_adc = ADRV903X_CPU_TX_3_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_TX_3_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX4:
            scratchRegIdStart_adc = ADRV903X_CPU_TX_4_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_TX_4_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX5:
            scratchRegIdStart_adc = ADRV903X_CPU_TX_5_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_TX_5_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX6:
            scratchRegIdStart_adc = ADRV903X_CPU_TX_6_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_TX_6_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV903X_TX7:
            scratchRegIdStart_adc = ADRV903X_CPU_TX_7_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV903X_CPU_TX_7_ORX_NCO_DP_FREQ_MSB;
            break;

        default:
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     txChannel,
                                     "Requested Tx channel is invalid. Must be Tx Channel 0-7.");
            goto cleanup;
            break;
    }

    /* Readback Scratchpad values */
    for (idx = 0U; idx < NCO_NUM_BYTES; idx++)
    {
        recoveryAction = adrv903x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                        (scratchRegIdStart_adc - idx),
                                                        &tmpByte);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read Tx to Orx Preset value for Orx ADC NCO");
            goto cleanup;
        }
        tmpAdc <<= 8U;
        tmpAdc |= ((uint32_t) tmpByte);

        recoveryAction = adrv903x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                        (scratchRegIdStart_dp - idx),
                                                        &tmpByte);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read Tx to Orx Preset value for Orx Datapath NCO");
            goto cleanup;
        }
        tmpDp <<= 8U;
        tmpDp |= ((uint32_t) tmpByte);
    }

    /* Readback values are 24bits. Returned results are int32_t, so sign extend data as needed */
    if ((tmpAdc & 0x00800000U) != 0U)
    {
        tmpAdc |= 0xFF000000U;
    }
    
    if ((tmpDp & 0x00800000U) != 0U)
    {
        tmpDp |= 0xFF000000U;
    }

    /* Store results */
    presetNco->ncoFreqAdc_Khz = (int32_t)tmpAdc;
    presetNco->ncoFreqDatapath_Khz = (int32_t)tmpDp;

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_StreamVersionGet(adi_adrv903x_Device_t* const        device,
                                                               adi_adrv903x_Version_t* const      streamVersion)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t ver[4U] = { 0U };
    
    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, streamVersion, cleanup);

    if ((device->devStateInfo.devState & ADI_ADRV903X_STATE_STREAMLOADED) != ADI_ADRV903X_STATE_STREAMLOADED)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Stream binary must be loaded before getting stream version");
        goto cleanup;
    }

    /* Read the StreamVersion */
    recoveryAction = adi_adrv903x_Registers32Read(device, NULL, ADI_ADRV903X_STREAM_VERSION_ADDR, ver, NULL, 4U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading Stream Version registers");
        goto cleanup;
    }
    /* Store the StreamVersion */
    streamVersion->majorVer       = ver[0U];
    streamVersion->minorVer       = ver[1U];
    streamVersion->maintenanceVer = ver[2U];
    streamVersion->buildVer       = ver[3U];

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RadioCtrlAntCalConfigSet(adi_adrv903x_Device_t* const device,
                                                                       adi_adrv903x_RxRadioCtrlAntennaCalConfig_t * const configRx,
                                                                       adi_adrv903x_TxRadioCtrlAntennaCalConfig_t * const configTx)
{

        static const uint32_t txScratchPad2[] = { (uint32_t)ADRV903X_ADDR_TX0_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_TX1_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_TX2_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_TX3_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_TX4_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_TX5_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_TX6_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_TX7_STREAM_SCRATCH2 };

    static const uint32_t rxScratchPad2[] = { (uint32_t)ADRV903X_ADDR_RX0_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_RX1_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_RX2_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_RX3_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_RX4_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_RX5_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_RX6_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_RX7_STREAM_SCRATCH2 };

    static const uint32_t TX_SCRATCH2_WRITE_MASK  = 0x00FFFFFFU;
    static const uint32_t RX_SCRATCH_WRITE_MASK  = 0xFFFFFFFFU;
    static const uint32_t RX_SCRATCH_WRITE_SHIFT = 24U;

    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t txScratch2WriteValue = 0U;
    uint32_t rxScratchWriteValue = 0U;
    adi_adrv903x_RxGain_t rxGain;
    uint16_t attenRegVal = 0U;
    uint32_t chanId = 0U;
    uint32_t chanSel = 0U;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, configRx, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, configTx, cleanup);

    ADI_LIBRARY_MEMSET(&rxGain, 0, sizeof(rxGain));

    /*Check that if requested Rx Channel valid*/
    if ((configRx->rxChannelMask & (~(uint32_t)ADI_ADRV903X_RX_MASK_ALL)) != 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configRx->rxChannelMask,
                               "Invalid channel is selected for Rx config. Valid values are 0 or any combinations of Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    if ((configTx->txChannelMask & (~(uint32_t)ADI_ADRV903X_TXALL)) != 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configTx->txChannelMask,
                               "Invalid channel is selected for Tx config. Valid values are 0 or any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    txScratch2WriteValue = (uint32_t)(configTx->txNcoFreqKhz) & TX_SCRATCH2_WRITE_MASK;
    /* Change the sign of the frequency because there is an I/Q swap in the HW */
    rxScratchWriteValue = (((uint32_t)configRx->rxGainIndex & 0x000000FF) << RX_SCRATCH_WRITE_SHIFT) |
                           ((uint32_t)(-configRx->rxNcoFreqKhz) & 0x00FFFFFF);

    /* for each Tx channel in txChannelMask */
    for (chanId = 0U; chanId < ADI_ADRV903X_MAX_TXCHANNELS; chanId++)
    {
        if ((configTx->txChannelMask & (1U << chanId)) == 0U)
        {
            /* Skip channel. It's not in chanMask. */
            continue;
        }

        if (configTx->txAttenuation_mdB > ADRV903X_TX_ATTEN_VALUE_MILLI_DB_MAX)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                   recoveryAction,
                                   configTx->txAttenuation_mdB,
                                   "Invalid txAttenuation_mdB value");
            goto cleanup;
        }

        /* ChanId is selected in chanMask */
        chanSel = (1U << chanId);
        recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)(chanSel), &txBaseAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }

        /* Conversion from the requested atten level (milli-dB) to equivalent
         * TxAttenTable index is always done based on a step size of 0.05.
         * Other step sizes are not supported. */
        attenRegVal = (configTx->txAttenuation_mdB / ADRV903X_TX_ATTEN_STEP_SIZE_DIV_0P05);

        recoveryAction = adrv903x_TxFuncs_TxAttenuation_BfSet(device,
                                                              NULL,
                                                              txBaseAddr,
                                                              attenRegVal);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Input Select");
            goto cleanup;
        }

        recoveryAction = adi_adrv903x_Register32Write(device, NULL, txScratchPad2[chanId], txScratch2WriteValue, TX_SCRATCH2_WRITE_MASK);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write Tx scratch pad register 2");
            goto cleanup;
        }
    }

    /* for each Rx channel in rxChannelMask */
    for (chanId = 0U; chanId < ADI_ADRV903X_MAX_RX_ONLY; chanId++)
    {
        if ((configRx->rxChannelMask & (1U << chanId)) == 0)
        {
            /* Skip channel. It's not in chanMask. */
            continue;
        }

        rxGain.gainIndex = configRx->rxGainIndex;
        rxGain.rxChannelMask = configRx->rxChannelMask;

        recoveryAction = adrv903x_RxGainSetRangeCheck(device, &rxGain, 1);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Rx gain is out of range");
            goto cleanup;
        }

        /* ChanId is selected in chanMask */
        recoveryAction = adi_adrv903x_Register32Write(device, NULL, rxScratchPad2[chanId], rxScratchWriteValue, RX_SCRATCH_WRITE_MASK);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write Rx scratch pad register 2");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RadioCtrlAntCalConfigGet(adi_adrv903x_Device_t* const device,
                                                                       adi_adrv903x_RxRadioCtrlAntennaCalConfig_t * const configRx,
                                                                       adi_adrv903x_TxRadioCtrlAntennaCalConfig_t * const configTx)
{

        static const uint32_t txScratchPad2[] = {
        (uint32_t)ADRV903X_ADDR_TX0_STREAM_SCRATCH2,
        (uint32_t)ADRV903X_ADDR_TX1_STREAM_SCRATCH2,
        (uint32_t)ADRV903X_ADDR_TX2_STREAM_SCRATCH2,
        (uint32_t)ADRV903X_ADDR_TX3_STREAM_SCRATCH2,
        (uint32_t)ADRV903X_ADDR_TX4_STREAM_SCRATCH2,
        (uint32_t)ADRV903X_ADDR_TX5_STREAM_SCRATCH2,
        (uint32_t)ADRV903X_ADDR_TX6_STREAM_SCRATCH2,
        (uint32_t)ADRV903X_ADDR_TX7_STREAM_SCRATCH2
    };

    static const uint32_t rxScratchPad2[] = {
        (uint32_t)ADRV903X_ADDR_RX0_STREAM_SCRATCH2,
        (uint32_t)ADRV903X_ADDR_RX1_STREAM_SCRATCH2,
        (uint32_t)ADRV903X_ADDR_RX2_STREAM_SCRATCH2,
        (uint32_t)ADRV903X_ADDR_RX3_STREAM_SCRATCH2,
        (uint32_t)ADRV903X_ADDR_RX4_STREAM_SCRATCH2,
        (uint32_t)ADRV903X_ADDR_RX5_STREAM_SCRATCH2,
        (uint32_t)ADRV903X_ADDR_RX6_STREAM_SCRATCH2,
        (uint32_t)ADRV903X_ADDR_RX7_STREAM_SCRATCH2
    };

    static const uint32_t TX_SCRATCH2_READ_MASK  = 0x00FFFFFFU;
    static const uint32_t RX_SCRATCH2_READ_MASK  = 0xFFFFFFFFU;
    static const uint32_t RX_SCRATCH2_READ_SHIFT = 24U;
    static const uint32_t NCO_SIGN_MASK = 0x00800000U;
    
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t txScratch2ReadValue = 0U;
    uint32_t rxScratch2ReadValue = 0U;
    uint16_t attenRegVal = 0U;
    uint32_t txChanId = 0U;
    uint32_t rxChanId = 0U;
    uint32_t chanSel = 0U;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    
    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, configRx, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, configTx, cleanup);

    if ((configRx->rxChannelMask != (uint32_t)ADI_ADRV903X_RXOFF) &&
        (configRx->rxChannelMask != (uint32_t)ADI_ADRV903X_RX0) &&
        (configRx->rxChannelMask != (uint32_t)ADI_ADRV903X_RX1) &&
        (configRx->rxChannelMask != (uint32_t)ADI_ADRV903X_RX2) &&
        (configRx->rxChannelMask != (uint32_t)ADI_ADRV903X_RX3) &&
        (configRx->rxChannelMask != (uint32_t)ADI_ADRV903X_RX4) &&
        (configRx->rxChannelMask != (uint32_t)ADI_ADRV903X_RX5) &&
        (configRx->rxChannelMask != (uint32_t)ADI_ADRV903X_RX6) &&
        (configRx->rxChannelMask != (uint32_t)ADI_ADRV903X_RX7))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   configRx->rxChannelMask,
                                   "Invalid Rx channel is selected. User should select one channel or CHOFF only");
            goto cleanup;
        }
    
    if ((configTx->txChannelMask != (uint32_t)ADI_ADRV903X_TXOFF) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV903X_TX0) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV903X_TX1) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV903X_TX2) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV903X_TX3) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV903X_TX4) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV903X_TX5) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV903X_TX6) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV903X_TX7))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configTx->txChannelMask,
                               "Invalid Tx channel is selected. User should select one channel or CHOFF only");
        goto cleanup;
    }

    if (configTx->txChannelMask == (uint32_t)ADI_ADRV903X_TXOFF) 
    {
        configTx->txAttenuation_mdB = 0U;
        configTx->txNcoFreqKhz = 0;
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
    }
    else
    {
        chanSel = configTx->txChannelMask;
        txChanId = adrv903x_TxChannelsToId((adi_adrv903x_TxChannels_e)chanSel);
        
        recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)(chanSel), &txBaseAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanSel, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }
        
        recoveryAction = adrv903x_TxFuncs_TxAttenuation_BfGet(device,
                                                              NULL,
                                                              txBaseAddr,
                                                              &attenRegVal);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx slice attenuation");
            goto cleanup;
        }
        
        recoveryAction = adi_adrv903x_Register32Read(device, NULL, txScratchPad2[txChanId], &txScratch2ReadValue, TX_SCRATCH2_READ_MASK);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot read Tx scratch pad register 2");
            goto cleanup;
        }

        if ((txScratch2ReadValue & NCO_SIGN_MASK) > 0)
        {
            /* handle the negative case */
            configTx->txNcoFreqKhz = (int32_t)((~txScratch2ReadValue & 0x00FFFFFF) + 1) * (-1);
        }
        else
        {
            /* handle the positive case */
            configTx->txNcoFreqKhz = (int32_t)(txScratch2ReadValue & 0x00FFFFFF);
        }

        /* Conversion from the atten level (milli-dB) to equivalent
         * TxAttenTable index is always done based on a step size of 0.05.
         * Other step sizes are not supported. */
        configTx->txAttenuation_mdB = (attenRegVal * ADRV903X_TX_ATTEN_STEP_SIZE_DIV_0P05);
    }
    
    if (configRx->rxChannelMask == (uint32_t)ADI_ADRV903X_RXOFF)
    {
        configRx->rxGainIndex = 0U;
        configRx->rxNcoFreqKhz = 0;
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
    }
    else
    {
        chanSel = configRx->rxChannelMask;
        rxChanId = adrv903x_RxChannelsToId((adi_adrv903x_RxChannels_e)chanSel);
    
        recoveryAction = adi_adrv903x_Register32Read(device, NULL, rxScratchPad2[rxChanId], &rxScratch2ReadValue, RX_SCRATCH2_READ_MASK);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot read Rx scratch pad register 2");
            goto cleanup;
        }

        if ((rxScratch2ReadValue & NCO_SIGN_MASK) > 0)
        {
            /* handle the negative case */
            configRx->rxNcoFreqKhz = (int32_t)((~rxScratch2ReadValue & 0x00FFFFFF) + 1) * (-1);
        }
        else
        {
            /* handle the positive case */
            configRx->rxNcoFreqKhz = (int32_t)(rxScratch2ReadValue & 0x00FFFFFF);
        }
        configRx->rxNcoFreqKhz *= (-1); /* Change the sign of the frequency because there is an I/Q swap in the HW */
        
        configRx->rxGainIndex  = (rxScratch2ReadValue >> RX_SCRATCH2_READ_SHIFT);
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RadioCtrlAntCalErrorGet(adi_adrv903x_Device_t* const device,
                                                                       const uint32_t channelSel,
                                                                       uint8_t * const errStatus)
{
        static const uint32_t txScratchPad1[] = {
        (uint32_t)ADRV903X_ADDR_TX0_STREAM_SCRATCH1,
        (uint32_t)ADRV903X_ADDR_TX1_STREAM_SCRATCH1,
        (uint32_t)ADRV903X_ADDR_TX2_STREAM_SCRATCH1,
        (uint32_t)ADRV903X_ADDR_TX3_STREAM_SCRATCH1,
        (uint32_t)ADRV903X_ADDR_TX4_STREAM_SCRATCH1,
        (uint32_t)ADRV903X_ADDR_TX5_STREAM_SCRATCH1,
        (uint32_t)ADRV903X_ADDR_TX6_STREAM_SCRATCH1,
        (uint32_t)ADRV903X_ADDR_TX7_STREAM_SCRATCH1
    };

    static const uint32_t rxScratchPad0[] = {
        (uint32_t)ADRV903X_ADDR_RX0_STREAM_SCRATCH0,
        (uint32_t)ADRV903X_ADDR_RX1_STREAM_SCRATCH0,
        (uint32_t)ADRV903X_ADDR_RX2_STREAM_SCRATCH0,
        (uint32_t)ADRV903X_ADDR_RX3_STREAM_SCRATCH0,
        (uint32_t)ADRV903X_ADDR_RX4_STREAM_SCRATCH0,
        (uint32_t)ADRV903X_ADDR_RX5_STREAM_SCRATCH0,
        (uint32_t)ADRV903X_ADDR_RX6_STREAM_SCRATCH0,
        (uint32_t)ADRV903X_ADDR_RX7_STREAM_SCRATCH0
    };

    static const uint32_t TX_ANT_CAL_ERROR_READ_MASK  = 0x000000D8U;
    static const uint32_t RX_ANT_CAL_ERROR_READ_MASK  = 0x00000CC0U;

    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t txScratch1ReadValue = 0U;
    uint32_t rxScratch0ReadValue = 0U;
    uint32_t chanId = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, errStatus, cleanup);

    if ((channelSel != (uint32_t)ADI_ADRV903X_CH0) &&
        (channelSel != (uint32_t)ADI_ADRV903X_CH1) &&
        (channelSel != (uint32_t)ADI_ADRV903X_CH2) &&
        (channelSel != (uint32_t)ADI_ADRV903X_CH3) &&
        (channelSel != (uint32_t)ADI_ADRV903X_CH4) &&
        (channelSel != (uint32_t)ADI_ADRV903X_CH5) &&
        (channelSel != (uint32_t)ADI_ADRV903X_CH6) &&
        (channelSel != (uint32_t)ADI_ADRV903X_CH7))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               channelSel,
                               "Invalid channel is selected. User should select one channel only");
        goto cleanup;
            
    }

    chanId = adrv903x_TxChannelsToId((adi_adrv903x_TxChannels_e)channelSel);
    
    recoveryAction = adi_adrv903x_Register32Read(device, NULL, txScratchPad1[chanId], &txScratch1ReadValue, TX_ANT_CAL_ERROR_READ_MASK);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot read Tx scratch pad register 1");
        goto cleanup;
    }

    recoveryAction = adi_adrv903x_Register32Read(device, NULL, rxScratchPad0[chanId], &rxScratch0ReadValue, RX_ANT_CAL_ERROR_READ_MASK);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot read Rx scratch pad register 0");
        goto cleanup;
    }

    /* In scratch register, Bit 3-4 has Tx Low High stream errors - Bit 6-7 has Tx Ant Cal Low High stream errors */
    txScratch1ReadValue = ((txScratch1ReadValue & 0xC0U) >> 6U) | ((txScratch1ReadValue & 0x18U) >> 1U);

    /* In scratch register, Bit 10-11 has Rx Low High stream errors - Bit 6-7 has Tx Ant Cal Low High stream errors */
    rxScratch0ReadValue = ((rxScratch0ReadValue & 0xC0U) >> 6U) | ((rxScratch0ReadValue & 0xC00U) >> 8U);

    /* Store Tx error bits in LSB and Rx error bits in MSB */
    *errStatus = (txScratch1ReadValue) | (rxScratch0ReadValue << 4U);

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RadioCtrlAntCalErrorClear(adi_adrv903x_Device_t* const device,
                                                                        const uint32_t channelMask,
                                                                        const uint8_t errClearMask)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    int implemented = 0;
    (void)channelMask;
    (void)errClearMask;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

        if (implemented == 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_FEATURE;
        ADI_API_NOT_IMPLEMENTED_REPORT_GOTO(&device->common, cleanup);
    }
cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_StreamProcErrorGet(   adi_adrv903x_Device_t* const            device,
                                                                    adi_adrv903x_StreamErrArray_t* const    streamErr)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t isErrInStream = 0U;
    uint16_t streamErrorCode = 0U; 
    uint8_t i = 0U;

    /*struct to aid separate processor type and id for each stream processor*/
    typedef struct adrv903x_streamProcAttribs
    {
        adrv903x_BfStreamprocChanAddr_e chanAddr;
        adi_adrv903x_StreamProcType_e   procType;
        uint8_t                         procId;
    } adrv903x_streamProcAttribs_t;

    /*Creates the NUM_STREAM_PROCS size array for the stream processors. 
        *Each element is a struct that has the address, the type and the id of the stream processor*/
    static const adrv903x_streamProcAttribs_t streamProcAttribs[ADI_ADRV903X_STREAM_MAX] = {
        {ADRV903X_BF_DIGITAL_CORE_MAIN_STREAM_PROC,     ADI_ADRV903X_STREAM_MAIN, 0},
        {ADRV903X_BF_DIGITAL_CORE_KFA_STREAM_PROC_REGS, ADI_ADRV903X_STREAM_KFA, 0},
        {ADRV903X_BF_SLICE_RX_0__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_RX, 0},
        {ADRV903X_BF_SLICE_RX_1__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_RX, 1},
        {ADRV903X_BF_SLICE_RX_2__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_RX, 2},
        {ADRV903X_BF_SLICE_RX_3__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_RX, 3},
        {ADRV903X_BF_SLICE_RX_4__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_RX, 4},
        {ADRV903X_BF_SLICE_RX_5__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_RX, 5},
        {ADRV903X_BF_SLICE_RX_6__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_RX, 6},
        {ADRV903X_BF_SLICE_RX_7__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_RX, 7},
        {ADRV903X_BF_SLICE_TX_0__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_TX, 0},
        {ADRV903X_BF_SLICE_TX_1__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_TX, 1},
        {ADRV903X_BF_SLICE_TX_2__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_TX, 2},
        {ADRV903X_BF_SLICE_TX_3__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_TX, 3},
        {ADRV903X_BF_SLICE_TX_4__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_TX, 4},
        {ADRV903X_BF_SLICE_TX_5__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_TX, 5},
        {ADRV903X_BF_SLICE_TX_6__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_TX, 6},
        {ADRV903X_BF_SLICE_TX_7__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_TX, 7},
        {ADRV903X_BF_SLICE_ORX_0__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_ORX, 0},
        {ADRV903X_BF_SLICE_ORX_1__SLICE_AHB_STREAM_PROC, ADI_ADRV903X_STREAM_ORX, 1}
    };

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, streamErr, cleanup);

    ADI_LIBRARY_MEMSET(streamErr, 0, sizeof(adi_adrv903x_StreamErrArray_t));

    for (i = 0U; i < ADI_ADRV903X_STREAM_MAX; i++)
    {
        /*Initializes the streamErr with the streamProcType and streamProcId for each stream processor*/
        streamErr->streamProcArray[i].streamProcType = streamProcAttribs[i].procType;
        streamErr->streamProcArray[i].streamProcId = streamProcAttribs[i].procId;

        recoveryAction = adrv903x_Streamproc_DbgRdbkMode_BfSet(device, NULL, streamProcAttribs[i].chanAddr, 1);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue setting Stream Error to volatile status");
            goto cleanup;
        }
        
        recoveryAction = adrv903x_Streamproc_StreamError_BfGet(device, NULL, streamProcAttribs[i].chanAddr, &isErrInStream);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue reading status error bit");
            goto cleanup;
        }
        if (isErrInStream == ADI_TRUE)
        {
            /* Since an error was detected, the stream number that triggered the error and the error 
                * code are saved in the stream processor struct. After storing these informations, the error
                * status is reset*/
            recoveryAction = adrv903x_Streamproc_ErroredStreamNumber_BfGet( device, 
                                                                            NULL, 
                                                                            streamProcAttribs[i].chanAddr,
                                                                            &streamErr->streamProcArray[i].erroredStreamNumber);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue reading the stream number that triggered the error");
                goto cleanup;
            }
            recoveryAction = adrv903x_Streamproc_RdbkErrorVal_BfGet(device,
                                                                    NULL,
                                                                    streamProcAttribs[i].chanAddr,
                                                                    &streamErrorCode);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue reading the error code for the stream error");
                goto cleanup;
            }
            streamErr->streamProcArray[i].streamErrEnum = (adi_adrv903x_StreamError_e)streamErrorCode;

            /*Clear the error in streams:*/
            recoveryAction = adrv903x_Streamproc_ErroredStreamNumber_BfSet( device, 
                                                                            NULL, 
                                                                            streamProcAttribs[i].chanAddr, 
                                                                            0xFU); /*is cleared when writing a dummy value*/ 
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue resetting the stream error number");
                goto cleanup;
            }
            recoveryAction = adrv903x_Streamproc_StreamError_BfSet( device,
                                                                    NULL,
                                                                    streamProcAttribs[i].chanAddr,
                                                                    1U); /*Writing 1 clears error bit*/ 
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue resetting the status error bit");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RadioCtrlAntCalGpioChannelSet(adi_adrv903x_Device_t* const device,
                                                                            const uint32_t txChannelMask,
                                                                            const uint32_t rxChannelMask) 
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    if ((txChannelMask & (~(uint32_t)ADI_ADRV903X_TXALL)) != 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txChannelMask,
                               "Invalid Tx channel is selected. Valid values are 0 or any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }
    
    if ((rxChannelMask & (~(uint32_t)ADI_ADRV903X_RX_MASK_ALL)) != 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannelMask,
                               "Invalid Rx channel is selected. Valid values are 0 or any combinations of Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    /* Write Tx Scratchpad value */
    recoveryAction = adrv903x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                    ADRV903X_GPIO_ANTCAL_TX_MASK,
                                                    (uint8_t)txChannelMask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write GPIO antenna cal Tx selection to scratchpad register");
        goto cleanup;
    }

    /* Write Rx Scratchpad value */
    recoveryAction = adrv903x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                    ADRV903X_GPIO_ANTCAL_RX_MASK,
                                                    (uint8_t)rxChannelMask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write GPIO antenna cal Rx selection to scratchpad register");
        goto cleanup;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RadioCtrlAntCalGpioChannelGet(adi_adrv903x_Device_t* const device,
                                                                            uint32_t * const txChannelMask,
                                                                            uint32_t * const rxChannelMask)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t scratchPadValue = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txChannelMask, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxChannelMask, cleanup);

    /* Read Tx Scratchpad value */
    recoveryAction = adrv903x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                    ADRV903X_GPIO_ANTCAL_TX_MASK,
                                                    &scratchPadValue);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read GPIO antenna cal Tx selection to scratchpad register");
        goto cleanup;
    }
    *txChannelMask = (uint32_t)scratchPadValue;

    /* Read Rx Scratchpad value */
    recoveryAction = adrv903x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                    ADRV903X_GPIO_ANTCAL_RX_MASK,
                                                    &scratchPadValue);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read GPIO antenna cal Rx selection to scratchpad register");
        goto cleanup;
    }
    *rxChannelMask = (uint32_t)scratchPadValue;

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
    
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RadioCtrlAntCalConfigSet_v2(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_RxRadioCtrlAntennaCalConfig_t * const configRx,
                                                                          adi_adrv903x_TxRadioCtrlAntennaCalConfig_t * const configTx,
                                                                          const uint8_t                                      rxGainValue,
                                                                          const uint8_t                                      enableFreeze)
{

        static const uint32_t txScratchPad2[] = { (uint32_t)ADRV903X_ADDR_TX0_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_TX1_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_TX2_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_TX3_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_TX4_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_TX5_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_TX6_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_TX7_STREAM_SCRATCH2 };

    static const uint32_t rxScratchPad2[] = { (uint32_t)ADRV903X_ADDR_RX0_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_RX1_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_RX2_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_RX3_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_RX4_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_RX5_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_RX6_STREAM_SCRATCH2,
                                              (uint32_t)ADRV903X_ADDR_RX7_STREAM_SCRATCH2 };

    static const uint32_t rxScratchPad5[] = { (uint32_t)ADRV903X_ADDR_RX0_STREAM_SCRATCH5,
                                              (uint32_t)ADRV903X_ADDR_RX1_STREAM_SCRATCH5,
                                              (uint32_t)ADRV903X_ADDR_RX2_STREAM_SCRATCH5,
                                              (uint32_t)ADRV903X_ADDR_RX3_STREAM_SCRATCH5,
                                              (uint32_t)ADRV903X_ADDR_RX4_STREAM_SCRATCH5,
                                              (uint32_t)ADRV903X_ADDR_RX5_STREAM_SCRATCH5,
                                              (uint32_t)ADRV903X_ADDR_RX6_STREAM_SCRATCH5,
                                              (uint32_t)ADRV903X_ADDR_RX7_STREAM_SCRATCH5 };

    static const uint32_t TX_SCRATCH2_WRITE_MASK  = 0x00FFFFFFU;
    static const uint32_t RX_SCRATCH_WRITE_MASK  = 0xFFFFFFFFU;
    static const uint32_t RX_SCRATCH_WRITE_SHIFT = 24U;
    static const uint32_t RX_SCRATCH5_GAIN_SHIFT = 8U;
    static const uint32_t RX_SCRATCH5_WRITE_MASk = 0xFF01U;

    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t txScratch2WriteValue = 0U;
    uint32_t rxScratchWriteValue = 0U;
    uint32_t rxScratch5WriteValue = 0U;
    adi_adrv903x_RxGain_t rxGain;
    uint16_t attenRegVal = 0U;
    uint32_t chanId = 0U;
    uint32_t chanSel = 0U;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, configRx, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, configTx, cleanup);

    ADI_LIBRARY_MEMSET(&rxGain, 0, sizeof(rxGain));

    /* Check enable parameter is valid */
    if ((enableFreeze != ADI_ENABLE) && (enableFreeze != ADI_DISABLE))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               enableFreeze,
                               "enableFreeze parameter must be either 0 for disabled or 1 for enabled");
        goto cleanup;
    }

    /*Check that if requested Rx Channel valid*/
    if ((configRx->rxChannelMask & (~(uint32_t)ADI_ADRV903X_RX_MASK_ALL)) != 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configRx->rxChannelMask,
                               "Invalid channel is selected for Rx config. Valid values are 0 or any combinations of Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    if ((configTx->txChannelMask & (~(uint32_t)ADI_ADRV903X_TXALL)) != 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configTx->txChannelMask,
                               "Invalid channel is selected for Tx config. Valid values are 0 or any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    txScratch2WriteValue = (uint32_t)(configTx->txNcoFreqKhz) & TX_SCRATCH2_WRITE_MASK;
    /* Change the sign of the frequency because there is an I/Q swap in the HW */
    rxScratchWriteValue = (((uint32_t)configRx->rxGainIndex & 0x000000FF) << RX_SCRATCH_WRITE_SHIFT) |
                           ((uint32_t)(-configRx->rxNcoFreqKhz) & 0x00FFFFFF);

    /* for each Tx channel in txChannelMask */
    for (chanId = 0U; chanId < ADI_ADRV903X_MAX_TXCHANNELS; chanId++)
    {
        if ((configTx->txChannelMask & (1U << chanId)) == 0U)
        {
            /* Skip channel. It's not in chanMask. */
            continue;
        }

        if (configTx->txAttenuation_mdB > ADRV903X_TX_ATTEN_VALUE_MILLI_DB_MAX)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                   recoveryAction,
                                   configTx->txAttenuation_mdB,
                                   "Invalid txAttenuation_mdB value");
            goto cleanup;
        }

        /* ChanId is selected in chanMask */
        chanSel = (1U << chanId);
        recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)(chanSel), &txBaseAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }

        /* Conversion from the atten level (milli-dB) to equivalent
         * TxAttenTable index is always done based on a step size of 0.05.
         * Other step sizes are not supported. */
        attenRegVal = (configTx->txAttenuation_mdB / ADRV903X_TX_ATTEN_STEP_SIZE_DIV_0P05);

        recoveryAction = adrv903x_TxFuncs_TxAttenuation_BfSet(device,
                                                              NULL,
                                                              txBaseAddr,
                                                              attenRegVal);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Input Select");
            goto cleanup;
        }

        recoveryAction = adi_adrv903x_Register32Write(device, NULL, txScratchPad2[chanId], txScratch2WriteValue, TX_SCRATCH2_WRITE_MASK);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write Tx scratch pad register 2");
            goto cleanup;
        }
    }

    /* for each Rx channel in rxChannelMask */
    for (chanId = 0U; chanId < ADI_ADRV903X_MAX_RX_ONLY; chanId++)
    {
        /* Write freeze and gainvalue to each scratchpad register */
        rxScratch5WriteValue = (((uint32_t)enableFreeze)  | ((uint32_t)rxGainValue) << RX_SCRATCH5_GAIN_SHIFT);
        recoveryAction = adi_adrv903x_Register32Write(device, NULL, rxScratchPad5[chanId], rxScratch5WriteValue, RX_SCRATCH5_WRITE_MASk);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write Rx scratch pad register 5");
            goto cleanup;
        }

        if ((configRx->rxChannelMask & (1U << chanId)) == 0)
        {
            /* Skip channel. It's not in chanMask. */
            continue;
        }

        rxGain.gainIndex = configRx->rxGainIndex;
        rxGain.rxChannelMask = configRx->rxChannelMask;

        recoveryAction = adrv903x_RxGainSetRangeCheck(device, &rxGain, 1);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Rx gain is out of range");
            goto cleanup;
        }

        /* ChanId is selected in chanMask */
        recoveryAction = adi_adrv903x_Register32Write(device, NULL, rxScratchPad2[chanId], rxScratchWriteValue, RX_SCRATCH_WRITE_MASK);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write Rx scratch pad register 2");
            goto cleanup;
        }

    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_GpioCtrlRxTxMapSet(adi_adrv903x_Device_t* const device,
                                                                 const adi_adrv903x_TxRxCtrlGpioMap_t txRxCtrlGpioMap[],
                                                                 const uint32_t numGpios)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    adi_adrv903x_StreamGpioPinCfg_t streamGpioCfg;
    adi_adrv903x_GpioSignal_e       signalArray[ADI_ADRV903X_GPIO_INVALID];
    uint32_t                        gpioMaskArray[ADI_ADRV903X_GPIO_INVALID];
    adi_adrv903x_TxRxCtrlGpioMap_t  txRxCtrlByGpio;

    uint32_t scratchRegisterAddress = 0U;
    uint8_t  channelMaskArray[1]    = { 0U };
    uint8_t  rxOrTxMaskArray[1]     = { 0U };
    uint8_t  prevReg216Value[1]     = { 0U };
    uint8_t  prevReg216Mask[1]      = { 0xFFU };

    uint8_t i = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txRxCtrlGpioMap, cleanup);

    ADI_LIBRARY_MEMSET(&streamGpioCfg, 0U, sizeof(streamGpioCfg));
    ADI_LIBRARY_MEMSET(&signalArray, 0U, sizeof(signalArray));
    ADI_LIBRARY_MEMSET(&gpioMaskArray, 0U, sizeof(gpioMaskArray));
    ADI_LIBRARY_MEMSET(&txRxCtrlByGpio, 0U, sizeof(txRxCtrlByGpio));

    if (numGpios > ADI_ADRV903X_GPIO_AS_TRXCONTROL_MAX_NUMBER_PINS)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numGpios, "Invalid array size. numGpios must be equal or smaller than 4");
        goto cleanup;
    }

    for (i = 0U; i < numGpios; i++)
    {
        txRxCtrlByGpio = txRxCtrlGpioMap[i];
        if ((txRxCtrlByGpio.logicalPin != ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_0) && 
            (txRxCtrlByGpio.logicalPin != ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_1) &&
            (txRxCtrlByGpio.logicalPin != ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_2) &&
            (txRxCtrlByGpio.logicalPin != ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_3))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRxCtrlByGpio.logicalPin, "Invalid logicalPin parameter value inside structure. Must be from 0 to 3");
            goto cleanup;
        }

        if (txRxCtrlByGpio.gpioPin >= ADI_ADRV903X_GPIO_INVALID)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRxCtrlByGpio.logicalPin, "Invalid gpioPin parameter value inside structure. Must be between 0 and 23");
            goto cleanup;
        }

        if (txRxCtrlByGpio.rxTxMask == ADRV903X_STREAM_TRIGGER_GPIO_TXRXMASK_TX)
        {
            if ((txRxCtrlByGpio.channelMask < (uint32_t)ADI_ADRV903X_TX0) || (txRxCtrlByGpio.channelMask > (uint32_t)ADI_ADRV903X_TX7))
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRxCtrlByGpio.channelMask, "Invalid Tx channelMask parameter passed - must be from Tx0 up to Tx7");
                goto cleanup;
            }
        }
        else if (txRxCtrlByGpio.rxTxMask == ADRV903X_STREAM_TRIGGER_GPIO_TXRXMASK_RX)
        {
            if (txRxCtrlByGpio.channelMask < (uint8_t)ADI_ADRV903X_RX0)
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRxCtrlByGpio.channelMask, "Invalid Rx channelMask parameter passed - must be from Rx0 up to Rx7");
                goto cleanup;
            }
        }
        else 
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRxCtrlByGpio.rxTxMask, "Invalid rxTxMask enum passed. Value must be 1 for Rx or 2 for Tx");
            goto cleanup;
        }

        scratchRegisterAddress = (uint32_t)ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS + ADRV903X_ADDR_CORE_STREAM_SCRATCH212 + (uint32_t)txRxCtrlByGpio.logicalPin;

        /* Check if target pin is not being used by other signals */
        recoveryAction = adi_adrv903x_GpioConfigAllGet(device, signalArray, gpioMaskArray, (uint8_t)ADI_ADRV903X_GPIO_INVALID);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while retreiving current GPIO configurations");
            goto cleanup;
        }

        if (signalArray[(uint8_t)txRxCtrlByGpio.gpioPin] != ADI_ADRV903X_GPIO_SIGNAL_UNUSED)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRxCtrlByGpio.gpioPin, "Pin already routed to another signal. Please select another pin");
            goto cleanup;
        }

        channelMaskArray[0] = (uint8_t)txRxCtrlByGpio.channelMask;

        /* 
         * ScratchRegisterAddress is which logical gpio is tied to the physical pin. 
         * channelMask tells which channels are linked to the logical gpio.
         */
        recoveryAction = adi_adrv903x_RegistersByteWrite(device,
                                                        NULL,
                                                        scratchRegisterAddress,
                                                        channelMaskArray,
                                                        1U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while writing on stream scratch register");
            goto cleanup;
        }

        if (txRxCtrlByGpio.logicalPin == ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_0)
        {
            rxOrTxMaskArray[0] = txRxCtrlByGpio.rxTxMask;
        } 
        else if (txRxCtrlByGpio.logicalPin == ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_1) 
        {
            rxOrTxMaskArray[0] = txRxCtrlByGpio.rxTxMask << 2U;
        }
        else if (txRxCtrlByGpio.logicalPin == ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_2)
        {
            rxOrTxMaskArray[0] = txRxCtrlByGpio.rxTxMask << 4U;
        }
        else
        {
            rxOrTxMaskArray[0] = txRxCtrlByGpio.rxTxMask << 6U;
        }

        /* 
         * Scratch Register 216 holds if each gpio is tied to rx or tx for all gpios, so we need to 
         * read first  and use it as a mask before updating it.
         */
        recoveryAction = adi_adrv903x_RegistersByteRead(device,
                                                        NULL,
                                                        (uint32_t)ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS + ADRV903X_ADDR_CORE_STREAM_SCRATCH216,
                                                        prevReg216Value,
                                                        prevReg216Mask,
                                                        1U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while reading stream scratch register");
            goto cleanup;
        }

        rxOrTxMaskArray[0] |= prevReg216Value[0];

        recoveryAction = adi_adrv903x_RegistersByteWrite(device,
                                                        NULL,
                                                        (uint32_t)ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS + ADRV903X_ADDR_CORE_STREAM_SCRATCH216,
                                                        rxOrTxMaskArray,
                                                        1U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while writing on stream scratch register");
            goto cleanup;
        }

        recoveryAction = adi_adrv903x_StreamGpioConfigGet(device, &streamGpioCfg);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Stream Gpio Config Get Failed");
            goto cleanup;
        }

        if (streamGpioCfg.streamGpInput[(uint8_t)txRxCtrlByGpio.gpioPin] != ADI_ADRV903X_GPIO_INVALID)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRxCtrlByGpio.gpioPin, "Pin already configured to a stream. Please review gpio and stream assignment");
            goto cleanup;
        }

        streamGpioCfg.streamGpInput[(uint8_t)txRxCtrlByGpio.gpioPin] = txRxCtrlByGpio.gpioPin;

        recoveryAction = adi_adrv903x_StreamGpioConfigSet(device, &streamGpioCfg);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Stream Gpio Config Set Failed");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_GpioCtrlRxTxMapGet(adi_adrv903x_Device_t* const device,
                                                                 const adi_adrv903x_GpioPinSel_e gpioPin,
                                                                 const adi_adrv903x_TxRxCtrlGpioLogicalPin_e gpioLogicalPin,
                                                                 adi_adrv903x_TxRxCtrlGpioMap_t* const txRxCtrlGpio)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_adrv903x_StreamGpioPinCfg_t streamGpioCfg;
        
    uint32_t scratchRegisterAddress = 0U;
    uint8_t  channelMaskArray[1]    = { 0U };
    uint8_t  regRxOrTxValue[1]      = { 0U };
    uint8_t  registerReadMask[1]    = { 0xFFU };

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txRxCtrlGpio, cleanup);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_LIBRARY_MEMSET(&streamGpioCfg, 0U, sizeof(streamGpioCfg));

    if ((gpioLogicalPin != ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_0) && 
        (gpioLogicalPin != ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_1) &&
        (gpioLogicalPin != ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_2) &&
        (gpioLogicalPin != ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_3))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gpioLogicalPin, "Invalid gpioLogicalPin parameter value. Must be from 0 to 3");
        goto cleanup;
    }

    if (gpioPin >= ADI_ADRV903X_GPIO_INVALID)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gpioPin, "Invalid gpioPin parameter value. Must be between 0 and 23");
        goto cleanup;
    }

    recoveryAction = adi_adrv903x_StreamGpioConfigGet(device, &streamGpioCfg);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Stream Gpio Config Get Failed");
        goto cleanup;
    }

    if (streamGpioCfg.streamGpInput[(uint8_t)gpioPin] == gpioPin)
    {
        txRxCtrlGpio->gpioPin = gpioPin;
    }
    else
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gpioPin, "GPIO Pin not assigned to a signal. Please confirm this is the correct pin and it was configured correctly");
        goto cleanup;
    }

    txRxCtrlGpio->logicalPin = gpioLogicalPin;

    scratchRegisterAddress = (uint32_t)ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS + ADRV903X_ADDR_CORE_STREAM_SCRATCH212 + (uint32_t)gpioLogicalPin;

    /* 
     * ScratchRegisterAddress is which logical gpio is tied to the physical pin. 
     * channelMask tells which channels are linked to the logical gpio.
     */
    recoveryAction = adi_adrv903x_RegistersByteRead(device,
                                                    NULL,
                                                    scratchRegisterAddress,
                                                    channelMaskArray,
                                                    registerReadMask,
                                                    1U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while writing on stream scratch register");
        goto cleanup;
    }

    txRxCtrlGpio->channelMask = channelMaskArray[0];

    recoveryAction = adi_adrv903x_RegistersByteRead(device,
                                                    NULL,
                                                    (uint32_t)ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS + ADRV903X_ADDR_CORE_STREAM_SCRATCH216,
                                                    regRxOrTxValue,
                                                    registerReadMask,
                                                    1U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while reading stream scratch register");
        goto cleanup;
    }

    if (gpioLogicalPin == ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_1) 
    {
        regRxOrTxValue[0] = regRxOrTxValue[0] >> 2U; 
    }
    else if (gpioLogicalPin == ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_2)
    {
        regRxOrTxValue[0] = regRxOrTxValue[0] >> 4U; 
    }
    else if (gpioLogicalPin == ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_3)
    {
        regRxOrTxValue[0] = regRxOrTxValue[0] >> 6U; 
    }
    
    regRxOrTxValue[0] &= 0x3;

    txRxCtrlGpio->rxTxMask = (adi_adrv903x_TxRxCtrlGpioRxOrTx_e)regRxOrTxValue[0];

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_GpioCtrlRxTxMapClear(adi_adrv903x_Device_t* const device,
                                                                   const adi_adrv903x_TxRxCtrlGpioMap_t txRxCtrlGpioMap[],
                                                                   const uint32_t numGpios)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    adi_adrv903x_StreamGpioPinCfg_t streamGpioCfg;
    adi_adrv903x_GpioSignal_e       signalArray[ADI_ADRV903X_GPIO_INVALID];
    adi_adrv903x_TxRxCtrlGpioMap_t  gpioLogPhyMapping;
    uint32_t scratchRegisterAddress = 0U;
    uint8_t clearChannelMaskArray[1] = { 0U };
    uint8_t clearRxOrTxMaskArray[1] = { 0U };
    uint8_t i = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txRxCtrlGpioMap, cleanup);

    ADI_LIBRARY_MEMSET(&streamGpioCfg, 0U, sizeof(streamGpioCfg));
    ADI_LIBRARY_MEMSET(&signalArray, 0U, sizeof(signalArray));
    ADI_LIBRARY_MEMSET(&gpioLogPhyMapping, 0U, sizeof(gpioLogPhyMapping));
    
    if (numGpios > ADI_ADRV903X_GPIO_AS_TRXCONTROL_MAX_NUMBER_PINS)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numGpios, "Invalid array size. numGpios must be equal or smaller than 4");
        goto cleanup;
    }

    for (i = 0U; i < numGpios; i++)
    {
        gpioLogPhyMapping = txRxCtrlGpioMap[i];
        if ((gpioLogPhyMapping.logicalPin != ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_0) && 
            (gpioLogPhyMapping.logicalPin != ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_1) &&
            (gpioLogPhyMapping.logicalPin != ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_2) &&
            (gpioLogPhyMapping.logicalPin != ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_3))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gpioLogPhyMapping.logicalPin, "Invalid logicalPin parameter value inside structure. Must be from 0 to 3");
            goto cleanup;
        }

        if (gpioLogPhyMapping.gpioPin >= ADI_ADRV903X_GPIO_INVALID)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gpioLogPhyMapping.logicalPin, "Invalid gpioPin parameter value inside structure. Must be between 0 and 23");
            goto cleanup;
        }

        recoveryAction = adi_adrv903x_StreamGpioConfigGet(device, &streamGpioCfg);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Stream Gpio Config Get Failed");
            goto cleanup;
        }

        streamGpioCfg.streamGpInput[(uint8_t)gpioLogPhyMapping.gpioPin] = ADI_ADRV903X_GPIO_INVALID;

        recoveryAction = adi_adrv903x_StreamGpioConfigSet(device, &streamGpioCfg);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Stream Gpio Config Reset Failed");
            goto cleanup;
        }

        recoveryAction = adi_adrv903x_RegistersByteWrite(device,
                                                        NULL,
                                                        (uint32_t)ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS + ADRV903X_ADDR_CORE_STREAM_SCRATCH216,
                                                        clearRxOrTxMaskArray,
                                                        1U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while writing on stream scratch register.");
            goto cleanup;
        }

        scratchRegisterAddress = (uint32_t)ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS + ADRV903X_ADDR_CORE_STREAM_SCRATCH212 + (uint32_t)gpioLogPhyMapping.logicalPin;

        recoveryAction = adi_adrv903x_RegistersByteWrite(device,
                                                        NULL,
                                                        scratchRegisterAddress,
                                                        clearChannelMaskArray,
                                                        1U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while writing on stream scratch register.");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);

}

