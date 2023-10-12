/***************************************************************************//**
 *   @file   fmcjesdadc1.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Antoniu Miclaus(antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2014(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <inttypes.h>
#include "ad9517.h"
#include "ad9250.h"
#include "parameters.h"
#include <xparameters.h>
#include <xil_printf.h>
#include <xil_cache.h>
#include "axi_adxcvr.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "xilinx_spi.h"
#include "xilinx_gpio.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"
#include "axi_jesd204_rx.h"
#include "demux_spi.h"

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

	int32_t status;
	uint32_t reg_value;

	// SPI configuration
	struct no_os_spi_init_param demux_spi_param = {
		.max_speed_hz = 2000000u,
		.chip_select = 0,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = &xil_spi_ops
	};

	struct no_os_spi_init_param ad9250_0_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = demux_spi_param.max_speed_hz,
		.chip_select = 0,
		.mode = demux_spi_param.mode,
		.platform_ops = &demux_spi_platform_ops
	};

	struct no_os_spi_init_param ad9250_1_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = demux_spi_param.max_speed_hz,
		.chip_select = 1,
		.mode = demux_spi_param.mode,
		.platform_ops = &demux_spi_platform_ops
	};

	struct no_os_spi_init_param ad9517_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = demux_spi_param.max_speed_hz,
		.chip_select = 4,
		.mode = demux_spi_param.mode,
		.platform_ops = &demux_spi_platform_ops
	};

	struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
	};

	demux_spi_param.extra = &xil_spi_param;
	ad9250_0_spi_param.extra = &demux_spi_param;
	ad9250_1_spi_param.extra = &demux_spi_param;
	ad9517_spi_param.extra = &demux_spi_param;

	struct no_os_gpio_init_param gpio_sysref_param = {
		.number = GPIO_JESD204_SYSREF,
		.platform_ops = &xil_gpio_ops
	};

	struct xil_gpio_init_param xil_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID
	};
	gpio_sysref_param.extra = &xil_gpio_param;

	struct no_os_gpio_desc *gpio_sysref;

	struct adxcvr_init ad9250_xcvr_param = {
		.name = "ad9250_xcvr",
		.base = RX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_CPLL,
		.out_clk_sel = ADXCVR_OUTCLK_PMA,
		.lpm_enable = 0,
		.ref_rate_khz = 245760,
		.lane_rate_khz = 4915200,
	};
	struct adxcvr *ad9250_xcvr;

	struct jesd204_rx_init  ad9250_jesd_param = {
		.name = "ad9250_jesd",
		.base = RX_JESD_BASEADDR,
		.octets_per_frame = 2,
		.frames_per_multiframe = 32,
		.subclass = 1,
		.device_clk_khz = 4915200 / 40,
		.lane_clk_khz = 4915200
	};
	struct axi_jesd204_rx *ad9250_jesd;

	struct axi_adc_init ad9250_core_param = {
		.name = "ad9250_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = 4
	};
	struct axi_adc	*ad9250_core;

	struct axi_dmac_init ad9250_dmac_param = {
		.name = "ad9250_dmac",
		.base = RX_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED
	};
	struct axi_dmac *ad9250_dmac;

	struct ad9517_platform_data ad9517_pdata_lpc = {
		/* PLL Reference */
		.ref_1_freq = 30720000,
		.ref_2_freq = 0,
		.diff_ref_en = 0,
		.ref_1_power_on = 1,
		.ref_2_power_on = 0,
		.ref_sel_pin_en = 0,
		.ref_sel_pin = 1,
		.ref_2_en = 0,
		.ext_clk_freq = 0,
		.int_vco_freq = 2500000000,
		.vco_clk_sel = 1,
		.power_down_vco_clk = 0,
		.name = "ad9517-lpc"
	};

	struct ad9517_lvpecl_channel_spec ad9517_lvpecl_channels[] = {
		{
			.channel_num = 0,
			.out_invert_en = 0,
			.out_diff_voltage = LVPECL_780mV,
			.name = "CH0"
		},
		{
			.channel_num = 1,
			.out_invert_en = 0,
			.out_diff_voltage = LVPECL_780mV,
			.name = "CH1"
		},
		{
			.channel_num = 2,
			.out_invert_en = 0,
			.out_diff_voltage = LVPECL_780mV,
			.name = "CH2"
		},
		{
			.channel_num = 3,
			.out_invert_en = 0,
			.out_diff_voltage = LVPECL_780mV,
			.name = "CH3"
		}
	};

	struct ad9517_lvds_cmos_channel_spec ad9517_lvds_cmos_channels[] = {
		{
			.channel_num = 4,
			.out_invert = 0,
			.logic_level = LVDS,
			.cmos_b_en = 0,
			.out_lvds_current = LVDS_3_5mA,
			.name = "CH4"
		},
		{
			.channel_num = 5,
			.out_invert = 0,
			.logic_level = LVDS,
			.cmos_b_en = 0,
			.out_lvds_current = LVDS_3_5mA,
			.name = "CH5"
		},
		{
			.channel_num = 6,
			.out_invert = 0,
			.logic_level = LVDS,
			.cmos_b_en = 0,
			.out_lvds_current = LVDS_3_5mA,
			.name = "CH6"
		},
		{
			.channel_num = 7,
			.out_invert = 0,
			.logic_level = LVDS,
			.cmos_b_en = 0,
			.out_lvds_current = LVDS_3_5mA,
			.name = "CH7"
		}
	};

	struct ad9250_platform_data ad9250_pdata_lpc = {
		.extrn_pdwnmode = 0,
		.en_clk_dcs = 1,
		.clk_selection = 0,
		.clk_div_ratio = 0,
		.clk_div_phase = 0,
		.adc_vref = 15,
		.pll_low_encode = 0,
		.name = "ad9250-lpc"
	};

	struct ad9250_jesd204b_cfg ad9250_0_jesd204b_interface = {
		.jtx_in_standby = 0,
		.cml_level = 3,
		.quick_cfg_option = 0x22,
		.subclass = 1,
		.ctrl_bits_no = 0,
		.ctrl_bits_assign = 0,
		.tail_bits_mode = 0,
		.did = 0xF0,
		.bid = 0x00,
		.lid0 = 0x00,
		.lid1 = 0x01,
		.k = 32,
		.scrambling = 1,
		.ilas_mode = 1,
		.invert_logic_bits = 0,
		.en_ilas_test = 0,
		.en_sys_ref = 1,
		.en_sync_in_b = 1,
		.sys_ref_mode = 0,
		.align_sync_in_b = 1,
		.align_sys_ref = 0,
		.lane0_assign = 0,
		.lane1_assign = 1,
	};

	struct ad9250_jesd204b_cfg ad9250_1_jesd204b_interface = {
		.jtx_in_standby = 0,
		.cml_level = 3,
		.quick_cfg_option = 0x22,
		.subclass = 1,
		.ctrl_bits_no = 0,
		.ctrl_bits_assign = 0,
		.tail_bits_mode = 0,
		.did  = 0xF0,
		.bid = 0x00,
		.lid0 = 0x02,
		.lid1 = 0x03,
		.k = 32,
		.scrambling = 1,
		.ilas_mode = 1,
		.invert_logic_bits = 0,
		.en_ilas_test = 0,
		.en_sys_ref = 1,
		.en_sync_in_b = 1,
		.sys_ref_mode = 0,
		.align_sync_in_b = 1,
		.align_sys_ref = 0,
		.lane0_assign = 0,
		.lane1_assign = 1,
	};

	struct ad9250_fast_detect_cfg ad9250_fast_detect = {
		.en_fd = 0,
		.pin_function = 0,
		.force_pins = 0,
		.pin_force_value = 0,
		.fd_upper_tresh = 0,
		.fd_lower_tresh = 0,
		.df_dwell_time = 0,
	};

	struct ad9250_dev		*ad9250_0_device;
	struct ad9250_dev		*ad9250_1_device;
	struct ad9517_dev		*ad9517_device;
	struct ad9250_init_param	ad9250_0_param;
	struct ad9250_init_param	ad9250_1_param;
	struct ad9517_init_param	ad9517_param;

	ad9517_param.ad9517_type = AD9517_1;
	ad9517_param.ad9517_st.pdata = &ad9517_pdata_lpc;
	ad9517_param.ad9517_st.lvpecl_channels = &ad9517_lvpecl_channels[0];
	ad9517_param.ad9517_st.lvds_cmos_channels = &ad9517_lvds_cmos_channels[0];

	ad9250_0_param.ad9250_st_init.pdata = &ad9250_pdata_lpc;
	ad9250_0_param.ad9250_st_init.p_jesd204b = &ad9250_0_jesd204b_interface;
	ad9250_0_param.ad9250_st_init.p_fd = &ad9250_fast_detect;

	ad9250_1_param.ad9250_st_init.pdata = &ad9250_pdata_lpc;
	ad9250_1_param.ad9250_st_init.p_jesd204b = &ad9250_1_jesd204b_interface;
	ad9250_1_param.ad9250_st_init.p_fd = &ad9250_fast_detect;

	// SPI configuration
	ad9517_param.spi_init = ad9517_spi_param;
	ad9250_0_param.spi_init = ad9250_0_spi_param;
	ad9250_1_param.spi_init = ad9250_1_spi_param;

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

	// setup GPIOs
	no_os_gpio_get(&gpio_sysref,  &gpio_sysref_param);
	no_os_gpio_direction_output(gpio_sysref,  1);
	no_os_mdelay(10);

	// set up clock generator
	status = ad9517_setup(&ad9517_device, ad9517_param);
	if(status < 0) {
		printf("Error ad9517_setup()\n");
		return -1;
	}

	/* Set the channel 0 power mode on */
	status = ad9517_power_mode(ad9517_device, 0, 0);
	if(status < 0) {
		printf("Error channel 0 ad9517_power_mode()\n");
		return -1;
	}
	/* Set the channel 0 frequency to 250Mhz */
	status = ad9517_frequency(ad9517_device, 0, 250000000);
	if(status < 0) {
		printf("Error channel 0 ad9517_frequency()\n");
		return -1;
	}

	/* Set the channel 1 power mode on */
	status = ad9517_power_mode(ad9517_device, 1, 0);
	if(status < 0) {
		printf("Error channel 1 ad9517_power_mode()\n");
		return -1;
	}

	/* Set the channel 1 frequency to 250Mhz */
	status = ad9517_frequency(ad9517_device, 1, 250000000);
	if(status < 0) {
		printf("Error channel 1 ad9517_frequency()\n");
		return -1;
	}

	/* Set the channel 4 power mode on */
	status = ad9517_power_mode(ad9517_device, 4, 0);
	if(status < 0) {
		printf("Error channel 4 ad9517_power_mode()\n");
		return -1;
	}

	/* Set the channel 4 frequency to 250Mhz */
	status = ad9517_frequency(ad9517_device, 4, 250000000);
	if(status < 0) {
		printf("Error channel 4 ad9517_frequency()\n");
		return -1;
	}

	/* Set the channel 5 power mode on */
	status = ad9517_power_mode(ad9517_device, 5, 0);
	if(status < 0) {
		printf("Error channel 5 ad9517_power_mode()\n");
		return -1;
	}

	/* Set the channel 5 frequency to 250Mhz */
	status = ad9517_frequency(ad9517_device, 5, 250000000);
	if(status < 0) {
		printf("Error channel 5 ad9517_frequency()\n");
		return -1;
	}

	/* Set the channel 6 power mode on */
	status = ad9517_power_mode(ad9517_device, 6, 0);
	if(status < 0) {
		printf("Error channel 6 ad9517_power_mode()\n");
		return -1;
	}

	/* Set the channel 6 frequency to 250Mhz */
	status = ad9517_frequency(ad9517_device, 6, 250000000);
	if(status < 0) {
		printf("Error channel 6 ad9517_frequency()\n");
		return -1;
	}

	/* Set the channel 7 power mode on */
	status = ad9517_power_mode(ad9517_device, 7, 0);
	if(status < 0) {
		printf("Error channel 7 ad9517_power_mode()\n");
		return -1;
	}

	/* Set the channel 7 frequency to 250Mhz */
	status = ad9517_frequency(ad9517_device, 7, 250000000);
	if(status < 0) {
		printf("Error channel 7 ad9517_frequency()\n");
		return -1;
	}

	status = ad9517_update(ad9517_device);
	if(status < 0) {
		printf("Error ad9517_update()\n");
		return -1;
	}

	status = ad9517_read(ad9517_device, AD9517_REG_PLL_READBACK, &reg_value);
	if (status < 0) {
		printf("Error ad9517_read()!\n");
		return -1;
	} else if (!(reg_value & AD9517_DIGITAL_LOCK_DETECT)) {
		printf("AD9517 PLL not locked!\n");
		return -1;
	}
	printf("AD9517 PLL locked.\n");

	// set up the devices
	status= ad9250_setup(&ad9250_0_device, ad9250_0_param);
	if(status< 0)
		printf("Error ad9250_0_setup()\n");

	status= ad9250_setup(&ad9250_1_device, ad9250_1_param);
	if(status< 0)
		printf("Error ad9250_1_setup()\n");

	// set up the XCVR core
	status = adxcvr_init(&ad9250_xcvr, &ad9250_xcvr_param);
	if (status != 0) {
		printf("error: %s: adxcvr_init() failed\n", ad9250_xcvr->name);
	}
	status = adxcvr_clk_enable(ad9250_xcvr);
	if (status != 0) {
		printf("error: %s: adxcvr_clk_enable() failed\n", ad9250_xcvr->name);
	}

	// setup JESD core
	status = axi_jesd204_rx_init_legacy(&ad9250_jesd, &ad9250_jesd_param);
	if (status != 0) {
		printf("error: %s: axi_jesd204_rx_init_legacy() failed\n", ad9250_jesd->name);
	}
	status = axi_jesd204_rx_lane_clk_enable(ad9250_jesd);
	if (status != 0) {
		printf("error: %s: axi_jesd204_tx_lane_clk_enable() failed\n",
		       ad9250_jesd->name);
	}

	// interface core setup
	status = axi_adc_init(&ad9250_core,  &ad9250_core_param);
	if (status != 0) {
		printf("axi_adc_init() error: %s\n", ad9250_core->name);
	}

	// JESD core status
	status = axi_jesd204_rx_status_read(ad9250_jesd);
	if (status != 0) {
		printf("axi_jesd204_rx_status_read() error: %"PRIi32"\n", status);
	}

	//ADC output_format
	ad9250_output_format(ad9250_0_device, AD9250_OUT_OFFSET_BINARY);
	ad9250_output_format(ad9250_1_device, AD9250_OUT_OFFSET_BINARY);

	// PRBS test
	ad9250_test_mode(ad9250_0_device, AD9250_TEST_PNLONG);
	ad9250_transfer(ad9250_0_device);

	ad9250_test_mode(ad9250_1_device, AD9250_TEST_PNLONG);
	ad9250_transfer(ad9250_1_device);
	if(axi_adc_pn_mon(ad9250_core, AXI_ADC_PN23, 10) == -1) {
		printf("%s ad9250 - PN23 sequence mismatch!\n", __func__);
	};

	// set up ramp output
	ad9250_test_mode(ad9250_0_device, AD9250_TEST_RAMP);
	ad9250_transfer(ad9250_0_device);
	ad9250_test_mode(ad9250_1_device, AD9250_TEST_RAMP);
	ad9250_transfer(ad9250_1_device);

	// test the captured data
	axi_dmac_init(&ad9250_dmac, &ad9250_dmac_param);
	struct axi_dma_transfer read_transfer = {
		// Number of bytes to write/read
		.size = 16384 * 4,
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)ADC_DDR_BASEADDR
	};
	axi_dmac_transfer_start(ad9250_dmac, &read_transfer);
	status = axi_dmac_transfer_wait_completion(ad9250_dmac, 500);
	if(status)
		return status;
	Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR,16384 * 4);
	Xil_DCacheFlush();

	// set up normal output
	ad9250_test_mode(ad9250_0_device, AD9250_TEST_OFF);
	ad9250_transfer(ad9250_0_device);
	ad9250_test_mode(ad9250_1_device, AD9250_TEST_OFF);
	ad9250_transfer(ad9250_1_device);

	// capture data with DMA
	struct axi_dma_transfer read_transfer_capture = {
		// Number of bytes to write/read
		.size = 16384 * 4,
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)ADC_DDR_BASEADDR
	};
	axi_dmac_transfer_start(ad9250_dmac, &read_transfer_capture);
	status = axi_dmac_transfer_wait_completion(ad9250_dmac, 500);
	if(status)
		return status;
	Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR,16384 * 4);
	Xil_DCacheFlush();

#ifdef IIO_SUPPORT
	printf("The board accepts libiio clients connections through the serial backend.\n");

	struct xil_uart_init_param platform_uart_init_par = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
		.type = UART_PL,
#else
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
#endif
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
	struct iio_device *adc_dev_desc;

	struct iio_axi_adc_init_param iio_axi_adc_init_par = {
		.rx_adc = ad9250_core,
		.rx_dmac = ad9250_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
#endif
	};

	status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (status < 0)
		return status;

	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);

	struct iio_data_buffer g_read_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("axi_adc", iio_axi_adc_desc, adc_dev_desc,
			       &g_read_buff, NULL, NULL),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	iio_app_run(app);

#endif

	printf("adc1: setup and configuration is done\n");

	ad9517_remove(ad9517_device);
	ad9250_remove(ad9250_0_device);
	ad9250_remove(ad9250_1_device);
	no_os_gpio_remove(gpio_sysref);

	/* Disable the instruction cache. */
	Xil_ICacheDisable();
	/* Disable the data cache. */
	Xil_DCacheDisable();

	return 0;
}
