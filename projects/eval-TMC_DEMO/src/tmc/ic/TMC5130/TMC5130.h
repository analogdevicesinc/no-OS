/*******************************************************************************
* Copyright © 2017 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef TMC_IC_TMC5130_H_
#define TMC_IC_TMC5130_H_

#include "tmc/helpers/API_Header.h"
#include "TMC5130_Register.h"
#include "TMC5130_Constants.h"
#include "TMC5130_Fields.h"

// Helper macros
#define TMC5130_FIELD_READ(tdef, address, mask, shift) \
	FIELD_GET(tmc5130_readInt(tdef, address), mask, shift)
#define TMC5130_FIELD_WRITE(tdef, address, mask, shift, value) \
	(tmc5130_writeInt(tdef, address, FIELD_SET(tmc5130_readInt(tdef, address), mask, shift, value)))

// Typedefs
typedef struct
{
	ConfigurationTypeDef *config;
	int32_t velocity, oldX;
	uint32_t oldTick;
	int32_t registerResetState[TMC5130_REGISTER_COUNT];
	uint8_t registerAccess[TMC5130_REGISTER_COUNT];
} TMC5130TypeDef;

typedef void (*tmc5130_callback)(TMC5130TypeDef*, ConfigState);

// Default Register values
#define R10 0x00071703  // IHOLD_IRUN
#define R26 0x00003DE9  // AMAX
#define R28 0x00003DE9  // DMAX
#define R3A 0x00010000  // ENC_CONST

#define R60 0xAAAAB554  // MSLUT[0]
#define R61 0x4A9554AA  // MSLUT[1]
#define R62 0x24492929  // MSLUT[2]
#define R63 0x10104222  // MSLUT[3]
#define R64 0xFBFFFFFF  // MSLUT[4]
#define R65 0xB5BB777D  // MSLUT[5]
#define R66 0x49295556  // MSLUT[6]
#define R67 0x00404222  // MSLUT[7]
#define R68 0xFFFF8056  // MSLUTSEL
#define R69 0x00F70000  // MSLUTSTART
#define R6C 0x000101D5  // CHOPCONF
#define R70 0x000504C8  // PWMCONF

static const int32_t tmc5130_defaultRegisterResetState[TMC5130_REGISTER_COUNT] =
{
//	0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   A,   B,   C,   D,   E,   F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x00 - 0x0F
	R10, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x10 - 0x1F
	0,   0,   0,   0,   0,   0, R26,   0, R28,   0,   0,   0,   0,   0,   0,   0, // 0x20 - 0x2F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   R3A, 0,   0,   0,   0,   0, // 0x30 - 0x3F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x40 - 0x4F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x50 - 0x5F
	R60, R61, R62, R63, R64, R65, R66, R67, R68, R69, 0,   0,   R6C, 0,   0,   0, // 0x60 - 0x6F
	R70, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x70 - 0x7F
};

// Undefine the default register values.
// This prevents warnings in case multiple TMC-API chip headers are included at once
#undef R10
#undef R3A
#undef R6C

// Register access permissions:
//   0x00: none (reserved)
//   0x01: read
//   0x02: write
//   0x03: read/write
//   0x13: read/write, separate functions/values for reading or writing
//   0x21: read, flag register (read to clear)
//   0x42: write, has hardware presets on reset
static const uint8_t tmc5130_defaultRegisterAccess[TMC5130_REGISTER_COUNT] =
{
//  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	0x03, 0x21, 0x01, 0x02, 0x13, 0x02, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, // 0x00 - 0x0F
	0x02, 0x02, 0x01, 0x02, 0x02, 0x02, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, // 0x10 - 0x1F
	0x03, 0x03, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, ____, 0x02, 0x02, 0x02, 0x03, ____, ____, // 0x20 - 0x2F
	____, ____, ____, 0x02, 0x03, 0x21, 0x01, ____, 0x03, 0x03, 0x02, 0x21, 0x01, ____, ____, ____, // 0x30 - 0x3F
	____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, // 0x40 - 0x4F
	____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, // 0x50 - 0x5F
	0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x01, 0x01, 0x03, 0x02, 0x02, 0x01, // 0x60 - 0x6F
	0x42, 0x01, 0x02, 0x01, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____  // 0x70 - 0x7F
};

// Register constants (only required for 0x42 registers, since we do not have
// any way to find out the content but want to hold the actual value in the
// shadow register so an application (i.e. the TMCL IDE) can still display
// the values. This only works when the register content is constant.
static const TMCRegisterConstant tmc5130_RegisterConstants[] =
{		// Use ascending addresses!
		{ 0x60, 0xAAAAB554 }, // MSLUT[0]
		{ 0x61, 0x4A9554AA }, // MSLUT[1]
		{ 0x62, 0x24492929 }, // MSLUT[2]
		{ 0x63, 0x10104222 }, // MSLUT[3]
		{ 0x64, 0xFBFFFFFF }, // MSLUT[4]
		{ 0x65, 0xB5BB777D }, // MSLUT[5]
		{ 0x66, 0x49295556 }, // MSLUT[6]
		{ 0x67, 0x00404222 }, // MSLUT[7]
		{ 0x68, 0xFFFF8056 }, // MSLUTSEL
		{ 0x69, 0x00F70000 }, // MSLUTSTART
		{ 0x70, 0x00050480 }  // PWMCONF
};

// API Functions
// All functions act on one IC identified by the TMC5130TypeDef pointer

void tmc5130_writeDatagram(TMC5130TypeDef *tmc5130, uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4);
void tmc5130_writeInt(TMC5130TypeDef *tmc5130, uint8_t address, int32_t value);
int32_t tmc5130_readInt(TMC5130TypeDef *tmc5130, uint8_t address);

void tmc5130_init(TMC5130TypeDef *tmc5130, uint8_t channel, ConfigurationTypeDef *config, const int32_t *registerResetState);
void tmc5130_fillShadowRegisters(TMC5130TypeDef *tmc5130);
uint8_t tmc5130_reset(TMC5130TypeDef *tmc5130);
uint8_t tmc5130_restore(TMC5130TypeDef *tmc5130);
void tmc5130_setRegisterResetState(TMC5130TypeDef *tmc5130, const int32_t *resetState);
void tmc5130_setCallback(TMC5130TypeDef *tmc5130, tmc5130_callback callback);
void tmc5130_periodicJob(TMC5130TypeDef *tmc5130, uint32_t tick);

void tmc5130_rotate(TMC5130TypeDef *tmc5130, int32_t velocity);
void tmc5130_right(TMC5130TypeDef *tmc5130, uint32_t velocity);
void tmc5130_left(TMC5130TypeDef *tmc5130, uint32_t velocity);
void tmc5130_stop(TMC5130TypeDef *tmc5130);
void tmc5130_moveTo(TMC5130TypeDef *tmc5130, int32_t position, uint32_t velocityMax);
void tmc5130_moveBy(TMC5130TypeDef *tmc5130, int32_t *ticks, uint32_t velocityMax);

#endif /* TMC_IC_TMC5130_H_ */
