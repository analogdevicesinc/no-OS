/**
 * \file headless.c
 *
 * \brief Contains example code for user integration with their application
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 *
 * All data structures required for operation have been initialized with values
 * which reflect these settings:
 *
 * Device Clock = 245.76 Mhz
 * Rx1 and Rx2 are enabled
 * Tx1 and Tx2 are enabled
 *
 * Talise Framer:
 * Lane Rate = 9830.4 Mbs
 * L = 2
 * K = 32
 * M = 4
 * F = 4
 *
 * Talise Deframer:
 * Lane Rate = 9830.4 Mbs
 * L = 4
 * K = 32
 * M = 4
 * F = 2
 *
 * Talise Observation framer:
 * Lane Rate = 9830.4 Mbs
 * L = 2
 * K = 32
 * M = 2
 * F = 2
 *
 * Using these profiles:
 * Rx 200Mhz, IQrate 245.76Mhz, Dec5
 * Tx 200/450Mhz, IQrate 491.52Mhz, Dec5
 * ORX 450Mhz, IQrate 491.52Mhz, Dec5
 *
 */

#include "headless.h"
#include "talise.h"
#include "talise_jesd204.h"
#include "talise_arm.h"
#include "talise_radioctrl.h"
#include "talise_cals.h"
#include "talise_config.h"
#include "talise_error.h"

/****< Insert User Includes Here >***/

/**********************************************************/
/**********************************************************/
/********** Talise Data Structure Initializations ********/
/**********************************************************/
/**********************************************************/

int main()
{
    /*< binary[114688]> contains ARM binary file as array - set by user > */
    uint8_t mcsStatus = 0;
    uint8_t pllLockStatus = 0;
    uint8_t armBinary[114688] = {0};
    uint32_t count = sizeof(armBinary);
    uint8_t streamBinary[4096] = {0};

    uint32_t initCalMask = TAL_TX_BB_FILTER | TAL_ADC_TUNER | TAL_TIA_3DB_CORNER | TAL_DC_OFFSET | TAL_RX_GAIN_DELAY | TAL_FLASH_CAL | TAL_PATH_DELAY | TAL_TX_LO_LEAKAGE_INTERNAL | TAL_TX_QEC_INIT | TAL_LOOPBACK_RX_LO_DELAY | TAL_LOOPBACK_RX_RX_QEC_INIT | TAL_RX_QEC_INIT | TAL_ORX_QEC_INIT | TAL_TX_DAC;

    uint32_t trackingCalMask = TAL_TRACK_NONE;

    uint8_t errorFlag = 0;
    uint16_t deframerStatus = 0;

    uint8_t framerStatus = 0;
    uint32_t talAction = TALACT_NO_ACTION;
    /**********************************************************/
    /**********************************************************/
    /************ Talise Initialization Sequence *************/
    /**********************************************************/
    /**********************************************************/

    /** < Insert User System Clock(s) Initialization Code Here >
     * System Clock should provide a device clock and SYSREF signal
     * to the Talise device.
     **/

    /*** < Insert User BBIC JESD204B Initialization Code Here > ***/

    /*******************************/
    /**** Talise Initialization ***/
    /*******************************/

    /*Open Talise Hw Device*/
    talAction = TALISE_openHw(&talDevice);
    if(talAction != TALACT_NO_ACTION)
    {
        /*** < User: decide what to do based on Talise recovery action returned > ***/
    }

    /* Toggle RESETB pin on Talise device */
    talAction = TALISE_resetDevice(&talDevice);
    if (talAction != TALACT_NO_ACTION)
    {
        /*** < User: decide what to do based on Talise recovery action returned > ***/
    }

    /* TALISE_initialize() loads the Talise device data structure
     * settings for the Rx/Tx/ORx profiles, FIR filters, digital
     * filter enables, calibrates the CLKPLL, loads the user provided Rx
     * gain tables, and configures the JESD204b serializers/framers/deserializers
     * and deframers.
     */
    talAction = TALISE_initialize(&talDevice, &talInit);
    if (talAction != TALACT_NO_ACTION)
    {
        /*** < User: decide what to do based on Talise recovery action returned > ***/
    }

    /*******************************/
    /***** CLKPLL Status Check *****/
    /*******************************/
	talAction = TALISE_getPllsLockStatus(&talDevice, &pllLockStatus);
    if (talAction != TALACT_NO_ACTION)
    {
        /*** < User: decide what to do based on Talise recovery action returned > ***/
    }

    /* Assert that Talise CLKPLL is locked */
    if ((pllLockStatus & 0x01) == 0)
    {
        /* <user code - CLKPLL not locked - ensure lock before proceeding */
    }

    /*******************************************************/
    /**** Perform MultiChip Sync (MCS) on Talise Device ***/
    /*******************************************************/
    talAction = TALISE_enableMultichipSync(&talDevice, 1, &mcsStatus);
    if (talAction != TALACT_NO_ACTION)
    {
        /*** < User: decide what to do based on Talise recovery action returned > ***/
    }

    /*< user code - Request minimum 3 SYSREF pulses from Clock Device - > */

    /*******************/
    /**** Verify MCS ***/
    /*******************/
    talAction = TALISE_enableMultichipSync(&talDevice, 0, &mcsStatus);
    if ((mcsStatus & 0x0A) != 0x0A)
    {
        /*< user code - MCS failed - ensure MCS before proceeding*/
    }

    /*******************************************************/
    /**** Prepare Talise Arm binary and Load Arm and    ****/
	/**** Stream processor Binaryes 					****/
    /*******************************************************/
    if (pllLockStatus & 0x01)
    {
        talAction = TALISE_initArm(&talDevice, &talInit);
        /*< user code- load Talise stream binary into streamBinary[4096] >*/
        /*< user code- load ARM binary byte array into armBinary[114688] >*/

        talAction = TALISE_loadStreamFromBinary(&talDevice, &streamBinary[0]);
        if (talAction != TALACT_NO_ACTION)
        {
            /*** < User: decide what to do based on Talise recovery action returned > ***/
        }

        talAction = TALISE_loadArmFromBinary(&talDevice, &armBinary[0], count);
        if (talAction != TALACT_NO_ACTION)
        {
            /*** < User: decide what to do based on Talise recovery action returned > ***/
        }

        /* TALISE_verifyArmChecksum() will timeout after 200ms
         * if ARM checksum is not computed
         */
        talAction = TALISE_verifyArmChecksum(&talDevice);
        if (talAction != TAL_ERR_OK)
        {
            /*< user code- ARM did not load properly - check armBinary & clock/profile settings >*/
        }

    }
    else
    {
        /*< user code- check settings for proper CLKPLL lock  > ***/
    }

    /*******************************/
    /**Set RF PLL LO Frequencies ***/
    /*******************************/
	TALISE_setRfPllLoopFilter(&talDevice, 50, 3);
    talAction = TALISE_setRfPllFrequency(&talDevice, TAL_RF_PLL, 1800000000);
    
    if (talAction != TALACT_NO_ACTION)
    {
        /*** < User: decide what to do based on Talise recovery action returned > ***/
    }

    /*** < wait 200ms for PLLs to lock - user code here > ***/

    talAction = TALISE_getPllsLockStatus(&talDevice, &pllLockStatus);
    if ((pllLockStatus & 0x07) != 0x07)
    {
        /*< user code - ensure lock of all PLLs before proceeding>*/
    }

    /****************************************************/
    /**** Run Talise ARM Initialization Calibrations ***/
    /****************************************************/
	/*** < User: Turn ON the PA (if any), and open any switches on ORx input used to isolate it for calibrations > ***/
	/*** < User: Open any switches on the Rx input (if used) to isolate Rx input and provide required VSWR at input > ***/
    talAction = TALISE_runInitCals(&talDevice, initCalMask & ~TAL_TX_LO_LEAKAGE_EXTERNAL);
    if (talAction != TALACT_NO_ACTION)
    {
        /*** < User: decide what to do based on Talise recovery action returned > ***/
    }

    talAction = TALISE_waitInitCals(&talDevice, 20000, &errorFlag);
    if (talAction != TALACT_NO_ACTION)
    {
        /*** < User: decide what to do based on Talise recovery action returned > ***/
    }

    if (errorFlag)
    {
        /*< user code - Check error flag to determine ARM  error> */
    }
    else
    {
        /*< user code - Calibrations completed successfully > */
    }

	/*************************************************************************/
	/*****  TALISE ARM Initialization External LOL Calibrations with PA  *****/
	/*************************************************************************/
	/*** < Action: Please ensure PA is enabled operational at this time > ***/
	if (initCalMask & TAL_TX_LO_LEAKAGE_EXTERNAL)
	{
		talAction = TALISE_runInitCals(&talDevice, TAL_TX_LO_LEAKAGE_EXTERNAL);
		if (talAction != TALACT_NO_ACTION)
		{
			/*** < User: decide what to do based on Talise recovery action returned > ***/
		}

		talAction = TALISE_waitInitCals(&talDevice, 20000, &errorFlag);
		if (talAction != TALACT_NO_ACTION)
		{
			/*** < User: decide what to do based on Talise recovery action returned > ***/
		}

		if (errorFlag)
		{
			/*< user code - Check error flag to determine ARM  error> */
		}
		else
		{
			/*< user code - Calibrations completed successfully > */
		}
	}

    
	/***************************************************/
	/**** Enable Talise JESD204B Framer ***/
	/***************************************************/

	talAction = TALISE_enableFramerLink(&talDevice, TAL_FRAMER_A, 0);
	if (talAction != TALACT_NO_ACTION)
	{
		/*** < User: decide what to do based on Talise recovery action returned > ***/
	}

	talAction |= TALISE_enableFramerLink(&talDevice, TAL_FRAMER_A, 1);
	if (talAction != TALACT_NO_ACTION)
	{
		/*** < User: decide what to do based on Talise recovery action returned > ***/
	}

	/*************************************************/
	/**** Enable SYSREF to Talise JESD204B Framer ***/
	/*************************************************/
	/*** < User: Make sure SYSREF is stopped/disabled > ***/

	talAction = TALISE_enableSysrefToFramer(&talDevice, TAL_FRAMER_A, 1);
	if (talAction != TALACT_NO_ACTION)
	{
		/*** < User: decide what to do based on Talise recovery action returned > ***/
	}

	/***************************************************/
	/**** Enable  Talise JESD204B Deframer ***/
	/***************************************************/

	talAction = TALISE_enableDeframerLink(&talDevice, TAL_DEFRAMER_A, 0);
	if (talAction != TALACT_NO_ACTION)
	{
		/*** < User: decide what to do based on Talise recovery action returned > ***/
	}

	talAction |= TALISE_enableDeframerLink(&talDevice, TAL_DEFRAMER_A, 1);
	if (talAction != TALACT_NO_ACTION)
	{
		/*** < User: decide what to do based on Talise recovery action returned > ***/
	}
	/***************************************************/
	/**** Enable SYSREF to Talise JESD204B Deframer ***/
	/***************************************************/
	talAction = TALISE_enableSysrefToDeframer(&talDevice, TAL_DEFRAMER_A, 1);
	if (talAction != TALACT_NO_ACTION)
	{
		/*** < User: decide what to do based on Talise recovery action returned > ***/
	}
	
    /*** < User Sends SYSREF Here > ***/

    /*** < Insert User JESD204B Sync Verification Code Here > ***/

	/**************************************/
	/**** Check Talise Deframer Status ***/
	/**************************************/
	talAction = TALISE_readDeframerStatus(&talDevice, TAL_DEFRAMER_A, &deframerStatus);
	if (talAction != TALACT_NO_ACTION)
	{
		/*** < User: decide what to do based on Talise recovery action returned > ***/
	}

	
	/************************************/
	/**** Check Talise Framer Status ***/
	/************************************/
	talAction = TALISE_readFramerStatus(&talDevice, TAL_FRAMER_A, &framerStatus);
	if (talAction != TALACT_NO_ACTION)
{
		/*** < User: decide what to do based on Talise recovery action returned > ***/
	}

    /*** < User: When links have been verified, proceed > ***/

    /***********************************************
    * Allow Rx1/2 QEC tracking and Tx1/2 QEC       *
    * tracking to run when in the radioOn state    *
    * Tx calibrations will only run if radioOn and *
    * the obsRx path is set to OBS_INTERNAL_CALS   *
    * **********************************************/

    talAction = TALISE_enableTrackingCals(&talDevice, trackingCalMask);
    if (talAction != TALACT_NO_ACTION)
    {
        /*** < User: decide what to do based on Talise recovery action returned > ***/
    }

    /* Function to turn radio on, Enables transmitters and receivers */
    /* that were setup during TALISE_initialize() */
    talAction = TALISE_radioOn(&talDevice);
    if (talAction != TALACT_NO_ACTION)
    {
        /*** < User: decide what to do based on Talise recovery action returned > ***/
    }


    /***********************************************
    * Shutdown Procedure *
    * **********************************************/
    /* Function to turn radio on, Disables transmitters and receivers */
    talAction = TALISE_radioOff(&talDevice);
    if (talAction != TALACT_NO_ACTION)
    {
        /*** < User: decide what to do based on Talise recovery action returned > ***/
    }
    /* Put Talise in safe state for power down */
    talAction = TALISE_shutdown(&talDevice);
    if (talAction != TALACT_NO_ACTION)
    {
       /*** < User: decide what to do based on Talise recovery action returned > ***/
    }

    /*Close Talise Hw Device*/
    talAction = TALISE_closeHw(&talDevice);
    if(talAction != TALACT_NO_ACTION)
    {
       /*** < User: decide what to do based on Talise recovery action returned > ***/
    }
    return 0;
}
