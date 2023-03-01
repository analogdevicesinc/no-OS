/***************************************************************************//**
 *   @file   aducm3029_trng.c
 *   @brief  Implementation of true random number generator
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 *   @copyright
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdlib.h>
#include "aducm3029_trng.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

/******************************************************************************/
/**************************** Global Variables ********************************/
/******************************************************************************/

/** Number of references to the descriptor */
static uint32_t		nb_references;
/** TRNG Descriptor */
static struct aducm3029_trng_desc	g_desc;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/* Initialize the TRNG device and the global descriptor */
static void aducm3029_gdesc_init()
{
	adi_rng_Open(NO_OS_ADUCM_TRNG_DEVICE_ID, g_desc.dev_mem, ADI_RNG_MEMORY_SIZE,
		     &g_desc.dev);

	adi_rng_SetSampleLen(g_desc.dev, NO_OS_TRNG_PRESCALER, NO_OS_TRNG_CNT_VAL);

	adi_rng_EnableBuffering(g_desc.dev, false);

	adi_rng_Enable(g_desc.dev, true);
}

/* Remove the TRNG device and the global descriptor */
static void aducm3029_gdesc_remove()
{
	adi_rng_Enable(g_desc.dev, false);
	adi_rng_Close(g_desc.dev);
}

/**
 * @brief Initialize descriptor
 * @param desc - Where to store the TRNG Descriptor
 * @param param - Unused
 * @return 0 or -1 if desc is null
 */
int aducm3029_trng_init(struct no_os_trng_desc **desc,
			struct no_os_trng_init_param *param)
{
	struct no_os_trng_desc *trng_desc;

	NO_OS_UNUSED_PARAM(param);

	if (!desc)
		return -1;

	trng_desc = (struct no_os_trng_desc *)no_os_calloc(1, sizeof(*trng_desc));
	if (!trng_desc)
		return -1;

	if (!nb_references)
		aducm3029_gdesc_init();

	trng_desc->extra = &g_desc;

	nb_references++;
	*desc = trng_desc;

	return 0;
}

/**
 * @brief Free resources allocated in descriptor
 * @param desc - TRNG Descriptor
 */
int aducm3029_trng_remove(struct no_os_trng_desc *desc)
{
	if (!desc || !desc->extra)
		return -1;

	nb_references--;
	if (nb_references == 0)
		aducm3029_gdesc_remove();

	no_os_free(desc);

	return 0;
}

/**
 * @brief Fill buffer with random numbers
 * @param desc - TRNG descriptor
 * @param buff - Buffer to be filled
 * @param len - Size of the buffer
 * @return 0 if no errors, -1 is an error occurs and the
 * data in the buffer is not useful.
 */
int aducm3029_trng_fill_buffer(struct no_os_trng_desc *desc, uint8_t *buff,
			       uint32_t len)
{
	uint32_t	data;
	uint32_t	i;
	bool		ready;
	bool		stuck;
	struct aducm3029_trng_desc *aducm_desc;

	aducm_desc = desc->extra;

	for (i = 0; i < len; i++) {
		ready = 0;
		while (!ready)
			adi_rng_GetRdyStatus(aducm_desc->dev, &ready);
		adi_rng_GetStuckStatus(aducm_desc->dev, &stuck);
		if (stuck) {
			/* This is needed in order to clear the stuck bit */
			no_os_gdesc_remove();
			no_os_gdesc_init();
			return -1;
		}
		adi_rng_GetRngData(aducm_desc->dev, &data);
		buff[i] = data & 0xFF;
	}

	return 0;
}

/**
 * @brief ADuCM3029 platform specific TRNG platform ops structure
 */
const struct no_os_trng_platform_ops aducm_trng_ops = {
	.init = &aducm3029_trng_init,
	.fill_buffer = &aducm3029_trng_fill_buffer,
	.remove = &aducm3029_trng_remove
};
