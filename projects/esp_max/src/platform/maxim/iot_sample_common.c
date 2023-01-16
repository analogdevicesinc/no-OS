// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#ifdef _MSC_VER
// warning C4996: 'getenv': This function or variable may be unsafe. Consider using _dupenv_s
// instead.
#pragma warning(disable : 4996)
#endif

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

#define IOT_SAMPLE_PRECONDITION_NOT_NULL(arg)   \
  do                                            \
  {                                             \
    if ((arg) == NULL)                          \
    {                                           \
      IOT_SAMPLE_LOG_ERROR("Pointer is NULL."); \
      exit(1);                                  \
    }                                           \
  } while (0)

static char iot_sample_hub_hostname_buffer[128];
static char iot_sample_provisioning_id_scope_buffer[16];

static char iot_sample_hub_device_id_buffer[64];
static char iot_sample_provisioning_registration_id_buffer[256];

static char iot_sample_hub_sas_key_buffer[128];
static char iot_sample_provisioning_sas_key_buffer[128];

static char iot_sample_x509_cert_pem_file_path_buffer[256];
static char iot_sample_x509_trust_pem_file_path_buffer[256];

//
// MQTT endpoints
//
//#define USE_WEB_SOCKET // Comment to use MQTT without WebSockets.
#ifdef USE_WEB_SOCKET
static az_span const mqtt_url_prefix = AZ_SPAN_LITERAL_FROM_STR("wss://");
// Note: Paho fails to connect to Hub when using AZ_IOT_HUB_CLIENT_WEB_SOCKET_PATH or an X509
// certificate.
static az_span const mqtt_url_suffix
    = AZ_SPAN_LITERAL_FROM_STR(":443" AZ_IOT_HUB_CLIENT_WEB_SOCKET_PATH_NO_X509_CLIENT_CERT);
#else
static az_span const mqtt_url_prefix = AZ_SPAN_LITERAL_FROM_STR("ssl://");
static az_span const mqtt_url_suffix = AZ_SPAN_LITERAL_FROM_STR(":8883");
#endif
static az_span const provisioning_global_endpoint
    = AZ_SPAN_LITERAL_FROM_STR("ssl://global.azure-devices-provisioning.net:8883");

//
// Functions
//
void build_error_message(
    char* out_full_message,
    size_t full_message_buf_size,
    char const* const error_message,
    ...)
{
  char const* const append_message = ": az_result return code 0x%08x.";

  size_t message_len = strlen(error_message) + 1;
  strncpy(out_full_message, error_message, full_message_buf_size);
  out_full_message[full_message_buf_size - 1] = 0;
  if (full_message_buf_size > message_len)
  {
    strncat(out_full_message, append_message, full_message_buf_size - message_len);
    out_full_message[full_message_buf_size - 1] = 0;
  }
}

bool get_az_span(az_span* out_span, char const* const error_message, ...)
{
  va_list args;
  va_start(args, error_message);

  *out_span = va_arg(args, az_span);
  va_end(args);

  if (az_span_size(*out_span) == 0) // There was no span
  {
    return false;
  }

  return true;
}

static void read_configuration_entry(
    char const* env_name,
    char* default_value,
    bool show_value,
    az_span destination,
    az_span* out_env_value)
{
  char* env_value = getenv(env_name);

  if (env_value == NULL && default_value != NULL)
  {
    env_value = default_value;
  }

  if (env_value != NULL)
  {
    (void)printf("%s = %s\n", env_name, show_value ? env_value : "***");
    az_span env_span = az_span_create_from_str(env_value);

    // Check the buffer is large enough to store the environment variable.
    if ((az_span_size(destination) < az_span_size(env_span)) || (az_span_size(env_span) < 0))
    {
      IOT_SAMPLE_LOG_ERROR(
          "Failed to read configuration from environment variables: Buffer is too small.");
      exit(1);
    }

    az_span_copy(destination, env_span);
    *out_env_value = az_span_slice(destination, 0, az_span_size(env_span));
  }
  else
  {
    IOT_SAMPLE_LOG_ERROR(
        "Failed to read configuration from environment variables: Environment variable %s not set.",
        env_name);
    exit(1);
  }
}

void iot_sample_read_environment_variables(
    iot_sample_type type,
    iot_sample_name name,
    iot_sample_environment_variables* out_env_vars)
{
  IOT_SAMPLE_PRECONDITION_NOT_NULL(out_env_vars);

  bool show_value = true;

  if (type == PAHO_IOT_HUB)
  {
    out_env_vars->hub_hostname = AZ_SPAN_FROM_BUFFER(iot_sample_hub_hostname_buffer);
    read_configuration_entry(
        IOT_SAMPLE_ENV_HUB_HOSTNAME,
        NULL,
        show_value,
        out_env_vars->hub_hostname,
        &(out_env_vars->hub_hostname));

    switch (name)
    {
      case PAHO_IOT_HUB_C2D_SAMPLE:
      case PAHO_IOT_HUB_METHODS_SAMPLE:
      case PAHO_IOT_HUB_TELEMETRY_SAMPLE:
      case PAHO_IOT_HUB_TWIN_SAMPLE:
      case PAHO_IOT_PNP_SAMPLE:
      case PAHO_IOT_PNP_COMPONENT_SAMPLE:
        out_env_vars->hub_device_id = AZ_SPAN_FROM_BUFFER(iot_sample_hub_device_id_buffer);
        read_configuration_entry(
            IOT_SAMPLE_ENV_HUB_DEVICE_ID,
            NULL,
            show_value,
            out_env_vars->hub_device_id,
            &(out_env_vars->hub_device_id));

        out_env_vars->x509_cert_pem_file_path
            = AZ_SPAN_FROM_BUFFER(iot_sample_x509_cert_pem_file_path_buffer);
        read_configuration_entry(
            IOT_SAMPLE_ENV_DEVICE_X509_CERT_PEM_FILE_PATH,
            NULL,
            show_value,
            out_env_vars->x509_cert_pem_file_path,
            &(out_env_vars->x509_cert_pem_file_path));
        break;

      case PAHO_IOT_HUB_SAS_TELEMETRY_SAMPLE:
        out_env_vars->hub_device_id = AZ_SPAN_FROM_BUFFER(iot_sample_hub_device_id_buffer);
        read_configuration_entry(
            IOT_SAMPLE_ENV_HUB_SAS_DEVICE_ID,
            NULL,
            show_value,
            out_env_vars->hub_device_id,
            &(out_env_vars->hub_device_id));

        out_env_vars->hub_sas_key = AZ_SPAN_FROM_BUFFER(iot_sample_hub_sas_key_buffer);
        read_configuration_entry(
            IOT_SAMPLE_ENV_HUB_SAS_KEY,
            NULL,
            !show_value,
            out_env_vars->hub_sas_key,
            &(out_env_vars->hub_sas_key));

        char duration_buffer[IOT_SAMPLE_SAS_KEY_DURATION_TIME_DIGITS];
        az_span duration = AZ_SPAN_FROM_BUFFER(duration_buffer);
        read_configuration_entry(
            IOT_SAMPLE_ENV_SAS_KEY_DURATION_MINUTES, "120", show_value, duration, &duration);

        az_result rc = az_span_atou32(duration, &(out_env_vars->sas_key_duration_minutes));
        if (az_result_failed(rc))
        {
          IOT_SAMPLE_LOG_ERROR(
              "Failed to read environment variables: az_result return code 0x%08x.", rc);
          exit(rc);
        }
        break;

      default:
        IOT_SAMPLE_LOG_ERROR("Failed to read environment variables: Hub sample name undefined.");
        exit(1);
    }
  }
  else if (type == PAHO_IOT_PROVISIONING)
  {
    out_env_vars->provisioning_id_scope
        = AZ_SPAN_FROM_BUFFER(iot_sample_provisioning_id_scope_buffer);
    read_configuration_entry(
        IOT_SAMPLE_ENV_PROVISIONING_ID_SCOPE,
        NULL,
        show_value,
        out_env_vars->provisioning_id_scope,
        &(out_env_vars->provisioning_id_scope));

    switch (name)
    {
      case PAHO_IOT_PNP_WITH_PROVISIONING_SAMPLE:
      case PAHO_IOT_PROVISIONING_SAMPLE:
        out_env_vars->provisioning_registration_id
            = AZ_SPAN_FROM_BUFFER(iot_sample_provisioning_registration_id_buffer);
        read_configuration_entry(
            IOT_SAMPLE_ENV_PROVISIONING_REGISTRATION_ID,
            NULL,
            show_value,
            out_env_vars->provisioning_registration_id,
            &(out_env_vars->provisioning_registration_id));

        // out_env_vars->x509_cert_pem_file_path
        //     = AZ_SPAN_FROM_BUFFER(iot_sample_x509_cert_pem_file_path_buffer);
        // read_configuration_entry(
        //     IOT_SAMPLE_ENV_DEVICE_X509_CERT_PEM_FILE_PATH,
        //     NULL,
        //     show_value,
        //     out_env_vars->x509_cert_pem_file_path,
        //     &(out_env_vars->x509_cert_pem_file_path));
        // break;

      case PAHO_IOT_PROVISIONING_SAS_SAMPLE:
        // out_env_vars->provisioning_registration_id
        //     = AZ_SPAN_FROM_BUFFER(iot_sample_provisioning_registration_id_buffer);
        // read_configuration_entry(
        //     IOT_SAMPLE_ENV_PROVISIONING_SAS_REGISTRATION_ID,
        //     NULL,
        //     show_value,
        //     out_env_vars->provisioning_registration_id,
        //     &(out_env_vars->provisioning_registration_id));

        // out_env_vars->provisioning_sas_key
        //     = AZ_SPAN_FROM_BUFFER(iot_sample_provisioning_sas_key_buffer);
        // read_configuration_entry(
        //     IOT_SAMPLE_ENV_PROVISIONING_SAS_KEY,
        //     NULL,
        //     !show_value,
        //     out_env_vars->provisioning_sas_key,
        //     &(out_env_vars->provisioning_sas_key));

        // char duration_buffer[IOT_SAMPLE_SAS_KEY_DURATION_TIME_DIGITS];
        // az_span duration = AZ_SPAN_FROM_BUFFER(duration_buffer);
        // read_configuration_entry(
        //     IOT_SAMPLE_ENV_SAS_KEY_DURATION_MINUTES, "120", show_value, duration, &duration);

        // az_result rc = az_span_atou32(duration, &(out_env_vars->sas_key_duration_minutes));
        // if (az_result_failed(rc))
        // {
        //   IOT_SAMPLE_LOG_ERROR(
        //       "Failed to read environment variables: az_result return code 0x%08x.", rc);
        //   exit(rc);
        // }
        break;

      default:
        IOT_SAMPLE_LOG_ERROR(
            "Failed to read environment variables: Provisioning sample name undefined.");
        exit(1);
    }
  }
  else
  {
    IOT_SAMPLE_LOG_ERROR("Failed to read environment variables: Sample type undefined.");
    exit(1);
  }

  // out_env_vars->x509_trust_pem_file_path
  //     = AZ_SPAN_FROM_BUFFER(iot_sample_x509_trust_pem_file_path_buffer);
  // read_configuration_entry(
  //     IOT_SAMPLE_ENV_DEVICE_X509_TRUST_PEM_FILE_PATH,
  //     "",
  //     show_value,
  //     out_env_vars->x509_trust_pem_file_path,
  //     &(out_env_vars->x509_trust_pem_file_path));

  IOT_SAMPLE_LOG(" "); // Formatting
}

void iot_sample_create_mqtt_endpoint(
    iot_sample_type type,
    iot_sample_environment_variables const* env_vars,
    char* out_endpoint,
    size_t endpoint_size)
{
  IOT_SAMPLE_PRECONDITION_NOT_NULL(env_vars);
  IOT_SAMPLE_PRECONDITION_NOT_NULL(out_endpoint);

  if (type == PAHO_IOT_HUB)
  {
    int32_t const required_size = az_span_size(mqtt_url_prefix)
        + az_span_size(env_vars->hub_hostname) + az_span_size(mqtt_url_suffix)
        + (int32_t)sizeof('\0');

    if ((size_t)required_size > endpoint_size)
    {
      IOT_SAMPLE_LOG_ERROR("Failed to create MQTT endpoint: Buffer is too small.");
      exit(1);
    }

    az_span hub_mqtt_endpoint = az_span_create((uint8_t*)out_endpoint, (int32_t)endpoint_size);
    az_span remainder = az_span_copy(hub_mqtt_endpoint, mqtt_url_prefix);
    remainder = az_span_copy(remainder, env_vars->hub_hostname);
    remainder = az_span_copy(remainder, mqtt_url_suffix);
    az_span_copy_u8(remainder, '\0');
  }
  else if (type == PAHO_IOT_PROVISIONING)
  {
    int32_t const required_size
        = az_span_size(provisioning_global_endpoint) + (int32_t)sizeof('\0');

    if ((size_t)required_size > endpoint_size)
    {
      IOT_SAMPLE_LOG_ERROR("Failed to create MQTT endpoint: Buffer is too small.");
      exit(1);
    }

    az_span provisioning_mqtt_endpoint
        = az_span_create((uint8_t*)out_endpoint, (int32_t)endpoint_size);
    az_span remainder = az_span_copy(provisioning_mqtt_endpoint, provisioning_global_endpoint);
    az_span_copy_u8(remainder, '\0');
  }
  else
  {
    IOT_SAMPLE_LOG_ERROR("Failed to create MQTT endpoint: Sample type undefined.");
    exit(1);
  }

  IOT_SAMPLE_LOG_SUCCESS("MQTT endpoint created at \"%s\".", out_endpoint);
}

void iot_sample_sleep_for_seconds(uint32_t seconds)
{
#ifdef _WIN32
  Sleep((DWORD)seconds * 1000);
#else
  sleep(seconds);
#endif
}

uint32_t iot_sample_get_epoch_expiration_time_from_minutes(uint32_t minutes)
{
  return (uint32_t)(time(NULL) + minutes * 60);
}




