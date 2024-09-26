/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ADE9153A example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ade9153a.h"
#include "no_os_uart.h"
#include "no_os_pwm.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_print_log.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"
#include "maxim_uart_stdio.h"
#include "maxim_pwm.h"
#include "maxim_spi.h"
#include "maxim_irq.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

// UART init params
extern struct no_os_uart_init_param uart_ip;
// GPIO LED init params
extern struct no_os_gpio_init_param gpio_led1_ip;
// SPI init params
extern struct no_os_spi_init_param ade9153a_spi_ip ;
extern struct no_os_gpio_init_param gpio_ss_ip;
extern struct no_os_gpio_init_param gpio_sck_ip;
// GPIO RESET init params
extern struct no_os_gpio_init_param gpio_reset_ip;
// GPIO RDY init params
extern struct no_os_gpio_init_param ade9153a_gpio_rdy_ip;
// GPIO IRQ init params
extern struct no_os_irq_init_param ade9153a_gpio_irq_ip;

/* Configuration for AD-APARD32690-SL */
// Port and pin for user LED
#define GPIO_LED_PORT               2
#define GPIO_LED_PIN                7
// SPI port and pins
#define GPIO_SS_PORT                2
#define GPIO_SS_PIN                 26
#define GPIO_SCK_PORT               2
#define GPIO_SCK_PIN                29
// Hardware reset port and pin
#define GPIO_RESET_PORT             2
#define GPIO_RESET_PIN              8
// Data ready interrup port and pin
#define GPIO_OPS                    &max_gpio_ops
#define GPIO_EXTRA                  &ade9153_gpio_extra_ip
#define GPIO_RDY_PORT               2
#define GPIO_RDY_PIN                9
// SPI config
#define SPI_DEVICE_ID               0
#define SPI_BAUDRATE                1000000
#define SPI_CS                      1
#define SPI_SLAVE_NUM               1
// UART config
#define UART_DEV_ID                 0
#define UART_BAUD                   115200
// IRQ config
#define GPIO_IRQ_OPS                &max_gpio_irq_ops
#define GPIO_CTRL_IRQ_ID            0
#define GPIO_IRQ_EXTRA              &ade9153a_gpio_extra_ip
#define NVIC_GPIO_IRQ               GPIO2_IRQn
// Value used for delay after reset
#define RESET_TIME                  500
#define SPI_EN                      1
// Read data interval in ms
#define READ_INTERVAL               3000

// Setup values for ADE9153A used for EV-ADE9153ASHIELDZ
// Vlevel value assuming 1/2 full scale
#define ADE9153A_VLEVEL             0x002C11E8
// value for R_small 1k
#define ADE9153A_RSMALL             0x03E8
// energy accumulate no of samples set to 4000
#define ADE9153A_NO_SAMPLES         0x0F9F
// AIGAIN -1 (IAP-IAN swap) AIGAIN = -1*2^28
#define ADE9153A_AIGAIN             -(1 << 28)
#define ADE9153A_AIGAIN2            -(1 << 27)

// Calibration values for EV-ADE9153ASHIELDZ
// Energy vals calibration coeffiecient (uWh/code)
#define ENERGY_CC                   0.858307
// Energy vals calibration coeffiecient (uW/code)
#define POWER_CC                    1508.743
// Current calibration coeffiecient (uA/code)
#define RMS_CURENT_CC               0.838190
// Voltage calibration coeffiecient (uV/code)
#define RMS_VOLTAGE_CC              13.41105
// PF calibration coeffiecient
#define PF_CC                       134217728
// Frequency calibration coeffiecient
#define FREQ_CC                     262144000
// Frequency multiplier for angle compute
#define MULT_CC_50HZ                0.017578125
// Frequency multiplier for angle compute
#define MULT_CC_60HZ                0.02109375

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Toggle user LED */
int interface_toggle_led(struct no_os_gpio_desc *gpio_led_desc);

/* Read measurements */
int read_measurements(struct ade9153a_dev *dev);

/* Autocalibration read values */
int autocalibration_read_vals(struct ade9153a_dev *dev);

/* Autocalibration start */
int autocalibration_start(struct ade9153a_dev *dev);

#endif
