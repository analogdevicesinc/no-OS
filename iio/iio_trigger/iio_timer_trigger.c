/**************************************************************************//**
*   @file   iio_ad3552r.c
*   @brief  IIO implementation for ad3552r Driver
*   @author Mihail Chindris (Mihail.Chindris@analog.com)
*
*******************************************************************************
* Copyright 2021(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification,
* are permitted provided that the following conditions are met:
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
* INCIDENTAL,SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include "iio_types.h"
#include "iio_timer_trigger.h"
#include "error.h"
#include "util.h"
#include "list.h"
#include <pthread.h>
#include <unistd.h>

/*****************************************************************************/
/******************** Macros and Constants Definitions ***********************/
/*****************************************************************************/

#define IIO_SAMPLING_FREQUENCY 0

#define STR(x) #x
#define XSTR(x) STR(x)

#define IIO_TRIGGER_ATTR(_name, _priv) {\
	.name = _name,\
	.priv = _priv,\
	.show = iio_attr_get,\
	.store = iio_attr_set\
}

struct iio_timer_trigger_desc {
	struct timer_desc	*timer;
	struct list_desc	*callbacks;
	struct iterator		*calls_iter;
};

/*****************************************************************************/
/************************* Functions Definitions *****************************/
/*****************************************************************************/

struct timer_trigger_callback {
	void (*callback)(void * dev);
	void *dev;
};

static int32_t iio_cmp_interfaces(struct timer_trigger_callback *a,
				  struct timer_trigger_callback *b)
{
	if (a->callback == b->callback && a->dev == b->dev)
		return 0;

	return -1;
}

void thread_function(struct iio_timer_trigger_desc *desc)
{
	struct timer_trigger_callback *call;
	int32_t err;

	while(true)
	{
		err = iterator_move_to_idx(desc->calls_iter, 0);
		while (!IS_ERR_VALUE(err)) {
			printf("Doing read\n");
			err = iterator_read(desc->calls_iter, (void **)&call);
			printf("Read done\n");
			call->callback(call->dev);
			err = iterator_move(desc->calls_iter, 1);
		}
		printf("Hello from thread\n");
		usleep(1000000);
	}
}
/* Init iio. */
int32_t iio_timer_trigger_init(struct iio_timer_trigger_desc **desc,
			       struct iio_timer_trigger_init_param *param)
{
	struct iio_timer_trigger_desc *ldesc;
	int32_t ret;

	if (!param || !desc)
		return FAILURE;

	ldesc = (struct iio_timer_trigger_desc *)calloc(1,
			sizeof(struct iio_timer_trigger_desc));
	if (!ldesc)
		return -ENOMEM;

	
	ret = list_init(&ldesc->callbacks, LIST_DEFAULT,
			(f_cmp)iio_cmp_interfaces);
	if (IS_ERR_VALUE(ret))
		goto err_desc;
	
	*desc = ldesc;
		
	ret = iterator_init(&ldesc->calls_iter, ldesc->callbacks, 1);
	if (IS_ERR_VALUE(ret))
		goto err_list;

	pthread_t pid;
	pthread_create(&pid, NULL, thread_function, ldesc);

	return 0;
err_desc:
	free(ldesc);
err_list:
	list_remove(ldesc->callbacks);
	return ret;
}

/* Free the resources allocated by iio_timer_trigger_init(). */
int32_t iio_timer_trigger_remove(struct iio_timer_trigger_desc *desc)
{
	struct timer_trigger_callback	*call;
	int32_t				err;

	if (!desc)
		return -EINVAL;

	while (true) {
		err = desc->callbacks->pop(desc->callbacks, (void **)&call);
		if (IS_ERR_VALUE(err))
			break;
		free(call);
	}

	iterator_remove(desc->calls_iter);
	list_remove(desc->callbacks);
	free(desc);

	return 0;
}

static ssize_t iio_attr_get(void *device, char *buf, size_t len,
			    const struct iio_ch_info *channel,
			    intptr_t priv)
{
	int ok = 0;
        if (priv == IIO_SAMPLING_FREQUENCY)
                ok++;

        return -EINVAL;

	// uint32_t	val;
	// uint16_t	val16;
	// int32_t		ret;
	// float		val2;

	// val = 0;
	// switch (priv) {
	// case IIO_SAMPLING_FREQUENCY:
	// 	return -EINVAL;
	// default:
        //         ret = ad3552r_get_ch_voltage(device, channel->ch_num, &val2);
	// 	if (IS_ERR_VALUE(ret))
	// 		return ret;

	// 	return snprintf(buf, len, "%.3f", val2);
	// 	if (channel) {
	// 		ret = ad3552r_get_ch_value(device, priv,
	// 					   channel->ch_num,
	// 					   &val16);
	// 		val = val16;
	// 	} else {
	// 		ret = ad3552r_get_dev_value(device, priv, &val);
	// 	}
	// 	break;
	// }

	// if (IS_ERR_VALUE(ret))
	// 	return ret;

	// return snprintf(buf, len, "%"PRIu32"", val);
}

static ssize_t iio_attr_set(void *device, char *buf, size_t len,
			    const struct iio_ch_info *channel, intptr_t priv)
{
	int ok = 0;
        if (priv == IIO_SAMPLING_FREQUENCY)
                ok++;

        return -EINVAL;

	// uint32_t val;
	// int32_t	 ret;
	// float	 val2;

	// val = srt_to_uint32(buf);
	// switch (priv) {
	// case AD3552R_CH_VOLTAGE:
	// 	ret = sscanf(buf, "%f", &val2);
	// 	if (ret != 1)
	// 		return -EINVAL;

	// 	ret = ad3552r_set_ch_voltage(device, channel->ch_num, val2);
	// 	if (IS_ERR_VALUE(ret))
	// 		return ret;

	// 	return len;
	// default:
	// 	if (channel)
	// 		ret = ad3552r_set_ch_value(device, priv,
	// 					   channel->ch_num,
	// 					   val);
	// 	else
	// 		ret = ad3552r_set_dev_value(device, priv, val);
	// 	break;
	// }

	// if (IS_ERR_VALUE(ret))
	// 	return ret;

	// return len;
}

int32_t iio_timer_trigger_add_callback(struct iio_timer_trigger_desc *desc,
				       void (*callback)(void * dev),
				       void *dev)
{
	struct timer_trigger_callback	*call;
	int32_t				err;

	if (!desc)
		return -EINVAL;

	call = (struct timer_trigger_callback *)calloc(1, sizeof(call));
	if (!call)
		return -ENOMEM;

	call->callback = callback;
	call->dev = dev;
	err = desc->callbacks->push(desc->callbacks, call);
	if (IS_ERR_VALUE(err)) {
		free(call);
		return err;
	}

	return 0;
}

int32_t iio_timer_trigger_remove_callback(struct iio_timer_trigger_desc *desc,
					  void (*callback)(void * dev),
					  void *dev)
{
	struct timer_trigger_callback	*call;
	struct timer_trigger_callback	cmp_call;
	int32_t				err;

	if (!desc)
		return -EINVAL;

	cmp_call.callback = callback;
	cmp_call.dev = dev;
	err = list_get_find(desc->callbacks, (void **)&call, &cmp_call);
	if (IS_ERR_VALUE(err))
		return err;

	free(call);

	return 0;
}

/******************************************************************************/
/************************** IIO Types Declarations *****************************/
/******************************************************************************/

struct iio_attribute iio_timer_trigger_attributes[] = {
	IIO_TRIGGER_ATTR("sampling_frequency", IIO_SAMPLING_FREQUENCY),
	END_ATTRIBUTES_ARRAY,
};

struct iio_device iio_timer_trigger_device_desc = {
	.is_trigger = true,
	.attributes = iio_timer_trigger_attributes,
	.add_callback = iio_timer_trigger_add_callback,
	.remove_callback = iio_timer_trigger_remove_callback,
};