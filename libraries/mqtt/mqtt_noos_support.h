/***************************************************************************//**
 *   @file   mqtt_noos_support.h
 *   @brief  Header file used to port the MQTT paho to use no-os
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
