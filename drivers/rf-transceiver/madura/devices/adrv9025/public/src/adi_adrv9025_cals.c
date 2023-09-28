// SPDX-License-Identifier: GPL-2.0
/**
* \file adi_adrv9025_cals.c
* \brief Contains Calibration features related function implementation defined in
* adi_adrv9025_cals.h
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "adi_adrv9025_cals.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025.h"
#include "adi_adrv9025_cpu.h"
#include "../../private/include/adrv9025_init.h"
#include "../../private/include/adrv9025_cpu.h"
#include "../../private/include/adrv9025_cpu_macros.h"
#include "../../private/include/adrv9025_reg_addr_macros.h"
#include "../../private/include/adrv9025_cals.h"
#include "../../private/include/adrv9025_bf_orx.h"
#include "../../private/include/adrv9025_bf_rx.h"

#ifdef _RELEASE_BUILD_
    #line __LINE__ "adi_adrv9025_cals.c"
#endif

int32_t adi_adrv9025_InitCalsRun(adi_adrv9025_Device_t*   device,
                                 adi_adrv9025_InitCals_t* initCals)
{
    int32_t recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    uint8_t payloadMailbox[4] = {0};
    uint8_t payload[1]        = {0};

    static const uint8_t CAL_CHANNEL_ALL_MASK = 0x0F;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        initCals);

    recoveryAction = adrv9025_InitCalStructureCheck(device,
                                                    initCals);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    initCals->calMask &= ADI_ADRV9025_MAX_INIT_CAL_MASK;
    payloadMailbox[0] = (uint8_t)initCals->calMask;
    payloadMailbox[1] = (uint8_t)(initCals->calMask >> 8);
    payloadMailbox[2] = (uint8_t)(initCals->calMask >> 16);
    payloadMailbox[3] = (uint8_t)(initCals->calMask >> 24);

    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              ADRV9025_CPU_C_ADDR_MAILBOX_RUN_INIT,
                                              &payloadMailbox[0],
                                              4,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to write CPU memory");
    ADI_ERROR_RETURN(device->common.error.newAction);

    payload[0] = initCals->channelMask & CAL_CHANNEL_ALL_MASK;
    adi_adrv9025_CpuCmdWrite(device,
                             ADI_ADRV9025_CPU_TYPE_C,
                             ADRV9025_CPU_RUNINIT_OPCODE,
                             &payload[0],
                             1);
    ADI_ERROR_RETURN(device->common.error.newAction);

    device->devStateInfo.devState = (adi_adrv9025_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV9025_STATE_INITCALS_RUN);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_InitCalsWait(adi_adrv9025_Device_t* device,
                                  uint32_t               timeoutMs,
                                  uint8_t*               errorFlag)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t cmdStatusByte  = 0;
    uint8_t errFlag        = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        errorFlag);

    static const uint16_t TIMEOUT_MS_FACTOR = 1000;

    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   ADRV9025_CPU_RUNINIT_OPCODE,
                                                   &cmdStatusByte,
                                                   (timeoutMs * TIMEOUT_MS_FACTOR),
                                                   ADI_ADRV9025_INITCALSWAIT_INTERVAL_US);
    errFlag = (cmdStatusByte >> 1);



    /* Don't update errorFlag if SPI error because errorFlag could be a random */
    /* value but update error flag for other recovery action types */
    if (recoveryAction == ADI_COMMON_ACT_ERR_RESET_INTERFACE)
    {
        *errorFlag = 0;
    }
    else
    {
        *errorFlag = errFlag;
    }

    /* ARM error handler to provide valid recovery action based on ARM error code */
    if (errFlag > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_RUNINIT_OPCODE,
                                                             0,
                                                             cmdStatusByte),
                                    recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_InitCalsCheckCompleteGet(adi_adrv9025_Device_t* device,
                                              uint8_t*               areCalsRunning,
                                              uint8_t*               errorFlag)
{
    int32_t              recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t              cmdStatusByte  = 0;
    uint8_t              armErrorFlag   = 0;
    static const uint8_t ARM_PENDING    = 0x01;
    static const uint8_t ARM_ERR_SHIFT  = 1;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        areCalsRunning);

    /* read the command status register for the initialization calibration opcode */
    recoveryAction = adi_adrv9025_CpuCmdStatusOpcodeGet(device,
                                                        ADI_ADRV9025_CPU_TYPE_C,
                                                        ADRV9025_CPU_RUNINIT_OPCODE,
                                                        &cmdStatusByte);
    /* don't check cmdStatusByte until return parameters are updated */



    armErrorFlag = (cmdStatusByte >> ARM_ERR_SHIFT);

    if (recoveryAction == ADI_COMMON_ACT_ERR_RESET_INTERFACE)
    {
        *areCalsRunning = 0;
    }
    else
    {
        *areCalsRunning = (cmdStatusByte & ARM_PENDING);
    }

    /* Don't update errorFlag if SPI error because errorFlag could be a random */
    /* value but update error flag for other recovery action types */
    if (errorFlag != NULL)
    {
        if (recoveryAction == ADI_COMMON_ACT_ERR_RESET_INTERFACE)
        {
            *errorFlag = 0;
        }
        else
        {
            *errorFlag = armErrorFlag;
        }
    }

    if (armErrorFlag > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_RUNINIT_OPCODE,
                                                             0,
                                                             cmdStatusByte),
                                    recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_InitCalsAbort(adi_adrv9025_Device_t* device,
                                   uint32_t*              calsCompleted)
{
    uint8_t cmdStatusByte          = 0;
    uint8_t calCompleteBitField[4] = {0};
    uint8_t extData[1]             = {ADRV9025_CPU_OBJECTID_INITCAL_STATUS};

    static const uint8_t ARM_ERR_SHIFT = 1;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADRV9025_BUGINFO(__FUNCTION__);

    /* send ARM abort opcode */
    adi_adrv9025_CpuCmdWrite(device,
                             ADI_ADRV9025_CPU_TYPE_C,
                             ADRV9025_CPU_ABORT_OPCODE,
                             0,
                             0);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* wait for command completion */
    adi_adrv9025_CpuCmdStatusWait(device,
                                  ADI_ADRV9025_CPU_TYPE_C,
                                  ADRV9025_CPU_ABORT_OPCODE,
                                  &cmdStatusByte,
                                  ADI_ADRV9025_INITCALSABORT_TIMEOUT_US,
                                  ADI_ADRV9025_INITCALSABORT_INTERVAL_US);



    if ((cmdStatusByte >> ARM_ERR_SHIFT) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_ABORT_OPCODE,
                                                             0,
                                                             cmdStatusByte),
                                    ADI_COMMON_ACT_ERR_RESET_FULL);
    }
    else
    {
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (calsCompleted != NULL)
    {
        /* send ARM get opcode for the init cals completed */
        adi_adrv9025_CpuCmdWrite(device,
                                 ADI_ADRV9025_CPU_TYPE_C,
                                 ADRV9025_CPU_GET_OPCODE,
                                 &extData[0],
                                 sizeof(extData));
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* wait for command completion */
        adi_adrv9025_CpuCmdStatusWait(device,
                                      ADI_ADRV9025_CPU_TYPE_C,
                                      ADRV9025_CPU_ABORT_OPCODE,
                                      &cmdStatusByte,
                                      ADI_ADRV9025_INITCALSABORT_TIMEOUT_US,
                                      ADI_ADRV9025_INITCALSABORT_INTERVAL_US);


        if ((cmdStatusByte >> ARM_ERR_SHIFT) > 0)
        {
            adrv9025_CpuCmdErrorHandler(device,
                                        ADI_ADRV9025_CPU_TYPE_C,
                                        ADI_ADRV9025_SRC_CPUCMD,
                                        ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                                 extData[0],
                                                                 cmdStatusByte),
                                        ADI_COMMON_ACT_ERR_RESET_FULL);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* read the ARM memory for the init cals completed bit field */
        adi_adrv9025_CpuMemRead(device,
                                ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                                &calCompleteBitField[0],
                                4,
                                0);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* form the calsComplted word and return if requested */
        *calsCompleted = (((uint32_t)calCompleteBitField[3]) << 24) | (((uint32_t)calCompleteBitField[2]) << 16) | (((uint32_t)calCompleteBitField[1]) << 8) | (
            ((uint32_t)calCompleteBitField[0]));
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_InitCalsDetailedStatusGet(adi_adrv9025_Device_t*        device,
                                               adi_adrv9025_InitCalStatus_t* initStatus)
{
    uint8_t cmdStatusByte   = 0;
    uint8_t calBitField[48] = {0};
    uint8_t i               = 0;
    uint8_t offset          = 0;

    static const uint8_t InitCalDoneObjectID = 0x43;
    static const uint8_t byteCnt             = 48;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        initStatus);

    ADRV9025_BUGINFO(__FUNCTION__);

    /* sending get command to ARM */
    adi_adrv9025_CpuCmdWrite(device,
                             ADI_ADRV9025_CPU_TYPE_C,
                             ADRV9025_CPU_GET_OPCODE,
                             &InitCalDoneObjectID,
                             1);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* waiting for command completion */
    adi_adrv9025_CpuCmdStatusWait(device,
                                  ADI_ADRV9025_CPU_TYPE_C,
                                  ADRV9025_CPU_GET_OPCODE,
                                  &cmdStatusByte,
                                  ADI_ADRV9025_INITCALSTATUSGET_TIMEOUT_US,
                                  ADI_ADRV9025_INITCALSTATUSGET_INTERVAL_US);



    if ((cmdStatusByte >> 1) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                             InitCalDoneObjectID,
                                                             cmdStatusByte),
                                    ADI_COMMON_ACT_ERR_RESET_FEATURE);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* reading the calibration bit field value */
    adi_adrv9025_CpuMemRead(device,
                            ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &calBitField[0],
                            byteCnt,
                            0);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* getting the offending error code */
    initStatus->initErrCode = (((uint32_t)calBitField[3]) << 24) | (((uint32_t)calBitField[2]) << 16) | (((uint32_t)calBitField[1]) << 8) | (((uint32_t)
        calBitField[0]));

    /* getting the object ID reporting the error */
    initStatus->initErrCal = (((uint32_t)calBitField[7]) << 24) | (((uint32_t)calBitField[6]) << 16) | (((uint32_t)calBitField[5]) << 8) | (((uint32_t)
        calBitField[4]));

    /* getting the Duration time in uSec of the last init cals run  */
    initStatus->calsDurationUsec = (((uint32_t)calBitField[11]) << 24) | (((uint32_t)calBitField[10]) << 16) | (((uint32_t)calBitField[9]) << 8) | (((uint32_t)
        calBitField[8]));

    /* getting the number of calibrations since power up */
    for (i = 0; i < 4; i++)
    {
        offset                          = i * 4 + 0xC;
        initStatus->calsSincePowerUp[i] = (((uint32_t)calBitField[offset + 3]) << 24) | (((uint32_t)calBitField[offset + 2]) << 16) | (((uint32_t)calBitField[
            offset + 1]) << 8) | (((uint32_t)calBitField[offset + 0]));
    }

    /* getting the number of calibrations since the last sequence */
    for (i = 0; i < 4; i++)
    {
        offset                     = i * 4 + 0x1C;
        initStatus->calsLastRun[i] = (((uint32_t)calBitField[offset + 3]) << 24) | (((uint32_t)calBitField[offset + 2]) << 16) | (((uint32_t)calBitField[offset
            + 1]) << 8) | (((uint32_t)calBitField[offset + 0]));
    }



    if (initStatus->initErrCode > 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV9025_SRC_CPUINITCALS,
                         initStatus->initErrCode,
                         ADI_ADRV9025_ACT_ERR_RESET_CPU,
                         device,
                         adrv9025_CalsErrMsgGet(device, initStatus->initErrCode));
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TrackingCalsEnableSet(adi_adrv9025_Device_t*                  device,
                                           uint64_t                                enableMask,
                                           adi_adrv9025_TrackingCalEnableDisable_e enableDisableFlag)
{
    uint8_t armData[8] = {0};
    uint8_t extData[2] = {0};
    uint32_t fwStatus = 0;
    uint8_t  status   = 0;

    static const uint8_t TRACKING_CAL_BITMAP_MODE = 0x80;
    static const uint8_t TRACKING_CAL_ENABLE_MODE = 0x40;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* read firmware state to make sure ARM is in ready/IDLE mode */
    ADRV9025_SPIREADBYTE("ARM_CMD_STATUS_8",
                         ADRV9025_CPU_C_ADDR_CMD_STATUS_8,
                         &status);
    ADI_ERROR_RETURN(device->common.error.newAction);

    fwStatus = (uint32_t)status;


    /* throw error if not in READY state */
    if (fwStatus != ADRV9025_CPU_FW_STATUS_READY)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV9025_SRC_CPUCMD,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_ADRV9025_ACT_ERR_RESET_CPU,
                         device,
                         "Device not in READY state. Error in adi_adrv9025_TrackingCalsEnableSet()\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    extData[0] = ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE;

    /* Selecting Enable/ Disable operation by setting the Enable/ Disable bit */
    switch (enableDisableFlag)
    {
    case ADI_ADRV9025_TRACKING_CAL_DISABLE:
        extData[1] = TRACKING_CAL_BITMAP_MODE;
        break;
    case ADI_ADRV9025_TRACKING_CAL_ENABLE:
        extData[1] = TRACKING_CAL_ENABLE_MODE | TRACKING_CAL_BITMAP_MODE;
        break;
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         enableDisableFlag,
                         "Invalid enableDisableFlag");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* populating armData array */
    armData[0] = (uint8_t)(enableMask & 0xFF);
    armData[1] = (uint8_t)((enableMask >> 8) & 0xFF);
    armData[2] = (uint8_t)((enableMask >> 16) & 0xFF);
    armData[3] = (uint8_t)((enableMask >> 24) & 0xFF);
    armData[4] = (uint8_t)((enableMask >> 32) & 0xFF);
    armData[5] = (uint8_t)((enableMask >> 40) & 0xFF);
    armData[6] = (uint8_t)((enableMask >> 48) & 0xFF);
    armData[7] = (uint8_t)((enableMask >> 56) & 0xFF);

    /* writing armData to ARM memory */
    adi_adrv9025_CpuMemWrite(device,
                             ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                             &armData[0],
                             8,
                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* sending write config command and wait for status */
    adi_adrv9025_CpuCmdWriteWait(
                                 device,
                                 ADI_ADRV9025_CPU_TYPE_C,
                                 ADRV9025_CPU_SET_OPCODE,
                                 &extData[0],
                                 2,
                                 ADI_ADRV9025_ENTRACKINGCALS_TIMEOUT_US,
                                 ADI_ADRV9025_ENTRACKINGCALS_INTERVAL_US,
                                 ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TrackingCalsEnableGet(adi_adrv9025_Device_t* device,
                                           uint64_t*              enableMask)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;
    uint8_t              extData               = 0;
    uint8_t              armData[8]            = {0};
    //uint8_t cmdStatusByte = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        enableMask);

    extData = (uint8_t)ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE;

    /* Executing TRACKING_CALS_STATE Arm Command */
    adi_adrv9025_CpuCmdWriteWait(
                                 device,
                                 ADI_ADRV9025_CPU_TYPE_C,
                                 ADRV9025_CPU_GET_OPCODE,
                                 &extData,
                                 sizeof(extData),
                                 ADI_ADRV9025_ENTRACKINGCALS_TIMEOUT_US,
                                 ADI_ADRV9025_ENTRACKINGCALS_INTERVAL_US,
                                 ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Test tone config from ARM mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            8,
                            ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing Tracking Enable/Disable status (first  8 bytes) */
    *enableMask = (uint64_t)(((uint64_t)armData[0]) |
        ((uint64_t)armData[1] << 8) |
        ((uint64_t)armData[2] << 16) |
        ((uint64_t)armData[3] << 24) |
        ((uint64_t)armData[4] << 32) |
        ((uint64_t)armData[5] << 40) |
        ((uint64_t)armData[6] << 48) |
        ((uint64_t)armData[7] << 56));

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TrackingCalDeserializerStatusGet(adi_adrv9025_Device_t*               device,
                                                      uint8_t                              desLane,
                                                      adi_adrv9025_DesTrackingCalStatus_t* desStatus)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;

    uint8_t extData[3]  = {0};
    uint8_t armData[20] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        desStatus);

    if ((desLane < 0) || (desLane > 4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         desLane,
                         "Invalid Deserializer Lane selected");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    extData[0] = ADRV9025_CPU_OBJECTID_CAL_STATUS;
    extData[1] = ADRV9025_CPU_OBJECTID_DES_TRACKING;
    extData[2] = desLane;

    /* Executing Tracking cals Tx LOL status get Arm Command */
    adi_adrv9025_CpuCmdWriteWait(
                                 device,
                                 ADI_ADRV9025_CPU_TYPE_C,
                                 ADRV9025_CPU_GET_OPCODE,
                                 &extData[0],
                                 sizeof(extData),
                                 ADI_ADRV9025_GETDESSTATUS_TIMEOUT_US,
                                 ADI_ADRV9025_GETDESSTATUS_INTERVAL_US,
                                 ADRV9025_CPU_OBJECTID_DES_TRACKING);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Tx LOL config from ARM mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            sizeof(armData),
                            ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing Tx Lol Status into the structure. */
    desStatus->errorCode = (uint32_t)(((uint32_t)armData[0]) |
        ((uint32_t)armData[1] << 8) |
        ((uint32_t)armData[2] << 16) |
        ((uint32_t)armData[3] << 24));

    desStatus->percentComplete = (uint32_t)(((uint32_t)armData[4]) |
        ((uint32_t)armData[5] << 8) |
        ((uint32_t)armData[6] << 16) |
        ((uint32_t)armData[7] << 24));

    desStatus->varianceMetric = (uint32_t)(((uint32_t)armData[8]) |
        ((uint32_t)armData[9] << 8) |
        ((uint32_t)armData[10] << 16) |
        ((uint32_t)armData[11] << 24));

    desStatus->iterCount = (uint32_t)(((uint32_t)armData[12]) |
        ((uint32_t)armData[13] << 8) |
        ((uint32_t)armData[14] << 16) |
        ((uint32_t)armData[15] << 24));

    desStatus->updateCount = (uint32_t)(((uint32_t)armData[16]) |
        ((uint32_t)armData[17] << 8) |
        ((uint32_t)armData[18] << 16) |
        ((uint32_t)armData[19] << 24));

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TrackingCalTxLolStatusGet(adi_adrv9025_Device_t*      device,
                                               adi_adrv9025_TxChannels_e   txChannel,
                                               adi_adrv9025_TxLolStatus_t* txLolStatus)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;

    uint8_t extData[3]  = {ADRV9025_CPU_OBJECTID_CAL_STATUS, ADRV9025_CPU_OBJECTID_TXLOL_TRACKING, 0};
    uint8_t armData[20] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txLolStatus);

    switch (txChannel)
    {
    case ADI_ADRV9025_TX1:
        extData[2] = 0; /*!< Tx0 channel enabled */
        break;
    case ADI_ADRV9025_TX2:
        extData[2] = 1; /*!< Tx1 channel enabled */
        break;
    case ADI_ADRV9025_TX3:
        extData[2] = 2; /*!< Tx2 channel enabled */
        break;
    case ADI_ADRV9025_TX4:
        extData[2] = 3; /*!< Tx3 channel enabled */
        break;
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid Tx Channel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Executing Tracking cals Tx LOL status get Arm Command */
    adi_adrv9025_CpuCmdWriteWait(
                                 device,
                                 ADI_ADRV9025_CPU_TYPE_C,
                                 ADRV9025_CPU_GET_OPCODE,
                                 &extData[0],
                                 sizeof(extData),
                                 ADI_ADRV9025_GETTXLOLSTATUS_TIMEOUT_US,
                                 ADI_ADRV9025_GETTXLOLSTATUS_INTERVAL_US,
                                 ADRV9025_CPU_OBJECTID_TXLOL_TRACKING);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Tx LOL config from ARM mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            sizeof(armData),
                            ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing Tx Lol Status into the structure. */
    txLolStatus->errorCode = (uint32_t)(((uint32_t)armData[0]) |
        ((uint32_t)armData[1] << 8) |
        ((uint32_t)armData[2] << 16) |
        ((uint32_t)armData[3] << 24));

    txLolStatus->percentComplete = (uint32_t)(((uint32_t)armData[4]) |
        ((uint32_t)armData[5] << 8) |
        ((uint32_t)armData[6] << 16) |
        ((uint32_t)armData[7] << 24));

    txLolStatus->varianceMetric = (uint32_t)(((uint32_t)armData[8]) |
        ((uint32_t)armData[9] << 8) |
        ((uint32_t)armData[10] << 16) |
        ((uint32_t)armData[11] << 24));

    txLolStatus->iterCount = (uint32_t)(((uint32_t)armData[12]) |
        ((uint32_t)armData[13] << 8) |
        ((uint32_t)armData[14] << 16) |
        ((uint32_t)armData[15] << 24));

    txLolStatus->updateCount = (uint32_t)(((uint32_t)armData[16]) |
        ((uint32_t)armData[17] << 8) |
        ((uint32_t)armData[18] << 16) |
        ((uint32_t)armData[19] << 24));

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TrackingCalTxQecStatusGet(adi_adrv9025_Device_t*      device,
                                               adi_adrv9025_TxChannels_e   txChannel,
                                               adi_adrv9025_TxQecStatus_t* txQecStatus)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;

    uint8_t extData[3]  = {ADRV9025_CPU_OBJECTID_CAL_STATUS, ADRV9025_CPU_OBJECTID_TXQEC_TRACKING, 0};
    uint8_t armData[24] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txQecStatus);

    switch (txChannel)
    {
    case ADI_ADRV9025_TX1:
        extData[2] = 0; /*!< Tx0 channel enabled */
        break;
    case ADI_ADRV9025_TX2:
        extData[2] = 1; /*!< Tx1 channel enabled */
        break;
    case ADI_ADRV9025_TX3:
        extData[2] = 2; /*!< Tx2 channel enabled */
        break;
    case ADI_ADRV9025_TX4:
        extData[2] = 3; /*!< Tx3 channel enabled */
        break;
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid Tx Channel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Executing Tracking cals TxQec status get Arm Command */
    adi_adrv9025_CpuCmdWriteWait(
                                 device,
                                 ADI_ADRV9025_CPU_TYPE_C,
                                 ADRV9025_CPU_GET_OPCODE,
                                 &extData[0],
                                 sizeof(extData),
                                 ADI_ADRV9025_GETTXQECSTATUS_TIMEOUT_US,
                                 ADI_ADRV9025_GETTXQECSTATUS_INTERVAL_US,
                                 ADRV9025_CPU_OBJECTID_TXQEC_TRACKING);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Tx Qec config from ARM mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            sizeof(armData),
                            ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing TxQec Status into the structure. */
    txQecStatus->errorCode = (uint32_t)(((uint32_t)armData[0]) |
        ((uint32_t)armData[1] << 8) |
        ((uint32_t)armData[2] << 16) |
        ((uint32_t)armData[3] << 24));

    txQecStatus->percentComplete = (uint32_t)(((uint32_t)armData[4]) |
        ((uint32_t)armData[5] << 8) |
        ((uint32_t)armData[6] << 16) |
        ((uint32_t)armData[7] << 24));

    txQecStatus->correctionMetric = (uint32_t)(((uint32_t)armData[8]) |
        ((uint32_t)armData[9] << 8) |
        ((uint32_t)armData[10] << 16) |
        ((uint32_t)armData[11] << 24));

    txQecStatus->iterCount = (uint32_t)(((uint32_t)armData[12]) |
        ((uint32_t)armData[13] << 8) |
        ((uint32_t)armData[14] << 16) |
        ((uint32_t)armData[15] << 24));

    txQecStatus->updateCount = (uint32_t)(((uint32_t)armData[16]) |
        ((uint32_t)armData[17] << 8) |
        ((uint32_t)armData[18] << 16) |
        ((uint32_t)armData[19] << 24));

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TrackingCalRxQecStatusGet(adi_adrv9025_Device_t*      device,
                                               adi_adrv9025_RxChannels_e   rxChannel,
                                               adi_adrv9025_RxQecStatus_t* rxQecStatus)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;

    uint8_t extData[3]  = {ADRV9025_CPU_OBJECTID_CAL_STATUS, ADRV9025_CPU_OBJECTID_RXQEC_TRACKING, 0};
    uint8_t armData[20] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        rxQecStatus);

    switch (rxChannel)
    {
    case ADI_ADRV9025_RX1:
        extData[2] = 0; /*!< Rx0 channel enabled */
        break;
    case ADI_ADRV9025_RX2:
        extData[2] = 1; /*!< Rx1 channel enabled */
        break;
    case ADI_ADRV9025_RX3:
        extData[2] = 2; /*!< Rx2 channel enabled */
        break;
    case ADI_ADRV9025_RX4:
        extData[2] = 3; /*!< Rx3 channel enabled */
        break;
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx Channel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Executing Tracking cals RxQec status get Arm Command */
    adi_adrv9025_CpuCmdWriteWait(
                                 device,
                                 ADI_ADRV9025_CPU_TYPE_C,
                                 ADRV9025_CPU_GET_OPCODE,
                                 &extData[0],
                                 sizeof(extData),
                                 ADI_ADRV9025_GETRXQECSTATUS_TIMEOUT_US,
                                 ADI_ADRV9025_GETRXQECSTATUS_INTERVAL_US,
                                 ADRV9025_CPU_OBJECTID_RXQEC_TRACKING);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Rx QEC status config from ARM mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            sizeof(armData),
                            ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing Rx Qec Status into the structure. */
    rxQecStatus->errorCode = (uint32_t)(((uint32_t)armData[0]) |
        ((uint32_t)armData[1] << 8) |
        ((uint32_t)armData[2] << 16) |
        ((uint32_t)armData[3] << 24));

    rxQecStatus->percentComplete = (uint32_t)(((uint32_t)armData[4]) |
        ((uint32_t)armData[5] << 8) |
        ((uint32_t)armData[6] << 16) |
        ((uint32_t)armData[7] << 24));

    rxQecStatus->selfcheckIrrDb = (uint32_t)(((uint32_t)armData[8]) |
        ((uint32_t)armData[9] << 8) |
        ((uint32_t)armData[10] << 16) |
        ((uint32_t)armData[11] << 24));

    rxQecStatus->iterCount = (uint32_t)(((uint32_t)armData[12]) |
        ((uint32_t)armData[13] << 8) |
        ((uint32_t)armData[14] << 16) |
        ((uint32_t)armData[15] << 24));

    rxQecStatus->updateCount = (uint32_t)(((uint32_t)armData[16]) |
        ((uint32_t)armData[17] << 8) |
        ((uint32_t)armData[18] << 16) |
        ((uint32_t)armData[19] << 24));

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TrackingCalOrxQecStatusGet(adi_adrv9025_Device_t*       device,
                                                adi_adrv9025_RxChannels_e    orxChannel,
                                                adi_adrv9025_OrxQecStatus_t* orxQecStatus)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;

    uint8_t extData[3]  = {ADRV9025_CPU_OBJECTID_CAL_STATUS, ADRV9025_CPU_OBJECTID_ORXQEC_TRACKING, 0};
    uint8_t armData[20] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        orxQecStatus);

    switch (orxChannel)
    {
    case ADI_ADRV9025_ORX1:
        extData[2] = 0; /*!< ORx0 channel enabled */
        break;
    case ADI_ADRV9025_ORX2:
        extData[2] = 1; /*!< ORx1 channel enabled */
        break;
    case ADI_ADRV9025_ORX3:
        extData[2] = 2; /*!< ORx2 channel enabled */
        break;
    case ADI_ADRV9025_ORX4:
        extData[2] = 3; /*!< ORx3 channel enabled */
        break;
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         orxChannel,
                         "Invalid ORx Channel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Executing Tracking cals ORxQec status get Arm Command */
    adi_adrv9025_CpuCmdWriteWait(
                                 device,
                                 ADI_ADRV9025_CPU_TYPE_C,
                                 ADRV9025_CPU_GET_OPCODE,
                                 &extData[0],
                                 sizeof(extData),
                                 ADI_ADRV9025_GETORXQECSTATUS_TIMEOUT_US,
                                 ADI_ADRV9025_GETORXQECSTATUS_INTERVAL_US,
                                 ADRV9025_CPU_OBJECTID_ORXQEC_TRACKING);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Orx QEC status config from ARM mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            sizeof(armData),
                            ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing Orx Qec Status into the structure. */
    orxQecStatus->errorCode = (uint32_t)(((uint32_t)armData[0]) |
        ((uint32_t)armData[1] << 8) |
        ((uint32_t)armData[2] << 16) |
        ((uint32_t)armData[3] << 24));

    orxQecStatus->percentComplete = (uint32_t)(((uint32_t)armData[4]) |
        ((uint32_t)armData[5] << 8) |
        ((uint32_t)armData[6] << 16) |
        ((uint32_t)armData[7] << 24));

    orxQecStatus->selfcheckIrrDb = (uint32_t)(((uint32_t)armData[8]) |
        ((uint32_t)armData[9] << 8) |
        ((uint32_t)armData[10] << 16) |
        ((uint32_t)armData[11] << 24));

    orxQecStatus->iterCount = (uint32_t)(((uint32_t)armData[12]) |
        ((uint32_t)armData[13] << 8) |
        ((uint32_t)armData[14] << 16) |
        ((uint32_t)armData[15] << 24));

    orxQecStatus->updateCount = (uint32_t)(((uint32_t)armData[16]) |
        ((uint32_t)armData[17] << 8) |
        ((uint32_t)armData[18] << 16) |
        ((uint32_t)armData[19] << 24));

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TrackingCalClgcStatusGet(adi_adrv9025_Device_t*                device,
                                              adi_adrv9025_TxChannels_e             txChannel,
                                              adi_adrv9025_ClgcTrackingCalStatus_t* clgcStatus)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;

    uint8_t extData[3]  = {ADRV9025_CPU_OBJECTID_CAL_STATUS, ADRV9025_CPU_OBJECTID_CLGC_TRACKING, 0};
    uint8_t armData[20] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        clgcStatus);

    switch (txChannel)
    {
    case ADI_ADRV9025_TX1:
        extData[2] = 0; /*!< Tx0 channel enabled */
        break;
    case ADI_ADRV9025_TX2:
        extData[2] = 1; /*!< Tx1 channel enabled */
        break;
    case ADI_ADRV9025_TX3:
        extData[2] = 2; /*!< Tx2 channel enabled */
        break;
    case ADI_ADRV9025_TX4:
        extData[2] = 3; /*!< Tx3 channel enabled */
        break;
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid Tx Channel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Executing Tracking cals Tx LOL status get Arm Command */
    adi_adrv9025_CpuCmdWriteWait(
                                 device,
                                 ADI_ADRV9025_CPU_TYPE_C,
                                 ADRV9025_CPU_GET_OPCODE,
                                 &extData[0],
                                 sizeof(extData),
                                 ADI_ADRV9025_GETTXCLGCSTATUS_TIMEOUT_US,
                                 ADI_ADRV9025_GETTXCLGCSTATUS_INTERVAL_US,
                                 ADRV9025_CPU_OBJECTID_CLGC_TRACKING);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Tx LOL config from ARM mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            sizeof(armData),
                            ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing CLGC Status into the structure. */
    clgcStatus->errorCode = (adi_adrv9025_ClgcTrackingCalError_e)(((uint32_t)armData[0]) |
        ((uint32_t)armData[1] << 8) |
        ((uint32_t)armData[2] << 16) |
        ((uint32_t)armData[3] << 24));

    clgcStatus->percentComplete = (uint32_t)(((uint32_t)armData[4]) |
        ((uint32_t)armData[5] << 8) |
        ((uint32_t)armData[6] << 16) |
        ((uint32_t)armData[7] << 24));

    clgcStatus->varianceMetric = (uint32_t)(((uint32_t)armData[8]) |
        ((uint32_t)armData[9] << 8) |
        ((uint32_t)armData[10] << 16) |
        ((uint32_t)armData[11] << 24));

    clgcStatus->iterCount = (uint32_t)(((uint32_t)armData[12]) |
        ((uint32_t)armData[13] << 8) |
        ((uint32_t)armData[14] << 16) |
        ((uint32_t)armData[15] << 24));

    clgcStatus->updateCount = (uint32_t)(((uint32_t)armData[16]) |
        ((uint32_t)armData[17] << 8) |
        ((uint32_t)armData[18] << 16) |
        ((uint32_t)armData[19] << 24));

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TrackingCalRxHd2StatusGet(adi_adrv9025_Device_t*      device,
                                               adi_adrv9025_RxChannels_e   rxChannel,
                                               adi_adrv9025_RxHd2Status_t* rxHd2Status)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;

    uint8_t extData[3]  = {ADRV9025_CPU_OBJECTID_CAL_STATUS, ADRV9025_CPU_OBJECTID_RXHD2_TRACKING, 0};
    uint8_t armData[20] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        rxHd2Status);

    switch (rxChannel)
    {
    case ADI_ADRV9025_RX1:
        extData[2] = 0; /*!< Rx0 channel enabled */
        break;
    case ADI_ADRV9025_RX2:
        extData[2] = 1; /*!< Rx1 channel enabled */
        break;
    case ADI_ADRV9025_RX3:
        extData[2] = 2; /*!< Rx2 channel enabled */
        break;
    case ADI_ADRV9025_RX4:
        extData[2] = 3; /*!< Rx3 channel enabled */
        break;
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx Channel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Executing Tracking cals RxHd2 status get Arm Command */
    adi_adrv9025_CpuCmdWriteWait(
                                 device,
                                 ADI_ADRV9025_CPU_TYPE_C,
                                 ADRV9025_CPU_GET_OPCODE,
                                 &extData[0],
                                 3,
                                 ADI_ADRV9025_GETRXHD2STATUS_TIMEOUT_US,
                                 ADI_ADRV9025_GETRXHD2STATUS_INTERVAL_US,
                                 ADRV9025_CPU_OBJECTID_RXHD2_TRACKING);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Rx Hd2 status config from ARM mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            sizeof(armData),
                            ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing Rx Hd2 Status into the structure. */
    rxHd2Status->errorCode = (uint32_t)(((uint32_t)armData[0]) |
        ((uint32_t)armData[1] << 8) |
        ((uint32_t)armData[2] << 16) |
        ((uint32_t)armData[3] << 24));

    rxHd2Status->percentComplete = (uint32_t)(((uint32_t)armData[4]) |
        ((uint32_t)armData[5] << 8) |
        ((uint32_t)armData[6] << 16) |
        ((uint32_t)armData[7] << 24));

    rxHd2Status->confidenceLevel = (uint32_t)(((uint32_t)armData[8]) |
        ((uint32_t)armData[9] << 8) |
        ((uint32_t)armData[10] << 16) |
        ((uint32_t)armData[11] << 24));

    rxHd2Status->iterCount = (uint32_t)(((uint32_t)armData[12]) |
        ((uint32_t)armData[13] << 8) |
        ((uint32_t)armData[14] << 16) |
        ((uint32_t)armData[15] << 24));

    rxHd2Status->updateCount = (uint32_t)(((uint32_t)armData[16]) |
        ((uint32_t)armData[17] << 8) |
        ((uint32_t)armData[18] << 16) |
        ((uint32_t)armData[19] << 24));

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TrackingCalRxHd2CfgSet(adi_adrv9025_Device_t*      device,
                                            adi_adrv9025_RxHd2Config_t* hd2CalConfig)
{
    uint8_t  armData[6] = {0};
    uint32_t fwStatus   = 0;
    uint8_t  status     = 0;
    uint16_t byteOffset = 0;
    uint8_t  byteCount  = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, hd2CalConfig);

    /* read firmware state to make sure ARM is in ready/IDLE mode */
    ADRV9025_SPIREADBYTE("ARM_CMD_STATUS_8",
                         ADRV9025_CPU_C_ADDR_CMD_STATUS_8,
                         &status);
    ADI_ERROR_RETURN(device->common.error.newAction);

    fwStatus = (uint32_t)status;

    /* Throw error if not in READY state */
    if (fwStatus != ADRV9025_CPU_FW_STATUS_READY)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV9025_SRC_CPUCMD,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_ADRV9025_ACT_ERR_RESET_CPU,
                         device,
                         "Device not in READY state. Error in adi_adrv9025_TrackingCalRxHd2CfgSet()\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Range check to see if only sideband update needed */
    if ((hd2CalConfig->control == 0) && (hd2CalConfig->correctStart == 0) && (hd2CalConfig->correctStop == 0))
    {
        armData[0] = (uint8_t)((hd2CalConfig->posSideBandSel > 0) ? 1 : 0);
        byteCount  = 1;
    }

        /* range check start stop freq range */
    else if ((hd2CalConfig->correctStart >= hd2CalConfig->correctStop))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         hd2CalConfig->correctStart,
                         "Invalid correction start/stop frequency range specified");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    else
    {
        armData[0] = (uint8_t)((hd2CalConfig->posSideBandSel > 0) ? 1 : 0);
        armData[1] = (uint8_t)hd2CalConfig->control;
        armData[2] = (uint8_t)hd2CalConfig->correctStart;
        armData[3] = (uint8_t)(hd2CalConfig->correctStart >> 8);
        armData[4] = (uint8_t)hd2CalConfig->correctStop;
        armData[5] = (uint8_t)(hd2CalConfig->correctStop >> 8);
        byteCount  = 6;
    }

    adi_adrv9025_CpuConfigWrite(device,
                                ADI_ADRV9025_CPU_TYPE_C,
                                ADRV9025_CPU_OBJECTID_RXHD2_TRACKING,
                                byteOffset,
                                &armData[0],
                                byteCount);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TrackingCalRxHd2CfgGet(adi_adrv9025_Device_t*      device,
                                            adi_adrv9025_RxHd2Config_t* hd2CalConfig)
{
    uint8_t  armData[6] = {0};
    uint32_t fwStatus   = 0;
    uint8_t  status     = 0;
    uint16_t byteOffset = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, hd2CalConfig);

    /* read firmware state to make sure ARM is in ready/IDLE mode */
    ADRV9025_SPIREADBYTE("ARM_CMD_STATUS_8",
                         ADRV9025_CPU_C_ADDR_CMD_STATUS_8,
                         &status);
    ADI_ERROR_RETURN(device->common.error.newAction);

    fwStatus = (uint32_t)status;

    /* Throw error if not in READY state */
    if (fwStatus != ADRV9025_CPU_FW_STATUS_READY)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV9025_SRC_CPUCMD,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_ADRV9025_ACT_ERR_RESET_CPU,
                         device,
                         "Device not in READY state. Error in adi_adrv9025_TrackingCalsEnableSet()\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    adi_adrv9025_CpuConfigRead(device,
                               ADI_ADRV9025_CPU_TYPE_C,
                               ADRV9025_CPU_OBJECTID_RXHD2_TRACKING,
                               byteOffset,
                               &armData[0],
                               6);
    ADI_ERROR_RETURN(device->common.error.newAction);

    hd2CalConfig->posSideBandSel = armData[0];
    hd2CalConfig->control        = armData[1];
    hd2CalConfig->correctStart   = (uint32_t)(((uint32_t)armData[2]) |
        ((uint32_t)armData[3] << 8));
    hd2CalConfig->correctStop = (uint32_t)(((uint32_t)armData[4]) |
        ((uint32_t)armData[5] << 8));

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TrackingCalAllStateGet(adi_adrv9025_Device_t*           device,
                                            adi_adrv9025_TrackingCalState_t* CalState)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;

    uint8_t extData[1]  = {ADRV9025_CPU_OBJECTID_TRACKING_CALS_STATE};
    uint8_t armData[41] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, CalState);

    /* Executing TRACKING_CALS_STATE Arm Command */
    adi_adrv9025_CpuCmdWriteWait(device,
                                 ADI_ADRV9025_CPU_TYPE_C,
                                 ADRV9025_CPU_GET_OPCODE,
                                 &extData[0],
                                 sizeof(extData),
                                 ADI_ADRV9025_GETTCALS_STATE_TIMEOUT_US,
                                 ADI_ADRV9025_GETTCALS_STATE_INTERVAL_US,
                                 ADRV9025_CPU_OBJECTID_TRACKING_CALS_STATE);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Test tone config from ARM mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            sizeof(armData),
                            ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserialize ARM Data into TrackingCalState Structure */

    /* Deserializing calibration Error (first  4 bytes) */
    CalState->calError = (uint32_t)(((uint32_t)armData[0]) |
        ((uint32_t)armData[1] << 8) |
        ((uint32_t)armData[2] << 16) |
        ((uint32_t)armData[3] << 24));

    /* Deserializing Rx Qec state for channel 1/2/3/4 */
    CalState->rx1Qec = armData[8];
    CalState->rx2Qec = armData[9];
    CalState->rx3Qec = armData[10];
    CalState->rx4Qec = armData[11];

    /* Deserializing Orx Qec state for channel 1/2/3/4 */
    CalState->orx1Qec = armData[12];
    CalState->orx2Qec = armData[13];
    CalState->orx3Qec = armData[14];
    CalState->orx4Qec = armData[15];

    /* Deserializing Tx LOL state for channel 1/2/3/4 */
    CalState->tx1Lol = armData[16];
    CalState->tx2Lol = armData[17];
    CalState->tx3Lol = armData[18];
    CalState->tx4Lol = armData[19];

    /* Deserializing Tx Qec state for channel 1/2/3/4 */
    CalState->tx1Qec = armData[20];
    CalState->tx2Qec = armData[21];
    CalState->tx3Qec = armData[22];
    CalState->tx4Qec = armData[23];

    /* Deserializing Tx DPD state for channel 1/2/3/4 */
    CalState->tx1Dpd = armData[24];
    CalState->tx2Dpd = armData[25];
    CalState->tx3Dpd = armData[26];
    CalState->tx4Dpd = armData[27];

    /* Deserializing Tx CLGC state for channel 1/2/3/4 */
    CalState->tx1Clgc = armData[28];
    CalState->tx2Clgc = armData[29];
    CalState->tx3Clgc = armData[30];
    CalState->tx4Clgc = armData[31];

    /* Deserializing VSWR state for channel 1/2/3/4 */
    CalState->tx1Vswr = armData[32];
    CalState->tx2Vswr = armData[33];
    CalState->tx3Vswr = armData[34];
    CalState->tx4Vswr = armData[35];

    /* Deserializing Rx Hd2 state for channel 1/2/3/4 */
    CalState->rx1Hd2 = armData[36];
    CalState->rx2Hd2 = armData[37];
    CalState->rx3Hd2 = armData[38];
    CalState->rx4Hd2 = armData[39];

    /* Deserializer Deserializer tracking cal state */
    CalState->deserializer = armData[40];

    return device->common.error.newAction;
}

int32_t adi_adrv9025_ExternalPathDelaySet(adi_adrv9025_Device_t*            device,
                                          adi_adrv9025_TxChannels_e         txChannel,
                                          adi_adrv9025_ExternalPathDelay_t* externalPathDelay)
{
    static const uint8_t DPD_PATH_DELAY_CTRL_PARAM = 0x10;
    static const uint8_t MAX_INTERPOLATION_INDEX   = 15;

    uint8_t extData[4] = {ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_DPD_TRACKING, (uint8_t)txChannel, DPD_PATH_DELAY_CTRL_PARAM};
    uint8_t armData[2] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, externalPathDelay);

    armData[0] = externalPathDelay->fifoDelay;
    armData[1] = externalPathDelay->interpolationIndex;

    /* Throw an error if the requested Tx channel is invalid */
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
                         "Invalid target Tx Channel requested to set external path delay");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Throw an error if the interpolation index is greater than 15 */
    if (externalPathDelay->interpolationIndex > MAX_INTERPOLATION_INDEX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         externalPathDelay,
                         "Invalid interpolation index encountered while attempting to configure external path delay");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Write the external path delay config to ARM mailbox */
    adi_adrv9025_CpuMemWrite(device,
                             (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                             &armData[0],
                             sizeof(armData),
                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Executing external path delay set Arm Command */
    adi_adrv9025_CpuCmdWriteWait(device,
                                 ADI_ADRV9025_CPU_TYPE_C,
                                 ADRV9025_CPU_SET_OPCODE,
                                 &extData[0],
                                 sizeof(extData),
                                 ADI_ADRV9025_SETEXTPATHDELAY_TIMEOUT_US,
                                 ADI_ADRV9025_SETEXTPATHDELAY_INTERVAL_US,
                                 ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExternalPathDelayGet(adi_adrv9025_Device_t*            device,
                                          adi_adrv9025_TxChannels_e         txChannel,
                                          adi_adrv9025_ExternalPathDelay_t* externalPathDelay)
{
    static const uint8_t DPD_PATH_DELAY_CTRL_PARAM = 0x00;
    static const uint8_t ARM_MEM_READ_AUTOINCR     = 1;

    uint8_t extData[4]  = {ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_DPD_TRACKING, (uint8_t)txChannel, DPD_PATH_DELAY_CTRL_PARAM};
    uint8_t armData[20] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, externalPathDelay);

    /* Throw an error if the requested Tx channel is invalid */
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
                         "Invalid Tx Channel requested to retrieve external path delay");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Executing external path delay get Arm Command */
    adi_adrv9025_CpuCmdWriteWait(device,
                                 ADI_ADRV9025_CPU_TYPE_C,
                                 ADRV9025_CPU_SET_OPCODE,
                                 &extData[0],
                                 sizeof(extData),
                                 ADI_ADRV9025_SETEXTPATHDELAY_TIMEOUT_US,
                                 ADI_ADRV9025_SETEXTPATHDELAY_INTERVAL_US,
                                 ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL);
    ADI_ERROR_RETURN(device->common.error.newAction);

    ADI_ERROR_RETURN(device->common.error.newAction);

    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                            &armData[0],
                            sizeof(armData),
                            ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Update the external path delay struct with the value read from the device */
    externalPathDelay->fifoDelay          = armData[0];
    externalPathDelay->interpolationIndex = armData[1];

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DigDcOffsetMShiftSet(adi_adrv9025_Device_t*  device,
                                          adi_adrv9025_DcOffSet_t dcOffSetMShift)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    static const uint8_t MSHIFT_MAX_RANGE = 31;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* range check mShift value */
    if (dcOffSetMShift.mShift > MSHIFT_MAX_RANGE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dcOffSetMShift.mShift,
                         "mShift value out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* check channel and set the address */
    switch (dcOffSetMShift.dcChannelSelect)
    {
    case ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH0:
        recoveryAction = adrv9025_RxBbdcMShiftBfSet(device,
                                                    ADRV9025_BF_RX_CH0,
                                                    dcOffSetMShift.mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH1:
        recoveryAction = adrv9025_RxBbdcMShiftBfSet(device,
                                                    ADRV9025_BF_RX_CH1,
                                                    dcOffSetMShift.mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH2:
        recoveryAction = adrv9025_RxBbdcMShiftBfSet(device,
                                                    ADRV9025_BF_RX_CH2,
                                                    dcOffSetMShift.mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH3:
        recoveryAction = adrv9025_RxBbdcMShiftBfSet(device,
                                                    ADRV9025_BF_RX_CH3,
                                                    dcOffSetMShift.mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX2_CH0:
        recoveryAction = adrv9025_OrxBbdcOrx2MShiftBfSet(device,
                                                         ADRV9025_BF_ORX_CH0,
                                                         dcOffSetMShift.mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX2_CH1:
        recoveryAction = adrv9025_OrxBbdcOrx2MShiftBfSet(device,
                                                         ADRV9025_BF_ORX_CH1,
                                                         dcOffSetMShift.mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX_CH0:
        recoveryAction = adrv9025_OrxBbdcMShiftBfSet(device,
                                                     ADRV9025_BF_ORX_CH0,
                                                     dcOffSetMShift.mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX_CH1:
        recoveryAction = adrv9025_OrxBbdcMShiftBfSet(device,
                                                     ADRV9025_BF_ORX_CH1,
                                                     dcOffSetMShift.mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK1_CH0:
        recoveryAction = adrv9025_OrxBbdcLoopbackMShiftBfSet(device,
                                                             ADRV9025_BF_ORX_CH0,
                                                             dcOffSetMShift.mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK1_CH1:
        recoveryAction = adrv9025_OrxBbdcLoopbackMShiftBfSet(device,
                                                             ADRV9025_BF_ORX_CH1,
                                                             dcOffSetMShift.mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dcOffSetMShift.dcChannelSelect,
                         "Invalid DcOffSet Channel Selected");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DigDcOffsetMShiftGet(adi_adrv9025_Device_t*   device,
                                          adi_adrv9025_DcOffSet_t* dcOffSetMShift)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        dcOffSetMShift);

    /* check channel and set the address */
    switch (dcOffSetMShift->dcChannelSelect)
    {
    case ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH0:
        recoveryAction = adrv9025_RxBbdcMShiftBfGet(device,
                                                    ADRV9025_BF_RX_CH0,
                                                    &dcOffSetMShift->mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH1:
        recoveryAction = adrv9025_RxBbdcMShiftBfGet(device,
                                                    ADRV9025_BF_RX_CH1,
                                                    &dcOffSetMShift->mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH2:
        recoveryAction = adrv9025_RxBbdcMShiftBfGet(device,
                                                    ADRV9025_BF_RX_CH2,
                                                    &dcOffSetMShift->mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH3:
        recoveryAction = adrv9025_RxBbdcMShiftBfGet(device,
                                                    ADRV9025_BF_RX_CH3,
                                                    &dcOffSetMShift->mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX2_CH0:
        recoveryAction = adrv9025_OrxBbdcOrx2MShiftBfGet(device,
                                                         ADRV9025_BF_ORX_CH0,
                                                         &dcOffSetMShift->mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX2_CH1:
        recoveryAction = adrv9025_OrxBbdcOrx2MShiftBfGet(device,
                                                         ADRV9025_BF_ORX_CH1,
                                                         &dcOffSetMShift->mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX_CH0:
        recoveryAction = adrv9025_OrxBbdcMShiftBfGet(device,
                                                     ADRV9025_BF_ORX_CH0,
                                                     &dcOffSetMShift->mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX_CH1:
        recoveryAction = adrv9025_OrxBbdcMShiftBfGet(device,
                                                     ADRV9025_BF_ORX_CH1,
                                                     &dcOffSetMShift->mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK1_CH0:
        recoveryAction = adrv9025_OrxBbdcLoopbackMShiftBfGet(device,
                                                             ADRV9025_BF_ORX_CH0,
                                                             &dcOffSetMShift->mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    case ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK1_CH1:
        recoveryAction = adrv9025_OrxBbdcLoopbackMShiftBfGet(device,
                                                             ADRV9025_BF_ORX_CH1,
                                                             &dcOffSetMShift->mShift);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;

    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dcOffSetMShift.dcChannelSelect,
                         "Invalid DcOffSet Channel Selected");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DigDcOffsetEnableSet(adi_adrv9025_Device_t* device,
                                          uint16_t               channelDisableEnableMask)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* RX Channel0 */
    if ((channelDisableEnableMask & (uint16_t)ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH0) > 0)
    {
        recoveryAction = adrv9025_RxBbdcTrackingEnableBfSet(device,
                                                            ADRV9025_BF_RX_CH0,
                                                            ADI_ENABLE);
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
        recoveryAction = adrv9025_RxBbdcTrackingEnableBfSet(device,
                                                            ADRV9025_BF_RX_CH0,
                                                            ADI_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* RX Channel1 */
    if ((channelDisableEnableMask & (uint16_t)ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH1) > 0)
    {
        recoveryAction = adrv9025_RxBbdcTrackingEnableBfSet(device,
                                                            ADRV9025_BF_RX_CH1,
                                                            ADI_ENABLE);
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
        recoveryAction = adrv9025_RxBbdcTrackingEnableBfSet(device,
                                                            ADRV9025_BF_RX_CH1,
                                                            ADI_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* RX Channel2 */
    if ((channelDisableEnableMask & (uint16_t)ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH2) > 0)
    {
        recoveryAction = adrv9025_RxBbdcTrackingEnableBfSet(device,
                                                            ADRV9025_BF_RX_CH2,
                                                            ADI_ENABLE);
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
        recoveryAction = adrv9025_RxBbdcTrackingEnableBfSet(device,
                                                            ADRV9025_BF_RX_CH2,
                                                            ADI_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* RX Channel3 */
    if ((channelDisableEnableMask & (uint16_t)ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH3) > 0)
    {
        recoveryAction = adrv9025_RxBbdcTrackingEnableBfSet(device,
                                                            ADRV9025_BF_RX_CH3,
                                                            ADI_ENABLE);
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
        recoveryAction = adrv9025_RxBbdcTrackingEnableBfSet(device,
                                                            ADRV9025_BF_RX_CH3,
                                                            ADI_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* ORX Channel0 */
    if ((channelDisableEnableMask & (uint16_t)ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX_CH0) > 0)
    {
        recoveryAction = adrv9025_OrxBbdcTrackingEnableBfSet(device,
                                                             ADRV9025_BF_ORX_CH0,
                                                             ADI_ENABLE);
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
        recoveryAction = adrv9025_OrxBbdcTrackingEnableBfSet(device,
                                                             ADRV9025_BF_ORX_CH0,
                                                             ADI_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* ORX Channel1 */
    if ((channelDisableEnableMask & (uint16_t)ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX_CH1) > 0)
    {
        recoveryAction = adrv9025_OrxBbdcTrackingEnableBfSet(device,
                                                             ADRV9025_BF_ORX_CH1,
                                                             ADI_ENABLE);
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
        recoveryAction = adrv9025_OrxBbdcTrackingEnableBfSet(device,
                                                             ADRV9025_BF_ORX_CH1,
                                                             ADI_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* ORX2 Channel0 */
    if ((channelDisableEnableMask & (uint16_t)ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX2_CH0) > 0)
    {
        recoveryAction = adrv9025_OrxBbdcOrx2TrackingEnableBfSet(device,
                                                                 ADRV9025_BF_ORX_CH0,
                                                                 ADI_ENABLE);
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
        recoveryAction = adrv9025_OrxBbdcOrx2TrackingEnableBfSet(device,
                                                                 ADRV9025_BF_ORX_CH0,
                                                                 ADI_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* ORX2 Channel1 */
    if ((channelDisableEnableMask & (uint16_t)ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX2_CH1) > 0)
    {
        recoveryAction = adrv9025_OrxBbdcOrx2TrackingEnableBfSet(device,
                                                                 ADRV9025_BF_ORX_CH1,
                                                                 ADI_ENABLE);
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
        recoveryAction = adrv9025_OrxBbdcOrx2TrackingEnableBfSet(device,
                                                                 ADRV9025_BF_ORX_CH1,
                                                                 ADI_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* ORXLoopback1 Channel0 */
    if ((channelDisableEnableMask & (uint16_t)ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK1_CH0) > 0)
    {
        recoveryAction = adrv9025_OrxBbdcLoopback1TrackingEnableBfSet(device,
                                                                      ADRV9025_BF_ORX_CH0,
                                                                      ADI_ENABLE);
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
        recoveryAction = adrv9025_OrxBbdcLoopback1TrackingEnableBfSet(device,
                                                                      ADRV9025_BF_ORX_CH0,
                                                                      ADI_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* ORXLoopback1 Channel1 */
    if ((channelDisableEnableMask & (uint16_t)ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK1_CH1) > 0)
    {
        recoveryAction = adrv9025_OrxBbdcLoopback1TrackingEnableBfSet(device,
                                                                      ADRV9025_BF_ORX_CH1,
                                                                      ADI_ENABLE);
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
        recoveryAction = adrv9025_OrxBbdcLoopback1TrackingEnableBfSet(device,
                                                                      ADRV9025_BF_ORX_CH1,
                                                                      ADI_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* ORXLoopback2 Channel0 */
    if ((channelDisableEnableMask & (uint16_t)ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK2_CH0) > 0)
    {
        recoveryAction = adrv9025_OrxBbdcLoopback2TrackingEnableBfSet(device,
                                                                      ADRV9025_BF_ORX_CH0,
                                                                      ADI_ENABLE);
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
        recoveryAction = adrv9025_OrxBbdcLoopback2TrackingEnableBfSet(device,
                                                                      ADRV9025_BF_ORX_CH0,
                                                                      ADI_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* ORXLoopback2 Channel1 */
    if ((channelDisableEnableMask & (uint16_t)ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK2_CH1) > 0)
    {
        recoveryAction = adrv9025_OrxBbdcLoopback2TrackingEnableBfSet(device,
                                                                      ADRV9025_BF_ORX_CH1,
                                                                      ADI_ENABLE);
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
        recoveryAction = adrv9025_OrxBbdcLoopback2TrackingEnableBfSet(device,
                                                                      ADRV9025_BF_ORX_CH1,
                                                                      ADI_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DigDcOffsetEnableGet(adi_adrv9025_Device_t* device,
                                          uint16_t*              channelDisableEnableMask)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t enableStatus   = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        channelDisableEnableMask);

    *channelDisableEnableMask = 0;

    /* ORXLoopback2 Channel1 */
    recoveryAction = adrv9025_OrxBbdcLoopback2TrackingEnableBfGet(device,
                                                                  ADRV9025_BF_ORX_CH1,
                                                                  &enableStatus);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *channelDisableEnableMask = (*channelDisableEnableMask | (uint16_t)enableStatus) << 1;

    /* ORXLoopback2 Channel0 */
    recoveryAction = adrv9025_OrxBbdcLoopback2TrackingEnableBfGet(device,
                                                                  ADRV9025_BF_ORX_CH0,
                                                                  &enableStatus);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *channelDisableEnableMask = (*channelDisableEnableMask | (uint16_t)enableStatus) << 1;

    /* ORXLoopback1 Channel1 */
    recoveryAction = adrv9025_OrxBbdcLoopback1TrackingEnableBfGet(device,
                                                                  ADRV9025_BF_ORX_CH1,
                                                                  &enableStatus);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *channelDisableEnableMask = (*channelDisableEnableMask | (uint16_t)enableStatus) << 1;

    /* ORXLoopback1 Channel0 */
    recoveryAction = adrv9025_OrxBbdcLoopback1TrackingEnableBfGet(device,
                                                                  ADRV9025_BF_ORX_CH0,
                                                                  &enableStatus);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *channelDisableEnableMask = (*channelDisableEnableMask | (uint16_t)enableStatus) << 1;

    /* ORX2 Channel1 */
    recoveryAction = adrv9025_OrxBbdcOrx2TrackingEnableBfGet(device,
                                                             ADRV9025_BF_ORX_CH1,
                                                             &enableStatus);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *channelDisableEnableMask = (*channelDisableEnableMask | (uint16_t)enableStatus) << 1;

    /* ORX2 Channel0 */
    recoveryAction = adrv9025_OrxBbdcOrx2TrackingEnableBfGet(device,
                                                             ADRV9025_BF_ORX_CH0,
                                                             &enableStatus);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *channelDisableEnableMask = (*channelDisableEnableMask | (uint16_t)enableStatus) << 1;

    /* ORX Channel1 */
    recoveryAction = adrv9025_OrxBbdcTrackingEnableBfGet(device,
                                                         ADRV9025_BF_ORX_CH1,
                                                         &enableStatus);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *channelDisableEnableMask = (*channelDisableEnableMask | (uint16_t)enableStatus) << 1;

    /* ORX Channel0 */
    recoveryAction = adrv9025_OrxBbdcTrackingEnableBfGet(device,
                                                         ADRV9025_BF_ORX_CH0,
                                                         &enableStatus);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *channelDisableEnableMask = (*channelDisableEnableMask | (uint16_t)enableStatus) << 1;

    /* RX Channel3 */
    recoveryAction = adrv9025_RxBbdcTrackingEnableBfGet(device,
                                                        ADRV9025_BF_RX_CH3,
                                                        &enableStatus);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *channelDisableEnableMask = (*channelDisableEnableMask | (uint16_t)enableStatus) << 1;

    /* RX Channel2 */
    recoveryAction = adrv9025_RxBbdcTrackingEnableBfGet(device,
                                                        ADRV9025_BF_RX_CH2,
                                                        &enableStatus);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *channelDisableEnableMask = (*channelDisableEnableMask | (uint16_t)enableStatus) << 1;

    /* RX Channel1 */
    recoveryAction = adrv9025_RxBbdcTrackingEnableBfGet(device,
                                                        ADRV9025_BF_RX_CH1,
                                                        &enableStatus);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *channelDisableEnableMask = (*channelDisableEnableMask | (uint16_t)enableStatus) << 1;

    /* RX Channel0 */
    recoveryAction = adrv9025_RxBbdcTrackingEnableBfGet(device,
                                                        ADRV9025_BF_RX_CH0,
                                                        &enableStatus);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    *channelDisableEnableMask = (*channelDisableEnableMask | (uint16_t)enableStatus);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtTxLolChannelReset(adi_adrv9025_Device_t*               device,
                                          adi_adrv9025_ExtTxLolChannelReset_t* txLolReset)
{
    uint32_t i          = 0;
    uint8_t  extData[4] = {ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_TXLOLEXT_INIT, 0, txLolReset->resetType};
    uint8_t  txChannel  = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, txLolReset);

    /* Range check */
    if ((txLolReset->channelSelect & ADI_ADRV9025_TXALL) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dcOffSetMShift.dcChannelSelect,
                         "Invalid TX Channel Selected");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < ADRV9025_NUMBER_OF_TX_CHANNELS; i++)
    {
        txChannel = (1 << i);
        if (txLolReset->channelSelect & txChannel)
        {
            extData[2] = txChannel;
        }
        else
        {
            continue;
        }

        /* sending write config command */
        adi_adrv9025_CpuCmdWriteWait(device,
                                     ADI_ADRV9025_CPU_TYPE_C,
                                     ADRV9025_CPU_SET_OPCODE,
                                     &extData[0],
                                     4,
                                     ADI_ADRV9025_ENTRACKINGCALS_TIMEOUT_US,
                                     ADI_ADRV9025_ENTRACKINGCALS_INTERVAL_US,
                                     ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DualBandSwitchingTimeGet(adi_adrv9025_Device_t* device,
                                              uint32_t*              milliseconds)
{
    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t  data[4]        = {0};
    uint16_t byteOffset     = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        milliseconds);

    recoveryAction = adi_adrv9025_CpuConfigRead(device,
                                                ADI_ADRV9025_CPU_TYPE_C,
                                                ADRV9025_CPU_OBJECTID_DUALBANDCFG,
                                                byteOffset,
                                                &data[0],
                                                sizeof(data));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to read Dual Band switching time command");
    ADI_ERROR_RETURN(device->common.error.newAction);

    *milliseconds = (uint32_t)(data[0] | data[1] << 8 | data[2] << 16 | data[3] << 24);
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DualBandSwitchingTimeSet(adi_adrv9025_Device_t* device,
                                              uint32_t               milliseconds)
{
    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t  armData[4]     = {0};
    uint16_t byteOffset     = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Range check */
    if ((milliseconds < ADI_ADRV9025_SWITCH_TIME_MIN) || (milliseconds > ADI_ADRV9025_SWITCH_TIME_MAX))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         milliseconds,
                         "Invalid Parameter, Milliseconds out of expected range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    armData[0] = (uint8_t)((milliseconds >> 0) & 0xff);
    armData[1] = (uint8_t)((milliseconds >> 8) & 0xff);
    armData[2] = (uint8_t)((milliseconds >> 16) & 0xff);
    armData[3] = (uint8_t)((milliseconds >> 24) & 0xff);

    recoveryAction = adi_adrv9025_CpuConfigWrite(device,
                                                 ADI_ADRV9025_CPU_TYPE_C,
                                                 ADRV9025_CPU_OBJECTID_DUALBANDCFG,
                                                 byteOffset,
                                                 &armData[0],
                                                 sizeof(armData));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "CpuConfigWrite failed writing CPU memory");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}


int32_t adi_adrv9025_ClgcExternalPathDelaySet(adi_adrv9025_Device_t*            device,
                                              adi_adrv9025_TxChannels_e         txChannel,
                                              adi_adrv9025_ExternalPathDelay_t* externalPathDelay)
{
    static const uint8_t PATH_DELAY_CTRL_PARAM   = 0x10;
    static const uint8_t MAX_INTERPOLATION_INDEX = 15;

    uint8_t extData[4] = { ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_CLGC_TRACKING, (uint8_t)txChannel, PATH_DELAY_CTRL_PARAM };
    uint8_t armData[2] = { 0 };

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, externalPathDelay);

    armData[0] = externalPathDelay->fifoDelay;
    armData[1] = externalPathDelay->interpolationIndex;

    /* Throw an error if the requested Tx channel is invalid */
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
                         "Invalid target Tx Channel requested to set CLGC external path delay");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Throw an error if the interpolation index is greater than 15 */
    if (externalPathDelay->interpolationIndex > MAX_INTERPOLATION_INDEX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         externalPathDelay,
                         "Invalid interpolation index encountered while attempting to configure CLGC external path delay");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Write the external path delay config to ARM mailbox */
    adi_adrv9025_CpuMemWrite(device,
                             (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                             &armData[0],
                             sizeof(armData),
                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Executing external path delay set Arm Command */
    adi_adrv9025_CpuCmdWriteWait(device,
                                 ADI_ADRV9025_CPU_TYPE_C,
                                 ADRV9025_CPU_SET_OPCODE,
                                 &extData[0],
                                 sizeof(extData),
                                 ADI_ADRV9025_SETEXTPATHDELAY_TIMEOUT_US,
                                 ADI_ADRV9025_SETEXTPATHDELAY_INTERVAL_US,
                                 ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ClgcExternalPathDelayGet(adi_adrv9025_Device_t*            device,
                                              adi_adrv9025_TxChannels_e         txChannel,
                                              adi_adrv9025_ExternalPathDelay_t* externalPathDelay)
{
    static const uint8_t PATH_DELAY_CTRL_PARAM = 0x00;
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;

    uint8_t extData[4] = { ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_CLGC_TRACKING, (uint8_t)txChannel, PATH_DELAY_CTRL_PARAM };
    uint8_t armData[20] = { 0 };

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        externalPathDelay);

    /* Throw an error if the requested Tx channel is invalid */
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
                         "Invalid Tx Channel requested to retrieve CLGC external path delay");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Executing external path delay get Arm Command */
    adi_adrv9025_CpuCmdWriteWait(device,
                                 ADI_ADRV9025_CPU_TYPE_C,
                                 ADRV9025_CPU_SET_OPCODE,
                                 &extData[0],
                                 sizeof(extData),
                                 ADI_ADRV9025_SETEXTPATHDELAY_TIMEOUT_US,
                                 ADI_ADRV9025_SETEXTPATHDELAY_INTERVAL_US,
                                 ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                            &armData[0],
                            sizeof(armData),
                            ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Update the external path delay struct with the value read from the device */
    externalPathDelay->fifoDelay          = armData[0];
    externalPathDelay->interpolationIndex = armData[1];

    return (device->common.error.newAction);
}
