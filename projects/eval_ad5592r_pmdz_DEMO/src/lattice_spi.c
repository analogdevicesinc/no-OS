/*******************************************************************************
 *   @file   lattice/lattice_spi.c
 *   @brief  Implementation of Lattice SPI Generic Driver.
 *   @author Villyam (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

#include "lattice_spi.h"
#include "spi_master.h"

#include "no_os_error.h"
#include "no_os_spi.h"
#include "no_os_alloc.h"

#define DEBUG_LATTICE_SPI

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, -1 otherwise.
 */

static int32_t spi_init_pl(struct no_os_spi_desc *desc,
			   const struct no_os_spi_init_param *param)
{
#ifdef SPI_MASTER_H

	int32_t					ret;
	struct spim_instance 	*l_spi_inst;
	struct latt_spi_init_param *l_init_par;
	struct latt_spi_desc 	*l_spi_desc;
	const uint16_t			prescaler_default = 64;
	const uint16_t			prescaler_min = 16;
	const uint16_t			prescaler_max = 1024;

	uint32_t				base_addr = 0;
	uint8_t					ssnp = 0;
	uint8_t					spi_clk_polarity = 0;
	uint8_t					spi_clk_phase = 0;
	uint8_t					spi_enable_mode = 0;
	const uint8_t			interrupts_en = 0;
	const uint8_t	 		data_width = 8;
	uint8_t	 				slave_count = 1;

	uint16_t				clk_prescaler = prescaler_default;
	uint32_t				input_clock = 0;

	l_init_par = (struct latt_spi_init_param *) param->extra;
	if(!l_init_par) {
		printf("error no l_init_par\n\r");
		return -1;
	}

	slave_count = l_init_par->slave_count;
	input_clock = l_init_par->input_clock;
	spi_enable_mode = l_init_par->spi_enable_mode;

	if(!l_init_par->base_addr){
		base_addr = param->device_id;
	}
	else{
		base_addr = l_init_par->base_addr;
	}

	clk_prescaler = input_clock/(desc->max_speed_hz << 1);

	if(clk_prescaler > prescaler_max){
		clk_prescaler = prescaler_max;
		printf("WARNING clk_prescaler > prescaler_max\n\r");
		printf("WARNING desc->max_speed_hz will be set to: %u\n\r",input_clock/(clk_prescaler<<1));
	}
	if(clk_prescaler < prescaler_min){
		clk_prescaler = prescaler_min;
		printf("WARNING clk_prescaler < prescaler_min\n\r");
		printf("WARNING desc->max_speed_hz will be set to: %u\n\r",input_clock/(clk_prescaler<<1));
	}

	l_spi_inst = (struct spim_instance*) no_os_malloc(sizeof(struct spim_instance));
	if(!l_spi_inst) {
#ifdef DEBUG_LATTICE_SPI
		printf("error l_spi_inst malloc\n\r");
#endif
		return -1;
	}

	l_spi_desc = (struct latt_spi_desc*) no_os_malloc(sizeof(struct latt_spi_desc));
	if(!l_spi_desc) {
#ifdef DEBUG_LATTICE_SPI
		printf("error l_spi_desc malloc\n\r");
#endif
		return -1;
	}

	l_spi_desc->type = l_init_par->type;
	l_spi_desc->instance = l_spi_inst;
	desc->extra = l_spi_desc;

	switch(desc->mode){
		case NO_OS_SPI_MODE_0:
			spi_clk_polarity = 0;
			spi_clk_phase = 0;
			break;
		case NO_OS_SPI_MODE_1:
			spi_clk_polarity = 0;
			spi_clk_phase = 1;
			break;
		case NO_OS_SPI_MODE_2:
			spi_clk_polarity = 1;
			spi_clk_phase = 0;
			break;
		case NO_OS_SPI_MODE_3:
			spi_clk_polarity = 1;
			spi_clk_phase = 1;
			break;
		default:
			spi_clk_polarity = 0;
			spi_clk_phase = 0;
	}

	ret = spi_master_init(l_spi_inst ,
			base_addr,
			slave_count,
			data_width,
			spi_clk_polarity,
			spi_clk_phase,
			interrupts_en,
			spi_enable_mode);

	if(ret != 0){
		goto ps_error;
#ifdef DEBUG_LATTICE_SPI
		printf("error spi_master_init\n\r");
#endif
	}

	ret = spi_master_config(l_spi_inst,
			data_width,
			clk_prescaler,
			spi_clk_polarity,
			spi_clk_phase,
			ssnp,
			spi_enable_mode);

	if(ret != 0){
		goto ps_error;
#ifdef DEBUG_LATTICE_SPI
		printf("error spi_master_config\n\r");
#endif
	}

	return 0;

ps_error:
	no_os_free(l_spi_inst);
	no_os_free(l_spi_desc);

#endif
	return -1;
}

int32_t latt_spi_init(struct no_os_spi_desc **desc,
		     const struct no_os_spi_init_param *param)
{
	int32_t				ret;
	enum latt_spi_type		*spi_type;

	if (!param) {
		return -1;
#ifdef DEBUG_LATTICE_SPI
		printf("error no param\n\r");
#endif
	}

	*desc = no_os_malloc(sizeof(**desc));
	if(! *desc) {
		goto init_error;
#ifdef DEBUG_LATTICE_SPI
		printf("error malloc\n\r");
#endif
	}

	spi_type = param->extra;

	if(!spi_type){
#ifdef DEBUG_LATTICE_SPI
		printf("error no spi_type\n\r");
#endif
		goto init_error;
	}

	(*desc)->max_speed_hz = param->max_speed_hz;
	(*desc)->mode = param->mode;
	(*desc)->bit_order = param->bit_order;
	(*desc)->chip_select = param->chip_select;

	switch (*spi_type) {
	case SPI_PL:
		ret = spi_init_pl(*desc, param);
		if (ret != 0) {
#ifdef DEBUG_LATTICE_SPI
			printf("error spi_init_pl\n\r");
#endif
		}
		break;
	default:
		goto init_error;
		break;
	}

	if (ret != 0) {
init_error:
		no_os_free(*desc);
		return -1;
	}

	return ret;
}

int32_t latt_spi_remove(struct no_os_spi_desc *desc){

	struct latt_spi_desc 	*ldesc = NULL;

	ldesc = desc->extra;

	if(!ldesc)
		return -1;

	switch (ldesc->type) {
	case SPI_PL:
		//to do if it will be the case
		break;
	case SPI_PS:
		//to do if it will be the case
		break;
	case SPI_ENGINE:
		//to do if it will be the case
		break;
	default:
		return -1;
	}

	if(ldesc)
		no_os_free(ldesc->instance);
	no_os_free(ldesc);
	no_os_free(desc);

	return 0;
}

int32_t latt_spi_write_and_read(struct no_os_spi_desc *desc,
			       uint8_t *data,
			       uint16_t bytes_number){
	int32_t ret;

	struct latt_spi_desc *ldesc = desc->extra;

	uint8_t ss;

	if(desc->chip_select < 8){
		ss = 1 << desc->chip_select;
	}
	else{
		ss = 0x80;
	}

	ret = spi_device_select(ldesc->instance, ss);

	if(ret != 0){
		return -1;
#ifdef DEBUG_LATTICE_SPI
		printf("something error spi_device_select\n\r");
#endif
	}

	ret = spi_master_readwrite(ldesc->instance, data, bytes_number);

	if(ret != 0){
		return -1;
#ifdef DEBUG_LATTICE_SPI
		printf("error spi_master_readwrite\n\r");
#endif
	}

	return 0;
}

/**
 * @brief Lattice platform specific SPI platform ops structure
 */
const struct no_os_spi_platform_ops latt_spi_ops = {
	.init = &latt_spi_init,
	.write_and_read = &latt_spi_write_and_read,
	.remove = &latt_spi_remove
};


