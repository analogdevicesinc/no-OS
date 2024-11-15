/***************************************************************************//**
 *   @file   no_os_tdm.c
 *   @brief  Implementation of the TDM interface
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <inttypes.h>
#include "no_os_tdm.h"
#include <stdlib.h>
#include "no_os_error.h"

/**
 * @brief Initialize the TDM communication peripheral.
 * @param desc - The TDM descriptor.
 * @param param - The structure that contains the TDM parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t  no_os_tdm_init(struct no_os_tdm_desc **desc,
			const struct no_os_tdm_init_param *param)
{
	if (!param)
		return -1;

	if ((param->platform_ops->tdm_ops_init(desc, param)))
		return -1;

	(*desc)->platform_ops = param->platform_ops;

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_tdm_init().
 * @param desc - The TDM descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t  no_os_tdm_remove(struct no_os_tdm_desc *desc)
{
	return desc->platform_ops->tdm_ops_remove(desc);
}

/**
 * @brief Read data using the TDM interface
 * @param desc - The TDM descriptor.
 * @param data - The buffer to store the received data.
 * @param nb_samples - Number of samples to read.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t  no_os_tdm_read(struct no_os_tdm_desc *desc,
			void *data,
			uint16_t nb_samples)
{
	return desc->platform_ops->tdm_ops_read(desc, data, nb_samples);
}

/**
 * @brief Pause TDM DMA transfer
 * @param desc - The TDM descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t  no_os_tdm_pause(struct no_os_tdm_desc *desc)
{
	return desc->platform_ops->tdm_ops_pause(desc);
}

/**
 * @brief Resume TDM DMA transfer
 * @param desc - The TDM descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t  no_os_tdm_resume(struct no_os_tdm_desc *desc)
{
	return desc->platform_ops->tdm_ops_resume(desc);
}

/**
 * @brief Stop TDM DMA transfer
 * @param desc - The TDM descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t  no_os_tdm_stop(struct no_os_tdm_desc *desc)
{
	return desc->platform_ops->tdm_ops_stop(desc);
}

/**
 * @brief Write data using the TDM interface
 * @param desc - The TDM descriptor.
 * @param data - The buffer with the data to be transmitted.
 * @param nb_samples - Number of samples to write.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t  no_os_tdm_write(struct no_os_tdm_desc *desc,
			 void *data,
			 uint16_t nb_samples)
{
	return desc->platform_ops->tdm_ops_write(desc, data, nb_samples);
}
