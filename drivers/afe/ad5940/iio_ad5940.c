#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include "no-os/error.h"
#include "no-os/delay.h"
#include "iio.h"
#include "iio_ad5940.h"
#include "no-os/util.h"

static int32_t _ad5940_read_register2(struct ad5940_iio_dev *dev, uint32_t reg,
				      uint32_t *readval)
{
	return ad5940_ReadReg(dev->ad5940, reg, readval);
}

static int32_t _ad5940_write_register2(struct ad5940_iio_dev *dev, uint32_t reg,
				       uint32_t writeval)
{
	return ad5940_WriteReg(dev->ad5940, reg, writeval);
}

static int ad5940_iio_read_chan_raw(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int ret;
	uint32_t mux, mask, flag;
	int32_t value;
	uint8_t gp_high = 1;
	struct ad5940_iio_dev *iiodev = (struct ad5940_iio_dev *)device;
	struct iio_device *iiod = iiodev->iio;

	mux = field_prep(BITM_AFE_ADCCON_MUXSELP, iiod->channels[channel->address].channel);
	mux |= field_prep(BITM_AFE_ADCCON_MUXSELN, iiod->channels[channel->address].channel2);
	ret = ad5940_WriteReg_mask(iiodev->ad5940, REG_AFE_ADCCON,
			BITM_AFE_ADCCON_MUXSELN | BITM_AFE_ADCCON_MUXSELP, mux);
	if (ret < 0)
		return ret;

	if (iiodev->source == AFERESULT_SINC3)
		flag = AFEINTSRC_ADCRDY;
	else
		flag = AFEINTSRC_DFTRDY;

	mask = BITM_AFE_AFECON_ADCCONVEN;
	value = field_prep(mask, 1);
	ret = ad5940_WriteReg_mask(iiodev->ad5940, REG_AFE_AFECON, mask, value);
	if (ret < 0)
		return ret;

	while(gp_high) { // TODO: timebound this loop
		ret = gpio_get_value(iiodev->ad5940->gp0_gpio, &gp_high);
		if (ret < 0)
			return ret;
	}

	value = field_prep(mask, 0);
	ret = ad5940_WriteReg_mask(iiodev->ad5940, REG_AFE_AFECON, mask, value);
	if (ret < 0)
		return ret;

	ret = ad5940_INTCClrFlag(iiodev->ad5940, flag);
	if (ret < 0)
		return ret;

	ret = ad5940_ReadAfeResult(iiodev->ad5940, iiodev->source, (uint32_t *)&value);
	if (ret < 0)
		return ret;

	value &= 0xffff;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &value);
}
/*
static int ad5940_iio_read_chan_scale(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int32_t vals[2] = {
			1820, // mV, fixed voltage reference
			16
	};

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, vals);
}
*/
/*
static int ad5940_iio_read_chan_offset(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int32_t val = -32768;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}
*/

int ad5940_iio_get_attr(void *device, char *buf, uint32_t len,
		      const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad5940_iio_dev *iiodev = (struct ad5940_iio_dev *)device;
	int32_t val;

	switch(priv) {
	case AD5940_IIO_SOURCE:
		val = iiodev->source;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	default:
		return -EINVAL;
	}
}

int ad5940_iio_set_attr(void *device, char *buf, uint32_t len,
		      const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad5940_iio_dev *iiodev = (struct ad5940_iio_dev *)device;
	int32_t val, val2, ret;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, &val2);
	if (ret < 0)
		return ret;

	switch(priv) {
	case AD5940_IIO_SOURCE:
		switch (val) {
		case AFERESULT_SINC3:
		case AFERESULT_DFTREAL:
		case AFERESULT_DFTIMAGE:
			/* only allow these as data source */
			break;
		default:
			return -EINVAL;
		}

		iiodev->source = val;
		return 0;
	default:
		return -EINVAL;
	}
}

struct iio_attribute ad5940_iio_global_attr[] = {
	{
		.name = "source",
		.priv = AD5940_IIO_SOURCE,
		.show = ad5940_iio_get_attr,
		.store = ad5940_iio_set_attr,
	},
	END_ATTRIBUTES_ARRAY,
};

static struct iio_device ad5940_iio_device = {
	.attributes = ad5940_iio_global_attr,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable = NULL,
	.post_disable = NULL,
	.read_dev = NULL,
	.debug_reg_read = (int32_t (*)())_ad5940_read_register2,
	.debug_reg_write = (int32_t (*)())_ad5940_write_register2
};

static struct iio_attribute ad5940_channel_attributes[] = {
	{
		.name = "raw",
		.show = ad5940_iio_read_chan_raw,
	},
	/*
	{
		.name = "scale",
		.show = ad5940_iio_read_chan_scale,
	},
	*/
	/*
	{
		.name = "offset",
		.show = ad5940_iio_read_chan_offset,
	},
	*/
	END_ATTRIBUTES_ARRAY
};

int32_t ad5940_iio_init(struct ad5940_iio_dev **iio_dev,
			struct ad5940_iio_init_param *init_param)
{
	int32_t ret;
	struct ad5940_iio_dev *desc;
	uint16_t ch;

	if (!iio_dev || !init_param)
		return -EINVAL;

	desc = (struct ad5940_iio_dev *)calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio = &ad5940_iio_device;

	desc->iio->channels = (struct iio_channel *)
			calloc(init_param->chan_init_count, sizeof(struct iio_channel));
	if (!desc->iio->channels)
		goto error_1;
	desc->iio->num_ch = init_param->chan_init_count;

	for (ch = 0; ch < init_param->chan_init_count; ch++) {
		desc->iio->channels[ch].name = init_param->chan_init[ch].name;
		desc->iio->channels[ch].ch_type = IIO_VOLTAGE;
		desc->iio->channels[ch].indexed = true;
		desc->iio->channels[ch].diferential = true;
		desc->iio->channels[ch].channel = init_param->chan_init[ch].pos;
		desc->iio->channels[ch].channel2 = init_param->chan_init[ch].neg;
		desc->iio->channels[ch].address = ch;
		desc->iio->channels[ch].scan_index = ch;
		desc->iio->channels[ch].attributes = ad5940_channel_attributes;
	}

	ret = ad5940_init(&desc->ad5940, init_param->ad5940_init);
	if (ret)
		goto error_2;

	ret = ad5940_WriteReg(desc->ad5940, REG_AFE_PMBW, ENUM_AFE_PMBW_LP | ENUM_AFE_PMBW_BW250);
	if (ret < 0)
		goto error_2;

	ret = ad5940_INTCCfg(desc->ad5940, AFEINTC_0, AFEINTSRC_ADCRDY | AFEINTSRC_DFTRDY, true);
	if (ret < 0)
		goto error_2;

	ret = ad5940_WriteReg_mask(desc->ad5940, REG_AFE_AFECON,
			BITM_AFE_AFECON_ADCEN | BITM_AFE_AFECON_DFTEN,
			BITM_AFE_AFECON_ADCEN | BITM_AFE_AFECON_DFTEN);
	if (ret < 0)
		goto error_2;
/*
	DFTCfg_Type dftcfg = {
			.DftNum = DFTNUM_4,
			.DftSrc = DFTSRC_SINC3,
			.HanWinEn = true,
	};

	ret = ad5940_DFTCfgS(desc->ad5940, &dftcfg);
	if (ret < 0)
		return ret;
*/

	ADCFilterCfg_Type ADCFilterCfg = {
		.ADCAvgNum = ADCAVGNUM_16,
		.ADCRate = ADCRATE_800KHZ,
		.ADCSinc3Osr = ADCSINC3OSR_2,
		.ADCSinc2Osr = ADCSINC2OSR_178,
		.BpNotch = true,
		.Sinc2NotchEnable = true,
		.Sinc2NotchClkEnable = true,
		.Sinc3ClkEnable = true,
		.WGClkEnable = true,
		.DFTClkEnable = true,
	};
	ret = ad5940_ADCFilterCfgS(desc->ad5940, &ADCFilterCfg);
	if (ret < 0)
		return ret;

	*iio_dev = desc;

	return SUCCESS;
error_2:
	free(desc->iio->channels);
error_1:
	free(desc);

	return ret;
}

int32_t ad5940_iio_remove(struct ad5940_iio_dev *desc)
{
	int32_t ret;

	ret = ad5940_remove(desc->ad5940);
	if (ret != SUCCESS)
		return ret;

	free(desc->iio->channels);
	free(desc);

	return SUCCESS;
}
