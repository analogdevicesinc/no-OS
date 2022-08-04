/**************************************************************************//**
*   @file   ad7280a.c
*   @brief  Driver's header for the AD7280A Lithium Ion Battery Monitoring System
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
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
*
******************************************************************************/

#ifndef _AD7280A_H_
#define _AD7280A_H_


/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/* GPIOs */
#define AD7280A_PD_OUT              no_os_gpio_direction_output(dev->gpio_pd,    \
			            NO_OS_GPIO_HIGH)
#define AD7280A_PD_HIGH             no_os_gpio_set_value(dev->gpio_pd,           \
			            NO_OS_GPIO_HIGH)
#define AD7280A_PD_LOW              no_os_gpio_set_value(dev->gpio_pd,           \
			            NO_OS_GPIO_LOW)
#define AD7280A_CNVST_OUT           no_os_gpio_direction_output(dev->gpio_cnvst, \
			            NO_OS_GPIO_HIGH)
#define AD7280A_CNVST_HIGH          no_os_gpio_set_value(dev->gpio_cnvst,        \
			            NO_OS_GPIO_HIGH)
#define AD7280A_CNVST_LOW           no_os_gpio_set_value(dev->gpio_cnvst,        \
			            NO_OS_GPIO_LOW)

#define AD7280A_ALERT_IN            no_os_gpio_direction_input(dev->gpio_alert)
#define AD7280_ALERT                (1 << 6)

/* Acquisition time */
#define AD7280A_ACQ_TIME_400ns      0
#define AD7280A_ACQ_TIME_800ns      1
#define AD7280A_ACQ_TIME_1200ns     2
#define AD7280A_ACQ_TIME_1600ns     3

/* Conversion averaging */
#define AD7280A_CONV_AVG_DIS        0
#define AD7280A_CONV_AVG_2          1
#define AD7280A_CONV_AVG_4          2
#define AD7280A_CONV_AVG_8          3

/* Alert register bits */
#define AD7280A_ALERT_REMOVE_VIN5       (1 << 2)
#define AD7280A_ALERT_REMOVE_VIN4_VIN5  (2 << 2)
#define AD7280A_ALERT_REMOVE_AUX5       (1 << 0)
#define AD7280A_ALERT_REMOVE_AUX4_AUX5  (2 << 0)

/* Registers */
#define AD7280A_CELL_VOLTAGE_1          0x0  /* D11 to D0, Read only */
#define AD7280A_CELL_VOLTAGE_2          0x1  /* D11 to D0, Read only */
#define AD7280A_CELL_VOLTAGE_3          0x2  /* D11 to D0, Read only */
#define AD7280A_CELL_VOLTAGE_4          0x3  /* D11 to D0, Read only */
#define AD7280A_CELL_VOLTAGE_5          0x4  /* D11 to D0, Read only */
#define AD7280A_CELL_VOLTAGE_6          0x5  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_1               0x6  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_2               0x7  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_3               0x8  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_4               0x9  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_5               0xA  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_6               0xB  /* D11 to D0, Read only */
#define AD7280A_SELF_TEST               0xC  /* D11 to D0, Read only */
#define AD7280A_CONTROL_HB              0xD  /* D15 to D8, Read/write */
#define AD7280A_CONTROL_LB              0xE  /* D7 to D0, Read/write */
#define AD7280A_CELL_OVERVOLTAGE        0xF  /* D7 to D0, Read/write */
#define AD7280A_CELL_UNDERVOLTAGE       0x10 /* D7 to D0, Read/write */
#define AD7280A_AUX_ADC_OVERVOLTAGE     0x11 /* D7 to D0, Read/write */
#define AD7280A_AUX_ADC_UNDERVOLTAGE    0x12 /* D7 to D0, Read/write */
#define AD7280A_ALERT                   0x13 /* D7 to D0, Read/write */
#define AD7280A_CELL_BALANCE            0x14 /* D7 to D0, Read/write */
#define AD7280A_CB1_TIMER               0x15 /* D7 to D0, Read/write */
#define AD7280A_CB2_TIMER               0x16 /* D7 to D0, Read/write */
#define AD7280A_CB3_TIMER               0x17 /* D7 to D0, Read/write */
#define AD7280A_CB4_TIMER               0x18 /* D7 to D0, Read/write */
#define AD7280A_CB5_TIMER               0x19 /* D7 to D0, Read/write */
#define AD7280A_CB6_TIMER               0x1A /* D7 to D0, Read/write */
#define AD7280A_PD_TIMER                0x1B /* D7 to D0, Read/write */
#define AD7280A_READ                    0x1C /* D7 to D0, Read/write */
#define AD7280A_CNVST_N_CONTROL         0x1D /* D7 to D0, Read/write */

/* Bits and Masks */
#define AD7280A_CTRL_HB_CONV_INPUT_ALL                  (0 << 6)
#define AD7280A_CTRL_HB_CONV_INPUT_6CELL_AUX1_3_4       (1 << 6)
#define AD7280A_CTRL_HB_CONV_INPUT_6CELL                (2 << 6)
#define AD7280A_CTRL_HB_CONV_INPUT_SELF_TEST            (3 << 6)
#define AD7280A_CTRL_HB_CONV_RES_READ_ALL               (0 << 4)
#define AD7280A_CTRL_HB_CONV_RES_READ_6CELL_AUX1_3_4    (1 << 4)
#define AD7280A_CTRL_HB_CONV_RES_READ_6CELL             (2 << 4)
#define AD7280A_CTRL_HB_CONV_RES_READ_NO                (3 << 4)
#define AD7280A_CTRL_HB_CONV_START_CNVST                (0 << 3)
#define AD7280A_CTRL_HB_CONV_START_CS                   (1 << 3)
#define AD7280A_CTRL_HB_CONV_AVG_DIS                    (0 << 1)
#define AD7280A_CTRL_HB_CONV_AVG_2                      (1 << 1)
#define AD7280A_CTRL_HB_CONV_AVG_4                      (2 << 1)
#define AD7280A_CTRL_HB_CONV_AVG_8                      (3 << 1)
#define AD7280A_CTRL_HB_CONV_AVG(x)                     ((x) << 1)
#define AD7280A_CTRL_HB_PWRDN_SW                        (1 << 0)

#define AD7280A_CTRL_LB_SWRST                           (1 << 7)
#define AD7280A_CTRL_LB_ACQ_TIME_400ns                  (0 << 5)
#define AD7280A_CTRL_LB_ACQ_TIME_800ns                  (1 << 5)
#define AD7280A_CTRL_LB_ACQ_TIME_1200ns                 (2 << 5)
#define AD7280A_CTRL_LB_ACQ_TIME_1600ns                 (3 << 5)
#define AD7280A_CTRL_LB_ACQ_TIME(x)                     ((x) << 5)
#define AD7280A_CTRL_LB_MUST_SET                        (1 << 4)
#define AD7280A_CTRL_LB_THERMISTOR_EN                   (1 << 3)
#define AD7280A_CTRL_LB_LOCK_DEV_ADDR                   (1 << 2)
#define AD7280A_CTRL_LB_INC_DEV_ADDR                    (1 << 1)
#define AD7280A_CTRL_LB_DAISY_CHAIN_RB_EN               (1 << 0)

#define AD7280A_ALERT_GEN_STATIC_HIGH                   (1 << 6)
#define AD7280A_ALERT_RELAY_SIG_CHAIN_DOWN              (3 << 6)

#define AD7280A_ALL_CELLS                               (0xAD << 16)

#define AD7280A_DEVADDR_MASTER                  0
#define AD7280A_DEVADDR_ALL                     0x1F

/* Value to be sent when readings are performed */
#define AD7280A_READ_TXVAL                      0xF800030A

#define NUMBITS_READ        22   // Number of bits for CRC when reading
#define NUMBITS_WRITE       21   // Number of bits for CRC when writing

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct ad7280a_dev {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_pd;
	struct no_os_gpio_desc	*gpio_cnvst;
	struct no_os_gpio_desc	*gpio_alert;
	/* Device Settings */
	uint32_t		read_data[24];
	float			cell_voltage[12];
	float			aux_adc[12];
};

struct ad7280a_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* GPIO */
	struct no_os_gpio_init_param	gpio_pd;
	struct no_os_gpio_init_param	gpio_cnvst;
	struct no_os_gpio_init_param	gpio_alert;
};

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/
/* Initializes the communication with the device. */
int8_t ad7280a_init(struct ad7280a_dev **device,
		    struct ad7280a_init_param init_param);

/* Free the resources allocated by AD7280A_Init(). */
int32_t ad7280a_remove(struct ad7280a_dev *dev);

/* Reads/transmits 32 data bits from/to AD7280A. */
uint32_t ad7280a_transfer_32bits(struct ad7280a_dev *dev,
				 uint32_t data);

/* Computes the CRC value for a write transmission, and prepares the complete
 write codeword */
uint32_t ad7280a_crc_write(uint32_t message);

/* Checks the received message if the received CRC and computed CRC are
the same. */
int32_t ad7280a_crc_read(uint32_t message);

/* Performs a read from all registers on 2 devices. */
int8_t ad7280a_convert_read_all(struct ad7280a_dev *dev);

/* Converts acquired data to float values. */
int8_t ad7280a_convert_data_all(struct ad7280a_dev *dev);

/* Reads the register content of one selected register. */
int16_t ad7280a_read_register(struct ad7280a_dev *dev,
			      uint8_t dev_addr,
			      uint8_t read_reg);

/* Reads the conversion of one channel. */
int16_t ad7280a_read_conversion(struct ad7280a_dev *dev,
				uint8_t dev_addr,
				uint8_t read_reg);

/* Converts the input data to a voltage value. */
float ad7280a_convert_data(uint8_t type,
			   uint16_t data);

/* Writes the content of one selected register from the selected device. */
void ad7280a_write_register(struct ad7280a_dev *dev,
			    uint8_t dev_addr,
			    uint8_t read_reg,
			    uint8_t reg_val);

/* Performs the self test for two devices(one master and one slave). */
void ad7280a_selftest_all(struct ad7280a_dev *dev,
			  float *self_test_reg_a,
			  float *self_test_reg_b);

/* Reads the value of Alert Pin from the device. */
uint8_t ad7280a_alert_pin(struct ad7280a_dev *dev);

#endif /*_AD7280A_H_*/
