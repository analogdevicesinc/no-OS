// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// Required for sleep(unsigned int)
#include <unistd.h>

#include <azure/az_core.h>
#include <azure/az_iot.h>

#include "iot_sample_common.h"

//
// MQTT endpoints
//
static az_span const mqtt_url_prefix = AZ_SPAN_LITERAL_FROM_STR("ssl://");
static az_span const mqtt_url_suffix = AZ_SPAN_LITERAL_FROM_STR(":8883");
static az_span const provisioning_global_endpoint
	= AZ_SPAN_LITERAL_FROM_STR("ssl://global.azure-devices-provisioning.net:8883");

//
// Functions
//
void iot_sample_create_mqtt_endpoint(
	iot_sample_type type,
	char* out_endpoint,
	size_t endpoint_size)
{
	if (type == PAHO_IOT_HUB) {
		int32_t const required_size = az_span_size(mqtt_url_prefix)
					      + az_span_size(mqtt_url_suffix)
					      + (int32_t)sizeof('\0');

		if ((size_t)required_size > endpoint_size) {
			IOT_SAMPLE_LOG_ERROR("Failed to create MQTT endpoint: Buffer is too small.");
			exit(1);
		}

		az_span hub_mqtt_endpoint = az_span_create((uint8_t*)out_endpoint,
					    (int32_t)endpoint_size);
		az_span remainder = az_span_copy(hub_mqtt_endpoint, mqtt_url_prefix);
		remainder = az_span_copy(remainder, mqtt_url_suffix);
		az_span_copy_u8(remainder, '\0');
	} else if (type == PAHO_IOT_PROVISIONING) {
		int32_t const required_size
			= az_span_size(provisioning_global_endpoint) + (int32_t)sizeof('\0');

		if ((size_t)required_size > endpoint_size) {
			IOT_SAMPLE_LOG_ERROR("Failed to create MQTT endpoint: Buffer is too small.");
			exit(1);
		}

		az_span provisioning_mqtt_endpoint
			= az_span_create((uint8_t*)out_endpoint, (int32_t)endpoint_size);
		az_span remainder = az_span_copy(provisioning_mqtt_endpoint,
						 provisioning_global_endpoint);
		az_span_copy_u8(remainder, '\0');
	} else {
		IOT_SAMPLE_LOG_ERROR("Failed to create MQTT endpoint: Sample type undefined.");
		exit(1);
	}

	IOT_SAMPLE_LOG_SUCCESS("MQTT endpoint created at \"%s\".", out_endpoint);
}
