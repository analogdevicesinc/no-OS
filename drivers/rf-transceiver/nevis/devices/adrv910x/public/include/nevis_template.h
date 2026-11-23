#ifndef __ADI_NEVIS_TEMPLATE_H__
#define __ADI_NEVIS_TEMPLATE_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "device_profile_bundle_t.h"

deviceProfileBundle_t config_uc_id_0 = {
		.profile = {
		.ver = {
		.major = 12, 
		.minor = 0}
, 
		.vcoFreq_daHz = 0, 
		.hsDigFreq_Hz = 0, 
		.devClock_Hz = 0, 
		.ps1ArmClkDiv = 0, 
		.armPowerSavingClkDiv = 0, 
		.refClockOutEn = 0, 
		.clkPllPowerLevel = COMPONENT_POWER_LEVEL_LOW, 
		.clkGenDis = 0, 
		.clkDoublerEn = 0, 
		.ps2En = 0, 
		.ps2ArmClkDiv = 0, 
		.loPhaseSync = 0, 
		.loConfig = { {
		.extLoMode = LOGEN_INTERNAL_IN, 
		.extLoPllConnect = LOGEN_EXTLO_DIFFERENTIAL, 
		.extLoDiv = 0}, {
		.extLoMode = LOGEN_INTERNAL_IN, 
		.extLoPllConnect = LOGEN_EXTLO_DIFFERENTIAL, 
		.extLoDiv = 0}  }, 
		.chanConfig = 0, 
		.observationMixer = 0, 
		.padding = { 0, 0  }, 
		.txConfig = { {
		.primaryBw_Hz = 0, 
		.txInputRate = 0, 
		.txInterfaceSampleRate = 0, 
		.offsetLo_Hz = 0, 
		.outputSignalingSel = IQ, 
		.txBbfConfig = {
		.butterFilterBw = 0, 
		.txBbfPowerMode = COMPONENT_POWER_LEVEL_LOW, 
		.padding = { 0, 0, 0  }}
, 
		.txPredistConfig = {
		.pdBiasCurrent = 0, 
		.pdGainEn = 0, 
		.prePDRealPole = 0, 
		.postPDRealPole = 0, 
		.padding = { 0, 0  }}
, 
		.txElbConfig = {
		.extLoopBackType = 0, 
		.extLoopBackForInitCal = 0, 
		.peakLoopBackPower = 0}
, 
		.freqDev = 0, 
		.txSsiConfig = {
		.ssiType = SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = SSI_1_LANE, 
		.strobeType = SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = TX_SSI, 
		.lvdsRxClkInversionEn = 0, 
		.cmosTxDdrNegStrobeEn = 0, 
		.cmosDdrPosClkEn = 0, 
		.cmosClkInversionEn = 0, 
		.ddrEn = 0, 
		.rxMaskStrobeEn = 0, 
		.padding = { 0, 0, 0  }}
, 
		.txEnableDelay = {
		.enableRiseToOnDelay = 0, 
		.enableRiseToAnalogOnDelay = 0, 
		.enableFallToOffDelay = 0, 
		.enableGuardDelay = 0, 
		.enableHoldDelay = 0}
, 
		.LoSelect = LO_ID_START, 
		.padding = { 0, 0  }}, {
		.primaryBw_Hz = 0, 
		.txInputRate = 0, 
		.txInterfaceSampleRate = 0, 
		.offsetLo_Hz = 0, 
		.outputSignalingSel = IQ, 
		.txBbfConfig = {
		.butterFilterBw = 0, 
		.txBbfPowerMode = COMPONENT_POWER_LEVEL_LOW, 
		.padding = { 0, 0, 0  }}
, 
		.txPredistConfig = {
		.pdBiasCurrent = 0, 
		.pdGainEn = 0, 
		.prePDRealPole = 0, 
		.postPDRealPole = 0, 
		.padding = { 0, 0  }}
, 
		.txElbConfig = {
		.extLoopBackType = 0, 
		.extLoopBackForInitCal = 0, 
		.peakLoopBackPower = 0}
, 
		.freqDev = 0, 
		.txSsiConfig = {
		.ssiType = SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = SSI_1_LANE, 
		.strobeType = SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = TX_SSI, 
		.lvdsRxClkInversionEn = 0, 
		.cmosTxDdrNegStrobeEn = 0, 
		.cmosDdrPosClkEn = 0, 
		.cmosClkInversionEn = 0, 
		.ddrEn = 0, 
		.rxMaskStrobeEn = 0, 
		.padding = { 0, 0, 0  }}
, 
		.txEnableDelay = {
		.enableRiseToOnDelay = 0, 
		.enableRiseToAnalogOnDelay = 0, 
		.enableFallToOffDelay = 0, 
		.enableGuardDelay = 0, 
		.enableHoldDelay = 0}
, 
		.LoSelect = LO_ID_START, 
		.padding = { 0, 0  }}  }, 
		.rxConfig = { {
		.primaryBw_Hz = 0, 
		.rxOutputRate = 0, 
		.rxInterfaceSampleRate = 0, 
		.offsetLo_Hz = 0, 
		.ncoEn = 0, 
		.outputSignalingSel = RX_SIGNAL_TYPE_IQ, 
		.adcCorner = 0, 
		.adcClk_Hz = 0, 
		.corner3dB = 0, 
		.tiaPower = COMPONENT_POWER_LEVEL_LOW, 
		.rxSsiConfig = {
		.ssiType = SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = SSI_1_LANE, 
		.strobeType = SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = TX_SSI, 
		.lvdsRxClkInversionEn = 0, 
		.cmosTxDdrNegStrobeEn = 0, 
		.cmosDdrPosClkEn = 0, 
		.cmosClkInversionEn = 0, 
		.ddrEn = 0, 
		.rxMaskStrobeEn = 0, 
		.padding = { 0, 0, 0  }}
, 
		.rxEnableDelay = {
		.enableRiseToOnDelay = 0, 
		.enableRiseToAnalogOnDelay = 0, 
		.enableFallToOffDelay = 0, 
		.enableGuardDelay = 0, 
		.enableHoldDelay = 0}
, 
		.gainTableType = RX_GAIN_CORRECTION_TABLE, 
		.LoSelect = LO_ID_START}, {
		.primaryBw_Hz = 0, 
		.rxOutputRate = 0, 
		.rxInterfaceSampleRate = 0, 
		.offsetLo_Hz = 0, 
		.ncoEn = 0, 
		.outputSignalingSel = RX_SIGNAL_TYPE_IQ, 
		.adcCorner = 0, 
		.adcClk_Hz = 0, 
		.corner3dB = 0, 
		.tiaPower = COMPONENT_POWER_LEVEL_LOW, 
		.rxSsiConfig = {
		.ssiType = SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = SSI_1_LANE, 
		.strobeType = SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = TX_SSI, 
		.lvdsRxClkInversionEn = 0, 
		.cmosTxDdrNegStrobeEn = 0, 
		.cmosDdrPosClkEn = 0, 
		.cmosClkInversionEn = 0, 
		.ddrEn = 0, 
		.rxMaskStrobeEn = 0, 
		.padding = { 0, 0, 0  }}
, 
		.rxEnableDelay = {
		.enableRiseToOnDelay = 0, 
		.enableRiseToAnalogOnDelay = 0, 
		.enableFallToOffDelay = 0, 
		.enableGuardDelay = 0, 
		.enableHoldDelay = 0}
, 
		.gainTableType = RX_GAIN_CORRECTION_TABLE, 
		.LoSelect = LO_ID_START}  }, 
		.orxConfig = { {
		.primaryBw_Hz = 0, 
		.rxOutputRate = 0, 
		.rxInterfaceSampleRate = 0, 
		.offsetLo_Hz = 0, 
		.ncoEn = 0, 
		.outputSignalingSel = RX_SIGNAL_TYPE_IQ, 
		.adcCorner = 0, 
		.adcClk_Hz = 0, 
		.corner3dB = 0, 
		.tiaPower = COMPONENT_POWER_LEVEL_LOW, 
		.rxSsiConfig = {
		.ssiType = SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = SSI_1_LANE, 
		.strobeType = SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = TX_SSI, 
		.lvdsRxClkInversionEn = 0, 
		.cmosTxDdrNegStrobeEn = 0, 
		.cmosDdrPosClkEn = 0, 
		.cmosClkInversionEn = 0, 
		.ddrEn = 0, 
		.rxMaskStrobeEn = 0, 
		.padding = { 0, 0, 0  }}
, 
		.rxEnableDelay = {
		.enableRiseToOnDelay = 0, 
		.enableRiseToAnalogOnDelay = 0, 
		.enableFallToOffDelay = 0, 
		.enableGuardDelay = 0, 
		.enableHoldDelay = 0}
, 
		.gainTableType = RX_GAIN_CORRECTION_TABLE, 
		.LoSelect = LO_ID_START}, {
		.primaryBw_Hz = 0, 
		.rxOutputRate = 0, 
		.rxInterfaceSampleRate = 0, 
		.offsetLo_Hz = 0, 
		.ncoEn = 0, 
		.outputSignalingSel = RX_SIGNAL_TYPE_IQ, 
		.adcCorner = 0, 
		.adcClk_Hz = 0, 
		.corner3dB = 0, 
		.tiaPower = COMPONENT_POWER_LEVEL_LOW, 
		.rxSsiConfig = {
		.ssiType = SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = SSI_1_LANE, 
		.strobeType = SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = TX_SSI, 
		.lvdsRxClkInversionEn = 0, 
		.cmosTxDdrNegStrobeEn = 0, 
		.cmosDdrPosClkEn = 0, 
		.cmosClkInversionEn = 0, 
		.ddrEn = 0, 
		.rxMaskStrobeEn = 0, 
		.padding = { 0, 0, 0  }}
, 
		.rxEnableDelay = {
		.enableRiseToOnDelay = 0, 
		.enableRiseToAnalogOnDelay = 0, 
		.enableFallToOffDelay = 0, 
		.enableGuardDelay = 0, 
		.enableHoldDelay = 0}
, 
		.gainTableType = RX_GAIN_CORRECTION_TABLE, 
		.LoSelect = LO_ID_START}  }, 
		.ilbConfig = { {
		.primaryBw_Hz = 0, 
		.rxOutputRate = 0, 
		.rxInterfaceSampleRate = 0, 
		.offsetLo_Hz = 0, 
		.ncoEn = 0, 
		.outputSignalingSel = RX_SIGNAL_TYPE_IQ, 
		.adcCorner = 0, 
		.adcClk_Hz = 0, 
		.corner3dB = 0, 
		.tiaPower = COMPONENT_POWER_LEVEL_LOW, 
		.rxSsiConfig = {
		.ssiType = SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = SSI_1_LANE, 
		.strobeType = SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = TX_SSI, 
		.lvdsRxClkInversionEn = 0, 
		.cmosTxDdrNegStrobeEn = 0, 
		.cmosDdrPosClkEn = 0, 
		.cmosClkInversionEn = 0, 
		.ddrEn = 0, 
		.rxMaskStrobeEn = 0, 
		.padding = { 0, 0, 0  }}
, 
		.rxEnableDelay = {
		.enableRiseToOnDelay = 0, 
		.enableRiseToAnalogOnDelay = 0, 
		.enableFallToOffDelay = 0, 
		.enableGuardDelay = 0, 
		.enableHoldDelay = 0}
, 
		.gainTableType = RX_GAIN_CORRECTION_TABLE, 
		.LoSelect = LO_ID_START}, {
		.primaryBw_Hz = 0, 
		.rxOutputRate = 0, 
		.rxInterfaceSampleRate = 0, 
		.offsetLo_Hz = 0, 
		.ncoEn = 0, 
		.outputSignalingSel = RX_SIGNAL_TYPE_IQ, 
		.adcCorner = 0, 
		.adcClk_Hz = 0, 
		.corner3dB = 0, 
		.tiaPower = COMPONENT_POWER_LEVEL_LOW, 
		.rxSsiConfig = {
		.ssiType = SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = SSI_1_LANE, 
		.strobeType = SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = TX_SSI, 
		.lvdsRxClkInversionEn = 0, 
		.cmosTxDdrNegStrobeEn = 0, 
		.cmosDdrPosClkEn = 0, 
		.cmosClkInversionEn = 0, 
		.ddrEn = 0, 
		.rxMaskStrobeEn = 0, 
		.padding = { 0, 0, 0  }}
, 
		.rxEnableDelay = {
		.enableRiseToOnDelay = 0, 
		.enableRiseToAnalogOnDelay = 0, 
		.enableFallToOffDelay = 0, 
		.enableGuardDelay = 0, 
		.enableHoldDelay = 0}
, 
		.gainTableType = RX_GAIN_CORRECTION_TABLE, 
		.LoSelect = LO_ID_START}  }, 
		.elbConfig = { {
		.primaryBw_Hz = 0, 
		.rxOutputRate = 0, 
		.rxInterfaceSampleRate = 0, 
		.offsetLo_Hz = 0, 
		.ncoEn = 0, 
		.outputSignalingSel = RX_SIGNAL_TYPE_IQ, 
		.adcCorner = 0, 
		.adcClk_Hz = 0, 
		.corner3dB = 0, 
		.tiaPower = COMPONENT_POWER_LEVEL_LOW, 
		.rxSsiConfig = {
		.ssiType = SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = SSI_1_LANE, 
		.strobeType = SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = TX_SSI, 
		.lvdsRxClkInversionEn = 0, 
		.cmosTxDdrNegStrobeEn = 0, 
		.cmosDdrPosClkEn = 0, 
		.cmosClkInversionEn = 0, 
		.ddrEn = 0, 
		.rxMaskStrobeEn = 0, 
		.padding = { 0, 0, 0  }}
, 
		.rxEnableDelay = {
		.enableRiseToOnDelay = 0, 
		.enableRiseToAnalogOnDelay = 0, 
		.enableFallToOffDelay = 0, 
		.enableGuardDelay = 0, 
		.enableHoldDelay = 0}
, 
		.gainTableType = RX_GAIN_CORRECTION_TABLE, 
		.LoSelect = LO_ID_START}, {
		.primaryBw_Hz = 0, 
		.rxOutputRate = 0, 
		.rxInterfaceSampleRate = 0, 
		.offsetLo_Hz = 0, 
		.ncoEn = 0, 
		.outputSignalingSel = RX_SIGNAL_TYPE_IQ, 
		.adcCorner = 0, 
		.adcClk_Hz = 0, 
		.corner3dB = 0, 
		.tiaPower = COMPONENT_POWER_LEVEL_LOW, 
		.rxSsiConfig = {
		.ssiType = SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = SSI_1_LANE, 
		.strobeType = SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = TX_SSI, 
		.lvdsRxClkInversionEn = 0, 
		.cmosTxDdrNegStrobeEn = 0, 
		.cmosDdrPosClkEn = 0, 
		.cmosClkInversionEn = 0, 
		.ddrEn = 0, 
		.rxMaskStrobeEn = 0, 
		.padding = { 0, 0, 0  }}
, 
		.rxEnableDelay = {
		.enableRiseToOnDelay = 0, 
		.enableRiseToAnalogOnDelay = 0, 
		.enableFallToOffDelay = 0, 
		.enableGuardDelay = 0, 
		.enableHoldDelay = 0}
, 
		.gainTableType = RX_GAIN_CORRECTION_TABLE, 
		.LoSelect = LO_ID_START}  }, 
		.tx1Config = {
		.txDpProfile = {
		.preProc = {
		.symMapSymbol0 = 0, 
		.symMapSymbol1 = 0, 
		.symMapSymbol2 = 0, 
		.symMapSymbol3 = 0, 
		.symMapDivFactor = 0, 
		.symMapQVal = 0, 
		.preProcMode = TX_DP_PREPROC_I_ONLY_BYPASSED, 
		.symMapperMode = TX_DP_SYM_MAPPER_DISABLE, 
		.txPreProcWbNbPfirIBank = PFIR_BANK_A, 
		.txPreProcWbNbPfirQBank = PFIR_BANK_A, 
		.padding = { 0, 0  }}
, 
		.wbIntTop = {
		.interpBy2Blk30En = 0, 
		.interpBy2Blk28En = 0, 
		.interpBy2Blk26En = 0, 
		.interpBy2Blk24En = 0, 
		.interpBy2Blk22En = 0, 
		.wbLpfBlk22p1En = 0, 
		.padding = { 0, 0  }}
, 
		.nbIntTop = {
		.interpBy2Blk20En = 0, 
		.interpBy2Blk18En = 0, 
		.interpBy2Blk16En = 0, 
		.interpBy2Blk14En = 0, 
		.interpBy2Blk12En = 0, 
		.interpBy3Blk10En = 0, 
		.interpBy2Blk8En = 0, 
		.scicBlk32En = 0, 
		.scicBlk32DivFactor = 0, 
		.padding = { 0, 0, 0  }}
, 
		.intTop = {
		.interpBy3Blk44p1En = 0, 
		.sinc3Blk44En = 0, 
		.sinc2Blk42En = 0, 
		.interpBy3Blk40En = 0, 
		.interpBy3Blk40p1En = 0, 
		.interpBy2Blk38En = 0, 
		.interpBy2Blk36En = 0, 
		.interpBy2BlkEn = 0}
, 
		.intTopFreqDevMap = {
		.rrc2Frac = 0, 
		.mpll = 0, 
		.nchLsw = 0, 
		.nchMsb = 0, 
		.freqDevMapEn = 0, 
		.txRoundEn = 0, 
		.padding = { 0  }}
, 
		.iqdmDuc = {
		.iqdmDucMode = TX_DP_IQDMDUC_MODE0, 
		.padding = { 0, 0, 0  }, 
		.iqdmDev = 0, 
		.iqdmDevOffset = 0, 
		.iqdmScalar = 0, 
		.iqdmThreshold = 0, 
		.iqdmNco = {
		.freq = 0, 
		.sampleFreq = 0, 
		.phase = 0, 
		.realOut = 0, 
		.freeRunClkEnable = 0}
}
}
, 
		.reserved = { 0  }}
, 
		.rx1Config = {
		.rxDpProfile = {
		.nbDecTop = {
		.scicBlk23En = 0, 
		.scicBlk23DivFactor = 0, 
		.scicBlk23LowRippleEn = 0, 
		.decBy2Blk35En = 0, 
		.decBy2Blk37En = 0, 
		.decBy2Blk39En = 0, 
		.decBy2Blk41En = 0, 
		.decBy2Blk43En = 0, 
		.decBy3Blk45En = 0, 
		.decBy2Blk47En = 0, 
		.padding = { 0, 0  }}
, 
		.wbDecTop = {
		.decBy2Blk25En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 0, 
		.decBy2Blk31En = 0, 
		.decBy2Blk33En = 0, 
		.wbLpfBlk33p1En = 0, 
		.padding = { 0, 0  }}
, 
		.decTop = {
		.decBy3Blk15En = 0, 
		.decBy3Blk15_1En = 0, 
		.decBy2Hb3Blk17p1En = 0, 
		.decBy2Hb4Blk17p2En = 0, 
		.decBy2Hb5Blk19p1En = 0, 
		.decBy2Hb6Blk19p2En = 0, 
		.padding = { 0, 0  }}
, 
		.sincHbTop = {
		.sincGainMux = RX_SINC_GAIN_MUX_0_DB, 
		.sincMux = RX_SINC_MUX5_OUTPUT_ZERO, 
		.hbMux = RX_HB_MUX_BYPASS, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 0, 0, 0, 0, 0, 0  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  }}
, 
		.rxNbDem = {
		.rxNbNco = {
		.rxNbNcoEn = 0, 
		.padding = { 0, 0, 0  }, 
		.rxNbNcoConfig = {
		.freq = 0, 
		.sampleFreq = 0, 
		.phase = 0, 
		.realOut = 0, 
		.freeRunClkEnable = 0}
}
, 
		.rxWbNbCompPFir = {
		.rxWbNbCompPFirInMuxSel = RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0, 
		.rxWbNbCompPFirBankSel = PFIR_BANK_A, 
		.padding = { 0  }}
, 
		.reSamp = {
		.rxReSampEn = 0, 
		.padding = { 0, 0, 0  }, 
		.reSampPhaseI = 0, 
		.reSampPhaseQ = 0}
, 
		.gsOutMuxSel = GSOUT_MUX_GS_OUTPUT, 
		.dnSampleSel = NO_DN_SAMPLE, 
		.rxOutSel = RXOUTSEL_IQ, 
		.rxRoundMode = RXROUNDMODE_IQ}
}
, 
		.wbSelNbDemod = 0, 
		.reserved = { 0, 0, 0  }}
, 
		.orx1Config = {
		.rxDpProfile = {
		.nbDecTop = {
		.scicBlk23En = 0, 
		.scicBlk23DivFactor = 0, 
		.scicBlk23LowRippleEn = 0, 
		.decBy2Blk35En = 0, 
		.decBy2Blk37En = 0, 
		.decBy2Blk39En = 0, 
		.decBy2Blk41En = 0, 
		.decBy2Blk43En = 0, 
		.decBy3Blk45En = 0, 
		.decBy2Blk47En = 0, 
		.padding = { 0, 0  }}
, 
		.wbDecTop = {
		.decBy2Blk25En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 0, 
		.decBy2Blk31En = 0, 
		.decBy2Blk33En = 0, 
		.wbLpfBlk33p1En = 0, 
		.padding = { 0, 0  }}
, 
		.decTop = {
		.decBy3Blk15En = 0, 
		.decBy3Blk15_1En = 0, 
		.decBy2Hb3Blk17p1En = 0, 
		.decBy2Hb4Blk17p2En = 0, 
		.decBy2Hb5Blk19p1En = 0, 
		.decBy2Hb6Blk19p2En = 0, 
		.padding = { 0, 0  }}
, 
		.sincHbTop = {
		.sincGainMux = RX_SINC_GAIN_MUX_0_DB, 
		.sincMux = RX_SINC_MUX5_OUTPUT_ZERO, 
		.hbMux = RX_HB_MUX_BYPASS, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 0, 0, 0, 0, 0, 0  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  }}
, 
		.rxNbDem = {
		.rxNbNco = {
		.rxNbNcoEn = 0, 
		.padding = { 0, 0, 0  }, 
		.rxNbNcoConfig = {
		.freq = 0, 
		.sampleFreq = 0, 
		.phase = 0, 
		.realOut = 0, 
		.freeRunClkEnable = 0}
}
, 
		.rxWbNbCompPFir = {
		.rxWbNbCompPFirInMuxSel = RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0, 
		.rxWbNbCompPFirBankSel = PFIR_BANK_A, 
		.padding = { 0  }}
, 
		.reSamp = {
		.rxReSampEn = 0, 
		.padding = { 0, 0, 0  }, 
		.reSampPhaseI = 0, 
		.reSampPhaseQ = 0}
, 
		.gsOutMuxSel = GSOUT_MUX_GS_OUTPUT, 
		.dnSampleSel = NO_DN_SAMPLE, 
		.rxOutSel = RXOUTSEL_IQ, 
		.rxRoundMode = RXROUNDMODE_IQ}
}
, 
		.wbSelNbDemod = 0, 
		.reserved = { 0, 0, 0  }}
, 
		.ilb1Config = {
		.rxDpProfile = {
		.nbDecTop = {
		.scicBlk23En = 0, 
		.scicBlk23DivFactor = 0, 
		.scicBlk23LowRippleEn = 0, 
		.decBy2Blk35En = 0, 
		.decBy2Blk37En = 0, 
		.decBy2Blk39En = 0, 
		.decBy2Blk41En = 0, 
		.decBy2Blk43En = 0, 
		.decBy3Blk45En = 0, 
		.decBy2Blk47En = 0, 
		.padding = { 0, 0  }}
, 
		.wbDecTop = {
		.decBy2Blk25En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 0, 
		.decBy2Blk31En = 0, 
		.decBy2Blk33En = 0, 
		.wbLpfBlk33p1En = 0, 
		.padding = { 0, 0  }}
, 
		.decTop = {
		.decBy3Blk15En = 0, 
		.decBy3Blk15_1En = 0, 
		.decBy2Hb3Blk17p1En = 0, 
		.decBy2Hb4Blk17p2En = 0, 
		.decBy2Hb5Blk19p1En = 0, 
		.decBy2Hb6Blk19p2En = 0, 
		.padding = { 0, 0  }}
, 
		.sincHbTop = {
		.sincGainMux = RX_SINC_GAIN_MUX_0_DB, 
		.sincMux = RX_SINC_MUX5_OUTPUT_ZERO, 
		.hbMux = RX_HB_MUX_BYPASS, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 0, 0, 0, 0, 0, 0  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  }}
, 
		.rxNbDem = {
		.rxNbNco = {
		.rxNbNcoEn = 0, 
		.padding = { 0, 0, 0  }, 
		.rxNbNcoConfig = {
		.freq = 0, 
		.sampleFreq = 0, 
		.phase = 0, 
		.realOut = 0, 
		.freeRunClkEnable = 0}
}
, 
		.rxWbNbCompPFir = {
		.rxWbNbCompPFirInMuxSel = RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0, 
		.rxWbNbCompPFirBankSel = PFIR_BANK_A, 
		.padding = { 0  }}
, 
		.reSamp = {
		.rxReSampEn = 0, 
		.padding = { 0, 0, 0  }, 
		.reSampPhaseI = 0, 
		.reSampPhaseQ = 0}
, 
		.gsOutMuxSel = GSOUT_MUX_GS_OUTPUT, 
		.dnSampleSel = NO_DN_SAMPLE, 
		.rxOutSel = RXOUTSEL_IQ, 
		.rxRoundMode = RXROUNDMODE_IQ}
}
, 
		.wbSelNbDemod = 0, 
		.reserved = { 0, 0, 0  }}
, 
		.elb1Config = {
		.rxDpProfile = {
		.nbDecTop = {
		.scicBlk23En = 0, 
		.scicBlk23DivFactor = 0, 
		.scicBlk23LowRippleEn = 0, 
		.decBy2Blk35En = 0, 
		.decBy2Blk37En = 0, 
		.decBy2Blk39En = 0, 
		.decBy2Blk41En = 0, 
		.decBy2Blk43En = 0, 
		.decBy3Blk45En = 0, 
		.decBy2Blk47En = 0, 
		.padding = { 0, 0  }}
, 
		.wbDecTop = {
		.decBy2Blk25En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 0, 
		.decBy2Blk31En = 0, 
		.decBy2Blk33En = 0, 
		.wbLpfBlk33p1En = 0, 
		.padding = { 0, 0  }}
, 
		.decTop = {
		.decBy3Blk15En = 0, 
		.decBy3Blk15_1En = 0, 
		.decBy2Hb3Blk17p1En = 0, 
		.decBy2Hb4Blk17p2En = 0, 
		.decBy2Hb5Blk19p1En = 0, 
		.decBy2Hb6Blk19p2En = 0, 
		.padding = { 0, 0  }}
, 
		.sincHbTop = {
		.sincGainMux = RX_SINC_GAIN_MUX_0_DB, 
		.sincMux = RX_SINC_MUX5_OUTPUT_ZERO, 
		.hbMux = RX_HB_MUX_BYPASS, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 0, 0, 0, 0, 0, 0  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  }}
, 
		.rxNbDem = {
		.rxNbNco = {
		.rxNbNcoEn = 0, 
		.padding = { 0, 0, 0  }, 
		.rxNbNcoConfig = {
		.freq = 0, 
		.sampleFreq = 0, 
		.phase = 0, 
		.realOut = 0, 
		.freeRunClkEnable = 0}
}
, 
		.rxWbNbCompPFir = {
		.rxWbNbCompPFirInMuxSel = RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0, 
		.rxWbNbCompPFirBankSel = PFIR_BANK_A, 
		.padding = { 0  }}
, 
		.reSamp = {
		.rxReSampEn = 0, 
		.padding = { 0, 0, 0  }, 
		.reSampPhaseI = 0, 
		.reSampPhaseQ = 0}
, 
		.gsOutMuxSel = GSOUT_MUX_GS_OUTPUT, 
		.dnSampleSel = NO_DN_SAMPLE, 
		.rxOutSel = RXOUTSEL_IQ, 
		.rxRoundMode = RXROUNDMODE_IQ}
}
, 
		.wbSelNbDemod = 0, 
		.reserved = { 0, 0, 0  }}
, 
		.txNbConfig = {
		.txNbDpProfile = {
		.preProc = {
		.symMapSymbol0 = 0, 
		.symMapSymbol1 = 0, 
		.symMapSymbol2 = 0, 
		.symMapSymbol3 = 0, 
		.symMapDivFactor = 0, 
		.symMapQVal = 0, 
		.preProcMode = TX_DP_PREPROC_I_ONLY_BYPASSED, 
		.symMapperMode = TX_DP_SYM_MAPPER_DISABLE, 
		.txPreProcWbNbPfirIBank = PFIR_BANK_A, 
		.txPreProcWbNbPfirQBank = PFIR_BANK_A, 
		.padding = { 0, 0  }}
, 
		.nbIntTopNew = {
		.interpBy5_2En = 0, 
		.interpBy5_1En = 0, 
		.interpBy3_1En = 0, 
		.interpBy2_6En = 0, 
		.interpBy2_5En = 0, 
		.interpBy2_4En = 0, 
		.interpBy2_3En = 0, 
		.interpBy2_2En = 0, 
		.interpBy2_1En = 0, 
		.scicNbBlk32En = 0, 
		.scicNbBlk32DivFactor = 0, 
		.padding = 0}
, 
		.intTop = {
		.interpBy3Blk44p1En = 0, 
		.sinc3Blk44En = 0, 
		.sinc2Blk42En = 0, 
		.interpBy3Blk40En = 0, 
		.interpBy3Blk40p1En = 0, 
		.interpBy2Blk38En = 0, 
		.interpBy2Blk36En = 0, 
		.interpBy2BlkEn = 0}
, 
		.intTopFreqDevMap = {
		.rrc2Frac = 0, 
		.mpll = 0, 
		.nchLsw = 0, 
		.nchMsb = 0, 
		.freqDevMapEn = 0, 
		.txRoundEn = 0, 
		.padding = { 0  }}
, 
		.iqdmDuc = {
		.iqdmDucMode = TX_DP_IQDMDUC_MODE0, 
		.padding = { 0, 0, 0  }, 
		.iqdmDev = 0, 
		.iqdmDevOffset = 0, 
		.iqdmScalar = 0, 
		.iqdmThreshold = 0, 
		.iqdmNco = {
		.freq = 0, 
		.sampleFreq = 0, 
		.phase = 0, 
		.realOut = 0, 
		.freeRunClkEnable = 0}
}
}
, 
		.reserved = { 0  }}
, 
		.rxNbConfig = {
		.rxNbDpProfile = {
		.nbDecTopNb = {
		.scicEn = 0, 
		.scicDivFactor = 0, 
		.rxDpScicControl = 0, 
		.decBy5Blk14En = 0, 
		.decBy5Blk16En = 0, 
		.decBy3Blk18En = 0, 
		.decBy2Blk20En = 0, 
		.decBy2Blk22En = 0, 
		.decBy2Blk24En = 0, 
		.decBy2Blk26En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 0}
, 
		.sincHbTopNb = {
		.sincGainMux = RX_SINC_GAIN_MUX_0_DB, 
		.sincMux = RXNB_SINC_MUX5_OUTPUT_ZERO, 
		.hbMux = RXNB_HB_MUX_BYPASS, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 0, 0, 0, 0, 0, 0  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  }}
, 
		.rxNbDemNb = {
		.wbPthSel = NB_PTH, 
		.dp3Sel = DP1_IQ22, 
		.corrFifoDelayEn = DTH_DLY_EN, 
		.corrSelDp2En = 0, 
		.dpInFifo = {
		.dpInFifoTestDataSel = DPINFIFOINPUTSEL_DP, 
		.dpInFifoEn = 0, 
		.dpInFifoMode = DPINFIFOMODE_DETECTING, 
		.padding = { 0  }}
, 
		.rxNbNco = {
		.rxNbNcoEn = 0, 
		.padding = { 0, 0, 0  }, 
		.rxNbNcoConfig = {
		.freq = 0, 
		.sampleFreq = 0, 
		.phase = 0, 
		.realOut = 0, 
		.freeRunClkEnable = 0}
}
, 
		.rxWbNbCompPFir = { {
		.rxWbNbCompPFirInMuxSel = RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0, 
		.rxWbNbCompPFirBankSel = PFIR_BANK_A, 
		.padding = { 0  }}, {
		.rxWbNbCompPFirInMuxSel = RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0, 
		.rxWbNbCompPFirBankSel = PFIR_BANK_A, 
		.padding = { 0  }}  }, 
		.reSamp = { {
		.rxReSampEn = 0, 
		.padding = { 0, 0, 0  }, 
		.reSampPhaseI = 0, 
		.reSampPhaseQ = 0}, {
		.rxReSampEn = 0, 
		.padding = { 0, 0, 0  }, 
		.reSampPhaseI = 0, 
		.reSampPhaseQ = 0}  }, 
		.gsOutMuxSel = { GSOUT_MUX_GS_OUTPUT, GSOUT_MUX_GS_OUTPUT  }, 
		.dnSampleSel = { NO_DN_SAMPLE, NO_DN_SAMPLE  }, 
		.FDSampleDelay = { 0, 0  }, 
		.pulPfirDelay = { 0, 0  }, 
		.rxOutSel = { RXOUTSEL_IQ, RXOUTSEL_IQ  }, 
		.rxRoundMode = { RXROUNDMODE_IQ, RXROUNDMODE_IQ  }, 
		.freqDeviation = { 0, 0  }, 
		.symbolRate = { 0, 0  }, 
		.dpArmSel = DPARMSEL_DP, 
		.dpEnabled = { 0, 0  }, 
		.padding = { 0, 0  }}
}
, 
		.adcOption = Default, 
		.gainControl = {
		.rxNbAgcHighThresholdModeEnable = false, 
		.padding = { 0, 0, 0  }, 
		.agcHighThresholdSettings = {
		.ApdPeakOverloadThresh = 0, 
		.ApdPeakUnderloadThresh = 0, 
		.Hb2PeakOverloadThresh = 0, 
		.Hb2PeakUnderloadThresh = 0, 
		.Hb2SecondaryUpperThresh = 0, 
		.padding = { 0, 0  }}
}
}
, 
		.orxNbConfig = {
		.rxNbDpProfile = {
		.nbDecTopNb = {
		.scicEn = 0, 
		.scicDivFactor = 0, 
		.rxDpScicControl = 0, 
		.decBy5Blk14En = 0, 
		.decBy5Blk16En = 0, 
		.decBy3Blk18En = 0, 
		.decBy2Blk20En = 0, 
		.decBy2Blk22En = 0, 
		.decBy2Blk24En = 0, 
		.decBy2Blk26En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 0}
, 
		.sincHbTopNb = {
		.sincGainMux = RX_SINC_GAIN_MUX_0_DB, 
		.sincMux = RXNB_SINC_MUX5_OUTPUT_ZERO, 
		.hbMux = RXNB_HB_MUX_BYPASS, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 0, 0, 0, 0, 0, 0  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  }}
, 
		.rxNbDemNb = {
		.wbPthSel = NB_PTH, 
		.dp3Sel = DP1_IQ22, 
		.corrFifoDelayEn = DTH_DLY_EN, 
		.corrSelDp2En = 0, 
		.dpInFifo = {
		.dpInFifoTestDataSel = DPINFIFOINPUTSEL_DP, 
		.dpInFifoEn = 0, 
		.dpInFifoMode = DPINFIFOMODE_DETECTING, 
		.padding = { 0  }}
, 
		.rxNbNco = {
		.rxNbNcoEn = 0, 
		.padding = { 0, 0, 0  }, 
		.rxNbNcoConfig = {
		.freq = 0, 
		.sampleFreq = 0, 
		.phase = 0, 
		.realOut = 0, 
		.freeRunClkEnable = 0}
}
, 
		.rxWbNbCompPFir = { {
		.rxWbNbCompPFirInMuxSel = RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0, 
		.rxWbNbCompPFirBankSel = PFIR_BANK_A, 
		.padding = { 0  }}, {
		.rxWbNbCompPFirInMuxSel = RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0, 
		.rxWbNbCompPFirBankSel = PFIR_BANK_A, 
		.padding = { 0  }}  }, 
		.reSamp = { {
		.rxReSampEn = 0, 
		.padding = { 0, 0, 0  }, 
		.reSampPhaseI = 0, 
		.reSampPhaseQ = 0}, {
		.rxReSampEn = 0, 
		.padding = { 0, 0, 0  }, 
		.reSampPhaseI = 0, 
		.reSampPhaseQ = 0}  }, 
		.gsOutMuxSel = { GSOUT_MUX_GS_OUTPUT, GSOUT_MUX_GS_OUTPUT  }, 
		.dnSampleSel = { NO_DN_SAMPLE, NO_DN_SAMPLE  }, 
		.FDSampleDelay = { 0, 0  }, 
		.pulPfirDelay = { 0, 0  }, 
		.rxOutSel = { RXOUTSEL_IQ, RXOUTSEL_IQ  }, 
		.rxRoundMode = { RXROUNDMODE_IQ, RXROUNDMODE_IQ  }, 
		.freqDeviation = { 0, 0  }, 
		.symbolRate = { 0, 0  }, 
		.dpArmSel = DPARMSEL_DP, 
		.dpEnabled = { 0, 0  }, 
		.padding = { 0, 0  }}
}
, 
		.adcOption = Default, 
		.gainControl = {
		.rxNbAgcHighThresholdModeEnable = false, 
		.padding = { 0, 0, 0  }, 
		.agcHighThresholdSettings = {
		.ApdPeakOverloadThresh = 0, 
		.ApdPeakUnderloadThresh = 0, 
		.Hb2PeakOverloadThresh = 0, 
		.Hb2PeakUnderloadThresh = 0, 
		.Hb2SecondaryUpperThresh = 0, 
		.padding = { 0, 0  }}
}
}
, 
		.ilbNbConfig = {
		.rxNbDpProfile = {
		.nbDecTopNb = {
		.scicEn = 0, 
		.scicDivFactor = 0, 
		.rxDpScicControl = 0, 
		.decBy5Blk14En = 0, 
		.decBy5Blk16En = 0, 
		.decBy3Blk18En = 0, 
		.decBy2Blk20En = 0, 
		.decBy2Blk22En = 0, 
		.decBy2Blk24En = 0, 
		.decBy2Blk26En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 0}
, 
		.sincHbTopNb = {
		.sincGainMux = RX_SINC_GAIN_MUX_0_DB, 
		.sincMux = RXNB_SINC_MUX5_OUTPUT_ZERO, 
		.hbMux = RXNB_HB_MUX_BYPASS, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 0, 0, 0, 0, 0, 0  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  }}
, 
		.rxNbDemNb = {
		.wbPthSel = NB_PTH, 
		.dp3Sel = DP1_IQ22, 
		.corrFifoDelayEn = DTH_DLY_EN, 
		.corrSelDp2En = 0, 
		.dpInFifo = {
		.dpInFifoTestDataSel = DPINFIFOINPUTSEL_DP, 
		.dpInFifoEn = 0, 
		.dpInFifoMode = DPINFIFOMODE_DETECTING, 
		.padding = { 0  }}
, 
		.rxNbNco = {
		.rxNbNcoEn = 0, 
		.padding = { 0, 0, 0  }, 
		.rxNbNcoConfig = {
		.freq = 0, 
		.sampleFreq = 0, 
		.phase = 0, 
		.realOut = 0, 
		.freeRunClkEnable = 0}
}
, 
		.rxWbNbCompPFir = { {
		.rxWbNbCompPFirInMuxSel = RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0, 
		.rxWbNbCompPFirBankSel = PFIR_BANK_A, 
		.padding = { 0  }}, {
		.rxWbNbCompPFirInMuxSel = RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0, 
		.rxWbNbCompPFirBankSel = PFIR_BANK_A, 
		.padding = { 0  }}  }, 
		.reSamp = { {
		.rxReSampEn = 0, 
		.padding = { 0, 0, 0  }, 
		.reSampPhaseI = 0, 
		.reSampPhaseQ = 0}, {
		.rxReSampEn = 0, 
		.padding = { 0, 0, 0  }, 
		.reSampPhaseI = 0, 
		.reSampPhaseQ = 0}  }, 
		.gsOutMuxSel = { GSOUT_MUX_GS_OUTPUT, GSOUT_MUX_GS_OUTPUT  }, 
		.dnSampleSel = { NO_DN_SAMPLE, NO_DN_SAMPLE  }, 
		.FDSampleDelay = { 0, 0  }, 
		.pulPfirDelay = { 0, 0  }, 
		.rxOutSel = { RXOUTSEL_IQ, RXOUTSEL_IQ  }, 
		.rxRoundMode = { RXROUNDMODE_IQ, RXROUNDMODE_IQ  }, 
		.freqDeviation = { 0, 0  }, 
		.symbolRate = { 0, 0  }, 
		.dpArmSel = DPARMSEL_DP, 
		.dpEnabled = { 0, 0  }, 
		.padding = { 0, 0  }}
}
, 
		.adcOption = Default, 
		.gainControl = {
		.rxNbAgcHighThresholdModeEnable = false, 
		.padding = { 0, 0, 0  }, 
		.agcHighThresholdSettings = {
		.ApdPeakOverloadThresh = 0, 
		.ApdPeakUnderloadThresh = 0, 
		.Hb2PeakOverloadThresh = 0, 
		.Hb2PeakUnderloadThresh = 0, 
		.Hb2SecondaryUpperThresh = 0, 
		.padding = { 0, 0  }}
}
}
, 
		.elbNbConfig = {
		.rxNbDpProfile = {
		.nbDecTopNb = {
		.scicEn = 0, 
		.scicDivFactor = 0, 
		.rxDpScicControl = 0, 
		.decBy5Blk14En = 0, 
		.decBy5Blk16En = 0, 
		.decBy3Blk18En = 0, 
		.decBy2Blk20En = 0, 
		.decBy2Blk22En = 0, 
		.decBy2Blk24En = 0, 
		.decBy2Blk26En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 0}
, 
		.sincHbTopNb = {
		.sincGainMux = RX_SINC_GAIN_MUX_0_DB, 
		.sincMux = RXNB_SINC_MUX5_OUTPUT_ZERO, 
		.hbMux = RXNB_HB_MUX_BYPASS, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 0, 0, 0, 0, 0, 0  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  }}
, 
		.rxNbDemNb = {
		.wbPthSel = NB_PTH, 
		.dp3Sel = DP1_IQ22, 
		.corrFifoDelayEn = DTH_DLY_EN, 
		.corrSelDp2En = 0, 
		.dpInFifo = {
		.dpInFifoTestDataSel = DPINFIFOINPUTSEL_DP, 
		.dpInFifoEn = 0, 
		.dpInFifoMode = DPINFIFOMODE_DETECTING, 
		.padding = { 0  }}
, 
		.rxNbNco = {
		.rxNbNcoEn = 0, 
		.padding = { 0, 0, 0  }, 
		.rxNbNcoConfig = {
		.freq = 0, 
		.sampleFreq = 0, 
		.phase = 0, 
		.realOut = 0, 
		.freeRunClkEnable = 0}
}
, 
		.rxWbNbCompPFir = { {
		.rxWbNbCompPFirInMuxSel = RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0, 
		.rxWbNbCompPFirBankSel = PFIR_BANK_A, 
		.padding = { 0  }}, {
		.rxWbNbCompPFirInMuxSel = RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0, 
		.rxWbNbCompPFirBankSel = PFIR_BANK_A, 
		.padding = { 0  }}  }, 
		.reSamp = { {
		.rxReSampEn = 0, 
		.padding = { 0, 0, 0  }, 
		.reSampPhaseI = 0, 
		.reSampPhaseQ = 0}, {
		.rxReSampEn = 0, 
		.padding = { 0, 0, 0  }, 
		.reSampPhaseI = 0, 
		.reSampPhaseQ = 0}  }, 
		.gsOutMuxSel = { GSOUT_MUX_GS_OUTPUT, GSOUT_MUX_GS_OUTPUT  }, 
		.dnSampleSel = { NO_DN_SAMPLE, NO_DN_SAMPLE  }, 
		.FDSampleDelay = { 0, 0  }, 
		.pulPfirDelay = { 0, 0  }, 
		.rxOutSel = { RXOUTSEL_IQ, RXOUTSEL_IQ  }, 
		.rxRoundMode = { RXROUNDMODE_IQ, RXROUNDMODE_IQ  }, 
		.freqDeviation = { 0, 0  }, 
		.symbolRate = { 0, 0  }, 
		.dpArmSel = DPARMSEL_DP, 
		.dpEnabled = { 0, 0  }, 
		.padding = { 0, 0  }}
}
, 
		.adcOption = Default, 
		.gainControl = {
		.rxNbAgcHighThresholdModeEnable = false, 
		.padding = { 0, 0, 0  }, 
		.agcHighThresholdSettings = {
		.ApdPeakOverloadThresh = 0, 
		.ApdPeakUnderloadThresh = 0, 
		.Hb2PeakOverloadThresh = 0, 
		.Hb2PeakUnderloadThresh = 0, 
		.Hb2SecondaryUpperThresh = 0, 
		.padding = { 0, 0  }}
}
}
, 
		.sysConfig = {
		.duplexMode = TDD, 
		.fhModeOn = 0, 
		.numDynamicProfile = 0, 
		.mcsMode = MCS_DISABLE, 
		.pllLockTime_us = 0, 
		.pllModuli = {
		.modulus = { 0, 0, 0, 0, 0  }, 
		.dmModulus = { 0, 0, 0, 0  }}
, 
		.pllPhaseSyncWait_us = 0, 
		.mcsInterfaceType = MCS_INTF_DISABLE, 
		.warmBootOn = 0, 
		.padding = { 0, 0  }}
, 
		.checksum = 0}
, 
		.pfirBuffer = {
		.pfirRxWbNbChFilterCoeff_A = {
		.numCoeff = 0, 
		.symmetricSel = PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = PFIR_32_TAPS, 
		.gainSel = PFIR_GAIN_MINUS_12DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirRxWbNbChFilterCoeff_B = {
		.numCoeff = 0, 
		.symmetricSel = PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = PFIR_32_TAPS, 
		.gainSel = PFIR_GAIN_MINUS_12DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirRxWbNbChFilterCoeff_C = {
		.numCoeff = 0, 
		.symmetricSel = PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = PFIR_32_TAPS, 
		.gainSel = PFIR_GAIN_MINUS_12DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirRxWbNbChFilterCoeff_D = {
		.numCoeff = 0, 
		.symmetricSel = PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = PFIR_32_TAPS, 
		.gainSel = PFIR_GAIN_MINUS_12DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirRxWbNbChFilterCoeff_E = {
		.numCoeff = 0, 
		.symmetricSel = PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = PFIR_32_TAPS, 
		.gainSel = PFIR_GAIN_MINUS_12DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirRxWbNbChFilterCoeff_F = {
		.numCoeff = 0, 
		.symmetricSel = PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = PFIR_32_TAPS, 
		.gainSel = PFIR_GAIN_MINUS_12DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirTxWbNbPulShpCoeff_A = {
		.numCoeff = 0, 
		.symmetricSel = PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = PFIR_32_TAPS, 
		.gainSel = PFIR_GAIN_MINUS_12DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirTxWbNbPulShpCoeff_B = {
		.numCoeff = 0, 
		.symmetricSel = PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = PFIR_32_TAPS, 
		.gainSel = PFIR_GAIN_MINUS_12DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirTxWbNbPulShpCoeff_C = {
		.numCoeff = 0, 
		.symmetricSel = PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = PFIR_32_TAPS, 
		.gainSel = PFIR_GAIN_MINUS_12DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirTxWbNbPulShpCoeff_D = {
		.numCoeff = 0, 
		.symmetricSel = PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = PFIR_32_TAPS, 
		.gainSel = PFIR_GAIN_MINUS_12DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirRxNbPulShpDp1 = { {
		.numCoeff = 0, 
		.symmetricSel = PFIR_COEF_NON_SYMMETRIC, 
		.taps = 0, 
		.gainSel = PFIR_GAIN_MINUS_12DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}, {
		.numCoeff = 0, 
		.symmetricSel = PFIR_COEF_NON_SYMMETRIC, 
		.taps = 0, 
		.gainSel = PFIR_GAIN_MINUS_12DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}  }, 
		.pfirRxNbPulShpDp2 = { {
		.numCoeff = 0, 
		.symmetricSel = PFIR_COEF_NON_SYMMETRIC, 
		.taps = 0, 
		.gainSel = PFIR_GAIN_MINUS_12DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}, {
		.numCoeff = 0, 
		.symmetricSel = PFIR_COEF_NON_SYMMETRIC, 
		.taps = 0, 
		.gainSel = PFIR_GAIN_MINUS_12DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}  }, 
		.pfirRxMagLowTiaLowSR_RX1 = {
		.numCoeff = 0, 
		.padding = { 0, 0, 0  }, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirRxMagLowTiaHighSR_RX1 = {
		.numCoeff = 0, 
		.padding = { 0, 0, 0  }, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirRxMagHighTiaHighSR_RX1 = {
		.numCoeff = 0, 
		.padding = { 0, 0, 0  }, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirRxMagLowTia_RXNB = {
		.numCoeff = 0, 
		.padding = { 0, 0, 0  }, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirRxMagHighTia_RXNB = {
		.numCoeff = 0, 
		.padding = { 0, 0, 0  }, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirTxMagComp1 = {
		.numCoeff = 0, 
		.padding = { 0, 0, 0  }, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirTxMagComp2 = {
		.numCoeff = 0, 
		.padding = { 0, 0, 0  }, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.pfirTxMagCompNb = { {
		.numCoeff = 0, 
		.padding = { 0, 0, 0  }, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}, {
		.numCoeff = 0, 
		.padding = { 0, 0, 0  }, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}  }, 
		.pfirRxMagCompNb = { {
		.numCoeff = 0, 
		.padding = { 0, 0, 0  }, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}, {
		.numCoeff = 0, 
		.padding = { 0, 0, 0  }, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}  }, 
		.pfirRxNbHb2Mitigation = {
		.numCoeff = 0, 
		.padding = { 0, 0, 0  }, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }}
, 
		.checksum = 0}
};

#endif /* __ADI_NEVIS_TEMPLATE_H__ */
