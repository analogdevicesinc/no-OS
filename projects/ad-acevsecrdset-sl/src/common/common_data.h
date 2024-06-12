/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by the STOUT project.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
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
#include "parameters.h"
#include "ade9113.h"
#include "adt75.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
// UART init params
extern struct no_os_uart_init_param ade9113_uart_ip;
// ADT75 I2C params
extern struct adt75_init_param adt75_ip;
// SPI init params
extern struct no_os_spi_init_param ade9113_spi_ip;
// GPIO RDY init params
extern struct no_os_gpio_init_param ade9113_gpio_rdy_ip;
// GPIO RESET init params
extern struct no_os_gpio_init_param ade9113_gpio_reset_ip;
// GPIO zero cross detection
extern struct no_os_gpio_init_param ade9113_gpio_ZX_ip;
// GPIO RCM TEST
extern struct no_os_gpio_init_param ade9113_gpio_rcm_test_ip;
// GPIO RCDAC
extern struct no_os_gpio_init_param ade9113_gpio_rcdac_ip;
// GPIO RCDDC
extern struct no_os_gpio_init_param ade9113_gpio_rcddc_ip;
// GPIO relay control
extern struct no_os_gpio_init_param ade9113_gpio_relay_ip;
// GPIO IRQ init params
extern struct no_os_irq_init_param ade9113_gpio_irq_ip;
// ADE9113 device init params
extern struct ade9113_init_param ade9113_ip;

// LEDs
extern struct no_os_gpio_init_param ade9113_gpio_led2_ip;
extern struct no_os_gpio_init_param ade9113_gpio_led1_ip;
extern struct no_os_gpio_init_param ade9113_gpio_led3_ip;
extern struct no_os_gpio_init_param ade9113_gpio_led4_ip;

// Vin min and max; Current values +15%/-15% from nominal value 230V
#define VIN_LOW_LIMIT           (195000l)
#define VIN_HIGH_LIMIT          (264000l)

// Vin min and max; Current values +15%/-15% from nominal value 120V
#define VIN_LOW_LIMIT_2         (102000l)
#define VIN_HIGH_LIMIT_2        (138000l)

// For the relay stuck detection values see the documentation (dependent on the cable attached)
// V relay_stuch min and max
#if defined(REV_A)
#define VR_OPEN_LOW_LIMIT       (2000l)
#elif defined(REV_D)
#define VR_OPEN_LOW_LIMIT       (50000l)
#endif
#define VR_OPEN_HIGH_LIMIT      (264000l)

// V2 relay closed maximum value
#define V2_CLOSED_LIMIT         (2000l)

//Temperature limits
#define TEMPERATURE_LIMIT_1     (45000l)
#define TEMPERATURE_LIMIT_2     (50000l)
#define MILLIDEGREE_PER_DEGREE	(1000UL)

//Relay control pin
#if defined(REV_A)
#define RELAY_CTRL_PORT     (0)
#define RELAY_CTRL_PIN      (10)
#elif defined(REV_D)
#define RELAY_CTRL_PORT     (1)
#define RELAY_CTRL_PIN      (4)
#endif

#endif /* __COMMON_DATA_H__ */
