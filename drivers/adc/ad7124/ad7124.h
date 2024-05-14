/***************************************************************************//**
*   @file    ad7124.h
*   @brief   AD7124 header file.
*   	     Devices: AD7124-4, AD7124-8
*
********************************************************************************
* Copyright 2015-2019, 2023(c) Analog Devices, Inc.
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
#ifndef __AD7124_H__
#define __AD7124_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_delay.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define	AD7124_RW 1   /* Read and Write */
#define	AD7124_R  2   /* Read only */
#define AD7124_W  3   /* Write only */

/* Total Number of Setups */
#define AD7124_MAX_SETUPS	8
/* Maximum number of channels */
#define AD7124_MAX_CHANNELS	16

/* AD7124-4 Standard Device ID */
#define AD7124_4_STD_ID  0x04
/* AD7124-4 B Grade Device ID */
#define AD7124_4_B_GRADE_ID  0x06
/* Device ID for the re-designed die in the AD7124-4 standard part and B-grade */
#define AD7124_4_NEW_ID  0x07

/* AD7124-8 Standard Device ID */
#define AD7124_8_STD_ID  0x14
/* AD7124-8 B and W Grade Device ID */
#define AD7124_8_B_W_GRADE_ID  0x16
/* Device ID for the re-designed die in the AD7124-8 standard part, B-grade and W-grade */
#define AD7124_8_NEW_ID  0x17

/* AD7124 Register Map */
#define AD7124_COMM_REG      0x00
#define AD7124_STATUS_REG    0x00
#define AD7124_ADC_CTRL_REG  0x01
#define AD7124_DATA_REG      0x02
#define AD7124_IO_CTRL1_REG  0x03
#define AD7124_IO_CTRL2_REG  0x04
#define AD7124_ID_REG        0x05
#define AD7124_ERR_REG       0x06
#define AD7124_ERREN_REG     0x07
#define AD7124_CH0_MAP_REG   0x09
#define AD7124_CH1_MAP_REG   0x0A
#define AD7124_CH2_MAP_REG   0x0B
#define AD7124_CH3_MAP_REG   0x0C
#define AD7124_CH4_MAP_REG   0x0D
#define AD7124_CH5_MAP_REG   0x0E
#define AD7124_CH6_MAP_REG   0x0F
#define AD7124_CH7_MAP_REG   0x10
#define AD7124_CH8_MAP_REG   0x11
#define AD7124_CH9_MAP_REG   0x12
#define AD7124_CH10_MAP_REG  0x13
#define AD7124_CH11_MAP_REG  0x14
#define AD7124_CH12_MAP_REG  0x15
#define AD7124_CH13_MAP_REG  0x16
#define AD7124_CH14_MAP_REG  0x17
#define AD7124_CH15_MAP_REG  0x18
#define AD7124_CFG0_REG      0x19
#define AD7124_CFG1_REG      0x1A
#define AD7124_CFG2_REG      0x1B
#define AD7124_CFG3_REG      0x1C
#define AD7124_CFG4_REG      0x1D
#define AD7124_CFG5_REG      0x1E
#define AD7124_CFG6_REG      0x1F
#define AD7124_CFG7_REG      0x20
#define AD7124_FILT0_REG     0x21
#define AD7124_FILT1_REG     0x22
#define AD7124_FILT2_REG     0x23
#define AD7124_FILT3_REG     0x24
#define AD7124_FILT4_REG     0x25
#define AD7124_FILT5_REG     0x26
#define AD7124_FILT6_REG     0x27
#define AD7124_FILT7_REG     0x28
#define AD7124_OFFS0_REG     0x29
#define AD7124_OFFS1_REG     0x2A
#define AD7124_OFFS2_REG     0x2B
#define AD7124_OFFS3_REG     0x2C
#define AD7124_OFFS4_REG     0x2D
#define AD7124_OFFS5_REG     0x2E
#define AD7124_OFFS6_REG     0x2F
#define AD7124_OFFS7_REG     0x30
#define AD7124_GAIN0_REG     0x31
#define AD7124_GAIN1_REG     0x32
#define AD7124_GAIN2_REG     0x33
#define AD7124_GAIN3_REG     0x34
#define AD7124_GAIN4_REG     0x35
#define AD7124_GAIN5_REG     0x36
#define AD7124_GAIN6_REG     0x37
#define AD7124_GAIN7_REG     0x38

/* Communication Register bits */
#define AD7124_COMM_REG_WEN    (0 << 7)
#define AD7124_COMM_REG_WR     (0 << 6)
#define AD7124_COMM_REG_RD     (1 << 6)
#define AD7124_COMM_REG_RA(x)  ((x) & 0x3F)

/* Status Register bits */
#define AD7124_STATUS_REG_RDY          (1 << 7)
#define AD7124_STATUS_REG_ERROR_FLAG   (1 << 6)
#define AD7124_STATUS_REG_POR_FLAG     (1 << 4)
#define AD7124_STATUS_REG_CH_ACTIVE(x) ((x) & 0xF)

/* ADC_Control Register bits */
#define AD7124_ADC_CTRL_REG_DOUT_RDY_DEL   (1 << 12)
#define AD7124_ADC_CTRL_REG_CONT_READ      (1 << 11)
#define AD7124_ADC_CTRL_REG_DATA_STATUS    (1 << 10)
#define AD7124_ADC_CTRL_REG_CS_EN          (1 << 9)
#define AD7124_ADC_CTRL_REG_REF_EN         (1 << 8)
#define AD7124_ADC_CTRL_REG_POWER_MODE(x)  (((x) & 0x3) << 6)
#define AD7124_ADC_CTRL_REG_MODE(x)        (((x) & 0xF) << 2)
#define AD7124_ADC_CTRL_REG_CLK_SEL(x)     (((x) & 0x3) << 0)

/* IO_Control_1 Register bits */
#define AD7124_IO_CTRL1_REG_GPIO_DAT2     (1 << 23)
#define AD7124_IO_CTRL1_REG_GPIO_DAT1     (1 << 22)
#define AD7124_IO_CTRL1_REG_GPIO_CTRL2    (1 << 19)
#define AD7124_IO_CTRL1_REG_GPIO_CTRL1    (1 << 18)
#define AD7124_IO_CTRL1_REG_PDSW          (1 << 15)
#define AD7124_IO_CTRL1_REG_IOUT1(x)      (((x) & 0x7) << 11)
#define AD7124_IO_CTRL1_REG_IOUT0(x)      (((x) & 0x7) << 8)
#define AD7124_IO_CTRL1_REG_IOUT_CH1(x)   (((x) & 0xF) << 4)
#define AD7124_IO_CTRL1_REG_IOUT_CH0(x)   (((x) & 0xF) << 0)

/* IO_Control_1 AD7124-8 specific bits */
#define AD7124_8_IO_CTRL1_REG_GPIO_DAT4     (1 << 23)
#define AD7124_8_IO_CTRL1_REG_GPIO_DAT3     (1 << 22)
#define AD7124_8_IO_CTRL1_REG_GPIO_DAT2     (1 << 21)
#define AD7124_8_IO_CTRL1_REG_GPIO_DAT1     (1 << 20)
#define AD7124_8_IO_CTRL1_REG_GPIO_CTRL4    (1 << 19)
#define AD7124_8_IO_CTRL1_REG_GPIO_CTRL3    (1 << 18)
#define AD7124_8_IO_CTRL1_REG_GPIO_CTRL2    (1 << 17)
#define AD7124_8_IO_CTRL1_REG_GPIO_CTRL1    (1 << 16)

/* IO_Control_2 Register bits */
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS7   (1 << 15)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS6   (1 << 14)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS5   (1 << 11)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS4   (1 << 10)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS3   (1 << 5)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS2   (1 << 4)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS1   (1 << 1)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS0   (1 << 0)

/* IO_Control_2 AD7124-8 specific bits */
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS15  (1 << 15)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS14  (1 << 14)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS13  (1 << 13)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS12  (1 << 12)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS11  (1 << 11)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS10  (1 << 10)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS9   (1 << 9)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS8   (1 << 8)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS7   (1 << 7)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS6   (1 << 6)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS5   (1 << 5)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS4   (1 << 4)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS3   (1 << 3)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS2   (1 << 2)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS1   (1 << 1)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS0   (1 << 0)

/* ID Register bits */
#define AD7124_ID_REG_DEVICE_ID(x)   (((x) & 0xF) << 4)
#define AD7124_ID_REG_SILICON_REV(x) (((x) & 0xF) << 0)

/* Error Register bits */
#define AD7124_ERR_REG_LDO_CAP_ERR        (1 << 19)
#define AD7124_ERR_REG_ADC_CAL_ERR        (1 << 18)
#define AD7124_ERR_REG_ADC_CONV_ERR       (1 << 17)
#define AD7124_ERR_REG_ADC_SAT_ERR        (1 << 16)
#define AD7124_ERR_REG_AINP_OV_ERR        (1 << 15)
#define AD7124_ERR_REG_AINP_UV_ERR        (1 << 14)
#define AD7124_ERR_REG_AINM_OV_ERR        (1 << 13)
#define AD7124_ERR_REG_AINM_UV_ERR        (1 << 12)
#define AD7124_ERR_REG_REF_DET_ERR        (1 << 11)
#define AD7124_ERR_REG_DLDO_PSM_ERR       (1 << 9)
#define AD7124_ERR_REG_ALDO_PSM_ERR       (1 << 7)
#define AD7124_ERR_REG_SPI_IGNORE_ERR     (1 << 6)
#define AD7124_ERR_REG_SPI_SLCK_CNT_ERR   (1 << 5)
#define AD7124_ERR_REG_SPI_READ_ERR       (1 << 4)
#define AD7124_ERR_REG_SPI_WRITE_ERR      (1 << 3)
#define AD7124_ERR_REG_SPI_CRC_ERR        (1 << 2)
#define AD7124_ERR_REG_MM_CRC_ERR         (1 << 1)
#define AD7124_ERR_REG_ROM_CRC_ERR        (1 << 0)

/* Error_En Register bits */
#define AD7124_ERREN_REG_MCLK_CNT_EN           (1 << 22)
#define AD7124_ERREN_REG_LDO_CAP_CHK_TEST_EN   (1 << 21)
#define AD7124_ERREN_REG_LDO_CAP_CHK(x)        (((x) & 0x3) << 19)
#define AD7124_ERREN_REG_ADC_CAL_ERR_EN        (1 << 18)
#define AD7124_ERREN_REG_ADC_CONV_ERR_EN       (1 << 17)
#define AD7124_ERREN_REG_ADC_SAT_ERR_EN        (1 << 16)
#define AD7124_ERREN_REG_AINP_OV_ERR_EN        (1 << 15)
#define AD7124_ERREN_REG_AINP_UV_ERR_EN        (1 << 14)
#define AD7124_ERREN_REG_AINM_OV_ERR_EN        (1 << 13)
#define AD7124_ERREN_REG_AINM_UV_ERR_EN        (1 << 12)
#define AD7124_ERREN_REG_REF_DET_ERR_EN        (1 << 11)
#define AD7124_ERREN_REG_DLDO_PSM_TRIP_TEST_EN (1 << 10)
#define AD7124_ERREN_REG_DLDO_PSM_ERR_ERR      (1 << 9)
#define AD7124_ERREN_REG_ALDO_PSM_TRIP_TEST_EN (1 << 8)
#define AD7124_ERREN_REG_ALDO_PSM_ERR_EN       (1 << 7)
#define AD7124_ERREN_REG_SPI_IGNORE_ERR_EN     (1 << 6)
#define AD7124_ERREN_REG_SPI_SCLK_CNT_ERR_EN   (1 << 5)
#define AD7124_ERREN_REG_SPI_READ_ERR_EN       (1 << 4)
#define AD7124_ERREN_REG_SPI_WRITE_ERR_EN      (1 << 3)
#define AD7124_ERREN_REG_SPI_CRC_ERR_EN        (1 << 2)
#define AD7124_ERREN_REG_MM_CRC_ERR_EN         (1 << 1)
#define AD7124_ERREN_REG_ROM_CRC_ERR_EN        (1 << 0)

/* Channel Registers 0-15 bits */
#define AD7124_CH_MAP_REG_CH_ENABLE    (1 << 15)
#define AD7124_CH_MAP_REG_SETUP(x)     (((x) & 0x7) << 12)
#define AD7124_CH_MAP_REG_AINP(x)      (((x) & 0x1F) << 5)
#define AD7124_CH_MAP_REG_AINM(x)      (((x) & 0x1F) << 0)

/* Configuration Registers 0-7 bits */
#define AD7124_CFG_REG_BIPOLAR     (1 << 11)
#define AD7124_CFG_REG_BURNOUT(x)  (((x) & 0x3) << 9)
#define AD7124_CFG_REG_REF_BUFP    (1 << 8)
#define AD7124_CFG_REG_REF_BUFM    (1 << 7)
#define AD7124_CFG_REG_AIN_BUFP    (1 << 6)
#define AD7124_CFG_REG_AINN_BUFM   (1 << 5)
#define AD7124_CFG_REG_REF_SEL(x)  ((x) & 0x3) << 3
#define AD7124_CFG_REG_PGA(x)      (((x) & 0x7) << 0)

/* Filter Register 0-7 bits */
#define AD7124_FILT_REG_FILTER(x)         (((x) & 0x7) << 21)
#define AD7124_FILT_REG_REJ60             (1 << 20)
#define AD7124_FILT_REG_POST_FILTER(x)    (((x) & 0x7) << 17)
#define AD7124_FILT_REG_SINGLE_CYCLE      (1 << 16)
#define AD7124_FILT_REG_FS(x)             (((x) & 0x7FF) << 0)

#define AD7124_CRC8_POLYNOMIAL_REPRESENTATION 0x07 /* x8 + x2 + x + 1 */
#define AD7124_DISABLE_CRC 0
#define AD7124_USE_CRC 1
#define AD7124_CHMAP_REG_SETUP_SEL_MSK  	NO_OS_GENMASK(14,12)
#define AD7124_CHMAP_REG_AINPOS_MSK    		NO_OS_GENMASK(9,5)
#define AD7124_CHMAP_REG_AINNEG_MSK    		NO_OS_GENMASK(4,0)
#define AD7124_ADC_CTRL_REG_MODE_MSK   		NO_OS_GENMASK(5,2)
#define AD7124_SETUP_CONF_REG_REF_SEL_MSK	NO_OS_GENMASK(4,3)
#define AD7124_REF_BUF_MSK                  NO_OS_GENMASK(8,7)
#define AD7124_AIN_BUF_MSK                  NO_OS_GENMASK(6,5)
#define AD7124_POWER_MODE_MSK			    NO_OS_GENMASK(7,6)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @enum	ad7124_device_type
 * @details AD7124 Device definitions
**/
enum ad7124_device_type {
	ID_AD7124_4,
	ID_AD7124_8
};

/**
 * @enum  ad7124_mode
 * @brief ADC Modes of Operation
**/
enum ad7124_mode {
	AD7124_CONTINUOUS,
	AD7124_SINGLE,
	AD7124_STANDBY,
	AD7124_POWER_DOWN,
	AD7124_IDLE,
	AD7124_IN_ZERO_SCALE_OFF,
	AD7124_IN_FULL_SCALE_GAIN,
	AD7124_SYS_ZERO_SCALE_OFF,
	AD7124_SYS_ZERO_SCALE_GAIN,
	ADC_MAX_MODES
};

/**
 * @enum ad7124_analog_input
 * @brief ADC input sources for each channel.
 **/
enum ad7124_analog_input {
	AD7124_AIN0,
	AD7124_AIN1,
	AD7124_AIN2,
	AD7124_AIN3,
	AD7124_AIN4,
	AD7124_AIN5,
	AD7124_AIN6,
	AD7124_AIN7,
	AD7124_AIN8,
	AD7124_AIN9,
	AD7124_AIN10,
	AD7124_AIN11,
	AD7124_AIN12,
	AD7124_AIN13,
	AD7124_AIN14,
	AD7124_AIN15,
	AD7124_AVSS,
	AD7124_TEMP_SENSOR,
	AD7124_IN_REF,
	AD7124_DGND,
	AD7124_AVDD_AVSS_P,
	AD7124_AVDD_AVSS_M,
	AD7124_IOVDD_DGND_P,
	AD7124_IOVDD_DGND_M,
	AD7124_ALDO_AVSS_P,
	AD7124_ALDO_AVSS_M,
	AD7124_DLDO_DGND_P,
	AD7124_DLDO_DGND_M,
	AD7124_V_20MV_P,
	AD7124_V_20MV_M
};

/**
 * @struct ad7124_analog_inputs
 * @brief Positive and negative analog inputs
**/
struct ad7124_analog_inputs {
	enum ad7124_analog_input ainp;
	enum ad7124_analog_input ainm;
};

/**
 * @struct ad7124_channel_map
 * @brief Channel mapping
**/
struct ad7124_channel_map {
	bool channel_enable;
	uint8_t setup_sel;
	struct ad7124_analog_inputs ain;
};

/**
 * @enum ad7124_reference_source
 * @brief Type of ADC Reference
**/
enum ad7124_reference_source {
	/* External Reference REFIN1+/-*/
	EXTERNAL_REFIN1,
	/* External Reference REFIN2+/-*/
	EXTERNAL_REFIN2,
	/* Internal 2.5V Reference */
	INTERNAL_REF,
	/* AVDD - AVSS */
	AVDD_AVSS,
	/* Maximum Reference Sources */
	MAX_REF_SOURCES
};

/**
  * @struct ad7124_channel_setup
  * @brief Channel setup
**/
struct ad7124_channel_setup {
	bool bi_unipolar;
	bool ref_buff;
	bool  ain_buff;
	enum ad7124_reference_source ref_source;
};

/**
  * @enum ad7124_power_mode
  * @brief Power modes
**/
enum ad7124_power_mode {
	AD7124_LOW_POWER,
	AD7124_MID_POWER,
	AD7124_HIGH_POWER
};

/* Device register info */
struct ad7124_st_reg {
	int32_t addr;
	int32_t value;
	int32_t size;
	int32_t rw;
};

/* AD7124 registers list */
enum ad7124_registers {
	AD7124_Status,
	AD7124_ADC_Control,
	AD7124_Data,
	AD7124_IOCon1,
	AD7124_IOCon2,
	AD7124_ID,
	AD7124_Error,
	AD7124_Error_En,
	AD7124_Mclk_Count,
	AD7124_Channel_0,
	AD7124_Channel_1,
	AD7124_Channel_2,
	AD7124_Channel_3,
	AD7124_Channel_4,
	AD7124_Channel_5,
	AD7124_Channel_6,
	AD7124_Channel_7,
	AD7124_Channel_8,
	AD7124_Channel_9,
	AD7124_Channel_10,
	AD7124_Channel_11,
	AD7124_Channel_12,
	AD7124_Channel_13,
	AD7124_Channel_14,
	AD7124_Channel_15,
	AD7124_Config_0,
	AD7124_Config_1,
	AD7124_Config_2,
	AD7124_Config_3,
	AD7124_Config_4,
	AD7124_Config_5,
	AD7124_Config_6,
	AD7124_Config_7,
	AD7124_Filter_0,
	AD7124_Filter_1,
	AD7124_Filter_2,
	AD7124_Filter_3,
	AD7124_Filter_4,
	AD7124_Filter_5,
	AD7124_Filter_6,
	AD7124_Filter_7,
	AD7124_Offset_0,
	AD7124_Offset_1,
	AD7124_Offset_2,
	AD7124_Offset_3,
	AD7124_Offset_4,
	AD7124_Offset_5,
	AD7124_Offset_6,
	AD7124_Offset_7,
	AD7124_Gain_0,
	AD7124_Gain_1,
	AD7124_Gain_2,
	AD7124_Gain_3,
	AD7124_Gain_4,
	AD7124_Gain_5,
	AD7124_Gain_6,
	AD7124_Gain_7,
	AD7124_REG_NO
};

/**
 * The structure describes the device and is used with the ad7124 driver.
 * @brief Device Structure
 **/
struct ad7124_dev {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
	/* Device Settings */
	struct ad7124_st_reg	*regs;
	int16_t use_crc;
	int16_t check_ready;
	int16_t spi_rdy_poll_cnt;
	enum ad7124_mode mode;
	/* Active Device */
	enum ad7124_device_type active_device;
	/* Reference enable */
	bool ref_en;
	/* Power modes */
	enum ad7124_power_mode power_mode;
	/* Setups */
	struct ad7124_channel_setup setups[AD7124_MAX_SETUPS];
	/* Channel Mapping*/
	struct ad7124_channel_map chan_map[AD7124_MAX_CHANNELS];
};

struct ad7124_init_param {
	/* SPI */
	struct no_os_spi_init_param		*spi_init;
	/* Device Settings */
	struct ad7124_st_reg	*regs;
	int16_t use_crc;
	int16_t check_ready;
	int16_t spi_rdy_poll_cnt;
	enum ad7124_mode mode;
	/* Active Device */
	enum ad7124_device_type active_device;
	/* Reference enable */
	bool ref_en;
	/* Power modes */
	enum ad7124_power_mode power_mode;
	/* Setups */
	struct ad7124_channel_setup setups[AD7124_MAX_SETUPS];
	/* Channel Mapping*/
	struct ad7124_channel_map chan_map[AD7124_MAX_CHANNELS];
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Reads the value of the specified register without a device state check. */
int32_t ad7124_no_check_read_register(struct ad7124_dev *dev,
				      struct ad7124_st_reg* p_reg);

/* Writes the value of the specified register without a device state check. */
int32_t ad7124_no_check_write_register(struct ad7124_dev *dev,
				       struct ad7124_st_reg reg);

/* Reads the value of the specified register. */
int32_t ad7124_read_register(struct ad7124_dev *dev,
			     struct ad7124_st_reg* p_reg);

/* Wrap the read register function to give it a modern signature. */
int32_t ad7124_read_register2(struct ad7124_dev *dev,
			      uint32_t reg,
			      uint32_t *readval);

/* Writes the value of the specified register. */
int32_t ad7124_write_register(struct ad7124_dev *dev,
			      struct ad7124_st_reg reg);

/* Wrap the write register function to give it a modern signature. */
int32_t ad7124_write_register2(struct ad7124_dev *dev,
			       uint32_t reg,
			       uint32_t writeval);

/* Resets the device. */
int32_t ad7124_reset(struct ad7124_dev *dev);

/* Waits until the device can accept read and write user actions. */
int32_t ad7124_wait_for_spi_ready(struct ad7124_dev *dev,
				  uint32_t timeout);

/* Waits until the device finishes the power-on reset operation. */
int32_t ad7124_wait_to_power_on(struct ad7124_dev *dev,
				uint32_t timeout);

/* Waits until a new conversion result is available. */
int32_t ad7124_wait_for_conv_ready(struct ad7124_dev *dev,
				   uint32_t timeout);

/* Reads the conversion result from the device. */
int32_t ad7124_read_data(struct ad7124_dev *dev,
			 int32_t* p_data);

/* Get the ID of the channel of the latest conversion. */
int32_t ad7124_get_read_chan_id(struct ad7124_dev *dev, uint32_t *status);

/* Computes the CRC checksum for a data buffer. */
uint8_t ad7124_compute_crc8(uint8_t* p_buf,
			    uint8_t buf_size);

/* Computes the XOR checksum for a data buffer. */
uint8_t AD7124_ComputeXOR8(uint8_t * p_buf,
			   uint8_t buf_size);

/* Updates the CRC settings. */
void ad7124_update_crcsetting(struct ad7124_dev *dev);

/* Updates the device SPI interface settings. */
void ad7124_update_dev_spi_settings(struct ad7124_dev *dev);

/* Get the AD7124 reference clock. */
int32_t ad7124_fclk_get(struct ad7124_dev *dev, float *f_clk);

/* Get the filter coefficient for the sample rate. */
int32_t ad7124_fltcoff_get(struct ad7124_dev *dev,
			   int16_t chn_no,
			   uint16_t *flt_coff);

/* Calculate ODR of the device. */
float ad7124_get_odr(struct ad7124_dev *dev, int16_t ch_no);

/* Set ODR of the device. */
int32_t ad7124_set_odr(struct ad7124_dev *dev,
		       float odr,
		       int16_t chn_no);

/* SPI write to device using a mask. */
int ad7124_reg_write_msk(struct ad7124_dev *dev,
			 uint32_t reg_addr,
			 uint32_t data,
			 uint32_t mask);

/* Set ADC Mode */
int ad7124_set_adc_mode(struct ad7124_dev *device, enum ad7124_mode mode);

/* Enable/Disable Channels */
int ad7124_set_channel_status(struct ad7124_dev *device,
			      uint8_t chn_no,
			      bool channel_status);

/* Configure Analog inputs to channel */
int ad7124_connect_analog_input(struct ad7124_dev *device,
				uint8_t chn_no,
				struct ad7124_analog_inputs analog_input);

/* Assign setup to channel */
int ad7124_assign_setup(struct ad7124_dev *device,
			uint8_t ch_no,
			uint8_t setup);

/* Assign polarity to setup */
int ad7124_set_polarity(struct ad7124_dev* device,
			bool bipolar,
			uint8_t setup_id);

/* Assign reference source to setup */
int ad7124_set_reference_source(struct ad7124_dev* device,
				enum ad7124_reference_source ref_source,
				uint8_t setup_id,
				bool ref_en);

/* Enable/Disable input and reference buffers to setup */
int ad7124_enable_buffers(struct ad7124_dev* device,
			  bool ain_buff,
			  bool ref_buff,
			  uint8_t setup_id);

/* Select the power mode */
int ad7124_set_power_mode(struct ad7124_dev *device,
			  enum ad7124_power_mode mode);

/* Initializes the AD7124 */
int32_t ad7124_setup(struct ad7124_dev **device,
		     struct ad7124_init_param *init_param);

/* Free the resources allocated by ad7124_setup(). */
int32_t ad7124_remove(struct ad7124_dev *dev);

#endif /* __AD7124_H__ */

