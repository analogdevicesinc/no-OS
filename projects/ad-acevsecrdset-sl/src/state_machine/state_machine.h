/***************************************************************************//**
 *   @file   state_machine.h
 *   @brief  STOUT state machine header file
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
#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define FIRMWARE_VERSION 	("v30923")
#define LED_NO 			(4u)

/******************************************************************************/
/**************************** Enumerations ************************************/
/******************************************************************************/
/**
 * @enum charger_state_e
 * @brief ADE9113 waveorm data.
 */
enum charger_state_e {
	STATE_POWER_ON = 0,
	STATE_RCD_SELF_TEST,
	STATE_A,
	STATE_B,
	STATE_C,
	STATE_D,
	STATE_RELAY_OPEN,
	STATE_CHECK_RELAY,
	STATE_FAULT
};

/**
 * @enum interface_err_status_e
 * @brief State machine fault condition.
 */
enum interface_err_status_e {
	/* No error */
	INTF_NO_ERR = 0,
	/* Relay error */
	INTF_RELAY_ERR,
	/* Input Vin error*/
	INTF_INPUT_V_ERR,
	/* Input undervoltage*/
	INTF_INPUT_V_ERR_U,
	/*Input overvoltage*/
	INTF_INPUT_V_ERR_O,
	/* RCD error */
	INTF_RCD_ERROR,
	/* Init RCD error */
	INTF_INIT_RCD_ERROR,
	/* Pilot error */
	INTF_PILOT_ERROR,
	/* Diode error */
	INTF_DIODE_ERR,
	/* Overcurrent */
	INTF_OVERCURRENT_ERR,
	/* Overtemperature fault*/
	INTF_TEMPERATURE_ERR,
#if defined(REV_D)
	/*PE upstream error*/
	INTF_PE_UPSTREAM_ERR,
#endif
	/*PE fault*/
	INTF_PE_ERROR
};

/**
 * @enum state_machine_events_e
 * @brief State machine event.
 */
enum state_machine_events_e {
	/* Supply undervoltage */
	S_M_UNDERVOLTAGE = 0,
	/* RCD */
	S_M_RCD,
	/* Overcurrent */
	S_M_OVERCURRENT,
	/* EV disconnected/no Earth */
	S_M_DISCONNECTED,
	/* Diode error */
	S_M_DIODE_ERR,
	/* Pilot error */
	S_M_CP_ERROR,
	/* RCD test initiated  */
	S_M_RCD_TEST_TRIGGERED,
	/* RCD test completed  */
	S_M_RCD_TEST_COMPLETED,
	/* RCD test failed  */
	S_M_RCD_TEST_FAIL,
	/* Ventilation required */
	S_M_VENTILATION_REQ,
	/* Wait for 5 seconds for check overcurrent */
	S_M_OVERCURRENT_WAIT,
	/* No event */
	S_M_NO_EVENT,
	/*Initial check done*/
	S_M_INITIAL_CHECK_DONE,
	/*Wait*/
	S_M_WAIT,
	/*EV connected*/
	S_M_EV_CONNECTED,
	/*Charging C*/
	S_M_CHARGING,
	/*Charging D*/
	S_M_CHARGING_D,
	/*Charging stopped*/
	S_M_CHARGING_STOPPED,
	/* EVSE ready*/
	S_M_EVSE_READY,
	/* Charging session requested*/
	S_M_CHARGING_REQ,
	/* Charging started*/
	S_M_CHARGING_START,
	/* Stop charging*/
	S_M_STOP_CHARGING,
	/* Stop charging when cable unplugged while charging*/
	S_M_STOP_CHARGING_2,
	/*Check relay*/
	S_M_CHECK_STUCK_RELAY,
	/*Relay check passed*/
	S_M_CHECK_RELAY_PASSED,
	/*IDLE B*/
	S_M_WAIT_B,
	/*Diode check*/
	S_M_DIODE_ERR_CHECK,
	/*Diode check done*/
	S_M_DIODE_CHECK_DONE,
	/*Vin undervoltage recovered*/
	S_M_VIN_RECOVER,
	/*Undervoltage wait*/
	S_M_UNDERVOLTAGE_WAIT,
	/*Overtemeperature 1 reduce power*/
	S_M_OVER_TEMPERATURE_1,
	/*Overtemperature 2 stop*/
	S_M_OVER_TEMPERATURE_2,
	/*Overvoltage*/
	S_M_OVERVOLTAGE,
	/*PE error*/
	S_M_PE_ERROR

};

/******************************************************************************/
/*********************** Structures Declarations ******************************/
/******************************************************************************/
/**
 * @struct stout
 * @brief Structure for Stout machine.
 */
struct stout {
	/* AD9113 device structure */
	struct ade9113_dev *ade9113;
	/* Relay descriptor */
	struct no_os_gpio_desc *relay;
	/* LED descriptors */
	struct no_os_gpio_desc *gpio_led[LED_NO];
	/* Current state */
	enum charger_state_e current_state;
	/* Previous state */
	enum charger_state_e previous_state;
	/* PWM low value measured */
	uint32_t pwm_low_val;
	/* PWM low value measured */
	uint32_t pwm_high_val;
	/* V2 max */
	int32_t v2_max;
	/* V2 max */
	int32_t v1_max;
	/* I val */
	int32_t i_val;
	/* Temperature*/
	int32_t temperature;
	/* Pilot AC/DC */
	uint8_t ac_dc;
	/* grid type 0 - 230V, 1 - 120V*/
	uint8_t grid;
	/* Maximum current */
	uint16_t i_limit;
	/* Maximum current not charging*/
	uint16_t i_limit_not_ch;
	/* RCD pins */
	struct no_os_gpio_desc *gpio_rcm_test;
	struct no_os_gpio_desc *gpio_rcddc;
	struct no_os_gpio_desc *gpio_rcdac;
	/* Current error status */
	enum interface_err_status_e err_status;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/*! Main routine */
int state_machine();

/*! Determine next state, CP measurements */
enum state_machine_events_e state_machine_det_event_cp(struct stout *stout,
		enum state_machine_events_e event_in);

/*! Determine next state, supply measurements */
enum state_machine_events_e state_machine_det_event_supply(struct stout *stout,
		enum state_machine_events_e event_in);

/*! Determine next state, temperature measurements */
enum state_machine_events_e state_machine_det_event_temperature(
	struct stout *stout,
	enum state_machine_events_e event_in);

/*! Get take action flag */
uint16_t get_action(void);

/*! Set take action flag */
void set_action_flag(void);

/*! Reset take action flag */
void reset_action_flag(void);

/*! Increment take action flag */
void inc_action_flag(void);

/*! Signal that EV starts charging */
void start_charging(void);

/*! Signal that EV stops charging */
void stop_charging(void);

/*! Get charging state */
uint8_t car_is_charging(void);

#endif /* __STATE_MACHINE_H__ */
