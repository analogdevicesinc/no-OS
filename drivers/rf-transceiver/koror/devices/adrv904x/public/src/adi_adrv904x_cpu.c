/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_cpu.c
* \brief Contains CPU features related function implementation defined in
* adi_adrv904x_cpu.h
*
* ADRV904X API Version: 2.9.0.4
*/
#include "adi_adrv904x_cpu.h"

#include "../../private/include/adrv904x_cpu.h"
#include "../../private/include/adrv904x_cpu_scratch_registers.h"
#include "../../private/include/adrv904x_reg_addr_macros.h"
#include "../../private/include/adrv904x_init.h"
#include "../../private/include/adrv904x_cpu_sw_bkpt_types.h"
#include "../../private/include/adrv904x_gpio.h"
#include "../../private/include/adrv904x_cpu_dfe_init_memory.h"
#include "../../private/include/adrv904x_dfe_memory.h"
#include "../../private/include/adrv904x_dfe_framework_cmd_t.h"
#include "../../private/include/adrv904x_dfe_cpu.h"

#define ADI_FILE    ADI_ADRV904X_FILE_PUBLIC_CPU

static adi_adrv904x_ErrAction_e adrv904x_CpuStackPtrWrite(adi_adrv904x_Device_t* const    device,
                                                          const adi_adrv904x_CpuType_e    cpuType,
                                                          const uint32_t*                 buf);

static adi_adrv904x_ErrAction_e adrv904x_CpuBootAddrWrite(adi_adrv904x_Device_t* const    device,
                                                          const adi_adrv904x_CpuType_e    cpuType,
                                                          const uint32_t*                 buf);

static uint32_t adrv904x_ChannelToChannelId(const adi_adrv904x_Channels_e channel);

/*****************************************************************************/
/***** Helper functions' definition ******************************************/
/*****************************************************************************/
static adi_adrv904x_ErrAction_e adrv904x_CpuStackPtrWrite(adi_adrv904x_Device_t* const    device,
                                                          const adi_adrv904x_CpuType_e    cpuType,
                                                          const uint32_t*                 buf)
{
    adi_adrv904x_ErrAction_e        recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const adi_adrv904x_CpuAddr_t*   cpuAddr         = NULL;
    const uint8_t*                  bytes           = (const uint8_t*) buf;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    cpuAddr = &(device->devStateInfo.cpu.cpuAddr[cpuType]);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cpuAddr);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, buf);

    ADRV904X_SPIWRITEBYTE_RETURN("CPU_STACK_PTR_BYTE_0", cpuAddr->stackPtrAddr, bytes[0U], recoveryAction);
    ADRV904X_SPIWRITEBYTE_RETURN("CPU_STACK_PTR_BYTE_1", cpuAddr->stackPtrAddr + 1u, bytes[1U], recoveryAction);
    ADRV904X_SPIWRITEBYTE_RETURN("CPU_STACK_PTR_BYTE_2", cpuAddr->stackPtrAddr + 2u, bytes[2U], recoveryAction);
    ADRV904X_SPIWRITEBYTE_RETURN("CPU_STACK_PTR_BYTE_3", cpuAddr->stackPtrAddr + 3u, bytes[3U], recoveryAction);

    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_CpuBootAddrWrite(adi_adrv904x_Device_t* const    device,
                                                          const adi_adrv904x_CpuType_e    cpuType,
                                                          const uint32_t*                 buf)
{
    adi_adrv904x_ErrAction_e  recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_CpuAddr_t* cpuAddr         = NULL;
    const uint8_t*          bytes           = (const uint8_t*) buf;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, buf);

    cpuAddr = &(device->devStateInfo.cpu.cpuAddr[cpuType]);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cpuAddr);

    ADRV904X_SPIWRITEBYTE_RETURN("CPU_BOOT_ADDR_BYTE_0", cpuAddr->bootAddr, bytes[0U], recoveryAction);
    ADRV904X_SPIWRITEBYTE_RETURN("CPU_BOOT_ADDR_BYTE_1", cpuAddr->bootAddr + 1u, bytes[1U], recoveryAction);
    ADRV904X_SPIWRITEBYTE_RETURN("CPU_BOOT_ADDR_BYTE_2", cpuAddr->bootAddr + 2u, bytes[2U], recoveryAction);
    ADRV904X_SPIWRITEBYTE_RETURN("CPU_BOOT_ADDR_BYTE_3", cpuAddr->bootAddr + 3u, bytes[3U], recoveryAction);

    return recoveryAction;
}

static uint32_t adrv904x_ChannelToChannelId(const adi_adrv904x_Channels_e channel)
{
    uint32_t div = 0U;

    for (div = 0U; div < 8U; div++)
    {
        if ((uint32_t)channel == (1U << div))
        {
            break;
        }
    }

    return div;
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuImageWrite(adi_adrv904x_Device_t* const    device,
                                                            const adi_adrv904x_CpuType_e    cpuType,
                                                            const uint32_t                  byteOffset,
                                                            const uint8_t                   binary[],
                                                            const uint32_t                  byteCount)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_CpuAddr_t *cpuAddr = NULL;
    uint32_t address[1] = { 0U };
    uint32_t debugOffset = 0U;
    uint32_t profileOffset = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, binary, cleanup);

     /* Verify cpuType */
    if ((cpuType != ADI_ADRV904X_CPU_TYPE_0) &&
            (cpuType != ADI_ADRV904X_CPU_TYPE_1))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuType, "Invalid CPU type");
        goto cleanup;
    }

    if ((byteCount == 0U) ||
        ((byteCount & 3U) > 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, byteCount, "Invalid Byte Count; Multiple of 4 Required");
        goto cleanup;
    }

    if ((byteOffset & 3U) > 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, byteCount, "Invalid Byte Offset; Multiple of 4 Required");
        goto cleanup;
    }

    /* extraction of stack pointer and boot address from top of array */
    if (byteOffset == 0U)
    {
        if (byteCount < 8U)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, byteCount, "Invalid Byte Count; Minimum of 8 Required");
            goto cleanup;
        }

        recoveryAction = adrv904x_CpuStackPtrWrite(device, cpuType, (const uint32_t *)binary);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Stack Pointer Write Issue");
            goto cleanup;
        }

        recoveryAction = adrv904x_CpuBootAddrWrite(device, cpuType, (const uint32_t *)&binary[4U]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Boot Address Write Issue");
            goto cleanup;
        }
    }

    if (cpuType == ADI_ADRV904X_CPU_TYPE_1)
    {
        /* extraction of profile address */
        profileOffset = ADRV904X_PM_DEVICE_PROFILE_PTR & ADI_ADRV904X_CPU_0_BINARY_IMAGE_FILE_SIZE_MASK;
        if ((profileOffset >= byteOffset) &&
            (profileOffset < (byteOffset + byteCount - 4U)))
        {
            device->devStateInfo.cpu.devProfileAddr = adrv904x_CpuIntFromBytesGet(binary + profileOffset - byteOffset, 4U);
        }
    }

    if (cpuType == ADI_ADRV904X_CPU_TYPE_0)
    {
        /* extraction of debug config address */
        debugOffset = ADRV904X_PM_DEBUG_CONFIG_BUF & ADI_ADRV904X_CPU_0_BINARY_IMAGE_FILE_SIZE_MASK;
        if ((debugOffset >= byteOffset) &&
            (debugOffset < (byteOffset + byteCount - 4U)))
        {
            device->devStateInfo.cpu.devDebugConfigAddr = adrv904x_CpuIntFromBytesGet(binary + debugOffset - byteOffset, 4U);
        }

#if ADI_ADRV904X_API_VERSION_RANGE_CHECK > 0

        /* extraction of rev data */
        {
            uint32_t revOffset = 0U;
            adi_adrv904x_Version_t minVersion = { 0U, 0U, 0U, 0U };

                        revOffset = ADRV904X_PM_DEVICE_REV_DATA & ADI_ADRV904X_CPU_0_BINARY_IMAGE_FILE_SIZE_MASK;
            if ((revOffset >= byteOffset) &&
                (revOffset < (byteOffset + byteCount - 4U)))
            {
                device->devStateInfo.cpu.fwVersion.majorVer = adrv904x_CpuIntFromBytesGet(binary + revOffset - byteOffset, 4U);
            }

            revOffset += 4U;
            if ((revOffset >= byteOffset) &&
                (revOffset < (byteOffset + byteCount - 4U)))
            {
                device->devStateInfo.cpu.fwVersion.minorVer = adrv904x_CpuIntFromBytesGet(binary + revOffset - byteOffset, 4U);
            }

            revOffset += 4U;
            if ((revOffset >= byteOffset) &&
                (revOffset < (byteOffset + byteCount - 4U)))
            {
                device->devStateInfo.cpu.fwVersion.maintenanceVer = adrv904x_CpuIntFromBytesGet(binary + revOffset - byteOffset, 4U);
            }

            revOffset += 4U;
            if ((revOffset >= byteOffset) &&
                (revOffset < (byteOffset + byteCount - 4U)))
            {
                device->devStateInfo.cpu.fwVersion.buildVer = adrv904x_CpuIntFromBytesGet(binary + revOffset - byteOffset, 4U);

                recoveryAction = adi_adrv904x_ApiVersionGet(device, &minVersion);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "ApiVersionGet issue");
                    goto cleanup;
                }

                recoveryAction = adrv904x_ApiVersionRangeCheck(device, &device->devStateInfo.cpu.fwVersion, &minVersion, &minVersion);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "ApiVersionRangeCheck issue");
                    goto cleanup;
                }
            }
        }
        #endif
    }

    /* get the particular processor's address map */
    cpuAddr = &(device->devStateInfo.cpu.cpuAddr[cpuType]);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuAddr, cleanup);

    address[0U] = cpuAddr->progStartAddr + byteOffset;

            recoveryAction = adi_adrv904x_RegistersByteWrite(device,
                                                         NULL,
                                                         address[0],
                                                         binary,
                                                         byteCount);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register Write Issue");
            goto cleanup;
        }

        /* mark the image loaded */
    cpuAddr->enabled = ADI_TRUE;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuProfileWrite(adi_adrv904x_Device_t* const    device,
                                                              const uint32_t                  byteOffset,
                                                              const uint8_t                   binary[],
                                                              const uint32_t                  byteCount)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t address[1] = { device->devStateInfo.cpu.devProfileAddr + byteOffset };
    uint32_t profileByteCount = 0U;

    uint32_t dfeProfileTempAddr = ADRV904X_DFE_CPU_INIT_ADDR_GET(deviceProfile);
    address[0U] = dfeProfileTempAddr + byteOffset;	

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, binary, cleanup);

    if((byteCount == 0U) ||
        ((byteCount & 3U) > 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, byteCount, "Invalid Byte Count; Multiple of 4 Required");
        goto cleanup;
    }

    if ((byteOffset & 3U) > 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, byteCount, "Invalid Byte Offset; Multiple of 4 Required");
        goto cleanup;
    }

#if ADI_ADRV904X_API_VERSION_RANGE_CHECK > 0
    /* extraction of rev data */
    {
        uint32_t revOffset = 0U;
        adi_adrv904x_Version_t minVersion = { 0U, 0U, 0U, 0U };

                if ((revOffset >= byteOffset) &&
            (revOffset < (byteOffset + byteCount - 4U)))
        {
            device->devStateInfo.cpu.devProfileVersion.majorVer = adrv904x_CpuIntFromBytesGet(binary + revOffset - byteOffset, 4U);
        }

        revOffset += 4U;
        if ((revOffset >= byteOffset) &&
            (revOffset < (byteOffset + byteCount - 4U)))
        {
            device->devStateInfo.cpu.devProfileVersion.minorVer = adrv904x_CpuIntFromBytesGet(binary + revOffset - byteOffset, 4U);
        }

        revOffset += 4U;
        if ((revOffset >= byteOffset) &&
            (revOffset < (byteOffset + byteCount - 4U)))
        {
            device->devStateInfo.cpu.devProfileVersion.maintenanceVer = adrv904x_CpuIntFromBytesGet(binary + revOffset - byteOffset, 4U);
        }

        revOffset += 4U;
        if ((revOffset >= byteOffset) &&
            (revOffset < (byteOffset + byteCount - 4U)))
        {
            device->devStateInfo.cpu.devProfileVersion.buildVer = adrv904x_CpuIntFromBytesGet(binary + revOffset - byteOffset, 4U);


            recoveryAction = adi_adrv904x_ApiVersionGet(device, &minVersion);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Device Profile Version Get Issue");
                goto cleanup;
            }

            recoveryAction = adrv904x_ApiVersionRangeCheck(device, &device->devStateInfo.cpu.devProfileVersion, &minVersion, &minVersion);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Device Profile Version Range Check Issue");
                goto cleanup;
            }
        }
    }
#endif

    profileByteCount = sizeof(adrv904x_DeviceProfile_t);
    if (byteOffset + byteCount >  profileByteCount)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, byteCount, "Invalid Byte Count; Must be equal or less than Device Profile");
        goto cleanup;
    }
    else
    {
        profileByteCount = byteCount;
    }

            recoveryAction = adi_adrv904x_RegistersByteWrite(device,
                                                         NULL,
                                                         address[0],
                                                         binary,
                                                         byteCount);
    
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register Write Issue");
            goto cleanup;
        }
    cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuStart(adi_adrv904x_Device_t* const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_CpuAddr_t *cpuAddr = NULL;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Set the start bit for CPU 0. CPU 0 will then start CPU 1 as required */
    cpuAddr = &(device->devStateInfo.cpu.cpuAddr[ADI_ADRV904X_CPU_TYPE_0]);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuAddr, cleanup);

    if (cpuAddr->enabled != ADI_FALSE)
    {
                /* Reset Mailbox */
        recoveryAction = adi_adrv904x_Register32Write( device, NULL, cpuAddr->mailboxGetAddr, 0xFFFFFFFFU, 0xFFFFFFFFU);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Mailbox Reset Issue");
            goto cleanup;
        }

        /* Clear Masking of Bus Errors before starting core */
        /* Using Direct SPI Write instead of AHB */
        recoveryAction = adrv904x_Core_Arm0MemHrespMask_BfSet(device,
                                                              NULL,
                                                              (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                              ADI_DISABLE);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU 0 Clear Mask Bus Error Issue");
            goto cleanup;
        }

        /* Set CPU0 as the primary CPU (and CPU1 as secondary) */
        recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        ADRV904X_CPU_CPU0_IS_PRIMARY,
                                                        0x1U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Set CPU0 primary bitfield issue");
            goto cleanup;
        }
        recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        ADRV904X_CPU_CPU1_IS_PRIMARY,
                                                        0x0U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Set CPU1 primary bitfield issue");
            goto cleanup;
        }

        /* Start */
        /* Using Direct SPI Write instead of AHB */
        recoveryAction = adrv904x_Core_Arm0M3Run_BfSet(device,
                                                       NULL,
                                                       (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                       ADI_ENABLE);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU 0 Start Issue");
            goto cleanup;
        }
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuAddr->enabled, "CPU 0 FW image must be loaded");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuStartStatusCheck(adi_adrv904x_Device_t* const    device,
                                                                  const uint32_t                  timeout_us)
{
        adi_adrv904x_ErrAction_e    recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuBootStatus_e    bootStatus      = ADRV904X_CPU_BOOT_STATUS_POWER_UP;
    uint32_t                    waitInterval_us = 0U;
    uint32_t                    numEventChecks  = 1U;
    uint32_t                    eventCheck      = 0U;
    uint8_t                     cpuDebugLoaded  = (device->devStateInfo.devState & ADI_ADRV904X_STATE_CPUDEBUGLOADED) ? 1U : 0U;
    adi_adrv904x_CpuAddr_t*     cpuAddr         = NULL;
    uint8_t                     tmpByte         = 0U;
    uint8_t                     bootFlag        = 0U;
    uint32_t                    errAddress      = 0U;
    uint32_t                    productId       = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Determine the number of reads equivalent to the requested timeout */
    waitInterval_us = (ADI_ADRV904X_GETCPUBOOTUP_INTERVAL_US > timeout_us) ? timeout_us : ADI_ADRV904X_GETCPUBOOTUP_INTERVAL_US;
    numEventChecks = (waitInterval_us == 0U) ? 1U : (timeout_us / waitInterval_us);

    /* Get CPU0's address map */
    cpuAddr = adrv904x_CpuAddrGet(&device->devStateInfo.cpu, ADI_ADRV904X_CPU_TYPE_0);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuAddr, cleanup);

    if (cpuAddr->enabled == ADI_FALSE)
    {
        /* The application has not called adi_adrv904x_CpuImageWrite successfully first */
        ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_CPU,
                                            ADI_ADRV904X_ERRCODE_CPU_IMAGE_NOT_LOADED,
                                            ADI_NO_VARIABLE,
                                            recoveryAction);
        goto cleanup;
    }

    /* Note: There's no need to check CPU1 boot status. CPU0 will wait for CPU1 to boot before reporting
     * it's own boot status as successful.
     */

    for (eventCheck = 0U; eventCheck <= numEventChecks; eventCheck++)
    {
        if (eventCheck == 0U)
        {
            /* Check CPU Boot has started */
            recoveryAction = adrv904x_Core_Arm0M3Run_BfGet(device,
                                                           NULL,
                                                           (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                           &bootFlag);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
                goto cleanup;
            }

            if (bootFlag == ADI_FALSE)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, bootFlag, "CPU 0 Boot Not Started; PreMcsInit NOK");
                goto cleanup;
            }
        }

        /* Get the CPU boot status */
        recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                         ADRV904X_CPU_CPU0_BOOT_STATUS_SCRATCH_REG_ID,
                                                         &tmpByte);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "bootStatus read failed");
            goto cleanup;
        }

        bootStatus = (adrv904x_CpuBootStatus_e)tmpByte;
      
        if (bootStatus == ADRV904X_CPU_BOOT_STATUS_POWER_UP || bootStatus == ADRV904X_CPU_BOOT_WAIT_FOR_CPUS)
        {
            /* Wait interval then check again */
            recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, waitInterval_us);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Wait#1 failed");
                goto cleanup;
            }
            continue;
        }
        else if ((cpuDebugLoaded == 1U) && (bootStatus == ADRV904X_CPU_JTAG_BUILD_STATUS_READY))
        {
            /* Wait interval then check again */
            recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, waitInterval_us);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Wait#2 failed");
                goto cleanup;
            }
            continue;
        }
        else if ((cpuDebugLoaded == 0U) && (bootStatus == ADRV904X_CPU_JTAG_BUILD_STATUS_READY))
        {
            /* CPU booted successfully into debug mode */
            device->devStateInfo.devState = (adi_adrv904x_ApiStates_e) (device->devStateInfo.devState | ADI_ADRV904X_STATE_CPUDEBUGLOADED);
            break;
        }
        else if (bootStatus == ADRV904X_CPU_STATUS_READY)
        {
            /* CPU booted successfully to 'normal' mode */
            device->devStateInfo.devState = (adi_adrv904x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV904X_STATE_ALLCPUSLOADED);
            break;
        }
        else
        {
            if (bootStatus == ADRV904X_CPU_BOOT_PID_PROFILE_MISMATCH_ERR)
            {
                /* Get Product ID for Additional Debug */
                if (ADI_ADRV904X_ERR_ACT_NONE == adi_adrv904x_Register32Read(   device,
                                                                                NULL,
                                                                                ADRV904X_CPU_0_PM_EFUSE_SETTINGS_PTR,
                                                                                &errAddress,
                                                                                0xFFFFFFFFU))
                {
                    (void) adi_adrv904x_Registers32Read(device,
                                                        NULL,
                                                        errAddress,
                                                        (uint32_t*) &productId,
                                                        NULL,
                                                        1U);
                    productId = (uint8_t)productId;
                }

                ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_CPU_BOOT,
                                                    bootStatus,
                                                    productId,
                                                    recoveryAction);
            }
            else
            {
                ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_CPU_BOOT,
                                                    bootStatus,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
            }

            goto debug;
        }
    }

    if (eventCheck > numEventChecks)
    {
        /* Boot Timeout */
        ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_CPU,
                                            ADI_ADRV904X_ERRCODE_CPU_BOOT_TIMEOUT,
                                            ADI_NO_VARIABLE,
                                            recoveryAction);
        goto debug;
    }

debug:
    adrv904x_CpuErrorDebugCheck(device);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuConfigSet( adi_adrv904x_Device_t* const    device,
                                                            const uint32_t                  objId,
                                                            const uint16_t                  offset,
                                                            const uint8_t                   configDataSet[],
                                                            const uint32_t                  length)
{
        adi_adrv904x_ErrAction_e            recoveryAction                                      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    char                                txBuf[sizeof(adrv904x_CpuCmd_SetConfigMaxSize_t)];
    adrv904x_CpuCmd_SetConfig_t* const  setInfo                                             = (adrv904x_CpuCmd_SetConfig_t *)&txBuf;
    adrv904x_CpuCmd_SetConfigResp_t     cmdRsp                                              = { ADRV904X_CPU_SYSTEM_SIMULATED_ERROR };
    adrv904x_CpuCmdStatus_e             cmdStatus                                           = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e             cpuErrorCode                                        = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                            i                                                   = 0U;

    adi_adrv904x_CpuType_e              cpuType[] = { ADI_ADRV904X_CPU_TYPE_0, ADI_ADRV904X_CPU_TYPE_1 };

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, configDataSet, cleanup);

    if ((length > MAX_CONFIG_DATA_SIZE) ||
         (length == 0U))
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Length is zero or greater than MAX_CONFIG_DATA_SIZE");
        goto cleanup;
    }

    ADI_LIBRARY_MEMSET(&txBuf[0U], 0, sizeof(txBuf));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv904x_CpuCmd_SetConfigResp_t));

    /* Prepare the command payload */
    setInfo->objId = ADRV904X_HTOCL(objId);
    setInfo->offset = ADRV904X_HTOCS(offset);
    setInfo->length = ADRV904X_HTOCS((uint16_t)length);

    ADI_LIBRARY_MEMCPY((void*)((uint8_t*)setInfo + sizeof(adrv904x_CpuCmd_SetConfig_t)), configDataSet, length) ;

    for (i = 0U; i < (uint32_t)ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++i)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                cpuType[i],
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_SET_CONFIG,
                                                (void*)setInfo,
                                                sizeof(adrv904x_CpuCmd_SetConfig_t)+length,
                                                (void*)&cmdRsp,
                                                sizeof(cmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuConfigGet( adi_adrv904x_Device_t* const    device,
                                                            const uint32_t                  objId,
                                                            const uint16_t                  offset,
                                                            uint8_t                         configDataGet[],
                                                            const uint32_t                  length)
{
        adi_adrv904x_ErrAction_e                recoveryAction                                      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_GetConfig_t             getInfo;
    uint8_t                                 rxBuf[sizeof(adrv904x_CpuCmd_GetConfigMaxSize_t)];
    adrv904x_CpuCmd_GetConfigResp_t* const  getConfigRsp                                        = (adrv904x_CpuCmd_GetConfigResp_t*)&rxBuf;
    adrv904x_CpuCmdStatus_e                 cmdStatus                                           = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e                 cpuErrorCode                                        = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, configDataGet, cleanup);

    if (length > MAX_CONFIG_DATA_SIZE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Length is greater than MAX_CONFIG_DATA_SIZE");
        goto cleanup;
    }

    ADI_LIBRARY_MEMSET(&rxBuf[0U], 0, sizeof(rxBuf));
    ADI_LIBRARY_MEMSET(&getInfo, 0, sizeof(adrv904x_CpuCmd_GetConfig_t));

    /* Prepare the command payload */
    getInfo.objId = ADRV904X_HTOCL(objId);
    getInfo.offset = ADRV904X_HTOCS(offset);
    getInfo.length = ADRV904X_HTOCS((uint16_t)length);

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(   device,
                                            ADI_ADRV904X_CPU_TYPE_0,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_CPU_CMD_ID_GET_CONFIG,
                                            (void*)&getInfo,
                                            sizeof(getInfo),
                                            (void*)getConfigRsp,
                                            sizeof(adrv904x_CpuCmd_GetConfigResp_t) + length,
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(getConfigRsp->cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* Extract the command-specific response from the response payload */
    ADI_LIBRARY_MEMCPY((void*)configDataGet, (void*)((uint8_t*)getConfigRsp + sizeof(adrv904x_CpuCmd_GetConfigResp_t)), length);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuDebugModeEnable(   adi_adrv904x_Device_t* const    device,
                                                                    const uint32_t                  enableKey)
{
        adi_adrv904x_ErrAction_e                recoveryAction                      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_EnterDebugMode_t        setInfo                             = { 0 };
    adrv904x_CpuCmd_EnterDebugModeResp_t    cmdRsp                              = { ADRV904X_CPU_SYSTEM_SIMULATED_ERROR };
    adrv904x_CpuCmdStatus_e                 cmdStatus                           = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e                 cpuErrorCode                        = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                                i                                   = 0U;
    adi_adrv904x_CpuType_e                  cpuType[ADI_ADRV904X_CPU_TYPE_MAX_RADIO] = { ADI_ADRV904X_CPU_TYPE_0, ADI_ADRV904X_CPU_TYPE_1 };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Prepare the command payload */
    setInfo.debugModeKey = ADRV904X_HTOCL(enableKey);

    for (i = 0U; i < (uint32_t)ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++i)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                cpuType[i],
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_ENTER_DEBUG_MODE,
                                                (void*)&setInfo,
                                                sizeof(setInfo),
                                                (void*)&cmdRsp,
                                                sizeof(cmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuConfigUnlock(  adi_adrv904x_Device_t* const    device,
                                                                const uint32_t                  configKey)
{
        adi_adrv904x_ErrAction_e            recoveryAction                      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_UnlockConfig_t      setInfo                             = { 0 };
    adrv904x_CpuCmd_UnlockConfigResp_t  cmdRsp                              = { ADRV904X_CPU_SYSTEM_SIMULATED_ERROR };
    adrv904x_CpuCmdStatus_e             cmdStatus                           = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e             cpuErrorCode                        = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                            i                                   = 0U;
    adi_adrv904x_CpuType_e              cpuType[ADI_ADRV904X_CPU_TYPE_MAX_RADIO]  = { ADI_ADRV904X_CPU_TYPE_0, ADI_ADRV904X_CPU_TYPE_1 };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Prepare the command payload */
    setInfo.configKey = ADRV904X_HTOCL(configKey);

    for (i = 0U; i < (uint32_t)ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++i)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                cpuType[i],
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_UNLOCK_CONFIG,
                                                (void*)&setInfo,
                                                sizeof(setInfo),
                                                (void*)&cmdRsp,
                                                sizeof(cmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuControlCmdExec(adi_adrv904x_Device_t* const    device,
                                                                const uint32_t                  objId,
                                                                const uint16_t                  cpuCmd,
                                                                const adi_adrv904x_Channels_e   channel,
                                                                const uint8_t                   cpuCtrlData[],
                                                                const uint32_t                  lengthSet,
                                                                uint32_t* const                 lengthResp,
                                                                uint8_t                         ctrlResp[],
                                                                const uint32_t                  lengthGet)
{

        adi_adrv904x_ErrAction_e                recoveryAction                                      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    char                                    txBuf[sizeof(adrv904x_CpuCmd_SetCtrlMaxSize_t)];
    char                                    rxBuf[sizeof(adrv904x_CpuCmd_SetCtrlRespMaxSize_t)];
    adrv904x_CpuCmd_SetCtrl_t* const        setInfo                                             = (adrv904x_CpuCmd_SetCtrl_t*)&txBuf;
    adrv904x_CpuCmd_SetCtrlResp_t* const    cmdRsp                                              = (adrv904x_CpuCmd_SetCtrlResp_t*)&rxBuf;
    adrv904x_CpuCmdStatus_e                 cmdStatus                                           = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e                 cpuErrorCode                                        = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                                fwChannel                                           = 0U;
    adi_adrv904x_CpuType_e                  cpuType                                             = ADI_ADRV904X_CPU_TYPE_UNKNOWN;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    if (lengthSet > 0U)
    {
        ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuCtrlData, cleanup);
    }
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, ctrlResp, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, lengthResp, cleanup);

    if (objId > 0xFFU)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, objId, "Ctrl command - Invalid Object ID.");
        goto cleanup;
    }

    /* Allow lengthSet 0 for some use cases (RX_QEC) */
    if (lengthSet > MAX_CTRL_DATA_SIZE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, lengthSet, "Ctrl command length is greater than MAX_CTRL_DATA_SIZE");
        goto cleanup;
    }

    if (lengthGet <= 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, lengthGet, "Ctrl command response buffer size must be greater than 0");
        goto cleanup;
    }

    /* Get the CPU that is responsible for the requested channel */
    recoveryAction = adrv904x_CpuChannelMappingGet(device,
                                                   channel,
                                                   (adrv904x_CpuObjectId_e)objId,
                                                   &cpuType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "Invalid channelMask provided");
        goto cleanup;
    }
    else
    {
        fwChannel = adrv904x_ChannelToChannelId(channel);
    }

    ADI_LIBRARY_MEMSET(&txBuf[0U], 0, sizeof(txBuf));
    ADI_LIBRARY_MEMSET(&rxBuf[0U], 0, sizeof(rxBuf));

    /* Prepare the command payload */
    setInfo->objId = ADRV904X_HTOCL(objId);
    setInfo->ctrlCmd = ADRV904X_HTOCS(cpuCmd);
    setInfo->channelNum = ADRV904X_HTOCL(fwChannel);
    setInfo->length = ADRV904X_HTOCS((uint16_t)lengthSet);

    if (lengthSet > 0U)
    {
        ADI_LIBRARY_MEMCPY((void*)((uint8_t*)setInfo + sizeof(adrv904x_CpuCmd_SetCtrl_t)), cpuCtrlData, lengthSet);
    }

    /* Send command and receive response.
     * Since we don't know how much data the CPU will send back,
     * we have to request MAX_CONFIG_DATA_SIZE bytes. We will only
     * copy out cmdRsp->length bytes of it below.
     */
    recoveryAction = adrv904x_CpuCmdSend(   device,
                                            cpuType,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_CPU_CMD_ID_SET_CTRL,
                                            (void*)setInfo,
                                            sizeof(adrv904x_CpuCmd_SetCtrl_t) + lengthSet,
                                            (void*)cmdRsp,
                                            sizeof(adrv904x_CpuCmd_SetCtrlResp_t) + MAX_CONFIG_DATA_SIZE,
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp->cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* Read the response data from the CPU, if the caller's buffer can hold it. */
    *lengthResp = ADRV904X_CTOHS(cmdRsp->length);
    if (*lengthResp <= lengthGet)
    {
        ADI_LIBRARY_MEMSET(ctrlResp, 0, lengthGet);
        ADI_LIBRARY_MEMCPY(ctrlResp, (void*)((uint8_t*)cmdRsp + sizeof(adrv904x_CpuCmd_SetCtrlResp_t)), *lengthResp);
    }
    else
    {
        *lengthResp = 0U;
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, lengthGet, "Ctrl Cmd Response size is greater than response buffer size");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuLogFilterSet(adi_adrv904x_Device_t* const                  device,
                                                              const adi_adrv904x_CpuLogEvent_e              eventFilter,
                                                              const adi_adrv904x_CpuLogCpuId_e              cpuIdFilter,
                                                              const adi_adrv904x_CpuLogObjIdFilter_t* const objIdFilter)
{
        adi_adrv904x_ErrAction_e        recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t                        cpuTypeIdx      = 0U;
    adrv904x_CpuCmd_SetLogFilters_t setLogCmd;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, objIdFilter, cleanup);

    ADI_LIBRARY_MEMSET(&setLogCmd, 0, sizeof(setLogCmd));

    if ((eventFilter < ADI_ADRV904X_CPU_LOG_EVENT_ERROR) ||
        (eventFilter > ADI_ADRV904X_CPU_LOG_EVENT_INVALID))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, eventFilter, "Invalid eventFilter provided.");
        goto cleanup;
    }

    /* cpuIdEventFilter == 0 is a valid parameter and is not checked here */
    if (((cpuIdFilter & ~ADI_ADRV904X_CPU_LOG_CPU_ID_ALL) != 0U) &&
         (cpuIdFilter != ADI_ADRV904X_CPU_LOG_CPU_ID_INVALID))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuIdFilter, "Invalid cpuIdFilter provided.");
        goto cleanup;
    }

    /* It is not possible to validate objIdFilter->objId (the list of Object IDs is sparse in the firmware) */
    if ((objIdFilter->objIdFilterEnable != ADI_ADRV904X_CPU_LOG_OBJ_ID_ENABLE) &&
        (objIdFilter->objIdFilterEnable != ADI_ADRV904X_CPU_LOG_OBJ_ID_DISABLE) &&
        (objIdFilter->objIdFilterEnable != ADI_ADRV904X_CPU_LOG_OBJ_ID_INVALID))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, objIdFilter->objIdFilterEnable, "Invalid objIdFilter.objIdFilterEnable provided.");
        goto cleanup;
    }

    /* Build the command */
    setLogCmd.logEventFilter = (adi_adrv904x_CpuLogEvent_t)eventFilter;
    setLogCmd.cpuIdFilter = (adi_adrv904x_CpuLogCpuId_t)cpuIdFilter;
    setLogCmd.objIdFilter.objIdFilterEnable = objIdFilter->objIdFilterEnable;
    setLogCmd.objIdFilter.objId = ADRV904X_HTOCL(objIdFilter->objId);

    /* For each CPU, send the set-log-filters command and verify the response */
    for (cpuTypeIdx = (uint32_t) ADI_ADRV904X_CPU_TYPE_0; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_SET_LOG_FILTERS,
                                                &setLogCmd,
                                                sizeof(setLogCmd),
                                                NULL,
                                                0,
                                                NULL);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Log Filters Command Send Issue");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuDebugCmdExec(  adi_adrv904x_Device_t* const    device,
                                                                const uint32_t                  objId,
                                                                const uint16_t                  cpuCmd,
                                                                const adi_adrv904x_Channels_e   channel,
                                                                const uint8_t                   cpuDebugData[],
                                                                const uint32_t                  lengthSet,
                                                                uint32_t* const                 lengthResp,
                                                                uint8_t                         debugResp[],
                                                                const uint32_t                  lengthGet)
{

        adi_adrv904x_ErrAction_e            recoveryAction                                      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    char                                txBuf[sizeof(adrv904x_CpuCmd_DebugMaxSize_t)];
    char                                rxBuf[sizeof(adrv904x_CpuCmd_DebugRespMaxSize_t)];
    adrv904x_CpuCmd_Debug_t* const      debugInfo                                           = (adrv904x_CpuCmd_Debug_t*)&txBuf;
    adrv904x_CpuCmd_DebugResp_t* const  cmdRsp                                              = (adrv904x_CpuCmd_DebugResp_t*)&rxBuf;
    adrv904x_CpuCmdStatus_e             cmdStatus                                           = ADRV904X_CPU_CMD_STATUS_NO_ERROR;
    adrv904x_CpuErrorCode_e             cpuErrorCode                                        = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                            fwChannel                                           = 0U;
    adi_adrv904x_CpuType_e              cpuType                                             = ADI_ADRV904X_CPU_TYPE_UNKNOWN;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuDebugData, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, debugResp, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, lengthResp, cleanup);

    if (objId > 0xFFU)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, objId, "Cpu Debug command - Invalid Object ID.");
        goto cleanup;
    }

    if (lengthSet > MAX_DEBUG_DATA_SIZE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, lengthSet, "Cpu Debug command length is greater than MAX_CTRL_DATA_SIZE");
        goto cleanup;
    }

    if (lengthSet <= 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, lengthSet, "Cpu Debug command length must be greater than 0");
        goto cleanup;
    }

    if (lengthGet <= 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, lengthGet, "Cpu Debug command response buffer size must be greater than 0");
        goto cleanup;
    }

    /* Get the CPU that is responsible for the requested channel */
    recoveryAction = adrv904x_CpuChannelMappingGet(device,
                                                   channel,
                                                   (adrv904x_CpuObjectId_e)objId,
                                                   &cpuType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "Invalid channelMask provided");
        goto cleanup;
    }
    else
    {
        fwChannel = adrv904x_ChannelToChannelId(channel);
    }

    ADI_LIBRARY_MEMSET(&txBuf[0U], 0, sizeof(txBuf));
    ADI_LIBRARY_MEMSET(&rxBuf[0U], 0, sizeof(rxBuf));

    /* Prepare the command payload */
    debugInfo->objId = ADRV904X_HTOCL(objId);
    debugInfo->debugCmd = ADRV904X_HTOCS(cpuCmd);
    debugInfo->channelNum = ADRV904X_HTOCL(fwChannel);
    debugInfo->length = ADRV904X_HTOCS((uint16_t)lengthSet);

    ADI_LIBRARY_MEMCPY((void*)((uint8_t*)debugInfo + sizeof(adrv904x_CpuCmd_Debug_t)), cpuDebugData, lengthSet);

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(   device,
                                            cpuType,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_CPU_CMD_ID_DEBUG,
                                            (void*)debugInfo,
                                            sizeof(adrv904x_CpuCmd_Debug_t)+lengthSet,
                                            (void*)cmdRsp,
                                            sizeof(adrv904x_CpuCmd_DebugResp_t),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp->cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    if (cmdRsp->length <= lengthGet)
    {
        ADI_LIBRARY_MEMSET(debugResp, 0, lengthGet);
        *lengthResp = cmdRsp->length;
        ADI_LIBRARY_MEMCPY(debugResp, ((adrv904x_CpuCmd_DebugMaxSize_t *)cmdRsp)->debugData, *lengthResp);
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, lengthGet, "Debug Cmd Response size is greater than response buffer size");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

static adi_adrv904x_ErrAction_e adrv904x_SendSysStatusCmd(adi_adrv904x_Device_t* const            device,
                                                          const adrv904x_CpuCmd_SysStatusType_e   type,
                                                          const adrv904x_CpuObjectId_e            sysObjId,
                                                          const adi_adrv904x_Channels_e           channel)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    char txBuf[ADRV904X_CPU_CMD_MAX_SIZE_BYTES];
    adrv904x_CpuCmd_t* pCmd = NULL;
    adrv904x_CpuCmd_GetSysStatus_t* pSysStatusCmd = NULL;
    adi_adrv904x_CpuType_e cpuType = ADI_ADRV904X_CPU_TYPE_UNKNOWN;

    /* Get the CPU that is responsible for the requested channel */
    recoveryAction = adrv904x_CpuChannelMappingGet(device, channel, sysObjId, &cpuType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid channel-to-CPU mapping");
        return recoveryAction;
    }

    ADI_LIBRARY_MEMSET(&txBuf, 0, sizeof(txBuf));

    /* Build the CPU command */
    pCmd = (adrv904x_CpuCmd_t*)txBuf;
    pSysStatusCmd = (adrv904x_CpuCmd_GetSysStatus_t*)((uint8_t*)pCmd + sizeof(adrv904x_CpuCmd_t));
    pSysStatusCmd->type = (adrv904x_CpuCmd_SysStatusType_t)type;
    pSysStatusCmd->sysObjId = ADRV904X_HTOCL(sysObjId);
    pSysStatusCmd->channelNum = ADRV904X_HTOCL(adrv904x_ChannelToChannelId(channel));

    /* Then send it */
    recoveryAction = adrv904x_CpuCmdWrite(device, cpuType, ADRV904X_LINK_ID_0, ADRV904X_CPU_CMD_ID_GET_SYS_STATUS, pCmd, sizeof(adrv904x_CpuCmd_GetSysStatus_t));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error sending GET_SYS_STATUS command.");
        return recoveryAction;
    }

    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_GetSysStatusCmdResp(adi_adrv904x_Device_t* const    device,
                                                             const adi_adrv904x_Channels_e   channel,
                                                             const size_t                    sysStatusSize,
                                                             void* const                     pRxBuf,
                                                             const void** const              pSysStatus)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmdId_t cmdId = 0U;
    adrv904x_CpuCmdResp_t* pCmd = (adrv904x_CpuCmdResp_t*)pRxBuf;
    adrv904x_CpuCmd_GetSysStatusResp_t* pCmdResp = (adrv904x_CpuCmd_GetSysStatusResp_t*)((uint8_t*)pCmd + sizeof(adrv904x_CpuCmdResp_t));
    adi_adrv904x_CpuType_e cpuType = ADI_ADRV904X_CPU_TYPE_UNKNOWN;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cmdErrorCode = ADRV904X_CPU_NO_ERROR;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, pRxBuf);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, pSysStatus);

    /* Get the CPU that is responsible for the requested channel */
    recoveryAction = adrv904x_CpuChannelMappingGet(device, channel, ADRV904X_CPU_OBJID_SYSTEM_END, &cpuType); /* use any objId other than ORX */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid channel-to-CPU mapping");
        return recoveryAction;
    }

    /* Read the response from the CPU */
    recoveryAction = adrv904x_CpuCmdRespRead(device, cpuType, ADRV904X_LINK_ID_0, &cmdId, pCmd, sizeof(adrv904x_CpuCmd_GetSysStatusResp_t) + sysStatusSize, &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        if (cmdStatus == ADRV904X_CPU_CMD_STATUS_CMD_FAILED)
        {
            /* If the command failed for a command-specific reason, extract the command status code and log the error. */
            cmdErrorCode = (adrv904x_CpuErrorCode_e)ADRV904X_CTOHL((uint32_t)pCmdResp->cmdStatus);
            ADI_ERROR_REPORT(&device->common, ADI_ADRV904X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, cmdErrorCode, "GET_SYS_STATUS command failed.");
            return recoveryAction;
        }
        else
        {
            /* Otherwise log a generic command failed error */
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting response for GET_SYS_STATUS command.");
            return recoveryAction;
        }
    }

    /* Find the sys status in the response payload, and set the caller's pointer to it. */
    *pSysStatus = (void*)((uint8_t*)pCmdResp + sizeof(adrv904x_CpuCmd_GetSysStatusResp_t));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuChannelMappingGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_CpuType_e       cpuTypes[],
                                                                   const uint8_t                numSerdesLanes)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    adi_adrv904x_CpuType_e cpuType = ADI_ADRV904X_CPU_TYPE_UNKNOWN;
    uint32_t               chanSel = 0U;
    uint8_t                i       = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(cpuTypes);

    
    if (ADI_ADRV904X_MAX_SERDES_LANES > numSerdesLanes)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numSerdesLanes, "Invalid size of cpuTypes array. Should be equal or smaller to ADI_ADRV904X_MAX_SERDES_LANES");
        goto cleanup;
    }

    for (i = 0U; i < numSerdesLanes; i++)
    {
        /* Get the CPU that is responsible for the requested lane. */
        chanSel = (uint32_t)(1U << i);
        recoveryAction = adrv904x_CpuChannelMappingGet(device, (adi_adrv904x_Channels_e)(chanSel), ADRV904X_CPU_OBJID_IC_SERDES, &cpuType);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to retrieve cpuType for a particular Serdes lane.");
            goto cleanup;
        }

        cpuTypes[i] = cpuType;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuSysPvtStatusGet(adi_adrv904x_Device_t* const device,
                                                                 const adi_adrv904x_Channels_e channel,
                                                                 const uint32_t objId,
                                                                 uint8_t  cpuSysStatusGet[],
                                                                 const uint32_t length)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const void* pSysStatus = NULL;
    const void** const ppSysStatus = &pSysStatus;
    char rxBuf[ADRV904X_CPU_CMD_RESP_MAX_SIZE_BYTES];

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuSysStatusGet, cleanup);

    if ((channel != ADI_ADRV904X_CH0) &&
        (channel != ADI_ADRV904X_CH1) &&
        (channel != ADI_ADRV904X_CH2) &&
        (channel != ADI_ADRV904X_CH3) &&
        (channel != ADI_ADRV904X_CH4) &&
        (channel != ADI_ADRV904X_CH5) &&
        (channel != ADI_ADRV904X_CH6) &&
        (channel != ADI_ADRV904X_CH7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "channel parameter is invalid.");
        goto cleanup;
    }

    if ((objId  <= (uint32_t) ADRV904X_CPU_OBJID_IC_END) ||
        ((objId  >=  (uint32_t) ADRV904X_CPU_OBJID_TC_START) &&
         (objId  <= (uint32_t) ADRV904X_CPU_OBJID_TC_END)) ||
        (objId  >  (uint32_t) ADRV904X_CPU_OBJID_SYSTEM_END))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, length, "objId parameter is invalid.");
        goto cleanup;
    }

    if (length == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, length, "Length is zero.");
        goto cleanup;
    }

    /* Send the system status get command to the CPU */
    recoveryAction = adrv904x_SendSysStatusCmd(device, ADRV904X_CPU_CMD_SYS_STATUS_PRIVATE, (adrv904x_CpuObjectId_e)objId, channel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SendCalStatusCmd failed");
        goto cleanup;
    }

    /* Get the response from the CPU */
    recoveryAction = adrv904x_GetSysStatusCmdResp(device, channel, length, (void*)&rxBuf[0], ppSysStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GetCalStatusCmd failed");
        goto cleanup;
    }

    if (pSysStatus == NULL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GET_CAL_STATUS command failed");
        goto cleanup;
    }

    ADI_LIBRARY_MEMCPY(cpuSysStatusGet, pSysStatus, length);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


static adi_adrv904x_ErrAction_e adrv904x_SysStatusGet(adi_adrv904x_Device_t* const    device,
                                                      const adrv904x_CpuObjectId_e    objId,
                                                      const adi_adrv904x_Channels_e   channel,
                                                      adi_adrv904x_CpuSysStatus_t* const status)
{
    const adi_adrv904x_CpuSysStatus_t* pSysStatus = NULL;
    const adi_adrv904x_CpuSysStatus_t** const ppSysStatus = &pSysStatus;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    char rxBuf[ADRV904X_CPU_CMD_RESP_MAX_SIZE_BYTES];

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, status);

    /* Send the sys status get command to the CPU */
    recoveryAction = adrv904x_SendSysStatusCmd(device, ADRV904X_CPU_CMD_SYS_STATUS_PUBLIC, objId, channel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SendSysStatusCmd failed");
        return recoveryAction;
    }

    ADI_LIBRARY_MEMSET(&rxBuf, 0, sizeof(rxBuf));

    /* Get the response from the CPU */
    recoveryAction = adrv904x_GetSysStatusCmdResp(device, channel, sizeof(adi_adrv904x_CpuSysStatus_t), (void*)&rxBuf[0], (const void** const)ppSysStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GetSysStatusCmdResp failed");
        return recoveryAction;
    }

    if (pSysStatus != NULL)
    {
        /* Translate the response from the CPU */
        status->errorCode = ADRV904X_CTOHL(pSysStatus->errorCode);
        status->placeHolder = ADRV904X_CTOHL(pSysStatus->placeHolder);
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuSysStatusGet(adi_adrv904x_Device_t* const                 device,
                                                              const adi_adrv904x_Channels_e                channel,
                                                              const uint32_t                               objId,
                                                              adi_adrv904x_CpuSysStatus_t* const           cpuSysStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuSysStatus, cleanup);

    if ((channel != ADI_ADRV904X_CH0) &&
        (channel != ADI_ADRV904X_CH1) &&
        (channel != ADI_ADRV904X_CH2) &&
        (channel != ADI_ADRV904X_CH3) &&
        (channel != ADI_ADRV904X_CH4) &&
        (channel != ADI_ADRV904X_CH5) &&
        (channel != ADI_ADRV904X_CH6) &&
        (channel != ADI_ADRV904X_CH7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "channel parameter is invalid.");
        goto cleanup;
    }

    if ((objId  <= (uint32_t) ADRV904X_CPU_OBJID_IC_END) ||
        ((objId  >=  (uint32_t) ADRV904X_CPU_OBJID_TC_START) &&
         (objId  <= (uint32_t) ADRV904X_CPU_OBJID_TC_END)) ||
        (objId  >  (uint32_t) ADRV904X_CPU_OBJID_SYSTEM_END))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, objId, "objId parameter is invalid.");
        goto cleanup;
    }

    /* Use the common system status get handler to get the data from the CPU */
    recoveryAction = adrv904x_SysStatusGet(device, (adrv904x_CpuObjectId_e)objId, channel, cpuSysStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting cpu system status.");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuFwVersionGet(adi_adrv904x_Device_t* const          device,
                                                              adi_adrv904x_CpuFwVersion_t* const    cpuFwVersion)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t verCpu[5U] = { 0U };
    adi_adrv904x_CpuAddr_t* cpuAddr = NULL;
    adi_adrv904x_CpuType_e  cpuType = ADI_ADRV904X_CPU_TYPE_0;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuFwVersion, cleanup);

    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_CPU0LOADED) != ADI_ADRV904X_STATE_CPU0LOADED ||
       (device->devStateInfo.devState & ADI_ADRV904X_STATE_CPU1LOADED) != ADI_ADRV904X_STATE_CPU1LOADED)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU binary must be loaded before getting fw version");
        goto cleanup;
    }

    /* Read the fw version from CPU */
    cpuType = ADI_ADRV904X_CPU_TYPE_0;
    cpuAddr = adrv904x_CpuAddrGet(&device->devStateInfo.cpu, cpuType);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuAddr, cleanup);
    recoveryAction = adi_adrv904x_Registers32Read(device, NULL, cpuAddr->versionAddr, verCpu, NULL, 5U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading CpuFwVersion CPU registers");
        goto cleanup;
    }

    /* Store the fw Version */
    cpuFwVersion->commVer.majorVer          = verCpu[0U];
    cpuFwVersion->commVer.minorVer          = verCpu[1U];
    cpuFwVersion->commVer.maintenanceVer    = verCpu[2U];
    cpuFwVersion->commVer.buildVer          = verCpu[3U];

    cpuFwVersion->cpuFwBuildType = (adi_adrv904x_CpuFwBuildType_e)(verCpu[4U] & (ADI_ADRV904X_CPU_FW_BUILD_DEBUG | ADI_ADRV904X_CPU_FW_BUILD_RELEASE | ADI_ADRV904X_CPU_FW_TRBLSHOOT));

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_HealthMonitorCpuStatusGet(adi_adrv904x_Device_t* const                    device,
                                                                        adi_adrv904x_HealthMonitorCpuStatus_t* const    healthMonitorStatus)
{
        static const uint32_t CPU_HEALTH_STATUS_ADDR_MASK = 0xFFFFFFFFU;
    const uint32_t BYTE_COUNT = sizeof(adi_adrv904x_HealthMonitorCpuStatus_t);
    const uint32_t WORD_COUNT = ((BYTE_COUNT + 3U) / 4U);
    uint32_t readData[WORD_COUNT];
    ADI_LIBRARY_MEMSET(readData, 0, 4U * WORD_COUNT);

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t cpuHealthStatusAddr = 0U;
    adi_adrv904x_HealthMonitorCpuStatus_t* tmpStatus = NULL;

    /* Verify inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, healthMonitorStatus, cleanup);

    /* Clear memory for return data */
    ADI_LIBRARY_MEMSET(healthMonitorStatus, 0, sizeof(adi_adrv904x_HealthMonitorCpuStatus_t));

    /* Read back cpu health status address */
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, (uint32_t)(ADRV904X_PM_HEALTH_STATUS_PTR), &cpuHealthStatusAddr, CPU_HEALTH_STATUS_ADDR_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading cpu health status address");
        goto cleanup;
    }

    /* Readback via shared cpu memory location */
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, cpuHealthStatusAddr, (uint8_t *)readData, NULL, BYTE_COUNT);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading CPU Health Monitor Status");
        goto cleanup;
    }

    /* Re-interrupt readData as the CPU byte-ordered struct. May leave some unused bytes at the tail of the read data. */
    tmpStatus = (adi_adrv904x_HealthMonitorCpuStatus_t*)readData;

    /* Correct byte order and copy to return struct */
    healthMonitorStatus->cpu0.initCalStatusMask     = ADRV904X_CTOHLL(tmpStatus->cpu0.initCalStatusMask);
    healthMonitorStatus->cpu0.numInitCalErrors      = ADRV904X_CTOHL(tmpStatus->cpu0.numInitCalErrors);
    healthMonitorStatus->cpu0.trackingCalStatusMask = ADRV904X_CTOHLL(tmpStatus->cpu0.trackingCalStatusMask);
    healthMonitorStatus->cpu0.numTrackingCalErrors  = ADRV904X_CTOHL(tmpStatus->cpu0.numTrackingCalErrors);
    healthMonitorStatus->cpu0.numSystemErrors       = ADRV904X_CTOHL(tmpStatus->cpu0.numSystemErrors);
    healthMonitorStatus->cpu0.numSystemWarnings     = ADRV904X_CTOHL(tmpStatus->cpu0.numSystemWarnings);
    healthMonitorStatus->cpu0.cpuUsage              = (adi_adrv904x_CpuUsage_e)(tmpStatus->cpu0.cpuUsage);

    healthMonitorStatus->cpu1.initCalStatusMask     = ADRV904X_CTOHLL(tmpStatus->cpu1.initCalStatusMask);
    healthMonitorStatus->cpu1.numInitCalErrors      = ADRV904X_CTOHL(tmpStatus->cpu1.numInitCalErrors);
    healthMonitorStatus->cpu1.trackingCalStatusMask = ADRV904X_CTOHLL(tmpStatus->cpu1.trackingCalStatusMask);
    healthMonitorStatus->cpu1.numTrackingCalErrors  = ADRV904X_CTOHL(tmpStatus->cpu1.numTrackingCalErrors);
    healthMonitorStatus->cpu1.numSystemErrors       = ADRV904X_CTOHL(tmpStatus->cpu1.numSystemErrors);
    healthMonitorStatus->cpu1.numSystemWarnings     = ADRV904X_CTOHL(tmpStatus->cpu1.numSystemWarnings);
    healthMonitorStatus->cpu1.cpuUsage              = (adi_adrv904x_CpuUsage_e)(tmpStatus->cpu1.cpuUsage);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuCheckException(adi_adrv904x_Device_t* const    device,
                                                                uint32_t* const                 isException)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t cpuIdx = 0U;
    uint32_t excpFlag = 0U;
    const uint32_t cpuExceptionAddr[ADI_ADRV904X_CPU_TYPE_MAX_RADIO] = { ADRV904X_CPU_0_PM_EXCEPTION_FLAG, ADRV904X_CPU_1_PM_EXCEPTION_FLAG }; /* Exception Flag Memory */
    
    /* Verify inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, isException, cleanup);
    
    *isException = 0U;
    
    /* Check exception status of ARM CPUs*/
    for (cpuIdx = ADI_ADRV904X_CPU_TYPE_0; cpuIdx < ADI_ADRV904X_CPU_TYPE_MAX_RADIO; cpuIdx++)
    {
        recoveryAction = adi_adrv904x_Register32Read(device, NULL, cpuExceptionAddr[cpuIdx], &excpFlag, 0xFFFFFFFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to Read Cpu Exception memory");
            goto cleanup;
        }        
        if (excpFlag != 0U)
        {
            *isException |= (1U << cpuIdx);
        }
    }
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

static adi_adrv904x_ErrAction_e GetBreakPointTableIndexFromObjId(adi_adrv904x_Device_t* const device,
                                                                 uint32_t objId,
                                                                 uint8_t * tableId,
                                                                 uint32_t * tableBaseAddr)
{
    static const uint32_t BREAKPOINT_TABLE_ADDR_MASK = 0xFFFFFFFFU;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t breakpointRowAddr = 0U;
    uint32_t breakpointObjId = 0U;
    uint32_t index = 0U;

    /* Read back global breakpoint table address */
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, (uint32_t)(ADRV904X_PM_SW_BKPT_TABLE_PTR), tableBaseAddr, BREAKPOINT_TABLE_ADDR_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading breakpoint table address");
        return recoveryAction;
    }

    for (index = 0U; index < ADRV904X_CPU_BKPT_TABLE_SIZE; index++)
    {
        breakpointRowAddr = (*tableBaseAddr) + (uint32_t)(sizeof(adi_adrv904x_SwBreakPointEntry_t)) * (uint32_t)index;
        recoveryAction = adi_adrv904x_Register32Read(device, NULL, breakpointRowAddr, &breakpointObjId, BREAKPOINT_TABLE_ADDR_MASK);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading breakpoint object id from table");
            return recoveryAction;
        }

        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

        if (breakpointObjId == objId)
        {
            *tableId = index;
            recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            break;
        }
    }

    return recoveryAction;
}

static adi_adrv904x_ErrAction_e GetBreakPointObjIdFromTableIndex(adi_adrv904x_Device_t* const device,
                                                                 uint8_t tableId,
                                                                 uint32_t * objId)
{
    static const uint32_t BREAKPOINT_TABLE_ADDR_MASK = 0xFFFFFFFFU;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t breakPointTableObjIdAddr = 0U;
    uint32_t tableBaseAddr = 0U;

    /* Read back global breakpoint table address */
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, (uint32_t)(ADRV904X_PM_SW_BKPT_TABLE_PTR), &tableBaseAddr, BREAKPOINT_TABLE_ADDR_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading breakpoint table address");
        return recoveryAction;
    }

    breakPointTableObjIdAddr = tableBaseAddr + (uint32_t)tableId * (uint32_t)(sizeof(adi_adrv904x_SwBreakPointEntry_t));
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, breakPointTableObjIdAddr, objId, BREAKPOINT_TABLE_ADDR_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading breakpoint object id from table");
        return recoveryAction;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointSet(adi_adrv904x_Device_t* const device,
                                                            const adi_adrv904x_SwBreakPointEntry_t * breakPointCfg)
{
        static const uint32_t BREAKPOINT_TABLE_ADDR_MASK = 0xFFFFFFFFU;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t breakPointTableId = 0U;
    uint32_t breakPointTableBaseAddr = 0U;
    uint32_t breakPointTableChanMaskAddr = 0U;
    uint32_t breakPointTableBkptMaskAddr = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, breakPointCfg, cleanup);

    static const uint32_t ALL_CHAN_MASK = ((uint32_t)ADI_ADRV904X_CH0 |
                                           (uint32_t)ADI_ADRV904X_CH1 |
                                           (uint32_t)ADI_ADRV904X_CH2 |
                                           (uint32_t)ADI_ADRV904X_CH3 |
                                           (uint32_t)ADI_ADRV904X_CH4 |
                                           (uint32_t)ADI_ADRV904X_CH5 |
                                           (uint32_t)ADI_ADRV904X_CH6 |
                                           (uint32_t)ADI_ADRV904X_CH7);

    if ((breakPointCfg->chanMask & (~(uint32_t)ALL_CHAN_MASK)) != 0U)
    {
        /* Invalid channel mask */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, breakPointCfg->chanMask, "Invalid channel mask is selected.");
        goto cleanup;
    }

    recoveryAction = GetBreakPointTableIndexFromObjId(device, breakPointCfg->objId, &breakPointTableId, &breakPointTableBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading breakpoint table");
        goto cleanup;
    }

    breakPointTableChanMaskAddr = breakPointTableBaseAddr + (uint32_t)breakPointTableId * (uint32_t)(sizeof(adi_adrv904x_SwBreakPointEntry_t)) + (uint32_t)(sizeof(breakPointCfg->objId));
    breakPointTableBkptMaskAddr = breakPointTableChanMaskAddr +  (uint32_t)(sizeof(breakPointCfg->chanMask));

    recoveryAction = adi_adrv904x_Register32Write(device, NULL, breakPointTableChanMaskAddr, breakPointCfg->chanMask, BREAKPOINT_TABLE_ADDR_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing channel mask for selected breakpoint");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_Register32Write(device, NULL, breakPointTableBkptMaskAddr, breakPointCfg->bkptMask, BREAKPOINT_TABLE_ADDR_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing breakpoint mask for selected breakpoint");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointGet(adi_adrv904x_Device_t* const device,
                                                            adi_adrv904x_SwBreakPointEntry_t * const breakPointCfgRead)
{
        static const uint32_t BREAKPOINT_TABLE_ADDR_MASK = 0xFFFFFFFFU;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t breakPointTableId = 0U;
    uint32_t breakPointTableBaseAddr = 0U;
    uint32_t breakPointTableChanMaskAddr = 0U;
    uint32_t breakPointTableBkptMaskAddr = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, breakPointCfgRead, cleanup);

    recoveryAction = GetBreakPointTableIndexFromObjId(device, breakPointCfgRead->objId, &breakPointTableId, &breakPointTableBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading breakpoint table");
        goto cleanup;
    }

    breakPointTableChanMaskAddr = breakPointTableBaseAddr + (uint32_t)breakPointTableId * (uint32_t)(sizeof(adi_adrv904x_SwBreakPointEntry_t)) + (uint32_t)(sizeof(breakPointCfgRead->objId));
    breakPointTableBkptMaskAddr = breakPointTableChanMaskAddr +  (uint32_t)(sizeof(breakPointCfgRead->chanMask));

    recoveryAction = adi_adrv904x_Register32Read(device, NULL, breakPointTableChanMaskAddr, &breakPointCfgRead->chanMask, BREAKPOINT_TABLE_ADDR_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading channel mask for selected breakpoint");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_Register32Read(device, NULL, breakPointTableBkptMaskAddr, &breakPointCfgRead->bkptMask, BREAKPOINT_TABLE_ADDR_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading breakpoint mask for selected breakpoint");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointHitRead(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SwBreakPointEntry_t * const cpu0BreakpointHit,
                                                                adi_adrv904x_SwBreakPointEntry_t * const cpu1BreakpointHit)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    static const uint32_t NO_BREAKPOINT_HIT_VALUE = 0xFFFFFFFFU;
    uint8_t breakPointIdCpu0 = 0;
    uint8_t breakPointNumCpu0 = 0;
    uint8_t breakPointChIdCpu0 = 0;
    uint8_t breakPointIdCpu1 = 0;
    uint8_t breakPointNumCpu1 = 0;
    uint8_t breakPointChIdCpu1 = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpu0BreakpointHit, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpu1BreakpointHit, cleanup);

    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device, NULL, ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS, (uint8_t)ADRV904X_CPU_CPU0_SWBKPT_INDEX_SCRATCH_REG_ID, &breakPointIdCpu0);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading Cpu0 breakpoint Id from scratchpad reg");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device, NULL, ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS, (uint8_t)ADRV904X_CPU_CPU0_SWBKPT_BKPT_NUM_SCRATCH_REG_ID, &breakPointNumCpu0);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading Cpu0 breakpoint num from scratchpad reg");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device, NULL, ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS, (uint8_t)ADRV904X_CPU_CPU0_SWBKPT_CHAN_NUM_SCRATCH_REG_ID, &breakPointChIdCpu0);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading Cpu0 breakpoint Ch Id from scratchpad reg");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device, NULL, ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS, (uint8_t)ADRV904X_CPU_CPU1_SWBKPT_INDEX_SCRATCH_REG_ID, &breakPointIdCpu1);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading Cpu1 breakpoint Id from scratchpad reg");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device, NULL, ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS, (uint8_t)ADRV904X_CPU_CPU1_SWBKPT_BKPT_NUM_SCRATCH_REG_ID, &breakPointNumCpu1);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading Cpu1 breakpoint num from scratchpad reg");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device, NULL, ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS, (uint8_t)ADRV904X_CPU_CPU1_SWBKPT_CHAN_NUM_SCRATCH_REG_ID, &breakPointChIdCpu1);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading Cpu1 breakpoint Ch Id from scratchpad reg");
        goto cleanup;
    }

    if (breakPointIdCpu0 == 0)
    {
        cpu0BreakpointHit->objId = NO_BREAKPOINT_HIT_VALUE;
        cpu0BreakpointHit->bkptMask = NO_BREAKPOINT_HIT_VALUE;
        cpu0BreakpointHit->chanMask = NO_BREAKPOINT_HIT_VALUE;
    }
    else
    {
        /* Cpu0 is hit a breakpoint */
        recoveryAction = GetBreakPointObjIdFromTableIndex(device, breakPointIdCpu0, &cpu0BreakpointHit->objId);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading CPU0 breakpoint object id");
            goto cleanup;
        }

        cpu0BreakpointHit->bkptMask = (uint32_t)1U << (uint32_t)breakPointNumCpu0;
        cpu0BreakpointHit->chanMask = (uint32_t)1U << (uint32_t)breakPointChIdCpu0;
    }

    if (breakPointIdCpu1 == 0)
    {
        cpu1BreakpointHit->objId = NO_BREAKPOINT_HIT_VALUE;
        cpu1BreakpointHit->bkptMask = NO_BREAKPOINT_HIT_VALUE;
        cpu1BreakpointHit->chanMask = NO_BREAKPOINT_HIT_VALUE;
    }
    else
    {
        /* Cpu1 is hit a breakpoint */
        recoveryAction = GetBreakPointObjIdFromTableIndex(device, breakPointIdCpu1, &cpu1BreakpointHit->objId);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading CPU1 breakpoint object id");
            goto cleanup;
        }

        cpu1BreakpointHit->bkptMask = (uint32_t)1U << (uint32_t)breakPointNumCpu1;
        cpu1BreakpointHit->chanMask = (uint32_t)1U << (uint32_t)breakPointChIdCpu1;
    }


cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointGpioCfgSet(adi_adrv904x_Device_t* const device,
                                                                   const adi_adrv904x_GpioPinSel_e gpioOutputForBreakpointHit,
                                                                   const adi_adrv904x_GpioPinSel_e gpioInputToResumeSleepingTasks)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;



    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Range check gpio */
    if (gpioOutputForBreakpointHit > ADI_ADRV904X_GPIO_INVALID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gpioOutputForBreakpointHit, "Invalid GPIO selected. Out of range.");
        goto cleanup;
    }

    if (gpioInputToResumeSleepingTasks > ADI_ADRV904X_GPIO_INVALID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gpioInputToResumeSleepingTasks, "Invalid GPIO selected. Out of range.");
        goto cleanup;
    }

    if ((gpioInputToResumeSleepingTasks == gpioOutputForBreakpointHit) &&
        (gpioInputToResumeSleepingTasks != ADI_ADRV904X_GPIO_INVALID))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gpioInputToResumeSleepingTasks, "Invalid GPIO selected. GPIO pins can not be the same.");
        goto cleanup;
    }

    recoveryAction = adrv904x_CpuGpioSet(device,
                                         gpioInputToResumeSleepingTasks,
                                         ADRV904X_CPU_CMD_GPIO_SIGNAL_SWBKPT_RESUME_FROM_BKPT,
                                         ADI_TRUE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while configuring the CPU GPIO input signal");
        goto cleanup;
    }

    recoveryAction = adrv904x_CpuGpioSet(device,
                                         gpioOutputForBreakpointHit,
                                         ADRV904X_CPU_CMD_GPIO_SIGNAL_SWBKPT_BKPT_HIT,
                                         ADI_FALSE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while configuring the CPU GPIO output signal");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointGpioCfgGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_GpioPinSel_e * const gpioOutputForBreakpointHit,
                                                                   adi_adrv904x_GpioPinSel_e * const gpioInputToResumeSleepingTasks)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);


    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gpioOutputForBreakpointHit, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gpioInputToResumeSleepingTasks, cleanup);

    *gpioOutputForBreakpointHit = ADI_ADRV904X_GPIO_INVALID;
    *gpioInputToResumeSleepingTasks = ADI_ADRV904X_GPIO_INVALID;

    recoveryAction = adrv904x_CpuGpioGet(device, gpioOutputForBreakpointHit, ADRV904X_CPU_CMD_GPIO_SIGNAL_SWBKPT_BKPT_HIT);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading GPIO pin for SW breakpoint hit signal");
        goto cleanup;
    }

    recoveryAction = adrv904x_CpuGpioGet(device, gpioInputToResumeSleepingTasks, ADRV904X_CPU_CMD_GPIO_SIGNAL_SWBKPT_RESUME_FROM_BKPT);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading GPIO pin for SW resume signal");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointResume(adi_adrv904x_Device_t* const device,
                                                               const adi_adrv904x_SwBreakPointEntry_t * breakpointToResume,
                                                               uint8_t resumeAll)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_ResumeBkpt_t breakpointResumeCmd;
    adrv904x_CpuCmd_ResumeBkptResp_t breakpointResumeCmdRsp;
    uint32_t cpuTypeIdx = 0U;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, breakpointToResume, cleanup);

    ADI_LIBRARY_MEMSET(&breakpointResumeCmd, 0, sizeof(adrv904x_CpuCmd_ResumeBkpt_t));
    ADI_LIBRARY_MEMSET(&breakpointResumeCmdRsp, 0, sizeof(adrv904x_CpuCmd_ResumeBkptResp_t));

    if (resumeAll > 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, resumeAll, "resumeAll parameter can be either 0 or 1");
        goto cleanup;
    }

    static const uint32_t ALL_CHAN_MASK = ((uint32_t)ADI_ADRV904X_CH0 |
                                           (uint32_t)ADI_ADRV904X_CH1 |
                                           (uint32_t)ADI_ADRV904X_CH2 |
                                           (uint32_t)ADI_ADRV904X_CH3 |
                                           (uint32_t)ADI_ADRV904X_CH4 |
                                           (uint32_t)ADI_ADRV904X_CH5 |
                                           (uint32_t)ADI_ADRV904X_CH6 |
                                           (uint32_t)ADI_ADRV904X_CH7);

    if ((breakpointToResume->chanMask & (~(uint32_t)ALL_CHAN_MASK)) != 0U)
    {
        /* Invalid channel mask */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, breakpointToResume->chanMask, "Invalid channel mask is selected.");
        goto cleanup;
    }

    /* Fill out run bkpt resume cmd params with user-provided params */
    breakpointResumeCmd.objectID = ADRV904X_HTOCL(breakpointToResume->objId);
    breakpointResumeCmd.chanMask = (uint8_t)(breakpointToResume->chanMask);
    breakpointResumeCmd.bResumeAll = (uint8_t)(resumeAll);

    /* For each CPU, send the resume command, wait for a response, and process any errors. */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_RESUME_BKPT,
                                                (void*)&breakpointResumeCmd,
                                                sizeof(breakpointResumeCmd),
                                                (void*)&breakpointResumeCmdRsp,
                                                sizeof(breakpointResumeCmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(breakpointResumeCmdRsp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointResumeFromHalt(adi_adrv904x_Device_t* const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    recoveryAction = adrv904x_Core_SpDbgGlobalResume_BfSet(device, NULL, ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS, 0x0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to set resume bit");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointGlobalHaltMaskSet(adi_adrv904x_Device_t* const device,
                                                                          const uint32_t globalHaltMask)
{
        static const uint32_t GLOBAL_HALTMASK_ADDR_MASK = 0xFFFFFFFFU;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t globalHaltMaskAddr = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Read back global halt mask address */
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, (uint32_t)(ADRV904X_PM_SW_BKPT_GLOBAL_HALT_MASK_PTR), &globalHaltMaskAddr, GLOBAL_HALTMASK_ADDR_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading global halt mask address");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_Register32Write(device, NULL, globalHaltMaskAddr, globalHaltMask, GLOBAL_HALTMASK_ADDR_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing global halt mask value");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointGlobalHaltMaskGet(adi_adrv904x_Device_t* const device,
                                                                          uint32_t * const globalHaltMask)
{
        static const uint32_t GLOBAL_HALTMASK_ADDR_MASK = 0xFFFFFFFFU;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t globalHaltMaskAddr = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, globalHaltMask, cleanup);

    /* Read back global halt mask address */
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, (uint32_t)(ADRV904X_PM_SW_BKPT_GLOBAL_HALT_MASK_PTR), &globalHaltMaskAddr, GLOBAL_HALTMASK_ADDR_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading global halt mask address");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_Register32Read(device, NULL, globalHaltMaskAddr, globalHaltMask, GLOBAL_HALTMASK_ADDR_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading global halt mask value");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);

}
