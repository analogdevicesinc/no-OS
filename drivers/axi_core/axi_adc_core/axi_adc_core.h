/***************************************************************************//**
 *   @file   axi_adc_core.h
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
#ifndef AXI_ADC_CORE_H_
#define AXI_ADC_CORE_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AXI_ADC_REG_RSTN		0x0040
#define AXI_ADC_MMCM_RSTN		NO_OS_BIT(1)
#define AXI_ADC_RSTN			NO_OS_BIT(0)

#define AXI_ADC_REG_CNTRL		0x0044
#define AXI_ADC_R1_MODE			NO_OS_BIT(2)
#define AXI_ADC_DDR_EDGESEL		NO_OS_BIT(1)
#define AXI_ADC_PIN_MODE		NO_OS_BIT(0)

#define AXI_ADC_REG_CNTRL_3		0x004C
#define AXI_ADC_CRC_EN			NO_OS_BIT(8)

#define AXI_ADC_REG_CLK_FREQ		0x0054
#define AXI_ADC_CLK_FREQ(x)		(((x) & 0xFFFFFFFF) << 0)
#define AXI_ADC_TO_CLK_FREQ(x)		(((x) >> 0) & 0xFFFFFFFF)

#define AXI_ADC_REG_CLK_RATIO		0x0058
#define AXI_ADC_CLK_RATIO(x)		(((x) & 0xFFFFFFFF) << 0)
#define AXI_ADC_TO_CLK_RATIO(x)		(((x) >> 0) & 0xFFFFFFFF)

#define AXI_ADC_REG_STATUS		0x005C
#define AXI_ADC_MUX_PN_ERR		NO_OS_BIT(3)
#define AXI_ADC_MUX_PN_OOS		NO_OS_BIT(2)
#define AXI_ADC_MUX_OVER_RANGE		NO_OS_BIT(1)
#define AXI_ADC_STATUS			NO_OS_BIT(0)

#define AXI_ADC_REG_DELAY_CNTRL	0x0060
#define ADC_DELAY_SEL			NO_OS_BIT(17)
#define ADC_DELAY_RWN			NO_OS_BIT(16)
#define ADC_DELAY_ADDRESS(x) 		(((x) & 0xFF) << 8)
#define ADC_TO_DELAY_ADDRESS(x) 	(((x) >> 8) & 0xFF)
#define ADC_DELAY_WDATA(x)		(((x) & 0x1F) << 0)
#define ADC_TO_DELAY_WDATA(x)		(((x) >> 0) & 0x1F)

#define AXI_ADC_REG_CHAN_CNTRL(c)	(0x0400 + (c) * 0x40)
#define AXI_ADC_PN_SEL			NO_OS_BIT(10)
#define AXI_ADC_IQCOR_ENB		NO_OS_BIT(9)
#define AXI_ADC_DCFILT_ENB		NO_OS_BIT(8)
#define AXI_ADC_FORMAT_SIGNEXT		NO_OS_BIT(6)
#define AXI_ADC_FORMAT_TYPE		NO_OS_BIT(5)
#define AXI_ADC_FORMAT_ENABLE		NO_OS_BIT(4)
#define AXI_ADC_PN23_TYPE		NO_OS_BIT(1)
#define AXI_ADC_ENABLE			NO_OS_BIT(0)

#define AXI_ADC_REG_CHAN_STATUS(c)	(0x0404 + (c) * 0x40)
#define AXI_ADC_PN_ERR			NO_OS_BIT(2)
#define AXI_ADC_PN_OOS			NO_OS_BIT(1)
#define AXI_ADC_OVER_RANGE		NO_OS_BIT(0)

#define AXI_ADC_REG_CHAN_CNTRL_1(c)	(0x0410 + (c) * 0x40)
#define AXI_ADC_DCFILT_OFFSET(x)	(((x) & 0xFFFFL) << 16)
#define AXI_ADC_TO_DCFILT_OFFSET(x)	(((x) >> 16) & 0xFFFF)
#define AXI_ADC_DCFILT_COEFF(x)		(((x) & 0xFFFF) << 0)
#define AXI_ADC_TO_DCFILT_COEFF(x)	(((x) >> 0) & 0xFFFF)

#define AXI_ADC_REG_CHAN_CNTRL_2(c)	(0x0414 + (c) * 0x40)
#define AXI_ADC_IQCOR_COEFF_1(x)	(((x) & 0xFFFFL) << 16)
#define AXI_ADC_TO_IQCOR_COEFF_1(x)	(((x) >> 16) & 0xFFFF)
#define AXI_ADC_IQCOR_COEFF_2(x)	(((x) & 0xFFFF) << 0)
#define AXI_ADC_TO_IQCOR_COEFF_2(x)	(((x) >> 0) & 0xFFFF)

#define AXI_ADC_REG_CHAN_CNTRL_3(c)	(0x0418 + (c) * 0x40)
#define AXI_ADC_ADC_PN_SEL(x)		(((x) & 0xF) << 16)
#define AXI_ADC_TO_ADC_PN_SEL(x)	(((x) >> 16) & 0xF)
#define AXI_ADC_ADC_DATA_SEL(x)		(((x) & 0xF) << 0)
#define AXI_ADC_TO_ADC_DATA_SEL(x)	(((x) >> 0) & 0xF)

#define AXI_ADC_REG_DELAY(l)		(0x0800 + (l) * 0x4)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct axi_adc
 * @brief AXI ADC Device Descriptor.
 */
struct axi_adc {
	/** Device Name */
	const char *name;
	/** Base Address */
	uint32_t base;
	/** Slave Base Address */
	uint32_t slave_base;
	/** Number of channels */
	uint8_t	num_channels;
	/** Number of slave channels */
	uint8_t	num_slave_channels;
	/** AXI ADC Clock */
	uint64_t clock_hz;
	/** AXI ADC Channel Mask*/
	uint32_t mask;
};

/**
 * @struct axi_adc_init
 * @brief AXI ADC Initialization Parameters structure.
 */
struct axi_adc_init {
	/** Device Name */
	const char *name;
	/** Base Address */
	uint32_t base;
	/** Number of channels */
	uint8_t	num_channels;
	/** Slave Base Address */
	uint32_t slave_base;
	/** Number of slave channels */
	uint8_t	num_slave_channels;
};

enum axi_adc_pn_sel {
	AXI_ADC_PN9 = 0,
	AXI_ADC_PN23A = 1,
	AXI_ADC_PN7 = 4,
	AXI_ADC_PN15 = 5,
	AXI_ADC_PN23 = 6,
	AXI_ADC_PN31 = 7,
	AXI_ADC_PN_CUSTOM = 9,
	AXI_ADC_PN_RAMP_NIBBLE = 10,
	AXI_ADC_PN_RAMP_16 = 11,
	AXI_ADC_PN_END = 12,
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/** Begin AXI ADC Initialization */
int32_t axi_adc_init_begin(struct axi_adc **adc_core,
			   const struct axi_adc_init *init);
/** Finish AXI ADC Initialization */
int32_t axi_adc_init_finish(struct axi_adc *adc);
/** AXI ADC Main Initialization */
int32_t axi_adc_init(struct axi_adc **adc_core,
		     const struct axi_adc_init *init);
/** Resources Deallocation */
int32_t axi_adc_remove(struct axi_adc *adc);
/** AXI ADC Read */
int32_t axi_adc_read(struct axi_adc *adc,
		     uint32_t reg_addr,
		     uint32_t *reg_data);
/** AXI ADC Write */
int32_t axi_adc_write(struct axi_adc *adc,
		      uint32_t reg_addr,
		      uint32_t reg_data);
/** Set AXI ADC PN sequence */
int32_t axi_adc_set_pnsel(struct axi_adc *adc,
			  uint32_t chan,
			  enum axi_adc_pn_sel sel);
/** Monitor the AXI ADC PN Sequence */
int32_t axi_adc_pn_mon(struct axi_adc *adc,
		       enum axi_adc_pn_sel sel,
		       uint32_t delay_ms);
/** Get the AXI ADC Sampling Frequency */
int32_t axi_adc_get_sampling_freq(struct axi_adc *adc,
				  uint32_t chan,
				  uint64_t *sampling_freq);
/** Set input/output delay primitive for specific interface line */
void axi_adc_idelay_set(struct axi_adc *adc,
			uint32_t lane,
			uint32_t val);
/** Set input/output delay primitive for number of lines */
int32_t axi_adc_delay_set(struct axi_adc *adc,
			  uint32_t no_of_lanes,
			  uint32_t delay);
/** Calibrate Delay using specific PN sequence */
int32_t axi_adc_delay_calibrate(struct axi_adc *core,
				uint32_t no_of_lanes,
				enum axi_adc_pn_sel sel);
/** Calibrate phase for specific AXI ADC channel */
int32_t axi_adc_set_calib_phase(struct axi_adc *adc,
				uint32_t chan,
				int32_t val,
				int32_t val2);
/** Get the phase calibration values for AXI ADC channel */
int32_t axi_adc_get_calib_phase(struct axi_adc *adc,
				uint32_t chan,
				int32_t *val,
				int32_t *val2);
/** Calibrate scale for specific AXI ADC channel */
int32_t axi_adc_set_calib_scale(struct axi_adc *adc,
				uint32_t chan,
				int32_t val,
				int32_t val2);
/** Get the scale calibration values for AXI ADC channel */
int32_t axi_adc_get_calib_scale(struct axi_adc *adc,
				uint32_t chan,
				int32_t *val,
				int32_t *val2);
/** Calibrate bias for specific AXI ADC channel */
int32_t axi_adc_set_calib_bias(struct axi_adc *adc,
			       uint32_t chan,
			       int32_t val,
			       int32_t val2);
/** Get the bias calibration values for AXI ADC channel */
int32_t axi_adc_get_calib_bias(struct axi_adc *adc,
			       uint32_t chan,
			       int32_t *val,
			       int32_t *val2);
/** Update active AXI ADC channels */
int32_t axi_adc_update_active_channels(struct axi_adc *adc, uint32_t mask);
#endif
