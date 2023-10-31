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
#include "no_os_print_log.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "xilinx_spi.h"
#include "xilinx_gpio.h"
#include "no_os_error.h"
#include "axi_adc_core.h"
#include <xil_io.h>
#include <stdio.h>
#include "parameters.h"

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_axi_adc.h"
#include "xilinx_uart.h"
#endif

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	const uint32_t resolution = AD7768_RESOLUTION;
	const uint32_t chan_no = AD7768_CH_NO;
	struct axi_adc *axi_adc_core_desc;
	const uint32_t sample_no = 1024;
	struct axi_dmac *dma_desc;
	uint32_t *data_ptr, i;
	uint32_t data_size;
	uint8_t reg_data;
	ad7768_dev *dev;
	int32_t data;
	int ret;

	struct axi_dmac_init dma_initial = {
		.name = "ad7768_dma",
		.base = AD7768_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED
	};

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
		.gpio_reset_value = NO_OS_GPIO_HIGH,
		/* Configuration */
		.mclk = 32768000,
		.datalines = 8
	};

	struct axi_adc_init axi_adc_initial = {
		.base = AD7768_ADC_BASEADDR,
		.name = "ad7768_axi_adc",
		.num_channels = chan_no
	};

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

	/* Start AD7768 device initialization */
	ret = ad7768_setup_begin(&dev, default_init_param);
	if (ret)
		goto error;

	/* Reset AD7768 */
	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
	if (ret)
		goto error_1;
	/* minimum 2 / mclck */
	no_os_udelay(100);
	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	if (ret != 0)
		goto error_1;

	// allow at least 1.66 ms after reset, (ADC start-up time)
	no_os_udelay(1660);

	/* Check Rev ID */
	ret = ad7768_spi_read(dev, AD7768_REG_REV_ID, &reg_data);
	if (ret)
		goto error;
	if (reg_data != 0x06) {
		pr_info("\nUnrecognized device ID  %#x.\n", reg_data);
		goto error;
	} else
		pr_info("\nAD7768 Rev ID %#x.\n", reg_data);

	/* Finish AD7768 device initialization */
	ad7768_set_available_sampl_freq(dev);
	ret = ad7768_set_power_mode_and_sampling_freq(dev, AD7768_FAST_MODE);
	if (ret)
		goto error_1;

	/* Start AXI ADC initialization */
	ret = axi_adc_init_begin(&axi_adc_core_desc, &axi_adc_initial);
	if (ret)
		goto error_1;

	/* Reset Rx ADC */
	axi_adc_write(axi_adc_core_desc, AXI_ADC_REG_RSTN, 0);
	axi_adc_write(axi_adc_core_desc, AXI_ADC_REG_RSTN,
		      AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);

	for (uint8_t ch = 0; ch < chan_no; ch++)
		axi_adc_write(axi_adc_core_desc, AXI_ADC_REG_CHAN_CNTRL(ch),
			      AXI_ADC_FORMAT_SIGNEXT | AXI_ADC_FORMAT_ENABLE |
			      AXI_ADC_ENABLE);

	no_os_mdelay(100);

	/* Finish AXI ADC initialization */
	axi_adc_write(axi_adc_core_desc, AXI_ADC_REG_CNTRL_3, AXI_ADC_CRC_EN);
	axi_adc_write(axi_adc_core_desc, AXI_ADC_REG_CNTRL,
		      (((chan_no) & 0x1F) << 8));

	/* Initialize AXI DMAC */
	ret = axi_dmac_init(&dma_desc, &dma_initial);
	if (ret != 0)
		goto error_2;

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
		goto error_3;
	ret = axi_dmac_transfer_wait_completion(dma_desc, 3000);
	if (ret)
		goto error_3;
	Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR, data_size);
	printf("Capture done\n");

	printf("   CH0      CH1      CH2      CH3      CH4      CH5      CH6      CH7   ");
	for (i = 0; i < (sample_no * chan_no); i++) {
		if ((i % chan_no) == 0)
			printf("\n\r");
		data_ptr = (uint32_t *)(ADC_DDR_BASEADDR + (i * sizeof(uint32_t)));
		*data_ptr = (*data_ptr) & ~0xFF000000;
		data = no_os_sign_extend32(*data_ptr, AD7768_RESOLUTION - 1);
		printf("%8.5f ", ((float)(data * 0.000000488)));
	}

#ifdef IIO_SUPPORT
	printf("The board accepts libiio clients connections through the serial backend.\n");

	struct xil_uart_init_param platform_uart_init_par = {
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
	};

	struct no_os_uart_init_param iio_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &platform_uart_init_par,
		.platform_ops = &xil_uart_ops
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_device *dev_desc;
	struct scan_type init_scan_type = {
		.sign = 's',
		.realbits = 24,
		.storagebits = 32,
		.shift = 0,
		.is_big_endian = false
	};
	struct iio_axi_adc_init_param iio_axi_adc_init_par = {
		.rx_adc = axi_adc_core_desc,
		.rx_dmac = dma_desc,
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange,
		.scan_type_common = &init_scan_type,
	};

	ret = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (ret)
		goto error_3;

	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &dev_desc);

	struct iio_data_buffer read_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = 400 * chan_no * AD7768_SAMPLE_SIZE / BITS_IN_BYTE,
	};

	struct iio_app_device devs[] = {
		IIO_APP_DEVICE("ad7768_dev", iio_axi_adc_desc, dev_desc,
			       &read_buff, NULL, NULL),
	};

	app_init_param.devices = devs;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devs);
	app_init_param.uart_init_params = iio_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		iio_axi_adc_remove(iio_axi_adc_desc);
		goto error_3;
	}

	return iio_app_run(app);
#endif

	/* Disable the instruction cache. */
	Xil_DCacheDisable();
	/* Disable the data cache. */
	Xil_ICacheDisable();

	//cleanup
	axi_dmac_remove(dma_desc);
	axi_adc_remove(axi_adc_core_desc);
	ad7768_remove(dev);
	return 0;

error_3:
	axi_dmac_remove(dma_desc);
error_2:
	axi_adc_remove(axi_adc_core_desc);
error_1:
	ad7768_remove(dev);
error:
	printf("Error %d.\n", ret);
	return ret;
}
