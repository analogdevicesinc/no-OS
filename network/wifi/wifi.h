/***************************************************************************//**
 *   @file   wifi.h
 *   @brief  Wifi interface for ESP8266
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 *   @copyright
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

#ifndef WIFI_DRIVER_H
#define WIFI_DRIVER_H

#include <stdint.h>
#include "network_interface.h"
#include "no_os_uart.h"
#include "no_os_irq.h"

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
