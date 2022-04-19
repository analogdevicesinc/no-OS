/***************************************************************************//**
 *   @file   iio_adc_demo_trig.c
 *   @brief  Implementation of iio_adc_demo_trig.c
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "no_os_error.h"
#include "iio.h"
#include "iio_adc_demo.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
static int iio_attr_trigger_now(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv);

/******************************************************************************/
/************************ Variable Declarations *******************************/
/******************************************************************************/
static struct iio_attribute iio_attr_trig_attributes[] = {
	{
		.name = "trigger_now",
		.store = iio_attr_trigger_now
	},
	END_ATTRIBUTES_ARRAY
};

struct iio_trigger adc_demo_iio_software_trigger_desc = {
	.is_synchronous = false,
	.attributes = iio_attr_trig_attributes,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Handles the write request for trigger_now attribute.
 *
 * @param trig    - The iio trigger structure.
 * @param buf     - Command buffer to be filled with the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info (is NULL).
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the writing procedure.
*******************************************************************************/
static int iio_attr_trigger_now(void *trig, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct adc_demo_trig *desc = trig;
	iio_process_trigger_type(*desc->iio_desc, desc->name);

	return 0;
}

/***************************************************************************//**
 * @brief Initialize adc_demo software trigger.
 *
 * @param iio_trig   - The iio trigger structure.
 * @param init_param - The structure that contains the sw trigger initial params.
 *
 * @return ret       - Result of the initialization procedure.
*******************************************************************************/
int32_t adc_demo_software_trigger_init(struct adc_demo_trig **iio_trig,
				       struct adc_demo_sw_trig_init_param *init_param)
{
	struct adc_demo_trig *trig_desc;

	if (!init_param->iio_desc || !init_param->name)
		return -EINVAL;
	trig_desc = (struct adc_demo_trig*)calloc(1, sizeof(*trig_desc));
	if (!trig_desc)
		return -ENOMEM;

	trig_desc->iio_desc = init_param->iio_desc;

	strncpy(trig_desc->name, init_param->name, TRIG_MAX_NAME_SIZE);

	*iio_trig = trig_desc;

	return 0;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adc_demo_trigger_init().
 *
 * @param trig - The trigger structure.
*******************************************************************************/
void adc_demo_trigger_remove(struct adc_demo_trig *trig)
{
	free(trig);
}
