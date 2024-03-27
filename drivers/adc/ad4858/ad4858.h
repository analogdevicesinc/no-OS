/***************************************************************************//**
 *   @file   ad4858.c
 *   @brief  Header file for the ad4858 drivers
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef AD4858_H_
#define AD4858_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_error.h"

#define AD4858_R1B          (1ul << 16)
#define AD4858_R2B          (2ul << 16)
#define AD4858_R3B          (3ul << 16)
#define AD4858_R4B          (4ul << 16)
#define AD4858_LEN(x)       ((x) >> 16)
#define AD4858_ADDR(x)      ((x) & 0xFFFF)

/** Register definitions */
#define AD4858_REG_INTERFACE_CONFIG_A       (AD4858_R1B | 0x00)
#define AD4858_REG_INTERFACE_CONFIG_B       (AD4858_R1B | 0x01)
#define AD4858_REG_DEVICE_CONFIG            (AD4858_R1B | 0x02)
#define AD4858_REG_CHIP_TYPE                (AD4858_R1B | 0x03)
#define AD4858_REG_PRODUCT_ID_L             (AD4858_R1B | 0x04)
#define AD4858_REG_PRODUCT_ID_H             (AD4858_R1B | 0x05)
#define AD4858_REG_CHIP_GRADE               (AD4858_R1B | 0x06)
#define AD4858_REG_SCRATCH_PAD              (AD4858_R1B | 0x0A)
#define AD4858_REG_SPI_REV                  (AD4858_R1B | 0x0B)
#define AD4858_REG_VENDOR_L                 (AD4858_R1B | 0x0C)
#define AD4858_REG_VENDOR_H                 (AD4858_R1B | 0x0D)
#define AD4858_REG_STREAM_MODE              (AD4858_R1B | 0x0E)
#define AD4858_REG_TRANSFER_CONFIG          (AD4858_R1B | 0x0F)
#define AD4858_REG_INTERFACE_CONFIG_C       (AD4858_R1B | 0x10)
#define AD4858_REG_INTERFACE_STATUS_A       (AD4858_R1B | 0x11)
#define AD4858_REG_SPI_CONFIG_D             (AD4858_R1B | 0x14)
#define AD4858_REG_DEVICE_STATUS            (AD4858_R1B | 0x20)
#define AD4858_REG_CH_OR_STATUS             (AD4858_R1B | 0x21)
#define AD4858_REG_CH_UR_STATUS             (AD4858_R1B | 0x22)
#define AD4858_REG_REGMAP_CRC               (AD4858_R2B | 0x23)
#define AD4858_REG_DEVICE_CTRL              (AD4858_R1B | 0x25)
#define AD4858_REG_PACKET                   (AD4858_R1B | 0x26)
#define AD4858_REG_OVERSAMPLE               (AD4858_R1B | 0x27)
#define AD4858_REG_SEAMLESS_HDR             (AD4858_R1B | 0x28)
#define AD4858_REG_CH_SLEEP                 (AD4858_R1B | 0x29)
#define AD4858_REG_CH_SOFTSPAN(chn)         (AD4858_R1B | (0x2A + (0x12 * chn)))
#define AD4858_REG_CH_OFFSET(chn)           (AD4858_R3B | (0x2B + (0x12 * chn)))
#define AD4858_REG_CH_GAIN(chn)             (AD4858_R2B | (0x2E + (0x12 * chn)))
#define AD4858_REG_CH_PHASE(chn)            (AD4858_R2B | (0x30 + (0x12 * chn)))
#define AD4858_REG_CH_OR(chn)               (AD4858_R3B | (0x32 + (0x12 * chn)))
#define AD4858_REG_CH_UR(chn)               (AD4858_R3B | (0x35 + (0x12 * chn)))
#define AD4858_REG_CH_TESTPAT(chn)          (AD4858_R4B | (0x38 + (0x12 * chn)))

/** AD4858_REG_INTERFACE_CONFIG_A bit masks */
#define AD4858_SW_RESET_MSK         NO_OS_BIT(7) | NO_OS_BIT(0)
#define AD4858_SDO_ENABLE_MSK       NO_OS_BIT(4)
#define AD4858_ADDR_ASCENSION_MSK   NO_OS_BIT(5)

/** AD4858_REG_INTERFACE_CONFIG_B bit masks */
#define AD4858_SINGLE_INST_MSK      NO_OS_BIT(7)

/** AD4858_REG_DEVICE_CONFIG bit masks */
#define AD4858_OPERATING_MODES_MSK  NO_OS_GENMASK(1,0)
#define AD4858_STATUS_BIT0_MSK      NO_OS_BIT(4)
#define AD4858_STATUS_BIT1_MSK      NO_OS_BIT(5)
#define AD4858_STATUS_BIT2_MSK      NO_OS_BIT(6)
#define AD4858_STATUS_BIT3_MSK      NO_OS_BIT(7)

/** AD4858_REG_TRANSFER_CONFIG bit masks */
#define AD4858_KEEP_STRM_LEN_MSK    NO_OS_BIT(2)

/** AD4858_REG_INTERFACE_CONFIG_C bit masks */
#define AD4858_ACTIVE_INF_MODE_MSK  NO_OS_GENMASK(3,2)
#define AD4858_CRC_ENABLE_MSK       NO_OS_GENMASK(7,6)

/** AD4858_REG_INTERFACE_STATUS_A bit masks */
#define AD4858_ADDR_INVALID_ERR_MSK     NO_OS_BIT(0)
#define AD4858_WR_TO_RD_ONLY_ERR_MSK    NO_OS_BIT(2)
#define AD4858_CRC_ERR_MSK              NO_OS_BIT(3)
#define AD4858_CLK_COUNT_ERR_MSK        NO_OS_BIT(4)
#define AD4858_NOT_READY_ERR_MSK        NO_OS_BIT(7)

/** AD4858_REG_SPI_CONFIG_D bit masks */
#define AD4858_CSDO_ON_SDO_MSK      NO_OS_BIT(0)

/** AD4858_REG_PACKET bit masks */
#define AD4858_TEST_PATTERN_MSK     NO_OS_BIT(2)
#define AD4858_PACKET_FORMAT_MSK    NO_OS_GENMASK(1,0)

/** AD4858_REG_OVERSAMPLE bit masks */
#define AD4858_OS_ENABLE_MSK        NO_OS_BIT(7)
#define AD4858_OS_RATIO_MSK         NO_OS_GENMASK(3,0)

/** AD4858_REG_CH_SOFTSPAN bit masks */
#define AD4858_SOFTSPAN_MSK         NO_OS_GENMASK(3,0)

/** Miscellaneous Definitions */
#define AD4858_REG_RD_BIT_MSK       NO_OS_BIT(7)
#define AD4858_PRODUCT_ID_L         0x60
#define AD4858_PRODUCT_ID_H         0x00
#define AD4858_NUM_CHANNELS         8
#define AD4858_DEF_CHN_SOFTSPAN     0xf
#define AD4858_DEF_CHN_OFFSET       0x0
#define AD4858_DEF_CHN_GAIN         0x8000
#define AD4858_DEF_CHN_PHASE        0x0
#define AD4858_DEF_CHN_OR           0x7ffff0
#define AD4858_DEF_CHN_UR           0x800000

/**
 * @enum ad4858_operating_mode
 * @brief Operating modes
 */
enum ad4858_operating_mode {
	AD4858_NORMAL_OP_MODE=0x0,
	AD4858_LOW_POWER_OP_MODE=0x3,
	AD4858_NUM_OF_OP_MODES=0x4
};

/**
 * @enum ad4858_ch_sleep_value
 * @brief Enable/diable sleep
 */
enum ad4858_ch_sleep_value {
	AD4858_SLEEP_DISABLE,
	AD4858_SLEEP_ENABLE
};

/**
 * @enum ad4858_ch_seamless_hdr
 * @brief Enable/diable seamless high dynamic range
 */
enum ad4858_ch_seamless_hdr {
	AD4858_SEAMLESS_HDR_DISABLE,
	AD4858_SEAMLESS_HDR_ENABLE
};

/**
 * @enum ad4858_interface_mode
 * @brief Interface modes
 */
enum ad4858_interface_mode {
	AD4858_CONFIG_INTERFACE_MODE,
	AD4858_DATA_INTERFACE_MODE,
	AD4858_NUM_OF_INTF_MODES
};

/**
 * @enum ad4858_spi_data_mode
 * @brief SPI data modes
 */
enum ad4858_spi_data_mode {
	AD4858_STREAMING_MODE,
	AD4858_SINGLE_INSTRUCTION_MODE,
	AD4858_NUM_OF_SPI_DATA_MODES
};

/**
 * @enum ad4858_osr_ratio
 * @brief OSR ratio values
 */
enum ad4858_osr_ratio {
	AD4858_OSR_2,
	AD4858_OSR_4,
	AD4858_OSR_8,
	AD4858_OSR_16,
	AD4858_OSR_32,
	AD4858_OSR_64,
	AD4858_OSR_128,
	AD4858_OSR_256,
	AD4858_OSR_512,
	AD4858_OSR_1024,
	AD4858_OSR_2048,
	AD4858_OSR_4096,
	AD4858_OSR_8192,
	AD4858_OSR_16384,
	AD4858_OSR_32768,
	AD4858_OSR_65536,
	AD4858_NUM_OF_OSR_RATIO
};

/**
 * @enum ad4858_packet_format
 * @brief Packet formats
 */
enum ad4858_packet_format {
	AD4858_PACKET_20_BIT,
	AD4858_PACKET_24_BIT,
	AD4858_PACKET_32_BIT,
	AD4858_NUM_OF_PACKETS
};

/**
 * @enum ad4858_chn_softspan
 * @brief Channel softspan
 */
enum ad4858_chn_softspan {
	AD4858_RANGE_0V_TO_2_5V,
	AD4858_RANGE_NEG_2_5V_TO_POS_2_5V,
	AD4858_RANGE_0V_TO_5_0V,
	AD4858_RANGE_NEG_5_0V_TO_POS_5_0V,
	AD4858_RANGE_0V_TO_6_25V,
	AD4858_RANGE_NEG_6_25V_TO_POS_6_25V,
	AD4858_RANGE_0V_TO_10_0V,
	AD4858_RANGE_NEG_10_0V_TO_POS_10_0V,
	AD4858_RANGE_0V_TO_12_5V,
	AD4858_RANGE_NEG_12_5V_TO_POS_12_5V,
	AD4858_RANGE_0V_TO_20_0V,
	AD4858_RANGE_NEG_20_0V_TO_POS_20_0V,
	AD4858_RANGE_0V_TO_25_0V,
	AD4858_RANGE_NEG_25_0V_TO_POS_25_0V,
	AD4858_RANGE_0V_TO_40_0V,
	AD4858_RANGE_NEG_40_0V_TO_POS_40_0V,
	AD4858_NUM_OF_SOFTSPAN
};

/**
 * @struct ad4858_conv_data
 * @brief ADC conversion data structure
 */
struct ad4858_conv_data {
	/* 20-bit ADC conversion raw data */
	uint32_t raw[AD4858_NUM_CHANNELS];
	/* 1-bit OR/UR status */
	bool or_ur_status[AD4858_NUM_CHANNELS];
	/* 3-bit channel ID */
	uint8_t chn_id[AD4858_NUM_CHANNELS];
	/* 4-bit softspan ID */
	uint32_t softspan_id[AD4858_NUM_CHANNELS];
};

/**
 * @struct ad4858_init_param
 * @brief AD4858 init parameters structure used for initializing the ad4858_dev
 */
struct ad4858_init_param {
	/** Host processor SPI configuration. */
	struct no_os_spi_init_param *spi_init;
	/** Power Down GPIO configuration. */
	struct no_os_gpio_init_param *gpio_pd;
	/** LVDS/CMOS select GPIO configuration. */
	struct no_os_gpio_init_param *gpio_cmos_lvds;
	/** Conversion Start GPIO configuration. */
	struct no_os_gpio_init_param *gpio_cnv;
	/** Busy GPIO configuration. */
	struct no_os_gpio_init_param *gpio_busy;
	/** Enable address ascension. */
	bool addr_ascension_enable;
	/** Operating mode. */
	enum ad4858_operating_mode operating_mode;
	/** Enable OSR. */
	bool osr_enable;
	/** OSR ratio */
	enum ad4858_osr_ratio osr_ratio;
	/** Packet format */
	enum ad4858_packet_format packet_format;
	/** Test pattern enable/disable status flag. */
	bool test_pattern;
	/** Use default/reset channel configs */
	bool use_default_chn_configs;
	/** Channel softspan value */
	enum ad4858_chn_softspan chn_softspan[AD4858_NUM_CHANNELS];
	/** Channel offset value */
	uint32_t chn_offset[AD4858_NUM_CHANNELS];
	/** Channel gain value */
	uint16_t chn_gain[AD4858_NUM_CHANNELS];
	/** Channel phase value */
	uint16_t chn_phase[AD4858_NUM_CHANNELS];
	/** Channel overrange limit value */
	uint16_t chn_or[AD4858_NUM_CHANNELS];
	/** Channel underrange limit value */
	uint16_t chn_ur[AD4858_NUM_CHANNELS];
	/** Channel sleep value */
	enum ad4858_ch_sleep_value chn_sleep_value[AD4858_NUM_CHANNELS];
	/** Channel seamless HDR value */
	enum ad4858_ch_seamless_hdr chn_seamless_hdr[AD4858_NUM_CHANNELS];
};

/**
 * @struct ad4858_dev
 * @brief AD4858 device descriptor
 */
struct ad4858_dev {
	/** SPI descriptor. */
	struct no_os_spi_desc *spi_desc;
	/** Power Down GPIO descriptor. */
	struct no_os_gpio_desc *gpio_pd;
	/** LVDS/CMOS select GPIO descriptor. */
	struct no_os_gpio_desc *gpio_cmos_lvds;
	/** Conversion Start GPIO descriptor. */
	struct no_os_gpio_desc *gpio_cnv;
	/** Busy GPIO descriptor. */
	struct no_os_gpio_desc *gpio_busy;
	/** Address ascension enable status. */
	bool addr_ascension_enable;
	/** Operating mode. */
	enum ad4858_operating_mode operating_mode;
	/** SPI data mode. */
	enum ad4858_spi_data_mode spi_data_mode;
	/** OSR enable status. */
	bool osr_enable;
	/** OSR ratio */
	enum ad4858_osr_ratio osr_ratio;
	/** Packet format */
	enum ad4858_packet_format packet_format;
	/** Test pattern enable/disable status flag. */
	bool test_pattern;
	/** Channel softspan value */
	enum ad4858_chn_softspan chn_softspan[AD4858_NUM_CHANNELS];
	/** Channel offset value */
	uint32_t chn_offset[AD4858_NUM_CHANNELS];
	/** Channel gain value */
	uint16_t chn_gain[AD4858_NUM_CHANNELS];
	/** Channel phase value */
	uint16_t chn_phase[AD4858_NUM_CHANNELS];
	/** Channel overrange limit value */
	uint16_t chn_or[AD4858_NUM_CHANNELS];
	/** Channel underrange limit value */
	uint16_t chn_ur[AD4858_NUM_CHANNELS];
	/** Big endianess status flag */
	bool big_endian;
	/** Channel sleep value */
	enum ad4858_ch_sleep_value chn_sleep_value[AD4858_NUM_CHANNELS];
	/** Channel seamless HDR value */
	enum ad4858_ch_seamless_hdr chn_seamless_hdr[AD4858_NUM_CHANNELS];
};

/* Initialize the device */
int ad4858_init(struct ad4858_dev **device,
		struct ad4858_init_param *init_param);

/* Remove the device */
int ad4858_remove(struct ad4858_dev *dev);

/* Write device register */
int ad4858_reg_write(struct ad4858_dev *dev, uint32_t reg_addr,
		     uint32_t reg_val);

/* Read device register */
int ad4858_reg_read(struct ad4858_dev *dev, uint32_t reg_addr,
		    uint32_t *reg_val);

/* Update specific register bits of an input register */
int ad4858_reg_mask(struct ad4858_dev *dev,
		    uint32_t reg_addr,
		    uint32_t mask,
		    uint32_t reg_val);

/* Software reset of the device */
int ad4858_soft_reset(struct ad4858_dev *dev);

/* Set operating mode */
int ad4858_set_operating_mode(struct ad4858_dev *dev,
			      enum ad4858_operating_mode mode);

/* Set SPI data mode */
int ad4858_set_spi_data_mode(struct ad4858_dev *dev,
			     enum ad4858_spi_data_mode mode);

/* Set device config interface mode. */
int ad4858_set_config_interface_mode(struct ad4858_dev *dev);

/* Set device data interface mode. */
int ad4858_set_data_interface_mode(struct ad4858_dev *dev);

/* Enable/Disable OSR */
int ad4858_enable_osr(struct ad4858_dev *dev, bool osr_status);

/* Set OSR ratio */
int ad4858_set_osr_ratio(struct ad4858_dev *dev,
			 enum ad4858_osr_ratio osr_ratio);

/* Set packet format */
int ad4858_set_packet_format(struct ad4858_dev *dev,
			     enum ad4858_packet_format packet_format);

/* Enable/Disable test pattern on ADC data output. */
int ad4858_enable_test_pattern(struct ad4858_dev *dev, bool test_pattern);

/* Set channel softspan */
int ad4858_set_chn_softspan(struct ad4858_dev *dev, uint8_t chn,
			    enum ad4858_chn_softspan chn_softspan);

/* Set channel offset */
int ad4858_set_chn_offset(struct ad4858_dev *dev, uint8_t chn,
			  uint32_t offset);

/* Set channel gain */
int ad4858_set_chn_gain(struct ad4858_dev *dev, uint8_t chn, uint16_t gain);

/* Set channel phase */
int ad4858_set_chn_phase(struct ad4858_dev *dev, uint8_t chn,
			 uint16_t phase);

/* Set channel overrange (OR) limit */
int ad4858_set_chn_or_limit(struct ad4858_dev *dev, uint8_t chn,
			    uint32_t or_limit);

/* Set channel underrange (UR) limit */
int ad4858_set_chn_ur_limit(struct ad4858_dev *dev, uint8_t chn,
			    uint32_t ur_limit);

/* Toggle the CNV pin to start a conversion. */
int ad4858_convst(struct ad4858_dev *dev);

/* Perform ADC conversion. */
int ad4858_perform_conv(struct ad4858_dev *dev);

/* Read ADC conversion data over SPI. */
int ad4858_spi_data_read(struct ad4858_dev *dev, struct ad4858_conv_data *data);

/* Perform conversion and read ADC data (for all channels). */
int ad4858_read_data(struct ad4858_dev *dev, struct ad4858_conv_data *data);

/* Enable/Disable channel sleep */
int ad4858_enable_ch_sleep(struct ad4858_dev* dev, uint8_t chn,
			   enum ad4858_ch_sleep_value sleep_status);

/* Enable/Disable seamless HDR */
int ad4858_enable_ch_seamless_hdr(struct ad4858_dev* dev, uint8_t chn,
				  enum ad4858_ch_seamless_hdr seamless_hdr_status);

#endif	// AD4858_H_
