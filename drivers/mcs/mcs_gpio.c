/***************************************************************************//**
 *   @file   mcs_gpio.c
 *   @brief  Implementation of MCS_GPIO Driver.
 *   @author GGMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include <stdlib.h>
#include <stdio.h>
#include "no_os_print_log.h"
#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "mcs_gpio.h"
#include "jesd204.h"

struct mcs_gpio_jesd204_priv {
	struct mcs_gpio_dev *mcs_gpio;
};

static int mcs_gpio_jesd204_clks_sync2(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason)

{
	struct mcs_gpio_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct mcs_gpio_dev *mcs_gpio = priv->mcs_gpio;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	no_os_gpio_set_value(mcs_gpio->gpio_req, 1);
	no_os_gpio_set_value(mcs_gpio->gpio_req, 0);

	return JESD204_STATE_CHANGE_DONE;
}

static const struct jesd204_dev_data jesd204_mcs_gpio_init = {
	.state_ops = {
		[JESD204_OP_CLK_SYNC_STAGE2] = {
			.per_device = mcs_gpio_jesd204_clks_sync2,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
	},
};


/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mcs_gpio_init(struct mcs_gpio_dev **device,
		     const struct mcs_gpio_init_param *init_param)
{
	struct mcs_gpio_jesd204_priv *priv;
	struct mcs_gpio_dev *dev;
	int32_t ret;

	dev = (struct mcs_gpio_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -1;

	ret = no_os_gpio_get(&dev->gpio_req, init_param->gpio_req);
	if (ret < 0)
		return ret;
	if (dev->gpio_req)
		no_os_gpio_direction_output(dev->gpio_req, 0);

	ret = jesd204_dev_register(&dev->jdev, &jesd204_mcs_gpio_init);
	if (ret < 0)
		return ret;
	priv = jesd204_dev_priv(dev->jdev);
	priv->mcs_gpio = dev;

	*device = dev;

	return 0;
}

/**
 * Remove the device - release resources.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mcs_gpio_remove(struct mcs_gpio_dev *device)
{
	int32_t ret;

	ret = no_os_gpio_remove(device->gpio_req);
	no_os_free(device);

	return ret;
}
