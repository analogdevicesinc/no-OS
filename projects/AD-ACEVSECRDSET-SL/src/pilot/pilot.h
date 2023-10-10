/***************************************************************************//**
 *   @file   pilot.h
 *   @brief  Pilot phase interface file.
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
