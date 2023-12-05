/***************************************************************************//**
 *   @file   ad405x.h
 *   @brief  Header file of AD405X Driver.
 *   @author Jorge Marques (jorge.marques@analog.com)
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
#ifndef __AD405X_H__
#define __AD405X_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_i3c.h"
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/* SPI commands */
#define AD405X_SPI_READ          0x01
#define AD405X_SPI_WRITE         0x00
#define AD405X_ADDR(x)			  ((x) & 0xFF)
#define GET_AD405X_TRANSF_LEN(x) (((x) >>  8) & 0x0000FF)
#define SET_AD405X_TRANSF_LEN(x) (((x) <<  8) & 0x00FF00)
#define GET_AD405X_RESET_VAL(x)  (((x) >> 16) & 0x0000FF)
#define SET_AD405X_RESET_VAL(x)  (((x) << 16) & 0xFF0000)

/* AD405X Register Map */
#define AD405X_DEVID_AD     (AD405X_ADDR(0x00) | SET_AD405X_TRANSF_LEN(1) | SET_AD405X_RESET_VAL(0xAD))
#define AD405X_DEVID_MST 	 (AD405X_ADDR(0x01) | SET_AD405X_TRANSF_LEN(1) | SET_AD405X_RESET_VAL(0x1D))
#define AD4055_PARTID       (AD405X_ADDR(0x02) | SET_AD405X_TRANSF_LEN(1) | SET_AD405X_RESET_VAL(0xED))
#define AD405X_REVID 		 (AD405X_ADDR(0x03) | SET_AD405X_TRANSF_LEN(1) | SET_AD405X_RESET_VAL(0x01))
#define AD405X_STATUS       (AD405X_ADDR(0x04) | SET_AD405X_TRANSF_LEN(1) | SET_AD405X_RESET_VAL(0x00))
#define AD405X_DATA        (AD405X_ADDR(0x08) | SET_AD405X_TRANSF_LEN(3))
#define AD405X_ACT_EN       (AD405X_ADDR(0x24) | SET_AD405X_TRANSF_LEN(1) | SET_AD405X_RESET_VAL(0x00))
#define AD405X_SYNC         (AD405X_ADDR(0x2B) | SET_AD405X_TRANSF_LEN(1) | SET_AD405X_RESET_VAL(0x00))
#define AD405X_POWER_CTL    (AD405X_ADDR(0x2D) | SET_AD405X_TRANSF_LEN(1) | SET_AD405X_RESET_VAL(0x01))
#define AD405X_SELF_TEST    (AD405X_ADDR(0x2E) | SET_AD405X_TRANSF_LEN(1) | SET_AD405X_RESET_VAL(0x00))
#define AD405X_RESET        (AD405X_ADDR(0x2F) | SET_AD405X_TRANSF_LEN(1) | SET_AD405X_RESET_VAL(0x00))

#define AD405X_RESET_CODE            0x52

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum ad405x_type {
	ID_AD4055,
};

enum ad405x_comm_type {
	AD405X_SPI_COMM,
	AD405X_I3C_COMM
};

union ad405x_comm_init_param {
	/** I3C Initialization structure. */
	struct no_os_i3c_init_param i3c_init;
	/** SPI Initialization structure. */
	struct no_os_spi_init_param spi_init;
} ;

/**
 * @struct ad405x_init_param
 * @brief Structure holding the parameters for AD405X device initialization.
 */
struct ad405x_init_param {
	/** Device Communication initialization structure: either SPI or I3C */
	union ad405x_comm_init_param comm_init;
	/** Device Communication type: AD405X_SPI_COMM, AD405X_I3C_COMM */
	enum ad405x_comm_type comm_type;
	/** Device type: AD405X or 359 */
	enum ad405x_type dev_type;
};

struct _ad405x_int_mask {
	uint8_t RDY_EN1 : 1;
	uint8_t FULL_EN1 : 1;
	uint8_t OVR_EN1 : 1;
	uint8_t ACT_EN1 : 1;
	uint8_t RDY_EN2 : 1;
	uint8_t FULL_EN2 : 1;
	uint8_t OVR_EN2 : 1;
	uint8_t ACT_EN2 : 1;
};

union ad405x_int_mask {
	struct _ad405x_int_mask fields;
	uint8_t value;
};

struct _ad405x_sts_reg_flags {
	uint8_t DATA_RDY : 1;
	uint8_t FIFO_FULL : 1;
	uint8_t FIFO_OVR : 1;
	uint8_t Activity : 1;
	uint8_t NVM_BUSY : 1;
	uint8_t reserved : 3;
};

union ad405x_sts_reg_flags {
	struct _ad405x_sts_reg_flags fields;
	uint8_t value;
};

struct _ad405x_act_en_flags {
	uint8_t ACT      : 1;
	uint8_t reserved : 4;
};

union ad405x_act_en_flags {
	struct _ad405x_act_en_flags fields;
	uint8_t value;
};


union ad405x_comm_desc {
	/** I3C Descriptor */
	struct no_os_i3c_slave_desc *i3c_desc;
	/** SPI Descriptor */
	struct no_os_spi_desc *spi_desc;
};

/**
 * @struct ad405x_dev
 * @brief AD405X Device structure.
 */
struct ad405x_dev {
	/** Device type */
	enum ad405x_type dev_type;
	/** Device communication descriptor */
	union ad405x_comm_desc com_desc;
	/** Device Communication type: AD405X_SPI_COMM, AD405X_I3C_COMM */
	enum ad405x_comm_type comm_type;
	union ad405x_act_en_flags act_en;
	uint8_t comm_buff[289];
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/*! Init. the comm. peripheral and checks if the AD405X part is present. */
int ad405x_init(struct ad405x_dev **device,
		 struct ad405x_init_param init_param);

/*! Free the resources allocated by ad405x_init(). */
int ad405x_remove(struct ad405x_dev *dev);

/*! Performs soft reset of the device. */
int ad405x_soft_reset(struct ad405x_dev *dev);

/*! Reads the raw output data of each axis. */
int ad405x_get_raw(struct ad405x_dev *dev, uint32_t *raw);

/*! Reads the status register value. */
int ad405x_get_sts_reg(struct ad405x_dev *dev,
			union ad405x_sts_reg_flags *status_flags);

#endif /* __AD405X_H__ */
