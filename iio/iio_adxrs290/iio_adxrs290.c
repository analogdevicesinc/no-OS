/***************************************************************************//**
 *   @file   iio_adxrs290.c
 *   @brief  Implementation of ADXRS290 iio.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
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

#include "iio_types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "adxrs290.h"
#include "util.h"
#include "error.h"

/*
 * Available cut-off frequencies of the low pass filter in Hz.
 * The integer part and fractional part are represented separately.
 */
static const int adxrs290_lpf_3db_freq_hz_table[][2] = {
	[0] = {480, 0},
	[1] = {320, 0},
	[2] = {160, 0},
	[3] = {80, 0},
	[4] = {56, 600000},
	[5] = {40, 0},
	[6] = {28, 300000},
	[7] = {20, 0},
};

/*
 * Available cut-off frequencies of the high pass filter in Hz.
 * The integer part and fractional part are represented separately.
 */
static const int adxrs290_hpf_3db_freq_hz_table[][2] = {
	[0] = {0, 0},
	[1] = {0, 11000},
	[2] = {0, 22000},
	[3] = {0, 44000},
	[4] = {0, 87000},
	[5] = {0, 175000},
	[6] = {0, 350000},
	[7] = {0, 700000},
	[8] = {1, 400000},
	[9] = {2, 800000},
	[10] = {11, 300000},
};

ssize_t get_adxrs290_iio_ch_raw(void *device, char *buf, size_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	int16_t data;

	adxrs290_get_rate_data((struct adxrs290_dev *)device,
			       channel->ch_num, &data);
	if (channel->ch_num == ADXRS290_CHANNEL_TEMP)
		data = (data << 4) >> 4;

	return snprintf(buf, len, "%d", data);
}


ssize_t get_adxrs290_iio_ch_scale(void *device, char *buf, size_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	if (channel->ch_num == ADXRS290_CHANNEL_TEMP)
		// Temperature scale 1 LSB = 0.1 degree Celsius
		return snprintf(buf, len, "100");

	// Angular velocity scale 1 LSB = 0.005 degrees/sec = 0.000087266 rad/sec
	return snprintf(buf, len, "0.000087266");
}

ssize_t get_adxrs290_iio_ch_hpf(void *device, char *buf, size_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	uint8_t index;
	adxrs290_get_hpf((struct adxrs290_dev *)device, &index);
	if (index > 0x0A)
		index = 0x0A;

	return snprintf(buf, len, "%d.%d",
			adxrs290_hpf_3db_freq_hz_table[index][0],
			adxrs290_hpf_3db_freq_hz_table[index][1]);
}

ssize_t set_adxrs290_iio_ch_hpf(void *device, char *buf, size_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	float hpf = strtof(buf, NULL);
	int32_t val = (int32_t)hpf;
	int32_t val2 = (int32_t)(hpf * 1000000) % 1000000;
	uint8_t i;
	uint8_t n = ARRAY_SIZE(adxrs290_hpf_3db_freq_hz_table);

	for (i = 0; i < n; i++)
		if (adxrs290_hpf_3db_freq_hz_table[i][0] == val
		    && adxrs290_hpf_3db_freq_hz_table[i][1] == val2) {
			adxrs290_set_hpf(device, (enum adxrs290_hpf) i);

			return len;
		}

	return FAILURE;
}

ssize_t get_adxrs290_iio_ch_lpf(void *device, char *buf, size_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	uint8_t index;

	adxrs290_get_lpf((struct adxrs290_dev *)device, &index);
	if (index > 0x07)
		index = 0x07;

	return snprintf(buf, len, "%d.%d", adxrs290_lpf_3db_freq_hz_table[index][0],
			adxrs290_lpf_3db_freq_hz_table[index][1]);
}

ssize_t set_adxrs290_iio_ch_lpf(void *device, char *buf, size_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	float lpf = strtof(buf, NULL);
	int32_t val = (int32_t)lpf;
	int32_t val2 = (int32_t)(lpf * 1000000) % 1000000;
	uint8_t i;
	uint8_t n = ARRAY_SIZE(adxrs290_lpf_3db_freq_hz_table);

	for (i = 0; i < n; i++)
		if (adxrs290_lpf_3db_freq_hz_table[i][0] == val
		    && adxrs290_lpf_3db_freq_hz_table[i][1] == val2) {
			adxrs290_set_lpf(device, (enum adxrs290_lpf) i);
			return len;
		}

	return FAILURE;
}

int32_t adxrs290_update_active_channels(void *device, uint32_t mask)
{
	struct adxrs290_dev *dev = device;

	adxrs290_set_active_channels(dev, mask);

	return SUCCESS;
}

int32_t adxrs290_read_samples(void *device, uint16_t *buff, uint32_t nb_samples)
{
	struct adxrs290_dev	*dev = device;
	uint32_t		i;
	uint32_t		offset;
	int16_t			data[ADXRS290_CHANNEL_COUNT];
	uint8_t			ch_cnt;
	bool			rdy;

	offset = 0;
	for (i = 0; i < nb_samples; i++) {
		/* Stop until data is available.
		 * This will not block at first data since sync pin will
		 * will always be high until read. */
		while(true) {
			adxrs290_get_data_ready(dev, &rdy);
			if (rdy == true)
				break;
		}
		adxrs290_get_burst_data(dev, data, &ch_cnt);
		memcpy(&buff[offset], data, ch_cnt*sizeof(int16_t));
		offset += ch_cnt;
	}

	return nb_samples;
}
