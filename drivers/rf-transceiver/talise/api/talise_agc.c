// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_agc.c
 * \brief Contains Talise API AGC function calls
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "talise_agc.h"
#include "talise_reg_addr_macros.h"
#include "talise_user.h"
#include "talise_hal.h"
#include "talise_error.h"
#include "talise_rx_types.h"
#include "talise_gpio.h"

uint32_t TALISE_setupRxAgc(taliseDevice_t *device, taliseAgcCfg_t *rxAgcCtrl)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t agcGainUpdateCtr[3] = {0};
	uint8_t agcRegister = 0;
	uint32_t agcClock_Hz = 0;
	uint32_t agcGainUpdateCounter = 0;
	uint32_t agcUnderRangeLowInterval = 0;
	uint32_t adcClock_Hz = 0;
	uint8_t minAgcSlowLoopSettlingDelay = 0;

	static const uint8_t agcPeakWaitTimeBitMask = 0x1F;
	static const uint32_t agcGainUpdateCounterBitMask = 0x003FFFFF;
	static const uint8_t powerEnableMeasurementBitMask = 0x01;
	static const uint8_t powerUseRfirOutBitMask = 0x02;
	static const uint8_t powerUseBBDC2BitMask = 0x08;
	static const uint8_t underRangeHighPowerThreshBitMask = 0x7F;
	static const uint8_t underRangeLowPowerThreshBitMask = 0x1F;
	static const uint8_t underRangeHighPowerGainStepRecoveryBitMask = 0x1F;
	static const uint8_t underRangeLowPowerGainStepRecoveryBitMask = 0x1F;
	static const uint8_t powerMeasurementDurationBitMask = 0x1F;
	static const uint8_t agcSlowLoopSettlingDelayBitMask = 0x7F;
	static const uint8_t ip3OverRangeThreshBitMask = 0x3F;
	static const uint8_t apdHighThreshMin = 0x07;
	static const uint8_t apdHighThreshMax = 0x31;
	static const uint8_t apdLowGainModeHighThreshMin = 0x07;
	static const uint8_t apdLowGainModeHighThreshMax = 0x31;
	static const uint8_t apdLowThreshMin = 0x07;
	static const uint8_t apdLowThreshMax = 0x31;
	static const uint8_t apdLowGainModeLowThreshMin = 0x07;
	static const uint8_t apdLowGainModeLowThreshMax = 0x31;
	static const uint8_t apdGainStepAttackBitMask = 0x1F;
	static const uint8_t apdGainStepRecoveryBitMask = 0x1F;
	static const uint8_t enableHb2OverloadBitMask = 0x80;
	static const uint8_t hb2OverloadDurationCntBitMask = 0x70;
	static const uint8_t hb2OverloadThreshCntBitMask = 0x0F;
	static const uint8_t hb2GainStepHighRecoveryBitMask = 0x1F;
	static const uint8_t hb2GainStepLowRecoveryBitMask = 0x1F;
	static const uint8_t hb2GainStepAttackBitMask = 0x1F;
	static const uint8_t hb2GainStepMidRecoveryBitMask = 0x1F;
	static const uint8_t hb2OverloadPowerModeBitMask = 0x04;
	static const uint8_t agcMsbGainUpdateTimeBitMask = 0x3F;
	static const uint8_t hb2OvrgSelBitMask = 0x20;
	static const uint8_t hb2ThreshConfigBitMask = 0x03;
	static const uint8_t upper0PowerThreshBitMask = 0x7F;
	static const uint8_t upper1PowerThreshBitMask = 0x0F;
	static const uint8_t powerLogShiftBitMask = 0x04;
	static const uint8_t overRangeLowPowerGainStepAttackBitMask = 0x1F;
	static const uint8_t overRangeHighPowerGainStepAttackBitMask = 0x1F;

	static const uint8_t agcLowThreshPreventGainBitMask = 0x20;
	static const uint8_t agcChangeGainIfThreshHighBitMask = 0x18;
	static const uint8_t agcPeakThreshGainControlModeBitMask = 0x04;
	static const uint8_t agcResetOnRxonBitMask = 0x02;
	static const uint8_t agcEnableSyncPulseForGainCounterBitMask = 0x01;
	static const uint8_t agcEnableIp3OptimizationThreshBitMask = 0x80;
	static const uint8_t agcEnableFastRecoveryLoopBitMask = 0x40;
	static const uint8_t agcRx1AttackDelayBitMask = 0x3F;
	static const uint8_t agcRx2AttackDelayBitMask = 0x3F;
	static const uint32_t agcUnderRangeLowIntervalMask = 0x0000FFFF;
	static const uint8_t agcClock_HzBitMask = 0x03;
	static const uint8_t agcUnderRangeMidIntervalMask = 0x3F;
	static const uint8_t agcUnderRangeHighIntervalMask = 0x3F;

	static const uint8_t MIN_SUPPORTED_SIREV = 0xC0;
	static const uint8_t ENABLE_IP3_OPTIMIZATION_MASK = 0x80;

#if TALISE_VERBOSE
	halError =talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				"TALISE_setupRxAgc()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* checking for valid taliseAgcCfg_t device->rx->rxAgcCtrl pointer to determine if it has been initialized */
	if (rxAgcCtrl == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_RX_STRUCT_INIT, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* performing AGC peak wait time value check */
	if (rxAgcCtrl->agcPeakWaitTime & ~agcPeakWaitTimeBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_RX_PEAK_WAIT_TIME_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		/* write AGC peak wait time */
		halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_AGC_CONFIG1,
					    rxAgcCtrl->agcPeakWaitTime, agcPeakWaitTimeBitMask, 0);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* performing range check for Rx1 and Rx2 max gain exceedence */
	if ((rxAgcCtrl->agcRx1MaxGainIndex >
	     device->devStateInfo.gainIndexes.rx1MaxGainIndex) ||
	    (rxAgcCtrl->agcRx2MaxGainIndex >
	     device->devStateInfo.gainIndexes.rx2MaxGainIndex) ||
	    (rxAgcCtrl->agcRx1MinGainIndex <
	     device->devStateInfo.gainIndexes.rx1MinGainIndex) ||
	    (rxAgcCtrl->agcRx2MinGainIndex <
	     device->devStateInfo.gainIndexes.rx2MinGainIndex)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_RX_MIN_MAX_GAIN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		/* writing Rx1 and Rx2 max and min gain indices */
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MAX_GAIN_INDEX_RX1,
					   rxAgcCtrl->agcRx1MaxGainIndex);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MIN_GAIN_INDEX_RX1,
					   rxAgcCtrl->agcRx1MinGainIndex);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MAX_GAIN_INDEX_RX2,
					   rxAgcCtrl->agcRx2MaxGainIndex);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MIN_GAIN_INDEX_RX2,
					   rxAgcCtrl->agcRx2MinGainIndex);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/*Calculation of Gain Update Time*/

	/*Reading back AGC Clock Division Ratio*/
	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_5,
				   &agcGainUpdateCtr[0], agcClock_HzBitMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	adcClock_Hz = (device->devStateInfo.clocks.hsDigClkDiv2_Hz << 1);

	/*Calculation of AGC clock in Hz*/
	switch (agcGainUpdateCtr[0]) {
	case 0:
		agcClock_Hz = device->devStateInfo.clocks.hsDigClkDiv4or5_Hz;
		break;

	case 1:
		agcClock_Hz = (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 2);
		break;

	case 2:
		agcClock_Hz = (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 4);
		break;

	case 3:
		agcClock_Hz = (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 8);
		break;

	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_CLK_DIV_RATIO_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (agcClock_Hz == 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_CLK_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((adcClock_Hz % agcClock_Hz) != 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_CLK_RATIO, retVal, TALACT_ERR_CHECK_PARAM);
	}

	minAgcSlowLoopSettlingDelay = (uint8_t)(65 / (adcClock_Hz / agcClock_Hz));

	agcGainUpdateCounter = (uint32_t)DIV_U64(((uint64_t)
			       rxAgcCtrl->agcGainUpdateCounter_us * (uint64_t)agcClock_Hz), 1000000);

	/* performing range check for gain update time */
	if (agcGainUpdateCounter & ~agcGainUpdateCounterBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_RX_GAIN_UPDATE_TIME_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		/* divide gain update counter value into three (3) registers to perform write */
		agcGainUpdateCtr[0] = (uint8_t)(agcGainUpdateCounter);
		agcGainUpdateCtr[1] = (uint8_t)(agcGainUpdateCounter >> 8);
		agcGainUpdateCtr[2] = (uint8_t)(agcGainUpdateCounter >> 16);

		/* write two low order bytes directly due to byte width of register value */
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_GAIN_UPDATE_COUNTER1,
					   agcGainUpdateCtr[0]);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_GAIN_UPDATE_COUNTER2,
					   agcGainUpdateCtr[1]);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* write MSB with mask due to extra control bits in register */
		halError = talSpiWriteField(device->devHalInfo,
					    TALISE_ADDR_GAIN_UPDATE_COUNTER3, agcGainUpdateCtr[2],
					    agcMsbGainUpdateTimeBitMask, 0);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (rxAgcCtrl->agcRx1AttackDelay & ~agcRx1AttackDelayBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_RX1ATTACKDELAY_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_AGC_ATTACK_DELAY_RX1,
					   rxAgcCtrl->agcRx1AttackDelay);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (rxAgcCtrl->agcRx2AttackDelay & ~agcRx2AttackDelayBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_RX2ATTACKDELAY_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_AGC_ATTACK_DELAY_RX2,
					   rxAgcCtrl->agcRx2AttackDelay);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* AGC Control register checks and write */
	if ((rxAgcCtrl->agcLowThreshPreventGain << 5) &
	    ~agcLowThreshPreventGainBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_RX_LOWTHRSHPREVENTGAIN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else if ((rxAgcCtrl->agcChangeGainIfThreshHigh << 3) &
		   ~agcChangeGainIfThreshHighBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_RX_CHANGEGAINTHRSHHIGH_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else if ((rxAgcCtrl->agcPeakThreshGainControlMode << 2) &
		   ~agcPeakThreshGainControlModeBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_RX_LOWTHRSHPREVENTGAIN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else if ((rxAgcCtrl->agcResetOnRxon << 1) & ~agcResetOnRxonBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_RX_RESETONRXON_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else if (rxAgcCtrl->agcEnableSyncPulseForGainCounter  &
		   ~agcEnableSyncPulseForGainCounterBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_RX_ENSYNCHPULSECAINCNTR_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else if ((rxAgcCtrl->agcEnableIp3OptimizationThresh << 7) &
		   ~agcEnableIp3OptimizationThreshBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_RX_ENIP3OPTTHRSH_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else if ((rxAgcCtrl->agcEnableFastRecoveryLoop << 6) &
		   ~agcEnableFastRecoveryLoopBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_RX_ENFASTRECOVERYLOOP_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		agcRegister = ((rxAgcCtrl->agcEnableIp3OptimizationThresh << 7)
			       | (rxAgcCtrl->agcEnableFastRecoveryLoop << 6)
			       | (rxAgcCtrl->agcLowThreshPreventGain << 5)
			       | (rxAgcCtrl->agcChangeGainIfThreshHigh << 3)
			       | (rxAgcCtrl->agcPeakThreshGainControlMode << 2)
			       | (rxAgcCtrl->agcResetOnRxon << 1)
			       | rxAgcCtrl->agcEnableSyncPulseForGainCounter);


		/* If C0 silicon or lower, clear [7], do not allow IP3 feature */
		if(device->devStateInfo.deviceSiRev <= MIN_SUPPORTED_SIREV) {
			agcRegister &= ~ENABLE_IP3_OPTIMIZATION_MASK;
		}

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SLOWLOOP_CONFIG,
					   agcRegister);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (rxAgcCtrl->agcSlowLoopSettlingDelay < minAgcSlowLoopSettlingDelay) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_MINAGCSLOWLOOPDELAY_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (rxAgcCtrl->agcSlowLoopSettlingDelay & ~agcSlowLoopSettlingDelayBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_SLOWLOOPDELAY_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_SLOWLOOP_SETTLING_DELAY, rxAgcCtrl->agcSlowLoopSettlingDelay);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* IP3Optimizatoin mode */
	if (rxAgcCtrl->agcEnableIp3OptimizationThresh > 0) {
		if (rxAgcCtrl->ip3OverRangeThresh & ~ip3OverRangeThreshBitMask) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_INV_AGC_PWR_IP3RANGE_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_DEC_IP3_OVERRANGE_THRESHOLD, rxAgcCtrl->ip3OverRangeThresh);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_AGC_IP3_OVERRANGE_THRESHOLD, rxAgcCtrl->ip3OverRangeThreshIndex);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_AGC_ADCOVRG_IP3_HIGH_COUNTER, rxAgcCtrl->ip3PeakExceededCnt);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* If peak detect data structure is not included into project */
	if (&rxAgcCtrl->agcPeak == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_STRUCT_INIT, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		agcUnderRangeLowInterval = (uint32_t)DIV_U64(((uint64_t)
					   rxAgcCtrl->agcPeak.agcUnderRangeLowInterval_ns * (uint64_t)agcClock_Hz),
					   1000000000);

		/* performing range check for gain update time */
		if (agcUnderRangeLowInterval & ~agcUnderRangeLowIntervalMask) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_INV_AGC_RX_GAIN_UNDERRANGE_UPDATE_TIME_PARM, retVal,
							  TALACT_ERR_CHECK_PARAM);
		}

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_AGC_UNDERRANGE0_0,
					   (uint8_t)(agcUnderRangeLowInterval));
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_AGC_UNDERRANGE0_1,
					   (uint8_t)(agcUnderRangeLowInterval >> 8));
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		if (rxAgcCtrl->agcPeak.agcUnderRangeMidInterval &
		    ~agcUnderRangeMidIntervalMask ) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_INV_AGC_RX_GAIN_UNDERRANGE_MID_INTERVAL_PARM, retVal,
							  TALACT_ERR_CHECK_PARAM);
		} else {
			halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_AGC_UNDERRANGE1,
						    rxAgcCtrl->agcPeak.agcUnderRangeMidInterval, agcUnderRangeMidIntervalMask, 0);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}

		if (rxAgcCtrl->agcPeak.agcUnderRangeHighInterval &
		    ~agcUnderRangeHighIntervalMask ) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_INV_AGC_RX_GAIN_UNDERRANGE_HIGH_INTERVAL_PARM, retVal,
							  TALACT_ERR_CHECK_PARAM);
		} else {
			halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_AGC_UNDERRANGE2,
						   rxAgcCtrl->agcPeak.agcUnderRangeHighInterval);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}
	}

	if ((rxAgcCtrl->agcPeak.apdHighThresh < apdHighThreshMin) ||
	    (rxAgcCtrl->agcPeak.apdHighThresh > apdHighThreshMax)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_HIGHTHRSH_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else if ((rxAgcCtrl->agcPeak.apdHighThresh <
		    rxAgcCtrl->agcPeak.apdLowThresh)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_RX_APD_HIGH_LOW_THRESH, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_UPPER_LEVEL_BLOCKER_THRESHOLD, rxAgcCtrl->agcPeak.apdHighThresh);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if ((rxAgcCtrl->agcPeak.apdLowGainModeHighThresh < apdLowGainModeHighThreshMin)
	    ||
	    (rxAgcCtrl->agcPeak.apdLowGainModeHighThresh > apdLowGainModeHighThreshMax)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_LOWGAINMODEHIGHTHRSH_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_UPPER_LEVEL_BLOCKER_THRESHOLD2,
					   rxAgcCtrl->agcPeak.apdLowGainModeHighThresh);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if ((rxAgcCtrl->agcPeak.apdLowThresh < apdLowThreshMin) ||
	    (rxAgcCtrl->agcPeak.apdLowThresh > apdLowThreshMax)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_LOWGAINHIGHTHRSH_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_LOWER_LEVEL_BLOCKER_THRESHOLD, rxAgcCtrl->agcPeak.apdLowThresh);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if ((rxAgcCtrl->agcPeak.apdLowGainModeLowThresh < apdLowGainModeLowThreshMin) ||
	    (rxAgcCtrl->agcPeak.apdLowGainModeLowThresh > apdLowGainModeLowThreshMax)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_LOWGAINTHRSH_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_LOWER_LEVEL_BLOCKER_THRESHOLD2,
					   rxAgcCtrl->agcPeak.apdLowGainModeLowThresh);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ULB_COUNT_THRESHOLD,
				   rxAgcCtrl->agcPeak.apdUpperThreshPeakExceededCnt);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_LLB_COUNT_THRESHOLD,
				   rxAgcCtrl->agcPeak.apdLowerThreshPeakExceededCnt);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if (rxAgcCtrl->agcPeak.apdGainStepAttack & ~apdGainStepAttackBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_GAINSTEPATTACK_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_AGC_OVRG_GAIN_STEP1,
					   rxAgcCtrl->agcPeak.apdGainStepAttack);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (rxAgcCtrl->agcPeak.apdGainStepRecovery & ~apdGainStepRecoveryBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_GAINSTEPRECOVERY_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_AGC_OVRG_GAIN_STEP4,
					   rxAgcCtrl->agcPeak.apdGainStepRecovery);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* HB2 Configuration */
	if ((rxAgcCtrl->agcPeak.enableHb2Overload << 7) & ~enableHb2OverloadBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_HB2OVRLD_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else if ((rxAgcCtrl->agcPeak.hb2OverloadDurationCnt << 4) & ~
		   (hb2OverloadDurationCntBitMask)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_HB2OVRLDDURATION_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else if (rxAgcCtrl->agcPeak.hb2OverloadThreshCnt &
		   ~hb2OverloadThreshCntBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_HB2OVRLDTHRSHCNT_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		/*HB2 Configuration register*/
		agcRegister = (rxAgcCtrl->agcPeak.enableHb2Overload << 7) &
			      enableHb2OverloadBitMask;
		agcRegister |= (rxAgcCtrl->agcPeak.hb2OverloadDurationCnt << 4) &
			       hb2OverloadDurationCntBitMask;
		agcRegister |= rxAgcCtrl->agcPeak.hb2OverloadThreshCnt &
			       hb2OverloadThreshCntBitMask;

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_DEC_OVERLOAD_CONFIG1,
					   agcRegister);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_DEC_OVERLOAD_UPPER_THRESHOLD, rxAgcCtrl->agcPeak.hb2HighThresh);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_DEC_UNDERRANGE_INTERVAL0_THRESHOLD,
				   rxAgcCtrl->agcPeak.hb2UnderRangeLowThresh);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_DEC_UNDERRANGE_INTERVAL1_THRESHOLD,
				   rxAgcCtrl->agcPeak.hb2UnderRangeMidThresh);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_DEC_OVERLOAD_LOWER_THRESHOLD,
				   rxAgcCtrl->agcPeak.hb2UnderRangeHighThresh);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_ADC_HIGH_OVRG_COUNT_THRESHOLD,
				   rxAgcCtrl->agcPeak.hb2UpperThreshPeakExceededCnt);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_ADC_LOW_OVRG_COUNT_THRESHOLD,
				   rxAgcCtrl->agcPeak.hb2LowerThreshPeakExceededCnt);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if (rxAgcCtrl->agcPeak.hb2GainStepHighRecovery &
	    ~hb2GainStepHighRecoveryBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_HB2GAINSTEPRECOVERY_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_AGC_OVRG_GAIN_STEP5,
					   rxAgcCtrl->agcPeak.hb2GainStepHighRecovery);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (rxAgcCtrl->agcPeak.hb2GainStepLowRecovery &
	    ~hb2GainStepLowRecoveryBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_HB2GAINSTEP0RECOVERY_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_AGC_OVRG_LOW_INT0_GAIN_STEP,
					   rxAgcCtrl->agcPeak.hb2GainStepLowRecovery);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (rxAgcCtrl->agcPeak.hb2GainStepMidRecovery &
	    ~hb2GainStepMidRecoveryBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_HB2GAINSTEP1RECOVERY_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_AGC_OVRG_LOW_INT1_GAIN_STEP,
					   rxAgcCtrl->agcPeak.hb2GainStepMidRecovery);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (rxAgcCtrl->agcPeak.hb2GainStepAttack & ~hb2GainStepAttackBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_HB2GAINSTEPATTACK_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_AGC_OVRG_GAIN_STEP2,
					   rxAgcCtrl->agcPeak.hb2GainStepAttack);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if ((rxAgcCtrl->agcPeak.hb2OverloadPowerMode << 2) &
	    ~hb2OverloadPowerModeBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_HB2OVRLDPWRMODE_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteField(device->devHalInfo,
					    TALISE_ADDR_DEC_OVERLOAD_CONFIG2, rxAgcCtrl->agcPeak.hb2OverloadPowerMode,
					    hb2OverloadPowerModeBitMask, 2);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if ((rxAgcCtrl->agcPeak.hb2OvrgSel << 5) & ~hb2OvrgSelBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_HB2OVRLDSEL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_AGC_CONFIG1,
					    rxAgcCtrl->agcPeak.hb2OvrgSel, hb2OvrgSelBitMask, 5);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (rxAgcCtrl->agcPeak.hb2ThreshConfig != hb2ThreshConfigBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_HB2THRSHCFG_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Because this field was added after production, preserve the past behavior if 0 is passed by setting to default */
	if (rxAgcCtrl->agcPeak.hb2UnderRangeLowThreshExceededCnt == 0) {
		rxAgcCtrl->agcPeak.hb2UnderRangeLowThreshExceededCnt = 3;
	}
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_AGC_ADCOVRG_LOW_INT0_COUNTER,
				   rxAgcCtrl->agcPeak.hb2UnderRangeLowThreshExceededCnt);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Because this field was added after production, preserve the past behavior if 0 is passed by setting to default */
	if (rxAgcCtrl->agcPeak.hb2UnderRangeMidThreshExceededCnt == 0) {
		rxAgcCtrl->agcPeak.hb2UnderRangeMidThreshExceededCnt = 3;
	}
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_AGC_ADCOVRG_LOW_INT1_COUNTER,
				   rxAgcCtrl->agcPeak.hb2UnderRangeMidThreshExceededCnt);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if (&rxAgcCtrl->agcPower ==
	    NULL) { /* Check for null power data structure pointer */
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PWR_STRUCT_INIT, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		/* Power Configuration register */
		halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_DEC_POWER_CONFIG_1,
					  &agcRegister);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		agcRegister &= ~(powerEnableMeasurementBitMask | powerUseRfirOutBitMask |
				 powerUseBBDC2BitMask);
		agcRegister |= ((rxAgcCtrl->agcPower.powerEnableMeasurement > 0 ? 1 : 0)
				| ((rxAgcCtrl->agcPower.powerUseRfirOut > 0 ? 1 : 0) << 1)
				| ((rxAgcCtrl->agcPower.powerUseBBDC2 > 0 ? 1 : 0) << 3));

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_DEC_POWER_CONFIG_1,
					   agcRegister);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (rxAgcCtrl->agcPower.underRangeHighPowerThresh &
	    ~underRangeHighPowerThreshBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PWR_LWR0THRSH_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_AGC_LOCK_LEVEL_THRESHOLDS,
					   rxAgcCtrl->agcPower.underRangeHighPowerThresh);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (rxAgcCtrl->agcPower.underRangeLowPowerThresh &
	    ~underRangeLowPowerThreshBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PWR_LWR1THRSH_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_POWER_THRESHOLDS,
					   rxAgcCtrl->agcPower.underRangeLowPowerThresh);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (rxAgcCtrl->agcPower.underRangeHighPowerGainStepRecovery &
	    ~underRangeHighPowerGainStepRecoveryBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PWR_LWR0PWRGAINSTEP_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_LOWER0_THRESHOLD_GAIN_STEP,
					   rxAgcCtrl->agcPower.underRangeHighPowerGainStepRecovery);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (rxAgcCtrl->agcPower.underRangeLowPowerGainStepRecovery &
	    ~underRangeLowPowerGainStepRecoveryBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PWR_LWR1PWRGAINSTEP_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_LOWER1_THRESHOLD_GAIN_STEP,
					   rxAgcCtrl->agcPower.underRangeLowPowerGainStepRecovery);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (rxAgcCtrl->agcPower.powerMeasurementDuration &
	    ~powerMeasurementDurationBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PWR_MSR_DURATION_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_DEC_POWER_DURATION,
					   rxAgcCtrl->agcPower.powerMeasurementDuration);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	agcRegister = (uint8_t)(rxAgcCtrl->agcPower.rx1TddPowerMeasDuration);
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_UL_SIG_POWER_MEAS_DURATION1, agcRegister);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	agcRegister = (uint8_t)(rxAgcCtrl->agcPower.rx1TddPowerMeasDuration >> 8);
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_UL_SIG_POWER_MEAS_DURATION2, agcRegister);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	agcRegister = (uint8_t)(rxAgcCtrl->agcPower.rx1TddPowerMeasDelay);
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_UL_SIG_POWER_MEAS_DELAY1, agcRegister);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	agcRegister = (uint8_t)(rxAgcCtrl->agcPower.rx1TddPowerMeasDelay >> 8);
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_UL_SIG_POWER_MEAS_DELAY2, agcRegister);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	agcRegister = (uint8_t)(rxAgcCtrl->agcPower.rx2TddPowerMeasDuration);
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_UL_SIG_POWER_MEAS_DURATION1, agcRegister);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	agcRegister = (uint8_t)(rxAgcCtrl->agcPower.rx2TddPowerMeasDuration >> 8);
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_UL_SIG_POWER_MEAS_DURATION2, agcRegister);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	agcRegister = (uint8_t)(rxAgcCtrl->agcPower.rx2TddPowerMeasDelay);
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_UL_SIG_POWER_MEAS_DELAY1, agcRegister);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	agcRegister = (uint8_t)(rxAgcCtrl->agcPower.rx2TddPowerMeasDelay >> 8);
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_UL_SIG_POWER_MEAS_DELAY2, agcRegister);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if (rxAgcCtrl->agcPower.upper0PowerThresh & ~upper0PowerThreshBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PWR_UPPWR0THRSH_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_AGC_LOCK_LEVEL,
					   rxAgcCtrl->agcPower.upper0PowerThresh);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (rxAgcCtrl->agcPower.upper1PowerThresh & ~upper1PowerThreshBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PWR_UPPWR1THRSH_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_AGC_UPPER_POWER_THRESHOLD, rxAgcCtrl->agcPower.upper1PowerThresh);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if ((rxAgcCtrl->agcPower.powerLogShift << 2) & ~powerLogShiftBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PWR_LOGSHIFT_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_DEC_POWER_CONFIG_1,
					    rxAgcCtrl->agcPower.powerLogShift, powerLogShiftBitMask, 2);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* Because this field was added after production, preserve the past behavior if 0 is passed by setting to default */
	if (rxAgcCtrl->agcPower.overRangeLowPowerGainStepAttack == 0) {
		rxAgcCtrl->agcPower.overRangeLowPowerGainStepAttack = 4;
	}

	if ((rxAgcCtrl->agcPower.overRangeLowPowerGainStepAttack) &
	    ~overRangeLowPowerGainStepAttackBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PWR_UPPWR0GAINSTEP_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_UPPER0_THRESHOLD_GAIN_STEP,
					   rxAgcCtrl->agcPower.overRangeLowPowerGainStepAttack);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* Because this field was added after production, preserve the past behavior if 0 is passed by setting to default */
	if (rxAgcCtrl->agcPower.overRangeHighPowerGainStepAttack == 0) {
		rxAgcCtrl->agcPower.overRangeHighPowerGainStepAttack = 4;
	}

	if ((rxAgcCtrl->agcPower.overRangeHighPowerGainStepAttack) &
	    ~overRangeHighPowerGainStepAttackBitMask) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PWR_UPPWR1GAINSTEP_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_UPPER1_THRESHOLD_GAIN_STEP,
					   rxAgcCtrl->agcPower.overRangeHighPowerGainStepAttack);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getAgcCtrlRegisters(taliseDevice_t *device,
				    taliseAgcCfg_t *agcCtrl)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t agcRegister[3] = {0};
	uint32_t agcClock_Hz = 0;
	uint32_t agcGainUpdateCounter = 0;


	static const uint8_t agcPeakWaitTimeMask = 0x1F;
	static const uint8_t agcGainUpdateCounterMask = 0x3F;
	static const uint8_t agcLowThreshPreventGainMask = 0x20;
	static const uint8_t agcChangeGainIfThreshHighMask = 0x18;
	static const uint8_t agcPeakThreshGainControlModeMask = 0x04;
	static const uint8_t agcResetOnRxonMask = 0x02;
	static const uint8_t agcEnableSyncPulseForGainCounterMask = 0x01;
	static const uint8_t agcEnableIp3OptimizationThreshMask = 0x80;
	static const uint8_t agcEnableFastRecoveryLoopBitMask = 0x40;
	static const uint8_t ip3OverRangeThreshMask = 0x3F;
	static const uint8_t agcClock_HzBitMask = 0x03;
	static const uint8_t agcRx1AttackDelayBitMask = 0x3F;
	static const uint8_t agcRx2AttackDelayBitMask = 0x3F;
	static const uint8_t agcSlowLoopSettlingDelayBitMask = 0x7F;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getAgcCtrlRegisters()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* checking for valid taliseAgcCfg_t device->rx->rxAgcCtrl pointer to determine if it has been initialized */
	if (agcCtrl == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_RX_STRUCT_INIT, retVal, TALACT_ERR_CHECK_PARAM);
	}

	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_AGC_CONFIG1,
				   &agcCtrl->agcPeakWaitTime, agcPeakWaitTimeMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_MAX_GAIN_INDEX_RX1,
				  &agcCtrl->agcRx1MaxGainIndex);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_MIN_GAIN_INDEX_RX1,
				  &agcCtrl->agcRx1MinGainIndex);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_MAX_GAIN_INDEX_RX2,
				  &agcCtrl->agcRx2MaxGainIndex);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_MIN_GAIN_INDEX_RX2,
				  &agcCtrl->agcRx2MinGainIndex);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_GAIN_UPDATE_COUNTER1,
				  &agcRegister[0]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_GAIN_UPDATE_COUNTER2,
				  &agcRegister[1]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_GAIN_UPDATE_COUNTER3,
				  &agcRegister[2]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	agcGainUpdateCounter = ((uint32_t)(agcRegister[2] & agcGainUpdateCounterMask) <<
				16) | ((uint32_t)agcRegister[1] << 8) | agcRegister[0];

	/*Reading back AGC Clock Division Ratio*/
	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_5,
				   &agcRegister[0], agcClock_HzBitMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/*Calculation of AGC clock in Hz*/
	switch (agcRegister[0]) {
	case 0:
		agcClock_Hz = device->devStateInfo.clocks.hsDigClkDiv4or5_Hz;
		break;

	case 1:
		agcClock_Hz = (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 2);
		break;

	case 2:
		agcClock_Hz = (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 4);
		break;

	case 3:
		agcClock_Hz = (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 8);
		break;

	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_CLK_DIV_RATIO_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if(agcClock_Hz == 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_CLK_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Calculation of Gain Update Time in uS*/
	agcCtrl->agcGainUpdateCounter_us = (uint32_t)DIV_U64(((uint64_t)
					   agcGainUpdateCounter * 1000000), agcClock_Hz);

	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_AGC_ATTACK_DELAY_RX1,
				   &agcCtrl->agcRx1AttackDelay, agcRx1AttackDelayBitMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_AGC_ATTACK_DELAY_RX2,
				   &agcCtrl->agcRx2AttackDelay, agcRx2AttackDelayBitMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_SLOWLOOP_SETTLING_DELAY, &agcCtrl->agcSlowLoopSettlingDelay,
				   agcSlowLoopSettlingDelayBitMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_SLOWLOOP_CONFIG,
				  &agcRegister[0]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	agcCtrl->agcLowThreshPreventGain = (agcRegister[0] &
					    agcLowThreshPreventGainMask) >> 5;
	agcCtrl->agcChangeGainIfThreshHigh = (agcRegister[0] &
					      agcChangeGainIfThreshHighMask) >> 3;
	agcCtrl->agcPeakThreshGainControlMode = (agcRegister[0] &
						agcPeakThreshGainControlModeMask) >> 2;
	agcCtrl->agcResetOnRxon = (agcRegister[0] & agcResetOnRxonMask) >> 1;
	agcCtrl->agcEnableSyncPulseForGainCounter = agcRegister[0] &
			agcEnableSyncPulseForGainCounterMask;
	agcCtrl->agcEnableIp3OptimizationThresh = (agcRegister[0] &
			agcEnableIp3OptimizationThreshMask) >> 7;
	agcCtrl->agcEnableFastRecoveryLoop = (agcRegister[0] &
					      agcEnableFastRecoveryLoopBitMask) >> 6;

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_DEC_IP3_OVERRANGE_THRESHOLD, &agcCtrl->ip3OverRangeThresh,
				   ip3OverRangeThreshMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_AGC_IP3_OVERRANGE_THRESHOLD, &agcCtrl->ip3OverRangeThreshIndex);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_AGC_ADCOVRG_IP3_HIGH_COUNTER, &agcCtrl->ip3PeakExceededCnt);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getAgcPeakRegisters(taliseDevice_t *device,
				    taliseAgcPeak_t *agcPeak)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t agcRegister[2] = {0};
	uint32_t agcClock_Hz = 0;
	uint16_t agcUnderRangeLowInterval = 0;

	static const uint8_t apdHighThreshMask = 0x3F;
	static const uint8_t apdLowGainModeHighThreshMask = 0x3F;
	static const uint8_t apdLowThreshMask = 0x3F;
	static const uint8_t apdLowGainModeLowThreshMask = 0x3F;
	static const uint8_t apdGainStepAttackMask = 0x1F;
	static const uint8_t apdGainStepRecoveryMask = 0x1F;
	static const uint8_t enableHb2OverloadMask = 0x80;
	static const uint8_t hb2OverloadDurationCntMask = 0x70;
	static const uint8_t hb2OverloadThreshCntMask = 0x0F;
	static const uint8_t hb2GainStepHighRecoveryMask = 0x1F;
	static const uint8_t hb2GainStepLowRecoveryMask = 0x1F;
	static const uint8_t hb2GainStepMidRecoveryMask = 0x1F;
	static const uint8_t hb2GainStepAttackMask = 0x1F;
	static const uint8_t hb2OverloadPowerModeMask = 0x04;
	static const uint8_t hb2OvrgSelMask = 0x20;
	static const uint8_t hb2ThreshConfigMask = 0x03;
	static const uint8_t agcClock_HzBitMask = 0x03;
	static const uint8_t agcUnderRangeMidIntervalMask = 0x3F;
	static const uint8_t agcUnderRangeHighIntervalMask = 0x3F;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getAgcPeakRegisters()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* checking for valid taliseAgcCfg_t device->rx->rxAgcCtrl pointer to determine if it has been initialized */
	if (agcPeak == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PKK_STRUCT_INIT, retVal, TALACT_ERR_CHECK_PARAM);
	}

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_AGC_UNDERRANGE0_0,
				  &agcRegister[0]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_AGC_UNDERRANGE0_1,
				  &agcRegister[1]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	agcUnderRangeLowInterval = (((uint16_t)agcRegister[1] << 8) | agcRegister[0]);

	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_5,
				   &agcRegister[0], agcClock_HzBitMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/*Calculation of AGC clock in Hz*/
	switch (agcRegister[0]) {
	case 0:
		agcClock_Hz = device->devStateInfo.clocks.hsDigClkDiv4or5_Hz;
		break;

	case 1:
		agcClock_Hz = (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 2);
		break;

	case 2:
		agcClock_Hz = (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 4);
		break;

	case 3:
		agcClock_Hz = (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 8);
		break;

	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_CLK_DIV_RATIO_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (agcClock_Hz == 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_CLK_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Calculation of Gain Update Time in ns*/
	agcPeak->agcUnderRangeLowInterval_ns = (uint32_t)DIV_U64(((
			uint64_t)agcUnderRangeLowInterval * 1000000000), agcClock_Hz);

	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_AGC_UNDERRANGE1,
				   &agcPeak->agcUnderRangeMidInterval, agcUnderRangeMidIntervalMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_AGC_UNDERRANGE2,
				   &agcPeak->agcUnderRangeHighInterval, agcUnderRangeHighIntervalMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_UPPER_LEVEL_BLOCKER_THRESHOLD, &agcPeak->apdHighThresh,
				   apdHighThreshMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_UPPER_LEVEL_BLOCKER_THRESHOLD2, &agcPeak->apdLowGainModeHighThresh,
				   apdLowGainModeHighThreshMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_LOWER_LEVEL_BLOCKER_THRESHOLD, &agcPeak->apdLowThresh,
				   apdLowThreshMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_LOWER_LEVEL_BLOCKER_THRESHOLD2, &agcPeak->apdLowGainModeLowThresh,
				   apdLowGainModeLowThreshMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_ULB_COUNT_THRESHOLD,
				  &agcPeak->apdUpperThreshPeakExceededCnt);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_LLB_COUNT_THRESHOLD,
				  &agcPeak->apdLowerThreshPeakExceededCnt);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_AGC_OVRG_GAIN_STEP1,
				   &agcPeak->apdGainStepAttack, apdGainStepAttackMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_AGC_OVRG_GAIN_STEP4,
				   &agcPeak->apdGainStepRecovery, apdGainStepRecoveryMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_DEC_OVERLOAD_CONFIG1,
				  &agcRegister[0]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	agcPeak->enableHb2Overload = (agcRegister[0] & enableHb2OverloadMask) >> 7;
	agcPeak->hb2OverloadDurationCnt = (agcRegister[0] & hb2OverloadDurationCntMask)
					  >> 4;
	agcPeak->hb2OverloadThreshCnt = agcRegister[0] & hb2OverloadThreshCntMask;

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_DEC_OVERLOAD_UPPER_THRESHOLD, &agcPeak->hb2HighThresh);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_DEC_UNDERRANGE_INTERVAL0_THRESHOLD,
				  &agcPeak->hb2UnderRangeLowThresh);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_DEC_UNDERRANGE_INTERVAL1_THRESHOLD,
				  &agcPeak->hb2UnderRangeMidThresh);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_DEC_OVERLOAD_LOWER_THRESHOLD, &agcPeak->hb2UnderRangeHighThresh);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_ADC_HIGH_OVRG_COUNT_THRESHOLD,
				  &agcPeak->hb2UpperThreshPeakExceededCnt);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_ADC_LOW_OVRG_COUNT_THRESHOLD,
				  &agcPeak->hb2LowerThreshPeakExceededCnt);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_AGC_OVRG_GAIN_STEP5,
				   &agcPeak->hb2GainStepHighRecovery, hb2GainStepHighRecoveryMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_AGC_OVRG_LOW_INT0_GAIN_STEP, &agcPeak->hb2GainStepLowRecovery,
				   hb2GainStepLowRecoveryMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_AGC_OVRG_LOW_INT1_GAIN_STEP, &agcPeak->hb2GainStepMidRecovery,
				   hb2GainStepMidRecoveryMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_AGC_OVRG_GAIN_STEP2,
				   &agcPeak->hb2GainStepAttack, hb2GainStepAttackMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_DEC_OVERLOAD_CONFIG2,
				  &agcRegister[0]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	agcPeak->hb2OverloadPowerMode = (agcRegister[0] & hb2OverloadPowerModeMask) >>
					2;
	agcPeak->hb2ThreshConfig = agcRegister[0] & hb2ThreshConfigMask;

	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_AGC_CONFIG1,
				   &agcPeak->hb2OvrgSel, hb2OvrgSelMask, 5);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_AGC_ADCOVRG_LOW_INT0_COUNTER,
				  &agcPeak->hb2UnderRangeLowThreshExceededCnt);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_AGC_ADCOVRG_LOW_INT1_COUNTER,
				  &agcPeak->hb2UnderRangeMidThreshExceededCnt);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getAgcPowerRegisters(taliseDevice_t *device,
				     taliseAgcPower_t *agcPower)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t agcRegister[3] = {0};

	static const uint8_t powerEnableMeasurementMask = 0x01;
	static const uint8_t powerUseRfirOutMask = 0x02;
	static const uint8_t powerUseBBDC2Mask = 0x08;
	static const uint8_t underRangeHighPowerThreshMask = 0x7F;
	static const uint8_t underRangeLowPowerThreshMask = 0x1F;
	static const uint8_t underRangeHighPowerGainStepRecoveryMask = 0x1F;
	static const uint8_t underRangeLowPowerGainStepRecoveryMask = 0x1F;
	static const uint8_t powerMeasurementDurationMask = 0x1F;
	static const uint8_t upper0PowerThreshMask = 0x7F;
	static const uint8_t upper1PowerThreshMask = 0x0F;
	static const uint8_t powerLogShiftMask = 0x04;
	static const uint8_t overRangeLowPowerGainStepAttackBitMask = 0x1F;
	static const uint8_t overRangeHighPowerGainStepAttackBitMask = 0x1F;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getAgcPowerRegisters()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* checking for valid taliseAgcCfg_t device->rx->rxAgcCtrl pointer to determine if it has been initialized */
	if (agcPower == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_PWR_STRUCT_INIT, retVal, TALACT_ERR_CHECK_PARAM);
	}

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_DEC_POWER_CONFIG_1,
				  &agcRegister[0]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	agcPower->powerEnableMeasurement = agcRegister[0] & powerEnableMeasurementMask;
	agcPower->powerUseRfirOut = (agcRegister[0] & powerUseRfirOutMask) >> 1;
	agcPower->powerUseBBDC2 = (agcRegister[0] & powerUseBBDC2Mask) >> 3;
	agcPower->powerLogShift = (agcRegister[0] & powerLogShiftMask) >> 2;

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_AGC_LOCK_LEVEL_THRESHOLDS, &agcPower->underRangeHighPowerThresh,
				   underRangeHighPowerThreshMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_POWER_THRESHOLDS,
				   &agcPower->underRangeLowPowerThresh, underRangeLowPowerThreshMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_LOWER0_THRESHOLD_GAIN_STEP,
				   &agcPower->underRangeHighPowerGainStepRecovery,
				   underRangeHighPowerGainStepRecoveryMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_LOWER1_THRESHOLD_GAIN_STEP,
				   &agcPower->underRangeLowPowerGainStepRecovery,
				   underRangeLowPowerGainStepRecoveryMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_DEC_POWER_DURATION,
				   &agcPower->powerMeasurementDuration, powerMeasurementDurationMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_RX1_UL_SIG_POWER_MEAS_DURATION1, &agcRegister[0]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_RX1_UL_SIG_POWER_MEAS_DURATION2, &agcRegister[1]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	agcPower->rx1TddPowerMeasDuration = (((uint16_t)agcRegister[1] << 8) |
					     agcRegister[0]);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_RX1_UL_SIG_POWER_MEAS_DELAY1, &agcRegister[0]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_RX1_UL_SIG_POWER_MEAS_DELAY2, &agcRegister[1]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	agcPower->rx1TddPowerMeasDelay = (((uint16_t)agcRegister[1] << 8) |
					  agcRegister[0]);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_RX2_UL_SIG_POWER_MEAS_DURATION1, &agcRegister[0]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_RX2_UL_SIG_POWER_MEAS_DURATION2, &agcRegister[1]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	agcPower->rx2TddPowerMeasDuration = (((uint16_t)agcRegister[1] << 8) |
					     agcRegister[0]);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_RX2_UL_SIG_POWER_MEAS_DELAY1, &agcRegister[0]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_RX2_UL_SIG_POWER_MEAS_DELAY2, &agcRegister[1]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	agcPower->rx2TddPowerMeasDelay = (((uint16_t)agcRegister[1] << 8) |
					  agcRegister[0]);

	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_AGC_LOCK_LEVEL,
				   &agcPower->upper0PowerThresh, upper0PowerThreshMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_AGC_UPPER_POWER_THRESHOLD, &agcPower->upper1PowerThresh,
				   upper1PowerThreshMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_UPPER0_THRESHOLD_GAIN_STEP,
				   &agcPower->overRangeLowPowerGainStepAttack,
				   overRangeLowPowerGainStepAttackBitMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_UPPER1_THRESHOLD_GAIN_STEP,
				   &agcPower->overRangeHighPowerGainStepAttack,
				   overRangeHighPowerGainStepAttackBitMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

static talRecoveryActions_t talSetupDualBandRx1Agc(taliseDevice_t *device,
		taliseAgcDualBandCfg_t *rxAgcCtrlDualBand)
{
	uint8_t registerValue = 0;
	uint16_t currGpio3p3Oe = 0;
	uint16_t usedGpio3p3pins = 0;
	uint16_t gpio3p3FreeMask = 0;
	uint16_t gpio3p3UsedMask = 0;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	const uint8_t RX1_DUALBAND_EXT_LNA_SOURCE_CONTROL = 0x05;

	/* range check the gain against the max and min expected values */
	if ((rxAgcCtrlDualBand->agcRxDualbandExtTableUpperIndex >
	     device->devStateInfo.gainIndexes.rx1MaxGainIndex)
	    || (rxAgcCtrlDualBand->agcRxDualbandExtTableLowerIndex <
		device->devStateInfo.gainIndexes.rx1MinGainIndex)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_SETUPDUALBANDRXAGC_GAIN_OUT_OF_RANGE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check current configuration of GPIO 3.3s */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_3P3V_LOWER_BYTE_SOURCE_CONTROL, &registerValue);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	retVal = (talRecoveryActions_t)TALISE_getGpio3v3Oe(device, &currGpio3p3Oe);
	IF_ERR_RETURN(retVal);

	/* Free GPIO3.3[3:0] if used */
	if (((registerValue & 0x0F) == RX1_DUALBAND_EXT_LNA_SOURCE_CONTROL) &&
	    ((currGpio3p3Oe & 0x0F) == 0x0F)) {
		gpio3p3FreeMask = 0x0F;
		usedGpio3p3pins = device->devStateInfo.usedGpio3p3pins & ~gpio3p3FreeMask;
	}

	/* Update GPIO3p3 source control and used masks if enabled*/
	if (rxAgcCtrlDualBand->dualBandGpioEnable > 0) {
		/* check if GPIO's are available for the new configuration*/
		gpio3p3UsedMask = 0x0F;
		if ((usedGpio3p3pins & gpio3p3UsedMask) != 0) {
			return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						TAL_ERR_SETUPDUALBANDRX1AGC_GPIO3P3_IN_USE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		usedGpio3p3pins |= gpio3p3UsedMask;
	}

	/* Set the source control */
	registerValue = (registerValue & ~0x0F ) | ((
				rxAgcCtrlDualBand->dualBandGpioEnable > 0) ?
			RX1_DUALBAND_EXT_LNA_SOURCE_CONTROL : 0x00);
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GPIO_3P3V_LOWER_BYTE_SOURCE_CONTROL, registerValue);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Set GPIO input enables */
	/* - Set OE for all freed GPIO3p3's to 0 (input) */
	/* - Set OE for all used GPIO3p3's to 1 (output) */
	retVal = (talRecoveryActions_t)TALISE_setGpio3v3Oe(device, gpio3p3UsedMask,
			(gpio3p3UsedMask | gpio3p3FreeMask));
	IF_ERR_RETURN(retVal);

	/* Update used and freed GPIO's */
	device->devStateInfo.usedGpio3p3pins = usedGpio3p3pins;

	/* Set upper LNA index */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_AGC_DUALBAND_INDEX_X,
				   rxAgcCtrlDualBand->agcRxDualbandExtTableUpperIndex );
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Set lower LNA index */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_AGC_DUALBAND_INDEX_Y,
				   rxAgcCtrlDualBand->agcRxDualbandExtTableLowerIndex );
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Enable Dec power for the channel. Also clear bitfield to ensure that Dec power calculation is done before compensation */
	registerValue = (rxAgcCtrlDualBand->dualBandGpioEnable > 0) ? 0x03 : 0x00;
	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_DDC_DEC_POWER_CONFIG, registerValue, 0x33, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);

	return retVal;
}

static talRecoveryActions_t talSetupDualBandRx2Agc(taliseDevice_t *device,
		taliseAgcDualBandCfg_t *rxAgcCtrlDualBand)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t registerValue = 0;
	uint16_t currGpio3p3Oe = 0;
	uint16_t usedGpio3p3pins = 0;
	uint16_t gpio3p3FreeMask = 0;
	uint16_t gpio3p3UsedMask = 0;

	const uint8_t RX2_DUALBAND_EXT_LNA_SOURCE_CONTROL = 0x05;

	/* range check the gain against the max and min expected values */
	if ((rxAgcCtrlDualBand->agcRxDualbandExtTableUpperIndex >
	     device->devStateInfo.gainIndexes.rx2MaxGainIndex)
	    || (rxAgcCtrlDualBand->agcRxDualbandExtTableLowerIndex <
		device->devStateInfo.gainIndexes.rx2MinGainIndex)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_SETUPDUALBANDRXAGC_GAIN_OUT_OF_RANGE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check current configuration of GPIO 3.3s */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_3P3V_LOWER_BYTE_SOURCE_CONTROL, &registerValue);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	retVal = (talRecoveryActions_t)TALISE_getGpio3v3Oe(device, &currGpio3p3Oe);
	IF_ERR_RETURN(retVal);

	/* Free GPIO3.3[7:4] if used */
	if ((((registerValue & 0xF0) >> 4) == RX2_DUALBAND_EXT_LNA_SOURCE_CONTROL) &&
	    ((currGpio3p3Oe & 0xF0) == 0xF0)) {
		gpio3p3FreeMask = 0xF0;
		usedGpio3p3pins = device->devStateInfo.usedGpio3p3pins & ~gpio3p3FreeMask;
	}

	/* Update GPIO3p3 source control and used masks if enabled*/
	if (rxAgcCtrlDualBand->dualBandGpioEnable > 0) {
		/* check if GPIO's are available for the new configuration*/
		gpio3p3UsedMask = 0xF0;
		if ((usedGpio3p3pins & gpio3p3UsedMask) != 0) {
			return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						TAL_ERR_SETUPDUALBANDRX2AGC_GPIO3P3_IN_USE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		usedGpio3p3pins |= gpio3p3UsedMask;
	}

	/* Set the source control */
	registerValue = (registerValue & ~0xF0 ) | (((
				rxAgcCtrlDualBand->dualBandGpioEnable > 0) ?
			RX2_DUALBAND_EXT_LNA_SOURCE_CONTROL : 0x00) << 4);
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GPIO_3P3V_LOWER_BYTE_SOURCE_CONTROL, registerValue);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Set GPIO input enables */
	/* - Set OE for all freed GPIO3p3's to 0 (input) */
	/* - Set OE for all used GPIO3p3's to 1 (output) */
	retVal = (talRecoveryActions_t)TALISE_setGpio3v3Oe(device, gpio3p3UsedMask,
			(gpio3p3UsedMask | gpio3p3FreeMask));
	IF_ERR_RETURN(retVal);

	/* Update used and freed GPIO's */
	device->devStateInfo.usedGpio3p3pins = usedGpio3p3pins;

	/* Set upper LNA index */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_AGC_DUALBAND_INDEX_X,
				   rxAgcCtrlDualBand->agcRxDualbandExtTableUpperIndex );
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Set lower LNA index */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_AGC_DUALBAND_INDEX_Y,
				   rxAgcCtrlDualBand->agcRxDualbandExtTableLowerIndex );
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Enable Dec power for the channel. Also clear bitfield to ensure that Dec power calculation is done before compensation */
	registerValue = (rxAgcCtrlDualBand->dualBandGpioEnable > 0) ? 0x0C : 0x00;
	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_DDC_DEC_POWER_CONFIG, registerValue, 0xCC, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);

	return retVal;
}

uint32_t TALISE_setupDualBandRxAgc( taliseDevice_t *device,
				    taliseRxChannels_t rxChannel, taliseAgcDualBandCfg_t *rxAgcCtrlDualBand)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setupDualBandRxAgc()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Check: Channel */
	if (rxChannel == TAL_RXOFF) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETUPDUALBANDRXAGC_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check: Null pointer */
	if ( rxAgcCtrlDualBand == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETUPDUALBANDRXAGC_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check: Power margin */
	if (rxAgcCtrlDualBand->agcDualbandPwrMargin > 0x1F) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETUPDUALBANDRXAGC_INV_PWRMARGIN, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check: Dec power measurement duration */
	if (rxAgcCtrlDualBand->decPowerDdcMeasurementDuration > 0x1F) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETUPDUALBANDRXAGC_INV_DECPWR, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* range check the gain table indices upper > lower */
	if ((rxAgcCtrlDualBand->agcRxDualbandExtTableUpperIndex <=
	     rxAgcCtrlDualBand->agcRxDualbandExtTableLowerIndex)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETUPDUALBANDRXAGC_GAIN_RANGE_MISMATCH, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Write Rx1 specific data */
	if ((rxChannel == TAL_RX1) ||
	    (rxChannel == TAL_RX1RX2)) {
		retVal = talSetupDualBandRx1Agc(device, rxAgcCtrlDualBand);
	}

	/* Write Rx2 specific data */
	if ((rxChannel == TAL_RX2) ||
	    (rxChannel == TAL_RX1RX2)) {
		retVal = talSetupDualBandRx2Agc(device, rxAgcCtrlDualBand);
	}

	/* Enable dualband mode */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SLOWLOOP_CONFIG2,
				   (uint8_t)((rxAgcCtrlDualBand->agcDualBandEnable > 0) ? 1 : 0));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Power margin */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_AGC_DUALBAND_TOTAL_PWR_MARGIN,
				   rxAgcCtrlDualBand->agcDualbandPwrMargin);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Band power margin */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_AGC_DUALBAND_BAND_PWR_MARGIN,
				   rxAgcCtrlDualBand->agcDualbandLnaStep);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* High LNA threshold */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_AGC_DUALBAND_HIGH_LNA_THRESHOLD,
				   rxAgcCtrlDualBand->agcDualbandHighLnaThreshold);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Low LNA threshold */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_AGC_DUALBAND_LOW_LNA_THRESHOLD,
				   rxAgcCtrlDualBand->agcDualbandLowLnaThreshold);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Dec power measurement duration */
	/* Also enable measurement control through AGC, and improved dynamic range of measured power */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_DDC_DEC_POWER_MEAS,
				   ((rxAgcCtrlDualBand->decPowerDdcMeasurementDuration & 0x1F) | 0x60));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);

	return (uint32_t)retVal;
}

uint32_t TALISE_getDualBandLnaControls(taliseDevice_t *device,
				       taliseRxChannels_t rxChannel,
				       taliseDualBandLnaControls_t *rxDualBandLnaControls)
{
	uint16_t lowerBandAddr = 0;
	uint16_t upperBandAddr = 0;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getDualBandLnaControls()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Read current value of Gain inc/dec and enable */
	switch (rxChannel) {
	case TAL_RX1:
		lowerBandAddr = TALISE_ADDR_GAIN_TABLE_DATA_OUTPUT_DUALBAND_CH1_BAND_A;
		upperBandAddr = TALISE_ADDR_GAIN_TABLE_DATA_OUTPUT_DUALBAND_CH1_BAND_B;
		break;

	case TAL_RX2:
		lowerBandAddr = TALISE_ADDR_GAIN_TABLE_DATA_OUTPUT_DUALBAND_CH2_BAND_A;
		upperBandAddr = TALISE_ADDR_GAIN_TABLE_DATA_OUTPUT_DUALBAND_CH2_BAND_B;
		break;

	default: {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETDUALBANDLNA_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}
	}

	/*Check passed pointers for NULL */
	if (rxDualBandLnaControls == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETDUALBANDLNA_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Disable gain table read - Otherwise, the readback will read a forced index */
	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_GAIN_TABLE_CONFIGURATION, 0x0, 0x04, 2);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Read control word for the current LNA index */
	halError = talSpiReadField(device->devHalInfo, lowerBandAddr,
				   &rxDualBandLnaControls->rxLowerBandLnaControl, 0x03, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiReadField(device->devHalInfo, upperBandAddr,
				   &rxDualBandLnaControls->rxUpperBandLnaControl, 0x03, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_setRxAgcMinMaxGainIndex(taliseDevice_t *device,
					taliseRxChannels_t rxChannel, uint8_t maxGainIndex, uint8_t minGainIndex)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setRxAgcMinMaxGainIndex()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Ensure that Max Gain Index is always greater than Min Gain Index */
	if(minGainIndex >= maxGainIndex) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_AGC_RX_MIN_GAIN_GRT_THAN_MAX_GAIN_PARM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if((rxChannel == TAL_RX1) || (rxChannel == TAL_RX1RX2)) {
		/*Ensure that requested min and max gain indexes are within the range supported by the current chip config*/
		if ((maxGainIndex > device->devStateInfo.gainIndexes.rx1MaxGainIndex) ||
		    (minGainIndex < device->devStateInfo.gainIndexes.rx1MinGainIndex)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_INV_AGC_RX_MIN_MAX_GAIN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* Writing Rx1 max and min gain indices to the SPI registers*/
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MAX_GAIN_INDEX_RX1,
					   maxGainIndex);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MIN_GAIN_INDEX_RX1,
					   minGainIndex);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if((rxChannel == TAL_RX2) || (rxChannel == TAL_RX1RX2)) {
		/*Ensure that requested min and max gain indexes are within the range supported by the current chip config*/
		if ((maxGainIndex > device->devStateInfo.gainIndexes.rx2MaxGainIndex) ||
		    (minGainIndex < device->devStateInfo.gainIndexes.rx2MinGainIndex)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_INV_AGC_RX_MIN_MAX_GAIN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* Writing Rx2 max and min gain indices to the SPI registers*/
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MAX_GAIN_INDEX_RX2,
					   maxGainIndex);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MIN_GAIN_INDEX_RX2,
					   minGainIndex);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_resetRxAgc(taliseDevice_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	static const uint8_t AGC_RESET = 1;
	static const uint8_t AGC_RESET_CLEAR = 0;
	static const uint8_t AGC_RESET_MASK = 0x10;
	static const uint8_t AGC_START_BIT = 4;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_resetRxAgc()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Soft Reset AGC */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_AGC_CONFIG2,
				    AGC_RESET, AGC_RESET_MASK, AGC_START_BIT);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/*Release AGC Reset*/
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_AGC_CONFIG2,
				    AGC_RESET_CLEAR, AGC_RESET_MASK, AGC_START_BIT);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}
