/***************************************************************************//**
 *   @file   no_os_spi.h
 *   @brief  Header file of SPI Interface
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#ifndef _NO_OS_SPI_H_
#define _NO_OS_SPI_H_

#include <stdint.h>

#define	NO_OS_SPI_CPHA	0x01
#define	NO_OS_SPI_CPOL	0x02
#define SPI_MAX_BUS_NUMBER 8

/**
 * @enum no_os_spi_mode
 * @brief SPI configuration for clock phase and polarity.
 */
enum no_os_spi_mode {
	/** Data on rising, shift out on falling */
	NO_OS_SPI_MODE_0 = (0 | 0),
	/** Data on falling, shift out on rising */
	NO_OS_SPI_MODE_1 = (0 | NO_OS_SPI_CPHA),
	/** Data on rising, shift out on falling */
	NO_OS_SPI_MODE_2 = (NO_OS_SPI_CPOL | 0),
	/** Data on falling, shift out on rising */
	NO_OS_SPI_MODE_3 = (NO_OS_SPI_CPOL | NO_OS_SPI_CPHA)
};

/**
 * @enum no_os_spi_bit_order
 * @brief SPI configuration for bit order (MSB/LSB).
 */
enum no_os_spi_bit_order {
	/** Most-significant bit (MSB) first */
	NO_OS_SPI_BIT_ORDER_MSB_FIRST = 0,
	/** Least-significant bit (LSB) first */
	NO_OS_SPI_BIT_ORDER_LSB_FIRST = 1,
};

/**
 * @enum no_os_spi_lanes
 * @brief SPI configuration for number of lanes.
 */
enum no_os_spi_lanes {
	/** Single Lane */
	NO_OS_SPI_SINGLE_LANE,
	/** Dual Lane */
	NO_OS_SPI_DUAL_LANE,
	/** Quad Lane */
	NO_OS_SPI_QUAD_LANE,
	/** Octo Lane */
	NO_OS_SPI_OCTO_LANE,
};

/**
 * @struct no_os_spi_msg_list
 * @brief List item describing a SPI transfer
 */
struct no_os_spi_msg {
	/** Buffer with data to send. If NULL, 0x00 will be sent */
	uint8_t			*tx_buff;
	/** Buffer where to store data. If NULL, incoming data won't be saved */
	uint8_t			*rx_buff;
	/** Length of buffers. Must have equal size. */
	uint32_t		bytes_number;
	/** If set, CS will be deasserted after the transfer */
	uint8_t			cs_change;
	/**
	 * Minimum delay (in us) between the CS de-assert event of the current message
	 * and the assert of the next one.
	 */
	uint32_t		cs_change_delay;
	/** Delay (in us) between the CS assert and the first SCLK edge. */
	uint32_t		cs_delay_first;
	/** Delay (in us) between the last SCLK edge and the CS deassert */
	uint32_t		cs_delay_last;
};

/**
 * @struct no_os_platform_spi_delays
 * @brief Delays resulted from components in the SPI signal path. The values is ns.
 */
struct no_os_platform_spi_delays {
	uint32_t cs_delay_first;
	uint32_t cs_delay_last;
};

/**
 * @struct no_os_spi_platform_ops
 * @brief Structure holding SPI function pointers that point to the platform
 * specific function
 */
struct no_os_spi_platform_ops ;

/**
 * @struct no_os_spi_init_param
 * @brief Structure holding the parameters for SPI initialization
 */
struct no_os_spi_init_param {
	/** Device ID */
	uint32_t	device_id;
	/** maximum transfer speed */
	uint32_t	max_speed_hz;
	/** SPI chip select */
	uint8_t		chip_select;
	/** SPI mode */
	enum no_os_spi_mode	mode;
	/** SPI bit order */
	enum no_os_spi_bit_order	bit_order;
	/** SPI Lanes */
	enum no_os_spi_lanes   lanes;
	/** SPI bus platform ops */
	const struct no_os_spi_platform_ops *platform_ops;
	/** SPI delays */
	struct no_os_platform_spi_delays platform_delays;
	/**  SPI extra parameters (device specific) */
	void		*extra;
	/** Parent of the device */
	struct no_os_spi_desc *parent;
};

/**
 * @struct no_os_spibus_desc
 * @brief SPI bus descriptor
*/
struct no_os_spibus_desc {
	/** SPI bus mutex (lock) */
	void 		*mutex;
	/** SPI bus slave number*/
	uint8_t         slave_number;
	/** SPI bus device id */
	uint32_t	device_id;
	/** SPI bus max speed */
	uint32_t	max_speed_hz;
	/** SPI bus mode */
	enum no_os_spi_mode	mode;
	/** SPI bus bit order */
	enum no_os_spi_bit_order	bit_order;
	/** SPI Lanes */
	enum no_os_spi_lanes   lanes;
	/** SPI bus platform ops */
	const struct no_os_spi_platform_ops *platform_ops;
	/** SPI bus extra */
	void		*extra;
};

/**
 * @struct no_os_spi_desc
 * @brief Structure holding SPI descriptor.
 */
struct no_os_spi_desc {
	/** SPI bus address */
	struct no_os_spibus_desc	*bus;
	/** SPI bus number (0 for SPI0, 1 for SPI1, ...) */
	uint32_t	device_id;
	/** maximum transfer speed */
	uint32_t	max_speed_hz;
	/** SPI chip select */
	uint8_t		chip_select;
	/** SPI mode */
	enum no_os_spi_mode	mode;
	/** SPI bit order */
	enum no_os_spi_bit_order	bit_order;
	/** SPI Lanes */
	enum no_os_spi_lanes   lanes;
	/** SPI bus platform ops */
	const struct no_os_spi_platform_ops *platform_ops;
	/** SPI delays */
	struct no_os_platform_spi_delays platform_delays;
	/**  SPI extra parameters (device specific) */
	void		*extra;
	/** Parent of the device */
	struct no_os_spi_desc *parent;
};

/**
 * @struct no_os_spi_platform_ops
 * @brief Structure holding SPI function pointers that point to the platform
 * specific function
 */
struct no_os_spi_platform_ops {
	/** SPI initialization function pointer */
	int32_t (*init)(struct no_os_spi_desc **, const struct no_os_spi_init_param *);
	/** SPI write/read function pointer */
	int32_t (*write_and_read)(struct no_os_spi_desc *, uint8_t *, uint16_t);
	/** Iterate over the spi_msg array and send all messages at once */
	int32_t (*transfer)(struct no_os_spi_desc *, struct no_os_spi_msg *, uint32_t);
	/** Iterate over the spi_msg array and send all messages using DMA.
	 * Blocks until the transfer is completed.
	 */
	int32_t (*transfer_dma)(struct no_os_spi_desc *, struct no_os_spi_msg *,
				uint32_t);
	/** Iterate over the spi_msg array and send all messages using DMA.
	 * Returns immediately after the transfer is started and invokes a
	 * callback once all the messages have been transfered.
	 */
	int32_t (*transfer_dma_async)(struct no_os_spi_desc *, struct no_os_spi_msg *,
				      uint32_t, void (*)(void *), void *);
	/** SPI remove function pointer */
	int32_t (*remove)(struct no_os_spi_desc *);
	/** SPI abort function pointer */
	int32_t (*transfer_abort)(struct no_os_spi_desc *);
};

/* Initialize the SPI communication peripheral. */
int32_t no_os_spi_init(struct no_os_spi_desc **desc,
		       const struct no_os_spi_init_param *param);

/* Free the resources allocated by no_os_spi_init(). */
int32_t no_os_spi_remove(struct no_os_spi_desc *desc);

/* Write and read data to/from SPI. */
int32_t no_os_spi_write_and_read(struct no_os_spi_desc *desc,
				 uint8_t *data,
				 uint16_t bytes_number);

/* Iterate over the spi_msg array and send all messages at once */
int32_t no_os_spi_transfer(struct no_os_spi_desc *desc,
			   struct no_os_spi_msg *msgs,
			   uint32_t len);

/* Transfer a list of messages using DMA. Wait until all transfers are done */
int32_t no_os_spi_transfer_dma(struct no_os_spi_desc *desc,
			       struct no_os_spi_msg *msgs,
			       uint32_t len);
/*
 * Transfer a list of messages using DMA. Return once the first one started and
 * invoke a callback when they are done.
 */
int32_t no_os_spi_transfer_dma_async(struct no_os_spi_desc *desc,
				     struct no_os_spi_msg *msgs,
				     uint32_t len,
				     void (*callback)(void *),
				     void *ctx);

/* Abort SPI transfers. */
int32_t no_os_spi_transfer_abort(struct no_os_spi_desc *desc);

/* Initialize SPI bus descriptor*/
int32_t no_os_spibus_init(const struct no_os_spi_init_param *param);

/* Free the resources allocated for SPI bus desc*/
void no_os_spibus_remove(uint32_t bus_number);


#endif // _NO_OS_SPI_H_
