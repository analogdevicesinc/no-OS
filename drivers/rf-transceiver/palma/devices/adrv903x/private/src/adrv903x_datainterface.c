/**
 * Copyright 2015 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adrv903x_data_interface.c
 *
 * ADRV903X API Version: 2.12.1.4
 */

#include "../../private/include/adrv903x_datainterface.h"


#define ADI_FILE ADI_ADRV903X_FILE_PRIVATE_DATAINTERFACE

#include "adi_adrv903x_user.h"
#include "adi_adrv903x_hal.h"
#include "../../private/bf/adrv903x_bf_core.h"
#include "../../private/include/adrv903x_init.h"

#include "../../private/include/adrv903x_reg_addr_macros.h"
#include "adi_adrv903x_types.h"
#include "adi_common_macros.h"
#include "adi_common_types.h"
#include "adi_adrv903x_error.h"
#include "../../private/include/adrv903x_init.h"
#include "../../private/bf/adrv903x_bf_tx_dig_types.h"
#include "../../private/bf/adrv903x_bf_tx_dig.h"
#include "../../private/bf/adrv903x_bf_orx_dig.h"

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxOrxDataCaptureConfigAddressGet( adi_adrv903x_Device_t* const    device,
                                                                            const adi_adrv903x_RxChannels_e channelSelect,
                                                                            uint32_t* const                 address)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    /* Null pointer checks */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_RETURN(address)

    /* select the ram base address based on the channel select value */
    switch (channelSelect)
    {
    case ADI_ADRV903X_RXOFF:
        /* invalid capture location request */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelSelect, "Invalid channel selected for capture or readback");
        break;
    case ADI_ADRV903X_RX0:
        *address = ADRV903X_ADDR_TX0_CPT_CONFIG;
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_RX1:
        *address = ADRV903X_ADDR_TX1_CPT_CONFIG;
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_RX2:
        *address = ADRV903X_ADDR_TX2_CPT_CONFIG;
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_RX3:
        *address = ADRV903X_ADDR_TX3_CPT_CONFIG;
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_RX4:
        *address = ADRV903X_ADDR_TX4_CPT_CONFIG;
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_RX5:
        *address = ADRV903X_ADDR_TX5_CPT_CONFIG;
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_RX6:
        *address = ADRV903X_ADDR_TX6_CPT_CONFIG;
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_RX7:
        *address = ADRV903X_ADDR_TX7_CPT_CONFIG;
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_ORX0:
        *address = ADRV903X_ADDR_ORX0_CPT_CONFIG;
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_ORX1:
        *address = ADRV903X_ADDR_ORX1_CPT_CONFIG;
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    default:
        /* invalid capture location request */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelSelect, "Invalid channel selected for capture or readback");
        break;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxOrxDataCaptureRead(adi_adrv903x_Device_t* const    device,
                                                               const adi_adrv903x_RxChannels_e channelSelect,
                                                               uint32_t                        ramData[],
                                                               const uint32_t                  wordCount,
                                                               const adi_adrv903x_RxOrxDataCaptureLocation_e loc)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t baseAddress = 0U;
    uint32_t cptCfgAddr = 0U;
    const uint32_t BASE_ADDRESS_MASK = 0xFFF00000U;
    uint32_t wordOffset = 0x8000U;
    uint32_t ramDataIndex = 0U;
    uint32_t bankSize = wordCount >> 1;
    static const uint8_t BYTE_ORIENTED_DATA = 1u;

    (void)loc;

    /* Null pointer checks */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_RETURN(ramData);

    /* select the ram base address based on the channel select value */
    recoveryAction = adrv903x_RxOrxDataCaptureConfigAddressGet(device, channelSelect, &cptCfgAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading from the capture memory");
        return recoveryAction;
    }
    baseAddress = cptCfgAddr & BASE_ADDRESS_MASK; /* Capture memory is located at the base address of the slice */

    /* read memory */
    if (channelSelect == ADI_ADRV903X_ORX0 ||
        channelSelect == ADI_ADRV903X_ORX1)  /* reading from ORx capture ram 3 banks 4k each */
    {
        wordOffset = 0x4000;
                    /* read from bank 0 */
            recoveryAction = adrv903x_Registers32bOnlyRead_vEndian(device,
                                                                   NULL,
                                                                   baseAddress,
                                                                   (uint8_t*)(&ramData[0U]),
                                                                   wordCount * 4U,
                                                                   BYTE_ORIENTED_DATA);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading from the capture memory");
                return recoveryAction;
            }

            if (wordCount > ADI_ADRV903X_CAPTURE_SIZE_4K) /* read from bank 1 */
            {
                recoveryAction = adrv903x_Registers32bOnlyRead_vEndian(device,
                                                                       NULL,
                                                                       baseAddress + wordOffset,
                                                                       (uint8_t*)(&ramData[ADI_ADRV903X_CAPTURE_SIZE_4K]),
                                                                       ADI_ADRV903X_CAPTURE_SIZE_4K * 4U,
                                                                       BYTE_ORIENTED_DATA);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading from the capture memory");
                    return recoveryAction;
                }
            }
            if (wordCount > ADI_ADRV903X_CAPTURE_SIZE_8K) /* read from bank 2 */
            {
                recoveryAction = adrv903x_Registers32bOnlyRead_vEndian(device,
                                                                       NULL,
                                                                       baseAddress + wordOffset * 2U,
                                                                       (uint8_t*)(&ramData[ADI_ADRV903X_CAPTURE_SIZE_8K]),
                                                                       ADI_ADRV903X_CAPTURE_SIZE_4K * 4U,
                                                                       BYTE_ORIENTED_DATA);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading from the capture memory");
                    return recoveryAction;
                }
            }
                }
    else /* reading from dpd capture memory 2 banks 8k each */
    {
                            /* read bank 0 memory */
            recoveryAction = adrv903x_Registers32bOnlyRead_vEndian(device, NULL, baseAddress, (uint8_t*)(&ramData[ramDataIndex]), bankSize * 4U, BYTE_ORIENTED_DATA);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading from the capture memory");
                return recoveryAction;
            }

            /* read bank 1 memory */
            ramDataIndex += bankSize;
            recoveryAction = adrv903x_Registers32bOnlyRead_vEndian(device, NULL, baseAddress + wordOffset, (uint8_t*)(&ramData[ramDataIndex]), bankSize * 4U, BYTE_ORIENTED_DATA);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading from the capture memory");
                return recoveryAction;
            }
                }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxOrxDataCaptureConfigSet( adi_adrv903x_Device_t* const                  device,
                                                                     const adi_adrv903x_RxOrxDataCaptureLocation_e captureLocation,
                                                                     const uint32_t                                size,
                                                                     uint32_t*  const                              config)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t sizeMask = 0U;
    /* Null pointer checks */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_RETURN(config);
    switch (size)
    {
        case ADI_ADRV903X_CAPTURE_SIZE_16K:
            sizeMask = ADI_ADRV903X_CAPTURE_SIZE_MASK_16K;
            break;
        case ADI_ADRV903X_CAPTURE_SIZE_12K:
            sizeMask = ADI_ADRV903X_CAPTURE_SIZE_MASK_12K;
            break;
        case ADI_ADRV903X_CAPTURE_SIZE_8K:
            sizeMask = ADI_ADRV903X_CAPTURE_SIZE_MASK_8K;
            break;
        case ADI_ADRV903X_CAPTURE_SIZE_4K:
            sizeMask = ADI_ADRV903X_CAPTURE_SIZE_MASK_4K;
            break;
        case ADI_ADRV903X_CAPTURE_SIZE_2K:
            sizeMask = ADI_ADRV903X_CAPTURE_SIZE_MASK_2K;
            break;
        case ADI_ADRV903X_CAPTURE_SIZE_1K:
            sizeMask = ADI_ADRV903X_CAPTURE_SIZE_MASK_1K;
            break;
        case ADI_ADRV903X_CAPTURE_SIZE_512:
            sizeMask = ADI_ADRV903X_CAPTURE_SIZE_MASK_512;
            break;
        case ADI_ADRV903X_CAPTURE_SIZE_256:
            sizeMask = ADI_ADRV903X_CAPTURE_SIZE_MASK_256;
            break;
        case ADI_ADRV903X_CAPTURE_SIZE_128:
            sizeMask = ADI_ADRV903X_CAPTURE_SIZE_MASK_128;
            break;
        case ADI_ADRV903X_CAPTURE_SIZE_64:
            sizeMask = ADI_ADRV903X_CAPTURE_SIZE_MASK_64;
            break;
        case ADI_ADRV903X_CAPTURE_SIZE_32:
            sizeMask = ADI_ADRV903X_CAPTURE_SIZE_MASK_32;
            break;
        default:
            sizeMask = ADI_ADRV903X_CAPTURE_SIZE_MASK_12K;
            break;
    }
    /* set the base configuration and capture size to max samples;
     * no size mask is applied for ORx config because the default is the max 12k */
    switch (captureLocation)
    {
    case ADI_ADRV903X_CAPTURE_LOC_DDC0:
        *config = ADI_ADRV903X_CAPTURE_LOC_DDC0_BASE_CONFIG | sizeMask; /* Rx channel DDC0 */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_CAPTURE_LOC_DDC1:
        *config = ADI_ADRV903X_CAPTURE_LOC_DDC1_BASE_CONFIG | sizeMask; /* Rx channel DDC1 */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_CAPTURE_LOC_DATAPATH:
        *config = ADI_ADRV903X_CAPTURE_LOC_FORMATTER_BASE_CONFIG; /* ORx data path */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_CAPTURE_LOC_DPD:
        *config = ADI_ADRV903X_CAPTURE_LOC_DPD_BASE_CONFIG | sizeMask; /* DPD */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_CAPTURE_LOC_DPD_PRE  :
        *config = ADI_ADRV903X_CAPTURE_LOC_DPD_PRE_BASE_CONFIG | sizeMask; /* DPD pre actuator */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_CAPTURE_LOC_DPD_POST :
        *config = ADI_ADRV903X_CAPTURE_LOC_DPD_POST_BASE_CONFIG | sizeMask; /* DPD post actuator */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_CAPTURE_LOC_FSC      :
        /* TODO: Not yet supported */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        break;
    case ADI_ADRV903X_CAPTURE_LOC_ORX_TX0  :
        *config = ADI_ADRV903X_CAPTURE_LOC_ORX_TX0_BASE_CONFIG | sizeMask; /* ORx capture synchronized with Tx 0 */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_CAPTURE_LOC_ORX_TX1  :
        *config = ADI_ADRV903X_CAPTURE_LOC_ORX_TX1_BASE_CONFIG | sizeMask; /* ORx capture synchronized with Tx 1 */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_CAPTURE_LOC_ORX_TX2  :
        *config = ADI_ADRV903X_CAPTURE_LOC_ORX_TX2_BASE_CONFIG | sizeMask; /* ORx capture synchronized with Tx 2 */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_CAPTURE_LOC_ORX_TX3  :
        *config = ADI_ADRV903X_CAPTURE_LOC_ORX_TX3_BASE_CONFIG | sizeMask; /* ORx capture synchronized with Tx 3 */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_CAPTURE_LOC_ORX_TX4  :
        *config = ADI_ADRV903X_CAPTURE_LOC_ORX_TX4_BASE_CONFIG | sizeMask; /* ORx capture synchronized with Tx 4 */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_CAPTURE_LOC_ORX_TX5  :
        *config = ADI_ADRV903X_CAPTURE_LOC_ORX_TX5_BASE_CONFIG | sizeMask; /* ORx capture synchronized with Tx 5 */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_CAPTURE_LOC_ORX_TX6  :
        *config = ADI_ADRV903X_CAPTURE_LOC_ORX_TX6_BASE_CONFIG | sizeMask; /* ORx capture synchronized with Tx 6 */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    case ADI_ADRV903X_CAPTURE_LOC_ORX_TX7  :
        *config = ADI_ADRV903X_CAPTURE_LOC_ORX_TX7_BASE_CONFIG | sizeMask; /* ORx capture synchronized with Tx 7 */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        break;
    default:
        /* invalid capture location requested */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        break;
    }

    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error configuring the capture location. An invalid capture location was written");
        return recoveryAction;
    }

    return recoveryAction;
}


ADI_API adi_adrv903x_ErrAction_e adrv903x_RxOrxDataCaptureStreamDebugPoll(adi_adrv903x_Device_t* const    device,
                                                                          const adi_adrv903x_RxChannels_e channelSelect,
                                                                          uint32_t*  const                bStreamDbgFlag)

{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    const uint32_t READBACK_MASK = 0xFFFFFFFFU;

    /* Null pointer checks */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_RETURN(bStreamDbgFlag);

    switch (channelSelect)
    {
        case ADI_ADRV903X_ORX0:
            recoveryAction = adi_adrv903x_Register32Read(device, NULL, ADRV903X_ADDR_ORX0_STREAM_SCRATCH1, bStreamDbgFlag, READBACK_MASK);
            break;
        case ADI_ADRV903X_ORX1:
            recoveryAction = adi_adrv903x_Register32Read(device, NULL, ADRV903X_ADDR_ORX1_STREAM_SCRATCH1, bStreamDbgFlag, READBACK_MASK);
            break;
        case ADI_ADRV903X_RX0:
            recoveryAction = adi_adrv903x_Register32Read(device, NULL, ADRV903X_ADDR_RX0_STREAM_SCRATCH1, bStreamDbgFlag, READBACK_MASK);
            break;
        case ADI_ADRV903X_RX1:
            recoveryAction = adi_adrv903x_Register32Read(device, NULL, ADRV903X_ADDR_RX1_STREAM_SCRATCH1, bStreamDbgFlag, READBACK_MASK);
            break;
        case ADI_ADRV903X_RX2:
            recoveryAction = adi_adrv903x_Register32Read(device, NULL, ADRV903X_ADDR_RX2_STREAM_SCRATCH1, bStreamDbgFlag, READBACK_MASK);
            break;
        case ADI_ADRV903X_RX3:
            recoveryAction = adi_adrv903x_Register32Read(device, NULL, ADRV903X_ADDR_RX3_STREAM_SCRATCH1, bStreamDbgFlag, READBACK_MASK);
            break;
        case ADI_ADRV903X_RX4:
            recoveryAction = adi_adrv903x_Register32Read(device, NULL, ADRV903X_ADDR_RX4_STREAM_SCRATCH1, bStreamDbgFlag, READBACK_MASK);
            break;
        case ADI_ADRV903X_RX5:
            recoveryAction = adi_adrv903x_Register32Read(device, NULL, ADRV903X_ADDR_RX5_STREAM_SCRATCH1, bStreamDbgFlag, READBACK_MASK);
            break;
        case ADI_ADRV903X_RX6:
            recoveryAction = adi_adrv903x_Register32Read(device, NULL, ADRV903X_ADDR_RX6_STREAM_SCRATCH1, bStreamDbgFlag, READBACK_MASK);
            break;
        case ADI_ADRV903X_RX7:
            recoveryAction = adi_adrv903x_Register32Read(device, NULL, ADRV903X_ADDR_RX7_STREAM_SCRATCH1, bStreamDbgFlag, READBACK_MASK);
            break;
        default:
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_FramerBitfieldAddressGet(adi_adrv903x_Device_t* const device,
                                                                   const adi_adrv903x_FramerSel_e framerIdx,
                                                                   adrv903x_BfJtxLinkChanAddr_e* const framerBitfieldAddr)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, framerBitfieldAddr);

    switch (framerIdx)
    {
        case ADI_ADRV903X_FRAMER_0:
            *framerBitfieldAddr = ADRV903X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;
            break;

        case ADI_ADRV903X_FRAMER_1:
            *framerBitfieldAddr = ADRV903X_BF_DIGITAL_CORE_JESD_JTX_LINK_1_;
            break;

        case ADI_ADRV903X_FRAMER_2:
            *framerBitfieldAddr = ADRV903X_BF_DIGITAL_CORE_JESD_JTX_LINK_2_;
            break;

        default:
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       framerIdx,
                                       "Invalid Framer selection parameter ");
                return recoveryAction;
            }
    }

    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_DeframerBitfieldAddressGet(adi_adrv903x_Device_t* const device,
                                                                     const adi_adrv903x_DeframerSel_e deframerIdx,
                                                                     adrv903x_BfJrxLinkChanAddr_e* const deframerBitfieldAddr)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, deframerBitfieldAddr);

    switch (deframerIdx)
    {
        case ADI_ADRV903X_DEFRAMER_0:
            *deframerBitfieldAddr = ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
            break;

        case ADI_ADRV903X_DEFRAMER_1:
            *deframerBitfieldAddr = ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_;
            break;

        default:
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       deframerIdx,
                                       "Invalid Deframer selection parameter ");
                return recoveryAction;
            }
    }

    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_FramerLaneSerdesPhyBitfieldAddressGet(adi_adrv903x_Device_t* const device,
                                                                                const uint8_t laneIdx,
                                                                                adrv903x_BfSerdesTxdigPhyRegmapCore1p2ChanAddr_e* const laneSerdesPhyBitfieldAddr)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, laneSerdesPhyBitfieldAddr);

    switch (laneIdx)
    {
        case 0:
            *laneSerdesPhyBitfieldAddr = ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_0_;
            break;

        case 1:
            *laneSerdesPhyBitfieldAddr = ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_1_;
            break;

        case 2:
            *laneSerdesPhyBitfieldAddr = ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_2_;
            break;

        case 3:
            *laneSerdesPhyBitfieldAddr = ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_3_;
            break;

        case 4:
            *laneSerdesPhyBitfieldAddr = ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_4_;
            break;

        case 5:
            *laneSerdesPhyBitfieldAddr = ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_5_;
            break;

        case 6:
            *laneSerdesPhyBitfieldAddr = ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_6_;
            break;

        case 7:
            *laneSerdesPhyBitfieldAddr = ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_7_;
            break;

        default:
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       laneIdx,
                                       "Invalid Lane selection parameter ");
                return recoveryAction;
            }
    }

    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_FramerLaneSerdesPowerSet(adi_adrv903x_Device_t* const device,
                                                                   const uint8_t framerSelMask,
                                                                   const uint8_t powerAct)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfJtxLinkChanAddr_e framerBaseAddr    = ADRV903X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;
    adrv903x_BfSerdesTxdigPhyRegmapCore1p2ChanAddr_e laneSerdesPhyBitfieldAddr = ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_0_;
    uint8_t  laneIdx = 0U;
    uint8_t  lanePd = 0U;
    uint8_t  laneBit = 0U;
    uint8_t  serdesNeedPowerDownBySelectedFramers = 0U;
    uint8_t  serdesUsedByUnselectedFramers = 0U;
    uint8_t  serdesPowerDownSet = 0U;
    uint8_t  framerIdx = 0U;
    uint8_t  framerSel = 0U;
    const uint8_t PHY_POWER_DOWN  = 0x01;
    const uint8_t PHY_POWER_UP  = 0x00;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Prepare for framers enabling case */
    if (powerAct)
    {
        for (framerIdx = 0; framerIdx < ADI_ADRV903X_MAX_FRAMERS; framerIdx++)
        {
            framerSel = 1U << framerIdx;

            /* Skip unselected framer */
            if ((framerSel & framerSelMask) == 0)
            {
                continue;
            }

            /* Get the base address of the selected framer */
            recoveryAction = adrv903x_FramerBitfieldAddressGet(device,
                                                               (adi_adrv903x_FramerSel_e) framerSel,
                                                               &framerBaseAddr);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected framer");
                return recoveryAction;
            }

            /* Get the lane crossbar selection */
            for (laneIdx = 0U; laneIdx < ADI_ADRV903X_MAX_SERDES_LANES; ++laneIdx)
            {
                recoveryAction =  adrv903x_JtxLink_JtxForceLanePd_BfGet(device,
                                                                        NULL,
                                                                        framerBaseAddr,
                                                                        laneIdx,
                                                                        &lanePd);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane crossbar selection for the selected lane");
                    return recoveryAction;
                }


                if (lanePd == 0U) /* LanePd is not set, so power it up */
                {
                    recoveryAction = (adi_adrv903x_ErrAction_e) adrv903x_FramerLaneSerdesPhyBitfieldAddressGet(device,
                                                                                                               laneIdx,
                                                                                                               &laneSerdesPhyBitfieldAddr);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane serdes PHY address.");
                        return recoveryAction;
                    }

                    recoveryAction =  adrv903x_SerdesTxdigPhyRegmapCore1p2_PdSer_BfSet(device,
                                                                                       NULL,
                                                                                       laneSerdesPhyBitfieldAddr,
                                                                                       PHY_POWER_UP);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to power up lane serdes PHY.");
                        return recoveryAction;
                    }
                }
            }
        }

        /* The power up was done above */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        return recoveryAction;
    }

    /* Prepare for framers disabling case */
    for (framerIdx = 0; framerIdx < ADI_ADRV903X_MAX_FRAMERS; framerIdx++)
    {
        framerSel = 1U << framerIdx;

        /* Get the base address of the selected framer */
        recoveryAction = adrv903x_FramerBitfieldAddressGet(device,
                                                           (adi_adrv903x_FramerSel_e) framerSel,
                                                           &framerBaseAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected framer");
            return recoveryAction;
        }

        /* Get the lane crossbar selection */
        for (laneIdx = 0U; laneIdx < ADI_ADRV903X_MAX_SERDES_LANES; ++laneIdx)
        {
            laneBit = 1U << laneIdx;
            recoveryAction =  adrv903x_JtxLink_JtxForceLanePd_BfGet(device,
                                                                    NULL,
                                                                    framerBaseAddr,
                                                                    laneIdx,
                                                                    &lanePd);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane crossbar selection for the selected lane");
                return recoveryAction;
            }


            if (lanePd) /* LanePd is set, so it can be powered it down if it is not shared */
            {
                if (framerSel & framerSelMask)
                {
                    serdesNeedPowerDownBySelectedFramers |= laneBit; /* aggregate all lane serdes that need power down by selected framers */
                }
                else
                {
                    serdesUsedByUnselectedFramers |= laneBit; /* aggregate all lane serdes being used by unselected framers */
                }
            }
        }
    }

    /* Exclude serdes that being used by unselected frames */
    serdesPowerDownSet = serdesNeedPowerDownBySelectedFramers & ~serdesUsedByUnselectedFramers;


    /* Power down the serdes PHY based on serdesPowerDownSet */
    for (laneIdx = 0U; laneIdx < ADI_ADRV903X_MAX_SERDES_LANES; ++laneIdx)
    {
        laneBit = 1 << laneIdx;
        if (laneBit & serdesPowerDownSet)
        {
            recoveryAction = (adi_adrv903x_ErrAction_e) adrv903x_FramerLaneSerdesPhyBitfieldAddressGet(device,
                                                                                                       laneIdx,
                                                                                                       &laneSerdesPhyBitfieldAddr);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane serdes PHY address.");
                return recoveryAction;
            }

            recoveryAction =  adrv903x_SerdesTxdigPhyRegmapCore1p2_PdSer_BfSet(device,
                                                                               NULL,
                                                                               laneSerdesPhyBitfieldAddr,
                                                                               PHY_POWER_DOWN);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to change power down serdes PHY.");
                return recoveryAction;
            }
        }
    }

    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_DeframerLaneSerdesPowerSet(adi_adrv903x_Device_t* const device,
                                                                     const uint8_t deframerSelMask,
                                                                     const uint8_t powerAct)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfJrxLinkChanAddr_e deframerBaseAddr    = ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    uint8_t deframerSel = 0U;
    uint8_t deframerIdx = 0U;
    uint8_t  serdesNeedPowerDownBySelectedDeframers = 0U;
    uint8_t  serdesUsedByUnselectedDeframers = 0U;
    uint8_t  serdesPowerDownSet = 0U;
    uint8_t  laneIdx = 0U;
    uint8_t  laneBit = 0U;
    uint8_t  dataByte = 0U;
    uint8_t  pLane = 0U;
    uint8_t  j = 0U;
    uint8_t  deserPhyPowerBit = 0U;
    uint8_t  numberofLaneL = 0U;
    uint8_t  deserPhyPowerSetMask = 0x0FU;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Prepare for framers enabling case */
    if (powerAct)
    {
        for (deframerIdx = 0U; deframerIdx < ADI_ADRV903X_MAX_DEFRAMERS; deframerIdx++)
        {
            deframerSel = 1U << deframerIdx;
            /* Skip unselected deframer */
            if ((deframerSel & deframerSelMask) == 0U)
            {
                continue;
            }

            /* Get the base address of the selected deframer */
            recoveryAction = adrv903x_DeframerBitfieldAddressGet(device,
                                                                 (adi_adrv903x_DeframerSel_e) deframerSel,
                                                                 &deframerBaseAddr);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
                return recoveryAction;
            }

            recoveryAction =  adrv903x_JrxLink_JrxCoreLCfg_BfGet(device,
                                                                 NULL,
                                                                 deframerBaseAddr,
                                                                 &numberofLaneL);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of lane");
                return recoveryAction;
            }

            /* Read the current deserializer power status for all lanes*/
            recoveryAction =  adrv903x_SerdesRxdig8packRegmapCore1p2_RxdesPdCh_BfGet(device,
                                                                                     NULL,
                                                                                     ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_TOP_8PACK,
                                                                                     &deserPhyPowerSetMask);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading deserializer Phy power status");
                return recoveryAction;
            }

            /* Get the lane crossbar selection */
            for (laneIdx = 0U; laneIdx < ADI_ADRV903X_MAX_SERDES_LANES; ++laneIdx)
            {
                for (j = 0U; j < numberofLaneL; j++)
                {
                    recoveryAction =  adrv903x_JrxLink_JrxCoreLaneSel_BfGet(device,
                                                                            NULL,
                                                                            deframerBaseAddr,
                                                                            j,
                                                                            &dataByte);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading Jrx Core Lane Sel field");
                        return recoveryAction;
                    }

                    if (dataByte == laneIdx)
                    {
                        deserPhyPowerBit |=  1U << laneIdx;
                        break;
                    }
                }
            }
        }

        /* Exclude serdes that being used by unselected frames */
        deserPhyPowerSetMask &= ~deserPhyPowerBit; /* Clear the corresponding bit to power up */
        recoveryAction =  adrv903x_SerdesRxdig8packRegmapCore1p2_RxdesPdCh_BfSet(device,
                                                                                 NULL,
                                                                                 ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_TOP_8PACK,
                                                                                 deserPhyPowerSetMask);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to change lane deserdes PHY power.");
            return recoveryAction;
        }

        /* The power up was done above */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        return recoveryAction;
    }

    /* Prepare for deframers disabling case */
    for (deframerIdx = 0U; deframerIdx < ADI_ADRV903X_MAX_DEFRAMERS; deframerIdx++)
    {
        deframerSel = 1U << deframerIdx;

        /* Get the base address of the selected deframer */
        recoveryAction = adrv903x_DeframerBitfieldAddressGet(device,
                                                             (adi_adrv903x_DeframerSel_e) deframerSel,
                                                             &deframerBaseAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
            return recoveryAction;
        }

        /* Get the number of logical lanes */
        recoveryAction =  adrv903x_JrxLink_JrxCoreLCfg_BfGet(device,
                                                             NULL,
                                                             deframerBaseAddr,
                                                             &numberofLaneL);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of lane");
            return recoveryAction;
        }

        /* Get the lane crossbar selection */
        for (laneIdx = 0U; laneIdx < ADI_ADRV903X_MAX_SERDES_LANES; ++laneIdx)
        {
            laneBit = 1U << laneIdx;
            pLane  = 0U;
            for (j = 0U; j < numberofLaneL; j++)
            {
                recoveryAction =  adrv903x_JrxLink_JrxCoreLaneSel_BfGet(device,
                                                                        NULL,
                                                                        deframerBaseAddr,
                                                                        j,
                                                                        &dataByte);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading Jrx Core Lane Sel field");
                    return recoveryAction;
                }

                if (dataByte == laneIdx)
                {
                    pLane = 1U;
                    break;
                }
            }

            if (pLane == 1U)
            {
                if (deframerSel & deframerSelMask)
                {
                    serdesNeedPowerDownBySelectedDeframers |= laneBit; /* aggregate all lane serdes that need power down by selected deframers */
                }
                else
                {
                    serdesUsedByUnselectedDeframers |= laneBit; /* aggregate all lane serdes being used by unselected deframers */
                }
            }
        }
    }

    /* Read the current deserializer power status for all lanes*/
    recoveryAction =  adrv903x_SerdesRxdig8packRegmapCore1p2_RxdesPdCh_BfGet(device,
                                                                             NULL,
                                                                             ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_TOP_8PACK,
                                                                             &deserPhyPowerSetMask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading deserializer Phy power status");
        return recoveryAction;
    }

    /* Exclude serdes that being used by unselected frames */
    serdesPowerDownSet = serdesNeedPowerDownBySelectedDeframers & ~serdesUsedByUnselectedDeframers;

    /* Prepare for serdes power bit mask.*/
    deserPhyPowerSetMask &= ~serdesPowerDownSet; /* Clear the corresponding bit to power up */

    recoveryAction =  adrv903x_SerdesRxdig8packRegmapCore1p2_RxdesPdCh_BfSet(device,
                                                                             NULL,
                                                                             ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_TOP_8PACK,
                                                                             deserPhyPowerSetMask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to power down lane deserdes PHY.");
        return recoveryAction;
    }

    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_FramerLaneEnableGet(adi_adrv903x_Device_t* const device,
                                                              const adi_adrv903x_FramerSel_e   framerSel,
                                                              adi_adrv903x_FramerCfg_t* const  framerCfg)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfJtxLinkChanAddr_e framerBaseAddr    = ADRV903X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;
    uint8_t  dataByte = 0U;
    uint8_t  laneId = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, framerCfg);

    /* Get the base address of the selected framer */
    recoveryAction = adrv903x_FramerBitfieldAddressGet(device, framerSel, &framerBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected framer");
        return recoveryAction;
    }

    /* Get the lane crossbar selection */
    framerCfg->serializerLanesEnabled = 0;
    for (laneId = 0U; laneId < ADI_ADRV903X_MAX_SERDES_LANES; ++laneId)
    {
        recoveryAction =  adrv903x_JtxLink_JtxLaneSel_BfGet(device,
                                                            NULL,
                                                            framerBaseAddr,
                                                            laneId,
                                                            &dataByte);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane crossbar selection for the selected framer");
            return recoveryAction;
        }

        framerCfg->serializerLanePdCrossbar.laneFramerOutSel[laneId] = 0x08U; /* assigned to out of range value */
        if (dataByte < 0x1FU)
        {
            framerCfg->serializerLanesEnabled |= (0x1U << laneId);
            /* Get the physical lane in use based on link and crossbar configuration */
            framerCfg->serializerLanePdCrossbar.laneFramerOutSel[laneId] = dataByte;
        }
    }

    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_DeframerLaneEnableGet(adi_adrv903x_Device_t* const device,
                                                                const adi_adrv903x_DeframerSel_e   deframerSel,
                                                                adi_adrv903x_DeframerCfg_t* const  deframerCfg)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfJrxLinkChanAddr_e deframerBaseAddr    = ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    uint8_t  dataByte = 0U;
    uint8_t  laneId = 0U;
    uint8_t  cfgLvalue = 0U;
    uint8_t  cfgM = 0U;
    uint8_t  cfgK = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, deframerCfg);

    /* Initialization */
    deframerCfg->deserializerLanesEnabled = 0U;
    for (laneId = 0U; laneId < ADI_ADRV903X_MAX_SERDES_LANES; ++laneId)
    {
        deframerCfg->deserializerLaneCrossbar.deframerInputLaneSel[laneId] = 0x08U; /* assigned to out of range value */
    }

    /* Get the base address of the selected framer */
    recoveryAction = adrv903x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        return recoveryAction;
    }

    /* Get deframer L value */
    recoveryAction = adrv903x_JrxLink_JrxCoreLCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &cfgLvalue);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG L cfg for the selected deframer");
        return recoveryAction;
    }

    /* Check if deframer link has lane configured or not */
    if (cfgLvalue == 0U)
    {
        /* get M - Number of converters per device */
        recoveryAction = adrv903x_JrxLink_JrxCoreMCfg_BfGet(device,
                                                            NULL,
                                                            deframerBaseAddr,
                                                            &cfgM);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of converters per device");
            return recoveryAction;
        }

        /* get K - Number of frames in extended multiblock */
        recoveryAction = adrv903x_JrxLink_JrxCoreKCfg_BfGet(device,
                                                            NULL,
                                                            deframerBaseAddr,
                                                            &cfgK);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of frames in extended multiblock");
            return recoveryAction;
        }

        if ((cfgM == 0U) && (cfgK == 0U))
        {
            /* Link has no lane configured */
            return recoveryAction;
        }
        /* Link has 1 lane */
    }

    /* Get the lane crossbar selection */
    for (laneId = 0U; laneId < ADI_ADRV903X_MAX_SERDES_LANES; ++laneId)
    {
        recoveryAction =  adrv903x_JrxLink_JrxCoreLaneSel_BfGet(device,
                                                                NULL,
                                                                deframerBaseAddr,
                                                                laneId,
                                                                &dataByte);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane crossbar selection");
            return recoveryAction;
        }

        if (laneId <= cfgLvalue)
        {
            if (dataByte < 0x1FU)
            {
                deframerCfg->deserializerLanesEnabled |= (0x1U << laneId);
                /* Get the physical lane in use based on link and crossbar configuration */
                deframerCfg->deserializerLaneCrossbar.deframerInputLaneSel[laneId] = dataByte;
            }
        }
    }

    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RadClkDividerValueCalculate(adi_adrv903x_Device_t* const    device,
                                                                      const adi_adrv903x_RxChannels_e channelSelect,
                                                                      const adi_adrv903x_RxOrxDataCaptureLocation_e loc,
                                                                      uint8_t*                        value)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t ratio = 0U; /* ratio can be as high as 128; must be a power of 2 */
    uint8_t divN = 0U;  /* divN is log2(ratio) */
    uint8_t rxOutputRateIndex = 11U; /* initialize the index to be out of range */
    uint8_t idx = 1U;
    uint32_t tgtSampleRate = 0U;
    const uint8_t MAXBIT = 8U; /* number of bits to use when calculating log base 2 */
    const uint8_t MAXRATEINDEX = 9U;

    /* Null pointer checks */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    if (loc == ADI_ADRV903X_CAPTURE_LOC_DDC0 ||
        loc == ADI_ADRV903X_CAPTURE_LOC_DDC1 ||
        loc == ADI_ADRV903X_CAPTURE_LOC_DATAPATH)
    {
        switch (channelSelect)
        {
            case ADI_ADRV903X_RXOFF:
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelSelect, "Invalid channel selected for capture or readback");
                break;
            case ADI_ADRV903X_RX0:
                rxOutputRateIndex = 0U;
                break;
            case ADI_ADRV903X_RX1:
                rxOutputRateIndex = 1U;
                break;
            case ADI_ADRV903X_RX2:
                rxOutputRateIndex = 2U;
                break;
            case ADI_ADRV903X_RX3:
                rxOutputRateIndex = 3U;
                break;
            case ADI_ADRV903X_RX4:
                rxOutputRateIndex = 4U;
                break;
            case ADI_ADRV903X_RX5:
                rxOutputRateIndex = 5U;
                break;
            case ADI_ADRV903X_RX6:
                rxOutputRateIndex = 6U;
                break;
            case ADI_ADRV903X_RX7:
                rxOutputRateIndex = 7U;
                break;
            case ADI_ADRV903X_ORX0:
                rxOutputRateIndex = 8U;
                break;
            case ADI_ADRV903X_ORX1:
                rxOutputRateIndex = 9U;
                break;
            default :
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelSelect, "Invalid channel selected for capture or readback");
                break;
        }
    }
    if (rxOutputRateIndex <= MAXRATEINDEX)
    {
        /* Make sure divide by zero doesn't happen */
        if (rxOutputRateIndex == 9U) /* ORx 1 */
        {
            if (device->initExtract.orx.orxChannelCfg[1].orxOutputRate_kHz > 0U)
            {
                tgtSampleRate = device->initExtract.orx.orxChannelCfg[1].orxOutputRate_kHz;
                ratio = device->initExtract.clocks.hsDigClk_kHz / tgtSampleRate;
                recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
            }
        }
        else if (rxOutputRateIndex == 8U) /* ORx 0 */
        {
            if (device->initExtract.orx.orxChannelCfg[0].orxOutputRate_kHz > 0U)
            {
                tgtSampleRate = device->initExtract.orx.orxChannelCfg[0].orxOutputRate_kHz;
                ratio = device->initExtract.clocks.hsDigClk_kHz / tgtSampleRate;
                recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
            }
        }
        else
        {
            if (loc == ADI_ADRV903X_CAPTURE_LOC_DDC0)
            {
                tgtSampleRate = device->initExtract.rx.rxChannelCfg[rxOutputRateIndex].rxDdc0OutputRate_kHz;
                ratio = device->initExtract.clocks.hsDigClk_kHz / tgtSampleRate;
                recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
            }
            else if (loc == ADI_ADRV903X_CAPTURE_LOC_DDC1)
            {
                tgtSampleRate = device->initExtract.rx.rxChannelCfg[rxOutputRateIndex].rxDdc1OutputRate_kHz;
                ratio = device->initExtract.clocks.hsDigClk_kHz / tgtSampleRate;
                recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
            }
            else
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelSelect, "Invalid location selected for the selected channel");
            }
        }
    }
    else
    {
        ratio = 1U;
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
    }
    /* The divider ratio is a 3 bit value and only divides by powers of 2. Check the ratio here and
     * return an error if the current configuration requires other than a power of 2 division.
     */
    if (ratio != 1 && /* divide by 2 to the power of 0 */
        ratio != 2 && /* divide by 2 to the power of 1 */
        ratio != 4 && /* divide by 2 to the power of 2 */
        ratio != 8 && /* divide by 2 to the power of 3 */
        ratio != 16 && /* divide by 2 to the power of 4 */
        ratio != 32 && /* divide by 2 to the power of 5 */
        ratio != 64 && /* divide by 2 to the power of 6 */
        ratio != 128) /* divide by 2 to the power of 7 */
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_FEATURE;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "The current clock configuration does not permit the correct sampling of data using internal capture memory");
    }
    if (ADI_ADRV903X_ERR_ACT_NONE == recoveryAction)
    {
        /* compute log 2 */
        for (idx = 0U; idx <= MAXBIT; idx++)
        {
            if (ratio >>= 1)
            {
                divN++;
            }
        }
        *value = divN;
    }
    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_StatusRegisterPoll(adi_adrv903x_Device_t* const    device,
                                                             const adi_adrv903x_RxChannels_e channelSelect,
                                                             uint8_t* const                  bCptBusy)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t statusRegister = 0U;
    const uint32_t bitfieldMask = 0xFFFF0000U; /* The bitfield function uses the slice base address to access the capture status registers */
    /* Null pointer checks */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    switch (channelSelect)
    {
    case ADI_ADRV903X_RX0:
        statusRegister = ADRV903X_ADDR_TX0_CPT_STATUS & bitfieldMask; /* the bitfield get function needs the slice base address */
        break;
    case ADI_ADRV903X_RX1:
        statusRegister = ADRV903X_ADDR_TX1_CPT_STATUS & bitfieldMask;
        break;
    case ADI_ADRV903X_RX2:
        statusRegister = ADRV903X_ADDR_TX2_CPT_STATUS & bitfieldMask;
        break;
    case ADI_ADRV903X_RX3:
        statusRegister = ADRV903X_ADDR_TX3_CPT_STATUS & bitfieldMask;
        break;
    case ADI_ADRV903X_RX4:
        statusRegister = ADRV903X_ADDR_TX4_CPT_STATUS & bitfieldMask;
        break;
    case ADI_ADRV903X_RX5:
        statusRegister = ADRV903X_ADDR_TX5_CPT_STATUS & bitfieldMask;
        break;
    case ADI_ADRV903X_RX6:
        statusRegister = ADRV903X_ADDR_TX6_CPT_STATUS & bitfieldMask;
        break;
    case ADI_ADRV903X_RX7:
        statusRegister = ADRV903X_ADDR_TX7_CPT_STATUS & bitfieldMask;
        break;
    case ADI_ADRV903X_ORX0:
        statusRegister = ADRV903X_ADDR_ORX0_CPT_STATUS0 & bitfieldMask;
        break;
    case ADI_ADRV903X_ORX1:
        statusRegister = ADRV903X_ADDR_ORX1_CPT_STATUS0 & bitfieldMask;
        break;
    default:
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, statusRegister, "Invalid capture status register chosen for readback");
        break;
    }
    if (channelSelect == ADI_ADRV903X_ORX0 ||
        channelSelect == ADI_ADRV903X_ORX1)
    {
        recoveryAction = adrv903x_OrxDig_CptBusy_BfGet(device,
                                                       NULL,
                                                       (adrv903x_BfOrxDigChanAddr_e) statusRegister,
                                                       bCptBusy);
    }
    else
    {
        recoveryAction = adrv903x_TxDig_TxCptBusy_BfGet(device,
                                                        NULL,
                                                        (adrv903x_BfTxDigChanAddr_e) statusRegister,
                                                        bCptBusy);
    }

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading from the capture status register capture busy bitfield");
    }
    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxRepairScreenTestChecker(adi_adrv903x_Device_t* const  device,
                                                                     uint8_t* const                screenID)
{
    adi_adrv903x_ErrAction_e  recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

    const uint8_t  numRetries = 5U;
    uint8_t        regValue   = 0U;
    uint32_t       efuseData  = 0U;
    uint32_t       i          = 0U;

    static const uint8_t EFUSE_READ_ADDRESS = 0x09U;
    static const uint8_t EFUSE_READ_CONTROL = 0x19U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Read ready bit. It needs to be 1 */
    recoveryAction = adi_adrv903x_ProductIdGet(device, &regValue);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Efuse Product Id");
        return recoveryAction;
    }

    /* Write Efuse Read Address */
    recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_EFUSE_READ_ADDR, &EFUSE_READ_ADDRESS, 1U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write Efuse Read control register");
        return recoveryAction;
    }

    /* Start read command */
    recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_EFUSE_READ_CTRL, &EFUSE_READ_CONTROL, 1U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write Efuse Read control register");
        return recoveryAction;
    }

    /* Wait for Data Valid and Read State */
    for (i = 0U; i < numRetries; i++)
    {
        recoveryAction = adrv903x_Core_EfuseReadDataValid_BfGet(device,
                                                                NULL,
                                                                ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                &regValue);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read Efuse Data Valid status");
            return recoveryAction;
        }

        if (regValue == 1U)
        {
            recoveryAction = adrv903x_Core_EfuseReadState_BfGet(device,
                                                                NULL,
                                                                ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                &regValue);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read Efuse Read State status");
                return recoveryAction;
            }

            if (regValue == 1U)
            {
                break;
            }
        }
    }

    if (regValue == 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_FEATURE;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to read Efuse Data");
        return recoveryAction;
    }

    /* Read Efuse Data */
    recoveryAction = adrv903x_Core_EfuseReadData_BfGet(device,
                                                       NULL,
                                                       ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                       &efuseData);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read Efuse Data");
        return recoveryAction;
    }

    /* get CM screen bit */
    *screenID = (uint8_t)((efuseData >> 16U) & 0x01U);

    return recoveryAction;
}
ADI_API adi_adrv903x_ErrAction_e adrv903x_GetJtxLanePoweredDown(adi_adrv903x_Device_t*                           const device,
                                                                adrv903x_BfSerdesTxdigPhyRegmapCore1p2ChanAddr_e const laneSerdesPhyBitfieldAddr,
                                                                uint8_t*                                         const phyLanePd)
{
        adi_adrv903x_ErrAction_e  recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

    uint8_t pdSer   = 0U;
    uint8_t serEnRc = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, phyLanePd);

    recoveryAction = adrv903x_SerdesTxdigPhyRegmapCore1p2_PdSer_BfGet(device,
                                                                      NULL,
                                                                      laneSerdesPhyBitfieldAddr,
                                                                      &pdSer);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to read phy lane pd");
        return recoveryAction;
    }

    if (pdSer == 1U)
    {
        /* Lane is in h/w power-down; No need to fetch SerEnRc */
        *phyLanePd = 1U;
        return recoveryAction;
    }
    
    recoveryAction = adrv903x_SerdesTxdigPhyRegmapCore1p2_SerEnRc_BfGet(device,
                                                                        NULL,
                                                                        laneSerdesPhyBitfieldAddr,
                                                                        &serEnRc);
    
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to read phy lane s/w power up");
        return recoveryAction;
    }

    /* serEnRc bitfield indicates lane powered up. Return value must indicate power *down* */
    *phyLanePd = (uint8_t) !serEnRc;
    
    return recoveryAction;
}
    
ADI_API adi_adrv903x_ErrAction_e adrv903x_DeframerCfgGet(adi_adrv903x_Device_t* const        device,
                                                         const adi_adrv903x_DeframerSel_e    deframerSel,
                                                         const adi_adrv903x_TxChannels_e     chanSel,
                                                         adi_adrv903x_DeframerCfg_t* const   deframerCfg,
                                                         const bool bBypass,
                                                         const bool bParamScaling)
{

    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfJrxLinkChanAddr_e deframerBaseAddr                            = ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    uint8_t                      enabledDeframers                            = 0U;
    uint8_t                      laneIdx                                     = 0U;
    uint8_t                      deframerIdx                                 = 0U;
    uint8_t                      tmpByte                                     = 0U;
    uint8_t interleavingEnabled;    

    /* Get all deframers link state */
    recoveryAction = adrv903x_JesdCommon_JrxLinkEn_BfGet(device,
        NULL,
        ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
        &enabledDeframers);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get deframer link state failed");
        return recoveryAction;
    }

    /* Get the base address of the selected framer */
    recoveryAction = adrv903x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected framer");
        return recoveryAction;
    }

    /* Get the Deframer IQ Rate and Lane Rate */
    switch ((int32_t) deframerSel)
    {
    case ADI_ADRV903X_DEFRAMER_0:
        deframerIdx = 0;
        break;

    case ADI_ADRV903X_DEFRAMER_1:
        deframerIdx = 1;
        break;
    }

    interleavingEnabled = device->initExtract.jesdSetting.deframerSetting[deframerIdx].interleavingEnabled;

    /* Get the deframer link type */
    recoveryAction = adrv903x_JrxLink_JrxLinkType_BfGet(device,
        NULL,
        deframerBaseAddr,
        &deframerCfg->enableJesd204C);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link type for the selected deframer");
        return recoveryAction;
    }

    /* Bank ID for Jrx is not available, assign it to zero */
    deframerCfg->bankId = 0;

    /* Get the deframer device ID */
    recoveryAction = adrv903x_JrxLink_JrxCoreDidCfg_BfGet(device,
        NULL,
        deframerBaseAddr,
        &deframerCfg->deviceId);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get device ID for the selected deframer");
        return recoveryAction;
    }

    /* Get the deframer lane ID */
    for (laneIdx = 0U; laneIdx < ADI_ADRV903X_MAX_DESERIALIZER_LANES; laneIdx++)
    {
        recoveryAction = adrv903x_JrxLink_JrxCoreLidCfg_BfGet(device,
            NULL,
            deframerBaseAddr,
            laneIdx,
            &deframerCfg->laneId[laneIdx]);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane ID for the selected deframer");
            return recoveryAction;
        }
    }

    /* get M - Number of converters per device */
    recoveryAction = adrv903x_JrxLink_JrxCoreMCfg_BfGet(device,
        NULL,
        deframerBaseAddr,
        &deframerCfg->jesd204M);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of converters per device");
        return recoveryAction;
    }
    ++deframerCfg->jesd204M;
    if (interleavingEnabled && bParamScaling)
    {
        deframerCfg->jesd204M *= 4;
    }    

    /* get K - Number of frames in extended multiblocks */
    recoveryAction = adrv903x_JrxLink_JrxCoreKCfg_BfGet(device,
        NULL,
        deframerBaseAddr,
        &tmpByte);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of frames in extended multiblocks");
        return recoveryAction;
    }

    deframerCfg->jesd204K = tmpByte + 1;
    if (interleavingEnabled && bParamScaling)
    {
        deframerCfg->jesd204K /= 4;
    }    

    /* get Np - Total number of bits per sample */
    recoveryAction = adrv903x_JrxLink_JrxCoreNpCfg_BfGet(device,
        NULL,
        deframerBaseAddr,
        &deframerCfg->jesd204Np);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get total number of bits per sample");
        return recoveryAction;
    }
    ++deframerCfg->jesd204Np;

    /* get F - Number of octets per frame per lane*/
    recoveryAction = adrv903x_JrxLink_JrxCoreFCfg_BfGet(device,
        NULL,
        deframerBaseAddr,
        &deframerCfg->jesd204F);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of octets per frame per lane");
        return recoveryAction;
    }
    ++deframerCfg->jesd204F;
    if (interleavingEnabled && bParamScaling)
    {
        deframerCfg->jesd204F *= 4;
    }    

    /* get E - Number of multiblocks in extended multiblocks */
    /* This parameter is only valid for Jesd204C */
    deframerCfg->jesd204E = 0;
    if (deframerCfg->enableJesd204C == 1)
    {
        recoveryAction = adrv903x_JrxLink_JrxCoreECfg_BfGet(device,
            NULL,
            deframerBaseAddr,
            &deframerCfg->jesd204E);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of multiblocks in extended multiblocks");
            return recoveryAction;
        }
    }

    /* get descrambling enabled */
    recoveryAction = adrv903x_JrxLink_JrxCoreDscrCfg_BfGet(device,
        NULL,
        deframerBaseAddr,
        &deframerCfg->decrambling);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get descrambling enabled");
        return recoveryAction;
    }

    /* Get the lane crossbar selection and physical lane crossbar */
    recoveryAction = adrv903x_DeframerLaneEnableGet(device, deframerSel, deframerCfg);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane crossbar selection and physical lane");
        return recoveryAction;
    }

    /* get global LMFC phase adjustment */
    recoveryAction = adrv903x_JrxLink_JrxCorePhaseAdjust_BfGet(device,
        NULL,
        deframerBaseAddr,
        &deframerCfg->lmfcOffset);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get global LMFC phase adjustment");
        return recoveryAction;
    }

    /* get sync Out Cmos Drive strength for both SyncOut0  and SyncOut1 */
    recoveryAction = adrv903x_Core_SyncOut0CmosTxDriveStrength_BfGet(device,
        NULL,
        (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
        &deframerCfg->syncbOut0CmosDriveStrength);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get syncbOut0CmosDriveStrength");
        return recoveryAction;
    }

    recoveryAction = adrv903x_Core_SyncOut1CmosTxDriveStrength_BfGet(device,
        NULL,
        (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
        &deframerCfg->syncbOut1CmosDriveStrength);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get syncbOut1CmosDriveStrength");
        return recoveryAction;
    }

    /* get sync out select */
    recoveryAction = adrv903x_JrxLink_JrxCoreSyncNSel_BfGet(device,
        NULL,
        deframerBaseAddr,
        &deframerCfg->syncbOutSelect);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sync out select");
        return recoveryAction;
    }

    /* Should be 0, 1, 2, or 3 */
    if (deframerCfg->syncbOutSelect > 3U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid syncbOutSelect value.");
        return recoveryAction;
    }

    /* Re-encode syncbOutSelect to 0: pin-0, 1: pin-1, 2: both pins, 3: no pin selected */
    if (deframerCfg->syncbOutSelect == 0U)
    {
        deframerCfg->syncbOutSelect = 3U;
    }
    else
    {
        deframerCfg->syncbOutSelect -= 1U;
    }

    /* If any pin selected */
    if (deframerCfg->syncbOutSelect != 3U)
    {
        /* get syncbOut Lvds mode */
        recoveryAction = adrv903x_Core_SyncOutLvdsAndCmosEnable_BfGet(device,
            NULL,
            (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
            &deframerCfg->syncbOutLvdsMode);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get syncbOut Lvds mode");
            return recoveryAction;
        }

        /* get lvds pin invert */
        recoveryAction = adrv903x_Core_SyncOutLvdsPnInvert_BfGet(device,
            NULL,
            (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
            &deframerCfg->syncbOutLvdsPnInvert);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lvds pin invert");
            return recoveryAction;
        }

        /* Case link-0 or both link-0 and link-1 selected */
        if ((deframerCfg->syncbOutSelect == 0U) || (deframerCfg->syncbOutSelect == 2U))
        {
            deframerCfg->syncbOutLvdsMode = deframerCfg->syncbOutLvdsMode & 0x01U;
            deframerCfg->syncbOutLvdsPnInvert = deframerCfg->syncbOutLvdsPnInvert & 0x01U;
        }
        else if (deframerCfg->syncbOutSelect == 1U) /* Case only link-1 selected */
        {
            deframerCfg->syncbOutLvdsMode = (deframerCfg->syncbOutLvdsMode >> 1U) & 0x01U;
            deframerCfg->syncbOutLvdsPnInvert = (deframerCfg->syncbOutLvdsPnInvert >> 1U) & 0x01U;
        }
        else
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid 'syncbOutSelect', should be 0-2");
            return recoveryAction;
        }
    }

    /* get Deframer output to DAC mapping */
    recoveryAction = adrv903x_DacSampleXbarGet(device,
                                               deframerSel,
                                               &deframerCfg->dacCrossbar);
    (void)bBypass;
    (void)chanSel;

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get dacCrossbar");
        return recoveryAction;
    }

    /* get sysref on relink enable */
    recoveryAction = adrv903x_JrxLink_JrxCoreSysrefForRelink_BfGet(device,
        NULL,
        deframerBaseAddr,
        &deframerCfg->newSysrefOnRelink);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sysref on relink enable");
        return recoveryAction;
    }

    /* get sysrefForStartup */
    recoveryAction = adrv903x_JrxLink_JrxCoreSysrefForStartup_BfGet(device,
        NULL,
        deframerBaseAddr,
        &deframerCfg->sysrefForStartup);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sysrefForStartup");
        return recoveryAction;
    }

    /* get sysrefNhot Enable*/
    recoveryAction = adrv903x_JrxLink_JrxCoreSysrefNShotEnable_BfGet(device,
        NULL,
        deframerBaseAddr,
        &deframerCfg->sysrefNShotEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sysrefNhot enable");
        return recoveryAction;
    }

    /* get sysrefNshotCount */
    recoveryAction = adrv903x_JrxLink_JrxCoreSysrefNShotCount_BfGet(device,
        NULL,
        deframerBaseAddr,
        &deframerCfg->sysrefNShotCount);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sysrefNshotCount");
        return recoveryAction;
    }

    /* get sysrefIgnoreWhenLinked*/
    recoveryAction = adrv903x_JrxLink_JrxCoreSysrefIgnoreWhenLinked_BfGet(device,
        NULL,
        deframerBaseAddr,
        &deframerCfg->sysrefIgnoreWhenLinked);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sysrefIgnoreWhenLinked");
        return recoveryAction;
    }

    deframerCfg->iqRate_kHz = device->initExtract.jesdSetting.deframerSetting[deframerIdx].iqRate_kHz;
    deframerCfg->laneRate_kHz = device->initExtract.jesdSetting.deframerSetting[deframerIdx].laneRate_kHz;

    return ADI_ADRV903X_ERR_ACT_NONE;
    
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_DacSampleXbarGet(adi_adrv903x_Device_t* const           device,
    const adi_adrv903x_DeframerSel_e       deframerSel,
    adi_adrv903x_DacSampleXbarCfg_t* const dacXbar)
{
    adi_adrv903x_ErrAction_e     recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfJrxLinkChanAddr_e deframerBaseAddr         = ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    uint8_t                      bfValue = 0U;
    uint8_t                      dacIdx = 0U;

            /* Get the base address of the selected deframer */
    recoveryAction = adrv903x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        return recoveryAction;
    }

    for (dacIdx = 0U; dacIdx < ADI_ADRV903X_NUM_DAC_SAMPLE_XBAR; dacIdx++)
    {
        /* Get the sample xbar converter */
        recoveryAction = adrv903x_JrxLink_JrxCoreConvSel_BfGet(device,
            NULL,
            deframerBaseAddr,
            dacIdx,
            &bfValue);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sample xbar converter.");
            return recoveryAction;
        }

        dacXbar->txDacChan[dacIdx] = (adi_adrv903x_DacSampleXbarSel_e) bfValue;
    }

        return ADI_ADRV903X_ERR_ACT_NONE;
    
}

