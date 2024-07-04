/***************************************************************************//**
 *   @file   self_test.c
 *   @brief  Relay implementation file.
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
#include "common_data.h"
#include "no_os_delay.h"
#include "parameters.h"
#include "no_os_util.h"
#include "self_test.h"
#include "interface.h"
#if defined(REV_D)
#include "inter.h"
#endif
#include "supply.h"
#include "pilot.h"
#include "relay.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/******************************************************************************/
/********************************** Variables *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Functions Definitions ****************************/
/******************************************************************************/
/**
 * @brief Startup test for power supply
 * @param stout - state  machine descriptor
 * @param rms_adc_values - structure holding the measurements values
 * @return 0 in case of success, error code otherwise
 */
int self_test_supply(struct stout *stout, struct rms_adc_values *rms_adc_values)
{
	uint8_t r_state = 1;
	//used to count the cycles of the input signal
	uint32_t cnt = 0;
	int ret;

	// Test Vin value within limits
	// Skip a number of cycles
	while (SELF_TEST_SKIP_CYCLES_NO >= cnt) {
		while(!get_zero_cross_flag_state());
		reset_zero_cross_flag_state();
		cnt++;
	}
	cnt = 0;

	// Calculate rms and ADC values over a number of cycles defined by SELF_TEST_SKIP_CYCLES_NO
	while (SELF_TEST_SKIP_CYCLES_NO >= cnt) {
		while (!get_zero_cross_flag_state()) {
			ret = rms_adc_values_read(stout, rms_adc_values);
		}
		reset_zero_cross_flag_state();
		cnt++;
	}
	cnt = 0;
	pr_debug("TEST SUPPLY: Vin %d mV \n", rms_adc_values->v1_rms);
	if (INTF_INPUT_V_ERR_U != stout->err_status) {
		if ((VIN_LOW_LIMIT < rms_adc_values->v1_rms)
		    && (VIN_HIGH_LIMIT > rms_adc_values->v1_rms))
			// If grid voltage is 230 than variable grid = 0;
			stout->grid = 0;
		else if ((VIN_LOW_LIMIT_2 < rms_adc_values->v1_rms)
			 || (VIN_HIGH_LIMIT_2 > rms_adc_values->v1_rms))
			// If grid voltage is 120 than variable grid = 1;
			stout->grid = 1;
	}
	if (stout->grid >= 1) {
		if ((VIN_LOW_LIMIT_2 > rms_adc_values->v1_rms)
		    || (VIN_HIGH_LIMIT_2 < rms_adc_values->v1_rms))
			goto error;
	} else if ((VIN_LOW_LIMIT > rms_adc_values->v1_rms)
		   || (VIN_HIGH_LIMIT < rms_adc_values->v1_rms))
		goto error;
	// Test relay close value
	pr_debug("TEST SUPPLY: Relay open \n");
	ret = relay_close(stout->relay);
	if (ret)
		goto error1;
	pr_debug("PASSED \n");
	// Allow time for relay to switch
	no_os_mdelay(DELAY_SELF_TEST_READING);
	// Test Vin value within limits
	// Skip SELF_TEST_SKIP_CYCLES_NO periods
	while(!get_zero_cross_flag_state());
	reset_zero_cross_flag_state();
	// Calculate rms and ADC values
#if defined(REV_A)
	while (SKIP_CYCLES_AFTER_RELAY_SW >= cnt) {
		while (!get_zero_cross_flag_state()) {
			ret = rms_adc_values_read(stout, rms_adc_values);
		}
		reset_zero_cross_flag_state();
		cnt++;
	}
#elif defined(REV_D)
	while (SELF_TEST_SKIP_CYCLES_NO >= cnt) {
		while (!get_zero_cross_flag_state()) {
			ret = rms_adc_values_read(stout, rms_adc_values);
		}
		reset_zero_cross_flag_state();
		cnt++;
	}
#endif
	cnt = 0;
	ret = self_test_relay_closed(stout, rms_adc_values->v2_rms);
	if (ret)
		goto error1;
	ret = relay_open(stout->relay);
	if (ret)
		goto error1;
	no_os_mdelay(DELAY_SELF_TEST_READING);
	// Test Vin value within limits
	// Skip SELF_TEST_SKIP_CYCLES_NO periods
	while(!get_zero_cross_flag_state());
	reset_zero_cross_flag_state();
	// Calculate rms and ADC values
	while (SELF_TEST_SKIP_CYCLES_NO >= cnt) {
		while (!get_zero_cross_flag_state()) {
			ret = rms_adc_values_read(stout, rms_adc_values);
		}
		reset_zero_cross_flag_state();
		cnt++;
	}
	cnt = 0;
	// Test V relay value within limits
	pr_debug("TEST SUPPLY: Relay close->open \n");
	ret = relay_state(stout->relay, &r_state);
	if (ret)
		goto error1;
	if (!r_state) {
#if defined(REV_A)
		if (VR_OPEN_LOW_LIMIT > rms_adc_values->v2_rms) {
			pr_debug("Relay Soldered %d, %d \n", rms_adc_values->v2_rms,
				 rms_adc_values->v1_rms);
			goto error1;
		}
#elif defined(REV_D)
		if (stout->grid >= 1)
			if  ((VIN_LOW_LIMIT_2 - VR_OPEN_LOW_LIMIT) < rms_adc_values->v2_rms) {
				pr_debug("Relay Soldered %d, %d \n", rms_adc_values->v2_rms,
					 rms_adc_values->v1_rms);
				goto error1;
			} else if ((VIN_LOW_LIMIT - VR_OPEN_LOW_LIMIT < rms_adc_values->v2_rms)) {
				pr_debug("Relay Soldered %d, %d \n", rms_adc_values->v2_rms,
					 rms_adc_values->v1_rms);
				goto error1;
			}
#endif
	} else
		goto error1;
	pr_debug("PASSED \n");
	pr_debug("TEST SUPPLY: Passed\n");
	return 0;
error:
	ret = INTF_INPUT_V_ERR;
error1:
	ret = INTF_RELAY_ERR;
	return ret;
}

/**
 * @brief Startup test for RCD
 * @param stout - state  machine descriptor
 * @return 0 in case of success, error code otherwise
 */
int self_test_rcd(struct stout *stout)
{
	uint8_t val_rcddc = 0;
	uint8_t val_rcdac = 0;
	int ret;

	// Disable RCD interrupts
	ret = no_os_irq_disable(stout->ade9113->irq_ctrl, GPIO_RCDAC_PIN);
	if (ret)
		return ret;
	ret = no_os_irq_disable(stout->ade9113->irq_ctrl, GPIO_RCDDC_PIN);
	if (ret)
		return ret;

	// Hold RCM test pin 0 for minimum of 40ms
	ret = no_os_gpio_set_value(stout->gpio_rcm_test, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_mdelay(60);

	ret = no_os_gpio_set_value(stout->gpio_rcm_test, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	// Wait for 700 ms and sample rcddc and rcdac pins (we should have rcddc 1 and rcdac 0)
	no_os_mdelay(700);
	ret = no_os_gpio_get_value(stout->gpio_rcddc, &val_rcddc);
	if (ret)
		return ret;
	ret = no_os_gpio_get_value(stout->gpio_rcdac, &val_rcdac);
	if (ret)
		return ret;

	if ((val_rcddc == NO_OS_GPIO_LOW) || (val_rcdac == NO_OS_GPIO_HIGH))
		goto error;

	// wait for another 700 ms, we should have both values 1
	no_os_mdelay(700);
	ret = no_os_gpio_get_value(stout->gpio_rcddc, &val_rcddc);
	if (ret)
		return ret;
	ret = no_os_gpio_get_value(stout->gpio_rcdac, &val_rcdac);
	if (ret)
		return ret;

	if ((val_rcddc == NO_OS_GPIO_LOW) || (val_rcdac == NO_OS_GPIO_LOW))
		goto error;

	// Wait for another 600 ms, we should have both values 0
	no_os_mdelay(720);
	ret = no_os_gpio_get_value(stout->gpio_rcddc, &val_rcddc);
	if (ret)
		return ret;
	ret = no_os_gpio_get_value(stout->gpio_rcdac, &val_rcdac);
	if (ret)
		return ret;

	if ((val_rcddc == NO_OS_GPIO_HIGH) || (val_rcdac == NO_OS_GPIO_HIGH))
		goto error;

	// Enable RCD interrupts
	ret = no_os_irq_enable(stout->ade9113->irq_ctrl, GPIO_RCDAC_PIN);
	if (ret)
		return ret;
	ret = no_os_irq_enable(stout->ade9113->irq_ctrl, GPIO_RCDDC_PIN);
	if (ret)
		return ret;

	pr_debug("TEST RCD: Passed\n");
	return 0;
error:
	ret = INTF_INIT_RCD_ERROR;
	return ret;
}

/**
 * @brief Running test for RCD
 * @param stout - state  machine descriptor
 * @param step - zero crossing count (used for counting 20ms periods for time tracking)
 * @param event - test trigger and test complete indicator
 * @return 0 in case of success, error code otherwise
 */
int self_test_rcd_running(struct stout *stout, uint16_t step,
			  enum state_machine_events_e *event)
{
	uint8_t val_rcddc = 0;
	uint8_t val_rcdac = 0;
	int ret;

	*event = S_M_RCD_TEST_TRIGGERED;

	if (0 == step) {
		pr_debug("RCD test step 1 - RCM pin low\n");
		// Disable RCD interrupts
		ret = no_os_irq_disable(stout->ade9113->irq_ctrl, GPIO_RCDAC_PIN);
		if (ret)
			return ret;
		ret = no_os_irq_disable(stout->ade9113->irq_ctrl, GPIO_RCDDC_PIN);
		if (ret)
			return ret;

		// Hold RCM test pin 0 for minimum of 40ms
		ret = no_os_gpio_set_value(stout->gpio_rcm_test, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
	}

	// Set RCD test pin back to 1 after 60 ms
	if (RESTORE_TEST_PIN_STEP == step) {
		pr_debug("RCD test step 2 - restore test pin H\n");
		ret = no_os_gpio_set_value(stout->gpio_rcm_test, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
	}

	// Wait for 700 ms and sample rcddc and rcdac pins (we should have rcddc 1 and rcdac 0)
	if (SAMPLE_PINS_STEP_1 == step) {
		pr_debug("RCD test step 3 - test DC out\n");
		ret = no_os_gpio_get_value(stout->gpio_rcddc, &val_rcddc);
		if (ret)
			return ret;
		ret = no_os_gpio_get_value(stout->gpio_rcdac, &val_rcdac);
		if (ret)
			return ret;

		if ((val_rcddc == NO_OS_GPIO_LOW) || (val_rcdac == NO_OS_GPIO_HIGH))
			goto error;
	}

	// wait for another 700 ms, we should have both values 1
	if (SAMPLE_PINS_STEP_2 == step) {
		pr_debug("RCD test step 4 - test RMS out\n");
		ret = no_os_gpio_get_value(stout->gpio_rcddc, &val_rcddc);
		if (ret)
			return ret;
		ret = no_os_gpio_get_value(stout->gpio_rcdac, &val_rcdac);
		if (ret)
			return ret;

		if ((val_rcddc == NO_OS_GPIO_LOW) || (val_rcdac == NO_OS_GPIO_LOW))
			goto error;
	}

	// Wait for another 600 ms (at least), we should have both values 0
	if (SAMPLE_PINS_STEP_3 == step) {
		pr_debug("RCD test step 5 - both outputs low\n");
		ret = no_os_gpio_get_value(stout->gpio_rcddc, &val_rcddc);
		if (ret)
			return ret;
		ret = no_os_gpio_get_value(stout->gpio_rcdac, &val_rcdac);
		if (ret)
			return ret;

		if ((val_rcddc == NO_OS_GPIO_HIGH) || (val_rcdac == NO_OS_GPIO_HIGH))
			goto error;

		// Enable RCD interrupts
		ret = no_os_irq_enable(stout->ade9113->irq_ctrl, GPIO_RCDAC_PIN);
		if (ret)
			return ret;
		ret = no_os_irq_enable(stout->ade9113->irq_ctrl, GPIO_RCDDC_PIN);
		if (ret)
			return ret;

		*event = S_M_RCD_TEST_COMPLETED;
		reset_rcd_flag_state();
		pr_debug("TEST RCD: Passed\n");
	}

	return 0;
error:
	*event = S_M_RCD_TEST_FAIL;
	ret = INTF_INIT_RCD_ERROR;
	return ret;
}

/**
 * @brief Running test for relay
 * @param stout - state  machine descriptor
 * @param v2 - value
 * @return 0 in case of success, error code otherwise
 */
int self_test_relay_closed(struct stout *stout, int32_t v2)
{
	uint8_t r_state;
	int ret;

	// Test V relay value within limits
	pr_debug("TEST SUPPLY: Relay open->close\n");
	ret = relay_state(stout->relay, &r_state);
	if (ret)
		return ret;

	if (r_state) {
#if defined(REV_A)
		if (V2_CLOSED_LIMIT < v2) {
			pr_debug("Relay error Vrel: %d \n", v2);
			return INTF_RELAY_ERR;
		}
#elif defined(REV_D)
		if (stout->grid >= 1) {
			if ((VIN_LOW_LIMIT_2 > v2) || (VIN_HIGH_LIMIT_2 < v2))
				return INTF_RELAY_ERR;
		} else if ((VIN_LOW_LIMIT > v2) || (VIN_HIGH_LIMIT < v2))
			return INTF_RELAY_ERR;
#endif
	}
	pr_debug("PASSED \n");

	return INTF_NO_ERR;
}

/**
 * @brief Startup test for pilot PWM
 * @param stout - state  machine descriptor
 * @return 0 in case of success, error code otherwise
 */
static int self_test_pilot(struct stout *stout)
{
	// TEST CP - check if High value is 1.8 and low value 0
	pr_debug("TEST PILOT: CP values %d mV %d mV\n",
		 pilot_convert_to_mv(get_pwm_high_val()),
		 pilot_convert_to_mv(get_pwm_low_val()));
	// test only the high portion of the CP
	if (((PWM_HIGH_TEST_VAL) > (pilot_convert_to_mv(get_pwm_high_val()))) &&
	    ((PWM_HIGH_TEST_VAL_EV_CONNECTED) < pilot_convert_to_mv(get_pwm_high_val()))) {
		// If the CP value coresponds to state A or state B value then no error
		pr_debug("TEST PILOT: Passed\n");
		return INTF_NO_ERR;
	} else
		return INTF_PILOT_ERROR;

	return 0;
}

#if defined(REV_D)
/**
 * @brief Startup test for PE upstream presence
 * @param stout - state  machine descriptor
 * @return 0 in case of success, error code otherwise
 */
int self_test_pe_upstream(struct stout *stout)
{
	pr_debug("TEST upstream PE. opto1 = %d, opto2 = %d \n",
		 get_gpio_opto_out1_flag_state(), get_gpio_opto_out2_flag_state());

	if (!(get_gpio_opto_out1_flag_state() ^ get_gpio_opto_out2_flag_state()))
		return INTF_PE_UPSTREAM_ERR;

	pr_debug("PE upstream presence: Passed\n");

	return INTF_NO_ERR;
}
#endif

/**
 * @brief Startup test
 * @param stout - state  machine descriptor
 * @param rms_adc_values - structure holding the measurements values
 * @return 0 in case of success, error code otherwise
 */
int self_test_startup(struct stout *stout,
		      struct rms_adc_values *rms_adc_values)
{
	int ret;
	//self test power supply

	ret = self_test_supply(stout, rms_adc_values);
	if (ret)
		goto error;
#if defined(REV_D)
	//Self test the PE upstream
	ret = self_test_pe_upstream(stout);
	if (ret)
		goto error;
#endif
	// Start the CP signal, so we can test it
	// Test only the high portion of the CP
	pilot_pwm_timer_set_duty_cycle(stout, PWM_DC);
	no_os_mdelay(200);
	ret = self_test_rcd(stout);
	if (ret)
		goto error;

	ret = self_test_pilot(stout);
	if (ret)
		goto error;

	pr_debug("TEST: Self-test passed\n\n");
	return 0;
error:
	pr_debug("TEST: Self-test failed\n");
	return ret;
}

/**
 * @brief Relay test
 * @param stout - state  machine descriptor
 * @return 0 in case of success, error code otherwise
 */
int self_test_relay_stuck(struct stout *stout)
{
	uint8_t r_state = 1;
	int ret;

	ret = relay_state(stout->relay, &r_state);
	if (ret)
		goto error;

	if (!r_state) {
#if defined(REV_A)
		if (stout->v2_val < VR_OPEN_LOW_LIMIT) {
			pr_debug("Error: Relay contacts soldered %d \n", stout->v2_val);
			goto error;
		}
#elif defined(REV_D)
		if (stout->grid >= 1)
			if  ((VIN_LOW_LIMIT_2 - VR_OPEN_LOW_LIMIT) < stout->v2_val) {
				pr_debug("Error: Relay contacts soldered %d \n", stout->v2_val);
				goto error;
			} else if ((VIN_LOW_LIMIT - VR_OPEN_LOW_LIMIT) < stout->v2_val) {
				pr_debug("Error: Relay contacts soldered %d \n", stout->v2_val);
				goto error;
			}
#endif
	} else
		goto error;
	return 0;
error:
	ret = INTF_RELAY_ERR;
	return ret;
}