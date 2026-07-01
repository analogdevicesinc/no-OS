// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#ifndef IOT_SAMPLE_COMMON_H
#define IOT_SAMPLE_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <azure/az_core.h>

//
// Logging
//
#define IOT_SAMPLE_LOG_ERROR(...)                                                  \
  do                                                                               \
  {                                                                                \
    (void)fprintf(stderr, "ERROR:\t\t%s:%s():%d: ", __FILE__, __func__, __LINE__); \
    (void)fprintf(stderr, __VA_ARGS__);                                            \
    (void)fprintf(stderr, "\n");                                                   \
    fflush(stdout);                                                                \
    fflush(stderr);                                                                \
  } while (0)

#define IOT_SAMPLE_LOG_SUCCESS(...) \
  do                                \
  {                                 \
    (void)printf("SUCCESS:\t");     \
    (void)printf(__VA_ARGS__);      \
    (void)printf("\n");             \
  } while (0)

#define IOT_SAMPLE_LOG(...)    \
  do                           \
  {                            \
    (void)printf("\t\t");      \
    (void)printf(__VA_ARGS__); \
    (void)printf("\n");        \
  } while (0)

typedef enum {
	PAHO_IOT_HUB,
	PAHO_IOT_PROVISIONING
} iot_sample_type;

/*
 * @brief Builds an MQTT endpoint c-string for an Azure IoT Hub or provisioning service.
 *
 * @param[in] type The enumerated type of the sample.
 * @param[out] endpoint A buffer with sufficient capacity to hold the built endpoint. If
 * successful, contains a null-terminated string of the endpoint.
 * @param[in] endpoint_size The size of \p out_endpoint in bytes.
 */
void iot_sample_create_mqtt_endpoint(
	iot_sample_type type,
	char* endpoint,
	size_t endpoint_size);

#endif // IOT_SAMPLE_COMMON_H
