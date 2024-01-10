/***************************************************************************//**
 *   @file   adf5355.h
 *   @brief  Header file for adf5355 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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

/* Register Definition */
#define ADF5355_REG(x)                          (x)

/* REG0 Bit Definitions */
#define ADF5355_REG0_INT(x)			            (((x) & 0xFFFF) << 4)
#define ADF5355_REG0_PRESCALER(x)		        ((x) << 20)
#define ADF5355_REG0_AUTOCAL(x)			        ((x) << 21)

/* REG1 Bit Definitions */
#define ADF5355_REG1_FRACT(x)			        (((x) & 0xFFFFFF) << 4)

/* REG2 Bit Definitions */
#define ADF5355_REG2_MOD2(x)			        (((x) & 0x3FFF) << 4)
#define ADF5355_REG2_FRAC2(x)			        (((x) & 0x3FFF) << 18)

/* REG3 Bit Definitions */
#define ADF5355_REG3_PHASE(x)			        (((x) & 0xFFFFFF) << 4)
#define ADF5355_REG3_PHASE_ADJUST(x)		    ((x) << 28)
#define ADF5355_REG3_PHASE_RESYNC(x)		    ((x) << 29)
#define ADF5355_REG3_EXACT_SDLOAD_RESET(x)	    ((x) << 30)

/* REG4 Bit Definitions */
#define ADF5355_REG4_COUNTER_RESET_EN(x)	    ((x) << 4)
#define ADF5355_REG4_CP_THREESTATE_EN(x)	    ((x) << 5)
#define ADF5355_REG4_POWER_DOWN_EN(x)		    ((x) << 6)
#define ADF5355_REG4_PD_POLARITY_POS(x)		    ((x) << 7)
#define ADF5355_REG4_MUX_LOGIC(x)		        ((x) << 8)
#define ADF5355_REG4_REFIN_MODE_DIFF(x)		    ((x) << 9)
#define ADF5355_REG4_CHARGE_PUMP_CURR(x)		(((x) & 0xF) << 10)
#define ADF5355_REG4_DOUBLE_BUFF_EN(x)		    ((x) << 14)
#define ADF5355_REG4_10BIT_R_CNT(x)		        (((x) & 0x3FF) << 15)
#define ADF5355_REG4_RDIV2_EN(x)		        ((x) << 25)
#define ADF5355_REG4_RMULT2_EN(x)		        ((x) << 26)
#define ADF5355_REG4_MUXOUT(x)			        (((x) & 0x7) << 27)

/* REG5 Bit Definitions */
#define ADF5355_REG5_DEFAULT			        0x00800025

/* REG6 Bit Definitions */
#define ADF4355_REG6_OUTPUTB_PWR(x)		        (((x) & 0x3) << 7)
#define ADF4355_REG6_RF_OUTB_EN(x)		        ((x) << 9)
#define ADF4356_REG6_RF_OUTB_SEL(x)                ((x) << 25)
#define ADF5355_REG6_OUTPUT_PWR(x)		        (((x) & 0x3) << 4)
#define ADF5355_REG6_RF_OUT_EN(x)		        ((x) << 6)
#define ADF5355_REG6_RF_OUTB_EN(x)		        ((x) << 10)
#define ADF5355_REG6_MUTE_TILL_LOCK_EN(x)	    ((x) << 11)
#define ADF5355_REG6_CP_BLEED_CURR(x)		    (((x) & 0xFF) << 13)
#define ADF5355_REG6_RF_DIV_SEL(x)		        (((x) & 0x7) << 21)
#define ADF5355_REG6_FEEDBACK_FUND(x)		    ((x) << 24)
#define ADF5355_REG6_NEG_BLEED_EN(x)		    ((x) << 29)
#define ADF5355_REG6_GATED_BLEED_EN(x)		    ((x) << 30)
#define ADF5356_REG6_BLEED_POLARITY(x)		    ((x) << 31)
#define ADF5355_REG6_DEFAULT			        0x14000006

/* REG7 Bit Definitions */
#define ADF5355_REG7_LD_MODE_INT_N_EN(x)		((x) << 4)
#define ADF5355_REG7_FACT_N_LD_PRECISION(x)	    (((x) & 0x3) << 5)
#define ADF5355_REG7_LOL_MODE_EN(x)		        ((x) << 7)
#define ADF5355_REG7_LD_CYCLE_CNT(x)		    (((x) & 0x3) << 8)
#define ADF5355_REG7_LE_SYNCED_REFIN_EN(x)	    ((x) << 25)
#define ADF5356_REG7_LE_SYNCE_EDGE_RISING_EN(x)	((x) << 27)
#define ADF5355_REG7_DEFAULT			        0x10000007
#define ADF5356_REG7_DEFAULT			        0x04000007

/* REG8 Bit Definitions */
#define ADF5355_REG8_DEFAULT			        0x102D0428
#define ADF5356_REG8_DEFAULT			        0x15596568

/* REG9 Bit Definitions */
#define ADF5355_REG9_SYNTH_LOCK_TIMEOUT(x)	    (((x) & 0x1F) << 4)
#define ADF5355_REG9_ALC_TIMEOUT(x)		        (((x) & 0x1F) << 9)
#define ADF5355_REG9_TIMEOUT(x)			        (((x) & 0x3FF) << 14)
#define ADF5355_REG9_VCO_BAND_DIV(x)		    (((x) & 0xFF) << 24)

/* REG10 Bit Definitions */
#define ADF5355_REG10_ADC_EN(x)			        ((x) << 4)
#define ADF5355_REG10_ADC_CONV_EN(x)		    ((x) << 5)
#define ADF5355_REG10_ADC_CLK_DIV(x)		    (((x) & 0xFF) << 6)
#define ADF5355_REG10_DEFAULT			        0x00C0000A

/* REG11 Bit Definitions */
#define ADF5356_REG11_VCO_BAND_HOLD_EN(x)	    ((x) << 24)
#define ADF5355_REG11_DEFAULT			        0x0061300B
#define ADF5356_REG11_DEFAULT			        0x0061200B

/* REG12 Bit Definitions */
#define ADF5355_REG12_PHASE_RESYNC_CLK_DIV(x)	(((x) & 0xFFFF) << 16)
#define ADF5355_REG12_DEFAULT			        0x0000041C
#define ADF5356_REG12_PHASE_RESYNC_CLK_DIV(x)	(((x) & 0xFFFFF) << 12)
#define ADF5356_REG12_DEFAULT			        0x000005FC

/* REG13 Bit Definitions (ADF5356) */
#define ADF5356_REG13_MOD2_MSB(x)		        (((x) & 0x3FFF) << 4)
#define ADF5356_REG13_FRAC2_MSB(x)		        (((x) & 0x3FFF) << 18)

/* Specifications */
#define ADF5355_MIN_VCO_FREQ		            3400000000ULL /* Hz */
#define ADF5355_MAX_VCO_FREQ		            6800000000ULL /* Hz */
#define ADF5355_MAX_OUT_FREQ		            ADF5355_MAX_VCO_FREQ /* Hz */
#define ADF5355_MIN_OUT_FREQ		            (ADF5355_MIN_VCO_FREQ / 64) /* Hz */
#define ADF5355_MAX_OUTB_FREQ		            (ADF5355_MAX_VCO_FREQ * 2) /* Hz */
#define ADF5355_MIN_OUTB_FREQ		            (ADF5355_MIN_VCO_FREQ * 2) /* Hz */

#define ADF4356_MIN_VCO_FREQ                        ADF5355_MIN_VCO_FREQ /* Hz */
#define ADF4356_MAX_VCO_FREQ                        ADF5355_MAX_VCO_FREQ /* Hz */

#define ADF4355_MIN_VCO_FREQ		            3400000000ULL /* Hz */
#define ADF4355_MAX_VCO_FREQ		            6800000000ULL /* Hz */
#define ADF4355_MAX_OUT_FREQ		            ADF4355_MAX_VCO_FREQ /* Hz */
#define ADF4355_MIN_OUT_FREQ		            (ADF4355_MIN_VCO_FREQ / 64) /* Hz */

#define ADF4355_3_MIN_VCO_FREQ		            3300000000ULL /* Hz */
#define ADF4355_3_MAX_VCO_FREQ		            6600000000ULL /* Hz */
#define ADF4355_3_MAX_OUT_FREQ		            ADF4355_3_MAX_VCO_FREQ /* Hz */
#define ADF4355_3_MIN_OUT_FREQ		            (ADF4355_3_MIN_VCO_FREQ / 64) /* Hz */

#define ADF4355_2_MIN_VCO_FREQ		            3400000000ULL /* Hz */
#define ADF4355_2_MAX_VCO_FREQ		            6800000000ULL /* Hz */
#define ADF4355_2_MAX_OUT_FREQ		            4400000000ULL /* Hz */
#define ADF4355_2_MIN_OUT_FREQ		            (ADF4355_2_MIN_VCO_FREQ / 64) /* Hz */

#define ADF5355_MAX_FREQ_PFD		            75000000UL /* Hz */
#define ADF5355_MAX_FREQ_REFIN		            600000000UL /* Hz */
#define ADF5355_MAX_MODULUS2		            16384
#define ADF5356_MAX_MODULUS2		            268435456
#define ADF5355_MAX_R_CNT		                1023

#define ADF5355_MODULUS1			            16777216ULL
#define ADF5355_MIN_INT_PRESCALER_89	        75

#define ADF5355_REG_NUM                         14

#define ADF5355_SPI_NO_BYTES                    4

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_spi.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum adf5355_device_id
 * @brief Devices supported by the drivers.
 */
enum adf5355_device_id {
	ADF5355,
	ADF4355,
	ADF4355_2,
	ADF4355_3,
	ADF4356,
	ADF5356,
};

/**
 * @enum adf5355_mux_out_sel
 * @brief Muxout selection.
 */
enum adf5355_mux_out_sel {
	ADF5355_MUXOUT_THREESTATE,
	ADF5355_MUXOUT_DVDD,
	ADF5355_MUXOUT_GND,
	ADF5355_MUXOUT_R_DIV_OUT,
	ADF5355_MUXOUT_N_DIV_OUT,
	ADF5355_MUXOUT_ANALOG_LOCK_DETECT,
	ADF5355_MUXOUT_DIGITAL_LOCK_DETECT,
};

/**
 * @struct adf5355_dev
 * @brief  Device descriptor.
 */
struct adf5355_dev {
	struct no_os_spi_desc	*spi_desc;
	enum adf5355_device_id      dev_id;
	bool                        all_synced;
	uint32_t                    regs[ADF5355_REG_NUM];
	uint64_t                    freq_req;
	uint8_t                     freq_req_chan;
	uint8_t                     num_channels;
	uint32_t	                clkin_freq;
	uint64_t                    max_out_freq;
	uint64_t                    min_out_freq;
	uint64_t                    min_vco_freq;
	uint32_t	                fpfd;
	uint32_t	                integer;
	uint32_t	                fract1;
	uint32_t	                fract2;
	uint32_t	                mod2;
	uint32_t                    cp_ua;
	bool                        cp_neg_bleed_en;
	bool                        cp_gated_bleed_en;
	bool                        cp_bleed_current_polarity_en;
	bool		                mute_till_lock_en;
	bool                        outa_en;
	bool                        outb_en;
	uint8_t                     outa_power;
	uint8_t                     outb_power;
	uint8_t                     phase_detector_polarity_neg;
	bool                        ref_diff_en;
	bool                        mux_out_3v3_en;
	bool                        outb_sel_fund;
	uint8_t		                ref_doubler_en;
	uint8_t		                ref_div2_en;
	uint8_t                     rf_div_sel;
	uint16_t                    ref_div_factor;
	enum adf5355_mux_out_sel    mux_out_sel;
	uint32_t                    delay_us;
};

/**
 * @struct ad5355_init_param
 * @brief  Structure containing the initialization parameters.
 */
struct adf5355_init_param {
	struct no_os_spi_init_param	*spi_init;
	enum adf5355_device_id      dev_id;
	uint64_t                    freq_req;
	uint8_t                     freq_req_chan;
	uint32_t	                clkin_freq;
	uint32_t                    cp_ua;
	bool                        cp_neg_bleed_en;
	bool                        cp_gated_bleed_en;
	bool                        cp_bleed_current_polarity_en;
	bool		                mute_till_lock_en;
	bool                        outa_en;
	bool                        outb_en;
	uint8_t                     outa_power;
	uint8_t                     outb_power;
	bool                        phase_detector_polarity_neg;
	bool                        ref_diff_en;
	bool                        mux_out_3v3_en;
	uint8_t		                ref_doubler_en;
	uint8_t		                ref_div2_en;
	enum adf5355_mux_out_sel    mux_out_sel;
	bool                        outb_sel_fund;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Recalculate rate corresponding to a channel. */
int32_t adf5355_clk_recalc_rate(struct adf5355_dev *dev, uint32_t chan,
				uint64_t *rate);

/* Set channel rate. */
int32_t adf5355_clk_set_rate(struct adf5355_dev *dev, uint32_t chan,
			     uint64_t rate);

/* Calculate closest possible rate */
int32_t adf5355_clk_round_rate(struct adf5355_dev *dev, uint64_t rate,
			       uint64_t *rounded_rate);

/* Initializes the ADF5355. */
int32_t adf5355_init(struct adf5355_dev **device,
		     const struct adf5355_init_param *init_param);

/* Remove the device. */
int32_t adf5355_remove(struct adf5355_dev *device);
