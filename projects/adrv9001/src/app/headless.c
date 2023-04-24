/***************************************************************************//**
 *   @file   headless.c
 *   @brief  adrv9002 main project file.
 *   @author Darius Berghe (darius.berghe@analog.com)
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef XILINX_PLATFORM
#include "xil_cache.h"
#endif /* XILINX_PLATFORM */

#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_spi.h"

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

#include "adrv9002.h"
#include "adi_adrv9001.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_radio.h"
#include "adi_adrv9001_profileutil.h"
#include "Navassa_CMOS_profile.h"

/* ADC/DAC Buffers */
#if defined(DMA_EXAMPLE) || defined(IIO_SUPPORT)
static uint32_t dac_buffers[IIO_DEV_COUNT][DAC_BUFFER_SAMPLES]
__attribute__((aligned));
static uint16_t adc_buffers[IIO_DEV_COUNT][ADC_BUFFER_SAMPLES]
__attribute__((aligned));
#endif

uint64_t sampling_freq;

int get_sampling_frequency(struct axi_adc *dev, uint32_t chan,
			   uint64_t *sampling_freq_hz)
{
	if (!dev || !sampling_freq_hz)
		return -EINVAL;

	*sampling_freq_hz = sampling_freq;
	return 0;
}

static struct adi_adrv9001_SpiSettings spiSettings = {
	.msbFirst = 1,
	.enSpiStreaming = 0,
	.autoIncAddrUp = 1,
	.fourWireMode = 1,
	.cmosPadDrvStrength = ADI_ADRV9001_CMOSPAD_DRV_STRONG,
};

struct adi_adrv9001_SpiSettings *adrv9002_spi_settings_get(void)
{
	return &spiSettings;
}

enum adi_adrv9001_SsiType adrv9002_ssi_type_detect(struct adrv9002_rf_phy *phy)
{
	enum adi_adrv9001_SsiType ssi, ssi2;
	char *ssi_str[3] = {
		"[SSI Disabled]",
		"CMOS",
		"LVDS"
	};

	ssi = adrv9002_axi_ssi_type_get(phy);

	ssi2 = phy->curr_profile->rx.rxChannelCfg[0].profile.rxSsiConfig.ssiType;
	if (ssi != ssi2) {
		printf("SSI mismatch: detected %s in HDL and %s in profile.\n", ssi_str[ssi],
		       ssi_str[ssi2]);
		return ADI_ADRV9001_SSI_TYPE_DISABLE;
	}

	return ssi;
}

static struct adi_adrv9001_GainControlCfg agc_defaults = {
	.peakWaitTime = 4,
	.maxGainIndex = ADI_ADRV9001_RX_GAIN_INDEX_MAX,
	.minGainIndex = ADI_ADRV9001_RX_GAIN_INDEX_MIN,
	.gainUpdateCounter = 11520,
	.attackDelay_us = 10,
	.lowThreshPreventGainInc = false,
	.slowLoopSettlingDelay = 16,
	.changeGainIfThreshHigh = 3,
	.agcMode = 1,
	.resetOnRxon = false,
	.resetOnRxonGainIndex = ADI_ADRV9001_RX_GAIN_INDEX_MAX,
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
		.feedback_inner_high_inner_low = ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
		.feedback_apd_high_apd_low = ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
	},
	.peak = {
		.agcUnderRangeLowInterval = 50,
		.agcUnderRangeMidInterval = 2,
		.agcUnderRangeHighInterval = 4,
		.apdHighThresh = 21,
		.apdLowThresh = 12,
		.apdUpperThreshPeakExceededCount = 6,
		.apdLowerThreshPeakExceededCount = 3,
		.apdGainStepAttack = 2,
		.apdGainStepRecovery = 0,
		.enableHbOverload = true,
		.hbOverloadDurationCount = 1,
		.hbOverloadThreshCount = 1,
		.hbHighThresh = 13044,
		.hbUnderRangeLowThresh = 5826,
		.hbUnderRangeMidThresh = 8230,
		.hbUnderRangeHighThresh = 7335,
		.hbUpperThreshPeakExceededCount = 6,
		.hbUnderRangeHighThreshExceededCount = 3,
		.hbGainStepHighRecovery = 2,
		.hbGainStepLowRecovery = 6,
		.hbGainStepMidRecovery = 4,
		.hbGainStepAttack = 2,
		.hbOverloadPowerMode = 0,
		.hbUnderRangeMidThreshExceededCount = 3,
		.hbUnderRangeLowThreshExceededCount = 3,
		.feedback_apd_low_hb_low = ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
		.feedback_apd_high_hb_high = ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
	},
};

#ifdef IIO_SUPPORT

static int32_t iio_run(struct iio_axi_adc_init_param *adc_pars,
		       struct iio_axi_dac_init_param *dac_pars)
{
	struct iio_axi_adc_desc *adcs[IIO_DEV_COUNT];
	struct iio_axi_dac_desc *dacs[IIO_DEV_COUNT];
	struct iio_data_buffer iio_dac_buffers[IIO_DEV_COUNT];
	struct iio_data_buffer iio_adc_buffers[IIO_DEV_COUNT];
	struct iio_device *iio_descs[IIO_DEV_COUNT * 2];
	struct iio_app_device app_devices[IIO_DEV_COUNT * 2] = {0};
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
	int32_t i, ret;
	int32_t a; // linear iterator for iio_descs and app_devices flat arrays

	for (i = 0; i < IIO_DEV_COUNT; i++) {
		/* ADC setup */
		iio_adc_buffers[i].buff = adc_buffers[i];
		iio_adc_buffers[i].size = sizeof(adc_buffers[i]);
		ret = iio_axi_adc_init(&adcs[i], &adc_pars[i]);
		if (ret < 0)
			return ret;
		a = 2 * i;
		iio_axi_adc_get_dev_descriptor(adcs[i], &iio_descs[a]);
		app_devices[a].name = adc_pars[i].rx_adc->name;
		app_devices[a].dev = adcs[i];
		app_devices[a].dev_descriptor = iio_descs[a];
		app_devices[a].read_buff = &iio_adc_buffers[i];

		/* DAC setup */
		iio_dac_buffers[i].buff = dac_buffers[i];
		iio_dac_buffers[i].size = sizeof(dac_buffers[i]);
		ret = iio_axi_dac_init(&dacs[i], &dac_pars[i]);
		if (ret < 0)
			return ret;
		a = 2 * i + 1;
		iio_axi_dac_get_dev_descriptor(dacs[i], &iio_descs[a]);
		app_devices[a].name = dac_pars[i].tx_dac->name;
		app_devices[a].dev = dacs[i];
		app_devices[a].dev_descriptor = iio_descs[a];
		app_devices[a].write_buff = &iio_dac_buffers[i];
	}

	app_init_param.devices = app_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(app_devices);
	app_init_param.uart_init_params = iio_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
#endif

int main(void)
{
	int ret;
	struct adi_common_ApiVersion api_version;
	struct adi_adrv9001_ArmVersion arm_version;
	struct adi_adrv9001_SiliconVersion silicon_version;
	struct adi_adrv9001_Device adrv9001_device = {0};
	struct adrv9002_chip_info chip = {0};
	struct adrv9002_rf_phy phy = {0};
	unsigned int c;

	struct axi_adc_init rx1_adc_init = {
		.name = "axi-adrv9002-rx-lpc",
		.base = RX1_ADC_BASEADDR,
		.num_channels = ADRV9001_I_Q_CHANNELS,
	};

	struct axi_dac_channel  tx1_dac_channels[2];
	tx1_dac_channels[0].sel = AXI_DAC_DATA_SEL_DMA;
	tx1_dac_channels[1].sel = AXI_DAC_DATA_SEL_DMA;

	struct axi_dac_init tx1_dac_init = {
		.name = "axi-adrv9002-tx-lpc",
		.base = TX1_DAC_BASEADDR,
		.num_channels = ADRV9001_I_Q_CHANNELS,
		.channels = tx1_dac_channels,
		.rate = 3
	};

#ifndef ADRV9002_RX2TX2
	struct axi_adc_init rx2_adc_init = {
		.name = "axi-adrv9002-rx2-lpc",
		.base = RX2_ADC_BASEADDR,
		.num_channels = ADRV9001_I_Q_CHANNELS,
	};

	struct axi_dac_channel  tx2_dac_channels[2];
	tx2_dac_channels[0].sel = AXI_DAC_DATA_SEL_DMA;
	tx2_dac_channels[1].sel = AXI_DAC_DATA_SEL_DMA;

	struct axi_dac_init tx2_dac_init = {
		.name = "axi-adrv9002-tx2-lpc",
		.base = TX2_DAC_BASEADDR,
		.num_channels = ADRV9001_I_Q_CHANNELS,
		.channels = tx2_dac_channels,
		.rate = 3
	};
#endif
	struct axi_dmac_init rx1_dmac_init = {
		"rx_dmac",
		RX1_DMA_BASEADDR,
		IRQ_DISABLED
	};

	struct axi_dmac_init tx1_dmac_init = {
		"tx_dmac",
		TX1_DMA_BASEADDR,
		IRQ_DISABLED
	};

#ifndef ADRV9002_RX2TX2
	struct axi_dmac_init rx2_dmac_init = {
		"rx_dmac",
		RX2_DMA_BASEADDR,
		IRQ_DISABLED
	};

	struct axi_dmac_init tx2_dmac_init = {
		"tx_dmac",
		TX2_DMA_BASEADDR,
		IRQ_DISABLED
	};
#endif

#ifdef XILINX_PLATFORM
	Xil_ICacheEnable();
	Xil_DCacheEnable();
#endif /* XILINX_PLATFORM */

	printf("Hello\n");

#if defined(ADRV9002_RX2TX2)
	phy.rx2tx2 = true;
#endif

	phy.adrv9001 = &adrv9001_device;

	/* ADRV9002 */
	chip.cmos_profile = "Navassa_CMOS_profile.json";
	chip.lvd_profile = "Navassa_LVDS_profile.json";
	chip.name = "adrv9002-phy";
	chip.n_tx = ADRV9002_CHANN_MAX;

	phy.chip = &chip;

	ret = adi_adrv9001_profileutil_Parse(phy.adrv9001, &phy.profile,
					     (char *)json_profile, strlen(json_profile));
	if (ret)
		goto error;

	phy.curr_profile = &phy.profile;

	sampling_freq = phy.curr_profile->rx.rxChannelCfg[0].profile.rxOutputRate_Hz;

	/* Initialize the ADC/DAC cores */
	ret = axi_adc_init_begin(&phy.rx1_adc, &rx1_adc_init);
	if (ret) {
		printf("axi_adc_init_begin() failed with status %d\n", ret);
		goto error;
	}

	ret = axi_dac_init_begin(&phy.tx1_dac, &tx1_dac_init);
	if (ret) {
		printf("axi_dac_init_begin() failed with status %d\n", ret);
		goto error;
	}
#ifndef ADRV9002_RX2TX2
	ret = axi_adc_init_begin(&phy.rx2_adc, &rx2_adc_init);
	if (ret) {
		printf("axi_adc_init_begin() failed with status %d\n", ret);
		goto error;
	}

	ret = axi_dac_init_begin(&phy.tx2_dac, &tx2_dac_init);
	if (ret) {
		printf("axi_dac_init_begin() failed with status %d\n", ret);
		goto error;
	}
#endif

	phy.ssi_type = adrv9002_ssi_type_detect(&phy);
	if (phy.ssi_type == ADI_ADRV9001_SSI_TYPE_DISABLE)
		goto error;

	/* Initialize AGC */
	for (c = 0; c < ADRV9002_CHANN_MAX; c++) {
		phy.rx_channels[c].agc = agc_defaults;
	}

	ret = adrv9002_setup(&phy);
	if (ret)
		return ret;

	adi_adrv9001_ApiVersion_Get(phy.adrv9001, &api_version);
	adi_adrv9001_arm_Version(phy.adrv9001, &arm_version);
	adi_adrv9001_SiliconVersion_Get(phy.adrv9001, &silicon_version);

	printf("%s Rev %d.%d, Firmware %u.%u.%u.%u API version: %u.%u.%u successfully initialized\n",
	       "ADRV9002", silicon_version.major, silicon_version.minor,
	       arm_version.majorVer, arm_version.minorVer,
	       arm_version.maintVer, arm_version.rcVer, api_version.major,
	       api_version.minor, api_version.patch);

	/* Post AXI DAC/ADC setup, digital interface tuning */
	ret = adrv9002_post_setup(&phy);
	if (ret) {
		printf("adrv9002_post_setup() failed with status %d\n", ret);
		goto error;
	}

	/* Finalize the ADC/DAC cores initialization */
	ret = axi_adc_init_finish(phy.rx1_adc);
	if (ret) {
		printf("axi_adc_init_finish() failed with status %d\n", ret);
		goto error;
	}

	ret = axi_dac_init_finish(phy.tx1_dac);
	if (ret) {
		printf("axi_dac_init_finish() failed with status %d\n", ret);
		goto error;
	}
	phy.tx1_dac->clock_hz = phy.curr_profile->tx.txProfile[0].txInputRate_Hz;
#ifndef ADRV9002_RX2TX2
	ret = axi_adc_init_finish(phy.rx2_adc);
	if (ret) {
		printf("axi_adc_init_finish() failed with status %d\n", ret);
		goto error;
	}

	ret = axi_dac_init_finish(phy.tx2_dac);
	if (ret) {
		printf("axi_dac_init_finish() failed with status %d\n", ret);
		goto error;
	}
	phy.tx2_dac->clock_hz = phy.curr_profile->tx.txProfile[1].txInputRate_Hz;
#endif

	/* Initialize the AXI DMA Controller cores */
	ret = axi_dmac_init(&phy.tx1_dmac, &tx1_dmac_init);
	if (ret) {
		printf("axi_dmac_init() failed with status %d\n", ret);
		goto error;
	}

	ret = axi_dmac_init(&phy.rx1_dmac, &rx1_dmac_init);
	if (ret) {
		printf("axi_dmac_init() failed with status %d\n", ret);
		goto error;
	}
#ifndef ADRV9002_RX2TX2
	ret = axi_dmac_init(&phy.tx2_dmac, &tx2_dmac_init);
	if (ret) {
		printf("axi_dmac_init() failed with status %d\n", ret);
		goto error;
	}

	ret = axi_dmac_init(&phy.rx2_dmac, &rx2_dmac_init);
	if (ret) {
		printf("axi_dmac_init() failed with status %d\n", ret);
		goto error;
	}
#endif

#ifdef DMA_EXAMPLE
	axi_dac_load_custom_data(phy.tx1_dac, sine_lut_iq,
				 NO_OS_ARRAY_SIZE(sine_lut_iq),
				 (uintptr_t)dac_buffers[0]);
#ifndef ADRV9002_RX2TX2
	axi_dac_load_custom_data(phy.tx2_dac, sine_lut_iq,
				 NO_OS_ARRAY_SIZE(sine_lut_iq),
				 (uintptr_t)dac_buffers[1]);
#endif
#ifdef XILINX_PLATFORM
	Xil_DCacheFlush();
#endif /* XILINX_PLATFORM */

	struct axi_dma_transfer transfer1 = {
		// Number of bytes to write/read
		.size = sizeof(sine_lut_iq),
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = CYCLIC,
		// Address of data source
		.src_addr = (uintptr_t)dac_buffers[0],
		// Address of data destination
		.dest_addr = 0
	};
	axi_dmac_transfer_start(phy.tx1_dmac, &transfer1);
#ifndef ADRV9002_RX2TX2
	struct axi_dma_transfer transfer2 = {
		// Number of bytes to write/read
		.size = sizeof(sine_lut_iq),
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = CYCLIC,
		// Address of data source
		.src_addr = (uintptr_t)dac_buffers[1],
		// Address of data destination
		.dest_addr = 0
	};
	axi_dmac_transfer_start(phy.tx2_dmac, &transfer2);
#endif

#ifdef XILINX_PLATFORM
	Xil_DCacheInvalidateRange((uintptr_t)adc_buffers[0], sizeof(sine_lut_iq));
#ifndef ADRV9002_RX2TX2
	Xil_DCacheInvalidateRange((uintptr_t)adc_buffers[1], sizeof(sine_lut_iq));
#endif
#endif /* XILINX_PLATFORM */

	no_os_mdelay(1000);

	struct axi_dma_transfer read_transfer1 = {
		// Number of bytes to write/read
		.size = ADC_BUFFER_SAMPLES * ADRV9001_I_Q_CHANNELS * 2, /* nr of samples * rx1 i/q, rx2 i/q * bytes per sample */
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)adc_buffers[0]
	};
	/* Transfer ADC_BUFFER_SAMPLES samples from ADC to MEM */
#ifdef ADRV9002_RX2TX2
	axi_adc_update_active_channels(phy.rx1_adc, 0xf);
#else
	axi_adc_update_active_channels(phy.rx1_adc, 0x3);
#endif
	axi_dmac_transfer_start(phy.rx1_dmac, &read_transfer1);
	ret = axi_dmac_transfer_wait_completion(phy.rx1_dmac, 500);
	if(ret)
		return ret;
#ifdef XILINX_PLATFORM
	Xil_DCacheInvalidateRange((uintptr_t)adc_buffers[0],
				  ADC_BUFFER_SAMPLES * /* nr of samples */
				  ADRV9001_I_Q_CHANNELS * /* rx1 i/q, rx2 i/q*/
				  2 /* bytes per sample */);
#endif /* XILINX_PLATFORM */
#ifndef ADRV9002_RX2TX2
	struct axi_dma_transfer read_transfer2 = {
		// Number of bytes to write/read
		.size = ADC_BUFFER_SAMPLES * ADRV9001_I_Q_CHANNELS * 2, /* nr of samples * rx1 i/q, rx2 i/q * bytes per sample */
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)adc_buffers[1]
	};
	axi_adc_update_active_channels(phy.rx2_adc, 0x3);
	axi_dmac_transfer_start(phy.rx2_dmac,&read_transfer2);
	ret = axi_dmac_transfer_wait_completion(phy.rx2_dmac, 500);
	if(ret)
		return ret;
#ifdef XILINX_PLATFORM
	Xil_DCacheInvalidateRange((uintptr_t)adc_buffers[1],
				  ADC_BUFFER_SAMPLES * /* nr of samples */
				  ADRV9001_I_Q_CHANNELS * /* nr of channels */
				  2 /* bytes per sample */);
#endif /* XILINX_PLATFORM */
	printf("DMA_EXAMPLE: address=%#lx samples=%lu channels=%u bits=%lu\n",
	       (uintptr_t)adc_buffers[1], ADC_BUFFER_SAMPLES * rx2_adc_init.num_channels,
	       rx2_adc_init.num_channels, 8 * sizeof(adc_buffers[1][0]));
#endif
	printf("DMA_EXAMPLE: address=%#lx samples=%lu channels=%u bits=%lu\n",
	       (uintptr_t)adc_buffers[0], ADC_BUFFER_SAMPLES * rx1_adc_init.num_channels,
	       rx1_adc_init.num_channels, 8 * sizeof(adc_buffers[0][0]));
#endif

#ifdef IIO_SUPPORT
	struct iio_axi_adc_init_param iio_axi_adcs_init_par[] = {{
			.rx_adc = phy.rx1_adc,
			.rx_dmac = phy.rx1_dmac,
#ifdef XILINX_PLATFORM
			.dcache_invalidate_range = (void (*)(uint32_t, uint32_t))Xil_DCacheInvalidateRange,
#endif /* XILINX_PLATFORM */
			.get_sampling_frequency = get_sampling_frequency,
		},
#ifndef ADRV9002_RX2TX2
		{
			.rx_adc = phy.rx2_adc,
			.rx_dmac = phy.rx2_dmac,
#ifdef XILINX_PLATFORM
			.dcache_invalidate_range = (void (*)(uint32_t, uint32_t))Xil_DCacheInvalidateRange,
#endif /* XILINX_PLATFORM */
			.get_sampling_frequency = get_sampling_frequency,
		}
#endif
	};

	struct iio_axi_dac_init_param iio_axi_dacs_init_par[] = {{
			.tx_dac = phy.tx1_dac,
			.tx_dmac = phy.tx1_dmac,
#ifdef XILINX_PLATFORM
			.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange,
#endif /* XILINX_PLATFORM */
		},
#ifndef ADRV9002_RX2TX2
		{
			.tx_dac = phy.tx2_dac,
			.tx_dmac = phy.tx2_dmac,
#ifdef XILINX_PLATFORM
			.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange,
#endif /* XILINX_PLATFORM */
		}
#endif
	};

	ret = iio_run(iio_axi_adcs_init_par, iio_axi_dacs_init_par);
	if (ret < 0) {
		printf("iio_run() failed with status %d\n", ret);
		goto error;
	}
#endif
	printf("Bye\n");

error:
	adi_adrv9001_HwClose(phy.adrv9001);
	axi_adc_remove(phy.rx1_adc);
	axi_dac_remove(phy.tx1_dac);
	axi_adc_remove(phy.rx2_adc);
	axi_dac_remove(phy.tx2_dac);
	axi_dmac_remove(phy.rx1_dmac);
	axi_dmac_remove(phy.tx1_dmac);
	axi_dmac_remove(phy.rx2_dmac);
	axi_dmac_remove(phy.tx2_dmac);
	return ret;
}
