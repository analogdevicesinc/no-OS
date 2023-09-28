// SPDX-License-Identifier: GPL-2.0
/**
* \file adi_adrv9025_cpu.c
* \brief Contains Adrv9025's processor features related function
* implementation declared in adi_adrv9025.h
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv9025.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_cpu.h"
#include "adi_adrv9025_error.h"
#include "../../private/include/adrv9025_bf_core.h"
#include "../../private/include/adrv9025_cpu.h"
#include "../../private/include/adrv9025_reg_addr_macros.h"
#include "../../private/include/adrv9025_init.h"

#ifdef _RELEASE_BUILD_
#line __LINE__ "adi_adrv9025_cpu.c"
#endif

/*****************************************************************************/
/***** Helper functions' prototypes ******************************************/
/*****************************************************************************/
static int32_t adrv9025_CpuStackPtrWrite(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    const uint32_t*        buf);

static int32_t adrv9025_CpuBootAddrWrite(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    const uint32_t*        buf);

static uint32_t adrv9025_CpuIntFromBytesGet(
    const uint8_t* buf,
    uint8_t        size);

/*****************************************************************************/
/***** Helper functions' definition ******************************************/
/*****************************************************************************/
int32_t adrv9025_CpuStackPtrWrite(adi_adrv9025_Device_t* device,
                                  adi_adrv9025_CpuType_e cpuType,
                                  const uint32_t*        buf)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    adi_adrv9025_CpuAddr_t* cpuAddr = NULL;
    const uint8_t*          bytes   = (const uint8_t *)buf;

    cpuAddr = adrv9025_CpuAddrGet(&device->devStateInfo.cpu,
                                  cpuType);
    ADI_NULL_PTR_RETURN(&device->common,
                        cpuAddr);
    ADRV9025_SPIWRITEBYTE("CPU_STACK_PTR_BYTE_0",
                          cpuAddr->stackPtrAddr,
                          bytes[0]);
    ADRV9025_SPIWRITEBYTE("CPU_STACK_PTR_BYTE_1",
                          cpuAddr->stackPtrAddr + 1u,
                          bytes[1]);
    ADRV9025_SPIWRITEBYTE("CPU_STACK_PTR_BYTE_2",
                          cpuAddr->stackPtrAddr + 2u,
                          bytes[2]);
    ADRV9025_SPIWRITEBYTE("CPU_STACK_PTR_BYTE_3",
                          cpuAddr->stackPtrAddr + 3u,
                          bytes[3]);
    return (device->common.error.newAction);
}

int32_t adrv9025_CpuBootAddrWrite(adi_adrv9025_Device_t *device,
                                  adi_adrv9025_CpuType_e cpuType,
                                  const uint32_t        *buf)
{
    adi_adrv9025_CpuAddr_t *cpuAddr = NULL;
    const uint8_t          *bytes   = (const uint8_t *)buf;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    cpuAddr = adrv9025_CpuAddrGet(&device->devStateInfo.cpu, cpuType);

    ADI_NULL_PTR_RETURN(&device->common, cpuAddr);

    ADRV9025_SPIWRITEBYTE("CPU_BOOT_ADDR_BYTE_0", cpuAddr->bootAddr, bytes[0]);
    ADRV9025_SPIWRITEBYTE("CPU_BOOT_ADDR_BYTE_1", cpuAddr->bootAddr + 1u, bytes[1]);
    ADRV9025_SPIWRITEBYTE("CPU_BOOT_ADDR_BYTE_2", cpuAddr->bootAddr + 2u, bytes[2]);
    ADRV9025_SPIWRITEBYTE("CPU_BOOT_ADDR_BYTE_3", cpuAddr->bootAddr + 3u, bytes[3]);

    return (device->common.error.newAction);
}

uint32_t adrv9025_CpuIntFromBytesGet(const uint8_t* buf,
                                     uint8_t        size)
{
    uint32_t result = 0;
    uint8_t  i      = 0;

    if (size <= 4)
    {
        for (i = 0; i < size; i++)
        {
            result |= (uint32_t)(*(buf + i)) << (i * 8);
        }
    }

    return result;
}

/*****************************************************************************/
/***** Public functions' definition ******************************************/
/*****************************************************************************/
int32_t adi_adrv9025_CpuStartStatusCheck(
    adi_adrv9025_Device_t* device,
    uint32_t               timeout_us)
{
    int32_t                         recoveryAction  = ADI_COMMON_ACT_NO_ACTION;
    int32_t                         halError        = (int32_t)ADI_COMMON_HAL_OK;
    uint8_t                         i               = 0;
    adi_adrv9025_CpuType_e          cpuType         = ADI_ADRV9025_CPU_TYPE_C;
    uint32_t                        fwStatus        = 0;
    uint32_t                        waitInterval_us = 0;
    uint32_t                        numEventChecks  = 1;
    uint32_t                        eventCheck      = 0;
    uint8_t                         status          = 0;
    adi_adrv9025_CpuChecksumTable_t checksum        = {{0}};
    uint8_t                         checksumValid   = 0;
    uint8_t                         cpuDebugLoaded  =
    ((device->devStateInfo.devState & ADI_ADRV9025_STATE_CPUDEBUGLOADED) == ADI_ADRV9025_STATE_CPUDEBUGLOADED) ? 1 : 0;
    uint8_t                 statusReadErrorFlag = 0;
    adi_adrv9025_CpuAddr_t* cpuAddr             = NULL;
    const char*             errStr              = NULL;

    static const uint32_t ADRV9025_CPU_FWSTATUS_ERROR = ADI_ADRV9025_SRC_CPUFWSTATUS << 16;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADRV9025_BUGINFO(__FUNCTION__);

    /* Wait for CPU to exit BOOTUP state */
    waitInterval_us = (ADI_ADRV9025_GETCPUBOOTUP_INTERVAL_US > timeout_us) ?
                          timeout_us : ADI_ADRV9025_GETCPUBOOTUP_INTERVAL_US;
    numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

    /* Get ARM-C address map */
    cpuAddr = adrv9025_CpuAddrGet(&device->devStateInfo.cpu, cpuType);
    ADI_NULL_PTR_RETURN(&device->common,
                        cpuAddr);

    for (eventCheck = 0; eventCheck <= numEventChecks && cpuAddr->enabled != ADI_FALSE; eventCheck++)
    {
        ADRV9025_SPIREADBYTE("CPU_CMD_STATUS_8",
                                cpuAddr->cmdStatusAddr + 8u,
                                &status);
        fwStatus = (uint32_t)status;

        /* if Non-boot status or Non-ready status encountered retry read for sticky error reported */
        if ((fwStatus != ADRV9025_CPU_FW_STATUS_POWERUP))
        {
            /* Confirm CPU status is consistent for 5 reads*/
            for (i = 0; i < 5; i++)
            {
                ADRV9025_SPIREADBYTE("CPU_CMD_STATUS_8",
                                        cpuAddr->cmdStatusAddr + 8u,
                                        &status);

                if ((uint8_t)fwStatus != status)
                {
                    statusReadErrorFlag = 1;
                    break;
                }
            }
        }

        if (statusReadErrorFlag == 0)
        {
            if (fwStatus == ADRV9025_CPU_FW_STATUS_POWERUP)
            {
                /* Evaluate first since it's the most common case. Wait interval then check again */
                /* If the ARM is stuck in powerup state, the API needs to generate an error */
                if (eventCheck == numEventChecks)
                {
                    ADI_ERROR_REPORT(&device->common,
                                        ADI_COMMON_ERRSRC_ADI_HAL,
                                        (ADRV9025_CPU_FWSTATUS_ERROR + fwStatus),
                                        ADI_COMMON_ACT_ERR_RESET_FEATURE,
                                        device,
                                        "CpuBootStatus_e: 1 - : CPU is stuck in Powerup mode");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }

                halError = adrv9025_hal_Wait_us(&device->common,
                                                    waitInterval_us);
                ADI_ERROR_REPORT(&device->common,
                                    ADI_COMMON_ERRSRC_ADI_HAL,
                                    halError,
                                    ADI_COMMON_ACT_ERR_CHECK_TIMER,
                                    device,
                                    "Timer not working");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else if ((cpuDebugLoaded == 1) && (fwStatus == ADRV9025_CPU_FW_STATUS_CPU_DEBUG_READY))
            {
                /* Wait for CPU to load from JTAG */
                halError = adrv9025_hal_Wait_us(&device->common,
                                                    waitInterval_us);
                ADI_ERROR_REPORT(&device->common,
                                    ADI_COMMON_ERRSRC_ADI_HAL,
                                    halError,
                                    ADI_COMMON_ACT_ERR_CHECK_TIMER,
                                    device,
                                    "Timer not working");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else if ((cpuDebugLoaded == 0) && (fwStatus == ADRV9025_CPU_FW_STATUS_CPU_DEBUG_READY))
            {
                device->devStateInfo.devState = (adi_adrv9025_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV9025_STATE_CPUDEBUGLOADED);
                return (device->common.error.newAction);
            }
            else if (fwStatus == ADRV9025_CPU_FW_STATUS_READY)
            {
                break; /* Valid case - CPU booted successfully */
            }
            else
            {
                recoveryAction = ADI_ADRV9025_ACT_ERR_RESET_CPU;
                switch (fwStatus)
                {
                case ADRV9025_CPU_FW_STATUS_FW_CHECKSUM_ERROR:
                    errStr = "CpuBootStatus_e: 2 - CPU Firmware checksum error";
                    break;
                case ADRV9025_CPU_FW_STATUS_DATA_MEMORY_ERROR:
                    errStr = "CpuBootStatus_e: 3 - CPU data memory error";
                    break;
                case ADRV9025_CPU_FW_STATUS_STREAM_CHECKSUM_ERROR:
                    errStr = "CpuBootStatus_e: 4 - Stream image checksum error";
                    break;
                case ADRV9025_CPU_FW_STATUS_PROFILE_ERROR:
                    errStr = "CpuBootStatus_e: 5 - Device profile checksum error";
                    break;
                case ADRV9025_CPU_FW_STATUS_CLKPLL_PROGRAMING_ERROR:
                    errStr = "CpuBootStatus_e: 6 - Bootup clkgen setup error";
                    break;
                case ADRV9025_CPU_FW_STATUS_JESD_SETUP_CONFIG_ERROR:
                    errStr = "CpuBootStatus_e: 7 - Bootup Jesd setup error";
                    break;
                case ADRV9025_CPU_FW_STATUS_POWER_UP_INIT_ERROR:
                    errStr = "CpuBootStatus_e: 8 - Power init setup error";
                    break;
                case ADRV9025_CPU_FW_STATUS_CLKLOGEN_ERROR:
                    errStr = "CpuBootStatus_e: 10 - Bootup clock LOGEN error";
                    break;
                case ADRV9025_CPU_FW_STATUS_RXQEC_HARDWARE_ERROR:
                    errStr = "CpuBootStatus_e: 11 - Error initializing RxQEC hardware";
                    break;
                case ADRV9025_CPU_FW_STATUS_HW_TIMER_ERROR:
                    errStr = "CpuBootStatus_e: 12 - Failed to create Health monitor timers";
                    break;
                case ADRV9025_CPU_FW_STATUS_ADC_RCAL_ERROR:
                    errStr = "CpuBootStatus_e: 13 - ADC RCAL error";
                    break;
                case ADRV9025_CPU_FW_STATUS_STREAM_RUNTIME_ERROR:
                    errStr = "CpuBootStatus_e: 14 - STREAM RUNTIME error";
                    break;
                case ADRV9025_CPU_FW_STATUS_BOOT_CLKGEN_RCAL_ERROR:
                    errStr = "CpuBootStatus_e: 15 - BOOT CLKGEN RCAL error";
                    break;
                case ADRV9025_CPU_FW_STATUS_BOOT_LDO_CONFIG_ERROR:
                    /* Attempt full hardware reset if LDO configuration is incorrect */
                    recoveryAction = adi_adrv9025_HwReset(device);
                    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
                    {
                        ADI_ERROR_REPORT(&device->common,
                                            ADI_COMMON_ERRSRC_API,
                                            ADI_COMMON_ERR_API_FAIL,
                                            recoveryAction,
                                            NULL,
                                            "CpuBootStatus_e: 16 - LDO Configured Incorrectly, adi_adrv9025_HwReset Failed");
                        ADI_ERROR_RETURN(device->common.error.newAction);
                    }
                    errStr         = "CpuBootStatus_e: 16 - LDO Configured Incorrectly";
                    recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
                    break;
                case ADRV9025_CPU_FW_STATUS_INVCHN_CONFIG_ERROR:
                    errStr = "CpuBootStatus_e: 17 - Detected device does not support the configured init channel mask";
                    break;
                case ADRV9025_CPU_D_FW_STATUS_CHECKSUM_ERROR:
                    errStr = "CpuBootStatus_e: 19 - ARM-D Firmware checksum error";
                    break;
                case ADRV9025_CPU_D_FW_STATUS_BOOT_ERROR:
                    errStr = "CpuBootStatus_e: 20 - ARM-D Boot error";
                    break;
                default:
                    errStr = "CpuBootStatus_e: CPU Unknown error during bootup";
                    break;
                }

                ADI_ERROR_REPORT(&device->common,
                                    ADI_ADRV9025_SRC_CPUFWSTATUS,
                                    (ADRV9025_CPU_FWSTATUS_ERROR + fwStatus),
                                    recoveryAction,
                                    device,
                                    errStr);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
        else
        {
            statusReadErrorFlag = 0;
        }
    }

    if (cpuDebugLoaded == 0)
    {
        checksumValid  = ADI_FALSE;
        recoveryAction = adi_adrv9025_CpuChecksumTableGet(device,
                                                            cpuType,
                                                            &checksum,
                                                            &checksumValid);
        if (checksumValid == ADI_FALSE)
        {
            ADI_ERROR_REPORT(&device->common,
                                ADI_ADRV9025_SRC_CPU,
                                ADI_COMMON_ERR_API_FAIL,
                                ADI_ADRV9025_ACT_ERR_RESET_CPU,
                                checksumValid,
                                "Checksum is invalid");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        ADI_ERROR_RETURN(device->common.error.newAction);
    }


    device->devStateInfo.devState = (adi_adrv9025_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV9025_STATE_CPULOADED);

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "CpuStartStatusCheck Failed");
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuChecksumTableGet(
    adi_adrv9025_Device_t*           device,
    adi_adrv9025_CpuType_e           cpuType,
    adi_adrv9025_CpuChecksumTable_t* checksum,
    uint8_t*                         checksumValid)
{
    int32_t                 recoveryAction                                     = ADI_COMMON_ACT_NO_ACTION;
    int32_t                 halError                                           = (int32_t)ADI_COMMON_HAL_OK;
    uint32_t                buildTimeChecksum                                  = 0x00000000;
    uint32_t                calculatedChecksum                                 = 0x00000000;
    uint8_t                 checkData[sizeof(adi_adrv9025_CpuChecksumTable_t)] = {0};
    uint32_t                checkAddr                                          = 0;
    uint32_t                timeout_us                                         = ADI_ADRV9025_VERIFY_CPU_CHKSUM_TIMEOUT_US;
    uint32_t                waitInterval_us                                    = ADI_ADRV9025_VERIFY_CPU_CHKSUM_INTERVAL_US;
    uint32_t                numEventChecks                                     = 1;
    uint32_t                eventCheck                                         = 0;
    adi_adrv9025_CpuAddr_t* cpuAddr                                            = NULL;
    uint8_t                 cpuDebugLoaded                                     =
    ((device->devStateInfo.devState & ADI_ADRV9025_STATE_CPUDEBUGLOADED) == ADI_ADRV9025_STATE_CPUDEBUGLOADED) ? 1 : 0;
    int j = 0;
    int i = 0;

    static const uint8_t  CHECKSUM_BYTES    = 0x4;
    static const uint8_t  CHECKSUMENTRYSIZE = 8;
    static const uint32_t MIN_TIMEOUT_US    = 10000;

    /* Cpu stream checksum order: main, rx1/2/3/4, tx1/2/3/4, orx12/34 */
    static const uint32_t streamChannel[] = {
        0xFFFFFFFF,
        ADI_ADRV9025_RX1,
        ADI_ADRV9025_RX2,
        ADI_ADRV9025_RX3,
        ADI_ADRV9025_RX4,
        ADI_ADRV9025_TX1 << ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV9025_TX2 << ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV9025_TX3 << ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV9025_TX4 << ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET,
        (ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2 | ADI_ADRV9025_LB12),
        (ADI_ADRV9025_ORX3 | ADI_ADRV9025_ORX4 | ADI_ADRV9025_LB34)
    };

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        checksum);

    ADI_NULL_PTR_RETURN(&device->common,
                        checksumValid);

    ADRV9025_BUGINFO(__FUNCTION__);

    /* This function is only supported for ARM-C */
    if ((adi_adrv9025_CpuType_e)cpuType != ADI_ADRV9025_CPU_TYPE_C)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         cpuType,
                         "This function can only be called for CPU-C");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }



    /* get the particular processor's address map */
    cpuAddr = adrv9025_CpuAddrGet(&device->devStateInfo.cpu,
                                  cpuType);
    ADI_NULL_PTR_RETURN(&device->common,
                        cpuAddr);

    if (timeout_us < MIN_TIMEOUT_US)
    {
        /* Set minimum timeout of 10 ms. Prevent divide by zero error */
        timeout_us = MIN_TIMEOUT_US;
    }

    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             cpuAddr->structChecksumAddr,
                                             checkData,
                                             CHECKSUM_BYTES,
                                             0);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to read CPU memory");
    ADI_ERROR_RETURN(device->common.error.newAction);
    checkAddr = ((((uint32_t)checkData[3]) << 24) | (((uint32_t)checkData[2]) << 16) | (((uint32_t)checkData[1]) << 8) | ((uint32_t)checkData[0]));



    waitInterval_us = (waitInterval_us > timeout_us) ? timeout_us : waitInterval_us;
    numEventChecks  = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
    {
        adi_adrv9025_CpuMemRead(device,
                                checkAddr,
                                &checkData[0],
                                sizeof(checkData),
                                0);
        ADI_ERROR_RETURN(device->common.error.newAction);

        buildTimeChecksum  = ((((uint32_t)checkData[3]) << 24) | (((uint32_t)checkData[2]) << 16) | (((uint32_t)checkData[1]) << 8) | ((uint32_t)checkData[0]));
        calculatedChecksum = ((((uint32_t)checkData[7]) << 24) | (((uint32_t)checkData[6]) << 16) | (((uint32_t)checkData[5]) << 8) | ((uint32_t)checkData[4]));



        if ((calculatedChecksum == 0) && (eventCheck < numEventChecks))
        {
            /* wait */
            halError = adrv9025_hal_Wait_us(&device->common,
                                              timeout_us);

            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_ADI_HAL,
                             halError,
                             ADI_COMMON_ACT_ERR_CHECK_TIMER,
                             eventCheck,
                             "Timer not working");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            break;
        }
    }



    /* CPU completed calculating checksum */
    if ((calculatedChecksum > 0) && (buildTimeChecksum > 0))
    {
        *checksumValid                      = 1;
        checksum->fwCheckSums.buildChecksum = buildTimeChecksum;
        checksum->fwCheckSums.runChecksum   = calculatedChecksum;



        /* performing checksum check, skip if checksum was not calculated (CPU DEBUG_MODE)*/
        if ((cpuDebugLoaded == 0) && (buildTimeChecksum != calculatedChecksum))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV9025_SRC_CPU,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_ADRV9025_ACT_ERR_RESET_CPU,
                             calculatedChecksum,
                             "FW checksum is invalid");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        for (j = 0; j < 11; j++)
        {
            i                 = CHECKSUMENTRYSIZE * (j + 1);
            buildTimeChecksum = ((((uint32_t)checkData[i + 3]) << 24) | (((uint32_t)checkData[i + 2]) << 16) | (((uint32_t)checkData[i + 1]) << 8) | ((uint32_t)
                checkData[i + 0]));
            calculatedChecksum = ((((uint32_t)checkData[i + 7]) << 24) | (((uint32_t)checkData[i + 6]) << 16) | (((uint32_t)checkData[i + 5]) << 8) | ((uint32_t
            )checkData[i + 4]));


            checksum->streamsCheckSum[j].buildChecksum = buildTimeChecksum;
            checksum->streamsCheckSum[j].runChecksum   = calculatedChecksum;

            /* Skip if channel not initialize */
            if ((device->devStateInfo.initializedChannels & streamChannel[j]) > 0)
            {
                /* performing checksum check */
                if (buildTimeChecksum != calculatedChecksum)
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_ADRV9025_SRC_CPUCMD,
                                     ADI_COMMON_ERR_API_FAIL,
                                     ADI_ADRV9025_ACT_ERR_RESET_CPU,
                                     calculatedChecksum,
                                     "Streams checksum is invalid");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
        }

        //i = offsetof(adi_adrv9025_checksumTable_t, deviceProfileCheckSum);
        i                 = CHECKSUMENTRYSIZE * 12;
        buildTimeChecksum = ((((uint32_t)checkData[i + 3]) << 24) | (((uint32_t)checkData[i + 2]) << 16) | (((uint32_t)checkData[i + 1]) << 8) | ((uint32_t)
            checkData[i + 0]));
        calculatedChecksum = ((((uint32_t)checkData[i + 7]) << 24) | (((uint32_t)checkData[i + 6]) << 16) | (((uint32_t)checkData[i + 5]) << 8) | ((uint32_t)
            checkData[i + 4]));


        checksum->deviceProfileCheckSum.buildChecksum = buildTimeChecksum;
        checksum->deviceProfileCheckSum.runChecksum   = calculatedChecksum;

#if ADI_ADRV9025_PROFILE_CHECKSUM_ENABLE > 0
        /* performing checksum check */
        if (buildTimeChecksum != calculatedChecksum)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_ADRV9025_SRC_CPUCMD,
                ADI_COMMON_ERR_API_FAIL,
                ADI_ADRV9025_ACT_ERR_RESET_CPU,
                calculatedChecksum,
                "CPU's Device Profile checksum is invalid");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
#endif

        i                 = CHECKSUMENTRYSIZE * 13;
        buildTimeChecksum = ((((uint32_t)checkData[i + 3]) << 24) | (((uint32_t)checkData[i + 2]) << 16) | (((uint32_t)checkData[i + 1]) << 8) | ((uint32_t)
            checkData[i + 0]));
        calculatedChecksum = ((((uint32_t)checkData[i + 7]) << 24) | (((uint32_t)checkData[i + 6]) << 16) | (((uint32_t)checkData[i + 5]) << 8) | ((uint32_t)
            checkData[i + 4]));


        checksum->adcProfilefwCheckSum.buildChecksum = buildTimeChecksum;
        checksum->adcProfilefwCheckSum.runChecksum   = calculatedChecksum;

#if ADI_ADRV9025_PROFILE_CHECKSUM_ENABLE > 0
        /* performing checksum check */
        if (buildTimeChecksum != calculatedChecksum)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_ADRV9025_SRC_CPUCMD,
                ADI_COMMON_ERR_API_FAIL,
                ADI_ADRV9025_ACT_ERR_RESET_CPU,
                calculatedChecksum,
                "Adc Profile checksum is invalid");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
#endif
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuCmdStatusGet(adi_adrv9025_Device_t  *device,
                                     adi_adrv9025_CpuType_e  cpuType,
                                     uint16_t               *errorWord,
                                     uint16_t               *statusWord)
{
    uint8_t                 i                    = 0;
    uint8_t                 bytes[8]             = {0};
    uint8_t                 status               = 0;
    static const uint8_t    CPU_ERR_MASK_LO      = 0x0E;
    static const uint8_t    CPU_ERR_MASK_HI      = 0xE0;
    static const uint8_t    CPU_PENDING_LO       = 0x01;
    static const uint8_t    CPU_PENDING_HI       = 0x10;
    static const uint8_t    ERROR_WORD_LO        = 0x0001;
    static const uint8_t    ERROR_WORD_HI        = 0x0002;
    static const uint8_t    STATUS_WORD_SHIFT_HI = 3;
    static const uint8_t    CMD_STATUS_MAX       = 8;
    adi_adrv9025_CpuAddr_t* cpuAddr              = NULL;

#if (ADRV9025_INIT_SPI_DEBUG > 0)
    static const char *cmdStatus[8] =
    {
        "CPU_CMD_STATUS_0",
        "CPU_CMD_STATUS_1",
        "CPU_CMD_STATUS_2",
        "CPU_CMD_STATUS_3",
        "CPU_CMD_STATUS_4",
        "CPU_CMD_STATUS_5",
        "CPU_CMD_STATUS_6",
        "CPU_CMD_STATUS_7"
    };
#endif

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, errorWord);

    ADI_NULL_PTR_RETURN(&device->common, statusWord);

    ADRV9025_BUGINFO(__FUNCTION__);

    /* ensuring the errorWord and statusWord are cleared */
    *errorWord  = 0;
    *statusWord = 0;

    /* get the particular processor's address map */
    cpuAddr = adrv9025_CpuAddrGet(&device->devStateInfo.cpu, cpuType);

    ADI_NULL_PTR_RETURN(&device->common, cpuAddr);

    /* read in the entire 64-bit status register into a byte array and parse one byte at a time */
    for (i = 0; i < CMD_STATUS_MAX; i++)
    {
#if (ADRV9025_INIT_SPI_DEBUG > 0)
        ADRV9025_SPIREADBYTE(cmdStatus[i], cpuAddr->cmdStatusAddr + i, &bytes[i]);
#else
        ADRV9025_SPIREADBYTE("", cpuAddr->cmdStatusAddr + i, &bytes[i]);
#endif


        /* assigning each pending bit from every opcode to a weighted position in statusWord */
        status = (bytes[i] & CPU_PENDING_HI) >> STATUS_WORD_SHIFT_HI;
        status |= (bytes[i] & CPU_PENDING_LO);
        *statusWord |= (uint16_t)(status << (i * 2));

        /* checking each 3-bit error field per nibble for > 0, then assigning to weighted bit in errorWord */
        if ((bytes[i] & CPU_ERR_MASK_LO) > 0)
        {
            *errorWord |= (ERROR_WORD_LO << (i * 2));
        }

        if ((bytes[i] & CPU_ERR_MASK_HI) > 0)
        {
            *errorWord |= (ERROR_WORD_HI << (i * 2));
        }
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuConfigWrite(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint8_t                objectId,
    uint16_t               byteOffset,
    const uint8_t          data[],
    uint8_t                byteCount)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    uint8_t                 extendedData[4] = {0};
    uint8_t                 cmdStatusByte   = 0;
    static const uint8_t    CPU_ERR_MASK    = 0x0E;
    adi_adrv9025_CpuAddr_t* cpuAddr         = NULL;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        data);

    ADRV9025_BUGINFO(__FUNCTION__);

    /* check for valid byteCount */
    if (byteCount == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid byteCount");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* get the particular processor's address map */
    cpuAddr = adrv9025_CpuAddrGet(&device->devStateInfo.cpu,
                                  cpuType);
    ADI_NULL_PTR_RETURN(&device->common,
                        cpuAddr);

    extendedData[0] = byteCount;
    extendedData[1] = 0;
    extendedData[2] = 0;
    extendedData[3] = 0;
    recoveryAction  = adi_adrv9025_CpuMemWrite(device,
                                               cpuAddr->mailboxSetAddr,
                                               &extendedData[0],
                                               1,
                                               ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "CpuConfigWrite failed writing CPU memory");
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              cpuAddr->mailboxSetAddr + 1,
                                              &data[0],
                                              byteCount,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* CPU Object id, byte offset LSB, offset MSB = 0, copy 2 bytes */
    extendedData[0] = ADRV9025_CPU_OBJECTID_CONFIG;
    extendedData[1] = objectId;
    extendedData[2] = (byteOffset & 0xFF);
    extendedData[3] = ((byteOffset >> 8) & 0xFF);

    adi_adrv9025_CpuCmdWrite(device,
                             cpuType,
                             ADRV9025_CPU_SET_OPCODE,
                             &extendedData[0],
                             4);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   cpuType,
                                                   ADRV9025_CPU_SET_OPCODE,
                                                   &cmdStatusByte,
                                                   ADI_ADRV9025_WRITECPUCFG_TIMEOUT_US,
                                                   ADI_ADRV9025_WRITECPUCFG_INTERVAL_US);



    if ((cmdStatusByte & CPU_ERR_MASK) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    cpuType,
                                    ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                             ADRV9025_CPU_OBJECTID_CONFIG,
                                                             cmdStatusByte),
                                    recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuAdcProfilesWrite(
    adi_adrv9025_Device_t*            device,
    const adi_adrv9025_AdcProfiles_t* adcProfiles)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        adcProfiles);

    ADRV9025_BUGINFO(__FUNCTION__);

    recoveryAction = adrv9025_CpuAdcProfilesWrite(device,
                                                  adcProfiles);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "CpuAdcProfilesWrite Failed");

    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuProfileWrite(
    adi_adrv9025_Device_t*     device,
    const adi_adrv9025_Init_t* init)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        init);

    ADRV9025_BUGINFO(__FUNCTION__);

    recoveryAction = adrv9025_CpuProfileWrite(device,
                                              init);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "CpuProfileWrite Failed");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuStart(
    adi_adrv9025_Device_t* device)
{
    int32_t                 recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                 reg            = 0u;
    uint8_t                 mailBox[4]     = {0xFF, 0xFF, 0xFF, 0xFF};
    adi_adrv9025_CpuAddr_t* cpuAddr        = NULL;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADRV9025_BUGINFO(__FUNCTION__);

    /* get the processor's address map for ARM-C and start it. ARM-D will be started by ARM-C if available*/
    cpuAddr = adrv9025_CpuAddrGet(&device->devStateInfo.cpu,
                                  ADI_ADRV9025_CPU_TYPE_C);
    ADI_NULL_PTR_RETURN(&device->common,
                        cpuAddr);

    if (cpuAddr->enabled != ADI_FALSE)
    {
        /* Set MailBox 0xFF for ARM-C. ARM-D doesn't have mailbox get buffer */
        recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                                  cpuAddr->mailboxGetAddr,
                                                  &mailBox[0],
                                                  4,
                                                  ADI_ADRV9025_CPU_MEM_AUTO_INCR);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "CpuStart Failed");
        ADI_ERROR_RETURN(device->common.error.newAction);

        reg = ADRV9025_CPU_MASK_CTL1_DEBUG_ENABLE | ADRV9025_CPU_MASK_CTL1_M3_RUN;
        ADRV9025_SPIWRITEBYTE("ARM_CTL_1",
                              cpuAddr->ctlAddr,
                              reg);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "CPU type C's FW image need to be loaded");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuCmdStatusOpcodeGet(adi_adrv9025_Device_t *device,
                                           adi_adrv9025_CpuType_e cpuType,
                                           uint8_t                opCode,
                                           uint8_t               *cmdStatByte)
{
    uint8_t                 cmdByteIndex        = 0;
    uint8_t                 cmdByte             = 0;
    uint8_t                 cmdByteVerify       = 0;
    uint8_t                 cpuStatusBitShift   = 0;
    adi_adrv9025_CpuAddr_t* cpuAddr             = NULL;
    static const uint8_t    CPU_STATUS_MASK     = 0x0F;
    static const uint8_t    CPU_STATUS_SHIFT_HI = 4;
    static const uint8_t    CPU_PENDING         = 0x01;

#if (ADRV9025_INIT_SPI_DEBUG > 0)
    static const char *cmdStatus[8] = {
        "CPU_CMD_STATUS_0",
        "CPU_CMD_STATUS_1",
        "CPU_CMD_STATUS_2",
        "CPU_CMD_STATUS_3",
        "CPU_CMD_STATUS_4",
        "CPU_CMD_STATUS_5",
        "CPU_CMD_STATUS_6",
        "CPU_CMD_STATUS_7"
    };
#endif

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, cmdStatByte);

    ADRV9025_BUGINFO(__FUNCTION__);

    /* check for even-numbered opCodes including opcode 0, but must not be greater than 31 */
    if ((opCode != (uint8_t)ADRV9025_CPU_STREAM_TRIGGER_OPCODE) && ADRV9025_CPU_OPCODE_VALID(opCode))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid CPU opcode given to adi_adrv9025_CpuCmdStatusOpcodeGet()");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* get the particular processor's address map */
    cpuAddr = adrv9025_CpuAddrGet(&device->devStateInfo.cpu, cpuType);
    ADI_NULL_PTR_RETURN(&device->common, cpuAddr);

    /* Set offset for stream and cpu status read */
    if (opCode == ADRV9025_CPU_STREAM_TRIGGER_OPCODE)
    {
        cpuStatusBitShift = CPU_STATUS_SHIFT_HI + 2;
    }
    else
    {
        cpuStatusBitShift = CPU_STATUS_SHIFT_HI;
    }

    /* calculating command status register offset based on opcode status packing */
    cmdByteIndex = opCode >> 2;

    /* reading the command status register for given opcode */
    /* Two reads back to back for verify correct SPI operation */
#if (ADRV9025_INIT_SPI_DEBUG > 0)
    ADRV9025_SPIREADBYTE(cmdStatus[(cmdByteIndex % 8)], ((uint16_t)ADRV9025_CPU_C_ADDR_CMD_STATUS_0 + cmdByteIndex), &cmdByte);

    ADRV9025_SPIREADBYTE(cmdStatus[(cmdByteIndex % 8)], ((uint16_t)ADRV9025_CPU_C_ADDR_CMD_STATUS_0 + cmdByteIndex), &cmdByteVerify);
#else
    ADRV9025_SPIREADBYTE("", cpuAddr->cmdStatusAddr + cmdByteIndex, &cmdByte);

    ADRV9025_SPIREADBYTE("", cpuAddr->cmdStatusAddr + cmdByteIndex, &cmdByteVerify);
#endif

    /* identifying nibble location in command register for given opcode */
    /* Manually set the pending bit if verification fails */
    if (((opCode >> 1) & (uint8_t)0x01) == (uint8_t)0x01)
    {
        *cmdStatByte = ((cmdByte == cmdByteVerify) ? ((cmdByte >> cpuStatusBitShift) & CPU_STATUS_MASK)
                        : ((cmdByte >> cpuStatusBitShift) & CPU_STATUS_MASK) | CPU_PENDING);
    }
    else
    {
        *cmdStatByte = ((cmdByte == cmdByteVerify) ? (cmdByte & CPU_STATUS_MASK) : (cmdByte & CPU_STATUS_MASK) | CPU_PENDING);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuCmdStatusWait(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint8_t                opCode,
    uint8_t*               cmdStatusByte,
    uint32_t               timeout_us,
    uint32_t               waitInterval_us)
{
    int32_t              recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    int32_t              halError       = (int32_t)ADI_COMMON_HAL_OK;
    uint32_t             eventCheck     = 0;
    uint32_t             numEventChecks = 0;
    static const uint8_t CPU_ERR_MASK   = 0x0E;
    static const uint8_t CPU_PENDING    = 0x01;
    uint32_t             exceptionValue = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        cmdStatusByte);

    ADRV9025_BUGINFO(__FUNCTION__);

    /* check for even-numbered opCodes including opcode 0, but must not be greater than 30 */
    if ((opCode != (uint8_t)ADRV9025_CPU_STREAM_TRIGGER_OPCODE) && ADRV9025_CPU_OPCODE_VALID(opCode))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid CPU opcode given to adi_adrv9025_CpuCmdStatusWait()");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    waitInterval_us = (waitInterval_us > timeout_us) ? timeout_us : waitInterval_us;
    numEventChecks  = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

    /* timeout event check loop */
    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
    {
        /* read status of opcode */
        recoveryAction = adi_adrv9025_CpuCmdStatusOpcodeGet(device,
                                                            cpuType,
                                                            opCode,
                                                            cmdStatusByte);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to get CPU command status");
        ADI_ERROR_RETURN(device->common.error.newAction);


        /* If error code is non zero in [3:1], - return error */
        if ((*cmdStatusByte & CPU_ERR_MASK) > 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV9025_SRC_CPUCMD,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_ADRV9025_ACT_ERR_RESET_CPU,
                             device,
                             "CpuCmdStatusWait() failed due to thrown CPU error. Is device in correct state for calling command?");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* if pending bit is set for opcode of interest and the number of events have not expired, perform wait */
        if (((*cmdStatusByte & CPU_PENDING) > 0) &&
            (eventCheck < numEventChecks))
        {
            halError = adrv9025_hal_Wait_us(&device->common,
                                              waitInterval_us);

            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV9025_SRC_CPUCMD,
                             halError,
                             ADI_COMMON_ACT_ERR_CHECK_TIMER,
                             device,
                             "Timer not working in CpuCmdStatusWait()");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            break;
        }
    }

    /* if CPU Command did not complete within the timeout period */
    if ((*cmdStatusByte & CPU_PENDING) > 0)
    {
        /* Check if exception has occurred */
        recoveryAction = adi_adrv9025_CpuExceptionGet(device,
                                                      cpuType,
                                                      &exceptionValue);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Fail to get CPU exception");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (exceptionValue > 0)
        {
            if (exceptionValue == 0xFFFFFFFF)
            {
	        exceptionValue = ADI_COMMON_ERR_CPU_EXCEPTION;
            }

            /* Return the ARM Exception in Err code */
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV9025_SRC_CPUCMD,
                             exceptionValue,
                             ADI_ADRV9025_ACT_ERR_RESET_CPU,
                             device,
                             "CpuCmdStatusWait() failed due to thrown CPU error. CPU time out ");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV9025_SRC_CPUCMD,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_ADRV9025_ACT_ERR_RESET_CPU,
                             device,
                             "CpuCmdStatusWait() failed due to thrown CPU error. CPU time out ");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuMailboxBusyGet(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint8_t*               mailboxBusy)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        mailboxBusy);

    /* Read arm_command_busy bit in arm_command(0x00c3) register*/
    if (cpuType == ADI_ADRV9025_CPU_TYPE_C)
    {
        recoveryAction = adrv9025_CoreArmCommandBusyBfGet(device,
                                                          ADRV9025_BF_CORE,
                                                          mailboxBusy);
    }
    else if (cpuType == ADI_ADRV9025_CPU_TYPE_D)
    {
        recoveryAction = adrv9025_CoreArmDCommandBusyBfGet(device,
                                                           ADRV9025_BF_CORE,
                                                           mailboxBusy);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Invalid Get for cpu type");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Invalid Get for mailboxBusy");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_CpuCmdWrite(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint8_t                opCode,
    const uint8_t          extendedData[],
    uint8_t                byteCount)
{
    int32_t                 halError        = (int32_t)ADI_COMMON_HAL_OK;
    uint8_t                 cpuCommandBusy  = 0;
    uint8_t                 i               = 0;
    uint32_t                timeout_us      = ADI_ADRV9025_SENDCPUCMD_TIMEOUT_US;
    uint32_t                waitInterval_us = ADI_ADRV9025_SENDCPUCMD_INTERVAL_US;
    uint32_t                eventCheck      = 0;
    uint32_t                numEventChecks  = 0;
    int32_t                 recoveryAction  = ADI_COMMON_ACT_NO_ACTION;
    adi_adrv9025_CpuAddr_t* cpuAddr         = NULL;
    uint32_t                exceptionValue  = 0;

#define EXT_CMD_BYTE_MAX 4

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if (byteCount > 0)
    {
        ADI_NULL_PTR_RETURN(&device->common,
                            extendedData);
    }

    ADRV9025_BUGINFO(__FUNCTION__);

    /* check for even-numbered opCodes only including opcode 0, but must not be greater than opCode 30 */
    if ((opCode != (uint8_t)ADRV9025_CPU_STREAM_TRIGGER_OPCODE) && ADRV9025_CPU_OPCODE_VALID(opCode))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid CPU opcode given to adi_adrv9025_CpuCmdWrite()");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* the number of valid extended data bytes is from 0-4 */
    if (byteCount > EXT_CMD_BYTE_MAX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid number of extended data in adi_adrv9025_CpuCmdWrite()");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* setting a 2 second timeout for mailbox busy bit to be clear (can't send an cpu mailbox command until mailbox is ready) */

    waitInterval_us = (waitInterval_us > timeout_us) ? timeout_us : waitInterval_us;
    numEventChecks  = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

    /* timeout event loop to permit non-blocking of thread */
    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
    {
        recoveryAction = adi_adrv9025_CpuMailboxBusyGet(device,
                                                        cpuType,
                                                        &cpuCommandBusy);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Invalid Get for adi_adrv9025_CpuMailboxBusyGet()");
        ADI_ERROR_RETURN(device->common.error.newAction);


        if (cpuCommandBusy == ADI_TRUE)
        {
            halError = adrv9025_hal_Wait_us(&device->common,
                                              waitInterval_us);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_ADI_HAL,
                             halError,
                             ADI_COMMON_ACT_ERR_CHECK_TIMER,
                             device,
                             "Timer not working in adi_adrv9025_CpuCmdWrite()");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            break;
        }
    }

    /* if busy bit remains set after timeout event loop function is exited, otherwise command is sent after extended bytes */
    if (cpuCommandBusy == ADI_TRUE)
    {
        /* Check if exception has occurred */
        recoveryAction = adi_adrv9025_CpuExceptionGet(device,
                                                      cpuType,
                                                      &exceptionValue);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Fail to get CPU exception");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (exceptionValue > 0)
        {
	        if (exceptionValue == 0xFFFFFFFF)
	        {
		        exceptionValue = ADI_COMMON_ERR_CPU_EXCEPTION;
	        }
            /* Return the ARM Exception in Err code */
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV9025_SRC_CPUCMD,
                             exceptionValue,
                             ADI_ADRV9025_ACT_ERR_RESET_CPU,
                             device,
                             "CPU Mailbox Busy. Command not executed in adi_adrv9025_CpuCmdWrite()");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV9025_SRC_CPUCMD,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_ADRV9025_ACT_ERR_RESET_CPU,
                             device,
                             "CPU Mailbox Busy. Command not executed in adi_adrv9025_CpuCmdWrite()");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    else
    {
        /* get the particular processor's address map */
        cpuAddr = adrv9025_CpuAddrGet(&device->devStateInfo.cpu,
                                      cpuType);
        ADI_NULL_PTR_RETURN(&device->common,
                            cpuAddr);

        if (byteCount > 0)
        {
            for (i = 0; i < byteCount; i++)
            {
                ADRV9025_SPIWRITEBYTE("Extend Data",
                                      cpuAddr->extCmdAddr + i,
                                      extendedData[i]);
            }
        }

        ADRV9025_SPIWRITEBYTE("CPU_COMMAND",
                              cpuAddr->cmdAddr,
                              opCode);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuCmdWriteWait(adi_adrv9025_Device_t *device,
                                     adi_adrv9025_CpuType_e cpuType,
                                     uint8_t                armOpCode,
                                     const uint8_t          extendedData[],
                                     uint8_t                byteCount,
                                     uint32_t               waitTimeoutUs,
                                     uint32_t               waitIntervalUs,
                                     uint8_t                armObjId)
{
    uint8_t cmdStatusByte = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    adi_adrv9025_CpuCmdWrite(device,
                             cpuType,
                             armOpCode,
                             extendedData,
                             byteCount);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* waiting for command to finish successfully */
    adi_adrv9025_CpuCmdStatusWait(device,
                                  ADI_ADRV9025_CPU_TYPE_C,
                                  armOpCode,
                                  &cmdStatusByte,
                                  waitTimeoutUs,
                                  waitIntervalUs);

    if ((cmdStatusByte >> ADRV9025_CPU_ERROR_SHIFT) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(armOpCode,
                                                             armObjId,
                                                             cmdStatusByte),
                                    ADI_COMMON_ACT_ERR_RESET_FEATURE);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuConfigRead(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint8_t                objectId,
    uint16_t               byteOffset,
    uint8_t                returnData[],
    uint8_t                byteCount)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    uint8_t                 extendedData[4] = {0};
    uint8_t                 cmdStatusByte   = 0;
    static const uint8_t    CPU_ERR_MASK    = 0x0E;
    static const uint8_t    AUTO_INC_BIT    = 0u;
    adi_adrv9025_CpuAddr_t* cpuAddr         = NULL;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        returnData);

    ADRV9025_BUGINFO(__FUNCTION__);

    /* check for valid byteCount */
    if (byteCount == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid byteCount");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    extendedData[0] = byteCount;
    extendedData[1] = 0;
    extendedData[2] = 0;
    extendedData[3] = 0;

    /* get the particular processor's address map */
    cpuAddr = adrv9025_CpuAddrGet(&device->devStateInfo.cpu,
                                  cpuType);
    ADI_NULL_PTR_RETURN(&device->common,
                        cpuAddr);

    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              cpuAddr->mailboxGetAddr,
                                              &extendedData[0],
                                              1u,
                                              AUTO_INC_BIT);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "adi_adrv9025_CpuMemWrite() failed");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* CPU Object id, byte offset LSB, offset MSB = 0, byteCount will read that number of bytes */
    extendedData[0] = ADRV9025_CPU_OBJECTID_CONFIG;
    extendedData[1] = objectId;
    extendedData[2] = (byteOffset & 0xFF);
    extendedData[3] = ((byteOffset >> 8) & 0xFF);

    /* send the read config CPU opcode */
    adi_adrv9025_CpuCmdWrite(device,
                             cpuType,
                             ADRV9025_CPU_GET_OPCODE,
                             &extendedData[0],
                             4);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* check the command status for timeout */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   cpuType,
                                                   ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   ADI_ADRV9025_READCPUCFG_TIMEOUT_US,
                                                   ADI_ADRV9025_READCPUCFG_INTERVAL_US);



    if ((cmdStatusByte & CPU_ERR_MASK) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                             ADRV9025_CPU_OBJECTID_CONFIG,
                                                             cmdStatusByte),
                                    recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    adi_adrv9025_CpuMemRead(device,
                            cpuAddr->mailboxGetAddr,
                            &returnData[0],
                            byteCount,
                            AUTO_INC_BIT);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuFwVersionGet(
    adi_adrv9025_Device_t*       device,
    adi_adrv9025_CpuType_e       cpuType,
    adi_adrv9025_CpuFwVersion_t* fwVersion)
{
    int32_t                 recoveryAction                       = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                 ver[8]                               = {0};
    uint32_t                fullVersion                          = 0;
    adi_adrv9025_CpuAddr_t* cpuAddr                              = NULL;
    static const uint8_t    CPU_FW_BUILD_DEBUG                   = 0x01;
    static const uint8_t    CPU_FW_BUILD_TESTOBJ                 = 0x04;
    static const uint8_t    CPU_FW_BUILD_USES_FOUR_DIGIT_VERSION = 0x01;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        fwVersion);

    ADRV9025_BUGINFO(__FUNCTION__);

    if ((device->devStateInfo.devState & ADI_ADRV9025_STATE_CPULOADED) != ADI_ADRV9025_STATE_CPULOADED)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV9025_SRC_CPUCMD,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "CPU binary must be loaded before calling adi_adrv9025_CpuFwVersionGet()");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* get the particular processor's address map */
    cpuAddr = adrv9025_CpuAddrGet(&device->devStateInfo.cpu,
                                  cpuType);
    ADI_NULL_PTR_RETURN(&device->common,
                        cpuAddr);

    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             cpuAddr->versionAddr,
                                             &ver[0],
                                             sizeof(ver),
                                             0);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to read CPU memory");
    ADI_ERROR_RETURN(device->common.error.newAction);

    fullVersion = (((uint32_t)ver[0]) | (((uint32_t)ver[1]) << 8) | (((uint32_t)ver[2]) << 16) | (((uint32_t)ver[3]) << 24));


    if (ver[5] & CPU_FW_BUILD_USES_FOUR_DIGIT_VERSION)
    {
        fwVersion->majorVer = (uint8_t)(fullVersion >> 28) & 0x0F;
        fwVersion->minorVer = (uint8_t)(fullVersion >> 24) & 0x0F;
        fwVersion->maintVer = (uint8_t)(fullVersion >> 16) & 0xFF;
        fwVersion->rcVer    = (uint16_t)(fullVersion & 0xFFFF);
    }
    else
    {
        fwVersion->rcVer    = (uint8_t)(fullVersion % 100);
        fwVersion->minorVer = (uint8_t)((fullVersion / 100) % 100);
        fwVersion->majorVer = (uint8_t)(fullVersion / 10000);
        fwVersion->maintVer = 0;
    }

    if (ver[4] & CPU_FW_BUILD_DEBUG)
    {
        fwVersion->cpuFwBuildType = ADI_ADRV9025_CPU_FW_BUILD_DEBUG;
    }
    else if (ver[4] & CPU_FW_BUILD_TESTOBJ)
    {
        fwVersion->cpuFwBuildType = ADI_ADRV9025_CPU_FW_BUILD_TESTOBJ;
    }
    else
    {
        fwVersion->cpuFwBuildType = ADI_ADRV9025_CPU_FW_BUILD_RELEASE;
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuMemRead(
    adi_adrv9025_Device_t* device,
    uint32_t               address,
    uint8_t                returnData[],
    uint32_t               byteCount,
    uint8_t                autoIncrement)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        returnData);

    ADRV9025_BUGINFO(__FUNCTION__);

    /* check for bytecount */
    if (byteCount == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_NULL_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         byteCount,
                         "byteCount is zero");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (adrv9025_CpuValidateMemoryAddr(&device->devStateInfo.cpu,
                                       address,
                                       byteCount) == ADI_FALSE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         address,
                         "Invalid CPU Memory Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_CpuDmaMemRead(device,
                                            address,
                                            returnData,
                                            byteCount,
                                            autoIncrement);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "adrv9025_CpuDmaMemRead() failed during DMA transaction");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuImageWrite(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint32_t               byteOffset,
    const uint8_t          binary[],
    uint32_t               byteCount)
{
    int32_t                 recoveryAction                      = ADI_COMMON_ACT_NO_ACTION;
    adi_adrv9025_CpuAddr_t* cpuAddr                             = NULL;
    static const uint32_t   ADRV9025_ADDR_DEVICE_PROFILE_OFFSET = 0x00000274;
    static const uint32_t   ADRV9025_ADDR_ADC_PROFILE_OFFSET    = 0x00000278;

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

    /* extraction of stack pointer and boot address from top of array */
    if (byteOffset == 0)
    {
        if (byteCount < 8)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             byteCount,
                             "Invalid byteCount. Must be larger than 8.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        adrv9025_CpuStackPtrWrite(device,
                                  cpuType,
                                  (const uint32_t *)binary);
        ADI_ERROR_RETURN(device->common.error.newAction);
        adrv9025_CpuBootAddrWrite(device,
                                  cpuType,
                                  (const uint32_t *)&binary[4]);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (cpuType == ADI_ADRV9025_CPU_TYPE_C)
    {
        /* extraction of profile address */
        if ((ADRV9025_ADDR_DEVICE_PROFILE_OFFSET >= byteOffset) &&
            (ADRV9025_ADDR_DEVICE_PROFILE_OFFSET < (byteOffset + byteCount + 4)))
        {
            device->devStateInfo.cpu.devProfileAddr = adrv9025_CpuIntFromBytesGet(
                                                                                  binary + ADRV9025_ADDR_DEVICE_PROFILE_OFFSET - byteOffset,
                                                                                  4);
        }

        /* extraction of ADC profile address */
        if ((ADRV9025_ADDR_ADC_PROFILE_OFFSET >= byteOffset) &&
            (ADRV9025_ADDR_ADC_PROFILE_OFFSET < (byteOffset + byteCount + 4)))
        {
            device->devStateInfo.cpu.adcProfileAddr = adrv9025_CpuIntFromBytesGet(
                                                                                  binary + ADRV9025_ADDR_ADC_PROFILE_OFFSET - byteOffset,
                                                                                  4);
        }
    }

    /* get the particular processor's address map */
    cpuAddr = adrv9025_CpuAddrGet(&device->devStateInfo.cpu,
                                  cpuType);
    ADI_NULL_PTR_RETURN(&device->common,
                        cpuAddr);

    recoveryAction = adi_adrv9025_CpuMemWrite(
                                              device,
                                              cpuAddr->progStartAddr + byteOffset,
                                              &binary[0],
                                              byteCount,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "CpuImageWrite Failed");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* mark the image loaded */
    cpuAddr->enabled = ADI_TRUE;

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuMemWrite(
    adi_adrv9025_Device_t* device,
    uint32_t               address,
    const uint8_t          data[],
    uint32_t               byteCount,
    uint8_t                autoIncrement)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        data);

    ADRV9025_BUGINFO(__FUNCTION__);

    /* check for bytecount*/
    if (byteCount == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_NULL_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         byteCount,
                         "byteCount is zero");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (adrv9025_CpuValidateMemoryAddr(&device->devStateInfo.cpu,
                                       address,
                                       byteCount) == ADI_FALSE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         address,
                         "Invalid CPU Memory Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_CpuDmaMemWrite(device,
                                             address,
                                             data,
                                             byteCount,
                                             autoIncrement);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "CpuMemWrite failed during DMA transaction");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_CpuExceptionGet(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint32_t*              exceptionValue)
{
    int32_t        recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    uint8_t        exceptionArray[4] = {0};
    uint32_t       armExceptionAddr  = 0; /* Exception Flag Memory */

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        exceptionValue);

    if (cpuType == ADI_ADRV9025_CPU_TYPE_C)
    {
        armExceptionAddr = 0x20028210;
    }
    else if (cpuType == ADI_ADRV9025_CPU_TYPE_D)
    {
        armExceptionAddr = 0x20000000;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            recoveryAction,
            NULL,
            "Invalid CPU type selection for CpuExceptionGet");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

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
    ADI_ERROR_RETURN(device->common.error.newAction);

    *exceptionValue = (uint32_t)(exceptionArray[0] | (exceptionArray[1] << 8) | (exceptionArray[2] << 16) | (exceptionArray[3] << 24));

    return device->common.error.newAction;
}

int32_t adi_adrv9025_CpuSystemErrorGet(adi_adrv9025_Device_t* device,
                                       adi_adrv9025_CpuType_e cpuType,
                                       const char**           errorMessage)
{
    int32_t           recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    const char* const localErrorMessage = "No CPU system error detected";
    uint16_t          systemErrCode     = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        errorMessage);

    recoveryAction = adrv9025_CpuSystemErrCodeGet(device,
                                                  cpuType,
                                                  &systemErrCode);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     systemErrCode,
                     "Error while reading Arm System Error");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Report Arm mailboxErrCode*/
    if (systemErrCode > 0)
    {
        *errorMessage = adrv9025_CpuErrCodeToString(device,
                                                    (uint32_t)systemErrCode);
        if (*errorMessage == NULL)
        {
            /* We have an error code but it doesn't match anything in error list */
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             systemErrCode,
                             ADI_COMMON_ACT_ERR_RESET_FEATURE,
                             systemErrCode,
                             "Error code couldn't be found, please check errCode field in device error structure");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    else
    {
        /* There is no ARM system error to be reported */
        *errorMessage = localErrorMessage;
    }

    return ADI_COMMON_ACT_NO_ACTION;
}
