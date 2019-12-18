/**
 * \file ad9371/src/devices/ad9528/ad9528.h
 * \brief Contains function declarations and ad9528Device_t structure typedef for ad9528.c
 */

/**
* Legal Disclaimer
* Copyright 2015-2017 Analog Devices Inc.
* Released under the AD9371 API license, for more information see the "LICENSE.txt" file in this zip file.
*
*/

#ifndef _AD9528_LIB_H_
#define _AD9528_LIB_H_


#include "t_ad9528.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLEMENTE_TDD_AD9528_CLKOUT_INDEX 13

//Helper function to setup the 9528 device data structure specifically for the ADI transceiver evaluation cards
ADI_ERR AD9528_initDeviceDataStruct(ad9528Device_t *device, uint32_t vcxoFrequency_Hz, uint32_t refAFrequency_Hz, uint32_t outputDeviceClock_Hz);
ADI_ERR AD9528_reportClockRates(ad9528Device_t *device); //debug function to help verify data structure is setup correctly.

//AD9528 API functions
ADI_ERR AD9528_resetDevice(ad9528Device_t *device);
ADI_ERR AD9528_setSpiSettings(ad9528Device_t *device);
ADI_ERR AD9528_initialize(ad9528Device_t *device); //writes all registers to desired values, leaves clock outputs off.
ADI_ERR AD9528_waitForPllLock(ad9528Device_t *device);
ADI_ERR AD9528_readPllStatus(ad9528Device_t *device, uint8_t *status);
ADI_ERR AD9528_enableClockOutputs(ad9528Device_t *device, uint16_t clkEnable); //clkEnable: bit per clock output
ADI_ERR AD9528_setupSYSREF(ad9528Device_t *device, uint16_t divideFromPll1Out,  sysrefPatternMode_t sysrefPatternMode, sysrefNshotMode_t nShotPulses);
ADI_ERR AD9528_requestSysref(ad9528Device_t *device, uint8_t enableSYSREF);

#ifdef __cplusplus
}
#endif

#endif
