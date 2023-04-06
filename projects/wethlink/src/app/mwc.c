#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "iio.h"
#include "mwc.h"
#include "no_os_util.h"
#include "adc.h"
#include "mxc_sys.h"
#include "led.h"

struct mwc_temp_if_correlation {
	uint8_t ifreq;
};

struct mwc_temp_if_correlation mwc_tx_temp_lookup[32][2] = {
	// lbtx, hbtx
	{{0}, {0}},
	{{14}, {15}},
	{{13}, {14}},
	{{13}, {14}},
	{{12}, {13}},
	{{12}, {13}},
	{{12}, {13}},
	{{12}, {13}},
	{{6}, {8}},
	{{6}, {8}},
	{{6}, {8}},
	{{6}, {8}},
	{{6}, {8}},
	{{6}, {8}},
	{{6}, {8}},
	{{6}, {8}},
	{{0}, {4}},
	{{0}, {4}},
	{{0}, {4}},
	{{0}, {4}},
	{{0}, {4}},
	{{0}, {4}},
	{{0}, {4}},
	{{0}, {4}},
	{{0}, {4}},
	{{0}, {4}},
	{{0}, {4}},
	{{0}, {4}},
	{{0}, {4}},
	{{0}, {4}},
	{{0}, {4}},
	{{0}, {4}},
};

struct mwc_temp_if_lna_correlation {
	uint8_t ifreq;
	uint8_t lna_gain;
};

struct mwc_temp_if_lna_correlation mwc_rx_temp_lookup[32][2] = {
	// lbtx, hbtx
	{{0}, {0}},
	{{11, HMC6301_LNA_GAIN_18dB}, {6, HMC6301_LNA_GAIN_18dB}},
	{{11, HMC6301_LNA_GAIN_18dB}, {6, HMC6301_LNA_GAIN_18dB}},
	{{11, HMC6301_LNA_GAIN_18dB}, {6, HMC6301_LNA_GAIN_18dB}},
	{{11, HMC6301_LNA_GAIN_18dB}, {6, HMC6301_LNA_GAIN_18dB}},
	{{11, HMC6301_LNA_GAIN_18dB}, {6, HMC6301_LNA_GAIN_18dB}},
	{{11, HMC6301_LNA_GAIN_18dB}, {6, HMC6301_LNA_GAIN_18dB}},
	{{11, HMC6301_LNA_GAIN_12dB}, {6, HMC6301_LNA_GAIN_12dB}},
	{{11, HMC6301_LNA_GAIN_12dB}, {6, HMC6301_LNA_GAIN_12dB}},
	{{11, HMC6301_LNA_GAIN_12dB}, {6, HMC6301_LNA_GAIN_12dB}},
	{{11, HMC6301_LNA_GAIN_12dB}, {6, HMC6301_LNA_GAIN_12dB}},
	{{11, HMC6301_LNA_GAIN_12dB}, {6, HMC6301_LNA_GAIN_12dB}},
	{{11, HMC6301_LNA_GAIN_12dB}, {6, HMC6301_LNA_GAIN_12dB}},
	{{11, HMC6301_LNA_GAIN_12dB}, {6, HMC6301_LNA_GAIN_12dB}},
	{{11, HMC6301_LNA_GAIN_12dB}, {6, HMC6301_LNA_GAIN_12dB}},
	{{11, HMC6301_LNA_GAIN_6dB}, {6, HMC6301_LNA_GAIN_6dB}},
	{{11, HMC6301_LNA_GAIN_6dB}, {6, HMC6301_LNA_GAIN_6dB}},
	{{11, HMC6301_LNA_GAIN_6dB}, {6, HMC6301_LNA_GAIN_6dB}},
	{{11, HMC6301_LNA_GAIN_6dB}, {6, HMC6301_LNA_GAIN_6dB}},
	{{11, HMC6301_LNA_GAIN_6dB}, {6, HMC6301_LNA_GAIN_6dB}},
	{{11, HMC6301_LNA_GAIN_6dB}, {6, HMC6301_LNA_GAIN_6dB}},
	{{11, HMC6301_LNA_GAIN_6dB}, {6, HMC6301_LNA_GAIN_6dB}},
	{{11, HMC6301_LNA_GAIN_6dB}, {6, HMC6301_LNA_GAIN_6dB}},
	{{11, HMC6301_LNA_GAIN_6dB}, {6, HMC6301_LNA_GAIN_6dB}},
	{{11, HMC6301_LNA_GAIN_6dB}, {6, HMC6301_LNA_GAIN_6dB}},
	{{11, HMC6301_LNA_GAIN_6dB}, {6, HMC6301_LNA_GAIN_6dB}},
	{{11, HMC6301_LNA_GAIN_6dB}, {6, HMC6301_LNA_GAIN_6dB}},
	{{11, HMC6301_LNA_GAIN_6dB}, {6, HMC6301_LNA_GAIN_6dB}},
	{{11, HMC6301_LNA_GAIN_6dB}, {6, HMC6301_LNA_GAIN_6dB}},
	{{11, HMC6301_LNA_GAIN_6dB}, {6, HMC6301_LNA_GAIN_6dB}},
	{{11, HMC6301_LNA_GAIN_6dB}, {6, HMC6301_LNA_GAIN_6dB}},
	{{8, HMC6301_LNA_GAIN_0dB}, {3, HMC6301_LNA_GAIN_0dB}},
};

int mwc_algorithms(struct mwc_iio_dev *mwc)
{
	int ret;
	uint8_t temp;
	struct hmc630x_dev *tx = mwc->tx_iiodev->dev;
	struct hmc630x_dev *rx = mwc->rx_iiodev->dev;

	if (mwc->tx_auto_ifvga) {
		ret = hmc630x_get_temp(tx, &temp);
		if (ret)
			return ret;

		ret = hmc630x_set_if_attn(tx, mwc_tx_temp_lookup[temp][mwc->hbtx].ifreq);
		if (ret)
			return ret;
	}

	if (mwc->rx_auto_ifvga_rflna) {
		ret = hmc630x_get_temp(rx, &temp);
		if (ret)
			return ret;

		ret = hmc630x_set_if_attn(rx, mwc_rx_temp_lookup[temp][mwc->hbtx].ifreq);
		if (ret)
			return ret;

		ret = hmc6301_set_lna_gain(rx, mwc_rx_temp_lookup[temp][mwc->hbtx].lna_gain);
	}

	if (mwc->tx_autotuning) {
		int attn, prev_attn;
		int match_count = 0;
		int iter_count = 0;
		uint16_t reading;
		uint32_t mV;
		bool done = false;
		mxc_adc_chsel_t ch;
#if (TARGET_NUM==32650)
		ch = MXC_ADC_CH_0;
#elif (TARGET_NUM==78000)
		ch = MXC_ADC_CH_3;
#endif
		while(!done) {
			mxc_adc_conversion_req_t req = {
				.channel = ch,
				.scale = MXC_ADC_SCALE_1,
			};
			MXC_ADC_Convert(&req);
			reading = req.rawADCValue;

			mV = (uint64_t)reading * 1220000000 / 1024 / 1000000;
			ret = no_os_pid_control(mwc->tx_pid, mwc->tx_target, mV, &attn);
			if (ret)
				break;
			ret = hmc6300_set_rf_attn(tx, attn);
			if (ret)
				break;

			if (attn == prev_attn)
				match_count++;
			else
				match_count = 0;

			prev_attn = attn;

			if (match_count == 10 || iter_count == 20) {
				done = true;
				led_tx_det_green(false);
				led_tx_det_red(false);
				if (abs((int)mV - (int)mwc->tx_target) <= mwc->tx_tolerance)
					led_tx_det_green(true);
				else
					led_tx_det_red(true);
			}

			iter_count++;
		}
	}
	else {
		led_tx_det_green(false);
		led_tx_det_red(false);
	}

	if (mwc->rx_autotuning) {
		int attn, prev_attn;
		uint8_t attn_temp, attn1, attn2, attni_fine;
		int match_count = 0;
		int iter_count = 0;
		uint16_t reading;
		uint32_t mV;
		bool done = false;
		mxc_adc_chsel_t ch;
		const uint8_t attn_reverse[] = {0, 2, 1, 3};
		const uint8_t attn_fine_reverse[] = {0, 6, 2, 5, 1, 5, 3};
#if (TARGET_NUM==32650)
		ch = MXC_ADC_CH_1;
#elif (TARGET_NUM==78000)
		ch = MXC_ADC_CH_4;
#endif
		while(!done) {
			mxc_adc_conversion_req_t req = {
				.channel = ch,
				.scale = MXC_ADC_SCALE_2,
			};
			MXC_ADC_Convert(&req);
			reading = req.rawADCValue;

			mV = (uint64_t)reading * 2 * 1220000000 / 1024 / 1000000;
			ret = no_os_pid_control(mwc->rx_pid, mwc->rx_target, mV, &attn);
			if (ret)
				break;
			attni_fine = attn % 6;
			attn_temp = (attn - attni_fine) / 6;
			attn1 = no_os_min(3, attn_temp);
			attn2 = attn_temp - attn1;
			ret = hmc6301_set_bb_attn(rx, attn_reverse[attn1], attn_reverse[attn2]);
			if (ret)
				break;
			ret = hmc6301_set_bb_attn_fine(rx, attn_fine_reverse[attni_fine], 0);
			if (ret)
				break;

			if (attn == prev_attn)
				match_count++;
			else
				match_count = 0;

			prev_attn = attn;

			if (match_count == 10 || iter_count == 20) {
				done = true;
				uint8_t if_attn;
				enum hmc6301_lna_gain rflna_attn;
				hmc630x_get_if_attn(rx, &if_attn);
				hmc6301_get_lna_gain(rx, &rflna_attn);
				float gain = 69 - (float)if_attn * 1.3 - (float)rflna_attn * 6 - 
						attn1 * 6 - attn2 * 6 - attni_fine;
				float s = mwc->hbtx ? 10 : 18;
				float e = mwc->hbtx ? 31.2 : 37.2;
				if ((gain - s) < 0)
					gain = s;
				led_rx_det(100 - (int)(100 * (gain - s) / (e - s)));
			}

			iter_count++;
		}
	}

	return ret;
}

int mwc_tx_rx_reset(struct mwc_iio_dev *mwc)
{
	if (!mwc)
		return -EINVAL;

	no_os_gpio_set_value(mwc->reset_gpio, NO_OS_GPIO_HIGH);
	no_os_mdelay(1);
	no_os_gpio_set_value(mwc->reset_gpio, NO_OS_GPIO_LOW);
	no_os_mdelay(1);

	return 0;
}

static int mwc_iio_read_attr(void *device, char *buf,
			     uint32_t len, const struct iio_ch_info *channel,
			     intptr_t priv)
{
	struct mwc_iio_dev *iiodev = (struct mwc_iio_dev *)device;
	int32_t val;

	switch (priv) {
	case MWC_IIO_ATTR_TX_AUTOTUNING:
		val = iiodev->tx_autotuning;
		break;
	case MWC_IIO_ATTR_TX_TARGET:
		val = iiodev->tx_target;
		break;
	case MWC_IIO_ATTR_TX_TOLERANCE:
		val = iiodev->tx_tolerance;
		break;
	case MWC_IIO_ATTR_RX_AUTOTUNING:
		val = iiodev->rx_autotuning;
		break;
	case MWC_IIO_ATTR_RX_TARGET:
		val = iiodev->rx_target;
		break;
	case MWC_IIO_ATTR_RX_TOLERANCE:
		val = iiodev->rx_tolerance;
		break;
	case MWC_IIO_ATTR_TX_AUTO_IFVGA:
		val = iiodev->tx_auto_ifvga;
		break;
	case MWC_IIO_ATTR_RX_AUTO_IFVGA_RFLNA:
		val = iiodev->rx_auto_ifvga_rflna;
		break;
	default:
		return -EINVAL;
	};

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int mwc_iio_write_attr(void *device, char *buf,
			      uint32_t len, const struct iio_ch_info *channel,
			      intptr_t priv)
{
	int ret = 0;
	int32_t val;
	struct mwc_iio_dev *iiodev = (struct mwc_iio_dev *)device;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	if (ret < 0)
		return ret;

	switch (priv) {
	case MWC_IIO_ATTR_TX_AUTOTUNING:
		iiodev->tx_autotuning = (bool)val;
		break;
	case MWC_IIO_ATTR_TX_TARGET:
		iiodev->tx_target = val;
		break;
	case MWC_IIO_ATTR_TX_TOLERANCE:
		iiodev->tx_tolerance = val;
		break;
	case MWC_IIO_ATTR_RX_AUTOTUNING:
		iiodev->rx_autotuning = (bool)val;
		break;
	case MWC_IIO_ATTR_RX_TARGET:
		iiodev->rx_target = val;
		break;
	case MWC_IIO_ATTR_RX_TOLERANCE:
		iiodev->rx_tolerance = val;
		break;
	case MWC_IIO_ATTR_TX_AUTO_IFVGA:
		iiodev->tx_auto_ifvga = (bool)val;
		break;
	case MWC_IIO_ATTR_RX_AUTO_IFVGA_RFLNA:
		iiodev->rx_auto_ifvga_rflna = (bool)val;
		break;
	case MWC_IIO_ATTR_RESET:
		ret = mwc_tx_rx_reset(iiodev);
		break;
	default:
		ret = -EINVAL;
		break;
	};

	return ret;
}

static int mwc_iio_read_raw(void *device, char *buf, uint32_t len,
			    const struct iio_ch_info *channel, intptr_t priv)
{
	uint16_t v;
	int32_t val;
	mxc_adc_chsel_t adcch;

	switch (channel->type) {
	case IIO_VOLTAGE:
		switch (channel->ch_num) {
#if (TARGET_NUM==32650)
		case 0:
			adcch = MXC_ADC_CH_0;
			break;
		case 1:
			adcch = MXC_ADC_CH_1;
			break;
#elif (TARGET_NUM==78000)
		case 0:
			adcch = MXC_ADC_CH_3;
			break;
		case 1:
			adcch = MXC_ADC_CH_4;
			break;
#endif
		default:
			return -EINVAL;
		};

		mxc_adc_conversion_req_t req = {
			.channel = adcch,
			.scale = channel->ch_num == 0 ? MXC_ADC_SCALE_1 : MXC_ADC_SCALE_2,
		};
		MXC_ADC_Convert(&req);
		v = req.rawADCValue;

		break;
	default:
		return -EINVAL;
	};

	val = v;
	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int mwc_iio_read_scale(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	int32_t valt;
	int32_t vals[2];

	switch (channel->type) {
	case IIO_VOLTAGE:
		/* 1220 mV or 2440 mV / 2^10 */
		vals[0] = channel->ch_num == 0 ? 1220 : 2440;
		vals[1] = 10;
		valt = IIO_VAL_FRACTIONAL_LOG2;
		break;
	default:
		return -EINVAL;
		break;
	}

	return iio_format_value(buf, len, valt, 2, vals);
}

static struct iio_attribute mwc_iio_ch_attrs[] = {
	{
		.name = "raw",
		.show = mwc_iio_read_raw,
	},
	{
		.name = "scale",
		.show = mwc_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel mwc_channels[] = {
	{
		.name = "tx_det",
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.indexed = 1,
		.attributes = mwc_iio_ch_attrs,
	},
	{
		.name = "rx_det",
		.ch_type = IIO_VOLTAGE,
		.channel = 1,
		.indexed = 1,
		.attributes = mwc_iio_ch_attrs,
	},
};

static struct iio_attribute mwc_iio_attrs[] = {
	{
		.name = "tx_autotuning",
		.priv = MWC_IIO_ATTR_TX_AUTOTUNING,
		.show = mwc_iio_read_attr,
		.store = mwc_iio_write_attr,
	},
	{
		.name = "tx_target",
		.priv = MWC_IIO_ATTR_TX_TARGET,
		.show = mwc_iio_read_attr,
		.store = mwc_iio_write_attr,
	},
	{
		.name = "tx_tolerance",
		.priv = MWC_IIO_ATTR_TX_TOLERANCE,
		.show = mwc_iio_read_attr,
		.store = mwc_iio_write_attr,
	},
	{
		.name = "rx_autotuning",
		.priv = MWC_IIO_ATTR_RX_AUTOTUNING,
		.show = mwc_iio_read_attr,
		.store = mwc_iio_write_attr,
	},
	{
		.name = "rx_target",
		.priv = MWC_IIO_ATTR_RX_TARGET,
		.show = mwc_iio_read_attr,
		.store = mwc_iio_write_attr,
	},
	{
		.name = "rx_tolerance",
		.priv = MWC_IIO_ATTR_RX_TOLERANCE,
		.show = mwc_iio_read_attr,
		.store = mwc_iio_write_attr,
	},
	{
		.name = "tx_auto_ifvga",
		.priv = MWC_IIO_ATTR_TX_AUTO_IFVGA,
		.show = mwc_iio_read_attr,
		.store = mwc_iio_write_attr,
	},
	{
		.name = "rx_auto_ifvga_rflna",
		.priv = MWC_IIO_ATTR_RX_AUTO_IFVGA_RFLNA,
		.show = mwc_iio_read_attr,
		.store = mwc_iio_write_attr,
	},
	{
		.name = "reset",
		.priv = MWC_IIO_ATTR_RESET,
		.store = mwc_iio_write_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device mwc_iio_device_template = {
	.attributes = mwc_iio_attrs,
	.num_ch = NO_OS_ARRAY_SIZE(mwc_channels),
	.channels = mwc_channels,
};

int mwc_iio_init(struct mwc_iio_dev **iiodev,
		 struct mwc_iio_init_param *init_param)
{
	int ret;
	struct mwc_iio_dev *d;
	struct iio_device *d2;

	d = (struct mwc_iio_dev *)calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d2 = (struct iio_device *)calloc(1, sizeof(*d2));
	if (!d2)
		goto end_0;

	*d2 = mwc_iio_device_template;
	d->iio_dev = d2;
	d->tx_autotuning = init_param->tx_autotuning;
	d->tx_target = init_param->tx_target;
	d->tx_tolerance = init_param->tx_tolerance;
	d->rx_autotuning = init_param->rx_autotuning;
	d->rx_target = init_param->rx_target;
	d->rx_tolerance = init_param->rx_tolerance;
	d->tx_auto_ifvga = init_param->tx_auto_ifvga;
	d->rx_auto_ifvga_rflna = init_param->rx_auto_ifvga_rflna;
	d->hbtx = init_param->hbtx;

	// initialize reset gpio separately
	ret = no_os_gpio_get(&d->reset_gpio, init_param->reset_gpio_ip);
	if (ret)
		goto end_1;

	// set gpio direction
	ret = no_os_gpio_direction_output(d->reset_gpio, NO_OS_GPIO_LOW);
	if (ret)
		goto end_1;

	MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_GPIO2);
	ret = MXC_ADC_Init();
	if (ret)
		goto end_1;

	struct no_os_pid_config pid_config = {
		.Kp = 15000,
		.Ki = 5000,
		.Kd = 0,
		.initial = 31,
		.hysteresis = 20,
		.i_clip = {
			.high = 100,
			.low = -100
		},
		.output_clip = {
			.high = 31,
			.low = 0
		}
	};
	ret = no_os_pid_init(&d->tx_pid, pid_config);
	if (ret)
		goto end_1;
	pid_config.output_clip.high = 41;
	ret = no_os_pid_init(&d->rx_pid, pid_config);
	if (ret)
		goto end_2;

	*iiodev = d;

	return 0;
end_2:
	no_os_pid_remove(d->tx_pid);
end_1:
	free(d2);
end_0:
	free(d);
	return ret;
}

int mwc_iio_remove(struct mwc_iio_dev *iiodev)
{
	no_os_pid_remove(iiodev->tx_pid);
	no_os_pid_remove(iiodev->rx_pid);
	free(iiodev);

	return 0;
}
