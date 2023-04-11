/***************************************************************************//**
 *   @file   axi_adc_core.c
 *   @brief  Driver for the Analog Devices AXI-ADC-CORE module.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "axi_adc_core.h"
#include "no_os_axi_io.h"


/**
 * @brief AXI ADC Data read.
 * @param adc - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - Read data value.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_read(struct axi_adc *adc,
		     uint32_t reg_addr,
		     uint32_t *reg_data)
{
	no_os_axi_io_read(adc->base, reg_addr, reg_data);

	return 0;
}

/**
 * @brief AXI ADC Data Write.
 * @param adc - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - Data value to be written.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_write(struct axi_adc *adc,
		      uint32_t reg_addr,
		      uint32_t reg_data)
{
	no_os_axi_io_write(adc->base, reg_addr, reg_data);

	return 0;
}

/**
 * @brief Slave AXI ADC Data read.
 * @param adc - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - Read data value.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_slave_adc_read(struct axi_adc *adc,
			   uint32_t reg_addr,
			   uint32_t *reg_data)
{
	return no_os_axi_io_read(adc->slave_base, reg_addr, reg_data);
}

/**
 * @brief Slave AXI ADC Data Write.
 * @param adc - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - Data value to be written.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_slave_adc_write(struct axi_adc *adc,
			    uint32_t reg_addr,
			    uint32_t reg_data)
{
	return no_os_axi_io_write(adc->slave_base, reg_addr, reg_data);
}

/**
 * @brief Set AXI ADC PN sequence.
 * @param adc - The device structure.
 * @param chan - The AXI ADC channel.
 * @param sel - PN sequence.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_set_pnsel(struct axi_adc *adc,
			  uint32_t chan,
			  enum axi_adc_pn_sel sel)
{
	uint32_t reg_data;

	axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL_3(chan), &reg_data);
	reg_data &= ~AXI_ADC_ADC_PN_SEL(~0);
	reg_data |= AXI_ADC_ADC_PN_SEL(sel);
	axi_adc_write(adc, AXI_ADC_REG_CHAN_CNTRL_3(chan), reg_data);

	return 0;
}

/**
 * @brief Monitor the AXI ADC PN Sequence.
 * @param adc - The device structure.
 * @param sel - PN sequence.
 * @param delay_ms - Delay in ms.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_pn_mon(struct axi_adc *adc,
		       enum axi_adc_pn_sel sel, uint32_t delay_ms)
{
	uint8_t	ch;
	uint32_t reg_data;

	for (ch = 0; ch < adc->num_channels; ch++) {
		axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL(ch), &reg_data);
		reg_data |= AXI_ADC_ENABLE;
		axi_adc_write(adc, AXI_ADC_REG_CHAN_CNTRL(ch), reg_data);
		axi_adc_set_pnsel(adc, ch, sel);
	}
	no_os_mdelay(1);

	for (ch = 0; ch < adc->num_channels; ch++) {
		axi_adc_write(adc, AXI_ADC_REG_CHAN_STATUS(ch), 0xff);
	}
	no_os_mdelay(delay_ms);

	for (ch = 0; ch < adc->num_channels; ch++) {
		axi_adc_read(adc, AXI_ADC_REG_CHAN_STATUS(ch), &reg_data);
		if (reg_data != 0)
			return -1;
	}

	return 0;
}

/**
 * @brief Get the AXI ADC Sampling Frequency
 * @param adc - The device structure.
 * @param chan - The AXI ADC channel.
 * @param sampling_freq - Sampling frequency value.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_get_sampling_freq(struct axi_adc *adc,
				  uint32_t chan,
				  uint64_t *sampling_freq)
{
	uint32_t freq;
	uint32_t ratio;

	axi_adc_read(adc, AXI_ADC_REG_CLK_FREQ, &freq);
	axi_adc_read(adc, AXI_ADC_REG_CLK_RATIO, &ratio);
	*sampling_freq = freq * ratio;
	*sampling_freq = ((*sampling_freq) * 390625) >> 8;

	return 0;
}

/**
 * @brief Set input/output delay primitive for specific interface line
 * @param adc - The device structure.
 * @param lane - The AXI ADC interface line.
 * @param val - Delay value.
 */
void axi_adc_idelay_set(struct axi_adc *adc,
			uint32_t lane,
			uint32_t val)
{
	axi_adc_write(adc, AXI_ADC_REG_DELAY(lane), val);

}

/**
 * @brief Set input/output delay primitive for number of lines
 * @param adc - The device structure.
 * @param no_of_lanes - The AXI ADC number of lanes.
 * @param delay - Delay value.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_delay_set(struct axi_adc *adc,
			  uint32_t no_of_lanes,
			  uint32_t delay)
{
	uint32_t i;
	uint32_t rdata;
	uint32_t pcore_version;

	axi_adc_read(adc, 0x0, &pcore_version);
	pcore_version >>= 16;
	if (pcore_version < 9) {
		printf(" pcore_version is : %d\n\r", (int)pcore_version);
		printf(" DRIVER DOES NOT SUPPORT PCORE VERSIONS OLDER THAN 10 !");
		return -1;
	} else {
		for (i = 0; i < no_of_lanes; i++) {
			axi_adc_idelay_set(adc, i, delay);
			axi_adc_read(adc, AXI_ADC_REG_DELAY(i), &rdata);
			if (rdata != delay) {
				printf("adc_delay_1: sel(%2d), rcv(%04x), exp(%04x)\n\r",
				       (int)i, (int)rdata, (int)delay);
			}
		}
	}

	return 0;
}

/**
 * @brief Calibrate Delay using specific PN sequence.
 * @param adc - The device structure.
 * @param no_of_lanes - The AXI ADC number of lanes.
 * @param sel - PN sequence.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_delay_calibrate(struct axi_adc *adc,
				uint32_t no_of_lanes,
				enum axi_adc_pn_sel sel)
{
	uint8_t err_field[32] = {0};
	uint16_t valid_range[5] = {0};
	uint16_t invalid_range[5] = {0};
	uint16_t delay = 0;
	uint16_t start_valid_delay = 32;
	uint16_t start_invalid_delay = 32;
	uint8_t interval = 0;
	uint8_t max_interval = 0;
	uint8_t max_valid_range = 0;
	uint8_t cnt_valid[5] = {0};
	uint8_t cnt_invalid = 0;
	uint8_t val = 0;
	uint8_t max_val = 32;

	for (delay = 0; delay < 32; delay++) {
		axi_adc_delay_set(adc, no_of_lanes, delay);
		no_os_mdelay(20);
		if (axi_adc_pn_mon(adc, sel, 100) == 0) {
			err_field[delay] = 0;
			start_valid_delay = start_valid_delay == 32 ?
					    delay : start_valid_delay;
		} else {
			err_field[delay] = 1;
		}
	}
	if (start_valid_delay > 31) {
		printf("%s FAILED.\n", __func__);
		axi_adc_delay_set(adc, no_of_lanes, 0);
		return -1;
	}

	start_valid_delay = 32;
	start_invalid_delay = 32;
	for (val = 0; val < max_val; val++) {
		if (err_field[val] == 0) {
			if (start_valid_delay == 32) {
				start_valid_delay = val;
			}
			if (start_valid_delay != 32 && start_invalid_delay != 32 ) {
				start_valid_delay = 32;
				start_invalid_delay = 32;
			}
			cnt_valid[interval]++;
		}
		if((err_field[val] == 1) || (val == max_val - 1)) {
			if (start_invalid_delay == 32) {
				start_invalid_delay = val;
			}
			if (start_valid_delay != 32 && start_invalid_delay != 32 ) {
				valid_range[interval] = start_valid_delay;
				invalid_range[interval] = start_invalid_delay;
				start_valid_delay = 32;
				start_invalid_delay = 32;
				interval++;
			}
			cnt_invalid++;
		}
	}

	for (val = 0; val < 5; val++) {
		if (cnt_valid[val] > max_valid_range) {
			max_valid_range = cnt_valid[val];
			max_interval = val;
		}
	}

	delay = (valid_range[max_interval] + invalid_range[max_interval] - 1) / 2;

	printf("adc_delay: setting zero error delay (%d)\n\r", delay);
	axi_adc_delay_set(adc, no_of_lanes, delay);

	return 0;
}

/**
 * @brief Calibrate phase for specific AXI ADC channel.
 * @param adc - The device structure.
 * @param phase - The AXI ADC phase value.
 * @param chan - AXI ADC channel.
 * @param val - Integer value
 * @param val2 - Fractional value
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_set_calib_phase_scale(struct axi_adc *adc,
				      uint32_t phase,
				      uint32_t chan,
				      int32_t val,
				      int32_t val2)
{
	uint32_t fract;
	uint64_t llval;
	uint32_t tmp;

	switch (val) {
	case 1:
		fract = 0x4000;
		break;
	case -1:
		fract = 0xC000;
		break;
	case 0:
		fract = 0;
		if (val2 < 0) {
			fract = 0x8000;
			val2 *= -1;
		}
		break;
	default:
		return -1;
	}

	llval = (uint64_t)val2 * 0x4000UL + (1000000UL / 2);
	no_os_do_div(&llval, 1000000UL);
	fract |= llval;

	axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL_2(chan), &tmp);

	if (!((chan + phase) % 2)) {
		tmp &= ~AXI_ADC_IQCOR_COEFF_1(~0);
		tmp |= AXI_ADC_IQCOR_COEFF_1(fract);
	} else {
		tmp &= ~AXI_ADC_IQCOR_COEFF_2(~0);
		tmp |= AXI_ADC_IQCOR_COEFF_2(fract);
	}

	axi_adc_write(adc, AXI_ADC_REG_CHAN_CNTRL_2(chan), tmp);

	return 0;
}

/**
 * @brief Get the phase calibration values for AXI ADC channel.
 * @param adc - The device structure.
 * @param phase - The AXI ADC phase value.
 * @param chan - AXI ADC channel.
 * @param val - Integer value
 * @param val2 - Fractional value
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_get_calib_phase_scale(struct axi_adc *adc,
				      uint32_t phase,
				      uint32_t chan,
				      int32_t *val,
				      int32_t *val2)
{
	uint32_t tmp;
	int32_t sign;
	uint64_t llval;

	axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL_2(chan), &tmp);

	if (!((phase + chan) % 2)) {
		tmp = AXI_ADC_TO_IQCOR_COEFF_1(tmp);
	} else {
		tmp = AXI_ADC_TO_IQCOR_COEFF_2(tmp);
	}

	if (tmp & 0x8000)
		sign = -1;
	else
		sign = 1;

	if (tmp & 0x4000)
		*val = 1 * sign;
	else
		*val = 0;

	tmp &= ~0xC000;

	llval = tmp * 1000000ULL + (0x4000 / 2);
	no_os_do_div(&llval, 0x4000);
	if (*val == 0)
		*val2 = llval * sign;
	else
		*val2 = llval;

	return 0;
}

/**
 * @brief Calibrate phase for specific AXI ADC channel.
 * @param adc - The device structure.
 * @param chan - AXI ADC channel.
 * @param val - Integer value
 * @param val2 - Fractional value
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_set_calib_phase(struct axi_adc *adc,
				uint32_t chan,
				int32_t val,
				int32_t val2)
{
	return axi_adc_set_calib_phase_scale(adc, 1, chan, val, val2);
}

/**
 * @brief Get the phase calibration values for AXI ADC channel.
 * @param adc - The device structure.
 * @param chan - AXI ADC channel.
 * @param val - Integer value
 * @param val2 - Fractional value
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_get_calib_phase(struct axi_adc *adc,
				uint32_t chan,
				int32_t *val,
				int32_t *val2)
{
	return axi_adc_get_calib_phase_scale(adc, 1, chan, val, val2);
}

/**
 * @brief Calibrate scale for specific AXI ADC channel.
 * @param adc - The device structure.
 * @param chan - AXI ADC channel.
 * @param val - Integer value
 * @param val2 - Fractional value
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_set_calib_scale(struct axi_adc *adc,
				uint32_t chan,
				int32_t val,
				int32_t val2)
{
	return axi_adc_set_calib_phase_scale(adc, 0, chan, val, val2);
}

/**
 * @brief Get the scale calibration values for AXI ADC channel.
 * @param adc - The device structure.
 * @param chan - AXI ADC channel.
 * @param val - Integer value
 * @param val2 - Fractional value
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_get_calib_scale(struct axi_adc *adc,
				uint32_t chan,
				int32_t *val,
				int32_t *val2)
{
	return axi_adc_get_calib_phase_scale(adc, 0, chan, val, val2);
}

/**
 * @brief Calibrate bias for specific AXI ADC channel.
 * @param adc - The device structure.
 * @param chan - AXI ADC channel.
 * @param val - Integer value
 * @param val2 - Fractional value
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_set_calib_bias(struct axi_adc *adc,
			       uint32_t chan,
			       int32_t val,
			       int32_t val2)
{
	uint32_t tmp;

	axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL_1(chan), &tmp);
	tmp &= ~AXI_ADC_DCFILT_OFFSET(~0);
	tmp |= AXI_ADC_DCFILT_OFFSET((uint16_t)val);

	axi_adc_write(adc, AXI_ADC_REG_CHAN_CNTRL_1(chan), tmp);

	return 0;
}

/**
 * @brief Get the scale calibration values for AXI ADC channel.
 * @param adc - The device structure.
 * @param chan - AXI ADC channel.
 * @param val - Integer value
 * @param val2 - Fractional value
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_get_calib_bias(struct axi_adc *adc,
			       uint32_t chan,
			       int32_t *val,
			       int32_t *val2)
{
	uint32_t tmp;

	axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL_1(chan), &tmp);
	*val = (uint16_t)AXI_ADC_TO_DCFILT_OFFSET(tmp);

	return 0;
}
/**
 * @brief Update active AXI ADC channels.
 * @param adc - The device structure.
 * @param mask - Channel mask.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_update_active_channels(struct axi_adc *adc, uint32_t mask)
{
	int32_t ret;
	uint32_t ch;
	uint32_t val;
	uint32_t new_val;

	if (mask == adc->mask)
		return 0;

	adc->mask = mask;
	for (ch = 0; ch < adc->num_channels; ch++) {
		if (ch > (adc->num_slave_channels - 1))
			ret = axi_slave_adc_read(adc,
						 AXI_ADC_REG_CHAN_CNTRL(ch - adc->num_slave_channels),
						 &val);
		else
			ret = axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL(ch), &val);

		if (ret)
			return ret;

		new_val = val & (~AXI_ADC_ENABLE);
		if (mask & 1)
			new_val = val | AXI_ADC_ENABLE;
		if (new_val != val) {
			if (ch > (adc->num_slave_channels - 1))
				ret = axi_slave_adc_write(adc,
							  AXI_ADC_REG_CHAN_CNTRL(ch - adc->num_slave_channels),
							  new_val);
			else
				ret = axi_adc_write(adc, AXI_ADC_REG_CHAN_CNTRL(ch), new_val);

			if (ret)
				return ret;
		}
		mask >>= 1;
	}

	return 0;
}

/**
 * @brief Begin AXI ADC Initialization.
 * @param adc_core - The device structure.
 * @param init - Initialization parameters.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_init_begin(struct axi_adc **adc_core,
			   const struct axi_adc_init *init)
{
	struct axi_adc *adc;

	adc = (struct axi_adc *)no_os_malloc(sizeof(*adc));
	if (!adc)
		return -1;

	adc->name = init->name;
	adc->base = init->base;
	adc->slave_base = init->slave_base;
	adc->num_channels = init->num_channels;
	adc->num_slave_channels = init->num_slave_channels;

	*adc_core = adc;

	return 0;
};

/**
 * @brief Begin AXI ADC Initialization.
 * @param adc - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_init_finish(struct axi_adc *adc)
{
	uint32_t reg_data;
	uint32_t freq;
	uint32_t ratio;

	axi_adc_read(adc, AXI_ADC_REG_STATUS, &reg_data);
	if(reg_data == 0x0) {
		printf("%s: Status errors\n", adc->name);
		return -1;
	}

	axi_adc_read(adc, AXI_ADC_REG_CLK_FREQ, &freq);
	axi_adc_read(adc, AXI_ADC_REG_CLK_RATIO, &ratio);
	adc->clock_hz = freq * ratio;
	adc->clock_hz = (adc->clock_hz * 390625) >> 8;

	printf("%s: Successfully initialized (%"PRIu64" Hz)\n",
	       adc->name, adc->clock_hz);

	return 0;
}

/**
 * @brief AXI ADC Main Initialization.
 * @param adc_core - The device structure.
 * @param init - Initialization parameters.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_init(struct axi_adc **adc_core,
		     const struct axi_adc_init *init)
{
	struct axi_adc *adc;
	int32_t ret;
	uint8_t ch;

	ret = axi_adc_init_begin(&adc, init);
	if (ret)
		return ret;

	axi_adc_write(adc, AXI_ADC_REG_RSTN, 0);
	axi_adc_write(adc, AXI_ADC_REG_RSTN,
		      AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);

	for (ch = 0; ch < adc->num_channels; ch++)
		axi_adc_write(adc, AXI_ADC_REG_CHAN_CNTRL(ch),
			      AXI_ADC_FORMAT_SIGNEXT | AXI_ADC_FORMAT_ENABLE |
			      AXI_ADC_ENABLE);

	no_os_mdelay(100);

	ret = axi_adc_init_finish(adc);
	if (ret)
		goto error;

	*adc_core = adc;

	return 0;
error:
	no_os_free(adc);

	return -1;
}

/**
 * @brief AXI ADC Resources deallocation.
 * @param adc - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_remove(struct axi_adc *adc)
{
	no_os_free(adc);

	return 0;
}
