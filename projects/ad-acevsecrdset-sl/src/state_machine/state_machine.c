/***************************************************************************//**
 *   @file   state_machine.c
 *   @brief  STOUT state machine implementation file
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
#include "state_machine.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "common_data.h"
#include "no_os_error.h"
#include "self_test.h"
#include "interface.h"
#include "ade9113.h"
#include "supply.h"
#include "pilot.h"
#if defined(REV_D)
#include "inter.h"
#endif
#include "relay.h"
#include "gpio.h"
#include "rcd.h"

// Flag used for synchronizing state machine
uint16_t zcross_cnt;
// Charging state
uint8_t is_charging;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/**
 * @brief State machine main execution
 *
 * @return Result of the example execution. If working correctly, will
 *         execute continuously the while(1) loop.
 */
int state_machine()
{
	/**************************************************************************/
	/****************************Variables declaration*************************/
	/**************************************************************************/
	// Input voltage maximum value over one periode
	int32_t v1_max = 0;
	// Relay voltage maximum value over one periode
	int32_t v2_max = 0;
	// The sume of the relay maximum values over 10 periodes used to compute the mean value
	int32_t v2_sum = 0;
	// The maximum current value over one periode
	int32_t i_max = 0;
	// intermediate value of the input voltage used for scaling
	int64_t v1 = 0;
	// intermediate value of the relay voltage used for scaling
	int64_t v2 = 0;
	// State machine events; default value S_M_NO_EVENT
	enum state_machine_events_e event = S_M_NO_EVENT;
	// PWM duty cycle value dependent on the output current set value
	int32_t current_set = PWM_DUTY_16A;
	// PWM duty cycle value set based on the temperature value
	int32_t current_value_limit = PWM_DUTY_16A;
	// Variable used for skipping RCD test in intervals smaller than a specified threshold
	uint8_t skip_test = 0;
	// Variable used for skipping values print in intervals smaller than a specified threshold
	uint8_t print_values = 0;
	// Variable used for skipping print charging in intervals smaller than a specified threshold
	uint8_t print_charging = 0;
	// Variable used for skipping the repeated execution of RCD test steps
	uint8_t count_rcd_state = 0;
	// Curent time value read from RTC
	struct no_os_time current_time = no_os_get_time();
	// The time interval in which the RCD test is disabled
	uint32_t rcd_test_s = current_time.s;
	// The time interval in which the print values is disabled
	uint32_t print_values_s = current_time.s;
	// The time passed since the last RCD test
	uint32_t s_elapsed_since_rcd_test = 0;
	// The time passed since the last print values
	uint32_t s_elapsed_since_print_values = 0;
	// Time when values were interpreted
	uint32_t recalc_time_us = current_time.us;
	// Time passed from last values update
	uint32_t us_elapsed_since_recalc = 0;
	// Variable indicating multiple of 20ms used to compute the ADE9113 values on multiple periods
	uint8_t multiple_20ms = 0;
	// Variable indicating a multiple of the periodes used to compute ADE9113 values.
	// Used to compute the Vrelay value
	uint8_t multiple_20ms_2 = 0;
	// Variable used for reading the ADT75 at an inteval multiple of 20ms
	uint8_t multiple_20ms_adt75 = 0;
	// Variable used to indicate the the Vrelay is available and can be used in the state machine logic
	uint8_t v2_ready = 0;
	// Variable used to indicate that the relay stuck needs to be tested and the Vrelay needs to be computed
	uint8_t v2_read = 0;
	// Flag for updating supply vaues
	uint8_t update_ade9113_values = 0;
	// Used for led blinking during charging
	uint8_t cnt_disp = 0;
	// Variables used to read and compute the temperature from ADT75
	int32_t adt75_value = 0;
	// Variable used for the values read continuously from ADE9113
	int32_t i_val = 0, v1_val = 0, v2_val = 0;
	// Variables used to read the RCD status
	uint8_t val_rcddc = 0;
	uint8_t val_rcdac = 0;
	// Pointer to the state machine structure
	struct stout *stout;
	// Pointer to the adt75_desc structure
	struct adt75_desc *adt75_desc;
	int ret = -22;
	/**************************************************************************/
	/**********************End of variables declaration************************/
	/**************************************************************************/

	/**************************************************************************/
	/****************INITALIZATION FOR THE STATE MACHINE***********************/
	/**************************************************************************/
	/* Clear the screen. */
	printf("%c%c%c%c", 27, '[', '2', 'J');
	no_os_mdelay(5);
	pr_debug("\nSTOUT app FIRMWARE VERSION: %s\n",FIRMWARE_VERSION);
	/*Board revision*/
#if defined(REV_A)
	pr_debug("STOUT app BOARD REVISION A\n\n");
#elif defined(REV_D)
	pr_debug("STOUT app BOARD REVISION D\n\n");
#endif

	/* Allocate memory for application structure */
	stout = (struct stout *)no_os_calloc(1, sizeof(*stout));
	if (!stout)
		return -ENOMEM;

	/* Initialize I2C adt75*/
	ret = adt75_init(&adt75_desc, &adt75_ip);
	if (ret) {
		no_os_free(stout);
		return ret;
	}

	/* Initialize LEDs and buttons */
	ret = interface_init(&stout->gpio_led[0]);
	if (ret) {
		no_os_free(stout);
		adt75_remove(adt75_desc);
		return ret;
	}

	/* Initialize power supply monitoring */
	ret = supply_init(&stout->ade9113);
	if (ret)
		goto error;

	/* Initialize relay */
	ret = relay_init(&stout->relay);
	if (ret)
		goto error;

	/* Initialize RCD monitoring */
	ret = rcd_init(stout->ade9113->irq_ctrl, &stout->gpio_rcm_test,
		       &stout->gpio_rcddc, &stout->gpio_rcdac);
	if (ret)
		goto error;

	/* Initialize pilot functions */
	ret = pilot_init(stout);
	if (ret)
		goto error;

#if defined(REV_D)
	/* Initialize GPIO2_3 interrupt */
	ret = inter_init(stout);
	if (ret)
		goto error;
#endif

	/* Open relay */
	ret = relay_open(stout->relay);
	if (ret)
		goto error;
	reset_relay_status();

	/* Clear the zero crossing flag */
	reset_zero_cross_flag_state();

	/* Enable data ready interrupt */
	ret = ade9113_drdy_int_enable(stout->ade9113);
	if (ret)
		goto error;

	// Current limit not charging
	stout->i_limit_not_ch = I_LIMIT_NOT_CHG;
	/**************************************************************************/
	/****************END OF INITALIZATION FOR THE STATE MACHINE****************/
	/**************************************************************************/

	/**************************************************************************/
	/****************************Start-up Test*********************************/
	/**************************************************************************/
	/* Run startup test */
	ret = self_test_startup(stout);

	if (ret == INTF_INPUT_V_ERR) {
		interface_disp(stout);
		event = S_M_UNDERVOLTAGE;
		if (stout->grid >= 1 ) {
			if (VIN_LOW_LIMIT_2 > v1_max)
				ret = INTF_INPUT_V_ERR_U;
			else if (VIN_HIGH_LIMIT_2 < v1_max) {
				ret = INTF_INPUT_V_ERR_O;
				stout->current_state = STATE_FAULT;
				stout->err_status = ret;
				// Stop the CP PWM signal
				pilot_pwm_timer_set_duty_cycle(stout, PWM_OFF);
				goto error;
			}
		} else if (VIN_LOW_LIMIT > v1_max)
			ret = INTF_INPUT_V_ERR_U;
		else if (VIN_HIGH_LIMIT < v1_max) {
			ret = INTF_INPUT_V_ERR_O;
			stout->current_state = STATE_FAULT;
			stout->err_status = ret;
			// Stop the CP PWM signal
			pilot_pwm_timer_set_duty_cycle(stout, PWM_OFF);
			goto error;
		}
		stout->err_status = ret;
		// Wait until no undervoltage
		do {
			ret = self_test_supply(stout);
		} while (ret == INTF_INPUT_V_ERR_U);
		// Run tests again if voltage back to normal
		ret = self_test_startup(stout);
	}
	if (ret) {
		stout->current_state = STATE_FAULT;
		stout->err_status = ret;
		// Stop the CP PWM signal
		pilot_pwm_timer_set_duty_cycle(stout, PWM_OFF);
		goto error;
	}

	// Skip one period
	while(!get_zero_cross_flag_state());
	reset_zero_cross_flag_state();

	// Disable the zero corssing interrupt
	ret = no_os_irq_disable(stout->ade9113->irq_ctrl, GPIO_ZC_PIN);
	if (ret) {
		stout->current_state = STATE_FAULT;
		stout->err_status = ret;
		goto error;
	}

	// Set CP signal at DC level at startup
	pilot_pwm_timer_set_duty_cycle(stout, PWM_DC);

	// Start from POWER ON state
	reset_rcd_flag_state();
	stout->previous_state = STATE_RCD_SELF_TEST;
	stout->current_state = STATE_POWER_ON;
	stout->err_status = INTF_NO_ERR;
	event = S_M_INITIAL_CHECK_DONE;
	/*******************************************************************************************************/
	/************The main loop of the State Machine (runs continuously if no error detected)****************/
	/*******************************************************************************************************/
	while(1) {
		// Update PWM LOW and PWM HIGH values each time a new conversion takes place
		if (get_pwm_low_flag_state()) {
			// Update values for computing PWM LOW and PWM HIGH averages
			pilot_update_vals();
			// Compute PWM LOW and PWM HIGH averages
			pilot_write_new_values(stout);
			// Get current time since device powered
			current_time = no_os_get_time();

			if (current_time.us < recalc_time_us)
				us_elapsed_since_recalc = current_time.us + 1000000 - recalc_time_us;
			else
				us_elapsed_since_recalc = current_time.us - recalc_time_us;

			if (VALUE_20MS < us_elapsed_since_recalc) {
				current_time = no_os_get_time();
				recalc_time_us = current_time.us;
				// The CP event is determined only if the the special cases are not present
				if ((event != S_M_CHECK_STUCK_RELAY) && (event != S_M_DIODE_ERR_CHECK)
				    && (event != S_M_RCD_TEST_TRIGGERED) && (event != S_M_UNDERVOLTAGE)) {
					event = state_machine_det_event_cp(stout, event);
				}
				// Flag for synchronizing state machine
				set_action_flag();
				update_ade9113_values = 1;
			}
			// Compute time elapsed since last RCD test
			s_elapsed_since_rcd_test = current_time.s - rcd_test_s;
			// If time elapsed larger than RCD_TIME_REPEAT_INTERVAL seconds, reenable test
			if ((RCD_TIME_REPEAT_INTERVAL < s_elapsed_since_rcd_test) && (skip_test == 1))
				skip_test = 0;
			// Compute time elapsed since last print values
			s_elapsed_since_print_values = current_time.s - print_values_s;
			// If time elapsed larger than PRINT_VALUES_TIME seconds, reenable print
			if ((PRINT_VALUES_TIME < s_elapsed_since_print_values) && (0 == print_values))
				print_values = 1;

			reset_pwm_low_flag_state();
		}

		// --------------------- COMPUTE ADE9113 VALUEAS AND EXTRACT MAXIMUM VALUES --------------
		ret = supply_conv_vals_to_mv(stout, &i_val, &v1_val, &v2_val);
		if (ret)
			goto error;

		v1 = supply_scale_v1(v1_val);
#if defined(REV_A)
		v2 = supply_scale_v2(v2_val);
#elif defined(REV_D)
		v2 = supply_scale_v1(v2_val);
#endif
		v1_max = no_os_max_t(int32_t, v1, v1_max);
		v2_max = no_os_max_t(int32_t, v2, v2_max);
		i_max = no_os_max_t(int32_t, i_val, i_max);
		// ----------------------------- END COMPUTE MAX -----------------------------------------

		//-----UPDATE THE VALUES OF THE STOUT STRUCTURE WITH THE VALUES COMPUTED FROM ADE9113-----
		if (update_ade9113_values == 1) {
			multiple_20ms++;
			multiple_20ms_adt75++;
			// Compute the values Vin and Iout each 20*COMPUTE_VALUES_INTERVAL
			if ((COMPUTE_VALUES_INTERVAL <= multiple_20ms)
			    && (COMPUTE_VRELAY_INTERVAL > multiple_20ms_2)) {
				// Update V1 and V2 maximum values
				stout->v1_max = v1_max;
				stout->i_val = i_max;
				if (1 == print_values) {
					pr_debug("Iout: %d mA, Vin: %d mV\n", stout->i_val, stout->v1_max);
					print_values_s = current_time.s;
					print_values = 0;
				}
				// The state machine runs every COMPUTE_VALUES_INTERVAL * 20ms
				event = state_machine_det_event_supply(stout, event);
				// Relay voltage is summed each 20 ms interval (first value is skipped)
				// Variable v2_sum will be used to compute the average of the relay maximum amplitude
				// over the COMPUTE_VREALY_INTERVAL
				if ((1 <= v2_read) && (1 <= multiple_20ms_2)) {
					v2_sum += v2_max;
				}
				// Reset V1, V2 and I maximum values
				v1_max = 0;
				i_max = 0;
				v2_max = 0;
				// Reset the 20ms variable
				multiple_20ms = 0;
				// Increment the counter for Vrelay compute time interval
				multiple_20ms_2++;
			} else if (COMPUTE_VRELAY_INTERVAL <= multiple_20ms_2) {
				// Compute Vrelay average value
				if (v2_read >= 1) {
					// The relay stuck detection state is active (a reading of Vrelay is needed)
					// Compute the average Vrelay value over the COMPUTE_VRELAY_INTERVAL
					stout->v2_max = v2_sum/(multiple_20ms_2-1);
					// reset the 20 ms counter to start with new values
					multiple_20ms = 0;
					// Indicates that the Vrealy value is available for the state machine
					v2_ready = 1;
				}
				// Reset flag for computing Vrelay
				multiple_20ms_2 = 0;
				// Reset Vrelay max value
				v2_max = 0;
				v1_max = 0;
				i_max = 0;
			}
			// Reset flag for updating values
			update_ade9113_values = 0;
		}

		//----------------------- END UPDATE VALUES COMPUTED FROM ADE913------------------------

		// ----------------------------- READ TEMPERATURE----------------------------------------
		if (TEMPERATURE_READ_RATE <= multiple_20ms_adt75) {
			ret = adt75_reg_read(adt75_desc, 0, &adt75_value);
			if (ret)
				goto error;

			adt75_value = no_os_field_get(ADT75_TEMP_MASK, adt75_value);
			stout->temperature = no_os_sign_extend32(adt75_value, ADT75_SIGN_BIT);
			stout->temperature *= MILLIDEGREE_PER_DEGREE / ADT75_TEMP_DIV;
			pr_debug("Temperature: %.03f C\n", (double)stout->temperature / 1000);

			// Run the state machine for temperature event
			event = state_machine_det_event_temperature(stout, event);

			// Reset the counter
			multiple_20ms_adt75 = 0;
		}
		// ----------------------------- END READ TEMPERATURE----------------------------------------

#if defined(REV_D)
		// ------------------------------ PE UPSTREAM ERROR -----------------------------------------
		// If PE interrupt triggere device has to be UNPLUGGED
		if (1 <= get_gpio_flag_state()) {
			// If PE fault go to error
			stout->previous_state = stout->current_state;
			stout->current_state = STATE_FAULT;
			stout->err_status = INTF_PE_UPSTREAM_ERR;
		}
		// -------------------------------- END RCD ACTIOM ---------------------------------------
#endif

		// -------------------------------- RCD ACTIOM ------------------------------------------
		// If RCD interrupt triggered, and it is not after RELAY OPEN, then device
		// has to be UNPLUGGED
		if (1 <= get_rcd_flag_state()) {
			// If RCD fault after opening relay, reset flag
			if (STATE_RELAY_OPEN == stout->previous_state) {
				reset_rcd_flag_state();
				stout->err_status = INTF_NO_ERR;
			} else {
				ret = no_os_gpio_get_value(stout->gpio_rcdac, &val_rcdac);
				if (ret)
					return ret;
				ret = no_os_gpio_get_value(stout->gpio_rcddc, &val_rcddc);
				if (ret)
					return ret;
				if ((val_rcddc == NO_OS_GPIO_HIGH) || (val_rcdac == NO_OS_GPIO_HIGH)) {
					stout->previous_state = stout->current_state;
					stout->current_state = STATE_FAULT;
					stout->err_status = INTF_RCD_ERROR;
				} else {
					reset_rcd_flag_state();
					stout->err_status = INTF_NO_ERR;
				}
			}
			stout->previous_state = stout->current_state;
		}
		// -------------------------------- END RCD ACTIOM ---------------------------------------

		// --------------------------------- STATE MACHINE ---------------------------------------
		if ((TAKE_S_M_ACTION == get_action())) {
			// Reset flag for running state machine
			reset_action_flag();

			switch (stout->current_state) {
			//------------THE STATE FOR OPENING THE RELAY CONTACTS------------------------
			case STATE_RELAY_OPEN:
				// Debug message
				if (stout->current_state != stout->previous_state) {
					pr_debug("STATE RELAY OPEN\n");
				}
				relay_open(stout->relay);
				reset_relay_status();
				stop_charging();
				event = S_M_STOP_CHARGING;
				// Reset the counter for delay between open and stuck relay check
				reset_count_ms();
				break;
			//--------------------------END STATE RELAY OPEN-------------------------------

			//-------------------STUCK RELAY CONTACTS CHECK STATE--------------------------
			case STATE_CHECK_RELAY:
				if (0 == v2_read) {
					// Reset the values for computing Vrelay
					v2_read = 1;
					multiple_20ms_2 = 0;
					v2_sum = 0;
					v2_max = 0;
					// EVSE not ready during the test
					pilot_pwm_timer_set_duty_cycle(stout, PWM_DC);
				}
				if ((S_M_CHECK_STUCK_RELAY == event) && (1 <= v2_ready)) {
					if (self_test_relay_stuck(stout)) {
						stout->current_state = STATE_FAULT;
						stout->err_status = INTF_RELAY_ERR;
						// Reset the Vrelay read flag
						v2_read = 0;
					} else {
						// Normal working conditions relay not stuck
						event = S_M_CHECK_RELAY_PASSED;
						// Resume EVSE ready
						pilot_pwm_timer_set_duty_cycle(stout, current_set);
						// Reset the Vrelay read flag
						v2_read = 0;
					}
					// Reset the Vrelay available value flag
					v2_ready = 0;
				}
				break;
			//--------------------------END STATE STUCK RELAY CHECK-------------------------------

			//-----------------------------RCD TEST STATE----------------------------------
			case STATE_RCD_SELF_TEST:
				// If the time passed from last check is less than RCD_TIME_REPEAT_INTERVAL then skip the test
				// else run the test
				if (1 == skip_test) {
					event = S_M_RCD_TEST_COMPLETED;
				} else if (0 == skip_test) {
					if (S_M_CHARGING_REQ == event) {
						// Reset the counter used to cover the RCD test
						count_rcd_state = 0;
						ret = self_test_rcd_running(stout, 0, &event);
						if (ret) {
							event = S_M_RCD_TEST_FAIL;
							stout->current_state = STATE_FAULT;
							stout->err_status = INTF_RCD_ERROR;
						}
						// Reset values to disable RCD test for the time specified by RCD_TIME_REPEAT_INTERVAL
						current_time = no_os_get_time();
						rcd_test_s = current_time.s;
						reset_count_ms();
					} else if (S_M_RCD_TEST_TRIGGERED == event) {
						// Run the last step of the RCD test if the other steps passed and the timing is right
						if ((SAMPLE_PINS_STEP_3 * 20 <= get_count_ms()) && (3 == count_rcd_state)) {
							ret = self_test_rcd_running(stout, SAMPLE_PINS_STEP_3, &event);
							if (ret) {
								event = S_M_RCD_TEST_FAIL;
								stout->current_state = STATE_FAULT;
								stout->err_status = INTF_RCD_ERROR;
							}
							// Set the flag that indicates that an RCD test was succesfuly completed
							// (used to skip the RCD test for RCD_TIME_REPEAT_INTERVAL )
							skip_test = 1;
							// Increment the RCD state counter
							count_rcd_state++;
						} else if ((SAMPLE_PINS_STEP_2 * 20 <= get_count_ms())
							   && (2 == count_rcd_state)) {
							// Should wait additional 700ms
							// Run the third step of the RCD test if the previouse steps passed and the timing is right
							ret = self_test_rcd_running(stout, SAMPLE_PINS_STEP_2, &event);
							if (ret) {
								event = S_M_RCD_TEST_FAIL;
								stout->current_state = STATE_FAULT;
								stout->err_status = INTF_RCD_ERROR;
							}
							// Increment the RCD state counter (go to nex step if step passed)
							count_rcd_state++;
						} else if ((SAMPLE_PINS_STEP_1 * 20 <= get_count_ms())
							   && (1 == count_rcd_state)) {
							// Should wait additional 700ms
							// Run the second step of the RCD test if the previouse steps passed and the timing is right
							ret = self_test_rcd_running(stout, SAMPLE_PINS_STEP_1, &event);
							if (ret) {
								event = S_M_RCD_TEST_FAIL;
								stout->current_state = STATE_FAULT;
								stout->err_status = INTF_RCD_ERROR;
							}
							// Increment the RCD state counter (go to nex step if step passed)
							count_rcd_state++;
						} else if ((RESTORE_TEST_PIN_STEP * 20 <= get_count_ms())
							   && (0 == count_rcd_state)) {
							// Should wait 60ms
							// Run the first step of the RCD test if the the timing is right
							ret = self_test_rcd_running(stout, RESTORE_TEST_PIN_STEP, &event);
							if (ret) {
								event = S_M_RCD_TEST_FAIL;
								stout->current_state = STATE_FAULT;
								stout->err_status = INTF_RCD_ERROR;
							}
							// Increment the RCD state counter (go to nex step if step passed)
							count_rcd_state++;
						}
					}
				}
				if (S_M_RCD_TEST_FAIL == event) {
					stout->current_state = STATE_FAULT;
					stout->err_status = INTF_RCD_ERROR;
				}
				break;
			//--------------------------END RCD TEST STATE-------------------------------

			//-----------------------------POWER ON STATE--------------------------------
			case STATE_POWER_ON:
				if (stout->current_state != stout->previous_state) {
					// Debug message
					pr_debug("STATE POWER ON\n");
				}
				reset_count_ms();
				break;
			//-----------------------------END POWER ON STATE-----------------------------

			//-----------------------------------STATE A----------------------------------
			// In this state the EV is not connected and the CP is DC level. It can be considered the IDLE state
			case STATE_A:
				if (stout->current_state != stout->previous_state) {
					// Debug message
					pr_debug("STATE A\n");
					// The CP is set to DC
					pilot_pwm_timer_set_duty_cycle(stout, PWM_DC);
				}
				// Cable disconnected while charging
				if ((S_M_CHARGING == event) || (S_M_CHARGING_D == event)) {
					// Open the relay
					relay_open(stout->relay);
					reset_relay_status();
					stop_charging();
				} else if (S_M_WAIT == event) {
					// Display on the LEDs that the EVSE is in state A
					interface_disp(stout);
				}
				break;
			//---------------------------------END STATE A---------------------------------

			//-----------------------------------STATE B-----------------------------------
			// In this state the EV is connected to the EVSE. If EVSE ready the CP signal will change to PWM
			// Can be considered an IDLE state with the EV connected if we reach here from a charging state.
			case STATE_B:
				if (S_M_EVSE_READY == event) {
					pr_debug("START THE PWM\n");
					// The duty cycle is set based on the Iout selected value (for this device can be 10A or 16A)
					pilot_pwm_timer_set_duty_cycle(stout, current_set);
				} else if (S_M_WAIT_B == event) {
					// Display on the LEDs that the EVSE is in state B
					interface_disp(stout);
				}
				break;
			//---------------------------------END STATE B---------------------------------

			//-----------------------------------STATE C-----------------------------------
			// In this state the EVSE is ready, the EV is connected and a charging session was initiated by the EV
			case STATE_C:
				// Debug message
				if (stout->current_state != stout->previous_state) {
					current_set = current_value_limit;
					// Set the PWM value based on Iout value
					pilot_pwm_timer_set_duty_cycle(stout, current_set);
					print_charging = 1;
				}
				// If overtemperature detected limit the current to 10A else the current is 16A
				if (S_M_OVER_TEMPERATURE_1 == event) {
					current_value_limit = PWM_DUTY_10A;
					if (1 == print_charging || current_set != current_value_limit) {
						pilot_pwm_timer_set_duty_cycle(stout, current_value_limit);
						current_set = current_value_limit;
						pr_debug("STATE C LIMIT CURRENT 10A\n");
						print_charging = 0;
					}
				} else {
					current_value_limit = PWM_DUTY_16A;
					if (1 == print_charging || current_set != current_value_limit) {
						pilot_pwm_timer_set_duty_cycle(stout, current_value_limit);
						current_set = current_value_limit;
						pr_debug("STATE C 16A\n");
						print_charging = 0;
					}
				}

				// Check the diode before charging
				if (S_M_DIODE_ERR_CHECK == event) {
					// Wait for 20 ms
					if (DIODE_ERR_DELAY <= get_count_ms()) {
						if (CP_DIODE_ERROR_LIMIT < stout->pwm_low_val) {
							event = S_M_DIODE_ERR;
							pr_debug("Diode error found while initiating charging session \n");
						} else {
							event = S_M_DIODE_CHECK_DONE;
							pr_debug("Diode OK \n");
						}
					}
				} else if (S_M_CHARGING_START == event) {
					pr_debug("CLOSE THE RELAY STATE C\n");
					// Close the relay
					ret = relay_close(stout->relay);
					if (ret)
						goto error;
					start_charging();
					set_relay_status();
					// Reset the counter used for led blinking during charging
					cnt_disp = 0;
				} else if ((S_M_CHARGING == event) && (LED_BLINKING_16A <= cnt_disp)) {
					// The LED will blink with a higher frequency if Iout is 16A and with a lower one if Iout is 10A
					interface_disp(stout);
					cnt_disp = 0;
				} else if ((S_M_OVER_TEMPERATURE_1 == event)
					   && (LED_BLINKING_10A <= cnt_disp)) {
					interface_disp(stout);
					cnt_disp = 0;
				}
				// Increment the counter used for LED blinking
				cnt_disp++;
				break;
			//---------------------------------END STATE C---------------------------------

			//-----------------------------------STATE D-----------------------------------
			// In this state the EVSE is ready, the EV is connected and a charging session with ventilation required
			// was initiated by the EV
			case STATE_D:
				if (stout->current_state != stout->previous_state) {
					pr_debug("State D 10A \n");
					// Set the CP PWM duty cycle based on the Iout limit
					current_set = current_value_limit;
					pilot_pwm_timer_set_duty_cycle(stout, current_set);
				}
				// Check the diode before charging
				if (S_M_DIODE_ERR_CHECK == event) {
					// Wait for 20 ms
					if (DIODE_ERR_DELAY <= get_count_ms()) {
						if (CP_DIODE_ERROR_LIMIT < stout->pwm_low_val) {
							event = S_M_DIODE_ERR;
							pr_debug("Diode error found while initiating charging session \n");
						} else {
							event = S_M_DIODE_CHECK_DONE;
							pr_debug("Diode OK \n");
						}
					}
				} else if ( S_M_CHARGING_START == event) {
					pr_debug("CLOSE THE RELAY STATE D\n");
					// Close the relay
					ret = relay_close(stout->relay);
					if (ret)
						goto error;
					start_charging();
					set_relay_status();
					// Reset the counter used for led blinking during charging
					cnt_disp = 0;
				} else if ((S_M_CHARGING_D == event) && (LED_BLINKING_10A <= cnt_disp)) {
					interface_disp(stout);
					cnt_disp = 0;
				}
				// Increment the counter used for LED blinking
				cnt_disp++;
				break;
			//---------------------------------END STATE D---------------------------------

			//---------------------------------STATE FAULT---------------------------------
			case STATE_FAULT:
				// Debug message
				if (stout->current_state != stout->previous_state) {
					pr_debug("STATE FAULT\n");
				}
				// Check with error is active
				switch (stout->err_status) {
				//-----------------------Diode error-----------------------------
				// Latching fault
				case INTF_DIODE_ERR:
					pr_debug("Diode error!\n");
					// CP value set to DC indicating EVSE not ready
					pilot_pwm_timer_set_duty_cycle(stout, PWM_DC);
					// Open the relay
					relay_open(stout->relay);
					reset_relay_status();
					stop_charging();
					// Stop the CP signal
					pilot_pwm_timer_set_duty_cycle(stout, PWM_OFF);
					// event = S_M_WAIT;
					goto error;
					break;
				//-----------------------Diode error end-------------------------

				//--------------------------CP error-----------------------------
				// Latching fault
				case INTF_PILOT_ERROR:
					pr_debug("Control Pilot error!\n");
					// Stop the CP signal
					pilot_pwm_timer_set_duty_cycle(stout, PWM_OFF);
					// Open the relay
					relay_open(stout->relay);
					reset_relay_status();
					stop_charging();
					goto error;
					break;
				//-----------------------CP error end-------------------------

				//----------------Input undervoltage detected-----------------
				// Non latching fault
				case INTF_INPUT_V_ERR_U:
					if (S_M_UNDERVOLTAGE == event) {
						pr_debug("Supply undervoltage error! Vin = %d mV\n", stout->v1_max);
						// CP value set to DC indicating EVSE not ready
						pilot_pwm_timer_set_duty_cycle(stout, PWM_DC);
						// Open relay
						relay_open(stout->relay);
						reset_relay_status();
						stop_charging();
					}
					break;
				//----------------Input undervoltage end-----------------

				//----------------Input overrvoltage detected-----------------
				// Non latching fault
				case INTF_INPUT_V_ERR_O:
					pr_debug("Supply overvoltage error! Vin = %d mV\n", stout->v1_max);
					// CP value set to DC indicating EVSE not ready
					pilot_pwm_timer_set_duty_cycle(stout, PWM_DC);
					// Open relay
					relay_open(stout->relay);
					reset_relay_status();
					stop_charging();
					goto error;
					break;
				//-------------------Overcurrent error-------------------
				// Latching fault
				case INTF_OVERCURRENT_ERR:
					if (S_M_OVERCURRENT_WAIT != event) {
						pr_debug("Current error! or PE missing Iout = %d mA\n", stout->i_val);
						// CP value set to DC indicating EVSE not ready
						pilot_pwm_timer_set_duty_cycle(stout, PWM_DC);
						// Open relay
						relay_open(stout->relay);
						reset_relay_status();
						stop_charging();
						// Stop the CP signal
						pilot_pwm_timer_set_duty_cycle(stout, PWM_OFF);
						goto error;
					}
					break;
				//----------------Overcurrent error end------------------

				//----------------------RCD error------------------------
				// Latching error
				case INTF_RCD_ERROR:
					pr_debug("RCD ERROR! \n");
					// CP value set to DC indicating EVSE not ready
					pilot_pwm_timer_set_duty_cycle(stout, PWM_DC);
					// Open relay
					relay_open(stout->relay);
					reset_relay_status();
					stop_charging();
					// Stop the CP signal
					pilot_pwm_timer_set_duty_cycle(stout, PWM_OFF);
					goto error;
					break;
				//----------------------RCD error end--------------------

				//------------------Overtemperature error----------------
				// Latching error (if this point is reached the temperature exceeded the high temperature limit)
				case INTF_TEMPERATURE_ERR:
					pr_debug("Overtemperature! Temperature = %d deg C\n", stout->temperature);
					// CP value set to DC indicating EVSE not ready
					pilot_pwm_timer_set_duty_cycle(stout, PWM_DC);
					// Open relay
					relay_open(stout->relay);
					reset_relay_status();
					stop_charging();
					// Stop the CP signal
					pilot_pwm_timer_set_duty_cycle(stout, PWM_OFF);
					goto error;
					break;
				//------------------Overtemperature error end-------------
				//------------------PE error----------------
				case INTF_PE_ERROR:
					pr_debug("PE ERROR!\n");
					// CP value set to DC indicating EVSE not ready
					pilot_pwm_timer_set_duty_cycle(stout, PWM_DC);
					// Open relay
					relay_open(stout->relay);
					reset_relay_status();
					stop_charging();
					// Stop the CP signal
					pilot_pwm_timer_set_duty_cycle(stout, PWM_OFF);
					goto error;
					break;
					//------------------PE error end-------------
#if defined(REV_D)
				//------------------PE upstream error--------
				case INTF_PE_UPSTREAM_ERR:
					pr_debug("PE UPSTREAM ERROR!\n");
					// CP value set to DC indicating EVSE not ready
					pilot_pwm_timer_set_duty_cycle(stout, PWM_DC);
					// Open relay
					relay_open(stout->relay);
					reset_relay_status();
					stop_charging();
					// Stop the CP signal
					pilot_pwm_timer_set_duty_cycle(stout, PWM_OFF);
					goto error;
					break;
					//-------------PE upstream error end---------
#endif

				default:
					// Insert code for an error state other than the above treated ones
					break;
				}
				break;
				//---------------------------------STATE FAULT END---------------------------------
			}
		}
		// ------------------------------- END STATE MACHINE -------------------------------------
	}
	/*******************************************************************************************************/
	/************************************The main loop of the State Machine END*****************************/
	/*******************************************************************************************************/

	/*****************************************ERROR latching and display****************************************/
error:
	pr_debug("Error. Returned %d.\n", ret);
	while (1) {
		ret = interface_disp(stout);
		if (ret)
			return ret;
		no_os_mdelay(50);
	}
	return ret;
	/*****************************************ERROR latching and display END****************************************/
}
/*******************************************************************************************************************/
/************************************************ State Machine function END****************************************/
/*******************************************************************************************************************/

/*******************************************************************************************************************/
/**************************************************** CP event function ********************************************/
/*******************************************************************************************************************/
/**
 * @brief Determine next state based on CP measurements and previous events
 * @param stout - state machine descriptor
 * @param event_in - previous event
 * @return event that took place
 */
enum state_machine_events_e state_machine_det_event_cp(struct stout *stout,
		enum state_machine_events_e event_in)
{
	enum state_machine_events_e event = event_in;

	if ((CP_H_LOW_LIMIT > stout->pwm_high_val)
	    && (S_M_INITIAL_CHECK_DONE != event)) {
		// CP error
		// The value of the CP high side is compared with its low limit
		// If just after startupt skip this step
		event = S_M_CP_ERROR;
		stout->previous_state = stout->current_state;
		// The next step is to interpret the error
		stout->current_state = STATE_FAULT;
		stout->err_status = INTF_PILOT_ERROR;
		// The counter is reset in case another check after a time amount is made
		// in the current configuration the error is latching
		reset_count_ms();
		if (event != event_in) {
			pr_debug("CP error detected\n");
		}
	} else if (((CP_DIODE_ERROR_LIMIT < stout->pwm_low_val)
		    && (S_M_DIODE_ERR_CHECK != event)
		    && (!stout->ac_dc)) && ((S_M_CHARGING == event) || (S_M_CHARGING_D == event))) {
		// CP diode error check
		// The low side value of CP is compared with its high limit
		// If during diode error check skip this step or if in another state than charging
		// The CP must be PWM to detect the diode
		event = S_M_DIODE_ERR;
		stout->previous_state = stout->current_state;
		// The next step is to interpret the error
		stout->current_state = STATE_FAULT;
		stout->err_status = INTF_DIODE_ERR;
		reset_count_ms();
		pr_debug("Diode error detected CP_H: %d mV CP_L: %d mV\n", stout->pwm_high_val,
			 stout->pwm_low_val);
	}

	//---------------------------------- CP value for state A -------------------------------------
	else if ((CPH_A_LIMIT_LOW < stout->pwm_high_val)
		 && (stout->pwm_high_val < CPH_A_LIMIT_HIGH)) {
		// CP high side value indicates state A
		if ((S_M_INITIAL_CHECK_DONE == event) || (S_M_VIN_RECOVER == event)) {
			// If after startup or recovery put in IDLE
			event = S_M_WAIT;
			stout->previous_state = stout->current_state;
			// next step is IDLE
			stout->current_state = STATE_A;
			stout->err_status = INTF_NO_ERR;
			if (event != event_in) {
				pr_debug("EV disconnected and system powered on\n");
			}
		} else if ((S_M_WAIT != event) && (S_M_DISCONNECTED != event)
			   && (S_M_CHARGING != event) && (S_M_CHARGING_D != event)
			   && (S_M_WAIT_B != event) && (S_M_EVSE_READY != event)
			   && (S_M_STOP_CHARGING != event) && (S_M_CHECK_RELAY_PASSED != event)
			   && (S_M_PE_ERROR != event)) {
			// EV disconnected from other state than charging or B
			event = S_M_DISCONNECTED;
			stout->previous_state = stout->current_state;
			// The next step is IDLE
			stout->current_state = STATE_A;
			stout->err_status = INTF_NO_ERR;
			if (event != event_in) {
				pr_debug("EV disconnected (IDLE)\n");
			}
		} else if ((S_M_CHARGING == event) || (S_M_CHARGING_D == event)) {
			// If state A is reached from a charging state then the cable was unplugged
			//while a charging session was in progress
			event = S_M_STOP_CHARGING_2;
			stout->previous_state = stout->current_state;
			// The next step is to open the relay
			stout->current_state = STATE_RELAY_OPEN;
			stout->err_status = INTF_NO_ERR;
			if (event != event_in) {
				pr_debug("Cable unplugged while charging/PE ERROR\n");
			}
		} else if (S_M_STOP_CHARGING == event) {
			// If state A is reached from a charging state then the cable was unplugged
			//while a charging session was in progress
			event = S_M_PE_ERROR;
			stout->previous_state = stout->current_state;
			// The next step is to open the relay
			stout->current_state = STATE_FAULT;
			stout->err_status = INTF_PE_ERROR;
			if (event != event_in) {
				pr_debug("Cable unplugged while charging/PE ERROR\n");
			}
		} else {
			// The A state IDLE
			event = S_M_WAIT;
			stout->previous_state = stout->current_state;
			// Remain in state A
			stout->current_state = STATE_A;
			stout->err_status = INTF_NO_ERR;
			reset_count_ms();
			if (event != event_in) {
				pr_debug("EV disconnected and system waiting\n");
			}
		}
	}
	//---------------------------------- END CP value for state A -------------------------------------

	//---------------------------------- CP value for state B -----------------------------------------
	else if ((CPH_B_LIMIT_LOW < stout->pwm_high_val)
		 && (stout->pwm_high_val < CPH_B_LIMIT_HIGH)) {
		// CP high side value indicates state B
		if (((S_M_WAIT == event) || (S_M_INITIAL_CHECK_DONE == event)
		     || (S_M_VIN_RECOVER == event)) && (1000 > get_count_ms())) {
			// If cable pluged in the EV this step should be reached in maximum 2s
			event = S_M_EV_CONNECTED;
			stout->previous_state = stout->current_state;
			stout->current_state = STATE_B;
			stout->err_status = INTF_NO_ERR;
			reset_count_ms();
			if (event != event_in) {
				pr_debug("EV ready and connected waiting charging start\n");
			}
		} else if ((S_M_EV_CONNECTED == event) && (CP_DC_PWM_DELAY <= get_count_ms())) {
			// If EV connected wait CP_DC_PWM_DELAY until change CP from DC (EVSE not ready) to PWM (EVSE ready)
			event = S_M_EVSE_READY;
			stout->previous_state = stout->current_state;
			stout->current_state = STATE_B;
			stout->err_status = INTF_NO_ERR;
			reset_count_ms();
			if (event != event_in) {
				pr_debug("EV ready and connected, EVSE ready (PWM)\n");
			}
		} else if ((S_M_CHARGING == event) || (S_M_CHARGING_D == event)) {
			// If state B reached from a charging state then the EV requested to stop charging
			event = S_M_STOP_CHARGING;
			stout->previous_state = stout->current_state;
			stout->current_state = STATE_RELAY_OPEN;
			stout->err_status = INTF_NO_ERR;
			if (event != event_in) {
				pr_debug("EV ready and charging stop requested by EV\n");
			}
		} else if (S_M_STOP_CHARGING == event) {
			// If relay opened check relay soldered contacts
			event = S_M_CHECK_STUCK_RELAY;
			stout->previous_state = stout->current_state;
			stout->current_state = STATE_CHECK_RELAY;
			stout->err_status = INTF_NO_ERR;
			//reset_count_ms();
			if (event != event_in) {
				pr_debug("EV ready and connected chargign stopped, check stuck relay\n");
			}
		} else if (S_M_CHECK_RELAY_PASSED == event) {
			// If relay check charging session stoped succesfuly
			event = S_M_CHARGING_STOPPED;
			stout->previous_state = stout->current_state;
			stout->current_state = STATE_B;
			stout->err_status = INTF_NO_ERR;
			if (event != event_in) {
				pr_debug("EV ready and connected chargign stopped, relay check passed\n");
			}
		} else if ((S_M_CHARGING_STOPPED == event) || (S_M_EVSE_READY == event)) {
			// IDLE with the EV connected
			event = S_M_WAIT_B;
			stout->previous_state = stout->current_state;
			stout->current_state = STATE_B;
			stout->err_status = INTF_NO_ERR;
			reset_count_ms();
			if (event != event_in) {
				pr_debug("EV ready and connected IDLE \n");
			}
		} else if (S_M_DIODE_ERR == event) {
			event = S_M_DIODE_ERR;
			stout->previous_state = stout->current_state;
			stout->current_state = STATE_FAULT;
			stout->err_status = INTF_DIODE_ERR;
		}
		//---------------------------------- END CP value for state B -------------------------------------

		//---------------------------------- CP value for state C -----------------------------------------
	} else if ((CPH_C_LIMIT_LOW < stout->pwm_high_val)
		   && (stout->pwm_high_val < CPH_C_LIMIT_HIGH)) {
		// CP high side value indicates state C
		if (S_M_WAIT_B == event) {
			// Charging session initiated by EV
			event = S_M_CHARGING_REQ;
			stout->previous_state = stout->current_state;
			// RCD must be checked before closing relay if the time from last check greater than RCD_TIME_REPEAT_INTERVAL
			stout->current_state = STATE_RCD_SELF_TEST;
			stout->err_status = INTF_NO_ERR;
			if (event != event_in) {
				pr_debug("EV charging requested, no ventilation\n");
			}
		} else if (S_M_RCD_TEST_COMPLETED == event) {
			// If RCD test passed check if the diode is present before closing the relay
			event = S_M_DIODE_ERR_CHECK;
			stout->previous_state = stout->current_state;
			stout->current_state = STATE_C;
			stout->err_status = INTF_NO_ERR;
			reset_count_ms();
			if (event != event_in) {
				pr_debug("EV ready and connected, checking diode presence\n");
			}
		} else if (S_M_DIODE_CHECK_DONE == event) {
			// If Diode check passed the charging can begin
			event = S_M_CHARGING_START;
			stout->previous_state = stout->current_state;
			stout->current_state = STATE_C;
			stout->err_status = INTF_NO_ERR;
			if (event != event_in) {
				pr_debug("EV charging can start, no ventilation\n");
			}
		} else if ((S_M_CHARGING_START == event)) {
			// Charging started
			event = S_M_CHARGING;
			stout->previous_state = stout->current_state;
			stout->current_state = STATE_C;
			stout->err_status = INTF_NO_ERR;
			if (event != event_in) {
				pr_debug("Ev charging started, no ventilation\n");
			}
		} else if (S_M_DIODE_ERR == event) {
			// If diode error detected go to error state
			event = S_M_DIODE_ERR;
			stout->previous_state = stout->current_state;
			stout->current_state = STATE_FAULT;
			stout->err_status = INTF_DIODE_ERR;
		}
		//---------------------------------- END CP value for state C -------------------------------------

		//---------------------------------- CP value for state D -----------------------------------------
	} else if ((CPH_D_LIMIT_LOW < stout->pwm_high_val)
		   && (stout->pwm_high_val < CPH_D_LIMIT_HIGH)) {
		// CP high side value indicates state D
		if (S_M_WAIT_B == event) {
			// Charging session with ventilation requested by EV
			event = S_M_CHARGING_REQ;
			stout->previous_state = stout->current_state;
			// RCD must be checked before closing relay if the time from last check greater than RCD_TIME_REPEAT_INTERVAL
			stout->current_state = STATE_RCD_SELF_TEST;
			stout->err_status = INTF_NO_ERR;
			if (event != event_in) {
				pr_debug("EV charging requested, ventilation required\n");
			}
		} else if (S_M_RCD_TEST_COMPLETED == event) {
			// RCD test completed succesfuly
			// The diode presence is checked
			event = S_M_DIODE_ERR_CHECK;
			stout->previous_state = stout->current_state;
			stout->current_state = STATE_D;
			stout->err_status = INTF_NO_ERR;
			reset_count_ms();
			if (event != event_in) {
				pr_debug("EV ready and connected, checking diode presence\n");
			}
		} else if (S_M_DIODE_CHECK_DONE == event) {
			// Diode check passed, relay can be closed
			event = S_M_CHARGING_START;
			stout->previous_state = stout->current_state;
			stout->current_state = STATE_D;
			stout->err_status = INTF_NO_ERR;
			if (event != event_in) {
				pr_debug("EV charging start, ventilation\n");
			}
		} else if ((S_M_CHARGING_START == event)) {
			// Charging with ventilation in progress
			event = S_M_CHARGING_D;
			stout->previous_state = stout->current_state;
			stout->current_state = STATE_D;
			stout->err_status = INTF_NO_ERR;
			if (event != event_in) {
				pr_debug("EV charging, ventilation\n");
			}
		} else if (S_M_DIODE_ERR == event) {
			// If diode error detected go to error state
			event = S_M_DIODE_ERR;
			stout->previous_state = stout->current_state;
			stout->current_state = STATE_FAULT;
			stout->err_status = INTF_DIODE_ERR;
		}
	}
	return event;
}
/*******************************************************************************************************************/
/**************************************************** CP event function END ****************************************/
/*******************************************************************************************************************/

/*******************************************************************************************************************/
/************************************************* Temperature event function **************************************/
/*******************************************************************************************************************/
/**
 * @brief Determine next state based on temperature measurements and previous events
 * @param stout - state machine descriptor
 * @param event_in - previous event
 * @return event that took place
 */
enum state_machine_events_e state_machine_det_event_temperature(
	struct stout *stout,
	enum state_machine_events_e event_in)
{
	enum state_machine_events_e event = event_in;

	if (TEMPERATURE_LIMIT_2 < stout->temperature) {
		// If second temperature limit exceeded go to fault state
		event = S_M_OVER_TEMPERATURE_2;
		stout->previous_state = stout->current_state;
		stout->current_state = STATE_FAULT;
		stout->err_status = INTF_TEMPERATURE_ERR;
		if (event != event_in) {
			pr_debug("Temperature limit exceeded Temperature = %d deg C/n",
				 stout->temperature);
		}
	} else if (TEMPERATURE_LIMIT_1 < stout->temperature) {
		// If first temperature limit exceeded derate output power (if charging with 16A reduce current to 10A)
		if ((S_M_CHARGING == event) || (S_M_CHARGING_D == event)
		    || (S_M_OVER_TEMPERATURE_1 == event)) {
			event = S_M_OVER_TEMPERATURE_1;
			stout->previous_state = stout->current_state;
			stout->err_status = INTF_NO_ERR;
			if (event != event_in) {
				pr_debug("Temperature high, limit power if in state C; Temperature = %d deg C/n",
					 stout->temperature);
			}
		}
	}

	return event;
}
/*******************************************************************************************************************/
/********************************************** Temperature event function END *************************************/
/*******************************************************************************************************************/

/*******************************************************************************************************************/
/*********************************************** Vin and Iout event function ***************************************/
/*******************************************************************************************************************/
/**
 * @brief Determine next state based on supply measurements and previous events
 * @param stout - state machine descriptor
 * @param event_in - previous event
 * @return event that took place
 */
enum state_machine_events_e state_machine_det_event_supply(struct stout *stout,
		enum state_machine_events_e event_in)
{
	enum state_machine_events_e event = event_in;

	if (((VIN_HIGH_LIMIT_2 < stout->v1_max) && (1 <= stout->grid)
	     && (S_M_CHECK_STUCK_RELAY != event))
	    || ((VIN_HIGH_LIMIT < stout->v1_max) && (0 == stout->grid)
		&& (S_M_CHECK_STUCK_RELAY != event))) {
		event = S_M_OVERVOLTAGE;
		stout->previous_state = stout->current_state;
		stout->current_state = STATE_FAULT;
		stout->err_status = INTF_INPUT_V_ERR_O;
	} else if (((VIN_LOW_LIMIT_2 > stout->v1_max) && (S_M_UNDERVOLTAGE != event)
		    && (S_M_CHECK_STUCK_RELAY != event) && (1 <= stout->grid))
		   || ((VIN_LOW_LIMIT > stout->v1_max) && (S_M_UNDERVOLTAGE != event)
		       && (S_M_CHECK_STUCK_RELAY != event) && (0 == stout->grid))) {
		// If undervoltage go to fault state
		event = S_M_UNDERVOLTAGE;
		stout->previous_state = stout->current_state;
		stout->current_state = STATE_FAULT;
		stout->err_status = INTF_INPUT_V_ERR_U;
	} else if (((VIN_LOW_LIMIT_2 > stout->v1_max) && (S_M_UNDERVOLTAGE == event)
		    && (S_M_CHECK_STUCK_RELAY != event) && (1 <= stout->grid))
		   || ((VIN_LOW_LIMIT > stout->v1_max) && (S_M_UNDERVOLTAGE == event)
		       && (S_M_CHECK_STUCK_RELAY != event) && (0 == stout->grid))) {
		// If undervoltage detected wait until Vin is in range
		event = S_M_UNDERVOLTAGE_WAIT;
		stout->previous_state = stout->current_state;
		stout->current_state = STATE_FAULT;
		stout->err_status = INTF_INPUT_V_ERR_U;
	} else if (((VIN_LOW_LIMIT_2 < stout->v1_max) && ((S_M_UNDERVOLTAGE == event)
			|| (S_M_UNDERVOLTAGE_WAIT == event)) && (1 <= stout->grid))
		   || ((VIN_LOW_LIMIT < stout->v1_max) && ((S_M_UNDERVOLTAGE == event)
				   || (S_M_UNDERVOLTAGE_WAIT == event)) && (0 == stout->grid))) {
		// If voltage in range recover
		event = S_M_VIN_RECOVER;
		stout->err_status = INTF_NO_ERR;
		reset_count_ms();
	}
	//###################### IMPLEMENT OVERVOLTAGE ################################

	if ((S_M_CHARGING == event) || (S_M_CHARGING_D == event)
	    || (S_M_OVERCURRENT_WAIT == event)
	    || (S_M_CHARGING_STOPPED == event) || (S_M_STOP_CHARGING == event)) {
		if ((stout->i_val > stout->i_limit) && (S_M_OVERCURRENT_WAIT != event)) {
			event = S_M_OVERCURRENT_WAIT;
			reset_count_ms();
			stout->previous_state = stout->current_state;
		} else if ((ADJUST_TIME <= get_count_ms()) && (S_M_OVERCURRENT_WAIT == event)) {
			if (stout->i_val > stout->i_limit) {
				pr_debug("Overcurrent detected /n");
				event = S_M_OVERCURRENT;
				stout->previous_state = stout->current_state;
				stout->current_state = STATE_FAULT;
				stout->err_status = INTF_OVERCURRENT_ERR;
			} else {
				event = S_M_CHARGING_START;
				stout->err_status = INTF_NO_ERR;
			}
		}
	} else if (((stout-> i_val > stout->i_limit_not_ch))
		   && ((S_M_CHECK_STUCK_RELAY != event) && (S_M_CP_ERROR != event))) {
		event = S_M_OVERCURRENT;
		stout->previous_state = stout->current_state;
		stout->current_state = STATE_FAULT;
		stout->err_status = INTF_OVERCURRENT_ERR;
	}

	return event;
}

/*******************************************************************************************************************/
/*********************************************** Vin and Iout event function END***************************************/
/*******************************************************************************************************************/

/**
 * @brief Get take action flag
 *
 * @return flag value
 */
uint16_t get_action(void)
{
	return zcross_cnt;
}

/**
 * @brief Set take action flag
 *
 * @return none
 */
void set_action_flag(void)
{
	zcross_cnt = TAKE_S_M_ACTION;
}

/**
 * @brief Reset take action flag
 *
 * @return none
 */
void reset_action_flag(void)
{
	zcross_cnt = 0;
}

/**
 * @brief Increment take action flag
 *
 * @return none
 */
void inc_action_flag(void)
{
	zcross_cnt++;
}

/**
 * @brief Signal that EV starts charging
 *
 * @return none
 */
void start_charging(void)
{
	is_charging = 1;
}

/**
 * @brief Signal that EV stops charging
 *
 * @return none
 */
void stop_charging(void)
{
	is_charging = 0;
}

/**
 * @brief Get charging state
 *
 * @return charging state - 1 charging, 0 - not charging
 */
uint8_t car_is_charging(void)
{
	return is_charging;
}

