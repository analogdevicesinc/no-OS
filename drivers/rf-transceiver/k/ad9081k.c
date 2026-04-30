/***************************************************************************//**
 *   @file   ad9081k.c
 *   @brief  AD9081K (K) no-OS dummy driver with JESD204 FSM ops.
 *   @author Analog Devices Inc.
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <string.h>
#include "ad9081k.h"
#include "app_config.h"
#include "jesd204.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_print_log.h"

/******************************************************************************/
/************************ JESD204 FSM Structures ******************************/
/******************************************************************************/

struct ad9081k_jesd204_priv {
	struct ad9081k_dev *device;
};

/******************************************************************************/
/******************** JESD204 FSM Callback Functions **************************/
/******************************************************************************/

/**
 * @brief JESD204 link_init callback — populate link parameters.
 */
static int ad9081k_jesd204_link_init(struct jesd204_dev *jdev,
				     enum jesd204_state_op_reason reason,
				     struct jesd204_link *lnk)
{
	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	printf("ad9081k: [JESD204_OP_LINK_INIT] link_id=%u\n", lnk->link_id);

	switch (lnk->link_id) {
	case AD9081K_DEFRAMER0_LINK_TX:
		lnk->num_lanes = AD9081K_TX_JESD_LANES_PER_DEVICE;
		lnk->num_converters = AD9081K_TX_JESD_CONVS_PER_DEVICE;
		lnk->octets_per_frame = AD9081K_TX_JESD_OCTETS_PER_FRAME;
		lnk->frames_per_multiframe = AD9081K_TX_JESD_FRAMES_PER_MULTIFRAME;
		lnk->bits_per_sample = AD9081K_TX_JESD_BITS_PER_SAMPLE;
		lnk->converter_resolution = AD9081K_TX_JESD_CONV_RESOLUTION;
		lnk->ctrl_bits_per_sample = AD9081K_TX_JESD_CTRL_BITS_PER_SAMPLE;
		lnk->high_density = AD9081K_TX_JESD_HIGH_DENSITY;
		lnk->subclass = JESD204_SUBCLASS_0;
		lnk->jesd_version = JESD204_VERSION_C;
		lnk->jesd_encoder = JESD204_ENCODER_64B66B;
		lnk->is_transmit = true;
		lnk->sample_rate = AD9081K_SAMPLE_CLK_KHZ * 1000ULL;
		lnk->sample_rate_div = AD9081K_TX_JESD_SAMPLES_PER_FRAME;
		break;
	case AD9081K_FRAMER0_LINK_RX:
		lnk->num_lanes = AD9081K_TX_JESD_LANES_PER_DEVICE;
		lnk->num_converters = AD9081K_TX_JESD_CONVS_PER_DEVICE;
		lnk->octets_per_frame = AD9081K_RX_JESD_OCTETS_PER_FRAME;
		lnk->frames_per_multiframe = AD9081K_RX_JESD_FRAMES_PER_MULTIFRAME;
		lnk->bits_per_sample = AD9081K_TX_JESD_BITS_PER_SAMPLE;
		lnk->converter_resolution = AD9081K_TX_JESD_CONV_RESOLUTION;
		lnk->ctrl_bits_per_sample = AD9081K_TX_JESD_CTRL_BITS_PER_SAMPLE;
		lnk->high_density = AD9081K_TX_JESD_HIGH_DENSITY;
		lnk->subclass = JESD204_SUBCLASS_0;
		lnk->jesd_version = JESD204_VERSION_C;
		lnk->jesd_encoder = JESD204_ENCODER_64B66B;
		lnk->is_transmit = false;
		lnk->sample_rate = AD9081K_SAMPLE_CLK_KHZ * 1000ULL;
		lnk->sample_rate_div = AD9081K_TX_JESD_SAMPLES_PER_FRAME;
		break;
	default:
		return -EINVAL;
	}

	printf("  L=%u M=%u F=%u K=%u NP=%u N=%u HD=%u %s\n",
	       lnk->num_lanes, lnk->num_converters,
	       lnk->octets_per_frame, lnk->frames_per_multiframe,
	       lnk->bits_per_sample, lnk->converter_resolution,
	       lnk->high_density,
	       lnk->is_transmit ? "TX" : "RX");

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 link_setup callback — arm MCS.
 */
static int ad9081k_jesd204_link_setup(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason)
{
	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 setup_stage1 callback — fire SYSREF for MCS.
 *
 * Mirrors the adrv903x pattern: call jesd204_sysref_async_force() so
 * the Si5391 resyncs its output dividers, delivering a SYSREF pulse to
 * both the device and the FPGA JESD204 IP.
 * post_state_sysref fires one additional pulse after this callback.
 */
static int ad9081k_jesd204_setup_stage1(struct jesd204_dev *jdev,
					enum jesd204_state_op_reason reason)
{
	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

//	printf("ad9081k: [JESD204_OP_OPT_SETUP_STAGE1] firing SYSREF...\n");
//
//	jesd204_sysref_async_force(jdev);

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 setup_stage2 callback — disarm MCS, run PostMcsInit.
 */
static int ad9081k_jesd204_setup_stage2(struct jesd204_dev *jdev,
					enum jesd204_state_op_reason reason)
{
	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	printf("ad9081k: [JESD204_OP_OPT_SETUP_STAGE2].\n");

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 clks_enable callback — enable framer/deframer clocks.
 */
static int ad9081k_jesd204_clks_enable(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason,
				       struct jesd204_link *lnk)
{
	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	printf("ad9081k: [JESD204_OP_CLOCKS_ENABLE] %s.\n",
	       lnk->link_id == 0 ? "Tx" : lnk->link_id == 2 ? "Rx" : "unknown");

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 link_enable callback — enable JESD204 link.
 */
static int ad9081k_jesd204_link_enable(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason,
				       struct jesd204_link *lnk)
{
	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	printf("ad9081k: [JESD204_OP_LINK_ENABLE] %s.\n",
	       lnk->link_id == 0 ? "Tx" : lnk->link_id == 2 ? "Rx" : "unknown");

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 link_running callback — verify link status.
 */
static int ad9081k_jesd204_link_running(struct jesd204_dev *jdev,
					enum jesd204_state_op_reason reason,
					struct jesd204_link *lnk)
{
	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	printf("ad9081k: [JESD204_OP_LINK_RUNNING] %s.\n",
	       lnk->link_id == 0 ? "Tx" : lnk->link_id == 2 ? "Rx" : "unknown");

	return JESD204_STATE_CHANGE_DONE;
}

/**
 * @brief JESD204 post_running_stage callback — activate signal chain.
 */
static int ad9081k_jesd204_post_running_stage(struct jesd204_dev *jdev,
					      enum jesd204_state_op_reason reason)
{
	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	printf("ad9081k: [JESD204_OP_OPT_POST_RUNNING_STAGE].\n");

	return JESD204_STATE_CHANGE_DONE;
}

/******************************************************************************/
/************************ JESD204 Device Registration *************************/
/******************************************************************************/

static const struct jesd204_dev_data jesd204_ad9081k_init = {
	.state_ops = {
		[JESD204_OP_LINK_INIT] = {
			.per_link = ad9081k_jesd204_link_init,
		},
		[JESD204_OP_LINK_SETUP] = {
			.per_device = ad9081k_jesd204_link_setup,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_OPT_SETUP_STAGE1] = {
			.per_device = ad9081k_jesd204_setup_stage1,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_OPT_SETUP_STAGE2] = {
			.per_device = ad9081k_jesd204_setup_stage2,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_CLOCKS_ENABLE] = {
			.per_link = ad9081k_jesd204_clks_enable,
		},
		[JESD204_OP_LINK_ENABLE] = {
			.per_link = ad9081k_jesd204_link_enable,
			.post_state_sysref = true,
		},
		[JESD204_OP_LINK_RUNNING] = {
			.per_link = ad9081k_jesd204_link_running,
		},
		[JESD204_OP_OPT_POST_RUNNING_STAGE] = {
			.per_device = ad9081k_jesd204_post_running_stage,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
	},

	.max_num_links = 3,
	.sizeof_priv = sizeof(struct ad9081k_jesd204_priv),
};

/******************************************************************************/
/********************** Functions Implementation ******************************/
/******************************************************************************/

/**
 * @brief Initialize AD9081K dummy device.
 *
 * Allocates the device structure and registers with the JESD204 framework.
 * All real hardware interaction is replaced with printf + getchar stubs
 * in each FSM callback.
 *
 * @param device     - Output pointer to the allocated device structure.
 * @param init_param - Initialization parameters (unused in dummy driver).
 *
 * @return 0 on success, negative error code on failure.
 */
int ad9081k_init(struct ad9081k_dev **device,
		 const struct ad9081k_init_param *init_param)
{
	struct ad9081k_jesd204_priv *priv;
	struct ad9081k_dev *dev;
	int ret;

	if (!device)
		return -EINVAL;

	dev = no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	printf("ad9081k: dummy device init — registering JESD204 device\n");

	ret = jesd204_dev_register(&dev->jdev, &jesd204_ad9081k_init);
	if (ret) {
		printf("ad9081k: jesd204_dev_register failed (%d)\n", ret);
		goto error_free;
	}

	priv = jesd204_dev_priv(dev->jdev);
	priv->device = dev;

	dev->is_initialized = true;
	*device = dev;

	printf("ad9081k: dummy device init complete\n");
	return 0;

error_free:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Free resources allocated by ad9081k_init().
 *
 * @param dev - Pointer to device structure.
 *
 * @return 0 on success, negative error code on failure.
 */
int ad9081k_remove(struct ad9081k_dev *dev)
{
	if (!dev)
		return -EINVAL;

	jesd204_dev_unregister(dev->jdev);
	no_os_free(dev);

	printf("ad9081k: dummy device removed\n");
	return 0;
}
