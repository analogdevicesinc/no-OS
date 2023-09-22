/***************************************************************************//**
 *   @file   supply.c
 *   @brief  Power supply phase implementation file.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
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
#include "no_os_print_log.h"
#include "common_data.h"
#include "supply.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/******************************************************************************/
/********************************** Variables *********************************/
/******************************************************************************/
// Flag indicating that power supply signal crossed x axis
static volatile int flag_zx = 0;

/******************************************************************************/
/*************************** Functions Definitions ****************************/
/******************************************************************************/
/**
 * @brief Zero crossing callback function
 * @param context - context variable
 * @return none
 */
static void zx_zero_cross_fn(void *context)
{
	flag_zx++;
}

/**
 * @brief Get zero crossing flag value
 * @param none
 * @return flag value
 */
int get_zero_cross_flag_state(void)
{
	return flag_zx;
}

/**
 * @brief Reset zero crossing flag value
 * @param none
 * @return none
 */
void reset_zero_cross_flag_state(void)
{
	flag_zx = 0;
}

/**
 * @brief Initialize pilot phase
 * @param desc - NVIC IRQ controller description
 * @return 0 in case of success, error code otherwise
 */
int supply_init(struct ade9113_dev **device)
{
	struct no_os_irq_ctrl_desc *ade9113_gpio_irq_desc;
	struct no_os_gpio_desc	*zx_zero_cross;
	int ret;

	/* Zero crossing callback */
	struct no_os_callback_desc zx_zero_cross_cb = {
		/** Callback to be called when an event occurs. */
		.callback = zx_zero_cross_fn,
		/** Parameter to be passed when the callback is called */
		.ctx = NULL,
		/** Event that triggers the calling of the callback. */
		.event = NO_OS_EVT_GPIO,
		/** Interrupt source peripheral specifier. */
		.peripheral = NO_OS_GPIO_IRQ,
		/** Not used in the case of a GPIO IRQ controller */
		.handle = NULL
	};

	/* Set GPIO for zero crossing */
	ret = no_os_gpio_get_optional(&zx_zero_cross, &ade9113_gpio_ZX_ip);
	if (ret)
		return ret;

	if (zx_zero_cross) {
		ret = no_os_gpio_direction_input(zx_zero_cross);
		if (ret)
			return ret;
	}

	/* Initialize GPIO IRQ controller */
	ret = no_os_irq_ctrl_init(&ade9113_gpio_irq_desc, &ade9113_gpio_irq_ip);
	if (ret)
		return ret;

	ade9113_ip.irq_ctrl = ade9113_gpio_irq_desc;

	ret = ade9113_init(device, ade9113_ip);
	if (ret)
		return ret;

	switch ((*device)->ver_product) {
	case ADE9113_3_CHANNEL_ADE9113:
		pr_debug("3 channel ADE9113 successfully initialized.\n");
		break;
	case ADE9113_2_CHANNEL_ADE9112:
		pr_debug("2 channel ADE9113 successfully initialized.\n");
		break;
	case ADE9113_NONISOLATED_ADE9103:
		pr_debug("Non Isolated ADE9113 successfully initialized.\n");
		break;
	default:
		pr_debug("Device not supported.\n");
		break;
	}

	ret = ade9113_set_normal_mode((*device));
	if (ret)
		return ret;

	ret = ade9113_select_zero_crossing_channel((*device), ADE9113_ZX_V1_SEL);
	if (ret)
		return ret;

	ret = ade9113_select_zero_crossing_edge((*device),
						ADE9113_ZX_INPUT_SIGNAL_SIGN);
	if (ret)
		return ret;

	ret = ade9113_set_crc_en_state((*device), 0);
	if (ret)
		return ret;

	/* Set digital signal processing configuration. */
	ret =  ade9113_set_dsp_config((*device),
				      ADE9113_SINC3_LPF_EN_4_KHZ_SAMPLING);

	ret = no_os_irq_register_callback((*device)->irq_ctrl, GPIO_ZC_PIN,
					  &zx_zero_cross_cb);
	if (ret)
		return ret;

	ret = no_os_irq_trigger_level_set((*device)->irq_ctrl,
					  GPIO_ZC_PIN, NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority((*device)->irq_ctrl, GPIO_ZC_PIN, 4);
	if (ret)
		return ret;

	ret = no_os_irq_enable((*device)->irq_ctrl, GPIO_ZC_PIN);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Scale V1 value
 * @param val - value to be scaled
 * @return scaled value
 */
int32_t supply_scale_v1(int32_t val)
{
	int64_t v;

	v = (int64_t)val;
	v = (v * V1_MULT) / V1_DIVIDER;

	return (int32_t)v;
}

/**
 * @brief Scale V2 value
 * @param val - value to be scaled
 * @return scaled value
 */
int32_t supply_scale_v2(int32_t val)
{
	int64_t v;

	v = (int64_t)val;
	v = (v * V1_MULT) / V2_DIVIDER;

	return (int32_t)v;
}

/**
 * @brief Convert values measured by the ADE9113 device to mV (mA val for I)
 * @param stout - state  machine descriptor
 * @param i_val - value of I waveform (in mA)
 * @param V1_val - value of V1 waveform in mV
 * @param V2_val - value of V2 waveform in mV
 * @return 0 in case of success, error code otherwise
 */
int supply_conv_vals_to_mv(struct stout *stout, int32_t *i_val, int32_t *v1_val,
			   int32_t *v2_val)
{
	int ret;

	ret = ade9113_convert_to_millivolts(stout->ade9113, ADE9113_I_WAV, i_val);
	if (ret)
		return ret;
	ret = ade9113_convert_to_millivolts(stout->ade9113, ADE9113_V1_WAV, v1_val);
	if (ret)
		return ret;
	return ade9113_convert_to_millivolts(stout->ade9113, ADE9113_V2_WAV, v2_val);
}
