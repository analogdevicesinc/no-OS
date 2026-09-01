/***************************************************************************//**
 *   @file   ad7490.h
 *   @brief  Header file of AD7490 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef AD7490_H_
#define AD7490_H_

#include <stdlib.h>
#include "no_os_spi.h"
#include "no_os_util.h"

#define AD7490_LOW_BYTE_MASK	NO_OS_GENMASK(7, 0)
#define AD7490_HIGH_BYTE_MASK	NO_OS_GENMASK(15, 8)

#define AD7490_VAL_MASK		NO_OS_GENMASK(11, 0)

#define AD7490_WRITE_MASK	NO_OS_BIT(11)
#define AD7490_SEQ_MASK		NO_OS_BIT(10)
#define AD7490_ADD_MASK		NO_OS_GENMASK(9, 6)
#define AD7490_PM_MASK		NO_OS_GENMASK(5, 4)
#define AD7490_SHADOW_MASK	NO_OS_BIT(3)
#define AD7490_WEAKTRI_MASK	NO_OS_BIT(2)
#define AD7490_RANGE_MASK	NO_OS_BIT(1)
#define AD7490_CODING_MASK	NO_OS_BIT(0)

#define AD7490_CFG_MASK		NO_OS_GENMASK(2, 0)

enum ad7490_vdd {
	AD7490_VDD_3V3,
	AD7490_VDD_5V,
};

enum ad7490_address {
	AD7490_CHAN_ADDR_VIN0,
	AD7490_CHAN_ADDR_VIN1,
	AD7490_CHAN_ADDR_VIN2,
	AD7490_CHAN_ADDR_VIN3,
	AD7490_CHAN_ADDR_VIN4,
	AD7490_CHAN_ADDR_VIN5,
	AD7490_CHAN_ADDR_VIN6,
	AD7490_CHAN_ADDR_VIN7,
	AD7490_CHAN_ADDR_VIN8,
	AD7490_CHAN_ADDR_VIN9,
	AD7490_CHAN_ADDR_VIN10,
	AD7490_CHAN_ADDR_VIN11,
	AD7490_CHAN_ADDR_VIN12,
	AD7490_CHAN_ADDR_VIN13,
	AD7490_CHAN_ADDR_VIN14,
	AD7490_CHAN_ADDR_VIN15,
	AD7490_CHAN_OFF,
};

enum ad7490_op_mode {
	AD7490_MODE_AUTOSTANDBY,
	AD7490_MODE_AUTOSHUTDOWN,
	AD7490_MODE_FULLSHUTDOWN,
	AD7490_MODE_NORMAL,
};

enum ad7490_dout_state {
	AD7490_DOUT_TRISTATE,
	AD7490_DOUT_WEAK,
};

enum ad7490_range {
	AD7490_RANGE_2XREFIN,
	AD7490_RANGE_REFIN,
};

enum ad7490_coding {
	AD7490_CODING_TWOS,
	AD7490_CODING_BINARY,
};

enum ad7490_seq_op {
	AD7490_NO_SEQ,
	AD7490_SHADOW_SEQ,
	AD7490_CONTROL_SEQ,
	AD7490_CONSECUTIVE_SEQ,
};

struct ad7490_init_param {
	struct no_os_spi_init_param *spi_param;
	enum ad7490_op_mode op_mode;
	enum ad7490_vdd vdd;
};

struct ad7490_desc {
	struct no_os_spi_desc *spi_desc;
	enum ad7490_address address;
	enum ad7490_op_mode op_mode;
	enum ad7490_seq_op seq_op;
	enum ad7490_vdd vdd;
	uint16_t channels;
	uint8_t nb_channels_seq;
	uint8_t cfg;
};

/* Set the operation mode of the AD7490. */
int ad7490_set_op_mode(struct ad7490_desc *desc, enum ad7490_op_mode op_mode);

/* Configuration of the range, coding and DOUT state. */
int ad7490_config(struct ad7490_desc *desc, enum ad7490_dout_state dout_state,
		  enum ad7490_range range, enum ad7490_coding coding);

/* Single channel reading function. */
int ad7490_read_ch(struct ad7490_desc *desc, enum ad7490_address channel,
		   int16_t *val);

/* Start SEQUENCER using desired type. */
int ad7490_start_seq(struct ad7490_desc *desc, enum ad7490_seq_op seq_op,
		     uint16_t channels, enum ad7490_address last_chan);

/* Change SEQUENCER type while it is running. */
int ad7490_change_seq(struct ad7490_desc *desc, enum ad7490_seq_op seq_op,
		      enum ad7490_coding coding, enum ad7490_range range);

/* Stop SEQUENCER. */
int ad7490_stop_seq(struct ad7490_desc *desc);

/* Read all selected channel values from the SEQUENCER. */
int ad7490_read_seq(struct ad7490_desc *desc, int16_t *channels_val);

/* Initialize the AD7490. */
int ad7490_init(struct ad7490_desc **desc,
		struct ad7490_init_param *init_param);

/* Remove allocated resources of the AD7490 by init function. */
int ad7490_remove(struct ad7490_desc *desc);

#endif /* AD7490_H_ */
