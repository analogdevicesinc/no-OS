/***************************************************************************//**
 *   @file   mqtt_client.c
 *   @brief  MQTT client implementation
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

#include <stdlib.h>
#include <string.h>
#include "mqtt_client.h"
#include "MQTTClient.h"
#include "error.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct mqtt_desc {
	MQTTClient		mqtt_client[1];
	Network			network;
};

/******************************************************************************/
/**************************** Global Variables ********************************/
/******************************************************************************/

/* TODO: After initial commit, modify MQTT to support context to enable handler
 * for multiple clients */
static void (*app_handler)(struct mqtt_message_data *);

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/* Callback used in MQTT subscribe */
static void mqtt_default_message_handler(MessageData *msg)
{
	struct mqtt_message_data	data;
	MQTTLenString			*topic;

	data.message.len = (uint32_t)msg->message->payloadlen;
	data.message.payload = (uint8_t *)msg->message->payload;
	data.message.qos = (enum mqtt_qos)msg->message->qos;
	data.message.retained = (bool)msg->message->retained;

	topic = &msg->topicName->lenstring;
	data.topic = (uint8_t *)malloc(topic->len + 1);
	if (!data.topic)
		return ;
	memcpy(data.topic, topic->data, topic->len);
	data.topic[topic->len] = '\0';

	app_handler(&data);

	free(data.topic);
}

/**
 * @brief Initialize the MQTT client
 * @param desc - Address where to store the MQTT client reference
 * @param param - Parameter used to configure the MQTT client
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t mqtt_init(struct mqtt_desc **desc,
		  struct mqtt_init_param *param)
{
	struct mqtt_desc	*ldesc;
	int32_t			ret;

	if (!desc || !param)
		return FAILURE;

	ldesc = (struct mqtt_desc *)calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return FAILURE;

	ret = mqtt_timer_init(param->timer_id, param->extra_timer_init_param);
	if (IS_ERR_VALUE(ret)) {
		free(ldesc);
		return FAILURE;
	}

	ldesc->network.sock = param->sock;
	ldesc->network.mqttread = mqtt_noos_read;
	ldesc->network.mqttwrite = mqtt_noos_write;

	app_handler = param->message_handler;

	MQTTClientInit(ldesc->mqtt_client, &ldesc->network,
		       (unsigned int)param->command_timeout_ms,
		       (unsigned char *)param->send_buff,
		       (size_t)param->send_buff_size,
		       (unsigned char *)param->read_buff,
		       (size_t)param->read_buff_size);

	*desc = ldesc;

	return SUCCESS;
}

/**
 * @brief Remove MQTT client resources
 * @param desc - Reference to MQTT client
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t mqtt_remove(struct mqtt_desc *desc)
{
	if (!desc)
		return FAILURE;

	free(desc);
	mqtt_timer_remove();

	return SUCCESS;
}

/**
 * @brief Send connect to MQTT broker
 * @param desc - Reference to MQTT client
 * @param conf - Connect options
 * @param result_optional - Address to store the result for the connect command.
 * It can be NULL if result is not needed.
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t mqtt_connect(struct mqtt_desc *desc,
		     const struct mqtt_connect_config *conf,
		     struct mqtt_connack_data *result_optional)
{
	if (!desc || !conf)
		return FAILURE;

	int32_t			ret;
	MQTTConnackData 	res;
	MQTTPacket_connectData 	data = MQTTPacket_connectData_initializer;

	data.MQTTVersion = (unsigned char)conf->version;
	data.clientID.cstring = (char *)conf->client_name;
	data.username.cstring = (char *)conf->username;
	data.password.cstring = (char *)conf->password;
	data.keepAliveInterval = (unsigned short)conf->keep_alive_ms;

	ret = MQTTConnectWithResults(desc->mqtt_client, &data, &res);
	if (result_optional) {
		result_optional->rc = res.rc;
		result_optional->session_present = res.sessionPresent;
	}

	return ret;
}

/**
 * @brief Send disconnect to MQTT broker
 * @param desc - Reference to MQTT client
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t mqtt_disconnect(struct mqtt_desc *desc)
{
	if (!desc)
		return FAILURE;

	return MQTTDisconnect(desc->mqtt_client);
}

/**
 * @brief Send publish to MQTT broker
 * @param desc - Reference to MQTT client
 * @param topic - Topic pattern which can include wildcards
 * @param msg - Message to send
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t mqtt_publish(struct mqtt_desc *desc, const int8_t* topic,
		     const struct mqtt_message* msg)
{
	if (!desc || !msg)
		return FAILURE;

	MQTTMessage message = { 0 };

	message.payload = (void *)msg->payload;
	message.payloadlen = (size_t)msg->len;
	message.qos = (enum QoS)msg->qos;
	message.retained = (unsigned char)msg->retained;

	return MQTTPublish(desc->mqtt_client, (char *)topic, &message);
}

/**
 * @brief Send subscribe to MQTT broker
 * @param desc - Reference to MQTT client
 * @param topic - Topic pattern which can include wildcards
 * @param qos - MQTT QoS to subscribe at
 * @param granted_qos_optional - Address to store the result for the subscribe
 * command. It can be NULL if result is not needed.
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t mqtt_subscribe(struct mqtt_desc *desc, const int8_t *topic,
		       enum mqtt_qos qos, enum mqtt_qos *granted_qos_optional)
{
	MQTTSubackData	result;
	int32_t		ret;

	if (!desc)
		return FAILURE;

	ret = MQTTSubscribeWithResults(desc->mqtt_client, (char *)topic,
				       (enum QoS)qos,
				       mqtt_default_message_handler,
				       &result);
	if (granted_qos_optional)
		*granted_qos_optional = (enum mqtt_qos)result.grantedQoS;

	return ret;
}

/**
 * @brief Send unsubscribe to MQTT broker
 * @param desc - Reference to MQTT client
 * @param topic - Topic pattern which can include wildcards
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t mqtt_unsubscribe(struct mqtt_desc *desc, const int8_t* topic)
{
	if (!desc)
		return FAILURE;

	return MQTTUnsubscribe(desc->mqtt_client, (char *)topic);
}

/**
 * @brief Allow messages to be received
 *
 * A call to this API must be made within the
 * \ref mqtt_connect_config.keep_alive_ms interval to keep the MQTT connection
 * alive. \n
 * Yield can be called if no other MQTT operation is needed.
 * @param desc - Reference to MQTT client
 * @param timeout_ms - Time for yield to be executed
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t mqtt_yield(struct mqtt_desc *desc, uint32_t timeout_ms)
{
	return MQTTYield(desc->mqtt_client, timeout_ms);
}
