/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for MAX30009 Bioimpedance Measurement
 *   @author Edelweise Escala (edelweise.escala@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
#include <errno.h>

#include "max30009.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_irq.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"
#include "no_os_util.h"
#include "parameters.h"

static volatile bool max30009_data_ready = false;

/**
 * @brief GPIO interrupt callback — runs in ISR context
 */
static void max30009_irq_handler(void *ctx)
{
	(void)ctx;
	max30009_data_ready = true;
}

/**
 * @brief Initializes the interrupt for the MAX30009 device
 * @param device - Pointer to the MAX30009 device descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int max30009_init_interrupt(struct max30009_dev *device)
{
	int ret;
	struct no_os_irq_ctrl_desc *irq_ctrl_desc;
	struct no_os_callback_desc max30009_gpio_irq_cb = {
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = MAX30009_GPIO_CB_HANDLE,
		.callback = max30009_irq_handler,
		.ctx = device,
	};

	ret = no_os_irq_ctrl_init(&irq_ctrl_desc, &max30009_gpio_irq_ip);
	if (ret) {
		pr_err("Failed to initialize IRQ controller: %d\n", ret);
		goto error;
	}

	ret = no_os_irq_register_callback(irq_ctrl_desc,
					  MAX30009_GPIO_TRIG_IRQ_ID,
					  &max30009_gpio_irq_cb);
	if (ret) {
		pr_err("Failed to register IRQ callback: %d\n", ret);
		goto remove_irq;
	}

	ret = no_os_irq_trigger_level_set(irq_ctrl_desc, MAX30009_GPIO_TRIG_IRQ_ID,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret) {
		pr_err("Failed to set IRQ trigger level: %d\n", ret);
		goto remove_irq;
	}

	ret = no_os_irq_set_priority(irq_ctrl_desc, MAX30009_GPIO_TRIG_IRQ_ID, 1);
	if (ret) {
		pr_err("Failed to set IRQ priority: %d\n", ret);
		goto remove_irq;
	}

	ret = no_os_irq_enable(irq_ctrl_desc, MAX30009_GPIO_TRIG_IRQ_ID);
	if (ret) {
		pr_err("Failed to enable IRQ: %d\n", ret);
		goto remove_irq;
	}

	return 0;

remove_irq:
	no_os_irq_ctrl_remove(irq_ctrl_desc);
error:
	return ret;
}

/**
 * @brief Main example function following clean initialization flow
 */
int example_main(void)
{
	struct max30009_dev *dev;
	struct max30009_pll_config pll_cfg;
	struct max30009_bioz_config bioz_cfg;
	struct max30009_bioz_mux_config mux_cfg;
	struct max30009_lead_detect_config lead_cfg;
	struct max30009_fifo_sample sample;
	uint8_t fifo_data[256 * MAX30009_FIFO_DATA_SIZE];
	uint16_t fifo_count;
	uint32_t elapsed;
	int ret;
	int i;

	pr_info("MAX30009 Basic BioZ Example\n");

	pr_info("Initializing MAX30009...\n");
	ret = max30009_init(&dev, &max30009_init_params);
	if (ret) {
		pr_err("  ERROR: Initialization failed: %d\n", ret);
		return ret;
	}
	pr_info("  SUCCESS: Device initialized\n\n");

	pr_info("Initializing interrupt...\n");
	pr_info("  INTB pin: P%d.%d (MAX32655FTHR)\n", GPIO_INTB_PORT_NUM,
		GPIO_INTB_PIN_NUM);
	ret = max30009_init_interrupt(dev);
	if (ret) {
		pr_err("  ERROR: Interrupt initialization failed: %d\n", ret);
		goto error;
	}
	pr_info("  SUCCESS: Interrupt initialized (falling edge trigger)\n\n");

	pr_info("Configuring PLL...\n");
	pr_info("  Target: 50 kHz stimulus, 100 sps sample rate\n");

	/* PLL Configuration for 50 kHz stimulus @ 100 sps
	 * Using internal 32.768 kHz clock
	 * PLL_CLK = REF_CLK × (MDIV + 1)
	 * Target PLL_CLK = 25.6 MHz
	 * MDIV = (25.6 MHz / 32.768 kHz) - 1 = 780
	 *
	 * F_BIOZ = PLL_CLK / (KDIV × DAC_OSR)
	 * 50 kHz = 25.6 MHz / (2 × 256)
	 *
	 * SR_BIOZ = PLL_CLK / (NDIV × ADC_OSR)
	 * 100 sps = 25.6 MHz / (512 × 512)
	 */
	pll_cfg.use_external_clk = false;
	pll_cfg.use_32768_hz = true;
	pll_cfg.mdiv = 511;
	pll_cfg.ndiv = MAX30009_NDIV_1024;
	pll_cfg.kdiv = MAX30009_KDIV_2;
	pll_cfg.pll_lock_wndw = false;
	pll_cfg.clk_fine_tune = 0;

	ret = max30009_set_pll_config(dev, &pll_cfg);
	if (ret) {
		pr_err("  ERROR: PLL configuration failed: %d\n", ret);
		goto error;
	}

	pr_info("Enabling PLL...\n");
	ret = max30009_pll_enable(dev, true);
	if (ret) {
		pr_err("  ERROR: PLL enable failed: %d\n", ret);
		goto error;
	}

	pr_info("  SUCCESS: PLL enabled\n");
	pr_info("    - PLL CLK: 25.6 MHz\n");
	pr_info("    - Stimulus frequency: 50 kHz\n");
	pr_info("    - Sample rate: 100 sps\n\n");

	pr_info("Configuring BioZ measurement...\n");

	bioz_cfg.dac_osr = MAX30009_DAC_OSR_256;
	bioz_cfg.adc_osr = MAX30009_ADC_OSR_1024;
	bioz_cfg.dhpf = MAX30009_DHPF_BYPASS;
	bioz_cfg.dlpf = MAX30009_DLPF_BYPASS;
	bioz_cfg.cmp_mode = 0;
	bioz_cfg.en_bioz_thresh = false;
	bioz_cfg.ext_res = false;
	bioz_cfg.loff_rapid = false;
	bioz_cfg.vdrv_mag = 2;
	bioz_cfg.idrv_rge = 2;
	bioz_cfg.drv_mode = MAX30009_CURRENT_DRV_MODE;
	bioz_cfg.fast_manual = false;
	bioz_cfg.fast_start_en = false;
	bioz_cfg.ahpf = MAX30009_AHPF_5KHZ;
	bioz_cfg.ina_mode = false;
	bioz_cfg.dm_dis = false;
	bioz_cfg.gain = MAX30009_BIOZ_GAIN_10;
	bioz_cfg.ext_cap = false;
	bioz_cfg.dc_restore = false;
	bioz_cfg.drv_reset = false;
	bioz_cfg.dac_reset = false;
	bioz_cfg.amp_rge = 0;
	bioz_cfg.amp_bw = MAX30009_BIOZ_HIGH_BW;
	bioz_cfg.lo_thresh = 0;
	bioz_cfg.hi_thresh = 0;
	bioz_cfg.stbyon = false;
	bioz_cfg.q_clk_phase = false;
	bioz_cfg.i_clk_phase = false;
	bioz_cfg.ina_chop_en = false;
	bioz_cfg.ch_fsel = false;

	ret = max30009_set_bioz_config(dev, &bioz_cfg);
	if (ret) {
		pr_err("  ERROR: BioZ configuration failed: %d\n", ret);
		goto error;
	}
	pr_info("  SUCCESS: BioZ configured\n");
	pr_info("    - Drive mode: Sine-wave current\n");
	pr_info("    - Current: 32 µA RMS (range 2, mag 2)\n");
	pr_info("    - Gain: 10 V/V\n");
	pr_info("    - Bandwidth: High\n\n");

	pr_info("Configuring electrode MUX...\n");
	pr_info("  Mode: 4-electrode (tetrapolar) BIA\n");

	/* Standard 4-electrode configuration:
	 * DRVP -> EL1 (current injection +)
	 * DRVN -> EL4 (current injection -)
	 * BIP  -> EL2A/EL2B (voltage sense +)
	 * BIN  -> EL3A/EL3B (voltage sense -)
	 */
	mux_cfg.bmux_rsel = MAX30009_BIA_600_OHM;
	mux_cfg.bmux_bist_en = false;
	mux_cfg.connect_cal_only = false;
	mux_cfg.mux_en = true;
	mux_cfg.cal_en = false;
	mux_cfg.bmux_gsr_rsel = MAX30009_GSR_101K_OHM;
	mux_cfg.gsr_load_en = false;
	mux_cfg.en_ext_inload = false;
	mux_cfg.en_int_inload = false;
	mux_cfg.bip_assign = 0x01;
	mux_cfg.bin_assign = 0x01;
	mux_cfg.drvp_assign = 0x00;
	mux_cfg.drvn_assign = 0x03;

	ret = max30009_set_bioz_mux_config(dev, &mux_cfg);
	if (ret) {
		pr_err("  ERROR: MUX configuration failed: %d\n", ret);
		goto error;
	}
	pr_info("  SUCCESS: MUX configured\n");
	pr_info("    - EL1 (DRVP): Current injection +\n");
	pr_info("    - EL4 (DRVN): Current injection -\n");
	pr_info("    - EL2A (BIP): Voltage sense +\n");
	pr_info("    - EL3A (BIN): Voltage sense -\n\n");

	pr_info("Configuring lead detection...\n");

	lead_cfg.en_lon_det = true;
	lead_cfg.en_loff_det = true;
	lead_cfg.en_ext_loff = false;
	lead_cfg.en_drv_oor = true;
	lead_cfg.loff_ipol = true;
	lead_cfg.loff_imag = MAX30009_LOFF_50NA;
	lead_cfg.loff_thresh = 8;
	lead_cfg.rbias_value = MAX30009_RBIAS_100M_OHM;
	lead_cfg.en_rbias_bip = true;
	lead_cfg.en_rbias_bin = true;

	ret = max30009_set_lead_detect_config(dev, &lead_cfg);
	if (ret) {
		pr_err("  ERROR: Lead detection configuration failed: %d\n", ret);
		goto error;
	}
	pr_info("  SUCCESS: Lead detection configured\n");
	pr_info("    - Lead-on detection: enabled\n");
	pr_info("    - Lead-off detection: enabled\n");
	pr_info("    - Drive OOR detection: enabled\n");
	pr_info("    - Lead-off current: 50 nA\n");
	pr_info("    - Lead bias: 100 MΩ\n\n");

	ret = max30009_set_fifo_watermark(dev, 1);
	if (ret) {
		pr_err("  ERROR: FIFO flush failed: %d\n", ret);
		goto error;
	}

	pr_info("\nFlushing FIFO to clear startup transients...\n");
	ret = max30009_clear_status(dev);
	if (ret) {
		pr_err("  ERROR: FIFO flush failed: %d\n", ret);
		goto error;
	}

	pr_info("\nFlushing FIFO to clear startup transients...\n");
	ret = max30009_fifo_flush(dev);
	if (ret) {
		pr_err("  ERROR: FIFO flush failed: %d\n", ret);
		goto error;
	}
	pr_info("  SUCCESS: FIFO flushed\n\n");

	pr_info("Enabling interrupts...\n");

	ret = max30009_enable_interrupt(dev, MAX30009_INT_FIFO_DATA_RDY, true);
	if (ret) {
		pr_err("  ERROR: Failed to enable FIFO data ready interrupt: %d\n", ret);
		goto error;
	}

	ret = max30009_enable_interrupt(dev, MAX30009_INT_A_FULL, true);
	if (ret) {
		pr_err("  ERROR: Failed to enable almost full interrupt: %d\n", ret);
		goto error;
	}

	pr_info("  SUCCESS: Interrupts enabled (FIFO data ready, almost full)\n");

	ret = max30009_bioz_bg_enable(dev, true);
	if (ret) {
		pr_err("  ERROR: BioZ BG enable failed: %d\n", ret);
		goto error;
	}

	pr_info("Enabling BioZ measurement pipeline...\n");

	ret = max30009_bioz_enable(dev, true);
	if (ret) {
		pr_err("  ERROR: BioZ enable failed: %d\n", ret);
		goto error;
	}
	pr_info("  SUCCESS: BioZ channels enabled (I, Q, BG)\n");

	pr_info("\nCollecting data for 10 seconds (interrupt-driven)...\n");

	elapsed = 0;
	while (elapsed < 10000) {
		if (max30009_data_ready) {
			max30009_data_ready = false;

			ret = max30009_fifo_get_count(dev, &fifo_count);
			if (ret || fifo_count == 0)
				goto next;

			ret = max30009_read_fifo_data(dev, fifo_data,
						      fifo_count * MAX30009_FIFO_DATA_SIZE);
			if (ret)
				goto next;

			for (i = 0; i < fifo_count; i++) {
				ret = max30009_fifo_decode_sample(
					      &fifo_data[i * MAX30009_FIFO_DATA_SIZE], &sample);
				if (ret)
					continue;
				if (sample.type == MAX30009_FIFO_SAMPLE_IN_PHASE)
					pr_info("I: %d\n", sample.data);
				else if (sample.type == MAX30009_FIFO_SAMPLE_QUADRATURE)
					pr_info("Q: %d\n", sample.data);
			}
next:
			max30009_clear_status(dev);
		}
		no_os_mdelay(1);
		elapsed++;
	}

	pr_info("\nData collection complete.\n");

	ret = max30009_enable_interrupt(dev, MAX30009_INT_FIFO_DATA_RDY, false);
	if (ret) {
		pr_err("  ERROR: Failed to disable FIFO data ready interrupt: %d\n", ret);
		goto error;
	}

	ret = max30009_enable_interrupt(dev, MAX30009_INT_A_FULL, false);
	if (ret) {
		pr_err("  ERROR: Failed to disable almost full interrupt: %d\n", ret);
		goto error;
	}

	pr_info("Example completed successfully!\n");

	max30009_remove(dev);
	return 0;

error:
	max30009_remove(dev);
	return ret;
}
