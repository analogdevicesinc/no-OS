/***************************************************************************//**
 *   @file   iio_ad5940.c
 *   @brief  Source file of ad5940 iio driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "iio.h"
#include "iio_ad5940.h"
#include "bia_measurement.h"

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
	int32_t values[2];
	AppBiaCfg_Type *pBiaCfg;
	struct ad5940_iio_dev *iiodev = (struct ad5940_iio_dev *)device;
	int32_t *pval;
	fImpCar_Type fCarZval;
	iImpCar_Type iCarVval;
	float fMagVal;
	uint32_t timeout = 100;
	uint8_t gpio;
	uint32_t count = 0;

	AppBiaGetCfg(&pBiaCfg);
	if(pBiaCfg->bParamsChanged)
		AppBiaInit(iiodev->ad5940, iiodev->AppBuff, 512);

	AppBiaCtrl(iiodev->ad5940, BIACTRL_START, 0);

	while (timeout--) {
		no_os_gpio_get_value(iiodev->ad5940->gp0_gpio, &gpio);
		if (gpio)
			no_os_mdelay(1);
		else
			break;
	}

	if (!timeout) {
		AppBiaCtrl(iiodev->ad5940, BIACTRL_STOPNOW, 0);
		return -ETIMEDOUT;
	}

	AppBiaISR(iiodev->ad5940, iiodev->AppBuff, &count);
	AppBiaCtrl(iiodev->ad5940, BIACTRL_STOPNOW, 0);

	signExtend18To32(iiodev->AppBuff, count);

	if (pBiaCfg->bImpedanceReadMode) {
		fCarZval = computeImpedance(iiodev->AppBuff);
		if (iiodev->magnitude_mode) {
			// respond with magnitude of impedance (one ieee754 float value)
			fMagVal = sqrt(fCarZval.Real*fCarZval.Real +
				       fCarZval.Image*fCarZval.Image);
			pval = (int32_t *)&fMagVal;
			values[0] = *pval;
			return iio_format_value(buf, len, IIO_VAL_INT, 1, values);
		} else {
			// respond with impedance as a complex number (two ieee754 float values)
			pval = (int32_t *)&fCarZval.Real;
			values[0] = *pval;
			pval = (int32_t *)&fCarZval.Image;
			values[1] = *pval;
			return iio_format_value(buf, len, IIO_VAL_INT_MULTIPLE, 2, values);
		}
	} else {
		if (iiodev->magnitude_mode) {
			// respond with magnitude of voltage only (one ieee754 float value)
			iCarVval = *((iImpCar_Type *)iiodev->AppBuff);
			fMagVal = sqrt((iCarVval.Real*1.0)*(iCarVval.Real*1.0) +
				       (iCarVval.Image*1.0)*(iCarVval.Image*1.0));
			pval = (int32_t *)&fMagVal;
			values[0] = *pval;
			return iio_format_value(buf, len, IIO_VAL_INT, 1, values);
		} else {
			// respond with raw complex voltage (two integer values)
			return iio_format_value(buf, len, IIO_VAL_INT_MULTIPLE, 2,
						(int32_t *)iiodev->AppBuff);
		}
	}
}

int ad5940_iio_get_attr(void *device, char *buf, uint32_t len,
			const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad5940_iio_dev *iiodev = (struct ad5940_iio_dev *)device;
	AppBiaCfg_Type *pBiaCfg;
	int32_t val;

	AppBiaGetCfg(&pBiaCfg);

	switch(priv) {
	case AD5940_IIO_IMPEDANCE_MODE:
		val = (int32_t)pBiaCfg->bImpedanceReadMode;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5940_IIO_MAGNITUDE_MODE:
		val = (int32_t)iiodev->magnitude_mode;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5940_IIO_EXCITATION_FREQUENCY:
		val = (int32_t)pBiaCfg->SinFreq;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case AD5940_IIO_EXCITATION_AMPLITUDE:
		val = (int32_t)pBiaCfg->DacVoltPP;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5940_IIO_GPIO1_TOGGLE:
		val = (int32_t)iiodev->gpio1;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	default:
		return -EINVAL;
	}
}

int ad5940_iio_set_attr(void *device, char *buf, uint32_t len,
			const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad5940_iio_dev *iiodev = (struct ad5940_iio_dev *)device;
	int32_t val, val2, ret;
	AppBiaCfg_Type *pBiaCfg;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, &val2);
	if (ret < 0)
		return ret;

	AppBiaGetCfg(&pBiaCfg);

	switch(priv) {
	case AD5940_IIO_IMPEDANCE_MODE:
		if (val < 0 || val > 1)
			return -EINVAL;
		if (pBiaCfg->bImpedanceReadMode != (bool)val) {
			pBiaCfg->bImpedanceReadMode = (bool)val;
			if(pBiaCfg->bImpedanceReadMode)
				pBiaCfg->FifoThresh = 4;
			else
				pBiaCfg->FifoThresh = 2;
			pBiaCfg->bParamsChanged = true;
		}
		break;
	case AD5940_IIO_MAGNITUDE_MODE:
		if (val < 0 || val > 1)
			return -EINVAL;
		if (iiodev->magnitude_mode != (bool)val) {
			iiodev->magnitude_mode = (bool)val;
		}
		break;
	/* Excitation signal frequency. */
	case AD5940_IIO_EXCITATION_FREQUENCY:
		if (val < 0 || val > 250000)
			return -EINVAL;
		if (pBiaCfg->SinFreq != (float)val) {
			pBiaCfg->SinFreq = val;
			pBiaCfg->bParamsChanged = true;
		}
		break;
	/* Excitation signal point to point amplitude */
	case AD5940_IIO_EXCITATION_AMPLITUDE:
		if (val < 0 || val > 800)
			return -EINVAL;
		if (pBiaCfg->DacVoltPP != (float)val) {
			pBiaCfg->DacVoltPP = val;
			pBiaCfg->bParamsChanged = true;
		}
		break;
	/* GPIO1 control */
	case AD5940_IIO_GPIO1_TOGGLE:
		if (val < 0 || val > 1)
			return -EINVAL;

		ret = ad5940_WriteReg(iiodev->ad5940, REG_AGPIO_GP0OUT, 0);
		if (ret < 0)
			return ret;
		no_os_udelay(10);
		ret = ad5940_WriteReg(iiodev->ad5940, REG_AGPIO_GP0OUT, AGPIO_Pin1);
		if (ret < 0)
			return ret;

		iiodev->gpio1 = val;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

struct iio_attribute ad5940_iio_global_attr[] = {
	{
		.name = "impedance_mode",
		.priv = AD5940_IIO_IMPEDANCE_MODE,
		.show = ad5940_iio_get_attr,
		.store = ad5940_iio_set_attr,
	},
	{
		.name = "magnitude_mode",
		.priv = AD5940_IIO_MAGNITUDE_MODE,
		.show = ad5940_iio_get_attr,
		.store = ad5940_iio_set_attr,
	},
	{
		.name = "excitation_frequency",
		.priv = AD5940_IIO_EXCITATION_FREQUENCY,
		.show = ad5940_iio_get_attr,
		.store = ad5940_iio_set_attr,
	},
	{
		.name = "excitation_amplitude",
		.priv = AD5940_IIO_EXCITATION_AMPLITUDE,
		.show = ad5940_iio_get_attr,
		.store = ad5940_iio_set_attr,
	},
	{
		.name = "gpio1_toggle",
		.priv = AD5940_IIO_GPIO1_TOGGLE,
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
	END_ATTRIBUTES_ARRAY
};

int32_t ad5940_iio_init(struct ad5940_iio_dev **iio_dev,
			struct ad5940_iio_init_param *init_param)
{
	int32_t ret;
	struct ad5940_iio_dev *desc;
	AppBiaCfg_Type *pBiaCfg;
	uint16_t ch;

	if (!iio_dev || !init_param)
		return -EINVAL;

	desc = (struct ad5940_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio = &ad5940_iio_device;

	desc->iio->channels = (struct iio_channel *)no_os_calloc(1,
			      sizeof(struct iio_channel));
	if (!desc->iio->channels)
		goto error_1;
	desc->iio->num_ch = 1;

	ch = 0;
	desc->iio->channels[ch].name = "bia";
	desc->iio->channels[ch].ch_type = IIO_VOLTAGE;
	desc->iio->channels[ch].indexed = true;
	desc->iio->channels[ch].attributes = ad5940_channel_attributes;

	ret = ad5940_init(&desc->ad5940, init_param->ad5940_init);
	if (ret)
		goto error_2;

	AppBiaGetCfg(&pBiaCfg);
	pBiaCfg->bParamsChanged = true;

	pBiaCfg->SeqStartAddr = 0;
	pBiaCfg->MaxSeqLen = 512; /** @todo add checker in function */

	pBiaCfg->RcalVal = 1000.0;
	pBiaCfg->DftNum = DFTNUM_8192;
	pBiaCfg->NumOfData = -1; /* Never stop until you stop it manually by
							  * AppBiaCtrl(dev, ) function */
	pBiaCfg->BiaODR = 20;	 /* ODR(Sample Rate) 20Hz */
	pBiaCfg->FifoThresh = 2;
	pBiaCfg->ADCSinc3Osr = ADCSINC3OSR_2;

	pBiaCfg->DacVoltPP = 300.0; //800.0
	pBiaCfg->SinFreq = 10000.0; /* 10000Hz */
	pBiaCfg->SweepCfg.SweepEn = false;
	pBiaCfg->SweepCfg.SweepStart = 10000;
	pBiaCfg->SweepCfg.SweepStop = 80000.0;
	pBiaCfg->SweepCfg.SweepPoints = 20;
	pBiaCfg->SweepCfg.SweepLog = true;
	pBiaCfg->SweepCfg.SweepIndex = 0;

	ret = AppBiaInit(desc->ad5940, desc->AppBuff, 512);
	if (ret < 0)
		goto error_2;

	*iio_dev = desc;

	return 0;
error_2:
	no_os_free(desc->iio->channels);
error_1:
	no_os_free(desc);

	return ret;
}

int32_t ad5940_iio_remove(struct ad5940_iio_dev *desc)
{
	int32_t ret;

	ret = ad5940_remove(desc->ad5940);
	if (ret != 0)
		return ret;

	no_os_free(desc->iio->channels);
	no_os_free(desc);

	return 0;
}
