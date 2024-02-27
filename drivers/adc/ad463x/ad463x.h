/***************************************************************************//**
 *   @file   ad463x.h
 *   @brief  Header file of AD463x Driver.
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
#ifndef AD463X_H_
#define AD463x_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "spi_engine.h"
#include "no_os_util.h"
#include "clk_axi_clkgen.h"
#include "no_os_pwm.h"
#include "no_os_gpio.h"

/******************************************************************************/
/********************** Macros and Types Declarations *************************/
/******************************************************************************/

/* Register addresses */
#define AD463X_REG_INTERFACE_CONFIG_A	0x00
#define AD463X_REG_INTERFACE_CONFIG_B	0x01
#define AD463X_REG_DEVICE_CONFIG	0x02
#define AD463X_REG_CHIP_TYPE		0x03
#define AD463X_REG_PRODUCT_ID_L		0x04
#define AD463X_REG_PRODUCT_ID_H		0x05
#define AD463X_REG_CHIP_GRADE		0x06
#define AD463X_REG_SCRATCH_PAD		0x0A
#define AD463X_REG_SPI_REVISION		0x0B
#define AD463X_REG_VENDOR_L		0x0C
#define AD463X_REG_VENDOR_H		0x0D
#define AD463X_REG_STREAM_MODE		0x0E
#define AD463X_REG_EXIT_CFG_MODE	0x14
#define AD463X_REG_AVG			0x15
#define AD463X_REG_OFFSET_BASE		0x16
#define AD463X_REG_OFFSET_X0_0		0x16
#define AD463X_REG_OFFSET_X0_1		0x17
#define AD463X_REG_OFFSET_X0_2		0x18
#define AD463X_REG_OFFSET_X1_0		0x19
#define AD463X_REG_OFFSET_X1_1		0x1A
#define AD463X_REG_OFFSET_X1_2		0x1B
#define AD463X_REG_GAIN_BASE		0x1C
#define AD463X_REG_GAIN_X0_LSB		0x1C
#define AD463X_REG_GAIN_X0_MSB		0x1D
#define AD463X_REG_GAIN_X1_LSB		0x1E
#define AD463X_REG_GAIN_X1_MSB		0x1F
#define AD463X_REG_MODES		0x20
#define AD463X_REG_OSCILATOR		0x21
#define AD463X_REG_IO			0x22
#define AD463X_REG_PAT0			0x23
#define AD463X_REG_PAT1			0x24
#define AD463X_REG_PAT2			0x25
#define AD463X_REG_PAT3			0x26
#define AD463X_REG_DIG_DIAG		0x34
#define AD463X_REG_DIG_ERR		0x35
/* INTERFACE_CONFIG_A */
#define AD463X_CFG_SW_RESET		(NO_OS_BIT(7) | NO_OS_BIT(0))
#define AD463X_CFG_SDO_ENABLE		NO_OS_BIT(4)
/* MODES */
#define AD463X_SW_RESET_MSK		(NO_OS_BIT(7) | NO_OS_BIT(0))
#define AD463X_LANE_MODE_MSK		(NO_OS_BIT(7) | NO_OS_BIT(6))
#define AD463X_CLK_MODE_MSK		(NO_OS_BIT(5) | NO_OS_BIT(4))
#define AD463X_DDR_MODE_MSK		NO_OS_BIT(3)
#define AD463X_SDR_MODE			0x00
#define AD463X_DDR_MODE			NO_OS_BIT(3)
#define AD463X_OUT_DATA_MODE_MSK	(NO_OS_BIT(2) | NO_OS_BIT(1) | NO_OS_BIT(0))
#define AD463X_24_DIFF			0x00
#define AD463X_16_DIFF_8_COM		0x01
#define AD463X_24_DIFF_8_COM		0x02
#define AD463X_30_AVERAGED_DIFF		0x03
#define AD463X_32_PATTERN		0x04
/* EXIT_CFG_MD */
#define AD463X_EXIT_CFG_MODE		NO_OS_BIT(0)
/* CHANNEL */
#define AD463X_CHANNEL_0		0x00
#define AD463X_CHANNEL_1		0x01
/* OFFSET */
#define AD463X_OFFSET_0			0x00
#define AD463X_OFFSET_1			0x01
#define AD463X_OFFSET_2			0x02
/* GAIN */
#define AD463X_GAIN_LSB			0x00
#define AD463X_GAIN_MSB			0x01
/* LANE MODE */
#define AD463X_ONE_LANE_PER_CH		0x00
#define AD463X_TWO_LANES_PER_CH		NO_OS_BIT(6)
#define AD463X_FOUR_LANES_PER_CH	NO_OS_BIT(7)
#define AD463X_SHARED_TWO_CH		(NO_OS_BIT(6) | NO_OS_BIT(7))
/* CLK MODE */
#define AD463X_SPI_COMPATIBLE_MODE	0x00
#define AD463X_ECHO_CLOCK_MODE		NO_OS_BIT(4)
#define AD463X_CLOCK_MASTER_MODE	NO_OS_BIT(5)
/* POWER MODE */
#define AD463X_NORMAL_MODE 		0x00
#define AD463X_LOW_POWER_MODE		(NO_OS_BIT(1) | NO_OS_BIT(0))
/* AVG */
#define AD463X_AVG_FILTER_RESET		NO_OS_BIT(7)
#define AD463X_CONFIG_TIMING		0x2000
#define AD463X_REG_READ_DUMMY		0x00
#define AD463X_REG_WRITE        	0x00
#define AD463X_REG_READ		    	NO_OS_BIT(7)
#define AD463X_REG_CHAN_OFFSET(ch, pos)	(AD463X_REG_OFFSET_BASE + (3 * ch) + pos)
#define AD463X_REG_CHAN_GAIN(ch, pos)	(AD463X_REG_GAIN_BASE + (2 * ch) + pos)
/* IO */
#define AD463X_DRIVER_STRENGTH_MASK	NO_OS_BIT(0)
#define AD463X_NORMAL_OUTPUT_STRENGTH	0x00
#define AD463X_DOUBLE_OUTPUT_STRENGTH	NO_OS_BIT(1)
/* OUT_DATA_PAT */
#define AD463X_OUT_DATA_PAT		0x5A5A0F0F

#define AD463X_TRIGGER_PULSE_WIDTH_NS	0x0A

#define AD463X_GAIN_MAX_VAL_SCALED	19997

/**
 * @enum ad463x_id
 * @brief Device type
 */
enum ad463x_id {
	/** AD4630-24 device */
	ID_AD4630_24,
	/** AD4630-20 device */
	ID_AD4630_20,
	/** AD4630-16 device */
	ID_AD4630_16,
	/** AD4631-24 device */
	ID_AD4631_24,
	/** AD4631-20 device */
	ID_AD4631_20,
	/** AD4631-16 device */
	ID_AD4631_16,
	/** AD4632-24 device */
	ID_AD4632_24,
	/** AD4632-20 device */
	ID_AD4632_20,
	/** AD4632-16 device */
	ID_AD4632_16,
	/** AD4030 device */
	ID_AD4030,
	/** ADAQ4224 device */
	ID_ADAQ4224,
};

/**
 * @enum ad463x_pgia_gain
 * @brief Available pgia gains.
 */
enum ad463x_pgia_gain {
	/** Vout/Vin = 0.33  */
	AD463X_GAIN_0_33 = 0,
	/** Vout/Vin = 0.56  */
	AD463X_GAIN_0_56 = 1,
	/** Vout/Vin = 2.22  */
	AD463X_GAIN_2_22 = 2,
	/** Vout/Vin = 6.67  */
	AD463X_GAIN_6_67 = 3,
};

/**
 * @struct ad463x_dev
 * @brief Device initialization parameters.
 */
struct ad463x_init_param {
	/** SPI */
	struct no_os_spi_init_param *spi_init;
	/** GPIO */
	struct no_os_gpio_init_param *gpio_resetn;
	struct no_os_gpio_init_param *gpio_pgia_a0;
	struct no_os_gpio_init_param *gpio_pgia_a1;
	/** PWM */
	struct no_os_pwm_init_param *trigger_pwm_init;
	/** SPI module offload init */
	struct spi_engine_offload_init_param *offload_init_param;
	/** Clock gen for hdl design init structure */
	struct axi_clkgen_init *clkgen_init;
	/** Clock generator rate */
	uint32_t axi_clkgen_rate;
	/** Register access speed */
	uint32_t reg_access_speed;
	/** Device id */
	enum ad463x_id device_id;
	/** Register data width */
	uint8_t reg_data_width;
	/** Lane Mode */
	uint8_t lane_mode;
	/** Clock Mode */
	uint8_t clock_mode;
	/** Data Rate Mode */
	uint8_t data_rate;
	/** Reference voltage */
	int32_t vref;
	/** Output Mode */
	uint8_t output_mode;
	/** Invalidate the Data cache for the given address range */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
};

/**
 * @struct ad463x_dev
 * @brief Device structure.
 */
struct ad463x_dev {
	/** SPI */
	struct no_os_spi_desc *spi_desc;
	/** GPIO */
	struct no_os_gpio_desc *gpio_resetn;
	struct no_os_gpio_desc *gpio_pgia_a0;
	struct no_os_gpio_desc *gpio_pgia_a1;
	/** PWM */
	struct no_os_pwm_desc *trigger_pwm_desc;
	/** SPI module offload init */
	struct spi_engine_offload_init_param *offload_init_param;
	/** Clock gen for hdl design structure */
	struct axi_clkgen *clkgen;
	/** Register access speed */
	uint32_t reg_access_speed;
	/** Device id */
	enum ad463x_id device_id;
	/** Register data width */
	uint8_t reg_data_width;
	/** Number of bytes to be read */
	uint8_t read_bytes_no;
	/** Capture data width */
	uint8_t capture_data_width;
	/** Lane Mode */
	uint8_t lane_mode;
	/** Clock Mode */
	uint8_t clock_mode;
	/** Data Rate Mode */
	uint8_t data_rate;
	/** Reference voltage */
	int32_t vref;
	/** pgia index */
	uint8_t pgia_idx;
	/** available scales */
	int32_t scale_table[4][2];
	/** Output Mode */
	uint8_t output_mode;
	/** ADC precision in bits */
	uint8_t real_bits_precision;
	/** pgia availability */
	bool has_pgia;
	/** Invalidate the Data cache for the given address range */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/** Read device register. */
int32_t ad463x_spi_reg_read(struct ad463x_dev *dev,
			    uint16_t reg_addr,
			    uint8_t *reg_data);

/** Write device register */
int32_t ad463x_spi_reg_write(struct ad463x_dev *dev,
			     uint16_t reg_addr,
			     uint8_t reg_data);

/** Read from device using a mask */
int32_t ad463x_spi_reg_read_masked(struct ad463x_dev *dev,
				   uint16_t reg_addr,
				   uint8_t mask,
				   uint8_t *data);

/** Write to device using a mask */
int32_t ad463x_spi_reg_write_masked(struct ad463x_dev *dev,
				    uint16_t reg_addr,
				    uint8_t mask,
				    uint8_t data);

/** Set power mode */
int32_t ad463x_set_pwr_mode(struct ad463x_dev *dev, uint8_t mode);

/** Set average frame length */
int32_t ad463x_set_avg_frame_len(struct ad463x_dev *dev, uint8_t mode);

/** Set drive strength */
int32_t ad463x_set_drive_strength(struct ad463x_dev *dev, uint8_t mode);

/** Exit register configuration mode */
int32_t ad463x_exit_reg_cfg_mode(struct ad463x_dev *dev);

/** Set channel gain */
int32_t ad463x_set_ch_gain(struct ad463x_dev *dev, uint8_t ch_idx,
			   uint64_t gain);

/** Set channel offset */
int32_t ad463x_set_ch_offset(struct ad463x_dev *dev, uint8_t ch_idx,
			     uint32_t offset);

/** Read data */
int32_t ad463x_read_data(struct ad463x_dev *dev,
			 uint32_t *buf,
			 uint16_t samples);

/** Device initialization */
int32_t ad463x_init(struct ad463x_dev **device,
		    struct ad463x_init_param *init_param);

/** Calculate PGIA gain */
int32_t ad463x_calc_pgia_gain(int32_t gain_int, int32_t gain_fract,
			      int32_t vref,
			      int32_t precision,
			      enum ad463x_pgia_gain *gain_idx);

/** Control PGIA gain */
int32_t ad463x_set_pgia_gain(struct ad463x_dev *dev,
			     enum ad463x_pgia_gain gain_idx);

/** Free resources */
int32_t ad463x_remove(struct ad463x_dev *dev);


/** Enter configuration Register mode */
int32_t ad463x_enter_config_mode(struct ad463x_dev *dev);

#endif // AD463X_H_
