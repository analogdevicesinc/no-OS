/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
#include "xstatus.h"
#include "xil_io.h"
#include "spi.h"
#include "cf_ad9739a.h"
#include "AD9739A.h"
#include "ADF4350.h"

void xil_printf(const char *ctrl1, ...);

/***************************************************************************//**
 * @brief Displays the state of the Mu controller and the state of the data
 *        receiver controller.
 *
 * @param mu_status - Mu controller status.
 * @param rc_status - Data receiver controller status.
 *
 * @return None.
*******************************************************************************/
void DisplayStatus(uint8_t mu_status, uint8_t rc_status)
{
	char* muStatMsg;
	char* rcStatMsg1;
	char* rcStatMsg2;

	if (mu_status & AD9739A_MU_STAT1_MU_LKD)
	{
		muStatMsg = "Locked";
	}
	else if (mu_status & AD9739A_MU_STAT1_MU_LST)
	{
		muStatMsg = "Not locked";
	}
	else
	{
		muStatMsg = "";
	}
	if (rc_status & AD9739A_LVDS_REC_STAT9_RCVR_LCK)
	{
		rcStatMsg1 = "Locked";
	}
	else
	{
		rcStatMsg1 = "Not locked";
	}
	if (rc_status & AD9739A_LVDS_REC_STAT9_RCVR_TRK_ON)
	{
		rcStatMsg2 = "Tracking";
	}
	else
	{
		rcStatMsg2 = "Not tracking";
	}
	xil_printf("AD9739A: Mu is %s. Rc is %s and %s.\r\n", muStatMsg, rcStatMsg1, rcStatMsg2);
	xil_printf("Press [q] to exit loop.\r\n\n\r");
}

/***************************************************************************//**
 * @brief Main function.
 *
 * @return 0.
*******************************************************************************/
int main(void)
{
	uint32_t mu_status;
	uint32_t rc_status;
	int32_t  ret;
	int32_t  error = 0;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

    /* Initialize the SPI peripheral */
	ret = SPI_Init(SPI_BASEADDR, 0, 0, 0);
	if(ret < 0)
	{
		return ret;
	}
	delay_ms(100);

	/* Configure ADF4350 device. */
	adf4350_setup(SPI_BASEADDR, 2);

	/* Configure AD9739A device. */
	ret = ad9739a_setup(SPI_BASEADDR, 1);

	/* Read the AD9739A part ID. */
	xil_printf("\n\r********************************************************************\r\n");
	xil_printf("  ADI AD9739A-FMC-EBZ Reference Design\n\r");
	xil_printf("  AD9739A PART ID: 0x%02x\n\r", ad9739a_read(AD9739A_REG_PART_ID));
	xil_printf("********************************************************************\r\n");
	if (ret < 0)
	{
		xil_printf("Error occurred during AD9739A setup.\n\r");
		xil_printf("Possible reason: wrong position of switch S1.\n\r");
		xil_printf("\n\rExit application.\n\r");
		while (1)
		{
			;
		}
	}
	else
	{
		xil_printf("AD9739A successfully configured.\n\r\n\r");
	}

	/* After Mu locked, program the dds/dma. */
	xil_printf("Enter [d] for DMA or any other key for DDS.\n\r");
	if (XUartLite_RecvByte(UART_BASEADDR) == 'd')
	{
		dma_setup();
	}
	else
	{
		dds_setup(300, 2500);
	}


	/* Continuously check the Mu controller status and RC status. */
	while (user_exit() == 0)
	{
		mu_status = ad9739a_read(AD9739A_REG_MU_STAT1);
		if (mu_status != AD9739A_MU_STAT1_MU_LKD)
		{
			xil_printf("AD9739A: Mu controller is not locked. Mu Status: 0x%02x(0x01)\n\r", mu_status);
			error = 1;
		}
		rc_status = ad9739a_read(AD9739A_REG_LVDS_REC_STAT9);
		if (rc_status != (AD9739A_LVDS_REC_STAT9_RCVR_TRK_ON | AD9739A_LVDS_REC_STAT9_RCVR_LCK))
		{
			xil_printf("AD9739A: Tracking not established or controller not locked.\r\n"
					   "         Data Receiver Controller Status 0x%02x(0x09)\n\r", rc_status);
			error = 1;
		}
		if (error == 1)
		{
			xil_printf("\n\rExit application.\n\r");
			break;
		}
		DisplayStatus(mu_status, rc_status);
		delay_ms(100);
	}

	while (1)
	{
		;
	}

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
