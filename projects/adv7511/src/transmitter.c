/***************************************************************************//**
 *   @file   transmitter.c
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "transmitter.h"
#include "tx_lib.h"
#include "cf_hdmi.h"
#include "xil_io.h"
#include "wrapper.h"
#include "edid.h"
#include "app_config.h"

/*****************************************************************************/
/******************* Macros and Constants Definitions ************************/
/*****************************************************************************/
#define TRANSMITTER_MODE_SWITCH_DELAY	500
/* This value defines the delay from the time   */
/* a mode change is detected to the time it is  */
/* reported to the application. It is used to   */
/* make sure the new mode is stable before it   */
/* is processed                                 */

#define TRANSMITTER_HOUSEKEEPING_DELAY	100
/* This value defines the delay (in ms) between */
/* any two consecutive calls to the repeater    */
/* housekeeping function. This function provide */
/* periodic check on hardware status to make    */
/* sure everything is in order. Smaller delay   */
/* result in better response but generate much  */
/* more i2c access. Recommended 100-200 ms      */

/*****************************************************************************/
/******************************* Variables ***********************************/
/*****************************************************************************/
static UINT32					HouseKeepingDelay;
static TRANSMITTER_OPER_MODE	LastDetMode;
static UINT32					ModeChngCount;
static struct axi_clkgen		*clk_gen;
TRANSMITTER_OPER_PARAMS			TransmitterParm;
UCHAR							MuteState;

TRANSMITTER_TX_VARS	TransmitterTxVars;

/**
 * @brief Link the transmitter clk_gen handler to the application one.
 * @param [in] clk_gen_handle - Pointer to the clk_axi_clkgen handle.
 * @return void
 */
void transmitter_link_clkgen(struct axi_clkgen *clk_gen_handle)
{
	clk_gen = clk_gen_handle;
}

/***************************************************************************//**
 * @brief Initializes the transmitter.
 *
 * @return Returns ATVERR_OK.
*******************************************************************************/
ATV_ERR ADIAPI_TransmitterInit(void)
{
	LastDetMode							= MODE_INVALID;
	TransmitterParm.Changed				= TRUE;
	TransmitterParm.Mode				= MODE_NONE;
	TransmitterParm.ReqOutputMode		= OUT_MODE_HDMI;
#if defined(PLATFORM_KC705) || defined(PLATFORM_ZC702) || \
	defined(PLATFORM_ZED)
	TransmitterParm.InPixelBitsPerColor = 8;
	TransmitterParm.InPixelFormat 		= SDR_422_SEP_SYNC;
#elif defined(PLATFORM_VC707)
	TransmitterParm.InPixelBitsPerColor = 12;
	TransmitterParm.InPixelFormat 		= SDR_444_SEP_SYNC;
#elif defined(PLATFORM_AC701) || defined(PLATFORM_ZC706)
	TransmitterParm.InPixelBitsPerColor = 8;
	TransmitterParm.InPixelFormat 		= SDR_444_SEP_SYNC;
#endif
	TransmitterParm.InPixelStyle 		= 2;
	TransmitterParm.InPixelAlignment 	= ALIGN_RIGHT;
	TransmitterParm.OutPixelEncFormat 	= OUT_ENC_RGB_444;
	TransmitterParm.InColorSpace 		= TX_CS_YUV_601;
	TransmitterParm.OutColorSpace 		= TX_CS_RGB;
	TransmitterParm.AudInterface		= TX_SPDIF;
	TransmitterParm.DebugControl		= 1;
	MuteState							= MUTE_ENABLE;

	TRANSMITTER_SoftwareInit();
	TRANSMITTER_HardwareInit();

	return ATVERR_OK;
}

/***************************************************************************//**
 * @brief Initializes the software parameters.
 *
 * @return None.
*******************************************************************************/
void TRANSMITTER_SoftwareInit(void)
{
	memset (&(TransmitterTxVars), 0, sizeof(TRANSMITTER_TX_VARS));

	TxStatus.Hpd        = FALSE;
	TxStatus.Msen       = FALSE;
}

/***************************************************************************//**
 * @brief Initializes the hardware of the transmitter.
 *
 * @return None.
*******************************************************************************/
void TRANSMITTER_HardwareInit(void)
{
	/* Initialize HDMI TX chip. */
	ADIAPI_TxInit(TRUE);

	/* Enable TMDS clock and data lines. */
	ADIAPI_TxEnableTmds (TRUE, TRUE);

	/* Set system mute. */
	ADIAPI_TxSetAvmute(TX_AVMUTE_OFF);
	ADIAPI_TransmitterSetMuteState();

	/* Set output mode. */
	ADIAPI_TxSetOutputMode(TransmitterParm.ReqOutputMode);

	/* Set input pixel data format. */
	ADIAPI_TxSetInputPixelFormat(TransmitterParm.InPixelBitsPerColor,
				     TransmitterParm.InPixelFormat,
				     TransmitterParm.InPixelStyle,
				     TransmitterParm.InPixelAlignment,
				     FALSE,
				     FALSE);

	/* Set output pixel format. */
	ADIAPI_TxSetOutputPixelFormat(TransmitterParm.OutPixelEncFormat,
				      TRUE);

	/* Set colour space conversion */
	ADIAPI_TxSetCSC(TransmitterParm.InColorSpace,
			TransmitterParm.OutColorSpace);

	/* Enable audio interface. */
	ADIAPI_TxSetAudioInterface(TransmitterParm.AudInterface,
				   AUD_SAMP_PKT,
				   1);

	/* Set interrupt masks. */
	ADIAPI_TxSetEnabledEvents (TX_EVENT_ALL_EVENTS, FALSE);
	ADIAPI_TxSetEnabledEvents ((TX_EVENT)(TX_EVENT_HPD_CHG |
					      TX_EVENT_MSEN_CHG |
					      TX_EVENT_EDID_READY),
				   TRUE);

	/* Enable AVI InfoFrame. */
	ADIAPI_TxEnablePackets(PKT_AV_INFO_FRAME,
			       TRUE);
}

/***************************************************************************//**
 * @brief Sets the transmitter power mode.
 *
 * @param pwrmode - transmitter power mode.
 *
 * @return Returns ATVERR_OK.
*******************************************************************************/
ATV_ERR ADIAPI_TransmitterSetPowerMode(TRANSMITTER_POWER_MODE pwrmode)
{
	TransmitterParm.PowerMode = pwrmode;

	return ATVERR_OK;
}

/***************************************************************************//**
 * @brief Gets the operating mode based on HDMI HPD and MSEN state.
 *
 * @param Mode - operating mode.
 *
 * @return Returns ATVERR_OK.
*******************************************************************************/
ATV_ERR ADIAPI_TransmitterGetDetectedMode(TRANSMITTER_OPER_MODE *Mode)
{
	BOOL Hpd;
	BOOL Msen;

	ADIAPI_TxGetHpdMsenState(&Hpd, &Msen);
	if(Hpd && Msen) {
		*Mode = MODE_XMT;
	} else {
		*Mode = MODE_NONE;
	}

	return ATVERR_OK;
}

/***************************************************************************//**
 * @brief Gets the current system operating mode.
 *
 * @param Mode - operating mode.
 *
 * @return Returns ATVERR_OK.
*******************************************************************************/
ATV_ERR ADIAPI_TransmitterGetOperatingMode(TRANSMITTER_OPER_MODE *Mode)
{
	*Mode = TransmitterParm.Mode;

	return ATVERR_OK;
}

/***************************************************************************//**
 * @brief Sets the system operating mode.
 *
 * @param Mode - operating mode.
 *
 * @return Returns ATVERR_OK.
*******************************************************************************/
ATV_ERR ADIAPI_TransmitterSetOperatingMode(TRANSMITTER_OPER_MODE Mode)
{
	if (TransmitterParm.Mode != Mode) {
		TransmitterParm.Mode = Mode;
		TransmitterParm.Changed = TRUE;
		DBG_MSG("APP: Changed system mode to ");
		if (TransmitterParm.Mode == MODE_XMT) {
			DBG_MSG("Transmitter\n\r");
		} else {
			DBG_MSG("Disconnected\n\r");
		}
	}

	return ATVERR_OK;
}


/***************************************************************************//**
 * @brief Sets the mute mode using the operating mode information.
 *
 * @param Mode - operating mode.
 *
 * @return None.
*******************************************************************************/
void ADIAPI_TransmitterSetMuteMode(TRANSMITTER_OPER_MODE Mode)
{
	if(Mode == MODE_XMT) {
		MuteState = MUTE_DISABLE;
	} else {
		MuteState = MUTE_ENABLE;
	}
}

/***************************************************************************//**
 * @brief Checks current AVR operating mode and notify application if mode
 * 		  changed.
 *
 * @return None.
*******************************************************************************/
void TRANSMITTER_MonitorAvrMode(void)
{
	TRANSMITTER_OPER_MODE Mode;

	ADIAPI_TransmitterGetDetectedMode(&Mode);
	if(Mode != LastDetMode) {
		LastDetMode = Mode;
		ModeChngCount = ATV_GetMsCountNZ();
	}
	if((Mode != TransmitterParm.Mode) && ModeChngCount) {
		if(ATV_GetElapsedMs(ModeChngCount, 0) > TRANSMITTER_MODE_SWITCH_DELAY) {
			ADIAPI_TransmitterSetOperatingMode(Mode);
			ADIAPI_TransmitterSetMuteMode(Mode);
			ModeChngCount = 0;
		}
	}
}

/***************************************************************************//**
 * @brief Main function for polling mode operation.
 * 		  This function should be called periodically (at least every 100 ms).
 *
 * @return None.
*******************************************************************************/
ATV_ERR ADIAPI_TransmitterMain(void)
{
	UCHAR Events;

	TRANSMITTER_MonitorAvrMode();
	Events = 0;
	if ( (TransmitterParm.PowerMode == REP_POWER_UP) && (TransmitterParm.Changed ||
			(ATV_GetElapsedMs(HouseKeepingDelay, 0) >= TRANSMITTER_HOUSEKEEPING_DELAY))) {
		TransmitterParm.Changed = FALSE;
		Events |= ADI_TASK_EVENT_TIMER;
		HouseKeepingDelay = HAL_GetCurrentMsCount();
	}
	Events &= ADI_TASK_EVENT_TIMER;
	if (ADIAPI_TxIntPending() == ATVERR_TRUE) {
		ADIAPI_TxIsr();
	}
	if ((Events & ADI_TASK_EVENT_TIMER)
	    && (TransmitterParm.PowerMode == REP_POWER_UP)) {
		TRANSMITTER_Housekeeping();
	}
	if((CurrMuteState == MUTE_DISABLE)
	    && (TransmitterParm.PowerMode == REP_POWER_UP)) {
		AudioClick();
	}

	return ATVERR_OK;
}

/***************************************************************************//**
 * @brief Performs TX housekeeping tasks.
 *		  This function should be called periodically (at least every 100 ms)
 *
 * @return None.
*******************************************************************************/
void TRANSMITTER_Housekeeping(void)
{
	/* Handle mute state. */
	ADIAPI_TransmitterSetMuteState();
}

/***************************************************************************//**
 * @brief This is the TX audio/video mute handling function.
 *
 * @return Returns ATVERR_OK.
*******************************************************************************/
ATV_ERR ADIAPI_TransmitterSetMuteState(void)
{
	if (MuteState != CurrMuteState) {
		CurrMuteState = MuteState;
		if(CurrMuteState) {
			TRANSMITTER_DBG_MSG("Mute audio and video.\n\r");
			ADIAPI_TxMuteAudio(TRUE);
			ADIAPI_TxMuteVideo(TRUE);
		} else {
			TRANSMITTER_DBG_MSG("Un-mute audio and video.\n\r");
			ADIAPI_TxMuteAudio(FALSE);
			ADIAPI_TxMuteVideo(FALSE);
		}
	}

	return ATVERR_OK;
}

/***************************************************************************//**
 * @brief Performs the notification operations.
 *
 * @return Returns 0.
*******************************************************************************/
UINT16 TRANSMITTER_Notification (TX_EVENT Ev, UINT16 Count, void *BufPtr)
{
	switch (Ev) {
	/* HPD changed */
	case TX_EVENT_HPD_CHG:
		TxStatus.Hpd = *((BOOL *)BufPtr);
		TRANSMITTER_DBG_MSG("HPD changed to %s\n\r", TxStatus.Hpd? "HI": "LOW");
		if (TxStatus.Hpd) {
			TRANSMITTER_HardwareInit();
		} else {
			TRANSMITTER_SoftwareInit();
		}
		break;
	/* MSEN changed */
	case TX_EVENT_MSEN_CHG:
		TxStatus.Msen = *((BOOL *)BufPtr);
		TRANSMITTER_DBG_MSG("MSEN changed to %s\n\r", TxStatus.Msen? "HI": "LOW");
		break;
	/* EDID ready */
	case TX_EVENT_EDID_READY:
		TRANSMITTER_DBG_MSG("A new EDID segment was read.\n\r");
		TRANSMITTER_NewEdidSegment(Count, BufPtr);
		break;
	default:
		break;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Parse the new EDID segment and set the resolution and the pixel clock
 * 		  according to EDID data.
 *
 * @param SegmentNum - Segment number.
 * @param SegPtr - Segment pointer.
 *
 * @return None.
*******************************************************************************/
void TRANSMITTER_NewEdidSegment(UINT16 SegmentNum, UCHAR *SegPtr)
{
	UCHAR  		   EdidData[256];
	UINT16 		   SpaOffset;
	struct edid_struct    *Edid;
	struct std_timing     *TDesc;
	unsigned short horizontalActiveTime     = 0;
	unsigned short verticalActiveTime       = 0;
	unsigned short horizontalBlankingTime   = 0;
	unsigned short verticalBlankingTime     = 0;
	unsigned short horizontalSyncPulseWidth = 0;
	unsigned short verticalSyncPulseWidth   = 0;
	unsigned short horizontalSyncOffset     = 0;
	unsigned short verticalSyncOffset       = 0;
	unsigned long  pixelClk                 = 0;
	unsigned char  edidIndex                = 0;
	unsigned long  ieeeRegistration         = 0;

	if (SegPtr) {
		memcpy (EdidData, SegPtr, 256);
	} else {
		memset (EdidData, 0, 256);
	}
	if (SegPtr && (SegmentNum < 2)) {
		for(edidIndex = 128; edidIndex <= 253; edidIndex++) {
			ieeeRegistration = ((unsigned long)EdidData[edidIndex + 2] << 16) |
					   ((unsigned short)EdidData[edidIndex + 1] << 8) |
					   EdidData[edidIndex];
			if(ieeeRegistration == HDMI_IEEE_REG) {
				break;
			}
		}
		if(ieeeRegistration == HDMI_IEEE_REG) {
			TRANSMITTER_DBG_MSG("HDMI device.\n\r");
			ADIAPI_TxSetOutputMode(OUT_MODE_HDMI);
		} else {
			TRANSMITTER_DBG_MSG("DVI device.\n\r");
			ADIAPI_TxSetOutputMode(OUT_MODE_DVI);
		}
		Edid = (struct edid_struct *)EdidData;
		TDesc = (struct std_timing *)(Edid->detailed_timing);

		pixelClk = (TDesc->pixel_clk[1] << 8) | TDesc->pixel_clk[0];
		pixelClk = pixelClk * 10000;
		axi_clkgen_set_rate(clk_gen, pixelClk);

		horizontalActiveTime = ((TDesc->h_act_blnk44 & 0xf0) << 4) |
				       TDesc->h_active;
		verticalActiveTime = ((TDesc->v_act_blnk44 & 0xf0) << 4) |
				     TDesc->v_active;
		horizontalBlankingTime = ((TDesc->h_act_blnk44 & 0x0f) << 8) |
					 TDesc->h_blanking;
		verticalBlankingTime = ((TDesc->v_act_blnk44 & 0x0f) << 8) |
				       TDesc->v_blanking;
		horizontalSyncPulseWidth = ((TDesc->hv_offs_pulse & 0x30) << 4) |
					   TDesc->h_sync_width;
		verticalSyncPulseWidth = ((TDesc->hv_offs_pulse & 0x03) << 4) |
					 (TDesc->v_offs_pulse & 0x0f);
		horizontalSyncOffset = ((TDesc->hv_offs_pulse & 0xC0) << 2) |
				       TDesc->h_sync_offs;
		verticalSyncOffset = ((TDesc->hv_offs_pulse & 0x0C) << 2) |
				     ((TDesc->v_offs_pulse & 0xf0) >> 4);
		InitHdmiVideoPcore(horizontalActiveTime,
				   horizontalBlankingTime,
				   horizontalSyncOffset,
				   horizontalSyncPulseWidth,
				   verticalActiveTime,
				   verticalBlankingTime,
				   verticalSyncOffset,
				   verticalSyncPulseWidth);

		ADIAPI_mw_edid_enable_debug_msg(TRUE);
		ADIAPI_mw_edid_parse(EdidData, &SpaOffset, SegmentNum);
	}
}
