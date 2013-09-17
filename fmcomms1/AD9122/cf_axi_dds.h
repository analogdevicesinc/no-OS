/***************************************************************************//**
 *   @file   cf_axi_dds.h
 *   @brief  Header file of AD9122 Driver helper functions.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

#ifndef __ADI_AXI_DDS_H__
#define __ADI_AXI_DDS_H__

/* This define controls the use of the DDS module by the AD9122 driver */
#define CF_AXI_DDS

#ifdef CUSTOM_DAC_PCORE
#undef CF_AXI_DDS
#endif

/******************************************************************************/
/************************ Data Types ******************************************/
/******************************************************************************/
enum
{
	IIO_CHAN_INFO_SCALE = 1,
	IIO_CHAN_INFO_FREQUENCY,
	IIO_CHAN_INFO_PHASE,
	IIO_CHAN_INFO_SAMP_FREQ
};

enum
{
	CLK_DATA,
	CLK_DAC,
	CLK_REF,
	CLK_NUM,
};

#ifdef CF_AXI_DDS
	struct cf_axi_dds_state
	{
		uint32_t	dac_clk;
	};
#endif //CF_AXI_DDS

struct cf_axi_converter
{
	int32_t* 	spi;
	uint32_t 	clk[CLK_NUM];
	uint32_t	id;
	uint32_t	interp_factor;
	uint32_t	fcenter_shift;
	uint32_t 	intp_modes[5];
	uint32_t 	cs_modes[17];
	int32_t		(*read)(uint8_t reg);
	int32_t		(*write)(uint8_t reg, uint8_t val);
	int32_t		(*setup)(struct cf_axi_converter *conv);
	int32_t		(*get_fifo_status)(struct cf_axi_converter *conv);
	uint32_t 	(*get_data_clk)(struct cf_axi_converter *conv);
	int32_t 	(*read_raw)(uint32_t channel,
							int32_t *val,
							int32_t *val2,
							int32_t mask);
	int32_t 	(*write_raw)(uint32_t channel,
			 	 	 	 	 int32_t val,
			 	 	 	 	 int32_t val2,
			 	 	 	 	 int32_t mask);
	void		(*pcore_set_sed_pattern)(unsigned chan,
										 unsigned pat1,
										 unsigned pat2);
	void		(*pcore_sync)();
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t  cf_axi_dds_of_probe();
#ifdef CF_AXI_DDS
int32_t  cf_axi_dds_write_raw(uint32_t channel,
						uint32_t address,
						int32_t val,
						int32_t val2,
						int32_t mask);
#endif //CF_AXI_DDS

#endif //__ADI_AXI_DDS_H__
