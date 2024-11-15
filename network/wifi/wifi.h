/***************************************************************************//**
 *   @file   wifi.h
 *   @brief  Wifi interface for ESP8266
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 *   @copyright
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef WIFI_DRIVER_H
#define WIFI_DRIVER_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "network_interface.h"
#include "no_os_uart.h"
#include "no_os_irq.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct wifi_desc
 * @brief Wifi descriptor
 */
struct wifi_desc;

/**
 * @struct wifi_init_param
 * @brief Parameter to initialize Wifi
 */
struct wifi_init_param {
	/** Uart descriptor where ESP8266 is connected */
	struct no_os_uart_desc	*uart_desc;
	/** Irq controler descriptor */
	struct no_os_irq_ctrl_desc	*irq_desc;
	/** Id of the UART interrupt */
	uint32_t		uart_irq_id;
	/** Configuration param for registering uart callback */
	void			*uart_irq_conf;
	/** ESP8266 Software reset enable */
	bool			sw_reset_en;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Wifi init */
int32_t wifi_init(struct wifi_desc **desc, struct wifi_init_param *param);
/* Wifi remove */
int32_t wifi_remove(struct wifi_desc *desc);
/* Wifi connect */
int32_t wifi_connect(struct wifi_desc *desc, const char *ssid,
		     const char *pass);
/* Wifi disconnect */
int32_t wifi_disconnect(struct wifi_desc *desc);
/* Wifi get network interface */
int32_t wifi_get_network_interface(struct wifi_desc *desc,
				   struct network_interface **net);
/* Wifi get ip interface */
int32_t wifi_get_ip(struct wifi_desc *desc, char *ip_buff, uint32_t buff_size);

#endif
