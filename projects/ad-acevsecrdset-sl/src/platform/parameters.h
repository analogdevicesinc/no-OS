/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to the Maxim platform used by the STOUT
 *           project.
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
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "maxim_uart_stdio.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_irq.h"
#include "maxim_i2c.h"
#include "maxim_spi.h"
/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
// UART parameters
#define UART_IRQ_ID                     UART0_IRQn

#define UART_DEVICE_ID                  0
#define UART_BAUDRATE                   57600
#define UART_EXTRA                      &ade9113_uart_extra_ip
#define UART_OPS                        &max_uart_ops

#define I2C_DEVICE_ID                   1
#define I2C_OPS                         &max_i2c_ops
#define I2C_EXTRA                       &adt75_i2c_extra

// SPI parameters
#if (TARGET_NUM == 78000)
#define SPI_DEVICE_ID                   1
#define SPI_CS                          1
#elif (TARGET_NUM == 32650)
#define SPI_DEVICE_ID                   1
#define SPI_CS                          0
#else
#define SPI_DEVICE_ID                   1
#define SPI_CS                          0
#endif

#define SPI_BAUDRATE                    1000000
#define SPI_OPS                         &max_spi_ops
#define SPI_EXTRA                       &ade9113_spi_extra_ip

// GPIO parameters
#define GPIO_OPS                        &max_gpio_ops
#define GPIO_EXTRA                      &ade9113_gpio_extra_ip
#define GPIO_RDY_PIN_NUM                15
#define GPIO_RDY_PORT_NUM               0

#define GPIO_IRQ_OPS                    &max_gpio_irq_ops
#define GPIO_CTRL_IRQ_ID                0
#define GPIO_IRQ_EXTRA                  &ade9113_gpio_extra_ip
#define NVIC_GPIO_IRQ                   GPIO0_IRQn

#define GPIO_RESET_PIN_NUM              19
#define GPIO_RESET_PORT_NUM             0

#define GPIO_ZC_PIN                     14

// RCD GPIOs
#define GPIO_RCM_TEST_PIN               18
#define GPIO_RCDDC_PIN                  26
#define GPIO_RCDAC_PIN                  27

// PWM parameters and CP measurement parameters
#define FREQ                            1000            // (Hz)
#define PWM_TIMER                       MXC_TMR0        // Port 0, Pin 2 output
#define PWM_LOW_TIMER                   MXC_TMR1        // Timer for measuring CP Low voltage
#define PWM_DUTY_10A                    (166u)
#define PWM_DUTY_16A                    (266u)
#define PWM_DC                          (999u)
#define PWM_OFF                         (1u)
#define PWM_LOW_TEST_VAL                (160u)
#define PWM_HIGH_TEST_VAL               (1890u)
#define PWM_HIGH_TEST_VAL_EV_CONNECTED  (1600u)
// ADC parameters
#define ADC_CHANNEL                     MXC_ADC_CH_0
// Take action flag
#define TAKE_S_M_ACTION                 (1000u)
// Steps required for RELAY checking
#define RELAY_CHECK_STEP                (3u)
// Steps required for RCD testing
#define MEASURE_RCDAC_RCDDC_STEP        (108u)
#define RESTORE_TEST_PIN_STEP           (3u)
#define SAMPLE_PINS_STEP_1              (37u)
#define SAMPLE_PINS_STEP_2              (72u)
#define SAMPLE_PINS_STEP_3              (108u)
// Steps for requiring overcurrent check
#define MEASURE_OVERCURRENT_STEP        (300u)
// Current limit not charging
#define I_LIMIT_NOT_CHG                 (200u)
// Current limit charging
#define I_LIMIT_16A                     (16500u)
// Curent limit charging low power
#define I_LIMIT_10A                     (10500u)
// Number of periodes skipped before Vin self test
#define SELF_TEST_SKIP_PERIODES_NO      (5u)
// Delay used between Vin and Vrelay readings during startup selftest
#define DELAY_SELF_TEST_READING         (300u)
// Value indicating the delay between readings (default 20ms)
#define VALUE_20MS                      (20000u)
// RCD test repeat disable interval in seconds
#define RCD_TIME_REPEAT_INTERVAL        (10u)
// Print values disable interval in seconds
#define PRINT_VALUES_TIME               (4u)
// The time rate used to compute Vin and Iout (multiple of 20ms)
#define COMPUTE_VALUES_INTERVAL         (5u)
// The time rate used to compute Vrelay (multiple of COMPUTE_VALUES_INTERVAL)
#define COMPUTE_VRELAY_INTERVAL         (25u)
// The rate at which the temperature is read (multiple of 20ms)
#define TEMPERATURE_READ_RATE           (250u)
// Delay for diode error check
#define DIODE_ERR_DELAY                 (20u)
// CP value limit for diode error detected
#define CP_DIODE_ERROR_LIMIT            (150u)
// Charging blinking LED timing for Iout 16A
#define LED_BLINKING_16A                (4u)
// Charging blinking LED timing for Iout 10A
#define LED_BLINKING_10A                (8u)
// Control pilot High side low limit value
#define CP_H_LOW_LIMIT                  (1100u)
// State A CP high side high limit
#define CPH_A_LIMIT_HIGH                (1890u)
// State A CP high side low limit
#define CPH_A_LIMIT_LOW                 (1800u)
// State B CP high side high limit
#define CPH_B_LIMIT_HIGH                (1670u)
// State B CP high side low limi
#define CPH_B_LIMIT_LOW                 (1600u)
// State C CP high side high limit
#define CPH_C_LIMIT_HIGH                (1450u)
// State C CP high side low limit
#define CPH_C_LIMIT_LOW                 (1390u)
// State D CP high side high limit
#define CPH_D_LIMIT_HIGH                (1226u)
// State D CP high side low limit
#define CPH_D_LIMIT_LOW                 (1180u)
// State B delay for changing CP from DC to PWM
#define CP_DC_PWM_DELAY                 (2000u)
// Adjust to new PWM duty time delay
#define ADJUST_TIME                     (5000l)
#if defined(REV_D)
//Upstream PE error detection pin
#define INT_PORT			            (2u)
#define INT_PIN			                (3u)
#define INT_PIN_OPTO1                   (5u)
#define INT_PIN_OPTO2                   (7u)
#endif

// Extra components
extern struct max_gpio_init_param ade9113_gpio_extra_ip;
extern struct max_uart_init_param ade9113_uart_extra_ip;
extern struct max_spi_init_param ade9113_spi_extra_ip;
extern struct max_i2c_init_param adt75_i2c_extra;


#endif /* __PARAMETERS_H__ */
