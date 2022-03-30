/***************************************************************************//**
 *   @file   ad7768_evb.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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
#include <xparameters.h>
#include "xil_cache.h"
#include "ad7768.h"
#include "axi_dmac.h"
#include "no_os_gpio.h"
#include "spi_extra.h"
#include "gpio_extra.h"
#include "no_os_error.h"
#include "axi_adc_core.h"
#include <xil_io.h>
#include <stdio.h>
#include "parameters.h"

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_axi_adc.h"
#endif

/***************************************************************************//**
* @brief ad7768evb_clear_status
*******************************************************************************/
void ad7768_evb_clear_status(struct xil_gpio_init_param *brd_gpio_init)
{
	uint8_t i;
	struct no_os_gpio_init_param temp_init = {
		.number = 0,
		.platform_ops = &xil_gpio_ops,
		.extra = brd_gpio_init
	};
	struct no_os_gpio_desc *temp_desc;
	int32_t ret;

	for (i = 0; i < 32; i ++) {
		temp_init.number = UP_STATUS_OFFSET + i;
		ret = no_os_gpio_get(&temp_desc, &temp_init);
		if (ret != 0)
			return;
		no_os_gpio_direction_output(temp_desc, NO_OS_GPIO_HIGH);
		ret = no_os_gpio_remove(temp_desc);
		if (ret != 0)
			return;
	}
}

/***************************************************************************//**
* @brief ad7768evb_verify_status
*******************************************************************************/
uint8_t ad7768_evb_verify_status(struct xil_gpio_init_param *brd_gpio_init)
{
	uint8_t i;
	uint8_t value;
	uint8_t status = 0;
	struct no_os_gpio_init_param temp_init = {
		.number = 0,
		.platform_ops = &xil_gpio_ops,
		.extra = brd_gpio_init
	};
	struct no_os_gpio_desc *temp_desc;
	int32_t ret;

	for (i = 0; i < 32; i ++) {
		temp_init.number = UP_STATUS_OFFSET + i;
		ret = no_os_gpio_get(&temp_desc, &temp_init);
		if (ret != 0)
			return -1;
		no_os_gpio_direction_input(temp_desc);
		no_os_gpio_get_value(temp_desc, &value);
		ret = no_os_gpio_remove(temp_desc);
		if (ret != 0)
			return -1;
		status += value;
	}

	return status;
}

static int32_t ad7768_if_gpio_setup(uint32_t gpio_no, uint8_t gpio_val)
{
	struct xil_gpio_init_param ps_gpio_init = {
		.device_id = GPIO_DEVICE_ID,
		.type = GPIO_PS
	};
	struct no_os_gpio_init_param temp_init = {
		.number = gpio_no,
		.platform_ops = &xil_gpio_ops,
		.extra = &ps_gpio_init
	};
	struct no_os_gpio_desc *temp_desc;
	int32_t ret;

	ret = no_os_gpio_get(&temp_desc, &temp_init);
	if (ret != 0)
		return -1;
	ret = no_os_gpio_direction_output(temp_desc, gpio_val);
	if (ret != 0)
		return -1;
	return no_os_gpio_remove(temp_desc);
}

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	ad7768_dev	*dev;
	struct xil_gpio_init_param axi_gpio_init;
	struct axi_dmac_init dma_initial = {
		.name = "ad7768_dma",
		.base = AD7768_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED
	};
	struct axi_dmac *dma_desc;
	int32_t ret;
	uint32_t data_size;
	const uint32_t chan_no = AD7768_CH_NO, resolution = AD7768_RESOLUTION,
		       sample_no = 1024;
	struct xil_spi_init_param xil_spi_initial = {
		.flags = 0,
		.type = SPI_PS
	};

	struct xil_gpio_init_param xil_gpio_initial = {
		.device_id = GPIO_DEVICE_ID,
		.type = GPIO_PS
	};

	ad7768_init_param default_init_param = {
		/* SPI */
		.spi_init = {
			.device_id = SPI_DEVICE_ID,
			.max_speed_hz = 1000000,
			.chip_select = SPI_AD7768_CS,
			.mode = NO_OS_SPI_MODE_0,
			.platform_ops = &xil_spi_ops,
			.extra = &xil_spi_initial
		},
		/* GPIO */
		.gpio_reset = {
			.number = GPIO_RESET_N,
			.platform_ops = &xil_gpio_ops,
			.extra = &xil_gpio_initial
		},
		.gpio_mode0 = {
			.number = GPIO_MODE_0_GPIO_0,
			.platform_ops = &xil_gpio_ops,
			.extra = &xil_gpio_initial
		},
		.gpio_mode1 = {
			.number = GPIO_MODE_1_GPIO_1,
			.platform_ops = &xil_gpio_ops,
			.extra = &xil_gpio_initial
		},
		.gpio_mode2 = {
			.number = GPIO_MODE_2_GPIO_2,
			.platform_ops = &xil_gpio_ops,
			.extra = &xil_gpio_initial
		},
		.gpio_mode3 = {
			.number = GPIO_MODE_3_GPIO_3,
			.platform_ops = &xil_gpio_ops,
			.extra = &xil_gpio_initial
		},
		.gpio_reset_value = NO_OS_GPIO_HIGH,
		/* Configuration */
		.pin_spi_input_value = AD7768_SPI_CTRL,
		.sleep_mode = AD7768_ACTIVE,
		.power_mode = AD7768_FAST,
		.mclk_div = AD7768_MCLK_DIV_4,
		.dclk_div = AD7768_DCLK_DIV_1,
		.conv_op = AD7768_STANDARD_CONV,
		.crc_sel = AD7768_CRC_16
	};
	struct axi_adc *axi_adc_core_desc;
	struct axi_adc_init axi_adc_initial = {
		.base = AD7768_ADC_BASEADDR,
		.name = "ad7768_axi_adc",
		.num_channels = chan_no
	};
	int32_t *data_ptr, i;

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

	ret = ad7768_if_gpio_setup(GPIO_UP_SSHOT, NO_OS_GPIO_LOW);
	if (ret != 0)
		return ret;

	/* Configure HDL */
	ret = ad7768_if_gpio_setup(GPIO_UP_FORMAT_1, NO_OS_GPIO_LOW);
	if (ret != 0)
		return ret;

	ret = ad7768_if_gpio_setup(GPIO_UP_FORMAT_0, NO_OS_GPIO_LOW);
	if (ret != 0)
		return ret;

	ret = ad7768_if_gpio_setup(GPIO_UP_CRC_ENABLE,
				   default_init_param.crc_sel ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
	if (ret != 0)
		return ret;

	ret = ad7768_if_gpio_setup(GPIO_UP_CRC_4_OR_16_N,
				   (default_init_param.crc_sel == AD7768_CRC_4) ? NO_OS_GPIO_HIGH :
				   NO_OS_GPIO_LOW);
	if (ret != 0)
		return ret;

	ad7768_setup(&dev, default_init_param);

	ret = axi_adc_init(&axi_adc_core_desc, &axi_adc_initial);
	if (ret != 0)
		return -1;

	axi_gpio_init.type = GPIO_PL;
	axi_gpio_init.device_id = XPAR_AD7768_GPIO_DEVICE_ID;

	ad7768_evb_clear_status(&axi_gpio_init);
	if (ad7768_evb_verify_status(&axi_gpio_init))
		printf("Interface errors\n");
	else
		printf("Interface OK\n");

	ret = axi_dmac_init(&dma_desc, &dma_initial);
	if (ret != 0)
		return -1;

	data_size = (sample_no * chan_no *
		     ((resolution + AD7768_HEADER_SIZE) / BITS_IN_BYTE));

	printf("Capture samples...\n");
	struct axi_dma_transfer read_transfer = {
		// Number of bytes to write/read
		.size = data_size,
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)ADC_DDR_BASEADDR
	};
	ret = axi_dmac_transfer_start(dma_desc, &read_transfer);
	if (ret != 0)
		return -1;
	ret = axi_dmac_transfer_wait_completion(dma_desc, 500);
	if (ret)
		return ret;
	Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR,data_size);
	printf("Capture done\n");

	if (ad7768_evb_verify_status(&axi_gpio_init))
		printf("Interface errors\n");
	else
		printf("Interface OK\n");

	printf("   CH0      CH1      CH2      CH3      CH4      CH5      CH6      CH7   ");
	for (i = 0; i < (sample_no * chan_no); i++) {
		if ((i % chan_no) == 0)
			printf("\n\r");
		data_ptr = (int32_t *)(ADC_DDR_BASEADDR + (i * sizeof(uint32_t)));
		printf("%8.5f ", ((float)(*data_ptr) * 0.000000488));
	}

#ifdef IIO_SUPPORT
	printf("The board accepts libiio clients connections through the serial backend.\n");

	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_device *dev_desc;
	struct iio_axi_adc_init_param iio_axi_adc_init_par = {
		.rx_adc = axi_adc_core_desc,
		.rx_dmac = dma_desc,
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
	};

	ret = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (ret < 0)
		return ret;

	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &dev_desc);

	struct iio_data_buffer read_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};

	struct iio_app_device devs[] = {
		IIO_APP_DEVICE("ad7768_dev", iio_axi_adc_desc, dev_desc,
			       &read_buff, NULL),
	};

	return iio_app_run(devs, NO_OS_ARRAY_SIZE(devs));
#endif

	/* Disable the instruction cache. */
	Xil_DCacheDisable();
	/* Disable the data cache. */
	Xil_ICacheDisable();

	return 0;
}
