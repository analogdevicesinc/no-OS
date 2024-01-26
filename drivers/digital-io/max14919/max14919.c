/***************************************************************************//**
 *   @file   max14919.c
 *   @brief  Source file of MAX14919 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "max14919.h"
#include "no_os_alloc.h"

/**
 * @brief Set the OUT channels state
 * @param desc - MAX14919 device descriptor
 * @param state - OUT channels state array containing information about the to
 * 		  be set channels state.
 * @return 0 in case of succes, negative error code otherwise
*/
int max14919_set_out(struct max14919_desc *desc, enum max14919_out_state *state)
{
	int ret, i;

	if (!desc)
		return -ENODEV;

	if (!desc->in_desc || !desc->out_state)
		return -EINVAL;

	for (i = 0; i < MAX14919_OUT_CHANNELS; i++) {
		ret = no_os_gpio_set_value(desc->in_desc[i],
					   state[i] ? NO_OS_GPIO_LOW : NO_OS_GPIO_HIGH);
		if (ret)
			return ret;

		desc->out_state[i] = state[i];
	}

	return 0;
}

/**
 * @brief Set current limiting
 * @param desc - MAX14919 devce descriptor
 * @param state - State to be set.
 * 		  TRUE - on.
 * 		  FALSE - off.
 * @return 0 in case of succes, negative error code otherwise
*/
int max14919_set_climit(struct max14919_desc *desc, bool state)
{
	if (!desc)
		return -ENODEV;

	if (!desc->inrush_desc)
		return -EINVAL;

	return no_os_gpio_set_value(desc->inrush_desc,
				    state ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
}

/**
 * @brief Get the FAULT state of the device
 * @param desc - MAX14919 device descriptor
 * @param fault - Fault state of the MAX14919
 * @return 0 in case of succes, negative error code otherwise.
*/
int max14919_get_fault(struct max14919_desc *desc, uint8_t* fault)
{
	if (!desc)
		return -ENODEV;

	if (!desc->fault_desc)
		return -EINVAL;

	return no_os_gpio_get_value(desc->fault_desc, fault);
}

/**
 * @brief Detect reverse polarity of the current
 * @param desc - MAX14919 device descriptor
 * @param rev - Reverse polarity state
 * @return 0 in case of succes, negative error code otherwise.
*/
int max14919_detect_rev(struct max14919_desc *desc, uint8_t* rev)
{
	if (!desc)
		return -ENODEV;

	if (!desc->rev_desc)
		return -EINVAL;

	return no_os_gpio_get_value(desc->rev_desc, rev);
}

/**
 * @brief Initialize the MAX14919 device descriptor
 * @param desc - MAX14919 device descriptor
 * @param init_param - MAX14919 initialization parameter
 * @return 0 in case of succes, negative error code otherwise
*/
int max14919_init(struct max14919_desc **desc,
		  struct max14919_init_param *init_param)
{
	struct max14919_desc *descriptor;
	int ret, i;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	for (i = 0; i < MAX14919_OUT_CHANNELS; i++) {
		ret = no_os_gpio_get(&descriptor->in_desc[i], init_param->in_param[i]);
		if (ret)
			goto error;

		ret = no_os_gpio_direction_output(descriptor->in_desc[i], NO_OS_GPIO_HIGH);
		if (ret)
			goto error;

		descriptor->out_state[i] = MAX14919_OUT_OFF;
	}

	ret = no_os_gpio_get_optional(&descriptor->inrush_desc,
				      init_param->inrush_param);
	if (ret)
		goto error;

	if (descriptor->inrush_desc) {
		ret = no_os_gpio_direction_output(descriptor->inrush_desc, NO_OS_GPIO_LOW);
		if (ret)
			goto error;
	}

	ret = no_os_gpio_get_optional(&descriptor->rev_desc, init_param->rev_param);
	if (ret)
		goto error;

	if (descriptor->rev_desc) {
		ret = no_os_gpio_direction_input(descriptor->rev_desc);
		if (ret)
			goto error;
	}

	ret = no_os_gpio_get(&descriptor->fault_desc, init_param->fault_param);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_input(descriptor->fault_desc);
	if (ret)
		goto error;

	*desc = descriptor;

	return 0;

error:
	max14919_remove(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param desc - MAX14919 device descriptor
 * @return 0 in case of succes, negative error code otherwise
*/
int max14919_remove(struct max14919_desc *desc)
{
	int i;

	if (!desc)
		return -ENODEV;

	for (i = 0; i < MAX14919_OUT_CHANNELS; i++) {
		no_os_gpio_remove(desc->in_desc[i]);
	}
	no_os_gpio_remove(desc->inrush_desc);
	no_os_gpio_remove(desc->rev_desc);
	no_os_gpio_remove(desc->fault_desc);
	no_os_free(desc);

	return 0;
}
