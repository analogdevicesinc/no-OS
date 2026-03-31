/***************************************************************************//**
 *   @file   curvetrace_example.c
 *   @brief  BJT Curve tracer example for AD5592R (NPN) and AD5593R (PNP).
 *   @author Sean Bono Macariola (SeanBono.Macariola@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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
#include "example.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"
#include "ad5592r.h"
#include "ad5593r.h"
#include "lm75.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define DEFINE_AD5592R_IP() { \
    .int_ref = true, \
    .spi_init = &ad5592r_spi_ip, \
    .i2c_init = NULL, \
    .ss_init = &ad5592r_spi_ss_ip, \
    .channel_modes = { \
        CH_MODE_DAC,         /* channel 0 */ \
        CH_MODE_ADC,         /* channel 1 */ \
        CH_MODE_DAC_AND_ADC, /* channel 2 */ \
        CH_MODE_DAC,         /* channel 3 For green LED */ \
        CH_MODE_UNUSED,      /* channel 4 */ \
        CH_MODE_UNUSED,      /* channel 5 */ \
        CH_MODE_UNUSED,      /* channel 6 */ \
        CH_MODE_UNUSED,      /* channel 7 */ \
     }, \
    .channel_offstate = { \
        CH_OFFSTATE_OUT_TRISTATE, /* channel 0 */ \
        CH_OFFSTATE_OUT_TRISTATE, /* channel 1 */ \
        CH_OFFSTATE_OUT_TRISTATE, /* channel 2 */ \
        CH_OFFSTATE_OUT_TRISTATE, /* channel 3 */ \
        CH_OFFSTATE_OUT_TRISTATE, /* channel 4 */ \
        CH_OFFSTATE_OUT_TRISTATE, /* channel 5 */ \
        CH_OFFSTATE_OUT_TRISTATE, /* channel 6 */ \
        CH_OFFSTATE_OUT_TRISTATE, /* channel 7 */ \
     }, \
    .adc_range = ZERO_TO_VREF, \
    .dac_range = ZERO_TO_VREF, \
    .adc_buf = false, \
}

#define DEFINE_AD5593R_IP() { \
    .int_ref = true, \
    .spi_init = NULL, \
    .i2c_init = &ad5593r_i2c_ip, \
    .ss_init = NULL, \
    .channel_modes = { \
        CH_MODE_DAC, /* channel 0 - Base drive */ \
        CH_MODE_ADC, /* channel 1 - Collector sense */ \
        CH_MODE_DAC_AND_ADC, /* channel 2 - Collector drive */ \
        CH_MODE_DAC, /* channel 3 - Emitter drive (PNP needs high-side supply) */ \
        CH_MODE_UNUSED, /* channel 4 */ \
        CH_MODE_UNUSED, /* channel 5 */ \
        CH_MODE_UNUSED, /* channel 6 */ \
        CH_MODE_UNUSED, /* channel 7 */ \
    }, \
 .channel_offstate = { \
        CH_OFFSTATE_OUT_TRISTATE, /* channel 0 */ \
        CH_OFFSTATE_OUT_TRISTATE, /* channel 1 */ \
        CH_OFFSTATE_OUT_TRISTATE, /* channel 2 */ \
        CH_OFFSTATE_OUT_TRISTATE, /* channel 3 */ \
        CH_OFFSTATE_OUT_TRISTATE, /* channel 4 */ \
        CH_OFFSTATE_OUT_TRISTATE, /* channel 5 */ \
        CH_OFFSTATE_OUT_TRISTATE, /* channel 6 */ \
        CH_OFFSTATE_OUT_TRISTATE, /* channel 7 */ \
    }, \
    .adc_range = ZERO_TO_VREF, \
    .dac_range = ZERO_TO_VREF, \
    .adc_buf = false, \
}

#define NUM_CURVES 5
#define NUM_POINTS 50

/*ASCII VALUES*/

#define GRAPH_HEIGHT 20
#define GRAPH_WIDTH  60



/* =========================================================================
 * Common curve trace infrastructure
 * ========================================================================= */

/** Function pointer types for AD559xR device operations */
typedef int32_t (*ad559xr_init_fn)(struct ad5592r_dev **,
				   struct ad5592r_init_param *);
typedef int32_t (*ad559xr_remove_fn)(struct ad5592r_dev *);
typedef int32_t (*ad559xr_write_dac_fn)(struct ad5592r_dev *, uint8_t,
					uint16_t);
typedef int32_t (*ad559xr_read_adc_fn)(struct ad5592r_dev *, uint8_t,
				       uint16_t *);

/** Configuration for a single curve trace run */
struct curve_trace_cfg {
	/* Device operations */
	ad559xr_init_fn init;
	ad559xr_remove_fn remove;
	ad559xr_write_dac_fn write_dac;
	ad559xr_read_adc_fn read_adc;

	/* Device init parameters */
	struct ad5592r_init_param dev_ip;

	/* Display labels */
	const char *device_name;   /* "AD5592R" or "AD5593R" */
	const char *bus_label;     /* "SPI" or "I2C" */
	const char *bjt_label;     /* "NPN" or "PNP" */
	const char *csv_tag;       /* "AD5592R_NPN" or "AD5593R_PNP" */

	/* Base voltage sweep (mV) */
	int16_t vb_start_mv;
	int16_t vb_step_mv;
	int num_curves;

	/* Collector voltage sweep (mV) */
	int16_t vc_start_mv;
	int16_t vc_step_mv;

	/* Number of curves to include in CSV output */
	int csv_curves;

	/* Store collector sweep points in reverse array order */
	bool reverse_fill;

	/* Use |Ic| for graph plotting */
	bool abs_current_plot;

	/* Invert Y-axis on ASCII graph */
	bool invert_y;

	/* Emitter drive: set emitter_drive_mv > 0 to enable */
	uint16_t emitter_drive_mv;
	uint8_t emitter_channel;

	/* Enable DAC test sweep before curve trace */
	bool dac_test_sweep;

	/* Enable verbose DAC init messages */
	bool verbose_init;
};

/**
 * @brief Run a DAC test sweep to verify DAC/ADC functionality.
 * @param uart_desc - UART descriptor for output.
 * @param cfg - Curve trace configuration.
 * @param dev - AD559xR device.
 * @param mV_per_lsb - Scale factor (mV per LSB).
 * @return 0 on success, negative error code otherwise.
 */
static int dac_test_sweep_run(struct no_os_uart_desc *uart_desc,
			      const struct curve_trace_cfg *cfg,
			      struct ad5592r_dev *dev, float mV_per_lsb)
{
	char msg_buf[128];
	uint16_t readback_raw;
	int ret;

	char msg_dac_test[] = "\n\rDAC Test Sweep (verifying DAC functionality):\n\r";
	no_os_uart_write(uart_desc, msg_dac_test, sizeof(msg_dac_test) - 1);

	for (uint16_t test_val = 0; test_val <= 4095; test_val += 1024) {
		ret = cfg->write_dac(dev, 2, test_val);
		if (ret)
			continue;
		no_os_mdelay(10);
		ret = cfg->read_adc(dev, 2, &readback_raw);
		if (ret == 0) {
			readback_raw &= 0x0FFF;
			float expected_v = (test_val * mV_per_lsb) / 1000.0f;
			float actual_v = (readback_raw * mV_per_lsb) / 1000.0f;
			sprintf(msg_buf,
				"  DAC=%u (%.3fV) -> ADC=%u (%.3fV) | diff=%.3fV\n\r",
				test_val, expected_v, readback_raw, actual_v,
				actual_v - expected_v);
			no_os_uart_write(uart_desc, msg_buf, strlen(msg_buf));
		}
	}

	return 0;
}

/**
 * @brief Plot curve data as an ASCII graph over UART.
 * @param uart_desc - UART descriptor for output.
 * @param cfg - Curve trace configuration.
 * @param curve_vcs - 2D array of collector voltages.
 * @param curve_ics - 2D array of collector currents.
 */
static void plot_ascii_graph(struct no_os_uart_desc *uart_desc,
			     const struct curve_trace_cfg *cfg,
			     float curve_vcs[][NUM_POINTS],
			     float curve_ics[][NUM_POINTS])
{
	char grid[GRAPH_HEIGHT + 2][GRAPH_WIDTH + 2];
	char buf[128];
	float max_v = 0.0f;
	float max_i = 0.0f;

	/* Initialize grid with spaces */
	for (int y = 0; y < GRAPH_HEIGHT + 2; y++)
		for (int x = 0; x < GRAPH_WIDTH + 2; x++)
			grid[y][x] = ' ';

	/* Draw borders */
	for (int x = 0; x < GRAPH_WIDTH + 2; x++) {
		grid[0][x] = '-';
		grid[GRAPH_HEIGHT + 1][x] = '-';
	}
	for (int y = 0; y < GRAPH_HEIGHT + 2; y++) {
		grid[y][0] = '|';
		grid[y][GRAPH_WIDTH + 1] = '|';
	}

	/* Draw corners */
	grid[0][0] = '+';
	grid[0][GRAPH_WIDTH + 1] = '+';
	grid[GRAPH_HEIGHT + 1][0] = '+';
	grid[GRAPH_HEIGHT + 1][GRAPH_WIDTH + 1] = '+';

	/* Find max voltage and current */
	for (int c = 0; c < cfg->num_curves; c++) {
		for (int p = 0; p < NUM_POINTS; p++) {
			if (curve_vcs[c][p] > max_v)
				max_v = curve_vcs[c][p];
			float i = cfg->abs_current_plot ?
				  (curve_ics[c][p] < 0 ?
				   -curve_ics[c][p] : curve_ics[c][p]) :
				  curve_ics[c][p];
			if (i > max_i)
				max_i = i;
		}
	}

	if (max_v < 0.01f) max_v = 1.0f;
	if (max_i < 0.001f) max_i = 0.01f;

	/* Plot curves */
	for (int c = 0; c < cfg->num_curves; c++) {
		for (int p = 0; p < NUM_POINTS; p++) {
			float v = curve_vcs[c][p];
			float i = cfg->abs_current_plot ?
				  (curve_ics[c][p] < 0 ?
				   -curve_ics[c][p] : curve_ics[c][p]) :
				  curve_ics[c][p];

			int x = 1 + (int)((v / max_v) * (GRAPH_WIDTH - 1));
			int y = 1 + (int)((i / max_i) * (GRAPH_HEIGHT - 1));

			if (cfg->invert_y)
				y = GRAPH_HEIGHT - y;

			if (x >= 1 && x <= GRAPH_WIDTH &&
			    y >= 1 && y <= GRAPH_HEIGHT)
				grid[y][x] = '*';
		}
	}

	/* Print graph header */
	no_os_mdelay(100);
	sprintf(buf, "\r\n\r\n === %s (%s) - %s Curve Tracer (Ic vs Vc) ===\r\n",
		cfg->device_name, cfg->bus_label, cfg->bjt_label);
	no_os_uart_write(uart_desc, buf, strlen(buf));

	sprintf(buf, "Y-axis: %s (0 to %.2f mA)\r\n",
		cfg->abs_current_plot ? "|Ic|" : "Ic", max_i);
	no_os_uart_write(uart_desc, buf, strlen(buf));

	sprintf(buf, "X-axis: Vc (0 to %.2f V)\r\n\r\n", max_v);
	no_os_uart_write(uart_desc, buf, strlen(buf));

	/* Print grid row by row */
	for (int y = 0; y < GRAPH_HEIGHT + 2; y++) {
		no_os_uart_write(uart_desc, (uint8_t*)grid[y], GRAPH_WIDTH + 2);
		no_os_uart_write(uart_desc, (uint8_t*)"\r\n", 2);
	}

	/* Print X-axis labels */
	sprintf(buf, "0.0");
	no_os_uart_write(uart_desc, (uint8_t*)buf, strlen(buf));

	float step = max_v / 5.0f;
	for (int k = 1; k <= 5; k++) {
		no_os_uart_write(uart_desc, (uint8_t*)"       ", 7);
		sprintf(buf, "%.2f", (double)(step * k));
		no_os_uart_write(uart_desc, (uint8_t*)buf, strlen(buf));
	}
	no_os_uart_write(uart_desc, (uint8_t*)" V\r\n\r\n", 6);

	sprintf(buf, "===== %s Curve Trace Complete =====\n\r\n\r",
		cfg->device_name);
	no_os_uart_write(uart_desc, (const uint8_t *)buf, strlen(buf));
}

/**
 * @brief Output curve data in CSV format over UART.
 * @param uart_desc - UART descriptor for output.
 * @param cfg - Curve trace configuration.
 * @param curve_vcs - 2D array of collector voltages.
 * @param curve_ics - 2D array of collector currents.
 */
static void output_csv_data(struct no_os_uart_desc *uart_desc,
			    const struct curve_trace_cfg *cfg,
			    float curve_vcs[][NUM_POINTS],
			    float curve_ics[][NUM_POINTS])
{
	char buf[128];
	int pos;

	sprintf(buf, "\n\r=== CSV DATA START: %s ===\n\r", cfg->csv_tag);
	no_os_uart_write(uart_desc, buf, strlen(buf));

	/* Generate CSV column header */
	pos = 0;
	for (int c = 0; c < cfg->csv_curves; c++) {
		pos += sprintf(buf + pos, "Vce%d,Ic%d", c + 1, c + 1);
		if (c < cfg->csv_curves - 1)
			pos += sprintf(buf + pos, ",");
	}
	pos += sprintf(buf + pos, "\n\r");
	no_os_uart_write(uart_desc, buf, pos);

	/* Output all curve data */
	for (int p = 0; p < NUM_POINTS; p++) {
		for (int c = 0; c < cfg->csv_curves; c++) {
			sprintf(buf, "%.4f,%.4f", curve_vcs[c][p],
				curve_ics[c][p]);
			no_os_uart_write(uart_desc, buf, strlen(buf));
			if (c < cfg->csv_curves - 1)
				no_os_uart_write(uart_desc, (uint8_t*)",", 1);
		}
		no_os_uart_write(uart_desc, (uint8_t*)"\r\n", 2);
	}

	char csv_footer[] = "=== CSV DATA END ===\n\r\n\r";
	no_os_uart_write(uart_desc, csv_footer, sizeof(csv_footer) - 1);
}

/**
 * @brief Common curve trace implementation for both NPN and PNP BJTs.
 * @param uart_desc - UART descriptor for output.
 * @param cfg - Curve trace configuration.
 * @return 0 on success, negative error code otherwise.
 */
static int curve_trace_common(struct no_os_uart_desc *uart_desc,
			      const struct curve_trace_cfg *cfg)
{
	struct ad5592r_dev *dev = NULL;
	uint16_t dac_value;
	float vc, ib, ic;
	uint32_t vref_mv;
	float mV_per_lsb;
	int ret;

	uint16_t Vcsense_raw;
	uint16_t Vcdrive_meas_raw;

	/* Circuit parameters */
	float Rsense = 47.0f;      /* Collector sense resistor (47 ohms) */
	float Rbase = 47.0e3f;     /* Base resistor (47 kOhms) */
	float Vbe = 0.7f;          /* Estimated base-emitter voltage drop */

	char msg_buf[128];

	/* Print title */
	sprintf(msg_buf, "\n\r========== %s (%s) %s Curve Tracer ==========\n\r",
		cfg->device_name, cfg->bus_label, cfg->bjt_label);
	no_os_uart_write(uart_desc, msg_buf, strlen(msg_buf));

	/* Initialize device */
	struct ad5592r_init_param ip = cfg->dev_ip;
	ret = cfg->init(&dev, &ip);
	if (ret) {
		sprintf(msg_buf, "Failed to initialize %s (%s)\n\r",
			cfg->device_name, cfg->bus_label);
		no_os_uart_write(uart_desc, msg_buf, strlen(msg_buf));
		goto cleanup;
	}

	sprintf(msg_buf, "%s (%s) initialized successfully\n\r",
		cfg->device_name, cfg->bus_label);
	no_os_uart_write(uart_desc, msg_buf, strlen(msg_buf));

	/* Get reference voltage */
	ret = ad5592r_get_ref(dev, &vref_mv);
	if (ret) {
		char msg_err[] = "Failed to get reference voltage\n\r";
		no_os_uart_write(uart_desc, msg_err, sizeof(msg_err) - 1);
		goto cleanup;
	}

	/* Calculate scale factor (mV per LSB) */
	mV_per_lsb = (float)vref_mv / (1 << 12);

	char scale_msg[64];
	sprintf(scale_msg, "Vref: %u mV, Scale: %.4f mV/LSB\n\r",
		vref_mv, mV_per_lsb);
	no_os_uart_write(uart_desc, scale_msg, strlen(scale_msg));

	/* Read temperature from channel 8 */
	uint16_t temp_raw;
	ret = cfg->read_adc(dev, 8, &temp_raw);
	if (ret == 0) {
		temp_raw &= 0x0FFF;
		float scale = ((float)vref_mv * 150715900.52f) / 1000000000.0f;
		float offset = (-75365.0f * 25.0f) / (float)vref_mv;
		float temperature = ((float)temp_raw + offset) * scale /
				    1000.0f; /* Convert mC to C */
		char temp_msg[64];
		sprintf(temp_msg, "%s Temperature: %.2f\xc2\xb0C (raw=%u)\n\r",
			cfg->device_name, temperature, temp_raw);
		no_os_uart_write(uart_desc, temp_msg, strlen(temp_msg));
	} else {
		char temp_err[] = "Failed to read temperature\n\r";
		no_os_uart_write(uart_desc, temp_err, sizeof(temp_err) - 1);
	}

	/* Set emitter drive if configured (PNP operation) */
	if (cfg->emitter_drive_mv > 0) {
		uint16_t vedrive_raw = (uint16_t)(cfg->emitter_drive_mv /
						  mV_per_lsb);
		ret = cfg->write_dac(dev, cfg->emitter_channel, vedrive_raw);
		if (ret) {
			sprintf(msg_buf,
				"Failed to write to DAC channel %u (Emitter)\n\r",
				cfg->emitter_channel);
			no_os_uart_write(uart_desc, msg_buf, strlen(msg_buf));
			goto cleanup;
		}
		sprintf(msg_buf,
			"Emitter drive (CH%u) set to %u counts (%.2f mV)\n\r",
			cfg->emitter_channel, vedrive_raw,
			vedrive_raw * mV_per_lsb);
		no_os_uart_write(uart_desc, msg_buf, strlen(msg_buf));
	}

	/* Initialize DACs to 500mV */
	dac_value = (uint16_t)(500.0f / mV_per_lsb);

	if (cfg->verbose_init) {
		sprintf(msg_buf,
			"Initializing Base/Collector DACs to %u counts (%.2f mV)\n\r",
			dac_value, dac_value * mV_per_lsb);
		no_os_uart_write(uart_desc, msg_buf, strlen(msg_buf));
	}

	ret = cfg->write_dac(dev, 0, dac_value);
	if (ret) {
		char msg_err[] = "Failed to write to DAC channel 0\n\r";
		no_os_uart_write(uart_desc, msg_err, sizeof(msg_err) - 1);
		goto cleanup;
	}

	if (cfg->verbose_init) {
		char msg_dac0[] = "DAC channel 0 written successfully\n\r";
		no_os_uart_write(uart_desc, msg_dac0, sizeof(msg_dac0) - 1);
	}

	ret = cfg->write_dac(dev, 2, dac_value);
	if (ret) {
		char msg_err[] = "Failed to write to DAC channel 2\n\r";
		no_os_uart_write(uart_desc, msg_err, sizeof(msg_err) - 1);
		goto cleanup;
	}

	if (cfg->verbose_init) {
		char msg_dac2[] = "DAC channel 2 written successfully\n\r";
		no_os_uart_write(uart_desc, msg_dac2, sizeof(msg_dac2) - 1);

		/* Verify by reading back channel 2 (DAC+ADC mode) */
		uint16_t readback_raw;
		no_os_mdelay(10);
		ret = cfg->read_adc(dev, 2, &readback_raw);
		if (ret == 0) {
			readback_raw &= 0x0FFF;
			float readback_v = (readback_raw * mV_per_lsb) /
					   1000.0f;
			sprintf(msg_buf,
				"DAC CH2 readback: raw=%u, voltage=%.4fV\n\r",
				readback_raw, readback_v);
			no_os_uart_write(uart_desc, msg_buf, strlen(msg_buf));
		}
	}

	/* DAC test sweep if enabled */
	if (cfg->dac_test_sweep)
		dac_test_sweep_run(uart_desc, cfg, dev, mV_per_lsb);

	/* Arrays to store curve data */
	float curve_vcs[NUM_CURVES][NUM_POINTS];
	float curve_ics[NUM_CURVES][NUM_POINTS];
	int curve_idx = 0;

	memset(curve_vcs, 0, sizeof(curve_vcs));
	memset(curve_ics, 0, sizeof(curve_ics));

	if (cfg->reverse_fill) {
		sprintf(msg_buf,
			"\n\rStarting %s curve sweep (sweeping HIGH to LOW)...\n\r",
			cfg->bjt_label);
	} else {
		sprintf(msg_buf, "\n\rStarting curve sweep...\n\r");
	}
	no_os_uart_write(uart_desc, msg_buf, strlen(msg_buf));

	/* Sweep base voltage */
	for (int ci = 0; ci < cfg->num_curves; ci++) {

		int vb_mv = cfg->vb_start_mv + ci * cfg->vb_step_mv;
		uint16_t vbdrive_raw = (uint16_t)((float)vb_mv / mV_per_lsb);

		ret = cfg->write_dac(dev, 0, vbdrive_raw);
		if (ret) {
			char msg_err[] = "Failed to write to DAC channel 0\n\r";
			no_os_uart_write(uart_desc, msg_err,
					 sizeof(msg_err) - 1);
			goto cleanup;
		}

		/* Allow base current to settle */
		no_os_mdelay(50);

		float vb_voltage = (vbdrive_raw * mV_per_lsb) / 1000.0f;
		ib = (vb_voltage - Vbe) / Rbase;

		char curveBuffer[128];
		float parsed_ib = ib * 1e6;
		sprintf(curveBuffer,
			"Base Drive:  %.11f  Volts,  %.10f  uA\n\r",
			vb_voltage, parsed_ib);
		no_os_uart_write(uart_desc, curveBuffer, strlen(curveBuffer));

		/* Sweep collector voltage */
		for (int pi = 0; pi < NUM_POINTS; pi++) {

			int vc_mv = cfg->vc_start_mv + pi * cfg->vc_step_mv;
			int point_idx = cfg->reverse_fill ?
					(NUM_POINTS - 1 - pi) : pi;
			uint16_t vcdrive_raw = (uint16_t)((float)vc_mv /
							  mV_per_lsb);

			ret = cfg->write_dac(dev, 2, vcdrive_raw);
			if (ret) {
				char msg_err[] =
					"Failed to write to DAC channel 2\n\r";
				no_os_uart_write(uart_desc, msg_err,
						 sizeof(msg_err) - 1);
				goto cleanup;
			}

			/* Small delay for settling */
			no_os_mdelay(10);

			/* Read ADC channels */
			ret = cfg->read_adc(dev, 1, &Vcsense_raw);
			if (ret) {
				char msg_err[] =
					"Failed to read ADC channel 1\n\r";
				no_os_uart_write(uart_desc, msg_err,
						 sizeof(msg_err) - 1);
				goto cleanup;
			}

			ret = cfg->read_adc(dev, 2, &Vcdrive_meas_raw);
			if (ret) {
				char msg_err[] =
					"Failed to read ADC channel 2\n\r";
				no_os_uart_write(uart_desc, msg_err,
						 sizeof(msg_err) - 1);
				goto cleanup;
			}

			/* Mask to 12-bit values */
			Vcsense_raw &= 0x0FFF;
			Vcdrive_meas_raw &= 0x0FFF;

			/* Calculate collector current (mV / ohm = mA) */
			ic = ((float)(Vcdrive_meas_raw - Vcsense_raw) *
			      mV_per_lsb) / Rsense;

			/* Collector voltage in volts */
			vc = (Vcsense_raw * mV_per_lsb) / 1000.0f;

			curve_vcs[curve_idx][point_idx] = vc;
			curve_ics[curve_idx][point_idx] = ic;

			char curveBuffer2[128];
			sprintf(curveBuffer2,
				"coll voltage:  %.11f   coll curre:  %.16f\n\r",
				vc, ic);
			no_os_uart_write(uart_desc, curveBuffer2,
					 strlen(curveBuffer2));
		}

		char msg_curve_done[] = "\n\r";
		no_os_uart_write(uart_desc, msg_curve_done,
				 sizeof(msg_curve_done) - 1);

		curve_idx++;
	}

	/* Plot ASCII graph */
	plot_ascii_graph(uart_desc, cfg, curve_vcs, curve_ics);

	/* Output CSV data */
	output_csv_data(uart_desc, cfg, curve_vcs, curve_ics);

	no_os_mdelay(1000);

cleanup:
	if (dev)
		cfg->remove(dev);

	return ret;
}

int ad5592r_curve_example(struct no_os_uart_desc *uart_desc)
{
	struct curve_trace_cfg cfg = {
		.init = ad5592r_init,
		.remove = ad5592r_remove,
		.write_dac = ad5592r_write_dac,
		.read_adc = ad5592r_read_adc,
		.dev_ip = DEFINE_AD5592R_IP(),
		.device_name = "AD5592R",
		.bus_label = "SPI",
		.bjt_label = "NPN",
		.csv_tag = "AD5592R_NPN",
		.vb_start_mv = 499,
		.vb_step_mv = 500,
		.num_curves = NUM_CURVES,
		.vc_start_mv = 0,
		.vc_step_mv = 50,
		.csv_curves = NUM_CURVES,
		.reverse_fill = false,
		.abs_current_plot = false,
		.invert_y = true,
		.emitter_drive_mv = 0,
		.emitter_channel = 0,
		.dac_test_sweep = false,
		.verbose_init = false,
	};

	return curve_trace_common(uart_desc, &cfg);
}

int ad5593r_curve_example(struct no_os_uart_desc *uart_desc)
{
	struct curve_trace_cfg cfg = {
		.init = ad5593r_init,
		.remove = ad5593r_remove,
		.write_dac = ad5593r_write_dac,
		.read_adc = ad5593r_read_adc,
		.dev_ip = DEFINE_AD5593R_IP(),
		.device_name = "AD5593R",
		.bus_label = "I2C",
		.bjt_label = "PNP",
		.csv_tag = "AD5593R_PNP",
		.vb_start_mv = 2000,
		.vb_step_mv = -500,
		.num_curves = NUM_CURVES - 1,
		.vc_start_mv = 2499,
		.vc_step_mv = -50,
		.csv_curves = NUM_CURVES - 1,
		.reverse_fill = true,
		.abs_current_plot = true,
		.invert_y = false,
		.emitter_drive_mv = 2499,
		.emitter_channel = 3,
		.dac_test_sweep = true,
		.verbose_init = true,
	};

	return curve_trace_common(uart_desc, &cfg);
}

int lm75_example(struct no_os_uart_desc *uart_desc)
{
	struct lm75_dev *lm75 = NULL;
	struct lm75_init_param lm75_ip = {
		.fault_count = 0, /* POR state */
		.os_polarity = lm75_os_active_low,
		.i2c_ip = &lm75_i2c_ip,
	};
	int ret;
	uint32_t temp_raw;
	uint32_t temp;


	ret = lm75_init(&lm75, &lm75_ip);
	if (ret) {
		char msg_err[] = "Failed to read LM75\n\r";
		no_os_uart_write(uart_desc, msg_err, sizeof(msg_err) - 1);
		goto cleanup;
	}

	no_os_mdelay(1000);
	for (int m = 0; m < 100; m++) {

		ret = lm75_read_temperature(lm75, lm75_die_temperature, &temp_raw);
		if (ret) {
			char msg_err[] = "Failed to read LM75\n\r";
			no_os_uart_write(uart_desc, msg_err, sizeof(msg_err) - 1);
			goto cleanup;
		}
		temp = lm75_raw_to_millicelsius(temp_raw); // <- for conversion
		char temp_msg[64];
		sprintf(temp_msg, "LM75 Temperature: %d.%d°C (raw=%u)\n\r", temp / 1000,
			temp % 1000, temp_raw);
		no_os_uart_write(uart_desc, temp_msg, strlen(temp_msg));
		no_os_mdelay(100);
	}
	char csv_footer[] = "=== LM75 TEST END ===\n\r\n\r";
	no_os_uart_write(uart_desc, csv_footer, sizeof(csv_footer) - 1);

	no_os_mdelay(1000);

cleanup:
	if (lm75)
		lm75_remove(lm75);

	return ret;
}

int curvetrace_example(void)
{
	struct no_os_uart_desc *uart_desc = NULL;
	int ret;

	/* Initialize UART for logging */
	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret) {
		return ret;
	}

	/* Add delay to ensure UART connection is fully established */
	no_os_mdelay(2000);

	char msg_clear[] = "\e[2J\e[H";
	no_os_uart_write(uart_desc, msg_clear, sizeof(msg_clear) - 1);

	char msg_header[] = "========================================\n\r";
	no_os_uart_write(uart_desc, msg_header, sizeof(msg_header) - 1);
	char msg_title[] = "  Dual Device BJT Curve Tracer Demo\n\r";
	no_os_uart_write(uart_desc, msg_title, sizeof(msg_title) - 1);
	no_os_uart_write(uart_desc, msg_header, sizeof(msg_header) - 1);

	/* Run AD5592R curve tracer (SPI) */
	ret = ad5592r_curve_example(uart_desc);
	if (ret) {
		char msg_err[] = "\n\rAD5592R curve tracer failed!\n\r";
		no_os_uart_write(uart_desc, msg_err, sizeof(msg_err) - 1);
		goto cleanup;
	}

	/* Add delay between tests */
	no_os_mdelay(2000);

	/* Run AD5593R curve tracer (I2C) */
	ret = ad5593r_curve_example(uart_desc);
	if (ret) {
		char msg_err[] = "\n\rAD5593R curve tracer failed!\n\r";
		no_os_uart_write(uart_desc, msg_err, sizeof(msg_err) - 1);
		goto cleanup;
	}

	/* Add delay between tests */
	no_os_mdelay(2000);

	/* Run AD5593R curve tracer (I2C) */
	ret = lm75_example(uart_desc);
	if (ret) {
		char msg_err[] = "\n\rLM75 failed!\n\r";
		no_os_uart_write(uart_desc, msg_err, sizeof(msg_err) - 1);
		goto cleanup;
	}

cleanup:
	if (uart_desc)
		no_os_uart_remove(uart_desc);

	return ret;
}

example_main("curvetrace_example");