/***************************************************************************//**
*   @file   ad9833.h
*   @brief  Header file of AD9833 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/
#ifndef _AD9833_H_
#define _AD9833_H_

/******************************************************************************/
/********************* Macros and Constants Definitions ***********************/
/******************************************************************************/

#define AD9833_CTRLB28          (1 << 13)
#define AD9833_CTRLHLB          (1 << 12)
#define AD9833_CTRLFSEL         (1 << 11)
#define AD9833_CTRLPSEL         (1 << 10)
#define AD9834_CTRLPINSW        (1 << 9)
#define AD9833_CTRLRESET        (1 << 8)
#define AD9833_CTRLSLEEP1       (1 << 7)
#define AD9833_CTRLSLEEP12      (1 << 6)
#define AD9833_CTRLOPBITEN      (1 << 5)
#define AD9834_CTRLSIGNPIB      (1 << 4)
#define AD9833_CTRLDIV2         (1 << 3)
#define AD9833_CTRLMODE         (1 << 1)

/* GPIOs */
#define AD9834_PSEL_OUT         gpio_direction_output(dev->gpio_psel,  \
			        GPIO_HIGH)
#define AD9834_PSEL_LOW         gpio_set_value(dev->gpio_psel,         \
			        GPIO_LOW)
#define AD9834_PSEL_HIGH        gpio_set_value(dev->gpio_psel,         \
			        GPIO_HIGH)

#define AD9834_FSEL_OUT         gpio_direction_output(dev->gpio_fsel,  \
			        GPIO_HIGH)
#define AD9834_FSEL_LOW         gpio_set_value(dev->gpio_fsel,         \
			        GPIO_LOW)
#define AD9834_FSEL_HIGH        gpio_set_value(dev->gpio_fsel,         \
			        GPIO_HIGH)

#define AD9834_RESET_OUT        gpio_direction_output(dev->gpio_reset, \
			        GPIO_HIGH)
#define AD9834_RESET_LOW        gpio_set_value(dev->gpio_reset,        \
			        GPIO_LOW)
#define AD9834_RESET_HIGH       pio_set_value(dev->gpio_reset,         \
			        GPIO_HIGH)

#define AD9834_SLEEP_OUT        gpio_direction_output(dev->gpio_sleep, \
			        GPIO_HIGH)
#define AD9834_SLEEP_LOW        gpio_set_value(dev->gpio_sleep,        \
			        GPIO_LOW)
#define AD9834_SLEEP_HIGH       gpio_set_value(dev->gpio_sleep,         \
			        GPIO_HIGH)


#define BIT_F0ADDRESS           0x4000      // Frequency Register 0 address.
#define BIT_F1ADDRESS           0x8000      // Frequency Register 1 address.
#define BIT_P0ADDRESS           0xC000      // Phase Register 0 address.
#define BIT_P1ADDRESS           0xE000      // Phase Register 1 address.

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* Supported devices */
typedef enum {
    ID_AD9833,
    ID_AD9834,
    ID_AD9837,
    ID_AD9838,
} AD9833_type;

typedef struct {
	/* SPI */
	spi_desc		*spi_desc;
	/* GPIO */
	gpio_desc		*gpio_psel;
	gpio_desc		*gpio_fsel;
	gpio_desc		*gpio_reset;
	gpio_desc		*gpio_sleep;
	/* Device Settings */
	AD9833_type		act_device;
	unsigned char		prog_method;
	unsigned short		ctrlRegValue;
	unsigned short		testOpbiten;
} AD9833_dev;

typedef struct {
	/* SPI */
	spi_init_param		spi_init;
	/* GPIO */
	int8_t			gpio_psel;
	int8_t			gpio_fsel;
	int8_t			gpio_reset;
	int8_t			gpio_sleep;
	/* Device Settings */
	AD9833_type		act_device;
} AD9833_init_param;

typedef struct {
    unsigned long mclk;
    float freq_const;
} ad9833_chip_info;

/******************************************************************************/
/************************** Functions Declarations ****************************/
/******************************************************************************/
/* Initialize the SPI communication with the device. */
char AD9833_Init(AD9833_dev **device,
		 AD9833_init_param init_param);
/* Free the resources allocated by adf7023_init(). */
int32_t AD9833_remove(AD9833_dev *dev);
/* Transmits 16 bits on SPI. */
void AD9833_TxSpi(AD9833_dev *dev,
		  short value);
/* Selects the type of output. */
char AD9833_OutMode(AD9833_dev *dev,
		    unsigned char vOutMode);
/* Loads a frequency value in a register. */
void AD9833_SetFreq(AD9833_dev *dev,
		    unsigned char registerNumber,
		    unsigned long frequencyValue);
/* Loads a phase value in a register. */
void AD9833_SetPhase(AD9833_dev *dev,
		     unsigned char registerNumber,
		     float phaseValue);
/* Select the frequency register to be used. */
void AD9833_SelectFreqReg(AD9833_dev *dev,
			  unsigned char freqReg);
/* Select the phase register to be used. */
void AD9833_SelectPhaseReg(AD9833_dev *dev,
			   unsigned char phaseReg);
/* Enable / Disable the sleep function. */
void AD9833_SleepMode(AD9833_dev *dev,
		      unsigned char sleepMode);

void AD9834_SelectProgMethod(AD9833_dev *dev,
			     unsigned char value);

void AD9834_LogicOutput(AD9833_dev *dev,
			unsigned char opbiten,
                        unsigned char signpib,
                        unsigned char div2);

#endif  /* _AD9833_H_ */
