// SPDX-License-Identifier: GPL-2.0
/*
 * ADRV9025/6 RF Transceiver
 *
 * Copyright 2020-2023 Analog Devices Inc.
 *
 * Licensed under the GPL-2.
 */

#include "adrv904x_cpu_device_profile_types.h"
#include "adi_adrv904x_utilities_types.h"
#include "adi_common_error_types.h"
#include "adi_adrv904x_all_types.h"
#include "adi_adrv904x_dfe_cpu.h"
#include "adi_common_error.h"

#include "xilinx_transceiver.h"
#include "no_os_print_log.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_util.h"
#include "no_os_spi.h"
#include "adrv904x.h"
#include "initdata.h"
#include "jesd204.h"
#include <stdbool.h>
#include <string.h>

#include "adi_adrv904x_cpu.h"

/* ADRV904X Initialization Configuration Data */
adrv904x_DeviceProfile_t adrv904xProfile;

typedef struct example_program_Ad9528Cfg {
	uint32_t RefA_kHz;          /*!< AD9528 Input Clock */
	uint32_t Vcxo_kHz;          /*!< Vcxo frequency */
	uint32_t Adrvgen6_Clk_kHz;  /*!< ADRV904X Device Clock */
	uint32_t Fpgagen6_Clk_kHz;  /*!< FPGAGEN6 Device Clock */
} example_program_Ad9528Cfg_t;

typedef struct example_program_Support {
	example_program_Ad9528Cfg_t
	clkCfg;                     /*!< AD9528 Clock Settings */

	adi_adrv904x_TrxFileInfo_t
	fileInfo;                    /*!< Pre MCS Init File Paths */
} example_program_Support_t;

example_program_Support_t programSupport;

#define ADI_EXAMPLE_STRNCPY(dest, src)                                              \
{                                                                                   \
    (void) ADI_LIBRARY_STRNCPY((char*) dest, src, ADI_ADRV904X_MAX_FILE_LENGTH);    \
    dest[ADI_ADRV904X_MAX_FILE_LENGTH - 1] = '\0';                                  \
}

uint32_t appApiCount;

/*
* \brief    Macro to perform the following:
*                       1) Check Return Code from Function Call
*                       2) Write Error Data to Standard Output if Non Zero
*                       3) Return NOK Application Return Code
*/
#define ADI_APP_API_CALL_RETURN(returnCode, funcName, ...)                                              \
{                                                                                                       \
    ++appApiCount;                                                                                      \
    returnCode = funcName( __VA_ARGS__);                                                                \
    --appApiCount;                                                                                      \
    if (returnCode)                                                                                     \
    {                                                                                                   \
        ADI_APP_ERROR_REPORT(ADI_COMMON_ERRSRC_API, returnCode, ADI_NO_VARIABLE, ADI_NO_ERROR_MESSAGE); \
                                                                                                        \
        adi_common_Device_t commonDev;                                                                  \
                                                                                                        \
        ADI_LIBRARY_MEMSET(&commonDev, 0, sizeof(adi_common_Device_t));                                 \
                                                                                                        \
        commonDev.errPtr = (adi_common_ErrData_t*) adi_hal_TlsGet(HAL_TLS_ERR);                         \
                                                                                                        \
        if (appApiCount == 0U)                                                                          \
        {                                                                                               \
            ADI_APP_ERROR_OUTPUT(commonDev.errPtr);                                                     \
        }                                                                                               \
                                                                                                        \
        return ADI_APP_API_NOK;                                                                         \
    }                                                                                                   \
}

/*
*  \brief ADI Example Application Return Error Codes
*/
typedef enum {
	ADI_APP_API_OK  = 0U,
	ADI_APP_API_NOK
} adi_app_ReturnType_e;

/*
*  \brief   Example App File Abstractions
*/
typedef enum {
	ADI_EXAMPLE_FILE_MAIN     = 0x1000U,
	ADI_EXAMPLE_FILE_CALS,
	ADI_EXAMPLE_FILE_GPIO,
	ADI_EXAMPLE_FILE_TX,
	ADI_EXAMPLE_FILE_RX,
	ADI_EXAMPLE_FILE_RADIOCTRL,
	ADI_EXAMPLE_FILE_SYSMON,
	ADI_EXAMPLE_FILE_DATA_INTERFACE,
	ADI_EXAMPLE_FILE_PROGRAM,
	ADI_EXAMPLE_FILE_BBIC,


	ADI_EXAMPLE_FILE_CARRIER_RECONFIGURE,
	ADI_EXAMPLE_FILE_DFE_APP,
} adi_example_File_e;

static void adrv904x_shutdown(struct adrv904x_rf_phy *phy)
{
	/***********************************************
	 * Shutdown Procedure *
	 * **********************************************/
	/* Function to turn radio on, Disables transmitters and receivers */

	adi_adrv904x_Shutdown(phy->kororDevice);
	adi_hal_HwClose(phy->kororDevice->common.devHalInfo);

	memset(&phy->adi_adrv904x_device.devStateInfo, 0,
	       sizeof(phy->adi_adrv904x_device.devStateInfo));
}

struct adrv904x_jesd204_link {
	unsigned int source_id;
	bool is_framer;
};

struct adrv904x_jesd204_priv {
	struct adrv904x_rf_phy *phy;
	struct adrv904x_jesd204_link link[5];
};

int adrv904x_jesd204_link_pre_setup(struct jesd204_dev *jdev,
				    enum jesd204_state_op_reason reason)
{
	struct adrv904x_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv904x_rf_phy *phy = priv->phy;
	uint32_t deviceClockScaled_kHz = 0;
	uint64_t dev_clk;
	int ret;

	deviceClockScaled_kHz =
		phy->kororDevice->initExtract.clocks.deviceClockScaled_kHz;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	switch (reason) {
	case JESD204_STATE_OP_REASON_INIT:
		break;
	default:
		return JESD204_STATE_CHANGE_DONE;
	}

	ret = no_os_clk_round_rate(phy->dev_clk,
				   deviceClockScaled_kHz * 1000, &dev_clk);
	if (ret)
		return JESD204_STATE_CHANGE_ERROR;

	if (dev_clk > 0 && ((dev_clk / 1000) ==
			    deviceClockScaled_kHz)) {
		no_os_clk_set_rate(phy->dev_clk, dev_clk);
		pr_debug("Device clock %u sucessfully set (%ld)\n",
			 deviceClockScaled_kHz * 1000, dev_clk);
	} else {
		pr_err("Requesting device clock %u failed got %ld",
		       deviceClockScaled_kHz * 1000, dev_clk);
		return -EINVAL;
	}

	uint64_t r = 0;
	no_os_clk_recalc_rate(phy->dev_clk, &r);

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv904x_jesd204_link_init(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason,
				      struct jesd204_link *lnk)
{
	struct adrv904x_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv904x_rf_phy *phy = priv->phy;
	uint32_t rate;

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
		priv->link[lnk->link_id].source_id = ADI_ADRV904X_DEFRAMER_0;
		phy->tx_iqRate_kHz  =
			phy->kororDevice->initExtract.jesdSetting.deframerSetting[0].iqRate_kHz;
		rate = phy->kororDevice->initExtract.jesdSetting.deframerSetting[0].iqRate_kHz;
		lnk->num_lanes = no_os_hweight8(
					 phy->kororDevice->initExtract.jesdSetting.deframerSetting[0].deserialLaneEnabled);
		lnk->num_converters =
			phy->kororDevice->initExtract.jesdSetting.deframerSetting[0].jesdM;
		lnk->bits_per_sample =
			phy->kororDevice->initExtract.jesdSetting.deframerSetting[0].jesdNp;
		break;
	case DEFRAMER1_LINK_TX:
		priv->link[lnk->link_id].source_id = ADI_ADRV904X_DEFRAMER_1;
		phy->tx_iqRate_kHz  =
			phy->kororDevice->initExtract.jesdSetting.deframerSetting[0].iqRate_kHz;
		rate = phy->kororDevice->initExtract.jesdSetting.deframerSetting[1].iqRate_kHz;
		lnk->num_lanes = no_os_hweight8(
					 phy->kororDevice->initExtract.jesdSetting.deframerSetting[1].deserialLaneEnabled);
		lnk->num_converters =
			phy->kororDevice->initExtract.jesdSetting.deframerSetting[1].jesdM;
		lnk->bits_per_sample =
			phy->kororDevice->initExtract.jesdSetting.deframerSetting[1].jesdNp;
		break;
	case FRAMER0_LINK_RX:
		priv->link[lnk->link_id].source_id = ADI_ADRV904X_FRAMER_0;
		priv->link[lnk->link_id].is_framer = true;
		phy->rx_iqRate_kHz  =
			phy->kororDevice->initExtract.jesdSetting.framerSetting[0].iqRate_kHz;
		rate = phy->kororDevice->initExtract.jesdSetting.framerSetting[0].iqRate_kHz;
		lnk->num_lanes = no_os_hweight8(
					 phy->kororDevice->initExtract.jesdSetting.framerSetting[0].serialLaneEnabled);
		lnk->num_converters =
			phy->kororDevice->initExtract.jesdSetting.framerSetting[0].jesdM;
		lnk->bits_per_sample =
			phy->kororDevice->initExtract.jesdSetting.framerSetting[0].jesdNp;
		break;
	case FRAMER1_LINK_RX:
		priv->link[lnk->link_id].source_id = ADI_ADRV904X_FRAMER_1;
		priv->link[lnk->link_id].is_framer = true;
		phy->rx_iqRate_kHz  =
			phy->kororDevice->initExtract.jesdSetting.framerSetting[1].iqRate_kHz;
		rate = phy->kororDevice->initExtract.jesdSetting.framerSetting[1].iqRate_kHz;
		lnk->num_lanes = no_os_hweight8(
					 phy->kororDevice->initExtract.jesdSetting.framerSetting[1].serialLaneEnabled);
		lnk->num_converters =
			phy->kororDevice->initExtract.jesdSetting.framerSetting[1].jesdM;
		lnk->bits_per_sample =
			phy->kororDevice->initExtract.jesdSetting.framerSetting[1].jesdNp;
		break;
	case FRAMER2_LINK_RX:
		priv->link[lnk->link_id].source_id = ADI_ADRV904X_FRAMER_2;
		priv->link[lnk->link_id].is_framer = true;
		phy->rx_iqRate_kHz  =
			phy->kororDevice->initExtract.jesdSetting.framerSetting[2].iqRate_kHz;
		rate = phy->kororDevice->initExtract.jesdSetting.framerSetting[2].iqRate_kHz;
		lnk->num_lanes = no_os_hweight8(
					 phy->kororDevice->initExtract.jesdSetting.framerSetting[2].serialLaneEnabled);
		lnk->num_converters =
			phy->kororDevice->initExtract.jesdSetting.framerSetting[2].jesdM;
		lnk->bits_per_sample =
			phy->kororDevice->initExtract.jesdSetting.framerSetting[2].jesdNp;
		break;
	default:
		return -EINVAL;
	}

	lnk->sample_rate = rate * 1000;

	if (phy->tx_iqRate_kHz && (lnk->link_id == DEFRAMER0_LINK_TX)) {
		lnk->octets_per_frame = 4;
		lnk->frames_per_multiframe = 64;
		lnk->device_id = 0;
		lnk->bank_id = 0;
		lnk->scrambling = 1;
		lnk->converter_resolution = 16;
		lnk->ctrl_bits_per_sample = 0;
		lnk->jesd_version = JESD204_VERSION_C;
		lnk->subclass = JESD204_SUBCLASS_1;
		lnk->is_transmit = false;
		lnk->jesd_encoder = JESD204_ENCODER_64B66B;
	} else if (phy->rx_iqRate_kHz && (lnk->link_id == FRAMER0_LINK_RX)) {
		lnk->octets_per_frame = 4;
		lnk->frames_per_multiframe = 64;
		lnk->device_id = 0;
		lnk->bank_id = 0;
		lnk->scrambling = 1;
		lnk->converter_resolution = 16;
		lnk->ctrl_bits_per_sample = 0;
		lnk->jesd_version = JESD204_VERSION_C;
		lnk->subclass = JESD204_SUBCLASS_1;
		lnk->is_transmit = true;
		lnk->jesd_encoder = JESD204_ENCODER_64B66B;
	};

	return JESD204_STATE_CHANGE_DONE;
}

int adrv904x_jesd204_link_setup(struct jesd204_dev *jdev,
				enum jesd204_state_op_reason reason)
{
	adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
	struct adrv904x_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv904x_rf_phy *phy = priv->phy;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));


	if (reason == JESD204_STATE_OP_REASON_UNINIT) {
		phy->is_initialized = 0;

		adi_adrv904x_HwClose(phy->kororDevice);

		memset(&phy->adi_adrv904x_device.devStateInfo, 0,
		       sizeof(phy->adi_adrv904x_device.devStateInfo));

		return JESD204_STATE_CHANGE_DONE;
	}

	memset(&phy->adi_adrv904x_device.devStateInfo, 0,
	       sizeof(phy->adi_adrv904x_device.devStateInfo));

	recoveryAction = adi_adrv904x_HwOpen(phy->kororDevice, &phy->spiSettings);
	if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE) {
		pr_err("ERROR adi_ad9528_HwOpen failed in %s at line %d.\n", __func__,
		       __LINE__);
		return JESD204_STATE_CHANGE_ERROR;
	}

	adi_common_LogLevelSet(&phy->kororDevice->common,
			       ADI_HAL_LOG_ERR | ADI_HAL_LOG_WARN);

	recoveryAction = adi_adrv904x_HwReset(phy->kororDevice);

	recoveryAction = adi_adrv904x_PreMcsInit(phy->kororDevice, &deviceInitStruct,
			 &phy->trxBinaryInfoPtr);
	if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE) {
		pr_err("ERROR adi_adrv904x_PreMcsInit failed in %s at line %d.\n", __func__,
		       __LINE__);
		return JESD204_STATE_CHANGE_ERROR;
	}

	recoveryAction = adi_adrv904x_PreMcsInit_NonBroadcast(phy->kororDevice,
			 &deviceInitStruct);
	if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE) {
		pr_err("ERROR adi_adrv904x_PreMcsInit_NonBroadcast failed in %s at line %d.\n",
		       __func__, __LINE__);
		return JESD204_STATE_CHANGE_ERROR;
	}

	recoveryAction = adi_adrv904x_MultichipSyncSet(phy->kororDevice, ADI_ENABLE);
	if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE) {
		pr_err("ERROR adi_adrv904x_MultichipSyncSet failed in %s at line %d.\n",
		       __func__, __LINE__);
		return JESD204_STATE_CHANGE_ERROR;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv904x_jesd204_setup_stage1(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason)
{
	adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_DEVICE;
	struct adrv904x_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv904x_rf_phy *phy = priv->phy;
	uint32_t mcsStatus;
	int i;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	/* This loop will send SysRef pulses up to 255 times unless MCS status achieved before. */
	for (i = 0; i < 255; i++) {
		recoveryAction = adi_adrv904x_MultichipSyncStatusGet(phy->kororDevice,
				 &mcsStatus);
		if (recoveryAction) {
			ADI_API_ERROR_REPORT(&phy->kororDevice->common, recoveryAction,
					     "Issue during getting multi-chip sync status");
			return JESD204_STATE_CHANGE_ERROR;
		}

		if ((mcsStatus & 0x01) == 0x01)
			break;

		jesd204_sysref_async_force(phy->jdev);
	}

	if (mcsStatus != 0x01) {
		pr_err("%s:%d Unexpected MCS sync status (0x%X)", __func__, __LINE__,
		       mcsStatus);

		return JESD204_STATE_CHANGE_ERROR;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv904x_jesd204_setup_stage2(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason)
{
	adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_DEVICE;
	struct adrv904x_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv904x_rf_phy *phy = priv->phy;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	/* MCS end sequence*/
	recoveryAction = adi_adrv904x_MultichipSyncSet(phy->kororDevice, ADI_DISABLE);
	if (recoveryAction) {
		pr_err("ERROR adi_adrv904x_MultichipSyncSet failed in %s at line %d.\n",
		       __func__, __LINE__);
		return JESD204_STATE_CHANGE_ERROR;
	}

	/* Post MCS */
	recoveryAction = adi_adrv904x_PostMcsInit(phy->kororDevice,
			 &utilityInit);
	if (recoveryAction) {
		pr_err("ERROR adi_adrv904x_PostMcsInit failed in %s at line %d.\n", __func__,
		       __LINE__);
		return JESD204_STATE_CHANGE_ERROR;
	}

 	recoveryAction = adi_adrv904x_SerializerReset(phy->kororDevice);
 	if (recoveryAction) {
 		pr_err();
 		return recoveryAction;
 	}

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv904x_jesd204_clks_enable(struct jesd204_dev *jdev,
					enum jesd204_state_op_reason reason,
					struct jesd204_link *lnk)
{
	struct adrv904x_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv904x_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	if (!lnk->num_converters)
		return JESD204_STATE_CHANGE_DONE;

	if (priv->link[lnk->link_id].is_framer) {
		if (phy->kororDevice->devStateInfo.linkSharingEnabled == 1) {
			ret = adi_adrv904x_FramerSysrefCtrlSet(phy->kororDevice,
							       ADI_ADRV904X_FRAMER_1, ADI_DISABLE);
			if (ret) {
				pr_err("ERROR adi_adrv904x_FramerSysrefCtrlSet failed in %s at line %d.\n",
				       __func__, __LINE__);
				return JESD204_STATE_CHANGE_ERROR;
			}

			ret = adi_adrv904x_FramerLinkStateSet(phy->kororDevice,
							      ADI_ADRV904X_FRAMER_1, ADI_DISABLE);
			if (ret) {
				pr_err("ERROR adi_adrv904x_FramerLinkStateSet failed in %s at line %d.\n",
				       __func__, __LINE__);
				return JESD204_STATE_CHANGE_ERROR;
			}

			ret = adi_adrv904x_SerializerReset(phy->kororDevice);
			if (ret) {
				pr_err("ERROR adi_adrv904x_SerializerReset failed in %s at line %d.\n",
				       __func__, __LINE__);
				return JESD204_STATE_CHANGE_ERROR;
			}

			ret = adi_adrv904x_FramerSysrefCtrlSet(phy->kororDevice,
							       ADI_ADRV904X_FRAMER_1, ADI_ENABLE);
			if (ret) {
				pr_err("ERROR adi_adrv904x_FramerSysrefCtrlSet failed in %s at line %d.\n",
				       __func__, __LINE__);
				return JESD204_STATE_CHANGE_ERROR;
			}

			ret = adi_adrv904x_FramerLinkStateSet(phy->kororDevice,
							      ADI_ADRV904X_FRAMER_1, ADI_ENABLE);
			if (ret) {
				pr_err("ERROR adi_adrv904x_FramerLinkStateSet failed in %s at line %d.\n",
				       __func__, __LINE__);
				return JESD204_STATE_CHANGE_ERROR;
			}

			pr_debug("%s:%d Link %d Framer enabled\n", __func__, __LINE__,
				 ADI_ADRV904X_FRAMER_1);

			jesd204_sysref_async_force(phy->jdev);

			ret = adi_adrv904x_FramerLinkStateSet(phy->kororDevice,
							      ADI_ADRV904X_FRAMER_1, ADI_DISABLE);
			if (ret) {
				pr_err("ERROR adi_adrv904x_FramerLinkStateSet failed in %s at line %d.\n",
				       __func__, __LINE__);
				return JESD204_STATE_CHANGE_ERROR;
			}

			ret = adi_adrv904x_FramerSysrefCtrlSet(phy->kororDevice,
							       ADI_ADRV904X_FRAMER_1, ADI_DISABLE);
			if (ret) {
				pr_err("ERROR adi_adrv904x_FramerSysrefCtrlSet failed in %s at line %d.\n",
				       __func__, __LINE__);
				return JESD204_STATE_CHANGE_ERROR;
			}

		}

		ret = adi_adrv904x_FramerSysrefCtrlSet(phy->kororDevice,
						       priv->link[lnk->link_id].source_id, ADI_DISABLE);
		if (ret) {
			pr_err("ERROR adi_adrv904x_FramerSysrefCtrlSet failed in %s at line %d.\n",
			       __func__, __LINE__);
			return JESD204_STATE_CHANGE_ERROR;
		}

		ret = adi_adrv904x_FramerLinkStateSet(phy->kororDevice,
						      priv->link[lnk->link_id].source_id, ADI_DISABLE);
		if (ret) {
			pr_err("ERROR adi_adrv904x_FramerLinkStateSet failed in %s at line %d.\n",
			       __func__, __LINE__);
			return JESD204_STATE_CHANGE_ERROR;
		}


		ret = adi_adrv904x_FramerLinkStateSet(phy->kororDevice,
						      priv->link[lnk->link_id].source_id, ADI_ENABLE);
		if (ret) {
			pr_err("ERROR adi_adrv904x_FramerLinkStateSet failed in %s at line %d.\n",
			       __func__, __LINE__);
			return JESD204_STATE_CHANGE_ERROR;
		}


		pr_debug("%s:%d Link %d Framer enabled\n", __func__, __LINE__,
			 priv->link[lnk->link_id].source_id);

		ret = adi_adrv904x_FramerSysrefCtrlSet(phy->kororDevice,
						       priv->link[lnk->link_id].source_id, ADI_DISABLE);
		if (ret) {
			pr_err("ERROR adi_adrv904x_FramerSysrefCtrlSet failed in %s at line %d.\n",
			       __func__, __LINE__);
			return JESD204_STATE_CHANGE_ERROR;
		}
	} else {
		ret = adi_adrv904x_DeframerSysrefCtrlSet(phy->kororDevice,
				priv->link[lnk->link_id].source_id, ADI_DISABLE);
		if (ret) {
			pr_err("ERROR adi_adrv904x_DeframerSysrefCtrlSet failed in %s at line %d.\n",
			       __func__, __LINE__);
			return JESD204_STATE_CHANGE_ERROR;
		}


		ret = adi_adrv904x_DeframerLinkStateSet(phy->kororDevice,
							priv->link[lnk->link_id].source_id, ADI_DISABLE);
		if (ret) {
			pr_err("ERROR adi_adrv904x_DeframerLinkStateSet failed in %s at line %d.\n",
			       __func__, __LINE__);
			return JESD204_STATE_CHANGE_ERROR;
		}

	};

	return JESD204_STATE_CHANGE_DONE;
}

static int adrv904x_jesd204_link_enable(struct jesd204_dev *jdev,
					enum jesd204_state_op_reason reason,
					struct jesd204_link *lnk)
{
	struct adrv904x_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct adrv904x_rf_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	if (!lnk->num_converters)
		return JESD204_STATE_CHANGE_DONE;

	if (!priv->link[lnk->link_id].is_framer) { /* DEFRAMER */
		adi_adrv904x_InitCals_t serdesCal = {
			.calMask = ADI_ADRV904X_IC_SERDES,
			.orxChannelMask = 0x00U,
			.rxChannelMask = 0x00U,
			.txChannelMask = 0xFFU, /* CAL_ALL_CHANNELS */
			.warmBoot = 0,
		};

		ret = adi_adrv904x_DeframerLinkStateSet(phy->kororDevice,
						    priv->link[lnk->link_id].source_id, ADI_ENABLE);
		if (ret) {
			pr_err("ERROR adi_adrv904x_DeframerLinkStateSet failed in %s at line %d.\n",
				   __func__, __LINE__);
			return JESD204_STATE_CHANGE_ERROR;
		}

		/* Notify ARM to run SERDES Calbriation if necessary */
		ret = adi_adrv904x_InitCalsRun(phy->kororDevice, &serdesCal);
		if (ret) {
			pr_err("ERROR adi_adrv904x_InitCalsRun failed in %s at line %d.\n",
				   __func__, __LINE__);
			return JESD204_STATE_CHANGE_ERROR;
		}

		/* Wait up to 60 seconds for ARM */
		ret = adi_adrv904x_InitCalsWait(phy->kororDevice, 60000);
		if (ret) {
			pr_err("ERROR adi_adrv904x_InitCalsWait failed in %s at line %d.\n", __func__, __LINE__);
			return JESD204_STATE_CHANGE_ERROR;
		}

		adi_adrv904x_InitCalStatus_t status = {0};
		adi_adrv904x_InitCalsDetailedStatusGet(phy->kororDevice, &status);

		/***************************************************/
		/**** Enable SYSREF to Koror JESD204C Deframer ***/
		/***************************************************/
		ret = adi_adrv904x_DeframerSysrefCtrlSet(phy->kororDevice,
				priv->link[lnk->link_id].source_id, ADI_ENABLE);
		if (ret) {
			pr_err("ERROR adi_adrv904x_DeframerSysrefCtrlSet failed in %s at line %d.\n",
				   __func__, __LINE__);
			return JESD204_STATE_CHANGE_ERROR;
		}

	};

	return JESD204_STATE_CHANGE_DONE;
}

 static int adrv904x_jesd204_link_running(struct jesd204_dev *jdev,
 		enum jesd204_state_op_reason reason,
 		struct jesd204_link *lnk)
 {
 	struct adrv904x_jesd204_priv *priv = jesd204_dev_priv(jdev);
 	struct adrv904x_rf_phy *phy = priv->phy;
 	int ret;

 	adi_adrv904x_FramerStatus_t framerStatus;
 	adi_adrv904x_DeframerStatus_t deframerStatus;
 	uint8_t deframerLinkCondition = 0;

 	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
 		 lnk->link_id, jesd204_state_op_reason_str(reason));

 	if (reason != JESD204_STATE_OP_REASON_INIT)
 		return JESD204_STATE_CHANGE_DONE;

 	if (!lnk->num_converters)
 		return JESD204_STATE_CHANGE_DONE;

 	if (priv->link[lnk->link_id].is_framer) {
 		ret = adi_adrv904x_FramerStatusGet(phy->kororDevice,
 						   priv->link[lnk->link_id].source_id, &framerStatus);
 		if (ret)
 			return -1;


 		if ((framerStatus.status & 0x0F) != 0x0A)
 			pr_warning("Link%u framerStatus 0x%X\n",
 				   lnk->link_id, framerStatus.status);
 	} else {
 		ret = adi_adrv904x_DeframerStatusGet(phy->kororDevice,
 						     priv->link[lnk->link_id].source_id, &deframerStatus);
 		if (ret)
 			return -1;

 		ret  = adi_adrv904x_DfrmLinkConditionGet(
 			       phy->kororDevice,
 			       priv->link[lnk->link_id].source_id,
 			       &deframerLinkCondition);

 		if ((deframerStatus.status & 0x7F) != 0x7) /* Ignore Valid ILAS checksum */
 			pr_warning("Link%u deframerStatus 0x%X\n",
 				   lnk->link_id, deframerStatus.status);

 		/* Kick off SERDES tracking cal if lanes are up */
 		ret = adi_adrv904x_TrackingCalsEnableSet(
 			      phy->kororDevice, ADI_ADRV904X_TC_TX_SERDES_MASK,
				  0xFFU, // all channels
 			      ADI_ADRV904X_TRACKING_CAL_ENABLE);
 		if (ret)
 			return -1;
 	};

 	return JESD204_STATE_CHANGE_DONE;
 }


 static int adrv904x_jesd204_post_running_stage(struct jesd204_dev *jdev,
 		enum jesd204_state_op_reason reason)
 {
 	struct adrv904x_jesd204_priv *priv = jesd204_dev_priv(jdev);
 	struct adrv904x_rf_phy *phy = priv->phy;
 	int ret;

 	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
 		 jesd204_state_op_reason_str(reason));

 	if (reason != JESD204_STATE_OP_REASON_INIT) {
 		phy->is_initialized = 0;
 		return JESD204_STATE_CHANGE_DONE;
 	}

// 	/* Initialize Tx Ramp down functionality */
// 	ret = 	(phy->madDevice, &phy->deviceInitStruct);
// 	if (ret)
// 		return adrv9025_dev_err(phy);
//
// 	/* Setup GP Interrupts from init structure */
// 	ret = adi_adrv904x_GpIntInit(phy->madDevice,
// 				     &phy->deviceInitStruct.gpInterrupts);
// 	if (ret)
// 		return adrv9025_dev_err(phy);

 	no_os_clk_set_rate(phy->clks[ADRV904X_RX_SAMPL_CLK], phy->rx_iqRate_kHz * 1000);
 	no_os_clk_set_rate(phy->clks[ADRV904X_TX_SAMPL_CLK], phy->tx_iqRate_kHz * 1000);

 	ret = adi_adrv904x_RxTxEnableSet(phy->kororDevice, 0x00, 0x00, ADI_ADRV904X_RX_MASK_ALL, ADI_ADRV904X_RX_MASK_ALL, ADI_ADRV904X_TXALL, ADI_ADRV904X_TXALL);
 	if (ret)
 		return -1;

 	phy->is_initialized = 1;
// 	adrv9025_info(phy);

 	return JESD204_STATE_CHANGE_DONE;
 }

static const struct jesd204_dev_data jesd204_adrv904x_init = {
	.state_ops = {
		[JESD204_OP_LINK_INIT] = {
			.per_link = adrv904x_jesd204_link_init,
		},
		[JESD204_OP_LINK_PRE_SETUP] = {
			.per_device = adrv904x_jesd204_link_pre_setup,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_LINK_SETUP] = {
			.per_device = adrv904x_jesd204_link_setup,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_OPT_SETUP_STAGE1] = {
			.per_device = adrv904x_jesd204_setup_stage1,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_OPT_SETUP_STAGE2] = {
			.per_device = adrv904x_jesd204_setup_stage2,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_CLOCKS_ENABLE] = {
			.per_link = adrv904x_jesd204_clks_enable,
		},
		[JESD204_OP_LINK_ENABLE] = {
			.per_link = adrv904x_jesd204_link_enable,
			.post_state_sysref = true,
		},
		[JESD204_OP_LINK_RUNNING] = {
			.per_link = adrv904x_jesd204_link_running,
		},
		[JESD204_OP_OPT_POST_RUNNING_STAGE] = {
			.per_device = adrv904x_jesd204_post_running_stage,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
	},

	.max_num_links = 10,
	.sizeof_priv = sizeof(struct adrv904x_jesd204_priv),
};

/* Helper function to populate trxBinaryInfoPtr structure used for PreMcs
 *
 * Returns 0 on success.
 */
static int populateTrxFileInfo(adi_adrv904x_TrxFileInfo_t * const
			       trxBinaryInfoPtr,
			       const char * const cpuBin,
			       const char * const configBin,
			       const char * const streamBin,
			       const char * const gainTable,
			       const char * const dfeBin,
			       const char * const radSeqBin)

{
	if(trxBinaryInfoPtr == NULL || cpuBin == NULL || streamBin == NULL
	    || gainTable == NULL) {
		pr_err("Invalid parameters for input\n");
		return -1;
	}

	pr_info("Firmware file: %s\n", cpuBin);
	pr_info("Gain Table file: %s\n", gainTable);
	pr_info("Streams file: %s\n", streamBin);
	pr_info("Device Profile file: %s\n", configBin);
	pr_info("DFE file: %s\n", dfeBin);
	pr_info("Radio Sequencer file: %s\n", radSeqBin);
	strncpy((char *)trxBinaryInfoPtr->dfeCpu.filePath, dfeBin,
		ADI_ADRV904X_MAX_FILE_LENGTH);
	trxBinaryInfoPtr->dfeCpu.filePath[ADI_ADRV904X_MAX_FILE_LENGTH - 1] = '\0';
	strncpy((char *)trxBinaryInfoPtr->radioSequencer.filePath, radSeqBin,
		ADI_ADRV904X_MAX_FILE_LENGTH);
	trxBinaryInfoPtr->radioSequencer.filePath[ADI_ADRV904X_MAX_FILE_LENGTH - 1] =
		'\0';

	fflush(stdout);

	strncpy((char *)trxBinaryInfoPtr->stream.filePath, streamBin,
		ADI_ADRV904X_MAX_FILE_LENGTH);
	trxBinaryInfoPtr->stream.filePath[ADI_ADRV904X_MAX_FILE_LENGTH - 1] = '\0';
	strncpy((char *)trxBinaryInfoPtr->cpu.filePath, cpuBin,
		ADI_ADRV904X_MAX_FILE_LENGTH);
	trxBinaryInfoPtr->cpu.filePath[ADI_ADRV904X_MAX_FILE_LENGTH - 1] = '\0';
	strncpy((char *)trxBinaryInfoPtr->cpuProfile.filePath, configBin,
		ADI_ADRV904X_MAX_FILE_LENGTH);
	trxBinaryInfoPtr->cpuProfile.filePath[ADI_ADRV904X_MAX_FILE_LENGTH - 1] = '\0';

	for (unsigned i = 0; i < ADI_ADRV904X_RX_GAIN_TABLE_ARR_MAX; i++) {
		trxBinaryInfoPtr->rxGainTable[i].channelMask = 1 << i;
		strncpy((char *)trxBinaryInfoPtr->rxGainTable[i].filePath, gainTable,
			ADI_ADRV904X_MAX_FILE_LENGTH);
		trxBinaryInfoPtr->rxGainTable[i].filePath[ADI_ADRV904X_MAX_FILE_LENGTH - 1] =
			'\0';
	}

	return 0;
}

/**
* Initialize the device.
* @param dev - The device structure.
* @param init_param - The structure that contains the device initial
* 		       parameters.
* @return 0 in case of success, negative error code otherwise.
*/
int32_t adrv904x_init(struct adrv904x_rf_phy **dev,
		      const struct adrv904x_init_param *init_param)
{
	const char dfeBin[ADI_ADRV904X_MAX_FILE_LENGTH] = "ADRV9040_DFE_CALS_FW.bin";
	const char configBin[ADI_ADRV904X_MAX_FILE_LENGTH] = "DeviceProfileTest.bin";
	const char streamBin[ADI_ADRV904X_MAX_FILE_LENGTH] = "stream_image.bin";
	const char gainTable[ADI_ADRV904X_MAX_FILE_LENGTH] = "RxGainTable.csv";
	const char cpuBin[ADI_ADRV904X_MAX_FILE_LENGTH] = "ADRV9040_FW.bin";
	const char radSeqBin[ADI_ADRV904X_MAX_FILE_LENGTH] = "";
	adi_adrv904x_SpiConfigSettings_t *spi_settings;
	adi_adrv904x_SpiOptions_t *spi_options;
	struct adrv904x_jesd204_priv *priv;
	struct adrv904x_rf_phy *phy;
	int ret;

	phy = (struct adrv904x_rf_phy *)no_os_calloc(1, sizeof(*phy));
	if (!phy)
		goto error;

	phy->kororDevice = init_param->adrv904x_device;
	phy->spi_device_id = 0;
	phy->device_id = 0;
	phy->dev_clk = init_param->dev_clk;

	spi_settings = adrv904x_spi_settings_get();
	spi_options = adrv904x_spi_options_get();
	phy->spiSettings.msbFirst = spi_settings->msbFirst;
	phy->spiOptions.allowSpiStreaming = spi_options->allowSpiStreaming;
	phy->spiOptions.allowAhbAutoIncrement = spi_options->allowAhbAutoIncrement;
	phy->spiOptions.allowAhbSpiFifoMode = spi_options->allowAhbSpiFifoMode;
	phy->spiSettings.fourWireMode = spi_settings->fourWireMode;
	phy->spiSettings.cmosPadDrvStrength = spi_settings->cmosPadDrvStrength;

	/* Load structure for PreMcsInit */
	ret = populateTrxFileInfo(&phy->trxBinaryInfoPtr, &cpuBin[0], &configBin[0],
				  &streamBin[0],
				  &gainTable[0], &dfeBin[0], &radSeqBin[0]);
	if (ret) {
		pr_err("ERROR populateTrxFileInfo failed in %s at line %d.\n", __func__,
		       __LINE__);
		return -1;
	}

	ret = adrv904x_setup(phy);
	if (ret < 0) {
		pr_err("%s: adrv9025_setup failed (%d)\n", __func__, ret);
		goto error_setup;
	}

	ret = jesd204_dev_register(&phy->jdev, &jesd204_adrv904x_init);
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

static int32_t adrv9040_bb_recalc_rate(struct no_os_clk_desc *desc,
				       uint64_t *rate)
{
	struct adrv904x_rf_phy *adrv9040_dev;
	uint64_t read_rate = 0;
	int ret;

	adrv9040_dev = desc->dev_desc;

	ret = no_os_clk_recalc_rate(adrv9040_dev->dev_clk, &read_rate);
	if (!ret)
		*rate = read_rate;

	return ret;
}

static int32_t adrv9040_bb_set_rate(struct no_os_clk_desc *desc,
				    uint64_t rate)
{
	struct adrv904x_rf_phy *adrv9040_dev;
	adrv9040_dev = desc->dev_desc;

	return no_os_clk_set_rate(adrv9040_dev->dev_clk, rate);
}

static int32_t adrv9040_bb_round_rate(struct no_os_clk_desc *desc,
				      uint64_t rate,
				      uint64_t *rounded_rate)
{
	pr_debug("%s: Rate %lu Hz", __func__, rate);

	*rounded_rate = rate;

	return 0;
}

static const struct no_os_clk_platform_ops adrv9040_bb_clk_ops = {
	.clk_round_rate = &adrv9040_bb_round_rate,
	.clk_set_rate = &adrv9040_bb_set_rate,
	.clk_recalc_rate = &adrv9040_bb_recalc_rate,
};

int adrv904x_setup(struct adrv904x_rf_phy *phy)
{
	adi_adrv904x_ExtractInitDataOutput_e checkExtractInitData =
		ADI_ADRV904X_EXTRACT_INIT_DATA_NOT_POPULATED;
	const char *names[NUM_ADRV904X_CLKS] = {
		"-rx_sampl_clk", "-tx_sampl_clk", "-orx_sampl_clk"
	};
	int recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
	struct no_os_clk_desc *orx_sample_clk = NULL;
	struct no_os_clk_desc *rx_sample_clk = NULL;
	struct no_os_clk_desc *tx_sample_clk = NULL;
	struct no_os_clk_init_param clk_init;
	adi_adrv904x_Version_t apiVersion;
	adi_common_ErrData_t errData;
	adi_common_ErrData_t* errPtr;
	uint8_t siRevision = 0xbb;
	int ret;

	phy->kororDevice = &phy->adi_adrv904x_device;
	phy->kororDevice->common.devHalInfo = &phy->hal;

	ADI_APP_MESSAGE("\nadrv904x_setup()\n");

	(void) ADI_LIBRARY_MEMSET(&errData, 0, sizeof(adi_common_ErrData_t));

	/* Configure Example Application for No Buffering to Standard Output */
	ADI_LIBRARY_SETVBUF(stdout, NULL, _IONBF, 0);

	ret = no_os_clk_enable(phy->dev_clk);
	if (ret)
		return ret;

	errPtr = (adi_common_ErrData_t *)no_os_calloc(1, sizeof(adi_common_ErrData_t));
	if (!errPtr)
		return -ENOMEM;

	phy->kororDevice->common.errPtr = errPtr;

	ret = adi_adrv904x_HwOpen(phy->kororDevice, adrv904x_spi_settings_get());
	if (ret)
		return ret;

	rx_sample_clk = no_os_calloc(1, sizeof(rx_sample_clk));
	if (!rx_sample_clk)
		goto rx_out_clk_error;

	/* Initialize clk component */
	clk_init.name = names[ADRV904X_RX_SAMPL_CLK];
	clk_init.hw_ch_num = 1;
	clk_init.platform_ops = &adrv9040_bb_clk_ops;
	clk_init.dev_desc = phy;

	ret = no_os_clk_init(&rx_sample_clk, &clk_init);
	if (ret)
		goto rx_out_clk_init_error;

	phy->clks[ADRV904X_RX_SAMPL_CLK] = rx_sample_clk;

	tx_sample_clk = no_os_calloc(1, sizeof(tx_sample_clk));
	if (!tx_sample_clk)
		goto rx_out_clk_init_error;

	/* Initialize clk component */
	clk_init.name = names[ADRV904X_TX_SAMPL_CLK];
	clk_init.hw_ch_num = 1;
	clk_init.platform_ops = &adrv9040_bb_clk_ops;
	clk_init.dev_desc = phy;

	ret = no_os_clk_init(&tx_sample_clk, &clk_init);
	if (ret)
		goto tx_out_clk_init_error;

	phy->clks[ADRV904X_TX_SAMPL_CLK] = tx_sample_clk;

	orx_sample_clk = no_os_calloc(1, sizeof(orx_sample_clk));
	if (!orx_sample_clk)
		goto tx_out_clk_init_error;

	/* Initialize clk component */
	clk_init.name = names[ADRV904X_ORX_SAMPL_CLK];
	clk_init.hw_ch_num = 1;
	clk_init.platform_ops = &adrv9040_bb_clk_ops;
	clk_init.dev_desc = phy;

	ret = no_os_clk_init(&orx_sample_clk, &clk_init);
	if (ret)
		goto orx_out_clk_init_error;

	phy->clks[ADRV904X_ORX_SAMPL_CLK] = orx_sample_clk;

	ret = adi_adrv904x_SpiVerify(phy->kororDevice);
	if (ret)
		goto orx_out_clk_init_error;

	ret = adi_adrv904x_ApiVersionGet(phy->kororDevice, &apiVersion);
	if (ret)
		goto orx_out_clk_init_error;

	ret = adi_adrv904x_DeviceRevGet(phy->kororDevice, &siRevision);
	if (ret)
		goto orx_out_clk_init_error;

	pr_info("adrv904x-phy Rev %d, API version: %u.%u.%u.%u found\n",
		phy->kororDevice->devStateInfo.deviceSiRev,
		apiVersion.majorVer, apiVersion.minorVer,
		apiVersion.maintenanceVer, apiVersion.buildVer);

	pr_info("adrv904x-device revision: %#x\n", siRevision);

	if (apiVersion.majorVer > 1U) {
		ADI_APP_API_CALL_RETURN(recoveryAction, adi_adrv904x_InitDataExtract,
					phy->kororDevice,
					&phy->trxBinaryInfoPtr.cpuProfile,
					&initStructApiVersion,
					&initStructArmVersion,
					&initStructStreamVersion,
					&deviceInitStruct,
					&utilityInit,
					&checkExtractInitData);

		switch (checkExtractInitData) {
		case ADI_ADRV904X_EXTRACT_INIT_DATA_LEGACY_PROFILE_BIN:
			ADI_APP_MESSAGE("\n\tUsing the Default Init and PostMcsInit Structures\n");
			break;

		case ADI_ADRV904X_EXTRACT_INIT_DATA_POPULATED:
			ADI_APP_MESSAGE("\n\tUsing the Profile Init and PostMcsInit Structures\n");
			break;

		case ADI_ADRV904X_EXTRACT_INIT_DATA_NOT_POPULATED:
		/* Fall Through */

		default:
			ADI_APP_ERROR_REPORT(ADI_COMMON_ERRCODE_INVALID_PARAM,
					     recoveryAction,
					     &deviceInitStruct,
					     "PreMcsInit and/or PostMcsInit Data Structures Not Populated");
			return ADI_APP_API_NOK;
			break;
		}
	}

	/* Extract Info from CPU Profile Binary */
	/* Required for Link init */
	recoveryAction = adi_adrv904x_DeviceInfoExtract(phy->kororDevice,
			 &phy->trxBinaryInfoPtr.cpuProfile);
	if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE) {
		ADI_API_ERROR_REPORT(&phy->kororDevice->common, recoveryAction,
				     "Issue during CPU Profile Binary Image Extract");
		return -1;
	}

	return 0;

orx_out_clk_init_error:
	no_os_free(orx_sample_clk);
tx_out_clk_init_error:
	no_os_free(tx_sample_clk);
rx_out_clk_init_error:
	no_os_free(rx_sample_clk);
rx_out_clk_error:
	return ret;
}

int adrv904x_remove(struct adrv904x_rf_phy *phy)
{
	no_os_clk_disable(phy->dev_clk);

	adrv904x_shutdown(phy);

	return 0;
}
