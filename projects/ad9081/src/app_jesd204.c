#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include "print_log.h"
#include "jesd204.h"

#ifndef _DT_BINDINGS_IIO_ADC_AD9081_H
#define _DT_BINDINGS_IIO_ADC_AD9081_H

#define AD9081_DAC_MODE_0 0x00		/*!< I0.Q0 -> DAC0, I1.Q1 -> DAC1 */
#define AD9081_DAC_MODE_1 0x01		/*!< (I0 + I1) / 2 -> DAC0, (Q0 + Q1) / 2 -> DAC1, Data Path NCOs Bypassed */
#define AD9081_DAC_MODE_2 0x02		/*!< I0 -> DAC0, Q0 -> DAC1, Datapath0 NCO Bypassed, Datapath1 Unused */
#define AD9081_DAC_MODE_3 0x03		/*!< (I0 + I1) / 2 -> DAC0, DAC1 Output Tied To Midscale */

#define AD9081_ADC_NCO_VIF 0		/*!< Variable IF Mode */
#define AD9081_ADC_NCO_ZIF 1		/*!< Zero IF Mode */
#define AD9081_ADC_NCO_FS_4_IF 2	/*!< Fs/4 Hz IF Mode */
#define AD9081_ADC_NCO_TEST 3		/*!< Test Mode */
#define AD9081_ADC_NCO_INVALID 4	/*!< Invalid NCO Mode */

#define AD9081_ADC_4_ADC_REAL_MODE 0	/*!< Quad ADC Real Mode */
#define AD9081_ADC_4_ADC_COMP_MODE 1	/*!< Quad ADC Complex Mode */
#define AD9081_ADC_2_ADC_REAL_MODE 2	/*!< Dual ADC Real Mode */
#define AD9081_ADC_2_ADC_COMP_MODE 3	/*!< Dual ADC Complex MOde */

#define AD9081_ADC_NYQUIST_ZONE_ODD  0x00	/*!< Odd  Zone */
#define AD9081_ADC_NYQUIST_ZONE_EVEN 0x01	/*!< Even Zone */

#define FDDC_I 0
#define FDDC_Q 1

#define DEFRAMER_LINK0_TX 0
#define DEFRAMER_LINK1_TX 1
#define FRAMER_LINK0_RX 2
#define FRAMER_LINK1_RX 3

#endif

const char pretty[] = "%-20s: %-10s %-20s() %-10s\n";

static inline const char *jesd204_state_op_reason_str(enum jesd204_state_op_reason reason)                                                                                                                                                    
{
    switch (reason) {
    case JESD204_STATE_OP_REASON_INIT:
        return "initialization";
    case JESD204_STATE_OP_REASON_UNINIT:
        return "uninitialization";
    default:
        return "unknown";
    }
}

int ad9081_jesd204_uninit(struct jesd204_dev *jdev,
			    enum jesd204_state_op_reason reason)
{
	if (reason != JESD204_STATE_OP_REASON_UNINIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug(pretty, jdev->name, "", __FUNCTION__, jesd204_state_op_reason_str(reason);

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9081_jesd204_link_init(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	int ret;

	switch (reason) {
	case JESD204_STATE_OP_REASON_INIT:
		break;
	default:
		return JESD204_STATE_CHANGE_DONE;
	}

	pr_debug("%s:%d link_num %u reason %s\n", __FUNC__, __LINE__, lnk->link_id, jesd204_state_op_reason_str(reason));

	switch (lnk->link_id) {
	case DEFRAMER_LINK0_TX:
		link = &phy->jesd_tx_link;
		lnk->param.sample_rate = phy->dac_frequency_hz;
		lnk->param.sample_rate_div = phy->tx_main_interp * phy->tx_chan_interp;
		break;
	case FRAMER_LINK0_RX:
		link = &phy->jesd_rx_link[0];
		lnk->param.sample_rate = phy->adc_frequency_hz;
		lnk->param.sample_rate_div = phy->adc_dcm;
		break;
	default:
		return -EINVAL;
	}

	jesd204_copy_link_params(lnk, &link->jesd204_link);

	if (lnk->param.jesd_version == JESD204_VERSION_C)
		lnk->param.jesd_encoder = JESD204_ENCODER_64B66B;
	else
		lnk->param.jesd_encoder = JESD204_ENCODER_8B10B;

	ret = jesd204_link_get_rate_khz(lnk, &link->param.lane_rate_kbps);
	if (ret)
		return ret;

	lnk->param.sysref.mode = JESD204_SYSREF_CONTINUOUS;

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9081_jesd204_clks_enable(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	printf(pretty, jdev->name, lnk->name, __FUNCTION__, !reason ? "" : "UNINIT");

	return 0;
}

static int ad9081_jesd204_link_enable(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	printf(pretty, jdev->name, lnk->name, __FUNCTION__, !reason ? "" : "UNINIT");

	return 0;
}

static int ad9081_jesd204_link_running(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	printf(pretty, jdev->name, lnk->name, __FUNCTION__, !reason ? "" : "UNINIT");

	return 0;
}

static int ad9081_jesd204_setup_stage1(struct jesd204_dev *jdev,
					 enum jesd204_state_op_reason reason)
{
	printf(pretty, jdev->name, "", __FUNCTION__, !reason ? "" : "UNINIT");

	return 0;
}

static int ad9081_jesd204_setup_stage2(struct jesd204_dev *jdev,
					 enum jesd204_state_op_reason reason)
{
	printf(pretty, jdev->name, "", __FUNCTION__, !reason ? "" : "UNINIT");

	return 0;
}

static int ad9081_jesd204_setup_stage3(struct jesd204_dev *jdev,
					 enum jesd204_state_op_reason reason)
{
	printf(pretty, jdev->name, "", __FUNCTION__, !reason ? "" : "UNINIT");

	return 0;
}

struct ad9081_jesd204_priv {
};

static const struct jesd204_dev_data jesd204_ad9081_init = {
	.state_ops = {
		[JESD204_OP_DEVICE_INIT] = {
			.per_device = ad9081_jesd204_uninit,
		},
		[JESD204_OP_LINK_INIT] = {
			.per_link = ad9081_jesd204_link_init,
		},
		[JESD204_OP_CLOCKS_ENABLE] = {
			.per_link = ad9081_jesd204_clks_enable,
		},
		[JESD204_OP_LINK_ENABLE] = {
			.per_link = ad9081_jesd204_link_enable,
		},
		[JESD204_OP_LINK_RUNNING] = {
			.per_link = ad9081_jesd204_link_running,
		},
		[JESD204_OP_OPT_SETUP_STAGE1] = {
			.per_device = ad9081_jesd204_setup_stage1,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_OPT_SETUP_STAGE2] = {
			.per_device = ad9081_jesd204_setup_stage2,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_OPT_SETUP_STAGE3] = {
			.per_device = ad9081_jesd204_setup_stage3,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
	},

	.max_num_links = 2,
	.num_retries = 3,
	.sizeof_priv = sizeof(struct ad9081_jesd204_priv),
};

int app_jesd204_init(void)
{
	int ret;
	struct jesd204_dev *jdev;
	struct jesd204_dev_info jdev_info;
	struct jesd204_link *rx;
	struct jesd204_link *tx;
	struct jesd204_link_param rx_link_param = {
		// TODO: fill out - this is equivalent to link configuration in linux device tree
	};
	struct jesd204_link_param tx_link_param = {
		// TODO: fill out - this is equivalent to link configuration in linux device tree
	};

	ret = jesd204_link("rx_link", &rx, rx_link_param);
	if (ret < 0)
		goto error;
	ret = jesd204_link("tx_link", &tx, tx_link_param);
	if (ret < 0)
		goto error;

	// Top-level device has no output, hence we call it with NULL.
	jdev = jesd204_device("trx0_ad9081", NULL, NULL, &jdev_info);
	if (jdev == NULL)
		goto error;

	struct jesd204_dev *axi_ad9081_core_rx;
	struct jesd204_dev_info axi_ad9081_core_rx_info;
	axi_ad9081_core_rx = jesd204_device("axi_ad9081_core_rx", jdev,  rx, &axi_ad9081_core_rx_info);
	if (axi_ad9081_core_rx == NULL)
		goto error;

	struct jesd204_dev *axi_ad9081_core_tx;
	struct jesd204_dev_info axi_ad9081_core_tx_info;
	axi_ad9081_core_tx = jesd204_device("axi_ad9081_core_tx", jdev, tx, &axi_ad9081_core_tx_info);
	if (axi_ad9081_core_tx == NULL)
		goto error;

	struct jesd204_dev *axi_ad9081_rx_jesd;
	struct jesd204_dev_info axi_ad9081_rx_jesd_info;
	axi_ad9081_rx_jesd = jesd204_device("axi_ad9081_rx_jesd", axi_ad9081_core_rx, rx, &axi_ad9081_rx_jesd_info);
	if (axi_ad9081_rx_jesd == NULL)
		goto error;

	struct jesd204_dev *axi_ad9081_tx_jesd;
	struct jesd204_dev_info axi_ad9081_tx_jesd_info;
	axi_ad9081_tx_jesd = jesd204_device("axi_ad9081_tx_jesd", axi_ad9081_core_tx, tx, &axi_ad9081_tx_jesd_info);
	if (axi_ad9081_tx_jesd == NULL)
		goto error;

	struct jesd204_dev *axi_ad9081_adxcvr_rx;
	struct jesd204_dev_info axi_ad9081_adxcvr_rx_info;
	axi_ad9081_adxcvr_rx = jesd204_device("axi_ad9081_adxcvr_rx", axi_ad9081_rx_jesd, rx, &axi_ad9081_adxcvr_rx_info);
	if (axi_ad9081_adxcvr_rx == NULL)
		goto error;

	struct jesd204_dev *axi_ad9081_adxcvr_tx;
	struct jesd204_dev_info axi_ad9081_adxcvr_tx_info;
	axi_ad9081_adxcvr_tx = jesd204_device("axi_ad9081_adxcvr_tx", axi_ad9081_tx_jesd, tx, &axi_ad9081_adxcvr_tx_info);
	if (axi_ad9081_adxcvr_tx == NULL)
		goto error;

	struct jesd204_dev *hmc7044;
	struct jesd204_dev_info hmc7044_info;
	hmc7044 = jesd204_device("hmc7044", axi_ad9081_adxcvr_rx, rx, &hmc7044_info);
	if (hmc7044 == NULL)
		goto error;
	
	jesd204_connect(hmc7044, axi_ad9081_adxcvr_tx, tx);

	return jesd204_init(jdev, &jesd204_ad9081_init);
error:
	jesd204_delete(jdev);
	return ret;
}