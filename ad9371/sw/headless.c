/***************************************************************************//**
 *   @file   headless.c
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
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "ad9528.h"
#include "mykonos.h"
#include "mykonos_m3.h"
#include "mykonos_gpio.h"
#include "platform_drivers.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
extern ad9528Device_t	clockAD9528_;
extern mykonosDevice_t	mykDevice;

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main(void)
{
	ADI_ERR				ad9528Error;
	mykonosErr_t		mykError;
	const char			*errorString;
	uint8_t				pllLockStatus;
	uint8_t				mcsStatus;
	uint8_t				arm_major;
	uint8_t				arm_minor;
	uint8_t				arm_release;
	mykonosGpioErr_t	mykGpioErr;
	uint32_t			initCalMask = TX_BB_FILTER | ADC_TUNER |
						  TIA_3DB_CORNER | DC_OFFSET | TX_ATTENUATION_DELAY |
						  RX_GAIN_DELAY | FLASH_CAL | RX_LO_DELAY | RX_QEC_INIT;
	uint8_t				errorFlag = 0;
	uint8_t				errorCode = 0;
	uint32_t			initCalsCompleted;
	uint8_t				framerStatus;
	uint8_t				obsFramerStatus;
	uint8_t				deframerStatus;
	uint32_t			trackingCalMask = TRACK_ORX1_QEC | TRACK_ORX2_QEC;
	uint32_t			status;

	/* Allocating memory for the errorString */
	errorString = (const char*) malloc(sizeof(char) * 200);

	printf("Please wait...\n");

	platform_init();

	/**************************************************************************/
	/*****      System Clocks Initialization Initialization Sequence      *****/
	/**************************************************************************/

	/* Perform a hard reset on the AD9528 DUT */
	ad9528Error = AD9528_resetDevice(&clockAD9528_);
	if (ad9528Error != ADIERR_OK) {
		printf("AD9528_resetDevice() failed\n");
		return ad9528Error;
	}

	/* Initialize the AD9528 by writing all SPI registers */
	ad9528Error = AD9528_initialize(&clockAD9528_);
	if (ad9528Error != ADIERR_OK) {
		printf("AD9528_initialize() failed\n");
		return ad9528Error;
	}

	/*************************************************************************/
	/*****                Mykonos Initialization Sequence                *****/
	/*************************************************************************/

	/* Perform a hard reset on the MYKONOS DUT (Toggle RESETB pin on device) */
	if ((mykError = MYKONOS_resetDevice(&mykDevice)) != MYKONOS_ERR_OK) {
		errorString = getMykonosErrorMessage(mykError);
		goto error;
	}

	if ((mykError = MYKONOS_initialize(&mykDevice)) != MYKONOS_ERR_OK)
	{
	    errorString = getMykonosErrorMessage(mykError);
	    goto error;
	}

	/*************************************************************************/
	/*****                Mykonos CLKPLL Status Check                    *****/
	/*************************************************************************/

	if ((mykError = MYKONOS_checkPllsLockStatus(&mykDevice, &pllLockStatus)) != MYKONOS_ERR_OK) {
	    errorString = getMykonosErrorMessage(mykError);
	    goto error;
    }

	/*************************************************************************/
	/*****                Mykonos Perform MultiChip Sync                 *****/
	/*************************************************************************/

	if ((mykError = MYKONOS_enableMultichipSync(&mykDevice, 1, &mcsStatus)) != MYKONOS_ERR_OK) {
	    errorString = getMykonosErrorMessage(mykError);
	    goto error;
    }

	/* Minimum 3 SYSREF pulses from Clock Device has to be produced for MulticChip Sync */
	AD9528_requestSysref(&clockAD9528_, 1);
	mdelay(1);
	AD9528_requestSysref(&clockAD9528_, 1);
	mdelay(1);
	AD9528_requestSysref(&clockAD9528_, 1);
	mdelay(1);
	AD9528_requestSysref(&clockAD9528_, 1);
	mdelay(1);

	/*************************************************************************/
	/*****                Mykonos Verify MultiChip Sync                 *****/
	/*************************************************************************/

	if ((mykError = MYKONOS_enableMultichipSync(&mykDevice, 0, &mcsStatus)) != MYKONOS_ERR_OK) {
	    errorString = getMykonosErrorMessage(mykError);
	    goto error;
    }

	if ((mcsStatus & 0x0B) == 0x0B)
		printf("MCS successful\n");
	else
		printf("MCS failed\n");

	/*************************************************************************/
	/*****                Mykonos Load ARM file                          *****/
	/*************************************************************************/

	if (pllLockStatus & 0x01) {
		printf("CLKPLL locked\n");
		if ((mykError = MYKONOS_initArm(&mykDevice)) != MYKONOS_ERR_OK) {
		    errorString = getMykonosErrorMessage(mykError);
		    goto error;
        }

		if ((mykError = MYKONOS_loadArmFromBinary(&mykDevice, &firmware_Mykonos_M3_bin[0], firmware_Mykonos_M3_bin_len)) != MYKONOS_ERR_OK) {
			errorString = getMykonosErrorMessage(mykError);
			goto error;
		}
	} else {
		printf("CLKPLL not locked (0x%x)\n", pllLockStatus);
		return -1;
	}

	/* Read back the version of the ARM binary loaded into the Mykonos ARM memory */
	if ((mykError = MYKONOS_getArmVersion(&mykDevice, &arm_major, &arm_minor, &arm_release)) == MYKONOS_ERR_OK)
		printf("AD9371 ARM version %d.%d.%d\n", arm_major, arm_minor, arm_release);

	/*************************************************************************/
	/*****                Mykonos Set RF PLL Frequencies                 *****/
	/*************************************************************************/

	if ((mykError = MYKONOS_setRfPllFrequency(&mykDevice, RX_PLL, mykDevice.rx->rxPllLoFrequency_Hz)) != MYKONOS_ERR_OK) {
        errorString = getMykonosErrorMessage(mykError);
		goto error;
    }

	if ((mykError = MYKONOS_setRfPllFrequency(&mykDevice, TX_PLL, mykDevice.tx->txPllLoFrequency_Hz)) != MYKONOS_ERR_OK) {
        errorString = getMykonosErrorMessage(mykError);
		goto error;
    }

	if ((mykError = MYKONOS_setRfPllFrequency(&mykDevice, SNIFFER_PLL, mykDevice.obsRx->snifferPllLoFrequency_Hz)) != MYKONOS_ERR_OK) {
	    errorString = getMykonosErrorMessage(mykError);
		goto error;
    }

	/* Wait 200ms for PLLs to lock */
	mdelay(200);

	if ((mykError = MYKONOS_checkPllsLockStatus(&mykDevice, &pllLockStatus)) != MYKONOS_ERR_OK) {
        errorString = getMykonosErrorMessage(mykError);
		goto error;
    }

	if ((pllLockStatus & 0x0F) == 0x0F)
		printf("PLLs locked\n");
	else {
		printf("PLLs not locked (0x%x)\n", pllLockStatus);
		return -1;
	}

	/*************************************************************************/
	/*****                Mykonos Set GPIOs                              *****/
	/*************************************************************************/

	if ((mykGpioErr = MYKONOS_setRx1GainCtrlPin(&mykDevice, 0, 0, 0, 0, 0)) != MYKONOS_ERR_GPIO_OK) {
	    errorString = getGpioMykonosErrorMessage(mykGpioErr);
	    goto error;
    }

	if ((mykGpioErr = MYKONOS_setRx2GainCtrlPin(&mykDevice, 0, 0, 0, 0, 0)) != MYKONOS_ERR_GPIO_OK) {
	    errorString = getGpioMykonosErrorMessage(mykGpioErr);
	    goto error;
    }

	if ((mykGpioErr = MYKONOS_setTx1AttenCtrlPin(&mykDevice, 0, 0, 0, 0, 0)) != MYKONOS_ERR_GPIO_OK) {
	    errorString = getGpioMykonosErrorMessage(mykGpioErr);
	    goto error;
    }

	if ((mykGpioErr = MYKONOS_setTx2AttenCtrlPin(&mykDevice, 0, 0, 0, 0)) != MYKONOS_ERR_GPIO_OK) {
	    errorString = getGpioMykonosErrorMessage(mykGpioErr);
	    goto error;
    }

	if ((mykGpioErr = MYKONOS_setupGpio(&mykDevice)) != MYKONOS_ERR_GPIO_OK) {
	    errorString = getGpioMykonosErrorMessage(mykGpioErr);
	    goto error;
    }

	/*************************************************************************/
	/*****                Mykonos Set manual gains values                *****/
	/*************************************************************************/

	if ((mykError = MYKONOS_setRx1ManualGain(&mykDevice, 255)) != MYKONOS_ERR_OK) {
	    errorString = getMykonosErrorMessage(mykError);
	    goto error;
	}

	if ((mykError = MYKONOS_setRx2ManualGain(&mykDevice, 255)) != MYKONOS_ERR_OK) {
	    errorString = getMykonosErrorMessage(mykError);
	    goto error;
	}

	if ((mykError = MYKONOS_setObsRxManualGain(&mykDevice, OBS_RX1_TXLO, 255)) != MYKONOS_ERR_OK) {
	    errorString = getMykonosErrorMessage(mykError);
	    goto error;
	}

	if ((mykError = MYKONOS_setObsRxManualGain(&mykDevice, OBS_RX2_TXLO, 255)) != MYKONOS_ERR_OK) {
	    errorString = getMykonosErrorMessage(mykError);
	    goto error;
	}

	if ((mykError = MYKONOS_setObsRxManualGain(&mykDevice, OBS_SNIFFER_A, 255)) != MYKONOS_ERR_OK) {
	    errorString = getMykonosErrorMessage(mykError);
	    goto error;
	}

	if ((mykError = MYKONOS_setObsRxManualGain(&mykDevice, OBS_SNIFFER_B, 255)) != MYKONOS_ERR_OK) {
	    errorString = getMykonosErrorMessage(mykError);
	    goto error;
	}
	if ((mykError = MYKONOS_setObsRxManualGain(&mykDevice, OBS_SNIFFER_C, 255)) != MYKONOS_ERR_OK) {
	    errorString = getMykonosErrorMessage(mykError);
	    goto error;
	}

	/*************************************************************************/
	/*****                Mykonos Initialize attenuations                *****/
	/*************************************************************************/

	if ((mykError = MYKONOS_setTx1Attenuation(&mykDevice, 0)) != MYKONOS_ERR_OK) {
	    errorString = getMykonosErrorMessage(mykError);
	    goto error;
	}

	if ((mykError = MYKONOS_setTx2Attenuation(&mykDevice, 0)) != MYKONOS_ERR_OK) {
	    errorString = getMykonosErrorMessage(mykError);
	    goto error;
	}

	/*************************************************************************/
	/*****           Mykonos ARM Initialization Calibrations             *****/
	/*************************************************************************/

    if ((mykError = MYKONOS_runInitCals(&mykDevice, (initCalMask & ~TX_LO_LEAKAGE_EXTERNAL))) != MYKONOS_ERR_OK) {
		errorString = getMykonosErrorMessage(mykError);
		goto error;
	}

	if ((mykError = MYKONOS_waitInitCals(&mykDevice, 60000, &errorFlag, &errorCode)) != MYKONOS_ERR_OK) {
		errorString = getMykonosErrorMessage(mykError);
		goto error;
	}

	if ((errorFlag != 0) || (errorCode != 0)) {
		/*** < Info: abort init cals > ***/
		if ((mykError = MYKONOS_abortInitCals(&mykDevice, &initCalsCompleted)) != MYKONOS_ERR_OK) {
			errorString = getMykonosErrorMessage(mykError);
			goto error;
		}
		if (initCalsCompleted)
			printf("Completed calibrations: %x\n", (unsigned int)initCalsCompleted);
	}
	else
		printf("Calibrations completed successfully\n");

	/*************************************************************************/
	/*****  Mykonos ARM Initialization External LOL Calibrations with PA *****/
	/*************************************************************************/

	/* Please ensure PA is enabled operational at this time */
	if (initCalMask & TX_LO_LEAKAGE_EXTERNAL) {
		if ((mykError = MYKONOS_runInitCals(&mykDevice, TX_LO_LEAKAGE_EXTERNAL)) != MYKONOS_ERR_OK) {
			errorString = getMykonosErrorMessage(mykError);
			goto error;
		}

		if ((mykError = MYKONOS_waitInitCals(&mykDevice, 60000, &errorFlag, &errorCode)) != MYKONOS_ERR_OK) {
			errorString = getMykonosErrorMessage(mykError);
			goto error;
		}

		if ((errorFlag != 0) || (errorCode != 0)) {
			/*** < Info: abort init cals > ***/
			if ((mykError = MYKONOS_abortInitCals(&mykDevice, &initCalsCompleted)) != MYKONOS_ERR_OK) {
				errorString = getMykonosErrorMessage(mykError);
				goto error;
			}
		} else
			printf("External LOL Calibrations completed successfully\n");
	}

	/*************************************************************************/
	/*****             SYSTEM JESD bring up procedure                    *****/
	/*************************************************************************/

	if ((mykError = MYKONOS_enableSysrefToRxFramer(&mykDevice, 1)) != MYKONOS_ERR_OK) {
	    errorString = getMykonosErrorMessage(mykError);
	    goto error;
	}
	/*** < Info: Mykonos is waiting for sysref in order to start
	 * transmitting CGS from the RxFramer> ***/

	if ((mykError = MYKONOS_enableSysrefToObsRxFramer(&mykDevice, 1)) != MYKONOS_ERR_OK) {
        errorString = getMykonosErrorMessage(mykError);
        goto error;
	}
	/*** < Info: Mykonos is waiting for sysref in order to start
	 * transmitting CGS from the ObsRxFramer> ***/

	if ((mykError = MYKONOS_enableSysrefToDeframer(&mykDevice, 0)) != MYKONOS_ERR_OK) {
        errorString = getMykonosErrorMessage(mykError);
        goto error;
	}

	if ((mykError = MYKONOS_resetDeframer(&mykDevice)) != MYKONOS_ERR_OK) {
        errorString = getMykonosErrorMessage(mykError);
        goto error;
	}

	if ((mykError = MYKONOS_enableSysrefToDeframer(&mykDevice, 1)) != MYKONOS_ERR_OK)
	{
        errorString = getMykonosErrorMessage(mykError);
        goto error;
	}

	/*************************************************************************/
	/*****            Enable SYSREF to Mykonos and BBIC                  *****/
	/*************************************************************************/

	/* Request a SYSREF from the AD9528 */
	AD9528_requestSysref(&clockAD9528_, 1);
	mdelay(1);

	/*** < Info: Mykonos is actively transmitting CGS from the RxFramer> ***/

	/*** < Info: Mykonos is actively transmitting CGS from the ObsRxFramer> ***/

	/* Initialize CLKGENs */
	status = clkgen_setup();
	if (status != 0) {
		printf("clkgen_setup() failed\n");
		return -1;
	}

	/* Initialize ADXCVRs */
	status = xcvr_setup();
	if (status != 0) {
		printf("xcvr_setup() failed\n");
		return -1;
	}

	/* Initialize JESDs */
	status = jesd_setup();
	if (status != 0) {
		printf("jesd_setup() failed\n");
		return -1;
	}

	/* Request two SYSREFs from the AD9528 */
	AD9528_requestSysref(&clockAD9528_, 1);
	mdelay(1);
	AD9528_requestSysref(&clockAD9528_, 1);
	mdelay(1);

	/*************************************************************************/
	/*****               Check Mykonos Framer Status                     *****/
	/*************************************************************************/

	if ((mykError = MYKONOS_readRxFramerStatus(&mykDevice, &framerStatus)) != MYKONOS_ERR_OK) {
        errorString = getMykonosErrorMessage(mykError);
        goto error;
	} else
		printf("RxFramerStatus = 0x%x\n", framerStatus);

	if ((mykError = MYKONOS_readOrxFramerStatus(&mykDevice, &obsFramerStatus)) != MYKONOS_ERR_OK) {
        errorString = getMykonosErrorMessage(mykError);
        goto error;
	} else
		printf("OrxFramerStatus = 0x%x\n", obsFramerStatus);

	/*************************************************************************/
	/*****               Check Mykonos Deframer Status                   *****/
	/*************************************************************************/

	if ((mykError = MYKONOS_readDeframerStatus(&mykDevice, &deframerStatus)) != MYKONOS_ERR_OK) {
        errorString = getMykonosErrorMessage(mykError);
        goto error;
	} else
		printf("DeframerStatus = 0x%x\n", deframerStatus);

	/*************************************************************************/
	/*****           Mykonos enable tracking calibrations                *****/
	/*************************************************************************/

	if ((mykError = MYKONOS_enableTrackingCals(&mykDevice, trackingCalMask)) != MYKONOS_ERR_OK) {
        errorString = getMykonosErrorMessage(mykError);
        goto error;
	}

	/*** < Info: Allow Rx1/2 QEC tracking and Tx1/2 QEC tracking to run when in the radioOn state
	     *  Tx calibrations will only run if radioOn and the obsRx path is set to OBS_INTERNAL_CALS > ***/

	/*** < Info: Function to turn radio on, Enables transmitters and receivers
	 * that were setup during MYKONOS_initialize() > ***/
	if ((mykError = MYKONOS_radioOn(&mykDevice)) != MYKONOS_ERR_OK) {
        errorString = getMykonosErrorMessage(mykError);
        goto error;
	}

	/*** < Info: Allow TxQEC to run when User: is not actively using ORx receive path > ***/
	if ((mykError = MYKONOS_setObsRxPathSource(&mykDevice, OBS_RXOFF)) != MYKONOS_ERR_OK) {
        errorString = getMykonosErrorMessage(mykError);
        goto error;
    }
	if ((mykError = MYKONOS_setObsRxPathSource(&mykDevice, OBS_INTERNALCALS)) != MYKONOS_ERR_OK) {
        errorString = getMykonosErrorMessage(mykError);
        goto error;
	}

	printf("Done\n");

	return 0;

error:
	printf("%s", errorString);

	return mykError;
}
