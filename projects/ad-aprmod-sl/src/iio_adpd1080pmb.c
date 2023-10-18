/***************************************************************************//**
 *   @file   iio_adpd1080pmb.c
 *   @brief  Implementation of the ADPD1080 IIO driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#include <stdint.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "iio.h"
#include "iio_adpd1080pmb.h"

char *gestures[6] = {
	"click",
	"up",
	"down",
	"left",
	"right",
};

enum adpd1080pmb_dev_attrs {
	ADPD1080PMB_DEV_ATTR_GESTURES,
	ADPD1080PMB_DEV_ATTR_TH_INTENSITY,
	ADPD1080PMB_DEV_ATTR_TH_CLICK,
};

int adpd1080pmb_get_fifo_data(struct adpd188_dev *desc, int32_t *buff)
{
	uint8_t byte_no;
	uint32_t s;
	uint16_t data_buff[16];
	int32_t ret;

	ret = adpd188_fifo_status_get(desc, &byte_no);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;
	
	if (byte_no < 16)
		return -EAGAIN; // nothing to do (yet)

	for (s = 0; s < 16; s++) {
		ret = adpd188_reg_read(desc, ADPD188_REG_FIFO_ACCESS,
					&data_buff[s]);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;
		
		if (s & 0x1)
			buff[s/2] = ((uint16_t)data_buff[s] << 16) | data_buff[s - 1];
	}

	return 0;
}

static unsigned isqrt(unsigned long val) {
    unsigned long temp, g=0, b = 0x8000, bshft = 15;
    do {
        if (val >= (temp = (((g << 1) + b)<<bshft--))) {
           g += b;
           val -= temp;
        }
    } while (b >>= 1);
    return g;
}

int adpd1080pmb_gesture_detection(struct adpd1080pmb_iio_desc *iiodev)
{
	struct adpd188_dev *desc = iiodev->dev;
	int32_t buff[8];
	int x, y, dx, dy, L, m, d;
	int gestureStopX, gestureStopY;
	enum gesture gesture;

	if (adpd1080pmb_get_fifo_data(desc, buff) == -EAGAIN) {
		// no data, no problem, let iiod run
		return 0;
	}
	
	// This algorithm is provided in the adpd2140 datasheet
	x = (buff[1] - buff[0]) * 1000 / (buff[1] + buff[0]);
	y = (buff[3] - buff[2]) * 1000 / (buff[3] + buff[2]);
	L = (buff[0] + buff[1] + buff[2] + buff[3]) * 1000;

	if (!iiodev->event && L > iiodev->th_intensity)
	{
		iiodev->evc = 0;
		iiodev->event = true;
		iiodev->gestureStartX = x;
		iiodev->gestureStartY = y;
	}

	if (iiodev->event) {
		iiodev->evc += 1;
		if (iiodev->evc >= 5 && L < iiodev->th_intensity) {
			iiodev->event = false;
			gestureStopX = x;
			gestureStopY = y;
			dx = iiodev->gestureStartX - gestureStopX;
			dy = iiodev->gestureStartY - gestureStopY;
			m = dy * 1000 / dx;
			d = isqrt(dx * dx + dy * dy);
		}
		if (d < iiodev->th_click)
			gesture = click;
		else {
			if (abs(m) > 1000) {
				if (iiodev->gestureStartY > gestureStopY)
					gesture = up;
				else
					gesture = down;
			}
			else {
				if (iiodev->gestureStartX > gestureStopX)
					gesture = right;
				else
					gesture = left;
			}
		}
		if (!iiodev->event) {
			iiodev->gestures |= NO_OS_BIT(gesture);
			iiodev->d_gestures |= NO_OS_BIT(gesture);
		}
	}

	return 0;
}

int32_t adpd1080pmb_iio_init(struct adpd1080pmb_iio_desc **iiodev,
			 struct adpd1080pmb_iio_init_param *iiodevconfig)
{
	int ret;
	struct adpd1080pmb_iio_desc *d = (struct adpd1080pmb_iio_desc *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->dev = iiodevconfig->dev;
	d->event = false;
	d->evc = 0;
	d->th_click = iiodevconfig->th_click;
	d->th_intensity = iiodevconfig->th_intensity;

	*iiodev = d;

	return 0;
}

int32_t adpd1080pmb_iio_remove(struct adpd1080pmb_iio_desc *iiodev)
{
	no_os_free(iiodev);
	return 0;
}

static int adpd1080pmb_attr_read(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int ret;
	struct adpd1080pmb_iio_desc *iiodev = (struct adpd1080pmb_iio_desc *)device;
	int32_t vals[2];
	int type;
	int valcount;


	switch (priv) {
	case ADPD1080PMB_DEV_ATTR_GESTURES:
		vals[0] = iiodev->gestures;
		iiodev->gestures = 0; // clear on read
		type = IIO_VAL_INT;
		valcount = 1;
		break;
	case ADPD1080PMB_DEV_ATTR_TH_INTENSITY:
		vals[0] = iiodev->th_intensity;
		type = IIO_VAL_INT;
		valcount = 1;
		break;
	case ADPD1080PMB_DEV_ATTR_TH_CLICK:
		vals[0] = iiodev->th_click;
		type = IIO_VAL_INT;
		valcount = 1;
		break;
	default:
		return -EINVAL;
	};

	return iio_format_value(buf, len, type, valcount, vals);
}

static int adpd1080pmb_attr_write(void *device, char *buf,
				  uint32_t len, const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int ret;
	int32_t val;
	struct adpd1080pmb_iio_desc *iiodev = (struct adpd1080pmb_iio_desc *)device;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	if (ret < 0)
		return ret;

	switch (priv) {
	case ADPD1080PMB_DEV_ATTR_TH_INTENSITY:
		if (val > 2000000)
			return -EINVAL;
		iiodev->th_intensity = val;
		break;
	case ADPD1080PMB_DEV_ATTR_TH_CLICK:
		if (val > 1000)
			return -EINVAL;
		iiodev->th_click = val;
		break;
	default:
		return -EINVAL;
	};

	return 0;
}

static struct iio_attribute adpd1080pmb_device_attributes[] = {
	{
		.name = "gestures",
		.priv = ADPD1080PMB_DEV_ATTR_GESTURES,
		.show = adpd1080pmb_attr_read,
	},
	{
		.name = "th_intensity",
		.priv = ADPD1080PMB_DEV_ATTR_TH_INTENSITY,
		.show = adpd1080pmb_attr_read,
		.store = adpd1080pmb_attr_write,
	},
	{
		.name = "th_click",
		.priv = ADPD1080PMB_DEV_ATTR_TH_CLICK,
		.show = adpd1080pmb_attr_read,
		.store = adpd1080pmb_attr_write,
	},
	END_ATTRIBUTES_ARRAY
};

struct iio_device iio_adpd1080pmb_device = {
	.attributes = adpd1080pmb_device_attributes,
};

