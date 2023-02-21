/***************************************************************************//**
 *   @file   no_os_lvgl.h
 *   @brief  Header file of the lvgl library interface
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef _NO_OS_LVGL_H_
#define _NO_OS_LVGL_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct no_os_lvgl_init_param
 * @brief  Structure containing the init parameters needed by lvgl library
 */
struct no_os_lvgl_init_param {
	/** Platform specific lvgl platform ops structure. */
	const struct no_os_lvgl_platform_ops *platform_ops;
	/** Vendor specific display init for lvgl */
	void(*lvgl_tft_init)(void);
	/** Vendor specific touchpad init for lvgl */
	void(*lvgl_touchpad_init)(void);
	/** lvgl extra parameters (platform specific) */
	void *extra;
};

/**
 * @struct no_os_lvgl_desc
 * @brief  Structure representing lvgl descriptor
 */
struct no_os_lvgl_desc {
	/** Platform specific lvgl platform ops structure. */
	const struct no_os_lvgl_platform_ops *platform_ops;
	/** lvgl extra parameters (platform specific) */
	void *extra;
};

/**
 * @struct no_os_lvgl_platform_ops
 * @brief Structure holding lvgl function pointers that point to the platform
 * specific function
 */
struct no_os_lvgl_platform_ops {
	/** lvgl init function pointer */
	int32_t (*init)(struct no_os_lvgl_desc **desc,
			const struct no_os_lvgl_init_param *param);
	/** lvgl remove function pointer */
	int32_t (*remove)(struct no_os_lvgl_desc *desc);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize the lvgl */
int32_t no_os_lvgl_init(struct no_os_lvgl_desc **desc,
			const struct no_os_lvgl_init_param *param);

int32_t no_os_lvgl_remove(struct no_os_lvgl_desc* desc);

#endif // _NO_OS_LVGL_H_
