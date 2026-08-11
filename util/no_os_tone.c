/***************************************************************************//**
 *   @file   no_os_tone.c
 *   @brief  Complex tone generation and single-bin analysis utility.
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

#include <stdbool.h>
#include <stdint.h>

#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_tone.h"
#include "no_os_util.h"

/*
 * Private reference oscillator, shared by the generator and the analyser below.
 *
 * sin(2*pi*i/512) in Q15: a stored value is the fraction v/32768, so full scale
 * is 32767 rather than 1.0 and every product needs >> SIN_TABLE_SHIFT to come
 * back to the scale of the input samples. Signed, so a lookup is a mask and an
 * index with no bias to undo. cos is the same table a quarter period earlier --
 * one add rather than a second table.
 *
 * Deliberately not no_os_sine_lut_16 (util/no_os_sin_lut.c): that table is
 * full-scale unsigned, so it is not this one biased by 0x8000 and the two are
 * not interchangeable. Kept private for the same reason -- a caller wanting a
 * sine table should pick one knowingly, not inherit this one.
 */
#define SIN_TABLE_LEN		512
#define SIN_TABLE_QUARTER	(SIN_TABLE_LEN / 4)
#define SIN_TABLE_MASK		(SIN_TABLE_LEN - 1)
#define SIN_TABLE_SHIFT		15

/*
 * Phase is a 32-bit accumulator, so the top 9 bits index the 512-entry table.
 * The low 23 bits are the fractional part, dropped at lookup: a frequency whose
 * step is an exact multiple of 2^23 therefore lands on a table entry every
 * sample, and any other is quantised to the nearest.
 */
#define SIN_TABLE_PHASE_SHIFT	23

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

static inline int32_t no_os_tone_sin_q15(uint32_t idx)
{
	return sin_table_q15[idx & SIN_TABLE_MASK];
}

static inline int32_t no_os_tone_cos_q15(uint32_t idx)
{
	return no_os_tone_sin_q15(idx + SIN_TABLE_QUARTER);
}

/**
 * @brief Check a buffer layout describes a usable I/Q pair.
 * @param layout - Interleaved buffer layout.
 * @return true if the layout is usable, false otherwise.
 */
static bool no_os_tone_layout_valid(const struct no_os_tone_layout *layout)
{
	if (!layout || !layout->num_conv)
		return false;

	if (layout->conv_i >= layout->num_conv ||
	    layout->conv_q >= layout->num_conv)
		return false;

	return layout->conv_i != layout->conv_q;
}

/**
 * @brief Phase step a tone generator and an analyser both derive.
 *
 * Shared so a transmitted frequency and the frequency it is scored at cannot
 * disagree about what a given rate means.
 *
 * no_os_div64_u64_rem() rather than no_os_div_u64(), whose divisor narrows to
 * uint32_t: a rate above 4.29 GHz would be truncated on the way in and yield a
 * step several times wrong, silently and without a compiler warning. High-speed
 * converter clocks do exceed that, so the wide divisor is not a precaution
 * against a hypothetical.
 *
 * @param freq_hz - Frequency, may be negative.
 * @param rate_hz - Sample rate in Hz.
 * @return Step in 32-bit phase turns per sample.
 */
uint32_t no_os_tone_phase_step(int64_t freq_hz, uint64_t rate_hz)
{
	uint64_t rem;

	if (!rate_hz)
		return 0;

	if (freq_hz < 0)
		return (uint32_t)(-(int32_t)no_os_div64_u64_rem(
					  (uint64_t)(-freq_hz) << 32, rate_hz,
					  &rem));

	return (uint32_t)no_os_div64_u64_rem((uint64_t)freq_hz << 32, rate_hz,
					     &rem);
}

/**
 * @brief Coherence of a capture against a complex tone at the expected offset.
 *
 * Correlates z[n] = I[n] + jQ[n] against exp(-jwn) and normalises by the total
 * captured power:
 *
 *   coherence = |sum z[n] exp(-jwn)|^2 / (N * sum |z[n]|^2)
 *
 * The result is ~NO_OS_TONE_SCALE when all the captured energy sits exactly at
 * the expected frequency and ~NO_OS_TONE_SCALE/N for noise or a tone anywhere
 * else, so it answers "is this the tone we asked for" rather than merely "is
 * something there". It is signed through the phase increment: a negative
 * expected frequency only correlates with a negative-rotating input, which is
 * what makes a swapped I/Q pair or a spectrally inverted datapath visible.
 *
 * The envelope of a single complex tone is constant, so the spread of I^2+Q^2
 * is a shape check independent of frequency. It is a min/max extreme statistic
 * over the whole capture though, so noise dominates it -- coherence is the
 * stricter test of the two.
 *
 * Samples are read as signed two's complement. Handing this offset-binary data
 * does not fail cleanly: the bias reads as a large DC term that inflates total
 * power, which drags coherence down to somewhere around 700-800 per-mille --
 * degraded, but able to land either side of a threshold. The dependable symptom
 * is the reported amplitude instead, which comes back *above* 16-bit full scale
 * and so cannot be a real converter level. Re-bias to signed before calling.
 *
 * @param buf - Capture buffer holding interleaved converter samples, read as
 *		signed two's-complement 16-bit.
 * @param samples - Samples captured per converter.
 * @param layout - Where the I/Q pair sits in the buffer.
 * @param freq_hz - Expected baseband rotation rate, signed, in Hz. This is the
 *		    rate the samples are expected to rotate at, not the
 *		    frequency programmed into any mixer: a downconverting mixer
 *		    fed a constant emits -f, an upconverter +f.
 * @param rate_hz - Capture sample rate in Hz.
 * @param result - Returns the coherence, amplitude and envelope spread.
 * @return 0 on success, negative error code otherwise.
 */
int no_os_tone_coherence(const uint16_t *buf, uint32_t samples,
			 const struct no_os_tone_layout *layout,
			 int64_t freq_hz, uint64_t rate_hz,
			 struct no_os_tone_result *result)
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

	if (!buf || !result || !samples || !rate_hz)
		return -EINVAL;

	if (!no_os_tone_layout_valid(layout))
		return -EINVAL;

	result->coherence = 0;
	result->amplitude = 0;
	result->spread = 0;

	/*
	 * Phase increment in 32-bit turns. Negative frequencies wrap to the top
	 * half of the accumulator, which is exactly the conjugate reference.
	 */
	step = no_os_tone_phase_step(freq_hz, rate_hz);

	for (i = 0; i < samples; i++) {
		s_i = (int16_t)buf[i * layout->num_conv + layout->conv_i];
		s_q = (int16_t)buf[i * layout->num_conv + layout->conv_q];

		/*
		 * Table lookup rather than an iterated rotation: the phase
		 * accumulator cannot drift over a long capture, where a
		 * recurrence would accumulate rounding error. Negating the sine
		 * gives the conjugate reference exp(-jwn).
		 */
		idx = phase >> SIN_TABLE_PHASE_SHIFT;
		ref_re = no_os_tone_cos_q15(idx);
		ref_im = -no_os_tone_sin_q15(idx);

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

	result->amplitude = (uint32_t)no_os_sqrt64(no_os_div_u64(env_sum,
			    samples));

	rms_max = (uint32_t)no_os_sqrt64(env_max);
	rms_min = (uint32_t)no_os_sqrt64(env_min);

	result->spread = rms_max ?
			 (uint32_t)(((uint64_t)(rms_max - rms_min) *
				     NO_OS_TONE_SCALE) / rms_max) : 0;

	if (!power)
		return 0;

	/*
	 * |sum| grows with the capture depth -- around 2^42 for 16384 samples at
	 * 16-bit full scale -- so squaring it directly would overflow. Divide both
	 * sides of the ratio by N first, which cancels the N in the denominator
	 * and leaves two per-sample quantities: the correlated amplitude and the
	 * mean power, each bounded by full scale whatever N is.
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

	result->coherence = (uint32_t)no_os_div_u64(mag * NO_OS_TONE_SCALE,
			    norm);

	return 0;
}

/**
 * @brief Fill a transmit buffer with a complex tone.
 *
 * Uses the same phase accumulator and table as no_os_tone_coherence(), so a
 * transmitted frequency and the frequency it is scored at cannot disagree about
 * what a given rate means. Converters beyond the I/Q pair are zeroed so an
 * unrelated datapath cannot contribute to a capture.
 *
 * Samples are written as signed two's complement. A transmit path expecting
 * offset binary needs them re-biased after this returns.
 *
 * @param buf - Destination buffer, layout->num_conv interleaved converters.
 * @param samples - Samples per converter to write.
 * @param layout - Where the I/Q pair sits in the buffer.
 * @param freq_hz - Tone frequency, may be negative.
 * @param rate_hz - Transmit sample rate in Hz.
 * @param amplitude - Peak amplitude in LSB.
 * @return 0 on success, negative error code otherwise.
 */
int no_os_tone_fill_iq(uint16_t *buf, uint32_t samples,
		       const struct no_os_tone_layout *layout,
		       int64_t freq_hz, uint64_t rate_hz, int32_t amplitude)
{
	uint32_t phase = 0;
	uint32_t step;
	uint32_t idx;
	uint32_t i;
	uint8_t c;

	if (!buf || !rate_hz)
		return -EINVAL;

	if (!no_os_tone_layout_valid(layout))
		return -EINVAL;

	step = no_os_tone_phase_step(freq_hz, rate_hz);

	for (i = 0; i < samples; i++) {
		idx = phase >> SIN_TABLE_PHASE_SHIFT;

		for (c = 0; c < layout->num_conv; c++)
			buf[i * layout->num_conv + c] = 0;

		buf[i * layout->num_conv + layout->conv_i] = (uint16_t)(int16_t)
				((amplitude * no_os_tone_cos_q15(idx))
				 >> SIN_TABLE_SHIFT);
		buf[i * layout->num_conv + layout->conv_q] = (uint16_t)(int16_t)
				((amplitude * no_os_tone_sin_q15(idx))
				 >> SIN_TABLE_SHIFT);

		phase += step;
	}

	return 0;
}

/**
 * @brief Report one measurement and return whether it passes.
 *
 * The single report every measurement ends with, so a log reads the same
 * whatever produced the capture. Judging is driven entirely by @p test: which
 * way the verdict runs, whether a conjugate probe counts, and which reference
 * levels the amplitude is worth stating against are all things the numbers
 * cannot say for themselves.
 *
 * The thresholds are per-test rather than fixed gates: the defaults in the
 * header are a calibrated starting point, not a rule that holds on every board.
 *
 * @param test - What the measurement is and what passing means for it.
 * @param result - Measurement to report.
 * @return true if the measurement passes, false otherwise.
 */
bool no_os_tone_report(const struct no_os_tone_test *test,
		       const struct no_os_tone_result *result)
{
	uint32_t expected;
	bool conj_scored;
	bool spread_ok;
	bool scored;
	bool pass;

	if (!test || !result)
		return false;

	pr_info("--- %s ---\n", test->name ? test->name : "Tone measurement");

	scored = result->coherence >= test->limits.coherence_min;
	spread_ok = result->spread <= test->limits.spread_max;
	conj_scored = test->conjugate &&
		      test->conjugate->coherence >= test->limits.coherence_min;

	switch (test->sense) {
	case NO_OS_TONE_SENSE_QUIET:
		/*
		 * Inverted: a quietened path passes by not scoring. Envelope
		 * spread is meaningless on noise, so it is left out of the
		 * verdict rather than compared and ignored.
		 */
		pass = !scored;
		break;
	case NO_OS_TONE_SENSE_EITHER:
		pass = spread_ok && (scored || conj_scored);
		break;
	default:
		pass = spread_ok && scored;
		break;
	}

	pr_info("  probed %ld Hz over %lu samples\n", (long)test->freq_hz,
		(unsigned long)test->samples);

	if (test->conjugate)
		pr_info("  coh +f %lu.%lu%%  -f %lu.%lu%%\n",
			NO_OS_TONE_PCT_WHOLE(result->coherence),
			NO_OS_TONE_PCT_TENTH(result->coherence),
			NO_OS_TONE_PCT_WHOLE(test->conjugate->coherence),
			NO_OS_TONE_PCT_TENTH(test->conjugate->coherence));
	else
		pr_info("  coh %lu.%lu%%\n",
			NO_OS_TONE_PCT_WHOLE(result->coherence),
			NO_OS_TONE_PCT_TENTH(result->coherence));

	/*
	 * A quiet capture has no tone to describe, so the shape and level lines
	 * would only be noise about noise. State the floor it should collapse to
	 * instead, which is what makes the number judgeable at a glance.
	 */
	if (test->sense == NO_OS_TONE_SENSE_QUIET) {
		expected = test->samples ?
			   NO_OS_TONE_SCALE / test->samples + 1 : 0;
		pr_info("  expected near %lu.%lu%% for noise\n",
			NO_OS_TONE_PCT_WHOLE(expected),
			NO_OS_TONE_PCT_TENTH(expected));
	} else {
		pr_info("  spread %lu (max %lu)  amp %lu LSB\n",
			(unsigned long)result->spread,
			(unsigned long)test->limits.spread_max,
			(unsigned long)result->amplitude);

		/*
		 * Two different denominators, so each is named where it applies:
		 * the fraction of what was transmitted is the loss through the
		 * path, the fraction of full scale is the headroom left.
		 */
		if (test->tx_amplitude)
			pr_info("      %lu.%lu%% of the %lu LSB transmitted\n",
				NO_OS_TONE_PCT_WHOLE(result->amplitude *
						     NO_OS_TONE_SCALE /
						     test->tx_amplitude),
				NO_OS_TONE_PCT_TENTH(result->amplitude *
						     NO_OS_TONE_SCALE /
						     test->tx_amplitude),
				(unsigned long)test->tx_amplitude);

		if (test->full_scale)
			pr_info("      %lu.%lu%% of full scale\n",
				NO_OS_TONE_PCT_WHOLE(result->amplitude *
						     NO_OS_TONE_SCALE /
						     test->full_scale),
				NO_OS_TONE_PCT_TENTH(result->amplitude *
						     NO_OS_TONE_SCALE /
						     test->full_scale));
	}

	/* Say why, so a failing log does not need the source to be read. */
	if (!pass && test->sense == NO_OS_TONE_SENSE_QUIET)
		pr_info("  an idle capture scores as a tone: the measurement\n"
			"      itself is not trustworthy\n");
	else if (!pass && !scored && !conj_scored)
		pr_info("  no tone at the probed frequency: it is elsewhere,\n"
			"      or nothing reached the buffer\n");
	else if (!pass && !spread_ok)
		pr_info("  envelope is not constant: the capture is not a "
			"single clean tone\n");

	if (test->sense == NO_OS_TONE_SENSE_EITHER && conj_scored && !scored)
		pr_info("  matched the conjugate: transmitted I and Q are "
			"swapped\n");

	pr_info("  %s\n\n", pass ? "PASS" : "FAIL");

	return pass;
}
