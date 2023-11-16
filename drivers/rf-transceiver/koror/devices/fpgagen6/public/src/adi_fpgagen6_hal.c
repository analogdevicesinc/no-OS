/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_hal.c
* \brief Contains ADI Transceiver Hardware Abstraction functions
*        Analog Devices maintains and provides updates to this code layer.
*        The end user should not modify this file or any code in this directory.
*
* FPGA API Version: 2.9.0.4
*/
#include "adi_fpgagen6_hal.h"
#include "adi_fpgagen6_hal_types.h"
#include "adi_platform.h"

#include "../../private/bf/fpgagen6_bf_axi_memory_pager.h"
#include "../../private/include/fpgagen6_data_interface.h"

#define ADI_FILE    ADI_FPGAGEN6_FILE_PUBLIC_HAL


ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RamRead(  adi_fpgagen6_Device_t* const    device,
                                                        const uint64_t                  ramAddress,
                                                        uint32_t                        data[],
                                                        const uint32_t                  length)
{
    adi_fpgagen6_ErrAction_e    recoveryAction      = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e               halError            = ADI_HAL_ERR_PARAM;
    //todo add loop for length > memory page size
    uint64_t endAddr = ramAddress + (length * sizeof(uint32_t));
    uint64_t currRamAddress = ramAddress;
    uint32_t remainingLength_words = length;
    uint32_t currLength_words = 0U;
    uint32_t dataOffset_words = 0U;

    /* ADS10 HBM memory = 8GB divided by pagesize with a max page size of 256MB */
    /* offset into a page accessed via devuio1 */
    uint64_t offsetAddr = ramAddress & (device->memoryPageSize - 1U); /* Mask off the upper bits in the addr addressed by Page ID */
    uint64_t pageSizeMask = ~((uint64_t)(device->memoryPageSize - 1U)); /* Mask off the lower bits */
    uint64_t pageId = ((uint64_t)ramAddress & pageSizeMask);
    uint64_t nextPageBoundary = pageId + device->memoryPageSize;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, data, cleanup);

    if (ramAddress % sizeof(uint32_t) != 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, ramAddress, recoveryAction, "ramAddress does not fall on a word boundary.");
        goto cleanup;
    }

    if (length == 0U)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
        goto cleanup;
    }

    recoveryAction = fpgagen6_AxiMemoryPager_PageSize_BfSet(device,
                                                            FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_MEMORY_PAGE,
                                                            (device->memoryPageSize - 1U));
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_HAL,
                                                ADI_FPGAGEN6_ERRCODE_HAL_RAM_READ,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    do
    {
        recoveryAction = fpgagen6_AxiMemoryPager_PageId_BfSet(  device,
                                                                FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_MEMORY_PAGE,
                                                                pageId);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_HAL,
                                                    ADI_FPGAGEN6_ERRCODE_HAL_RAM_READ,
                                                    pageId,
                                                    recoveryAction,
                                                    cleanup);
        }

        currLength_words = (uint32_t)((endAddr > nextPageBoundary) ? (nextPageBoundary - currRamAddress)  / sizeof(uint32_t) : remainingLength_words); /* If we cross a page boundary, only send the difference */

        halError = adi_hal_BbicRegistersRead(   device->common.devHalInfo,
                                                offsetAddr, 
                                                &data[dataOffset_words],
                                                currLength_words);
        if (ADI_HAL_ERR_OK != halError)
        {
            recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
            ADI_ERROR_REPORT(   &device->common,
                                ADI_COMMON_ERRSRC_DEVICEHAL,
                                halError,
                                recoveryAction,
                                offsetAddr,
                                "RAM Read HAL Error (Loop)");
            goto cleanup;
        }

        remainingLength_words   -= currLength_words;
        offsetAddr              = 0U;                                           /* Except for the first access, every access will start at offset 0 */
        currRamAddress          += currLength_words * sizeof(uint32_t);         /* Move to where the next access will point to in RAM */
        pageId                  = ((uint64_t)currRamAddress & pageSizeMask);    /* Calculate the pageId based off the physical address */
        dataOffset_words        += currLength_words;                            /* Move to the next block in the data array */
        nextPageBoundary        += device->memoryPageSize;
    } while (remainingLength_words > 0U);

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RamWrite( adi_fpgagen6_Device_t* const    device,
                                                        const uint64_t                  ramAddress,
                                                        const uint32_t                  data[],
                                                        const uint32_t                  length)
{
    adi_fpgagen6_ErrAction_e    recoveryAction      = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e               halError            = ADI_HAL_ERR_PARAM;
    //todo add loop for length > memory page size
    uint64_t endAddr = ramAddress + (length * sizeof(uint32_t));
    uint64_t currRamAddress = ramAddress;
    uint32_t remainingLength_words = length;
    uint32_t currLength_words = 0U;
    uint32_t dataOffset_words = 0U;

    // offset into a page accessed via devuio1
    uint64_t offsetAddr = ramAddress & (device->memoryPageSize - 1U); /* Mask off the upper bits in the addr addressed by Page ID */
    uint64_t pageSizeMask = ~((uint64_t)(device->memoryPageSize - 1U)); /* Mask off the lower bits */
    uint64_t pageId = ((uint64_t)ramAddress & pageSizeMask);
    uint64_t nextPageBoundary = pageId + device->memoryPageSize;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, data, cleanup);

    if (ramAddress % sizeof(uint32_t) != 0)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, ramAddress, recoveryAction, "ramAddress does not fall on a word boundary.");
        goto cleanup;
    }
    
    if (length == 0U)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
        goto cleanup;
    }
    
    recoveryAction = fpgagen6_AxiMemoryPager_PageSize_BfSet(device,
                                                            FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_MEMORY_PAGE,
                                                            (device->memoryPageSize - 1U));
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue Writing DRP Mask");
        goto cleanup;
    }
    
    do
    {
        recoveryAction = fpgagen6_AxiMemoryPager_PageId_BfSet(device,
                                                            FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_MEMORY_PAGE,
                                                            pageId);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_HAL,
                                                    ADI_FPGAGEN6_ERRCODE_HAL_RAM_WRITE,
                                                    pageId,
                                                    recoveryAction,
                                                    cleanup);
        }

        currLength_words = (uint32_t)((endAddr > nextPageBoundary) ? (nextPageBoundary - currRamAddress) / sizeof(uint32_t) : remainingLength_words); /* If we cross a page boundary, only send the difference */

        halError = adi_hal_BbicRegistersWrite(  device->common.devHalInfo,
                                                offsetAddr,
                                                &data[dataOffset_words],
                                                currLength_words);
        if (ADI_HAL_ERR_OK == halError)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
        }
        else
        {
            recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_ErrCodeConvert(halError) ;
            ADI_ERROR_REPORT(   &device->common,
                                ADI_COMMON_ERRSRC_DEVICEHAL,
                                halError,
                                recoveryAction,
                                offsetAddr,
                                "RAM Write HAL Error");
            goto cleanup;
        }

        remainingLength_words   -= currLength_words;
        offsetAddr              = 0U;                                           /* Except for the first access, every access will start at offset 0 */
        currRamAddress          += currLength_words * sizeof(uint32_t);         /* Move to where the next access will point to in RAM */
        pageId                  = ((uint64_t)currRamAddress & pageSizeMask);    /* Move to the next page */
        dataOffset_words        += currLength_words;                            /* Move to the next block in the data array */
        nextPageBoundary        += device->memoryPageSize;
    } while (remainingLength_words > 0U);

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RegisterRead( adi_fpgagen6_Device_t* const    device,
                                                            const uint32_t                  addr,
                                                            uint32_t* const                 data)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e               halError        = ADI_HAL_ERR_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    halError = adi_hal_BbicRegisterRead(device->common.devHalInfo, addr, data);
    if (ADI_HAL_ERR_OK == halError)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
    }
    else
    {
        recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_ErrCodeConvert(halError) ;
        ADI_ERROR_REPORT(   &device->common,
                            ADI_COMMON_ERRSRC_DEVICEHAL,
                            halError,
                            recoveryAction,
                            addr,
                            "FPGA AXI Reg Read error");
        goto cleanup;
    }

    ADI_VARIABLE_LOG(&device->common, ADI_HAL_LOG_SPI, "BBIC Register Read (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32")", addr, *data);

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RegistersRead( adi_fpgagen6_Device_t* const    device,
    const uint64_t                  ramAddress,
    uint32_t                        data[],
    const uint32_t                  length)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e               halError        = ADI_HAL_ERR_PARAM;
    int32_t registersReadLoopLimit = (length + BBIC_RAMARRAYSIZE - 1U) / BBIC_RAMARRAYSIZE;
    int32_t i = 0U;
    uint64_t dataOffset = 0U;
    uint64_t currAddr = ramAddress;
    uint32_t currLength = BBIC_RAMARRAYSIZE;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, data, cleanup);

    for (i = 0U; i < registersReadLoopLimit; i++)
    {
        if ((dataOffset + currLength) > length)
        {
            currLength = length - dataOffset;
        }
        else
        {
            currLength = BBIC_RAMARRAYSIZE;
        }
        
        halError = adi_hal_BbicRegistersRead(   device->common.devHalInfo,
                                                currAddr, 
                                                &data[dataOffset],
                                                currLength);
        if (ADI_HAL_ERR_OK != halError)
        {
            recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
            ADI_ERROR_REPORT(   &device->common,
                ADI_COMMON_ERRSRC_DEVICEHAL,
                halError,
                recoveryAction,
                ramAddress,
                "RAM Read HAL Error (Loop)");
            goto cleanup;
        }

        ADI_VARIABLE_LOG(&device->common, ADI_HAL_LOG_SPI, "BBIC Registers Read (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32")", ramAddress, *data);
        
        dataOffset += currLength;
        currAddr += currLength;
    }
    
    recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RegisterWrite(adi_fpgagen6_Device_t* const    device,
                                                            const uint32_t                  addr,
                                                            const uint32_t                  data)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e               halError        = ADI_HAL_ERR_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    halError = adi_hal_BbicRegisterWrite(device->common.devHalInfo, addr, data);
    if (ADI_HAL_ERR_OK == halError)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
    }
    else
    {
        recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_ErrCodeConvert(halError) ;
        ADI_ERROR_REPORT(   &device->common,
                            ADI_COMMON_ERRSRC_DEVICEHAL,
                            halError,
                            recoveryAction,
                            addr,
                            "FPGA Reg write error");
        goto cleanup;
    }

    ADI_VARIABLE_LOG(&device->common, ADI_HAL_LOG_SPI, "BBIC Register Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32")", addr, data);

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RegistersWrite(adi_fpgagen6_Device_t* const    device,
    const uint64_t                  ramAddress,
    const uint32_t                  data[],
    const uint32_t                  length)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e               halError        = ADI_HAL_ERR_PARAM;
    int32_t registersReadLoopLimit = (length + BBIC_RAMARRAYSIZE - 1U) / BBIC_RAMARRAYSIZE;
    int32_t i = 0U;
    uint64_t dataOffset = 0U;
    uint64_t currAddr = ramAddress;
    uint32_t currLength = BBIC_RAMARRAYSIZE;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, data, cleanup);

    for (i = 0U; i < registersReadLoopLimit; i++)
    {
        if ((dataOffset + currLength) > length)
        {
            currLength = length - dataOffset;
        }
        else
        {
            currLength = BBIC_RAMARRAYSIZE;
        }
        
        halError = adi_hal_BbicRegistersWrite(  device->common.devHalInfo,
            currAddr,
            &data[dataOffset],
            currLength);
        if (ADI_HAL_ERR_OK != halError)
        {
            recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
            ADI_ERROR_REPORT(   &device->common,
                ADI_COMMON_ERRSRC_DEVICEHAL,
                halError,
                recoveryAction,
                ramAddress,
                "RAM Read HAL Error (Loop)");
            goto cleanup;
        }

        ADI_VARIABLE_LOG(&device->common, ADI_HAL_LOG_SPI, "BBIC Registers Read (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32")", ramAddress, *data);
        
        dataOffset += currLength;
        currAddr += currLength;
    }
    
    recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RegisterFieldRead(adi_fpgagen6_Device_t* const    device,
                                                                uint32_t                        addr,
                                                                uint32_t* const                 fieldVal,
                                                                uint32_t                        mask,
                                                                uint8_t                         startBit)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t                    rdData          = 0U;
    uint32_t                    regVal          = 0U;
    uint32_t                    byteShiftVal    = addr - (addr & ~0x3U);

    addr = (addr & ~0x3U);
    mask = mask << (8U * byteShiftVal);
    startBit = startBit + (8U * byteShiftVal);

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, fieldVal, cleanup);

    recoveryAction = adi_fpgagen6_RegisterRead(device, addr, &rdData);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_HAL,
                                                ADI_FPGAGEN6_ERRCODE_HAL_REG_FIELD_READ,
                                                addr,
                                                recoveryAction,
                                                cleanup);
    }

    regVal    = rdData;
    *fieldVal = (regVal & mask) >> startBit;

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RegisterFieldWrite(   adi_fpgagen6_Device_t* const    device,
                                                                    uint32_t                        addr,
                                                                    const uint32_t                  fieldVal,
                                                                    uint32_t                        mask,
                                                                    uint8_t                         startBit)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t                    regVal          = 0U;
    uint32_t                    registerVal     = 0U;
    uint32_t                    newRegVal       = 0U;
    uint32_t                    byteShiftVal    = addr - (addr & ~0x3U);

    addr = (addr & ~0x3U);
    mask = mask << (8U * byteShiftVal);
    startBit = startBit + (8U * byteShiftVal);
    regVal = ((fieldVal << startBit) & mask);

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    /* Immediate Read Modify Write  */
    recoveryAction = adi_fpgagen6_RegisterRead(device, addr, &registerVal);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_HAL,
                                                ADI_FPGAGEN6_ERRCODE_HAL_REG_FIELD_WRITE,
                                                addr,
                                                recoveryAction,
                                                cleanup);
    }

    /* modify the register value */
    newRegVal = (registerVal & ~mask) | regVal;

    /* write to the register with modified value */
    recoveryAction = adi_fpgagen6_RegisterWrite(device, addr, newRegVal);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_HAL,
                                                ADI_FPGAGEN6_ERRCODE_HAL_REG_FIELD_WRITE,
                                                addr,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}