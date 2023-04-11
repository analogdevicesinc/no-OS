/******************************************************************************
*   @file   ADF4153.c
*   @brief  Implementation of ADF4153 Driver for Microblaze processor.
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
*
*******************************************************************************
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
******************************************************************************/

/*****************************************************************************/
/****************************** Include Files ********************************/
/*****************************************************************************/
#include <stdlib.h>
#include "adf4153.h"
#include "no_os_alloc.h"

/* For round up division */
#define CEIL(a, b) (((a) / (b)) + (((a) % (b)) > 0 ? 1 : 0))

#define DATA_MASK_MSB8      0xFF0000
#define DATA_OFFSET_MSB8    16
#define DATA_MASK_MID8      0xFF00
#define DATA_OFFSET_MID8    8
#define DATA_MASK_LSB8      0xFF
#define DATA_OFFSET_LSB8    0
#define ADDRESS_MASK        3

#define FREQ_2_GHZ          2000000000

/*****************************************************************************/
/*************************** Functions definitions ***************************/
/*****************************************************************************/

/**************************************************************************//**
 * @brief Initialize SPI and Initial Values for ADF4106 Board.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return success
******************************************************************************/
int8_t adf4153_init(struct adf4153_dev **device,
		    struct adf4153_init_param init_param)
{
	struct adf4153_dev *dev;
	int8_t status;

	dev = (struct adf4153_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->adf4153_st = init_param.adf4153_st;

	dev->adf4153_rfin_min_frq = 10000000;   // 10 Mhz
	dev->adf4153_rfin_max_frq = 250000000;  // 250 Mhz
	dev->adf4153_pfd_max_frq  = 32000000;   // 32 Mhz
	dev->adf4153_vco_min_frq  = 500000000;  // 500 Mhz
	dev->adf4153_vco_max_frq  = 4000000000u; // 4 Ghz
	dev->adf4153_mod_max      = 4095;       // the MOD is stored in 12 bits
	dev->r0 = 0;
	dev->r1 = 0;
	dev->r2 = 0;
	dev->r3 = 0;

	/* CPHA = 1; CPOL = 0; */
	status = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	/* GPIO */
	status |= no_os_gpio_get(&dev->gpio_le, &init_param.gpio_le);
	status |= no_os_gpio_get(&dev->gpio_ce, &init_param.gpio_ce);
	status |= no_os_gpio_get(&dev->gpio_le2, &init_param.gpio_le2);
	status |= no_os_gpio_get(&dev->gpio_ce2, &init_param.gpio_ce2);

	/* Bring CE high to put device to power up */
	ADF4153_CE_OUT;
	ADF4153_CE_HIGH;
	/* Initialize the load enable pin */
	ADF4153_LE_OUT;
	ADF4153_LE_LOW;

	/* Write all zeros to the noise and spur register */
	adf4153_update_latch(dev,
			     ADF4153_CTRL_NOISE_SPUR |
			     0x0);
	/* selects the lowest noise mode by default */
	adf4153_update_latch(dev,
			     ADF4153_CTRL_NOISE_SPUR |
			     0x3C7);
	/* Set up the control register and enable the counter reset */
	adf4153_update_latch(dev,
			     ADF4153_CTRL_CONTROL |
			     ADF4153_R2_COUNTER_RST(ADF4153_CR_ENABLED) |
			     ADF4153_R2_CP_3STATE(dev->adf4153_st.cp_three_state) |
			     ADF4153_R2_POWER_DOWN(dev->adf4153_st.power_down) |
			     ADF4153_R2_LDP(dev->adf4153_st.ldp) |
			     ADF4153_R2_PD_POL(dev->adf4153_st.pd_polarity) |
			     ADF4153_R2_CP_CURRENT(dev->adf4153_st.cp_current) |
			     ADF4153_R2_REF_DOUBLER(dev->adf4153_st.ref_doubler) |
			     ADF4153_R2_RESYNC(dev->adf4153_st.resync)
			    );
	/* If resync feature is enabled */
	if(init_param.adf4153_st.resync != 0x0) {
		/* Load the R divider register */
		adf4153_update_latch(dev,
				     ADF4153_CTRL_R_DIVIDER |
				     ADF4153_R1_MOD(10) |    //Resync Delay
				     ADF4153_R1_RCOUNTER(dev->adf4153_st.r_counter) |
				     ADF4153_R1_PRESCALE(dev->adf4153_st.prescaler) |
				     ADF4153_R1_MUXOUT(dev->adf4153_st.muxout) |
				     ADF4153_R1_LOAD(ADF4153_LOAD_RESYNC)
				    );
	}
	/* Load the R divider register */
	adf4153_update_latch(dev,
			     ADF4153_CTRL_R_DIVIDER |
			     ADF4153_R1_MOD(dev->adf4153_st.mod_value) |
			     ADF4153_R1_RCOUNTER(dev->adf4153_st.r_counter) |
			     ADF4153_R1_PRESCALE(dev->adf4153_st.prescaler) |
			     ADF4153_R1_MUXOUT(dev->adf4153_st.muxout) |
			     ADF4153_R1_LOAD(ADF4153_LOAD_NORMAL)
			    );
	/* Load the N divider register */
	adf4153_update_latch(dev,
			     ADF4153_CTRL_N_DIVIDER |
			     ADF4153_R0_FRAC(dev->adf4153_st.frac_value) |
			     ADF4153_R0_INT(dev->adf4153_st.int_value) |
			     ADF4153_R0_FASTLOCK(dev->adf4153_st.fastlock)
			    );

	/* Disable the counter reset in the Control Register */
	dev->r2 &= ~ADF4153_R2_COUNTER_RST(ADF4153_R2_COUNTER_RST_MASK);
	adf4153_update_latch(dev,
			     ADF4153_CTRL_CONTROL |
			     dev->r2 |
			     ADF4153_R2_COUNTER_RST(ADF4153_CR_DISABLED)
			    );

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adf4153_init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t adf4153_remove(struct adf4153_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	ret |= no_os_gpio_remove(dev->gpio_le);
	ret |= no_os_gpio_remove(dev->gpio_ce);
	ret |= no_os_gpio_remove(dev->gpio_le2);
	ret |= no_os_gpio_remove(dev->gpio_ce2);

	no_os_free(dev);

	return ret;
}

/**************************************************************************//**
 * @brief Update one of the latch via the SPI interface
 *
 * @param dev          - The device structure.
 * @param   latch_data - the data which will be written to the latch
 *
 * @return
******************************************************************************/
void adf4153_update_latch(struct adf4153_dev *dev,
			  uint32_t latch_data)
{
	uint8_t data_buffer[3] = {0,};
	uint8_t latch_type = latch_data & 0x3;

	/* Update the internal buffers */
	switch(latch_type) {
	case ADF4153_CTRL_N_DIVIDER :
		dev->r0 = latch_data;
		break;
	case ADF4153_CTRL_R_DIVIDER :
		dev->r1 = latch_data;
		break;
	case ADF4153_CTRL_CONTROL :
		dev->r2 = latch_data;
		break;
	case ADF4153_CTRL_NOISE_SPUR :
		dev->r3 = latch_data;
		break;
	}

	data_buffer[0] = (latch_data & DATA_MASK_MSB8) >> DATA_OFFSET_MSB8;
	data_buffer[1] = (latch_data & DATA_MASK_MID8) >> DATA_OFFSET_MID8;
	data_buffer[2] = (latch_data & DATA_MASK_LSB8) >> DATA_OFFSET_LSB8;

	no_os_spi_write_and_read(dev->spi_desc,
				 data_buffer,
				 3);

	/* Generate a load pulse */
	ADF4153_LE_HIGH;
	ADF4153_LE_LOW;
}

/***************************************************************************//**
 * @brief Return the value of a desired latch
 *
 * @param dev        - The device structure.
 * @param latch_type - the type of the latch:
 *                      0 - 'ADF4153_CTRL_N_DIVIDER'
 *                      1 - 'ADF4153_CTRL_R_DIVIDER'
 *                      2 - 'ADF4153_CTRL_CONTROL'
 *                      3 - 'ADF4153_CTRL_NOISE_SPUR'
 *
 * @return latchValue - the value of the desired latch
*******************************************************************************/
uint32_t adf4153_read_latch(struct adf4153_dev *dev,
			    uint8_t latch_type)
{
	switch(latch_type) {
	case ADF4153_CTRL_N_DIVIDER :
		return dev->r0;

	case ADF4153_CTRL_R_DIVIDER :
		return dev->r1;

	case ADF4153_CTRL_CONTROL :
		return dev->r2;

	case ADF4153_CTRL_NOISE_SPUR :
		return dev->r3;

	default :
		return -1;
	}
}

/***************************************************************************//**
 * @brief Increases the R counter value until the ADF4106_PDF_MAX_FREQ is
 *        greater than PFD frequency.
 *
 * @param dev       - The device structure.
 * @param r_counter - pointer of the R counter variable.
 *
 * @return pfdFrequency - The value of the PFD frequency.
*******************************************************************************/
uint32_t adf4153_tune_rcounter(struct adf4153_dev *dev,
			       uint16_t *r_counter)
{
	uint32_t pfd_frequency = 0;              // PFD frequency
	uint8_t ref_doubler = 0;                // Reference Doubler

	ref_doubler = (dev->r2 & ADF4153_R2_REF_DOUBLER(ADF4153_R2_RESYNC_MASK)) >> \
		      ADF4153_R2_REF_DOUBLER_OFFSET; // the actual reference doubler
	do {
		(*r_counter)++;
		pfd_frequency = dev->adf4153_st.ref_in * \
				((float)(1 + ref_doubler) / (*r_counter));
	} while(pfd_frequency > dev->adf4153_pfd_max_frq);

	return pfd_frequency;
}

/***************************************************************************//**
 * @brief Sets the output frequency.
 *
 * @param dev       - The device structure.
 * @param frequency - The desired frequency value.
 *
 * @return calculatedFrequency - The actual frequency value that was set.
*******************************************************************************/
uint64_t adf4153_set_frequency(struct adf4153_dev *dev,
			       uint64_t frequency)
{
	uint64_t vco_frequency = 0;     // VCO frequency
	uint32_t pfd_frequency = 0;     // PFD frequency
	uint64_t calculated_frequency = 0;     // Actual VCO frequency
	uint32_t int_value = 0;     // INT value
	uint32_t frac_value = 0;     // FRAC value
	uint32_t mod_value = 0;     // MOD value
	uint16_t r_counter = 0;     // R Counter
	float buffer = 0;
	uint8_t device_prescaler = 0;
	uint8_t int_min = 0;
	/* validate the given frequency parameter */
	if(frequency <= dev->adf4153_vco_max_frq) {
		if(frequency >= dev->adf4153_vco_min_frq) {
			vco_frequency = frequency;
		} else {
			vco_frequency = dev->adf4153_vco_min_frq;
		}
	} else {
		vco_frequency = dev->adf4153_vco_max_frq;
	}

	/* define the value of MOD */
	mod_value = CEIL(dev->adf4153_st.ref_in,
			 dev->adf4153_st.channel_spacing);
	/* if the mod_value is too high, increase the channel spacing */
	if(mod_value > dev->adf4153_mod_max) {
		do {
			dev->adf4153_st.channel_spacing++;
			mod_value = CEIL(dev->adf4153_st.ref_in,
					 dev->adf4153_st.channel_spacing);
		} while(mod_value <= dev->adf4153_mod_max);
	}
	/* define prescaler */
	device_prescaler = (vco_frequency <= FREQ_2_GHZ) ? ADF4153_PRESCALER_4_5 : \
			   ADF4153_PRESCALER_8_9;
	int_min = (device_prescaler == ADF4153_PRESCALER_4_5) ? 31 : 91;
	/* define the PFD frequency, R counter ant INT value */
	do {
		/* define the PFD frequency and R Counter, using the TuneRCounter() */
		pfd_frequency = adf4153_tune_rcounter(dev,
						      &r_counter);
		int_value = vco_frequency / pfd_frequency;
	} while(int_value < int_min);
	/*define FRAC value */
	do {
		frac_value++;
		buffer = int_value + ((float)frac_value/mod_value);
		calculated_frequency = (uint64_t)(buffer * pfd_frequency);
	} while(calculated_frequency <= vco_frequency);
	frac_value--;

	/* Find the actual VCO frequency. */
	buffer = int_value + ((float)frac_value/mod_value);
	calculated_frequency = (uint64_t)(buffer * pfd_frequency);

	/* Enable the Counter Reset */
	adf4153_update_latch(dev,
			     ADF4153_CTRL_CONTROL |
			     dev->r2 |
			     ADF4153_R2_COUNTER_RST(ADF4153_CR_ENABLED));

	/* Load the R divider with the new values */
	dev->r1 &= (~ADF4153_R1_MOD(ADF4153_R1_MOD_MASK) &
		    ~ADF4153_R1_RCOUNTER(ADF4153_R1_RCOUNTER_MASK) &
		    ~ADF4153_R1_PRESCALE(ADF4153_R1_PRESCALE_MASK));
	adf4153_update_latch(dev,
			     ADF4153_CTRL_R_DIVIDER |
			     dev->r1 |
			     ADF4153_R1_MOD(mod_value) |
			     ADF4153_R1_RCOUNTER(r_counter) |
			     ADF4153_R1_PRESCALE(device_prescaler));

	/* Load the N divider with the new values */
	dev->r0 &= (~ADF4153_R0_FRAC(ADF4153_R0_FRAC_MASK) &
		    ~ADF4153_R0_INT(ADF4153_R0_INT_MASK));
	adf4153_update_latch(dev,
			     ADF4153_CTRL_N_DIVIDER |
			     dev->r0 |
			     ADF4153_R0_FRAC(frac_value) |
			     ADF4153_R0_INT(int_value));
	/* Disable the Counter Reset */
	dev->r2 &= ~ADF4153_R2_COUNTER_RST(ADF4153_R2_COUNTER_RST_MASK);
	adf4153_update_latch(dev,
			     ADF4153_CTRL_CONTROL |
			     dev->r2 |
			     ADF4153_R2_COUNTER_RST(ADF4153_CR_DISABLED));

	return calculated_frequency;
}

/***************************************************************************//**
 * @brief Return the value of the channel spacing
 *
 * @param dev - The device structure.
 *
 * @return calculatedFrequency - The actual frequency value that was set.
*******************************************************************************/
uint32_t adf4153_get_channel_spacing(struct adf4153_dev *dev)
{
	return dev->adf4153_st.channel_spacing;
}
