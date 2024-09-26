/***************************************************************************//**
 *   @file   mqtt_noos_support.h
 *   @brief  Header file used to port the MQTT paho to use no-os
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#ifndef MQTT_NOOS
#define MQTT_NOOS

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "tcp_socket.h"
#include "no_os_timer.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/** Typedef for \ref timer_port_noos */
typedef struct timer_port_noos		Timer;

/** Typedef for \ref network_port_noos */
typedef struct network_port_noos	Network;

/**
 * @struct timer_port_noos
 * @brief Timer structure used by MQTTClient.
 */
struct timer_port_noos {
	/** Time when the countdown is started */
	uint32_t	start_time;
	/** Time when the countdown value */
	uint32_t	ms;
};

/**
 * @struct network_port_noos
 * @brief Network structure used by MQTTClient.
 */
struct network_port_noos {
	/** Reference to no-os socket */
	struct tcp_socket_desc	*sock;
	/** Reference to no-os network wrapper read function */
	int			(*mqttread)(Network*, unsigned char*, int, int);
	/** Reference to no-os network wrapper write function */
	int			(*mqttwrite)(Network*, unsigned char*, int,
					     int);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Init porting file */
int32_t mqtt_timer_init(struct no_os_timer_init_param *timer_init_param);
/* Uninit porting file */
void mqtt_timer_remove();

/* Function to be linked to Network.mqttread */
int mqtt_noos_read(Network*, unsigned char*, int, int);
/* Function to be linked to Network.mqttwrite */
int mqtt_noos_write(Network*, unsigned char*, int, int);

#endif
