/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ADE9153A example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/


/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

// UART init params
extern struct no_os_uart_init_param ade9153a_uart_ip;
// SPI init params
extern struct no_os_spi_init_param ade9153a_spi_ip;
// GPIO RDY init params
extern struct no_os_gpio_init_param ade9153a_gpio_rdy_ip;
// GPIO RESET init params
extern struct no_os_gpio_init_param ade9153a_gpio_reset_ip;
// GPIO zero cross detection
extern struct no_os_gpio_init_param ade9153a_gpio_ZX_ip;
// GPIO relay control
extern struct no_os_gpio_init_param ade9153_gpio_relay_ip;
// GPIO IRQ init params
extern struct no_os_irq_init_param ade9153a_gpio_irq_ip;
// ADE9113 device init params
extern struct ade9153a_init_param ade9113_ip;

#define GPIO_LED_PORT               2                
#define GPIO_LED_PIN                7
#define GPIO_SS_PORT                2
#define GPIO_SS_PIN                 26
#define GPIO_SCK_PORT               2
#define GPIO_SCK_PIN                29
#define GPIO_RESET_PORT             2
#define GPIO_RESET_PIN              8
#define SPI_DEVICE_ID               0
#define SPI_BAUDRATE                1000000
#define SPI_CS                      1
#define RESET_TIME                  500
#define SPI_SERIAL                  1

// Vlevel value assuming 1/2 full scale
#define ADE9153A_VLEVEL             0x002C11E8
// value for R_small 1k
#define ADE9153A_RSMALL             0x03E8
// energy accumulate no of samples set to 4000
#define ADE9153A_NO_SAMPLES         0x0F9F
// AIGAIN -1 (IAP-IAN swap) AIGAIN = -1*2^28 
#define ADE9153A_AIGAIN             -(1 << 28)

#endif