/***************************************************************************//**
 *   @file   mqtt_noos_support.c
 *   @brief  Implementation file used to port the MQTT paho to use no-os
 *   @author Mihail Chindris (mihail.chindris@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "mqtt_noos_support.h"
#include <stdlib.h>
#include "no_os_timer.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_error.h"

#ifdef NO_OS_LWIP_NETWORKING
#include "lwip_socket.h"
#endif

/******************************************************************************/
/**************************** Global Variables ********************************/
/******************************************************************************/

/* Timer reference used by the functions */
static struct no_os_timer_desc	*timer;

/* Number of timer references */
static uint32_t			nb_references;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/* Allcoate resources for timer. Must be called from mqqt_init */
int32_t mqtt_timer_init(struct no_os_timer_init_param *timer_init_param)
{
	int32_t			ret;

	ret = no_os_timer_init(&timer, timer_init_param);
	if (NO_OS_IS_ERR_VALUE(ret)) {
		timer = NULL;
		return -1;
	}

	ret = no_os_timer_start(timer);
	if (NO_OS_IS_ERR_VALUE(ret)) {
		no_os_timer_remove(timer);
		timer = NULL;
		return -1;
	}

	nb_references++;

	return 0;
}

/* Remove resources allocated with \ref mqtt_timere_init. */
void mqtt_timer_remove()
{
	nb_references--;
	if (!nb_references) {
		no_os_timer_remove(timer);
		timer = NULL;
	}
}

/* Implementation of TimerInit used by MQTTClient.c */
void TimerInit(Timer* t)
{
	NO_OS_UNUSED_PARAM(t);
	/* Do nothing */
}

/* Implementation of TimerCountdownMS used by MQTTClient.c */
void TimerCountdownMS(Timer* t, unsigned int ms)
{
	no_os_timer_counter_get(timer, &t->start_time);
	t->ms = ms;
}

/* Implementation of TimerCountdown used by MQTTClient.c */
void TimerCountdown(Timer* t, unsigned int seconds)
{
	no_os_timer_counter_get(timer, &t->start_time);
	t->ms = seconds * 1000;
}

/* Implementation of TimerLeftMS used by MQTTClient.c */
int TimerLeftMS(Timer* t)
{
	uint32_t ms;

	no_os_timer_counter_get(timer, &ms);
	ms -= t->start_time;

	if (ms > t->ms)
		return 0;
	else
		return (t->ms - ms);
}

/* Implementation of TimerIsExpired used by MQTTClient.c */
char TimerIsExpired(Timer* t)
{
	if (TimerLeftMS(t) == 0)
		return true;

	return false;
}

/* Implementation of mqtt_noos_read used by MQTTClient.c */
int mqtt_noos_read(Network* net, unsigned char* buff, int len, int timeout)
{
	uint32_t	sent;
	int32_t		rc;

	if (!len)
		return 0;

	sent = 0;
	do {
#ifdef NO_OS_LWIP_NETWORKING
		/*
		 * Currently, the LWIP networking layer doesn't implement packet RX
		 * using interrupts, so we have to poll.
		 */
		no_os_lwip_step(net->sock->net->net, NULL);
#endif
		rc = socket_recv(net->sock, (void *)(buff + sent),
				 (uint32_t)(len - sent));
		if (rc != -EAGAIN) { //If data available or error
			if (NO_OS_IS_ERR_VALUE(rc))
				return rc;

			sent += rc;
			if (sent >= len)
				return sent;
		}

		no_os_mdelay(1);
	} while (--timeout);

	/* 0 bytes have been read */
	return 0;
}

/* Implementation of mqtt_noos_write used by MQTTClient.c */
int mqtt_noos_write(Network* net, unsigned char* buff, int len, int timeout)
{
	/* non blocking read is not implemented */
	NO_OS_UNUSED_PARAM(timeout);

	return socket_send(net->sock, (const void *)buff, (uint32_t)len);
}
