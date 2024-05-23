/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for ad469x_fmcz project.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "iio_example.h"
#include "common_data.h"
#include "iio.h"
#include "ad469x.h"
#include "no_os_util.h"
#include "no_os_print_log.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/* Data channel mask */
static uint32_t g_mask;
/* Temperature channel mask */
static uint32_t g_temp_mask;

/**
 * @brief Prepare conversion mode.
 * @param dev - ad469x device pointer
 * @param mask - mask of enabled channesls
 * @return ret - will setup the conversion mode for the device
*/
static int32_t iio_ad469x_prepare_conversion(struct ad469x_dev *dev,
		uint32_t mask)
{
	int32_t	ret;

	if (mask == g_mask)
		return 0;

	ret = ad469x_exit_conversion_mode(dev);
	if (ret)
		return ret;

	if (mask & g_temp_mask) {
		ret = ad469x_sequence_enable_temp(dev);
		if (ret)
			return ret;
	} else {
		ret = ad469x_sequence_disable_temp(dev);
		if (ret)
			return ret;
	}

	ret = ad469x_std_sequence_ch(dev, mask & ~(g_temp_mask));
	if (ret)
		return ret;

	ret = ad469x_set_channel_sequence(dev, AD469x_standard_seq);
	if (ret)
		return ret;

	ret = ad469x_enter_conversion_mode(dev);
	if (ret)
		return ret;

	g_mask = mask;

	return ret;
}

/**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *				execute continuously function iio_app_run and will not return.
*/
int iio_example_main()
{
	int ret;
	uint8_t num_channels;
	uint32_t i;
	bool temp_enabled = false;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct ad469x_dev *dev;
	struct iio_channel *ad469x_iio_channels;
	struct iio_data_buffer read_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR,
	};
	struct scan_type ad469x_scan_type = {
		.sign = 'u',
		.realbits = 19,
		.storagebits = 32,
		.shift = 0,
		.is_big_endian = false
	};
	struct iio_device ad469x_iio_descriptor = {
		.pre_enable = (int32_t (*)(void *, uint32_t))iio_ad469x_prepare_conversion,
		.read_dev = (int32_t (*)(void *, void *, uint32_t))ad469x_seq_read_data
	};

	ret = ad469x_init(&dev, &ad469x_init_param);
	if (ret < 0)
		return ret;

	ret = ad469x_get_num_channels(dev, &num_channels);
	if (ret)
		goto err_remove;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad469x", dev, &ad469x_iio_descriptor,
			       &read_buff, NULL, NULL),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	ad469x_iio_channels =
		no_os_calloc(1, num_channels * sizeof(*ad469x_iio_channels));
	if (!ad469x_iio_channels)
		return -ENOMEM;

	g_temp_mask = 0;
	for (i = 0; i < num_channels; i++) {
		if (ad469x_is_temp_channel(dev, i)) {
			ad469x_iio_channels[i].ch_type = IIO_TEMP;
			g_temp_mask = (1 << i);
		} else {
			ad469x_iio_channels[i].ch_type = IIO_VOLTAGE;
		}
		ad469x_iio_channels[i].channel = i;
		ad469x_iio_channels[i].scan_index = i;
		ad469x_iio_channels[i].scan_type = &ad469x_scan_type;
		ad469x_iio_channels[i].indexed = true;
	}

	ad469x_iio_descriptor.num_ch = num_channels;
	ad469x_iio_descriptor.channels = ad469x_iio_channels;

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		pr_info("Error: iio_app_init: %d\n", ret);
		goto err;
	}

	ret = iio_app_run(app);
	if (ret)
		pr_info("Error: iio_app_run: %d\n", ret);

	iio_app_remove(app);
err:
	no_os_free(ad469x_iio_channels);
err_remove:
	ad469x_remove(dev);

	return ret;
}
