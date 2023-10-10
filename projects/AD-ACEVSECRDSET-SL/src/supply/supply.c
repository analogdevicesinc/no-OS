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
static int supply_read_wavs(struct ade7913_dev *dev)
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
	struct ade7913_dev *desc = dev;
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
int supply_init(struct ade7913_dev **device)
{
	struct no_os_irq_ctrl_desc *ade7913_gpio_irq_desc;
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
	ret = no_os_gpio_get_optional(&zx_zero_cross, &ade7913_gpio_ZX_ip);
	if (ret)
		return ret;

	if (zx_zero_cross) {
		ret = no_os_gpio_direction_input(zx_zero_cross);
		if (ret)
			return ret;
	}

	/* Initialize GPIO IRQ controller */
	ret = no_os_irq_ctrl_init(&ade7913_gpio_irq_desc, &ade7913_gpio_irq_ip);
	if (ret)
		return ret;

	ade7913_ip.irq_ctrl = ade7913_gpio_irq_desc;
	ade7913_ip.drdy_callback = supply_irq_handler;

	ret = ade7913_init(device, ade7913_ip);
	if (ret)
		goto ade7913_init_err;

	switch ((*device)->ver_product) {
	case ADE7913_3_CHANNEL_ADE7913:
		pr_debug("3 channel ADE7913 successfully initialized.\n");
		break;
	case ADE7913_2_CHANNEL_ADE9112:
		pr_debug("2 channel ADE7913 successfully initialized.\n");
		break;
	case ADE7913_NONISOLATED_ADE9103:
		pr_debug("Non Isolated ADE7913 successfully initialized.\n");
		break;
	default:
		pr_debug("Device not supported.\n");
		break;
	}

	ret = ade7913_set_normal_mode((*device));
	if (ret)
		goto error;

	ret = ade7913_select_zero_crossing_channel((*device), ADE7913_ZX_V1_SEL);
	if (ret)
		goto error;

	ret = ade7913_select_zero_crossing_edge((*device),
						ADE7913_ZX_INPUT_SIGNAL_SIGN);
	if (ret)
		goto error;

	ret = ade7913_set_crc_en_state((*device), 0);
	if (ret)
		goto error;

	/* Set digital signal processing configuration. */
	ret =  ade7913_set_dsp_config((*device),
				      ADE7913_SINC3_LPF_EN_4_KHZ_SAMPLING);

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
	ade7913_remove(device);
ade7913_init_err:
	no_os_irq_ctrl_remove(ade7913_gpio_irq_desc);
	ade7913_gpio_irq_desc = NULL;

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
 * @brief Convert values measured by the ADE7913 device to mV (mA val for I)
 * @param stout - state  machine descriptor
 * @param i_val - value of I waveform (in mA)
 * @param v1_val - value of V1 waveform in mV
 * @param v2_val - value of V2 waveform in mV
 * @return 0 in case of success, error code otherwise
 */
int supply_conv_vals_to_mv(struct stout *stout, int32_t *i_val, int32_t *v1_val,
			   int32_t *v2_val)
{
	int ret;

	ret = ade7913_convert_to_millivolts(stout->ade7913, ADE7913_I_WAV, i_val);
	if (ret)
		return ret;
	ret = ade7913_convert_to_millivolts(stout->ade7913, ADE7913_V1_WAV, v1_val);
	if (ret)
		return ret;
	return ade7913_convert_to_millivolts(stout->ade7913, ADE7913_V2_WAV, v2_val);
}
