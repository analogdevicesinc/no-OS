/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by eval-adxl355-pmdz
 *           project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_stdio.h"


/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define UART_DEVICE_ID		1
#define INTC_DEVICE_ID		0
#define UART_IRQ_ID		UART1_IRQn
#define UART_BAUDRATE		57600

#define WIFI_SSID	        "moto g 5G plus"
#define WIFI_PWD	        "blabla1234"
#define SERVER_PORT		8883
#define SERVER_ADDR		"iot-hub-ylr4pd6jbvxtc.azure-devices.net"
#define DPS_SERVER_ADDR		"dps-ylr4pd6jbvxtc.azure-devices-provisioning.net"
#define BUFF_LEN		200
#define TIMER_ID		1
#define MQTT_CONFIG_CMD_TIMEOUT	20000
#define MQTT_CONFIG_VERSION	MQTT_VERSION_3_1_1
#define MQTT_CONFIG_KEEP_ALIVE	7200
#define MQTT_CONFIG_CLI_PASS	NULL
#define SCAN_SENSOR_TIME	500

#define AZ_IOT_PROVISIONING_CUSTOM_PAYLOAD	"{\"modelId\":\"dtmi:com:analog:ADX12345;1\"}"
#define AZ_IOT_PROVISIONING_ID_SCOPE		"0ne0090DBBD"
#define AZ_IOT_PROVISIONING_REGISTRATION_ID 	"EnergyMonitoringDevice1"

#define CA_CERT                                                            \
    "-----BEGIN CERTIFICATE-----\r\n"                                      \
    "MIIDHzCCAgegAwIBAgIULv9GOV/gHojd0KXRx9GZ0PUqyBwwDQYJKoZIhvcNAQEL\r\n" \
    "BQAwHzEdMBsGA1UEAwwUQW5hbG9nIERldmljZXMsIEluYy4wHhcNMjMwMTEyMDkz\r\n" \
    "NjExWhcNMzIxMDExMDkzNjExWjAfMR0wGwYDVQQDDBRBbmFsb2cgRGV2aWNlcywg\r\n" \
    "SW5jLjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANrAoQnTHwOW2TXr\r\n" \
    "v4hWXSeU/7upsbIdIPB5TgWFN5lX+SY0tx0ljcGKymrnf6a/tAzdljGqRXH2H4Ln\r\n" \
    "sW2sj7qbntppe1023SHA5S8d4WGqbmoh7Lf+FrYY9oAScT/nGNMXZ5hWgpJkloLN\r\n" \
    "AFYCS7eJ08aR0geUt2Qc7YM77Rogmp0x95Z2yIV2QvKB41U6p151x63D5/tWvICJ\r\n" \
    "q9PPKxeGuS2wa0h2zP82BKLyHDZdxSUkV2O/MbjxY/48amvZHrYRk+t8HfClIOt8\r\n" \
    "ECobTnuLO8uEcz/H8mwaQGWDd5B3IwoUulI8gsCcQI1/iXcxZOEFgv8zwW/PeV4D\r\n" \
    "h9vJkQMCAwEAAaNTMFEwHQYDVR0OBBYEFBsV5sXaaKZFFpNHB3rtmiBgMEsZMB8G\r\n" \
    "A1UdIwQYMBaAFBsV5sXaaKZFFpNHB3rtmiBgMEsZMA8GA1UdEwEB/wQFMAMBAf8w\r\n" \
    "DQYJKoZIhvcNAQELBQADggEBANexrLjMSe9r3inDDTme9LgWQpLeJ374ZgIMesgn\r\n" \
    "hjrMYWloJa2Uqvj/7qc0aE9nkaGmnQwlMQ8ozBFizkyNoUFl5JdT+JzpQZv+6B3P\r\n" \
    "DfQ+AKjfvcVNwsSgMIs6zZs4P47i745p5uvjdtVGW3rExQmI8/2398cBjeByyNT9\r\n" \
    "N+TCK8ZwHS4fHYDJTSLUuWvX54Xl7S3Nmgq5frca4Bi7+i2wqjQWW6/QTbK7ZSdG\r\n" \
    "m7qpW8nDxClkgO+5IQmqaENkohMwAlF99FBK0iJlSSSIsuo0SFXtqINFa7emFwGa\r\n" \
    "oAMxi2Zfx52/MhR5WfVvQx4YIX2fIQP+OPLBTMMYNdesZ+U=\r\n"                 \
    "-----END CERTIFICATE-----\r\n"

#define DEVICE_CERT                                                        \
    "-----BEGIN CERTIFICATE-----\n"                                        \
    "MIICyDCCAbACFBhDPi10huvZNJYgKltRGCBE2l9lMA0GCSqGSIb3DQEBCwUAMB8x\r\n" \
    "HTAbBgNVBAMMFEFuYWxvZyBEZXZpY2VzLCBJbmMuMB4XDTIzMDExMjEwMzUxN1oX\r\n" \
    "DTIzMDIxMTEwMzUxN1owIjEgMB4GA1UEAwwXRW5lcmd5TW9uaXRvcmluZ0Rldmlj\r\n" \
    "ZTEwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQD5e81cT1BLOsZNHz7s\r\n" \
    "wn4/WXm5gFO/kK8JjlvC2G2QLlhzUJDQ+FraTcbWGz0sRlbX5kareyxLpYNcKKxF\r\n" \
    "LCcglI8fRlPLM2v88vsE8xiVav6uIyPh7koolmn9RQ2bEZlwcXJRoo53W9XbEWMK\r\n" \
    "gADsL4gzj+IK3Wd7JAdmqsJEiSg6u2ePjuBF/1XB+4LvkvbqrP+NmmDzqouu06hj\r\n" \
    "PLsf3uWzOtUO1DsWjMkUW00V6ZVV0Ljr73dxdezuQGyH5GIKy2bjO4C+3NNFl517\r\n" \
    "+3VmEiIIR1n8sfRCtRaeO/o4VJW5rpaMHbhI9xTYnk6vTc4BMuHP3eFxtj/aU09K\r\n" \
    "xhyXAgMBAAEwDQYJKoZIhvcNAQELBQADggEBAF7UDpmL7PswJcYf7mGTjesOaTDr\r\n" \
    "cGgFtotuXlidLz2S2exX5nUKuoNmWzEl+vUDmI/d8VqWy61I6gDk35pqXgrcX9Rm\r\n" \
    "naqYluhHcjQwQfsHwkDsGDz9uwLAe0JK6wlbCZDcgSXdvU6CxUDtrv9WBXl7kcgU\r\n" \
    "Lph5a/t9L/LFp24Sh5wsK6iUTtCW4MFGwqWhFdeglTDJrM7YdPLsf72feBkH9vfR\r\n" \
    "fxm/2d6dxkqcJg952k49qq5iCB41fAemS6TwizwNJfcGPT31dE4WS0wCI1wx6jHy\r\n" \
    "Zfm7ChOD+OGOwx627UdIXtfBOzv45SMQIAGvj55GRBvQZ9Hoo3j0GbXNA3M=\r\n"     \
    "-----END CERTIFICATE-----\r\n";

#define DEVICE_PRIVATE_KEY                                                 \
    "-----BEGIN PRIVATE KEY-----\n"                                        \
    "MIIEwAIBADANBgkqhkiG9w0BAQEFAASCBKowggSmAgEAAoIBAQD5e81cT1BLOsZN\r\n" \
    "Hz7swn4/WXm5gFO/kK8JjlvC2G2QLlhzUJDQ+FraTcbWGz0sRlbX5kareyxLpYNc\r\n" \
    "KKxFLCcglI8fRlPLM2v88vsE8xiVav6uIyPh7koolmn9RQ2bEZlwcXJRoo53W9Xb\r\n" \
    "EWMKgADsL4gzj+IK3Wd7JAdmqsJEiSg6u2ePjuBF/1XB+4LvkvbqrP+NmmDzqouu\r\n" \
    "06hjPLsf3uWzOtUO1DsWjMkUW00V6ZVV0Ljr73dxdezuQGyH5GIKy2bjO4C+3NNF\r\n" \
    "l517+3VmEiIIR1n8sfRCtRaeO/o4VJW5rpaMHbhI9xTYnk6vTc4BMuHP3eFxtj/a\r\n" \
    "U09KxhyXAgMBAAECggEBAOmntF7m/Xlns39ZazKMuXaVUzaT5Fu+ouCdTn1++6YU\r\n" \
    "ccyWQPsmDtj0DOxLmJBV1FHiHn/OHV8/xXZTtXwXbRHLcPvGfwYJw/KkkspA7t5J\r\n" \
    "1fq7eIjKBvRaYyvMN4DrC7zO01V3nFxXS0Qf1ZVo6vRJVBOOhxLL53Xh+XiDJuzN\r\n" \
    "ethkGaDEHRbnMk3s3I/5OB5SmshIubteQbLfGj6m27BGdUmWEdbcoNz0xWJX3pir\r\n" \
    "YyraXKM6RctfBTL/KNXf3iz52OHbyhwKtpmVs4yvoL3jvxMzd5qQsMluE6rOopfd\r\n" \
    "eBsRFnId5kMCrd/ngzPaki2keXagkQJXmxvIbf46HYkCgYEA/cd4CI+j5AQWAS/6\r\n" \
    "EyDmTW8PL+D5jgteBsVoNzvN6TDqIPBhMEx66XwRhbkWgjd4VOfb6QCm6UPWXwHu\r\n" \
    "GsBzz2xLhtwD2S1+BfYklS63bT+jHRHQuUEkID3u/aWJeiQ207NOvxEk3/0ZsByc\r\n" \
    "UxEjI7a0NLJJPkwMaxeK1eJfT00CgYEA+6q1ye1fFJXxlWFIE1/j7/eesNo/E6V0\r\n" \
    "Eykr24kiNZCrlMJV7o/YyLG6GEvvjt6eHRKUfVBTZj6FHP6IsvrGeGXeQcSj4gsU\r\n" \
    "U0vaVIv2+oJqJNPzrYY8w05DlqFK5VGUx8YyhhT/oAQOZ7bPLpeIJ1oNqvJBD1vu\r\n" \
    "b1EeNdaG8XMCgYEAmhBANKJPEpvQaAQiQv6UEao8hAqGNc9804uASxfJV6nUWQ7V\r\n" \
    "Uab8b0ZctNu6aG8YZCMmA/vbWH1QZPkc/aycIe5So5pp06ykdyYI63Ai9vXNJlas\r\n" \
    "aeq77b7rmZynKzlU94xBYAkeY8SZlaqo1FzxAjtmhx89C87LMzBlnpcA2XkCgYEA\r\n" \
    "yjbhuO2NL2pH4kJ3enHbIa7yiKHkevG/scGsJssqfKXQ6Ee6amSUyLXdlLY3N7Hy\r\n" \
    "k+OhAovvZ0EnDiQLTNypaoZXrrrE/9TtMPzK4pJQEfuGNC9GJhCtdDTHpBnXpykF\r\n" \
    "BfxgDlKDD10TAgzmrkY9tXjrcIcU9bVlgXZJcvL8HLcCgYEA7eIu1N8T0bH0Bd/w\r\n" \
    "QRzNo1wzYLR21AZ0aNqgATQmTHdwXDtEoL14wJisr/L0w+rV+AEA+XczvR81NzIe\r\n" \
    "Rb7SPxgKJ9ZviuxC1FXTsxlAMlEyhJDQOc1EYreidKZvuk8Mq3NWXHyMQdRiz9Uw\r\n" \
    "gtV47HSZkKlslkvgtRxCe6nuZ6U=\r\n"                                     \
    "-----END PRIVATE KEY-----\r\n";

#endif /* __PARAMETERS_H__ */
