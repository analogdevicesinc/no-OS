/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
*   \file   adi_adrv904x_ver_redirect.h
* 
*   \brief  Auto generated function pointer shim layer to ease linking to one of multiple 
            versions of the API at runtime
*
*   Generated from ADRV904X API Version: 2.15.0.4
*/

#ifndef API_VER_REDIRECT_H
#define API_VER_REDIRECT_H

#include <stdio.h>

/* Used to mark extern functions correctly for C vs C++ */
#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

/* Used to mark extern functions correctly for C vs C++ */
#ifndef ADI_API_EX
  #ifdef __cplusplus
    #define ADI_API_EX ADI_API
  #else
    #define ADI_API_EX ADI_API extern
  #endif
#endif

#include "adi_adrv904x_all_types.h"

/** Single set of fn ptrs as seen by Application - compatible with the normal
 * API (i.e. no change in app required to use versioned/non-versioned libs)
 *
 * Application linking directly to specific API version pulls in fn decls
 * resolved at link time.
 *
 * Application linking to API-versioning shim pull in a bunch of function
 * pointers which are populated at run-time by calling
 * api_adrv904x_VersionLoad().
 */

ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcCfgSet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_AgcCfg_t agcConfig[], const uint32_t numOfAgcCfgs);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcCfgGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxChannels_e rxChannel, adi_adrv904x_AgcCfg_t * const agcConfigReadBack);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcFreezeSet)(adi_adrv904x_Device_t* const device, const uint32_t rxChannelMask, const uint32_t freezeEnable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcFreezeGet)(adi_adrv904x_Device_t* const device, uint32_t* const freezeEnable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcFreezeOnGpioSet)(adi_adrv904x_Device_t* const device, const uint32_t rxChannels, const adi_adrv904x_GpioPinSel_e gpioPin, const uint8_t enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcFreezeOnGpioGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxChannels_e rxChannel, adi_adrv904x_GpioPinSel_e* const gpioPin);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcGainIndexRangeSet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_AgcGainRange_t agcGainRange[], const uint32_t numOfAgcRangeCfgs);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcGainIndexRangeGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxChannels_e rxChannel, adi_adrv904x_AgcGainRange_t * const agcGainConfigReadback);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcReset)(adi_adrv904x_Device_t * const device, const uint32_t rxChannelMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxDualBandLnaGainTableWrite)(adi_adrv904x_Device_t* const device, const uint32_t rxChannelMask, const uint8_t gainIndexOffset, const adi_adrv904x_RxDualBandLnaGainTableRow_t gainTableRow[], const uint32_t arraySize);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxDualBandLnaGainTableRead)(adi_adrv904x_Device_t* const device, const uint32_t rxChannelMask, const uint8_t gainIndexOffset, adi_adrv904x_RxDualBandLnaGainTableRow_t gainTableRow[], const uint32_t arraySize, uint8_t* const numGainIndicesRead);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcDualBandCfgSet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_AgcDualBandCfg_t agcDualBandConfig[], const uint32_t numOfAgcDualBandCfgs);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcDualBandCfgGet)(adi_adrv904x_Device_t * const device, adi_adrv904x_AgcDualBandCfg_t * const agcDualBandConfigReadBack);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcDualBandGpioCfgSet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_AgcDualBandGpioCfg_t * agcDualBandGpioConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcDualBandGpioCfgGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_AgcDualBandGpioCfg_t* const agcDualBandGpioConfigReadBack);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcGpioReSyncSet)(adi_adrv904x_Device_t * const device, const uint32_t rxChannelMask, const adi_adrv904x_GpioPinSel_e gpioSelection, const uint8_t enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcGpioReSyncGet)(adi_adrv904x_Device_t * const device, const uint32_t rxChannelMask, adi_adrv904x_GpioPinSel_e * const gpioSelection, uint8_t * const enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcDualBandActiveExternalLnaGainWordGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxChannels_e rxChannel, uint8_t * const bandAExternalLnaGainWord, uint8_t * const bandBExternalLnaGainWord);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcUpperLevelBlockerGet)(adi_adrv904x_Device_t * const device, uint8_t * const agcULBlockerBitMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcLowerLevelBlockerGet)(adi_adrv904x_Device_t * const device, uint8_t * const agcLLBlockerBitMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcHighThresholdPeakDetectorGet)(adi_adrv904x_Device_t * const device, uint8_t * const thresholdPeakDetectorBitMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AgcLowThresholdPeakDetectorGet)(adi_adrv904x_Device_t * const device, uint8_t * const thresholdPeakDetectorBitMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_InitCalsRun)(adi_adrv904x_Device_t* const device, const adi_adrv904x_InitCals_t* const initCals);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_InitCalsDetailedStatusGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_InitCalStatus_t* const initStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_InitCalsDetailedStatusGet_v2)( adi_adrv904x_Device_t* const device, adi_adrv904x_InitCalErrData_t* const initCalErrData);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_InitCalsWait)( adi_adrv904x_Device_t* const device, const uint32_t timeoutMs);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_InitCalsWait_v2)( adi_adrv904x_Device_t* const device, const uint32_t timeoutMs, adi_adrv904x_InitCalErrData_t* const initCalErrData);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_InitCalsCheckCompleteGet)(adi_adrv904x_Device_t* const device, uint8_t* const areCalsRunning, const uint8_t calErrorCheck);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_InitCalsCheckCompleteGet_v2)( adi_adrv904x_Device_t* const device, uint8_t* const areCalsRunning, adi_adrv904x_InitCalErrData_t* const initCalErrData);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_InitCalsAbort)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TrackingCalsEnableSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TrackingCalibrationMask_t calMask, const uint32_t channelMask, const adi_adrv904x_TrackingCalEnableDisable_e enableDisableFlag);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TrackingCalsEnableSet_v2)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TrackingCalibrationMask_e calMask, const adi_adrv904x_ChannelTrackingCals_t* const channelMask, const adi_adrv904x_TrackingCalEnableDisable_e enableDisableFlag);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TrackingCalsEnableGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_TrackingCalEnableMasks_t* const enableMasks);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TrackingCalAllStateGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_TrackingCalState_t* const calState);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TrackingCalStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TrackingCalibrationMask_e calId, const adi_adrv904x_Channels_e channel, adi_adrv904x_CalStatus_t* const calStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_InitCalStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_InitCalibrations_e calId, const adi_adrv904x_Channels_e channel, adi_adrv904x_CalStatus_t* const calStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxHrmDataGet)(adi_adrv904x_Device_t* const device, const uint8_t channelMask, adi_adrv904x_TxHrmData_t txHrmDataArray[], uint32_t arrayLength);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxHrmDataSet)(adi_adrv904x_Device_t* const device, const uint8_t channelMask, const adi_adrv904x_TxHrmData_t txHrmDataArray[], uint32_t arrayLength);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CalPvtStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Channels_e channel, const uint32_t objId, uint8_t calStatusGet[], uint32_t length);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CalSpecificStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Channels_e channel, const uint32_t objId, uint8_t calStatusGet[], uint32_t length);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DigDcOffsetEnableSet)(adi_adrv904x_Device_t* const device, const uint32_t rxChannelMask, const uint32_t rxChannelEnableDisable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DigDcOffsetEnableGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxChannels_e rxChannel, uint8_t* const isEnabled);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxLolReset)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxLolReset_t* const txLolReset);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxQecReset)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxQecReset_t* const txQecReset);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DigDcOffsetCfgSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CpuCmd_SetDcOffset_t* const dcOffSetCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DigDcOffsetCfgGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_CpuCmd_GetDcOffsetResp_t* const dcOffSetCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Lock)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Unlock)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_HwOpen)(adi_adrv904x_Device_t* const device, const adi_adrv904x_SpiConfigSettings_t* const spiSettings);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_HwClose)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_HwReset)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Initialize)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Init_t* const init);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Shutdown)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_MultichipSyncSet)(adi_adrv904x_Device_t* const device, const uint8_t enableSync);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_MultichipSyncSet_v2)(adi_adrv904x_Device_t* const device, const adi_adrv904x_McsSyncMode_e mcsMode);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_MultichipSyncStatusGet)(adi_adrv904x_Device_t* const device, uint32_t* const mcsStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_ProfilesVerify)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Init_t* const init);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SpiCfgSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_SpiConfigSettings_t* const spiCtrlSettings);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SpiCfgGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_SpiConfigSettings_t* const spi);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AuxSpiCfgSet)( adi_adrv904x_Device_t* const device, const adi_adrv904x_AuxSpiConfig_t* const config);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AuxSpiCfgGet)( adi_adrv904x_Device_t* const device, adi_adrv904x_AuxSpiConfig_t* const config);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SpiRuntimeOptionsSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_SpiOptions_t* const spiOptions);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SpiRuntimeOptionsGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_SpiOptions_t* const spiOptions);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SpiVerify)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_ApiVersionGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_Version_t* const apiVersion);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeviceRevGet)(adi_adrv904x_Device_t* const device, uint8_t* const siRevision);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_ProductIdGet)(adi_adrv904x_Device_t* const device, uint8_t* const productId);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeviceCapabilityGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_CapabilityModel_t* const devCapability);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SpiDoutPadDriveStrengthSet)(adi_adrv904x_Device_t* const device, const uint8_t driveStrength);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SpiHysteresisSet)( adi_adrv904x_Device_t* const device, const uint32_t enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DigitalHysteresisSet)( adi_adrv904x_Device_t* const device, const uint32_t enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuImageWrite)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CpuType_e cpuType, const uint32_t byteOffset, const uint8_t binary[], const uint32_t byteCount);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuProfileWrite)(adi_adrv904x_Device_t * const device, const uint32_t byteOffset, const uint8_t binary[], const uint32_t byteCount);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuStart)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuStartStatusCheck)(adi_adrv904x_Device_t* const device, const uint32_t timeout_us);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuConfigSet)( adi_adrv904x_Device_t* const device, const uint32_t objId, const uint16_t offset, const uint8_t configDataSet[], const uint32_t length);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuConfigGet)( adi_adrv904x_Device_t* const device, const uint32_t objId, const uint16_t offset, uint8_t configDataGet[], const uint32_t length);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuDebugModeEnable)(adi_adrv904x_Device_t* const device, const uint32_t enableKey);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuConfigUnlock)(adi_adrv904x_Device_t* const device, const uint32_t configKey);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuControlCmdExec)(adi_adrv904x_Device_t* const device, const uint32_t objId, const uint16_t cpuCmd, const adi_adrv904x_Channels_e channel, const uint8_t cpuCtrlData[], const uint32_t lengthSet, uint32_t* const lengthResp, uint8_t ctrlResp[], const uint32_t lengthGet);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuLogFilterSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CpuLogEvent_e eventFilter, const adi_adrv904x_CpuLogCpuId_e cpuIdFilter, const adi_adrv904x_CpuLogObjIdFilter_t* const objIdFilter);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuDebugCmdExec)( adi_adrv904x_Device_t* const device, const uint32_t objId, const uint16_t cpuCmd, const adi_adrv904x_Channels_e channel, const uint8_t cpuDebugData[], const uint32_t lengthSet, uint32_t* const lengthResp, uint8_t debugResp[], const uint32_t lengthGet);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuChannelMappingGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_CpuType_e cpuTypes[], const uint8_t numSerdesLanes);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuSysPvtStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Channels_e channel, const uint32_t objId, uint8_t cpuSysStatusGet[], const uint32_t length);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuSysStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Channels_e channel, const uint32_t objId, adi_adrv904x_CpuSysStatus_t* const cpuSysStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuFwVersionGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_CpuFwVersion_t* const cpuFwVersion);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_HealthMonitorCpuStatusGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_HealthMonitorCpuStatus_t* const healthMonitorStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuCheckException)(adi_adrv904x_Device_t* const device, uint32_t* const isException);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_BreakpointSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_SwBreakPointEntry_t * breakPointCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_BreakpointGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_SwBreakPointEntry_t * const breakPointCfgRead);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_BreakpointHitRead)(adi_adrv904x_Device_t* const device, adi_adrv904x_SwBreakPointEntry_t * const cpu0BreakpointHit, adi_adrv904x_SwBreakPointEntry_t * const cpu1BreakpointHit);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_BreakpointGpioCfgSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_GpioPinSel_e gpioOutputForBreakpointHit, const adi_adrv904x_GpioPinSel_e gpioInputToResumeSleepingTasks);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_BreakpointGpioCfgGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_GpioPinSel_e * const gpioOutputForBreakpointHit, adi_adrv904x_GpioPinSel_e * const gpioInputToResumeSleepingTasks);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_BreakpointResume)(adi_adrv904x_Device_t* const device, const adi_adrv904x_SwBreakPointEntry_t * breakpointToResume, uint8_t resumeAll);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_BreakpointResumeFromHalt)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_BreakpointGlobalHaltMaskSet)(adi_adrv904x_Device_t* const device, const uint32_t globalHaltMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_BreakpointGlobalHaltMaskGet)(adi_adrv904x_Device_t* const device, uint32_t * const globalHaltMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxOrxDataCaptureStart)( adi_adrv904x_Device_t* const device, const adi_adrv904x_RxChannels_e channelSelect, const adi_adrv904x_RxOrxDataCaptureLocation_e captureLocation, uint32_t captureData[], const uint32_t captureLength, const uint8_t trigger, const uint32_t timeout_us);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AdcSampleXbarSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, const adi_adrv904x_AdcSampleXbarCfg_t* const adcXbar);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_AdcSampleXbarGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, adi_adrv904x_AdcSampleXbarCfg_t* const adcXbar);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DacSampleXbarSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, const adi_adrv904x_DacSampleXbarCfg_t* const dacXbar);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DacSampleXbarGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, adi_adrv904x_DacSampleXbarCfg_t* const dacXbar);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerCfgGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, adi_adrv904x_FramerCfg_t* const framerCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerCfgGet_v2)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, const adi_adrv904x_RxChannels_e chanSel, adi_adrv904x_FramerCfg_t* const framerCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerCfgGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_DeframerSel_e deframerSel, adi_adrv904x_DeframerCfg_t* const deframerCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerCfgGet_v2)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, const adi_adrv904x_TxChannels_e chanSel, adi_adrv904x_DeframerCfg_t* const deframerCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerCfgGetScaled)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, const adi_adrv904x_TxChannels_e chanSel, adi_adrv904x_DeframerCfg_t* const deframerCfg, const uint8_t bypass);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerLinkStateGet)(adi_adrv904x_Device_t* const device, uint8_t* const framerLinkState);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerLinkStateSet)(adi_adrv904x_Device_t* device, const uint8_t framerSelMask, uint8_t const enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmPrbsCountReset)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerLinkStateGet)(adi_adrv904x_Device_t* device, uint8_t* const deframerLinkState);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerLinkStateSet)(adi_adrv904x_Device_t* device, const uint8_t deframerSelMask, uint8_t const enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmPrbsCheckerStateSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DfrmPrbsCfg_t * const dfrmPrbsCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmPrbsCheckerStateGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_DfrmPrbsCfg_t * const dfrmPrbsCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerSysrefCtrlSet)(adi_adrv904x_Device_t* const device, const uint8_t framerSelMask, uint8_t const enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerSysrefCtrlGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, uint8_t * const enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerSysrefCtrlSet)(adi_adrv904x_Device_t* const device, const uint8_t deframerSelMask, uint8_t const enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerSysrefCtrlGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, uint8_t * const enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerTestDataSet)(adi_adrv904x_Device_t* const device, adi_adrv904x_FrmTestDataCfg_t * const frmTestDataCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerTestDataGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, adi_adrv904x_FrmTestDataCfg_t * const frmTestDataCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmPrbsErrCountGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_DfrmPrbsErrCounters_t * const counters);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SerializerReset)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SerializerReset_v2)(adi_adrv904x_Device_t* const device, adi_adrv904x_CpuCmd_SerReset_t* const pSerResetParms, adi_adrv904x_CpuCmd_SerResetResp_t* const pSerResetResp);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerLmfcOffsetSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSelect, const uint16_t lmfcOffset);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerLmfcOffsetGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSelect, uint16_t * const lmfcOffset);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmLmfcOffsetSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSelect, const uint16_t lmfcOffset);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmLmfcOffsetGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSelect, uint16_t * const lmfcOffset);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmPhaseDiffGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSelect, uint16_t * const phaseDiff);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerStatusGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_FramerSel_e framerSel, adi_adrv904x_FramerStatus_t * const framerStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerStatusGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_DeframerSel_e deframerSel, adi_adrv904x_DeframerStatus_t * const deframerStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerStatusGet_v2)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, adi_adrv904x_DeframerStatus_v2_t * const deframerStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmErrCounterStatusGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_DeframerSel_e deframerSel, const uint8_t laneNumber, adi_adrv904x_DfrmErrCounterStatus_t * const errCounterStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmErrCounterReset)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, const uint8_t laneNumber, uint32_t const errCounterMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmErrCounterThresholdSet)(adi_adrv904x_Device_t* const device, const uint8_t threshold);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Dfrm204cErrCounterStatusGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_DeframerSel_e deframerSel, const uint8_t laneNumber, adi_adrv904x_Dfrm204cErrCounterStatus_t * const errCounterStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Dfrm204cErrCounterReset)(adi_adrv904x_Device_t * const device, const adi_adrv904x_DeframerSel_e deframerSel);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmLinkConditionGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_DeframerSel_e deframerSel, uint8_t * const dfrmLinkCondition);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmFifoDepthGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, const uint8_t laneNumber, uint8_t * const fifoDepth);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmCoreBufDepthGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, uint8_t * const coreBufDepth);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmIlasMismatchGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, adi_adrv904x_DfrmCompareData_t* const dfrmData);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmIlasMismatchGet_v2)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, adi_adrv904x_DfrmCompareData_v2_t* const dfrmData);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerLoopbackSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerLoopbackDisable)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, const adi_adrv904x_AdcSampleXbarCfg_t* const adcXbar);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerLoopbackSet)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerLoopbackDisable)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerLoopbackDisable_v2)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, const adi_adrv904x_AdcSampleXbarCfg_t* const adcXbar);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerLaneLoopbackSet)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerLaneLoopbackDisable)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerSyncbModeSet)(adi_adrv904x_Device_t* const device, const uint8_t framerSelMask, const uint8_t syncbMode);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerSyncbModeGet)(adi_adrv904x_Device_t* const device, const uint8_t framerSelMask, uint8_t* const syncbMode);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerSyncbStatusSet)(adi_adrv904x_Device_t* const device, const uint8_t framerSelMask, const uint8_t syncbStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerSyncbStatusGet)(adi_adrv904x_Device_t* const device, const uint8_t framerSelMask, uint8_t* const syncbStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerSyncbErrCntGet)(adi_adrv904x_Device_t* const device, const uint8_t framerSelMask, uint8_t* const syncbErrCnt);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerSyncbErrCntReset)(adi_adrv904x_Device_t* const device, const uint8_t framerSelMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerSyncbErrCntGet)(adi_adrv904x_Device_t* const device, const uint8_t deframerSelMask, uint8_t* const syncbErrCnt);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerSyncbErrCntReset)(adi_adrv904x_Device_t* const device, const uint8_t deframerSelMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerErrorCtrl)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, const adi_adrv904x_SerdesErrAction_e action);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeframerErrorCtrl)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, const adi_adrv904x_SerdesErrAction_e action);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmIrqMaskGet)( adi_adrv904x_Device_t* const device, uint16_t* const irqMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmIrqMaskSet)( adi_adrv904x_Device_t* const device, const uint16_t irqMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmIrqSourceReset)( adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmIrqSourceGet)( adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSelect, adi_adrv904x_DeframerIrqVector_t* const irqSourceVector);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmErrCntrCntrlSet)( adi_adrv904x_Device_t* const device, const adi_adrv904x_DfrmErrCounterIrqSel_e interruptEnable, const uint8_t laneNumber, const uint8_t errCounterControl, const uint8_t errCounterHoldCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfrmErrCntrCntrlGet)( adi_adrv904x_Device_t* const device, const uint8_t laneNumber, uint8_t* const errCounterControl, uint8_t* const errCounterHoldCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RunEyeSweep)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CpuCmd_RunEyeSweep_t* const runEyeSweep, adi_adrv904x_CpuCmd_RunEyeSweepResp_t* const runEyeSweepResp);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RunEyeSweep_v2)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CpuCmd_RunEyeSweep_t* const runEyeSweep, adi_adrv904x_CpuCmd_RunEyeSweepResp_t* const runEyeSweepResp);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RunVerticalEyeSweep)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CpuCmd_RunVertEyeSweep_t* const runVerticalEyeSweep, adi_adrv904x_CpuCmd_RunVertEyeSweepResp_t* const runVerticalEyeSweepResp);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RunVerticalEyeSweep_v2)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CpuCmd_RunVertEyeSweep_t* const runVerticalEyeSweep, adi_adrv904x_CpuCmd_RunVertEyeSweepResp_t* const runVerticalEyeSweepResp);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_FramerTestDataInjectError)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSelect, const uint8_t laneMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SerLaneCfgSet)(adi_adrv904x_Device_t* const device, const uint8_t laneNumber, const adi_adrv904x_SerLaneCfg_t* const serLaneCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SerLaneCfgGet)(adi_adrv904x_Device_t* const device, const uint8_t laneNumber, adi_adrv904x_SerLaneCfg_t* const serLaneCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SerdesInitCalStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_GenericStrBuf_t* const filePath, const uint8_t laneNumber, const adi_adrv904x_GenericStrBuf_t* const msg, adi_adrv904x_SerdesInitCalStatus_t* const calStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SerdesTrackingCalStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_GenericStrBuf_t* const filePath, const uint8_t laneNumber, const adi_adrv904x_GenericStrBuf_t* const msg, adi_adrv904x_SerdesTrackingCalStatus_t* calStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CddcModeSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, const adi_adrv904x_CddcMode_e cddcMode);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CddcModeGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, adi_adrv904x_CddcMode_e * const cddcMode);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CducModeSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, const adi_adrv904x_CducMode_e cducMode);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CducModeGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, adi_adrv904x_CducMode_e * const cducMode);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CddcSampleXBarSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, const adi_adrv904x_RxChannels_e rxChanSel, const adi_adrv904x_CddcSampleXbarCfg_t * const cddcSampleXbarCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CddcSampleXBarGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, const adi_adrv904x_RxChannels_e rxChanSel, adi_adrv904x_CddcSampleXbarCfg_t * const cddcSampleXbarCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CducSampleXBarSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, const adi_adrv904x_TxChannels_e txChanSel, const adi_adrv904x_CducSampleXbarCfg_t * const cducSampleXbarCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CducSampleXBarGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, const adi_adrv904x_TxChannels_e txChanSel, adi_adrv904x_CducSampleXbarCfg_t * const cducSampleXbarCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CddcRxChanXBarSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, const adi_adrv904x_CddcChannelXbarCfg_t * const cddcChannelXbarCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CddcRxChanXBarGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, adi_adrv904x_CddcChannelXbarCfg_t * const cddcChannelXbarCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CducTxChanXBarSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, const adi_adrv904x_CducChannelXbarCfg_t * const cducChannelXbarCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CducTxChanXBarGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DeframerSel_e deframerSel, adi_adrv904x_CducChannelXbarCfg_t * const cducChannelXbarCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_OrxAdcSampleXbarSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, const adi_adrv904x_OrxAdcSampleXbarCfg_t* const orxAdcXbar);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_OrxAdcSampleXbarGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_FramerSel_e framerSel, adi_adrv904x_OrxAdcSampleXbarCfg_t* const orxAdcXbar);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrThresholdSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CfrThrConfig_t* const cfrThrConfig, const uint32_t cfrTxChannelMask, const uint32_t configSelectMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrThresholdGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_CfrThrConfig_t* const cfrThrConfig, const uint32_t cfrTxChannelSel, const uint32_t configSelect);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrEnableSet)(adi_adrv904x_Device_t* const device, const uint32_t cfrEngineEnable[], const uint32_t cfrEngineBypass[], const uint32_t arraySize, const uint32_t cfrTxChannelMask, const uint32_t configSelectMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrEnableGet)(adi_adrv904x_Device_t* const device, uint32_t cfrEngineEnable[], uint32_t cfrEngineBypass[], const uint32_t arraySize, const uint32_t cfrTxChannelSel, const uint32_t configSelect);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrConfigSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CfrConfig_t* const cfrCtrlConfig, const uint32_t cfrTxChannelMask, const uint32_t configSelectMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrConfigGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_CfrConfig_t* const cfrCtrlConfig, const uint32_t cfrTxChannelSel, const uint32_t configSelect);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrActiveCfgSet)(adi_adrv904x_Device_t* const device, const uint32_t cfrTxChannelMask, const adi_adrv904x_CfrConfigSel_e configSelect, const uint8_t immediateUpdate);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrActiveCfgGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannelSel, adi_adrv904x_CfrConfigSel_e* const configSelectRead);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrActivePulseSet)(adi_adrv904x_Device_t* const device, const uint32_t cfrTxChannelMask, const adi_adrv904x_CfrPulseSel_e pulseSelect, const uint8_t immediateUpdate);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrActivePulseGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannelSel, adi_adrv904x_CfrPulseSel_e* const pulseSelectRead);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrStatisticsCfgSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CfrStatisticsCfg_t* const cfrStatisticsConfig, const uint32_t cfrTxChannelMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrStatisticsCfgGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_CfrStatisticsCfg_t* const cfrStatisticsConfig, const uint32_t cfrTxChannelSel);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrStatisticsGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_CfrStatistics_t* const cfrStatistics, const adi_adrv904x_TxChannels_e cfrTxChannelSel, const uint8_t prevWindow, const uint8_t clearStats);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrCorrectionPulseWrite)(adi_adrv904x_Device_t* const device, const uint32_t cfrTxChannelMask, const adi_adrv904x_CfrCorrectionPulse_t cfrCorrectionPulses[], const uint32_t numCorrectionPulses);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrCorrectionPulseRead)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e cfrTxChannelSel, adi_adrv904x_CfrCorrectionPulse_t* const cfrCorrectionPulse, const adi_adrv904x_CfrPulseSel_e pulseSelect);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrControlPeakDurationSet)(adi_adrv904x_Device_t* const device, const uint32_t cfrTxChannelMask, const uint32_t configSelectMask, const uint8_t samplesCount);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrControlPeakDurationGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannelSel, const adi_adrv904x_CfrConfigSel_e configSelect, uint8_t* const samplesCount);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrControlDelaySet)(adi_adrv904x_Device_t* const device, const uint32_t cfrTxChannelMask, const uint16_t delay0, const uint16_t delay1, const uint16_t delay2);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrControlDelayGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannelSel, uint16_t* const delay0, uint16_t* const delay1, uint16_t* const delay2);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_ClgcCaptureConfigSet)(adi_adrv904x_Device_t* const device, const uint32_t clgcTxChannelMask, const adi_adrv904x_DfeAppCalClgcCaptureCfg_t* const clgcCaptureCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_ClgcCaptureConfigGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e clgcTxChannelSel, adi_adrv904x_DfeAppCalClgcCaptureCfg_t* const clgcCaptureCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_ClgcTrackingConfigSet)(adi_adrv904x_Device_t* const device, const uint32_t clgcTxChannelMask, const adi_adrv904x_DfeAppCalClgcTrackCfg_t* const clgcTrackCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_ClgcTrackingConfigGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e clgcTxChannelSel, adi_adrv904x_DfeAppCalClgcTrackCfg_t* const clgcTrackCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_ClgcTrackingRun)(adi_adrv904x_Device_t* const device, const uint32_t clgcTxChannelMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_ClgcTrackingReset)(adi_adrv904x_Device_t* const device, const uint32_t clgcTxChannelMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeClgcStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e clgcTxChannelSel, adi_adrv904x_DfeAppCalClgcStatus_t* const clgcStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeClgcCaptureBuffersAccessSet)(adi_adrv904x_Device_t* const device, const uint32_t clgcTxChannelMask, adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessSet_t* const clgcCapBufAccess);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeClgcCaptureBuffersAccessGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e clgcTxChannelSel, adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessGet_t* const clgcCapBufAccess);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_ClgcCaptureConfigSet_int)(adi_adrv904x_Device_t* const device, const uint32_t clgcTxChannelMask, const adi_adrv904x_DfeAppCalClgcCaptureCfgInt_t* const clgcCaptureCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_ClgcCaptureConfigGet_int)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e clgcTxChannelSel, adi_adrv904x_DfeAppCalClgcCaptureCfgInt_t* const clgcCaptureCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_ClgcTrackingConfigSet_int)(adi_adrv904x_Device_t* const device, const uint32_t clgcTxChannelMask, const adi_adrv904x_DfeAppCalClgcTrackCfgInt_t* const clgcTrackCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_ClgcTrackingConfigGet_int)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e clgcTxChannelSel, adi_adrv904x_DfeAppCalClgcTrackCfgInt_t* const clgcTrackCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeClgcStatusGet_int)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e clgcTxChannelSel, adi_adrv904x_DfeAppCalClgcStatusInt_t* const clgcStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeLoadGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_DfeCpuLoad_t* const pDfeCpuLoad);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCpuAndMemLoadGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_DfeCpuAndMemLoad_t* const pDfeCpuLoad);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCpuStart)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCpuStartStatusCheck)(adi_adrv904x_Device_t* const device, const uint32_t timeout_us);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCpuImageLoad)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DfeCpuBinaryInfo_t* const cpuDfeBinaryInfo);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePrepareLoad)(adi_adrv904x_Device_t* const device, const uint8_t reload);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCpuReload)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DfeCpuBinaryInfo_t* const cpuDfeBinaryInfo);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCpuCheckException)(adi_adrv904x_Device_t* const device, uint32_t* const isException);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeAppCommand)(adi_adrv904x_Device_t* const device, const uint8_t command, const uint8_t payload[], const uint32_t payload_length, const uint8_t response[], const uint32_t response_length);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCaptureSequencerConfigSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DfeAppCaptureSequencerCfg_t* const capSeqCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCaptureSequencerConfigGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_DfeAppCaptureSequencerCfg_t* const capSeqCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeTrackingCalsEnableSet)(adi_adrv904x_Device_t* const device, const uint32_t calMask, const uint32_t channelMask, const adi_adrv904x_DfeAppTrackingCalEnableDisable_t enableDisableFlag);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeTrackingCalsEnableGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_DfeAppFrameworkTrackingCalEnableMasks_t* const enableMasks);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeTrackingCalTimerSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DfeAppFrameworkTrackingCalType_t calType, const uint32_t calTimerValue);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeTrackingCalTimerGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DfeAppFrameworkTrackingCalType_t calType, uint32_t* const calTimerValue);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeSvcConfigSet)(adi_adrv904x_Device_t* const device, const uint8_t configID, const uint32_t offset, const uint8_t buffer[], const uint32_t size);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeSvcConfigGet)(adi_adrv904x_Device_t* const device, const uint8_t configID, const uint32_t offset, const uint8_t buffer[], const uint32_t size);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeAppConfigGet)(adi_adrv904x_Device_t* const device, const uint8_t configID, const uint32_t offset, const uint8_t buffer[], const uint32_t size);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeAppConfigSet)(adi_adrv904x_Device_t* const device, const uint8_t configID, const uint32_t offset, const uint8_t buffer[], const uint32_t size);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeConfigUnlock)(adi_adrv904x_Device_t* const device, const uint32_t cfgKey);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeAppControlCmdExec)(adi_adrv904x_Device_t* const device, const uint32_t objId, const uint16_t cpuCmd, const adi_adrv904x_Channels_e channel, const uint8_t cpuCtrlData[], const uint32_t lengthSet, uint32_t* const lengthResp, uint8_t ctrlResp[], const uint32_t lengthGet);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeSysStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Channels_e channel, const uint32_t objID, adi_adrv904x_CpuSysStatus_t* const status);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeTrackingCalAllStateGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_DfeAppFrameworkTrackingCalState_t* const calState);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeTrackingCalStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DfeAppFrameworkTrackingCalibrationMask_e calId, const adi_adrv904x_Channels_e channel, adi_adrv904x_CalStatus_t* const calStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCalPvtStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Channels_e channel, const uint32_t objId, uint8_t calStatusGet[], const uint32_t length);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCalSpecificStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Channels_e channel, const uint32_t objId, uint8_t calStatusGet[], const uint32_t length);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCalExtendedStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Channels_e channel, const uint32_t objId, uint8_t calStatusGet[], const uint32_t length);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeAppInitStatusGet)(adi_adrv904x_Device_t* const device, uint32_t* const dfeAppInitStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeBootErrorGet)( adi_adrv904x_Device_t* const device, uint32_t* const dfeBootError);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeSysPvtStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Channels_e channel, const uint32_t objID, uint8_t cpuSysStatusGet[], const uint32_t length);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeFwDebugModeSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DfeFwDebugMode_e dfeFwDebugMode);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeDebugMode)(adi_adrv904x_Device_t* const device, const uint32_t debugModeKey);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeDebugModeCmd)(adi_adrv904x_Device_t* const device, const uint32_t objId, const uint16_t cpuCmd, const uint8_t cpuDebugData[], const uint32_t lengthSet, uint32_t* const lengthResp, uint8_t debugResp[], const uint32_t lengthGet);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeWatchdogConfigSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DfeSvcWatchdogInstance_t wdt, const uint32_t timeout_ms);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeUpdateEcc)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCpuLogMaskSet)( adi_adrv904x_Device_t* const device, const uint32_t logMaskSet);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCpuLogMaskGet)( adi_adrv904x_Device_t* const device, uint32_t* const logMaskGet);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCpuTelemetryMaskSet)( adi_adrv904x_Device_t* const device, const uint64_t sdkTelemetryMask0, const uint64_t sdkTelemetryMask1, const uint64_t appTelemetryMask0, const uint64_t appTelemetryMask1);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCpuTelemetryMaskGet)( adi_adrv904x_Device_t* const device, uint64_t * const sdkTelemetryMask0, uint64_t * const sdkTelemetryMask1, uint64_t * const appTelemetryMask0, uint64_t * const appTelemetryMask1);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCpuPintSwStatusGet)(adi_adrv904x_Device_t* const device, uint8_t* const statusPintSw0, uint8_t* const statusPintSw1);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCpuPintSwStatusClear)(adi_adrv904x_Device_t* const device, const uint8_t clearPintSw0, const uint8_t clearPintSw1);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCpuPintSw0DetailedInfoGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DfeCpuPintSw0Status_e interruptSelect, uint32_t* const pInfo);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCpuPintSw1DetailedInfoGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DfeCpuPintSw1Status_e interruptSelect, uint32_t* const pInfo);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeSvcLogTraceErrLogGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_DfeSvcLogTraceErrLog_t* const lastErrorData);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdModelConfigDpdSet)(adi_adrv904x_Device_t* const device, const uint32_t dpdTxChannelMask, const adi_adrv904x_DfeAppCalDpdModelType_e dpdModelType, const adi_adrv904x_DfeAppCalDpdModelDesc_t* const modelDesc);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdModelConfigDpdGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e dpdTxChannelSel, const adi_adrv904x_DfeAppCalDpdModelType_e dpdModelType, adi_adrv904x_DfeAppCalDpdModelDesc_t* const modelDesc);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdModelConfigDpdSet_int)(adi_adrv904x_Device_t* const device, const uint32_t dpdTxChannelMask, const adi_adrv904x_DfeAppCalDpdModelType_e dpdModelType, const adi_adrv904x_DfeAppCalDpdModelDescInt_t* const modelDesc);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdModelConfigDpdGet_int)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e dpdTxChannelSel, const adi_adrv904x_DfeAppCalDpdModelType_e dpdModelType, adi_adrv904x_DfeAppCalDpdModelDescInt_t* const modelDesc);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdReset)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const adi_adrv904x_DfeAppDpdResetMode_e dpdResetMode);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Ctc2EnableSet)(adi_adrv904x_Device_t* const device, const uint32_t dpdTxChannelMask, const uint8_t enableCtc2);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Ctc2EnableGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e dpdTxChannelSel, uint32_t* const ctc2Enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdActuatorGainMonitorConfigSet)(adi_adrv904x_Device_t* const device, const uint32_t dpdTxChannelMask, const adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t* const dpdPowerMeterCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdActuatorGainMonitorConfigGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e dpdTxChannelSel, adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t* const dpdPowerMeterCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdCaptureConfigSet)(adi_adrv904x_Device_t* const device, const uint32_t dpdTxChannelMask, const adi_adrv904x_DfeAppCalDpdCaptureCfg_t* const dpdCaptureCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdCaptureConfigGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e dpdTxChannelSel, adi_adrv904x_DfeAppCalDpdCaptureCfg_t* const dpdCaptureCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_WBBufSegConfigSet)( adi_adrv904x_Device_t* const device, const adi_adrv904x_DfeAppCalDpdWbRegBufSeg_e wbBufNum);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_WBBufSegConfigGet)( adi_adrv904x_Device_t* const device, adi_adrv904x_DfeAppCalDpdWbRegBufSeg_e* wbBufNum);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdTrackingConfigSet)(adi_adrv904x_Device_t* const device, const uint32_t dpdTxChannelMask, const adi_adrv904x_DfeAppCalDpdTrackCfg_t* const dpdTrackCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdTrackingConfigGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e dpdTxChannelSel, adi_adrv904x_DfeAppCalDpdTrackCfg_t* const dpdTrackCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdTrackingConfigSet_int)(adi_adrv904x_Device_t* const device, const uint32_t dpdTxChannelMask, const adi_adrv904x_DfeAppCalDpdTrackCfgInt_t* const dpdTrackCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdTrackingConfigGet_int)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e dpdTxChannelSel, adi_adrv904x_DfeAppCalDpdTrackCfgInt_t* const dpdTrackCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdPowerMeterConfigSet)(adi_adrv904x_Device_t* const device, const uint32_t dpdTxChannelMask, const adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t* const dpdPowerMeterCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdPowerMeterConfigGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e dpdTxChannelSel, adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t* const dpdPowerMeterCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdCaptureBuffersAccessSet)(adi_adrv904x_Device_t* const device, const uint32_t dpdTxChannelMask, adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessSet_t* const dpdCapBufAccess);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdCaptureBuffersAccessGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e dpdTxChannelSel, adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessGet_t* const dpdCapBufAccess);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeDpdActSatStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Channels_e channel, adi_adrv904x_DfeSvcDfeActuatorSaturation_t* const calDpdActSatStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeDpdActiveModelTableGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Channels_e channel, uint8_t* const calDpdActiveModelTable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeDpdCaptureDataFromBufMemGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Channels_e orxChannelSel, adi_adrv904x_DfeAppDpdCaptureData_t* const dpdCaptureData);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeDpdCalSpecificStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Channels_e channel, adi_adrv904x_DfeAppCalDpdStatus_t* const calDpdSpecStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeDpdCalSpecificStatusGet_int)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Channels_e channel, adi_adrv904x_DfeAppCalDpdStatusInt_t* const calDpdSpecStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdStabilityCfgSet)(adi_adrv904x_Device_t* const device, const uint32_t dpdTxChannelMask, adi_adrv904x_DfeAppCalDpdStabilityCfg_t* const dpdStabilityCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdStabilityCfgGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e dpdTxChannelSel, adi_adrv904x_DfeAppCalDpdStabilityCfg_t* const dpdStabilityCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdTddLutSwitchCfgSet)(adi_adrv904x_Device_t* const device, const uint32_t dfeTxChannelMask, const adi_adrv904x_DfeAppDpdActTddLutSwitch_t* const tddLutSwitchConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdTddLutSwitchCfgGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e dpdTxChannelSel, adi_adrv904x_DfeAppDpdActTddLutSwitch_t* const tddLutSwitchConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_WbRegBufferAccess)(adi_adrv904x_Device_t* const device, adi_adrv904x_DfeAppwbRegularization_t* const DfeAppWbReg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_WbRegBufferWrite)(adi_adrv904x_Device_t* const device, adi_adrv904x_DfeAppwbRegData_t* const DfeAppWbRegData);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdPathdelayCfg)(adi_adrv904x_Device_t* const device, const uint32_t dpdTxChannelMask, const adi_adrv904x_DfeAppCalDpdPathDlyStatus_t* const dpdPathdelayCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdResetPathdelayInit)(adi_adrv904x_Device_t* const device, const uint32_t dpdTxChannelMask, const adi_adrv904x_DfeAppDpdResetMode_e dpdResetMode, const adi_adrv904x_DfeAppCalDpdPathDlyStatus_t* const dpdPathdelayCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdPathdelaySeed)(adi_adrv904x_Device_t* const device, const uint32_t dpdTxChannelMask, const adi_adrv904x_DfeAppCalDpdPathDlySeed_t* const dpdPathdelay);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdPathdelayResetSeed)(adi_adrv904x_Device_t* const device, const uint32_t dpdTxChannelMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Ctc2ModelConfigSet)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const adi_adrv904x_DfeAppCalCtc2ModelDesc_t* const modelDesc);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Ctc2ModelConfigSet_v2)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const adi_adrv904x_DfeAppCalCtc2ModelDesc_v2_t* const modelDesc);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Ctc2ModelConfigGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannelSel, adi_adrv904x_DfeAppCalCtc2ModelType_e ctc2ModelType, adi_adrv904x_DfeAppCalCtc2ModelDesc_t* const modelDesc);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Ctc2ModelConfigGet_v2)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannelSel, adi_adrv904x_DfeAppCalCtc2ModelType_e ctc2ModelType, adi_adrv904x_DfeAppCalCtc2ModelDesc_v2_t* const modelDesc);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdGainSet)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const adi_adrv904x_DfeAppCalDpdGainCfg_t* const gainCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DpdGainGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannelSel, adi_adrv904x_DfeAppCalDpdGainCfg_t* const gainCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SbetCfrThresholdSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CfrThrConfig_t* const cfrThrConfig, const uint32_t hardClipperThreshold, const uint32_t cfrTxChannelMask, const uint32_t cfrPowerIndex);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SbetCfrMappingSet)(adi_adrv904x_Device_t* const device, const uint32_t cfrTxChannelMask, const uint32_t mapping);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SbetTxAttenSet)(adi_adrv904x_Device_t* const device, adi_adrv904x_SbetTxAtten_t * const configTx);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SbetTxAttenGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_SbetTxAtten_t* const configTx);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeSdkVersionGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_CpuFwVersion_t* const dfeSdkVersion);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeAppVersionGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_CpuFwVersion_t* const dfeAppVersion);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioSequencerErrorInfoGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_DfeSvcRadioCtrlSequencerErrorInfo_t* const errorInfo);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioSequencerSsbSyncPhaseErrorClear)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeLogTraceBufferGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_DfeLogTraceBuffer_t* const logTrace);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCaptureTypeGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_DfeCaptureType_e* const orx0CaptureType, adi_adrv904x_DfeCaptureType_e* const orx1CaptureType);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_VswrPlaybackDataWrite)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const adi_adrv904x_VswrPlaybackData_t* const playbackData);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_VswrPlaybackDataWrite_v2)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const adi_adrv904x_VswrPlaybackData_t* const playbackData);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_VswrPlaybackDataRead)(adi_adrv904x_Device_t* const device, const uint32_t txChannelSel, adi_adrv904x_VswrPlaybackData_t* const playbackData);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_VswrTrackingConfigSet)(adi_adrv904x_Device_t* const device, const uint32_t vswrTxChannelMask, const adi_adrv904x_DfeAppCalVswrCfg_t* const cfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_VswrTrackingConfigGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e vswrTxChannelSel, adi_adrv904x_DfeAppCalVswrCfg_t* const cfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_VswrStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Channels_e vswrTxChannelSel, adi_adrv904x_DfeAppCalVswrStatus_t* const vswrStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_VswrReset)(adi_adrv904x_Device_t* const device, const uint32_t vswrTxChannelMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_ErrInfoGet)(const adi_adrv904x_ErrSource_e errSrc, const int64_t errCode, const char** const errMsgPtr, const char** const errCausePtr, adi_adrv904x_ErrAction_e* const actionCodePtr, const char** const actionMsgPtr);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_ErrDataGet)(const adi_adrv904x_Device_t* const device, const adi_common_ErrFrameId_e frameId, adi_adrv904x_ErrSource_e* const errSrcPtr, int64_t* const errCodePtr, const char** const errMsgPtr, const char** const errCausePtr, adi_adrv904x_ErrAction_e* const actionCodePtr, const char** const actionMsgPtr);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioForceHiZAllPins)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioForceHiZ)(adi_adrv904x_Device_t* const device, const adi_adrv904x_GpioPinSel_e gpio, const uint8_t oRide);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioStatusRead)( adi_adrv904x_Device_t* const device, adi_adrv904x_GpioStatus_t* const status);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioConfigGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_GpioPinSel_e gpio, adi_adrv904x_GpioSignal_e* const signal, uint32_t* const channelMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioConfigAllGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_GpioSignal_e signalArray[], uint32_t channelMaskArray[], const uint32_t arraySize);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioAnalogConfigGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_GpioAnaPinSel_e gpio, adi_adrv904x_GpioSignal_e* const signal, uint32_t* const channelMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioAnalogConfigAllGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_GpioSignal_e signalArray[], uint32_t channelMaskArray[], const uint32_t arraySize);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioMonitorOutSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_GpioPinSel_e gpio, const adi_adrv904x_GpioSignal_e signal, const uint8_t channel);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioMonitorOutRelease)(adi_adrv904x_Device_t* const device, const adi_adrv904x_GpioPinSel_e gpio);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioManualInputDirSet)( adi_adrv904x_Device_t* const device, const uint32_t gpioInputMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioManualOutputDirSet)( adi_adrv904x_Device_t* const device, const uint32_t gpioOutputMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioManualInputPinLevelGet)( adi_adrv904x_Device_t* const device, uint32_t * const gpioInPinLevel);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioManualOutputPinLevelGet)( adi_adrv904x_Device_t* const device, uint32_t * const gpioOutPinLevel);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioManualOutputPinLevelSet)( adi_adrv904x_Device_t* const device, const uint32_t gpioPinMask, const uint32_t gpioOutPinLevel);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioAnalogForceHiZAllPins)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioAnalogForceHiZ)( adi_adrv904x_Device_t* const device, const adi_adrv904x_GpioAnaPinSel_e gpio, const uint8_t oRide);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioAnalogManualInputDirSet)( adi_adrv904x_Device_t* const device, const uint16_t gpioAnalogInputMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioAnalogManualOutputDirSet)( adi_adrv904x_Device_t* const device, const uint16_t gpioAnalogOutputMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioAnalogManualInputPinLevelGet)( adi_adrv904x_Device_t* const device, uint16_t * const gpioAnalogInPinLevel);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioAnalogManualOutputPinLevelGet)( adi_adrv904x_Device_t* const device, uint16_t * const gpioAnalogOutPinLevel);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioAnalogManualOutputPinLevelSet)( adi_adrv904x_Device_t* const device, const uint16_t gpioAnalogPinMask, const uint16_t gpioAnalogOutPinLevel);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpIntPinMaskCfgSet)( adi_adrv904x_Device_t* const device, const adi_adrv904x_GpIntPinSelect_e pinSelect, const adi_adrv904x_GpIntPinMaskCfg_t* const pinMaskCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpIntPinMaskCfgGet)( adi_adrv904x_Device_t* const device, const adi_adrv904x_GpIntPinSelect_e pinSelect, adi_adrv904x_GpIntPinMaskCfg_t* const pinMaskCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpIntStatusGet)( adi_adrv904x_Device_t* const device, adi_adrv904x_GpIntMask_t* const gpIntStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpIntStatusClear)( adi_adrv904x_Device_t* const device, const adi_adrv904x_GpIntMask_t* const gpIntClear);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpIntStickyBitMaskSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_GpIntMask_t* const gpIntStickyBitMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpIntStickyBitMaskGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_GpIntMask_t* const gpIntStickyBitMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioHysteresisSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_GpioDigitalPin_e pinName, const uint32_t enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioHysteresisGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_GpioDigitalPin_e pinName, uint32_t* const enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioDriveStrengthSet)( adi_adrv904x_Device_t* const device, const adi_adrv904x_GpioDigitalPin_e pinName, const adi_adrv904x_CmosPadDrvStr_e driveStrength);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_GpioDriveStrengthGet)( adi_adrv904x_Device_t* const device, const adi_adrv904x_GpioDigitalPin_e pinName, adi_adrv904x_CmosPadDrvStr_e* const driveStrength);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_SpiFlush)(adi_adrv904x_Device_t* const device, const uint8_t data[], uint32_t* const count);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Registers32Write)(adi_adrv904x_Device_t* const device, adi_adrv904x_SpiCache_t* const spiCache, const uint32_t addr[], const uint32_t writeData[], const uint32_t mask[], const uint32_t count);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Register32Write)(adi_adrv904x_Device_t* const device, adi_adrv904x_SpiCache_t* const spiCache, uint32_t addr, const uint32_t writeData, const uint32_t mask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Register32Read)(adi_adrv904x_Device_t* const device, adi_adrv904x_SpiCache_t* const spiCache, uint32_t addr, uint32_t* const readData, const uint32_t mask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Registers32Read)(adi_adrv904x_Device_t* const device, adi_adrv904x_SpiCache_t* const spiCache, const uint32_t addr, uint32_t readData[], uint32_t mask[], const uint32_t count);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RegistersByteWrite)(adi_adrv904x_Device_t* const device, adi_adrv904x_SpiCache_t* const spiCache, const uint32_t addr, const uint8_t writeData[], const uint32_t count);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RegistersByteRead)(adi_adrv904x_Device_t* const device, adi_adrv904x_SpiCache_t* const spiCache, const uint32_t addr, uint8_t readData[], const uint8_t mask[], const uint32_t count);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Registers32bOnlyRead)(adi_adrv904x_Device_t* const device, adi_adrv904x_SpiCache_t* const spiCache, const uint32_t addr, uint8_t readData[], const uint32_t count);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_Registers32bOnlyWrite)(adi_adrv904x_Device_t* const device, adi_adrv904x_SpiCache_t* const spiCache, const uint32_t addr, const uint8_t writeData[], const uint32_t count);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_StreamImageWrite)(adi_adrv904x_Device_t* const device, uint32_t byteOffset, const uint8_t binary[], uint32_t byteCount);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxTxEnableSet)(adi_adrv904x_Device_t* const device, const uint32_t orxChannelMask, const uint32_t orxChannelEnable, const uint32_t rxChannelMask, const uint32_t rxChannelEnable, const uint32_t txChannelMask, const uint32_t txChannelEnable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxTxEnableGet)(adi_adrv904x_Device_t* const device, uint32_t* const orxChannelMask, uint32_t* const rxChannelMask, uint32_t* const txChannelMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_ChannelEnableGet)(adi_adrv904x_Device_t* const device, uint32_t* const orxChannelMask, uint32_t* const rxChannelMask, uint32_t* const txChannelMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlCfgSet)(adi_adrv904x_Device_t* const device, adi_adrv904x_RadioCtrlModeCfg_t* const radioCtrlCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlTxRxEnCfgSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RadioCtrlTxRxEnCfg_t* const txRxEnCfg, uint8_t pinIndex, uint8_t configSel);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlTxRxEnCfgGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_RadioCtrlTxRxEnCfg_t* const txRxEnCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlCfgGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxChannels_e rxChannel, const adi_adrv904x_TxChannels_e txChannel, adi_adrv904x_RadioCtrlModeCfg_t* const radioCtrlCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_LoFrequencySet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_LoConfig_t* const loConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_LoFrequencyGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_LoConfigReadback_t* const loConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfgPllToChanCtrl)(adi_adrv904x_Device_t* const device, uint8_t rf0MuxTx0_3, uint8_t rf0MuxTx4_7, uint8_t rf0MuxRx0_3, uint8_t rf0MuxRx4_7);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_LoLoopFilterSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_LoName_e loName, const adi_adrv904x_LoLoopFilterCfg_t* const loLoopFilterConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_LoLoopFilterGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_LoName_e loName, adi_adrv904x_LoLoopFilterCfg_t* const loLoopFilterConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_PllStatusGet)(adi_adrv904x_Device_t* const device, uint32_t* const pllLockStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxTxLoFreqGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_RxTxLoFreqReadback_t* const rxTxLoFreq);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TemperatureGet)(adi_adrv904x_Device_t* const device, const uint16_t avgMask, adi_adrv904x_DevTempData_t* const deviceTemperature);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TemperatureEnableGet)(adi_adrv904x_Device_t* const device, uint16_t* const tempEnData);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TemperatureEnableSet)(adi_adrv904x_Device_t* const device, uint16_t* const tempEnData);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_StreamGpioConfigSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_StreamGpioPinCfg_t* const streamGpioPinCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_StreamGpioConfigGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_StreamGpioPinCfg_t* const streamGpioPinCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_OrxNcoFreqCalculate)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxChannels_e orxChannel, const uint32_t txSynthesisBwLower_kHz, const uint32_t txSynthesisBwUpper_kHz, int32_t* const ncoShiftFreqAdc_kHz, int32_t* const ncoShiftFreqDatapath_kHz);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxToOrxMappingInit)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxToOrxMappingConfigGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_TxToOrxMappingConfig_t * const mappingConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxToOrxMappingSet)(adi_adrv904x_Device_t* const device, const uint8_t mapping);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxToOrxMappingGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxChannels_e orxChannel, adi_adrv904x_TxChannels_e* const txChannel);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxToOrxMappingPresetAttenSet)( adi_adrv904x_Device_t* const device, const uint32_t mapping, const uint8_t presetAtten_dB, const uint8_t immediateUpdate);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxToOrxMappingPresetAttenGet_v2)( adi_adrv904x_Device_t* const device, const adi_adrv904x_TxToOrxMappingPinTable_e mapping, uint8_t* const presetAtten_dB);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxToOrxMappingPresetAttenGet)( adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannel, uint8_t* const presetAtten_dB);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxToOrxMappingPresetNcoSet)( adi_adrv904x_Device_t* const device, const uint32_t mapping, const adi_adrv904x_TxToOrxMappingPresetNco_t* const presetNco, const uint8_t immediateUpdate);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxToOrxMappingPresetNcoGet_v2)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxToOrxMappingPinTable_e mapping, adi_adrv904x_TxToOrxMappingPresetNco_t* const presetNco);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxToOrxMappingPresetNcoGet)( adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannel, adi_adrv904x_TxToOrxMappingPresetNco_t* const presetNco);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_StreamVersionGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_Version_t* const streamVersion);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalConfigSet)(adi_adrv904x_Device_t* const device, adi_adrv904x_RxRadioCtrlAntennaCalConfig_t * const configRx, adi_adrv904x_TxRadioCtrlAntennaCalConfig_t * const configTx);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalConfigGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_RxRadioCtrlAntennaCalConfig_t * const configRx, adi_adrv904x_TxRadioCtrlAntennaCalConfig_t * const configTx);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalErrorGet)(adi_adrv904x_Device_t* const device, const uint32_t channelSel, uint8_t * const errStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalErrorClear)(adi_adrv904x_Device_t* const device, const uint32_t channelMask, const uint8_t errClearMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_StreamProcErrorGet)( adi_adrv904x_Device_t* const device, adi_adrv904x_StreamErrArray_t* const streamErr);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalGpioChannelSet)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const uint32_t rxChannelMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalGpioChannelGet)(adi_adrv904x_Device_t* const device, uint32_t * const txChannelMask, uint32_t * const rxChannelMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalConfigSet_v2)(adi_adrv904x_Device_t* const device, adi_adrv904x_RxRadioCtrlAntennaCalConfig_t * const configRx, adi_adrv904x_TxRadioCtrlAntennaCalConfig_t * const configTx, const uint8_t rxGain, const uint8_t enableFreeze);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioSequencerImageWrite)(adi_adrv904x_Device_t* const device, uint32_t byteOffset, const uint8_t binary[], uint32_t byteCount);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioSequencerPhaseSet)(adi_adrv904x_Device_t* const device, uint32_t phaseAdjustment);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioSequencerPhaseGet)(adi_adrv904x_Device_t* const device, uint32_t* phaseAdjustment);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioSequencerPreMcsCfg)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioSequencerPostMcsCfg)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioSequencerRadioStart)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioSequencerRadioStop)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioSequencerFrameTimingPatternSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RsSpecialSequencer_t* const specialPatternInfo);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioSequencerFrameTimingPatternGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_RsSpecialSequencer_t* const specialPatternInfo);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioSequencerFrameTimingPatternClear)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxToOrxMappingAndVswrDirSet)(adi_adrv904x_Device_t* const device, const uint8_t mapping, const uint8_t vswrDirection);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxToOrxMappingAndVswrDirGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxChannels_e orxChannel, adi_adrv904x_TxChannels_e* const txChannel, adi_adrv904x_DfeVswrDirection_e* const vswrDirection);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlPaSelectInputSet)(adi_adrv904x_Device_t* const device, const uint32_t paEnOutput, const adi_adrv904x_PaEnInputSel_e paEnInput);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlPaSelectInputGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_PaEnOutputs_e paEnOutput, adi_adrv904x_PaEnInputSel_e * const paEnInput);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlDtxFanoutToPaEnConfigSet)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const adi_adrv904x_DtxFanoutToPaEnCfg_t * const dtxFanoutCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlDtxFanoutToPaEnConfigGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_DtxFanoutToPaEnCfg_t * const dtxFanoutCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlExtAlarmConfigSet)(adi_adrv904x_Device_t* const device, const uint32_t alarmMask, const adi_adrv904x_RadioCtrlAlarmConfig_t * const alarmCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlExtAlarmConfigGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Alarms_e alarmId, adi_adrv904x_RadioCtrlAlarmConfig_t * const alarmCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlExtAlarmStateGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Alarms_e alarmId, adi_adrv904x_AlarmState_e * const alarmState);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlExtAlarmGpioStateGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Alarms_e alarmId, adi_adrv904x_AlarmState_e * const alarmState);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlExtAlarmStateClear)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Alarms_e alarmId);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlDbgLnaAlwaysOnSet)(adi_adrv904x_Device_t* const device, const uint32_t lnaEnMask, const uint8_t activate);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlDbgLnaAlwaysOnGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_LnaEnOutputs_e lnaEnId, uint8_t * const activate);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlDbgLnaAlwaysOffSet)(adi_adrv904x_Device_t* const device, const uint32_t lnaEnMask, const uint8_t activate);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlDbgLnaAlwaysOffGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_LnaEnOutputs_e lnaEnId, uint8_t * const activate);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlDbgPaAlwaysOnSet)(adi_adrv904x_Device_t* const device, const uint32_t paEnMask, const uint8_t activate);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlDbgPaAlwaysOnGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_PaEnOutputs_e paEnId, uint8_t * const activate);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlDbgPaAlwaysOffSet)(adi_adrv904x_Device_t* const device, const uint32_t paEnMask, const uint8_t activate);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlDbgPaAlwaysOffGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_PaEnOutputs_e paEnId, uint8_t * const activate);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlOcFuseEnOutSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Alarms_e alarmId, const adi_adrv904x_OutputState_e state);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlDbgGpioOutputSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_OutGpioSigTypes_e gpioSigType, const uint8_t signalMask, const adi_adrv904x_OutGpioPinStates_e gpioPinstate);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlDbgGpioOutputGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_OutGpioSigTypes_e gpioSigType, const uint8_t signalMask, adi_adrv904x_OutGpioPinStates_e * const gpioPinstate);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalCarrierCfgInitSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RadioCtrlAntCalCarrierCfgInit_t* const antCalCarrierCfgInit);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalCarrierCfgSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RadioCtrlAntCalCarrierCfg_t* const antCalCarrierCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalCarrierCfgSet_v2)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RadioCtrlAntCalCarrierCfg_t* const antCalCarrierCfg, uint32_t postCalPatternId);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalCarrierCfgGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_RadioCtrlAntCalCarrierCfg_t* const antCalCarrierCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalCarrierGainCountersClear)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalCarrierGainCountersGet)(adi_adrv904x_Device_t* const device, uint8_t* const antCalGpioCounter, uint8_t* const antCalCarrierTableIndex);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCtrlTxToOrxMappingConfigGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_DfeCtrlTxToOrxMappingConfig_t * const mappingConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCtrlTxToOrxMappingSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxChannels_e orxChannel, const adi_adrv904x_TxChannels_e txChannel);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCtrlTxToOrxMappingAndVswrDirSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxChannels_e orxChannel, const adi_adrv904x_TxChannels_e txChannel, const adi_adrv904x_DfeVswrDirection_e vswrDirection);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfeCtrlTxToOrxMappingTxOffLevelsSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxChannels_e orxChannel, const uint8_t txOffGpioLevels);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalOutGpioCfgGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_StreamGpioAntCalOutCfg_t* const pCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalOutGpioLevelSet)(adi_adrv904x_Device_t* const device, const uint32_t drive);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalGpioTruthTableSet)(adi_adrv904x_Device_t* const device, uint8_t tableSelection);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioCtrlAntCalGpioTruthTableGet)(adi_adrv904x_Device_t* const device, uint8_t* pTableSelection);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxGainTableWrite)(adi_adrv904x_Device_t* const device, const uint32_t rxChannelMask, const uint8_t gainIndexOffset, const adi_adrv904x_RxGainTableRow_t gainTableRow[], const uint32_t arraySize );
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxGainTableRead)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxChannels_e rxChannel, const uint8_t gainIndexOffset, adi_adrv904x_RxGainTableRow_t gainTableRow[], const uint32_t arraySize, uint16_t* const numGainIndicesRead);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxMinMaxGainIndexSet)(adi_adrv904x_Device_t* const device, const uint32_t rxChannelMask, const uint8_t minGainIndex, const uint8_t maxGainIndex);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxGainTableExtCtrlPinsSet)(adi_adrv904x_Device_t* const device, const uint32_t rxChannelMask, const uint32_t channelEnable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxGainSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxGain_t rxGain[], const uint32_t arraySize);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxMgcGainGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxChannels_e rxChannel, adi_adrv904x_RxGain_t * const rxGain);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxGainGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxChannels_e rxChannel, adi_adrv904x_RxGain_t * const rxGain);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxDataFormatGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxChannels_e rxChannel, adi_adrv904x_RxDataFormatRt_t * const rxDataFormat);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CddcDataFormatSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CddcDataFormatRt_t cddcDataFormat[], const uint32_t arraySize);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CddcDataFormatGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxChannels_e rxChannel, adi_adrv904x_CddcDataFormatRt_t * const cddcDataFormat);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxSlicerPositionGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxChannels_e rxChannel, uint8_t * const slicerPosition);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxLoSourceGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxChannels_e rxChannel, adi_adrv904x_LoSel_e * const rxLoSource);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxNcoShifterSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxNcoConfig_t * const rxNcoConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxNcoShifterGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_RxNcoConfigReadbackResp_t* const rxRbConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxCddcNcoSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxCddcNcoConfig_t* const rxNcoConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxCddcNcoGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_RxCddcNcoConfigReadbackResp_t* const rxRbConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_OrxNcoSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_ORxNcoConfig_t * const orxNcoConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_OrxNcoSet_v2)(adi_adrv904x_Device_t* const device, const adi_adrv904x_ORxNcoConfig_t * const orxNcoConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_OrxNcoAltSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_ORxAltNcoConfig_t * const orxAltNcoConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_OrxNcoAltGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_ORxAltNcoConfig_t* const orxAltNcoConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_OrxNcoGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_ORxNcoConfigReadbackResp_t* const orxRbConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxDecimatedPowerCfgSet)(adi_adrv904x_Device_t * const device, adi_adrv904x_RxDecimatedPowerCfg_t rxDecPowerCfg[], const uint32_t numOfDecPowerCfgs);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxDecimatedPowerCfgGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxChannels_e rxChannel, const adi_adrv904x_DecPowerMeasurementBlock_e decPowerBlockSelection, adi_adrv904x_RxDecimatedPowerCfg_t * const rxDecPowerCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxDecimatedPowerGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxChannels_e rxChannel, const adi_adrv904x_DecPowerMeasurementBlock_e decPowerBlockSelection, uint8_t * const powerReadBack);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_OrxAttenSet)(adi_adrv904x_Device_t* const device, const uint32_t channelMask, const uint8_t attenDb);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_OrxAttenGet)(adi_adrv904x_Device_t* const device, const uint8_t channelId, uint8_t* const attenDb);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxGainCtrlModeSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxGainCtrlModeCfg_t gainCtrlModeCfg[], const uint32_t arraySize);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxGainCtrlModeGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxChannels_e rxChannel, adi_adrv904x_RxGainCtrlMode_e* gainCtrlMode);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxTempGainCompSet)(adi_adrv904x_Device_t* const device, const uint32_t rxChannelMask, const int8_t gainValue);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxTempGainCompGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxChannels_e rxChannel, int8_t* const gainValue);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxTestDataSet)(adi_adrv904x_Device_t* device, const uint32_t rxChannelMask, const adi_adrv904x_RxTestDataCfg_t* const rxTestDataCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxTestDataGet)(adi_adrv904x_Device_t* device, const adi_adrv904x_RxChannels_e rxChannel, adi_adrv904x_RxTestDataCfg_t* const rxTestDataCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxLoPowerDownSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxChannels_e rxChannelMask, const uint8_t enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxCarrierRssiCfgSet)(adi_adrv904x_Device_t * const device, adi_adrv904x_RxCarrierRssiCfg_t rxCarrierRssiCfg[], const uint32_t numCfgs);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxCarrierRssiCfgGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask, adi_adrv904x_RxCarrierRssiCfg_t * const cfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxCarrierRssiEnable)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask, const uint32_t rxCarrierEnable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxCarrierRssiManualStart)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxCarrierRssiManualPause)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask, const uint32_t rxCarrierPause);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxCarrierRssiGpioCfgSet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxCarrierRssiGpioCfg_t * const gpioCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxCarrierRssiGpioCfgGet)(adi_adrv904x_Device_t * const device, adi_adrv904x_RxCarrierRssiGpioCfg_t * const gpioCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxCarrierRssiPowerRead)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask, int32_t * const gain_mdB);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxCarrierGainAdjustSet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask, const int32_t gain_mdB);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxCarrierGainAdjustGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask, int32_t * const gain_mdB);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxDynamicCarrierCalculate)(adi_adrv904x_Device_t* const device, adi_adrv904x_CarrierJesdCfg_t* const jesdCfg, adi_adrv904x_CarrierRadioCfg_t rxCarrierConfigs[], const adi_adrv904x_CarrierChannelFilterApplicationSel_t rxCarrierChannelFilterApplicationSel[], adi_adrv904x_ChannelFilterCfg_t rxCarrierChannelFilter[], const uint32_t numCarrierProfiles, const uint8_t useCustomFilters);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxDynamicCarrierWrite)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxDynamicCarrierReconfigureWithFilterSelect)( adi_adrv904x_Device_t* const device, adi_adrv904x_CarrierJesdCfg_t* const jesdCfg, adi_adrv904x_CarrierRadioCfg_t rxCarrierConfigs[], const adi_adrv904x_CarrierChannelFilterApplicationSel_t rxCarrierChannelFilterApplicationSel[], const uint32_t numCarrierProfiles);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxDynamicCarrierReconfigure)( adi_adrv904x_Device_t* const device, adi_adrv904x_CarrierJesdCfg_t* const jesdCfg, adi_adrv904x_CarrierRadioCfg_t rxCarrierConfigs[], adi_adrv904x_ChannelFilterCfg_t rxCarrierChannelFilter[], const uint32_t numCarrierProfiles);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxCarrierSettingsGet)( adi_adrv904x_Device_t* const device, const uint32_t rxChannel, adi_adrv904x_CarrierRadioCfg_t* const rxCarrierConfigs, adi_adrv904x_CarrierRuntime_t* const carrierRuntimeSettings);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxCarrierJesdCfgGet)( adi_adrv904x_Device_t* const device, adi_adrv904x_CarrierJesdCfg_t* const rxCarrierJesdCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxCarrierLatencyGet)( adi_adrv904x_Device_t* const device, const adi_adrv904x_RxChannels_e rxChannel, adi_adrv904x_CarrierReconfigLatencyCfg_t* const rxCarrierLatencyCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxRssiConfigSet)(adi_adrv904x_Device_t* const device, const uint16_t meterMask, const uint32_t channelMask, const adi_adrv904x_RxRssiCfg_t* const pPwrMtrRssiCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxRssiConfigGet)(adi_adrv904x_Device_t* const device, const uint16_t meterSel, const uint32_t channelSel, adi_adrv904x_RxRssiCfg_t* const pPwrMtrRssiCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxRssiEnable)(adi_adrv904x_Device_t* const device, const uint16_t meterMask, const uint32_t channelMask, const uint8_t enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxRssiReadBack)(adi_adrv904x_Device_t* const device, const uint16_t meterSel, const uint32_t channelSel, float* const pPwrMeasDb, float* const pPwrMeasLinear);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxRssiReadBack_v2)(adi_adrv904x_Device_t* const device, const uint16_t meterSel, const uint32_t channelSel, uint8_t* const pPwrMeasDb, uint64_t* const pPwrMeasLinear);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxAttenTableRead)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannel, const uint32_t txAttenIndexOffset, adi_adrv904x_TxAttenTableRow_t txAttenTableRows[], const uint32_t numTxAttenEntries);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxAttenSet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_TxAtten_t txAttenuation[], const uint32_t numTxAttenConfigs);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxAttenGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_TxChannels_e txChannel, adi_adrv904x_TxAtten_t * const txAttenuation);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxAttenCfgSet)(adi_adrv904x_Device_t* const device, const uint32_t chanMask, adi_adrv904x_TxAttenCfg_t* const txAttenCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxAttenCfgGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannel, adi_adrv904x_TxAttenCfg_t* const txAttenCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxAttenS0S1Set)(adi_adrv904x_Device_t* const device, const uint32_t chanMask, const uint32_t levelMilliDB, const uint8_t isS0);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxAttenS0S1Get)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannel, uint32_t* const levelMilliDB, const uint8_t isS0);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxAttenUpdateCfgSet)(adi_adrv904x_Device_t* const device, const uint32_t chanMask, const adi_adrv904x_TxAttenUpdateCfg_t* const cfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxAttenUpdateCfgGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannel, adi_adrv904x_TxAttenUpdateCfg_t* const cfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxAttenUpdate)(adi_adrv904x_Device_t *const device, const uint32_t chanMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxLoSourceGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_TxChannels_e txChannel, adi_adrv904x_LoSel_e * const txLoSource);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxPowerMonitorCfgSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_PowerMonitorCfgRt_t txPowerMonitorCfg[], const uint32_t numPowerProtectCfgs);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxPowerMonitorCfgGet)(adi_adrv904x_Device_t * const device, adi_adrv904x_PowerMonitorCfgRt_t* const txPowerMonitorCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxProtectionErrorGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannel, uint32_t* const eventBits);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxProtectionErrorClear)(adi_adrv904x_Device_t * const device, const adi_adrv904x_TxChannels_e txChannel, const uint32_t eventBits);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxSlewRateDetectorCfgSet)(adi_adrv904x_Device_t* const device, adi_adrv904x_SlewRateDetectorCfgRt_t txSlewRateDetectorCfg[], const uint32_t numSlewRateDetCfgs);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxSlewRateDetectorCfgGet)(adi_adrv904x_Device_t * const device, adi_adrv904x_SlewRateDetectorCfgRt_t * const txSlewRateDetectorCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxSlewRateStatisticsRead)(adi_adrv904x_Device_t * const device, const adi_adrv904x_TxChannels_e txChannel, const uint8_t clearStats, uint16_t * const statisticsReadback);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxProtectionRampSampleHoldEnableSet)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const uint8_t enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxProtectionRampSampleHoldEnableGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannel, uint8_t* const enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxProtectionRampCfgSet)(adi_adrv904x_Device_t* const device, adi_adrv904x_ProtectionRampCfgRt_t txProtectionRampCfg[], const uint32_t numProtectionRampCfgs);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxProtectionRampCfgGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_ProtectionRampCfgRt_t* const txProtectionRampCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxPowerMonitorStatusGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_TxChannels_e txChannel, adi_adrv904x_TxPowerMonitorStatus_t * const powerMonitorStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxNcoShifterSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxNcoMixConfig_t * const txNcoConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxNcoShifterGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_TxNcoMixConfigReadbackResp_t* const txRbConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxCducNcoSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxCducNcoConfig_t * const txCducNcoConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxCducNcoGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_TxCducNcoConfigReadbackResp_t* const txRbConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxPfirFracDelaySet)(adi_adrv904x_Device_t* const device, const uint8_t txChan, const uint8_t fracDelay);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxPfirFracDelaySet_v2)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const uint8_t fracDelay);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxTestToneSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxTestNcoConfig_t * const txNcoConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxTestToneGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_TxTestNcoConfigReadbackResp_t* const txRbConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxDecimatedPowerCfgSet)(adi_adrv904x_Device_t * const device, adi_adrv904x_TxDecimatedPowerCfg_t txDecPowerCfg[], const uint32_t numOfDecPowerCfgs);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxDecimatedPowerCfgGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_TxChannels_e txChannel, adi_adrv904x_TxDecimatedPowerCfg_t * const txDecPowerCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxDecimatedPowerGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_TxChannels_e txChannel, uint8_t * const powerReadBack, uint8_t * const powerPeakReadBack);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxAttenPhaseSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxAttenPhaseCfg_t * const txAttenPhaseCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxAttenPhaseGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_TxAttenPhaseCfg_t* const txRbConfig);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DtxCfgSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DtxCfg_t dtxCfg[], const uint32_t numDtxCfgs);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DtxCfgGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannel, adi_adrv904x_DtxCfg_t* const dtxCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DtxGpioCfgSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_DtxGpioCfg_t* dtxGpioCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DtxGpioCfgGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_DtxGpioCfg_t* const dtxGpioCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DtxForceSet)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const uint8_t dtxForce);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DtxStatusGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannel, uint8_t * const dtxStatus);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrTssiConfigSet)(adi_adrv904x_Device_t* const device, const uint32_t channelMask, const adi_adrv904x_DfeSvcDfePwrMtrTssiCfg_t* pPwrMtrTssiCfg, const adi_adrv904x_DfeSvcDfePwrMtrTssi_e meter, const adi_adrv904x_DfeSvcDfePwrMtrTssiSrc_e source);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrTssiConfigGet)(adi_adrv904x_Device_t* const device, const uint32_t channelSel, adi_adrv904x_DfeSvcDfePwrMtrTssiCfg_t* pPwrMtrTssiCfg, const adi_adrv904x_DfeSvcDfePwrMtrTssi_e meter);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_PostCfrGainSet)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const uint32_t gain_milli);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_PostCfrGainGet)(adi_adrv904x_Device_t* const device, const uint32_t txChannelSel, uint32_t * const gain_milli);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrTssiEnable)(adi_adrv904x_Device_t* const device, const uint32_t channelMask, const uint8_t enable, const adi_adrv904x_DfeSvcDfePwrMtrTssi_e meter);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrClgcTssiEnable)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const uint8_t enable, const uint8_t useTxMeter1, const uint8_t useTxMeter2);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrTssiManualStart)(adi_adrv904x_Device_t* const device, const uint32_t channelMask, const uint8_t start, const adi_adrv904x_DfeSvcDfePwrMtrTssi_e meter);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrTssiReadBack)(adi_adrv904x_Device_t* const device, const uint32_t channelSel, const adi_adrv904x_DfeSvcDfePwrMtrTssi_e meter, adi_adrv904x_DfeSvcDfePwrMtrTssiMeasResult_t* pPwrMeas);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrClgcTssiReadBack)(adi_adrv904x_Device_t* const device, const uint32_t txChannelSel, const uint8_t useTxMeter1, const uint8_t useTxMeter2, uint64_t *pPwrMeasTx1, uint64_t *pPwrMeasTx2, uint64_t *pPwrMeasOrx);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrDpdInConfigSet)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const adi_adrv904x_DfeSvcDfePwrMtrDpdCfg_t* pPwrMtrDpdIn);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrDpdInConfigGet)(adi_adrv904x_Device_t* const device, const uint32_t txChannelSel, adi_adrv904x_DfeSvcDfePwrMtrDpdCfg_t* pPwrMtrDpdIn);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrDpdOutConfigSet)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const adi_adrv904x_DfeSvcDfePwrMtrDpdCfg_t* pPwrMtrDpdOut);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrDpdOutConfigGet)(adi_adrv904x_Device_t* const device, const uint32_t txChannelSel, adi_adrv904x_DfeSvcDfePwrMtrDpdCfg_t* pPwrMtrDpdOut);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrDpdInEnable)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const uint8_t enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrDpdOutEnable)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const uint8_t enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrDpdInOutReadBack)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannelSel, adi_adrv904x_DfeSvcDfePwrMtrDpdResults_t* pPwrDpdInOut);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrTxCarrierCfgSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxCarrierMask_t* const txCarrierMask, const adi_adrv904x_DfeSvcDfeTxCarrierPowerMeasCfg_t* pPwrTxCarrierCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrTxCarrierCfgGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxCarrierMask_t* const txCarrierSel, adi_adrv904x_DfeSvcDfeTxCarrierPowerMeasCfg_t* pPwrTxCarrrierCfgResp);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrTxCarrierMeasEnable)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxCarrierMask_t* const txCarrierMask, const uint8_t enable);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrTxCarrierManualStart)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxCarrierMask_t* const txCarrierMask, const uint8_t start);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DfePwrMtrTxCarrierMeasGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxCarrierMask_t* const txCarrierSel, adi_adrv904x_DfeSvcDfeTxCarrierPowerMeasResult_t* pPwrTxCarrrierMeasResult);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_HardClipperConfigSet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_HardClipperConfig_t * const hardClipperConfig, const uint32_t hcTxChannelMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_HardClipperConfigGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_HardClipperConfig_t * const hardClipperConfig, const uint32_t hcTxChannelSel);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_HardClipperStatisticsGet)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannelSel, adi_adrv904x_HardClipperStatistics_t * const hardClipperStat, uint8_t clearStats);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxCarrierGainAdjustSet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_TxCarrierMask_t * const txCarrierMask, const int32_t gain_mdB);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxCarrierGainAdjustGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_TxCarrierMask_t * const txCarrierSel, int32_t * const gain_mdB);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxCarrierBandAttenSet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_TxBandMask_t * const txBandMask, const uint32_t atten_mdB);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxCarrierBandAttenGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_TxBandMask_t * const txBandSel, uint32_t * const atten_mdB);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_PostDpdAttenSet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_TxDpdMask_t * const txDpdMask, const adi_adrv904x_PostDpdAtten_t * const attenParams);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_PostDpdAttenGet)(adi_adrv904x_Device_t * const device, const adi_adrv904x_TxDpdMask_t * const txDpdSel, adi_adrv904x_PostDpdAtten_t * const attenParams);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxPlaybackDataWrite)(adi_adrv904x_Device_t* const device, const uint32_t txChannelMask, const uint8_t bandSelect, const adi_adrv904x_TxPlaybackData_t* const playbackData);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxPlaybackDataRead)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannelSel, const uint8_t bandSelect, adi_adrv904x_TxPlaybackData_t* const playbackData);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxPlaybackStart)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannelSel, const uint8_t bandSelect, const uint32_t dataRate_kHz);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxPlaybackStop)(adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannelSel, const uint8_t bandSelect);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxDynamicCarrierCalculate)(adi_adrv904x_Device_t* const device, adi_adrv904x_CarrierJesdCfg_t* const jesdCfg, adi_adrv904x_CarrierRadioCfg_t txCarrierConfigs[], const adi_adrv904x_CarrierChannelFilterApplicationSel_t txCarrierChannelFilterApplicationSel[], adi_adrv904x_ChannelFilterCfg_t txCarrierChannelFilter[], const uint32_t numCarrierProfiles, const uint8_t useCustomFilters);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxDynamicCarrierCalculateExtended)(adi_adrv904x_Device_t* const device, adi_adrv904x_CarrierJesdCfg_t* const jesdCfg, adi_adrv904x_CarrierRadioCfg_t txCarrierConfigs[], const adi_adrv904x_CarrierChannelFilterApplicationSel_t txCarrierChannelFilterApplicationSel[], adi_adrv904x_ChannelFilterCfg_t txCarrierChannelFilter[], adi_adrv904x_CarrierRadioCfgExtended_t txCarrierConfigsExtended[], const uint32_t numCarrierProfiles, const uint8_t useCustomFilters);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxDynamicCarrierWrite)(adi_adrv904x_Device_t* const device);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxDynamicCarrierReconfigureWithFilterSelect)(adi_adrv904x_Device_t* const device, adi_adrv904x_CarrierJesdCfg_t* const jesdCfg, adi_adrv904x_CarrierRadioCfg_t txCarrierConfigs[], const adi_adrv904x_CarrierChannelFilterApplicationSel_t txCarrierChannelFilterApplicationSel[], const uint32_t numCarrierProfiles);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxDynamicCarrierReconfigureWithFilterSelectExtended)(adi_adrv904x_Device_t* const device, adi_adrv904x_CarrierJesdCfg_t* const jesdCfg, adi_adrv904x_CarrierRadioCfg_t txCarrierConfigs[], const adi_adrv904x_CarrierChannelFilterApplicationSel_t txCarrierChannelFilterApplicationSel[], adi_adrv904x_CarrierRadioCfgExtended_t txCarrierConfigsExtended[], const uint32_t numCarrierProfiles);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxDynamicCarrierReconfigureExtended)(adi_adrv904x_Device_t* const device, adi_adrv904x_CarrierJesdCfg_t* const jesdCfg, adi_adrv904x_CarrierRadioCfg_t txCarrierConfigs[], adi_adrv904x_ChannelFilterCfg_t txCarrierChannelFilter[], adi_adrv904x_CarrierRadioCfgExtended_t txCarrierConfigsExtended[], const uint32_t numCarrierProfiles);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxDynamicCarrierReconfigure)(adi_adrv904x_Device_t* const device, adi_adrv904x_CarrierJesdCfg_t* const jesdCfg, adi_adrv904x_CarrierRadioCfg_t txCarrierConfigs[], adi_adrv904x_ChannelFilterCfg_t txCarrierChannelFilter[], const uint32_t numCarrierProfiles);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxCarrierSettingsGet)( adi_adrv904x_Device_t* const device, const uint32_t txChannel, adi_adrv904x_CarrierRadioCfg_t* const txCarrierConfigs, adi_adrv904x_CarrierRuntime_t* const carrierRuntimeSettings);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxCarrierJesdCfgGet)( adi_adrv904x_Device_t* const device, adi_adrv904x_CarrierJesdCfg_t* const txCarrierJesdCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_TxCarrierLatencyGet)( adi_adrv904x_Device_t* const device, const adi_adrv904x_TxChannels_e txChannel, adi_adrv904x_CarrierReconfigLatencyCfg_t* const txCarrierLatencyCfg);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuImageLoad)(adi_adrv904x_Device_t* const device, const adi_adrv904x_cpuBinaryInfo_t* const cpuBinaryInfo);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_StreamImageLoad)(adi_adrv904x_Device_t* const device, const adi_adrv904x_streamBinaryInfo_t* const streamBinaryInfoPtr);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxGainTableLoad)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxGainTableInfo_t rxGainTableInfo[], const uint32_t rxGainTableArrSize);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeviceInfoExtract)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CpuProfileBinaryInfo_t* const cpuProfileBinaryInfoPtr);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuProfileImageLoad)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CpuProfileBinaryInfo_t* const cpuProfileBinaryInfoPtr);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_DeviceCopy)( adi_adrv904x_Device_t* const deviceSrc, adi_adrv904x_Device_t* const deviceDest);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_PreMcsInit)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Init_t* const init, const adi_adrv904x_TrxFileInfo_t* const trxBinaryInfoPtr);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_PreMcsInit_NonBroadcast)(adi_adrv904x_Device_t* const device, const adi_adrv904x_Init_t* const init);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_PostMcsInit)(adi_adrv904x_Device_t* const device, const adi_adrv904x_PostMcsInit_t* const utilityInit);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_StandbyEnter)(adi_adrv904x_Device_t* const device, adi_adrv904x_StandbyRecover_t* const standbyRecover);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_StandbyRecover)(adi_adrv904x_Device_t* const device, adi_adrv904x_StandbyRecover_t* const standbyRecover);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_StandbyExit)(adi_adrv904x_Device_t* const device, adi_adrv904x_StandbyRecover_t* const standbyRecover);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_StandbyExitStatusGet)(adi_adrv904x_Device_t* const device, adi_adrv904x_StandbyRecover_t* const standbyRecover, uint8_t * const done);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_JrxRepairSwCEnableGet)(adi_adrv904x_Device_t* const device, uint8_t* const swcEnMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_JrxRepairSwCEnableSet)(adi_adrv904x_Device_t* const device, const uint8_t swcEnMask);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_JrxRepairVcmLanesFix)(adi_adrv904x_Device_t* const device, uint8_t laneMask, uint8_t enableFix);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_JrxRepairInitialization)(adi_adrv904x_Device_t* const device, uint8_t enableRepair);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuMemDump)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CpuMemDumpBinaryInfo_t* const cpuMemDumpBinaryInfoPtr, const uint8_t forceException, uint32_t* const dumpSize);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CpuMemDump_vRamOnly)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CpuMemDumpBinaryInfo_t* const cpuMemDumpBinaryInfoPtr, const uint8_t forceException, uint32_t* const dumpSize);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxGainTableChecksumRead)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxGainTableInfo_t* const rxGainTableInfoPtr, uint32_t* const rxGainTableChecksum);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RxGainTableChecksumCalculate)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RxChannels_e rxChannel, uint32_t* const rxGainTableChecksum);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_InitDataExtract)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CpuProfileBinaryInfo_t* const cpuBinaryInfo, adi_adrv904x_Version_t* const apiVer, adi_adrv904x_CpuFwVersion_t* const fwVer, adi_adrv904x_Version_t* const streamVer, adi_adrv904x_Init_t* const init, adi_adrv904x_PostMcsInit_t* const postMcsInit, adi_adrv904x_ExtractInitDataOutput_e* const checkOutput);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioSequencerImageLoad)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RadioSequencerBinaryInfo_t* const rsBinaryInfoPtr);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_RadioSequencerImageReload)(adi_adrv904x_Device_t* const device, const adi_adrv904x_RadioSequencerBinaryInfo_t* const rsBinaryInfoPtr);
ADI_API_EX adi_adrv904x_ErrAction_e (*adi_adrv904x_CfrPulseExtractAndWrite)(adi_adrv904x_Device_t* const device, const adi_adrv904x_CpuProfileBinaryInfo_t* const cpuProfileBinaryInfoPtr);


/**
 * \brief Loads the version of the ADRV904X device contained in the shared
 * library supplied for use by the application.
 *
 * Opens the specified library and searches it for implementations of all the
 * functions in the ADRV904X API. Then assigns the global function pointer
 * corresponding to each function to the implementation in the specified shared
 * library.
 *
 * \param[in] soPath The shared library to open. Unless this is a relative or fully
 * qualified path LD_LIBRARY_PATH env var or other shared object location
 * mechanism must be correctly set to find the shared lib.
 *
 * Note: The default is that applications statically link to the device
 * library. To  use this function the application must be compiled as a
 * multiversioning capable application. See the ADRV904X Software User Guide
 * for details.
 *
 * \returns Returns 0 on success.
 */
ADI_API int adi_adrv904x_VersionLoad(const char* const soPath);

/**
 * \brief Same as adi_adrv904x_VersionLoad but with control of log messages.
 *
 * \param[in] soPath Same as adi_adrv904x_VersionLoad.
 * \param[in] logStream A valid FILE* stream where error messages will be sent. If NULL
 *     error are not logged.
 *
 * \returns Same as adi_adrv904x_VersionLoad.
 */
ADI_API int adi_adrv904x_VersionLoad_vLogCtl(const char* const soPath, FILE* const logStream);

#endif
