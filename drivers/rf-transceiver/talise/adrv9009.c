/***************************************************************************//**
 *   @file   adrv9009.c
 *   @brief  Implementation of ADRV9009 Driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include <string.h>

#include "talise_agc.h"
#include "talise_arm.h"
#include "talise_cals.h"
#include "talise_gpio.h"
#include "talise_jesd204.h"
#include "talise_radioctrl.h"
#include "talise_tx.h"

#include "adi_hal.h"

#include "jesd204.h"

#include "no_os_alloc.h"
#include "no_os_clk.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"
#include "no_os_util.h"

#include "adrv9009.h"

struct adrv9009_jesd204_link {
	unsigned int source_id;
	bool is_framer;
};

struct adrv9009_jesd204_priv {
	struct adrv9009_rf_phy *phy;
	struct adrv9009_jesd204_link link[3];
};

static inline bool has_rx(struct adrv9009_rf_phy *phy)
{
	return phy->spi_device_id != ID_ADRV90082;
}

static inline bool has_tx(struct adrv9009_rf_phy *phy)
{
	return phy->spi_device_id != ID_ADRV90081;
}

static inline bool has_rx_and_en(struct adrv9009_rf_phy *phy)
{
	return has_rx(phy) && (phy->talInit->rx.rxChannels != TAL_RXOFF) &&
		(phy->jesd_rx_clk || phy->jdev);
}

static inline bool has_tx_and_en(struct adrv9009_rf_phy *phy)
{
	return has_tx(phy) && (phy->talInit->tx.txChannels != TAL_TXOFF) &&
		(phy->jesd_tx_clk || phy->jdev);
}

static void adrv9009_info(struct adrv9009_rf_phy *phy)
{
	taliseArmVersionInfo_t talArmVersionInfo;
	uint32_t api_vers[4];
	uint8_t rev;

	TALISE_getArmVersion_v2(phy->talDevice, &talArmVersionInfo);
	TALISE_getApiVersion(phy->talDevice, &api_vers[0], &api_vers[1], &api_vers[2],
			&api_vers[3]);
	TALISE_getDeviceRev(phy->talDevice, &rev);

	pr_info(
		"%s: Rev %d, Firmware %u.%u.%u API version: %u.%u.%u.%u successfully initialized%s\n",
		__func__, rev, talArmVersionInfo.majorVer,
		talArmVersionInfo.minorVer, talArmVersionInfo.rcVer,
		api_vers[0], api_vers[1], api_vers[2], api_vers[3],
		phy->jdev ? " via jesd204-fsm" : "");
}

int adrv9009_jesd204_link_pre_setup(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason)
{
	struct adrv9009_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9009_rf_phy *phy = priv->phy;
	uint64_t dev_clk;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		jesd204_state_op_reason_str(reason));

	if (reason == JESD204_STATE_OP_REASON_UNINIT) {
		TALISE_shutdown(phy->talDevice);
		TALISE_closeHw(phy->talDevice);
		phy->is_initialized = 0;

		memset(&phy->talise_device.devStateInfo, 0,
			sizeof(phy->talise_device.devStateInfo));

		return JESD204_STATE_CHANGE_DONE;
	}

	no_os_clk_round_rate(phy->dev_clk,
			phy->talInit->clocks.deviceClock_kHz * 1000,
				     &dev_clk);

	if (dev_clk > 0 && ((dev_clk / 1000) ==
		phy->talInit->clocks.deviceClock_kHz)) {
		no_os_clk_set_rate(phy->dev_clk, dev_clk);
	} else {
		pr_err(
			"Requesting device clock %u failed got %ld\n",
			phy->talInit->clocks.deviceClock_kHz * 1000, dev_clk);
		return -EINVAL;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9009_jesd204_link_init(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct adrv9009_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9009_rf_phy *phy = priv->phy;
	taliseJesd204bFramerConfig_t *framer = NULL;
	taliseJesd204bDeframerConfig_t *deframer = NULL;
	bool orx_adc_stitching_enabled;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		lnk->link_id, jesd204_state_op_reason_str(reason));

	switch (reason) {
	case JESD204_STATE_OP_REASON_INIT:
		break;
	default:
		return JESD204_STATE_CHANGE_DONE;
	}

	switch (lnk->link_id) {
	case DEFRAMER_LINK_TX:
		deframer = &phy->talInit->jesd204Settings.deframerA;
		lnk->sample_rate = phy->talInit->tx.txProfile.txInputRate_kHz * 1000;
		priv->link[DEFRAMER_LINK_TX].source_id = TAL_DEFRAMER_A;
		break;
	case FRAMER_LINK_RX:
		framer = &phy->talInit->jesd204Settings.framerA;
		lnk->sample_rate = phy->talInit->rx.rxProfile.rxOutputRate_kHz * 1000;
		priv->link[FRAMER_LINK_RX].source_id = TAL_FRAMER_A;
		priv->link[FRAMER_LINK_RX].is_framer = true;
		break;
	case FRAMER_LINK_ORX:
		orx_adc_stitching_enabled =
			(phy->talInit->obsRx.orxProfile.rfBandwidth_Hz > 200000000) ?
			1 : 0;

		if (orx_adc_stitching_enabled) {
			if (phy->talInit->obsRx.framerSel != TAL_FRAMER_B) {
				pr_warning("%s:%d: Can't apply fixup",
					__func__, __LINE__);
			} else {
				phy->talInit->jesd204Settings.framerB.M = 2;
				phy->talInit->jesd204Settings.framerB.F = 2;
				if (phy->talInit->obsRx.obsRxChannelsEnable == TAL_ORX1ORX2)
					phy->talInit->obsRx.obsRxChannelsEnable = TAL_ORX1;
			}
		} else {
			phy->talInit->jesd204Settings.framerB.M = phy->framer_b_m;
			phy->talInit->jesd204Settings.framerB.F = phy->framer_b_f;
			phy->talInit->obsRx.obsRxChannelsEnable = phy->orx_channel_enabled;
		}

		framer = &phy->talInit->jesd204Settings.framerB;
		lnk->sample_rate = phy->talInit->obsRx.orxProfile.orxOutputRate_kHz * 1000;
		priv->link[FRAMER_LINK_ORX].source_id = TAL_FRAMER_B;
		priv->link[FRAMER_LINK_ORX].is_framer = true;
		break;
	default:
		return -EINVAL;
	}

	if (framer) {
		lnk->num_converters = framer->M;
		lnk->num_lanes = no_os_hweight8(framer->serializerLanesEnabled);
		lnk->octets_per_frame = framer->F;
		lnk->frames_per_multiframe = framer->K;
		lnk->device_id = framer->deviceId;
		lnk->bank_id = framer->bankId;
		lnk->scrambling = framer->scramble;
		lnk->bits_per_sample = framer->Np;
		lnk->converter_resolution = 16;
		lnk->ctrl_bits_per_sample = 0;
		lnk->jesd_version = JESD204_VERSION_B;
		lnk->subclass = framer->externalSysref ?
			JESD204_SUBCLASS_1 : JESD204_SUBCLASS_0;
		lnk->is_transmit = false;
	} else if (deframer) {
		lnk->num_converters = deframer->M;
		lnk->num_lanes = no_os_hweight8(deframer->deserializerLanesEnabled);
		lnk->octets_per_frame = (deframer->Np * lnk->num_converters) /
			(8 * lnk->num_lanes);
		lnk->frames_per_multiframe = deframer->K;
		lnk->device_id = deframer->deviceId;
		lnk->bank_id = deframer->bankId;
		lnk->scrambling = deframer->scramble;
		lnk->bits_per_sample = deframer->Np;
		lnk->converter_resolution = 16;
		lnk->ctrl_bits_per_sample = 0;
		lnk->jesd_version = JESD204_VERSION_B;
		lnk->subclass = deframer->externalSysref ?
			JESD204_SUBCLASS_1 : JESD204_SUBCLASS_0;
		lnk->is_transmit = true;
	};

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9009_jesd204_clks_enable(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct adrv9009_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9009_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		lnk->link_id, jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	if (!lnk->num_converters)
		return JESD204_STATE_CHANGE_DONE;

	if (priv->link[lnk->link_id].is_framer) {
		ret = TALISE_enableSysrefToFramer(phy->talDevice,
			priv->link[lnk->link_id].source_id, 0);
		if (ret != TALACT_NO_ACTION) {
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			ret = -EFAULT;
		}

		ret = TALISE_enableFramerLink(phy->talDevice,
			priv->link[lnk->link_id].source_id, 0);
		if (ret != TALACT_NO_ACTION) {
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			ret = -EFAULT;
		}

		ret = TALISE_enableFramerLink(phy->talDevice,
			priv->link[lnk->link_id].source_id, 1);
		if (ret != TALACT_NO_ACTION) {
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}

		pr_debug(
			"%s:%d Link %d Framer enabled", __func__, __LINE__,
			priv->link[lnk->link_id].source_id);

		/*************************************************/
		/**** Enable SYSREF to Talise JESD204B Framer ***/
		/*************************************************/
		/*** < User: Make sure SYSREF is stopped/disabled > ***/
		ret = TALISE_enableSysrefToFramer(phy->talDevice,
			priv->link[lnk->link_id].source_id, 1);
		if (ret != TALACT_NO_ACTION) {
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}
	} else {
		ret = TALISE_enableSysrefToDeframer(phy->talDevice,
			priv->link[lnk->link_id].source_id, 0);
		if (ret != TALACT_NO_ACTION) {
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			ret = -EFAULT;
		}

		ret = TALISE_enableDeframerLink(phy->talDevice,
			priv->link[lnk->link_id].source_id, 0);
		if (ret != TALACT_NO_ACTION) {
			pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			ret = -EFAULT;
		}
	};

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9009_jesd204_link_enable(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct adrv9009_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9009_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		lnk->link_id, jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	if (!lnk->num_converters)
		return JESD204_STATE_CHANGE_DONE;

	if (!priv->link[lnk->link_id].is_framer) {
		ret = TALISE_enableDeframerLink(phy->talDevice,
			priv->link[lnk->link_id].source_id, 1);
		if (ret != TALACT_NO_ACTION) {
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}
		/***************************************************/
		/**** Enable SYSREF to Talise JESD204B Deframer ***/
		/***************************************************/
		ret = TALISE_enableSysrefToDeframer(phy->talDevice,
			priv->link[lnk->link_id].source_id, 1);
		if (ret != TALACT_NO_ACTION) {
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}
	};

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9009_jesd204_link_running(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct adrv9009_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9009_rf_phy *phy = priv->phy;
	int ret;
	uint16_t deframerStatus = 0;
	uint8_t framerStatus = 0;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		lnk->link_id, jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	if (!lnk->num_converters)
		return JESD204_STATE_CHANGE_DONE;

	if (priv->link[lnk->link_id].is_framer) {
		ret = TALISE_readFramerStatus(phy->talDevice,
			priv->link[lnk->link_id].source_id, &framerStatus);
		if (ret != TALACT_NO_ACTION) {
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			ret = -EFAULT;
		}

		if ((framerStatus & 0x07) != 0x05)
			pr_warning(
				"Link%u TAL_FRAMER_A framerStatus 0x%X\n",
				lnk->link_id, framerStatus);
	} else {
		ret = TALISE_readDeframerStatus(phy->talDevice,
			priv->link[lnk->link_id].source_id, &deframerStatus);
		if (ret != TALACT_NO_ACTION) {
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			ret = -EFAULT;
		}

		if ((deframerStatus & 0xF7) != 0x86)
			pr_warning(
				"Link%u TAL_DEFRAMER_A deframerStatus 0x%X\n",
				lnk->link_id, deframerStatus);
	};

	return JESD204_STATE_CHANGE_DONE;
}

int adrv9009_jesd204_link_setup(struct jesd204_dev *jdev,
				enum jesd204_state_op_reason reason)
{
	struct adrv9009_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9009_rf_phy *phy = priv->phy;
	uint8_t mcsStatus = 0;
	uint8_t pllLockStatus = 0;
	int ret = TALACT_NO_ACTION;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	/*** < Insert User BBIC JESD204B Initialization Code Here > ***/

	/*******************************/
	/**** Talise Initialization ***/
	/*******************************/

	/*Open Talise Hw Device*/
	ret = TALISE_openHw(phy->talDevice);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}
	/* Toggle RESETB pin on Talise device */
	ret = TALISE_resetDevice(phy->talDevice);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	/* TALISE_initialize() loads the Talise device data structure
	 * settings for the Rx/Tx/ORx profiles, FIR filters, digital
	 * filter enables, calibrates the CLKPLL, loads the user provided Rx
	 * gain tables, and configures the JESD204b serializers/framers/deserializers
	 * and deframers.
	 */
	ret = TALISE_initialize(phy->talDevice, phy->talInit);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	/*******************************/
	/***** CLKPLL Status Check *****/
	/*******************************/

	ret = TALISE_getPllsLockStatus(phy->talDevice, &pllLockStatus);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	/* Assert that Talise CLKPLL is locked */
	if ((pllLockStatus & 0x01) == 0) {
		pr_err( "%s:%d: CLKPLL is unlocked (0x%X)\n",
			__func__, __LINE__, pllLockStatus);
		return -EFAULT;
	}

	/*******************************************************/
	/**** Perform MultiChip Sync (MCS) on Talise Device ***/
	/*******************************************************/
	ret = TALISE_enableMultichipSync(phy->talDevice, 1, &mcsStatus);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9009_jesd204_setup_stage1(struct jesd204_dev *jdev,
					 enum jesd204_state_op_reason reason)
{
	struct adrv9009_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9009_rf_phy *phy = priv->phy;
	int ret;
	uint8_t mcsStatus;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	ret = TALISE_enableMultichipSync(phy->talDevice, 0, &mcsStatus);
	if (ret != TALACT_NO_ACTION)
		return -EFAULT;

	if ((mcsStatus & 0x08) != 0x08) {
		pr_err(
			"%s:%d Unexpected MCS sync status (0x%X)\n",
			__func__, __LINE__, mcsStatus);
		return -EFAULT;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9009_jesd204_setup_stage2(struct jesd204_dev *jdev,
					 enum jesd204_state_op_reason reason)
{
	struct adrv9009_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9009_rf_phy *phy = priv->phy;
	uint8_t mcsStatus = 0;
	uint8_t pllLockStatus_mask, pllLockStatus = 0;
	int ret;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	/*******************/
	/**** Verify MCS ***/
	/*******************/
	ret = TALISE_enableMultichipSync(phy->talDevice, 0, &mcsStatus);
	if ((mcsStatus & 0x0B) != 0x0B) {
		pr_err(
			"%s:%d Unexpected MCS sync status (0x%X)\n",
			__func__, __LINE__, mcsStatus);
		return -EFAULT;
	}

	/*******************************************************/
	/**** Prepare Talise Arm binary and Load Arm and    ****/
	/**** Stream processor Binaryes                     ****/
	/*******************************************************/

	ret = TALISE_initArm(phy->talDevice, phy->talInit);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	ret = TALISE_loadStreamFromBinary(phy->talDevice,
		phy->stream);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	ret = TALISE_loadArmFromBinary(phy->talDevice, phy->fw,
				       phy->fw_size);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	/* TALISE_verifyArmChecksum() will timeout after 200ms
	 * if ARM checksum is not computed
	 */
	ret = TALISE_verifyArmChecksum(phy->talDevice);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	ret = TALISE_setArmGpioPins(phy->talDevice, &phy->arm_gpio_config);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	/*******************************/
	/**Set RF PLL LO Frequencies ***/
	/*******************************/
	phy->current_loopBandwidth_kHz[0] = 50;

	ret = TALISE_setRfPllLoopFilter(phy->talDevice,
		phy->current_loopBandwidth_kHz[0],
		phy->loopFilter_stability);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	ret = TALISE_setRfPllFrequency(phy->talDevice, TAL_RF_PLL,
				       phy->trx_lo_frequency);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	if (phy->talInit->tx.txChannels == TAL_TXOFF)
		pllLockStatus_mask = 0x3;
	else
		pllLockStatus_mask = 0x7;

	ret = TALISE_getPllsLockStatus(phy->talDevice, &pllLockStatus);
	if ((pllLockStatus & pllLockStatus_mask) != pllLockStatus_mask) {
		no_os_mdelay(200);
		ret = TALISE_getPllsLockStatus(phy->talDevice, &pllLockStatus);
		if ((pllLockStatus & pllLockStatus_mask) != pllLockStatus_mask) {
			pr_err( "%s:%d RF PLL unlocked (0x%x)\n",
				__func__, __LINE__, pllLockStatus);
			return -EFAULT;
		}
	}

	ret = TALISE_enableMultichipRfLOPhaseSync(phy->talDevice, 1);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9009_jesd204_setup_stage3(struct jesd204_dev *jdev,
					 enum jesd204_state_op_reason reason)
{
	struct adrv9009_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9009_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	ret = TALISE_enableMultichipRfLOPhaseSync(phy->talDevice, 0);

	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9009_jesd204_setup_stage4(struct jesd204_dev *jdev,
					 enum jesd204_state_op_reason reason)

{
	struct adrv9009_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9009_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	/* Parallelization stage ... */

	/****************************************************/
	/**** Run Talise ARM Initialization Calibrations ***/
	/****************************************************/
	/*** < User: Turn ON the PA (if any), and open any switches on ORx input used to isolate it for calibrations > ***/
	/*** < User: Open any switches on the Rx input (if used) to isolate Rx input and provide required VSWR at input > ***/
	ret = TALISE_runInitCals(phy->talDevice,
		phy->initCalMask & ~TAL_TX_LO_LEAKAGE_EXTERNAL);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	//schedule();

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9009_jesd204_setup_stage5(struct jesd204_dev *jdev,
					 enum jesd204_state_op_reason reason)
{
	struct adrv9009_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9009_rf_phy *phy = priv->phy;
	int ret;
	uint8_t errorFlag;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	ret = TALISE_waitInitCals(phy->talDevice, 20000, &errorFlag);

	if (ret == TALACT_ERR_RERUN_INIT_CALS) {
		/* Try once more */
		ret = TALISE_runInitCals(phy->talDevice,
			phy->initCalMask & ~TAL_TX_LO_LEAKAGE_EXTERNAL);
		if (ret != TALACT_NO_ACTION) {
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}

		ret = TALISE_waitInitCals(phy->talDevice, 20000, &errorFlag);
	}

	if ((ret != TALACT_NO_ACTION) || errorFlag) {
		uint32_t calsSincePowerUp = 0, calsLastRun = 0, calsMinimum = 0;
		uint8_t initErrCal = 0, initErrCode = 0;

		pr_err(
			"%s:%d (ret %d): Init Cal errorFlag (0x%X)\n",
			__func__, __LINE__, ret, errorFlag);

		ret = TALISE_getInitCalStatus(phy->talDevice, &calsSincePowerUp,
			&calsLastRun, &calsMinimum, &initErrCal, &initErrCode);

		pr_err(
			"%s:%d (ret %d): Init Cal calsSincePowerUp (0x%X) calsLastRun (0x%X) calsMinimum (0x%X) initErrCal (0x%X) initErrCode (0x%X)\n",
			__func__, __LINE__, ret, calsSincePowerUp, calsLastRun,
			calsMinimum, initErrCal, initErrCode);

		return -EFAULT;
	}

	/*************************************************************************/
	/*****  TALISE ARM Initialization External LOL Calibrations with PA  *****/
	/*************************************************************************/
	/*** < Action: Please ensure PA is enabled operational at this time > ***/
	if (phy->initCalMask & TAL_TX_LO_LEAKAGE_EXTERNAL) {
		ret = TALISE_runInitCals(phy->talDevice,
			TAL_TX_LO_LEAKAGE_EXTERNAL);
		if (ret != TALACT_NO_ACTION)
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);

		ret = TALISE_waitInitCals(phy->talDevice, 20000, &errorFlag);
		if ((ret != TALACT_NO_ACTION) || errorFlag) {
			pr_err(
				"%s:%d (ret %d): Init Cal errorFlag (0x%X)\n",
				__func__, __LINE__, ret, errorFlag);
		}
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9009_jesd204_post_running_stage(struct jesd204_dev *jdev,
	enum jesd204_state_op_reason reason)
{
	struct adrv9009_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9009_rf_phy *phy = priv->phy;
	int ret;

	uint32_t trackingCalMask = phy->tracking_cal_mask =  TAL_TRACK_NONE;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT) {
		phy->is_initialized = 0;
		return JESD204_STATE_CHANGE_DONE;
	}

	/***********************************************
	 * Allow Rx1/2 QEC tracking and Tx1/2 QEC       *
	 * tracking to run when in the radioOn state    *
	 * Tx calibrations will only run if radioOn and *
	 * the obsRx path is set to OBS_INTERNAL_CALS   *
	 * **********************************************/
	ret = TALISE_setGpIntMask(phy->talDevice, TAL_GP_MASK_AUX_SYNTH_UNLOCK);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}
	ret = TALISE_enableTrackingCals(phy->talDevice, trackingCalMask);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}
	if (has_rx_and_en(phy)) {

		ret = TALISE_setupRxAgc(phy->talDevice, phy->rxAgcCtrl);
		if (ret != TALACT_NO_ACTION) {
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}
#if 0 // FIXME
		ret = adrv9009_gt_fw_load(phy);
		if (ret < 0) {
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}
#endif
	}
	if (has_tx_and_en(phy)) {
		ret = TALISE_setTxAttenCtrlPin(phy->talDevice,
			TAL_TX1, &phy->tx1_atten_ctrl_pin);
		if (ret != TALACT_NO_ACTION) {
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}
		ret = TALISE_setTxAttenCtrlPin(phy->talDevice,
			TAL_TX2, &phy->tx2_atten_ctrl_pin);
		if (ret != TALACT_NO_ACTION) {
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}
	}
	/* Function to turn radio on, Enables transmitters and receivers */
	/* that were setup during TALISE_initialize() */
	ret = TALISE_radioOn(phy->talDevice);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}
	ret = TALISE_setRxTxEnable(phy->talDevice,
		has_rx_and_en(phy) ? phy->talInit->rx.rxChannels : 0,
		has_tx_and_en(phy) ? phy->talInit->tx.txChannels : 0);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}
	if (has_rx(phy))
		//clk_set_rate(phy->clks[RX_SAMPL_CLK],
		no_os_clk_set_rate(phy->clks[RX_SAMPL_CLK],
			phy->talInit->rx.rxProfile.rxOutputRate_kHz * 1000);
	if (has_tx(phy)) {
		//clk_set_rate(phy->clks[OBS_SAMPL_CLK],
		no_os_clk_set_rate(phy->clks[OBS_SAMPL_CLK],
			phy->talInit->obsRx.orxProfile.orxOutputRate_kHz * 1000);
		//clk_set_rate(phy->clks[TX_SAMPL_CLK],
		no_os_clk_set_rate(phy->clks[TX_SAMPL_CLK],
			phy->talInit->tx.txProfile.txInputRate_kHz * 1000);
	}

	ret = TALISE_setupAuxDacs(phy->talDevice, &phy->auxdac);
	if (ret != TALACT_NO_ACTION) {
		pr_err(
			"%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	if (phy->gpio3v3SrcCtrl) {
		ret = TALISE_setGpio3v3SourceCtrl(phy->talDevice,
			phy->gpio3v3SrcCtrl);
		if (ret != TALACT_NO_ACTION) {
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}
		TALISE_setGpio3v3PinLevel(phy->talDevice, phy->gpio3v3PinLevel);
		TALISE_setGpio3v3Oe(phy->talDevice, phy->gpio3v3OutEn, 0xFFF);
	}
	if (has_tx(phy)) {
		ret = TALISE_setPaProtectionCfg(phy->talDevice,
			&phy->tx_pa_protection);
		if (ret != TALACT_NO_ACTION) {
			pr_err(
				"%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}
	}

	phy->is_initialized = 1;
	//enable_irq(phy->spi->irq);
	adrv9009_info(phy);

	return JESD204_STATE_CHANGE_DONE;
}

static const struct jesd204_dev_data jesd204_adrv9009_init = {
	.state_ops = {
		[JESD204_OP_LINK_PRE_SETUP] = {
			.per_device = adrv9009_jesd204_link_pre_setup,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_LINK_INIT] = {
			.per_link = adrv9009_jesd204_link_init,
		},
		[JESD204_OP_CLOCKS_ENABLE] = {
			.per_link = adrv9009_jesd204_clks_enable,
		},
		[JESD204_OP_LINK_SETUP] = {
			.per_device = adrv9009_jesd204_link_setup,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_LINK_ENABLE] = {
			.per_link = adrv9009_jesd204_link_enable,
			.post_state_sysref = true,
		},
		[JESD204_OP_LINK_RUNNING] = {
			.per_link = adrv9009_jesd204_link_running,
		},
		[JESD204_OP_OPT_SETUP_STAGE1] = {
			.per_device = adrv9009_jesd204_setup_stage1,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_OPT_SETUP_STAGE2] = {
			.per_device = adrv9009_jesd204_setup_stage2,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_OPT_SETUP_STAGE3] = {
			.per_device = adrv9009_jesd204_setup_stage3,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_OPT_SETUP_STAGE4] = {
			.per_device = adrv9009_jesd204_setup_stage4,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_OPT_SETUP_STAGE5] = {
			.per_device = adrv9009_jesd204_setup_stage5,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_OPT_POST_RUNNING_STAGE] = {
			.per_device = adrv9009_jesd204_post_running_stage,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
	},
	.max_num_links = 3,
	.sizeof_priv = sizeof(struct adrv9009_jesd204_priv),
};


int adrv9009_parse_init_param(struct adrv9009_rf_phy *phy,
				const struct adrv9009_init_param *init_param)
{
	phy->spi_device_id = init_param->spi_device_id;

	phy->stream = init_param->stream;
	phy->fw = init_param->fw;
	phy->fw_size = init_param->fw_size;

	phy->talInit = init_param->talInit;

	phy->rxAgcCtrl = init_param->rxAgcCtrl;

	phy->arm_gpio_config.orx1TxSel0Pin.gpioPinSel =
		init_param->arm_gpio_config_orx1_tx_sel0_pin_gpio_pin_sel;
	phy->arm_gpio_config.orx1TxSel0Pin.polarity =
		init_param->arm_gpio_config_orx1_tx_sel0_pin_polarity;
	phy->arm_gpio_config.orx1TxSel0Pin.enable =
		init_param->arm_gpio_config_orx1_tx_sel0_pin_enable;

	phy->arm_gpio_config.orx1TxSel1Pin.gpioPinSel =
		init_param->arm_gpio_config_orx1_tx_sel1_pin_gpio_pin_sel;
	phy->arm_gpio_config.orx1TxSel1Pin.polarity =
		init_param->arm_gpio_config_orx1_tx_sel1_pin_polarity;
	phy->arm_gpio_config.orx1TxSel1Pin.enable =
		init_param->arm_gpio_config_orx1_tx_sel1_pin_enable;
	phy->arm_gpio_config.orx2TxSel0Pin.gpioPinSel =
		init_param->arm_gpio_config_orx2_tx_sel0_pin_gpio_pin_sel;
	phy->arm_gpio_config.orx2TxSel0Pin.polarity =
		init_param->arm_gpio_config_orx2_tx_sel0_pin_polarity;
	phy->arm_gpio_config.orx2TxSel0Pin.enable =
		init_param->arm_gpio_config_orx2_tx_sel0_pin_enable;

	phy->arm_gpio_config.orx2TxSel1Pin.gpioPinSel =
		init_param->arm_gpio_config_orx2_tx_sel1_pin_gpio_pin_sel;
	phy->arm_gpio_config.orx2TxSel1Pin.polarity =
		init_param->arm_gpio_config_orx2_tx_sel1_pin_polarity;
	phy->arm_gpio_config.orx2TxSel1Pin.enable =
		init_param->arm_gpio_config_orx2_tx_sel1_pin_enable;
	phy->arm_gpio_config.enTxTrackingCals.gpioPinSel =
		init_param->arm_gpio_config_en_tx_tracking_cals_gpio_pin_sel;
	phy->arm_gpio_config.enTxTrackingCals.polarity =
		init_param->arm_gpio_config_en_tx_tracking_cals_polarity;
	phy->arm_gpio_config.enTxTrackingCals.enable =
		init_param->arm_gpio_config_en_tx_tracking_cals_enable;

	phy->tx1_atten_ctrl_pin.stepSize =
		init_param->tx1_atten_ctrl_pin_step_size;
	phy->tx1_atten_ctrl_pin.txAttenIncPin =
		init_param->tx1_atten_ctrl_pin_tx_atten_inc_pin;
	phy->tx1_atten_ctrl_pin.txAttenDecPin =
		init_param->tx1_atten_ctrl_pin_tx_atten_dec_pin;
	phy->tx1_atten_ctrl_pin.enable =
		init_param->tx1_atten_ctrl_pin_enable;

	phy->tx2_atten_ctrl_pin.stepSize =
		init_param->tx2_atten_ctrl_pin_step_size;
	phy->tx2_atten_ctrl_pin.txAttenIncPin =
		init_param->tx2_atten_ctrl_pin_tx_atten_inc_pin;
	phy->tx2_atten_ctrl_pin.txAttenDecPin =
		init_param->tx2_atten_ctrl_pin_tx_atten_dec_pin;
	phy->tx2_atten_ctrl_pin.enable =
		init_param->tx2_atten_ctrl_pin_enable;

	phy->tx_pa_protection.avgDuration =
		init_param->tx_pa_protection_avg_duration;
	phy->tx_pa_protection.txAttenStep =
		init_param->tx_pa_protection_tx_atten_step;
	phy->tx_pa_protection.tx1PowerThreshold =
		init_param->tx_pa_protection_tx1_power_threshold;
	phy->tx_pa_protection.tx2PowerThreshold =
		init_param->tx_pa_protection_tx2_power_threshold;
	phy->tx_pa_protection.peakCount =
		init_param->tx_pa_protection_peak_count;
	phy->tx_pa_protection.tx1PeakThreshold =
		init_param->tx_pa_protection_tx1_peak_threshold;
	phy->tx_pa_protection.tx2PeakThreshold =
		init_param->tx_pa_protection_tx2_peak_threshold;

	phy->auxdac.auxDacEnables = init_param->aux_dac_enables;
	phy->auxdac.auxDacVref[0] = init_param->aux_dac_vref0;
	phy->auxdac.auxDacResolution[0] = init_param->aux_dac_resolution0;
	phy->auxdac.auxDacValues[0] = init_param->aux_dac_values0;
	phy->auxdac.auxDacVref[1] = init_param->aux_dac_vref1;
	phy->auxdac.auxDacResolution[1] = init_param->aux_dac_resolution1;
	phy->auxdac.auxDacValues[1] = init_param->aux_dac_values1;
	phy->auxdac.auxDacVref[2] = init_param->aux_dac_vref2;
	phy->auxdac.auxDacResolution[2] = init_param->aux_dac_resolution2;
	phy->auxdac.auxDacValues[2] = init_param->aux_dac_values2;
	phy->auxdac.auxDacVref[3] = init_param->aux_dac_vref3;
	phy->auxdac.auxDacResolution[3] = init_param->aux_dac_resolution3;
	phy->auxdac.auxDacValues[3] = init_param->aux_dac_values3;
	phy->auxdac.auxDacVref[4] = init_param->aux_dac_vref4;
	phy->auxdac.auxDacResolution[4] = init_param->aux_dac_resolution4;
	phy->auxdac.auxDacValues[4] = init_param->aux_dac_values4;
	phy->auxdac.auxDacVref[5] = init_param->aux_dac_vref5;
	phy->auxdac.auxDacResolution[5] = init_param->aux_dac_resolution5;
	phy->auxdac.auxDacValues[5] = init_param->aux_dac_values5;
	phy->auxdac.auxDacVref[6] = init_param->aux_dac_vref6;
	phy->auxdac.auxDacResolution[6] = init_param->aux_dac_resolution6;
	phy->auxdac.auxDacValues[6] = init_param->aux_dac_values6;
	phy->auxdac.auxDacVref[7] = init_param->aux_dac_vref7;
	phy->auxdac.auxDacResolution[7] = init_param->aux_dac_resolution7;
	phy->auxdac.auxDacValues[7] = init_param->aux_dac_values7;
	phy->auxdac.auxDacVref[8] = init_param->aux_dac_vref8;
	phy->auxdac.auxDacResolution[8] = init_param->aux_dac_resolution8;
	phy->auxdac.auxDacValues[8] = init_param->aux_dac_values8;
	phy->auxdac.auxDacVref[9] = init_param->aux_dac_vref9;
	phy->auxdac.auxDacResolution[9] = init_param->aux_dac_resolution9;
	phy->auxdac.auxDacValues[9] = init_param->aux_dac_values9;
	phy->auxdac.auxDacValues[10] = init_param->aux_dac_values10;
	phy->auxdac.auxDacValues[11] = init_param->aux_dac_values11;

	phy->gpio3v3SrcCtrl = init_param->gpio3v3_source_control;
	phy->gpio3v3PinLevel = init_param->gpio3v3_output_level_mask;
	phy->gpio3v3OutEn = init_param->gpio3v3_output_enable_mask;

	phy->dev_clk = init_param->dev_clk;

	phy->trx_lo_frequency = init_param->trx_pll_lo_frequency_hz;

	phy->loopFilter_stability = 3;

	return 0;
}

int adrv9009_init(struct adrv9009_rf_phy **dev,
		const struct adrv9009_init_param *init_param)
{
	struct adrv9009_jesd204_priv *priv;
	struct adrv9009_rf_phy *phy;
	int ret;

	phy = (struct adrv9009_rf_phy *)no_os_calloc(1, sizeof(*phy));
	if (!phy)
		return -ENOMEM;

	ret = no_os_gpio_get(&phy->no_os_hal.gpio_adrv_resetb,
			init_param->gpio_adrv_resetb_init);
	if (ret < 0)
			goto error_1;

	ret = no_os_gpio_get_optional(&phy->no_os_hal.gpio_adrv_sysref_req,
			init_param->gpio_adrv_sysref_req_init);
	if (ret < 0)
			goto error_2;

	ret = no_os_spi_init(&phy->no_os_hal.spi_adrv_desc,
			init_param->spi_init);
	if (ret < 0)
			goto error_3;

	adrv9009_parse_init_param(phy, init_param);

	phy->no_os_hal.log_level = ADIHAL_LOG_ERR | ADIHAL_LOG_WARN;

	phy->talDevice = &phy->talise_device;
	phy->talDevice->devHalInfo = &phy->no_os_hal;

	phy->framer_b_m = phy->talInit->jesd204Settings.framerB.M;
	phy->framer_b_f = phy->talInit->jesd204Settings.framerB.F;
	phy->orx_channel_enabled = phy->talInit->obsRx.obsRxChannelsEnable;

	switch (phy->spi_device_id) {
	case ID_ADRV9009:
	case ID_ADRV9009_X2:
	case ID_ADRV9009_X4:
		phy->initCalMask = TAL_TX_BB_FILTER | TAL_ADC_TUNER |  TAL_TIA_3DB_CORNER |
			TAL_DC_OFFSET | TAL_RX_GAIN_DELAY | TAL_FLASH_CAL |
			TAL_PATH_DELAY | TAL_TX_LO_LEAKAGE_INTERNAL |
			TAL_TX_QEC_INIT | TAL_LOOPBACK_RX_LO_DELAY |
			TAL_LOOPBACK_RX_RX_QEC_INIT | TAL_RX_QEC_INIT |
			TAL_ORX_QEC_INIT | TAL_TX_DAC  | TAL_ADC_STITCHING |
			TAL_RX_PHASE_CORRECTION;
		break;
	case ID_ADRV90081:
		phy->initCalMask = TAL_ADC_TUNER | TAL_TIA_3DB_CORNER | TAL_DC_OFFSET |
			TAL_RX_GAIN_DELAY | TAL_FLASH_CAL | TAL_RX_QEC_INIT |
			TAL_RX_PHASE_CORRECTION;
		phy->talInit->jesd204Settings.deframerA.M = 0;
		phy->talInit->jesd204Settings.deframerB.M = 0;
		phy->talInit->tx.txChannels = TAL_TXOFF;
		phy->talInit->obsRx.obsRxChannelsEnable = TAL_ORXOFF;
		break;
	case ID_ADRV90082:
		phy->initCalMask = TAL_TX_BB_FILTER | TAL_ADC_TUNER | TAL_TIA_3DB_CORNER |
			TAL_DC_OFFSET | TAL_FLASH_CAL | TAL_PATH_DELAY |
			TAL_TX_LO_LEAKAGE_INTERNAL | TAL_TX_QEC_INIT |
			TAL_LOOPBACK_RX_LO_DELAY | TAL_LOOPBACK_RX_RX_QEC_INIT |
			TAL_ORX_QEC_INIT | TAL_TX_DAC  | TAL_ADC_STITCHING;
		phy->talInit->jesd204Settings.framerA.M = 0;
		phy->talInit->rx.rxChannels = TAL_RXOFF;
		break;
	default:
		ret = -EINVAL;
		goto error_4;
	}

	ret = jesd204_dev_register(&phy->jdev, &jesd204_adrv9009_init);
	if (ret < 0)
		goto error_1;
	priv = jesd204_dev_priv(phy->jdev);
	priv->phy = phy;

	*dev = phy;

	return 0;

error_4:
	no_os_spi_remove(phy->no_os_hal.spi_adrv_desc);
error_3:
	no_os_gpio_remove(phy->no_os_hal.gpio_adrv_sysref_req);
error_2:
	no_os_gpio_remove(phy->no_os_hal.gpio_adrv_resetb);
error_1:
	no_os_free(phy);

	return ret;
}

int adrv9009_remove(struct adrv9009_rf_phy *dev)
{
	no_os_gpio_remove(dev->no_os_hal.gpio_adrv_resetb);
	no_os_gpio_remove(dev->no_os_hal.gpio_adrv_sysref_req);
	no_os_spi_remove(dev->no_os_hal.spi_adrv_desc);

	no_os_free(dev);

	return 0;
}
