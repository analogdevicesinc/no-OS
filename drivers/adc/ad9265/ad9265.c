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
#include <stdio.h>
#include <stdlib.h>
#include "axi_adc_core.h"
#include "ad9265.h"
#include "no_os_alloc.h"

#define DCO_DEBUG

/***************************************************************************//**
* @brief ad9265_spi_read
*******************************************************************************/
int32_t ad9265_spi_read(struct ad9265_dev *dev,
			uint16_t reg_addr,
			uint8_t *reg_data)
{
	uint8_t buf[3];

	int32_t ret;

	buf[0] = 0x80 | (reg_addr >> 8);
	buf[1] = reg_addr & 0xFF;
	buf[2] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 3);
	*reg_data = buf[2];

	return ret;
}

/***************************************************************************//**
* @brief ad9265_spi_write
*******************************************************************************/
int32_t ad9265_spi_write(struct ad9265_dev *dev,
			 uint16_t reg_addr,
			 uint8_t reg_data)
{
	uint8_t buf[3];

	int32_t ret;

	buf[0] = reg_addr >> 8;
	buf[1] = reg_addr & 0xFF;
	buf[2] = reg_data;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 3);

	return ret;
}

/***************************************************************************//**
* @brief ad9265_setup
*******************************************************************************/
int32_t ad9265_outputmode_set(struct ad9265_dev *dev,
			      uint8_t mode)
{
	int32_t ret;

	ret = ad9265_spi_write(dev, AD9265_REG_OUTPUT_MODE, mode);
	if (ret < 0)
		return ret;
	ret = ad9265_spi_write(dev, AD9265_REG_TEST_IO, TESTMODE_OFF);
	if (ret < 0)
		return ret;

	return ad9265_spi_write(dev, AD9265_REG_TRANSFER, TRANSFER_SYNC);
}

/***************************************************************************//**
* @brief ad9265_setup
*******************************************************************************/
int32_t ad9265_testmode_set(struct ad9265_dev *dev,
			    uint8_t mode)
{
	ad9265_spi_write(dev, AD9265_REG_TEST_IO, mode);
	ad9265_spi_write(dev, AD9265_REG_TRANSFER, TRANSFER_SYNC);

	return 0;
}

/***************************************************************************//**
* @brief ad9265_calibrate
*******************************************************************************/
int32_t ad9265_calibrate(struct ad9265_dev *dev,
			 struct ad9265_init_param init_param,
			 struct axi_adc core)
{
	int32_t ret, val, cnt, start, max_start, max_cnt;
	uint32_t stat, inv_range = 0, do_inv, lane,
		       chan_ctrl0, max_val = init_param.dco ? 32 : 31;
	uint8_t err_field[66];

	uint32_t reg_cntrl;

	ret = ad9265_outputmode_set(dev,
				    init_param.output_mode  &
				    ~OUTPUT_MODE_TWOS_COMPLEMENT);
	if (ret < 0)
		return ret;

	axi_adc_read(&core, AXI_ADC_REG_CHAN_CNTRL(0), &chan_ctrl0);

	do {
		if (!init_param.dco) {
			axi_adc_read(&core, AXI_ADC_REG_CNTRL, &reg_cntrl);

			if (inv_range)
				reg_cntrl |= AXI_ADC_DDR_EDGESEL;
			else
				reg_cntrl &= ~AXI_ADC_DDR_EDGESEL;
			axi_adc_write(&core, AXI_ADC_REG_CNTRL, reg_cntrl);
		}

		ad9265_testmode_set(dev, TESTMODE_PN9_SEQ);
		axi_adc_write(&core, AXI_ADC_REG_CHAN_CNTRL(0), AXI_ADC_ENABLE);
		axi_adc_set_pnsel(&core, 0, AXI_ADC_PN9);
		axi_adc_write(&core, AXI_ADC_REG_CHAN_STATUS(0), ~0);

		for (val = 0; (uint32_t)val <= max_val; val++) {
			if (init_param.dco) {
				ad9265_spi_write(dev, AD9265_REG_OUTPUT_DELAY,
						 val > 0 ? ((val - 1) |
							    init_param.
							    dco_en) : 0);
				ad9265_spi_write(dev, AD9265_REG_TRANSFER,
						 TRANSFER_SYNC);
			} else {
				for (lane = 0;
				     lane < init_param.nb_lanes;
				     lane++) {
					axi_adc_idelay_set(&core, lane, val);
				}
			}

			axi_adc_write(&core, AXI_ADC_REG_CHAN_STATUS(0), ~0);

			no_os_mdelay(1);

			axi_adc_read(&core, AXI_ADC_REG_CHAN_STATUS(0), &stat);

			err_field[val + (inv_range * (max_val + 1))] =
				! !(stat & (AXI_ADC_PN_ERR |
					    AXI_ADC_PN_OOS));
		}

		for (val = 0, cnt = 0, max_cnt = 0, start = -1, max_start = 0;
		     (uint32_t)val <= (max_val + (inv_range * (max_val + 1)));
		     val++) {
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

		if ((inv_range == 0) && ((max_cnt < 3) ||
					 (err_field[max_val] == 0))) {
			do_inv = 1;
			inv_range = 1;
		} else {
			do_inv = 0;
		}

	} while (do_inv);

	val = max_start + (max_cnt / 2);

#ifdef DCO_DEBUG
	for (cnt = 0;
	     (uint32_t)cnt <= (max_val + (inv_range * (max_val + 1)));
	     cnt++) {
		if (cnt == val)
			printf("|");
		else
			printf("%c", err_field[cnt] ? '-' : 'o');
		if ((uint32_t)cnt == max_val)
			printf("\n");
	}
#endif
	if ((uint32_t)val > max_val) {
		val -= max_val + 1;
		if (!init_param.dco) {
			axi_adc_read(&core, AXI_ADC_REG_CNTRL, &reg_cntrl);
			reg_cntrl |= AXI_ADC_DDR_EDGESEL;
			axi_adc_write(&core, AXI_ADC_REG_CNTRL, reg_cntrl);
		}
		cnt = 1;
	} else {
		if (init_param.dco) {
			ad9265_spi_write(dev, AD9265_REG_OUTPUT_PHASE,
					 OUTPUT_EVEN_ODD_MODE_EN);
		} else {
			axi_adc_read(&core, AXI_ADC_REG_CNTRL, &reg_cntrl);
			reg_cntrl &= ~AXI_ADC_DDR_EDGESEL;
			axi_adc_write(&core, AXI_ADC_REG_CNTRL, reg_cntrl);
		}
		cnt = 0;
	}

#ifdef DCO_DEBUG
	if (init_param.dco)
		printf(" %s DCO 0x%X\n", cnt ? "INVERT" : "",
		       val > 0 ?
		       (uint16_t)((val - 1) | init_param.dco_en) : 0);
	else
		printf(" %s IDELAY 0x%x\n", cnt ? "INVERT" : "",
		       (uint16_t)val);
#endif

	ad9265_testmode_set(dev, TESTMODE_OFF);
	if (init_param.dco) {
		ad9265_spi_write(dev, AD9265_REG_OUTPUT_DELAY,
				 val > 0 ?
				 ((val - 1) | init_param.dco_en) : 0);
		ad9265_spi_write(dev, AD9265_REG_TRANSFER, TRANSFER_SYNC);
	} else {
		for (lane = 0; lane < init_param.nb_lanes; lane++) {
			axi_adc_idelay_set(&core, lane, val);
		}
	}

	axi_adc_write(&core, AXI_ADC_REG_CHAN_CNTRL(0), chan_ctrl0);

	ret = ad9265_outputmode_set(dev, init_param.output_mode);
	if (ret < 0)
		return ret;

	return 0;
}

/***************************************************************************//**
* @brief ad9265_setup
*******************************************************************************/
int32_t ad9265_setup(struct ad9265_dev **device,
		     struct ad9265_init_param init_param,
		     struct axi_adc core)
{
	uint8_t chip_id;
	int32_t ret;
	struct ad9265_dev *dev;

	dev = (struct ad9265_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	ad9265_spi_read(dev, AD9265_REG_CHIP_ID, &chip_id);
	if(chip_id != AD9265_CHIP_ID) {
		printf("Error: Invalid CHIP ID (0x%x).\n", chip_id);
		return -1;
	}

	init_param.output_mode = AD9265_DEF_OUTPUT_MODE |
				 OUTPUT_MODE_TWOS_COMPLEMENT;
	ad9265_outputmode_set(dev, init_param.output_mode);

	init_param.dco = 1;
	init_param.dco_en = 0;
	init_param.nb_lanes = 6;
	ad9265_calibrate(dev, init_param, core);

	*device = dev;

	printf("AD9265 successfully initialized.\n");

	return ret;
}

/***************************************************************************//**
* @brief ad9265_remove
*******************************************************************************/
int32_t ad9265_remove(struct ad9265_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	no_os_free(dev);

	return ret;
}
