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
#define SERVER_ADDR		"iot-hub-mopdcwnti4mly.azure-devices.net"
#define DPS_SERVER_ADDR	"dps-mopdcwnti4mly.azure-devices-provisioning.net"
#define BUFF_LEN		200
#define TIMER_ID		1
#define MQTT_CONFIG_CMD_TIMEOUT	20000
#define MQTT_CONFIG_VERSION	MQTT_VERSION_3_1_1
#define MQTT_CONFIG_KEEP_ALIVE	7200
#define MQTT_CONFIG_CLI_PASS	NULL
#define SCAN_SENSOR_TIME	500

#define AZ_IOT_PROVISIONING_CUSTOM_PAYLOAD	"{\"modelId\":\"dtmi:com:analog:ADX12345;1\"}"
#define AZ_IOT_PROVISIONING_ID_SCOPE		"0ne0093A0F8"
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
	"MIIDHzCCAgegAwIBAgIUc+Oqra6h1X6O/0IPGspXOG45LAkwDQYJKoZIhvcNAQEL\r\n"	\
	"BQAwHzEdMBsGA1UEAwwUQW5hbG9nIERldmljZXMsIEluYy4wHhcNMjMwMTI1MTIy\r\n"	\
	"MDE0WhcNMzIxMDI0MTIyMDE0WjAfMR0wGwYDVQQDDBRBbmFsb2cgRGV2aWNlcywg\r\n"	\
	"SW5jLjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANM99Nt4Tx3gAzk1\r\n"	\
	"M+m3fTJQso4747m/kj4p5x9vlHlTLhaLrNwBGZjCcKbPXatbSu6ZZQ2Afr7wRFgi\r\n"	\
	"3CJYrtjbEBQYHwu2vz8iSlmXbDN2hFuVcvjrYCq+pivkP+U/tTi4Ow9GyTUcSH1a\r\n"	\
	"gscY23xZ+Kg4/C3VDlv7lin7SBAM76fqXJVkFAO/I9P1GSoIQV8i3A9Y5JA0Pin+\r\n"	\
	"0GIoj4bKFu9acu2N0wznrbcgBTgEBXhCOKtzvYAhtu0U7zR/dX7XWslNXtkC+9D/\r\n"	\
	"6MBpP67XYYBZ2d6CtJPCrAbN6RnfRnqfP7Ae4w41t7QgYqHQ721BpVMrsTMPeKzf\r\n"	\
	"Uhy4RskCAwEAAaNTMFEwHQYDVR0OBBYEFCzFoI84Pw+yPEv8WxONCenffkYiMB8G\r\n"	\
	"A1UdIwQYMBaAFCzFoI84Pw+yPEv8WxONCenffkYiMA8GA1UdEwEB/wQFMAMBAf8w\r\n"	\
	"DQYJKoZIhvcNAQELBQADggEBABnmJApLvZ2rt8Qaybu3oQkUuQsChlxkbg2SiBqK\r\n"	\
	"yTXam3OudT4ON0EVuj412eayklX1fSbITe0rMBhaLZRQe/Pf4cr+FpfACgLeT4HP\r\n"	\
	"SeLQAvPySZ5dAcqTjFX/a71FoU1IshfVfvadHeq59x1C2NpDT0/cCR2/aexbrgt4\r\n"	\
	"ZxchzLxc+VX803yf6VCHxybGRu9YMVxszP3UDx+KMVchN1YtlQ110PaxEcdOUhPQ\r\n"	\
	"vENDWxA/LdW0yO04QC/cXHbHnrJwtri+dCXLmP2OdPUnCvdTlHOA20upxbWL9u3x\r\n"	\
	"Jwf/SyMHSrXBH4OfpDQCWhdGvMuXdt8K+chCD6WkymiJbVQ=\r\n"	\
    "-----END CERTIFICATE-----\r\n"

#define DEVICE_CERT                                                        \
    "-----BEGIN CERTIFICATE-----\n"                                        \
	"MIICyDCCAbACFDP3EO4cwQwlyiOLON4/OppHIVgbMA0GCSqGSIb3DQEBCwUAMB8x\r\n"	\
	"HTAbBgNVBAMMFEFuYWxvZyBEZXZpY2VzLCBJbmMuMB4XDTIzMDEyNTEyMjYyMloX\r\n"	\
	"DTIzMDIyNDEyMjYyMlowIjEgMB4GA1UEAwwXRW5lcmd5TW9uaXRvcmluZ0Rldmlj\r\n"	\
	"ZTEwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCht47OhatdEHlV3IkE\r\n"	\
	"3IQE06lgRrN42Hiwf8agAtZ0FKFz2tYFSELAvZ9MYM0Tdg0+6yt2vTVa7Hm9mhrL\r\n"	\
	"Pz1pJPf4RinZ+/mcz0fl0qFNVoSB+y2Dy3hOdTqo7ciDtcjgmt+JJHKdfRQs3KGZ\r\n"	\
	"bmLqXBIIWBaiHowcTujOp2BmwNU9bGyHb78WdoVkwY9tDhNBfLOnDCFi+TlWgOEN\r\n"	\
	"pfybPSu7gK+/sxdygBBKSK3fQKcejdJ31Gy+JzzX5HFDXiREWy7rfj6MBEKSKq6d\r\n"	\
	"eTXxjS3QDj/etDX8yLSyrAh2MnVgQGlEXLJFCejQDSeHGgWXO1UBLtSOglqTI1fL\r\n"	\
	"qQSBAgMBAAEwDQYJKoZIhvcNAQELBQADggEBAEqlPflHBkNEEOLyTn51tYvCgiUY\r\n"	\
	"ISplsMNJEjjJVpWDtMjsa5qaYbqfk54f2N0Irgd+g/3tyuQi7WiDhddxWlb/ugn0\r\n"	\
	"Xbh1ZpdCBV/JI6N8o8eOwe+PbZWTOK8LkabRKAWtPWuhYGfHio7uClaINdcZtJOu\r\n"	\
	"RAT7+O94XrMSUYmQg9x/eeyC0YSRgnfBauxg0e7b6ItAlUTL2zFlaA+9ZBhB6Vlb\r\n"	\
	"g1H0X8cpy1dd7AvBwIAyboU8Avxg1rlVQU3iqQ16AKB5B7rbuYLazsRNOXYMyWSm\r\n"	\
	"jvZD7gkyecItXfzkZCPRw/8KKipF6UlDDsWRGa4LiSyQcOeE04/wkB3VqEk=\r\n"	\
    "-----END CERTIFICATE-----\r\n";

#define DEVICE_PRIVATE_KEY                                                 \
    "-----BEGIN PRIVATE KEY-----\n"                                        \
	"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCht47OhatdEHlV\r\n"	\
	"3IkE3IQE06lgRrN42Hiwf8agAtZ0FKFz2tYFSELAvZ9MYM0Tdg0+6yt2vTVa7Hm9\r\n"	\
	"mhrLPz1pJPf4RinZ+/mcz0fl0qFNVoSB+y2Dy3hOdTqo7ciDtcjgmt+JJHKdfRQs\r\n"	\
	"3KGZbmLqXBIIWBaiHowcTujOp2BmwNU9bGyHb78WdoVkwY9tDhNBfLOnDCFi+TlW\r\n"	\
	"gOENpfybPSu7gK+/sxdygBBKSK3fQKcejdJ31Gy+JzzX5HFDXiREWy7rfj6MBEKS\r\n"	\
	"Kq6deTXxjS3QDj/etDX8yLSyrAh2MnVgQGlEXLJFCejQDSeHGgWXO1UBLtSOglqT\r\n"	\
	"I1fLqQSBAgMBAAECggEALUZiqNcrfsaZ8bvocH/8Rg2sIdQpcu/H+kILrcidmaoc\r\n"	\
	"+X6O5LOyXJu5+Xwwbf5i/vaAGKIoSehivP5j+fseik1bTxr1pJ2y6H/vtRwfJLPQ\r\n"	\
	"LUPRHlDcwbp4TcsklBtUve0eIBsgK49VEqpCY95PuyVDtypSS9kZiONeslyR1LI+\r\n"	\
	"EUqRWpi9mPLfwJ8T2Mx8Y8QoNoXBL9JFdX8fpdbzpIj/XRWynW2GRRgZH7NW5cI1\r\n"	\
	"o+7+u2GJAw5A+LMx+wXL7gvm7yYiigiIn6Oam9o5cGrklbMq1J1DdAfCgqwAT4JO\r\n"	\
	"CpcJ+Cn556JZOpWT+tIVuaif0OXwVbbAiTTJVS++cQKBgQDNvD9qrNO2K+0Y5eDN\r\n"	\
	"uEFjQppTUEE+b2uYdrqVHSfGns5tnoNs454hn8hhvBfMBGrsP1XejgdwbwFexl/r\r\n"	\
	"88iQzqxscv1+i5ABna5WzbcZysUeQgXtII3VCd+UXcwxxieL7DDGHKNYUWqUYVyO\r\n"	\
	"s3Z00bCw/rwdoDR7SwNt4dPb+wKBgQDJOi0/162cu6ZhHhBpaq3rbf6BOr+d8seK\r\n"	\
	"jauMOA+TMhze9MqXCIEuCv8ktTIS5XqoFwUEWBPNiEBuvgYS7OpdqUDbNS5oJ5b8\r\n"	\
	"eU9iepSppgX+6kor5vIVedBsQUADZfJHM5Xoe3CL4Qi+H5FrVgmwpIZvMTE7e/Zi\r\n"	\
	"/mGfUuhcswKBgHJFWe/1WI45rbKDlC6PZfos9yzJScPU/S0s3QuPqxUeBchLH/Ju\r\n"	\
	"JtnENvdaEtmlpoHb1+UKfEVN0rQCeSrEOCV+NbTy1avg9rZ/D2N1VXsE7iGVSFdi\r\n"	\
	"GoU2u7Fbu9v1lOeZ46lWJMAcUJdj0JYORgAyq/JTOsc1wZCtpJT2tsbbAoGAUM7K\r\n"	\
	"CQ9FquE4aVu3X331pcugHqlG9djfBObrcv24U3YWXfqTuJ8zsRe9ajJRj4EUBphn\r\n"	\
	"7VypZypGJbKtob/+ReqIHv92SWFMosrcdEGhnZcMZVgUvRibMsCeUDrLyBGSf8FZ\r\n"	\
	"e+jGQ4X1CtSGg0Ij1gnb6p8olCitwwdAkMisgz0CgYEApu0faIrgXJGxuxnHzZUF\r\n"	\
	"tulQANKwNwy2Y5tj6E765p29XLjFpzjp8ovu/Rx7ZEyuDYyIuDTjYM59ixUjX3PS\r\n"	\
	"IJ1ddXeZ4wy4D5AWnkVdEjtt0leFsGKrd7Y3quX/nobQBFY3xjqOUHMJfL2NOyGP\r\n"	\
	"lWvqonWWbqFUj/XLg9gMF58=\r\n"	\
    "-----END PRIVATE KEY-----\r\n";

#endif /* __PARAMETERS_H__ */
