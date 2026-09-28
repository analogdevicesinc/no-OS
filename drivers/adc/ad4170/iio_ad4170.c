/***************************************************************************//**
 *   @file   iio_ad4170.c
 *   @brief  Implementation of the AD417x and AD419x IIO driver.
 *   @author Janani Sunil (janani.sunil@analog.com)
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

#include <stdint.h>
#include "ad4170.h"
#include "iio_ad4170.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

#define AD4170_IIO_INT_CLOCK_HZ		16000000UL
#define AD4170_IIO_EXT_CLOCK_MIN_HZ	1000000UL
#define AD4170_IIO_EXT_CLOCK_MAX_HZ	17000000UL
#define AD4170_IIO_INT_REF_MV		2500
#define AD4170_IIO_FS_MULT		32
#define AD4170_IIO_FS_STEP		4
#define AD4170_IIO_SINC3_MIN_FS		4
#define AD4170_IIO_SINC3_MAX_FS		65532
#define AD4170_IIO_SINC5_MIN_FS		1
#define AD4170_IIO_SINC5_MAX_FS		256
#define AD4170_IIO_CONV_TIMEOUT		1000000
#define AD4170_IIO_REALBITS		24
#define AD4170_IIO_STORAGEBITS		32
#define AD4170_IIO_BYTE_MASK		0xff
#define AD4170_IIO_BYTE_BITS		8
#define AD4170_IIO_MAX_REG_ADDR		(AD4170_ADDR(AD4170_REG_INPUT_DATA) + \
		AD4170_TRANSF_LEN(AD4170_REG_INPUT_DATA) - 1)

static const uint32_t ad4170_iio_regs[] = {
	AD4170_REG_INTERFACE_CONFIG_A,
	AD4170_REG_INTERFACE_CONFIG_B,
	AD4170_REG_DEVICE_CONFIG,
	AD4170_REG_CHIP_TYPE,
	AD4170_REG_PRODUCT_ID_L,
	AD4170_REG_PRODUCT_ID_H,
	AD4170_REG_CHIP_GRADE,
	AD4170_REG_SCRATCH_PAD,
	AD4170_REG_SPI_REVISION,
	AD4170_REG_VENDOR_L,
	AD4170_REG_VENDOR_H,
	AD4170_REG_INTERFACE_CONFIG_C,
	AD4170_REG_INTERFACE_STATUS_A,
	AD4170_REG_DATA_STATUS,
	AD4170_REG_DATA_16b,
	AD4170_REG_DATA_16b_STATUS,
	AD4170_REG_DATA_24b,
	AD4170_REG_DATA_24b_STATUS,
	AD4170_REG_DATA_32b,
	AD4170_REG_DATA_PER_CHANNEL(0),
	AD4170_REG_DATA_PER_CHANNEL(1),
	AD4170_REG_DATA_PER_CHANNEL(2),
	AD4170_REG_DATA_PER_CHANNEL(3),
	AD4170_REG_DATA_PER_CHANNEL(4),
	AD4170_REG_DATA_PER_CHANNEL(5),
	AD4170_REG_DATA_PER_CHANNEL(6),
	AD4170_REG_DATA_PER_CHANNEL(7),
	AD4170_REG_DATA_PER_CHANNEL(8),
	AD4170_REG_DATA_PER_CHANNEL(9),
	AD4170_REG_DATA_PER_CHANNEL(10),
	AD4170_REG_DATA_PER_CHANNEL(11),
	AD4170_REG_DATA_PER_CHANNEL(12),
	AD4170_REG_DATA_PER_CHANNEL(13),
	AD4170_REG_DATA_PER_CHANNEL(14),
	AD4170_REG_DATA_PER_CHANNEL(15),
	AD4170_REG_PIN_MUXING,
	AD4170_REG_CLOCK_CTRL,
	AD4170_REG_STANDBY_CTRL,
	AD4170_REG_POWER_DOWN_SW,
	AD4170_REG_ADC_CTRL,
	AD4170_REG_ERROR_EN,
	AD4170_REG_ERROR,
	AD4170_REG_CHANNEL_EN,
	AD4170_REG_ADC_CHANNEL_SETUP(0),
	AD4170_REG_ADC_CHANNEL_MAP(0),
	AD4170_REG_ADC_CHANNEL_SETUP(1),
	AD4170_REG_ADC_CHANNEL_MAP(1),
	AD4170_REG_ADC_CHANNEL_SETUP(2),
	AD4170_REG_ADC_CHANNEL_MAP(2),
	AD4170_REG_ADC_CHANNEL_SETUP(3),
	AD4170_REG_ADC_CHANNEL_MAP(3),
	AD4170_REG_ADC_CHANNEL_SETUP(4),
	AD4170_REG_ADC_CHANNEL_MAP(4),
	AD4170_REG_ADC_CHANNEL_SETUP(5),
	AD4170_REG_ADC_CHANNEL_MAP(5),
	AD4170_REG_ADC_CHANNEL_SETUP(6),
	AD4170_REG_ADC_CHANNEL_MAP(6),
	AD4170_REG_ADC_CHANNEL_SETUP(7),
	AD4170_REG_ADC_CHANNEL_MAP(7),
	AD4170_REG_ADC_CHANNEL_SETUP(8),
	AD4170_REG_ADC_CHANNEL_MAP(8),
	AD4170_REG_ADC_CHANNEL_SETUP(9),
	AD4170_REG_ADC_CHANNEL_MAP(9),
	AD4170_REG_ADC_CHANNEL_SETUP(10),
	AD4170_REG_ADC_CHANNEL_MAP(10),
	AD4170_REG_ADC_CHANNEL_SETUP(11),
	AD4170_REG_ADC_CHANNEL_MAP(11),
	AD4170_REG_ADC_CHANNEL_SETUP(12),
	AD4170_REG_ADC_CHANNEL_MAP(12),
	AD4170_REG_ADC_CHANNEL_SETUP(13),
	AD4170_REG_ADC_CHANNEL_MAP(13),
	AD4170_REG_ADC_CHANNEL_SETUP(14),
	AD4170_REG_ADC_CHANNEL_MAP(14),
	AD4170_REG_ADC_CHANNEL_SETUP(15),
	AD4170_REG_ADC_CHANNEL_MAP(15),
	AD4170_REG_ADC_SETUPS_MISC(0),
	AD4170_REG_ADC_SETUPS_AFE(0),
	AD4170_REG_ADC_SETUPS_FILTER(0),
	AD4170_REG_ADC_SETUPS_FILTER_FS(0),
	AD4170_REG_ADC_SETUPS_OFFSET(0),
	AD4170_REG_ADC_SETUPS_GAIN(0),
	AD4170_REG_ADC_SETUPS_MISC(1),
	AD4170_REG_ADC_SETUPS_AFE(1),
	AD4170_REG_ADC_SETUPS_FILTER(1),
	AD4170_REG_ADC_SETUPS_FILTER_FS(1),
	AD4170_REG_ADC_SETUPS_OFFSET(1),
	AD4170_REG_ADC_SETUPS_GAIN(1),
	AD4170_REG_ADC_SETUPS_MISC(2),
	AD4170_REG_ADC_SETUPS_AFE(2),
	AD4170_REG_ADC_SETUPS_FILTER(2),
	AD4170_REG_ADC_SETUPS_FILTER_FS(2),
	AD4170_REG_ADC_SETUPS_OFFSET(2),
	AD4170_REG_ADC_SETUPS_GAIN(2),
	AD4170_REG_ADC_SETUPS_MISC(3),
	AD4170_REG_ADC_SETUPS_AFE(3),
	AD4170_REG_ADC_SETUPS_FILTER(3),
	AD4170_REG_ADC_SETUPS_FILTER_FS(3),
	AD4170_REG_ADC_SETUPS_OFFSET(3),
	AD4170_REG_ADC_SETUPS_GAIN(3),
	AD4170_REG_ADC_SETUPS_MISC(4),
	AD4170_REG_ADC_SETUPS_AFE(4),
	AD4170_REG_ADC_SETUPS_FILTER(4),
	AD4170_REG_ADC_SETUPS_FILTER_FS(4),
	AD4170_REG_ADC_SETUPS_OFFSET(4),
	AD4170_REG_ADC_SETUPS_GAIN(4),
	AD4170_REG_ADC_SETUPS_MISC(5),
	AD4170_REG_ADC_SETUPS_AFE(5),
	AD4170_REG_ADC_SETUPS_FILTER(5),
	AD4170_REG_ADC_SETUPS_FILTER_FS(5),
	AD4170_REG_ADC_SETUPS_OFFSET(5),
	AD4170_REG_ADC_SETUPS_GAIN(5),
	AD4170_REG_ADC_SETUPS_MISC(6),
	AD4170_REG_ADC_SETUPS_AFE(6),
	AD4170_REG_ADC_SETUPS_FILTER(6),
	AD4170_REG_ADC_SETUPS_FILTER_FS(6),
	AD4170_REG_ADC_SETUPS_OFFSET(6),
	AD4170_REG_ADC_SETUPS_GAIN(6),
	AD4170_REG_ADC_SETUPS_MISC(7),
	AD4170_REG_ADC_SETUPS_AFE(7),
	AD4170_REG_ADC_SETUPS_FILTER(7),
	AD4170_REG_ADC_SETUPS_FILTER_FS(7),
	AD4170_REG_ADC_SETUPS_OFFSET(7),
	AD4170_REG_ADC_SETUPS_GAIN(7),
	AD4170_REG_REF_CONTROL,
	AD4170_REG_V_BIAS,
	AD4170_REG_I_PULLUP,
	AD4170_REG_CURRENT_SOURCE(0),
	AD4170_REG_CURRENT_SOURCE(1),
	AD4170_REG_CURRENT_SOURCE(2),
	AD4170_REG_CURRENT_SOURCE(3),
	AD4170_REG_FIR_CONTROL,
	AD4170_REG_COEFF_WRITE_DATA,
	AD4170_REG_COEFF_READ_DATA,
	AD4170_REG_COEFF_ADDRESS,
	AD4170_REG_COEFF_WRRD_STB,
	AD4170_REG_DAC_SPAN,
	AD4170_REG_DAC_CHANNEL_EN,
	AD4170_REG_DAC_HW_TOGGLE_MASK,
	AD4170_REG_DAC_HW_LDAC_MASK,
	AD4170_REG_DAC_DATA(0),
	AD4170_REG_DAC_SW_TOGGLE_TRIGGERS,
	AD4170_REG_DAC_SW_LDAC_TRIGGERS,
	AD4170_REG_DAC_INPUTA(0),
	AD4170_REG_DAC_INPUTB(0),
	AD4170_REG_GPIO_MODE,
	AD4170_REG_OUTPUT_DATA,
	AD4170_REG_INPUT_DATA,
};

/* Base-2 logarithm of the gain each ad4170_pga_gain setting selects. */
static const int8_t ad4170_iio_pga_gain_log2[] = {
	[AD4170_PGA_GAIN_1] = 0,
	[AD4170_PGA_GAIN_2] = 1,
	[AD4170_PGA_GAIN_4] = 2,
	[AD4170_PGA_GAIN_8] = 3,
	[AD4170_PGA_GAIN_16] = 4,
	[AD4170_PGA_GAIN_32] = 5,
	[AD4170_PGA_GAIN_64] = 6,
	[AD4170_PGA_GAIN_128] = 7,
	[AD4170_PGA_GAIN_0P5] = -1,
	[AD4170_PGA_GAIN_1_PRECHARGE] = 0,
};

static const char * const ad4170_iio_chan_names[AD4170_NUM_CHANNELS] = {
	"ch0", "ch1", "ch2", "ch3", "ch4", "ch5", "ch6", "ch7",
	"ch8", "ch9", "ch10", "ch11", "ch12", "ch13", "ch14", "ch15"
};

/**
 * @brief Get the master clock frequency the digital filter runs from.
 * @param iio_ad4170 - The IIO device structure.
 * @return The clock frequency in Hz.
 */
static uint32_t ad4170_iio_fclk_hz(struct ad4170_iio_dev *iio_ad4170)
{
	struct ad4170_config *config = &iio_ad4170->ad4170_dev->config;

	return iio_ad4170->mclk_hz >> config->clock_ctrl.clockdiv;
}

/**
 * @brief Get the reference voltage used by the setup of a given channel.
 * @param iio_ad4170 - The IIO device structure.
 * @param ch - Sequencer channel number.
 * @return The reference voltage in mV.
 */
static uint32_t ad4170_iio_vref_mv(struct ad4170_iio_dev *iio_ad4170,
				   uint8_t ch)
{
	struct ad4170_config *config = &iio_ad4170->ad4170_dev->config;
	uint8_t setup_n = config->setup[ch].setup_n;

	if (config->setups[setup_n].afe.ref_select == AD4170_REFIN_REFOUT)
		return AD4170_IIO_INT_REF_MV;

	return iio_ad4170->vref_mv;
}

/**
 * @brief Get the right shift turning an ADC code into a value in mV units.
 * @param iio_ad4170 - The IIO device structure.
 * @param ch - Sequencer channel number.
 * @return The number of bits to shift the reference voltage by.
 */
static uint8_t ad4170_iio_scale_shift(struct ad4170_iio_dev *iio_ad4170,
				      uint8_t ch)
{
	struct ad4170_config *config = &iio_ad4170->ad4170_dev->config;
	uint8_t setup_n = config->setup[ch].setup_n;
	struct ad4170_afe *afe = &config->setups[setup_n].afe;
	struct iio_channel *chan = &iio_ad4170->iio_dev->channels[ch];
	uint8_t realbits = chan->scan_type->realbits;

	return realbits - afe->bipolar +
	       ad4170_iio_pga_gain_log2[afe->pga_gain];
}

/**
 * @brief Wait for a conversion result to be available on every enabled channel.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad4170_iio_wait_for_conv(struct ad4170_dev *dev)
{
	uint32_t timeout = AD4170_IIO_CONV_TIMEOUT;
	uint16_t status;
	int ret;

	while (timeout--) {
		ret = ad4170_get_status(dev, &status);
		if (ret)
			return ret;

		if (!(status & AD4170_DATA_STATUS_RDYB_MSK))
			return 0;
	}

	return -ETIMEDOUT;
}

/**
 * @brief Run a single conversion on one channel and read its result.
 * @param dev - The device structure.
 * @param ch - Sequencer channel to convert.
 * @param code - Pointer to the location where the raw code is stored.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad4170_iio_convert_once(struct ad4170_dev *dev, uint8_t ch,
				   uint32_t *code)
{
	uint32_t timeout = AD4170_IIO_CONV_TIMEOUT;
	struct ad4170_adc_ctrl adc_ctrl;
	uint8_t status;
	int ret;

	adc_ctrl = dev->config.adc_ctrl;
	adc_ctrl.mode = AD4170_MODE_IDLE;
	adc_ctrl.multi_data_reg_sel = false;
	ret = ad4170_set_adc_ctrl(dev, adc_ctrl);
	if (ret)
		return ret;

	/* Convert the requested channel alone so that RDYB refers to it. */
	ret = ad4170_set_channel_en(dev, AD4170_CHANNEL(ch));
	if (ret)
		return ret;

	/*
	 * Flush: a conversion the previous configuration left unread keeps
	 * RDYB asserted, which the poll below would take for this result.
	 * Reading the data register re-arms the flag.
	 */
	ret = ad4170_get_data24s(dev, code, &status);
	if (ret)
		return ret;

	/* No mode but the continuous ones drives RDYB low on its own. */
	adc_ctrl.mode = AD4170_MODE_SINGLE;

	ret = ad4170_set_adc_ctrl(dev, adc_ctrl);
	if (ret)
		return ret;

	do {
		ret = ad4170_get_data24s(dev, code, &status);
		if (ret)
			return ret;
	} while ((status & AD4170_DATA_STATUS_RDYB_MSK) && --timeout);

	if (!timeout)
		return -ETIMEDOUT;

	return 0;
}

/**
 * @brief Handles the read request for the raw attribute.
 * @param dev - The IIO device structure.
 * @param buf - Command buffer to be filled with the requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Command attribute id.
 * @return The size of the read data in case of success, negative error code
 *	   otherwise.
 */
static int ad4170_iio_read_raw(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad4170_iio_dev *iio_ad4170 = (struct ad4170_iio_dev *)dev;
	struct scan_type *scan_type;
	struct ad4170_adc_ctrl prev_adc_ctrl;
	struct ad4170_dev *ad4170;
	uint16_t prev_channel_en;
	int restore_ret;
	uint32_t code;
	int32_t val;
	int ret;

	if (!iio_ad4170 || !iio_ad4170->ad4170_dev)
		return -EINVAL;

	ad4170 = iio_ad4170->ad4170_dev;
	scan_type = iio_ad4170->iio_dev->channels[channel->address].scan_type;
	prev_channel_en = ad4170->config.channel_en;
	prev_adc_ctrl = ad4170->config.adc_ctrl;

	ret = ad4170_iio_convert_once(ad4170, channel->address, &code);

	restore_ret = ad4170_set_adc_ctrl(ad4170, prev_adc_ctrl);
	if (!ret)
		ret = restore_ret;

	restore_ret = ad4170_set_channel_en(ad4170, prev_channel_en);
	if (!ret)
		ret = restore_ret;

	if (ret)
		return ret;

	if (scan_type->sign == 's')
		val = no_os_sign_extend32(code, scan_type->realbits - 1);
	else
		val = code;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Determine the scale attribute.
 * @param dev - The IIO device structure.
 * @param buf - Command buffer to be filled with the requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Command attribute id.
 * @return The size of the read data in case of success, negative error code
 *	   otherwise.
 */
static int ad4170_iio_read_scale(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad4170_iio_dev *iio_ad4170 = (struct ad4170_iio_dev *)dev;
	int32_t vals[2];

	if (!iio_ad4170 || !iio_ad4170->ad4170_dev)
		return -EINVAL;

	vals[0] = ad4170_iio_vref_mv(iio_ad4170, channel->address);
	vals[1] = ad4170_iio_scale_shift(iio_ad4170, channel->address);

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, vals);
}

/**
 * @brief Handles the read request for the sampling_frequency attribute.
 * @param dev - The IIO device structure.
 * @param buf - Command buffer to be filled with the requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Command attribute id.
 * @return The size of the read data in case of success, negative error code
 *	   otherwise.
 */
static int ad4170_iio_read_samp_freq(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct ad4170_iio_dev *iio_ad4170 = (struct ad4170_iio_dev *)dev;
	struct ad4170_config *config;
	int32_t vals[2];
	uint8_t setup_n;

	if (!iio_ad4170 || !iio_ad4170->ad4170_dev)
		return -EINVAL;

	config = &iio_ad4170->ad4170_dev->config;
	setup_n = config->setup[channel->address].setup_n;

	vals[0] = ad4170_iio_fclk_hz(iio_ad4170);
	vals[1] = AD4170_IIO_FS_MULT * config->setups[setup_n].filter_fs;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
}

/**
 * @brief Handles the write request for the sampling_frequency attribute.
 * @param dev - The IIO device structure.
 * @param buf - Command buffer holding the value to be written.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Command attribute id.
 * @return The size of the written data in case of success, negative error code
 *	   otherwise.
 */
static int ad4170_iio_write_samp_freq(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct ad4170_iio_dev *iio_ad4170 = (struct ad4170_iio_dev *)dev;
	struct ad4170_config *config;
	struct ad4170_setup setup;
	uint32_t filter_fs, fs_max;
	uint32_t freq;
	uint8_t setup_n;
	int ret;

	if (!iio_ad4170 || !iio_ad4170->ad4170_dev)
		return -EINVAL;

	freq = no_os_str_to_uint32(buf);
	if (!freq)
		return -EINVAL;

	config = &iio_ad4170->ad4170_dev->config;
	setup_n = config->setup[channel->address].setup_n;
	setup = config->setups[setup_n];

	fs_max = setup.filter.filter_type == AD4170_FILT_SINC5 ?
		 AD4170_IIO_SINC5_MAX_FS : AD4170_IIO_SINC3_MAX_FS;

	freq = no_os_min(freq, ad4170_iio_fclk_hz(iio_ad4170) /
			 AD4170_IIO_FS_MULT);

	filter_fs = NO_OS_DIV_ROUND_CLOSEST(ad4170_iio_fclk_hz(iio_ad4170),
					    AD4170_IIO_FS_MULT * freq);

	/*
	 * Plain sinc5 also takes FILTER_FS 1 and 2, which are its top two ODRs;
	 * the other filters start at 4. From 4 upwards all of them take
	 * multiples of 4 only, so FILTER_FS 3 rounds up to 4.
	 */
	if (setup.filter.filter_type == AD4170_FILT_SINC5 &&
	    filter_fs < AD4170_IIO_FS_STEP - 1)
		filter_fs = no_os_max(filter_fs, AD4170_IIO_SINC5_MIN_FS);
	else
		filter_fs = no_os_clamp(filter_fs & ~(AD4170_IIO_FS_STEP - 1UL),
					AD4170_IIO_SINC3_MIN_FS, fs_max);

	setup.filter_fs = filter_fs;

	ret = ad4170_set_setup(iio_ad4170->ad4170_dev, setup_n, setup);
	if (ret)
		return ret;

	return len;
}

/**
 * @brief Find the register holding a given byte address.
 * @param addr - Byte address to look up.
 * @param offset - Pointer where the byte position in the register is stored,
 * 		   counted from the least significant byte.
 * @return The register address, encoded with its transfer length.
 */
static uint32_t ad4170_iio_reg_search(uint32_t addr, uint32_t *offset)
{
	uint32_t reg;
	uint32_t i;

	*offset = 0;

	for (i = 0; i < NO_OS_ARRAY_SIZE(ad4170_iio_regs); i++) {
		if (addr == AD4170_ADDR(ad4170_iio_regs[i]))
			return ad4170_iio_regs[i];

		if (addr < AD4170_ADDR(ad4170_iio_regs[i]))
			break;
	}

	if (i) {
		reg = ad4170_iio_regs[i - 1];
		if (addr - AD4170_ADDR(reg) < AD4170_TRANSF_LEN(reg)) {
			*offset = addr - AD4170_ADDR(reg);
			return reg;
		}
	}

	return AD4170_R1B | addr;
}

/**
 * @brief Read a device register.
 * @param dev - The IIO device structure.
 * @param reg - Byte address of the register.
 * @param readval - Pointer where the byte read is stored.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t ad4170_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	struct ad4170_iio_dev *iio_ad4170 = (struct ad4170_iio_dev *)dev;
	uint32_t offset;
	uint32_t base;
	int ret;

	if (!iio_ad4170 || !iio_ad4170->ad4170_dev || !readval)
		return -EINVAL;

	if (reg > AD4170_IIO_MAX_REG_ADDR)
		return -EINVAL;

	base = ad4170_iio_reg_search(reg, &offset);

	ret = ad4170_spi_reg_read(iio_ad4170->ad4170_dev, base, readval);
	if (ret)
		return ret;

	*readval >>= offset * AD4170_IIO_BYTE_BITS;
	*readval &= AD4170_IIO_BYTE_MASK;

	return 0;
}

/**
 * @brief Write a device register.
 * @param dev - The IIO device structure.
 * @param reg - Byte address of the register.
 * @param writeval - Byte to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t ad4170_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	struct ad4170_iio_dev *iio_ad4170 = (struct ad4170_iio_dev *)dev;
	uint32_t offset;
	uint32_t base;
	uint32_t val;
	int ret;

	if (!iio_ad4170 || !iio_ad4170->ad4170_dev)
		return -EINVAL;

	if (reg > AD4170_IIO_MAX_REG_ADDR)
		return -EINVAL;

	base = ad4170_iio_reg_search(reg, &offset);

	ret = ad4170_spi_reg_read(iio_ad4170->ad4170_dev, base, &val);
	if (ret)
		return ret;

	val &= ~(AD4170_IIO_BYTE_MASK << (offset * AD4170_IIO_BYTE_BITS));
	val |= (writeval & AD4170_IIO_BYTE_MASK) << (offset *
			AD4170_IIO_BYTE_BITS);

	return ad4170_spi_reg_write(iio_ad4170->ad4170_dev, base, val);
}

/**
 * @brief Prepare the device for a buffered read.
 * @param dev - The IIO device structure.
 * @param mask - Mask of the channels enabled by the buffer.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t ad4170_iio_update_channels(void *dev, uint32_t mask)
{
	struct ad4170_iio_dev *iio_ad4170 = (struct ad4170_iio_dev *)dev;
	struct ad4170_adc_ctrl adc_ctrl;
	int ret;

	if (!iio_ad4170 || !iio_ad4170->ad4170_dev)
		return -EINVAL;

	adc_ctrl = iio_ad4170->ad4170_dev->config.adc_ctrl;

	iio_ad4170->active_channels = mask;
	iio_ad4170->num_active_channels = no_os_hweight32(mask);
	iio_ad4170->prev_channel_en = iio_ad4170->ad4170_dev->config.channel_en;
	iio_ad4170->prev_mode = adc_ctrl.mode;
	iio_ad4170->prev_multi_data_reg_sel = adc_ctrl.multi_data_reg_sel;
	ret = ad4170_set_channel_en(iio_ad4170->ad4170_dev, mask);
	if (ret)
		return ret;

	adc_ctrl.multi_data_reg_sel = false;
	adc_ctrl.mode = AD4170_MODE_CONT;

	return ad4170_set_adc_ctrl(iio_ad4170->ad4170_dev, adc_ctrl);
}

/**
 * @brief Restore the configuration the device had before the buffered read.
 * @param dev - The IIO device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t ad4170_iio_post_disable(void *dev)
{
	struct ad4170_iio_dev *iio_ad4170 = (struct ad4170_iio_dev *)dev;
	struct ad4170_adc_ctrl adc_ctrl;
	int ret;

	if (!iio_ad4170 || !iio_ad4170->ad4170_dev)
		return -EINVAL;

	adc_ctrl = iio_ad4170->ad4170_dev->config.adc_ctrl;
	adc_ctrl.mode = iio_ad4170->prev_mode;
	adc_ctrl.multi_data_reg_sel = iio_ad4170->prev_multi_data_reg_sel;

	ret = ad4170_set_adc_ctrl(iio_ad4170->ad4170_dev, adc_ctrl);
	if (ret)
		return ret;

	return ad4170_set_channel_en(iio_ad4170->ad4170_dev,
				     iio_ad4170->prev_channel_en);
}

/**
 * @brief Fill the IIO buffer with samples read from the per channel data
 *	  registers.
 * @param iio_dev_data - The IIO device data structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t ad4170_iio_submit_buffer(struct iio_device_data *iio_dev_data)
{
	struct ad4170_iio_dev *iio_ad4170;
	struct iio_device *iio_dev;
	struct iio_buffer *buffer;
	uint32_t *push_data;
	uint32_t total_samples;
	uint32_t sample = 0;
	uint32_t code;
	uint16_t i;
	void *block;
	int ret;

	if (!iio_dev_data || !iio_dev_data->dev)
		return -EINVAL;

	iio_ad4170 = (struct ad4170_iio_dev *)iio_dev_data->dev;
	iio_dev = iio_ad4170->iio_dev;
	buffer = iio_dev_data->buffer;

	ret = iio_buffer_get_block(buffer, &block);
	if (ret)
		return ret;

	push_data = (uint32_t *)block;

	total_samples = buffer->samples * iio_ad4170->num_active_channels;

	while (sample < total_samples) {
		ret = ad4170_iio_wait_for_conv(iio_ad4170->ad4170_dev);
		if (ret)
			return ret;

		for (i = 0; i < iio_dev->num_ch; i++) {
			if (!(iio_ad4170->active_channels & NO_OS_BIT(i)))
				continue;

			ret = ad4170_get_ch_data(iio_ad4170->ad4170_dev,
						 iio_dev->channels[i].address,
						 &code);
			if (ret)
				return ret;

			push_data[sample++] = code;
		}
	}

	return iio_buffer_block_done(buffer);
}

static struct iio_attribute ad4170_iio_ch_attrs[] = {
	{
		.name = "raw",
		.show = ad4170_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ad4170_iio_read_scale,
	},
	{
		.name = "sampling_frequency",
		.show = ad4170_iio_read_samp_freq,
		.store = ad4170_iio_write_samp_freq,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device ad4170_iio_dev_template = {
	.pre_enable = ad4170_iio_update_channels,
	.post_disable = ad4170_iio_post_disable,
	.submit = ad4170_iio_submit_buffer,
	.debug_reg_read = ad4170_iio_reg_read,
	.debug_reg_write = ad4170_iio_reg_write,
};

/**
 * @brief Describe one IIO channel per sequencer channel.
 *
 * Every sequencer channel gets an IIO channel regardless of the Channel_En
 * register contents, so that the channels the ADC converts can be picked at
 * buffer enable or at raw read time.
 *
 * @param chans - Array of AD4170_NUM_CHANNELS channels.
 * @param scan_types - Array of AD4170_NUM_CHANNELS scan types.
 * @param config - The device configuration the channels are described from.
 */
static void ad4170_iio_fill_channels(struct iio_channel *chans,
				     struct scan_type *scan_types,
				     struct ad4170_config *config)
{
	uint8_t ch;

	for (ch = 0; ch < AD4170_NUM_CHANNELS; ch++) {
		uint8_t setup_n = config->setup[ch].setup_n;

		/* Only a bipolar setup spends a bit on the sign. */
		scan_types[ch].sign = config->setups[setup_n].afe.bipolar ?
				      's' : 'u';
		scan_types[ch].realbits = AD4170_IIO_REALBITS;
		scan_types[ch].storagebits = AD4170_IIO_STORAGEBITS;
		scan_types[ch].is_big_endian = false;

		chans[ch].name = ad4170_iio_chan_names[ch];
		chans[ch].ch_type = IIO_VOLTAGE;
		chans[ch].channel = ch;
		chans[ch].address = ch;
		chans[ch].scan_index = ch;
		chans[ch].scan_type = &scan_types[ch];
		chans[ch].attributes = ad4170_iio_ch_attrs;
		chans[ch].indexed = true;
	}
}

/**
 * @brief Write the mux mapping and setup assignment of every sequencer channel.
 * @param dev - The device structure.
 * @param config - The device configuration to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad4170_iio_write_channels(struct ad4170_dev *dev,
				     struct ad4170_config *config)
{
	uint8_t ch;
	int ret;

	for (ch = 0; ch < AD4170_NUM_CHANNELS; ch++) {
		if (config->channel_en & AD4170_CHANNEL(ch))
			continue;

		ret = ad4170_set_channel_setup(dev, ch, config->setup[ch]);
		if (ret)
			return ret;

		ret = ad4170_set_channel_map(dev, ch, config->map[ch]);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Initialize the AD4170 IIO driver.
 * @param device - The IIO device structure.
 * @param init_param - The structure holding the device initial parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_iio_init(struct ad4170_iio_dev **device,
		    struct ad4170_iio_dev_init_param *init_param)
{
	struct ad4170_config *config;
	struct ad4170_iio_dev *desc;
	struct scan_type *scan_types;
	struct iio_channel *chans;
	uint32_t mclk_hz;
	int ret;

	if (!device || !init_param || !init_param->ad4170_init_param) {
		pr_err("ad4170 iio: missing init parameters\r\n");
		return -EINVAL;
	}

	config = &init_param->ad4170_init_param->config;

	switch (config->clock_ctrl.clocksel) {
	case AD4170_INTERNAL_OSC:
	case AD4170_INTERNAL_OSC_OUTPUT:
		/* The internal oscillator runs at a fixed rate. */
		mclk_hz = AD4170_IIO_INT_CLOCK_HZ;
		break;
	default:
		mclk_hz = init_param->mclk_hz;
		if (mclk_hz < AD4170_IIO_EXT_CLOCK_MIN_HZ ||
		    mclk_hz > AD4170_IIO_EXT_CLOCK_MAX_HZ) {
			pr_err("ad4170 iio: external clock %lu Hz "
			       "out of range\r\n", (unsigned long)mclk_hz);
			return -EINVAL;
		}
		break;
	}

	desc = (struct ad4170_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = (struct iio_device *)no_os_calloc(1,
			sizeof(*desc->iio_dev));
	if (!desc->iio_dev) {
		ret = -ENOMEM;
		goto error_desc;
	}

	chans = (struct iio_channel *)no_os_calloc(AD4170_NUM_CHANNELS,
			sizeof(*chans));
	if (!chans) {
		ret = -ENOMEM;
		goto error_iio_dev;
	}

	scan_types = (struct scan_type *)no_os_calloc(AD4170_NUM_CHANNELS,
			sizeof(*scan_types));
	if (!scan_types) {
		ret = -ENOMEM;
		goto error_chans;
	}

	ad4170_iio_fill_channels(chans, scan_types, config);

	*desc->iio_dev = ad4170_iio_dev_template;
	desc->iio_dev->num_ch = AD4170_NUM_CHANNELS;
	desc->iio_dev->channels = chans;
	desc->scan_types = scan_types;
	desc->vref_mv = init_param->vref_mv;
	desc->mclk_hz = mclk_hz;

	ret = ad4170_init(&desc->ad4170_dev, init_param->ad4170_init_param);
	if (ret)
		goto error_scan_types;

	pr_info("ad4170 iio: clock %lu Hz, reference %lu mV\r\n",
		(unsigned long)ad4170_iio_fclk_hz(desc),
		(unsigned long)ad4170_iio_vref_mv(desc, 0));

	ret = ad4170_iio_write_channels(desc->ad4170_dev, config);
	if (ret)
		goto error_dev;

	*device = desc;

	return 0;

error_dev:
	ad4170_remove(desc->ad4170_dev);
error_scan_types:
	no_os_free(scan_types);
error_chans:
	no_os_free(chans);
error_iio_dev:
	no_os_free(desc->iio_dev);
error_desc:
	no_os_free(desc);

	pr_err("ad4170 iio: init failed with status %d\r\n", ret);

	return ret;
}

/**
 * @brief Free the resources allocated by ad4170_iio_init().
 * @param device - The IIO device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_iio_remove(struct ad4170_iio_dev *device)
{
	int ret;

	if (!device)
		return -EINVAL;

	ret = ad4170_remove(device->ad4170_dev);

	no_os_free(device->scan_types);
	if (device->iio_dev)
		no_os_free(device->iio_dev->channels);
	no_os_free(device->iio_dev);
	no_os_free(device);

	return ret;
}
