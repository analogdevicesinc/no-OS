/***************************************************************************//**
 *   @file   main.c
 *   @brief  Implementation of Main Function.
********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include "mykonos.h"
#include "myk_clock_init.h"
#include "myk_init.h"
#include "mykonos_m3.h"
#include "platform_drivers.h"

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main(void)
{
	mykonosErr_t mykError;
	ADI_ERR ad9528Error;
	uint8_t pllLockStatus;
	uint8_t arm_major;
	uint8_t arm_minor;
	uint8_t arm_release;
	uint32_t status;

	/* Performs a hard reset on the AD9528 DUT */
	ad9528Error = AD9528_resetDevice(&ad9528Clock);
	if (ad9528Error != ADIERR_OK) {
		printf("AD9528_resetDevice() failed\n");
		return -1;
	}

	ad9528Error = AD9528_initialize(&ad9528Clock);
	if (ad9528Error != ADIERR_OK) {
		printf("AD9528_initialize() failed\n");
		return -1;
	}

    /* Perform a hard reset on the MYKONOS DUT (Toggle RESETB pin on device) */
    mykError = MYKONOS_resetDevice(&mykDevice);
	if (mykError != MYKONOS_ERR_OK) {
		printf("MYKONOS_resetDevice() failed\n");
		return -1;
	}

    /* Initialize the Mykonos device based on the desired device settings */
    mykError = MYKONOS_initialize(&mykDevice);
	if (mykError != MYKONOS_ERR_OK) {
		printf("MYKONOS_initialize() failed\n");
		return -1;
	}

	/* Checks if CLKPLL is locked */
	mykError = MYKONOS_checkPllsLockStatus(&mykDevice,
										   &pllLockStatus);
	if (mykError != MYKONOS_ERR_OK) {
		printf("MYKONOS_checkPllsLockStatus() failed\n");
		return -1;
	} else
		if (!(pllLockStatus & 0x01)) {
			printf("MYKONOS_checkPllsLockStatus() failed\n");
			return -1;
		}

	/* Reset the ARM processor and perform initialization */
	mykError = MYKONOS_initArm(&mykDevice);
	if (mykError != MYKONOS_ERR_OK) {
		printf("MYKONOS_initArm() failed\n");
		return -1;
	}

	/* Loads binary byte array into ARM program memory */
	mykError = MYKONOS_loadArmFromBinary(&mykDevice,
										 &firmware_Mykonos_M3_bin[0],
										 firmware_Mykonos_M3_bin_len);
	if (mykError != MYKONOS_ERR_OK) {
		printf("MYKONOS_loadArmFromBinary() failed\n");
		return -1;
	}

	/* Read back the version of the ARM binary loaded into the Mykonos ARM memory */
	mykError = MYKONOS_getArmVersion(&mykDevice,
			&arm_major, &arm_minor, &arm_release);
	if (mykError != MYKONOS_ERR_OK) {
		printf("MYKONOS_loadArmFromBinary() failed\n");
		return -1;
	} else
		printf("AD9371 ARM version %d.%d.%d\n",
					arm_major, arm_minor, arm_release);

	/* Initialize the CLKGENs */
	status = clkgen_setup();
	if (status != 0) {
		printf("clkgen_setup() failed\n");
		return -1;
	}

	/* Initialize the ADXCVRs */
	status = xcvr_setup();
	if (status != 0) {
		printf("xcvr_setup() failed\n");
		return -1;
	}

	printf("Done\n");

	return 0;
}
