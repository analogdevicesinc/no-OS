/***************************************************************************//**
 *   @file   axi_dac_core.h
 *   @brief  Driver for the Analog Devices AXI-DAC-CORE module.
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
#ifndef AXI_DAC_CORE_H_
#define AXI_DAC_CORE_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @struct axi_dac
 * @brief AXI DAC Device Descriptor.
 */
struct axi_dac {
	/** Device Name */
	const char *name;
	/** Base Address */
	uint32_t base;
	/** Number of channels */
	uint8_t	num_channels;
	/** AXI DAC Clock */
	uint64_t clock_hz;
	/** DAC channels manual configuration */
	struct axi_dac_channel *channels;
};

struct axi_dac_init {
	/** Device Name */
	const char *name;
	/** Base Address */
	uint32_t base;
	/** Number of channels */
	uint8_t	num_channels;
	/** DAC channels manual configuration */
	struct axi_dac_channel *channels;
	/** The effective DAC rate */
	uint8_t rate;
};

enum axi_dac_data_sel {
	AXI_DAC_DATA_SEL_DDS,
	AXI_DAC_DATA_SEL_SED,
	AXI_DAC_DATA_SEL_DMA,
	AXI_DAC_DATA_SEL_ZERO,
	AXI_DAC_DATA_SEL_PN7,
	AXI_DAC_DATA_SEL_PN15,
	AXI_DAC_DATA_SEL_PN23,
	AXI_DAC_DATA_SEL_PN31,
	AXI_DAC_DATA_SEL_LB,
	AXI_DAC_DATA_SEL_PNXX,
};

struct axi_dac_channel {
	uint32_t dds_frequency_0;       // in hz (1000*1000 for MHz)
	uint32_t dds_phase_0;           // in milli(?) angles (90*1000 for 90 degrees = pi/2)
	int32_t dds_scale_0;            // in micro units (1.0*1000*1000 is 1.0)
	uint32_t dds_frequency_1;       // in hz (1000*1000 for MHz)
	uint32_t dds_phase_1;           // in milli(?) angles (90*1000 for 90 degrees = pi/2)
	int32_t dds_scale_1;            // in micro units (1.0*1000*1000 is 1.0)
	uint32_t dds_dual_tone;         // if using single tone for this channel, set to 0x0
	uint32_t pat_data;              // if using SED/debug that sort of thing
	enum axi_dac_data_sel sel;      // set to one of the enumerated type above.
};

extern const uint16_t sine_lut[128];

extern const uint32_t sine_lut_iq[1024];

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/** Begin AXI DAC Initialization */
int32_t axi_dac_init_begin(struct axi_dac **dac_core,
			   const struct axi_dac_init *init);
/** Finish AXI DAC Initialization */
int32_t axi_dac_init_finish(struct axi_dac *dac);
/** AXI DAC Main Initialization */
int32_t axi_dac_init(struct axi_dac **dac_core,
		     const struct axi_dac_init *init);
/** Resources Deallocation */
int32_t axi_dac_remove(struct axi_dac *dac);
/** AXI DAC Data Select */
int32_t axi_dac_set_datasel(struct axi_dac *dac,
			    int32_t chan,
			    enum axi_dac_data_sel sel);
/** AXI DAC Set DDS Frequency */
int32_t axi_dac_dds_set_frequency(struct axi_dac *dac,
				  uint32_t chan, uint32_t freq_hz);
/** AXI DAC Get DDS Frequency */
int32_t axi_dac_dds_get_frequency(struct axi_dac *dac,
				  uint32_t chan, uint32_t *freq);
/** AXI DAC Set DDS Phase */
int32_t axi_dac_dds_set_phase(struct axi_dac *dac,
			      uint32_t chan, uint32_t phase);
/** AXI DAC Get DDS Phase */
int32_t axi_dac_dds_get_phase(struct axi_dac *dac,
			      uint32_t chan, uint32_t *phase);
/** AXI DAC Set DDS Scale */
int32_t axi_dac_dds_set_scale(struct axi_dac *dac,
			      uint32_t chan,
			      int32_t scale_micro_units);
/** AXI DAC Get DDS Phase */
int32_t axi_dac_dds_get_scale(struct axi_dac *dac,
			      uint32_t chan,
			      int32_t *scale_micro_units);
/** AXI DAC Set Buffer */
int32_t axi_dac_set_buff(struct axi_dac *dac,
			 uint32_t address,
			 uint16_t *buff,
			 uint32_t buff_size);
/** AXI DAC Set Sine Lookup Table */
uint32_t axi_dac_set_sine_lut(struct axi_dac *dac,
			      uint32_t address);
/** Get the scale calibration values for AXI DAC channel */
int32_t axi_dac_dds_get_calib_scale(struct axi_dac *dac,
				    uint32_t chan,
				    int32_t *val,
				    int32_t *val2);
/** Get the phase calibration values for AXI DAC channel */
int32_t axi_dac_dds_get_calib_phase(struct axi_dac *dac,
				    uint32_t chan,
				    int32_t *val,
				    int32_t *val2);
/** Calibrate scale for specific AXI DAC channel */
int32_t axi_dac_dds_set_calib_scale(struct axi_dac *dac,
				    uint32_t chan,
				    int32_t val,
				    int32_t val2);
/** Calibrate phase for specific AXI DAC channel */
int32_t axi_dac_dds_set_calib_phase(struct axi_dac *dac,
				    uint32_t chan,
				    int32_t val,
				    int32_t val2);
/** AXI DAC Load Custom Data */
int32_t axi_dac_load_custom_data(struct axi_dac *dac,
				 const uint32_t *custom_data_iq,
				 uint32_t custom_tx_count,
				 uint32_t address);
/** Setup the AXI DAC Data */
int32_t axi_dac_data_setup(struct axi_dac *dac);

#endif
