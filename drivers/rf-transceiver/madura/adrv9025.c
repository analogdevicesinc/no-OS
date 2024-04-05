// SPDX-License-Identifier: GPL-2.0
/*
 * ADRV9025/6 RF Transceiver
 *
 * Copyright 2020-2023 Analog Devices Inc.
 *
 * Licensed under the GPL-2.
 */

#include "xilinx_transceiver.h"
#include "no_os_print_log.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_util.h"
#include "no_os_spi.h"
#include "adrv9025.h"
#include "jesd204.h"
#include <stdbool.h>
#include <string.h>

static int __adrv9025_dev_err(struct adrv9025_rf_phy *phy, const char *function,
			      const int line)
{
	int ret;

	pr_err("%s, %d: failed with %s (%d)\n", function, line,
	       phy->madDevice->common.error.errormessage ?
	       phy->madDevice->common.error.errormessage :
	       "",
	       phy->madDevice->common.error.errCode);

	switch (phy->madDevice->common.error.errCode) {
	case ADI_COMMON_ERR_INV_PARAM:
	case ADI_COMMON_ERR_NULL_PARAM:
		ret = -EINVAL;
		break;
	case ADI_COMMON_ERR_API_FAIL:
		ret = -EFAULT;
		break;
	case ADI_COMMON_ERR_SPI_FAIL:
		ret = -EIO;
		break;
	case ADI_COMMON_ERR_OK:
		ret = 0;
		break;
	default:
		ret = -EFAULT;
	}

	adrv9025_ErrorClear(&phy->madDevice->common);

	return ret;
}

#define adrv9025_dev_err(phy) __adrv9025_dev_err(phy, __func__, __LINE__)

int adrv9025_spi_read(struct no_os_spi_desc *spi, unsigned int reg)
{
	uint8_t buf[3];
	int ret;

	buf[0] = 0x80 | (reg >> 8);
	buf[1] = reg & 0xFF;
	ret = no_os_spi_write_and_read(spi, &buf[0], 2);

	pr_debug("%s: REG: 0x%X VAL: 0x%X (%d)\n", __func__, reg,
		 buf[0], ret);

	if (ret < 0) {
		pr_err("%s: failed (%d)\n", __func__, ret);
		return ret;
	}

	return buf[2];
}

int adrv9025_spi_write(struct no_os_spi_desc *spi, unsigned int reg,
		       unsigned int val)
{
	unsigned char buf[3];
	int ret;

	buf[0] = reg >> 8;
	buf[1] = reg & 0xFF;
	buf[2] = val;

	ret = no_os_spi_write_and_read(spi, buf, 3);
	if (ret < 0) {
		pr_err("%s: failed (%d)\n", __func__, ret);
		return ret;
	}

	pr_debug("%s: REG: 0x%X VAL: 0x%X (%d)\n", __func__, reg, val,
		 ret);

	return 0;
}

int adrv9025_RxLinkSamplingRateFind(adi_adrv9025_Device_t *device,
				    adi_adrv9025_Init_t *adrv9025Init,
				    adi_adrv9025_FramerSel_e framerSel,
				    uint32_t *iqRate_kHz)
{
	int recoveryAction = ADI_COMMON_ACT_NO_ACTION;
	adi_adrv9025_AdcSampleXbarSel_e conv = ADI_ADRV9025_ADC_RX1_Q;
	uint32_t framerIndex = 0;

	/* Check device pointer is not null */
	ADI_NULL_DEVICE_PTR_RETURN(device);
	ADI_NULL_PTR_RETURN(&device->common, iqRate_kHz);

	ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

	switch (framerSel) {
	case ADI_ADRV9025_FRAMER_0:
		framerIndex = 0;
		break;
	case ADI_ADRV9025_FRAMER_1:
		framerIndex = 1;
		break;
	case ADI_ADRV9025_FRAMER_2:
		framerIndex = 2;
		break;
	default:
		ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API,
				 ADI_COMMON_ERR_INV_PARAM,
				 ADI_COMMON_ACT_ERR_CHECK_PARAM, framerSel,
				 "Only one framer can be selected at a time.");
		ADI_ERROR_RETURN(device->common.error.newAction);
		break;
	}

	if (adrv9025Init->dataInterface.framer[framerIndex].jesd204M < 1) {
		*iqRate_kHz = 0;
		return recoveryAction;
	}

	conv = adrv9025Init->dataInterface.framer[framerIndex]
	       .adcCrossbar.conv0;

	switch (conv) {
	case ADI_ADRV9025_ADC_RX1_I: /* fall through */
	case ADI_ADRV9025_ADC_RX1_Q: /* fall through */
	case ADI_ADRV9025_ADC_DUALBAND_RX1_BAND_B_I: /* fall through */
	case ADI_ADRV9025_ADC_DUALBAND_RX1_BAND_B_Q: /* fall through */
		*iqRate_kHz = adrv9025Init->rx.rxChannelCfg[0]
			      .profile.rxOutputRate_kHz;
		break;
	case ADI_ADRV9025_ADC_RX2_I: /* fall through */
	case ADI_ADRV9025_ADC_RX2_Q: /* fall through */
	case ADI_ADRV9025_ADC_DUALBAND_RX2_BAND_B_I: /* fall through */
	case ADI_ADRV9025_ADC_DUALBAND_RX2_BAND_B_Q: /* fall through */
		*iqRate_kHz = adrv9025Init->rx.rxChannelCfg[1]
			      .profile.rxOutputRate_kHz;
		break;
	case ADI_ADRV9025_ADC_RX3_I: /* fall through */
	case ADI_ADRV9025_ADC_RX3_Q: /* fall through */
	case ADI_ADRV9025_ADC_DUALBAND_RX3_BAND_B_I: /* fall through */
	case ADI_ADRV9025_ADC_DUALBAND_RX3_BAND_B_Q: /* fall through */
		*iqRate_kHz = adrv9025Init->rx.rxChannelCfg[2]
			      .profile.rxOutputRate_kHz;
		break;
	case ADI_ADRV9025_ADC_RX4_I: /* fall through */
	case ADI_ADRV9025_ADC_RX4_Q: /* fall through */
	case ADI_ADRV9025_ADC_DUALBAND_RX4_BAND_B_I: /* fall through */
	case ADI_ADRV9025_ADC_DUALBAND_RX4_BAND_B_Q: /* fall through */
		*iqRate_kHz = adrv9025Init->rx.rxChannelCfg[3]
			      .profile.rxOutputRate_kHz;
		break;
	// NOTE: ORx2/4 profiles never referenced, ORx2IQ enum below refers to digital channel not RF input
	// RF ORx1/2 share digital ORX1, and RF ORX3/4 share digital ORX2
	case ADI_ADRV9025_ADC_ORX1_I: /* fall through */
	case ADI_ADRV9025_ADC_ORX1_Q: /* fall through */
		*iqRate_kHz = adrv9025Init->rx.rxChannelCfg[4]
			      .profile.rxOutputRate_kHz;
		break;
	// NOTE: ORx2/4 profiles never referenced, ORx2IQ enum below refers to digital channel not RF input
	// RF ORx1/2 share digital ORX1, and RF ORX3/4 share digital ORX2
	case ADI_ADRV9025_ADC_ORX2_I: /* fall through */
	case ADI_ADRV9025_ADC_ORX2_Q: /* fall through */
		*iqRate_kHz = adrv9025Init->rx.rxChannelCfg[6]
			      .profile.rxOutputRate_kHz;
		break;
	default:
		*iqRate_kHz = 0;
		ADI_ERROR_REPORT(
			&device->common, ADI_COMMON_ERRSRC_API,
			ADI_COMMON_ERR_INV_PARAM,
			ADI_COMMON_ACT_ERR_CHECK_PARAM, adcCrossbar.conv0,
			"Invalid ADC crossbar used to read iqRate_kHz");
		ADI_ERROR_RETURN(device->common.error.newAction);
	}

	return recoveryAction;
}

int adrv9025_TxLinkSamplingRateFind(adi_adrv9025_Device_t *device,
				    adi_adrv9025_Init_t *adrv9025Init,
				    adi_adrv9025_DeframerSel_e deframerSel,
				    uint32_t *iqRate_kHz)
{
	int recoveryAction = ADI_COMMON_ACT_NO_ACTION;
	uint32_t deframerIndex = 0;

	/* Check device pointer is not null */
	ADI_NULL_DEVICE_PTR_RETURN(device);
	ADI_NULL_PTR_RETURN(&device->common, iqRate_kHz);

	ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

	switch (deframerSel) {
	case ADI_ADRV9025_DEFRAMER_0:
		deframerIndex = 0;
		break;
	case ADI_ADRV9025_DEFRAMER_1:
		deframerIndex = 1;
		break;
	default:
		ADI_ERROR_REPORT(
			&device->common, ADI_COMMON_ERRSRC_API,
			ADI_COMMON_ERR_INV_PARAM,
			ADI_COMMON_ACT_ERR_CHECK_PARAM, deframerSel,
			"Only one deframer can be selected at a time.");
		ADI_ERROR_RETURN(device->common.error.newAction);
		break;
	}

	if (adrv9025Init->dataInterface.deframer[deframerIndex].jesd204M < 1) {
		*iqRate_kHz = 0;
		return recoveryAction;
	}

	//Use samplerate of DAC set to use deframer output 0.
	if ((adrv9025Init->dataInterface.deframer[deframerIndex]
	     .dacCrossbar.tx1DacChanI == ADI_ADRV9025_DEFRAMER_OUT0) ||
	    (adrv9025Init->dataInterface.deframer[deframerIndex]
	     .dacCrossbar.tx1DacChanQ == ADI_ADRV9025_DEFRAMER_OUT0)) {
		*iqRate_kHz = adrv9025Init->tx.txChannelCfg[0]
			      .profile.txInputRate_kHz;
	} else if ((adrv9025Init->dataInterface.deframer[deframerIndex]
		    .dacCrossbar.tx2DacChanI ==
		    ADI_ADRV9025_DEFRAMER_OUT0) ||
		   (adrv9025Init->dataInterface.deframer[deframerIndex]
		    .dacCrossbar.tx2DacChanQ ==
		    ADI_ADRV9025_DEFRAMER_OUT0)) {
		*iqRate_kHz = adrv9025Init->tx.txChannelCfg[1]
			      .profile.txInputRate_kHz;
	} else if ((adrv9025Init->dataInterface.deframer[deframerIndex]
		    .dacCrossbar.tx3DacChanI ==
		    ADI_ADRV9025_DEFRAMER_OUT0) ||
		   (adrv9025Init->dataInterface.deframer[deframerIndex]
		    .dacCrossbar.tx3DacChanQ ==
		    ADI_ADRV9025_DEFRAMER_OUT0)) {
		*iqRate_kHz = adrv9025Init->tx.txChannelCfg[2]
			      .profile.txInputRate_kHz;
	} else if ((adrv9025Init->dataInterface.deframer[deframerIndex]
		    .dacCrossbar.tx4DacChanI ==
		    ADI_ADRV9025_DEFRAMER_OUT0) ||
		   (adrv9025Init->dataInterface.deframer[deframerIndex]
		    .dacCrossbar.tx4DacChanQ ==
		    ADI_ADRV9025_DEFRAMER_OUT0)) {
		*iqRate_kHz = adrv9025Init->tx.txChannelCfg[3]
			      .profile.txInputRate_kHz;
	}

	return recoveryAction;
}

static void adrv9025_shutdown(struct adrv9025_rf_phy *phy)
{
	/***********************************************
	 * Shutdown Procedure *
	 * **********************************************/
	/* Function to turn radio on, Disables transmitters and receivers */

	adi_adrv9025_Shutdown(phy->madDevice);
	adi_adrv9025_HwClose(phy->madDevice);

	memset(&phy->adi_adrv9025_device.devStateInfo, 0,
	       sizeof(phy->adi_adrv9025_device.devStateInfo));
}

#define ADRV9025_MAX_CLK_NAME 79

static void adrv9025_info(struct adrv9025_rf_phy *phy)
{
	adi_adrv9025_ApiVersion_t apiVersion;
	adi_adrv9025_ArmVersion_t armVersion;
	adi_adrv9025_StreamVersion_t streamVersion;

	adi_adrv9025_ApiVersionGet(phy->madDevice, &apiVersion);
	adi_adrv9025_ArmVersionGet(phy->madDevice, &armVersion);
	adi_adrv9025_StreamVersionGet(phy->madDevice, &streamVersion);

	pr_info(
		"adrv9025-phy Rev %d, Firmware %u.%u.%u.%u API version: %u.%u.%u.%u Stream version: %u.%u.%u.%u successfully initialized%s",
		phy->madDevice->devStateInfo.deviceSiRev, armVersion.majorVer,
		armVersion.minorVer, armVersion.maintVer, armVersion.rcVer,
		apiVersion.majorVer, apiVersion.minorVer,
		apiVersion.maintenanceVer, apiVersion.buildVer,
		streamVersion.majorVer, streamVersion.minorVer,
		streamVersion.mainVer, streamVersion.buildVer,
		phy->jdev ? " via jesd204-fsm\n" : "\n");
}

struct adrv9025_jesd204_link {
	unsigned int source_id;
	bool is_framer;
};

struct adrv9025_jesd204_priv {
	struct adrv9025_rf_phy *phy;
	struct adrv9025_jesd204_link link[5];
};

int adrv9025_jesd204_link_pre_setup(struct jesd204_dev *jdev,
				    enum jesd204_state_op_reason reason)
{
	struct adrv9025_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9025_rf_phy *phy = priv->phy;
	uint64_t dev_clk;
	int ret;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	switch (reason) {
	case JESD204_STATE_OP_REASON_INIT:
		break;
	default:
		return JESD204_STATE_CHANGE_DONE;
	}

	ret = no_os_clk_round_rate(phy->dev_clk,
				   phy->deviceInitStruct.clocks.deviceClock_kHz * 1000, &dev_clk);
	if (ret)
		return JESD204_STATE_CHANGE_ERROR;

	if (dev_clk > 0 && ((dev_clk / 1000) ==
			    phy->deviceInitStruct.clocks.deviceClock_kHz)) {
		no_os_clk_set_rate(phy->dev_clk, dev_clk);
	} else {
		pr_err("Requesting device clock %u failed got %ld",
		       phy->deviceInitStruct.clocks.deviceClock_kHz * 1000, dev_clk);
		return -EINVAL;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9025_jesd204_link_init(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason,
				      struct jesd204_link *lnk)
{
	struct adrv9025_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9025_rf_phy *phy = priv->phy;
	adi_adrv9025_FrmCfg_t *framer = NULL;
	adi_adrv9025_DfrmCfg_t *deframer = NULL;
	uint32_t rate;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	switch (reason) {
	case JESD204_STATE_OP_REASON_INIT:
		break;
	default:
		return JESD204_STATE_CHANGE_DONE;
	}

	switch (lnk->link_id) {
	case DEFRAMER0_LINK_TX:
		deframer = &phy->deviceInitStruct.dataInterface.deframer[0];
		priv->link[lnk->link_id].source_id = ADI_ADRV9025_DEFRAMER_0;
		ret = adrv9025_TxLinkSamplingRateFind(phy->madDevice, &phy->deviceInitStruct,
						      ADI_ADRV9025_DEFRAMER_0,
						      &rate);
		phy->tx_iqRate_kHz  = rate;
		break;
	case DEFRAMER1_LINK_TX:
		deframer = &phy->deviceInitStruct.dataInterface.deframer[1];
		priv->link[lnk->link_id].source_id = ADI_ADRV9025_DEFRAMER_1;
		ret = adrv9025_TxLinkSamplingRateFind(phy->madDevice, &phy->deviceInitStruct,
						      ADI_ADRV9025_DEFRAMER_1,
						      &rate);
		break;
	case FRAMER0_LINK_RX:
		framer = &phy->deviceInitStruct.dataInterface.framer[0];
		priv->link[lnk->link_id].source_id = ADI_ADRV9025_FRAMER_0;
		priv->link[lnk->link_id].is_framer = true;
		ret = adrv9025_RxLinkSamplingRateFind(phy->madDevice, &phy->deviceInitStruct,
						      ADI_ADRV9025_FRAMER_0,
						      &rate);
		phy->rx_iqRate_kHz = rate;
		break;
	case FRAMER1_LINK_RX:
		framer = &phy->deviceInitStruct.dataInterface.framer[1];
		priv->link[lnk->link_id].source_id = ADI_ADRV9025_FRAMER_1;
		priv->link[lnk->link_id].is_framer = true;
		ret = adrv9025_RxLinkSamplingRateFind(phy->madDevice, &phy->deviceInitStruct,
						      ADI_ADRV9025_FRAMER_1,
						      &rate);
		break;
	case FRAMER2_LINK_RX:
		framer = &phy->deviceInitStruct.dataInterface.framer[2];
		priv->link[lnk->link_id].source_id = ADI_ADRV9025_FRAMER_2;
		priv->link[lnk->link_id].is_framer = true;
		ret = adrv9025_RxLinkSamplingRateFind(phy->madDevice, &phy->deviceInitStruct,
						      ADI_ADRV9025_FRAMER_2,
						      &rate);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return adrv9025_dev_err(phy);

	lnk->sample_rate = rate * 1000;

	if (framer) {
		lnk->num_converters = framer->jesd204M;
		lnk->num_lanes = no_os_hweight8(framer->serializerLanesEnabled);
		lnk->octets_per_frame = framer->jesd204F;
		lnk->frames_per_multiframe = framer->jesd204K;
		lnk->device_id = framer->deviceId;
		lnk->bank_id = framer->bankId;
		lnk->scrambling = framer->scramble;
		lnk->bits_per_sample = framer->jesd204Np;
		lnk->converter_resolution = framer->jesd204Np;
		lnk->ctrl_bits_per_sample = 0;
		lnk->jesd_version = framer->enableJesd204C ? JESD204_VERSION_C :
				    JESD204_VERSION_B;
		lnk->subclass = JESD204_SUBCLASS_1;
		lnk->is_transmit = false;
	} else if (deframer) {
		lnk->num_converters = deframer->jesd204M;
		lnk->num_lanes = no_os_hweight8(deframer->deserializerLanesEnabled);
		lnk->octets_per_frame = deframer->jesd204F;
		lnk->frames_per_multiframe = deframer->jesd204K;
		lnk->device_id = deframer->deviceId;
		lnk->bank_id = deframer->bankId;
		lnk->scrambling = deframer->scramble;
		lnk->bits_per_sample = deframer->jesd204Np;
		lnk->converter_resolution = deframer->jesd204Np;
		lnk->ctrl_bits_per_sample = 0;
		lnk->jesd_version = deframer->enableJesd204C ? JESD204_VERSION_C :
				    JESD204_VERSION_B;
		lnk->subclass = JESD204_SUBCLASS_1;
		lnk->is_transmit = true;
	};

	return JESD204_STATE_CHANGE_DONE;
}

int adrv9025_jesd204_link_setup(struct jesd204_dev *jdev,
				enum jesd204_state_op_reason reason)
{
	struct adrv9025_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9025_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));


	if (reason == JESD204_STATE_OP_REASON_UNINIT) {
		phy->is_initialized = 0;

		adi_adrv9025_Shutdown(phy->madDevice);
		adi_adrv9025_HwClose(phy->madDevice);

		memset(&phy->adi_adrv9025_device.devStateInfo, 0,
		       sizeof(phy->adi_adrv9025_device.devStateInfo));

		return JESD204_STATE_CHANGE_DONE;
	}

	memset(&phy->adi_adrv9025_device.devStateInfo, 0,
	       sizeof(phy->adi_adrv9025_device.devStateInfo));

	ret = adi_adrv9025_HwOpen(phy->madDevice, &phy->spiSettings);
	if (ret)
		return adrv9025_dev_err(phy);

	adi_common_LogLevelSet(&phy->madDevice->common,
			       ADI_HAL_LOG_ERR | ADI_HAL_LOG_WARN);

	/* Pre MCS - Broadcastable */
	ret = adi_adrv9025_PreMcsInit_v2(phy->madDevice, &phy->deviceInitStruct,
					 phy->platformFiles.armImageFile,
					 phy->platformFiles.streamImageFile,
					 phy->platformFiles.rxGainTableFileArr,
					 phy->platformFiles.rxGainTableFileArrSize,
					 phy->platformFiles.txAttenTableFileArr,
					 phy->platformFiles.txAttenTableFileArrSize);
	if (ret)
		return adrv9025_dev_err(phy);

	/* Pre MCS - Non-Broadcastable */
	ret = adi_adrv9025_PreMcsInit_NonBroadCast(phy->madDevice,
			&phy->deviceInitStruct);
	if (ret)
		return adrv9025_dev_err(phy);

	/* MCS start sequence*/
	ret = adi_adrv9025_MultichipSyncSet(phy->madDevice, ADI_ENABLE);
	if (ret)
		return adrv9025_dev_err(phy);

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9025_jesd204_setup_stage1(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason)
{
	struct adrv9025_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9025_rf_phy *phy = priv->phy;
	int ret, i;
	uint32_t mcsStatus;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	/* This loop will send SysRef pulses up to 255 times unless MCS status achieved before. */
	for (i = 0; i < 255; i++) {
		ret = adi_adrv9025_MultichipSyncStatusGet(phy->madDevice,
				&mcsStatus);
		if (ret)
			return adrv9025_dev_err(phy);

		if ((mcsStatus & 0x17) == 0x17)
			break;

		jesd204_sysref_async_force(phy->jdev);
	}

	if (mcsStatus != 0x17) {
		pr_err("%s:%d Unexpected MCS sync status (0x%X)",
		       __func__, __LINE__, mcsStatus);

		return adrv9025_dev_err(phy);
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9025_jesd204_setup_stage2(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason)
{
	struct adrv9025_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9025_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	/* MCS end sequence*/
	ret = adi_adrv9025_MultichipSyncSet(phy->madDevice, ADI_DISABLE);
	if (ret)
		return adrv9025_dev_err(phy);

	/* Post MCS */
	ret = adi_adrv9025_PostMcsInit(phy->madDevice,
				       &phy->adrv9025PostMcsInitInst);
	if (ret)
		return adrv9025_dev_err(phy);

	ret = adi_adrv9025_SerializerReset(
		      phy->madDevice, phy->deviceInitStruct.clocks.serdesPllVcoFreq_kHz);
	if (ret)
		return adrv9025_dev_err(phy);

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9025_jesd204_clks_enable(struct jesd204_dev *jdev,
					enum jesd204_state_op_reason reason,
					struct jesd204_link *lnk)
{
	struct adrv9025_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9025_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	if (!lnk->num_converters)
		return JESD204_STATE_CHANGE_DONE;

	if (priv->link[lnk->link_id].is_framer) {

		if (phy->madDevice->devStateInfo.linkSharingEnabled == 1) {
			ret = adi_adrv9025_FramerSysrefCtrlSet(phy->madDevice,
							       ADI_ADRV9025_FRAMER_1, 0);
			if (ret)
				return adrv9025_dev_err(phy);

			ret = adi_adrv9025_FramerLinkStateSet(phy->madDevice,
							      ADI_ADRV9025_FRAMER_1, 0);
			if (ret)
				return adrv9025_dev_err(phy);

			ret = adi_adrv9025_FramerLinkStateSet(phy->madDevice,
							      ADI_ADRV9025_FRAMER_1, 1);
			if (ret)
				return adrv9025_dev_err(phy);

			pr_debug("%s:%d Link %d Framer enabled", __func__, __LINE__,
				 ADI_ADRV9025_FRAMER_1);

			/*************************************************/
			/**** Enable SYSREF to Talise JESD204B Framer ***/
			/*************************************************/
			/*** < User: Make sure SYSREF is stopped/disabled > ***/
			ret = adi_adrv9025_FramerSysrefCtrlSet(phy->madDevice,
							       ADI_ADRV9025_FRAMER_1, 1);
			if (ret)
				return adrv9025_dev_err(phy);

			jesd204_sysref_async_force(phy->jdev);

			ret = adi_adrv9025_FramerLinkStateSet(phy->madDevice,
							      ADI_ADRV9025_FRAMER_1, 0);
			if (ret)
				return adrv9025_dev_err(phy);

			ret = adi_adrv9025_FramerSysrefCtrlSet(phy->madDevice,
							       ADI_ADRV9025_FRAMER_1, 0);
			if (ret)
				return adrv9025_dev_err(phy);

		}

		ret = adi_adrv9025_FramerSysrefCtrlSet(phy->madDevice,
						       priv->link[lnk->link_id].source_id, 0);
		if (ret)
			return adrv9025_dev_err(phy);

		ret = adi_adrv9025_FramerLinkStateSet(phy->madDevice,
						      priv->link[lnk->link_id].source_id, 0);
		if (ret)
			return adrv9025_dev_err(phy);


		ret = adi_adrv9025_FramerLinkStateSet(phy->madDevice,
						      priv->link[lnk->link_id].source_id, 1);
		if (ret)
			return adrv9025_dev_err(phy);


		pr_debug("%s:%d Link %d Framer enabled", __func__, __LINE__,
			 priv->link[lnk->link_id].source_id);

		/*************************************************/
		/**** Enable SYSREF to Talise JESD204B Framer ***/
		/*************************************************/
		/*** < User: Make sure SYSREF is stopped/disabled > ***/
		ret = adi_adrv9025_FramerSysrefCtrlSet(phy->madDevice,
						       priv->link[lnk->link_id].source_id, 1);
		if (ret)
			return adrv9025_dev_err(phy);

	} else {
		ret = adi_adrv9025_DeframerSysrefCtrlSet(phy->madDevice,
				priv->link[lnk->link_id].source_id, 0);
		if (ret)
			return adrv9025_dev_err(phy);


		ret = adi_adrv9025_DfrmLinkStateSet(phy->madDevice,
						    priv->link[lnk->link_id].source_id, 0);
		if (ret)
			return adrv9025_dev_err(phy);

	};

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9025_jesd204_link_enable(struct jesd204_dev *jdev,
					enum jesd204_state_op_reason reason,
					struct jesd204_link *lnk)
{
	struct adrv9025_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9025_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	if (!lnk->num_converters)
		return JESD204_STATE_CHANGE_DONE;

	if (!priv->link[lnk->link_id].is_framer) { /* DEFRAMER */
		uint8_t errFlags = 0;
		adi_adrv9025_InitCals_t serdesCal = {
			.calMask = ADI_ADRV9025_SERDES_INIT,
			.channelMask = 0xF, /* CAL_ALL_CHANNELS */
			.warmBoot = 0,
		};

		ret = adi_adrv9025_DfrmLinkStateSet(phy->madDevice,
						    priv->link[lnk->link_id].source_id, 1);
		if (ret)
			return adrv9025_dev_err(phy);

		/* Notify ARM to run SERDES Calbriation if necessary */
		ret = adi_adrv9025_InitCalsRun(phy->madDevice, &serdesCal);
		if (ret)
			return adrv9025_dev_err(phy);

		/* Wait up to 60 seconds for ARM */
		ret = adi_adrv9025_InitCalsWait(phy->madDevice, 60000, &errFlags);
		if (ret) {
			pr_err("Error: InitCalsWait 0x%X\n", errFlags);
			return adrv9025_dev_err(phy);
		}

		/***************************************************/
		/**** Enable SYSREF to Talise JESD204B Deframer ***/
		/***************************************************/
		ret = adi_adrv9025_DeframerSysrefCtrlSet(phy->madDevice,
				priv->link[lnk->link_id].source_id, 1);
		if (ret)
			return adrv9025_dev_err(phy);

	};

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv9025_jesd204_link_running(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct adrv9025_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9025_rf_phy *phy = priv->phy;
	int ret;

	adi_adrv9025_FramerStatus_t framerStatus;
	adi_adrv9025_DeframerStatus_t deframerStatus;
	uint8_t deframerLinkCondition = 0;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	if (!lnk->num_converters)
		return JESD204_STATE_CHANGE_DONE;

	if (priv->link[lnk->link_id].is_framer) {
		ret = adi_adrv9025_FramerStatusGet(phy->madDevice,
						   priv->link[lnk->link_id].source_id, &framerStatus);
		if (ret)
			return adrv9025_dev_err(phy);


		if ((framerStatus.status & 0x0F) != 0x0A)
			pr_warning("Link%u framerStatus 0x%X\n",
				   lnk->link_id, framerStatus.status);
	} else {
		ret = adi_adrv9025_DeframerStatusGet(phy->madDevice,
						     priv->link[lnk->link_id].source_id, &deframerStatus);
		if (ret)
			return adrv9025_dev_err(phy);

		ret  = adi_adrv9025_DfrmLinkConditionGet(
			       phy->madDevice,
			       priv->link[lnk->link_id].source_id,
			       &deframerLinkCondition);

		if ((deframerStatus.status & 0x7F) != 0x7) /* Ignore Valid ILAS checksum */
			pr_warning("Link%u deframerStatus 0x%X\n",
				   lnk->link_id, deframerStatus.status);

		/* Kick off SERDES tracking cal if lanes are up */
		ret = adi_adrv9025_TrackingCalsEnableSet(
			      phy->madDevice, ADI_ADRV9025_TRACK_DESERIALIZER,
			      ADI_ADRV9025_TRACKING_CAL_ENABLE);
		if (ret)
			return adrv9025_dev_err(phy);
	};


	return JESD204_STATE_CHANGE_DONE;
}


static int adrv9025_jesd204_post_running_stage(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason)
{
	struct adrv9025_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv9025_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT) {
		phy->is_initialized = 0;
		return JESD204_STATE_CHANGE_DONE;
	}

	/* Initialize Tx Ramp down functionality */
	ret = adi_adrv9025_TxRampDownInit(phy->madDevice, &phy->deviceInitStruct);
	if (ret)
		return adrv9025_dev_err(phy);

	/* Setup GP Interrupts from init structure */
	ret = adi_adrv9025_GpIntInit(phy->madDevice,
				     &phy->deviceInitStruct.gpInterrupts);
	if (ret)
		return adrv9025_dev_err(phy);

	no_os_clk_set_rate(phy->clks[ADRV9025_RX_SAMPL_CLK], phy->rx_iqRate_kHz * 1000);
	no_os_clk_set_rate(phy->clks[ADRV9025_TX_SAMPL_CLK], phy->tx_iqRate_kHz * 1000);

	ret = adi_adrv9025_RxTxEnableSet(phy->madDevice, 0xF, ADI_ADRV9025_TXALL);
	if (ret)
		return adrv9025_dev_err(phy);

	phy->is_initialized = 1;
	adrv9025_info(phy);

	return JESD204_STATE_CHANGE_DONE;
}

static const struct jesd204_dev_data jesd204_adrv9025_init = {
	.state_ops = {
		[JESD204_OP_LINK_INIT] = {
			.per_link = adrv9025_jesd204_link_init,
		},
		[JESD204_OP_LINK_PRE_SETUP] = {
			.per_device = adrv9025_jesd204_link_pre_setup,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_OPT_SETUP_STAGE1] = {
			.per_device = adrv9025_jesd204_setup_stage1,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_OPT_SETUP_STAGE2] = {
			.per_device = adrv9025_jesd204_setup_stage2,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_CLOCKS_ENABLE] = {
			.per_link = adrv9025_jesd204_clks_enable,
		},
		[JESD204_OP_LINK_SETUP] = {
			.per_device = adrv9025_jesd204_link_setup,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_LINK_ENABLE] = {
			.per_link = adrv9025_jesd204_link_enable,
			.post_state_sysref = true,
		},
		[JESD204_OP_LINK_RUNNING] = {
			.per_link = adrv9025_jesd204_link_running,
		},
		[JESD204_OP_OPT_POST_RUNNING_STAGE] = {
			.per_device = adrv9025_jesd204_post_running_stage,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
	},

	.max_num_links = 5,
	.sizeof_priv = sizeof(struct adrv9025_jesd204_priv),
};

/**
 * Initialize the device.
 * @param dev - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adrv9025_init(struct adrv9025_rf_phy **dev,
		      const struct adrv9025_init_param *init_param)
{
	adi_adrv9025_SpiSettings_t *spi_settings;
	struct adrv9025_jesd204_priv *priv;
	struct adrv9025_rf_phy *phy;
	int ret;

	phy = (struct adrv9025_rf_phy *)no_os_calloc(1, sizeof(*phy));
	if (!phy)
		goto error;

	phy->madDevice = init_param->adrv9025_device;
	phy->spi_device_id = 0;
	phy->device_id = 0;
	phy->dev_clk = init_param->dev_clk;

	spi_settings = adrv9025_spi_settings_get();
	phy->spiSettings.msbFirst = spi_settings->msbFirst;
	phy->spiSettings.enSpiStreaming = spi_settings->enSpiStreaming;
	phy->spiSettings.autoIncAddrUp = spi_settings->autoIncAddrUp;
	phy->spiSettings.fourWireMode = spi_settings->fourWireMode;
	phy->spiSettings.cmosPadDrvStrength = spi_settings->cmosPadDrvStrength;

	strncpy(phy->platformFiles.streamImageFile, init_param->streamImageFile,
		sizeof(phy->platformFiles.streamImageFile));

	ret = adrv9025_setup(phy);
	if (ret < 0) {
		pr_err("%s: adrv9025_setup failed (%d)\n", __func__, ret);
		goto error_setup;
	}

	ret = jesd204_dev_register(&phy->jdev, &jesd204_adrv9025_init);
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

static int32_t adrv9025_bb_recalc_rate(struct no_os_clk_desc *desc,
				       uint64_t *rate)
{
	struct adrv9025_rf_phy *adrv9025_dev;

	adrv9025_dev = desc->dev_desc;

	if (!strcmp(desc->name, "-rx_sampl_clk"))
		*rate = adrv9025_dev->rx_iqRate_kHz * 1000;
	else if (!strcmp(desc->name, "-tx_sampl_clk"))
		*rate = adrv9025_dev->tx_iqRate_kHz * 1000;
	else
		return -EINVAL;

	return 0;
}

static int32_t adrv9025_bb_set_rate(struct no_os_clk_desc *desc,
				    uint64_t rate)
{
	// Do nothing
	return 0;
}

static int32_t adrv9025_bb_round_rate(struct no_os_clk_desc *desc,
				      uint64_t rate,
				      uint64_t *rounded_rate)
{
	pr_debug("%s: Rate %lu Hz", __func__, rate);

	*rounded_rate = rate;

	return 0;
}

static const struct no_os_clk_platform_ops adrv9025_bb_clk_ops = {
	.clk_round_rate = &adrv9025_bb_round_rate,
	.clk_set_rate = &adrv9025_bb_set_rate,
	.clk_recalc_rate = &adrv9025_bb_recalc_rate,
};

int adrv9025_setup(struct adrv9025_rf_phy *phy)
{
	const char *names[NUM_ADRV9025_CLKS] = {
		"-rx_sampl_clk", "-tx_sampl_clk"
	};
	struct no_os_clk_desc *rx_sample_clk = NULL;
	struct no_os_clk_desc *tx_sample_clk = NULL;
	struct no_os_clk_init_param clk_init;
	adi_adrv9025_ApiVersion_t apiVersion;
	int ret, i;

	phy->madDevice = &phy->adi_adrv9025_device;
	phy->madDevice->common.devHalInfo = &phy->hal;

	adrv9025_ErrorClear(&phy->madDevice->common);

	strncpy(phy->platformFiles.armImageFile,
		"ADRV9025_FW.bin;ADRV9025_DPDCORE_FW.bin",
		sizeof(phy->platformFiles.armImageFile));

	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->platformFiles.rxGainTableFileArr); i++) {
		strncpy(phy->platformFiles.rxGainTableFileArr[i].rxGainTableCsvFileName,
			"ADRV9025_RxGainTable.h",
			sizeof(phy->platformFiles.rxGainTableFileArr[0].rxGainTableCsvFileName));
		phy->platformFiles.rxGainTableFileArr[i].rxChannelMask = 0xFF;
		phy->platformFiles.rxGainTableFileArrSize++;
	}

	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->platformFiles.txAttenTableFileArr); i++) {
		strncpy(phy->platformFiles.txAttenTableFileArr[i].txAttenTableCsvFileName,
			"ADRV9025_TxAttenTable.h",
			sizeof(phy->platformFiles.txAttenTableFileArr[0].txAttenTableCsvFileName));
		phy->platformFiles.txAttenTableFileArr[i].txChannelMask = 0x0F;
		phy->platformFiles.txAttenTableFileArrSize++;
	}

	ret = no_os_clk_enable(phy->dev_clk);
	if (ret)
		return ret;

	adi_common_LogLevelSet(&phy->madDevice->common,
			       ADI_HAL_LOG_ALL);

	ret = adi_adrv9025_HwOpen(phy->madDevice, adrv9025_spi_settings_get());
	if (ret)
		return adrv9025_dev_err(phy);

	ret = adi_adrv9025_ConfigFileLoad(phy->madDevice, "ActiveUseCase.profile",
					  &phy->deviceInitStruct);
	if (ret)
		return adrv9025_dev_err(phy);

	ret = adi_adrv9025_UtilityInitFileLoad(phy->madDevice, "ActiveUtilInit.profile",
					       &phy->adrv9025PostMcsInitInst);
	if (ret)
		return adrv9025_dev_err(phy);

	rx_sample_clk = no_os_calloc(1, sizeof(rx_sample_clk));
	if (!rx_sample_clk)
		goto rx_out_clk_error;

	/* Initialize clk component */
	clk_init.name = names[ADRV9025_RX_SAMPL_CLK];
	clk_init.hw_ch_num = 1;
	clk_init.platform_ops = &adrv9025_bb_clk_ops;
	clk_init.dev_desc = phy;

	ret = no_os_clk_init(&rx_sample_clk, &clk_init);
	if (ret)
		goto rx_out_clk_init_error;

	phy->clks[ADRV9025_RX_SAMPL_CLK] = rx_sample_clk;

	tx_sample_clk = no_os_calloc(1, sizeof(tx_sample_clk));
	if (!tx_sample_clk)
		goto rx_out_clk_init_error;

	/* Initialize clk component */
	clk_init.name = names[ADRV9025_TX_SAMPL_CLK];
	clk_init.hw_ch_num = 1;
	clk_init.platform_ops = &adrv9025_bb_clk_ops;
	clk_init.dev_desc = phy;

	ret = no_os_clk_init(&tx_sample_clk, &clk_init);
	if (ret)
		goto tx_out_clk_init_error;

	phy->clks[ADRV9025_TX_SAMPL_CLK] = tx_sample_clk;

	adi_adrv9025_ApiVersionGet(phy->madDevice, &apiVersion);
	adi_adrv9025_Shutdown(phy->madDevice);
	adi_adrv9025_HwClose(phy->madDevice);

	pr_info("adrv9025-phy Rev %d, API version: %u.%u.%u.%u found\n",
		phy->madDevice->devStateInfo.deviceSiRev,
		apiVersion.majorVer, apiVersion.minorVer,
		apiVersion.maintenanceVer, apiVersion.buildVer);

	return 0;

tx_out_clk_init_error:
	no_os_free(tx_sample_clk);
rx_out_clk_init_error:
	no_os_free(rx_sample_clk);
rx_out_clk_error:
	return ret;
}

int adrv9025_remove(struct adrv9025_rf_phy *phy)
{
	no_os_clk_disable(phy->dev_clk);

	adrv9025_shutdown(phy);

	return 0;
}
