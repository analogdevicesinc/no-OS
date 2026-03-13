/***************************************************************************//**
 *   @file   adrv903x.c
 *   @brief  ADRV903X (ADRV9030/ADRV9032) no-OS driver implementation.
 *   @author Analog Devices Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders. This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "adrv903x.h"
#include "adi_adrv903x_core.h"
#include "adi_adrv903x_utilities.h"
#include "adi_adrv903x_datainterface.h"
#include "adi_adrv903x_cals.h"
#include "adi_adrv903x_radioctrl.h"
#include "adi_adrv903x_tx.h"
#include "adi_common_error.h"
#include "jesd204.h"
#include "no_os_alloc.h"
#include "no_os_clk.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "app_config.h"
#include <string.h>

/******************************************************************************/
/************************ JESD204 FSM Structures ******************************/
/******************************************************************************/

struct adrv903x_jesd204_link {
	unsigned int source_id;
	bool is_framer;
};

struct adrv903x_jesd204_priv {
	struct adrv903x_rf_phy *phy;
	struct adrv903x_jesd204_link link[3];
};

/******************************************************************************/
/******************** JESD204 FSM Callback Functions **************************/
/******************************************************************************/

/**
 * @brief JESD204 link_setup callback — arm MCS on the device.
 *
 * Calls MultichipSyncSet_v2(START) so the device starts listening for
 * SYSREF pulses.  The FSM fires SYSREF via post_state_sysref after this
 * callback returns, delivering the first pulse to both the device and
 * the FPGA JESD204 IP simultaneously.
 */
static int adrv903x_jesd204_link_setup(struct jesd204_dev *jdev,
					enum jesd204_state_op_reason reason)
{
	struct adrv903x_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv903x_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	ret = adi_adrv903x_MultichipSyncSet_v2(phy->palmaDevice,
					       ADI_ADRV903X_MCS_START);
	if (ret) {
		pr_err("adrv903x: MultichipSyncSet_v2 START failed (%d)\n",
		       ret);
		return JESD204_STATE_CHANGE_ERROR;
	}

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 setup_stage1 callback — poll MCS status, fire SYSREF
 *        until the device reports sync complete.
 *
 * Mirrors the adrv904x pattern: call jesd204_sysref_async_force() per
 * iteration so the FSM fires the AD9528 GPIO-triggered NSHOT burst each
 * time.  The loop exits as soon as bit 0 of mcsStatus is set.
 * post_state_sysref fires one additional pulse after this callback.
 */
static int adrv903x_jesd204_setup_stage1(struct jesd204_dev *jdev,
					  enum jesd204_state_op_reason reason)
{
	struct adrv903x_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv903x_rf_phy *phy = priv->phy;
	uint32_t mcsStatus = 0;
	int i, ret;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	for (i = 0; i < 255; i++) {
		ret = adi_adrv903x_MultichipSyncStatusGet(phy->palmaDevice,
							  &mcsStatus);
		if (ret) {
			pr_err("adrv903x: MultichipSyncStatusGet failed (%d)\n",
			       ret);
			return JESD204_STATE_CHANGE_ERROR;
		}

		if ((mcsStatus & 0x01U) == 0x01U)
			break;

		jesd204_sysref_async_force(phy->jdev);
	}

	if ((mcsStatus & 0x01U) != 0x01U) {
		pr_err("adrv903x: MCS did not complete after %d pulses (status=0x%x)\n",
		       i, mcsStatus);
		return JESD204_STATE_CHANGE_ERROR;
	}

	pr_info("adrv903x: MCS complete (status=0x%x, %d pulse(s))\n",
		mcsStatus, i + 1);

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 setup_stage2 callback — disarm MCS and run PostMcsInit.
 *
 * Calls MultichipSyncSet_v2(OFF) then PostMcsInit.  post_state_sysref
 * fires SYSREF once more after this callback so the FPGA has captured
 * a fresh pulse before CLOCKS_ENABLE and LINK_ENABLE run.
 */
static int adrv903x_jesd204_setup_stage2(struct jesd204_dev *jdev,
					  enum jesd204_state_op_reason reason)
{
	struct adrv903x_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv903x_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	ret = adi_adrv903x_MultichipSyncSet_v2(phy->palmaDevice,
					       ADI_ADRV903X_MCS_OFF);
	if (ret) {
		pr_err("adrv903x: MultichipSyncSet_v2 OFF failed (%d)\n", ret);
		return JESD204_STATE_CHANGE_ERROR;
	}

	ret = adi_adrv903x_PostMcsInit(phy->palmaDevice, &phy->postMcsInitInst);
	if (ret) {
		pr_err("adrv903x: PostMcsInit failed (%d)\n", ret);
		return JESD204_STATE_CHANGE_ERROR;
	}

	pr_info("adrv903x: PostMcsInit complete\n");

	ret = adi_adrv903x_SerializerReset(phy->palmaDevice);
	if (ret) {
		pr_err("adrv903x: SerializerReset failed (%d)\n", ret);
		return JESD204_STATE_CHANGE_ERROR;
	}

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 link_init callback — read framer/deframer configuration
 *        from the device and populate the JESD204 link parameters.
 */
static int adrv903x_jesd204_link_init(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason,
				      struct jesd204_link *lnk)
{
	struct adrv903x_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv903x_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	switch (lnk->link_id) {
	case DEFRAMER0_LINK_TX: {
		adi_adrv903x_DeframerCfg_t deframerCfg = { 0 };

		ret = adi_adrv903x_DeframerCfgGet(phy->palmaDevice,
						  ADI_ADRV903X_DEFRAMER_0,
						  &deframerCfg);
		if (ret) {
			pr_err("adrv903x: DeframerCfgGet failed (%d)\n", ret);
			return JESD204_STATE_CHANGE_ERROR;
		}

		priv->link[lnk->link_id].source_id = ADI_ADRV903X_DEFRAMER_0;
		priv->link[lnk->link_id].is_framer = false;

		lnk->num_lanes = no_os_hweight8(
					deframerCfg.deserializerLanesEnabled);
		lnk->num_converters = deframerCfg.jesd204M;
		lnk->bits_per_sample = deframerCfg.jesd204Np;
		lnk->octets_per_frame = deframerCfg.jesd204F;
		lnk->frames_per_multiframe = deframerCfg.jesd204K;
		lnk->device_id = deframerCfg.deviceId;
		lnk->bank_id = deframerCfg.bankId;
		lnk->scrambling = deframerCfg.decrambling;
		lnk->converter_resolution = deframerCfg.jesd204Np;
		lnk->num_of_multiblocks_in_emb = deframerCfg.jesd204E;
		lnk->ctrl_bits_per_sample = 0;
		lnk->jesd_version = deframerCfg.enableJesd204C ?
				    JESD204_VERSION_C : JESD204_VERSION_B;
		lnk->jesd_encoder = deframerCfg.enableJesd204C ?
				    JESD204_ENCODER_64B66B :
				    JESD204_ENCODER_8B10B;
		lnk->subclass = JESD204_SUBCLASS_1;
		lnk->is_transmit = true;
		lnk->sample_rate = deframerCfg.iqRate_kHz * 1000ULL;

		pr_info("adrv903x: deframer0 M=%u F=%u K=%u Np=%u lanes=%u %s\n",
			deframerCfg.jesd204M, deframerCfg.jesd204F,
			deframerCfg.jesd204K, deframerCfg.jesd204Np,
			lnk->num_lanes,
			deframerCfg.enableJesd204C ? "204C" : "204B");
		break;
	}
	case FRAMER0_LINK_RX: {
		adi_adrv903x_FramerCfg_t framerCfg = { 0 };

		ret = adi_adrv903x_FramerCfgGet(phy->palmaDevice,
						ADI_ADRV903X_FRAMER_0,
						&framerCfg);
		if (ret) {
			pr_err("adrv903x: FramerCfgGet failed (%d)\n", ret);
			return JESD204_STATE_CHANGE_ERROR;
		}

		priv->link[lnk->link_id].source_id = ADI_ADRV903X_FRAMER_0;
		priv->link[lnk->link_id].is_framer = true;

		lnk->num_lanes = no_os_hweight8(
					framerCfg.serializerLanesEnabled);
		lnk->num_converters = framerCfg.jesd204M;
		lnk->bits_per_sample = framerCfg.jesd204Np;
		lnk->octets_per_frame = framerCfg.jesd204F;
		lnk->frames_per_multiframe = framerCfg.jesd204K;
		lnk->device_id = framerCfg.deviceId;
		lnk->bank_id = framerCfg.bankId;
		lnk->scrambling = framerCfg.scramble;
		lnk->converter_resolution = framerCfg.jesd204Np;
		lnk->num_of_multiblocks_in_emb = framerCfg.jesd204E;
		lnk->ctrl_bits_per_sample = 0;
		lnk->jesd_version = framerCfg.enableJesd204C ?
				    JESD204_VERSION_C : JESD204_VERSION_B;
		lnk->jesd_encoder = framerCfg.enableJesd204C ?
				    JESD204_ENCODER_64B66B :
				    JESD204_ENCODER_8B10B;
		lnk->subclass = JESD204_SUBCLASS_1;
		lnk->is_transmit = false;
		lnk->sample_rate = framerCfg.iqRate_kHz * 1000ULL;

		pr_info("adrv903x: framer0 M=%u F=%u K=%u Np=%u lanes=%u %s\n",
			framerCfg.jesd204M, framerCfg.jesd204F,
			framerCfg.jesd204K, framerCfg.jesd204Np,
			lnk->num_lanes,
			framerCfg.enableJesd204C ? "204C" : "204B");
		break;
	}
	default:
		return JESD204_STATE_CHANGE_DONE;
	}

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 clks_enable callback.
 *
 * Framer (ADRV903X TX → FPGA RX):
 *   Enable SYSREF → disable link → enable link → disable SYSREF.
 *
 * Deframer (FPGA TX → ADRV903X RX):
 *   Disable SYSREF → disable link (prepare for link_enable phase).
 */
static int adrv903x_jesd204_clks_enable(struct jesd204_dev *jdev,
					enum jesd204_state_op_reason reason,
					struct jesd204_link *lnk)
{
	struct adrv903x_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv903x_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	if (!lnk->num_converters)
		return JESD204_STATE_CHANGE_DONE;

	if (priv->link[lnk->link_id].is_framer) {
		ret = adi_adrv903x_FramerSysrefCtrlSet(phy->palmaDevice,
			priv->link[lnk->link_id].source_id, 1);
		if (ret) {
			pr_err("adrv903x: FramerSysrefCtrlSet enable failed (%d)\n",
			       ret);
			return JESD204_STATE_CHANGE_ERROR;
		}

		ret = adi_adrv903x_FramerLinkStateSet(phy->palmaDevice,
			priv->link[lnk->link_id].source_id, 0);
		if (ret) {
			pr_err("adrv903x: FramerLinkStateSet disable failed (%d)\n",
			       ret);
			return JESD204_STATE_CHANGE_ERROR;
		}

		ret = adi_adrv903x_FramerLinkStateSet(phy->palmaDevice,
			priv->link[lnk->link_id].source_id, 1);
		if (ret) {
			pr_err("adrv903x: FramerLinkStateSet enable failed (%d)\n",
			       ret);
			return JESD204_STATE_CHANGE_ERROR;
		}

		pr_info("adrv903x: framer %u enabled\n",
			priv->link[lnk->link_id].source_id);

		ret = adi_adrv903x_FramerSysrefCtrlSet(phy->palmaDevice,
			priv->link[lnk->link_id].source_id, 0);
		if (ret) {
			pr_err("adrv903x: FramerSysrefCtrlSet disable failed (%d)\n",
			       ret);
			return JESD204_STATE_CHANGE_ERROR;
		}
	} else {
		ret = adi_adrv903x_DeframerSysrefCtrlSet(phy->palmaDevice,
			ADI_ADRV903X_ALL_DEFRAMER, 0);
		if (ret) {
			pr_err("adrv903x: DeframerSysrefCtrlSet disable failed (%d)\n",
			       ret);
			return JESD204_STATE_CHANGE_ERROR;
		}

		ret = adi_adrv903x_DeframerLinkStateSet(phy->palmaDevice,
			ADI_ADRV903X_ALL_DEFRAMER, 0);
		if (ret) {
			pr_err("adrv903x: DeframerLinkStateSet disable failed (%d)\n",
			       ret);
			return JESD204_STATE_CHANGE_ERROR;
		}
	}

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 link_enable callback.
 *
 * Deframer only (FPGA TX → ADRV903X RX):
 *   Enable deframer link → run SERDES initial calibration →
 *   wait for cal completion → disable/re-enable link → enable SYSREF.
 *
 * This callback runs AFTER the FPGA TX lane clocks are enabled by the
 * axi_jesd204_tx link_enable callback, so SERDES cal sees serial data.
 */
static int adrv903x_jesd204_link_enable(struct jesd204_dev *jdev,
					enum jesd204_state_op_reason reason,
					struct jesd204_link *lnk)
{
	struct adrv903x_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv903x_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	if (!lnk->num_converters)
		return JESD204_STATE_CHANGE_DONE;

	if (!priv->link[lnk->link_id].is_framer) {
		adi_adrv903x_InitCals_t serdesCal = { 0 };

		ret = adi_adrv903x_DeframerLinkStateSet(phy->palmaDevice,
			priv->link[lnk->link_id].source_id, 1);
		if (ret) {
			pr_err("adrv903x: DeframerLinkStateSet enable failed (%d)\n",
			       ret);
			return JESD204_STATE_CHANGE_ERROR;
		}

		/* Run SERDES initial calibration for deframer alignment */
		serdesCal.calMask = ADI_ADRV903X_IC_SERDES;
		serdesCal.rxChannelMask = 0xFF;

		ret = adi_adrv903x_InitCalsRun(phy->palmaDevice, &serdesCal);
		if (ret) {
			pr_err("adrv903x: InitCalsRun SERDES failed (%d)\n",
			       ret);
			return JESD204_STATE_CHANGE_ERROR;
		}

		ret = adi_adrv903x_InitCalsWait_v2(phy->palmaDevice,
						   60000, NULL);
		if (ret) {
			pr_err("adrv903x: InitCalsWait SERDES failed (%d)\n",
			       ret);
			pr_info("adrv903x: continuing despite SERDES cal error\n");
		} else {
			pr_info("adrv903x: SERDES calibration complete\n");
		}

		/* Disable SYSREF before enabling the deframer link */
		ret = adi_adrv903x_DeframerSysrefCtrlSet(phy->palmaDevice,
			priv->link[lnk->link_id].source_id, 0);
		if (ret) {
			pr_err("adrv903x: DeframerSysrefCtrlSet disable failed (%d)\n",
			       ret);
			return JESD204_STATE_CHANGE_ERROR;
		}

		ret = adi_adrv903x_DeframerLinkStateSet(phy->palmaDevice,
			priv->link[lnk->link_id].source_id, 1);
		if (ret) {
			pr_err("adrv903x: DeframerLinkStateSet enable2 failed (%d)\n",
			       ret);
			return JESD204_STATE_CHANGE_ERROR;
		}

		ret = adi_adrv903x_DeframerSysrefCtrlSet(phy->palmaDevice,
			priv->link[lnk->link_id].source_id, 1);
		if (ret) {
			pr_err("adrv903x: DeframerSysrefCtrlSet enable failed (%d)\n",
			       ret);
			return JESD204_STATE_CHANGE_ERROR;
		}

		pr_info("adrv903x: deframers enabled, JESD204 link ready\n");
	}

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 link_running callback — verify link status.
 */
static int adrv903x_jesd204_link_running(struct jesd204_dev *jdev,
					 enum jesd204_state_op_reason reason,
					 struct jesd204_link *lnk)
{
	struct adrv903x_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv903x_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	if (!lnk->num_converters)
		return JESD204_STATE_CHANGE_DONE;

	if (priv->link[lnk->link_id].is_framer) {
		adi_adrv903x_FramerStatus_t framerStatus = { 0 };

		ret = adi_adrv903x_FramerStatusGet(phy->palmaDevice,
			priv->link[lnk->link_id].source_id, &framerStatus);
		if (ret) {
			pr_err("adrv903x: FramerStatusGet failed (%d)\n", ret);
			return JESD204_STATE_CHANGE_ERROR;
		}

		pr_info("adrv903x: link%u framer status 0x%X\n",
			lnk->link_id, framerStatus.status);

		if ((framerStatus.status & 0x02) != 0x02)
			pr_warning("adrv903x: link%u framer not synced (status=0x%X)\n",
				   lnk->link_id, framerStatus.status);
	} else {
		adi_adrv903x_DeframerStatus_v2_t deframerStatus = { 0 };

		ret = adi_adrv903x_DeframerStatusGet_v2(phy->palmaDevice,
			priv->link[lnk->link_id].source_id, &deframerStatus);
		if (ret) {
			pr_err("adrv903x: DeframerStatusGet_v2 failed (%d)\n",
			       ret);
			return JESD204_STATE_CHANGE_ERROR;
		}

		pr_info("adrv903x: link%u deframer linkState 0x%X\n",
			lnk->link_id, deframerStatus.linkState);

		for (int i = 0; i < (int)lnk->num_lanes; i++)
			pr_info("adrv903x: link%u lane%d status 0x%X\n",
				lnk->link_id, i,
				deframerStatus.laneStatus[i]);
	}

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 post_running_stage callback — activate signal chain.
 *
 * Mirrors the Linux driver's adrv903x_jesd204_post_running_stage():
 *   1. Enable-disable-enable cycle on TX and RX channels.
 *      A single enable is insufficient — the device's internal data path
 *      from deframer→TX DAC and RX ADC→framer requires this "kick".
 *   2. Set default TX attenuation (6 dB).
 *
 * Note: ORX channels are excluded — they require observation path mapping
 * (adi_adrv903x_OrxEnableSet) not configured in the basic/DMA examples.
 *
 * Tracking calibrations are not enabled here. In the UC101 4T4R profile,
 * enabling TX tracking cals (TC_TX_LB_ADC, TC_TX_SERDES) unconditionally
 * causes an ARM CPU exception. Linux guards each cal with
 * adi_adrv903x_InitCalsDetailedStatusGet() and skips cals that were not
 * run during InitCals. TODO: add that check.
 */
static int adrv903x_jesd204_post_running_stage(struct jesd204_dev *jdev,
					       enum jesd204_state_op_reason reason)
{
	struct adrv903x_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv903x_rf_phy *phy = priv->phy;
	uint32_t init_chans, tx_mask = 0, rx_mask = 0;
	adi_adrv903x_TxAtten_t txAtten[1];
	int ret;
	uint8_t i;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	/* Build TX/RX channel masks from the device's initialized bitmap */
	init_chans = phy->palmaDevice->devStateInfo.initializedChannels;

	for (i = 0; i < ADI_ADRV903X_MAX_TXCHANNELS; i++) {
		if (init_chans & (1U << (i + ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET)))
			tx_mask |= (ADI_ADRV903X_TX0 << i);
	}

	for (i = 0; i < ADI_ADRV903X_MAX_RX_ONLY; i++) {
		if (init_chans & (1U << i))
			rx_mask |= (ADI_ADRV903X_RX0 << i);
	}

	pr_info("adrv903x: initialized channels 0x%x (TX=0x%x RX=0x%x)\n",
		init_chans, tx_mask, rx_mask);

	/* Enable-disable-enable cycle: required to activate the signal chain */
	ret = (int)adi_adrv903x_RxTxEnableSet(phy->palmaDevice,
					       0, 0,
					       rx_mask, rx_mask,
					       tx_mask, tx_mask);
	if (ret) {
		pr_err("adrv903x: RxTxEnableSet (enable) failed (%d)\n", ret);
		return JESD204_STATE_CHANGE_ERROR;
	}

	ret = (int)adi_adrv903x_RxTxEnableSet(phy->palmaDevice,
					       0, 0,
					       rx_mask, 0,
					       tx_mask, 0);
	if (ret) {
		pr_err("adrv903x: RxTxEnableSet (disable) failed (%d)\n", ret);
		return JESD204_STATE_CHANGE_ERROR;
	}

	ret = (int)adi_adrv903x_RxTxEnableSet(phy->palmaDevice,
					       0, 0,
					       rx_mask, rx_mask,
					       tx_mask, tx_mask);
	if (ret) {
		pr_err("adrv903x: RxTxEnableSet (re-enable) failed (%d)\n", ret);
		return JESD204_STATE_CHANGE_ERROR;
	}

	/* Set TX attenuation to 6 dB (matches Linux default) */
	txAtten[0].txChannelMask = tx_mask;
	txAtten[0].txAttenuation_mdB = 6000;
	ret = (int)adi_adrv903x_TxAttenSet(phy->palmaDevice, txAtten, 1);
	if (ret) {
		pr_err("adrv903x: TxAttenSet failed (%d)\n", ret);
		return JESD204_STATE_CHANGE_ERROR;
	}

	pr_info("adrv903x: signal chain activated\n");

	return JESD204_STATE_CHANGE_DONE;
}

/******************************************************************************/
/************************ JESD204 Device Registration *************************/
/******************************************************************************/

static const struct jesd204_dev_data jesd204_adrv903x_init = {
	.state_ops = {
		[JESD204_OP_LINK_INIT] = {
			.per_link = adrv903x_jesd204_link_init,
		},
		[JESD204_OP_LINK_SETUP] = {
			.per_device = adrv903x_jesd204_link_setup,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_OPT_SETUP_STAGE1] = {
			.per_device = adrv903x_jesd204_setup_stage1,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_OPT_SETUP_STAGE2] = {
			.per_device = adrv903x_jesd204_setup_stage2,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_CLOCKS_ENABLE] = {
			.per_link = adrv903x_jesd204_clks_enable,
		},
		[JESD204_OP_LINK_ENABLE] = {
			.per_link = adrv903x_jesd204_link_enable,
			.post_state_sysref = true,
		},
		[JESD204_OP_LINK_RUNNING] = {
			.per_link = adrv903x_jesd204_link_running,
		},
		[JESD204_OP_OPT_POST_RUNNING_STAGE] = {
			.per_device = adrv903x_jesd204_post_running_stage,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
	},

	.max_num_links = 3, /* link[] array holds indices 0–2; max used link_id is FRAMER0_LINK_RX=2 */
	.sizeof_priv = sizeof(struct adrv903x_jesd204_priv),
};

/******************************************************************************/
/********************** Functions Implementation ******************************/
/******************************************************************************/

/**
 * @brief Initialize ADRV903X device.
 *
 * Sequence:
 *   1.  Allocate and configure device structure
 *   2.  adi_adrv903x_HwOpen              — open SPI bus and GPIO
 *   3.  adi_adrv903x_HwReset             — hardware reset pulse
 *   4.  adi_adrv903x_SpiVerify           — loopback SPI test
 *   5.  adi_adrv903x_ApiVersionGet       — confirm device responds
 *   6.  adi_adrv903x_InitDataExtract     — extract init structs from profile binary
 *   7.  adi_adrv903x_PreMcsInit          — load stream/FW/profile/gain tables; start ARM CPU
 *   8.  adi_adrv903x_PreMcsInit_NonBroadcast — per-device non-broadcast config
 *   9.  Register JESD204 device for FSM-driven link bring-up
 *
 * MCS (MultichipSyncSet_v2 + SYSREF polling) and PostMcsInit are handled
 * by the JESD204 FSM callbacks (LINK_SETUP, OPT_SETUP_STAGE1,
 * OPT_SETUP_STAGE2) so that SYSREF is fired through the FSM before
 * LINK_ENABLE.  This ensures adxcvr elastic buffer alignment succeeds on
 * the first attempt rather than after 100 retries.
 *
 * JESD204 link bring-up (framer/deframer enable, SerializerReset, SERDES
 * calibration) is handled by CLOCKS_ENABLE and LINK_ENABLE callbacks.
 * Call jesd204_topology_init() and jesd204_fsm_start() after this
 * function returns to start the link.
 *
 * @param phy        - Output pointer to the allocated device structure.
 * @param init_param - Initialization parameters.
 *
 * @return 0 on success, negative error code on failure.
 */
int adrv903x_init(struct adrv903x_rf_phy **phy,
		  const struct adrv903x_init_param *init_param)
{
	const char *dev_name = "ADRV903X";
	adi_adrv903x_ExtractInitDataOutput_e extractOutput;
	adi_adrv903x_CpuFwVersion_t armVersion = { 0 };
	adi_adrv903x_Version_t streamVersion = { 0 };
	adi_adrv903x_Version_t apiVersion = { 0 };
	struct adrv903x_jesd204_priv *priv;
	adi_common_ErrData_t *errPtr;
	struct adrv903x_rf_phy *p;
	bool hw_open_ok = false;
	int ret;

	if (!phy || !init_param)
		return -EINVAL;

	p = no_os_calloc(1, sizeof(*p));
	if (!p)
		return -ENOMEM;

	p->dev_clk     = init_param->dev_clk;
	p->palmaDevice = &p->adi_adrv903x_device;
	p->spiSettings = *adrv903x_spi_settings_get();

	/* Point the API device's HAL handle at our hal struct */
	p->palmaDevice->common.devHalInfo = &p->hal;

	/* Allocate error data block required by the API */
	errPtr = no_os_calloc(1, sizeof(*errPtr));
	if (!errPtr) {
		ret = -ENOMEM;
		goto error_free_phy;
	}
	p->palmaDevice->common.errPtr           = errPtr;
	p->palmaDevice->common.deviceInfo.id   = 0x9030;
	p->palmaDevice->common.deviceInfo.name = dev_name;
	p->palmaDevice->common.deviceInfo.type = 0x00;

	/* Set up file paths for embedded firmware binaries */
	strncpy((char *)p->trxBinaryInfoPtr.cpuProfile.filePath,
		ADRV903X_PROFILE_FILE,
		sizeof(p->trxBinaryInfoPtr.cpuProfile.filePath) - 1);
	strncpy((char *)p->trxBinaryInfoPtr.cpu.filePath,
		ADRV903X_CPU_FW_FILE,
		sizeof(p->trxBinaryInfoPtr.cpu.filePath) - 1);
	strncpy((char *)p->trxBinaryInfoPtr.stream.filePath,
		ADRV903X_STREAM_FILE,
		sizeof(p->trxBinaryInfoPtr.stream.filePath) - 1);
	strncpy((char *)p->trxBinaryInfoPtr.rxGainTable[0].filePath,
		ADRV903X_RX_GAIN_TABLE_FILE,
		sizeof(p->trxBinaryInfoPtr.rxGainTable[0].filePath) - 1);
	p->trxBinaryInfoPtr.rxGainTable[0].channelMask = ADRV903X_RX_GAIN_TABLE_MASK;

	if (p->dev_clk) {
		ret = no_os_clk_enable(p->dev_clk);
		if (ret)
			goto error_free_errptr;
	}

	/* Open SPI bus and configure GPIO reset */
	ret = adi_adrv903x_HwOpen(p->palmaDevice, &p->spiSettings);
	if (ret) {
		pr_err("adrv903x: HwOpen failed (%d)\n", ret);
		goto error_disable_clk;
	}
	hw_open_ok = true;

	/* Pulse hardware reset */
	ret = adi_adrv903x_HwReset(p->palmaDevice);
	if (ret) {
		pr_err("adrv903x: HwReset failed (%d)\n", ret);
		goto error_hw_close;
	}

	/* Verify SPI communication via scratchpad loopback */
	ret = adi_adrv903x_SpiVerify(p->palmaDevice);
	if (ret) {
		pr_err("adrv903x: SpiVerify failed (%d)\n", ret);
		goto error_hw_close;
	}

	/* Read API version to confirm device is responding */
	ret = adi_adrv903x_ApiVersionGet(p->palmaDevice, &apiVersion);
	if (ret) {
		pr_err("adrv903x: ApiVersionGet failed (%d)\n", ret);
		goto error_hw_close;
	}

	pr_info("adrv903x-phy Rev %d, API version: %u.%u.%u.%u found\n",
		p->palmaDevice->devStateInfo.deviceSiRev,
		apiVersion.majorVer, apiVersion.minorVer,
		apiVersion.maintenanceVer, apiVersion.buildVer);

	/* Extract init structs from the embedded device profile binary */
	ret = adi_adrv903x_InitDataExtract(p->palmaDevice,
					   &p->trxBinaryInfoPtr.cpuProfile,
					   &apiVersion, &armVersion,
					   &streamVersion,
					   &p->deviceInitStruct,
					   &p->postMcsInitInst, &extractOutput);
	if (ret) {
		pr_err("adrv903x: InitDataExtract failed (%d)\n", ret);
		goto error_hw_close;
	}

	pr_info("adrv903x: FW %u.%u.%u.%u, stream %u.%u.%u.%u\n",
		armVersion.commVer.majorVer, armVersion.commVer.minorVer,
		armVersion.commVer.maintenanceVer, armVersion.commVer.buildVer,
		streamVersion.majorVer, streamVersion.minorVer,
		streamVersion.maintenanceVer, streamVersion.buildVer);

	/*
	 * PreMcsInit: load stream image, ARM CPU firmware, device profile and
	 * RX gain tables into the chip; start the ARM CPU.
	 */
	ret = adi_adrv903x_PreMcsInit(p->palmaDevice, &p->deviceInitStruct,
				      &p->trxBinaryInfoPtr);
	if (ret) {
		pr_err("adrv903x: PreMcsInit failed (%d)\n", ret);
		goto error_hw_close;
	}

	/* Non-broadcast portion of pre-MCS initialization */
	ret = adi_adrv903x_PreMcsInit_NonBroadcast(p->palmaDevice,
						    &p->deviceInitStruct);
	if (ret) {
		pr_err("adrv903x: PreMcsInit_NonBroadcast failed (%d)\n", ret);
		goto error_hw_close;
	}

	pr_info("adrv903x: firmware loaded, ARM CPU running\n");

	/*
	 * MCS (MultichipSyncSet_v2 START, SYSREF polling loop, OFF) and
	 * PostMcsInit are deferred to the JESD204 FSM callbacks:
	 *   LINK_SETUP      → MultichipSyncSet_v2(START), post_state_sysref
	 *   OPT_SETUP_STAGE1 → poll status + jesd204_sysref_async_force loop
	 *   OPT_SETUP_STAGE2 → MultichipSyncSet_v2(OFF) + PostMcsInit
	 * This ensures SYSREF reaches the FPGA JESD204 IP before LINK_ENABLE
	 * so adxcvr elastic buffer alignment succeeds on the first attempt.
	 */

	/* Register JESD204 device for FSM-driven link bring-up */
	ret = jesd204_dev_register(&p->jdev, &jesd204_adrv903x_init);
	if (ret) {
		pr_err("adrv903x: jesd204_dev_register failed (%d)\n", ret);
		goto error_hw_close;
	}

	priv = jesd204_dev_priv(p->jdev);
	priv->phy = p;

	p->is_initialized = true;
	*phy = p;
	return 0;

error_hw_close:
	if (hw_open_ok)
		adi_hal_HwClose(p->palmaDevice->common.devHalInfo);
error_disable_clk:
	if (p->dev_clk)
		no_os_clk_disable(p->dev_clk);
error_free_errptr:
	no_os_free(errPtr);
error_free_phy:
	no_os_free(p);
	return ret;
}

/**
 * @brief Free resources allocated by adrv903x_init().
 *
 * @param phy - Pointer to device structure.
 *
 * @return 0 on success, negative error code on failure.
 */
int adrv903x_remove(struct adrv903x_rf_phy *phy)
{
	if (!phy)
		return -EINVAL;

	jesd204_dev_unregister(phy->jdev);

	if (phy->is_initialized)
		adi_hal_HwClose(phy->palmaDevice->common.devHalInfo);

	if (phy->dev_clk)
		no_os_clk_disable(phy->dev_clk);

	no_os_free(phy->palmaDevice->common.errPtr);
	no_os_free(phy);
	return 0;
}
