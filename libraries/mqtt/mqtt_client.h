/***************************************************************************//**
 *   @file   mqtt_client.h
 *   @brief  MQTT client interface
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
********************************************************************************
 * @section mqqt_details MQTT description
 *   This file wrapps the MQTT eclipse paho client implementation from
 *   https://www.eclipse.org/paho/clients/c/embedded/
 *  @subsection mqtt_example Sample code
 *   @code{.c}
 * #define BUFF_SIZE 100
 *
 * void message_handler(struct mqtt_message_data *msg)
 * {
 * 	msg->message.payload[msg->message.len] = 0;
 * 	printf("Topic:%s -- Payload: %s\n", msg->topic, msg->message.payload);
 * }
 *
 * int32_t example_mqtt_main()
 * {
 * 	uint8_t send_buff[BUFF_SIZE];
 * 	uint8_t read_buff[BUFF_SIZE];
 *
 * 	struct tcp_socket_desc	*sock;
 * 	struct mqtt_desc	*mqtt;
 * 	struct mqtt_init_param	mqtt_init_param = {
 * 		.timer_id = 0,
 * 		.extra_timer_init_param = NULL,
 * 		.command_timeout_ms = 20000,
 * 		.send_buff = send_buff,
 * 		.read_buff = read_buff,
 * 		.send_buff_size = BUFF_SIZE,
 * 		.read_buff_size = BUFF_SIZE,
 * 		.message_handler = message_handler
 * 	};
 *
 * 	init_socket(&sock); //Initialize socket depending on network stack
 *
 * 	struct socket_address server = {.addr = "mqtt.server.com",
 * 					.port = 1883};
 * 	socket_connect(sock, &server);
 *
 * 	mqtt_init_param.sock = sock;
 * 	ret = mqtt_init(&mqtt, &mqtt_init_param);
 * 	//Connect
 * 	struct mqtt_connect_config conn_config = {
 * 		.version = MQTT_VERSION_3_1,
 * 		.keep_alive_ms = 7200,
 * 		.client_name = (int8_t *)"noos-client",
 * 		.username = NULL,
 * 		.password = NULL
 * 	};
 * 	mqtt_connect(mqtt, &conn_config, NULL);
 * 	//Publish
 * 	struct mqtt_message msg = {
 * 		.qos = MQTT_QOS0,
 * 		.retained = true,
 * 		.payload = "Hello World\n",
 * 		.len = strlen("Hello World\n")
 * 	};
 * 	mqtt_publish(mqtt, "my_publish", &msg);
 * 	//Subscribe
 * 	mqtt_subscribe(mqtt, "my_subscribe", MQTT_QOS0, NULL);
 * 	while (true)
 * 		mqtt_yield(mqtt, 1000); //Read messages from topic my_subscirbe
 * 	//Cleanup
 * 	mqtt_remove(mqtt);
 * 	clean_socket(sock);
 * }
 *    @endcode
*******************************************************************************/

#ifndef MQTT_NOOS_CLIENT
#define MQTT_NOOS_CLIENT

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdbool.h>
#include "tcp_socket.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum mqtt_qos
 * @brief Type of Quality of Service. Number of guaranteed message deliveries
 */
enum mqtt_qos {
	/** QOS0 : At most once */
	MQTT_QOS0,
	/** QOS1 : At least once */
	MQTT_QOS1,
	/** QOS2 : Exactly once */
	MQTT_QOS2,
	/** SUBFAIL from MQTT_PAHO */
	MQTT_SUBFAIL = 0x80
};

/**
 * @enum mqtt_version
 * @brief Version of protocol to be used
 */
enum mqtt_version {
	/** Version 3.1 */
	MQTT_VERSION_3_1 = 3,
	/** Version 3.1.1 */
	MQTT_VERSION_3_1_1 = 4
};

/**
 * @struct mqtt_connect_config
 * @brief Parameter to configure a connect
 */
struct mqtt_connect_config {
	/** Mqtt version */
	enum mqtt_version	version;
	/**
	 * Maximum length of time that the broker and client may not
	 * communicate with each other. In milliseconds
	 */
	uint16_t		keep_alive_ms;
	/** Name the client will have */
	char			*client_name;
	/** Username  */
	char			*username;
	/** Password */
	char			*password;
};

/**
 * @struct mqtt_connack_data
 * @brief Result form a connect
 */
struct mqtt_connack_data {
	/**
	 * Only from version 3.1.1.  Tells the client if a previously
	 * established session is still available on the broker.
	 */
	uint8_t session_present;
	/** Result code from broker */
	uint8_t rc;
};

/**
 * @struct mqtt_message
 * @brief Message structure
 */
struct mqtt_message {
	/** Quality of services */
	enum mqtt_qos	qos;
	/** Payload */
	char		*payload;
	/** Length */
	size_t		len;
	/** If set, a retained message will be sent. See MQTT documentation */
	bool		retained;
};

/**
 * @struct mqtt_message_data
 * @brief Messages received from a topic
 */
struct mqtt_message_data {
	/** Message */
	struct mqtt_message	message;
	/** Topic */
	uint8_t			*topic;
};

/**
 * @struct mqtt_init_param
 * @brief Parameter used to initialize an MQTT client
 */
struct mqtt_init_param {
	/**
	 * Reference to a socket. The connection must be established before
	 * calling any MQTT client functions.
	 */
	struct tcp_socket_desc	*sock;
	/** Parameter to initialize a \ref timer_desc */
	struct no_os_timer_init_param	*timer_init_param;
	/** Timeout for a MQTT command to be executed */
	uint32_t		command_timeout_ms;
	/** Buffer used by the client to read messages */
	uint8_t			*send_buff;
	/** Buffer used by the client to write messages */
	uint8_t			*read_buff;
	/** Size of the send buffer */
	uint32_t		send_buff_size;
	/** Size of the read buffer */
	uint32_t		read_buff_size;
	/**
	 * Callback to be called when a message is received from the broker
	 * @param Message received from the broker.
	 */
	void			(*message_handler)(struct mqtt_message_data *);
};

/**
 * @struct mqtt_desc
 * @brief Reference to MQTT client
 */
struct mqtt_desc;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Init MQTT client */
int32_t mqtt_init(struct mqtt_desc **desc,
		  struct mqtt_init_param *param);
/* Uninit MQTT client */
int32_t mqtt_remove(struct mqtt_desc *desc);

/* Send connect to MQTT broker */
int32_t mqtt_connect(struct mqtt_desc *desc,
		     const struct mqtt_connect_config *conf,
		     struct mqtt_connack_data *result_optional);
/* Send disconnect to MQTT broker */
int32_t mqtt_disconnect(struct mqtt_desc *desc);

/* Send publish to MQTT broker */
int32_t mqtt_publish(struct mqtt_desc *desc, const char *topic,
		     const struct mqtt_message* msg);
/* Send subscribe to MQTT broker */
int32_t mqtt_subscribe(struct mqtt_desc *desc, const char *topic,
		       enum mqtt_qos qos, enum mqtt_qos *granted_qos_optional);
/* Send unsubscribe to MQTT broker */
int32_t mqtt_unsubscribe(struct mqtt_desc *desc, const char *topic);
/* Allow messages to be received */
int32_t mqtt_yield(struct mqtt_desc *desc, uint32_t timeout_ms);

#endif
