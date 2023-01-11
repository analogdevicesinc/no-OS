/***************************************************************************//**
 *   @file   ltc2672.h
 *   @brief  Implementation of ltc2672 Driver.
 *   @author JSanBuen (jose.sanbuenaventura@analog.com)
 *   @author MSosa (marcpaolo.sosa@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#ifndef __MAX31855_H__
#define __MAX31855_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define LTC2672_CODE_TO_CHANNEL_X 						0b0000
#define LTC2672_CODE_TO_CHANNEL_ALL 					0b1000
#define LTC2672_SPAN_TO_CHANNEL_X 						0b0110
#define LTC2672_SPAN_TO_CHANNEL_ALL 					0b1110
#define LTC2672_PWRUP_UPD_CHANNEL_X 					0b0001
#define LTC2672_PWRUP_UPD_CHANNEL_ALL 					0b1001
#define LTC2672_CODE_PWRUP_UPD_CHANNEL_X 				0b0011
#define LTC2672_CODE_TO_CHANNEL_X_PWRUP_UPD_CHANNEL_ALL 0b0010
#define LTC2672_CODE_PWRUP_UPD_CHANNEL_ALL 				0b1010
#define LTC2672_PWRDWN_CHANNEL_X 						0b0100
#define LTC2672_PWRDWN_CHANNEL_DEV 						0b0101
#define LTC2672_MON_MUX 								0b1011
#define LTC2672_TOGGLE_SEL 								0b1100
#define LTC2672_TOGGLE_GLBL 							0b1101
#define LTC2672_CNFG_CMD 								0b0111
#define LTC2672_NO_OP 									0b1111

enum ltc2672_device_id {
	LTC2672_12 = 0,
	LTC2672_16 = 1
};

enum ltc2672_dac_ch {
	LTC2672_DAC0,
	LTC2672_DAC1,
	LTC2672_DAC2, 
	LTC2672_DAC3,
	LTC2672_DAC4 
};

enum ltc2672_out_range {
	LTC2672_OFF,
	LTC2672_50VREF,
	LTC2672_100VREF, 
	LTC2672_200VREF, 
	LTC2672_400VREF, 
	LTC2672_800VREF, 
	LTC2672_1600VREF, 
	LTC2672_3200VREF, 
	LTC2672_VM_VREF, 
	LTC2672_4800VREF = 0XF 	
};



/**
 * @struct ltc2672_dev
 * @brief Structure holding ltc2672 descriptor.
 */
struct ltc2672_dev {
	struct no_os_spi_desc *comm_desc;
};

/**
 * @struct ltc2672_init_param
 * @brief Structure holding the parameters for ltc2672 initialization.
 */
struct ltc2672_init_param {
	struct no_os_spi_init_param spi_init;
};


/** Device and comm init function */
int ltc2672_init(struct ltc2672_dev **, struct ltc2672_init_param *);

/** Free resources allocated by the init function */
int ltc2672_remove(struct ltc2672_dev *);

/** Read raw register value */
int ltc2672_read(struct ltc2672_dev *, uint8_t, uint8_t *);

/** Write raw register value */
int ltc2672_write(struct ltc2672_dev *, uint8_t, uint8_t);

/** Read fault register value */
int ltc2672_read_fault(struct ltc2672_dev *, uint8_t *);

/** Clear all faults */
int ltc2672_clear_fault(struct ltc2672_dev *);

/** Enable bias */
int ltc2672_enable_bias(struct ltc2672_dev *, bool bias_en);

/** Enable auto-convert */
int ltc2672_auto_convert(struct ltc2672_dev *, bool auto_conv_en);

/** Enable 50Hz filter, default is 60Hz */
int ltc2672_enable_50Hz(struct ltc2672_dev *, bool filt_en);

/** Set threshold **/
int ltc2672_set_threshold(struct ltc2672_dev *, uint16_t, uint16_t);

/** Get Lower threshold **/
int ltc2672_get_lower_threshold(struct ltc2672_dev*, uint16_t *);

/** Get Upper threshold **/
int ltc2672_get_upper_threshold(struct ltc2672_dev*, uint16_t *);

/** Set Wires **/
int ltc2672_set_wires(struct ltc2672_dev *, bool is_odd_wire);

/** Read RTD **/
int ltc2672_read_rtd(struct ltc2672_dev *, uint16_t *);

#endif // __MAX31855_H__

