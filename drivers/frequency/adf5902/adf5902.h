/***************************************************************************//**
 *   @file   adf5902.h
 *   @brief  Header file for adf5902 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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

#ifndef SRC_ADF5902_H_
#define SRC_ADF5902_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdin.h>
#include "spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* ADF5902 Registers */
#define ADF5902_REG0	0x0
#define ADF5902_REG1	0x1
#define ADF5902_REG2	0x2
#define ADF5902_REG3	0x3
#define ADF5902_REG4	0x4
#define ADF5902_REG5	0x5
#define ADF5902_REG6	0x6
#define ADF5902_REG7	0x7
#define ADF5902_REG8	0x8
#define ADF5902_REG9	0x9
#define ADF5902_REG10	0x10
#define ADF5902_REG11	0x11
#define ADF5902_REG12	0x12
#define ADF5902_REG13	0x13
#define ADF5902_REG14	0x14
#define ADF5902_REG15	0x15
#define ADF5902_REG16	0x16
#define ADF5902_REG17	0x17

/* Register 0 Definitions */
#define ADF5902_REG0_PLO(x)		(((x) & 0x1) << 5)
#define ADF5902_REG0_PTX1(x)		(((x) & 0x1) << 6)
#define ADF5902_REG0_PTX2(x)		(((x) & 0x1) << 7)
#define ADF5902_REG0_PADC(x)		(((x) & 0x1) << 8)
#define ADF5902_REG0_VCAL(x)		(((x) & 0x1) << 9)
#define ADF5902_REG0_PVCO(x)		(((x) & 0x1) << 10)
#define ADF5902_REG0_TX1C(x)		(((x) & 0x1) << 11)
#define ADF5902_REG0_TX2C(x)		(((x) & 0x1) << 12)
#define ADF5902_REG0_RESERVED		(0x4007F << 13)

/* Register 1 Definitions */
#define ADF5902_REG1_TX_AMP_CAL_REF(x)	(((x) & 0xFF) << 5)
#define ADF5902_REG1_RESERVED		(0x7FFBF << 13)

/* Register 2 Definitions */
#define ADF5902_REG2_ADC_CLK_DIV(x)	(((x) & 0xFF) << 5)
#define ADF5902_REG2_ADC_AVG(x)		(((x) & 0x3) << 13)
#define ADF5902_REG2_ADC_START(x)	(((x) & 0x1) << 15)
#define ADF5902_REG2_RESERVED		(0x2 << 16)

/* Register 3 Definitions */
#define ADF5902_REG3_READBACK_CTRL(x)	(((x) & 0x3F) << 5)
#define ADF5902_REG3_IO_LVL(x)		(((x) & 0x1) << 11)
#define ADF5902_REG3_MUXOUT(x)		(((x) & 0xF) << 12)
#define ADF5902_REG3_RESERVED		(0x189 << 16)

/* Register 4 Definitions */
#define ADF5902_REG4_TEST_BUS(x)	(((x) & 0x7FFF) << 5)
#define ADF5902_REG4_RESERVED		(0x0 << 16)

/* Register 5 Definitions */
#define ADF5902_REG5_FRAC_MSB_WORD(x)	(((x) & 0xFFF) << 5)
#define ADF5902_REG5_INTEGER_WORD(x)	(((x) & 0xFFF) << 17)
#define ADF5902_REG5_RAMP_ON(x)		(((x) & 0x1) << 29)
#define ADF5902_REG5_RESERVED		(0x0 << 30)

/* Register 6 Definitions */
#define ADF5902_REG6_FRAC_LSB_WORD(x)	(((x) & 0x1FFF) << 5)
#define ADF5902_REG6_RESERVED		(0x0 << 18)

/* Register 7 Definitions */
#define ADF5902_REG7_R_DIVIDER(x)	(((x) & 0x1F) << 5)
#define ADF5902_REG7_REF_DOUBLER(x)	(((x) & 0x1) << 10)
#define ADF5902_REG7_R_DIV_2(x)		(((x) & 0x1) << 11)
#define ADF5902_REG7_CLK_DIV(x)		(((x) & 0xFFF) << 12)
#define ADF5902_REG7_MASTER_RESET(x)	(((x) & 0x1) << 25)
#define ADF5902_REG6_RESERVED		((0x0 << 26) & (0x1 << 24))

/* Register 8 Definitions */
#define ADF5902_REG8_FREQ_CAL_DIV(x)	(((x) & 0x3FF) << 5)
#define ADF5902_REG8_RESERVED		(0x8000 << 15)

/* Register 9 Definitions */
#define ADF5902_REG9_RESERVED_CALIB	(0x15105C9 << 5)
#define ADF5902_REG9_RESERVED_NORMAL	(0x14005C9 << 5)

/* Register 10 Definitions */
#define ADF5902_REG10_RESERVED		(0xE99532 << 5)

/* Register 11 Definitions */
#define ADF5902_REG11_CNTR_RESET(x)	(((x) & 0x1) << 5)
#define ADF5902_REG11_RAMP_MODE(x)	(((x) & 0x3) << 7)
#define ADF5902_REG11_SING_FULL_TRI(x)	(((x) & 0x1) << 9)
#define ADF5902_REG11_SD_RESET(x)	(((x) & 0x1) << 11)
#define ADF5902_REG11_RESERVED		((0x0 << 6) & (0x0 << 10) & (0x0 << 12))

/* Register 12 Definitions */
#define ADF5902_REG12_CP_TRISTATE(x)	(((x) & 0x1) << 15)
#define ADF5902_REG12_CHARGE_PUMP(x)	(((x) & 0xF) << 17)
#define ADF5902_REG12_RESERVED		((0x0 << 5) & (0x1 << 16) & (0x2 << 21))

/* Register 13 Definitions */
#define ADF5902_REG13_CLK_DIV_SEL(x)	(((x) & 0x3) << 5)
#define ADF5902_REG13_CLK_DIV_2(x)	(((x) & 0xFFF) << 7)
#define ADF5902_REG13_CLK_DIV_MODE(x)	(((x) & 0x3) << 19)
#define ADF5902_REG13_LE_SEL(x)		(((x) & 0x1) << 21)
#define ADF5902_REG13_RESERVED		(0x0 << 22)

/* Register 14 Definitions */
#define ADF5902_REG14_DEV_WORD(x)	(((x) & 0xFFFF) << 5)
#define ADF5902_REG14_DEV_OFFSET(x)	(((x) & 0xF) << 21)
#define ADF5902_REG14_DEV_SEL(x)	(((x) & 0x3) << 25)
#define ADF5902_REG14_TX_RAMP_CLK(x)	(((x) & 0x1) << 30)
#define ADF5902_REG14_TX_DATA_INV(x)	(((x) & 0x1) << 31)
#define ADF5902_REG14_RESERVED		(0x0 << 27)

/* Register 15 Definitions */
#define ADF5902_REG15_STEP_WORD(x)	(((x) & 0xFFFFF) << 5)
#define ADF5902_REG15_STEP_SEL(x)	(((x) & 0x3) << 25)
#define ADF5902_REG15_RESERVED		(0x0 << 27)

/* Register 16 Definitions */
#define ADF5902_REG16_DEL_START_WORD(x)	(((x) & 0xFFF) << 5)
#define ADF5902_REG16_RAMP_DEL(x)	(((x) & 0x1) << 19)
#define ADF5902_REG16_TX_DATA_TRIG(x)	(((x) & 0x1) << 20)
#define ADF5902_REG16_DEL_SEL(x)	(((x) & 0x3) << 23)
#define ADF5902_REG16_RESERVED		((0x0 << 17) & (0x0 << 21) & (0x1 << 25))

/* Register 17 Definitions */
#define ADF5902_REG17_RESERVED		(0x0 << 5)

/** Specifications */
#define ADF5902_MAX_VCO_FREQ	24250000000ull
#define ADF5902_MIN_VCO_FREQ	24000000000ull
#define ADF5902_MIN_REFIN_FREQ	10000000
#define ADF5902_MAX_REFIN_FREQ	260000000
#define ADF5902_MAX_FREQ_PFD	110000000

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct adf5902_init_param {
	/* SPI Initialization parameters */
	spi_init_param		*spi_init;
	/* Reference input frequency */
	uint64_t		ref_in;
	/* Output frequency of the internal VCO */
	uint64_t		rf_out;
	/* Divide ration of the binary 5-bit reference counter */
	uint8_t			ref_div_factor;
	/* Reference doubler enable */
	uint8_t			ref_doubler_en;
	/* Reference divide by 2 bit */
	uint8_t			ref_div2_en;
};

struct adf5902_dev {
	/* SPI Descriptor */
	spi_desc		*spi_desc;
	/* Reference input frequency*/
	uint64_t		ref_in;
	/* Output frequency of the internal VCO */
	uint64_t		rf_out;
	/* Divide ration of the binary 5-bit reference counter */
	uint8_t			ref_div_factor;
	/* Reference doubler enable */
	uint8_t			ref_doubler_en;
	/* Reference divide by 2 bit */
	uint8_t			ref_div2_en;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** ADF5902 SPI write */
int32_t adf5902_write(struct adf5902_dev *device, uint32_t data);

/** ADF5902 SPI Readback */
int32_t adf5902_read(struct adf5902_dev *device, uint32_t *data);

/** ADF5902 Initialization */
int32_t adf5902_init(struct adf5902_dev **device,
		     adf5902_init_param *init_param);

/** ADF5902 Resources Deallocation */
int32_t adf5902_remove(struct adf5902_dev *device);
#endif /* SRC_ADF5902_H_ */
