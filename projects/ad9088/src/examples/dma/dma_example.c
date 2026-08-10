/***************************************************************************//**
 *   @file   dma_example.c
 *   @brief  DMA capture example for the ad9088 project
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
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

#include "dma_example.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "ad9088.h"
#include "adi_apollo_cddc.h"
#include "adi_apollo_fddc.h"
#include "adi_apollo_cduc.h"
#include "adi_apollo_fduc.h"
#include "jesd204.h"
#include "axi_adxcvr.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"
#include "no_os_axi_io.h"
#include "jesd204_clk.h"
#include "parameters.h"
#include "xil_cache.h"

/* Side and datapath the test tone is injected on */
#define TEST_TONE_SIDE		0
#define TEST_TONE_FDDC		0
/* Mid-scale test tone amplitude, within the RX FNCO 0x1FFF limit */
#define TEST_TONE_OFFSET	0x1000

/*
 * Test tone frequency as a divisor of the capture rate rather than an absolute
 * value, so it stays inside the FDDC passband whatever the profile decimates by.
 * A power of two no larger than 256 keeps the tone an integer number of Hz and
 * its phase step an exact multiple of the table stride, so the accumulator
 * lands on a table entry every sample rather than between two of them. At 256
 * it advances exactly two entries.
 */
#define TEST_TONE_RATE_DIV	256

/*
 * Default coarse NCO, as a divisor of the DAC rate. The fractional part of the
 * frequency tuning word is discarded unless dual modulus mode is on
 * (ad9088.c:64-66), so only frequencies that divide the DAC rate exactly are
 * tuned without residual error -- a power of two guarantees it.
 */
#define DEFAULT_CNCO_RATE_DIV	8
#define DEFAULT_FNCO_HZ		0

/*
 * Converter pair the FDDC under test lands on. The FDDC emits a complex tone,
 * so its I and Q arrive on two adjacent converters of the JESD204 link.
 */
#define TONE_CONV_I		0
#define TONE_CONV_Q		1

/* Number of I/Q pairs printed per converter when dumping a capture */
#define DUMP_SAMPLES		16

/*
 * Half scale for the transmitted tone. The first passing run measured the tone
 * coming back at 3.5% of ADC full scale, 17 dB down, so the receiver has ample
 * headroom and the earlier quarter-scale setting only cost signal-to-noise --
 * which the envelope spread, a min/max statistic, is the first thing to feel.
 */
#define LOOPBACK_TX_AMPLITUDE	16384

/*
 * The TX data offload replays its whole BRAM regardless of how much was written
 * into it, so anything left unwritten comes back as noise. Sized from
 * mem-size-log2 = 19 and cross-checked at runtime against MEMORY_SIZE_LSB.
 */
#define TX_OFFLOAD_BRAM_BYTES		(512 * 1024)
#define AXI_DO_REG_MEMORY_SIZE_LSB	0x0014

/*
 * AXI data path width of the TX DMAC (dma-data-width-src = 1024). The driver
 * rejects a source address that is not a multiple of it; the transfer size is
 * floored to the same boundary so the last beat is whole.
 */
#define DMA_SRC_WIDTH_BYTES	128

/* Largest M the JESD204 link can report (ADI_APOLLO_CONV_PER_LINK_16) */
#define MAX_LINK_CONVERTERS	16

#define DMA_BUFFER_ALIGN	1024

/*
 * Coherence is carried in per-mille to keep the whole estimator in integers,
 * and printed as a percentage with one decimal. A tone sitting exactly where
 * predicted scores ~1000; noise or a tone at any other frequency collapses
 * toward 1000/N.
 */
#define COHERENCE_SCALE		1000
#define COHERENCE_PASS		900

/*
 * Per-mille printed as a percentage with one decimal, which it divides into
 * exactly. Two arguments against a "%lu.%lu%%" format, since there is no
 * floating point in this build.
 */
#define PCT_WHOLE(x)		((unsigned long)(x) / 10)
#define PCT_TENTH(x)		((unsigned long)(x) % 10)

/*
 * Envelope of a single complex tone is constant, so the spread of I^2+Q^2 is a
 * shape check independent of frequency. It is a min/max extreme statistic over
 * the whole capture though, so noise dominates it: the measured floor alone puts
 * it near 40 per-mille and doubling the noise doubles that, while coherence
 * stays at full scale. Kept loose enough to catch a grossly non-constant
 * envelope without failing on noise, since coherence is the stricter test.
 */
#define ENVELOPE_SPREAD_MAX	250

/*
 * sin(2*pi*i/512) in Q15: the stored value is the fraction v/32768, so full
 * scale is 32767 rather than 1.0 and every product needs >> SIN_TABLE_SHIFT to
 * come back to the scale of the input samples. Signed, so a lookup is a mask
 * and an index with no offset to undo. cos is the same table a quarter period
 * earlier -- one add rather than a second table.
 */
#define SIN_TABLE_LEN		512
#define SIN_TABLE_QUARTER	(SIN_TABLE_LEN / 4)
#define SIN_TABLE_MASK		(SIN_TABLE_LEN - 1)
#define SIN_TABLE_SHIFT		15

/* Phase is a 32-bit accumulator, so the top 9 bits index the table. */
#define SIN_TABLE_PHASE_SHIFT	23

/* Peak of a 16-bit signed converter sample, to report a level in context. */
#define SAMPLE_FULL_SCALE	32767

static const int16_t sin_table_q15[SIN_TABLE_LEN] = {
	0,    402,    804,   1206,   1608,   2009,   2410,   2811,
	3212,   3612,   4011,   4410,   4808,   5205,   5602,   5998,
	6393,   6786,   7179,   7571,   7962,   8351,   8739,   9126,
	9512,   9896,  10278,  10659,  11039,  11417,  11793,  12167,
	12539,  12910,  13279,  13645,  14010,  14372,  14732,  15090,
	15446,  15800,  16151,  16499,  16846,  17189,  17530,  17869,
	18204,  18537,  18868,  19195,  19519,  19841,  20159,  20475,
	20787,  21096,  21403,  21705,  22005,  22301,  22594,  22884,
	23170,  23452,  23731,  24007,  24279,  24547,  24811,  25072,
	25329,  25582,  25832,  26077,  26319,  26556,  26790,  27019,
	27245,  27466,  27683,  27896,  28105,  28310,  28510,  28706,
	28898,  29085,  29268,  29447,  29621,  29791,  29956,  30117,
	30273,  30424,  30571,  30714,  30852,  30985,  31113,  31237,
	31356,  31470,  31580,  31685,  31785,  31880,  31971,  32057,
	32137,  32213,  32285,  32351,  32412,  32469,  32521,  32567,
	32609,  32646,  32678,  32705,  32728,  32745,  32757,  32765,
	32767,  32765,  32757,  32745,  32728,  32705,  32678,  32646,
	32609,  32567,  32521,  32469,  32412,  32351,  32285,  32213,
	32137,  32057,  31971,  31880,  31785,  31685,  31580,  31470,
	31356,  31237,  31113,  30985,  30852,  30714,  30571,  30424,
	30273,  30117,  29956,  29791,  29621,  29447,  29268,  29085,
	28898,  28706,  28510,  28310,  28105,  27896,  27683,  27466,
	27245,  27019,  26790,  26556,  26319,  26077,  25832,  25582,
	25329,  25072,  24811,  24547,  24279,  24007,  23731,  23452,
	23170,  22884,  22594,  22301,  22005,  21705,  21403,  21096,
	20787,  20475,  20159,  19841,  19519,  19195,  18868,  18537,
	18204,  17869,  17530,  17189,  16846,  16499,  16151,  15800,
	15446,  15090,  14732,  14372,  14010,  13645,  13279,  12910,
	12539,  12167,  11793,  11417,  11039,  10659,  10278,   9896,
	9512,   9126,   8739,   8351,   7962,   7571,   7179,   6786,
	6393,   5998,   5602,   5205,   4808,   4410,   4011,   3612,
	3212,   2811,   2410,   2009,   1608,   1206,    804,    402,
	0,   -402,   -804,  -1206,  -1608,  -2009,  -2410,  -2811,
	-3212,  -3612,  -4011,  -4410,  -4808,  -5205,  -5602,  -5998,
	-6393,  -6786,  -7179,  -7571,  -7962,  -8351,  -8739,  -9126,
	-9512,  -9896, -10278, -10659, -11039, -11417, -11793, -12167,
	-12539, -12910, -13279, -13645, -14010, -14372, -14732, -15090,
	-15446, -15800, -16151, -16499, -16846, -17189, -17530, -17869,
	-18204, -18537, -18868, -19195, -19519, -19841, -20159, -20475,
	-20787, -21096, -21403, -21705, -22005, -22301, -22594, -22884,
	-23170, -23452, -23731, -24007, -24279, -24547, -24811, -25072,
	-25329, -25582, -25832, -26077, -26319, -26556, -26790, -27019,
	-27245, -27466, -27683, -27896, -28105, -28310, -28510, -28706,
	-28898, -29085, -29268, -29447, -29621, -29791, -29956, -30117,
	-30273, -30424, -30571, -30714, -30852, -30985, -31113, -31237,
	-31356, -31470, -31580, -31685, -31785, -31880, -31971, -32057,
	-32137, -32213, -32285, -32351, -32412, -32469, -32521, -32567,
	-32609, -32646, -32678, -32705, -32728, -32745, -32757, -32765,
	-32767, -32765, -32757, -32745, -32728, -32705, -32678, -32646,
	-32609, -32567, -32521, -32469, -32412, -32351, -32285, -32213,
	-32137, -32057, -31971, -31880, -31785, -31685, -31580, -31470,
	-31356, -31237, -31113, -30985, -30852, -30714, -30571, -30424,
	-30273, -30117, -29956, -29791, -29621, -29447, -29268, -29085,
	-28898, -28706, -28510, -28310, -28105, -27896, -27683, -27466,
	-27245, -27019, -26790, -26556, -26319, -26077, -25832, -25582,
	-25329, -25072, -24811, -24547, -24279, -24007, -23731, -23452,
	-23170, -22884, -22594, -22301, -22005, -21705, -21403, -21096,
	-20787, -20475, -20159, -19841, -19519, -19195, -18868, -18537,
	-18204, -17869, -17530, -17189, -16846, -16499, -16151, -15800,
	-15446, -15090, -14732, -14372, -14010, -13645, -13279, -12910,
	-12539, -12167, -11793, -11417, -11039, -10659, -10278,  -9896,
	-9512,  -9126,  -8739,  -8351,  -7962,  -7571,  -7179,  -6786,
	-6393,  -5998,  -5602,  -5205,  -4808,  -4410,  -4011,  -3612,
	-3212,  -2811,  -2410,  -2009,  -1608,  -1206,   -804,   -402
};

static inline int32_t dma_example_sin_q15(uint32_t idx)
{
	return sin_table_q15[idx & SIN_TABLE_MASK];
}

static inline int32_t dma_example_cos_q15(uint32_t idx)
{
	return dma_example_sin_q15(idx + SIN_TABLE_QUARTER);
}

/*
 * Fixed-capacity static capture buffer. Samples-per-converter is derived from
 * the link's M at runtime and clamped to what fits here, so a profile with more
 * converters shortens the capture instead of overrunning the buffer.
 */
static uint16_t adc_buffer_dma[ADC_BUFFER_SAMPLES * 8]
__attribute__((aligned(DMA_BUFFER_ALIGN)));

/* Sized to the whole TX offload BRAM, see TX_OFFLOAD_BRAM_BYTES. */
static uint16_t dac_buffer_dma[TX_OFFLOAD_BRAM_BYTES / sizeof(uint16_t)]
__attribute__((aligned(DMA_BUFFER_ALIGN)));

/**
 * @brief Phase step the tone generator and the estimator both derive.
 *
 * Shared so a transmitted frequency and the frequency it is scored at cannot
 * disagree about what a given rate means.
 *
 * @param freq_hz - Frequency, may be negative.
 * @param rate_hz - Sample rate in Hz.
 * @return Step in 32-bit phase turns per sample.
 */
static uint32_t dma_example_phase_step(int64_t freq_hz, uint64_t rate_hz)
{
	if (freq_hz < 0)
		return (uint32_t) - (int32_t)no_os_div_u64(
			       (uint64_t)(-freq_hz) << 32, rate_hz);

	return (uint32_t)no_os_div_u64((uint64_t)freq_hz << 32, rate_hz);
}

/**
 * @brief Integer square root of a 64-bit value.
 */
static uint64_t dma_example_sqrt64(uint64_t v)
{
	uint64_t rem = 0;
	uint64_t root = 0;
	int i;

	for (i = 0; i < 32; i++) {
		root <<= 1;
		rem = (rem << 2) | (v >> 62);
		v <<= 2;
		if (root < rem) {
			rem -= root | 1;
			root += 2;
		}
	}

	return root >> 1;
}

/**
 * @brief Coherence of a capture against a complex tone at the expected offset.
 *
 * Correlates z[n] = I[n] + jQ[n] against exp(-jwn) and normalises by the total
 * captured power:
 *
 *   coherence = |sum z[n] exp(-jwn)|^2 / (N * sum |z[n]|^2)
 *
 * The result is ~COHERENCE_SCALE when all the captured energy sits exactly at
 * the expected frequency and ~COHERENCE_SCALE/N for noise or a tone anywhere
 * else, so it answers "is this the tone we asked for" rather than merely "is
 * something there". It is signed through the phase increment: a negative
 * expected frequency only correlates with a negative-rotating input, which is
 * what makes a swapped I/Q pair or a spectrally inverted datapath visible.
 *
 * @param buf - Capture buffer holding interleaved converter samples.
 * @param samples - Samples captured per converter.
 * @param num_conv - Number of interleaved converters.
 * @param freq_hz - Expected baseband rotation rate, signed, in Hz. This is the
 *		   rate the samples are expected to rotate at, not the frequency
 *		   programmed into any mixer -- see the sign note at the capture
 *		   call site.
 * @param rate_hz - Capture sample rate in Hz.
 * @param amplitude - Returns the mean tone envelope in LSB, may be NULL.
 * @param spread - Returns the envelope spread in per-mille, may be NULL.
 * @return Coherence in per-mille.
 */
static uint32_t dma_example_coherence(const uint16_t *buf, uint32_t samples,
				      uint8_t num_conv, int64_t freq_hz,
				      uint64_t rate_hz, uint32_t *amplitude,
				      uint32_t *spread)
{
	uint64_t env_min = UINT64_MAX;
	uint64_t env_max = 0;
	uint64_t env_sum = 0;
	int64_t acc_re = 0;
	int64_t acc_im = 0;
	uint64_t power = 0;
	uint32_t phase = 0;
	uint32_t step;
	int32_t ref_re;
	int32_t ref_im;
	int32_t s_i;
	int32_t s_q;
	uint32_t idx;
	uint64_t env;
	uint32_t rms_max;
	uint32_t rms_min;
	int64_t mean_re;
	int64_t mean_im;
	uint64_t mag;
	uint64_t norm;
	uint32_t i;

	/*
	 * Phase increment in 32-bit turns. Negative frequencies wrap to the top
	 * half of the accumulator, which is exactly the conjugate reference.
	 */
	step = dma_example_phase_step(freq_hz, rate_hz);

	for (i = 0; i < samples; i++) {
		s_i = (int16_t)buf[i * num_conv + TONE_CONV_I];
		s_q = (int16_t)buf[i * num_conv + TONE_CONV_Q];

		/*
		 * Table lookup rather than an iterated rotation: the phase
		 * accumulator cannot drift over a long capture, where a
		 * recurrence would accumulate rounding error. Negating the sine
		 * gives the conjugate reference exp(-jwn).
		 */
		idx = phase >> SIN_TABLE_PHASE_SHIFT;
		ref_re = dma_example_cos_q15(idx);
		ref_im = -dma_example_sin_q15(idx);

		acc_re += ((int64_t)s_i * ref_re - (int64_t)s_q * ref_im)
			  >> SIN_TABLE_SHIFT;
		acc_im += ((int64_t)s_i * ref_im + (int64_t)s_q * ref_re)
			  >> SIN_TABLE_SHIFT;

		env = (uint64_t)((int64_t)s_i * s_i + (int64_t)s_q * s_q);
		power += env;
		env_sum += env;
		if (env < env_min)
			env_min = env;
		if (env > env_max)
			env_max = env;

		phase += step;
	}

	if (amplitude)
		*amplitude = samples ?
			     (uint32_t)dma_example_sqrt64(no_os_div_u64(env_sum,
					     samples)) : 0;

	if (spread) {
		rms_max = (uint32_t)dma_example_sqrt64(env_max);
		rms_min = (uint32_t)dma_example_sqrt64(env_min);

		*spread = rms_max ?
			  (uint32_t)(((uint64_t)(rms_max - rms_min) *
				      COHERENCE_SCALE) / rms_max) : 0;
	}

	if (!power || !samples)
		return 0;

	/*
	 * |sum| reaches ~2^42 over a long capture, so squaring it directly would
	 * overflow. Divide both sides of the ratio by N first, which cancels the
	 * N in the denominator and leaves two per-sample quantities: the
	 * correlated amplitude and the mean power, each bounded by full scale.
	 *
	 *   |sum z exp(-jwn)|^2 / (N * sum|z|^2)
	 *     = |sum/N|^2 / (sum|z|^2 / N)
	 */
	mean_re = no_os_div_s64(acc_re, (int32_t)samples);
	mean_im = no_os_div_s64(acc_im, (int32_t)samples);

	mag = (uint64_t)(mean_re * mean_re + mean_im * mean_im);
	norm = no_os_div_u64(power, samples);
	if (!norm)
		return 0;

	return (uint32_t)no_os_div_u64(mag * COHERENCE_SCALE, norm);
}

/**
 * @brief Capture one buffer from the RX DMAC into DDR.
 * @param rx_dmac - RX DMA controller.
 * @param size - Transfer size in bytes.
 * @return 0 on success, negative error code otherwise.
 */
static int dma_example_capture(struct axi_dmac *rx_dmac, uint32_t size)
{
	struct axi_dma_transfer read_transfer = {
		.size = size,
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = 0,
		.dest_addr = (uintptr_t)adc_buffer_dma,
	};
	int ret;

	ret = axi_dmac_transfer_start(rx_dmac, &read_transfer);
	if (ret) {
		pr_err("RX DMA transfer start failed (%d)\n", ret);
		return ret;
	}

	ret = axi_dmac_transfer_wait_completion(rx_dmac, 1000);
	if (ret) {
		pr_err("RX DMA transfer timed out (%d)\n", ret);
		return ret;
	}

	/*
	 * main() enables the data cache unconditionally, so the CPU would
	 * otherwise read stale lines instead of what the DMA just wrote.
	 */
	Xil_DCacheInvalidateRange((uintptr_t)adc_buffer_dma, size);

	return 0;
}

/**
 * @brief Derive the rate the captured samples arrive at.
 *
 * The FNCO mixes at the CDDC output rate and the FDDC decimates after it, so the
 * captured rate carries both ratios. The decimations are read back from the
 * profile the JESD204 FSM populated rather than assuming a value, so a profile
 * change cannot silently shift every predicted frequency.
 *
 * @param phy - AD9088 device.
 * @param rx_adc - RX TPL core, used only to cross-check the derived rate.
 * @param capture_rate - Returns the FDDC output rate in Hz.
 * @return 0 on success, negative error code otherwise.
 */
static int dma_example_get_capture_rate(struct ad9088_phy *phy,
					struct axi_adc *rx_adc,
					uint64_t *capture_rate)
{
	adi_apollo_rxpath_t *rx_path = &phy->profile.rx_path[TEST_TONE_SIDE];
	uint32_t cddc_dcm;
	uint32_t fddc_dcm;
	uint8_t cddc_pi;
	uint8_t fddc_pi;
	uint64_t adc_rate;
	uint64_t delta;
	int ret;

	cddc_pi = (TEST_TONE_FDDC / 2) % ADI_APOLLO_CDUCS_PER_SIDE;
	ret = adi_apollo_cddc_dcm_bf_to_val(&phy->ad9088,
					    rx_path->rx_cddc[cddc_pi].drc_ratio,
					    &cddc_dcm);
	if (ret) {
		pr_err("Reading the CDDC decimation failed (%d)\n", ret);
		return ret;
	}

	fddc_pi = TEST_TONE_FDDC % ADI_APOLLO_FDUCS_PER_SIDE;
	ret = adi_apollo_fddc_dcm_bf_to_val(&phy->ad9088,
					    rx_path->rx_fddc[fddc_pi].drc_ratio,
					    &fddc_dcm);
	if (ret) {
		pr_err("Reading the FDDC decimation failed (%d)\n", ret);
		return ret;
	}

	if (!cddc_dcm || !fddc_dcm) {
		pr_err("Invalid decimation CDDC=%lu FDDC=%lu\n",
		       (unsigned long)cddc_dcm, (unsigned long)fddc_dcm);
		return -EINVAL;
	}

	adc_rate = phy->profile.adc_cfg[TEST_TONE_SIDE].adc_sampling_rate_Hz;
	*capture_rate = no_os_div_u64(adc_rate, cddc_dcm * fddc_dcm);

	if (!*capture_rate) {
		pr_err("Invalid capture rate\n");
		return -EINVAL;
	}

	pr_info("Profile Rates: ADC %lu kHz / CDDC %lu / FDDC %lu = capture %lu kHz\n",
		(unsigned long)no_os_div_u64(adc_rate, 1000),
		(unsigned long)cddc_dcm, (unsigned long)fddc_dcm,
		(unsigned long)no_os_div_u64(*capture_rate, 1000));

	/*
	 * The TPL core derives the same rate from its own clock registers, so a
	 * mismatch means the decimation model above is wrong and every predicted
	 * frequency would be off by that factor.
	 */
	if (rx_adc->clock_hz) {
		delta = rx_adc->clock_hz > *capture_rate ?
			rx_adc->clock_hz - *capture_rate :
			*capture_rate - rx_adc->clock_hz;

		pr_info("TPL core reports %lu kHz\n",
			(unsigned long)no_os_div_u64(rx_adc->clock_hz, 1000));

		if (no_os_div_u64(delta * 100, *capture_rate) > 5)
			pr_info("Warning: TPL rate disagrees with the profile "
				"by more than 5%%\n");
	}

	pr_info("\n");
	return 0;
}

/**
 * @brief Derive the rate the transmitted samples leave the DMA at.
 *
 * The mirror of dma_example_get_capture_rate() for the transmit direction. The
 * interpolation enums are encoded differently from the decimation ones, so the
 * CDUC/FDUC converters must be used rather than the CDDC/FDDC pair, otherwise
 * every transmitted frequency lands off by that ratio.
 *
 * @param phy - AD9088 device.
 * @param tx_dac - TX TPL core, used only to cross-check the derived rate.
 * @param tx_rate - Returns the FDUC input rate in Hz.
 * @return 0 on success, negative error code otherwise.
 */
static int dma_example_get_tx_rate(struct ad9088_phy *phy,
				   struct axi_dac *tx_dac, uint64_t *tx_rate)
{
	adi_apollo_txpath_t *tx_path = &phy->profile.tx_path[TEST_TONE_SIDE];
	uint32_t cduc_int;
	uint32_t fduc_int;
	uint8_t cduc_pi;
	uint8_t fduc_pi;
	uint64_t dac_rate;
	uint64_t delta;
	int ret;

	cduc_pi = (TEST_TONE_FDDC / 2) % ADI_APOLLO_CDUCS_PER_SIDE;
	ret = adi_apollo_cduc_interp_bf_to_val(&phy->ad9088,
					       tx_path->tx_cduc[cduc_pi].drc_ratio,
					       &cduc_int);
	if (ret) {
		pr_err("Reading the CDUC interpolation failed (%d)\n", ret);
		return ret;
	}

	fduc_pi = TEST_TONE_FDDC % ADI_APOLLO_FDUCS_PER_SIDE;
	ret = adi_apollo_fduc_interp_bf_to_val(&phy->ad9088,
					       tx_path->tx_fduc[fduc_pi].drc_ratio,
					       &fduc_int);
	if (ret) {
		pr_err("Reading the FDUC interpolation failed (%d)\n", ret);
		return ret;
	}

	if (!cduc_int || !fduc_int) {
		pr_err("Invalid interpolation CDUC=%lu FDUC=%lu\n",
		       (unsigned long)cduc_int, (unsigned long)fduc_int);
		return -EINVAL;
	}

	dac_rate = phy->profile.dac_cfg[TEST_TONE_SIDE].dac_sampling_rate_Hz;
	*tx_rate = no_os_div_u64(dac_rate, cduc_int * fduc_int);

	if (!*tx_rate) {
		pr_err("Invalid transmit rate\n");
		return -EINVAL;
	}

	pr_info("Profile Rates: DAC %lu kHz / CDUC %lu / FDUC %lu = "
		"transmit %lu kHz\n",
		(unsigned long)no_os_div_u64(dac_rate, 1000),
		(unsigned long)cduc_int, (unsigned long)fduc_int,
		(unsigned long)no_os_div_u64(*tx_rate, 1000));

	if (tx_dac->clock_hz) {
		delta = tx_dac->clock_hz > *tx_rate ?
			tx_dac->clock_hz - *tx_rate :
			*tx_rate - tx_dac->clock_hz;

		pr_info("TPL core reports %lu kHz\n",
			(unsigned long)no_os_div_u64(tx_dac->clock_hz, 1000));

		if (no_os_div_u64(delta * 100, *tx_rate) > 5)
			pr_info("Warning: TPL rate disagrees with the profile "
				"by more than 5%%\n");
	}

	return 0;
}

/**
 * @brief Put both sides' NCOs on a known default frequency.
 *
 * A CDUC/FDUC upconverts and a CDDC/FDDC downconverts, so a tone written at
 * f_lut comes back at f_lut + (tx shifts) - (rx shifts). The profile places the
 * two sides 1.4 GHz apart, which puts the received tone outside the FDDC
 * passband whatever it was transmitted at. Tuning both to the same frequency
 * cancels the translation, so a tone arrives where it was sent.
 *
 * Idempotent, so it doubles as the undo for anything that retunes an NCO.
 *
 * @param phy - AD9088 device.
 * @return 0 on success, negative error code otherwise.
 */
static int dma_example_set_default_nco(struct ad9088_phy *phy)
{
	uint64_t dac_rate = phy->profile.dac_cfg[TEST_TONE_SIDE]
			    .dac_sampling_rate_Hz;
	int64_t cnco_hz;
	int64_t tx_cnco = 0;
	int64_t tx_fnco = 0;
	int64_t rx_cnco = 0;
	int64_t rx_fnco = 0;
	uint8_t cddc = (TEST_TONE_FDDC / 2) % ADI_APOLLO_CDUCS_PER_SIDE;
	int ret;

	cnco_hz = (int64_t)no_os_div_u64(dac_rate, DEFAULT_CNCO_RATE_DIV);

	ret = ad9088_set_cnco_freq(phy, ADI_APOLLO_TX, TEST_TONE_SIDE, cddc,
				   cnco_hz);
	if (!ret)
		ret = ad9088_set_cnco_freq(phy, ADI_APOLLO_RX, TEST_TONE_SIDE,
					   cddc, cnco_hz);
	if (!ret)
		ret = ad9088_set_fnco_freq(phy, ADI_APOLLO_TX, TEST_TONE_SIDE,
					   TEST_TONE_FDDC, DEFAULT_FNCO_HZ);
	if (!ret)
		ret = ad9088_set_fnco_freq(phy, ADI_APOLLO_RX, TEST_TONE_SIDE,
					   TEST_TONE_FDDC, DEFAULT_FNCO_HZ);
	if (ret) {
		pr_err("Tuning the NCOs failed (%d)\n", ret);
		return ret;
	}

	ret = ad9088_get_cnco_freq(phy, ADI_APOLLO_TX, TEST_TONE_SIDE, cddc,
				   &tx_cnco);
	if (!ret)
		ret = ad9088_get_fnco_freq(phy, ADI_APOLLO_TX, TEST_TONE_SIDE,
					   TEST_TONE_FDDC, &tx_fnco);
	if (!ret)
		ret = ad9088_get_cnco_freq(phy, ADI_APOLLO_RX, TEST_TONE_SIDE,
					   cddc, &rx_cnco);
	if (!ret)
		ret = ad9088_get_fnco_freq(phy, ADI_APOLLO_RX, TEST_TONE_SIDE,
					   TEST_TONE_FDDC, &rx_fnco);
	if (ret) {
		pr_err("Reading back the NCOs failed (%d)\n", ret);
		return ret;
	}

	pr_info("  NCO tx c/f %ld/%ld kHz  rx c/f %ld/%ld kHz  net %ld kHz\n",
		(long)no_os_div_s64(tx_cnco, 1000),
		(long)no_os_div_s64(tx_fnco, 1000),
		(long)no_os_div_s64(rx_cnco, 1000),
		(long)no_os_div_s64(rx_fnco, 1000),
		(long)no_os_div_s64((tx_cnco + tx_fnco) - (rx_cnco + rx_fnco),
				    1000));

	/*
	 * The tuning word drops its fractional part, so a rate that the divisor
	 * does not divide exactly lands a few Hz off. Far too little to move the
	 * tone off its bin, but it should not pass unremarked.
	 */
	if (tx_cnco != cnco_hz || rx_cnco != cnco_hz)
		pr_info("  Warning: CNCO asked %ld Hz, tuned tx %ld rx %ld\n",
			(long)cnco_hz, (long)tx_cnco, (long)rx_cnco);

	if ((tx_cnco + tx_fnco) != (rx_cnco + rx_fnco)) {
		pr_err("The NCOs did not take the default tuning\n");
		return -EIO;
	}

	return 0;
}

/**
 * @brief Fill a transmit buffer with a complex tone.
 *
 * Uses the same phase accumulator and table as dma_example_coherence(), so the
 * transmitted frequency and the probed frequency cannot disagree about what a
 * given rate means. Converters beyond the I/Q pair are zeroed so an unrelated
 * datapath cannot contribute to the capture.
 *
 * @param buf - Destination buffer, num_conv interleaved converters.
 * @param samples - Samples per converter to write.
 * @param num_conv - Converters per sample.
 * @param freq_hz - Tone frequency, may be negative.
 * @param rate_hz - Transmit sample rate in Hz.
 * @param amplitude - Peak amplitude in LSB.
 */
static void dma_example_fill_tone(uint16_t *buf, uint32_t samples,
				  uint8_t num_conv, int64_t freq_hz,
				  uint64_t rate_hz, int32_t amplitude)
{
	uint32_t phase = 0;
	uint32_t step;
	uint32_t idx;
	uint32_t i;
	uint8_t c;

	step = dma_example_phase_step(freq_hz, rate_hz);

	for (i = 0; i < samples; i++) {
		idx = phase >> SIN_TABLE_PHASE_SHIFT;

		for (c = 0; c < num_conv; c++)
			buf[i * num_conv + c] = 0;

		buf[i * num_conv + TONE_CONV_I] = (uint16_t)(int16_t)
						  ((amplitude * dma_example_cos_q15(idx))
						   >> SIN_TABLE_SHIFT);
		buf[i * num_conv + TONE_CONV_Q] = (uint16_t)(int16_t)
						  ((amplitude * dma_example_sin_q15(idx))
						   >> SIN_TABLE_SHIFT);

		phase += step;
	}
}

int dma_example_main()
{
	struct adf4382_dev *adf4382_dev;
	struct hmc7044_dev *hmc7044_dev;
	struct axi_jesd204_rx *rx_jesd;
	struct axi_jesd204_tx *tx_jesd;
	struct adxcvr *rx_adxcvr;
	struct adxcvr *tx_adxcvr;
	struct jesd204_clk rx_jesd_clk = {0};
	struct jesd204_clk tx_jesd_clk = {0};
	struct no_os_clk_desc rx_lane_clk = {0};
	struct no_os_clk_desc tx_lane_clk = {0};
	struct axi_dmac *rx_dmac;
	struct axi_dmac *tx_dmac;
	struct ad9088_phy *ad9088_phy;
	struct axi_adc *rx_adc;
	struct axi_dac *tx_dac;
	uint32_t samples_per_conv;
	uint32_t transfer_size;
	uint8_t num_conv;
	uint8_t np;
	int64_t tone_hz;
	uint64_t capture_rate;
	uint32_t coherence;
	uint32_t envelope_spread;
	uint32_t amplitude;
	uint32_t floor_coherence;
	bool tone_pass;
	bool floor_pass;
	struct axi_dma_transfer tx_transfer;
	uint64_t tx_rate;
	uint32_t tx_bram_size = 0;
	uint32_t tx_samples;
	uint32_t tx_size;
	uint32_t conj_coherence;
	uint32_t amplitude_tx;
	uint32_t amplitude_fs;
	uint8_t tx_num_conv;
	bool loopback_pass;

	int ret = 0;

	pr_info("Enter DMA example\n");

	ret = adf4382_init(&adf4382_dev, &adf4382_ip);
	if (ret) {
		pr_info("ADF4382 initialization failed\n");
		goto error;
	}

	ret = hmc7044_init(&hmc7044_dev, &hmc7044_ip);
	if (ret) {
		pr_info("HMC7044 initialization failed\n");
		goto error_adf4382;
	}

	ret = axi_dmac_init(&rx_dmac, &rx_dmac_ip);
	if (ret) {
		pr_info("RX DMAC initialization failed\n");
		goto error_hmc7044;
	}

	ret = axi_dmac_init(&tx_dmac, &tx_dmac_ip);
	if (ret) {
		pr_info("TX DMAC initialization failed\n");
		goto error_rx_dmac;
	}

	ret = adxcvr_init(&tx_adxcvr, &tx_adxcvr_ip);
	if (ret) {
		pr_info("TX ADXCVR initialization failed\n");
		goto error_tx_dmac;
	}
	tx_jesd_clk.xcvr = tx_adxcvr;

	ret = adxcvr_init(&rx_adxcvr, &rx_adxcvr_ip);
	if (ret) {
		pr_info("RX ADXCVR initialization failed\n");
		goto error_tx_adxcvr;
	}
	rx_jesd_clk.xcvr = rx_adxcvr;

	rx_lane_clk.platform_ops = &jesd204_clk_ops;
	rx_lane_clk.dev_desc = &rx_jesd_clk;
	rx_jesd204_ip.lane_clk = &rx_lane_clk;

	tx_lane_clk.platform_ops = &jesd204_clk_ops;
	tx_lane_clk.dev_desc = &tx_jesd_clk;
	tx_jesd204_ip.lane_clk = &tx_lane_clk;

	ret = axi_jesd204_rx_init(&rx_jesd, &rx_jesd204_ip);
	if (ret) {
		pr_info("JESD RX initialization failed\n");
		goto error_rx_adxcvr;
	}
	rx_jesd_clk.jesd_rx = rx_jesd;

	ret = axi_jesd204_tx_init(&tx_jesd, &tx_jesd204_ip);
	if (ret) {
		pr_info("JESD TX initialization failed\n");
		goto error_rx_jesd;
	}
	tx_jesd_clk.jesd_tx = tx_jesd;

	ret = ad9088_init(&ad9088_phy, &ad9088_ip);
	if (ret) {
		pr_info("AD9088 initialization failed\n");
		goto error_tx_jesd;
	}

	struct jesd204_topology *topology;
	struct jesd204_topology_dev devs[] = {
		{
			.jdev = hmc7044_dev->jdev,
			.link_ids = {
				FRAMER_LINK_A0_RX,
				DEFRAMER_LINK_A0_TX
			},
			.links_number = 2,
			.is_sysref_provider = true,
		},
		{
			.jdev = rx_jesd->jdev,
			.link_ids = {FRAMER_LINK_A0_RX},
			.links_number = 1,
		},
		{
			.jdev = tx_jesd->jdev,
			.link_ids = {DEFRAMER_LINK_A0_TX},
			.links_number = 1,
		},
		{
			.jdev = ad9088_phy->jdev,
			.link_ids = {
				FRAMER_LINK_A0_RX,
				DEFRAMER_LINK_A0_TX
			},
			.links_number = 2,
			.is_top_device = true,
		},
	};

	ret = jesd204_topology_init(&topology, devs,
				    NO_OS_ARRAY_SIZE(devs));
	if (ret) {
		pr_info("JESD204 topology init failed\n");
		goto error_ad9088;
	}

	ret = jesd204_fsm_start(topology, JESD204_LINKS_ALL);
	if (ret) {
		pr_info("JESD204 FSM start failed\n");
		goto error_topology;
	}

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);

	/*
	 * Derive the capture geometry from the link the FSM just brought up
	 * rather than hardcoding it, so a profile change cannot silently
	 * corrupt the buffer layout.
	 */
	num_conv = ad9088_phy->profile.jtx[TEST_TONE_SIDE].tx_link_cfg[0].m_minus1 + 1;
	np = ad9088_phy->profile.jtx[TEST_TONE_SIDE].tx_link_cfg[0].np_minus1 + 1;

	if (!num_conv || num_conv > MAX_LINK_CONVERTERS) {
		pr_err("Unexpected converter count M=%u\n", num_conv);
		ret = -EINVAL;
		goto error_topology;
	}

	/* Clamp the capture depth to what the static buffer can hold. */
	samples_per_conv = ADC_BUFFER_SAMPLES;
	if (samples_per_conv * num_conv > NO_OS_ARRAY_SIZE(adc_buffer_dma))
		samples_per_conv = NO_OS_ARRAY_SIZE(adc_buffer_dma) / num_conv;

	transfer_size = samples_per_conv * num_conv * sizeof(adc_buffer_dma[0]);

	pr_info("Capture geometry: M=%u NP=%u samples/conv=%lu bytes=%lu\n",
		num_conv, np, (unsigned long)samples_per_conv,
		(unsigned long)transfer_size);

	struct axi_adc_init rx_adc_init = {
		.name = "rx_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = num_conv,
	};

	struct axi_dac_init tx_dac_init = {
		.name = "tx_dac",
		.base = TX_CORE_BASEADDR,
		.num_channels = num_conv,
	};

	ret = axi_adc_init(&rx_adc, &rx_adc_init);
	if (ret) {
		pr_err("RX TPL core init failed (%d)\n", ret);
		goto error_topology;
	}

	ret = axi_dac_init(&tx_dac, &tx_dac_init);
	if (ret) {
		pr_err("TX TPL core init failed (%d)\n", ret);
		goto error_rx_adc;
	}

	/* Leave the TX datapath idle: the FNCO tone is injected inside RX. */
	axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_ZERO);
	pr_info("Project configured\n\n");

	ret = dma_example_get_capture_rate(ad9088_phy, rx_adc, &capture_rate);
	if (ret)
		goto error_tx_dac;

	tone_hz = (int64_t)no_os_div_u64(capture_rate, TEST_TONE_RATE_DIV);

	/*
	 * Every measurement below runs from this operating point rather than
	 * wherever the profile happened to leave the NCOs.
	 */
	ret = dma_example_set_default_nco(ad9088_phy);
	if (ret)
		goto error_tx_dac;

	/*
	 * Baseline first, with the datapath idle. Test mode is off after
	 * bring-up, but disable it explicitly so the floor measures a state this
	 * block guarantees rather than one it assumes. Coherence here should
	 * collapse to roughly COHERENCE_SCALE/N, which is what shows the
	 * estimator rejects noise instead of scoring anything handed to it.
	 */
	ret = ad9088_set_fnco_test_tone(ad9088_phy, ADI_APOLLO_RX,
					TEST_TONE_SIDE, TEST_TONE_FDDC, false,
					0);
	if (ret) {
		pr_err("Disabling the RX FNCO test tone failed (%d)\n", ret);
		goto error_tx_dac;
	}

	no_os_mdelay(10);

	ret = dma_example_capture(rx_dmac, transfer_size);
	if (ret)
		goto error_tx_dac;

	floor_coherence = dma_example_coherence(adc_buffer_dma, samples_per_conv,
						num_conv, -tone_hz,
						capture_rate, NULL, NULL);

	/*
	 * Held in its own flag rather than ret: the tone block below issues
	 * device calls that overwrite ret, so the two verdicts are combined only
	 * once both blocks have run.
	 */
	floor_pass = floor_coherence < COHERENCE_PASS;

	pr_info("--- Noise floor: probed at %ld Hz ---\n", (long)(-tone_hz));
	pr_info("  coh %lu.%lu%% (expect near %lu.%lu%%)\n",
		PCT_WHOLE(floor_coherence), PCT_TENTH(floor_coherence),
		PCT_WHOLE(COHERENCE_SCALE / samples_per_conv + 1),
		PCT_TENTH(COHERENCE_SCALE / samples_per_conv + 1));

	if (!floor_pass)
		pr_err("  the estimator scores the idle capture as a tone; "
		       "the measurement itself is not trustworthy\n");

	pr_info("  %s\n\n", floor_pass ? "PASS" : "FAIL");

	/*
	 * Test mode replaces the mixer input with a constant which the NCO then
	 * rotates, so the FDDC emits a complex tone at the FNCO frequency.
	 */
	ret = ad9088_set_fnco_test_tone(ad9088_phy, ADI_APOLLO_RX,
					TEST_TONE_SIDE, TEST_TONE_FDDC, true,
					TEST_TONE_OFFSET);
	if (ret) {
		pr_err("Enabling the RX FNCO test tone failed (%d)\n", ret);
		goto error_tx_dac;
	}

	ret = ad9088_set_fnco_freq(ad9088_phy, ADI_APOLLO_RX, TEST_TONE_SIDE,
				   TEST_TONE_FDDC, tone_hz);
	if (ret) {
		pr_err("Setting the RX FNCO frequency failed (%d)\n", ret);
		goto error_tone;
	}

	pr_info("--- FNCO tone test: %ld Hz (capture rate / %u) ---\n",
		(long)tone_hz, TEST_TONE_RATE_DIV);
	pr_info("  expected amplitude %lu LSB (offset 0x%x), scored %ld Hz\n",
		(unsigned long)(TEST_TONE_OFFSET * 2828 / 1000),
		TEST_TONE_OFFSET, (long)(-tone_hz));

	no_os_mdelay(10);

	ret = dma_example_capture(rx_dmac, transfer_size);
	if (ret)
		goto error_tone;

	// dma_example_report(adc_buffer_dma, samples_per_conv, num_conv);

	/*
	 * An RX FDDC downconverts: it multiplies by exp(-jwn) so that a signal
	 * at +f_nco lands at DC. The test tone is a constant injected ahead of
	 * that mixer, so it comes out rotating at -f_nco. Verified on HW -- a
	 * programmed +Fs/8 captures as exactly -45 deg/sample. A TX FDUC
	 * upconverts and would need the opposite sign here.
	 */
	coherence = dma_example_coherence(adc_buffer_dma, samples_per_conv,
					  num_conv, -tone_hz, capture_rate,
					  &amplitude, &envelope_spread);

	tone_pass = coherence >= COHERENCE_PASS &&
		    envelope_spread <= ENVELOPE_SPREAD_MAX;

	pr_info("  coh %lu.%lu%% spread %lu amp %lu\n",
		PCT_WHOLE(coherence), PCT_TENTH(coherence),
		(unsigned long)envelope_spread,
		(unsigned long)amplitude);
	pr_info("  %s\n\n", tone_pass ? "PASS" : "FAIL");

	/*
	 * Everything above validates the RX datapath only: test mode discards
	 * the FDDC mixer input, so the tone never passes through the DAC, the
	 * cables or the ADC. Drive the DAC from DMA instead and capture what
	 * comes back, which is the first measurement that covers the whole
	 * chain. Test mode has to go first -- it would discard the loopback
	 * signal exactly as it discards everything else upstream.
	 */
	ret = ad9088_set_fnco_test_tone(ad9088_phy, ADI_APOLLO_RX,
					TEST_TONE_SIDE, TEST_TONE_FDDC, false,
					0);
	if (ret) {
		pr_err("Disabling the RX FNCO test tone failed (%d)\n", ret);
		goto error_tone;
	}

	pr_info("--- DAC loopback test ---\n");

	ret = dma_example_get_tx_rate(ad9088_phy, tx_dac, &tx_rate);
	if (ret)
		goto error_tone;

	/*
	 * The tone test retuned the RX FNCO, which stays where it was left and
	 * would otherwise translate the loopback signal by that much.
	 */
	ret = dma_example_set_default_nco(ad9088_phy);
	if (ret)
		goto error_tone;

	tx_num_conv = ad9088_phy->profile.jrx[TEST_TONE_SIDE]
		      .rx_link_cfg[0].m_minus1 + 1;

	if (!tx_num_conv || tx_num_conv > MAX_LINK_CONVERTERS) {
		pr_err("Unexpected transmit converter count M=%u\n",
		       tx_num_conv);
		ret = -EINVAL;
		goto error_tone;
	}

	/*
	 * The offload replays its whole BRAM whatever was written into it, so
	 * fill all of it rather than leaving the tail to come back as noise.
	 */
	no_os_axi_io_read(TX_DATA_OFFLOAD_BASEADDR, AXI_DO_REG_MEMORY_SIZE_LSB,
			  &tx_bram_size);

	tx_size = sizeof(dac_buffer_dma);
	if (tx_bram_size && tx_bram_size < tx_size)
		tx_size = tx_bram_size;

	/* The DMAC rejects a source address off its data path width. */
	tx_size &= ~(uint32_t)(DMA_SRC_WIDTH_BYTES - 1);
	tx_samples = tx_size / (tx_num_conv * sizeof(dac_buffer_dma[0]));
	tx_size = tx_samples * tx_num_conv * sizeof(dac_buffer_dma[0]);

	pr_info("  tone %ld Hz (capture rate / %u), M=%u, %lu samples/conv, "
		"%lu bytes\n", (long)tone_hz, TEST_TONE_RATE_DIV, tx_num_conv,
		(unsigned long)tx_samples, (unsigned long)tx_size);

	/*
	 * The same frequency the tone test uses. With both sides' NCOs on the
	 * same default it comes back where it was sent, on a capture bin, and a
	 * whole number of cycles fits the transmit buffer, so the cyclic wrap
	 * leaves no phase discontinuity for a capture to straddle -- which
	 * matters because the capture is shorter than the replay and can start
	 * anywhere in it.
	 */
	dma_example_fill_tone(dac_buffer_dma, tx_samples, tx_num_conv,
			      tone_hz, tx_rate, LOOPBACK_TX_AMPLITUDE);

	/* MEM_TO_DEV reads DDR, so dirty lines have to land there first. */
	Xil_DCacheFlushRange((uintptr_t)dac_buffer_dma, tx_size);

	ret = axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_DMA);
	if (ret) {
		pr_err("Selecting the DMA data source failed (%d)\n", ret);
		goto error_tone;
	}

	tx_transfer.size = tx_size;
	tx_transfer.transfer_done = 0;
	tx_transfer.cyclic = CYCLIC;
	tx_transfer.src_addr = (uintptr_t)dac_buffer_dma;
	tx_transfer.dest_addr = 0;

	/*
	 * Cyclic keeps the tone running for the whole capture. It is a build
	 * option of the DMAC rather than a guarantee, so fall back to a single
	 * pass if the core rejects it. Never wait for completion either way: a
	 * cyclic transfer raises no end-of-transfer and would only time out.
	 */
	ret = axi_dmac_transfer_start(tx_dmac, &tx_transfer);
	if (ret) {
		pr_info("  cyclic transfer unavailable, using a single pass\n");
		tx_transfer.cyclic = NO;
		ret = axi_dmac_transfer_start(tx_dmac, &tx_transfer);
	}

	if (ret) {
		pr_err("TX DMA transfer start failed (%d)\n", ret);
		goto error_tx_stream;
	}

	no_os_mdelay(10);

	ret = dma_example_capture(rx_dmac, transfer_size);
	if (ret)
		goto error_tx_stream;

	/*
	 * The tone test scores -tone_hz because test mode injects a constant
	 * ahead of a downconverting mixer. This one transmits a real +tone_hz
	 * and the aligned mixers cancel, so it arrives at +tone_hz. The second
	 * probe covers the transmitted I/Q ordering, the one convention in this
	 * path no measurement has confirmed yet: a swapped pair sends -f.
	 */
	coherence = dma_example_coherence(adc_buffer_dma, samples_per_conv,
					  num_conv, tone_hz, capture_rate,
					  &amplitude, &envelope_spread);

	conj_coherence = dma_example_coherence(adc_buffer_dma, samples_per_conv,
					       num_conv, -tone_hz, capture_rate,
					       NULL, NULL);

	loopback_pass = envelope_spread <= ENVELOPE_SPREAD_MAX &&
			(coherence >= COHERENCE_PASS ||
			 conj_coherence >= COHERENCE_PASS);

	/*
	 * Two different denominators, so both are named: the fraction of what
	 * was transmitted is the loss through the cables, while the fraction of
	 * full scale is the headroom left at the ADC.
	 */
	amplitude_tx = amplitude * COHERENCE_SCALE / LOOPBACK_TX_AMPLITUDE;
	amplitude_fs = amplitude * COHERENCE_SCALE / SAMPLE_FULL_SCALE;

	pr_info("  coh +f %lu.%lu%%  -f %lu.%lu%%  spread %lu\n",
		PCT_WHOLE(coherence), PCT_TENTH(coherence),
		PCT_WHOLE(conj_coherence), PCT_TENTH(conj_coherence),
		(unsigned long)envelope_spread);
	pr_info("  amp %lu LSB: %lu.%lu%% of tx %u, %lu.%lu%% of fullscale\n",
		(unsigned long)amplitude,
		PCT_WHOLE(amplitude_tx), PCT_TENTH(amplitude_tx),
		LOOPBACK_TX_AMPLITUDE,
		PCT_WHOLE(amplitude_fs), PCT_TENTH(amplitude_fs));

	if (conj_coherence >= COHERENCE_PASS && coherence < COHERENCE_PASS)
		pr_info("  matched the conjugate: transmitted I and Q are "
			"swapped\n");

	pr_info("  %s\n", loopback_pass ? "PASS" : "FAIL");

	ret = (floor_pass && tone_pass && loopback_pass) ? 0 : -EIO;

	/*
	 * The TX stream is left running on the pass path: error_tx_stream is the
	 * stop, and the teardown below is the same sequence the error ladder
	 * runs, so a passing run shares it from that label on.
	 */
	goto error_tx_stream;

error_tx_stream:
	axi_dmac_transfer_stop(tx_dmac);
	axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_ZERO);
error_tone:
	ad9088_set_fnco_test_tone(ad9088_phy, ADI_APOLLO_RX, TEST_TONE_SIDE,
				  TEST_TONE_FDDC, false, 0);
error_tx_dac:
	axi_dac_remove(tx_dac);
error_rx_adc:
	axi_adc_remove(rx_adc);
error_topology:
	jesd204_topology_remove(topology);
error_ad9088:
	ad9088_remove(ad9088_phy);
error_tx_jesd:
	axi_jesd204_tx_remove(tx_jesd);
error_rx_jesd:
	axi_jesd204_rx_remove(rx_jesd);
error_rx_adxcvr:
	adxcvr_remove(rx_adxcvr);
error_tx_adxcvr:
	adxcvr_remove(tx_adxcvr);
error_tx_dmac:
	axi_dmac_remove(tx_dmac);
error_rx_dmac:
	axi_dmac_remove(rx_dmac);
error_hmc7044:
	hmc7044_remove(hmc7044_dev);
error_adf4382:
	adf4382_remove(adf4382_dev);
error:
	if (ret)
		pr_info("Error!\n");

	return ret;
}
