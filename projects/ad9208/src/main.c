/***************************************************************************//**
 *   @file   ad9208/src/main.c
 *   @brief  Implementation of Main Function.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include "hmc7044.h"
#include "ad9208.h"
#include "axi_adxcvr.h"
#include "axi_adc_core.h"
#include "axi_jesd204_rx.h"
#include "axi_dmac.h"
#include "parameters.h"
#include "xil_printf.h"
#include <xparameters.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_spi.h"
#include "xilinx_spi.h"
#include "no_os_gpio.h"
#include "xilinx_gpio.h"
#include <xil_cache.h>

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_axi_adc.h"
#include "xilinx_uart.h"

int32_t start_iiod(struct axi_adc *rx_0_adc, struct axi_adc *rx_1_adc,
		   struct axi_dmac *rx_dmac)
{
	struct iio_axi_adc_init_param	iio_axi_adc_0_init_par;
	struct iio_axi_adc_init_param	iio_axi_adc_1_init_par;
	struct iio_app_init_param app_init_param = { 0 };
	struct iio_axi_adc_desc		*iio_axi_adc_0_desc;
	struct iio_axi_adc_desc		*iio_axi_adc_1_desc;
	struct iio_device		*adc0_dev_desc;
	struct iio_device		*adc1_dev_desc;
	struct iio_data_buffer		rd_buff0;
	struct iio_data_buffer		rd_buff1;
	struct iio_app_desc *app;
	int32_t				status;
	uint32_t			buff_len;

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

	iio_axi_adc_0_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = rx_0_adc,
		.rx_dmac = rx_dmac
	};
	iio_axi_adc_1_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = rx_1_adc,
		.rx_dmac = rx_dmac
	};

	buff_len = (ADC_DDR_HIGHADDR - ADC_DDR_BASEADDR) / 2;
	rd_buff0 = (struct iio_data_buffer) {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = buff_len
	};

	rd_buff1 = (struct iio_data_buffer) {
		.buff = (void *)(ADC_DDR_BASEADDR + buff_len),
		.size = buff_len
	};

	status = iio_axi_adc_init(&iio_axi_adc_0_desc, &iio_axi_adc_0_init_par);
	if (NO_OS_IS_ERR_VALUE(status))
		return -1;

	iio_axi_adc_get_dev_descriptor(iio_axi_adc_0_desc, &adc0_dev_desc);

	status = iio_axi_adc_init(&iio_axi_adc_1_desc, &iio_axi_adc_1_init_par);
	if (NO_OS_IS_ERR_VALUE(status))
		return -1;

	iio_axi_adc_get_dev_descriptor(iio_axi_adc_1_desc, &adc1_dev_desc);

	struct iio_app_device devices[2] = {
		IIO_APP_DEVICE("axi_adc0", iio_axi_adc_0_desc, adc0_dev_desc,
			       &rd_buff0, NULL, NULL),
		IIO_APP_DEVICE("axi_adc1", iio_axi_adc_1_desc, adc1_dev_desc,
			       &rd_buff1, NULL, NULL),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	return iio_app_run(app);
}

#endif /*IIO_SUPPORT */

int main(void)
{
	struct xil_spi_init_param xil_spi_param = {
		.type = SPI_PL,
		.flags = 0
	};

	struct no_os_spi_init_param hmc7044_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 10000000,
		.mode = NO_OS_SPI_MODE_0,
		.chip_select = SPI_HMC7044_CS,
		.platform_ops = &xil_spi_ops,
		.extra = &xil_spi_param
	};

	struct hmc7044_chan_spec chan_spec[8] = {
		/* ADC_B_CLK */
		{
			.disable = 0, .num = 0, .divider = 1, .driver_mode = 1,
			.start_up_mode_dynamic_enable = false,
			.high_performance_mode_dis = false
		},
		/* ADC_B_SYSREF */
		{
			.disable = 0, .num = 1, .divider = 512, .driver_mode = 2,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true
		},
		/* ADC_A_CLK */
		{
			.disable = 0, .num = 2,  .divider = 1, .driver_mode = 1,
			.start_up_mode_dynamic_enable = false,
			.high_performance_mode_dis = false
		},
		/* ADC_A_SYSREF */
		{
			.disable = 0, .num = 3,  .divider = 512, .driver_mode = 2,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true
		},
		/* FMC_REFCLK0 */
		{
			.disable = 0, .num = 8,  .divider = 4, .driver_mode = 1,
			.start_up_mode_dynamic_enable = false,
			.high_performance_mode_dis = false
		},
		/* FMC_REFCLK1 */
		{
			.disable = 0, .num = 9,  .divider = 8, .driver_mode = 1,
			.start_up_mode_dynamic_enable = false,
			.high_performance_mode_dis = false
		},
		/* FMC_0_SYSREF */
		{
			.disable = 0, .num = 10, .divider = 512, .driver_mode = 2,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true
		},
		/* FMC_1_SYSREF */
		{
			.disable = 0, .num = 11, .divider = 512, .driver_mode = 2,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true
		},
	};

	struct hmc7044_init_param hmc7044_param = {
		.spi_init = &hmc7044_spi_param,
		.clkin_freq = {125000000, 0, 0, 0},
		.vcxo_freq = 125000000,
		.pll2_freq = 3000000000,
		.pll1_loop_bw = 200,
		.sysref_timer_div = 1024,
		.pulse_gen_mode = 7, /* HMC7044_PULSE_GEN_CONT_PULSE */
		.in_buf_mode = {0x07, 0, 0, 0, 0x03},
		.gpi_ctrl = {0x00, 0x00, 0x00, 0x00},
		.gpo_ctrl = {0x1f, 0x2b, 0x00, 0x00},
		.num_channels = 8,
		.channels = chan_spec,
	};

	struct no_os_spi_init_param ad9208_0_spi_param = {
		.max_speed_hz = 10000000,
		.mode = NO_OS_SPI_MODE_3,
		.chip_select = SPI_AD9208_0_CS,
		.platform_ops = &xil_spi_ops,
		.extra = &xil_spi_param
	};

	struct no_os_spi_init_param ad9208_1_spi_param = {
		.max_speed_hz = 10000000,
		.mode = NO_OS_SPI_MODE_3,
		.chip_select = SPI_AD9208_CS,
		.platform_ops = &xil_spi_ops,
		.extra = &xil_spi_param
	};

	struct ad9208_ddc ad9208_0_ddc_init[1] = {
		{
			.decimation = 2,
			.nco_mode = AD9208_NCO_MODE_VIF,
			.carrier_freq_hz = 70000000,
			.po = 0,
			.gain_db = true,
		},
	};

	struct ad9208_ddc ad9208_1_ddc_init[1] = {
		{
			.decimation = 2,
			.nco_mode = AD9208_NCO_MODE_VIF,
			.carrier_freq_hz = 70000000,
			.po = 0,
			.gain_db = true,
		},
	};

	jesd_param_t jesd_param_init = {
		.jesd_F = 1, /* octets per frame */
		.jesd_K = 32, /* frames per multiframe */
		.jesd_HD = false, /* high density */
		.jesd_N = 16, /* converter resolution */
		.jesd_NP = 16, /* bits per sample */
		.jesd_M = 2, /* converters per device */
		.jesd_CS = 0, /* control bits per sample */
		.jesd_L = 8, /* lanes per device */
	};

	struct adxcvr_init rx_0_adxcvr_init = {
		.name = "rx_0_adxcvr",
		.base = RX_0_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_REFCLK,
		.lpm_enable = 1,
		/* LaneRate = (M/L)*NP*(10/8)*DataRate */
		.lane_rate_khz = 15000000,
		/* FMC_REFCLK0, output 8 of HMC 7044 */
		.ref_rate_khz = 750000,
	};

	struct adxcvr_init rx_1_adxcvr_init = {
		.name = "rx_1_adxcvr",
		.base = RX_1_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_REFCLK,
		.lpm_enable = 1,
		/* LaneRate = (M/L)*NP*(10/8)*DataRate */
		.lane_rate_khz = 15000000,
		/* FMC_REFCLK0, output 8 of HMC 7044 */
		.ref_rate_khz = 750000,
	};

	struct jesd204_rx_init rx_0_jesd_init = {
		.name = "rx_0_jesd",
		.base = RX_0_JESD_BASEADDR,
		.octets_per_frame = 1,
		.frames_per_multiframe = 32,
		.subclass = 1,
		/* (lane_clk_khz / 40) output 9 of HMC 7044*/
		.device_clk_khz = 375000,
		/* LaneRate = (M/L)*NP*(10/8)*DataRate */
		.lane_clk_khz = 15000000,
	};

	struct jesd204_rx_init rx_1_jesd_init = {
		.name = "rx_1_jesd",
		.base = RX_1_JESD_BASEADDR,
		.octets_per_frame = 1,
		.frames_per_multiframe = 32,
		.subclass = 1,
		/* (lane_clk_khz / 40) output 9 of HMC 7044*/
		.device_clk_khz = 375000,
		/* LaneRate = (M/L)*NP*(10/8)*DataRate */
		.lane_clk_khz = 15000000,
	};

	struct axi_adc_init rx_0_adc_init = {
		.name = "rx_0_adc",
		.base = RX_0_CORE_BASEADDR,
		.num_channels = 2,
	};

	struct axi_adc_init rx_1_adc_init = {
		.name = "rx_1_adc",
		.base = RX_1_CORE_BASEADDR,
		.num_channels = 2,
	};

	struct axi_dmac_init rx_dmac_init = {
		.name = "rx_dmac",
		.base = RX_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED
	};

	struct xil_gpio_init_param xilinx_gpio_init_param = {
		.type = GPIO_PL,
		.device_id = GPIO_DEVICE_ID
	};

	struct ad9208_init_param ad9208_0_param = {
		.spi_init = &ad9208_0_spi_param,
		.gpio_powerdown = {
			.number = 54 + 34, /* adc0_pwdn */
			.platform_ops = &xil_gpio_ops,
			.extra = &xilinx_gpio_init_param
		},
		.sampling_frequency_hz = 3000000000,
		.input_div = 1,
		.powerdown_pin_en = false,
		.powerdown_mode = AD9208_POWERDOWN, /* Full Powerdown Mode */
		/* duty cycle stabilizer enable */
		.duty_cycle_stabilizer_en = true,
		.current_scale = AD9208_ADC_SCALE_1P7_VPP,
		/* analog input dc coupling enable */
		.analog_input_mode = COUPLING_AC,
		.ext_vref_en = false,
		/* analog input neg buffer current */
		.buff_curr_n = AD9208_BUFF_CURR_600_UA,
		/* analog input pos buffer current */
		.buff_curr_p = AD9208_BUFF_CURR_600_UA,
		/* Number of Carrier Frequencies */
		.fc_ch = AD9208_FULL_BANDWIDTH_MODE,
		.ddc = ad9208_0_ddc_init,
		.ddc_cnt = NO_OS_ARRAY_SIZE(ad9208_0_ddc_init),
		/* ddc complex to real enable */
		.ddc_output_format_real_en = false,
		/* ddc mixer real enable */
		.ddc_input_format_real_en = false,
		.test_mode_ch0 = AD9208_TESTMODE_OFF,
		.test_mode_ch1 = AD9208_TESTMODE_RAMP,

		/* SYSREF Config */
		.sysref_lmfc_offset = 0,
		.sysref_edge_sel = false,
		.sysref_clk_edge_sel = false,
		.sysref_neg_window_skew = 0,
		.sysref_pos_window_skew = 0,
		.sysref_mode = AD9208_SYSREF_ONESHOT,
		.sysref_count = 15,
		.jesd_param = &jesd_param_init,
		.jesd_subclass = 1,
	};

	struct ad9208_init_param ad9208_1_param = {
		.spi_init = &ad9208_1_spi_param,
		.gpio_powerdown = {
			.number = 54 + 33, /* adc1_pwdn */
			.platform_ops = &xil_gpio_ops,
			.extra = &xilinx_gpio_init_param
		},
		.sampling_frequency_hz = 3000000000,
		.input_div = 1,
		.powerdown_pin_en = false,
		.powerdown_mode = AD9208_POWERDOWN, /* Full Powerdown Mode */
		.duty_cycle_stabilizer_en = true,
		/* duty cycle stabilizer enable */
		.current_scale = AD9208_ADC_SCALE_1P7_VPP,
		/* analog input dc coupling enable */
		.analog_input_mode = COUPLING_AC,
		.ext_vref_en = false,
		/* analog input neg buffer current */
		.buff_curr_n = AD9208_BUFF_CURR_600_UA,
		/* analog input pos buffer current */
		.buff_curr_p = AD9208_BUFF_CURR_600_UA,
		/* Number of Carrier Frequencies */
		.fc_ch = AD9208_FULL_BANDWIDTH_MODE,
		.ddc = ad9208_1_ddc_init,
		.ddc_cnt = NO_OS_ARRAY_SIZE(ad9208_1_ddc_init),
		/* ddc complex to real enable */
		.ddc_output_format_real_en = false,
		/* ddc mixer real enable */
		.ddc_input_format_real_en = false,
		.test_mode_ch0 = AD9208_TESTMODE_OFF,
		.test_mode_ch1 = AD9208_TESTMODE_ONE_ZERO_TOGGLE,

		/* SYSREF Config */
		.sysref_lmfc_offset = 0,
		.sysref_edge_sel = false,
		.sysref_clk_edge_sel = false,
		.sysref_neg_window_skew = 0,
		.sysref_pos_window_skew = 0,
		.sysref_mode = AD9208_SYSREF_ONESHOT,
		.sysref_count = 15,
		.jesd_param = &jesd_param_init,
		.jesd_subclass = 1,
	};

	struct hmc7044_dev *hmc7044_device;
	struct ad9208_dev *ad9208_0_device, *ad9208_1_device;
	struct axi_jesd204_rx *rx_0_jesd, *rx_1_jesd;
	struct adxcvr *rx_0_adxcvr, *rx_1_adxcvr;
	struct axi_adc *rx_0_adc, *rx_1_adc;
	struct axi_dmac *rx_dmac;
	int32_t status;
	uint32_t size;

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

	status = hmc7044_init(&hmc7044_device, &hmc7044_param);
	if (status != 0) {
		xil_printf("hmc7044_init() error: %"PRIi32"\n", status);
		return status;
	}

	status = ad9208_initialize(&ad9208_0_device, &ad9208_0_param);
	if (status != 0) {
		xil_printf("ad9208_0_init() error: %"PRIi32"\n", status);
		goto error_1;
	}

	status = ad9208_remove(ad9208_0_device);
	if (status != 0) {
		xil_printf("ad9208_remove() error: %"PRIi32"\n", status);
		goto error_1;
	}

	no_os_mdelay(100);

	status = ad9208_initialize(&ad9208_1_device, &ad9208_1_param);
	if (status != 0) {
		xil_printf("ad9208_1_init() error: %"PRIi32"\n", status);
		goto error_1;
	}

	status = ad9208_remove(ad9208_1_device);
	if (status != 0) {
		xil_printf("ad9208_remove() error: %"PRIi32"\n", status);
		goto error_1;
	}

	status = adxcvr_init(&rx_0_adxcvr, &rx_0_adxcvr_init);
	if (status != 0) {
		xil_printf("error: %s: adxcvr_init() failed\n",
			   rx_0_adxcvr_init.name);
		goto error_1;
	}

	status = adxcvr_clk_enable(rx_0_adxcvr);
	if (status != 0) {
		xil_printf("error: %s: adxcvr_clk_enable() failed\n",
			   rx_0_adxcvr->name);
		goto error_2;
	}

	status = axi_jesd204_rx_init_legacy(&rx_0_jesd, &rx_0_jesd_init);
	if (status != 0) {
		xil_printf("error: %s: axi_jesd204_rx_init_legacy() failed\n",
			   rx_0_jesd_init.name);
		goto error_3;
	}

	status = axi_jesd204_rx_lane_clk_enable(rx_0_jesd);
	if (status != 0) {
		xil_printf("error: %s: axi_jesd204_rx_lane_clk_enable() err\n",
			   rx_0_jesd->name);
		goto error_4;
	}

	status = axi_jesd204_rx_status_read(rx_0_jesd);
	if (status != 0) {
		xil_printf("axi_jesd204_rx_status_read() error: %"PRIi32"\n",
			   status);
		goto error_5;
	}

	/* Initialize the ADC core */
	status = axi_adc_init(&rx_0_adc, &rx_0_adc_init);
	if (status != 0) {
		xil_printf("axi_adc_init() error: %"PRIi32"\n", status);
		goto error_5;
	}

	status = adxcvr_init(&rx_1_adxcvr, &rx_1_adxcvr_init);
	if (status != 0) {
		xil_printf("error: %s: adxcvr_init() failed\n",
			   rx_1_adxcvr_init.name);
		goto error_6;
	}

	status = adxcvr_clk_enable(rx_1_adxcvr);
	if (status != 0) {
		xil_printf("error: %s: adxcvr_clk_enable() failed\n",
			   rx_1_adxcvr->name);
		goto error_7;
	}

	status = axi_jesd204_rx_init_legacy(&rx_1_jesd, &rx_1_jesd_init);
	if (status != 0) {
		xil_printf("error: %s: axi_jesd204_rx_init_legacy() failed\n",
			   rx_1_jesd_init.name);
		goto error_8;
	}

	status = axi_jesd204_rx_lane_clk_enable(rx_1_jesd);
	if (status != 0) {
		xil_printf("error: %s: axi_jesd204_rx_lane_clk_enable() err\n",
			   rx_1_jesd->name);
		goto error_9;
	}

	status = axi_jesd204_rx_status_read(rx_1_jesd);
	if (status != 0) {
		xil_printf("axi_jesd204_rx_status_read() error: %"PRIi32"\n",
			   status);
		goto error_10;
	}

	/* Initialize the ADC core */
	status = axi_adc_init(&rx_1_adc, &rx_1_adc_init);
	if (status != 0) {
		xil_printf("axi_adc_init() error: %"PRIi32"\n", status);
		goto error_10;
	}

	/* Initialize the DMAC and transfer 16384 samples from ADC to MEM */
	status = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (status != 0) {
		xil_printf("axi_dmac_init() error: %"PRIi32"\n", status);
		goto error_11;
	}

	size = (rx_1_adc->num_channels + rx_0_adc->num_channels) * sizeof(uint16_t);
	struct axi_dma_transfer read_transfer = {
		// Number of bytes to write/read
		.size = 16384 * size,
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)ADC_DDR_BASEADDR
	};
	status = axi_dmac_transfer_start(rx_dmac, &read_transfer);
	if (status != 0)
		xil_printf("axi_dmac_transfer_start() error: %"PRIi32"\n", status);
	/* Wait until transfer finishes */
	status = axi_dmac_transfer_wait_completion(rx_dmac, 500);
	if(status)
		return status;
	Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR, 16384 * size);

#ifdef IIO_SUPPORT
	status = start_iiod(rx_0_adc, rx_1_adc, rx_dmac);
#endif

error_11:
	axi_adc_remove(rx_1_adc);
error_10:
	axi_jesd204_rx_lane_clk_disable(rx_1_jesd);
error_9:
	axi_jesd204_rx_remove(rx_1_jesd);
error_8:
	adxcvr_clk_disable(rx_1_adxcvr);
error_7:
	adxcvr_remove(rx_1_adxcvr);
error_6:
	axi_adc_remove(rx_0_adc);
error_5:
	axi_jesd204_rx_lane_clk_disable(rx_0_jesd);
error_4:
	axi_jesd204_rx_remove(rx_0_jesd);
error_3:
	adxcvr_clk_disable(rx_0_adxcvr);
error_2:
	adxcvr_remove(rx_0_adxcvr);
error_1:
	hmc7044_remove(hmc7044_device);

	xil_printf("Bye\n");

	/* Disable the instruction cache. */
	Xil_ICacheDisable();
	/* Disable the data cache. */
	Xil_DCacheDisable();

	return 0;
}
