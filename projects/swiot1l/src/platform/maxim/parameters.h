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
#define CA_CERT                                                             \
    "-----BEGIN CERTIFICATE-----\n"                                       \
    "MIICGTCCAb+gAwIBAgIUfUilbMM2CLs3W2DzTLc334CISgUwCgYIKoZIzj0EAwIw\r\n"  \
    "YjELMAkGA1UEBhMCQVUxCzAJBgNVBAgMAkFBMQswCQYDVQQHDAJCQjEMMAoGA1UE\r\n"  \
    "CgwDQURJMQswCQYDVQQLDAJDQzELMAkGA1UEAwwCREQxETAPBgkqhkiG9w0BCQEW\r\n"  \
    "AkVFMB4XDTI0MTAyNDExMDYxOFoXDTM0MTAyMjExMDYxOFowYjELMAkGA1UEBhMC\r\n"  \
    "QVUxCzAJBgNVBAgMAkFBMQswCQYDVQQHDAJCQjEMMAoGA1UECgwDQURJMQswCQYD\r\n"  \
    "VQQLDAJDQzELMAkGA1UEAwwCREQxETAPBgkqhkiG9w0BCQEWAkVFMFkwEwYHKoZI\r\n"  \
    "zj0CAQYIKoZIzj0DAQcDQgAEdmQy9SlQrHP+2bAoGXJREWDD35QAQxR/zHZimxJ+\r\n"  \
    "fILRTt/CGIuo4CUTX4cRByCnOz7VUD71Sxbx6oZaycLtnaNTMFEwHQYDVR0OBBYE\r\n"  \
    "FAFaULsdIF3XDldeIrLANi9M/SYlMB8GA1UdIwQYMBaAFAFaULsdIF3XDldeIrLA\r\n"  \
    "Ni9M/SYlMA8GA1UdEwEB/wQFMAMBAf8wCgYIKoZIzj0EAwIDSAAwRQIhAKcZuIPt\r\n"  \
    "+xp97vfHl94n4jp8ejb/8KispWj66OKwGf52AiA7Qpts04IkPNbDfFcfsAGZORYy\r\n"  \
    "jgrJ7IyKvBu3h+RNVg==\r\n"  \
    "-----END CERTIFICATE-----\r\n";

/* Populate here your device certificate content */
#define DEVICE_CERT                                                        \
    "-----BEGIN CERTIFICATE-----\n"                                            \
    "MIICEjCCAbegAwIBAgIUK/zCtY0zFeo75Ih51lgMcV+T0f0wCgYIKoZIzj0EAwIw\r\n"  \
    "YjELMAkGA1UEBhMCQVUxCzAJBgNVBAgMAkFBMQswCQYDVQQHDAJCQjEMMAoGA1UE\r\n"  \
    "CgwDQURJMQswCQYDVQQLDAJDQzELMAkGA1UEAwwCREQxETAPBgkqhkiG9w0BCQEW\r\n"  \
    "AkVFMB4XDTI0MTAyNDExMDk0OVoXDTI1MTAyNDExMDk0OVowazELMAkGA1UEBhMC\r\n"  \
    "TEwxCzAJBgNVBAgMAk1NMQswCQYDVQQHDAJOTjELMAkGA1UECgwCT08xCzAJBgNV\r\n"  \
    "BAsMAlBQMRUwEwYDVQQDDAwxOTIuMTY4LjAuODAxETAPBgkqhkiG9w0BCQEWAlBQ\r\n"  \
    "MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEBDJAVCLqAtZoI/S1Yq3HYYXfgPYc\r\n"  \
    "jVPNz1nW4wa/CoEd0iMQggA5eoJ/ZYjpRb0irQ6wXMtyNSq/D+gF9hBl4KNCMEAw\r\n"  \
    "HQYDVR0OBBYEFGFTeTvNQUSlQ/dN+AELzLSToBnCMB8GA1UdIwQYMBaAFAFaULsd\r\n"  \
    "IF3XDldeIrLANi9M/SYlMAoGCCqGSM49BAMCA0kAMEYCIQD5/l3uibkt+4h2FRCH\r\n"  \
    "kUJw6+DrUocBsPSTNW6IXjpmRQIhAO+E+LxgmM2uSH9ovxssm9eUECHb/YTj1AxN\r\n"  \
    "MoRSGDYf\r\n"  \
    "-----END CERTIFICATE-----\r\n";

/* Populate here your device private key content */
#define DEVICE_PRIVATE_KEY                                                 \
    "-----BEGIN EC PRIVATE KEY-----\n"                                     \
    "MHcCAQEEIL8dt3Q3X3PLyFYKl5ZGC7QefyDpT5ZmDO3q0UvkpzO+oAoGCCqGSM49\r\n"  \
    "AwEHoUQDQgAEBDJAVCLqAtZoI/S1Yq3HYYXfgPYcjVPNz1nW4wa/CoEd0iMQggA5\r\n"  \
    "eoJ/ZYjpRb0irQ6wXMtyNSq/D+gF9hBl4A==\r\n"  \
    "-----END EC PRIVATE KEY-----\r\n";

#endif

#endif /* __PARAMETERS_H__ */
