/***************************************************************************//**
 *   @file   transmitter_defs.h
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

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
