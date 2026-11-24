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
#include "Nevis_uc5_profile.h"
// #include "Nevis_uc1_profile.h"
#include "Nevis_pfirBuffer.h"
#include "Nevis_gain_tables.h"
#include "ADRV9104_FW.h"

/* axi_adrv9001 RX core registers, see Linux adrv9104-backend.c */
#define ADRV910X_AXI_REG_CONFIG		0x0c
#define ADRV910X_AXI_IS_CMOS		NO_OS_BIT(7)
#define ADRV910X_AXI_RX_R1_MODE		NO_OS_BIT(2)
#define ADRV910X_AXI_SSI_N_LANES_MSK	NO_OS_GENMASK(12, 8)
#define ADRV910X_AXI_SSI_DDR		NO_OS_BIT(16)

/* Bytes of one capture: ADC_BUFFER_SAMPLES 16 bit I/Q pairs */
#define ADC_BUFFER_SIZE	(ADC_BUFFER_SAMPLES * ADRV9001_I_Q_CHANNELS * \
			 sizeof(uint16_t))

/* The AXI cores of one RX port. They are owned by the application. */
struct adrv910x_axi_rx {
	struct axi_adc_init adc_init;
	struct axi_dmac_init dmac_init;
	struct axi_adc *adc;
	struct axi_dmac *dmac;
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

#if defined(DMA_EXAMPLE) || defined(IIO_SUPPORT)
static uint16_t adc_buffers[ADRV910X_RX_MAX][ADC_BUFFER_SAMPLES *
		ADRV9001_I_Q_CHANNELS]
__attribute__((aligned(1024)));
#endif

int get_sampling_frequency(struct axi_adc *dev, uint32_t chan,
			   uint64_t *sampling_freq_hz)
{
	int c;

	if (!dev || !sampling_freq_hz)
		return -EINVAL;

	for (c = 0; c < ADRV910X_RX_MAX; c++) {
		if (dev != axi_rx[c].adc)
			continue;

		*sampling_freq_hz = axi_rx[c].rate;
		return 0;
	}

	return -ENODEV;
}

static struct adi_adrv910x_SpiSettings spiSettings = {
	.msbFirst = 1,
	.enSpiStreaming = 0,
	.autoIncAddrUp = 1,
	.fourWireMode = 1,
	.cmosPadDrvStrength = ADI_ADRV910X_CMOSPAD_DRV_STRONG,
};

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

//static struct adi_adrv9001_GainControlCfg agc_defaults = {
//	.peakWaitTime = 4,
//	.maxGainIndex = ADI_ADRV9001_RX_GAIN_INDEX_MAX,
//	.minGainIndex = ADI_ADRV9001_RX_GAIN_INDEX_MIN,
//	.gainUpdateCounter = 11520,
//	.attackDelay_us = 10,
//	.lowThreshPreventGainInc = false,
//	.slowLoopSettlingDelay = 16,
//	.changeGainIfThreshHigh = 3,
//	.agcMode = 1,
//	.resetOnRxon = false,
//	.resetOnRxonGainIndex = ADI_ADRV9001_RX_GAIN_INDEX_MAX,
//	.enableSyncPulseForGainCounter = false,
//	.enableFastRecoveryLoop = false,
//	.power = {
//		.powerEnableMeasurement = true,
//		.underRangeHighPowerThresh = 10,
//		.underRangeLowPowerThresh = 4,
//		.underRangeHighPowerGainStepRecovery = 2,
//		.underRangeLowPowerGainStepRecovery = 4,
//		.powerMeasurementDuration = 10,
//		.powerMeasurementDelay = 2,
//		.rxTddPowerMeasDuration = 0,
//		.rxTddPowerMeasDelay = 0,
//		.overRangeHighPowerThresh = 0,
//		.overRangeLowPowerThresh = 7,
//		.overRangeHighPowerGainStepAttack = 4,
//		.overRangeLowPowerGainStepAttack = 4,
//		.feedback_inner_high_inner_low = ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
//		.feedback_apd_high_apd_low = ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
//	},
//	.peak = {
//		.agcUnderRangeLowInterval = 50,
//		.agcUnderRangeMidInterval = 2,
//		.agcUnderRangeHighInterval = 4,
//		.apdHighThresh = 21,
//		.apdLowThresh = 12,
//		.apdUpperThreshPeakExceededCount = 6,
//		.apdLowerThreshPeakExceededCount = 3,
//		.apdGainStepAttack = 2,
//		.apdGainStepRecovery = 0,
//		.enableHbOverload = true,
//		.hbOverloadDurationCount = 1,
//		.hbOverloadThreshCount = 1,
//		.hbHighThresh = 13044,
//		.hbUnderRangeLowThresh = 5826,
//		.hbUnderRangeMidThresh = 8230,
//		.hbUnderRangeHighThresh = 7335,
//		.hbUpperThreshPeakExceededCount = 6,
//		.hbUnderRangeHighThreshExceededCount = 3,
//		.hbGainStepHighRecovery = 2,
//		.hbGainStepLowRecovery = 6,
//		.hbGainStepMidRecovery = 4,
//		.hbGainStepAttack = 2,
//		.hbOverloadPowerMode = 0,
//		.hbUnderRangeMidThreshExceededCount = 3,
//		.hbUnderRangeLowThreshExceededCount = 3,
//		.feedback_apd_low_hb_low = ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
//		.feedback_apd_high_hb_high = ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
//	},
//};

/*
 * Linux adrv9104_backend_disable(): allocate the core and hold it in reset, so
 * it does not interfere with the init calibrations.
 */
static int adrv910x_axi_rx_begin(struct adrv910x_axi_rx *rx)
{
	int ret;

	ret = axi_adc_init_begin(&rx->adc, &rx->adc_init);
	if (ret)
		return ret;

	return axi_adc_write(rx->adc, AXI_ADC_REG_RSTN, 0);
}

/*
 * Linux adrv9104_backend_setup() and iio_backend_enable(): configure the SSI
 * of the core as the profile says, then take the core out of reset. The port
 * must already be enabled, the core has no clock otherwise.
 */
static int adrv910x_axi_rx_setup(struct adrv910x_rf_phy *phy, uint32_t port)
{
	const struct adrv910x_chan *chan = phy->channels[port];
	const ssiConfig_t *ssi =
			&phy->device_profile->profilePS1.rxConfig[chan->idx].rxSsiConfig;
	struct adrv910x_axi_rx *rx = &axi_rx[port];
	uint32_t val, lanes, ch;
	bool cmos;
	int ret;

	ret = axi_adc_read(rx->adc, ADRV910X_AXI_REG_CONFIG, &val);
	if (ret)
		return ret;

	cmos = !!(val & ADRV910X_AXI_IS_CMOS);
	if (cmos != (phy->ssi_type == SSI_TYPE_CMOS)) {
		pr_err("%s: HDL is built for %s, the profile is not\n",
		       rx->adc_init.name, cmos ? "CMOS" : "LVDS");
		return -EINVAL;
	}

	switch (ssi->numLaneSel) {
	case SSI_1_LANE:
		lanes = 1;
		break;
	case SSI_2_LANE:
		if (cmos)
			return -EINVAL;
		lanes = 2;
		break;
	case SSI_4_LANE:
		if (!cmos)
			return -EINVAL;
		lanes = 4;
		break;
	default:
		return -EINVAL;
	}

	ret = axi_adc_read(rx->adc, AXI_ADC_REG_CNTRL, &val);
	if (ret)
		return ret;

	val &= ~(ADRV910X_AXI_SSI_N_LANES_MSK | ADRV910X_AXI_SSI_DDR);
	/* Split mode: each RX core carries its own I/Q pair */
	val |= ADRV910X_AXI_RX_R1_MODE;
	val |= no_os_field_prep(ADRV910X_AXI_SSI_N_LANES_MSK, lanes);
	if (cmos && ssi->ddrEn)
		val |= ADRV910X_AXI_SSI_DDR;

	ret = axi_adc_write(rx->adc, AXI_ADC_REG_CNTRL, val);
	if (ret)
		return ret;

	ret = axi_adc_write(rx->adc, AXI_ADC_REG_RSTN,
			    AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);
	if (ret)
		return ret;

	for (ch = 0; ch < rx->adc_init.num_channels; ch++) {
		ret = axi_adc_write(rx->adc, AXI_ADC_REG_CHAN_CNTRL(ch),
				    AXI_ADC_FORMAT_SIGNEXT |
				    AXI_ADC_FORMAT_ENABLE | AXI_ADC_ENABLE);
		if (ret)
			return ret;
	}

	no_os_mdelay(100);

	ret = axi_adc_init_finish(rx->adc);
	if (ret)
		return ret;

	rx->rate = chan->rate;

	return axi_dmac_init(&rx->dmac, &rx->dmac_init);
}

#ifdef DMA_EXAMPLE
/* Capture ADC_BUFFER_SAMPLES I/Q pairs of one RX port into its buffer */
static int adrv910x_capture(uint32_t port)
{
	struct adrv910x_axi_rx *rx = &axi_rx[port];
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

	ret = axi_adc_update_active_channels(rx->adc,
					     NO_OS_GENMASK(ADRV9001_I_Q_CHANNELS - 1, 0));
	if (ret)
		return ret;

	ret = axi_dmac_transfer_start(rx->dmac, &transfer);
	if (ret)
		return ret;

	ret = axi_dmac_transfer_wait_completion(rx->dmac, timeout_ms);
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
/* Expose every enabled RX port as an IIO device with a capture buffer */
static int32_t iio_run(const struct adrv910x_rf_phy *phy)
{
	struct iio_axi_adc_init_param adc_pars[ADRV910X_RX_MAX] = { 0 };
	struct iio_data_buffer iio_adc_buffers[ADRV910X_RX_MAX];
	struct iio_app_device app_devices[ADRV910X_RX_MAX] = { 0 };
	struct iio_axi_adc_desc *adcs[ADRV910X_RX_MAX];
	struct iio_device *iio_descs[ADRV910X_RX_MAX];
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
	uint32_t n = 0;
	int32_t c, ret;

	for (c = 0; c < ADRV910X_RX_MAX; c++) {
		if (!phy->channels[c]->enabled)
			continue;

		adc_pars[n].rx_adc = axi_rx[c].adc;
		adc_pars[n].rx_dmac = axi_rx[c].dmac;
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
	adrv910x_init_par.pfbuf = pfirBuffer_9;
	adrv910x_init_par.rx_gain_table = gainTableRows_12;
	adrv910x_init_par.tx_atten_table = attenTableRows_14;
	/* Use case settings: RX2 (narrowband) and TXNB at 450 MHz */
	adrv910x_init_par.rx_carrier_hz[ADRV910X_RX2] = 450000000;
	adrv910x_init_par.tx_carrier_hz = 450000000;
	adrv910x_init_par.rx_agc[ADRV910X_RX2] = &agc_uc5;
	deviceProfilePS2_t profilePS2 = {
		.ver = {
			.major = 12,
			.minor = 0
		},
		.ps2ArmClk = 0,
		.checksum = 0
	};
	adi_adrv910x_Profiles_t dev_profile = {
		.profilePS1 = profile_5,
		.profilePS2 = profilePS2
	};
	adrv910x_init_par.device_profile = &dev_profile;
	adrv910x_init_par.arm_binary_ptr = Adrv910xFirmware_C0_0_1_365_bin;
	adrv910x_init_par.arm_binary_size = Adrv910xFirmware_C0_0_1_365_bin_len;

	/* The RX cores stay in reset while the transceiver is initialized */
	for (c = 0; c < ADRV910X_RX_MAX; c++) {
		ret = adrv910x_axi_rx_begin(&axi_rx[c]);
		if (ret) {
			pr_err("%s: core reset failed: %d\n",
			       axi_rx[c].adc_init.name, ret);
			return ret;
		}
	}

	/* Initialize the device from the init parameters */
	ret = adrv910x_init(&phy, &adrv910x_init_par);
	if (ret) {
		pr_err("Device initialization failed: %d\n", ret);
		goto error;
	}

	printf("ADRV9104 initialization completed successfully!\n");

	/* Prepare every RX port the profile enables for a data capture */
	for (c = 0; c < ADRV910X_RX_MAX; c++) {
		if (!phy.channels[c]->enabled)
			continue;

		/*
		 * The enable pins are not driven by this application, so the
		 * port is controlled over SPI and left receiving.
		 */
		ret = adrv910x_port_en_mode_set(&phy, c, ADI_ADRV910X_SPI_MODE);
		if (ret)
			goto error;

		ret = adrv910x_ensm_mode_set(&phy, c,
					     ADI_ADRV910X_CHANNEL_RF_ENABLED);
		if (ret)
			goto error;

		ret = adrv910x_axi_rx_setup(&phy, c);
		if (ret) {
			pr_err("%s: setup failed: %d\n",
			       axi_rx[c].adc_init.name, ret);
			goto error;
		}

		printf("%s: ready for capture at %lu Hz\n",
		       axi_rx[c].adc_init.name, (unsigned long)axi_rx[c].rate);
	}

#ifdef DMA_EXAMPLE
	for (c = 0; c < ADRV910X_RX_MAX; c++) {
		if (!phy.channels[c]->enabled)
			continue;

		ret = adrv910x_capture(c);
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
	for (c = 0; c < ADRV910X_RX_MAX; c++) {
		if (axi_rx[c].dmac)
			axi_dmac_remove(axi_rx[c].dmac);
		if (axi_rx[c].adc)
			axi_adc_remove(axi_rx[c].adc);
	}

	return ret;
}
