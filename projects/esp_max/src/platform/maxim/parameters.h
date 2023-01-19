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
#define DPS_SERVER_ADDR	"dps-ylr4pd6jbvxtc.azure-devices-provisioning.net"
#define BUFF_LEN		200
#define TIMER_ID		1
#define MQTT_CONFIG_CMD_TIMEOUT	20000
#define MQTT_CONFIG_VERSION	MQTT_VERSION_3_1_1
#define MQTT_CONFIG_KEEP_ALIVE	7200
#define MQTT_CONFIG_CLI_PASS	NULL
#define SCAN_SENSOR_TIME	500

#define AZ_IOT_PROVISIONING_CUSTOM_PAYLOAD	"{\"modelId\":\"dtmi:com:analog:ADX12345;1\"}"
#define AZ_IOT_PROVISIONING_ID_SCOPE		"0ne00925B00"
#define AZ_IOT_PROVISIONING_REGISTRATION_ID 	"EnergyMonitoringDevice1"

#define SAMPLE_TYPE PAHO_IOT_PROVISIONING
#define SAMPLE_NAME PAHO_IOT_PNP_WITH_PROVISIONING_SAMPLE

#define QUERY_TOPIC_BUFFER_LENGTH 256
#define REGISTER_TOPIC_BUFFER_LENGTH 128
#define PROVISIONING_ENDPOINT_BUFFER_LENGTH 256
#define MQTT_PAYLOAD_BUFFER_LENGTH 256

#define CLIENT_ID_BUFFER_LENGTH 128

#define CA_CERT                                                            \
    "-----BEGIN CERTIFICATE-----\r\n"                                      \
	"MIIDHzCCAgegAwIBAgIUUEovGzdDwqPY0CVSKm1tpC2UYVowDQYJKoZIhvcNAQEL\r\n" \
	"BQAwHzEdMBsGA1UEAwwUQW5hbG9nIERldmljZXMsIEluYy4wHhcNMjMwMTE5MTIz\r\n" \
	"ODMxWhcNMzIxMDE4MTIzODMxWjAfMR0wGwYDVQQDDBRBbmFsb2cgRGV2aWNlcywg\r\n" \
	"SW5jLjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAM/oRFAdUo7yjFAM\r\n" \
	"y9cEiP2/2T+ZY63VNnO62qqsfe5VjHcdN/vbORF3Wo3BPcqHrlHmem5M/Vbw+Zd4\r\n" \
	"x421yQO0hyKmT/VIA2de1SokvLDoAd4ZGI6P4obzfOtaUJmgOfEbVaqDaGPyPGOv\r\n" \
	"fECusxOmle45ooIqa3/mkQxjHthG9STJf8Rm6s5ZRLK/gdJrQci+ZAyhSJCaMyDI\r\n" \
	"Ee4KBAFTqKO+hBewkLE8MMzsKQrsCtQlfbaFJjV1HB55fzORp7tnjnflSm1VLzBA\r\n" \
	"QXt8U53sYxNU/7nve42uZsm65/hoFfLlRdhP3sH4nhYGKEOGJsDsmBh82YMa8mZT\r\n" \
	"RGhMDN8CAwEAAaNTMFEwHQYDVR0OBBYEFHlaykRf7EMbCTixbQjOTqVD9rztMB8G\r\n" \
	"A1UdIwQYMBaAFHlaykRf7EMbCTixbQjOTqVD9rztMA8GA1UdEwEB/wQFMAMBAf8w\r\n" \
	"DQYJKoZIhvcNAQELBQADggEBAAwJJBJ8Ym96ruG9vfn8lCUZAV6MxbAk5naehN6s\r\n" \
	"A0GTl3MTQBVGfv44J3UgtFgi6m6UBT4L2XSJngAuTcpkoRyBfLmPmVytpEVyIwYo\r\n" \
	"S4unJUR14+2tdWxY9VulNIhvd1W82vhte5yBUpJLEiBTWwS7qNEPin3WdGNy2aXc\r\n" \
	"Af+9FGInZYiu80ohDwqHXVMfCdtpbYWlIQ+wR51PBy5CBLM1gc65+deg4rrm+ZNm\r\n" \
	"Pi4gWs/Gz+WRiIOWTTp/OpIC78kBLOV3j4pzquTL+J5qLJc2sAlvC85zaIYuBjtv\r\n" \
	"gw7M6fDJRIFMJb/2BGo//XZlVSRi2PkLbE1TmRkI2kb/l9g=\r\n" \
    "-----END CERTIFICATE-----\r\n"

#define DEVICE_CERT                                                        \
    "-----BEGIN CERTIFICATE-----\n"                                        \
	"MIICyDCCAbACFBV4BYqf1eAygyTVXFHRkfewPERbMA0GCSqGSIb3DQEBCwUAMB8x\r\n" \
	"HTAbBgNVBAMMFEFuYWxvZyBEZXZpY2VzLCBJbmMuMB4XDTIzMDExOTEzMDMxNloX\r\n" \
	"DTIzMDIxODEzMDMxNlowIjEgMB4GA1UEAwwXRW5lcmd5TW9uaXRvcmluZ0Rldmlj\r\n" \
	"ZTEwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCrYpBzIjn6CXHFWVqs\r\n" \
	"FlU/EX577KSGrGlGjKVfZrDmZiP5kNfZSd5mU75dtlT9eExhy09f9+2YtWdbILgn\r\n" \
	"4nm74Za8L4Mf5iUVMsgQ9Bu6jLHgvKdItyKc5C5oLRlX9rlc5DIYxJXHJv6SaOfY\r\n" \
	"X+2f08X2Yin01NhlO0s4lCmKV7PEWXRnwLNrB+0O9DXw5/tM+cvcj2mNeVpzdx+r\r\n" \
	"ExDhabiiMqOVTYvNEeO7JJYDFB++7JfqjRg+GjXkYII96hPbyWu0ClU9+0hjV47f\r\n" \
	"5IDHXWFzPmX/+PIrT9FObreeHdRNgQkF1bqy20GGL9KTBCeZjvgEfiIvN+nGomlK\r\n" \
	"b+tLAgMBAAEwDQYJKoZIhvcNAQELBQADggEBAF+qIGrRJBbk8vGdWkdTu+LLbOPp\r\n" \
	"6EBdMeN494xkRCpmhvg1CmGh6iKmB9kh9ZwVmV9u7dd067cxG01fcuuuLA9xIgtD\r\n" \
	"k3BeOHSzyGxPaaWF5oZIswBxRv1sC5Lr4szPNhRY+MJc5yFPXbPhAlKoT1thaN5c\r\n" \
	"szsGRMuLxmPjqWC65VHwy2p5fEqFC3KA/C5srKzy0z+pgIpN2DlYhCFDSZKTT2jq\r\n" \
	"+e6lZdGgws8vhSwvplNWPt59+lSryrcLn0ThDPn+opyOz71J7cqm3/7EFnUc9I45\r\n" \
	"/4pYs4p/DkLXlGW+Z1AkPoApyfU6lVEms5h9kcbyageL2fvajyHDoFPw+ko=\r\n" \
    "-----END CERTIFICATE-----\r\n";

#define DEVICE_PRIVATE_KEY                                                 \
    "-----BEGIN PRIVATE KEY-----\n"                                        \
	"MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCrYpBzIjn6CXHF\r\n" \
	"WVqsFlU/EX577KSGrGlGjKVfZrDmZiP5kNfZSd5mU75dtlT9eExhy09f9+2YtWdb\r\n" \
	"ILgn4nm74Za8L4Mf5iUVMsgQ9Bu6jLHgvKdItyKc5C5oLRlX9rlc5DIYxJXHJv6S\r\n" \
	"aOfYX+2f08X2Yin01NhlO0s4lCmKV7PEWXRnwLNrB+0O9DXw5/tM+cvcj2mNeVpz\r\n" \
	"dx+rExDhabiiMqOVTYvNEeO7JJYDFB++7JfqjRg+GjXkYII96hPbyWu0ClU9+0hj\r\n" \
	"V47f5IDHXWFzPmX/+PIrT9FObreeHdRNgQkF1bqy20GGL9KTBCeZjvgEfiIvN+nG\r\n" \
	"omlKb+tLAgMBAAECggEAIloPOgSSOlvKQhQ26GdRjuKix5QXJidIsqteOlELBCNU\r\n" \
	"8qDlotf7i+cdMQtq3aNPLArkTggTx5gUr19Pii8K123QGMgtMCehJy6Wi09if3HT\r\n" \
	"9Yfav+h1N51E7/yaJ9d8L/JNWEmO++wE+rOsROJ1bqOz+j6SylAfT6si2KRZ7zBw\r\n" \
	"9d4FJudojzzUl7e6+sTGpqUuncggZ8ulnOFHsqR9pYHzVz9DBStbdC+kZAIy+FIJ\r\n" \
	"j3ZDn1v/4RpfZHfAD0KCGOeYv+ikaalutAe3kJCEdWbHGJm6kMSAEc43NmOpMMfi\r\n" \
	"4zYjdKi0ctKPA9s+OXnW9kkbBEw6P3pxXjJjFZdcAQKBgQDb/416qU5BMASGjjua\r\n" \
	"CtG1cLmeL0egLDYrdDZjH95+aX7OD/gOtvyN/HhyJ0uT+Ip03jkTj8yOwW5iAsST\r\n" \
	"DRflBOrGyP316A+LDz+Hh9tWND1tzr2il0STs72VbFHDKrdpDMXprljj0R+DGf/4\r\n" \
	"Hf85909T/FVD1knwzNsRjr3eqwKBgQDHbnGldTzTE07I8prvGsnZv/TtsnMqC0dJ\r\n" \
	"paCvrtOXkw2qyB9A52QAAaZfa86VA0S4Qa3YH4O/fMiBRd/SXfvwkNWRwE11HiXi\r\n" \
	"y7SlRcsEav5YDyCporhoAqyP+8gmQS1JDHth3BVzf/+vBMa7LwP7+Y4xVakNg95d\r\n" \
	"hgWuQyGl4QKBgA9wLIZ6ERLEAt1KZ+ZUcgjwgyYVnUohMB/2hMaJ+vwV/R8KwdaO\r\n" \
	"gsSX/f/MwCH/JAA1g/3abnlqG4vnM9I57f5VTQBsJF44TbbteADTyNGQ0uq8wlQB\r\n" \
	"l39h8qDkvAwdGEfX7J+Ma5J5H2P81BieSkXQFBIJuBYNmJy8U3c09CAHAoGAEPJY\r\n" \
	"o+e3eHQ+kKLU+ZZ8Ko2+QWiVkrAMqbxyUBt6DXpIggepaDmBbOK3AYrGrjDt6P5j\r\n" \
	"yGa9KktvOlT2tQiZfRLBozYFHvul2oTW3+hzXPe+jzsR2nhiVSxM/5s0odOOBpQR\r\n" \
	"8/ngtMTetD+ip4tSOlo1dAnEaMqY3beiyfEYbeECgYAvhfw+9cb9GfNt/k2j/HvP\r\n" \
	"uflG8wWQk22Uy5enKvqmWQVd9LH0TNsLqQiUPtZ2wQGL31fPD5tcNjS1kgrrahuc\r\n" \
	"mR2dNzs7DxJBbUUd4c6bu4jeYMwkw+s98wYvLB1e0dE+fa/fNyFLgKCLyxRtOmoA\r\n" \
	"0zI+eL4TQPypIu7FuOd+Ew==\r\n" \
    "-----END PRIVATE KEY-----\r\n";

#endif /* __PARAMETERS_H__ */
