/***************************************************************************//**
 *   @file   transmitter.h
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

#ifndef _TRANSMITTER_H_
#define _TRANSMITTER_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "transmitter_defs.h"
#include "clk_axi_clkgen.h"

/******************************************************************************/
/******************* Macros and Variables Definitions *************************/
/******************************************************************************/
#define ADI_TASK_EVENT_TIMER	0x1
#define TRANSMITTER_DBG_MSG		if(TransmitterParm.DebugControl)DBG_MSG

typedef struct {
	UCHAR			CurrMuteState;	/* Current mute state of A/V        */
	TX_STATUS_PKT	TxStatus;		/* State of TX hardware */
} TRANSMITTER_TX_VARS;

extern TRANSMITTER_TX_VARS	TransmitterTxVars;

#define CurrMuteState			TransmitterTxVars.CurrMuteState
#define TxStatus				TransmitterTxVars.TxStatus

#define HDMI_IEEE_REG			0x000C03

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Link the transmitter clk_gen handler to the application one. */
void transmitter_link_clkgen(struct axi_clkgen *clk_gen_handle);

/*! Initializes the transmitter. */
ATV_ERR ADIAPI_TransmitterInit(void);

/*! Initializes the software parameters. */
void TRANSMITTER_SoftwareInit(void);

/*! Initializes the hardware of the transmitter. */
void TRANSMITTER_HardwareInit(void);

/*! Sets the transmitter power mode. */
ATV_ERR ADIAPI_TransmitterSetPowerMode(TRANSMITTER_POWER_MODE pwrmode);

/*! Gets the operating mode based on HDMI HPD and MSEN state. */
ATV_ERR ADIAPI_TransmitterGetDetectedMode(TRANSMITTER_OPER_MODE *Mode);

/*! Gets the current system operating mode. */
ATV_ERR ADIAPI_TransmitterGetOperatingMode(TRANSMITTER_OPER_MODE *Mode);

/*! Sets the system operating mode. */
ATV_ERR ADIAPI_TransmitterSetOperatingMode(TRANSMITTER_OPER_MODE Mode);

/*! @brief Sets the mute mode using the operating mode information. */
void ADIAPI_TransmitterSetMuteMode(TRANSMITTER_OPER_MODE Mode);

/*! Checks current AVR operating mode and notify application if mode changed. */
void TRANSMITTER_MonitorAvrMode(void);

/*! @brief Main function for polling mode operation. */
ATV_ERR ADIAPI_TransmitterMain(void);

/*! Performs TX housekeeping tasks. */
void TRANSMITTER_Housekeeping(void);

/*! @brief This is the TX audio/video mute handling function. */
ATV_ERR ADIAPI_TransmitterSetMuteState(void);

/*! Performs the notification operations. */
UINT16 TRANSMITTER_Notification (TX_EVENT Ev, UINT16 Count, void *BufPtr);

/*! @brief Parse the new EDID segment. */
void TRANSMITTER_NewEdidSegment(UINT16 SegmentNum, UCHAR *SegPtr);

#endif
