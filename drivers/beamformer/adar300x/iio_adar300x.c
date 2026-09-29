/***************************************************************************//**
 *   @file   iio_adar300x.c
 *   @brief  Implementation of the ADAR300X IIO driver.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "iio_adar300x.h"
#include "no_os_alloc.h"
#include "no_os_util.h"

#define ADAR300X_IIO_BEAMSTATE_CHANNELS	(ADAR300X_IIO_BEAMS * \
					 ADAR300X_UNPACKED_BEAMSTATE_LEN)

/* One byte of the scan mask per beam, one bit per beamstate value */
#define ADAR300X_IIO_BEAM_SCAN_MSK(beam)	(NO_OS_GENMASK(7, 0) << \
						 ((beam) * ADAR300X_UNPACKED_BEAMSTATE_LEN))

/* Data sheet step sizes: 0.865 ps of delay and 0.5 dB of attenuation */
#define ADAR300X_IIO_DELAY_SCALE_NUM		865
#define ADAR300X_IIO_DELAY_SCALE_DEN		1000
#define ADAR300X_IIO_ATTEN_SCALE_NUM		1
#define ADAR300X_IIO_ATTEN_SCALE_DEN		2

/* Amplifier attributes pack the amplifier index, its state and its bank */
#define ADAR300X_IIO_AMP_PRIV(element, state, idx) \
	((element) << 8 | (state) << 4 | (idx))
#define ADAR300X_IIO_AMP_IS_ELEMENT(priv)	((priv) >> 8)
#define ADAR300X_IIO_AMP_STATE(priv)		(((priv) >> 4) & 0xF)
#define ADAR300X_IIO_AMP_INDEX(priv)		((priv) & 0xF)

static const char *const adar300x_iio_modes[] = {
	[ADAR300X_IIO_DIRECT] = "direct",
	[ADAR300X_IIO_MEMORY] = "memory",
	[ADAR300X_IIO_FIFO] = "fifo",
	[ADAR300X_IIO_INST_DIRECT] = "instant_direct",
	[ADAR300X_IIO_RESET] = "reset",
	[ADAR300X_IIO_MUTE] = "mute",
};

static const char *const adar300x_iio_update_sources[] = {
	"pin",
	"SPI",
};

/**
 * @brief Finds a string in a list, ignoring a trailing newline.
 * @param buf  - String to look up.
 * @param list - Candidate strings.
 * @param n    - Number of candidates.
 * @return     - Index of the match or -EINVAL.
 */
static int adar300x_iio_match(const char *buf, const char *const *list, int n)
{
	size_t l;
	int i;

	for (i = 0; i < n; i++) {
		l = strlen(list[i]);
		if (!strncmp(buf, list[i], l) &&
		    (buf[l] == '\0' || buf[l] == '\n'))
			return i;
	}

	return -EINVAL;
}

/**
 * @brief Prints a list of strings separated by spaces.
 * @param buf  - Output buffer.
 * @param len  - Output buffer length.
 * @param list - Strings to print.
 * @param n    - Number of strings.
 * @return     - Number of characters written.
 */
static int adar300x_iio_list(char *buf, uint32_t len, const char *const *list,
			     int n)
{
	uint32_t l = 0;
	int i;

	for (i = 0; i < n && l < len; i++)
		l += snprintf(buf + l, len - l, "%s%s", i ? " " : "", list[i]);

	return l;
}

/**
 * @brief Parses an integer attribute value and range checks it.
 * @param buf - Attribute value.
 * @param max - Largest accepted value.
 * @param val - Parsed value.
 * @return    - 0 in case of success or negative error code otherwise.
 */
static int adar300x_iio_parse(char *buf, int32_t max, uint8_t *val)
{
	int32_t v;
	int ret;

	ret = iio_parse_value(buf, IIO_VAL_INT, &v, NULL);
	if (ret)
		return ret;

	if (v < 0 || v > max)
		return -EINVAL;

	*val = v;

	return 0;
}

static int adar300x_iio_show_int(char *buf, uint32_t len, int32_t val)
{
	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int adar300x_iio_show_mode(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct adar300x_iio_dev *iio = dev;

	return snprintf(buf, len, "%s", adar300x_iio_modes[iio->mode[priv]]);
}

static int adar300x_iio_store_mode(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct adar300x_iio_dev *iio = dev;
	int mode, ret;

	mode = adar300x_iio_match(buf, adar300x_iio_modes,
				  NO_OS_ARRAY_SIZE(adar300x_iio_modes));
	if (mode < 0)
		return mode;

	if (mode == ADAR300X_IIO_RESET || mode == ADAR300X_IIO_MUTE) {
		/* A beam in instantaneous direct mode cannot enter either */
		if (iio->mode[priv] == ADAR300X_IIO_INST_DIRECT)
			return -EINVAL;
	} else {
		ret = adar300x_set_beam_mode(iio->adar300x_dev, priv,
					     (enum adar300x_beam_mode)mode);
		if (ret)
			return ret;
	}

	iio->mode[priv] = mode;

	return len;
}

static int adar300x_iio_show_modes(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	return adar300x_iio_list(buf, len, adar300x_iio_modes,
				 NO_OS_ARRAY_SIZE(adar300x_iio_modes));
}

static int adar300x_iio_show_update(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct adar300x_iio_dev *iio = dev;
	uint8_t val;
	int ret;

	/* The strobe self deasserts, so this reads 0 once the update is done */
	ret = adar300x_page_reg_read(iio->adar300x_dev, ADAR300X_PAGE_CONFIG,
				     ADAR300X_REG_BEAMWISE_UPDATE, &val);
	if (ret)
		return ret;

	return adar300x_iio_show_int(buf, len,
				     !!(val & ADAR300X_BEAM_UPDATE_MSK(priv)));
}

static int adar300x_iio_store_update(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct adar300x_iio_dev *iio = dev;
	enum adar300x_beam_command cmd;
	int ret;

	if (iio->mode[priv] == ADAR300X_IIO_RESET)
		cmd = ADAR300X_BEAM_CMD_RESET;
	else if (iio->mode[priv] == ADAR300X_IIO_MUTE)
		cmd = ADAR300X_BEAM_CMD_MUTE;
	else
		cmd = ADAR300X_BEAM_CMD_UPDATE;

	ret = adar300x_beam_command(iio->adar300x_dev, NO_OS_BIT(priv), cmd);
	if (ret)
		return ret;

	return len;
}

static int adar300x_iio_show_load_mode(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adar300x_iio_dev *iio = dev;

	return snprintf(buf, len, "%s",
			adar300x_iio_modes[iio->load_mode[priv]]);
}

static int adar300x_iio_store_load_mode(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct adar300x_iio_dev *iio = dev;
	int ret;

	ret = adar300x_iio_match(buf, adar300x_iio_modes,
				 NO_OS_ARRAY_SIZE(adar300x_iio_modes));
	if (ret < 0)
		return ret;

	iio->load_mode[priv] = ret;

	return len;
}

static int adar300x_iio_show_ram_index(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adar300x_iio_dev *iio = dev;

	return adar300x_iio_show_int(buf, len, iio->ram_index[priv]);
}

static int adar300x_iio_store_ram_index(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct adar300x_iio_dev *iio = dev;
	int ret;

	ret = adar300x_iio_parse(buf, ADAR300X_RAM_STATES_PER_BEAM - 1,
				 &iio->ram_index[priv]);
	if (ret)
		return ret;

	return len;
}

static int adar300x_iio_show_ram_start(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adar300x_iio_dev *iio = dev;
	uint8_t start;
	int ret;

	ret = adar300x_get_seq_range(iio->adar300x_dev, priv, &start, NULL);
	if (ret)
		return ret;

	return adar300x_iio_show_int(buf, len, start);
}

static int adar300x_iio_store_ram_start(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct adar300x_iio_dev *iio = dev;
	uint8_t start, stop;
	int ret;

	ret = adar300x_iio_parse(buf, ADAR300X_RAM_STATES_PER_BEAM - 1, &start);
	if (ret)
		return ret;

	ret = adar300x_get_seq_range(iio->adar300x_dev, priv, NULL, &stop);
	if (ret)
		return ret;

	ret = adar300x_set_seq_range(iio->adar300x_dev, priv, start, stop);
	if (ret)
		return ret;

	return len;
}

static int adar300x_iio_show_ram_stop(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct adar300x_iio_dev *iio = dev;
	uint8_t stop;
	int ret;

	ret = adar300x_get_seq_range(iio->adar300x_dev, priv, NULL, &stop);
	if (ret)
		return ret;

	return adar300x_iio_show_int(buf, len, stop);
}

static int adar300x_iio_store_ram_stop(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adar300x_iio_dev *iio = dev;
	uint8_t start, stop;
	int ret;

	ret = adar300x_iio_parse(buf, ADAR300X_RAM_STATES_PER_BEAM - 1, &stop);
	if (ret)
		return ret;

	ret = adar300x_get_seq_range(iio->adar300x_dev, priv, &start, NULL);
	if (ret)
		return ret;

	ret = adar300x_set_seq_range(iio->adar300x_dev, priv, start, stop);
	if (ret)
		return ret;

	return len;
}

static int adar300x_iio_show_fifo_rd(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct adar300x_iio_dev *iio = dev;
	uint8_t rd;
	int ret;

	ret = adar300x_get_fifo_pointers(iio->adar300x_dev, priv, NULL, &rd);
	if (ret)
		return ret;

	return adar300x_iio_show_int(buf, len, rd);
}

static int adar300x_iio_show_fifo_wr(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct adar300x_iio_dev *iio = dev;
	uint8_t wr;
	int ret;

	ret = adar300x_get_fifo_pointers(iio->adar300x_dev, priv, &wr, NULL);
	if (ret)
		return ret;

	return adar300x_iio_show_int(buf, len, wr);
}

static int adar300x_iio_show_update_src(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct adar300x_iio_dev *iio = dev;
	bool spi;
	int ret;

	ret = adar300x_get_update_source_spi(iio->adar300x_dev, &spi);
	if (ret)
		return ret;

	return snprintf(buf, len, "%s", adar300x_iio_update_sources[spi]);
}

static int adar300x_iio_store_update_src(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct adar300x_iio_dev *iio = dev;
	int ret;

	ret = adar300x_iio_match(buf, adar300x_iio_update_sources,
				 NO_OS_ARRAY_SIZE(adar300x_iio_update_sources));
	if (ret < 0)
		return ret;

	ret = adar300x_set_update_source_spi(iio->adar300x_dev, ret);
	if (ret)
		return ret;

	return len;
}

static int adar300x_iio_show_update_srcs(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	return adar300x_iio_list(buf, len, adar300x_iio_update_sources,
				 NO_OS_ARRAY_SIZE(adar300x_iio_update_sources));
}

static int adar300x_iio_get_amp(struct adar300x_iio_dev *iio, intptr_t priv,
				uint8_t *bias, bool *enable)
{
	if (ADAR300X_IIO_AMP_IS_ELEMENT(priv))
		return adar300x_get_element_amp(iio->adar300x_dev,
						ADAR300X_IIO_AMP_STATE(priv),
						ADAR300X_IIO_AMP_INDEX(priv),
						bias, enable);

	return adar300x_get_beam_amp(iio->adar300x_dev,
				     ADAR300X_IIO_AMP_STATE(priv),
				     ADAR300X_IIO_AMP_INDEX(priv), bias, enable);
}

static int adar300x_iio_set_amp(struct adar300x_iio_dev *iio, intptr_t priv,
				uint8_t bias, bool enable)
{
	if (ADAR300X_IIO_AMP_IS_ELEMENT(priv))
		return adar300x_set_element_amp(iio->adar300x_dev,
						ADAR300X_IIO_AMP_STATE(priv),
						ADAR300X_IIO_AMP_INDEX(priv),
						bias, enable);

	return adar300x_set_beam_amp(iio->adar300x_dev,
				     ADAR300X_IIO_AMP_STATE(priv),
				     ADAR300X_IIO_AMP_INDEX(priv), bias, enable);
}

static int adar300x_iio_show_amp_bias(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	uint8_t bias;
	bool enable;
	int ret;

	ret = adar300x_iio_get_amp(dev, priv, &bias, &enable);
	if (ret)
		return ret;

	return adar300x_iio_show_int(buf, len, bias);
}

static int adar300x_iio_store_amp_bias(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	uint8_t bias, old;
	bool enable;
	int ret;

	ret = adar300x_iio_parse(buf, ADAR300X_AMP_BIAS_MAX, &bias);
	if (ret)
		return ret;

	ret = adar300x_iio_get_amp(dev, priv, &old, &enable);
	if (ret)
		return ret;

	ret = adar300x_iio_set_amp(dev, priv, bias, enable);
	if (ret)
		return ret;

	return len;
}

static int adar300x_iio_show_amp_en(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	uint8_t bias;
	bool enable;
	int ret;

	ret = adar300x_iio_get_amp(dev, priv, &bias, &enable);
	if (ret)
		return ret;

	return adar300x_iio_show_int(buf, len, enable);
}

static int adar300x_iio_store_amp_en(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	uint8_t bias, enable;
	bool old;
	int ret;

	ret = adar300x_iio_parse(buf, 1, &enable);
	if (ret)
		return ret;

	ret = adar300x_iio_get_amp(dev, priv, &bias, &old);
	if (ret)
		return ret;

	ret = adar300x_iio_set_amp(dev, priv, bias, enable);
	if (ret)
		return ret;

	return len;
}

/**
 * @brief Maps a load mode onto the register bank it addresses.
 * @param mode - Load mode of the beam.
 * @return     - The beamstate bank.
 */
static enum adar300x_beamstate adar300x_iio_bank(enum adar300x_iio_mode mode)
{
	if (mode == ADAR300X_IIO_RESET)
		return ADAR300X_BEAMSTATE_RESET;

	if (mode == ADAR300X_IIO_MUTE)
		return ADAR300X_BEAMSTATE_MUTE;

	return ADAR300X_BEAMSTATE_DIRECT;
}

static int adar300x_iio_show_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	uint8_t values[ADAR300X_UNPACKED_BEAMSTATE_LEN];
	struct adar300x_iio_dev *iio = dev;
	uint8_t beam, idx, val;
	int ret;

	if (channel->type == IIO_TEMP) {
		ret = adar300x_get_temp(iio->adar300x_dev, &val);
		if (ret)
			return ret;

		return adar300x_iio_show_int(buf, len, val);
	}

	beam = channel->address / ADAR300X_UNPACKED_BEAMSTATE_LEN;
	idx = channel->address % ADAR300X_UNPACKED_BEAMSTATE_LEN;

	switch (iio->load_mode[beam]) {
	case ADAR300X_IIO_MEMORY:
		ret = adar300x_get_ram_beamstate(iio->adar300x_dev, beam,
						 iio->ram_index[beam], values);
		if (ret)
			return ret;

		val = values[idx];
		break;
	case ADAR300X_IIO_FIFO:
		val = iio->fifo_state[beam][idx];
		break;
	default:
		ret = adar300x_get_element(iio->adar300x_dev,
					   adar300x_iio_bank(iio->load_mode[beam]),
					   beam, idx / 2, idx % 2, &val);
		if (ret)
			return ret;
	}

	return adar300x_iio_show_int(buf, len, val);
}

static int adar300x_iio_store_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	uint8_t values[ADAR300X_UNPACKED_BEAMSTATE_LEN];
	struct adar300x_iio_dev *iio = dev;
	uint8_t beam, idx, val;
	int ret;

	ret = adar300x_iio_parse(buf, ADAR300X_RAW_MAX, &val);
	if (ret)
		return ret;

	beam = channel->address / ADAR300X_UNPACKED_BEAMSTATE_LEN;
	idx = channel->address % ADAR300X_UNPACKED_BEAMSTATE_LEN;

	switch (iio->load_mode[beam]) {
	case ADAR300X_IIO_MEMORY:
		ret = adar300x_get_ram_beamstate(iio->adar300x_dev, beam,
						 iio->ram_index[beam], values);
		if (ret)
			return ret;

		values[idx] = val;
		ret = adar300x_set_ram_beamstate(iio->adar300x_dev, beam,
						 iio->ram_index[beam], values);
		break;
	case ADAR300X_IIO_FIFO:
		/* A FIFO load needs the whole beamstate, so stage it first */
		iio->fifo_state[beam][idx] = val;
		if (idx == ADAR300X_UNPACKED_BEAMSTATE_LEN - 1)
			ret = adar300x_load_fifo_beamstate(iio->adar300x_dev,
							   beam,
							   iio->fifo_state[beam]);
		break;
	default:
		ret = adar300x_set_element(iio->adar300x_dev,
					   adar300x_iio_bank(iio->load_mode[beam]),
					   beam, idx / 2, idx % 2, val);
	}
	if (ret)
		return ret;

	return len;
}

static int adar300x_iio_show_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	int32_t vals[2];

	switch (channel->type) {
	case IIO_TEMP:
		/* Millidegrees Celsius per code */
		vals[0] = ADAR300X_TEMP_SLOPE_NUM;
		vals[1] = ADAR300X_TEMP_SLOPE_DEN;
		break;
	case IIO_PHASE:
		/* Picoseconds of delay per step */
		vals[0] = ADAR300X_IIO_DELAY_SCALE_NUM;
		vals[1] = ADAR300X_IIO_DELAY_SCALE_DEN;
		break;
	case IIO_POWER:
		/* Decibels of attenuation per step */
		vals[0] = ADAR300X_IIO_ATTEN_SCALE_NUM;
		vals[1] = ADAR300X_IIO_ATTEN_SCALE_DEN;
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
}

static int adar300x_iio_show_offset(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	/*
	 * IIO applies (raw + offset) * scale. With the scale in millidegrees
	 * per code, the nominal point gives
	 *	offset = nominal_mdegc / scale - nominal_code
	 */
	int32_t vals[2] = {
		ADAR300X_TEMP_NOMINAL_MDEGC * ADAR300X_TEMP_SLOPE_DEN -
		ADAR300X_TEMP_NOMINAL_CODE * ADAR300X_TEMP_SLOPE_NUM,
		ADAR300X_TEMP_SLOPE_NUM
	};

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
}

/**
 * @brief Checks that buffered beams are whole beamstates bound for RAM or FIFO.
 * @param dev  - The IIO device descriptor.
 * @param mask - Active channel mask.
 * @return     - 0 in case of success or negative error code otherwise.
 */
static int adar300x_iio_pre_enable(void *dev, uint32_t mask)
{
	struct adar300x_iio_dev *iio = dev;
	uint32_t beam_mask;
	uint8_t beam;

	for (beam = 0; beam < ADAR300X_IIO_BEAMS; beam++) {
		beam_mask = mask & ADAR300X_IIO_BEAM_SCAN_MSK(beam);
		if (!beam_mask)
			continue;

		if (beam_mask != ADAR300X_IIO_BEAM_SCAN_MSK(beam))
			return -EINVAL;

		if (iio->load_mode[beam] != ADAR300X_IIO_MEMORY &&
		    iio->load_mode[beam] != ADAR300X_IIO_FIFO)
			return -EINVAL;
	}

	return 0;
}

/**
 * @brief Loads the pushed buffer into the RAM or FIFO of each active beam.
 *
 * Every scan carries one beamstate per active beam. In memory load mode scan n
 * lands in beamstate ram_index + n; in FIFO load mode each scan is queued.
 *
 * @param dev_data - The IIO device data.
 * @return	   - 0 in case of success or negative error code otherwise.
 */
static int adar300x_iio_submit(struct iio_device_data *dev_data)
{
	uint8_t scan[ADAR300X_IIO_BEAMSTATE_CHANNELS];
	struct adar300x_iio_dev *iio = dev_data->dev;
	struct iio_buffer *buffer = dev_data->buffer;
	uint32_t n, state;
	uint8_t beam, *values;
	int ret, i;

	for (n = 0; n < buffer->samples; n++) {
		ret = iio_buffer_pop_scan(buffer, scan);
		if (ret)
			return ret;

		values = scan;
		for (beam = 0; beam < ADAR300X_IIO_BEAMS; beam++) {
			if (!(buffer->active_mask &
			      ADAR300X_IIO_BEAM_SCAN_MSK(beam)))
				continue;

			/* Six of the eight storage bits are real, see scan_type */
			for (i = 0; i < ADAR300X_UNPACKED_BEAMSTATE_LEN; i++)
				values[i] &= ADAR300X_RAW_MSK;

			if (iio->load_mode[beam] == ADAR300X_IIO_MEMORY) {
				state = iio->ram_index[beam] + n;
				if (state >= ADAR300X_RAM_STATES_PER_BEAM)
					return -EINVAL;

				ret = adar300x_set_ram_beamstate(iio->adar300x_dev,
								 beam, state,
								 values);
			} else {
				ret = adar300x_load_fifo_beamstate(iio->adar300x_dev,
								   beam, values);
			}
			if (ret)
				return ret;

			values += ADAR300X_UNPACKED_BEAMSTATE_LEN;
		}
	}

	return 0;
}

static int adar300x_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	struct adar300x_iio_dev *iio = dev;
	uint8_t val;
	int ret;

	ret = adar300x_reg_read(iio->adar300x_dev, reg, &val);
	if (ret)
		return ret;

	*readval = val;

	return 0;
}

static int adar300x_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	struct adar300x_iio_dev *iio = dev;

	return adar300x_reg_write(iio->adar300x_dev, reg, writeval);
}

static struct scan_type adar300x_iio_scan_type = {
	.sign = 'u',
	.realbits = 6,
	.storagebits = 8,
	.shift = 0,
	.is_big_endian = false,
};

static struct iio_attribute adar300x_iio_beamstate_attrs[] = {
	{
		.name = "raw",
		.show = adar300x_iio_show_raw,
		.store = adar300x_iio_store_raw,
	},
	{
		.name = "scale",
		.show = adar300x_iio_show_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adar300x_iio_temp_attrs[] = {
	{
		.name = "raw",
		.show = adar300x_iio_show_raw,
	},
	{
		.name = "offset",
		.show = adar300x_iio_show_offset,
	},
	{
		.name = "scale",
		.show = adar300x_iio_show_scale,
	},
	END_ATTRIBUTES_ARRAY
};

/*
 * The address is the value's position within its beam's unpacked beamstate,
 * offset by the beam: delay then attenuation for each element in turn.
 */
#define ADAR300X_IIO_VALUE_CH(_type, _beam, _el, _param) {		\
	.ch_type = _type,						\
	.channel = (_beam) * ADAR300X_ELEMENTS_PER_BEAM + (_el),	\
	.address = (_beam) * ADAR300X_UNPACKED_BEAMSTATE_LEN +		\
		   (_el) * 2 + (_param),				\
	.scan_index = (_beam) * ADAR300X_UNPACKED_BEAMSTATE_LEN +	\
		      (_el) * 2 + (_param),				\
	.scan_type = &adar300x_iio_scan_type,				\
	.attributes = adar300x_iio_beamstate_attrs,			\
	.ch_out = true,							\
	.indexed = true,						\
}

#define ADAR300X_IIO_ELEMENT_CHS(_beam, _el)				\
	ADAR300X_IIO_VALUE_CH(IIO_PHASE, _beam, _el, ADAR300X_DELAY),	\
	ADAR300X_IIO_VALUE_CH(IIO_POWER, _beam, _el, ADAR300X_ATTENUATION)

#define ADAR300X_IIO_BEAM_CHS(_beam)					\
	ADAR300X_IIO_ELEMENT_CHS(_beam, 0),				\
	ADAR300X_IIO_ELEMENT_CHS(_beam, 1),				\
	ADAR300X_IIO_ELEMENT_CHS(_beam, 2),				\
	ADAR300X_IIO_ELEMENT_CHS(_beam, 3)

/* Channel order is scan order: the buffer mask bit of a channel is its index */
static struct iio_channel adar300x_iio_channels[] = {
	ADAR300X_IIO_BEAM_CHS(0),
	ADAR300X_IIO_BEAM_CHS(1),
	ADAR300X_IIO_BEAM_CHS(2),
	ADAR300X_IIO_BEAM_CHS(3),
	{
		.ch_type = IIO_TEMP,
		.channel = ADAR300X_IIO_BEAMS * ADAR300X_ELEMENTS_PER_BEAM,
		.attributes = adar300x_iio_temp_attrs,
		.indexed = true,
	},
};

#define ADAR300X_IIO_BEAM_ATTR(_beam, _suffix, _show, _store) {		\
	.name = "beam" #_beam "_" _suffix,				\
	.priv = _beam,							\
	.show = _show,							\
	.store = _store,						\
}

#define ADAR300X_IIO_BEAM_ATTRS(_beam)					\
	ADAR300X_IIO_BEAM_ATTR(_beam, "mode", adar300x_iio_show_mode,	\
			       adar300x_iio_store_mode),		\
	ADAR300X_IIO_BEAM_ATTR(_beam, "mode_available",			\
			       adar300x_iio_show_modes, NULL),		\
	ADAR300X_IIO_BEAM_ATTR(_beam, "update",				\
			       adar300x_iio_show_update,		\
			       adar300x_iio_store_update),		\
	ADAR300X_IIO_BEAM_ATTR(_beam, "load_mode",			\
			       adar300x_iio_show_load_mode,		\
			       adar300x_iio_store_load_mode),		\
	ADAR300X_IIO_BEAM_ATTR(_beam, "load_mode_available",		\
			       adar300x_iio_show_modes, NULL),		\
	ADAR300X_IIO_BEAM_ATTR(_beam, "ram_index",			\
			       adar300x_iio_show_ram_index,		\
			       adar300x_iio_store_ram_index),		\
	ADAR300X_IIO_BEAM_ATTR(_beam, "ram_start",			\
			       adar300x_iio_show_ram_start,		\
			       adar300x_iio_store_ram_start),		\
	ADAR300X_IIO_BEAM_ATTR(_beam, "ram_stop",			\
			       adar300x_iio_show_ram_stop,		\
			       adar300x_iio_store_ram_stop),		\
	ADAR300X_IIO_BEAM_ATTR(_beam, "fifo_rd",			\
			       adar300x_iio_show_fifo_rd, NULL),	\
	ADAR300X_IIO_BEAM_ATTR(_beam, "fifo_wr",			\
			       adar300x_iio_show_fifo_wr, NULL)

#define ADAR300X_IIO_AMP_ATTR(_bank, _idx, _field, _state_name, _el, _state) { \
	.name = #_bank #_idx "_amp_" #_field "_" _state_name,		\
	.priv = ADAR300X_IIO_AMP_PRIV(_el, _state, _idx),		\
	.show = adar300x_iio_show_amp_##_field,				\
	.store = adar300x_iio_store_amp_##_field,			\
}

#define ADAR300X_IIO_AMP_ATTRS(_bank, _idx, _state_name, _el, _state)	\
	ADAR300X_IIO_AMP_ATTR(_bank, _idx, bias, _state_name, _el, _state), \
	ADAR300X_IIO_AMP_ATTR(_bank, _idx, en, _state_name, _el, _state)

#define ADAR300X_IIO_BEAM_AMP_ATTRS(_beam)				\
	ADAR300X_IIO_AMP_ATTRS(beam, _beam, "reset", 0,			\
			       ADAR300X_BEAM_AMP_RESET),		\
	ADAR300X_IIO_AMP_ATTRS(beam, _beam, "operational", 0,		\
			       ADAR300X_BEAM_AMP_OPERATIONAL),		\
	ADAR300X_IIO_AMP_ATTRS(beam, _beam, "mute", 0,			\
			       ADAR300X_BEAM_AMP_MUTE),			\
	ADAR300X_IIO_AMP_ATTRS(beam, _beam, "sleep", 0,			\
			       ADAR300X_BEAM_AMP_SLEEP)

#define ADAR300X_IIO_ELEMENT_AMP_ATTRS(_el)				\
	ADAR300X_IIO_AMP_ATTRS(element, _el, "operational", 1,		\
			       ADAR300X_ELEMENT_AMP_OPERATIONAL),	\
	ADAR300X_IIO_AMP_ATTRS(element, _el, "sleep", 1,		\
			       ADAR300X_ELEMENT_AMP_SLEEP)

static struct iio_attribute adar300x_iio_attrs[] = {
	ADAR300X_IIO_BEAM_ATTRS(0),
	ADAR300X_IIO_BEAM_ATTRS(1),
	ADAR300X_IIO_BEAM_ATTRS(2),
	ADAR300X_IIO_BEAM_ATTRS(3),
	{
		.name = "update_intf_ctrl",
		.show = adar300x_iio_show_update_src,
		.store = adar300x_iio_store_update_src,
	},
	{
		.name = "update_intf_ctrl_available",
		.show = adar300x_iio_show_update_srcs,
	},
	ADAR300X_IIO_BEAM_AMP_ATTRS(0),
	ADAR300X_IIO_BEAM_AMP_ATTRS(1),
	ADAR300X_IIO_BEAM_AMP_ATTRS(2),
	ADAR300X_IIO_BEAM_AMP_ATTRS(3),
	ADAR300X_IIO_ELEMENT_AMP_ATTRS(0),
	ADAR300X_IIO_ELEMENT_AMP_ATTRS(1),
	ADAR300X_IIO_ELEMENT_AMP_ATTRS(2),
	ADAR300X_IIO_ELEMENT_AMP_ATTRS(3),
	END_ATTRIBUTES_ARRAY
};

static struct iio_device adar300x_iio_template = {
	.num_ch = NO_OS_ARRAY_SIZE(adar300x_iio_channels),
	.channels = adar300x_iio_channels,
	.attributes = adar300x_iio_attrs,
	.pre_enable = adar300x_iio_pre_enable,
	.submit = adar300x_iio_submit,
	.debug_reg_read = adar300x_iio_reg_read,
	.debug_reg_write = adar300x_iio_reg_write,
};

/**
 * @brief Initializes the ADAR300X IIO device.
 * @param iio_dev    - The IIO device descriptor.
 * @param init_param - The initialization parameters.
 * @return	     - 0 in case of success or negative error code otherwise.
 */
int adar300x_iio_init(struct adar300x_iio_dev **iio_dev,
		      struct adar300x_iio_dev_init_param *init_param)
{
	struct adar300x_iio_dev *desc;
	int ret;

	if (!iio_dev || !init_param || !init_param->adar300x_dev_init)
		return -EINVAL;

	desc = no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	ret = adar300x_init(&desc->adar300x_dev, init_param->adar300x_dev_init);
	if (ret)
		goto free_desc;

	/* Zeroed, every beam starts in direct mode, as the device does */
	desc->iio_dev = &adar300x_iio_template;
	*iio_dev = desc;

	return 0;

free_desc:
	no_os_free(desc);

	return ret;
}

/**
 * @brief Frees the resources allocated by adar300x_iio_init().
 * @param desc - The IIO device descriptor.
 * @return     - 0 in case of success or negative error code otherwise.
 */
int adar300x_iio_remove(struct adar300x_iio_dev *desc)
{
	int ret;

	if (!desc)
		return -ENODEV;

	ret = adar300x_remove(desc->adar300x_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
