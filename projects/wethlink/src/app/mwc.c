#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_crc8.h"
#include "iio.h"
#include "mwc.h"
#include "no_os_util.h"
#include "adc.h"
#include "mxc_sys.h"
#include "led.h"

const union nvmp255 factory_defaults_template = {
	.data = {
		.hw_version =  "-",
		.carrier_version = "-",
		.hw_serial = "-",
		.carrier_model = "-",
		.carrier_serial = "-",

		.tx_autotuning = true,
		.tx_target = 350,
		.tx_tolerance = 50,
		.rx_autotuning = true,
		.rx_target = 1950,
		.rx_tolerance = 50,
		.tx_auto_ifvga = true,
		.rx_auto_ifvga_rflna = true,
		.temp_correlation = {
			{
				// lbtx
				{1, 3, 7, 15, 31}, // temperature
				{15, 15, 15, 15, 10}, // tx if_attn
				{6, 6, 6, 6, 3}, // rx if_attn
				{HMC6301_LNA_ATTN_18dB, HMC6301_LNA_ATTN_18dB, HMC6301_LNA_ATTN_12dB, HMC6301_LNA_ATTN_6dB, HMC6301_LNA_ATTN_6dB} // rx lna_attn
			},
			{
				// hbtx
				{1, 3, 7, 15, 31},
				{15, 15, 15, 10, 0},
				{12, 12, 12, 12, 9},
				{HMC6301_LNA_ATTN_18dB, HMC6301_LNA_ATTN_18dB, HMC6301_LNA_ATTN_12dB, HMC6301_LNA_ATTN_6dB, HMC6301_LNA_ATTN_6dB}
			}
		},

		.hmc6300_enabled = true,
		.hmc6300_vco = {59850000000, 63262500000},
		.hmc6300_if_attn = 13,
		.hmc6300_rf_attn = 15,

		.hmc6301_enabled = true,
		.hmc6301_vco = {63262500000, 59850000000},
		.hmc6301_if_attn = 11,
		.hmc6301_lna_attn = HMC6301_LNA_ATTN_12dB,
		.hmc6301_bb_attn1 = HMC6301_BB_ATTN_18dB,
		.hmc6301_bb_attn2 = HMC6301_BB_ATTN_18dB,
		.hmc6301_bb_attni_fine = HMC6301_BB_ATTN_FINE_3dB,
		.hmc6301_bb_attnq_fine = HMC6301_BB_ATTN_FINE_0dB,
	}
};

static int32_t mwc_read(struct mwc_iio_dev *mwc, uint32_t reg,
			uint32_t *readval)
{
	if (reg >= 20)
		return -EINVAL;

	uint8_t	col = reg % 5;
	uint8_t row = reg / 5;
	uint8_t attn;

	attn = mwc->temp_correlation[row][col];
	*readval = attn;

	return 0;
}

static int32_t mwc_write(struct mwc_iio_dev *mwc, uint32_t reg,
			 uint32_t writeval)
{
	if (reg >= 20)
		return -EINVAL;

	uint8_t	col = reg % 5;
	uint8_t row = reg / 5;

	mwc->temp_correlation[row][col] = writeval;

	return 0;
}

void mwc_temp_correlation(uint8_t (*correlation)[5], uint8_t temp,
			  uint8_t *tx_if, uint8_t *rx_if, uint8_t *rx_rflna)
{
	uint8_t e;

	for (e = 0; e < NO_OS_ARRAY_SIZE(correlation[0]); e++) {
		if (temp <= correlation[0][e]) {
			if (tx_if)
				*tx_if = correlation[1][e];
			if (rx_if)
				*rx_if = correlation[2][e];
			if (rx_rflna)
				*rx_rflna = correlation[3][e];
			break;
		}
	}
}

int mwc_algorithms(struct mwc_iio_dev *mwc)
{
	int ret;
	static uint8_t prev_bb_attn, prev_rf_attn;
	uint8_t temp;
	struct hmc630x_dev *tx = mwc->tx_iiodev->dev;
	struct hmc630x_dev *rx = mwc->rx_iiodev->dev;

	if (mwc->tx_auto_ifvga) {
		uint8_t if_attn;
		ret = hmc630x_get_temp(tx, &temp);
		if (ret)
			return ret;

		mwc_temp_correlation(mwc->temp_correlation,
				     temp, &if_attn, NULL, NULL);

		ret = hmc630x_set_if_attn(tx, if_attn);
		if (ret)
			return ret;
	}

	if (mwc->rx_auto_ifvga_rflna) {
		uint8_t if_attn, lna_attn;
		ret = hmc630x_get_temp(rx, &temp);
		if (ret)
			return ret;

		mwc_temp_correlation(mwc->temp_correlation,
				     temp, NULL, &if_attn, &lna_attn);

		ret = hmc630x_set_if_attn(rx, if_attn);
		if (ret)
			return ret;

		ret = hmc6301_set_lna_gain(rx, lna_attn);
	}

	if (mwc->tx_autotuning) {
		int attn;
		int iter_count = 0;
		uint16_t reading;
		uint32_t mV;
		mxc_adc_chsel_t ch = MXC_ADC_CH_0;

		no_os_pid_reset(mwc->tx_pid);
		no_os_pid_hysteresis(mwc->tx_pid, mwc->tx_tolerance);
		while(true) {
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
			if (prev_rf_attn != attn) {
				ret = hmc6300_set_rf_attn(tx, attn);
				if (ret)
					break;

				prev_rf_attn = attn;
			} else if (iter_count >= 8) {
				led_tx_det_green(false);
				led_tx_det_red(false);
				if (abs((int)mV - (int)mwc->tx_target) <= mwc->tx_tolerance)
					led_tx_det_green(true);
				else
					led_tx_det_red(true);

				break;
			}

			iter_count++;
		}
	} else {
		led_tx_det_green(false);
		led_tx_det_red(false);
	}

	if (mwc->rx_autotuning) {
		int attn;
		uint8_t attn_temp, attn1, attn2, attni_fine;
		int iter_count = 0;
		uint16_t reading;
		uint32_t mV;
		mxc_adc_chsel_t ch = MXC_ADC_CH_1_DIV_5;
		const uint8_t attn_reverse[] = {0, 2, 1, 3};
		const uint8_t attn_fine_reverse[] = {0, 4, 2, 6, 1, 5, 3};

		no_os_pid_reset(mwc->rx_pid);
		no_os_pid_hysteresis(mwc->rx_pid, mwc->rx_tolerance);
		while(true) {
			mxc_adc_conversion_req_t req = {
				.channel = ch,
				.scale = MXC_ADC_SCALE_2X,
			};
			MXC_ADC_Convert(&req);
			reading = req.rawADCValue;

			mV = (uint64_t)reading * 5 / 2 * 1220000000 / 1024 / 1000000;
			ret = no_os_pid_control(mwc->rx_pid, mwc->rx_target, mV, &attn);
			if (ret)
				break;
			if (prev_bb_attn != attn) {
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

				prev_bb_attn = attn;
			} else if (iter_count >= 8) {
				led_rx_det_green(false);
				led_rx_det_red(false);
				if (abs((int)mV - (int)mwc->rx_target) <= mwc->rx_tolerance)
					led_rx_det_green(true);
				else
					led_rx_det_red(true);

				break;
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

int mwc_save_to_eeprom(struct mwc_iio_dev *mwc, uint16_t address)
{
	int ret;
	bool enabled;
	uint64_t freq;
	uint8_t attn, attn2;

	struct hmc630x_dev *dev;
	const uint16_t nvmpsz = sizeof(union nvmp255);
	uint8_t eebuf[nvmpsz + 1];
	union nvmp255 nvmp = factory_defaults_template;

	// firmware specific parameters
	nvmp.data.tx_autotuning = mwc->tx_autotuning;
	nvmp.data.tx_target = mwc->tx_target;
	nvmp.data.tx_tolerance = mwc->tx_tolerance;
	nvmp.data.rx_autotuning = mwc->rx_autotuning;
	nvmp.data.rx_target = mwc->rx_target;
	nvmp.data.rx_tolerance = mwc->rx_tolerance;
	nvmp.data.tx_auto_ifvga = mwc->tx_auto_ifvga;
	nvmp.data.rx_auto_ifvga_rflna = mwc->rx_auto_ifvga_rflna;
	memcpy(&nvmp.data.temp_correlation[mwc->hbtx], mwc->temp_correlation,
	       sizeof(nvmp.data.temp_correlation[mwc->hbtx]));

	// hmc6300 parameters
	dev = mwc->tx_iiodev->dev;

	ret = hmc630x_get_enable(dev, &enabled);
	if (ret)
		return ret;
	nvmp.data.hmc6300_enabled = enabled;

	if (enabled) {
		ret = hmc630x_get_vco(dev, &freq);
		if (ret)
			return ret;
		nvmp.data.hmc6300_vco[mwc->hbtx] = freq;
	}

	if (!mwc->tx_auto_ifvga) {
		ret = hmc630x_get_if_attn(dev, &attn);
		if (ret)
			return ret;
		nvmp.data.hmc6300_if_attn = attn;
	}

	if (!mwc->tx_autotuning) {
		ret = hmc6300_get_rf_attn(dev, &attn);
		if (ret)
			return ret;
		nvmp.data.hmc6300_rf_attn = attn;
	}

	// hmc6301 parameters
	dev = mwc->rx_iiodev->dev;

	ret = hmc630x_get_enable(dev, &enabled);
	if (ret)
		return ret;
	nvmp.data.hmc6301_enabled = enabled;

	if (enabled) {
		ret = hmc630x_get_vco(dev, &freq);
		if (ret)
			return ret;
		nvmp.data.hmc6301_vco[mwc->hbtx] = freq;
	}

	if (!mwc->rx_auto_ifvga_rflna) {
		ret = hmc630x_get_if_attn(dev, &attn);
		if (ret)
			return ret;
		nvmp.data.hmc6301_if_attn = attn;

		ret = hmc6301_get_lna_gain(dev, &attn);
		if (ret)
			return ret;
		nvmp.data.hmc6301_lna_attn = attn;
	}

	if (!mwc->rx_autotuning) {
		ret = hmc6301_get_bb_attn(dev, &attn, &attn2);
		if (ret)
			return ret;
		nvmp.data.hmc6301_bb_attn1 = attn;
		nvmp.data.hmc6301_bb_attn2 = attn2;

		ret = hmc6301_get_bb_attn_fine(dev, &attn, &attn2);
		if (ret)
			return ret;
		nvmp.data.hmc6301_bb_attni_fine = attn;
		nvmp.data.hmc6301_bb_attnq_fine = attn2;
	}

	memcpy(eebuf, &nvmp, nvmpsz);
	eebuf[nvmpsz] = no_os_crc8(mwc->crc8, eebuf, nvmpsz, 0xa5);
	return no_os_eeprom_write(mwc->eeprom, address, eebuf, nvmpsz+1);
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
	case MWC_IIO_ATTR_RESET:
	case MWC_IIO_ATTR_SAVE:
		val = 0; // dummy, to avoid attribute read error
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
	case MWC_IIO_ATTR_SAVE:
		ret = mwc_save_to_eeprom(iiodev, NVMP_AREA_ADDRESS(0));
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
		case 0:
			adcch = MXC_ADC_CH_0;
			break;
		case 1:
			adcch = MXC_ADC_CH_1_DIV_5;
			break;
		default:
			return -EINVAL;
		};

		mxc_adc_conversion_req_t req = {
			.channel = adcch,
			.scale = channel->ch_num == MXC_ADC_CH_0 ? MXC_ADC_SCALE_1 : MXC_ADC_SCALE_2X,
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
		vals[0] = channel->ch_num == 0 ? 1220 : 3050;
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
		.show = mwc_iio_read_attr,
		.store = mwc_iio_write_attr,
	},
	{
		.name = "save",
		.priv = MWC_IIO_ATTR_SAVE,
		.show = mwc_iio_read_attr,
		.store = mwc_iio_write_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device mwc_iio_device_template = {
	.attributes = mwc_iio_attrs,
	.num_ch = NO_OS_ARRAY_SIZE(mwc_channels),
	.channels = mwc_channels,
	.debug_reg_read = (int32_t (*)())mwc_read,
	.debug_reg_write = (int32_t (*)())mwc_write,
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
	d->temp_correlation = init_param->temp_correlation;
	d->id = init_param->id;
	d->hbtx = init_param->hbtx;
	d->crc8 = init_param->crc8;
	d->eeprom = init_param->eeprom;
	d->adin1300 = init_param->adin1300;
	d->max24287 = init_param->max24287;

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
		.Kp = 20000,
		.Ki = 500,
		.Kd = 0,
		.initial = 31,
		.hysteresis = 50,
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
