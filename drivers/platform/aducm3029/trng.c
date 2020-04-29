/***************************************************************************//**
 *   @file   trng.c
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
#include <drivers/rng/adi_rng.h>
#include "trng.h"
#include "util.h"
#include "error.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* LenReload: 0 - 4095 */
#define TRNG_CNT_VAL		4095
/* Prescaler: 0 - 10 */
#define TRNG_PRESCALER		2
/* Aducm device ID */
#define ADUCM_TRNG_DEVICE_ID	0

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* Stucture holding the TRNG descriptor. */
struct trng_desc {
	/*
	 * Memory used by the DFP
	 * At least ADI_RNG_MEMORY_SIZE bytes of 4 bytes aligned memory are
	 * needed by the DFP driver. The formula is to align memory only.
	 */
	uint32_t	dev_mem[(ADI_RNG_MEMORY_SIZE + 3)/4];
	/* DFP Hanler */
	ADI_RNG_HANDLE	dev;
};

/******************************************************************************/
/**************************** Global Variables ********************************/
/******************************************************************************/

/** Number of references to the descriptor */
static uint32_t		nb_references;
/** TRNG Descriptor */
static struct trng_desc	g_desc;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/* Initialize the TRNG device and the global descriptor */
static void gdesc_init()
{
	adi_rng_Open(ADUCM_TRNG_DEVICE_ID, g_desc.dev_mem, ADI_RNG_MEMORY_SIZE,
		     &g_desc.dev);

	adi_rng_SetSampleLen(g_desc.dev, TRNG_PRESCALER, TRNG_CNT_VAL);

	adi_rng_EnableBuffering(g_desc.dev, false);

	adi_rng_Enable(g_desc.dev, true);
}

/* Remove the TRNG device and the global descriptor */
static void gdesc_remove()
{
	adi_rng_Enable(g_desc.dev, false);
	adi_rng_Close(g_desc.dev);
}

/**
 * @brief Initialize descriptor
 * @param desc - Where to store the TRNG Descriptor
 * @param param - Unused
 * @return \ref SUCCESS or \ref FAILURE if desc is null
 */
int32_t trng_init(struct trng_desc **desc, struct trng_init_param *param)
{
	UNUSED_PARAM(param);

	if (!desc)
		return FAILURE;

	if (!nb_references)
		gdesc_init();

	nb_references++;
	*desc = &g_desc;

	return SUCCESS;
}

/**
 * @brief Free resources allocated in descriptor
 * @param desc - TRNG Descriptor
 */
void trng_remove(struct trng_desc *desc)
{
	nb_references--;
	if (nb_references == 0)
		gdesc_remove();
}

/**
 * @brief Fill buffer with random numbers
 * @param desc - TRNG descriptor
 * @param buff - Buffer to be filled
 * @param len - Size of the buffer
 * @return \ref SUCCESS if no errors, \ref FAILURE is an error occurs and the
 * data in the buffer is not useful.
 */
int32_t trng_fill_buffer(struct trng_desc *desc, uint8_t *buff, uint32_t len)
{
	uint32_t	data;
	uint32_t	i;
	bool		ready;
	bool		stuck;

	for (i = 0; i < len; i++) {
		ready = 0;
		while (!ready)
			adi_rng_GetRdyStatus(desc->dev, &ready);
		adi_rng_GetStuckStatus(desc->dev, &stuck);
		if (stuck) {
			/* This is needed in order to clear the stuck bit */
			gdesc_remove();
			gdesc_init();
			return FAILURE;
		}
		adi_rng_GetRngData(desc->dev, &data);
		buff[i] = data & 0xFF;
	}

	return SUCCESS;
}
