// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for AD9088 and similar mixed signal front end (MxFE®)
 *
 * Copyright 2024 Analog Devices Inc.
 */

#include "ad9088.h"

//#define DEBUG

// #include <linux/kernel.h>
// #include <linux/module.h>
// #include <linux/mod_devicetable.h>
// #include <linux/spi/spi.h>
// #include <linux/delay.h>
// #include <linux/wait.h>

// #include <linux/iio/iio.h>
// #include <linux/iio/buffer.h>
// #include <linux/iio/kfifo_buf.h>

#include "adi_apollo_bf_rx_spectrum_sniffer.h"

#define AD9088_FFT_SNIFFER_SI_INDEX 0
#define AD9088_FFT_SNIFFER_I_INDEX 1
#define AD9088_FFT_SNIFFER_Q_INDEX 2
#define AD9088_FFT_SNIFFER_MAGN_INDEX 3
#define AD9088_FFT_SNIFFER_CHAN_MAX 4

// static const unsigned long ad9088_fft_sniffer_available_scan_masks[] = {
// 	BIT(AD9088_FFT_SNIFFER_SI_INDEX) | BIT(AD9088_FFT_SNIFFER_I_INDEX) | BIT(AD9088_FFT_SNIFFER_Q_INDEX),
// 	BIT(AD9088_FFT_SNIFFER_I_INDEX) | BIT(AD9088_FFT_SNIFFER_Q_INDEX),
// 	BIT(AD9088_FFT_SNIFFER_Q_INDEX),
// 	BIT(AD9088_FFT_SNIFFER_I_INDEX),
// 	BIT(AD9088_FFT_SNIFFER_SI_INDEX) | BIT(AD9088_FFT_SNIFFER_MAGN_INDEX),
// 	BIT(AD9088_FFT_SNIFFER_MAGN_INDEX),
// 	0x00000000,
// };

// struct ad9088_fft_sniffer_state {
// 	struct device		*dev;
// 	struct iio_trigger		*trig;
// 	struct mutex			lock;
// 	struct completion		complete;
// 	struct ad9088_phy		*phy;
// 	int irq;
// 	u32 regmap_base;
// 	u32 delay_ms;
// 	u32 mode;
// 	struct delayed_work	sync_work;
// 	struct iio_dev *indio_dev;

// 	u32 side_sel;
// 	adi_apollo_sniffer_param_t sniffer_config;
// 	adi_apollo_sniffer_param_t sniffer_config_hw;

// 	__le16 buffer[AD9088_FFT_SNIFFER_CHAN_MAX];
// 	__le16 buffer_hw[AD9088_FFT_SNIFFER_CHAN_MAX] __aligned(IIO_DMA_MINALIGN);
// };

// static int ad9088_rx_sniffer_populate_default_params(adi_apollo_sniffer_mode_e mode, adi_apollo_sniffer_param_t *config)
// {

// 	config->init.fft_hold_sel = 1;                 // 0 - gpio 1 - regmap
// 	config->init.fft_enable_sel = 1;               // 0 - gpio 1 - regmap
// 	config->init.real_mode = 1;                    // 1 real 0 complex
// 	config->init.max_threshold = 20;                // Max threshold for max
// 	config->init.min_threshold = 0;                // Min threshold for min
// 	config->init.sniffer_enable = 1;               // Enable spec sniffer

// 	config->pgm.sniffer_mode = mode;               // see \ref adi_apollo_sniffer_mode_e
// 	config->pgm.sort_enable = 1;                   // 1 enable 0 disable
// 	config->pgm.continuous_mode = 0;               // 1 continuous 0 single
// 	config->pgm.bottom_fft_enable = 0;             // 1 enable 0 disable
// 	config->pgm.window_enable = 0;                 // 1 enable 0 disable
// 	config->pgm.low_power_enable = 0;              // 1 enable 0 disable
// 	config->pgm.dither_enable = 0;                 // 1 enable 0 disable
// 	config->pgm.alpha_factor = 0;                  // exp. avg. 0 - disable 1-8 - enable
// 	config->pgm.adc = ADI_APOLLO_ADC_0;

// 	config->read.run_fft_engine_background = 0;
// 	config->read.timeout_us = 1000;

// 	if (mode > ADI_APOLLO_SNIFFER_INSTANT_MAGNITUDE) { // IQ mode necessities
// 	    config->init.real_mode = 0;
// 	    config->pgm.sort_enable = 0;
// 	    config->pgm.continuous_mode = 0;
// 	    config->pgm.alpha_factor = 0;
// 	}

// 	return API_CMS_ERROR_OK;
// }

// static int ad9088_fft_sniffer_request(struct ad9088_fft_sniffer_state *st)
// {
// 	int ret;
// 	u8 reg;

// 	ret = adi_apollo_hal_reg_get(&st->phy->ad9088, st->regmap_base, &reg);
// 	if (ret)
// 		return ret;

// 	reg &= ~BIT(5);
// 	reg |= BIT(1);

// 	return adi_apollo_hal_reg_set(&st->phy->ad9088, st->regmap_base, reg);
// }

// static int ad9088_fft_sniffer_data_read(struct ad9088_fft_sniffer_state *st, adi_apollo_sniffer_param_t *config)
// {
// 	adi_apollo_device_t *device = &st->phy->ad9088;
// 	u32 valid_data_length;
// 	u8 fft_done = 0;
// 	bool iq_mode;
// 	int ret, i, j;

//         ret = adi_apollo_hal_bf_get(device, BF_FFT_DONE_INFO(st->regmap_base), &fft_done, 1);
// 	if (ret)
// 		dev_err(st->dev, "adi_apollo_hal_bf_set failed in (%s:%d)\n", __func__, __LINE__);

// 	if (!config->read.run_fft_engine_background) {
// 		ret  = adi_apollo_hal_bf_set(device, BF_FFT_ENABLE_INFO(st->regmap_base), 0);
// 		if (ret)
// 			dev_err(st->dev, "adi_apollo_hal_bf_set failed in (%s:%d)\n", __func__, __LINE__);
// 	}

// 	ret  = adi_apollo_hal_bf_set(device, BF_FFT_HOLD_INFO(st->regmap_base), 1);
// 	if (ret)
// 		dev_err(st->dev, "adi_apollo_hal_bf_set failed in (%s:%d)\n", __func__, __LINE__);

// 	if (!fft_done) {
// 		dev_dbg(st->dev, "FFT not done\n");
// 		return 0;
// 	}

// 	valid_data_length = (config->init.real_mode) ? (ADI_APOLLO_SNIFFER_FFT_LENGTH / 2) : ADI_APOLLO_SNIFFER_FFT_LENGTH;
// 	iq_mode = config->pgm.sniffer_mode > ADI_APOLLO_SNIFFER_INSTANT_MAGNITUDE;

// 	for (i = 0; i < valid_data_length; i++) {

// 		// if (iq_mode || config->pgm.sort_enable) { // IQ mode or sorting enabled
// 		//	ret = adi_apollo_hal_reg32_get(device, REG_MAGNITUDE_I_0_ADDR(st->regmap_base, i), (u32 *)&st->buffer_hw[0]);
// 		//	if (ret)
// 		//		dev_err(st->dev, "adi_apollo_hal_bf_get failed in \n", __func__, __LINE__);
// 		// } else {
// 		//			// Read Magnitude / I data
// 		//	ret  = adi_apollo_hal_bf_get(device, BF_MAGNITUDE_I_INFO(st->regmap_base, i),
// 		//				(uint8_t *) &st->buffer_hw[0], 2);
// 		//	if (ret)
// 		//		dev_err(st->dev, "adi_apollo_hal_bf_get failed in \n", __func__, __LINE__);

// 		//	st->buffer_hw[1] = i; // When sorting is disabled, every bin_number returns as 0 so we manually set it in software
// 		// }

// 		// Read Magnitude / I data
// 		ret  = adi_apollo_hal_bf_get(device, BF_MAGNITUDE_I_INFO(st->regmap_base, i),
// 					(uint8_t *) &st->buffer_hw[0], 2);
// 		if (ret)
// 			dev_err(st->dev, "adi_apollo_hal_bf_get failed in (%s:%d)\n", __func__, __LINE__);

// 		// Read Bin number / Q data
// 		if (iq_mode || config->pgm.sort_enable) { // IQ mode or sorting enabled
// 			ret  = adi_apollo_hal_bf_get(device, BF_BIN_NUMBER_Q_INFO(st->regmap_base, i),
// 							(uint8_t *) &st->buffer_hw[1], 2);
// 			if (ret)
// 				dev_err(st->dev, "adi_apollo_hal_bf_get failed in (%s:%d)\n", __func__, __LINE__);
// 		} else {
// 			st->buffer_hw[1] = i; // When sorting is disabled, every bin_number returns as 0 so we manually set it in software
// 		}

// 		j = 0;

// 		if (st->indio_dev->active_scan_mask[0] & BIT(AD9088_FFT_SNIFFER_SI_INDEX))
// 			st->buffer[j++] = iq_mode ? i : st->buffer_hw[1];

// 		if (st->indio_dev->active_scan_mask[0] & BIT(AD9088_FFT_SNIFFER_I_INDEX))
// 			st->buffer[j++] = st->buffer_hw[0];

// 		if (st->indio_dev->active_scan_mask[0] & BIT(AD9088_FFT_SNIFFER_Q_INDEX))
// 			st->buffer[j++] = st->buffer_hw[1];

// 		if (st->indio_dev->active_scan_mask[0] & BIT(AD9088_FFT_SNIFFER_MAGN_INDEX))
// 			st->buffer[j++] = st->buffer_hw[0];

// 		iio_push_to_buffers(st->indio_dev, st->buffer);
// 	}

//     return ret;
// }

// static int ad9088_fft_sniffer_read_raw(struct iio_dev *indio_dev,
// 			    struct iio_chan_spec const *chan,
// 			    int *val, int *val2, long mask)
// {
// 	int ret = 0;
// 	struct ad9088_fft_sniffer_state *st = iio_priv(indio_dev);
// 	pr_err("%s:%d\n", __func__, __LINE__);

// 	guard(mutex)(&st->phy->lock);

// 	switch (mask) {

// 	default:
// 		ret = -EINVAL;
// 		break;
// 	}


// 	return ret;
// }

// static ssize_t ad9088_fft_sniffer_ext_info_read(struct iio_dev *indio_dev,
// 				    uintptr_t private,
// 				    const struct iio_chan_spec *chan, char *buf)
// {
// 	struct ad9088_fft_sniffer_state *st = iio_priv(indio_dev);
// 	struct ad9088_phy *phy = st->phy;
// 	int ret = 0;
// 	u32 val;

// 	guard(mutex)(&phy->lock);

// 	switch (private) {
// 	case 0:
// 			val = st->sniffer_config.init.max_threshold;
// 		break;
// 	case 1:
// 			val = st->sniffer_config.init.min_threshold;
// 		break;
// 	default:
// 		ret = -EINVAL;
// 	}

// 	if (ret == 0)
// 		ret = sprintf(buf, "%u\n", val);

// 	return ret;
// }

// static ssize_t ad9088_fft_sniffer_ext_info_write(struct iio_dev *indio_dev,
// 				     uintptr_t private,
// 				     const struct iio_chan_spec *chan,
// 				     const char *buf, size_t len)
// {
// 	struct ad9088_fft_sniffer_state *st = iio_priv(indio_dev);
// 	struct ad9088_phy *phy = st->phy;
// 	long long readin;
// 	int ret;
// 	u8 val;

// 	ret = kstrtoll(buf, 10, &readin);
// 	if (ret)
// 		return ret;

// 	val = clamp_t(int, readin, 0, 255);

// 	guard(mutex)(&phy->lock);

// 	switch (private) {
// 	case 0:
// 		ret = adi_apollo_hal_bf_set(&st->phy->ad9088, BF_MAX_THRESHOLD_INFO(st->regmap_base), val);
// 		if (!ret)
// 			st->sniffer_config.init.max_threshold = val;
// 		break;
// 	case 1:
// 		ret = adi_apollo_hal_bf_set(&st->phy->ad9088, BF_MIN_THRESHOLD_INFO(st->regmap_base), val);
// 		if (!ret)
// 			st->sniffer_config.init.min_threshold = val;
// 		break;
// 	default:
// 		ret = -EINVAL;
// 	}

// 	return ret ? ret : len;
// }

// static const char *const ad9088_fft_sniffer_modes[] = {
// 	[0] = "normal",
// 	[1] = "instant",
// };

// static int ad9088_fft_sniffer_modes_read(struct iio_dev *indio_dev,
// 				const struct iio_chan_spec *chan)
// {
// 	struct ad9088_fft_sniffer_state *st = iio_priv(indio_dev);

// 	return st->mode;
// }

// static int ad9088_fft_sniffer_modes_write(struct iio_dev *indio_dev,
// 				 const struct iio_chan_spec *chan,
// 				 unsigned int item)
// {
// 	struct ad9088_fft_sniffer_state *st = iio_priv(indio_dev);

// 	st->mode = item;

// 	return 0;
// }

// static const struct iio_enum ad9088_testmode_enum = {
// 	.items = ad9088_fft_sniffer_modes,
// 	.num_items = ARRAY_SIZE(ad9088_fft_sniffer_modes),
// 	.set = ad9088_fft_sniffer_modes_write,
// 	.get = ad9088_fft_sniffer_modes_read,
// };

// static struct iio_chan_spec_ext_info ad9088_fft_sniffer_ext_info[] = {
// 	IIO_ENUM("mode", IIO_SHARED_BY_TYPE, &ad9088_testmode_enum),
// 	IIO_ENUM_AVAILABLE("mode", IIO_SHARED_BY_TYPE, &ad9088_testmode_enum),
// 	{
// 		.name = "max_threshold",
// 		.read = ad9088_fft_sniffer_ext_info_read,
// 		.write = ad9088_fft_sniffer_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = 0,
// 	},
// 	{
// 		.name = "min_threshold",
// 		.read = ad9088_fft_sniffer_ext_info_read,
// 		.write = ad9088_fft_sniffer_ext_info_write,
// 		.shared = IIO_SHARED_BY_TYPE,
// 		.private = 1,
// 	},
// 	{},
// };

// static const struct iio_chan_spec ad9088_fft_sniffer_channels[] = {
// 	{
// 		.type = IIO_INDEX,
// 		.info_mask_separate = 0,
// 		.channel = 0,
// 		.scan_index = AD9088_FFT_SNIFFER_SI_INDEX,
// 		.scan_type = {
// 			.sign = 'u',
// 			.realbits = 9,
// 			.storagebits = 16,
// 			.shift =  0,
// 			.endianness = IIO_LE,
// 		},
// 	},
// 	{
// 		.type = IIO_VOLTAGE,
// 		.info_mask_separate = 0,
// 		.ext_info = ad9088_fft_sniffer_ext_info,
// 		.indexed = 1,
// 		.modified = 1,
// 		.channel = 0,
// 		.channel2 = IIO_MOD_I,
// 		.scan_index = AD9088_FFT_SNIFFER_I_INDEX,
// 		.scan_type = {
// 			.sign = 's',
// 			.realbits = 9,
// 			.storagebits = 16,
// 			.shift = 0,
// 			.endianness = IIO_LE,
// 		},
// 	},
// 	{
// 		.type = IIO_VOLTAGE,
// 		.info_mask_separate = 0,
// 		.ext_info = ad9088_fft_sniffer_ext_info,
// 		.indexed = 1,
// 		.modified = 1,
// 		.channel = 0,
// 		.channel2 = IIO_MOD_Q,
// 		.scan_index = AD9088_FFT_SNIFFER_Q_INDEX,
// 		.scan_type = {
// 			.sign = 's',
// 			.realbits = 9,
// 			.storagebits = 16,
// 			.shift = 0,
// 			.endianness = IIO_LE,
// 		},
// 	},
// 	{
// 		.type = IIO_MAGN,
// 		.info_mask_separate = 0,
// 		.ext_info = ad9088_fft_sniffer_ext_info,
// 		.indexed = 1,
// 		.channel = 0,
// 		.scan_index = AD9088_FFT_SNIFFER_MAGN_INDEX,
// 		.scan_type = {
// 			.sign = 'u',
// 			.realbits = 9,
// 			.storagebits = 16,
// 			.shift = 0,
// 			.endianness = IIO_LE,
// 		},
// 	},
// };

// static int ad9088_fft_sniffer_debugfs_reg_access(struct iio_dev *indio_dev,
// 				      unsigned int reg, unsigned int writeval,
// 				      unsigned int *readval)
// {
// 	struct ad9088_fft_sniffer_state *st = iio_priv(indio_dev);
// 	struct ad9088_phy *phy = st->phy;
// 	u8 val;
// 	int ret;

// 	guard(mutex)(&st->phy->lock);
// 	if (readval == NULL)
// 		return adi_apollo_hal_reg_set(&phy->ad9088, reg, writeval);

// 	ret = adi_apollo_hal_reg_get(&phy->ad9088, reg, &val);
// 	if (ret < 0)
// 		return ret;

// 	*readval = val;

// 	return 0;

// }

// static int ad9088_fft_sniffer_update_scan_mode(struct iio_dev *indio_dev,
// 				    const unsigned long *active_scan_mask)
// {
// 	struct ad9088_fft_sniffer_state *st  = iio_priv(indio_dev);

// 	guard(mutex)(&st->phy->lock);

// 	memcpy(&st->sniffer_config_hw, &st->sniffer_config, sizeof(st->sniffer_config_hw));

// 	if (active_scan_mask[0] & BIT(AD9088_FFT_SNIFFER_MAGN_INDEX)) {
// 		st->sniffer_config_hw.pgm.sniffer_mode = st->mode ? ADI_APOLLO_SNIFFER_INSTANT_MAGNITUDE : ADI_APOLLO_SNIFFER_NORMAL_MAGNITUDE;
// 	} else {
// 		st->sniffer_config_hw.pgm.sniffer_mode = st->mode ? ADI_APOLLO_SNIFFER_INSTANT_IQ : ADI_APOLLO_SNIFFER_NORMAL_IQ;
// 		st->sniffer_config_hw.init.real_mode = 0;
// 		st->sniffer_config_hw.pgm.sort_enable = 0;
// 		st->sniffer_config_hw.pgm.continuous_mode = 0;
// 		st->sniffer_config_hw.pgm.alpha_factor = 0;
// 	}

// 	dev_dbg(st->dev, "sniffer_mode %d\n", st->sniffer_config_hw.pgm.sniffer_mode);

// 	return 0;
// }

// static const struct iio_info ad9088_fft_sniffer_info = {
// 	.read_raw = &ad9088_fft_sniffer_read_raw,
// 	.debugfs_reg_access = &ad9088_fft_sniffer_debugfs_reg_access,
// 	.update_scan_mode = ad9088_fft_sniffer_update_scan_mode,
// };

// static int ad9088_fft_sniffer_buffer_postenable(struct iio_dev *indio_dev)
// {
// 	struct ad9088_fft_sniffer_state *st = iio_priv(indio_dev);
// 	int ret = 0;

// 	dev_dbg(st->dev, "%s:%d\n", __func__, __LINE__);

// 	guard(mutex)(&st->phy->lock);

// 	ret = adi_apollo_sniffer_init(&st->phy->ad9088, st->side_sel, &st->sniffer_config_hw.init);
// 	if (ret != API_CMS_ERROR_OK) {
// 		dev_err(st->dev, "Error in adi_apollo_sniffer_pgm: %d\n", ret);
// 		return ret;
// 	}
// 	/* Program the sniffer */
// 	ret = adi_apollo_sniffer_pgm(&st->phy->ad9088, st->side_sel, &st->sniffer_config_hw.pgm);
// 	if (ret != API_CMS_ERROR_OK) {
// 		dev_err(st->dev, "Error in adi_apollo_sniffer_pgm: %d\n", ret);
// 		return ret;
// 	}

// 	reinit_completion(&st->complete);
// 	queue_delayed_work(system_freezable_wq, &st->sync_work, msecs_to_jiffies(st->delay_ms));

// 	return 0;
// }

// static int ad9088_fft_sniffer_buffer_predisable(struct iio_dev *indio_dev)
// {
// 	struct ad9088_fft_sniffer_state *st = iio_priv(indio_dev);
// 	int ret;

// 	dev_dbg(st->dev, "%s:%d\n", __func__, __LINE__);

// 	complete_all(&st->complete);
// 	cancel_delayed_work_sync(&st->sync_work);

// 	guard(mutex)(&st->phy->lock);

// 	ret = adi_apollo_sniffer_enable_set(&st->phy->ad9088, st->side_sel, 0);
// 	if (ret != API_CMS_ERROR_OK) {
// 		dev_err(st->dev, "Error in adi_apollo_sniffer_pgm: %d\n", ret);
// 	}

// 	return ret;
// }

// static const struct iio_buffer_setup_ops ad9088_fft_sniffer_buffer_ops = {
// 	.postenable = ad9088_fft_sniffer_buffer_postenable,
// 	.predisable = ad9088_fft_sniffer_buffer_predisable,
// };

// static irqreturn_t ad9088_fft_sniffer_irq_handler(int irq, void *private)
// {
// 	struct iio_dev *indio_dev = private;
// 	struct ad9088_fft_sniffer_state *st = iio_priv(indio_dev);

// 	complete_all(&st->complete);

// 	return IRQ_HANDLED;
// }

// static int ad9088_rx_sniffer_debugfs_init(struct iio_dev *indio_dev, adi_apollo_sniffer_param_t *config)
// {
// 	struct ad9088_fft_sniffer_state *st = iio_priv(indio_dev);
// 	struct dentry *d;

// 	d = iio_get_debugfs_dentry(indio_dev);
// 	if (d) {
// 		debugfs_create_u8("fft_hold_sel", 0644, d, &config->init.fft_hold_sel);
// 		debugfs_create_u8("fft_enable_sel", 0644, d, &config->init.fft_enable_sel);
// 		debugfs_create_u8("real_mode", 0644, d, &config->init.real_mode);
// 		debugfs_create_u8("max_threshold", 0644, d, &config->init.max_threshold);
// 		debugfs_create_u8("min_threshold", 0644, d, &config->init.min_threshold);
// 		debugfs_create_u8("sort_enable", 0644, d, &config->pgm.sort_enable);
// 		debugfs_create_u8("continuous_mode", 0644, d, &config->pgm.continuous_mode);
// 		debugfs_create_u8("bottom_fft_enable", 0644, d, &config->pgm.bottom_fft_enable);
// 		debugfs_create_u8("window_enable", 0644, d, &config->pgm.window_enable);
// 		debugfs_create_u8("low_power_enable", 0644, d, &config->pgm.low_power_enable);
// 		debugfs_create_u8("dither_enable", 0644, d, &config->pgm.dither_enable);
// 		debugfs_create_u8("alpha_factor", 0644, d, &config->pgm.alpha_factor);
// 		debugfs_create_u16("adc", 0644, d, &config->pgm.adc);
// 		debugfs_create_u8("run_fft_engine_background", 0644, d, &config->read.run_fft_engine_background);
// 		debugfs_create_u32("delay_capture_ms", 0644, d, &st->delay_ms);
// 		debugfs_create_u64("adc_sampling_rate_Hz", 0444, d, &st->phy->profile.adc_config[st->side_sel - 1].adc_sampling_rate_Hz);
// 	}

// 	return 0;
// }

// static void ad9088_fft_sniffer_sync_work_func(struct work_struct *work)
// {
// 	struct ad9088_fft_sniffer_state *st =
// 		container_of(work, struct ad9088_fft_sniffer_state, sync_work.work);
// 	int ret;

// 	scoped_guard(mutex, &st->phy->lock) {
// 		ret = ad9088_fft_sniffer_request(st);
// 		if (ret != API_CMS_ERROR_OK) {
// 			dev_err(st->dev, "Error in adi_apollo_sniffer_data_request: %d\n", ret);
// 		}
// 	}

// 	wait_for_completion_interruptible(&st->complete);
// 	reinit_completion(&st->complete);

// 	guard(mutex)(&st->phy->lock);

// 	ad9088_fft_sniffer_data_read(st, &st->sniffer_config_hw);

// 	queue_delayed_work(system_freezable_wq, &st->sync_work, msecs_to_jiffies(st->delay_ms));
// }

// int ad9088_fft_sniffer_probe(struct ad9088_phy *phy, adi_apollo_side_select_e side_sel)
// {
// 	int ret;
// 	struct iio_dev *indio_dev;
// 	struct ad9088_fft_sniffer_state *st;
// 	struct device *dev = &phy->spi->dev;

// 	static char *irq_name;
// 	static adi_apollo_gpio_func_e gpio_func;
// 	u8 pin;

// 	indio_dev = devm_iio_device_alloc(dev, sizeof(*st));
// 	if (!indio_dev) {
// 		pr_err("Can't allocate iio device\n");
// 		return -ENOMEM;
// 	}

// 	st = iio_priv(indio_dev);
// 	st->dev = dev;
// 	st->phy = phy;
// 	st->indio_dev = indio_dev;
// 	st->delay_ms = 100;
// 	st->mode = 1; /* instant */

// 	mutex_init(&st->lock);
// 	init_completion(&st->complete);
// 	INIT_DELAYED_WORK(&st->sync_work, ad9088_fft_sniffer_sync_work_func);

// 	indio_dev->info = &ad9088_fft_sniffer_info;
// 	indio_dev->modes = INDIO_DIRECT_MODE;
// 	indio_dev->channels = ad9088_fft_sniffer_channels;
// 	indio_dev->num_channels = ARRAY_SIZE(ad9088_fft_sniffer_channels);
// 	indio_dev->available_scan_masks = ad9088_fft_sniffer_available_scan_masks;

// 	switch (side_sel) {
// 	case ADI_APOLLO_SIDE_A:
// 		irq_name = "fft_done_A";
// 		indio_dev->name = "ad9088-fft-sniffer-A";
// 		gpio_func = ADI_APOLLO_FUNC_FFT_DONE_A;
// 		pin = 17;
// 		st->side_sel = ADI_APOLLO_SNIFFER_A;
// 		st->regmap_base = RX_SPECTRUM_SNIFFER_RX_SLICE_0_RX_DIGITAL0;
// 		break;
// 	case ADI_APOLLO_SIDE_B:
// 		irq_name = "fft_done_B";
// 		indio_dev->name = "ad9088-fft-sniffer-B";
// 		gpio_func = ADI_APOLLO_FUNC_FFT_DONE_B;
// 		pin = 18;
// 		st->side_sel = ADI_APOLLO_SNIFFER_B;
// 		st->regmap_base = RX_SPECTRUM_SNIFFER_RX_SLICE_0_RX_DIGITAL1;
// 		break;
// 	default:
// 		dev_err(&phy->spi->dev, "Invalid side selection\n");
// 		return -EINVAL;
// 	}

// 	st->side_sel = side_sel;
// 	st->irq = fwnode_irq_get_byname(dev_fwnode(dev), irq_name);

// 	if (st->irq == -EPROBE_DEFER)
// 		return -EPROBE_DEFER;

// 	if (st->irq < 0) {
// 		st->irq = 0;
// 		return 0;
// 	}

// 	ret = devm_iio_kfifo_buffer_setup_ext(st->dev, indio_dev,
// 					      &ad9088_fft_sniffer_buffer_ops,
// 					      NULL);
// 	if (ret)
// 		return ret;

// 	ret = devm_request_irq(dev, st->irq,
// 					ad9088_fft_sniffer_irq_handler, 0,
// 					indio_dev->name, indio_dev);
// 	if (ret)
// 		return dev_err_probe(st->dev, ret, "Failed to request irq\n");

// 	adi_apollo_gpio_quick_config_mode_set(&phy->ad9088, 0);
// 	adi_apollo_gpio_cmos_func_mode_set(&phy->ad9088, pin, gpio_func);

// 	ad9088_rx_sniffer_populate_default_params(ADI_APOLLO_SNIFFER_INSTANT_MAGNITUDE, &st->sniffer_config);

// 	ret = devm_iio_device_register(dev, indio_dev);

// 	ad9088_rx_sniffer_debugfs_init(indio_dev, &st->sniffer_config);

// 	return ret;

// }
// EXPORT_SYMBOL_GPL(ad9088_fft_sniffer_probe);
