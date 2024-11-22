/***************************************************************************//**
 *   @file   adrv9009.c
 *   @brief  ADRV9009 initialization and control routines.
 *   @author George Mois (george.mois@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
// stdlibs
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// platform drivers
#include "no_os_print_log.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_util.h"

// talise
#include "talise_reg_addr_macros.h"
#include "talise_stream_binary.h"
#include "talise_arm_binary.h"
#include "talise_radioctrl.h"
#include "talise_jesd204.h"
#include "talise_config.h"
#include "talise_error.h"
#include "talise_types.h"
#include "talise_cals.h"
#include "talise_gpio.h"
#include "talise_arm.h"
#include "talise_agc.h"
#include "talise_tx.h"
#include "talise.h"

// jesd
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"

// hal
#include "app_config.h"
#include "parameters.h"
#include "adi_hal.h"

// header
#include "adrv9009.h"

static int __adrv9009_of_get_param(void *out_value, void *in_value,
				   uint32_t defval, uint32_t size)
{
	switch (size) {
	case 1:
		if (*(uint8_t *)in_value) {
			*(uint8_t *)out_value = *(uint8_t *)in_value;
		} else {
			*(uint8_t *)out_value = defval;
		}
		break;
	case 2:
		if (*(uint16_t *)in_value) {
			*(uint16_t *)out_value = *(uint16_t *)in_value;
		} else {
			*(uint16_t *)out_value = defval;
		}
		break;
	case 4:
		if (*(uint32_t *)in_value) {
			*(uint32_t *)out_value = *(uint32_t *)in_value;
		} else {
			*(uint32_t *)out_value = defval;
		}
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

#define ADRV9009_OF_PROP(_member_, _init_member_, _default) \
	__adrv9009_of_get_param(_member_, _init_member_, _default, sizeof(*_member_))

bool adrv9009_check_sysref_rate(uint32_t lmfc, uint32_t sysref)
{
	uint32_t div, mod;

	div = lmfc / sysref;
	mod = lmfc % sysref;

	/* Ignore minor deviations that can be introduced by rounding. */
	return mod <= div || mod >= sysref - div;
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

	pr_info("%s: %s Rev %d, Firmware %u.%u.%u API version: %u.%u.%u.%u successfully initialized%s",
		__func__, "adrv9009-phy", rev, talArmVersionInfo.majorVer,
		talArmVersionInfo.minorVer, talArmVersionInfo.rcVer,
		api_vers[0], api_vers[1], api_vers[2], api_vers[3],
		phy->jdev ? " via jesd204-fsm" : "");
}

struct adrv9009_jesd204_link {
	unsigned int source_id;
	bool is_framer;
};

struct adrv9009_jesd204_priv {
	struct adrv9009_rf_phy *phy;
	struct adrv9009_jesd204_link link[5];
};

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
		deframer = &phy->talInit.jesd204Settings.deframerA;
		lnk->sample_rate = phy->talInit.tx.txProfile.txInputRate_kHz * 1000;
		priv->link[DEFRAMER_LINK_TX].source_id = TAL_DEFRAMER_A;
		break;
	case FRAMER_LINK_RX:
		framer = &phy->talInit.jesd204Settings.framerA;
		lnk->sample_rate = phy->talInit.rx.rxProfile.rxOutputRate_kHz * 1000;
		priv->link[FRAMER_LINK_RX].source_id = TAL_FRAMER_A;
		priv->link[FRAMER_LINK_RX].is_framer = true;
		break;
	case FRAMER_LINK_ORX:
		orx_adc_stitching_enabled =
			(phy->talInit.obsRx.orxProfile.rfBandwidth_Hz > 200000000) ?
			1 : 0;

		if (orx_adc_stitching_enabled) {
			if (phy->talInit.obsRx.framerSel != TAL_FRAMER_B) {
				pr_warning("%s:%d: Can't apply fixup\n",
					   __func__, __LINE__);
			} else {
				phy->talInit.jesd204Settings.framerB.M = 2;
				phy->talInit.jesd204Settings.framerB.F = 2;
				if (phy->talInit.obsRx.obsRxChannelsEnable == TAL_ORX1ORX2)
					phy->talInit.obsRx.obsRxChannelsEnable = TAL_ORX1;
			}
		} else {
			phy->talInit.jesd204Settings.framerB.M = phy->framer_b_m;
			phy->talInit.jesd204Settings.framerB.F = phy->framer_b_f;
			phy->talInit.obsRx.obsRxChannelsEnable = phy->orx_channel_enabled;
		}

		framer = &phy->talInit.jesd204Settings.framerB;
		lnk->sample_rate = phy->talInit.obsRx.orxProfile.orxOutputRate_kHz * 1000;
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

int adrv9009_jesd204_link_pre_setup(struct jesd204_dev *jdev,
				    enum jesd204_state_op_reason reason)
{
	struct adrv9009_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9009_rf_phy *phy = priv->phy;
	uint64_t dev_clk;
	int ret;

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

	ret = no_os_clk_round_rate(phy->dev_clk,
				   phy->talInit.clocks.deviceClock_kHz * 1000, &dev_clk);
	if (ret)
		return JESD204_STATE_CHANGE_ERROR;

	if (dev_clk > 0 && ((dev_clk / 1000) ==
			    phy->talInit.clocks.deviceClock_kHz)) {
		ret = no_os_clk_set_rate(phy->dev_clk, (unsigned long) dev_clk);
		if (ret)
			return JESD204_STATE_CHANGE_ERROR;
	} else {
		pr_err("Requesting device clock %u failed got %ld\n",
		       phy->talInit.clocks.deviceClock_kHz * 1000, dev_clk);
		return -EINVAL;
	}

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
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
			ret = -EFAULT;
		}

		ret = TALISE_enableFramerLink(phy->talDevice,
					      priv->link[lnk->link_id].source_id, 0);
		if (ret != TALACT_NO_ACTION) {
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
			ret = -EFAULT;
		}

		ret = TALISE_enableFramerLink(phy->talDevice,
					      priv->link[lnk->link_id].source_id, 1);
		if (ret != TALACT_NO_ACTION) {
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}

		pr_debug("%s:%d Link %d Framer enabled", __func__, __LINE__,
			 priv->link[lnk->link_id].source_id);

		/*************************************************/
		/**** Enable SYSREF to Talise JESD204B Framer ***/
		/*************************************************/
		/*** < User: Make sure SYSREF is stopped/disabled > ***/
		ret = TALISE_enableSysrefToFramer(phy->talDevice,
						  priv->link[lnk->link_id].source_id, 1);
		if (ret != TALACT_NO_ACTION) {
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}
	} else {
		ret = TALISE_enableSysrefToDeframer(phy->talDevice,
						    priv->link[lnk->link_id].source_id, 0);
		if (ret != TALACT_NO_ACTION) {
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
			ret = -EFAULT;
		}

		ret = TALISE_enableDeframerLink(phy->talDevice,
						priv->link[lnk->link_id].source_id, 0);
		if (ret != TALACT_NO_ACTION) {
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
			ret = -EFAULT;
		}
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
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}
	/* Toggle RESETB pin on Talise device */
	ret = TALISE_resetDevice(phy->talDevice);
	if (ret != TALACT_NO_ACTION) {
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	/* TALISE_initialize() loads the Talise device data structure
	 * settings for the Rx/Tx/ORx profiles, FIR filters, digital
	 * filter enables, calibrates the CLKPLL, loads the user provided Rx
	 * gain tables, and configures the JESD204b serializers/framers/deserializers
	 * and deframers.
	 */
	ret = TALISE_initialize(phy->talDevice, &phy->talInit);
	if (ret != TALACT_NO_ACTION) {
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	/*******************************/
	/***** CLKPLL Status Check *****/
	/*******************************/

	ret = TALISE_getPllsLockStatus(phy->talDevice, &pllLockStatus);
	if (ret != TALACT_NO_ACTION) {
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	/* Assert that Talise CLKPLL is locked */
	if ((pllLockStatus & 0x01) == 0) {
		pr_err("%s:%d: CLKPLL is unlocked (0x%X)\n",
		       __func__, __LINE__, pllLockStatus);
		return -EFAULT;
	}

	/*******************************************************/
	/**** Perform MultiChip Sync (MCS) on Talise Device ***/
	/*******************************************************/
	ret = TALISE_enableMultichipSync(phy->talDevice, 1, &mcsStatus);
	if (ret != TALACT_NO_ACTION) {
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9009_jesd204_setup_stage1(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason)
{
	struct adrv9009_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9009_rf_phy *phy = priv->phy;
	uint8_t mcsStatus;
	int ret;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	ret = TALISE_enableMultichipSync(phy->talDevice, 0, &mcsStatus);
	if (ret != TALACT_NO_ACTION)
		return -EFAULT;

	if ((mcsStatus & 0x08) != 0x08) {
		pr_err("%s:%d Unexpected MCS sync status aaaaa (0x%X)\n",
		       __func__, __LINE__, mcsStatus);
		return -EFAULT;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9009_jesd204_setup_stage2(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason)
{
	struct adrv9009_jesd204_priv *priv = jesd204_dev_priv(jdev);
	uint8_t pllLockStatus_mask, pllLockStatus = 0;
	struct adrv9009_rf_phy *phy = priv->phy;
	uint32_t count = sizeof(armBinary);
	uint8_t mcsStatus = 0;
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
		pr_err("%s:%d Unexpected MCS sync status BBBBB (0x%X)\n",
		       __func__, __LINE__, mcsStatus);
		return -EFAULT;
	}

	/*******************************************************/
	/**** Prepare Talise Arm binary and Load Arm and    ****/
	/**** Stream processor Binaryes                     ****/
	/*******************************************************/

	ret = TALISE_initArm(phy->talDevice, &phy->talInit);
	if (ret != TALACT_NO_ACTION) {
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	ret = TALISE_loadStreamFromBinary(phy->talDevice, &streamBinary[0]);
	if (ret != TALACT_NO_ACTION) {
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	ret = TALISE_loadArmFromBinary(phy->talDevice,  &armBinary[0], count);
	if (ret != TALACT_NO_ACTION) {
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	/* TALISE_verifyArmChecksum() will timeout after 200ms
	 * if ARM checksum is not computed
	 */
	ret = TALISE_verifyArmChecksum(phy->talDevice);
	if (ret != TALACT_NO_ACTION) {
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	ret = TALISE_setArmGpioPins(phy->talDevice, &phy->arm_gpio_config);
	if (ret != TALACT_NO_ACTION) {
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
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
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	ret = TALISE_setRfPllFrequency(phy->talDevice, TAL_RF_PLL,
				       phy->trx_lo_frequency);
	if (ret != TALACT_NO_ACTION) {
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	if (phy->talInit.tx.txChannels == TAL_TXOFF)
		pllLockStatus_mask = 0x3;
	else
		pllLockStatus_mask = 0x7;

	ret = TALISE_getPllsLockStatus(phy->talDevice, &pllLockStatus);
	if ((pllLockStatus & pllLockStatus_mask) != pllLockStatus_mask) {
		no_os_mdelay(200);
		ret = TALISE_getPllsLockStatus(phy->talDevice, &pllLockStatus);
		if ((pllLockStatus & pllLockStatus_mask) != pllLockStatus_mask) {
			pr_err("%s:%d RF PLL unlocked (0x%x)\n",
			       __func__, __LINE__, pllLockStatus);
			return -EFAULT;
		}
	}

	ret = TALISE_enableMultichipRfLOPhaseSync(phy->talDevice, 1);
	if (ret != TALACT_NO_ACTION) {
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
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
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
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
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9009_jesd204_setup_stage5(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason)
{
	struct adrv9009_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9009_rf_phy *phy = priv->phy;
	uint8_t errorFlag;
	int ret;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	ret = TALISE_waitInitCals(phy->talDevice, 60000, &errorFlag);

	if (ret == TALACT_ERR_RERUN_INIT_CALS) {
		/* Try once more */
		ret = TALISE_runInitCals(phy->talDevice,
					 phy->initCalMask & ~TAL_TX_LO_LEAKAGE_EXTERNAL);
		if (ret != TALACT_NO_ACTION) {
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}

		ret = TALISE_waitInitCals(phy->talDevice, 20000, &errorFlag);
	}

	if ((ret != TALACT_NO_ACTION) || errorFlag) {
		uint32_t calsSincePowerUp = 0, calsLastRun = 0, calsMinimum = 0;
		uint8_t initErrCal = 0, initErrCode = 0;

		pr_err("%s:%d (ret %d): Init Cal errorFlag (0x%X)\n",
		       __func__, __LINE__, ret, errorFlag);

		ret = TALISE_getInitCalStatus(phy->talDevice, &calsSincePowerUp,
					      &calsLastRun, &calsMinimum, &initErrCal, &initErrCode);

		pr_err("%s:%d (ret %d): Init Cal calsSincePowerUp (0x%X) calsLastRun (0x%X) calsMinimum (0x%X) initErrCal (0x%X) initErrCode (0x%X)\n",
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
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);

		ret = TALISE_waitInitCals(phy->talDevice, 20000, &errorFlag);
		if ((ret != TALACT_NO_ACTION) || errorFlag) {
			pr_err("%s:%d (ret %d): Init Cal errorFlag (0x%X)",
			       __func__, __LINE__, ret, errorFlag);
		}
	}

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
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}
		/***************************************************/
		/**** Enable SYSREF to Talise JESD204B Deframer ***/
		/***************************************************/
		ret = TALISE_enableSysrefToDeframer(phy->talDevice,
						    priv->link[lnk->link_id].source_id, 1);
		if (ret != TALACT_NO_ACTION) {
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
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
	uint16_t deframerStatus = 0;
	uint8_t framerStatus = 0;
	int ret;

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
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
			ret = -EFAULT;
		}

		if ((framerStatus & 0x07) != 0x05)
			pr_warning("Link%u TAL_FRAMER_A framerStatus 0x%X\n",
				   lnk->link_id, framerStatus);
	} else {
		ret = TALISE_readDeframerStatus(phy->talDevice,
						priv->link[lnk->link_id].source_id, &deframerStatus);
		if (ret != TALACT_NO_ACTION) {
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
			ret = -EFAULT;
		}

		if ((deframerStatus & 0xF7) != 0x86)
			pr_warning("Link%u TAL_DEFRAMER_A deframerStatus 0x%X\n",
				   lnk->link_id, deframerStatus);
	};

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
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}
	ret = TALISE_enableTrackingCals(phy->talDevice, trackingCalMask);
	if (ret != TALACT_NO_ACTION) {
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}
	if (has_rx_and_en(phy)) {
		ret = TALISE_setupRxAgc(phy->talDevice, &phy->rxAgcCtrl);
		if (ret != TALACT_NO_ACTION) {
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}
	}
	if (has_tx_and_en(phy)) {
		ret = TALISE_setTxAttenCtrlPin(phy->talDevice,
					       TAL_TX1, &phy->tx1_atten_ctrl_pin);
		if (ret != TALACT_NO_ACTION) {
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}
		ret = TALISE_setTxAttenCtrlPin(phy->talDevice,
					       TAL_TX2, &phy->tx2_atten_ctrl_pin);
		if (ret != TALACT_NO_ACTION) {
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}
	}
	/* Function to turn radio on, Enables transmitters and receivers */
	/* that were setup during TALISE_initialize() */
	ret = TALISE_radioOn(phy->talDevice);
	if (ret != TALACT_NO_ACTION) {
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}
	ret = TALISE_setRxTxEnable(phy->talDevice,
				   has_rx_and_en(phy) ? (taliseRxORxChannels_t)phy->talInit.rx.rxChannels : 0,
				   has_tx_and_en(phy) ? phy->talInit.tx.txChannels : 0);
	if (ret != TALACT_NO_ACTION) {
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}

	if (has_rx(phy))
		no_os_clk_set_rate(phy->clks[RX_SAMPL_CLK],
				   phy->talInit.rx.rxProfile.rxOutputRate_kHz * 1000);
	if (has_tx(phy)) {
		no_os_clk_set_rate(phy->clks[OBS_SAMPL_CLK],
				   phy->talInit.obsRx.orxProfile.orxOutputRate_kHz * 1000);
		no_os_clk_set_rate(phy->clks[TX_SAMPL_CLK],
				   phy->talInit.tx.txProfile.txInputRate_kHz * 1000);
	}
	ret = TALISE_setupAuxDacs(phy->talDevice, &phy->auxdac);
	if (ret != TALACT_NO_ACTION) {
		pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
		return -EFAULT;
	}
	if (phy->gpio3v3SrcCtrl) {
		ret = TALISE_setGpio3v3SourceCtrl(phy->talDevice,
						  phy->gpio3v3SrcCtrl);
		if (ret != TALACT_NO_ACTION) {
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
			return -EFAULT;
		}
		TALISE_setGpio3v3PinLevel(phy->talDevice, phy->gpio3v3PinLevel);
		TALISE_setGpio3v3Oe(phy->talDevice, phy->gpio3v3OutEn, 0xFFF);
	}
	if (has_tx(phy)) {
		ret = TALISE_setPaProtectionCfg(phy->talDevice,
						&phy->tx_pa_protection);
		if (ret != TALACT_NO_ACTION) {
			pr_err("%s:%d (ret %d)\n", __func__, __LINE__, ret);
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
		[JESD204_OP_LINK_INIT] = {
			.per_link = adrv9009_jesd204_link_init,
		},
		[JESD204_OP_LINK_PRE_SETUP] = {
			.per_device = adrv9009_jesd204_link_pre_setup,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
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

static int adrv9009_phy_parse_agc_params(struct adrv9009_rf_phy *phy,
		const struct adrv9009_init_param *init_param)
{
	int ret;

	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.agcUnderRangeLowInterval_ns,
			       &init_param->rxAgcConfig_init_param->agcPeak.agcUnderRangeLowInterval_ns, 205);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.agcUnderRangeMidInterval,
			       &init_param->rxAgcConfig_init_param->agcPeak.agcUnderRangeMidInterval, 2);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.agcUnderRangeHighInterval,
			       &init_param->rxAgcConfig_init_param->agcPeak.agcUnderRangeHighInterval, 4);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.apdHighThresh,
			       &init_param->rxAgcConfig_init_param->agcPeak.apdHighThresh, 39);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.apdLowGainModeHighThresh,
			       &init_param->rxAgcConfig_init_param->agcPeak.apdLowGainModeHighThresh, 36);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.apdLowThresh,
			       &init_param->rxAgcConfig_init_param->agcPeak.apdLowThresh, 23);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.apdLowGainModeLowThresh,
			       &init_param->rxAgcConfig_init_param->agcPeak.apdLowGainModeLowThresh, 19);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.apdUpperThreshPeakExceededCnt,
			       &init_param->rxAgcConfig_init_param->agcPeak.apdUpperThreshPeakExceededCnt, 6);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.apdLowerThreshPeakExceededCnt,
			       &init_param->rxAgcConfig_init_param->agcPeak.apdLowerThreshPeakExceededCnt, 3);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.apdGainStepAttack,
			       &init_param->rxAgcConfig_init_param->agcPeak.apdGainStepAttack, 4);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.apdGainStepRecovery,
			       &init_param->rxAgcConfig_init_param->agcPeak.apdGainStepRecovery, 2);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.enableHb2Overload,
			       &init_param->rxAgcConfig_init_param->agcPeak.enableHb2Overload, 1);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.hb2OverloadDurationCnt,
			       &init_param->rxAgcConfig_init_param->agcPeak.hb2OverloadDurationCnt, 1);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.hb2OverloadThreshCnt,
			       &init_param->rxAgcConfig_init_param->agcPeak.hb2OverloadThreshCnt, 4);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.hb2HighThresh,
			       &init_param->rxAgcConfig_init_param->agcPeak.hb2HighThresh, 181);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.hb2UnderRangeLowThresh,
			       &init_param->rxAgcConfig_init_param->agcPeak.hb2UnderRangeLowThresh, 45);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.hb2UnderRangeMidThresh,
			       &init_param->rxAgcConfig_init_param->agcPeak.hb2UnderRangeMidThresh, 90);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.hb2UnderRangeHighThresh,
			       &init_param->rxAgcConfig_init_param->agcPeak.hb2UnderRangeHighThresh, 128);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.hb2UpperThreshPeakExceededCnt,
			       &init_param->rxAgcConfig_init_param->agcPeak.hb2UpperThreshPeakExceededCnt, 6);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.hb2LowerThreshPeakExceededCnt,
			       &init_param->rxAgcConfig_init_param->agcPeak.hb2LowerThreshPeakExceededCnt, 3);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.hb2GainStepHighRecovery,
			       &init_param->rxAgcConfig_init_param->agcPeak.hb2GainStepHighRecovery, 2);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.hb2GainStepLowRecovery,
			       &init_param->rxAgcConfig_init_param->agcPeak.hb2GainStepLowRecovery, 4);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.hb2GainStepMidRecovery,
			       &init_param->rxAgcConfig_init_param->agcPeak.hb2GainStepMidRecovery, 8);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.hb2GainStepAttack,
			       &init_param->rxAgcConfig_init_param->agcPeak.hb2GainStepAttack, 4);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.hb2OverloadPowerMode,
			       &init_param->rxAgcConfig_init_param->agcPeak.hb2OverloadPowerMode, 1);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.hb2OvrgSel,
			       &init_param->rxAgcConfig_init_param->agcPeak.hb2OvrgSel, 0);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeak.hb2ThreshConfig,
			       &init_param->rxAgcConfig_init_param->agcPeak.hb2ThreshConfig, 3);
	if (ret)
		return ret;

	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPower.powerEnableMeasurement,
			       &init_param->rxAgcConfig_init_param->agcPower.powerEnableMeasurement, 1);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPower.powerUseRfirOut,
			       &init_param->rxAgcConfig_init_param->agcPower.powerUseRfirOut, 1);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPower.powerUseBBDC2,
			       &init_param->rxAgcConfig_init_param->agcPower.powerUseBBDC2, 0);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPower.underRangeHighPowerThresh,
			       &init_param->rxAgcConfig_init_param->agcPower.underRangeHighPowerThresh, 9);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPower.underRangeLowPowerThresh,
			       &init_param->rxAgcConfig_init_param->agcPower.underRangeLowPowerThresh, 2);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(
		      &phy->rxAgcCtrl.agcPower.underRangeHighPowerGainStepRecovery,
		      &init_param->rxAgcConfig_init_param->agcPower.underRangeHighPowerGainStepRecovery,
		      4);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(
		      &phy->rxAgcCtrl.agcPower.underRangeLowPowerGainStepRecovery,
		      &init_param->rxAgcConfig_init_param->agcPower.underRangeLowPowerGainStepRecovery,
		      4);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPower.powerMeasurementDuration,
			       &init_param->rxAgcConfig_init_param->agcPower.powerMeasurementDuration, 5);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPower.rx1TddPowerMeasDuration,
			       &init_param->rxAgcConfig_init_param->agcPower.rx1TddPowerMeasDuration, 5);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPower.rx1TddPowerMeasDelay,
			       &init_param->rxAgcConfig_init_param->agcPower.rx1TddPowerMeasDelay, 1);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPower.rx2TddPowerMeasDuration,
			       &init_param->rxAgcConfig_init_param->agcPower.rx2TddPowerMeasDuration, 5);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPower.rx2TddPowerMeasDelay,
			       &init_param->rxAgcConfig_init_param->agcPower.rx2TddPowerMeasDelay, 1);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPower.upper0PowerThresh,
			       &init_param->rxAgcConfig_init_param->agcPower.upper0PowerThresh, 2);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPower.upper1PowerThresh,
			       &init_param->rxAgcConfig_init_param->agcPower.upper1PowerThresh, 0);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPower.powerLogShift,
			       &init_param->rxAgcConfig_init_param->agcPower.powerLogShift, 0);
	if (ret)
		return ret;

	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeakWaitTime,
			       &init_param->rxAgcConfig_init_param->agcPeakWaitTime, 4);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcRx1MaxGainIndex,
			       &init_param->rxAgcConfig_init_param->agcRx1MaxGainIndex, 255);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcRx1MinGainIndex,
			       &init_param->rxAgcConfig_init_param->agcRx1MinGainIndex, 195);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcRx2MaxGainIndex,
			       &init_param->rxAgcConfig_init_param->agcRx2MaxGainIndex, 255);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcRx2MinGainIndex,
			       &init_param->rxAgcConfig_init_param->agcRx2MinGainIndex, 195);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcGainUpdateCounter_us,
			       &init_param->rxAgcConfig_init_param->agcGainUpdateCounter_us, 250);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcRx1AttackDelay,
			       &init_param->rxAgcConfig_init_param->agcRx1AttackDelay, 10);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcRx2AttackDelay,
			       &init_param->rxAgcConfig_init_param->agcRx2AttackDelay, 10);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcSlowLoopSettlingDelay,
			       &init_param->rxAgcConfig_init_param->agcSlowLoopSettlingDelay, 16);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcLowThreshPreventGain,
			       &init_param->rxAgcConfig_init_param->agcLowThreshPreventGain, 0);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcChangeGainIfThreshHigh,
			       &init_param->rxAgcConfig_init_param->agcChangeGainIfThreshHigh, 1);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcPeakThreshGainControlMode,
			       &init_param->rxAgcConfig_init_param->agcPeakThreshGainControlMode, 1);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcResetOnRxon,
			       &init_param->rxAgcConfig_init_param->agcResetOnRxon, 0);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcEnableSyncPulseForGainCounter,
			       &init_param->rxAgcConfig_init_param->agcEnableSyncPulseForGainCounter, 0);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcEnableIp3OptimizationThresh,
			       &init_param->rxAgcConfig_init_param->agcEnableIp3OptimizationThresh, 0);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.ip3OverRangeThresh,
			       &init_param->rxAgcConfig_init_param->ip3OverRangeThresh, 31);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.ip3OverRangeThreshIndex,
			       &init_param->rxAgcConfig_init_param->ip3OverRangeThreshIndex, 246);
	if (ret)
		return ret;
	ret = ADRV9009_OF_PROP(&phy->rxAgcCtrl.ip3PeakExceededCnt,
			       &init_param->rxAgcConfig_init_param->ip3PeakExceededCnt, 4);
	if (ret)
		return ret;
	return ADRV9009_OF_PROP(&phy->rxAgcCtrl.agcEnableFastRecoveryLoop,
				&init_param->rxAgcConfig_init_param->agcEnableFastRecoveryLoop, 0);

	return 0;
}

/**
 * Initialize the device.
 * @param dev - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adrv9009_init(struct adrv9009_rf_phy **dev,
		      const struct adrv9009_init_param *init_param)
{
	struct adrv9009_jesd204_priv *priv;
	struct adrv9009_rf_phy *phy;
	int ret;

	phy = (struct adrv9009_rf_phy *)no_os_calloc(1, sizeof(*phy));
	if (!phy)
		goto error;

	phy->talDevice = init_param->adrv9009_device;
	phy->spi_device_id = 0;
	phy->device_id = 0;
	phy->dev_clk = init_param->dev_clk;

	ret = adrv9009_phy_parse_agc_params(phy, init_param);
	if (ret)
		goto error_setup;

	// Copy the settings from the talInit structure to phy->talInit
	memcpy(&phy->talInit, adrv9009_initial_settings_get(), sizeof(phy->talInit));

	phy->framer_b_m = phy->talInit.jesd204Settings.framerB.M;
	phy->framer_b_f = phy->talInit.jesd204Settings.framerB.F;
	phy->orx_channel_enabled = phy->talInit.obsRx.obsRxChannelsEnable;

	phy->loopFilter_stability = 3;
	phy->trx_lo_frequency = 2400000000ULL;
	if (init_param->trx_lo_frequency)
		phy->trx_lo_frequency = init_param->trx_lo_frequency;

	phy->tx_pa_protection.tx1PowerThreshold = 4096;
	if (init_param->tx_pa_protection.tx1PowerThreshold)
		phy->tx_pa_protection.tx1PowerThreshold =
			init_param->tx_pa_protection.tx1PowerThreshold;

	phy->tx_pa_protection.tx2PowerThreshold = 4096;
	if (init_param->tx_pa_protection.tx2PowerThreshold)
		phy->tx_pa_protection.tx2PowerThreshold =
			init_param->tx_pa_protection.tx2PowerThreshold;

	phy->tx_pa_protection.tx1PeakThreshold = 128;
	if (init_param->tx_pa_protection.tx1PeakThreshold)
		phy->tx_pa_protection.tx1PeakThreshold =
			init_param->tx_pa_protection.tx1PeakThreshold;

	phy->tx_pa_protection.tx2PeakThreshold = 128;
	if (init_param->tx_pa_protection.tx2PeakThreshold)
		phy->tx_pa_protection.tx2PeakThreshold =
			init_param->tx_pa_protection.tx2PeakThreshold;

	strncpy(phy->fw, init_param->armImageFile, sizeof(phy->fw));
	strncpy(phy->stream, init_param->streamImageFile, sizeof(phy->stream));

	ret = adrv9009_setup(phy);
	if (ret < 0) {
		pr_err("%s: adrv9009_setup failed (%d)\n", __func__, ret);
		goto error_setup;
	}

	ret = jesd204_dev_register(&phy->jdev, &jesd204_adrv9009_init);
	if (ret)
		goto error_setup;

	priv = jesd204_dev_priv(phy->jdev);
	priv->phy = phy;

	*dev = phy;

	return 0;
error_setup:
	no_os_free(phy);
error:
	return ret;
}

static int32_t adrv9009_bb_recalc_rate(struct no_os_clk_desc *desc,
				       uint64_t *rate)
{
	struct adrv9009_rf_phy *adrv9009_dev;

	adrv9009_dev = desc->dev_desc;

	*rate = 0;
	if (!strcmp(desc->name, "-rx_sampl_clk"))
		*rate = adrv9009_dev->talInit.rx.rxProfile.rxOutputRate_kHz * 1000;
	else if (!strcmp(desc->name, "-orx_sampl_clk"))
		*rate = adrv9009_dev->talInit.obsRx.orxProfile.orxOutputRate_kHz * 1000;
	else if (!strcmp(desc->name, "-tx_sampl_clk"))
		*rate = adrv9009_dev->talInit.tx.txProfile.txInputRate_kHz * 1000;
	else
		return -EINVAL;

	return 0;
}

static int32_t adrv9009_bb_set_rate(struct no_os_clk_desc *desc,
				    uint64_t rate)
{
	// Do nothing
	return 0;
}

static int32_t adrv9009_bb_round_rate(struct no_os_clk_desc *desc,
				      uint64_t rate,
				      uint64_t *rounded_rate)
{
	pr_debug("%s: Rate %lu Hz", __func__, rate);

	*rounded_rate = rate;

	return 0;
}

static const struct no_os_clk_platform_ops adrv9009_bb_clk_ops = {
	.clk_round_rate = &adrv9009_bb_round_rate,
	.clk_set_rate = &adrv9009_bb_set_rate,
	.clk_recalc_rate = &adrv9009_bb_recalc_rate,
};

int adrv9009_setup(struct adrv9009_rf_phy *phy)
{
	const char *names[NUM_ADRV9009_CLKS] = {
		"-rx_sampl_clk", "-orx_sampl_clk", "-tx_sampl_clk"
	};
	struct no_os_clk_desc *rx_sample_clk = NULL;
	struct no_os_clk_desc *orx_sample_clk = NULL;
	struct no_os_clk_desc *tx_sample_clk = NULL;
	struct no_os_clk_init_param clk_init;
	uint32_t api_vers[4];
	uint8_t rev;
	int ret;

#if defined(ADRV9008_1)
	phy->initCalMask = TAL_ADC_TUNER | TAL_TIA_3DB_CORNER | TAL_DC_OFFSET |
			   TAL_RX_GAIN_DELAY | TAL_FLASH_CAL | TAL_RX_QEC_INIT;
#elif defined(ADRV9008_2)
	phy->initCalMask = TAL_TX_BB_FILTER | TAL_ADC_TUNER | TAL_TIA_3DB_CORNER |
			   TAL_DC_OFFSET | TAL_FLASH_CAL | TAL_PATH_DELAY |
			   TAL_TX_LO_LEAKAGE_INTERNAL | TAL_TX_QEC_INIT |
			   TAL_LOOPBACK_RX_LO_DELAY | TAL_LOOPBACK_RX_RX_QEC_INIT |
			   TAL_ORX_QEC_INIT | TAL_TX_DAC  | TAL_ADC_STITCHING;
#else
	phy->initCalMask =  TAL_TX_BB_FILTER | TAL_ADC_TUNER |  TAL_TIA_3DB_CORNER |
			    TAL_DC_OFFSET | TAL_RX_GAIN_DELAY | TAL_FLASH_CAL |
			    TAL_PATH_DELAY | TAL_TX_LO_LEAKAGE_INTERNAL |
			    TAL_TX_QEC_INIT | TAL_LOOPBACK_RX_LO_DELAY |
			    TAL_LOOPBACK_RX_RX_QEC_INIT | TAL_RX_QEC_INIT |
			    TAL_ORX_QEC_INIT | TAL_TX_DAC  | TAL_ADC_STITCHING;
#endif

	ret = no_os_clk_enable(phy->dev_clk);
	if (ret)
		return ret;

	/*******************************/
	/**** Talise Initialization ***/
	/*******************************/

	/*Open Talise Hw Device*/
	ret = TALISE_openHw(phy->talDevice);
	if (ret != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		pr_info("error: TALISE_openHw() failed\n");
		goto error;
	}

	/* Toggle RESETB pin on Talise device */
	ret = TALISE_resetDevice(phy->talDevice);
	if (ret != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		pr_info("error: TALISE_resetDevice() failed\n");
		goto error;
	}

	/* Set 3 or 4-wire SPI mode, MSBFirst/LSBfirst in device, pushes CPOL=0, CPHA=0, longInstWord=1 into device->spiSettings */
	ret = (talRecoveryActions_t)TALISE_setSpiSettings(phy->talDevice,
			adrv9009_spi_settings_get());
	if (ret != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		pr_info("error: TALISE_setSpiSettings() failed\n");
		goto error;
	}

	/* Initialize clk components */
	rx_sample_clk = no_os_calloc(1, sizeof(rx_sample_clk));
	if (!rx_sample_clk)
		goto error;

	clk_init.name = names[RX_SAMPL_CLK];
	clk_init.hw_ch_num = 1;
	clk_init.platform_ops = &adrv9009_bb_clk_ops;
	clk_init.dev_desc = phy;

	ret = no_os_clk_init(&rx_sample_clk, &clk_init);
	if (ret)
		goto rx_out_clk_init_error;

	phy->clks[RX_SAMPL_CLK] = rx_sample_clk;

	orx_sample_clk = no_os_calloc(1, sizeof(orx_sample_clk));
	if (!orx_sample_clk)
		goto rx_out_clk_init_error;

	clk_init.name = names[OBS_SAMPL_CLK];
	clk_init.hw_ch_num = 1;
	clk_init.platform_ops = &adrv9009_bb_clk_ops;
	clk_init.dev_desc = phy;

	ret = no_os_clk_init(&orx_sample_clk, &clk_init);
	if (ret)
		goto orx_out_clk_init_error;

	phy->clks[OBS_SAMPL_CLK] = orx_sample_clk;

	tx_sample_clk = no_os_calloc(1, sizeof(tx_sample_clk));
	if (!tx_sample_clk)
		goto orx_out_clk_init_error;

	clk_init.name = names[TX_SAMPL_CLK];
	clk_init.hw_ch_num = 1;
	clk_init.platform_ops = &adrv9009_bb_clk_ops;
	clk_init.dev_desc = phy;

	ret = no_os_clk_init(&tx_sample_clk, &clk_init);
	if (ret)
		goto tx_out_clk_init_error;

	phy->clks[TX_SAMPL_CLK] = tx_sample_clk;

	TALISE_getDeviceRev(phy->talDevice, &rev);
	TALISE_getApiVersion(phy->talDevice,
			     &api_vers[0], &api_vers[1], &api_vers[2], &api_vers[3]);

	pr_info("Talise: Device Revision %d, API %lu.%lu.%lu.%lu\n",
		rev, api_vers[0], api_vers[1], api_vers[2], api_vers[3]);

	/* Close Talise Hw Device*/
	ret = TALISE_closeHw(phy->talDevice);
	if (ret != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		pr_info("error: TALISE_closeHw() failed\n");
		goto error;
	}

	return 0;

tx_out_clk_init_error:
	no_os_free(tx_sample_clk);
orx_out_clk_init_error:
	no_os_free(orx_sample_clk);
rx_out_clk_init_error:
	no_os_free(rx_sample_clk);
error:
	return ret;
}
