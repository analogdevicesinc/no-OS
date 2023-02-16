/***************************************************************************//**
 *   @file   no_os_tdm.c
 *   @brief  Implementation of the TDM interface
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
