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
#include "maxim_spi.h"
#include "supply.h"
#include "spi.h"

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
 *
 * @return flag value
 */
int get_zero_cross_flag_state(void)
{
	return flag_zx;
}

/**
 * @brief Reset zero crossing flag value
 *
 * @return none
 */
void reset_zero_cross_flag_state(void)
{
	flag_zx = 0;
}

/**
 * @brief Read waveforms.
 * @param dev- The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int supply_read_wavs(struct ade9113_dev *dev)
{
	// int ret;
	/* buffer for data read */
	/* set read bit and set long operation bit
	 * buff[12] = 0xC0;
	 * set address to read from
	 * buff[13] = 0x00;
	 * add CRC to command is 0xD8
	 * buff[15] = 0xD8; */
	uint8_t buff[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xC0, 0, 0, 0xD8 };
	uint8_t rx_buff[16] = {0};
	mxc_spi_req_t req;

	req.spi = MXC_SPI0;
	req.ssIdx = 0;
	req.txData = buff;
	req.rxData = rx_buff;
	req.txCnt = 0;
	req.rxCnt = 0;
	req.ssDeassert = 1;
	req.txLen = 16;
	req.rxLen = 16;

	MXC_SPI_MasterTransaction(&req);

	dev->i_wav = no_os_sign_extend32(no_os_get_unaligned_le24(&rx_buff[1]), 23);
	dev->v1_wav = no_os_sign_extend32(no_os_get_unaligned_le24(&rx_buff[5]), 23);
	dev->v2_wav = no_os_sign_extend32(no_os_get_unaligned_le24(&rx_buff[9]), 23);

	return 0;
}

/**
 * @brief GPIO interrupt handler for data ready.
 * @param dev - The device structure.
 */
static void supply_irq_handler(void *dev)
{
	struct ade9113_dev *desc = dev;
	int ret;

	if (!dev)
		return;

	/* Disable interrupt while reading data. */
	ret = no_os_irq_disable(desc->irq_ctrl, desc->gpio_rdy->number);
	if (ret)
		return;

	/* READ the data and place it in device structure */
	supply_read_wavs(desc);

	/* Reenable interrupt */
	ret = no_os_irq_enable(desc->irq_ctrl,
			       desc->gpio_rdy->number);

	return;
}

/**
 * @brief Initialize supply component
 * @param device - ADE9713 device descriptor
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
	ade9113_ip.drdy_callback = supply_irq_handler;

	ret = ade9113_init(device, ade9113_ip);
	if (ret)
		goto ade9113_init_err;

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
		goto error;

	ret = ade9113_select_zero_crossing_channel((*device), ADE9113_ZX_V1_SEL);
	if (ret)
		goto error;

	ret = ade9113_select_zero_crossing_edge((*device),
						ADE9113_ZX_INPUT_SIGNAL_SIGN);
	if (ret)
		goto error;

	ret = ade9113_set_crc_en_state((*device), 0);
	if (ret)
		goto error;

	/* Set digital signal processing configuration. */
	ret =  ade9113_set_dsp_config((*device),
				      ADE9113_SINC3_LPF_EN_4_KHZ_SAMPLING);

	ret = no_os_irq_register_callback((*device)->irq_ctrl, GPIO_ZC_PIN,
					  &zx_zero_cross_cb);
	if (ret)
		goto error;

	ret = no_os_irq_trigger_level_set((*device)->irq_ctrl,
					  GPIO_ZC_PIN, NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		goto error;

	ret = no_os_irq_set_priority((*device)->irq_ctrl, GPIO_ZC_PIN, 4);
	if (ret)
		goto error;

	ret = no_os_irq_enable((*device)->irq_ctrl, GPIO_ZC_PIN);
	if (ret)
		goto error;

	return 0;

error:
	ade9113_remove(device);
ade9113_init_err:
	no_os_irq_ctrl_remove(ade9113_gpio_irq_desc);
	ade9113_gpio_irq_desc = NULL;

	return ret;
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
 * @brief Compute v1 rms value
 * @param sample - v1 sample
 * @return v1 rms value
 */
int64_t rms_filter_v1(int32_t sample)
{
	static int32_t rms = INITIAL;
	static int64_t sum_squares = 1UL * SAMPLES * INITIAL * INITIAL;

	sum_squares -= sum_squares / SAMPLES;
	sum_squares += (int64_t) sample * sample;
	if (rms == 0) rms = 1;    /* do not divide by zero */
	rms = (rms + sum_squares / SAMPLES / rms) / 2;
	return rms;
}

/**
 * @brief Compute v2 rms value
 * @param sample - v2 sample
 * @return v2 rms value
 */
int64_t rms_filter_v2(int32_t sample)
{
	static int32_t rms = INITIAL;
	static int64_t sum_squares = 1UL * SAMPLES * INITIAL * INITIAL;

	sum_squares -= sum_squares / SAMPLES;
	sum_squares += (int64_t) sample * sample;
	if (rms == 0) rms = 1;    /* do not divide by zero */
	rms = (rms + sum_squares / SAMPLES / rms) / 2;
	return rms;
}

/**
 * @brief Compute i rms value
 * @param sample - i sample
 * @return i rms value
 */
int64_t rms_filter_i(int32_t sample)
{
	static int32_t rms = INITIAL;
	static int64_t sum_squares = 1UL * SAMPLES * INITIAL * INITIAL;

	sum_squares -= sum_squares / SAMPLES;
	sum_squares += (int64_t) sample * sample;
	if (rms == 0) rms = 1;    /* do not divide by zero */
	rms = (rms + sum_squares / SAMPLES / rms) / 2;
	return rms;
}

/**
 * @brief saves the current and voltage values in rms_adc structure
 * @param stout - application strucutre
 * @param rms - structure holding the measurements values
 * @return 0 in case of success, negative error code otherwise
 */
int rms_adc_values_read(struct stout *stout, struct rms_adc_values *rms)
{
	int32_t i_val, v1_val, v2_val, v1_rms, v2_rms, i_rms;
	int ret;

	ret = ade9113_convert_to_millivolts(stout->ade9113, ADE9113_I_WAV, &i_val);
	if (ret)
		return ret;
	ret = ade9113_convert_to_millivolts(stout->ade9113, ADE9113_V1_WAV, &v1_val);
	if (ret)
		return ret;
	ret = ade9113_convert_to_millivolts(stout->ade9113, ADE9113_V2_WAV, &v2_val);
	if (ret)
		return ret;
	rms->v1_rms_adc = (int32_t)rms_filter_v1((int32_t)v1_val);
	rms->v2_rms_adc = (int32_t)rms_filter_v2((int32_t)v2_val);
	rms->i_rms_adc = (int32_t)rms_filter_i((int32_t)i_val);
	// Scale is 0,03125 = 3125/100000 = 1/32 = 1/2^5
	rms->i_rms = (((int64_t)((int32_t)(int64_t)rms->i_rms_adc))*ADE9113_VREF) /
		     (1 << 28);
	rms->v1_rms = (((int64_t)((int32_t)(supply_scale_v1((int64_t)rms->v1_rms_adc))))
		       *ADE9113_VREF) / (1 << 23);
#if defined(REV_A)
	rms->v2_rms = (((int64_t)((int32_t)(supply_scale_v2((int64_t)rms->v2_rms_adc))))
		       *ADE9113_VREF) / (1 << 23);
#elif defined(REV_D)
	rms->v2_rms = (((int64_t)((int32_t)(supply_scale_v1((int64_t)rms->v2_rms_adc))))
		       *ADE9113_VREF) / (1 << 23);
#endif

	return 0;
}
