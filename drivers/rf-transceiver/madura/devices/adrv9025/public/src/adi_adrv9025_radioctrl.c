// SPDX-License-Identifier: GPL-2.0
/**
* \file adi_adrv9025_radioctrl.c
* \brief Contains features related function implementation defined in
* adi_adrv9025_radioctrl.h
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "adi_adrv9025_radioctrl.h"
#include "adi_adrv9025_cals.h"
#include "adi_adrv9025_cpu.h"
#include "adi_adrv9025_gpio.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025.h"
#include "adi_adrv9025_arm.h"
#include "adi_adrv9025_rx.h"
#include "adi_adrv9025_tx.h"
#include "../../private/include/adrv9025_radioctrl.h"
#include "../../private/include/adrv9025_init.h"
#include "../../private/include/adrv9025_cpu.h"
#include "../../private/include/adrv9025_cpu_macros.h"
#include "../../private/include/adrv9025_reg_addr_macros.h"
#include "../../private/include/adrv9025_bf_pll_mem_map.h"
#include "../../private/include/adrv9025_shared_resource_manager.h"

#ifdef _RELEASE_BUILD_
    #line __LINE__ "adi_adrv9025_radioctrl.c"
#endif

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

static void adrv9025_BinaryImageInfoGet(adi_adrv9025_Device_t* device,
                                        const uint8_t          binary[],
                                        uint16_t*              binaryImageSize,
                                        uint16_t*              binaryImageOffset)
{
    UNUSED_PARA(device);
    /* populating variables from binary array */
    *binaryImageSize   = ((((uint16_t)binary[1]) << 8) | ((uint16_t)binary[0]));
    *binaryImageOffset = ((((uint16_t)binary[3]) << 8) | (uint16_t)(binary[2]));
}

static void adrv9025_BinaryParamsGetBaseAddr(adi_adrv9025_Device_t* device,
                                             const uint8_t          binary[],
                                             uint32_t*              word)
{
    UNUSED_PARA(device);
    /* populating variables from binary array */
    *word = (((uint32_t)binary[3]) << 24) | (((uint32_t)binary[2]) << 16) | (((uint32_t)binary[1]) << 8) | (uint32_t)(binary[0]);
}

static void adrv9025_BinaryParamsGetNumberStreamImageSize(adi_adrv9025_Device_t* device,
                                                          const uint8_t          binary[],
                                                          uint8_t*               numberStreams,
                                                          uint32_t*              imageSize)
{
    UNUSED_PARA(device);
    *numberStreams = binary[0];
    *imageSize     = (((uint32_t)binary[3]) << 8) | (uint32_t)(binary[2]);
}

int32_t adi_adrv9025_StreamImageWrite(adi_adrv9025_Device_t* device,
                                      uint32_t               byteOffset,
                                      uint8_t                binary[],
                                      uint32_t               byteCount)
{
    uint16_t binaryImageSize   = 0;
    uint16_t binaryImageOffset = 0;
    uint32_t i                 = 0;
    uint32_t addr              = 0;
    uint32_t word1             = 0;
    uint32_t binOffset         = 0;
    uint8_t  streamCtl         = 0;
    uint8_t  byte1             = 0;
    uint32_t imageSize         = 0;
    uint32_t imageOffset       = 0;
    uint8_t  mainStreamBankSel = 0;
    uint32_t streamAddr[]      = { ADRV9025_ADDR_MAIN_STREAM_CTL_OFFSET,
                                   ADRV9025_ADDR_TX0_STREAM_CTL_OFFSET,
                                   ADRV9025_ADDR_TX1_STREAM_CTL_OFFSET,
                                   ADRV9025_ADDR_TX2_STREAM_CTL_OFFSET,
                                   ADRV9025_ADDR_TX3_STREAM_CTL_OFFSET,
                                   ADRV9025_ADDR_RX0_STREAM_CTL_OFFSET,
                                   ADRV9025_ADDR_RX1_STREAM_CTL_OFFSET,
                                   ADRV9025_ADDR_RX2_STREAM_CTL_OFFSET,
                                   ADRV9025_ADDR_RX3_STREAM_CTL_OFFSET,
                                   ADRV9025_ADDR_ORX0_STREAM_CTL_OFFSET,
                                   ADRV9025_ADDR_ORX1_STREAM_CTL_OFFSET
    };

    uint8_t streamControlDefaultVal[] = {0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xFC, 0xFC};

#ifdef ADRV9025_INIT_DEBUG
    static const char * imageName[] = { "MAIN_STREAM", "TX0_STREAM", "TX1_STREAM","TX2_STREAM","TX3_STREAM",
        "RX0_STREAM","RX1_STREAM","RX2_STREAM","RX3_STREAM", "ORX0_STREAM", "ORX1_STREAM" };
#endif
    static const uint8_t MIN_BYTECOUNT     = 68;
    static const uint8_t MAIN_IMAGE_OFFSET = 64;

    /* Arm stream download order: main, tx1/2/3/4, rx1/2/3/4, orx12/34 */
    static const uint32_t streamChannel[] = {
        0xFFFFFFFF,
        ADI_ADRV9025_TX1 << ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV9025_TX2 << ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV9025_TX3 << ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV9025_TX4 << ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV9025_RX1,
        ADI_ADRV9025_RX2,
        ADI_ADRV9025_RX3,
        ADI_ADRV9025_RX4,
        (ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2 | ADI_ADRV9025_LB12),
        (ADI_ADRV9025_ORX3 | ADI_ADRV9025_ORX4 | ADI_ADRV9025_LB34)
    };

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        binary);

    ADRV9025_BUGINFO(__FUNCTION__);

    if ((byteCount % 4) > 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         byteCount,
                         "Invalid byteCount. Must be multiple of 4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((byteOffset % 4) > 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         byteCount,
                         "Invalid byteOffset. Must be multiple of 4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (byteOffset == 0)
    {
        if (byteCount < MIN_BYTECOUNT)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             byteCount,
                             "Invalid byteCount. Must be 68 or greater.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        for (i = 0; i < ADRV9025_STREAM_MAX; i++)
        {
            adrv9025_BinaryImageInfoGet(device,
                                        &binary[12 + i * 4],
                                        &binaryImageSize,
                                        &binaryImageOffset);
            device->devStateInfo.chunkStreamImageSize[i]   = binaryImageSize;
            device->devStateInfo.chunkStreamImageOffset[i] = binaryImageOffset;
#ifdef ADRV9025_INIT_DEBUG
            ADRV9025_BUGINFO(imageName[i]);
            ADRV9025_BUGINFO_NUM("binaryImageOffset", (uint32_t)binaryImageOffset);
            ADRV9025_BUGINFO_NUM("binaryImageSize", (uint32_t)binaryImageSize);
#endif
        }

        device->devStateInfo.currentStreamImageIndex    = 0;
        device->devStateInfo.currentStreamBinBaseAddr   = 0;
        device->devStateInfo.currentStreamBaseAddr      = 0;
        device->devStateInfo.currentStreamImageSize     = 0;
        device->devStateInfo.currentStreamNumberStreams = 0;
    }

    for (i = device->devStateInfo.currentStreamImageIndex; i < ADRV9025_STREAM_MAX; i++)
    {
#ifdef ADRV9025_INIT_DEBUG
        ADRV9025_BUGINFO(imageName[i]);
        ADRV9025_BUGINFO_NUM("binary", (uint32_t)binary);
        ADRV9025_BUGINFO_NUM("byteCount", (uint32_t)byteCount);
#endif
        imageOffset = device->devStateInfo.chunkStreamImageOffset[i];
        if (i == 0)
        {
            imageOffset = MAIN_IMAGE_OFFSET;
        }
        if ((byteOffset <= imageOffset) &&
            ((byteOffset + byteCount) >= imageOffset + 4))
        {
            binOffset = imageOffset - byteOffset;
            adrv9025_BinaryParamsGetBaseAddr(device,
                                             &binary[binOffset],
                                             &word1);
            device->devStateInfo.currentStreamBinBaseAddr = word1;
            device->devStateInfo.currentStreamImageSize   = device->devStateInfo.chunkStreamImageSize[i];
        }

        if ((byteOffset <= imageOffset + 4) &&
            ((byteOffset + byteCount) >= imageOffset + 8))
        {
            binOffset = imageOffset + 4 - byteOffset;
            adrv9025_BinaryParamsGetBaseAddr(device,
                                             &binary[binOffset],
                                             &word1);
            device->devStateInfo.currentStreamBaseAddr = word1;
        }

        if ((byteOffset <= imageOffset + 8) &&
            ((byteOffset + byteCount) >= imageOffset + 12))
        {
            binOffset = imageOffset + 8 - byteOffset;
            adrv9025_BinaryParamsGetNumberStreamImageSize(device,
                                                          &binary[binOffset],
                                                          &byte1,
                                                          &word1);
            device->devStateInfo.currentStreamNumberStreams = byte1;
            //device->devStateInfo.chunkImageSize = word1;
        }

        if ((device->devStateInfo.currentStreamBinBaseAddr > 0) &&
            (device->devStateInfo.currentStreamImageSize > 0))
        {
            addr = streamAddr[i];

            if (device->devStateInfo.currentStreamImageSize == device->devStateInfo.chunkStreamImageSize[i])
            {
                if ((device->devStateInfo.initializedChannels & streamChannel[i]) > 0)
                {
                    streamCtl = streamControlDefaultVal[i] | ADRV9025_STREAM_RESET;
                    ADRV9025_SPIWRITEBYTE("STREAM_CTL",
                                          addr,
                                          streamCtl);
                }
            }

            /* 3.    Load the stream image to memory, starting at the memory address specified by Stream_binary_base. */
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
                adrv9025_CpuDmaMemWrite(device,
                                        device->devStateInfo.currentStreamBinBaseAddr,
                                        &binary[0],
                                        imageSize,
                                        ADI_ADRV9025_CPU_MEM_AUTO_INCR);
                ADI_ERROR_RETURN(device->common.error.newAction);
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
                    /* 4.    Populate registers(stream_base_byte0) and (stream_base_byte1)with the lower 16 bits of the Stream_base. */
                    ADRV9025_SPIWRITEBYTE("STREAM_BASE_BYTE0",
                                          addr + ADRV9025_STREAM_BASE_BYTE0_OFFSET,
                                          (uint8_t)(device->devStateInfo.currentStreamBaseAddr));

                    ADRV9025_SPIWRITEBYTE("STREAM_BASE_BYTE1",
                                          addr + ADRV9025_STREAM_BASE_BYTE1_OFFSET,
                                          (uint8_t)(device->devStateInfo.currentStreamBaseAddr >> 8));

                    /* 5.    Populate register  (last_stream_num)with the No_of_streams value. */
                    ADRV9025_SPIWRITEBYTE("LAST_STREAM_NUMBER",
                                          addr + ADRV9025_LAST_STREAM_NUMBER_OFFSET,
                                          device->devStateInfo.currentStreamNumberStreams - 1);

                    /* 6.    Clear bit D0 in register  (stream_reset) */
                    if (i == 0)
                    {
                        /* Set memory bank select for main stream processor */
                        if ((device->devStateInfo.currentStreamBaseAddr >= 0x20018000) &&
                            (device->devStateInfo.currentStreamBaseAddr <= 0x2004FFFF))
                        {
                            mainStreamBankSel = (device->devStateInfo.currentStreamBaseAddr >> 16) & 0x07;

                            /* Set memory bank select for main stream processor */
                            streamCtl = (mainStreamBankSel << 5) & ~ADRV9025_STREAM_RESET;
                        }
                        else
                        {
                            ADI_ERROR_REPORT(&device->common,
                                             ADI_COMMON_ERRSRC_API,
                                             ADI_COMMON_ERR_INV_PARAM,
                                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                             mainStreamBankSel,
                                             "Main Stream Processor image has invalid memory location");
                            ADI_ERROR_RETURN(device->common.error.newAction);
                        }
                    }
                    else
                    {
                        /* Just clear reset bit on other channel processors */
                        streamCtl = streamControlDefaultVal[i] & ~ADRV9025_STREAM_RESET;
                    }

                    ADRV9025_SPIWRITEBYTE("STREAM_CTL",
                                          addr,
                                          streamCtl);
                }

                device->devStateInfo.currentStreamImageIndex++;
                device->devStateInfo.currentStreamBinBaseAddr   = 0;
                device->devStateInfo.currentStreamBaseAddr      = 0;
                device->devStateInfo.currentStreamImageSize     = 0;
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

    if (device->devStateInfo.currentStreamImageIndex == ADRV9025_STREAM_MAX)
    {
        device->devStateInfo.devState = (adi_adrv9025_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV9025_STATE_STREAMLOADED);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_ArmGpioPinsSetRangeCheck(adi_adrv9025_Device_t*     device,
                                              adi_adrv9025_ArmGpioCfg_t* armGpio)
{
    static const uint8_t SIGNAL_ID[] = { ADRV9025_CPU_ORX1_TX_SEL0_SIGNALID,
                                         ADRV9025_CPU_ORX1_TX_SEL1_SIGNALID,
                                         ADRV9025_CPU_ORX2_TX_SEL0_SIGNALID,
                                         ADRV9025_CPU_ORX2_TX_SEL1_SIGNALID,
                                         ADRV9025_CPU_ORX3_TX_SEL0_SIGNALID,
                                         ADRV9025_CPU_ORX3_TX_SEL1_SIGNALID,
                                         ADRV9025_CPU_ORX4_TX_SEL0_SIGNALID,
                                         ADRV9025_CPU_ORX4_TX_SEL1_SIGNALID,
                                         ADRV9025_CPU_TX12CAL_ENA_SIGNALID,
                                         ADRV9025_CPU_TX34CAL_ENA_SIGNALID,
                                         ADRV9025_CPU_CAL_UPDATE0_SIGNALID,
                                         ADRV9025_CPU_CAL_UPDATE1_SIGNALID,
                                         ADRV9025_CPU_CAL_UPDATE2_SIGNALID,
                                         ADRV9025_CPU_RF_PLL_FREQ_HOP_SIGNALID
    };

    uint8_t signalIndex = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        armGpio);

    for (signalIndex = 0; signalIndex < sizeof(SIGNAL_ID); signalIndex++)
    {
        switch (SIGNAL_ID[signalIndex])
        {
        case(ADRV9025_CPU_ORX1_TX_SEL0_SIGNALID):
        {
            if ((armGpio->orx1TxSel0Pin.enable > 0) &&
                (armGpio->orx1TxSel0Pin.gpioPinSel > ADI_ADRV9025_GPIO_18))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 armGpio->orx1TxSel0Pin.gpioPinSel,
                                 "Invalid Gpio Pin encountered for signal ORX1_TX_SEL0");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            break;
        }
        case(ADRV9025_CPU_ORX1_TX_SEL1_SIGNALID):
        {
            if ((armGpio->orx1TxSel1Pin.enable > 0) &&
                (armGpio->orx1TxSel1Pin.gpioPinSel > ADI_ADRV9025_GPIO_18))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 armGpio->orx1TxSel1Pin.gpioPinSel,
                                 "Invalid Gpio Pin encountered for signal ORX1_TX_SEL1");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            break;
        }
        case(ADRV9025_CPU_ORX2_TX_SEL0_SIGNALID):
        {
            if ((armGpio->orx2TxSel0Pin.enable > 0) &&
                (armGpio->orx2TxSel0Pin.gpioPinSel > ADI_ADRV9025_GPIO_18))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 armGpio->orx2TxSel0Pin.gpioPinSel,
                                 "Invalid Gpio Pin encountered for signal ORX2_TX_SEL0");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            break;
        }
        case(ADRV9025_CPU_ORX2_TX_SEL1_SIGNALID):
        {
            if ((armGpio->orx2TxSel1Pin.enable > 0) &&
                (armGpio->orx2TxSel1Pin.gpioPinSel > ADI_ADRV9025_GPIO_18))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 armGpio->orx2TxSel1Pin.gpioPinSel,
                                 "Invalid Gpio Pin encountered for signal ORX2_TX_SEL1");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            break;
        }
        case(ADRV9025_CPU_ORX3_TX_SEL0_SIGNALID):
        {
            if ((armGpio->orx3TxSel0Pin.enable > 0) &&
                (armGpio->orx3TxSel0Pin.gpioPinSel > ADI_ADRV9025_GPIO_18))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 armGpio->orx3TxSel0Pin.gpioPinSel,
                                 "Invalid Gpio Pin encountered for signal ORX3_TX_SEL0");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            break;
        }
        case(ADRV9025_CPU_ORX3_TX_SEL1_SIGNALID):
        {
            if ((armGpio->orx3TxSel1Pin.enable > 0) &&
                (armGpio->orx3TxSel1Pin.gpioPinSel > ADI_ADRV9025_GPIO_18))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 armGpio->orx3TxSel1Pin.gpioPinSel,
                                 "Invalid Gpio Pin encountered for signal ORX3_TX_SEL1");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            break;
        }
        case(ADRV9025_CPU_ORX4_TX_SEL0_SIGNALID):
        {
            if ((armGpio->orx4TxSel0Pin.enable > 0) &&
                (armGpio->orx4TxSel0Pin.gpioPinSel > ADI_ADRV9025_GPIO_18))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 armGpio->orx4TxSel0Pin.gpioPinSel,
                                 "Invalid Gpio Pin encountered for signal ORX4_TX_SEL0");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            break;
        }
        case(ADRV9025_CPU_ORX4_TX_SEL1_SIGNALID):
        {
            if ((armGpio->orx4TxSel1Pin.enable > 0) &&
                (armGpio->orx4TxSel1Pin.gpioPinSel > ADI_ADRV9025_GPIO_18))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 armGpio->orx4TxSel1Pin.gpioPinSel,
                                 "Invalid Gpio Pin encountered for signal ORX4_TX_SEL1");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            break;
        }
        case(ADRV9025_CPU_TX12CAL_ENA_SIGNALID):
        {
            if ((armGpio->tx12CalEnPin.enable > 0) &&
                (armGpio->tx12CalEnPin.gpioPinSel > ADI_ADRV9025_GPIO_18))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 armGpio->tx12CalEnPin.gpioPinSel,
                                 "Invalid Gpio Pin encountered for signal TX12CAL_ENA");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            break;
        }
        case(ADRV9025_CPU_TX34CAL_ENA_SIGNALID):
        {
            if ((armGpio->tx34CalEnPin.enable > 0) &&
                (armGpio->tx34CalEnPin.gpioPinSel > ADI_ADRV9025_GPIO_18))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 armGpio->tx34CalEnPin.gpioPinSel,
                                 "Invalid Gpio Pin encountered for signal TX34CAL_ENA");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            break;
        }
        case(ADRV9025_CPU_CAL_UPDATE0_SIGNALID):
        {
            if ((armGpio->calUpdate0Pin.enable > 0) &&
                (armGpio->calUpdate0Pin.gpioPinSel > ADI_ADRV9025_GPIO_18))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 armGpio->calUpdate0Pin.gpioPinSel,
                                 "Invalid Gpio Pin encountered for signal CAL_UPDATE0");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            break;
        }
        case(ADRV9025_CPU_CAL_UPDATE1_SIGNALID):
        {
            if ((armGpio->calUpdate1Pin.enable > 0) &&
                (armGpio->calUpdate1Pin.gpioPinSel > ADI_ADRV9025_GPIO_18))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 armGpio->calUpdate1Pin.gpioPinSel,
                                 "Invalid Gpio Pin encountered for signal CAL_UPDATE1");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            break;
        }
        case(ADRV9025_CPU_CAL_UPDATE2_SIGNALID):
        {
            if ((armGpio->calUpdate2Pin.enable > 0) &&
                (armGpio->calUpdate2Pin.gpioPinSel > ADI_ADRV9025_GPIO_18))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 armGpio->calUpdate2Pin.gpioPinSel,
                                 "Invalid Gpio Pin encountered for signal CAL_UPDATE2");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            break;
        }
        case(ADRV9025_CPU_RF_PLL_FREQ_HOP_SIGNALID):
        {
            if ((armGpio->rfPllFreqHopPin.enable > 0) &&
                (armGpio->rfPllFreqHopPin.gpioPinSel > ADI_ADRV9025_GPIO_18))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 armGpio->calUpdate2Pin.gpioPinSel,
                                 "Invalid Gpio Pin encountered for signal RF_PLL_FREQ_HOP");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            break;
        }
        default:
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             SIGNAL_ID[signalIndex],
                             "Invalid Signal ID encountered while attempting to configure ARM GPIO pins");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_ArmGpioPinsSet(adi_adrv9025_Device_t*     device,
                                    adi_adrv9025_ArmGpioCfg_t* armGpio)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, armGpio);

    UNUSED_PARA(device);
    UNUSED_PARA(armGpio);
    return 0;
}

int32_t adi_adrv9025_ArmGpioPinsGet(adi_adrv9025_Device_t*     device,
                                    adi_adrv9025_ArmGpioCfg_t* armGpio)
{
	ADI_NULL_DEVICE_PTR_RETURN(device);

	ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

	ADI_NULL_PTR_RETURN(&device->common, armGpio);

	UNUSED_PARA(device);
    UNUSED_PARA(armGpio);
    return 0;
}

int32_t adi_adrv9025_RadioCtrlCfgSetRangeCheck(adi_adrv9025_Device_t*           device,
                                               adi_adrv9025_RadioCtrlModeCfg_t* radioCtrlCfg)
{
    static const uint8_t MAX_PIN_SEL_SETTLING_DELAY_ARM_CLK_CYCLES = 0x0F;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        radioCtrlCfg);

    if (((device->devStateInfo.profilesValid & ADI_ADRV9025_RX_PROFILE_VALID) == ADI_ADRV9025_RX_PROFILE_VALID) &&
        (radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode != ADI_ADRV9025_RX_EN_INVALID_MODE) &&
        (radioCtrlCfg->rxRadioCtrlModeCfg.rxChannelMask != ADI_ADRV9025_RXOFF))
    {
        if ((radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode != ADI_ADRV9025_RX_EN_SPI_MODE) &&
            (radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode != ADI_ADRV9025_RX_EN_PIN_MODE))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode,
                             "Invalid Rx signal chain enable mode selected. Valid values include SPI, Pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    if (((device->devStateInfo.profilesValid & ADI_ADRV9025_TX_PROFILE_VALID) == ADI_ADRV9025_TX_PROFILE_VALID) &&
        (radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode != ADI_ADRV9025_TX_EN_INVALID_MODE) &&
        (radioCtrlCfg->txRadioCtrlModeCfg.txChannelMask != ADI_ADRV9025_TXOFF))
    {
        if ((radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode != ADI_ADRV9025_TX_EN_SPI_MODE) &&
            (radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode != ADI_ADRV9025_TX_EN_PIN_MODE))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode,
                             "Invalid Tx signal chain enable mode selected. Valid values include SPI, Pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    if (((device->devStateInfo.profilesValid & ADI_ADRV9025_ORX_PROFILE_VALID) == ADI_ADRV9025_ORX_PROFILE_VALID) &&
        (radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV9025_ORX_EN_INVALID_MODE))
    {
        if ((radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV9025_ORX_EN_SPI_MODE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV9025_ORX_EN_SINGLE_CH_3PIN_MODE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV9025_ORX_EN_SINGLE_CH_2PIN_MODE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV9025_ORX_EN_SINGLE_CH_1PIN_MODE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV9025_ORX_EN_DUAL_CH_4PIN_MODE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV9025_ORX_EN_DUAL_CH_2PIN_MODE))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode,
                             "Invalid ORx signal chain enable mode selected. Valid values include SPI, Single channel 3 Pin, 2 Pin, 1 Pin modes, Dual channel 4 pin, 2 pin modes");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV9025_ORX_EN_SPI_MODE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.orxPinSelectSettlingDelay_armClkCycles > MAX_PIN_SEL_SETTLING_DELAY_ARM_CLK_CYCLES))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             radioCtrlCfg->orxRadioCtrlModeCfg.orxPinSelectSettlingDelay_armClkCycles,
                             "Out of range pin select settling delay detected. Valid values are in the range 0x00 - 0x0F(2-18 ARM clk cycles)");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode == ADI_ADRV9025_ORX_EN_SINGLE_CH_1PIN_MODE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.singleChannel1PinModeOrxSel != ADI_ADRV9025_SINGLE_CH_PIN_MODE_ORX1_FE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.singleChannel1PinModeOrxSel != ADI_ADRV9025_SINGLE_CH_PIN_MODE_ORX2_FE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.singleChannel1PinModeOrxSel != ADI_ADRV9025_SINGLE_CH_PIN_MODE_ORX3_FE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.singleChannel1PinModeOrxSel != ADI_ADRV9025_SINGLE_CH_PIN_MODE_ORX4_FE))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             radioCtrlCfg->orxRadioCtrlModeCfg.singleChannel1PinModeOrxSel,
                             "Single channel 1 Pin ORx enable mode selected but ORx select is out of range. Valid ORx selects include ORx1, ORx2, ORx3, ORx4");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode == ADI_ADRV9025_ORX_EN_SINGLE_CH_2PIN_MODE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.singleChannel2PinModeLowOrxSel != ADI_ADRV9025_SINGLE_CH_PIN_MODE_ORX1_FE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.singleChannel2PinModeLowOrxSel != ADI_ADRV9025_SINGLE_CH_PIN_MODE_ORX2_FE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.singleChannel2PinModeLowOrxSel != ADI_ADRV9025_SINGLE_CH_PIN_MODE_ORX3_FE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.singleChannel2PinModeLowOrxSel != ADI_ADRV9025_SINGLE_CH_PIN_MODE_ORX4_FE))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             radioCtrlCfg->orxRadioCtrlModeCfg.singleChannel2PinModeLowOrxSel,
                             "Single channel 2 Pin ORx enable mode selected but pin level low ORx select is out of range. Valid ORx selects include ORx1, ORx2, ORx3, ORx4");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode == ADI_ADRV9025_ORX_EN_SINGLE_CH_2PIN_MODE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.singleChannel2PinModeHighOrxSel != ADI_ADRV9025_SINGLE_CH_PIN_MODE_ORX1_FE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.singleChannel2PinModeHighOrxSel != ADI_ADRV9025_SINGLE_CH_PIN_MODE_ORX2_FE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.singleChannel2PinModeHighOrxSel != ADI_ADRV9025_SINGLE_CH_PIN_MODE_ORX3_FE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.singleChannel2PinModeHighOrxSel != ADI_ADRV9025_SINGLE_CH_PIN_MODE_ORX4_FE))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             radioCtrlCfg->orxRadioCtrlModeCfg.singleChannel2PinModeHighOrxSel,
                             "Single channel 2 Pin ORx enable mode selected but pin level high ORx select is out of range. Valid ORx selects include ORx1, ORx2, ORx3, ORx4");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode == ADI_ADRV9025_ORX_EN_DUAL_CH_2PIN_MODE) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.dualChannel2PinModeOrxSel != ADI_ADRV9025_DUAL_CH_PIN_MODE_ORX1_ORX3_SEL) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.dualChannel2PinModeOrxSel != ADI_ADRV9025_DUAL_CH_PIN_MODE_ORX1_ORX4_SEL) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.dualChannel2PinModeOrxSel != ADI_ADRV9025_DUAL_CH_PIN_MODE_ORX2_ORX3_SEL) &&
            (radioCtrlCfg->orxRadioCtrlModeCfg.dualChannel2PinModeOrxSel != ADI_ADRV9025_DUAL_CH_PIN_MODE_ORX2_ORX4_SEL))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             radioCtrlCfg->orxRadioCtrlModeCfg.singleChannel2PinModeHighOrxSel,
                             "Dual channel 2 Pin ORx enable mode selected but ORx select is out of range. Valid ORx combinations include {ORx1,ORx3}, {ORx1,ORx4}, {ORx2,ORx3}, {ORx2,ORx4}");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RadioCtrlCfgSet(adi_adrv9025_Device_t*           device,
                                     adi_adrv9025_RadioCtrlModeCfg_t* radioCtrlCfg)
{
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        radioCtrlCfg);

#if ADI_ADRV9025_RADIOCTRL_RANGE_CHECK > 0
    adi_adrv9025_RadioCtrlCfgSetRangeCheck(device,
                                           radioCtrlCfg);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    if (((device->devStateInfo.profilesValid & ADI_ADRV9025_RX_PROFILE_VALID) == ADI_ADRV9025_RX_PROFILE_VALID) &&
        (radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode != ADI_ADRV9025_RX_EN_INVALID_MODE) &&
        (radioCtrlCfg->rxRadioCtrlModeCfg.rxChannelMask != ADI_ADRV9025_RXOFF))
    {
        adrv9025_RxRadioCtrlCfgSet(device,
                                   &radioCtrlCfg->rxRadioCtrlModeCfg);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (((device->devStateInfo.profilesValid & ADI_ADRV9025_TX_PROFILE_VALID) == ADI_ADRV9025_TX_PROFILE_VALID) &&
        (radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode != ADI_ADRV9025_TX_EN_INVALID_MODE) &&
        (radioCtrlCfg->txRadioCtrlModeCfg.txChannelMask != ADI_ADRV9025_TXOFF))
    {
        adrv9025_TxRadioCtrlCfgSet(device,
                                   &radioCtrlCfg->txRadioCtrlModeCfg);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (((device->devStateInfo.profilesValid & ADI_ADRV9025_ORX_PROFILE_VALID) == ADI_ADRV9025_ORX_PROFILE_VALID) &&
        (radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV9025_ORX_EN_INVALID_MODE))
    {
        adrv9025_OrxRadioCtrlCfgSet(device,
                                    &radioCtrlCfg->orxRadioCtrlModeCfg);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RadioCtrlCfgGet(adi_adrv9025_Device_t*           device,
                                     adi_adrv9025_RxChannels_e        rxChannel,
                                     adi_adrv9025_TxChannels_e        txChannel,
                                     adi_adrv9025_RadioCtrlModeCfg_t* radioCtrlCfg)
{
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        radioCtrlCfg);

    adrv9025_RxRadioCtrlCfgGet(device,
                               rxChannel,
                               &radioCtrlCfg->rxRadioCtrlModeCfg);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_TxRadioCtrlCfgGet(device,
                               txChannel,
                               &radioCtrlCfg->txRadioCtrlModeCfg);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_OrxRadioCtrlCfgGet(device,
                                &radioCtrlCfg->orxRadioCtrlModeCfg);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RadioStateGet(adi_adrv9025_Device_t* device,
                                   uint32_t*              radioStatus)
{
	ADI_NULL_DEVICE_PTR_RETURN(device);

	ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

	ADI_NULL_PTR_RETURN(&device->common, radioStatus);

	UNUSED_PARA(device);
    UNUSED_PARA(radioStatus);
    return 0;
}

int32_t adi_adrv9025_RxTxEnableSetRangeCheck(adi_adrv9025_Device_t* device,
                                             uint32_t               rxChannelMask,
                                             uint32_t               txChannelMask)
{
    /*TODO : update this static const once Tx channels have been numbered from Tx1 - Tx4*/
    static const uint32_t ALL_TX_MASK  = (ADI_ADRV9025_TX1 | ADI_ADRV9025_TX2 | ADI_ADRV9025_TX3 | ADI_ADRV9025_TX4);
    static const uint32_t ALL_RX_MASK  = (ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4);
    static const uint32_t ALL_ORX_MASK = (ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2 | ADI_ADRV9025_ORX3 | ADI_ADRV9025_ORX4);

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /*Check that ARM and Stream processors have been loaded before enabling*/
    if (device->devStateInfo.devState < ADI_ADRV9025_STATE_CPULOADED)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannelMask,
                         "Channel Enable/Disable is valid only after ARM and stream processors have been loaded");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that Rx/ORx channel mask is valid*/
    if (rxChannelMask > (ALL_RX_MASK | ALL_ORX_MASK))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannelMask,
                         "Invalid Rx/ORx Channel mask encountered while attempting to enable Rx/ORx signal chain in SPI mode. Valid masks are Rx1-Rx4, ORx1 - ORx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that Tx channel mask is valid*/
    if (txChannelMask > ALL_TX_MASK)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannelMask,
                         "Invalid Tx Channel mask encountered while attempting to enable Tx signal chain in SPI mode. Valid masks are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that requested Rx/ORx channels are initialized*/
    if ((rxChannelMask & device->devStateInfo.initializedChannels) != rxChannelMask)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannelMask,
                         "Requested Rx/ORx channels to enable are not initialized");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that requested Tx channels are initialized*/
    if ((txChannelMask & (device->devStateInfo.initializedChannels >> ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET)) != txChannelMask)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannelMask,
                         "Requested Tx channels to enable are not initialized");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxTxEnableSet(adi_adrv9025_Device_t* device,
                                   uint32_t               rxChannelMask,
                                   uint32_t               txChannelMask)
{
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

#if ADI_ADRV9025_RADIOCTRL_RANGE_CHECK > 0
    adi_adrv9025_RxTxEnableSetRangeCheck(device,
                                         rxChannelMask,
                                         txChannelMask);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /*Enable requested Rx Channel signal chains*/
    adrv9025_RxEnableSet(device,
                         rxChannelMask);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Enable requested ORx Channel signal chains*/
    adrv9025_OrxEnableSet(device,
                          rxChannelMask);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Enable requested Tx Channel signal chains*/
    adrv9025_TxEnableSet(device,
                         txChannelMask);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxTxEnableGet(adi_adrv9025_Device_t* device,
                                   uint32_t*              rxChannelMask,
                                   uint32_t*              txChannelMask)
{
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        rxChannelMask);

    ADI_NULL_PTR_RETURN(&device->common,
                        txChannelMask);

    adrv9025_RxEnableGet(device,
                         rxChannelMask);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_OrxEnableGet(device,
                          rxChannelMask);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_TxEnableGet(device,
                         txChannelMask);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxToOrxMappingSet(adi_adrv9025_Device_t*    device,
                                       adi_adrv9025_RxChannels_e orxChannel,
                                       adi_adrv9025_TxChannels_e txChannel)
{
    uint8_t TXMAP_ORX1_EN_RISE_STREAM_ID  = 0;
    uint8_t TXMAP_ORX1_EN_FALL_STREAM_ID  = 0;
    uint8_t TXMAP_ORX1_SEL_RISE_STREAM_ID = 0;
    uint8_t TXMAP_ORX1_SEL_FALL_STREAM_ID = 0;
    uint8_t TXMAP_ORX2_EN_RISE_STREAM_ID  = 0;
    uint8_t TXMAP_ORX2_EN_FALL_STREAM_ID  = 0;
    uint8_t TXMAP_ORX2_SEL_RISE_STREAM_ID = 0;
    uint8_t TXMAP_ORX2_SEL_FALL_STREAM_ID = 0;
    uint8_t TXMAP_ORX3_EN_RISE_STREAM_ID  = 0;
    uint8_t TXMAP_ORX3_EN_FALL_STREAM_ID  = 0;
    uint8_t TXMAP_ORX3_SEL_RISE_STREAM_ID = 0;
    uint8_t TXMAP_ORX3_SEL_FALL_STREAM_ID = 0;
    uint8_t TXMAP_ORX4_EN_RISE_STREAM_ID  = 0;
    uint8_t TXMAP_ORX4_EN_FALL_STREAM_ID  = 0;
    uint8_t TXMAP_ORX4_SEL_RISE_STREAM_ID = 0;
    uint8_t TXMAP_ORX4_SEL_FALL_STREAM_ID = 0;

    TXMAP_ORX1_EN_RISE_STREAM_ID  = 0x80;
    TXMAP_ORX1_EN_FALL_STREAM_ID  = 0x82;
    TXMAP_ORX1_SEL_RISE_STREAM_ID = 0x81;
    TXMAP_ORX1_SEL_FALL_STREAM_ID = 0x83;
    TXMAP_ORX2_EN_RISE_STREAM_ID  = 0x88;
    TXMAP_ORX2_EN_FALL_STREAM_ID  = 0x8A;
    TXMAP_ORX2_SEL_RISE_STREAM_ID = 0x89;
    TXMAP_ORX2_SEL_FALL_STREAM_ID = 0x8B;
    TXMAP_ORX3_EN_RISE_STREAM_ID  = 0x84;
    TXMAP_ORX3_EN_FALL_STREAM_ID  = 0x86;
    TXMAP_ORX3_SEL_RISE_STREAM_ID = 0x85;
    TXMAP_ORX3_SEL_FALL_STREAM_ID = 0x87;
    TXMAP_ORX4_EN_RISE_STREAM_ID  = 0x8C;
    TXMAP_ORX4_EN_FALL_STREAM_ID  = 0x8E;
    TXMAP_ORX4_SEL_RISE_STREAM_ID = 0x8D;
    TXMAP_ORX4_SEL_FALL_STREAM_ID = 0x8F;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

#if ADI_ADRV9025_RADIOCTRL_RANGE_CHECK > 0
    adrv9025_TxToOrxMappingSetRangeCheck(device,
                                         orxChannel,
                                         txChannel);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    switch (orxChannel)
    {
    case(ADI_ADRV9025_ORX1):
    {
        if (txChannel == ADI_ADRV9025_TXOFF)
        {
            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX1_EN_FALL_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if (txChannel == ADI_ADRV9025_TX1)
        {
            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX1_EN_RISE_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX1_SEL_FALL_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if (txChannel == ADI_ADRV9025_TX2)
        {
            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX1_EN_RISE_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX1_SEL_RISE_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             txChannel,
                             "Invalid Tx channel selected for Tx-ORx1 mapping. Valid Tx channels are Tx1, Tx2, TxOFF(disable)");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        break;
    }
    case(ADI_ADRV9025_ORX2):
    {
        if (txChannel == ADI_ADRV9025_TXOFF)
        {
            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX2_EN_FALL_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if (txChannel == ADI_ADRV9025_TX1)
        {
            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX2_EN_RISE_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX2_SEL_FALL_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if (txChannel == ADI_ADRV9025_TX2)
        {
            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX2_EN_RISE_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX2_SEL_RISE_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             txChannel,
                             "Invalid Tx channel selected for Tx-ORx2 mapping. Valid Tx channels are Tx1, Tx2, TxOFF(disable)");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        break;
    }
    case(ADI_ADRV9025_ORX3):
    {
        if (txChannel == ADI_ADRV9025_TXOFF)
        {
            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX3_EN_FALL_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if (txChannel == ADI_ADRV9025_TX3)
        {
            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX3_EN_RISE_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX3_SEL_FALL_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if (txChannel == ADI_ADRV9025_TX4)
        {
            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX3_EN_RISE_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX3_SEL_RISE_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             txChannel,
                             "Invalid Tx channel selected for Tx-ORx3 mapping. Valid Tx channels are Tx3, Tx4, TxOFF(disable)");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        break;
    }
    case(ADI_ADRV9025_ORX4):
    {
        if (txChannel == ADI_ADRV9025_TXOFF)
        {
            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX4_EN_FALL_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if (txChannel == ADI_ADRV9025_TX3)
        {
            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX4_EN_RISE_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX4_SEL_FALL_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if (txChannel == ADI_ADRV9025_TX4)
        {
            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX4_EN_RISE_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_StreamTrigger(device,
                                   TXMAP_ORX4_SEL_RISE_STREAM_ID);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             txChannel,
                             "Invalid Tx channel selected for Tx-ORx3 mapping. Valid Tx channels are Tx3, Tx4, TxOFF(disable)");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        break;
    }

    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         orxChannel,
                         "Invalid ORx channel selected for Tx to ORx mapping. Valid ORx channels are ORx1-ORx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxToOrxMappingGet(adi_adrv9025_Device_t*     device,
                                       adi_adrv9025_RxChannels_e  orxChannel,
                                       adi_adrv9025_TxChannels_e* txChannel)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;
    static const uint8_t ARM_CMD_ERROR_MASK    = 0x0E;

    int32_t  recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    uint16_t encodedTxToOrxMapping = 0;
    uint8_t  armData[2]            = {0};
    uint8_t  extData[1]            = {0};
    uint8_t  cmdStatusByte         = 0;
    int32_t  errHdl                = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txChannel);

    /* Executing the GET Tx-ORx mapping command */
    extData[0] = ADRV9025_CPU_OBJECTID_TX_ORX_MAPPING;
    adi_adrv9025_CpuCmdWrite(device,
                             ADI_ADRV9025_CPU_TYPE_C,
                             (uint8_t)ADRV9025_CPU_GET_OPCODE,
                             &extData[0],
                             sizeof(extData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_GETTXTOORXMAP_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETTXTOORXMAP_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_CMD_ERROR_MASK) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    errHdl,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                             extData[0],
                                                             cmdStatusByte),
                                    recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read the encoded Tx-ORx mapping from ARM mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            sizeof(armData),
                            ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Decode Tx-ORx mapping */
    encodedTxToOrxMapping = (uint16_t)(((uint16_t)armData[1] << 8) | (uint16_t)armData[0]);
    recoveryAction        = adrv9025_OrxMapDecode(device,
                                                  encodedTxToOrxMapping,
                                                  orxChannel,
                                                  txChannel);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_StreamGpioConfigSet(adi_adrv9025_Device_t*           device,
                                         adi_adrv9025_StreamGpioPinCfg_t* streamGpioPinCfg)
{
    uint8_t signalId = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        streamGpioPinCfg);

#if ADI_ADRV9025_RADIOCTRL_RANGE_CHECK > 0
    adrv9025_StreamGpioConfigSetRangeCheck(device,
                                           streamGpioPinCfg);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /* Loop through each pin and call the corresponding set function */
    signalId = 0;
    adrv9025_StreamGpioPinSet(device,
                              signalId++,
                              streamGpioPinCfg->streamGpInput0);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinSet(device,
                              signalId++,
                              streamGpioPinCfg->streamGpInput1);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinSet(device,
                              signalId++,
                              streamGpioPinCfg->streamGpInput2);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinSet(device,
                              signalId++,
                              streamGpioPinCfg->streamGpInput3);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinSet(device,
                              signalId++,
                              streamGpioPinCfg->streamGpInput4);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinSet(device,
                              signalId++,
                              streamGpioPinCfg->streamGpInput5);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinSet(device,
                              signalId++,
                              streamGpioPinCfg->streamGpInput6);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinSet(device,
                              signalId++,
                              streamGpioPinCfg->streamGpInput7);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinSet(device,
                              signalId++,
                              streamGpioPinCfg->streamGpInput8);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinSet(device,
                              signalId++,
                              streamGpioPinCfg->streamGpInput9);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinSet(device,
                              signalId++,
                              streamGpioPinCfg->streamGpInput10);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinSet(device,
                              signalId++,
                              streamGpioPinCfg->streamGpInput11);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinSet(device,
                              signalId++,
                              streamGpioPinCfg->streamGpInput12);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinSet(device,
                              signalId++,
                              streamGpioPinCfg->streamGpInput13);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinSet(device,
                              signalId++,
                              streamGpioPinCfg->streamGpInput14);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinSet(device,
                              signalId++,
                              streamGpioPinCfg->streamGpInput15);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_StreamGpioConfigGet(adi_adrv9025_Device_t*           device,
                                         adi_adrv9025_StreamGpioPinCfg_t* streamGpioPinCfg)
{
    uint8_t signalId = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        streamGpioPinCfg);

    /* Loop through each pin and call the corresponding get function */
    signalId = 0;
    adrv9025_StreamGpioPinGet(device,
                              signalId++,
                              &streamGpioPinCfg->streamGpInput0);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinGet(device,
                              signalId++,
                              &streamGpioPinCfg->streamGpInput1);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinGet(device,
                              signalId++,
                              &streamGpioPinCfg->streamGpInput2);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinGet(device,
                              signalId++,
                              &streamGpioPinCfg->streamGpInput3);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinGet(device,
                              signalId++,
                              &streamGpioPinCfg->streamGpInput4);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinGet(device,
                              signalId++,
                              &streamGpioPinCfg->streamGpInput5);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinGet(device,
                              signalId++,
                              &streamGpioPinCfg->streamGpInput6);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinGet(device,
                              signalId++,
                              &streamGpioPinCfg->streamGpInput7);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinGet(device,
                              signalId++,
                              &streamGpioPinCfg->streamGpInput8);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinGet(device,
                              signalId++,
                              &streamGpioPinCfg->streamGpInput9);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinGet(device,
                              signalId++,
                              &streamGpioPinCfg->streamGpInput10);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinGet(device,
                              signalId++,
                              &streamGpioPinCfg->streamGpInput11);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinGet(device,
                              signalId++,
                              &streamGpioPinCfg->streamGpInput12);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinGet(device,
                              signalId++,
                              &streamGpioPinCfg->streamGpInput13);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinGet(device,
                              signalId++,
                              &streamGpioPinCfg->streamGpInput14);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_StreamGpioPinGet(device,
                              signalId++,
                              &streamGpioPinCfg->streamGpInput15);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PllFrequencySetRangeCheck(adi_adrv9025_Device_t* device,
                                               adi_adrv9025_PllName_e pllName,
                                               uint64_t               pllLoFrequency_Hz)
{
    int32_t                   recoveryAction     = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                   areCalsRunning     = 0;
    uint8_t                   errorFlag          = 0;
    uint32_t                  i                  = 0;
    int64_t                   freqValid          = 0;
    uint64_t                  maxRfBandwidth_kHz = 0;
    adi_adrv9025_LoSel_e      pllLoSelect        = ADI_ADRV9025_LOSEL_LO1;
    adi_adrv9025_LoSel_e      loSel              = ADI_ADRV9025_LOSEL_LO1;
    adi_adrv9025_TxChannels_e txChannel          = ADI_ADRV9025_TXOFF;
    adi_adrv9025_RxChannels_e rxChannel          = ADI_ADRV9025_RXOFF;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /*Check that PLL selected is valid*/
    if ((pllName != ADI_ADRV9025_LO1_PLL) &&
        (pllName != ADI_ADRV9025_LO2_PLL) &&
        (pllName != ADI_ADRV9025_AUX_PLL))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pllName,
                         "Invalid PLL selected for setting PLL frequency");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Determine the PLL Frequency LO Selection */
    if (pllName == ADI_ADRV9025_LO1_PLL)
    {
        pllLoSelect = ADI_ADRV9025_LOSEL_LO1;
    }
    else if (pllName == ADI_ADRV9025_LO2_PLL)
    {
        pllLoSelect = ADI_ADRV9025_LOSEL_LO2;
    }
    else if (pllName == ADI_ADRV9025_AUX_PLL)
    {
        pllLoSelect = ADI_ADRV9025_LOSEL_AUXLO;
    }

    /* Find the greatest TX Input Rates value of the same LO */
    maxRfBandwidth_kHz = 0;
    if ((device->devStateInfo.profilesValid & ADI_ADRV9025_TX_PROFILE_VALID) == ADI_ADRV9025_TX_PROFILE_VALID)
    {
        for (i = ADI_ADRV9025_MAX_TX_CHANNEL_START; i <= ADI_ADRV9025_MAX_TX_CHANNEL_END; i++)
        {
            /* convert the index to matching the channel */
            txChannel = (adi_adrv9025_TxChannels_e)((uint8_t)(1 << i));
            if (((device->devStateInfo.initializedChannels >> ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET) & (uint32_t)txChannel) == (uint32_t)txChannel)
            {
                recoveryAction = adi_adrv9025_TxLoSourceGet(device,
                                                            txChannel,
                                                            &loSel);
                if ((recoveryAction == ADI_COMMON_ACT_NO_ACTION) &&
                    (loSel == pllLoSelect))
                {
                    /* Now check if matching LO then compare to get max value */
                    if (device->devStateInfo.txRfBandWidth_kHz[i] > maxRfBandwidth_kHz)
                    {
                        maxRfBandwidth_kHz = device->devStateInfo.txRfBandWidth_kHz[i];
                    }
                }
            }
        }
    }

    /* Find the greatest RX Output Rates value of the same LO */
    if ((device->devStateInfo.profilesValid & ADI_ADRV9025_RX_PROFILE_VALID) == ADI_ADRV9025_RX_PROFILE_VALID)
    {
        for (i = ADI_ADRV9025_MAX_RX_CHANNEL_START; i <= ADI_ADRV9025_MAX_RX_CHANNEL_END; i++)
        {
            /* convert the index to matching the channel */
            rxChannel = (adi_adrv9025_RxChannels_e)((uint8_t)(1 << i));
            if ((device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == (uint32_t)rxChannel)
            {
                recoveryAction = adi_adrv9025_RxLoSourceGet(device,
                                                            rxChannel,
                                                            &loSel);
                if ((recoveryAction == ADI_COMMON_ACT_NO_ACTION) &&
                    (loSel == pllLoSelect))
                {
                    /* Now check if matching LO then compare to get max value */
                    if (device->devStateInfo.rxRfBandWidth_kHz[i] > maxRfBandwidth_kHz)
                    {
                        maxRfBandwidth_kHz = device->devStateInfo.rxRfBandWidth_kHz[i];
                    }
                }
            }
        }
    }

    /* Find the greatest ORX Output Rates value of the same LO */
    if ((device->devStateInfo.profilesValid & ADI_ADRV9025_ORX_PROFILE_VALID) == ADI_ADRV9025_ORX_PROFILE_VALID)
    {
        for (i = ADI_ADRV9025_MAX_ORX_CHANNEL_START; i <= ADI_ADRV9025_MAX_ORX_CHANNEL_END; i++)
        {
            /* convert the index to matching the channel */
            rxChannel = (adi_adrv9025_RxChannels_e)((uint8_t)(1 << i));
            if ((device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == (uint32_t)rxChannel)
            {
                recoveryAction = adi_adrv9025_RxLoSourceGet(device,
                                                            rxChannel,
                                                            &loSel);
                if ((recoveryAction == ADI_COMMON_ACT_NO_ACTION) &&
                    (loSel == pllLoSelect))
                {
                    /* Now check if matching LO then compare to get max value */
                    if (device->devStateInfo.rxRfBandWidth_kHz[i] > maxRfBandwidth_kHz)
                    {
                        maxRfBandwidth_kHz = device->devStateInfo.rxRfBandWidth_kHz[i];
                    }
                }
            }
        }
    }

    /* The rule is that the PLL LO frequency must always more than half of the greatest bandwidth. */
    freqValid = (int64_t)pllLoFrequency_Hz - (int64_t)((maxRfBandwidth_kHz * 1000) >> 1);
    if ((freqValid <= 0) && (maxRfBandwidth_kHz > 0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pllLoFrequency_Hz,
                         "(pllFreq - Bandwidth/2) should be greater than DC.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that init cals are not running*/
    adi_adrv9025_InitCalsCheckCompleteGet(device,
                                          &areCalsRunning,
                                          &errorFlag);
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (areCalsRunning == 1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pllLoFrequency_Hz,
                         "Can not set PLL frequency while InitCals are running");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PllFrequencySetRangeCheck_v2(adi_adrv9025_Device_t*    device,
                                                  adi_adrv9025_PllConfig_t* pllConfig)
{
    int32_t                   recoveryAction     = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                   areCalsRunning     = 0;
    uint8_t                   errorFlag          = 0;
    uint32_t                  i                  = 0;
    int64_t                   freqValid          = 0;
    uint64_t                  maxRfBandwidth_kHz = 0;
    adi_adrv9025_LoSel_e      pllLoSelect        = ADI_ADRV9025_LOSEL_LO1;
    adi_adrv9025_LoSel_e      loSel              = ADI_ADRV9025_LOSEL_LO1;
    adi_adrv9025_TxChannels_e txChannel          = ADI_ADRV9025_TXOFF;
    adi_adrv9025_RxChannels_e rxChannel          = ADI_ADRV9025_RXOFF;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        pllConfig);

    /*Check that PLL selected is valid*/
    if ((pllConfig->pllName != ADI_ADRV9025_LO1_PLL) &&
        (pllConfig->pllName != ADI_ADRV9025_LO2_PLL) &&
        (pllConfig->pllName != ADI_ADRV9025_AUX_PLL))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pllConfig->pllName,
                         "Invalid PLL selected for setting PLL frequency");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that PLL mode is valid*/
    if ((pllConfig->pllModeSel != ADI_ADRV9025_PLL_FAST_MODE) &&
        (pllConfig->pllModeSel != ADI_ADRV9025_PLL_SLOW_MODE))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pllConfig->pllModeSel,
                         "Invalid PLL mode selected for setting PLL frequency");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Determine the PLL Frequency LO Selection */
    if (pllConfig->pllName == ADI_ADRV9025_LO1_PLL)
    {
        pllLoSelect = ADI_ADRV9025_LOSEL_LO1;
    }
    else if (pllConfig->pllName == ADI_ADRV9025_LO2_PLL)
    {
        pllLoSelect = ADI_ADRV9025_LOSEL_LO2;
    }
    else if (pllConfig->pllName == ADI_ADRV9025_AUX_PLL)
    {
        pllLoSelect = ADI_ADRV9025_LOSEL_AUXLO;
    }

    /* Find the greatest TX Input Rates value of the same LO */
    maxRfBandwidth_kHz = 0;
    if ((device->devStateInfo.profilesValid & ADI_ADRV9025_TX_PROFILE_VALID) == ADI_ADRV9025_TX_PROFILE_VALID)
    {
        for (i = ADI_ADRV9025_MAX_TX_CHANNEL_START; i <= ADI_ADRV9025_MAX_TX_CHANNEL_END; i++)
        {
            /* convert the index to matching the channel */
            txChannel = (adi_adrv9025_TxChannels_e)((uint8_t)(1 << i));
            if (((device->devStateInfo.initializedChannels >> ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET) & (uint32_t)txChannel) == (uint32_t)txChannel)
            {
                recoveryAction = adi_adrv9025_TxLoSourceGet(device,
                                                            txChannel,
                                                            &loSel);
                if ((recoveryAction == ADI_COMMON_ACT_NO_ACTION) &&
                    (loSel == pllLoSelect))
                {
                    /* Now check if matching LO then compare to get max value */
                    if (device->devStateInfo.txRfBandWidth_kHz[i] > maxRfBandwidth_kHz)
                    {
                        maxRfBandwidth_kHz = device->devStateInfo.txRfBandWidth_kHz[i];
                    }
                }
            }
        }
    }

    /* Find the greatest RX Output Rates value of the same LO */
    if ((device->devStateInfo.profilesValid & ADI_ADRV9025_RX_PROFILE_VALID) == ADI_ADRV9025_RX_PROFILE_VALID)
    {
        for (i = ADI_ADRV9025_MAX_RX_CHANNEL_START; i <= ADI_ADRV9025_MAX_RX_CHANNEL_END; i++)
        {
            /* convert the index to matching the channel */
            rxChannel = (adi_adrv9025_RxChannels_e)((uint8_t)(1 << i));
            if ((device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == (uint32_t)rxChannel)
            {
                recoveryAction = adi_adrv9025_RxLoSourceGet(device,
                                                            rxChannel,
                                                            &loSel);
                if ((recoveryAction == ADI_COMMON_ACT_NO_ACTION) &&
                    (loSel == pllLoSelect))
                {
                    /* Now check if matching LO then compare to get max value */
                    if (device->devStateInfo.rxRfBandWidth_kHz[i] > maxRfBandwidth_kHz)
                    {
                        maxRfBandwidth_kHz = device->devStateInfo.rxRfBandWidth_kHz[i];
                    }
                }
            }
        }
    }

    /* Find the greatest ORX Output Rates value of the same LO */
    if ((device->devStateInfo.profilesValid & ADI_ADRV9025_ORX_PROFILE_VALID) == ADI_ADRV9025_ORX_PROFILE_VALID)
    {
        for (i = ADI_ADRV9025_MAX_ORX_CHANNEL_START; i <= ADI_ADRV9025_MAX_ORX_CHANNEL_END; i++)
        {
            /* convert the index to matching the channel */
            rxChannel = (adi_adrv9025_RxChannels_e)((uint8_t)(1 << i));
            if ((device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == (uint32_t)rxChannel)
            {
                recoveryAction = adi_adrv9025_RxLoSourceGet(device,
                                                            rxChannel,
                                                            &loSel);
                if ((recoveryAction == ADI_COMMON_ACT_NO_ACTION) &&
                    (loSel == pllLoSelect))
                {
                    /* Now check if matching LO then compare to get max value */
                    if (device->devStateInfo.rxRfBandWidth_kHz[i] > maxRfBandwidth_kHz)
                    {
                        maxRfBandwidth_kHz = device->devStateInfo.rxRfBandWidth_kHz[i];
                    }
                }
            }
        }
    }

    /* The rule is that the PLL LO frequency must always more than half of the greatest bandwidth. */
    freqValid = (int64_t)pllConfig->pllLoFrequency_Hz - (int64_t)((maxRfBandwidth_kHz * 1000) >> 1);
    if ((freqValid <= 0) && (maxRfBandwidth_kHz > 0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pllConfig->pllLoFrequency_Hz,
                         "(pllFreq - Bandwidth/2) should be greater than DC.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that init cals are not running*/
    adi_adrv9025_InitCalsCheckCompleteGet(device,
                                          &areCalsRunning,
                                          &errorFlag);
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (areCalsRunning == 1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pllConfig->pllLoFrequency_Hz,
                         "Can not set PLL frequency while InitCals are running");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PllFrequencySet(adi_adrv9025_Device_t* device,
                                     adi_adrv9025_PllName_e pllName,
                                     uint64_t               pllLoFrequency_Hz)
{
    static const uint8_t PLL_GP_INTERRUPT_PIN0_MASK = 1;
    static const uint8_t PLL_GP_INTERRUPT_PIN1_MASK = 1;

    int32_t recoveryAction                     = ADI_COMMON_ACT_NO_ACTION;
    uint8_t armData[8]                         = {0};
    uint8_t extData[2]                         = {0};
    uint8_t cmdStatusByte                      = 0;
    int32_t errHdl                             = 0;
    uint8_t pllUnlockGpInterruptPin0Mask       = 0;
    uint8_t pllUnlockGpInterruptPin1Mask       = 0;
    uint8_t auxPllGpInterruptPin0Mask          = 0;
    uint8_t auxPllGpInterruptPin1Mask          = 0;
    uint8_t pll1OverrangeGpInterruptPin0Mask   = 0;
    uint8_t pll1OverrangeGpInterruptPin1Mask   = 0;
    uint8_t pll2OverrangeGpInterruptPin0Mask   = 0;
    uint8_t pll2OverrangeGpInterruptPin1Mask   = 0;
    uint8_t auxPllOverrangeGpInterruptPin0Mask = 0;
    uint8_t auxPllOverrangeGpInterruptPin1Mask = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

#if ADI_ADRV9025_RADIOCTRL_RANGE_CHECK > 0
    adi_adrv9025_PllFrequencySetRangeCheck(device,
                                           pllName,
                                           pllLoFrequency_Hz);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /*Store the current PLL unlock GP Interrupt status for the requested PLL*/
    adrv9025_PllUnlockGpInterruptMaskGet(device,
                                         pllName,
                                         &pllUnlockGpInterruptPin0Mask,
                                         &pllUnlockGpInterruptPin1Mask);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Mask the PLL unlock interrupt*/
    adrv9025_PllUnlockGpInterruptMaskSet(device,
                                         pllName,
                                         PLL_GP_INTERRUPT_PIN0_MASK,
                                         PLL_GP_INTERRUPT_PIN1_MASK);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Mask the Aux PLL GP Interrupt if RFLO1 or RFLO2 PLLs are targeted */
    if (pllName != ADI_ADRV9025_AUX_PLL)
    {
        /* Store the current Aux PLL unlock GP Interrupt status for the requested PLL */
        adrv9025_PllUnlockGpInterruptMaskGet(device,
                                             ADI_ADRV9025_AUX_PLL,
                                             &auxPllGpInterruptPin0Mask,
                                             &auxPllGpInterruptPin1Mask);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Mask the Aux PLL unlock interrupt */
        adrv9025_PllUnlockGpInterruptMaskSet(device,
                                             ADI_ADRV9025_AUX_PLL,
                                             PLL_GP_INTERRUPT_PIN0_MASK,
                                             PLL_GP_INTERRUPT_PIN1_MASK);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Store the current PLL overrange GP Interrupt status for RF PLL1, RF PLL2 and the Aux PLL */
    adrv9025_PllOverrangeGpInterruptMaskGet(device,
                                            ADI_ADRV9025_LO1_PLL,
                                            &pll1OverrangeGpInterruptPin0Mask,
                                            &pll1OverrangeGpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_PllOverrangeGpInterruptMaskGet(device,
                                            ADI_ADRV9025_LO2_PLL,
                                            &pll2OverrangeGpInterruptPin0Mask,
                                            &pll2OverrangeGpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_PllOverrangeGpInterruptMaskGet(device,
                                            ADI_ADRV9025_AUX_PLL,
                                            &auxPllOverrangeGpInterruptPin0Mask,
                                            &auxPllOverrangeGpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Mask the PLL overrange interrupts */
    adrv9025_PllOverrangeGpInterruptMaskSet(device,
                                            ADI_ADRV9025_LO1_PLL,
                                            PLL_GP_INTERRUPT_PIN0_MASK,
                                            PLL_GP_INTERRUPT_PIN1_MASK);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_PllOverrangeGpInterruptMaskSet(device,
                                            ADI_ADRV9025_LO2_PLL,
                                            PLL_GP_INTERRUPT_PIN0_MASK,
                                            PLL_GP_INTERRUPT_PIN1_MASK);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_PllOverrangeGpInterruptMaskSet(device,
                                            ADI_ADRV9025_AUX_PLL,
                                            PLL_GP_INTERRUPT_PIN0_MASK,
                                            PLL_GP_INTERRUPT_PIN1_MASK);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Loading byte array with parsed bytes from pllLoFrequency_Hz word */
    armData[0] = (uint8_t)(pllLoFrequency_Hz & 0xFF);
    armData[1] = (uint8_t)((pllLoFrequency_Hz >> 8) & 0xFF);
    armData[2] = (uint8_t)((pllLoFrequency_Hz >> 16) & 0xFF);
    armData[3] = (uint8_t)((pllLoFrequency_Hz >> 24) & 0xFF);
    armData[4] = (uint8_t)((pllLoFrequency_Hz >> 32) & 0xFF);
    armData[5] = (uint8_t)((pllLoFrequency_Hz >> 40) & 0xFF);
    armData[6] = (uint8_t)((pllLoFrequency_Hz >> 48) & 0xFF);
    armData[7] = (uint8_t)((pllLoFrequency_Hz >> 56) & 0xFF);

    /* Write PLL Frequency to ARM mailbox */
    adi_adrv9025_CpuMemWrite(device,
                             (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                             &armData[0],
                             sizeof(armData),
                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Executing the SET PLL Freq command */
    extData[0] = ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY;
    extData[1] = (uint8_t)pllName;
    adi_adrv9025_CpuCmdWrite(device,
                             ADI_ADRV9025_CPU_TYPE_C,
                             (uint8_t)ADRV9025_CPU_SET_OPCODE,
                             &extData[0],
                             sizeof(extData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_SETRFPLL_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_SETRFPLL_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte >> 1) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    errHdl,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                             extData[0],
                                                             cmdStatusByte),
                                    recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Restore the current PLL overrange GP Interrupt status for RF PLL1, RF PLL2 and the Aux PLL */
    adrv9025_PllOverrangeGpInterruptMaskSet(device,
                                            ADI_ADRV9025_LO1_PLL,
                                            pll1OverrangeGpInterruptPin0Mask,
                                            pll1OverrangeGpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_PllOverrangeGpInterruptMaskSet(device,
                                            ADI_ADRV9025_LO2_PLL,
                                            pll2OverrangeGpInterruptPin0Mask,
                                            pll2OverrangeGpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_PllOverrangeGpInterruptMaskSet(device,
                                            ADI_ADRV9025_AUX_PLL,
                                            auxPllOverrangeGpInterruptPin0Mask,
                                            auxPllOverrangeGpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Restore PLL unlock GP Interrupt Status for the requested PLL*/
    adrv9025_PllUnlockGpInterruptMaskSet(device,
                                         pllName,
                                         pllUnlockGpInterruptPin0Mask,
                                         pllUnlockGpInterruptPin1Mask);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Restore Aux PLL unlock GP Interrupt Status if RFLO1 or RFLO2 are targeted*/
    if (pllName != ADI_ADRV9025_AUX_PLL)
    {
        adrv9025_PllUnlockGpInterruptMaskSet(device,
                                             ADI_ADRV9025_AUX_PLL,
                                             auxPllGpInterruptPin0Mask,
                                             auxPllGpInterruptPin1Mask);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PllFrequencySet_v2(adi_adrv9025_Device_t*    device,
                                        adi_adrv9025_PllConfig_t* pllConfig)
{
    static const uint8_t PLL_GP_INTERRUPT_PIN0_MASK = 1;
    static const uint8_t PLL_GP_INTERRUPT_PIN1_MASK = 1;
    static const uint8_t ARM_ERROR_CODE             = 0x0E;

    int32_t recoveryAction                     = ADI_COMMON_ACT_NO_ACTION;
    uint8_t armData[8]                         = {0};
    uint8_t extData[2]                         = {0};
    uint8_t cmdStatusByte                      = 0;
    int32_t errHdl                             = 0;
    uint8_t pllUnlockGpInterruptPin0Mask       = 0;
    uint8_t pllUnlockGpInterruptPin1Mask       = 0;
    uint8_t auxPllGpInterruptPin0Mask          = 0;
    uint8_t auxPllGpInterruptPin1Mask          = 0;
    uint8_t pll1OverrangeGpInterruptPin0Mask   = 0;
    uint8_t pll1OverrangeGpInterruptPin1Mask   = 0;
    uint8_t pll2OverrangeGpInterruptPin0Mask   = 0;
    uint8_t pll2OverrangeGpInterruptPin1Mask   = 0;
    uint8_t auxPllOverrangeGpInterruptPin0Mask = 0;
    uint8_t auxPllOverrangeGpInterruptPin1Mask = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        pllConfig);

#if ADI_ADRV9025_RADIOCTRL_RANGE_CHECK > 0
    adi_adrv9025_PllFrequencySetRangeCheck_v2(device,
                                              pllConfig);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /*Store the current PLL unlock GP Interrupt status for the requested PLL*/
    adrv9025_PllUnlockGpInterruptMaskGet(device,
                                         pllConfig->pllName,
                                         &pllUnlockGpInterruptPin0Mask,
                                         &pllUnlockGpInterruptPin1Mask);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Mask the PLL unlock interrupt*/
    adrv9025_PllUnlockGpInterruptMaskSet(device,
                                         pllConfig->pllName,
                                         PLL_GP_INTERRUPT_PIN0_MASK,
                                         PLL_GP_INTERRUPT_PIN1_MASK);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Mask the Aux PLL GP Interrupt if RFLO1 or RFLO2 PLLs are targeted */
    if (pllConfig->pllName != ADI_ADRV9025_AUX_PLL)
    {
        /* Store the current Aux PLL unlock GP Interrupt status for the requested PLL */
        adrv9025_PllUnlockGpInterruptMaskGet(device,
                                             ADI_ADRV9025_AUX_PLL,
                                             &auxPllGpInterruptPin0Mask,
                                             &auxPllGpInterruptPin1Mask);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Mask the Aux PLL unlock interrupt */
        adrv9025_PllUnlockGpInterruptMaskSet(device,
                                             ADI_ADRV9025_AUX_PLL,
                                             PLL_GP_INTERRUPT_PIN0_MASK,
                                             PLL_GP_INTERRUPT_PIN1_MASK);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Store the current PLL overrange GP Interrupt status for RF PLL1, RF PLL2 and the Aux PLL */
    adrv9025_PllOverrangeGpInterruptMaskGet(device,
                                            ADI_ADRV9025_LO1_PLL,
                                            &pll1OverrangeGpInterruptPin0Mask,
                                            &pll1OverrangeGpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_PllOverrangeGpInterruptMaskGet(device,
                                            ADI_ADRV9025_LO2_PLL,
                                            &pll2OverrangeGpInterruptPin0Mask,
                                            &pll2OverrangeGpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_PllOverrangeGpInterruptMaskGet(device,
                                            ADI_ADRV9025_AUX_PLL,
                                            &auxPllOverrangeGpInterruptPin0Mask,
                                            &auxPllOverrangeGpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Mask the PLL overrange interrupts */
    adrv9025_PllOverrangeGpInterruptMaskSet(device,
                                            ADI_ADRV9025_LO1_PLL,
                                            PLL_GP_INTERRUPT_PIN0_MASK,
                                            PLL_GP_INTERRUPT_PIN1_MASK);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_PllOverrangeGpInterruptMaskSet(device,
                                            ADI_ADRV9025_LO2_PLL,
                                            PLL_GP_INTERRUPT_PIN0_MASK,
                                            PLL_GP_INTERRUPT_PIN1_MASK);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_PllOverrangeGpInterruptMaskSet(device,
                                            ADI_ADRV9025_AUX_PLL,
                                            PLL_GP_INTERRUPT_PIN0_MASK,
                                            PLL_GP_INTERRUPT_PIN1_MASK);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Loading byte array with parsed bytes from pllLoFrequency_Hz word */
    armData[0] = (uint8_t)(pllConfig->pllLoFrequency_Hz & 0xFF);
    armData[1] = (uint8_t)((pllConfig->pllLoFrequency_Hz >> 8) & 0xFF);
    armData[2] = (uint8_t)((pllConfig->pllLoFrequency_Hz >> 16) & 0xFF);
    armData[3] = (uint8_t)((pllConfig->pllLoFrequency_Hz >> 24) & 0xFF);
    armData[4] = (uint8_t)((pllConfig->pllLoFrequency_Hz >> 32) & 0xFF);
    armData[5] = (uint8_t)((pllConfig->pllLoFrequency_Hz >> 40) & 0xFF);
    armData[6] = (uint8_t)((pllConfig->pllLoFrequency_Hz >> 48) & 0xFF);
    armData[7] = (uint8_t)((pllConfig->pllLoFrequency_Hz >> 56) & 0xFF);

    /* Write PLL Frequency to ARM mailbox */
    adi_adrv9025_CpuMemWrite(device,
                             (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                             &armData[0],
                             sizeof(armData),
                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Executing the SET PLL Freq command */
    extData[0] = ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY;
    extData[1] = ((uint8_t)pllConfig->pllModeSel | (uint8_t)pllConfig->pllAuxLoResyncSel | (uint8_t)pllConfig->pllAuxLoOffsetProgSel | (uint8_t)pllConfig->
        pllName);
    adi_adrv9025_CpuCmdWrite(device,
                             ADI_ADRV9025_CPU_TYPE_C,
                             (uint8_t)ADRV9025_CPU_SET_OPCODE,
                             &extData[0],
                             sizeof(extData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_SETRFPLL_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_SETRFPLL_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERROR_CODE) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    errHdl,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                             extData[0],
                                                             cmdStatusByte),
                                    recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Restore the current PLL overrange GP Interrupt status for RF PLL1, RF PLL2 and the Aux PLL */
    adrv9025_PllOverrangeGpInterruptMaskSet(device,
                                            ADI_ADRV9025_LO1_PLL,
                                            pll1OverrangeGpInterruptPin0Mask,
                                            pll1OverrangeGpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_PllOverrangeGpInterruptMaskSet(device,
                                            ADI_ADRV9025_LO2_PLL,
                                            pll2OverrangeGpInterruptPin0Mask,
                                            pll2OverrangeGpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_PllOverrangeGpInterruptMaskSet(device,
                                            ADI_ADRV9025_AUX_PLL,
                                            auxPllOverrangeGpInterruptPin0Mask,
                                            auxPllOverrangeGpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Restore PLL unlock GP Interrupt Status for the requested PLL*/
    adrv9025_PllUnlockGpInterruptMaskSet(device,
                                         pllConfig->pllName,
                                         pllUnlockGpInterruptPin0Mask,
                                         pllUnlockGpInterruptPin1Mask);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Restore Aux PLL unlock GP Interrupt Status if RFLO1 or RFLO2 are targeted*/
    if (pllConfig->pllName != ADI_ADRV9025_AUX_PLL)
    {
        adrv9025_PllUnlockGpInterruptMaskSet(device,
                                             ADI_ADRV9025_AUX_PLL,
                                             auxPllGpInterruptPin0Mask,
                                             auxPllGpInterruptPin1Mask);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PllFrequencyGet(adi_adrv9025_Device_t* device,
                                     adi_adrv9025_PllName_e pllName,
                                     uint64_t*              pllLoFrequency_Hz)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t armData[8]     = {0};
    uint8_t extData[2]     = {0};
    uint8_t cmdStatusByte  = 0;
    int32_t errHdl         = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        pllLoFrequency_Hz);

    /* Executing the GET PLL Freq command */
    extData[0] = ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY;
    extData[1] = (uint8_t)pllName;
    adi_adrv9025_CpuCmdWrite(device,
                             ADI_ADRV9025_CPU_TYPE_C,
                             (uint8_t)ADRV9025_CPU_GET_OPCODE,
                             &extData[0],
                             sizeof(extData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_GETRFPLL_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETRFPLL_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte >> 1) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    errHdl,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                             extData[0],
                                                             cmdStatusByte),
                                    recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read PLL Frequency from ARM mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            sizeof(armData),
                            ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Form pllFrequency word with data read back from ARM mailbox*/
    *pllLoFrequency_Hz = ((uint64_t)(armData[0])) |
    ((uint64_t)(armData[1]) << 8) |
    ((uint64_t)(armData[2]) << 16) |
    ((uint64_t)(armData[3]) << 24) |
    ((uint64_t)(armData[4]) << 32) |
    ((uint64_t)(armData[5]) << 40) |
    ((uint64_t)(armData[6]) << 48) |
    ((uint64_t)(armData[7]) << 56);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PllStatusGet(adi_adrv9025_Device_t* device,
                                  uint32_t*              pllLockStatus)
{
    static const uint8_t  CLK_PLL_LOCK_STATUS_SHIFT = 0;
    static const uint8_t  LO1_PLL_LOCK_STATUS_SHIFT = 1;
    static const uint8_t  LO2_PLL_LOCK_STATUS_SHIFT = 2;
    static const uint8_t  AUX_PLL_LOCK_STATUS_SHIFT = 3;
    static const uint32_t PLL_STATUS_MASK           = 0x0000000F;

    uint8_t pllLockStatusRead = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that pllLockStatus pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        pllLockStatus);

    /* Clear status of all PLLs */
    *pllLockStatus &= ~PLL_STATUS_MASK;

    /* Read CLK Pll status */
    adrv9025_PllMemMapSynLockBfGet(device,
                                   ADRV9025_BF_CLK_PLL,
                                   &pllLockStatusRead);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Update pllLockStatus bit 0 with Clk Pll Status */
    *pllLockStatus |= ((uint32_t)pllLockStatusRead & 0x00000001) << CLK_PLL_LOCK_STATUS_SHIFT;

    /* Read LO1 Pll status */
    adrv9025_PllMemMapSynLockBfGet(device,
                                   ADRV9025_BF_RF1_PLL,
                                   &pllLockStatusRead);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Update pllLockStatus bit 1 with LO1 Pll Status */
    *pllLockStatus |= ((uint32_t)pllLockStatusRead & 0x00000001) << LO1_PLL_LOCK_STATUS_SHIFT;

    /* Read LO2 Pll status */
    adrv9025_PllMemMapSynLockBfGet(device,
                                   ADRV9025_BF_RF2_PLL,
                                   &pllLockStatusRead);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Update pllLockStatus bit 2 with LO2 Pll Status */
    *pllLockStatus |= ((uint32_t)pllLockStatusRead & 0x00000001) << LO2_PLL_LOCK_STATUS_SHIFT;

    /* Read Aux Pll status */
    adrv9025_PllMemMapSynLockBfGet(device,
                                   ADRV9025_BF_AUX_PLL,
                                   &pllLockStatusRead);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Update pllLockStatus bit 3 with Aux Pll Status */
    *pllLockStatus |= ((uint32_t)pllLockStatusRead & 0x00000001) << AUX_PLL_LOCK_STATUS_SHIFT;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PllLoopFilterSetRangeCheck(adi_adrv9025_Device_t*           device,
                                                adi_adrv9025_PllName_e           pllName,
                                                adi_adrv9025_PllLoopFilterCfg_t* pllLoopFilterConfig)
{
    static const uint8_t  MINIMUM_PLL_LOOP_FILTER_PHASE_MARGIN_DEGREES = 40;
    static const uint8_t  MAXIMUM_PLL_LOOP_FILTER_PHASE_MARGIN_DEGREES = 85;
    static const uint16_t MINIMUM_LOOP_FILTER_BANDWIDTH_KHZ            = 50;
    static const uint16_t MAXIMUM_LOOP_FILTER_BANDWIDTH_KHZ            = 1500;
    static const uint8_t  MINIMUM_POWER_SCALE_FACTOR                   = 0;
    static const uint8_t  MAXIMUM_POWER_SCALE_FACTOR                   = 10;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that pllLockStatus pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        pllLoopFilterConfig);

    /*Check that PLL selected is valid*/
    if ((pllName != ADI_ADRV9025_LO1_PLL) &&
        (pllName != ADI_ADRV9025_LO2_PLL) &&
        (pllName != ADI_ADRV9025_AUX_PLL))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pllName,
                         "Invalid PLL selected for PLL Loop Filter setting");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that Loop Filter phase margin is between 40-85 degrees*/
    if ((pllLoopFilterConfig->phaseMargin_degrees < MINIMUM_PLL_LOOP_FILTER_PHASE_MARGIN_DEGREES) ||
        (pllLoopFilterConfig->phaseMargin_degrees > MAXIMUM_PLL_LOOP_FILTER_PHASE_MARGIN_DEGREES))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pllLoopFilterConfig->phaseMargin_degrees,
                         "Phase margin out of range for PLL loop filter setting. Valid range 40-85 degrees");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that loop filter bandwidth is in valid range*/
    if ((pllLoopFilterConfig->loopBandwidth_kHz < MINIMUM_LOOP_FILTER_BANDWIDTH_KHZ) ||
        (pllLoopFilterConfig->loopBandwidth_kHz > MAXIMUM_LOOP_FILTER_BANDWIDTH_KHZ))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pllLoopFilterConfig->loopBandwidth_kHz,
                         "Loop filter bandwidth is out of range for PLL loop filter setting. Valid range 50-1500 KHz.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that power scale factor is between 0-10*/
    if ((pllLoopFilterConfig->powerScale < MINIMUM_POWER_SCALE_FACTOR) ||
        (pllLoopFilterConfig->powerScale > MAXIMUM_POWER_SCALE_FACTOR))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pllLoopFilterConfig->powerScale,
                         "Power scale factor out of range for PLL loop filter setting. Valid range 0-10");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PllLoopFilterSet(adi_adrv9025_Device_t*           device,
                                      adi_adrv9025_PllName_e           pllName,
                                      adi_adrv9025_PllLoopFilterCfg_t* pllLoopFilterConfig)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t armData[4]     = {0};
    uint8_t extData[2]     = {0};
    uint8_t cmdStatusByte  = 0;
    int32_t errHdl         = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that pllLockStatus pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        pllLoopFilterConfig);

#if ADI_ADRV9025_RADIOCTRL_RANGE_CHECK > 0
    adi_adrv9025_PllLoopFilterSetRangeCheck(device,
                                            pllName,
                                            pllLoopFilterConfig);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /* Loading byte array with parsed bytes from pllLoopFilterConfig struct */
    armData[0] = pllLoopFilterConfig->phaseMargin_degrees;
    armData[1] = (uint8_t)(pllLoopFilterConfig->loopBandwidth_kHz & 0x00FF);
    armData[2] = (uint8_t)((pllLoopFilterConfig->loopBandwidth_kHz >> 8) & 0x00FF);
    armData[3] = pllLoopFilterConfig->powerScale;

    /* Write PLL Loop Filter Config to ARM mailbox */
    adi_adrv9025_CpuMemWrite(device,
                             (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                             &armData[0],
                             sizeof(armData),
                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Executing the SET PLL Loop filter config command */
    extData[0] = ADRV9025_CPU_OBJECTID_PLL_LOOP_BW;
    extData[1] = (uint8_t)pllName;
    adi_adrv9025_CpuCmdWrite(device,
                             ADI_ADRV9025_CPU_TYPE_C,
                             (uint8_t)ADRV9025_CPU_SET_OPCODE,
                             &extData[0],
                             sizeof(extData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_SETRFPLL_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_SETRFPLL_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte >> 1) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    errHdl,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                             extData[0],
                                                             cmdStatusByte),
                                    recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PllLoopFilterGet(adi_adrv9025_Device_t*           device,
                                      adi_adrv9025_PllName_e           pllName,
                                      adi_adrv9025_PllLoopFilterCfg_t* pllLoopFilterConfig)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t armData[4]     = {0};
    uint8_t extData[2]     = {0};
    uint8_t cmdStatusByte  = 0;
    int32_t errHdl         = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that pllLockStatus pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        pllLoopFilterConfig);

    /*Check that PLL selected is valid*/
    if ((pllName != ADI_ADRV9025_LO1_PLL) &&
        (pllName != ADI_ADRV9025_LO2_PLL) &&
        (pllName != ADI_ADRV9025_AUX_PLL))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pllName,
                         "Invalid PLL selected for PLL Loop Filter setting readback");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Executing the GET PLL Freq command */
    extData[0] = ADRV9025_CPU_OBJECTID_PLL_LOOP_BW;
    extData[1] = (uint8_t)pllName;
    adi_adrv9025_CpuCmdWrite(device,
                             ADI_ADRV9025_CPU_TYPE_C,
                             (uint8_t)ADRV9025_CPU_GET_OPCODE,
                             &extData[0],
                             sizeof(extData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_GETRFPLL_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETRFPLL_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte >> 1) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    errHdl,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                             extData[0],
                                                             cmdStatusByte),
                                    recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read PLL Frequency from ARM mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            sizeof(armData),
                            ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Deserialize ARM Data into pllLoopFilterConfig Structure*/
    pllLoopFilterConfig->phaseMargin_degrees = armData[0];
    pllLoopFilterConfig->loopBandwidth_kHz   = (((uint16_t)armData[1]) |
        ((uint16_t)armData[2] << 8));
    pllLoopFilterConfig->powerScale = armData[3];

    return device->common.error.newAction;
}

int32_t adi_adrv9025_DualBandStatusGet(adi_adrv9025_Device_t*         device,
                                       adi_adrv9025_DualbandStatus_e* status)
{
    static const uint8_t ARM_ERR_CODE      = 0x0E;
    int32_t              recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    uint8_t              armData[1]        = {0};
    uint8_t              extData[1]        = {ADRV9025_CPU_OBJECTID_BANDSWITCH_STAT};
    uint8_t              cmdStatusByte     = 0;
    const uint8_t        DUAL_BAND_ENABLE  = 0;
    const uint8_t        DUAL_BAND_DISABLE = 4;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        status);

    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to send ARM command");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_GETBANDSWITCHSTAT_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETBANDSWITCHSTAT_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
                                                     ADI_ADRV9025_CPU_TYPE_C,
                                                     ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                                              extData[0],
                                                                              cmdStatusByte),
                                                     recoveryAction,
                                                     ADI_ADRV9025_ACT_ERR_RESET_CPU);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     cmdStatusByte,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Dualband status from ARM mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            sizeof(armData),
                            ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (((uint8_t)armData[0] >= DUAL_BAND_ENABLE) && ((uint8_t)armData[0] < DUAL_BAND_DISABLE))
    {
        /* Dualband is enabled */
        *status = ADI_ADRV9025_DUALBAND_SWITCHING_ENABLED;
    }
    else
    {
        /* Dualband is disabled */
        *status = ADI_ADRV9025_DUALBAND_SWITCHING_DISABLED;
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_StreamVersionGet(adi_adrv9025_Device_t*        device,
                                      adi_adrv9025_StreamVersion_t* streamVersion)
{
    uint8_t ver[4]         = {0};
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    //Check device pointer is not null
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        streamVersion);

    ADRV9025_BUGINFO(__FUNCTION__);

    if ((device->devStateInfo.devState & ADI_ADRV9025_STATE_STREAMLOADED) != ADI_ADRV9025_STATE_STREAMLOADED)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Stream binary must be loaded before getting stream version");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adi_adrv9025_ArmMemRead(device,
                                             ADRV9025_CPU_ADDR_STREAM_VERSION,
                                             &ver[0],
                                             sizeof(ver),
                                             0);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to read ARM memory");
    ADI_ERROR_RETURN(device->common.error.newAction);

    //Store the StreamVersion
    streamVersion->majorVer = ver[0];
    streamVersion->minorVer = ver[1];
    streamVersion->mainVer  = ver[2];
    streamVersion->buildVer = ver[3];

    return (device->common.error.newAction);
}
