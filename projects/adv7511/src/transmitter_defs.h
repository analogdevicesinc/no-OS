/***************************************************************************//**
 *   @file   transmitter_defs.h
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _TRANSMITTER_DEFS_H_
#define _TRANSMITTER_DEFS_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "tx_lib.h"

/******************************************************************************/
/******************* Macros and Constants Definitions *************************/
/******************************************************************************/
#define MUTE_DISABLE			0x0
#define MUTE_ENABLE				0x1

/******************************************************************************/
/************************** Types Declarations ********************************/
/******************************************************************************/
typedef enum {
	MODE_XMT,           /* Transmitter mode */
	MODE_NONE,          /* None */
	MODE_INVALID        /* Invalid value, for internal use only */
} TRANSMITTER_OPER_MODE;

typedef enum {
	REP_POWER_DOWN,
	REP_POWER_UP
} TRANSMITTER_POWER_MODE;

typedef enum {
	AUD_IN_I2S0,
	AUD_IN_I2S1,
	AUD_IN_I2S2,
	AUD_IN_I2S3,
	AUD_IN_I2S,
	AUD_IN_SPDIF,
	AUD_IN_DSD0,
	AUD_IN_DSD1,
	AUD_IN_DSD2,
	AUD_IN_DSD3,
	AUD_IN_DSD4,
	AUD_IN_DSD5,
	AUD_IN_DSD6,
	AUD_IN_DSD7,
	AUD_IN_DSD,
	AUD_IN_DST,
	AUD_IN_ALL
} TRANSMITTER_AUD_INTERFACE;

typedef enum {
	OUT_MODE_HDMI,
	OUT_MODE_DVI
} TRANSMITTER_OUTPUT_MODE;

typedef struct {
	BOOL						Changed;
	TRANSMITTER_OPER_MODE		Mode;
	TRANSMITTER_POWER_MODE		PowerMode;
	TRANSMITTER_OUTPUT_MODE		ReqOutputMode;
	UCHAR						InPixelBitsPerColor;
	TX_IN_FORMAT				InPixelFormat;
	UCHAR						InPixelStyle;
	TX_CHAN_ALIGN				InPixelAlignment;
	TX_OUT_ENCODING				OutPixelEncFormat;
	TX_CS_MODE					InColorSpace;
	TX_CS_MODE					OutColorSpace;
	TRANSMITTER_AUD_INTERFACE	AudInterface;
	UINT16						DebugControl;
} TRANSMITTER_OPER_PARAMS;

typedef struct {
	BOOL    Hpd;
	BOOL    Msen;
} TX_STATUS_PKT;

#endif
