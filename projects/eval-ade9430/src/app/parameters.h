/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by eval-ade9430
 *           project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef DISABLE_SECURE_SOCKET
#include "iot_sample_common.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define UART_DEVICE_ID		1
#define INTC_DEVICE_ID		0
#define UART_IRQ_ID		UART1_IRQn
#define UART_BAUDRATE		57600

#ifdef RTC_SET_DEFAULT
/* RTC BCD Format (example 26.01.2023-13:15:00) */
#define RTC_SEC_DEFAULT		0
#define RTC_MIN_DEFAULT		15
#define RTC_HR_DEFAULT		13
#define RTC_DAY_DEFAULT		26
#define RTC_MON_DEFAULT		1
#define RTC_YEAR_DEFAULT	23
#endif

/* 1 sec acuumulation */
#define ADE9430_SAMPLES_NR  7999

/* Wifi Parameters */
#define WIFI_SSID	            "WifiSSID"
#define WIFI_PWD	            "WifiPassword"

#ifndef DISABLE_SECURE_SOCKET
/* Server Port is common for both IoT and DPS */
#define SERVER_PORT				8883
/* Azure IoT Hub Server Address */
#define SERVER_ADDR				"iot-hub-mopdcwnti4mly.azure-devices.net"
/* Azure Device Provisioning Service Server Address */
#define DPS_SERVER_ADDR				"dps-mopdcwnti4mly.azure-devices-provisioning.net"
#define AZ_IOT_PROVISIONING_CUSTOM_PAYLOAD	"{\"modelId\":\"dtmi:com:analog:ADX12345;1\"}"
#define AZ_IOT_PROVISIONING_ID_SCOPE		"0ne009919BB"
#define AZ_IOT_PROVISIONING_REGISTRATION_ID	"EnergyMonitoringDevice1"
#define SAMPLE_TYPE				PAHO_IOT_PROVISIONING
#define REGISTER_TOPIC_BUFFER_LENGTH		128
#define PROVISIONING_ENDPOINT_BUFFER_LENGTH	256
#define MQTT_PAYLOAD_BUFFER_LENGTH		256
#define CLIENT_ID_BUFFER_LENGTH			128
#else
/* MQTT Broker Parameters, non-encrypted communication */
#define SERVER_PORT             1883
#define SERVER_ADDR             "192.168.128.123"
#define MQTT_PUBLISH_TOPIC	"ade9430"
#define MQTT_SUBSCRIBE_TOPIC	"maxim_messages"
#define MQTT_CONFIG_CLIENT_NAME	"maxim-client"
#define MQTT_CONFIG_CLI_USER	NULL
#endif

#define BUFF_LEN		200
#define TIMER_ID		1
#define MQTT_CONFIG_CMD_TIMEOUT	20000

/* For Azure encrypted communication MQTT version 3.1.1. is required.*/
#ifndef DISABLE_SECURE_SOCKET
#define MQTT_CONFIG_VERSION     MQTT_VERSION_3_1_1
#else
#define MQTT_CONFIG_VERSION     MQTT_VERSION_3_1
#endif

#define MQTT_CONFIG_CLI_PASS	NULL
#define MQTT_CONFIG_KEEP_ALIVE	7200
#define SCAN_SENSOR_TIME	500

#ifndef DISABLE_SECURE_SOCKET
/* Populate here your CA certificate content */
#define CA_CERT                                                            \
    "-----BEGIN CERTIFICATE-----\r\n"                                      \
	"MIIDHzCCAgegAwIBAgIUCFbk7jeOFFqrrLVQjCoLQDCvNp0wDQYJKoZIhvcNAQEL\r\n"	\
	"BQAwHzEdMBsGA1UEAwwUQW5hbG9nIERldmljZXMsIEluYy4wHhcNMjMwMjI3MTAw\r\n"	\
	"NjQwWhcNMzIxMTI2MTAwNjQwWjAfMR0wGwYDVQQDDBRBbmFsb2cgRGV2aWNlcywg\r\n"	\
	"SW5jLjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAPLUgsHWu4SCB11U\r\n"	\
	"GxlmqJ3pIzP4YWdH/cww2K+OjCS4KWy5Fb//KlZMxXjZeJ6iAd5pcOvcsE4w5CQM\r\n"	\
	"kRVxdqzLmLcVsh9hMW+dB+Aeeura8pav7TtlO3BOZxsQN/PWi8AX5g8rjO5xC9hs\r\n"	\
	"9kxR5MAXlUOlq9WJ2T8xdtxZSQHT5pnrHEVnjd72rZlC2rAS8vYTrMxQLWm2GK+2\r\n"	\
	"7jYhQ9jSKuvy/g5XSYI0OHlWFrzo1fDuR/Ma5aJZBzBRKOKAXl2uFweiEYVH1sZe\r\n"	\
	"iztZlhxtxHAfJHVhnEL/wf7GnaAClCoMd4amxBuiESxrP+DU/lwwNerBbB+wMxqr\r\n"	\
	"n+/Hb2MCAwEAAaNTMFEwHQYDVR0OBBYEFIfN1T+xxFvPuxcT8ROronOnTQDhMB8G\r\n"	\
	"A1UdIwQYMBaAFIfN1T+xxFvPuxcT8ROronOnTQDhMA8GA1UdEwEB/wQFMAMBAf8w\r\n"	\
	"DQYJKoZIhvcNAQELBQADggEBADpA6t61wwEqQ4yBXWk9sX5dW3NQpj/FigpWIUnf\r\n"	\
	"geQedXfrn/zZFOC7iA05uHdjRpP+Fp4ebJNxHOMbL7TCMPOG+SBYgbMv9ZgRDAYj\r\n"	\
	"Ca3Osm53EqeMi+26ka6xkBEHYZ+vVt1bwZOjwxzX56J6lNiKLvgQgn9EgeKpQker\r\n"	\
	"lV12T04/NgeIm38mfRqxueYG22YrLuo+dzijwzY2wwBGntwFviXWLGDYMGiBMPSp\r\n"	\
	"VtB5eptX+YUdO84E+86irjOIrsgqO1G7MYu8beuonIrjgmtApOtT7xPPDS39YO1J\r\n"	\
	"mQoyLhuU7nr8QZbiLTi+dSBlytJFvPkgjrgqR7WWkLPEj0w=\r\n"	\
    "-----END CERTIFICATE-----\r\n"

/* Populate here your device certificate content */
#define DEVICE_CERT                                                        \
    "-----BEGIN CERTIFICATE-----\n"                                        \
	"MIICyDCCAbACFFJnmg1mTQALnBB/w4LUQTQ+hckpMA0GCSqGSIb3DQEBCwUAMB8x\r\n" 	\
	"HTAbBgNVBAMMFEFuYWxvZyBEZXZpY2VzLCBJbmMuMB4XDTIzMDIyNzEwMTM1NloX\r\n"	\
	"DTIzMDMyOTEwMTM1NlowIjEgMB4GA1UEAwwXRW5lcmd5TW9uaXRvcmluZ0Rldmlj\r\n"	\
	"ZTEwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDHjhmvEevCFkVm8g7D\r\n"	\
	"+9DJQHwt/pmWOg11EwmsmuaCt3V+j6KfujJRoqCUqlc0/vvqiaUKxlqKRq9Rdjnq\r\n"	\
	"YKcYvWOHT+pXpRlNPxbL/u9gL1t2PTb67UWYKQRYUaP+lH9UFJamKcVd6rDpJDbD\r\n"	\
	"lruzQUr542G5zlzeIsT8Bw9p2Qtk54bdenMK8bcGKcJTUSlLP++30lj1E5dg57XD\r\n"	\
	"uMiwz/dP3TC01/807K59vR3IwSCQUIbes1h3n4xTbgJrl1jx/G1FzGi/Oo5OBIQB\r\n"	\
	"3JFIs8nMMAElGutOAdznyLTzK0IzzvJylJeOJtlbafv2KB1CehNFkXVgkqnugZSX\r\n"	\
	"Vf4ZAgMBAAEwDQYJKoZIhvcNAQELBQADggEBAAkI4/Bfo9bUUWmbzsfPRAACa1yr\r\n"	\
	"ZYhPm7gjB3UO2TNI1+kZq1mbptm4AQlRKHZOc4RwOKw2+gEc++FwK9v8Ai1EcVgW\r\n"	\
	"bCgmL1fWzxwmPCAatjWAIXIwtao4g3xPFOSw8Afg54upUNLzpLHyxyuByLOIs+TQ\r\n"	\
	"qCpjNXMsBblrU+7XOjo0Z5qxyWGxzFkI+/MfW9F5+q0dt77f6GxBmEAj42HIou32\r\n"	\
	"B8CohaPoJ8qJAWd9X+zKGzzstXEmmnlSX0T1H3jaL+ENRNzWpMT6ASt3u+Mq+WpY\r\n"	\
	"che9sgKqK4yPsqewrBSacSORc0aY+Htp1MwVWjnhKI/X+7eEV6CQ3Y7FYoc=\r\n"	\
    "-----END CERTIFICATE-----\r\n";

/* Populate here your device private key content */
#define DEVICE_PRIVATE_KEY                                                 \
    "-----BEGIN PRIVATE KEY-----\n"                                        \
	"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDHjhmvEevCFkVm\r\n"	\
	"8g7D+9DJQHwt/pmWOg11EwmsmuaCt3V+j6KfujJRoqCUqlc0/vvqiaUKxlqKRq9R\r\n"	\
	"djnqYKcYvWOHT+pXpRlNPxbL/u9gL1t2PTb67UWYKQRYUaP+lH9UFJamKcVd6rDp\r\n"	\
	"JDbDlruzQUr542G5zlzeIsT8Bw9p2Qtk54bdenMK8bcGKcJTUSlLP++30lj1E5dg\r\n"	\
	"57XDuMiwz/dP3TC01/807K59vR3IwSCQUIbes1h3n4xTbgJrl1jx/G1FzGi/Oo5O\r\n"	\
	"BIQB3JFIs8nMMAElGutOAdznyLTzK0IzzvJylJeOJtlbafv2KB1CehNFkXVgkqnu\r\n"	\
	"gZSXVf4ZAgMBAAECggEAb6Um5XXHSw0ewxvF+wwVoaL8VtdMomnUQZ3nGbSIJrXx\r\n"	\
	"fF9sAqUvpdCwurwakkHeOzfLKJ4U5avqRk8409JDamn7Fyc02tg5sagMXxFAZ7XX\r\n"	\
	"G+3fpr+84gaAsdDrSXFXU3k5V7mi/Ipjc+yY3xCj7wQmqGv4rvWvq3AUeVSR4Qta\r\n"	\
	"aq/yELOc6zkCuXoPVht6E5xsqhYE428MUVq3T8VJ8fIm410ymDRcroAeobhxzflO\r\n"	\
	"MOX84byL7kLt6QboOHoxUV/BBH8UQWQvZv8aP1gXvPp84jj+pAcf1MdF1sLR4yGa\r\n"	\
	"Mh5KkmcHi95SjvDS0B4Z3D2WgiN7IjeQ2tt0+TF+KQKBgQDqa9FmDEczE6aiJXCq\r\n"	\
	"XnP2J4e4V1WCHpNN2YUcogEWHtEN97XE3Sc7iqoYSCZW7cuR3nG8fqsiIcqf2hn7\r\n"	\
	"bfopUDqmNeV0xLCoDCXeuDnEkRrBBQXTPDoyekFH8KtqLvAFJApPBnX0TbU7bkzO\r\n"	\
	"gEqXLNJlK4MJRCSAWV2V+v8BiwKBgQDZ7KfypeaByTLwOB4WtmlwFZWq8EFN6Wwx\r\n"	\
	"5i3Eo8l5vhtkWws+DyJpBPpX+TAIpnYQ4PSMPNxsft2YFygIR5eBEoIEBtbDfiUv\r\n"	\
	"svlEH6+f+71wabu0ePkMTdEf8pl36YEspFq+0Iz+o53EiqGHb+kLTDc81CAQVBl2\r\n"	\
	"xaKQgDorawKBgQDlYLCZ2QPGL8FKQbZXjmqLfyynLRWnZ8GdWG2OkdrcSTUoJK1A\r\n"	\
	"v2FHOqyra9XQE4iw5+eEmLFdiZEaDzCDPJ6e1Dk/L9ehBWESXiikIMGt3IpAOmjz\r\n"	\
	"w6fygnvkJ9Oi5+DGNvi7UMgUUAE48PnIyfGysRICGqxyYbIRwN/5BIuHdwKBgCcI\r\n"	\
	"3/BzzP00Z95lfuY8mFhOVXe//0KQbCPoAgy19dHLvqZUNIhSN6yuCpWVeggioQVW\r\n"	\
	"9hbkk+sPMmwawb3x7O5evVExVGjCALExkrqkHlY+xmkLV2b1QE725V2em+TBu7Se\r\n"	\
	"X+7L9mVqM0lQN6zF2+19ImvP50pldgYzUnIltcWvAoGAcolYRYjTo3gjxwD9xhAq\r\n"	\
	"TLmCEugNmiYkCHgdOWmkHk+5AOJkctkwCGGu9+Bz1yJ46Afvv6uRO540QJKdaHBS\r\n"	\
	"np2l72ukSASAVaM1S+HyPrz9s1bCTIpB82kHKHuxx4SSSrLuxUkRLz7rZkRvsEai\r\n"	\
	"mKmY/gyPLJZRq4Lr1lOwoCM=\r\n"	\
    "-----END PRIVATE KEY-----\r\n";
#endif

#endif /* __PARAMETERS_H__ */
