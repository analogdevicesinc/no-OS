/***************************************************************************//**
 *   @file   pilot.h
 *   @brief  Pilot phase interface file.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PILOT_H__
#define __PILOT_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "state_machine.h"
#include "parameters.h"
#include "no_os_irq.h"

#define ADC_VREF	(1220u)
/******************************************************************************/
/************************* Functions Declarations *****************************/
/******************************************************************************/

/*! Get PWM low flag value. */
int get_pwm_low_flag_state(void);

/*! Get PWM LOW value */
uint32_t get_pwm_low_val(void);

/*! Get PWM HIGH value */
uint32_t get_pwm_high_val(void);

/*! Reset PWM low flag value. */
void reset_pwm_low_flag_state(void);

/*! Initialize pilot phase */
int pilot_init(struct stout *stout);

/*! Set PMW parameters. */
void pilot_pwm_timer_setup(unsigned int duty_cycle);

/*! Set pilot CP analog read interrupts. */
int pilot_interrupts_setup(struct no_os_irq_ctrl_desc *desc,
			   struct stout *stout);

/*! Set PMW duty cycle. */
void pilot_pwm_timer_set_duty_cycle(struct stout *stout,
				    unsigned int duty_cycle);

/*! Setup PWM voltage CP analog ADC */
int pilot_setup_adc(void);

/*! Read PWM voltage value. */
unsigned int pilot_read_val(void);

/*! Compute ADC voltage value */
uint32_t pilot_convert_to_mv(uint32_t value);

/*! Compute PWM value average */
uint32_t pilot_compute_average(uint8_t high);

/*! Update PWM values in the state machine structure */
int pilot_write_new_values(struct stout *stout);

/*! Update PWM values */
void pilot_update_vals(void);

/*! Get milliseconds count */
uint32_t get_count_ms(void);

/*! Reset milliseconds count */
void reset_count_ms(void);

#endif /* __PILOT_H__ */
