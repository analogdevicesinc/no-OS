/***************************************************************************//**
 * @file dac_demo.c
 * @brief Implementation of DAC Demo Driver.
 * @author RNechita (ramona.nechita@analog.com)
 ********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "dac_demo.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief init function for the dac demo driver
 * @param desc - descriptor for the dac
 * @param param - initialization param for dac
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t dac_demo_init(struct dac_demo_desc **desc,
		      struct dac_demo_init_param *param)
{
	struct dac_demo_desc *adesc;
	adesc = (struct dac_demo_desc*)no_os_calloc(1, sizeof(*adesc));

	if(!adesc)
		return -ENOMEM;

	adesc->loopback_buffers = param->loopback_buffers;
	adesc->loopback_buffer_len = param->loopback_buffer_len;

	for(int i = 0; i < TOTAL_DAC_CHANNELS; i++)
		adesc->dac_ch_attr[i] = param->dev_ch_attr[i];
	adesc->dac_global_attr = param->dev_global_attr;
	*desc = adesc;
	return 0;
}

/****************************************************************************//**
 * @brief free allocated resources
 * @param desc - descriptor for the dac to be removed
 * @return 0 in case of success, -1 otherwise.
******************************************************************************/
int32_t dac_demo_remove(struct dac_demo_desc *desc)
{
	if(!desc)
		return -EINVAL;

	no_os_free(desc);

	return 0;
}

/***************************************************************************//**
 * @brief update number of active channels
 * @param dev - physical instance of a dac device
 * @param mask - the new number of active channels
 * @return 0 in case of success.
*******************************************************************************/
int32_t update_dac_channels(void *dev, int32_t mask)
{
	struct dac_demo_desc *desc;
	if(!dev)
		return -ENODEV;

	desc = dev;

	desc->active_ch = mask;

	return 0;
}

/**********************************************************************//**
 * @brief close all channels
 * @param dev - physical instance of an adc device
 * @return 0 in case of success.
**************************************************************************/
int32_t close_dac_channels(void* dev)
{
	struct dac_demo_desc *desc;
	if(!dev)
		return -ENODEV;

	desc = dev;

	desc->active_ch = 0;

	return 0;
}

/**********************************************************************//**
 * @brief read function for the dac demo driver
 * @param desc - descriptor for the dac
 * @param reg_index - the address at which we want to read
 * @param readval- the value read from register
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t dac_demo_reg_read(struct dac_demo_desc *desc, uint8_t reg_index,
			  uint8_t *readval)
{
	if(!desc || reg_index >= NO_OS_ARRAY_SIZE(desc->reg))
		return -EINVAL;

	*readval = desc->reg[reg_index];

	return 0;
}

/***************************************************************************//**
 * @brief write function for the dac demo driver
 * @param desc - descriptor for the dac
 * @param reg_index - the address at which we want to write
 * @param writeval - the value to be written
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t dac_demo_reg_write(struct dac_demo_desc *desc, uint8_t reg_index,
			   uint8_t writeval)
{
	if(!desc || reg_index >= NO_OS_ARRAY_SIZE(desc->reg))
		return -EINVAL;

	desc->reg[reg_index] = writeval;

	return 0;
}
