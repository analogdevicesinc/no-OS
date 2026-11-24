#ifndef __ADI_NEVIS_TEMPLATE_PROFILE_PS2_H__
#define __ADI_NEVIS_TEMPLATE_PROFILE_PS2_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "device_profile_bundle_t.h"

deviceProfilePS2_t config_uc_id_profile_ps2_0 = {
		.ver = {
		.major = 13, 
		.minor = 0}
, 
		.ps2ArmClk = 0, 
		.devClock_Hz = 0, 
		.ps1ArmClk = 0, 
		.ps1ArmClkDiv = 0, 
		.sysConfig = {
		.duplexMode = 0}
, 
		.rxConfig = { {
		.outputSignalingSel = 0, 
		.gainTableType = 0, 
		.rxOutputRate = 0}, {
		.outputSignalingSel = 0, 
		.gainTableType = 0, 
		.rxOutputRate = 0}  }, 
		.orxConfig = { {
		.rxOutputRate = 0}, {
		.rxOutputRate = 0}  }, 
		.ilbConfig = { {
		.rxOutputRate = 0}, {
		.rxOutputRate = 0}  }, 
		.elbConfig = { {
		.rxOutputRate = 0}, {
		.rxOutputRate = 0}  }, 
		.rxNbConfig = {
		.gainControl = {
		.rxNbAgcHighThresholdModeEnable = 0, 
		.agcHighThresholdSettings = {
		.ApdPeakOverloadThresh = 0, 
		.ApdPeakUnderloadThresh = 0, 
		.Hb2PeakOverloadThresh = 0, 
		.Hb2PeakUnderloadThresh = 0, 
		.Hb2SecondaryUpperThresh = 0}
}
}
, 
		.txConfig = { {
		.outputSignalingSel = 0, 
		.txInputRate = 0}, {
		.outputSignalingSel = 0, 
		.txInputRate = 0}  }, 
		.checksum = 0};

#endif /* __ADI_NEVIS_TEMPLATE_PROFILE_PS2_H__ */
