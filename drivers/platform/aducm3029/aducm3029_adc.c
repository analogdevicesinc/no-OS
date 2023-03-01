/***************************************************************************//**
 *   @file   aducm3029_adc.c
 *   @brief  Implementation of aducm3029_adc.c
 *   @author Mihail Chindris (mihail.chindris@analog.com)
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

#include <drivers/adc/adi_adc.h>
#include "aducm3029_adc.h"
#include "no_os_error.h"
#include <stdio.h>
#include <stdlib.h>
#include "no_os_util.h"
#include "no_os_alloc.h"

#define ADUCM3029_DEV_ID 0

/**
 * @struct adc_desc
 * @brief Adc descriptor
 */
struct adc_desc {
	/** Memory needed by bsp */
	uint8_t		dev_mem[ADI_ADC_MEMORY_SIZE];
	/** BSP handler */
	ADI_ADC_HANDLE	dev;
	/** Active channels. Each bit represents a channel */
	uint32_t	ch_mask;
};

/**
 * @brief Activate adc channels
 * @param desc - Adc descriptor
 * @param mask - Channels to activates. Use ADUCM3029_CH define
 * @return 0 in case of success, negative value otherwise.
 */
int32_t aducm3029_adc_update_active_channels(struct adc_desc *desc,
		uint32_t mask)
{
	if (!desc)
		return -EINVAL;

	desc->ch_mask = mask;

	return 0;
}

/**
 * @brief Read adc data. aducm3029_adc_update_active_channels should be called
 * in order to enable channeles.
 * @param desc - Adc descriptor
 * @param buff - Buffer where to store data. The available size should be
 * number of activated channels * nb_samples.
 * @param nb_samples - Number of samples to read for each channel.
 * @return 0 in case of success, negative value otherwise.
 */
int32_t aducm3029_adc_read(struct adc_desc *desc, uint16_t *buff,
			   uint32_t nb_samples)
{
	ADI_ADC_BUFFER	adi_buff, *pbuff;
	int32_t		ret;
	uint32_t	i;

	if (!desc)
		return -EINVAL;

	for (i = 0; i < nb_samples; i++) {
		adi_buff.nChannels = desc->ch_mask;
		adi_buff.pDataBuffer = buff + i * no_os_hweight32(desc->ch_mask);
		adi_buff.nNumConversionPasses = 1;
		adi_buff.nBuffSize = no_os_hweight32(desc->ch_mask) * sizeof(uint16_t);
		ret = adi_adc_SubmitBuffer(desc->dev, &adi_buff);
		if (ret != ADI_ADC_SUCCESS)
			return -ret;
		ret = adi_adc_Enable(desc->dev, true);
		if (ret != ADI_ADC_SUCCESS)
			return -ret;
		ret = adi_adc_GetBuffer(desc->dev, &pbuff);
		if (ret != ADI_ADC_SUCCESS)
			return -ret;
		ret = adi_adc_Enable(desc->dev, false);
		if (ret != ADI_ADC_SUCCESS)
			return -ret;
	}

	return 0;
}

/**
 * @brief Allocate adc_desc and initialize adc
 * @param desc - Adc descriptor
 * @param param - Initialization parameter
 * @return 0 in case of success, negative value otherwise.
 */
int32_t aducm3029_adc_init(struct adc_desc **desc, struct adc_init_param *param)
{
	NO_OS_UNUSED_PARAM(param);

	ADI_ADC_RESULT	ret;
	struct adc_desc	*ldesc;
	bool		ready;

	if (!desc)
		return -EINVAL;

	ldesc = (struct adc_desc *)no_os_calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -ENOMEM;

	ret = adi_adc_Open(ADUCM3029_DEV_ID, ldesc->dev_mem,
			   ADI_ADC_MEMORY_SIZE, &ldesc->dev);
	if (ret != ADI_ADC_SUCCESS)
		goto free_desc;

	ret = adi_adc_PowerUp(ldesc->dev, true);
	if (ret != ADI_ADC_SUCCESS)
		goto close_adc;

	ret = adi_adc_SetVrefSource(ldesc->dev, ADI_ADC_VREF_SRC_INT_2_50_V);
	if (ret != ADI_ADC_SUCCESS)
		goto close_adc;

	ret = adi_adc_EnableADCSubSystem(ldesc->dev, true);
	if (ret != ADI_ADC_SUCCESS)
		goto close_adc;

	do {
		ret = adi_adc_IsReady(ldesc->dev, &ready);
		if (ret != ADI_ADC_SUCCESS)
			goto close_adc;
	} while (!ready);

	ret = adi_adc_StartCalibration(ldesc->dev);
	if (ret != ADI_ADC_SUCCESS)
		goto close_adc;
	do {
		ret = adi_adc_IsCalibrationDone(ldesc->dev, &ready);
		if (ret != ADI_ADC_SUCCESS)
			goto close_adc;
	} while (!ready);

	*desc = ldesc;

	return 0;
close_adc:
	adi_adc_PowerUp(ldesc->dev, false);
	adi_adc_Close(ldesc->dev);
free_desc:
	no_os_free(ldesc);
	*desc = NULL;
	return -ret;
}


/**
 * @brief Dealocate resources allocated by aducm3029_adc_init
 * @param desc - Adc descriptor
 * @return 0
 */
int32_t aducm3029_adc_remove(struct adc_desc *desc)
{
	adi_adc_EnableADCSubSystem(desc->dev, false);
	adi_adc_PowerUp(desc->dev, false);
	adi_adc_Close(desc->dev);

	no_os_free(desc);

	return 0;
}
