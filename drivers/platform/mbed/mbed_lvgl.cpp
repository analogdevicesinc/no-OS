/**************************************************************************//**
 *   @file   mbed_lvgl.cpp
 *   @brief  Implementation of lvgl library interface for Mbed Platform
*******************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
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

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/

#include <stdio.h>
#include <mbed.h>

//Platform Drivers need to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif	//__cplusplus

#include "no_os_lvgl.h"
#include "no_os_error.h"
#include "no_os_irq.h"
#include "mbed_lvgl.h"
#include "lvgl/lvgl.h"

using namespace std::chrono;

struct mbed_irq_callback_desc {
	struct no_os_callback_desc desc;
	void (*mbed_callback)(void);
};

/* Mbed irq callback structure variable */
static struct mbed_irq_callback_desc mbed_lvgl_callback;

/*****************************************************************************/
/************************** Functions Definitions ****************************/
/*****************************************************************************/

static void mbed_lvgl_tick_callback(void)
{
	if (mbed_lvgl_callback.desc.callback)
		/* Invoke the application registered callback function */
		mbed_lvgl_callback.desc.callback(mbed_lvgl_callback.desc.ctx);
}

/**
 * @brief Initialize the lvlg library interface.
 * @param desc[in, out] - The lvgl descriptor.
 * @param init_param[in] - Structure that contains lvgl initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mbed_lvgl_init(struct no_os_lvgl_desc **desc,
		       const struct no_os_lvgl_init_param *param)
{
	struct no_os_lvgl_desc *lvgl_desc; 		// Pointer to lvgl descriptor
	struct mbed_lvgl_desc *mbed_lvgl_desc;	// Pointer to mbed lvgl descriptor
	struct no_os_callback_desc *callback_desc;	// Callback descriptor
	mbed::Ticker* ticker;	// systick for lvgl task handler

	if (!desc || !param)
		return -EINVAL;

	lvgl_desc = (struct no_os_lvgl_desc*)calloc(1, sizeof(*lvgl_desc));
	if (!lvgl_desc)
		return -ENOMEM;

	mbed_lvgl_desc = (struct mbed_lvgl_desc*)calloc(1, sizeof(*mbed_lvgl_desc));
	if (!mbed_lvgl_desc)
		goto err_mbed_lvgl_desc;

	callback_desc = ((struct mbed_lvgl_init_param *)(param->extra))->callback_desc;
	mbed_lvgl_callback.desc.callback = callback_desc->callback;
	mbed_lvgl_callback.desc.ctx = callback_desc->ctx;

	/* Ticker (systick) timer is required for lvgl task handling */
	ticker = new Ticker();
	if (!ticker)
		goto err_lvgl_ticker;

	/* Attach callback to ticker timer */
	ticker->attach(mbed_lvgl_tick_callback,
		       microseconds(((struct mbed_lvgl_init_param*)(param->extra))->tick_period_us));
	
	/* Initialize the lvgl library */
	lv_init();
	
	/* Initialize the vendor specific touchpad and display */
	param->lvgl_tft_init();
	param->lvgl_touchpad_init();

	lvgl_desc->extra = mbed_lvgl_desc;

	*desc = lvgl_desc;
	return 0;

err_lvgl_ticker:
	free(mbed_lvgl_desc);
err_mbed_lvgl_desc:
	free(lvgl_desc);

	return -ENOMEM;
}

/**
* @brief Release all the resources allocated by mbed_lvgl_init().
* @param desc[in] - The lvgl Descriptor.
* @return 0 in case of success, negative error code otherwise.
*/
int32_t mbed_lvgl_remove(struct no_os_lvgl_desc *desc)
{
	if (!desc || !desc->extra)
		return -EINVAL;

	/* Ticker doesn't have destructor implementation, so nothing to free */
	mbed_lvgl_callback.desc.callback = NULL;

	/* Free the lvgl descriptors */
	free(desc->extra);
	free(desc);

	return 0;
}

/**
* @brief Mbed platform specific lvgl platform ops structure
*/
const struct no_os_lvgl_platform_ops mbed_lvgl_ops = {
	.init = &mbed_lvgl_init,
	.remove = &mbed_lvgl_remove,
};

#ifdef __cplusplus
}
#endif // __cplusplus
