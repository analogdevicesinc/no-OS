/***************************************************************************//**
 *   @file   parameters.h
 *   @brief Definitions specific to Maxim platform used by the swiot1l project
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

#ifndef DISABLE_SECURE_SOCKET
/* Populate here your CA certificate content */
#define CA_CERT                                                            \
    "-----BEGIN CERTIFICATE-----\r\n"                                       \
    "MIIB4DCCAYWgAwIBAgIUHpqP4S7K66hCtVEh+RN77svnW9gwCgYIKoZIzj0EAwIw\r\n"  \
    "RTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoMGElu\r\n"  \
    "dGVybmV0IFdpZGdpdHMgUHR5IEx0ZDAeFw0yNDEwMDQxNDEyNDFaFw0zNDEwMDIx\r\n"  \
    "NDEyNDFaMEUxCzAJBgNVBAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEwHwYD\r\n"  \
    "VQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwWTATBgcqhkjOPQIBBggqhkjO\r\n"  \
    "PQMBBwNCAARwmufphokl0wcFvDdWCK7Ep5wW6bU42wWtiIi61VOqiPZQSsLDXoce\r\n"  \
    "OM3lappoIXZN/ZYOmecM/hpJ1ngRwlmvo1MwUTAdBgNVHQ4EFgQUoGglOXpmAXqM\r\n"  \
    "FLmD1/CI0G24ftYwHwYDVR0jBBgwFoAUoGglOXpmAXqMFLmD1/CI0G24ftYwDwYD\r\n"  \
    "VR0TAQH/BAUwAwEB/zAKBggqhkjOPQQDAgNJADBGAiEAmB3i33r6fnCxTmYhMomc\r\n"  \
    "OtSZIUge085aVD5CLqdzXiwCIQCNTihRRvEAbihzZ2NvzOox7XJ6qL/RrKIIqt59\r\n"  \
    "TUHO3g==\r\n"  \
    "-----END CERTIFICATE-----\r\n";

/* Populate here your device certificate content */
#define DEVICE_CERT                                                        \
    "-----BEGIN CERTIFICATE-----\n"                                          \
    "MIIB3TCCAYOgAwIBAgIUbloQHlMZ+kpmwOcBnyUFCjUuTJ4wCgYIKoZIzj0EAwIw\r\n"  \
    "RTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoMGElu\r\n"  \
    "dGVybmV0IFdpZGdpdHMgUHR5IEx0ZDAeFw0yNDEwMDQxNDE0NTVaFw0yNTEwMDQx\r\n"  \
    "NDE0NTVaMFQxCzAJBgNVBAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEwHwYD\r\n"  \
    "VQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQxDTALBgNVBAMMBGplYW4wWTAT\r\n"  \
    "BgcqhkjOPQIBBggqhkjOPQMBBwNCAATwEpCVKjiEUDkZDY0w0iQ/EOOZ/3uE1Mfe\r\n"  \
    "Sl2yXlU1sA0BONIvtwCXAHmvgsYv01uWX3dMbtAk69DeLdZOb4npo0IwQDAdBgNV\r\n"  \
    "HQ4EFgQUuyguzpQo8u/6Ib9NWetXbtXhmwMwHwYDVR0jBBgwFoAUoGglOXpmAXqM\r\n"  \
    "FLmD1/CI0G24ftYwCgYIKoZIzj0EAwIDSAAwRQIhAKOAjczTfadWxsmj3lyW+RL3\r\n"  \
    "64u8bX4iPmJXNTQz7tOJAiAUahgCzBCRf/1eyJ6Ks6wHiuTNmufakOjnQ8z3vFY9\r\n"  \
    "8Q==\r\n"  \
    "-----END CERTIFICATE-----\r\n";    

/* Populate here your device private key content */
#define DEVICE_PRIVATE_KEY                                                 \
    "-----BEGIN EC PRIVATE KEY-----\n"                                      \
    "MHcCAQEEIGJ6HljUxTGMFCAkchF4E2sPb/Ank//Qia5zof3vCLX9oAoGCCqGSM49\r\n"  \
    "AwEHoUQDQgAE8BKQlSo4hFA5GQ2NMNIkPxDjmf97hNTH3kpdsl5VNbANATjSL7cA\r\n"  \
    "lwB5r4LGL9Nbll93TG7QJOvQ3i3WTm+J6Q==\r\n"  \
    "-----END EC PRIVATE KEY-----\r\n";

#endif

#endif /* __PARAMETERS_H__ */
