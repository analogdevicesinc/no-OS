/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_cpu.h
* \brief Contains ADRV904X processor function prototypes for
*    adi_adrv904x_cpu.c
*
* ADRV904X API Version: 2.10.0.4
*/

#ifndef _ADI_ADRV904X_CPU_H_
#define _ADI_ADRV904X_CPU_H_

#include "adi_adrv904x_cpu_types.h"
#include "adi_adrv904x_error.h"
#include "adi_adrv904x_cpu_log_types.h"
#include "adi_adrv904x_cpu_sys_types.h"
#include "adi_adrv904x_cpu_sw_bkpt_types.h"
#include "adi_adrv904x_cpu_health_monitor_types.h"
#include "adi_common_error_types.h"

/**
* \brief Loads binary array into CPU program memory
*
* This function sets the CPU DMA control register bits for an CPU memory write, auto-incrementing
* the address. Valid memory addresses are: Program Memory (0x01000000 - 0x0104FFFF)
*
* The top level application reads the binary file, parsing it into any array, starting at the first data byte
* on the first line of the file. The array is passed to this function and writes it to CPU program memory. Any non-data
* bytes should be excluded from the binary array when parsing.
*
* \pre This function is called after the device has been initialized, PLL lock status has been verified, and
* the stream binary has been loaded
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device data structure containing settings
* \param[in] cpuType    type of cpu
* \param[in] byteOffset Offset (starting from 0) of where to place the binary
*                       array in CPU memory (if loaded in multiple function
*                       calls)
* \param[in] binary     Byte array containing all valid FW file data bytes
* \param[in] byteCount  The number of bytes in the binary array
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuImageWrite(adi_adrv904x_Device_t* const    device,
                                                            const adi_adrv904x_CpuType_e    cpuType,
                                                            const uint32_t                  byteOffset,
                                                            const uint8_t                   binary[],
                                                            const uint32_t                  byteCount);

/**
* \brief Writes the ADRV904X CPU device profile binary
*
* The top level application reads the binary file, parsing it into any array, starting at the first data byte
* on the first line of the file. The array is passed to this function and writes it to CPU device profile memory. Any non-data
* bytes should be excluded from the binary array when parsing.
*
* \pre This function is called before adi_adrv904x_CpuStart(), and
*      this function must be called after loading the firmware.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device data structure containing settings
* \param[in] byteOffset Offset (starting from 0) of where to place the binary
*                   array in CPU memory (if loaded in multiple function
*                   calls)
* \param[in] binary Byte array containing all valid FW file data bytes
* \param[in] byteCount The number of bytes in the binary array
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuProfileWrite(adi_adrv904x_Device_t * const   device,
                                                              const uint32_t                  byteOffset,
                                                              const uint8_t                   binary[],
                                                              const uint32_t                  byteCount);

/**
* \brief Start the CPU(s)
*
* Sets run bit to 1. Then wait and check for FW Status.
*
*
* \pre This function is called after the device has been initialized and before multichip-sync
* (MCS) has been completed
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuStart(adi_adrv904x_Device_t* const device);

/**
* \brief A polling wait for the CPU status to be ready.
*
* Polls firmware status (fwStatus) until it becomes ADRV904X_CPU_FW_STATUS_READY,
* ADRV904X_CPU_FW_STATUS_CPU_DEBUG_READY, any of the error states or a timeout occurs.
*
* If a previous call to this function returned with fwStatus as ADRV904X_CPU_FW_STATUS_CPU_DEBUG_READY
* then even if fwStatus becomes STATUS_CPU_DEBUG_READY during the timeout period wait until the timeout
* duration to see if it becomes STATUS_READY. At that point return based on the fwStatus.
*
* Returns and error if fwStatus becomes any of the firmware error states. Calls adi_adrv904x_HwReset if
* fwStatus becomes LDO config error.
*
* \pre This function is called after the device has been initialized and before multichip-sync
* (MCS) has been completed.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in] timeout_us Timeout to stop waiting for CPU to boot up.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuStartStatusCheck(adi_adrv904x_Device_t* const    device,
                                                                  const uint32_t                  timeout_us);

/**
* \brief Sets the specified Set Config in the adrv904x_CpuCmd_SetConfig_t structure.
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] objId  Object ID of calibration or system component
* \param[in] offset Offset into the configuration structure
* \param[in] configDataSet Configuration data for Set. Size of configDataSet must be equal or greater than length param.
* \param[in] length Length of the configuration in bytes
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuConfigSet( adi_adrv904x_Device_t* const    device,
                                                            const uint32_t                  objId,
                                                            const uint16_t                  offset,
                                                            const uint8_t                   configDataSet[],
                                                            const uint32_t                  length);

/**
* \brief Gets the specified Get Config in the adrv904x_CpuCmd_GetConfig_t structure.
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] objId  Object ID of calibration or system component
* \param[in] offset Offset into the configuration structure
* \param[in, out] configDataGet Configuration data for Get. Size of configDataGet must be equal or greater than length param.
* \param[in] length Length of the configuration in bytes
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuConfigGet( adi_adrv904x_Device_t* const    device,
                                                            const uint32_t                  objId,
                                                            const uint16_t                  offset,
                                                            uint8_t                         configDataGet[],
                                                            const uint32_t                  length);

/**
* \brief Enable CPU debug mode with enableKey.
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] enableKey key to enable CPU Debug
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuDebugModeEnable(adi_adrv904x_Device_t* const   device,
                                                                 const uint32_t                 enableKey);

/**
* \brief Set Cpu Config Unlock with configKey.
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] configKey key to unlock Cpu Config
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuConfigUnlock(adi_adrv904x_Device_t* const  device,
                                                              const uint32_t                configKey);

                                                              /**
* \brief Sets the specified Set Ctrl in the adrv904x_CpuCmd_SetCtrl_t structure.
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] objId  Object ID of calibration or system component
* \param[in] cpuCmd Command to be executed
* \param[in] channel channel enum
* \param[in] cpuCtrlData Pointer to data to send TO the CPU
* \param[in] lengthSet Length of cpuCtrlData, in bytes, if 0, cpuCtrlData can be NULL
* \param[out] lengthResp Length of data received FROM the CPU into ctrlResp buffer, in bytes
* \param[out] ctrlResp Pointer to buffer in which data FROM the CPU should be received.
* \param[in] lengthGet Size of ctrlResp buffer, in bytes. MUST be greater than or equal to lengthResp.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuControlCmdExec(adi_adrv904x_Device_t* const    device,
                                                                const uint32_t                  objId,
                                                                const uint16_t                  cpuCmd,
                                                                const adi_adrv904x_Channels_e   channel,
                                                                const uint8_t                   cpuCtrlData[],
                                                                const uint32_t                  lengthSet,
                                                                uint32_t* const                 lengthResp,
                                                                uint8_t                         ctrlResp[],
                                                                const uint32_t                  lengthGet);

/**
* \brief Configure CPU log filter settings
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] eventFilter Specifies the highest severity event to log. For example, if ADI_ADRV904X_CPU_LOG_EVENT_WARNING
*                        is selected, WARNING and ERROR events will be logged, but INFO and INFO_2 will not. Set to INVALID
*                        if this option should be ignored.
* \param[in] cpuIdFilter Bitmask that specifies the CPU(s) on which logging should be enabled. A single CPU can be specified,
*                        or NONE (logging disabled), or ALL. Set to INVALID if this option should be ignored.
* \param[in] objIdFilter Specifies the CPU object ID that should be logged. Setting objIdFilter.objIdFilterEnable to DISABLE allows
*                        logging from all object IDs (and objIdFilter.objId is ignored). Setting objIdFilter.objIdFilterEnable
*                        to ENABLE and setting objIdFilter.objId to a valid CPU object ID (see adrv904x_CpuObjectId_e) enables
*                        logging for only the specified object ID. Set to objIdFilterEnable to INVALID if this option should be
*                        ignored.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuLogFilterSet(adi_adrv904x_Device_t* const                  device,
                                                              const adi_adrv904x_CpuLogEvent_e              eventFilter,
                                                              const adi_adrv904x_CpuLogCpuId_e              cpuIdFilter,
                                                              const adi_adrv904x_CpuLogObjIdFilter_t* const objIdFilter);

/**
* \brief Sets the specified Cpu Debug command in the adrv904x_CpuCmd_Debug_t structure.
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] objId  Object ID of calibration or system component
* \param[in] cpuCmd Command to be executed
* \param[in] channel channel enum
* \param[in] cpuDebugData Debug data for Debug command.
* \param[in] lengthSet Length of the Debug data in bytes
* \param[out] lengthResp Length of the Control set command response in bytes
* \param[in, out] debugResp Debug response buffer.
* \param[in] lengthGet Length of response buffer in bytes.MUST be greater than or equal to lengthResp.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuDebugCmdExec(  adi_adrv904x_Device_t* const    device,
                                                              const uint32_t                    objId,
                                                              const uint16_t                    cpuCmd,
                                                              const adi_adrv904x_Channels_e     channel,
                                                              const uint8_t                     cpuDebugData[],
                                                              const uint32_t                    lengthSet,
                                                              uint32_t* const                   lengthResp,
                                                              uint8_t                           debugResp[],
                                                              const uint32_t                    lengthGet);

/**
* \brief Get the CPU index assigned to all JESD deserializer lanes in an array format.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
* \param[in, out] cpuTypes array which will contain what CPU is assigned to the associated channel number 
*                          based on index position i.e. cpuTypes[0] is ADI_ADRV904X_CH1, cpuTypes[1] is ADI_ADRV904X_CH2, etc.
* \param[in] numSerdesLanes number of deserializer lanes desired to be inspected. Should be ADI_ADRV904X_MAX_SERDES_LANES to 
*                           inspect all lanes. If smaller, will only inspect up to the index provided, e.g. 4 will look only up to ADI_ADRV904X_CH4
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuChannelMappingGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_CpuType_e       cpuTypes[],
                                                                   const uint8_t                numSerdesLanes);

/**
* \brief Returns detailed status information specific to the private cpu system component
*
* \pre This function may be called any time after the device has been initialized, and
* initialization calibrations have taken place
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
* \param[in] channel Channel selection to read back cal status. Must be a single channel
* \param[in] objId Object ID of cpu system component
* \param[in, out] cpuSysStatusGet Status of the cpu system component. Size of cpuSysStatusGet must be equal or greater than length param.
* \param[in] length Length of the configuration in bytes
*
* \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuSysPvtStatusGet(adi_adrv904x_Device_t* const   device,
                                                                 const adi_adrv904x_Channels_e  channel,
                                                                 const uint32_t                 objId,
                                                                 uint8_t                        cpuSysStatusGet[],
                                                                 const uint32_t                 length);



/**
* \brief Returns the status of the cpu system component
*
* The function can be called to read back the status of the cpu system component.
*
* \pre This function may be called any time after the device has been initialized, and
* initialization calibrations have taken place
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
* \param[in] channel Channel selection to read back cal status. Must be a single channel.
* \param[in] objId Object ID of cpu system component
* \param[out] cpuSysStatus Status of the cpu system, as a structure of type adi_adrv904x_CpuSysStatus_t is returned to this pointer address
*
* \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuSysStatusGet(adi_adrv904x_Device_t* const                 device,
                                                              const adi_adrv904x_Channels_e                channel,
                                                              const uint32_t                               objId,
                                                              adi_adrv904x_CpuSysStatus_t* const           cpuSysStatus);


/**
* \brief Reads back the FW version of the CPU binary loaded into the CPU memory
*
* This function reads the CPU memory to read back the major.minor.maint.build version
* for the CPU binary loaded into CPU memory.
*
* \pre This function may be called after the CPU binary have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] cpuFwVersion Pointer to four parts of CPU FW Version
*
* \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuFwVersionGet(adi_adrv904x_Device_t* const          device,
                                                              adi_adrv904x_CpuFwVersion_t* const    cpuFwVersion);

/**
* \brief Reads back health monitoring status of the CPUs
*
* \pre This function is called after the device is initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] healthMonitorStatus Pointer to memory location where health monitor CPU status readback data will be written
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_HealthMonitorCpuStatusGet(adi_adrv904x_Device_t* const                    device,
                                                                        adi_adrv904x_HealthMonitorCpuStatus_t* const    healthMonitorStatus);

/**
* \brief Check if CPU-exception flag is set.
*
* \pre This function may be called any time after device initialization
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] isException - Is set to non-zero if either CPU's exception flag is set. The value set is not
*     defined but can be analyzed by ADI and should be included in bug reports.
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if the exception flag was read successfully. 
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuCheckException(adi_adrv904x_Device_t* const    device,
                                                                uint32_t* const                 isException);
    
/**
* \brief Configures FW breakpoint for debugging purposes
*
* User can add/remove breakpoints in various FW features to debug potential issues.
* FW needs to be in debug mode to use breakpoints. When FW hits a breakpoint,
* it will stop the operation of certain subsystems. User is allowed to select which
* subsystems to halt through global halt mask, which can be configured with
* adi_adrv904x_GlobalHaltMaskSet() function. If global halt mask is configured to be 0,
* then FW will only pause the task which hits breakpoint. Others subsystems and tasks
* should continue to operate in this case. User can configure a GPIO output to monitor
* if FW hits a breakpoint during operation. This GPIO can be configured with
* adi_adrv904x_BreakpointGpioCfgSet(). This function can also be used to configure
* a GPIO to resume all tasks sleeping in FW. adi_adrv904x_BreakpointHitRead() function
* can be used to monitor which breakpoint is hit. User can either poll this function
* periodically until FW hits a breakpoint or they can monitor the GPIO output and call
* this function when GPIO is asserted. User can allow the system to resume operation
* with adi_adrv904x_BreakpointResume() function call or asserting selected GPIO if
* global halt mask is 0.  If global halt mask is non-zero
* adi_adrv904x_BreakpointResumeFromHalt() function needs to be used to resume halted subsystems.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization. Device must be in
* debug mode.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure
* \param[in] breakPointCfg Point to breakpoint configuration to set
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointSet(adi_adrv904x_Device_t* const device,
                                                            const adi_adrv904x_SwBreakPointEntry_t * breakPointCfg);

/**
* \brief This function reads the status of selected breakpoint
*
* User is expected to set the objId of the desired breakpoint. Channel mask and
* breakpoint mask configuration of selected breakpoint will be returned to user.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization. Device must be in
* debug mode.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure
* \param[in,out] breakPointCfgRead Pointer to breakpoint structure to readback selected breakpoint
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointGet(adi_adrv904x_Device_t* const device,
                                                            adi_adrv904x_SwBreakPointEntry_t * const breakPointCfgRead);

/**
* \brief Reads back which breakpoint is hit in FW
*
* This function will read the breakpoint hit for both CPUs. It will
* return 0xFFFFFFFF value for all members of breakpoint structure if
* no breakpoint hit for that CPU.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization. Device must be in
* debug mode.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure
* \param[in,out] cpu0BreakpointHit Pointer to breakpoint hit by CPU0
* \param[in,out] cpu1BreakpointHit Pointer to breakpoint hit by CPU1
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointHitRead(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SwBreakPointEntry_t * const cpu0BreakpointHit,
                                                                adi_adrv904x_SwBreakPointEntry_t * const cpu1BreakpointHit);

/**
* \brief Configures a GPIO output to monitor breakpoint hit signal and
* GPIO input to resume all the tasks which are sleeping
*
* FW will use the selected GPIO to output breakpoint hit signal. This signal
* can be monitored by user and adi_adrv904x_BreakpointHitRead() function
* can be used to detect the breakpoint id once GPIO is asserted. To disable GPIO
* output feature, user can select ADI_ADRV904X_GPIO_INVALID
*
* User can configure a GPIO input to resume all sleeping tasks. ADI_ADRV904X_GPIO_INVALID
* can be selected if this feature is not needed.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization. Device must be in
* debug mode.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure
* \param[in] gpioOutputForBreakpointHit Gpio selection to output breakpoint hit signal
* \param[in] gpioInputToResumeSleepingTasks Gpio selection to resume all sleeping tasks

* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointGpioCfgSet(adi_adrv904x_Device_t* const device,
                                                                   const adi_adrv904x_GpioPinSel_e gpioOutputForBreakpointHit,
                                                                   const adi_adrv904x_GpioPinSel_e gpioInputToResumeSleepingTasks);

/**
* \brief This function reads back the GPIO configuration for breakpoint hit
* output signal and breakpoint resume input signals. It returns invalid GPIO
* for the signals which are not assigned to any GPIOs.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization. Device must be in
* debug mode.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure
* \param[out] gpioOutputForBreakpointHit Gpio selection readback for breakpoint hit signal
* \param[out] gpioInputToResumeSleepingTasks Gpio selection readback for resume all signal

* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointGpioCfgGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_GpioPinSel_e * const gpioOutputForBreakpointHit,
                                                                   adi_adrv904x_GpioPinSel_e * const gpioInputToResumeSleepingTasks);

/**
* \brief Allows selected tasks to resume their operation. This function should
* be used when global halt mask is configured to 0.
* ADI cannot guarantee that the system will resume safely as there can be
* instability due to subsystems are assumed to be working at real time.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization. Device must be in
* debug mode.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure
* \param[in] breakpointToResume Selects the breakpoint and channel to resume from sleep. Breakpoint mask is ignored since this
*                               command will resume the breakpoint id regardless of which specific breakpoint is hit for that breakpoint id.
* \param[in] resumeAll 1:Ignore breakpointToResume selection and resume all tasks that are sleeping
                       0:Use breakpointToResume to select which breakpoint to resume


* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointResume(adi_adrv904x_Device_t* const device,
                                                               const adi_adrv904x_SwBreakPointEntry_t * breakpointToResume,
                                                               uint8_t resumeAll);

/**
* \brief Allows halted subsystems to continue operation
*
* ADI cannot guarantee that the system will resume safely as there can be
* instability due to subsystems are assumed to be working at real time.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization. Device must be in
* debug mode.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure

* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointResumeFromHalt(adi_adrv904x_Device_t* const device);

/**
* \brief Configures a global halt mask for FW breakpoints.
* User can select which subsystems to be halted when a breakpoint is hit.
* This global halt mask is shared by all breakpoints (for both CPUs)
* When global halt mask is configured to 0, the task which hits breakpoint will sleep,
* and all other tasks and subsystems will resume operation.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization. Device must be in
* debug mode.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure
* \param[in] globalHaltMask Global halt mask to be set

* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointGlobalHaltMaskSet(adi_adrv904x_Device_t* const device,
                                                                          const uint32_t globalHaltMask);

/**
* \brief This function reads back global halt mask for breakpoints
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization. Device must be in
* debug mode.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure
* \param[out] globalHaltMask Pointer to global halt mask for readback

* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_BreakpointGlobalHaltMaskGet(adi_adrv904x_Device_t* const device,
                                                                          uint32_t * const globalHaltMask);

#endif /* _ADI_ADRV904X_CPU_H_ */
