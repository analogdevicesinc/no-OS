/***************************************************************************//**
 *   @file   no_os_ain.h
 *   @author PMallick (Pratyush.Mallick@analog.com)
********************************************************************************
 * Copyright (c) 2021-22 Analog Devices, Inc.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef NO_OS_AIN_H
#define NO_OS_AIN_H

#include <stdint.h>


/**
 * @struct ain_init_param
 * @brief Structure holding parameters for analog input initialization
 */
struct no_os_ain_init_param {
	/* Analog input reference voltage */
	float vref;
	/* Analog extra parameters (device specific) */
	void *extra;
};

/**
 * @struct ain_desc
 * @brief Structure holding analog input descriptor
 */
struct no_os_ain_desc {
	/* Analog input reference voltage */
	float vref;
	/* Analog extra parameters (device specific) */
	void *extra;
};

/* Read analog input voltage */
int32_t no_os_ain_get_voltage(struct no_os_ain_desc *desc, float *value);

/* Initialize the analog input peripheral*/
int32_t no_os_ain_init(struct no_os_ain_desc **desc,
		       const struct no_os_ain_init_param *param);

/* Free the resources allocated by no_os_ain_init() */
int32_t no_os_ain_remove(struct no_os_ain_desc *desc);

#endif	// end of NO_OS_AIN_H
