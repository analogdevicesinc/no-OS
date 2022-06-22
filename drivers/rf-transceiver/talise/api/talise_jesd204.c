// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_jesd204.c
 * \brief Contains functions to support Talise JESD204b data interface
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "talise_jesd204.h"
#include "talise_reg_addr_macros.h"
#include "talise_arm_macros.h"
#include "talise_hal.h"
#include "talise_user.h"
#include "talise_error.h"
#include "talise.h"

uint32_t TALISE_setupSerializers(taliseDevice_t *device, taliseInit_t *init)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t laneInvert = 0;
	uint8_t activeFramersMask =
		0; /* bit per framer to for quick access of valid framers enabled. */
	uint8_t activeLanesEnabled = 0;
	uint8_t activeLanesPowerDown = 0xF;
	uint8_t enPreEmphasisMask = 0x00; /* bit per lane */
	uint8_t preEmp = 0x00; /* same value for all lanes */
	uint8_t txser_div = 0;
	uint8_t i = 0;
	uint8_t obsRxL = 0;
	uint8_t rxL = 0;
	uint32_t rxLaneRate_kHz = 0;
	uint32_t obsRxLaneRate_kHz = 0;
	uint32_t fasterLaneRate_kHz = 0;
	uint32_t slowerLaneRate_kHz = 0;
	uint8_t txser_div_reg = 0;
	uint8_t serAmplitude = 0;
	taliseJesd204bFramerConfig_t *currentFramer = NULL;

	static const uint8_t DUALBANDENABLED = 1;

	/* Determine which framer structures are valid */
	if (device->devStateInfo.profilesValid & RX_PROFILE_VALID) {
		switch(init->rx.framerSel) {
		case TAL_FRAMER_A:
			activeFramersMask |= 0x01;
			activeLanesEnabled |= init->jesd204Settings.framerA.serializerLanesEnabled;
			laneInvert |= ((init->jesd204Settings.framerA.serializerLanesEnabled) &
				       (init->jesd204Settings.serInvertLanePolarity));
			currentFramer = &init->jesd204Settings.framerA;
			break;
		case TAL_FRAMER_B:
			activeFramersMask |= 0x02;
			activeLanesEnabled |= init->jesd204Settings.framerB.serializerLanesEnabled;
			laneInvert |= ((init->jesd204Settings.framerB.serializerLanesEnabled) &
				       (init->jesd204Settings.serInvertLanePolarity));
			currentFramer = &init->jesd204Settings.framerB;
			break;
		case TAL_FRAMER_A_AND_B:
			activeFramersMask |= 0x03;
			activeLanesEnabled |= init->jesd204Settings.framerA.serializerLanesEnabled;
			activeLanesEnabled |= init->jesd204Settings.framerB.serializerLanesEnabled;
			laneInvert |= ((init->jesd204Settings.framerA.serializerLanesEnabled) &
				       (init->jesd204Settings.serInvertLanePolarity));
			laneInvert |= ((init->jesd204Settings.framerB.serializerLanesEnabled) &
				       (init->jesd204Settings.serInvertLanePolarity));
			currentFramer = &init->jesd204Settings.framerA;
			break;
		default:
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SER_INV_RXFRAMER_SEL, retVal, TALACT_ERR_CHECK_PARAM);
		}

		serAmplitude = init->jesd204Settings.serAmplitude;
		preEmp = init->jesd204Settings.serPreEmphasis;
		enPreEmphasisMask |= (init->jesd204Settings.serPreEmphasis > 0) ?
				     activeLanesEnabled : 0;

		/* TODO: Need error check to make sure that if FRAMER A and B, then make sure each framer has same number of lanes, and M matches */
		for (i = 0; i < 4; i++) {
			rxL += ((currentFramer->serializerLanesEnabled >> i) & 0x01);
		}

		if (rxL > 0) {
			rxLaneRate_kHz = currentFramer->Np * 10 * init->rx.rxProfile.rxOutputRate_kHz *
					 currentFramer->M / (8 * rxL);
			if ((rxLaneRate_kHz != 0) && ((rxLaneRate_kHz < 768000)
						      || (rxLaneRate_kHz > 12288000))) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_SER_INV_LANERATE_PARM, retVal, TALACT_ERR_CHECK_PARAM);
			}
		}

		if ((rxL != 0) &&
		    (rxL != 1) &&
		    (rxL != 2) &&
		    (rxL != 4)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SER_INV_L_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if ((currentFramer->Np != 12) &&
		    (currentFramer->Np != 16) &&
		    (currentFramer->Np != 24)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SER_INV_NP_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if ((currentFramer->M == 1)
		    && !(init->rx.rxProfile.rxDdcMode == TAL_RXDDC_INT2_REALIF)
		    && !(init->rx.rxProfile.rxDdcMode == TAL_RXDDC_DEC2_REALIF)
		    && !(init->rx.rxProfile.rxDdcMode == TAL_RXDDC_FILTERONLY_REALIF)
		    && !(init->rx.rxProfile.rxDdcMode == TAL_RXDDC_BYPASS_REALIF)) {
			return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						TAL_ERR_SER_INV_ZIF_TO_RIF_DATA_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if ((currentFramer->M == 8) &&
		    !(device->devStateInfo.rxDualBandEnabled != DUALBANDENABLED)) {
			return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						TAL_ERR_SER_INV_DUALBAND_DATA_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if ((currentFramer->M != 0) &&
		    (currentFramer->M != 1) &&
		    (currentFramer->M != 2) &&
		    (currentFramer->M != 4)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SER_INV_M_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if (currentFramer->serializerLanesEnabled > 15) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SER_INV_LANEEN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}
	}

	if (device->devStateInfo.profilesValid & ORX_PROFILE_VALID) {
		switch(init->obsRx.framerSel) {
		case TAL_FRAMER_A:
			activeFramersMask |= 0x01;
			activeLanesEnabled |= init->jesd204Settings.framerA.serializerLanesEnabled;
			laneInvert |= ((init->jesd204Settings.framerA.serializerLanesEnabled) &
				       (init->jesd204Settings.serInvertLanePolarity));
			currentFramer = &init->jesd204Settings.framerA;
			break;
		case TAL_FRAMER_B:
			activeFramersMask |= 0x02;
			activeLanesEnabled |= init->jesd204Settings.framerB.serializerLanesEnabled;
			laneInvert |= ((init->jesd204Settings.framerB.serializerLanesEnabled) &
				       (init->jesd204Settings.serInvertLanePolarity));
			currentFramer = &init->jesd204Settings.framerB;
			break;
		case TAL_FRAMER_A_AND_B:
			activeFramersMask |= 0x03;
			activeLanesEnabled |= init->jesd204Settings.framerA.serializerLanesEnabled;
			activeLanesEnabled |= init->jesd204Settings.framerB.serializerLanesEnabled;
			laneInvert |= ((init->jesd204Settings.framerA.serializerLanesEnabled) &
				       (init->jesd204Settings.serInvertLanePolarity));
			laneInvert |= ((init->jesd204Settings.framerB.serializerLanesEnabled) &
				       (init->jesd204Settings.serInvertLanePolarity));
			currentFramer = &init->jesd204Settings.framerA;
			break;
		default:
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SER_INV_ORXFRAMER_SEL, retVal, TALACT_ERR_CHECK_PARAM);
		}

		serAmplitude = init->jesd204Settings.serAmplitude;
		preEmp = init->jesd204Settings.serPreEmphasis;
		enPreEmphasisMask |= (init->jesd204Settings.serPreEmphasis > 0) ?
				     activeLanesEnabled : 0;

		/* TODO: Need error check to make sure that if FRAMER A and B, then make sure each framer has same number of lanes, and M matches */
		for(i = 0; i < 4; i++) {
			obsRxL += ((currentFramer->serializerLanesEnabled >> i) & 0x01);
		}

		if (obsRxL > 0) {
			obsRxLaneRate_kHz = currentFramer->Np * 10 *
					    init->obsRx.orxProfile.orxOutputRate_kHz * currentFramer->M / (8 * obsRxL);
			if ((obsRxLaneRate_kHz != 0) && ((obsRxLaneRate_kHz < 768000)
							 || (obsRxLaneRate_kHz > 12288000))) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_SER_INV_ORX_LANERATE_PARM, retVal, TALACT_ERR_CHECK_PARAM);
			}
		}

		if ((obsRxL != 0) &&
		    (obsRxL != 1) &&
		    (obsRxL != 2) &&
		    (obsRxL != 4)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SER_INV_ORX_L_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if ((currentFramer->Np != 12) &&
		    (currentFramer->Np != 16) &&
		    (currentFramer->Np != 24)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SER_INV_ORX_NP_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if ((currentFramer->M != 0) &&
		    (currentFramer->M != 1) &&
		    (currentFramer->M != 2) &&
		    (currentFramer->M != 4)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SER_INV_ORX_M_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if (currentFramer->serializerLanesEnabled > 15) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SER_INV_ORX_LANEEN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}
	}

	if (activeFramersMask == 0x03) {
		/* Verify FramerA and FramerB do not share physical lanes */
		if (init->jesd204Settings.framerA.serializerLanesEnabled &
		    init->jesd204Settings.framerB.serializerLanesEnabled
		   ) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SER_LANE_CONFLICT_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}
	}

	if (init->jesd204Settings.serAmplitude > 15) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SER_INV_AMP_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (init->jesd204Settings.serPreEmphasis > 4) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SER_INV_PREEMP_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (init->jesd204Settings.serInvertLanePolarity > 15) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SER_INV_LANEPN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Calculate needed JESD204 serializer lane rates */
	if ((rxLaneRate_kHz > 0) &&
	    (obsRxLaneRate_kHz > 0)) {
		fasterLaneRate_kHz = (rxLaneRate_kHz >= obsRxLaneRate_kHz) ?
				     (rxLaneRate_kHz) : (obsRxLaneRate_kHz);
		slowerLaneRate_kHz = (rxLaneRate_kHz >= obsRxLaneRate_kHz) ?
				     (obsRxLaneRate_kHz) : (rxLaneRate_kHz);

		/* Verify that lane rates are integer multiples of each other */
		if (fasterLaneRate_kHz % slowerLaneRate_kHz != 0) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SER_LANE_RATE_CONFLICT_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}
	} else {
		fasterLaneRate_kHz = (rxLaneRate_kHz >= obsRxLaneRate_kHz) ?
				     (rxLaneRate_kHz) : (obsRxLaneRate_kHz);
	}

	/* performing integer multiple check on HS clock and lane rate clock */
	if (fasterLaneRate_kHz == 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SER_LANERATE_ZERO, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((init->clocks.clkPllVcoFreq_kHz % fasterLaneRate_kHz) != 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_HS_AND_LANE_RATE_NOT_INTEGER_MULT, retVal, TALACT_ERR_CHECK_PARAM);
	}

	txser_div = (uint8_t)(init->clocks.clkPllVcoFreq_kHz / fasterLaneRate_kHz);

	switch(txser_div) {
	case 1:
		txser_div_reg = 0;
		break;
	case 2:
		txser_div_reg = 1;
		break;
	case 4:
		txser_div_reg = 2;
		break;
	case 8:
		txser_div_reg = 2;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SER_INV_TXSER_DIV_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Set Tx serializer divider */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_3,
				    txser_div_reg, 0x03, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Power down all lanes */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_JESD_FRAMER_SRST_CFG,
				   0xFF); /* hold framer output data in reset */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Reset serializers */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SER_PHY_PD,
				   0x1E); /* power down all lanes */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	activeLanesPowerDown = (~((activeLanesEnabled & 0x09) | ((
					  activeLanesEnabled & 0x02) << 1) | ((activeLanesEnabled & 0x04) >> 1)) & 0x0F);
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SER_PHY_PD,
				   (0x80 | (activeLanesPowerDown << 1))); /* power up desired lanes */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Manual Serializer reset sequence */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SER_PHY_TXCTRL_1,
				   0x3F);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SER_PHY_TXCTRL_1,
				   0x2F);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SER_PHY_TXCTRL_1,
				   0x27);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SER_PHY_TXCTRL_1,
				   0x23);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SER_PHY_TXCTRL_1,
				   0x21);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SER_PHY_TXCTRL_1,
				   0x20);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* data structure encoding: 0 = PreEmp off, 1 = on with preEmp=0 */
	if (preEmp > 0) {
		preEmp = preEmp - 1;
	}

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SER_PREEMPH_CTRL,
				   ((preEmp << 4) | (preEmp)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	enPreEmphasisMask = ((enPreEmphasisMask & 0x09) | ((enPreEmphasisMask & 0x02) <<
			     1) | ((enPreEmphasisMask & 0x04) >> 1));
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SER_PHY_ENABLES,
				   (0x50 | (enPreEmphasisMask & 0x0F)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SER_PHY_ENABLES,
				   (0x10 | (enPreEmphasisMask & 0x0F)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set serializer amplitude */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SER_PHY_TXCTRL_0,
				   (0x03 | ((serAmplitude & 0x0F) << 3)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Allow framer data to output to serializer (clear reset) */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_JESD_FRAMER_SRST_CFG,
				   0x0F);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* correct lane swaps and PN inverts on device laminate - swap lane 1 and 2 and invert all Framer lanes */
	laneInvert = ((laneInvert & 0x09) | ((laneInvert & 0x02) << 1) | ((
				laneInvert & 0x04) >> 1));
	laneInvert = ~laneInvert;
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_JESD_FRAMER_COMMON_CFG, laneInvert);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Toggle Serializer SRESET */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_JESD_FRAMER_SRST_CFG,
				   0xFF);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Allow framer data to output to serializer (clear reset) */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_JESD_FRAMER_SRST_CFG,
				   0x0F);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_setupDeserializers(taliseDevice_t *device, taliseInit_t *init)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t cdrDiv = 0;
	uint8_t deserializerDiv = 0;
	uint8_t laneInvert = 0;
	uint8_t lanesEnabled = 0;
	uint8_t lanesPowerDown = 0x0F;
	uint8_t halfRateMode = 0;
	uint32_t laneRate_kHz = 0;
	uint8_t totalM = 0;
	uint8_t totalNp = 0;
	uint8_t totalL = 0;
	uint8_t i = 0;
	uint8_t laneRateRatio = 0;
	uint8_t desEqSetting = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setupDeserializers()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (device->devStateInfo.profilesValid & TX_PROFILE_VALID) {
		if (init->tx.deframerSel == TAL_DEFRAMER_A) {
			lanesEnabled = init->jesd204Settings.deframerA.deserializerLanesEnabled;
			totalM = init->jesd204Settings.deframerA.M;
			totalNp = init->jesd204Settings.deframerA.Np;
		} else if (init->tx.deframerSel == TAL_DEFRAMER_B) {
			lanesEnabled = init->jesd204Settings.deframerB.deserializerLanesEnabled;
			totalM = init->jesd204Settings.deframerB.M;
			totalNp = init->jesd204Settings.deframerB.Np;
		} else if (init->tx.deframerSel == TAL_DEFRAMER_A_AND_B) {
			/* verify deframer A and deframer B do not share common physical lanes */
			if ((init->jesd204Settings.deframerA.deserializerLanesEnabled &
			     init->jesd204Settings.deframerB.deserializerLanesEnabled) > 0) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_DESER_LANECONFLICT_PARM, retVal, TALACT_ERR_CHECK_PARAM);
			}

			lanesEnabled = init->jesd204Settings.deframerA.deserializerLanesEnabled |
				       init->jesd204Settings.deframerB.deserializerLanesEnabled;

			if (init->jesd204Settings.deframerA.M != init->jesd204Settings.deframerB.M) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_DESER_M_CONFLICT, retVal, TALACT_ERR_CHECK_PARAM);
			}

			if (init->jesd204Settings.deframerA.M > 2) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_DESER_INV_DEFAB_M, retVal, TALACT_ERR_CHECK_PARAM);
			}

			if (init->jesd204Settings.deframerA.Np != init->jesd204Settings.deframerB.Np) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_DESER_NP_CONFLICT, retVal, TALACT_ERR_CHECK_PARAM);
			}

			/* M * 2 since lane rate needs to be calculated with total M for both framers */
			totalM = init->jesd204Settings.deframerA.M * 2;
			totalNp = init->jesd204Settings.deframerA.Np;
		} else {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_DESER_INV_DEFRAMERSEL, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if (lanesEnabled > 0x0F) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_DESER_INV_LANEEN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		lanesPowerDown = ((~lanesEnabled) & 0x0F);
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DESER_TXPROFILE_INV, retVal, TALACT_ERR_CHECK_PARAM);
	}

	for (i = 0; i < 4; i++) {
		totalL += ((lanesEnabled >> i) & 0x01);
	}

	if ((totalL == 0) ||
	    (totalL == 3) ||
	    (totalL > 4)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DESER_INV_L_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((totalM != 2) &&
	    (totalM != 4)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DESER_INV_M_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((totalNp != 12) &&
	    (totalNp != 16)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DESER_INV_NP_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	laneRate_kHz = (init->tx.txProfile.txInputRate_kHz * totalM * 10 * totalNp) /
		       (8 * totalL);

	if ((laneRate_kHz < 614400) ||
	    (laneRate_kHz > 12288000)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DESER_INV_LANERATE_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (init->clocks.clkPllVcoFreq_kHz % laneRate_kHz != 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DESES_HS_AND_LANE_RATE_NOT_INTEGER_MULT, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if ((totalNp == 12) &&
	    (init->clocks.clkPllVcoFreq_kHz != laneRate_kHz)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DESES_INV_LANE_RATE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	laneRateRatio = (uint8_t)(init->clocks.clkPllVcoFreq_kHz / laneRate_kHz);
	switch (laneRateRatio) {
	case 1:
		halfRateMode = 1;
		deserializerDiv = 0; /* divide by 1 */
		break;
	case 2:
		halfRateMode = 0; /* Full rate mode */
		deserializerDiv = 0; /* divide by 1 */
		break;
	case 4:
		halfRateMode = 0; /* Full rate mode */
		deserializerDiv = 1; /* divide by 2 */
		break;
	case 8:
		halfRateMode = 0; /* Full rate mode */
		deserializerDiv = 2; /* divide by 2 */
		break;
	case 16:
		halfRateMode = 0; /* Full rate mode */
		deserializerDiv = 3; /* divide by 2 */
		break;
	default: {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DESES_INV_LANE_RATE_DIV, retVal, TALACT_ERR_CHECK_PARAM);
	}
	}

	if ((init->jesd204Settings.desEqSetting != 0) &&
	    (init->jesd204Settings.desEqSetting != 1) &&
	    (init->jesd204Settings.desEqSetting != 2)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DESER_INV_EQ_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (init->jesd204Settings.desInvertLanePolarity > 15) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DESER_INV_LANEPN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Set lane PN inverts - invert all lanes by default due to PN inverts in Talise laminate */
	laneInvert = (~(init->jesd204Settings.desInvertLanePolarity) & 0x0F);

	/* Power up deserializers */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_DES_PHY_PWR_DWN_0,
				   0x00);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_DES_PHY_PWR_DWN_4,
				   0x20);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set deserializer half rate/full rate mode and deserializer divider */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_DES_PHY_CLK_CTL_0,
				    halfRateMode, 0x40, 6);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_DES_PHY_CLK_CTL_1,
				   deserializerDiv);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set CDR divider - set lane clock frequency (keep lane clock in 3.072G to 6.144G range) */
	cdrDiv = 0;  /* always div 1 on CDR divider */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_3,
				    cdrDiv, 0x0C, 2);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set lane PN inverts - invert all lanes by default due to PN inverts in Talise laminate */
	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_JESD_DEFRAMER_COMMON_CFG3, laneInvert, 0xF0, 4);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set CDR clock enable */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_DES_PHY_GENERAL_CTL_0, 0x90);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Release lane power downs */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_DES_PHY_PWR_DWN_4,
				    lanesPowerDown, 0x0F, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Release deserializer master power down */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_DES_PHY_PWR_DWN_4,
				    0, 0x20, 5);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set EQ */
	desEqSetting = (init->jesd204Settings.desEqSetting << 6) |
		       (init->jesd204Settings.desEqSetting << 4) |
		       (init->jesd204Settings.desEqSetting << 2) | init->jesd204Settings.desEqSetting;
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_DES_PHY_EQ_CONTROL2,
				   desEqSetting);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Enable deserializers */
	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_DES_PHY_GENERAL_CTL_1, 1, 0x10, 4);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_setupJesd204bFramer(taliseDevice_t *device, taliseInit_t *init,
				    taliseFramerSel_t framerSel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	taliseJesd204bFramerConfig_t *framer = NULL;
	uint8_t i = 0;
	uint8_t L = 0;
	uint8_t CF = 0;
	uint8_t CS = 0;
	uint8_t S = 1;
	uint8_t JESDVER = 1;
	uint8_t JESDSUBCLASS = 1;
	uint32_t framerAdcXbar = 0;
	uint16_t FK = 0;
	uint8_t HD = 0;
	uint8_t framerLaneXbar = 0xE4;
	uint32_t outputRate_kHz = 0;
	uint8_t pclkDiv = 0;
	uint32_t hsDigClkDiv4or5_Hz = 0;
	uint32_t pclk_Hz = 0;
	uint16_t framerAddrOffset = 0;
	uint8_t scr = 0;

	const uint8_t DUALBANDENABLED = 1;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG,
				 TAL_ERR_OK, "TALISE_setupJesd204bFramer()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (framerSel == TAL_FRAMER_A) {
		framer = &init->jesd204Settings.framerA;
		framerAddrOffset = 0;
	} else if (framerSel == TAL_FRAMER_B) {
		framer = &init->jesd204Settings.framerB;
		framerAddrOffset = TALISE_JESD_FRAMERB_OFFSET;
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMER_INV_FRAMERSEL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (framer->M == 0) {
		/* Disable Framer link and exit Framer setup */
		halError = talSpiWriteByte(device->devHalInfo,
					   (TALISE_ADDR_JESD_FRAMER_CFG_0 + framerAddrOffset), 0x02);
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
	} else if ((framer->M == 1)
		   && !(init->rx.rxProfile.rxDdcMode == TAL_RXDDC_INT2_REALIF)
		   && !(init->rx.rxProfile.rxDdcMode == TAL_RXDDC_DEC2_REALIF)
		   && !(init->rx.rxProfile.rxDdcMode == TAL_RXDDC_FILTERONLY_REALIF)
		   && !(init->rx.rxProfile.rxDdcMode == TAL_RXDDC_BYPASS_REALIF)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SER_INV_ZIF_TO_RIF_DATA_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	else if ((framer->M == 8) &&
		 !(device->devStateInfo.rxDualBandEnabled == DUALBANDENABLED)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMER_INV_DUALBAND_DATA_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((framer->M != 1) &&
	    (framer->M != 2) &&
	    (framer->M != 4) &&
	    (framer->M != 8)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMER_INV_M_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((framer->Np != 12) &&
	    (framer->Np != 16) &&
	    (framer->Np != 24)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMER_INV_NP_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (framer->bankId > 15) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMER_INV_BANKID_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* no need to check deviceId, its range is full uint8_t range */
	if (framer->lane0Id > 31) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMER_INV_LANEID_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (framer->syncbInSelect > 1) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMER_INV_SYNCBIN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* count number of lanes */
	L = 0;
	for (i = 0; i < 4; i++) {
		L += ((framer->serializerLanesEnabled >> i) & 0x01);
	}

	if (L == 0) {
		/* Disable Link, and return from function */
		halError = talSpiWriteByte(device->devHalInfo,
					   (TALISE_ADDR_JESD_FRAMER_CFG_0 + framerAddrOffset), 0x02);
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
	} else if ((L != 1) &&
		   (L != 2) &&
		   (L != 4)) {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_RXFRAMER_INV_L_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check for fractional S value  - invalid settings */
	if (((framer->F * L * 8) % (framer->Np * framer->M)) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMER_INV_S_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	S = (framer->F * L * 8) / (framer->Np * framer->M);
	if ((S != 1) &&
	    (S != 2) &&
	    (S != 4) &&
	    (S != 8)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMER_INV_S_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	FK = ((uint16_t)(framer->K) * (uint16_t)(framer->F));

	if ((FK < 20) ||
	    (FK > 1024) ||
	    (FK % 4 != 0)) {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_RXFRAMER_INV_FK_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	HD = ((((uint16_t)(framer->F) * 8) % framer->Np) > 0) ? (1) : (0);

	if (framer->enableManualLaneXbar != 0) {
		framerLaneXbar = framer->serializerLaneCrossbar;
	} else {
		switch((framer->serializerLanesEnabled & 0x0F)) {
		/* Lanes 1 and 2 are swapped on laminate, correct here */
		/* Set unused lanes to select value b11 */
		case 1:
			framerLaneXbar = 0xFC;
			break;
		case 2:
			framerLaneXbar = 0xCF;
			break;
		case 3:
			framerLaneXbar = 0xDC;
			break;
		case 4:
			framerLaneXbar = 0xF3;
			break;
		case 5:
			framerLaneXbar = 0xF4;
			break;
		case 6:
			framerLaneXbar = 0xC7;
			break;
		case 8:
			framerLaneXbar = 0x3F;
			break;
		case 9:
			framerLaneXbar = 0x7C;
			break;
		case 10:
			framerLaneXbar = 0x4F;
			break;
		case 12:
			framerLaneXbar = 0x73;
			break;
		case 15:
			framerLaneXbar = 0xD8;
			break;
		default:
			framerLaneXbar = 0xD8;
			break;
		}
	}

	if (framer->M == 8) {
		framerAdcXbar = 0x37251301;
	} else {
		framerAdcXbar = 0x00001301;
	}

	/* Determine framer PCLK frequency */
	if (device->devStateInfo.profilesValid & RX_PROFILE_VALID) {
		/* If Rx profile is using this framer, use the Rx IQ data rate */
		if ((init->rx.framerSel == TAL_FRAMER_A_AND_B) ||
		    (init->rx.framerSel == framerSel)) {
			outputRate_kHz = init->rx.rxProfile.rxOutputRate_kHz;
		}
	}

	if (device->devStateInfo.profilesValid & ORX_PROFILE_VALID) {
		/* If ORx profile is using this framer, use the ORx IQ data rate */
		if ((init->obsRx.framerSel == TAL_FRAMER_A_AND_B)
		    || (init->obsRx.framerSel == framerSel)) {
			outputRate_kHz = init->obsRx.orxProfile.orxOutputRate_kHz;
		}
	}

	/* If Rx and ORx share same framer - set PCLK to faster of two rates */
	if ((device->devStateInfo.profilesValid & RX_PROFILE_VALID) &&
	    (device->devStateInfo.profilesValid & ORX_PROFILE_VALID) &&
	    (init->rx.framerSel == init->obsRx.framerSel) &&
	    (init->rx.framerSel == framerSel)) {
		if (init->obsRx.orxProfile.orxOutputRate_kHz >
		    init->rx.rxProfile.rxOutputRate_kHz) {
			outputRate_kHz =  init->obsRx.orxProfile.orxOutputRate_kHz;
		} else {
			outputRate_kHz = init->rx.rxProfile.rxOutputRate_kHz;
		}
	}

	if (outputRate_kHz == 0) {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_RXFRAMER_INV_OUTPUT_RATE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	pclk_Hz = (outputRate_kHz * (uint32_t)(framer->F) * 1000) / ((uint32_t)(S) * 4);

	/* Determine PCLK divider */

	hsDigClkDiv4or5_Hz = device->devStateInfo.clocks.hsDigClkDiv4or5_Hz;

	if ((framer->F == 3) ||
	    (framer->F == 6)) {
		pclkDiv = 0;
	} else {
		for (pclkDiv = 0; pclkDiv < 8; pclkDiv++) {
			if ((hsDigClkDiv4or5_Hz >> pclkDiv) == pclk_Hz) {
				break; /* Use current pclk divider */
			}

			if (pclkDiv == 7) {
				return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_RXFRAMER_INV_PCLKFREQ, retVal, TALACT_ERR_CHECK_PARAM);
			}
		}
	}

	/* Disable JESD204b link enable */
	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_CFG_0 + framerAddrOffset),
				   ((framer->syncbInSelect << 5) | 0x02));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_L0_CFG0_0 + framerAddrOffset),
				   framer->deviceId); /* DID */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_L0_CFG1_0 + framerAddrOffset),
				   (framer->bankId & 0x0F));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_L0_CFG2_0 + framerAddrOffset),
				   (framer->lane0Id & 0x1F));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	scr = (framer->scramble == 0) ? 0x00 : 0x80;

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_L0_CFG3_0 + framerAddrOffset), (scr | ((L-1) & 0x1F)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_L0_CFG4_0 + framerAddrOffset), (framer->F - 1));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_L0_CFG5_0 + framerAddrOffset), (framer->K - 1));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_L0_CFG6_0 + framerAddrOffset), (framer->M - 1));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_L0_CFG7_0 + framerAddrOffset),
				   (((CS & 3) << 6) | ((framer->Np & 0x1F) - 1)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_L0_CFG8_0 + framerAddrOffset),
				   (((JESDSUBCLASS & 7) << 5) | ((framer->Np & 0x1F) - 1)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_L0_CFG9_0 + framerAddrOffset),
				   (((JESDVER & 7) << 5) | (S - 1)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_L0_CFG10_0 + framerAddrOffset),
				   ((HD << 7) | (CF & 0x1F)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_JESD_FRAMER_ILAS_MF_COUNT_0, 0x00); /* 4 multiframes in ILAS */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Sample Crossbar */
	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_SAMPLE_XBAR_01_0 + framerAddrOffset),
				   (uint8_t)(framerAdcXbar));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_SAMPLE_XBAR_23_0 + framerAddrOffset),
				   (uint8_t)(framerAdcXbar >> 8));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_SAMPLE_XBAR_45_0 + framerAddrOffset),
				   (uint8_t)(framerAdcXbar >> 16));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_SAMPLE_XBAR_67_0 + framerAddrOffset),
				   (uint8_t)(framerAdcXbar >> 24));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Framer: Set Lane Crossbar */
	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_LANE_XBAR_0 + framerAddrOffset), framerLaneXbar);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* enabling the SYSREF for relink if newSysrefOnRelink is set */
	if (framer->newSysrefOnRelink > 0) {
		halError = talSpiWriteField(device->devHalInfo,
					    (TALISE_ADDR_JESD_FRAMER_SYSREF_CFG_0 + framerAddrOffset), 0x01, 0x02, 1);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (framer->lmfcOffset < framer->K) {
		halError = talSpiWriteByte(device->devHalInfo,
					   (TALISE_ADDR_JESD_FRAMER_CFG5_0 + framerAddrOffset),
					   ((pclkDiv << 5) | (framer->lmfcOffset & 0x1F)));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	} else {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMER_INV_LMFC_OFFSET_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* If 11G mode, set async PCLK bit */
	if ((framer->F == 3) ||
	    (framer->F == 6)) {
		halError = talSpiWriteField(device->devHalInfo,
					    (TALISE_ADDR_JESD_FRAMER_CFG4_0 + framerAddrOffset), 0x01, 0x08, 3);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* Enable Framer link */
	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_CFG_0 + framerAddrOffset),
				   ((framer->syncbInSelect << 5) | 0x03));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_setupJesd204bDeframer(taliseDevice_t *device,
				      taliseInit_t *init, taliseDeframerSel_t deframerSel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t deframerAddrOffset = 0;
	taliseJesd204bDeframerConfig_t *deframer = NULL;
	uint8_t i = 0;
	uint8_t newSysrefOnRelink = 0;
	uint8_t scr = 0;
	uint8_t L = 0;
	uint8_t F = 0;
	uint8_t S = 1;
	uint32_t pclk_kHz = 0;
	uint16_t FK = 0;
	uint8_t HD = 0;
	uint32_t hsDigClkDiv4or5_Hz = 0;
	uint8_t pclkDiv = 0;
	uint8_t pclkReg = 0;
	uint8_t extSysref = 0;
	uint8_t syncBarSel = 1;
	uint8_t deframerInput = 0;
	uint8_t lane = 0;
	uint8_t deframerlaneXbar = 0;

	static const uint8_t CS = 0;
	static const uint8_t CF = 0;
	static const uint8_t SUBCLASSV = 1;
	static const uint8_t JESDV = 1;
	static const uint8_t DFRMRCMMIRQMASK = 0x80;

	if (device->devStateInfo.profilesValid & TX_PROFILE_VALID) {
		if (deframerSel == TAL_DEFRAMER_A) {
			deframerAddrOffset = 0;
			deframer = &init->jesd204Settings.deframerA;
		} else if (deframerSel == TAL_DEFRAMER_B) {
			deframerAddrOffset = TALISE_JESD_DEFRAMERB_OFFSET;
			deframer = &init->jesd204Settings.deframerB;
		} else {
			return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_DEFRAMER_INV_DEFSEL, retVal, TALACT_ERR_CHECK_PARAM);
		}
	} else {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DEFRAMER_INV_TXPROFILE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	newSysrefOnRelink = (deframer->newSysrefOnRelink > 0) ? 1 : 0;

	scr = (deframer->scramble > 0) ? 1 : 0;

	if (deframer->deserializerLanesEnabled > 15) {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DEFRAMER_INV_LANESEN, retVal, TALACT_ERR_CHECK_PARAM);
	}

	for (i = 0; i < 4; i++) {
		L += ((deframer->deserializerLanesEnabled >> i) & 0x01);
	}

	if ((L != 1) &&
	    (L != 2) &&
	    (L != 4)) {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DEFRAMER_INV_L, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((deframer->M != 2) &&
	    (deframer->M != 4)) {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DEFRAMER_INV_M, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((deframer->Np != 12) &&
	    (deframer->Np != 16)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DEFRAMER_INV_NP, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check for fractional F - invalid settings */
	if(device->devStateInfo.swTest > 0) {
		/*For a software test bypass bounds checking on F and assign a valid value*/
		F = 2;
	} else {
		if (((deframer->Np * deframer->M * S) % (8 * L)) > 0) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_DEFRAMER_INV_F, retVal, TALACT_ERR_CHECK_PARAM);
		}

		F = (deframer->Np * deframer->M * S) / (8 * L);
	}

	if ((F < 1) ||
	    (F > 32)) {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DEFRAMER_INV_F, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((deframer->K < 1) ||
	    (deframer->K > 32)) {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DEFRAMER_INV_K, retVal, TALACT_ERR_CHECK_PARAM);
	}

	FK = (uint16_t)F * (uint16_t)deframer->K;
	if (((FK % 4) != 0) ||
	    (FK < 20) ||
	    (FK > 1024)) {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DEFRAMER_INV_FK, retVal, TALACT_ERR_CHECK_PARAM);
	}

	pclk_kHz = init->tx.txProfile.txInputRate_kHz * (uint32_t)(F) / (uint32_t)(
			   S) / 4;
	HD = ((((uint16_t)(F) * 8) % deframer->Np) > 0) ? 1 : 0;

	hsDigClkDiv4or5_Hz = device->devStateInfo.clocks.hsDigClkDiv4or5_Hz;

	if ((F == 3) ||
	    (F == 6)) {
		pclkReg = 0;
	} else {
		if (((hsDigClkDiv4or5_Hz / 1000) % pclk_kHz) != 0) {
			return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_DEFRAMER_INV_PCLK, retVal, TALACT_ERR_CHECK_PARAM);
		}

		pclkDiv = (uint8_t)(hsDigClkDiv4or5_Hz / 1000 / pclk_kHz);

		switch (pclkDiv) {
		case 1:
			pclkReg = 0; /* PCLK = HsDigClk /4 */
			break;
		case 2:
			pclkReg = 1; /* PCLK = HsDigClk /8 */
			break;
		case 4:
			pclkReg = 2; /* PCLK = HsDigClk /16 */
			break;
		case 8:
			pclkReg = 3; /* PCLK = HsDigClk /32 */
			break;
		case 16:
			pclkReg = 4; /* PCLK = HsDigClk /64 */
			break;
		case 32:
			pclkReg = 5; /* PCLK = HsDigClk /128 */
			break;
		default: {
			return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_DEFRAMER_INV_PCLKDIV, retVal, TALACT_ERR_CHECK_PARAM);
		}
		}
	}

	if (deframer->bankId > 15) {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DEFRAMER_INV_BANKID, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (deframer->lane0Id > 31) {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_ERR_DEFRAMER_INV_LANEID, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (deframer->lmfcOffset >= deframer->K) {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DEFRAMER_INV_LMFC_OFFSET, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Disable deframer link */
	retVal = (talRecoveryActions_t)TALISE_enableDeframerLink(device, deframerSel,
			0);
	IF_ERR_RETURN_U32(retVal);

	/* checksum mode - clear [7] for correct mode */
	halError = talSpiWriteField(device->devHalInfo,
				    (TALISE_ADDR_JESD_DEFRAMER_CFG3_0 + deframerAddrOffset), 0, 0x80, 7);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Disable Deframer CMM IRQ Interrupt */
	halError = talSpiWriteField(device->devHalInfo,
				    (TALISE_ADDR_JESD_DEFRAMER_IP_CFG3_0 + deframerAddrOffset), DFRMRCMMIRQMASK,
				    0x80, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set LMFC Offset */
	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_DEFRAMER_CFG4_0 + deframerAddrOffset), deframer->lmfcOffset);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	extSysref = (deframer->externalSysref > 0) ? 1 : 0;
	halError = talSpiWriteField(device->devHalInfo,
				    (TALISE_ADDR_JESD_DEFRAMER_SYSREF_CFG2_0 + deframerAddrOffset), extSysref, 0x20,
				    5);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* If no external sysref, also clear SYSREF enable bit */
	if (extSysref == 0) {
		halError = talSpiWriteField(device->devHalInfo,
					    (TALISE_ADDR_JESD_DEFRAMER_SYSREF_CFG_0 + deframerAddrOffset), 0, 0x01, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	halError = talSpiWriteField(device->devHalInfo,
				    (TALISE_ADDR_JESD_DEFRAMER_SYSREF_CFG_0 + deframerAddrOffset),
				    newSysrefOnRelink, 0x02, 1);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* set PCLK for deframers */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_JESD_DEFRAMER_COMMON_CFG, (0x10 | pclkReg));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* select SYNCOUTB 0 or 1 pin */
	/* TODO: Fix this SYNCB selection - can use both and they get ANDed*/

	syncBarSel = (deframer->syncbOutSelect > 0) ? 2 : 1;

	halError = talSpiWriteField(device->devHalInfo,
				    (TALISE_ADDR_JESD_DEFRAMER_CFG_0 + deframerAddrOffset), syncBarSel, 0x60, 5);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_DEFRAMER_L0_CFG0_0 + deframerAddrOffset), deframer->deviceId);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_DEFRAMER_L0_CFG1_0 + deframerAddrOffset),
				   (deframer->bankId & 0x0F));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_DEFRAMER_L0_CFG2_0 + deframerAddrOffset),
				   (deframer->lane0Id & 0x1F));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_DEFRAMER_L0_CFG3_0 + deframerAddrOffset),
				   ((scr << 7) | ((L - 1) & 0x1F)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_DEFRAMER_L0_CFG4_0 + deframerAddrOffset), (F - 1));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_DEFRAMER_L0_CFG5_0 + deframerAddrOffset),
				   ((deframer->K - 1) & 0x1F));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_DEFRAMER_L0_CFG6_0 + deframerAddrOffset), (deframer->M - 1));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_DEFRAMER_L0_CFG7_0 + deframerAddrOffset),
				   (((CS & 0x03) << 6) | ((deframer->Np - 1) & 0x1F)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_DEFRAMER_L0_CFG8_0 + deframerAddrOffset),
				   (((SUBCLASSV & 0x7) << 5) | ((deframer->Np - 1) & 0x1F)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_DEFRAMER_L0_CFG9_0 + deframerAddrOffset),
				   (((JESDV & 0x7) << 5) | ((S - 1) & 0x1F)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_DEFRAMER_L0_CFG10_0 + deframerAddrOffset),
				   ((HD << 7) | (CF * 0x1F)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_DEFRAMER_ILAS_MF_COUNT_0 + deframerAddrOffset), 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if (deframer->enableManualLaneXbar != 0) {
		deframerlaneXbar = deframer->deserializerLaneCrossbar;
	} else {
		/* Lane crossbar  - Allow user to reorder lanes in deframer->deserializerLaneCrossbar, but this code still */
		/* maps used lanes to deframer inputs */
		deframerInput = 0;
		for (lane = 0; lane < 4; lane++) {
			if ((deframer->deserializerLanesEnabled >> lane) & 1) {
				deframerlaneXbar |= (((deframer->deserializerLaneCrossbar >>
						       (lane << 1)) & 3) << (deframerInput << 1));
				deframerInput += 1;
			}
		}
	}
	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_DEFRAMER_LANE_XBAR_0 + deframerAddrOffset), deframerlaneXbar);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Enable deframer link */
	retVal = (talRecoveryActions_t)TALISE_enableDeframerLink(device, deframerSel,
			1);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_enableFramerLink(taliseDevice_t *device,
				 taliseFramerSel_t framerSel, uint8_t enable)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t enableLink = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_enableFramerLink()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	enableLink = (enable > 0) ? 1 : 0;

	if ((framerSel != TAL_FRAMER_A) && (framerSel != TAL_FRAMER_B)
	    && (framerSel != TAL_FRAMER_A_AND_B)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_RSTFRAMER_INV_FRAMERSEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((framerSel == TAL_FRAMER_A) ||
	    (framerSel == TAL_FRAMER_A_AND_B)) {
		halError = talSpiWriteField(device->devHalInfo, (TALISE_ADDR_JESD_FRAMER_CFG_0),
					    enableLink, 0x01, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

	}

	if ((framerSel == TAL_FRAMER_B) ||
	    (framerSel == TAL_FRAMER_A_AND_B)) {
		halError = talSpiWriteField(device->devHalInfo,
					    (TALISE_ADDR_JESD_FRAMER_CFG_0 + TALISE_JESD_FRAMERB_OFFSET), enableLink, 0x01,
					    0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_enableDeframerLink(taliseDevice_t *device,
				   taliseDeframerSel_t deframerSel, uint8_t enable)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t deframerOffset = 0;
	static const uint16_t CDR_RESET_ADDR = 0x187e;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_enableDeframerLink()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Add reset for CDR to prevent lockup issue */
	halError = talSpiWriteField(device->devHalInfo, CDR_RESET_ADDR, 1, 0x01, 7);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if (deframerSel == TAL_DEFRAMER_A) {
		deframerOffset = 0;
	} else if (deframerSel == TAL_DEFRAMER_B) {
		deframerOffset = TALISE_JESD_DEFRAMERB_OFFSET;
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_RSTDEFRAMER_INV_DEFSEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (enable > 0) {
		/* Enable the deframer JESD link */
		halError = talSpiWriteField(device->devHalInfo,
					    (TALISE_ADDR_JESD_DEFRAMER_CFG_0 + deframerOffset), 1, 0x01, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	} else {
		/* clear deframer link enable bit */
		halError = talSpiWriteField(device->devHalInfo,
					    (TALISE_ADDR_JESD_DEFRAMER_CFG_0 + deframerOffset), 0, 0x01, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_enableSysrefToFramer(taliseDevice_t *device,
				     taliseFramerSel_t framerSel, uint8_t enable)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t enableSysref = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_enableSysrefToFramer()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	enableSysref = (enable > 0) ? 1 : 0;

	if ((framerSel != TAL_FRAMER_A) &&
	    (framerSel != TAL_FRAMER_B) &&
	    (framerSel != TAL_FRAMER_A_AND_B)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_ENFRAMERSYSREF_INV_FRAMERSEL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((framerSel == TAL_FRAMER_A) ||
	    (framerSel == TAL_FRAMER_A_AND_B)) {
		halError = talSpiWriteField(device->devHalInfo,
					    TALISE_ADDR_JESD_FRAMER_SYSREF_CFG_0, enableSysref, 0x01, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if ((framerSel == TAL_FRAMER_B) ||
	    (framerSel == TAL_FRAMER_A_AND_B)) {
		halError = talSpiWriteField(device->devHalInfo,
					    (TALISE_ADDR_JESD_FRAMER_SYSREF_CFG_0 + TALISE_JESD_FRAMERB_OFFSET),
					    enableSysref, 0x01, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_enableSysrefToDeframer(taliseDevice_t *device,
				       taliseDeframerSel_t deframerSel, uint8_t enable)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t enableSysref = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_enableSysrefToDeframer()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	enableSysref = (enable > 0) ? 1 : 0;

	if ((deframerSel != TAL_DEFRAMER_A) &&
	    (deframerSel != TAL_DEFRAMER_B) &&
	    (deframerSel != TAL_DEFRAMER_A_AND_B)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_ENDEFSYSREF_INV_DEFRAMERSEL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((deframerSel == TAL_DEFRAMER_A) ||
	    (deframerSel == TAL_DEFRAMER_A_AND_B)) {
		halError = talSpiWriteField(device->devHalInfo,
					    TALISE_ADDR_JESD_DEFRAMER_SYSREF_CFG_0, enableSysref, 0x01, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if ((deframerSel == TAL_DEFRAMER_B) ||
	    (deframerSel == TAL_DEFRAMER_A_AND_B)) {
		halError = talSpiWriteField(device->devHalInfo,
					    (TALISE_ADDR_JESD_DEFRAMER_SYSREF_CFG_0 + TALISE_JESD_DEFRAMERB_OFFSET),
					    enableSysref, 0x01, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_readFramerStatus(taliseDevice_t *device,
				 taliseFramerSel_t framerSel, uint8_t *framerStatus)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t sysrefStatus = 0;
	uint8_t configStatus3 = 0;
	uint8_t configStatus4 = 0;
	uint8_t syncStatus = 0;
	uint16_t sysrefStatusAddr = TALISE_ADDR_JESD_FRAMER_SYSREF_CFG2_0;
	uint16_t syncStatusAddr = TALISE_ADDR_JESD_FRAMER_CFG_0;
	uint16_t configStatus3Addr = TALISE_ADDR_JESD_FRAMER_SYSREF_CFG3_0;
	uint16_t configStatus4Addr = TALISE_ADDR_JESD_FRAMER_CFG4_0;
	static const uint8_t INVALIDCFG_STS_MIN_SUPPORTED_SIREV = 0xC0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_readFramerStatus()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (framerSel == TAL_FRAMER_A) {
		sysrefStatusAddr = TALISE_ADDR_JESD_FRAMER_SYSREF_CFG2_0;
		syncStatusAddr = TALISE_ADDR_JESD_FRAMER_CFG_0;
		configStatus3Addr = TALISE_ADDR_JESD_FRAMER_CFG3_0;
		configStatus4Addr = TALISE_ADDR_JESD_FRAMER_CFG4_0;
	} else if (framerSel == TAL_FRAMER_B) {
		sysrefStatusAddr = TALISE_ADDR_JESD_FRAMER_SYSREF_CFG2_0 +
				   TALISE_JESD_FRAMERB_OFFSET;
		syncStatusAddr = TALISE_ADDR_JESD_FRAMER_CFG_0 + TALISE_JESD_FRAMERB_OFFSET;
		configStatus3Addr = TALISE_ADDR_JESD_FRAMER_CFG3_0 + TALISE_JESD_FRAMERB_OFFSET;
		configStatus4Addr = TALISE_ADDR_JESD_FRAMER_CFG4_0 + TALISE_JESD_FRAMERB_OFFSET;
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMERSTATUS_INV_FRAMERSEL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (framerStatus == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMERSTATUS_NULL_FRAMERSTATUS_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	halError = ADIHAL_spiReadField(device->devHalInfo, sysrefStatusAddr,
				       &sysrefStatus, 0x03, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = ADIHAL_spiReadField(device->devHalInfo, syncStatusAddr, &syncStatus,
				       0x30, 4);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = ADIHAL_spiReadField(device->devHalInfo, configStatus3Addr,
				       &configStatus3, 0x01, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/*Read Invalid Framer Config status only if Silicon Revision is C0 or higher, else report 0 as invalid config*/
	if(device->devStateInfo.deviceSiRev >= INVALIDCFG_STS_MIN_SUPPORTED_SIREV) {
		halError = ADIHAL_spiReadField(device->devHalInfo, configStatus4Addr,
					       &configStatus4, 0x01, 0);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	*framerStatus = (configStatus4 << 6) | (configStatus3 << 5) |
			(syncStatus << 2) | sysrefStatus;

	return (uint32_t)retVal;
}

uint32_t TALISE_readDeframerStatus(taliseDevice_t *device,
				   taliseDeframerSel_t deframerSel, uint16_t *deframerStatus)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t deframerOffset = 0;
	uint16_t configStatus3Addr = TALISE_ADDR_JESD_DEFRAMER_CFG3_0;
	uint8_t configStatus3 = 0;
	uint8_t eventObs = 0;
	static const uint8_t INVALIDCFG_STS_MIN_SUPPORTED_SIREV = 0xC0;

#if TALISE_VERBOSE
	talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
		      "TALISE_readDeframerStatus()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (deframerSel == TAL_DEFRAMER_A) {
		deframerOffset = 0;
	} else if (deframerSel == TAL_DEFRAMER_B) {
		deframerOffset = TALISE_JESD_DEFRAMERB_OFFSET;
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DEFSTATUS_INV_DEFRAMERSEL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (deframerStatus == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DEFSTATUS_NULL_DEFRAMERSTATUS_PARAM, retVal, TALACT_ERR_CHECK_PARAM);

	}

	halError = talSpiReadByte(device->devHalInfo,
				  (TALISE_ADDR_JESD_DEFRAMER_EVENT_OBS_0 + deframerOffset), &eventObs);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if(device->devStateInfo.deviceSiRev >= INVALIDCFG_STS_MIN_SUPPORTED_SIREV) {
		halError = ADIHAL_spiReadField(device->devHalInfo,
					       (configStatus3Addr + deframerOffset), &configStatus3, 0x01, 0);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	*deframerStatus = ((configStatus3 << 8) | eventObs);

	return (uint32_t)retVal;
}

static talRecoveryActions_t talCheckDacSampleXbarSelectEnum (
	taliseDevice_t *device, taliseDacSampleXbarSelect_t lane)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	if ((lane != TAL_DEFRAMERA_OUT0) && (lane != TAL_DEFRAMERA_OUT1) &&
	    (lane != TAL_DEFRAMERA_OUT2) && (lane != TAL_DEFRAMERA_OUT3) &&
	    (lane != TAL_DEFRAMERB_OUT0) && (lane != TAL_DEFRAMERB_OUT1) &&
	    (lane != TAL_DEFRAMERB_OUT2) && (lane != TAL_DEFRAMERB_OUT3)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_INV_DAC_SAMP_XBAR_SELECT_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	return retVal;
}

uint32_t TALISE_setupDacSampleXbar(taliseDevice_t *device,
				   taliseTxChannels_t channelSel, taliseDacSampleXbar_t dacXbar)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t channelAddr = 0;
	uint8_t xbarReg = 0;
	static const uint8_t CONV_MASK = 0x07;
	static const uint8_t CONV_POS  = 0x4;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setupDacSampleXbar()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* performing Tx channel selection check */
	switch (channelSel) {
	case TAL_TX1:
		channelAddr = TALISE_ADDR_JESD_DEFRAMER_CH1_SAMPLE_XBAR;
		break;
	case TAL_TX2:
		channelAddr = TALISE_ADDR_JESD_DEFRAMER_CH2_SAMPLE_XBAR;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_DAC_SAMP_XBAR_CHANNEL_SEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check Crossbar for valid settings */
	retVal = talCheckDacSampleXbarSelectEnum (device, dacXbar.dacChanI);
	IF_ERR_RETURN_U32(retVal);
	retVal = talCheckDacSampleXbarSelectEnum (device, dacXbar.dacChanQ);
	IF_ERR_RETURN_U32(retVal);

	/* setting the DAC sample crossbar */
	xbarReg = (((uint8_t)dacXbar.dacChanI) & CONV_MASK) | ((((
				uint8_t)dacXbar.dacChanQ) & CONV_MASK) << CONV_POS);
	halError = talSpiWriteByte(device->devHalInfo, channelAddr, xbarReg);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

static talRecoveryActions_t talCheckAdcSampleXbarSelectEnum (
	taliseDevice_t *device, taliseAdcSampleXbarSelect_t conv)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	if ((conv != TAL_ADC_RX1_I) &&
	    (conv != TAL_ADC_RX1_Q) &&
	    (conv != TAL_ADC_RX2_I) &&
	    (conv != TAL_ADC_RX2_Q) &&
	    (conv != TAL_ADC_DUALBAND_RX1_BAND_A_I) &&
	    (conv != TAL_ADC_DUALBAND_RX1_BAND_A_Q) &&
	    (conv != TAL_ADC_DUALBAND_RX2_BAND_A_I) &&
	    (conv != TAL_ADC_DUALBAND_RX2_BAND_A_Q) &&
	    (conv != TAL_ADC_DUALBAND_RX1_BAND_B_I) &&
	    (conv != TAL_ADC_DUALBAND_RX1_BAND_B_Q) &&
	    (conv != TAL_ADC_DUALBAND_RX2_BAND_B_I) &&
	    (conv != TAL_ADC_DUALBAND_RX2_BAND_B_Q)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_INV_ADC_SAMP_XBAR_SELECT_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	return retVal;
}

uint32_t TALISE_setupAdcSampleXbar(taliseDevice_t *device,
				   taliseFramerSel_t framerSel, taliseAdcSampleXbar_t adcXbar)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t framerOffset = 0;
	uint8_t xbarReg = 0;
	static const uint8_t CONV_MASK = 0x07;
	static const uint8_t CONV_POS  = 0x3;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setupAdcSampleXbar()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	switch (framerSel) {
	case TAL_FRAMER_A:
		framerOffset = 0;
		break;
	case TAL_FRAMER_B:
		framerOffset = TALISE_JESD_FRAMERB_OFFSET;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_ADC_SAMP_XBAR_FRAMER_SEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check Crossbar for valid settings */
	retVal = talCheckAdcSampleXbarSelectEnum (device, adcXbar.conv0);
	IF_ERR_RETURN_U32(retVal);
	retVal = talCheckAdcSampleXbarSelectEnum (device, adcXbar.conv1);
	IF_ERR_RETURN_U32(retVal);
	retVal = talCheckAdcSampleXbarSelectEnum (device, adcXbar.conv2);
	IF_ERR_RETURN_U32(retVal);
	retVal = talCheckAdcSampleXbarSelectEnum (device, adcXbar.conv3);
	IF_ERR_RETURN_U32(retVal);
	retVal = talCheckAdcSampleXbarSelectEnum (device, adcXbar.conv4);
	IF_ERR_RETURN_U32(retVal);
	retVal = talCheckAdcSampleXbarSelectEnum (device, adcXbar.conv5);
	IF_ERR_RETURN_U32(retVal);
	retVal = talCheckAdcSampleXbarSelectEnum (device, adcXbar.conv6);
	IF_ERR_RETURN_U32(retVal);
	retVal = talCheckAdcSampleXbarSelectEnum (device, adcXbar.conv7);
	IF_ERR_RETURN_U32(retVal);

	/* writing setting to the ADC sample crossbar */
	xbarReg = (((uint8_t)adcXbar.conv0) & CONV_MASK) | ((((uint8_t)adcXbar.conv1) &
			CONV_MASK) << CONV_POS);
	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_SAMPLE_XBAR_01_0 + framerOffset), xbarReg);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	xbarReg = (((uint8_t)adcXbar.conv2) & CONV_MASK) | ((((uint8_t)adcXbar.conv3) &
			CONV_MASK) << CONV_POS);
	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_SAMPLE_XBAR_23_0 + framerOffset), xbarReg);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	xbarReg = (((uint8_t)adcXbar.conv4) & CONV_MASK) | ((((uint8_t)adcXbar.conv5) &
			CONV_MASK) << CONV_POS);
	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_SAMPLE_XBAR_45_0 + framerOffset), xbarReg);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	xbarReg = (((uint8_t)adcXbar.conv6) & CONV_MASK) | ((((uint8_t)adcXbar.conv7) &
			CONV_MASK) << CONV_POS);
	halError = talSpiWriteByte(device->devHalInfo,
				   (TALISE_ADDR_JESD_FRAMER_SAMPLE_XBAR_67_0 + framerOffset), xbarReg);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_enableFramerTestData(taliseDevice_t *device,
				     taliseFramerSel_t framerSelect, taliseFramerDataSource_t testDataSource,
				     taliseFramerInjectPoint_t injectPoint)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_enableFramerTestData()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (testDataSource > TAL_FTD_RAMP) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMER_INV_TESTDATA_SOURCE_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (injectPoint > TAL_FTD_POST_LANEMAP) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMER_INV_INJECTPOINT_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((framerSelect != TAL_FRAMER_A) &&
	    (framerSelect != TAL_FRAMER_B) &&
	    (framerSelect != TAL_FRAMER_A_AND_B)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMER_INV_FRAMERSEL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((framerSelect == TAL_FRAMER_A) ||
	    (framerSelect == TAL_FRAMER_A_AND_B)) {
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_JESD_FRAMER_TEST_CFG_0,
					   (uint8_t)((injectPoint << 4) | (testDataSource)));
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if ((framerSelect == TAL_FRAMER_B) ||
	    (framerSelect == TAL_FRAMER_A_AND_B)) {
		halError = talSpiWriteByte(device->devHalInfo,
					   (TALISE_ADDR_JESD_FRAMER_TEST_CFG_0 + TALISE_JESD_FRAMERB_OFFSET),
					   (uint8_t)((injectPoint << 4) | (testDataSource)));
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_injectFramerTestDataError(taliseDevice_t *device,
		taliseFramerSel_t framerSelect)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t testDataInvertAddr = TALISE_ADDR_JESD_FRAMER_TEST_CFG_0;
	uint8_t readData = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_injectFramerTestDataError()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (framerSelect == TAL_FRAMER_A) {
		testDataInvertAddr = TALISE_ADDR_JESD_FRAMER_TEST_CFG_0;
	} else if (framerSelect == TAL_FRAMER_B) {
		testDataInvertAddr = (TALISE_ADDR_JESD_FRAMER_TEST_CFG_0 +
				      TALISE_JESD_FRAMERB_OFFSET);
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMER_ERRINJECT_INV_FRAMERSEL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* reading current PRBS control register contents */
	halError = talSpiReadByte(device->devHalInfo, testDataInvertAddr, &readData);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* invert test data, then invert back to normal */
	halError = talSpiWriteByte(device->devHalInfo, testDataInvertAddr,
				   (readData |= 0x40));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, testDataInvertAddr,
				   (readData &= ~0x40));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_enableDeframerPrbsChecker(taliseDevice_t *device,
		taliseDeframerPrbsOrder_t polyOrder, taliseDefPrbsCheckLoc_t checkerLocation)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;


#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_enableDeframerPrbsChecker()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	if ((polyOrder >= TAL_PRBS_DISABLE) &&
	    (polyOrder <= TAL_PRBS31) &&
	    (checkerLocation >= TAL_PRBSCHECK_LANEDATA) &&
	    (checkerLocation <= TAL_PRBSCHECK_SAMPLEDATA)) {

		retVal = (talRecoveryActions_t)TALISE_clearDeframerPrbsCounters(device);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_JESD_DEFRAMER_PRBS_CFG,
					   (uint8_t)((polyOrder << 2) | checkerLocation));
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_EN_DEFRAMER_PRBS_INV_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* If higher priority retVal has no error, allow possible lower priority warning to be returned */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_clearDeframerPrbsCounters(taliseDevice_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_clearDeframerPrbsCounters()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_JESD_DEFRAMER_PRBS_ERROR_FLAG, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_readDeframerPrbsCounters(taliseDevice_t *device, uint8_t lane,
		uint8_t *prbsErrorCount, uint8_t *prbsInvertedStatus)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t prbsCheckLocation = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_readDeframerPrbsCounters()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (prbsErrorCount == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_READDFRMPRBS_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (prbsInvertedStatus == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_READDFRMPRBS_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Read back deframer output 0 sample counter */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_JESD_DEFRAMER_PRBS_CFG, &prbsCheckLocation);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Save Lane Data Inverted Status */
	*prbsInvertedStatus =  ((prbsCheckLocation & 0xF0) >> 4);

	/* Mask off the source bit */
	prbsCheckLocation = (prbsCheckLocation & 0x01);

	if (((taliseDefPrbsCheckLoc_t) prbsCheckLocation) == TAL_PRBSCHECK_SAMPLEDATA) {
		/* Write to update deframer output 0 sample counter */
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_JESD_DEFRAMER_SAMPLE_PRBS_ERROR_COUNT, 0);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* Read back deframer output 0 sample counter */
		halError = talSpiReadByte(device->devHalInfo,
					  TALISE_ADDR_JESD_DEFRAMER_SAMPLE_PRBS_ERROR_COUNT, prbsErrorCount);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	} else {
		/* Write PRBS error Count register to force prbs error count update for all 4 lanes */
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_JESD_DEFRAMER_PRBS_ERROR_COUNT0, 0);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		if (lane == 0) {
			halError = talSpiReadByte(device->devHalInfo,
						  TALISE_ADDR_JESD_DEFRAMER_PRBS_ERROR_COUNT0, prbsErrorCount);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		} else if (lane == 1) {
			halError = talSpiReadByte(device->devHalInfo,
						  (TALISE_ADDR_JESD_DEFRAMER_PRBS_ERROR_COUNT1), prbsErrorCount);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		} else if (lane == 2) {
			halError = talSpiReadByte(device->devHalInfo,
						  (TALISE_ADDR_JESD_DEFRAMER_PRBS_ERROR_COUNT2), prbsErrorCount);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		} else if (lane == 3) {
			halError = talSpiReadByte(device->devHalInfo,
						  (TALISE_ADDR_JESD_DEFRAMER_PRBS_ERROR_COUNT3), prbsErrorCount);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		} else {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_READDFRMPRBS_INV_DEFSEL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
		}
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getDfrmIlasMismatch(taliseDevice_t *device,
				    taliseDeframerSel_t deframerSelect, uint32_t *mismatch,
				    taliseJesd204bLane0Config_t *dfrmCfg, taliseJesd204bLane0Config_t *dfrmIlas)
{
	static const uint8_t LINK_ENABLE_MASK = 0x01;
	static const uint8_t JESD_SYNC_MASK   = 0x10;

	talRecoveryActions_t retVal =
		TALACT_NO_ACTION;     /* Local api error container                                             */
	adiHalErr_t halError =
		ADIHAL_OK;       /* Local arm error container                                             */
	uint16_t i =
		0;                         /* Local loop counter                                                    */
	uint8_t ilasdata[15] = {0};             /* Local holds lane0 ilas configuration data                             */
	uint8_t cfgdata[15] = {0};              /* Local holds lane0 configuration data                                  */
	uint16_t zeroCheck =
		0x0000;            /* Local used for or'ing to determine if lane configuration is all zeros */
	uint16_t ilasAddress =
		0;               /* Local used to assemble ilas addresses on the fly                      */
	uint16_t cfgAddress =
		0;                /* Local used to assemble cfg addresses on the fly                       */
	uint16_t syncbAddress =
		0;              /* Local used to assemble syncB register addresses on the fly            */
	uint8_t syncRegister =
		0;               /* Local holds contents of syncB register                                */
	uint16_t cfgAddrArray[15] = {0};        /* Local holds cfg addresses for spiReadBytes to fetch Cfg values        */
	uint16_t ilasAddrArray[15] = {0};       /* Local holds ilas addresses for spiReadBytes to fetch ILAS values      */
	taliseJesd204bLane0Config_t dfrmCfgLocal = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; /* Local deframer configuration settings                               */
	taliseJesd204bLane0Config_t dfrmIlasLocal = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; /* Local deframer Ilas settings                                       */

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getDfrmIlasMismatch()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (mismatch == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_JESD204B_ILAS_MISMATCH_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	*mismatch = 0;

	/* select the deframer to interrogate */
	if (deframerSelect == TAL_DEFRAMER_A) {
		ilasAddress = TALISE_ADDR_JESD_DEFRAMER_IP_OBS0_0;
		cfgAddress = TALISE_ADDR_JESD_DEFRAMER_L0_CFG0_0;
		syncbAddress = TALISE_ADDR_JESD_DEFRAMER_CFG_0;
	} else if (deframerSelect == TAL_DEFRAMER_B) {
		ilasAddress = TALISE_ADDR_JESD_DEFRAMER_IP_OBS0_1;
		cfgAddress = TALISE_ADDR_JESD_DEFRAMER_L0_CFG0_1;
		syncbAddress = TALISE_ADDR_JESD_DEFRAMER_CFG_1;
	} else {
		retVal = talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					  TAL_ERR_JESD204B_ILAS_MISMATCH_INVALID_DEFRAMER, retVal,
					  TALACT_ERR_CHECK_PARAM);
		IF_ERR_RETURN_U32(retVal);
	}

	/* check the deframer for link enable and syncB active */
	halError = talSpiReadByte(device->devHalInfo, syncbAddress, &syncRegister);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if ((syncRegister & LINK_ENABLE_MASK) == 0) {
		retVal = talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
					  TAL_ERR_JESD204B_ILAS_MISMATCH_NO_ACTIVE_LINK, retVal,
					  TALACT_ERR_CHECK_PARAM);
		IF_ERR_RETURN_U32(retVal);
	}

	if ((syncRegister & JESD_SYNC_MASK) == 0) {
		retVal = talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
					  TAL_ERR_JESD204B_ILAS_MISMATCH_SYNC_NOT_DETECTED, retVal,
					  TALACT_ERR_CHECK_PARAM);
		IF_ERR_RETURN_U32(retVal);
	}

	/* if parameter dfrmCfg is null allocation buffer */
	if (dfrmCfg == NULL) {
		dfrmCfg = &dfrmCfgLocal;
	}

	/* if parameter dfrmIlas is null allocation buffer */
	if (dfrmIlas == NULL) {
		dfrmIlas = &dfrmIlasLocal;
	}

	/* load the cfg and ilas address arrays */
	for (i = 0; i < 11; i++) {
		cfgAddrArray[i] = (cfgAddress + i);
		ilasAddrArray[i] = (ilasAddress + i);
	}

	cfgAddrArray[11] = (deframerSelect == TAL_DEFRAMER_A) ?
			   TALISE_ADDR_JESD_DEFRAMER_IP_OBS25_0 : TALISE_ADDR_JESD_DEFRAMER_IP_OBS25_1;
	ilasAddrArray[11] = (deframerSelect == TAL_DEFRAMER_A) ?
			    TALISE_ADDR_JESD_DEFRAMER_IP_OBS13_0 : TALISE_ADDR_JESD_DEFRAMER_IP_OBS13_1;

	cfgAddrArray[12] = (deframerSelect == TAL_DEFRAMER_A) ?
			   TALISE_ADDR_JESD_DEFRAMER_IP_OBS38_0 : TALISE_ADDR_JESD_DEFRAMER_IP_OBS38_1;
	ilasAddrArray[12] = (deframerSelect == TAL_DEFRAMER_A) ?
			    TALISE_ADDR_JESD_DEFRAMER_IP_OBS35_0 : TALISE_ADDR_JESD_DEFRAMER_IP_OBS35_1;

	cfgAddrArray[13] = (deframerSelect == TAL_DEFRAMER_A) ?
			   TALISE_ADDR_JESD_DEFRAMER_IP_OBS39_0 : TALISE_ADDR_JESD_DEFRAMER_IP_OBS39_1;
	ilasAddrArray[13] = (deframerSelect == TAL_DEFRAMER_A) ?
			    TALISE_ADDR_JESD_DEFRAMER_IP_OBS36_0 : TALISE_ADDR_JESD_DEFRAMER_IP_OBS36_1;

	cfgAddrArray[14] = (deframerSelect == TAL_DEFRAMER_A) ?
			   TALISE_ADDR_JESD_DEFRAMER_IP_OBS40_0 : TALISE_ADDR_JESD_DEFRAMER_IP_OBS40_1;
	ilasAddrArray[14] = (deframerSelect == TAL_DEFRAMER_A) ?
			    TALISE_ADDR_JESD_DEFRAMER_IP_OBS37_0 : TALISE_ADDR_JESD_DEFRAMER_IP_OBS37_1;

	for (i = 0; i < 15; i++) {
		/* fetch the ilas data items */
		halError = talSpiReadByte(device->devHalInfo, cfgAddrArray[i], &cfgdata[i]);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* fetch the cfg data items */
		halError = talSpiReadByte(device->devHalInfo, ilasAddrArray[i], &ilasdata[i]);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* do the zero check of ilas data items */
	zeroCheck = 0x0000;
	for (i = 0; i < 15; i++) {
		zeroCheck = zeroCheck | ilasdata[i];
	}

	/* If ILAS configuration is all zeros then lane0 is not running */
	if (zeroCheck == 0x0000) {
		retVal = talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
					  TAL_ERR_JESD204B_ILAS_MISMATCH_NO_ACTIVE_LINK, retVal,
					  TALACT_ERR_CHECK_PARAM);
		IF_ERR_RETURN_U32(retVal);
	}

	/* loading the structures with the read values for easier reading when doing compares */
	dfrmIlas->DID = ilasdata[0];                 /* jesd_deframer_IP_obs0  */
	dfrmIlas->BID = (ilasdata[1] & 0x0F);        /* jesd_deframer_IP_obs1  */
	dfrmIlas->LID0 = (ilasdata[2] & 0x1F);       /* jesd_deframer_IP_obs2  */
	dfrmIlas->L = (ilasdata[3] & 0x1F);          /* jesd_deframer_IP_obs3  */
	dfrmIlas->SCR = ((ilasdata[3] & 0x80) >> 7); /* jesd_deframer_IP_obs3  */
	dfrmIlas->F = ilasdata[4];                   /* jesd_deframer_IP_obs4  */
	dfrmIlas->K = (ilasdata[5] & 0x1F);          /* jesd_deframer_IP_obs5  */
	dfrmIlas->M = ilasdata[6];                   /* jesd_deframer_IP_obs6  */
	dfrmIlas->N = (ilasdata[7] & 0x1F);          /* jesd_deframer_IP_obs7  */
	dfrmIlas->CS = ((ilasdata[7] & 0xC0) >> 6);  /* jesd_deframer_IP_obs7  */
	dfrmIlas->NP = (ilasdata[8] & 0x1F);         /* jesd_deframer_IP_obs8  */
	dfrmIlas->S = (ilasdata[9] & 0x1F);          /* jesd_deframer_IP_obs9  */
	dfrmIlas->CF = (ilasdata[10] & 0x1F);        /* jesd_deframer_IP_obs10 */
	dfrmIlas->HD = ((ilasdata[10] & 0x80) >> 7); /* jesd_deframer_IP_obs10 */
	dfrmIlas->FCHK0 = ilasdata[11];              /* lane0 Ilas CheckSum    */
	dfrmIlas->FCHK1 = ilasdata[12];              /* lane1 Ilas CheckSum    */
	dfrmIlas->FCHK2 = ilasdata[13];              /* lane2 Ilas CheckSum    */
	dfrmIlas->FCHK3 = ilasdata[14];              /* lane3 Ilas CheckSum    */


	dfrmCfg->DID = cfgdata[0];                  /* jesd_deframer_LO_cfg0       */
	dfrmCfg->BID = (cfgdata[1] & 0x0F);         /* jesd_deframer_LO_cfg1       */
	dfrmCfg->LID0 = (cfgdata[2] & 0x1F);        /* jesd_deframer_LO_cfg2       */
	dfrmCfg->L = (cfgdata[3] & 0x1F);           /* jesd_deframer_LO_cfg3       */
	dfrmCfg->SCR = ((cfgdata[3] & 0x80) >> 7);  /* jesd_deframer_LO_cfg3       */
	dfrmCfg->F = cfgdata[4];                    /* jesd_deframer_LO_cfg4       */
	dfrmCfg->K = (cfgdata[5] & 0x1F);           /* jesd_deframer_LO_cfg5       */
	dfrmCfg->M = cfgdata[6];                    /* jesd_deframer_LO_cfg6       */
	dfrmCfg->N = (cfgdata[7] & 0x1F);           /* jesd_deframer_LO_cfg7       */
	dfrmCfg->CS = ((cfgdata[7] & 0xC0) >> 6);   /* jesd_deframer_LO_cfg7       */
	dfrmCfg->NP = (cfgdata[8] & 0x1F);          /* jesd_deframer_LO_cfg8       */
	dfrmCfg->S = (cfgdata[9] & 0x1F);           /* jesd_deframer_LO_cfg9       */
	dfrmCfg->CF = (cfgdata[10] & 0x1F);         /* jesd_deframer_LO_cfg10      */
	dfrmCfg->HD = ((cfgdata[10] & 0x80) >> 7);  /* jesd_deframer_LO_cfg10      */
	dfrmCfg->FCHK0 = cfgdata[11];               /* lane0 Cfg Computed CheckSum */
	dfrmCfg->FCHK1 = cfgdata[12];               /* lane1 Cfg Computed CheckSum */
	dfrmCfg->FCHK2 = cfgdata[13];               /* lane2 Cfg Computed CheckSum */
	dfrmCfg->FCHK3 = cfgdata[14];               /* lane3 Cfg Computed CheckSum */


	/* performing ILAS mismatch check */
	*mismatch = 0x00000000;
	if (dfrmIlas->DID != dfrmCfg->DID) {
		*mismatch |= 0x00000001;
	}

	if (dfrmIlas->BID != dfrmCfg->BID) {
		*mismatch |= 0x00000002;
	}

	if (dfrmIlas->LID0 != dfrmCfg->LID0) {
		*mismatch |= 0x00000004;
	}

	if (dfrmIlas->L != dfrmCfg->L) {
		*mismatch |= 0x00000008;
	}

	if (dfrmIlas->SCR != dfrmCfg->SCR) {
		*mismatch |= 0x00000010;
	}

	if (dfrmIlas->F != dfrmCfg->F) {
		*mismatch |= 0x00000020;
	}

	if (dfrmIlas->K != dfrmCfg->K) {
		*mismatch |= 0x00000040;
	}

	if (dfrmIlas->M != dfrmCfg->M) {
		*mismatch |= 0x00000080;
	}

	if (dfrmIlas->N != dfrmCfg->N) {
		*mismatch |= 0x00000100;
	}

	if (dfrmIlas->CS != dfrmCfg->CS) {
		*mismatch |= 0x00000200;
	}

	if (dfrmIlas->NP != dfrmCfg->NP) {
		*mismatch |= 0x00000400;
	}

	if (dfrmIlas->S != dfrmCfg->S) {
		*mismatch |= 0x00000800;
	}

	if (dfrmIlas->CF != dfrmCfg->CF) {
		*mismatch |= 0x00001000;
	}

	if (dfrmIlas->HD != dfrmCfg->HD) {
		*mismatch |= 0x00002000;
	}

	/* verify lane ilas checksums equal lane computed checksums */
	for (i = 11; i < 15; i++) {
		/* if ilas checksum equal zero then the lane is inactive so suppress lane checksum mismatch flag */
		if ((ilasdata[i] != cfgdata[i]) && (ilasdata[i] != 0x00)) {
			*mismatch |= (0x00004000 << (i - 11));

		}
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_setDfrmIrqMask(taliseDevice_t *device,
			       taliseDeframerSel_t deframerSelect, uint16_t irqMask)
{
	static const uint8_t ERROR_COUNTERS_ENABLE_MASK = 0x3F;
	static const uint8_t IRQ_BIT8_MASK = 0x80;

	uint8_t enCount = 0;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t dataArray[2] = {0};
	uint16_t addrArray[2] = {0};

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setDfrmIrqMask()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* param range check */
	if ((deframerSelect != TAL_DEFRAMER_A) && (deframerSelect != TAL_DEFRAMER_B)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETDFRMIRQMASK_INV_DEFRAMERSEL_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if (deframerSelect == TAL_DEFRAMER_A) {
		addrArray[0] = TALISE_ADDR_JESD_DEFRAMER_IP_CFG2_0;
		addrArray[1] = TALISE_ADDR_JESD_DEFRAMER_IP_CFG3_0;
	} else {
		addrArray[0] = TALISE_ADDR_JESD_DEFRAMER_IP_CFG2_1;
		addrArray[1] = TALISE_ADDR_JESD_DEFRAMER_IP_CFG3_1;
	}

	/* fetch the contents of the enable counters register where bit7 is IRQ[8:8] readmodifywrite operation*/
	halError = talSpiReadByte(device->devHalInfo, addrArray[1], &enCount);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	dataArray[0] = (uint8_t)irqMask;
	dataArray[1] = (enCount & ERROR_COUNTERS_ENABLE_MASK) |
		       IRQ_BIT8_MASK; /* BIT8 CMM IRQ is disabled to correct BUG */

	halError = talSpiWriteBytes(device->devHalInfo, &addrArray[0], &dataArray[0],
				    2);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return ((uint32_t)retVal);
}


uint32_t TALISE_getDfrmIrqMask(taliseDevice_t *device,
			       taliseDeframerSel_t deframerSelect, uint16_t *irqMask)
{
	static const uint8_t IRQ_BIT8_MASK = 0x80;

	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t dataArray[2] = {0};
	uint16_t addrArray[2] = {0};

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getDfrmIrqMask()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* param range check */
	if (irqMask == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETDFRMIRQMASK_NULL_IRQMASK_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/* param range check */
	if ((deframerSelect != TAL_DEFRAMER_A) &&
	    (deframerSelect != TAL_DEFRAMER_B)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETDFRMIRQMASK_INV_DEFRAMERSELECT_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if (deframerSelect == TAL_DEFRAMER_A) {
		addrArray[0] = TALISE_ADDR_JESD_DEFRAMER_IP_CFG2_0;
		addrArray[1] = TALISE_ADDR_JESD_DEFRAMER_IP_CFG3_0;
	} else {
		addrArray[0] = TALISE_ADDR_JESD_DEFRAMER_IP_CFG2_1;
		addrArray[1] = TALISE_ADDR_JESD_DEFRAMER_IP_CFG3_1;
	}

	/* fetch IRQ mask low */
	halError = talSpiReadByte(device->devHalInfo, addrArray[0], &dataArray[0]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* fetch IRQ mask high */
	halError = talSpiReadByte(device->devHalInfo, addrArray[1], &dataArray[1]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	*irqMask = ((uint16_t)(dataArray[1] & IRQ_BIT8_MASK)) << 1 | ((
				uint16_t)dataArray[0]);

	return ((uint32_t)retVal);
}

uint32_t TALISE_getDfrmIrqSource(taliseDevice_t *device,
				 taliseDeframerSel_t deframerSelect, uint16_t *irqSourceValue)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t irqVector[8] = {0};
	uint8_t dataArray[8] = {0};

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getDfrmIrqSource()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* param range check */
	if (irqSourceValue == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETDFRMIRQSRC_NULL_STATUS_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* param range check */
	if ((deframerSelect != TAL_DEFRAMER_A) &&
	    (deframerSelect != TAL_DEFRAMER_B)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETDFRMIRQSRC_INV_DEFRAMERSEL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (deframerSelect == TAL_DEFRAMER_A) {
		irqVector[0] = TALISE_ADDR_JESD_DEFRAMER_IP_OBS14_0;
		irqVector[1] = TALISE_ADDR_JESD_DEFRAMER_IP_OBS15_0;
		irqVector[2] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_DET_LAT_BUF_0;
		irqVector[3] = TALISE_ADDR_JESD_DEFRAMER_SYSREF_CFG2_0;
		irqVector[4] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_LANE0;
		irqVector[5] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_LANE1;
		irqVector[6] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_LANE2;
		irqVector[7] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_LANE3;
	} else {
		irqVector[0] = TALISE_ADDR_JESD_DEFRAMER_IP_OBS14_1;
		irqVector[1] = TALISE_ADDR_JESD_DEFRAMER_IP_OBS15_1;
		irqVector[2] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_DET_LAT_BUF_1;
		irqVector[3] = TALISE_ADDR_JESD_DEFRAMER_SYSREF_CFG2_1;
		irqVector[4] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_LANE0;
		irqVector[5] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_LANE1;
		irqVector[6] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_LANE2;
		irqVector[7] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_LANE3;
	}

	/* fetch IRQ sources */
	halError = talSpiReadBytes(device->devHalInfo, &irqVector[0], &dataArray[0], 8);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	*irqSourceValue = ((uint16_t)(dataArray[7] & 0x01) << 14) | (((uint16_t)(
				  dataArray[6] & 0x01) << 13) |
			  (((uint16_t)(dataArray[5] & 0x01) << 12) | (((uint16_t)(
					  dataArray[4] & 0x01) << 11) |
					  (((uint16_t)(dataArray[3] & 0x02) << 9) | (((uint16_t)(dataArray[2] & 0x01) <<
							  9) |
							  (((uint16_t)(dataArray[1] & 0x01) << 8) | dataArray[0]))))));

	return((uint32_t)retVal);
}

uint32_t TALISE_clearDfrmIrq(taliseDevice_t *device,
			     taliseDeframerSel_t deframerSelect)
{
	static const uint8_t IRQ_BIT8_MASK = 0x80;

	uint8_t enCount = 0;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t irqVector[8] = {0};
	uint8_t dataArray[8] = {0};
	uint16_t irqMask = 0;
	int16_t i = 0;
	uint8_t irqSource = 0;
	uint16_t irqSourceAddr = TALISE_ADDR_JESD_DEFRAMER_IP_OBS14_0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_clearDfrmIrq()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/* param range check */
	if ((deframerSelect != TAL_DEFRAMER_A) &&
	    (deframerSelect != TAL_DEFRAMER_B)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_CLRDFRMIRQ_INV_DEFRAMERSEL_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if (deframerSelect == TAL_DEFRAMER_A) {
		irqVector[0] = TALISE_ADDR_JESD_DEFRAMER_IP_CFG2_0;
		irqVector[1] = TALISE_ADDR_JESD_DEFRAMER_IP_CFG3_0;
		irqVector[2] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_DET_LAT_BUF_0;
		irqVector[3] = TALISE_ADDR_JESD_DEFRAMER_SYSREF_CFG2_0;
		irqVector[4] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_LANE0;
		irqVector[5] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_LANE1;
		irqVector[6] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_LANE2;
		irqVector[7] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_LANE3;

		irqSourceAddr = TALISE_ADDR_JESD_DEFRAMER_IP_OBS14_0;
	} else {
		irqVector[0] = TALISE_ADDR_JESD_DEFRAMER_IP_CFG2_1;
		irqVector[1] = TALISE_ADDR_JESD_DEFRAMER_IP_CFG3_1;
		irqVector[2] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_DET_LAT_BUF_1;
		irqVector[3] = TALISE_ADDR_JESD_DEFRAMER_SYSREF_CFG2_1;
		irqVector[4] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_LANE0;
		irqVector[5] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_LANE1;
		irqVector[6] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_LANE2;
		irqVector[7] = TALISE_ADDR_JESD_DEFRAMER_PTR_DBG_LANE3;

		irqSourceAddr = TALISE_ADDR_JESD_DEFRAMER_IP_OBS14_1;
	}

	retVal = (talRecoveryActions_t)TALISE_getDfrmIrqMask(device, deframerSelect,
			&irqMask);
	IF_ERR_RETURN_U32(retVal);

	/* fetch the contents of the deframer IRQ source */
	halError = talSpiReadByte(device->devHalInfo, irqSourceAddr, &irqSource);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* fetch the contents of the enable counters register where bit7 is IRQ[8:8] readmodifywrite operation*/
	halError = talSpiReadByte(device->devHalInfo, irqVector[1], &enCount);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	dataArray[0] = ((~irqSource) & 0xFF);
	dataArray[1] = (enCount | IRQ_BIT8_MASK);

	/* Clear the IRQ[7:0] and IRQ[8:8] */
	halError = talSpiWriteBytes(device->devHalInfo, &irqVector[0], &dataArray[0],
				    2);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo, irqVector[3], dataArray[3],
				    0x02, 1);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Clear the remain static IRQs */
	for (i = 4; i <= 7; i++) {
		if (dataArray[i] > 0) {
			halError = talSpiWriteByte(device->devHalInfo, irqVector[i], 0x00);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}
	}

	/* restore orignal irqmask */
	halError = talSpiWriteByte(device->devHalInfo, irqVector[0],
				   (uint8_t)(irqMask & 0x00FF));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* disable CMM IRQ */
	halError = talSpiWriteField(device->devHalInfo, irqVector[1], 1, IRQ_BIT8_MASK,
				    7);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* If higher priority retVal has no error, allow possible lower priority warning to be returned */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return ((uint32_t)retVal);
}

talRecoveryActions_t talFindDfrmrLaneCntErr(taliseDevice_t *device,
		taliseDeframerSel_t deframer, int32_t *deframerInputsMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	uint8_t msByte = 0;
	uint8_t lsByte = 0;
	uint8_t cntrResetMask = 0;
	uint16_t deframerTcrAddress0 = 0;
	uint16_t deframerTcrAddress1 = 0;
	uint16_t deframerEcntRstAddress0 = 0;
	uint16_t deframerEcntRstAddress1 = 0;

	static const uint16_t ECNT_TCH_LANE0_MASK = 0x0007;
	static const uint16_t ECNT_TCH_LANE1_MASK = 0x0038;
	static const uint16_t ECNT_TCH_LANE2_MASK = 0x0007;
	static const uint16_t ECNT_TCH_LANE3_MASK = 0x0038;

	if (deframer == TAL_DEFRAMER_A) {
		deframerEcntRstAddress0 = TALISE_ADDR_JESD_DEFRAMER_IP_CFG5_0;
		deframerEcntRstAddress1 = TALISE_ADDR_JESD_DEFRAMER_IP_CFG6_0;
		deframerTcrAddress0 = TALISE_ADDR_JESD_DEFRAMER_IP_OBS23_0;
		deframerTcrAddress1 = TALISE_ADDR_JESD_DEFRAMER_IP_OBS24_0;

	} else if (deframer == TAL_DEFRAMER_B) {
		deframerEcntRstAddress0 = TALISE_ADDR_JESD_DEFRAMER_IP_CFG5_1;
		deframerEcntRstAddress1 = TALISE_ADDR_JESD_DEFRAMER_IP_CFG6_1;
		deframerTcrAddress0 = TALISE_ADDR_JESD_DEFRAMER_IP_OBS23_1;
		deframerTcrAddress1 = TALISE_ADDR_JESD_DEFRAMER_IP_OBS24_1;
	} else {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_TALFINDDFRMRLANECNTERROR_INV_DEFRAMERSEL_PARAM, retVal,
					TALACT_ERR_CHECK_PARAM);
	}

	if (deframerInputsMask == NULL) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_TALFINDDFRMRLANECNTERROR_NULL_PARAM, retVal,
					TALACT_ERR_CHECK_PARAM);
	}

	/* Read the TCR registers */
	halError = talSpiReadByte(device->devHalInfo, deframerTcrAddress1, &msByte);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiReadByte(device->devHalInfo, deframerTcrAddress0, &lsByte);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Deframer input 0 */
	*deframerInputsMask = 0;
	cntrResetMask = (lsByte & ECNT_TCH_LANE0_MASK);
	if (cntrResetMask > 0) {
		*deframerInputsMask |= 0x01;
	}

	/* Deframer input 1 */
	cntrResetMask = (lsByte & ECNT_TCH_LANE1_MASK);
	if (cntrResetMask > 0) {
		*deframerInputsMask |= 0x02;
	}

	/* Deframer input 2 */
	cntrResetMask = (msByte & ECNT_TCH_LANE2_MASK);
	if (cntrResetMask > 0) {
		*deframerInputsMask |= 0x04;
	}

	/* Deframer input 3 */
	cntrResetMask = (msByte & ECNT_TCH_LANE3_MASK);
	if (cntrResetMask > 0) {
		*deframerInputsMask |= 0x08;
	}

	/* clear the error counter */
	halError = talSpiWriteByte(device->devHalInfo, deframerEcntRstAddress0, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* clear the error counter */
	halError = talSpiWriteByte(device->devHalInfo, deframerEcntRstAddress1, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	return (retVal);
}

talRecoveryActions_t talFindDfrmrLaneErr(taliseDevice_t *device,
		uint32_t dfrmErrAddress, uint8_t nibbleToUse, int32_t *deframerInputsMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	uint8_t dfrmErrMask =
		0; /* 4 bits in either upper /lower nibble that describe the lanes an error occurred on */

	static const uint8_t HIGH = 1;

	if (deframerInputsMask == NULL) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_TALFINDDFRMRLANEERROR_NULL_PARAM, retVal,
					TALACT_ERR_CHECK_PARAM);
	}

	/* Read error flags from requested deframer register address */
	halError = talSpiReadByte(device->devHalInfo, dfrmErrAddress, &dfrmErrMask);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	if (nibbleToUse >= HIGH) {
		/* Grab lane error mask from upper 4 bits in each register */
		*deframerInputsMask = ((dfrmErrMask >> 4) & 0x0F);
	} else {
		/* Grab lane error mask from lower 4 bits in each register */
		*deframerInputsMask = (dfrmErrMask & 0x0F);
	}

	return (retVal);
}

uint32_t TALISE_framerSyncbToggle(taliseDevice_t *device,
				  taliseFramerSel_t framerSel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t framerAddrOffset = 0;
	uint8_t framerCfg0Value = 0;

	static const uint8_t FORCE_SYNCB_HIGH = 0x0C;
	static const uint8_t FORCE_SYNCB_LOW = 0x04;
	static const uint8_t FORCE_SYNCB_CLEAR_BITMASK = 0xF3;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_framerSyncbToggle()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if ((framerSel != TAL_FRAMER_A) &&
	    (framerSel != TAL_FRAMER_B) &&
	    (framerSel != TAL_FRAMER_A_AND_B)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_FRAMERSYSREFTOGGLE_INV_FRAMERSEL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Save possible warning from logging until end of function */
	retValWarn = retVal;

	if ((framerSel == TAL_FRAMER_A) ||
	    (framerSel == TAL_FRAMER_A_AND_B)) {
		framerAddrOffset = 0;

		/* Perform Read/modify/write to prevent changing other bits in the register */
		halError = talSpiReadByte(device->devHalInfo,
					  (TALISE_ADDR_JESD_FRAMER_CFG_0 + framerAddrOffset), &framerCfg0Value);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		framerCfg0Value &= FORCE_SYNCB_CLEAR_BITMASK; /* Clear bits [3:2] */
		framerCfg0Value |=
			FORCE_SYNCB_HIGH; /* Set reg [3:2] = b11 - force SYNCB high */
		halError = talSpiWriteByte(device->devHalInfo,
					   (TALISE_ADDR_JESD_FRAMER_CFG_0 + framerAddrOffset), framerCfg0Value);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		framerCfg0Value &= FORCE_SYNCB_CLEAR_BITMASK; /* Clear bits [3:2] */
		framerCfg0Value |= FORCE_SYNCB_LOW; /* Set reg [3:2] = b01 - force SYNCB low */
		halError = talSpiWriteByte(device->devHalInfo,
					   (TALISE_ADDR_JESD_FRAMER_CFG_0 + framerAddrOffset), framerCfg0Value);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		framerCfg0Value &=
			FORCE_SYNCB_CLEAR_BITMASK; /* Clear bits [3:2], Set reg [3:2] = b00 - disable force of SYNCB */
		halError = talSpiWriteByte(device->devHalInfo,
					   (TALISE_ADDR_JESD_FRAMER_CFG_0 + framerAddrOffset), framerCfg0Value);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if ((framerSel == TAL_FRAMER_B) ||
	    (framerSel == TAL_FRAMER_A_AND_B)) {
		framerAddrOffset = TALISE_JESD_FRAMERB_OFFSET;

		/* Perform Read/modify/write to prevent changing other bits in the register */
		halError = talSpiReadByte(device->devHalInfo,
					  (TALISE_ADDR_JESD_FRAMER_CFG_0 + framerAddrOffset), &framerCfg0Value);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		framerCfg0Value &= FORCE_SYNCB_CLEAR_BITMASK; /* Clear bits [3:2] */
		framerCfg0Value |=
			FORCE_SYNCB_HIGH; /* Set reg [3:2] = b11 - force SYNCB high */
		halError = talSpiWriteByte(device->devHalInfo,
					   (TALISE_ADDR_JESD_FRAMER_CFG_0 + framerAddrOffset), framerCfg0Value);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		framerCfg0Value &= FORCE_SYNCB_CLEAR_BITMASK; /* Clear bits [3:2] */
		framerCfg0Value |= FORCE_SYNCB_LOW; /* Set reg [3:2] = b01 - force SYNCB low */
		halError = talSpiWriteByte(device->devHalInfo,
					   (TALISE_ADDR_JESD_FRAMER_CFG_0 + framerAddrOffset), framerCfg0Value);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		framerCfg0Value &=
			FORCE_SYNCB_CLEAR_BITMASK; /* Clear bits [3:2], Set reg [3:2] = b00 - disable force of SYNCB */
		halError = talSpiWriteByte(device->devHalInfo,
					   (TALISE_ADDR_JESD_FRAMER_CFG_0 + framerAddrOffset), framerCfg0Value);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* If higher priority retVal has no error, allow possible lower priority warning to be returned */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}
