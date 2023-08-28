/***************************************************************************//**
 *   @file   app.c
 *   @brief  AD9081 application example.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#include "no_os_error.h"
#include "no_os_gpio.h"
#include "xilinx_gpio.h"
#include "no_os_spi.h"
#include "xilinx_spi.h"
#include "ad9081.h"
#include "hmc7044.h"
#include "app_clock.h"
#include "app_jesd.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"
#include "parameters.h"
#include "app_config.h"
#include "xil_cache.h"

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_axi_adc.h"
#include "iio_axi_dac.h"
#include "xilinx_uart.h"
#endif

#ifdef IIO_SUPPORT

static int16_t dac_buffer[MAX_DAC_BUF_SAMPLES] __attribute__ ((aligned));
static int16_t adc_buffer[MAX_ADC_BUF_SAMPLES] __attribute__ ((aligned));

#endif

extern struct axi_jesd204_rx *rx_jesd;
extern struct axi_jesd204_tx *tx_jesd;
extern struct hmc7044_dev* hmc7044_dev;

int main(void)
{
	struct no_os_clk app_clk[MULTIDEVICE_INSTANCE_COUNT];
	struct no_os_clk jesd_clk[2];
	struct xil_gpio_init_param  xil_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID
	};
	struct no_os_gpio_init_param	gpio_phy_resetb = {
		.number = PHY_RESET,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_param
	};
	struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
	};
	struct no_os_spi_init_param phy_spi_init_param = {
		.device_id = PHY_SPI_DEVICE_ID,
		.max_speed_hz = 1000000,
		.mode = NO_OS_SPI_MODE_0,
		.chip_select = PHY_CS,
		.platform_ops = &xil_spi_ops,
		.extra = &xil_spi_param
	};
	struct link_init_param jrx_link_tx = {
		.device_id = 0,
		.octets_per_frame = AD9081_TX_JESD_F,
		.frames_per_multiframe = AD9081_TX_JESD_K,
		.samples_per_converter_per_frame = AD9081_TX_JESD_S,
		.high_density = AD9081_TX_JESD_HD,
		.converter_resolution = AD9081_TX_JESD_N,
		.bits_per_sample = AD9081_TX_JESD_NP,
		.converters_per_device = AD9081_TX_JESD_M,
		.control_bits_per_sample = AD9081_TX_JESD_CS,
		.lanes_per_device = AD9081_TX_JESD_L,
		.subclass = AD9081_TX_JESD_SUBCLASS,
		.link_mode = AD9081_TX_JESD_MODE,
		.dual_link = 0,
		.version = AD9081_TX_JESD_VERSION,
		.logical_lane_mapping = AD9081_TX_LOGICAL_LANE_MAPPING,
		.tpl_phase_adjust = AD9081_JRX_TPL_PHASE_ADJUST
	};
	struct link_init_param jtx_link_rx = {
		.device_id = 0,
		.octets_per_frame = AD9081_RX_JESD_F,
		.frames_per_multiframe = AD9081_RX_JESD_K,
		.samples_per_converter_per_frame = AD9081_RX_JESD_S,
		.high_density = AD9081_RX_JESD_HD,
		.converter_resolution = AD9081_RX_JESD_N,
		.bits_per_sample = AD9081_RX_JESD_NP,
		.converters_per_device = AD9081_RX_JESD_M,
		.control_bits_per_sample = AD9081_RX_JESD_CS,
		.lanes_per_device = AD9081_RX_JESD_L,
		.subclass = AD9081_RX_JESD_SUBCLASS,
		.link_mode = AD9081_RX_JESD_MODE,
		.dual_link = 0,
		.version = AD9081_RX_JESD_VERSION,
		.logical_lane_mapping = AD9081_RX_LOGICAL_LANE_MAPPING,
		.link_converter_select = AD9081_RX_LINK_CONVERTER_SELECT,
	};
	struct ad9081_init_param phy_param = {
		.gpio_reset = &gpio_phy_resetb,
		.spi_init = &phy_spi_init_param,
		.dev_clk = &app_clk[0],
		.jesd_tx_clk = &jesd_clk[1],
		.jesd_rx_clk = &jesd_clk[0],
		.sysref_coupling_ac_en = 0,
		.sysref_cmos_input_enable = 0,
		.config_sync_0a_cmos_enable = 0,
		.multidevice_instance_count = 1,
#ifdef QUAD_MXFE
		.jesd_sync_pins_01_swap_enable = true,
#else
		.jesd_sync_pins_01_swap_enable = false,
#endif
		.lmfc_delay_dac_clk_cycles = 0,
		.nco_sync_ms_extra_lmfc_num = 0,
		.nco_sync_direct_sysref_mode_enable = 0,
		.sysref_average_cnt_exp = 7,
		.continuous_sysref_mode_disable = 0,
		.tx_disable = false,
		.rx_disable = false,
		/* TX */
		.dac_frequency_hz = AD9081_DAC_FREQUENCY,
		/* The 4 DAC Main Datapaths */
		.tx_main_interpolation = AD9081_TX_MAIN_INTERPOLATION,
		.tx_main_nco_frequency_shift_hz = AD9081_TX_MAIN_NCO_SHIFT,
		.tx_dac_channel_crossbar_select = AD9081_TX_DAC_CHAN_CROSSBAR,
		/* The 8 DAC Channelizers */
		.tx_channel_interpolation = AD9081_TX_CHAN_INTERPOLATION,
		.tx_channel_nco_frequency_shift_hz = AD9081_TX_CHAN_NCO_SHIFT,
		.tx_channel_gain = AD9081_TX_CHAN_GAIN,
		.jrx_link_tx[0] = &jrx_link_tx,
		.jrx_link_tx[1] = NULL,
		/* RX */
		.adc_frequency_hz = AD9081_ADC_FREQUENCY,
		.nyquist_zone = AD9081_ADC_NYQUIST_ZONE,
		/* The 4 ADC Main Datapaths */
		.rx_main_nco_frequency_shift_hz = AD9081_RX_MAIN_NCO_SHIFT,
		.rx_main_decimation = AD9081_RX_MAIN_DECIMATION,
		.rx_main_complex_to_real_enable = {0, 0, 0, 0},
		.rx_main_enable = AD9081_RX_MAIN_ENABLE,
		/* The 8 ADC Channelizers */
		.rx_channel_nco_frequency_shift_hz = AD9081_RX_CHAN_NCO_SHIFT,
		.rx_channel_decimation = AD9081_RX_CHAN_DECIMATION,
		.rx_channel_complex_to_real_enable = {0, 0, 0, 0, 0, 0, 0, 0},
		.rx_channel_enable = AD9081_RX_CHAN_ENABLE,
		.jtx_link_rx[0] = &jtx_link_rx,
		.jtx_link_rx[1] = NULL,
	};

	struct axi_adc_init rx_adc_init = {
		.name = "rx_adc",
		.base = RX_CORE_BASEADDR
	};
	struct axi_adc *rx_adc;
	struct axi_dac_init tx_dac_init = {
		.name = "tx_dac",
		.base = TX_CORE_BASEADDR,
		.channels = NULL,
		.rate = 3
	};
	struct axi_dac *tx_dac;
	struct axi_dmac_init rx_dmac_init = {
		"rx_dmac",
		RX_DMA_BASEADDR,
		IRQ_DISABLED
	};
	struct axi_dmac *rx_dmac;
	struct axi_dmac_init tx_dmac_init = {
		"tx_dmac",
		TX_DMA_BASEADDR,
		IRQ_DISABLED
	};
	struct axi_dmac *tx_dmac;
	struct ad9081_phy* phy[MULTIDEVICE_INSTANCE_COUNT];
	int32_t status;
	int32_t i;

	printf("Hello\n");

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

#ifdef QUAD_MXFE
	struct xil_gpio_init_param  xil_gpio_param_2 = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_2_DEVICE_ID
	};
	struct no_os_gpio_init_param	ad9081_gpio0_mux_init = {
		.number = AD9081_GPIO_0_MUX,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_param_2
	};
	struct no_os_gpio_desc *ad9081_gpio0_mux;

	status = no_os_gpio_get(&ad9081_gpio0_mux, &ad9081_gpio0_mux_init);
	if (status)
		return status;

	status = no_os_gpio_set_value(ad9081_gpio0_mux, 1);
	if (status)
		return status;
#endif

	status = app_clock_init(app_clk);
	if (status != 0)
		printf("app_clock_init() error: %" PRId32 "\n", status);

	status = app_jesd_init(jesd_clk,
			       500000, 250000, 250000, 10000000, 10000000);
	if (status != 0)
		printf("app_jesd_init() error: %" PRId32 "\n", status);

	rx_adc_init.num_channels = 0;
	tx_dac_init.num_channels = 0;

	for (i = 0; i < MULTIDEVICE_INSTANCE_COUNT; i++) {
		gpio_phy_resetb.number = PHY_RESET + i;
		phy_spi_init_param.chip_select = PHY_CS + i;
		phy_param.dev_clk = &app_clk[i];
		jtx_link_rx.device_id = i;

		status = ad9081_init(&phy[i], &phy_param);
		if (status != 0)
			printf("ad9081_init() error: %" PRId32 "\n", status);

		rx_adc_init.num_channels += phy[i]->jtx_link_rx[0].jesd_param.jesd_m +
					    phy[i]->jtx_link_rx[1].jesd_param.jesd_m;

		tx_dac_init.num_channels += phy[i]->jrx_link_tx[0].jesd_param.jesd_m *
					    (phy[i]->jrx_link_tx[0].jesd_param.jesd_duallink > 0 ? 2 : 1);
	}

	axi_jesd204_rx_watchdog(rx_jesd);

	axi_dac_init(&tx_dac, &tx_dac_init);
	axi_adc_init(&rx_adc, &rx_adc_init);

	axi_dmac_init(&tx_dmac, &tx_dmac_init);
	axi_dmac_init(&rx_dmac, &rx_dmac_init);

	struct jesd204_topology *topology;
	struct jesd204_topology_dev devs[] = {
		{
			.jdev = hmc7044_dev->jdev,
			.link_ids = {FRAMER_LINK0_RX, DEFRAMER_LINK0_TX},
			.links_number = 2,
			.is_sysref_provider = true,
		},
		{
			.jdev = rx_jesd->jdev,
			.link_ids = {FRAMER_LINK0_RX},
			.links_number = 1,
		},
		{
			.jdev = tx_jesd->jdev,
			.link_ids = {DEFRAMER_LINK0_TX},
			.links_number = 1,
		},
#if MULTIDEVICE_INSTANCE_COUNT == 4
		{
			.jdev = phy[0]->jdev,
			.link_ids = {FRAMER_LINK0_RX, DEFRAMER_LINK0_TX},
			.links_number = 2,
		},
		{
			.jdev = phy[1]->jdev,
			.link_ids = {FRAMER_LINK0_RX, DEFRAMER_LINK0_TX},
			.links_number = 2,
		},
		{
			.jdev = phy[2]->jdev,
			.link_ids = {FRAMER_LINK0_RX, DEFRAMER_LINK0_TX},
			.links_number = 2,
		},
		{
			.jdev = phy[3]->jdev,
			.link_ids = {FRAMER_LINK0_RX, DEFRAMER_LINK0_TX},
			.links_number = 2,
			.is_top_device = true,
		},
#else
		{
			.jdev = phy[0]->jdev,
			.link_ids = {FRAMER_LINK0_RX, DEFRAMER_LINK0_TX},
			.links_number = 2,
			.is_top_device = true,
		},
#endif
	};

	jesd204_topology_init(&topology, devs,
			      sizeof(devs)/sizeof(*devs));

	jesd204_fsm_start(topology, JESD204_LINKS_ALL);

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);

	no_os_mdelay(10);

#ifdef IIO_SUPPORT

	/* iio axi adc configurations. */
	struct iio_axi_adc_init_param iio_axi_adc_init_par;

	/* iio axi dac configurations. */
	struct iio_axi_dac_init_param iio_axi_dac_init_par;

	/* iio instance descriptor. */
	struct iio_axi_adc_desc *iio_axi_adc_desc;

	/* iio instance descriptor. */
	struct iio_axi_dac_desc *iio_axi_dac_desc;

	/* iio devices corresponding to every device. */
	struct iio_device *adc_dev_desc, *dac_dev_desc;

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

	status = axi_dmac_init(&tx_dmac, &tx_dmac_init);
	if(status < 0)
		return status;

	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = rx_adc,
		.rx_dmac = rx_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
#endif
	};

	status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if(status < 0)
		return status;
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);
	struct iio_data_buffer read_buff = {
		.buff = (void *)adc_buffer,
		.size = sizeof(adc_buffer),
	};

	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = tx_dac,
		.tx_dmac = tx_dmac,
#ifndef PLATFORM_MB
		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange,
#endif
	};

	status = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_init_par);
	if (status < 0)
		return status;
	iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &dac_dev_desc);

	struct iio_data_buffer write_buff = {
		.buff = (void *)dac_buffer,
		.size = sizeof(dac_buffer),
	};

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("axi_adc", iio_axi_adc_desc, adc_dev_desc, &read_buff, NULL, NULL),
		IIO_APP_DEVICE("axi_dac", iio_axi_dac_desc, dac_dev_desc, NULL, &write_buff, NULL)
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	iio_app_run(app);

	/* Disable the instruction cache. */
	Xil_DCacheDisable();
	/* Disable the data cache. */
	Xil_ICacheDisable();

#else // IIO_SUPPORT
	printf("Bye\n");

	/* Disable the instruction cache. */
	Xil_DCacheDisable();
	/* Disable the data cache. */
	Xil_ICacheDisable();

	return 0;
#endif

}
