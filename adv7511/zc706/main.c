/***************************************************************************//**
 *   @file   main.c
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include "xil_cache.h"
#include "xbasic_types.h"
#include "xil_io.h"
#include "cf_hdmi.h"
#include "atv_platform.h"
#include "transmitter.h"
#include "xil_exception.h"
#include "xuartps.h"

extern void delay_ms(u32 ms_count);
extern char inbyte(void);

/******************************************************************************/
/************************** Macros Definitions ********************************/
/******************************************************************************/
#define HDMI_CALL_INTERVAL_MS	10			/* Interval between two         */
											/* iterations of the main loop  */
#define DBG_MSG                 xil_printf

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
static UCHAR    MajorRev;      /* Major Release Number */
static UCHAR    MinorRev;      /* Usually used for code-drops */
static UCHAR    RcRev;         /* Release Candidate Number */
static BOOL     DriverEnable;
static BOOL     LastEnable;

/***************************************************************************//**
 * @brief Enables the driver.
 *
 * @return Returns ATVERR_OK.
*******************************************************************************/
void APP_EnableDriver (BOOL Enable)
{
    DriverEnable = Enable;
}

/***************************************************************************//**
 * @brief Returns the driver enable status.
 *
 * @return Returns the driver enable status.
*******************************************************************************/
static BOOL APP_DriverEnabled (void)
{
    if ((DriverEnable && HAL_GetMBSwitchState()) != LastEnable)
    {
        LastEnable = DriverEnable && HAL_GetMBSwitchState();
        DBG_MSG ("APP: Driver %s\n\r", LastEnable? "Enabled": "Disabled");
    }
    return (LastEnable);
}

/***************************************************************************//**
 * @brief Displays the application version and the chip revision.
 *
 * @return None.
*******************************************************************************/
static void APP_PrintRevisions (void)
{
	UINT16 TxRev;

	ADIAPI_TxGetChipRevision(&TxRev);

	DBG_MSG("\n\r********************************************************************\r\n");
	DBG_MSG("  ADI HDMI Trasmitter Application Ver R%d.%d.%d\n\r", MajorRev, MinorRev, RcRev);
	DBG_MSG("  HDMI-TX:  ADV7511 Rev 0x%x\r\n", TxRev);
	DBG_MSG("  Created:  %s At %s\n\r", __DATE__, __TIME__);
	DBG_MSG("********************************************************************\r\n\n\r");
}

/***************************************************************************//**
 * @brief Changes the video resolution.
 *
 * @return None.
*******************************************************************************/
static void APP_ChangeResolution (void)
{
	char *resolutions[7] = {"640x480", "800x600", "1024x768", "1280x720", "1360x768", "1600x900", "1920x1080"};
	char receivedChar    = 0;

	if(XUartPs_IsReceiveData(UART_BASEADDR))
	{
		receivedChar = inbyte();
		if((receivedChar >= 0x30) && (receivedChar <= 0x36))
		{
			SetVideoResolution(receivedChar - 0x30);
			DBG_MSG("Resolution was changed to %s \r\n", resolutions[receivedChar - 0x30]);
		}
		else
		{
			if((receivedChar != 0x0A) && (receivedChar != 0x0D))
			{
				SetVideoResolution(RESOLUTION_640x480);
				DBG_MSG("Resolution was changed to %s \r\n", resolutions[0]);
			}
		}
	}
}

/***************************************************************************//**
 * @brief Main function.
 *
 * @return Returns 0.
*******************************************************************************/
int main()
{
	UINT32 StartCount;

	MajorRev     = 1;
	MinorRev     = 1;
	RcRev        = 1;
	DriverEnable = TRUE;
	LastEnable   = FALSE;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

#ifdef XPAR_AXI_IIC_0_BASEADDR
	HAL_PlatformInit(XPAR_AXI_IIC_0_BASEADDR,	/* Perform any required platform init */
					 XPAR_SCUTIMER_DEVICE_ID,	/* including hardware reset to HDMI devices */
					 XPAR_SCUGIC_SINGLE_DEVICE_ID,
					 XPAR_SCUTIMER_INTR);
#else
	HAL_PlatformInit(XPAR_AXI_IIC_MAIN_BASEADDR,	/* Perform any required platform init */
					 XPAR_SCUTIMER_DEVICE_ID,	/* including hardware reset to HDMI devices */
					 XPAR_SCUGIC_SINGLE_DEVICE_ID,
					 XPAR_SCUTIMER_INTR);
#endif

	Xil_ExceptionEnable();

	SetVideoResolution(RESOLUTION_640x480);
	InitHdmiAudioPcore();

	APP_PrintRevisions();       /* Display S/W and H/W revisions */

	DBG_MSG("To change the video resolution press:\r\n");
	DBG_MSG("  '0' - 640x480;  '1' - 800x600;  '2' - 1024x768; '3' - 1280x720 \r\n");
	DBG_MSG("  '4' - 1360x768; '5' - 1600x900; '6' - 1920x1080.\r\n");

	ADIAPI_TransmitterInit();   /* Initialize ADI repeater software and h/w */

	ADIAPI_TransmitterSetPowerMode(REP_POWER_UP);

	StartCount = HAL_GetCurrentMsCount();

	while(1)
	{
		if (ATV_GetElapsedMs (StartCount, NULL) >= HDMI_CALL_INTERVAL_MS)
		{
			StartCount = HAL_GetCurrentMsCount();
			if (APP_DriverEnabled())
			{
				ADIAPI_TransmitterMain();
			}
		}
		APP_ChangeResolution();
	}

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return(0);
}
