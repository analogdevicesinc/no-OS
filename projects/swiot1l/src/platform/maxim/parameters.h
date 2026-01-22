/***************************************************************************//**
 *   @file   parameters.h
 *   @brief Definitions specific to Maxim platform used by the swiot1l project
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"
#include "maxim_i2c.h"
#include "maxim_timer.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID  0
#endif
#define UART_IRQ_ID     UART0_IRQn

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200
#define UART_OPS        &max_uart_ops
#define UART_EXTRA      &adin1110_uart_extra_ip

#define SPI_DEVICE_ID   2
#define SPI_BAUDRATE    15000000
#define SPI_CS          0
#define SPI_OPS         &max_spi_ops
#define SPI_EXTRA       &adin1110_spi_extra_ip
#define SPI1_EXTRA       &adxl355_spi_extra_ip


#define I2C_EXTRA	&vddioh_i2c_extra
#define GPIO_EXTRA	&vddioh_gpio_extra

extern struct max_uart_init_param adin1110_uart_extra_ip;
extern struct max_spi_init_param adin1110_spi_extra_ip;
extern struct max_i2c_init_param vddioh_i2c_extra;
extern struct max_gpio_init_param vddioh_gpio_extra;
extern struct max_spi_init_param adxl355_spi_extra_ip; 

#define AD74413R_GPIO_TRIG_IRQ_ID     22
#define AD74413R_GPIO_CB_HANDLE       NULL

#define GPIO_IRQ_ID             1 /* Pin 0 */
#define GPIO_IRQ_OPS            &max_gpio_irq_ops
#define GPIO_IRQ_EXTRA          NULL

#define SCAN_SENSOR_TIME	5000
//#define MBEDTLS_THREADING_PTHREAD
#define MBEDTLS_THREADING_C
#define MBEDTLS_PLATFORM_MEMORY
#ifndef DISABLE_SECURE_SOCKET

/* Populate here your CA certificate content */
#define CA_CERT                                                             \
    "-----BEGIN CERTIFICATE-----\n"                                       \
    "MIICdzCCAh2gAwIBAgIUOZfTq/TjSYTtb1mjimf4+9GWi7QwCgYIKoZIzj0EAwIw\r\n"  \
    "gZAxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdNdW5zdGVyMREwDwYDVQQHDAhMaW1l\r\n"  \
    "cmljazEbMBkGA1UECgwSQW5hbG9nIERldmljZXMgSW5jMRQwEgYDVQQLDAtTZWN1\r\n"  \
    "cml0eSBDQTEpMCcGCSqGSIb3DQEJARYabmlhbGwuYnJvZGVyaWNrQGFuYWxvZy5j\r\n"  \
    "b20wHhcNMjQxMjEzMTU0MjExWhcNMzQxMjExMTU0MjExWjCBkDELMAkGA1UEBhMC\r\n"  \
    "SUUxEDAOBgNVBAgMB011bnN0ZXIxETAPBgNVBAcMCExpbWVyaWNrMRswGQYDVQQK\r\n"  \
    "DBJBbmFsb2cgRGV2aWNlcyBJbmMxFDASBgNVBAsMC1NlY3VyaXR5IENBMSkwJwYJ\r\n"  \
    "KoZIhvcNAQkBFhpuaWFsbC5icm9kZXJpY2tAYW5hbG9nLmNvbTBZMBMGByqGSM49\r\n"  \
    "AgEGCCqGSM49AwEHA0IABIkA1E+I11SIxATgFXo4Y/8/vK7SyvTuMzOG1eDLTT2Z\r\n"  \
    "A3SNQXZNHPNfwBIvcjyrl883hi8/miekqiQw95P6NHWjUzBRMB0GA1UdDgQWBBRV\r\n"  \
    "i6LNI1ApHG6bnC2sAeYCb8crgDAfBgNVHSMEGDAWgBRVi6LNI1ApHG6bnC2sAeYC\r\n"  \
    "b8crgDAPBgNVHRMBAf8EBTADAQH/MAoGCCqGSM49BAMCA0gAMEUCIQDploYuVwiD\r\n"  \
    "15i4SmWU/BQb0iRYgbx3qHpimMj5Z3UKYgIgQstgXgzATpP8/SYpg6I+oiaXjnQA\r\n"  \
    "RrsQYqkIs3VhugY=\r\n"  \
    "-----END CERTIFICATE-----\r\n";

/* Populate here your device certificate content */
#define DEVICE_CERT                                                        \
    "-----BEGIN CERTIFICATE-----\n"                                            \
    "MIICfjCCAiWgAwIBAgIUVztlNCJx90g7pNuIhFkxqCNwok8wCgYIKoZIzj0EAwIw\r\n"  \
    "gZAxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdNdW5zdGVyMREwDwYDVQQHDAhMaW1l\r\n"  \
    "cmljazEbMBkGA1UECgwSQW5hbG9nIERldmljZXMgSW5jMRQwEgYDVQQLDAtTZWN1\r\n"  \
    "cml0eSBDQTEpMCcGCSqGSIb3DQEJARYabmlhbGwuYnJvZGVyaWNrQGFuYWxvZy5j\r\n"  \
    "b20wHhcNMjQxMjEzMTU0ODQ5WhcNMjUxMjEzMTU0ODQ5WjCBqTELMAkGA1UEBhMC\r\n"  \
    "SUUxEDAOBgNVBAgMB011bnN0ZXIxETAPBgNVBAcMCExpbWVyaWNrMRswGQYDVQQK\r\n"  \
    "DBJBbmFsb2cgRGV2aWNlcyBJbmMxGDAWBgNVBAsMD1NlY3VyaXR5RGV2aWNlczET\r\n"  \
    "MBEGA1UEAwwKMTAuMzIuNTEuMjEpMCcGCSqGSIb3DQEJARYabmlhbGwuYnJvZGVy\r\n"  \
    "aWNrQGFuYWxvZy5jb20wWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAATSjeQuZ38F\r\n"  \
    "4L0YwQrlbotQUSRWP1vYTM5MtaVmcTsWJ9Nd0MTFoEzdH0Wzkg5EJ/9XUUF7E0ee\r\n"  \
    "YlzKNkiCylG3o0IwQDAdBgNVHQ4EFgQU3FVkwW7iDIrRLLuesipvh5BvsuIwHwYD\r\n"  \
    "VR0jBBgwFoAUVYuizSNQKRxum5wtrAHmAm/HK4AwCgYIKoZIzj0EAwIDRwAwRAIg\r\n"  \
    "RTLfOEC76K3rwPIP3G0Psrd9AWzICU9sDrr35NXuMEsCIENWpdEoXhj/tTPNMEMC\r\n"  \
    "NoJpwZJtFPnXTtAM8Z1uatlQ\r\n"  \
    "-----END CERTIFICATE-----\r\n";

/* Populate here your device private key content */
#define DEVICE_PRIVATE_KEY                                                 \
    "-----BEGIN EC PRIVATE KEY-----\n"                                     \
    "MHcCAQEEIGu1YwmW5f8NcFixHOmZOPgFfY3TL/mU56unnEkhKpJaoAoGCCqGSM49\r\n"  \
    "AwEHoUQDQgAE0o3kLmd/BeC9GMEK5W6LUFEkVj9b2EzOTLWlZnE7FifTXdDExaBM\r\n"  \
    "3R9Fs5IORCf/V1FBexNHnmJcyjZIgspRtw==\r\n"  \
    "-----END EC PRIVATE KEY-----\r\n";

#endif

#endif /* __PARAMETERS_H__ */
