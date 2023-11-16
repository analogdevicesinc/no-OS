/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adrv904x_dfe_cpu.h
 * \brief Contains ADRV904X DFE CPU related private function prototypes for
 *        adrv904x_dfe_cpu.c.
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef _ADRV904X_DFE_CPU_H_
#define _ADRV904X_DFE_CPU_H_

#include "adi_adrv904x_error.h"
#include "adi_adrv904x_dfe_cpu_types.h"
#include "adi_adrv904x_dfe_framework_tracking_cal_t.h"
#include "adi_adrv904x_dfe_vswr.h"
#include "adi_adrv904x_dfe_cpu_pintsw_status_types.h"
#include "adi_adrv904x_cpu_sys_types.h"
#include "../../private/bf/adrv904x_bf_core.h"
#include "../../private/include/adrv904x_cpu_scratch_registers.h"
#include "../../private/include/adrv904x_cpu_types.h"
#include "../../private/include/adrv904x_cpu_cmd_intf.h"
#include "../../private/include/adrv904x_dfe_memory.h"
#include "../../private/include/adrv904x_dfe_svc_cmd_force_exception.h"
#include "../../private/include/adrv904x_dfe_framework_cmd_sys_status_t.h"
#include "../../private/include/adrv904x_cpu_object_ids_types.h"
#include "../../private/include/adrv904x_dfe_svc_cmd_t.h"
#include "adi_adrv904x_dfe_svc_log_trace_events_t.h"

/*!< Get the address of a specific member of the DFE init structure */
#define ADRV904X_DFE_CPU_INIT_ADDR_GET( entry ) ( ADRV904X_DFE_CODE_MEM_REGION_END - \
                                                  sizeof(adrv904x_DfeMemInitTemp_t) + \
                                                  ADI_LIBRARY_OFFSETOF(adrv904x_DfeMemInitTemp_t, entry ) )


#define ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_RETURN(cmdStatusErrorCode, cmdStatus, cpuErrorCode, recoveryAction)     \
                                                                                                                    \
if (cmdStatus == ADRV904X_CPU_CMD_STATUS_CMD_FAILED)                                                                \
{                                                                                                                   \
    cpuErrorCode = ADRV904X_CTOHL(cmdStatusErrorCode);                                                              \
    ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_DFE_SVC_RUNTIME,                                        \
                                        cpuErrorCode,                                                               \
                                        cmdStatus,                                                                  \
                                        recoveryAction);                                                            \
    return recoveryAction;                                                                                          \
}                                                                                                                   \
else                                                                                                                \
{                                                                                                                   \
    ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_DFE_CPU,                                                \
                                        ADI_ADRV904X_ERRCODE_DFE_CPU_CMD_RESPONSE,                                  \
                                        cmdStatus,                                                                  \
                                        recoveryAction);                                                            \
    return recoveryAction;                                                                                          \
}                                                                                                                   \

#define ADI_ADRV904X_DFE_CPU_APP_RESP_CHECK_RETURN(cmdStatusErrorCode, cmdStatus, cpuErrorCode, recoveryAction)     \
                                                                                                                    \
if (cmdStatus == ADRV904X_CPU_CMD_STATUS_CMD_FAILED)                                                                \
{                                                                                                                   \
    cpuErrorCode = ADRV904X_CTOHL(cmdStatusErrorCode);                                                              \
    ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_DFE_APP_RUNTIME,                                        \
                                        cpuErrorCode,                                                               \
                                        cmdStatus,                                                                  \
                                        recoveryAction);                                                            \
    return recoveryAction;                                                                                          \
}                                                                                                                   \
else                                                                                                                \
{                                                                                                                   \
    ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_DFE_CPU,                                                \
                                        ADI_ADRV904X_ERRCODE_DFE_CPU_CMD_RESPONSE,                                  \
                                        cmdStatus,                                                                  \
                                        recoveryAction);                                                            \
    return recoveryAction;                                                                                          \
}                                                                                                                   \

#define ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(cmdStatusErrorCode, cmdStatus, cpuErrorCode, recoveryAction, label)\
                                                                                                                    \
if (cmdStatus == ADRV904X_CPU_CMD_STATUS_CMD_FAILED)                                                                \
{                                                                                                                   \
    cpuErrorCode = ADRV904X_CTOHL(cmdStatusErrorCode);                                                              \
    ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_DFE_SVC_RUNTIME,                                        \
                                        cpuErrorCode,                                                               \
                                        cmdStatus,                                                                  \
                                        recoveryAction);                                                            \
    goto label;                                                                                                     \
}                                                                                                                   \
else                                                                                                                \
{                                                                                                                   \
    ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_DFE_CPU,                                                \
                                        ADI_ADRV904X_ERRCODE_DFE_CPU_CMD_RESPONSE,                                  \
                                        cmdStatus,                                                                  \
                                        recoveryAction);                                                            \
    goto label;                                                                                                     \
}

#define ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_GOTO(cmdStatusErrorCode, cmdStatus, cpuErrorCode, recoveryAction, label)\
                                                                                                                    \
if (cmdStatus == ADRV904X_CPU_CMD_STATUS_CMD_FAILED)                                                                \
{                                                                                                                   \
    cpuErrorCode = ADRV904X_CTOHL(cmdStatusErrorCode);                                                              \
    ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_DFE_APP_RUNTIME,                                        \
                                        cpuErrorCode,                                                               \
                                        cmdStatus,                                                                  \
                                        recoveryAction);                                                            \
    goto label;                                                                                                     \
}                                                                                                                   \
else                                                                                                                \
{                                                                                                                   \
    ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_DFE_CPU,                                                \
                                        ADI_ADRV904X_ERRCODE_DFE_CPU_CMD_RESPONSE,                                  \
                                        cmdStatus,                                                                  \
                                        recoveryAction);                                                            \
    goto label;                                                                                                     \
}


#define ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_RETURN(cmdStatusErrorCode, cmdStatus, cpuErrorCode, recoveryAction)     \
                                                                                                                        \
if (cmdStatus == ADRV904X_CPU_CMD_STATUS_CMD_FAILED)                                                                    \
{                                                                                                                       \
    cpuErrorCode = ADRV904X_CTOHL(cmdStatusErrorCode);                                                                  \
    ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_DFE_APP_RUNTIME,                                            \
                                        cpuErrorCode,                                                                   \
                                        cmdStatus,                                                                      \
                                        recoveryAction);                                                                \
    return recoveryAction;                                                                                              \
}                                                                                                                       \
else                                                                                                                    \
{                                                                                                                       \
    ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_DFE_CPU,                                                    \
                                        ADI_ADRV904X_ERRCODE_DFE_CPU_CMD_RESPONSE,                                      \
                                        cmdStatus,                                                                      \
                                        recoveryAction);                                                                \
    return recoveryAction;                                                                                              \
}                                                                                                                       \

#define ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_GOTO(cmdStatusErrorCode, cmdStatus, cpuErrorCode, recoveryAction, label)\
                                                                                                                        \
if (cmdStatus == ADRV904X_CPU_CMD_STATUS_CMD_FAILED)                                                                    \
{                                                                                                                       \
    cpuErrorCode = ADRV904X_CTOHL(cmdStatusErrorCode);                                                                  \
    ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_DFE_APP_RUNTIME,                                            \
                                        cpuErrorCode,                                                                   \
                                        cmdStatus,                                                                      \
                                        recoveryAction);                                                                \
    goto label;                                                                                                         \
}                                                                                                                       \
else                                                                                                                    \
{                                                                                                                       \
    ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_DFE_CPU,                                                    \
                                        ADI_ADRV904X_ERRCODE_DFE_CPU_CMD_RESPONSE,                                      \
                                        cmdStatus,                                                                      \
                                        recoveryAction);                                                                \
    goto label;                                                                                                         \
}
/**
* \brief Get DFE SDK LID Version Global Address
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[out] dfeSdkDataAddr Pointer to DFE SDK DATA Address
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeSdkDataAddrGet(adi_adrv904x_Device_t* const device,
                                                            uint32_t* const dfeSdkDataAddr);

/**
* \brief Set capture buffer access parameters for selected cal and channels
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] txChannelMask Tx channel mask to apply the command
* \param[in] objId Cal obj id
* \param[in] cpuCmd Cal specific ctrl command for selected object id
* \param[in] capBufAccessSet Pointer to the DPD Capture Buffers Access config structure which contains the configuration to be set in FW
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeCaptureBuffersAccessSet(adi_adrv904x_Device_t* const                                    device,
                                                                     const uint32_t                                                  txChannelMask,
                                                                     const uint32_t                                                  objId,
                                                                     const uint16_t                                                  cpuCmd,
                                                                     adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessSet_t* const capBufAccessSet);

/**
* \brief Get capture buffer access parameters for selected cal and channels
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] txChannelSel Tx channel selection to read the capture buffer data
* \param[in] objId Cal obj id
* \param[in] cpuCmd Cal specific ctrl command for selected object id
* \param[out] capBufAccessGet Pointer to the DPD Capture Buffers data structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeCaptureBuffersAccessGet(adi_adrv904x_Device_t* const                                    device,
                                                                     const adi_adrv904x_TxChannels_e                                 txChannelSel,
                                                                     const uint32_t                                                  objId,
                                                                     const uint16_t                                                  cpuCmd,
                                                                     adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessGet_t* const capBufAccessGet);
    
/**
* \brief Calculates a standard 32 bit crc for a block of data
* \details The seedCrc in first block must be set to 0xFFFFFFFF and finalCrc must 
* be set to 1 on the last block. 
* Polynomial: 0x04C11DB7 
* Shift: Left 
* Reverse Data: Yes
* Reverse CRC on final block: Yes
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] buf buffer containing the data on which the crc will be calculated.
* \param[in] bufLen the number of bytes in the buffer
* \param[in] seedCrc initial CRC value.
* \param[in] finalCrc flag indicating that the block is final
*
* \retval calculated CRC. This value must be fed back into seedCRC on subsequent calls.
*/
unsigned int adrv904x_DfeCrc32Chunk( const uint8_t       buf[],
                                    const uint32_t      bufLen,
                                    const uint32_t      seedCrc,
                                    const uint8_t       finalCrc );


/**
* \brief Send a service command to the DFE and receive a command response
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device  Structure pointer to the ADRV904X data structure containing settings
* \param linkId  ID of the desired CPU's link for the command
* \param cmdId  ID of the service command to send
* \param pCmdPayload  Pointer to the data payload to send with this command. Optional. Set to NULL if not needed.
* \param cmdPayloadSize Size of data payload, in bytes. Set to zero if pCmdPayload is set to NULL.
* \param[out] pRespPayload  Pointer to buffer in which command response payload should be placed. Optional. Set to NULL if not needed.
* \param respPayloadSz  Amount of data to copy into pRespPayload, in bytes. Set to zero if pRespPayload is set to NULL.
* \param[out] status  Status of command response. Optional. Set to NULL if not needed.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeSvcCmdSend(adi_adrv904x_Device_t* const        device,
                                                        const adrv904x_LinkId_e             linkId,
                                                        const uint8_t                       cmdId,
                                                        void* const                         pCmdPayload,
                                                        const size_t                        cmdPayloadSz,
                                                        void* const                         pRespPayload,
                                                        const size_t                        respPayloadSz,
                                                        adrv904x_DfeSvcCmdStatus_t* const   status);

/**
* \brief Send an application command to the DFE and receive a command response
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device  Structure pointer to the ADRV904X data structure containing settings
* \param linkId  ID of the desired CPU's link for the command
* \param cmdId  ID of the application command to send
* \param pCmdPayload  Pointer to the data payload to send with this command. Optional. Set to NULL if not needed.
* \param cmdPayloadSize Size of data payload, in bytes. Set to zero if pCmdPayload is set to NULL.
* \param[out] pRespPayload  Pointer to buffer in which command response payload should be placed. Optional. Set to NULL if not needed.
* \param respPayloadSz  Amount of data to copy into pRespPayload, in bytes. Set to zero if pRespPayload is set to NULL.
* \param[out] status  Status of command response. Optional. Set to NULL if not needed.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeAppCmdSend(adi_adrv904x_Device_t* const        device,
                                                        const adrv904x_LinkId_e             linkId,
                                                        const uint8_t                       cmdId,
                                                        void* const                         pCmdPayload,
                                                        const size_t                        cmdPayloadSz,
                                                        void* const                         pRespPayload,
                                                        const size_t                        respPayloadSz,
                                                        adrv904x_DfeSvcCmdStatus_t* const   status);

/**
* \brief Function used by ADRV904X API to test API/FW Ping
*
* This function sends a Ping command through the CPU Mailbox interface to see if the CPU is functioning
* 
* \pre This function may be called any time after device initialization
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device data structure containing settings
* \param[in] cpuType    Selection of the desired CPU processor to execute this command on.
* \param[in] writeData  data write to CPU.
* \param[out] readData   data read from CPU.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeCpuAppPing(adi_adrv904x_Device_t* const    device,
                                                        const adi_adrv904x_CpuType_e    cpuType,
                                                        const uint32_t                  writeData,
                                                        uint32_t* const                 readData);

/**
* \brief Validate the Packaging Information
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device  Structure pointer to the ADRV904X data structure containing settings
* \param cpuDfeBinaryInfo  adi_adrv904x_DfeCpuBinaryInfo_t to be checked
*
* \retv ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
 ADI_API adi_adrv904x_ErrAction_e adrv904x_PackagingInformationValidate(adi_adrv904x_Device_t* const device,
                                                                        const adi_adrv904x_DfeCpuBinaryInfo_t* const cpuDfeBinaryInfo);
 /**
 * \brief Get exception data from BBIC bridge
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device  Structure pointer to the ADRV904X data structure containing settings
 * \param dfeExceptionData  adrv904x_DfeSvcCmdDfeExceptionData_t structure to copy exception data into
 *
 * \retv ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
 ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeExceptionDataGet(adi_adrv904x_Device_t* const device,
                                                                    adrv904x_DfePlatformExceptionData_t* dfeExceptionData);

/**
* \brief Calculates a standard 32 bit crc for a block of data
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] objId the calibration object ID to get the calibration status from
* \param[in] channel the channel to get the calibration status from
* \param[out] status the structure containing the status information from the specified 
*                       channel and calibration object ID.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if successful
*/
adi_adrv904x_ErrAction_e adrv904x_DfeCalCommonStatusGet(adi_adrv904x_Device_t* const device,
                                                        const uint32_t objId,
                                                        const adi_adrv904x_Channels_e channel,
                                                        adi_adrv904x_CalStatus_t* const status);
/**
* \brief Top level Send/Receive System Status command for public status
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] objId the object ID to get the system status from
* \param[in] channel the channel to get the system status from
* \param[in,out] status the structure containing the status information from the specified 
*                       channel and calibration object ID.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if successful
*/
adi_adrv904x_ErrAction_e adrv904x_DfeSysStatusGet(adi_adrv904x_Device_t* const device,
                                                  const adrv904x_CpuObjectId_e objId,
                                                  const adi_adrv904x_Channels_e channel,
                                                  adi_adrv904x_CpuSysStatus_t* const status);

/**
* \brief Sends a DfeGetStatus command
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] type either public or private system status
* \param[in] sysObjId object ID in the system to get the status from
* \param[in] channel the channel to get the system status from
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if successful
*/
adi_adrv904x_ErrAction_e adrv904x_DfeSendSysStatusCmd(adi_adrv904x_Device_t* const device,
                                                      const adrv904x_DfeFrameworkCmdSysStatusType_e type,
                                                      const adrv904x_CpuObjectId_e sysObjId,
                                                      const adi_adrv904x_Channels_e channel);

/**
* \brief Gets the response to a DfeGetSysStatus command
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] channel to get the response from
* \param[in] sysStatusSize  size of the expected system status
* \param[in,out] RxBuf Received data buffer
* \param[out] SysStatus status of the system status request
*
* \retval  ADI_ADRV904X_ERR_ACT_NONE if successful
*/
adi_adrv904x_ErrAction_e adrv904x_DfeGetSysStatusCmdResp(adi_adrv904x_Device_t* const device,
                                                         const adi_adrv904x_Channels_e channel,
                                                         const size_t sysStatusSize,
                                                         void* const RxBuf,
                                                         const void** const SysStatus);

/**
* \brief Converts a tracking cal mask enumeration to an object ID
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] calId  enumerated tracking cal mask
*
* \retval Object ID associated with the provided enumerated tracking cal
*/
uint32_t adrv904x_DfeTrackingCalToObjId(const adi_adrv904x_DfeAppFrameworkTrackingCalibrationMask_e calId);

/**
* \brief Verify that the channel provided is an available one in the system
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] channel enumerated value indicating a particular channel
*
* \retval  ADI_ADRV904X_ERR_ACT_NONE if the channel has been verified
*/
adi_adrv904x_ErrAction_e adrv904x_DfeVerifyChannel(const adi_adrv904x_Channels_e channel);

/**
* \brief Finds the bit position of the first 1 in a bitfield
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] mask bitfield potentially containing one or more 1s
*
* \retval bit location of the first 1 in mask
*/
uint32_t adrv904x_DfeGetBitPosition(const uint32_t mask);

/**
* \brief Converts the enumerated channel adi_adrv904x_Channels_e to a number
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] mask the enumerated channel mask to convert
*
* \retval channel number
*/
uint32_t adrv904x_DfeChanMaskToNum(const adi_adrv904x_Channels_e mask);

/**
* \brief Converts the enumerated channel adi_adrv904x_Channels_e to a channel ID
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] channel adi_adrv904x_Channels_e to be converted
*
* \retval channel ID
*/
uint32_t adrv904x_DfeChannelToChannelId(const adi_adrv904x_Channels_e channel);

/**
* \brief Sends command to disable or enable ECC of specified memory regions 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] eccEnableMemRegion bitfield of memory regions to enable or disable for ECC
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
adi_adrv904x_ErrAction_e adrv904x_DfeEccEnableSet(adi_adrv904x_Device_t* const device,
                                                  const uint32_t eccEnableMemRegion); 

/**
* \brief Sends command to get the ECC state  of specified memory regions (enabled or disabled)
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in,out] eccEnableMemRegion pointer to the bitfield of memory regions state for ECC
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
adi_adrv904x_ErrAction_e adrv904x_DfeEccEnableGet(adi_adrv904x_Device_t* const device,
                                                  uint32_t* const  eccEnableMemRegion);

/**
* \brief Read the last errorcode used by the logTrace module from the SDK.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device data structure containing settings
* \param[out]    lastErrorCode last errorcode used by the logTrace module
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
adi_adrv904x_ErrAction_e adrv904x_DfeLastErrorCodeGet(adi_adrv904x_Device_t* const               device,
                                                      adi_adrv904x_DfeSvcLogTraceErrLog_t* const lastErrorCode);


/**
* \brief Service to debug dfe cpu after runtime error
*
* \pre This function is called after the dfe cpu has errored
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
*
*/
ADI_API void adrv904x_DfeCpuErrorDebugCheck(adi_adrv904x_Device_t* const device);


#endif
