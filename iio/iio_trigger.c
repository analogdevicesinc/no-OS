/***************************************************************************//**
 *   @file   iio_trigger.c
 *   @brief  Implementation of generic iio trigger.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
*/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "iio.h"
#include "iio_trigger.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
#ifndef LINUX_PLATFORM
/**
 * @brief Initialize hardware trigger.
 *
 * @param iio_trig   - The iio trigger structure.
 * @param init_param - The structure that contains the trigger initial params.
 *
 * @return ret       - Result of the initialization procedure.
 */
int iio_hw_trig_init(struct iio_hw_trig **iio_trig,
		     struct iio_hw_trig_init_param *init_param)
{
	struct iio_hw_trig *trig_desc;
	int ret;

	if (!init_param->name)
		return -EINVAL;

	trig_desc = (struct iio_hw_trig*)no_os_calloc(1, sizeof(*trig_desc));
	if (!trig_desc)
		return -ENOMEM;

	trig_desc->iio_desc = init_param->iio_desc;
	strncpy(trig_desc->name, init_param->name, TRIG_MAX_NAME_SIZE);
	trig_desc->irq_ctrl = init_param->irq_ctrl;
	trig_desc->irq_id = init_param->irq_id;
	trig_desc->irq_trig_lvl = init_param->irq_trig_lvl;

	struct no_os_callback_desc irq_cb = {
		.callback = iio_hw_trig_handler,
		.ctx = trig_desc,
		.event = init_param->cb_info.event,
		.handle = init_param->cb_info.handle,
		.peripheral = init_param->cb_info.peripheral
	};

	ret = no_os_irq_register_callback(trig_desc->irq_ctrl,
					  trig_desc->irq_id, &irq_cb);
	if (ret)
		goto error;

	if (init_param->cb_info.event == NO_OS_EVT_GPIO) {
		ret = no_os_irq_trigger_level_set(trig_desc->irq_ctrl,
						  trig_desc->irq_id, trig_desc->irq_trig_lvl);
		if (ret)
			goto error;
	}

	*iio_trig = trig_desc;

	return 0;
error:
	no_os_free(trig_desc);
	return ret;
}

/**
 * @brief Enable system interrupt which is linked to the given trigger.
 *
 * @param trig - Trigger structure.
 *
 * @return ret - Result of the enable procedure.
*/
int iio_trig_enable(void *trig)
{
	if(!trig)
		return -EINVAL;

	struct iio_hw_trig *desc = trig;

	return no_os_irq_enable(desc->irq_ctrl, desc->irq_id);
}

/**
 * @brief Disable system interrupt which is linked to the given trigger.
 *
 * @param trig - Trigger structure.
 *
 * @return ret - Result of the disable procedure.
*/
int iio_trig_disable(void *trig)
{
	if(!trig)
		return -EINVAL;

	struct iio_hw_trig *desc = trig;

	return no_os_irq_disable(desc->irq_ctrl, desc->irq_id);
}

/**
 * @brief Trigger interrupt handler. This function will be called when a system
 * interrupt is asserted for the configured trigger.
 *
 * @param trig - Trigger structure which is linked to this handler.
*/
void iio_hw_trig_handler(void *trig)
{
	if(!trig)
		return;

	struct iio_hw_trig *desc = trig;

	iio_process_trigger_type(desc->iio_desc, desc->name);
}

/**
 * @brief Free the resources allocated by iio_hw_trig_init().
 *
 * @param trig - The trigger structure.
 *
 * @return ret - Result of the remove procedure.
*/
int iio_hw_trig_remove(struct iio_hw_trig *trig)
{
	if(trig)
		no_os_free(trig);

	return 0;
}
#endif

/**
 * @brief Initialize software trigger.
 *
 * @param iio_trig   - The iio trigger structure.
 * @param init_param - The structure that contains the sw trigger initial params.
 *
 * @return ret       - Result of the initialization procedure.
*/
int iio_sw_trig_init(struct iio_sw_trig **iio_trig,
		     struct iio_sw_trig_init_param *init_param)
{
	struct iio_sw_trig *trig_desc;

	if (!init_param->iio_desc || !init_param->name)
		return -EINVAL;

	trig_desc = (struct iio_sw_trig*)no_os_calloc(1, sizeof(*trig_desc));
	if (!trig_desc)
		return -ENOMEM;

	trig_desc->iio_desc = init_param->iio_desc;
	strncpy(trig_desc->name, init_param->name, TRIG_MAX_NAME_SIZE);

	*iio_trig = trig_desc;

	return 0;
}

/**
 * @brief Handles the write request for trigger_now attribute.
 *
 * @param trig    - The iio trigger structure.
 * @param buf     - Command buffer to be filled with the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info (is NULL).
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the sw trigger handler procedure.
*/
int iio_sw_trig_handler(void *trig, char *buf, uint32_t len,
			const struct iio_ch_info *channel,
			intptr_t priv)
{
	if(!trig)
		return -EINVAL;

	struct iio_sw_trig *desc = trig;

	return iio_process_trigger_type(desc->iio_desc, desc->name);
}

/**
 * @brief Free the resources allocated by iio_sw_trig_init().
 *
 * @param trig - The trigger structure.
 *
 * @return ret - Result of the remove procedure.
*/
int iio_sw_trig_remove(struct iio_sw_trig *trig)
{
	if(trig)
		no_os_free(trig);

	return 0;
}
