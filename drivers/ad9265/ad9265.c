/***************************************************************************//**
* @file ad9265.c
* @brief Implementation of AD9265 Driver.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* - Neither the name of Analog Devices, Inc. nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
* - The use of this software may or may not infringe the patent rights
* of one or more patent holders. This license does not release you
* from the requirement that you obtain separate licenses from these
* patent holders to use this software.
* - Use of the software either in source or binary form, must be run
* on or directly connected to an Analog Devices Inc. component.
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
#include <stdint.h>
#include <xil_printf.h>
#include "adc_core.h"
#include "platform_drivers.h"
#include "ad9265.h"

#define DCO_DEBUG

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
uint8_t ad9265_slave_select;
uint8_t ad9265_output_mode;

/***************************************************************************//**
* @brief ad9265_spi_read
*******************************************************************************/
int32_t ad9265_spi_read(uint16_t reg_addr, uint8_t *reg_data)
{
	uint8_t buf[3];
	int32_t ret;

	buf[0] = 0x80 | (reg_addr >> 8);
	buf[1] = reg_addr & 0xFF;
	buf[2] = 0x00;

	ret = spi_write_and_read(ad9265_slave_select, buf, 3);
	*reg_data = buf[2];

	return ret;
}

/***************************************************************************//**
* @brief ad9265_spi_write
*******************************************************************************/
int32_t ad9265_spi_write(uint16_t reg_addr, uint8_t reg_data)
{
	uint8_t buf[3];
	int32_t ret;

	buf[0] = reg_addr >> 8;
	buf[1] = reg_addr & 0xFF;
	buf[2] = reg_data;

	ret = spi_write_and_read(ad9265_slave_select, buf, 3);

	return ret;
}

/***************************************************************************//**
* @brief ad9265_setup
*******************************************************************************/
int32_t ad9265_outputmode_set(uint8_t mode)
{
	int32_t ret;

	ret = ad9265_spi_write(AD9265_REG_OUTPUT_MODE, mode);
	if (ret < 0)
		return ret;
	ret = ad9265_spi_write(AD9265_REG_TEST_IO, TESTMODE_OFF);
	if (ret < 0)
		return ret;

	return ad9265_spi_write(AD9265_REG_TRANSFER, TRANSFER_SYNC);
}

/***************************************************************************//**
* @brief ad9265_setup
*******************************************************************************/
int32_t ad9265_testmode_set(uint8_t chan, uint8_t mode)
{
	ad9265_spi_write(AD9265_REG_CHAN_INDEX, 1 << chan);
	ad9265_spi_write(AD9265_REG_TEST_IO, mode);
	ad9265_spi_write(AD9265_REG_CHAN_INDEX, 0x3);
	ad9265_spi_write(AD9265_REG_TRANSFER, TRANSFER_SYNC);

	return 0;
}

/***************************************************************************//**
* @brief ad9265_calibrate
*******************************************************************************/
int32_t ad9265_calibrate(uint8_t dco,
						 uint8_t dco_en,
						 uint8_t nb_lanes,
						 adc_core core)
{
	int32_t ret, val, cnt, start, max_start, max_cnt;
	uint32_t stat, inv_range = 0, do_inv, lane,
		 chan_ctrl0, max_val = dco ? 32 : 31;
	unsigned char err_field[66];
	uint32_t reg_cntrl;

	ret = ad9265_outputmode_set(ad9265_output_mode & ~OUTPUT_MODE_TWOS_COMPLEMENT);
	if (ret < 0)
		return ret;

	adc_read(core, ADC_REG_CHAN_CNTRL(0), &chan_ctrl0);

	do {
		if (!dco) {
			adc_read(core, ADC_REG_CNTRL, &reg_cntrl);

			if (inv_range)
				reg_cntrl |= ADC_DDR_EDGESEL;
			else
				reg_cntrl &= ~ADC_DDR_EDGESEL;
			adc_write(core, ADC_REG_CNTRL, reg_cntrl);
		}

		ad9265_testmode_set(0, TESTMODE_PN9_SEQ);
		adc_write(core, ADC_REG_CHAN_CNTRL(0), ADC_ENABLE);
		adc_set_pnsel(core, 0, ADC_PN9);
		adc_write(core, ADC_REG_CHAN_STATUS(0), ~0);

		for (val = 0; val <= max_val; val++) {
			if (dco) {
				ad9265_spi_write(AD9265_REG_OUTPUT_DELAY,
						val > 0 ? ((val - 1) | dco_en) : 0);
				ad9265_spi_write(AD9265_REG_TRANSFER,
						TRANSFER_SYNC);
			} else {
				for (lane = 0; lane < nb_lanes; lane++) {
					adc_write(core, ADC_REG_DELAY_CNTRL, 0);

					adc_write(core, ADC_REG_DELAY_CNTRL,
							ADC_DELAY_ADDRESS(lane)
							| ADC_DELAY_WDATA(val)
							| ADC_DELAY_SEL);
				}
			}

			adc_write(core, ADC_REG_CHAN_STATUS(0), ~0);

			mdelay(1);

			adc_read(core, ADC_REG_CHAN_STATUS(0), &stat);

			err_field[val + (inv_range * (max_val + 1))] =
			    ! !(stat & (ADC_PN_ERR | ADC_PN_OOS));
		}

		for (val = 0, cnt = 0, max_cnt = 0, start = -1, max_start = 0;
		     val <= (max_val + (inv_range * (max_val + 1))); val++) {
			if (err_field[val] == 0) {
				if (start == -1)
					start = val;
				cnt++;
			} else {
				if (cnt > max_cnt) {
					max_cnt = cnt;
					max_start = start;
				}
				start = -1;
				cnt = 0;
			}
		}

		if (cnt > max_cnt) {
			max_cnt = cnt;
			max_start = start;
		}

		if ((inv_range == 0) && ((max_cnt < 3) || (err_field[max_val] == 0))) {
			do_inv = 1;
			inv_range = 1;
		} else {
			do_inv = 0;
		}

	} while (do_inv);

	val = max_start + (max_cnt / 2);

#ifdef DCO_DEBUG
	for (cnt = 0; cnt <= (max_val + (inv_range * (max_val + 1))); cnt++) {
		if (cnt == val)
			xil_printf("|");
		else
			xil_printf("%c", err_field[cnt] ? '-' : 'o');
		if (cnt == max_val)
			xil_printf("\n");
	}
#endif
	if (val > max_val) {
		val -= max_val + 1;
		if (!dco) {
			adc_read(core, ADC_REG_CNTRL, &reg_cntrl);
			reg_cntrl |= ADC_DDR_EDGESEL;
			adc_write(core, ADC_REG_CNTRL, reg_cntrl);
		}
		cnt = 1;
	} else {
		if (dco) {
			ad9265_spi_write(AD9265_REG_OUTPUT_PHASE,
				 OUTPUT_EVEN_ODD_MODE_EN);
		} else {
			adc_read(core, ADC_REG_CNTRL, &reg_cntrl);
			reg_cntrl &= ~ADC_DDR_EDGESEL;
			adc_write(core, ADC_REG_CNTRL, reg_cntrl);
		}
		cnt = 0;
	}

#ifdef DCO_DEBUG
	if (dco)
		xil_printf(" %s DCO 0x%X\n", cnt ? "INVERT" : "",
				val > 0 ? ((val - 1) | dco_en) : 0);
	else
		xil_printf(" %s IDELAY 0x%x\n", cnt ? "INVERT" : "", val);
#endif

	ad9265_testmode_set(0, TESTMODE_OFF);
	ad9265_testmode_set(1, TESTMODE_OFF);
	if (dco) {
		ad9265_spi_write(AD9265_REG_OUTPUT_DELAY,
				val > 0 ? ((val - 1) | dco_en) : 0);
		ad9265_spi_write(AD9265_REG_TRANSFER, TRANSFER_SYNC);
	} else {
		for (lane = 0; lane < nb_lanes; lane++) {
			adc_write(core, ADC_REG_DELAY_CNTRL, 0);

			adc_write(core, ADC_REG_DELAY_CNTRL,
					ADC_DELAY_ADDRESS(lane)
					| ADC_DELAY_WDATA(val)
					| ADC_DELAY_SEL);
		}
	}

	adc_write(core, ADC_REG_CHAN_CNTRL(0), chan_ctrl0);

	ret = ad9265_outputmode_set(ad9265_output_mode);
	if (ret < 0)
		return ret;

	return 0;
}

/***************************************************************************//**
* @brief ad9265_setup
*******************************************************************************/
int32_t ad9265_setup(uint32_t spi_device_id,
					 uint8_t slave_select,
					 adc_core core)
{
	uint8_t chip_id;

	ad9265_slave_select = slave_select;
	spi_init(spi_device_id, 0, 0);

	ad9265_spi_read(AD9265_REG_CHIP_ID, &chip_id);
	if(chip_id != AD9265_CHIP_ID)
	{
		xil_printf("Error: Invalid CHIP ID (0x%x).\n", chip_id);
		return -1;
	}

	ad9265_output_mode = AD9265_DEF_OUTPUT_MODE | OUTPUT_MODE_TWOS_COMPLEMENT;
	ad9265_outputmode_set(ad9265_output_mode);

	ad9265_calibrate(1, 0, 0, core);

	xil_printf("AD9265 successfully initialized.\n");

	return 0;
}
