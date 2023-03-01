/**************************************************************************//**
*   @file   adf4156.c
*   @brief  Implementation of ADF4156 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "adf4156.h"
#include "adf4156_cfg.h"
#include "no_os_alloc.h"

/***************************************************************************//**
 * @brief Initialize the SPI communication with the device.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
int8_t adf4156_init(struct adf4156_dev **device,
		    struct adf4156_init_param init_param)
{
	struct adf4156_dev *dev;
	uint32_t cfg_value;
	int8_t status;

	dev = (struct adf4156_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* Setup GPIO pads */
	status = no_os_gpio_get(&dev->gpio_le, &init_param.gpio_le);
	status |= no_os_gpio_get(&dev->gpio_ce, &init_param.gpio_ce);

	/* Setup Control GPIO Pins */
	ADF4156_LE_OUT;
	ADF4156_LE_LOW;
	ADF4156_CE_OUT;
	ADF4156_CE_HIGH;
	//ADF4156_LE2_OUT;
	//ADF4156_CE2_OUT;

	/* Setup SPI Interface */
	status |= no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	/* R4 */
	cfg_value = adf4156_pdata_lpc.r4_user_settings |
		    ADF4156_R4_CTRL;
	adf4156_set(dev,
		    cfg_value);
	dev->adf4156_st.reg_val[ADF4156_REG4] = cfg_value;

	/* R3 */
	cfg_value = adf4156_pdata_lpc.r3_user_settings |
		    ADF4156_R3_CTRL;
	adf4156_set(dev,
		    cfg_value);
	dev->adf4156_st.reg_val[ADF4156_REG3] = cfg_value;

	/* R2 */
	cfg_value = adf4156_pdata_lpc.r2_user_settings |
		    ADF4156_PRESCALER(0)               |
		    (adf4156_pdata_lpc.ref_div2_en ? ADF4156_RDIV2(1) : ADF4156_RDIV2(0))        |
		    (adf4156_pdata_lpc.ref_doubler_en ? ADF4156_REF_DBL(1) : ADF4156_REF_DBL(0)) |
		    ADF4156_R_CNT(1)                   |
		    ADF4156_MOD_WORD(0)                |
		    ADF4156_R2_CTRL;
	adf4156_set(dev,
		    cfg_value);
	dev->adf4156_st.reg_val[ADF4156_REG2] = cfg_value;

	/* R1 */
	cfg_value = ADF4156_PHASE_VAL(1)     |
		    ADF4156_R1_CTRL;
	adf4156_set(dev,
		    cfg_value);
	dev->adf4156_st.reg_val[ADF4156_REG1] = cfg_value;

	/* R0 */
	cfg_value = adf4156_pdata_lpc.r0_user_settings |
		    ADF4156_INT_VAL(1)                 |
		    ADF4156_FRAC_VAL(1)                |
		    ADF4156_R0_CTRL;
	adf4156_set(dev,
		    cfg_value);
	dev->adf4156_st.reg_val[ADF4156_REG0] = cfg_value;

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adf4156_init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t adf4156_remove(struct adf4156_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	ret |= no_os_gpio_remove(dev->gpio_le);
	ret |= no_os_gpio_remove(dev->gpio_ce);

	no_os_free(dev);

	return ret;
}

/**************************************************************************//**
 * @brief Transmits 32 bits on SPI.
 *
 * @param dev   - The device structure.
 * @param value - Data which will be transmitted.
 *
 * @return none.
******************************************************************************/
int8_t adf4156_set(struct adf4156_dev *dev,
		   uint32_t value)
{
	int8_t validation = 0;
	int8_t status = 0;
	uint8_t tx_buffer[4] = {0, 0, 0, 0};// TX SPI Buffer

	tx_buffer[0] = (uint8_t)((value & 0xFF000000) >> 24);
	tx_buffer[1] = (uint8_t)((value & 0x00FF0000) >> 16);
	tx_buffer[2] = (uint8_t)((value & 0x0000FF00) >> 8);
	tx_buffer[3] = (uint8_t)((value & 0x000000FF) >> 0);

	ADF4156_LE_LOW;
	validation = no_os_spi_write_and_read(dev->spi_desc,
					      tx_buffer,
					      4);
	if (validation != 4) {
		status = -1;
	}
	ADF4156_LE_HIGH;

	return status;
}

/***************************************************************************//**
 * @brief Increases the R counter value until the PFD frequency is
 *        smaller than ADF4351_MAX_FREQ_PFD.
 *
 * @param dev   - The device structure.
 * @param r_cnt - Initial r_cnt value.
 *
 * @return Final r_cnt value.
*******************************************************************************/
int32_t adf4156_tune_r_cnt(struct adf4156_dev *dev,
			   int32_t r_cnt)
{
	struct adf4156_platform_data *pdata = dev->adf4156_st.pdata;

	do {
		r_cnt++;
		dev->adf4156_st.fpfd = (pdata->clkin * \
					(pdata->ref_doubler_en ? 2 : 1)) /
				       (r_cnt * (pdata->ref_div2_en ? 2 : 1));
	} while (dev->adf4156_st.fpfd > ADF4156_MAX_FREQ_PFD);

	return r_cnt;
}

/***************************************************************************//**
 * @brief Computes the greatest common divider of two numbers.
 *
 * @return Returns the gcd.
*******************************************************************************/
uint32_t gcd(uint32_t x, uint32_t y)
{
	uint32_t tmp;

	while (y != 0) {
		tmp = x % y;
		x = y;
		y = tmp;
	}

	return x;
}

/***************************************************************************//**
 * @brief Sets the ADF4156 output frequency.
 *
 * @param dev     - The device structure.
 * @param freq    - The desired frequency value.
 *
 * @return calculatedFrequency - The actual frequency value that was set.
*******************************************************************************/
double adf4156_set_freq(struct adf4156_dev *dev,
			double freq)
{
	uint64_t tmp;
	uint32_t       div_gcd, prescaler, chspc;
	uint16_t mdiv;
	uint16_t r_cnt = 0;
	double result;


	if ((freq > ADF4156_MAX_OUT_FREQ) || (freq < ADF4156_MIN_OUT_FREQ)) {
		return -1;
	}
	if (freq > ADF4156_MAX_FREQ_45_PRESC) {
		prescaler = ADF4156_PRESCALER(1);
		mdiv = 75;
	} else {
		prescaler = ADF4156_PRESCALER(0);
		mdiv = 23;
	}

	freq *= 1000000;
	chspc = dev->adf4156_st.pdata->channel_spacing;

	if ((dev->adf4156_st.pdata->clkin > ADF4156_MAX_FREQ_REFIN) ||
	    (dev->adf4156_st.pdata->clkin < ADF4156_MIN_FREQ_REFIN)) {
		return -1;
	}

	do {
		do {
			r_cnt = adf4156_tune_r_cnt(dev, r_cnt);
			dev->adf4156_st.r2_mod = \
						 dev->adf4156_st.fpfd / chspc;
			if (r_cnt > ADF4156_MAX_R_CNT) {
				printf("Error! R-counter has a too high \
value for the selected spacing. Try with a higher spacing value.\r\n");
				return -1;
			}
		} while ((dev->adf4156_st.r2_mod > ADF4156_MAX_MODULUS) && r_cnt);
		dev->adf4156_st.r_cnt = r_cnt;
		tmp = freq * (uint64_t)dev->adf4156_st.r2_mod + (dev->adf4156_st.fpfd >> 1);
		tmp = (uint64_t)(tmp / dev->adf4156_st.fpfd);
		dev->adf4156_st.r0_fract = tmp % dev->adf4156_st.r2_mod;
		tmp = tmp / dev->adf4156_st.r2_mod;
		dev->adf4156_st.r0_int = (uint32_t)tmp;
	} while (mdiv > dev->adf4156_st.r0_int);

	if (dev->adf4156_st.r0_fract && dev->adf4156_st.r2_mod) {
		div_gcd = gcd(dev->adf4156_st.r2_mod,
			      dev->adf4156_st.r0_fract);
		dev->adf4156_st.r2_mod /= div_gcd;
		dev->adf4156_st.r0_fract /= div_gcd;
	} else {
		dev->adf4156_st.r0_fract = 0;
		dev->adf4156_st.r2_mod = 1;
	}

	/* register R3 */
	dev->adf4156_st.reg_val[ADF4156_REG3] &= ~(ADF4156_CNT_RST(-1));
	dev->adf4156_st.reg_val[ADF4156_REG3] |= (ADF4156_CNT_RST(1));

	adf4156_set(dev,
		    dev->adf4156_st.reg_val[ADF4156_REG3]);

	/* register R2 */
	dev->adf4156_st.reg_val[ADF4156_REG2] &= ~(ADF4156_MOD_WORD(-1) |
			ADF4156_R_CNT(-1)    |
			ADF4156_PRESCALER(-1));
	dev->adf4156_st.reg_val[ADF4156_REG2] |= (ADF4156_MOD_WORD(
				dev->adf4156_st.r2_mod) |
			ADF4156_R_CNT(dev->adf4156_st.r_cnt)     |
			prescaler);
	adf4156_set(dev,
		    dev->adf4156_st.reg_val[ADF4156_REG2]);

	/* register R0 */
	dev->adf4156_st.reg_val[ADF4156_REG0] &= ~(ADF4156_INT_VAL(-1) |
			ADF4156_FRAC_VAL(-1));
	dev->adf4156_st.reg_val[ADF4156_REG0] |= (ADF4156_INT_VAL(
				dev->adf4156_st.r0_int) |
			ADF4156_FRAC_VAL(dev->adf4156_st.r0_fract));
	adf4156_set(dev,
		    dev->adf4156_st.reg_val[ADF4156_REG0]);

	/* register R3 */
	dev->adf4156_st.reg_val[ADF4156_REG3] &= ~(ADF4156_CNT_RST(-1));
	adf4156_set(dev,
		    dev->adf4156_st.reg_val[ADF4156_REG3]);

	result = dev->adf4156_st.r0_int * (float)(dev->adf4156_st.fpfd / 1000000);
	result = result + ((float)dev->adf4156_st.r0_fract / dev->adf4156_st.r2_mod) *\
		 (dev->adf4156_st.fpfd / 1000000);

	return result;
}
