/***************************************************************************//**
 *   @file   ad4080.h
 *   @brief  Header file of AD4080 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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

#ifndef __AD4080_H__
#define __AD4080_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/** Register Definition */
#define AD4080_REG_INTERFACE_CONFIG_A		0x0000
#define AD4080_REG_INTERFACE_CONFIG_B		0x0001
#define AD4080_REG_DEVICE_CONFIG		0x0002
#define AD4080_REG_CHIP_TYPE			0x0003
#define AD4080_REG_PRODUCT_ID_L			0x0004
#define AD4080_REG_PRODUCT_ID_H			0x0005
#define AD4080_REG_CHIP_GRADE			0x0006
#define AD4080_REG_SCRATCH_PAD			0x000A
#define AD4080_REG_SPI_REVISION			0x000B
#define AD4080_REG_VENDOR_L			0x000C
#define AD4080_REG_VENDOR_H			0x000D
#define AD4080_REG_STREAM_MODE			0x000E
#define AD4080_REG_TRANSFER_CONFIG		0x000F
#define AD4080_REG_INTERFACE_CONFIG_C		0x0010
#define AD4080_REG_INTERFACE_STATUS_A		0x0011
#define AD4080_REG_DEVICE_STATUS		0x0014
#define AD4080_REG_DATA_INTF_CONFIG_A		0x0015
#define AD4080_REG_DATA_INTF_CONFIG_B		0x0016
#define AD4080_REG_DATA_INTF_CONFIG_C		0x0017
#define AD4080_REG_PWR_CTRL			0x0018
#define AD4080_REG_GPIO_CONFIG_A		0x0019
#define AD4080_REG_GPIO_CONFIG_B		0x001A
#define AD4080_REG_GPIO_CONFIG_C		0x001B
#define AD4080_REG_GENERAL_CONFIG		0x001C
#define AD4080_REG_FIFO_WATERMARK		0x001D
#define AD4080_REG_EVENT_HYSTERESIS		0x001F
#define AD4080_REG_EVENT_DETECTION_HI		0x0021
#define AD4080_REG_EVENT_DETECTION_LO		0x0023
#define AD4080_REG_OFFSET			0x0025
#define AD4080_REG_GAIN				0x0027
#define AD4080_REG_FILTER_CONFIG		0x0029

/** AD4080_REG_INTERFACE_CONFIG_A Bit Definition */
#define AD4080_SW_RESET_MSK			NO_OS_BIT(7) | NO_OS_BIT(0)
#define AD4080_ADDR_ASC_MSK			NO_OS_BIT(5)
#define AD4080_SDO_ENABLE_MSK			NO_OS_BIT(4)

/** AD4080_REG_INTERFACE_CONFIG_B Bit Definition */
#define AD4080_SINGLE_INST_MSK			NO_OS_BIT(7)
#define AD4080_SHORT_INST_MSK			NO_OS_BIT(3)

/** AD4080_REG_DEVICE_CONFIG Bit Definition */
#define AD4080_OP_MODE_MSK			NO_OS_GENMASK(1, 0)

/** AD4080_REG_TRANSFER_CONFIG Bit Definition */
#define AD4080_KEEP_STREAM_LEN_VAL_MSK		NO_OS_BIT(2)

/** AD4080_REG_INTERFACE_CONFIG_C Bit Definition */
#define AD4080_STRICT_REG_ACCESS_MSK		NO_OS_BIT(5)

/** AD4080_REG_DATA_INTF_CONFIG_A Bit Definition */
#define AD4080_INTF_CHK_EN_MSK			NO_OS_BIT(4)
#define AD4080_SPI_LVDS_LANES_MSK		NO_OS_BIT(2)
#define AD4080_DATA_INTF_MODE_MSK		NO_OS_BIT(0)

/** AD4080_REG_DATA_INTF_CONFIG_B Bit Definition */
#define AD4080_LVDS_CNV_CLK_CNT_MSK		NO_OS_GENMASK(7, 4)
#define AD4080_LVDS_SELF_CLK_MODE_MSK		NO_OS_BIT(3)
#define AD4080_LVDS_CNV_EN			NO_OS_BIT(0)

/** AD4080_REG_DATA_INTF_CONFIG_C Bit Definition */
#define AD4080_LVDS_VOD_MSK			NO_OS_GENMASK(6, 4)

/** AD4080_REG_PWR_CTRL Bit Definition */
#define AD4080_ANA_DIG_LDO_PD_MSK		NO_OS_BIT(1)
#define AD4080_INTF_LDO_PD_MSK			NO_OS_BIT(0)

/** AD4080_GPIO_CONFIG Bit Defintion  */
#define AD4080_GPIO_EN_MSK(x)			NO_OS_BIT(x)
#define AD4080_GPIO_SEL_MSK(x)			(NO_OS_GENMASK(3, 0) << (4 * ((x)%2)))
#define AD4080_GPIO_DATA_MSK(x)			(NO_OS_BIT(x) << 4)

/** AD4080_REG_GENERAL_CONFIG Bit Definition */
#define AD4080_FIFO_MODE_MSK			NO_OS_GENMASK(1, 0)

/** Miscellaneous Definitions */
#define AD4080_SW_RESET				NO_OS_BIT(7) | NO_OS_BIT(0)
#define AD4080_SPI_READ          		NO_OS_BIT(7)
#define BYTE_ADDR_H				NO_OS_GENMASK(15, 8)
#define BYTE_ADDR_L				NO_OS_GENMASK(7, 0)
#define AD4080_CHIP_ID				NO_OS_GENMASK(2, 0)
#define AD4080_FIFO_SIZE		  NO_OS_BIT(14)

/******************************************************************************/
/************************ Types Declarations **********************************/
/******************************************************************************/

/** AD4080 Sequential Addressing Behavior */
enum ad4080_addr_asc {
	AD4080_ADDR_DECR,
	AD4080_ADDR_INCR,
};

/** AD4080 Single/Streaming Mode */
enum ad4080_single_instr {
	AD4080_STREAM_MODE,
	AD4080_SINGLE_INST,
};

/** AD4080 Short Instruction */
enum ad4080_short_instr {
	AD4080_15_BIT_ADDR,
	AD4080_7_BIT_ADDR,
};

/** AD4080 Operation Mode */
enum ad4080_op_mode {
	AD4080_OP_NORMAL,
	AD4080_OP_STANDBY,
	AD4080_OP_LOW_POWER,
};

/** AD4080 Strict Register Access */
enum ad4080_strict_reg_access {
	AD4080_REG_NORMAL_MODE,
	AD4080_REG_STRICT_MODE,
};

/** AD4080 Output Pattern */
enum ad4080_intf_chk_en {
	AD4080_DATA,
	AD4080_FIXED_PATTERN,
};

/** AD4080 LVDS/SPI Lane Control. */
enum ad4080_cnv_spi_lvds_lanes {
	AD4080_ONE_LANE,
	AD4080_MULTIPLE_LANES
};

/** AD4080 Data Interface Configuration */
enum ad4080_conv_data_spi_lvds {
	AD4080_CONV_DATA_LVDS,
	AD4080_CONV_DATA_SPI
};

/** AD4080 LVDS Self Clock Mode */
enum ad4080_lvds_self_clk_mode {
	AD4080_ECHO_CLK_MODE,
	AD4080_SELF_CLK_MODE,
};

/** AD4080 LVDS CNV Clock Mode */
enum ad4080_lvds_cnv_clk_mode {
	AD4080_CNV_CMOS_MODE,
	AD4080_CNV_LVDS_MODE,
};

/** AD4080 LVDS Differential Output Voltage */
enum ad4080_lvds_vod {
	AD4080_185mVPP = 1,
	AD4080_240mVPP = 2,
	AD4080_325mVPP = 4,
};

/** AD4080 Analog/Digital LDO */
enum ad4080_ana_dig_ldo_pd {
	AD4080_AD_LDO_EN,
	AD4080_AD_LDO_DISABLE,
};

/** AD4080 Interface LDO */
enum ad4080_intf_ldo_pd {
	AD4080_INTF_LDO_EN,
	AD4080_INTF_LDO_DISABLE,
};

/* AD4080 GPIOs */
enum ad4080_gpio {
	AD4080_GPIO_0,
	AD4080_GPIO_1,
	AD4080_GPIO_2,
	AD4080_GPIO_3,
	NUM_AD4080_GPIO
};

/* AD4080 GPIO Output Enable Selection */
enum ad4080_gpio_op_enable {
	AD4080_GPIO_INPUT,
	AD4080_GPIO_OUTPUT
};

/** AD4080 GPIO Function Select */
enum ad4080_gpio_op_func_sel {
	AD4080_GPIO_ADI_NSPI_SDO_DATA,
	AD4080_GPIO_FIFO_FULL,
	AD4080_GPIO_FIFO_READ_DONE,
	AD4080_GPIO_FILTER_RESULT_READY,
	AD4080_GPIO_HT_DETECT,
	AD4080_GPIO_LT_DETECT,
	AD4080_GPIO_STATUS_ALERT,
	AD4080_GPIO_GPO_DATA,
	AD4080_GPIO_FILTER_SYNC_INPUT,
	AD4080_GPIO_EXT_EVENT_TRIGGER_FIFO,
	AD4080_GPIO_CNV_INHIBIT_INPUT
};

/** AD4080 Conversion Data FIFO Mode */
enum ad4080_fifo_mode {
	AD4080_FIFO_DISABLE,
	AD4080_IMMEDIATE_TRIGGER,
	AD4080_EVENT_TRIGGER_LAST_WM,
	AD4080_EVENT_TRIGGER,
};

/**
 * @struct ad4080_dev
 * @brief ad4080 Device structure.
 */
struct ad4080_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
	/* SPI 3-Wire Connection */
	bool spi3wire;
	/** Address Ascension */
	enum ad4080_addr_asc addr_asc;
	/** Single/Streaming Mode */
	enum ad4080_single_instr single_instr;
	/** Short Instruction */
	enum ad4080_short_instr	short_instr;
	/** Operation Mode */
	enum ad4080_op_mode op_mode;
	/** Strict Register Access */
	enum ad4080_strict_reg_access strict_reg;
	/** AD4080 Output Pattern */
	enum ad4080_intf_chk_en intf_chk_en;
	/** AD4080 LVDS/SPI Lane Control. */
	enum ad4080_cnv_spi_lvds_lanes cnv_spi_lvds_lanes;
	/** AD4080 Data Interface Configuration */
	enum ad4080_conv_data_spi_lvds conv_data_spi_lvds;
	/** AD4080 Interface clock periods from CNV rising edge */
	uint8_t lvds_cnv_clk_cnt;
	/** AD4080 LVDS Self Clock Mode */
	enum ad4080_lvds_self_clk_mode lvds_self_clk_mode;
	/** AD4080 LVDS CNV Clock Mode */
	enum ad4080_lvds_cnv_clk_mode cnv_clk_mode;
	/** AD4080 LVDS Differential Output Voltage */
	enum ad4080_lvds_vod lvds_vod;
	/** AD4080 Analog/Digital LDO */
	enum ad4080_ana_dig_ldo_pd ana_dig_ldo_pd;
	/** AD4080 Interface LDO */
	enum ad4080_intf_ldo_pd intf_ldo_pd;
	/** AD4080 Conversion Data FIFO Mode */
	enum ad4080_fifo_mode fifo_mode;
	/** AD4080 GPIO Output Enable state */
	enum ad4080_gpio_op_enable gpio_op_enable[NUM_AD4080_GPIO];
	/** AD4080 GPIO Output Function Selection */
	enum ad4080_gpio_op_func_sel gpio_op_func_sel[NUM_AD4080_GPIO];
};

/**
 * @struct ad4080_init_param
 * @brief ad4080 Device initialization parameters.
 */
struct ad4080_init_param {
	/* SPI */
	struct no_os_spi_init_param	*spi_init;
	/* SPI 3-Wire Connection */
	bool spi3wire;
	/** Address Ascension */
	enum ad4080_addr_asc addr_asc;
	/** Single/Streaming Mode */
	enum ad4080_single_instr single_instr;
	/** Short Instruction */
	enum ad4080_short_instr	short_instr;
	/** Operation Mode */
	enum ad4080_op_mode op_mode;
	/** Strict Register Access */
	enum ad4080_strict_reg_access strict_reg;
	/** AD4080 Output Pattern */
	enum ad4080_intf_chk_en intf_chk_en;
	/** AD4080 LVDS/SPI Lane Control. */
	enum ad4080_cnv_spi_lvds_lanes cnv_spi_lvds_lanes;
	/** AD4080 Data Interface Configuration */
	enum ad4080_conv_data_spi_lvds conv_data_spi_lvds;
	/** AD4080 Interface clock periods from CNV rising edge */
	uint8_t lvds_cnv_clk_cnt;
	/** AD4080 LVDS Self Clock Mode */
	enum ad4080_lvds_self_clk_mode lvds_self_clk_mode;
	/** AD4080 LVDS CNV Clock Mode */
	enum ad4080_lvds_cnv_clk_mode cnv_clk_mode;
	/** AD4080 LVDS Differential Output Voltage */
	enum ad4080_lvds_vod lvds_vod;
	/** AD4080 Analog/Digital LDO */
	enum ad4080_ana_dig_ldo_pd ana_dig_ldo_pd;
	/** AD4080 Interface LDO */
	enum ad4080_intf_ldo_pd intf_ldo_pd;
	/** AD4080 Conversion Data FIFO Mode */
	enum ad4080_fifo_mode fifo_mode;
	/** AD4080 GPIO Output Enable state */
	enum ad4080_gpio_op_enable gpio_op_enable[NUM_AD4080_GPIO];
	/** AD4080 GPIO Output Function Selection */
	enum ad4080_gpio_op_func_sel gpio_op_func_sel[NUM_AD4080_GPIO];
};
/** Writes data into a register.  */
int ad4080_write(struct ad4080_dev *dev, uint16_t reg_addr, uint8_t reg_val);

/** Reads data from a register. */
int ad4080_read(struct ad4080_dev *dev, uint16_t reg_addr, uint8_t *reg_val);

/** Update specific register bits */
int ad4080_update_bits(struct ad4080_dev *dev, uint16_t reg_addr, uint8_t mask,
		       uint8_t reg_val);

/** Software Reset. */
int ad4080_soft_reset(struct ad4080_dev *dev);

/** Set Address Ascension */
int ad4080_set_addr_asc(struct ad4080_dev *dev, enum ad4080_addr_asc addr_asc);

/** Get Address Ascension */
int ad4080_get_addr_asc(struct ad4080_dev *dev, enum ad4080_addr_asc *addr_asc);

/** Set Single/Streaming Mode */
int ad4080_set_single_instr(struct ad4080_dev *dev,
			    enum ad4080_single_instr single_instr);

/** Get Single/Streaming Mode */
int ad4080_get_single_instr(struct ad4080_dev *dev,
			    enum ad4080_single_instr *single_instr);

/** Set Short Instruction */
int ad4080_set_short_instr(struct ad4080_dev *dev,
			   enum ad4080_short_instr short_instr);

/** Get Short Instruction */
int ad4080_get_short_instr(struct ad4080_dev *dev,
			   enum ad4080_short_instr *short_instr);

/** Set Operation Mode */
int ad4080_set_op_mode(struct ad4080_dev *dev,
		       enum ad4080_op_mode op_mode);

/** Get Operation Mode */
int ad4080_get_op_mode(struct ad4080_dev *dev,
		       enum ad4080_op_mode *op_mode);

/** Set Strict Register Access */
int ad4080_set_strict_reg_access(struct ad4080_dev *dev,
				 enum ad4080_strict_reg_access strict_reg);

/** Get Strict Register Access */
int ad4080_get_strict_reg_access(struct ad4080_dev *dev,
				 enum ad4080_strict_reg_access *strict_reg);

/** Set AD4080 Output Pattern */
int ad4080_set_intf_chk_en(struct ad4080_dev *dev,
			   enum ad4080_intf_chk_en intf_chk_en);

/** Get AD4080 Output Pattern */
int ad4080_get_intf_chk_en(struct ad4080_dev *dev,
			   enum ad4080_intf_chk_en *intf_chk_en);

/** Set AD4080 LVDS/SPI Lane Control */
int ad4080_set_cnv_spi_lvds_lanes(struct ad4080_dev *dev,
				  enum ad4080_cnv_spi_lvds_lanes cnv_spi_lvds_lanes);

/** Get AD4080 LVDS/SPI Lane Control */
int ad4080_get_cnv_spi_lvds_lanes(struct ad4080_dev *dev,
				  enum ad4080_cnv_spi_lvds_lanes *cnv_spi_lvds_lanes);

/** Set AD4080 Data Interface Configuration */
int ad4080_set_conv_data_spi_lvds(struct ad4080_dev *dev,
				  enum ad4080_conv_data_spi_lvds conv_data_spi_lvds);

/** Get AD4080 Data Interface Configuration */
int ad4080_get_conv_data_spi_lvds(struct ad4080_dev *dev,
				  enum ad4080_conv_data_spi_lvds *conv_data_spi_lvds);

/** Set AD4080 Interface clock periods from CNV rising edge*/
int ad4080_set_lvds_cnv_clk_cnt(struct ad4080_dev *dev,
				uint8_t lvds_cnv_clk_cnt);

/** Get AD4080 Interface clock periods from CNV rising edge */
int ad4080_get_lvds_cnv_clk_cnt(struct ad4080_dev *dev,
				uint8_t *lvds_cnv_clk_cnt);

/** Set AD4080 LVDS Self Clock Mode */
int ad4080_set_lvds_self_clk_mode(struct ad4080_dev *dev,
				  enum ad4080_lvds_self_clk_mode lvds_self_clk_mode);

/** Get AD4080 LVDS Self Clock Mode */
int ad4080_get_lvds_self_clk_mode(struct ad4080_dev *dev,
				  enum ad4080_lvds_self_clk_mode *lvds_self_clk_mode);


/** Set AD4080 LVDS CNV Clock Mode */
int ad4080_set_lvds_cnv_clk_mode(struct ad4080_dev *dev,
				 enum ad4080_lvds_cnv_clk_mode cnv_clk_mode);

/** Get AD4080 LVDS CNV Clock Mode */
int ad4080_get_lvds_cnv_clk_mode(struct ad4080_dev *dev,
				 enum ad4080_lvds_cnv_clk_mode *cnv_clk_mode);

/** Set AD4080 LVDS Differential Output Voltage */
int ad4080_set_lvds_vod(struct ad4080_dev *dev,
			enum ad4080_lvds_vod lvds_vod);

/** Get AD4080 LVDS Differential Output Voltage */
int ad4080_get_lvds_vod(struct ad4080_dev *dev, enum ad4080_lvds_vod *lvds_vod);

/** Set AD4080 Analog/Digital LDO */
int ad4080_set_ana_dig_ldo_pd(struct ad4080_dev *dev,
			      enum ad4080_ana_dig_ldo_pd ana_dig_ldo_pd);

/** Get AD4080 Analog/Digital LDO */
int ad4080_get_ana_dig_ldo_pd(struct ad4080_dev *dev,
			      enum ad4080_ana_dig_ldo_pd *ana_dig_ldo_pd);

/** Set AD4080 Interface LDO */
int ad4080_set_intf_ldo_pd(struct ad4080_dev *dev,
			   enum ad4080_intf_ldo_pd intf_ldo_pd);

/** Get AD4080 Interface LDO */
int ad4080_get_intf_ldo_pd(struct ad4080_dev *dev,
			   enum ad4080_intf_ldo_pd *intf_ldo_pd);

/** Set AD4080 Conversion Data FIFO Mode */
int ad4080_set_fifo_mode(struct ad4080_dev *dev,
			 enum ad4080_fifo_mode fifo_mode);

/** Get AD4080 Conversion Data FIFO Mode */
int ad4080_get_fifo_mode(struct ad4080_dev *dev,
			 enum ad4080_fifo_mode *fifo_mode);

/** Set AD4080 FIFO Watermark */
int ad4080_set_fifo_watermark(struct ad4080_dev *dev,
			      uint16_t fifo_watermark);

/** Get AD4080 FIFO Watermark */
int ad4080_get_fifo_watermark(struct ad4080_dev *dev,
			      uint16_t *fifo_watermark);

/** Configure GPIO as input or output */
int ad4080_set_gpio_output_enable(struct ad4080_dev *dev,
				  enum ad4080_gpio gpio,
				  enum ad4080_gpio_op_enable gpio_op_enable);

/** Configure the GPIO for a specific function */
int ad4080_set_gpio_output_func(struct ad4080_dev *dev,
				enum ad4080_gpio gpio,
				enum ad4080_gpio_op_func_sel gpio_func);

/** Set the GPIO data */
int ad4080_gpio_write_data(struct ad4080_dev *dev,
			   enum ad4080_gpio gpio,
			   bool data);

/** Read the GPIO data */
int ad4080_gpio_read_data(struct ad4080_dev *dev,
			  enum ad4080_gpio gpio,
			  bool *data);

/** Configure the config SPI interface during initialization */
int ad4080_configuration_intf_init(struct ad4080_dev *device,
				   struct ad4080_init_param init_param);

/** Configure the data interface during initialization */
int ad4080_data_intf_init(struct ad4080_dev *device,
			  struct ad4080_init_param init_param);

/** Initialize the device. */
int ad4080_init(struct ad4080_dev **device,
		struct ad4080_init_param init_param);

/** Remove the device and release resources. */
int ad4080_remove(struct ad4080_dev *dev);

#endif /* __AD4080_H__ */
