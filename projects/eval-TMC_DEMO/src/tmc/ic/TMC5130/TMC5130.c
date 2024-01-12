/*******************************************************************************
* Copyright © 2017 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#include "TMC5130.h"

// => SPI wrapper
// Send [length] bytes stored in the [data] array over SPI and overwrite [data]
// with the reply. The first byte sent/received is data[0].
extern void tmc5130_readWriteArray(uint8_t channel, uint8_t *data, size_t length);
// <= SPI wrapper

// Writes (x1 << 24) | (x2 << 16) | (x3 << 8) | x4 to the given address
void tmc5130_writeDatagram(TMC5130TypeDef *tmc5130, uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4)
{
	uint8_t data[5] = { address | TMC5130_WRITE_BIT, x1, x2, x3, x4 };
	tmc5130_readWriteArray(tmc5130->config->channel, &data[0], 5);

	int32_t value = ((uint32_t)x1 << 24) | ((uint32_t)x2 << 16) | (x3 << 8) | x4;

	// Write to the shadow register and mark the register dirty
	address = TMC_ADDRESS(address);
	tmc5130->config->shadowRegister[address] = value;
	tmc5130->registerAccess[address] |= TMC_ACCESS_DIRTY;
}

// Write an integer to the given address
void tmc5130_writeInt(TMC5130TypeDef *tmc5130, uint8_t address, int32_t value)
{
	tmc5130_writeDatagram(tmc5130, address, BYTE(value, 3), BYTE(value, 2), BYTE(value, 1), BYTE(value, 0));
}

// Read an integer from the given address
int32_t tmc5130_readInt(TMC5130TypeDef *tmc5130, uint8_t address)
{
	address = TMC_ADDRESS(address);

	// register not readable -> shadow register copy
	if(!TMC_IS_READABLE(tmc5130->registerAccess[address]))
		return tmc5130->config->shadowRegister[address];

	uint8_t data[5] = { 0, 0, 0, 0, 0 };

	data[0] = address;
	tmc5130_readWriteArray(tmc5130->config->channel, &data[0], 5);

	data[0] = address;
	tmc5130_readWriteArray(tmc5130->config->channel, &data[0], 5);

	return ((uint32_t)data[1] << 24) | ((uint32_t)data[2] << 16) | (data[3] << 8) | data[4];
}

// Initialize a TMC5130 IC.
// This function requires:
//     - tmc5130: The pointer to a TMC5130TypeDef struct, which represents one IC
//     - channel: The channel index, which will be sent back in the SPI callback
//     - config: A ConfigurationTypeDef struct, which will be used by the IC
//     - registerResetState: An int32_t array with 128 elements. This holds the values to be used for a reset.
void tmc5130_init(TMC5130TypeDef *tmc5130, uint8_t channel, ConfigurationTypeDef *config, const int32_t *registerResetState)
{
	tmc5130->velocity  = 0;
	tmc5130->oldTick   = 0;
	tmc5130->oldX      = 0;

	tmc5130->config               = config;
	tmc5130->config->callback     = NULL;
	tmc5130->config->channel      = channel;
	tmc5130->config->configIndex  = 0;
	tmc5130->config->state        = CONFIG_READY;

	size_t i;
	for(i = 0; i < TMC5130_REGISTER_COUNT; i++)
	{
		tmc5130->registerAccess[i]      = tmc5130_defaultRegisterAccess[i];
		tmc5130->registerResetState[i]  = registerResetState[i];
	}
}

// Fill the shadow registers of hardware preset non-readable registers
// Only needed if you want to 'read' those registers e.g to display the value
// in the TMCL IDE register browser
void tmc5130_fillShadowRegisters(TMC5130TypeDef *tmc5130)
{
	// Check if we have constants defined
	if(ARRAY_SIZE(tmc5130_RegisterConstants) == 0)
		return;

	size_t i, j;
	for(i = 0, j = 0; i < TMC5130_REGISTER_COUNT; i++)
	{
		// We only need to worry about hardware preset, write-only registers
		// that have not yet been written (no dirty bit) here.
		if(tmc5130->registerAccess[i] != TMC_ACCESS_W_PRESET)
			continue;

		// Search the constant list for the current address. With the constant
		// list being sorted in ascended order, we can walk through the list
		// until the entry with an address equal or greater than i
		while(j < ARRAY_SIZE(tmc5130_RegisterConstants) && (tmc5130_RegisterConstants[j].address < i))
			j++;

		// Abort when we reach the end of the constant list
		if (j == ARRAY_SIZE(tmc5130_RegisterConstants))
			break;

		// If we have an entry for our current address, write the constant
		if(tmc5130_RegisterConstants[j].address == i)
		{
			tmc5130->config->shadowRegister[i] = tmc5130_RegisterConstants[j].value;
		}
	}
}

// Reset the TMC5130.
uint8_t tmc5130_reset(TMC5130TypeDef *tmc5130)
{
	if(tmc5130->config->state != CONFIG_READY)
		return false;

	// Reset the dirty bits and wipe the shadow registers
	size_t i;
	for(i = 0; i < TMC5130_REGISTER_COUNT; i++)
	{
		tmc5130->registerAccess[i] &= ~TMC_ACCESS_DIRTY;
		tmc5130->config->shadowRegister[i] = 0;
	}

	tmc5130->config->state        = CONFIG_RESET;
	tmc5130->config->configIndex  = 0;

	return true;
}

// Restore the TMC5130 to the state stored in the shadow registers.
// This can be used to recover the IC configuration after a VM power loss.
uint8_t tmc5130_restore(TMC5130TypeDef *tmc5130)
{
	if(tmc5130->config->state != CONFIG_READY)
		return false;

	tmc5130->config->state        = CONFIG_RESTORE;
	tmc5130->config->configIndex  = 0;

	return true;
}

// Change the values the IC will be configured with when performing a reset.
void tmc5130_setRegisterResetState(TMC5130TypeDef *tmc5130, const int32_t *resetState)
{
	size_t i;
	for(i = 0; i < TMC5130_REGISTER_COUNT; i++)
	{
		tmc5130->registerResetState[i] = resetState[i];
	}
}

// Register a function to be called after completion of the configuration mechanism
void tmc5130_setCallback(TMC5130TypeDef *tmc5130, tmc5130_callback callback)
{
	tmc5130->config->callback = (tmc_callback_config) callback;
}

// Helper function: Configure the next register.
static void writeConfiguration(TMC5130TypeDef *tmc5130)
{
	uint8_t *ptr = &tmc5130->config->configIndex;
	const int32_t *settings;

	if(tmc5130->config->state == CONFIG_RESTORE)
	{
		settings = tmc5130->config->shadowRegister;
		// Find the next restorable register
		while((*ptr < TMC5130_REGISTER_COUNT) && !TMC_IS_RESTORABLE(tmc5130->registerAccess[*ptr]))
		{
			(*ptr)++;
		}
	}
	else
	{
		settings = tmc5130->registerResetState;
		// Find the next resettable register
		while((*ptr < TMC5130_REGISTER_COUNT) && !TMC_IS_RESETTABLE(tmc5130->registerAccess[*ptr]))
		{
			(*ptr)++;
		}
	}

	if(*ptr < TMC5130_REGISTER_COUNT)
	{
		tmc5130_writeInt(tmc5130, *ptr, settings[*ptr]);
		(*ptr)++;
	}
	else // Finished configuration
	{
		if(tmc5130->config->callback)
		{
			((tmc5130_callback)tmc5130->config->callback)(tmc5130, tmc5130->config->state);
		}

		tmc5130->config->state = CONFIG_READY;
	}
}

// Call this periodically
void tmc5130_periodicJob(TMC5130TypeDef *tmc5130, uint32_t tick)
{
	if(tmc5130->config->state != CONFIG_READY)
	{
		writeConfiguration(tmc5130);
		return;
	}

	int32_t XActual;
	uint32_t tickDiff;

	// Calculate velocity v = dx/dt
	if((tickDiff = tick - tmc5130->oldTick) >= 5)
	{
		XActual = tmc5130_readInt(tmc5130, TMC5130_XACTUAL);
		// ToDo CHECK 2: API Compatibility - write alternative algorithm w/o floating point? (LH)
		tmc5130->velocity = (int32_t) ((float32_t) ((XActual - tmc5130->oldX) / (float32_t) tickDiff) * (float32_t) 1048.576);

		tmc5130->oldX     = XActual;
		tmc5130->oldTick  = tick;
	}
}

// Rotate with a given velocity (to the right)
void tmc5130_rotate(TMC5130TypeDef *tmc5130, int32_t velocity)
{
	// Set absolute velocity
	tmc5130_writeInt(tmc5130, TMC5130_VMAX, abs(velocity));
	// Set direction
	tmc5130_writeInt(tmc5130, TMC5130_RAMPMODE, (velocity >= 0) ? TMC5130_MODE_VELPOS : TMC5130_MODE_VELNEG);
}

// Rotate to the right
void tmc5130_right(TMC5130TypeDef *tmc5130, uint32_t velocity)
{
	tmc5130_rotate(tmc5130, velocity);
}

// Rotate to the left
void tmc5130_left(TMC5130TypeDef *tmc5130, uint32_t velocity)
{
	tmc5130_rotate(tmc5130, -velocity);
}

// Stop moving
void tmc5130_stop(TMC5130TypeDef *tmc5130)
{
	tmc5130_rotate(tmc5130, 0);
}

// Move to a specified position with a given velocity
void tmc5130_moveTo(TMC5130TypeDef *tmc5130, int32_t position, uint32_t velocityMax)
{
	tmc5130_writeInt(tmc5130, TMC5130_RAMPMODE, TMC5130_MODE_POSITION);

	// VMAX also holds the target velocity in velocity mode.
	// Re-write the position mode maximum velocity here.
	tmc5130_writeInt(tmc5130, TMC5130_VMAX, velocityMax);

	tmc5130_writeInt(tmc5130, TMC5130_XTARGET, position);
}

// Move by a given amount with a given velocity
// This function will write the absolute target position to *ticks
void tmc5130_moveBy(TMC5130TypeDef *tmc5130, int32_t *ticks, uint32_t velocityMax)
{
	// determine actual position and add numbers of ticks to move
	*ticks += tmc5130_readInt(tmc5130, TMC5130_XACTUAL);

	tmc5130_moveTo(tmc5130, *ticks, velocityMax);
}
