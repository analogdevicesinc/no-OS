/***************************************************************************//**
 *   @file   AD9467.h
 *   @brief  Header file of AD9467 Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
#ifndef __AD9467_H__
#define __AD9467_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_spi.h"

/******************************************************************************/
/*********************************** AD9467 ***********************************/
/******************************************************************************/
#define AD9467_READ                         (1 << 15)
#define AD9467_WRITE                        (0 << 15)
#define AD9467_CNT(x)                       (((x) - 1) << 13)
#define AD9467_ADDR(x)                      ((x) & 0X1FFF)

/* Chip configuration registers */
#define AD9467_REG_CHIP_PORT_CFG            0x00
#define AD9467_REG_CHIP_ID                  0x01
#define AD9467_REG_CHIP_GRADE               0x02

/* Channel index and transfer registers */
#define AD9467_REG_DEVICE_UPDATE            0xFF

/* ADC functions registers */
#define AD9467_REG_MODES                    0x08
#define AD9467_REG_TEST_IO                  0x0D
#define AD9467_REG_ADC_INPUT                0x0F
#define AD9467_REG_OFFSET                   0x10
#define AD9467_REG_OUT_MODE                 0x14
#define AD9467_REG_OUT_ADJ                  0x15
#define AD9467_REG_OUT_PHASE                0x16
#define AD9467_REG_OUT_DELAY                0x17
#define AD9467_REG_V_REF                    0x18
#define AD9467_REG_ANALOG_INPUT             0x2C
#define AD9467_REG_BUFF_CURRENT_1           0x36
#define AD9467_REG_BUFF_CURRENT_2           0x107

/* AD9467_REG_CHIP_PORT_CFG */
#define AD9467_CHIP_PORT_CGF_LSB_FIRST      (1 << 6)
#define AD9467_CHIP_PORT_CGF_SOFT_RST       (1 << 5)

/* AD9467_REG_CHIP_GRADE */
#define AD9467_CHIP_GRADE_BITS(x)           (((x) & 0x3) << 4)

/* AD9467_REG_DEVICE_UPDATE */
#define AD9467_DEVICE_UPDATE_SW             (1 << 0)

/* AD9467_REG_MODES */
#define AD9467_MODES_INT_PD_MODE(x)         (((x) & 0x3) << 0)

/* AD9467_REG_TEST_IO */
#define AD9467_TEST_IO_RST_PN_LONG          (1 << 5)
#define AD9467_TEST_IO_RST_PN_SHORT         (1 << 4)
#define AD9467_TEST_IO_OUT_TEST(x)          (((x) & 0xF) << 0)

/* AD9467_REG_ADC_INPUT */
#define AD9467_ADC_INPUT_XVREF              (1 << 7)
#define AD9467_ADC_INPUT_ANALOG_DSCN        (1 << 2)

/* AD9467_REG_OUT_MODE */
#define AD9467_OUT_MODE_DOUT_DISBL          (1 << 4)
#define AD9467_OUT_MODE_OUT_INV             (1 << 2)
#define AD9467_OUT_MODE_DATA_FORMAT(x)      (((x) & 0x3) << 0)

/* AD9467_REG_OUT_ADJ */
#define AD9467_OUT_ADJ_LVDS                 (1 << 3)
#define AD9467_OUT_ADJ_OUT_CURRENT(x)       (((x) & 0x7) << 0)

/* AD9467_REG_OUT_PHASE */
#define AD9467_OUT_PHASE_DCO_INV            (1 << 7)

/* AD9467_REG_OUT_DELAY */
#define AD9467_OUT_DELAY_DCO_DLY_EN         (1 << 7)
#define AD9467_OUT_DELAY_OUT_DLY(x)         (((x) & 0x1F) << 0)

/* AD9467_REG_V_REF */
#define AD9467_V_REF_IN_FS_RANGE(x)         (((x) & 0xF) << 0)

/* AD9467_REG_ANALOG_INPUT */
#define AD9467_ANALOG_INPUT_COUPLING        (1 << 2)

/* AD9467_REG_BUFF_CURRENT_1 */
#define AD9467_BUFF_CURRENT_1(x)            (((x) & 0x3F) << 2)

/* AD9467_REG_BUFF_CURRENT_2 */
#define AD9467_BUFF_CURRENT_2(x)            (((x) & 0x3F) << 2)

/******************************************************************************/
/************************ Types Definitions ***********************************/
/******************************************************************************/
struct ad9467_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
};

struct ad9467_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/*! Configures the test mode and the output mode to a default state. */
int32_t ad9467_setup(struct ad9467_dev **device,
		     struct ad9467_init_param init_param);
/*! Free the resources allocated by ad9467_setup(). */
int32_t ad9467_remove(struct ad9467_dev *dev);
/*! Writes data into a register.  */
int32_t ad9467_write(struct ad9467_dev *dev,
		     uint16_t reg_addr,
		     uint8_t reg_val);
/*! Reads data from a register. */
int32_t ad9467_read(struct ad9467_dev *dev,
		    uint16_t reg_addr,
		    uint8_t *reg_val);
/*! Sets a bit or a group of bits inside a register without modifying the other
    bits. */
uint32_t ad9467_set_bits_to_reg(struct ad9467_dev *dev,
				uint16_t register_address,
				uint8_t bits_value,
				uint8_t mask);
/*! Configures the power mode. */
int32_t ad9467_pwr_mode(struct ad9467_dev *dev,
			int32_t mode,
			int32_t *ret_mode);
/*! Sets the ADC's test mode. */
int32_t ad9467_test_mode(struct ad9467_dev *dev,
			 int32_t mode,
			 int32_t *ret_mode);
/*! Sets (1) or clears (0) the reset short PN sequence bit(PN9). */
int32_t ad9467_reset_pn9(struct ad9467_dev *dev,
			 int32_t rst,
			 int32_t *ret_stat);
/*! Sets (1) or clears (0) the reset long PN sequence bit(PN23). */
int32_t ad9467_reset_pn23(struct ad9467_dev *dev,
			  int32_t rst,
			  int32_t *ret_stat);
/*!  Enables (1) or disables (0) the external voltage reference. */
int32_t ad9467_external_ref(struct ad9467_dev *dev,
			    int32_t en,
			    int32_t *ret_stat);
/*! Disconnects (1) or connects (0) the analog input from or to the ADC
    channel. */
int32_t ad9467_analog_input_disconnect(struct ad9467_dev *dev,
				       int32_t en,
				       int32_t *ret_stat);
/*! Sets the offset adjustment. */
int32_t ad9467_offset_adj(struct ad9467_dev *dev,
			  int32_t adj,
			  int32_t *ret_stat);
/*! Disables (1) or enables (0) the data output. */
int32_t ad9467_output_disable(struct ad9467_dev *dev,
			      int32_t en,
			      int32_t *ret_stat);
/*! Activates the inverted (1) or normal (0) output mode. */
int32_t ad9467_output_invert(struct ad9467_dev *dev,
			     int32_t invert,
			     int32_t *ret_stat);
/*! Specifies the output format. */
int32_t ad9467_output_format(struct ad9467_dev *dev,
			     int32_t format,
			     int32_t *ret_stat);
/*! Determines LVDS output properties. */
int32_t ad9467_coarse_lvds_adj(struct ad9467_dev *dev,
			       int32_t lvds_adj,
			       int32_t *ret_stat);
/*! Sets the output current adjustment. */
int32_t ad9467_output_current_adj(struct ad9467_dev *dev,
				  int32_t adj,
				  int32_t *ret_stat);
/*! Activates the normal (0) or inverted (1) DCO clock. */
int32_t ad9467_dco_clock_invert(struct ad9467_dev *dev,
				int32_t invert,
				int32_t *ret_stat);
/*! Configures the clock delay setting. */
int32_t ad9467_dco_output_clock_delay(struct ad9467_dev *dev,
				      int32_t delay,
				      int32_t *ret_stat);
/*! Configures the full-scale input voltage selection. */
int32_t ad9467_full_scale_range(struct ad9467_dev *dev,
				float v_fs,
				float *ret_stat);
/*! Sets the AC coupling(0) or DC coupling(1) mode. */
int32_t ad9467_analog_input_coupling(struct ad9467_dev *dev,
				     int32_t coupling_mode,
				     int32_t *ret_stat);
/*! Changes the input buffer current(1). */
int32_t ad9467_buffer_current_1(struct ad9467_dev *dev,
				int32_t percentage,
				int32_t *ret_stat);
/*! Changes the input buffer current(2). */
int32_t ad9467_buffer_current_2(struct ad9467_dev *dev,
				int32_t percentage,
				int32_t *ret_stat);
/*! Initiates a transfer and waits for the operation to end. */
int32_t ad9467_transfer(struct ad9467_dev *dev);

#endif /* __AD9467_H__ */
