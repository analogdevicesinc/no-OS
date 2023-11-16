
/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adrv904x_cpu.h
 * \brief Contains ADRV904X CPU related private function prototypes for
 *        adrv904x_cpu.c which helps adi_adrv904x_cpu.c
 *
 * ADRV904X API Version: 2.9.0.4
 */
#ifndef _ADRV904X_CPU_H_
#define _ADRV904X_CPU_H_

#include "adrv904x_cpu_types.h"
#include "adrv904x_cpu_macros.h"
#include "adrv904x_cpu_health_monitor_types.h"
#include "adrv904x_cpu_cmd.h"
#include "../../private/bf/adrv904x_bf_core.h"

#include "adi_adrv904x_cpu_types.h"
#include "adi_adrv904x_error_types.h"
#include "adi_adrv904x_error.h"

#define ADI_ADRV904X_CPU_CMD_RESP_CHECK_RETURN(cmdStatusErrorCode, cmdStatus, cpuErrorCode, recoveryAction)         \
                                                                                                                    \
if (cmdStatus == ADRV904X_CPU_CMD_STATUS_CMD_FAILED)                                                                \
{                                                                                                                   \
    cpuErrorCode = ADRV904X_CTOHL(cmdStatusErrorCode);                                                              \
    ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_CPU_RUNTIME,                                            \
                                        cpuErrorCode,                                                               \
                                        cmdStatus,                                                                  \
                                        recoveryAction);                                                            \
    return recoveryAction;                                                                                          \
}                                                                                                                   \
else                                                                                                                \
{                                                                                                                   \
    ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_CPU,                                                    \
                                        ADI_ADRV904X_ERRCODE_CPU_CMD_RESPONSE,                                      \
                                        cmdStatus,                                                                  \
                                        recoveryAction);                                                            \
    return recoveryAction;                                                                                          \
}                                                                                                                   \

#define ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdStatusErrorCode, cmdStatus, cpuErrorCode, recoveryAction, label)    \
                                                                                                                    \
if (cmdStatus == ADRV904X_CPU_CMD_STATUS_CMD_FAILED)                                                                \
{                                                                                                                   \
    cpuErrorCode = ADRV904X_CTOHL(cmdStatusErrorCode);                                                              \
    ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_CPU_RUNTIME,                                            \
                                        cpuErrorCode,                                                               \
                                        cmdStatus,                                                                  \
                                        recoveryAction);                                                            \
    goto label;                                                                                                     \
}                                                                                                                   \
else                                                                                                                \
{                                                                                                                   \
    ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_CPU,                                                    \
                                        ADI_ADRV904X_ERRCODE_CPU_CMD_RESPONSE,                                      \
                                        cmdStatus,                                                                  \
                                        recoveryAction);                                                            \
    goto label;                                                                                                     \
}                                                                                                                   \

typedef adi_adrv904x_ErrAction_e(*adrv904xCoreBfSet8FnPtr_t)(   adi_adrv904x_Device_t* const    device,
                                                                adi_adrv904x_SpiCache_t* const  spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint8_t                   bfValue);

typedef adi_adrv904x_ErrAction_e(*adrv904xCoreBfGet8FnPtr_t)(   adi_adrv904x_Device_t* const    device,
                                                                adi_adrv904x_SpiCache_t* const  spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint8_t* const                  bfValue);

typedef adi_adrv904x_ErrAction_e(*adrv904xCoreBfSet16FnPtr_t)(  adi_adrv904x_Device_t* const    device,
                                                                adi_adrv904x_SpiCache_t* const  spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t                  bfValue);

typedef adi_adrv904x_ErrAction_e(*adrv904xCoreBfGet16FnPtr_t)(  adi_adrv904x_Device_t* const    device,
                                                                adi_adrv904x_SpiCache_t* const  spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const                 bfValue);

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
ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuPing(adi_adrv904x_Device_t* const    device,
                                                  const adi_adrv904x_CpuType_e    cpuType,
                                                  const uint32_t                  writeData,
                                                  uint32_t* const                 readData);

/**
* \brief Reads back efuse data from CPU
*
* \pre This function is called after the device is initialized. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device data structure containing settings
* \param[in] address efuse address.
* \param[out] value pointer to a memory location of retuning efuse data.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuEfuseGet(adi_adrv904x_Device_t* const device,
                                                      const uint32_t               address,
                                                      uint32_t* const              value);

/**
* \brief Function used by ADRV904X API to test API/FW CPU Force Exception
*
* This function sends a Force Exception command through the CPU Mailbox Command interface to force a CPU exception
* 
* \pre This function may be called any time after device initialization
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device data structure containing settings
* \param[in] cpuType    Selection of the desired CPU processor to execute this command on.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuForceException(adi_adrv904x_Device_t* const device,
                                                            const adi_adrv904x_CpuType_e cpuType);

/**
* \brief Read the M4 ECC scrubbing enable state
*
* \pre This function is called after the device is initialized. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out]    eccEnable M4 ECC enable state (0 = disabled, !0 = enabled)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_EccEnableGet(adi_adrv904x_Device_t* const device,
                                                           uint8_t* const  eccEnable);

/**
* \brief Read the M4 ECC scrubbing enable state
*
* \pre This function is called after the device is initialized. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] eccEnable M4 ECC enable state to set (0 = disabled, !0 = enabled)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_EccEnableSet(adi_adrv904x_Device_t* const device,
                                                           uint8_t const  eccEnable);

#ifndef CLIENT_IGNORE
/**
* \brief Private Helper function to initialize cpu's data structure
*
*  This is a private function and is automatically called by the API.
*
* \param[in,out] device Structure pointer to the ADRV904X data structure containing settings
*        
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuInitialize(adi_adrv904x_Device_t* const device);

/**
* \brief Private Helper function to get a CPU's key addresses
*
*  This is a private function and is automatically called by the API.
*
* \param[in] cpu Structure pointer to the ADRV904X's CPU data structure containing settings
* \param[in] cpuType type of cpu
*
* \retval adi_adrv904x_CpuAddr_t* for the requested CPU's or NULL if not found.
*/
ADI_API adi_adrv904x_CpuAddr_t* adrv904x_CpuAddrGet(adi_adrv904x_Cpu_t* const       cpu,
                                                    const adi_adrv904x_CpuType_e    cpuType);

                                                    
/**
* \brief Private Helper function to get a uint32_t from binary array
*
*  This is a private function and is automatically called by the API.
*
* \param[in] buf Structure pointer to buffer array
* \param[in] size number of bytes to convert to uint32_t 
*
* \retval uint32_t return uint32_t from buffer
*/
ADI_API uint32_t adrv904x_CpuIntFromBytesGet(const uint8_t* const buf, const uint8_t size);

/**
* \brief Low level helper function used by ADRV904X API to write CPU Command
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device     Structure pointer to the ADRV904X data structure containing settings
* \param cpuType    Selection of the desired CPU processor to execute this command on.
* \param linkId     Selection of the desired Link for the command.
* \param cmdId      To Identify a Command.
* \param cmd        Contain command buffer.
* \param payloadSize Number of bytes in the cmd payload
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuCmdWrite(adi_adrv904x_Device_t* const    device,
                                                      const adi_adrv904x_CpuType_e    cpuType,
                                                      const adrv904x_LinkId_e         linkId,
                                                      const adrv904x_CpuCmdId_t       cmdId,
                                                      adrv904x_CpuCmd_t* const        cmd,
                                                      const uint32_t                  payloadSize);

/**
* \brief Low level helper function used by ADRV904X API to read CPU Command Response
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV904X data structure containing settings
* \param cpuType Selection of the desired CPU processor to execute this command on.
* \param linkId  Selection of the desired Link for the command.
* \param *cmdId  To Identify a Command.
* \param cmdRsp  Contain command response buffer.
* \param payloadSize Size of expected payload, in bytes
* \param[out] status Status of command response. Optional. Set to NULL if not needed.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuCmdRespRead(adi_adrv904x_Device_t* const    device,
                                                         const adi_adrv904x_CpuType_e    cpuType,
                                                         const adrv904x_LinkId_e         linkId,
                                                         adrv904x_CpuCmdId_t*const       cmdId,
                                                         adrv904x_CpuCmdResp_t* const    cmdRsp,
                                                         const uint32_t                  payloadSize,
                                                         adrv904x_CpuCmdStatus_e*const   status);

/**
* \brief Send a command to a given CPU and receive a command response
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device  Structure pointer to the ADRV904X data structure containing settings
* \param cpuType  ID of the desired CPU to execute this command on
* \param linkId  ID of the desired CPU's link for the command
* \param cmdId  ID of the command to send
* \param pCmdPayload  Pointer to the data payload to send with this command. Optional. Set to NULL if not needed.
* \param cmdPayloadSize Size of data payload, in bytes. Set to zero if pCmdPayload is set to NULL.
* \param[out] pRespPayload  Pointer to buffer in which command response payload should be placed. Optional. Set to NULL if not needed.
* \param respPayloadSz  Amount of data to copy into pRespPayload, in bytes. Set to zero if pRespPayload is set to NULL.
* \param[out] status  Status of command response. Optional. Set to NULL if not needed.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuCmdSend(adi_adrv904x_Device_t* const       device,
                                                     const adi_adrv904x_CpuType_e       cpuType,
                                                     const adrv904x_LinkId_e            linkId,
                                                     const adrv904x_CpuCmdId_t          cmdId,
                                                     void* const                        pCmdPayload,
                                                     const size_t                       cmdPayloadSz,
                                                     void* const                        pRespPayload,
                                                     const size_t                       respPayloadSz,
                                                     adrv904x_CpuCmdStatus_e* const     status);

/**
* \brief Get the CPU assigned to a particular channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device  Structure pointer to the ADRV904X data structure containing settings
* \param[in] channel channel number
* \param[in] objId   object ID
* \param[out] cpuType CPU assigned to requested channel number
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuChannelMappingGet(adi_adrv904x_Device_t* const     device,
                                                               const adi_adrv904x_Channels_e    channel,
                                                               const adrv904x_CpuObjectId_e     objId,
                                                               adi_adrv904x_CpuType_e* const    cpuType);

/**
* \brief Start a capture RAM access (obtain exclusive lock)
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device         structure pointer to the ADRV904X data structure containing settings
* \param[in]  captureRamType type of capture RAM to lock
* \param[in]  channelNumber  channel number of capture RAM to lock (one channel, NOT a mask of multiple channels)
* \param[out] success        1 if successfully locked, 0 otherwise
*
* \retval ADI_COMMON_ACT_NO_ACTION if no unexpected failure was encountered (failure other than lock failure).
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuRamAccessStart(adi_adrv904x_Device_t* const            device,
                                                            const adrv904x_CpuCmd_CaptureRamType_e  captureRamType,
                                                            const adi_adrv904x_Channels_e           channelNumber,
                                                            uint8_t* const                          success);

/**
* \brief Stop a capture RAM access (release exclusive lock)
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device         structure pointer to the ADRV904X data structure containing settings
* \param[in]  captureRamType type of capture RAM to unlock
* \param[in]  channelNumber  channel number of capture RAM to unlock (one channel, NOT a mask of multiple channels)
* \param[out] success        1 if successfully unlocked, 0 otherwise
*
* \retval ADI_COMMON_ACT_NO_ACTION if no unexpected failure was encountered (failure other than unlock failure).
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuRamAccessStop( adi_adrv904x_Device_t* const            device,
                                                            const adrv904x_CpuCmd_CaptureRamType_e  captureRamType,
                                                            const adi_adrv904x_Channels_e           channelNumber,
                                                            uint8_t* const                          success);




/**
* \brief       Run Cyclic Redundancy Check on the specified block of memory in chunk.
*
* This function was based on the Crc32 function but can be used on chunk of memory block.
* The function can be call multiple times.
* The first call, set seedCrc to 0, finalCrc to 0.
* The return CRC is use as seedCrc for the next call.
* The last call, set finalCrc to 1.
*
* \details     CRC32 algorithm, operating on 8-bit words
*
* Parameters:
* \param[in] buf - array of bytes on which CRC is run
* \param[in] bufLen - length of the input array in bytes
* \param[in] seedCrc - Seed for the next block of memory, use 0 for initial seedCrc.
* \param[in] finalCrc - 0: return the CRC use for seedSrc. 1: return the final CRC32.
*
* \retval     32-bit checksum
*/
ADI_API uint32_t adrv904x_Crc32ForChunk(const uint8_t       buf[],
                                        const uint32_t      bufLen,
                                        const uint32_t      seedCrc,
                                        const uint8_t       finalCrc);

/**
* \brief Reads back private health monitoring status of the CPUs
*
* \pre This function is called after the device is initialized. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] healthMonitorStatus Pointer to memory location where private health monitor CPU status readback data will be written
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_HealthMonitorPrivateCpuStatusGet( adi_adrv904x_Device_t* const                        device,
                                                                            adrv904x_HealthMonitorPrivateCpuStatus_t* const     healthMonitorStatus);

/**
* \brief Configures the GPIO signal for both CPUs
*
* \pre This function is called after the device is initialized. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] gpioSelect GPIO selection for the CPU signal
* \param[in] cpuGpioSignal CPU GPIO signal selection
* \param[in] isInput Needs to be set to 1:If selected signal is input to CPU  0:If selected signal is output from CPU
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuGpioSet(adi_adrv904x_Device_t* const device,
                                                     const adi_adrv904x_GpioPinSel_e gpioSelect,
                                                     const adrv904x_CpuCmd_GpioSignal_e cpuGpioSignal,
                                                     const uint8_t isInput);

/**
* \brief Read the GPIO pin for a given CPU GPIO signal
*
* \pre This function is called after the device is initialized. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] gpioSelect Readback GPIO pin for the given CPU signal
* \param[in] cpuGpioSignal CPU GPIO signal selection
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuGpioGet(adi_adrv904x_Device_t* const device,
                                                     adi_adrv904x_GpioPinSel_e * const gpioSelect,
                                                     const adrv904x_CpuCmd_GpioSignal_e cpuGpioSignal);


/**
* \brief Service to debug cpu after runtime error
*
* \pre This function is called after the cpu has errored
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
*
*/
ADI_API void adrv904x_CpuErrorDebugCheck(adi_adrv904x_Device_t* const device);

#endif //CLIENT_IGNORE



#endif /* _ADRV904X_CPU_H_ */
