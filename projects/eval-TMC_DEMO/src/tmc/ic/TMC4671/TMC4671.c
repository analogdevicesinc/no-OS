/*******************************************************************************
* Copyright © 2016 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#include "TMC4671.h"

#define STATE_NOTHING_TO_DO    0
#define STATE_START_INIT       1
#define STATE_WAIT_INIT_TIME   2
#define STATE_ESTIMATE_OFFSET  3

// => SPI wrapper
extern uint8_t tmc4671_readwriteByte(uint8_t motor, uint8_t data, uint8_t lastTransfer);
// <= SPI wrapper

// spi access
int32_t tmc4671_readInt(uint8_t motor, uint8_t address)
{
	// clear write bit
	address &= 0x7F;

	// write address
	tmc4671_readwriteByte(motor, address, false);

	// read data
	int32_t value = tmc4671_readwriteByte(motor, 0, false);
	value <<= 8;
	value |= tmc4671_readwriteByte(motor, 0, false);
	value <<= 8;
	value |= tmc4671_readwriteByte(motor, 0, false);
	value <<= 8;
	value |= tmc4671_readwriteByte(motor, 0, true);

	return value;
}

void tmc4671_writeInt(uint8_t motor, uint8_t address, int32_t value)
{
	// write address
	tmc4671_readwriteByte(motor, address|0x80, false);

	// write value
	tmc4671_readwriteByte(motor, 0xFF & (value>>24), false);
	tmc4671_readwriteByte(motor, 0xFF & (value>>16), false);
	tmc4671_readwriteByte(motor, 0xFF & (value>>8), false);
	tmc4671_readwriteByte(motor, 0xFF & (value>>0), true);
}

uint16_t tmc4671_readRegister16BitValue(uint8_t motor, uint8_t address, uint8_t channel)
{
	int32_t registerValue = tmc4671_readInt(motor, address);

	// read one channel
	switch(channel)
	{
	case BIT_0_TO_15:
		return (registerValue & 0xFFFF);
		break;
	case BIT_16_TO_31:
		return ((registerValue >> 16) & 0xFFFF);
		break;
	}
	return 0;
}

void tmc4671_writeRegister16BitValue(uint8_t motor, uint8_t address, uint8_t channel, uint16_t value)
{
	// read actual register content
	int32_t registerValue = tmc4671_readInt(motor, address);

	// update one channel
	switch(channel)
	{
	case BIT_0_TO_15:
		registerValue &= 0xFFFF0000;
		registerValue |= value;
		break;
	case BIT_16_TO_31:
		registerValue &= 0x0000FFFF;
		registerValue |= (value << 16);
		break;
	}
	// write the register
	tmc4671_writeInt(motor, address, registerValue);
}

void tmc4671_switchToMotionMode(uint8_t motor, uint8_t mode)
{
	// switch motion mode
	uint32_t actualModeRegister = tmc4671_readInt(motor, TMC4671_MODE_RAMP_MODE_MOTION);
	actualModeRegister &= 0xFFFFFF00;
	actualModeRegister |= mode;
	tmc4671_writeInt(motor, TMC4671_MODE_RAMP_MODE_MOTION, actualModeRegister);
}

void tmc4671_setTargetTorque_raw(uint8_t motor, int32_t targetTorque)
{
	tmc4671_switchToMotionMode(motor, TMC4671_MOTION_MODE_TORQUE);
	tmc4671_writeRegister16BitValue(motor, TMC4671_PID_TORQUE_FLUX_TARGET, BIT_16_TO_31, targetTorque);
}

int32_t tmc4671_getTargetTorque_raw(uint8_t motor)
{
	// remember last set index
	uint32_t lastIndex = tmc4671_readInt(motor, TMC4671_INTERIM_ADDR);

	// get value
	tmc4671_writeInt(motor, TMC4671_INTERIM_ADDR, 0);
	int32_t value = (int32_t)tmc4671_readInt(motor, TMC4671_INTERIM_DATA);

	// reset last set index
	tmc4671_writeInt(motor, TMC4671_INTERIM_ADDR, lastIndex);
	return value;
}

int32_t tmc4671_getActualTorque_raw(uint8_t motor)
{
	return (int16_t) tmc4671_readRegister16BitValue(motor, TMC4671_PID_TORQUE_FLUX_ACTUAL, BIT_16_TO_31);
}

int32_t tmc4671_getActualRampTorque_raw(uint8_t motor)
{
	// no ramp implemented
	UNUSED(motor);
	return 0;
}

void tmc4671_setTargetTorque_mA(uint8_t motor, uint16_t torqueMeasurementFactor, int32_t targetTorque)
{
	tmc4671_switchToMotionMode(motor, TMC4671_MOTION_MODE_TORQUE);
	tmc4671_writeRegister16BitValue(motor, TMC4671_PID_TORQUE_FLUX_TARGET, BIT_16_TO_31, (targetTorque * 256) / (int32_t) torqueMeasurementFactor);
}

int32_t tmc4671_getTargetTorque_mA(uint8_t motor, uint16_t torqueMeasurementFactor)
{
	return (tmc4671_getTargetTorque_raw(motor) * (int32_t) torqueMeasurementFactor) / 256;
}

int32_t tmc4671_getActualTorque_mA(uint8_t motor, uint16_t torqueMeasurementFactor)
{
	return (tmc4671_getActualTorque_raw(motor) * (int32_t) torqueMeasurementFactor) / 256;
}

int32_t tmc4671_getTargetTorqueFluxSum_mA(uint8_t motor, uint16_t torqueMeasurementFactor)
{
	// remember last set index
	uint32_t lastIndex = tmc4671_readInt(motor, TMC4671_INTERIM_ADDR);

	// get target torque value
	tmc4671_writeInt(motor, TMC4671_INTERIM_ADDR, 0);
	int32_t torque = (int32_t) tmc4671_readInt(motor, TMC4671_INTERIM_DATA);

	// get target flux value
	tmc4671_writeInt(motor, TMC4671_INTERIM_ADDR, 1);
	int32_t flux = (int32_t) tmc4671_readInt(motor, TMC4671_INTERIM_DATA);

	// reset last set index
	tmc4671_writeInt(motor, TMC4671_INTERIM_ADDR, lastIndex);

	return (((int32_t)flux+(int32_t)torque) * (int32_t)torqueMeasurementFactor) / 256;
}

int32_t tmc4671_getActualTorqueFluxSum_mA(uint8_t motor, uint16_t torqueMeasurementFactor)
{
	int32_t registerValue = tmc4671_readInt(motor, TMC4671_PID_TORQUE_FLUX_ACTUAL);
	int16_t flux = (registerValue & 0xFFFF);
	int16_t torque = ((registerValue >> 16) & 0xFFFF);
	return (((int32_t)flux+(int32_t)torque) * (int32_t)torqueMeasurementFactor) / 256;
}

int32_t tmc4671_getActualRampTorque_mA(uint8_t motor, uint16_t torqueMeasurementFactor)
{
	// no ramp implemented
	UNUSED(motor);
	UNUSED(torqueMeasurementFactor);
	return 0;
}

void tmc4671_setTargetFlux_raw(uint8_t motor, int32_t targetFlux)
{
	// do not change the MOTION_MODE here! target flux can also be used during velocity and position modes
	tmc4671_writeRegister16BitValue(motor, TMC4671_PID_TORQUE_FLUX_TARGET, BIT_0_TO_15, targetFlux);
}

int32_t tmc4671_getTargetFlux_raw(uint8_t motor)
{
	// remember last set index
	uint32_t lastIndex = tmc4671_readInt(motor, TMC4671_INTERIM_ADDR);

	// get value
	tmc4671_writeInt(motor, TMC4671_INTERIM_ADDR, 1);
	int32_t value = (int32_t) tmc4671_readInt(motor, TMC4671_INTERIM_DATA);

	// reset last set index
	tmc4671_writeInt(motor, TMC4671_INTERIM_ADDR, lastIndex);
	return value;
}

int32_t tmc4671_getActualFlux_raw(uint8_t motor)
{
	return (int16_t) tmc4671_readRegister16BitValue(motor, TMC4671_PID_TORQUE_FLUX_ACTUAL, BIT_0_TO_15);
}

void tmc4671_setTargetFlux_mA(uint8_t motor, uint16_t torqueMeasurementFactor, int32_t targetFlux)
{
	// do not change the MOTION_MODE here! target flux can also be used during velocity and position modes
	tmc4671_writeRegister16BitValue(motor, TMC4671_PID_TORQUE_FLUX_TARGET, BIT_0_TO_15, (targetFlux * 256) / (int32_t) torqueMeasurementFactor);
}

int32_t tmc4671_getTargetFlux_mA(uint8_t motor, uint16_t torqueMeasurementFactor)
{
	return (tmc4671_getTargetFlux_raw(motor) * (int32_t) torqueMeasurementFactor) / 256;
}

int32_t tmc4671_getActualFlux_mA(uint8_t motor, uint16_t torqueMeasurementFactor)
{
	return (tmc4671_getActualFlux_raw(motor) * (int32_t) torqueMeasurementFactor) / 256;
}

void tmc4671_setTorqueFluxLimit_mA(uint8_t motor, uint16_t torqueMeasurementFactor, int32_t max)
{
	tmc4671_writeRegister16BitValue(motor, TMC4671_PID_TORQUE_FLUX_LIMITS, BIT_0_TO_15, (max * 256) / (int32_t) torqueMeasurementFactor);
}

int32_t tmc4671_getTorqueFluxLimit_mA(uint8_t motor, uint16_t torqueMeasurementFactor)
{
	return ((int32_t) tmc4671_readRegister16BitValue(motor, TMC4671_PID_TORQUE_FLUX_LIMITS, BIT_0_TO_15) * (int32_t) torqueMeasurementFactor) / 256;
}

void tmc4671_setTargetVelocity(uint8_t motor, int32_t targetVelocity)
{
	tmc4671_switchToMotionMode(motor, TMC4671_MOTION_MODE_VELOCITY);
	tmc4671_writeInt(motor, TMC4671_PID_VELOCITY_TARGET, targetVelocity);
}

int32_t tmc4671_getTargetVelocity(uint8_t motor)
{
	return (int32_t) tmc4671_readInt(motor, TMC4671_PID_VELOCITY_TARGET);
}

int32_t tmc4671_getActualVelocity(uint8_t motor)
{
	return (int32_t) tmc4671_readInt(motor, TMC4671_PID_VELOCITY_ACTUAL);
}

int32_t tmc4671_getActualRampVelocity(uint8_t motor)
{
	UNUSED(motor);
	// no ramp implemented
	return 0;
}

void tmc4671_setAbsolutTargetPosition(uint8_t motor, int32_t targetPosition)
{
	tmc4671_switchToMotionMode(motor, TMC4671_MOTION_MODE_POSITION);
	tmc4671_writeInt(motor, TMC4671_PID_POSITION_TARGET, targetPosition);
}

void tmc4671_setRelativeTargetPosition(uint8_t motor, int32_t relativePosition)
{
	tmc4671_switchToMotionMode(motor, TMC4671_MOTION_MODE_POSITION);
	// determine actual position and add relative position ticks
	tmc4671_writeInt(motor, TMC4671_PID_POSITION_TARGET, (int32_t) tmc4671_readInt(motor, TMC4671_PID_POSITION_ACTUAL) + relativePosition);
}

int32_t tmc4671_getTargetPosition(uint8_t motor)
{
	return (int32_t) tmc4671_readInt(motor, TMC4671_PID_POSITION_TARGET);
}

void tmc4671_setActualPosition(uint8_t motor, int32_t actualPosition)
{
	tmc4671_writeInt(motor, TMC4671_PID_POSITION_ACTUAL, actualPosition);
}

int32_t tmc4671_getActualPosition(uint8_t motor)
{
	return (int32_t) tmc4671_readInt(motor, TMC4671_PID_POSITION_ACTUAL);
}

int32_t tmc4671_getActualRampPosition(uint8_t motor)
{
	UNUSED(motor);
	// no ramp implemented
	return 0;
}

// encoder initialization
void tmc4671_doEncoderInitializationMode0(uint8_t motor, uint8_t *initState, uint16_t initWaitTime, uint16_t *actualInitWaitTime, uint16_t startVoltage,
		uint16_t *last_Phi_E_Selection, uint32_t *last_UQ_UD_EXT, int16_t *last_PHI_E_EXT)
{
	switch (*initState)
	{
	case STATE_NOTHING_TO_DO:
		*actualInitWaitTime = 0;
		break;
	case STATE_START_INIT: // started by writing 1 to initState

		// save actual set values for PHI_E_SELECTION, UQ_UD_EXT, and PHI_E_EXT
		*last_Phi_E_Selection = (uint16_t) tmc4671_readRegister16BitValue(motor, TMC4671_PHI_E_SELECTION, BIT_0_TO_15);
		*last_UQ_UD_EXT = (uint32_t) tmc4671_readInt(motor, TMC4671_UQ_UD_EXT);
		*last_PHI_E_EXT = (int16_t) tmc4671_readRegister16BitValue(motor, TMC4671_PHI_E_EXT, BIT_0_TO_15);

		//switch motion mode for running motor in open loop
		tmc4671_writeInt(motor, TMC4671_MODE_RAMP_MODE_MOTION, TMC4671_MOTION_MODE_UQ_UD_EXT);

		// set ABN_DECODER_PHI_E_OFFSET to zero
		tmc4671_writeRegister16BitValue(motor, TMC4671_ABN_DECODER_PHI_E_PHI_M_OFFSET, BIT_16_TO_31, 0);

		// select phi_e_ext
		tmc4671_writeRegister16BitValue(motor, TMC4671_PHI_E_SELECTION, BIT_0_TO_15, 1);

		// set an initialization voltage on UD_EXT (to the flux, not the torque!)
		tmc4671_writeRegister16BitValue(motor, TMC4671_UQ_UD_EXT, BIT_16_TO_31, 0);
		tmc4671_writeRegister16BitValue(motor, TMC4671_UQ_UD_EXT, BIT_0_TO_15, startVoltage);

		// set the "zero" angle
		tmc4671_writeRegister16BitValue(motor, TMC4671_PHI_E_EXT, BIT_0_TO_15, 0);

		*initState = STATE_WAIT_INIT_TIME;
		break;
	case STATE_WAIT_INIT_TIME:
		// wait until initialization time is over (until no more vibration on the motor)
		(*actualInitWaitTime)++;
		if(*actualInitWaitTime >= initWaitTime)
		{
			// set internal encoder value to zero
			tmc4671_writeInt(motor, TMC4671_ABN_DECODER_COUNT, 0);

			// switch back to last used UQ_UD_EXT setting
			tmc4671_writeInt(motor, TMC4671_UQ_UD_EXT, *last_UQ_UD_EXT);

			// set PHI_E_EXT back to last value
			tmc4671_writeRegister16BitValue(motor, TMC4671_PHI_E_EXT, BIT_0_TO_15, *last_PHI_E_EXT);

			// switch back to last used PHI_E_SELECTION setting
			tmc4671_writeRegister16BitValue(motor, TMC4671_PHI_E_SELECTION, BIT_0_TO_15, *last_Phi_E_Selection);

			// go to next state
			*initState = STATE_ESTIMATE_OFFSET;
		}
		break;
	case STATE_ESTIMATE_OFFSET:
		// you can do offset estimation here (wait for N-Channel if available and save encoder value)

		// go to ready state
		*initState = 0;
		break;
	default:
		*initState = 0;
		break;
	}
}

int16_t tmc4671_getS16CircleDifference(int16_t newValue, int16_t oldValue)
{
	return (newValue - oldValue);
}

void tmc4671_doEncoderInitializationMode2(uint8_t motor, uint8_t *initState, uint16_t *actualInitWaitTime,
		int16_t *hall_phi_e_old, int16_t *hall_phi_e_new, int16_t *hall_actual_coarse_offset, uint16_t *last_Phi_E_Selection)
{
	switch (*initState)
	{
	case STATE_NOTHING_TO_DO:
		*actualInitWaitTime = 0;
		break;
	case STATE_START_INIT: // started by writing 1 to initState
		// save actual set value for PHI_E_SELECTION
		*last_Phi_E_Selection = (uint16_t)tmc4671_readRegister16BitValue(motor, TMC4671_PHI_E_SELECTION, BIT_0_TO_15);

		// turn hall_mode interpolation off (read, clear bit 8, write back)
		tmc4671_writeInt(motor, TMC4671_HALL_MODE, tmc4671_readInt(motor, TMC4671_HALL_MODE) & 0xFFFFFEFF);

		// set ABN_DECODER_PHI_E_OFFSET to zero
		tmc4671_writeRegister16BitValue(motor, TMC4671_ABN_DECODER_PHI_E_PHI_M_OFFSET, BIT_16_TO_31, 0);

		// read actual hall angle
		*hall_phi_e_old = TMC4671_FIELD_READ(motor, TMC4671_HALL_PHI_E_INTERPOLATED_PHI_E, TMC4671_HALL_PHI_E_MASK, TMC4671_HALL_PHI_E_SHIFT);

		// read actual abn_decoder angle and compute difference to actual hall angle
		*hall_actual_coarse_offset = tmc4671_getS16CircleDifference(*hall_phi_e_old, (int16_t) tmc4671_readRegister16BitValue(motor, TMC4671_ABN_DECODER_PHI_E_PHI_M, BIT_16_TO_31));

		// set ABN_DECODER_PHI_E_OFFSET to actual hall-abn-difference, to use the actual hall angle for coarse initialization
		tmc4671_writeRegister16BitValue(motor, TMC4671_ABN_DECODER_PHI_E_PHI_M_OFFSET, BIT_16_TO_31, *hall_actual_coarse_offset);

		// normally MOTION_MODE_UQ_UD_EXT is only used by e.g. a wizard, not in normal operation
		if (TMC4671_FIELD_READ(motor, TMC4671_MODE_RAMP_MODE_MOTION, TMC4671_MODE_MOTION_MASK, TMC4671_MODE_MOTION_SHIFT) != TMC4671_MOTION_MODE_UQ_UD_EXT)
		{
			// select the use of phi_e_hall to start motor with hall signals
			tmc4671_writeRegister16BitValue(motor, TMC4671_PHI_E_SELECTION, BIT_0_TO_15, TMC4671_PHI_E_HALL);
		}

		*initState = STATE_WAIT_INIT_TIME;
		break;
	case STATE_WAIT_INIT_TIME:
		// read actual hall angle
		*hall_phi_e_new = TMC4671_FIELD_READ(motor, TMC4671_HALL_PHI_E_INTERPOLATED_PHI_E, TMC4671_HALL_PHI_E_MASK, TMC4671_HALL_PHI_E_SHIFT);

		// wait until hall angle changed
		if(*hall_phi_e_old != *hall_phi_e_new)
		{
			// estimated value = old value + diff between old and new (handle int16_t overrun)
			int16_t hall_phi_e_estimated = *hall_phi_e_old + tmc4671_getS16CircleDifference(*hall_phi_e_new, *hall_phi_e_old)/2;

			// read actual abn_decoder angle and consider last set abn_decoder_offset
			int16_t abn_phi_e_actual = (int16_t) tmc4671_readRegister16BitValue(motor, TMC4671_ABN_DECODER_PHI_E_PHI_M, BIT_16_TO_31) - *hall_actual_coarse_offset;

			// set ABN_DECODER_PHI_E_OFFSET to actual estimated angle - abn_phi_e_actual difference
			tmc4671_writeRegister16BitValue(motor, TMC4671_ABN_DECODER_PHI_E_PHI_M_OFFSET, BIT_16_TO_31, tmc4671_getS16CircleDifference(hall_phi_e_estimated, abn_phi_e_actual));

			// switch back to last used PHI_E_SELECTION setting
			tmc4671_writeRegister16BitValue(motor, TMC4671_PHI_E_SELECTION, BIT_0_TO_15, *last_Phi_E_Selection);

			// go to ready state
			*initState = 0;
		}
		break;
	default:
		*initState = 0;
		break;
	}
}

void tmc4671_checkEncderInitialization(uint8_t motor, uint32_t actualSystick, uint8_t initMode, uint8_t *initState, uint16_t initWaitTime, uint16_t *actualInitWaitTime, uint16_t startVoltage,
		int16_t *hall_phi_e_old, int16_t *hall_phi_e_new, int16_t *hall_actual_coarse_offset,
		uint16_t *last_Phi_E_Selection, uint32_t *last_UQ_UD_EXT, int16_t *last_PHI_E_EXT)
{
	// use the systick as 1ms timer for encoder initialization
	static uint32_t lastSystick = 0;
	if(actualSystick != lastSystick)
	{
		// needs timer to use the wait time
		if(initMode == 0)
		{
			tmc4671_doEncoderInitializationMode0(motor, initState, initWaitTime, actualInitWaitTime, startVoltage, last_Phi_E_Selection, last_UQ_UD_EXT, last_PHI_E_EXT);
		}
		lastSystick = actualSystick;
	}

	// needs no timer
	if(initMode == 2)
	{
		tmc4671_doEncoderInitializationMode2(motor, initState, actualInitWaitTime, hall_phi_e_old, hall_phi_e_new, hall_actual_coarse_offset, last_Phi_E_Selection);
	}
}

void tmc4671_periodicJob(uint8_t motor, uint32_t actualSystick, uint8_t initMode, uint8_t *initState, uint16_t initWaitTime, uint16_t *actualInitWaitTime, uint16_t startVoltage,
		int16_t *hall_phi_e_old, int16_t *hall_phi_e_new, int16_t *hall_actual_coarse_offset,
		uint16_t *last_Phi_E_Selection, uint32_t *last_UQ_UD_EXT, int16_t *last_PHI_E_EXT)
{
	tmc4671_checkEncderInitialization(motor, actualSystick, initMode, initState, initWaitTime, actualInitWaitTime, startVoltage,
			hall_phi_e_old, hall_phi_e_new, hall_actual_coarse_offset, last_Phi_E_Selection, last_UQ_UD_EXT, last_PHI_E_EXT);
}

void tmc4671_startEncoderInitialization(uint8_t mode, uint8_t *initMode, uint8_t *initState)
{
	// allow only a new initialization if no actual initialization is running
	if(*initState == STATE_NOTHING_TO_DO)
	{
		if(mode == 0) // estimate offset
		{
			// set mode
			*initMode = 0;

			// start initialization
			*initState = 1;
		}
		else if(mode == 2) // use hall sensor signals
		{
			// set mode
			*initMode = 2;

			// start initialization
			*initState = 1;
		}
	}
}

void tmc4671_updatePhiSelectionAndInitialize(uint8_t motor, uint8_t actualPhiESelection, uint8_t desiredPhiESelection, uint8_t initMode, uint8_t *initState)
{
	if (actualPhiESelection != desiredPhiESelection)
	{
		tmc4671_writeInt(motor, TMC4671_PHI_E_SELECTION, desiredPhiESelection);

		switch(desiredPhiESelection)
		{
			case 3:
				tmc4671_startEncoderInitialization(initMode, &initMode, initState);
				break;
		}
	}
}

// =====

void tmc4671_disablePWM(uint8_t motor)
{
	tmc4671_writeInt(motor, TMC4671_PWM_SV_CHOP, 0);
}

uint8_t tmc4671_getMotorType(uint8_t motor)
{
	return FIELD_GET(tmc4671_readInt(motor, TMC4671_MOTOR_TYPE_N_POLE_PAIRS), TMC4671_MOTOR_TYPE_MASK, TMC4671_MOTOR_TYPE_SHIFT);
}

void tmc4671_setMotorType(uint8_t motor, uint8_t motorType)
{
	TMC4671_FIELD_UPDATE(motor, TMC4671_MOTOR_TYPE_N_POLE_PAIRS, TMC4671_MOTOR_TYPE_MASK, TMC4671_MOTOR_TYPE_SHIFT, motorType);
}

uint8_t tmc4671_getPolePairs(uint8_t motor)
{
	return FIELD_GET(tmc4671_readInt(motor, TMC4671_MOTOR_TYPE_N_POLE_PAIRS), TMC4671_N_POLE_PAIRS_MASK, TMC4671_N_POLE_PAIRS_SHIFT);
}

void tmc4671_setPolePairs(uint8_t motor, uint8_t polePairs)
{
	TMC4671_FIELD_UPDATE(motor, TMC4671_MOTOR_TYPE_N_POLE_PAIRS, TMC4671_N_POLE_PAIRS_MASK, TMC4671_N_POLE_PAIRS_SHIFT, polePairs);
}

uint16_t tmc4671_getAdcI0Offset(uint8_t motor)
{
	return FIELD_GET(tmc4671_readInt(motor, TMC4671_ADC_I0_SCALE_OFFSET), TMC4671_ADC_I0_OFFSET_MASK, TMC4671_ADC_I0_OFFSET_SHIFT);
}

void tmc4671_setAdcI0Offset(uint8_t motor, uint16_t offset)
{
	TMC4671_FIELD_UPDATE(motor, TMC4671_ADC_I0_SCALE_OFFSET, TMC4671_ADC_I0_OFFSET_MASK, TMC4671_ADC_I0_OFFSET_SHIFT, offset);
}

uint16_t tmc4671_getAdcI1Offset(uint8_t motor)
{
	return FIELD_GET(tmc4671_readInt(motor, TMC4671_ADC_I1_SCALE_OFFSET), TMC4671_ADC_I1_OFFSET_MASK, TMC4671_ADC_I1_OFFSET_SHIFT);
}

void tmc4671_setAdcI1Offset(uint8_t motor, uint16_t offset)
{
	TMC4671_FIELD_UPDATE(motor, TMC4671_ADC_I1_SCALE_OFFSET, TMC4671_ADC_I1_OFFSET_MASK, TMC4671_ADC_I1_OFFSET_SHIFT, offset);
}

void tmc4671_setTorqueFluxPI(uint8_t motor, uint16_t pParameter, uint16_t iParameter)
{
	tmc4671_writeInt(motor, TMC4671_PID_FLUX_P_FLUX_I, ((uint32_t)pParameter << 16) | (uint32_t)iParameter);
	tmc4671_writeInt(motor, TMC4671_PID_TORQUE_P_TORQUE_I, ((uint32_t)pParameter << 16) | (uint32_t)iParameter);
}

void tmc4671_setVelocityPI(uint8_t motor, uint16_t pParameter, uint16_t iParameter)
{
	tmc4671_writeInt(motor, TMC4671_PID_VELOCITY_P_VELOCITY_I, ((uint32_t)pParameter << 16) | (uint32_t)iParameter);
}

void tmc4671_setPositionPI(uint8_t motor, uint16_t pParameter, uint16_t iParameter)
{
	tmc4671_writeInt(motor, TMC4671_PID_POSITION_P_POSITION_I, ((uint32_t)pParameter << 16) | (uint32_t)iParameter);
}

int32_t tmc4671_readFieldWithDependency(uint8_t motor, uint8_t reg, uint8_t dependsReg, uint32_t dependsValue, uint32_t mask, uint8_t shift)
{
	// remember old depends value
	uint32_t lastDependsValue = tmc4671_readInt(motor, dependsReg);

	// set needed depends value
	tmc4671_writeInt(motor, dependsReg, dependsValue);
	uint32_t value = FIELD_GET(tmc4671_readInt(motor, reg), mask, shift);

	// set old depends value
	tmc4671_writeInt(motor, dependsReg, lastDependsValue);
	return value;
}
