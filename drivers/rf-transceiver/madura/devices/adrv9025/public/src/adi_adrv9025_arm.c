// SPDX-License-Identifier: GPL-2.0
/**
* \file adi_adrv9025_arm.c
* \brief Contains ARM features related function implementation defined in
* adi_adrv9025_arm.h
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "adi_adrv9025_arm.h"
#include "adi_adrv9025_cpu.h"

#ifdef _RELEASE_BUILD_
    #line __LINE__ "adi_adrv9025_arm.c"
#endif

int32_t adi_adrv9025_ArmStart(adi_adrv9025_Device_t*     device,
                              const adi_adrv9025_Init_t* init)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    (void)init;
    adi_adrv9025_CpuStart(device);
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ArmStartStatusCheck(adi_adrv9025_Device_t* device,
                                         uint32_t               timeout_us)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    adi_adrv9025_CpuStartStatusCheck(device,
                                     timeout_us);
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ArmProfileWrite(adi_adrv9025_Device_t*     device,
                                     const adi_adrv9025_Init_t* init)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    adi_adrv9025_CpuProfileWrite(device,
                                 init);
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ArmImageWrite(adi_adrv9025_Device_t* device,
                                   uint32_t               byteOffset,
                                   const uint8_t          binary[],
                                   uint32_t               byteCount)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    adi_adrv9025_CpuImageWrite(device,
                               ADI_ADRV9025_CPU_TYPE_C,
                               byteOffset,
                               binary,
                               byteCount);
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_AdcProfilesWrite(adi_adrv9025_Device_t*            device,
                                      const adi_adrv9025_AdcProfiles_t* adcProfiles)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    adi_adrv9025_CpuAdcProfilesWrite(device,
                                     adcProfiles);
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ArmMemRead(adi_adrv9025_Device_t* device,
                                uint32_t               address,
                                uint8_t                returnData[],
                                uint32_t               byteCount,
                                uint8_t                autoIncrement)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    adi_adrv9025_CpuMemRead(device,
                            address,
                            returnData,
                            byteCount,
                            autoIncrement);
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ArmMemWrite(adi_adrv9025_Device_t* device,
                                 uint32_t               address,
                                 const uint8_t          data[],
                                 uint32_t               byteCount)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    adi_adrv9025_CpuMemWrite(device,
                             address,
                             data,
                             byteCount,
                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ArmConfigWrite(adi_adrv9025_Device_t* device,
                                    uint8_t                objectId,
                                    uint16_t               byteOffset,
                                    const uint8_t          data[],
                                    uint8_t                byteCount)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    adi_adrv9025_CpuConfigWrite(device,
                                ADI_ADRV9025_CPU_TYPE_C,
                                objectId,
                                byteOffset,
                                data,
                                byteCount);
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ArmConfigRead(adi_adrv9025_Device_t* device,
                                   uint8_t                objectId,
                                   uint16_t               byteOffset,
                                   uint8_t                returnData[],
                                   uint8_t                byteCount)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    adi_adrv9025_CpuConfigRead(device,
                               ADI_ADRV9025_CPU_TYPE_C,
                               objectId,
                               byteOffset,
                               returnData,
                               byteCount);
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ArmCmdStatusGet(adi_adrv9025_Device_t *device,
                                     uint16_t              *errorWord,
                                     uint16_t              *statusWord)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common, errorWord);
    ADI_NULL_PTR_RETURN(&device->common, statusWord);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    adi_adrv9025_CpuCmdStatusGet(device, ADI_ADRV9025_CPU_TYPE_C, errorWord, statusWord);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ArmCmdStatusOpcodeGet(adi_adrv9025_Device_t *device,
                                           uint8_t                opCode,
                                           uint8_t               *cmdStatByte)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common, cmdStatByte);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    adi_adrv9025_CpuCmdStatusOpcodeGet(device, ADI_ADRV9025_CPU_TYPE_C, opCode, cmdStatByte);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ArmCmdStatusWait(adi_adrv9025_Device_t* device,
                                      uint8_t                opCode,
                                      uint8_t*               cmdStatusByte,
                                      uint32_t               timeout_us,
                                      uint32_t               waitInterval_us)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    adi_adrv9025_CpuCmdStatusWait(device,
                                  ADI_ADRV9025_CPU_TYPE_C,
                                  opCode,
                                  cmdStatusByte,
                                  timeout_us,
                                  waitInterval_us);
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ArmCmdWrite(adi_adrv9025_Device_t* device,
                                 uint8_t                opCode,
                                 const uint8_t          extendedData[],
                                 uint8_t                byteCount)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    adi_adrv9025_CpuCmdWrite(device,
                             ADI_ADRV9025_CPU_TYPE_C,
                             opCode,
                             extendedData,
                             byteCount);
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ArmVersionGet(adi_adrv9025_Device_t*     device,
                                   adi_adrv9025_ArmVersion_t* armVersion)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    adi_adrv9025_CpuFwVersionGet(device,
                                 ADI_ADRV9025_CPU_TYPE_C,
                                 (adi_adrv9025_CpuFwVersion_t*)armVersion);
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ArmChecksumTableGet(adi_adrv9025_Device_t*        device,
                                         adi_adrv9025_ChecksumTable_t* checksum,
                                         uint8_t*                      checksumValid)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    adi_adrv9025_CpuChecksumTableGet(device,
                                     ADI_ADRV9025_CPU_TYPE_C,
                                     checksum,
                                     checksumValid);
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ArmMailboxBusyGet(adi_adrv9025_Device_t* device,
                                       uint8_t*               mailboxBusy)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    adi_adrv9025_CpuMailboxBusyGet(device,
                                   ADI_ADRV9025_CPU_TYPE_C,
                                   mailboxBusy);
    return device->common.error.newAction;
}

int32_t adi_adrv9025_ArmSystemErrorGet(adi_adrv9025_Device_t* device,
                                       const char**           errorMessage)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    adi_adrv9025_CpuSystemErrorGet(device,
                                   ADI_ADRV9025_CPU_TYPE_C,
                                   errorMessage);
    return device->common.error.newAction;
}
