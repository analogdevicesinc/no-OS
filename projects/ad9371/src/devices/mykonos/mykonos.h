/*!
 * \file mykonos.h
 * \brief Contains macro definitions and function prototypes for mykonos.c
 *
 * Mykonos API version: 1.5.2.3566
 */

/**
* \page Disclaimer Legal Disclaimer
* Copyright 2015-2017 Analog Devices Inc.
* Released under the AD9371 API license, for more information see the "LICENSE.txt" file in this zip file.
*
*/

#ifndef _MYKONOS_LIB_H_
#define _MYKONOS_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "t_mykonos.h"
#include "mykonos_version.h"

#define TX_PROFILE_VALID    0x01
#define RX_PROFILE_VALID    0x02
#define ORX_PROFILE_VALID   0x04
#define SNIFF_PROFILE_VALID 0x08

/* 
 ****************************************************************************
 * General Initialization functions 
 ****************************************************************************
 */
mykonosErr_t MYKONOS_resetDevice(mykonosDevice_t *device);
mykonosErr_t MYKONOS_getDeviceRev(mykonosDevice_t *device, uint8_t *revision);
mykonosErr_t MYKONOS_getProductId(mykonosDevice_t *device, uint8_t *productId);
mykonosErr_t MYKONOS_setSpiSettings(mykonosDevice_t *device);
mykonosErr_t MYKONOS_verifyDeviceDataStructure(mykonosDevice_t *device);
mykonosErr_t MYKONOS_verifyProfiles(mykonosDevice_t *device);
mykonosErr_t MYKONOS_initialize(mykonosDevice_t *device); 
mykonosErr_t MYKONOS_waitForEvent(mykonosDevice_t *device, waitEvent_t waitEvent, uint32_t timeout_us);
mykonosErr_t MYKONOS_readEventStatus(mykonosDevice_t *device, waitEvent_t waitEvent, uint8_t *eventDone);
mykonosErr_t MYKONOS_getApiVersion (mykonosDevice_t *device, uint32_t *siVer, uint32_t *majorVer, uint32_t *minorVer, uint32_t *buildVer);

/*
 *****************************************************************************
 * PLL / Synthesizer functions
 *****************************************************************************
 */
mykonosErr_t MYKONOS_initDigitalClocks(mykonosDevice_t *device);
mykonosErr_t MYKONOS_setRfPllFrequency(mykonosDevice_t *device, mykonosRfPllName_t pllName, uint64_t rfPllLoFrequency_Hz);
mykonosErr_t MYKONOS_getRfPllFrequency(mykonosDevice_t *device, mykonosRfPllName_t pllName, uint64_t *rfPllLoFrequency_Hz);
mykonosErr_t MYKONOS_checkPllsLockStatus(mykonosDevice_t *device, uint8_t *pllLockStatus);
mykonosErr_t MYKONOS_setRfPllLoopFilter(mykonosDevice_t *device, mykonosRfPllName_t pllName, uint16_t loopBandwidth_kHz, uint8_t stability);
mykonosErr_t MYKONOS_getRfPllLoopFilter(mykonosDevice_t *device, mykonosRfPllName_t pllName, uint16_t *loopBandwidth_kHz, uint8_t *stability);
/*
 *****************************************************************************
 * ARM Processor Control Functions
 *****************************************************************************
 */
mykonosErr_t MYKONOS_initArm(mykonosDevice_t *device);
mykonosErr_t MYKONOS_loadArmFromBinary(mykonosDevice_t *device, uint8_t *binary, uint32_t count);
mykonosErr_t MYKONOS_loadArmConcurrent(mykonosDevice_t *device, uint8_t *binary, uint32_t count);
mykonosErr_t MYKONOS_verifyArmChecksum(mykonosDevice_t *device);
mykonosErr_t MYKONOS_checkArmState(mykonosDevice_t *device, mykonosArmState_t armStateCheck);
mykonosErr_t MYKONOS_getArmVersion(mykonosDevice_t *device, uint8_t *majorVer, uint8_t *minorVer, uint8_t *rcVer, mykonosBuild_t *buildType);

mykonosErr_t MYKONOS_configDpd(mykonosDevice_t *device);
mykonosErr_t MYKONOS_getDpdConfig(mykonosDevice_t *device);
mykonosErr_t MYKONOS_getDpdStatus(mykonosDevice_t *device, mykonosTxChannels_t txChannel, mykonosDpdStatus_t *dpdStatus);
mykonosErr_t MYKONOS_restoreDpdModel(mykonosDevice_t *device, mykonosTxChannels_t txChannel, uint8_t *modelDataBuffer, uint32_t modelNumberBytes);
mykonosErr_t MYKONOS_saveDpdModel(mykonosDevice_t *device, mykonosTxChannels_t txChannel, uint8_t *modelDataBuffer, uint32_t modelNumberBytes);
mykonosErr_t MYKONOS_setDpdActState(mykonosDevice_t *device, mykonosTxChannels_t txChannel, uint8_t actState);
mykonosErr_t MYKONOS_resetDpd(mykonosDevice_t *device, mykonosTxChannels_t txChannel, mykonosDpdResetMode_t reset);
mykonosErr_t MYKONOS_setDpdBypassConfig(mykonosDevice_t *device, mykonosDpdBypassConfig_t *actConfig);
mykonosErr_t MYKONOS_getDpdBypassConfig(mykonosDevice_t *device, mykonosDpdBypassConfig_t *actConfig);
mykonosErr_t MYKONOS_setDpdActuatorCheck(mykonosDevice_t *device, mykonosDpdActuatorCheck_t *actCheck);
mykonosErr_t MYKONOS_getDpdActuatorCheck(mykonosDevice_t *device, mykonosDpdActuatorCheck_t *actCheck);

mykonosErr_t MYKONOS_setClgcAttenTuningConfig(mykonosDevice_t *device, mykonosClgcAttenTuningConfig_t *attRangeCfg);
mykonosErr_t MYKONOS_getClgcAttenTuningConfig(mykonosDevice_t *device, mykonosClgcAttenTuningConfig_t *attRangeCfg);

mykonosErr_t MYKONOS_configClgc(mykonosDevice_t *device);
mykonosErr_t MYKONOS_getClgcConfig(mykonosDevice_t *device);
mykonosErr_t MYKONOS_getClgcStatus(mykonosDevice_t *device, mykonosTxChannels_t txChannel, mykonosClgcStatus_t *clgcStatus);
mykonosErr_t MYKONOS_setClgcGain(mykonosDevice_t *device, mykonosTxChannels_t txChannel, int16_t gain);

mykonosErr_t MYKONOS_configVswr(mykonosDevice_t *device);
mykonosErr_t MYKONOS_getVswrConfig(mykonosDevice_t *device);
mykonosErr_t MYKONOS_getVswrStatus(mykonosDevice_t *device, mykonosTxChannels_t txChannel, mykonosVswrStatus_t *vswrStatus);

mykonosErr_t MYKONOS_runInitCals(mykonosDevice_t *device, uint32_t calMask);
mykonosErr_t MYKONOS_waitInitCals(mykonosDevice_t *device, uint32_t timeoutMs, uint8_t *errorFlag, uint8_t *errorCode);
mykonosErr_t MYKONOS_abortInitCals(mykonosDevice_t *device, uint32_t *calsCompleted);
mykonosErr_t MYKONOS_getInitCalStatus(mykonosDevice_t *device, mykonosInitCalStatus_t *initCalStatus);

mykonosErr_t MYKONOS_resetExtTxLolChannel(mykonosDevice_t *device, mykonosTxChannels_t channelSel);

mykonosErr_t MYKONOS_radioOn(mykonosDevice_t *device);
mykonosErr_t MYKONOS_radioOff(mykonosDevice_t *device);
mykonosErr_t MYKONOS_getRadioState(mykonosDevice_t *device, uint32_t *radioStatus);
mykonosErr_t MYKONOS_enableTrackingCals(mykonosDevice_t *device, uint32_t enableMask);
mykonosErr_t MYKONOS_rescheduleTrackingCal(mykonosDevice_t *device, mykonosTrackingCalibrations_t trackingCal);
mykonosErr_t MYKONOS_setAllTrackCalState(mykonosDevice_t *device, uint32_t trackingCalMask);
mykonosErr_t MYKONOS_getAllTrackCalState(mykonosDevice_t *device, uint32_t *trackCals);
mykonosErr_t MYKONOS_setTrackingCalState(mykonosDevice_t *device, mykonosTrackingCalibrations_t trackingCal, uint8_t trackCalState);
mykonosErr_t MYKONOS_getTrackingCalState(mykonosDevice_t *device, mykonosTrackingCalibrations_t trackingCal, uint8_t *trackCalState);
mykonosErr_t MYKONOS_getEnabledTrackingCals(mykonosDevice_t *device, uint32_t *enableMask);
mykonosErr_t MYKONOS_getPendingTrackingCals(mykonosDevice_t *device, uint32_t *pendingCalMask);
mykonosErr_t MYKONOS_getTxLolStatus(mykonosDevice_t *device, mykonosTxChannels_t txChannel, mykonosTxLolStatus_t *txLolStatus);
mykonosErr_t MYKONOS_getTxQecStatus(mykonosDevice_t *device, mykonosTxChannels_t txChannel, mykonosTxQecStatus_t *txQecStatus);

mykonosErr_t MYKONOS_getRxQecStatus(mykonosDevice_t *device, mykonosRxChannels_t rxChannel, mykonosRxQecStatus_t *rxQecStatus);
mykonosErr_t MYKONOS_getOrxQecStatus(mykonosDevice_t *device, mykonosObsRxChannels_t orxChannel, mykonosOrxQecStatus_t *orxQecStatus);

mykonosErr_t MYKONOS_setSnifferChannel(mykonosDevice_t *device, mykonosSnifferChannel_t snifferChannel);
mykonosErr_t MYKONOS_setRadioControlPinMode(mykonosDevice_t *device);

mykonosErr_t MYKONOS_setPathDelay(mykonosDevice_t *device, mykonosPathdelay_t *pathDelay);
mykonosErr_t MYKONOS_getPathDelay(mykonosDevice_t *device, mykonosPathDelaySel_t select, mykonosPathdelay_t *pathDelay);

mykonosErr_t MYKONOS_getDpdErrorCounters(mykonosDevice_t *device,  mykonosTxChannels_t txChannel, mykonosDpdErrorCounters_t *dpdErrCnt);

/* Low level ARM functions */
mykonosErr_t MYKONOS_readArmMem(mykonosDevice_t *device, uint32_t address, uint8_t *returnData, uint32_t bytesToRead, uint8_t autoIncrement);
mykonosErr_t MYKONOS_writeArmMem(mykonosDevice_t *device, uint32_t address, uint8_t *data, uint32_t byteCount);
mykonosErr_t MYKONOS_writeArmConfig(mykonosDevice_t *device, uint8_t objectId, uint16_t offset, uint8_t *data, uint8_t byteCount);
mykonosErr_t MYKONOS_readArmConfig(mykonosDevice_t *device, uint8_t objectId, uint16_t offset, uint8_t *data, uint8_t byteCount);
mykonosErr_t MYKONOS_sendArmCommand(mykonosDevice_t *device, uint8_t opCode,  uint8_t *extendedData, uint8_t extendedDataNumBytes);
mykonosErr_t MYKONOS_readArmCmdStatus(mykonosDevice_t *device, uint16_t *errorWord, uint16_t *statusWord);
mykonosErr_t MYKONOS_readArmCmdStatusByte(mykonosDevice_t *device, uint8_t opCode, uint8_t *cmdStatByte);
mykonosErr_t MYKONOS_waitArmCmdStatus(mykonosDevice_t *device, uint8_t opCode, uint32_t timeoutMs, uint8_t *cmdStatByte);
mykonosErr_t MYKONOS_writeArmProfile(mykonosDevice_t *device);
mykonosErr_t MYKONOS_loadAdcProfiles(mykonosDevice_t *device);


/*
 *****************************************************************************
 * JESD204B Datapath Functions
 *****************************************************************************
 */
/* Functions to setup the JESD204B IP */
mykonosErr_t MYKONOS_resetDeframer(mykonosDevice_t *device);
mykonosErr_t MYKONOS_setupSerializers(mykonosDevice_t *device);
mykonosErr_t MYKONOS_setupDeserializers(mykonosDevice_t *device);
mykonosErr_t MYKONOS_setupJesd204bFramer(mykonosDevice_t *device);
mykonosErr_t MYKONOS_setupJesd204bObsRxFramer(mykonosDevice_t *device);
mykonosErr_t MYKONOS_setupJesd204bDeframer(mykonosDevice_t *device);
mykonosErr_t MYKONOS_enableRxFramerLink(mykonosDevice_t *device, uint8_t enable);
mykonosErr_t MYKONOS_enableObsRxFramerLink(mykonosDevice_t *device, uint8_t enable);

/* Functions to initialize the JESD204B link */
mykonosErr_t MYKONOS_enableMultichipSync(mykonosDevice_t *device, uint8_t enableMcs, uint8_t *mcsStatus);
mykonosErr_t MYKONOS_enableSysrefToRxFramer(mykonosDevice_t *device, uint8_t enable);
mykonosErr_t MYKONOS_enableSysrefToObsRxFramer(mykonosDevice_t *device, uint8_t enable);
mykonosErr_t MYKONOS_enableSysrefToDeframer(mykonosDevice_t *device, uint8_t enable);

/* Functions to help debug the JESD204B link */
mykonosErr_t MYKONOS_readRxFramerStatus(mykonosDevice_t *device, uint8_t *framerStatus);
mykonosErr_t MYKONOS_readOrxFramerStatus(mykonosDevice_t *device, uint8_t *obsFramerStatus);
mykonosErr_t MYKONOS_readDeframerStatus(mykonosDevice_t *device, uint8_t *deframerStatus);
mykonosErr_t MYKONOS_getDeframerFifoDepth(mykonosDevice_t *device, uint8_t *fifoDepth, uint8_t *readEnLmfcCount);

/* PRBS debug functions */
mykonosErr_t MYKONOS_enableRxFramerPrbs(mykonosDevice_t *device, mykonosPrbsOrder_t polyOrder, uint8_t enable);
mykonosErr_t MYKONOS_enableObsRxFramerPrbs(mykonosDevice_t *device, mykonosPrbsOrder_t polyOrder, uint8_t enable);
mykonosErr_t MYKONOS_rxInjectPrbsError(mykonosDevice_t *device);
mykonosErr_t MYKONOS_obsRxInjectPrbsError(mykonosDevice_t *device);
mykonosErr_t MYKONOS_enableDeframerPrbsChecker(mykonosDevice_t *device, uint8_t lanes, mykonosPrbsOrder_t polyOrder, uint8_t enable);
mykonosErr_t MYKONOS_clearDeframerPrbsCounters(mykonosDevice_t *device);
mykonosErr_t MYKONOS_readDeframerPrbsCounters(mykonosDevice_t *device, uint8_t counterSelect, uint32_t *prbsErrorCount);

/* Miscellaneous debug functions */
mykonosErr_t MYKONOS_jesd204bIlasCheck(mykonosDevice_t *device, uint16_t *mismatch);
mykonosErr_t MYKONOS_setRxFramerDataSource(mykonosDevice_t *device, uint8_t dataSource);
mykonosErr_t MYKONOS_setObsRxFramerDataSource(mykonosDevice_t *device, uint8_t dataSource);

/*
 *****************************************************************************
 * Shared Data path functions
 *****************************************************************************
 */
mykonosErr_t MYKONOS_programFir(mykonosDevice_t *device, mykonosfirName_t filterToProgram, mykonosFir_t *firFilter);
mykonosErr_t MYKONOS_readFir(mykonosDevice_t *device, mykonosfirName_t filterToRead, mykonosFir_t *firFilter);

/*
 *****************************************************************************
 * Rx Data path functions
 *****************************************************************************
 */
mykonosErr_t MYKONOS_programRxGainTable(mykonosDevice_t *device, uint8_t *gainTablePtr, uint8_t numGainIndexesInTable, mykonosGainTable_t rxChannel);
mykonosErr_t MYKONOS_setRx1ManualGain(mykonosDevice_t *device, uint8_t gainIndex);
mykonosErr_t MYKONOS_setRx2ManualGain(mykonosDevice_t *device, uint8_t gainIndex);
mykonosErr_t MYKONOS_getRx1Gain(mykonosDevice_t *device, uint8_t *rx1GainIndex);
mykonosErr_t MYKONOS_getRx2Gain(mykonosDevice_t *device, uint8_t *rx2GainIndex);
mykonosErr_t MYKONOS_setupRxAgc(mykonosDevice_t *device);
mykonosErr_t MYKONOS_resetRxAgc(mykonosDevice_t *device);
mykonosErr_t MYKONOS_setRxAgcMinMaxGainIndex(mykonosDevice_t *device, mykonosRxChannels_t rxChannelSelect, uint8_t maxGainIndex, uint8_t minGainIndex);
mykonosErr_t MYKONOS_setObsRxAgcMinMaxGainIndex(mykonosDevice_t *device, mykonosObsRxChannels_t obsRxChannelSelect, uint8_t maxGainIndex, uint8_t minGainIndex);
mykonosErr_t MYKONOS_setRx1TempGainComp(mykonosDevice_t *device, int16_t rx1TempCompGain_mdB);
mykonosErr_t MYKONOS_getRx1TempGainComp(mykonosDevice_t *device, int16_t *rx1TempCompGain_mdB);
mykonosErr_t MYKONOS_setRx2TempGainComp(mykonosDevice_t *device, int16_t rx2TempCompGain_mdB);
mykonosErr_t MYKONOS_getRx2TempGainComp(mykonosDevice_t *device, int16_t *rx2TempCompGain_mdB);
mykonosErr_t MYKONOS_setObsRxTempGainComp(mykonosDevice_t *device, int16_t obsRxTempCompGain_mdB);
mykonosErr_t MYKONOS_getObsRxTempGainComp(mykonosDevice_t *device, int16_t *obsRxTempCompGain_mdB);
mykonosErr_t MYKONOS_enableRxGainCtrSyncPulse(mykonosDevice_t *device, uint8_t enable);
mykonosErr_t MYKONOS_setRxGainControlMode(mykonosDevice_t *device, mykonosGainMode_t mode);
mykonosErr_t MYKONOS_getRx1DecPower(mykonosDevice_t *device, uint16_t *rx1DecPower_mdBFS);
mykonosErr_t MYKONOS_getRx2DecPower(mykonosDevice_t *device, uint16_t *rx2DecPower_mdBFS);

/*
 *****************************************************************************
 * Observation Rx Data path functions
 *****************************************************************************
 */
mykonosErr_t MYKONOS_setDefaultObsRxPath(mykonosDevice_t *device,  mykonosObsRxChannels_t defaultObsRxCh);
mykonosErr_t MYKONOS_setObsRxPathSource(mykonosDevice_t *device, mykonosObsRxChannels_t obsRxCh);
mykonosErr_t MYKONOS_getObsRxPathSource(mykonosDevice_t *device, mykonosObsRxChannels_t *obsRxCh);
mykonosErr_t MYKONOS_setObsRxManualGain(mykonosDevice_t *device, mykonosObsRxChannels_t obsRxCh, uint8_t gainIndex);
mykonosErr_t MYKONOS_getObsRxGain(mykonosDevice_t *device, uint8_t *gainIndex);
mykonosErr_t MYKONOS_setupObsRxAgc(mykonosDevice_t *device);
mykonosErr_t MYKONOS_enableObsRxGainCtrSyncPulse(mykonosDevice_t *device, uint8_t enable);
mykonosErr_t MYKONOS_setObsRxGainControlMode(mykonosDevice_t *device, mykonosGainMode_t mode);
mykonosErr_t MYKONOS_getObsRxDecPower(mykonosDevice_t *device, uint16_t *obsRxDecPower_mdBFS);

/*
 *****************************************************************************
 * Tx Data path functions
 *****************************************************************************
 */
mykonosErr_t MYKONOS_setTx1Attenuation(mykonosDevice_t *device, uint16_t tx1Attenuation_mdB);
mykonosErr_t MYKONOS_setTx2Attenuation(mykonosDevice_t *device, uint16_t tx2Attenuation_mdB);
mykonosErr_t MYKONOS_getTx1Attenuation(mykonosDevice_t *device, uint16_t *tx1Attenuation_mdB);
mykonosErr_t MYKONOS_getTx2Attenuation(mykonosDevice_t *device, uint16_t *tx2Attenuation_mdB);
mykonosErr_t MYKONOS_getTxFilterOverRangeStatus(mykonosDevice_t *device, uint8_t *txFilterStatus);
mykonosErr_t MYKONOS_enableTxNco(mykonosDevice_t *device, uint8_t enable, int32_t tx1ToneFreq_kHz, int32_t tx2ToneFreq_kHz);

/*
 *****************************************************************************
 * PA Protection Functions
 *****************************************************************************
 */
mykonosErr_t MYKONOS_setupPaProtection(mykonosDevice_t *device, uint16_t powerThreshold, uint8_t attenStepSize, uint8_t avgDuration, uint8_t stickyFlagEnable, uint8_t txAttenControlEnable);
mykonosErr_t MYKONOS_enablePaProtection(mykonosDevice_t *device, uint8_t enable);
mykonosErr_t MYKONOS_getDacPower(mykonosDevice_t *device, mykonosTxChannels_t channel, uint16_t *channelPower);
mykonosErr_t MYKONOS_getPaProtectErrorFlagStatus(mykonosDevice_t *device, uint8_t *errorFlagStatus);
mykonosErr_t MYKONOS_clearPaErrorFlag(mykonosDevice_t *device);

/*
 *****************************************************************************
 * Low level functions and helper functions. BBP should not need to call
 *****************************************************************************
 */
const char* getMykonosErrorMessage(mykonosErr_t errorCode);

mykonosErr_t MYKONOS_initSubRegisterTables(mykonosDevice_t *device);
mykonosErr_t MYKONOS_setTxPfirSyncClk(mykonosDevice_t *device); /* Helper function */
mykonosErr_t MYKONOS_setRxPfirSyncClk(mykonosDevice_t *device); /* Helper function */

/*
 *****************************************************************************
 * Functions to configure DC offsets
 *****************************************************************************
 */
mykonosErr_t MYKONOS_setRfDcOffsetCnt(mykonosDevice_t *device, mykonosDcOffsetChannels_t channel, uint16_t measureCount);
mykonosErr_t MYKONOS_getRfDcOffsetCnt(mykonosDevice_t *device, mykonosDcOffsetChannels_t channel, uint16_t *measureCount);
mykonosErr_t MYKONOS_setDigDcOffsetMShift(mykonosDevice_t *device, mykonosDcOffsetChannels_t channel, uint8_t mShift);
mykonosErr_t MYKONOS_getDigDcOffsetMShift(mykonosDevice_t *device, mykonosDcOffsetChannels_t channel, uint8_t *mShift);
mykonosErr_t MYKONOS_setDigDcOffsetEn(mykonosDevice_t *device, uint8_t enableMask);
mykonosErr_t MYKONOS_getDigDcOffsetEn(mykonosDevice_t *device, uint8_t *enableMask);

#ifdef __cplusplus
}
#endif

#endif
