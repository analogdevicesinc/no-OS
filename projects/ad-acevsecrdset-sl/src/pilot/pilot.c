/***************************************************************************//**
 *   @file   pilot.c
 *   @brief  Pilot phase implementation file.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_print_log.h"
#include "interface.h"
#include "adc_reva.h"
#include "ade9113.h"
#include "pilot.h"
#include "gpio.h"
#include "tmr.h"
#include "adc.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/******************************************************************************/
/********************************** Variables *********************************/
/******************************************************************************/
// Flag indicating PWM values have been acquired
static volatile int flag_pwm_low;
// Milliseconds count
static uint32_t count_ms;

// Instant PWM values
static volatile uint32_t pwm_low_val;
static volatile uint32_t pwm_high_val;

// Last 10 PWM values used for averaging
static uint32_t pwm_high[10] = { 0 }, pwm_low[10] = { 0 };

extern struct no_os_irq_ctrl_desc *stout_nvic_desc;

/******************************************************************************/
/*************************** Functions Definitions ****************************/
/******************************************************************************/

/**
 * @brief TMR0 ISR
 *
 * @return none
 */
void TMR0_IRQHandler_CP()
{
	// Disable ADE9113 irq (all GPIO interrupts)
	NVIC_DisableIRQ(NVIC_GPIO_IRQ);

	// Store PWM LOW value
	pwm_low_val = MXC_ADC->data;
	// Trigger a new conversion
	MXC_ADC->ctrl |= (uint32_t)(0x1UL);
	// Start TMR1
	MXC_TMR_Start(PWM_LOW_TIMER);
	// Clear TMR0 flags
	MXC_TMR_ClearFlags(PWM_TIMER);

	// Enable ADE9113 irq (all GPIO interrupts)
	NVIC_EnableIRQ(NVIC_GPIO_IRQ);
}

/**
 * @brief TMR1 ISR
 *
 * @return none
 */
void TMR1_IRQHandler_CP()
{
	// Disable ADE9113 irq (all GPIO interrupts)
	NVIC_DisableIRQ(NVIC_GPIO_IRQ);

	// Store PWM LOW value
	pwm_high_val = MXC_ADC->data;
	// Trigger a new conversion
	MXC_ADC->ctrl |= (uint32_t)(0x1UL);
	// Clear TMR1 flags
	MXC_TMR_ClearFlags(PWM_LOW_TIMER);
	MXC_TMR_Stop(PWM_LOW_TIMER);
	MXC_TMR_SetCount(PWM_LOW_TIMER, 1);

	// Increase flg to signal main loop that a conversion took place
	flag_pwm_low++;
	count_ms++;

	// Enable ADE9113 irq (all GPIO interrupts)
	NVIC_EnableIRQ(NVIC_GPIO_IRQ);
}

/**
 * @brief Get PWM low flag value
 *
 * @return flag value
 */
int get_pwm_low_flag_state(void)
{
	return flag_pwm_low;
}

/**
 * @brief Get PWM LOW value
 *
 * @return PWM LOW value
 */
uint32_t get_pwm_low_val(void)
{
	return pwm_low_val;
}

/**
 * @brief Get PWM HIGH value
 *
 * @return PWM HIGH value
 */
uint32_t get_pwm_high_val(void)
{
	return pwm_high_val;
}

/**
 * @brief Reset PWM low flag value
 *
 * @return none
 */
void reset_pwm_low_flag_state(void)
{
	flag_pwm_low = 0;
}

/**
 * @brief Initialize pilot phase
 *
 * @return 0 in case of success, error code otherwise
 */
int pilot_init(struct stout *stout)
{
	int ret;

	/* Initialize ADC */
	ret = pilot_setup_adc();

	if (ret != E_NO_ERROR)
		return ret;

	/* Setup pilot interrupts for reading CP analog */
	ret = pilot_interrupts_setup(stout_nvic_desc, stout);
	if (ret)
		return ret;

	/* Setup PWM parameters and start PWMs */
	pr_debug("Starting PWM timer\n");
	pilot_pwm_timer_setup(PWM_DC);

	//clear selction bits
	MXC_ADC->ctrl &= ~(MXC_F_ADC_REVA_CTRL_CH_SEL);
	//set selction its to next channel to convert
	MXC_ADC->ctrl |= (ADC_CHANNEL << MXC_F_ADC_REVA_CTRL_CH_SEL_POS) &
			 MXC_F_ADC_REVA_CTRL_CH_SEL;

	return 0;
}

/**
 * @brief Set PMW parameters
 * @param duty_cycle - duty cycle of output PWM
 * @return none
 */
void pilot_pwm_timer_setup(unsigned int duty_cycle)
{
	mxc_tmr_cfg_t tmr; // timer configuration for output PWM
	mxc_tmr_cfg_t tmr1; // timer configuration for
	// measuring input PWM Low voltage value
	// period and initial duty cycle of output PWM
	unsigned int periodTicks = PeripheralClock / FREQ;
	unsigned int dutyTicks = periodTicks * duty_cycle / 1000;

	/* Output PWM settings */
	/*
	Steps for configuring a timer for PWM mode:
	1. Disable the timer
	2. Set the pre-scale value
	3. Set polarity, PWM parameters
	4. Configure the timer for PWM mode
	5. Enable Timer
	*/
	MXC_TMR_Shutdown(PWM_TIMER);

	tmr.pres    = TMR_PRES_1;
	tmr.mode    = TMR_MODE_PWM;
	tmr.cmp_cnt = periodTicks;
	tmr.pol     = 1;

	MXC_TMR_Init(PWM_TIMER, &tmr, TRUE);
	MXC_TMR_SetPWM(PWM_TIMER, dutyTicks);
	MXC_NVIC_SetVector(TMR0_IRQn, TMR0_IRQHandler_CP);
	MXC_TMR_EnableInt(PWM_TIMER);
	MXC_TMR_Start(PWM_TIMER);

	/* Configure timer for measuring Low value of feedback PWM */
	MXC_TMR_Shutdown(PWM_LOW_TIMER);

	tmr1.pres    = TMR_PRES_1;
	tmr1.mode    = TMR_MODE_CONTINUOUS;
	tmr1.cmp_cnt = periodTicks / 2; // measure at 50% of PWM period
	tmr1.pol     = 1;

	MXC_TMR_Init(PWM_LOW_TIMER, &tmr1, FALSE);
	MXC_NVIC_SetVector(TMR1_IRQn, TMR1_IRQHandler_CP);
	MXC_TMR_EnableInt(PWM_LOW_TIMER);
}

/**
 * @brief Set pilot CP analog read interrupts
 * @param desc - NVIC IRQ controller description
 * @param stout - the stout device structure
 * @return 0 in case of success, error code otherwise
 */
int pilot_interrupts_setup(struct no_os_irq_ctrl_desc *desc,
			   struct stout *stout)
{
	int ret;

	ret = no_os_irq_enable(desc, TMR0_IRQn);
	if (ret)
		return ret;

	ret = no_os_irq_enable(desc, TMR1_IRQn);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Set PMW duty cycle
 * @param stout - state  machine descriptor
 * @param duty_cycle - duty cycle of output PWM times 10
 * @return none
 */
void pilot_pwm_timer_set_duty_cycle(struct stout *stout,
				    unsigned int duty_cycle)
{
	// period and initial duty cycle of output PWM
	unsigned int periodTicks = PeripheralClock / FREQ;
	unsigned int dutyTicks = periodTicks * duty_cycle / 1000;

	if (duty_cycle == PWM_DC) {
		stout->ac_dc = 1;
		stout->i_limit = I_LIMIT_NOT_CHG;
	} else {
		stout->ac_dc = 0;
		if (duty_cycle == PWM_DUTY_10A)
			stout->i_limit = I_LIMIT_10A;
		else
			stout->i_limit = I_LIMIT_16A;
	}

	MXC_TMR_SetPWM(PWM_TIMER, dutyTicks);
}

/*! Setup PWM voltage CP analog ADC */
/**
 * @brief Set PMW duty cycle
 *
 * @return 0 in case of success, error code otherwise
 */
int pilot_setup_adc(void)
{
	int ret;

	MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_GPIO2);

	/* Initialize ADC */
	ret = MXC_ADC_Init();
	if (ret != E_NO_ERROR) {
		pr_debug("Error on inintializing ADC\n");
		return ret;
	}

	MXC_ADC_SetExtScale(MXC_ADC_SCALE_2);

	MXC_GPIO_Config(&gpio_cfg_adc_ain0);

	MXC_ADC_StartConversion(ADC_CHANNEL);

	return 0;
}

/**
 * @brief Read PWM voltage value
 *
 * @return ADC value
 */
unsigned int pilot_read_val(void)
{
	//clear ADC done interrupt flag
	MXC_ADC_RevA_ClearFlags(MXC_ADC, MXC_F_ADC_REVA_INTR_DONE_IF);

	//set start bit
	MXC_ADC->ctrl |= MXC_F_ADC_REVA_CTRL_START;

	return E_NO_ERROR;
}

/**
 * @brief Compute ADC voltage value
 * @param value - rad ADC value
 * @return ADC value in mV
 */
uint32_t pilot_convert_to_mv(uint32_t value)
{
	return ((value * ADC_VREF * MXC_ADC_SCALE_2) / 1023);
}

/**
 * @brief Compute PWM value average
 * @param high - 1 - HIGH PWM value
 * 	       - 0 - LOW PWM value
 * @return average value comouted from last read values
 */
uint32_t pilot_compute_average(uint8_t high)
{
	uint32_t avg = 0;
	unsigned int length = 10;
	unsigned int i;

	for (i = 0; i < length; i++)
		if (high)
			avg += pwm_high[i];
		else
			avg += pwm_low[i];

	avg /= length;

	return pilot_convert_to_mv(avg);
}

/**
 * @brief Update PWM values in the state machine structure
 * @param stout - state  machine descriptor
 * @return ADC value in mV
 */
int pilot_write_new_values(struct stout *stout)
{
	// Compute PWM LOW and PWM HIGH averages
	stout->pwm_high_val = pilot_compute_average(1);
	stout->pwm_low_val = pilot_compute_average(0);

	return 0;
}

/**
 * @brief Update PWM values
 *
 * @return none
 */
void pilot_update_vals(void)
{
	for (unsigned int i = 9; i > 0; i--) {
		pwm_high[i] = pwm_high[i-1];
	}
	pwm_high[0] = pwm_high_val;

	for (unsigned int i = 9; i > 0; i--) {
		pwm_low[i] = pwm_low[i-1];
	}
	pwm_low[0] = pwm_low_val;
}

/*! Get milliseconds count */
uint32_t get_count_ms(void)
{
	return count_ms;
}

/*! Reset milliseconds count */
void reset_count_ms(void)
{
	count_ms = 0;
}
