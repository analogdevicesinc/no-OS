/***************************************************************************//**
 *   @file   adf5611.h
 *   @brief  Implementation of adf5611 Driver.
 *   @author Jude Osemene (jude.osemene@analog.com)
********************************************************************************
* Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_spi.h"

/* ADF5611 REG0000 Map */
#define ADF5611_SOFT_REST_R_MSK             NO_OS_BIT(7)
#define ADF5611_LSB_FIRST_R_MSK             NO_OS_BIT(6)
#define ADF5611_ADDRESS_ASC_R_MSK           NO_OS_BIT(5)
#define ADF5611_SDO_ACTIVE_R_MSK            NO_OS_BIT(4)
#define ADF5611_SDO_ACTIVE_MSK              NO_OS_BIT(3)
#define ADF5611_ADDRESS_ASC_MSK             NO_OS_BIT(2)
#define ADF5611_LSB_FIRST_MSK               NO_OS_BIT(1)
#define ADF5611_SOFT_RESET_MSK              NO_OS_BIT(0)
#define ADF5611_RESET_CMD		    0x81

/* ADF5611 REG0000 NO_OS_BIT DEFINITION */
#define ADF5611_SDO_ACTIVE_SPI_3WIRE        0x0
#define ADF5611_SDO_ACTIVE_SPI_4WIRE        0x1

#define ADF5611_ADDRESS_ASC_AUTO_DECR       0x0
#define ADF5611_ADDRESS_ASC_AUTO_INCR       0x1

#define ADF5611_LSB_FIRST_MSB		    0x0
#define ADF5611_LSB_FIRST_LSB		    0x1

#define ADF5611_SOFT_RESET_NORMAL_OP	    0x0
#define ADF5611_SOFT_RESET_EN		    0x1

/* ADF5611 REG0001 MAP */
#define ADF5611_SINGLE_INSTR_MSK            NO_OS_BIT(7)
#define ADF5611_MASTER_READBACK_CTRL_MSK    NO_OS_BIT(5)

/* ADF5611 REG0001 NO_OS_BIT DEFINITION */
#define ADF5611_SPI_STREAM_EN		    0x0
#define ADF5611_SPI_STREAM_DIS		    0x1

#define ADF5611_RB_SLAVE_REG		    0x0
#define ADF5611_RB_MASTER_REG		    0x1

/* ADF5611 REG0003 NO_OS_BIT DEFINITION */
#define ADF5611_CHIP_TYPE		    0x06

/* ADF5611 REG0004 NO_OS_BIT DEFINITION */
#define ADF5611_PRODUCT_ID_LSB		    0x0005

/* ADF5611 REG0005 NO_OS_BIT DEFINITION */
#define ADF5611_PRODUCT_ID_MSB		    0x0005

/* ADF5611 REG000A Map */
#define ADF5611_SCRATCHPAD_MSK		    NO_OS_GENMASK(7, 0)

/* ADF5611 REG000C NO_OS_BIT DEFINITION */
#define ADF5611_VENDOR_ID_LSB		    0x56

/* ADF5611 REG000D NO_OS_BIT DEFINITION */
#define ADF5611_VENDOR_ID_MSB		    0x04

/* ADF5611 REG0010 MAP*/
#define ADF5611_N_INT_LSB_MSK		    NO_OS_GENMASK(7, 0)

/* ADF5611 REG0011 MAP*/
#define ADF5611_N_INT_MID_MSK		    NO_OS_GENMASK(7, 0)

/* ADF5611 REG0012 MAP */
#define ADF5611_FRAC1WORD_LSB_MSK	    NO_OS_GENMASK(7, 4)
#define ADF5611_N_INT_MSB_MSK		    NO_OS_GENMASK(3, 0)

/* ADF5611 REG0013 MAP */
#define ADF5611_FRAC1WORD_MID_MSK	    NO_OS_GENMASK(7, 0)

/* ADF5611 REG0014 MAP */
#define ADF5611_FRAC1WORD_MID_MSB_MSK	    NO_OS_GENMASK(7, 0)

/* ADF5611 REG0015 MAP */
#define ADF5611_M_VCO_BIAS_MSK              NO_OS_GENMASK(7, 5)
#define ADF5611_FRAC1WORD_MSB_MSK           NO_OS_GENMASK(4, 0)

/* ADF5611 REG0016 MAP */
#define ADF5611_M_VCO_BAND_MSK              NO_OS_GENMASK(7, 1)
#define ADF5611_M_VCO_CORE_MSK              NO_OS_BIT(0)

/* ADF5611 REG0016 NO_OS_BIT DEFINITION */
#define ADF5611_VCO_0_HIGHEST_FREQUENCY     0x0
#define ADF5611_VCO_1_LOWEST_FREQUENCY      0x1

/* ADF5611 REG0017 MAP */
#define ADF5611_FRAC2WORD_LSB_MSK	    NO_OS_GENMASK(7, 0)

/* ADF5611 REG0018 MAP */
#define ADF5611_FRAC2WORD_MID_MSK	    NO_OS_GENMASK(7, 0)

/* ADF5611 REG0019 MAP */
#define ADF5611_FRAC2WORD_MSB_MSK	    NO_OS_GENMASK(7, 0)

/* ADF5611 REG001A MAP */
#define ADF5611_MOD2WORD_LSB_MSK	    NO_OS_GENMASK(7, 0)

/* ADF5611 REG001B MAP */
#define ADF5611_MOD2WORD_MID_MSK	    NO_OS_GENMASK(7, 0)

/* ADF5611 REG001C MAP */
#define ADF5611_MOD2WORD_MSB_MSK	    NO_OS_GENMASK(7, 0)

/* ADF5611 REG001D MAP */
#define ADF5611_BLEED_I_MSK		    NO_OS_GENMASK(7, 0)

/* ADF5611 REG001E MAP */
#define ADF5611_EN_AUTOCAL_MSK              NO_OS_BIT(7)
#define ADF5611_EN_BLEED_MSK                NO_OS_BIT(6)
#define ADF5611_EN_DCLK_MODE_MSK            NO_OS_BIT(5)
#define ADF5611_EN_DNCLK_MSK                NO_OS_BIT(4)
#define ADF5611_DNCLK_DIV1_MSK              NO_OS_GENMASK(3, 2)
#define ADF5611_PFD_POL_MSK                 NO_OS_BIT(1)
#define ADF5611_BLEED_POL_MSK               NO_OS_BIT(0)

/* ADF5611 REG001E NO_OS_BIT DEFINITION */
#define ADF5611_VCO_CALIBRATION_DIS         0x0
#define ADF5611_VCO_CALIBRATION_EN          0x1

#define ADF5611_BLEED_CURRENT_DIS           0x0
#define ADF5611_BLEED_CURRENT_EN            0x1

#define ADF5611_FREQUENCY_REDUCTION_DIS     0x0
#define ADF5611_FREQUENCY_REDUCTION_EN      0x1

#define ADF5611_DIV_NCLK_OFF                0x0
#define ADF5611_DIV_NCLK_ON                 0x1

#define ADF5611_CURRENT_SINK                0x0
#define ADF5611_CURRENT_SOURCE              0x1

/* ADF5611 REG001F MAP */
#define ADF5611_R_DIV_LSB_MSK               NO_OS_GENMASK(7, 0)

/* ADF5611 REG0020 MAP */
#define ADF5611_R_DIV_MSB_MSK          	    NO_OS_GENMASK(5, 0)


/* ADF5611 REG0021 MAP */
#define ADF5611_INTMODE_EN_MSK              NO_OS_BIT(6)
#define ADF5611_RST_RDIV_MSK                NO_OS_BIT(5)
#define ADF5611_EN_RCNTR_MSK                NO_OS_BIT(4)
#define ADF5611_CP_I_MSK                    NO_OS_GENMASK(3, 0)

/* ADF5611 REG0021 NO_OS_BIT DEFINITION */
#define ADF5611_FRAC_MODE                   0x0
#define ADF5611_INTEGER_MODE                0x1

/* ADF5611 REG0022 MAP */
#define ADF5611_RFOUT_DIV_MSK      	    NO_OS_GENMASK(7, 5)
#define ADF5611_RFOUT_PWR_MSK               NO_OS_GENMASK(4, 3)
#define ADF5611_DIV_PWR_MSK                 NO_OS_GENMASK(1, 0)

/* ADF5611 REG0023 MAP*/
#define ADF5611_PHASE_WORD_LSB_MSK          NO_OS_GENMASK(7, 0)

/* ADF5611 REG0024 MAP*/
#define ADF5611_PHASE_WORD_MID_MSK          NO_OS_GENMASK(7, 0)

/* ADF5611 REG0025 MAP*/
#define ADF5611_PHASE_WORD_MSB_MSK          NO_OS_GENMASK(7, 0)

/* ADF5611 REG0026 MAP */
#define ADF5611_LSB_P1_MSK		    NO_OS_BIT(6)
#define ADF5611_VAR_MOD_EN_MSK              NO_OS_BIT(5)
#define ADF5611_DITHER1_SCALE_MSK           NO_OS_GENMASK(4, 2)
#define ADF5611_EN_DITHER2_MSK              NO_OS_BIT(1)
#define ADF5611_EN_DITHER1_MSK              NO_OS_BIT(0)

/* ADF5611 REG0027 MAP */
#define ADF5611_PD_ALL_MSK		    NO_OS_BIT(7)
#define ADF5611_PD_RDIV_MSK		    NO_OS_BIT(6)
#define ADF5611_PD_NDIV_MSK		    NO_OS_BIT(5)
#define ADF5611_PD_VCO_MSK		    NO_OS_BIT(4)
#define ADF5611_PD_LD_MSK		    NO_OS_BIT(3)
#define ADF5611_PD_PFDCP_MSK		    NO_OS_BIT(2)
#define ADF5611_PD_ADC_MSK		    NO_OS_BIT(1)
#define ADF5611_PD_CALGEN_MSK		    NO_OS_BIT(0)

/* ADF5611 REG0028 MAP */
#define ADF5611_PD_PFDNCLK_MSK		    NO_OS_BIT(1)
#define ADF5611_PD_ODIV_MSK		    NO_OS_BIT(0)

/* ADF5611 REG0029 MAP */
#define ADF5611_LDWIN_PW_MSK		    NO_OS_GENMASK(7, 5)
#define ADF5611_LD_COUNT_MSK		    NO_OS_GENMASK(4, 0)

/* ADF5611 REG002A MAP*/
#define ADF5611_EN_CP_IBX_MSK		    NO_OS_GENMASK(7, 6)
#define ADF5611_EN_LOL_MSK		    NO_OS_BIT(5)
#define ADF5611_EN_LDWIN_MSK		    NO_OS_BIT(4)
#define ADF5611_SPARE_2A_MSK                NO_OS_BIT(3)
#define ADF5611_RST_LD_MSK		    NO_OS_BIT(2)
#define ADF5611_ABPW_WD_MSK		    NO_OS_BIT(1)
#define ADF5611_RST_CNTR_MSK                NO_OS_BIT(0)

/* ADF5611 REG002B MAP */
#define ADF5611_MUXOUT_MSK		    NO_OS_GENMASK(7, 4)
#define ADF5611_EN_MUXOUT_MSK               NO_OS_BIT(3)
#define ADF5611_EN_CPTEST_MSK		    NO_OS_BIT(2)
#define ADF5611_CP_DOWN_MSK		    NO_OS_BIT(1)
#define ADF5611_CP_UP_MSK		    NO_OS_BIT(0)

/* ADF5611 REG002C MAP */
#define ADF5611_CLKODIV_DB_MSK              NO_OS_BIT(7)
#define ADF5611_DCLK_DIV_DB_MSK             NO_OS_BIT(6)
#define ADF5611_SPARE_2C_MSK                NO_OS_BIT(5)
#define ADF5611_RST_SYS_MSK		    NO_OS_BIT(4)
#define ADF5611_EN_ADC_CLK_MSK		    NO_OS_BIT(3)
#define ADF5611_EN_VCAL_MSK		    NO_OS_BIT(2)
#define ADF5611_CAL_CT_SEL_MSK		    NO_OS_BIT(1)
#define ADF5611_EN_NOTCH_MSK		    NO_OS_BIT(0)

/* ADF5611 REG002D Map */
#define ADF5611_VCO_FSM_TEST_MUX_MSK	    NO_OS_GENMASK(7, 5)
#define ADF5611_SPARE_2D_MSK		    NO_OS_GENMASK(4, 3)
#define ADF5611_O_VCO_BIAS_MSK		    NO_OS_BIT(2)
#define ADF5611_O_VCO_BAND_MSK		    NO_OS_BIT(1)
#define ADF5611_O_VCO_CORE_MSK		    NO_OS_BIT(0)


/* ADF5611 REG002F MAP */
#define ADF5611_CAL_COUNT_TO_MSK 	    NO_OS_GENMASK(7, 0)

/* ADF5611 REG0030 MAP */
#define ADF5611_CAL_VTUNE_TO_LSB_MSK	    NO_OS_GENMASK(7, 0)

/* ADF5611 REG0031 MAP */
#define ADF5611_0_VCO_DB_MSK                NO_OS_BIT(7)
#define ADF5611_CAL_VTUNE_TO_MSB_MSK        NO_OS_GENMASK(6, 0)

/* ADF5611 REG0032 MAP */
#define ADF5611_CAL_VCO_TO_LSB_MSK          NO_OS_GENMASK(7, 0)

/* ADF5611 REG0033 MAP */
#define ADF5611_DEL_CTRL_DB_MSK             NO_OS_BIT(7)
#define ADF5611_CAL_VCO_TO_MSB_MSK          NO_OS_GENMASK(6, 0)

/* ADF5611 REG0034 MAP */
#define ADF5611_CNTR_DIV_WORD_LSB_MSK       NO_OS_GENMASK(7, 0)

/* ADF5611 REG0035 MAP */
#define ADF5611_SPARE_35_MSK                NO_OS_GENMASK(7, 6)
#define ADF5611_CMOS_OV_MSK                 NO_OS_BIT(5)
#define ADF5611_CMOS_OV(x)                  no_os_field_prep(ADF5611_CMOS_OV_MSK, x)
#define ADF5611_READ_MODE_MSK               NO_OS_BIT(4)
#define ADF5611_CNTR_DIV_WORD_MSB_MSK       NO_OS_GENMASK(3, 0)

/* ADF5611 REG0036 MAP */
#define ADF5611_ADC_CLK_DIV_MSK             NO_OS_GENMASK(7, 0)

/* ADF5611 REG0037 MAP */
#define ADF5611_EN_ADC_CNV_MSK		    NO_OS_BIT(7)
#define ADF5611_EN_ADC_VTEST_MSK	    NO_OS_BIT(6)
#define ADF5611_ADC_VTEST_SEL_MSK	    NO_OS_BIT(5)
#define ADF5611_ADC_MUX_SEL_MSK		    NO_OS_BIT(4)
#define ADF5611_ADC_F_CONV_MSK		    NO_OS_BIT(3)
#define ADF5611_ADC_C_CONV_MSK		    NO_OS_BIT(2)
#define ADF5611_EN_ADC_MSK		    NO_OS_BIT(1)
#define ADF5611_ADC_CLK_TEST_SEL_MSK        NO_OS_BIT(0)

/* ADF5611 REG0038 MAP */
#define ADF5611_SPARE_38_MSK                NO_OS_BIT(7)
#define ADF5611_VPTAT_CALGEN_MSK            NO_OS_GENMASK(6, 0)

/* ADF5611 REG0039 MAP */
#define ADF5611_SPARE_39_MSK                NO_OS_BIT(7)
#define ADF5611_VCTAT_CALGEN_MSK            NO_OS_GENMASK(6, 0)

/* ADF5611 REG003A MAP */
#define ADF5611_NVMDIN_MSK                  NO_OS_GENMASK(7, 0)

/* ADF5611 REG003B MAP */
#define ADF5611_SPARE_3B_MSK		    NO_OS_BIT(7)
#define ADF5611_NVMADDR_MSK		    NO_OS_GENMASK(6, 3)
#define ADF5611_NVMNO_OS_BIT_SEL_MSK	    NO_OS_GENMASK(2, 0)

/* ADF5611 REG003C MAP */
#define ADF5611_TRIM_LATCH_MSK		    NO_OS_BIT(7)
#define ADF5611_NVMTEST_MSK		    NO_OS_BIT(6)
#define ADF5611_NVMPROG_MSK		    NO_OS_BIT(5)
#define ADF5611_NVMRD_MSK		    NO_OS_BIT(4)
#define ADF5611_NVMSTART_MSK	            NO_OS_BIT(3)
#define ADF5611_NVMON_MSK	            NO_OS_BIT(2)
#define ADF5611_MARGIN_MSK	            NO_OS_GENMASK(1, 0)

/* ADF5611 REG003D MAP */
#define ADF5611_NVMDOUT_MSK		    NO_OS_GENMASK(7, 0)

/* ADF5611 REG003E MAP */
#define ADF5611_SCAN_MODE_CODE_MSK 	    NO_OS_GENMASK(7, 0)

/* ADF5611 REG003F MAP */
#define ADF5611_TEMP_OFFSET_MSK		    NO_OS_GENMASK(7, 0)

/* ADF5611 REG0040 MAP */
#define ADF5611_SPARE_40_MSK		    NO_OS_GENMASK(7, 6)
#define ADF5611_TEMP_SLOPE_MSK		    NO_OS_GENMASK(5, 0)

/* ADF5611 REG0044 MAP */
#define ADF5611_ADC_ST_CNV_MSK		    NO_OS_BIT(0)

/* ADF5611 REG0048 MAP */
#define ADF5611_ADC_BUSY_MSK		    NO_OS_BIT(2)
#define ADF5611_FSM_BUSY_MSK		    NO_OS_BIT(1)
#define ADF5611_LOCKED_MSK		    NO_OS_BIT(0)

/* ADF5611 REG0100 MAP*/
#define ADF5611_CORE0_BIAS_TABLE_1_MSK      NO_OS_GENMASK(5, 3)
#define ADF5611_CORE0_BIAS_TABLE_0_MSK      NO_OS_GENMASK(2, 0)

/* ADF5611 REG0101 MAP*/
#define ADF5611_CORE0_BIAS_TABLE_3_MSK      NO_OS_GENMASK(5, 3)
#define ADF5611_CORE0_BIAS_TABLE_2_MSK      NO_OS_GENMASK(2, 0)

/* ADF5611 REG0102 MAP*/
#define ADF5611_CORE0_BIAS_TABLE_5_MSK      NO_OS_GENMASK(5, 3)
#define ADF5611_CORE0_BIAS_TABLE_4_MSK      NO_OS_GENMASK(2, 0)

/* ADF5611 REG0103 MAP*/
#define ADF5611_CORE0_BIAS_TABLE_7_MSK      NO_OS_GENMASK(5, 3)
#define ADF5611_CORE0_BIAS_TABLE_6_MSK      NO_OS_GENMASK(2, 0)

/* ADF5611 REG0104 MAP*/
#define ADF5611_CORE1_BIAS_TABLE_1_MSK      NO_OS_GENMASK(5, 3)
#define ADF5611_CORE1_BIAS_TABLE_0_MSK      NO_OS_GENMASK(2, 0)

/* ADF5611 REG0105 MAP*/
#define ADF5611_CORE1_BIAS_TABLE_3_MSK      NO_OS_GENMASK(5, 3)
#define ADF5611_CORE1_BIAS_TABLE_2_MSK      NO_OS_GENMASK(2, 0)

/* ADF5611 REG0106 MAP*/
#define ADF5611_CORE1_BIAS_TABLE_5_MSK      NO_OS_GENMASK(5, 3)
#define ADF5611_CORE1_BIAS_TABLE_4_MSK      NO_OS_GENMASK(2, 0)

/* ADF5611 REG0107 MAP*/
#define ADF5611_CORE1_BIAS_TABLE_7_MSK      NO_OS_GENMASK(5, 3)
#define ADF5611_CORE1_BIAS_TABLE_6_MSK      NO_OS_GENMASK(2, 0)


#define ADF5611_SPI_4W_CFG(x)		    (no_os_field_prep(ADF5611_SDO_ACTIVE_MSK, x) | \
					     no_os_field_prep(ADF5611_SDO_ACTIVE_R_MSK, x))

#define ADF5611_SPI_SCRATCHPAD_TEST	0x2A

/* ADF5611 SPECIFICATIONS */
#define ADF5611_SPI_WRITE_CMD		0x0
#define ADF5611_SPI_READ_CMD		0x8000
#define ADF5611_SPI_DUMMY_DATA		0x00
#define ADF5611_BUFF_SIZE_BYTES		3
#define ADF5611_VCO_FREQ_MIN		3650000000U // 3.65MHz
#define ADF5611_VCO_FREQ_MAX		7300000000U // 7.3MHz
#define ADF5611_MOD1WORD		0x2000000U   // 2^25
#define ADF5611_MOD2WORD_MAX		0xFFFFFFU    // 2^24 - 1
#define ADF5611_CHANNEL_SPACING_MAX	78125U
#define ADF5611_CPI_VAL_MAX		15
#define ADF5611_REF_DIV_MAX             16383
#define ADF5611_BLEED_TIME_CONST        4
#define ADF5611_BLEED_CURRENT           3125
#define ADF5611_RFOUT_PWR_MAX		3
#define ADF5611_RFOUTDIV_PWR_MAX	ADF5611_RFOUT_PWR_MAX
#define ADF5611_RFOUTDIV_DIV_MAX	7U
#define ADF5611_POR_DELAY_US		200
#define ADF5611_LKD_DELAY_US		500
#define ADF5611_RFOUT_MAX               14600000000U 	//14.6GHz
#define ADF5611_RFOUT_MIN               7300000000U 	//7.3GHz
#define ADF5611_REF_CLK_MAX		300000000000U   //300MHz                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
#define ADF5611_REF_CLK_MIN		50000000U       //50MHz
#define ADF5611_OUTPUT_DOUBLER          0x2U
#define ADF5611_PFD_FREQ_MAX		100000000U	//100MHz
#define ADF5612_RFOUT_MAX               8500000000U 	//8.5GHz
#define ADF5612_RFOUT_MIN               7300000000U 	//7.3GHz
#define ADF5612_VCO_FREQ_MAX            7300000000U 	//7.3GHz
#define KHZ				1000
#define MHZ				KHZ * KHZ
#define GHZ                             KHZ * KHZ * KHZ
#define s_TO_ns				1000000000U
#define ns_TO_ps			1000
#define uA_TO_A				1000000

/**
 * @brief Supported device ids
*/
enum adf5611_device_id {
	ID_ADF5611,
	ID_ADF5612,
};

/**
 * @struct adf5611_init_param
 * @brief ADF5611 Inintialization Parameters Structure.
*/
struct adf5611_init_param {
	/** SPI Initialization Parameters*/
	struct no_os_spi_init_param	*spi_init;
	/** SPI 4-Wire */
	bool				spi4wire;
	bool				cmos_3v3;
	uint64_t			ref_clk_freq;
	uint64_t			rfout_freq;
	uint8_t				ref_div;
	uint8_t				cp_i;
	uint16_t			bleed_word;
	uint8_t				ld_count;
	uint8_t				rfoutdiv_div;
	enum adf5611_device_id		id;
};

/**
 * @struct adf5611_dev
 * @brief ADF5611 Device Descriptor.
*/
struct adf5611_dev {
	/** SPI Descriptor */
	struct no_os_spi_desc		*spi_desc;
	/** SPI 3-Wire */
	bool				spi4wire;
	bool				cmos_3v3;
	/** Input Reference Clock */
	uint64_t			ref_clk_freq;
	/** Input Reference Clock */
	uint64_t			rfout_freq;
	uint32_t			ref_div;
	uint8_t				cp_i;
	uint16_t			bleed_word;
	uint8_t				ld_count;
	uint8_t				rfoutdiv_div;
	uint64_t			freq_max;
	uint64_t			freq_min;
	uint64_t			vco_max;
	uint64_t			vco_min;
};

/**
 * @struct reg_sequence
 * @brief ADF5611 Register Format Structure for Default Values
*/
struct reg_sequence {
	uint16_t reg;
	uint8_t val;
};

/**
 * @struct adf5611_reg_defaults
 * @brief ADF5611 register initialization
 */
static const struct reg_sequence adf5611_reg_defaults[] = {
	{ 0x0000, 0x18 },
	{ 0x001F, 0x02 },
	{ 0x0020, 0x00 },
	{ 0x001E, 0xE2 },
	{ 0x002C, 0x08 },
	{ 0x001F, 0xF3 },
	{ 0x0104, 0x24 },
	{ 0x0105, 0x24 },
	{ 0x0106, 0x24 },
	{ 0x0107, 0x24 },
	{ 0x0040, 0x2B },
	{ 0x003F, 0x5D },
	{ 0x0039, 0x22 },
	{ 0x0038, 0x4E },
	{ 0x0037, 0x82 },
	{ 0x0036, 0x3E },
	{ 0x0035, 0x00 },
	{ 0x0033, 0x00 },
	{ 0x0032, 0x64 },
	{ 0x0031, 0x00 },
	{ 0x0030, 0x32 },
	{ 0x002F, 0x19 },
	{ 0x002D, 0x00 },
	{ 0x002C, 0x08 },
	{ 0x002B, 0x00 },
	{ 0x002A, 0x70 },
	{ 0x0029, 0x2C },
	{ 0x0028, 0x01 },
	{ 0x0027, 0x00 },
	{ 0x0026, 0x00 },
	{ 0x0025, 0x7f },
	{ 0x0024, 0xff },
	{ 0x0023, 0xff },
	{ 0x0022, 0x18 },
	{ 0x0021, 0x5f },
	{ 0x0020, 0x00 },
	{ 0x001F, 0x02 },
	{ 0x001E, 0x92 },
	{ 0x001D, 0x11 },
	{ 0x001C, 0x00 },
	{ 0x001B, 0x00 },
	{ 0x001A, 0x01 },
	{ 0x0019, 0x00 },
	{ 0x0018, 0x00 },
	{ 0x0017, 0x00 },
	{ 0x0016, 0x00 },
	{ 0x0015, 0x00 },
	{ 0x0014, 0x00 },
	{ 0x0013, 0x00 },
	{ 0x0012, 0x00 },
	{ 0x0010, 0x78 },
};

/** ADF5611 SPI write */
int adf5611_spi_write(struct adf5611_dev *dev, uint16_t reg_addr, uint8_t data);

/** ADF5611 SPI Read */
int adf5611_spi_read(struct adf5611_dev *dev, uint16_t reg_addr, uint8_t *data);

/** ADF5611 updates a bit in the register space over SPI */
int adf5611_spi_update_bits(struct adf5611_dev *dev, uint16_t reg_addr,
			    uint8_t mask, uint8_t data);

/** ADF5611 Set reference frequency attribute */
int adf5611_set_ref_clk(struct adf5611_dev *dev, uint64_t val);

/** ADF5611 Get reference frequency attribute */
int adf5611_get_ref_clk(struct adf5611_dev *dev, uint64_t *val);

/** ADF5611 Set reference divider attribute */
int adf5611_set_ref_div(struct adf5611_dev *dev, int32_t div);

/** ADF5611 Get reference divider attribute */
int adf5611_get_ref_div(struct adf5611_dev *dev, int32_t *div);

/** ADF5611 Set the charge pump current attribute */
int adf5611_set_cp_i(struct adf5611_dev *dev, int32_t reg_val);

/** ADF5611 Get the charge pump current attribute */
int adf5611_get_cp_i(struct adf5611_dev *dev, int32_t *reg_val);

/** ADF5611 Set the output power attribute */
int adf5611_set_output_power(struct adf5611_dev *dev, int8_t pwr);

/** ADF5611 Get the output power attribute */
int adf5611_get_output_power(struct adf5611_dev *dev, int8_t *pwr);

/** ADF5611 Set the rfoutdiv power attribute */
int adf5611_set_rfoutdiv_power(struct adf5611_dev *dev, int32_t pwr);

/** ADF5611 Get the rfoutdiv power attribute */
int adf5611_get_rfoutdiv_power(struct adf5611_dev *dev, int32_t *pwr);

/** ADF5611 Set the rfoutdiv divider attribute */
int adf5611_set_rfoutdiv_divider(struct adf5611_dev *dev, uint8_t div_val);

/** ADF5611 Get the rfoutdiv divider attribute */
int adf5611_get_rfoutdiv_divider(struct adf5611_dev *dev, int8_t *div);

/** ADF5611 Set the enable/disable rfoutdiv */
int adf5611_set_en_rfoutdiv(struct adf5611_dev *dev, bool en);

/** ADF5611 Get the enable/disable rfoutdiv */
int adf5611_get_en_rfoutdiv(struct adf5611_dev *dev, bool *en);

/** ADF5611 Set output frequency attribute */
int adf5611_set_rfout(struct adf5611_dev *dev, uint64_t val);

/** ADF5611 Get output frequency attribute */
int adf5611_get_rfout(struct adf5611_dev *dev, uint64_t *val);

/** ADF5611 Sets frequency */
int adf5611_set_freq(struct adf5611_dev *dev);

/** ADF5611 Initialization */
int adf5611_init(struct adf5611_dev **device,
		 struct adf5611_init_param *init_param);

/** ADF5611 Remove */
int adf5611_remove(struct adf5611_dev *dev);
