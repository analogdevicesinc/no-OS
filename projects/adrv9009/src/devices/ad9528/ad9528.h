/**
 * \file ad9528.h
 * \brief Contains function declarations and ad9528Device_t structure typedef for ad9528.c
 */

/**
* \page Disclaimer Legal Disclaimer
* Copyright 2015-2017 Analog Devices Inc.
* Released under the AD9371 API license.
*
*/

#ifndef _AD9528_H_
#define _AD9528_H_


#include "t_ad9528.h"
#include "adi_hal.h"

//Helper function to setup the 9528 device data structure specifically for the ADI transceiver evaluation cards
adiHalErr_t AD9528_initDeviceDataStruct(ad9528Device_t *device,
					uint32_t vcxoFrequency_Hz, uint32_t refAFrequency_Hz,
					uint32_t outputDeviceClock_Hz);

//AD9528 API functions
adiHalErr_t AD9528_resetDevice(ad9528Device_t *device);
adiHalErr_t AD9528_initialize(ad9528Device_t
			      *device); //writes all registers to desired values, leaves clock outputs off.
adiHalErr_t AD9528_setupSYSREF(ad9528Device_t *device,
			       uint16_t divideFromPll1Out,  sysrefPatternMode_t sysrefPatternMode,
			       sysrefNshotMode_t nShotPulses);
adiHalErr_t AD9528_requestSysref(ad9528Device_t *device, uint8_t enableSYSREF);
adiHalErr_t AD9528_remove(ad9528Device_t *device);

#endif
