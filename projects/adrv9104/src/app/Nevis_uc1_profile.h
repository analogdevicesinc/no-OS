#include "device_profile_bundle_t.h"

deviceProfile_t usecase_01_0 = {
		.ver = {
		.major = 12, 
		.minor = 0}
, 
		.vcoFreq_daHz = 921600000, 
		.hsDigFreq_Hz = 1152000000, 
		.devClock_Hz = 38400000, 
		.ps1ArmClkDiv = 0, 
		.armPowerSavingClkDiv = 0, 
		.refClockOutEn = 1, 
		.clkPllPowerLevel = componentPowerLevel_e::COMPONENT_POWER_LEVEL_LOW,
		.clkGenDis = 0, 
		.clkDoublerEn = 0, 
		.ps2En = 0, 
		.ps2ArmClkDiv = 0, 
		.loPhaseSync = 0, 
		.loConfig = { {
		.extLoMode = loGenInMode_e::LOGEN_INTERNAL_IN, 
		.extLoPllConnect = loGenExtPllConnect_e::LOGEN_EXTLO_DIFFERENTIAL, 
		.extLoDiv = 0}, {
		.extLoMode = loGenInMode_e::LOGEN_INTERNAL_IN, 
		.extLoPllConnect = loGenExtPllConnect_e::LOGEN_EXTLO_DIFFERENTIAL, 
		.extLoDiv = 0}  }, 
		.chanConfig = 138, 
		.observationMixer = 1, 
		.padding = { 0, 0  }, 
		.txConfig = { {
		.primaryBw_Hz = 0, 
		.txInputRate = 0, 
		.txInterfaceSampleRate = 0, 
		.offsetLo_Hz = 0, 
		.outputSignalingSel = signalType_e::IQ, 
		.txBbfConfig = {
		.butterFilterBw = 0, 
		.txBbfPowerMode = componentPowerLevel_e::COMPONENT_POWER_LEVEL_LOW, 
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
		.ssiType = ssiType_e::SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = ssiDataFormat_e::SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = ssiNumLane_e::SSI_1_LANE, 
		.strobeType = ssiStrobeType_e::SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ssiTxRefClockPin_e::TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = rxSsiDataSrc_e::SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = txDataSrc_e::TX_SSI, 
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
		.LoSelect = LoId_e::LO_ID_LO1, 
		.padding = { 0, 0  }}, {
		.primaryBw_Hz = 12000, 
		.txInputRate = 24000, 
		.txInterfaceSampleRate = 24000, 
		.offsetLo_Hz = 0, 
		.outputSignalingSel = signalType_e::IQ, 
		.txBbfConfig = {
		.butterFilterBw = 1000000, 
		.txBbfPowerMode = componentPowerLevel_e::COMPONENT_POWER_LEVEL_HIGH, 
		.padding = { 0, 0, 0  }}
, 
		.txPredistConfig = {
		.pdBiasCurrent = 1, 
		.pdGainEn = 0, 
		.prePDRealPole = 1000000000, 
		.postPDRealPole = 530000000, 
		.padding = { 0, 0  }}
, 
		.txElbConfig = {
		.extLoopBackType = 0, 
		.extLoopBackForInitCal = 0, 
		.peakLoopBackPower = -180}
, 
		.freqDev = 648, 
		.txSsiConfig = {
		.ssiType = ssiType_e::SSI_TYPE_CMOS, 
		.ssiDataFormatSel = ssiDataFormat_e::SSI_FORMAT_16_BIT_I_Q_DATA, 
		.numLaneSel = ssiNumLane_e::SSI_1_LANE, 
		.strobeType = ssiStrobeType_e::SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ssiTxRefClockPin_e::TX_REF_CLOCK_PIN_CMOS_DCLK_OUT_P_ENABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = rxSsiDataSrc_e::SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = txDataSrc_e::TX_SSI, 
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
		.LoSelect = LoId_e::LO_ID_LO1, 
		.padding = { 0, 0  }}  }, 
		.rxConfig = { {
		.primaryBw_Hz = 0, 
		.rxOutputRate = 0, 
		.rxInterfaceSampleRate = 0, 
		.offsetLo_Hz = 0, 
		.ncoEn = 0, 
		.outputSignalingSel = rxSignalType_e::RX_SIGNAL_TYPE_IQ, 
		.adcCorner = 0, 
		.adcClk_Hz = 0, 
		.corner3dB = 0, 
		.tiaPower = componentPowerLevel_e::COMPONENT_POWER_LEVEL_LOW, 
		.rxSsiConfig = {
		.ssiType = ssiType_e::SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = ssiDataFormat_e::SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = ssiNumLane_e::SSI_1_LANE, 
		.strobeType = ssiStrobeType_e::SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ssiTxRefClockPin_e::TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = rxSsiDataSrc_e::SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = txDataSrc_e::TX_SSI, 
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
		.gainTableType = rxGainTableType_e::RX_GAIN_CORRECTION_TABLE, 
		.LoSelect = LoId_e::LO_ID_LO1}, {
		.primaryBw_Hz = 12000, 
		.rxOutputRate = 24000, 
		.rxInterfaceSampleRate = 24000, 
		.offsetLo_Hz = 442000, 
		.ncoEn = 1, 
		.outputSignalingSel = rxSignalType_e::RX_SIGNAL_TYPE_IQ, 
		.adcCorner = 480000, 
		.adcClk_Hz = 38400000, 
		.corner3dB = 1707000, 
		.tiaPower = componentPowerLevel_e::COMPONENT_POWER_LEVEL_HIGH, 
		.rxSsiConfig = {
		.ssiType = ssiType_e::SSI_TYPE_CMOS, 
		.ssiDataFormatSel = ssiDataFormat_e::SSI_FORMAT_16_BIT_I_Q_DATA, 
		.numLaneSel = ssiNumLane_e::SSI_1_LANE, 
		.strobeType = ssiStrobeType_e::SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ssiTxRefClockPin_e::TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = rxSsiDataSrc_e::SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = txDataSrc_e::TX_SSI, 
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
		.gainTableType = rxGainTableType_e::RX_GAIN_CORRECTION_TABLE, 
		.LoSelect = LoId_e::LO_ID_LO1}  }, 
		.orxConfig = { {
		.primaryBw_Hz = 0, 
		.rxOutputRate = 0, 
		.rxInterfaceSampleRate = 0, 
		.offsetLo_Hz = 0, 
		.ncoEn = 0, 
		.outputSignalingSel = rxSignalType_e::RX_SIGNAL_TYPE_IQ, 
		.adcCorner = 0, 
		.adcClk_Hz = 0, 
		.corner3dB = 0, 
		.tiaPower = componentPowerLevel_e::COMPONENT_POWER_LEVEL_LOW, 
		.rxSsiConfig = {
		.ssiType = ssiType_e::SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = ssiDataFormat_e::SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = ssiNumLane_e::SSI_1_LANE, 
		.strobeType = ssiStrobeType_e::SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ssiTxRefClockPin_e::TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = rxSsiDataSrc_e::SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = txDataSrc_e::TX_SSI, 
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
		.gainTableType = rxGainTableType_e::RX_GAIN_CORRECTION_TABLE, 
		.LoSelect = LoId_e::LO_ID_LO1}, {
		.primaryBw_Hz = 12000, 
		.rxOutputRate = 24000, 
		.rxInterfaceSampleRate = 24000, 
		.offsetLo_Hz = 0, 
		.ncoEn = 0, 
		.outputSignalingSel = rxSignalType_e::RX_SIGNAL_TYPE_IQ, 
		.adcCorner = 480000, 
		.adcClk_Hz = 38400000, 
		.corner3dB = 2800000, 
		.tiaPower = componentPowerLevel_e::COMPONENT_POWER_LEVEL_HIGH, 
		.rxSsiConfig = {
		.ssiType = ssiType_e::SSI_TYPE_CMOS, 
		.ssiDataFormatSel = ssiDataFormat_e::SSI_FORMAT_16_BIT_I_Q_DATA, 
		.numLaneSel = ssiNumLane_e::SSI_1_LANE, 
		.strobeType = ssiStrobeType_e::SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ssiTxRefClockPin_e::TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = rxSsiDataSrc_e::SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = txDataSrc_e::TX_SSI, 
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
		.gainTableType = rxGainTableType_e::RX_GAIN_CORRECTION_TABLE, 
		.LoSelect = LoId_e::LO_ID_LO1}  }, 
		.ilbConfig = { {
		.primaryBw_Hz = 0, 
		.rxOutputRate = 0, 
		.rxInterfaceSampleRate = 0, 
		.offsetLo_Hz = 0, 
		.ncoEn = 0, 
		.outputSignalingSel = rxSignalType_e::RX_SIGNAL_TYPE_IQ, 
		.adcCorner = 0, 
		.adcClk_Hz = 0, 
		.corner3dB = 0, 
		.tiaPower = componentPowerLevel_e::COMPONENT_POWER_LEVEL_LOW, 
		.rxSsiConfig = {
		.ssiType = ssiType_e::SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = ssiDataFormat_e::SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = ssiNumLane_e::SSI_1_LANE, 
		.strobeType = ssiStrobeType_e::SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ssiTxRefClockPin_e::TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = rxSsiDataSrc_e::SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = txDataSrc_e::TX_SSI, 
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
		.gainTableType = rxGainTableType_e::RX_GAIN_CORRECTION_TABLE, 
		.LoSelect = LoId_e::LO_ID_LO1}, {
		.primaryBw_Hz = 12000, 
		.rxOutputRate = 24000, 
		.rxInterfaceSampleRate = 24000, 
		.offsetLo_Hz = 442000, 
		.ncoEn = 1, 
		.outputSignalingSel = rxSignalType_e::RX_SIGNAL_TYPE_IQ, 
		.adcCorner = 480000, 
		.adcClk_Hz = 38400000, 
		.corner3dB = 2800000, 
		.tiaPower = componentPowerLevel_e::COMPONENT_POWER_LEVEL_HIGH, 
		.rxSsiConfig = {
		.ssiType = ssiType_e::SSI_TYPE_CMOS, 
		.ssiDataFormatSel = ssiDataFormat_e::SSI_FORMAT_16_BIT_I_Q_DATA, 
		.numLaneSel = ssiNumLane_e::SSI_1_LANE, 
		.strobeType = ssiStrobeType_e::SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ssiTxRefClockPin_e::TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = rxSsiDataSrc_e::SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = txDataSrc_e::TX_SSI, 
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
		.gainTableType = rxGainTableType_e::RX_GAIN_CORRECTION_TABLE, 
		.LoSelect = LoId_e::LO_ID_LO1}  }, 
		.elbConfig = { {
		.primaryBw_Hz = 0, 
		.rxOutputRate = 0, 
		.rxInterfaceSampleRate = 0, 
		.offsetLo_Hz = 0, 
		.ncoEn = 0, 
		.outputSignalingSel = rxSignalType_e::RX_SIGNAL_TYPE_IQ, 
		.adcCorner = 0, 
		.adcClk_Hz = 0, 
		.corner3dB = 0, 
		.tiaPower = componentPowerLevel_e::COMPONENT_POWER_LEVEL_LOW, 
		.rxSsiConfig = {
		.ssiType = ssiType_e::SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = ssiDataFormat_e::SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = ssiNumLane_e::SSI_1_LANE, 
		.strobeType = ssiStrobeType_e::SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ssiTxRefClockPin_e::TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = rxSsiDataSrc_e::SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = txDataSrc_e::TX_SSI, 
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
		.gainTableType = rxGainTableType_e::RX_GAIN_CORRECTION_TABLE, 
		.LoSelect = LoId_e::LO_ID_LO1}, {
		.primaryBw_Hz = 12000, 
		.rxOutputRate = 24000, 
		.rxInterfaceSampleRate = 24000, 
		.offsetLo_Hz = 0, 
		.ncoEn = 0, 
		.outputSignalingSel = rxSignalType_e::RX_SIGNAL_TYPE_IQ, 
		.adcCorner = 480000, 
		.adcClk_Hz = 38400000, 
		.corner3dB = 2800000, 
		.tiaPower = componentPowerLevel_e::COMPONENT_POWER_LEVEL_HIGH, 
		.rxSsiConfig = {
		.ssiType = ssiType_e::SSI_TYPE_CMOS, 
		.ssiDataFormatSel = ssiDataFormat_e::SSI_FORMAT_16_BIT_I_Q_DATA, 
		.numLaneSel = ssiNumLane_e::SSI_1_LANE, 
		.strobeType = ssiStrobeType_e::SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ssiTxRefClockPin_e::TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsBitInversion = 0, 
		.lvdsUseLsbIn12bitMode = 0, 
		.rxSsiDataSrc = rxSsiDataSrc_e::SSI_RX1_DP_RXNB_DP1, 
		.txDataSrc = txDataSrc_e::TX_SSI, 
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
		.gainTableType = rxGainTableType_e::RX_GAIN_CORRECTION_TABLE, 
		.LoSelect = LoId_e::LO_ID_LO1}  }, 
		.tx1Config = {
		.txDpProfile = {
		.preProc = {
		.symMapSymbol0 = 0, 
		.symMapSymbol1 = 0, 
		.symMapSymbol2 = 0, 
		.symMapSymbol3 = 0, 
		.symMapDivFactor = 0, 
		.symMapQVal = 0, 
		.preProcMode = txDpPreProcMode_e::TX_DP_PREPROC_I_ONLY_BYPASSED, 
		.symMapperMode = txDpSymbleMapperMode_e::TX_DP_SYM_MAPPER_DISABLE, 
		.txPreProcWbNbPfirIBank = pfirBank_e::PFIR_BANK_A, 
		.txPreProcWbNbPfirQBank = pfirBank_e::PFIR_BANK_A, 
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
		.iqdmDucMode = txDpIqDmDucMode_e::TX_DP_IQDMDUC_MODE0, 
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
		.sincGainMux = rxSincGainMuxOutput_e::RX_SINC_GAIN_MUX_0_DB, 
		.sincMux = rxSincMux5Output_e::RX_SINC_MUX5_OUTPUT_ZERO, 
		.hbMux = rxHBMuxOutput_e::RX_HB_MUX_BYPASS, 
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
		.rxWbNbCompPFirInMuxSel = rpFirInMuxSel_e::RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 1, 
		.rxWbNbCompPFirBankSel = pfirBank_e::PFIR_BANK_A, 
		.padding = { 0  }}
, 
		.reSamp = {
		.rxReSampEn = 0, 
		.padding = { 0, 0, 0  }, 
		.reSampPhaseI = 0, 
		.reSampPhaseQ = 0}
, 
		.gsOutMuxSel = gsOutMuxSel_e::GSOUT_MUX_GS_OUTPUT, 
		.dnSampleSel = dnSampleSel_e::NO_DN_SAMPLE, 
		.rxOutSel = rxOutSel_e::RXOUTSEL_IQ, 
		.rxRoundMode = rxRoundMode_e::RXROUNDMODE_IQ}
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
		.sincGainMux = rxSincGainMuxOutput_e::RX_SINC_GAIN_MUX_0_DB, 
		.sincMux = rxSincMux5Output_e::RX_SINC_MUX5_OUTPUT_ZERO, 
		.hbMux = rxHBMuxOutput_e::RX_HB_MUX_BYPASS, 
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
		.rxWbNbCompPFirInMuxSel = rpFirInMuxSel_e::RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 1, 
		.rxWbNbCompPFirBankSel = pfirBank_e::PFIR_BANK_A, 
		.padding = { 0  }}
, 
		.reSamp = {
		.rxReSampEn = 0, 
		.padding = { 0, 0, 0  }, 
		.reSampPhaseI = 0, 
		.reSampPhaseQ = 0}
, 
		.gsOutMuxSel = gsOutMuxSel_e::GSOUT_MUX_GS_OUTPUT, 
		.dnSampleSel = dnSampleSel_e::NO_DN_SAMPLE, 
		.rxOutSel = rxOutSel_e::RXOUTSEL_IQ, 
		.rxRoundMode = rxRoundMode_e::RXROUNDMODE_IQ}
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
		.sincGainMux = rxSincGainMuxOutput_e::RX_SINC_GAIN_MUX_0_DB, 
		.sincMux = rxSincMux5Output_e::RX_SINC_MUX5_OUTPUT_ZERO, 
		.hbMux = rxHBMuxOutput_e::RX_HB_MUX_BYPASS, 
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
		.rxWbNbCompPFirInMuxSel = rpFirInMuxSel_e::RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 1, 
		.rxWbNbCompPFirBankSel = pfirBank_e::PFIR_BANK_A, 
		.padding = { 0  }}
, 
		.reSamp = {
		.rxReSampEn = 0, 
		.padding = { 0, 0, 0  }, 
		.reSampPhaseI = 0, 
		.reSampPhaseQ = 0}
, 
		.gsOutMuxSel = gsOutMuxSel_e::GSOUT_MUX_GS_OUTPUT, 
		.dnSampleSel = dnSampleSel_e::NO_DN_SAMPLE, 
		.rxOutSel = rxOutSel_e::RXOUTSEL_IQ, 
		.rxRoundMode = rxRoundMode_e::RXROUNDMODE_IQ}
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
		.sincGainMux = rxSincGainMuxOutput_e::RX_SINC_GAIN_MUX_0_DB, 
		.sincMux = rxSincMux5Output_e::RX_SINC_MUX5_OUTPUT_ZERO, 
		.hbMux = rxHBMuxOutput_e::RX_HB_MUX_BYPASS, 
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
		.rxWbNbCompPFirInMuxSel = rpFirInMuxSel_e::RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 1, 
		.rxWbNbCompPFirBankSel = pfirBank_e::PFIR_BANK_A, 
		.padding = { 0  }}
, 
		.reSamp = {
		.rxReSampEn = 0, 
		.padding = { 0, 0, 0  }, 
		.reSampPhaseI = 0, 
		.reSampPhaseQ = 0}
, 
		.gsOutMuxSel = gsOutMuxSel_e::GSOUT_MUX_GS_OUTPUT, 
		.dnSampleSel = dnSampleSel_e::NO_DN_SAMPLE, 
		.rxOutSel = rxOutSel_e::RXOUTSEL_IQ, 
		.rxRoundMode = rxRoundMode_e::RXROUNDMODE_IQ}
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
		.symMapDivFactor = 1, 
		.symMapQVal = 0, 
		.preProcMode = txDpPreProcMode_e::TX_DP_PREPROC_I_Q, 
		.symMapperMode = txDpSymbleMapperMode_e::TX_DP_SYM_MAPPER_DISABLE, 
		.txPreProcWbNbPfirIBank = pfirBank_e::PFIR_BANK_A, 
		.txPreProcWbNbPfirQBank = pfirBank_e::PFIR_BANK_A, 
		.padding = { 0, 0  }}
, 
		.nbIntTopNew = {
		.interpBy5_2En = 1, 
		.interpBy5_1En = 0, 
		.interpBy3_1En = 0, 
		.interpBy2_6En = 1, 
		.interpBy2_5En = 1, 
		.interpBy2_4En = 0, 
		.interpBy2_3En = 0, 
		.interpBy2_2En = 0, 
		.interpBy2_1En = 1, 
		.scicNbBlk32En = 1, 
		.scicNbBlk32DivFactor = 10, 
		.padding = 0}
, 
		.intTop = {
		.interpBy3Blk44p1En = 0, 
		.sinc3Blk44En = 0, 
		.sinc2Blk42En = 1, 
		.interpBy3Blk40En = 0, 
		.interpBy3Blk40p1En = 0, 
		.interpBy2Blk38En = 0, 
		.interpBy2Blk36En = 1, 
		.interpBy2BlkEn = 0}
, 
		.intTopFreqDevMap = {
		.rrc2Frac = 0, 
		.mpll = 0, 
		.nchLsw = 0, 
		.nchMsb = 0, 
		.freqDevMapEn = 0, 
		.txRoundEn = 1, 
		.padding = { 0  }}
, 
		.iqdmDuc = {
		.iqdmDucMode = txDpIqDmDucMode_e::TX_DP_IQDMDUC_MODE0, 
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
		.scicEn = 1, 
		.scicDivFactor = 10, 
		.rxDpScicControl = 0, 
		.decBy5Blk14En = 1, 
		.decBy5Blk16En = 0, 
		.decBy3Blk18En = 0, 
		.decBy2Blk20En = 1, 
		.decBy2Blk22En = 1, 
		.decBy2Blk24En = 0, 
		.decBy2Blk26En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 1}
, 
		.sincHbTopNb = {
		.sincGainMux = rxSincGainMuxOutput_e::RX_SINC_GAIN_MUX_0_DB, 
		.sincMux = rxNbSincMux4Output_e::RXNB_SINC_MUX5_OUTPUT_SINC2, 
		.hbMux = rxNbHBMuxOutput_e::RXNB_HB_MUX_OUTPUT_HB1, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  }}
, 
		.rxNbDemNb = {
		.wbPthSel = wbPthSel_e::NB_PTH, 
		.dp3Sel = formatingSel_e::DP1_IQ22, 
		.corrFifoDelayEn = corrFifoDelayEn_e::DTH_DLY_EN, 
		.corrSelDp2En = 0, 
		.dpInFifo = {
		.dpInFifoTestDataSel = dpInFifoInputSel_e::DPINFIFOINPUTSEL_DP, 
		.dpInFifoEn = 0, 
		.dpInFifoMode = dpInFifoMode_e::DPINFIFOMODE_DETECTING, 
		.padding = { 0  }}
, 
		.rxNbNco = {
		.rxNbNcoEn = 0, 
		.padding = { 0, 0, 0  }, 
		.rxNbNcoConfig = {
		.freq = 0, 
		.sampleFreq = 24000, 
		.phase = 0, 
		.realOut = 0, 
		.freeRunClkEnable = 0}
}
, 
		.rxWbNbCompPFir = { {
		.rxWbNbCompPFirInMuxSel = rpFirInMuxSel_e::RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 1, 
		.rxWbNbCompPFirBankSel = pfirBank_e::PFIR_BANK_A, 
		.padding = { 0  }}, {
		.rxWbNbCompPFirInMuxSel = rpFirInMuxSel_e::RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 1, 
		.rxWbNbCompPFirBankSel = pfirBank_e::PFIR_BANK_A, 
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
		.gsOutMuxSel = { gsOutMuxSel_e::GSOUT_MUX_GS_OUTPUT, gsOutMuxSel_e::GSOUT_MUX_GS_OUTPUT  }, 
		.dnSampleSel = { dnSampleSel_e::NO_DN_SAMPLE, dnSampleSel_e::NO_DN_SAMPLE  }, 
		.FDSampleDelay = { 1, 1  }, 
		.pulPfirDelay = { 0, 0  }, 
		.rxOutSel = { rxOutSel_e::RXOUTSEL_IQ, rxOutSel_e::RXOUTSEL_IQ  }, 
		.rxRoundMode = { rxRoundMode_e::RXROUNDMODE_IQ, rxRoundMode_e::RXROUNDMODE_IQ  }, 
		.freqDeviation = { 0, 0  }, 
		.symbolRate = { 0, 0  }, 
		.dpArmSel = dpArmSel_e::DPARMSEL_DP, 
		.dpEnabled = { 0, 0  }, 
		.padding = { 0, 0  }}
}
, 
		.adcOption = adcOption_e::Option5, 
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
		.scicEn = 1, 
		.scicDivFactor = 10, 
		.rxDpScicControl = 0, 
		.decBy5Blk14En = 1, 
		.decBy5Blk16En = 0, 
		.decBy3Blk18En = 0, 
		.decBy2Blk20En = 1, 
		.decBy2Blk22En = 1, 
		.decBy2Blk24En = 0, 
		.decBy2Blk26En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 1}
, 
		.sincHbTopNb = {
		.sincGainMux = rxSincGainMuxOutput_e::RX_SINC_GAIN_MUX_0_DB, 
		.sincMux = rxNbSincMux4Output_e::RXNB_SINC_MUX5_OUTPUT_SINC2, 
		.hbMux = rxNbHBMuxOutput_e::RXNB_HB_MUX_OUTPUT_HB1, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  }}
, 
		.rxNbDemNb = {
		.wbPthSel = wbPthSel_e::NB_PTH, 
		.dp3Sel = formatingSel_e::DP1_IQ22, 
		.corrFifoDelayEn = corrFifoDelayEn_e::DTH_DLY_EN, 
		.corrSelDp2En = 0, 
		.dpInFifo = {
		.dpInFifoTestDataSel = dpInFifoInputSel_e::DPINFIFOINPUTSEL_DP, 
		.dpInFifoEn = 0, 
		.dpInFifoMode = dpInFifoMode_e::DPINFIFOMODE_DETECTING, 
		.padding = { 0  }}
, 
		.rxNbNco = {
		.rxNbNcoEn = 0, 
		.padding = { 0, 0, 0  }, 
		.rxNbNcoConfig = {
		.freq = 0, 
		.sampleFreq = 24000, 
		.phase = 0, 
		.realOut = 0, 
		.freeRunClkEnable = 0}
}
, 
		.rxWbNbCompPFir = { {
		.rxWbNbCompPFirInMuxSel = rpFirInMuxSel_e::RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 1, 
		.rxWbNbCompPFirBankSel = pfirBank_e::PFIR_BANK_A, 
		.padding = { 0  }}, {
		.rxWbNbCompPFirInMuxSel = rpFirInMuxSel_e::RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 1, 
		.rxWbNbCompPFirBankSel = pfirBank_e::PFIR_BANK_A, 
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
		.gsOutMuxSel = { gsOutMuxSel_e::GSOUT_MUX_GS_OUTPUT, gsOutMuxSel_e::GSOUT_MUX_GS_OUTPUT  }, 
		.dnSampleSel = { dnSampleSel_e::NO_DN_SAMPLE, dnSampleSel_e::NO_DN_SAMPLE  }, 
		.FDSampleDelay = { 1, 1  }, 
		.pulPfirDelay = { 0, 0  }, 
		.rxOutSel = { rxOutSel_e::RXOUTSEL_IQ, rxOutSel_e::RXOUTSEL_IQ  }, 
		.rxRoundMode = { rxRoundMode_e::RXROUNDMODE_IQ, rxRoundMode_e::RXROUNDMODE_IQ  }, 
		.freqDeviation = { 0, 0  }, 
		.symbolRate = { 0, 0  }, 
		.dpArmSel = dpArmSel_e::DPARMSEL_DP, 
		.dpEnabled = { 0, 0  }, 
		.padding = { 0, 0  }}
}
, 
		.adcOption = adcOption_e::Default, 
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
		.scicEn = 1, 
		.scicDivFactor = 10, 
		.rxDpScicControl = 0, 
		.decBy5Blk14En = 1, 
		.decBy5Blk16En = 0, 
		.decBy3Blk18En = 0, 
		.decBy2Blk20En = 1, 
		.decBy2Blk22En = 1, 
		.decBy2Blk24En = 0, 
		.decBy2Blk26En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 1}
, 
		.sincHbTopNb = {
		.sincGainMux = rxSincGainMuxOutput_e::RX_SINC_GAIN_MUX_0_DB, 
		.sincMux = rxNbSincMux4Output_e::RXNB_SINC_MUX5_OUTPUT_SINC2, 
		.hbMux = rxNbHBMuxOutput_e::RXNB_HB_MUX_OUTPUT_HB1, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  }}
, 
		.rxNbDemNb = {
		.wbPthSel = wbPthSel_e::NB_PTH, 
		.dp3Sel = formatingSel_e::DP1_IQ22, 
		.corrFifoDelayEn = corrFifoDelayEn_e::DTH_DLY_EN, 
		.corrSelDp2En = 0, 
		.dpInFifo = {
		.dpInFifoTestDataSel = dpInFifoInputSel_e::DPINFIFOINPUTSEL_DP, 
		.dpInFifoEn = 0, 
		.dpInFifoMode = dpInFifoMode_e::DPINFIFOMODE_DETECTING, 
		.padding = { 0  }}
, 
		.rxNbNco = {
		.rxNbNcoEn = 0, 
		.padding = { 0, 0, 0  }, 
		.rxNbNcoConfig = {
		.freq = 0, 
		.sampleFreq = 24000, 
		.phase = 0, 
		.realOut = 0, 
		.freeRunClkEnable = 0}
}
, 
		.rxWbNbCompPFir = { {
		.rxWbNbCompPFirInMuxSel = rpFirInMuxSel_e::RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 1, 
		.rxWbNbCompPFirBankSel = pfirBank_e::PFIR_BANK_A, 
		.padding = { 0  }}, {
		.rxWbNbCompPFirInMuxSel = rpFirInMuxSel_e::RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 1, 
		.rxWbNbCompPFirBankSel = pfirBank_e::PFIR_BANK_A, 
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
		.gsOutMuxSel = { gsOutMuxSel_e::GSOUT_MUX_GS_OUTPUT, gsOutMuxSel_e::GSOUT_MUX_GS_OUTPUT  }, 
		.dnSampleSel = { dnSampleSel_e::NO_DN_SAMPLE, dnSampleSel_e::NO_DN_SAMPLE  }, 
		.FDSampleDelay = { 1, 1  }, 
		.pulPfirDelay = { 0, 0  }, 
		.rxOutSel = { rxOutSel_e::RXOUTSEL_IQ, rxOutSel_e::RXOUTSEL_IQ  }, 
		.rxRoundMode = { rxRoundMode_e::RXROUNDMODE_IQ, rxRoundMode_e::RXROUNDMODE_IQ  }, 
		.freqDeviation = { 0, 0  }, 
		.symbolRate = { 0, 0  }, 
		.dpArmSel = dpArmSel_e::DPARMSEL_DP, 
		.dpEnabled = { 0, 0  }, 
		.padding = { 0, 0  }}
}
, 
		.adcOption = adcOption_e::Option5, 
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
		.scicEn = 1, 
		.scicDivFactor = 10, 
		.rxDpScicControl = 0, 
		.decBy5Blk14En = 1, 
		.decBy5Blk16En = 0, 
		.decBy3Blk18En = 0, 
		.decBy2Blk20En = 1, 
		.decBy2Blk22En = 1, 
		.decBy2Blk24En = 0, 
		.decBy2Blk26En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 1}
, 
		.sincHbTopNb = {
		.sincGainMux = rxSincGainMuxOutput_e::RX_SINC_GAIN_MUX_0_DB, 
		.sincMux = rxNbSincMux4Output_e::RXNB_SINC_MUX5_OUTPUT_SINC2, 
		.hbMux = rxNbHBMuxOutput_e::RXNB_HB_MUX_OUTPUT_HB1, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  }}
, 
		.rxNbDemNb = {
		.wbPthSel = wbPthSel_e::NB_PTH, 
		.dp3Sel = formatingSel_e::DP1_IQ22, 
		.corrFifoDelayEn = corrFifoDelayEn_e::DTH_DLY_EN, 
		.corrSelDp2En = 0, 
		.dpInFifo = {
		.dpInFifoTestDataSel = dpInFifoInputSel_e::DPINFIFOINPUTSEL_DP, 
		.dpInFifoEn = 0, 
		.dpInFifoMode = dpInFifoMode_e::DPINFIFOMODE_DETECTING, 
		.padding = { 0  }}
, 
		.rxNbNco = {
		.rxNbNcoEn = 0, 
		.padding = { 0, 0, 0  }, 
		.rxNbNcoConfig = {
		.freq = 0, 
		.sampleFreq = 24000, 
		.phase = 0, 
		.realOut = 0, 
		.freeRunClkEnable = 0}
}
, 
		.rxWbNbCompPFir = { {
		.rxWbNbCompPFirInMuxSel = rpFirInMuxSel_e::RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 1, 
		.rxWbNbCompPFirBankSel = pfirBank_e::PFIR_BANK_A, 
		.padding = { 0  }}, {
		.rxWbNbCompPFirInMuxSel = rpFirInMuxSel_e::RPFIRIN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 1, 
		.rxWbNbCompPFirBankSel = pfirBank_e::PFIR_BANK_A, 
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
		.gsOutMuxSel = { gsOutMuxSel_e::GSOUT_MUX_GS_OUTPUT, gsOutMuxSel_e::GSOUT_MUX_GS_OUTPUT  }, 
		.dnSampleSel = { dnSampleSel_e::NO_DN_SAMPLE, dnSampleSel_e::NO_DN_SAMPLE  }, 
		.FDSampleDelay = { 1, 1  }, 
		.pulPfirDelay = { 0, 0  }, 
		.rxOutSel = { rxOutSel_e::RXOUTSEL_IQ, rxOutSel_e::RXOUTSEL_IQ  }, 
		.rxRoundMode = { rxRoundMode_e::RXROUNDMODE_IQ, rxRoundMode_e::RXROUNDMODE_IQ  }, 
		.freqDeviation = { 0, 0  }, 
		.symbolRate = { 0, 0  }, 
		.dpArmSel = dpArmSel_e::DPARMSEL_DP, 
		.dpEnabled = { 0, 0  }, 
		.padding = { 0, 0  }}
}
, 
		.adcOption = adcOption_e::Default, 
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
		.duplexMode = duplexMode_e::TDD, 
		.fhModeOn = 0, 
		.numDynamicProfile = 1, 
		.mcsMode = mcsMode_e::MCS_DISABLE, 
		.pllLockTime_us = 750, 
		.pllModuli = {
		.modulus = { 8388593, 8388593, 8388593, 8388593, 8388593  }, 
		.dmModulus = { 8388593, 8388593, 8388593, 8388593  }}
, 
		.pllPhaseSyncWait_us = 750, 
		.mcsInterfaceType = mcsInf_e::MCS_INTF_DISABLE, 
		.warmBootOn = 0, 
		.padding = { 0, 0  }}
, 
		.checksum = 2879507892};
