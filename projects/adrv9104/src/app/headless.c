/***************************************************************************//**
 *   @file   headless.c
 *   @brief  adrv910x main project file.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef XILINX_PLATFORM
#include "xil_cache.h"
#endif /* XILINX_PLATFORM */

#include "no_os_print_log.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_clk.h"
#include "no_os_delay.h"

#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"

#include "parameters.h"

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_axi_adc.h"
#include "iio_axi_dac.h"
#include "xilinx_uart.h"
#endif

#include "adi_adrv910x_types.h"
#include "adi_adrv910x_arm_types.h"
#include "adi_adrv910x.h"
#include "adrv910x.h"
#include "nevis_template_profile.h"
#include "Nevis_profile.h"
#include "ADRV9104_FW.h"
#include "ADRV9104_Stream.h"

/* Bytes of one capture: ADC_BUFFER_SAMPLES 16 bit I/Q pairs */
#define ADC_BUFFER_SIZE	(ADC_BUFFER_SAMPLES * ADRV9001_I_Q_CHANNELS * \
			 sizeof(uint16_t))

/*
 * Init parameters of the AXI cores of one RX port. The cores themselves are
 * part of struct adrv910x_rf_phy.
 */
struct adrv910x_axi_rx {
	struct axi_adc_init adc_init;
	struct axi_dmac_init dmac_init;
	uint32_t rate;
};

static struct adrv910x_axi_rx axi_rx[ADRV910X_RX_MAX] = {
	[ADRV910X_RX1] = {
		.adc_init = {
			.name = "axi-adrv910x-rx1-lpc",
			.base = RX1_ADC_BASEADDR,
			.num_channels = ADRV9001_I_Q_CHANNELS,
		},
		.dmac_init = { "rx1_dmac", RX1_DMA_BASEADDR, IRQ_DISABLED },
	},
	[ADRV910X_RX2] = {
		.adc_init = {
			.name = "axi-adrv910x-rx2-lpc",
			.base = RX2_ADC_BASEADDR,
			.num_channels = ADRV9001_I_Q_CHANNELS,
		},
		.dmac_init = { "rx2_dmac", RX2_DMA_BASEADDR, IRQ_DISABLED },
	},
};

/* Init parameters of the AXI cores of the TX port */
struct adrv910x_axi_tx {
	struct axi_dac_init dac_init;
	struct axi_dmac_init dmac_init;
	uint32_t rate;
};

/*
 * By default the TX core sends a single DDS tone, I and Q in quadrature. It is
 * switched to the DMA when a buffer is pushed over IIO or by the DMA example.
 */
#define ADRV910X_DDS_TONE_HZ	100000
#define ADRV910X_DDS_SCALE	250000	/* 0.25 per tone, two tones summed */

static struct axi_dac_channel tx_dac_channels[ADRV9001_I_Q_CHANNELS] = {
	/* I */
	[0] = {
		.sel = AXI_DAC_DATA_SEL_DDS,
		.dds_frequency_0 = ADRV910X_DDS_TONE_HZ,
		.dds_phase_0 = 90000,
		.dds_scale_0 = ADRV910X_DDS_SCALE,
	},
	/* Q */
	[1] = {
		.sel = AXI_DAC_DATA_SEL_DDS,
		.dds_frequency_0 = ADRV910X_DDS_TONE_HZ,
		.dds_phase_0 = 0,
		.dds_scale_0 = ADRV910X_DDS_SCALE,
	},
};

static struct adrv910x_axi_tx axi_tx = {
	.dac_init = {
		.name = "axi-adrv910x-tx-lpc",
		.base = TX1_DAC_BASEADDR,
		.num_channels = ADRV9001_I_Q_CHANNELS,
		.channels = tx_dac_channels,
	},
	.dmac_init = { "tx_dmac", TX1_DMA_BASEADDR, IRQ_DISABLED },
};

#if defined(DMA_EXAMPLE) || defined(IIO_SUPPORT)
static uint16_t adc_buffers[ADRV910X_RX_MAX][ADC_BUFFER_SAMPLES *
		ADRV9001_I_Q_CHANNELS]
__attribute__((aligned(1024)));
#endif

#if defined(DMA_EXAMPLE) || defined(IIO_SUPPORT)
static uint32_t dac_buffer[DAC_BUFFER_SAMPLES * ADRV9001_I_Q_CHANNELS]
__attribute__((aligned(1024)));
#endif

#ifdef IIO_SUPPORT

static int get_dac_sampling_frequency(struct axi_dac *dev, uint32_t chan,
				      uint64_t *sampling_freq_hz)
{
	if (!dev || dev->base != axi_tx.dac_init.base || !sampling_freq_hz)
		return -EINVAL;

	*sampling_freq_hz = axi_tx.rate;

	return 0;
}
#endif

int get_sampling_frequency(struct axi_adc *dev, uint32_t chan,
			   uint64_t *sampling_freq_hz)
{
	int c;

	if (!dev || !sampling_freq_hz)
		return -EINVAL;

	for (c = 0; c < ADRV910X_RX_MAX; c++) {
		if (dev->base != axi_rx[c].adc_init.base)
			continue;

		*sampling_freq_hz = axi_rx[c].rate;
		return 0;
	}

	return -ENODEV;
}

/* spiSettings comes from Nevis_profile.h */
struct adi_adrv910x_SpiSettings *adrv910x_spi_settings_get(void)
{
	return &spiSettings;
}

static int32_t adrv910x_dev_clk_recalc_rate(struct no_os_clk_desc *desc,
		uint64_t *rate)
{
	*rate = 38400000;

	return 0;
}

static int32_t adrv910x_dev_clk_set_rate(struct no_os_clk_desc *desc,
		uint64_t rate)
{
	// Do nothing
	return 0;
}

static int32_t adrv910x_dev_clk_round_rate(struct no_os_clk_desc *desc,
		uint64_t rate,
		uint64_t *rounded_rate)
{
	pr_debug("%s: Rate %lu Hz", __func__, rate);

	*rounded_rate = rate;

	return 0;
}

static const struct no_os_clk_platform_ops adrv910x_dev_clk_ops = {
	.clk_round_rate = &adrv910x_dev_clk_round_rate,
	.clk_set_rate = &adrv910x_dev_clk_set_rate,
	.clk_recalc_rate = &adrv910x_dev_clk_recalc_rate,
};

/* AGC settings of the use case, previously hardcoded in adrv910x_init() */
static const adi_adrv910x_GainControlCfg_t agc_uc5 = {
	.peakWaitTime = 4,
	.maxGainIndex = 255,
	.minGainIndex = 187,
	.gainUpdateCounter = 600,
	.attackDelay_us = 10,
	.slowLoopSettlingDelay = 16,
	.lowThreshPreventGainInc = false,
	.changeGainIfThreshHigh = 3,
	.agcMode = ADI_ADRV910X_RX_GAIN_CONTROL_DETECTION_MODE_PEAK,
	.resetOnRxon = false,
	.resetOnRxonGainIndex = 255,
	.enableSyncPulseForGainCounter = false,
	.enableFastRecoveryLoop = false,
	.power = {
		.powerEnableMeasurement = true,
		.underRangeHighPowerThresh = 10,
		.underRangeLowPowerThresh = 4,
		.underRangeHighPowerGainStepRecovery = 2,
		.underRangeLowPowerGainStepRecovery = 4,
		.powerMeasurementDuration = 10,
		.powerMeasurementDelay = 2,
		.rxTddPowerMeasDuration = 0,
		.rxTddPowerMeasDelay = 0,
		.overRangeHighPowerThresh = 0,
		.overRangeLowPowerThresh = 7,
		.overRangeHighPowerGainStepAttack = 4,
		.overRangeLowPowerGainStepAttack = 4,
		.feedback_inner_high_inner_low = ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED,
		.feedback_apd_high_apd_low = ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED
	},
	.peak = {
		.agcUnderRangeLowInterval = 50,
		.agcUnderRangeMidInterval = 2,
		.agcUnderRangeHighInterval = 4,
		.apdHighThresh = 19,
		.apdLowThresh = 12,
		.apdUpperThreshPeakExceededCount = 6,
		.apdLowerThreshPeakExceededCount = 3,
		.apdGainStepAttack = 2,
		.apdGainStepRecovery = 2,
		.enableHbOverload = true,
		.hbOverloadDurationCount = 1,
		.hbOverloadThreshCount = 1,
		.hbHighThresh = 8208,
		.hbUnderRangeLowThresh = 2911,
		.hbUnderRangeMidThresh = 4112,
		.hbUnderRangeHighThresh = 5810,
		.hbUpperThreshPeakExceededCount = 6,
		.hbUnderRangeHighThreshExceededCount = 3,
		.hbGainStepHighRecovery = 2,
		.hbGainStepLowRecovery = 6,
		.hbGainStepMidRecovery = 4,
		.hbGainStepAttack = 2,
		.hbOverloadPowerMode = 0,
		.hbUnderRangeMidThreshExceededCount = 3,
		.hbUnderRangeLowThreshExceededCount = 3,
		.feedback_apd_low_hb_low = ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED,
		.feedback_apd_high_hb_high = ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED
	},
	.extLna = {
		.settlingDelay = 0
	},
	.rxQecFreezeEnable = false,
	.gpioFreezePin = ADI_ADRV910X_GPIO_UNASSIGNED
};
/* The AXI cores of an RX port, kept in the phy as in the adrv9002 driver */
static struct axi_adc **adrv910x_rx_adc(struct adrv910x_rf_phy *phy,
					uint32_t port)
{
	return port == ADRV910X_RX1 ? &phy->rx1_adc : &phy->rx2_adc;
}

static struct axi_dmac **adrv910x_rx_dmac(struct adrv910x_rf_phy *phy,
		uint32_t port)
{
	return port == ADRV910X_RX1 ? &phy->rx1_dmac : &phy->rx2_dmac;
}

#ifdef DMA_EXAMPLE
/*
 * Transmit the sine of sine_lut_iq on the TX port, cyclically, as the adrv9001
 * project does.
 */
static int adrv910x_transmit_sine(struct adrv910x_rf_phy *phy)
{
	struct axi_dma_transfer transfer = {
		.size = sizeof(sine_lut_iq),
		.transfer_done = 0,
		.cyclic = CYCLIC,
		.src_addr = (uintptr_t)dac_buffer,
		.dest_addr = 0,
	};
	int ret;

	/* Copies the samples to the buffer and selects the DMA as source */
	ret = axi_dac_load_custom_data(phy->tx1_dac, sine_lut_iq,
				       NO_OS_ARRAY_SIZE(sine_lut_iq),
				       (uintptr_t)dac_buffer);
	if (ret)
		return ret;

#ifdef XILINX_PLATFORM
	Xil_DCacheFlushRange((uintptr_t)dac_buffer, sizeof(sine_lut_iq));
#endif /* XILINX_PLATFORM */

	ret = axi_dmac_transfer_start(phy->tx1_dmac, &transfer);
	if (ret)
		return ret;

	printf("DMA_EXAMPLE %s: transmitting %lu samples cyclically\n",
	       axi_tx.dac_init.name,
	       (unsigned long)NO_OS_ARRAY_SIZE(sine_lut_iq));

	/* Let the signal settle before any capture */
	no_os_mdelay(1000);

	return 0;
}

/* Capture ADC_BUFFER_SAMPLES I/Q pairs of one RX port into its buffer */
static int adrv910x_capture(struct adrv910x_rf_phy *phy, uint32_t port)
{
	struct adrv910x_axi_rx *rx = &axi_rx[port];
	struct axi_adc *adc = *adrv910x_rx_adc(phy, port);
	struct axi_dmac *dmac = *adrv910x_rx_dmac(phy, port);
	struct axi_dma_transfer transfer = {
		.size = ADC_BUFFER_SIZE,
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = 0,
		.dest_addr = (uintptr_t)adc_buffers[port],
	};
	uint32_t timeout_ms;
	int ret;

	/* The narrowband rates are low: wait for the capture time plus margin */
	timeout_ms = (uint64_t)ADC_BUFFER_SAMPLES * 1000 / rx->rate + 500;

	ret = axi_adc_update_active_channels(adc,
					     NO_OS_GENMASK(ADRV9001_I_Q_CHANNELS - 1, 0));
	if (ret)
		return ret;

	ret = axi_dmac_transfer_start(dmac, &transfer);
	if (ret)
		return ret;

	ret = axi_dmac_transfer_wait_completion(dmac, timeout_ms);
	if (ret)
		return ret;

#ifdef XILINX_PLATFORM
	Xil_DCacheInvalidateRange((uintptr_t)adc_buffers[port], ADC_BUFFER_SIZE);
#endif /* XILINX_PLATFORM */

	printf("DMA_EXAMPLE %s: address=%#lx samples=%lu channels=%u bits=%lu\n",
	       rx->adc_init.name, (uintptr_t)adc_buffers[port],
	       (unsigned long)ADC_BUFFER_SAMPLES * rx->adc_init.num_channels,
	       rx->adc_init.num_channels, 8 * sizeof(adc_buffers[port][0]));

	return 0;
}
#endif

#ifdef IIO_SUPPORT
/*
 * Expose every enabled RX port as an IIO device with a capture buffer and the
 * TX port, if enabled, as an IIO device with a transmit buffer.
 */
static int32_t iio_run(struct adrv910x_rf_phy *phy)
{
	struct iio_axi_adc_init_param adc_pars[ADRV910X_RX_MAX] = { 0 };
	struct iio_data_buffer iio_adc_buffers[ADRV910X_RX_MAX];
	struct iio_app_device app_devices[ADRV910X_CHAN_MAX] = { 0 };
	struct iio_axi_adc_desc *adcs[ADRV910X_RX_MAX];
	struct iio_device *iio_descs[ADRV910X_CHAN_MAX];
	struct iio_axi_dac_init_param dac_par = { 0 };
	struct iio_data_buffer iio_dac_buffer;
	struct iio_axi_dac_desc *dac;
	struct xil_uart_init_param platform_uart_init_par = {
		.type = UART_PS,
#ifdef SDT
		.base_addr = XPAR_XUARTPS_0_BASEADDR,
#endif
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
	uint32_t n = 0;
	int32_t c, ret;

	for (c = 0; c < ADRV910X_RX_MAX; c++) {
		if (!phy->channels[c]->enabled)
			continue;

		adc_pars[n].rx_adc = *adrv910x_rx_adc(phy, c);
		adc_pars[n].rx_dmac = *adrv910x_rx_dmac(phy, c);
#ifdef XILINX_PLATFORM
		adc_pars[n].dcache_invalidate_range =
			(void (*)(uint32_t, uint32_t))Xil_DCacheInvalidateRange;
#endif /* XILINX_PLATFORM */
		adc_pars[n].get_sampling_frequency = get_sampling_frequency;

		ret = iio_axi_adc_init(&adcs[n], &adc_pars[n]);
		if (ret < 0)
			return ret;

		iio_adc_buffers[n].buff = adc_buffers[c];
		iio_adc_buffers[n].size = sizeof(adc_buffers[c]);
		iio_axi_adc_get_dev_descriptor(adcs[n], &iio_descs[n]);
		app_devices[n].name = (char *)axi_rx[c].adc_init.name;
		app_devices[n].dev = adcs[n];
		app_devices[n].dev_descriptor = iio_descs[n];
		app_devices[n].read_buff = &iio_adc_buffers[n];
		n++;
	}

	if (phy->channels[ADRV910X_TX]->enabled) {
		dac_par.tx_dac = phy->tx1_dac;
		dac_par.tx_dmac = phy->tx1_dmac;
#ifdef XILINX_PLATFORM
		dac_par.dcache_flush_range =
			(void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange;
#endif /* XILINX_PLATFORM */
		dac_par.get_sampling_frequency = get_dac_sampling_frequency;

		ret = iio_axi_dac_init(&dac, &dac_par);
		if (ret < 0)
			return ret;

		iio_dac_buffer.buff = dac_buffer;
		iio_dac_buffer.size = sizeof(dac_buffer);
		iio_axi_dac_get_dev_descriptor(dac, &iio_descs[n]);
		app_devices[n].name = (char *)axi_tx.dac_init.name;
		app_devices[n].dev = dac;
		app_devices[n].dev_descriptor = iio_descs[n];
		app_devices[n].write_buff = &iio_dac_buffer;
		n++;
	}

	app_init_param.devices = app_devices;
	app_init_param.nb_devices = n;
	app_init_param.uart_init_params = iio_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
#endif

int main(void)
{
	struct adrv910x_init_param adrv910x_init_par = { 0 };
	adi_adrv910x_Device_t adrv910x_device = { 0 };
	struct no_os_clk_init_param clk_init = { 0 };
	struct adrv910x_rf_phy phy = { 0 };
	uint32_t c;
	int ret;

#ifdef XILINX_PLATFORM
	Xil_ICacheEnable();
	Xil_DCacheEnable();
#endif /* XILINX_PLATFORM */

	printf("Hello\nStarting ADRV9104 initialization...\n");
	printf("Built on %s %s\n", __DATE__, __TIME__);
	phy.adrv910x = &adrv910x_device;
	phy.adrv910x->common.devHalInfo = &phy.hal;

	/* Initialize dev_clk component */
	clk_init.name = "adrv910x_ext_refclk";
	clk_init.hw_ch_num = 1;
	clk_init.platform_ops = &adrv910x_dev_clk_ops;
	clk_init.dev_desc = &phy;

	ret = no_os_clk_init(&adrv910x_init_par.dev_clk, &clk_init);
	if (ret) {
		pr_err("Failed to create descriptor for dev_clk.\n");
		return ret;
	}

	/* Configure initialization parameters */
	adrv910x_init_par.dev_clk_frequency_hz = 38400000; /* 38.4 MHz */
	adrv910x_init_par.clk_out_divisor = ADI_ADRV910X_DEVICECLOCKDIVISOR_BYPASS;
	adrv910x_init_par.reset_gpio_init = NULL; /* GPIO initialized by platform */
	adrv910x_init_par.spi = NULL; /* SPI will be initialized by HAL */
	adrv910x_init_par.pfbuf = pfirBuffer;
	adrv910x_init_par.rx_gain_table = gainTableRows_RX_1_CORRECTION_TABLE;
	adrv910x_init_par.tx_atten_table = attenTableRows;
	/*
	 * Use case settings: the profile enables RX1 and TX1. They share LO1,
	 * so both get the carrier of the profile.
	 */
	adrv910x_init_par.rx_carrier_hz[ADRV910X_RX1] =
		carrier_RX_1.carrierFrequency_Hz;
	adrv910x_init_par.tx_carrier_hz = carrier_RX_1.carrierFrequency_Hz;
	adrv910x_init_par.rx_agc[ADRV910X_RX2] = &agc_uc5;
	adrv910x_init_par.device_profile = &profile_PS1;
	adrv910x_init_par.arm_binary_ptr = ADRV9104_FW_PS1_bin;
	adrv910x_init_par.arm_binary_size = sizeof(ADRV9104_FW_PS1_bin);
	adrv910x_init_par.stream_binary_ptr = ADRV9104_Stream_bin;
	adrv910x_init_par.stream_binary_size = sizeof(ADRV9104_Stream_bin);

	/*
	 * Allocate the ADC/DAC cores and assign them to the phy. The driver
	 * configures them in adrv910x_init().
	 */
	for (c = 0; c < ADRV910X_RX_MAX; c++) {
		ret = axi_adc_init_begin(adrv910x_rx_adc(&phy, c),
					 &axi_rx[c].adc_init);
		if (ret) {
			printf("axi_adc_init_begin() failed with status %d\n",
			       ret);
			goto error;
		}
	}

	ret = axi_dac_init_begin(&phy.tx1_dac, &axi_tx.dac_init);
	if (ret) {
		printf("axi_dac_init_begin() failed with status %d\n", ret);
		goto error;
	}

	/* Initialize the device from the init parameters */
	ret = adrv910x_init(&phy, &adrv910x_init_par);
	if (ret) {
		pr_err("Device initialization failed: %d\n", ret);
		goto error;
	}

	printf("ADRV9104 initialization completed successfully!\n");

	/*
	 * The enable pins are not driven by this application, so every port
	 * the profile enables, RX and TX, is controlled over SPI and moved to
	 * RF enabled. The AXI cores have no clock otherwise.
	 */
	for (c = 0; c < ADRV910X_CHAN_MAX; c++) {
		if (!phy.channels[c]->enabled)
			continue;

		ret = adrv910x_port_en_mode_set(&phy, c, ADI_ADRV910X_SPI_MODE);
		if (ret) {
			printf("port %lu: SPI enable mode failed: %d\n",
			       (unsigned long)c, ret);
			goto error;
		}

		ret = adrv910x_ensm_mode_set(&phy, c,
					     ADI_ADRV910X_CHANNEL_RF_ENABLED);
		if (ret) {
			printf("port %lu: move to rf_enabled failed: %d\n",
			       (unsigned long)c, ret);
			goto error;
		}
	}

	/*
	 * Report the state and the carrier every port ended up with: in TDD
	 * the device might not keep RX and TX RF enabled at the same time, and
	 * ports sharing an LO must have the same carrier.
	 */
	for (c = 0; c < ADRV910X_CHAN_MAX; c++) {
		adi_adrv910x_ChannelState_e state;
		uint64_t carrier_hz;

		if (!phy.channels[c]->enabled)
			continue;

		ret = adrv910x_ensm_mode_get(&phy, c, &state);
		if (ret) {
			printf("port %lu: state read failed: %d\n",
			       (unsigned long)c, ret);
			goto error;
		}

		ret = adrv910x_carrier_freq_get(&phy, c, &carrier_hz);
		if (ret) {
			printf("port %lu: carrier read failed: %d\n",
			       (unsigned long)c, ret);
			goto error;
		}

		printf("%s: state %d (%s), carrier %lu Hz, LO %d\n",
		       c == ADRV910X_TX ? "TX" : (c ? "RX2" : "RX1"), state,
		       state == ADI_ADRV910X_CHANNEL_RF_ENABLED ?
		       "rf_enabled" : "not rf_enabled",
		       (unsigned long)carrier_hz, phy.channels[c]->lo);
	}

	/* Finalize the ADC/DAC cores initialization and init their DMAs */
	for (c = 0; c < ADRV910X_RX_MAX; c++) {
		if (!phy.channels[c]->enabled)
			continue;

		ret = axi_adc_init_finish(*adrv910x_rx_adc(&phy, c));
		if (ret) {
			printf("axi_adc_init_finish() failed with status %d\n",
			       ret);
			goto error;
		}

		ret = axi_dmac_init(adrv910x_rx_dmac(&phy, c),
				    &axi_rx[c].dmac_init);
		if (ret) {
			printf("axi_dmac_init() failed with status %d\n", ret);
			goto error;
		}

		axi_rx[c].rate = phy.channels[c]->rate;
		printf("%s: ready for capture at %lu Hz\n",
		       axi_rx[c].adc_init.name, (unsigned long)axi_rx[c].rate);
	}

	printf("TX: %s by the profile\n",
	       phy.channels[ADRV910X_TX]->enabled ? "enabled" : "not enabled");

	if (phy.channels[ADRV910X_TX]->enabled) {
		ret = axi_dac_init_finish(phy.tx1_dac);
		if (ret) {
			printf("axi_dac_init_finish() failed with status %d\n",
			       ret);
			goto error;
		}

		/*
		 * The DDS runs at the sample rate of the port, not at the
		 * interface clock axi_dac_init_finish() measured, so that is
		 * what its frequencies must be computed against.
		 */
		phy.tx1_dac->clock_hz = phy.channels[ADRV910X_TX]->rate;

		/* Apply the data source of tx_dac_channels: the DDS tone */
		axi_dac_data_setup(phy.tx1_dac);

		ret = axi_dmac_init(&phy.tx1_dmac, &axi_tx.dmac_init);
		if (ret) {
			printf("axi_dmac_init() failed with status %d\n", ret);
			goto error;
		}

		axi_tx.rate = phy.channels[ADRV910X_TX]->rate;
		printf("%s: ready for transmit at %lu Hz\n",
		       axi_tx.dac_init.name, (unsigned long)axi_tx.rate);
	}

#ifdef DMA_EXAMPLE
	if (phy.channels[ADRV910X_TX]->enabled) {
		ret = adrv910x_transmit_sine(&phy);
		if (ret) {
			pr_err("%s: transmit failed: %d\n",
			       axi_tx.dac_init.name, ret);
			goto error;
		}
	}

	for (c = 0; c < ADRV910X_RX_MAX; c++) {
		if (!phy.channels[c]->enabled)
			continue;

		ret = adrv910x_capture(&phy, c);
		if (ret) {
			pr_err("%s: capture failed: %d\n",
			       axi_rx[c].adc_init.name, ret);
			goto error;
		}
	}
#endif

#ifdef IIO_SUPPORT
	ret = iio_run(&phy);
	if (ret < 0) {
		printf("iio_run() failed with status %d\n", ret);
		goto error;
	}
#endif
	printf("Bye\n");

	return 0;

error:
	printf("Stopped with status %d\n", ret);

	for (c = 0; c < ADRV910X_RX_MAX; c++) {
		if (*adrv910x_rx_dmac(&phy, c))
			axi_dmac_remove(*adrv910x_rx_dmac(&phy, c));
		if (*adrv910x_rx_adc(&phy, c))
			axi_adc_remove(*adrv910x_rx_adc(&phy, c));
	}
	if (phy.tx1_dmac)
		axi_dmac_remove(phy.tx1_dmac);
	if (phy.tx1_dac)
		axi_dac_remove(phy.tx1_dac);

	return ret;
}
