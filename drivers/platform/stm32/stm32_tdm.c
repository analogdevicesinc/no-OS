/***************************************************************************//**
 *   @file   stm32/stm32_tdm.c
 *   @brief  Implementation of TDM driver using STM32's SAI peripheral.
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
#include <stdlib.h>
#include <errno.h>
#include "gpio.h"
#include "stm32_gpio.h"
#include "tdm.h"
#include "stm32_tdm.h"
#include "error.h"

/**
 * @brief stm32 platform specific TDM platform ops structure
 */
const struct tdm_platform_ops stm32_tdm_platform_ops = {
	.tdm_ops_init = &stm32_tdm_init,
	.tdm_ops_read = &stm32_tdm_read,
	.tdm_ops_remove = &stm32_tdm_remove
};

/**
 * @brief Initialize the TDM communication peripheral.
 * @param desc - The TDM descriptor.
 * @param param - The structure that contains the TDM parameters.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t stm32_tdm_init(struct tdm_desc **desc,
		       const struct tdm_init_param *param)
{
	int32_t ret;
	uint32_t tmp;
	struct tdm_desc	*tdm_desc;

	if (!desc || !param)
		return -EINVAL;

	if (param->mode > TDM_SLAVE_RX)
		return -EINVAL;

	tdm_desc = (struct tdm_desc *)calloc(1, sizeof(*tdm_desc));
	if (!tdm_desc)
		return -ENOMEM;

	struct stm32_tdm_desc *tdesc;
	struct stm32_tdm_init_param *tinit;

	tdesc = (stm32_tdm_desc*)calloc(1,sizeof(stm32_tdm_desc));
	if (!tdesc) {
		ret = -ENOMEM;
		goto error;
	}

	tdm_desc->extra = tdesc;
	tinit = param->extra;

	tdesc->hsai.Instance = tinit->base;
	tdesc->hsai.Init.Protocol = SAI_FREE_PROTOCOL;
	switch(param->mode) {
	case TDM_MASTER_TX:
	case TDM_MASTER_RX:
	case TDM_SLAVE_TX:
		// TODO: implement master tx/rx and slave tx
		ret = -ENOTSUP;
		goto error;
	case TDM_SLAVE_RX:
		tmp = SAI_MODESLAVE_RX;
		break;
	default:
		ret = -EINVAL;
		break;
	};
	tdesc->hsai.Init.AudioMode = tmp;
	switch(param->data_size) {
	case 8:
		tmp = SAI_DATASIZE_8;
		break;
	case 10:
		tmp = SAI_DATASIZE_10;
		break;
	case 16:
		tmp = SAI_DATASIZE_16;
		break;
	case 20:
		tmp = SAI_DATASIZE_20;
		break;
	case 24:
		tmp = SAI_DATASIZE_24;
		break;
	case 32:
		tmp = SAI_DATASIZE_32;
		break;
	default:
		ret = -ENOTSUP;
		goto error;
		break;
	};
	tdesc->hsai.Init.DataSize = tmp;
	tdesc->hsai.Init.FirstBit = param->data_lsb_first ? SAI_FIRSTBIT_LSB :
				    SAI_FIRSTBIT_MSB;
	tdesc->hsai.Init.ClockStrobing = param->rising_edge_sampling ?
					 SAI_CLOCKSTROBING_RISINGEDGE : SAI_CLOCKSTROBING_FALLINGEDGE;
	tdesc->hsai.Init.Synchro = SAI_ASYNCHRONOUS;
	tdesc->hsai.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
	tdesc->hsai.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
	tdesc->hsai.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
	tdesc->hsai.Init.MonoStereoMode = SAI_STEREOMODE;
	tdesc->hsai.Init.CompandingMode = SAI_NOCOMPANDING;
	tdesc->hsai.Init.TriState = SAI_OUTPUT_NOTRELEASED;
	tdesc->hsai.FrameInit.FrameLength = (param->data_size + param->data_offset) *
					    param->slots_per_frame;
	tdesc->hsai.FrameInit.ActiveFrameLength = param->fs_active_length;
	tdesc->hsai.FrameInit.FSDefinition = SAI_FS_STARTFRAME;
	tdesc->hsai.FrameInit.FSPolarity = param->fs_active_low ? SAI_FS_ACTIVE_LOW :
					   SAI_FS_ACTIVE_HIGH;
	tdesc->hsai.FrameInit.FSOffset = param->fs_lastbit ? SAI_FS_BEFOREFIRSTBIT :
					 SAI_FS_FIRSTBIT;
	tdesc->hsai.SlotInit.FirstBitOffset = param->data_offset;
	tdesc->hsai.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
	tdesc->hsai.SlotInit.SlotNumber = param->slots_per_frame;
	tdesc->hsai.SlotInit.SlotActive = (1u << param->slots_per_frame) - 1u;
	if (HAL_SAI_Init(&tdesc->hsai) != HAL_OK) {
		ret = -EIO;
		goto error;
	}

	*desc = tdm_desc;

	return SUCCESS;
error:
	free(tdm_desc);
	free(tdesc);
	return ret;
}

/**
 * @brief Free the resources allocated by tdm_init().
 * @param desc - The TDM descriptor.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t stm32_tdm_remove(struct tdm_desc *desc)
{
	struct stm32_tdm_desc *tdesc;

	if (!desc || !desc->extra)
		return -EINVAL;

	tdesc = desc->extra;
	HAL_SAI_DeInit(&tdesc->hsai);
	free(desc->extra);
	free(desc);

	return SUCCESS;
}

/**
 * @brief Read data using SAI TDM mode.
 * @param desc - The TDM descriptor.
 * @param data - The buffer to fill with the received data.
 * @param nb_samples - Number of samples to read.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t stm32_tdm_read(struct tdm_desc *desc,
		       void *data,
		       uint16_t nb_samples)
{
	int32_t	ret;
	struct stm32_tdm_desc *tdesc;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	if (!nb_samples)
		return SUCCESS;

	tdesc = desc->extra;

	ret = HAL_SAI_Receive(&tdesc->hsai, data, nb_samples, HAL_MAX_DELAY);
	if (ret != HAL_OK) {
		if (ret == HAL_TIMEOUT)
			ret = -ETIMEDOUT;
		else
			ret = -EIO;
	}

	return ret;
}
