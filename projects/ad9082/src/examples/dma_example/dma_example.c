/***************************************************************************//**
 *   @file   dma_example.c
 *   @brief  DMA example for the AD9082 (VPK180) project.
 *
 *           Brings the HMC7044 clocks, the AXI-JESD204 RX/TX cores and the
 *           AD9082 (ad9081 driver) up exactly like the basic example, then
 *           moves data: a sine wave is played out of the DACs from a DDR
 *           buffer through the TX DMAC, and ADC samples are captured into a DDR
 *           buffer through the RX DMAC.
 *
 *           NOTE 1: the PL DMACs master the DDR over the Versal NoC, which the
 *           ADI vpk180 HDL already routes to the DDR controller.  They are not
 *           cache-coherent with the CPU, so the buffers live in a dedicated DDR
 *           region (parameters.h) and the CPU flushes / invalidates them by hand
 *           around each transfer; otherwise the DAC plays stale data or the RX
 *           capture reads back stale cache contents.
 *
 *           NOTE 2: the HDL places an AXI data-offload between the TX DMAC and
 *           the DAC.  Loaded once with a NON-cyclic transfer, the offload
 *           replays its BRAM continuously, giving a gap-free DAC output.  The
 *           sine block is therefore replicated across the BRAM so the replay
 *           never streams uninitialised memory.  (Driving the DMAC in cyclic
 *           mode instead chops the DAC output into bursts at each reload.)
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "dma_example.h"
#include "common_data.h"
#include "parameters.h"
#include "app_config.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "xilinx_gpio.h"
#include "xilinx_spi.h"
#include "xil_cache.h"

#include "ad9081.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"

/* TX plays the shared I/Q sine LUT; RX captures per-channel 16-bit samples for
 * every JESD converter of the device.  The DAC custom-data loader writes one
 * word per (LUT sample, I/Q channel) -- i.e. SINE_LUT_SAMPLES * (M/2) words --
 * so the DAC buffer must be sized to match or the load overruns it. */
#define SINE_LUT_SAMPLES	1024
#define DAC_BUFFER_SAMPLES	(SINE_LUT_SAMPLES * (AD9081_TX_JESD_M / 2))
#define DAC_BUFFER_BYTES	(DAC_BUFFER_SAMPLES * sizeof(uint32_t))
#define ADC_BUFFER_SAMPLES	16384
#define ADC_BUFFER_SAMPLES_TOTAL	(ADC_BUFFER_SAMPLES * AD9081_RX_JESD_M)
#define ADC_BUFFER_BYTES	(ADC_BUFFER_SAMPLES_TOTAL * sizeof(uint16_t))

/* The TX data-offload replays its whole BRAM, so the one sine block written by
 * axi_dac_load_custom_data is replicated across the BRAM (DAC_FILL_REPS copies)
 * and loaded with a single non-cyclic transfer -- see NOTE 2 in the file
 * banner.  DAC_FILL_REPS * DAC_BUFFER_BYTES must stay within the dedicated DDR
 * region and clear of ADC_DDR_BASEADDR. */
#define DAC_FILL_REPS		16
#define DAC_FILL_BYTES		(DAC_BUFFER_BYTES * DAC_FILL_REPS)

/* The PL DMACs are not cache-coherent with the CPU on this design, so the CPU
 * manages coherency by hand (flush before the DMAC reads the DAC buffer,
 * invalidate before/after it writes the ADC buffer).  The buffers live in a
 * dedicated DDR region (DAC_DDR_BASEADDR / ADC_DDR_BASEADDR in parameters.h),
 * away from the CPU image / stack / heap, so a cache op on one buffer can never
 * disturb the other or any program data.  The bases are 64 MB aligned, well
 * above the cache line (64 B on the A72, 32 B on the R5), and the same for both
 * cores -- no per-core split is required. */
static uint32_t * const dac_buffer_dma = (uint32_t *)DAC_DDR_BASEADDR;
static uint16_t * const adc_buffer_dma = (uint16_t *)ADC_DDR_BASEADDR;

/* Shared sine look-up table, defined in the AXI DAC core driver. */
extern const uint32_t sine_lut_iq[1024];

/***************************************************************************//**
 * @brief DMA example main execution.
 *
 * @return 0 on success, negative error code otherwise.
*******************************************************************************/
int example_main(void)
{
	struct no_os_clk app_clk[MULTIDEVICE_INSTANCE_COUNT];
	struct no_os_clk jesd_clk[2];

	/* MxFE reset / sync lines are on an AXI GPIO in the PL (XGpio). */
	struct xil_gpio_init_param xil_gpio_param = {
		.type = GPIO_PL,
#ifdef SDT
		.base_addr = XPAR_XGPIO_0_BASEADDR,
#endif
		.device_id = GPIO_DEVICE_ID
	};
	struct no_os_gpio_init_param gpio_phy_resetb = {
		.number = PHY_RESET,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_param
	};

	/* AD9082 (MxFE) on PS SPI0. */
	struct xil_spi_init_param xil_spi_param = {
		.type = SPI_PS,
#ifdef SDT
		.base_addr = XPAR_XSPIPS_0_BASEADDR,
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
		.multidevice_instance_count = 1,
		.jesd_sync_pins_01_swap_enable = false,
		.config_sync_0a_cmos_enable = false,
		.lmfc_delay_dac_clk_cycles = 0,
		.nco_sync_ms_extra_lmfc_num = 0,
		.nco_sync_direct_sysref_mode_enable = 0,
		.sysref_average_cnt_exp = 7,
		.continuous_sysref_mode_disable = 0,
		.tx_disable = false,
		.rx_disable = false,
		/* TX */
		.dac_frequency_hz = AD9081_DAC_FREQUENCY,
		.tx_main_interpolation = AD9081_TX_MAIN_INTERPOLATION,
		/* Play the sine LUT at baseband (no main-NCO upconversion) so the
		 * DAC->ADC loopback captures a recognisable tone.  The shared profile
		 * shifts by 1 GHz, which in this full-bandwidth RX profile aliases to
		 * broadband hash in the capture; baseband keeps the example's captured
		 * data plottable.  Matches the Linux m1-l8 reference (nco-frequency-
		 * shift-hz = 0). */
		.tx_main_nco_frequency_shift_hz = {0, 0, 0, 0},
		.tx_dac_channel_crossbar_select = AD9081_TX_DAC_CHAN_CROSSBAR,
		.tx_maindp_dac_1x_non1x_crossbar_select = AD9081_TX_DAC_1X_NON1X_CROSSBAR,
		.tx_full_scale_current_ua = AD9081_TX_FSC,
		.tx_channel_interpolation = AD9081_TX_CHAN_INTERPOLATION,
		.tx_channel_nco_frequency_shift_hz = AD9081_TX_CHAN_NCO_SHIFT,
		.tx_channel_gain = AD9081_TX_CHAN_GAIN,
		.jrx_link_tx[0] = &jrx_link_tx,
		.jrx_link_tx[1] = NULL,
		/* RX */
		.adc_frequency_hz = AD9081_ADC_FREQUENCY,
		.nyquist_zone = AD9081_ADC_NYQUIST_ZONE,
		.rx_main_nco_frequency_shift_hz = AD9081_RX_MAIN_NCO_SHIFT,
		.rx_main_decimation = AD9081_RX_MAIN_DECIMATION,
		.rx_main_complex_to_real_enable = {0, 0, 0, 0},
		.rx_main_digital_gain_6db_enable = {0, 0, 0, 0},
		.rx_main_enable = AD9081_RX_MAIN_ENABLE,
		.rx_channel_nco_frequency_shift_hz = AD9081_RX_CHAN_NCO_SHIFT,
		.rx_channel_decimation = AD9081_RX_CHAN_DECIMATION,
		.rx_channel_complex_to_real_enable = {0, 0, 0, 0, 0, 0, 0, 0},
		.rx_channel_nco_mixer_mode = {
			AD9081_ADC_NCO_VIF, AD9081_ADC_NCO_VIF,
			AD9081_ADC_NCO_VIF, AD9081_ADC_NCO_VIF, AD9081_ADC_NCO_VIF,
			AD9081_ADC_NCO_VIF, AD9081_ADC_NCO_VIF, AD9081_ADC_NCO_VIF
		},
		.rx_channel_digital_gain_6db_enable = {0, 0, 0, 0, 0, 0, 0, 0},
		.rx_channel_enable = AD9081_RX_CHAN_ENABLE,
		.jtx_link_rx[0] = &jtx_link_rx,
		.jtx_link_rx[1] = NULL,
		.leader_follower_sync = false,
		.nco_test = false,
	};

	struct axi_adc_init rx_adc_init = {
		.name = "rx_adc",
		.base = RX_CORE_BASEADDR
	};
	struct axi_adc *rx_adc;
	struct axi_dac_init tx_dac_init = {
		.name = "tx_dac",
		.base = TX_CORE_BASEADDR,
		.channels = NULL
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
	struct ad9081_phy *phy[MULTIDEVICE_INSTANCE_COUNT];
	int32_t status;
	int32_t i;
	uint32_t rep;

	printf("Hello, this is the dma example for ad9082\n");

	/* Self-report the core so the console log identifies APU vs RPU builds. */
#if defined(__aarch64__)
	printf("Running on Cortex-A72 (APU, aarch64)\n");
#elif defined(__arm__)
	printf("Running on Cortex-R5F (RPU, armv7-r)\n");
#endif

	/* Enable the instruction and data caches. */
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	status = app_clock_init(app_clk);
	if (status != 0) {
		printf("app_clock_init() error: %" PRId32 "\n", status);
		goto error;
	}

	status = app_jesd_init(jesd_clk, ADXCVR_REF_CLK_KHZ,
			       ADXCVR_RX_DEV_CLK_KHZ, ADXCVR_TX_DEV_CLK_KHZ,
			       ADXCVR_RX_LANE_CLK_KHZ, ADXCVR_TX_LANE_CLK_KHZ);
	if (status != 0) {
		printf("app_jesd_init() error: %" PRId32 "\n", status);
		goto error_clk;
	}

	rx_adc_init.num_channels = 0;
	tx_dac_init.num_channels = 0;

	for (i = 0; i < MULTIDEVICE_INSTANCE_COUNT; i++) {
		gpio_phy_resetb.number = PHY_RESET + i;
		phy_spi_init_param.chip_select = PHY_CS + i;
		phy_param.dev_clk = &app_clk[i];
		jtx_link_rx.device_id = i;

		status = ad9081_init(&phy[i], &phy_param);
		if (status != 0) {
			printf("ad9081_init() error: %" PRId32 "\n", status);
			goto error_phy;
		}

		rx_adc_init.num_channels += phy[i]->jtx_link_rx[0].jesd_param.jesd_m +
					    phy[i]->jtx_link_rx[1].jesd_param.jesd_m;

		tx_dac_init.num_channels += phy[i]->jrx_link_tx[0].jesd_param.jesd_m *
					    (phy[i]->jrx_link_tx[0].jesd_param.jesd_duallink > 0 ? 2 : 1);
	}

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
		{
			.jdev = phy[0]->jdev,
			.link_ids = {FRAMER_LINK0_RX, DEFRAMER_LINK0_TX},
			.links_number = 2,
			.is_top_device = true,
		},
	};

	status = jesd204_topology_init(&topology, devs,
				       sizeof(devs) / sizeof(*devs));
	if (status) {
		printf("jesd204_topology_init() error: %" PRId32 "\n", status);
		goto error_phy;
	}

	status = jesd204_fsm_start(topology, JESD204_LINKS_ALL);
	if (status) {
		printf("jesd204_fsm_start() error: %" PRId32 "\n", status);
		goto error_phy;
	}

	axi_jesd204_rx_watchdog(rx_jesd);

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);

	status = axi_dac_init(&tx_dac, &tx_dac_init);
	if (status != 0) {
		printf("axi_dac_init() error: %" PRId32 "\n", status);
		goto error_phy;
	}
	status = axi_adc_init(&rx_adc, &rx_adc_init);
	if (status != 0) {
		printf("axi_adc_init() error: %" PRId32 "\n", status);
		goto error_dac;
	}

	status = axi_dmac_init(&tx_dmac, &tx_dmac_init);
	if (status != 0) {
		printf("axi_dmac_init() tx error: %" PRId32 "\n", status);
		goto error_adc;
	}
	status = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (status != 0) {
		printf("axi_dmac_init() rx error: %" PRId32 "\n", status);
		goto error_txdmac;
	}

	/* ---- TX: play the sine LUT out of the DACs from DDR ----
	 * Each step is logged with plain printf so a stall is pin-pointable. */
	printf("[dma] TX: set DAC data source to DMA\n");
	status = axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_DMA);
	if (status) {
		printf("axi_dac_set_datasel() error: %" PRId32 "\n", status);
		goto error_rxdmac;
	}

	printf("[dma] TX: load sine LUT into DDR buffer @%#lx\n",
	       (uintptr_t)dac_buffer_dma);
	status = axi_dac_load_custom_data(tx_dac, sine_lut_iq, SINE_LUT_SAMPLES,
					  (uintptr_t)dac_buffer_dma);
	if (status) {
		printf("axi_dac_load_custom_data() error: %" PRId32 "\n", status);
		goto error_rxdmac;
	}
	/* Replicate the one loaded block across the offload BRAM so its
	 * continuous replay streams the sine end-to-end, never stale memory. */
	for (rep = 1; rep < DAC_FILL_REPS; rep++)
		memcpy((uint8_t *)dac_buffer_dma + rep * DAC_BUFFER_BYTES,
		       dac_buffer_dma, DAC_BUFFER_BYTES);
	/* Clean the written samples from cache to DDR so the DMAC (not
	 * cache-coherent) reads the real data, not stale memory. */
	Xil_DCacheFlushRange((uintptr_t)dac_buffer_dma, DAC_FILL_BYTES);

	struct axi_dma_transfer tx_transfer = {
		.size = DAC_FILL_BYTES,
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = (uintptr_t)dac_buffer_dma,
		.dest_addr = 0
	};
	printf("[dma] TX: load offload (%u bytes, non-cyclic)\n",
	       (unsigned)DAC_FILL_BYTES);
	status = axi_dmac_transfer_start(tx_dmac, &tx_transfer);
	printf("[dma] TX: transfer_start returned %" PRId32 "\n", status);
	if (status)
		goto error_rxdmac;

	pr_info("DMA_EXAMPLE Tx: address=%#lx samples=%lu channels=%u bits=32\n",
		(uintptr_t)dac_buffer_dma,
		(unsigned long)(DAC_FILL_BYTES / sizeof(uint32_t)),
		(unsigned)AD9081_TX_JESD_M);

	/* Let the DAC output settle before capturing. */
	no_os_mdelay(200);

	/* ---- RX: capture ADC samples into DDR through the RX DMAC ---- */
	struct axi_dma_transfer rx_transfer = {
		.size = ADC_BUFFER_BYTES,
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = 0,
		.dest_addr = (uintptr_t)adc_buffer_dma
	};
	/* Drop any stale/dirty cache lines for the capture buffer before the
	 * DMAC writes it, so no later eviction overwrites the captured samples. */
	Xil_DCacheInvalidateRange((uintptr_t)adc_buffer_dma, ADC_BUFFER_BYTES);

	printf("[dma] RX: start capture -> DDR @%#lx (%u bytes)\n",
	       (uintptr_t)adc_buffer_dma, (unsigned)ADC_BUFFER_BYTES);
	status = axi_dmac_transfer_start(rx_dmac, &rx_transfer);
	printf("[dma] RX: transfer_start returned %" PRId32 "\n", status);
	if (status)
		goto error_txstop;

	printf("[dma] RX: waiting for completion (<=1000 ms)...\n");
	status = axi_dmac_transfer_wait_completion(rx_dmac, 1000);
	printf("[dma] RX: wait_completion returned %" PRId32 "%s\n", status,
	       status ? " (RX DMA timed out)" : "");
	if (status)
		goto error_txstop;

	/* Invalidate again so the CPU reads the freshly captured DDR contents and
	 * not whatever the cache still holds for this region. */
	Xil_DCacheInvalidateRange((uintptr_t)adc_buffer_dma, ADC_BUFFER_BYTES);

	/* Report the capture buffer in the shared DMA_EXAMPLE format so the
	 * standard tooling can pull it straight into .csv (see the README):
	 *   xsct .../capture.tcl VERSAL <addr> <samples> <ch> 16
	 *   python .../plot.py <ch>
	 */
	pr_info("DMA_EXAMPLE Rx: address=%#lx samples=%lu channels=%u bits=16\n",
		(uintptr_t)adc_buffer_dma,
		(unsigned long)ADC_BUFFER_SAMPLES_TOTAL,
		(unsigned)AD9081_RX_JESD_M);

error_txstop:
	axi_dmac_transfer_stop(tx_dmac);
error_rxdmac:
	axi_dmac_remove(rx_dmac);
error_txdmac:
	axi_dmac_remove(tx_dmac);
error_adc:
	axi_adc_remove(rx_adc);
error_dac:
	axi_dac_remove(tx_dac);
error_phy:
	while (i--)
		ad9081_remove(phy[i]);
	axi_jesd204_tx_remove(tx_jesd);
	axi_jesd204_rx_remove(rx_jesd);
error_clk:
	hmc7044_remove(hmc7044_dev);

	if (status)
		goto error;

	printf("Bye\n");

	/* Disable the data and instruction caches. */
	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;

error:
	printf("Error %" PRId32 "\n", status);

	return status;
}
