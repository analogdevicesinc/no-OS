/**************************************************************************//**
*   @file    AD717X.h
*   @brief   AD717X header file.
*   	     Devices: AD7172-2, AD7172-4, AD7173-8, AD7175-2, AD7175-8, AD7176-2,
*            AD7177-2, AD4111, AD4112, AD4114, AD4115, AD4116
*   @author  acozma (andrei.cozma@analog.com)
*            dnechita (dan.nechita@analog.com)
*******************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
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
******************************************************************************/

#ifndef __AD717X_H__
#define __AD717X_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_spi.h"
#include "no_os_util.h"
#include <stdbool.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/* Total Number of Setups in the AD717x-AD411x family */
#define AD717x_MAX_SETUPS			8
/* Maximum number of channels in the AD717x-AD411x family */
#define AD717x_MAX_CHANNELS			16

/*
 *@enum	ad717x_mode
 *@details ADC Modes of Operation
**/
enum ad717x_mode {
	CONTINUOUS, 			/* Continuous Mode- Default */
	SINGLE, 				/* Single Mode */
	STANDBY, 				/* Stand-by Mode */
	POWER_DOWN, 			/* Power Down Mode */
	INTERNAL_OFFSET_CALIB,	/* Internal Offset Calibration*/
	INTERNAL_GAIN_CALIB, 	/* Internal Gain Calibration */
	SYS_OFFSET_CALIB, 		/* System Offset Calibraion */
	SYS_GAIN_CALIB			/* System Gain Calibration */
};

/*
 *@enum	ad717x_reference_source
 *@details Type of ADC Reference
**/
enum ad717x_reference_source {
	EXTERNAL_REF = 0x0, /* External Reference REF+/-*/
	INTERNAL_REF = 0x2,	/* Internal 2.5V Reference */
	AVDD_AVSS = 0x3		/* AVDD - AVSS */
};

/*
 *@enum	ad717x_analog_input_pairs
 *@details Analog Input Pairs to channels for the AD411X Family
**/
enum ad717x_analog_input_pairs {
	VIN0_VIN1 = 0x1,
	VIN0_VINCOM = 0x10,
	VIN1_VIN0 = 0x20,
	VIN1_VINCOM = 0x30,
	VIN2_VIN3 = 0x43,
	VIN2_VINCOM = 0x50,
	VIN3_VIN2 = 0x62,
	VIN3_VINCOM = 0x70,
	VIN4_VIN5 = 0x85,
	VIN4_VINCOM = 0x90,
	VIN5_VIN4 = 0xA4,
	VIN5_VINCOM = 0xB0,
	VIN6_VIN7 = 0xC7,
	VIN6_VINCOM = 0xD0,
	VIN7_VIN6 = 0xE6,
	VIN7_VINCOM = 0xF0,
	IIN3P_IIN3M = 0x18B,
	IIN2P_IIN2M = 0x1AA,
	IIN1P_IIN1M = 0x1C9,
	IIN0P_IIN0M = 0x1E8,
	TEMPERATURE_SENSOR = 0x232,
	REFERENCE = 0x2B6
};

/*
 *@enum	ad717x_analog_input
 *@details Positive/Negative Analog Input to channels for the AD717x Family
**/
enum ad717x_analog_input {
	AIN0 = 0x0,
	AIN1 = 0x1,
	AIN2 = 0x2,
	AIN3 = 0x3,
	AIN4 = 0x4,
	TEMP_SENSOR_P = 0x11,
	TEMP_SENSOR_M = 0x12,
	AVDD_AVSS_P = 0x13,
	AVDD_AVSS_M = 0x14,
	REF_P = 0x15,
	REF_M = 0x16
};

/*
 *@union ad717x_analog_inputs
 *@details Types of Analog Inputs
**/
union ad717x_analog_inputs {
	enum ad717x_analog_input_pairs analog_input_pairs;
	struct {
		enum ad717x_analog_input pos_analog_input;
		enum ad717x_analog_input neg_analog_input;
	} ainp;
};

/*
 *@enum	ad717x_device_type
 *@details AD717x-AD411x Device definitions
**/
enum ad717x_device_type {
	ID_AD4111,
	ID_AD4112,
	ID_AD4114,
	ID_AD4115,
	ID_AD4116,
	ID_AD7172_2,
	ID_AD7172_4,
	ID_AD7173_8,
	ID_AD7175_2,
	ID_AD7175_8,
	ID_AD7176_2,
	ID_AD7177_2
};

/*
 *@struct ad717x_channel_setup
 *@details Channel setup
**/
struct ad717x_channel_setup {
	bool bi_unipolar;
	bool ref_buff;
	bool input_buff;
	enum ad717x_reference_source ref_source;
};

/*
 *@enum ad717x_enhfilt
 *@details Post filter for enhanced rejection
**/
enum ad717x_enhfilt {
	sps27_db47_ms36p7 = 0x2,
	sps25_db62_ms40 = 0x3,
	sps20_db86_ms50 = 0x5,
	sps16p6_db82_ms60 = 0x6
};

/*
 *@enum ad717x_order
 *@details Order of digital filter
**/
enum ad717x_order {
	sinc5_sinc1 = 0x0,
	sinc3 = 0x3
};

/*
 *@enum ad717x_odr
 *@details Output data rate
**/
enum ad717x_odr {
	sps_31250_a = 0x0,
	sps31250_b = 0x1,
	sps_31250_c = 0x2,
	sps_31250_d = 0x3,
	sps31250_e = 0x4,
	sps_31250_f = 0x5,
	sps_15625 = 0x6,
	sps_10417 = 0x7,
	sps_5208 = 0x8,
	sps_2957 = 0x9,
	sps_1007 = 0xA,
	sps_503 = 0xB,
	sps_381 = 0xC,
	sps_200 = 0xD,
	sps_100 = 0xE,
	sps_59 = 0xF,
	sps_49 = 0x10,
	sps_20 = 0x11,
	sps_16 = 0x12,
	sps_10 =0x13,
	sps_5 = 0x14,
	sps_2p5 = 0x15,
	sps_1p25 = 0x16
};

/*
 *@struct ad717x_filtcon
 *@details Filter configuration
**/
struct ad717x_filtcon {
	bool sinc3_map;
	bool enhfilten;
	enum ad717x_enhfilt enhfilt;
	enum ad717x_order oder;
	enum ad717x_odr odr;
};

/*
 *@struct ad717x_channel_map
 *@details Channel mapping
**/
struct ad717x_channel_map {
	bool channel_enable;
	uint8_t setup_sel;
	union ad717x_analog_inputs analog_inputs;
};

typedef enum {
	AD717X_DISABLE,
	AD717X_USE_CRC,
	AD717X_USE_XOR,
} ad717x_crc_mode;

/*! AD717X register info */
typedef struct {
	int32_t addr;
	int32_t value;
	int32_t size;
} ad717x_st_reg;

/*
 * The structure describes the device and is used with the ad717x driver.
 * @slave_select_id: The ID of the Slave Select to be passed to the SPI calls.
 * @regs: A reference to the register list of the device that the user must
 *       provide when calling the Setup() function.
 * @num_regs: The length of the register list.
 * @userCRC: Error check type to use on SPI transfers.
 */
typedef struct {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
	/* Device Settings */
	ad717x_st_reg		*regs;
	uint8_t			num_regs;
	ad717x_crc_mode		useCRC;
	/* Active Device */
	enum ad717x_device_type active_device;
	/* Reference enable */
	bool ref_en;
	/* Number of channels */
	uint8_t num_channels;
	/* Setups */
	struct ad717x_channel_setup setups[AD717x_MAX_SETUPS];
	/* Channel Mapping*/
	struct ad717x_channel_map chan_map[AD717x_MAX_CHANNELS];
	/* Gain */
	uint32_t pga[AD717x_MAX_SETUPS];
	/* Filter */
	struct ad717x_filtcon filter_configuration[AD717x_MAX_SETUPS];
	/* ADC Mode */
	enum ad717x_mode mode;
} ad717x_dev;

typedef struct {
	/* SPI */
	struct no_os_spi_init_param		spi_init;
	/* Device Settings */
	ad717x_st_reg		*regs;
	uint8_t			num_regs;
	/* Active Device */
	enum ad717x_device_type active_device;
	/* Reference Enable */
	bool ref_en;
	/* Number of Channels */
	uint8_t num_channels;
	/* Number of Setups */
	uint8_t num_setups;
	/* Channel Mapping */
	struct ad717x_channel_map chan_map[AD717x_MAX_CHANNELS];
	/* Setups */
	struct ad717x_channel_setup setups[AD717x_MAX_SETUPS];
	/* Gain */
	uint32_t pga[AD717x_MAX_SETUPS];
	/* Filter Configuration */
	struct ad717x_filtcon filter_configuration[AD717x_MAX_SETUPS];
	/* ADC Mode */
	enum ad717x_mode mode;
} ad717x_init_param;

/*****************************************************************************/
/***************** AD717X Register Definitions *******************************/
/*****************************************************************************/

/* AD717X Register Map */
#define AD717X_COMM_REG       0x00
#define AD717X_STATUS_REG     0x00
#define AD717X_ADCMODE_REG    0x01
#define AD717X_IFMODE_REG     0x02
#define AD717X_REGCHECK_REG   0x03
#define AD717X_DATA_REG       0x04
#define AD717X_GPIOCON_REG    0x06
#define AD717X_ID_REG         0x07
#define AD717X_CHMAP0_REG     0x10
#define AD717X_CHMAP1_REG     0x11
#define AD717X_CHMAP2_REG     0x12
#define AD717X_CHMAP3_REG     0x13
#define AD717X_CHMAP4_REG     0x14
#define AD717X_CHMAP5_REG     0x15
#define AD717X_CHMAP6_REG     0x16
#define AD717X_CHMAP7_REG     0x17
#define AD717X_CHMAP8_REG     0x18
#define AD717X_CHMAP9_REG     0x19
#define AD717X_CHMAP10_REG    0x1A
#define AD717X_CHMAP11_REG    0x1B
#define AD717X_CHMAP12_REG    0x1C
#define AD717X_CHMAP13_REG    0x1D
#define AD717X_CHMAP14_REG    0x1E
#define AD717X_CHMAP15_REG    0x1F
#define AD717X_SETUPCON0_REG  0x20
#define AD717X_SETUPCON1_REG  0x21
#define AD717X_SETUPCON2_REG  0x22
#define AD717X_SETUPCON3_REG  0x23
#define AD717X_SETUPCON4_REG  0x24
#define AD717X_SETUPCON5_REG  0x25
#define AD717X_SETUPCON6_REG  0x26
#define AD717X_SETUPCON7_REG  0x27
#define AD717X_FILTCON0_REG   0x28
#define AD717X_FILTCON1_REG   0x29
#define AD717X_FILTCON2_REG   0x2A
#define AD717X_FILTCON3_REG   0x2B
#define AD717X_FILTCON4_REG   0x2C
#define AD717X_FILTCON5_REG   0x2D
#define AD717X_FILTCON6_REG   0x2E
#define AD717X_FILTCON7_REG   0x2F
#define AD717X_OFFSET0_REG    0x30
#define AD717X_OFFSET1_REG    0x31
#define AD717X_OFFSET2_REG    0x32
#define AD717X_OFFSET3_REG    0x33
#define AD717X_OFFSET4_REG    0x34
#define AD717X_OFFSET5_REG    0x35
#define AD717X_OFFSET6_REG    0x36
#define AD717X_OFFSET7_REG    0x37
#define AD717X_GAIN0_REG      0x38
#define AD717X_GAIN1_REG      0x39
#define AD717X_GAIN2_REG      0x3A
#define AD717X_GAIN3_REG      0x3B
#define AD717X_GAIN4_REG      0x3C
#define AD717X_GAIN5_REG      0x3D
#define AD717X_GAIN6_REG      0x3E
#define AD717X_GAIN7_REG      0x3F

/* Communication Register bits */
#define AD717X_COMM_REG_WEN    (0 << 7)
#define AD717X_COMM_REG_WR     (0 << 6)
#define AD717X_COMM_REG_RD     (1 << 6)
#define AD717X_COMM_REG_RA(x)  ((x) & 0x3F)

/* Status Register bits */
#define AD717X_STATUS_REG_RDY      (1 << 7)
#define AD717X_STATUS_REG_ADC_ERR  (1 << 6)
#define AD717X_STATUS_REG_CRC_ERR  (1 << 5)
#define AD717X_STATUS_REG_REG_ERR  (1 << 4)
#define AD717X_STATUS_REG_CH(x)    ((x) & 0x0F)

/* ADC Mode Register bits */
#define AD717X_ADCMODE_REG_REF_EN     (1 << 15)
#define AD717X_ADCMODE_SING_CYC       (1 << 13)
#define AD717X_ADCMODE_REG_DELAY(x)   (((x) & 0x7) << 8)
#define AD717X_ADCMODE_REG_MODE(x)    (((x) & 0x7) << 4)
#define AD717X_ADCMODE_REG_CLKSEL(x)  (((x) & 0x3) << 2)

/* ADC Mode Register additional bits for AD7172-2, AD7172-4, AD4111 and AD4112 */
#define AD717X_ADCMODE_REG_HIDE_DELAY   (1 << 14)

/* Interface Mode Register bits */
#define AD717X_IFMODE_REG_ALT_SYNC      (1 << 12)
#define AD717X_IFMODE_REG_IOSTRENGTH    (1 << 11)
#define AD717X_IFMODE_REG_DOUT_RESET    (1 << 8)
#define AD717X_IFMODE_REG_CONT_READ     (1 << 7)
#define AD717X_IFMODE_REG_DATA_STAT     (1 << 6)
#define AD717X_IFMODE_REG_REG_CHECK     (1 << 5)
#define AD717X_IFMODE_REG_XOR_EN        (0x01 << 2)
#define AD717X_IFMODE_REG_CRC_EN        (0x02 << 2)
#define AD717X_IFMODE_REG_XOR_STAT(x)   (((x) & AD717X_IFMODE_REG_XOR_EN) == AD717X_IFMODE_REG_XOR_EN)
#define AD717X_IFMODE_REG_CRC_STAT(x)   (((x) & AD717X_IFMODE_REG_CRC_EN) == AD717X_IFMODE_REG_CRC_EN)
#define AD717X_IFMODE_REG_DATA_WL16     (1 << 0)

/* Interface Mode Register additional bits for AD717x family, not for AD411x */
#define AD717X_IFMODE_REG_HIDE_DELAY    (1 << 10)

/* GPIO Configuration Register bits */
#define AD717X_GPIOCON_REG_MUX_IO      (1 << 12)
#define AD717X_GPIOCON_REG_SYNC_EN     (1 << 11)
#define AD717X_GPIOCON_REG_ERR_EN(x)   (((x) & 0x3) << 9)
#define AD717X_GPIOCON_REG_ERR_DAT     (1 << 8)
#define AD717X_GPIOCON_REG_IP_EN1      (1 << 5)
#define AD717X_GPIOCON_REG_IP_EN0      (1 << 4)
#define AD717X_GPIOCON_REG_OP_EN1      (1 << 3)
#define AD717X_GPIOCON_REG_OP_EN0      (1 << 2)
#define AD717X_GPIOCON_REG_DATA1       (1 << 1)
#define AD717X_GPIOCON_REG_DATA0       (1 << 0)

/* GPIO Configuration Register additional bits for AD7172-4, AD7173-8 */
#define AD717X_GPIOCON_REG_GP_DATA3    (1 << 7)
#define AD717X_GPIOCON_REG_GP_DATA2    (1 << 6)
#define AD717X_GPIOCON_REG_GP_DATA1    (1 << 1)
#define AD717X_GPIOCON_REG_GP_DATA0    (1 << 0)

/* GPIO Configuration Register additional bits for AD7173-8 */
#define AD717X_GPIOCON_REG_PDSW        (1 << 14)
#define AD717X_GPIOCON_REG_OP_EN2_3    (1 << 13)

/* GPIO Configuration Register additional bits for AD4111, AD4112, AD4114, AD4115 */
#define AD4111_GPIOCON_REG_OP_EN0_1    (1 << 13)
#define AD4111_GPIOCON_REG_DATA1       (1 << 7)
#define AD4111_GPIOCON_REG_DATA0       (1 << 6)

/* GPIO Configuration Register additional bits for AD4116 */
#define AD4116_GPIOCON_REG_OP_EN2_3    (1 << 13)
#define AD4116_GPIOCON_REG_DATA3       (1 << 7)
#define AD4116_GPIOCON_REG_DATA2       (1 << 6)

/* GPIO Configuration Register additional bits for AD4111 */
#define AD4111_GPIOCON_REG_OW_EN       (1 << 12)

/* Channel Map Register 0-3 bits */
#define AD717X_CHMAP_REG_CH_EN         (1 << 15)
#define AD717X_CHMAP_REG_SETUP_SEL(x)  (((x) & 0x7) << 12)
#define AD717X_CHMAP_REG_AINPOS(x)     (((x) & 0x1F) << 5)
#define AD717X_CHMAP_REG_AINNEG(x)     (((x) & 0x1F) << 0)

/* Channel Map Register additional bits for AD4111, AD4112, AD4114, AD4115, AD4116 */
#define AD4111_CHMAP_REG_INPUT(x)      (((x) & 0x3FF) << 0)

/* Setup Configuration Register 0-3 bits */
#define AD717X_SETUP_CONF_REG_BI_UNIPOLAR  (1 << 12)
#define AD717X_SETUP_CONF_REG_REF_SEL(x)   (((x) & 0x3) << 4)

/* Setup Configuration Register additional bits for AD7173-8 */
#define AD717X_SETUP_CONF_REG_REF_BUF(x)  (((x) & 0x3) << 10)
#define AD717X_SETUP_CONF_REG_AIN_BUF(x)  (((x) & 0x3) << 8)
#define AD717X_SETUP_CONF_REG_BURNOUT_EN  (1 << 7)
#define AD717X_SETUP_CONF_REG_BUFCHOPMAX  (1 << 6)

/* Setup Configuration Register additional bits for AD7172-2, AD7172-4, AD7175-2 */
#define AD717X_SETUP_CONF_REG_REFBUF_P    (1 << 11)
#define AD717X_SETUP_CONF_REG_REFBUF_N    (1 << 10)
#define AD717X_SETUP_CONF_REG_AINBUF_P    (1 << 9)
#define AD717X_SETUP_CONF_REG_AINBUF_N    (1 << 8)

/* Setup Configuration Register additional bits for AD4111, AD4112, AD4114, AD4115, AD4116 */
#define AD4111_SETUP_CONF_REG_REFPOS_BUF   (1 << 11)
#define AD4111_SETUP_CONF_REG_REFNEG_BUF   (1 << 10)
#define AD4111_SETUP_CONF_REG_AIN_BUF(x)   (((x) & 0x3) << 8)
#define AD4111_SETUP_CONF_REG_BUFCHOPMAX   (1 << 6)

/* Filter Configuration Register 0-3 bits */
#define AD717X_FILT_CONF_REG_SINC3_MAP    (1 << 15)
#define AD717X_FILT_CONF_REG_ENHFILTEN    (1 << 11)
#define AD717X_FILT_CONF_REG_ENHFILT(x)   (((x) & 0x7) << 8)
#define AD717X_FILT_CONF_REG_ORDER(x)     (((x) & 0x3) << 5)
#define AD717X_FILT_CONF_REG_ODR(x)       (((x) & 0x1F) << 0)

/* ID register mask for relevant bits */
#define AD717X_ID_REG_MASK	  0xFFF0
/* AD7172-2 ID */
#define AD7172_2_ID_REG_VALUE 0x00D0
/* AD7172-4 ID */
#define AD7172_4_ID_REG_VALUE 0x2050
/* AD7173-8 ID */
#define AD7173_8_ID_REG_VALUE 0x30D0
/* AD7175-2 ID */
#define AD7175_2_ID_REG_VALUE 0x0CD0
/* AD7175-8 ID */
#define AD7175_8_ID_REG_VALUE 0x3CD0
/* AD7176-2 ID */
#define AD7176_2_ID_REG_VALUE 0x0C90
/* AD7177-2 ID */
#define AD7177_2_ID_REG_VALUE 0x4FD0
/* AD4111, AD4112 IDs */
#define AD411X_ID_REG_VALUE   0x30D0
/* AD4114, AD4115 IDs */
#define AD4114_5_ID_REG_VALUE   0x31D0
/* AD4116 ID */
#define AD4116_ID_REG_VALUE   0x34D0

/*****************************************************************************/
/******************* AD717X Constants ****************************************/
/*****************************************************************************/
#define AD717X_CRC8_POLYNOMIAL_REPRESENTATION 0x07 /* x8 + x2 + x + 1 */
/* Timeout for ADC Conversion */
#define AD717X_CONV_TIMEOUT			10000

#define AD717x_CHANNEL_INPUT_MASK			NO_OS_GENMASK(9,0)
#define AD717X_CHMAP_REG_SETUP_SEL_MSK  	NO_OS_GENMASK(14,12)
#define AD717X_CHMAP_REG_AINPOS_MSK    		NO_OS_GENMASK(9,5)
#define AD717X_CHMAP_REG_AINNEG_MSK    		NO_OS_GENMASK(4,0)
#define AD717X_ADCMODE_REG_MODE_MSK   		NO_OS_GENMASK(6,4)
#define AD717X_SETUP_CONF_REG_REF_SEL_MSK	NO_OS_GENMASK(5,4)
#define AD717x_ODR_MSK				NO_OS_GENMASK(4,0)

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/

/*! Retrieves a pointer to the register that matches the given address */
ad717x_st_reg *AD717X_GetReg(ad717x_dev *device,
			     uint8_t reg_address);

/*! Reads the value of the specified register. */
int32_t AD717X_ReadRegister(ad717x_dev *device,
			    uint8_t addr);

/*! Writes the value of the specified register. */
int32_t AD717X_WriteRegister(ad717x_dev *device,
			     uint8_t);

/*! Resets the device. */
int32_t AD717X_Reset(ad717x_dev *device);

/*! Waits until a new conversion result is available. */
int32_t AD717X_WaitForReady(ad717x_dev *device,
			    uint32_t timeout);

/*! Reads the conversion result from the device. */
int32_t AD717X_ReadData(ad717x_dev *device,
			int32_t* pData);

/*! Computes data register read size to account for bit number and status
 *  read. */
int32_t AD717X_ComputeDataregSize(ad717x_dev *device);

/*! Computes the CRC checksum for a data buffer. */
uint8_t AD717X_ComputeCRC8(uint8_t* pBuf,
			   uint8_t bufSize);

/*! Computes the XOR checksum for a data buffer. */
uint8_t AD717X_ComputeXOR8(uint8_t * pBuf,
			   uint8_t bufSize);

/*! Updates the CRC settings. */
int32_t AD717X_UpdateCRCSetting(ad717x_dev *device);

/*! Initializes the AD717X. */
int32_t AD717X_Init(ad717x_dev **device,
		    ad717x_init_param init_param);

/*! Free the resources allocated by AD717X_Init(). */
int32_t AD717X_remove(ad717x_dev *dev);

/* Enable/Disable Channels */
int ad717x_set_channel_status(ad717x_dev *device, uint8_t channel_id,
			      bool channel_status);
/* Set ADC Mode */
int ad717x_set_adc_mode(ad717x_dev *device, enum ad717x_mode mode);

/* Configure Analog inputs to channel */
int ad717x_connect_analog_input(ad717x_dev *device, uint8_t channel_id,
				union ad717x_analog_inputs analog_input);

/* Assign setup to channel */
int ad717x_assign_setup(ad717x_dev *device, uint8_t channel_id,
			uint8_t setup);

/* Assign polarity to setup*/
int ad717x_set_polarity(ad717x_dev* device, bool bipolar,
			uint8_t setup_id);

/* Assign reference source to setup */
int ad717x_set_reference_source(ad717x_dev* device,
				enum ad717x_reference_source ref_source, uint8_t setup_id);

/* Enable/Disable input and reference buffers to setup */
int ad717x_enable_buffers(ad717x_dev* device, bool inbuf_en,
			  bool refbuf_en, uint8_t setup_id);

/* Perform single conversion and read sample */
int ad717x_single_read(ad717x_dev* device, uint8_t id,
		       int32_t *adc_raw_data);

/* Configure device ODR */
int32_t ad717x_configure_device_odr(ad717x_dev *dev, uint8_t filtcon_id,
				    uint8_t odr_sel);

#endif /* __AD717X_H__ */
