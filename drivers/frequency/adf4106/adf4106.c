/******************************************************************************
*   @file   ADF4106.c
*   @brief  Implementation of ADF4106 Driver for Microblaze processor.
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
#include "adf4106.h"
#include "no_os_alloc.h"

#define DATA_MASK_MSB8      0xFF0000
#define DATA_OFFSET_MSB8    16
#define DATA_MASK_MID8      0xFF00
#define DATA_OFFSET_MID8    8
#define DATA_MASK_LSB8      0xFF
#define DATA_OFFSET_LSB8    0
#define ADDRESS_MASK        3

#define FREQ_2_4_GHZ        2400000000u
#define FREQ_5_2_GHZ        5200000000u

/******************************************************************************/
/************************** Constants Definitions *****************************/
/******************************************************************************/

const struct adf4106_chip_info chip_info[] = {
	[ID_ADF4001] = {
		.vco_max_frequency = 200000000,           // 200  Mhz
		.pfd_max_frequency = 55000000,            // 55   Mhz
		.vco_min_frequency = 5000000,             // 5    Mhz
		.pfd_min_frequency = 5000000,             // 5    Mhz
	},
	[ID_ADF4002] = {
		.vco_max_frequency = 400000000,           // 400  Mhz
		.pfd_max_frequency = 104000000,           // 104  Mhz
		.vco_min_frequency = 5000000,             // 5    Mhz
		.pfd_min_frequency = 20000000,            // 20   Mhz
	},
	[ID_ADF4106] = {
		.vco_max_frequency = 6000000000,          // 6    Ghz
		.pfd_max_frequency = 104000000,           // 104  Mhz
		.vco_min_frequency = 500000000,           // 500  Mhz
		.pfd_min_frequency = 20000000,            // 20   Mhz
	}
};

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
int8_t adf4106_init(struct adf4106_dev **device,
		    struct adf4106_init_param init_param)
{
	struct adf4106_dev *dev;
	int8_t status = -1;

	dev = (struct adf4106_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* lock the current device */
	dev->chip_info = chip_info[init_param.this_device];

	dev->adf4106_st = init_param.adf4106_st;
	dev->this_device = init_param.this_device;

	dev->r_latch = 0;
	dev->n_latch = 0;
	dev->f_latch = 0;
	dev->i_latch = 0;

	/* CPHA = 1; CPOL = 0; */
	status = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	/* GPIO */
	status |= no_os_gpio_get(&dev->gpio_le, &init_param.gpio_le);
	status |= no_os_gpio_get(&dev->gpio_ce, &init_param.gpio_ce);
	status |= no_os_gpio_get(&dev->gpio_le2, &init_param.gpio_le2);
	status |= no_os_gpio_get(&dev->gpio_ce2, &init_param.gpio_ce2);

	/* Bring CE high to put device to power up */
	ADF4106_CE_OUT;
	ADF4106_CE_HIGH;
	/* Initialize the load enable pin */
	ADF4106_LE_OUT;
	ADF4106_LE_LOW;

	/* Import the PFD max limit, from user set up */
	if(dev->chip_info.pfd_max_frequency > dev->adf4106_st.pfd_max) {
		dev->chip_info.pfd_max_frequency = dev->adf4106_st.pfd_max;
	}

	switch(init_param.init_method) {
	case INIT_LATCH :
		adf4106_init_latch_method(dev);
		break;
	case INIT_CEPIN :
		adf4106_init_cepin_method(dev);
		break;
	case INIT_COUNTER_RESET :
		adf4106_init_counte_reset_method(dev);
		break;
	}

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adf4106_init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t adf4106_remove(struct adf4106_dev *dev)
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
 * @param dev        - The device structure.
 * @param latch_data - the data which will be written to the latch
 *
 * @return
******************************************************************************/
void adf4106_update_latch(struct adf4106_dev *dev,
			  uint32_t latch_data)
{
	uint8_t data_buffer[3] = {0,};
	uint8_t latch_type = latch_data & 0x3;

	/* Update the internal buffers */
	switch(latch_type) {
	case ADF4106_CTRL_R_COUNTER :
		dev->r_latch = latch_data;
		break;
	case ADF4106_CTRL_N_COUNTER :
		dev->n_latch = latch_data;
		break;
	case ADF4106_CTRL_FUNCTION_LATCH :
		dev->f_latch = latch_data;
		break;
	case ADF4106_CTRL_INIT_LATCH :
		dev->i_latch = latch_data;
		dev->f_latch = ADF4106_CTRL_FUNCTION_LATCH |
			       (latch_data & ~ADF4106_CTRL_MASK);
		break;
	}

	data_buffer[0] = (latch_data & DATA_MASK_MSB8) >> DATA_OFFSET_MSB8;
	data_buffer[1] = (latch_data & DATA_MASK_MID8) >> DATA_OFFSET_MID8;
	data_buffer[2] = (latch_data & DATA_MASK_LSB8) >> DATA_OFFSET_LSB8;

	no_os_spi_write_and_read(dev->spi_desc,
				 data_buffer,
				 3);

	/* Generate a load pulse */
	ADF4106_LE_HIGH;
	ADF4106_LE_LOW;
}

/**************************************************************************//**
 * @brief Initialization latch method
 *
 * @param dev - The device structure.
 *
 * @return
******************************************************************************/
void adf4106_init_latch_method(struct adf4106_dev *dev)
{
	/* Program initialization latch (COUNTER RESET bit is zero) */
	adf4106_update_latch(dev,
			     ADF4106_CTRL_INIT_LATCH | \
			     ADF4106_CR(dev->adf4106_st.counter_reset) | \
			     ADF4106_PD1(dev->adf4106_st.power_down1) | \
			     ADF4106_MUXOUT(dev->adf4106_st.muxout_control) | \
			     ADF4106_PDPOL(dev->adf4106_st.phase_detector_pol) | \
			     ADF4106_CP(dev->adf4106_st.cp_type) | \
			     ADF4106_FASTLOCK(dev->adf4106_st.fast_lock_mode) | \
			     ADF4106_TCC(dev->adf4106_st.timer_counter_control) |\
			     ADF4106_CS1(dev->adf4106_st.current_setting1) | \
			     ADF4106_CS2(dev->adf4106_st.current_setting2) | \
			     ADF4106_PD2(dev->adf4106_st.power_down2) | \
			     ADF4106_PS(dev->adf4106_st.prescaler_value));

	/* Do an R load */
	adf4106_update_latch(dev,
			     ADF4106_CTRL_R_COUNTER | \
			     ADF4106_R_COUNTER(dev->adf4106_st.ref_counter) | \
			     ADF4106_R_ABP(dev->adf4106_st.anti_backlash_width)| \
			     ADF4106_R_TMB(dev->adf4106_st.test_mode_bits) | \
			     ADF4106_R_LDP(dev->adf4106_st.lock_detect_precision));

	/* Do an N load */
	adf4106_update_latch(dev,
			     ADF4106_CTRL_N_COUNTER | \
			     ADF4106_N_COUNTER_B(dev->adf4106_st.b_n_counter) | \
			     ADF4106_N_CP(dev->adf4106_st.cp_gain));

	/* Update the local variable for Function Latch */
	dev->f_latch = ADF4106_CTRL_FUNCTION_LATCH | (dev->i_latch & 0xFFFFFFFC);
}

/**************************************************************************//**
 * @brief CE Pin method
 *
 * @param dev - The device structure.
 *
 * @return
******************************************************************************/
void adf4106_init_cepin_method(struct adf4106_dev *dev)
{
	/* Bring CE low to put the device into power down. */
	ADF4106_CE_LOW;
	/* Program the function latch */
	adf4106_update_latch(dev,
			     ADF4106_CTRL_FUNCTION_LATCH | \
			     ADF4106_CR(dev->adf4106_st.counter_reset) | \
			     ADF4106_PD1(dev->adf4106_st.power_down1) | \
			     ADF4106_MUXOUT(dev->adf4106_st.muxout_control) | \
			     ADF4106_PDPOL(dev->adf4106_st.phase_detector_pol) | \
			     ADF4106_CP(dev->adf4106_st.cp_type) | \
			     ADF4106_FASTLOCK(dev->adf4106_st.fast_lock_mode) | \
			     ADF4106_TCC(dev->adf4106_st.timer_counter_control) |\
			     ADF4106_CS1(dev->adf4106_st.current_setting1) | \
			     ADF4106_CS2(dev->adf4106_st.current_setting2) | \
			     ADF4106_PD2(dev->adf4106_st.power_down2));
	/* Program the R counter latch */
	adf4106_update_latch(dev,
			     ADF4106_CTRL_R_COUNTER | \
			     ADF4106_R_COUNTER(dev->adf4106_st.ref_counter) | \
			     ADF4106_R_ABP(dev->adf4106_st.anti_backlash_width)| \
			     ADF4106_R_TMB(dev->adf4106_st.test_mode_bits) | \
			     ADF4106_R_LDP(dev->adf4106_st.lock_detect_precision));
	/* Program the N counter latch */
	adf4106_update_latch(dev,
			     ADF4106_CTRL_N_COUNTER | \
			     ADF4106_N_COUNTER_B(dev->adf4106_st.b_n_counter) | \
			     ADF4106_N_CP(dev->adf4106_st.cp_gain));
	/* Bring CE high to take the device out of power-down */
	ADF4106_CE_HIGH;
	/* Wait for the input buffer bias to reach steady state */
	no_os_mdelay(1);
}

/**************************************************************************//**
 * @brief Counter reset method
 *
 * @param dev - The device structure.
 *
 * @return
******************************************************************************/
void adf4106_init_counte_reset_method(struct adf4106_dev *dev)
{
	/* Program the function latch, enable the counter reset */
	adf4106_update_latch(dev,
			     ADF4106_CTRL_FUNCTION_LATCH | \
			     ADF4106_CR_RESET | \
			     ADF4106_PD1(dev->adf4106_st.power_down1) | \
			     ADF4106_MUXOUT(dev->adf4106_st.muxout_control) | \
			     ADF4106_PDPOL(dev->adf4106_st.phase_detector_pol) | \
			     ADF4106_CP(dev->adf4106_st.cp_type) | \
			     ADF4106_FASTLOCK(dev->adf4106_st.fast_lock_mode) | \
			     ADF4106_TCC(dev->adf4106_st.timer_counter_control) |\
			     ADF4106_CS1(dev->adf4106_st.current_setting1) | \
			     ADF4106_CS2(dev->adf4106_st.current_setting2) | \
			     ADF4106_PD2(dev->adf4106_st.power_down2));
	/* Program the R counter latch */
	adf4106_update_latch(dev,
			     ADF4106_CTRL_R_COUNTER | \
			     ADF4106_R_COUNTER(dev->adf4106_st.ref_counter) | \
			     ADF4106_R_ABP(dev->adf4106_st.anti_backlash_width)| \
			     ADF4106_R_TMB(dev->adf4106_st.test_mode_bits) | \
			     ADF4106_R_LDP(dev->adf4106_st.lock_detect_precision));
	/* Program the N counter latch */
	adf4106_update_latch(dev,
			     ADF4106_CTRL_N_COUNTER | \
			     ADF4106_N_COUNTER_B(dev->adf4106_st.b_n_counter) | \
			     ADF4106_N_CP(dev->adf4106_st.cp_gain));
	/* Do a Function Load, this time disable the counter reset */
	adf4106_update_latch(dev,
			     ADF4106_CTRL_FUNCTION_LATCH | \
			     ADF4106_CR_NORMAL | \
			     ADF4106_PD1(dev->adf4106_st.power_down1) | \
			     ADF4106_MUXOUT(dev->adf4106_st.muxout_control) | \
			     ADF4106_PDPOL(dev->adf4106_st.phase_detector_pol) | \
			     ADF4106_CP(dev->adf4106_st.cp_type) | \
			     ADF4106_FASTLOCK(dev->adf4106_st.fast_lock_mode) | \
			     ADF4106_TCC(dev->adf4106_st.timer_counter_control) |\
			     ADF4106_CS1(dev->adf4106_st.current_setting1) | \
			     ADF4106_CS2(dev->adf4106_st.current_setting2) | \
			     ADF4106_PD2(dev->adf4106_st.power_down2));
}

/***************************************************************************//**
 * @brief Return the value of a desired latch
 *
 * @param dev        - The device structure.
 * @param latch_type - the type of the latch:
 *                      0 - 'ADF4106_CTRL_R_COUNTER'
 *                      1 - 'ADF4106_CTRL_N_COUNTER'
 *                      2 - 'ADF4106_CTRL_FUNCTION_LATCH'
 *                      3 - 'ADF4106_CTRL_INIT_LATCH'
 *
 * @return latchValue - the value of the desired latch
*******************************************************************************/
uint32_t adf4106_read_latch(struct adf4106_dev *dev,
			    uint8_t latch_type)
{
	switch(latch_type) {
	case ADF4106_CTRL_R_COUNTER :
		return dev->r_latch;

	case ADF4106_CTRL_N_COUNTER :
		return dev->n_latch;

	case ADF4106_CTRL_FUNCTION_LATCH :
		return dev->f_latch;

	case ADF4106_CTRL_INIT_LATCH :
		return dev->i_latch;

	default :
		return -1;
	}
}

/***************************************************************************//**
 * @brief Increases the R counter value until the ADF4106_PDF_MAX_FREQ is
 *        greater than PFD frequency.
 *
 * @param dev      - The device structure.
 * @param r_counter - R counter value.
 *
 * @return rCounter - modified R counter value.
*******************************************************************************/
uint16_t adf4106_tune_rcounter(struct adf4106_dev *dev,
			       uint16_t r_counter)
{
	uint32_t frequency_pfd = 0; // PFD frequency

	do {
		r_counter++;
		frequency_pfd = dev->adf4106_st.ref_in / r_counter;
	} while(frequency_pfd > dev->chip_info.pfd_max_frequency);

	return r_counter;
}

/***************************************************************************//**
 * @brief Sets the output frequency.
 *
 * @param dev       - The device structure.
 * @param frequency - The desired frequency value.
 *
 * @return calculatedFrequency - The actual frequency value that was set.
*******************************************************************************/
uint64_t adf4106_set_frequency(struct adf4106_dev *dev,
			       uint64_t frequency)
{
	uint64_t vco_frequency = 0; // VCO frequency
	uint32_t frequency_pfd = 0; // PFD frequency
	uint32_t freq_ratio = 0; // VCOfreq / PFDfreq
	uint64_t calculated_frequency = 0; // Actual VCO frequency
	uint16_t r_counter_value = 0; // Value for R counter
	uint16_t a = 0; // Value for A counter
	uint16_t b = 0; // Value for B counter
	uint8_t device_prescaler = 0;
	uint8_t user_prescaler = 0; // prescaler defined by user

	/* Force "frequency" parameter to the [minMHz...maxGHz] interval. */
	if(frequency <= dev->chip_info.vco_max_frequency) {
		if(frequency >= dev->chip_info.vco_min_frequency) {
			vco_frequency = frequency;
		} else {
			vco_frequency = dev->chip_info.vco_min_frequency;
		}
	} else {
		vco_frequency = dev->chip_info.vco_max_frequency;
	}
	do {
		/* Get the actual PFD frequency. */
		r_counter_value = adf4106_tune_rcounter(dev,
							r_counter_value);
		frequency_pfd = dev->adf4106_st.ref_in / r_counter_value;
		/* Define the value of the prescaler
		*  By default the prescaler is constant for ADF4001 and ADF4002
		*  In case of ADF4106, the minimum admitted prescaler will be selected,
		*  or the user defined prescaler if it is valid (big enough)
		*  Valid min prescaler : VCO < 2.4Ghz -> 8/9
		*                        VCO > 2.4Ghz and VCO < 5.4Ghz -> 16/17
		*                        VCO > 5.4Ghz -> 32/33
		*/
		user_prescaler = ADF4106_PRESCALE(dev->f_latch >> ADF4106_PS_OFFSET);
		device_prescaler = (dev->this_device != ID_ADF4106) ? \
				   ADF4106_PRESCALE(ADF4106_PS_8_9) : \
				   (vco_frequency <= FREQ_2_4_GHZ) ? \
				   ADF4106_PRESCALE(ADF4106_PS_8_9) : \
				   ((vco_frequency > FREQ_2_4_GHZ) & \
				    (vco_frequency < FREQ_5_2_GHZ)) ? \
				   ADF4106_PRESCALE(ADF4106_PS_16_17) : \
				   ADF4106_PRESCALE(ADF4106_PS_32_33);
		if(device_prescaler < user_prescaler) {
			device_prescaler = user_prescaler;
		}
		/* Find the values for Counter A and Counter B using VCO frequency and
		*  PFD frequency.
		*/
		freq_ratio = (uint16_t)((float)vco_frequency / frequency_pfd);
		b = freq_ratio / device_prescaler;
		a = freq_ratio % device_prescaler;
	} while(a > b); // B must be greater or equal to A
	/* Find the actual VCO frequency. */
	calculated_frequency = (uint64_t)((b * device_prescaler) + a) * frequency_pfd;
	/* Load the saved values into the registers using Counter Reset Method. */
	adf4106_update_latch(dev,
			     ADF4106_CTRL_FUNCTION_LATCH |
			     dev->f_latch |
			     ADF4106_CR(ADF4106_CR_RESET));

	dev->r_latch &= ~ADF4106_R_COUNTER(ADF4106_R_COUNTER_MASK);
	adf4106_update_latch(dev,
			     ADF4106_CTRL_R_COUNTER |
			     dev->r_latch |
			     ADF4106_R_COUNTER(r_counter_value));

	dev->n_latch &= ~(ADF4106_N_COUNTER_A(ADF4106_N_COUNTER_A_MASK) |
			  ADF4106_N_COUNTER_B(ADF4106_N_COUNTER_B_MASK));
	if(dev->this_device == ID_ADF4106) {
		adf4106_update_latch(dev,
				     ADF4106_CTRL_N_COUNTER |
				     dev->n_latch |
				     ADF4106_N_COUNTER_A(a) |
				     ADF4106_N_COUNTER_B(b));
	} else {
		adf4106_update_latch(dev,
				     ADF4106_CTRL_N_COUNTER |
				     dev->n_latch |
				     ADF4106_N_COUNTER_B((b * device_prescaler) + a));
	}

	dev->f_latch &= ~ADF4106_CR(ADF4106_CR_MASK);
	adf4106_update_latch(dev,
			     ADF4106_CTRL_FUNCTION_LATCH |
			     dev->f_latch |
			     ADF4106_CR(ADF4106_CR_NORMAL));

	return calculated_frequency;
}
