/*!
REVISION HISTORY
$Revision: 1807 $
$Date: 2013-07-29 13:06:06 -0700 (Mon, 29 Jul 2013) $

Copyright (c) 2013, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/

/*! @file
    @ingroup LTC2315
    Header for LTC2315: 12/14-bit 1Msps ADC
*/

#ifndef LTC231X_H
#define LTC231X_H

/*! Define the SPI CS pin */
#ifndef LTC2315_CS
#define LTC2315_CS QUIKEVAL_CS
#endif

/*! @name LTC2315 Channel Address */
//! @{
// Channel Address
#define LTC2315_ADDRESS 0x00
//!@}

#define LTC2312_READ_BYTES_NUMBER 2
#define LTC2312_READ_VALUES_NUMBER 100

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_delay.h"
#include "no_os_spi.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum device_type {
	LTC2312_12,
	LTC2312_14
};

struct ltc2312_dev {
	/* Device characteristics */
	enum device_type type;
	/* SPI */
	struct no_os_spi_desc *spi_desc;
};

struct ltc2312_init_param {
	/* Device characteristics */
	enum device_type type;
	/* SPI */
	struct no_os_spi_init_param spi_init;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initializes the ltc231x. */
int32_t ltc2312_setup(struct ltc2312_dev **device,
		      struct ltc2312_init_param *init_param);

/* Free the resources allocated by ltc231x_setup(). */
int32_t ltc2312_remove(struct ltc2312_dev *dev);

/* Reads the LTC2315 and returns 32-bit data in offset binary format. */
int32_t ltc2312_read(struct ltc2312_dev *dev, uint16_t *ptr_adc_code);

/* Calculates the LTC2315 input voltage given the binary data and LSB weight. */
void ltc2312_code_to_voltage(struct ltc2312_dev *dev, uint16_t adc_code,
			     float vref, float *voltage);

#endif  //  LTC2315_H
